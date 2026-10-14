// Header file included by ANTLR UniLexer

#include "PAL_platformheader.h"

#if defined ( _WIN32 )
#include <tchar.h>
#include <process.h>
#endif      // ( _WIN32 )

#include <atomic>
#include <string>
#include <typeinfo>

#include "CTLSThread.h"                 // Util
#include "TypeName.h"                   // Util
#include "OutputLogger.h"
#include "SessionManager.h"

#ifndef EVOL_NDEBUG
#include "../util/Debug.h"
#include "../db/util/e_string/utilstring.h"
#include "Hex.h"
#endif

#if defined ( _WIN32 )
#pragma warning ( disable: 4073)
#pragma init_seg(lib)
#endif      // _WIN32

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace Util
{

	std::map<DWORD, std::pair<std::string, HANDLE> > CTLSThread::m_ThreadNames;
	Util::CSmartCriticalSection CTLSThread::m_NameLock;

	Util::CSmartCriticalSection TypeNameCSContainer::sm_TypeNameCS;     // Declared here so that no new .cpp file was needed.

																		/// Convert string from wide character to multibyte
	std::string ToUtf8(const std::wstring &s)
	{
		std::string result;

		const size_t len(wcstombs(NULL, s.c_str(), 0));

		if (len)
		{
			char *mbs = new char[len + 1];
			wcstombs(mbs, s.c_str(), len + 1);

			result = mbs;
			delete[] mbs;
		}

		return result;
	}

	// This class provides an explicit interface to Thread Local Storage.
	// The way it works is that you create an instance of the class and
	// it allocates a TLS slot which is basically a single DWORD of
	// space that will be replaced on every thread swap.
	// If you wanted to store more than 32 bits, you would allocate a block
	// of memory and store the pointer to it in the 32 bit value.
	// Since we only have a single HANDLE to store in TLS, we'll just store
	// the handle there.
	class TLSHandle
	{
	public:

		/// Constructor
		TLSHandle()
		{
			m_handle = PAL_absclass::PAL_tlsAlloc();
			Assert(m_handle != PAL_TLS_OUT_OF_INDEXES, CNamedException(_T("Cannot allocate Thread-Local Storage")));
		}

		/// Destructor
		~TLSHandle()
		{
			PAL_absclass::PAL_tlsFree(m_handle);
		}

		/// Retrieve TLS value
		LPVOID GetValue() { return PAL_absclass::PAL_tlsGetValue(m_handle); }

		// Set TLS value
		void SetValue(LPVOID i_value)
		{
			if (!PAL_absclass::PAL_tlsSetValue(m_handle, i_value))
			{
				CPlatformException e;
				LogAndThrow(e, _T("Problem with Thread Local Storage, store data"), _T(__FILE__), __LINE__);
			}
		}

	private:
		DWORD m_handle;
	};

	static TLSHandle TheTLSHandle;      // Global

										// ============================================================================================
										// CRunnableClass
										// ============================================================================================

#ifdef USE_ACCOUNTANT
	int CRunnableClass::dataType = 0;
	std::string CRunnableClass::dataTypeName("CRunnableClass");
#endif

	CRunnableClass::CRunnableClass() : m_ExitEvent((HANDLE)NULL)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}

	CRunnableClass::~CRunnableClass()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}

	// ============================================================================================
	// CTLSThread
	// ============================================================================================

#ifdef USE_ACCOUNTANT
	int CTLSThread::dataType = 0;
	std::string CTLSThread::dataTypeName("CTLSThread");
#endif

	CTLSThread::CTLSThread(
		CRunnableClass &i_runnable,
		int i_Priority,
		const int i_PreferredProcessor,
		const char *iThreadName,
		bool startIt,
		bool managedThread,
		bool superThread) :
		m_runnable(i_runnable),
		m_Priority(i_Priority),
		m_PreferredProcessor(i_PreferredProcessor),
		m_managedThread(managedThread),
		m_superThread(superThread)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		m_state = NotRunning;
		m_ThreadID = 0;

		try
		{
			PRECONDITION(m_runnable.m_ExitEvent == NULL);     // Object is not associated with a thread object

			m_Name = (NULL == iThreadName) ? "AThread" : iThreadName;

			if (Debug::isRunning())
				Debug::trace("CTLSThread", "---- creating '" + m_Name + "' ----");

			m_runnable.m_ExitEvent = m_hExitEvent = PAL_absclass::PAL_createEvent(NULL, TRUE, FALSE, NULL);

			Assert(m_runnable.m_ExitEvent != NULL, Util::CPlatformException());
			m_hStartedEvent = PAL_absclass::PAL_createEvent(NULL, TRUE, FALSE, NULL);

			if (startIt)
				RestartThread();
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception in CTLSThread CTOR (" + m_Name + ")!\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;

		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception in CTLSThread CTOR (" + m_Name + ")!");
			LogCatchAndThrow;
		}
	}

	CTLSThread::~CTLSThread()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
		try
		{
			Debug::trace("CTLSThread", "---- destroying '" + m_Name + "' ----");

			if (!IsRunning())
				Debug::trace("CTLSThread", "Thread is not running");
			else
			{
				StopThread();

				// Wait until it is shut down.
				// Does not use the MSG function because we have to be sure that the thread
				// has terminated before continuing.
				while (IsRunning())
				{
					Debug::trace("CTLSThread", "Waiting for '" + m_Name + "' to end...");
					PAL_absclass::PAL_sleep(250);
				}

				Debug::trace("CTLSThread", "wait done!");
			}

			if ((m_hStartedEvent) && (!PAL_absclass::PAL_closeHandle(m_hStartedEvent)))
				Debug::trace("CTLSThread", "Error closing handle");

			m_hStartedEvent = NULL;

			Debug::trace("CTLSThread", "Destroy thread COMPLETE");
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception in CTLSThread DTOR (" + m_Name + ")!\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;

		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception during CTLSThread DTOR!");
			LogCatchAndThrow;
		}
	}

	DWORD CTLSThread::ThreadExitCode()
	{
		DWORD ExitCode = 0;

		Debug::trace("CTLSThread", "Getting thread exit code");

		try
		{
			PRECONDITION((m_ThreadHandle) && (m_ThreadHandle != INVALID_HANDLE_VALUE)); // There is a thread to check!

			PAL_absclass::PAL_getExitCodeThread(m_ThreadHandle.GetHandle(), &ExitCode);
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception while getting exit code (" + m_Name + ")!\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception while getting exit code!");
			LogCatchAndThrow;
		}

		return ExitCode;
	}

	bool CTLSThread::WaitForCompletion(DWORD timeout)
	{
		Debug::trace("CTLSThread", "Wait for thread completion");

		try
		{
			if (IsRunning())
			{
				Debug::trace("CTLSThread", "Waiting...");

				while (IsRunning())
				{
					if (timeout <= 0)
					{
						Debug::trace("CTLSThread", "Thread did not complete within timeout!!");
						return false;
					}

					PAL_absclass::PAL_sleep(100);
					timeout -= 100;
				}

				Debug::trace("CTLSThread", "wait done!");
				return true;
			}
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException in WaitForCompletion() (" + m_Name + ")!\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception in WaitForCompletion()");
			LogCatchAndThrow;
		}

		return true;
	}

	const std::string& CTLSThread::Name(const DWORD& iThreadId)
	{
		static std::string s_default("AThread");
		std::string & l_ret = s_default;

		try
		{
			Util::CSmartCriticalSectionGuard guard(&m_NameLock);
			// We use the base api call, in case "CritSecMonitoring" is enabled
			std::map<DWORD, std::pair<std::string, HANDLE> >::iterator l_iter = m_ThreadNames.find(iThreadId);
			if (l_iter != m_ThreadNames.end())
			{
				l_ret = l_iter->second.first;
			}
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException in Name()\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatch;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception in Name()");
			LogCatch;
		}

		return l_ret;
	}

	bool CTLSThread::IsRunning()
	{
		if ((m_ThreadHandle) && (INVALID_HANDLE_VALUE != m_ThreadHandle))
		{
			Util::CSmartCriticalSectionGuard sg(&m_stateLock);

			const ThreadState s(m_state);

			if ((Starting == s) || (Running == s) || (Stopping == s))
				return true;
			else if (WAIT_OBJECT_0 == PAL_absclass::PAL_waitForSingleObj(m_hStartedEvent, 0))
				return true;
		}

		return false;
	}

	void CTLSThread::RestartThread()
	{
		try
		{
			if (Debug::isRunning())
				Debug::trace("CTLSThread", "---- restarting thread '" + m_Name + "' ----");

			try
			{
				if (IsRunning())
				{
					StopThread();
					WaitForCompletion(INFINITE);
				}
			}
			catch (Util::CPlatformException &e)
			{
				Debug::trace("CTLSThread.exception", "CPlatformException in RestartThread() (" + m_Name + ") while stopping thread\n" +
					ToUtf8(e.DetailedReportMsg()));
				LogCatch;
			}
			catch (...)
			{
				Debug::trace("CTLSThread.exception", "Exception in CTLSThread::RestartThread() while stopping thread");
				LogCatch;
			}

			// Reset the exit event in the thread
			Util::CSmartCriticalSectionGuard sg(&m_stateLock);

			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Reset events...");

			PAL_absclass::PAL_resetEvent(m_hExitEvent);
			PAL_absclass::PAL_resetEvent(m_hStartedEvent);
			m_state = Starting;

			try
			{
				Util::CSmartCriticalSectionGuard guard(&m_NameLock);
				m_ThreadNames.erase(m_ThreadID);
			}
			catch (Util::CPlatformException &e)
			{
				Debug::trace("CTLSThread.exception", "CPlatformException in RestartThread() (" + m_Name + ") while erase name\n" +
					ToUtf8(e.DetailedReportMsg()));
				LogCatch;
			}
			catch (...)
			{
				Debug::trace("CTLSThread.exception", "Exception in RestartThread() while erase name");
				LogCatch;
			}

			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Begin thread...");

			m_ThreadHandle = (HANDLE)PAL_absclass::PAL_beginthreadex(NULL, 0, STATIC_Run, this, PAL_THREAD_CREATE_SUSPENDED, &m_ThreadID);
			Assert((m_ThreadHandle != (HANDLE)0) && (INVALID_HANDLE_VALUE != m_ThreadHandle), Util::CPlatformException());

			try
			{
				Util::CSmartCriticalSectionGuard guard(&m_NameLock);
				m_ThreadNames[m_ThreadID] = std::pair<std::string, HANDLE>(m_Name, m_ThreadHandle);
			}
			catch (Util::CPlatformException &e)
			{
				Debug::trace("CTLSThread.exception", "CPlatformException in RestartThread() (" + m_Name + ") while set name\n" +
					ToUtf8(e.DetailedReportMsg()));
				LogCatch;
			}
			catch (...)
			{
				Debug::trace("CTLSThread.exception", "Exception in RestartThread() while set name");
				LogCatch;
			}

			// Set thread priority.
			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Set priority...");

			Assert(PAL_absclass::PAL_setThreadPriority(m_ThreadHandle, m_Priority) == TRUE, Util::CPlatformException());

			// Set preferred processor.
			// Select from processors foreseen in the process affinity mask.
			if (m_PreferredProcessor > 0)
			{
				DWORD processAffinityMask;
				DWORD systemAffinityMask;
				Assert(PAL_absclass::PAL_getProcessAffinityMask(
					PAL_absclass::PAL_getCurrProc(),
					(PDWORD_PTR) &processAffinityMask,
					(PDWORD_PTR) &systemAffinityMask), Util::CPlatformException());

				for (int idealProcessor = m_PreferredProcessor - 1;
					idealProcessor < m_PreferredProcessor - 1 + 32; ++idealProcessor)
					if (processAffinityMask & (1 << (idealProcessor % 32)))
					{
						Assert(PAL_absclass::PAL_setThreadIdealProcessor(
							m_ThreadHandle,
							idealProcessor) != (DWORD)-1, Util::CPlatformException());
					}
			}

			// Resume thread.
			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Resume thread...");

			Assert(PAL_absclass::PAL_resumeThread(m_ThreadHandle) == 1, Util::CPlatformException());

			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Wait for thread to start...");

			// wait for thread to start
			PAL_absclass::PAL_waitForSingleObj(m_hStartedEvent, INFINITE);

			if (Debug::isRunning())
				Debug::trace("CTLSThread", "Thread started!");
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException beginning (" + m_Name + ")\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception beginning '" + m_Name + "' thread!!");
			LogCatchAndThrow;
		}
	}

	unsigned int __stdcall CTLSThread::STATIC_Run(void *pCTLSThread)
	{
		CTLSThread *thread((CTLSThread*)pCTLSThread);
		DWORD result(0);

		char threadId[64];
		sprintf(threadId, "%d", thread->m_ThreadID);

		std::string threadName(thread->m_Name);

		try
		{
			// We are now running...
			PAL_absclass::PAL_setEvent(thread->m_hStartedEvent);

			Debug::trace("CTLSThread", "---- Starting '" + threadName + "' thread (id " + threadId + ") ----");

			thread->m_stateLock.Enter();
			const bool shouldRun(Starting == thread->m_state);
			thread->m_state = Running;
			thread->m_stateLock.Leave();

			try
			{
				if (thread->m_managedThread)
					SessionManager::getInstance()->registerThread(thread->m_ThreadID, thread->m_superThread);

				// run!
				if (shouldRun)
				{
					// setup exit event for this thread
					CRunnableClass *runnable = &thread->m_runnable;

					TheTLSHandle.SetValue(runnable->m_ExitEvent.GetHandle());

					result = runnable->Run();
				}
			}
			catch (Util::CTerminateNow&)
			{
				Debug::trace("CTLSThread.exception", "CTerminateNow exception in thread '" + threadName + "'");
			}
			catch (Util::CPlatformException &e)
			{
				Debug::trace("CTLSThread.exception", "CPlatformException exception in thread '" + threadName + "'\n" +
					ToUtf8(e.DetailedReportMsg()));
			}
			catch (...)
			{
				Debug::trace("CTLSThread.exception", "Exception in thread '" + threadName + "'");
			}

			Debug::trace("CTLSThread", "---- Exiting '" + threadName + "' thread ----");

			if (thread->m_managedThread)
				SessionManager::getInstance()->deRegisterThread(thread->m_ThreadID, thread->m_superThread);
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception in in thread run function '" + threadName + "'\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Unhandled exception in thread run function!");

			// ***** ERROR: UnitTester testing case 3 (-n 3) - "FATAL: exception not rethrown"
			LogCatchAndThrow;       // ***** WARNING: Add this to pass "core" dump. But there must be something wrong
		}

		Debug::trace("CTLSThread", "Exit thread COMPLETE");

		// reset exit event
		TheTLSHandle.SetValue(NULL);

		// We are not running any longer
		PAL_absclass::PAL_resetEvent(thread->m_hStartedEvent);

#if defined ( _WIN32 )
		thread->m_stateLock.Enter();
		thread->m_state = Exited;
		thread->m_stateLock.Leave();
#else
		Util::CSmartCriticalSectionGuard sg(&thread->m_stateLock);
		thread->m_state = Exited;
#endif

		// Note: the result value may actually _be_ STILL_ACTIVE!
		// This shouldn't prove a problem, though, it just means an
		// extra call to WaitForSingleObject.
		PAL_absclass::PAL_endthreadex(result);

		return result;
	}

	void CTLSThread::StopThread()
	{
		try
		{
			Util::CSmartCriticalSectionGuard sg(&m_stateLock);

			if (!IsRunning())
				return;

			Debug::trace("CTLSThread", "Stopping thread...");

			m_state = Stopping;

			// Direct the thread to shutdown
			//m_runnable.m_ExitEvent.EventSet();
			PAL_absclass::PAL_setEvent(m_hExitEvent);

			try
			{
				Util::CSmartCriticalSectionGuard guard(&m_NameLock);
				m_ThreadNames.erase(m_ThreadID);
			}
			catch (Util::CPlatformException &e)
			{
				Debug::trace("CTLSThread.exception", "CPlatformException exception in StopThread '" + m_Name + "' while erase name\n" +
					ToUtf8(e.DetailedReportMsg()));
				LogCatch;
			}
			catch (...)
			{
				Debug::trace("CTLSThread.exception", "Exception in StopThread() while erase name");
				LogCatch;
			}

			Debug::trace("CTLSThread", "Stopping thread done!");
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception in StopThread '" + m_Name + "'\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception 2 in StopThread()");
			LogCatchAndThrow;
		}
	}

	void CTLSThread::SetPriority(int priority)
	{
		Debug::trace("CTLSThread", "Setting priority...");

		if (!IsRunning())
			Debug::trace("CTLSThread", "thread not running!");
		else if (!PAL_absclass::PAL_setThreadPriority(m_ThreadHandle, m_Priority))
			Debug::trace("CTLSThread", "error setting priority");
		else
		{
			Debug::trace("CTLSThread", "priority set");
			m_Priority = priority;
		}
	}

#if defined ( __GNUC__ )        // http://wiki.services.openoffice.org/wiki/Writing_correct_Cplusplus#Extra_Qualification
	void MSGSleep(DWORD dwMilliseconds)
#else
	void Util::MSGSleep(DWORD dwMilliseconds)
#endif
	{
		try
		{
			PRECONDITION(CRunnableClass::ExitEvent() != INVALID_HANDLE_VALUE);        // being called from a non class thread
			PRECONDITION(CRunnableClass::ExitEvent() != NULL);        // being called from a non class thread

			CRunnableClass::ExitEvent();

			if (WAIT_TIMEOUT != PAL_absclass::PAL_waitForSingleObj(CRunnableClass::ExitEvent(), dwMilliseconds))
				throw CTerminateNow();
		}
		catch (Util::CTerminateNow&)
		{
			Debug::trace("CTLSThread.exception", "CTerminateNow in MSGSleep()");
			throw;      // Do nothing
		}
		catch (Util::CPlatformException &e)
		{
			Debug::trace("CTLSThread.exception", "CPlatformException exception in MSGSleep\n" +
				ToUtf8(e.DetailedReportMsg()));
			LogCatchAndThrow;
		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "Exception in MSGSleep()");
			LogCatchAndThrow;
		}
	}

	// The Event handle is stored in Thread Local Storage; as there is only one
	// value to be stored there, we don't need to allocate memory, just store the
	// event handle as the TLS value.

	HANDLE CRunnableClass::ExitEvent()
	{
		return TheTLSHandle.GetValue();
	}
	void SetExitEvent(HANDLE iHandle)
	{

		try
		{
			TheTLSHandle.SetValue(iHandle);

		}
		catch (...)
		{
			Debug::trace("CTLSThread.exception", "---- Exception in CTLSThread::SetExitEvent() ----");
			LogCatchAndThrow;
		}
	}

	void CTLSThread::ReportThreadStatuses()
	{
		try
		{
			Util::CLockCriticalSection l_lock(m_NameLock);
			Util::COutputLogger::ReportToErrorLog(L"********* Thread Monitor Report *********", L"");
			std::map<DWORD, std::pair<std::string, HANDLE> >::iterator l_iter = m_ThreadNames.begin();
			for (; l_iter != m_ThreadNames.end(); l_iter++)
			{
				char l_buff[180];

				DWORD l_status;

				HANDLE & l_handle = l_iter->second.second;
				PAL_absclass::PAL_getExitCodeThread(l_handle, &l_status);
				sprintf(l_buff, " Thread Name: %s Status : %s \r\n", l_iter->second.first.c_str(),
					(l_status == STILL_ACTIVE) ? "Active" : "InActive");
				Util::COutputLogger::GetErrorLogger().WriteStr(l_buff);
				{
					PAL_FILETIME CreationTime, ExitTime, KernelTime, UserTime;
					if (TRUE == PAL_absclass::PAL_getThreadTimes(l_handle, &CreationTime, &ExitTime, &KernelTime, &UserTime))
					{
						std::string l_ss;
						UINT64 temp;
						temp = (UINT64)CreationTime.dwLowDateTime;
						temp += ((UINT64)CreationTime.dwHighDateTime << 32);
						char l_buff[80];
						sprintf(l_buff, "\tCreationTime %lld \r\n", temp / 10000);
						l_ss = l_buff;
						Util::COutputLogger::GetErrorLogger().WriteStr(l_ss);
						temp = (UINT64)ExitTime.dwLowDateTime;
						temp += ((UINT64)ExitTime.dwHighDateTime << 32);
						sprintf(l_buff, "\tExitTime %lld \r\n", temp / 10000);
						l_ss = l_buff;
						Util::COutputLogger::GetErrorLogger().WriteStr(l_ss);

						temp = (UINT64)KernelTime.dwLowDateTime;
						temp += ((UINT64)KernelTime.dwHighDateTime << 32);
						sprintf(l_buff, "\tKernelTime %lld \r\n", temp / 10000);
						l_ss = l_buff;
						Util::COutputLogger::GetErrorLogger().WriteStr(l_ss);

						temp = (UINT64)UserTime.dwLowDateTime;
						temp += ((UINT64)UserTime.dwHighDateTime << 32);
						sprintf(l_buff, "\tUserTime %lld \r\n", temp / 10000);
						l_ss = l_buff;
						Util::COutputLogger::GetErrorLogger().WriteStr(l_ss);

					}
					else
						Util::COutputLogger::ReportToErrorLog(L" Cannot Get Thread Times   ", L"");

				}



			}

		}

		catch (...)
		{
			Debug::trace("CTLSThread.exception", "---- Exception in CTLSThread::ReportThreadStatuses() ----");
			LogCatch;
		}
	}

#ifdef USE_ACCOUNTANT
	int CThreadTerminator::dataType = 0;
	std::string CThreadTerminator::dataTypeName("CThreadTerminator");
#endif

	CThreadTerminator::CThreadTerminator(CTLSThread& i_Thread)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
		/* From http://msdn.microsoft.com/en-us/library/ms686717(VS.85).aspx
		* TerminateThread is a dangerous function that should only be used in the most extreme cases.
		* You should call TerminateThread only if you know exactly what the target thread is doing,
		* and you control all of the code that the target thread could possibly be running at the
		* time of the termination. For example, TerminateThread can result in the following problems:
		*
		* If the target thread owns a critical section, the critical section will not be released.
		* If the target thread is allocating memory from the heap, the heap lock will not be released.
		* If the target thread is executing certain kernel32 calls when it is terminated, the kernel32 state for the thread's process could be inconsistent.
		* If the target thread is manipulating the global state of a shared DLL, the state of the DLL could be destroyed, affecting other users of the DLL.
		*
		*/
		PAL_absclass::PAL_terminateThread(i_Thread.GetHandle(), (DWORD)-1);
	}

	CThreadTerminator::~CThreadTerminator()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}
} // end of the namespace
