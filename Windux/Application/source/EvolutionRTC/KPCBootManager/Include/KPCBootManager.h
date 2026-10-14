#ifndef KPCBOOTMANAGER_H_
#define KPCBOOTMANAGER_H_
/**
 * @file KPCBootManager.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Eric Landes
 * @brief CAN Wrapper firmware update manager
 * 
 *
 **/
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "CHobartTimer.h"
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "KPCBootStatus.h"
#include "KPCBootFrame.h"
#include "WrapperEvent.h"

#include <map>
#include <mutex>
#include <vector>

#define USEUI

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that Manages firmware updates of wrapper nodes
class KPCBootManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{
  public:
	// ========================================================================
	// DTOR
	// ========================================================================

	virtual ~KPCBootManager();

	/// Event handler for incoming messages
	/**
     * @param msg the message received
     */
	virtual void processEvent(Util::CSmartPtr<EvolMessageWrapper> &msg);

	// ========================================================================
	// Static Methods
	// ========================================================================

	static KPCBootManager *getInstance();
	static bool IsUpgrading();

	static void shutdown();

  protected:
	/// check each device in device list if firmware update is required
	virtual DWORD Run();

  private:
	std::string systemStyle;
	const int BYTES_PER_FRAME = 8;
	const int MAX_FRAMES = 8;
	const unsigned int RETRY_COUNT = 3;
	enum Mode
	{
		_Idle,
		_Startup,
		_RequestStatus,
		_WaitForStatus,
		_RequestBootVersion,
		_WaitForBootVersion,
		_RequestAppVersion,
		_WaitForAppVersion,
		_CheckForUpgrade,
		_ProcessHeader,
		_ProcessFrames, 		//10
		_WaitForAck,
		_WaitForUI,
		_StartUI,
		_Flush,
		_Verify,				//15
		_UpgradeComplete,
		_Shutdown,
		_Done,
		_Jumping,
		_Fault,					//20
		_SendUIStart,
		_SendUIProgress,
		_WaitForSession,

		// SOF-3478
//		_WaitForUIDecision,	
		_CancelUpgrade,			
		_UpgradeRequest,		// 25 upgrade requested by UI (cancel timer ran out)
	};

	static std::mutex instanceMutex_;
	static KPCBootManager *instance_;

	static const std::string boardSite_;
	static const std::string upgradePathKPC_;
	static const std::string upgradePathTof_;
	static const std::string upgradePathFSSS_;
	static std::string bootloaderVersion_;
	static std::string weigherAppVersion_;
	static std::string printerAppVersion_;
	static std::string tofAppVersion_;

	static const int canMsgTimeout_ = 4 * 1000; //4 sec timeout for waiting for can message
	static const int canAckTimeout_ = 5 * 1000; //10 sec timeout for waiting for all ack messages
	static const int uiResponseTimeout_ = 10000;  // 10 sec timeout, waiting for user input 
	static const int canMsgTimerID_ = 1;
	static const int canAckTimerID_ = 2;

	static KPCBootStatus::BootloaderFaults CurrentFault;
	static KPCBootStatus::BootStatusCode   CurrentStatusCode;

	static unsigned long long LastBufferPosition;
	static unsigned long long ImageBufferPosition;
	static std::vector<unsigned char> ImageBuffer;

	static unsigned int retryCounter;
	static unsigned int retryFrameTimer_;
	static uint8_t FrameAckList;

	Mode mode = _Startup;
	Mode lastMode = mode;
	Mode returnState = mode; //mode to enter after waiting for ack or status

	bool upgradeWanted; 	// flag to keep track of if user wants to upgrade or not when requested
	bool shutUpSent;	// SOF-5058: flag to track if command to quiet ready message so that boot manager thread is not restarted.

	Util::CTLSThread *myRunThread_;

	std::mutex mutex_;

	KPCBootStatus *myStatus = nullptr;

	static const int K_KPC_PID 			= 0x220;	// HTm KPC without valuemax 
	static const int K_KPCV_PID 		= 0x221;	// HTm KPC with valuemax 
	static const int K_KPC_ARRATA		= 0x020;	// SOF-4985 
	static const int K_KPCV_ARRATA	= 0x021;	// SOF-4985 
	static const int K_FSSS_PID 		= 0x250;	// Free Standing Self Service Scale 
	static const int tofID 					= 112;		// Wrapper 2 Printer System with TOF Controller

	unsigned long productID_;
	uint8_t address_;

    int prevPercent;
    
	/// Constructor.
	KPCBootManager();

	void handleKpcBootReady();
	void handleKpcBootStatus(const KPCBootStatus msg);

	void requestStatus();
	void requestAppVersion();
	void requestBootVersion();

	void handleKpcBootEvent(KPCBootEvent event, const char *recvBuffer);
	#ifdef USEUI
	void handleUIEvent(KPCBootEvent event, const char *recvBuffer);
	#endif

	void processUpgrade();
	void timeoutProcess(int timerId);

	bool isUiEvent(KPCBootEvent);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
