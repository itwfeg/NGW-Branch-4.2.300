#include "EvolMessage.h"
#include "EvolPrinterCodeUpdater.h"
#include "IPeripheralUpdater.h"

#include "VersionManagerEvent.h"
#include "PeripheralCodeUpdate.h"
#include "PeripheralRequestCodeUpdate.h"
#include "VmStatusMsg.h"
#include "CUPrinterCommManager.h"

#include "filesys/file.h"
#include "filesys/directory.h"
#include "HobVersion.h"
#include "UsbManager.h"

#include "Debug.h"
#include "Hex.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolPrinterCodeUpdater::dataType = 0;
std::string EvolPrinterCodeUpdater::dataTypeName("EvolPrinterCodeUpdater"); 
#endif

EvolPrinterCodeUpdater::EvolPrinterCodeUpdater(USBD_NODE_ID usbNodeId,
        UINT16 usbDeviceType,
        HobVersion hardwareVersion) : 
    IUsbCodeUpdater(usbNodeId, usbDeviceType), 
    responseCounter_(0)
{
    Debug::trace("EvolPrinterCodeUpdater", HobString("hardwareVersion: ") + HobString(hardwareVersion.getRevisionString()));

    SUsbDescriptors *descs = UsbManager::getInstance()->getUsbDescs( usbNodeId );
    if ( descs != NULL )
        pCommMgr_ = new CUPrinterCommManager(usbNodeId, descs );
   
    forceQuit = false;
    updateComplete_ = false;
	lockUpdater_ = false;

    responseStateMachine_ = waitingForResponses;
	Debug::trace("EvolPrinterCodeUpdater", HobString("EvolPrinterCodeUpdater my Mutex: ") + Integer::toString64((long long)update_lock) );
	//PeripheralRequestCodeUpdate reqCodeUpdate(usbNodeId, hardwareVersion, HobVersion(HobVersion::Epp2PrinterSW, 0, 0));
    //dispatchEvent(&reqCodeUpdate);

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);
    util_acc::addInstance(dataType);
#endif
}

EvolPrinterCodeUpdater::~EvolPrinterCodeUpdater()
{
    Debug::trace("EvolPrinterCodeUpdater", HobString("Destructor Called"));

    delete pCommMgr_;

#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}

void EvolPrinterCodeUpdater::handleVersionManagerDevEvent(VersionManagerEvent verManagerEvent, const char *recvBuffer)
{
    VmStatusMsg status;

    switch (verManagerEvent)
    {
        case PeripheralUpdateCode:
		{	
            Debug::trace("EvolPrinterCodeUpdater", HobString("XPE Printer recvd ver mgr event PeripheralUpdateCode"));
            
			PeripheralCodeUpdate codeUpdate(recvBuffer);
			if(usbNodeId_ == codeUpdate.getNodeId())
			{
				Debug::trace("EvolPrinterCodeUpdater", HobString("XPE Printer is about to do peripheral update"));
				DoPeripheralUpdate(recvBuffer);
				Debug::trace("EvolPrinterCodeUpdater", HobString("XPE Printer finished peripheral update"));
			}
			else
			{
				Debug::trace("EvolPrinterCodeUpdater", HobString("XPE Printer nodeId did not match myId: 0x")+Hex::toString((long long)usbNodeId_)+HobString(" to their id: 0x")+Hex::toString((long long)codeUpdate.getNodeId()));
			}
			break;
		}
        default:
            Debug::trace("EvolPrinterCodeUpdater", HobString("unhandled ver mgr event 0x") + Hex::toString((long)verManagerEvent));
            break;
    }

    return;
}

void EvolPrinterCodeUpdater::DoPeripheralUpdate(const char *recvBuffer)
{
    VmStatusMsg status;

	PeripheralCodeUpdate codeUpdate(recvBuffer);
 
	if(!lockUpdater_)
	{
		//ownership of the mutex
		if (WAIT_OBJECT_0 != PAL_absclass::PAL_waitForSingleObj(update_lock, 0))
			Debug::trace("EvolPrinterCodeUpdater", "Failed to claim ownship of Mutex: Unable to lock Updater sync Mutex!");

		lockUpdater_ = true;
	}

    switch(responseStateMachine_)
    {
		case waitingForResponses:
        {
            Debug::trace("EvolPrinterCodeUpdater", HobString("Total received responses so far ") + Integer::toString(responseCounter_));

            if (codeUpdate.getNodeId() == usbNodeId_)
            {
				switch (codeUpdate.getResponseType())
                {
					case SoftwareSystem::AvailableUpgrade:
                    {
						bool flashUpdateRequired = false;

						Debug::trace("EvolPrinterCodeUpdater", HobString("ready to flash new software for nodeId = ") + Hex::toString((long long)codeUpdate.getNodeId()));

						switch (codeUpdate.getVersionType())
						{
							case HobVersion::EssPrinterSW:
							case HobVersion::EscPrinterSW:
							case HobVersion::EppPrinterSW:
							case HobVersion::Epp2PrinterSW:		/* Alex Zavala's code for XPE version */
							{
								Debug::trace("EvolPrinterCodeUpdater", HobString("setting up the burn new printer sw") );

								pCommMgr_->setupCodeFlash(CuPrnBurnPrinter);
								flashUpdateRequired = true;
								break;
							}
							case HobVersion::EssPrnBootCodeSW:
							case HobVersion::EscPrnBootCodeSW:
							case HobVersion::EppPrnBootCodeSW:
							case HobVersion::Epp2PrnBootCodeSW:		/* Alex Zavala's code for XPE version (value does not exist)*/
							{
								Debug::trace("EvolPrinterCodeUpdater", HobString("setting up the burn new printer Boot code ") );
								pCommMgr_->setupCodeFlash(CuPrnBurnBootCode);
								flashUpdateRequired = true;
								break;
							}                                                           
							case HobVersion::EssPrnFlashUpdateSW:
							case HobVersion::EscPrnFlashUpdateSW:
							case HobVersion::EppPrnFlashUpdateSW:
							case HobVersion::Epp2PrnFlashUpdateSW:		/* Alex Zavala's code for XPE version (value does not exist)*/
							{
								Debug::trace("EvolPrinterCodeUpdater", HobString("setting up the burn new printer FLUD ") );
								pCommMgr_->setupCodeFlash(CuPrnBurnFlud);
								flashUpdateRequired = true;
								break;
							}
							default:
								break;
						}

                        if (flashUpdateRequired)
                        {

                            HobString codeLocation = "";
                            Directory *srcDir = codeUpdate.getPath();
                            File sourceBin(srcDir->getFullPath(), "source.bin");

                            if (sourceBin.exists() == true)
                            {
                                Debug::trace("EvolPrinterCodeUpdater", HobString("Sending down the new source, processing tar path=") + HobString(sourceBin.getFullPath().c_str()));
                                codeLocation = HobString(sourceBin.getFullPath().c_str());

                                VmStatusMsg status(NULL, VersionManagerUsbPrinterDownloading, HfcResponse::RESPONSESTYLE_NOINPUT, false);
                                dispatchEvent(&status);

                                pCommMgr_->createCommLink();
                                pCommMgr_->initializePeripheral();
                                pCommMgr_->downloadCode(codeLocation.c_str());
                                pCommMgr_->finalizePeripheral();
                                pCommMgr_->rebootPeripheral();
                                pCommMgr_->deleteCommLink();

                                cleanUpWorkspace(srcDir);
                                //was in the destructor! -- ats
                                VmStatusMsg statusComplete(NULL, VersionManagerUsbPrinterDownloading, HfcResponse::RESPONSESTYLE_NOINPUT, true);
                                dispatchEvent(&statusComplete);

                                responseStateMachine_ = sendingOutCommand;
                            }
                            else
                            {
                                VmStatusMsg status(NULL, VersionManagerUsbPrinterDownloadFailed, HfcResponse::RESPONSESTYLE_OK, false);
                                dispatchEvent(&status);

                            }
                        }
                        break;
                    }
                    case SoftwareSystem::NotAvailable:
                    {
                        //testing for JRB
                        Debug::trace("EvolPrinterCodeUpdater", HobString("code not available on internal cf") );
                        break;
                    }
                    case SoftwareSystem::AvailableSameVersion:
                    {
                        //testing for JRB
                        Debug::trace("EvolPrinterCodeUpdater", HobString("Available same version") );
                        break;
                    }
                    case SoftwareSystem::retryCntrOver:
                    {
                        Debug::trace("EvolPrinterCodeUpdater", HobString("failure counter over limit") );
                        VmStatusMsg status(NULL, VersionManagerUsbPrinterRetryOverflow, HfcResponse::RESPONSESTYLE_OK, false);
                        dispatchEvent(&status);
                        responseStateMachine_ = failedDontRun;
                    }
                    default:
                    {
                            Debug::trace("EvolPrinterCodeUpdater", HobString("probably invalid hardware version") );                                                                    
                            VmStatusMsg status(NULL, VersionManagerUsbPrinterDownloadFailed, HfcResponse::RESPONSESTYLE_OK, false);
                            dispatchEvent(&status);

                            break;
                    }
				}
                responseCounter_++;
			}

            //we have the three expected responses and no required flash update.
            if ((responseCounter_ >= 3) && (responseStateMachine_ == waitingForResponses))
            {
				//added ats, notify UsbManager of our node state change.

				UsbManager::getInstance()->DetachDevice(usbNodeId_);



                Debug::trace("EvolPrinterCodeUpdater", HobString("Received three responses. so just tell the printer to run normally") );
                pCommMgr_->setupCodeFlash(CuPrnRunNormal);
                responseStateMachine_ = sendingOutCommand;

				Debug::trace("EvolPrinterCodeUpdater", HobString("Try to Release the LOCK!!!!!: ") + Integer::toString64((long long)update_lock) );
	
				//UPDATER_SYNC: release lock
				if(PAL_absclass::PAL_releaseMutex(update_lock))
				{
					Debug::trace("EvolPrinterCodeUpdater", HobString("LOCK Released!") );
				}
				else
				{
					DWORD palError = PAL_absclass::PAL_getLastError();
					Debug::trace("EvolPrinterCodeUpdater", HobString("Failed to Release LOCK!!!") + Integer::toString64(palError));
				}
            }
        } //case waitingForResponses
        break;

        case failedDontRun:
        {
            Debug::trace("EvolPrinterCodeUpdater", HobString("failed state, checking for newly installed SW") );                                                                        
            switch (codeUpdate.getResponseType())
            {
                case SoftwareSystem::AvailableUpgrade:
                {
                    Debug::trace("EvolPrinterCodeUpdater", HobString("failed state, new software so send reset cmd") );                                                                     
                    pCommMgr_->setupCodeFlash(CuPrnResetErrCounter);
                    responseStateMachine_ = sendingOutCommand;
                }
                default:
                    // default to remove warning under gcc
                    break;
            }

        } //case failedDontRun
        break;
        default:
            // default only to remove gcc warning
            break;
     } //switch responseStateMachine_
}

bool EvolPrinterCodeUpdater::isUpdateComplete()
{
#if 0
    return updateComplete_;

#else
    bool updateComplete = false;

    if (updateComplete_)
    {
        Debug::trace("EvolPrinterCodeUpdater", HobString("usb peripheral remove so just kill the updater") );                                                                   
        updateComplete = true;
    }
    else
    {           

        switch(responseStateMachine_)
        {
            case waitingForResponses:
                if (timedOut_)
                {
                    Debug::trace("EvolPrinterCodeUpdater", HobString("timed out waiting on response from version manager") );                                                                   
                    updateComplete = true;
                }
                break;
            case sendingOutCommand:
                responseStateMachine_ = checkForCommandComplete;
                break;
            case checkForCommandComplete:
                updateComplete = pCommMgr_->areTransactionsComplete();
                break;
                //if we failed to flash then hang around. maybe we will get a 
                //firmware upgrade that will reset the system.
                //case failedDontRun:
                //      {
                //              Debug::trace("EvolPrinterCodeUpdater", HobString("keep usb printer waiting for response until new sw arrives") );                                                                       
                //      }
                //      break;
            case failedDontRun:
                {
                    Debug::trace("EvolPrinterCodeUpdater", HobString("failedDontRun: updater self-terminating") );                                                                      
                    updateComplete = true;
                }
                break;
        }
    }

    Debug::trace("EvolPrinterCodeUpdater", HobString("is update complete? ") + string(updateComplete ? "yes" : "no"));

    return updateComplete;
#endif 
}

