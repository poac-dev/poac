#ifndef POAC_UTIL_STEP_FUNCTIONS_HPP
#define POAC_UTIL_STEP_FUNCTIONS_HPP

#include <vector>
#include <tuple>
#include <future>
#include <functional>
#include <chrono>


namespace poac::util {
    class step_functions {
    private:
        const std::vector<std::function<void()>> funcs;
        const size_t size;
        std::future<void> mutable func_now;
        unsigned int mutable index = 0;

    public:
        template < typename ...Funcs >
        explicit step_functions(Funcs ...fs) : funcs({fs...}), size(sizeof...(Funcs)) {}
        explicit step_functions(std::vector<std::function<void()>>&& fs) : funcs(fs), size(fs.size()) {}

        void start() const { if (index == 0) run(); }
        void run()   const { func_now = std::async(std::launch::async, funcs[index]); }
        int  next()  const { ++index; run(); return index; }
        // All done: -1, Now: index
        template <class Rep, class Period>
        int wait_for(const std::chrono::duration<Rep, Period>& rel_time) const {
            if (std::future_status::ready == func_now.wait_for(rel_time)) {
                if (index < (size-1)) { return next(); }
                else                  { return -1; }
            }
            else { return index; }
        }
    };

    class step_funcs_with_status {
    private:
        // Notfound, Downloading, Cloning, Building, Copying, Error, Done
        using ret_type = bool;
        std::future<ret_type> func_now;
        unsigned int index = 0;
        std::string alldone;

    public:
        std::vector<std::tuple<std::string, std::function<ret_type()>>> funcs;
        std::string error_msg;
        std::string finish_msg;

        step_funcs_with_status() {}


        void start() { if (index == 0) run(); }
        void run()   { func_now = std::async(std::launch::async, std::get<1>(funcs[index])); }
        std::string next()  { ++index; run(); return std::get<0>(funcs[index]); }
        // All done: -1, Now: index
        template <class Rep, class Period>
        std::string wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
            if (!alldone.empty()) {
                return alldone;
            }
            else if (std::future_status::ready == func_now.wait_for(rel_time)) {
                if (func_now.get()) {
                    alldone = (error_msg.empty() ? "Error" : error_msg);
                    return alldone;
                }
                else if (index < (funcs.size()-1)) {
                    return next();
                }
                else {
                    alldone = (finish_msg.empty() ? "Done" : finish_msg);
                    return alldone;
                }
            }
            else { return std::get<0>(funcs[index]); }
        }
        bool is_done() {
            return !alldone.empty();
        }
    };

    template <typename T>
    T async_func(std::function<T()> func) {
        const auto func_now = std::async(std::launch::async, func);
        const auto ready =  std::future_status::ready;
        const auto rel_time = std::chrono::milliseconds(0);
        while (ready != func_now.wait_for(rel_time));
        return func_now.get();
    }

    template <typename T>
    T async_func(std::function<T()> func, std::function<void(int)> status) {
        auto func_now = std::async(std::launch::async, func);
        const auto ready =  std::future_status::ready;
        const auto rel_time = std::chrono::milliseconds(100);
        int count = 0;
        while (ready != func_now.wait_for(rel_time)) { status(count++); }
        return func_now.get();
    }
} // end namespace
#endif // !POAC_UTIL_STEP_FUNCTIONS_HPP
