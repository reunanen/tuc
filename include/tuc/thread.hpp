#pragma once

#include <thread>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace tuc
{ 
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
