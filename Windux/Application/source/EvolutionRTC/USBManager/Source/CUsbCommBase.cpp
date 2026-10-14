#if 1 // HACK HACK HACK
#define STATUS_DEVICE_NOT_CONNECTED      (0xC000009DL)
#endif // HACK HACK HACK
#include "CUsbCommBase.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CUsbCommBase::dataType = 0;
std::string CUsbCommBase::dataTypeName("CUsbCommBase");
#endif

CUsbCommBase::CUsbCommBase( SUsbDescriptors * descs, USBD_NODE_ID node ) : usbNode(node), usbDescriptors( descs ), pipesEnabled(false)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}

CUsbCommBase::~CUsbCommBase()
{
    pipeMap.clear();
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

std::string& CUsbCommBase::getDevicePath()
{
    return UsbManager::getInstance()->getDevicePath(usbNode);
}

void CUsbCommBase::createPipes()
{
    try
    {
        Util::CLockCriticalSection lock( pipeMapCS );
        for ( 
                uint32_t i = 0;
                i < usbDescriptors->endpointDescs.size();
                i++
            )
        {
            Util::CSmartPtr< CUsbPipe > pipe ( new CUsbPipe( this, usbDescriptors, i ) );
            CUsbPipe::EvolPipeType pipe_type = pipe->GetEvolPipeType();
            if ( pipe_type >= CUsbPipe::InPipe && pipe_type <= CUsbPipe::BulkPipe )
            {
                pipeMap[pipe_type] = pipe;
            }
        }
    }
    catch (...)
    {
		Debug::trace("Exception", std::string("CUsbCommBase::createPipes()"));
        LogCatchAndThrow;
    }
}

void CUsbCommBase::startPipes()
{
    try
    {
        Util::CLockCriticalSection lock( pipeMapCS );

        pipesEnabled = true;

        startAPipe(CUsbPipe::BulkPipe);
        startAPipe(CUsbPipe::InPipe);
    }
    catch (...)
    {
		Debug::trace("Exception", std::string("CUsbCommBase::startPipes()"));
        LogCatchAndThrow;
    }
}

void CUsbCommBase::startAPipe(CUsbPipe::EvolPipeType pipeType)
{
    std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::const_iterator iter = pipeMap.find( pipeType  );
    if ( iter != pipeMap.end() )
    {
        iter->second->startPipe();
    }
}

/*
   void CUsbCommBase::closePipes()
   {
   Util::CLockCriticalSection lock( pipeMapCS );
   std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::iterator iter = pipeMap.begin();

   for (; iter != pipeMap.end(); iter++)
   {
   CUsbPipe* p = iter->second;
   try
   {
   p->closePipeHandle();
   }
   catch (...)
   {
   LogCatchAndThrow;
   }
   }
   pipesEnabled = false;
   }
   */
/*
   void CUsbCommBase::stopPipes()
   {

//bulk and interrupt in pipes are stopped when their thread is stopped.
Util::CLockCriticalSection lock( pipeMapCS );

std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::iterator iter = pipeMap.begin();
for (; iter != pipeMap.end(); iter++)
{
try
{
CUsbPipe* p = iter->second;
p->stopPipe();
}
catch (...)
{
LogCatchAndThrow;
}
}

pipesEnabled = false;
}
*/
bool CUsbCommBase::sendUsbBulkMessage( const char * msg, uint32_t length )
{
    Util::CLockCriticalSection lock( pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::const_iterator iter = pipeMap.find( CUsbPipe::BulkPipe  );
    if (pipesEnabled && ( iter != pipeMap.end() ))
    {
        iter->second->sendUsbBulkMessage( msg, length );
        return true;
    }
    else
	{
		Debug::trace("CUsbCommBase.error", std::string("sendUsbBulkMessage():  PROBLEM!  pipesEnabled = ")
									+ std::string(Bool::toString(pipesEnabled))
									+ std::string(" ( iter != pipeMap.end() ) = ")
									+ std::string(Bool::toString(( iter != pipeMap.end() ))));
        return false;
	}
}

void CUsbCommBase::sendUsbInterruptMessage( const char * msg, uint32_t length, DWORD & result, bool & is_custom_code )
{
    Util::CLockCriticalSection lock( pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::const_iterator iter = pipeMap.find( CUsbPipe::OutPipe );
    if (pipesEnabled && ( iter != pipeMap.end() ))
        iter->second->sendUsbInterruptMessage( msg, length, result, is_custom_code );
    else
    {
		Debug::trace("CUsbCommBase.error", std::string("sendUsbInterruptMessage():  PROBLEM!  pipesEnabled = ")
									+ std::string(Bool::toString(pipesEnabled))
									+ std::string(" ( iter != pipeMap.end() ) = ")
									+ std::string(Bool::toString(( iter != pipeMap.end() ))));
        is_custom_code = true;
        result = CUsbPipe::PipeNotExist;
    }
}

void CUsbCommBase::cancelBulkTransfer()
{
    Util::CLockCriticalSection lock( pipeMapCS );

    std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::const_iterator iter = pipeMap.find( CUsbPipe::BulkPipe );
    if ( iter != pipeMap.end() )
        iter->second->cancelReadingWriting();
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
	Debug::trace("CUsbCommBase.error",
            std::string("HandlePipeError: node ") + std::string(Hex::toString((long long)usbNode))
            +std::string(" pipe type ") + std::string(Hex::toString((long)p->GetEvolPipeType()) )
            +std::string(" pipe handle ") + std::string(Hex::toString((long long)p->getPipeHandle()) )
            +std::string(" error ") + std::string(Hex::toString((long)error)) );

#if 1
    // For debugging device removal/stuck open pipes.
    // If any pipe error occurs, close all pipes associated with the
    // usb node.  Notify UsbManager to change the node state to DETACHED.

    // TODO: determine which errors occur ONLY on device removal

    Util::CLockCriticalSection lock( pipeMapCS );
    std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > >::iterator iter = pipeMap.begin();

    for (; iter != pipeMap.end(); iter++)
    {
        try
        {
            CUsbPipe* pipe = iter->second;
            pipe->closePipeHandle();
        }
        catch (...)
        {
			Debug::trace("Exception", std::string("CUsbCommBase::HandlePipeError()"));
            LogCatchAndThrow;
        }
    }

    pipesEnabled = false;
    UsbManager::getInstance()->DetachDevice(usbNode);
#endif
    return true;
}

