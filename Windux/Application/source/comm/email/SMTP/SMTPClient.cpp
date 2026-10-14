#include "PAL_platformheader.h"

#include "SMTPClient.h"



#include "../Email.h"
#include "StateMachine/StateMachine.h"
#include "Debug.h"
#include "Integer.h"



using namespace std;

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SMTPClient::_dataType_ = 0;
	std::string SMTPClient::_dataTypeName_("SMTPClient"); 
#endif

const std::string SMTPClient::_ThreadName_("tSMTPClient");
const int SMTPClient::_ThreadPriority_ = 125;				//needs to be 120 or greater. causes awt task crash at 100.
const int SMTPClient::_ThreadStackSize_ = 20 * 1024;        // In bytes
const int SMTPClient::_ThreadDelay_ = 2 * 1000;             // In milliseconds
const int SMTPClient::_DefaultSMTPServerPort_ = 25;         // SMTP well-known port
const int SMTPClient::_MaxRetryTime_ = 15 * 60;              // In Seconds (15 minutes)
const int SMTPClient::_RetryDelay_ = 30;                    //in ticks
const int SMTPClient::_idleDelay_ = 10;                    //in ticks
const int SMTPClient::_retryDelay_ = 5;                    //in ticks



SMTPClient::SMTPClient(const SMTPClientInterface& interface0 )
		: interface_(interface0),
		serverName_(""), serverIP_(""), serverPort_(_DefaultSMTPServerPort_),
		serverParametersMutex_(), pending_(), pendingMutex_(), fsm_(0), sock_(0),
		responseTime_() , currentEmail_(0), leaveState_(false), firstTime_(true), oneSuceeded_(false),
		oneFailed_(false), currentEmailAddr_("") 
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	checkingThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, _ThreadName_.c_str() );			
}


SMTPClient::~SMTPClient()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	if (checkingThread)
	{
		checkingThread->StopThread();
		checkingThread->WaitForCompletion();
	} 

	if ( fsm_ ) 
		delete fsm_; 

	if ( sock_ ) 
		delete sock_;
}


/* void SMTPClient::debug(const std::string& category, int level, const std::string& message) const
{
}
*/

DWORD SMTPClient::Run()
{
	// Initialize and setup the State Machine
	fsm_ = new StateMachine<SMTPClient>(*this); 
	setupStateMachine(); 

	fsm_->start(new SMTPClientEvent(SMTPClientEvent::START_EVENT), IDLING_STATE);
                      
	fsm_->runStateMachine();

	if ( (*fsm_)() != stateMachine_type::STOPPED_STATE )
	{
	}
	else
	{
		if ( pending_.size() )
		{
		}
		else
		{
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep(_ThreadDelay_);
#else
			Sleep(_ThreadDelay_);
#endif      // LINUX_FIX
		}
	}

	return 0;
}


bool SMTPClient::enterIdlingState(const SMTPClientEvent&)
{

	if (currentEmail_ != 0)
	{
		delete currentEmail_;
		currentEmail_ = 0;
	}


	return true;
}


bool SMTPClient::idlingState(const SMTPClientEvent&)
{
	if ( sizePendingEmails() )
	{
		Debug::trace("Email.SMTP", "Received new email");
		currentEmail_ = getCurrentEmail();

		fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::EMAIL_AVAILABLE_EVENT));

		return true;
	}
	else
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep(_idleDelay_*10);
#else
		Sleep(_idleDelay_*10);
#endif      // LINUX_FIX

	return false;
}

bool SMTPClient::enteringConnectingState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "enteringConnectingState");

	if (sock_ != NULL)
	{
		Debug::trace("Email.SMTP", "Closing and deleting old socket");
		sock_->cmClose();
		delete sock_;
		sock_ = NULL;
	}

	sock_ = new Socket();

	if (sock_ == NULL)
	{
		Debug::trace("Email.SMTP", "Failed to create new socket");

		leaveState_ = true;
		return true;
	}
  	responseTime_.getCurrentDateTime();

	string debugmsg ("Setting up socket ip:");
	
	debugmsg += serverIP_;
	debugmsg += " port:";
	debugmsg += Integer::toString(serverPort_);

	Debug::trace("Email.SMTP", debugmsg);

	sock_->setPort(serverPort_);
	sock_->setDnsName(serverIP_);

	sock_->sConnect();

#ifdef LINUX_FIX
        PAL_absclass::PAL_sleep(100);
#else
	Sleep(100);
#endif      // LINUX_FIX

	if (sock_->isValid())
	{
		Debug::trace("Email.SMTP", "Socket is valid");
		leaveState_ = false;
		sock_->setNonBlockingIO();
	}
	else
	{
		Debug::trace("Email.SMTP", "Socket is invalid");
		leaveState_ = true;
	}

	return true;
}


bool SMTPClient::processingConnectingState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingConnectingState");

	if (leaveState_)
	{
		fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::CONNECT_FAILED_EVENT));
		return true;
	}

	complete = getResponse(result, string("220") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::CONNECT_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::CONNECT_FAILED_EVENT));
			break;
#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
	}

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}

bool SMTPClient::enteringSendingHeloState(const SMTPClientEvent&)
{
string sendMsg("helo test\r\n");
const char* cStrPtr = 0;
char responseMSG[1024];
int bytesRecvd = 0;

	Debug::trace("Email.SMTP", "enteringSendingHeloState");
	Debug::trace("Email.SMTP", sendMsg);

	cStrPtr = sendMsg.c_str();

  	responseTime_.getCurrentDateTime();
      
	if (sock_->cmNumMsgs() > 0)
		bytesRecvd = sock_->receive(responseMSG,sizeof(responseMSG)); //clear out receive buffer.

	sock_->cmSend(cStrPtr, (int)sendMsg.length());

	return true;
}

bool SMTPClient::processingSendingHeloState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingHeloState");

	complete = getResponse(result, string("250") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::HELO_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::HELO_FAILED_EVENT));
			break;

#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
        }

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}

bool SMTPClient::enteringSendingMailState(const SMTPClientEvent&)
{
string sendMsg("mail from:<");
const char* cStrPtr = 0;
char responseMSG[1024];
int bytesRecvd = 0;

	Debug::trace("Email.SMTP", "enteringSendingMailState");

	sendMsg += currentEmail_->getFrom();
	sendMsg += ">\r\n";
	cStrPtr = sendMsg.c_str();

	Debug::trace("Email.SMTP", sendMsg);

  	responseTime_.getCurrentDateTime();
      
	if (sock_->cmNumMsgs() > 0)
		bytesRecvd = sock_->receive(responseMSG,sizeof(responseMSG)); //clear out receive buffer.

	sock_->cmSend(cStrPtr, (int)sendMsg.length());

	return true;
}

bool SMTPClient::processingSendingMailState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingMailState");

	complete = getResponse(result, string("250") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::MAIL_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::MAIL_FAILED_EVENT));
			break;

#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
	}

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}

bool SMTPClient::getResponse(SMTPClient::responseCode& result, string responseCode, int timeOutSecs)
{
char responseMsg[1024];
unsigned int bytesRecvd = 0;


	if (sock_->cmNumMsgs() > 0)
	{
		bytesRecvd = sock_->receive(responseMsg, sizeof(responseMsg));

		if ((bytesRecvd > 0) && (strncmp(responseMsg, responseCode.c_str(), 3) == 0))
			result = SUCCESS_CODE;
		else
			result = FAILURE_CODE;

		return true;

	}
	else
	{
		DateTime now = DateTime();
		if ( (now - responseTime_).getTotalSeconds() >= timeOutSecs )
		{
			result = TIME_OUT;
			return true;
		}
		else
		{
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep(_RetryDelay_*10); //wait a bit of time before checking for response again
#else
			Sleep(_RetryDelay_*10); //wait a bit of time before checking for response again
#endif      // LINUX_FIX
		}
			
	}
	result = NOT_COMPLETE;

	return false;
}


bool SMTPClient::enteringSendingToState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "enteringSendingToState");

	emailList_ = currentEmail_->getToList();
	firstTime_ = true;
	oneSuceeded_ = false;
	oneFailed_ = false;

	if (emailList_->size() > 0)
	{
		leaveState_ = false;
		listIter_ = emailList_->begin();
		currentEmailAddr_ = *listIter_;

		Debug::trace("Email.SMTP", currentEmailAddr_);
	}
	else
		leaveState_ = true;

	return true;

}

bool SMTPClient::enteringSendingCCState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "enteringSendingCCState");

	emailList_ = currentEmail_->getCcList();
	firstTime_ = true;
	oneSuceeded_ = false;
	oneFailed_ = false;

	if (emailList_->size() > 0)
	{
		leaveState_ = false;
		listIter_ = emailList_->begin();
		currentEmailAddr_ = *listIter_;

		Debug::trace("Email.SMTP", currentEmailAddr_);
	}
	else
		leaveState_ = true;

	return true;

}

bool SMTPClient::enteringSendingBCCState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "enteringSendingBCCState");

	emailList_ = currentEmail_->getBccList();
	firstTime_ = true;
	oneSuceeded_ = false;
	oneFailed_ = false;

	if (emailList_->size() > 0)
	{
		leaveState_ = false;
		listIter_ = emailList_->begin();
		currentEmailAddr_ = *listIter_;

		Debug::trace("Email.SMTP", currentEmailAddr_);
	}
	else
		leaveState_ = true;

	return true;

}

bool SMTPClient::processingSendingRcptState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingRcptState");

	if (leaveState_)
	{
		fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::RCPT_ALL_FAILED_EVENT));
		return true;
	}
	
	if (firstTime_)
	{
		firstTime_ = false;
		string sendMsg("rcpt to:<");
		const char* cStrPtr = 0;
		char responseMSG[1024];
		int bytesRecvd = 0;

		sendMsg += currentEmailAddr_;
		sendMsg += ">\r\n";
		cStrPtr = sendMsg.c_str();

		Debug::trace("Email.SMTP", sendMsg);

		if (sock_->cmNumMsgs() > 0)
			bytesRecvd = sock_->receive(responseMSG,sizeof(responseMSG)); //clear out receive buffer.

		sock_->cmSend(cStrPtr, (int)sendMsg.length());

	}
	else
	{
		bool currentEmailComplete = getResponse(result, string("250") , 20);

		switch (result)
		{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			goodList_.push_back(currentEmailAddr_);
			oneSuceeded_ = true;
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			oneFailed_ = true;
			break;
#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
		}

		if (currentEmailComplete) //advance to next email address in list
		{
			++listIter_;
			if (listIter_ == emailList_->end())
				complete = true;
			else
				currentEmailAddr_ = *listIter_;
			firstTime_ = true;
		}
		else
		{
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
			Sleep (_retryDelay_*10);
#endif      // LINUX_FIX
		}

	}


	if (complete)
	{
		if (oneSuceeded_)
		{
			Debug::trace("Email.SMTP", "At least one destination succeeded");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::RCPT_ONE_SUCCEEDED_EVENT));
		}
		else
		{
			Debug::trace("Email.SMTP", "All destinations failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::RCPT_ALL_FAILED_EVENT));
		}
	}

	return complete;
}

bool SMTPClient::leavingSendingToState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "leavingSendingToState");

	if (oneFailed_)
		currentEmail_->setToList(goodList_);

	goodList_.clear();

	return true;
}

bool SMTPClient::leavingSendingCCState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "leavingSendingCCState");
	if (oneFailed_)
		currentEmail_->setCcList(goodList_);

	goodList_.clear();

	return true;
}

bool SMTPClient::leavingSendingBCCState(const SMTPClientEvent&)
{
	Debug::trace("Email.SMTP", "leavingSendingBCCState");
	if (oneFailed_)
		currentEmail_->setBccList(goodList_);

	goodList_.clear();

	return true;
}

bool SMTPClient::enteringSendingDataState(const SMTPClientEvent&)
{
string sendMsg("data\r\n");
const char* cStrPtr = 0;
char responseMSG[1024];
int bytesRecvd = 0;

	Debug::trace("Email.SMTP", "enteringSendingDataState");

	Debug::trace("Email.SMTP", sendMsg);

	cStrPtr = sendMsg.c_str();

  	responseTime_.getCurrentDateTime();
      
	if (sock_->cmNumMsgs() > 0)
		bytesRecvd = sock_->receive(responseMSG,sizeof(responseMSG)); //clear out receive buffer.

	sock_->cmSend(cStrPtr, (int)sendMsg.length());

	return true;
}

bool SMTPClient::processingSendingDataState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingDataState");

	complete = getResponse(result, string("354") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::DATA_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::DATA_FAILED_EVENT));
			break;

#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
	}

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}


bool SMTPClient::enteringSendingBodyState(const SMTPClientEvent&)
{
string emailText;
const char* cStrPtr = 0;
char responseMSG[1024];
int bytesRecvd = 0;

	Debug::trace("Email.SMTP", "enteringSendingBodyState");

	currentEmail_->buildSMTPEmail(emailText);

	Debug::trace("Email.SMTP", emailText);

	cStrPtr = emailText.c_str();

  	responseTime_.getCurrentDateTime();
      
	if (sock_->cmNumMsgs() > 0)
		bytesRecvd = sock_->receive(responseMSG, sizeof(responseMSG)); //clear out receive buffer.

	sock_->cmSend(cStrPtr, (int)emailText.length());

	return true;
}

bool SMTPClient::processingSendingBodyState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingBodyState");

	complete = getResponse(result, string("250") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::BODY_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::BODY_FAILED_EVENT));
			break;

#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
	}

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}

bool SMTPClient::enteringSendingQuitState(const SMTPClientEvent&)
{
string sendMsg("quit\r\n");
const char* cStrPtr = 0;
char responseMSG[1024];
int bytesRecvd = 0;

	Debug::trace("Email.SMTP", "enteringSendingQuitState");

	cStrPtr = sendMsg.c_str();

  	responseTime_.getCurrentDateTime();
      
	if (sock_->cmNumMsgs() > 0)
		bytesRecvd = sock_->receive(responseMSG, sizeof(responseMSG)); //clear out receive buffer.

	sock_->cmSend(cStrPtr, (int)sendMsg.length());

	return true;
}

bool SMTPClient::processingSendingQuitState(const SMTPClientEvent&)
{
bool complete = false;
SMTPClient::responseCode result;

	Debug::trace("Email.SMTP", "processingSendingQuitState");

	complete = getResponse(result, string("221") , 20);

	switch (result)
	{
		case SUCCESS_CODE:
			Debug::trace("Email.SMTP", "Received the correct response back");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::QUIT_OK_EVENT));
			break;
		case FAILURE_CODE:
		case TIME_OUT:
			Debug::trace("Email.SMTP", "Response failed");
			fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::QUIT_FAILED_EVENT));
			break;

#ifdef LINUX_FIX
                default:
                        break;
#endif      // LINUX_FIX
	}

	if (!complete)
#ifdef LINUX_FIX
		PAL_absclass::PAL_sleep (_retryDelay_*10);
#else
		Sleep (_retryDelay_*10);
#endif      // LINUX_FIX

	return complete;

}

bool SMTPClient::rejectingState(const SMTPClientEvent&)
{

	Debug::trace("Email.SMTP", "rejectingState");

	fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::REJECTED_OK_EVENT));

	return true;
}

bool SMTPClient::retryingState(const SMTPClientEvent&)
{
	unsigned short retries = currentEmail_->getNumRetries();

	Debug::trace("Email.SMTP", "retryingState");

	if (retries > 0)
	{
		string debugmsg ("Retries remaining now ");

		debugmsg += Integer::toString(retries - 1);

		Debug::trace("Email.SMTP", debugmsg);

		currentEmail_->decrementRetries();

		Debug::trace("Email.SMTP", "Sleep for a while before retrying");
#ifdef LINUX_FIX
                PAL_absclass::PAL_sleep (_MaxRetryTime_*10);
#else
		Sleep (_MaxRetryTime_*10);
#endif      // LINUX_FIX

		fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::RETRIES_OK_EVENT));

	}
	else
	{
		Debug::trace("Email.SMTP", "Retries expended, reject the email");
		fsm_->postEvent(new SMTPClientEvent(SMTPClientEvent::TOO_MANY_RETRIES_EVENT));
	}


	return true;
}


void SMTPClient::setupStateMachine()
{
	// Add states
	fsm_->addState(IDLING_STATE, &SMTPClient::idlingState, &SMTPClient::enterIdlingState);
	fsm_->addState(CONNECTING_STATE, &SMTPClient::processingConnectingState, &SMTPClient::enteringConnectingState);
	fsm_->addState(SENDING_HELO_STATE, &SMTPClient::processingSendingHeloState, &SMTPClient::enteringSendingHeloState);
	fsm_->addState(SENDING_MAIL_STATE, &SMTPClient::processingSendingMailState, &SMTPClient::enteringSendingMailState);
	fsm_->addState(SENDING_TO_STATE, &SMTPClient::processingSendingRcptState, &SMTPClient::enteringSendingToState, &SMTPClient::leavingSendingToState);
	fsm_->addState(SENDING_CC_STATE, &SMTPClient::processingSendingRcptState, &SMTPClient::enteringSendingCCState, &SMTPClient::leavingSendingCCState);
	fsm_->addState(SENDING_BCC_STATE, &SMTPClient::processingSendingRcptState, &SMTPClient::enteringSendingBCCState, &SMTPClient::leavingSendingBCCState);
	fsm_->addState(SENDING_DATA_STATE, &SMTPClient::processingSendingDataState, &SMTPClient::enteringSendingDataState);
	fsm_->addState(SENDING_BODY_STATE, &SMTPClient::processingSendingBodyState, &SMTPClient::enteringSendingBodyState);
	fsm_->addState(SENDING_QUIT_STATE, &SMTPClient::processingSendingQuitState, &SMTPClient::enteringSendingQuitState);
	fsm_->addState(SETTINGUP_RETRIES_STATE, &SMTPClient::retryingState);
	fsm_->addState(REJECTING_EMAIL_STATE, &SMTPClient::rejectingState);

	// Add IDLING_STATE transitions
	fsm_->addTransition(IDLING_STATE, SMTPClientEvent::END_EVENT);
	fsm_->addTransition(IDLING_STATE, SMTPClientEvent::EMAIL_AVAILABLE_EVENT, CONNECTING_STATE);

	// Add CONNECTING_STATE transitions
	fsm_->addTransition(CONNECTING_STATE, SMTPClientEvent::CONNECT_OK_EVENT, SENDING_HELO_STATE);
	fsm_->addTransition(CONNECTING_STATE, SMTPClientEvent::CONNECT_FAILED_EVENT, SETTINGUP_RETRIES_STATE);

	// Add SENDING_HELO_STATE transitions
	fsm_->addTransition(SENDING_HELO_STATE, SMTPClientEvent::HELO_OK_EVENT, SENDING_MAIL_STATE);
	fsm_->addTransition(SENDING_HELO_STATE, SMTPClientEvent::HELO_FAILED_EVENT, SETTINGUP_RETRIES_STATE);

	// Add SENDING_MAIL_STATE transitions
	fsm_->addTransition(SENDING_MAIL_STATE, SMTPClientEvent::MAIL_OK_EVENT, SENDING_TO_STATE);
	fsm_->addTransition(SENDING_MAIL_STATE, SMTPClientEvent::MAIL_FAILED_EVENT, REJECTING_EMAIL_STATE);

	// Add SENDING_TO_STATE transitions
	fsm_->addTransition(SENDING_TO_STATE, SMTPClientEvent::RCPT_ONE_SUCCEEDED_EVENT, SENDING_CC_STATE);
	fsm_->addTransition(SENDING_TO_STATE, SMTPClientEvent::RCPT_ALL_FAILED_EVENT, REJECTING_EMAIL_STATE);

	// Add SENDING_CC_STATE transitions
	fsm_->addTransition(SENDING_CC_STATE, SMTPClientEvent::RCPT_ONE_SUCCEEDED_EVENT, SENDING_BCC_STATE);
	fsm_->addTransition(SENDING_CC_STATE, SMTPClientEvent::RCPT_ALL_FAILED_EVENT, SENDING_BCC_STATE);

	// Add SENDING_BCC_STATE transitions
	fsm_->addTransition(SENDING_BCC_STATE, SMTPClientEvent::RCPT_ONE_SUCCEEDED_EVENT, SENDING_DATA_STATE);
	fsm_->addTransition(SENDING_BCC_STATE, SMTPClientEvent::RCPT_ALL_FAILED_EVENT, SENDING_DATA_STATE);

	// Add SENDING_DATA_STATE transitions
	fsm_->addTransition(SENDING_DATA_STATE, SMTPClientEvent::DATA_OK_EVENT, SENDING_BODY_STATE);
	fsm_->addTransition(SENDING_DATA_STATE, SMTPClientEvent::DATA_FAILED_EVENT, SETTINGUP_RETRIES_STATE);

	// Add SENDING_BODY_STATE transitions
	fsm_->addTransition(SENDING_BODY_STATE, SMTPClientEvent::BODY_OK_EVENT, SENDING_QUIT_STATE);
	fsm_->addTransition(SENDING_BODY_STATE, SMTPClientEvent::BODY_FAILED_EVENT, SETTINGUP_RETRIES_STATE);

	// Add SENDING_QUIT_STATE transitions
	fsm_->addTransition(SENDING_QUIT_STATE, SMTPClientEvent::QUIT_OK_EVENT, IDLING_STATE);
	fsm_->addTransition(SENDING_QUIT_STATE, SMTPClientEvent::QUIT_FAILED_EVENT, IDLING_STATE);

	// Add SETTINGUP_RETRIES_STATE transitions
	fsm_->addTransition(SETTINGUP_RETRIES_STATE, SMTPClientEvent::RETRIES_OK_EVENT, CONNECTING_STATE);
	fsm_->addTransition(SETTINGUP_RETRIES_STATE, SMTPClientEvent::TOO_MANY_RETRIES_EVENT, REJECTING_EMAIL_STATE);

	// Add REJECTING_EMAIL_STATE transitions
	fsm_->addTransition(REJECTING_EMAIL_STATE, SMTPClientEvent::REJECTED_OK_EVENT, IDLING_STATE);
}


