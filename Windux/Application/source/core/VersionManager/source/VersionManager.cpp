#include "PAL_platformheader.h"

#include <fstream>

#include "VersionManager.h"
#include "VersionManagerEvent.h"
#include "properties/properties.h"
#include "filesys/file.h"

#include "EvolMessage.h"

#include "HobComplexVersion.h"
#include "UpdateSoftwareManually.h"
#include "UpdateSoftwareAutomatically.h"
#include "PeripheralRequestCodeUpdate.h"
#include "PeripheralCodeUpdate.h"
#include "UsbAddComponent.h"
#include "UsbRemoveComponent.h"
#include "ShutdownSystemResponse.h"
#include "ShutdownSystemRequest.h"
#include "ProcessDBFilesRequest.h"

#include "Configuration.h"
#include "Hex.h"
#include "VmStatusMsg.h"
#include "HfcResponse.h"
#include "CMessageCenter.h"
#include "CZip.h"
#include "ImportManager.h"
#include "ImportEvent.h"
#include "SessionEvent.h"
#include "EvolWrapperRemoveUpgradeFiles.h"
#include "CLARemoveUpgradeFiles.h"//Alex Zavala 10/3
#include "FpmManager.h"

#include "PythonScripter.h"
#include "HobTokenizerCSV.h"

#include "ULong.h"					  // Convert pointer to string for debugging

#if defined ( __linux__ )
#include <sys/utsname.h>
#endif

#define ERROR_CODE_FAILED_CONNECTION -2
#define NOTHING_TO_INSTALL			 -3

using namespace std;
using namespace util_filesystem;
using namespace util_env;

const USBD_NODE_ID VersionManager::SYSTEM_CONTROLLER_NODEID 	= (USBD_NODE_ID)0; 


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int VersionManager::dataType = 0;
std::string VersionManager::dataTypeName("VersionManager"); 
#endif

VersionManager *VersionManager::instance = (VersionManager *) NULL;


VersionManager::VersionManager()
	: Hobart::CEvolMsgReceiver("VersionManager"),
	rebootRequested_(false), safeToReboot_(false), convertDb_(false), onlyNewerVersions_(false), importManager_(0), deletedFirmware_(false), dbTmpDir_("")
{
	Debug::trace("VersionManager", "VersionManager::VersionManager()");

#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    currentSoftwareSystem_ = NULL;
    newSoftwareSystem_ = NULL;

    //lcg rev 0.2 memDrv();

	registerEvent(VersionManagerDev, UpgradeAutomatic);
	registerEvent(VersionManagerDev, UpgradeAutomaticViaServer);
    registerEvent(VersionManagerDev, UpgradeManual);
    registerEvent(VersionManagerDev, PeripheralReqUpdateCode);		
    registerEvent(VersionManagerDev, AddUsbComponent);		
    registerEvent(VersionManagerDev, RemoveUsbComponent);		
    registerEvent(SessionDev, SnBootupCompleted);		
	registerEvent(VersionManagerDev, RemoveWrapperSWComponents);
	registerEvent(VersionManagerDev, RemoveCLASWComponents);//Alex Zavala 10/3
	registerEvent(VersionManagerDev, UpdateNow2016);	
	registerEvent(VersionManagerDev, SendToDept2016);
	


	loadCurrentSystemComponents(); /*get currently installed software components*/

    validateCurrentSystem();//

    dbTmpDir_.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::TEMPORARYDBDIRECTORY));
}

void VersionManager::timeoutProcess( __int32 timer_id )
{
    if ( timer_id == delayBeforeRebootAvailable)
    {
        safeToReboot_ = true;

        if (rebootRequested_)
        {
#if defined( PAL_WIN32_RUN )
            disableAppLaunchOnBootUp();
            enableAppInstallerOnReboot();
#elif   defined( __linux__ )
            // ***** WARNING: we need the equivlent functions in Linux

#else
#error "Implement something for your system."
#endif

            requestShutdown(convertDb_);
        }
    }
}

void VersionManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )						//added ats
{
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

    switch (msg->getDeviceType())
	{
		case SessionDev:
		{
			switch ( msg->getEventType())
			{
				case SnBootupCompleted:
				{
					Debug::trace("VersionManager.handlemsg", HobString("processEvent:SnBootupCompleted"));

					createTimer(5000, delayBeforeRebootAvailable);
					break;
				}
			}
			break;
		}

		case VersionManagerDev:
		{
			switch ( msg->getEventType())
			{
				case UpgradeAutomatic:
				case UpgradeAutomaticViaServer:
				{
					if (config->toBoolCfgI(CfgInternal::SCALEMANAGEMENTHANDLESUPGRADE))
					{
						if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
						{
							const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::PYTHONSFTPUPDATEPORT), "sftpUpgradeNow") );

							Debug::trace("VersionManager", "scaleManagementHandlesUpgrade=true, UpgradeAutomatic or UpgradeAutomaticViaServer, command: " + command);
							string connectionStatusOutput;
							PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
							// We don't care about any output from the command.
						}
						else
							Debug::trace("VersionManager", "scaleManagementHandlesUpgrade=true, but python disabled");
					}
					else
					{
						UINT8 theEvent = msg->getEventType();
						Debug::trace("VersionManager.upgrade", "********    " + (theEvent == UpgradeAutomatic) ? HobString("Handling UpgradeAutomatic") : HobString("Handling UpgradeAutomaticViaServer"));
						Debug::trace("VersionManager.upgrade", HobString("rebootRequested_ = ") + Bool::toString(rebootRequested_));

						if (!rebootRequested_)
						{
							UpdateSoftwareAutomatically updateMsg (msg->Data(), msg->Length());
							{
								VmStatusMsg status(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, false);
								dispatchEvent(&status);
							}

							PAL_absclass::PAL_sleep( 500 );

							STATUS st = ERROR_CODE_FAILED_CONNECTION;

							if ( theEvent == UpgradeAutomatic)
							{
								st = updateFirmWareFromTFTP( &updateMsg );						// this is where the actual FTP upgrade actions occur
								Debug::trace("VersionManager.upgrade", HobString("processEvent():  updateFirmWareFromTFTP() returned ") + Integer::toString(st));
							}
							else if ( theEvent == UpgradeAutomaticViaServer)
							{
								st = updateFirmWareFromServer( &updateMsg );						// this is where the actual copy and upgrade actions occur
								Debug::trace("VersionManager.upgrade", HobString("processEvent():  updateFirmWareFromServer() returned ") + Integer::toString(st));
							}

							if ((!rebootRequested_) && (st != ERROR_CODE_FAILED_CONNECTION))
							{
								Debug::trace("VersionManager.upgrade", "processEvent():  " + HobString("STATUS = ") + Integer::toString(st));
								handleRemoteDatabaseFiles(&updateMsg);
								handleRemoteGraphicsFiles(&updateMsg);
							}

							if (!rebootRequested_)
							{
								VmStatusMsg status(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, true);
								dispatchEvent(&status);
							}
						}
					}
					break;	
				}
				case UpgradeManual:
				{
					if (!rebootRequested_)
					{
						UpdateSoftwareManually updateMsg(msg->Data(), msg->Length());
						Debug::trace("VersionManager.upgrade", HobString("processEvent(UpgradeManual):  Calling updateFirmWareFromThumbDrive(" + updateMsg.getPath() + ");"));
						updateFirmWareFromThumbDrive(updateMsg.getPath());
					}
//					Debug::trace("VersionManager", "Return from upDateThumb");
					break;
				}

				case PeripheralReqUpdateCode:
				{
#ifdef APC_PERIPHERAL_UPDATE_REQUIRES_REBOOT
					if (!rebootRequested_)
#endif
						/*race condition here for prepack device's. add lock to process one at a time!
						*/
						Util::CLockCriticalSection lock( versionMgrCs );

					handleUSBRequestUpdate (msg->Data());
					break;
				}
				case RemoveUsbComponent:
				{
					UsbRemoveComponent removeMsg(msg->Data());
					removeAHardwareComponent(removeMsg.getNodeId());
					break;
				}
				case AddUsbComponent:
				{
					UsbAddComponent addMsg(msg->Data());
					HobVersion *pNewHV = new HobVersion(*addMsg.getHardwareVersion());

					addAHardwareComponent(addMsg.getNodeId(), pNewHV);
					break;
				}
				case RemoveWrapperSWComponents:
				{
					Debug::trace("VersionManager", "RemoveWrapperSWComponents");

					FpmManager *pSerialFlash( FpmManager::getInstance() );

				    if ( pSerialFlash )
				    {
						const std::string systemStyle( pSerialFlash->getMFGSystemStyle() );

						if (( systemStyle == "AWS3" ) ||
							( systemStyle == "NGW1" ) || ( systemStyle == "NGW2" ) ||
							( systemStyle == "NGW3" ) )
						{
							std::list<std::string> files;

							// NGW
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node1/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node1/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node2/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node2/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node3/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node3/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node4/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node4/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node5/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node5/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node6/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw/wrapper/node6/version.ini" );

							// NGW3
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node1/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node1/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node2/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node2/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node3/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node3/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node4/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node4/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node5/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node5/version.ini" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node6/source.bin" );
							files.push_back( "/lib/firmware/hobart/ngw3/wrapper/node6/version.ini" );

							Debug::trace("VersionManager", "Remove Firmware!");

							for ( const auto &file : files )
							{
								File firmware(file);

								if (firmware.exists())
									firmware.remove();
							}
						}
				    }

					if (!currentSoftwareSystem_)
					{
						Debug::trace("VersionManager", "RemoveWrapperSWComponents:  Calling loadCurrentSystemComponents()");
						loadCurrentSystemComponents();
					}

					if (!currentSoftwareSystem_)
					{
						Debug::trace("VersionManager", "RemoveWrapperSWComponents:  currentSoftwareSystem_ is 0!");
					}
					else
					{
						Debug::trace("VersionManager", "RemoveWrapperSWComponents:  Calling removeAllWrapperSWComponents()");
						if (currentSoftwareSystem_->removeAllWrapperSWComponents())
						{
							Debug::trace("VersionManager", "RemoveWrapperSWComponents:  Removed wrapper SW components. Calling savePropertiesToFile()");
							currentSoftwareSystem_->savePropertiesToFile(true);							
						}

						// we are going to do this irreguardless because maybe this is all they want to do is clear the version.ini
						clearVersionIniComponents();
					}

					EvolWrapperRemoveUpgradeFiles msgDone(this, EvolWrapperRemoveUpgradeFiles::RemovalComplete);
					dispatchEvent(&msgDone);
					break;
				}
				//Added 9/24 Alex Zavala
				case RemoveCLASWComponents:
				{
					//TODO
					Debug::trace("VersionManager", "RemoveCLASWComponents");
					if(currentSoftwareSystem_)
					{
						Debug::trace("VersionManager", "RemoveCLASWComponents:  Calling removeAllCLASWComponents()");
						if (currentSoftwareSystem_->removeAllCLASWComponents())
						{
							Debug::trace("VersionManager", "RemoveCLASWComponents:  Removed CLA SW components. Calling savePropertiesToFile()");
							currentSoftwareSystem_->savePropertiesToFile(true);	
						}

						// we are going to do this irreguardless because maybe this is all they want to do is clear the version.ini
						clearVersionIniComponents();
					}
					else
					{
						Debug::trace("VersionManager", "RemoveCLASWComponents:  Calling loadCurrentSystemComponents()");
						loadCurrentSystemComponents();
					}

					CLARemoveUpgradeFiles msgDone(this, CLARemoveUpgradeFiles::RemovalComplete);
					dispatchEvent(&msgDone);
					break;
				}
			}
			break;
		}

		case ImportDev:
		{
			switch ( msg->getEventType())
			{
				case ImportTerminate:
				{
					if (importManager_)
					{
						deregisterEvent(ImportDev, ImportTerminate);
						delete importManager_;
						importManager_ = 0;

						Configuration* pCFG = Configuration::getInstance();
						util_filesystem::Directory vmTempDir("");
						vmTempDir.appendPath(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
						vmTempDir.appendPath("Graphics");
						vmTempDir.remove(true);
					}
				}
				break;
			}
			break;
		}

		default:
			break;
	}
}

VersionManager::~VersionManager()
{
	Debug::trace("VersionManager", "VersionManager::~VersionManager()");
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
    shutdownTimerThreads();
	ShutdownNicely();
	
	const HobVersion *pDel;
    deregisterAllEvents();

    if (currentSoftwareSystem_ != NULL)
        delete currentSoftwareSystem_;

    if (newSoftwareSystem_ != NULL)
        delete newSoftwareSystem_;

    for (ComponentListIterator i = hardwareComponentList_.begin(); i != hardwareComponentList_.end(); i++)
    {
        pDel = i->second;
        delete pDel;
    }

    hardwareComponentList_.clear();
}


VersionManager *VersionManager::getInstance()
{
    if (instance == 0)
    {
        instance = new VersionManager();
    }

    return instance;
}


HobComplexVersion * VersionManager::getSystemControllerSoftwareVersion()
{
	Debug::trace("VersionManager", "VersionManager::getSystemControllerSoftwareVersion()");
    if (currentSoftwareSystem_ == NULL)
        return NULL;

    return (currentSoftwareSystem_->getSoftwareVersionOfComponentType(HobVersion::SystemControllerSW));
}

HobVersion * VersionManager::getSystemControllerHardwareVersion()
{
	Debug::trace("VersionManager", "VersionManager::getSystemControllerHardwareVersion()");
    return getAHWComponent(HobVersion::SystemControllerSW);
}

HobVersion * VersionManager::getOSVersion()
{
	Debug::trace("VersionManager", "VersionManager::getOSVersion()");
    return getAHWComponent(HobVersion::XPEImageSW);
}

string VersionManager::getOSVersionAsString()
{
    Debug::trace("VersionManager", "loadOSVersion");

#if defined ( __linux__ )

    struct utsname uts;
    uname( &uts );
    string osVersion(&uts.release[0]);
	Debug::trace("VersionManager", "getOSVersionAsString - version: " + osVersion);
	return osVersion;
#else
    string osVersion("");

    const string command( PythonScripter::buildPythonCall("getWinOSVersion") );

    string versionOutput;
    PAL_absclass::PAL_executeWithResults(command, versionOutput);
    Debug::trace("VersionManager", "Python results of calling getWinOSVersion.py: " + versionOutput);

    osVersion = "6.2";
    if (versionOutput.size())
    {
        HobTokenizerCSV tok(&versionOutput, '\n');
        HobString token;
        if (tok.next(&token) != -1)  // First line will be the IP Address
        {
            osVersion = token;
        }
    }

    return osVersion;
#endif
}


HobVersion * VersionManager::getAHWComponent(HobVersion::VersionType searchType)
{
	Debug::trace("VersionManager", "VersionManager::getAHWComponent(" + Integer::toString(searchType) + ")");
    ComponentListIterator i;

    if (hardwareComponentList_.empty())
        return NULL;

    for (i = hardwareComponentList_.begin(); i != hardwareComponentList_.end(); i++)
    {
        if (((HobVersion *)i->second)->getVersionType() == searchType)
            break;
    }

    if (i != hardwareComponentList_.end())
        return i->second;
    else
        return NULL;
}


void VersionManager::handleUSBRequestUpdate(const char *recvBuffer)
{
	Debug::trace("VersionManager", "VersionManager::handleUSBRequestUpdate()");
    PeripheralRequestCodeUpdate msg(recvBuffer);

    if (currentSoftwareSystem_ == NULL)
    {
        Debug::trace("VersionManager.updatePeripheral", HobString("handleUSBRequestUpdate -- currentSoftwareSystem: NULL"));
    }
    else
    {
        PeripheralCodeUpdate *pReplyMsg = NULL;
        Debug::trace("VersionManager.updatePeripheral", HobString("handleUSBRequestUpdate -- currentSoftwareSystem: !NULL"));

#ifdef APC_PERIPHERAL_UPDATE_REQUIRES_REBOOT
        bool rebootNecessary = currentSoftwareSystem_->requestPeripheralUpgrade(&msg, hardwareComponentList_, &pReplyMsg);
        if (rebootNecessary)
        {
            Debug::trace("VersionManager.updatePeripheral", HobString("handleUSBRequestUpdate:need to flash apc peripheral must reboot."));
            tryRequestShutdown(false);
        }
        else 
#else
        // reboot is not required with UsbManager architecture and AT91 support

		// Determine the device asking for an update.
		HobVersion::VersionType newVerType = HobVersion::UnknownComponent;
		const HobVersion *softVer = msg.getSoftwareVersion();
		if ( softVer && !softVer->isNULL() )
			newVerType = softVer->getVersionType();
		else
		{
			const HobVersion *hardVer = msg.getHardwareVersion();
			if ( hardVer && !hardVer->isNULL() )
				newVerType = HobComplexVersion::getCorrespondingSoftTypeFromHardType(hardVer->getVersionType());
		}

		// if the device is the APC/FPC printer/weigher, we have a new way of determining if the firmware needs to be flashed to it,
		// basically if the firmware binary resides in a particular directory, then it needs to be burned to the FPC.
		// if this isn't the APC/FPC, then use the old way of checking if the firmware needs to be flashed,
		// by using version.ini and checking for a difference between what's on the device and what is in the file system.
		if ( ( newVerType == HobVersion::ApcPrinterSW ) || ( newVerType == HobVersion::FpcPrinterSW ) || ( newVerType == HobVersion::Epp3PrinterSW ))
		{
			Directory directoryFromWhereFirmwareGetsBurnedToDevice("");
			HobComplexVersion::getExtractionDir(newVerType, directoryFromWhereFirmwareGetsBurnedToDevice, 0); // the last parameter isn't used, but has to be supplied

			File newLocationFile(directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
			if (newLocationFile.exists())
			{
				Debug::trace("VersionManager.updatePeripheral", "Have firmware to burn to FPC: " + newLocationFile.getFullPath());

				string fpcVersion = inttostring((int)newVerType) + ".1.0.0";
				HobComplexVersion tempDummyHCV(fpcVersion, "", true);
				pReplyMsg = new PeripheralCodeUpdate(msg.getNodeId(), SoftwareSystem::AvailableUpgrade, tempDummyHCV);
			}
			else
				Debug::trace("VersionManager.updatePeripheral", "Did not find firmware to burn to FPC: " + newLocationFile.getFullPath());

		}
		else
			currentSoftwareSystem_->requestPeripheralUpgrade(&msg, hardwareComponentList_, &pReplyMsg);
#endif
        if (pReplyMsg != NULL)
        {			
			dispatchEvent(pReplyMsg);
            delete pReplyMsg;
        }
    }
}

/*
   repeat failure condition exists when the usbperipheral indicates that a flash failure exists
   more that three times. Once this occurs we retry 6times. Once this point has been reached
   we no longer attempt to flash the usbperipheral. The retry counter is reloaded when the
   particular software is installed. The counter is also refreshed when the scale is booted up.
   */
bool VersionManager::checkForRepeatFailureCondition(PeripheralRequestCodeUpdate & msg)
{
    if (msg.getFailureCount() > 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}


STATUS VersionManager::loadCurrentSystemComponents()
{
    Configuration* pCFG = Configuration::getInstance();
    Directory evDir(pCFG->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));
    File verINI(evDir.getFullPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

    if (currentSoftwareSystem_ != NULL)
        delete currentSoftwareSystem_;

    currentSoftwareSystem_ = new SoftwareSystem(verINI, true);

    return OK;
}


HobDependency::DependencyResult VersionManager::validateCurrentSystem()
{
	Debug::trace("VersionManager", "VersionManager::validateCurrentSystem()");

//	HTA-9: FasTrack does not check OS or SystemController versions in VersionManager.
//    if (hardwareComponentList_.size() == 0)
//    {
//        loadOSVersion();
//        loadSystemControllerHardwareVersion();
//    }

    if (currentSoftwareSystem_ != NULL)
        return currentSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_);

    return HobDependency::NoDependency;
}

//#if defined(PAL_WIN32_RUN)
STATUS VersionManager::getSrcFromFTP(PAL_INET_HANDLE hFTP, util_filesystem::File& srcZip,
        SoftwareSystem** pNewSC)
{
    STATUS stat = ERROR_CODE;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
    util_filesystem::File destZip (vmTempDir.getFullPath(), srcZip.getFileName());

    *pNewSC = 0;

    if (!vmTempDir.exists())
        vmTempDir.create();
    if (destZip.exists())
        destZip.remove();

    if (
            PAL_absclass::PAL_ftpGetFile(
                hFTP, 
                srcZip.getFileName().c_str(), 
                destZip.getFullPath().c_str(), 
                false,
                PAL_FILE_ATTR_NORMAL,
                PAL_FTP_TRANSFER_TYPE_BINARY, 
                0)
       )
    {
        if (destZip.exists() )
        {
            Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:ftp copy successfull"));

            Hobart::CZip::unZipSingleFile(destZip.getFullPath().c_str(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME).c_str(), destZip.getPath().c_str());

            File verIniTargetLoc (destZip.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

            if (verIniTargetLoc.exists())
            {
                Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:version.ini exists. create software system"));
                *pNewSC = new SoftwareSystem(verIniTargetLoc);

                HobComplexVersion * pVersion = (*pNewSC)->findMajorComponent();
                if (pVersion)
                {
                    Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:moving zip file to safe area"));
                    util_filesystem::Directory tmpDir("");
                    util_filesystem::File tmpZip ("");
                    HobComplexVersion::getTempDir(pVersion->getVersionType(), tmpDir, tmpZip);
//                    Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:  tmpDir = ") + tmpDir.getFullPath() + ",  tmpZip = " + tmpZip.getFullPath());

                    if (tmpDir.exists())
                        tmpDir.removeContents(true);
                    else
                        tmpDir.create();

                    destZip.moveTo(tmpZip);

                    if (tmpZip.exists())
                    {
                        //this is just a check for file corruptions
                        if (Hobart::CZip::unZip(tmpZip.getFullPath().c_str(), tmpZip.getPath().c_str()) == 0)
                        {
                            Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:  ZIP completely extracted ok! No file corruptions!"));

                            File verIniTmpLoc (tmpZip.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));
                            if (verIniTmpLoc.exists())
                            {
                                Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:  version.ini file exists! All is good."));

                                stat = OK;
                            }
                        }
                        else
                        {
                            tmpDir.remove(true);
                            Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:zip file failed to completely extract!zip file must be corrupted or incomplete!"));
                        }
                    }
                }
                else
                {
                    Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:something is wrong with version.ini file"));
                }

                verIniTargetLoc.remove();
            }
            else
            {
                Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:version.ini DOES NOT EXIST"));
            }
        }
        else
        {
            Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:zip file does not exist locally"));
        }
    }
    else
        Debug::trace("VersionManager.upgrade.FTP", HobString("getSrcFromFTP:failed to copy zip file"));

    if ((stat != OK) && (*pNewSC != 0))
    {
        delete *pNewSC;
        *pNewSC = 0;
    }

    if (destZip.exists())
        destZip.remove();

    return stat;
}

STATUS VersionManager::getIniFromFTP(PAL_INET_HANDLE hFTP, util_filesystem::File& srcIni,
        SoftwareSystem** pNewSC)
{
    STATUS stat = ERROR_CODE;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));

    util_filesystem::File destIni (vmTempDir.getFullPath(), srcIni.getFileName());

    *pNewSC = 0;

    if (!vmTempDir.exists())
        vmTempDir.create();

    if ( 
            PAL_absclass::PAL_ftpGetFile( 
                hFTP, 
                srcIni.getFullPath().c_str(), //getFileName()
                destIni.getFullPath().c_str(), 
                false,
                PAL_FILE_ATTR_NORMAL,
                PAL_FTP_TRANSFER_TYPE_BINARY, 
                0)
       )
    {
        if (destIni.exists())
        {
            Debug::trace("VersionManager.upgrade.FTP", HobString("getIniFromFTP:version.ini exists. create software system"));
            *pNewSC = new SoftwareSystem(destIni);

            stat = OK;
        }
        else
        {
            Debug::trace("VersionManager.upgrade.FTP", HobString("getIniFromFTP:version.ini DOES NOT EXIST"));
        }
    }
    else
        Debug::trace("VersionManager.upgrade.FTP", HobString("getIniFromFTP:failed to copy target files"));

    if (destIni.exists())
        destIni.remove();

    return stat;
}

// This method builds the proper URL path for ftp access.  The libcurl library (Linux) was not determining
// ftp to use the ftp protocol for upgrade via FTP, so we have to explicitly add it to the front.
// Additionally, if the user doesn't put a directory separator on the end of the directory name, Linux also
// wasn't treating it as a directory to read file names from, so we have to add it if it isn't there.
void VersionManager::validateFtpPath(std::string &path)
{
	std::string head = "ftp://";
	if (path.find(head) == string::npos)
		path.insert(0, head);

	std::string dirSep = Configuration::getInstance()->getDirectorySeparator();
	if (path.at(path.size()-1) != dirSep.at(0) )
		path.append(dirSep);
}

STATUS VersionManager::updateFirmWareFromTFTP(UpdateSoftwareAutomatically* msg)
{
    PAL_INET_HANDLE hInet   = PAL_INET_HANDLE_FAIL;
    PAL_INET_HANDLE hFTP    = PAL_INET_HANDLE_FAIL;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
    util_filesystem::File currentVersionIni (pCFG->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

    if (newSoftwareSystem_ != NULL)
        delete newSoftwareSystem_;

	Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:parsing current software system. all componenents are installed"));
    newSoftwareSystem_ = new SoftwareSystem(currentVersionIni, true);
    if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
    {
        const string *pNew = NULL;

		Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:existing software system is invalid!!!!Dont Allow Upgrades"));

        pNew = newSoftwareSystem_->toString();

        Debug::trace("VersionManager.upgrade.FTP", *pNew);

        VmStatusMsg status(NULL, VersionManagerCurSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
        dispatchEvent(&status);
        return ERROR_CODE;
    }
    else
    {
		Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:current software system is valid"));
    }

    if (vmTempDir.exists())
        vmTempDir.remove(true);

    hInet = PAL_absclass::PAL_internetOpen(("VersionManager"), PAL_INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0); 
    if (hInet)
    {
        string path = msg->getFtpServerName();

#if defined ( __linux__ )
        // linux can open the entire path at once,
        // making the following code a little 
        // simpler
        path += msg->getPath();
        validateFtpPath(path);

#endif      // __linux__

        hFTP = PAL_absclass::PAL_internetConnect(
                hInet, 
                path.c_str(), 
                PAL_INTERNET_DEFAULT_FTP_PORT,
                msg->getFtpUserName().c_str(), 
                msg->getFtpPassword().c_str(),
                PAL_INTERNET_SERVICE_FTP, 
                0, 
                0);
        if (hFTP)
        {
            util_filesystem::File currentFile ("");
#if defined (PAL_WIN32_RUN)
            // I know this isn't the correct use of our COMPILE flags but I had no choice
            // the follow code is the orignal code

            if (msg->getPath() != "")
            {
                CHAR currentDir[MAX_PATH];
                DWORD size = sizeof(currentDir);

                if (FtpGetCurrentDirectoryA(hFTP, currentDir, &size))
                {
                    util_filesystem::Directory dCurrent ("");
                    if (strcmp(currentDir, "/") != 0)
                        dCurrent.appendPath(currentDir);
                    dCurrent.appendPath(msg->getPath());

                    // jhc gcd to WINDUX: START new code to replace original FtpSetCurrentDirectoryA below
					// Code is different than Pevola version in Walmart. This needs throughly tested
					CHAR slashCorrectedCurrentDir[MAX_PATH] = { 0 };
					for (int i = 0; i < dCurrent.getFullPath().length(); ++ i)
						slashCorrectedCurrentDir[i] = (dCurrent.getFullPath()[i] == '\\') ? '/' : dCurrent.getFullPath()[i];

					FtpSetCurrentDirectoryA(hFTP, slashCorrectedCurrentDir);
					// jhc gcd to WINDUX: DONE
                    
                    // jhc gcd to WINDUX: original line below - kept because a little skeptical
                    //FtpSetCurrentDirectoryA(hFTP, dCurrent.getFullPath().c_str());
                }
            }

            WIN32_FIND_DATAA fileInfo;

            HINTERNET hFile = FtpFindFirstFileA(hFTP, "*.ini", &fileInfo, INTERNET_FLAG_NEED_FILE, 0);
            BOOL found = (hFile ? TRUE : FALSE);

            while (found)
            {
                currentFile.setFileName(fileInfo.cFileName);

                if (currentFile.getFileNameExtension() == "ini")
                {
                    SoftwareSystem *pPartialSystem = 0;

                    getIniFromFTP(hFTP, currentFile, &pPartialSystem);

                    if (pPartialSystem)
                    {
                        newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);

                        delete pPartialSystem;
                    }
                }

                found = InternetFindNextFileA(hFile, &fileInfo);
            }

            if (hFile)
            {
                PAL_absclass::PAL_internetCloseHandle( hFile );
            }

#else
            // this block is for linux
            PAL_INET_HANDLE hFile = (PAL_INET_HANDLE) 0;
            vector<ftpFileInfo> files;

            files = PAL_absclass::PAL_ftpFindFilesByExtension(hFTP, ".ini");

            for (unsigned int i=0; i<files.size(); i++)
            {
                currentFile.setFileName(files[i].name);

                SoftwareSystem *pPartialSystem = 0;

                getIniFromFTP(hFTP, currentFile, &pPartialSystem);

                if (pPartialSystem)
                {
                    newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);

                    delete pPartialSystem;
                }
            }
#endif      // PAL_WIN32_RUN

            if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
            {
                Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:new software system is invalid!!!!Dont Allow Upgrades"));
                VmStatusMsg status(NULL, VersionManagerResSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
                dispatchEvent(&status);
                vmTempDir.remove(true);
#if defined ( __linux__ )
                PAL_absclass::PAL_internetCloseHandle( hFile );
#endif
               return ERROR_CODE;
            }

            if (!newSoftwareSystem_->doesAnyComponentNeedInstalling())
            {
					Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:nothing new to install"));
                vmTempDir.remove(true);
#if defined ( __linux__ )
                PAL_absclass::PAL_internetCloseHandle( hFile );
#endif
                return OK;
            }

            //something to install.

            if (newSoftwareSystem_ != NULL)
                delete newSoftwareSystem_;

				Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:parsing current software system. all componenents are installed"));
            newSoftwareSystem_ = new SoftwareSystem(currentVersionIni, true);

#if defined ( PAL_WIN32_RUN )
            hFile = FtpFindFirstFileA(hFTP, "*.zip", &fileInfo, INTERNET_FLAG_NEED_FILE, 0);
            found = (hFile ? TRUE : FALSE);

            while (found)
            {
                currentFile.setFileName(fileInfo.cFileName);

                if (currentFile.getFileNameExtension() == "zip")
                {
                    SoftwareSystem *pPartialSystem = 0;

                    getSrcFromFTP(hFTP, currentFile, &pPartialSystem);

                    if (pPartialSystem)
                    {
                        newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);

                        delete pPartialSystem;
                    }
                }

                found = InternetFindNextFileA(hFile, &fileInfo);
            }

            if (hFile != PAL_INET_HANDLE_FAIL)
            {
                PAL_absclass::PAL_internetCloseHandle( hFile );
            }

#else
            files = PAL_absclass::PAL_ftpFindFilesByExtension(hFTP, ".zip");
            for (unsigned int i=0; i<files.size(); i++)
            {
                string s = msg->getPath() + files[i].name;
                currentFile.setFileName(s);

                SoftwareSystem *pPartialSystem = 0;

                getSrcFromFTP(hFTP, currentFile, &pPartialSystem);

                if (pPartialSystem)
                {
                    newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);

                    delete pPartialSystem;
                }
            }

            PAL_absclass::PAL_internetCloseHandle( hFile );

#endif      // PAL_WIN32_RUN

            PAL_absclass::PAL_internetCloseHandle( hFTP );

            if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
            {
				Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:new software system is invalid!!!! Don't Allow Upgrades"));
                VmStatusMsg status(NULL, VersionManagerResSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
                dispatchEvent(&status);
                vmTempDir.remove(true);
                return ERROR_CODE;
            }

            if (!newSoftwareSystem_->doesAnyComponentNeedInstalling())
            {
					Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:nothing new to install"));
                vmTempDir.remove(true);
                return OK;
            }


            //at this point something must be flashed.


				{//save the current custom internal config file, configc.dat, if it exists
					util_filesystem::File origCustomIntConfig (DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME);
					util_filesystem::File savedCustomIntConfig (INSTALLATION_CUSTOM_INTERNAL_CONFIG_FILE_NAME);

					if (savedCustomIntConfig.exists())
						savedCustomIntConfig.remove();

					if (origCustomIntConfig.exists())
						origCustomIntConfig.copyTo(savedCustomIntConfig);
				}


            if (newSoftwareSystem_->flashSystem(&hardwareComponentList_) == ERROR_CODE)
					Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:failed to flash the system"));


            if (newSoftwareSystem_->doesSystemNeedRebooting())
            {
					Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:system needs to be rebooted for proper install"));


                bool doesDBNeedBackup = newSoftwareSystem_->doesDBNeedBackingUp();

                if (doesDBNeedBackup)
                {
						Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:db needs to be backed up"));
                }
                else
                {
						Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:db does not need to be backed up"));
                }

                tryRequestShutdown(doesDBNeedBackup);

            }
            else
            {
					Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:upgrade usb peripheral only"));

                VmStatusMsg status(NULL, VersionManagerUsbUpgradeComplete, HfcResponse::RESPONSESTYLE_OK, false);
                dispatchEvent(&status);

                loadCurrentSystemComponents();
            }

        }
        else
        {
            Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:failed to connect to FTP servier"));
            Debug::trace("VersionManager.upgrade.FTP", HobString("server name = ") + msg->getFtpServerName());
            Debug::trace("VersionManager.upgrade.FTP", HobString("user name = ") + msg->getFtpUserName());
            Debug::trace("VersionManager.upgrade.FTP", HobString("password = ") + msg->getFtpPassword());
			Debug::trace("VersionManager.upgrade.FTP", HobString("upgradeServer = ") + msg->getUpgradeServer());

            VmStatusMsg status(NULL, VersionManagerErrorLoadingNewFirmware, HfcResponse::RESPONSESTYLE_OK, false);
            dispatchEvent(&status);
            vmTempDir.remove(true);
            return ERROR_CODE_FAILED_CONNECTION;
        }

        PAL_absclass::PAL_internetCloseHandle( hInet );
    }
    else
    {
        Debug::trace("VersionManager.upgrade.FTP", HobString("updateFirmWareFromTFTP:InternetOpen failed"));

        VmStatusMsg status(NULL, VersionManagerErrorLoadingNewFirmware, HfcResponse::RESPONSESTYLE_OK, false);
        dispatchEvent(&status);
        vmTempDir.remove(true);
        return ERROR_CODE_FAILED_CONNECTION;
    }

    return OK;

}

STATUS VersionManager::updateFirmWareFromServer(UpdateSoftwareAutomatically* msg)
{
	Configuration* pCFG = Configuration::getInstance();
	util_filesystem::Directory vmTempDir(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
	util_filesystem::File currentVersionIni (pCFG->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

	if (newSoftwareSystem_ != NULL)
		delete newSoftwareSystem_;

	newSoftwareSystem_ = new SoftwareSystem(currentVersionIni, true);
	
	if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
	{
		// test the hardware to see if it is valid to upgrade
		const string *pNew = NULL;

		Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromSERVER:existing software system is invalid!!!!Dont Allow Upgrades"));

		pNew = newSoftwareSystem_->toString();

		Debug::trace("VersionManager.upgrade.SERVER", *pNew);

		VmStatusMsg status(NULL, VersionManagerCurSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
		dispatchEvent(&status);
		return ERROR_CODE;
	}
	else
	{
		Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromSERVER:current software system is valid"));
	}

	if (vmTempDir.exists())
		vmTempDir.remove();

	vmTempDir.create();
	
	// this begins the upgrade procedure, this code is what came from inside: if (hInet)
	
	util_filesystem::File currentFile ("");

	if (msg->getUpgradeServer() != "")
	{
		Debug::trace("VersionManager.upgrade.SERVER", HobString("directory containing the upgrade files is ") + msg->getUpgradeServer());

		// name the files I want to copy from the remote file system
		util_filesystem::File remoteIniFile (msg->getUpgradeServer().c_str(), pCFG->toStringCfgI(CfgInternal::UPGRADEINIFILE));
		util_filesystem::File remoteZipFile (msg->getUpgradeServer().c_str(), pCFG->toStringCfgI(CfgInternal::UPGRADEZIPFILE));
		
		util_filesystem::File localIniFile (vmTempDir.getFullPath(), pCFG->toStringCfgI(CfgInternal::UPGRADEINIFILE));
		util_filesystem::File localZipFile (vmTempDir.getFullPath(), pCFG->toStringCfgI(CfgInternal::UPGRADEZIPFILE));
		
		bool copiedAll = true;

		if ( !(localIniFile << remoteIniFile) )        // Copy remote INI file to local file
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer: Could not copy remote INI file to local INI file"));
			copiedAll = false;
		}
		if ( !(localZipFile << remoteZipFile) )        // Copy remote ZIP file to local file	
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer: Could not copy remote ZIP file to local ZIP file"));
			copiedAll = false;
		}



		// i need to remove the files that I upgraded from, to get them off the system.
		if (remoteIniFile.exists())
			remoteIniFile.remove();
		
		if (remoteZipFile.exists())
			remoteZipFile.remove();


		if (!copiedAll)
		{	
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:files did not get copied from file system"));
			vmTempDir.remove(true);
			return OK;
		}
	
					
		// at this point we have supposebly copied the files, lets merge the INI
		if (localIniFile.exists())
		{
			SoftwareSystem *pPartialSystem = 0;
			pPartialSystem = new SoftwareSystem(localIniFile, true);

			if (pPartialSystem)
			{
				//getIniFromFileSystem();
				newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);
				delete pPartialSystem;
			}
		}
		else
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:local ini file was not created so exiting."));
			vmTempDir.remove(true);
			return OK;
		}

		if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:new software system is invalid!!!!Dont Allow Upgrades"));
			VmStatusMsg status(NULL, VersionManagerResSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
			dispatchEvent(&status);
			vmTempDir.remove(true);
			return ERROR_CODE;
		}

		// lets see if I comment out this test, lets see if it updates.
		//if (!newSoftwareSystem_->doesAnyComponentNeedInstalling())
		//{
		//	Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:nothing new to install"));
		//	vmTempDir.remove(true);
		//	return OK;
		//}

		//something to install.
		
		if (newSoftwareSystem_ != NULL)
			delete newSoftwareSystem_;

		Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:parsing current software system. all componenents are installed"));
		newSoftwareSystem_ = new SoftwareSystem(currentVersionIni, true);

		// lets unzip and merge some stuff
		if (localZipFile.exists())
		{
			SoftwareSystem *pPartialSystem = 0;

			//if (pPartialSystem)
			//{

	//////////////////////////////////////////////////////////////////////////////////////////////////
				
				STATUS stat = ERROR_CODE;

				Hobart::CZip::unZipSingleFile(localZipFile.getFullPath().c_str(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME).c_str(), localZipFile.getPath().c_str());

				File verIniTargetLoc (localZipFile.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

				if (verIniTargetLoc.exists())
				{
					Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:remote version.ini exists. create software system"));
					pPartialSystem = new SoftwareSystem(verIniTargetLoc);

					HobComplexVersion * pVersion = (*pPartialSystem).findMajorComponent();
					if (pVersion)
					{
						Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:moving zip file to safe area"));
						util_filesystem::Directory tmpDir("");
						util_filesystem::File tmpZip ("");
						HobComplexVersion::getTempDir(pVersion->getVersionType(), tmpDir, tmpZip);
						
						if (tmpDir.exists())
							tmpDir.removeContents(true);
						else
							tmpDir.create();

						localZipFile.moveTo(tmpZip);

						if (tmpZip.exists())
						{
							//this is just a check for file corruptions
							if (Hobart::CZip::unZip(tmpZip.getFullPath().c_str(), tmpZip.getPath().c_str()) == 0)
							{

								Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:zip completely extracted ok! no file corruptions!"));

								File verIniTmpLoc (tmpZip.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));
								if (verIniTmpLoc.exists())
								{
									Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:version.ini file exists! all is good."));

									stat = OK;
								}

							}
							else
							{
								tmpDir.remove(true);
								Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:zip file failed to completely extract!zip file must be corrupted or incomplete!"));
							}

						}
					}
					else
					{
						Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:something is wrong with version.ini file"));
					}

					verIniTargetLoc.remove();

				}
				else
				{
					Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:version.ini DOES NOT EXIST"));
				}

				//if ((stat != OK) && (*pPartialSystem != 0))
				if ((stat != OK) && (pPartialSystem != 0))
				{
					delete pPartialSystem;
					pPartialSystem = 0;
				}

	//////////////////////////////////////////////////////////////////////////////////////////////////

				if (pPartialSystem)
				{
					newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_);
					delete pPartialSystem;
				}
			//}
		}
		else
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:local ZIP file was not created so exiting."));
			vmTempDir.remove(true);
			return OK;
		}
		
		
		if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:new software system is invalid!!!!Dont Allow Upgrades"));
			VmStatusMsg status(NULL, VersionManagerResSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
			dispatchEvent(&status);
			vmTempDir.remove(true);
			return ERROR_CODE;
		}

		if (!newSoftwareSystem_->doesAnyComponentNeedInstalling())
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:nothing new to install"));
			vmTempDir.remove(true);
			return OK;
		}

		//at this point something must be flashed.


		{//save the current custom internal config file, configc.dat, if it exists
			util_filesystem::File origCustomIntConfig (DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME);
			util_filesystem::File savedCustomIntConfig (INSTALLATION_CUSTOM_INTERNAL_CONFIG_FILE_NAME);

			if (savedCustomIntConfig.exists())
				savedCustomIntConfig.remove();

			if (origCustomIntConfig.exists())
				origCustomIntConfig.copyTo(savedCustomIntConfig);
		}


		if (newSoftwareSystem_->flashSystem(&hardwareComponentList_) == ERROR_CODE)
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:failed to flash the system"));


		if (newSoftwareSystem_->doesSystemNeedRebooting())
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:system needs to be rebooted for proper install"));


			bool doesDBNeedBackup = newSoftwareSystem_->doesDBNeedBackingUp();

			if (doesDBNeedBackup)
			{
				Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:db needs to be backed up"));
			}
			else
			{
				Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:db does not need to be backed up"));
			}

			tryRequestShutdown(doesDBNeedBackup);

		}
		else
		{
			Debug::trace("VersionManager.upgrade.SERVER", HobString("updateFirmWareFromServer:upgrade usb peripheral only"));

			VmStatusMsg status(NULL, VersionManagerUsbUpgradeComplete, HfcResponse::RESPONSESTYLE_OK, false);
			dispatchEvent(&status);
			
			loadCurrentSystemComponents();
		}
	}

	return OK;
}

STATUS VersionManager::getSrcFromThumbDrive(util_filesystem::File& srcZip,
        SoftwareSystem** pNewSC)
{
    STATUS stat = ERROR_CODE;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
    util_filesystem::File destZip (vmTempDir.getFullPath(), srcZip.getFileName());

    *pNewSC = 0;

    if (!vmTempDir.exists())
        vmTempDir.create();

    if (destZip.exists())
        destZip.remove();

    destZip << srcZip;

    if (destZip.exists())
    {
        Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  TAR archive is OK"));

        Hobart::CZip::unZipSingleFile(destZip.getFullPath().c_str(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME).c_str(), destZip.getPath().c_str());

        File verIniTargetLoc (destZip.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

        if (verIniTargetLoc.exists())
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  version.ini exists. create software system"));
            *pNewSC = new SoftwareSystem(verIniTargetLoc);

            HobComplexVersion * pVersion = (*pNewSC)->findMajorComponent();
            if (pVersion)
            {
                Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  Moving zip file to safe area"));
                util_filesystem::Directory tmpDir("");
                util_filesystem::File tmpZip ("");
                HobComplexVersion::getTempDir(pVersion->getVersionType(), tmpDir, tmpZip);
//               	Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  tmpDir = ") + tmpDir.getFullPath() + ",  tmpZip = " + tmpZip.getFullPath());

                if (tmpDir.exists())
                    tmpDir.removeContents(true);
                else
                    tmpDir.create();

                destZip.moveTo(tmpZip);

                if (tmpZip.exists())
                {
                    //this is just a check for file corruptions
                    if (Hobart::CZip::unZip(tmpZip.getFullPath().c_str(), tmpZip.getPath().c_str()) == 0)
                    {
                        Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  ZIP completely extracted ok! no file corruptions!"));

                        File verIniTmpLoc (tmpZip.getPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));
                        if (verIniTmpLoc.exists())
                        {
                            Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  version.ini file exists! all is good."));

#if defined ( __linux__ )							
							//change permission of all upgrade files
							int result = system("chmod -R 777 /etc/hobart/vmTmp");
											
							if (result == -1)
							{
								Debug::trace("Session", "Failed to change permissions of upgrade files");
							}
#endif

                            stat = OK;
                        }
                    }
                    else
                    {
                        tmpDir.remove(true);
                        Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  ZIP file failed to completely extract!zip file must be corrupted or incomplete!"));
                    }
                }
            }
            else
            {
                Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  Something is wrong with version.ini file"));
            }

            verIniTargetLoc.remove();
        }
        else
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  version.ini DOES NOT EXIST"));
        }
    }
    else
        Debug::trace("VersionManager.upgrade.USB", HobString("getSrcFromThumbDrive:  Failed to copy zip file"));

    if ((stat != OK) && (*pNewSC != 0))
    {
        delete *pNewSC;
        *pNewSC = 0;
    }

    if (destZip.exists())
        destZip.remove();


    return stat;
}

#if defined( PAL_WIN32_RUN )
STATUS VersionManager::disableHobartServicesOnBootUp()
{
    SC_HANDLE schSCManager;

    schSCManager = OpenSCManager( 
            NULL,                    // local machine 
            NULL,                    // ServicesActive database 
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (schSCManager)
    {
        disableAutoStartService(schSCManager, Configuration::getInstance()->toStringCfgI(CfgInternal::SCALESERVICENAME));
        //there is a problem with the IIS webserver conflicting with the goahead webserver if
        //we disable the goahead web service now
        disableAutoStartService(schSCManager, Configuration::getInstance()->toStringCfgI(CfgInternal::WEBSERVICENAME));

		// Disable the appRunOnce service (originally created for Walmart, to look for RunOnce strings at bootup)
		disableAutoStartService(schSCManager, std::string("appRunOnce"));

        CloseServiceHandle(schSCManager);
        return OK;
    }

    return ERROR_CODE;
}

STATUS VersionManager::disableAppLaunchOnBootUp()
{
    disableHobartServicesOnBootUp();
    disableAutoStartIE();

    return OK;
}

STATUS VersionManager::enableAppInstallerOnReboot()
{
    STATUS result = ERROR_CODE;
    HKEY hKey;

	if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::APPINSTALLERISSERVICE))
	{
		util_filesystem::File fAppInstallerPath(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR), Configuration::getInstance()->toStringCfgI(CfgInternal::APPINSTALLEREXE));

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SOFTWARE\\microsoft\\windows\\CurrentVersion\\Run"), 
			0, KEY_WRITE, &hKey))
		{
			Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:failed to open ie run key"));
		}
		else
		{
			std::string sAppInstallerPath = fAppInstallerPath.getFullPath();

			if (RegSetValueExA(hKey,              // subkey handle 
					"appInstaller",        // value name 
					0,                         // must be zero 
					REG_SZ,             // value type 
					(LPBYTE) sAppInstallerPath.c_str(),        // pointer to value data 
               		(DWORD)sAppInstallerPath.size()+1)) // length of value data 
			{
				Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:failed to create appInstaller key value"));
			}
			else
			{
				Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:created appInstaller key value"));
				result = OK;
			}
		}

		if (hKey)
			RegCloseKey(hKey);
	}
	else
	{
		bool startNow = true;
		SC_HANDLE schSCManager;

		schSCManager = OpenSCManager( 
			NULL,                    // local machine 
			NULL,                    // ServicesActive database 
			SC_MANAGER_ALL_ACCESS);  // full access rights

		if (!schSCManager)
			return result;
		
		SC_HANDLE schService = 0;
		SC_LOCK sclLock = 0; 

		// Need to acquire database lock before reconfiguring. 

		do
		{
			sclLock = LockServiceDatabase(schSCManager);
			if (!sclLock)
			{
				//std::cout << ".";
				PAL_absclass::PAL_sleep (1000);
			}

		}
		while (!sclLock);

		//std::cout << std::endl << "got the scl lock!" << std::endl;

		if (sclLock)
		{

			schService = OpenServiceA( 
				schSCManager,           // SCManager database 
				"appInstaller",           // name of service 
				SERVICE_ALL_ACCESS); // need CHANGE access 
			if (schService)
			{

				if (! ChangeServiceConfig( 
					schService,        // handle of service 
					SERVICE_NO_CHANGE, // service type: no change 
					SERVICE_AUTO_START,       // change service start type 
					SERVICE_NO_CHANGE, // error control: no change 
					NULL,              // binary path: no change 
					NULL,              // load order group: no change 
					NULL,              // tag ID: no change 
					NULL,              // dependencies: no change 
					NULL,              // account name: no change 
					NULL,              // password: no change 
					NULL) )            // display name: no change
				{
					Debug::trace("VersionManager.upgrade", "Failed to change Service to auto start!");
				}
				else 
				{
					Debug::trace("VersionManager.upgrade", "Service found and changed to auto start!");
				}
			}
			else
			{
				Debug::trace("VersionManager.upgrade", "Failed to open Service!");

				DWORD dw = GetLastError(); 
				if (dw == ERROR_SERVICE_DOES_NOT_EXIST)
				{
					Debug::trace("VersionManager.upgrade", "Service does not exist!");

					schService = CreateServiceA( 
						schSCManager,              // SCManager database 
						"appInstaller",        // name of service 
						"appInstaller",// service name to display 
						SERVICE_ALL_ACCESS,        // desired access 
						SERVICE_WIN32_OWN_PROCESS, // service type 
						SERVICE_AUTO_START,      // start type 
						SERVICE_ERROR_NORMAL,      // error control type 
						"appInstaller.exe",// path to service's binary 
						NULL,                      // no load ordering group 
						NULL,                      // no tag identifier 
						NULL,                      // no dependencies 
						NULL,                      // LocalSystem account 
						NULL);                     // no password 

					if (schService)
					{
						Debug::trace("VersionManager.upgrade", "Service created!");
					}
					else
					{
						Debug::trace("VersionManager.upgrade", "Failed to create Service!");
					}
				}
			}

			if (sclLock)
				UnlockServiceDatabase(sclLock); 

	 
			if (schService && startNow)
			{

				if (!StartService(
						schService,  // handle to service 
						0,           // number of arguments 
						NULL) )      // no arguments 
				{
					std::cout << "Failed to start Service!!!!" << std::endl;
				}
				else 
				{
					std::cout << "Service started!!!!" << std::endl;
				}
			}
		}
		else
		{
			std::cout << "FAILED to lock service database!!!! " << std::endl;
		}

	 
		// Close the handle to the service. 
		if (schService)
			CloseServiceHandle(schService); 
	}


    util_filesystem::Directory tmpDir("");
    util_filesystem::File tmpZip ("");
    HobComplexVersion::getTempDir(HobVersion::SystemControllerSW, tmpDir, tmpZip);
//    Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:  tmpDir = ") + tmpDir.getFullPath() + ",  tmpZip = " + tmpZip.getFullPath());

    File srcAppInstExe (tmpDir.getFullPath(), Configuration::getInstance()->toStringCfgI(CfgInternal::APPINSTALLEREXE));
    File destAppInstExe(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR), Configuration::getInstance()->toStringCfgI(CfgInternal::APPINSTALLEREXE));

    if (!srcAppInstExe.exists())
        srcAppInstExe.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));

    Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:  Copying appInstaller.exe from: ") + srcAppInstExe.getFullPath());
    Debug::trace("VersionManager.upgrade", HobString("enableAppInstallerOnReboot:  Copying appInstaller.exe to: ") + destAppInstExe.getFullPath());

    if (destAppInstExe.exists())
        destAppInstExe.remove();

    destAppInstExe << srcAppInstExe;

    PAL_absclass::PAL_sleep( 1000 );

    return result;
}

STATUS VersionManager::disableAutoStartIE()
{
    STATUS result = ERROR_CODE;
    HKEY hKey = 0;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SOFTWARE\\microsoft\\windows\\CurrentVersion\\Run"), 
                0, KEY_WRITE, &hKey))
    {
        Debug::trace("VersionManager.upgrade", HobString("disableAppLaunchOnBootUp:failed to open ie explorer\run key"));
    }
    else
    {
        if (RegDeleteValue(hKey, _T("appLauncher")))
        {
            Debug::trace("VersionManager.upgrade", HobString("disableAppLaunchOnBootUp:failed to delete appLauncher key value"));
        }
        else
        {
            Debug::trace("VersionManager.upgrade", HobString("disableAppLaunchOnBootUp:deleted appLauncher key value"));
            result = OK;
        }
    }

    if (hKey)
        RegCloseKey(hKey);

    return result;
}

STATUS VersionManager::disableAutoStartService(SC_HANDLE schSCManager, const std::string& serviceName)
{
    STATUS result = ERROR_CODE;
    SC_HANDLE schService = 0;
    SC_LOCK sclLock = 0; 

    Debug::trace("VersionManager.upgrade", HobString("disableAutoStartService:Attempt to change service to manual start. name = ") + serviceName);
    // Need to acquire database lock before reconfiguring. 

    sclLock = LockServiceDatabase(schSCManager); 

    if (sclLock)
    {

        schService = OpenServiceA( 
                schSCManager,           // SCManager database 
                serviceName.c_str(),           // name of service 
                SERVICE_ALL_ACCESS); // need CHANGE access 
        if (schService)
        {

            if (! ChangeServiceConfig( 
                        schService,        // handle of service 
                        SERVICE_NO_CHANGE, // service type: no change 
                        SERVICE_DEMAND_START, // change service start type 
                        SERVICE_NO_CHANGE, // error control: no change 
                        NULL,              // binary path: no change 
                        NULL,              // load order group: no change 
                        NULL,              // tag ID: no change 
                        NULL,              // dependencies: no change 
                        NULL,              // account name: no change 
                        NULL,              // password: no change 
                        NULL) )            // display name: no change
            {
				Debug::trace("VersionManager.upgrade", HobString("disableAutoStartService:  Failed to change to manual start.") );
            }
            else 
            {
				Debug::trace("VersionManager.upgrade", HobString("disableAutoStartService:  Changed to manual start."));
                result = OK;
            }
        }
        else
        {
			Debug::trace("VersionManager.upgrade", HobString("disableAutoStartService:  Failed to open service."));
        }
    }
    else
    {
		Debug::trace("VersionManager.upgrade", HobString("disableAutoStartService:  Failed to lock service database."));
    }

    if (schService)
        CloseServiceHandle(schService);

    if (sclLock)
        UnlockServiceDatabase(sclLock);

    return result;
}

#elif   defined( __linux__ )
STATUS VersionManager::disableHobartServicesOnBootUp()
{
    STATUS result = ERROR_CODE;

    // ***** WARNING: Service Manager is not available in Linux, need to implement another mechanism.

    return result;
}

//STATUS disableAutoStartService();
//STATUS disableAutoStartIE();
//STATUS enableAppInstallerOnReboot();
//STATUS disableAppLaunchOnBootUp();

#else
#error "Implement something like _Service Manager_ for your system."

#endif

STATUS VersionManager::updateFirmWareFromThumbDrive(const string& path)
{
    Configuration* pCFG = Configuration::getInstance();
    bool bForce = pCFG->toBoolCfgI(CfgInternal::VERSIONMANAGERFORCEUPGRADE);
    util_filesystem::Directory vmTempDir("");
    vmTempDir.appendPath(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));
    util_filesystem::Directory workingDir("");
    workingDir.appendPath(path);
    util_filesystem::Directory::const_iterator dirIter;
    util_filesystem::File currentVersionIni (pCFG->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

    if (newSoftwareSystem_ != NULL)
        delete newSoftwareSystem_;

    Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Parsing current software system. All componenents are installed"));
    newSoftwareSystem_ = new SoftwareSystem(currentVersionIni, true);
    if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
    {
        const string *pNew = NULL;

        Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Existing software system is invalid!!!!Dont Allow Upgrades"));

        pNew = newSoftwareSystem_->toString();

        Debug::trace("VersionManager.upgrade.USB", *pNew);

        VmStatusMsg status(NULL, VersionManagerCurSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
        dispatchEvent(&status);
        return ERROR_CODE;
    }
    else
    {
        Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Current software system is valid"));
    }


    if (vmTempDir.exists())
        vmTempDir.remove(true);

    workingDir.select("*.zip");

    dirIter = workingDir.begin();

    if (dirIter == workingDir.end( ))
    {
#ifndef EVOL_NDEBUG
        Debug::trace("VersionManager.upgrade.USB", "nothing to install");
#endif
        //VmStatusMsg status(NULL, VersionManagerNothingNewToInstall, HfcResponse::RESPONSESTYLE_OK, false);
        //dispatchEvent(&status);

        Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Returning OK"));
        return OK;
    }
    else
    {
        VmStatusMsg status(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, false);
        dispatchEvent(&status);

        PAL_absclass::PAL_sleep( 500 );

        while (dirIter != workingDir.end( ))
        {
            File fullPath (workingDir.getFullPath(), dirIter->getName());
            SoftwareSystem *pPartialSystem = 0;

            getSrcFromThumbDrive(fullPath, &pPartialSystem);

            if (pPartialSystem)
            {
                newSoftwareSystem_->mergeSystems(pPartialSystem, onlyNewerVersions_, bForce);
                delete pPartialSystem;
            }

            ++dirIter;
        }

        if (newSoftwareSystem_->validateSoftwareSystem(hardwareComponentList_) != HobDependency::DependencySucceeded)
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  New software system is invalid!!!!  Do not Allow Upgrades"));
            VmStatusMsg rstatus(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, true);
            dispatchEvent(&rstatus);
            VmStatusMsg status(NULL, VersionManagerResSysInvalid, HfcResponse::RESPONSESTYLE_OK, false);
            dispatchEvent(&status);
            vmTempDir.remove(true);
            return ERROR_CODE;
        }

        if (!newSoftwareSystem_->doesAnyComponentNeedInstalling())
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Nothing new to install"));
            VmStatusMsg rstatus(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, true);
            dispatchEvent(&rstatus);
            VmStatusMsg status(NULL, VersionManagerNothingNewToInstall, HfcResponse::RESPONSESTYLE_OK, false);
            dispatchEvent(&status);
            vmTempDir.remove(true);
            return OK;
        }


        //at this point something must be flashed.


        //save the current custom internal config file, configc.dat, if it exists
		util_filesystem::File origCustomIntConfig (DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME);
		util_filesystem::File savedCustomIntConfig (INSTALLATION_CUSTOM_INTERNAL_CONFIG_FILE_NAME);

		if (savedCustomIntConfig.exists())
			savedCustomIntConfig.remove();

		if (origCustomIntConfig.exists())
			origCustomIntConfig.copyTo(savedCustomIntConfig);


        if (newSoftwareSystem_->flashSystem(&hardwareComponentList_) == ERROR_CODE)
            Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Failed to flash the system"));


        if (newSoftwareSystem_->doesSystemNeedRebooting())
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  System needs to be rebooted for proper install"));

            bool doesDBNeedBackup = newSoftwareSystem_->doesDBNeedBackingUp();

            if (doesDBNeedBackup)
            {
                Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Db needs to be backed up"));
            }
            else
            {
                Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Db does not need to be backed up"));
            }

            tryRequestShutdown(doesDBNeedBackup);

        }
        else
        {
            Debug::trace("VersionManager.upgrade.USB", HobString("updateFirmWareFromThumbDrive:  Upgrade usb peripheral only"));

            VmStatusMsg rstatus(NULL, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT, true);
            dispatchEvent(&rstatus);
            VmStatusMsg status(NULL, VersionManagerUsbUpgradeComplete, HfcResponse::RESPONSESTYLE_OK, false);
            dispatchEvent(&status);

            loadCurrentSystemComponents();

            Debug::trace("VersionManager", HobString("Send Request"));
            //request the code revisions from the AWS Wrapper, this will kick off the upgrade process.
            EvolMessage cCheckMsg(VersionManagerDev, ComponentCheckComplete);
            dispatchEvent(&cCheckMsg);
        }

        return OK;
    }
}

void VersionManager::tryRequestShutdown(bool convertDB)
{
    if (safeToReboot_)
    {
        Debug::trace("VersionManager.upgrade", HobString("tryRequestShutdown:  Fully booted up. Request reboot."));

#if defined( WIN32 )
        disableAppLaunchOnBootUp();
        enableAppInstallerOnReboot();

#elif   defined( __linux__ )
        // ***** WARNING: we need the equivlent functions in Linux

#else
#error "Implement something for your system."
#endif

        requestShutdown(convertDB);
    }
    else
    {
        Debug::trace("VersionManager.upgrade", HobString("tryRequestShutdown:  Wait for system to fully boot up before requesting reboot."));

        rebootRequested_ = true;
        convertDb_ = convertDB;
    }
}

STATUS VersionManager::requestShutdown(bool convertDB)
{
    ShutdownSystemRequest msg(convertDB);
    dispatchEvent(&msg);

    rebootRequested_ = true;

    Debug::trace("VersionManager.upgrade", HobString("requestShutdown:  Sent out the reboot system message"));

    return OK;
}

STATUS VersionManager::loadOSVersion()
{
#if !defined ( __linux__ )
    const string * componentVer = 0;
    HobVersion * hwVersion = 0;
    string newVersion("");

    File fOSVerFile("c:\\", "WERuntime.INI");

    util_env::Properties OSVerProps(fOSVerFile.getFullPath());
    Debug::trace("VersionManager", "loadOSVersion");

    componentVer = OSVerProps.getProperty("RunTimeOEMRev");

    if (componentVer)
    {
        if (*componentVer == "")
            newVersion = "3.1.2.0";
        else 
            newVersion = *componentVer;
    }

    Debug::trace("VersionManager", string ("RunTimeOEMRev = ") + newVersion);

    if (HobVersion::isAValidFullVersionString(newVersion))
    {
        hwVersion = new HobVersion(newVersion);
    }
    else
    {
        hwVersion = new HobVersion(HobVersion::XPEImageSW, 0, 0);
    }

    HobString msg("loadOSVersion:Hardware version type:");
    msg += Integer::toString(hwVersion->getVersionType());
    msg += " maj: ";
    msg += Integer::toString(hwVersion->getMajorRevision());
    msg += " min: ";
    msg += Integer::toString(hwVersion->getMinorRevision());

    Debug::trace("VersionManager", msg);

    if (hwVersion)
    {
        Debug::trace("VersionManager", "adding to hardware list");

        addAHardwareComponent(0, hwVersion);
    }

#else
    HobVersion * hwVersion = 0;
    string newVersion("");
    string component("");

    struct utsname uts;
    Debug::trace("VersionManager", "loadOSVersion");

    component = "3.";
    uname( &uts );

    string osVersion(&uts.release[0]);

    newVersion = component + osVersion;//"3.3.0.35"
    Debug::trace("VersionManager", string ("Component = ") + component);
    Debug::trace("VersionManager", string ("OS = ") + osVersion);
    Debug::trace("VersionManager", string ("RunTimeOEMRev = ") + newVersion);

    hwVersion = new HobVersion(newVersion);
    HobString msg("loadOSVersion:Hardware version type:");
    msg += Integer::toString(hwVersion->getVersionType());
    msg += " maj: ";
    msg += Integer::toString(hwVersion->getMajorRevision());
    msg += " min: ";
    msg += Integer::toString(hwVersion->getMinorRevision());

    Debug::trace("VersionManager", msg);

    if (hwVersion)
    {
        Debug::trace("VersionManager", "adding to hardware list");

        addAHardwareComponent(0, hwVersion);
    }

#endif
    return OK;
}

STATUS VersionManager::loadSystemControllerHardwareVersion()
{

    Debug::trace("VersionManager", "loadSystemControllerHardwareVersion");

    USHORT model = 0;
/*replaced with fastrack serial flash
    HobVersion * hwVersion = new HobVersion();
    MainBoardEEPROM * pSerialEEP = MainBoardEEPROM::getInstance();

    Smbus::SmbusStatus stat = pSerialEEP->readHardwareRevision(*hwVersion);
    stat = pSerialEEP->readModelType(&model);

    HobString msg("loadSystemControllerHardwareVersion:Hardware version type:");
    msg += Integer::toString(hwVersion->getVersionType());
    msg += " maj: ";
    msg += Integer::toString(hwVersion->getMajorRevision());
    msg += " min: ";
    msg += Integer::toString(hwVersion->getMinorRevision());
    msg += " model: ";
    msg += Integer::toString(model);

    Debug::trace("VersionManager", msg);


    if (stat == Smbus::STATUS_SUCCEEDED)
    {
        Debug::trace("VersionManager", "adding to hardware list");

        addAHardwareComponent(0, hwVersion);
    }
*/
    return OK;
}


void VersionManager::addAHardwareComponent(USBD_NODE_ID nodeId, HobVersion *version)
{
    HobVersion *pHardVer = NULL;
    ComponentListIterator i;

    HobString msg("addAHardwareComponent:Adding a HC: ");
    msg += Hex::toString((long long)nodeId);		//ULong::toString( HandleToULong(nodeId) );

    Debug::trace("firmware.upgrade", msg);

    //if it already exists delete the old one. this shouldnt happen since the node ids are unique.
    i = hardwareComponentList_.find(nodeId);

    if (i != hardwareComponentList_.end())
    {
        Debug::trace("firmware.upgrade", "addAHardwareComponent:  nodeId found - will delete");

        pHardVer = i->second;
        delete pHardVer;
        hardwareComponentList_.erase(nodeId);
    }

    hardwareComponentList_.insert(make_pair(nodeId, version));
}


bool VersionManager::removeAHardwareComponent(USBD_NODE_ID nodeId)
{
    HobVersion *pHardVer = NULL;
    ComponentListIterator i;

    HobString msg("removeAHardwareComponent:Removing a HC: ");
    msg += Hex::toString((long long)nodeId);	//ULong::toString( HandleToULong(nodeId) );

    Debug::trace("firmware.upgrade", msg);

    i = hardwareComponentList_.find(nodeId);

    if (i != hardwareComponentList_.end())
    {
		Debug::trace("firmware.upgrade", "removeAHardwareComponent:  nodeId found - will delete.");

        pHardVer = i->second;
        delete pHardVer;
        hardwareComponentList_.erase(nodeId);
        return true;
    }
    return false;
}


bool VersionManager::dumpHardwareComponents()
{
    const HobVersion * pCurComponent;
    ComponentListIterator i;

    string tmpMsg("dumpHardwareComponents: list = ");

    for (i = hardwareComponentList_.begin(); i != hardwareComponentList_.end(); ++i)
    {
        pCurComponent = i->second;

        if (pCurComponent)
        {
            tmpMsg += "nodeId: ";
            tmpMsg += Hex::toString((long long)i->first);		//ULong::toString( HandleToULong(i->first) );
            tmpMsg += "V: ";
            tmpMsg += pCurComponent->getRevisionString();
            tmpMsg += "\n";
        }
    }
	Debug::trace("VersionManager.upgrade", tmpMsg);

    return false;
}


//#if defined(PAL_WIN32_RUN)
STATUS VersionManager::handleRemoteGraphicsFiles(UpdateSoftwareAutomatically* msg)
{
	Debug::trace("VersionManager", "VersionManager::handleRemoteGraphicsFiles()");
    PAL_INET_HANDLE hInet;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir("");
    vmTempDir.appendPath(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));

    util_filesystem::Directory tmpDir("");
    tmpDir.appendPath(vmTempDir.getFullPath());
    tmpDir.appendPath("Graphics");

	hInet = PAL_absclass::PAL_internetOpen(("VersionManager"), PAL_INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0); 

	if (hInet != PAL_INET_HANDLE_FAIL)
	{
		Debug::trace("VersionManager.graphics", "handleRemoteGraphicsFiles:  Opened ftp conntection.");

		util_filesystem::File currentFile ("");

		util_filesystem::Directory dCurrent ("");
		if (msg->getPath() != "")
			dCurrent.appendPath(msg->getPath());
		dCurrent.appendPath("Graphics");

		copyFtpDirectory(msg, hInet, dCurrent, tmpDir);
    }

	if (hInet != PAL_INET_HANDLE_FAIL)
		PAL_absclass::PAL_internetCloseHandle( hInet );

    if (tmpDir.exists() && !tmpDir.isEmpty())
    {
        Debug::trace("VersionManager.graphics", HobString("handleRemoteGraphicsFiles:  We have graphics files. so process"));

        if (importManager_)
            delete importManager_;

        registerEvent(ImportDev, ImportTerminate);

        importManager_ = new ImportManager(tmpDir.getFullPath(), false);
    }

    return OK;

}

void VersionManager::getLastDir(std::string & searchPath, std::string & subDir)
{
    string tmpString(""), lastDir("");

	for (size_t i = 0; i < searchPath.size(); i++)
	{
		if ((searchPath.at(i) == '\\') || (searchPath.at(i) == '/'))
		{
			lastDir = tmpString;
			tmpString = "";
		}
		else
			tmpString.append(1, toupper(searchPath.at(i)));
	}

    if (tmpString != "")
        subDir = tmpString;
    else
        subDir = lastDir;
}

void VersionManager::clearVersionIniComponents()
{
	if( deletedFirmware_ )
	{
		// bc we have to delete CLA and Wrapper and dont want to do this until both are deleted.

		if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
		{
			const string command( PythonScripter::buildPythonCall("deleteComponents.py") );
			string componentsStatusOutput;			// though honestly we don't care about any output from the command.
			PAL_absclass::PAL_executeWithResults(command, componentsStatusOutput);
		}

		deletedFirmware_ = false;
	}
	else
	{
		deletedFirmware_ = true;
	}
}

void VersionManager::copyFtpDirectory(
        UpdateSoftwareAutomatically* msg, 
		PAL_INET_HANDLE hInet,
        util_filesystem::Directory & currentSrcDir, 
        util_filesystem::Directory & currentDestDir)
{
    PAL_INET_HANDLE hFile   = PAL_INET_HANDLE_FAIL;
    PAL_INET_HANDLE hFTP    = PAL_INET_HANDLE_FAIL;
#if !defined ( __linux__) // linux can open the full path at once
    WIN32_FIND_DATAA fileInfo;
#endif      // __linux__
    list<string> dirList;

    string path = msg->getFtpServerName();
#if defined ( __linux__) // linux can open the full path at once
    path += currentSrcDir.getFullPath().c_str();
    validateFtpPath(path);

#endif      // __linux__
    hFTP = PAL_absclass::PAL_internetConnect(
            hInet,
            path.c_str(),
            PAL_INTERNET_DEFAULT_FTP_PORT,
            msg->getFtpUserName().c_str(), 
            msg->getFtpPassword().c_str(),
            PAL_INTERNET_SERVICE_FTP,
            0,
            0);
    if (hFTP)
    {

        if (!currentDestDir.exists())
            currentDestDir.create();

#ifdef PAL_WIN32_RUN
        // jhc gcd to WINDUX: START new code to replace original FtpSetCurrentDirectoryA below
		// Code is different than Pevola version in Walmart. This needs throughly tested
        CHAR slashCorrectedCurrentSrcDir[MAX_PATH] = { 0 };
		for (unsigned int i = 0; i < currentSrcDir.getFullPath().length(); ++ i)
			slashCorrectedCurrentSrcDir[i] = (currentSrcDir.getFullPath()[i] == '\\') ? '/' : currentSrcDir.getFullPath()[i];

		FtpSetCurrentDirectoryA(hFTP, slashCorrectedCurrentSrcDir);
		// jhc gcd to WINDUX: DONE
		
		// jhc gcd to WINDUX: original line below - kept because a little skeptical
		//FtpSetCurrentDirectoryA(hFTP, currentSrcDir.getFullPath().c_str());

        {
            CHAR currentDir[MAX_PATH];
            DWORD size = sizeof(currentDir);
            std::string requestedPath(currentSrcDir.getFullPath());
            std::string requestedLastDir("");

            getLastDir(requestedPath, requestedLastDir);

            if (FtpGetCurrentDirectoryA(hFTP, currentDir, &size))
            {
                std::string actualPath(currentDir);
                std::string actualLastDir("");
                getLastDir(actualPath, actualLastDir);

                if (actualLastDir != requestedLastDir)
                    return;
            }
        }

        hFile = FtpFindFirstFileA(hFTP, "*.*", &fileInfo, INTERNET_FLAG_NEED_FILE, 0);
		BOOL found = (hFile ? TRUE : FALSE);
        while (found)
        {
            if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((strcmp (fileInfo.cFileName, ".") != 0) && (strcmp (fileInfo.cFileName, "..") != 0))
                    dirList.push_back(fileInfo.cFileName);
            }
            else
            {
                util_filesystem::File destFile (currentDestDir.getFullPath(), fileInfo.cFileName);

                if (FtpGetFileA(hFTP, fileInfo.cFileName, destFile.getFullPath().c_str(), false,
                            FILE_ATTRIBUTE_NORMAL,FTP_TRANSFER_TYPE_BINARY, 0))
                {
                    Debug::trace("VersionManager.ftp", HobString("handleRemoteGraphicsFiles:  Succesfully got remote file"));
                }
                else
                {
                    Debug::trace("VersionManager.ftp", HobString("handleRemoteGraphicsFiles:  Failed to get remote file"));
                }
            }

            found = InternetFindNextFileA(hFile, &fileInfo);
        }
#else
        hFile = (PAL_INET_HANDLE) 0;
        vector<ftpFileInfo> files;

        files = PAL_absclass::PAL_ftpFindFilesByExtension(hFTP, "");      // null for match all
        for (unsigned int i=0; i<files.size(); i++)
        {
            if (files[i].bDir)
            {
                dirList.push_back(files[i].name);
            }
            else
            {
                string s = msg->getPath() + files[i].name;
                util_filesystem::File destFile (currentDestDir.getFullPath(), files[i].name);
                PAL_absclass::PAL_ftpGetFile( 
                        hFTP, 
                        s.c_str(), 
                        destFile.getFullPath().c_str(), 
                        false,
                        PAL_FILE_ATTR_NORMAL,
                        PAL_FTP_TRANSFER_TYPE_BINARY, 
                        0);
            }
        }

#endif      // PAL_WIN32_RUN
		if (hFile != PAL_INET_HANDLE_FAIL)
		{
			PAL_absclass::PAL_internetCloseHandle( hFile );
		}

		PAL_absclass::PAL_internetCloseHandle( hFTP );
	}

    while (dirList.size()> 0)
    {
        util_filesystem::Directory nextDestDirectory("");
        nextDestDirectory.appendPath(currentDestDir.getFullPath());
        nextDestDirectory.appendPath(dirList.front());

        util_filesystem::Directory nextSrcDirectory("");
        nextSrcDirectory.appendPath(currentSrcDir.getFullPath());
        nextSrcDirectory.appendPath(dirList.front());

        copyFtpDirectory(msg, hInet, nextSrcDirectory, nextDestDirectory);

        dirList.pop_front();
	}
}

STATUS VersionManager::handleRemoteDatabaseFiles(UpdateSoftwareAutomatically* msg)
{
	Debug::trace("VersionManager", "VersionManager::handleRemoteDatabaseFiles()");
	PAL_INET_HANDLE hInet;
    Configuration* pCFG = Configuration::getInstance();
    util_filesystem::Directory vmTempDir("");
    vmTempDir.appendPath(pCFG->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));

    util_filesystem::Directory tmpDir("");
    tmpDir.appendPath(pCFG->toStringCfgI(CfgInternal::TEMPORARYDBDIRECTORY));

	hInet = PAL_absclass::PAL_internetOpen(("VersionManager"), PAL_INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0); 

	if (hInet != PAL_INET_HANDLE_FAIL)
	{
		Debug::trace("VersionManager.database", "handleRemoteDatabaseFiles:  Opened ftp conntection.");

		util_filesystem::Directory dCurrent ("");
		if (msg->getPath() != "")
			dCurrent.appendPath(msg->getPath());
		dCurrent.appendPath("Database");

		copyFtpDirectory(msg, hInet, dCurrent, tmpDir);
	}

	if (hInet != PAL_INET_HANDLE_FAIL)
	{
		PAL_absclass::PAL_internetCloseHandle( hInet );
	}

    if (tmpDir.exists() && !tmpDir.isEmpty())
    {
		Debug::trace("VersionManager.database", "handleRemoteDatabaseFiles:  Sending ProcessDBFilesRequest message.");

        dbTmpDir_.selectAllFiles();
        dbDirIter_ = dbTmpDir_.begin();

        if (dbDirIter_ != dbTmpDir_.end())
        {
			EvolMessage proccessDbFilesReq(VersionManagerDev, ProcessDBFilesReq, this);

			dispatchEvent(&proccessDbFilesReq);
		}
	}
	return OK;
}


STATUS VersionManager::getRemoteDBFile(string& destPath)
{
	Debug::trace("VersionManager", "VersionManager::getRemoteDBFile()");

	destPath = "";

	if (dbDirIter_ != dbTmpDir_.end())
    {
        File tmpFile (dbTmpDir_.getFullPath(), dbDirIter_->getName());

        destPath = tmpFile.getFullPath();

        dbDirIter_++;
    }

    Debug::trace("VersionManager.database", HobString("getRemoteDBFile:  END"));

    return OK;
}

void VersionManager::logInstallationMsg(std::string msg)
{
	Debug::trace("VersionManager", "VersionManager::logInstallationMsg():  " + msg);
	time_t currTime;
	struct tm currTimeParts;
	time(&currTime);
	currTimeParts = *localtime(&currTime);
	char timeStr[30];
	strcpy(timeStr, asctime(&currTimeParts));
	char *ptr = timeStr + strlen(timeStr) - 1;
	if (*ptr == '\n')
		*ptr = '\0';

	std::ostream *outFile = new std::ofstream("C:\\Program Files\\Hobart\\appInstallerMsgs.dat", std::ios_base::app);
	*outFile << timeStr << "  " << msg.c_str() << std::endl;
	outFile->flush();
}
