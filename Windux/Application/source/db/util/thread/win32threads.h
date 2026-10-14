#ifndef WIN32THREADS_H
#define WIN32THREADS_H



#include "PAL_platformheader.h"



#ifndef NO_NAMESPACES
namespace util_thread
{
#endif



class Win32Threads
{
	public:
		// Types, subtypes
		typedef PCRITICAL_SECTION MutexType;  /* (SJJC port 6/19/06) */
		typedef HANDLE EventType;
		typedef DWORD ThreadIDType;
		typedef HANDLE ThreadHandle;
		typedef DWORD ThreadExitCodeType;


		// --- Processing methods ---
		// Thread methods
		static ThreadIDType currentThreadID() { return GetCurrentThreadId(); }

		// Mutex methods
		static MutexType createMutex()	/* (SJJC port 6/19/06) */
				{ MutexType mutex = new CRITICAL_SECTION(); InitializeCriticalSection(mutex); return mutex;}
		static void acquireMutex(MutexType* mutex) { EnterCriticalSection(*mutex); }  /* (SJJC port 6/19/06) */
		static void releaseMutex(MutexType* mutex) { LeaveCriticalSection(*mutex); }  /* (SJJC port 6/19/06) */
		static void deleleMutex(MutexType* mutex) { DeleteCriticalSection(*mutex); }  /* (SJJC port 6/19/06) */

		// Event methods
/*		static EventType invalidEvent() { return INVALID_HANDLE; }
		static EventType createEvent(bool signaled) { return CreateEvent(NULL, true, signaled, NULL); }
		static bool waitEvent(EventType* event, int millisecs = -1)
			{ return WaitForSingleObject(*event, (millisecs == -1) ? INFINITE : millisecs) != WAIT_FAILED; }
		static bool signalEvent(EventType* event) { SetEvent(*event); }
		static bool isSignaled(EventType* event) { return WaitForSingleObject(*event, 0) == WAIT_OBJECT_O; }
		static bool resetEvent(EventType* event) { ResetEvent(*event); }
		static bool deleteEvent(EventType* event) { CloseHandle(*event); } */
};



#ifndef NO_NAMESPACES
} // namespace util_thread
#endif



#endif // WIN32THREADS_H

