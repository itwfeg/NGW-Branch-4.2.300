#ifndef WRAPPERMANAGER_H
#define WRAPPERMANAGER_H

#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"

#include "OpModeEvent.h"
#include "WeigherEvent.h"
#include "WrapperEvent.h"
#include "SystemEvent.h"
#include "WrapperSensors.h"
#include "WrapperErrorHistory.h"
#include "WrapperStatus.h"
#include "WrapperInquire.h"
#include "EvolWrapperStatus.h"
#include "WrapperPackageHistory.h"
#include "EvolWrapperPackageHistory.h"
#include "EvolWrapperCameraScan.h"
#include "EvolWrapperSensors.h"
#include "EvolWrapperPackageDesc.h"
#include "VersionManagerEvent.h"
#include "EvolWrapperPkgDetectorStatus.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperConfiguration.h"


#include <SessionEvent.h>

class Configuration;
class EvolWrapperTrayNames;
class EvolWrapperPackageTimings;

class WrapperManager : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer
{

	public:
		static int ignoreWakeups;

		WrapperManager(Hobart::CEvolMsgReceiver *pMyWrapper,
					   OpModeEvent currentOpMode = OpModeOperate,
					   UINT16 physicalAddress = 0x8181,
					   WrapperKind wrKind = WrUltimaWrapper);
		~WrapperManager();

		virtual void timeoutProcess( __int32 timer_id );
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );


		bool isWrapperEvolWrapper();


	protected:

		ULONG wrapperProbeTime_;
		bool probeWrapper;

		bool trackCameras;
		EvolWrapperTest::WrEvolCameraTestParam trackCameraNumber;

		bool upgradeInProgress;

		ULONG requestSensorTime;
		bool requestSensorReading;
		bool trackSensors;

		ULONG wrapperTestTime_;

	private:
		
	    Hobart::CEvolMsgReceiver *pMyWrapper;
		Configuration *pConfig;

		INT32 ticksPerSecond;

		WrapperSensors lastSenseMsg;
		EvolWrapperSensors lastEvolWrapperSensors;

		WrapperErrorHistory lastErrorHistoryMsg;				//UWS error history
		WrapperPackageHistory wrapperPackageHistory_;

		EvolWrapperPackageHistory evolWrapperPackageHistory_;

		WrapperStatus lastWrapperStatus;
		EvolWrapperStatus lastEvolWrapperStatus;

		OpModeEvent currentOpMode;
		UINT16 physicalAddress;

		bool probeTimerOn;
		bool savingWrapperTimings_;
		bool waitingToCycleWrapper_;

		int dimensionErrors_;

		WrapperKind wrKind;

		bool sendGripperPWMTimings_;
		bool requestedErrorHistory_;
		bool requestedPkgTimings_;
		bool requestWrapperModelUI_;

		EvolWrapperTrayNames *wrTrayNames_;
		EvolWrapperTest *holdTestMsg_;

		EvolWrapperCameraScan lastLeftScan;
		EvolWrapperCameraScan lastRightScan;

		EvolWrapperPkgDetectorStatus elevatorDetectorStatus;
		EvolWrapperPkgDetectorStatus sealerDetectorStatus;

		bool isManfTestKeyPresent_;
		ULONG evolWrapperCycleCount_;
		ULONG previousCycleCount_;

		std::string style;
		bool partialCan_;
		bool fullBrightCan_;

		int msgCntr_;

		bool pollTimerStopped_;
		bool pppTestTimerStart_;
		bool pppPollStart_;

		WrapperManager();

		void handleSessionDevMessage(SessionEvent e, const char *recvBuffer);
		void handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer);
		void handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer);
		void handleWrapperSysEvent(SysEvent sysEvent, const char *recvBuffer);
		void handleVersionManagerMessage(VersionManagerEvent vmEvent, char *recvBuffer);

		void requestWakeupMsg();
		void requestSystemWakeupMsg();
		void requestStatusMsg();
		void requestSensorMsg();
		void requestErrorHistoryMsg();
		void requestCamerasScanMsg(EvolWrapperTest::WrEvolCameraTestParam whichCamera);
		void receivedWrapperFeedback();

		void requestInquireMsg();

		void sendWrapperKind();
		void sendMessageToCommMgr(const char* buffer);

		void addPackageToHistory(WrPackCode packCode, UINT16 width, UINT16 length);
		void addPackageToEvolHistory(EvolWrapperPackageDesc &wrPackageDesc);

		EvolWrapperPackageTimings* getEvolWrapperPackageTimings();
		void setEvolWrapperPackageTimings(EvolWrapperPackageTimings *packageTimings);

		EvolWrapperPackageTimings *pMyTimings_;

		void setEvolWrapperTrayNames(EvolWrapperTrayNames *trayNames);

		void setWrapperCycleCount(ULONG count){evolWrapperCycleCount_ = count;} 
		void requestEvolWrapperCycleCount();
		void requestEvolWrapperSystemInfo();
		void requestEvolWrapperSystemReset();
		void requestEvolWrapperCameraThreshold();

		void registerOperationalEvents();
		void sendEvolWrapperTest(EvolWrapperTest *testMsg = 0);		//	HA-81

		void writeCycleCountsToFlashThread(); // SOF-2402

		int probeTimeoutCntr_;

		enum {CAMERA_TRACKING_TIMER = 500};
		enum {HEIGHT_SCAN_TRACKING_TIMER = 250};

		enum WrapperManagerTimer
		{
			wrapperProbe = 1,
			wrapperNoProbeResponse,
			sensorTimerExpired,
			statusTimerExpired,
			cycleCountTimerExpired,
			wrapperTestTimerExpired,
			cameraTracking,
			heightScanTracking,
			pppTestTimer,
			pppProbeTimer
		};

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //WRAPPERMANAGER_H
