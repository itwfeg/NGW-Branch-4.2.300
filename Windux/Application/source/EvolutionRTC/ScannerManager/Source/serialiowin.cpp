/**
 * @file serialiowin.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "serialiowin.h"

#ifndef __linux__

#include <process.h>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int __stdcall _SerialCommThreadFn( void *param )
{
    serialComm *obj( reinterpret_cast<serialComm*>( param ) );

    SetEvent( obj->threadStarted_ );
    obj->run();
    ResetEvent( obj->threadStarted_ );

    ExitThread( 0 );

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serialComm::serialComm() :
    comm_( INVALID_HANDLE_VALUE ),
    thread_( INVALID_HANDLE_VALUE ),
    threadStarted_( INVALID_HANDLE_VALUE ),
    threadTerminated_( INVALID_HANDLE_VALUE ),
    m_( INVALID_HANDLE_VALUE )
{
    threadStarted_ = CreateEvent( NULL, TRUE, FALSE, NULL );
    threadTerminated_ = CreateEvent( NULL, TRUE, FALSE, NULL );

    m_ = CreateMutex( NULL, FALSE, NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serialComm::~serialComm()
{
    // close device
    close();

    close( threadTerminated_ );
    close( threadStarted_ );

    close( m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::isValid() const
{
    return (INVALID_HANDLE_VALUE != comm_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::close()
{
    stop();
    close( comm_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::open( const std::string &portName, DWORD baudRate, BYTE parity, BYTE stopBits, BYTE byteSize )
{
    DCB dcb = {0};
    COMMTIMEOUTS timeouts = {0};

    // already running
    if ( isValid() )
        return true;

    // reset events
    ResetEvent( threadStarted_ );
    ResetEvent( threadTerminated_ );

    // open COM port
    if ( INVALID_HANDLE_VALUE == (comm_ = CreateFileA( portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0 )) )
    {
        std::cerr << "failed to create COM port!" << std::endl;
        goto error;
    }

    // set comm mask
    if ( !SetCommMask( comm_, EV_RXCHAR | EV_TXEMPTY ) )
    {
        std::cerr << "failed to set comm mask!" << std::endl;
        goto error;
    }

    // set baud rate, parity, etc...
    dcb.DCBlength = sizeof(DCB);

    if ( !GetCommState( comm_, &dcb ) )
    {
        std::cerr << "failed to get comm state!" << std::endl;
        goto error;
    }

    dcb.BaudRate = baudRate;
    dcb.ByteSize = byteSize;
    dcb.Parity = parity;

    if ( 1 == stopBits )
        dcb.StopBits = ONESTOPBIT;
    else if ( 2 == stopBits )
        dcb.StopBits = TWOSTOPBITS;
    else
        dcb.StopBits = ONE5STOPBITS;

    if ( !SetCommState( comm_, &dcb ) )
    {
        std::cerr << "failed to get comm state!" << std::endl;
        goto error;
    }

    // set timeouts (we control the timeout overselves using WaitFor)
    timeouts.ReadIntervalTimeout = MAXDWORD;

    if ( !SetCommTimeouts( comm_, &timeouts ) )
    {
        std::cerr << "failed to set comm timeouts!" << std::endl;
        goto error;
    }

    // go!
    if ( start() )
        return true;

error:

    close( comm_ );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string serialComm::read()
{
    std::string result;

    // grab buffered data
    if ( WAIT_OBJECT_0 == WaitForSingleObject( m_, INFINITE ) )
    {
        result = buffer_;
        buffer_.clear();

        ReleaseMutex( m_ );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::write( const std::string &buffer )
{
    bool result( false );

    // make sure valid
    if ( isValid() )
    {
        OVERLAPPED ov = {0};
        ov.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

        DWORD bytesWritten( 0 );

        // write
        if ( !WriteFile( comm_, buffer.c_str(), (DWORD) buffer.length(), &bytesWritten, &ov ) )
        {
            if ( ERROR_IO_PENDING == GetLastError() )
            {
                // wait for completion
                result = (WAIT_OBJECT_0 == WaitForSingleObject( ov.hEvent, COMM_WAIT_TIMEOUT_WRITE ));
            }
            else
            {
                result = (buffer.length() == bytesWritten);
            }
        }

        close( ov.hEvent );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::isRunning() const
{
    return (WAIT_OBJECT_0 == WaitForSingleObject( threadStarted_, 0 ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::run()
{
    DWORD eventMask( 0 );

    OVERLAPPED ov = {0};
    ov.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    HANDLE handles[2];
    handles[0] = threadTerminated_;
    handles[1] = ov.hEvent;

    for ( ;; )
    {
        // set comm event
        if ( !WaitCommEvent( comm_, &eventMask, &ov ) )
            if ( ERROR_IO_PENDING != GetLastError() )
            {
                std::cerr << "failed to set comm event!" << std::endl;
                goto done;
            }

        // wait...
        switch ( WaitForMultipleObjects( 2, handles, FALSE, INFINITE ) )
        {
        // terminate event set
        case WAIT_OBJECT_0:
            goto done;

        // overlapped io event set
        case WAIT_OBJECT_0+1:
            {
                DWORD mask;

                CHAR buffer[1024];
                DWORD bytesRead( 0 );

                OVERLAPPED ovRead = {0};
                ovRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

                // check for tx queue empty
                if (( GetCommMask( comm_, &mask ) ) && ( EV_TXEMPTY == mask ))
                    ; // nothing to do

                // read device
                else if ( !ReadFile( comm_, buffer, sizeof(buffer), &bytesRead, &ovRead ) )
                {
                    std::cerr << "failed to read from device!" << std::endl;

                    close( ovRead.hEvent );
                    goto done;
                }

                // append to buffer
                else if ( 0 < bytesRead )
                {
                    if ( WAIT_OBJECT_0 == WaitForSingleObject( m_, INFINITE ) )
                    {
                        buffer_.append( buffer, bytesRead );
                        ReleaseMutex( m_ );
                    }
                }

                close( ovRead.hEvent );
            }

            ResetEvent( ov.hEvent );
            break;

        default:
            std::cerr << "bad WFMO result!" << std::endl;
            goto done;
        }
    }

done:

    close( ov.hEvent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::start()
{
    // start thread
    thread_ = (HANDLE) _beginthreadex( NULL, 0, _SerialCommThreadFn, (void*) this, 0, NULL );

    return (WAIT_OBJECT_0 == WaitForSingleObject( threadStarted_, INFINITE ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::stop()
{
    // stop thread
    if ( isRunning() )
        SignalObjectAndWait( threadTerminated_, thread_, INFINITE, FALSE );

    close( thread_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::close( HANDLE &handle )
{
    if ( INVALID_HANDLE_VALUE == handle )
        return;

    if ( !CloseHandle( handle ) )
        std::cerr << "failed to close handle: " << handle << std::endl;

    invalidate( handle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::invalidate( HANDLE &handle )
{
    handle = INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // __linux__
