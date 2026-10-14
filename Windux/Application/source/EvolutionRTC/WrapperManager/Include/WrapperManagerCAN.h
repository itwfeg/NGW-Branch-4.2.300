/**
 * @file WrapperManagerCAN.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief Header for AWS3 and NGW Wrappers Manager
 * Created on: Sept 5, 2017
 * @addtogroup CAN
 * @{
 */
#ifndef WRAPPERMANAGERCAN_H
#define WRAPPERMANAGERCAN_H
#include "Wrapper.h"
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "OpModeEvent.h"
#include "EvolWrapperStatus.h"
#include "EvolWrapperTest.h"
#include "EvolWrapperPackageTimings.h"
#include "EvolWrapperPackageHistory.h"
#include "EvolWrapperTrayNames.h"


class WrapperManagerCAN : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer {

	public:

	WrapperManagerCAN( Hobart::CEvolMsgReceiver *pMsgHandler, WrapperKind wrKind = WrAWS3Wrapper );
	~WrapperManagerCAN();

	virtual void timeoutProcess( __int32 id );
	virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	protected:

	private:
	Hobart::CEvolMsgReceiver *pMsgHandler_;
	OpModeEvent mode_;
	WrapperKind kind_;

	EvolWrapperStatus lastStatus_;

	void registerMyEvents();

	void handleWrapperEvent(WrEvent wrEvent, const char *recvBuffer);
	void sendKind();
	void requestPing();
	void requestSensors();
	void requestWrapperCfg();
	void requestWrapperWrapBoxTimings();


	int pingRetry_;
	void clearPingCount() { pingRetry_ = 0; }

	EvolWrapperTest::WrEvolCameraTestParam camera_;

	EvolWrapperPackageTimings *pMyTimings_;

	EvolWrapperPackageHistory pkgHistory_;
	EvolWrapperTrayNames *pTrayNames_;

	static const int ticksPerSec_ = 10000;
	static const int sensorPollTime_ = ticksPerSec_ / 8;
	static const int cameraPollTime_ = ticksPerSec_ / 25;	//~250 mSec

	int pingTime_;

	#define MAX_RETRYS	3
	#define WRAPPER_DISCONNTECTED	201

	typedef enum
	{
		PingTimer = 1,
		SensorPollTimer,
		CameraPollTimer,
		IntakeHeightPollTimer,
		UnitTestTimer
	}WrapperTimers;

};
#endif
