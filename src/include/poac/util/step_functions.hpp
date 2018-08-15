#ifndef POAC_UTIL_STEP_FUNCTIONS_HPP
#define POAC_UTIL_STEP_FUNCTIONS_HPP

#include <vector>
#include <tuple>
#include <future>
#include <functional>
#include <chrono>


namespace poac::util {
    // TODO: 各async_funcs, return bool
    // TODO: error handling
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
        const std::vector<std::tuple<std::string, std::function<void()>>> funcs;
        const size_t size;
        std::future<void> mutable func_now;
        unsigned int mutable index = 0;

    public:
        template < typename ...Funcs >
        explicit step_funcs_with_status(Funcs ...fs)
            : funcs({fs...}), size(sizeof...(Funcs)) {}
        explicit step_funcs_with_status(std::vector<std::tuple<std::string, std::function<void()>>>&& fs)
            : funcs(fs), size(fs.size()) {}

        void start() const { if (index == 0) run(); }
        void run()   const { func_now = std::async(std::launch::async, std::get<1>(funcs[index])); }
        std::string next()  const { ++index; run(); return std::get<0>(funcs[index]); }
        // All done: -1, Now: index
        template <class Rep, class Period>
        std::string wait_for(const std::chrono::duration<Rep, Period>& rel_time) const {
            if (std::future_status::ready == func_now.wait_for(rel_time)) {
                if (index < (size-1)) { return next(); }
                else                  { return std::string("Done"); }
            }
            else { return std::get<0>(funcs[index]); }
        }
    };
} // end namespace
#endif // !POAC_UTIL_STEP_FUNCTIONS_HPP
