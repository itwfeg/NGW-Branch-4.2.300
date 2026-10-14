
/**
 * @file CANWeigherManager.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Header file prepack CAN weigher device
 * Created on: Oct 24, 2018
 * @addtogroup CAN
 * @{
 */
#ifndef CANWEIGHERMANAGER_H_
#define CANWEIGHERMANAGER_H_
#include "CHobartTimer.h"
#include "CEvolMsgReceiver.h"
#include "Weigher.h"
#include "WeigherEvent.h"
#include "WeigherStatus.h"
#include "SystemEvent.h"
#include "KpcSystemEvents.h"

class CANWeigherManager : public Hobart::CEvolMsgReceiver, Hobart::CHobartTimer
{

	public:
		CANWeigherManager( Hobart::CEvolMsgReceiver *pMyWeigher, WeigherKind wgKind = WgCanPrepack );
		~CANWeigherManager();


		virtual void timeoutProcess( __int32 timer_id );
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	protected:

		ULONG pollTime_;

		//#define CAT3_UNIT_TEST
		#ifdef CAT3_UNIT_TEST
		#define _UNIT_TEST
		#endif

		//#define VALUE_MAX_UNIT_TEST
		#ifdef VALUE_MAX_UNIT_TEST
		#define _UNIT_TEST
		#endif

		//#define WG_DEFAULTS_TEST
		#ifdef WG_DEFAULTS_TEST
		#define _UNIT_TEST
		#endif

		#ifdef _UNIT_TEST
		unsigned long testTime_;
		int testCounter_;
		int testMsgCount_;
		#endif



	private:
		CANWeigherManager();
		void registerWeighEvents();

		void handleWeigherEvent(WgEvent wgEvent, const char *recvBuffer);
		void handleWeigherSysEvent(SysEvent sysEvent, const char *recvBuffer);
		void handleKpcSystemError( KpcSystemEvent event, const char *recvBuffer);
		void clearResponseCntr() {noResponseCntr_ = 0;}

		void requestWeigherWakeup();
		void requestWeigherSystemWakeup();
		void requestWeigherStatus();
		void requestWeigherSystemStatus();
		void requestWeigherInquire();
		void sendWeigherMode(WgOperatingMode opMode);
		void sendWeigherKind();
		void sendDateTime();



		std::string style_;

		// Pointer to the corresponding Translator. Used to send
		// messages to the physical device.
		Hobart::CEvolMsgReceiver *pTranslator_;

		// Controls whether weigher status messages received from the physical
		// device will be relayed through the system. A value of true ("locked")
		// means no relay.
		bool weigherLocked_;
		// Stores whether a large motion was received while "locked".
		bool largeMotionLocked_;
		bool cfgOutOfRange_;
		bool calSwitch_;

#if defined ( __linux__ )
		#define BOARD_SITE_FILE_NAME "/etc/hobart/boardsite" 
		bool boardSiteTest_;
		bool inquireOnce_;
#endif
		int noResponseCntr_;
		int reqConfigResendCounter_;

		bool vmErrorSent_;					//SOF-3958
		#define MILLISECTOSEC		1000	//SOF-3958
		


		WeigherKind wgKind_;

		WeigherStatus lastStatusMsg_;
		//	Keep track of the weigh mode because if it changes, we may need to send a WgColdReset to the weigher.
		WeighMode	currentWeighMode_;

		// Maintained by the WeigherManager so it can pass it along to the WeigherStatus
		// message, which needs it to determine the weight; see the getWeight() method
		// of WeigherStatus class.
		UINT32 scaleFactor_;

		enum WeigherManagerTimer
		{
			weigherProbe,
			WeigherTestCat3,
			WeigherTestValueMax,
			WeigherTestDefaults,
			WeigherReqConfig,
			VMErrorTimer					//SOF-3958
		};
};
#endif
