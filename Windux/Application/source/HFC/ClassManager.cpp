/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ClassManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBClassesRM.h"

#include "ClassTree.h"
#include "HfcErrorClass.h"
#include "ResponseMessageManager.h"
#include "HobDateTime.h"
#include "Configuration.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ClassManager::dataType = 0;
std::string ClassManager::dataTypeName("ClassManager"); 
#endif


ClassManager::ClassManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

ClassManager::ClassManager(hfc::HFCManagerContainer *container)
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

ClassManager::~ClassManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentClass)
		delete currentClass;
		
	destroyQueriesAndIters();
}


void ClassManager::init()
{
	mySession = 0;

	currentClass = 0;

	allClassesQuery = 0;
	allClassesIter  = 0;
}


DBSession *ClassManager::getMySession()
{
	return mySession;
}
void ClassManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Class *ClassManager::firstClass()
{
	if (!dbAccess)
		return (Class *)NULL;

	DBClassesRM *dbClassesRM = dbAccess->getDBClassesRM();
	if (!dbClassesRM)
		return (Class *)NULL;

	// If the user calls firstClass() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allClassesQuery = dbClassesRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allClassesQuery)
		return (Class *)NULL;

	DBQuery<DBClass>::const_iterator tempIter = allClassesQuery->begin();
	if (tempIter == allClassesQuery->end())
		return (Class *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextClass()
	 * method call below).
	 */
	allClassesIter = new DBQuery<DBClass>::const_iterator(tempIter);
	DBClass *c = new DBClass(*(*allClassesIter));
	Class *newClass = new Class(c, dbAccess);
	return newClass;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstClass() method, the first call to this method will
 * result in the first record being returned.
 */
Class *ClassManager::nextClass()
{
	if (!dbAccess)
		return (Class *)NULL;

	if (allClassesQuery && !allClassesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allClassesQuery || !allClassesIter)
		return firstClass();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allClassesIter);
	if (*allClassesIter == allClassesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Classes in the db. */
		destroyQueriesAndIters();
		return (Class *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBClass copy constructor in order for the Hfc Class to
	 * control the destruction of the DBClass record that the iterator contains; otherwise, the DBClass will
	 * be deleted when the iterator goes out of scope.  The DBClass copy will be deleted by the Class record
	 * created below.
	 */
	DBClass *c = new DBClass(*(*allClassesIter));
	Class *newClass = new Class(c, dbAccess);
	return newClass;
}


/* Provide a countAll method so the caller doesn't need to know about a dbClassesRM.
 */
int ClassManager::countAllClasses()
{
	if (!dbAccess)
		return -1;

	DBClassesRM *dbClassesRM = dbAccess->getDBClassesRM();
	if (!dbClassesRM)
		return -1;

	return dbClassesRM->countAll();
}


Class *ClassManager::getCurrentClass()
{
	return currentClass;
}
void ClassManager::setCurrentClass(Class *newClass)
{
	if (currentClass)
		delete currentClass;
	currentClass = newClass;
}


void ClassManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	Class *msgClass = dynamic_cast<Class *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("ClassManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgClass);
			break;

		case DtWrite:
			handleWrite(msgClass);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgClass);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleClassCreateMessage(msgClass);
			break;

		case ClassGetTree:
		{
			HobDateTime t1;
			bool isTiming = Debug::isTracing("ClassManager.ClassTreeTiming");

			if (isTiming)
			{
				t1.setToCurrent();
				HobString startTime = t1.formatDate();
				startTime.append(t1.formatTime(FormatTime_HHMMSS));
				startTime.append("");
				Debug::trace("ClassManager.ClassTreeTiming", HobString("handleMessage() - before handleGetClassTreeMessage() - time: ") + startTime);
			}

			handleGetClassTreeMessage();

			if (isTiming)
			{
				t1.setToCurrent();
				HobString endTime = t1.formatDate();
				endTime.append(t1.formatTime(FormatTime_HHMMSS));
				endTime.append("");
				Debug::trace("ClassManager.ClassTreeTiming", HobString("handleMessage() - after handleGetClassTreeMessage() - time: ") + endTime);
			}
			break;
		}

		default:
			Debug::trace("ClassManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void ClassManager::handleRead(Class *msgClass)
{
	if (!msgClass || !dbAccess)
		return;

	if (msgClass->getClassNum() == (HobRecordNum)RECORD_NOT_FOUND)
	{
		setCurrentClass((Class *)NULL);
		return;
	}

	Class *lookedUpClass = Class::findRecord(msgClass->getClassNum(), msgClass->getDeptNum(),  dbAccess);

	if (!lookedUpClass || lookedUpClass->getErrorCode() != RECORD_OK)
	{
		// Send error message to browser indicating Class not found.
		if (mySession)
			mySession->addDialogMessage(ErrClassDev, ClassErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpClass)
			delete lookedUpClass;
		return;
	}

	// We have a new current class.
	setCurrentClass(lookedUpClass);

	Debug::trace("ClassManager.Class", "handleClassReadMessage(): currentClass = " + ptrtostring(currentClass));

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL ClassRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a Class, but doesn't want the data sent back?
		SessionMessage classData(ClassDev, DtWrite);
		classData.setFormatType(BrowserFormat);
		classData.setHfcObject(currentClass);

		// Send the product to the UI (Browser)
		mySession->send(&classData, Messenger::DEST_UI);
	}
}


void ClassManager::handleWrite(Class *modifiedClass)
{
	if ((!modifiedClass) || (!currentClass))
		return;

	Debug::trace("ClassManager", "handleClassWriteMessage(): currentClass = " + ptrtostring(currentClass) +
			"  , modifiedClass = " + ptrtostring(modifiedClass));

  	currentClass->updateRecord(modifiedClass);
}


void ClassManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Class *msgClass = dynamic_cast<Class *> (msg->getHfcObject());
	if (!msgClass || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Classes))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Class *existingClass = Class::findRecord(msgClass->getClassNum(), msgClass->getDeptNum(), dbAccess);
		if (existingClass)
		{
			if (!existingClass->isReservedClass())
			{
				existingClass->deleteRecord();
				Debug::trace("ClassManager", HobString("handleDelete - class deleted - classNum:") + longtostring(msgClass->getClassNum()) + HobString("  deptNum:") + longtostring(msgClass->getDeptNum()) );
				if (msg->getEventType() != DtBatchApplyDelete)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Classes))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, existingClass, getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
						
						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
					}
				}
			}
			else
				Debug::trace("ClassManager", HobString("handleDelete - can't delete a reserved class - classNum:") +
																HobString(Long::toString((long)msgClass->getClassNum())));

			delete existingClass;
		}
		else
			Debug::trace("ClassManager", HobString("handleDelete - class to be deleted was not in the database to delete - classNum:") +
															HobString(Long::toString((long)msgClass->getClassNum())));
	}
}


void ClassManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void ClassManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("ClassManager", "handleDeleteAll - deleting all classes");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Classes))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(getDevice(), getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBClassesRM *dbRM = dbAccess->getDBClassesRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		Debug::trace("ClassManager", "handleDeleteAll - will create reserved records next");
		Class::verifyReservedRecords(dbAccess);
	}

	Debug::trace("ClassManager", "handleDeleteAll - leaving");
}


void ClassManager::handleUpdateAndSave(Class *modifiedClass)
{
	Debug::trace("ClassManager", "handleUpdateAndSave(): currentClass = " + ptrtostring(currentClass));

	if (currentClass)
	{
		if (modifiedClass)
		  	currentClass->updateRecord(modifiedClass);
		handleSaveRecord(currentClass);
	}
}

void ClassManager::handleSaveRecord(Class *recordToSave)
{
	Debug::trace("ClassManager", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Classes))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			
			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void ClassManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Class *restoreClass = dynamic_cast<Class *> (msg->getHfcObject());
	if (!restoreClass || !dbAccess)
		return;

//		Debug::trace("ClassManager", "restoreClass - attempting to restore class " + HobString(Long::toString(restoreClass->getClassNum())));

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Classes))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreClass);

			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
		}
		else
			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

		// batch the record being restored; don't actually write it to the database right now.
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreClass->setDBAccess(dbAccess);

		Class *existingClass = Class::findRecord(restoreClass->getClassNum(), restoreClass->getDeptNum(), dbAccess);
		if (existingClass)
		{
	//			Debug::trace("ClassManager", "restoreClass - class already existed");
			if (existingClass->isRecordValid())
			{
				existingClass->updateRecord(restoreClass);
				existingClass->saveRecord();								
				Debug::trace("ClassManager", "restoreClass - saved updated class");
			}
			else
			{
				existingClass->deleteRecord();	   
				restoreClass->saveRecord();
			}

			delete existingClass;
		}
		else
		{
	//			Debug::trace("ClassManager", "restoreClass - did not find class, going to save");
			restoreClass->saveRecord();
			Debug::trace("ClassManager", "restoreClass - created and saved class");
		}
	}
}



void ClassManager::handleGetClassTreeMessage()
{
	if (!dbAccess)
		return;

	ClassTree *tree = new ClassTree(dbAccess);
	if (tree)
	{
		if (mySession)
		{
			tree->setDeptNum( Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
			tree->buildClassTree();		//	Build the Class tree list.

			// Send the Class tree to the browser.
			SessionMessage treeMsg(ClassDev, ClassGetTree);
			treeMsg.setFormatType(BrowserFormat);
			treeMsg.setHfcObject(tree);

			// Send the error message to the UI (Browser)
			mySession->send(&treeMsg, Messenger::DEST_UI);
		}
		delete tree;
	}
}

void ClassManager::handleClassCreateMessage(Class *createClass)
{
	if (!dbAccess)
		return;

	//	Create a new Class.
	Class *newClass = new Class();
	if (newClass)
	{
		newClass->setDeptNum(createClass->getDeptNum());
		newClass->setClassNum(createClass->getClassNum());
		newClass->setErrorCode(RECORD_OK);
		newClass->setDBAccess(dbAccess);

		if (createClass->isReceived(fieldOffsetReportingClassNum))
			newClass->setReportingClassNum(createClass->getReportingClassNum());

		setCurrentClass(newClass);

		// Send the new Class to the Browser.
		SessionMessage classData(ClassDev, DtWrite);
		classData.setFormatType(BrowserFormat);
		classData.setHfcObject(currentClass);

		mySession->send(&classData, Messenger::DEST_UI);

		handleSaveRecord(currentClass);
	}
}

void ClassManager::destroyQueriesAndIters()
{
	if (allClassesIter)
	{
		delete allClassesIter;
		allClassesIter = (DBQuery<DBClass>::const_iterator *)NULL;
	}

	if (allClassesQuery)
	{
		delete allClassesQuery;
		allClassesQuery = (DBQuery<DBClass> *)NULL;
	}
}
