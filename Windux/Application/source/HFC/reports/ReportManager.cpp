/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ReportManager.h"
#include "ReportEvent.h"

#include "MultiLanguageText.h"
#include "SessionManager.h"
#include "Session.h"

//#include "NetworkErrorReport.h"
#include "ConfigurationReport.h"
#include "GenericTextReport.h"
#include "WrapperTotalsReport.h"
#include "WrapperSettingsReport.h"
#include "ProductRunTotalsReport.h"
#include "WeigherAuditTrailReport.h"
#include "OperatorChangesReport.h"
#include "SupervisorChangesReport.h"

#include "SessionMessage.h"
#include "ReportTerminatedMessage.h"
#include "HfcStartReport.h"

#include "DBManager.h"

#include "Configuration.h"
#include "HobList.h"
#include "Long.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ReportManager::dataType = 0;
std::string ReportManager::dataTypeName("ReportManager"); 
#endif

using namespace std;



ReportManager::ReportManager(SessionMessage* msg)
	: thread_(0), msgManager_(0), report_(0), state_(IdleState), event_(UnknownReportEvent),
	reportType_(UnknownReportType), printerType_(FirstPrinter), requestMessage_(0),cutTestNumber_(0),cutTestReportType_(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ReportManager", 20, "Constructing the ReportManager");


	if ( msg )
	{
		event_ = (ReportEventType)msg->getEventType();

		HfcStartReport* hfcMsg = dynamic_cast<HfcStartReport*>(msg->getHfcObject());
		if ( hfcMsg )
		{
			// Make a copy of the original message to pass it on to the
			// corresponding report in case there is additional data in it.
			requestMessage_ = new HfcStartReport(*hfcMsg);

			reportType_        = requestMessage_->getReportType();
			reportName_        = requestMessage_->getReportTitle();
			reportContent_     = requestMessage_->getReportContent();
			printerType_       = requestMessage_->getPrinterType();
			cutTestNumber_     = requestMessage_->getCutTestNumber();
			cutTestReportType_ = requestMessage_->getCutTestReportType(); 

			Debug::trace("ReportManager", 24, "Event Type passed to constructor is " +
					inttostring(event_) + ". Report type is " + inttostring(reportType_) +
					" and printer type is " + inttostring(printerType_) + 
					" and cut test number is " + inttostring(cutTestNumber_) +
					" and cut test report type is " + inttostring(cutTestReportType_) );
		}
		else
		{
			Debug::trace("ReportManager", 10, string("No HfcStartReport was passed in!!??") +
					"    Event Type passed to constructor is " + inttostring(event_) +
					" and message's HFC object is " + ptrtostring(msg->getHfcObject()));
		}
	}
	else
		Debug::trace("ReportManager", 5, "No message was passed into the constructor!!??");

	msgManager_ = new Hobart::CEvolMsgManager(this, "Report Manager");
	thread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, "ReportManager");

	//Register for CancelReport with the Event Manager 
	registerEvent(ReportManagerDev, CancelReport);
}


ReportManager::~ReportManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	Debug::trace("ReportManager", 20, "Destroying the ReportManager");

	if (thread_)
	{
		thread_->StopThread();
		thread_->WaitForCompletion();
	}

	if ( report_ )
	{
		Debug::trace("ReportManager", 24, "Destructor: report_ is not null (" +
				ptrtostring(report_) + "). Going to delete it.");

		delete report_;
	}

	// deregister for all events
	deregisterAllEvents();

	if ( requestMessage_ )
		delete requestMessage_;

	if ( msgManager_ )
		delete msgManager_;


	Debug::trace("ReportManager", 24, "Leaving destructor.");
}


void ReportManager::init()
{
	Debug::trace("ReportManager", 20, "Initializing the ReportManager");


	// Set the internationalized text to print the report
	Session* priSession = SessionManager::getInstance()->getPrimaryConsoleSession();
	hfc::MultiLanguageText::setLanguageSelection(priSession ? priSession->getCurrentLanguageSelection() : PrimaryLanguage);


	// Now process the event passed into the constructor
	if ( event_ == StartReport )
	{
		switch ( reportType_ )
		{
/*			case NetworkErrorsReportType:
			{
#ifndef EVOL_NDEBUG
				Debug::trace("ReportManager", 10, "Received request to start Network Errors report.");
#endif

				report_ = new NetworkErrorReport();

				//	Get the number of Error records in the database

				state_ = ReportingState;

			 	break;
			} */
			case ConfigurationReportType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Configuration report.");

				report_ = new ConfigurationReport(this, printerType_);

				state_ = ReportingState;

			 	break;
			}
			case GenericTextReportType:
			{
#ifndef EVOL_NDEBUG
				Debug::trace("ReportManager", 10, "Received request to start GenericText report.");
#endif

				report_ = new GenericTextReport(reportName_, reportContent_, this, printerType_);

				state_ = ReportingState;

			 	break;
			}
			case WrapperTotalsReportType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Wrapper Totals report.");

				report_ = new WrapperTotalsReport(this, printerType_);

				state_ = ReportingState;

				break;
			}
			case WrapperSettingsReportType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Wrapper Settings report.");

				report_ = new WrapperSettingsReport(requestMessage_, this, printerType_);

				state_ = ReportingState;

				break;
			}
			case RunTotalsReportType:
			{
				// Prints the Run Totals Report
				Debug::trace("ReportManager", "case RunTotalsReportType:  Received request to start ProductRunTotalsReport.");

				Session* session = SessionManager::getInstance()->getPrimaryConsoleSession();
				if (session)
                {
				    Debug::trace("ReportManager", "case RunTotalsReportType: IF condition");
					report_ = new ProductRunTotalsReport(this, printerType_, session, cutTestNumber_, cutTestReportType_);
                }
                else
                {
				    Debug::trace("ReportManager", "case RunTotalsReportType:  ELSE condition");
                }
				
				state_ = ReportingState;
				break;			
			}
			case WeigherAuditTrailType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Weigher Audit Trail report.");

				report_ = new WeigherAuditTrailReport(this, printerType_);

				state_ = ReportingState;

			 	break;
			}
			case OperatorChangesReportType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Operator Changes Report.");

				report_ = new OperatorChangesReport(this, printerType_);

				state_ = ReportingState;

				break;
			}
			case SupervisorChangesReportType:
			{
				Debug::trace("ReportManager", 10, "Received request to start Supervisor Changes Report.");

				report_ = new SupervisorChangesReport(this, printerType_);

				state_ = ReportingState;

				break;
			}
	 		default:
			{
				Debug::trace("ReportManager", 10, "Unknown report type " +
						inttostring(reportType_) + ". Cannot start report.");

				state_ = DoneState;

				break;
			}
		}

		if ( report_ )
			report_->initialize();
	}
	else
	{
		Debug::trace("ReportManager", 5, "Unknown/unexpected event type received in the constructor.");

		state_ = DoneState;
	}


	// Now that we have initialized, start processing of events
	msgManager_->RestartThread();
} 


DWORD ReportManager::Run()
{
	bool looping = true;

	//Issue #3642 - Give event processing thread time to come up before proceeding
	PAL_absclass::PAL_sleep(10);
	
	init();

	while ( looping )
	{
		switch ( state_ )
		{
			case ReportingState:
			{
				if ( report_ )
				{
					if ( report_->doReport() )
					{
						Debug::trace("ReportManager", 20, "Report finished. State = DoneState");

						state_ = DoneState;
					}
				}
				else
				{
					Debug::trace("ReportManager", 10, "Reporting but no report. State = DoneState");

					state_ = DoneState;
				}

				break;	 	
			}
			case CancelState:
			{
				handleCancel();

				Debug::trace("ReportManager", 10, "Report cancelled. State_ = CancelState");

//				state_ = DoneState;
				looping = false;

				break;
			}
			case DoneState:
			{
				handleDone();

				Debug::trace("ReportManager", 24, "Report complete. State = DoneState");

//				state_ = IdleState;
				looping = false;

				break;
			}
			case IdleState:
			default:
			{
                PAL_absclass::PAL_sleep(1000);

				break;
			}
		}

		PAL_absclass::PAL_sleep(100);
	}

	return 0;
}


void ReportManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg )
{
	Debug::trace("ReportManager.event", 24, "Received a message through the message queue");

	EvolMessage evolMessage(msg->Data());
	if ( evolMessage.getDeviceType() == ReportManagerDev )
	{
		switch ( evolMessage.getEventType() )
		{
			case CancelReport:
			{
				Debug::trace("ReportManager.event", 10, "Message: cancel the report. Cancelling ...");

				state_ = CancelState;
				break;
			}
			default:
			{
				Debug::trace("ReportManager.event", 10, "Message: unknown ReportManager message");

				break;
			}
		}	 
	}
	else
	{
		if ( state_ == ReportingState && report_ )
		{
			Debug::trace("ReportManager.event", 24,
					"Message: non-ReportManager message. Passing onto report");
			report_->handleMessage(msg->Data());
		}
		else
		{
			Debug::trace("ReportManager.event", 10,
					"Message: non-ReportManager message. Currently not reporting. Dropped message");
		}
	}
}


void ReportManager::handleCancel()
{
	Debug::trace("ReportManager", 24, "Cancelling report ...");

	if ( report_ )
		report_->cancelReport();

/*	SessionMessage done(ReportManagerDev, TerminateReport);
	done.setFormatType(InternalFormat);
	session_->send(&done, Messenger::DEST_RTC); */
	ReportTerminatedMessage message(true);
	msgManager_->dispatchEvent(&message);
}


void ReportManager::handleDone()
{
	Debug::trace("ReportManager", 24, 
			"Wrapping up completion of report. Sending message to my Session to terminate me ...");	

	// Tell session that we are done and have session tell the UI that we are done
/*	SessionMessage done(ReportManagerDev, TerminateReport);
	done.setFormatType(BrowserFormat);
	session_->send(&done, Messenger::DEST_UI);

	done.setFormatType(InternalFormat);
	session_->send(&done, Messenger::DEST_RTC); */
	ReportTerminatedMessage message(false);
	msgManager_->dispatchEvent(&message);
}


