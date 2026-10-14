//
/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

//#include "RFIDManager.h"

/*
#include "RFIDEvent.h"
#include "TIRISReadOnlyId.h"
#include "RFIDStatus.h"

#include "Debug.h"
#include "Configuration.h"

#include "util/thread/lock.h"
*/

#if defined( __linux__ )

#include <errno.h>

void RFIDManager::connect(LPCWSTR comPort)
{
    int name_len = wcslen(comPort);
    char pathname[name_len+1];
    struct termios options;

    // Convert wchar_t to char_t
    wcstombs(pathname, comPort, name_len);

    for ( int i = 0; i < OPEN_COM_RETRY_COUNT && hComPort_ == PAL_INVALID_COM_HANDLE_VALUE; ++i )
    {
        hComPort_ = open(pathname, O_RDWR | O_NOCTTY | O_NONBLOCK);

        if ( hComPort_ == PAL_INVALID_COM_HANDLE_VALUE )
        {
            PAL_showError("Failed to create handle");
            PAL_absclass::PAL_sleep( 6 * 1000 );        // Sleep 6 seconds and try to open the port again

            continue;
        }

        if ( tracing_ )
            Debug::trace("RFIDManager", "connect success");

        if (tcgetattr(hComPort_, &options) != 0)
        {
            PAL_showError("Failed to get COM port options");
            continue;
        }

        // set Read & Write baudrate to 9600 bps
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);

        // Protocol = 8N1
        options.c_cflag &= ~PARENB;     // Non parity
        options.c_cflag &= ~CSTOPB;     // one stop bit
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;         // 8-bits
        options.c_cflag &= ~(IXON | IXOFF);     // Disable XON/XOFF flow control
        options.c_cflag &= ~CRTSCTS;    // Disable (hardware) RTS/CTS flow control
        // ***** WARNING: we need to disable RTS flow control (RTS_CONTROL_DISABLE in win)
        //  But there is no equivlent settings, also it depends on driver's implementaiton.

        // Timeout
        //  VMIN:   minimum amount of characters to read
        //  VTIME:  time to wait for the amount of data specified by VMIN in tenths of a second.
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 1;

        options.c_cflag |= CLOCAL;      // Ignore Modem control line
        options.c_cflag |= CREAD;       // Enable receiver

        if (tcsetattr(hComPort_, TCSANOW, &options) != 0)
        {
            PAL_showError("Failed to set COM port options");
        }

        // Start/Restart the thread
        if (!RFIDManagerThread_)
        {
            if ( tracing_ )
            {
                Debug::trace("RFIDManager", "creating new run thread");
            }
            RFIDManagerThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "RFIDManager Thread" );
        }
        else
        {
            if ( tracing_ )
            {
                Debug::trace("RFIDManager", "restarting run thread");
            }
            RFIDManagerThread_->RestartThread();
        }
    }   // End of for(i)
}


void RFIDManager::disconnect()
{
    Util::CRunnableClass::m_ExitEvent.EventSet();

    if ( RFIDManagerThread_->ThreadExitCode() == STILL_ACTIVE )
    {
        util_time::Time t0;

        RFIDManagerThread_->WaitForCompletion();

        util_time::Time t1;
        Debug::trace("RFIDManager", "disconnecting: had to wait for " +
                util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " ms. before trying to close handle");
    }


    if ( hComPort_ != PAL_INVALID_COM_HANDLE_VALUE )
    {
        if ( tracing_ )
            Debug::trace("RFIDManager", "disconnecting comport");

        if ( close(hComPort_) != 0 )
        {
            PAL_showError("Failed to close the handle");
        }

        hComPort_ = PAL_INVALID_COM_HANDLE_VALUE;
    }
}

DWORD RFIDManager::ReadBytesBlocking(DWORD bytesToRead, UINT8 * buffer, PAL_POVERLAPPED /* pOVL */, DWORD *pBytesRead)
{
    ssize_t bytes_read;

	try
	{
        bytes_read = read(hComPort_, buffer, bytesToRead);

        if (bytes_read < 0)
        {
            PAL_showError("Read bytes failed", errno);
            return COM_ERROR;
        }

        *pBytesRead = bytes_read;
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

DWORD RFIDManager::WriteBytesBlocking(DWORD bytesToWrite, UINT8* buffer, PAL_POVERLAPPED /* pOVL */, DWORD *pBytesWrote)
{
    ssize_t bytes_write;

	try
	{
        bytes_write = write(hComPort_, buffer, bytesToWrite);

        if (bytes_write < 0)
        {
            PAL_showError("Failed to Write bytes", errno);
            return COM_ERROR;
        }

        *pBytesWrote = bytes_write;

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

#endif  // __linux__
