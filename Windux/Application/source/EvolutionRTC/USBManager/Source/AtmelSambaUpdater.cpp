#include "AtmelSambaUpdater.h"
#include "AtmelDeviceHandler.h"
#include "UsbManager.h"
#include "Debug.h"
#include "Hex.h"
#include "FCError.h"
#include "HobString.h"
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AtmelSambaUpdater::_dataType_ = 0;
std::string AtmelSambaUpdater::_dataTypeName_("AtmelSambaUpdater"); 
#endif

AtmelSambaUpdater::AtmelSambaUpdater(USBD_NODE_ID nodeId) :
    IUsbCodeUpdater(nodeId, 0) // usbDeviceType == don't care
{
    Debug::trace("AtmelSambaUpdater", HobString("constructor for usb node ") + Hex::toString(nodeId));

    // Relay the usb node arrival to the device handler.
    AtmelDeviceHandler::getInstance()->DeviceArrival(nodeId);

    // Signal the UsbManager that it's ok to delete this instance 
    // after this USB node has been removed. The device handler 
    // continues to update its nodes regardless.
    //UsbManager::getInstance()->DetachDevice(nodeId);

    // all done -- AtmelDeviceHandler class does the rest

//////////////////////////////////
//UPDATER_SYNC
    PAL_absclass::PAL_releaseMutex(update_lock);
/////////////////////////////////

#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);
    util_acc::addInstance(_dataType_);
#endif
}

AtmelSambaUpdater::~AtmelSambaUpdater()
{
    Debug::trace("AtmelSambaUpdater", HobString("destructor for usb node ") + Hex::toString(usbNodeId_));

    // Relay the usb node removal to the device handler.
    AtmelDeviceHandler::getInstance()->DeviceRemoval(usbNodeId_);

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

// Dummy implementation to resolve the IUsbCodeUpdater interface.
// We don't register or send messages, so nothing required here.
void AtmelSambaUpdater::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    UNREFERENCED_PARAMETER(msg);
    Debug::trace("AtmelSambaUpdater", HobString("processEvent ") + msg->CStr());
}


// Dummy implementation to resolve the IUsbCodeUpdater interface.
// We don't register or send messages, so nothing required here.
void AtmelSambaUpdater::handleVersionManagerDevEvent(
    VersionManagerEvent verManagerEvent, 
    const char *UNUSED(recvBuffer))
{
    UNREFERENCED_PARAMETER(recvBuffer);

    Debug::trace("AtmelSambaUpdater", HobString("ver mgr event 0x") + Hex::toString(verManagerEvent));

}



