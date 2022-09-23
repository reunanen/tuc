#pragma once

#include <thread>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#endif // WIN32

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
            return *this;
        }

    private:
        std::thread t;
    };

    void inline set_current_thread_to_idle_priority()
    {
#ifdef WIN32
        SetThreadPriority(GetCurrentThread(), -15);
#else // WIN32
        struct sched_param param = {};
        param.sched_priority = 0;
        pthread_setschedparam(pthread_self(), SCHED_IDLE, &param);
#endif // WIN32
    }
}
