/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

//using namespace std;

#include "evolution.h"
#include "BackupRestoreManager.h"
#include "BackupRestoreSelections.h"
#include "BackupRestoreProgress.h"
#include "BackupRestoreErrors.h"
#include "BackupRestoreInitiateReadAllMsg.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "CMReaderWriterEvolTag.h"
#include "CMReaderWriterDBIndependent.h"
#include "CMReaderWriterUltimaDisk.h"
#include "CMFile.h"

#include "ProductManager.h"
#include "ClassManager.h"
#include "DeptManager.h"
#include "NutrifactsManager.h"
#include "ExpirationLabelManager.h"
#include "ProductTextManager.h"
#include "LabelTypeManager.h"
#include "GraphicManager.h"
#include "TransactionTypeManager.h"

#include "CategoryManager.h"
#include "SpecialsManager.h"
#include "COOLManager.h"
#include "SoftkeyManager.h"
#include "SoftkeyFeatureManager.h"
#include "SoftkeyGroupManager.h"
#include "SoftkeyStyleManager.h"

#include "OperatorManager.h"
#include "OperatorChangeManager.h"
#include "OperatorNoteManager.h"
#include "StreamingGraphicManager.h"
#include "GenericLabelTextManager.h"
#include "ConfigurationManager.h"
#include "HourlyTotalManager.h"
#include "OperatorTotalManager.h"
#include "ClassTotalManager.h"
#include "ProductTotalManager.h"
#include "COOLTotalManager.h"
#include "TransactionManager.h"
#include "EmailAlertsManager.h"
#include "ProductDefaultManager.h"
#include "DeviceManager.h"

#include "LabelGraphicManager.h"
#include "CategoryGraphicManager.h"
#include "ProductGraphicManager.h"
#include "SoftkeyGraphicManager.h"
#include "SpecialsGraphicManager.h"
#include "StreamingGraphicManager.h"
#include "RecallManager.h"
#include "Recall.h"
#include "DeviceManager.h"
#include "Device.h"
#include "ProductTextCfgManager.h"
#include "ProductTextCfg.h"
#include "BarcodeScannerManager.h"
#include "HfcScannerProductRead.h"

#include "Product.h"
#include "ProductDefault.h"
#include "Class.h"
#include "Dept.h"
#include "Nutrifacts.h"
#include "ProductText.h"
#include "Category.h"
#include "Special.h"
#include "TransactionType.h"
#include "COOLDevEvent.h"
#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLCategoryCountry.h"
#include "COOLCategoryProdStep.h"
#include "COOLCategoryPretext.h"
#include "SoftkeyDevEvent.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"
#include "Softkey.h"

#include "ExpirationLabel.h"

#include "CoolTotal.h"
#include "COOLTotalEvent.h"

#include "ProductTotal.h"
#include "ClassTotal.h"
#include "OperatorTotal.h"
#include "HourlyTotal.h"

#include "CutTestManager.h"
#include "CutTestTotalManager.h"
#include "CuttingTest.h"
#include "CutTestTotal.h"
 
#include "TransactionDevEvent.h"

#include "Messages/ProcessUserFileMessage.h"

#include "Transaction.h"

#include "HobOperator.h"
#include "OperatorNote.h"
#include "OperatorChange.h"

#include "Graphic.h"
#include "LabelType.h"
#include "GenericLabelText.h"

#include "Configuration.h"
#include "CfgAll.h"
#include "CfgStoreInfo.h"

#include "StreamingGraphic.h"

#include "HobList.h"
#include "GenericHfcObject.h"

#include "PresentationMethodManager.h"
#include "PresentationMethod.h"
#include "ProductStatistic.h"

#include "EvolWrapperTimingsManager.h"
#include "EvolWrapperRequestPackageTimings.h"
#include "HfcEvolWrapperPackageTimingsPointer.h"
#include "HfcEvolWrapperPackageTimings.h"

#include "PrinterEvent.h"
#include "HfcPrinterConfig.h"
#include "HfcPrinterRequestInquire.h"

#include "WeigherEvent.h"
#include "HfcWeigherConfig.h"
#include "WeigherRequestInquire.h"

#include "HobEmailAlert.h"

#include "ScaleManager.h"
#include "Scale.h"
#include "BatchManager.h"
#include "Batch.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChangeManager.h"
#include "SynchronizationChange.h"
#include "SynchronizationManager.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"
#include "ULong.h"

#include "filesys/file.h"
#include "e_string/utilstring.h"

#include "VersionManagerEvent.h"
#include "PythonScripter.h"
#include "thread.h"

#include "ScaleStatus.h"
#include "Statistics.h"

#include "AllDepts.h"

#define DISPLAY_DIALOG true
#define REMOVE_DIALOG false
#define PRINTER_CONFIG_RETRIES	5	//	Number of times to retry, waiting for a Printer Configuration message.
#define WEIGHER_CONFIG_RETRIES	5	//	Number of times to retry, waiting for a Weigher Configuration message.

using namespace hfc;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BackupRestoreManager::dataType = 0;
std::string BackupRestoreManager::dataTypeName("BackupRestoreManager"); 
#endif


BackupRestoreManager::BackupRestoreManager(Session *session, SessionMessage *msg)
		: DBSession(0, "BackupRestoreManager")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init(); 
	mySession = session;
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "(mySession->getCurrentModeOp() == OP_PROCESSINGDOWNLOADEDFILES):  " + booltostring((mySession->getCurrentModeOp() == Session::OP_PROCESSINGDOWNLOADEDFILES)));

	if (mySession->getCurrentModeOp() == Session::OP_PROCESSINGDOWNLOADEDFILES)
	{
		//	Request all of the current printer configurations, in case we need to set them.
		registerEvent(Printer, PrInquire);
		requestPrinterConfig(FirstPrinter);
		requestPrinterConfig(SecondPrinter);
		requestPrinterConfig(ThirdPrinter);
	}

	if (msg)
	{
		BackupRestoreSelections *newSelections = dynamic_cast<BackupRestoreSelections *> (msg->getHfcObject());
		if (newSelections)
		{
			if (!SessionManager::getInstance()->isEvolutionWrapperAttached())
			{
				newSelections->setWrapperSettingsSelected(false);
				Debug::trace("BackupRestoreManager.EvolutionWrapper", "Not attached so set wrapper settings selection to false.");
			}
			if (isTracing)	
			{
				Debug::trace("BackupRestoreManager", HobString("selections: isPluDataSelected = ") + Bool::toString(newSelections->isPluDataSelected()) + 
					" isResourcesSelected = " + Bool::toString(newSelections->isResourcesSelected()) +
					" isOperatorDataSelected = " + Bool::toString(newSelections->isOperatorDataSelected()) +
					" isStoreDataSelected = " + Bool::toString(newSelections->isStoreDataSelected()) +
					" isScaleConfigSelected = " + Bool::toString(newSelections->isScaleConfigSelected()) +
					" isWrapperSettingsSelected = " + Bool::toString(newSelections->isWrapperSettingsSelected()) +
					" isSoftkeyDataSelected = " + Bool::toString(newSelections->isSoftkeyDataSelected()) +
					" isCOOLDataSelected = " + Bool::toString(newSelections->isCOOLDataSelected()) +
					" isSelfServeDataSelected = " + Bool::toString(newSelections->isSelfServeDataSelected()) +
					" isSynchronizeConfigSelected = " + Bool::toString(newSelections->isSynchronizeConfigSelected()) +
					" isEveryDataTypeSelected = " + Bool::toString(newSelections->isEveryDataTypeSelected()) +
					" isScaleStatusSelected = " + Bool::toString(newSelections->isScaleStatusSelected()) +
					" isScaleStatisticsSelected = " + Bool::toString(newSelections->isScaleStatisticsSelected()) +
					" isPrinterSettingsSelected = " + Bool::toString(newSelections->isPrinterSettingsSelected())
					);
			}
			event = msg->getEventType();
			selections = *newSelections;
		}
	}

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "BackupRestoreManager constructor finishing, setting status to active next");
	setStatus(STATUS_ACTIVE);
}

// static
char *BackupRestoreManager::autoGenerateName(Session *session)
{
	char *brName = new char[11];
	if (session)
		sprintf(brName, "tBR%06d", session->getId() % 1000000);
	else
		strcpy(brName, "tBR999999");
	return brName;
}

BackupRestoreManager::~BackupRestoreManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "~BackupRestoreManager()");

	if (subStateList) delete subStateList;

/* Don't delete this here.  The File has ownership of the reader/writer, even if it was created by
 * the Backup/Restore Manager.
	if (frw)		delete frw;
*/
	if (fpIn)        delete fpIn;
	if (fpOut)		 delete fpOut;
	if (hobList)     delete hobList;
	if (hobListFI)   delete hobListFI;
	if (progress)    delete progress;
	if (progMsg)     delete progMsg;
	if (errorsLists) delete errorsLists;
	if (errorsMsg)   delete errorsMsg;

	if (presentationMethodManager) delete presentationMethodManager;
	if (productStatistic)          delete productStatistic;

	if (wrapperTimingsManager) delete wrapperTimingsManager;
}


void BackupRestoreManager::init()
{
	mySession = 0;

	event = UnknownBackRestore;

	subStateList = new std::list<BRSubStates>;
	doneWithSubState = false;

	//The subState will remain idle until state switches from idle
	state = BRIdleState;
	subState = BRIdleSubState;

	mode = BRIdle;
	format = BRNULL;

	startingNewFile = true;

	fileName.erase();
	fpIn = 0;
	frwIn = 0;
	fpOut = 0;	//	Used as the input file when Processing files
	frwOut = 0;	//	For Process files

	fileWriteErrorOccurred = false;

	displayStats = false;
	selections.setDisplayStats(false);

	currentRecordCount    = 0;
	totalRecords          = 0;
	currentRecordsIgnored = 0;
	totalTaskRecords      = 0;
	progressTotal         = 0;
	progressTotalIgnored  = 0;
	totalRecordErrors     = 0;

	hobList = 0;
	hobListFI = 0;

	progress = 0;
	progMsg = 0;

	errorsLists = 0;
	errorsMsg = 0;

	myBackupID_in = Messenger::DEST_UI;
	myBackupID_out = Messenger::DEST_BACKUP;

	writeUserTar = false;
	writeSystemTar = false;

	verifySoftkeyGroupReservedRecords = false;

	pausedThreads = false;

	//restoreRecordMsg = 0;

	dept = 0;
	product = 0;
	classes = 0;
	nutriFact = 0;
	productText = 0;
	labelType = 0;
	graphic = 0;
	category = 0;
	special = 0;

	coolCategory = 0;
	coolCountry = 0;
	coolProductionStep = 0;
	coolPretext = 0;
	coolText = 0;
	coolShortlist = 0;
	coolCategoryCountry = 0;
	coolCategoryProdStep = 0;
	coolCategoryPretext = 0;

	softkeyGroup = 0;
	softkeyStyle = 0;
	softkeyFeature = 0;
	softkey = 0;

	expirationLabel = 0;

	hobOperator = 0;
//	operatorChange_ = 0;

	streamingGraphic = 0;

	genericLabelText = 0;
	genericLabelTextUpdated = false;

	configuration = 0;

//	transactionType_ = 0;
//	productTotal_ = 0;
//	classTotal_ = 0;
//	operatorTotal_ = 0;
//	hourlyTotal_ = 0;
//	transaction_ = 0;
//	coolTotal_ = 0;

	emailAlert = 0;

	scale = 0;
	batch = 0;
	synchChange = 0;

	presentationMethodManager = 0;
	presentationMethod = 0;

	productStatistic = 0;

	previousFileIndicator = BRFIUnknown;

	wrapperTimingsManager = 0;
	wrPackageTimingsPtr = 0;
	pMyWrapperTimings = 0;
	wrapperTimingsReady = false;

	recall = 0;

	device = 0;
	productTextCfg = 0;
	hfcScannerProductRead = 0;

	_printerType_ = 0;
	_printerCfgRetries_ = PRINTER_CONFIG_RETRIES;

	isTracing = Debug::isTracing("BackupRestoreManager");
}


short BackupRestoreManager::getSessionType() const
{
	if (mySession)
		return mySession->getSessionType();
	return DBSession::getSessionType();
}


//DWORD BackupRestoreManager::Run()
void BackupRestoreManager::noMessageFromMessengerProcessing()
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "noMessageFromMessengerProcessing() - begin");

	if (subState == BRCompressingDB)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "Waiting for db compress to finish ...");
        PAL_absclass::PAL_sleep( 100 ); // delay for 1/10th of a second
	}
	else
	{
		try
		{
			//Util::CLockCriticalSection lock ( checkingCS );
			//if (isTracing)	
			//	Debug::trace("BackupRestoreManager", HobString("noMessageFromMessengerProcessing() - state:") + HobString(Integer::toString(state)));
			switch (state)
			{
				case BRBackupState:
					handleBackup();
					break;
				case BRRestoreState:
					handleRestore();
					break;
				case BRProcessFileState:
					handleProcessFile();
					break;
				case BRBadRecordScanState:
					handleBadRecordScan();
					break;
				//case BRIntranetState:
				//	handleIntranet();
				//	break;
				case BRDoneState:
				{
					if (pausedThreads)
					{
						pausedThreads = false;
						SessionManager::getInstance()->resumePausedThreads();
					}

					//tell session that we are done and have session tell the UI that we are done
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "Went to done state, sending message to mySession to terminate me ...");

					Configuration *config = Configuration::getInstance();

					// We were backing up the entire DB in anticipation of an upgrade.
					if ( event == Backup && selections.getFormat() == BRVersionIndependent )
					{
						const string cmd( "htdfBackupSuccess" ); //"htdfBackupFailed"
						const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::PYTHONDBBACKUPRESPONSEPORT), cmd) );
						if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
						{
							Debug::trace("BackupRestoreManager", "before executing python script, command: " + command);
							string connectionStatusOutput;
							PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
							// We don't care about any output from the command.
						}
						else
							Debug::trace("BackupRestoreManager", "python disabled, python script not called, command: " + command);

						// This message would have been displayed if we were backing up the entire DB in anticipation of an upgrade. So remove that message
						if (SessionManager::getInstance()->getPrimaryConsoleSession())
							SessionManager::getInstance()->getPrimaryConsoleSession()->removeDialogMessage(ErrVersionManagerDev, VersionManagerBackupIndependentFormat, HfcResponse::RESPONSESTYLE_NOINPUT);
					}

					if ( event == Restore )
					{
						if ( isBatching() )
						{
							if ( subState == BRCancelSubState )
							{
								bool abortedBatch = false;

								/* Trigger the execution of "Use Case 5.42: Aborting batch", passing the batch identifier. */
								synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
								if ( proxy )
									 abortedBatch = proxy->abortBatch(currentBatchId);

								if ( isTracing )	
								{
									if ( abortedBatch )
										Debug::trace("BackupRestoreManager", "Aborted the batch because the restore/merge operation was cancelled.");
									else
									{
										Debug::trace("BackupRestoreManager", "Failed to abort the batch!");
                                    }
								}
							}
							else
							{
								//6. Trigger the execution of �Use Case 5.28: Processing user file�,
								//   passing batch identifier (the batch will be completed during the execution of this use case).
								Util::CSmartPtr<synchronization::ProcessUserFileMessage> message(new synchronization::ProcessUserFileMessage(currentBatchId));
								dispatchEvent(message.get());
							}

							resetBatchId();
						}
						else
						{
							if ( verifySoftkeyGroupReservedRecords )
							{
								Debug::trace("BackupRestoreManager", "Going to perform SoftkeyGroup::verifyReservedRecords");

								SoftkeyGroup::verifyReservedRecords(this);

								//  Directly above, the call to Softkey::verifyReservedRecords was commented out,
								//  but we now have some default softkeys, for the HTi SelfServe alphabetic search buttons and the best seller button.
								//  So, I'm call the verifyReservedRecords method, but passing in the specific group number that I need.
								Softkey::verifyReservedRecords(this, ReservedSK_SelfservAZCategoryButtons);
							}
						}
					}

					if (isTracing)	
						Debug::trace("BackupRestoreManager", "Before checking to send message to Session displayStats:" + inttostring(displayStats) + "  subState:" + inttostring(subState) );

					if (displayStats && mySession && subState != BRCancelSubState)
					{
						if (isTracing)	
							Debug::trace("BackupRestoreManager", "Sending message to my Session - BackupRestore, Terminate - mySession:" + mySession->getName());

						updateUI(BRGIPlu, BRFIUnknown);  // Note, the group indicator (first parameter) doesn't matter.

						SessionMessage done(BackupRestore, Terminate);
						done.setFormatType(BrowserFormat);
						mySession->send(&done, Messenger::DEST_UI);
					}

					if ( event == Restore && selections.getMode() == BRMerge && selections.getFormat() == BRVersionIndependent )
					{
						if ( configurationManager && configurationManager->hasPendingSynchronizationChange() )
							configurationManager->applyPendingSynchronizationChange(true);

						Debug::trace("BackupRestoreManager", "Just finished merging db independent data. Initializing Synchronization");

						EvolMessage message(SessionDev, SnInitializeSynchronization);
						dispatchEvent(&message);
					}
					else
					{
						if ( configurationManager && configurationManager->hasPendingSynchronizationChange() )
							configurationManager->applyPendingSynchronizationChange();
					}


					//looping = false;
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "In BRDoneState State ... setting status to terminated next");
					setStatus(STATUS_TERMINATED);

					if (isTracing)	
						Debug::trace("BackupRestoreManager", "In BRDoneState State ... just set status to terminated");
					break;
				}
				case BRIdleState:
				default:
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "In Idle State ...");
                    PAL_absclass::PAL_sleep( 1000 );
					break;
			}
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::Run() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "noMessageFromMessengerProcessing() - end");
}

// Overridden from AbstractSession
// virtual
void BackupRestoreManager::sessionMessageProcess(SessionMessage * msg)
{
	//Windux IR#73 -> debug when on crashing application when restoring databases
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "sessionMessageProcess() - begin");

	if (!msg)
	{
		if (isTracing)
			Debug::trace("BackupRestoreManager", "sessionMessageProcess() - no message passed in");
		return;
	}

	if (msg->getDeviceType() == BackupRestore)
	{
		if (msg->getEventType() == BRCancel)
		{
			if (isTracing)
				Debug::trace("BackupRestoreManager", "Received BRCancel Event");

			subState = BRCancelSubState;
			state = BRDoneState;
			noMessageFromMessengerProcessing();
		}
		else if (msg->getEventType() == InitiateReadAll)
		{
			if (isTracing)
				Debug::trace("BackupRestoreManager", "Received InitiateReadAll Event");

			BackupRestoreInitiateReadAllMsg *readAll = dynamic_cast<BackupRestoreInitiateReadAllMsg *> (msg->getHfcObject());
			if (!readAll)
			{
				if (isTracing)
					Debug::trace("BackupRestoreManager", "BackupRestoreInitiateReadAllMsg could not be determined from the InitiateReadAll message!!");
				HobString m = msg->getOriginalBuff();
				if (isTracing)
					Debug::trace("BackupRestoreManager", HobString("buffer sent in for InitiateReadAll is: ") + HobString(Hex::toString(m.data(), (int)m.size())));
				subState = BRCancelSubState;
				state = BRDoneState;
			}
			//getMessenger().setMyUI(readAll->getUiClient());
			else if (readAll->getReadAllDevice() == SessionDev && readAll->getReadAllEvent() == SnDBIndependentBackup)
				initiateVersionIndependentBackup();
			else
			{
				CommMechanism *ui = readAll->getUiClient();
				if (ui)
					ui->setMode(CommMechanism::MODE_OUTPUT);
				myBackupID_out = getMessenger().addCommMechanism(ui);
				initiateReadAll(readAll->getReadAllDevice(), readAll->getReadAllEvent());
			}
		}
		else if (msg->getEventType() == BackupEntireDB)
		{
			// Handle it
			if (isTracing)
				Debug::trace("BackupRestoreManager", "Received BackupEntireDB Event, from input file or via comm");

			if (SessionManager::getInstance()->getPrimaryConsoleSession())
				SessionManager::getInstance()->getPrimaryConsoleSession()->addDialogMessage(ErrVersionManagerDev, VersionManagerBackupIndependentFormat, HfcResponse::RESPONSESTYLE_NOINPUT);
			PAL_absclass::PAL_sleep( 100 );

			initiateVersionIndependentBackup();
		}
	}
	else if (msg->getDeviceType() == SessionDev && msg->getEventType() == SnDBCompressDone)
	{
		// This message will come in when the SessionManager finishes with the compressDB request,
		// which was requested during a Restore in which all Product information is deleted.
		if (isTracing)
			Debug::trace("BackupRestoreManager", "Received SnDBCompressDone Event");
		subState = BRAfterCompressingDB;
		deregisterEvent(SessionDev, SnDBCompressDone);

		//initDB();
	}
	else if (msg->getDeviceType() == Wrapper && msg->getEventType() == WrEvolBackupPackageTimings)
	{
		EvolWrapperPackageTimings *wrapperTimings = 0;
		
		wrPackageTimingsPtr =  dynamic_cast<HfcEvolWrapperPackageTimingsPointer *> (msg->getHfcObject());
		if (wrPackageTimingsPtr)
		{
			wrapperTimings = wrPackageTimingsPtr->getTimingsPointer();
			if(wrapperTimings)
				wrPackageTimingsPtr = new HfcEvolWrapperPackageTimingsPointer(WrEvolBackupPackageTimings, wrapperTimings);

			if (wrPackageTimingsPtr)
				wrapperTimingsReady = true;
		}
	}
	else if ((event == ProcessFile) && ((msg->getEventType() == DtReadAll) ||
										(msg->getDeviceType() == Printer && msg->getEventType() == PrBackupPrinterSettings)))
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "sessionMessageProcess():  Calling initiateReadAll() for PrBackupPrinterSettings");
		initiateReadAll(msg->getDeviceType(), msg->getEventType());
	}
	else if ((event == ProcessFile) && ((msg->getEventType() == DtReadAll) ||
										(msg->getDeviceType() == Weigher && msg->getEventType() == WgBackupWeigherSettings)))
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "sessionMessageProcess():  Calling initiateReadAll() for WgBackupWeigherSettings");
		initiateReadAll(msg->getDeviceType(), msg->getEventType());
	}
	else
	{
		// All messages read from the file, or socket in the case of Scale Communications,
		// will be passed on to the handleMessage() method for processing.

//Windux IR#73 -> debug when on crashing application when restoring databases
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "sessionMessageProcess(): passing message on to handleMessage()");
		handleMessage(msg);
	}

//Windux IR#73 -> debug when on crashing application when restoring databases
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "sessionMessageProcess() - end");
}


int BackupRestoreManager::getEvent() const
{
	return event;
}

const BackupRestoreSelections &BackupRestoreManager::getSelections() const
{
	return selections;
}


void BackupRestoreManager::initiateReadAll(BRSubStates whichData, BRSubStates whichDataNext)
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "initiateReadAll(" + inttostring(whichData) + ", " + inttostring(whichDataNext) + "):  begin");
	if (event != ProcessFile)
		event = Backup;
	if (whichData > 0)  // > BRIdleSubState
		subStateList->push_back(whichData);
	if (whichDataNext > 0)		// > BRIdleSubState
		subStateList->push_back(whichDataNext);
	subStateList->push_back(BRDoneSubState);

	mode = BRMerge;
	format = BREvolution;
	fileName = "";

	state = BRBackupState;
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "initiateReadAll(which):  state = BRBackupState,  subStateList->size() = " + inttostring(subStateList->size()) + ",  END.");
}

void BackupRestoreManager::initiateReadAll(EvolDeviceType device, UINT8 event)
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", HobString("initiateReadAll(dev, ev) - begin - dev:") + HobString(Integer::toString(device)) + HobString("  event:") + HobString(Integer::toString((int)event)));
	switch (device)
	{
		case ProductDev:
			if (event == ProductReadAllExpirationLabels)
				initiateReadAll(BRExpirationLabelSubState);
			else
				initiateReadAll(BRProductSubState);
			break;
		case ProductDefaultsDev:
			initiateReadAll(BRProductDefaultSubState);
			break;
		case ClassDev:
			initiateReadAll(BRClassSubState);
			break;
		case DeptDev:
			initiateReadAll(BRDeptSubState);
			break;

		case NutrifactsDev:
			initiateReadAll(BRNutriDataSubState);
			break;
		case ExpandedTextDev:
			initiateReadAll(BRExpandedTextSubState);
			break;
		case SpecialMessageDev:
			initiateReadAll(BRSpecialMessageSubState);
			break;
		case ProductNotesDev:
			initiateReadAll(BRProductNoteSubState);
			break;
		case MarqueeTextDev:
			initiateReadAll(BRMarqueeTextSubState);
			break;
		case ProductText5Dev:
			initiateReadAll(BRProductText5SubState);
			break;
		case ProductText6Dev:
			initiateReadAll(BRProductText6SubState);
			break;
		case ProductText7Dev:
			initiateReadAll(BRProductText7SubState);
			break;
		case ProductText8Dev:
			initiateReadAll(BRProductText8SubState);
			break;
		case ProductText9Dev:
			initiateReadAll(BRProductText9SubState);
			break;
		case BarcodeScannerDev:
			initiateReadAll(BRBarcodeScannerSubState);
			break;
		case TransactionTypeDev:
			initiateReadAll(BRTransactionTypeSubState);
			break;

		case OperatorDev:
			if (event == DtReadAll)
				initiateReadAll(BROperatorDataSubState);
			else if (event == OpReadNote)
				initiateReadAll(BROperatorNoteSubState);
			break;

		case ConfigurationDev:
			if (event == CfgStoreInfoRead)
				initiateReadAll(BRStoreDataSubState);
			else if (event == DtReadAll)
				initiateReadAll(BRScaleConfigSubState);
			else if (event == CfgSynchronizationRead)
				initiateReadAll(BRSynchronizeConfigSubState);
			break;

		case GraphicDev:
			if (event == DtReadAll)
				initiateReadAll(BRGraphicsSubState);
			else if (event == ReadAllProductGraphics)
				initiateReadAll(BRProductGraphicsSubState);
			else if (event == ReadAllCategoryGraphics)
				initiateReadAll(BRCategoryGraphicsSubState);
			else if (event == ReadAllSpecialsGraphics)
				initiateReadAll(BRSpecialsGraphicsSubState);
			else if (event == ReadAllStreamingGraphics)
				initiateReadAll(BRStreamingGraphicsGraphicsSubState);
			else if (event == ReadAllSoftkeyGraphics)
				initiateReadAll(BRSoftkeyGraphicsSubState);
			break;

		case LabelTypeDev:
			initiateReadAll(BRLabelTypesSubState);
			break;
		case GenericTextDev:
			initiateReadAll(BRLabelTextNamesSubState);
			break;

		case TransactionDev:
			if ( event == TransactionReadAndClear )
				initiateReadAll(BRReadClearTransactionsSubState);
			break;

		case HourlyTotalDev:
			if ( event == DtReadAll )
				initiateReadAll(BRHourlyTotalsSubState);
			break;

		case OperatorTotalDev:
			if ( event == DtReadAll )
				initiateReadAll(BROperatorTotalsSubState);
			break;

		case ProductTotalDev:
			if ( event == DtReadAll )
				initiateReadAll(BRProductTotalsSubState);
			break;

		case ClassTotalDev:
			if ( event == DtReadAll )
				initiateReadAll(BRClassTotalsSubState);
			break;

		case COOLTotalDev:
			if ( event == DtReadAll )
				initiateReadAll(BRCOOLTotalsSubState);
			else if ( event == hfc::ReadAllCOOLTotalsForUltima )
				initiateReadAll(BRCOOLTotalUltimaSubState);
			break;

		case OperatorChangesDev:
			if ( event == DtReadAll )
				initiateReadAll(BROperatorChangesSubState);
			break;

		case CategoryDev:
			initiateReadAll(BRCategoriesSubState);
			break;
		case PresentationMethodDev:
			initiateReadAll(BRPresentationMethodsSubState);
			break;
		case SpecialsDev:
			initiateReadAll(BRSpecialsSubState);
			break;
		case StreamingGraphicDev:
			initiateReadAll(BRStreamingGraphicsSubState);
			break;
		case ProductStatisticsDev:
			initiateReadAll(BRProductStatisticsSubState);
			break;

		case EmailAlertsDev:
			initiateReadAll(BREmailAlertsSubState);
			break;

		case COOLDev:
			if (event == COOLDevReadAllCategories)
				initiateReadAll(BRCOOLCategorySubState);
			else if (event == COOLDevReadAllCountries)
				initiateReadAll(BRCOOLCountrySubState);
			else if (event == COOLDevReadAllProductionSteps)
				initiateReadAll(BRCOOLProductionStepSubState);
			else if (event == COOLDevReadAllPretexts)
				initiateReadAll(BRCOOLPretextSubState);
			else if (event == COOLDevReadAllCoolText)
				initiateReadAll(BRCOOLTextSubState);
			else if (event == COOLDevReadAllShortlists)
				initiateReadAll(BRCOOLShortlistSubState);
			else if (event == COOLDevReadAllCategoryCountries)
				initiateReadAll(BRCOOLCategoryCountrySubState);
			else if (event == COOLDevReadAllCategoryProductionSteps)
				initiateReadAll(BRCOOLCategoryProductionStepSubState);
			else if (event == COOLDevReadAllCategoryPretexts)
				initiateReadAll(BRCOOLCategoryPretextSubState);
			else if (event == COOLDevReadAllCoolTextForUltima)
				initiateReadAll(BRCOOLTextUltimaSubState, BRCOOLPretextUltimaSubState);
			else if (event == COOLDevReadAllShortlistsForUltima)
				initiateReadAll(BRCOOLShortlistUltimaSubState);
			else if (event == COOLDevReadAllCategoryCountriesForUltima)
				initiateReadAll(BRCOOLCategoryCountryUltimaSubState);
			else if (event == COOLDevReadAllCategoryProdStepsForUltima)
				initiateReadAll(BRCOOLCategoryProductionStepUltimaSubState);
			break;

		case SoftkeyDev:
			if (event == ReadAllSoftkeyGroups)
				initiateReadAll(BRSoftkeyGroupSubState);
			else if (event == ReadAllSoftkeyStyles)
				initiateReadAll(BRSoftkeyStyleSubState);
			else if (event == ReadAllSoftkeyFeatures)
				initiateReadAll(BRSoftkeyFeatureSubState);
			else if (event == ReadAllSoftkeys)
				initiateReadAll(BRSoftkeySubState);
			break;

		case CutTestTotalDev:
			if (event == DtReadAll)
				initiateReadAll(BRCutTestTotalSubState);
			break;

		case CutTestDev:
			if (event == DtReadAll)
				initiateReadAll(BRCuttingTestSubState);
			break;

		case Printer:
			Debug::trace("BackupRestoreManager.PrinterSettings", "initiateReadAll(Printer, " + inttostring(event) + ")");
			if (event == PrBackupPrinterSettings)
				initiateReadAll(BRPrinterSettingsSubState);
			break;

		case Weigher:
			Debug::trace("BackupRestoreManager.WeigherSettings", "initiateReadAll(Weigher, " + inttostring(event) + ")");
			if (event == WgBackupWeigherSettings)
				initiateReadAll(BRWeigherSettingsSubState);
			break;

		default:
			// If we can't determine what they are trying to read-all,
			// we have to call initiateRealAll with something so that the BRDoneSubState
			// will get pushed onto the list, so the BackupRestoreManager the Session
			// created will get deleted appropriately.
			initiateReadAll(BRIdleSubState);
			break;
	}
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "initiateReadAll(dev, ev) - end");
}


void BackupRestoreManager::initiateVersionIndependentBackup()
{
	Debug::trace("BackupRestoreManager", "initiateVersionIndependentBackup - begin, before requestToPause");
	//SessionManager::getInstance()->requestToPause(sessionThread_->GetThreadID() /*util_thread::Thread<>::getCurrentThreadID()*/, true, false);
	//pausedThreads = true;
	Debug::trace("BackupRestoreManager", "initiateVersionIndependentBackup - after requestToPause");

	event = Backup;

	subStateList->push_back(BRScaleConfigSubState);
	subStateList->push_back(BREmailAlertsSubState);
	subStateList->push_back(BRProductDefaultSubState);
	subStateList->push_back(BRDevicesSubState);
	subStateList->push_back(BRProductTextCfgSubState);
	subStateList->push_back(BRBarcodeScannerSubState);

	subStateList->push_back(BRDeptSubState);
	subStateList->push_back(BRClassSubState);
	subStateList->push_back(BRNutriDataSubState);
	subStateList->push_back(BRExpandedTextSubState);
	subStateList->push_back(BRSpecialMessageSubState);
	subStateList->push_back(BRProductNoteSubState);
	subStateList->push_back(BRMarqueeTextSubState);
	subStateList->push_back(BRProductText5SubState);
	subStateList->push_back(BRProductText6SubState);
	subStateList->push_back(BRProductText7SubState);
	subStateList->push_back(BRProductText8SubState);
	subStateList->push_back(BRProductText9SubState);
	subStateList->push_back(BRTransactionTypeSubState);

	subStateList->push_back(BRCategoriesSubState);
	subStateList->push_back(BRPresentationMethodsSubState);
	subStateList->push_back(BRSpecialsSubState);
	subStateList->push_back(BRStreamingGraphicsSubState);

	subStateList->push_back(BRCOOLCategorySubState);
	subStateList->push_back(BRCOOLCountrySubState);
	subStateList->push_back(BRCOOLProductionStepSubState);
	subStateList->push_back(BRCOOLPretextSubState);
	subStateList->push_back(BRCOOLTextSubState);
	subStateList->push_back(BRCOOLShortlistSubState);
	subStateList->push_back(BRCOOLCategoryCountrySubState);
	subStateList->push_back(BRCOOLCategoryProductionStepSubState);
	subStateList->push_back(BRCOOLCategoryPretextSubState);

	subStateList->push_back(BRProductSubState);

	subStateList->push_back(BRExpirationLabelSubState);

	subStateList->push_back(BRProductStatisticsSubState);

	subStateList->push_back(BRLabelTypesSubState);
	subStateList->push_back(BRLabelTextNamesSubState);

	subStateList->push_back(BROperatorNoteSubState);
	subStateList->push_back(BROperatorDataSubState);

	subStateList->push_back(BRTransactionsSubState);

	// When restoring this data, the operators, products, categories,
	// and softkey features should already exist.
	// so backup the softkey data after those listed here,
	// (with the softkeys after the softkey features), and
	// (with the softkey features after the softkey styles).
	subStateList->push_back(BRSoftkeyGroupSubState);
	subStateList->push_back(BRSoftkeyStyleSubState);
	subStateList->push_back(BRSoftkeyFeatureSubState);
	subStateList->push_back(BRSoftkeySubState);

	subStateList->push_back(BRScalesSubState);
	subStateList->push_back(BRBatchesSubState);
	subStateList->push_back(BRSynchronizationChangesSubState);

	subStateList->push_back(BRSynchronizeConfigSubState);

	subStateList->push_back(BRRecallSubState);

	subStateList->push_back(BRCuttingTestSubState);

	subStateList->push_back(BRDoneSubState);

	// Put this info in the selections object, since handleBackup will be called and
	// it will pull these values from the selections object.
	selections.setMode(BRMerge);
	selections.setFormat(BRVersionIndependent);
	selections.setFileName(Configuration::getInstance()->getVersionIndependentBackupFileName());
	displayStats = false;

	state = BRBackupState;
	subState = BRIdleSubState;

    getMessenger().setPostponingInput(true);

	// Give a chance to close down the synchronization and other sessions
    PAL_absclass::PAL_sleep( 1000 );
}


void BackupRestoreManager::initialBackup()
{
	//What UI check boxes are selected?	what does the user want to backup
	//Also, figure out how many total records for the task

	updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreCountingRecords, HfcResponse::RESPONSESTYLE_NOINPUT);

	totalTaskRecords = 0;

	if (selections.getFormat() == BRUltima)
		subStateList->push_back(BRUltimaHeaderSubState);

	if (selections.isStoreDataSelected())
	{
		subStateList->push_back(BRStoreDataSubState);
  		totalTaskRecords += 1;
	}

	if (selections.isScaleConfigSelected())
	{
		subStateList->push_back(BRScaleConfigSubState);

		configuration = Configuration::getInstance();
		totalTaskRecords += 1;

		if (selections.getFormat() != BRUltima)
		{
			subStateList->push_back(BREmailAlertsSubState);

			if (emailAlertsManager)
  				totalTaskRecords += emailAlertsManager->countAllEmailAlerts();
	
			// There are currently 4, and only 4, Product Default records.  So start the count there.
			totalTaskRecords += 4;
			subStateList->push_back(BRProductDefaultSubState);

			subStateList->push_back(BRProductTextCfgSubState);
			if (productTextCfgManager)
  				totalTaskRecords += productTextCfgManager->countAllProductTextCfgs();
		
		}
	}

	if (selections.isPluDataSelected())
	{
		if (selections.getFormat() != BRUltima)
		{
			if (deptManager)
				totalTaskRecords += deptManager->countAllDepts();
		}

		if (classManager)
  			totalTaskRecords += classManager->countAllClasses();

		if (nutrifactsManager)
  			totalTaskRecords += nutrifactsManager->countAllNutrifacts();

		if (expandedTextManager)
  			totalTaskRecords += expandedTextManager->countAllProductTexts();

		if (specialMessageManager)
  			totalTaskRecords += specialMessageManager->countAllProductTexts();

		if (selections.getFormat() != BRUltima)
		{
			if (productNoteManager)
	  			totalTaskRecords += productNoteManager->countAllProductTexts();
		}

		if (marqueeTextManager)
  			totalTaskRecords += marqueeTextManager->countAllProductTexts();

		if (selections.getFormat() != BRUltima)
		{
			if (productText5Manager)
  				totalTaskRecords += productText5Manager->countAllProductTexts();
			if (productText6Manager)
  				totalTaskRecords += productText6Manager->countAllProductTexts();
			if (productText7Manager)
  				totalTaskRecords += productText7Manager->countAllProductTexts();
			if (productText8Manager)
  				totalTaskRecords += productText8Manager->countAllProductTexts();
			if (productText9Manager)
  				totalTaskRecords += productText9Manager->countAllProductTexts();
		}

		if (selections.getFormat() != BRUltima)
		{
		    if (barcodeScannerManager)
  		        totalTaskRecords += barcodeScannerManager->countAllBarcodeScanners();
		}

		if (transactionTypeManager)
  			totalTaskRecords += transactionTypeManager->countAllRecords();

		if (productManager)
  			totalTaskRecords += productManager->countAllProducts();

		if (categoryManager)
			totalTaskRecords += categoryManager->countAllCategories();

		if (specialsManager)
			totalTaskRecords += specialsManager->countAllSpecials();
	}

	if (selections.isSelfServeDataSelected() && selections.getFormat() != BRUltima)
	{
		presentationMethodManager = new PresentationMethodManager();
		if (presentationMethodManager)
		{
			presentationMethodManager->setDBAccess(this);
			totalTaskRecords += presentationMethodManager->countAllPresentationMethods();
			delete presentationMethodManager;
			presentationMethodManager = 0;
		}

		if (streamingGraphicManager)
			totalTaskRecords += streamingGraphicManager->countAllStreamingGraphics();

		if (productManager)
			totalTaskRecords += productManager->countAllProductStatistics();
	}

	if (selections.isCOOLDataSelected() && selections.getFormat() != BRUltima)
	{
		Debug::trace("BackupRestoreManager.cool", "COOL Data selected for backup, counting COOL records ");

		if (coolManager)
		{
			Debug::trace("BackupRestoreManager.cool", HobString("before counting COOL records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLCategories();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL Category records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLCountries();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL Country records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLProductionSteps();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL ProductionStep records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLPretexts();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL Pretext records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLTexts();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL Text records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLShortlists();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL Shortlist records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLCategoryCountries();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL CategoryCountry records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLCategoryProductionSteps();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL CategoryProductionStep records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += coolManager->countAllCOOLCategoryPretexts();
			Debug::trace("BackupRestoreManager.cool", HobString("after counting COOL CategoryPretext records ") + HobString(Integer::toString(totalTaskRecords)));
		}
	}


	if (selections.isPluDataSelected())
	{
		if (selections.getFormat() != BRUltima)
			subStateList->push_back(BRDeptSubState);
		subStateList->push_back(BRClassSubState);
		subStateList->push_back(BRNutriDataSubState);
		subStateList->push_back(BRExpandedTextSubState);
		subStateList->push_back(BRSpecialMessageSubState);
		subStateList->push_back(BRMarqueeTextSubState);
		if (selections.getFormat() != BRUltima)
		{
			subStateList->push_back(BRProductNoteSubState);
			subStateList->push_back(BRProductText5SubState);
			subStateList->push_back(BRProductText6SubState);
			subStateList->push_back(BRProductText7SubState);
			subStateList->push_back(BRProductText8SubState);
			subStateList->push_back(BRProductText9SubState);
            subStateList->push_back(BRBarcodeScannerSubState);
		}
		subStateList->push_back(BRTransactionTypeSubState);
		subStateList->push_back(BRCategoriesSubState);
		subStateList->push_back(BRSpecialsSubState);
	}

	if (selections.isSelfServeDataSelected() && selections.getFormat() != BRUltima)
	{
		subStateList->push_back(BRPresentationMethodsSubState);
		subStateList->push_back(BRStreamingGraphicsSubState);
	}

	if (selections.isCOOLDataSelected() && selections.getFormat() != BRUltima)
	{
		Debug::trace("BackupRestoreManager.cool", "COOL Data selected for backup, adding substates to backup COOL records ");
		subStateList->push_back(BRCOOLCategorySubState);
		subStateList->push_back(BRCOOLCountrySubState);
		subStateList->push_back(BRCOOLProductionStepSubState);
		subStateList->push_back(BRCOOLPretextSubState);
		subStateList->push_back(BRCOOLTextSubState);
		subStateList->push_back(BRCOOLShortlistSubState);
		subStateList->push_back(BRCOOLCategoryCountrySubState);
		subStateList->push_back(BRCOOLCategoryProductionStepSubState);
		subStateList->push_back(BRCOOLCategoryPretextSubState);
	}

	if (selections.isPluDataSelected())
	{
		subStateList->push_back(BRProductSubState);
		subStateList->push_back(BRCategoryGraphicsSubState);
		subStateList->push_back(BRProductGraphicsSubState);
		subStateList->push_back(BRSpecialsGraphicsSubState);

		if (categoryGraphicManager)
		{
			categoryGraphicManager->setCurrentSubType(Graphic::SUBTYPE_USER);
  			totalTaskRecords += categoryGraphicManager->countAllGraphics();
			Debug::trace("BackupRestoreManager.Graphics", HobString("TYPE_CATEGORYGRAPHIC, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
		}
		if (productGraphicManager)
		{
			productGraphicManager->setCurrentSubType(Graphic::SUBTYPE_USER);
  			totalTaskRecords += productGraphicManager->countAllGraphics();
			Debug::trace("BackupRestoreManager.Graphics", HobString("TYPE_PRODUCTGRAPHIC, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
		}
		if (specialsGraphicManager)
		{
  			totalTaskRecords += specialsGraphicManager->countAllGraphics();
			Debug::trace("BackupRestoreManager.Graphics", HobString("TYPE_SPECIALSGRAPHIC, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
		}
	}

	if (selections.isSelfServeDataSelected() && selections.getFormat() != BRUltima)
	{
		subStateList->push_back(BRProductStatisticsSubState);
		subStateList->push_back(BRStreamingGraphicsGraphicsSubState);

		if (streamingGraphicManager)
		{
  			totalTaskRecords += streamingGraphicManager->countAllStreamingGraphics();
			Debug::trace("BackupRestoreManager.Graphics", HobString("TYPE_STREAMINGGRAPHIC, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
		}
	}


	if (selections.isResourcesSelected())
	{
		if (selections.getFormat() == BREvolution && Configuration::getInstance()->toBoolCfgI(CfgInternal::BACKUPROMLABELS))
		{
			subStateList->push_back(BRLabelTypesSubState);
			if (labelTypeManager)
				totalTaskRecords += labelTypeManager->countAllLabelTypes();
		}
		else
		{
			if (selections.getFormat() == BREvolution)
			{
				// Resources consist of ...
				subStateList->push_back(BRGraphicsSubState);
				subStateList->push_back(BRLabelTypesSubState);

				if (labelGraphicManager)
				{
					totalTaskRecords += labelGraphicManager->countAllCustomGraphics();
					Debug::trace("BackupRestoreManager.Graphics", HobString("custom graphics, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
				}

				if (labelTypeManager)
					totalTaskRecords += labelTypeManager->countAllCustomLabelTypes();
			}

			subStateList->push_back(BRLabelTextNamesSubState);
			if (genericLabelTextManager)
				totalTaskRecords += genericLabelTextManager->countAllGenericLabelTexts();
		}
	}


	if (selections.isOperatorDataSelected())
	{
		subStateList->push_back(BROperatorDataSubState);
		subStateList->push_back(BROperatorNoteSubState);

		if (operatorManager)
			totalTaskRecords += operatorManager->countAllOperators() + 1;  // The +1 is for the operator note record.  Currently, we only support one of them.
	}


	if (selections.isWrapperSettingsSelected())
	{			
		requestWrapperTimings();
		
		//wait for the timingsManager to get timings
		subStateList->push_back(BRWrapperSettingsRequestSubState);
		subStateList->push_back(BRWrapperSettingsSubState);
	}


	if (selections.isSoftkeyDataSelected() && selections.getFormat() != BRUltima)
	{
		subStateList->push_back(BRSoftkeyGroupSubState);
		subStateList->push_back(BRSoftkeyStyleSubState);
		subStateList->push_back(BRSoftkeyFeatureSubState);
		subStateList->push_back(BRSoftkeySubState);
		subStateList->push_back(BRSoftkeyGraphicsSubState);

		if (softkeyGraphicManager)
		{
	  		totalTaskRecords += softkeyGraphicManager->countAllGraphics();
			Debug::trace("BackupRestoreManager.Graphics", HobString("TYPE_SOFTKEYGRAPHIC, totalTaskRecords = ") + HobString(Integer::toString(totalTaskRecords)));
		}

		Debug::trace("BackupRestoreManager.cool", "Softkey Data selected for backup, counting records ");

		if (softkeyManager)
		{
			if (isTracing)
				Debug::trace("BackupRestoreManager", HobString("before counting softkey group records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += softkeyGroupManager->countAllSoftkeyGroups();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("before counting softkey style records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += softkeyStyleManager->countAllSoftkeyStyles();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("after counting softkey style records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += softkeyFeatureManager->countAllSoftkeyFeatures();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("after counting softkey feature records ") + HobString(Integer::toString(totalTaskRecords)));
			totalTaskRecords += softkeyManager->countAllSoftkeys();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("after counting softkey records ") + HobString(Integer::toString(totalTaskRecords)));
		}
	}

	if (selections.isSynchronizeConfigSelected() && selections.getFormat() != BRUltima)
	{
		subStateList->push_back(BRSynchronizeConfigSubState);
  		totalTaskRecords += 1;
	}
	
	// Added for Remote Scale Maintenance feature. Only applies to Read Scale status  and Read Scale Statistice commands dropped into the scales "inbox"
	// Also only applies to EvolutionTaggedFormat.
	if (selections.isScaleStatusSelected() && selections.getFormat() !=BRUltima)
	{
		subStateList->push_back(BRScaleStatusSubState);
//		totalTaskRecords +=1;
	}

	if (selections.isScaleStatisticsSelected() && selections.getFormat() !=BRUltima)
	{
		subStateList->push_back(BRScaleStatisticsSubState);
//		totalTaskRecords +=1;
	}

	if (selections.isPrinterSettingsSelected())
	{			
		Debug::trace("BackupRestoreManager.PrinterSettings", "initialBackup():   push_back(BRPrinterSettingsSubState)");
		subStateList->push_back(BRPrinterSettingsSubState);
	}

	if (selections.getFormat() == BRUltima)
		subStateList->push_back(BRUltimaFooterSubState);

	//This will tell us that we are done processing everything the user wanted
	subStateList->push_back(BRDoneSubState);

	updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreCountingRecords, HfcResponse::RESPONSESTYLE_NOINPUT);
}


void BackupRestoreManager::handleBackup()
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleBackup():  begin.  subState = " + inttostring(subState) + ",  (subState == BRIdleSubState) = " + booltostring(subState == BRIdleSubState) + ",  (event == ProcessFile) = "  + booltostring(event == ProcessFile));

	bool openOutputFile = false;
	if ((event == ProcessFile) && !fpOut)
		openOutputFile = true;

	if ((subState == BRIdleSubState) || openOutputFile)
	{
		mode = selections.getMode();				//The mode will tell us how to handle the backup or restore
		format = selections.getFormat();			//What format should the backup be Evolution or Ultima
		if (event == ProcessFile)
			fileName = selections.getAltFileName();	//Get the name of the backup file to create
		else
			fileName = selections.getFileName();	//Get the name of the backup file to create

		subState = subStateList->front();		//get subState off of list
		subStateList->remove(subState);
		//open file for restore

		if (isTracing)	
			Debug::trace("BackupRestoreManager", "handleBackup():  1) Remove:  subState = " + inttostring(subState) + ",   subStateList->size() = " + inttostring(subStateList->size()));

		if (!fpOut && fileName.size())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleBackup():  Create a new CMFile");

			fpOut = new CMFile(fileName.c_str(), CommMechanism::MODE_OUTPUT /*ios::in|ios::out|ios::trunc*/);
			if (fpOut)
			{
			 	if (!frwOut)
				{
					switch ( format )
					{
						case BRUltima:
							frwOut = new CMReaderWriterUltimaDisk();
							break;
						case BRVersionIndependent:
							frwOut = new CMReaderWriterDBIndependent();
							break;
						default:
							frwOut = new CMReaderWriterEvolTag();
							break;
					}
				}
				fpOut->setMyReaderWriter(frwOut);
				
				//keep track of ID!!!
				myBackupID_out = getMessenger().addCommMechanism(fpOut);
			}
			else
			{
				subState = BRDoneSubState;
				state = BRDoneState;
				return;
			}
		}
	}

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleBackup():  doneWithSubState = " + booltostring(doneWithSubState));
	if (doneWithSubState)
	{
		subState = subStateList->front();		//get subState off of list
		subStateList->remove(subState);

		if (isTracing)	
			Debug::trace("BackupRestoreManager", "handleBackup():  2) Remove:  subState = " + inttostring(subState) + ",   subStateList->size() = " + inttostring(subStateList->size()));

		if (subState == BRDoneSubState)
		{
			//send updateUI and done message "might have to save the last subState"
			//set main state to done
			state = BRDoneState;
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleBackup():  state = BRDoneState");
		}
		else
		{
			doneWithSubState = false;
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleBackup():  doneWithSubState = false");
		}
	}
	else
	{
		processBackupSubState();
	}

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleBackup():  DONE");
}

void BackupRestoreManager::processBackupSubState()
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "processBackupSubState() - begin");

	switch (subState)
	{
		case BRProductSubState:
			backupProducts();
			break;
		case BRProductDefaultSubState:
			backupProductDefaults();
			break;
		case BRClassSubState:
			backupClasses();
			break;
		case BRNutriDataSubState:
			backupNutriData();
			break;

		case BRExpandedTextSubState:
		case BRSpecialMessageSubState:
		case BRProductNoteSubState:
		case BRMarqueeTextSubState:
		case BRProductText5SubState:
		case BRProductText6SubState:
		case BRProductText7SubState:
		case BRProductText8SubState:
		case BRProductText9SubState:
			backupProductTexts(subState);
			break;

		case BRTransactionTypeSubState:
			backupTransactionTypes();
			break;

		case BRGraphicsSubState:
			backupLabelGraphics();
			break;
		case BRLabelTypesSubState:
			backupLabelTypes();
			break;
		case BRLabelTextNamesSubState:
			backupLabelTextNames();
			break;

		case BRStoreDataSubState:
			backupStoreData();
			break;
		case BRScaleConfigSubState:
			backupScaleConfig();
			break;

		case BROperatorDataSubState:
			backupOperatorData();
			break;
		case BROperatorNoteSubState:
			backupOperatorNote();
			break;
		case BROperatorChangesSubState:
			backupOperatorChanges();
			break;

		case BRWrapperSettingsSubState:
			backupWrapperSettings();
			break;
		case BRWrapperSettingsRequestSubState:
			backupWrapperRequestSettings();
			break;

		case BRProductTotalsSubState:
			backupProductTotals();
			break;
		case BRClassTotalsSubState:
			backupClassTotals();
			break;
		case BROperatorTotalsSubState:
			backupOperatorTotals();
			break;
		case BRHourlyTotalsSubState:
			backupHourlyTotals();
			break;
		case BRCOOLTotalsSubState:
			backupCOOLTotals(false);
			break;
		case BRCOOLTotalUltimaSubState:
			backupCOOLTotals(true);
			break;

		case BRCutTestTotalSubState:
			backupCutTestTotals();
			break;
		case BRCuttingTestSubState:
			backupCuttingTests();
			break;

		case BRTransactionsSubState:
			backupTransactions();
			break;
		case BRReadClearTransactionsSubState:
			backupNotClearedTransactions();
			break;

		case BRCategoriesSubState:
			backupCategories();
			break;
		case BRPresentationMethodsSubState:
			backupPresentationMethods();
			break;
		case BRSpecialsSubState:
			backupSpecials();
			break;
		case BRStreamingGraphicsSubState:
			backupStreamingGraphics();
			break;
		case BRProductStatisticsSubState:
			backupProductStatistics();
			break;

		case BRCategoryGraphicsSubState:
			backupCategoryGraphics();
			break;
		case BRProductGraphicsSubState:
			backupProductGraphics();
			break;
		case BRSpecialsGraphicsSubState:
			backupSpecialsGraphics();
			break;
		case BRStreamingGraphicsGraphicsSubState:
			backupStreamingGraphicsGraphics();
			break;

		case BRSoftkeyGraphicsSubState:
			backupSoftkeyGraphics();
			break;

		case BREmailAlertsSubState:
			backupEmailAlerts();
			break;

		case BRCOOLCategorySubState:
			backupCOOLCategories();
			break;
		case BRCOOLCountrySubState:
			backupCOOLCountries();
			break;
		case BRCOOLProductionStepSubState:
			backupCOOLProductionSteps();
			break;
		case BRCOOLPretextSubState:
			backupCOOLPretexts();
			break;
		case BRCOOLPretextUltimaSubState:
			backupCOOLPretexts(true);
			break;
		case BRCOOLTextSubState:
			backupCOOLTexts();
			break;
		case BRCOOLTextUltimaSubState:
			backupCOOLTexts(true);
			break;
		case BRCOOLShortlistSubState:
			backupCOOLShortlists();
			break;
		case BRCOOLShortlistUltimaSubState:
			backupCOOLShortlists(true);
			break;

		case BRCOOLCategoryCountrySubState:
			backupCOOLCategoryCountries();
			break;
		case BRCOOLCategoryCountryUltimaSubState:
			backupCOOLCategoryCountriesForUltima();
			break;
		case BRCOOLCategoryProductionStepSubState:
			backupCOOLCategoryProductionSteps();
			break;
		case BRCOOLCategoryProductionStepUltimaSubState:
			backupCOOLCategoryProductionStepsForUltima();
			break;
		case BRCOOLCategoryPretextSubState:
			backupCOOLCategoryPretexts();
			break;

		case BRSoftkeyGroupSubState:
			backupSoftkeyGroups();
			break;
		case BRSoftkeyStyleSubState:
			backupSoftkeyStyles();
			break;
		case BRSoftkeyFeatureSubState:
			backupSoftkeyFeatures();
			break;
		case BRSoftkeySubState:
			backupSoftkeys();
			break;

		case BRExpirationLabelSubState:
			backupExpirationLabels();
			break;

		case BRScalesSubState:
			backupScales();
			break;
		case BRBatchesSubState:
			backupBatches();
			break;
		case BRSynchronizationChangesSubState:
			backupSynchronizationChanges();
			break;

		case BRSynchronizeConfigSubState:
			 backupSynchronizationConfig();
			 break;

		case BRRecallSubState:
			backupRecalls();
			break;

		case BRDevicesSubState:
			backupDevices();
			break;

		case BRProductTextCfgSubState:
			backupProductTextCfgs();
			break;

		// Added for the Remote Scale Maintenance feature
		case BRScaleStatusSubState:
			backupScaleStatus();
			break;
		case BRScaleStatisticsSubState:
			backupScaleStatistics();
			break;
		case BRPrinterSettingsSubState:
			Debug::trace("BackupRestoreManager.PrinterSettings", "processBackupSubState():  BRPrinterSettingsSubState. Calling backupPrinterSettings()");
			backupPrinterSettings();
			break;
		case BRWeigherSettingsSubState:
			Debug::trace("BackupRestoreManager.WeigherSettings", "processBackupSubState():  BRWeigherSettingsSubState. Calling backupWeigherSettings()");
			backupWeigherSettings();
			break;

		case BRBarcodeScannerSubState:
			backupBarcodeScanners();
			break;

		case BRDeptSubState:
			backupDepts();
			break;

		case BRUltimaHeaderSubState:
			backupUltimaHeader();
			break;
		case BRUltimaFooterSubState:
			backupUltimaFooter();
			break;

		default:
			break;
	}
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "processBackupSubState() - done");
}


void BackupRestoreManager::backupARecord(SessionMessage *msg)
{
	int bytesSent = send(msg, myBackupID_out);
	if (bytesSent < 0)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupARecord - bytesSent < 0") );
		subState = BRCancelSubState;
		state = BRDoneState;

		fileWriteErrorOccurred = true;

		if (mySession)
		{
			// Have the Session send a message to the browser to cancel this backup/restore
			EvolMessage cancel(BackupRestore, BRCancel, this);
			dispatchEvent(&cancel, mySession);
		}

		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreFileWriteError, HfcResponse::RESPONSESTYLE_OK);
	}
}


void BackupRestoreManager::backupProducts()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (product)
		{
			delete product;
			product = 0;
		}
		if (productManager)
		{
			totalRecords = productManager->countAllProducts();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupProducts() - number of records to backup is: ") + HobString(Long::toString(totalRecords)));
			product = productManager->firstProduct();
		}
	}

	if (product)
	{
		if (product->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupProducts() - backup product num: ") + HobString(Long::toString(product->getProductNum())));

			SessionMessage msg(ProductDev, ProductWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

//				product->setFormatType(DBIndependentFormatFormat);
			msg.setHfcObject(product);
			backupARecord(&msg);
		}
		else
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupProducts() - Record is not valid: ") + HobString(Long::toString(product->getProductNum())));
			totalRecordErrors++;
		}
		delete product;
		product = productManager->nextProduct();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupProducts() - product is NULL: "));
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIProducts);

}

void BackupRestoreManager::backupProductDefaults()
{
	// There are only 4 ProductDefault records in the database.  I'm going to back them all up at once;
	// as opposed to iterating through each one in the database, like all other data types do via their Managers.

	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		totalRecords = 4;
	}

	AllDepts allDepts;
	allDepts.findAll(this);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		backupProductDefault(Product::PRODUCTTYPE_RANDOMWEIGHT, deptNum);
		backupProductDefault(Product::PRODUCTTYPE_FIXEDWEIGHT, deptNum);
		backupProductDefault(Product::PRODUCTTYPE_BYCOUNT, deptNum);
		backupProductDefault(Product::PRODUCTTYPE_FLUIDOUNCE, deptNum);

		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically backed up the ProductDefault records for dept 0 in the first pass through the while loop (since deptNumStr was initialized to "0").
		// If the database doesn't contain the default dept record, we still need to backup the ProductDefault records for the default dept (Dept 0).
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}

	startingNewFile = true;
	doneWithSubState = true;
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIProductDefaults);
}

void BackupRestoreManager::backupProductDefault(short productType, HobRecordNum deptNum)
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", HobString("Backup a product default - type:") + HobString(Integer::toString(productType)));
	ProductDefault *productDefault = ProductDefault::findRecord(productType, deptNum, this);

	if (productDefault)
	{
		if (productDefault->isRecordValid())
		{
			SessionMessage msg(ProductDefaultsDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			if (format == BRVersionIndependent)
				msg.setFormatType(DBIndependentFormatFormat);
			else
				msg.setFormatType(EvolutionTaggedFormat);

				//productDefault->setFormatType(DBIndependentFormatFormat);
			msg.setHfcObject(productDefault);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete productDefault;
	}

	currentRecordCount++;
	progressTotal++;
	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIProductDefaults);
}

void BackupRestoreManager::backupClasses()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (classes)
		{
			delete classes;
			classes = 0;
		}
		if (classManager)
		{
			totalRecords = classManager->countAllClasses();
			classes = classManager->firstClass();
		}
	}

	if (classes)
	{
		if (classes->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(ClassDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(classes);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete classes;
		classes = classManager->nextClass();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIClasses);

}

void BackupRestoreManager::backupNutriData()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (nutriFact)
		{
			delete nutriFact;
			nutriFact = 0;
		}
		if (nutrifactsManager)
		{
			totalRecords = nutrifactsManager->countAllNutrifacts();
			nutriFact = nutrifactsManager->firstNutrifacts();
		}
	}

	if (nutriFact)
	{
		if (nutriFact->isRecordValid())
		{
			SessionMessage msg(NutrifactsDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(nutriFact);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete nutriFact;
		nutriFact = nutrifactsManager->nextNutrifacts();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFINutriFacts);
}


void BackupRestoreManager::backupProductTexts(BRSubStates)
{
	switch (subState)
	{
		case BRExpandedTextSubState:
			backupOneProductText(expandedTextManager, ExpandedTextDev, BRFIExpandedText);
			break;
		case BRSpecialMessageSubState:
			backupOneProductText(specialMessageManager, SpecialMessageDev, BRFISpecialMessages);
			break;
		case BRProductNoteSubState:
			backupOneProductText(productNoteManager, ProductNotesDev, BRFIProductNotes);
			break;
		case BRMarqueeTextSubState:
			backupOneProductText(marqueeTextManager, MarqueeTextDev, BRFIMarqueeText);
			break;
		case BRProductText5SubState:
			backupOneProductText(productText5Manager, ProductText5Dev, BRFIProductText5);
			break;
		case BRProductText6SubState:
			backupOneProductText(productText6Manager, ProductText6Dev, BRFIProductText6);
			break;
		case BRProductText7SubState:
			backupOneProductText(productText7Manager, ProductText7Dev, BRFIProductText7);
			break;
		case BRProductText8SubState:
			backupOneProductText(productText8Manager, ProductText8Dev, BRFIProductText8);
			break;
		case BRProductText9SubState:
			backupOneProductText(productText9Manager, ProductText9Dev, BRFIProductText9);
			break;
		default:
			break;
	}
}

void BackupRestoreManager::backupOneProductText(ProductTextManager *prodTextMgr, EvolDeviceType dev, BRFileIndicator fileInd)
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (productText)
		{
			delete productText;
			productText = 0;
		}
		if (prodTextMgr)
		{
			totalRecords = prodTextMgr->countAllProductTexts();
			productText = prodTextMgr->getFirstRecord();
		}
	}

	if (productText)
	{
		if (productText->isRecordValid())
		{
			SessionMessage msg(dev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(productText);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete productText;
		productText = prodTextMgr->getNextRecord();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, fileInd);
}

void BackupRestoreManager::backupTransactionTypes()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( transactionType_.notNull() )
			transactionType_ = 0;
		if ( transactionTypeManager )
		{
			totalRecords = transactionTypeManager->initializeAllTransactionTypesQuery();
 			transactionType_ = transactionTypeManager->getFirstTransactionType();
		}
	}

	if ( transactionType_.notNull() )
	{
		if ( transactionType_->isRecordValid() )
		{
			SessionMessage msg(TransactionTypeDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(transactionType_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		transactionType_ = transactionTypeManager->getNextTransactionType();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFITransactionTypeNames);
}


void BackupRestoreManager::backupStreamingGraphicsGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (graphicManager)
			{
				totalRecords = 0;//zzz graphicManager->countAllGraphics(graphicType);
	 			graphic = 0; //zzz graphicManager->firstGraphic(graphicType);
				Debug::trace("BackupRestoreManager.Graphics", "backupStreamingGraphicsGraphics :     totalRecords = " +
					util_string::inttostring(totalRecords) + "   firstGraphic = " + util_string::ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFIStreamingGraphicsGraphics;
	int saveEvent = WriteStreamingGraphic;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupStreamingGraphicsGraphics :     isCustom = " +
			booltostring(graphic->isCustom()) + "   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->getErrorCode() == RECORD_OK)
		{
			SessionMessage msg(GraphicDev, saveEvent);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(graphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
		delete graphic;
		graphic = 0; //zzz graphicManager->nextGraphic(graphicType);
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}

void BackupRestoreManager::backupLabelGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (labelGraphicManager)
			{
				totalRecords = labelGraphicManager->countAllGraphics();
	 			graphic = labelGraphicManager->firstGraphic();
				Debug::trace("BackupRestoreManager.Graphics", "backupLabelGraphics :   totalRecords = " + inttostring(totalRecords) +
					"   firstGraphic = " + ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFIUserLabelGraphics;
	int saveEvent = DtWrite;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupLabelGraphics :   isCustom = " + booltostring(graphic->isCustom()) +
			"   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->isCustom())
		{
			if (graphic->getErrorCode() == RECORD_OK)
			{
				SessionMessage msg(GraphicDev, saveEvent);
				msg.setFormatType(EvolutionTaggedFormat);
				msg.setHfcObject(graphic);
				backupARecord(&msg);
			}
			else
				totalRecordErrors++;

			currentRecordCount++;
			progressTotal++;
		}
		delete graphic;
		graphic = labelGraphicManager->nextGraphic();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}


void BackupRestoreManager::backupProductGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (productGraphicManager)
			{
				productGraphicManager->setCurrentSubType(Graphic::SUBTYPE_USER);
				totalRecords = productGraphicManager->countAllGraphics();
	 			graphic = productGraphicManager->firstGraphic();
				Debug::trace("BackupRestoreManager.Graphics", "backupProductGraphics :   totalRecords = " + inttostring(totalRecords) +
					"   firstGraphic = " + ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFIProductGraphics;
	int saveEvent = WriteProductGraphic;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupProductGraphics :   isCustom = " + booltostring(graphic->isCustom()) +
			"   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->getErrorCode() == RECORD_OK)
		{
			SessionMessage msg(GraphicDev, saveEvent);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(graphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
		delete graphic;
		graphic = productGraphicManager->nextGraphic();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}


void BackupRestoreManager::backupCategoryGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (categoryGraphicManager)
			{
				categoryGraphicManager->setCurrentSubType(Graphic::SUBTYPE_USER);
				totalRecords = categoryGraphicManager->countAllGraphics();
	 			graphic = categoryGraphicManager->firstGraphic();
				Debug::trace("BackupRestoreManager.Graphics", "backupCategoryGraphics :   totalRecords = " + inttostring(totalRecords) +
					"   firstGraphic = " + ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFICategoryGraphics;
	int saveEvent = WriteCategoryGraphic;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupCategoryGraphics :   isCustom = " + booltostring(graphic->isCustom()) +
			"   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->getErrorCode() == RECORD_OK)
		{
			SessionMessage msg(GraphicDev, saveEvent);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(graphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
		delete graphic;
		graphic = categoryGraphicManager->nextGraphic();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}


void BackupRestoreManager::backupSpecialsGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (specialsGraphicManager)
			{
				totalRecords = specialsGraphicManager->countAllGraphics();
	 			graphic = specialsGraphicManager->firstGraphic();
				Debug::trace("BackupRestoreManager.Graphics", "backupSpecialsGraphics :   totalRecords = " + inttostring(totalRecords) +
					"   firstGraphic = " + ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFISpecialsGraphics;
	int saveEvent = WriteSpecialsGraphic;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupSpecialsGraphics :   isCustom = " + booltostring(graphic->isCustom()) +
			"   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->getErrorCode() == RECORD_OK)
		{
			SessionMessage msg(GraphicDev, saveEvent);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(graphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
		delete graphic;
		graphic = specialsGraphicManager->nextGraphic();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}


void BackupRestoreManager::backupSoftkeyGraphics()
{
	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the graphics in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (graphic)
			{
				delete graphic;
				graphic = 0;
			}
			if (softkeyGraphicManager)
			{
				totalRecords = softkeyGraphicManager->countAllGraphics();
	 			graphic = softkeyGraphicManager->firstGraphic();
				Debug::trace("BackupRestoreManager.Graphics", "backupSoftkeyGraphics :   totalRecords = " + inttostring(totalRecords) +
					"   firstGraphic = " + ptrtostring(graphic));
			}
		}
	}

	BRFileIndicator whichGraphic = BRFISoftkeyGraphics;
	int saveEvent = WriteSoftkeyGraphic;

	if (graphic)
	{
		Debug::trace("BackupRestoreManager.Graphics", "backupSoftkeyGraphics :   isCustom = " + booltostring(graphic->isCustom()) +
			"   errorCode = " + longtostring(graphic->getErrorCode()) + "   graphic = " + ptrtostring(graphic));
		if (graphic->getErrorCode() == RECORD_OK)
		{
			SessionMessage msg(GraphicDev, saveEvent);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(graphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
		delete graphic;
		graphic = softkeyGraphicManager->nextGraphic();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, whichGraphic);
}



void BackupRestoreManager::backupLabelTypes()
{
	bool backingupRomLabels = Configuration::getInstance()->toBoolCfgI(CfgInternal::BACKUPROMLABELS);

	if (startingNewFile)
	{
		if (format == BRUltima)
		{
			// We don't backup the labelTypes in Ultima disk format.
			doneWithSubState = true;
			return;
		}
		else
		{
			startingNewFile = false;
			currentRecordCount = 0;
			if (labelType)
			{
				delete labelType;
				labelType = 0;
			}
			if (labelTypeManager)
			{
				if (backingupRomLabels)
				{
					totalRecords = labelTypeManager->countAllLabelTypes();  // It doesn't matter, but this doesn't work; it passes in -1 for the deptNum, which just returns back without doing anything.  Obviously this method is never used!!
					labelType = labelTypeManager->firstLabelType(0);
				}
				else
				{
					totalRecords = labelTypeManager->countAllCustomLabelTypes();
					labelType = labelTypeManager->firstCustomLabelType();
				}
			}
		}
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, BRFILabelTypes);

	if (labelType)
	{
		if (labelType->isCustom() || backingupRomLabels)
		{
			if (labelType->isRecordValid())
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", HobString("Backup LabelType ") + HobString(Long::toString(labelType->getLabelTypeNum())));
				SessionMessage msg(LabelTypeDev, DtWrite);
				// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
				switch (format)
				{
					case BRVersionIndependent:
						msg.setFormatType(DBIndependentFormatFormat);
						break;

					default:
						msg.setFormatType(EvolutionTaggedFormat);
						break;
				}

				msg.setHfcObject(labelType);
				backupARecord(&msg);
			}
			else
				totalRecordErrors++;

			currentRecordCount++;
			progressTotal++;
		}

		delete labelType;
		if (backingupRomLabels)
			labelType = labelTypeManager->nextLabelType(false, 0, 0);
		else
			labelType = labelTypeManager->nextCustomLabelType();
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, BRFILabelTypes);
}

void BackupRestoreManager::backupLabelTextNames()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (genericLabelText)
		{
			delete genericLabelText;
			genericLabelText = 0;
		}
		if (genericLabelTextManager)
		{
			totalRecords = genericLabelTextManager->countAllGenericLabelTexts();
			genericLabelText = genericLabelTextManager->firstGenericLabelText();
		}
	}

	if (genericLabelText)
	{
		if (genericLabelText->isRecordValid())
		{
			SessionMessage msg(GenericTextDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(genericLabelText);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete genericLabelText;
		genericLabelText = genericLabelTextManager->nextGenericLabelText();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIResources, BRFILabelTextNames);
}


void BackupRestoreManager::backupStoreData()
{
	SessionMessage msg(ConfigurationDev, CfgStoreInfoWrite);
	msg.setFormatType((format == BRUltima) ? UltimaTaggedFormat : EvolutionTaggedFormat);
	CfgStoreInfo myConfig;
	msg.setHfcObject(&myConfig);
	backupARecord(&msg);

	currentRecordCount = 1;
	totalRecords = 1;
	progressTotal++;

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIStoreData, BRFIStData);
}

void BackupRestoreManager::backupScaleConfig()
{
	Debug::trace("BackupRestoreManager", "Back up ScaleConfig.");
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		totalRecords = 1;
	}
	CfgAll cfg;

	SessionMessage msg(ConfigurationDev, DtWrite);
	// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
	switch (format)
	{
		case BRVersionIndependent:
	Debug::trace("BackupRestoreManager", "Back up ScaleConfig.  BRVersionIndependent");
			msg.setFormatType(DBIndependentFormatFormat);
			break;
		case BRUltima:
	Debug::trace("BackupRestoreManager", "Back up ScaleConfig.  BRUltima");
			msg.setFormatType(UltimaTaggedFormat);
			break;
		default:
	Debug::trace("BackupRestoreManager", "Back up ScaleConfig.  default");
			msg.setFormatType(EvolutionTaggedFormat);
			break;
	}

	msg.setHfcObject(&cfg);
	backupARecord(&msg);

	currentRecordCount++;
	progressTotal++;

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIConfigData);
}

// Added for the Remote Scale Maintenance feature. Only expected to be used with this feature.
void BackupRestoreManager::backupScaleStatus()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
	}
	ScaleStatus status;

	SessionMessage statusMsg(SessionDev, SnWriteScaleStatus);
	// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
	switch (format)
	{
		case BRVersionIndependent:
			statusMsg.setFormatType(DBIndependentFormatFormat);
			break;
		default:
			statusMsg.setFormatType(EvolutionTaggedFormat);
			break;
	}

	statusMsg.setHfcObject(&status);
	backupARecord(&statusMsg);

	currentRecordCount++;
	progressTotal++;

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	//if (displayStats)
	//	updateUI(BRGIScaleConfig, BRFIConfigData);
}

// Added for the Remote Scale Maintenance feature. Only expected to be used with this feature.

void BackupRestoreManager::backupScaleStatistics()
{
	currentRecordCount = 0;
	totalRecords = 1;

	SessionMessage statusMsg(SessionDev, SnWriteStatistics);
	// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
	switch (format)
	{
		case BRVersionIndependent:
			statusMsg.setFormatType(DBIndependentFormatFormat);
			break;
		default:
			statusMsg.setFormatType(EvolutionTaggedFormat);
			break;
	}

	Statistics stats(this);
	
	statusMsg.setHfcObject(&stats);
	backupARecord(&statusMsg);

	currentRecordCount++;
	progressTotal++;

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	//if (displayStats)
	//	updateUI(BRGIScaleConfig, BRFIConfigData);
}

void BackupRestoreManager::backupEmailAlerts()
{
//	emailAlertsManager->showAllEmailAlerts();

	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (emailAlert)
		{
			delete emailAlert;
			emailAlert = 0;
		}
		if (emailAlertsManager)
		{
			totalRecords = emailAlertsManager->countAllEmailAlerts();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "BackupEmailAlerts():   totalRecords = " + inttostring(totalRecords));
			emailAlert = emailAlertsManager->firstEmailAlert();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "BackupEmailAlerts():   totalRecords = " + inttostring(totalRecords) + "    first emailAlert = " + ptrtostring(emailAlert));
		}
	}

	if (emailAlert)
	{
		if (emailAlert->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(EmailAlertsDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(emailAlert);
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("Backup EmailAlerts:  Record is valid - calling backupARecord()."));
			backupARecord(&msg);
		}
		else
		{
			totalRecordErrors++;
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("Backup EmailAlerts:  Record is NOT valid - totalRecordErrors = ") + HobString(Integer::toString(totalRecordErrors)));
		}

		delete emailAlert;
		emailAlert = emailAlertsManager->nextEmailAlert();
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "Backup EmailAlerts():   nextEmailAlert = " + ptrtostring(emailAlert));
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIEmailAlerts);
}

void BackupRestoreManager::backupOperatorData()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (hobOperator)
		{
			delete hobOperator;
			hobOperator = 0;
		}
		if (operatorManager)
		{
			totalRecords = operatorManager->countAllOperators();
			hobOperator = operatorManager->firstOperator();
		}
	}

	if (hobOperator)
	{
		if (hobOperator->isRecordValid())
		{
			SessionMessage msg(OperatorDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(hobOperator);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete hobOperator;
		hobOperator = operatorManager->nextOperator();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIOperatorData, BRFIOpData);
}

void BackupRestoreManager::backupOperatorNote()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		totalRecords = 1;
	}

	OperatorNote *opNote = OperatorNote::findOperatorNote(this);
	if (opNote)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "backupOperatorNote() - found operator note in the db");
		if (opNote->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "backupOperatorNote() - operator note is valid");
			SessionMessage msg(OperatorDev, OpWriteNote);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(opNote);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete opNote;

		currentRecordCount++;
		progressTotal++;
	}
	else
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "backupOperatorNote() - no operator note in the db");

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIOperatorData, BRFIOpNote);
}

void BackupRestoreManager::backupOperatorChanges()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( operatorChange_.notNull() )
			operatorChange_ = 0;
		if ( operatorChangeManager )
		{
			totalRecords = operatorChangeManager->initializeAllOperatorChangesQuery();
			operatorChange_ = operatorChangeManager->getFirstOperatorChange();
		}
	}

	if ( operatorChange_.notNull() )
	{
		if ( operatorChange_->isRecordValid() )
		{
			SessionMessage msg(OperatorChangesDev, DtWrite);
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(operatorChange_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		operatorChange_ = operatorChangeManager->getNextOperatorChange();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGIOperatorData, BRFIOpChanges);
}


void BackupRestoreManager::backupWrapperRequestSettings()
{
	if(wrapperTimingsReady)	
	{
		if(wrapperTimingsManager)
			pMyWrapperTimings = wrapperTimingsManager->getEvolWrapperPackageTimings();

		wrapperTimingsReady = false;
		doneWithSubState = true;
	}
}
void BackupRestoreManager::backupWrapperSettings()
{
	Debug::trace("BackupRestoreManager.EvolutionWrapper", "Back up Wrapper Timings.");
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		totalRecords = 1;
	}
	if (wrPackageTimingsPtr)
	{
		Debug::trace("BackupRestoreManager.EvolutionWrapper", "Got a Wrapper Timings msg.");
		bool traceWrap = Debug::isTracing("BackupRestoreManager.EvolutionWrapper");
		if (traceWrap)
			Debug::trace("BackupRestoreManager.EvolutionWrapper", "wrPackageTimingsPtr = " + HobString(Hex::toString((long long) wrPackageTimingsPtr)));
		EvolWrapperPackageTimings *tempWrPtr = wrPackageTimingsPtr->getTimingsPointer();
		
		if (tempWrPtr)
		{
			SessionMessage wespt(Wrapper, WrEvolBackupPackageTimings);
			wespt.setFormatType(EvolutionTaggedFormat);
			wespt.setHfcObject(wrPackageTimingsPtr);
						
			backupARecord(&wespt);
			Debug::trace("BackupRestoreManager.EvolutionWrapper", "Wrapper Timings have been backed up.");

			//delete tempWrPtr;				//	The recipient of this pointer needs to delete it when done with it.
			//delete wrPackageTimingsPtr;
			//wrPackageTimingsPtr = 0;
		}
		else
			totalRecordErrors++;

		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		Debug::trace("BackupRestoreManager.EvolutionWrapper", "No Wrapper Timings");
	}

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIWrapperSettings, BRFIWrSettings);
}


void BackupRestoreManager::backupPrinterSettings()
{
	Debug::trace("BackupRestoreManager.PrinterSettings", "Back up Printer Settings.");
	bool nextPrinter = true;
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		_printerType_ = FirstPrinter;
		_printerCfgRetries_ = PRINTER_CONFIG_RETRIES;
		Debug::trace("BackupRestoreManager.PrinterSettings", "Back up Printer Timings:  Starting with FirstPrinter.");
	}
	HobString printerString = (_printerType_ == FirstPrinter ? "FirstPrinter" : (_printerType_ == SecondPrinter ? "SecondPrinter" : (_printerType_ == ThirdPrinter ? "ThirdPrinter" : "Invalid Printer")));

	//	Is this printer attached?
	if (SessionManager::getInstance()->isPrinterAttached((PrinterType) _printerType_))
	{
		//	This BackupRestore 'Session' knows nothing about any attached peripherals, so we have to go to the Primary session.
		bool problemRetrievingInfo = true;
		Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
		if (primarySession)
		{
			ConfigurationManager *primaryConfigMgr = primarySession->getConfigurationManager();
			if (primaryConfigMgr)
			{
				Debug::trace("BackupRestoreManager.PrinterSettings", "getCurrentPrinterConfig(" + Integer::toString(_printerType_) + ")");
				HfcPrinterConfig *pHfcPrinterConfig = primaryConfigMgr->getCurrentPrinterConfig((PrinterType) _printerType_);
				if (pHfcPrinterConfig)
				{
					Debug::trace("BackupRestoreManager.PrinterSettings", "********        Got configuration for Printer # " + Integer::toString(_printerType_));
					pHfcPrinterConfig->show();
					Debug::trace("BackupRestoreManager.PrinterSettings", "********************************************************");

					SessionMessage pset(Printer, PrBackupPrinterSettings);
					pHfcPrinterConfig->setEventType(PrBackupPrinterSettings);
					switch (format)
					{
						case BRVersionIndependent:
							pset.setFormatType(DBIndependentFormatFormat);
							break;
						default:
							pset.setFormatType(EvolutionTaggedFormat);
							break;
					}
					pset.setHfcObject(pHfcPrinterConfig);
								
					backupARecord(&pset);
					Debug::trace("BackupRestoreManager.PrinterSettings", "Printer Settings have been backed up for Printer # " + Integer::toString(_printerType_));

					currentRecordCount++;
					progressTotal++;

					MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Backed up Printer Settings for " + printerString + ".");
				}
				else
				{
					if (--_printerCfgRetries_ <= 0)
					{
						Debug::trace("BackupRestoreManager.PrinterSettings", "No Printer Settings for Printer # " + Integer::toString(_printerType_));
						MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + ":  Could not retrieve Printer configuration.");
					}
					else
					{
						//	Send a PrReqInquire message
						Debug::trace("BackupRestoreManager.PrinterSettings", "Requesting Printer Config for Printer # " + Integer::toString(_printerType_) + ",  Retries = " + Integer::toString(_printerCfgRetries_));
						requestPrinterConfig((PrinterType) _printerType_);
						PAL_absclass::PAL_sleep( 500 );
						MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + ":  Requesting Printer configuration.");
						nextPrinter = false;
					}
				}
				problemRetrievingInfo = false;		//	We've already logged a specifig message.
			}
			else
			{
				Debug::trace("BackupRestoreManager.PrinterSettings", "backupPrinterSettings():  Can't get the primary ConfigurationManager! Exiting.");
			}
		}
		else
		{
			Debug::trace("BackupRestoreManager.PrinterSettings", "backupPrinterSettings():  Can't get the primary Session! Exiting.");
		}

		if (problemRetrievingInfo)
		{
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + ":  Could not retrieve Printer configuration.");
		}
	}
	else
	{
		MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + " is not attached");
	}

	if (nextPrinter)
	{
		++_printerType_;
		_printerCfgRetries_ = PRINTER_CONFIG_RETRIES;	//	Reset it for the next printer type, if applicable.
	}

	if (_printerType_ >= MaxPrinterTypes)
	{
		startingNewFile = true;
		doneWithSubState = true;
		Debug::trace("BackupRestoreManager.PrinterSettings", "Done with Printer Settings.");
	}
	else
	{
		Debug::trace("BackupRestoreManager.PrinterSettings", "Next time get Printer Settings for Printer # " + Integer::toString(_printerType_));
	}

	//update the UI with the current task progress
	//	Just use configuration since this will just be in the background anyway.
//	if (displayStats)
//		updateUI(BRGIScaleConfig, BRFIConfigData);
}

void BackupRestoreManager::backupWeigherSettings()
{
	Debug::trace("BackupRestoreManager.WeigherSettings", "Back up Weigher Settings.");
	bool nextWeigher = true;
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		_weigherCfgRetries_ = WEIGHER_CONFIG_RETRIES;
	}

	//	Is Weigher attached?
	if ( SessionManager::getInstance()->isWeigherAttached() )
	{
		//	This BackupRestore 'Session' knows nothing about any attached peripherals, so we have to go to the Primary session.
		bool problemRetrievingInfo = true;
		Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
		if (primarySession)
		{
			ConfigurationManager *primaryConfigMgr = primarySession->getConfigurationManager();
			if (primaryConfigMgr)
			{
				Debug::trace("BackupRestoreManager.WeigherSettings", "getCurrentWeigherConfig()");
				HfcWeigherConfig *pHfcWeigherConfig = primaryConfigMgr->getCurrentWeigherConfig();
				if (pHfcWeigherConfig)
				{
					Debug::trace("BackupRestoreManager.WeigherSettings", "********        Got configuration for Weigher ");
					pHfcWeigherConfig->show();
					Debug::trace("BackupRestoreManager.WeigherSettings", "********************************************************");

					SessionMessage pset(Weigher, WgRestoreWeigherSettings);
					pHfcWeigherConfig->setEventType(WgRestoreWeigherSettings);
					pset.setFormatType(EvolutionTaggedFormat);
					pset.setHfcObject(pHfcWeigherConfig);
								
					backupARecord(&pset);
					Debug::trace("BackupRestoreManager.WeigherSettings", "Weigher Settings have been backed up");

					currentRecordCount++;
					progressTotal++;

					MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Backed up Weigher Settings.");
				}
				else
				{
					if (--_weigherCfgRetries_ <= 0)
					{
						Debug::trace("BackupRestoreManager.WeigherSettings", "No Weigher Settings");
						MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Could not retrieve Weigher configuration.");
					}
					else
					{
						//	Send a WgReqInquire message
						Debug::trace("BackupRestoreManager.WeigherSettings", "Requesting Weigher Config for Weigher,  Retries = " + Integer::toString(_weigherCfgRetries_));
						requestWeigherConfig();
						PAL_absclass::PAL_sleep( 500 );
						MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Requesting Weigher configuration.");
					}
				}
				problemRetrievingInfo = false;		//	We've already logged a specifig message.
			}
			else
			{
				Debug::trace("BackupRestoreManager.WeigherSettings", "backupWeigherSettings():  Can't get the primary ConfigurationManager! Exiting.");
			}
		}
		else
		{
			Debug::trace("BackupRestoreManager.WeigherSettings", "backupWeigherSettings():  Can't get the primary Session! Exiting.");
		}

		if (problemRetrievingInfo)
		{
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Could not retrieve Weigher configuration.");
		}
	}
	else
	{
		MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher is not attached");
	}

	startingNewFile = true;
	doneWithSubState = true;
	Debug::trace("BackupRestoreManager.WeigherSettings", "Done with Weigher Settings.");

	//update the UI with the current task progress
	//	Just use configuration since this will just be in the background anyway.
//	if (displayStats)
//		updateUI(BRGIScaleConfig, BRFIConfigData);
}


void BackupRestoreManager::backupProductTotals()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( productTotal_.notNull() )
			productTotal_ = 0;

		allDepts.findAll(this);
		deptNumList = allDepts.getDeptNumList();
		HobString deptNumStr = "0";

		if ( productTotalManager )
		{
			totalRecords = productTotalManager->initializeAllProductTotalsQuery(deptNumStr);
 			productTotal_ = productTotalManager->getFirstProductTotal();
		}
	}

	if ( productTotal_.notNull() )
	{
		if ( productTotal_->isRecordValid() )
		{
			SessionMessage msg(ProductTotalDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(productTotal_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		productTotal_ = productTotalManager->getNextProductTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically read the totals for dept 0 in the first pass through this method (since deptNumStr was initialized to "0").
		HobString deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}

		if (deptNumStr.empty())
		{
			startingNewFile = true;
			doneWithSubState = true;
		}
		else
		{
			if (productTotal_.notNull())
				productTotal_ = 0;

			if (productTotalManager)
			{
				totalRecords += productTotalManager->initializeAllProductTotalsQuery(deptNumStr);
				productTotal_ = productTotalManager->getFirstProductTotal();
			}

		}
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIProductTotals);
}

void BackupRestoreManager::backupClassTotals()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( classTotal_.notNull() )
			classTotal_ = 0;

		allDepts.findAll(this);
		deptNumList = allDepts.getDeptNumList();
		HobString deptNumStr = "0";

		if ( classTotalManager )
		{
			totalRecords = classTotalManager->initializeAllClassTotalsQuery(deptNumStr);
 			classTotal_ = classTotalManager->getFirstClassTotal();
		}
	}

	if ( classTotal_.notNull() )
	{
		if ( classTotal_->isRecordValid() )
		{
			SessionMessage msg(ClassTotalDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(classTotal_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		classTotal_ = classTotalManager->getNextClassTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically read the totals for dept 0 in the first pass through this method (since deptNumStr was initialized to "0").
		HobString deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}

		if (deptNumStr.empty())
		{
			startingNewFile = true;
			doneWithSubState = true;
		}
		else
		{
			if (classTotal_.notNull())
				classTotal_ = 0;

			if (classTotalManager)
			{
				totalRecords += classTotalManager->initializeAllClassTotalsQuery(deptNumStr);
				classTotal_ = classTotalManager->getFirstClassTotal();
			}

		}
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIClassTotals);
}

void BackupRestoreManager::backupOperatorTotals()
{
	if ( startingNewFile )
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( operatorTotal_.notNull() )
			operatorTotal_ = 0;
		if ( operatorTotalManager )
		{
			totalRecords = operatorTotalManager->initializeAllOperatorTotalsQuery();
 			operatorTotal_ = operatorTotalManager->getFirstOperatorTotal();
		}
	}

	if ( operatorTotal_.notNull() )
	{
		if ( operatorTotal_->isRecordValid() )
		{
			SessionMessage msg(OperatorTotalDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(operatorTotal_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		operatorTotal_ = operatorTotalManager->getNextOperatorTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIOperatorTotals);
}

void BackupRestoreManager::backupHourlyTotals()
{
	if ( startingNewFile )
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( hourlyTotal_.notNull() )
			hourlyTotal_ = 0;
		if ( hourlyTotalManager )
		{
			totalRecords = hourlyTotalManager->initializeAllHourlyTotalsQuery();
 			hourlyTotal_ = hourlyTotalManager->getFirstHourlyTotal();
		}
	}

	if ( hourlyTotal_.notNull() )
	{
		if ( hourlyTotal_->isRecordValid() )
		{
			SessionMessage msg(HourlyTotalDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(hourlyTotal_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		hourlyTotal_ = hourlyTotalManager->getNextHourlyTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIHourlyTotals);
}


void BackupRestoreManager::backupTransactions()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( transaction_.notNull() )
			transaction_ = 0;
		if ( transactionManager )
		{
			totalRecords = transactionManager->initializeAllTransactionsQuery();
			transaction_ = transactionManager->getFirstTransaction();
		}
	}

	if ( transaction_.notNull() )
	{
		if ( transaction_->isRecordValid() )
		{
			SessionMessage msg(TransactionDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(transaction_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		transaction_ = transactionManager->getNextTransaction();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIHourlyTotals);
}


void BackupRestoreManager::backupNotClearedTransactions()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( transaction_.notNull() )
			transaction_ = 0;
		if ( transactionManager )
		{
			totalRecords = transactionManager->initializeNotClearedQuery();
			// In order to be able to clear the transactions later, we first need to mark them as read.
			transaction_ = transactionManager->getFirstReadClearTransaction();
		}
	}

	if ( transaction_.notNull() )
	{
		if ( transaction_->isRecordValid() )
		{
			// HW-140: This Jira entry is not a bug, so it should not have been fixed.  I am removing the changes that were put in.
			//if ( transaction_->getVoid() == Transaction::VoidNotVoided )	//	HW-140: Include it only if it has not been voided.
			//{
				SessionMessage msg(TransactionDev, DtWrite);
				Transaction *trans = transaction_.get();
				// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
				switch (format)
				{
					case BRVersionIndependent:
						msg.setFormatType(DBIndependentFormatFormat);
						break;
					default:
						msg.setFormatType(EvolutionTaggedFormat);
						if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgReadTransactionsCompatibility))
						{
							trans->setFormatType(ReadTransactionCompatibility);
							trans->setScaleId( Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsScaleId) );
						}
						break;
				}
				msg.setHfcObject(trans);
				backupARecord(&msg);
			//}
		}
		else
			totalRecordErrors++;

		// In order to be able to clear the transactions later, we first need to mark them as read.
		transaction_ = transactionManager->getNextReadClearTransaction();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		// We can now ask the transaction manager to clear the transactions
		if ( transactionManager )
			transactionManager->clearReadTransactions();

		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFIHourlyTotals);
}


void BackupRestoreManager::backupCutTestTotals()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (cutTestTotal_.notNull())
			cutTestTotal_ = 0;
		if (cutTestTotalManager)
		{
			totalRecords = cutTestTotalManager->initializeAllCutTestTotalsQuery();
			cutTestTotal_ = cutTestTotalManager->getFirstCutTestTotal();
		}
	}

	if (cutTestTotal_.notNull())
	{
		if (cutTestTotal_->isRecordValid())
		{
			SessionMessage msg(CutTestTotalDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			msg.setHfcObject(cutTestTotal_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		cutTestTotal_ = cutTestTotalManager->getNextCutTestTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFICutTestTotal);
}


void BackupRestoreManager::backupCuttingTests()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (cuttingTest_.notNull())
			cuttingTest_ = 0;
		if (cutTestManager)
		{
			totalRecords = cutTestManager->countAllCuttingTests();
 			cuttingTest_ = cutTestManager->firstCuttingTest();
		}
	}

	if (cuttingTest_.notNull())
	{
		if (cuttingTest_->isRecordValid())
		{
			SessionMessage msg(CutTestDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			msg.setFormatType(EvolutionTaggedFormat);
			if (format == BRVersionIndependent)
				cuttingTest_->setFormatType(DBIndependentFormatFormat);
			msg.setHfcObject(cuttingTest_.get());
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		cuttingTest_ = cutTestManager->nextCuttingTest();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGITotals, BRFICuttingTest);
}


void BackupRestoreManager::backupCategories()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (category)
		{
			delete category;
			category = 0;
		}
		if (categoryManager)
		{
			totalRecords = categoryManager->countAllCategories();
			category = categoryManager->firstCategory();
		}
	}

	if (category)
	{
		if (category->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(CategoryDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(category);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete category;
		category = categoryManager->nextCategory();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISelfServe, BRFICategories);
}

void BackupRestoreManager::backupPresentationMethods()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (presentationMethod)
		{
			delete presentationMethod;
			presentationMethod = 0;
		}
		presentationMethodManager = new PresentationMethodManager();
		if (presentationMethodManager)
		{
			presentationMethodManager->setDBAccess(this);
			totalRecords = presentationMethodManager->countAllPresentationMethods();
			presentationMethod = presentationMethodManager->firstPresentationMethod();
		}
	}

	if (presentationMethod)
	{
		if (presentationMethod->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(PresentationMethodDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(presentationMethod);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete presentationMethod;
		presentationMethod = presentationMethodManager->nextPresentationMethod();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		if (presentationMethodManager)
		{
			delete presentationMethodManager;
			presentationMethodManager = 0;
		}
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISelfServe, BRFIPresentationMethods);
}

void BackupRestoreManager::backupSpecials()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (special)
		{
			delete special;
			special = 0;
		}
		if (specialsManager)
		{
			totalRecords = specialsManager->countAllSpecials();
			special = specialsManager->firstSpecial();
		}
	}

	if (special)
	{
		if (special->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(SpecialsDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(special);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete special;
		special = specialsManager->nextSpecial();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISelfServe, BRFISpecials);
}

void BackupRestoreManager::backupStreamingGraphics()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (streamingGraphic)
		{
			delete streamingGraphic;
			streamingGraphic = 0;
		}
		if (streamingGraphicManager)
		{
			totalRecords = streamingGraphicManager->countAllStreamingGraphics();
			streamingGraphic = streamingGraphicManager->firstStreamingGraphic();
		}
	}

	if (streamingGraphic)
	{
		if (streamingGraphic->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(StreamingGraphicDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(streamingGraphic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete streamingGraphic;
		streamingGraphic = streamingGraphicManager->nextStreamingGraphic();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISelfServe, BRFIStreamingGraphics);
}

void BackupRestoreManager::backupProductStatistics()
{
	if (format == BRUltima)
	{
		doneWithSubState = true;
		return;
	}

	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (productStatistic)
		{
			delete productStatistic;
			productStatistic = 0;
		}
		if (productManager)
		{
			totalRecords = productManager->countAllProductStatistics();
			productStatistic = productManager->firstProductStatistic();
		}
	}

	if (productStatistic)
	{
		if (productStatistic->isRecordValid())
		{
			SessionMessage msg(ProductStatisticsDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(productStatistic);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete productStatistic;
		productStatistic = productManager->nextProductStatistic();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIProductStatistics);
}


void BackupRestoreManager::backupCOOLCategories()
{
	if (startingNewFile)
	{
		Debug::trace("BackupRestoreManager.cool", "Entering backupCOOLCategories for the first time.");
		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCategory)
		{
			delete coolCategory;
			coolCategory = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCategories();
			coolCategory = coolManager->firstCOOLCategory();
		}
	}

	if (coolCategory)
	{
		Debug::trace("BackupRestoreManager.cool", "have a coolCategory for backup");
		if (coolCategory->isRecordValid())			//if the record is not valid, don't send it
		{
			Debug::trace("BackupRestoreManager.cool", "have a coolCategory for backup, and it's valid");
			SessionMessage msg(COOLDev, COOLDevWriteCategory);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolCategory);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolCategory;
		coolCategory = coolManager->nextCOOLCategory();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategory);
}

void BackupRestoreManager::backupCOOLCountries()
{
	if (startingNewFile)
	{
		Debug::trace("BackupRestoreManager.cool", "Entering backupCOOLCountries for the first time.");
		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCountry)
		{
			delete coolCountry;
			coolCountry = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCountries();
			coolCountry = coolManager->firstCOOLCountry();
		}
	}

	if (coolCountry)
	{
		if (coolCountry->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteCountry);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolCountry);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolCountry;
		coolCountry = coolManager->nextCOOLCountry();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCountry);
}

void BackupRestoreManager::backupCOOLProductionSteps()
{
	if (startingNewFile)
	{
		Debug::trace("BackupRestoreManager.cool", "Entering backupCOOLProductionSteps for the first time.");
		startingNewFile = false;
		currentRecordCount = 0;
		if (coolProductionStep)
		{
			delete coolProductionStep;
			coolProductionStep = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLProductionSteps();
			coolProductionStep = coolManager->firstCOOLProductionStep();
		}
	}

	if (coolProductionStep)
	{
		if (coolProductionStep->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteProductionStep);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolProductionStep);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolProductionStep;
		coolProductionStep = coolManager->nextCOOLProductionStep();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLProductionStep);
}

void BackupRestoreManager::backupCOOLPretexts(bool forUltima)
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "backupCOOLPretexts(";
		msgDebug.append(Bool::toString(forUltima)).append("):  ");
	}

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolPretext)
		{
			delete coolPretext;
			coolPretext = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLPretexts();
			coolPretext = coolManager->firstCOOLPretext();
		}
	}

	if (coolPretext)
	{
		if (coolPretext->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWritePretext);
			if (forUltima)
				msg.setEventType(COOLDevWriteAllCoolTextForUltima);

			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					if (forUltima)
						coolPretext->setFormatType(UltimaTaggedFormat);
					break;
			}

			msg.setHfcObject(coolPretext);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolPretext;
		coolPretext = coolManager->nextCOOLPretext();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLPretext);
}

void BackupRestoreManager::backupCOOLTexts(bool forUltima)
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "backupCOOLTexts(";
		msgDebug.append(Bool::toString(forUltima)).append("):  ");
	}

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolText)
		{
			delete coolText;
			coolText = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLTexts();
			coolText = coolManager->firstCOOLText();
		}
	}

	if (coolText)
	{
		if (coolText->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteCoolText);
			if (forUltima)
				msg.setEventType(COOLDevWriteAllCoolTextForUltima);

			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					if (forUltima)
						coolText->setFormatType(UltimaTaggedFormat);
					break;
			}

			msg.setHfcObject(coolText);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolText;
		coolText = coolManager->nextCOOLText();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLText);
}

void BackupRestoreManager::backupCOOLShortlists(bool forUltima)
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "backupCOOLShortlists(";
		msgDebug.append(Bool::toString(forUltima)).append("):  ");
	}

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolShortlist)
		{
			delete coolShortlist;
			coolShortlist = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLShortlists();
			coolShortlist = coolManager->firstCOOLShortlist();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolShortlist)
	{
		if (coolShortlist->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteShortlist);
			if (forUltima)
				msg.setEventType(COOLDevWriteAllShortlistsForUltima);

			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					if (forUltima)
						coolShortlist->setFormatType(UltimaTaggedFormat);
					break;
			}

			msg.setHfcObject(coolShortlist);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolShortlist;
		coolShortlist = coolManager->nextCOOLShortlist();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLShortlist);
}



void BackupRestoreManager::backupCOOLCategoryCountries()
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "backupCOOLCategoryCountries(";

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCategoryCountry)
		{
			delete coolCategoryCountry;
			coolCategoryCountry = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCategoryCountries();
			coolCategoryCountry = coolManager->firstCOOLCategoryCountry();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolCategoryCountry)
	{
		if (coolCategoryCountry->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteCategoryCountry);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.

			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolCategoryCountry);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolCategoryCountry;
		coolCategoryCountry = coolManager->nextCOOLCategoryCountry();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategoryCountry);
}


void BackupRestoreManager::backupCOOLCategoryCountriesForUltima()
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "backupCOOLCategoryCountriesForUltima():  ";

	//	This file needs to read the dbCoolCountry file and then read the dbCoolCategoryCountry file
	//	to put a single CoolCategoryCountry record together.
	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCountry)
		{
			delete coolCountry;
			coolCountry = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCountriesForAllCategories();
			totalRecords += coolManager->countAllCOOLCategoryCountries();
			coolCountry = coolManager->firstCOOLCountry();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolCountry)
	{
		if (coolCountry->isRecordValid())			//if the record is not valid, don't send it
		{
			DBAccess *myDBAccess = mySession;

			if (coolCountry->isAllCategoriesFlag())
			{
				//	Backup this Country since it applies to all categories.
				COOLCategoryCountry catCountry(coolCountry, myDBAccess);

				SessionMessage msg(COOLDev, COOLDevWriteAllCategoryCountriesForUltima);
				switch (format)
				{
					case BRVersionIndependent:
						msg.setFormatType(DBIndependentFormatFormat);
						break;
					default:
						msg.setFormatType(EvolutionTaggedFormat);
						catCountry.setFormatType(UltimaTaggedFormat);
						break;
				}

				msg.setHfcObject(&catCountry);
				backupARecord(&msg);

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  Country record ").append(coolCountry->getName1());
					msgDebug2.append(" was backed up.");
					Debug::trace("BackupRestoreManager.cool", msgDebug2);
				}
			}
			else
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  Country record ").append(coolCountry->getName1());
					msgDebug2.append(" was NOT backed up since it has categories in the other file.");
					Debug::trace("BackupRestoreManager.cool", msgDebug2);
				}
			}

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Now check to see if there are categories for ").append(coolCountry->getName1()).append(".");
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
			
			//	Now check to see if this country is in the dbCoolCategoryCountry file.
			DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = myDBAccess->getDBCoolCategoryCountriesRM();
			if (dbCoolCategoryCountriesRM)
			{
				DBQuery<DBCoolCategoryCountry> *allCoolCategoriesCountryQuery = dbCoolCategoryCountriesRM->retrieveLinkedtoCoolCountry(coolCountry->getMyDbRecord());
				if (allCoolCategoriesCountryQuery)
				{
					if (traceOn)
						Debug::trace("BackupRestoreManager.cool", msgDebug + "Got a DBCoolCategoryCountry query.");

					DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = allCoolCategoriesCountryQuery->begin();

					//	Write out all the categories linked to this country.
					//	This should only happen via COMM for backwards compatibility with Quantum/Ultima.
					while (tempIter != allCoolCategoriesCountryQuery->end())
					{
						DBCoolCategoryCountry *c = new DBCoolCategoryCountry(*tempIter);
						COOLCategoryCountry *newCatCountry = new COOLCategoryCountry(c, myDBAccess);
						if (newCatCountry)
						{
							if (newCatCountry->isRecordValid())
							{
								SessionMessage msg(COOLDev, COOLDevWriteAllCategoryCountriesForUltima);

								switch (format)
								{
									case BRVersionIndependent:
										msg.setFormatType(DBIndependentFormatFormat);
										break;
									default:
										msg.setFormatType(EvolutionTaggedFormat);
										newCatCountry->setFormatType(UltimaTaggedFormat);
										break;
								}

								msg.setHfcObject(newCatCountry);
								backupARecord(&msg);
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("  COOLCategoryCountry record backed up. Category is ").append(Integer::toString(newCatCountry->getCategoryNum()));
									Debug::trace("BackupRestoreManager.cool", msgDebug + "");
								}
							}
							else
								totalRecordErrors++;

							delete newCatCountry;

							//	Update the counters
							currentRecordCount++;
							progressTotal++;
						}
						++tempIter;
					}
					delete allCoolCategoriesCountryQuery;
				}
			}
		}
		else
			totalRecordErrors++;

		delete coolCountry;
		coolCountry = coolManager->nextCOOLCountry();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategoryCountry);
}


void BackupRestoreManager::backupCOOLCategoryProductionSteps()
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "backupCOOLCategoryProductionSteps(";

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCategoryProdStep)
		{
			delete coolCategoryProdStep;
			coolCategoryProdStep = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCategoryProductionSteps();
			coolCategoryProdStep = coolManager->firstCOOLCategoryProductionStep();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolCategoryProdStep)
	{
		if (coolCategoryProdStep->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteCategoryProductionStep);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.

			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolCategoryProdStep);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolCategoryProdStep;
		coolCategoryProdStep = coolManager->nextCOOLCategoryProductionStep();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategoryProductionStep);
}


void BackupRestoreManager::backupCOOLCategoryProductionStepsForUltima()
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "backupCOOLCategoryProductionStepsForUltima(";

	//	This file needs to read the dbCoolProductionStep file and then read the dbCoolCategoryProductionStep file
	//	to put a single CoolCategoryProductionStep record together.
	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolProductionStep)
		{
			delete coolProductionStep;
			coolProductionStep = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLProductionSteps();
			totalRecords += coolManager->countAllCOOLCategoryProductionSteps();
			coolProductionStep = coolManager->firstCOOLProductionStep();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolProductionStep)
	{
		if (coolProductionStep->isRecordValid())			//if the record is not valid, don't send it
		{
			DBAccess *myDBAccess = mySession;

			if (coolProductionStep->isAllCategoriesFlag())			//if the record is not valid, don't send it
			{
				//	Backup this ProductionStep since it applies to all categories.
				COOLCategoryProdStep catProductionStep(coolProductionStep, myDBAccess);

				SessionMessage msg(COOLDev, COOLDevWriteAllCategoryProdStepsForUltima);

				switch (format)
				{
					case BRVersionIndependent:
						msg.setFormatType(DBIndependentFormatFormat);
						break;
					default:
						msg.setFormatType(EvolutionTaggedFormat);
						coolProductionStep->setFormatType(UltimaTaggedFormat);
						break;
				}

				msg.setHfcObject(&catProductionStep);
				backupARecord(&msg);

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  ProductionStep record ").append(coolProductionStep->getName1());
					msgDebug2.append(" was backed up.");
					Debug::trace("BackupRestoreManager.cool", msgDebug2);
				}
			}
			else
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  ProductionStep record ").append(coolProductionStep->getName1());
					msgDebug2.append(" was NOT backed up since it has categories in the other file.");
					Debug::trace("BackupRestoreManager.cool", msgDebug2);
				}
			}

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Now check to see if there are categories for ").append(coolProductionStep->getName1()).append(".");
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
			
			//	Now check to see if this country is in the dbCoolCategoryCountry file.
			DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = myDBAccess->getDBCoolCategoryProductionStepsRM();
			if (dbCoolCategoryProductionStepsRM)
			{
				DBQuery<DBCoolCategoryProductionStep> *allCoolCategoriesProductionStepQuery = dbCoolCategoryProductionStepsRM->retrieveLinkedtoCoolProductionStep(coolProductionStep->getMyDbRecord());
				if (allCoolCategoriesProductionStepQuery)
				{
					DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = allCoolCategoriesProductionStepQuery->begin();

					//	Write out all the categories linked to this country.
					//	This should only happen via COMM for backwards compatibility with Quantum/Ultima.
					while (tempIter != allCoolCategoriesProductionStepQuery->end())
					{
						DBCoolCategoryProductionStep *c = new DBCoolCategoryProductionStep(*tempIter);
						COOLCategoryProdStep *newCatProductionStep = new COOLCategoryProdStep(c, myDBAccess);
						if (newCatProductionStep)
						{
							if (newCatProductionStep->isRecordValid())
							{
								SessionMessage msg(COOLDev, COOLDevWriteAllCategoryProdStepsForUltima);

								switch (format)
								{
									case BRVersionIndependent:
										msg.setFormatType(DBIndependentFormatFormat);
										break;
									default:
										msg.setFormatType(EvolutionTaggedFormat);
										newCatProductionStep->setFormatType(UltimaTaggedFormat);
										break;
								}

								msg.setHfcObject(newCatProductionStep);
								backupARecord(&msg);
							}
							else
								totalRecordErrors++;

							delete newCatProductionStep;

							//	Update the counters
							currentRecordCount++;
							progressTotal++;
						}
						++tempIter;
					}
					delete allCoolCategoriesProductionStepQuery;
				}
			}
		}
		else
			totalRecordErrors++;

		delete coolProductionStep;
		coolProductionStep = coolManager->nextCOOLProductionStep();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategoryProductionStep);
}


void BackupRestoreManager::backupCOOLCategoryPretexts()
{
	bool traceOn = Debug::isTracing("BackupRestoreManager.cool");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "backupCOOLCategoryPretexts():  ";

	if (startingNewFile)
	{
		if (traceOn)
			Debug::trace("BackupRestoreManager.cool", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (coolCategoryPretext)
		{
			delete coolCategoryPretext;
			coolCategoryPretext = 0;
		}
		if (coolManager)
		{
			totalRecords = coolManager->countAllCOOLCategoryPretexts();
			coolCategoryPretext = coolManager->firstCOOLCategoryPretext();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  totalRecords = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager.cool", msgDebug2);
			}
		}
	}

	if (coolCategoryPretext)
	{
		if (coolCategoryPretext->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(COOLDev, COOLDevWriteCategoryPretext);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(coolCategoryPretext);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete coolCategoryPretext;
		coolCategoryPretext = coolManager->nextCOOLCategoryPretext();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGICOOL, BRFICOOLCategoryPretext);
}


void BackupRestoreManager::backupCOOLTotals(bool forUltima)
{
	if ( startingNewFile )
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if ( coolTotal_.notNull() )
			coolTotal_ = 0;
		if ( coolTotalManager )
		{
			totalRecords = coolTotalManager->initializeAllCOOLTotalsQuery();
			coolTotal_ = coolTotalManager->getFirstCOOLTotal();
		}
	}

	if ( coolTotal_.notNull() )
	{
		if ( coolTotal_->isRecordValid() )
		{
			SessionMessage msg(COOLTotalDev, DtWrite);
			msg.setFormatType(EvolutionTaggedFormat);
			if ( forUltima )
			{
				coolTotal_->setFormatType(UltimaTaggedFormat);
				msg.setEventType(WriteAllCOOLTotalsForUltima);
			}
			msg.setHfcObject(coolTotal_);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		coolTotal_ = coolTotalManager->getNextCOOLTotal();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
//	if (displayStats)
//		updateUI(BRGICOOL, BRFICOOLTotal);
}


void BackupRestoreManager::backupSoftkeyGroups()
{
	HobString msgDebug = "";
	if (isTracing)
		msgDebug = "backupSoftkeyGroups():  ";

	if (startingNewFile)
	{
		if (isTracing)
			Debug::trace("BackupRestoreManager", msgDebug + "Entering for the first time.");

		startingNewFile = false;
		currentRecordCount = 0;
		if (softkeyGroup)
		{
			delete softkeyGroup;
			softkeyGroup = 0;
		}
		if (softkeyManager)
		{
			totalRecords = softkeyGroupManager->countAllSoftkeyGroups();
			softkeyGroup = softkeyGroupManager->firstSoftkeyGroup();
			if (isTracing)	
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("total SoftkeyGroup Records = ").append(Integer::toString(totalRecords));
				Debug::trace("BackupRestoreManager", msgDebug2);
			}
		}
	}

	if (softkeyGroup)
	{
		if (softkeyGroup->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(SoftkeyDev, WriteSoftkeyGroup);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}
			msg.setHfcObject(softkeyGroup);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete softkeyGroup;
		softkeyGroup = softkeyGroupManager->nextSoftkeyGroup();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISoftkey, BRFISoftkeyGroup);
}


void BackupRestoreManager::backupSoftkeyStyles()
{
	if (startingNewFile)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "backupSoftkeyStyles():  Entering for the first time.");
		startingNewFile = false;
		currentRecordCount = 0;
		if (softkeyStyle)
		{
			delete softkeyStyle;
			softkeyStyle = 0;
		}
		if (softkeyManager)
		{
			totalRecords = softkeyStyleManager->countAllSoftkeyStyles();
			softkeyStyle = softkeyStyleManager->firstSoftkeyStyle();
		}
	}

	if (softkeyStyle)
	{
		if (softkeyStyle->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(SoftkeyDev, WriteSoftkeyStyle);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(softkeyStyle);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete softkeyStyle;
		softkeyStyle = softkeyStyleManager->nextSoftkeyStyle();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISoftkey, BRFISoftkeyStyle);
}

void BackupRestoreManager::backupSoftkeyFeatures()
{
	if (startingNewFile)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "backupSoftkeyFeatures():  Entering for the first time.");
		startingNewFile = false;
		currentRecordCount = 0;
		if (softkeyFeature)
		{
			delete softkeyFeature;
			softkeyFeature = 0;
		}
		if (softkeyManager)
		{
			totalRecords = softkeyFeatureManager->countAllSoftkeyFeatures();
			softkeyFeature = softkeyFeatureManager->firstSoftkeyFeature();
		}
	}


	if (softkeyFeature)
	{
		if (softkeyFeature->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(SoftkeyDev, WriteSoftkeyFeature);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(softkeyFeature);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete softkeyFeature;
		softkeyFeature = softkeyFeatureManager->nextSoftkeyFeature();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISoftkey, BRFISoftkeyFeature);
}

void BackupRestoreManager::backupSoftkeys()
{
	if (startingNewFile)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "entering backupSoftkeys for the first time");
		startingNewFile = false;
		currentRecordCount = 0;
		if (softkey)
		{
			delete softkey;
			softkey = 0;
		}
		if (softkeyManager)
		{
			totalRecords = softkeyManager->countAllSoftkeys();
			softkey = softkeyManager->firstSoftkey();
		}
	}

	if (softkey)
	{
		if (softkey->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(SoftkeyDev, WriteSoftkey);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.

			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}


			msg.setHfcObject(softkey);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete softkey;
		softkey = softkeyManager->nextSoftkey();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISoftkey, BRFISoftkey);
}

void BackupRestoreManager::backupExpirationLabels()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (expirationLabel)
		{
			delete expirationLabel;
			expirationLabel = 0;
		}
		if (expirationLabelManager)
		{
			totalRecords = expirationLabelManager->countAllExpirationLabels();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupExpirationLabels() - number of records to backup is: ") + HobString(Long::toString(totalRecords)));
			expirationLabel = expirationLabelManager->firstExpirationLabel();
		}
	}

	if (expirationLabel)
	{
		if (expirationLabel->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupExpirationLabels() - backup record for pluNumber: ") + HobString(Long::toString(expirationLabel->getPluNumber())));

			SessionMessage msg(ProductDev, ProductWriteExpirationLabel);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.

			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(expirationLabel);
			backupARecord(&msg);
		}
		else
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupExpirationLabels() - Record is not valid - pluNumber: ") + HobString(Long::toString(expirationLabel->getPluNumber())));
			totalRecordErrors++;
		}
		delete expirationLabel;
		expirationLabel = expirationLabelManager->nextExpirationLabel();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupExpirationLabels() - expirationLabel is NULL: "));
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIExpirationLabels);
}


void BackupRestoreManager::backupScales()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (scale)
		{
			delete scale;
			scale = 0;
		}
		if (getScaleManager())
		{
			totalRecords = 0; //getScaleManager()->countAllScales();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupScales() - number of records to backup is: ") + HobString(Long::toString(totalRecords)));
			scale = getScaleManager()->getFirstScale();
		}
	}

	if (scale)
	{
		if (scale->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupScales() - backup record for scaleId: ") + HobString(Long::toString(scale->getIdentifier())));

			SessionMessage msg(ScaleDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.

			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(scale);
			backupARecord(&msg);
		}
		else
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupScales() - Record is not valid - scaleId: ") + HobString(Long::toString(scale->getIdentifier())));
			totalRecordErrors++;
		}
		delete scale;
		scale = getScaleManager()->getNextScale();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupScales() - scale is NULL: "));
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	//if (displayStats)
	//	updateUI(BRGIPlu, BRFIExpirationLabels);
}


void BackupRestoreManager::backupBatches()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (batch)
		{
			delete batch;
			batch = 0;
		}
		if (getBatchManager())
		{
			totalRecords = 0; //getScaleManager()->countAllBatches();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupBatches() - number of records to backup is: ") + HobString(Long::toString(totalRecords)));
			batch = getBatchManager()->getFirstBatch();
		}
	}

	if (batch)
	{
		if (batch->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupBatches() - backup record for batch number: ") + HobString(Long::toString(batch->getNumber())));

			SessionMessage msg(BatchDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(batch);
			backupARecord(&msg);
		}
		else
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupBatches() - Record is not valid - batch number: ") + HobString(Long::toString(batch->getNumber())));
			totalRecordErrors++;
		}
		delete batch;
		batch = getBatchManager()->getNextBatch();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupBatches() - batch is NULL: "));
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	//if (displayStats)
	//	updateUI(BRGIPlu, BRFIExpirationLabels);
}


void BackupRestoreManager::backupSynchronizationChanges()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (synchChange)
		{
			delete synchChange;
			synchChange = 0;
		}
		if (getSynchronizationChangeManager())
		{
			totalRecords = 0; //getScaleManager()->countAllBatches();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupSynchronizationChanges() - number of records to backup is: ") + HobString(Long::toString(totalRecords)));
			synchChange = getSynchronizationChangeManager()->getFirstChange();
		}
	}

	if (synchChange)
	{
		if (synchChange->isRecordValid())
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupSynchronizationChanges() - backup record for synch change Id: ") + HobString(Long::toString(synchChange->getIdentifier())));

			SessionMessage msg(SynchronizationChangeDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(synchChange);
			backupARecord(&msg);
		}
		else
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", HobString("backupSynchronizationChanges() - Record is not valid - synch change Id: ") + HobString(Long::toString(synchChange->getIdentifier())));
			totalRecordErrors++;
		}
		delete synchChange;
		synchChange = getSynchronizationChangeManager()->getNextChange();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", HobString("backupSynchronizationChanges() - scale is NULL: "));
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	//if (displayStats)
	//	updateUI(BRGIPlu, BRFIExpirationLabels);
}


void BackupRestoreManager::backupSynchronizationConfig()
{
	SessionMessage msg(ConfigurationDev, CfgSynchronizationWrite);

	switch (format)
	{
		case BRVersionIndependent:
			msg.setFormatType(DBIndependentFormatFormat);
			break;
		default:
			msg.setFormatType(EvolutionTaggedFormat);
			break;
	}
	CfgSynchronization myConfig;
	msg.setHfcObject(&myConfig);
	backupARecord(&msg);

	currentRecordCount = 1;
	totalRecords = 1;
	progressTotal++;

	startingNewFile = true;
	doneWithSubState = true;

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGISchronizeConfig, BRFISynchronizeConfigData);
}


void BackupRestoreManager::backupRecalls()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (recall)
		{
			delete recall;
			recall = 0;
		}
		if (recallManager)
		{
			totalRecords = recallManager->countAllRecalls();
			recall = recallManager->firstRecall();
		}
	}

	if (recall)
	{
		if (recall->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(RecallDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(recall);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete recall;
		recall = recallManager->nextRecall();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIRecall);
}


void BackupRestoreManager::backupDevices()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (device)
		{
			delete device;
			device = 0;
		}
		if (deviceManager)
		{
			totalRecords = deviceManager->countAllDevices();
			device = deviceManager->firstDevice();
		}
	}

	if (device)
	{
		if (device->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(DeviceDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(device);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete device;
		device = deviceManager->nextDevice();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIDevice);
}

void BackupRestoreManager::backupProductTextCfgs()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (productTextCfg)
		{
			delete productTextCfg;
			productTextCfg = 0;
		}
		if (productTextCfgManager)
		{
			totalRecords = productTextCfgManager->countAllProductTextCfgs();
			productTextCfg = productTextCfgManager->firstProductTextCfg();
		}
	}

	if (productTextCfg)
	{
		if (productTextCfg->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(ProductTextCfgDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(productTextCfg);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete productTextCfg;
		productTextCfg = productTextCfgManager->nextProductTextCfg();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIScaleConfig, BRFIProductTextCfg);
}


void BackupRestoreManager::backupBarcodeScanners()
{
	if ( startingNewFile )
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (hfcScannerProductRead)
		{
			delete hfcScannerProductRead;
			hfcScannerProductRead = 0;
		}
		if (barcodeScannerManager)
		{
			totalRecords = barcodeScannerManager->countAllBarcodeScanners();
			hfcScannerProductRead = barcodeScannerManager->firstBarcodeScanner();
		}
	}

	if (hfcScannerProductRead)
	{
		if (hfcScannerProductRead->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(BarcodeScannerDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
				case BRVersionIndependent:
					msg.setFormatType(DBIndependentFormatFormat);
					break;
				case BRUltima:
					msg.setFormatType(UltimaTaggedFormat);
					break;
				default:
					msg.setFormatType(EvolutionTaggedFormat);
					break;
			}

			msg.setHfcObject(hfcScannerProductRead);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete hfcScannerProductRead;
		hfcScannerProductRead = barcodeScannerManager->nextBarcodeScanner();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIBarcodeScanner);
}

void BackupRestoreManager::backupDepts()
{
	if (startingNewFile)
	{
		startingNewFile = false;
		currentRecordCount = 0;
		if (dept)
		{
			delete dept;
			dept = 0;
		}
		if (deptManager)
		{
			totalRecords = deptManager->countAllDepts();
			dept = deptManager->firstDept();
		}
	}

	if (dept)
	{
		if (dept->isRecordValid())			//if the record is not valid, don't send it
		{
			SessionMessage msg(DeptDev, DtWrite);
			// BRVersionIndependent format writes the message in EvolutionTaggedFormat, with just a different serialize format for the object.
			switch (format)
			{
			case BRVersionIndependent:
				msg.setFormatType(DBIndependentFormatFormat);
				break;
			default:
				msg.setFormatType(EvolutionTaggedFormat);
				break;
			}

			msg.setHfcObject(dept);
			backupARecord(&msg);
		}
		else
			totalRecordErrors++;

		delete dept;
		dept = deptManager->nextDept();
		currentRecordCount++;
		progressTotal++;
	}
	else
	{
		startingNewFile = true;
		doneWithSubState = true;
	}

	//update the UI with the current task progress
	if (displayStats)
		updateUI(BRGIPlu, BRFIDepts);

}

void BackupRestoreManager::backupUltimaHeader()
{
	// todo - Note:  This does not consider backing up to multiple disk files.
	SessionMessage msg(UltimaDiskDev, HeaderWrite);
	msg.setFormatType(UltimaTaggedFormat);
	GenericHfcObject gho("01");
	msg.setHfcObject(&gho);
	backupARecord(&msg);

	startingNewFile = true;
	doneWithSubState = true;
}

void BackupRestoreManager::backupUltimaFooter()
{
	// todo Note:  This does not consider backing up to multiple disk files.
	//        It only writes the footer as if we backed up to one file.
	SessionMessage msg(UltimaDiskDev, FooterWrite);
	msg.setFormatType(UltimaTaggedFormat);
	HobString h1 = Integer::toString(progressTotal, 6);
	GenericHfcObject gho(h1 + h1 + HobString("99"));
	msg.setHfcObject(&gho);
	backupARecord(&msg);

	startingNewFile = true;
	doneWithSubState = true;
}


void BackupRestoreManager::handleRestore()
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleRestore():  Entering with subState " + inttostring(subState));

	if (subState == BRIdleSubState)
	{
		mode = selections.getMode();				//The mode will tell us how to handle the backup or restore
		// Don't need to get the format on a restore, we auto-determine the format from the data read from the file
		//format = selections.getFormat();
		fileName = selections.getFileName();	//Get the name of the backup file to create

		// open file for restore
		// We do this before calling clearSelections() so that we can determine if the file exists, and if
		// we can identify the format of the data within.  If we can't, then there is no sense in deleting 
		// the existing data, since no data will be brought in and we would be left with an "empty" database.

		// As it exists today, 3/16/05, we are not checking that we have at least one record to restore before
		// calling clearSelections().  Again, if we wouldn't bring in any data, (say, the user selected a valid
		// file, but does not contain the records which they are trying to restore, i.e. they selected the
		// wrong file by mistake), then we probably shouldn't delete the existing data.
		// For the future, we could also delay the call to clear selections() until we determine we have the
		// first record to be restored.

		if (!fpIn && fileName.size())
		{
			fpIn = new CMFile(fileName.c_str(), CommMechanism::MODE_INPUT);
			if (fpIn)
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "Successfully opened restore file " + fileName);
				if (selections.getFormat() == BRVersionIndependent)
					format = BRVersionIndependent;
				if ( format == BRVersionIndependent)
					fpIn->setMyReaderWriter(new CMReaderWriterDBIndependent());
				else
					fpIn->determineReaderWriter();
				if (!fpIn->getMyReaderWriter())
				{
					// if we haven't determined the readerwriter for the data file,
					// i.e. we can't identify the data format, then skip the whole thing and indicate we are done.
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "Could not identify the data format in the restore file " + fileName);
					subState = BRDoneSubState;
					state = BRDoneState;
				}
				else
				{
					// We know we have the file and have identified the data format.
					// Now we can assume we can go ahead and delete the current data if we're doing a restore.
					//fpIn->setMessageHandler(this);

					// don't actually tell the Messenger to start reading from this file yet;
					// the call to getMessenger().addCommMechanism(fpIn) will happen after the
					// existing data has been deleted.
				}
			}
			else
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "Could not open restore file " + fileName);
				subState = BRDoneSubState;
				state = BRDoneState;
			}
		}

		if (state != BRDoneState)
		{
			if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone )
			{
				//Use Case 2.28: Processing user file
				//  3.  Execute �Use Case 5.40: Creating batch�, passing �local� and �user batch� parameters,
				//      and receiving the new batch identifier.
				resetBatchId();
				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
				if ( proxy )
					 proxy->createBatch(&currentBatchId, hfc::Batch::OriginLocal, hfc::Batch::TypeUser);

/*				if (!isBatching())
					;  // uh, what should I do??  Terminate the restore/merge?? */
			}

			if (mode == BROverWrite)
				//clear database of all selected hfc objects before starting
				clearSelections();
			else
				subState = BRAfterCompressingDB;
		}
	}

	// NOTE:  this cannot be an 'else if' conditional
	if (subState == BRAfterCompressingDB)
	{
		subState = BRRestoreSubState;

		// Now tell my Messenger to start reading records from the input file.
		if (fpIn)
			myBackupID_in = getMessenger().addCommMechanism(fpIn);
		else
		{
			subState = BRDoneSubState;
			state = BRDoneState;
		}
	}
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "Leaving handleRestore with subState " + inttostring(subState));
}

void BackupRestoreManager::handleProcessFile()
{
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleProcessFile():  Entering with subState " + inttostring(subState));

	if (subState == BRIdleSubState)
	{
//		mode = selections.getMode();				//The mode will tell us how to handle the backup or restore
		// Don't need to get the format on a restore, we auto-determine the format from the data read from the file
		//format = selections.getFormat();
		fileName = selections.getFileName();					//	Get the name of the file we are reading from
		HobString outFileName = selections.getAltFileName();	//	Get the name of the file we are writing to, potentially.

		if (isTracing)	
			Debug::trace("BackupRestoreManager", "handleProcessFile():  Input file is [" + fileName + "],  Output file is [" + outFileName + "]");

		// open file for restore
		// We do this before calling clearSelections() so that we can determine if the file exists, and if
		// we can identify the format of the data within.  If we can't, then there is no sense in deleting 
		// the existing data, since no data will be brought in and we would be left with an "empty" database.

		// As it exists today, 3/16/05, we are not checking that we have at least one record to restore before
		// calling clearSelections().  Again, if we wouldn't bring in any data, (say, the user selected a valid
		// file, but does not contain the records which they are trying to restore, i.e. they selected the
		// wrong file by mistake), then we probably shouldn't delete the existing data.
		// For the future, we could also delay the call to clear selections() until we determine we have the
		// first record to be restored.

		if (!fpIn && fileName.size())
		{
			fpIn = new CMFile(fileName.c_str(), CommMechanism::MODE_INPUT);
			if (fpIn)
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "CMReaderWriterEvolTag():  Successfully opened file " + fileName);
				if (selections.getFormat() == BRVersionIndependent)
					format = BRVersionIndependent;
				if ( format == BRVersionIndependent)
					fpIn->setMyReaderWriter(new CMReaderWriterDBIndependent());
				else
					fpIn->determineReaderWriter();
				if (!fpIn->getMyReaderWriter())
				{
					// if we haven't determined the readerwriter for the data file,
					// i.e. we can't identify the data format, then skip the whole thing and indicate we are done.
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "handleProcessFile():  Could not identify the data format in the downloaded file " + fileName);
					subState = BRDoneSubState;
					state = BRDoneState;
				}
				else
				{
					// We know we have the file and have identified the data format.
					// Now we can assume we can go ahead and delete the current data if we're doing a restore.
					//fpIn->setMessageHandler(this);

					// don't actually tell the Messenger to start reading from this file yet;
					// the call to getMessenger().addCommMechanism(fpIn) will happen after the
					// existing data has been deleted.
				}
			}
			else
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleProcessFile():  Could not open the file: " + fileName);
				subState = BRDoneSubState;
				state = BRDoneState;
			}
		}

		if (subState != BRDoneState)
		{
	//		subState = BRRestoreSubState;

			selections.setEveryDataTypeSelected(true);

			// Now tell my Messenger to start reading records from the input file.
			if (fpIn)
			{
				myBackupID_in = getMessenger().addCommMechanism(fpIn);
				if (fpOut)
					myBackupID_out = getMessenger().addCommMechanism(fpOut);
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleProcessFile():  myBackupID_in = " + inttostring(myBackupID_in) + ",  myBackupID_out = " + inttostring(myBackupID_out));

				subState = BRProcessFileSubState;
			}
			else
			{
				subState = BRDoneSubState;
				state = BRDoneState;
			}
		}
	}
	else
	{
		if ( subStateList->empty() )
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleProcessFile():  subStateList is empty. We are done.");
			subState = BRDoneSubState;
			state = BRDoneState;
		}
		else
		{
			subState = subStateList->front();			//	Get subState off of list to check it out.
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleProcessFile():  state = " + inttostring(state) + ".  Next to process:  subState = " + inttostring(subState) + ",  (state == BRBackupState) = " + booltostring(state == BRBackupState));
			if (subState == BRDoneSubState)
			{
				state = BRDoneState;
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleProcessFile():  subState = BRDoneSubState = " + inttostring(state) + ". We are done.");
			}
			else
			{
				state = BRBackupState;
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleProcessFile():  state = BRBackupState = " + inttostring(state));
			}
		}
	}
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "handleProcessFile():  Leaving with subState " + inttostring(subState));
}


void BackupRestoreManager::clearSelections()
{
	bool dbCompressionNeeded = false;

	if (selections.isScaleConfigSelected())
	{
		if (productDefaultManager)
			productDefaultManager->handleDeleteAll();

		if (emailAlertsManager)
			emailAlertsManager->handleDeleteAll();
	}

	// Should we delete all the operators, if there are none to import???
	if (selections.isOperatorDataSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingOperators, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (operatorManager)
			operatorManager->handleDeleteAll();

		if (operatorNoteManager)
			operatorNoteManager->handleDelete();

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingOperators, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	// Should we delete all the custom graphics and label types, if there are none to import???
	if (selections.isResourcesSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingResources, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (labelGraphicManager)
			labelGraphicManager->handleDeleteAll();

		if (labelTypeManager)
			labelTypeManager->handleDeleteAll();

		if (genericLabelTextManager)
		{
			genericLabelTextUpdated = true;
			genericLabelTextManager->handleDeleteAll();
		}

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingResources, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	if (selections.isSelfServeDataSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingSelfServeData, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (productManager)
			productManager->handleDeleteAllProductStatistic();

		if (presentationMethodManager)
			presentationMethodManager->handleDeleteAll();

		if (streamingGraphicManager)
			streamingGraphicManager->handleDeleteAll();

//zzz		if (graphicManager)
//zzz			graphicManager->handleDeleteAll(Graphic::TYPE_STREAMINGGRAPHIC);

		dbCompressionNeeded = true;

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingSelfServeData, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	if (selections.isCOOLDataSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingCOOLData, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (coolManager)
		{
			coolManager->handleDeleteAllCOOLCategories();
			coolManager->handleDeleteAllCOOLCountries();
			coolManager->handleDeleteAllCOOLProductionSteps();
			coolManager->handleDeleteAllCOOLPretexts();
			coolManager->handleDeleteAllCOOLTexts();
			coolManager->handleDeleteAllCOOLShortlists();
			coolManager->handleDeleteAllCOOLListTexts();
		}

		dbCompressionNeeded = true;

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingCOOLData, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	if (selections.isSoftkeyDataSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingSoftkeyData, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (softkeyGraphicManager)
			softkeyGraphicManager->handleDeleteAll();

		if (softkeyManager)
		{
			softkeyManager->handleDeleteAll();
			softkeyFeatureManager->handleDeleteAll();
			softkeyStyleManager->handleDeleteAll();
			softkeyGroupManager->handleDeleteAll(false);
		}


		SoftkeyFeature::verifyReservedRecords(this);
		SoftkeyStyle::verifyReservedRecords(this);
		// Postpone this verification to the end of the restore procedure
//		SoftkeyGroup::verifyReservedRecords(this);
		verifySoftkeyGroupReservedRecords = true;
		//  We will only do this in the context of individual missing softkey groups
//		Softkey::verifyReservedRecords(this);

		dbCompressionNeeded = true;

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingSoftkeyData, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	if (selections.isPluDataSelected())
	{
		updateUIDialog(DISPLAY_DIALOG, ErrBackupRestore, BackupRestoreDeletingProductData, HfcResponse::RESPONSESTYLE_NOINPUT);

		if (classManager)
			classManager->handleDeleteAll();

		if (deptManager)
			deptManager->handleDeleteAll();

		if (expandedTextManager)
			expandedTextManager->handleDeleteAll();

		if (specialMessageManager)
			specialMessageManager->handleDeleteAll();

		if (nutrifactsManager)
			nutrifactsManager->handleDeleteAll();

		if (productNoteManager)
			productNoteManager->handleDeleteAll();

		if (productText5Manager)
			productText5Manager->handleDeleteAll();

		if (productText6Manager)
			productText6Manager->handleDeleteAll();

		if (productText7Manager)
			productText7Manager->handleDeleteAll();

		if (productText8Manager)
			productText8Manager->handleDeleteAll();

		if (productText9Manager)
			productText9Manager->handleDeleteAll();

		if (marqueeTextManager)
			marqueeTextManager->handleDeleteAll();

		if (categoryManager)
			categoryManager->handleDeleteAll();

		if (specialsManager)
			specialsManager->handleDeleteAll();

		if (categoryGraphicManager)
			categoryGraphicManager->handleDeleteAll();
		if (productGraphicManager)
			productGraphicManager->handleDeleteAll();
		if (specialsGraphicManager)
			specialsGraphicManager->handleDeleteAll();
        // SOF-4663 Self-Serve: RTD4 records have data that shouldn't exist and are not deleted with Restore
		// Delete scanner codes on a delete
		if (barcodeScannerManager)
			barcodeScannerManager->handleDeleteAll();

		if (transactionTypeManager)
			transactionTypeManager->handleDeleteAll();

		if (productManager)
			productManager->handleDeleteAll();

		dbCompressionNeeded = true;

		updateUIDialog(REMOVE_DIALOG, ErrBackupRestore, BackupRestoreDeletingProductData, HfcResponse::RESPONSESTYLE_NOINPUT);
	}

	dbCompressionNeeded = false;    // No longer need to force a db compression, the deletes now clear the file.
	if (dbCompressionNeeded)
		requestDBCompression();
	else
		subState = BRAfterCompressingDB;
}

void BackupRestoreManager::requestDBCompression()
{
	subState = BRCompressingDB;

	registerEvent(SessionDev, SnDBCompressDone);

	SessionMessage compressDB(SessionDev, SnRequestDBCompress);
	compressDB.setFormatType(InternalFormat);
	send(&compressDB, Messenger::DEST_RTC);
}


void BackupRestoreManager::handleMessage(SessionMessage *msg)
{
//Windux IR#73 -> debug when on crashing application when restoring databases
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "Handling message device: " + inttostring(msg->getDeviceType()) +
														"  event: " + inttostring(msg->getEventType()));

	switch (msg->getDeviceType())
	{
		case ProductDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ProductDev)");
			UINT8 ev = msg->getEventType();
			if ((ev == ProductRestoreExpirationLabel && selections.isEveryDataTypeSelected()) ||
				 (ev != ProductRestoreExpirationLabel && selections.isPluDataSelected()))
			{
				if (!productManager)
				{
					productManager = new ProductManager();
					if (productManager)
						productManager->setDBAccess(this);
				}
				if (productManager)
				{
					try
					{
						productManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ProductDev ********** database exception **********  event=") + HobString(Integer::toString((int)ev)));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, ((ev == ProductRestoreExpirationLabel) ? BRFIExpirationLabels : BRFIProducts));
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, ((ev == ProductRestoreExpirationLabel) ? BRFIExpirationLabels : BRFIProducts), 0);
			}
			break;
		}

		case ProductDefaultsDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ProductDefaultsDev)");
			if (selections.isScaleConfigSelected())
			{
				if (!productManager)
				{
					productManager = new ProductManager();
					if (productManager)
						productManager->setDBAccess(this);
				}
				if (productManager)
				{
					try
					{
						productManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ProductDefaultsDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIProductDefaults);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIProductDefaults, 0);
			}
			break;
		}

		case ClassDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ClassDev)");
			if (selections.isPluDataSelected())
			{
				if (!classManager)
				{
					classManager = new ClassManager();
					if (classManager)
						classManager->setDBAccess(this);
				}
				if (classManager)
				{
					try
					{
						classManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ClassDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFIClasses);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFIClasses, 0);
			}
			break;
		}

		case DeptDev:
		{
			if (isTracing)
				Debug::trace("BackupRestoreManager", "handleMessage(DeptDev)");
			if (selections.isPluDataSelected())
			{
				if (!deptManager)
				{
					deptManager = new DeptManager();
					if (deptManager)
						deptManager->setDBAccess(this);
				}
				if (deptManager)
				{
					try
					{
						deptManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() DeptDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFIDepts);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFIDepts, 0);
			}
			break;
		}

		case NutrifactsDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(NutrifactsDev)");
			if(selections.isPluDataSelected())
			{
				if (!nutrifactsManager)
				{
					nutrifactsManager = new NutrifactsManager();
					if (nutrifactsManager)
						nutrifactsManager->setDBAccess(this);
				}
				if (nutrifactsManager)
				{
					try
					{
						nutrifactsManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() NutrifactsDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFINutriFacts);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFINutriFacts, 0);
			}
			break;
		}

		case ExpandedTextDev:
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ExpandedTextDev)");
			handleProductTextMessage(msg, expandedTextManager, psttExpandedText, BRFIExpandedText);
			break;

		case SpecialMessageDev:
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(SpecialMessageDev)");
			handleProductTextMessage(msg, specialMessageManager, psttSpecialMessage, BRFISpecialMessages);
			break;

		case ProductNotesDev:
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ProductNotesDev)");
			handleProductTextMessage(msg, productNoteManager, psttProductNote, BRFIProductNotes);
			break;

		case MarqueeTextDev:
			handleProductTextMessage(msg, marqueeTextManager, psttMarqueeText, BRFIMarqueeText);
			break;

		case ProductText5Dev:
			handleProductTextMessage(msg, productText5Manager, psttProductText5, BRFIProductText5);
			break;

		case ProductText6Dev:
			handleProductTextMessage(msg, productText6Manager, psttProductText6, BRFIProductText6);
			break;

		case ProductText7Dev:
			handleProductTextMessage(msg, productText7Manager, psttProductText7, BRFIProductText7);
			break;

		case ProductText8Dev:
			handleProductTextMessage(msg, productText8Manager, psttProductText8, BRFIProductText8);
			break;

		case ProductText9Dev:
			handleProductTextMessage(msg, productText9Manager, psttProductText9, BRFIProductText9);
			break;

		case TransactionTypeDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(TransactionTypeDev)");
			if (selections.isPluDataSelected())
			{
				if ( !transactionTypeManager )
					transactionManager = getTransactionManager();

				if ( transactionTypeManager )
				{
					try
					{
						transactionTypeManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", "BackupRestoreManager::handleMessage() TransactionTypeDev ********** database exception **********");
							Debug::trace("DBAccess.exception", "     DBException: type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFITransactionTypeNames);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFITransactionTypeNames, 0);
			}
			break;
		}


		case OperatorDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(OperatorDev)");
			bool okToProcess = false;
			if (msg->getEventType() < OpLogin)
			{
				if (selections.isOperatorDataSelected())
					okToProcess = true;
			}
			else if (selections.isEveryDataTypeSelected())
				okToProcess = true;

			if (okToProcess)
			{
				if (!operatorManager)
				{
					operatorManager = new OperatorManager();
					if (operatorManager)
						operatorManager->setDBAccess(this);
				}
				if (operatorManager)
				{
					try
					{
						operatorManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() OperatorDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIOperatorData, BRFIOpData);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIOperatorData, BRFIOpData, 0);
			}
			break;
		}


		case ConfigurationDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ConfigurationDev):   EventType = " + Integer::toString(msg->getEventType()));
			if ( (msg->getEventType() == CfgStoreInfoRestore && selections.isStoreDataSelected()) ||
				 (msg->getEventType() == CfgSynchronizationRestore && selections.isSynchronizeConfigSelected()) ||
				 (msg->getEventType() == DtRestore && selections.isScaleConfigSelected()) ||
				 msg->getEventType() == CfgDeleteAllFiles )
			{
				if (!configurationManager)
				{
					configurationManager = new ConfigurationManager();
					if (configurationManager)
						configurationManager->setDBAccess(this);
				}
				if (configurationManager)
				{
					try
					{
						if (isTracing)	
							Debug::trace("BackupRestoreManager", "handleMessage(ConfigurationDev, " + Integer::toString(msg->getEventType()) + "):  configurationManager->handleMessage(msg)");
						configurationManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ConfigurationDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIConfigData);
				}
			}
			else
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleMessage(ConfigurationDev, " + Integer::toString(msg->getEventType()) + "):  IGNORING the message.");
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIConfigData, 0);
			}
			break;
		}


		case GraphicDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(GraphicDev)");
//			Debug::trace("BackupRestoreManager.Graphics", HobString("handleMessage(): GraphicDev   eventType = ") + HobString(Integer::toString(msg->getEventType())));
			BRFileIndicator whichGraphic;
			BRGroupIndicator whichGroup = BRGIPlu;	//	Default for Product, Category, Specials & Streaming graphics

			switch (msg->getEventType())
			{
				case RestoreProductGraphic:
					whichGraphic = BRFIProductGraphics;
					break;
				case RestoreCategoryGraphic:
					whichGraphic = BRFICategoryGraphics;
					break;
				case RestoreSpecialsGraphic:
					whichGraphic = BRFISpecialsGraphics;
					break;
				case RestoreStreamingGraphic:
					whichGraphic = BRFIStreamingGraphicsGraphics;
					break;
				case RestoreSoftkeyGraphic:
					whichGraphic = BRFISoftkeyGraphics;
					break;
				case DtRestore:
				default:
				{
					writeUserTar = true;
					whichGraphic = BRFIUserLabelGraphics;
					whichGroup = BRGIResources;
					break;
				}
			}

			if ((selections.isPluDataSelected()  && (msg->getEventType() == RestoreProductGraphic || msg->getEventType() == RestoreCategoryGraphic || msg->getEventType() == RestoreSpecialsGraphic)) ||
				 (selections.isResourcesSelected() && msg->getEventType() == DtRestore) ||
				 (selections.isSelfServeDataSelected() && msg->getEventType() == RestoreStreamingGraphic) ||
				 (selections.isSoftkeyDataSelected() && msg->getEventType() == RestoreSoftkeyGraphic))
			{
				if (graphicManager)
				{
					graphicManager->handleGraphicMessage(msg);

// todo - How can I tell if the save to the database failed.  I would probably want to update the
//        error counter if there was something wrong with the record.
					currentRecordCount++;
					progressTotal++;
					if (displayStats)
						updateUI(BRGIResources, whichGraphic);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIResources, whichGraphic, 0);
			}
			break;
		}

		case LabelTypeDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(LabelTypeDev)");
			if (selections.isResourcesSelected())
			{
				if (!labelTypeManager)
				{
					labelTypeManager = new LabelTypeManager();
					if (labelTypeManager)
						labelTypeManager->setDBAccess(this);
				}
				if (labelTypeManager)
				{
					labelTypeManager->setRecordIgnored(false);
					try
					{
						labelTypeManager->handleMessage(msg);
						if (labelTypeManager->isRecordIgnored())
						{
							++currentRecordsIgnored;
							++progressTotalIgnored;
						}
						else
						{
							currentRecordCount++;
							progressTotal++;
						}
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() LabelTypeDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
					{
						if (labelTypeManager->isRecordIgnored())
							updateUI(BRGIResources, BRFILabelTypes, 0, 1);
						else
							updateUI(BRGIResources, BRFILabelTypes);
					}
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIResources, BRFILabelTypes, 0);
			}
			break;
		}


		case GenericTextDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(GenericTextDev)");
			if (selections.isResourcesSelected())
			{
				if (!genericLabelTextManager)
				{
					genericLabelTextManager = new GenericLabelTextManager();
					if (genericLabelTextManager)
						genericLabelTextManager->setDBAccess(this);
				}
				if (genericLabelTextManager)
				{
					try
					{
						genericLabelTextUpdated = true;
						genericLabelTextManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() GenericTextDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIResources, BRFILabelTextNames);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIResources, BRFILabelTextNames, 0);
			}
			break;
		}

		case TransactionDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(TransactionDev)");
			if ( selections.isEveryDataTypeSelected() )
			{
				if ( !transactionManager )
					transactionManager = getTransactionManager();

				if ( transactionManager )
				{
					try
					{
						transactionManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() TransactionDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
//					if (displayStats)
//						updateUI(BRGIPlu, BRFIProductTotals);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
//				if (displayStats)
//					updateUI(BRGIPlu, BRFIProductTotals, 0);
			}

			break;
		}

		case CategoryDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(CategoryDev)");
			if (selections.isPluDataSelected())
			{
				if (!categoryManager)
				{
					categoryManager = new CategoryManager();
					if (categoryManager)
						categoryManager->setDBAccess(this);
				}
				if (categoryManager)
				{
					try
					{
						categoryManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() CategoryDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGISelfServe, BRFICategories);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGISelfServe, BRFICategories, 0);
			}
			break;
		}

		case PresentationMethodDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(PresentationMethodDev)");
			if (selections.isSelfServeDataSelected())
			{
				if (!presentationMethodManager)
				{
					presentationMethodManager = new PresentationMethodManager();
					if (presentationMethodManager)
						presentationMethodManager->setDBAccess(this);
				}
				if (presentationMethodManager)
				{
					try
					{
						presentationMethodManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() PresentationMethodDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGISelfServe, BRFIPresentationMethods);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGISelfServe, BRFIPresentationMethods, 0);
			}
			break;
		}

		case SpecialsDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(SpecialsDev)");
			if (selections.isPluDataSelected())
			{
				if (!specialsManager)
				{
					specialsManager = new SpecialsManager();
					if (specialsManager)
						specialsManager->setDBAccess(this);
				}
				if (specialsManager)
				{
					try
					{
						specialsManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() SpecialsDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGISelfServe, BRFISpecials);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGISelfServe, BRFISpecials, 0);
			}
			break;
		}

		case StreamingGraphicDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(StreamingGraphicDev)");
			if (selections.isSelfServeDataSelected())
			{
				if (!streamingGraphicManager)
				{
					streamingGraphicManager = new StreamingGraphicManager();
					if (streamingGraphicManager)
						streamingGraphicManager->setDBAccess(this);
				}
				if (streamingGraphicManager)
				{
					try
					{
						streamingGraphicManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() StreamingGraphicDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGISelfServe, BRFIStreamingGraphics);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGISelfServe, BRFIStreamingGraphics, 0);
			}
			break;
		}

		case ProductStatisticsDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ProductStatisticsDev)");
			if (selections.isSelfServeDataSelected())
			{
				if (!productManager)
				{
					productManager = new ProductManager();
					if (productManager)
						productManager->setDBAccess(this);
				}
				if (productManager)
				{
					try
					{
						productManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ProductStatisticsDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFIProductStatistics);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFIProductStatistics, 0);
			}
			break;
		}

		case EmailAlertsDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(EmailAlertsDev)");
			if ((msg->getEventType() == DtRestore) && selections.isScaleConfigSelected())
			{
				if (!emailAlertsManager)
				{
					emailAlertsManager = new EmailAlertsManager();
					if (emailAlertsManager)
						emailAlertsManager->setDBAccess(this);
				}
				if (emailAlertsManager)
				{
					try
					{
						emailAlertsManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() EmailAlertsDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIEmailAlerts);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIEmailAlerts, 0);
			}
			break;
		}

		case COOLDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(COOLDev)");
			BRFileIndicator fileInd;
			switch (msg->getEventType())
			{
				case COOLDevRestoreCategory:
					fileInd = BRFICOOLCategory;
					break;
				case COOLDevRestoreCountry:
					fileInd = BRFICOOLCountry;
					break;
				case COOLDevRestoreProductionStep:
					fileInd = BRFICOOLProductionStep;
					break;
				case COOLDevRestorePretext:
					fileInd = BRFICOOLPretext;
					break;
				case COOLDevRestoreCoolText:
					fileInd = BRFICOOLText;
					break;
				case COOLDevRestoreShortlist:
				case COOLDevRestoreShortlistForUltima:
					fileInd = BRFICOOLShortlist;
					break;
				case COOLDevRestoreCategoryCountry:
					fileInd = BRFICOOLCategoryCountry;
					break;
				case COOLDevRestoreCategoryProductionStep:
					fileInd = BRFICOOLCategoryProductionStep;
					break;
				case COOLDevRestoreCategoryPretext:
					fileInd = BRFICOOLCategoryPretext;
					break;
				default:
					// have to set it to something, and we don't have an 'unknown' defined
					fileInd = (BRFileIndicator)0;
					break;
			}

			if (selections.isCOOLDataSelected())
			{
				if (!coolManager)
				{
					coolManager = new COOLManager();
					if (coolManager)
						coolManager->setDBAccess(this);
				}
				if (coolManager)
				{
					try
					{
						coolManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							HobString msgDebug = "BackupRestoreManager::handleMessage() COOLDev, eventType = ";
							msgDebug.append(Integer::toString(msg->getEventType())).append("  ********** database exception **********");
							Debug::trace("DBAccess.exception", msgDebug);
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGICOOL, fileInd);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGICOOL, fileInd, 0);
			}
			break;
		}

		case SoftkeyDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(SoftkeyDev)");
			BRFileIndicator fileInd = (BRFileIndicator)0;
			if (msg->getEventType() == RestoreSoftkey)
				fileInd = BRFISoftkey;
			else if (msg->getEventType() == RestoreSoftkeyFeature)
				fileInd = BRFISoftkeyFeature;
			else if (msg->getEventType() == RestoreSoftkeyStyle)
				fileInd = BRFISoftkeyStyle;
			else if (msg->getEventType() == RestoreSoftkeyGroup)
				fileInd = BRFISoftkeyGroup;

			if (selections.isSoftkeyDataSelected())
			{
				if (!softkeyManager)
				{
					softkeyManager = new SoftkeyManager();
					if (softkeyManager)
						softkeyManager->setDBAccess(this);
				}
				if (softkeyManager)
				{
					try
					{
						softkeyManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							HobString msgDebug = "BackupRestoreManager::handleMessage() SoftkeyDev, eventType = ";
							msgDebug.append(Integer::toString(msg->getEventType())).append("  ********** database exception **********");
							Debug::trace("DBAccess.exception", msgDebug);
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGISoftkey, fileInd);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGISoftkey, fileInd, 0);
			}
			break;
		}

		case SessionDev:
		{
			UINT8 event = msg->getEventType();
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(SessionDev)");
			if (event == SnDebugTrace)
				dispatchEvent(msg, mySession);
			else if (event == SnExecuteTask || event == SnBackendRemotePrint || event == SnBackendIsScaleAvailable || event == SnBackendRemotePrintTemplateLabel)
			{
				msg->setFormatType(InternalFormat);
		        send(msg, Messenger::DEST_RTC);
			}
			else if ((state == BRProcessFileState) && (subState == BRProcessFileSubState))
			{
				if ((event == SnReadStatistics))
				{
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "handleMessage(SessionDev, SnReadStatistics):  BRProcessFileSubState --> BRScaleStatisticsSubState");
					subStateList->push_back(BRScaleStatisticsSubState);
//					subStateList->push_back(BRDoneSubState);
				}
				else if ((event == SnReadScaleStatus))
				{
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "handleMessage(SessionDev, SnReadScaleStatus):  BRProcessFileSubState --> BRScaleStatusSubState");
					subStateList->push_back(BRScaleStatusSubState);
//					subStateList->push_back(BRDoneSubState);
				}
			}
			break;
		}

		case ScaleDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(ScaleDev)");
			if (selections.getFormat() == BRVersionIndependent && getScaleManager())
			{
				try
				{
					getScaleManager()->handleMessage(msg);
					currentRecordCount++;
					progressTotal++;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ScaleDev ********** database exception **********"));
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
					currentRecordsIgnored++;
					progressTotalIgnored++;
					totalRecordErrors++;
				}
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes);
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes, 0);
			}
			break;
		}

		case BatchDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(BatchDev)");
			if (selections.getFormat() == BRVersionIndependent && getBatchManager())
			{
				try
				{
					getBatchManager()->handleMessage(msg);
					currentRecordCount++;
					progressTotal++;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() BatchDev ********** database exception **********"));
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
					currentRecordsIgnored++;
					progressTotalIgnored++;
					totalRecordErrors++;
				}
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes);
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes, 0);
			}
			break;
		}

		case Wrapper:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(Wrapper)");
			if (selections.isWrapperSettingsSelected())
			{
				if (mySession)
				{
					Debug::trace("SessionWrapper", "Restore: Handling wrapper message.");
					mySession->handleWrapperRestoreMessage(msg);
					currentRecordCount++;
					progressTotal++;
					if (displayStats)
						updateUI(BRGIWrapperSettings, BRFIWrSettings);
				}
			}
			else
			{
				Debug::trace("SessionWrapper", "Restore: Ignoring wrapper message.");
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIWrapperSettings, BRFIWrSettings, 0);
			}
			break;
		}

		case SynchronizationChangeDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(SynchronizationChangeDev)");
			if (selections.getFormat() == BRVersionIndependent && getSynchronizationChangeManager())
			{
				try
				{
					getSynchronizationChangeManager()->handleMessage(msg);
					currentRecordCount++;
					progressTotal++;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() BatchDev ********** database exception **********"));
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
					currentRecordsIgnored++;
					progressTotalIgnored++;
					totalRecordErrors++;
				}
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes);
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				//if (displayStats)
				//	updateUI(BRGIPlu, BRFIProductNotes, 0);
			}
			break;
		}

		case RecallDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(RecallDev)");
			if ( 1 /*selections.isPluDataSelected()*/)
			{
				if (!recallManager)
				{
					recallManager = new RecallManager();
					if (recallManager)
						recallManager->setDBAccess(this);
				}
				if (recallManager)
				{
					try
					{
						recallManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() RecallDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIRecall);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIRecall, 0);
			}
			break;
		}

		case DeviceDev:
		{
			if ( selections.isScaleConfigSelected() )
			{
				if (!deviceManager)
				{
					deviceManager = new DeviceManager();
					if (deviceManager)
						deviceManager->setDBAccess(this);
				}
				if (deviceManager)
				{
					try
					{
						deviceManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() DeviceDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIDevice);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIDevice, 0);
			}
			break;
		}

		case ProductTextCfgDev:
		{
			if ( selections.isScaleConfigSelected() )
			{
				if (!productTextCfgManager)
				{
					productTextCfgManager = new ProductTextCfgManager();
					if (productTextCfgManager)
						productTextCfgManager->setDBAccess(this);
				}
				if (productTextCfgManager)
				{
					try
					{
						productTextCfgManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ProductTextCfgDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIProductTextCfg);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIScaleConfig, BRFIProductTextCfg, 0);
			}
			break;
		}
		
		case BarcodeScannerDev:
		{
            // SOF-4663 Self-Serve: RTD4 records have data that shouldn't exist and are not deleted with Restore
		    // Working on 4663, saw this
			if( selections.isPluDataSelected() )
			{
				if (!barcodeScannerManager)
				{
					barcodeScannerManager = new BarcodeScannerManager();
					if ( barcodeScannerManager )
						barcodeScannerManager->setDBAccess(this);
				}
				if ( barcodeScannerManager )
				{
					try
					{
						barcodeScannerManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() BarcodeScannerDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIScaleConfig, BRFIBarcodeScanner);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFIBarcodeScanner, 0);
			}
			break;
		}
		

		case Printer:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(Printer):  subState = " + inttostring(subState));

			if (msg->getEventType() == PrRestorePrinterSettings)
			{
				Debug::trace("BackupRestoreManager.PrinterSettings", "handleMessage(Printer, PrRestorePrinterSettings)");
				PrinterType printerType = MaxPrinterTypes;
				bool processedRecord = false;
				bool waitingForPrConfig = false;
//				if (selections.isPrinterSettingsSelected())
//				{
					if (mySession)
					{
						HfcPrinterConfig *newPrinterSettings = dynamic_cast<HfcPrinterConfig *> (msg->getHfcObject());
						if (!newPrinterSettings)
						{
							Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  Can't get the Printer settings from the SessionMessage! Exiting.");
						}
						else
						{
							//	Which printer is the message for? Is this printer attached?
							printerType = newPrinterSettings->getPrinterType();
							if ((printerType < FirstPrinter) || (printerType >= MaxPrinterTypes))
							{
								Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  Don't have a valid Printer type from the Message! Exiting.");
								MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "A printer number " + Integer::toString(printerType) + " is not a valid printer.");
							}
							else
							{
								HobString printerString = (printerType == FirstPrinter ? "FirstPrinter" : (printerType == SecondPrinter ? "SecondPrinter" : (printerType == ThirdPrinter ? "ThirdPrinter" : "Invalid Printer")));
								if (SessionManager::getInstance()->isPrinterAttached(printerType))
								{
									Session *priSession = SessionManager::getInstance()->getPrimaryConsoleSession();
									if (priSession)
									{
										ConfigurationManager *primaryConfigMgr = priSession->getConfigurationManager();
										if (primaryConfigMgr)
										{
											HfcPrinterConfig *currPrConfig = 0;
											HfcPrinterConfig *tmpPrConfig = primaryConfigMgr->getCurrentPrinterConfig(printerType);
											if (tmpPrConfig)
												currPrConfig = new HfcPrinterConfig(tmpPrConfig);

											if (currPrConfig)
											{
												mySession->getConfigurationManager()->setCurrentPrinterConfig(printerType, currPrConfig);

												mySession->handleMessage(msg);

												PAL_absclass::PAL_sleep(400);	//	Make sure we can get this to the printer before we possibly terminate.
												processedRecord = true;
												Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  Sent message to primary session.");
											}
											else
											{
												if (--_printerCfgRetries_ <= 0)
												{
													Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  No Printer Settings for Printer # " + Integer::toString(printerType));
													MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + ":  Could not retrieve Printer configuration.");
												}
												else
												{
													//	Send a PrReqInquire message
													Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  Requesting Printer Config for Printer # " + Integer::toString(printerType) + ",  Retries = " + Integer::toString(_printerCfgRetries_));
													requestPrinterConfig((PrinterType) printerType);
													PAL_absclass::PAL_sleep( 500 );
													MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + ":  Requesting Printer configuration.");

													waitingForPrConfig = true;
												}
											}
										}
									}
									else
									{
										Debug::trace("BackupRestoreManager.PrinterSettings", "PrRestorePrinterSettings:  Can not find the primary session!");
										MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Unable to change the settings for the " + printerString + ".");
									}
								}
								else
								{
									MessageLogger::logMessage(MessageLogger::ScaleEventsLog, printerString + " is not attached.");
								}
							}
						}
					}
//				}
				if (processedRecord)
				{
					currentRecordCount++;
					progressTotal++;
//	This is currently an internal function - not a typical restore.
//	So for now, we won't update BRGroupIndicator and BRFileIndicator.
//						if (displayStats)
//							updateUI(BRGIPrinterSettings, BRFIPrSettings);
					_printerCfgRetries_ = PRINTER_CONFIG_RETRIES;
					requestPrinterConfig((PrinterType) printerType);	//	Do this to update the Primary session.
				}
				else if (!waitingForPrConfig)
				{
					Debug::trace("BackupRestoreManager", "PrRestorePrinterSettings:  Ignoring printer message.");
					currentRecordsIgnored++;
					progressTotalIgnored++;
//	This is currently an internal function - not a typical restore.
//	So for now, we won't update BRGroupIndicator and BRFileIndicator.
//					if (displayStats)
//						updateUI(BRGIPrinterSettings, BRFIPrSettings, 0);
					_printerCfgRetries_ = PRINTER_CONFIG_RETRIES;
				}
			}
			else
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleMessage(Printer):  I don't understand this message!");
			}
			Debug::trace("BackupRestoreManager", "handleMessage(Printer):  Done.");
			break;
		}

		case Weigher:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(Weigher):  subState = " + inttostring(subState));

			if (msg->getEventType() == WgRestoreWeigherSettings)
			{
				Debug::trace("BackupRestoreManager.WeigherSettings", "handleMessage(Weigher, WgRestoreWeigherSettings)");
				bool processedRecord = false;
				bool waitingForWgConfig = false;
//				if (selections.isWeigherSettingsSelected())
//				{
					if (mySession)
					{
						HfcWeigherConfig *newWeigherSettings = dynamic_cast<HfcWeigherConfig *> (msg->getHfcObject());
						if (!newWeigherSettings)
						{
							Debug::trace("BackupRestoreManager.WeigherSettings", "WgRestoreWeigherSettings:  Can't get the Weigher settings from the SessionMessage! Exiting.");
						}
						else
						{
							if (SessionManager::getInstance()->isWeigherAttached())
							{
								Session *priSession = SessionManager::getInstance()->getPrimaryConsoleSession();
								if (priSession)
								{
									ConfigurationManager *primaryConfigMgr = priSession->getConfigurationManager();
									if (primaryConfigMgr)
									{
										HfcWeigherConfig *currWgConfig = 0;
										HfcWeigherConfig *tmpWgConfig = primaryConfigMgr->getCurrentWeigherConfig();
										if (tmpWgConfig)
											currWgConfig = new HfcWeigherConfig(tmpWgConfig);

										if (currWgConfig)
										{
											mySession->getConfigurationManager()->setCurrentWeigherConfig(currWgConfig);

											mySession->handleMessage(msg);

											PAL_absclass::PAL_sleep(400);	//	Make sure we can get this to the Weigher before we possibly terminate.
											processedRecord = true;
											Debug::trace("BackupRestoreManager.WeigherSettings", "WgRestoreWeigherSettings:  Sent message to primary session.");
										}
										else
										{
											if (--_weigherCfgRetries_ <= 0)
											{
												Debug::trace("BackupRestoreManager.WeigherSettings", "WgRestoreWeigherSettings:  No Weigher Settings for Weigher");
												MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Could not retrieve Weigher configuration.");
											}
											else
											{
												//	Send a WgReqInquire message
												Debug::trace("BackupRestoreManager.WeigherSettings", "WgRestoreWeigherSettings:  Requesting Weigher Config for Weigher,  Retries = " + Integer::toString(_weigherCfgRetries_));
												requestWeigherConfig();
												PAL_absclass::PAL_sleep( 500 );
												MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Requesting Weigher configuration.");

												waitingForWgConfig = true;
											}
										}
									}
								}
								else
								{
									Debug::trace("BackupRestoreManager.WeigherSettings", "WgRestoreWeigherSettings:  Can not find the primary session!");
									MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Unable to change the settings for the Weigher.");
								}
							}
							else
							{
								MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher is not attached.");
							}
						}
					}
//				}
				if (processedRecord)
				{
					currentRecordCount++;
					progressTotal++;
//	This is currently an internal function - not a typical restore.
//	So for now, we won't update BRGroupIndicator and BRFileIndicator.
//						if (displayStats)
//							updateUI(BRGIWeigherSettings, BRFIWgSettings);
					_weigherCfgRetries_ = WEIGHER_CONFIG_RETRIES;
					requestWeigherConfig();  // Do this to update the Primary session.
				}
				else if (!waitingForWgConfig)
				{
					Debug::trace("BackupRestoreManager", "WgRestoreWeigherSettings:  Ignoring weigher message.");
					currentRecordsIgnored++;
					progressTotalIgnored++;
//	This is currently an internal function - not a typical restore.
//	So for now, we won't update BRGroupIndicator and BRFileIndicator.
//					if (displayStats)
//						updateUI(BRGIWeigherSettings, BRFIWgSettings, 0);
					_weigherCfgRetries_ = WEIGHER_CONFIG_RETRIES;
				}
			}
			else
			{
				if (isTracing)	
					Debug::trace("BackupRestoreManager", "handleMessage(Weigher):  I don't understand this message!");
			}
			Debug::trace("BackupRestoreManager", "handleMessage(Weigher):  Done.");
			break;
		}

		case Error:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(Error)");
			switch (msg->getEventType())
			{
				case ErrEOF:
					if (isTracing)	
						Debug::trace("BackupRestoreManager", "Reached end of input file.  state = " + inttostring(state));

					if ((state != BRDoneState) && (state != BRIdleState) && (event != ProcessFile))
						state = BRDoneState;
					if ((event == ProcessFile) && (state != BRProcessFileState))
					{
						if (fpIn)
						{
							getMessenger().removeCommMechanism(fpIn);
							if (isTracing)
								Debug::trace("BackupRestoreManager", "handleMessage(Error):  getMessenger().removeCommMechanism(fpIn)");

							fpIn->cmClose();
							if (isTracing)
								Debug::trace("BackupRestoreManager", "handleMessage(Error):  fpIn->cmClose()");

							delete fpIn;
							fpIn = NULL;
						}
						state = BRProcessFileState;
						subState = BRExecuteProcessFileSubState;
						if (isTracing)	
							Debug::trace("BackupRestoreManager", "handleMessage(Error):   ProcessFile - Changing state from " + inttostring(state) + " to BRProcessFileState.  subState = BRExecuteProcessFileSubState");
					}
					if ((state == BRDoneState) || (state == BRProcessFileState))
//					if (state == BRDoneState)
						noMessageFromMessengerProcessing();
					break;

				default:
					break;
			}
			break;
		}

		case CutTestDev:
		{
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage(CutTestDev)");
			if ( selections.isPluDataSelected() || selections.getFormat() == BRVersionIndependent )
			{
				if (!cutTestManager)
				{
					cutTestManager = new CutTestManager();
					if (cutTestManager)
						cutTestManager->setDBAccess(this);
				}
				if (cutTestManager)
				{
					try
					{
						cutTestManager->handleMessage(msg);
						currentRecordCount++;
						progressTotal++;
					}
					catch (DBException &dbe)
					{
						if (Debug::isTracing("DBAccess.exception"))
						{
							Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() CutTestDev ********** database exception **********"));
							DBException::Type t = dbe.getType();
							Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
						}
						currentRecordsIgnored++;
						progressTotalIgnored++;
						totalRecordErrors++;
					}
					if (displayStats)
						updateUI(BRGIPlu, BRFICutTests);
				}
			}
			else
			{
				currentRecordsIgnored++;
				progressTotalIgnored++;
				if (displayStats)
					updateUI(BRGIPlu, BRFICutTests, 0);
			}
			break;
		}

		default:
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "handleMessage():  No match for DeviceType " + Integer::toString(msg->getDeviceType()));
			break;
	}
	if (isTracing)	
		Debug::trace("BackupRestoreManager", "done handling message device: " + inttostring(msg->getDeviceType()) +
														"  event: " + inttostring(msg->getEventType()));
}

void BackupRestoreManager::handleProductTextMessage(SessionMessage *msg, ProductTextManager *productTextManager, ProdSupportTextType type, BRFileIndicator fileInd)
{
	if (selections.isPluDataSelected())
	{
		if (!productTextManager)
		{
			productTextManager = new ProductTextManager(type);
			if (productTextManager)
				productTextManager->setDBAccess(this);
		}
		if (productTextManager)
		{
			try
			{
				productTextManager->handleMessage(msg);
				currentRecordCount++;
				progressTotal++;
			}
			catch (DBException &dbe)
			{
				if (Debug::isTracing("DBAccess.exception"))
				{
					Debug::trace("DBAccess.exception", HobString("BackupRestoreManager::handleMessage() ProductText ********** database exception **********  type:") + HobString(Integer::toString(type)));
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				currentRecordsIgnored++;
				progressTotalIgnored++;
				totalRecordErrors++;
			}
			if (displayStats)
				updateUI(BRGIPlu, fileInd);
		}
	}
	else
	{
		currentRecordsIgnored++;
		progressTotalIgnored++;
		if (displayStats)
			updateUI(BRGIPlu, fileInd, 0);
	}
}


void BackupRestoreManager::initialBadRecord()
{
	return;
/*
	finishedWithClasses = false;
	finishedWithExpandedText = false;
	finishedWithSpecialMessages = false;
	finishedWithNutrifacts = false;
	finishedWithProductNotes = false;
	finishedWithTransactions = false;
	finishedWithProducts = false;

	//initialize all managers, get the first record and record total for the bad record scan
	//productManager = new ProductManager();
	//productManager->setDBAccess(this);
  	totalTaskRecords += productManager->countAllProducts();

	//classManager = new ClassManager();
	//classManager->setDBAccess(this);
  	totalTaskRecords += classManager->countAllClasses();

  	totalTaskRecords += expandedTextManager->countAllProductTexts();

  	totalTaskRecords += specialMessageManager->countAllProductTexts();

	//nutrifactsManager = new NutrifactsManager();
	//nutrifactsManager->setDBAccess(this);
	totalTaskRecords += nutrifactsManager->countAllNutrifacts();

	totalTaskRecords += productNoteManager->countAllProductTexts();

  	totalTaskRecords += marqueeTextManager->countAllProductTexts();

	//transactionTypeManager = new TransactionTypeManager();
	//transactionTypeManager->setDBAccess(this);
	totalTaskRecords += transactionTypeManager->countAllRecords();

	hobList = new HobList();
	hobListFI = new HobList();
*/
}

void BackupRestoreManager::handleBadRecordScan()
{
	return;
/*
	HobString myString;

	//Scan the database for bad records
	if ( (classManager) && (!finishedWithClasses) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = classManager->countAllClasses();
			classes = classManager->firstClass();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFIClasses);			 //update the user interface with the states progress

		if (classes)
		{
			if (!classes->isRecordValid())
			{
				totalRecordErrors++;

				//push back number and description
				hobListFI->push_back(Long::toString(BRFIClasses));
				myString = Long::toString(classes->getClassNum());
				myString.append(" - ");
				myString.append(classes->getClassName());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete classes;
			classes = classManager->nextClass();
			currentRecordCount++;
		}
		else
		{
			finishedWithClasses = true;
			startingNewFile = true;
		}
	}
	else if ( (expandedTextManager) && (!finishedWithExpandedText) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = expandedTextManager->countAllProductTexts();
			expandedText = expandedTextManager->getFirstRecord();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFIExpandedText);

		if (expandedText)
		{
			if (!expandedText->isRecordValid())
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFIExpandedText));
				myString = Long::toString(expandedText->getTextNum());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete expandedText;
			expandedText = expandedTextManager->getNextRecord();
			currentRecordCount++;
		}
		else
		{
			finishedWithExpandedText = true;
			startingNewFile = true;
		}
	}
	else if ( (specialMessageManager) && (!finishedWithSpecialMessages) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = specialMessageManager->countAllProductTexts();
			specialMessage = specialMessageManager->getFirstRecord();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFISpecialMessages);

		if (specialMessage)
		{
			if (!specialMessage->isRecordValid())
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFISpecialMessages));
				myString = Long::toString(specialMessage->getTextNum());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete specialMessage;
			specialMessage = specialMessageManager->getNextRecord();
			currentRecordCount++;
		}
		else
		{
			finishedWithSpecialMessages = true;
			startingNewFile = true;
		}
	}
	else if ( (nutrifactsManager) && (!finishedWithNutrifacts) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = nutrifactsManager->countAllNutrifacts();
			nutriFact = nutrifactsManager->firstNutrifacts();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFINutriFacts);

		if (nutriFact)
		{
			if (!nutriFact->isRecordValid())
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFINutriFacts));
				myString = Long::toString(nutriFact->getTextNum());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete nutriFact;
			nutriFact = nutrifactsManager->nextNutrifacts();
			currentRecordCount++;
		}
		else
		{
			finishedWithNutrifacts = true;
			startingNewFile = true;
		}
	}
	else if ( (productNoteManager) && (!finishedWithProductNotes) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = productNoteManager->countAllProductTexts();
			productNote = productNoteManager->getFirstRecord();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFIProductNotes);

		if (productNote)
		{
			if (!productNote->isRecordValid())
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFIProductNotes));
				myString = Long::toString(productNote->getTextNum());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete productNote;
			productNote = productNoteManager->getNextRecord();
			currentRecordCount++;
		}
		else
		{
			finishedWithProductNotes = true;
			startingNewFile = true;
		}
	}
	else if ((transactionTypeManager) && (!finishedWithTransactions) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = transactionTypeManager->initializeAllTransactionTypesQuery();
	 		transactionType_ = transactionTypeManager->getFirstTransactionType();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFITransactionTypeNames);

		if ( transactionType_.notNull() )
		{
			if ( !transactionType_->isRecordValid() )
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFITransactionTypeNames));
				myString = Long::toString(transactionType_->getNumber());
				myString.append(" - ");
				myString.append(transactionType_->getName1());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			transactionType_ = transactionTypeManager->getNextTransactionType();
			currentRecordCount++;
		}
		else
		{
			finishedWithTransactions = true;
			startingNewFile = true;
		}
	}
	else if ((productManager) && (!finishedWithProducts) )
	{
		if (startingNewFile)
		{
			startingNewFile = false;
			currentRecordCount = 0;
			totalRecords = productManager->countAllProducts();
			product = productManager->firstProduct();
		}

		//update the UI with the current task progress
		updateUI(BRGIPlu, BRFIProducts);

   		if (product)
		{
			if (!product->isRecordValid())
			{
				totalRecordErrors++;

				hobListFI->push_back(Long::toString(BRFIProducts));
				myString = Long::toString(product->getProductNum());
				myString.append(" - ");
				myString.append(product->getDescription());
				hobList->push_back(myString);
				updateUI();								//update the user interface with the list of bad records
			}
			delete product;
			product = productManager->nextProduct();
			currentRecordCount++;
   		}
		else
		{
			finishedWithProducts = true;
			startingNewFile = true;
			state = BRDoneState;
			updateUI(BRGIPlu, BRFIProducts);
		}
	}
	
	progressTotal++;
*/
}


void BackupRestoreManager::handleIntranet()
{
}



void BackupRestoreManager::requestWrapperTimings()
{
	Debug::trace("BackupRestoreManager.EvolutionWrapper", "requestWrapperTimings()");
	registerEvent(Wrapper, WrEvolBackupPackageTimings);

	GenericHfcObject gho(Bool::toString(true));		//	Current timings

	SessionMessage req(Wrapper, WrEvolLoadPackageTimings);
	req.setFormatType(BrowserFormat);
	req.setHfcObject(&gho);
	mySession->handleMessage(&req);
}


void BackupRestoreManager::requestPrinterConfig(PrinterType printerType)
{
	Debug::trace("BackupRestoreManager.PrinterSettings", "requestPrinterConfig()  for printerType " + Integer::toString(printerType));
	registerEvent(Printer, PrInquire);

	Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
	if (primarySession)
	{
		PrinterRequestInquire prInqReq(this, printerType);
		dispatchEvent(&prInqReq, primarySession);
		Debug::trace("BackupRestoreManager.PrinterSettings", "requestPrinterConfig()  Dispatched the message.");
	}
}


void BackupRestoreManager::requestWeigherConfig()
{
	Debug::trace("BackupRestoreManager.WeigherSettings", "requestWeigherConfig()  for Weigher ");
	registerEvent(Weigher, WgInquire);

	Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
	if (primarySession)
	{
		WeigherRequestInquire wgInqReq(this);
		dispatchEvent(&wgInqReq, primarySession);
		Debug::trace("BackupRestoreManager.WeigherSettings", "requestWeigherConfig()  Dispatched the message.");
	}
}


void BackupRestoreManager::updateUI(BRGroupIndicator group, BRFileIndicator file, int resetTo, int resetIgnoredTo)
{
	if (!displayStats || !mySession)
		return;

	if (!progress)
		progress = new BackupRestoreProgress(group, file);

	if (!progMsg)
	{
		progMsg = new SessionMessage(BackupRestore, Progress);
		progMsg->setFormatType(BrowserFormat);
		progMsg->setHfcObject(progress);
	}

	if (file != previousFileIndicator)
	{
		if (previousFileIndicator != BRFIUnknown)
		{
			mySession->send(progMsg, Messenger::DEST_UI);
            PAL_absclass::PAL_sleep( 100 );
		}
		previousFileIndicator = file;
	}

	if (progress && progMsg && file != BRFIUnknown)
	{
		if (state == BRRestoreState && progress->getFileIndicator() != file)
		{
			currentRecordCount = resetTo;
			currentRecordsIgnored = resetIgnoredTo;
		}

		progress->setGroupIndicator(group);
		progress->setFileIndicator(file);
		progress->setFileRecordCount(currentRecordCount);
		progress->setTaskRecordCount(progressTotal);
		progress->setTotalFileRecordCount(totalRecords);
		progress->setTotalTaskRecordCount(totalTaskRecords);
		progress->setErrorCount(totalRecordErrors);
		progress->setFilesIgnoredRecordCount(currentRecordsIgnored);
		progress->setTotalFilesIgnoredRecordCount(progressTotalIgnored);

		if ((currentRecordCount % 10) == 0)
		{
			mySession->send(progMsg, Messenger::DEST_UI);
            PAL_absclass::PAL_sleep( 10 );
		}
	}
}

void BackupRestoreManager::updateUI()
{
	if (!displayStats || !mySession)
		return;

	if (!errorsLists)
	{
		errorsLists = new BackupRestoreErrors();
		errorsLists->setFileIndicatorsList(hobListFI);
		errorsLists->setErrorRecordsList(hobList);
	}

	if (!errorsMsg)
	{
		errorsMsg = new SessionMessage(BackupRestore, ProgressBadRecords);
		errorsMsg->setFormatType(BrowserFormat);
		errorsMsg->setHfcObject(errorsLists);
	}

	if (Debug::isTracing("BadRecordScan"))
	{
		const char *data = errorsLists->serialize(BrowserFormat);
		int len = errorsLists->getSerializedLength();
		Debug::trace("BadRecordScan", HobString(data, len));
	}

	//send both lists to the user interface
	if (errorsLists && errorsMsg)
	{
		mySession->send(errorsMsg, Messenger::DEST_UI);
		//Sleep(3);
	}
}

void BackupRestoreManager::updateUIDialog(bool displayDialog, ErrorEvent errorDevice, int errorCode, short responseStyle)
{
	if (!displayStats || !mySession)
		return;

	if (displayDialog)
	{
		mySession->addDialogMessage(errorDevice, errorCode, responseStyle);
        PAL_absclass::PAL_sleep( 500 );
	}
	else
	{
		mySession->removeDialogMessage(errorDevice, errorCode, responseStyle);
        PAL_absclass::PAL_sleep( 100 );
	}
}


// override from AbstractSession
//virtual
void BackupRestoreManager::initializationBeforeRun()
{
	if (isTracing)
		Debug::trace("BackupRestoreManager", "initializationBeforeRun() - begin");

	DBSession::initializationBeforeRun();

	switch (event)
	{
		case Backup:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = Backup");
			displayStats = selections.isDisplayStats();
			initialBackup();
			state = BRBackupState;
		 	break;

		case Restore:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = Restore");
			// todo - this is only temporary, for testing
			//if (selections.getFormat() == BRVersionIndependent)
			//	selections.setEveryDataTypeSelected(true);

			displayStats = selections.isDisplayStats();
			state = BRRestoreState;
		 	break;

		case ProcessFile:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = ProcessFile");

			displayStats = selections.isDisplayStats();
			state = BRProcessFileState;
		 	break;

		case BadRecordScan:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = BadRecordScan");
			displayStats = selections.isDisplayStats();
			initialBadRecord();
			state = BRBadRecordScanState;
			break;

		case Intranet:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = Intranet");
			state = BRIntranetState;
			break;

		case BRCancel:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = BRCancel");
			state = BRCancelState;
			break;

		case UnknownBackRestore:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = UnknownBackRestore");
			break;

 		default:
			if (isTracing)
				Debug::trace("BackupRestoreManager", "initializationBeforeRun():  event = default");
			if (isTracing)	
				Debug::trace("BackupRestoreManager", "SessionMessage Event - SessionMessage passed in may not have contained a BackupRestoreSelections object");
			subState = BRCancelSubState;
			state = BRDoneState;
			break;
	}

	if (isTracing)
		Debug::trace("BackupRestoreManager", "initializationBeforeRun() - end");
}

// override from AbstractSession
//virtual
void BackupRestoreManager::finalizationAfterRun()
{
	if (isTracing)
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - begin");

	if (fpIn)
	{
		getMessenger().removeCommMechanism(fpIn);
		if (isTracing)
			Debug::trace("BackupRestoreManager", "finalizationAfterRun():  getMessenger().removeCommMechanism(fpIn)");
	}

	if (fpOut)
	{
		getMessenger().removeCommMechanism(fpOut);
		if (isTracing)
			Debug::trace("BackupRestoreManager", "finalizationAfterRun():  getMessenger().removeCommMechanism(fpOut)");
	}

	// tell the reader writer to close and, if the backup has been cancelled, delete the file
	if (fpIn)
	{
		fpIn->cmClose();
		if (isTracing)
			Debug::trace("BackupRestoreManager", "finalizationAfterRun():  fpIn->cmClose()");
	}

	if (fpOut)
	{
		fpOut->cmClose();
		if (isTracing)
			Debug::trace("BackupRestoreManager", "finalizationAfterRun():  fpOut->cmClose()");
	}

	// if the user cancelled the backup, delete the file
	if ((event == Backup && subState == BRCancelSubState) ||
		((event == ProcessFile) && (subState == BRProcessFileSubState)))
	{
		File f(fileName);
		if (f.exists())
		{
			f.remove();
			if (isTracing)
				Debug::trace("BackupRestoreManager", "finalizationAfterRun():  Removing [" + fileName + "].");
		}
	}

	if (isTracing)
	{
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - Before calling DBSession::finalizationAfterRun ...");
		//Debug::addTraceCategory("HFCManagerContainer");
	}

	DBSession::finalizationAfterRun();

	if (isTracing)	
	{
		//Debug::removeTraceCategory("HFCManagerContainer");
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - After calling DBSession::finalizationAfterRun ...");
	}

	if (writeUserTar == true)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "writeUserTar == true");

		EvolMessage msg(SessionDev, SnTarUserDirectory); 
		dispatchEvent( &msg );
	}

	if (writeSystemTar == true)
	{
		if (isTracing)	
			Debug::trace("BackupRestoreManager", "writeSystemTar == true");

		EvolMessage msg(SessionDev, SnTarSystemDirectory);
		dispatchEvent(&msg);
	}

	if (genericLabelTextUpdated)
	{
		EvolMessage msg(ConfigurationDev, CfgGenericTextUpdated);
		dispatchEvent(&msg);
		genericLabelTextUpdated = false;
	}

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - Before sending message to mySession to terminate me ...");

	EvolMessage done(BackupRestore, Terminate, this);
	dispatchEvent(&done, mySession);

	if (isTracing)	
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - Sent message to mySession to terminate me, going to idle state ...");

	if (isTracing)
		Debug::trace("BackupRestoreManager", "finalizationAfterRun() - end");
}

// override from AbstractSession
//virtual
void BackupRestoreManager::quitGracefully()
{
	if ( event != Backup )
	{
		Debug::trace("BackupRestoreManager", "quitGracefully: going to quit");

		subState = BRCancelSubState;
		state = BRDoneState;
		noMessageFromMessengerProcessing();
	}
	else
	{
		// When my session goes away it will delete me, in the mean time, changing synchronization
		// does not affect this backup manager since we are backing up data (not changing the db).
		Debug::trace("BackupRestoreManager", "quitGracefully: ignoring since we are backing up");
	}
}
