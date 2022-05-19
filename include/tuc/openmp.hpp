#pragma once

#include <memory> // scoped_ptr
#include <string>

namespace tuc
{ 
    namespace openmp
    {
        template <typename Function>
        void maybe_parallelize_for_loop(Function function, int loops, bool parallelize)
        {
            if (parallelize) {
                std::exception_ptr error = nullptr;
#pragma omp parallel for
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
