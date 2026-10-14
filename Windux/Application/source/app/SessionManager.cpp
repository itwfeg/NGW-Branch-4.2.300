/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SessionManager.h"
#include "SessionManagerHelper.h"

#include "MetSig.h"
#include "MemShow.h"
#include "Session.h"
#include "PrimaryCommSession.h"
#include "SecondaryCommSession.h"
#include "ResponseMessageManager.h"
#include "Configuration.h"
#include "ConfigurationEvent.h"
#include "CfgInternal.h"
#include "filesys/file.h"
#include "TimerManager.h"
#include "VersionManager.h"
#include "ProductDevEvent.h"
#include "Class.h"
#include "ProductDefault.h"
#include "TransactionTypeManager.h"
#include "GenericLabelText.h"
#include "PresentationMethod.h"
#include "Category.h"
#include "Special.h"
#include "DBAccess.h"
#include "DBManager.h"
#include "DBConfigurationsRM.h"
#include "WeigherEvent.h"
#include "HfcWeigherConfig.h"
#include "WeigherCat3DateTime.h"
#include "Dept.h"

#include "EmailManager.h"
#include "cgi/CGICallManager.h"
#include "cgi/CGIHTTPAdapter.h"
#include "NetworkConnectivityManager.h"
#include "NetworkConnectivityEvents.h"
#include "RFIDManager.h"
#include "RFIDEvent.h"
#include "RFIDStatus.h"
#include "ScannerManager.h"
#include "BarcodeScannerEvent.h"
#include "HobEmailAlert.h"
#include "Softkey.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"
#include "PeripheralManager.h"
#include "GenericHfcObject.h"
#include "TerminateMessage.h"
#include "GraphicManager.h"
#include "GraphicFileNames.h"
#include "BackupRestoreEvent.h"
#include "ConfigurationManager.h"
#include "DiagnosticsManager.h"

#include "PrimarySynchronizationManager.h"
#include "SecondarySynchronizationManager.h"
#include "Messages/SynchronizationDevEvent.h"
#include "CMReaderWriterSyncTag.h"
#include "CMReaderWriterHandsFreeDevice.h"

#include "MultiLanguageText.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"
#include "CMessageCenter.h"
#include "Messages/AddNewSessionMessage.h"
#include "Messages/RebootMessage.h"
#include "Messages/ChangeSynchronizationTypeMessage.h"
#include "Messages/ChangePrimaryScaleMessage.h"
#include "Messages/RemoveSessionStarterMessage.h"

#include "Messages/SynchronizationDevEvent.h"
#include "Messages/ComingUpOnLineMessage.h"
#include "Messages/GoingOffLineMessage.h"

#include "UpdateSoftwareAutomatically.h"

#include "MarqueeEvent.h"
#include "MarqueeGeneric.h"
#include "MarqueeUpdate.h"
#include "NumericEvent.h"
#include "NumericGeneric.h"

#include "ProductTextCfg.h"
#include "HfcScannerProductRead.h"

#include "CSmartPtr.h"
#include "Long.h"
#include "ULong.h"
#include "Integer.h"
#include "UInteger.h"
#include "Hex.h"
#include "Bool.h"

#include "GCDSession.h"
#include "GCDFtpUpdateMsg.h"

#include "HfcPeripheralList.h"

#include "UpdateSoftwareManually.h"
#include "filesys/directory.h"

//#include "TestWorkerThread.h"

#include "MessageLogger.h"
#include "RecallEvent.h"

#include "UsbManager.h"
#include "LabelApplierEvent.h"//Added 4/28/14 Alex Zavala

#include "PythonScripter.h"
#include "HobTokenizerCSV.h"
#include "SystemPrinters.h"

#include "Device.h"

#include "PrinterStockMetrics.h"

#include "FpmManager.h"
#include "FpmEvent.h"
#include "Weigher.h"
#include "WeigherWriteCat3Record.h"
#include "VmStatusMsg.h"

#include "HobTokenizerBar.h"
#include "HobSocket.h"
#include "bootParameters.h"

#include "BuildNumber.h"

#include <HobartCustomerHook.h>

#include "Wrapper.h"
#include "WrapperInquire.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

using namespace std;

#define DAYLIGHT_SAVINGS_FILE "C:\\PROGRAM FILES\\HOBART\\ACCESS SCALE\\DST.TXT"

const std::string SessionManager::_statusOKText_                                ("OK");
const std::string SessionManager::_statusUnknownSynchronizationTypeText_        ("Unknown synchronization type");
const std::string SessionManager::_statusNoSynchronizationManagerText_          ("No Synchronization Manager");
const std::string SessionManager::_statusCannotChangePrimaryIfNotSecondaryText_ ("Cannot change Primary if not a Secondary");
const std::string SessionManager::_statusUnknownText_                           ("Unknown");
const std::string SessionManager::_statusCouldNotInitializeSynchronizationManagerText_ ("There was a problem initializing the Synchronization Manager");

const std::string SessionManager::boardSiteLvdsFile_("/etc/hobart/boardsitelvds");

std::string corruptStr("_corrupt");

// static initializer
SessionManager *SessionManager::instance = 0;



SessionManager::SessionManager()
    : Hobart::CEvolMsgReceiver("SessionManager"),
	primaryConsoleSession(0), timerManager(0), 
	sentFirmwareUpdateMessage(false), sentGCDMediaUpdateMessage(false), dbIndDataExists(false), timeToDoDBCompress(false),
	downloadingFilesTimerId_(0), configDebugFileChecksum_(0), allowConnections(true), 
    shutdownStep(0), rebootRequest_(RebootRequest_NotReceived), requestToCompressDB_(false), requestToPerformAutoBackupDB_(false),
	requestToInitializeSynchronization_(false), requestToTerminateSynchronization_(false), requestToChangeSynchronizationType_(false), requestedSyncType_(ScaleSynchronizationTypeInvalid), requestToChangePrimaryScale_(false),
	bootupUpgradeCounter_(0), scaleOnline_(ScaleOnlineTypeUnknown), inputProcessingEnabled_(true), inputDisablingCondition_(0),
    synchronizationManager_(0), queuedPausers_(0),
    blockCommonThreadsEvent_(true, true), blockSuperThreadsEvent_(true, true), blockAllRequestsToPauseEvent_(true, true),
    blockPauseInitiatorAllEvent_(true, true), blockPauseInitiatorCommonEvent_(true, true),
    lastHourExpiredLabelsChecked(-1), usingEmptyDatabase(false), usingMirrorDatabase(false),
	gcdSession(0), createGCDSession(false), ctz_stat_on_file(-1), ctz_stat(-1), panicShutdown_(false), initiateShutdown_(false),
	virtualServiceKeyPresent_(false), initialized_(false), deleteAllFilesInProgress_(false)
{
    sessionManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "SessionManager Thread" );
}

SessionManager::~SessionManager()
{
    if (sessionManagerThread)
	{
		sessionManagerThread->StopThread();
		sessionManagerThread->WaitForCompletion();
	}

	for (startersContainer_iterator it = starters_.begin(); it != starters_.end(); ++ it)
		it->get()->cleanupThread();
	ShutdownNicely();
	
	remoteSessions.clear();
    // jhc gcd to WINDUX: not needed after the merge, but wanted to keep
    // it here until tested out. Up above now
    //sessionManagerThread->StopThread();
    deregisterAllEvents();

	//GCD Session
	if ( gcdSession )
	{
		delete gcdSession;
		gcdSession = NULL;
	}
}

int SessionManager::activeFunction(void *UNUSED(pObject))
{
    return 0;
}

void SessionManager::init()
{
    Configuration *config = Configuration::getInstance();

    serverSocket.setPort((config) ? config->getWellKnownPort() : 6000);
    /* bind socket to local address */
    serverSocket.sBind();
    /* create queue for client connection requests */
    serverSocket.sListen();

    serverSocket.setNonBlocking();

	util_filesystem::File cfgDebugFile(DEBUG_CONFIG_FILE_NAME);
	if (cfgDebugFile.exists())
		configDebugFileChecksum_ = cfgDebugFile.checksum();
}

void SessionManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    EvolMessage evolMessage( msg->Data() );
    switch (evolMessage.getDeviceType())
    {
        case RFIDReader:
            {
                if (evolMessage.getEventType() == RFIDCurrentStatus)
                {
                    RFIDStatus status(msg->Data());
                    if (!status.getComStatus())
                        RFIDManager::deleteInstance();
                }
            }
            break;

        case TimerDev:
            if (evolMessage.getEventType() == TimerExpired)
            {
                Timeout timeout( msg->Data() );
                if (!timeout.isSessionIdSet())
                {
                    // The SessionManager only pays attention to timers that are not assigned to/created by
                    // a specific Session.  Any timer with a 'mySessionId' set is supposed to have that Session
                    // listening for TimerDev|TimerExpired messages, so it can handle its own timers.
                    // The automatic-database-writeback-to-flash task is such a task that is not assigned
                    // to a specific Session.
                    // The SessionManager creates a Session to handle the timer and assigns the
                    // new Session's attributes to the expired timer.

                    //memShow(0);
                    //backCount++;
                    //Debug::trace("SessionManager", HobString("backup count = ") + HobString(Integer::toString(backCount)));

                    //sjjc                                      if (!primaryConsoleSession)
                    //sjjc                                              Debug::trace("SessionManager", "A timer expired but there is no primaryConsole Session!");
                    //sjjc                                      else
                    {
                        if (!timerManager)
                            Debug::trace("SessionManager", "A timer expired but there is no TimerManager!");
                        else
                        {
                            Timer *timer = timerManager->getTimer(timeout.getTimerId());
                            //                                          timer->show();
                            TimedFunction timerPurpose = timer->getTimerPurpose();
                            switch (timerPurpose)
                            {
                                case timedFuncDailyTimersCheck:
                                {
                                    Debug::trace("SessionManager", "timeout - This is a timedFuncDailyTimersCheck - check timers for today.");
                                    checkDailyTimers();
                                    break;
                                }
                                case timedFuncShutdownWaitingForRemotesSessionsToQuit:
                                {
                                    Debug::trace("SessionManager", "timeout - The timer waiting for all remote and comm sessions to terminate expired - continuing with the shutdown.");
                                    handleShutdownRequest();
                                    break;
                                }
								case timedFuncMonitorMemory:
								{
									Debug::trace("SessionManager", "timeout - This is a timedFuncMonitorMemory - used with the Accountant.");
									monitorMemory();
									break;
								}
								case timedFuncDownloadFilesCheck:
								{
									Debug::trace("SessionManager.download", "timeout - timedFuncDownloadFilesCheck");
									downloadingFilesTimerId_ = 0;
									checkForDownloadedFiles();
									break;
								}
								case timedFuncDumpWrapperData:
								{
									Debug::trace("SessionManager", "timeout - timedFuncDumpWrapperData");

									if (!isWrapperAttached() || !primaryConsoleSession)
									{
										Debug::trace("SessionManager", "timedFuncDumpWrapperData - Wrapper isn't ready for messages. Aborting.");
										return;
									}

									// Spool up a new session to handle this task.
									Session *newSession = new Session();
									if (newSession)
									{
										PAL_absclass::PAL_sleep(500);
										newSession->setSessionType(AbstractSession::SESSIONTYPE_HELPER_SENDWRAPPERDATA);
										addSession(newSession);
										newSession->setExpiredTimer(timeout);

										// Follow this to see the handling logic for this session mode
										newSession->setCurrentModeOp(Session::OP_SENDWRAPPERDATATOAPPHOOK); 

										Debug::trace("SessionManager", "Created new wrapper-data session");
									}

									/*
									// We only need to capture WrControl messages once per "timer cycle" -- the wrapper sends this very frequently
									startTrackingWrStatusForWrapperDump(); 
									
									HfcEvolWrapperControl* wrControl = primaryConsoleSession->getConfigurationManager()->getCurrentEvolWrapperControl();
									MtdNGWWrapperData* wrapperFlashData = FpmManager::getInstance()->readWrapperData();

									// We'll grab what we can from the Configuration and FPM managers, since that data is readily accessible.
									primaryConsoleSession->sendWrapperDataToHook(wrControl);
									primaryConsoleSession->sendWrapperDataToHook(wrapperFlashData);

									// We'll have to get the rest of the data from WrEvolStatus events, which fortunately occur frequently.
									// See handleWrapperMessage in SessionWrapper.cpp for more. It's a bit of a hack, but the alternative 
									// would require a wrapper firmware update which -- as of this writing -- won't be happening soon.
									*/

									break;

								}
                                default:
                                {
                                    Debug::trace("SessionManager", "Creating a Session because a timer expired ");

                                    Session *newSession = new Session();
									if (newSession)
									{
										PAL_absclass::PAL_sleep(500);
										newSession->setSessionType(AbstractSession::SESSIONTYPE_HELPER);
										addSession(newSession);
										newSession->setExpiredTimer(timeout);
										newSession->setCurrentModeOp(Session::OP_HANDLETIMEOUT);
									}
                                    break;
                                }
                            }

							if (timer->getRepeatFrequency() == Timer::RepeatNO)
							{
								TimerManager *timerManager = TimerManager::getInstance();
								if (timerManager)
									timerManager->removeTimer(timer->getTimerId());
							}
                        }
                    }
                }
            }
            break;

        case SessionDev:
            {
                Debug::trace("SessionManager", "Received a SessionDev message");

                switch ( evolMessage.getEventType() )
                {
                    case SnTerminated:
                        {
                            AbstractSession *s = dynamic_cast<AbstractSession *> (evolMessage.getSourceObject());
                            if (s)
                            {
                                HobString sn = s->getName();
                                Debug::trace("SessionManager", HobString("Will be removing Session ") + sn);
                                removeSession(s);
                                Debug::trace("SessionManager", HobString("removed Session ") + sn);
                            }
                            else
                                Debug::trace("SessionManager", HobString("supposed to remove a Session, but the message's source was not a Session"));

                            break;
                        }
                    case SnRequestDBCompress:
                        {
                            requestToCompressDB_ = true;
                            break;
                        }
                    case SnShutdownRequest:
                        {
                            handleShutdownRequest();

                            break;
                        }
					case SnLogoffRequest:
					{
						handleLogoffRequest();

						break;
					}
                    case SnReboot:
                        {
                            RebootMessage reboot(msg->Data());
                            RebootRequest request = reboot.isPowerDownOnly() ? RebootRequest_PowerDown : RebootRequest_Reboot;

                            Debug::trace("SessionManager", HobString("Received an SnReboot message. req = ") + Integer::toString(request));

                            if (shutdownStep > 0 && shutdownStep != 99)
                            {
                                Debug::trace("SessionManager", HobString("shutdownStep = ") + Integer::toString(shutdownStep));

                                rebootRequest_ = request;
                            }
                            else
                            {
                                Debug::trace("SessionManager", HobString("calling handleRebootMessage"));
                                handleRebootMessage(request);
                            }

                            break;
                        }
                    case SnAddNewSession:
                        {
                            AddNewSessionMessage message(msg->Data());
                            AbstractSession* session = message.getSession();

                            if ( session )
                            {
                                Debug::trace("SessionManager", "Received an AddNewSession message. Going to add it.");

                                addSession(session);
                            }
                            else
                                Debug::trace("SessionManager", "Received an AddNewSession message without a session!?");

                            break;
                        }
                    case SnRemoveSessionStarter:
                        {
                            Util::CSmartPtr<RemoveSessionStarterMessage> message(new RemoveSessionStarterMessage(msg->Data()));

                            removeSessionStarter(message->getSessionStarter());

                            break;
                        }
					case SnInitializeSynchronization:
                        {
							requestToInitializeSynchronization_ = true;
                            break;
                        }
                    case SnTerminateSynchronization:
                        {
							requestToTerminateSynchronization_ = true;
                            break;
                        }
                    case SnChangeSynchronizationType:
                        {
                            Util::CSmartPtr<ChangeSynchronizationTypeMessage> change(new ChangeSynchronizationTypeMessage(msg->Data()));
							requestToChangeSynchronizationType_ = true;
							requestedSyncType_ = change->getSynchronizationType();
							requestedSyncTypePrimaryScale_ = change->getPrimaryScale();
                            break;
                        }
                    case SnChangePrimaryScale:
                        {
                            Util::CSmartPtr<ChangePrimaryScaleMessage> change(new ChangePrimaryScaleMessage(msg->Data()));
							requestToChangePrimaryScale_ = true;
							requestedPrimaryScale_ = change->getPrimaryScale();
                            break;
                        }
                    case SnCheckForDownloadFiles:
                        {
                            checkForDownloadedFiles();
                            break;
                        }
					case snMergeNow:	// SOF-4769
					{
						checkForFilesToMerge();
						break;
					}
					case snMergeNowAfterComm:	// SOF-4769
					{
						checkForFilesToMergeAfterComm();
						break;
					}
                    case SnPerformAutoBackupDB:
                        {
                            requestToPerformAutoBackupDB_ = true;
                            break;
                        }
					
                    default:
                        {
                            break;
                        }
                }

				// Check if any sync messages were received, and try to process them.
				performSynchronizationRequest();

				break;
            }

		case ConfigurationDev:
			{
				Debug::trace("SessionManager", "Received a ConfigurationDev message");

				switch ( evolMessage.getEventType() )
				{
					case CfgDeleteAllFilesInProgress:
						Debug::trace("SessionManager", "Received CfgDeleteAllFilesInProgress");
						deleteAllFilesInProgress_ = true;
						break;

					case CfgDeleteAllFilesFinished:
						Debug::trace("SessionManager", "Received CfgDeleteAllFilesFinished");
						deleteAllFilesInProgress_ = false;
						break;

					case CfgSaved:
						// Check and see if fractionalPricingEnabled or PoundsForPricingEnabled changed.
						// If so, need to write an audit trail message.
						Debug::trace("SessionManager", "Received CfgSaved");
						checkMetrologySealableParameters();

#if defined( __linux__ )
						if ( Configuration::getInstance()->getCustomerHooksEnabled() )
							hobart_cust_hook_config_saved();
#endif

						break;

					default:
						break;
				}
			}

        case VersionManagerDev:
            {
                Debug::trace("SessionManager", "Received a VersionManagerDev message");

                switch ( evolMessage.getEventType() )
                {
					case FirmwareUpdateStatus:
					{
						// Check and see if we have a new FPC version.
						// If so, need to write an audit trail message.

						VmStatusMsg status(msg->Data());
						if ( status.getStatusCode() == VersionManagerUsbDeviceDownloadSucceeded )
						{
							Debug::trace("SessionManager", "Received VersionManagerUsbDeviceDownloadSucceeded");

							string msg = "USB device firmware updated successfully.";
							MessageLogger::logMessage(MessageLogger::InfoLog, msg);

							// Don't need to reboot, but if we did, the following code would do that.
							//PAL_absclass::PAL_sleep(1000);
							//rebootRequest_ = RebootRequest_Reboot;
							//panicShutdown_ = true;
							//initiateShutdown_ = true;

							checkMetrologySealableParameters();
						}
						break;
					}
					default:
						break;
				}
			}

        case NetworkConnectivityDev:
            {
                Debug::trace("SessionManager", "Received a NetworkConnectivityDev message");

                switch ( evolMessage.getEventType() )
                {
                    case NetworkAvailable:
                        {
                            Debug::trace("SessionManager", "Received a NetworkConnectivityDev:NetworkAvailable message");

                            usecaseComingUpOnLine();

                            break;
                        }
                    case NetworkUnavailable:
                        {
                            Debug::trace("SessionManager", "Received a NetworkConnectivityDev:NetworkUnavailable message");

                            usecaseGoingOffLine();

                            break;
                        }
                    default:
                        {
                            Debug::trace("SessionManager", "Received an unknown NetworkConnectivityDev message with event=" +
                                    util_string::uinttostring(evolMessage.getEventType()));

                            break;
                        }
                }

                break;
            }
        default:
            break;
    }

}

//GCD
void SessionManager::InitializeGCDSession()
{
	if ( Configuration::getInstance()->isServiceCounterScale() )
	{
		//Create the GCD Session only if we have a Primary Session
		if (primaryConsoleSession)
		{
			Debug::trace("GCDSession", "SessionManager::InitializeGCDSession():  We have a Primary Session so go ahead and startGCDSession().");
			startGCDSession();
		}
		else
		{
			Debug::trace("GCDSession", "SessionManager::InitializeGCDSession():  We don't have a Primary Session yet, so wait.");
			createGCDSession = true;
		}
	}
}

//GCD
void SessionManager::startGCDSession()
{
	// Already have GCD session
	if ( gcdSession )
		return;
		
#if defined( __linux__)
	//determine if the GCD is physically connected to the system controller
	//this script checks a GPIO
	if( system("/usr/local/hobart/python/getGcdStatus.sh") )
		Debug::trace("GCD", "SessionManager - GCD is physically attached");
	else
	{
		Debug::trace("GCD", "SessionManager - GCD is not physically attached ");
		return;
	}
#else
	// TODO
	//determine if the GCD is physically connected to the system controller
#endif

	//Create the GCD Session
	//Debug::trace("GCD", "SessionManager - SnSignalGCD - creating the customer display session");
	//Debug::trace("GCDSession", "SessionManager::startGCDSession()");
	
	gcdSession = new GCDSession();
//	The constructor for GCDSession should initialize its own variables.		gcdSession->InitializeGCDVariables();
	
	//Debug::trace("GCDSession", "SessionManager::startGCDSession():  gcdSession = " + Hex::toString((LONG)(LONG_PTR)gcdSession));
	//Debug::trace("GCD", HobString("SessionManager - SnSignalGCD gcdSession: ") + Hex::toString((LONG)(LONG_PTR)gcdSession));

	addSession(gcdSession);
}

DWORD SessionManager::Run()
{
    edb::DBManager *myDatabase = (edb::DBManager *)NULL;

    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;
    HobString msgDebug;
    if (traceOn)
        msgDebug = "Run():  ";

    if (traceOn)
		Debug::trace("SessionManager", msgDebug + "activeFunctionStart is TRUE");

    init();
    if (traceOn)
		Debug::trace("SessionManager", msgDebug + "init() is done");

    // startup diagnostics manager
    if ( Configuration::getInstance()->isDiagnosticsEnabled() )
    {
		Debug::trace( "SessionManager", "Starting diagnostics..." );
		DiagnosticsManager::instance();
    }

    // The Session Manager is the first task to access the database,
    // via the Configuration singleton.  It opens it up for all to use.

    bool checkResult = true;
    if (Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINTEGRITY))
        checkResult = checkDatabaseIntegrity(true);

    if (checkResult && Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINDICES))
        checkResult = checkDatabaseIndices();

    if (!checkResult)
	{
		bool handledCorrupt = handleCorruptDatabase();
        if (!handledCorrupt)
		{
			string msg = "Database corruption detected; Do not have a good database to use.  Exiting!!!";
			MessageLogger::logMessage(MessageLogger::InfoLog, msg);
			if (traceOn)
				Debug::trace("SessionManager", msgDebug + msg);
			return 0;
		}
	}

	// Now open the database.
	bool isOpen = openTheDatabase();
	if (isOpen)
	{
		// The indices check out OK, and we were able to open the database.  So go ahead and close it and make a backup of the database.
		closeTheDatabase();

		std::string cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc;
		bool goodDirs = validateDatabaseDirectories(cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc);

		copyDatabaseFiles(cfMirrorDbLoc, cfDbLoc);

		// Now reopen the database for the application.
		isOpen = openTheDatabase();
	}

	if (!isOpen)
	{
		string msg = "Database error at bootup; Could not open the database.  Exiting!!!";
		MessageLogger::logMessage(MessageLogger::InfoLog, msg);
		if (traceOn)
			Debug::trace("SessionManager", msgDebug + msg);
		return 0;
	}

    try
    {
        myDatabase = edb::DBManager::getInstance();

        hfc::HFCManagerContainer dba;
        dba.initDB();


{
		string sqlname, sqlcmd;
		try
		{
			DBConfigurationsRM *dbConfigurationsRM = dba.getDBConfigurationsRM();
			if (dbConfigurationsRM)
			{
				int delay = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYBETWEENSESSIONMESSAGES);
				int i = 0;
				while (i>=0)
				{
					sqlname = "SQLInitialCommand" + inttostring(i++);
					sqlcmd = Configuration::getInstance()->toStringCfgI(sqlname);
					if (sqlcmd.size())
					{
						Debug::trace("SessionManager", msgDebug + sqlname + "=" + sqlcmd);

						bool res = dbConfigurationsRM->execute(sqlcmd);
						PAL_absclass::PAL_sleep(delay);
					}
					else
						break;
				}
			}
		}
		catch (std::exception e)
		{
			string es = e.what();
			Debug::trace("SessionManager", "Initial SQL command - command not found - " + sqlname);
		}
}


        Configuration::getInstance()->load(&dba);
        Configuration::getInstance()->show();


#if defined( __linux__ )
		// have the hook open the database
		if ( Configuration::getInstance()->getCustomerHooksEnabled() )
		{
            static const std::string empty;

		    hobart_cust_hook_open_database();

            // build number
            const std::string buildNum( BuildNumber::buildNumber );

			hobart_cust_hook_update_scale_state( HOOK_SS_SCALE_BUILD_NUMBER, buildNum.c_str() );
		}
#endif


		Configuration::getInstance()->writeSystemLangFile();

        dba.updateSyncList();

		PrinterStockMetrics::initStatics();

        // Set the MultiLanguageText component to use the PrimaryLanguage for the purpose of
        // creating reserved records
		try
		{
			hfc::MultiLanguageText::setLanguageSelection(PrimaryLanguage);

  			Platform::getInstance().setupLocale(Configuration::getInstance()->getPrimaryLanguageCode(), Configuration::getInstance()->getPrimaryCountryCode());
		}
		catch (std::exception e)
		{
			string es = e.what();
			Debug::trace("SessionManager", "Exception calling MultiLanguageText::setLanguageSelection(PrimaryLanguage) - " + es);
		}

		hfc::MultiLanguageText internationalize;
		MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessageApplicationStarted"));

        //      Recreate the default Generic Label Text records.
        GenericLabelText::verifyDefaultRecords(&dba);

		//      Recreate the default Email Alert records.
		HobEmailAlert::verifyReservedRecords(&dba);

        //      Recreate the default PresentationMethod records.
        PresentationMethod::verifyReservedRecords(&dba);

        //      Recreate the default ProductTextCfg records.
        ProductTextCfg::verifyDefaultRecords(&dba);

		//      Recreate the default HfcScannerProductRead records - But I dont need these.
        //HfcScannerProductRead::verifyDefaultRecords(&dba);

        //      Recreate the default Transaction Type records.
        {
            Util::CSmartPtr<hfc::TransactionTypeManager> manager(new hfc::TransactionTypeManager(&dba));
            manager->verifyReservedRecords();
        }

		verifyDeptBasedReservedRecords(&dba);

		Session::setRemotePrintReturnCodes();

    }
    catch (DBException &dbe)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("SessionManager::activeFunctionTask() ********** database exception **********"));
            DBException::Type t = dbe.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
        }
    }


    myDatabase->setDatabaseLanguage(1, Configuration::getInstance()->getPrimaryLanguageCode() + "_" + Configuration::getInstance()->getPrimaryCountryCode());
    myDatabase->setDatabaseLanguage(2, Configuration::getInstance()->getSecondaryLanguageCode() + "_" + Configuration::getInstance()->getSecondaryCountryCode());

    edb::DBManager::destroyInstance(myDatabase);
	myDatabase = (DBManager *)NULL;


	// Read the printers that the OS knows about.
	SystemPrinters::findAllSystemPrinters();


    //  Create the Timer Manager here so we only have one timer manager per scale.
    if (Configuration::getInstance()->isTimerManagerEnabled())
    {
        Debug::trace("SessionManager", "TimerManager is enabled");
        timerManager = TimerManager::getInstance();
        PAL_absclass::PAL_sleep(1);

        if (timerManager)
        {
			int timerVal = Configuration::getInstance()->getDbBackupFrequency();
            
			if (timerVal > 0)
			{
				timerManager->addTimer(timerVal * 60, Timer::RepeatInMinutes, timedFuncBackupDatabase);
				Debug::trace("SessionManager", "Created automatic backups timer: minutes=" + inttostring(timerVal));
			}

			//  Create a timer to go off every minute.
			timerVal = Configuration::getInstance()->toIntCfgI(CfgInternal::SESSIONMANAGERPERIODICTIMER);
			if (timerVal > 0)
				timerManager->addTimer(timerVal, Timer::RepeatInMinutes, timedFuncDailyTimersCheck);

			//  Create a timer for when to check for downloaded files.
			timerVal = Configuration::getInstance()->toIntCfgI(CfgInternal::DOWNLOADCHECKINGINTERVALTIMER);
			if (timerVal > 0)
				timerManager->addTimer(timerVal, Timer::RepeatInMinutes, timedFuncDownloadFilesCheck);

			//	Create a timer to check Memory at a specified interval.
			timerVal = Configuration::getInstance()->toIntCfgI(CfgInternal::MONITORMEMORYINTERVAL);
			if (timerVal > 0)
			{
				timerManager->addTimer(timerVal, Timer::RepeatInMinutes, timedFuncMonitorMemory);
				Debug::trace("MonitorMemory", "Created timedFuncMonitorMemory timer to go off every " + inttostring(timerVal) + " seconds.");
			}

			bool isWrapper = Configuration::getInstance()->isWrapper();
			bool hooksEnabled = Configuration::getInstance()->getCustomerHooksEnabled();
			// Create a timer to fetch wrapper data and send it to the customer hook.
			// ... But only if this is a wrapper and there is a hook to send it to, of course. No point in making this otherwise.
			if (timerVal > 0 && isWrapper && hooksEnabled)
			{
				timerManager->addTimer(300, Timer::RepeatInMinutes, timedFuncDumpWrapperData);
				Debug::trace("DumpWrapperData", "Created wrapper data dump timer (device is a wrapper and hook is enabled).");
			}
        }
        else
            Debug::trace("SessionManager", "Failed to get a TimerManager instance !!!");
    }
    else
        Debug::trace("SessionManager", "TimerManager is not enabled !!!");

    registerEvent(NetworkConnectivityDev, NetworkAvailable);
    registerEvent(NetworkConnectivityDev, NetworkUnavailable);

	/* ERROR - we cant run on windows with the connectivity manager */
#if defined( __linux__ )
#if defined (__linux__)
	NetworkConnectivityManager::getInstance(); //only here to create the email manager singleton. It should neve be deleted
#endif

#endif

	EmailManager::getInstance(); //only here to create the email manager singleton. It should neve be deleted
    comm::CGICallManager::getInstance(new comm::CGIHTTPAdapter()); //only here to create the CGI Call manager singleton. It should neve be deleted

    //JRBRunTestCommon();

    if (Configuration::getInstance()->toBoolCfgI(CfgInternal::RFIDENABLED))
    {
        TCHAR com[16];
		memset(com, 0, 16 *sizeof(TCHAR));
#if defined (__linux__)
        // Linux serial port name is "/dev/ttyx"
        swprintf(com, 16, _T("/dev/ttyS%d"), (Configuration::getInstance()->toIntCfgI(CfgInternal::RFIDCOMPORT)-1));
#else
        wsprintf(com, _T("COM%01d"), Configuration::getInstance()->toIntCfgI(CfgInternal::RFIDCOMPORT));
#endif

        registerEvent(RFIDReader, CurrentStatus);
        RFIDManager::getInstance()->connect(com);
    }
    
    PAL_absclass::PAL_sleep(0);
	// DEBUG HOOKING UP: PAL_absclass::PAL_sleep(20000);
	
    if (Configuration::getInstance()->toBoolCfgI(CfgInternal::BARCODESCANNERENABLED))
    {
	    ScannerManager::getInstance();
    }

    registerEvent(TimerDev, TimerExpired);
    registerEvent(SessionDev, SnTerminated);
    registerEvent(SessionDev, SnShutdownRequest);
	registerEvent(SessionDev, SnLogoffRequest);
    registerEvent(SessionDev, SnReboot);
    registerEvent(SessionDev, SnAddNewSession);
    registerEvent(SessionDev, SnRemoveSessionStarter);

    // The SessionManager is the task concerned with performing the db compression.
    registerEvent(SessionDev, SnRequestDBCompress);

    registerEvent(ProductDev, ProductChange);

	// The SessionManager is also responsible for changes to Synchronization
	registerEvent(SessionDev, SnInitializeSynchronization);
	registerEvent(SessionDev, SnTerminateSynchronization);
	registerEvent(SessionDev, SnChangeSynchronizationType);
	registerEvent(SessionDev, SnChangePrimaryScale);

    registerEvent(SessionDev, SnCheckForDownloadFiles);
    registerEvent(SessionDev, SnPerformAutoBackupDB);
	registerEvent(SessionDev, snMergeNow);
	registerEvent(SessionDev, snMergeNowAfterComm);

	registerEvent(ConfigurationDev, CfgDeleteAllFilesInProgress);
	registerEvent(ConfigurationDev, CfgDeleteAllFilesFinished);
	registerEvent(ConfigurationDev, CfgSaved);

	registerEvent(VersionManagerDev, FirmwareUpdateStatus);
	
	if ( Configuration::getInstance()->isServiceCounterScale() )
	{
		//GCD
		// don't call InitializeGCDSession() here, we haven't yet received the notifications.
		// APCCommManager.cpp will call SessionManager::InitializeGCDSession() based on the NmWakeupMsg message fields
		// Randy QMX DEMO
		// for standalong GCD, create the session of we are not a prepack scale
		if ( CustomerDisplayGCD == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCustomerDisplay) )
			if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::GCDENABLEDSTANDALONE) )
				if ( !Configuration::getInstance()->toBoolCfg( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) )
					createGCDSession = true;
	}


#ifdef USE_ACCOUNTANT
	monitorMemory();
#endif


	initialized_ = true;


    // Look and see if the db independent version data file exists.  If it does, then create a Session to restore the db data.
    if (Configuration::getInstance()->isAutoLoadVersionIndependentData())
    {
        File dbIndData(Configuration::getInstance()->getVersionIndependentBackupFileName());
        if (dbIndData.exists())
        {
            Debug::trace("SessionManager", "Upon startup, I found db independent data.  creating a 'helper' Session to restore the db data");
            dbIndDataExists = true;

			// Create a session to do this for me.
			Session *newSession = new Session();
			if (newSession)
			{
				PAL_absclass::PAL_sleep(500);
				newSession->setSessionType(AbstractSession::SESSIONTYPE_HELPER);
				addSession(newSession);
				newSession->setCurrentModeOp(Session::OP_RESTORINGDBINDEPENDENTDATA);
			}
        }
        else
            Debug::trace("SessionManager", "Upon startup, NO db independent data found.");
    }


	// Randy B. requested to auto-backup the db at bootup.  I don't want to do that yet if there is a db-independent-data file existing at bootup;
	// there wouldn't be any data in the database to backup.  If the db-independent-data file exists, we'll do the auto-backup of the database
	// after it's restored.  If it doesn't exist, go ahead and run the auto-backup now.
	// LAP - 8/10/18 - Now just set the flag that we want it to be done, and when checkDailyTimers() gets called, it will do the backup if it can.
	//                 If it can't (see performDBBackup() for why it couldn't), it will leave the flag set and try again in another minute.
	requestToPerformAutoBackupDB_ = true;

	if ( !dbIndDataExists )
	{
		Debug::trace("SessionManager", "No db-independent-data file exists at bootup, now calling checkMetrologySealableParameters");
		// Check the sealable metrological parameters that are not stored in the Peripheral
		// Controller's serial flash; they are stored in the system serial flash
		checkMetrologySealableParameters();

		// do not run if we are a boardsite build for lvds.
		util_filesystem::File  boardSiteFileLvds( boardSiteLvdsFile_ );
        if( !boardSiteFileLvds.exists( ) ) {

			// This was added here to go out and look for any files that are in the 'pending' directory at bootup, and if any exist, process them.
			// This could happen if a file was put into the 'download' directory, and SessionManager saw it during a call to checkDailyTimers(), which occurs every minute,
			// then moved it to pending for processing, but the scale rebooted before it could be fully processed.  At the next bootup, we wouldn't have looked for any
			// files in 'pending', so that file wouldn't be be processed until another file shows up in the download file, and who knows when that would happen.
			// So this following call will go out and process any files in 'pending' at bootup, irregardless of how that file got to be there.
			// We are going to use this capability to put new firmware into the 'pending' directory at first boot, instead of in the 'download' directory, so that it
			// can be processed sooner (remember that we only look for files in the 'download' every minute).		
			processDownloadedFiles();
		}
	}

	// This was used for testing cat3 functionality, to create a bunch of cat3 entries/records up-front.
	checkMetrologySealableParametersLots();

	int msgDelay = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYBETWEENMANAGERTHREADMESSAGES);

	if (Debug::isTracing("HobTokenizerTest"))
	{
		Debug::addTraceCategory("HobTokenizerBarTest");
		HobTokenizerBar::test();
		Debug::removeTraceCategory("HobTokenizerBarTest");
		Debug::addTraceCategory("HobTokenizerCSVTest");
		HobTokenizerCSV::test();
		Debug::removeTraceCategory("HobTokenizerCSVTest");
	}

    while (1)
    {
        /**
         * This socket is created when a client establishes a connection with the Session Manager.
         * A Session is created, and the Session communicates with the connecting client via this
         * socket connection.
         *
         * Doesn't need to be a class attribute, just set up as a local variable in the
         * method that accepts connections on the server socket.
         */

		PAL_absclass::PAL_sleep(msgDelay);

		if (dbIndDataExists)
		{
			// Look and see if the db independent version data file still exists.  The helper Session assigned to restore the data will 
			// delete the file when it is finished with it.  If it doesn't exist now, then the db data is restored, so reset the flag.
			// Also dispatch a message to any other Sessions still waiting the the restore to occur.
			File dbIndData(Configuration::getInstance()->getVersionIndependentBackupFileName());
			if (!dbIndData.exists())
			{
				Debug::trace("SessionManager", "Upon startup, found db independent data, but it's gone now");
				dbIndDataExists = false;

				EvolMessage e(SessionDev, DBRestoreFinished);
				dispatchEvent(&e);

				Debug::trace("SessionManager", "db-independent-data was restored, now calling checkMetrologySealableParameters");
				// Check the sealable metrological parameters that are not stored in the Peripheral
				// Controller's serial flash; they are stored in the system serial flash
				checkMetrologySealableParameters();

				// SOF-2835: Need to have processDownloadedFiles() here also to check for files in 'pending' at bootup.
				//           This will address the condition when after a core upgrade, the database restore is processed
				//           first.
				processDownloadedFiles();

				checkDailyTimers();
			}
			else
				// May as well nap a little since it takes some time to restore the db data.
				PAL_absclass::PAL_sleep(2000);
		}

        if ( allowConnections ) // True basically means not shutting down the system
        {
            /* (TODO) should this be removed so we always let the connection happen? */
            if ( inputProcessingEnabled_ )  //Do not accept connections while performing critical operations
            {
                Socket *clientSocket = serverSocket.sAccept();

                if (clientSocket)
                {
                    if (Debug::isTracing("Memory"))
                        memShow(0);
                    Debug::trace("SessionManager", HobString("start - Server accepted new client connection on socket: ") + HobString(clientSocket->toString()));

                    clientSocket->setNonBlockingIO();

                    /* (uc1.7) If this is a connection from a remote host and the Scale on-line flag
                       is clear, then execute "Use Case 1.3: Coming up on-line". */
                    /* (TODO) uncomment code */
                    //                                  if ( !sessionMgr_->isScaleOnline() && socket_->isSourceRemoteHost() )
                    //                                  {
                    /* (TODO) may need a proxy for SessionManager to execute Use Case 1.3 if we
                       need the effect of inline*/
                    //                                  }



                    /* Use SessionStarter to start a new session. */
                    addSessionStarter(new SessionStarter(this, clientSocket));
                }
            }

			if (initiateShutdown_)
			{
				initiateShutdown_ = false;
				handleShutdownRequest();
				handleRebootMessage( (rebootRequest_ != RebootRequest_NotReceived) ? rebootRequest_ : RebootRequest_Reboot );
			}
        }
        else
        {
            // We are waiting for all remote Sessions to terminate, because we are in the process of a shutdown.
            // Wait for all remote and comm sessions to terminate

            if (shutdownStep == 1)
            {     
                // the GCDSession might be blocked waiting for RAPI/ActiveSync to establish a connection
				// so remove it here explicitly
				if ( gcdSession )
				{
					gcdSession->ShutDownSession();
					removeSession( gcdSession);
				}
				
				if (!remoteSessions.empty())
                {
                    std::list<AbstractSession *>::iterator pos;
                    pos = remoteSessions.begin();
                    while (pos != remoteSessions.end())
                    {
                        //if ((*pos)->getSessionType() != Session::SESSIONTYPE_PRIMARY)
                        Debug::trace("SessionManager", HobString("handleShutdownRequest() - waiting for Session to terminate: ") + (*pos)->getName());
                        pos++;
                    }
                }
                if (remoteSessions.size() == 1)  // only the primary console remaining
                    handleShutdownRequest();
                else
                    PAL_absclass::PAL_sleep(6);
            }
            else if (shutdownStep > 1)
            {
                handleShutdownRequest();
            }

        }

		if (primaryConsoleSession)
		{
			if (usingMirrorDatabase)
			{
				PAL_absclass::PAL_sleep(2000);
				Debug::trace("SessionManager.badDB", "Dispatching a message to the session about recovering and using the mirrored database.");
				EvolMessage e(SessionDev, SnUsingMirrorDatabase);
				dispatchEvent(&e);
				usingMirrorDatabase = false;
			}
			else if (usingEmptyDatabase)
			{
				PAL_absclass::PAL_sleep(2000);
				Debug::trace("SessionManager.badDB", "Dispatching a message to the session about recovering and using the empty database.");
				EvolMessage e(SessionDev, SnUsingEmptyDatabase);
				dispatchEvent(&e);
				usingEmptyDatabase = false;
			}
		}

        if (requestToCompressDB_ && !commSessionExists())
        {
            requestToCompressDB_ = false;
			// Initially, we aren't going to compress the database; we aren't doing that for a SQLite database.
			// We still want to backup the database, which is why I separated the two out from each other, so I can do them separately.
            //performDBCompress();
			performDBBackup();

        }
        PAL_absclass::PAL_sleep(1);
    }
}


void SessionManager::verifyDeptBasedReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	//      Recreate the default Product Default records.
	ProductDefault::verifyReservedRecords(dbAccess);

	//      Recreate the default Class records.
	Class::verifyReservedRecords(dbAccess);

	//      Recreate the default Category records.
	Category::verifyReservedRecords(dbAccess);

	//      Recreate the default Specials records.
	Special::verifyReservedRecords(dbAccess);

	//      Recreate the default SoftkeyGroup records.
	SoftkeyGroup::verifyReservedRecords(dbAccess);
	//      Recreate the default SoftkeyFeature records.
	SoftkeyFeature::verifyReservedRecords(dbAccess);
	//      Recreate the default SoftkeyStyle records.
	SoftkeyStyle::verifyReservedRecords(dbAccess);
	//      Recreate the default Softkey records.
	//  We will only do this in the context of individual missing softkey groups
	//            Softkey::verifyReservedRecords(dbAccess);

	//  Directly above, the call to Softkey::verifyReservedRecords was commented out,
	//  but we now have some default softkeys, for the HTi SelfServe alphabetic search buttons and the best seller button.
	//  So, I'm call the verifyReservedRecords method, but passing in the specific group number that I need.
	Softkey::verifyReservedRecords(dbAccess, ReservedSK_SelfservAZCategoryButtons);

	Dept::verifyReservedRecords(dbAccess);
}


bool SessionManager::isRestoringDB()
{
	return dbIndDataExists;
}


void SessionManager::addSession(AbstractSession *aSession)
{
    if (!aSession)
        return;

	string msg = " name: " + aSession->getName() + " id: " + uinttostring(aSession->getId());

    if (findSession(aSession->getId()))
    {
        // This should never happen.  The autogeneration of Session Id's should
        // never generate duplicate session Id's.  Just in case, log an error message.
        Debug::trace("SessionManager", "Duplicate AbstractSession being added; AbstractSession not added: " + msg);
    }
    else
    {
        remoteSessions.push_back(aSession);
		int sessionListSize = (int)remoteSessions.size();
		Debug::trace("SessionManager", "adding AbstractSession: " + msg + "  size of remoteSessions list: " + inttostring(sessionListSize));
    }

    displayAllSessions();
}

void SessionManager::removeSession(AbstractSession *aSession)
{
    if (!aSession)
        return;

    displayAllSessions();

    HobString sessionName(aSession->getName());

    // Session is supposed to call this in its terminate() method.  Don't continue to
    // remove the Session if its status does not indicate that it is terminated.
    if (aSession->getStatus() != AbstractSession::STATUS_TERMINATED)
    {
        Debug::trace("SessionManager.removeSession", HobString("Supposed to remove AbstractSession, but status not TERMINATED: ") + sessionName);
        return;
    }

    Session *s = dynamic_cast<Session *> (aSession);
    if (s && s == primaryConsoleSession)
    {
        Debug::trace("SessionManager.removeSession", HobString("The primary console session is being deleted!!!!!!!!!!!!!!!!! ") + sessionName);
        primaryConsoleSession = 0;
    }

    // Remove any timers this session may have created.
    if (timerManager)
        timerManager->removeAllEventTimersForSession(aSession->getId());

    // In case this Session is monitoring the Primary Console,
    // tell the Primary Session to remove it from the monitoring list.
    if (primaryConsoleSession)
        primaryConsoleSession->removeMonitoringSession(aSession);

    if (!findSession(aSession->getId()))
    {
        // This should never happen.  We shouldn't be removing a session that was never
        // added to the AbstractSession collection.  Just in case, log an error(warning) message.
        Debug::trace("SessionManager.removeSession", HobString("THIS SHOULD NEVER HAPPEN!! Can't remove the AbstractSession; it was never added: ") + aSession->getName());
    }
    else
    {
        Debug::trace("SessionManager.removeSession", HobString("removing AbstractSession from remoteSessions List: ") + sessionName);
        remoteSessions.remove(aSession);
    }

    // If gcdSession is allowed to be deleted, it terminates the shut down thread
	if (aSession == gcdSession)
		return;
		
	/* The following 'delete' of the AbstractSession is initiated by the AbstractSession.  After the delete,
     * the session will be gone and no further processing will occur.  Therefore, the delete
     * should be the last thing done for the AbstractSession, since the AbstractSession is initiating the delete.
     */
    Debug::trace("SessionManager.removeSession", HobString("just before deleting AbstractSession ") + sessionName +
            HobString("  size of remoteSessions list: ") + HobString(Integer::toString((int)remoteSessions.size())));
    delete aSession;
}

AbstractSession *SessionManager::findSession(const unsigned int sessionID)
{
    AbstractSession *retSession = (AbstractSession *)NULL;

    if (!remoteSessions.empty())
    {
        std::list<AbstractSession *>::iterator pos;
        pos = remoteSessions.begin();
        while (pos != remoteSessions.end())
        {
            if ((*pos)->getId() == sessionID)
            {
                retSession = *pos;
                break;
            }
            pos++;
        }
    }

    return retSession;
}

bool SessionManager::doesSessionExist(const short sessionType) 
{
    if ( !remoteSessions.empty() )
    {
        std::list<AbstractSession *>::iterator pos;
        pos = remoteSessions.begin();
        while (pos != remoteSessions.end())
        {
			if ( (*pos)->getSessionType() == sessionType )
				return true;
			pos++;
        }
    }

    return false;
}

/*
   Session *SessionManager::findSessionByTaskId(const int taskID)
   {
   Session *retSession = (Session *)NULL;

   if (!remoteSessions.empty())
   {
   list<AbstractSession *>::iterator pos;
   pos = remoteSessions.begin();
   while (pos != remoteSessions.end())
   {
//lcg rev 0.1 if ((*pos)->VXWTask::id() == taskID)
if ( (void * )(*pos) == ( void * )taskID ) //lcg rev
{
retSession = *pos;
break;
}
pos++;
}
}

return retSession;
}*/


bool SessionManager::registerSessionType(AbstractSession *aSession)
{
    if (aSession)
    {
		Debug::trace("SessionManager", string("registerSessionType() - Attempting to register session ") + aSession->getName() + "  as type: " + inttostring(aSession->getSessionType()));

		if (aSession->isPrimarySession())
        {

            if (primaryConsoleSession)
            {
                Debug::trace("SessionManager", aSession->getName() + HobString("FAILED to register session as Primary Console Session - already have one: ") +
                        primaryConsoleSession->getName());

                // Error condition.  You can't have more than one primary console session.
                // Write a log entry and return false.
                return FALSE;
            }
            else
            {
                Session *s = dynamic_cast<Session *> (aSession);
                if (s)
                    primaryConsoleSession = s;

                Debug::trace("SessionManager", HobString("Registered session as Primary Console Session") + aSession->getName());

				if (createGCDSession)
				{
					Debug::trace("SessionManager", "registerSessionType():  We have a Primary Session now, so go ahead and startGCDSession().");
					startGCDSession();
					createGCDSession = false;
				}

/* We don't have the Primary Session restore the data anymore.
                if (dbIndDataExists)
                {
                    Debug::trace("SessionManager", HobString("Upon startup, I found db independent data.  I'm now telling the primary console session to load it. ") +
                            aSession->getName());

                    //session->setCurrentModeOp(Session::OP_RESTOREDBINDEPENDENTDATA);
                    EvolMessage e(SessionDev, SnRestoreIndependentData);
                    dispatchEvent(&e);

                    / * The SessionManager has notified the primary console session that the database data needs loaded                                   * db independent format datafile.  It's now its responsibility to load that data into the
                     * from the database.  The SessionManager has no further concerns with the existance of this file.
                     * The primary console session is to delete the file when it is done loading the data.
                     * /
                    dbIndDataExists = false;
                }
*/

                return TRUE;
            }
        }
        else if (aSession->isCommSession())
        {
            return TRUE;
        }
        else if (aSession->isIntranetSession())
        {
            return TRUE;
        }
		else if ( aSession->isRemotePrintSession() )
		{
			return TRUE;
		}
        else
        {
            // Not a primary console, must be a remote.
            Debug::trace("SessionManager", HobString("Registering session as remote session: ") + aSession->getName());

            bool countok = (getRemoteConsoleSessionCount() > Configuration::getInstance()->getMaxRemoteConsoleSessions()) ? false : true;
            if (!countok)
            {
                HobString h = HobString(Integer::toString(getRemoteConsoleSessionCount()));
                Debug::trace("SessionManager", HobString("Register session - remote session count:") + h +
                        HobString(" exceeds max:") +
                        HobString(Integer::toString(Configuration::getInstance()->getMaxRemoteConsoleSessions())));
            }
            return countok;
        }
    }
    return FALSE;
}


Session *SessionManager::getPrimaryConsoleSession()
{
    return primaryConsoleSession;
}


short SessionManager::getRemoteConsoleSessionCount()
{
    short count = 0;
    if (!remoteSessions.empty())
    {
        list<AbstractSession *>::iterator pos;
        pos = remoteSessions.begin();
        while (pos != remoteSessions.end())
        {
            if ((*pos)->getSessionType() == Session::SESSIONTYPE_REMOTE)
                count++;
            pos++;
        }
    }

    Debug::trace("SessionManager", HobString("count remote sessions : cnt=") +
            HobString(Integer::toString(count)));
    return count;
}


void SessionManager::performDBCompress()
{
    bool tracing = (Debug::isTracing("SessionManager")) ? true : false;

    if (tracing)
        Debug::trace("SessionManager", "performDBCompress - entering");

    EvolMessage start(SessionDev, SnDBCompressStarting);
    dispatchEvent(&start);
    // delay for 1 second to give Sessions time to be notified that the compress is about to start
    PAL_absclass::PAL_sleep(1000);

    requestToPause(0, true, false);

    if (tracing)
    {
        Debug::trace("SessionManager", "performDBCompress - DBManager::showAll() before getting DBManager instance");
        edb::DBManager::showAll();
        Debug::trace("SessionManager", "performDBCompress - before getting DBManager instance");
    }

    try
    {
		DBManager *myDatabase = DBManager::getInstance();
		if (myDatabase)
		{
			if (tracing)
				Debug::trace("SessionManager", "performDBCompress - before closing database");
			myDatabase->closeDatabase();
			if (tracing)
				Debug::trace("SessionManager", "performDBCompress - database is closed, before compressing");

			myDatabase->compressDatabase();
			if (tracing)
				Debug::trace("SessionManager", "performDBCompress - database is compressed");

			if (tracing)
				Debug::trace("SessionManager", "performDBCompress - before reopening database");
			myDatabase->openDatabase();
			if (tracing)
				Debug::trace("SessionManager", "performDBCompress - database is reopened");

			DBManager::destroyInstance(myDatabase);
			myDatabase = (DBManager *)NULL;
		}
    }
    catch (DBException &dbe)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("SessionManager::performDBCompress() ********** database exception **********"));
            DBException::Type t = dbe.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
        }
    }

    if (tracing)
    {
        Debug::trace("SessionManager", "performDBCompress - DBManager::showAll() after destroying my DBManager instance after the compress");
        DBManager::showAll();
        Debug::trace("SessionManager", "performDBCompress - before sending SnDBCompressDone message");
    }

    EvolMessage done(SessionDev, SnDBCompressDone);
    dispatchEvent(&done);

    resumePausedThreads();

    if (tracing)
        Debug::trace("SessionManager", "performDBCompress - leaving");
}


void SessionManager::performDBBackup()
{
    bool tracing = (Debug::isTracing("SessionManager")) ? true : false;

    if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::DBBACKUPENABLED))
		return;

    if (tracing)
        Debug::trace("SessionManager", "performDBBackup - entering");

    bool checkResult = true;
    if (Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINTEGRITY))
        checkResult = checkDatabaseIntegrity(true);

	if (!checkResult)
	{
		string msg("Database was not backed up due to the database being corrupted!");
		MessageLogger::logMessage(MessageLogger::InfoLog, msg);
		if (tracing)
			Debug::trace("SessionManager", msg);
	}
	else
	{
		EvolMessage start(SessionDev, SnDBBackupStarting);
		dispatchEvent(&start);
		// delay for 1 second to give Sessions time to be notified that the backup is about to start
		PAL_absclass::PAL_sleep(1000);

		requestToPause(0, true, false);

		if (tracing)
		{
			Debug::trace("SessionManager", "performDBBackup - DBManager::showAll() before getting DBManager instance");
			edb::DBManager::showAll();
			Debug::trace("SessionManager", "performDBBackup - before getting DBManager instance");
		}

		try
		{
			DBManager *myDatabase = DBManager::getInstance();
			if (myDatabase)
			{
				if (tracing)
					Debug::trace("SessionManager", "performDBBackup - before closing database");
				myDatabase->closeDatabase();
				if (tracing)
					Debug::trace("SessionManager", "performDBBackup - database is closed, before backing up");

				std::string cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc;
				validateDatabaseDirectories(cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc);

				copyDatabaseFiles(cfMirrorDbLoc, cfDbLoc);
				if (tracing)
					Debug::trace("SessionManager", "performDBBackup - database is backed up");


				if (tracing)
					Debug::trace("SessionManager", "performDBBackup - before reopening database");
				myDatabase->openDatabase();
				if (tracing)
					Debug::trace("SessionManager", "performDBBackup - database is reopened");

				DBManager::destroyInstance(myDatabase);
				myDatabase = (DBManager *)NULL;
			}
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("SessionManager::performDBBackup() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}

		if (tracing)
		{
			Debug::trace("SessionManager", "performDBBackup - DBManager::showAll() after destroying my DBManager instance after the backup");
			DBManager::showAll();
			Debug::trace("SessionManager", "performDBBackup - before sending SnDBBackupDone message");
		}

		EvolMessage done(SessionDev, SnDBBackupDone);
		dispatchEvent(&done);

		resumePausedThreads();
	}

    if (tracing)
        Debug::trace("SessionManager", "performDBBackup - leaving");
}


SessionManager *SessionManager::getInstance()
{
    if (instance == 0)
    {
        instance = new SessionManager();
    }
    return instance;
}


void SessionManager::displayAllSessions()
{
    if ( remoteSessions.empty() || !Debug::isTracing("SessionManager") )
        return;

    list<AbstractSession *>::iterator iter = remoteSessions.begin();
    AbstractSession *s;

    Debug::trace("SessionManager", "Displaying all sessions ...");
    while (iter != remoteSessions.end())
    {
        s = *iter;
		if (s)
		{
			long i = s->getId();
			HobString h = s->getName();
			Session *session = dynamic_cast<Session *> (s);
			if (session && session == primaryConsoleSession)
				h.append(": primary");
			Debug::trace("SessionManager", HobString(Long::toString(i)) + HobString(" -- ") +h);
			iter++;
		}
    }
}


void SessionManager::checkDailyTimers()
{
	// Don't need to do this if shutting down.
    if (shutdownStep > 0)
		return;

	Debug::trace("SessionManager", "Entering checkDailyTimers");


	if (Debug::isTracing("database"))
	{
		Debug::trace("database", "checkDailyTimers() - showAll DBManagers:");
		edb::DBManager::showAll();
	}


	// Check if any sync messages were received, and try to process them.
	performSynchronizationRequest();


	bool traceOn = Debug::isTracing("SessionManager.DailyTimers");
	bool tracingRSM = Debug::isTracing("SessionManager.remoteScaleMaintenance") ? true : false; // remote scale maintenance feature
    Configuration *config = Configuration::getInstance();
    if ( !config ) return;

	if (config->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED))
	{
		EvolMessage wrs(Weigher, WgReqStatus);
		dispatchEvent(&wrs); 
	}

	/*
	 * Check if either of the internal configuration files (config.dat configc.dat) has changed.
	 * If so, have CfgInternal reload its internal configuration parameters.
	 */
	config->checkReloadOfCfgI();

	/*
	 * Check if the file containing the debugging parameters, configdebug.dat, has changed.
	 * If so, have Trace reload its debugging parameters.
	 */
	util_filesystem::File cfgDebugFile(DEBUG_CONFIG_FILE_NAME);
	if (cfgDebugFile.exists() || configDebugFileChecksum_)
	{
		unsigned int newChecksum = (cfgDebugFile.exists()) ? cfgDebugFile.checksum() : 0;
		if (newChecksum != configDebugFileChecksum_)
		{
			Properties traceProps(DEBUG_CONFIG_FILE_NAME);
			Trace::getInstance(traceProps);
			configDebugFileChecksum_ = newChecksum;

			config->checkTracingCfgI();
			config->checkTracingCfg();

			try
			{
				if (config->toBoolCfgI("notifyWhenConfigDebugReloaded"))
				{
					std::string msg = string(DEBUG_CONFIG_FILE_NAME);
					msg.append((cfgDebugFile.exists()) ? " reloaded." : " deleted.");
					SessionManagerHelper::getInstance()->addDialog(0, 1, msg);
				}
			}
			catch (std::exception e)
			{
			}
//* Don't run met sig tests in production code - I uncommented-out this code because it only gets executed it MetSig.test is active in the debug categories
// which is a conscious effort by the one who sets it in configdebug.dat (ie. a developer). So the following will "never" be executed anyway. 
			if (Debug::isTracing("MetSig.test"))
			{
				SessionManagerHelper::getInstance()->addDialog(0, 1, "MetSig testing in progress ...");
				PAL_absclass::PAL_sleep(10);

				Debug::trace("MetSig.test", "******************** MetSig::test START ********************");

				if (Debug::isTracing("MetSig.test_allMetSig"))
					MetSig::test_allMetSig();

				// Price calculation routines

				if (Debug::isTracing("MetSig.test_calculateUnitPriceRounded"))
					MetSig::test_calculateUnitPriceRounded();
				if (Debug::isTracing("MetSig.test_calculateUnitPriceNoFractionalCents"))
					MetSig::test_calculateUnitPriceNoFractionalCents();
				if (Debug::isTracing("MetSig.test_calcFixedWeightPricePerUnit"))
					MetSig::test_calcFixedWeightPricePerUnit();
				if (Debug::isTracing("MetSig.test_divideAndRoundPortions"))
					MetSig::test_divideAndRoundPortions();
				if (Debug::isTracing("MetSig.test_byCountCalcTotalPrice"))
					MetSig::test_byCountCalcTotalPrice();
				if (Debug::isTracing("MetSig.test_determineImplicitDecimalPrice"))
					MetSig::test_determineImplicitDecimalPrice();

				// Weigher-related and weight calculation routines

				if (Debug::isTracing("MetSig.test_isWeigherDualRangeCapable"))
					MetSig::test_isWeigherDualRangeCapable();
				if (Debug::isTracing("MetSig.test_checkToSwitchDualRangeDirection"))
					MetSig::test_checkToSwitchDualRangeDirection();
				if (Debug::isTracing("MetSig.test_checkToSwitchDualRangeDirectionOnNetWeight"))
					MetSig::test_checkToSwitchDualRangeDirectionOnNetWeight();
				if (Debug::isTracing("MetSig.test_applyScaleFactorToWeight"))
					MetSig::test_applyScaleFactorToWeight();
				if (Debug::isTracing("MetSig.test_weightCountsToFixedWeight"))
					MetSig::test_weightCountsToFixedWeight();
				if (Debug::isTracing("MetSig.test_fixedWeightToWeightCounts"))
					MetSig::test_fixedWeightToWeightCounts();
				if (Debug::isTracing("MetSig.test_fixedWeightOuncesToDecimalWeight"))
					MetSig::test_fixedWeightOuncesToDecimalWeight();
				if (Debug::isTracing("MetSig.test_ouncesToPoundCounts"))
					MetSig::test_ouncesToPoundCounts();
				if (Debug::isTracing("MetSig.test_ouncesToGrams"))
					MetSig::test_ouncesToGrams();
				if (Debug::isTracing("MetSig.test_gramsToOunces"))
					MetSig::test_gramsToOunces();
				if (Debug::isTracing("MetSig.test_fluidOuncesToLiters"))
					MetSig::test_fluidOuncesToLiters();
				if (Debug::isTracing("MetSig.test_fluidOuncesToGallons"))
					MetSig::test_fluidOuncesToGallons();
				if (Debug::isTracing("MetSig.test_fluidOuncesToQuarts"))
					MetSig::test_fluidOuncesToQuarts();
				if (Debug::isTracing("MetSig.test_fluidOuncesToPints"))
					MetSig::test_fluidOuncesToPints();
				if (Debug::isTracing("MetSig.test_BRConvertFixedWeightToTotalWeight"))
					MetSig::test_BRConvertFixedWeightToTotalWeight();
				if (Debug::isTracing("MetSig.test_determineImplicitDecimalWeight"))
					MetSig::test_determineImplicitDecimalWeight();
				if (Debug::isTracing("MetSig.test_determineImplicitDecimalKeyboardNetWeight"))
					MetSig::test_determineImplicitDecimalKeyboardNetWeight();
				if (Debug::isTracing("MetSig.test_roundWeightTo2DecimalsThenBackTo3"))
					MetSig::test_roundWeightTo2DecimalsThenBackTo3();
				if (Debug::isTracing("MetSig.test_applyTare"))
					MetSig::test_applyTare();
				if (Debug::isTracing("MetSig.test_applyTareSetback"))
					MetSig::test_applyTareSetback();
				if (Debug::isTracing("MetSig.test_applyProportionalTare"))
					MetSig::test_applyProportionalTare();
				if (Debug::isTracing("MetSig.test_calculateRandomWeightTotalPrice"))
					MetSig::test_calculateRandomWeightTotalPrice();
				if (Debug::isTracing("MetSig.test_calculateRandomWeightOldTotalPrice"))
					MetSig::test_calculateRandomWeightOldTotalPrice();

				Debug::trace("MetSig.test", "******************** MetSig::test END ********************");


				SessionManagerHelper::getInstance()->removeDialog(0, 1);
			}
//*/
		}
	}

	if ( config->isPackageLicensed("SCALE_HANDSFREEDEVICE") )
	{
		// This will connect to the device process, which is the liason to communicate with any devices that are in-range.

		// Only create the session to manage communicating with the device if it doesn't yet exist
		if ( !doesSessionExist(AbstractSession::SESSIONTYPE_HANDSFREEDEVICEMGR) )
		{
			Debug::trace("HandsFree", "Hands-Free Devices licensed, do not yet have a Session to deal with them, attempting to connect with hfpdd");

			Socket *sock = new Socket();
			if (sock)
			{
				//responseTime_.getCurrentDateTime();
				sock->setAddress( config->toStringCfgI(fieldOffsetCfgIHFPDDIpAddress).c_str() );
				sock->setPort( config->toIntCfgI(fieldOffsetCfgIHFPDDPort) );
				sock->sConnect();

				//PAL_absclass::PAL_sleep(100);

				bool socketInvalid = true;

				if ( sock->isValid() )
				{
					Debug::trace("HandsFree", "Socket for hands-free devices is valid");
					socketInvalid = false;
					sock->setNonBlockingIO();
					sock->setMyReaderWriter(new CMReaderWriterHandsFreeDevice());

					Debug::trace("HandsFree", "SessionManager is creating a Session to communicate with hands-free devices.");
					Session *handsFreeDeviceSession = new Session(sock);
					PAL_absclass::PAL_sleep(50);
					if (handsFreeDeviceSession)
					{
						addSession(handsFreeDeviceSession);
						handsFreeDeviceSession->setSessionType(AbstractSession::SESSIONTYPE_HANDSFREEDEVICEMGR);
						handsFreeDeviceSession->setCurrentModeOp(Session::OP_INITHANDSFREEDEVICEMGR);
						Debug::trace("HandsFree", "SessionManager created a Session to communicate with hands-free devices: " + handsFreeDeviceSession->getName());
					}
					else
						socketInvalid = true;

				}

				if (socketInvalid)
				{
					Debug::trace("HandsFree", "Socket for hands-free devices is invalid: " + sock->handleError());
					sock->cmClose();
					delete sock;
					sock = 0;
				}
			}
		}
	}



	if ( config->toBoolCfgI(fieldOffsetCfgIBackupVersionIndependentDataNow) )
	{
		EvolMessage backupEntireDB(BackupRestore, BackupEntireDB);
		dispatchEvent(&backupEntireDB); 
	}


    //  Check all Daily timers:

    //  Do we need to check the Wrapper Sealer belt times?
    if (isWrapperAttached() && primaryConsoleSession)
    {
        //      What day of the week is it?
		HobString logMessage = "";
        HobString timeStrOn, timeStrOff, dayIs;

        HobDateTime now;
        HobString nowString(now.formatTime(FormatTime_HHMM, HourFormat_AMPM));
        now.setToCurrent();             //      To be sure we get a valid DayOfWeek
        short dow = now.getDayOfWeek();
        switch (dow)
        {
            case 0:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnSun);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffSun);
                dayIs = "Sunday";
                break;
            case 1:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnMon);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffMon);
                dayIs = "Monday";
                break;
            case 2:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnTue);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffTue);
                dayIs = "Tuesday";
                break;
            case 3:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnWed);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffWed);
                dayIs = "Wednesday";
                break;
            case 4:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnThu);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffThu);
                dayIs = "Thursday";
                break;
            case 5:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnFri);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffFri);
                dayIs = "Friday";
                break;
            case 6:
                timeStrOn = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOnSat);
                timeStrOff = config->toStringCfg(fieldOffsetCfgWrSealerBeltTimeOffSat);
                dayIs = "Saturday";
                break;
        }
        if (traceOn)
        {
            HobString msgDebug("Check SealerBeltTime,  DayOfWeek = ");
            msgDebug.append(Integer::toString(dow)).append(", ").append(dayIs).append(".  nowString = ").append(nowString).append("  timeStrOn = ").append(timeStrOn).append("  timeStrOff = ").append(timeStrOff);
            Debug::trace("SessionManager.DailyTimers", msgDebug);
        }

        if (timeStrOn == nowString)
        {
            Debug::trace("SessionManager.DailyTimers", "Turn the Wrapper Sealer belt ON.");
            primaryConsoleSession->setWrapperSealerBeltPower(true, primaryConsoleSession->getPeripheralManager()->getCurrentList().getWrapper());
			logMessage = "DailyTimers: Turning ON the Wrapper Sealer Belt.";
        }
        else
        {
            if (timeStrOff == nowString)
            {
                Debug::trace("SessionManager.DailyTimers", "Turn the Wrapper Sealer belt OFF.");
                primaryConsoleSession->setWrapperSealerBeltPower(false, primaryConsoleSession->getPeripheralManager()->getCurrentList().getWrapper());
				logMessage = "DailyTimers: Turning OFF the Wrapper Sealer Belt.";
            }
        }
		if( !logMessage.empty() )
			MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, logMessage);
    }


	HobDateTime now;
	int min = now.getMinute();
	int hour = now.getHour24();
	int dayOfWeek = now.getDayOfWeek();


	if ( config->toBoolCfgI(CfgInternal::DBCHECKINTEGRITY) )
	{
		// We periodically check the database to see if it's corrupt, based on an internal config parameter.

		// The number of times per day to perform the db integrity check (0 = never, 1 = at midnight, 2 = at midnight and noon,
		// 24 = hourly on the hour, 48 = every half hour, 96 = every 15 minutes, 144 = every 10 minutes, 288 = every 5 minutes, 1440 = every minute)
		int dbCheckIntegrityFrequency = config->toIntCfgI(CfgInternal::DBCHECKINTEGRITYFREQUENCY);

		// The frequency is in "# of times per day", so:
		// 0 = never check
		// 1 = once, at midnight
		// 2 = twice, at noon and midnight
		// 24 = every hour, on the hour
		// 48 = every hour and every half hour (every 30 minutes)
		// 96 = every 15 minutes
		// 144 = every 10 minutes
		// 288 = every 5 minutes
		// 1440 = every minute
		bool checkForDBIntegrity = false;
		switch (dbCheckIntegrityFrequency)
		{
			case 0:
				break;
			case 1:
				if ( min == 0 && hour == 0 )
					checkForDBIntegrity = true;
				break;
			case 2:
				if ( min == 0 && (hour == 0 || hour == 12) )
					checkForDBIntegrity = true;
				break;
			case 24:
				if ( min == 0 )
					checkForDBIntegrity = true;
				break;
			case 48:
				if ( min == 0 || min == 30 )
					checkForDBIntegrity = true;
				break;
			case 96:
				if ( (int)(min % 15) == 0 )
					checkForDBIntegrity = true;
				break;
			case 144:
				if ( (int)(min % 10) == 0 )
					checkForDBIntegrity = true;
				break;
			case 288:
				if ( (int)(min % 5) == 0 )
					checkForDBIntegrity = true;
				break;
			case 1440:
				checkForDBIntegrity = true;
				break;
			default:
				break;
		}

		if ( checkForDBIntegrity )
		{
			Debug::trace("SessionManager.DBIntegrityCheck", "It is " + string(now.toString()) + ", time to check db integrity.");

			bool checkResult = checkDatabaseIntegrity(false);

			Debug::trace("SessionManager.DBIntegrityCheck", "checkDatabaseIntegrity() returned " + booltostring(checkResult));

			if ( !checkResult )
			//if ( !checkResult || (min == config->toIntCfgI("dbCorruptMinute") && min != 0) )   // Use this line to facilitate testing of this functionality.
			{
				Debug::trace("SessionManager.DBIntegrityCheck", "Database integrity check failed, will now try to restore a valid one");

				if (primaryConsoleSession)
				{
					addDialog(ErrSessionDev, SessionDevDBIsCorrupt, "");
					//addDialog(ErrSessionDev, SessionDevRebootImmediate, "");
				}
				else
				{
					// No primary console, so just shutdown!
					rebootRequest_ = RebootRequest_Reboot;
					panicShutdown_ = true;
					initiateShutdown_ = true;
				}

				// Anything else we want to do to handle this!!
			}
		}
	}


	// We periodically check the database for "expired" records.  These include transactions,
	// shelf life expiration labels, and Product Statistic records.  We create a Session to do this for us.

	// The number of times per day to check for expired records (0 = never, 1 = at midnight, 2 = at midnight and noon,
	// 24 = hourly on the hour, 48 = every half hour, 96 = every 15 minutes, 144 = every 10 minutes, 288 = every 5 minutes, 1440 = every minute)
	int expiredRecordsCheckFrequency = config->toIntCfgI(CfgInternal::EXPIREDRECORDSCHECKFREQUENCY);

	// The frequency is in "# of times per day", so:
	// 0 = never check
	// 1 = once, at midnight
	// 2 = twice, at noon and midnight
	// 24 = every hour, on the hour
	// 48 = every hour and every half hour
	// 96 = every 15 minutes
	// 144 = every 10 minutes
	// 288 = every 5 minutes
	// 1440 = every minute
	bool checkForExpiredRecords = false;
	switch (expiredRecordsCheckFrequency)
	{
		case 0:
			break;
		case 1:
			if ( min == 0 && hour == 0 )
				checkForExpiredRecords = true;
			break;
		case 2:
			if ( min == 0 && (hour == 0 || hour == 12) )
				checkForExpiredRecords = true;
			break;
		case 24:
			if ( min == 0 )
				checkForExpiredRecords = true;
			break;
		case 48:
			if ( min == 0 || min == 30 )
				checkForExpiredRecords = true;
			break;
		case 96:
			if ( (int)(min % 15) == 0 )
				checkForExpiredRecords = true;
			break;
		case 144:
			if ( (int)(min % 10) == 0 )
				checkForExpiredRecords = true;
			break;
		case 288:
			if ( (int)(min % 5) == 0 )
				checkForExpiredRecords = true;
			break;
		case 1440:
			checkForExpiredRecords = true;
			break;
		default:
			break;
	}

    if ( checkForExpiredRecords )
    {
		Debug::trace("PurgeOldRecords", "It is " + string(now.toString()) + ", time to check for expired db records: transactions, expiration labels, product statistics.");
/*
		// Create a session to do this for me.
		Debug::trace("PurgeOldRecords", "SessionManager is creating a 'helper' Session to check for expired records.");
		Session *newSession = new Session();
		PAL_absclass::PAL_sleep(500);
		newSession->setSessionType(SESSIONTYPE_HELPER);
		addSession(newSession);
		newSession->setCurrentModeOp(Session::OP_CHECKFOREXPIREDDBRECORDS);
*/
		// The primary session is the only one listening for this device / event.
		Debug::trace("PurgeOldRecords", "SessionManager is sending a message to the Primary Session to check for expired records.");
		EvolMessage msg(SessionDev, SnCheckForExpiredRecords);
		dispatchEvent(&msg);
	}


	// We periodically check to automatically backup the database.
	// This was initially put in here for the HyVee updateds done in 2016.
	// We create a BackupRestore Session to do this for us.

	// The number of times per day to auto-backup the database (0 = never, 1 = at midnight, 2 = at midnight and noon,
	// 24 = hourly on the hour, 48 = every half hour, 96 = every 15 minutes, 144 = every 10 minutes, 288 = every 5 minutes, 1440 = every minute)
	int autoBackupDBFrequency = config->toIntCfg(fieldOffsetCfgAutoBackupDBFrequency);

	Debug::trace("SessionManager.Autobackup", "autoBackupDBFrequency=" + util_string::inttostring(autoBackupDBFrequency) );

	// The frequency is in "# of times per day", so:
	// 0 = never check
	// 1 = once, at midnight
	// 2 = twice, at noon and midnight
	// 24 = every hour, on the hour
	// 48 = every hour and every half hour
	// 96 = every 15 minutes
	// 144 = every 10 minutes
	// 288 = every 5 minutes
	// 1440 = every minute
	bool autoBackupDB = false;
	if (requestToPerformAutoBackupDB_)
	{
		autoBackupDB = true;		// always set to true?
		Debug::trace("SessionManager.Autobackup", "requestToPerformAutoBackupDB_ = TRUE so set autoBackupDB to true." );
	}
	else
	{
		switch (autoBackupDBFrequency)
		{
			case 0:
				Debug::trace("SessionManager.Autobackup", "switch (autoBackupDBFrequency)::case 0: so autoBackupDB = false");
				break;
			case 1:
				if ( min == 0 && hour == 0 )
					autoBackupDB = true;
				break;
			case 2:
				if ( min == 0 && (hour == 0 || hour == 12) )
					autoBackupDB = true;
				break;
			case 24:
				if ( min == 0 )
					autoBackupDB = true;
				break;
			case 48:
				if ( min == 0 || min == 30 )
					autoBackupDB = true;
				break;
			case 96:
				if ( (int)(min % 15) == 0 )
					autoBackupDB = true;
				break;
			case 144:
				if ( (int)(min % 10) == 0 )
					autoBackupDB = true;
				break;
			case 288:
				if ( (int)(min % 5) == 0 )
					autoBackupDB = true;
				break;
			case 1440:
				autoBackupDB = true;
				break;
			default:
				break;
		}
	}

    if ( autoBackupDB )
		performAutoBackupDB();


	// We aren't going to be able to check for an upgrade 5 minutes after bootup anymore, since we always upgrade if the file is on the ftp/sftp server;
	// We don't check the version numbers and not upgrade if the version intstalled is the same as what is on the server.
	// If we check for an upgrade 5 minutes after bootup, then the upgrade package will still likely be on the server, since it will take some
	// time to upgrade an entire chain, so we'll just upgrade again, and again, and again, and ...

    //  Do we need to check for a new Firmware version?
    
	///PRE-UPDATE NOW BELOW
	//if (config->isAutoSearchNewFirmwareVersions())
	
	// Is the scale using scheduled updating?
	Debug::trace("SessionManager.UpdateNowCheck","Timer is checking jobs");
	Debug::trace("SessionManager.UpdateNowCheck","string  useAutomatic is : " + config->toStringCfg(fieldOffsetCfgFTPuseAutomatic));

	if (config->toStringCfg(fieldOffsetCfgFTPuseAutomatic) == "1")
    {

		Debug::trace("SessionManager.UpdateNowCheck","STARTING UPDATENOW CHECK");
			
        // Check to see if it is time to have the VersionManager look for new firmware.
 	
		HobString updateNowSearchTime = config->getUpdateNowSearchTime();
		
		HobDateTime sTime;
        sTime.setTimeParts(updateNowSearchTime, FormatTime_HHMM, HourFormat_AMPM);
        updateNowSearchTime = sTime.formatTime(FormatTime_HHMM, HourFormat_AMPM);
		bool dayIsChosen = false;
		bool doUpdateNow = false;
        HobString nowString(now.formatTime(FormatTime_HHMM, HourFormat_AMPM));
					
		switch (dayOfWeek)
		{
			case 0: // Sunday
				if(config->toStringCfg(fieldOffsetCfgFTPsunday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.UpdateNowCheck","Sunday matches up");
		
				break;
			case 1: // Monday
		        if(config->toStringCfg(fieldOffsetCfgFTPmonday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.UpdateNowCheck","Monday matches up");
				break;
			case 2: // Tuesday
				if(config->toStringCfg(fieldOffsetCfgFTPtuesday) == "1")
					dayIsChosen = true;
								Debug::trace("SessionManager.UpdateNowCheck","Tuesday matches up");

				break;
			case 3: // Wednesday
				if(config->toStringCfg(fieldOffsetCfgFTPwednesday) == "1")
					dayIsChosen = true;
								Debug::trace("SessionManager.UpdateNowCheck","Wednesday matches up");

				break;
			case 4: // Thursday
				if(config->toStringCfg(fieldOffsetCfgFTPthursday) == "1")
					dayIsChosen = true;
								Debug::trace("SessionManager.UpdateNowCheck","Thursday matches up");

				break;
			case 5: // Friday
				if(config->toStringCfg(fieldOffsetCfgFTPfriday) == "1")
					dayIsChosen = true;
								Debug::trace("SessionManager.UpdateNowCheck","Friday matches up");

				break;
			case 6: // Saturday
     			if(config->toStringCfg(fieldOffsetCfgFTPsaturday) == "1")
					dayIsChosen = true;
								Debug::trace("SessionManager.UpdateNowCheck","Saturday matches up");

				break;
			default:
				doUpdateNow = false;
				dayIsChosen = false;
				break;
		}

		if(dayIsChosen)
		{
			// if daily check start time
			if(config->toStringCfg(fieldOffsetCfgFTPdailyUpdates) == "1" && updateNowSearchTime == nowString)
			{
				doUpdateNow = true;
								Debug::trace("SessionManager.UpdateNowCheck","daily and the right time");

			}
			
			// if hourly check minute is zero
			else if(config->toStringCfg(fieldOffsetCfgFTPhourlyUpdates) == "1" && (int)min == 0)
			{
				doUpdateNow = true;
								Debug::trace("SessionManager.UpdateNowCheck","hourly and on the hour");

			}

			// if every 15 minutes check modulus of minute/15 to be zero
			else if(config->toStringCfg(fieldOffsetCfgFTP15MinuteUpdates) == "1" && (int)(min % 15) == 0)
			{
								Debug::trace("SessionManager.UpdateNowCheck","15 minute and on the quarter hour");

				doUpdateNow = true;
			}
			else
			{
								Debug::trace("SessionManager.UpdateNowCheck","No update Now this minute");

				doUpdateNow = false;
			}
		}

		if(doUpdateNow)
		{
			// Here is where the updateNow occurs
			if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
			{
				HobString updateNowCommand;

			    // command format	updateNow|ip_address|remote_update_location|configuration_flags|user|password|id 
				updateNowCommand = "\"updateNow|";
				updateNowCommand += "";
				std::string serverName("");
				std::string userName(config->toStringCfg(fieldOffsetCfgFTPUserName));
				std::string password(config->toStringCfg(fieldOffsetCfgFTPPassword));
				std::string subDir(config->toStringCfg(fieldOffsetCfgFTPSubDir));
				std::string upgradeserver(config->toStringCfgI(CfgInternal::DEFAULTUPGRADEDIRECTORY)/*config->Configuration::getServerUpgradeName()*/);
				config->getFTPServerIPorHostName().getAsString(serverName);
				std::string pluDataFlag("plu_data");
				std::string labelDataFlag("label_data");
				std::string flashkeyDataFlag("flashkey_data");
				std::string operatorDataFlag("operator_data");
				std::string playlistsFlag("playlists");
				std::string alertsFlag("alerts");
				std::string scaleConfigurationDataFlag("configuration_data");
				std::string synchronizationConfigurationDataFlag("synchronization_data");
				std::string storeNameNumberDataFlag("store_data");
				std::string videosFlag("videos");
				std::string imagesFlag("images");
				std::string audioFlag("audio");
				std::string firmwareFlag("upgrade"); // this is the name of the folder which core files or features are placed in.

                updateNowCommand += serverName + "|";
				updateNowCommand += subDir + "|";

				bool areAnyFlags = false;

     			Debug::trace("SessionManager.UpdateNowCheck","HERE ARE THE USER CHOICES FOR UPDATENOW:");
				Debug::trace("SessionManager.UpdateNowCheck","PLU Data            : " + config->toStringCfg(fieldOffsetCfgFTPUpdatePLUData));
				Debug::trace("SessionManager.UpdateNowCheck","Label Data          : " + config->toStringCfg(fieldOffsetCfgFTPUpdateLabelData));
				Debug::trace("SessionManager.UpdateNowCheck","Flashkey Data       : " + config->toStringCfg(fieldOffsetCfgFTPUpdateFlashkeyData));
				Debug::trace("SessionManager.UpdateNowCheck","Operator Data       : " + config->toStringCfg(fieldOffsetCfgFTPUpdateOperatorData));
				Debug::trace("SessionManager.UpdateNowCheck","Playlists Data      : " + config->toStringCfg(fieldOffsetCfgFTPUpdatePlaylists));
				Debug::trace("SessionManager.UpdateNowCheck","Alerts              : " + config->toStringCfg(fieldOffsetCfgFTPUpdateAlerts));
				Debug::trace("SessionManager.UpdateNowCheck","Scale Configuration : " + config->toStringCfg(fieldOffsetCfgFTPUpdateScaleConfiguration));
				Debug::trace("SessionManager.UpdateNowCheck","PLU Data            : " + config->toStringCfg(fieldOffsetCfgFTPUpdateSynchronizationConfiguration));
				Debug::trace("SessionManager.UpdateNowCheck","PLU Data            : " + config->toStringCfg(fieldOffsetCfgFTPUpdateStoreNameNumber));
				Debug::trace("SessionManager.UpdateNowCheck","Videos              : " + config->toStringCfg(fieldOffsetCfgFTPUpdateVideos));
				Debug::trace("SessionManager.UpdateNowCheck","Images              : " + config->toStringCfg(fieldOffsetCfgFTPUpdateImages));
				Debug::trace("SessionManager.UpdateNowCheck","Audio               : " + config->toStringCfg(fieldOffsetCfgFTPUpdateAudio));
				Debug::trace("SessionManager.UpdateNowCheck","Firmware            : " + config->toStringCfg(fieldOffsetCfgFTPUpdateFirmware));
				Debug::trace("SessionManager.UpdateNowCheck","searchTime : " + updateNowSearchTime);

				if(config->toStringCfg(fieldOffsetCfgFTPUpdatePLUData) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += pluDataFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdateLabelData) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += labelDataFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateFlashkeyData) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += flashkeyDataFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateOperatorData) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += operatorDataFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdatePlaylists) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += playlistsFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateAlerts) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += alertsFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateScaleConfiguration) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += scaleConfigurationDataFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateSynchronizationConfiguration) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += synchronizationConfigurationDataFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdateStoreNameNumber) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += storeNameNumberDataFlag; 
					areAnyFlags = true;
				}
 				if(config->toStringCfg(fieldOffsetCfgFTPUpdateVideos) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += videosFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdateImages) == "1")
     			{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += imagesFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdateAudio) == "1")
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += audioFlag; 
					areAnyFlags = true;
				}
				if(config->toStringCfg(fieldOffsetCfgFTPUpdateFirmware) == "1" && (config->toStringCfg(fieldOffsetCfgFTPdownloadFirmwareOnly)== "1" || config->toStringCfg(fieldOffsetCfgFTPdownloadInstallFirmware) == "1"))
				{
					if(areAnyFlags) updateNowCommand += ",";
					updateNowCommand += firmwareFlag; 
					areAnyFlags = true;
				}

				if(userName == "") updateNowCommand += "|root|ho1bart2||";
				else updateNowCommand += "|" + userName + "|" + password + "||";

				if(config->toStringCfg(fieldOffsetCfgFTPdownloadInstallFirmware) == "1" && config->toStringCfg(fieldOffsetCfgFTPUpdateFirmware) == "1")
				{
					updateNowCommand += "installFirmwareToo";
				}
				updateNowCommand += "|1\n\"";

				// Backgrounded
				const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::SMBACKENDPORT), updateNowCommand) );
				// Blocking		
        		//string command = PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONSENDSMBACKENDMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::SMBACKENDPORT), updateNowCommand);
				
				Debug::trace("SessionManager.UpdateNowCheck", "Calling Python command: " + updateNowCommand);
					
				string connectionStatusOutput;
				PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
				Debug::trace("SessionManager.UpdateNowCheck","Response from python call is : " + connectionStatusOutput);
				}
		   }
    }

	if (config->toBoolCfg(fieldOffsetCfgGCDAutoSearch))
	{
		// Check to see if it is time to have the VersionManager look for new firmware.
		HobDateTime sTime;
		sTime.setHour(config->toIntCfg(fieldOffsetCfgGCDSearchHour) + (config->toIntCfg(fieldOffsetCfgGCDSearchAmOrPM) ? 12 : 0));
		sTime.setMinute(config->toIntCfg(fieldOffsetCfgGCDSearchMinute));
		HobString searchTime = sTime.formatTime(FormatTime_HHMM, HourFormat_AMPM);

		if (searchTime.size()) // format should be "HH:MM AM" (12 hour format with AM/PM indicator)
		{
			HobString nowString(now.formatTime(FormatTime_HHMM, HourFormat_AMPM));

			if (traceOn)
			{
				HobString msgDebug("NewGCDMediaUpdateSearchTime");
				msgDebug.append("  nowString = ").append(nowString).append("  searchTime = ").append(searchTime);
				Debug::trace("SessionManager.DailyTimers", msgDebug);
			}
			if (searchTime == nowString)
			{
				if (!sentGCDMediaUpdateMessage)
				{
					Debug::trace("SessionManager.DailyTimers", "Notifying GCDSession that it's time to check for media updates");
					MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "DailyTimers: Dispatching event notifying GCDSession it's time to check for media updates.");

					std::string gcdServer = config->toStringCfg(fieldOffsetCfgGCDMediaServerName);
					if (0 == gcdServer.length())
					{
						gcdServer = config->toStringCfg(fieldOffsetCfgGCDMediaServerIP);
					}

					GCDFtpUpdateMsg msg(gcdServer,
						config->toStringCfg(fieldOffsetCfgGCDFtpUserName),
						config->toStringCfg(fieldOffsetCfgGCDFtpPassword),
						config->toStringCfg(fieldOffsetCfgGCDFtpDirectory),
						config->toStringCfg(fieldOffsetCfgGCDPlaylistFile),
						true,
						config->toStringCfg(fieldOffsetCfgGCDDownloadProtocol));

					dispatchEvent(&msg);

					sentGCDMediaUpdateMessage = true;
				}
			}
			else if (sentGCDMediaUpdateMessage)
				sentGCDMediaUpdateMessage = false;
		}
	}


    if (config->isAutoCompressDB())
    {
        // Check to see if it is time to perform a database compress
        HobString compressTime = config->getCompressDBTime();
        if (compressTime.size()) // format should be "HH:MM AM" (12 hour format with AM/PM indicator)
        {
            HobString nowString(now.formatTime(FormatTime_HHMM, HourFormat_AMPM));

            if (compressTime == nowString)
            {
                if (!timeToDoDBCompress)
                {
                    Debug::trace("SessionManager.DailyTimers", "Time to do a database compress");
					MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "DailyTimers: Time to compress the database.");
                    timeToDoDBCompress = true;
                    requestToCompressDB_ = true;
                }
            }
            else if (timeToDoDBCompress)
                timeToDoDBCompress = false;
        }
    }

	//--------------------------
	// SOF-4769: Scheduled Merge
	//--------------------------
	if (config->toStringCfg(fieldOffsetCfgCommScheduledUseAutomaticMerges) == "1")
	{

		Debug::trace("SessionManager.ScheduledMerge", "Starting ScheduledMerge check");

		HobString scheduledMergeStartTime = config->toStringCfg(fieldOffsetCfgCommScheduledStartTime);

		HobDateTime sTime;
		sTime.setTimeParts(scheduledMergeStartTime, FormatTime_HHMM, HourFormat_AMPM);
		scheduledMergeStartTime = sTime.formatTime(FormatTime_HHMM, HourFormat_AMPM);
		bool dayIsChosen = false;
		bool doMergeNow = false;
		HobString nowString(now.formatTime(FormatTime_HHMM, HourFormat_AMPM));

		switch (dayOfWeek)
		{
			case 0: // Sunday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledSunday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Sunday matches up");
				break;

			case 1: // Monday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledMonday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Monday matches up");
				break;

			case 2: // Tuesday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledTuesday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Tuesday matches up");
				break;

			case 3: // Wednesday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledWednesday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Wednesday matches up");
				break;

			case 4: // Thursday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledThursday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Thursday matches up");
				break;

			case 5: // Friday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledFriday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Friday matches up");
				break;

			case 6: // Saturday
				if (config->toStringCfg(fieldOffsetCfgCommScheduledSaturday) == "1")
					dayIsChosen = true;
				Debug::trace("SessionManager.ScheduledMerge", "Saturday matches up");
				break;

			default:
				doMergeNow = false;
				dayIsChosen = false;
				break;
		}

		if (dayIsChosen)
		{
			// if daily check start time
			if (config->toStringCfg(fieldOffsetCfgCommScheduledDailyUpdates) == "1" && scheduledMergeStartTime == nowString)
			{
				doMergeNow = true;
				Debug::trace("SessionManager.ScheduledMerge", "daily and the right time");
			}
			// if hourly check minute is zero
			else if (config->toStringCfg(fieldOffsetCfgCommScheduledHourlyUpdates) == "1" && (int)min == 0)
			{
				doMergeNow = true;
				Debug::trace("SessionManager.ScheduledMerge", "hourly and on the hour");
			}
			else
			{
				doMergeNow = false;
				Debug::trace("SessionManager.ScheduledMerge", "No Scheduled Merge this minute");
			}
		}

		if ( doMergeNow )
		{
			checkForFilesToMerge();
		}

		Debug::trace("SessionManager.ScheduledMerge", "End of ScheduledMerge check");
	}

/* For now, just create a Session to deal with checking for a new planogram!
   Leave this code here in case we want to detect a change in the most current planogram 
   before telling a Session to act on it.

   We create a Session to deal with it so SessionManager doesn't have to deal with the database.

	if (config->toBoolCfgI(CfgInternal::PLANOGRAMSENABLED))
	{
		GraphicFileNames graphicFileNames(Graphic::TYPE_PLANOGRAMGRAPHIC);
		graphicFileNames.findFileNames(Graphic::TYPE_PLANOGRAMGRAPHIC, Graphic::SUBTYPE_USERANDSYSTEM, 0, Directory::MODIFIED_SORT);
		HobString graphicName;
		int found = graphicFileNames.last(graphicName);
		if (found >= 0)
		{
			util_filesystem::Filename planoFilename;
			planoFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			planoFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
			planoFilename.setFileName(graphicName);

			File plano(planoFilename.getFullPath());
			unsigned int planoCRC = 0;
			if (plano.exists())
				planoCRC = plano.checksum();

			string planoName = plano.getFileName();
			string dbPlanoName = config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic);

			if (config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic) != planoName ||
				config->toIntCfg(fieldOffsetCfgMostRecentPlanogramGraphicCRC) != planoCRC)
			{
				Debug::trace("SessionManager", "Detected a new planogram.  Creating a Session to update the database and inform all UI Sessions.");

				// Create a session to do this for me.
				Session *newSession = new Session();
				PAL_absclass::PAL_sleep(500);
				newSession->setSessionType(SESSIONTYPE_HELPER);
				addSession(newSession);
				newSession->setCurrentModeOp(Session::OP_PLANOGRAMPROCESSING);
			}
		}
	}
*/
	if (config->toBoolCfgI(CfgInternal::PLANOGRAMSENABLED))
	{
/*
		// Create a session to do this for me.
		Debug::trace("Planogram", "SessionManager is creating a 'helper' Session to check for a new planogram.");
		Session *newSession = new Session();
		PAL_absclass::PAL_sleep(500);
		newSession->setSessionType(SESSIONTYPE_HELPER);
		addSession(newSession);
		newSession->setCurrentModeOp(Session::OP_PLANOGRAMPROCESSING);
*/

		// The primary session is the only one listening for this device / event.
		Debug::trace("Planogram", "SessionManager is sending a message to the Primary Session to check for a new planogram.");
		EvolMessage msg(GraphicDev, CheckForNewPlanogramReceived);
		dispatchEvent(&msg);
	}

	if (config->toBoolCfgI(CfgInternal::RECALLSENABLED))
	{
		// The primary session is the only one listening for this device / event.
		Debug::trace("Recall", "SessionManager is sending a message to the Primary Session to check for unacknowledged recalls.");
		EvolMessage msg(RecallDev, CheckForRecalls);
		dispatchEvent(&msg);
	}

	// Look for the fake Service key to appear, or disappear, and if either occurs, get new KeyActivation messages sent (from UsbManager) to the Sessions.
	{
		bool needToRequestKeyActivations = false;

		util_filesystem::Directory evolutionDir("");
		evolutionDir.appendPath(config->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));
		util_filesystem::File serviceKeyFile(evolutionDir.getFullPath(), config->getServiceKeyFile());
		if ( serviceKeyFile.exists() )
		{
			if (!virtualServiceKeyPresent_)
			{
				virtualServiceKeyPresent_ = true;
				needToRequestKeyActivations = true;
			}
		}
		else
		{
			if (virtualServiceKeyPresent_)
			{
				virtualServiceKeyPresent_ = false;
				needToRequestKeyActivations = true;
			}
		}

		if (needToRequestKeyActivations)
		{
			// check for service or supervisor keys now
			EvolMessage keyActivationMsg(SessionDev, SnKeyActivation);
			dispatchEvent(&keyActivationMsg);
		}
	}

	// Remote Scale Management feature
	// Check if it is time to remove any file/s from the outbox directory using the files information and the configuration for
	// how many days files should remain in the outbox directory.

	int daysToClear = config->toIntCfgI( CfgInternal::REMOTESCALEMAINTCLROUTBOXDAYS );
	if (daysToClear > 0)
	{
		bool remoteCfgFilesDirectoryExists = true;
		std::string remoteOutboxDirStr = config->toStringCfgI( CfgInternal::DOWNLOADOUTPUTDIR );	//REMOTE_MAINTENANCE_OUTPUT_DIR;
		std::string msg;

		util_filesystem::Directory::const_iterator dirOutboxIter;
		//RemoteOutboxFilesIterator ufOutboxIter;

		util_filesystem::Directory workingOutboxDir(remoteOutboxDirStr);
		if ( !workingOutboxDir.exists() )
		{
			msg = "Remote maintenance output directory [" + remoteOutboxDirStr + "] does not exist.";
			remoteCfgFilesDirectoryExists = false;
			if( tracingRSM )
				Debug::trace("SessionManager.remoteScaleMaintenance", HobString(msg));
			MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, msg);  // log result of directory creation
		}

		if( remoteCfgFilesDirectoryExists )  	//	If directory exists process - check for old files.
		{
			util_time::DateTime cutOffDate;
			cutOffDate -= daysToClear;
			util_time::DateTime cutOffDateTime(cutOffDate);

			util_time::DateTime fileModDate;	//	Current Date/Time of the file we are looking at.
			util_time::DateTime now;

			workingOutboxDir.select("*");
			dirOutboxIter = workingOutboxDir.begin();
			if(tracingRSM)
			{
				Debug::trace("SessionManager.remoteScaleMaintenance", HobString("Starting to look for valid files in the outbox directory"));
				Debug::trace("SessionManager.remoteScaleMaintenance", HobString("The current date/time is ") + HobString(now.standardLook()));
				Debug::trace("SessionManager.remoteScaleMaintenance", HobString("The configuration max days for the outbox directory is ") + HobString(Integer::toString(daysToClear)));
			}

			std::string fileInfoStr;
			while (dirOutboxIter != workingOutboxDir.end() && remoteCfgFilesDirectoryExists)
			{
				// Go through all the files in the remote scale maintenance "outbox" directory and see if
				// there are files that have reached their configured time limit - requiring removal.

				std::string fnameOutbox = dirOutboxIter->getName();
				File remoteOutboxDirFile (workingOutboxDir.getFullPath(), fnameOutbox);
				fileModDate = remoteOutboxDirFile.getModified();
				// A file is there check if needing removal.
				long64 size = remoteOutboxDirFile.size();
				Debug::trace("SessionManager.remoteScaleMaintenance", HobString("The current filename is [") + fnameOutbox + HobString("] with a length of ") + HobString(ULong::toString((unsigned long)size)) + " bytes.");

				fileInfoStr = "[" + fnameOutbox + "] dated [" + HobString(fileModDate.standardLook()) + "]";
				if ( fileModDate < cutOffDateTime )
				{
					//	The file has exceeded the max number of days
					if( remoteOutboxDirFile.remove() )		//	Remove the file from the outbox directory
					{
						msg = "Removed";
					}
					else
					{
						msg = "Unable to remove";
					}
					msg.append(" ").append(fileInfoStr).append(".");
					if(tracingRSM)
					{
						Debug::trace("SessionManager.remoteScaleMaintenance", msg);
					}
					MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, msg);  // log result of directory creation
				}
				else
				{
					//	The file has NOT exceeded the max number of days
					if(tracingRSM)
					{
						msg = fileInfoStr + " has not reached the cutoff date.";
						Debug::trace("SessionManager.remoteScaleMaintenance", msg);
					}
				}
				++dirOutboxIter;
			}
		}
		if(tracingRSM)
			Debug::trace("SessionManager.remoteScaleMaintenance", HobString("Finished looking for outdated files in the outbox directory"));
	}

	// Check if its time to adjust the log files because they are too long
	// Check log files shutdown.dat, scheduledEvents.log, dataMaintenance.log, scaleEvents.log, info.log, warnings.log, errors.log
	long64 maxFileSize = config->toIntCfgI( CfgInternal::MESSAGELOGGINGMAXFILESIZE );
	if(tracingRSM)
		Debug::trace("SessionManager.remoteScaleMaintenance", HobString("Checking log files for a max size of ") + int64tostring(maxFileSize) + " bytes.");
	if ( maxFileSize > 0 )
	{
		MessageLogger::LogFileIndicator currentLogIndicator = MessageLogger::InfoLog;
		MessageLogger::LogFileIndicator	nextLogIndicator;
		std::string			workingDirectory, currentLogFile, rotatedLogFile;

		util_filesystem::Directory workingDir(config->toStringCfgI( CfgInternal::MESSAGELOGGINGDIRECTORY ));
		if(tracingRSM)
			Debug::trace("SessionManager.remoteScaleMaintenance", HobString("MESSAGELOGGINGDIRECTORY: [") + workingDir.getFullPath() + "]");
		while( currentLogIndicator != MessageLogger::endLogFileIndicator )
		{
			switch( currentLogIndicator )
			{
				case MessageLogger::InfoLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGINFOFILENAME );
					nextLogIndicator = MessageLogger::WarningsLog;
					break;
				}
				case MessageLogger::WarningsLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGWARNINGSFILENAME );
					nextLogIndicator = MessageLogger::ErrorsLog;
					break;
				}
				case MessageLogger::ErrorsLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGERRORSFILENAME );
					nextLogIndicator = MessageLogger::StartupShutdownLog;
					break;
				}
				case MessageLogger::StartupShutdownLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGSTARTUPSHUTDOWNFILENAME );
					nextLogIndicator = MessageLogger::ScaleMaintenanceLog;
					break;
				}
				case MessageLogger::ScaleMaintenanceLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGDATAMAINTENANCEFILENAME );
					nextLogIndicator = MessageLogger::ScaleEventsLog;
					break;
				}
				case MessageLogger::ScaleEventsLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGSCALEEVENTSFILENAME );
					nextLogIndicator = MessageLogger::ScheduledEventsLog;
					break;
				}
				case MessageLogger::ScheduledEventsLog:
				{
					currentLogFile = config->toStringCfgI( CfgInternal::MESSAGELOGGINGSCHEDULEDEVENTSFILENAME );
					nextLogIndicator = MessageLogger::endLogFileIndicator;
					break;
				}
				default:
				{
					currentLogFile = "";
					nextLogIndicator = MessageLogger::endLogFileIndicator;
					break;
				}
			} // end of switch

			if( currentLogFile != "" )
			{
				File logFile(workingDir.getFullPath(), currentLogFile);
				if(tracingRSM && logFile.exists())
					Debug::trace("SessionManager.remoteScaleMaintenance", HobString("[") + currentLogFile + "] is " + int64tostring(logFile.size()) + " bytes.");

				if( logFile.exists() && (logFile.size() >= maxFileSize) )
				{
					//	Need to move the current log file to the old/rotated log file.
					rotatedLogFile = logFile.getFileNameBase() + "_old";
					File rotatedLog(workingDir.getFullPath(), rotatedLogFile);
					rotatedLog.setFileNameExtension(logFile.getFileNameExtension());
					if(tracingRSM)
						Debug::trace("SessionManager.remoteScaleMaintenance", HobString("rotatedLogFile = [") + rotatedLog.getFullPath() + "]");

					if( rotatedLog.exists() )
						rotatedLog.remove();		//	Get rid of the current rotated file.

					//	Move the current log file to the rotated log file
					std::string	msg = " " + currentLogFile + " to " + rotatedLogFile + ".";
					if( logFile.moveTo(rotatedLog) )
					{
						msg = "Moved log file" + msg;
					}
					else
					{
						//	Not sure what is wrong. Remove the logFile and start over.
						logFile.remove();
						msg = "Unable to move" + msg + " It has been deleted.";
						// what else!!!
					}
					MessageLogger::logMessage(currentLogIndicator, msg);
				}
			}
			currentLogIndicator = nextLogIndicator;		//	Set up the next log indicator.
			
		} // end of while
	}

	//--------------------------------
	// HT-326 Check for RDP Connection
	//--------------------------------
	
	const string command(PythonScripter::buildPythonCall("checkForRDP.py"));
	string checkForRdpResult;
	Debug::trace("SessionManager.DailyTimers", " - Checking for RDP connection, command: " + command);
	int rc(PAL_absclass::PAL_executeWithResults(command, checkForRdpResult));
	Debug::trace("SessionManager.DailyTimers", " - output received from checkForRDP.py: " + Integer::toString(rc) + string(" : ") + checkForRdpResult);

	if (rc == 1)
	{
		// Send message to UI that there is an RDP connection
		Debug::trace("SessionManager.DailyTimers", " - Sending message that RDP is connnected");
		EvolMessage rdpActiveMsg(SessionDev, SnRdpActive);
		dispatchEvent(&rdpActiveMsg);
	}
	else
	{
		// Send message to UI that there is NOT an RDP connection
		Debug::trace("SessionManager.DailyTimers", " - Sending message that RDP is NOT connnected");
		EvolMessage rdpInActiveMsg(SessionDev, SnRdpInactive);
		dispatchEvent(&rdpInActiveMsg);
	} 

#if defined( WIN32 )
    //Check if daylight savings time switch has occurred
    CheckIfDaylightSavingsSwitched();
#endif
}


void SessionManager::performSynchronizationRequest()
{
	if ( requestToInitializeSynchronization_ || requestToTerminateSynchronization_ || requestToChangeSynchronizationType_ || requestToChangePrimaryScale_ )
	{
		HobDateTime now;
		Debug::trace("SessionManager.sync", "It is " + string(now.toString()) + ", will now try to process a sync request.");

		if ( doesSessionExist(AbstractSession::SESSIONTYPE_HELPER_AUTOBACKUPDB) )
		{
			Debug::trace("SessionManager.sync", "Another Session is already processing an auto-backup task.  Will try the sync request later.");
		}
		else if ( dbIndDataExists || doesSessionExist(AbstractSession::SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES) )
		{
			Debug::trace("SessionManager.sync", "Database is being restore, or another Session is processing downloaded files.  Will try the sync request later.");
		}
		else if ( doesSessionExist(AbstractSession::SESSIONTYPE_HELPER) )
		{
			Debug::trace("SessionManager.sync", "A helper Session is running.  Will try the sync request later.");
		}
		else
		{
			Debug::trace("SessionManager.sync", "Nothing is blocking the processing of this sync request.  Will process it now.");

			if (requestToInitializeSynchronization_)
			{
				Debug::trace("SessionManager.sync", "Processing InitializeSynchronization ...");
				requestToInitializeSynchronization_ = false;
				usecaseBootingUp();
			}

			if (requestToChangeSynchronizationType_)
			{
				Debug::trace("SessionManager.sync", "Processing ChangeSynchronizationType ...");
				requestToChangeSynchronizationType_ = false;
				if (requestedSyncType_ != ScaleSynchronizationTypeInvalid)
				{
					usecaseChangingSynchronizationType(requestedSyncType_, requestedSyncTypePrimaryScale_);

					requestedSyncType_ = ScaleSynchronizationTypeInvalid;
					requestedSyncTypePrimaryScale_ = "";
				}
			}

			if (requestToChangePrimaryScale_)
			{
				Debug::trace("SessionManager.sync", "Processing ChangePrimaryScale ...");
				requestToChangePrimaryScale_ = false;

				usecaseChangingPrimaryScale(requestedPrimaryScale_);

				requestedPrimaryScale_ = "";
			}

			if (requestToTerminateSynchronization_)
			{
				Debug::trace("SessionManager.sync", "Processing TerminateSynchronization ...");
				requestToTerminateSynchronization_ = false;
				processShutdownSynchronization();
			}
		}
	}
}


void SessionManager::performAutoBackupDB()
{
	HobDateTime now;
	Debug::trace("AutoBackupDatabase", "It is " + string(now.toString()) + ", time to auto-backup the database to .ht files.");

	if ( doesSessionExist( AbstractSession::SESSIONTYPE_HELPER_AUTOBACKUPDB) )
	{
		Debug::trace("AutoBackupDatabase", "Another Session is already processing an auto-backup task.  Skipping this request.");
		requestToPerformAutoBackupDB_ = false;
	}
	else if ( dbIndDataExists || doesSessionExist(AbstractSession::SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES) )
	{
		Debug::trace("AutoBackupDatabase", "Database is being restore, or another Session is processing downloaded files.  Wait to process the auto-backup until it's finished.");
	}
	else
	{
		requestToPerformAutoBackupDB_ = false;

		// Create a Session to do this for me.
		Debug::trace("AutoBackupDatabase", "SessionManager is creating a 'helper' Session to auto-backup the database to .ht files.");
		Session *newSession = new Session();
		PAL_absclass::PAL_sleep(500);
		newSession->setSessionType(AbstractSession::SESSIONTYPE_HELPER_AUTOBACKUPDB);
		addSession(newSession);
		newSession->setCurrentModeOp(Session::OP_AUTOBACKUPDATABASE);
	}
}


void SessionManager::lookForDownloadedFiles(std::string dirPathStr, std::string fileSpecifiersStr)
{
	bool tracing = Debug::isTracing("SessionManager.download");
	if (tracing)
	{
//		Debug::trace("SessionManager.download", "lookForDownloadedFiles([" + dirPathStr + "], [" + fileSpecifiersStr + "])");
		Debug::trace("SessionManager.download", "lookForDownloadedFiles():  availableFiles.size() = " + util_string::inttostring(availableFiles.size()));
	}

	if (!dirPathStr.empty())
	{
		util_filesystem::Directory downloadDir("");
		downloadDir.appendPath(dirPathStr);
		if (downloadDir.exists())
		{
			string downloadFileSpecifiers = fileSpecifiersStr;
			if (downloadFileSpecifiers.size() == 0)
				downloadFileSpecifiers = "*";
//			if (tracing)
//				Debug::trace("SessionManager.download", "lookForDownloadedFiles():  downloadFileSpecifiers = [" + downloadFileSpecifiers + "]");

			unsigned int count = 0;
			HobTokenizerCSV tok(&downloadFileSpecifiers, BrowserFormatSeparator);
			HobString token;
			while (tok.next(&token) != -1)
			{
				util_filesystem::Directory::getAllAvailableFiles(dirPathStr, &availableFiles, token);
				if (!availableFiles.empty() && (count < availableFiles.size()))
				{
					if (tracing)
						Debug::trace("SessionManager.download", "lookForDownloadedFiles():  Found " + util_string::inttostring(availableFiles.size() - count) + " file(s) with extension: " + token);
					count = (unsigned int) availableFiles.size();
				}
			}
		}
	}

	if (tracing)
		Debug::trace("SessionManager.download", "lookForDownloadedFiles():  End");
}


void SessionManager::checkForDownloadedFiles()
{
	bool tracing = Debug::isTracing("SessionManager.download");

    Configuration *config = Configuration::getInstance();
    if ( !config ) return;

	if (tracing)
		Debug::trace("SessionManager.download", "checkForDownloadedFiles():  Start");

	availableFiles.clear();

	// Look for a download file that shows up in the local directory where we look for automatic downloads.
	std::string downloadDirStr = config->toStringCfgI(CfgInternal::DOWNLOADDIR);

	std::string downloadFileSpecifiers = config->toStringCfgI(CfgInternal::DOWNLOADFILESPECIFIERS);
	lookForDownloadedFiles(downloadDirStr, downloadFileSpecifiers);

	if (availableFiles.empty())		//	Let potential upgrade files take priority
	{
		//	Now check for any Graphic files. These would be in subdirectories of the download
		downloadFileSpecifiers = config->toStringCfgI(CfgInternal::GRAPHICFILESPECIFIERS);		//	TODO: Check if we should use generic or specific to the type?
		lookForDownloadedFiles(downloadDirStr, downloadFileSpecifiers);
	}

	prepareToProcessDownloadedFiles();
	
	if (tracing)
		Debug::trace("SessionManager.download", "checkForDownloadedFiles():  End");
}


void SessionManager::prepareToProcessDownloadedFiles()
{
	if (availableFiles.empty())
		return;

    Configuration *config = Configuration::getInstance();
    if ( !config ) return;

	bool tracing = Debug::isTracing("SessionManager.download");
	bool downloadFilesChanged = false;
	util_filesystem::Directory::DirectoryFileListIterator dfIter;

	//	Now we have a list of available files in the download directory
	util_filesystem::Directory::DirectoryFileListIterator availIter = availableFiles.begin();
	while (availIter != availableFiles.end())
	{
		// Go through all the files in the download directory and see if we are tracking them, in the filesInProcess map.

		std::string fname = availIter->first;
		File downloadDirFile (fname);

		dfIter = filesInProcess.find(fname);
		if (dfIter == filesInProcess.end())
		{
			// not yet in list, so add it
			filesInProcess.insert(make_pair(fname, (unsigned int) downloadDirFile.size()));
			downloadFilesChanged = true;
			if (tracing)
				Debug::trace("SessionManager.download", HobString("Found a new download file ") + fname + HobString("  size is ") + HobString(Long::toString((long)downloadDirFile.size())));
		}
		else if (availIter->second != downloadDirFile.size())
		{
			// already in list, check file size; if different, update it.
			availIter->second = (unsigned int) downloadDirFile.size();
			downloadFilesChanged = true;
			if (tracing)
				Debug::trace("SessionManager.download", HobString("Updating existing download file ") + fname + HobString("  size is ") + HobString(Long::toString((long)(dfIter->second))));
		}

		++availIter;
	}

	if ( downloadFilesChanged || doesSessionExist(AbstractSession::SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES) || doesSessionExist(AbstractSession::SESSIONTYPE_HELPER_AUTOBACKUPDB) )
	{
		// Create a timer that is shorter than one minute, to keep looking for download files showing up and/or their file size changing (so it's still downloading, but not complete yet)
		if (timerManager)
		{
			int timeval = config->toIntCfgI(CfgInternal::DOWNLOADCHECKINGUPDATETIMER);
			if (timeval <= 0)
				timeval = 30;
			if (tracing)
				Debug::trace("SessionManager.download", "creating a timer to continue looking for downloaded files, timeval=" + inttostring(timeval));
			timerManager->addTimer(timeval, Timer::RepeatNO, timedFuncDownloadFilesCheck);
		}
	}
	else
	{
		// The files in the download directory are now here.  We have no new files that have shown up, and none of the files that are
		// in the download directory have changed in their file size.  Also, we don't already have a Session that is processing downloaded files
		//    (if we did, we will wait until it is finished with the files currently in the 'pending' directory so we don't
		//     let it start processingone of these new files without it being completely copied to the 'pending' directory),
		// so move these files to the 'pending' directory, and create a Session to process them.
		if (tracing)
			Debug::trace("SessionManager.download", HobString("Download files exist, and none have changed. Create a Session to process them: fileList size is ") + HobString(Integer::toString((int)filesInProcess.size())));

		// Move the files from the download directory to a 'processing' directory, but only move those that are specified
		// in the 'filesInProcess' map, since we know these are completely downloaded.
		std::string downloadDirStr = config->toStringCfgI(CfgInternal::DOWNLOADDIR);
		std::string processingDirStr = config->toStringCfgI(CfgInternal::DOWNLOADPROCESSINGDIR);
		if (processingDirStr.size())
		{
			// Get the filenames of the files from the 'filesInProcess' map, and move them to the processing directory.
			dfIter = filesInProcess.begin();
			while (dfIter != filesInProcess.end())
			{
				// move the files
				File downloadFile (dfIter->first);		//	dfIter->first is the complete path and filename.
				if (downloadFile.exists())
				{
					std::string filenameStr = dfIter->first;
					if (tracing)
						Debug::trace("SessionManager.download", HobString("filenameStr = ") + filenameStr);

					//	Remove the download path from filenameStr to get the relative path
					if (filenameStr.find(downloadDirStr) == 0)
						filenameStr.erase(0, downloadDirStr.length());
					if (tracing)
						Debug::trace("SessionManager.download", HobString("Now filenameStr = ") + filenameStr);

					File processingFile(processingDirStr + filenameStr);	//	Check the directory exists first

					util_filesystem::Directory processingDir("");
					processingDir.appendPath(processingFile.getPath());
					if (tracing)
						Debug::trace("SessionManager.download", HobString("processingDir = ") + processingDir.getFullPath());

					if (!processingDir.exists())
					{
						if (tracing)
							Debug::trace("SessionManager.download", HobString("Creating processingDir: ") + processingDir.getFullPath());

						std::string msg("Remote maintenance working directory did not exist");
						if (processingDir.create())
						{
#if defined ( __linux__ )
							// Need to make sure that Linux version creates this directory with owner group set to root:root and security set to 775

							std::string systemStr = "sudo chmod -R 775 ";
							systemStr.append(processingDirStr);
							int result = system(systemStr.c_str());

							PAL_absclass::PAL_sleep(1000);

							std::string systemStr1 = "sudo chown -R root:root ";
							systemStr1.append(processingDirStr);
							int result1 = system(systemStr1.c_str());

							if(result == -1 || result1 == -1)
							{
								msg.append(", and could not be created");
							}
							else
							{
								msg.append(".  Created:");
//								processingFilesDirectoryExists = true;
							}
							msg.append(" [").append(processingDir.getFullPath()).append("].  result= ").append(util_string::inttostring(result)).append(", result1= ").append(util_string::inttostring(result1));
#else
							msg.append(".  Created: [").append(processingDir.getFullPath()).append("]");
//							processingFilesDirectoryExists = true;
#endif
						}
						else
						{
							// add a entry into the scheduledEvents.log 
							msg.append(". Failed to create directory: ").append(processingDir.getFullPath());
						}
						if( tracing )
							Debug::trace("SessionManager.download", HobString(msg));
						MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, msg);  // log result of directory creation
					}
					if (!processingDir.exists())
					{
						if (tracing)
							Debug::trace("SessionManager.download", "Could not create the directory for processing the download files!");
					}
					else
					{
						if (tracing)
							Debug::trace("SessionManager.download", "Moving file from " + downloadFile.getFullPath() + " to " + processingFile.getFullPath());
						downloadFile.moveTo(processingFile);
					}
				}
				++dfIter;
			}
		}

		// Clear the filesInProcess map.  We have received all of the files that will be processed in this download.
		filesInProcess.clear();

		processDownloadedFiles();
	}
}

void SessionManager::processDownloadedFiles()
{
	// Create a session to process the downloaded files, instead of burdening the Primary Console Session
	Debug::trace("SessionManager.download", "SessionManager is creating a 'helper' Session to process the downloaded files.");

	Session *newSession = new Session();
	PAL_absclass::PAL_sleep(500);
	newSession->setSessionType(AbstractSession::SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES);
	addSession(newSession);
	newSession->setCurrentModeOp(Session::OP_PROCESSDOWNLOADEDFILES);

	// Create a message to be sent to the primary Session, the one that actually handles the processing of the downloaded files.
	// NOTE:  Could create a helper Session to deal with them so we don't bother the primary Session.
	//Debug::trace("SessionManager", "SessionManager is sending a message to the Primary Session to process downloaded files.");
	//EvolMessage msg(SessionDev, SnProcessDownloadedFiles);
	//dispatchEvent(&msg);
}

// SOF-4769: Scheduled Merge
void SessionManager::checkForFilesToMerge()
{
	//------------------------------------------------------------------------------------
	// Copy the contents of the 'merge' directory to the 'download' directory on the scale
	//------------------------------------------------------------------------------------

	Configuration *config = Configuration::getInstance();
	if (!config)
		
		return;

	bool tracing = Debug::isTracing("SessionManager.ScheduledMerge");

	if (tracing)
		Debug::trace("SessionManager.ScheduledMerge", "Starting scheduled merge ...");

	availableFiles.clear();
	util_filesystem::Directory::DirectoryFileListIterator dfIter;

	// Look for file(s) in the local 'merge' directory
	std::string mergeDirStr = config->toStringCfgI(CfgInternal::MERGEDIR);
	std::string mergeFileSpecifiers = config->toStringCfgI(CfgInternal::MERGEFILESPECIFIERS);

	// Re-using this function to look in the 'merge' directory ...
	lookForDownloadedFiles(mergeDirStr, mergeFileSpecifiers);

	if (!availableFiles.empty())		//	Let potential upgrade files take priority
	{
		util_filesystem::Directory::DirectoryFileListIterator dfIter;

		//	Now we have a list of available files in the merge directory
		util_filesystem::Directory::DirectoryFileListIterator availIter = availableFiles.begin();
		while (availIter != availableFiles.end())
		{
			// Go through all the files in the merge directory and see if we are tracking them, in the filesInMergeProcess map.

			std::string fname = availIter->first;
			File mergeDirFile(fname);

			dfIter = filesInMergeProcess.find(fname);
			if (dfIter == filesInMergeProcess.end())
			{
				// not yet in list, so add it
				filesInMergeProcess.insert(make_pair(fname, (unsigned int)mergeDirFile.size()));

				if (tracing)
					Debug::trace("SessionManager.ScheduledMerge", HobString("Found a new merge file ") + fname + HobString("  size is ") + HobString(Long::toString((long)mergeDirFile.size())));
			}
			else if (availIter->second != mergeDirFile.size())
			{
				// already in list, check file size; if different, update it.
				availIter->second = (unsigned int)mergeDirFile.size();

				if (tracing)
					Debug::trace("SessionManager.ScheduledMerge", HobString("Updating existing merge file ") + fname + HobString("  size is ") + HobString(Long::toString((long)(dfIter->second))));
			}

			++availIter;
		}
	}

		// The files in the merge directory are now here.  We have no new files that have shown up, and none of the files that are
		// in the merge directory have changed in their file size.  Now we can copy them to the 'download' directory.

		if (tracing)
			Debug::trace("SessionManager.ScheduledMerge", HobString("Merge files exist??? ... none have changed. The fileList size is ") + HobString(Integer::toString((int)filesInMergeProcess.size())));

		// Copy the files from the 'merge' directory to a 'download' directory, but only copy those that are specified
		// in the 'filesInMergeProcess' map, since we know these are completely downloaded.

		std::string processingDirStr = config->toStringCfgI(CfgInternal::DOWNLOADDIR);

		if (processingDirStr.size())
		{
			// Get the filenames of the files from the 'filesInMergeProcess' map, and copy them to the download directory.
			dfIter = filesInMergeProcess.begin();

			while (dfIter != filesInMergeProcess.end())
			{
				// copy the files
				File mergeFile(dfIter->first);		//	dfIter->first is the complete path and filename.

				if (mergeFile.exists())
				{
					std::string filenameStr = dfIter->first;
					if (tracing)
						Debug::trace("SessionManager.ScheduledMerge", HobString("filenameStr = ") + filenameStr);

					//	Remove the download path from filenameStr to get the relative path
					if (filenameStr.find(mergeDirStr) == 0)
						filenameStr.erase(0, mergeDirStr.length());
					if (tracing)
						Debug::trace("SessionManager.ScheduledMerge", HobString("Now filenameStr = ") + filenameStr);

					File processingFile(processingDirStr + filenameStr);	//	Check the directory exists first

					util_filesystem::Directory processingDir("");
					processingDir.appendPath(processingFile.getPath());

					if (tracing)
						Debug::trace("SessionManager.ScheduledMerge", HobString("processingDir = ") + processingDir.getFullPath());

					if (tracing)
						Debug::trace("SessionManager.ScheduledMerge", "Copying file from " + mergeFile.getFullPath() + " to " + processingFile.getFullPath());

					mergeFile.copyTo(processingFile);
				}

				++dfIter;
			}

			//--------------------------------------------
			// Record the date/time of the merge in config
			//--------------------------------------------
			HobDateTime mergeSessionStart;
			HobString dateTime = mergeSessionStart.convertToDBFormat();
			DBAccess dba;
			dba.initDB();

			Debug::trace("SessionManager.ScheduledMerge", HobString("Saving date/time for Merge History into Config db: ") + dateTime);
			Configuration::getInstance()->setMergeHistory(dateTime);
			Configuration::getInstance()->save(&dba);

		}

		// Clear the filesInMergeProcess map.  We have received all of the files that will be processed
		filesInMergeProcess.clear();

	if (tracing)
		Debug::trace("SessionManager.ScheduledMerge", "End of scheduled merge processing");
}

// SOF-4769: Scheduled Merge
void SessionManager::checkForFilesToMergeAfterComm()
{
	//-----------------------------------------------------
	// SOF-4769: Execute the merge after 'Comm' is complete
	//-----------------------------------------------------

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	bool tracing = Debug::isTracing("SessionManager.ScheduledMerge");

	if (tracing)
		Debug::trace("SessionManager.ScheduledMerge", "Checking if merge after Comm???");


	// Only if configured ...
	if (config->toStringCfg(fieldOffsetCfgCommScheduledExecMergeAfterComm) == "1")
	{
		if (tracing)
			Debug::trace("SessionManager.ScheduledMerge", "Executing merge after Comm ...");

		checkForFilesToMerge();
	}
}

#if defined( WIN32 )
void SessionManager::CheckIfDaylightSavingsSwitched()
{
    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;

    TIME_ZONE_INFORMATION tzi_current;
    HANDLE dst_filehandle = NULL;
    DWORD len = 0;
    int nResult = 0;

    //grab the time zone information, will be either TIME_ZONE_ID_DAYLIGHT, TIME_ZONE_ID_STANDARD, or TIME_ZOND_ID_UNKNOWN
    ctz_stat = GetTimeZoneInformation(&tzi_current);

    // TIME_ZONE_ID_UNKNOWN indicates DST is not used in the current time zone
    // because there are no transition dates. Nothing to do so return.
    if (TIME_ZONE_ID_UNKNOWN == ctz_stat)
        return;

    //If we haven't read in the saved daylight savings information from C:\Program Files\Hobart\Access Scale\DST.txt,
    //do it now!
    if ( ctz_stat_on_file < 0 )
    {
        dst_filehandle = CreateFileA(DAYLIGHT_SAVINGS_FILE,FILE_ALL_ACCESS,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

        if (INVALID_HANDLE_VALUE == dst_filehandle)
        {
            if (traceOn)
                Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Failed to create or open DST persistence file\n");
            return;
        }

        if (ERROR_ALREADY_EXISTS != GetLastError())
        {
            // File didn't exist. Create it and exit.
            HobString temp = Integer::toString(ctz_stat);
            nResult = WriteFile(dst_filehandle, temp.c_str(), (DWORD)temp.length(), &len, NULL);
            CloseHandle(dst_filehandle);
            if (0 == nResult)
            {
                if (traceOn)
                    Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()" ,"Failed to write first time to DST persistence file\n");
                return; 
            }
            else
            {
                if (traceOn)
                    Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Successfully created DST persistence file for first time\n");
                ctz_stat_on_file = ctz_stat;
                return;
            }
        }

        char ctz_string_from_file[1];

        // Read the persistent DST info in the file.
        nResult = ReadFile(dst_filehandle, (LPVOID)ctz_string_from_file, 1, &len, NULL);

        if (0 == nResult)
        {
            if (traceOn)
                Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "failed reading the contents of the DST persistence file\n");
            CloseHandle(dst_filehandle);
            return;
        }
        else
        {
            ctz_stat_on_file = atoi((char*)ctz_string_from_file);

            if (ctz_stat_on_file < 0 && ctz_stat_on_file > 2) //bad number, default it to what API gave us
            {
                if (traceOn)
                    Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Read in bad number from DST.txt!!");
                ctz_stat_on_file = ctz_stat;
                HobString temp = Integer::toString(ctz_stat);
                nResult = WriteFile(dst_filehandle, temp.c_str(), (DWORD)temp.length(), &len, NULL);
            }
        }
    }

    // Compare the first char in the file
    if (ctz_stat == ctz_stat_on_file )
    {
        // Identical, no daylight savings time status change, so just return
        if (dst_filehandle)
            CloseHandle(dst_filehandle);
        return;
    }
    else
    {
        // Changed. Update dst.txt and ctz_stat_on_file with updated time zone information

        //if we dont have the file handle, get it!
        if (!dst_filehandle)
            dst_filehandle = CreateFileA(DAYLIGHT_SAVINGS_FILE,FILE_ALL_ACCESS,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

        // Reset file pointer to begin of file.
        nResult = SetFilePointer(dst_filehandle,(LONG)0,NULL,FILE_BEGIN);
        // nResult should be the new filepointer
        if (0 != nResult)
        {
            if (traceOn)
                Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Failed to reset file pointer\n");
            return;
        }

        HobString temp = Integer::toString(ctz_stat);
        nResult = WriteFile(dst_filehandle, temp.c_str(), (DWORD)temp.length(), &len, NULL);

        CloseHandle(dst_filehandle);

        if (0 == nResult)
        {
            if (traceOn)
                Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Failed to write the change to DST.txt\n");
            return;
        }
        else
        {
            if (traceOn)
                Debug::trace("SessionManager::CheckIfDaylightSavingsSwitched()", "Detected Daylight Savings Time change\n");

            //also here copy ctz_stat to ctz_stat_on_file, since we changed the file
            ctz_stat_on_file = ctz_stat;

            //now do a commit on c:\windows\system32\config\system to update daylight savings settings in registry
            //this will eliminate the adding of 1 hour every time the scale reboots when File based write filter is on!

            std::string argumentList("/commit c: \\windows\\system32\\config\\system");
            HINSTANCE res = ShellExecuteA (NULL,"open","fbwfmgr.exe",argumentList.c_str(),"",SW_SHOWMINNOACTIVE);

            if (traceOn)
                Debug::trace("SessionManager:", HobString("Save daylight savings registry changes - res:") + HobString(Hex::toString((long long)res)) );

            return;
        }
    }
}
#endif

bool SessionManager::isGCDAttached()
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return ( GCDNoGCD != mgr->getCurrentList().getGCD() );
}

bool SessionManager::isWrapperAttached()
{
    bool isAttached = false;

	if ( primaryConsoleSession )
	{
		PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

		if ( mgr )
			isAttached = mgr->getCurrentList().isWrapperAttached();
	}

    if (Debug::isTracing("SessionManager.Wrapper"))
    {
        HobString msgDebug("isWrapperAttached = ");
        msgDebug.append(Bool::toString(isAttached));
        Debug::trace("SessionManager.Wrapper", msgDebug);
    }
    return isAttached;
}


bool SessionManager::isEvolutionWrapperAttached()
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

    return mgr->getCurrentList().isEvolutionWrapperAttached();
}

bool SessionManager::isCLAEPSAC2Attached()
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return mgr->getCurrentList().isCLAEPSAC2Attached();
}


#ifdef SUPPORT_ULTIMA_WRAPPERS
bool SessionManager::isUltimaWrapperAttached()
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

    return mgr->getCurrentList().isUltimaWrapperAttached();
}
#endif

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
WeigherKind SessionManager::getFirstWeigher()
{
	if ( !primaryConsoleSession )
		return WgNoWeigher;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return WgNoWeigher;

	return mgr->getCurrentList().getFirstWeigher();
}

//METSIG?? NO
bool SessionManager::isWeigherAttached()
{
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED))
	{
		Configuration::getInstance()->setWeigherModel( (WeigherModel) Configuration::getInstance()->toIntCfgI("fakeWeigherModel") );
		return true;
	}

	if ( !Configuration::getInstance()->isWeigherAttached() )
		return false;
		
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return (( WgNoWeigher != mgr->getCurrentList().getFirstWeigher() ) || ( WgNoWeigher != mgr->getCurrentList().getSecondWeigher() ));
}

bool SessionManager::isApcWeigherAttached()
{
	if ( !Configuration::getInstance()->isWeigherAttached() )
		return false;
		
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return (( WgUsbApc == mgr->getCurrentList().getFirstWeigher() ) || ( WgUsbApc == mgr->getCurrentList().getSecondWeigher() ));
}

bool SessionManager::isKpcWeigherAttached()
{
    return (Configuration::getInstance()->isWeigherAttached() && primaryConsoleSession && primaryConsoleSession->getPeripheralManager() &&
            (
             (primaryConsoleSession->getPeripheralManager()->getCurrentList().getFirstWeigher() == WgCanKpc) ||
             (primaryConsoleSession->getPeripheralManager()->getCurrentList().getSecondWeigher() == WgCanKpc)
            )
           );
}

bool SessionManager::isAtmelWeigherAttached()
{
	if ( !Configuration::getInstance()->isWeigherAttached() )
		return false;
		
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return (( WgUsbAWS == mgr->getCurrentList().getFirstWeigher() ) || ( WgUsbAWS == mgr->getCurrentList().getSecondWeigher() ));
}

bool SessionManager::isEzUsbWeigherAttached()
{
	if ( !Configuration::getInstance()->isWeigherAttached() )
		return false;
		
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return (( WgUsbEvol == mgr->getCurrentList().getFirstWeigher() ) || ( WgUsbEvol == mgr->getCurrentList().getSecondWeigher() ));
}


bool SessionManager::isGCDOperational(void)	// GCDR-183
{
	return (gcdSession ? gcdSession->getGCDOperational() : false);
}

bool SessionManager::isCustomerDisplayAttached()		//	GCDR-158
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return mgr->getCurrentList().isCustomerDisplayAttached();
}

bool SessionManager::isPrinterAttached(PrinterType printerNumber)
{
	if ( !primaryConsoleSession )
		return false;

	PeripheralManager *mgr( primaryConsoleSession->getPeripheralManager() );

	if ( !mgr )
		return false;

	return mgr->getCurrentList().isPrinterAttached( printerNumber );
}

bool SessionManager::isPrinterStreamable(PrinterType whichPrinter)
{
	Util::CLockCriticalSection lock( sessionManagerCS );
	bool result = false;
	if ( primaryConsoleSession )
	{
		Device *printerDevice = Device::findDevice(Printer, whichPrinter, primaryConsoleSession);
		if ( printerDevice )
		{
			if ( printerDevice->isANonHobartLabelPrinter() )
				result = true;
			delete printerDevice;
			printerDevice = 0;
		}
	}
	return result;
}


bool SessionManager::applyTareSetback()
{
    OpModeEvent theMode = (primaryConsoleSession ? primaryConsoleSession->getCurrentMode() : OpModeIdle);
    bool isWrapping = false;
    if ((theMode == OpModeAutoWeigh) && isWrapperAttached())
        isWrapping = true;

    if (Debug::isTracing("SessionManager.Wrapper"))
    {
        HobString msgDebug("applyTareSetback = ");
        msgDebug.append(Bool::toString(isWrapping)).append("  currentMode = ").append(Integer::toString(theMode));
        Debug::trace("SessionManager.Wrapper", msgDebug);
    }
    return isWrapping;
}

bool SessionManager::commSessionExists()
{
    if (!remoteSessions.empty())
    {
        list<AbstractSession *>::iterator pos;
        pos = remoteSessions.begin();
        while (pos != remoteSessions.end())
        {
            if ((*pos)->getSessionType() == Session::SESSIONTYPE_COMM)
                return true;
            pos++;
        }
    }
    return false;
}

bool SessionManager::remoteSessionExists()
{
    if (!remoteSessions.empty())
    {
        list<AbstractSession *>::iterator pos;
        pos = remoteSessions.begin();
        while (pos != remoteSessions.end())
        {
            if ((*pos)->getSessionType() == Session::SESSIONTYPE_REMOTE)
                return true;
            pos++;
        }
    }
    return false;
}


synchronization::SynchronizationManagerProxy* SessionManager::getSyncManagerProxy() const
{
	Debug::trace( "SynchronizationManager", "SessionManager::getSyncManagerProxy()  synchronizationManager_ * " + util_string::ptrtostring(synchronizationManager_) );
   if ( synchronizationManager_ )
   {
		Debug::trace( "SynchronizationManager", "SessionManager::getSyncManagerProxy()  proxy * " + util_string::ptrtostring(synchronizationManager_->getProxy()) );
        return synchronizationManager_->getProxy();
   }

 	Debug::trace( "SynchronizationManager", "SessionManager::getSyncManagerProxy()  returning NULL * " );
   return 0;
}


/* Primary/Secondary feature: "Use Case 1.1: Booting up" */
int SessionManager::usecaseBootingUp()
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Booting up: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
    {
        /* (uc1.1) Execute "Inquiring Network Connectivity state" use case, receiving On-line/Off-line
            state and setting the Scale Online flag with that value. */
        //          EvolMessage requestCurrentStatus(NetworkConnectivityDev, RequestCurrentStatus);
        //          Util::CSmartPtr<EvolMessage> responseMessage = NetworkConnectivityManager::getInstance()->
        //                  processEventExternal(Util::CSmartPtr<EvolMessageWrapper>(new EvolMessageWrapper(&requestCurrentStatus)));
        //          scaleOnline_ = (responseMessage->getEventType() == GoodComm);
        /* We do not need to inquire for the current state. We will start with what we already know,
            which may simply be "don't know". Unless we know we are online, the SynchronizationManager
            will start as offline and will transition to online whenever we know we are online. */
        //          scaleOnline_ = NetworkConnectivityManager::getInstance()->checkNetworkAvailability();


        /* (uc1.1) Read the database configuration to determine the current value of the Scale Type
            configuration element. If it is Stand Alone, then end the execution of this use case. */
        ScaleSynchronizationType scaleSynchronizationType = Configuration::getInstance()->getScaleSynchronizationType();
        if ( scaleSynchronizationType == ScaleSynchronizationTypeStandAlone )
        { status = _statusOK_; break; }
        if ( scaleSynchronizationType != ScaleSynchronizationTypePrimary && scaleSynchronizationType != ScaleSynchronizationTypeSecondary )
        { status = _statusUnknownSynchronizationType_; break; }


        /* (uc1.1) Execute "Use Case 1.9: Disabling input processing" passing the message:
            "Initializing Synchronization mechanism. Wait until procedure completes" */
        status = usecaseDisablingInputProcessing(synchronization::InitSynchronizationMechanismSyncError);
        if ( status != _statusOK_ ) break;


        /* (uc1.1) Execute "Use Case 1.2: Starting up Synchronization Manager" passing the Scale Type
            obtained above. */
        status = usecaseStartingupSynchronizationManager(scaleSynchronizationType);
        //          if ( status != _statusOK_ ) break;    /* this line is removed so that we can enable input processing below */


        /* (uc1.1) Trigger the execution of "Use Case 1.8: Enabling input processing" */
        /* [We call this inline so as to resume paused threads as soon as possible.] */
        usecaseEnablingInputProcessing();
    }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Booting up: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Booting up: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* Primary/Secondary feature: "Use Case 1.2: Starting up Synchronization Manager" */
int SessionManager::usecaseStartingupSynchronizationManager(ScaleSynchronizationType scaleSynchronizationType, bool registerFlag,
        const std::string & primaryScale)
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Starting up Synchronization Manager (Sync Type = " +
            util_string::inttostring(scaleSynchronizationType) + ", registering = " + util_string::booltostring(registerFlag) +
            ", primary scale = " + primaryScale + "): Starting");
#endif



    /* (uc1.2) Pre-condition: No Synchronization Manager is running */
	// Commenting out the following line because no one was trapping this exception that could be thrown.
	// Instead, just don't create a synchronizationManager_ if one already exists.
    // Assert(!synchronizationManager_, Util::CParamException());

	if ( !synchronizationManager_ )
	{
		int doFakeLoop = 1;
		while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
			{
				/* (uc1.2) Based on the new Scale Type configuration value received, initialize/start
				   the correct Synchronization Manager, passing the Scale Online flag and the optional
				   parameter(s) received, if any. */

				try
				{
					switch ( scaleSynchronizationType )
					{
						case ScaleSynchronizationTypePrimary:
						{
							synchronizationManager_ = new synchronization::PrimarySynchronizationManager(scaleOnline_ == ScaleOnlineTypeOnline);

							break;
						}
						case ScaleSynchronizationTypeSecondary:
						{
							synchronizationManager_ = new synchronization::SecondarySynchronizationManager(scaleOnline_ == ScaleOnlineTypeOnline, registerFlag, primaryScale);

							break;
						}
						default:
						{
#ifndef EVOL_NDEBUG
							Debug::trace("SessionManager", "************** Sync Type = " +
									util_string::inttostring(scaleSynchronizationType) + " is unknown *****************");
#endif

							status = _statusUnknownSynchronizationType_;
						}
					}
				}
				catch (...)
				{
					// There was a problem initializing the synchronization manager.
					status = _statusCouldNotInitializeSynchronizationManager_;

#ifndef EVOL_NDEBUG
					Debug::trace("SessionManager", "***** Exception in SessionManager::usecaseStartingupSynchronizationManager syncType=" + util_string::inttostring(scaleSynchronizationType) );
#endif
				}
			}


#ifndef EVOL_NDEBUG
		if ( status == _statusOK_ )
			Debug::trace("SessionManager", "Use case Starting up Synchronization Manager: completed successfully");
		else
			Debug::trace("SessionManager", "Use case Starting up Synchronization Manager: failed (status = " + getStatusText(status) + " )");
#endif
	}

    return status;
}


/* "Use Case 1.3: Coming up on-line" */
int SessionManager::usecaseComingUpOnLine()
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Coming up on-line: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            /* (uc1.3) If the Scale on-line flag is set, then end the execution of this use case. */
            if ( scaleOnline_ == ScaleOnlineTypeOnline )
            { status = _statusOK_; break; }


            /* (uc1.3) Set the Scale on-line flag. */
            scaleOnline_ = ScaleOnlineTypeOnline;


            /* (uc1.3) If there is no Synchronization Manager, then end the execution of this use case. */
            if ( !synchronizationManager_ )
            { status = _statusOK_; break; }


            /* (uc1.3) Trigger the execution of "Use Case 5.3: Coming up on-line". */
#ifndef EVOL_NDEBUG
            Debug::trace("SessionManager", "Use case Coming up on-line: Going to notify the Synchronization Manager of state change");
#endif

            synchronization::ComingUpOnLineMessage message;
            dispatchEvent(&message);
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Coming up on-line: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Coming up on-line: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* "Use Case 1.5: Going off-line" */
int SessionManager::usecaseGoingOffLine()
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Going off-line: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            ScaleOnlineType currentScaleOnline = scaleOnline_;


            /* (uc1.5) If the Scale 0n-line flag is clear, then end the execution of this use case. */
            if ( scaleOnline_ == ScaleOnlineTypeOffline )
            { status = _statusOK_; break; }


            /* (uc1.5) Clear the Scale on-line flag. */
            scaleOnline_ = ScaleOnlineTypeOffline;


            /* (uc1.5) If there is no Synchronization Manager, then end the execution of this use case. */
            if ( !synchronizationManager_ )
            { status = _statusOK_; break; }


            /* (uc1.5) Trigger the execution of "Use Case 5.5: Going off-line". */
            if ( currentScaleOnline == ScaleOnlineTypeOnline )
            {
#ifndef EVOL_NDEBUG
                Debug::trace("SessionManager", "Use case Going off-line: Going to notify the Synchronization Manager of state change");
#endif

                synchronization::GoingOffLineMessage message;
                dispatchEvent(&message);
            }
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Going off-line: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Going off-line: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


void SessionManager::processShutdownSynchronization()
{
#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Shutting down Synchronization: Starting");
#endif


    if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone &&
            synchronizationManager_ )
    {
        // Don't allow the Session Manager to allow anyone to connect to the scale.
        allowConnections = false;

        usecaseDisablingInputProcessing(synchronization::ShutdownSynchronizationMechanismSyncError);

        synchronizationManager_->setShutdownFlag();

        delete synchronizationManager_;
        synchronizationManager_ = 0;

        usecaseEnablingInputProcessing();
    }


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Shutting down Synchronization: completed successfully");
#endif
}


/* Primary/Secondary feature: "Use Case 1.6: Stopping Synchronization Manager" */
int SessionManager::usecaseStoppingSynchronizationManager()
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Stopping Synchronization Manager: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            if ( !synchronizationManager_ )
            { 
				status = _statusOK_;
				//SOF-5414: used to be _statusNoSynchronizationManager_
				// Problem is (I believe) is that we have no synchronization manager on startup, thus we return non-ok status
				// This was causing the application to crash on startup when
				// synchronization was enabled - only reason no synchronizaton manager should matter here is in deleting the pointer
				break; 
			}

            /* (uc1.6) Trigger the execution of "Use Case 5.6: Stopping Synchronization Manager" for the
               current Synchronization Manager. */
            /* [Nothing to do here since it happens inside the destructor to the Synchronization Manager.] */


            /* (uc1.6) Wait until receiving the Stop Synchronization Manager processed message. */
            /* [Nothing to do here since the return from the destructor guarantees all is cleaned up.] */


            /* (uc1.6) Destroy the Synchronization Manager. */
            delete synchronizationManager_;
            synchronizationManager_ = 0;
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Stopping Synchronization Manager: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Stopping Synchronization Manager: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* Primary/Secondary feature: "Use Case 1.7: Accepting TCP/IP socket connection" */
/* See the ::Run method, at the top of its while loop, and the SessionStarter
   subtype. */


/* Primary/Secondary feature: "Use Case 1.8: Enabling input processing" */
int SessionManager::usecaseEnablingInputProcessing()
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Enabling input processing: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            /* (uc1.8) For each active Primary/Remote Console Session, execute "Use Case 2.8: Enabling
               input processing". */
            /* [By calling this function all Session threads will now be unblocked and notified of the
               pausing so they can resume their activies or act accordingly.] */
            resumePausedThreads();


            /* (uc1.8) Set the Input processing enabled flag. */
            inputDisablingCondition_ = 0;
            inputProcessingEnabled_ = true;
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Enabling input processing: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Enabling input processing: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* Primary/Secondary feature: "Use Case 1.9: Disabling input processing" */
int SessionManager::usecaseDisablingInputProcessing(int condition)
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Disabling input processing: Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            /* (uc1.9) Clear the Input Processing Enabled flag */
            inputProcessingEnabled_ = false;


            /* (uc1.9) Store the reference to the text string passed in. */
            inputDisablingCondition_ = condition;


            /* (uc1.9) For each active session, if it is of type: */
            /* [By calling this method, all sessions will be paused until we later call
               resumePausedThreads in "Use Case 1.8: Enabling input processing". Additionally,
               the synchronization threads will be notified that we are in the process of
               changing Synchronization.] */
            requestToPause(0, false, false);
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Disabling input processing: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Disabling input processing: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* Primary/Secondary feature: "Use Case 1.10: Changing Synchronization Type" */
int SessionManager::usecaseChangingSynchronizationType(ScaleSynchronizationType scaleSynchronizationType, const std::string & primaryScale)
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Changing Synchronization Type (new type = " +
            util_string::inttostring(scaleSynchronizationType) + ", new primary = " + primaryScale + "): Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
        {
            /* (uc1.10) Execute "Use Case 1.9: Disabling input processing", passing a reference to the
               text string corresponding to "Changing Synchronization Type. Wait until procedure completes". */
            status = usecaseDisablingInputProcessing(synchronization::ChangingSynchronizationTypeSyncError);
            if ( status != _statusOK_ ) break;

            if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone )
            {
                /* (uc1.10) Execute "Use Case 1.6: Stopping Synchronization Manager". */
                status = usecaseStoppingSynchronizationManager();
                //                      if ( status != _statusOK_ ) break;          // Don't break so we can reenable input processing below


                /* (uc1.10) Execute the Database Compression procedure. */
                /* [This is not needed since the Use Case 1.6 really wipes out any traces of scales,
                   batches, or synchronization changes.] */
            }

			//IR #35 -> pulled out the saving of configuration changes regardless of whether 
			//synchronizationEnabled=F in the configuration
            DBAccess dba;
            dba.initDB();

            /* (uc1.10) Change Synchronization Type configuration element to new value passed in. */
            Configuration::getInstance()->setScaleSynchronizationType(scaleSynchronizationType);

            /* (uc1.10) Change Primary Scale configuration element to the new value passed in if any. */
            std::string primaryScaleInfo(primaryScale);
            if ( scaleSynchronizationType == ScaleSynchronizationTypePrimary )
                primaryScaleInfo.clear();

            Configuration::getInstance()->setPrimaryScale(primaryScaleInfo);
            Configuration::getInstance()->save(&dba);

            EvolMessage message(ConfigurationDev, CfgSaved);
            dispatchEvent(&message);

            /* Update database configuration pertaining to Synchronization. */
            if ( status == _statusOK_ )
            {
                /* (uc1.10) If the new Synchronization Type is: */
                switch ( scaleSynchronizationType )
                {
                    case ScaleSynchronizationTypePrimary:
                        {
                            /* (uc1.10) a. Primary, then execute "Use Case 1.2: Starting up Synchronization
                               Manager", passing Primary as the Synchronization Type value. */
                            status = usecaseStartingupSynchronizationManager(scaleSynchronizationType);

                            break;
                        }
                    case ScaleSynchronizationTypeSecondary:
                        {
                            /* (uc1.10) b. Secondary, then execute "Use Case 1.2: Starting up Synchronization
                               Manager", passing Secondary as the Synchronization Type value, the "register"
                               parameter, and the new Primary Scale information if received. */
                            status = usecaseStartingupSynchronizationManager(scaleSynchronizationType, true, primaryScale);

                            break;
                        }
                    case ScaleSynchronizationTypeStandAlone:
                        {
#ifndef EVOL_NDEBUG
                            Debug::trace("SessionManager", "Changing to Stand Alone");
#endif
                            status = _statusOK_;

                            break;
                        }
                    default:
                        {
#ifndef EVOL_NDEBUG
                            Debug::trace("SessionManager", "************** Sync Type = " +
                                    util_string::inttostring(scaleSynchronizationType) + " is unknown *****************");
#endif

                            status = _statusUnknownSynchronizationType_;

                            break;
                        }
                }
            }


            /* (uc1.10) Trigger the execution of "Use Case 1.8: Enabling input processing". */
            usecaseEnablingInputProcessing();
        }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Changing Synchronization Type: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Changing Synchronization Type: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


/* Primary/Secondary feature: "Use Case 1.11: Changing Primary Scale" */
int SessionManager::usecaseChangingPrimaryScale(const std::string & primaryScale)
{
    int status = _statusOK_;


#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager", "Use case Changing Primary Scale (new primary = " + primaryScale + "): Starting");
#endif


	int doFakeLoop = 1;
	while ( doFakeLoop-- == 1 )	 // This is not meant to be a loop
    {
        if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeSecondary )
        { status = _statusCannotChangePrimaryIfNotSecondary_; break; }


        /* (uc1.11) Execute "Use Case 1.9: Disabling input processing", passing a reference to the
            text string corresponding to "Changing Primary Scale. Wait until procedure completes". */
        status = usecaseDisablingInputProcessing(synchronization::ChangingPrimaryScaleSyncError);
        if ( status != _statusOK_ ) break;


        /* (uc1.11) Execute "Use Case 1.6: Stopping Synchronization Manager". */
        status = usecaseStoppingSynchronizationManager();
        //          if ( status != _statusOK_ ) break;     // Don't break so that we can reenable input processing


        /* (uc1.11) Execute the Database Compression procedure. */
        /* [This is not needed since the Use Case 1.6 really wipes out any traces of scales,
            batches, or synchronization changes.] */


        /* Update database configuration pertaining to Synchronization. */
        if ( status == _statusOK_ )
        {
            {
                DBAccess dba;
                dba.initDB();


                /* (uc1.11) Change Primary Scale configuration element to the new value passed in if any. */
                if ( primaryScale.size() )
                    Configuration::getInstance()->setPrimaryScale(primaryScale);


                Configuration::getInstance()->save(&dba);


                EvolMessage message(ConfigurationDev, CfgSaved);
                dispatchEvent(&message);
            }


            /* (uc1.11) Execute "Use Case 1.2: Starting up Synchronization Manager", passing Secondary
                as the Synchronization Type value, the "register" parameter, and the new Primary Scale
                information if received. */
            if ( status == _statusOK_ )
                status = usecaseStartingupSynchronizationManager(ScaleSynchronizationTypeSecondary, true, primaryScale);
        }


        /* (uc1.11) Trigger the execution of "Use Case 1.8: Enabling input processing". */
        usecaseEnablingInputProcessing();
    }


#ifndef EVOL_NDEBUG
    if ( status == _statusOK_ )
        Debug::trace("SessionManager", "Use case Changing Primary Scale: completed successfully");
    else
        Debug::trace("SessionManager", "Use case Changing Primary Scale: failed (status = " + getStatusText(status) + " )");
#endif


    return status;
}


bool SessionManager::isAllowConnections()
{
    return allowConnections;
}
void SessionManager::setAllowConnections(bool allow)
{
    allowConnections = allow;
}

void SessionManager::handleLogoffRequest()
{
	Debug::trace("SessionManager", HobString("handleLogoffRequest - SessionManager handling SnLogoffRequest Event"));

	MarqueeUpdate mqClearProduct(MqProduct, " ");
	dispatchEvent(&mqClearProduct);

	MarqueeUpdate mqClearDaily(MqDaily, "");
	dispatchEvent(&mqClearDaily);

	NumericGeneric nReset(NmReset);
	dispatchEvent(&nReset);

	if (primaryConsoleSession)
	{
		if (primaryConsoleSession->getResponseMessageManager())
			primaryConsoleSession->getResponseMessageManager()->removeAllMessages();

		EvolMessage termin8(SessionDev, SnTerminate);
		dispatchEvent(&termin8, primaryConsoleSession);
		PAL_absclass::PAL_sleep(3000);
		Debug::trace("SessionManager", HobString("handleLogoffRequest - sent terminate message to primary session, then paused for 3 seconds.  Now continuing."));
	}

	handleRebootMessage(RebootRequest_Logout);
}



// This is the method called when SIGTERM is detected by chobart_d.cpp (Linux).
// When called, we need to safely shutdown the backend as quickly as possible (close database and any open files).
void SessionManager::panicShutdown()
{
	Debug::trace("SessionManager", "panicShutdown initiated!!!");

	panicShutdown_ = true;
	shutdownStep = 0;

    requestToPause(0, true, false);

	handleShutdownRequest();

	Debug::trace("SessionManager", "panicShutdown finished!!!");

	hfc::MultiLanguageText internationalize;
	MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessagePanicShutdown"));
}


void SessionManager::handleShutdownRequest()
{
	bool isTracing = Debug::isTracing("SessionManager");

    if (shutdownStep == 0)
    {
		if (isTracing)
			Debug::trace("SessionManager", "handleShutdownRequest - SessionManager handling SnShutdownRequest Event");

        MarqueeUpdate mqClearProduct(MqProduct, " ");
        dispatchEvent(&mqClearProduct);

        MarqueeUpdate mqClearDaily(MqDaily, "");
        dispatchEvent(&mqClearDaily);

        NumericGeneric nReset(NmReset);
        dispatchEvent(&nReset);

        // Don't allow the Session Manager to allow anyone to connect to the scale.
        allowConnections = false;

        // Tell any BackupRestoreManagers to stop what they're doing.
        EvolMessage stopBRM(BackupRestore, BRCancel);
        dispatchEvent(&stopBRM);

        // Stop the timer for the database backup.
        if (timerManager)
        {
            Timer *dbBackupTimer = timerManager->getTimer(timedFuncBackupDatabase);
            if (dbBackupTimer)
            {
                timerManager->removeTimer(dbBackupTimer->getTimerId());
				if (isTracing)
					Debug::trace("SessionManager", "handleShutdownRequest - deleting the timer for timedFuncBackupDatabase");
            }
        }
		
        if (!panicShutdown_ && !remoteSessions.empty())
        {
			// Display dialog that the scale is shutting down.
			bool needShutdownTimer = false;

			list<AbstractSession *>::iterator pos;
			pos = remoteSessions.begin();
			while (pos != remoteSessions.end())
			{
				if ((*pos)->getSessionType() == Session::SESSIONTYPE_REMOTE || (*pos)->getSessionType() == Session::SESSIONTYPE_COMM)
					needShutdownTimer = true;

				if ((*pos)->getSessionType() == Session::SESSIONTYPE_PRIMARY || (*pos)->getSessionType() == Session::SESSIONTYPE_REMOTE)
				{
					Session *s = dynamic_cast<Session *> (*pos);
					if (s && s->getResponseMessageManager())
					{
						s->getResponseMessageManager()->removeAllMessages();
						if (s->getSessionType() == Session::SESSIONTYPE_PRIMARY)
							s->getResponseMessageManager()->addMessage(ErrSessionDev, SessionDevShuttingdown, HfcResponse::RESPONSESTYLE_NOINPUT);
					}
				}
				pos++;
			}

			if (needShutdownTimer && timerManager)
			{
				if (isTracing)
					Debug::trace("SessionManager", "handleShutdownRequest - creating a timer to continue shutdown if all remote or comm sessions don't terminate.");
				timerManager->addTimer(10, Timer::RepeatNO, timedFuncShutdownWaitingForRemotesSessionsToQuit);
			}
        }

        // Shutdown all remote connections so no other task can update the database while we back it up.
        // Gracefully shut down any comm session that is active.
        TerminateMessage shutdownRemotes(this, true);
        dispatchEvent(&shutdownRemotes);

		if (!panicShutdown_)
			PAL_absclass::PAL_sleep(3000);

        processShutdownSynchronization();

        shutdownStep = 1;

		// If we're in a normal shutdown scenario, return to the event loop to see if another message needs to be processed.
		// If we're in panicShutdown state, continue shutting down.
		if (!panicShutdown_)
			return;
    }

    if (shutdownStep == 1)
    {
        // Delete this timer in case it is still active.  Don't need it anymore.
        if (timerManager)
        {
            Timer *shutdownTimer = timerManager->getTimer(timedFuncShutdownWaitingForRemotesSessionsToQuit);
            if (shutdownTimer)
            {
                timerManager->removeTimer(shutdownTimer->getTimerId());
				if (isTracing)
					Debug::trace("SessionManager", "handleShutdownRequest - deleting the timer for waiting for remote and comm sessions to terminate.");
            }
        }

        shutdownStep = 2;

		// If we're in a normal shutdown scenario, return to the event loop to see if another message needs to be processed.
		// If we're in panicShutdown state, continue shutting down.
		if (!panicShutdown_)
			return;
    }

    if (shutdownStep == 2)
    {
		if (isTracing)
			Debug::trace("SessionManager", "handleShutdownRequest() - all remote and comm sessions have terminated");

        shutdownStep = 3;

#if defined( __linux__ )
		// finalize hook (before db close)
		if ( Configuration::getInstance()->getCustomerHooksEnabled() )
		    hobart_cust_hook_finalize();
#endif

        try
        {
	 		if (isTracing)
				Debug::trace("SessionManager", "before getting database instance");
            DBManager *myDatabase = DBManager::getInstance();
			if (isTracing)
				Debug::trace("SessionManager", "after  getting database instance");

            if (myDatabase)
            {
				// Close database first.
				if (isTracing)
					Debug::trace("SessionManager", "before closing database");
				myDatabase->closeDatabase();  // ->forceCloseEntireDatabase();
				if (isTracing)
					Debug::trace("SessionManager", "after  closing database");

                if (!panicShutdown_)
				{
					bool checkResult = true;
					if (Configuration::getInstance()->isDbCheckBeforeShutdown())
					{
						// Check the database before backing it up.
						if (isTracing)
							Debug::trace("SessionManager", "before checking database");
						try
						{
							std::string resultExplanation;
							checkResult = myDatabase->checkDatabase(resultExplanation);
						}
						catch (DBException &dbe)
						{
							if (Debug::isTracing("DBAccess.exception"))
							{
								Debug::trace("DBAccess.exception", "SessionManager::handleShutdownRequest() ********** database exception during checkDatabase **********");
								DBException::Type t = dbe.getType();
								Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
							}
							myDatabase->showAll();
						}
						if (isTracing)
							Debug::trace("SessionManager", HobString("after  checking database - result:") + HobString(Bool::toString(checkResult)));
					}

					if (checkResult)  // database check returned true, db is OK
					{
						// This is a remnant from Access on VxWorks, but if someone utilized the internal config "dbBackupLocation" for something else, still don't back it up if we're panicing.

						// Write the database back to flash.
						if (isTracing)
							Debug::trace("SessionManager", "ShutDownSystemRequest - writing the database back to flash");
						HobString path = Configuration::getInstance()->getDbBackupLocation();
						if (path == "")
						{
							if (isTracing)
								Debug::trace("SessionManager", "No path to backup the database to.");
						}
						else
						{
							if (isTracing)
								Debug::trace("SessionManager", "Before myDatabase->backupDatabase()");
							try
							{
								myDatabase->backupDatabase(path);
							}
							catch (DBException &dbe)
							{
								if (Debug::isTracing("DBAccess.exception"))
								{
									Debug::trace("DBAccess.exception", "SessionManager::handleShutdownRequest() ********** database exception during backupDatabase **********");
									DBException::Type t = dbe.getType();
									Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
								}
							}
							if (isTracing)
								Debug::trace("SessionManager", "After myDatabase->backupDatabase()");
						}
					}
                }

                DBManager::destroyInstance(myDatabase);
                myDatabase = (DBManager *)NULL;
            }
        }
        catch (DBException &dbe)
        {
            if (Debug::isTracing("DBAccess.exception"))
            {
                Debug::trace("DBAccess.exception", "SessionManager::handleShutdownRequest() ********** database exception **********");
                DBException::Type t = dbe.getType();
                Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
            }
        }

        // shutdown diagnostics manager
		Debug::trace( "SessionManager", "Finalizing diagnostics..." );
        DiagnosticsManager::finalize();

        // Close any other files that may be open on the compact flash??


		if (!panicShutdown_)
		{
			// Go ahead and terminate the RTC Manager Processes??
			PAL_absclass::PAL_sleep (200);//make sure that SnReboot gets time to be processed first.
			if (primaryConsoleSession && primaryConsoleSession->getResponseMessageManager())
				primaryConsoleSession->getResponseMessageManager()->removeAllMessages();

/*  Not handled by the UsbManager anymore.
			if (rebootRequest_ == RebootRequest_NotReceived && primaryConsoleSession)
			{
				if (UsbManager::getInstance()->isBulkStorageDevicePresent())
				{
					if (isTracing)
						Debug::trace("SessionManager", "warn to remove usb drive");

					primaryConsoleSession->addDialogMessage(ErrVersionManagerDev, VersionManagerUsbDriveStillPresent, HfcResponse::RESPONSESTYLE_NOINPUT);
					while (UsbManager::getInstance()->isBulkStorageDevicePresent())
						PAL_absclass::PAL_sleep( 1000 );    
					primaryConsoleSession->removeDialogMessage(ErrVersionManagerDev, VersionManagerUsbDriveStillPresent, HfcResponse::RESPONSESTYLE_NOINPUT);
				}
			}
*/

			// Notify the primary console that the system is shutdown.
			EvolMessage shutdownComplete(SessionDev, SnShutdownComplete);
			dispatchEvent(&shutdownComplete);

			if (isTracing)
				Debug::trace("SessionManager", HobString("handleShutdownRequest. req = ") + Integer::toString(rebootRequest_));

			hfc::MultiLanguageText internationalize;
			MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessageApplicationShutdown"));

			shutdownStep = 99;  // This is meant to indicate that we are shutdown

			if ( rebootRequest_ != RebootRequest_NotReceived )
			{
				handleRebootMessage(rebootRequest_);
			}
			else
			{
				//Display dialog that the scale is shut down.
				if (primaryConsoleSession && primaryConsoleSession->getResponseMessageManager())
					primaryConsoleSession->getResponseMessageManager()->addMessage(ErrSessionDev, SessionDevShutdownComplete, 12 /*HfcResponse::RESPONSESTYLE_NOINPUT*/);
			}
		}
    }
}


void SessionManager::handleRebootMessage(RebootRequest request)
{
    Debug::trace("SessionManager", " - handling SnReboot Event");

	if (Debug::isTracing("database"))
	{
		Debug::trace("database", "SessionManager::handleRebootMessage() - showAll before reboot/shutdown");
		edb::DBManager::showAll();
	}

	if (Debug::isTracing("bypassShutdown") || Configuration::getInstance()->toBoolCfgI("bypassShutdown"))
    {
	    Debug::trace("SessionManager", "bypassShutdown is set; the scale would have rebooted now.");
        Debug::trace("bypassShutdown", "bypassShutdown is set; the scale would have rebooted now.");
        return;
    }
#if defined ( __linux__ )
    if ( request == RebootRequest_PowerDown )
    {
	   Debug::trace("SessionManager", HobString("Sending system shutdown")); 
	   system("echo >/home/halt"); //file to double check runlelvel
	   system("sync");	//flush to disk
       int result = system("init 0"); //enter runlevel shutdown or halt

		if (result == -1)
			Debug::trace("SessionManager", "Failed to halt!");
    }
    else
    {
    	system("sync"); //flush to disk
        int result = system("init 6");	//enter runlevel for reboot

		if (result == -1)
			Debug::trace("SessionManager", "Failed to reboot!");
    }
#else
    int failure = 0;
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Open the process token to enable adjusting the process' privileges
    if ( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) )
    {
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

        // Adjust the privileges to enable shutting down
        if ( AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0) )
        {
            //                  ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
            //                  InitiateSystemShutdownEx(NULL, NULL /*_T("User-requested Shutdown")*/, 100, TRUE, TRUE, 0);
            if ( request == RebootRequest_PowerDown )
            {
                Debug::trace("SessionManager", "Going to perform Psuedo Power Off function");
				if(isEvolutionWrapperAttached())//Added 4/24/14 Alex Zavala
				{
					Debug::trace("SessionManager", "sending wrapper reset");
					sendWrapperReset();
				}
				if(isCLAEPSAC2Attached())
				{
					Debug::trace("SessionManager", "sending CLA reset");
					sendLabelApplierReset();
				}

				// System never is to be shutdown and powered off
				if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
				{
					const string command( PythonScripter::buildPythonCall("pseudoShutdown.py") );
					string output;
					PAL_absclass::PAL_executeWithResults(command, output);
				}
			}


                //if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, 0) )
                //    failure = 3;
            
			else if ( request == RebootRequest_Reboot )
            {
                Debug::trace("SessionManager", "Going to perform Reboot function");
				if(isEvolutionWrapperAttached())//Added 4/24/14 Alex Zavala
				{
					Debug::trace("SessionManager", "sending wrapper reset");
					sendWrapperReset();
				}
				if(isCLAEPSAC2Attached())
				{
					Debug::trace("SessionManager", "sending CLA reset");
					sendLabelApplierReset();
				}

                if ( !ExitWindowsEx(EWX_REBOOT, 0) )
                    failure = 4;
            }
			else if ( request == RebootRequest_Logout )
			{
				if ( !ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED) ) 
				{
					//if (primaryConsoleSession)
					//	primaryConsoleSession->addDialogMessage(ErrSessionDev, 100001);
					while (1)
					{
						Sleep(5000);
						Debug::trace("SessionManager", HobString("handleRebootMessage - waiting to logout ..."));
					}
				}
				else
					Debug::trace("SessionManager", HobString("handleRebootMessage - could not tell Windows to logout"));
			}

        }
        else failure = 2;
    }
    else failure = 1;

    if ( failure )
    {
        int err = GetLastError();
        Debug::trace("SessionManager", "Failed step " + util_string::inttostring(failure) +
                " during Rebooting the scale (LastError = " + util_string::inttostring(err) + ")");
    }
#endif          //LINUX_FIX
}


#if 0
// This code existed when the SessionManager was responsible for handling the deleteAll in which we replaced the db file(s)
// for the record type with those from the empty database.  With the db's "zap" interface, this is no longer required.

void SessionManager::handleDeleteAll(EvolDeviceType recordType)
{
    bool tracing = (Debug::isTracing("SessionManager")) ? true : false;

    if (tracing)
        Debug::trace("SessionManager", "handleDeleteAll - entering for recordType " + HobString(Integer::toString((int)recordType)));

    DBManager *myDatabase = DBManager::getInstance();

    EvolMessage start(SessionDev, SnDBClosingTemporarily);
    dispatchEvent(&start);
    // delay for 1 second to give Sessions time to be notified that the database is about to be closed temporarily
    PAL_absclass::PAL_sleep(1000);

    if (myDatabase)
    {
        try
        {
            if (tracing)
                Debug::trace("SessionManager", "handleDeleteAll - before closing database");
            myDatabase->closeDatabase();
            if (tracing)
                Debug::trace("SessionManager", "handleDeleteAll - database is closed, before deleting record type");

            std::string dbloc = Configuration::getInstance()->toStringCfgI(CfgInternal::DBLOCATION);
            //                  std::string cfloc = Configuration::getInstance()->toStringCfgI(CfgInternal::DBBACKUPLOCATION);
            std::string emptyloc = Configuration::getInstance()->toStringCfgI(CfgInternal::EMPTYDBLOCATION);

            if (recordType == ProductDev)
            {
                {
                    File emptyFileDBF(emptyloc, "PRODS.DBF");
                    File emptyFileCDX(emptyloc, "PRODS.CDX");
                    File emptyFileFPT(emptyloc, "PRODS.FPT");
                    if (emptyFileDBF.exists() && emptyFileCDX.exists() && emptyFileFPT.exists())
                    {
                        // Copy all Product db files from the empty db directory to the ram drive and compact flash
                        emptyFileDBF.copyTo(File(dbloc, "PRODS.DBF"));
                        //                                              emptyFileDBF.copyTo(File(cfloc, "PRODS.DBF"));
                        if (tracing)
                            Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileDBF.getFullPath());

                        emptyFileCDX.copyTo(File(dbloc, "PRODS.CDX"));
                        //                                              emptyFileCDX.copyTo(File(cfloc, "PRODS.CDX"));
                        if (tracing)
                            Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileCDX.getFullPath());

                        emptyFileFPT.copyTo(File(dbloc, "PRODS.FPT"));
                        //                                              emptyFileFPT.copyTo(File(cfloc, "PRODS.FPT"));
                        if (tracing)
                            Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileFPT.getFullPath());
                    }
                    else if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - one of the Product files does not exist in the empty directory");
                }
                {
                    File emptyFileDBF(emptyloc, "PCSEQS.DBF");
                    File emptyFileCDX(emptyloc, "PCSEQS.CDX");
                    if (emptyFileDBF.exists() && emptyFileCDX.exists())
                    {
                        // Copy all Product Sequences db files from the empty db directory to the ram drive and compact flash
                        emptyFileDBF.copyTo(File(dbloc, "PCSEQS.DBF"));
                        //                                              emptyFileDBF.copyTo(File(cfloc, "PCSEQS.DBF"));
                        if (tracing)
                            Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileDBF.getFullPath());

                        emptyFileCDX.copyTo(File(dbloc, "PCSEQS.CDX"));
                        //                                              emptyFileCDX.copyTo(File(cfloc, "PCSEQS.CDX"));
                        if (tracing)
                            Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileCDX.getFullPath());
                    }
                    else if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - one of the Product Sequences files does not exist in the empty directory");
                }
            }
            else if (recordType == ExpandedTextDev)
            {
                File emptyFileDBF(emptyloc, "TEXTS.DBF");
                File emptyFileCDX(emptyloc, "TEXTS.CDX");
                File emptyFileFPT(emptyloc, "TEXTS.FPT");
                if (emptyFileDBF.exists() && emptyFileCDX.exists() && emptyFileFPT.exists())
                {
                    // Copy all Expanded Text db files from the empty db directory to the ram drive and compact flash
                    emptyFileDBF.copyTo(File(dbloc, "TEXTS.DBF"));
                    //                                  emptyFileDBF.copyTo(File(cfloc, "TEXTS.DBF"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileDBF.getFullPath());

                    emptyFileCDX.copyTo(File(dbloc, "TEXTS.CDX"));
                    //                                  emptyFileCDX.copyTo(File(cfloc, "TEXTS.CDX"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileCDX.getFullPath());

                    emptyFileFPT.copyTo(File(dbloc, "TEXTS.FPT"));
                    //                                  emptyFileFPT.copyTo(File(cfloc, "TEXTS.FPT"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileFPT.getFullPath());
                }
                else if (tracing)
                    Debug::trace("SessionManager", "handleDeleteAll - one of the Expanded Text files does not exist in the empty directory");
            }
            else if (recordType == SpecialMessageDev)
            {
                File emptyFileDBF(emptyloc, "SMSGS.DBF");
                File emptyFileCDX(emptyloc, "SMSGS.CDX");
                File emptyFileFPT(emptyloc, "SMSGS.FPT");
                if (emptyFileDBF.exists() && emptyFileCDX.exists() && emptyFileFPT.exists())
                {
                    // Copy all Special Messages db files from the empty db directory to the ram drive and compact flash
                    emptyFileDBF.copyTo(File(dbloc, "SMSGS.DBF"));
                    //                                  emptyFileDBF.copyTo(File(cfloc, "SMSGS.DBF"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileDBF.getFullPath());

                    emptyFileCDX.copyTo(File(dbloc, "SMSGS.CDX"));
                    //                                  emptyFileCDX.copyTo(File(cfloc, "SMSGS.CDX"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileCDX.getFullPath());

                    emptyFileFPT.copyTo(File(dbloc, "SMSGS.FPT"));
                    //                                  emptyFileFPT.copyTo(File(cfloc, "SMSGS.FPT"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileFPT.getFullPath());
                }
                else if (tracing)
                    Debug::trace("SessionManager", "handleDeleteAll - one of the Special Messages files does not exist in the empty directory");
            }
            else if (recordType == NutrifactsDev)
            {
                File emptyFileDBF(emptyloc, "NUTRFS.DBF");
                File emptyFileCDX(emptyloc, "NUTRFS.CDX");
                File emptyFileFPT(emptyloc, "NUTRFS.FPT");
                if (emptyFileDBF.exists() && emptyFileCDX.exists() && emptyFileFPT.exists())
                {
                    // Copy all Nutri Facts db files from the empty db directory to the ram drive and compact flash
                    emptyFileDBF.copyTo(File(dbloc, "NUTRFS.DBF"));
                    //                                  emptyFileDBF.copyTo(File(cfloc, "NUTRFS.DBF"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileDBF.getFullPath());

                    emptyFileCDX.copyTo(File(dbloc, "NUTRFS.CDX"));
                    //                                  emptyFileCDX.copyTo(File(cfloc, "NUTRFS.CDX"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileCDX.getFullPath());

                    emptyFileFPT.copyTo(File(dbloc, "NUTRFS.FPT"));
                    //                                  emptyFileFPT.copyTo(File(cfloc, "NUTRFS.FPT"));
                    if (tracing)
                        Debug::trace("SessionManager", "handleDeleteAll - copied " + emptyFileFPT.getFullPath());
                }
                else if (tracing)
                    Debug::trace("SessionManager", "handleDeleteAll - one of the Nutri Facts files does not exist in the empty directory");
            }
        }
        catch (DBException compressDBExc)
        {
            if (Debug::isTracing("DBAccess.exception"))
            {
                Debug::trace("DBAccess.exception", HobString("SessionManager::handleDeleteAll() ********** database exception thrown while deleting a record type **********"));
                DBException::Type t = compressDBExc.getType();
                Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + compressDBExc.info());
            }
        }
        catch (...)
        {
            if (Debug::isTracing("DBAccess.exception"))
            {
                Debug::trace("DBAccess.exception", HobString("SessionManager::handleDeleteAll() ********** some other exception thrown while deleting a record type **********"));
            }
        }

        try
        {
            if (tracing)
                Debug::trace("SessionManager", "handleDeleteAll - before reopening database");
            myDatabase->openDatabase();
            if (tracing)
                Debug::trace("SessionManager", "handleDeleteAll - database is reopened");
        }
        catch (DBException openDBExc)
        {
            if (Debug::isTracing("DBAccess.exception"))
            {
                Debug::trace("DBAccess.exception", HobString("SessionManager::handleDeleteAll() ********** database exception thrown on opening the database after the delete of a record type **********"));
                DBException::Type t = openDBExc.getType();
                Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + openDBExc.info());
            }
        }

        DBManager::destroyInstance(myDatabase);
		myDatabase = (DBManager *)NULL;
    }

    EvolMessage done(SessionDev, SnDBReopened);
    dispatchEvent(&done);

    if (tracing)
        Debug::trace("SessionManager", "handleDeleteAll - leaving");
}

#endif



bool SessionManager::checkDatabaseIndices()
{
    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "checkDatabaseIndices()";
        Debug::trace("SessionManager", msgDebug);
        msgDebug.append(":  ");
    }

	bool checkResult = true;
	try
	{
		if (traceOn)
			Debug::trace("SessionManager", msgDebug + "before getting database instance");
		DBManager *myDatabase = edb::DBManager::getInstance();
		if (traceOn)
			Debug::trace("SessionManager", msgDebug + "after  getting database instance");

		if (myDatabase)
		{
			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "before checking database indices");

			bool shouldFixIndices = Configuration::getInstance()->toBoolCfgI(CfgInternal::DBREBUILDINDICESENABLED);
			checkResult = myDatabase->checkDatabaseIndices(shouldFixIndices);
			if (!checkResult)
				if (traceOn)
					Debug::trace("SessionManager", msgDebug + "CHECKING DATABASE INDICES FAILED");

			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "after checking database indices");

			DBManager::destroyInstance(myDatabase);
			myDatabase = (DBManager *)NULL;
		}
		else
		{
			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "COULD NOT GET INSTANCE OF THE DATABASE MANAGER!!!");
			checkResult = false;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("SessionManager::checkDatabaseIndices() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
		checkResult = false;
	}

    return checkResult;
}

/*
 * The parameter, reportAllStatus, if true, is used so that we report completely non-corrupt status to info.log.
 * If false, then we only report a status if it was returned from the call to checkDatabase().
 * checkDatabase() can return a more precise explanation when it can't determine the integrity of the database,
 * in which case it assumes that the database is not corrupt.
  */
bool SessionManager::checkDatabaseIntegrity(bool reportAllStatus)
{
    bool traceOn = (Debug::isTracing("SessionManager.DBIntegrityCheck")) ? true : false;
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "checkDatabaseIntegrity() ";
        Debug::trace("SessionManager.DBIntegrityCheck", msgDebug);
        msgDebug.append(":  ");
    }

    bool checkResult = true;
	try
	{
		if (traceOn)
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "before getting database instance");
		DBManager *myDatabase = edb::DBManager::getInstance();
		if (traceOn)
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "after  getting database instance");

		if (myDatabase)
		{
			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "before checking database integrity");

			std::string resultExplanation;
			checkResult = myDatabase->checkDatabase(resultExplanation);

			if (!resultExplanation.empty())
				MessageLogger::logMessage(MessageLogger::InfoLog, resultExplanation);
			else if (reportAllStatus && checkResult)
				MessageLogger::logMessage(MessageLogger::InfoLog, "Database Integrity Check - OK");

			if (!checkResult)
				if (traceOn)
					Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "CHECKING DATABASE INTEGRITY FAILED");

			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "after checking database integrity");

			DBManager::destroyInstance(myDatabase);
			myDatabase = (DBManager *)NULL;
		}
		else
		{
			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "COULD NOT GET INSTANCE OF THE DATABASE MANAGER!!!");
			checkResult = false;
		}
    }
    catch (DBException &dbe)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("SessionManager::checkDatabaseIntegrity() ********** database exception **********"));
            DBException::Type t = dbe.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
        }
        checkResult = false;
    }

    return checkResult;
}


bool SessionManager::handleCorruptDatabase()
{
	bool traceOn = (Debug::isTracing("SessionManager.DBIntegrityCheck")) ? true : false;
	HobString msgDebug = "handleCorruptDatabase(): ";

	if (traceOn)
		Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "started!");

	bool result = false;


	requestToPause(0, true, false);


	std::string cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc;
	bool goodDirs = validateDatabaseDirectories(cfDbLoc, cfEmptyDbLoc, cfMirrorDbLoc, cfCorruptDbLoc);

	if (goodDirs)
	{
		if (traceOn)
		{
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "**  There is a problem opening the database.  **");
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "         cfDbLoc = [" + cfDbLoc + "]");
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "    cfEmptyDbLoc = [" + cfEmptyDbLoc + "]");
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "   cfMirrorDbLoc = [" + cfMirrorDbLoc + "]");
			Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "  cfCorruptDbLoc = [" + cfCorruptDbLoc + "]");
		}

		bool isGood = false;
		isGood = copyDatabaseFiles(cfCorruptDbLoc, cfDbLoc);
		if (!isGood)
		{
			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "Could not save the database files to the latest corrupt database files directory.");
		}


		// First try and recover the last-saved good database and use it.
		isGood = copyDatabaseFiles(cfDbLoc, cfMirrorDbLoc);
		if (isGood)
		{
			bool checkResult = true;
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINTEGRITY))
				checkResult = checkDatabaseIntegrity(true);

			if (checkResult && Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINDICES))
				checkResult = checkDatabaseIndices();

			if (checkResult)
			{
				usingMirrorDatabase = true;
				result = true;
				string msg = "Database corruption resolution; Using the mirrored (backup) database.";
				MessageLogger::logMessage(MessageLogger::InfoLog, msg);
				if (traceOn)
					Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + msg);
			}
			else
			{
				isGood = false;
				if (traceOn)
					Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "The mirrored db either doesn't exist or is corrupt.");
			}
		}

		if (!isGood)
		{
			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "Could not copy the mirrored database files to the normal database directory. Will try to use the empty database files.");

			// If can't get the last-saved good database, use the empty database.
			isGood = copyDatabaseFiles(cfDbLoc, cfEmptyDbLoc);
			if (isGood)
			{
				bool checkResult = true;
				if (Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINTEGRITY))
					checkResult = checkDatabaseIntegrity(true);

				if (checkResult && Configuration::getInstance()->toBoolCfgI(CfgInternal::DBCHECKINDICES))
					checkResult = checkDatabaseIndices();

				if (checkResult)
				{
					usingEmptyDatabase = true;
					result = true;
					string msg = "Database corruption resolution; Using the factory default (empty) database.";
					MessageLogger::logMessage(MessageLogger::InfoLog, msg);
					if (traceOn)
						Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + msg);
				}
				else
				{
					isGood = false;
					if (traceOn)
						Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + "The factory default (empty) db either doesn't exist or is corrupt.");
				}
			}
		}

		if (!isGood)
		{
			string msg = "Database corruption resolution; Could not use the mirrored nor the empty database.";
			MessageLogger::logMessage(MessageLogger::InfoLog, msg);
			if (traceOn)
				Debug::trace("SessionManager.DBIntegrityCheck", msgDebug + msg);
		}
	}


    resumePausedThreads();
	return result;
}



bool SessionManager::openTheDatabase(/*DBManager *myDatabase*/)
{
    bool isOpen = false;
    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;
    HobString msgDebug = "openTheDatabase():  ";

    try
    {
        if (traceOn)
            Debug::trace("SessionManager", msgDebug + "before getting database instance");
        DBManager *myDatabase = DBManager::getInstance();
        if (traceOn)
            Debug::trace("SessionManager", msgDebug + "after  getting database instance");

		if (myDatabase)
		{
			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "before opening database");

			bool gotDbException = false;
			try
			{
				myDatabase->openDatabase();
				isOpen = true;

				if (traceOn)
					Debug::trace("SessionManager", msgDebug + "successfully opened database");
			}
			catch (DBException &dbe)
			{
				Debug::trace("SessionManager", "Exception trying to open the database!!!");
				if (Debug::isTracing("DBAccess.exception"))
				{
					Debug::trace("DBAccess.exception", HobString("SessionManager::openTheDatabase(): openDatabase() ********** database exception 1 **********"));
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				gotDbException = true;
			}

			if (gotDbException)
			{
				if (traceOn)
					Debug::trace("SessionManager", msgDebug + "exception opening database");
				try
				{
					myDatabase->closeDatabase();
				}
				catch (DBException &dbe)
				{
					Debug::trace("SessionManager", "Exception trying to close the bad database!!!");
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", HobString("SessionManager::openTheDatabase(): closeDatabase() ********** database exception 2 **********"));
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						Debug::trace("DBAccess.exception", "Calling DBManager::destroyInstance().");
					}
				}
			}

			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "after  opening database");

			DBManager::destroyInstance(myDatabase);
			myDatabase = (DBManager *)NULL;
		}
		else
		{
			if (traceOn)
				Debug::trace("SessionManager", "COULD NOT GET AN INSTANCE OF THE DATABASE MANAGER!!!");
		}
	}
    catch (DBException &dbex)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("SessionManager::openTheDatabase() ********** database exception **********"));
            DBException::Type t = dbex.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbex.info());
        }
    }

    return isOpen;
}


bool SessionManager::closeTheDatabase(/*DBManager *myDatabase*/)
{
    bool isClosed = false;
    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;
	HobString msgDebug = "closeTheDatabase():  ";

	try
	{
        if (traceOn)
            Debug::trace("SessionManager", msgDebug + "before getting database instance");
        DBManager *myDatabase = DBManager::getInstance();
        if (traceOn)
            Debug::trace("SessionManager", msgDebug + "after  getting database instance");

		if (myDatabase)
		{
			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "before closing database");

			try
			{
				myDatabase->closeDatabase();
				isClosed = true;

				if (traceOn)
					Debug::trace("SessionManager", msgDebug + "successfully closed database");
			}
			catch (DBException &dbe)
			{
				Debug::trace("SessionManager", "Exception trying to close the database!!!");
				if (Debug::isTracing("DB Access.exception"))
				{
					Debug::trace("DBAccess.exception", HobString("SessionManager::closeTheDatabase(): closeDatabase() ********** database exception 2 **********"));
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					Debug::trace("DBAccess.exception", "Calling DBManager::destroyInstance().");
				}
			}

			if (traceOn)
				Debug::trace("SessionManager", msgDebug + "after  closing database");

			DBManager::destroyInstance(myDatabase);
            myDatabase = (DBManager *)NULL;
		}
		else
		{
			if (traceOn)
				Debug::trace("SessionManager", "COULD NOT GET AN INSTANCE OF THE DATABASE MANAGER!!!");
		}
	}
    catch (DBException &dbex)
    {
        if (Debug::isTracing("DBAccess.exception"))
        {
            Debug::trace("DBAccess.exception", HobString("SessionManager::closeTheDatabase() ********** database exception **********"));
            DBException::Type t = dbex.getType();
            Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbex.info());
        }
    }

	return isClosed;
}


bool SessionManager::validateDatabaseDirectories(std::string &cfDbLoc, std::string &cfEmptyDbLoc, std::string &cfMirrorDbLoc, std::string &cfCorruptDbLoc)
{
    //  Get the directory names for the scale DB, empty DB, mirror DB and corrupt DB.
    //  We will return an error if empty DB can not be found.
    bool isGood = false;
    bool traceOn = (Debug::isTracing("SessionManager.badDB")) ? true : false;
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "validateDatabaseDirectories()";
        Debug::trace("SessionManager.badDB", msgDebug);
        msgDebug.append(":  ");
    }

    cfDbLoc = "";
    cfEmptyDbLoc = "";
    cfMirrorDbLoc = "";
    cfCorruptDbLoc = "";


    //  ****    Check that the scaleDB_empty directory on the CompactFlash does exist.
    cfEmptyDbLoc = Configuration::getInstance()->toStringCfgI(CfgInternal::EMPTYDBLOCATION);
	size_t len = cfEmptyDbLoc.length();
    if (len && cfEmptyDbLoc.rfind('/') == len - 1)
        cfEmptyDbLoc.erase(len - 1);

    Directory dEDB(const_cast<char *>(cfEmptyDbLoc.c_str()));
    bool dirStatus = dEDB.exists();
    if (!dirStatus)
    {
        //      The scaleDB_empty directory does not exist! We are done here. Exit.
        if (traceOn)
            Debug::trace("SessionManager.badDB", msgDebug + cfEmptyDbLoc + " does NOT exist!  Exiting.");
        return isGood;
    }
    if (traceOn)
        Debug::trace("SessionManager.badDB", msgDebug + cfEmptyDbLoc + " does exist.");


    //  ****    Check that the scaleDB directory on the CompactFlash does exist. If it doesn't, we can always create it.
    cfDbLoc = Configuration::getInstance()->toStringCfgI(CfgInternal::DBLOCATION);
    len = (int) cfDbLoc.length();
    if (len && cfDbLoc.rfind('/') == len - 1)
        cfDbLoc.erase(len - 1);

    Directory dDB(const_cast<char *>(cfDbLoc.c_str()));
    dirStatus = dDB.exists();
    if (traceOn)
    {
        if (!dirStatus)
            Debug::trace("SessionManager.badDB", msgDebug + cfDbLoc + " does NOT exist!");
        else
            Debug::trace("SessionManager.badDB", msgDebug + cfDbLoc + " does exist.");
    }

    //  ****    Check that the scaleDB_Mirror directory on the CompactFlash does exist. If it doesn't, we can always create it.
    cfMirrorDbLoc = Configuration::getInstance()->toStringCfgI(CfgInternal::DBMIRRORLOCATION);
    len = (int) cfMirrorDbLoc.length();
    if (len && cfMirrorDbLoc.rfind('/') == len - 1)
        cfMirrorDbLoc.erase(len - 1);

    Directory dBMirrorDB(const_cast<char *>(cfMirrorDbLoc.c_str()));
    dirStatus = dBMirrorDB.exists();
    if (traceOn)
    {
        if (!dirStatus)
            Debug::trace("SessionManager.badDB", msgDebug + cfMirrorDbLoc + " does NOT exist!");
        else
            Debug::trace("SessionManager.badDB", msgDebug + cfMirrorDbLoc + " does exist.");
    }

    //  Find out how many corrupt directories already exist.
    HobString tempDir;
    int corruptDirs = 0;
    do
    {
        tempDir = cfDbLoc + corruptStr;
        if (corruptDirs > 0)
            tempDir.append(inttostring(corruptDirs));
        Directory dCORR(const_cast<char *>(tempDir.c_str()));
        dirStatus = dCORR.exists();
        if (dirStatus)
        {
            ++corruptDirs;
            if (traceOn)
                Debug::trace("SessionManager.badDB", msgDebug + tempDir + " does exist.");
        }
    } while (dirStatus);

    //  Set up the corrupt DB location string.
    cfCorruptDbLoc.append(cfDbLoc).append(corruptStr);
    if (corruptDirs > 0)
        cfCorruptDbLoc.append(inttostring(corruptDirs));

    return true;
}


bool SessionManager::copyDatabaseFiles(const std::string cfDestinationDbLoc, const std::string cfSourceDbLoc)
{
    //  We will copy the files from the source directory to the destination directory.
    // This method is intended to copy the database files from one directory to another.
    // i.e. from the empty db directory to the normal db directory, or from the backup directory back to the normal db directory.

    bool isGood = false;
    bool traceOn = (Debug::isTracing("SessionManager")) ? true : false;
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "copyDatabaseFiles()";
        Debug::trace("SessionManager", msgDebug);
        msgDebug.append(":  ");
    }
    /*
       if ((cfDbLoc == "") || (cfEmptyDbLoc == "") || (cfCorruptDbLoc == ""))
       {
       Debug::trace("SessionManager", msgDebug + "At lease one of the directory names is null! Exiting.");
       return isGood;
       }
       */

    //  Does the destination (scaleDB) directory currently exist?

    //Directory dbDestinationDir(const_cast<char *>(cfDestinationDbLoc.c_str()));
    Directory dbDestinationDir(cfDestinationDbLoc);
    bool dirStatus = dbDestinationDir.exists();
    if (!dirStatus)
    {
        if (traceOn)
            Debug::trace("SessionManager", msgDebug + cfDestinationDbLoc + " does not exist. Create it.");

        //      Now create a new scaleDB directory so we can copy the source database files to it.
        dirStatus = dbDestinationDir.create();
        if (!dirStatus)
        {
            if (traceOn)
                Debug::trace("SessionManager", msgDebug + "Cannot create directory " + cfDestinationDbLoc + "!  Exiting!!!");
            return isGood;
        }
    }
	else
	{
		// Delete all files in the destination directory (but not subdirectories of the destination directory, for now anyway)
		util_filesystem::Directory::const_iterator dirIter;
		dbDestinationDir.selectAllFiles();
		dirIter = dbDestinationDir.begin();
		while (dirIter != dbDestinationDir.end())
		{
			std::string filename = dirIter->getName();
			File destFile (dbDestinationDir.getFullPath(), filename);
			if (destFile.exists())
				destFile.remove();

			++dirIter;
		}
	}

    //Directory dbSourceDir(const_cast<char *>(cfSourceDbLoc.c_str()));
    Directory dbSourceDir(cfSourceDbLoc);

    if (!dbSourceDir.exists())
        return isGood;

    //  Copy the source directory to the destination directory.
    dbDestinationDir << contents << dbSourceDir;

    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("copyDirectory: dirStatus = ").append(Integer::toString(dirStatus));
        msgDebug2.append(". Copied files from ").append(cfSourceDbLoc).append(" to ").append(cfDestinationDbLoc);
        Debug::trace("SessionManager", msgDebug2);
    }

    isGood = true;
    return isGood;
}



void SessionManager::addSessionStarter(SessionStarter* starter)
{
    Debug::trace("SessionManager.SessionStarter", "Adding SessionStarter " + util_string::ptrtostring(starter) +
            " to starters container");

    util_thread::Lock< util_thread::Mutex<> > lock(startersMutex_);

    starters_.push_back(Util::CSmartPtr<SessionStarter>(starter));
}


void SessionManager::removeSessionStarter(SessionStarter* starter)
{
    util_thread::Lock< util_thread::Mutex<> > lock(startersMutex_);
    startersContainer_iterator iter = starters_.begin();
    while ( iter != starters_.end() )
    {
        if ( iter->get() == starter )
        {
            starters_.erase(iter);

            Debug::trace("SessionManager.SessionStarter", "Removed SessionStarter " + util_string::ptrtostring(starter) +
                    " from starters container");

            return;
        }

        ++iter;
    }

    Debug::trace("SessionManager.SessionStarter", "Could not remove SessionStarter " + util_string::ptrtostring(starter) +
            "; did not find it in starters container");
}


SessionManager::SessionStarter::SessionStarter(SessionManager* sessionMgr, Socket* socket) :
	thread_(0),
	sessionMgr_(sessionMgr),
	socket_(socket),
	done_(false)
{
    Debug::trace("SessionManager.SessionStarter", "Creating SessionManager.SessionStarter object " +
            util_string::ptrtostring(this));

    thread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, "SessionStarter Thread");
}

SessionManager::SessionStarter::~SessionStarter()
{
	Debug::trace("SessionManager.SessionStarter", "Destroying SessionManager.SessionStarter object " +
            util_string::ptrtostring(this));

	if (thread_)
	{
		Debug::trace("SessionManager.SessionStarter", "Ending session starter thread...");

		thread_->StopThread();
		thread_->WaitForCompletion();

		// this will delete thread
		thread_ = NULL;
	}

	Debug::trace("SessionManager.SessionStarter", "Destroying done.");
}


DWORD SessionManager::SessionStarter::Run()
{
    char buffer[16 * 1024]; // CSM-120
    int bytesReceived;

    bool uiConnection = false;
    bool timedout = false;
    bool invalid = false;

    // (uc1.7) Peek at the communication header (only waiting for a limited amount of time).
    const int maxTimeToWait( Configuration::getInstance()->toIntCfgI(CfgInternal::CONNECTIONIDENTIFICATIONTIMER) );
    const int timeDelay( Configuration::getInstance()->toIntCfgI(CfgInternal::CONNECTIONIDENTIFICATIONDELAY) );

    Debug::trace("SessionManager.SessionStarter",
    	"Max time to wait: " + Integer::toString(maxTimeToWait) + ", "
		"Time delay: " + Integer::toString(timeDelay));

    const util_time::Time begin;

    for ( ;; )
    {
        // If certain amount of time has elapsed and we have not received any data yet, then quit.
        const util_time::Time now;
        const util_time::Time::Interval delta( now - begin );

        if ( maxTimeToWait < delta.getTotalSeconds() )
        {
            Debug::trace("SessionManager.SessionStarter", "Timed out: did not receive any byte of data");
            timedout = true;
            break;
        }

		const int numMsgs = socket_->cmNumMsgs();

        if ( 0 < numMsgs )
        {
            Debug::trace("SessionManager.SessionStarter", "Process message...");

            bytesReceived = socket_->receive(buffer, sizeof(buffer) - 1, true);

            Debug::trace("SessionManager.SessionStarter", "Process message... got " + Integer::toString(bytesReceived) + " bytes");

            if ( 0 < bytesReceived )
            {
                // The following check is performed so that when a byte like 0x80 is read we don't pass
                // a negative integer value to the 'isdigit' function (which causes a debuging assertion
                // to trigger an exception and the application to crash).
                if ( *buffer < 0 )
                {
                    Debug::trace("SessionManager.SessionStarter", "Received invalid input. Connection rejected.");
                    invalid = true;
                }
                else if ( isdigit(*buffer) )
                {
                    Debug::trace("SessionManager.SessionStarter", "Have UI connection.");
                    uiConnection = true;
                }

                break;
            }
            else if ( bytesReceived < 0 )
            {
                Debug::trace("SessionManager.SessionStarter", "Bad socket receive. Connection rejected.");
                invalid = true;
                break;
            }
        }
		else if ( numMsgs < 0 )
		{
			Debug::trace("SessionManager.SessionStarter", "Bad socket receive. Connection rejected.");
			invalid = true;
			break;
		}

        Debug::trace("SessionManager.SessionStarter", "Waiting for comm...");

        // When we boot up and accept the connection from the primary console UI, if we don't have
        // a significant delay here we run the risk of timing out without getting any bytes from the
        // UI. The latter appears to attempt the connection again, which sometimes fails in the same
        // manner, and then it gives up leaving the UI on the logos screen for ever.
        PAL_absclass::PAL_sleep(timeDelay);
    }


    if (( timedout ) || ( invalid ))
    {
        Debug::trace("SessionManager.SessionStarter", "Close socket.");
        socket_->cmClose();

        delete socket_;
        socket_ = 0;
    }
    else if ( uiConnection )
    {
        Debug::trace("SessionManager.SessionStarter", "Creating UI session...");

        // Only a console session would send data where the first character is a digit.
        // They either come from a local or remote console.
        Session* session = new Session(socket_, sessionMgr_->isInputProcessingEnabled());
		
		// is this sleep really needed??
		// PAL_absclass::PAL_sleep(100);
        
		socket_ = 0;

		// Don't use a message sent to myself to add the Session, just call addSession directly
		//AddNewSessionMessage message(session);
		//Hobart::CMessageCenter::instance() << &message;
		SessionManager::getInstance()->addSession(session);
    }
    else
	{
		// (uc1.7) Peek at the communication header (only waiting for a limited amount of time).
		// Ensure that we have received the full header before peeking at it.
		while ( !recordSeparatorPresent(buffer, &bytesReceived) )
		{
	        const util_time::Time now;
	        const util_time::Time::Interval delta( now - begin );

			if ( maxTimeToWait < delta.getTotalSeconds() )
			{
				Debug::trace("SessionManager.SessionStarter", "Timed out: did not receive full header");
				timedout = true;
				break;
			}

	        Debug::trace("SessionManager.SessionStarter", "Waiting for RS...");

			bytesReceived = socket_->receive(buffer, sizeof(buffer) - 1, true);

            Debug::trace("SessionManager.SessionStarter", "Waiting for RS... got " + Integer::toString(bytesReceived) + " bytes");

            if ( bytesReceived < 0 )
            {
                Debug::trace("SessionManager.SessionStarter", "Bad socket receive. Close socket.");
                socket_->cmClose();

                delete socket_;
                socket_ = 0;

                timedout = true;
                break;
            }

            PAL_absclass::PAL_sleep(timeDelay);
		}

		Debug::trace("SessionManager.SessionStarter", "Received header: " + std::string(buffer, bytesReceived));

		if ( !timedout )
		{
			int recordType = 0;
			int dataType = 0;
			hfc::Scale::ScaleRole remoteScaleRole = hfc::Scale::RoleInvalid;

			HobString headerData(buffer, bytesReceived);
			HobString temp;

			HobTokenizerUS tokenizer(&headerData, UnitSeparator);
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *>(HobToken::create(temp, HobTaggedToken));

			if (token)
			{
				while ( tokenizer.next(token) != -1 )
				{
					Debug::trace("SessionManager.SessionStarter", "Process token");

					// Extract the RT tag
					if ( token->getTag() == "RT" )
					{
						recordType = util_string::stringtoint(token->getData());
						Debug::trace("SessionManager.SessionStarter", "Received header's RT tag: " + token->getData());
					}
					// Extract the DT tag
					else if ( token->getTag() == "DT" )
					{
						dataType = util_string::stringtoint(token->getData());
						Debug::trace("SessionManager.SessionStarter", "Received header's DT tag: " + token->getData());
					}
					else if ( token->getTag() == "SR" )
					{
						remoteScaleRole = (hfc::Scale::ScaleRole)util_string::stringtoint(token->getData());
						Debug::trace("SessionManager.SessionStarter", "Received header's SR tag: " + token->getData());
					}
				}

				delete token;
			}

			Debug::trace("SessionManager.SessionStarter", "Have DT " + Integer::toString(dataType) );

			// (uc1.7) Spawn the corresponding session to handle this connection.
			switch ( dataType )
			{
			// DT1 means HQ communications.
			case 1:
				{
					Session* session = new Session(socket_, sessionMgr_->isInputProcessingEnabled());

					// is this sleep really needed??
					// PAL_absclass::PAL_sleep(100);
					
					// The socket has been given to the Session, so just reset our reference to it.
					socket_ = 0;

					// Don't use a message sent to myself to add the Session, just call addSession directly
					//AddNewSessionMessage message(session);
					//Hobart::CMessageCenter::instance() << &message;
					SessionManager::getInstance()->addSession(session);

					break;
				}

			// DT2 means synchronization communications.
			case 2:
				{
					Debug::trace("SessionManager.SessionStarter", "Have Remote Scale Role " + Integer::toString(remoteScaleRole) );

					switch ( remoteScaleRole )
					{
					case hfc::Scale::RolePrimary:
						{
							socket_->setMyReaderWriter(new CMReaderWriterSyncTag());

							Debug::trace("SessionManager.SessionStarter", "Starting SecondaryCommSession");

							AbstractSession* session = new synchronization::SecondaryCommSession(Configuration::getInstance()->getScaleSynchronizationType(),
									socket_, sessionMgr_->isInputProcessingEnabled());
							PAL_absclass::PAL_sleep(100);
							socket_ = 0;

							// Don't use a message sent to myself to add the Session, just call addSession directly
							//AddNewSessionMessage message(session);
							//Hobart::CMessageCenter::instance() << &message;
							SessionManager::getInstance()->addSession(session);

							break;
						}

					case hfc::Scale::RoleSecondary:
						{
							socket_->setMyReaderWriter(new CMReaderWriterSyncTag());

							Debug::trace("SessionManager.SessionStarter", "Starting PrimaryCommSession");

							AbstractSession* session = new synchronization::PrimaryCommSession(Configuration::getInstance()->getScaleSynchronizationType(),
									socket_, sessionMgr_->isInputProcessingEnabled());
							PAL_absclass::PAL_sleep(100);
							socket_ = 0;

							// Don't use a message sent to myself to add the Session, just call addSession directly
							//AddNewSessionMessage message(session);
							//Hobart::CMessageCenter::instance() << &message;
							SessionManager::getInstance()->addSession(session);

							break;
						}

					default:
						{
							Debug::trace("SessionManager.SessionStarter",
									"Connection rejected: received connection from a scale with role " + util_string::inttostring(remoteScaleRole));

							socket_->cmClose();
							delete socket_;
							socket_ = 0;

							break;
						}
					} // switch remoteScaleRole

					break;
				}

			// datatype = 0 means DT was not received.  I created an internal config param so that if I
			// don't get the DT tag, as what is now happening to simplify comm with our Scale Management package,
			// then just treat it as normal comm.
			case 0:
				{
					if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::COMMREQUIRESDATATYPETAG))
					{
						Debug::trace("SessionManager.SessionStarter", "Connection accepted even though no DT Tag received");

						Session* session = new Session(socket_, sessionMgr_->isInputProcessingEnabled());

						// is this sleep really needed??
						// PAL_absclass::PAL_sleep(100);

						// The socket has been given to the Session, so just reset our reference to it.
						socket_ = 0;

						// Don't use a message sent to myself to add the Session, just call addSession directly
						//AddNewSessionMessage message(session);
						//Hobart::CMessageCenter::instance() << &message;
						session->setSessionType(AbstractSession::SESSIONTYPE_COMM);
						SessionManager::getInstance()->addSession(session);
						break;
					}

					// Leave this as is; I want it to fall through to the default case if the internal config param
					// indicates that we have to receive the DT tag to accept the comm Session, so no 'break' here.
				}

			default:
				{
					Debug::trace("SessionManager.SessionStarter",
							"Connection rejected: received connection from a scale using DT " + util_string::inttostring(dataType));

					socket_->cmClose();
					delete socket_;
					socket_ = 0;

					break;
				}
			} // switch dataType
		}
    }

    Debug::trace("SessionManager.SessionStarter",
		"Sending RemoveSessionStarterMessage for SesssionManager to remove me from list object " +
		util_string::ptrtostring(this));

    RemoveSessionStarterMessage message(this);
    Hobart::CMessageCenter::instance() << &message;
    socket_ = 0;

    done_ = true;

    Debug::trace("SessionManager.SessionStarter", "Thread complete!");
    return 0;
}


bool SessionManager::SessionStarter::recordSeparatorPresent(const char * buffer, int* bufferLength) const
{
    for ( int i = 0; i < *bufferLength; ++i )
    {
        if ( buffer[i] == '\x1E' )
        {
            *bufferLength = i + 1;
            return true;
        }
    }

    return false;
}


const std::string& SessionManager::getStatusText(int status) const
{
    switch ( status )
    {
        case _statusOK_:                                     return _statusOKText_;
        case _statusUnknownSynchronizationType_:             return _statusUnknownSynchronizationTypeText_;
        case _statusNoSynchronizationManager_:               return _statusNoSynchronizationManagerText_;
        case _statusCannotChangePrimaryIfNotSecondary_:      return _statusCannotChangePrimaryIfNotSecondaryText_;
        default:                                             return _statusUnknownText_;
    }
}
/*
   void SessionManager::RunTestCommon()
   {

   try
   {
   TestWorkerThread* pTestPauser1 = (TestWorkerThread *) new TestWorkerThread(true, false);
   TestWorkerThread* pTestCommon1 = (TestWorkerThread *) new TestWorkerThread(false, false);
   TestWorkerThread* pTestCommon2 = (TestWorkerThread *) new TestWorkerThread(false, false);
   TestWorkerThread* pTestCommon3 = (TestWorkerThread *) new TestWorkerThread(false, false);
   TestWorkerThread* pTestCommon4 = (TestWorkerThread *) new TestWorkerThread(false, false);


   PAL_absclass::PAL_sleep(30000);

   testDumpRegistry();

   delete pTestCommon4;

   PAL_absclass::PAL_sleep(10000);

   delete pTestCommon3;

   PAL_absclass::PAL_sleep(10000);

   delete pTestCommon2;

   PAL_absclass::PAL_sleep(10000);

   delete pTestCommon1;

   PAL_absclass::PAL_sleep(10000);

   delete pTestPauser1;


   Debug::trace("SessionManager.managedThreads", "run test Complete");

   }
   catch (Util::CTerminateNow&)
   {
   Debug::trace("SessionManager.managedThreads", "run test Complete");


   }
   }

   void SessionManager::testDumpRegistry()
   {

   ManagedThreadMapIterator i;

   char buff[16], buff1[16];

   sprintf(buff, "%04x", commonThreads_.size());
   sprintf(buff1, "%4x", superThreads_.size());
   Debug::trace("SessionManager.managedThreads",
   std::string("registry common threads size:") + std::string(buff) +
   std::string(" super threads size:") + std::string(buff1));


   for (i = commonThreads_.end(); i != commonThreads_.end();i++)
   {
   sprintf(buff, "%04x", commonThreads_.size());

   Debug::trace("SessionManager.managedThreads",
   std::string("common thread id = :") + std::string(buff));


   }

for (i = superThreads_.end(); i != superThreads_.end();i++)
{
    sprintf(buff, "%04x", superThreads_.size());

    Debug::trace("SessionManager.managedThreads",
            std::string("super thread id = :") + std::string(buff));


}
}*/

void SessionManager::registerThread(unsigned threadId, bool superThread)
{
    Util::CLockCriticalSection lock( managedThreadListCS_);

#ifndef EVOL_NDEBUG
    char buff[16], buff1[16];

    sprintf(buff, "%04x", threadId);
    sprintf(buff1, "%4x", superThread);
    Debug::trace("SessionManager.managedThreads",
            std::string("register id:") + std::string(buff) +
            std::string(" super Thread:") + std::string(buff1));
#endif

    threadStatus ts;
    ts.safe = true;
    ts.status = 0;

    if (superThread)
    {
        superThreads_.insert(make_pair(threadId, ts));
    }
    else
    {
        commonThreads_.insert(make_pair(threadId, ts));
    }

}

void SessionManager::deRegisterThread(unsigned threadId, bool superThread)
{
    Util::CLockCriticalSection lock( managedThreadListCS_);

#ifndef EVOL_NDEBUG
    char buff[16], buff1[16];

    sprintf(buff, "%04x", threadId);
    sprintf(buff1, "%4x", superThread);
    Debug::trace("SessionManager.managedThreads",
            std::string("deregister id:") + std::string(buff) +
            std::string(" super Thread:") + std::string(buff1));
#endif

    if (superThread)
    {
        superThreads_.erase(threadId);
    }
    else
    {
        commonThreads_.erase(threadId);
    }

    updatePauseInitiatorEvents();
}

bool SessionManager::managedThreadWouldBlock(bool superThread)
{
    if (superThread)
        return !blockSuperThreadsEvent_.IsEventSet();
    else
        return !blockSuperThreadsEvent_.IsEventSet();

}

UINT32 SessionManager::getLastBlockedStatus(unsigned threadId, bool superThread)
{
    Util::CLockCriticalSection lock( managedThreadListCS_);

    ManagedThreadMapIterator i;

    if (superThread)
    {
        i = superThreads_.find(threadId);

        if (i == superThreads_.end())
            return 0;

    }
    else
    {
        i = commonThreads_.find(threadId);

        if (i == commonThreads_.end())
            return 0;
    }

    UINT32 status = i->second.status;
    i->second.status = 0;

    return status;
}


UINT32 SessionManager::requestToContinue(unsigned threadId, bool superThread)
{
    UINT32 blockedBitmask = 0;

    if (!changeBlockabilityStatus(threadId, true, superThread))
    {
#ifndef EVOL_NDEBUG
        char buff[16];

        sprintf(buff, "%04x", threadId);
        Debug::trace("SessionManager.managedThreads",
                std::string("thread is not registered id:") + std::string(buff));

#endif
        return 0;
    }


#ifndef EVOL_NDEBUG
    if (!blockSuperThreadsEvent_.IsEventSet())
    {
        char buff[16];

        sprintf(buff, "%04x", threadId);
        Debug::trace("SessionManager.managedThreads",
                std::string("rtc blocking super id:") + std::string(buff));

    }
    if (!blockCommonThreadsEvent_.IsEventSet())
    {
        char buff[16];

        sprintf(buff, "%04x", threadId);
        Debug::trace("SessionManager.managedThreads",
                std::string("rtc blocking common id:") + std::string(buff));

    }
#endif


    if (superThread)
        blockSuperThreadsEvent_.Wait();
    else
        blockCommonThreadsEvent_.Wait();

    //dzone

    changeBlockabilityStatus(threadId, false, superThread);

    blockedBitmask = getLastBlockedStatus(threadId, superThread);

#ifndef EVOL_NDEBUG
    if (blockedBitmask > 0)
    {
        char buff[16];

        sprintf(buff, "%04x", threadId);

        Debug::trace("SessionManager.managedThreads",
                std::string("releasing blocked common thread id:") + Hex::toString(threadId));
    }
#endif
    return blockedBitmask;

}


bool SessionManager::changeBlockabilityStatus(unsigned threadId, bool outsideOfCS, bool superThread)
{
    Util::CLockCriticalSection lock( managedThreadListCS_);

    ManagedThreadMapIterator i;

    if (superThread)
    {
        i = superThreads_.find(threadId);

        if (i == superThreads_.end())
            return false;

    }
    else
    {
        i = commonThreads_.find(threadId);

        if (i == commonThreads_.end())
            return false;
    }

    i->second.safe = outsideOfCS;

    updatePauseInitiatorEvents();

    return true;
}



void SessionManager::updatePauseInitiatorEvents()
{

    bool commonsSafeToPause = areAllCommonThreadsSafe();
    bool supersSafeToPause = areAllSuperThreadsSafe(); // RANDY TEST

    if (commonsSafeToPause && supersSafeToPause)
    {
        blockPauseInitiatorAllEvent_.EventSet();
        blockPauseInitiatorCommonEvent_.EventSet();
    }
    else if (commonsSafeToPause)
    {
        blockPauseInitiatorAllEvent_.EventReset();
        blockPauseInitiatorCommonEvent_.EventSet();
    }
    else
    {
        blockPauseInitiatorAllEvent_.EventReset();
        blockPauseInitiatorCommonEvent_.EventReset();
    }

}

bool SessionManager::areAllCommonThreadsSafe()
{

    ManagedThreadMapIterator i;

    for (i = commonThreads_.begin(); i != commonThreads_.end(); i++)
    {
        if (i->second.safe == false)
        {
#ifndef EVOL_NDEBUG
        	char buff[16];

        	sprintf(buff, "%04x", i->first);

			Debug::trace("SessionManager.managedThreads",
					std::string("common thread id is not safe:") + Hex::toString(i->first));
#endif
            return false;
        }
    }
    return true;

}

bool SessionManager::areAllSuperThreadsSafe()
{

    ManagedThreadMapIterator i;

    for (i = superThreads_.begin(); i != superThreads_.end(); i++)
    {
        if (i->second.safe == false)
        {
#ifndef EVOL_NDEBUG
        	char buff[16];

        	sprintf(buff, "%04x", i->first);

			Debug::trace("SessionManager.managedThreads",
					std::string("super thread id is not safe:") + Hex::toString(i->first));
#endif
            return false;
        }
    }

    return true;
}

void SessionManager::markManagedThreadsStatus(UINT32 status)
{
    ManagedThreadMapIterator i;

    Util::CLockCriticalSection lock( managedThreadListCS_);

    for (i = commonThreads_.begin(); i != commonThreads_.end(); i++)
    {
        i->second.status |= status;
    }

    for (i = superThreads_.begin(); i != superThreads_.end(); i++)
    {
        i->second.status |= status;
    }

}

void SessionManager::requestToPause(unsigned threadId, bool blockAll, bool superThread)
{
#ifndef EVOL_NDEBUG
    char buff[16], buff1[16];

    sprintf(buff, "%04x", threadId);
    sprintf(buff1, "%4x", blockAll);
    Debug::trace("SessionManager.managedThreads",
            std::string("request to pause id:") + std::string(buff) +
            std::string(" block all:") + std::string(buff1));
#endif

    if (threadId != 0)
        changeBlockabilityStatus(threadId, true, superThread);

    PAL_absclass::PAL_InterlockedIncrement(&queuedPausers_);

    blockAllRequestsToPauseEvent_.Wait();
    blockAllRequestsToPauseEvent_.EventReset();     //block future callers.

    PAL_absclass::PAL_InterlockedDecrement(&queuedPausers_);

    //at this point we know that we are not blocking any managed threads yet.


    if (blockAll)
    {

        blockCommonThreadsEvent_.EventReset();      //block common threads
        blockSuperThreadsEvent_.EventReset();       //block super threads

        if (!panicShutdown_)
           PAL_absclass::PAL_sleep(3000);              //let managed threads in dzone continue first.

        blockPauseInitiatorAllEvent_.Wait();

        markManagedThreadsStatus(COMMON_BLOCKED | SUPER_BLOCKED);
    }
    else
    {

        blockCommonThreadsEvent_.EventReset();      //block common threads.
        blockSuperThreadsEvent_.EventSet();         //allow super thread to proceed.

        PAL_absclass::PAL_sleep(500);               //let managed threads in dzone continue first.

        blockPauseInitiatorCommonEvent_.Wait();

        markManagedThreadsStatus(COMMON_BLOCKED);
    }

    if (panicShutdown_)
       PAL_absclass::PAL_sleep(500);

#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager.managedThreads",
            std::string("pause initiator proceeding with critical database function"));
#endif
}


void SessionManager::resumePausedThreads()
{

#ifndef EVOL_NDEBUG
    Debug::trace("SessionManager.managedThreads",
            std::string("resumePausedThreads") );
#endif

    if (queuedPausers_ == 0)
    {
#ifndef EVOL_NDEBUG
        Debug::trace("SessionManager.managedThreads",
                std::string("unblocking all managed threads that are currently blocked queuedPausers = ") + Integer::toString(queuedPausers_));
#endif

        blockCommonThreadsEvent_.EventSet();//unblock common threads
        blockSuperThreadsEvent_.EventSet();//unblock super threads
    }

    blockAllRequestsToPauseEvent_.EventSet();//unblock pause requestor threads
}


void SessionManager::addDialog(int errorDevice, int errorCode, const std::string msgData)
{
	if (errorDevice == 0 && errorCode == 999)
	{
#if !defined ( __linux__ )
		const char * aa = msgData.c_str();
		MessageBoxA(NULL,aa,"Error",MB_ICONSTOP |MB_OK);
#endif
	}
	else if (primaryConsoleSession)
	{
		if (msgData.size())
		{
			HobList data;
			data.setQuotingElements(true);
			data.push_back(msgData);
			primaryConsoleSession->addDialogMessage((ErrorEvent)errorDevice, errorCode, HfcResponse::RESPONSESTYLE_OK, &data, true);
		}
		else
			primaryConsoleSession->addDialogMessage((ErrorEvent)errorDevice, errorCode, HfcResponse::RESPONSESTYLE_OK);
	}
}

void SessionManager::removeDialog(int errorDevice, int errorCode)
{
	if (errorDevice == 0 && errorCode == 999)
	{
		// nothing to do
	}
	else if (primaryConsoleSession)
	{
		primaryConsoleSession->removeDialogMessage((ErrorEvent)errorDevice, errorCode, HfcResponse::RESPONSESTYLE_OK);
	}
}

void SessionManager::sendWrapperReset()//Added 4/24/14 Alex Zavala
{
	EvolMessage reqWrapperSystemReset(Wrapper, WrEvolReqSystemReset);
	dispatchEvent(&reqWrapperSystemReset);
}

void SessionManager::sendLabelApplierReset()//Added 4/24/14 Alex Zavala
{
	EvolMessage reqLabelApplierSystemReset(LabelApplier, LaReqSystemReset);
	dispatchEvent(&reqLabelApplierSystemReset);
}


//	This is a carryover feature from the vxWorks Access, modified to work on Windux.
//	It will display the memory of the system, if the category "MonitorMemory" is turned on for Debug.
//	It will also display the Accountant counters, if the code has been built with -DUSE_ACCOUNTANT
//	and the category "Accountant.showSortedCounters" is turned on for Debug.
void SessionManager::monitorMemory()
{
	bool traceOn = Debug::isTracing("MonitorMemory");
	HobString msgDebug = "SessionManager::monitorMemory():  ";
	if (traceOn)
	{
		Debug::trace("MonitorMemory", msgDebug + "Entering   >>>>>>>>>>>>>>>>");

		HobString driveName = Configuration::getInstance()->getCompactFlashDriveName();

		long64 flashUsedBytes = 0;
		long64 flashFreeBytes = 0;
		long64 ramUsedBytes = 0;
		long64 ramFreeBytes = 0;

		try
		{
			util_platform::Platform& plat = util_platform::Platform::getInstance();

			// Amount of used and free flash memory
			// Browser wants this in KB, not bytes.  So divde the number of bytes by 1024.
			flashUsedBytes = plat.fileSystemUsedSpace(driveName);
			flashFreeBytes = plat.fileSystemFreeSpace(driveName);

			// Amount of used and free RAM memory
			PAL_MEMORYSTATUS memStats; 
			PAL_absclass::PAL_globalMemoryStatus(&memStats);

			ramUsedBytes = memStats.dwTotalPhys - memStats.dwAvailPhys;
			ramFreeBytes = memStats.dwAvailPhys;
		}
		catch (edb::DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("Statistics::init() ********** database exception **********"));
				edb::DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("	 DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
		catch (...)
		{
			Debug::trace("MonitorMemory", "*****************************************************************");
			Debug::trace("MonitorMemory", "****************** Just got an unknown exception ****************");
			Debug::trace("MonitorMemory", "*****************************************************************");
		}

		Debug::trace("MonitorMemory", msgDebug + "All data is in bytes.");
		Debug::trace("MonitorMemory", msgDebug + "       ramFree = " + HobString(ULong::toString((unsigned long)ramFreeBytes)));
		Debug::trace("MonitorMemory", msgDebug + "       ramUsed = " + HobString(ULong::toString((unsigned long)ramUsedBytes)));
		Debug::trace("MonitorMemory", msgDebug + "     flashFree = " + HobString(ULong::toString((unsigned long)flashFreeBytes)));
		Debug::trace("MonitorMemory", msgDebug + "     flashUsed = " + HobString(ULong::toString((unsigned long)flashUsedBytes)));
	}

#ifdef USE_ACCOUNTANT
	Debug::trace("Accountant.showSortedCounters", msgDebug + "*****  ACCOUNTANT INFO  *****");
	util_acc::showInstanceCounters(false);
#endif

	if (traceOn)
	{
		Debug::trace("MonitorMemory", msgDebug + "Leaving    <<<<<<<<<<<<<<<");
	}
}


bool SessionManager::isDeleteAllFilesInProgress() const
{
	return deleteAllFilesInProgress_;
}


// Check the sealable metrological parameters that are not stored in the Peripheral
// Controller's serial flash; they are stored in the System Controller's serial flash.
void SessionManager::checkMetrologySealableParameters()
{
	// Check if the fractional pricing enabled flag or the pounds-for enabled flag in the database
	// are set to the same value as they are in the serial flash.
	// Note:  The database has to be already open to check the flags.
	// Note:  if they aren't in the serial flash, set them up, and write a record to the audit trail
	// Note:  If we are booting up right after an upgrade, have to wait until the database is reloaded to check the flags.
	// Note:  We have to check them anytime configuration is saved to the database, so we will listen
	//        for the ConfigurationDev|CfgSaved message as the indication to perform this check.

	bool isTracing = Debug::isTracing("cat3");

	if (isTracing)
		Debug::trace("cat3", "checkMetrologySealableParameters beginning");

	Configuration *cfg = Configuration::getInstance();
	if ( !cfg )
		return;
	if (isTracing)
		Debug::trace("cat3", "checkMetrologySealableParameters have config");

	if ( !cfg->isCAT3AuditTrailLicensed() )
		return;

	FpmManager *fpm = FpmManager::getInstance();
	if ( !fpm )
		return;
	if (isTracing)
		Debug::trace("cat3", string("checkMetrologySealableParameters have fpm: ") + util_string::ptrtostring(fpm));
 	
	std::string style = fpm->getMFGSystemStyle();
	MtdScaleSWConfig *swCfg = fpm->readScaleSWConfigData();	
	if ( !swCfg )
	{
		string msg = "Could not get the Software Section of the System Controller's Serial Flash when checking Metrology Sealable Parameters!";
		MessageLogger::logMessage(MessageLogger::InfoLog, msg);
		if (isTracing)
			Debug::trace("cat3", msg);
		return;
	}
	if (isTracing)
	{
		Debug::trace("cat3", string("checkMetrologySealableParameters have MtdScaleSWConfig  swCfg: ") + util_string::ptrtostring(swCfg));
		swCfg->show();
	}


	string fpcWeigherVer = cfg->toStringCfg(fieldOffsetCfgFPCWeigherVersion);
	int major = 0;
	int minor = 0;


	// Send the current date/time to the FPC in case any audit records are written below
	time_t now;
	time(&now);
	WeigherCat3DateTime wCat3DateTime( this, (UINT32) now );
	dispatchEvent( &wCat3DateTime );
	PAL_absclass::PAL_sleep(100);


	HobTokenizerCSV tok( &fpcWeigherVer, HobVersion::VERSION_SEPERATOR);
	HobString token;
	if (tok.next(&token) != -1)
		major = Integer::valueOf(token);
	else
	{
		if (isTracing)
			Debug::trace("cat3", "checkMetrologySealableParameters - no major metrology version number determined, so just ignore it");
		return;
	}

	// All HTm, FSS, and ARK boards support cat3 with very first release version 
	if( ( style != "HTK" )  && ( style != "FSS" ) ) {
		if ( major < 4 )
		{
			if (isTracing)
				Debug::trace("cat3", "checkMetrologySealableParameters - major metrology version number is less than the very first that supported cat3, so just ignore it");
			return;
		}
	}

    char str[INT_STR_SIZE];
	sprintf(str,"%03d",major);
	string allTogetherNow(str);

	// minor and engRev together
	allTogetherNow.append((tok.next(&token) != -1) ? token : "   ");

	// application version
	allTogetherNow.append((tok.next(&token) != -1) ? token : "   ");

	if (isTracing)
	    Debug::trace("cat3", string("checkMetrologySealableParameters - put together applMetrologyVersion:") + allTogetherNow);
	
	unsigned long applMetrologyVersion = Long::valueOf(allTogetherNow); 
	unsigned long flashMetrologyVersion = swCfg->getMetrologicalVersion();
	if (isTracing)
	    Debug::trace("cat3", string("checkMetrologySealableParameters - applMetrologyVersion:") + util_string::longtostring(applMetrologyVersion) + string("  flashMetrologyVersion:") + util_string::longtostring(flashMetrologyVersion));

	if ( flashMetrologyVersion != applMetrologyVersion )
	{
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - flashMetrologyVersion is different between serial flash and application, setting flash to: ") + std::string(longtostring(applMetrologyVersion)));

		// They're different, so write the current setting to the serial flash, and add an entry to the Weigher's Audit Trail.
		//swCfg->setMetrologicalVersion( applMetrologyVersion );

		SessionMessage msg(MfgTestDev, WriteOfScaleSWConfigMetrologicalVersion);
		GenericHfcObject gho( Long::toString(applMetrologyVersion) );
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting MetrologicalVersion: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set MetrologicalVersion result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) UPGRADE );
		cat3Rec.setParamId( (SealableParamID) SOFTWARE_UPDATE );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long) applMetrologyVersion );
		cat3Rec.setPreviousValue( (unsigned long) flashMetrologyVersion );
		dispatchEvent(&cat3Rec);
	}


	bool fpEnabled = swCfg->getFractionalPricingEnabled();
	if ( isTracing )
	    Debug::trace("cat3", string("checkMetrologySealableParameters - flash fpEnabled:") + Bool::toString(fpEnabled) + string("  in cfg:") + cfg->toStringCfg(fieldOffsetCfgFractionalPricingEnabled));

	if ( fpEnabled != cfg->toBoolCfg(fieldOffsetCfgFractionalPricingEnabled) )
	{
		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - FractionalPricingEnabled is different between serial flash and db, setting flash to: " + cfg->toStringCfg(fieldOffsetCfgFractionalPricingEnabled));

		// They're different, so write the current setting to the serial flash, and add an entry to the Weigher's Audit Trail.
		//swCfg->setFractionalPricingEnabled( cfg->toStringCfg(fieldOffsetCfgFractionalPricingEnabled) );

		SessionMessage msg(MfgTestDev, WriteOfScaleSWConfigFractionalPrice);
		GenericHfcObject gho( cfg->toStringCfg(fieldOffsetCfgFractionalPricingEnabled) );
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting FractionalPrice: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set FractionalPrice result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) UPGRADE );
		cat3Rec.setParamId( (SealableParamID) FRACTIONAL_PRICING );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long) cfg->toIntCfg(fieldOffsetCfgFractionalPricingEnabled) );
		cat3Rec.setPreviousValue( (unsigned long) fpEnabled );
		dispatchEvent(&cat3Rec);
	}


	bool pfEnabled = swCfg->getPoundsForEnabled();
	if ( isTracing )
		Debug::trace("cat3", string("checkMetrologySealableParameters - flash pfEnabled:") + Bool::toString(pfEnabled) + string("  in cfg:") + cfg->toStringCfg(fieldOffsetCfgPoundsForPricingEnabled));

	if ( pfEnabled != cfg->toBoolCfg(fieldOffsetCfgPoundsForPricingEnabled) )
	{
		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - PoundsForPricingEnabled is different between serial flash and db, setting flash to: " + cfg->toStringCfg(fieldOffsetCfgPoundsForPricingEnabled));

		// They're different, so write the current setting to the serial flash, and add an entry to the Weigher's Audit Trail.
		//swCfg->setPoundForEnabled( cfg->toStringCfg(fieldOffsetCfgPoundsForPricingEnabled) );

		SessionMessage msg(MfgTestDev, WriteOfScaleSWConfigPoundsFor);
		GenericHfcObject gho( cfg->toStringCfg(fieldOffsetCfgPoundsForPricingEnabled) );
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting PoundsForPricingEnabled: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set PoundsForPricingEnabled result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) UPGRADE );
		cat3Rec.setParamId( (SealableParamID) POUNDS_FOR_MULTIPLIER );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long) cfg->toIntCfg(fieldOffsetCfgPoundsForPricingEnabled) );
		cat3Rec.setPreviousValue( (unsigned long) pfEnabled );
		dispatchEvent(&cat3Rec);
	}


	CfgCurrency myDbCurrency;
	myDbCurrency = cfg->getCfgCurrency();

	if ( isTracing ){
			
		Debug::trace("cat3", string(" Currency Data: Database "));
		Debug::trace("cat3", string(" char decimalSeparator_;") 		+ myDbCurrency.getDecimalSeparator());
		Debug::trace("cat3", string(" MonetaryRule monetaryRule_;") 	+ Integer::toString(myDbCurrency.getMonetaryRule()));
		Debug::trace("cat3", string(" short totalPriceTotalDigits_;") 	+ Integer::toString(myDbCurrency.getTotalPriceTotalDigits()));
		Debug::trace("cat3", string(" short unitPriceTotalDigits_;") 	+ Integer::toString(myDbCurrency.getUnitPriceTotalDigits()));
		Debug::trace("cat3", string(" short roundingFactor_;") 			+ Integer::toString(myDbCurrency.getRoundingFactor()));
		Debug::trace("cat3", string(" Rounding::Method roundingMethod_;") 			+ Integer::toString(myDbCurrency.getRoundingMethod()));
		Debug::trace("cat3", string(" HobString primaryMonetarySymbol_;") 			+ myDbCurrency.getPrimaryMonetarySymbol());
		Debug::trace("cat3", string(" short primaryMonetaryDecimalDigits_;") 		+  Integer::toString(myDbCurrency.getPrimaryMonetaryDecimalDigits()));
		Debug::trace("cat3", string(" HobString secondaryMonetarySymbol_;") 		+  myDbCurrency.getSecondaryMonetarySymbol());
		Debug::trace("cat3", string(" short secondaryMonetaryDecimalDigits_;") 		+ Integer::toString(myDbCurrency.getSecondaryMonetaryDecimalDigits()));

		
	}


	//Primary Currency 

	//Primary_Currency_Symbol from Database (upto 3 chars)
	HobString moneySymbolStringDb(myDbCurrency.getPrimaryMonetarySymbol());
	Debug::trace("cat3", string(" HobString moneySymbolStringDb(myDbCurrency.getPrimaryMonetarySymbol());") 	+  moneySymbolStringDb);
	
	//Primary_Currency_Symbol from Serial Flash		
	string moneySymbolStringFlash = swCfg->getPrimaryCurrencySymbol();
	Debug::trace("cat3", string(" string moneySymbolStringFlash = swCfg->getPrimaryCurrencySymbol();") 	+ moneySymbolStringFlash);

	if ( moneySymbolStringDb != moneySymbolStringFlash)
	{
		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - primary_currency_symbol_ is different between serial flash and db, setting flash to: " + moneySymbolStringDb);

		//save current value, will be used as "previous" later.
		unsigned long moneySymbolPrevious	= 0;
		unsigned char *mychar_ptr 			= (unsigned char *)&moneySymbolPrevious;

		*mychar_ptr	= (unsigned char)moneySymbolStringFlash[0];		

		//second char
		mychar_ptr ++;
		*mychar_ptr	= (unsigned char)moneySymbolStringFlash[1];			

		//third char
		mychar_ptr ++;
		*mychar_ptr	= (unsigned char)moneySymbolStringFlash[2];		


		Debug::trace("cat3", std::string("checkMetrologySealableParameters - Current: moneySymbolStringFlash ") + ULong::toString(moneySymbolPrevious) );
		
				
		SessionMessage msg(MfgTestDev, WriteOfPrimaryMonetarySymbol);
		GenericHfcObject gho( myDbCurrency.getPrimaryMonetarySymbol());
		msg.setHfcObject(&gho);
		
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting primary_currency_symbol_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set primary_currency_symbol_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) PRIMARY_MONETARY_SYMBOL );
		
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );

		//save NEW value, 
		unsigned long moneySymbolNew	= 0;
		mychar_ptr 						= (unsigned char *)&moneySymbolNew;

		*mychar_ptr = (unsigned char)moneySymbolStringDb[0];	

		//second char
		mychar_ptr ++;	
		*mychar_ptr = (unsigned char)moneySymbolStringDb[1];	

		//third char
		mychar_ptr ++;
		*mychar_ptr = (unsigned char)moneySymbolStringDb[2];		

		cat3Rec.setNewValue( moneySymbolNew );
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - cat3Rec.setNewValue( strtoul(moneySymbolStringDb.data(), NULL, 10) );: ") + ULong::toString(moneySymbolNew) );
		cat3Rec.setPreviousValue( moneySymbolPrevious );
		dispatchEvent(&cat3Rec);

	}


	//Unit Separator
	if ( myDbCurrency.getDecimalSeparator() != swCfg->getUnitSeparator() )
	{
		//save current value, will be used as "previous" later.
		unsigned char current_value 	= swCfg->getUnitSeparator();//Flash
		char db_unit_separator 			= myDbCurrency.getDecimalSeparator();//Db

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - decimalSeparator_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getDecimalSeparator()));

		
		SessionMessage msg(MfgTestDev, WriteOfUnitSymbolSeparator);
		GenericHfcObject gho( &db_unit_separator);
		msg.setHfcObject(&gho);
		
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting decimalSeparator_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set decimalSeparator_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) DECIMAL_SEPARATOR );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( db_unit_separator );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
		
	}


	//Currency Symbol Placement
	if ( myDbCurrency.getMonetaryRule() != swCfg->getCurrencySymbolPlacement() )
	{
		//save current value, will be used as "previous" later.
		short current_value = swCfg->getCurrencySymbolPlacement();//flash

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - currency_symbol_placement_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getMonetaryRule()));

		
		SessionMessage msg(MfgTestDev, WriteOfCurrencySymbolPlacement);
		GenericHfcObject gho( Integer::toString(myDbCurrency.getMonetaryRule()));
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting currency_symbol_placement_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set currency_symbol_placement_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) MONETARY_RULE );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long)myDbCurrency.getMonetaryRule() );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
	}

	//Total Price Digits
	if ( myDbCurrency.getTotalPriceTotalDigits() != swCfg->getTotalPriceDigits() )
	{
		//save current value, will be used as "previous" later.
		short current_value = swCfg->getTotalPriceDigits();

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - totalPriceTotalDigits_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getTotalPriceTotalDigits()));

		
		SessionMessage msg(MfgTestDev, WriteOfTotalPriceDigits);
		GenericHfcObject gho( Integer::toString(myDbCurrency.getTotalPriceTotalDigits()));
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting totalPriceTotalDigits_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set totalPriceTotalDigits_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) TOTAL_PRICE_DIGITS );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long)myDbCurrency.getTotalPriceTotalDigits() );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
	}

	//Unit Price Digits
	if ( myDbCurrency.getUnitPriceTotalDigits() != swCfg->getUnitPriceDigits() )
	{
		//save current value, will be used as "previous" later.
		short current_value = swCfg->getUnitPriceDigits();

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - unitPriceTotalDigits_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getUnitPriceTotalDigits()));

		
		SessionMessage msg(MfgTestDev, WriteOfUnitPriceDigits);
		GenericHfcObject gho( Integer::toString(myDbCurrency.getUnitPriceTotalDigits()));
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting unitPriceTotalDigits_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set unitPriceTotalDigits_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) UNIT_PRICE_DIGITS );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long)myDbCurrency.getUnitPriceTotalDigits() );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
	}

	//Primary Precision Digits
	if ( myDbCurrency.getPrimaryMonetaryDecimalDigits() != swCfg->getPrimaryPrecisionDigits() )
	{
		//save current value, will be used as "previous" later.
		short current_value = swCfg->getPrimaryPrecisionDigits();

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - primaryMonetaryDecimalDigits_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getPrimaryMonetaryDecimalDigits()));

		
		SessionMessage msg(MfgTestDev, WriteOfPrimaryPrecisionDigits);
		GenericHfcObject gho( Integer::toString(myDbCurrency.getPrimaryMonetaryDecimalDigits()));
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting primary_precision_digits_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set primary_precision_digits_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) PRIMARY_MONETARY_DECIMAL_DIGITS );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long)myDbCurrency.getPrimaryMonetaryDecimalDigits() );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
	}

	//Rounding Method
	if ( myDbCurrency.getRoundingMethod() != swCfg->getCurrencyRoundingMethod() )
	{
		//save current value, will be used as "previous" later.
		short current_value = swCfg->getCurrencyRoundingMethod();

		if ( isTracing )
			Debug::trace("cat3", "checkMetrologySealableParameters - currency_rounding_method_ is different between serial flash and db, setting flash to: " + Integer::toString(myDbCurrency.getRoundingMethod()));

		
		SessionMessage msg(MfgTestDev, WriteOfCurrencyRoundingMethod);
		GenericHfcObject gho( Integer::toString(myDbCurrency.getRoundingMethod()));
		msg.setHfcObject(&gho);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm setting currency_rounding_method_: ") + gho.getData());
		bool result = fpm->handleWriteMessage(&msg);
		if ( isTracing )
			Debug::trace("cat3", std::string("checkMetrologySealableParameters - fpm set currency_rounding_method_ result: ") + Bool::toString(result) );

		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) CONFIGURATION );
		cat3Rec.setParamId( (SealableParamID) ROUNDING_METHOD );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long)myDbCurrency.getRoundingMethod() );
		cat3Rec.setPreviousValue( current_value );
		dispatchEvent(&cat3Rec);
	}


}



void SessionManager::checkMetrologySealableParametersLots()
{
	Debug::trace("cat3", "checkMetrologySealableParametersLots");

	int j = Configuration::getInstance()->toIntCfgI("metrologyRecs");
	Debug::trace("cat3", std::string("checkMetrologySealableParametersLots - num of recs: ") + Integer::toString(j) );


	for (int i=0; i < j; i++)
	{
		{
		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) UPGRADE );
		cat3Rec.setParamId( (SealableParamID) FRACTIONAL_PRICING );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long) 1 );
		cat3Rec.setPreviousValue( (unsigned long) 0 );
		dispatchEvent(&cat3Rec);
		PAL_absclass::PAL_sleep(100);
		}
		{
		WeigherWriteCat3Record cat3Rec(this);
		cat3Rec.setAuditType( (AuditType) REMOTE_EVENT );
		cat3Rec.setEvent( (AuditEvent) UPGRADE );
		cat3Rec.setParamId( (SealableParamID) FRACTIONAL_PRICING );
		time_t now;
		time(&now);
		cat3Rec.setEpochTime( now );
		cat3Rec.setNewValue( (unsigned long) 0 );
		cat3Rec.setPreviousValue( (unsigned long) 1 );
		dispatchEvent(&cat3Rec);
		PAL_absclass::PAL_sleep(100);
		}
	}
}
