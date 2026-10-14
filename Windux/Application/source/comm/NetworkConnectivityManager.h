/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NETWORKCONNECTIVITYMANAGER_H
#define NETWORKCONNECTIVITYMANAGER_H


#include "CEvolMsgReceiver.h"
#include "TimerManager.h"

#include "util/thread/mutex.h"
#include "util/thread/lock.h"


class NetworkConnectivityManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{
	public:

		typedef enum SendCommState
		{
			FORCE_ONE_COMM_MSG,
			CHECK_FOR_TRANSITIONS
		} SendCommState_t;


		enum Activity
		{
			Activity_Normal = 0,
			Activity_Busy
		};

		~NetworkConnectivityManager();

		std::string getPingTarget() const { util_thread::Lock< util_thread::Mutex<> > lock(pingTargetMutex_); return pingTarget_; }
		void setPingTarget(const std::string& target) { util_thread::Lock< util_thread::Mutex<> > lock(pingTargetMutex_); pingTarget_ = target; }

		bool checkNetworkAvailability();

		Activity checkNetworkActivity() const;

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ); //lcg rev 0.1
		Util::CSmartPtr< EvolMessage> processEventExternal(const Util::CSmartPtr< EvolMessageWrapper> & msg );

		static NetworkConnectivityManager *getInstance();

	private:

		bool online_;
		bool checkEnabled_;
		bool networkTrafficOnly_;
		int networkType_;       // 0 = wired, 1 = wireless
		int wirelessStrength_;

		int onlineTimerId_;
		int onlinePingTimerId_;
		int offlineCheckingPingTimerId_;

		unsigned int currentPingIntervalValue_;

		/*dont send comm messages to session until its ready to receive them.
		this is to protect the ui at boot time.*/
		SendCommState commOutState_;

		Util::CMSGEvent onlineStateKnown_;

		int offLineTimeCounter_;
		Util::CSmartPtr<Util::CTLSThread>	networkConnectivityManagerThread;

		int offlinePingCounter_;
		int pingAttempts_;
		std::string pingTarget_;
		mutable util_thread::Mutex<> pingTargetMutex_;

		void setupPingTarget();
		bool isThereADuplicateIpAddress();
		void getDefaultGateway(std::string & defGWay);

		void transitionFromOnlineToOffline();

		NetworkConnectivityManager();

		bool isNetworkAlive();
		bool isNetworkPingable();

		void dispatchNetworkStatus(bool pingToCheckIfConnected = true);

		virtual DWORD Run();
		
		static NetworkConnectivityManager *instance;
		bool weAreBootedUp_;		

		Activity lastActivity_;
		DWORD inBusyThreshold_;
		DWORD outBusyThreshold_;
		mutable DWORD lastInBusyStat_;
		mutable DWORD lastOutBusyStat_;
		

	void PAL_executeWithResults(const std::string &command, std::string &commandOutput);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //NETWORKCONNECTIVITYMANAGER_H




