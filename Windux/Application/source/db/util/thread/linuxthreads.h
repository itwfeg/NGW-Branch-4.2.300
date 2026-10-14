//
// File : linuxthreads.h
//
//

#ifndef _LINUX_THREADS_H_
#define _LINUX_THREADS_H_

#include "PAL_platformheader.h"
#include <pthread.h>


#ifndef NO_NAMESPACES
namespace util_thread
{
#endif


class LinuxThreads
{
    public:
        // Types, subtypes
        typedef pthread_mutex_t         MutexType;
        typedef pthread_mutexattr_t     MutexAttrType;
        typedef pthread_t               ThreadIDType;
        typedef HANDLE                  ThreadHandle;
        typedef void                    *ThreadExitCodeType;


        // --- Processing methods ---
        // Thread methods
        static ThreadIDType currentThreadID()       { return pthread_self(); }

        // Mutex methods
        static MutexType createMutex()
        {
        	MutexAttrType _attr_;
            MutexType _mutex_;

            pthread_mutexattr_init(&_attr_);
            pthread_mutexattr_settype(&_attr_, PTHREAD_MUTEX_RECURSIVE);

            pthread_mutex_init(&_mutex_, &_attr_);

            return _mutex_;
        }
        static void acquireMutex(MutexType* mutex)  { pthread_mutex_lock(mutex);    }
        static void releaseMutex(MutexType* mutex)  { pthread_mutex_unlock(mutex);  }
        static void deleleMutex(MutexType* mutex)   { pthread_mutex_destroy(mutex); }
};


#ifndef NO_NAMESPACES
} // namespace util_thread
#endif


#endif  // _LINUX_THREADS_H_
