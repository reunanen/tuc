#pragma once

namespace tuc
{ 
    namespace openmp
    {
        template <typename Function>
        void maybe_parallelize_for_loop(Function function, int loops, bool parallelize)
        {
            if (parallelize) {
                // TODO: add some sort of exception handling
#pragma omp parallel for
                for (int i = 0; i < loops; ++i) {
                    function(i);
                }
            }
            else {
                for (int i = 0; i < loops; ++i) {
                    function(i);
                }
            }
        }
    }
}
