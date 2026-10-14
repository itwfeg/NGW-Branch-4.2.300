/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "DeptManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"
#include "HfcResponse.h"
//#include "ResponseMessageManager.h"
#include "HobList.h"
#include "Dept.h"
#include "AllDepts.h"
#include "SoftkeyDevEvent.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DeptManager::dataType = 0;
std::string DeptManager::dataTypeName("DeptManager"); 
#endif


DeptManager::DeptManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

DeptManager::DeptManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

DeptManager::~DeptManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentDept)
		delete currentDept;

	destroyQueriesAndIters();

	SessionManager::getInstance()->verifyDeptBasedReservedRecords((dbAccess) ? dbAccess : ((mgrContainer_) ? mgrContainer_ : 0));
}


void DeptManager::init()
{
	mySession = 0;

	currentDept = 0;

	allDeptsQuery = 0;
	allDeptsIter  = 0;
}


DBSession *DeptManager::getMySession()
{
	return mySession;
}
void DeptManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Dept *DeptManager::firstDept()
{
	if (!dbAccess)
		return (Dept *)NULL;

	DBDeptsRM *dbDeptsRM = dbAccess->getDBDeptsRM();
	if (!dbDeptsRM)
		return (Dept *)NULL;

	// If the user calls firstDept() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allDeptsQuery = dbDeptsRM->retrieveAll();

	// Couldn't get the query from which to access the Depts, just return false.
	if (!allDeptsQuery)
		return (Dept *)NULL;

	DBQuery<DBDept>::const_iterator tempIter = allDeptsQuery->begin();
	if (tempIter == allDeptsQuery->end())
		return (Dept *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextDept()
	 * method call below).
	 */
	allDeptsIter = new DBQuery<DBDept>::const_iterator(tempIter);
	DBDept *d = new DBDept(*(*allDeptsIter));
	Dept *dept = new Dept(d, dbAccess);
	return dept;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstDept() method, the first call to this method will
 * result in the first record being returned.
 */
Dept *DeptManager::nextDept()
{
	if (!dbAccess)
		return (Dept *)NULL;

	if (allDeptsQuery && !allDeptsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allDeptsQuery || !allDeptsIter)
		return firstDept();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allDeptsIter);
	if (*allDeptsIter == allDeptsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Depts in the db. */
		destroyQueriesAndIters();
		return (Dept *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBDept copy constructor in order for the Hfc Dept to
	 * control the destruction of the DBDept record that the iterator contains; otherwise, the DBDept will
	 * be deleted when the iterator goes out of scope.  The DBDept copy will be deleted by the Dept record
	 * created below.
	 */
	DBDept *d = new DBDept(*(*allDeptsIter));
	Dept *dept = new Dept(d, dbAccess);
	return dept;
}


/* Provide a countAll method so the caller doesn't need to know about a dbDeptsRM.
 */
int DeptManager::countAllDepts()
{
	if (!dbAccess)
		return -1;

	DBDeptsRM *dbDeptsRM = dbAccess->getDBDeptsRM();
	if (!dbDeptsRM)
		return -1;

	return dbDeptsRM->countAll();
}


Dept *DeptManager::getCurrentDept()
{
	return currentDept;
}
void DeptManager::setCurrentDept(Dept *newDept)
{
	if (currentDept)
		delete currentDept;
	currentDept = newDept;
}



void DeptManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than DeptsDev.
	if (!msg)
		return;

	Dept *msgDept = dynamic_cast<Dept *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("DeptManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgDept);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgDept);
			break;

		case DtDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgDept);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgDept);
			break;

		case ReadCurrentDept:
		{
			if (mySession && currentDept && currentDept->getDeptNum() == Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum))
			{
				SessionMessage writeDeptMsg(DeptDev, DtWrite);
				writeDeptMsg.setFormatType(BrowserFormat);
				writeDeptMsg.setHfcObject(currentDept);
				mySession->send(&writeDeptMsg, Messenger::DEST_UI);
				break;
			}
			else
			{
				msgDept = new Dept();
				msgDept->setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			// NOTICE!!  There is no break statement here for this case;  I intentionally want it to fall through to the SetCurrentDept case
		}

		case SetCurrentDept:
		{
			if (handleRead(msgDept))
			{
				Configuration::getInstance()->updateField(fieldOffsetCfgMostRecentActiveDeptNum, longtostring(currentDept->getDeptNum()));
				Configuration::getInstance()->saveField(fieldOffsetCfgMostRecentActiveDeptNum, mySession);
			}
			// The following is here if the ReadCurrentDept case above fell through to this case
			if (event == ReadCurrentDept)
			{
				delete msgDept;
				msgDept = 0;
			}
			break;
		}

		default:
			Debug::trace("DeptManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}

}


bool DeptManager::handleRead(Dept *d)
{
	if (!d || !dbAccess)
		return false;

	Dept *lookedUpDept = Dept::findRecord(d->getDeptNum(), dbAccess);
	if (!lookedUpDept)
	{
		// Send error message to browser indicating Dept not found.
		if (mySession)
			mySession->addDialogMessage(ErrDeptDev, DeptErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpDept)
			delete lookedUpDept;
		return false;
	}

	// We have a new current Dept.
	setCurrentDept(lookedUpDept);

	if (mySession)
	{
		SessionMessage writeDeptMsg(DeptDev, DtWrite);
		writeDeptMsg.setFormatType(BrowserFormat);
		writeDeptMsg.setHfcObject(currentDept);

		mySession->send(&writeDeptMsg, Messenger::DEST_UI);
	}

	return true;
}


void DeptManager::handleReadAll()
{
	Debug::trace("DeptManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	AllDepts all;
	all.findAll(dbAccess);

	SessionMessage allDeptsMsg(DeptDev, DtWriteAll);
	allDeptsMsg.setFormatType(BrowserFormat);
	allDeptsMsg.setHfcObject(&all);
	mySession->send(&allDeptsMsg, Messenger::DEST_UI);
}


void DeptManager::handleWrite(Dept *d)
{
	if ((!d) || (!currentDept))
		return;

  	currentDept->updateRecord(d);
}


void DeptManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Dept *msgDept = dynamic_cast<Dept *> (msg->getHfcObject());
	if (!msgDept)
		return;

	handleDelete(msgDept->getDeptNum());
}


void DeptManager::handleDelete(HobRecordNum deptNum)
{
	//We don't delete any data associated with the default dept, dept 0.
	if (deptNum == 0)
		return;

	if (!dbAccess)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	Dept *existingDept = Dept::findRecord(deptNum, dbAccess);
	if (existingDept)
	{
		existingDept->deleteRecord();
		Debug::trace("DeptManager", string("handleDelete - Dept deleted - deptNum:") + longtostring(deptNum));

		if ( currentDept && currentDept->getDeptNum() == deptNum )
		{
			delete currentDept;
			currentDept = (Dept*) NULL;
		}

		delete existingDept;

		if (config->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum) == deptNum)
		{
			config->updateField(fieldOffsetCfgMostRecentActiveDeptNum, "0");
			config->saveField(fieldOffsetCfgMostRecentActiveDeptNum, dbAccess);
		}

		// Don't need to delete the dept data if a 'delete all files' is in progress, since the data will be deleted by that process
		if ( !SessionManager::getInstance()->isDeleteAllFilesInProgress() )
			handleDeleteDeptRelatedRecords(deptNum);
	}
	else
		Debug::trace("DeptManager", string("handleDelete - dept to be deleted was not in the database to delete - dept num:") + longtostring(deptNum));
}

void DeptManager::handleDeleteDeptRelatedRecords(HobRecordNum deptNum)
{
	//We don't delete any data associated with the default dept, dept 0.
	if (deptNum == 0)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	if (mySession)
		mySession->addDialogMessage(ErrDeptDev, DeptErrDeletingRelatedRecords, HfcResponse::RESPONSESTYLE_NOINPUT);
	PAL_absclass::PAL_sleep(50);    //	To make sure the message above makes it to the display before we take over the processor.

	Debug::trace("DeptManager", "handleDeleteDeptRelatedRecords() started");

	try
	{
		DataManager* deleteAllForDeptDM = 0;
		hfc::HFCManagerContainer::recordTypeContainerIterator_type deleteAllForDeptDMIterator = mgrContainer_->beginDeleteAllFilesForDeptDataManager();
		while (deleteAllForDeptDMIterator != mgrContainer_->endDeleteAllFilesForDeptDataManager())
		{
			deleteAllForDeptDM = mgrContainer_->getDataManager(deleteAllForDeptDMIterator);
			if (deleteAllForDeptDM)
			{
				int dmDevice = (int)(deleteAllForDeptDM->getDevice());
				Debug::trace("DeptManager", string("calling handleDeleteAll for DataManager device:") + inttostring(dmDevice));
				deleteAllForDeptDM->handleDeleteAll(deptNum);
			}

			++deleteAllForDeptDMIterator;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("DeptManager::handleDeleteDeptRelatedRecords() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}


	if (mySession)
		mySession->removeDialogMessage(ErrDeptDev, DeptErrDeletingRelatedRecords, HfcResponse::RESPONSESTYLE_NOINPUT);


	//	Force a Config message to all Sessions (no super login required for one thing, others?).
////////////////////////////////////	dispatchConfigurationSaved();

	// Dispatch a message that all UI Sessions will be listening for in order to update the
	// soft keys on their displays.
	if (mgrContainer_)
	{
		EvolMessage msg(SoftkeyDev, LoadAllSoftkeyGroups);
		mgrContainer_->dispatchMessage(&msg);
	}
}


void DeptManager::handleDeleteAll()
{
	Debug::trace("DeptManager", "handleDeleteAll - deleting all depts");

	if (!dbAccess)
		return;

	DBDeptsRM *dbRM = dbAccess->getDBDeptsRM();
	if (!dbRM)
		return;

	destroyQueriesAndIters();

	bool traceOn = Debug::isTracing("DeptManager");
	try
	{
		DBQuery<DBDept>* allQuery = dbRM->retrieveAll();
		if (!allQuery)
			return;

		DBQuery<DBDept>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			HobRecordNum deptNum = Long::valueOf(iter->getNumber());
			if (traceOn)
				Debug::trace("DeptManager", string("handleDeleteAll - deptnum ") + iter->getNumber() + "  name " + iter->getName1());

			++iter;
			handleDelete(deptNum);
		}
		delete allQuery;
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "DeptManager::handleDeleteAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}


	Debug::trace("DeptManager", "handleDeleteAll - will create reserved records next");
	Dept::verifyReservedRecords(dbAccess);

	Configuration *config = Configuration::getInstance();
	if (config && config->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum) > 0)
	{
		config->updateField(fieldOffsetCfgMostRecentActiveDeptNum, "0");
		config->saveField(fieldOffsetCfgMostRecentActiveDeptNum, dbAccess);
	}
}


void DeptManager::handleUpdateAndSave(Dept *modifiedDept)
{
	Debug::trace("DeptManager", HobString("handleUpdateAndSave(): currentDept = ") + HobString(Hex::toString((long long) currentDept)));
	if (currentDept)
	{
		if (modifiedDept)
		  	currentDept->updateRecord(modifiedDept);
		handleSaveRecord(currentDept);
	}
}


void DeptManager::handleSaveRecord(Dept *recordToSave)
{
	Debug::trace("DeptManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
		recordToSave->saveRecord();

	SessionManager::getInstance()->verifyDeptBasedReservedRecords( (dbAccess) ? dbAccess : ( (mgrContainer_) ? mgrContainer_ : 0 ) );
}


void DeptManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Dept *restoreDept = dynamic_cast<Dept *> (msg->getHfcObject());
	if (!restoreDept || !dbAccess)
		return;

	restoreDept->setDBAccess(dbAccess);

	Dept *existingDept = Dept::findRecord(restoreDept->getDeptNum(), dbAccess);
	if (existingDept)
	{
		if (existingDept->isRecordValid())
		{
			existingDept->updateRecord(restoreDept);
			existingDept->saveRecord();
		}
		else
		{
			existingDept->deleteRecord();
			restoreDept->saveRecord();
		}

		delete existingDept;
		existingDept = 0;
	}
	else
		restoreDept->saveRecord();
}


void DeptManager::handleCreate(Dept *dept)
{
	if (!dept)
		return;

	Dept *newDept = new Dept();

	if (newDept)
	{
		newDept->setDeptNum(dept->getDeptNum());
		newDept->setDeptName1("");
		newDept->setDeptName2("");
		newDept->setDBAccess(dbAccess);
		handleSaveRecord(newDept);

		//	Set this new dept as the current dept.
		setCurrentDept(newDept);

		if (mySession)
		{
			// Send the new dept to the UI
			SessionMessage deptData(DeptDev, DtWrite);
			deptData.setFormatType(BrowserFormat);
			deptData.setHfcObject(currentDept);
			mySession->send(&deptData, Messenger::DEST_UI);
		}
	}
}


void DeptManager::destroyQueriesAndIters()
{
	if (allDeptsIter)
	{
		delete allDeptsIter;
		allDeptsIter = (DBQuery<DBDept>::const_iterator *)NULL;
	}

	if (allDeptsQuery)
	{
		delete allDeptsQuery;
		allDeptsQuery = (DBQuery<DBDept> *)NULL;
	}
}
