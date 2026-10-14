#include "KPCBootUpgrader.h"
#include "CanManager.h"
#if defined ( __linux__ )
// Larry added kpcBootGeneric not needed
#else
#include "KpcBootGeneric.h"
#endif
#include "KpcBootUpgraderEvent.h"
#include "KPCBootUpgraderCmd.h"
#include "KPCBootVersion.h"
#include "KPCBootUpgraderFrameHeader.h"
#include "KPCBootUpgraderAck.h"
#include "KPCBootUpgraderFlush.h"
#include "KPCBootUpgraderBulkMsg.h"
#include "KPCBootAppVersion.h"
#include "KPCBootUpgraderStatus.h"
#include "KpcBootEvent.h"

#include "KPCUpgradeInfo.h"
#include "KPCUpgradeProgress.h"

#include "RealTimeController.h"
#include "FpmManager.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"
#include "filesys/file.h"

#include <algorithm>
#include <fstream>
#include <stdio.h>

const std::string KPCBootUpgrader::upgradePathKPC_("/lib/firmware/kpc/boot/");
const std::string KPCBootUpgrader::upgradePathTof_("/lib/firmware/hobart/ngw/wrapper/tof/boot/");
const std::string KPCBootUpgrader::upgradePathFSSS_("/lib/firmware/fsss/boot/");
std::string KPCBootUpgrader::bootloaderVersion_("");
std::string BootloaderAvailableVersion("");

KPCBootUpgrader *KPCBootUpgrader::instance_(nullptr);
std::mutex KPCBootUpgrader::instanceMutex_;

KPCBootUpgraderStatus::BootUpgraderFaults KPCBootUpgrader::CurrentFault;
KPCBootUpgraderStatus::BootUpgraderStatusCode   KPCBootUpgrader::CurrentStatusCode;

unsigned long long KPCBootUpgrader::LastBufferPosition;
unsigned long long KPCBootUpgrader::ImageBufferPosition;
std::vector<unsigned char> KPCBootUpgrader::ImageBuffer;

unsigned int KPCBootUpgrader::retryCounter = 0;
unsigned int KPCBootUpgrader::retryFrameTimer_ = 0;

uint8_t KPCBootUpgrader::FrameAckList = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootUpgrader *KPCBootUpgrader::getInstance()
{

	if (!instance_)
	{
		Debug::trace("KPCBootUpgrader.info", "getInstance(): no instance found");
		std::lock_guard<std::mutex> guard(instanceMutex_);
		if (!instance_)
		{
			Debug::trace("KPCBootUpgrader.info", "getInstance(): creating new instance");
			instance_ = new KPCBootUpgrader();
		}
	}

	return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootUpgrader::KPCBootUpgrader() : Hobart::CEvolMsgReceiver("KPCBootUpgrader")
{
	Debug::trace("KPCBootUpgrader", "KPCBootUpgrader()");
	// register for upgrade messages
	registerEvent(KPCBootUpdater, KpcBootUpgraderStatus);
	registerEvent(KPCBootUpdater, KpcBootUpgraderAck);
	registerEvent(KPCBootUpdater, KpcBootUpgraderReady);

	registerEvent(KPCBootUpdater, KpcBootUpgraderReqUpgradeInfo);
	registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeUIReady);

	productID_ = 0;

	myRunThread_ = new Util::CTLSThread(*this, 1, 0, "KPCBootUpgrader");
	Debug::trace("KPCBootUpgrader", "KPCBootUpgrader Thread Running");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
KPCBootUpgrader::~KPCBootUpgrader()
{
	Debug::trace("KPCBootUpgrader", "~KPCBootUpgrader()");

	deregisterAllEvents();

	ShutdownNicely();

	// tell the thread to exit
	m_ExitEvent.EventSet();

	// wait for the thread to exit before deleting all the resources!
	if (myRunThread_)
	{
		while (myRunThread_->IsRunning())
			PAL_absclass::PAL_sleep(1);

		Debug::trace("KPCBootUpgrader", "~KPCBootUpgrader() Thread has exited.");
		PAL_absclass::PAL_sleep(100); //So we see the debug
		delete myRunThread_;
		myRunThread_ = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD KPCBootUpgrader::Run()
{
	Debug::trace("KPCBootUpgrader.info", "Run() Started");
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
			Debug::trace("KPCBootUpgrader.info", "Mode Changed:" + Integer::toString(mode));
			Debug::trace("KPCBootUpgrader.info", "Last Mode:" + Integer::toString(lastMode));
			lastMode = mode;
		}

		switch (mode)
		{

		case _Startup:
		{
			ImageBufferPosition = 0;
			ImageBuffer.clear();
			retryCounter = 0;
			retryFrameTimer_ = 0;
			FrameAckList = 0;
			//SOF-5023
			lastPercent_ = 0;
			//SOF-5294
			if( FpmManager::getInstance()->isFreeStandingSelfService() ||
				FpmManager::getInstance()->isWrapperTwoPrinter()	
			) {
				mode = _RequestStatus;
			} else {
				mode = _RequestBootVersion;
			}					
			returnState = _RequestBootVersion;
			break;
		}
		case _RequestStatus:
			Debug::trace("KPCBootUpgrader.info", "_RequestStatus requestStatus()");
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
					//mode = _Fault;
					mode = _Shutdown;
					Debug::trace("KPCBootUpgrader.info", "_WaitForStatus: Switching to Mode :" + Integer::toString(mode));
				}
				Debug::trace("KPCBootUpgrader.info", "_WaitForStatus requestStatus()");
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
		case _CheckForUpgrade:
		{
			std::string upgradePath_;
			std::string sourceFile_;
			
			util_filesystem::File imageFile(upgradePathKPC_, "sourceKPCBoot.bin");
			int offset = -10;
			
			Debug::trace("KPCBootUpgrader.info", "productID_" + Hex::toString(productID_));
			if( productID_ == 144 ) 
			{
				Debug::trace("KPCBootUpgrader.info", "Handling TOF");
				imageFile = upgradePathTof_ + "sourceTOFBoot.bin";
				//offset = -14;
				address_ = 0x0A;
			}
			else if( productID_ == K_FSSS_PID ) { // TO DO: get model number from cherie 
				Debug::trace("KPCBootUpgrader.info", "Handling FSSS");
				imageFile = upgradePathFSSS_ + "sourceFsssBoot.bin";
				address_ = 0x0B;							//SOF-5332
			} else if( ( productID_ == K_KPC_PID ) || ( productID_ ==  K_KPCV_PID) ) {
				Debug::trace("KPCBootUpgrader.info", "Handling KPC");
				imageFile = upgradePathKPC_ + "sourceKPCBoot.bin";
				address_ = 0x0B;
			} else if( productID_ == 0 ) {
				Debug::trace("KPCBootUpgrader.info", "productID_: 0 _RequestStatus" );
				mode = _RequestStatus;
				address_ = 1;
			} else {
				address_ = 1;
			}

			std::ifstream upgradeFile(imageFile.getFullPath().c_str(),
														std::ifstream::in | std::ios::binary);

			if (imageFile.exists())
			{
				int temp = 0;
				//move pointer to end of file with offset of 10 bytes. This is where the versions are in the firmware file
				upgradeFile.seekg(offset, upgradeFile.end);


				//read printer firmware version
				upgradeFile.read((char *)&temp, 1);
				BootloaderAvailableVersion = (Integer::toString(temp));
				BootloaderAvailableVersion.append(".");
				upgradeFile.read((char *)&temp, 1);
				BootloaderAvailableVersion.append(Integer::toString(temp));
				BootloaderAvailableVersion.append(".");
				upgradeFile.read((char *)&temp, 1);
				BootloaderAvailableVersion.append(Integer::toString(temp));

				//reset ifstream pointer to the begining of file 
				upgradeFile.seekg(0, upgradeFile.beg);

				Debug::trace("KPCBootUpgrader.info", "Run(): Bootloader Available Version: " + BootloaderAvailableVersion);
				Debug::trace("KPCBootUpgrader.info", "Run(): Bootloader Version: " + bootloaderVersion_);
				if (!(BootloaderAvailableVersion.compare(bootloaderVersion_)))
				{
					//version strings are the same
					mode = _Shutdown;
				}
				else
				{
					//prepare to send upgrade
					ImageBufferPosition = 0;

					std::ifstream upgradeFile(imageFile.getFullPath().c_str(),
																		std::ifstream::in | std::ios::binary);

					/*stop removing my data! */
					upgradeFile.unsetf(std::ios::skipws);

					const long64 imageFileSize(imageFile.size());

					//set up image buffer
					if (imageFileSize)
					{
						ImageBuffer.reserve(imageFileSize);
						ImageBuffer.assign(std::istream_iterator<unsigned char>(upgradeFile),
															 std::istream_iterator<unsigned char>());
						//KPCBootUpgraderCmd cmdMsg(this,1,0);//TODO: DYNAMICALLY SET DESTINATION AND SOURCE
						KPCBootUpgraderCmd cmdMsg(this,address_,0);
						cmdMsg.setCommand(INIT_BOOT_UPGRADE);
						dispatchEvent(&cmdMsg);
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
						Debug::trace("KPCBootUpgrader.Error", "Run(): Upgrade file was empty");
						//upgrade file was empty
						mode = _Shutdown;
					}
				}
			}
			else
			{
				Debug::trace("KPCBootUpgrader.Error", "Run(): No version file found");
				mode = _Shutdown;
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
				Debug::trace("KPCBootUpgrader.info", "Run(): All Frames Acked Good Job!!!");
				mode = returnState;
			}
			break;
		}
		case _Flush:
		{
			KPCBootUpgraderFlush msg(this);
			dispatchEvent(&msg);
			mode = _WaitForStatus;
			returnState = _ProcessFrames;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		}
		case _Verify:
		{
			//KPCBootUpgraderCmd cmdMsg(this,1,0);//TODO: DYNAMICALLY SET SOURCE AND DESTINATION
			KPCBootUpgraderCmd cmdMsg(this,address_,0);
			cmdMsg.setCommand(VERIFY_BOOT);
			dispatchEvent(&cmdMsg);
			mode = _WaitForStatus;
			returnState = _UpgradeComplete;
			retryFrameTimer_ = 0;
			retryCounter = 0;
			break;
		}
		case _UpgradeComplete:
		{
			mode = _Shutdown;
			GpioDeviceReset kpcReset;
			kpcReset.setDeciceTypeKpc();
			Debug::trace("KPCBootUpgrader.info", "Run(): Resetting KPC....");
			kpcReset.releaseResetDevice();
#if defined ( __linux__ )
			PAL_API::usleep(500);
#endif
			kpcReset.resetDevice();
			break;
		}
		case _Shutdown:
		{
			Debug::trace("KPCBootUpgrader.info", "Run(): _Shutdown");
			//KPCBootUpgraderCmd cmdMsg(this,1,0);//TODO: DYNAMICALLY SET SOURCE AND DESTINATION
			KPCBootUpgraderCmd cmdMsg(this,address_,0);
			cmdMsg.setCommand(KILL_UPGRADER);
			dispatchEvent(&cmdMsg);
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
			//TODO: Handle Faults
			Debug::trace("KPCBootUpgrader.Error", "Run(): Fault: " + Integer::toString(CurrentFault));
			switch(CurrentFault){
				case KPCBootUpgraderStatus::BootUpgraderFaults::IMAGE_CHECKSUM_MISMATCH:
					mode = _RequestBootVersion;
					break;
				default:
					mode = _Done;
				break;
		}
		break;
		}
#ifdef USEUI
		case _StartUI:
		{	
			KPCBootUpgraderGeneric startUI(KPCBootUpdater, KpcBootUpgraderUpgradeRequired, this);
			dispatchEvent(&startUI);
			mode = _WaitForUI;
			returnState = _SendUIStart;
			break;
		}
		case _WaitForUI:
		{
			//TODO: TIMEOUT
			
			break;
		}
		case _SendUIStart:
		{
			KPCBootUpgraderGeneric startUI(KPCBootUpdater, KpcBootUpgraderStartUpgrade, this);
			dispatchEvent(&startUI);
			mode = _ProcessFrames;
			break;
		}
		case _SendUIProgress:
		{
			const size_t imageSize(ImageBuffer.size());
			int percent = (int) (((float)ImageBufferPosition / (float)imageSize) *100.0);
			//SOF-5023: do not flood the UI with progress messages.
			if( percent >= ( lastPercent_ + 5 ) ) {
				lastPercent_ = percent;
				KPCUpgradeProgress prog(KPCBootUpdater, this);
				prog.setProgress(percent);
				prog.show();
				dispatchEvent(&prog);
				Debug::trace("KPCBootUpgrader.info", "Run(): Upgrade percent: " + Integer::toString(percent));
				mode = _Flush;
			} else {
				mode = _Flush;	
			}
			break;
		}

#endif
		default:
			Debug::trace("KPCBootUpgrader.Error", "Run(): Invalid State" + Integer::toString(mode));
			mode = _Fault;
			break;
		}
	} while (!m_ExitEvent.IsEventSet());

	Debug::trace("KPCBootUpgrader", "Finished.");

	KPCBootUpgraderGeneric generic(KPCBootUpdater, KpcBootUpgraderDone, this);
	dispatchEvent(&generic);

	Debug::trace("KPCBootUpgrader", "Exiting thread");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootUpgrader::shutdown()
{
	Debug::trace("KPCBootUpgrader", "shutdown()");
	
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
void KPCBootUpgrader::processEvent(Util::CSmartPtr<EvolMessageWrapper> &msg)
{
	std::lock_guard<std::mutex> guard(mutex_);
	if (!msg.get())
	{
		Debug::trace("KPCBootUpgrader.Error", "processEvent(): message received is NULL! Ignoring");
		return;
	}
	switch (msg->getDeviceType())
	{
	case KPCBootUpdater:
		if(!isUiEvent((KPCBootUpgraderEvent)msg->getEventType())){
			handleKpcBootUpgraderEvent((KPCBootUpgraderEvent)msg->getEventType(), msg->Data());
		}
#ifdef USEUI
		else
		{
			handleUIEvent((KPCBootUpgraderEvent)msg->getEventType(), msg->Data());
		}
#endif
		break;
	default:
		Debug::trace("KPCBootUpgrader.Error", "processEvent(): Unhandled Device Type DeviceType: " + Integer::toString(msg->getDeviceType()));
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootUpgrader::handleKpcBootUpgraderEvent(KPCBootUpgraderEvent event, const char *recvBuffer)
{
	Debug::trace("KPCBootUpgrader.info", "processEvent(): EventType: " + Integer::toString(event));
	switch (event)
	{
	case KpcBootUpgraderStatus:
	{
		Debug::trace("KPCBootUpgrader.FrameTime", "processEvent(): Boot Status: ");
		Debug::trace("KPCBootUpgrader.FrameTime", "processEvent(): Boot Status: cancel timer done ");
		const KPCBootUpgraderStatus status(recvBuffer);
		Debug::trace("KPCBootUpgrader.FrameTime", "processEvent(): Boot Status: created message");
		handleKpcBootStatus(status);
		Debug::trace("KPCBootUpgrader.FrameTime", "processEvent(): Boot Status: call done");
		break;
	}
	case KpcBootUpgraderVersion:
	{
		Debug::trace("KPCBootUpgrader.info", "processEvent(): KpcBootUpgraderVersion Mode: " + Integer::toString(mode));
		if (mode == _WaitForBootVersion)
		{
			KPCBootVersion version(recvBuffer);
			version.show();
			bootloaderVersion_ = version.getBootVersionString();
			mode = _CheckForUpgrade;
		}
		break;
	}
	case KpcBootUpgraderAck:
	{
		Debug::trace("KPCBootUpgrader.FrameTime", "processEvent(): Ack: ");
		KPCBootUpgraderAck ack(recvBuffer);
		Debug::trace("KPCBootUpgrader.info", "processEvent(): Ack: " + Integer::toString(ack.getAckedEvent()));
		FrameAckList = 0xFF;
	}

	default:
			Debug::trace("KPCBootUpgrader.Error", "processEvent(): Unhandled Event KPCBootUpgraderEvent: " + Integer::toString(event));
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USEUI
void KPCBootUpgrader::handleUIEvent(KPCBootUpgraderEvent event, const char *recvBuffer)
{
	Debug::trace("KPCBootUpgrader.info", "handleUIEvent:" + Integer::toString(event) );

	switch (event)
	{
		case KpcBootUpgraderReqUpgradeInfo:
		{
			//KPCBootUpgraderGeneric msg(KPCBootUpdater, KpcBootUpgraderReqVersion, this);
			//dispatchEvent(&msg);

			KPCUpgradeInfo info(KPCBootUpdater,this);
			Debug::trace("KPCBootUpgrader.info", "handleUIEvent(): Bootloader Available Version: " + BootloaderAvailableVersion);
			Debug::trace("KPCBootUpgrader.info", "handleUIEvent(): Bootloader Version: " + bootloaderVersion_);
			info.setAvailableVersion(BootloaderAvailableVersion);
			info.setCurrentVersion(bootloaderVersion_);
			info.show();
			dispatchEvent(&info);
			break;
		}
		case KpcBootUpgraderUpgradeUIReady:
		{	
			if(mode == _WaitForUI)
				mode = returnState;
		break;
		}
	default:
			Debug::trace("KPCBootUpgrader.Error", "processEvent(): Unhandled Event KPCBootUpgraderEvent: " + Integer::toString(event));	
		break;
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
void KPCBootUpgrader::handleKpcBootStatus(const KPCBootUpgraderStatus msg)
{
	productID_ = msg.getId();

	//SOF-5332
	if( productID_ == 0x20 ) {
		productID_ = K_KPC_PID;
	}
	Debug::trace("KPCBootUpgrader.info", HobString("productID = ") + HobString(Integer::toString(productID_)));

	if ((msg.getBootUpgraderFaults() != KPCBootUpgraderStatus::BootUpgraderFaults::NOFAULT) ||
			(msg.getStatusCode() == KPCBootUpgraderStatus::BootUpgraderStatusCode::BOOT_FAULT))
	{
		mode = _Fault; //handle bootloader fault
		CurrentFault = msg.getBootUpgraderFaults();
		CurrentStatusCode = msg.getStatusCode();
	}
	else
	{
		if (mode == _WaitForStatus)
		{
			mode = returnState;
			
			//mode = _CheckForUpgrade;
		}
	}
	Debug::trace(" No version file found.FrameTime", "processEvent(): Done With Status: ");
}

void KPCBootUpgrader::requestStatus()
{
	KPCBootUpgraderGeneric msg(KPCBootUpdater, KpcBootUpgraderReqStatus, this);
	dispatchEvent(&msg);
}

void KPCBootUpgrader::requestAppVersion()
{
	KPCBootUpgraderGeneric msg(KPCBootUpdater, KpcBootUpgraderReqAppVersion, this);
	dispatchEvent(&msg);
}

void KPCBootUpgrader::requestBootVersion()
{
	KPCBootUpgraderGeneric msg(KPCBootUpdater, KpcBootUpgraderReqVersion, this);
	dispatchEvent(&msg);
}

void KPCBootUpgrader::processUpgrade()
{
	FrameAckList = 0;
	Debug::trace("KPCBootUpgrader", "processUpgrade()");

	//SOF-5332
	if( address_ == 0 ) {
		if( ( productID_ == K_KPC_PID ) || ( productID_ ==  K_KPCV_PID) ) {
				address_ = 0x0B;
		}
	}

	const size_t imageSize(ImageBuffer.size());
	Debug::trace("KPCBootUpgrader", "processUpgrade(): Image Size " + Integer::toString(imageSize));
	Debug::trace("KPCBootUpgrader", "processUpgrade(): ImageBufferPosition: " + Integer::toString(ImageBufferPosition));
	if (imageSize <= ImageBufferPosition)
	{
		Debug::trace("KPCBootUpgrader", "Download complete");
		mode = _Verify;
		return;
	}

	//Debug::trace("KPCBootUpgrader", "processUpgrade(): Image Size - Buffer pos: " + Integer::toString(imageSize - ImageBufferPosition));
	// determine the number of frames needed to send record
	if ((imageSize - ImageBufferPosition) >= (BYTES_PER_FRAME * MAX_FRAMES))
	{
		Debug::trace("KPCBootUpgrader", "Creating Bulk msg");
		//SOF-5023							//SOF-5332
		if( ( productID_ == K_KPC_PID ) || ( productID_ ==  K_KPCV_PID) ) {
			Debug::trace("KPCBootUpgrader", "Creating Bulk msg for K_KPC_PID");
			KPCBootUpgraderBulkMsg bulkMsg(this, &ImageBuffer.at(ImageBufferPosition),
												 __BlockWriteProgramFlash, (int)(MAX_FRAMES), BYTES_PER_FRAME, 0x01 );
			bulkMsg.show();
			dispatchEvent(&bulkMsg);
			LastBufferPosition = ImageBufferPosition;
			ImageBufferPosition += BYTES_PER_FRAME * MAX_FRAMES;

		} else {
			KPCBootUpgraderBulkMsg bulkMsg(this, &ImageBuffer.at(ImageBufferPosition),
												 __BlockWriteProgramFlash, (int)(MAX_FRAMES), BYTES_PER_FRAME, 0x04 );			
			bulkMsg.show();
			dispatchEvent(&bulkMsg);
			LastBufferPosition = ImageBufferPosition;
			ImageBufferPosition += BYTES_PER_FRAME * MAX_FRAMES;
		}
	
	}
	else
	{
		Debug::trace("KPCBootUpgrader", "Creating partial Bulk msg");
		//SOF-5023							//SOF-5332
		if( ( productID_ == K_KPC_PID ) || ( productID_ ==  K_KPCV_PID) ) {
			KPCBootUpgraderBulkMsg bulkMsg(this, &ImageBuffer.at(ImageBufferPosition),
														 __BlockWriteProgramFlash, (imageSize - ImageBufferPosition) / MAX_FRAMES, imageSize % BYTES_PER_FRAME, 0x01);
			bulkMsg.show();
			dispatchEvent(&bulkMsg);
			LastBufferPosition = ImageBufferPosition;
			ImageBufferPosition += ((((imageSize - ImageBufferPosition) / MAX_FRAMES) - 1) * BYTES_PER_FRAME) + (imageSize % BYTES_PER_FRAME);

		} else {
			Debug::trace("KPCBootUpgrader", "Creating Bulk msg for K_KPC_PID");
			KPCBootUpgraderBulkMsg bulkMsg(this, &ImageBuffer.at(ImageBufferPosition),
														 __BlockWriteProgramFlash, (imageSize - ImageBufferPosition) / MAX_FRAMES, imageSize % BYTES_PER_FRAME, 0x04);			
			bulkMsg.show();
			dispatchEvent(&bulkMsg);
			LastBufferPosition = ImageBufferPosition;
			ImageBufferPosition += ((((imageSize - ImageBufferPosition) / MAX_FRAMES) - 1) * BYTES_PER_FRAME) + (imageSize % BYTES_PER_FRAME);
		}
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

bool KPCBootUpgrader::isUiEvent(KPCBootUpgraderEvent event){
	// first and last ui event
	return (event >= (KPCBootUpgraderEvent)KpcBootStartUpgrade && event <= (KPCBootUpgraderEvent)KpcBootUpgradeInfo);
}
