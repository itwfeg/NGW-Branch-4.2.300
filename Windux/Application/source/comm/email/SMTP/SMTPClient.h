#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H



#include <string>



#include "SMTPClientEvent.h"

class Email;
#include "../EmailComponent.h"
#include "../Email.h"
#include "HobSocket.h"

#include "StateMachine/StateMachineController.h"
#include "CTLSThread.h"
#include "CSmartPtr.h"

class SMTPClientInterface;
template <class Controller> class StateMachine;



class SMTPClient : public Util::CRunnableClass, public IStateMachineController
{
	public:
		// Types and subtypes
		typedef std::list<Email*> pendingList_type;
		typedef enum
		{
			NOT_COMPLETE,
			SUCCESS_CODE,
			FAILURE_CODE,
			TIME_OUT
		} responseCode;

		typedef enum
		{
			IDLING_STATE,
			CONNECTING_STATE,
			SENDING_HELO_STATE,
			SENDING_MAIL_STATE,
			SENDING_TO_STATE,
			SENDING_CC_STATE,
			SENDING_BCC_STATE,
			SENDING_DATA_STATE,
			SENDING_BODY_STATE,
			SENDING_QUIT_STATE,
			SETTINGUP_RETRIES_STATE,
			REJECTING_EMAIL_STATE
		} stateId_type;
		typedef SMTPClientEvent event_type;
		typedef StateMachine<SMTPClient> stateMachine_type;


		// Constructors and destructor
		SMTPClient(const SMTPClientInterface& interface1);
		~SMTPClient();

		// Accessor methods
		// Most of these methods are protected by critical sections so there will be no
		// conflict when either the SMTPClient or the SMTPClientInterface, which run in
		// different threads, call them at the same time
		const std::string& getSMTPServerName() const
				{ Util::CLockCriticalSection lock(serverParametersMutex_); return serverName_; }
		void setSMTPServerName(const std::string& name)
				{ Util::CLockCriticalSection lock(serverParametersMutex_); serverName_ = name; }
		const std::string& getSMTPServerIP() const
				{ Util::CLockCriticalSection lock(serverParametersMutex_); return serverIP_; }
		void setSMTPServerIP(const std::string& ip)
				{ Util::CLockCriticalSection lock(serverParametersMutex_); serverIP_ = ip; }
		int getSMTPServerPort() const
				{ Util::CLockCriticalSection lock(serverParametersMutex_); return serverPort_; }
		void setSMTPServerPort(int port) 
				{ Util::CLockCriticalSection lock(serverParametersMutex_); serverPort_ = port; }

		void addEmail(Email* email)
				{ Util::CLockCriticalSection lock(pendingMutex_); pending_.push_back(email); }
		size_t sizePendingEmails() const
				{ Util::CLockCriticalSection lock(pendingMutex_); return pending_.size(); }

//		void debug(const std::string& category, int level, const std::string& message) const;



	protected:
		DWORD Run();

	private:
		const SMTPClientInterface& interface_;
		std::string serverName_;
		std::string serverIP_;
		int serverPort_;
		mutable Util::CSmartCriticalSection serverParametersMutex_;

		pendingList_type pending_;
		mutable Util::CSmartCriticalSection pendingMutex_;

		StateMachine<SMTPClient>* fsm_;

		Socket * sock_;

                util_time::DateTime responseTime_;

		Email *currentEmail_;
		bool leaveState_;

		Email::addressList* emailList_;
		Email::addressList goodList_;
		Email::addressListIterator listIter_;

		bool firstTime_;
		bool oneSuceeded_;
		bool oneFailed_;
		std::string currentEmailAddr_;

		// Do not allow the default copy constructor.
		SMTPClient(const SMTPClient&);

		// Do not allow the default assignment operator.
		SMTPClient operator=(const SMTPClient& rhs);


		void setupStateMachine();
		Email* getCurrentEmail()
		{ Util::CLockCriticalSection lock(pendingMutex_);
				 Email * pFront = pending_.front();
				 pending_.pop_front();
				 return pFront; }
		bool getResponse(responseCode& result, std::string responseCode, int timeOutSecs);

		// Action functions
		bool enterIdlingState(const SMTPClientEvent& event);
		bool idlingState(const SMTPClientEvent& event);
		bool enteringConnectingState(const SMTPClientEvent& event);
		bool processingConnectingState(const SMTPClientEvent& event);
		bool enteringSendingHeloState(const SMTPClientEvent& event);
		bool processingSendingHeloState(const SMTPClientEvent& event);
		bool enteringSendingMailState(const SMTPClientEvent& event);
		bool processingSendingMailState(const SMTPClientEvent& event);
		bool enteringSendingToState(const SMTPClientEvent& event);
		bool leavingSendingToState(const SMTPClientEvent& event);
		bool enteringSendingCCState(const SMTPClientEvent& event);
		bool leavingSendingCCState(const SMTPClientEvent& event);
		bool enteringSendingBCCState(const SMTPClientEvent& event );
		bool leavingSendingBCCState(const SMTPClientEvent& event );
		bool processingSendingRcptState(const SMTPClientEvent& event);
		bool enteringSendingDataState(const SMTPClientEvent& event);
		bool processingSendingDataState(const SMTPClientEvent& event);
		bool enteringSendingBodyState(const SMTPClientEvent& event);
		bool processingSendingBodyState(const SMTPClientEvent& event);
		bool enteringSendingQuitState(const SMTPClientEvent& event);
		bool processingSendingQuitState(const SMTPClientEvent& event);
		bool retryingState(const SMTPClientEvent& event);
		bool rejectingState(const SMTPClientEvent& event);


		static const std::string _ThreadName_;
		static const int _ThreadPriority_;
		static const int _ThreadStackSize_;
		static const int _ThreadDelay_;
		static const int _retryDelay_;

		static const int _DefaultSMTPServerPort_;

		static const int _MaxRetryTime_;
		static const int _RetryDelay_;
		static const int _idleDelay_;

		Util::CSmartPtr<Util::CTLSThread> checkingThread;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

};



#endif //SMTPCLIENT_H

