#ifndef KPCBOOTUPGRADER_H_
#define KPCBOOTUPGRADER_H_
/**
 * @file KPCBootUpgrader.h
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Eric Landes
 * @brief CAN Wrapper firmware update manager
 *
 **/
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "CHobartTimer.h"
#include "CEvolMsgReceiver.h"
#include "CTLSThread.h"
#include "KPCBootUpgraderStatus.h"
#include "KPCBootUpgraderFrame.h"



#include <map>
#include <mutex>
#include <vector>
#define USEUI

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that Manages firmware updates of wrapper nodes
class KPCBootUpgrader : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{
  public:
	// ========================================================================
	// DTOR
	// ========================================================================

	virtual ~KPCBootUpgrader();

	/// Event handler for incoming messages
	/**
     * @param msg the message received
     */
	virtual void processEvent(Util::CSmartPtr<EvolMessageWrapper> &msg);

	// ========================================================================
	// Static Methods
	// ========================================================================

	static KPCBootUpgrader *getInstance();

	static void shutdown();

  protected:
	/// check each device in device list if firmware update is required
	virtual DWORD Run();

  private:
	std::string systemStyle;

	const int BYTES_PER_FRAME = 8;
	const int MAX_FRAMES = 8;
	const unsigned int RETRY_COUNT = 15;

	const int K_KPC_PID = 0x220;
	const int K_KPCV_PID 		= 0x221;	// HTm KPC with valuemax 
	const int K_FSSS_PID = 0x250;


	enum Mode
	{
		_Idle,
		_Startup,
		_RequestStatus,
		_WaitForStatus,
		_RequestBootVersion,
		_WaitForBootVersion,
		_CheckForUpgrade,
		_ProcessHeader,
		_ProcessFrames, 		
		_WaitForAck,
		_WaitForUI,
		_StartUI,
		_Flush,
		_Verify,				
		_UpgradeComplete,
		_Shutdown,
		_Done,
		_Fault,
		_SendUIStart,
		_SendUIProgress
	};

	static std::mutex instanceMutex_;
	static KPCBootUpgrader *instance_;

	static const std::string upgradePathKPC_;
	static const std::string upgradePathTof_;
	static std::string bootloaderVersion_;

	static const std::string upgradePathFSSS_;

	static const int canMsgTimeout_ = 4 * 1000; //4 sec timeout for waiting for can message
	static const int canAckTimeout_ = 5 * 1000; //10 sec timeout for waiting for all ack messages

	static unsigned long long LastBufferPosition;
	static unsigned long long ImageBufferPosition;
	static std::vector<unsigned char> ImageBuffer;

	static unsigned int retryCounter;
	static unsigned int retryFrameTimer_;

	static KPCBootUpgraderStatus::BootUpgraderFaults CurrentFault;
	static KPCBootUpgraderStatus::BootUpgraderStatusCode   CurrentStatusCode;

	static uint8_t FrameAckList;

	Mode mode = _Startup;
	Mode lastMode = mode;
	Mode returnState = mode; //mode to enter after waiting for ack or status

	Util::CTLSThread *myRunThread_;

	std::mutex mutex_;

	KPCBootUpgraderStatus *myStatus = nullptr;

	unsigned long productID_;
	unsigned long address_;
	//SOF-5023
	int lastPercent_;
	/// Constructor.
	KPCBootUpgrader();

	void handleKpcBootStatus(const KPCBootUpgraderStatus msg);

	void requestStatus();
	void requestAppVersion();
	void requestBootVersion();

	void handleKpcBootUpgraderEvent(KPCBootUpgraderEvent event, const char *recvBuffer);

	void handleUIEvent(KPCBootUpgraderEvent, const char *);


	void processUpgrade();
	bool isUiEvent(KPCBootUpgraderEvent);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* KPCBOOTUPGRADER_H_ */
