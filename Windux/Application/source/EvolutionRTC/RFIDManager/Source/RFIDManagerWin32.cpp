//
/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "RFIDManager.h"
#include "RFIDEvent.h"
#include "TIRISReadOnlyId.h"
#include "RFIDStatus.h"

#include "Debug.h"
#include "Configuration.h"

#include "util/thread/lock.h"


#if defined(PAL_WIN32_RUN)

void RFIDManager::connect(LPCWSTR comPort)
{
	for ( int i = 0; i < OPEN_COM_RETRY_COUNT && hComPort_ == PAL_INVALID_COM_HANDLE_VALUE; ++i )
	{
		hComPort_ = ::CreateFile(comPort, GENERIC_READ|GENERIC_WRITE,
				0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		if ( hComPort_ != INVALID_HANDLE_VALUE )
		{
			if ( tracing_ )
				Debug::trace("RFIDManager", "connect success");

			COMMTIMEOUTS commTimeouts;

			if ( ::GetCommTimeouts(hComPort_, &commTimeouts) )
			{
				commTimeouts.ReadIntervalTimeout = 20; 
				commTimeouts.ReadTotalTimeoutMultiplier = 0;
				commTimeouts.ReadTotalTimeoutConstant = 5000;//timeout 5s
				commTimeouts.WriteTotalTimeoutMultiplier = 0;
				commTimeouts.WriteTotalTimeoutConstant = 5000;//timeout 5s


				if ( ::SetCommTimeouts(hComPort_, &commTimeouts) )
				{
					if ( ::GetCommState(hComPort_, &commProperties_) )
					{
						commProperties_.BaudRate = CBR_9600;
						commProperties_.ByteSize = 8;
						commProperties_.Parity = NOPARITY;
						commProperties_.StopBits = ONESTOPBIT;
						commProperties_.fOutX = false;
						commProperties_.fInX = false;
						commProperties_.XonChar = 0x11;
						commProperties_.XoffChar = 0x13;
						commProperties_.fRtsControl  = RTS_CONTROL_DISABLE;

						if ( ::SetCommState(hComPort_, &commProperties_) )
						{
							if (!RFIDManagerThread_)
							{
								if ( tracing_ )
									Debug::trace("RFIDManager", "creating new run thread");

								RFIDManagerThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "RFIDManager Thread" );
							}
							else
							{
								if ( tracing_ )
									Debug::trace("RFIDManager", "restarting run thread");

								RFIDManagerThread_->RestartThread();
							}
						}
						else
                        {
                            PAL_showError("Failed to set COMM State");
                        }
					}
					else
                    {
                        PAL_showError("Failed to get COMM State");
                    }
				}
				else
                {
                    PAL_showError("Failed to set COMM Timeouts");
                }
			}
			else
            {
                PAL_showError("Failed to get COMM Timeouts");
            }
		}
		else
		{
            PAL_showError("Failed to create handle");
			//Sleep(6*1000); // Sleep 6 seconds and try to open the port again
			PAL_absclass::PAL_sleep(6*1000);
		}
	}
}


void RFIDManager::disconnect()
{
	Util::CRunnableClass::m_ExitEvent.EventSet();

    if (( RFIDManagerThread_ ) && ( RFIDManagerThread_->IsRunning() ))
	{
		util_time::Time t0;

		RFIDManagerThread_->WaitForCompletion();

		util_time::Time t1;
		Debug::trace("RFIDManager", "disconnecting: had to wait for " +
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " ms. before trying to close handle");
	}


	if ( hComPort_ != INVALID_HANDLE_VALUE )
	{
		if ( tracing_ )
			Debug::trace("RFIDManager", "disconnecting comport");

		if ( !::CloseHandle(hComPort_) )
        {
            PAL_showError("Failed to close the handle");
        }

		hComPort_ = INVALID_HANDLE_VALUE;
	}
}


DWORD RFIDManager::ReadBytesBlocking(DWORD bytesToRead, UINT8 * buffer, LPOVERLAPPED pOVL, DWORD *pBytesRead)
{
	try
	{
		BOOL b = ::ReadFile(hComPort_, buffer, bytesToRead, pBytesRead, pOVL);
		if ( !b )
		{
			DWORD err = ::GetLastError();

			if ( err == ERROR_IO_PENDING )
			{
				DWORD l_ret = Util::MSGWaitForSingleObject(pOVL->hEvent, INFINITE);
				switch (l_ret)
				{
					case WAIT_OBJECT_0:
					{
						b = GetOverlappedResult(hComPort_, pOVL, pBytesRead, TRUE);

						if ( !b )
                        {
                            PAL_showError("GetOverlappedResult failed on Read");
                        }
						break;
					}
					case WAIT_TIMEOUT:
					{
						if ( tracingError_ )
							Debug::trace("RFIDManager.Error", "************ ReadBytesBlocking: Timedout in ReadBytesBlocking waiting on event. This should not happen");

						return COM_TIMED_OUT;
					}
					default:
					{
						if ( tracingError_ )
							Debug::trace("RFIDManager.Error", "************ ReadBytesBlocking: Unexpected return from MSGWaitForSingleObject");

						return COM_ERROR;
					}
				}
			}
			else
			{
                PAL_showError("Read bytes failed", err);
				return COM_ERROR;
			}
		}

		if ( *pBytesRead < bytesToRead )
		{
			if ( tracingError_ )
				Debug::trace("RFIDManager.Error", "******** Did not read as many bytes as intended");

			return COM_TIMED_OUT;
		}

		return COM_SUCCESS;
	}
	catch ( const Util::CTerminateNow& )
	{
		if ( tracingError_ )
			Debug::trace("RFIDManager.Error", "************ Received CTerminateNow exception in ReadBytesBlocking");

		return TERMINATE_THREAD;
	}
	catch (...)
	{
		if ( tracingError_ )
			Debug::trace("RFIDManager.Error", "************ Received exception in ReadBytesBlocking");

		return COM_ERROR;
	}

}

DWORD RFIDManager::WriteBytesBlocking(DWORD bytesToWrite, UINT8* buffer, LPOVERLAPPED pOVL, DWORD *pBytesWrote)
{
	try
	{
		BOOL b = ::WriteFile(hComPort_, buffer, bytesToWrite, pBytesWrote, pOVL);
		if ( !b )
		{
			DWORD err = GetLastError();

			if (err == ERROR_IO_PENDING)
			{

				DWORD l_ret = Util::MSGWaitForSingleObject(pOVL->hEvent, INFINITE);
				switch (l_ret)
				{
					case WAIT_OBJECT_0:
					{
						b = GetOverlappedResult(hComPort_, pOVL, pBytesWrote, TRUE);

						if ( !b )
                        {
                            PAL_showError("GetOverlappedResult failed on Write");
                        }
						break;
					}
					case WAIT_TIMEOUT:
					{
						if ( tracingError_ )
							Debug::trace("RFIDManager.Error", "************ WriteBytesBlocking: Timedout waiting on event. This should not happen");

						return COM_TIMED_OUT;
					}
					default:
					{
						if ( tracingError_ )
							Debug::trace("RFIDManager.Error", "************ WriteBytesBlocking: Unexpected return from MSGWaitForSingleObject");

						return COM_ERROR;
					}
				}
			}
			else
			{
                PAL_showError("Failed to Write bytes", err);
				return COM_ERROR;
			}
		}

		if ( *pBytesWrote < bytesToWrite )
		{
			if ( tracingError_ )
			{
				Debug::trace("RFIDManager.Error", "************ WriteBytesBlocking: Only wrote " +
					util_string::ulongtostring(*pBytesWrote) + " bytes of " + util_string::ulongtostring(bytesToWrite) +
					" of '" + util_string::stringtohexstring(std::string((char*)buffer, bytesToWrite)) + "'");
			}

			return COM_TIMED_OUT;
		}

		return COM_SUCCESS;
	}
	catch ( const Util::CTerminateNow& )
	{
		if ( tracingError_ )
			Debug::trace("RFIDManager.Error", "************ Received CTerminateNow exception in WriteBytesBlocking");

		return TERMINATE_THREAD;
	}
	catch (...)
	{
		if ( tracingError_ )
			Debug::trace("RFIDManager.Error", "************ Received exception in WriteBytesBlocking");

		return COM_ERROR;
	}

}

std::string RFIDManager::getLastErrorMessage(DWORD error)
{
	LPVOID errorMessage;
	std::string message;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL);

	message.assign(util_string::wchartostring((const wchar_t *)errorMessage));

	LocalFree(errorMessage);

	return message;
}

#endif  // PAL_WIN32_RUN
