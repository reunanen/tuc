#pragma once

namespace tuc
{ 
    namespace raii
    {
        template <typename T, typename ReleaseFunction>
        class wrapper {
        public:
            wrapper(T& wrappee, ReleaseFunction& release)
                : wrappee(wrappee)
                , release(release)
            {}

            ~wrapper() {
                release();
            }

            T wrappee;
            ReleaseFunction& release;
        };
    }
}
