#pragma once

#include <thread>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace tuc
{
    
    class thread {
    public:
        template <typename Function, typename... Arguments>
        thread(Function function, Arguments... arguments)
            : t(function, arguments...)
        {}

        thread(tuc::thread&& thread)
            : t(std::move(thread.t))
        {}

        thread(std::thread&& thread)
            : t(std::move(thread))
        {}

        ~thread() {
            if (t.joinable()) {
                t.join();
            }
        }

        thread& operator=(thread&& that) {
            t = std::move(that.t);
        }

    private:
        std::thread t;
    };

    void set_current_thread_to_idle_priority()
    {
#ifdef WIN32
        SetThreadPriority(GetCurrentThread(), -15);
#else // WIN32
        int policy = 0;
        struct sched_param param = {};
        const int self = pthread_self();
        pthread_getschedparam(self, &policy, &param);
        param.sched_priority = sched_get_priority_min(policy);
        pthread_setschedparam(self, policy, &param);
#endif // WIN32
    }
}
