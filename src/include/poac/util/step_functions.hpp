#ifndef POAC_UTIL_STEP_FUNCTIONS_HPP
#define POAC_UTIL_STEP_FUNCTIONS_HPP

#include <vector>
#include <future>
#include <functional>
#include <chrono>


namespace poac::util {
    // TODO: 各async_funcs, return bool
    // TODO: error handling
    // TODO: const std::string now = [&status]() {
    //                    if (status == 0)      return "Downloading";
    //                    else if (status == 1) return "Building";
    //                    else if (status == 2) return "Copying";
    //                    else                  return "Error";
    //                }();
    //                rewrite(std::bind(&progress, *index_now, now, src), info_func);
    //  この辺りのstatus funcを持ちたい？
    struct step_functions {
        const std::vector<std::function<void()>> funcs;
        const size_t size;
        std::future<void> mutable func_now;
        unsigned int mutable index = 0;

        template < typename ...Funcs >
        explicit step_functions(Funcs ...fs) : funcs({fs...}), size(sizeof...(Funcs)) {}

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
} // end namespace
#endif // !POAC_UTIL_STEP_FUNCTIONS_HPP
