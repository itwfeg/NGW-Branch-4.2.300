#if 1
#define DEBUG_ME
#endif

#define STATUS_DEVICE_NOT_CONNECTED      (0xC000009DL)
#include "CUsbCommBase.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CUsbCommBase::dataType = 0;
std::string CUsbCommBase::dataTypeName("CUsbCommBase");
#endif

CUsbCommBase::CUsbCommBase( SUsbDescriptors * descs, USBD_NODE_ID node ) : usbNode(node), usbDescriptors( descs ), pipesEnabled(false)
{
    Debug::trace("CUsbCommBase", "CUsbCommBase():  Entry");
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    libusb_handle = NULL;		//	Initialize this puppy.

	bRestoreKernelDriver = false;

    pthread_mutexattr_t attr;

    int mutexResult = pthread_mutexattr_init(&attr);
	if (mutexResult != 0)
	{
	    Debug::trace("CUsbCommBase.error", "pthread_mutexattr_init(&attr) returned " + std::string(Integer::toString(mutexResult)));
	}

    mutexResult = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	if (mutexResult != 0)
	{
	    Debug::trace("CUsbCommBase.error", "pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) returned " + std::string(Integer::toString(mutexResult)));
	}

    mutexResult = pthread_mutex_init(&pipeMapCS, &attr);
	if (mutexResult != 0)
	{
	    Debug::trace("CUsbCommBase.error", "pthread_mutex_init(&pipeMapCS, &attr) returned " + std::string(Integer::toString(mutexResult)));
	}

    mutexResult = pthread_mutexattr_destroy(&attr);
	if (mutexResult != 0)
	{
	    Debug::trace("CUsbCommBase.error", "pthread_mutexattr_destroy() returned " + std::string(Integer::toString(mutexResult)));
	}
    Debug::trace("CUsbCommBase", "CUsbCommBase():  Exit");
}

CUsbCommBase::~CUsbCommBase()
{
    Debug::trace("CUsbCommBase", "~CUsbCommBase():  Entry");
	CUsbPipe *pipe;

    // shutdown pipe threads
    pthread_mutex_lock( &pipeMapCS );

    // stop all pipes
    for ( std::map<CUsbPipe::EvolPipeType, CUsbPipe*>::iterator i( pipeMap.begin() ); i != pipeMap.end(); ++i )
    	if ( pipe = i->second )
    		delete pipe;

    pipeMap.clear();

    pthread_mutex_unlock( &pipeMapCS );

    // release libusb
    if ( libusb_handle )
    {
		libusb_release_interface(libusb_handle,0);

		if (bRestoreKernelDriver)
			libusb_attach_kernel_driver(libusb_handle, 0);

		libusb_close(libusb_handle);
		libusb_handle = NULL;
    }

    // destroy our mutex
    pthread_mutex_destroy(&pipeMapCS);

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
    Debug::trace("CUsbCommBase", "~CUsbCommBase():  Exit");
}

std::string& CUsbCommBase::getDevicePath()
{
    return UsbManager::getInstance()->getDevicePath(usbNode);
}

void CUsbCommBase::createPipes()
{
    Debug::trace("CUsbCommBase", "createPipes():  Begin");

    try
    {
    	int lResult;

    	// open the device
        if ( 0 != (lResult = libusb_open( usbDescriptors->dev, &libusb_handle )) )
		    Debug::trace("CUsbCommBase.error", "createPipes():  Problem! libusb_open() failed. " + Integer::toString(lResult));
        else
        {
		    Debug::trace("CUsbCommBase", "createPipes():  libusb_open():  libusb_handle = " + Hex::toString((long long)libusb_handle));

		    // check for kernel driver attached to device
		    if ( 1 == (lResult = libusb_kernel_driver_active( libusb_handle, 0 )) )
            {
			    Debug::trace("CUsbCommBase", "createPipes():  libusb_kernel_driver_active");
                bRestoreKernelDriver = true;

                // detach it...
                libusb_detach_kernel_driver( libusb_handle, 0 );
            }

		    Debug::trace("CUsbCommBase", "createPipes():  libusb_kernel_driver_active returned " + Integer::toString(lResult));

		    // claim the device
		    if ( 0 != (lResult = libusb_claim_interface( libusb_handle, 0 )) )
            {
			    Debug::trace("CUsbCommBase.error", "createPipes():  Problem! libusb_claim_interface() failed. " + Integer::toString(lResult));
            }
		    else
		    {
		    	Debug::trace("CUsbCommBase", "createPipes():  libusb_claim_interface returned " + Integer::toString(lResult));

                int epts = 0;

                if ( usbDescriptors )
                	if (( usbDescriptors->dev_config ) && ( 0 < usbDescriptors->dev_config->bNumInterfaces ))
                		if (( usbDescriptors->dev_config->interface ) && ( 0 < usbDescriptors->dev_config->interface->num_altsetting ))
                			epts = usbDescriptors->dev_config->interface->altsetting->bNumEndpoints;

                if ( epts )
                {
                    pthread_mutex_lock( &pipeMapCS );

                    // attach pipe to each endpoint
					for ( int i = 0; i < epts; i++)
					{
						// create the pipe
						CUsbPipe *pipe = new CUsbPipe( this,
									&usbDescriptors->dev_config[0].interface[0].altsetting[0].endpoint[i],
									i);

						if (pipe)
						{
						    Debug::trace("CUsbCommBase", "createPipes():  new CUsbPipe = " + std::string(Hex::toString((long long)pipe)));
							// set the protocol for the pipe based on the device product id
							pipe->setPipeProtocol( usbDescriptors->dev_desc.idProduct );

							// add to mapping
							pipeMap[ pipe->GetEvolPipeType() ] = pipe;
						}
					}

					pthread_mutex_unlock( &pipeMapCS );
                }
            }
        }
    }
    catch (...)
    {
		Debug::trace("CUsbCommBase.error", HobString("createPipes():  catch (...)"));
        LogCatchAndThrow;
	    Debug::trace("CUsbCommBase.error", "createPipes():  Exception!");
    }

    Debug::trace("CUsbCommBase", "createPipes():  Done");
}

void CUsbCommBase::startPipes()
{
    Debug::trace("CUsbCommBase", "startPipes():  Begin");
    try
    {
        pthread_mutex_lock( &pipeMapCS );

        pipesEnabled = true;

        startAPipe(CUsbPipe::BulkPipe);
        startAPipe(CUsbPipe::InPipe);
        pthread_mutex_unlock( &pipeMapCS );
    }
    catch (...)
    {
		Debug::trace("CUsbCommBase.error", HobString("startPipes():  catch (...)"));
        LogCatchAndThrow;
	    Debug::trace("CUsbCommBase.error", "startPipes():  Exception!");
    }
    Debug::trace("CUsbCommBase", "startPipes():  Done");
}

void CUsbCommBase::startAPipe(CUsbPipe::EvolPipeType pipeType)
{
    std::map< CUsbPipe::EvolPipeType, CUsbPipe * >::const_iterator iter = pipeMap.find( pipeType  );
    if ( iter != pipeMap.end() )
    {
        iter->second->startPipe();
    }
}

bool CUsbCommBase::sendUsbBulkMessage( const char * msg, uint32_t length )
{
    bool retVal = false;
    pthread_mutex_lock( &pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, CUsbPipe * >::const_iterator iter = pipeMap.find( CUsbPipe::BulkPipe  );
    if (pipesEnabled && ( iter != pipeMap.end() ))
    {
        iter->second->sendUsbBulkMessage( msg, length );
        retVal = true;
    }
    pthread_mutex_unlock( &pipeMapCS );
    return retVal;
}

void CUsbCommBase::sendUsbInterruptMessage( const char * msg, uint32_t length, DWORD & result, bool & is_custom_code )
{
    pthread_mutex_lock( &pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, CUsbPipe * >::const_iterator iter = pipeMap.find( CUsbPipe::OutPipe );
    if (pipesEnabled && ( iter != pipeMap.end() ))
        iter->second->sendUsbInterruptMessage( msg, length, result, is_custom_code );
    else
    {
        is_custom_code = true;
        result = CUsbPipe::PipeNotExist;
    }
    pthread_mutex_unlock( &pipeMapCS );

    if( result == CUsbPipe::PipeNotExist )//Check if the pipe exists
        Debug::trace("CUsbCommBase", "sendUsbInterruptMessage():  PIPE DOES NOT EXIST!");
}

void CUsbCommBase::cancelBulkTransfer()
{
    pthread_mutex_lock( &pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, CUsbPipe * >::const_iterator iter = pipeMap.find( CUsbPipe::BulkPipe );
    if ( iter != pipeMap.end() )
        iter->second->cancelReadingWriting();

    pthread_mutex_unlock( &pipeMapCS );
}

void CUsbCommBase::printUSBError( DWORD result, bool user_defined_error )
{
    Debug::trace("CUsbCommBase",
            std::string("printUSBError: node ") + std::string(Hex::toString((long long)usbNode))
            + std::string(" result ") + std::string(Hex::toString(result))
            + std::string( user_defined_error ? " (user defined)":" " )
            );
}
//returns true to indicate all pipes were closed.
bool CUsbCommBase::HandlePipeError( CUsbPipe* p, DWORD error )
{
    Debug::trace("CUsbCommBase",
            std::string("HandlePipeError: node ") + std::string(Hex::toString((long long)usbNode))
            +std::string(" pipe type ") + std::string(Hex::toString((long)p->GetEvolPipeType()) )
            +std::string(" pipe handle ") + std::string(Hex::toString((long long)p->getPipeHandle()) )
            +std::string(" error ") + std::string(Hex::toString((long)error)) );

#if 1
    // For debugging device removal/stuck open pipes.
    // If any pipe error occurs, close all pipes associated with the
    // usb node.  Notify UsbManager to change the node state to DETACHED.

    // TODO: determine which errors occur ONLY on device removal

    pthread_mutex_lock( &pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, CUsbPipe * >::iterator iter = pipeMap.begin();

    for (; iter != pipeMap.end(); iter++)
    {
        try
        {
            CUsbPipe* pipe = iter->second;
            pipe->closePipeHandle();
        }
        catch (...)
        {
			Debug::trace("CUsbCommBase.error", HobString("HandlePipeError():  catch (...)"));
            LogCatchAndThrow;
		    Debug::trace("CUsbCommBase.error", "HandlePipeError():  Exception!");
        }
    }
    pthread_mutex_unlock( &pipeMapCS );

    pipesEnabled = false;
    UsbManager::getInstance()->DetachDevice(usbNode);
#endif
    return true;
}

