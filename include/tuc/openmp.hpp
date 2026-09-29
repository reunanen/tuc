#pragma once

#include <memory> // scoped_ptr
#include <string>

#ifdef _OPENMP
#include <omp.h>
#include <algorithm> // std::clamp
#endif // _OPENMP

namespace tuc
{ 
    namespace openmp
    {
        template <typename Function>
        void maybe_parallelize_for_loop(Function function, int loops, bool parallelize)
        {
            if (parallelize) {
                std::exception_ptr error = nullptr;
#ifdef _OPENMP
                // A thread with no iteration to run would only wait at the barrier
                int const thread_count = std::clamp(loops, 1, omp_get_max_threads());
#pragma omp parallel for num_threads(thread_count)
#endif // _OPENMP
                for (int i = 0; i < loops; ++i) {
                    try {
                        function(i);
                    }
                    catch (std::exception&) {
#pragma omp critical
                        if (!error) {
                            error = std::current_exception();
                        }
                    }
                }
                if (error) {
                    std::rethrow_exception(error);
                }
            }
            else {
                for (int i = 0; i < loops; ++i) {
                    function(i);
                }
            }
        }

        template <typename Function>
        void parallelize_for_loop(Function function, int loops)
        {
            maybe_parallelize_for_loop(function, loops, true);
        }
    }
}
