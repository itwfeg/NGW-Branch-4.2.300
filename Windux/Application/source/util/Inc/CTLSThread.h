// Header file included by ANTLR UniLexer


#ifdef PAL_WIN32_RUN
#include <tchar.h>
#endif

#ifndef _CTLS_THREAD_HEADER_
#define _CTLS_THREAD_HEADER_

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:                   Base class

PURPOSE:                Designed to provide an interface to objects that are
to run as separate objects.


FUNCTION:               see above


INTERFACES:             "See 'INTERFACES:' in the module declaration below"


RESOURCES:

REFERENCES:


PROCESSING:             Construct the class and then pass the instance of it to
the CTLSThread class that will be running it.

The CTLSThread class will then call the Polymorphic Run method to
execute the code


DATA:                   See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:                   Concrete class

PURPOSE:                To run a CRunnableClass using Thread Local Store
to allocate and store the terminate event.

FUNCTION:               This class will cause the Run function of the CRunnableClass
object passed into the constructor to be called from a new thread.


INTERFACES:             "See 'INTERFACES:' in the module declaration below"


RESOURCES:

REFERENCES:


PROCESSING:             Construct the class. This will allocate the TLS area.
The object in the Constructor will then be executed.

If the running object needs to use resources then the class
can be waiting not only on the resources but also on the TLS
termainte event as well. This latter point will be trasparent t
to the user however.

Deleting a CTLSThread object will ensure that the thread has
terminated before the destructor returns.

The class should not be inherited from.

DATA:                   See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/


#include "MSGEvent.h"
#include "CCriticalSection.h"

#include <map>
#include <string>

namespace Util
{
#if defined(PAL_NON_USED_CODE)  // Disable SetThreadName() under Linux
	void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);
#endif

	class CRunnableClass
	{
	public:
		// Description
		//          A base class that all classes which will be thread entities need
		//          to be derived from in order to be compiled
		CRunnableClass();

		// Description
		//          Destrcutor
		virtual ~CRunnableClass();

		// Description
		//          This function will be called in a new thread when passed into the
		//          constructor of a CClassThread object.
		virtual DWORD Run() = 0;

		// The following are essentially PRIVATE: DO NOT TOUCH THEM!!!
		Util::CMSGEvent m_ExitEvent;
		// This is the handle of the event that should be used to terminate the
		// CClassThread that is running this object.  It is used by the CClassThread
		// destructor.

		// This is the handle of the exit event that is checked by the replacement
		// WaitFor... functions.  The object is owned by the CRunnableClass object and
		// will be deleted when that is.
		static HANDLE ExitEvent();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};

	void MSGSleep(DWORD dwMilliseconds);
	// Use the above functions in preference to the Windows NT implementations;
	// They will throw an exception when the thread has been asked to terminate.
	// This will allow the code to terminate gracefully (through the use of
	// destructors of objects on the stack).
	// DO NOT catch this exception and then continue execution; it MUST result
	// in the termination of the current thread, otherwise the program will not
	// exit properly.


	//  class CTLSThread : public Util::CStatisticsProvider
	class CTLSThread
	{
	public:

		// Description
		//          Constructor
		CTLSThread(
			CRunnableClass& i_runnable,					// The instance of the derived
														// class to run.
			int i_Priority = THREAD_PRIORITY_NORMAL,	// Thread priority:
			const int i_PreferredProcessor = 0,			// Preferred processor.
														// <= 0: No preference.
														// >  0: Use nth processor available
														//       in process affinity mask.
			const char * iThreadName = NULL,			// allow a name
			bool startIt = true,						// allow create suspended
			bool managedThread = false,					// have session manager manage this thread.
			bool superThread = false);					// what type of managed threads we have.

														// Description
														//          Destructor
		~CTLSThread();


		// Description
		//          This thread signals to the thread that the thread must terminate
		//          immediately.
		//          Calling this function sets the exit event for the CRunnableClass object
		//          which will cause it to throw an exception next time it blocks.
		void StopThread();

		// Description
		//          Waits for the thread to conclude.  Returns true if completed.
		bool WaitForCompletion(DWORD timeout = INFINITE);

		// Description
		// Must only be called when the old thread has completed.
		void RestartThread();

		// Description
		//          Get a reference to the thread handle
		// Returns
		//          HANDLE          The handle to the thread.
		//                                  this is used in the MSGWaitForMultipleObjects
		HANDLE GetHandle() { return m_ThreadHandle.GetHandle(); }


		// DATA:
		// The associated CRunnableClass object.
		CRunnableClass& m_runnable;

		unsigned GetThreadID() { return m_ThreadID; }
		static  const std::string & Name(const DWORD & iThreadId);
		static  void  ReportThreadStatuses();

		bool IsThreadManaged() { return m_managedThread; }
		bool IsASuperThread() { return m_superThread; }
		bool IsRunning();

		int GetPriority() const { return m_Priority; }

		void SetPriority(int priority);

	private:
		static Util::CSmartCriticalSection m_NameLock;
		static std::map<DWORD, std::pair<std::string, HANDLE> > m_ThreadNames;

		std::string m_Name;

		// Description
		//          Copy constructor
		// Arguments
		//          the Threads to copy
		CTLSThread(const CTLSThread&);

		// The exit code of the thread.  STILL_ACTIVE if not finished yet.
		DWORD ThreadExitCode();

		// Description
		//          = operator
		// Arguments
		//          The thread to copy over the this object
		CTLSThread* operator=(const CTLSThread&);


		// These two declarations cause a compilation error if the object is
		// assigned to, or otherwise copied.
		Util::CSmartHandle m_ThreadHandle;
		// The handle of the thread.

		// Description
		//          The internal run method call that will call the run method
		//          in the runnableClass
		// Arguments
		//          pRunnableClass          A pointer to the runnable class itself.
		static unsigned int __stdcall STATIC_Run(void *pCTLSThread);


		// Thread priority:
		// THREAD_PRIORITY_ABOVE_NORMAL             Indicates 1 point above normal priority
		//                                                                  for the priority class.
		// THREAD_PRIORITY_BELOW_NORMAL             Indicates 1 point below normal priority
		//                                                                  for the priority class.
		// THREAD_PRIORITY_HIGHEST
		//                                                                  Indicates 2 points above normal priority
		//                                                                  for the priority class.
		// THREAD_PRIORITY_IDLE                             Indicates a base priority level of 1
		//                                                                  for IDLE_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS,
		//                                                                  or HIGH_PRIORITY_CLASS processes, and a base
		//                                                                  priority level of 16 for REALTIME_PRIORITY_CLASS
		//                                                                  processes.
		// THREAD_PRIORITY_LOWEST                   Indicates 2 points below normal priority
		//                                                                  for the priority class.
		// THREAD_PRIORITY_NORMAL                   Indicates normal priority for the priority class.
		// THREAD_PRIORITY_TIME_CRITICAL    Indicates a base priority level of 15 for
		//                                                                  IDLE_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, or
		//                                                                  HIGH_PRIORITY_CLASS processes, and a base priority
		//                                                                  level of 31 for REALTIME_PRIORITY_CLASS processes.
		int m_Priority;

		// Preferred processor.
		// <= 0: No preference.
		// >  0: Use nth processor available in process affinity mask.
		int m_PreferredProcessor;


		enum ThreadState { NotRunning, Starting, Running, Stopping, Exited };

		Util::CSmartCriticalSection m_stateLock;
		ThreadState m_state;

		HANDLE m_hStartedEvent;

		unsigned m_ThreadID;
		HANDLE m_hExitEvent;

		bool m_managedThread;
		bool m_superThread;
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};


	// Thread killer....
	class CThreadTerminator
	{
	public:
		CThreadTerminator(CTLSThread& i_Thread);
		~CThreadTerminator();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};

} //  end of the Util namepace

#endif

