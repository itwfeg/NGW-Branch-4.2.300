#include "KPCBootManager.h"
#include "CanManager.h"
#include "FpmManager.h"
#include "KpcBootEvent.h"
#include "KPCBootReady.h"
#include "KPCBootCmd.h"
#include "KPCBootVersion.h"
#include "KPCBootFrameHeader.h"
#include "KPCBootAck.h"
#include "KPCBootFlush.h"
#include "KPCBootBulkMsg.h"
#include "KPCBootAppVersion.h"
#include "KPCBootStatus.h"

#include "NGWGeneric.h"
#include "NGWUpgradeStatus.h"
#include "NGWUpgradeInfo.h"
#include "NGWUpgradeProgress.h"
#include "NGWUpgradeFinished.h"
#include "NGWUpgradeRequired.h"
#include "NGWUpgradeUIReady.h"
#include "NGWEvent.h"

#include "KPCUpgradeInfo.h"
#include "KPCUpgradeProgress.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"
#include "filesys/file.h"

#include "SessionManagerHelper.h"

#include <algorithm>
#include <fstream>
#include <stdio.h>
const std::string KPCBootManager::boardSite_("/etc/hobart/boardsite");
const std::string KPCBootManager::upgradePathKPC_("/lib/firmware/kpc/app/");
const std::string KPCBootManager::upgradePathFSSS_("/lib/firmware/fsss/app/");
const std::string KPCBootManager::upgradePathTof_("/lib/firmware/hobart/ngw/wrapper/tof/app/");
std::string KPCBootManager::bootloaderVersion_("");
std::string KPCBootManager::printerAppVersion_("");
std::string KPCBootManager::weigherAppVersion_("");
std::string KPCBootManager::tofAppVersion_("");
std::string PrinterAvailableVersion("");
std::string WeigherAvailableVersion("");
std::string TofAvailableVersion("");
KPCBootManager *KPCBootManager::instance_(nullptr);
std::mutex KPCBootManager::instanceMutex_;

KPCBootStatus::BootloaderFaults KPCBootManager::CurrentFault;
KPCBootStatus::BootStatusCode   KPCBootManager::CurrentStatusCode;

unsigned long long KPCBootManager::LastBufferPosition;
unsigned long long KPCBootManager::ImageBufferPosition;
std::vector<unsigned char> KPCBootManager::ImageBuffer;

unsigned int KPCBootManager::retryCounter = 0;
unsigned int KPCBootManager::retryFrameTimer_ = 0;

uint8_t KPCBootManager::FrameAckList = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootManager *KPCBootManager::getInstance()
{

	if (!instance_)
	{
		Debug::trace("KPCBootManager.info", "getInstance(): no instance found");
		std::lock_guard<std::mutex> guard(instanceMutex_);
		if (!instance_)
		{
			Debug::trace("KPCBootManager.info", "getInstance(): creating new instance");
			instance_ = new KPCBootManager();
		}
	}

	return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootManager::KPCBootManager() : Hobart::CEvolMsgReceiver("KPCBootManager"), upgradeWanted(false)
{
	Debug::trace("KPCBootManager", "KPCBootManager()");
	// register for upgrade messages
	registerEvent(KPCBootloader, KpcBootStatus);
	registerEvent(KPCBootloader, KpcBootAck);
	registerEvent(KPCBootloader, KpcBootReady);

	registerEvent(KPCBootloader, KpcBootReqUpgradeInfo);
	registerEvent(KPCBootloader, KpcBootUpgradeUIReady);
	registerEvent(KPCBootloader, KpcBootUpgradeCancel);
	registerEvent(KPCBootloader, KpcBootUpgradeRequested);
	registerEvent(KPCBootloader, KpcBootUpgradeUpdateUI);

	productID_ = 0;
	prevPercent = 0;
	
	shutUpSent = false;
	
	myRunThread_ = new Util::CTLSThread(*this, 1, 0, "KPCBootManager");
	Debug::trace("KPCBootManager", "KPCBootManager Thread Running");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootManager::~KPCBootManager()
{
	Debug::trace("KPCBootManager", "~KPCBootManager()");

	deregisterAllEvents();

	ShutdownNicely();

	// tell the thread to exit
	m_ExitEvent.EventSet();

	// wait for the thread to exit before deleting all the resources!
	if (myRunThread_)
	{
		while (myRunThread_->IsRunning())
			PAL_absclass::PAL_sleep(1);

		Debug::trace("KPCBootManager", "~KPCBootManager() Thread has exited.");
		PAL_absclass::PAL_sleep(100); //So we see the debug
		delete myRunThread_;
		myRunThread_ = nullptr;
	}
}

bool KPCBootManager::IsUpgrading(){
	// as long as the we are initialized and not done or shutting down the upgrade is done 
	return !(instance_ == nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD KPCBootManager::Run()
{
	Debug::trace("KPCBootManager.info", "Run() Started");
	/* 
	I'm not using chobart timer for my timeouts because it takes ~100 ms to execute createTimer and cancelTimer
	this makes the upgrade take 30 min instead of 3 min 
	*/
	do
	{
		// yield thread...
		PAL_absclass::PAL_sleep(1);
		std::lock_guard<std::mutex> guard(mutex_);
		if (mode != lastMode)
		{
			Debug::trace("KPCBootManager.info", "Mode Changed:" + Integer::toString(mode));
			Debug::trace("KPCBootManager.info", "Last Mode:" + Integer::toString(lastMode));
			lastMode = mode;
		}

		switch (mode)
		{

		case _Startup:
		{
			if(FpmManager::getInstance()->isWrapperTwoPrinter())
			{
				address_ = 0x10;
			}
			else if(productID_ == K_KPC_PID )
			{
				address_ = 0x0b;
			}
			else if( productID_ == K_FSSS_PID)
			{
				address_ = 0x0e;
			}
			ImageBufferPosition = 0;
			ImageBuffer.clear();
			retryCounter = 0;
			retryFrameTimer_ = 0;
			FrameAckList = 0;

			break;
		}
		case _RequestStatus:
			requestStatus();
			mode = _WaitForStatus;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		case _WaitForStatus:
		{
			// resend after timeout
			if (canMsgTimeout_ <= ++retryFrameTimer_)
			{
				retryCounter++;
				if (retryCounter >= RETRY_COUNT)
				{
					mode = _Fault;
				}
				requestStatus();
				retryFrameTimer_ = 0;
			}
			break;
		}
		case _RequestBootVersion:
		{
			requestBootVersion();
			mode = _WaitForBootVersion;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		}
		case _WaitForBootVersion:
		{
			if (canMsgTimeout_ <= ++retryFrameTimer_)
			{
				retryCounter++;
				if (retryCounter >= RETRY_COUNT)
				{
					mode = _Fault;
				}
				requestBootVersion();
				retryFrameTimer_ = 0;
			}
			break;
		}
		case _RequestAppVersion:
			requestAppVersion();
			mode = _WaitForAppVersion;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		case _WaitForAppVersion:
		{
			if (canMsgTimeout_ <= ++retryFrameTimer_)
			{
				retryCounter++;
				if (retryCounter >= RETRY_COUNT)
				{
					mode = _Fault;
				}
				requestAppVersion();
				retryFrameTimer_ = 0;
			}
			break;
		}
		case _CheckForUpgrade:
		{
			util_filesystem::File iniFile;
			util_filesystem::File imageFile;

			if( productID_ == tofID ) {
				 // check for tof
				iniFile.setPath( upgradePathTof_ );
				iniFile.setFileName( "version.ini" );
			    imageFile.setPath( upgradePathTof_ );
			    imageFile.setFileName( "sourceTOFApp.bin" );
			// SOF-4985
			//} else if( productID_ == K_KPC_PID ) {
			//			 
			// Released bootloader version reports incorrect id. ( 0x0020 or 0x0021 instead of 0x220 or 0x221 )	
			} else if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
				       ( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) ) {
				iniFile.setPath( upgradePathKPC_ ); 
				iniFile.setFileName( "version.ini" );
				imageFile.setPath( upgradePathKPC_ );
				imageFile.setFileName( "sourceKPCApp.bin" );
			} else if( productID_ == K_FSSS_PID ) {
				iniFile.setPath( upgradePathFSSS_ );
				iniFile.setFileName( "version.ini" );
				imageFile.setPath( upgradePathFSSS_ );
				imageFile.setFileName( "sourceFsssApp.bin" );
			} else {
				Debug::trace("KPCBootManager.info", "Unknown productID_: " + Hex::toString(productID_));	
			}
			std::ifstream verFile(iniFile.getFullPath().c_str(),
														std::ifstream::in | std::ifstream::binary);

			std::ifstream upgradeFile(imageFile.getFullPath().c_str(),
														std::ifstream::in | std::ios::binary);
			if (imageFile.exists())
			{
				int temp = 0;
				uint32_t startAddress = 0;
				//move pointer to end of file with offset of 16 bytes. This is where the application start address is in the firmware file
				upgradeFile.seekg(-24, upgradeFile.end);
				
				upgradeFile.read((char*)&startAddress, 4);
				
				//move pointer to end of file with offset of 16 bytes. This is where the versions are in the firmware file
				upgradeFile.seekg(-16, upgradeFile.end);

				if( productID_ == tofID ) // check for tof
				{
					Debug::trace("KPCBootManager", "Handling Tof upgrade");
					upgradeFile.read((char *)&temp, 1);
					TofAvailableVersion = (Integer::toString(temp));
					TofAvailableVersion.append(".");
					upgradeFile.read((char *)&temp, 1);
					TofAvailableVersion.append(Integer::toString(temp));
					TofAvailableVersion.append(".");
					upgradeFile.read((char *)&temp, 1);
					TofAvailableVersion.append(Integer::toString(temp));

				}
				else
				{
				    //read printer firmware version
				    upgradeFile.read((char *)&temp, 1);
				    PrinterAvailableVersion = (Integer::toString(temp));
				    PrinterAvailableVersion.append(".");
				    upgradeFile.read((char *)&temp, 1);
				    PrinterAvailableVersion.append(Integer::toString(temp));
				    PrinterAvailableVersion.append(".");
				    upgradeFile.read((char *)&temp, 1);
				    PrinterAvailableVersion.append(Integer::toString(temp));
				    //read weigher firmware
				    upgradeFile.read((char *)&temp, 1);
				    WeigherAvailableVersion = (Integer::toString(temp));
				    WeigherAvailableVersion.append(".");
				    upgradeFile.read((char *)&temp, 1);
				    WeigherAvailableVersion.append(Integer::toString(temp));
				    WeigherAvailableVersion.append(".");
				    upgradeFile.read((char *)&temp, 1);
				    WeigherAvailableVersion.append(Integer::toString(temp));
				}

				//reset ifstream pointer to the begining of file 
				upgradeFile.seekg(0, upgradeFile.beg);

				if( productID_ == tofID ) // check for tof
				{
					Debug::trace("KPCBootManager.info", "Run(): Tof Available Version: " + TofAvailableVersion);

					// current tof version will be stored in the printer app version
					Debug::trace("KPCBootManager.info", "Run(): Tof App Version: " + printerAppVersion_);
				}
				else 
				{
				    Debug::trace("KPCBootManager.info", "Run(): Start Address: " + Hex::toString(startAddress));
				    Debug::trace("KPCBootManager.info", "Run(): Printer Available Version: " + PrinterAvailableVersion);
				    Debug::trace("KPCBootManager.info", "Run(): Weigher Available Version: " + WeigherAvailableVersion);
				    Debug::trace("KPCBootManager.info", "Run(): Bootloader Version: " + bootloaderVersion_);
				    Debug::trace("KPCBootManager.info", "Run(): Printer App Version: " + printerAppVersion_);
				    Debug::trace("KPCBootManager.info", "Run(): Weigher App Version: " + weigherAppVersion_);
				}

				Debug::trace("KPCBootManager", "Compare Printer Available: " + Integer::toString(PrinterAvailableVersion.compare(printerAppVersion_)));

				Debug::trace("KPCBootManager", "Compare Weigher Available: " + Integer::toString(WeigherAvailableVersion.compare(weigherAppVersion_)));

                                // Very confusing here, but we receive the TOF version as we do with KPC, so the TOF version
				// is stored in the printer app version
				//SOF-5706 both strings need to match otherwise upgrade is needed. TOF 2-printer wrapper was never released so removed from check. (added back in) 						
				//if ( ( PrinterAvailableVersion.compare( printerAppVersion_ ) == 0 ) && ( WeigherAvailableVersion.compare( weigherAppVersion_ ) == 0 ) )
                                if ( !( PrinterAvailableVersion.compare(printerAppVersion_) || WeigherAvailableVersion.compare(weigherAppVersion_) ) ||	!TofAvailableVersion.compare(printerAppVersion_) )
                                {
		                	Debug::trace("KPCBootManager.info", "_CheckForUpgrade: Version strings match, jumping...");

					//version strings are the same
					mode = _Jumping;
				}
				else
				{				
					if(upgradeWanted)
					{
						//prepare to send upgrade
						ImageBufferPosition = 0;
						/*stop removing my data! */
						upgradeFile.unsetf(std::ios::skipws);

						const long64 imageFileSize(imageFile.size());

						//set up image buffer
						if (imageFileSize)
						{
							ImageBuffer.reserve(imageFileSize);
							ImageBuffer.assign(std::istream_iterator<unsigned char>(upgradeFile),
																std::istream_iterator<unsigned char>());
							Debug::trace("KPCBootManager.info", "Sending Start Address: " + Hex::toString(startAddress));
						    // SOF-4985
						    if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
				       			( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) || ( productID_ == tofID ) )
                                                    {
                                                                //Debug::trace("KPCBootManager.info", "IF Sending Destination: " + Hex::toString(startAddress));

								KPCBootCmd cmdMsg(this,address_,0);
								cmdMsg.setCommand(INIT_UPGRADE);
								cmdMsg.setAddress(startAddress);
								cmdMsg.show();
								dispatchEvent(&cmdMsg);
							} else {
                                                                //Debug::trace("KPCBootManager.info", "ELSE Sending Destination: " + Hex::toString(startAddress));
								KPCBootCmd cmdMsg(this,0x0e,0);
								cmdMsg.setCommand(INIT_UPGRADE);
								cmdMsg.setAddress(startAddress);
								cmdMsg.show();
								dispatchEvent(&cmdMsg);
							}
						    
							retryFrameTimer_ = 0;
							retryCounter = 0;

							mode = _WaitForStatus;
#ifndef USEUI
							returnState = _ProcessFrames;
#else
							returnState = _StartUI;
#endif
						}
						else
						{
							Debug::trace("KPCBootManager.Error", "Run(): Upgrade file was empty");
							//upgrade file was empty
							mode = _Jumping;
						}
					}
					else
					{
						// we need to start the ui to give a chance to hit cancel before firmware gets erased
						mode = _StartUI;
					}
									
				}
			}
			else
			{
				Debug::trace("KPCBootManager.Error", "Run(): No version file found");
				mode = _Jumping;
			}
			break;
		}
		case _Idle:
		{
			break;
		}
		case _ProcessFrames:
		{
			processUpgrade();
			break;
		}
		case _WaitForAck:
		{
			if (canMsgTimeout_ <= ++retryFrameTimer_)
			{
				retryCounter++;
				if (retryCounter >= RETRY_COUNT)
				{
					mode = _Fault;
				}
				ImageBufferPosition = LastBufferPosition; //reset buffer pointer
				processUpgrade();													//send frame
				retryFrameTimer_ = 0;
			}

			if (FrameAckList == 0xFF)
			{
				Debug::trace("KPCBootManager.info", "Run(): All Frames Acked Good Job!!!");
				mode = returnState;
			}
			break;
		}
		case _Flush:
		{
			KPCBootFlush msg(this);
			dispatchEvent(&msg);
			mode = _WaitForStatus;
			returnState = _ProcessFrames;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		}
		case _Verify:
		{
			//if( productID_ == K_KPC_PID ) {
			// SOF-4985
			if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
				( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) ) {
				KPCBootCmd cmdMsg(this,address_,0);
				cmdMsg.setCommand(VERIFY_APP);
				dispatchEvent(&cmdMsg);
			} else {
				KPCBootCmd cmdMsg(this,0x0e,0);
				cmdMsg.setCommand(VERIFY_APP);
				dispatchEvent(&cmdMsg);
			}

			mode = _WaitForStatus;
			returnState = _UpgradeComplete;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		}
		case _UpgradeComplete:
		{
			mode = _Jumping;
			break;
		}
		case _Jumping:
		{
			//if( productID_ == K_KPC_PID ) {
			// SOF-4985
			if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
				( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) ) {				
				KPCBootCmd cmdMsg(this,address_,0);
				cmdMsg.setCommand(JUMP);
				dispatchEvent(&cmdMsg);
			} else {
				KPCBootCmd cmdMsg(this,address_,0);
				cmdMsg.setCommand(JUMP);
				dispatchEvent(&cmdMsg);
			}
			mode = _Shutdown;
			break;
		}
		case _Shutdown:
		{
			m_ExitEvent.EventSet();
			mode = _Done;
			break;
		}
		case _Done:
		{
			//waiting for my death
			break;
		}
		case _Fault:
		{
			Debug::trace( "KPCBootManager.info", "Entering _Fault | lastMode == " + Integer::toString( lastMode ) );
			
			// This case only happens sometimes when waiting for wrapper upgrades on two printer systems when upgrading the TOF
			if( FpmManager::getInstance()->isWrapperTwoPrinter() && lastMode == _WaitForAppVersion )
			{	
				Debug::trace( "KPCBootManager.info", "Requesting app version again" );
				mode = _RequestAppVersion;
			}

			//TODO: Handle Faults
			switch(CurrentFault){
				case KPCBootStatus::BootloaderFaults::IMAGE_CHECKSUM_MISMATCH:
				mode = _RequestAppVersion;
				
				default:
				break;
			}
			break;
		}
#ifdef USEUI
		case _StartUI:
		{	
			if(upgradeWanted)
			{
				Debug::trace("KPCBootManager.info", "sending update to UI");
				
				// let UI know we are proceeding with the upgrade
				KPCBootGeneric updateMsg(KPCBootloader, KpcBootUpgradeUpdateUI, this);
				dispatchEvent(&updateMsg);
				retryFrameTimer_ = 0;
				mode = _WaitForUI;
				returnState = _SendUIStart;
			}
			else
			{
				Debug::trace("KPCBootManager.info", "startUI sending upgraderequired message");
				KPCBootGeneric startUI(KPCBootloader, KpcBootUpgradeRequired, this);
				dispatchEvent(&startUI);	
				retryFrameTimer_ = 0;			
				mode = _WaitForUI;
			}
			

			break;
		}
		case _WaitForUI:
		{
			if (uiResponseTimeout_ <= ++retryFrameTimer_)
			{
				retryFrameTimer_ = 0;
				KPCBootGeneric startUI(KPCBootloader, KpcBootUpgradeRequired, this);
				dispatchEvent(&startUI);
				Debug::trace("KPCBootManager.info","Re-requesting ui");
			}
			
			break;
		}
		case _SendUIStart:
		{
			KPCBootGeneric startUI(KPCBootloader, KpcBootStartUpgrade, this);
			dispatchEvent(&startUI);
			mode = _ProcessFrames;
			break;
		}
		case _SendUIProgress:
		{
			const size_t imageSize(ImageBuffer.size());
			int percent = (int) (((float)ImageBufferPosition / (float)imageSize) *100.0);
            if( prevPercent != percent ) {
				prevPercent = percent;
				KPCUpgradeProgress prog(KPCBootUpdater, this);
				prog.setProgress(percent);
				dispatchEvent(&prog);
				Debug::trace("KPCBootManager.prog", "Run(): Upgrade percent: " + Integer::toString(percent));
			}
			mode = _Flush;
			break;
		}
		case _WaitForSession:
		{
			Debug::trace("KPCBootManager.info", " hasPrimarySession(): " + Integer::toString( SessionManagerHelper::getInstance()->hasPrimarySession() ) );
			if(SessionManagerHelper::getInstance()->hasPrimarySession()){
				mode = _CheckForUpgrade;
			} else {
				util_filesystem::File  boardSiteFile( boardSite_ );
				//if we are running board site the have the KPC jump.
				if ( boardSiteFile.exists() )  {
					mode = _Jumping;
				}					
			}
			break;
		}
		case _CancelUpgrade: 
		{
			Debug::trace("KPCBootManager.info", "case CancelUpgrade");

			// stop the upgrade
			mode = _Jumping;

			Debug::trace("KPCBootManager.info", "case CancelUpgrade STOPPED");

			break;
		}
		case _UpgradeRequest:
		{
			Debug::trace("KPCBootManager.info", "case UpgradeRequest");
			upgradeWanted = true;
			mode = _CheckForUpgrade; 

			break;
		}

#endif
		default:
			Debug::trace("KPCBootManager.Error", "Run(): Invalid State" + Integer::toString(mode));
			mode = _Fault;
			break;
		}
	} while (!m_ExitEvent.IsEventSet());

	Debug::trace("KPCBootManager", "Finished.");

	KPCBootGeneric generic(KPCBootloader, KpcBootDone, this);
	dispatchEvent(&generic);

	Debug::trace("KPCBootManager", "Exiting thread");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootManager::shutdown()
{
	Debug::trace("KPCBootManager", "shutdown()");

	if (instance_)
	{
		std::lock_guard<std::mutex> guard(instanceMutex_);

		if (instance_)
		{
			delete instance_;
			instance_ = nullptr;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootManager::processEvent(Util::CSmartPtr<EvolMessageWrapper> &msg)
{
	std::lock_guard<std::mutex> guard(mutex_);
	if (!msg.get())
	{
		Debug::trace("KPCBootManager.Error", "processEvent(): message received is NULL! Ignoring");
		return;
	}
	switch (msg->getDeviceType())
	{
	case KPCBootloader:
		if(!isUiEvent((KPCBootEvent) msg->getEventType())){
			handleKpcBootEvent((KPCBootEvent)msg->getEventType(), msg->Data());
		}
#ifdef USEUI
		else{
			handleUIEvent((KPCBootEvent)msg->getEventType(), msg->Data());
		}
#endif
		break;
	default:
		Debug::trace("KPCBootManager.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(msg->getDeviceType()));
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootManager::handleKpcBootEvent(KPCBootEvent event, const char *recvBuffer)
{
	Debug::trace("KPCBootManager.info", "processEvent(): EventType:" + Integer::toString(event));
	switch (event)
	{
	case KpcBootReady:
		handleKpcBootReady();
		break;

	case KpcBootStatus:
	{
		Debug::trace("KPCBootManager.FrameTime", "processEvent(): Boot Status: ");
		Debug::trace("KPCBootManager.FrameTime", "processEvent(): Boot Status: cancel timer done ");
		const KPCBootStatus status(recvBuffer);
		Debug::trace("KPCBootManager.FrameTime", "processEvent(): Boot Status: created message");
		handleKpcBootStatus(status);
		Debug::trace("KPCBootManager.FrameTime", "processEvent(): Boot Status: call done");
		break;
	}
	case KpcBootAppVersion:
	{
		if (mode == _WaitForAppVersion)
		{
			KPCBootAppVersion version(recvBuffer);
			version.show();
			printerAppVersion_ = version.getPrinterAppVersionString();
			weigherAppVersion_ = version.getWeigherAppVersionString();
			mode = _WaitForSession;
		}
		break;
	}
	case KpcBootVersion:
	{
		if (mode == _WaitForBootVersion)
		{
			KPCBootVersion version(recvBuffer);
			bootloaderVersion_ = version.getBootVersionString();
			mode = _RequestAppVersion;
		}
		break;
	}
	case KpcBootAck:
	{
		Debug::trace("KPCBootManager.FrameTime", "processEvent(): Ack: ");
		KPCBootAck ack(recvBuffer);
		Debug::trace("KPCBootManager.info", "processEvent(): Ack: " + Integer::toString(ack.getAckedEvent()));
		FrameAckList = 0xFF;
		break;
	}

	default:
		Debug::trace("KPCBootManager.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(event));
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USEUI
void KPCBootManager::handleUIEvent(KPCBootEvent event, const char *recvBuffer)
{
	Debug::trace("KPCBootManager", "handleUIEvent");

	Debug::trace("KPCBootManager", "handleUIEvent() Event: " + Integer::toString(event));

	switch (event)
	{
		case KpcBootReqUpgradeInfo:
		{
			Debug::trace("KPCBootManager", "case KpcBootReqUpgradeInfo setVersion setApp");
			KPCUpgradeInfo info(KPCBootloader,this);
			if( productID_ == tofID )
			{
				info.setAvailableVersion(TofAvailableVersion);
				info.setCurrentVersion(printerAppVersion_);
			}
			else
			{
			    info.setAvailableVersion(PrinterAvailableVersion + "/" + WeigherAvailableVersion);
			    info.setCurrentVersion(printerAppVersion_ + "/" + weigherAppVersion_);
			}
			dispatchEvent(&info);
			
			break;
		}
		case KpcBootUpgradeUIReady:
		{	
			Debug::trace("KPCBootManager", "case KpcBootUpgradeUIReady");
			if(mode == _WaitForUI)
				mode = returnState;
			break;
		}
		case KpcBootUpgradeCancel:
		{
			Debug::trace("KPCBootManager", "handleUIEvent(): case KpcBootUpgradeCancel");
			mode = _CancelUpgrade;
			break;
		}
		case KpcBootUpgradeRequested:
		{
			Debug::trace("KPCBootManager", "handleUIEvent(): case KpcBootUpgradeRequested");
			mode = _UpgradeRequest;
			break;
		}
		default:
			Debug::trace("KPCBootManager.Error", "handleUIEvent(): Unhandled Event KPCBootEvent: " + Integer::toString(event));	
		break;
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootManager::handleKpcBootReady()
{
	Debug::trace("KPCBootManager", HobString("mode = ") + HobString(Integer::toString(mode)));
	if (mode == _Startup)
	{
		Debug::trace("KPCBootManager", HobString("handleKpcBootReady() if _Startup: productID_ = ") + HobString(Integer::toString(productID_)));
		Debug::trace("KPCBootManager", HobString("handleKpcBootReady() if _Startup:  address_ = ") + HobString(Integer::toString(address_)));
		// SOF-4985
		//if( productID_ == K_KPC_PID ) {
		if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
			( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) ) {
			KPCBootCmd cmdMsg(this,address_,0);
			cmdMsg.setCommand(I_HEARD_YOU_NOW_SHUT_UP);
			Debug::trace("KPCBootManager", HobString("handleKpcBootReady() if _Startup if #1: I_HEARD_YOU_NOW_SHUT_UP = " ));
			dispatchEvent(&cmdMsg);
		} else {
			if( productID_ != 0 ) {
				KPCBootCmd cmdMsg(this,0x0e,0);
				cmdMsg.setCommand(I_HEARD_YOU_NOW_SHUT_UP);
				Debug::trace("KPCBootManager", HobString("handleKpcBootReady() if _Startup else if #2: I_HEARD_YOU_NOW_SHUT_UP = "));
				dispatchEvent(&cmdMsg);
				shutUpSent = true;
			} else {
				Debug::trace("KPCBootManager", HobString(" Switch mode to request status to get our product id"));
				Debug::trace("KPCBootManager", HobString("handleKpcBootReady() if _Startup else #3: I_HEARD_YOU_NOW_SHUT_UP = "));
				//need to get our product id.
				mode = _RequestStatus;		
			}
		}
		mode = _RequestStatus;
		returnState = _RequestBootVersion;
	}
	else
	{
		Debug::trace("KPCBootManager", HobString("handleKpcBootReady() productID_ = ") + HobString(Integer::toString(productID_)));		
		if( productID_ == 0 ) {
			mode = _RequestStatus;
			returnState = _RequestBootVersion;
		}
	}
}

void KPCBootManager::handleKpcBootStatus(const KPCBootStatus msg)
{
	msg.show();

	productID_ = msg.getId();
	Debug::trace("KPCBootManager", HobString("handleKpcBootStatus(): productID_ = ") + HobString(Integer::toString(productID_)));
	//SOF-5058: This is required if the product id is 0 when we receive the boot ready.
	if( !shutUpSent ) {
		if(FpmManager::getInstance()->isWrapperTwoPrinter())
			address_ = 0x10;
		else if( ( productID_ == K_KPC_PID ) || ( productID_ == K_KPCV_PID ) || 
			( productID_ == K_KPC_ARRATA ) || ( productID_ == K_KPCV_ARRATA ) ) 
			address_ = 0x0b;
		else if( productID_ == K_FSSS_PID)
			address_ = 0x0e;

Debug::trace("KPCBootManager", HobString("handleKpcBootStatus(): address_ = ") + HobString(Integer::toString(address_)));
		KPCBootCmd cmdMsg( this, address_, 0 );
		cmdMsg.setCommand( I_HEARD_YOU_NOW_SHUT_UP );
		dispatchEvent( &cmdMsg );
		shutUpSent = true;
	}
	//big mess, but basically we want to catch all faults unless it is a checksum mismatch before the upgrade
	if (((msg.getBootloaderFaults() != KPCBootStatus::BootloaderFaults::NOFAULT) ||
			(msg.getStatusCode() == KPCBootStatus::BootStatusCode::BOOT_FAULT)) &&
			( (returnState == _StartUI || returnState == _ProcessFrames ) && 
			msg.getBootloaderFaults() == KPCBootStatus::BootloaderFaults::CHECKSUM))
	{

		mode = _Fault; //handle bootloader fault
		CurrentFault = msg.getBootloaderFaults();
		CurrentStatusCode = msg.getStatusCode();
	}
	else
	{
		if (mode == _WaitForStatus)
		{
			mode = returnState;
		}
	}
	Debug::trace("KPCBootManager.FrameTime", "processEvent(): Done With Status: ");
}

void KPCBootManager::requestStatus()
{
	KPCBootGeneric msg(KPCBootloader, KpcBootReqStatus, this);
	dispatchEvent(&msg);
}

void KPCBootManager::requestAppVersion()
{
	KPCBootGeneric msg(KPCBootloader, KpcBootReqAppVersion, this);
	dispatchEvent(&msg);
}

void KPCBootManager::requestBootVersion()
{
	KPCBootGeneric msg(KPCBootloader, KpcBootReqVersion, this);
	dispatchEvent(&msg);
}

void KPCBootManager::processUpgrade()
{
	FrameAckList = 0;
	Debug::trace("KPCBootManager", "processUpgrade()");
	const size_t imageSize(ImageBuffer.size());
	Debug::trace("KPCBootManager", "processUpgrade(): Image Size " + Integer::toString(imageSize));

	if (imageSize <= ImageBufferPosition)
	{
		Debug::trace("KPCBootManager", "Download complete");
		mode = _Verify;
		return;
	}

	Debug::trace("KPCBootManager", "processUpgrade(): Image Size - Buffer pos: " + Integer::toString(imageSize - ImageBufferPosition));
	// determine the number of frames needed to send record
	if ((imageSize - ImageBufferPosition) >= (BYTES_PER_FRAME * MAX_FRAMES))
	{
		Debug::trace("KPCBootManager", "Creating Bulk msg");

		UINT8 id = 0;
		if(  productID_ == K_FSSS_PID  )
                { 
			id = 0x0E;
		}
                else if ( productID_ == tofID ) 
                {
			id = 0x10;
		}
                else
                {
                    id = 0x0B;
                }
		KPCBootBulkMsg bulkMsg( this, &ImageBuffer.at(ImageBufferPosition),
													 __BlockWriteProgramFlash, (int)(MAX_FRAMES), BYTES_PER_FRAME, id );
		bulkMsg.show();
	 	dispatchEvent(&bulkMsg);
		LastBufferPosition = ImageBufferPosition;
		ImageBufferPosition += BYTES_PER_FRAME * MAX_FRAMES;
	}
	else
	{
		Debug::trace("KPCBootManager", "Creating partial Bulk msg");
		UINT8 id = 0;
		if(  productID_ == K_FSSS_PID  )
                { 
			id = 0x0E;
		}
                else if ( productID_ == tofID ) 
                {
			id = 0x10;
		}
                else
                {
                    id = 0x0B;
                }
		KPCBootBulkMsg bulkMsg(this, &ImageBuffer.at(ImageBufferPosition),
													 __BlockWriteProgramFlash, (imageSize - ImageBufferPosition) / MAX_FRAMES, imageSize % BYTES_PER_FRAME, id );
		bulkMsg.show();
		dispatchEvent(&bulkMsg);
		LastBufferPosition = ImageBufferPosition;
		ImageBufferPosition += ((((imageSize - ImageBufferPosition) / MAX_FRAMES) - 1) * BYTES_PER_FRAME) + (imageSize % BYTES_PER_FRAME);
	}

	mode = _WaitForAck;
#ifndef USEUI
	returnState = _Flush;
#else 
	returnState = _SendUIProgress;
#endif
	retryFrameTimer_ = 0;
	retryCounter = 0;
}

bool KPCBootManager::isUiEvent(KPCBootEvent event){
	// first and last ui event
	return (event >= (KPCBootEvent)KpcBootStartUpgrade && event <= (KPCBootEvent)KpcBootUpgradeRequested);
}
