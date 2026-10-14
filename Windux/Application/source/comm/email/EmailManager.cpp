/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "EmailManager.h"
#include "EmailManagerEvents.h"
#include "Email.h"
#include "OutgoingMailerI.h"
#include "SMTP/SMTPClientInterface.h"
#include "Session.h"
#include "SessionMessage.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "DBManager.h"
#include "util/e_string/utilstring.h"
#include "Configuration.h"
#include "HobEmailAlert.h"
#include "SessionManager.h"
#include "NetworkConnectivityEvents.h"
#include "GenericHfcObject.h"
#include "HandsFreeDeviceEvent.h"

#include <time.h>

/*STATUS testSendErrorEmail(bool error, bool outOfLabels)
{
EventManager * pEventMgr = EventManager::getInstance();

EmailManager* pMan = EmailManager::getInstance();
SendErrorEmailEvent event(error ? SendErrorEmailEvent::ErrorEmailTypeError: SendErrorEmailEvent::ErrorEmailTypeResolved,
						  outOfLabels ? SendErrorEmailEvent::ErrorEmailCodeOutOfLabels : SendErrorEmailEvent::ErrorEmailCodeCassetteOpen);

DispatchEvent de(&event, pEventMgr);
const char *pDisMsg = de.serialize();
int mLength = de.getSerializedLength();
int rv;

	rv = pEventMgr->send(pDisMsg, mLength);

	pMan = NULL; //used only to make sure that version manager is instantiated.

return OK;
}*/

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int EmailManager::_dataType_ = 0;
	std::string EmailManager::_dataTypeName_("EmailManager"); 
#endif

const int EmailManager::_MaxMessages_ = 100;
const int EmailManager::_MaxMessageSize_ = 2048;
const int EmailManager::_MaxStackSize_ = 48;
Util::CSmartCriticalSection EmailManager::_singletonMutex_;;
EmailManager* EmailManager::_instance_ = 0;

EmailManager::EmailManager() : CEvolMsgReceiver("Email manager", true, false), outMailer_(new SMTPClientInterface()), pendingEvents_()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	init();
}


EmailManager::~EmailManager()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	shutdownTimerThreads();
	ShutdownNicely();
	if (outMailer_)
		delete outMailer_;
}


EmailManager *EmailManager::getInstance()
{
	if ( _instance_ == 0 )
	{
		Util::CLockCriticalSection lock(_singletonMutex_);

		if ( _instance_ == 0 )
			_instance_ = new EmailManager();
	}

	return _instance_;
}

void EmailManager::init()
{
	registerEvent(EmailManagerDev, SendEmail);
	registerEvent(EmailManagerDev, SendErrorEmail);
	registerEvent(EmailManagerDev, SendGcdPlaylistErrorEmail);
	registerEvent(EmailManagerDev, SendGcdFtpErrorEmail);

	registerEvent(Printer, PrStatus);

	try
	{
		Debug::trace("EmailManager", "before initializing DBManager");
		initDB();
		Debug::trace("EmailManager", "after  initializing DBManager");
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("EmailManager::activeFunctionTask() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}		
	}

	int searchCode;
	for (searchCode = 0; searchCode < SendErrorEmailEvent::EndErrorEmailCode; searchCode++)
	{
		HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert((HobRecordNum)searchCode, this);
		if (emailAlert)
		{
			receivedResolutionLast[searchCode] = true;
			if ((emailAlert->getErrorCode() != RECORD_NOT_FOUND) && (emailAlert->getFailureSent()))
				receivedResolutionLast[searchCode] = false;

			delete emailAlert;
		}
	}
}

void EmailManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	EvolMessage message( msg->Data() );
	unsigned threadId = CEvolMsgReceiver::GetThreadID();

	SessionManager::getInstance()->requestToContinue(threadId, false);

	switch (message.getDeviceType())
	{
		case EmailManagerDev:
		{
			switch ( message.getEventType())
			{
	/*			case SendEmail:
				{
					SendEmailEvent evt(messageBuffer, messageSize);
					handleSendEmail(evt);

					break;
				} */
				case SendErrorEmail:
				{
					Debug::trace("EmailManager","SendErrorEmail event");

					SendErrorEmailEvent newEvent(msg->Data());

					addANewErrorEvent (newEvent);

					break;
				}
				case SendGcdPlaylistErrorEmail:
				{
					// FT-761
					Debug::trace("EmailManager","SendGcdPlaylistErrorEmail event");
					SendGcdPlaylistErrorEmailEvent newEvent(msg->Data(), msg->Length());
					addANewErrorEvent (newEvent);
					break;

				}
				case SendGcdFtpErrorEmail:
				{
					// FT-761
					Debug::trace("EmailManager","SendGcdFtpErrorEmail event");
					SendGcdFtpErrorEmailEvent newEvent(msg->Data());
					addANewErrorEvent (newEvent);
					break;

				}
				default:
				{
					break;
				}
			}
			break;
		}

		case Printer:
		{
			Debug::trace("EmailManager", "Handling printer message");

			if (message.getEventType() == PrStatus)
			{
				Debug::trace("EmailManager", "Handling printer status message");

				PrinterStatus ps(msg->Data());

				handlePrinterStatus(ps);

			}
			break;
		}

		default:
			break;
	}

	SessionManager::getInstance()->changeBlockabilityStatus(threadId, true, false);

	PAL_absclass::PAL_sleep( 10 ); 
}

STATUS EmailManager::addANewErrorEvent(SendGcdFtpErrorEmailEvent& evt)
{
	std::string dbgmsg;

	SendErrorEmailEvent::ErrorEmailCode searchCode = SendErrorEmailEvent::ErrorEmailCodeGcdFtpError;
	SendErrorEmailEvent::ErrorEmailType searchType = SendErrorEmailEvent::ErrorEmailTypeGcdFtpError;

	HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert((HobRecordNum)searchCode, this);
	if (emailAlert)
	{
		if (emailAlert->getErrorCode() == RECORD_NOT_FOUND)
		{
			dbgmsg = "DB find failed. Did not find the error code =";
			dbgmsg += Integer::toString (searchCode);

			Debug::trace("EmailManager", dbgmsg);

			delete emailAlert;
			return ERROR_CODE;
		}

		dbgmsg = "DB find succeeded. Found the error code =";
		dbgmsg += Integer::toString (searchCode);

		Debug::trace("EmailManager", dbgmsg);

		if (emailAlert->getNotifyOnFailure() || emailAlert->getNotifyHandsFreeDeviceOnFailure())
		{

			int delayBeforeSending = emailAlert->getDelay();

			dbgmsg = "we must create a new timer, timeout duration :";

			dbgmsg += Integer::toString(delayBeforeSending);

			Debug::trace("EmailManager", dbgmsg);

			if (delayBeforeSending > 0)
			{
				static int  timerId = 0;
				timerId++;
				createTimer( delayBeforeSending * 60 * 1000, timerId);

				SendGcdFtpErrorEmailEvent * pNewErrorEvent = new SendGcdFtpErrorEmailEvent(evt.getErrorMessage(), this);
				pendingEvents_.insert(std::make_pair(timerId, pNewErrorEvent));

			}
			else
			{
				Debug::trace("EmailManager", "Timer duration was 0. Go ahead and send it out now.");
				HobString msg(evt.getErrorMessage());
				handleSendErrorEmail(searchType, emailAlert, msg);
			}
		}
	}

	return OK;
}

STATUS EmailManager::addANewErrorEvent(SendGcdPlaylistErrorEmailEvent& evt)
{
	std::string dbgmsg;

	SendErrorEmailEvent::ErrorEmailCode searchCode = SendErrorEmailEvent::ErrorEmailCodeGcdPlaylistError;
	SendErrorEmailEvent::ErrorEmailType searchType = SendErrorEmailEvent::ErrorEmailTypeGcdPlaylistError;

	HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert((HobRecordNum)searchCode, this);
	if (emailAlert)
	{
		if (emailAlert->getErrorCode() == RECORD_NOT_FOUND)
		{
			dbgmsg = "DB find failed. Did not find the error code =";
			dbgmsg += Integer::toString (searchCode);

			Debug::trace("EmailManager", dbgmsg);

			delete emailAlert;
			return ERROR_CODE;
		}

		dbgmsg = "DB find succeeded. Found the error code =";
		dbgmsg += Integer::toString (searchCode);

		Debug::trace("EmailManager", dbgmsg);

		if (emailAlert->getNotifyOnFailure() || emailAlert->getNotifyHandsFreeDeviceOnFailure())
		{

			int delayBeforeSending = emailAlert->getDelay();

			dbgmsg = "we must create a new timer, timeout duration :";

			dbgmsg += Integer::toString(delayBeforeSending);

			Debug::trace("EmailManager", dbgmsg);

			if (delayBeforeSending > 0)
			{
				static int  timerId = 0;
				timerId++;
				createTimer( delayBeforeSending * 60 * 1000, timerId);

				SendGcdPlaylistErrorEmailEvent * pNewErrorEvent = new SendGcdPlaylistErrorEmailEvent(evt.getErrorMessage(), this);
				pendingEvents_.insert(std::make_pair(timerId, pNewErrorEvent));

			}
			else
			{
				Debug::trace("EmailManager", "Timer duration was 0. Go ahead and send it out now.");
				HobString msg(evt.getErrorMessage());
				handleSendErrorEmail(searchType, emailAlert, msg);

			}
		}
	}

	return OK;
}

STATUS EmailManager::addANewErrorEvent(SendErrorEmailEvent& evt)
{
	bool found = false;
	std::string dbgmsg;

	SendErrorEmailEvent::ErrorEmailCode searchCode = evt.getCode();
	SendErrorEmailEvent::ErrorEmailType searchType = evt.getType();

	if ((searchCode >= SendErrorEmailEvent::EndErrorEmailCode) ||
		((searchType == SendErrorEmailEvent::ErrorEmailTypeResolved) && (receivedResolutionLast[searchCode] == true)) ||
		((searchType == SendErrorEmailEvent::ErrorEmailTypeError) && (receivedResolutionLast[searchCode] == false)))
		return OK;		//	Nothing has changed so we will do nothing.

	if (searchType == SendErrorEmailEvent::ErrorEmailTypeResolved)
		receivedResolutionLast[searchCode] = true;
	else if (searchType == SendErrorEmailEvent::ErrorEmailTypeError)
		receivedResolutionLast[searchCode] = false;

	HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert((HobRecordNum)searchCode, this);
	if (emailAlert)
	{
		if (emailAlert->getErrorCode() == RECORD_NOT_FOUND)
		{
			dbgmsg = "DB find failed. Did not find the error code =";
			dbgmsg += Integer::toString (searchCode);

			Debug::trace("EmailManager", dbgmsg);

			delete emailAlert;
			return ERROR_CODE;
		}

		dbgmsg = "DB find succeeded. Found the error code =";
		dbgmsg += Integer::toString (searchCode);

		Debug::trace("EmailManager", dbgmsg);

		if ( (searchType == SendErrorEmailEvent::ErrorEmailTypeError) && ( emailAlert->getNotifyOnFailure() || emailAlert->getNotifyHandsFreeDeviceOnFailure() ) )
		{// we must check to see if a timer already exists for this event. if one does then do nothing
			pendingEvent_iterator i;
			SendErrorEmailEvent* pTestEvent = 0;

			Debug::trace("EmailManager", "we have a new error event. check to see if one already exists");

			for (i = pendingEvents_.begin(); i != pendingEvents_.end() ; i++)
			{
				pTestEvent = dynamic_cast<SendErrorEmailEvent* > (i->second);

				if (( pTestEvent ) && ( pTestEvent->getCode() == searchCode ))
				{
					found = true;
					break;
				}

			}

			if ((!found) && ! emailAlert->getFailureSent())
			{// a timer has not been created for this event. create it and then start it.
				int delayBeforeSending = emailAlert->getDelay();

				dbgmsg = "we must create a new timer, timeout duration :";

				dbgmsg += Integer::toString(delayBeforeSending);

				Debug::trace("EmailManager", dbgmsg);

				if (delayBeforeSending > 0)
				{
					static int  timerId = 0;
					timerId++;
					createTimer( delayBeforeSending * 60 * 1000, timerId);

					SendErrorEmailEvent * pNewErrorEvent = new SendErrorEmailEvent(searchType, searchCode, this);
					pendingEvents_.insert(std::make_pair(timerId, pNewErrorEvent));

					emailAlert->setFailureSent(false);
					emailAlert->saveRecord();
				}
				else
				{
					Debug::trace("EmailManager", "Timer duration was 0. Go ahead and send it out now.");
					HobString empty_str("");
					handleSendErrorEmail(searchType, emailAlert, empty_str);

					emailAlert->setFailureSent(true);
					emailAlert->saveRecord();
				}
			}
			else
			{
				Debug::trace("EmailManager", "The failure email has already been sent out");
			}
		}
		else if (searchType == SendErrorEmailEvent::ErrorEmailTypeResolved)
		{
			pendingEvent_iterator i;
			SendErrorEmailEvent* pTestEvent = 0;

			Debug::trace("EmailManager", "This is a resolved event, try to see if it is pending");

			for (i = pendingEvents_.begin(); i != pendingEvents_.end(); i++)
			{
				pTestEvent = dynamic_cast<SendErrorEmailEvent*> (i->second);

				if (( pTestEvent ) && ( pTestEvent->getCode() == searchCode )) // FT-1038
				{
					found = true;
					break;
				}
			}

			if (found)
			{// found the event. we will destroy it no matter if we send out the resolve email or not

				Debug::trace("EmailManager", "corresponding error email was found in pending map");

				dbgmsg = "kill timer: ";

				dbgmsg += Integer::toString((int)i->first);

				Debug::trace("EmailManager", dbgmsg);

				cancelTimer(i->first);

				pendingEvents_.erase(i->first);

				delete pTestEvent;
			}
			else
			{// maybe have turned off
				Debug::trace("EmailManager", "Timer not found in map.");
			}

			if ((emailAlert->getNotifyOnResolution() || emailAlert->getNotifyHandsFreeDeviceOnResolution()) && emailAlert->getFailureSent() ) //get from db whether or not the problem email was sent out.
			{
				Debug::trace("EmailManager", "Sending out the resolved email");
				HobString empty_str("");
				handleSendErrorEmail(searchType, emailAlert, empty_str);
			}
			else
			{
				Debug::trace("EmailManager", "Not sending out the resolved email");
			}
			emailAlert->setFailureSent(false);
			emailAlert->saveRecord();
		}
		delete emailAlert;
	}

	return OK;
}

void EmailManager::timeoutProcess( __int32 timerID )
{
	pendingEvent_iterator i = pendingEvents_.find(timerID);
	std::string dbgmsg;

	dbgmsg = "Handling timer timeout, timer id :";

	dbgmsg += Integer::toString((int)timerID);
	Debug::trace("EmailManager",dbgmsg);

	if (i != pendingEvents_.end())
	{
		EvolMessage* pEvent = i->second;

		if (pEvent)
		{
			switch ( pEvent->getEventType() )
			{

	/*			case SendEmail:
				{
					SendEmailEvent event(messageBuffer, messageSize);
					handleSendEmail(event);

					break;
				}*/
				case SendGcdFtpErrorEmail:
				{
					SendGcdFtpErrorEmailEvent* pErrorEvent =  dynamic_cast<SendGcdFtpErrorEmailEvent*> (pEvent);
					HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeGcdFtpError, this);
					
					if (( pErrorEvent ) && ( emailAlert ))
					{
						handleSendErrorEmail(SendErrorEmailEvent::ErrorEmailTypeGcdFtpError, emailAlert, pErrorEvent->getErrorMessage());
						delete (emailAlert);
					}
					break;
				}
				case SendGcdPlaylistErrorEmail:
				{
					SendGcdPlaylistErrorEmailEvent* pErrorEvent =  dynamic_cast<SendGcdPlaylistErrorEmailEvent*> (pEvent);
					HobEmailAlert *emailAlert = HobEmailAlert::findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeGcdPlaylistError, this);
					
					if (( pErrorEvent ) && ( emailAlert ))
					{
						handleSendErrorEmail(SendErrorEmailEvent::ErrorEmailTypeGcdPlaylistError, emailAlert, pErrorEvent->getErrorMessage());
						delete (emailAlert);
					}
					break;
				}
				case SendErrorEmail:
				{
					Debug::trace("EmailManager","Processing error email\n");

					SendErrorEmailEvent* pErrorEvent =  dynamic_cast<SendErrorEmailEvent*> (pEvent);
					HobEmailAlert *emailAlert = NULL;

					if ( pErrorEvent )
						emailAlert = HobEmailAlert::findEmailAlert((HobRecordNum)pErrorEvent->getCode(), this);

					if (( pErrorEvent ) && ( emailAlert ))
					{
						if (emailAlert->getErrorCode() == RECORD_NOT_FOUND)
						{
							dbgmsg = "DB find failed. Did not find the error code =";
							dbgmsg += Integer::toString (pErrorEvent->getCode());

							Debug::trace("EmailManager", dbgmsg);

							delete emailAlert;
							//return ERROR_CODE;
						}

						HobString empty_str("");
						handleSendErrorEmail(pErrorEvent->getType(), emailAlert, empty_str);

						emailAlert->setFailureSent(true);
						emailAlert->saveRecord();
						delete emailAlert;
					}
					break;
				}
				default:
				{
					break;
				}
			}


		}
	}

	//return OK;
}


void EmailManager::handleSendErrorEmail(SendErrorEmailEvent::ErrorEmailType eventType, HobEmailAlert* pAlert, HobString& msg)
{
	Configuration * pConfig = Configuration::getInstance();
	std::string toList(pConfig->getToList());
	std::string ccList("");
	std::string bccList("");
	std::string fromEmailAddr(pConfig->getFrom());
	std::string scaleName(pConfig->getDnsHostName());

	std::string ipAddressString;
	std::string gatewayAddressString;
	pConfig->getIPAndGatewayAddresses(ipAddressString, gatewayAddressString);

	std::string subject, body;

	switch ( eventType )
	{
		case SendErrorEmailEvent::ErrorEmailTypeError:
			subject.append("Problem '");
			break;

		case SendErrorEmailEvent::ErrorEmailTypeResolved:
			subject.append("Resolved '");
			break;

		default:
			subject.append("'");
			break;
	}

	subject.append (pAlert->getSubject());

	body = subject + "'.";
	body += "\n";
	body += msg;

	subject.append("' in scale ").append(scaleName).append(" (IP Address ").append(ipAddressString).append(")");

	if (toList != "")
	{
		Email* pEmail = new Email(fromEmailAddr, toList, ccList, bccList, subject, body);

		Debug::trace("EmailManager","Sending email to SMTP client\n");
		outMailer_->send(pEmail);
	}

	if ( pAlert->getNotifyHandsFreeDeviceOnFailure() || pAlert->getNotifyHandsFreeDeviceOnResolution() )
	{
		string hfMsg = (eventType == SendErrorEmailEvent::ErrorEmailTypeError) ? "P:" : "R:";
		hfMsg.append(pAlert->getSubject());
		GenericHfcObject gho(hfMsg.substr(0, 16));
		SessionMessage hfdMsg(HandsFreeDevice, HFDSendMessage);
		hfdMsg.setFormatType(InternalFormat);
		hfdMsg.setHfcObject(&gho);
		const char *aaa = hfdMsg.serialize();
		UINT32 bbb = hfdMsg.getSerializedLength();
		dispatchEvent(aaa, bbb);
	}
}

STATUS EmailManager::handlePrinterStatus(PrinterStatus& newPrinterStatus)
{
	{
		bool outOfLables = newPrinterStatus.isOutOfMedia() | newPrinterStatus.isOutOfStock();
		bool labelsSized = newPrinterStatus.isSizeCmdComplete();

		if (outOfLables)
		{
			SendErrorEmailEvent evt(SendErrorEmailEvent::ErrorEmailTypeError, SendErrorEmailEvent::ErrorEmailCodeOutOfLabels);
			addANewErrorEvent(evt);
		}
		else if (labelsSized)
		{
			SendErrorEmailEvent evt(SendErrorEmailEvent::ErrorEmailTypeResolved, SendErrorEmailEvent::ErrorEmailCodeOutOfLabels);
			addANewErrorEvent(evt);
		}
	}

	{
		bool headDisengaged = newPrinterStatus.isHeadDisengaged();

		SendErrorEmailEvent evt(headDisengaged ? SendErrorEmailEvent::ErrorEmailTypeError : SendErrorEmailEvent::ErrorEmailTypeResolved,
		                        SendErrorEmailEvent::ErrorEmailCodeCassetteOpen);

		addANewErrorEvent(evt);
	}

	if ( Configuration::getInstance()->isSelfServiceScale() )
	{
		bool labelPresent = newPrinterStatus.isTakeLabelError();

		SendErrorEmailEvent evt(labelPresent ? SendErrorEmailEvent::ErrorEmailTypeError : SendErrorEmailEvent::ErrorEmailTypeResolved,
		                        SendErrorEmailEvent::ErrorEmailCodeLabelPresent);

		addANewErrorEvent(evt);
	}

	return OK;
}

void EmailManager::handleMessage(SessionMessage *msg, Session *UNUSED(pSession))
{
	UNREFERENCED_PARAMETER(pSession);

	if (!msg)
		return;

	switch (msg->getDeviceType())
	{
		case NetworkConnectivityDev:

			switch (msg->getEventType())
			{
				case GoodComm:
				{
					Debug::trace("Session.NetworkConnectivityMgr", "received goodComm message");
					SendErrorEmailEvent evt(SendErrorEmailEvent::ErrorEmailTypeResolved, SendErrorEmailEvent::ErrorEmailCodeNetworkOffline);
					addANewErrorEvent(evt);
					break;
				}
				case BadComm:
				{
					Debug::trace("Session.NetworkConnectivityMgr", "received badComm message");
					SendErrorEmailEvent evt(SendErrorEmailEvent::ErrorEmailTypeError, SendErrorEmailEvent::ErrorEmailCodeNetworkOffline);
					addANewErrorEvent(evt);
					break;
				}
				default:
					Debug::trace("EmailManager", "handleMessage(): No method for event " + HobString(Integer::toString(msg->getEventType())));
					break;
			}
			break;

		default:
			Debug::trace("EmailManager", "handleMessage(): No method for device " + HobString(Integer::toString(msg->getDeviceType())));
			break;
	}
}
