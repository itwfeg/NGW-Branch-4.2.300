#include "EvolMessage.h"
#include "EvolWeigherCodeUpdater.h"
#include "IPeripheralUpdater.h"

#include "VersionManagerEvent.h"
#include "PeripheralCodeUpdate.h"
#include "PeripheralRequestCodeUpdate.h"
#include "VmStatusMsg.h"
#include "CypressEzUsbUpdater.h"

#include "filesys/file.h"
#include "filesys/directory.h"

#include "UsbManager.h"
#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWeigherCodeUpdater::dataType = 0;
std::string EvolWeigherCodeUpdater::dataTypeName("EvolWeigherCodeUpdater"); 
#endif

EvolWeigherCodeUpdater::EvolWeigherCodeUpdater(USBD_NODE_ID usbNodeId,
        UINT16 usbDeviceType,
        HobVersion hardwareVersion) : 
    IUsbCodeUpdater(usbNodeId, usbDeviceType)
{
#if 0
    Debug::trace("EvolWeigherCodeUpdater", HobString("hardwareVersion: ") + HobString(hardwareVersion.getRevisionString()));
#endif
	Debug::trace("EvolWeigherCodeUpdater", HobString("EvolWeigherCodeUpdater my Mutex: ") + Integer::toString64((long long)update_lock) );
#if defined ( __linux__ )
    pCommMgr_ = new CypressEzUsbUpdaterLinux(usbNodeId, usbDeviceType);
#else
    pCommMgr_ = new CypressEzUsbUpdater(usbNodeId, usbDeviceType);
#endif
    forceQuit = false;

    PeripheralRequestCodeUpdate reqCodeUpdate(usbNodeId, hardwareVersion, HobVersion(HobVersion::UsbWeigher15KgSW, 0, 0));
    dispatchEvent(&reqCodeUpdate);

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);
    util_acc::addInstance(dataType);
#endif
}

EvolWeigherCodeUpdater::~EvolWeigherCodeUpdater()
{
    Debug::trace("EvolWeigherCodeUpdater", HobString("Destructor Called"));

    delete pCommMgr_;

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}


void EvolWeigherCodeUpdater::handleVersionManagerDevEvent(VersionManagerEvent verManagerEvent, const char *recvBuffer)
{
    VmStatusMsg status;

    switch (verManagerEvent)
    {
        case PeripheralUpdateCode:
		{
            Debug::trace("EvolWeigherCodeUpdater", HobString("handleVersionManagerDevEvent():  Received PeripheralUpdateCode"));
            PeripheralCodeUpdate codeUpdate(recvBuffer);
			if(codeUpdate.getNodeId() == usbNodeId_)
			{
				//ownership of the mutex
				if (WAIT_OBJECT_0 != PAL_absclass::PAL_waitForSingleObj(update_lock, 0))
					Debug::trace("EvolWeigherCodeUpdater", "Failed to claim ownship of Mutex: Unable to lock Updater sync Mutex!");

				DoPeripheralUpdate(recvBuffer);

				Debug::trace("EvolWeigherCodeUpdater", HobString("Try to Release The LOCK!!!!!!: ") + Integer::toString64((long long)update_lock) );
				//UPDATER_SYNC

				if(PAL_absclass::PAL_releaseMutex(update_lock))
				{
					Debug::trace("EvolWeigherCodeUpdater", HobString("LOCK Released!"));
				}
				else
				{
					DWORD palError = PAL_absclass::PAL_getLastError();
					Debug::trace("EvolWeigherCodeUpdater", HobString("Release of LOCK Failed") + Integer::toString64(palError));
				}
			}
		}        
		break;
        default:
            Debug::trace("EvolWeigherCodeUpdater", HobString("handleVersionManagerDevEvent():  Unhandled ver mgr event 0x") + Hex::toString((long)verManagerEvent));
            break;
    }
    return;
}

void EvolWeigherCodeUpdater::DoPeripheralUpdate(const char *recvBuffer)
{
    VmStatusMsg status;

    PeripheralCodeUpdate codeUpdate(recvBuffer);
    codeUpdate.show();
    HobString codeLocation = "";
    util_filesystem::Directory *srcDir = codeUpdate.getPath();
#if defined ( __linux__ )
// (haberrc)    util_filesystem::File weigherHex("/lib/firmware/hobart/firmware_bins/Weigher.hex");
    util_filesystem::File weigherHex(srcDir->getFullPath(), "Weigher.hex");
#else
    util_filesystem::File weigherHex(srcDir->getFullPath(), "Weigher.hex");
#endif

    if (codeUpdate.getNodeId() != usbNodeId_)
    {
        //Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate(): Wrong usb node id 0x") + Hex::toString((long long)codeUpdate.getNodeId()) + ", wait for the right msg.");
        Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate: codeUpdate has wrong id: 0x") + Hex::toString((long long)codeUpdate.getNodeId()) + HobString("my id: 0x") + Hex::toString((long long)usbNodeId_));
		goto noErr_justExit;
    }

    if (codeUpdate.getVersionType() !=  HobVersion::UsbWeigher15KgSW)
    {
        Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate(): Wrong version type 0x") + Hex::toString((long)codeUpdate.getVersionType()) + ", wait for the right msg.");
        goto noErr_justExit;
    }

    if ( codeUpdate.getResponseType() == SoftwareSystem::NotAvailable)
    {
        Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate(): Response indicates upgrade not available"));
        goto err_exit;
    }

    if ( codeUpdate.getResponseType() != SoftwareSystem::AvailableUpgrade )
    {
        Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate(): Response unrecognized"));
        goto err_exit;
    }

    if (!weigherHex.exists()) 
    {
        Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate(): Firmware file not found"));
        goto err_exit;
    }


    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  Processing tar path: ") + HobString(weigherHex.getFullPath().c_str()));                                                                    

    codeLocation = HobString(weigherHex.getFullPath().c_str());                                                                                                                                         

    status.setStatusCode(VersionManagerUsbWeigherDownloading);
    status.setResponseType(HfcResponse::RESPONSESTYLE_NOINPUT);
    status.setRemoveDialog(false);
    dispatchEvent(&status);


    //the following code should be moved to its own thread

    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  forceQuit = ") + Bool::toString(forceQuit));
    if (forceQuit || !pCommMgr_->createCommLink())
    {
	    goto err_exit;
	}

    if (forceQuit || !pCommMgr_->initializePeripheral())        //download cypress bootstrap loader
    {
        goto err_exit1;
	}

    if (forceQuit || !pCommMgr_->downloadCode(codeLocation.c_str()))    //download hex file
    {
        goto err_exit1;
	}

    if (forceQuit || !pCommMgr_->finalizePeripheral())
    {
        goto err_exit1;
	}

    //ok_exit:
    updateComplete_ = true;

    status.setStatusCode(VersionManagerUsbWeigherDownloading);
    status.setResponseType(HfcResponse::RESPONSESTYLE_NOINPUT);

    status.setRemoveDialog(true);
    dispatchEvent(&status);

	
	
	/*added linux define around detach device function call. This is ok for Linux because the cypress updater uses a system call to perform the update.
	However, Windows uses the cypress driver and we don't want the USBManager to cleanup before we are finished.*/

#if defined ( __linux__ ) 
    // the following call marks this object as DETACHED
    // so that the subsquent device removal notification 
    // will cause the object to be deleted
    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  Calling DetachDevice for node id ") + Hex::toString((long long)usbNodeId_));
    UsbManager::getInstance()->DetachDevice(usbNodeId_);
#endif

    pCommMgr_->rebootPeripheral();  
    pCommMgr_->deleteCommLink();	//close the device handle

#if defined ( _WIN32 ) 
	// the following call marks this object as DETACHED
    // so that the subsquent device removal notification 
    // will cause the object to be deleted
    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  Calling DetachDevice for node id ") + Hex::toString((long long)usbNodeId_));
    UsbManager::getInstance()->DetachDevice(usbNodeId_); 
#endif

    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  Code download completed successfully for node id ") + Hex::toString((long long)usbNodeId_));
    //goto ok_exit;


noErr_justExit:
    return;

err_exit1:
    pCommMgr_->deleteCommLink();

err_exit:
    // the following call marks this object for subsequent deletion
    UsbManager::getInstance()->DetachDevice(usbNodeId_);
    updateComplete_ = true;
    //cleanUpWorkspace(srcDir);
    Debug::trace("EvolWeigherCodeUpdater", HobString("DoPeripheralUpdate():  Code download failed or aborted for node id ") + Hex::toString((long long)usbNodeId_));
    status.setStatusCode(VersionManagerUsbWeigherDownloadFailed);
    status.setResponseType(HfcResponse::RESPONSESTYLE_OK);
    status.setRemoveDialog(false);
    dispatchEvent(&status);
}
