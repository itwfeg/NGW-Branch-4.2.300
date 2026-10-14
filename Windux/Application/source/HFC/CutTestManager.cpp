/* Access Scale Project
 * COPYRIGHT (c) 2010.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CutTestManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "SessionManager.h"
#include "SessionManagerHelper.h"
#include "Configuration.h"

#include "HfcError.h"
#include "ProductManager.h"
#include "Product.h"
#include "CutTestTotalManager.h"
#include "CuttingTest.h"
#include "CuttingTestReport.h"
#include "CutTestDetailsOption1.h"
#include "CutTestHistoryListing.h"
#include "CutTestDevEvent.h"
#include "Softkey.h"
#include "ProductTotalManager.h"
#include "OperatorManager.h"
#include "HobOperator.h"
#include "OperatorEvent.h"
#include "SoftkeyGroupManager.h"

#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "filesys/file.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCuttingTestsRM.h"
#include "DBTransactionsRM.h"
#include "DBTransaction.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CutTestManager::_dataType_ = 0;
std::string CutTestManager::_dataTypeName_("CutTestManager"); 
#endif


CutTestManager::CutTestManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

CutTestManager::CutTestManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

CutTestManager::~CutTestManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (currentCuttingTest)
		delete currentCuttingTest;

	destroyQueriesAndIters();
}


void CutTestManager::init()
{
	mySession_ = 0;

	currentCuttingTest = 0;
	cuttingTestOn_ = false;

	allCuttingTestsQuery = 0;
	allCuttingTestsIter  = 0;

	currentDescription_ = "";

	waitingForLoginToStartCutTest_ = false;
    waitingToStartCutTest_ = false;
}


void CutTestManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	int event = msg->getEventType();

	HobString msgDebug("handleMessage():  HfcObject = ");
	if (Debug::isTracing("CutTestManager"))
	{
		msgDebug.append(Hex::toString((long long) msg->getHfcObject()));
		msgDebug.append("   device = ").append(Integer::toString(msg->getDeviceType()));
		msgDebug.append("   event = ").append(Integer::toString(event));
		Debug::trace("CutTestManager", msgDebug);
	}

	if ( msg->getDeviceType() == CutTestDev )
	{
		switch(event)
		{
			case DtRead:
				Debug::trace("CutTestManager", msgDebug + "  DtRead");
				if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
					readCuttingTestOption1(msg);
				break;

			case DtRestore:
				Debug::trace("CutTestManager", msgDebug + "  DtRestore");
				handleRestore(msg);
				break;

			case BeginCuttingTest:		// This actually puts the cutting test into Active status
				Debug::trace("CutTestManager", msgDebug + "  BeginCuttingTest");

				if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
					beginCuttingTestOption1(msg);
				else
					beginCuttingTest(msg);
				break;

			case CuttingTestClear:
				Debug::trace("CutTestManager", msgDebug + "  CuttingTestClear");
				clearCuttingTestOption1();
				break;

			case CancelCuttingTest:		// Not for 'Option 1'
				Debug::trace("CutTestManager", msgDebug + "  CancelCuttingTest");
				cancelCuttingTest();
				break;

			case CuttingTestDetailsRead:
				Debug::trace("CutTestManager", msgDebug + "  CuttingTestDetailsRead");

				// The UI passes in the cutting test number in to read the details for, so pass the msg to the method
				if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
					readCuttingTestDetailsOption1(msg);
				else
					readCuttingTestDetails(msg);
				break;

			case CuttingTestHistoryRead:   // ReadCutTestsForOperator
				Debug::trace("CutTestManager", msgDebug + "  ReadCutTestsForOperator");
				readCuttingTestHistoryOption1();
				break;

			case SuspendCuttingTest:
				Debug::trace("CutTestManager", msgDebug + "  SuspendCuttingTest");

				if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1)
				{
					// Don't care about any data passed in.  Operator can only suspend the current cutting test.
					// Costco 01-15-2025: Dont allow Default Cut Tests to suspend tests
					suspendCutTestOption1(msg);
				}

				break;

			case DeleteCuttingTest:
				Debug::trace("CutTestManager", msgDebug + "  DeleteCuttingTest");

				if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1)
					deleteCuttingTestOption1(msg);
				break;

			case ResumeCuttingTest:
				Debug::trace("CutTestManager", msgDebug + "  ResumeCuttingTest");
				
				if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1)
					resumeCutTestOption1(msg);

				break;

			// The following message is a ReportManagerDev device message, so it won't come into the CutTestManager
			case PrintCuttingTest:
				Debug::trace("CutTestManager", msgDebug + "  PrintCuttingTest - printing is commented out in the code. Dont know why but it is commented out.");
				//printCuttingTestOption1(msg);
				break;

			case SaveCuttingTestToUsb:
				Debug::trace("CutTestManager", msgDebug + "  SaveCuttingTestToUsb");
				saveCuttingTestToUsbOption1(msg);
				break;

			case CuttingTestHistoryLogin:
				break;

			case CompleteCuttingTest:
				Debug::trace("CutTestManager", msgDebug + "  CompleteCuttingTest");
				completeCuttingTestOption1(msg);
				break;

			case DtDeleteAll:
			{
				int count = deleteAllCuttingTests();
				Debug::trace("CutTestManager", "handleMessage(DtDeleteAll):  " + Integer::toString(count) + " Cutting Tests were deleted.");
				break;
			}

		   default:
				Debug::trace("CutTestManager", "handleMessage(" + Integer::toString(event) + "):  I don't recognize this event!");
				break;
		}
	}
}


void CutTestManager::handleSoftkeyMessage(Softkey *msgSoftkey)
{
	if (!msgSoftkey)
		return;

	HobString msgDebug(" handleSoftkeyMessage():  ");

	switch ( msgSoftkey->getAction() )
	{
		case Action_BeginCutTest:		// This creates a new cutting test and passes it to the UI so the operator can fill in the initial information screen
			Debug::trace("CutTestManager", msgDebug + "Action_BeginCutTest");
			if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
				initiateCuttingTestOption1();
			else
				initiateCuttingTest();
			break;

		case Action_EndCutTest:
			Debug::trace("CutTestManager", msgDebug + "Action_EndCutTest");
			if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
				endCuttingTestOption1();
			else
				endCuttingTest();
			break;

		case Action_CutTestHistory:
			Debug::trace("CutTestManager", msgDebug + "Action_CutTestHistory");
			readCuttingTestHistoryOption1();
			break;

		case Action_CutTestSuspend:
			Debug::trace("CutTestManager", msgDebug + "Action_CutTestSuspend");

			if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1)
				suspendCutTestOption1(0);

			break;

		default:
			Debug::trace("CutTestManager", msgDebug + "I don't recognize this action!");
			break;
	}
}


void CutTestManager::destroyQueriesAndIters()
{
	destroyCuttingTestsQueryAndIter();
}


CuttingTest *CutTestManager::getCurrentCuttingTest()
{
	return currentCuttingTest;
}
void CutTestManager::setCurrentCuttingTest(CuttingTest *newCuttingTest)
{
	meatSpecSheetPLUList_ = "";

	if (currentCuttingTest)
		delete currentCuttingTest;
	currentCuttingTest = newCuttingTest;

	if ( !currentCuttingTest )
		//	If flag to indicate cut test is ON, turn it OFF.
		turnCuttingTestOff();
}


HobRecordNum CutTestManager::getCurrentCutTestNumber()
{
	HobRecordNum cutTestNum = 0;
	if (cuttingTestOn_ && currentCuttingTest)
		cutTestNum = currentCuttingTest->getTestNumber();

	return cutTestNum;
}

/* If the first record was not read, this method will return NULL.
 */
CuttingTest *CutTestManager::firstCuttingTest()
{
	if (!dbAccess)
		return 0;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return 0;

	// If the user calls firstCuttingTest() after they have already called it,
	// then delete the old query and create a new one.
	destroyCuttingTestsQueryAndIter();

	allCuttingTestsQuery = dbCuttingTestsRM->retrieveAll();

	// Couldn't get the query from which to access the CuttingTests, just return false.
	if (!allCuttingTestsQuery)
		return 0;

	DBQuery<DBCuttingTest>::const_iterator tempIter = allCuttingTestsQuery->begin();
	if (tempIter == allCuttingTestsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCuttingTest()
	 * method call below).
	 */
	allCuttingTestsIter = new DBQuery<DBCuttingTest>::const_iterator(tempIter);
	DBCuttingTest *ht = new DBCuttingTest(*(*allCuttingTestsIter));
	CuttingTest *newCuttingTest = new CuttingTest(ht, dbAccess);
	return newCuttingTest;
}


/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstCuttingTest() method, the first call to this method will
 * result in the first record being returned.
 */
CuttingTest *CutTestManager::nextCuttingTest()
{
	if (!dbAccess)
		return 0;

	if (allCuttingTestsQuery && !allCuttingTestsQuery->isEnabled())
		destroyCuttingTestsQueryAndIter();

	if (!allCuttingTestsQuery || !allCuttingTestsIter)
		return firstCuttingTest();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCuttingTestsIter);
	if (*allCuttingTestsIter == allCuttingTestsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all CuttingTests in the db. */
		destroyCuttingTestsQueryAndIter();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBCuttingTest copy constructor in order for the Hfc CuttingTest to
	 * control the destruction of the DBCuttingTest record that the iterator contains; otherwise, the DBCuttingTest will
	 * be deleted when the iterator goes out of scope.  The DBCuttingTest copy will be deleted by the CuttingTest record
	 * created below.
	 */
	DBCuttingTest *ht = new DBCuttingTest(*(*allCuttingTestsIter));
	CuttingTest *newCuttingTest = new CuttingTest(ht, dbAccess);
	return newCuttingTest;
}


/* Provide a countAll method so the caller doesn't need to know about a dbCutTestsRM.
 */
int CutTestManager::countAllCuttingTests(bool notDeleted)
{
	int recordCount = -1;

	if (!dbAccess)
		return recordCount;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return recordCount;

	bool traceOn = Debug::isTracing("RecordCount");

	if (notDeleted)
	{
		//	retrieveAll() will return a query of records that are not deleted.
		DBQuery<DBCuttingTest> *myQuery = dbCuttingTestsRM->retrieveAll();
		if (myQuery)
		{
			recordCount = (int) myQuery->size();
			delete myQuery;
		}
	}
	else
	{
		recordCount = dbCuttingTestsRM->countAll();
	}
	if (traceOn)
		Debug::trace("RecordCount", "countAllCuttingTests(" + booltostring(notDeleted) + "):  recordCount = " + inttostring(recordCount));

	return recordCount;
}


void CutTestManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	CuttingTest *restoreCutTest = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!restoreCutTest || !dbAccess)
		return;

	if (mySession_ && mySession_->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CutTests))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession_->getBatchId(), receivedChange);
	}
	else
	{
		restoreCutTest->setDBAccess(dbAccess);

		CuttingTest *usedCutTest = (CuttingTest *)NULL;
		CuttingTest *existingCutTest = CuttingTest::findCuttingTest(restoreCutTest->getTestNumber(), dbAccess);
		if (existingCutTest)
		{
			if (existingCutTest->isRecordValid())
			{
				existingCutTest->update(restoreCutTest);
				existingCutTest->saveRecord();
				usedCutTest = existingCutTest;								
			}
			else
			{
				existingCutTest->deleteRecord();	   
				restoreCutTest->saveRecord();
				usedCutTest = restoreCutTest;								
			}
		}
		else
		{
			restoreCutTest->saveRecord();
			usedCutTest = restoreCutTest;								
		}

		if (usedCutTest && usedCutTest->hasErrors() &&
			 mySession_ && mySession_->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedCutTest->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession_->send(&errMsg, Messenger::DEST_UI);

				err = usedCutTest->nextError();
			}
		}
		if (existingCutTest)
			delete existingCutTest;
	}
}


HobOperator *CutTestManager::checkForLoggedInOperator()
{
	Session *s = dynamic_cast<Session *> (mySession_);
	if (!s)
		return 0;

	HobOperator *oper = 0;
	if ( s->getOperatorManager() )
	{
		oper = s->getOperatorManager()->getLoggedInOperator();
		if (!oper)
		{
			SessionMessage msg(OperatorDev, OpPromptForIDOrRFID);
			msg.setFormatType(BrowserFormat);
			mySession_->send(&msg, Messenger::DEST_UI);
			return 0;
		}
	}
	return oper;
}


void CutTestManager::initiateCuttingTest()		//	Official Cutting test method.
{
	if (!dbAccess)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "initiateCuttingTest()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	//	First, clear the current product.
	ProductManager *myProductManager = mgrContainer_->getProductManager();
	if (myProductManager)
		myProductManager->clearCurrentProduct(false);		//	Don't update the UI.

	//	Then clear the current product run.
	ProductTotalManager *myProductTotalManager = mgrContainer_->getProductTotalManager();
	if (myProductTotalManager)
		myProductTotalManager->clearAndDisplayProductRunTotal();

//	Get the next available cutting test number from the database.
	int newTestNum = 1;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (dbCuttingTestsRM)
	{
		DBQuery<DBCuttingTest> *cuttingTestQuery = dbCuttingTestsRM->retrieveAll();
		DBQuery<DBCuttingTest>::const_iterator iter = cuttingTestQuery->rbegin();
		if (iter != cuttingTestQuery->rend())
			newTestNum = Long::valueOf(iter->getTestNumber()) + 1;
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug.append("newTestNum = ").append(Long::toString(newTestNum));
		Debug::trace("CutTestManager", msgDebug);
	}

//	Create a new CuttingTest record.
	CuttingTest *newTest = new CuttingTest(newTestNum, dbAccess);

//	Make it the current cutting test. Don't save it to the database just yet.
	setCurrentCuttingTest(newTest);

//	Send the CuttingTest data to the Browser.
	if (currentCuttingTest)
	{
		SessionMessage testData(CutTestDev, DtWrite);
		testData.setFormatType(BrowserFormat);
		testData.setHfcObject(currentCuttingTest);

		// Send the product to the UI (Browser)
		mySession_->send(&testData, Messenger::DEST_UI);
	}
}


// The first step in the cut test process.
void CutTestManager::initiateCuttingTestOption1()
{
	if (!dbAccess)
		return;

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

	waitingForLoginToStartCutTest_ = false;
    waitingToStartCutTest_ = true;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "initiateCuttingTest()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	Session *s = dynamic_cast<Session *> (mySession_);
	if (!s)
		return;

	HobOperator *oper = checkForLoggedInOperator();
	if (!oper)
	{
		waitingForLoginToStartCutTest_ = true;
		return;
	}

	if ( !checkForSuspendedCutTests( oper->getOpNum() ) )
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" Either of the suspended cut test counts was exceeded. Can't start another cut test.");
		Debug::trace("CutTestManager", msgDebug2);
		return;
	}

	//	First, clear the current product.
	ProductManager *myProductManager = mgrContainer_->getProductManager();
	if (myProductManager)
		myProductManager->clearCurrentProduct(false);		//	Don't update the UI.

	//	Then clear the current product run.
	ProductTotalManager *myProductTotalManager = mgrContainer_->getProductTotalManager();
	if (myProductTotalManager)
		myProductTotalManager->clearAndDisplayProductRunTotal();

	//	Get the next available cutting test number from the Configs db table.
	int newTestNum = cfg->toIntCfg(fieldOffsetCfgNextCutTestNumber);
	bool foundCutTest = false;

	// Try to find the cut test record for what is supposed to be the next cut test,
	// just in case we reached the max cut test number and rolled over back to 1 at some time.
	CuttingTest *existingCutTest = CuttingTest::findCuttingTest(newTestNum, dbAccess);
	if (existingCutTest)
	{
		// If it exists, delete it.
		
		// Also, we should find any cut test transactions/totals associated with the existing cut test and
		// clear out the cut test number.  I don't want to delete the transaction record since it was a valid transaction;
		// I just don't want the old transactions associated with the cut test number that is now being reused.
		DBTransactionsRM* dbTransactionsRM = mgrContainer_->getDBTransactionsRM();
		if ( dbTransactionsRM )
		{
			DBQuery<DBTransaction> *cutTestTotalsQuery = dbTransactionsRM->retrieveCutTestTransactionsForCutTest(longtostring(newTestNum));

			// If couldn't get the query from which to access the CuttingTests, just assume there aren't any transaction records for this cut test.
			if (cutTestTotalsQuery)
			{
				DBQuery<DBTransaction>::const_iterator *totalIter;
				DBTransaction *dbTransaction = 0;

				DBQuery<DBTransaction>::const_iterator tempIter = cutTestTotalsQuery->begin();
				while ( tempIter != cutTestTotalsQuery->end() )
				{
					totalIter = new DBQuery<DBTransaction>::const_iterator(tempIter);
					if (totalIter)
					{
						DBTransaction *total = new DBTransaction(*(*totalIter));
						if (total)
						{
							total->setCutTestNumberNull();
							dbTransactionsRM->saveRecord(total);
							delete total;
						}
						delete totalIter;
					}
					++tempIter;
				}
				delete cutTestTotalsQuery;
				cutTestTotalsQuery = 0;
			}
		}

		existingCutTest->deleteRecord();

		delete existingCutTest;
		existingCutTest = 0;
	}

	// Increment the next cutting test number, for the next cutting test that gets run sometime in the future.
	int nextTestNum = newTestNum + 1;
	if ( nextTestNum > cfg->toIntCfgI(fieldOffsetCfgIMaxCutTestNumber) )
	{
		nextTestNum = 1;
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("nextCutTestNum greater than max allowed in CfgInternal, reset to 1 for the next cutting test");
		Debug::trace("CutTestManager", msgDebug2);
	}

	// Save the next cutting test number to the Configs db table.
	cfg->updateField(fieldOffsetCfgNextCutTestNumber, inttostring(nextTestNum));
	cfg->saveField(fieldOffsetCfgNextCutTestNumber, dbAccess);

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug.append("newTestNum = ").append(Long::toString(newTestNum));
		Debug::trace("CutTestManager", msgDebug);
	}


//	Create a new CuttingTest record.
	CuttingTest *newTest = new CuttingTest(newTestNum, dbAccess);

//	Make it the current cutting test.
	setCurrentCuttingTest(newTest);

//	Send the CuttingTest data to the Browser.
	if (currentCuttingTest)
	{
		currentCuttingTest->setStatus(CuttingTest::Status_Initial);

		currentCuttingTest->show();
		currentCuttingTest->saveRecord();

		SessionMessage testData(CutTestDev, DtWrite);
		testData.setFormatType(BrowserFormat);
		testData.setHfcObject(currentCuttingTest);
		mySession_->send(&testData, Messenger::DEST_UI);

		turnCuttingTestOn();
		//	Cutting test is ON.
	}
    waitingToStartCutTest_ = false;				// no longer waiting, it is a go
}


// Return false if error message is displayed.
bool CutTestManager::checkForSuspendedCutTests(HobString opNum)
{
	if (!dbAccess)
		return false;

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return false;

	//waitingForLoginToStartCutTest_ = false;

	// Per requirements, can't start another cutting test if total suspended cut tests or total suspended cut tests for this operator are at their max 
	int currentSuspendedCutTestsForOperator = 0;
	int currentSuspendedCutTests = 0;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (dbCuttingTestsRM)
	{
		DBQuery<DBCuttingTest> *allCutTestsQuery = dbCuttingTestsRM->retrieveAll();

		// If couldn't get the query from which to access the CuttingTests, just assume operator doesn't have any suspended cut tests, so ok to suspend.
		if (allCutTestsQuery)
		{
			DBQuery<DBCuttingTest>::const_iterator *cutTestsIter;

			DBQuery<DBCuttingTest>::const_iterator tempIter = allCutTestsQuery->begin();
			while ( tempIter != allCutTestsQuery->end() )
			{
				cutTestsIter = new DBQuery<DBCuttingTest>::const_iterator(tempIter);
				if (cutTestsIter)
				{
					DBCuttingTest *dbCutTest = new DBCuttingTest(*(*cutTestsIter));
					if (dbCutTest)
					{
						if ( (CuttingTest::CutTestStatus)(dbCutTest->getStatus()) == CuttingTest::Status_Suspended )
						{
							currentSuspendedCutTests++;
							if ( opNum.compare( dbCutTest->getOperatorNumber() ) == 0 )
								currentSuspendedCutTestsForOperator++;
						}
						delete dbCutTest;
					}
					delete cutTestsIter;
				}
				++tempIter;
			}
			delete allCutTestsQuery;
			allCutTestsQuery = 0;
		}
	}

	if ( currentSuspendedCutTestsForOperator >= Configuration::getInstance()->toIntCfg(fieldOffsetCfgMaxSuspendedCutTestsOperator) )
	{
		SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantStartBecauseTooManyOperatorSuspended, "");
		waitingForLoginToStartCutTest_ = false;			// if this is true, kill the cut test flag
		return false;
	}
	else if ( currentSuspendedCutTests >= Configuration::getInstance()->toIntCfg(fieldOffsetCfgMaxSuspendedCutTestsOverall) )
	{
		SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantStartBecauseTooManyOverallSuspended, "");
		waitingForLoginToStartCutTest_ = false;			// if this is true, kill the cut test flag
		return false;
	}

	return true;
}


// The operator is cancelling this Cutting Test from the initial information screen.
// Since we've already saved it to the database, delete it and reclaim the cut test number so it can be used for the next one.
void CutTestManager::clearCuttingTestOption1()
{
	if (!dbAccess)
		return;

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "clearCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	//	First, clear the current product.
	ProductManager *myProductManager = mgrContainer_->getProductManager();
	if (myProductManager)
		myProductManager->clearCurrentProduct(false);		//	Don't update the UI.

	// Have to have a current cutting test or there is nothing to clear
	if (currentCuttingTest)
	{
		// Try to find the cut test record for what is supposed to be the next cut test,
		// just in case we reached the max cut test number and rolled over back to 1 at some time.
		CuttingTest *existingCutTest = CuttingTest::findCuttingTest(currentCuttingTest->getTestNumber(), dbAccess);
		if (existingCutTest)
		{
			// It is supposed to exist, so delete it.
		
			// Also, we should find any cut test transactions/totals associated with the existing cut test and
			// clear out the cut test number. There shouldn't be any, but just in case, check for them.
			// I don't want to delete the transaction record since it was a valid transaction;
			// I just don't want the old transactions associated with the cut test number that is now being reused.
			DBTransactionsRM* dbTransactionsRM = mgrContainer_->getDBTransactionsRM();
			if ( dbTransactionsRM )
			{
				DBQuery<DBTransaction> *cutTestTotalsQuery = dbTransactionsRM->retrieveCutTestTransactionsForCutTest(longtostring(currentCuttingTest->getTestNumber()));

				// If couldn't get the query from which to access the CuttingTests, just assume there aren't any transaction records for this cut test.
				if (cutTestTotalsQuery)
				{
					DBQuery<DBTransaction>::const_iterator *totalIter;
					DBTransaction *dbTransaction = 0;

					DBQuery<DBTransaction>::const_iterator tempIter = cutTestTotalsQuery->begin();
					while ( tempIter != cutTestTotalsQuery->end() )
					{
						totalIter = new DBQuery<DBTransaction>::const_iterator(tempIter);
						if (totalIter)
						{
							DBTransaction *total = new DBTransaction(*(*totalIter));
							if (total)
							{
								total->setCutTestNumberNull();
								dbTransactionsRM->saveRecord(total);
								delete total;
							}
							delete totalIter;
						}
						++tempIter;
					}
					delete cutTestTotalsQuery;
					cutTestTotalsQuery = 0;
				}
			}

			existingCutTest->deleteRecord();

			delete existingCutTest;
			existingCutTest = 0;
		}

		//	Get the next available cutting test number from the Configs db table.
		int newTestNum = cfg->toIntCfg(fieldOffsetCfgNextCutTestNumber);

		// Decrement the cutting test number to reclaim the number, for the next cutting test that gets run sometime in the future.
		int nextTestNum = newTestNum - 1;
		if ( nextTestNum == 0 )
			nextTestNum = 1;

		// Save the next cutting test number to the Configs db table.
		cfg->updateField(fieldOffsetCfgNextCutTestNumber, inttostring(nextTestNum));
		cfg->saveField(fieldOffsetCfgNextCutTestNumber, dbAccess);

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug.append("newTestNum = ").append(Long::toString(newTestNum));
			Debug::trace("CutTestManager", msgDebug);
		}

		//	Clear the current cutting test.
		setCurrentCuttingTest(0);
	}
}


void CutTestManager::readCuttingTestOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "readCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	// Try to find the cut test record for what is supposed to be the next cut test,
	// just in case we reached the max cut test number and rolled over back to 1 at some time.
	CuttingTest *existingCutTest = CuttingTest::findCuttingTest(msgData->getTestNumber(), dbAccess);
	if (existingCutTest)
	{
		setCurrentCuttingTest(existingCutTest);

		SessionMessage testData(CutTestDev, DtWrite);
		testData.setFormatType(BrowserFormat);
		testData.setHfcObject(currentCuttingTest);
		mySession_->send(&testData, Messenger::DEST_UI);

		turnCuttingTestOn();
		//	Cutting test is ON.
	}

}


void CutTestManager::beginCuttingTest(SessionMessage *msg)		//	Official Cutting test method.
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "beginCuttingTest()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

//	Save the current cutting test to the database.
	if (currentCuttingTest)
	{
		currentCuttingTest->update(msgData);
		currentCuttingTest->show();
		currentCuttingTest->saveRecord();

		turnCuttingTestOn();
//	Cutting test is ON.
	}
}

void CutTestManager::beginCuttingTestOption1(SessionMessage *msg)		//	Official Cutting test method.
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "beginCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	if ( currentCuttingTest )
	{
		// The UI sends updated information to this method
		currentCuttingTest->update(msgData);
		setCurrentCutTestInProcess();
	}
}

//  This method is shared between beginning a cut test and resuming one.
void CutTestManager::setCurrentCutTestInProcess()
{
	meatSpecSheetPLUList_ = "";

//	Save the current cutting test to the database.
	if (currentCuttingTest)
	{
		currentCuttingTest->setStatus(CuttingTest::Status_InProcess);

		currentCuttingTest->show();
		currentCuttingTest->saveRecord();

		HobRecordNum pluNum = Long::valueOf(currentCuttingTest->getProductNumber());
		Product *prod = Product::findRecord(pluNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
		if (prod)
		{
			HobRecordNum meatSpecSheetGroupNum = prod->getCutTestMeatSpecSheetGroupNum();
			if ( meatSpecSheetGroupNum > 0 )
			{
				SoftkeyGroupManager *mySoftkeyGroupManager = mgrContainer_->getSoftkeyGroupManager();
				if (mySoftkeyGroupManager)
					mySoftkeyGroupManager->determinePLUsInSoftkeyGroup(meatSpecSheetGroupNum, 0, meatSpecSheetPLUList_);
			}

			delete prod;
			prod = 0;
		}
	}

	turnCuttingTestOn();
	//	Cutting test is ON.
}


// This method is for checking if the PLU that the operator wants to use during the cut test as one of the primal cuts
// is a valid PLU for the primal.  If the primal has a Meat Spec Sheet, denoted as having a flashkey group number associated
// with the Primal PLU in the meatSpecSheetGroupNum field, then the operator can only select PLUs that are in the Meat Spec
// Sheet.  If the Primal PLU doesn't have a Meat Spec Sheet, then any PLU in the database is valid for that primal.
// This method handles both scenarios.
bool CutTestManager::isPLUValidForCutTestOption1(const HobRecordNum pluNum) const
{
	if (pluNum <= 0)
		return false;

	string searchPLU = "," + Long::toString(pluNum) + ",";
	if ( !meatSpecSheetPLUList_.empty() )
	{
		if ( meatSpecSheetPLUList_.find(searchPLU) == std::string::npos )
			return false;
		else
			return true;
	}
	else
		return true;
}


void CutTestManager::cancelCuttingTest()		//	Official Cutting test method.
{
	Debug::trace("CutTestManager", "handleCancelCuttingTest()");

//	Delete the current cutting test from memory.
	setCurrentCuttingTest(0);
}


void CutTestManager::readCuttingTestDetails(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "readCuttingTestDetails()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

}


// Read the details for the cutting test, displayed at the end when completing the cutting test
void CutTestManager::readCuttingTestDetailsOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "readCuttingTestDetailsOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	CutTestDetailsOption1 ctdo;
	ctdo.createDetailsOption1(dbAccess, longtostring(msgData->getTestNumber()));

	SessionMessage testData(CutTestDev, CuttingTestDetailsWrite);
	testData.setFormatType(BrowserFormat);
	testData.setHfcObject(&ctdo);
	mySession_->send(&testData, Messenger::DEST_UI);
}

// Read the details for the cutting test, displayed at the end when completing the cutting test
CutTestDetailsOption1 *CutTestManager::readCuttingTestDetailsForReport(int testNumber)
{
    Debug::trace("CutTestManager", "readCuttingTestDetailsForReport() testNumber: " + longtostring( testNumber ) );

	HobRecordNum hrnTestNumber = testNumber;
	CutTestDetailsOption1* ctdo = new CutTestDetailsOption1();
	if( ctdo )
		ctdo->createDetailsOption1(dbAccess, longtostring( testNumber ));
	
	Debug::trace("CutTestManager", "readCuttingTestDetailsForReport() read cutTestDetails from DB: ");
    
	return ctdo;
}

const HobString &CutTestManager::readCuttingTestDetailsDescription(HobString plunum) const
{
	Debug::trace("CutTestManager", "readCuttingTestDetailsDescription()" );

	HobRecordNum productNum = Long::valueOf( plunum );
	Util::CSmartPtr<Product> theProduct(Product::findRecord(productNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess));

	Debug::trace("CutTestManager", "got smart pointer()" );

	if( theProduct.null() )
	{
		Debug::trace("CutTestManager", "The product is null" );
		currentDescription_ = "Description does not exist";
	}
	else
	{
		currentDescription_ = theProduct->getDescription();
		Debug::trace("CutTestManager", "currentDescription = " + currentDescription_);
	}

	return currentDescription_;
}


// Read the non-deleted, non-InProcess cutting tests that the currently logged in operator has access to view
void CutTestManager::readCuttingTestHistoryOption1()
{
	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "readCuttingTestHistoryOption1() for Option1 AND Default";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	HobString operatorNum = "0";

	Debug::trace("CutTestManager", HobString(" Cut Test Version: ") + inttostring(Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled)) );

	if (CutTestOption_Option1 == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) )
	{
		Debug::trace("CutTestManager", HobString("IF-ed as Option1."));

		HobOperator* oper = checkForLoggedInOperator();
		if (!oper)
		{
			Debug::trace("CutTestManager", HobString("readHistory() NO OPERATOR"));
			return;			//  if default we are going to try and roll with no operators - so continue on
		}

		operatorNum = oper->getOpNum();
	}

	CutTestHistoryListing cthl;
	cthl.createHistoryListing(dbAccess, operatorNum);

	SessionMessage testData(CutTestDev, CuttingTestHistoryWrite);
	testData.setFormatType(BrowserFormat);
	testData.setHfcObject(&cthl);
	if (mySession_)
		mySession_->send(&testData, Messenger::DEST_UI);
}


void CutTestManager::suspendCutTestOption1(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "suspendCutTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = 0;
	if (msg)
		msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());

	if (currentCuttingTest)
	{

// Don't need to notify the UI that the Suspend was successful.  The Suspend will always be successful; we are checking
// whether the operator has too many suspended cut tests up-front, when they try to start a new cut test instead of when
// they try to suspend it. (Per requirements).
/*
		// Check how many suspended cutting tests the operator has.
		// If too many, display error message; otherwise, suspend it.
		string operatorNum = Long::toString(currentCuttingTest->getOperatorId());
		int currentSuspendedCutTestsForOperator = 0;
		int currentSuspendedCutTests = 0;

		DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
		if (dbCuttingTestsRM)
		{
			DBQuery<DBCuttingTest> *allCutTestsQuery = dbCuttingTestsRM->retrieveAll();

			// If couldn't get the query from which to access the CuttingTests, just assume operator doesn't have any suspended cut tests, so ok to suspend.
			if (allCutTestsQuery)
			{
				DBQuery<DBCuttingTest>::const_iterator allCutTestsIter = allCutTestsQuery->begin();
				while ( allCutTestsIter != allCutTestsQuery->end() )
				{
					DBCuttingTest *test = new DBCuttingTest(*(allCutTestsIter));
					if ( (CuttingTest::CutTestStatus)(Integer::valueOf(test->getStatus())) == CuttingTest::Status_Suspended )
					{
						currentSuspendedCutTests++;
						if ( operatorNum.compare( test->getOperatorNumber() ) == 0 )
							currentSuspendedCutTestsForOperator++;
					}
					allCutTestsIter++;
				}
				delete allCutTestsQuery;
				allCutTestsQuery = 0;
			}
		}

		if ( currentSuspendedCutTestsForOperator >= Configuration::getInstance()->toIntCfg(fieldOffsetCfgMaxSuspendedCutTestsOperator) )
		{
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantSuspendTooManyOperator, "");
		}
		else if ( currentSuspendedCutTests >= Configuration::getInstance()->toIntCfg(fieldOffsetCfgMaxSuspendedCutTestsOverall) )
		{
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantSuspendTooManyOverall, "");
		}
		else
*/
		{
			// If the cutting test data was sent in with this Suspend message, as is the case when the operator suspends
			// the cut test before even starting it, then we need to save that data, because it would not have yet been saved.
			currentCuttingTest->update(msgData);

			// Validate the primal plu.  If it doesn't exist, clear out the plu number in the cut test record.
			HobRecordNum pluNum = Long::valueOf(currentCuttingTest->getProductNumber());
			Product *prod = Product::findRecord(pluNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
			if (!prod || ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIRequireMeatSpecSheet) && prod->getCutTestMeatSpecSheetGroupNum() == 0 ) )
				currentCuttingTest->setProductNumber("");
			if ( prod )
			{
				delete prod;
				prod = 0;
			}

			currentCuttingTest->updateCutTestTotalsOption1();

			// Don't have too many suspended cut tests.  So go ahead and suspend the current one.
			currentCuttingTest->setStatus(CuttingTest::Status_Suspended);

			currentCuttingTest->show();
			currentCuttingTest->saveRecord();

			setCurrentCuttingTest(0);

			// Don't need to notify the UI that the Suspend was successful.  The Suspend will always be successful; we are checking
			// whether the operator has too many suspended cut tests up-front, when they try to start a new cut test instead of when
			// they try to suspend it. (Per requirements).
			//SessionMessage suspendSuccessMsg(CutTestDev, CuttingTestSuspendSuccess);
			//mySession_->send(&suspendSuccessMsg, Messenger::DEST_UI);
		}
	}
}
	

void CutTestManager::deleteCuttingTestOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "deleteCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	HobOperator *oper = checkForLoggedInOperator();
	if (!oper)
		return;

	// Access levels 1 to 4 cannot delete cut tests.
	if ( oper->getAccessLevel() <= 4 )
	{
		SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrOperatorAccessLevelInsufficientForDelete, "");
		return;
	}

	// The cut test number to delete is passed into this method.  We aren't necessarily deleting the current cut test.
	HobRecordNum cutTestNum = msgData->getTestNumber();
	CuttingTest *myCutTest = CuttingTest::findCuttingTest(cutTestNum, dbAccess);
	if (myCutTest)
	{
		// Can't delete a cut test that is completed, or already deleted.
		if ( myCutTest->getStatus() == CuttingTest::Status_Completed )
		{
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantDeleteCompletedCutTest, "");
			return;
		}
		else if ( myCutTest->getStatus() == CuttingTest::Status_Deleted )
		{
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCantDeleteAlreadyDeletedCutTest, "");
			return;
		}
		else if ( oper->getAccessLevel() > 4 && oper->getAccessLevel() < 9 && myCutTest->getOperatorId() != oper->getOpNum() )
		{
			// Access levels 4 to 8 can only delete their own cut tests.
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCanOnlyDeleteYourOwnCutTests, "");
			return;
		}
		else
		{
			myCutTest->setStatus(CuttingTest::Status_Deleted);

			//Per requirement "Design and Implementation Constraints" section, pg 3 of 39, #11-a and #11-c.
			myCutTest->setFinalComments("");
			myCutTest->setPercentTotalShrinkage(0);
			myCutTest->setTotalPriceOfCutProducts(0);
			myCutTest->setTotalWeightOfCutProducts(0);

			myCutTest->show();
			myCutTest->saveRecord();

			// Also, we should find any cut test transactions/totals associated with the existing cut test and
			// delete them - per the requirements spec.
			DBTransactionsRM* dbTransactionsRM = mgrContainer_->getDBTransactionsRM();
			if ( dbTransactionsRM )
			{
				DBQuery<DBTransaction> *cutTestTotalsQuery = dbTransactionsRM->retrieveCutTestTransactionsForCutTest(longtostring(cutTestNum));

				// If couldn't get the query from which to access the CuttingTests, just assume there aren't any transaction records for this cut test.
				if (cutTestTotalsQuery)
				{
					DBQuery<DBTransaction>::const_iterator totalsIter = cutTestTotalsQuery->begin();
					while ( totalsIter != cutTestTotalsQuery->end() )
					{
						DBTransaction *total = new DBTransaction(*(totalsIter));
						dbTransactionsRM->deleteRecord(total);
						totalsIter++;
					}
					delete cutTestTotalsQuery;
					cutTestTotalsQuery = 0;
				}
			}
		}
	}
}


void CutTestManager::resumeCutTestOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "resumeCutTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	HobOperator *oper = checkForLoggedInOperator();
	if (!oper)
		return;

	HobRecordNum cutTestNum = msgData->getTestNumber();
	CuttingTest *myCutTest = CuttingTest::findCuttingTest(cutTestNum, dbAccess);
	if (myCutTest)
	{
		if ( oper->getAccessLevel() < 9 && myCutTest->getOperatorId() != oper->getOpNum() )
		{
			// Access levels 1 to 8 can only resume their own cut tests.
			SessionManagerHelper::getInstance()->addDialog(ErrCutTestDev, CutTestErrCanOnlyResumeYourOwnCutTests, "");
			return;
		}
		else
		{
			//	Make it the current cutting test.
			setCurrentCuttingTest(myCutTest);
			setCurrentCutTestInProcess();

			SessionMessage testData(CutTestDev, DtWrite);
			testData.setFormatType(BrowserFormat);
			testData.setHfcObject(currentCuttingTest);
			mySession_->send(&testData, Messenger::DEST_UI);
		}
	}
}


void CutTestManager::saveCuttingTestToUsbOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "saveCuttingTestToUsbOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());
	if (!msgData)
		return;

	// For this message, the UI passes in the cutting test number to write out to the file, and the filename.
	// In the CuttingTest object, the operator number is the second parameter, and it's a string datatype.
	// We will still use a CuttingTest object for this message, and the filename to store the data, the second parameter,
	// will be found in the CuttingTest object's operator number, since that is the second parameter passed in
	// when creating a CuttingTest object.
	// UPDATE 06-27-2016: I am now sending the testNumber (which is not needed) the operator id and the filename
	
	HobRecordNum cutTestNum = msgData->getTestNumber();			// throw this away
	string operatorId = msgData->getOperatorId();

	string outputFilename = msgData->getVendorNumber();
	if ( outputFilename.empty() )
		// No output filename specified, then leave.
		return;

	File output(outputFilename);
	if (output.exists())
		output.remove();

	if ( 0 == cutTestNum )
	{
		CutTestHistoryListing cthl;
		cthl.createHistoryListing(dbAccess, operatorId);

		HobList *testNumList = cthl.getCutTestNumList();
		if ( 0 == testNumList->size() )
			return;

		HobString listedTestNum;
		int gotEntry = testNumList->first(listedTestNum);
		while ( 0 == gotEntry )
		{
			// needs a header too
			CutTestDetailsOption1 ctdo;
			ctdo.createDetailsOption1(dbAccess, listedTestNum);

			Session *s = dynamic_cast<Session *> (mySession_);
			LanguageSelection whichLang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

			ctdo.exportToFile(outputFilename, whichLang);

			gotEntry = testNumList->next(listedTestNum);
		}
	}
	else
	{
		CutTestDetailsOption1 ctdo;
		ctdo.createDetailsOption1(dbAccess, Long::toString(cutTestNum));

		Session *s = dynamic_cast<Session *> (mySession_);
		LanguageSelection whichLang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

		ctdo.exportToFile(outputFilename, whichLang);
	}

#if defined ( __linux__ )
	system("sync");
#endif

	SessionMessage done(CutTestDev, CuttingTestSaveToUSBFinished);
	done.setFormatType(BrowserFormat);
	mySession_->send(&done, Messenger::DEST_UI);
}


void CutTestManager::completeCuttingTestOption1(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "completeCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	// This CuttingTest object is currently being passed in with the 'complete cut test' message,
	// because the final comments need to be written to the database, but the object doesn't have
	// to be passed in (if the data was already updated and nothing new needed to be updated).
	// The call to update(msgData) below handles null being passed to it.
	CuttingTest *msgData = dynamic_cast<CuttingTest *> (msg->getHfcObject());

	//	Save the final comments to the database, and set the status to 'Completed'.
	if (currentCuttingTest)
	{
		currentCuttingTest->update(msgData);
		currentCuttingTest->setStatus(CuttingTest::Status_Completed);
		currentCuttingTest->updateCutTestTotalsOption1();

		currentCuttingTest->show();
		currentCuttingTest->saveRecord();

		turnCuttingTestOff();
		//	Cutting test is OFF.
	}
}


//	For the Original Cutting test implementation.
void CutTestManager::endCuttingTest()
{
	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "endCuttingTest()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	HobDateTime now;
	int totalsCount = -1;
	CutTestTotalManager* cutTestTotalManager = mgrContainer_->getCutTestTotalManager();

	turnCuttingTestOff();
//	Cutting test is OFF.

	if (currentCuttingTest)
	{
		//	Modify the currentCuttingTest with the endTime, then save it.
		currentCuttingTest->setEndDate(now);
		currentCuttingTest->saveRecord();

		currentCuttingTest->show();

		//	Create then send the Report.
		if (cutTestTotalManager)
		{
			CuttingTestReport *ctReport = cutTestTotalManager->createCutTestReport(currentCuttingTest, totalsCount);
			if (!ctReport)
				currentCuttingTest->deleteRecord();
			else
			{
				SessionMessage testReport(CutTestDev, CuttingTestDetailsWrite);
				testReport.setFormatType(BrowserFormat);
				testReport.setHfcObject(ctReport);

				// Send the product to the UI (Browser)
				mySession_->send(&testReport, Messenger::DEST_UI);
			}

			//	Find out how many totals there are for this cutting test.
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("# of totals for Cut test #").append(Integer::toString(currentCuttingTest->getTestNumber()));
				msgDebug2.append(" = ").append(Integer::toString(totalsCount));
				Debug::trace("CutTestManager", msgDebug2);
			}
		}
		//	If no totals, then delete this cutting test.
		if (totalsCount <= 0)
			currentCuttingTest->deleteRecord();

		setCurrentCuttingTest(0);
	}
}


void CutTestManager::endCuttingTestOption1()
{
	bool traceOn = Debug::isTracing("CutTestManager");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "endCuttingTestOption1()";
		Debug::trace("CutTestManager", msgDebug);
		msgDebug.append(":  ");
	}

	turnCuttingTestOff();
//	Cutting test is OFF.

	if (currentCuttingTest)
	{
		//	Modify the currentCuttingTest with the endTime, then save it.
		HobDateTime now;
		currentCuttingTest->setEndDate(now);

		currentCuttingTest->show();
		currentCuttingTest->saveRecord();
	}
}


int CutTestManager::deleteAllCuttingTests()
{
	Debug::trace("CutTestManager", "handleDeleteAllCuttingTests()");

	if (!dbAccess)
		return 0;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return 0;

	int count = 0;   //dbCuttingTestsRM->deleteAll();

	DBQuery<DBCuttingTest> *allCutTestsQuery = dbCuttingTestsRM->retrieveAll();
	if (allCutTestsQuery)
	{
		DBQuery<DBCuttingTest>::const_iterator *cutTestsIter;

		DBQuery<DBCuttingTest>::const_iterator tempIter = allCutTestsQuery->begin();
		while ( tempIter != allCutTestsQuery->end() )
		{
			cutTestsIter = new DBQuery<DBCuttingTest>::const_iterator(tempIter);
			if (cutTestsIter)
			{
				DBCuttingTest *dbCutTest = new DBCuttingTest(*(*cutTestsIter));
				if (dbCutTest)
				{
					CuttingTest::CutTestStatus stat( (CuttingTest::CutTestStatus)dbCutTest->getStatus() );
					bool deleteIt = false;
					if ((stat == CuttingTest::Status_Suspended && Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) != CutTestOption_Classic) ||
						stat == CuttingTest::Status_Initial ||
						stat == CuttingTest::Status_InProcess)
						; // don't delete it.  I formatted it this way because I thought the condition was easier to read
					else
					{
						dbCuttingTestsRM->deleteRecord(dbCutTest);
						count++;
					}

					delete dbCutTest;
					dbCutTest = 0;
				}
				delete cutTestsIter;
			}
			++tempIter;
		}
		delete allCutTestsQuery;
		allCutTestsQuery = 0;
	}


	destroyCuttingTestsQueryAndIter();

	return count;
}


bool CutTestManager::isCuttingTestOn()
{
	return cuttingTestOn_;
}

void CutTestManager::turnCuttingTestOn()
{
	cuttingTestOn_ = true;
	Debug::trace("CutTestManager", "turnCuttingTestOn():  cuttingTestOn = " + booltostring(cuttingTestOn_));
}

void CutTestManager::turnCuttingTestOff()
{
	cuttingTestOn_ = false;
	Debug::trace("CutTestManager", "turnCuttingTestOff():  cuttingTestOn = " + booltostring(cuttingTestOn_));
}


void CutTestManager::destroyCuttingTestsQueryAndIter()
{
	if (allCuttingTestsIter)
	{
		delete allCuttingTestsIter;
		allCuttingTestsIter = 0;
	}

	if (allCuttingTestsQuery)
	{
		delete allCuttingTestsQuery;
		allCuttingTestsQuery = 0;
	}
}
