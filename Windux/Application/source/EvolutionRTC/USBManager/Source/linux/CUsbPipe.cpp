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

Util::CSmartCriticalSection CUsbPipe::criticalSection;

CUsbPipe::CUsbPipe(
        CUsbCommBase * base,
        const libusb_endpoint_descriptor * descs,
        uint32_t i_pipe_no
        ) : usbCommManager(base), ept(descs), pipe_no(i_pipe_no)
{

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    outgoingBulkMessages = NULL;


    uint8_t addr = ept->bEndpointAddress;
    uint8_t type = ept->bmAttributes & USB_ENDPOINT_TYPE_MASK;



    if ( USB_ENDPOINT_DIRECTION_OUT(addr)
            && type == USB_ENDPOINT_TYPE_INTERRUPT )
    {
        Debug::trace("CUsbPipe", HobString ("CUsbPipe():Interrupt out pipe"));
        evolPipeType = OutPipe;
        MsgTimeout = OUT_WRITE_TIME_OUT;
    }
    else if ( USB_ENDPOINT_DIRECTION_IN(addr)
            && type == USB_ENDPOINT_TYPE_INTERRUPT )
    {
        Debug::trace("CUsbPipe", HobString ("CUsbPipe():Interrupt in pipe"));
        evolPipeType = InPipe;
        MsgTimeout = READ_TIME_OUT;
    }
    else if ( type == USB_ENDPOINT_TYPE_BULK )
    {
        Debug::trace("CUsbPipe", HobString ("CUsbPipe():Bulk pipe"));
        evolPipeType = BulkPipe;
        MsgTimeout = BULK_WRITE_TIME_OUT;
    }
    else
    {
        evolPipeType = UnknownPipe;
        MsgTimeout = 0;
    }

    //default the protocol to hlx,access
    pipeProtocol_ = HLX_Access_Protocol;

    handle = base->libusb_handle;
    readWriteThread = NULL;
}

void CUsbPipe::closePipeHandle()
{
    Debug::trace("CUsbPipe", HobString ("closePipeHandle(): handle = ") + Hex::toString(HandleToULong(handle)));
    handle = (libusb_device_handle *)INVALID_HANDLE_VALUE;
}

CUsbPipe::~CUsbPipe()
{
    Debug::trace( "CUsbPipe",
    	HobString("~CUsbPipe: handle ") +
    	Hex::toString( (long) handle ) +
    	HobString(" type ") +
    	Integer::toString( evolPipeType ) );

    try
    {
    	if ( readWriteThread )
   		{
    		if ( readWriteThread->IsRunning() )
			{
				readWriteThread->StopThread();
				readWriteThread->WaitForCompletion(INFINITE);
			}

            delete readWriteThread;
            readWriteThread = NULL;
        }

        handle = (libusb_device_handle *)INVALID_HANDLE_VALUE;
    }
    catch(...)
    {
        Debug::trace("CUsbPipe", HobString ("~CUsbPipe(): exception when closing handle ") + Hex::toString((long)handle));
    }

    Debug::trace("CUsbPipe", HobString ("~CUsbPipe(): Exit "));
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

void CUsbPipe::startPipe()
{
    if (evolPipeType == InPipe)
    {
        Debug::trace("CUsbPipe", HobString ("Starting In pipe."));

        readWriteThread = new Util::CTLSThread( *this, 5, 0, "USB In Pipe Thread" );

    }
    else if (evolPipeType == BulkPipe )
    {
        Debug::trace("CUsbPipe", HobString ("Starting Bulk Out pipe."));
	/*bump thread priority to resolve streaming 8" label issue 
        readWriteThread = new Util::CTLSThread( *this, 0, 0, "USB Bulk Pipe Thread" );
	*/   	
        readWriteThread = new Util::CTLSThread( *this, 1, 0, "USB Bulk Pipe Thread" );
	
    }
}

CUsbPipe::EvolPipeType CUsbPipe::GetEvolPipeType()
{
    return evolPipeType;
}

/*pipe protocol is based on the device product id.  */
void CUsbPipe::setPipeProtocol(unsigned short productID)
{
	switch(productID)
	{
		case HOB_LSMN_PROD_ID:
		{
			Debug::trace("CUsbPipe", "setPipeProtocol(LSM_Protocol)");
			pipeProtocol_ = LSM_Protocol;
			break;
		}
		case HOB_ACCESS_WEIGHER_BL_PROD_ID:
		case HOB_ACCESS_WEIGHER_PROD_ID:
		case HOB_ACCESS_PRINTER_BL_PROD_ID:
		case HOB_ACCESS_PRINTER_PROD_ID:
		case HOB_HLX_APC_BL_PROD_ID:
		case HOB_HLX_APC_PROD_ID:
		case HOB_ACCESS_ARM_WGH_BL_PROD_ID:
		case HOB_ACCESS_ARM_WGH_PROD_ID:
		case HOB_ACCESS_ARM_PTR_BL_PORD_ID:
		case HOB_ACCESS_ARM_PRINTER_PROD_ID:
		case HOB_ACCESS_WRAPPER_PROD_ID:
		case HOB_ACCESS_WRAPPER_BL_PROD_ID:
		case HOB_ACCESS_CLA_PROD_ID:
		case HOB_ACCESS_CLA_BL_PROD_ID:
		{
			Debug::trace("CUsbPipe", "setPipeProtocol(HLX_Access_Protocol)");
			pipeProtocol_ = HLX_Access_Protocol;
			break;
		}
		case HOB_FASTRACK_FPC:
		{
			Debug::trace("CUsbPipe", "setPipeProtocol(Fastrack_Protocol)");
			pipeProtocol_ = Fastrack_Protocol;
			break;
		}
		default:
		{
			pipeProtocol_ = NO_Protocol;
			Debug::trace("CUsbPipe.Error", "FATAL - invalid Pipe protocol");
		}
	}
}

void CUsbPipe::sendUsbBulkMessage( const char * msg, uint32_t length)
{

    //Util::CLockCriticalSection lock( criticalSection );
    if (handle == INVALID_HANDLE_VALUE)
    {
        usbCommManager->sendBulkMessageCallBack( ERROR_INVALID_HANDLE );
    }
    else
    {

#if 0
    	while(bulkSendEvent.IsEventSet() != 0)
    	{
    		PAL_absclass::PAL_sleep(1);
    	}
#endif
        outgoingBulkMessages = msg;
        outgoingBulkMessageLength = length;


        DWORD result = OK;
		bool is_custom_code = false;
        if ( outgoingBulkMessages != NULL )
		{

			send( outgoingBulkMessages, outgoingBulkMessageLength, true, result, is_custom_code );
			outgoingBulkMessages = NULL;
			outgoingBulkMessageLength = 0;
		}


#if 0
        if(!bulkSendEvent.EventSet())
        {
        	Debug::trace("PrinterManager.Xfer", "Signal Bulk Send Event ERROR!!!");
        	PAL_absclass::PAL_sleep(10);
        	bulkSendEvent.EventSet();
        }
#endif
    }
}

// called by CUsbCommBase::cancelBulkTransfer
void CUsbPipe::cancelReadingWriting()
{
    Debug::trace("CUsbPipe.send.data", "cancelReadingWriting");
    cancelIoEvent.EventSet();
}

void CUsbPipe::sendUsbInterruptMessage(
        const char * msg,
        uint32_t length,
        DWORD & result,
        bool & is_custom_code
        )
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        result = ERROR_INVALID_HANDLE;
        is_custom_code = false;
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

    if ( evolPipeType == OutPipe || evolPipeType == BulkPipe ) // interrupt out or bulk out
        dwResult = Run_BulkPipe();
    else if (evolPipeType == InPipe) // interrupt in
        dwResult = Run_InPipe();
    else
    {
        // error
        Debug::trace("CUsbPipe.Error", "FATAL - invalid Pipe type");
        dwResult = PAL_ERROR_BROKEN_PIPE;
    }

    return dwResult;
}



DWORD CUsbPipe::Run_InPipe()
{
    static const DWORD nMsgBufferSize( 65545 );	//max body size plus 10 bytes header
    static const DWORD nMsgHeaderLength( 10 );

    unsigned char msg[nMsgBufferSize];
    DWORD nTotalBytes( 0 );

    int Result;

    //
    // Initialize the array of wait events.
    //

    HANDLE hExitEvent( m_ExitEvent.GetHandle() );
    HANDLE hCancelIoEvent( cancelIoEvent.GetHandle() );

    Debug::trace("CUsbPipe.Sync", HobString("Start Pipe "));
	Debug::trace("CUsbPipe.Sync", HobString("Run_InPipe():   handle: ") + Hex::toString((long)handle) + " this: " + Hex::toString((long)this) );

    for ( ;; )
    {
    	bool bGotData( false );

        if ( WAIT_OBJECT_0 == PAL_absclass::PAL_waitForSingleObj( hExitEvent, 0 ) )
        {
            Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: exit event signaled"));
            break;
        }
        else if ( WAIT_OBJECT_0 == PAL_absclass::PAL_waitForSingleObj( hCancelIoEvent, 0 ) )
        {
            Debug::trace("CUsbPipe.run", HobString ("CUsbPipe::Run_InPipe: cancelIoEvent signaled"));

            nTotalBytes = 0;
            continue;
        }
        else if ( INVALID_HANDLE_VALUE == handle )
        {
            Debug::trace("CUsbPipe.Error", HobString ("CUsbPipe::Run_InPipe: bad handle"));
            break;
        }

		int nBytesToRead( MIN(ept->wMaxPacketSize, (nMsgBufferSize-nTotalBytes) ) );
		int nActualBytesRead( 0 );

		Result = libusb_interrupt_transfer( handle, ept->bEndpointAddress, &msg[nTotalBytes], nBytesToRead, &nActualBytesRead, 500 );

		if ( LIBUSB_SUCCESS == Result )
		{
			// Accumulate the byte count
			nTotalBytes += nActualBytesRead;
			bGotData = true;
		}
		else if( LIBUSB_ERROR_TIMEOUT == Result )
		{
			Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():   libusb_interrupt_transfer returned LIBUSB_ERROR_TIMEOUT"));

			//if the transfer count is not zero, the data is valid!!!
			if ( nActualBytesRead )
			{
				nTotalBytes += nActualBytesRead;
				bGotData = true;

				//added for debug
				Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():   handle: ") + Hex::toString((long)handle) + HobString ("TimeOut!!! Data: ") + Hex::toString((const char*) msg, nTotalBytes, true ));
			}
		}
		else if (( LIBUSB_ERROR_NO_DEVICE == Result ) || ( LIBUSB_ERROR_OTHER == Result ))
		{
			Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():   libusb_interrupt_transfer returned LIBUSB_ERROR_NO_DEVICE"));
			break;
		}
		else if ( LIBUSB_ERROR_IO == Result )
		{
			Debug::trace("CUsbPipe.Error", "Run_InPipe():   libusb_interrupt_transfer returned LIBUSB_ERROR_IO");
			break;
		}
		else
		{
			Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():  handle = ") + Hex::toString((long)handle) + ", Result = " + Integer::toString(Result));
		}

		// no data to process; yield thread
		if ( !bGotData )
			continue;

		for ( ;; )
		{
			if ( Debug::isTracing("CUsbPipe.Message") )
				Debug::trace("CUsbPipe.Message", HobString("Run_InPipe():   buffer: ") + Hex::toString((const char*) msg, nTotalBytes, true ));

			// no header yet...
			if ( nTotalBytes < nMsgHeaderLength )
				break;

			// error correction of message
			if ( !usbCommManager->isValidHeader( (const char*) msg ) )
			{
				Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():   bad header: ") + Hex::toString((const char*) msg, nTotalBytes, true ));

				unsigned int address( 0 );

				// try to locate next valid header
				if ( usbCommManager->locateNextHeader( (const char*) msg, nTotalBytes, address ) )
				{
					Debug::trace("CUsbPipe.Error", HobString("Run_InPipe():   found new header at address ") + Integer::toString(address));

					memmove( &msg[0], &msg[address], nTotalBytes - address );
					nTotalBytes -= address;

					continue;
				}
			}

			// if the header has not yet been processed,
			// and we have received at least the length
			// of the header, then examine the header to
			// determine the length of the remainder of
			// the message.

			// Read the length of the message, not including header.
			const DWORD msgLen = (((DWORD)(msg[8] & 0xff)) << 8) | msg[9];
			DBGASSERT(msgLen <= (nMsgBufferSize - nTotalBytes));

			// Update the required # bytes for the current msg.
			const DWORD nMsgLength = nMsgHeaderLength + msgLen;
			DBGASSERT(nMsgLength <= nMsgBufferSize);

			// no body yet...
			if ( nTotalBytes < nMsgLength )
				break;

			//
			// Message completed, push out to system
			//

			Debug::trace("CUsbPipe.run", HobString("CUsbPipe::Run_InPipe: process message len ") + Integer::toString(nMsgLength) + " [" + Integer::toString(nTotalBytes) + "]");

			std::string s( (const char*) msg, nMsgLength );
			usbCommManager->processUsbMessage( s, evolPipeType );

			// more data!!!
			if ( nMsgLength < nTotalBytes )
				memmove( &msg[0], &msg[nMsgLength], nTotalBytes - nMsgLength );

			nTotalBytes -= nMsgLength;
		}
    }

    return Result;
}

DWORD CUsbPipe::Run_BulkPipe()
{
    DWORD rc = 0;
    HANDLE WaitEvents[3] = {0};

    bool bRunning = true;

    enum
    {
        EVT_EXIT,
        EVT_CANCEL,
        EVT_SEND,
        N_EVENTS
    };

    WaitEvents[EVT_EXIT] 	= m_ExitEvent.GetHandle();
    WaitEvents[EVT_CANCEL] 	= cancelIoEvent.GetHandle();
    WaitEvents[EVT_SEND] 	= bulkSendEvent.GetHandle();

    while (bRunning)
    {
        try
        {
            DWORD waitResult = PAL_absclass::PAL_waitForMultipleObjs(N_EVENTS,WaitEvents,FALSE,INFINITE);

            switch (waitResult)
            {
            	case WAIT_OBJECT_0 + EVT_EXIT:
            	{
                    Debug::trace("CUsbPipe.Error", HobString ("CUsbPipe::Run_BulkPipe: exit event signaled, exiting normally"));
                    bRunning = false;
                    break;
            	}
                case WAIT_OBJECT_0 + EVT_CANCEL:
            	{
                    Debug::trace("CUsbPipe.Error", HobString ("CUsbPipe::Run_BulkPipe: cancel event signaled"));
                    //
                    // nothing to do here
                    //
                    cancelIoEvent.EventReset();
                    break;
            	}
                case WAIT_OBJECT_0 + EVT_SEND:
                {
					#if 0 //do we really need this to be a thread?
					if (handle != INVALID_HANDLE_VALUE)
					{
						DWORD result = OK;
						bool is_custom_code = false;
						if ( outgoingBulkMessages != NULL )
						{
							Debug::trace("PrinterManager.Xfer", "Pipe Run Calling Send");
							send( outgoingBulkMessages, outgoingBulkMessageLength, true, result, is_custom_code );
							outgoingBulkMessages = NULL;
							outgoingBulkMessageLength = 0;

						}
						else
						{
							Debug::trace("CUsbPipe.Error", "Bulk Buffer is NULL!!!!");
						}
					}
					Debug::trace("PrinterManager.Xfer", "Clear Bulk Send Event");
					#endif

					bulkSendEvent.EventReset();
                }
                break;
            }

        }
        catch(Util::CTerminateNow)
        {
            Debug::trace("CUsbPipe.Error", HobString ("CUsbPipe::Run_BulkPipe: caught CTerminateNow exception, exiting"));
            rc = 1;
            bRunning = false;
        }
        catch(...)
        {
            Debug::trace("CUsbPipe.Error", HobString ("CUsbPipe::Run_BulkPipe: caught exception, exiting with error"));
            rc = (DWORD) -1;
            bRunning = false;
        }
    }

    return rc;
}

//TO DO: check interrupt xfer size to the endpoint and split if needed.
void CUsbPipe::send(
	const char *msg,
	uint32_t length,
	bool bulk_msg,
	DWORD &result,
	bool &is_custom_code )
{
	if (( !msg ) || ( !length ))
	{
		Debug::trace("CUsbPipe.Sync", HobString("Bad message or length!"));
		return;
	}

	if ( bulk_msg )
        Debug::trace("CUsbPipe.send.data"," -- Bulk Out");
	else if ( Debug::isTracing("CUsbPipe.send.data") )
	{
		Debug::trace("CUsbPipe.send.data", HobString(" -- Interrupt Out: ") +
			Hex::toString( msg, length, true ) );
    }

    try
    {
    	Util::CLockCriticalSection lock( criticalSection );

       	const Util::CMSGTime send_start_time( Util::CMSGTime::GetTheCurrentTime() );

       	uint32_t index( 0 );
       	uint32_t write_size( 10 ); // send header first
       	uint32_t retry( 0 );

        if (( Fastrack_Protocol == pipeProtocol_ ) || ( BulkPipe == evolPipeType ))
        	write_size = length;

        for ( ;; )
        {
			if ( INVALID_HANDLE_VALUE == handle )
			{
				Debug::trace("CUsbPipe.Error", "ERROR: INVALID_HANDLE_VALUE!");

				result = ERROR_INVALID_HANDLE;
				is_custom_code = false;

				break;
			}
			else if ( cancelIoEvent.IsEventSet() )
			{
				Debug::trace("CUsbPipe.Error", "Cancel Event is set, aborting operation!");
				cancelIoEvent.EventReset();

				result = PAL_ERROR_OPERATION_ABORTED;
				is_custom_code = false;

				break;
			}
			else if ( 3 <= retry )
			{
				Debug::trace("CUsbPipe.Error", "Too many attempts, aborting operation!");

				result = PAL_ERROR_OPERATION_ABORTED;
				is_custom_code = false;

				break;
			}

			int result( 0 );
			int written( 0 );

        	if ( Debug::isTracing("CUsbPipe.send.data") )
        	{
        		Debug::trace("CUsbPipe.send.data", HobString("Send data to driver: ") +
        			Hex::toString( &msg[index], write_size, true ) );
            }


			if ( bulk_msg )
			{
				result = libusb_bulk_transfer(
					handle,
					ept->bEndpointAddress,
					(unsigned char*) &msg[index],
					write_size,
					&written,
					CUsbPipe::BULK_WRITE_TIME_OUT );
			}
			else
			{
				result = libusb_interrupt_transfer(
					handle,
					ept->bEndpointAddress,
					(unsigned char*) &msg[index],
					write_size,
					&written,
					CUsbPipe::OUT_WRITE_TIME_OUT );
			}

			if (( LIBUSB_SUCCESS == result ) || ( LIBUSB_ERROR_TIMEOUT == result ))
			{
				if ( LIBUSB_ERROR_TIMEOUT == result )
				{
					Debug::trace("CUsbPipe.Sync", HobString("OUT PIPE TIMEOUT!!!!  Written: ") +
						Integer::toString(written) );
				}

				index += written;

				if ( !written )
					++retry;
				else if ( length <= index )
				{
					result = OK;
					is_custom_code = false;

					break;
				}
				else
				{
					if ( 0 == (write_size -= written) )
						write_size = (length - index);

					retry = 0;
				}
			}
			//Cancel the message if there is no device handle
			else if ( LIBUSB_ERROR_NO_DEVICE == result )
            {
				Debug::trace("CUsbPipe.Sync", HobString("OUT PIPE NO DEVICE!!!!"));

		        result = OK;
                break;
            }
			else
			{
				const Util::CMSGTime timeOutTime( Util::CMSGTime::GetTheCurrentTime() );

				char StartTime[64];
				sprintf( StartTime, "%2d:%.2d:%.3d", send_start_time.GetMinute(), send_start_time.GetSecond(), send_start_time.GetMilliseconds() );

				char EndTime[64];
				sprintf( EndTime, "%2d:%.2d:%.3d", timeOutTime.GetMinute(), timeOutTime.GetSecond(), timeOutTime.GetMilliseconds() );

				Debug::trace("CUsbPipe.Sync",
					HobString("OUT PIPE ERROR!!!!  ") + Integer::toString(result) +
					HobString(" h: ") + Hex::toString((long)handle) +
					HobString(" Index: ") + Integer::toString(index) +
					HobString(" Write size: ") + Integer::toString(write_size) +
					HobString(" Buffer Addr: ") + Hex::toString((long)msg) +
					HobString(" Buffer: ") + Hex::toString(msg, length, true ) +
					HobString(" Total bytes: ") + Integer::toString(length) +
					HobString(" Start Time: ") + StartTime +
					HobString(" End Time: ") + EndTime );

				++retry;
			}
        }

        if ( bulk_msg )
            usbCommManager->sendBulkMessageCallBack( result );

        Debug::trace("CUsbPipe.send.data", HobString(" -- Sent USB Message Result: ") + Integer::toString(result));
    }
    catch ( ... )
    {
        Debug::trace("CUsbPipe.Error", "send():  catch (...)");
        LogCatchAndThrow;
    }
}
