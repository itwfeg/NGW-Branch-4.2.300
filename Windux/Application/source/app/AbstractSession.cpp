#if 0
#define SHOW_BYTE_DETAIL
#endif
/* LX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


#include "AbstractSession.h"
#include "Session.h"
#include "SessionManager.h"
#include "SessionMessage.h"
#include "GenericHfcObject.h"
#include "Trace.h"
#include "Debug.h"
#include "Configuration.h"

#include "Integer.h"
#include "Hex.h"
#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int AbstractSession::_dataType_ = 0;
	std::string AbstractSession::_dataTypeName_("AbstractSession"); 
#endif


using namespace std;


// initialize statics
unsigned int AbstractSession::nextID = 0;


AbstractSession::AbstractSession(CommMechanism *uiClient, const char* name, bool start, bool managedThread, bool superThread)
: Hobart::CEvolMsgReceiver(name ? name : "AbstractSession", managedThread, superThread), reinitDB_(true), status(STATUS_UNKNOWN), sessionLocked( false )
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	autoGenerateName();

	Debug::trace("AbstractSession.construct", util_string::ptrtostring(this) + " - AbstractSession(CommMechanism *uiClient)");
	AbstractSession::init(uiClient);
	sessionThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, name ? name : "AbstractSession Thread", false , managedThread, superThread);			
	if (start)
		sessionThread_->RestartThread();
}

AbstractSession::~AbstractSession()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

	if (sessionThread_)
	{
		sessionThread_->StopThread();
		sessionThread_->WaitForCompletion();
	}
	ShutdownNicely();
	
	Debug::trace("AbstractSession.construct",
		util_string::ptrtostring(this) + " id = " + getName() + " - ~AbstractSession destructor");

	if (myMessenger.getMyUI())
	{
		Trace::getInstance()->removeTraceListener(myMessenger.getMyUI());
		myMessenger.getMyUI()->cmClose();
	}

	//	If the nextID value is one more than the current ID, we'll reuse the current ID next time around.
	if (nextID == (id + 1))
		nextID--;

	if (!monitoringSessions.empty())
		monitoringSessions.clear();
	
	deregisterAllEvents();
	//sessionThread->StopThread();
}


void AbstractSession::init(CommMechanism *uiClient)
{
	//Debug::setTraceLevel(60);
	Debug::trace("AbstractSession", HobString("init() - AbstractSession"));

	// The attribute, monitoringSessions object, needs no additional initialization beyond its constructor
	// The private attributes, id and name, will be initialized when determineSessionId() is called in Run().

	sessionType = SESSIONTYPE_UNKNOWN;

	//We don't want to sleep in Run() unless told to!
	sleepInRunFunction = false; 

	// did we try to register as a remote print session when we already had one going on?
	remoteRegFailed = false;

	// The attribute, status, is initialized in the constructor's data-member initialization

	myMessenger.setMySession(this);
	myMessenger.setMyUI(uiClient);
	
	//setStatus(STATUS_ACTIVE);
	Debug::trace("AbstractSession", HobString("AbstractSession init() finished."));
}


// private
void AbstractSession::autoGenerateName()
{
	char tName[11];
	id = nextID % 1000000;
	nextID++;
	sprintf(&tName[0], "tSsn%06d", id);
	name = tName;
	extName = name;
}

unsigned int AbstractSession::getId() const
{
	return id;
}

const HobString &AbstractSession::getName() const
{
	//return name;
	return getExtName();
}

const HobString &AbstractSession::getExtName() const
{
	return extName;
}

short AbstractSession::getSessionType() const
{
	return sessionType;
}

void AbstractSession::setSessionType(const short sessionType)
{
	if (sessionType < SESSIONTYPE_MIN || sessionType > SESSIONTYPE_MAX)
	{
		//todo - write warning log message indicating received bad message.
	}
	else
	{
		this->sessionType = sessionType;

		switch (sessionType)
		{
			case SESSIONTYPE_UNKNOWN:						extName.append("_unknownType"); break;
			case SESSIONTYPE_PRIMARY:						extName.append("_primary"); break;
			case SESSIONTYPE_REMOTE:						extName.append("_remote"); break;
			case SESSIONTYPE_COMM:							extName.append("_comm"); break;
			case SESSIONTYPE_INTRANET:						extName.append("_intranet"); break;
			case SESSIONTYPE_SCALEMANAGEMENT:				extName.append("_scaleMgmt"); break;
			case SESSIONTYPE_GCD:							extName.append("_gcd"); break;
			case SESSIONTYPE_HELPER:						extName.append("_helper"); break;
			case SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES:	extName.append("_downloadFile"); break;
			case SESSIONTYPE_HELPER_AUTOBACKUPDB:			extName.append("_autobackup"); break;
			case SESSIONTYPE_SMBACKEND:						extName.append("_smbackend"); break;
			case SESSIONTYPE_REMOTE_PRINT:                  extName.append("_remotePrint"); break;
			case SESSIONTYPE_HANDSFREEDEVICEMGR:            extName.append("_HandsFreeDeviceMgr"); break;
			case SESSIONTYPE_HELPER_SENDWRAPPERDATA:		extName.append("_sendWrapperData"); break;
			default:										extName.append("_???"); break;
		}

		// Check to make sure the SessionManager says this Session can become this type.
		if (!SessionManager::getInstance()->registerSessionType(this))
		{
			Debug::trace("AbstractSession", getName() + " setSessionType() - Could not register this session so go terminate connection.");
			terminate(true);
			return;
		}

		registerEvent(SessionDev, SnTerminate);
		registerEvent(SessionDev, SnShutdownNonPrimary);  // A primary session will turn this one off when it registers as the primary session.
	}
}

bool AbstractSession::isUnknownSession() const 
{
	return (sessionType == SESSIONTYPE_UNKNOWN) ? true : false;
}

bool AbstractSession::isPrimarySession() const
{
	return (sessionType == SESSIONTYPE_PRIMARY) ? true : false;
}

bool AbstractSession::isRemoteSession() const
{
	return (sessionType == SESSIONTYPE_REMOTE) ? true : false;
}

bool AbstractSession::isCommSession() const
{
	return (sessionType == SESSIONTYPE_COMM) ? true : false;
}

bool AbstractSession::isIntranetSession() const
{
	return (sessionType == SESSIONTYPE_INTRANET) ? true : false;
}

bool AbstractSession::isGCDSession() const
{
	return (sessionType == SESSIONTYPE_GCD) ? true : false;
}

bool AbstractSession::isScaleManagementSession() const
{
	return (sessionType == SESSIONTYPE_SCALEMANAGEMENT) ? true : false;
}

bool AbstractSession::isHelperSession() const
{
	return (sessionType == SESSIONTYPE_HELPER) ? true : false;
}

bool AbstractSession::isHelperProcessDownloadFilesSession() const
{
	return (sessionType == SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES) ? true : false;	
}

bool AbstractSession::isHelperBackupDBSession() const
{
	return (sessionType == SESSIONTYPE_HELPER_AUTOBACKUPDB) ? true : false;
}

bool AbstractSession::isSmBackendSession() const
{
	return (sessionType == SESSIONTYPE_SMBACKEND) ? true : false;
}

bool AbstractSession::isRemotePrintSession() const
{
	return (sessionType == SESSIONTYPE_REMOTE_PRINT) ? true : false;
}

bool AbstractSession::isHandsFreeDeviceMgrSession() const
{
	return (sessionType == SESSIONTYPE_HANDSFREEDEVICEMGR) ? true : false;
}
short AbstractSession::getStatus() const
{
	return status;
}
void AbstractSession::setStatus(const short status)
{
	if ( this->status != status )
	{
		Debug::trace("AbstractSession", getName() + HobString(" setStatus() - Session status is now ") + Integer::toString(status));
		this->status = status;
	}
}


Messenger &AbstractSession::getMessenger()
{
	return myMessenger;
}


/*****************************************************************************
 *
 * This is the method that allows other classes to send messages.
 * This allows the Session to have some control over whether messages
 * should be sent or not.  No one should ask the Session's Messenger directly
 * to send a message; they should go through the Session (via this method).
 *
 ****************************************************************************/
int AbstractSession::send(EvolMessage *message, int destination, bool shouldDelayAfterSending)
{
	int bytesSent = -1;

	if (message)
	{
		message->setSourceObject(this);

		if (Debug::isTracing("Session.send"))
		{
			const char *m = message->serialize();
			int ml = message->getSerializedLength();
			HobString hDest = (destination == Messenger::DEST_UI) ? "UI" :
										((destination == Messenger::DEST_RTC) ? "RTC" :
											((destination == Messenger::DEST_BACKUP) ? "BACKUP" : Integer::toString(destination)));
#ifdef SHOW_BYTE_DETAIL
			HobString h = getName() + HobString(" - message sent to ") + hDest + HobString(": ") + HobString(Hex::toString(m, ml));
			Debug::trace("Session.send", h);
#endif

			SessionMessage *s = (SessionMessage *)NULL;
			s = dynamic_cast<SessionMessage *> (message);
			
			if (s)
			{
				if ( s->getFormatType() == BrowserFormat || isCommSession() )
				{
					HobString h2 = getName() + HobString(" - message sent to ") + hDest + HobString(": ") + HobString(m, ml);
					Debug::trace("Session.send", h2);
				}
			}
			else if (Debug::isTracing("Session.sendAll"))
			{
				HobString h = string("SessionMessage is NULL, EvolMessage is: ") + getName() + HobString(" - message sent to ") + hDest + HobString(": ") + HobString(Hex::toString(m, ml));
				Debug::trace("Session.sendAll", h);
			}
		}

		bytesSent = myMessenger.send(message, destination, shouldDelayAfterSending);

		/* Have to check the destination below because we don't necessarily want to terminate the Session
		 * when it can't send a message.  Specifically, we don't want to terminate when sending a message
		 * to a file (in the case of a backup; The BackupRestoreManager will handle those errors).  Also,
		 * we don't necessarily want to terminate if we can't send a message to the RTC (EventManager). We
		 * could display a message in this case.
		 */
		if (bytesSent < 0 && destination == Messenger::DEST_UI)
		{
			/* Error occurred sending message!  Terminate this Session, but first send a message that
			 * we are terminating.
			 */
			Debug::trace("Session.send", getName() + HobString(" - Session send returned < 0 ... will be terminating ... "));
			if ( isRemotePrintSession() )
			{
				Debug::trace("ProductManager.remotePrint", getName() + " - Error occurred sending message, but dont terminate a remoteprint session");
				return -1;					// dont  be killing no sessions if we are remote. I be killin it myselves
			}

			terminate(true);
		}

		// Don't send RTC or file messages to monitoring sessions.  They don't need those messages.
		// Only send messages that are intended for the UI.
		if (destination == Messenger::DEST_UI) {
            //Debug::trace("Session.send", "sending message to monitoring sessions");
			sendToMonitoringSessions(message, destination);
        }
	}
	else
	{
		// todo - log warning message, trying to send a message without passing in a Message to send.
	}
	return bytesSent;
}


// The following is used for syncing up the DOM's.
int AbstractSession::send(const HobString &message, int destination, bool shouldDelayAfterSending)
{
	int bytesSent = -1;

	if (!message.empty())
	{
		if (Debug::isTracing("Session.send"))
		{
			HobString hDest = (destination == Messenger::DEST_UI) ? "UI" :
										((destination == Messenger::DEST_RTC) ? "RTC" : "DEST_" + Integer::toString(destination));
			HobString h = getExtName() + HobString(" - DOM-syncing message sent to ") + hDest + HobString(": ") +
							  HobString(Hex::toString(message.c_str(), (int)message.length()));
			Debug::trace("Session.send", h);

			HobString h2 = getExtName() + HobString(" - DOM-syncing message sent to ") + hDest + HobString(": ") + HobString(message.c_str(), message.length());
			Debug::trace("Session.send", h2);
		}

		bytesSent = myMessenger.send(message.c_str(), (int)message.length(), destination, shouldDelayAfterSending);
		if (bytesSent < 0)
		{
			/* Error occurred sending message!  Terminate this Session, but first send a message that
			 * we are terminating.
			 */
			Debug::trace("Session.send", getName() + HobString(" - Session send returned < 0 ... will be terminating ... "));
			terminate(true);
		}
	}
	return bytesSent;
}


void AbstractSession::terminate(bool shouldSendUINotification)
{
	Debug::trace("AbstractSession", getName() + " - TERMINATING!!! " + (shouldSendUINotification ? "send notification" : "") );


	// Tell myMessenger to close the connection between this Session and its UI.
	if (myMessenger.getMyUI())
		myMessenger.getMyUI()->cmClose();

	// Tell myMessenger to close this Session's Message Queue.
	// What else needs to be deleted.

	setStatus(STATUS_TERMINATED);
}


bool AbstractSession::operator==(const AbstractSession &aSession) const
{

	return (id == aSession.getId()) ? true : false;
}


void AbstractSession::addMonitoringSession(AbstractSession *session)
{
	Debug::trace("AbstractSession.monitor", "Adding a Session to list of monitoring Sessions ...");

	if (session && !isMonitoring(session))
	{
		Debug::trace("AbstractSession.monitor", "Added Session to list of monitoring Sessions :" + HobString(Hex::toString((const char *)&session, 4)));
		monitoringSessions.push_back(session);

		// todo - what else needs done when a Session initially enters monitor mode.
	}
}

void AbstractSession::removeMonitoringSession(AbstractSession *session)
{
	Debug::trace("AbstractSession.monitor", "Removing a Session from list of monitoring Sessions ...");

	if (session && !monitoringSessions.empty())
	{
		std::vector<AbstractSession *>::iterator pos;
		pos = monitoringSessions.begin();
		while (pos != monitoringSessions.end())
		{
			if (*pos == session)
			{
				Debug::trace("AbstractSession.monitor", "Removed Session from list of monitoring Sessions :" + HobString(Hex::toString((const char *)&session, 4)));
				monitoringSessions.erase(pos);
				break;
			}
			pos++;
		}

	}

	// todo - what else needs done when a Session leaves monitor mode.
}

// private
/**
 * Returns true if the Session is already monitoring the Primary Console.
 */
bool AbstractSession::isMonitoring(const AbstractSession *session) const
{
	if (session && !monitoringSessions.empty())
	{
		vector<AbstractSession *>::const_iterator pos;
		pos = monitoringSessions.begin();
		while (pos != monitoringSessions.end())
		{
			if (*pos == session)
				return(true);
			pos++;
		}
	}
	return(false);
}

// private
void AbstractSession::sendToMonitoringSessions(EvolMessage *message, int destination)
{
	/* if any sessions are monitoring the primary console, and the message was just sent to the UI,
	 * each monitoring session should also send the message to its UI.
	 */

	if (monitoringSessions.empty() || destination != Messenger::DEST_UI || !message || (message->getDeviceType() == SessionDev && message->getEventType() == SnSyncDOMRequest))
		return;

	vector<AbstractSession *>::iterator pos;
	pos = monitoringSessions.begin();
	while (pos != monitoringSessions.end())
	{
		Debug::trace("AbstractSession.monitor", "Sending this message to monitoring Session: " + HobString((*pos)->getName()));
		(*pos)->send(message, destination);
		pos++;
	}
}

// private
// The following is used for synching up the DOM's.
void AbstractSession::sendToMonitoringSessions(const HobString &message, int destination)
{
	/* if any sessions are monitoring the primary console, and the message was just sent to the UI,
	 * each monitoring session should also send the message to its UI.
	 */
	if (monitoringSessions.empty() || destination != Messenger::DEST_UI || message.empty())
		return;

	vector<AbstractSession *>::iterator pos;
	pos = monitoringSessions.begin();
	while (pos != monitoringSessions.end())
	{
		Debug::trace("AbstractSession.monitor", "Sending this DOM-synching message to monitoring Session: " + HobString((*pos)->getName()));
		(*pos)->send(message, destination);
		pos++;
	}
}


void AbstractSession::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	try
	{
#ifdef _DEBUG	
		{
			char s[200];
			Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
			sprintf( s, "%d:%d:%d: Session %d receives message from msg center (%d,%d): \r\n", 
						ct.GetHour(), ct.GetMinute(), ct.GetSecond(),
						id, msg->getDeviceType(), msg->getEventType() );
			OutputDebugStringA( s );
		}
#endif
		
		if (status == STATUS_TERMINATED)
			return;

		UINT32 result = SessionManager::getInstance()->requestToContinue(GetThreadID(), IsASuperThread());
		handleResultOfRTC(result);

		// SOF-1407
		Util::CSmartPtr<SessionMessage> sessionMsg( SessionMessage::create( msg->Data(), msg->Length() ) );
		if ( sessionMsg != NULL )
		{
			Util::CLockCriticalSection lock( sessionCS );
			sessionLocked = true;

			sessionMessageProcess( sessionMsg.get() );

			sessionLocked = false;
		}

		SessionManager::getInstance()->changeBlockabilityStatus(GetThreadID(), true, IsASuperThread());
	}
	catch (...)
	{
		Debug::trace("AbstractSession.error", name + HobString(" processEvent():   catch (...)    My id = ") + HobString(Integer::toString((int)id)));
		LogCatchAndThrow;
	}
}

DWORD AbstractSession::Run()
{
	while (status != STATUS_ACTIVE)
		PAL_absclass::PAL_sleep(100);

	Debug::trace("AbstractSession", name + HobString(" - Entering Run() ...  my id:") + HobString(Integer::toString((int)id)));
    PAL_absclass::PAL_sleep(0);

	initializationBeforeRun();

	const int delay( Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYBETWEENSESSIONMESSAGES) );

	size_t counter( 0 );

	while ( STATUS_TERMINATED != status )
	{
		SessionMessage *msg( NULL );

		// this check is eating a lot of CPU time... limit how often this happens...
		if ( 0 == (++counter % 32) )
		{
			const UINT32 result( SessionManager::getInstance()->requestToContinue(sessionThread_->GetThreadID(), sessionThread_->IsASuperThread()) );
			handleResultOfRTC( result );
		}

		try
		{
			if ( (msg = myMessenger.receive()) )
			{
				Util::CSmartPtr<SessionMessage> sessionMsg ( msg );

				Util::CLockCriticalSection lock( sessionCS );
				sessionLocked = true;

				sessionMessageProcess( sessionMsg.get() );
				
				sessionLocked = false;
			}
			else
			{
				noMessageFromMessengerProcessing();

				if ( sleepInRunFunction )
					PAL_absclass::PAL_sleep( delay );
				else
				{
					Debug::trace("AbstractSession.sleep", "Run1 - not sleeping: " + name);
					PAL_absclass::PAL_sleep( 1 );
				}
			}
		}

		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("Session::activeFunctionTask() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
			if (msg)
			{
				const HobString &m = msg->getOriginalBuff();
				HobString h = getName() + HobString(" - exception thrown on message: ") + HobString(Hex::toString(m.data(), (int)m.size()));

				Debug::trace("Session", h);
				if ( msg->getFormatType() == BrowserFormat || isCommSession())
				{
					HobString h2 = getName() + HobString(" - exception thrown on message: ") + m;
					Debug::trace("Session", h2);
				}
			}
			//status = STATUS_TERMINATED;
			//SessionManager::getInstance()->setAllowConnections(false);
		}

		catch (...)
		{
			Debug::trace("AbstractSession", "*****************************************************************");
			Debug::trace("AbstractSession", "****************** Just got an unknown exception ****************");
			Debug::trace("AbstractSession", "*****************************************************************");
		}
	}

	Debug::trace("AbstractSession", getName() + HobString(" - Leaving main loop in Run() ..."));

	{
		// This is probably not necessary now, but it may become important later on
		Util::CLockCriticalSection lock( sessionCS );
		sessionLocked = true;

		finalizationAfterRun();

		sessionLocked = false;
	}

	return 0;
}


// It is intended that this method will be overridden.
// virtual
void AbstractSession::noMessageFromMessengerProcessing()
{
}

// It is intended that this method will be overridden.
// virtual
void AbstractSession::sessionMessageProcess(SessionMessage * UNUSED(msg))
{
}

// It is intended that this method will be overridden.
//virtual
void AbstractSession::initializationBeforeRun()
{
}

// It is intended that this method will be overridden.
//virtual
void AbstractSession::finalizationAfterRun()
{
}

// It is intended that this method will be overridden.
//virtual
void AbstractSession::quitGracefully()
{
}

bool AbstractSession::unlockCritSectionForLongRunningProcess()
{
	if ( sessionLocked )
	{
		sessionCS.Leave();
		return true;
	}

	return false;
}

void AbstractSession::lockCritSectionAfterLongRunningProcess()
{
	sessionCS.Enter();
	sessionLocked = true;
} 
