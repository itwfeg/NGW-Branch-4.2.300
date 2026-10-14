/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EMAILMANAGER_H
#define EMAILMANAGER_H

#include "evolution.h" 
#include "DBAccess.h"
#include <map>
#include "EmailComponent.h"
#include "SendErrorEmailEvent.h"
#include "SendGcdPlaylistErrorEmailEvent.h"
#include "SendGcdFtpErrorEmailEvent.h"
#include "time.h"
#include "PrinterStatus.h"
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"

class Email;
class OutgoingMailerI;
class HobEmailAlert;
class Session;
class SessionMessage;
namespace edb
{
class DBManager;
}

class EmailManager : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer, public DBAccess
{
	friend class SteveManger;

	public:
		// The constructor is private to properly implement the Singleton pattern
		~EmailManager();

		typedef std::map<int, EvolMessage*> pendingEvent_type;
		typedef std::map<int, EvolMessage*>::iterator pendingEvent_iterator;

		static EmailManager *getInstance();
		void handleMessage(SessionMessage *msg, Session * pSession);

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	private:
		OutgoingMailerI* outMailer_;
		pendingEvent_type pendingEvents_;
		bool receivedResolutionLast[SendErrorEmailEvent::EndErrorEmailCode];

		EmailManager();
		// Do not allow the default copy constructor.
		EmailManager(const EmailManager&);
		// Do not allow the default assignment operator.
		EmailManager operator=(const EmailManager& rhs);

		void init();
		STATUS addANewErrorEvent(SendErrorEmailEvent& pEvent);
		STATUS addANewErrorEvent(SendGcdFtpErrorEmailEvent& pEvent);
		STATUS addANewErrorEvent(SendGcdPlaylistErrorEmailEvent& pEvent);
		virtual void timeoutProcess( __int32 timer_id );
		void handleSendErrorEmail(SendErrorEmailEvent::ErrorEmailType eventType, HobEmailAlert* alert, HobString& msg);
		STATUS handlePrinterStatus(PrinterStatus& newPrinterStatus);
		
		// Singleton data members
		static EmailManager* _instance_;
		static Util::CSmartCriticalSection _singletonMutex_;

		static const int _MaxMessages_;
		static const int _MaxMessageSize_;
		static const int _MaxStackSize_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif //EMAILMANAGER_H

