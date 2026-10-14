#include "PAL_platformheader.h"

#include "CUsbPipe.h"
#include "CUsbCommBase.h"
#include "MsgTime.h"
#include "evolution.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"
#include "CReadWriteObject.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CUsbPipe::dataType = 0;
	std::string CUsbPipe::dataTypeName("CUsbPipe");
#endif

#pragma warning (disable: 4311)

Util::CSmartCriticalSection CUsbPipe::criticalSection;

CUsbPipe::CUsbPipe(
		CUsbCommBase * base,
		SUsbDescriptors * descs,
		uint32_t i_pipe_no
		) : usbCommManager( base)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    Debug::trace("CUsbPipe", HobString ("CUsbPipe"));
    pipe_no = i_pipe_no;
    outgoingBulkMessages = NULL;
    pipeHandle = INVALID_HANDLE_VALUE;

    endPointDesc = descs->endpointDescs[pipe_no];

	unsigned __int32 addr = endPointDesc->bEndpointAddress;
	unsigned __int32 type = endPointDesc->bmAttributes & USB_ENDPOINT_TYPE_MASK;

    if ( USB_ENDPOINT_DIRECTION_OUT(addr)
            && type == USB_ENDPOINT_TYPE_INTERRUPT )
    {
        evolPipeType = OutPipe;
        MsgTimeout = OUT_WRITE_TIME_OUT;
    }
    else if ( USB_ENDPOINT_DIRECTION_IN(addr)
            && type == USB_ENDPOINT_TYPE_INTERRUPT )
    {
        evolPipeType = InPipe;
        MsgTimeout = READ_TIME_OUT;
    }
    else if ( type == USB_ENDPOINT_TYPE_BULK )
    {
        evolPipeType = BulkPipe;
        MsgTimeout = BULK_WRITE_TIME_OUT;
    }
    else
    {
        evolPipeType = UnknownPipe;
        MsgTimeout = 0;
    }

    pipeHandle = INVALID_HANDLE_VALUE;
    if ( evolPipeType != UnknownPipe )
    {
        std::string pipe_name = usbCommManager->getDevicePath();
        char str[20];
        sprintf( str, "\\PIPE0%d", pipe_no );
        pipe_name += str;

        // OPEN THE PIPE

        if (evolPipeType == InPipe)
        {
			//
            // open overlapped for read
            //

			pipeHandle = PAL_absclass::PAL_createFile(
					pipe_name.c_str(),
					PAL_FILE_GEN_WR|PAL_FILE_GEN_RD,
					PAL_FILE_SHARE_WR|PAL_FILE_SHARE_RD,
					(PAL_FILESEC_ATTRIB)NULL,
					PAL_FILE_OPEN,  //OPEN_EXISTING
					PAL_FILE_FLAG_OVERLAPPED,
					NULL );
        }//if (evolPipeType == InPipe)
        else
        {
            // open non-overlapped for write
			pipeHandle = PAL_absclass::PAL_createFile(
					pipe_name.c_str(),
					PAL_FILE_GEN_WR | PAL_FILE_GEN_RD,
					PAL_FILE_SHARE_WR | PAL_FILE_SHARE_RD,
					(PAL_FILESEC_ATTRIB)NULL,
					PAL_FILE_OPEN,
					PAL_FILE_ATTR_NORMAL,	//	HTA-31:  Was 0 but there was no Win definition for this.
					NULL );
        }
        if ( pipeHandle == INVALID_HANDLE_VALUE )
        {
            evolPipeType = InvalidPipe;
            Debug::trace("CUsbPipe", HobString ("failed to open pipe ") + std::string(Hex::toString((long)pipe_no)));
        }
        else
        {
            Debug::trace("CUsbPipe", HobString ("opened pipe ") + std::string(Hex::toString((long)pipe_no)) + HobString(", handle ") + std::string(Hex::toString((void*)pipeHandle)));
        }
    }//if ( evolPipeType != UnknownPipe )
    else
    {
        Debug::trace("CUsbPipe", HobString ("unknown pipe type") + std::string(Hex::toString((long)pipe_no)) + HobString(", handle ") + std::string(Hex::toString((void*)pipeHandle)));
    }
}

void CUsbPipe::closePipeHandle()
{
    Debug::trace("CUsbPipe", HobString ("closePipeHandle: handle ") + Hex::toString((long long)pipeHandle));

    try
    {
        Util::CLockCriticalSection lock( criticalSection );
        HANDLE p = pipeHandle;
        if ( p != INVALID_HANDLE_VALUE )
        {
            pipeHandle = INVALID_HANDLE_VALUE;
			PAL_absclass::PAL_closeHandle( p );
        }
    }
    catch(...)
    {
		Debug::trace("Exception", HobString ("CUsbPipe::closePipeHandle()"));
    }
}

CUsbPipe::~CUsbPipe()
{
	Debug::trace("CUsbPipe", HobString ("~CUsbPipe: handle ") + std::string(Hex::toString((void*)pipeHandle)) + ",  evolPipeType = " + Integer::toString(evolPipeType));

    try
    {
        //Util::CLockCriticalSection lock( criticalSection );
        if ( pipeHandle != INVALID_HANDLE_VALUE )
		{
			PAL_absclass::PAL_closeHandle( pipeHandle );
			pipeHandle = INVALID_HANDLE_VALUE;
		}
	}
	catch(...)
	{
		Debug::trace("Exception", "CUsbPipe::~CUsbPipe:  closing handle " + Hex::toString((void*)pipeHandle));
	}

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
	Debug::trace("CUsbPipe", "~CUsbPipe: Goodbye " + Hex::toString((void*)pipeHandle));
}

void CUsbPipe::startPipe()
{
    if (evolPipeType == InPipe)
    {
        Debug::trace("CUsbPipe", HobString ("Starting In pipe."));

        readWriteThread = new Util::CTLSThread( *this, THREAD_PRIORITY_HIGHEST, 0, "USB In Pipe Thread" );
    }
    else if (evolPipeType == BulkPipe )
    {
        Debug::trace("CUsbPipe", HobString ("Starting Bulk Out pipe."));

        readWriteThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "USB Bulk Pipe Thread" );
    }
}

CUsbPipe::EvolPipeType CUsbPipe::GetEvolPipeType()
{
    return evolPipeType;
}

void CUsbPipe::sendUsbBulkMessage(
        const char * msg,
		uint32_t length
        )
{

    Debug::trace("CUsbPipe.send.data", "sendUsbBulkMessage");
	//Util::CLockCriticalSection lock( criticalSection );		//	Now handled in send()
    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        usbCommManager->sendBulkMessageCallBack( ERROR_INVALID_HANDLE );
    }
    else
    {
        outgoingBulkMessages = msg;
        outgoingBulkMessageLength = length;
        bulkSendEvent.EventSet();
    }
}

// called by CUsbCommBase::cancelBulkTransfer
void CUsbPipe::cancelReadingWriting()
{
    cancelIoEvent.EventSet();
}

void CUsbPipe::sendUsbInterruptMessage(
		const char * msg,
		uint32_t length,
		DWORD & result,
		bool & is_custom_code
		)
{
    Debug::trace("CUsbPipe.send.data", "sendUsbInterruptMessage");
	//Util::CLockCriticalSection lock( criticalSection );		//	Now handled in send()
    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        result = ERROR_INVALID_HANDLE;
        is_custom_code = false;
		Debug::trace("CUsbPipe.send.data", "CUsbPipe::sendUsbInterruptMessage() INVALID PIPE HANDLE");
    }
    else
    {
        send( msg, length, false, result, is_custom_code );
    }
}

// implements CRunnableClass::Run()
DWORD CUsbPipe::Run()
{
    DWORD dwResult = 0;

	Debug::trace("CUsbPipe", HobString ("CUsbPipe::Run()"));
    if ( evolPipeType == OutPipe || evolPipeType == BulkPipe ) // interrupt out or bulk out
        dwResult = Run_BulkPipe();
    else if (evolPipeType == InPipe) // interrupt in 
        dwResult = Run_InPipe();
    else
    {
        // error
        Debug::trace("CUsbPipe", "FATAL - invalid Pipe type");
        dwResult = PAL_ERROR_BROKEN_PIPE;
    }

    return dwResult;
}

// macro to reset msg tracking items
#define RESET_MSG_BUF() \
    ptr = msg; \
    nMsgLength = nMsgHeaderLength; \
    nTotalBytes = 0; \
    bGotData = bGotHeader = false; \
    msgStartTime = 0; \
    memset(ptr, 0, nMsgBufferSize);


DWORD CUsbPipe::Run_InPipe()
{
	DWORD Result, WaitResult, Error;
	DWORD WaitTimeout = INFINITE;
	PAL_OVERLAPPED OverlappedRead  = {0};
	HANDLE WaitEvents[3] = {0};
	bool bRunning = true;

    bool traceRun = Debug::isTracing("CUsbPipe.run");
	Debug::trace("CUsbPipe", HobString ("CUsbPipe::Run_InPipe()"));

	// message buffer
	static const DWORD nMsgBufferSize = 8192;		//	PTR 1553: To match USB_MAX_TRANSFER_SIZE in WghUsb.h (windows driver). Was 4096.

    unsigned char * msg = new unsigned char[nMsgBufferSize];
    // ReadFile input buffer
    unsigned char * ptr;
    // size of the message header
    static const DWORD nMsgHeaderLength = 10;
    // number of data bytes expected for the message
    DWORD nMsgLength;
    // cumulative total for the current message
    DWORD nTotalBytes;
    // # bytes requested in ReadFile
    DWORD nBytesToRead;
    // # bytes returned by ReadFile
    DWORD nActualBytesRead;
    // snapshot of system timer when msg header was received
    // if 0, indicates no data has been received yet
    DWORD msgStartTime;
    // true = header has been received
    bool bGotHeader;
    // true = ReadFile or GetOverlappedResult returned data
    bool bGotData;

    // indices for WaitEvents[]
    const enum
    {
         EVT_EXIT
        ,EVT_CANCEL
        ,EVT_READ
        ,N_EVENTS
    };

	try
	{
		OverlappedRead.hEvent = PAL_absclass::PAL_createEvent(NULL, TRUE, FALSE, NULL);
		if (OverlappedRead.hEvent == 0)
		{
			Debug::trace("CUsbPipe.error", HobString ("CUsbPipe::Run_InPipe: failed to create wait handle for overlapped read"));
			throw Util::CReadException();
		}

        //
        // Initialize the array of wait events.
        //
        WaitEvents[EVT_EXIT] = m_ExitEvent.GetHandle();
        // Note, the exit event handle must be 0th element in the
        // array, so that it has priority over the other handles.

        WaitEvents[EVT_CANCEL] = cancelIoEvent.GetHandle();
        // signaled (in pipe) = cancel pending I/O
        // Note this event must occupy a lower array position than
        // the overlapped events so that it has priority over them.

        WaitEvents[EVT_READ] = OverlappedRead.hEvent;
        // signaled = overlapped read has completed

        RESET_MSG_BUF();

        while (bRunning)
        {
            if (pipeHandle == INVALID_HANDLE_VALUE)
            {
                Result = ERROR_INVALID_HANDLE;
                if (traceRun)
					Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe:  pipeHandle == INVALID_HANDLE_VALUE.  Calling RESET_MSG_BUF()"));
                RESET_MSG_BUF();
            }

            // Test the state of the terminate event.
            // If signaled, cancel I/O, set the exit flag, and
            // continue at the top of the while loop, which exits normally.

			if (PAL_absclass::PAL_waitForSingleObj(WaitEvents[EVT_EXIT], 0) == WAIT_OBJECT_0)
            {
                if (traceRun)
	                Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: exit event signaled"));
                if (pipeHandle != INVALID_HANDLE_VALUE)
                {
					PAL_absclass::PAL_cancelIo(pipeHandle);
                }
                bRunning = false;
                continue; // while (bRunning)
            }

            // Test the state of the cancelIoEvent. If signaled, cancel
            // I/O and do not attempt the read. Reset the message buffer
            // and restart at the top of the while loop.

			if (PAL_absclass::PAL_waitForSingleObj(WaitEvents[EVT_CANCEL], 0) == WAIT_OBJECT_0)
            {
                if (traceRun)
	                Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: cancelIoEvent signaled"));
                cancelIoEvent.EventReset();
                if (pipeHandle != INVALID_HANDLE_VALUE)
                {
					PAL_absclass::PAL_cancelIo(pipeHandle);
                }
                RESET_MSG_BUF();
                continue; // while (bRunning)
            }

            if (pipeHandle != INVALID_HANDLE_VALUE)
            {
                // Try to read an entire packet if available
                nBytesToRead = min(endPointDesc->wMaxPacketSize, (nMsgBufferSize-nTotalBytes) );

                //
                // Issue the overlapped read request.
                //

				BOOL readResult = PAL_absclass::PAL_readFile ( pipeHandle,
						(LPVOID) ptr,
						nBytesToRead,
						&nActualBytesRead,
						&OverlappedRead );

                if (readResult == false)
                {
					Error = PAL_absclass::PAL_getLastError();
                    if ((Error == ERROR_IO_PENDING) || (Error == ERROR_SUCCESS))	//	HTA-31
                    {
                        // If ERROR_IO_PENDING, got expected error indicating read request was
                        // successfully pended. Drop through and wait for
                        // the read event to be signaled.
//		                if (traceRun)
//							Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: ERROR_IO_PENDING"));

                        // readResult is false but Error is ERROR_SUCCESS - so no error. Keep going.
		                if (traceRun && (Error == ERROR_SUCCESS))
							Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: Result == false, Error == ERROR_SUCCESS. Keep going."));
                    }
                    else
                    {
                        // ReadFile returned an actual error.
		                if (traceRun)
							Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: Error = ") + Hex::toString(Error));
                        goto ErrorHandler;
                    }
                }
                else
                {
                    // Initial call to ReadFile returned data.
                    // Set flag so that WaitForMultipleObjects is
                    // bypassed and data is processed immediately.
                    bGotData = true;
                }
            }


            if (!bGotData)
            {
                //
                //   Wait for any of the events to be signaled
                //

				WaitResult = PAL_absclass::PAL_waitForMultipleObjs(N_EVENTS,WaitEvents,FALSE,WaitTimeout);

				switch (WaitResult)
				{
					case WAIT_OBJECT_0 + EVT_EXIT:
		                if (traceRun)
							Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: exit event signaled in wait"));

						PAL_absclass::PAL_cancelIo(pipeHandle);
						RESET_MSG_BUF();
						bRunning = false;
						break;

					case WAIT_OBJECT_0 + EVT_CANCEL:
		                if (traceRun)
							Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: cancelIoEvent signaled in wait"));
						cancelIoEvent.EventReset();

						PAL_absclass::PAL_cancelIo(pipeHandle);
						RESET_MSG_BUF();
						break;

					case WAIT_OBJECT_0 + EVT_READ:
						Result = GetOverlappedResult(
								pipeHandle,
								&OverlappedRead,
								&nActualBytesRead,
								TRUE);
						PAL_absclass::PAL_resetEvent(WaitEvents[EVT_READ]);

                        if (Result)
                        {
                            bGotData = true;
                        }
                        else
                        {
							Error = PAL_absclass::PAL_getLastError();
							goto ErrorHandler;
						}
						break;

					case WAIT_TIMEOUT:
		                if (traceRun)
							Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: wait timeout"));
						if (bGotHeader)
						{
							if ((PAL_absclass::PAL_getTickCount() - msgStartTime) > MsgTimeout )
							{
				                if (traceRun)
									Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: message read timeout, canceling"));
								Error = WAIT_TIMEOUT;
								goto ErrorHandler;
							}
						}
						break;

					case WAIT_FAILED:
						Error = PAL_absclass::PAL_getLastError();
		                if (traceRun)
	                        Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: wait failed, error ") + Hex::toString(Error));
                        goto ErrorHandler;
                        break;

                    default:
		                if (traceRun)
	                        Debug::trace("CUsbPipe.run", std::string("CUsbPipe::Run_InPipe: unknown wait result ") + Hex::toString(WaitResult));
                        Error = WaitResult;
                        goto ErrorHandler;
                }
            }

            if (bGotData)
            {
                // We received data from the device.

                // Accumulate the byte count
                nTotalBytes += nActualBytesRead;
                DBGASSERT(nTotalBytes <= nMsgBufferSize);

                if (0 == msgStartTime)
                {
                    // This is the start of a message.
                    // Capture the system timer as
                    // the start time of the message.

					msgStartTime = PAL_absclass::PAL_getTickCount();

                    // Reduce the wait timeout interval
                    // while reading the remainder of
                    // the message.
                    WaitTimeout = MsgTimeout/10;
                }
            }

            // Message parse loop. While we have sufficient
            // data in the buffer, look for a message header
            // followed by a message body. The header contains
            // the length of the message body.

            while (bGotData)
            {
                if (!bGotHeader)
                {
                    // if the header has not yet been processed,
                    // and we have received at least the length
                    // of the header, then examine the header to
                    // determine the length of the remainder of
                    // the message.

                    if (nTotalBytes >= nMsgHeaderLength)
                    {
                        //
                        // TODO: verify header data in CUsbPipe::Run_InPipe to ensure msg integrity and sync.
                        //

                        // We now have the header.
                        bGotHeader = true;

                        // Read the length of the message, not including header.
                        DWORD msgLen = (((DWORD)(msg[8] & 0xff)) << 8) | msg[9];
                        DBGASSERT(msgLen <= (nMsgBufferSize - nTotalBytes));

                        // Update the required # bytes for the current msg.
                        nMsgLength = nMsgHeaderLength + msgLen;
                        DBGASSERT(nMsgLength <= nMsgBufferSize);
                    }
                }
                else  // already scanned the header
                {
                    if ( nTotalBytes >= nMsgLength )
                    {
                        //
                        // Message completed, push out to system
                        //

                        // dump message bytes if enabled
                        if (Debug::isTracing("CUsbPipe.receive.data"))
                        {
                            std::string dump_msg(" -- Received USB Message:  ");
                            dump_msg += Hex::toString((const char *)msg, (int)nMsgLength);
                            Debug::trace("CUsbPipe.receive.data", dump_msg);
                        }
                        std::string s;
                        s.assign((const char *)msg, nMsgLength);
                        usbCommManager->processUsbMessage(s, evolPipeType );

                        // Shift any remaining data to start of buffer
                        memcpy(msg, msg + nMsgLength, (nMsgBufferSize - nMsgLength));

                        // Adjust total byte count and ReadFile pointer.
                        DBGASSERT(nTotalBytes >= nMsgLength);
                        nTotalBytes -= nMsgLength;
                        ptr = msg + nTotalBytes;

                        // Set required msg length to header size
                        // and clear the header flag.
                        nMsgLength = nMsgHeaderLength;
                        bGotHeader = false;

                        // Reset message timeout.
                        msgStartTime = 0;

                        // Restore wait timeout interval to default
                        // while waiting for a message header.
                        WaitTimeout = INFINITE;
                    }
                } // bGotHeader

                // If we don't have enough buffered data to
                // continue parsing the message, we're done here.
                // Clear the flag to exit the parse loop and
                // read some more data.

                if ( nTotalBytes < nMsgLength )
                {
                    bGotData = false;

                    // update ReadFile buffer location
                    ptr = msg + nTotalBytes;
                    DBGASSERT(ptr <= (msg + nMsgBufferSize));
                }

            } // bGotData

            continue; // while(bRunning)

ErrorHandler:
			// ReadFile returned an actual error. Cancel I/O, reset the
			// message buffer, and call the error handlers.  Restart
			// at top of the while loop.

			PAL_absclass::PAL_cancelIo(pipeHandle);
            RESET_MSG_BUF();
            if (Error != WAIT_TIMEOUT)
                usbCommManager->HandlePipeError(this, Error);
            usbCommManager->receiveErrorCallBack( evolPipeType, Error, false );
        } // bRunning

        if (traceRun)
	        Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: exiting normally "));
    }
    catch(Util::CTerminateNow)
    {
		Debug::trace("Exception", HobString ("CUsbPipe::Run_InPipe():  CTerminateNow caught - exiting"));
        Result =  1;
    }
    catch(...)
    {
		Debug::trace("Exception", HobString ("CUsbPipe::Run_InPipe():  Exiting with error"));
        if (traceRun)
	        Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: exception caught, exiting with error"));
        LogCatch;
        Result = (DWORD) -1;
    }

    if (msg)
        delete[] msg;

    return Result;
}

DWORD CUsbPipe::Run_BulkPipe()
{
    DWORD rc = 0;
    HANDLE WaitEvents[3] = {0};
    bool bRunning = true;

    // indices for WaitEvents[]
    //const 
    enum
    {
        EVT_EXIT,
        EVT_CANCEL,
        EVT_SEND,
        N_EVENTS
    };

    WaitEvents[EVT_EXIT] = m_ExitEvent.GetHandle();
    WaitEvents[EVT_CANCEL] = cancelIoEvent.GetHandle();
    WaitEvents[EVT_SEND] = bulkSendEvent.GetHandle();

	Debug::trace("CUsbPipe", HobString ("CUsbPipe::Run_BulkPipe()"));
    while (bRunning)
    {
        try
        {
            DWORD waitResult = PAL_absclass::PAL_waitForMultipleObjs(N_EVENTS,WaitEvents,FALSE,INFINITE);

            switch (waitResult)
            {
                case WAIT_OBJECT_0 + EVT_EXIT:
                    // exit event signaled
                    Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_BulkPipe: exit event signaled, exiting normally"));
                    bRunning = false;
                    break;

                case WAIT_OBJECT_0 + EVT_CANCEL:
                    Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_BulkPipe: cancel event signaled"));
                    //
                    // nothing to do here
                    //
                    cancelIoEvent.EventReset();
                    break;

                case WAIT_OBJECT_0 + EVT_SEND:
                    // bulk send event signaled
                    {
						//Util::CLockCriticalSection lock( criticalSection );	//	Now handled in send()

                        if (pipeHandle != INVALID_HANDLE_VALUE)
                        {
                            DWORD result = OK;
                            bool is_custom_code = false;
                            if ( outgoingBulkMessages != NULL )
                            {
                                send( outgoingBulkMessages, outgoingBulkMessageLength, true, result, is_custom_code );
                                outgoingBulkMessages = NULL;
                            }
                        }
                        bulkSendEvent.EventReset();
                    }
                    break;
            }
        }
        catch(Util::CTerminateNow)
        {
			Debug::trace("Exception", HobString ("CUsbPipe::Run_BulkPipe():  caught CTerminateNow exception - exiting"));
            rc = 1;
            bRunning = false;
        }
        catch(...)
        {
			Debug::trace("Exception", HobString ("CUsbPipe::Run_BulkPipe:  Exception, exiting with error"));
            rc = (DWORD) -1;
            bRunning = false;
        }
    }

    return rc;
}

void CUsbPipe::send(
		const char * msg,
		uint32_t length,
		bool is_bulk_msg,
		DWORD & result,
		bool & is_custom_code
		)
{
    try
    {
		Util::CLockCriticalSection lock( criticalSection );

        bool isTracing = Debug::isTracing("CUsbPipe.send.data");

		//	Prevent the bulk_msg indicator from changing from a subsequent send() call by setting it after the lock is achieved.
		bool bulk_msg = is_bulk_msg;

        if ( !bulk_msg )
        {
            if (isTracing)
            {
                HobString dump_msg(" -- Begin to Send USB Message:  ");
                dump_msg += Hex::toString( msg, length, true );

                Debug::trace("CUsbPipe.send.data", dump_msg);
            }
        }
		else
		{
			//	EPP printer bug PATCH for PTR 1422 - slowing down the bulk pipe transmission.
			//	The printer code will need to be changed to better handle the speed
			//	that Windows sends the bulk data. Apparently vxWorks and Linux don't transmit as fast.
			//	Aaron wants it to be clear that this is a patch - not a bug fix for 1422.
			//	4/12/12	- Per Carlos Guzman and Tom Fink, change the delay from 2ms to 11ms to insure
			//			  the printer has enough time to get set up to receive the data. According to Tom,
			//			  this will not require further work or code change to the printer.
			Sleep(11); 
		}

        DWORD total_bytes_to_send = length;
        const char * ptr = msg;
        if ( total_bytes_to_send > 0 ) //need to send
        {
            if (isTracing)
				Debug::trace("CUsbPipe.send.data", "Got into need to send");
            Util::CMSGTime send_start_time = Util::CMSGTime::GetTheCurrentTime();
            DWORD sent_bytes = 0;
            UINT32 maxPacket = 16384;
            bool sent_header;
            bool bSending = true;

            if ( evolPipeType == BulkPipe)
                sent_header = true;
            else
                sent_header = false;

            while ( bSending )
            {
                DWORD write_size = total_bytes_to_send - sent_bytes;
                if ( !sent_header )
                {
                    write_size = 10;
                    sent_header = true;
                }
                else if ( write_size > maxPacket )
                    write_size = maxPacket;

	            if (isTracing)
	                Debug::trace("CUsbPipe.send.data", "Send data to driver");

                if (pipeHandle == INVALID_HANDLE_VALUE)
                {
					Debug::trace("CUsbPipe.error", "send():  pipeHandle == INVALID_HANDLE_VALUE.  Set result = ERROR_INVALID_HANDLE");
                    result = ERROR_INVALID_HANDLE;
                    is_custom_code = false;
                    // don't process pipe error if handle closed
                    //usbCommManager->HandlePipeError(this, result);
                    if ( bulk_msg )
                        usbCommManager->sendBulkMessageCallBack( result );
                    bSending = false;
                    continue;
                }

                if (cancelIoEvent.IsEventSet())
                {
					Debug::trace("CUsbPipe.error", "send():  CancelIoEvent is set.  Set result = PAL_ERROR_OPERATION_ABORTED");
                    cancelIoEvent.EventReset();
                    result = PAL_ERROR_OPERATION_ABORTED;
                    is_custom_code = false;
                    // don't process pipe error if handle closed
                    //usbCommManager->HandlePipeError(this, result);
                    if ( bulk_msg )
                        usbCommManager->sendBulkMessageCallBack( result );
                    bSending = false;
                    continue;
                }

                DWORD current_sent_bytes = 0;
				BOOL writeResult = PAL_absclass::PAL_writeFile( pipeHandle, (LPVOID)(ptr + sent_bytes), write_size, &current_sent_bytes, NULL );

                if (writeResult)
				{
					sent_bytes += current_sent_bytes;
				}
				else
				{
					result = PAL_absclass::PAL_getLastError();
					Debug::trace("CUsbPipe.error", "send():  Failed on writeFile. result = " + Integer::toString(result));
                    is_custom_code = false;
                    usbCommManager->HandlePipeError(this, result);
                    if ( bulk_msg )
                        usbCommManager->sendBulkMessageCallBack( result );
                    break;
                }
                if ( sent_bytes < total_bytes_to_send )
                {
                    Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
					uint64_t elapse_time = ( ct - send_start_time ).GetSpanInMilliseconds();
                    if ( ( evolPipeType == OutPipe && elapse_time > OUT_WRITE_TIME_OUT ) ||
                            ( evolPipeType == BulkPipe && elapse_time > BULK_WRITE_TIME_OUT ) )
                    {
#ifndef CHECK_FT
					Debug::trace("CUsbPipe.error", "send():  TIMED OUT - Doing nothing here.");
                    /* PTR 1147 - the system hits the time out often on initialization - let WriteFile deal with the USB device detaching
						result = TimeOut;
						is_custom_code = true;
						if ( bulk_msg )
							usbCommManager->sendBulkMessageCallBack( result, is_custom_code );
						break;
					*/
#else
			            if (isTracing)
							Debug::trace("CUsbPipe.send.data", "TIMED OUT");
						result = TimeOut;
						is_custom_code = true;
						// don't process pipe error on timeout
						// usbCommManager->HandlePipeError(this, result);
						if ( bulk_msg )
							usbCommManager->sendBulkMessageCallBack( result, is_custom_code );
						break;
#endif
                    }
                }
                else
                {
                    result = OK;

					if ( !bulk_msg )
					{
			            if (isTracing)
							Debug::trace("CUsbPipe.send.data", HobString (" -- Sent USB Message Completely! "));
					}

                    is_custom_code = false;
                    if ( bulk_msg )
                        usbCommManager->sendBulkMessageCallBack( 0 );
                    break;
                }
            }
        }
    }
    catch (...)
    {
		Debug::trace("Exception", "CUsbPipe::send()");
        LogCatchAndThrow;
    }
}
