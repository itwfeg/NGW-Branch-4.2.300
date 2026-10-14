/**
 * @file RS485Manager.cpp
 * @copyright Copyright (c) 2018 Hobart Corportation
 * @author Aaron Swift
 * @brief Responsible for sending label data over RS485
 * @addtogroup CommManagers
 * @{
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#if defined ( __linux__ )		// Windows 
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#include <string>
#include "RS485Manager.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterLabelTransferReady.h"
#include "PrinterRS485FlowControl.h"
#include "PrinterCommand.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"


RS485Manager *RS485Manager::pInstance = 0;
// BCM board
//const std::string RS485Manager::UART = "/dev/ttymxc3";
// ARA / ARK board
const std::string RS485Manager::UART = "/dev/ttymxc4";


RS485Manager::RS485Manager()
{
    Util::CLockCriticalSection lock( initLock );

    initialized_ 	= false;
    busy_ 			= false;
    lastTransfer_ 	= false;
    reSend_ 		= false;
	gotFlowControl_ = false;

    state_ 			= prevState_ = _Init;
    imageSize_ 		= 0;

    pBfrBase_		= nullptr;
    pBfr_ 			= nullptr;

    reSendCntr_		= 0;
    stationId_		= 99; 		//set to unused printer type.
    bytesXfrd_      = 0;

	memset( &rBfr[0], 0, sizeof(rBfr) );

#if defined ( __linux__ )		// Windows 
	//open port for writes only
    tty_fd = open( UART.data(), O_WRONLY | O_NONBLOCK  );
    if( tty_fd < 0 ) {
        Debug::trace( "RS485Manager.init", "failed to open UART: " + UART );
    } else {
        //get the UART configuration
        if( tcgetattr( tty_fd, &uart_config ) < 0 ) {
            Debug::trace( "RS485Manager.init", "failed to get UART configuration." );
        } else {
            //set up UART for 3Mbps 8n1 transmit rate
            uart_config.c_oflag = 0;
            uart_config.c_iflag = 0;
            uart_config.c_cflag = CS8|CLOCAL;

            //set the baud rate to 2Mbps synchronous
            if( cfsetspeed( &uart_config, _2000000_BAUD ) < 0 ) {
                Debug::trace( "RS485Manager.init", "failed to set the new UART speed." );
            } else {
                //remove the current configuration
                if( tcflush(tty_fd, TCIOFLUSH) < 0) {
                    Debug::trace( "RS485Manager.init", "failed to flush UART configuration." );
                } else {
                    //set the new configuration
                    if( tcsetattr( tty_fd, TCSANOW,  &uart_config ) < 0 ) {
                        Debug::trace("RS485Manager.init", "failed to set the new UART configuration." );
                    } else {
                        Debug::trace("RS485Manager.init", "UART configuration set." );

                        registerEvent( RS485Mgr, PrRS485FlowControl );
                        registerEvent( RS485Mgr, PrLabelTransferReady);

                        pThread_ = new Util::CTLSThread( *this, 1, 0, "RS485Manager" );
                        if( pThread_ != NULL ) {
                        	initialized_ = true;
                        } else {
                        	Debug::trace("RS485Manager.init", "Failed to create thread." );
                        }
                    }
                }
            }
        }
    }
#endif
}


RS485Manager *RS485Manager::getInstance()
{
	if ( !pInstance )
	{
		pInstance = new RS485Manager();

		if ( !pInstance )
			Debug::trace("RS485Manager", "getInstance(): pInstance is NULL!");

		//if the interface does not initialize then the instance was not created
		else if ( !pInstance->isinitialized_() )
		{
			delete pInstance;
			pInstance = 0;
		}
	}

	return pInstance;
}


RS485Manager::~RS485Manager()
{
	Debug::trace("RS485Manager", "~RS485Manager() -- Entry");

#if defined ( __linux__ )
	//close my interface
	if (( 0 <= tty_fd ) && ( close( tty_fd ) < 0 ))
		Debug::trace( "RS485Manager.Error", "~RS485Manager(): failed to close RS485 handle." );
#endif

	deregisterAllEvents();
	ShutdownNicely();

	// tell the thread to exit
	m_ExitEvent.EventSet();

	// wait for the thread to exit before deleting all the resources!
	if ( pThread_ )
	{
		while ( pThread_->IsRunning() )
			PAL_absclass::PAL_sleep(1);

	        delete pThread_;
	        pThread_ = nullptr;
	}

	if ( pBfr_ )
	{
		//delete the image buffer
		delete[] pBfrBase_;
		pBfrBase_ = pBfr_ = nullptr;
	}


	if ( pInstance )
	{
		delete pInstance;
		pInstance = 0;
	}

	Debug::trace("RS485Manager", "~RS485Manager() -- Exit");
}


bool RS485Manager::isinitialized_()
{
    return initialized_;
}

DWORD RS485Manager::Run()
{
	Debug::trace("RS485Manager", "Entering thread");

	int sleepTimer;
	
	do
	{
		sleepTimer = 1;

		if ( prevState_ != state_ )
		{
			Debug::trace("RS485Manager.StateMachine", "State Changed -- LastState: " + Integer::toString( prevState_) + " New State " + Integer::toString(state_));
			prevState_ = state_;
		}

		switch ( state_ )
		{
		case _Init:
#if 0
			if(!labelQueue.empty())
			{
				Debug::trace("RS485Manager", "Popping data off the queue");
				LabelTransfer transfer = labelQueue.front();
				labelQueue.pop();
				assignLabelImage(transfer.id, transfer.pImage, transfer.size, transfer.xferSize, transfer.complete);
				delete transfer.pImage;
				state_ = _Transfer;
			}
#endif
			sleepTimer = 10;
			break;

		case _Transfer:
			Debug::trace("RS485Manager", "Transfer payload to printer");
			//transfer the payload to the printer
			transfer();
			Debug::trace("RS485Manager", "Wait for flow control");
			state_ = _WaitForReady;
			break;

		case _WaitForReady:
			//wait for flow control from printer
			if ( gotFlowControl_ )
			{
				gotFlowControl_ = false;

				if ( !lastTransfer_ )
				{
					Debug::trace("RS485Manager", "Changing states: _CheckTransferStatus");
					state_ = _CheckTransferStatus;
				}
				else
				{
					Debug::trace("RS485Manager", "Image transfer complete. Total bytes sent: " + Integer::toString(bytesXfrd_));
					if ( pBfr_ != nullptr )
					{
						//delete the image buffer
						delete[] pBfrBase_;
						pBfrBase_ = pBfr_ = nullptr;
					}
					//clear our busy flag
					busy_ = false;

					//clear our last transfer flag
					lastTransfer_ = false;

					imageSize_ = 0;
					bytesXfrd_ = 0;

					state_ = _Init;
				}
			}

			break;

		case _CheckTransferStatus:
			if ( !reSend_ )
			{
				//deduct the last transfer from the overall image size
				imageSize_ -= ( transferSize_ - CHECK_SUM_SIZE );

				//keep track of bytes sent without errors
				bytesXfrd_ += ( transferSize_ - CHECK_SUM_SIZE );
				Debug::trace("RS485Manager", "bytesXfrd_: " + Integer::toString(bytesXfrd_));

				//set counters for next chunk.
				pBfr_ += ( transferSize_ - CHECK_SUM_SIZE );

				//are we at the end?
				if ( imageSize_ >= ( (long) transferSize_ - CHECK_SUM_SIZE ) )
					state_ = _Transfer;
				else
				{
					Debug::trace("RS485Manager", "Last chunk to transfer: " + Integer::toString(imageSize_));

					//image size is less than our transfer size
					lastTransfer_ = true;
					state_ = _Transfer;
				}
			}
			else
			{
				Debug::trace("RS485Manager", "reSend " + Integer::toString(reSend_));
			}

			break;
		}

		// yield thread
		PAL_absclass::PAL_sleep( sleepTimer );

	} while ( !m_ExitEvent.IsEventSet() );

	return 0;
}

void RS485Manager::processEvent( Util::CSmartPtr<EvolMessageWrapper> &msg )
{
	Util::CLockCriticalSection lock( csLock );

	if( msg ) {
		switch ( msg->getDeviceType() )
		{
			case RS485Mgr:
				
				// flow control message received change states
				if( msg->getEventType() == PrRS485FlowControl ) {
					const char *pBuffer = msg->Data();

					PrinterRS485FlowControl flowcntl( pBuffer );
					reSend_ = flowcntl.isResend();
					if( reSend_ ) {
						reSendCntr_++;
						Debug::trace("RS485Manager.Error", "Resend transfer! Total re-sends: " + Integer::toString(reSendCntr_));
					}
					gotFlowControl_ = true;
				}

				if( msg->getEventType() == PrLabelTransferReady ) {
					Debug::trace("RS485Manager", "PrLabelTransferReady: start transfer to printer.");
					PrinterLabelTransferReady ready( msg->Data() );

					//is this ready message from the printer we are processing?
					if( stationId_ == ready.getStationId() ) {
						//station is ready to receive label data
						if( ( imageSize_ > 0) && ( pBfr_ != nullptr ) ){
							state_ = _Transfer;
						} else {
							Debug::trace("RS485Manager.Error", "Changing states: _Transfer");
						}
					}else
					{
						Debug::trace("RS485Manager.Error", "Incorrect station ID");
					}
					
				}
			break;
			default:
			break;
		}

	} else {
        Debug::trace( "RS485Manager.Error", "Received NULL message" );
	}
}

bool RS485Manager::assignLabelImage( unsigned int id,  unsigned char *pImage, unsigned long size, unsigned long xferSize, bool complete  )
{
	bool result = false;

	Util::CLockCriticalSection lock( csLock );

	Debug::trace("RS485Manager", "assignLabelImage():  size: " + Integer::toString(size) + " xferSize: " + Integer::toString(xferSize));

	stationId_ = id;
	complete_ = complete;
	unsigned long byteSize = size;
	if( ( pImage != nullptr ) && ( byteSize > 0 ) ) {
		/* SOF-3852 if we were on our last transfer and have not heard the 
		   handshake yet just continue with next label */
		if( ( lastTransfer_ == true ) && ( imageSize_ == 0 ) ) {
			Debug::trace("RS485Manager.Error", "Just continue with next label");
			
			lastTransfer_ = false;
			busy_ = false;
			bytesXfrd_ = 0;
			state_ = _Init;
		} 
		
		if( !busy_ ) {
			busy_ = true;

			pBfr_ = new unsigned char[byteSize];

			pBfrBase_ = pBfr_;

			if( pBfr_ != nullptr ) {
				memset( pBfr_, 0, byteSize );
				memcpy( pBfr_, pImage, byteSize );

#if 0
			Debug::trace("PrinterManager.Flip", "pHeelBuffer: " + Hex::toString((long long)pImage) );
			int numLines = ( byteSize / ( 448 / 8 ) );
			int lineSize = 28;
			UINT16 *pStart = (UINT16 *)pImage;
			for( int x = 0; x < numLines; x++ ) {
				std::string str = "";
				for(int i = 0; i < lineSize; i++ ) {
					
					str.append( Hex::toString( *pStart++ ) );
				}
				Debug::trace("PrinterManager.Flip", "RS485Manager: " + str );
			}
#endif

				//set our overall image size
				imageSize_ = byteSize;

				//is the image size smaller than a transfer?
				if( imageSize_ <= (long) xferSize ) {					
					lastTransfer_ = true;
				}

				//set our max transfer size (data + 4byte checksum)
				transferSize_ =  xferSize;

				result = true;
			} else {
				Debug::trace("RS485Manager.Error", "assignLabelImage(): Failed to create buffer!!! ");
			}
		} else {
			Debug::trace("RS485Manager.Error", "assignLabelImage(): busy processing label image! ");
		#if 0
			unsigned char * temp = new unsigned char[byteSize];
			if( temp != nullptr ) {
				memset( temp, 0, byteSize );
				memcpy( temp, pImage, byteSize );
			}

			LabelTransfer val;
			val.id = id;
			val.complete = complete;
			val.size = size;
			val.xferSize = xferSize;
			val.pImage = temp;
			labelQueue.push(val); 
		#endif	
		}
	} else {
		Debug::trace("RS485Manager.Error", "assignLabelImage(): pBfr_ = " + Integer::toString( (long long)pBfr_ ) + "size: " + Integer::toString( size ));
	}
	return result;
}

void RS485Manager::transfer()
{

#if defined ( __linux__ )
	//setup the payload to transfer to printer
	unsigned char payload[transferSize_] = {0};
	
	if( !lastTransfer_ ) {
		//leave room for the checksum
		memcpy( &payload[0], pBfr_, ( transferSize_ - CHECK_SUM_SIZE ) );
	} else {
		memcpy( &payload[0], pBfr_, ( imageSize_ ) );
		bytesXfrd_ += imageSize_;
		pBfr_ -= ( transferSize_ - CHECK_SUM_SIZE );
		//final load
		imageSize_ = 0;
	}

	unsigned long chk = 0;
	//calc and append checksum
	for( int i = 0; i < ( transferSize_ - CHECK_SUM_SIZE ); i++ ) {
		chk += payload[i];
	}
	//Debug::trace("RS485Manager", "transfer() checksum: 0x" + Hex::toString(chk));

	memcpy( &payload[( transferSize_ - CHECK_SUM_SIZE )], &chk, sizeof( chk ) );

	int x = 0, size = transferSize_;
	Debug::trace("RS485Manager", "Transefer Size " + Integer::toString(size));
	do {

		int nBytes = write( tty_fd, &payload[x], size );
		//don't wait on the last transfer
		if( !lastTransfer_ ) {
			//wait until the transmission is complete
			tcdrain( tty_fd );
			Debug::trace("RS485Manager", "Drained..-..-");
		}
		Debug::trace("RS485Manager", "transfer() nBytes: " + Integer::toString(nBytes));

		//did we transfer it all?
		if( nBytes < transferSize_ ) {
			x += nBytes;
			size -= nBytes;
		} else {
			size = 0;
		}
	} while( size > 0 );
#endif
}

RS485Manager::RS485States RS485Manager::getState(){
	return state_;
}


//THIS SHOULD ONLY BE CALLED DURING BOARD SITE TESTING AND SHOULD NO BE REFERENCED AT ANY OTHER TIME!
void RS485Manager::transferTestPattern()
{

#if defined ( __linux__ )
	unsigned char 	tstPattern[4090] = {0xAA};
	
	int x = 0, size = 4090;
	Debug::trace("RS485Manager", "transferTestPattern() Transefer Size " + Integer::toString(size));
	do {
		
		lastTransfer_ = false;

		int nBytes = write( tty_fd, &tstPattern[x], size );
		//don't wait on the last transfer
		if( !lastTransfer_ ) {
			//wait until the transmission is complete
			tcdrain( tty_fd );
			Debug::trace("RS485Manager", "Drained..-..-");
		}
		Debug::trace("RS485Manager", "transferTestPattern() nBytes: " + Integer::toString(nBytes));

		//did we transfer it all?
		if( nBytes < transferSize_ ) {
			x += nBytes;
			size -= nBytes;
		} else {
			size = 0;
		}
	} while( size > 0 );

#endif
}


