#ifndef MUTEX_H
#define MUTEX_H


#include <string>
#include <list>
#include <functional>



//#ifdef WIN32_OS
//#include "win32threads.h"
//#endif
//#ifdef VXWORKS
//#include "vxworkstasks.h"
//#endif
#if defined( WIN32 )
    #include    "win32threads.h"

#elif   defined( __linux__ )
    #include    "linuxthreads.h"

#elif   defined( SOLARIS )
    #error "Not support yet"

#elif   defined( VXWORKS )
    #include    "vxworkstasks.h"

#else
    #error "Unknown system"

#endif



#ifndef NO_NAMESPACES
namespace util_thread
{
#endif




#ifdef WIN32_OS
template <class ThreadingModel = Win32Threads>
#endif
#ifdef VXWORKS
template <class ThreadingModel = VxWorksTasks>
#endif
#ifdef  __linux__
template <class ThreadingModel = LinuxThreads>
#endif
class Mutex
{
	public:
		// Types, subtypes

		// --- Constructors and destructor ---
		Mutex() : mutex_(ThreadingModel::createMutex()), locks_(0)
		{
//			mutex_ = ThreadingModel::createMutex(&mutex_);
		}
		~Mutex()
		{
			while ( locks_ > 0 )
				release();
			ThreadingModel::deleleMutex(&mutex_);
		}


		// --- Accessor methods ---
		bool isLocked() { return locks_ > 0; }
		int getLocks() { return locks_; }


		// --- Processing methods ---
		void acquire() { ThreadingModel::acquireMutex(&mutex_); ++locks_; }
		void release() { if ( locks_ > 0 ) { --locks_; ThreadingModel::releaseMutex(&mutex_); } }


	private:
		typename ThreadingModel::MutexType mutex_;
		int locks_;
};



#ifndef NO_NAMESPACES
} // namespace util_thread
#endif



#endif // MUTEX_H

