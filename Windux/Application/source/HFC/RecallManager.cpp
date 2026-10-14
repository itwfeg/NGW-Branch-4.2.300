/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "RecallManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "HfcUpdateStatus.h"
#include "AllRecalls.h"
#include "GenericHfcObject.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"

#include "MultiLanguageText.h"
#include "MessageLogger.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBRecallsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RecallManager::_dataType_ = 0;
std::string RecallManager::_dataTypeName_("RecallManager"); 
#endif


RecallManager::RecallManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

RecallManager::RecallManager(hfc::HFCManagerContainer *container)
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


RecallManager::~RecallManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();
}


void RecallManager::init()
{
	mySession = 0;

	currentRecall = 0;

	allRecallsQuery = 0;
	allRecallsIter  = 0;
}


DBSession *RecallManager::getMySession()
{
	return mySession;
}
void RecallManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Recall *RecallManager::firstRecall()
{
	if (!dbAccess)
		return (Recall *)NULL;

	DBRecallsRM *dbRecallsRM = dbAccess->getDBRecallsRM();
	if (!dbRecallsRM)
		return (Recall *)NULL;

	// If the user calls firstRecall() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allRecallsQuery = dbRecallsRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allRecallsQuery)
		return (Recall *)NULL;

	DBQuery<DBRecall>::const_iterator tempIter = allRecallsQuery->begin();
	if (tempIter == allRecallsQuery->end())
		return (Recall *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextRecall()
	 * method call below).
	 */
	allRecallsIter = new DBQuery<DBRecall>::const_iterator(tempIter);
	DBRecall *c = new DBRecall(*(*allRecallsIter));
	Recall *newRecall = new Recall(c, dbAccess);
	return newRecall;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstRecall() method, the first call to this method will
 * result in the first record being returned.
 */
Recall *RecallManager::nextRecall()
{
	if (!dbAccess)
		return (Recall *)NULL;

	if (allRecallsQuery && !allRecallsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allRecallsQuery || !allRecallsIter)
		return firstRecall();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allRecallsIter);
	if (*allRecallsIter == allRecallsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Recalls in the db. */
		destroyQueriesAndIters();
		return (Recall *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBRecall copy constructor in order for the Hfc Recall to
	 * control the destruction of the DBRecall record that the iterator contains; otherwise, the DBRecall will
	 * be deleted when the iterator goes out of scope.  The DBRecall copy will be deleted by the Recall record
	 * created below.
	 */
	DBRecall *c = new DBRecall(*(*allRecallsIter));
	Recall *newRecall = new Recall(c, dbAccess);
	return newRecall;
}


/* Provide a countAll method so the caller doesn't need to know about a dbRecallsRM.
 */
int RecallManager::countAllRecalls()
{
	if (!dbAccess)
		return -1;

	DBRecallsRM *dbRecallsRM = dbAccess->getDBRecallsRM();
	if (!dbRecallsRM)
		return -1;

	return dbRecallsRM->countAll();
}


Recall *RecallManager::getCurrentRecall()
{
	return currentRecall;
}
void RecallManager::setCurrentRecall(Recall *newRecall)
{
	if (currentRecall)
		delete currentRecall;
	currentRecall = newRecall;
}


void RecallManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	Recall *msgRecall = dynamic_cast<Recall *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("RecallManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgRecall);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgRecall);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgRecall);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgRecall);
			break;

		case AcknowledgeRecall:
			handleAcknowledgeRecall(msgRecall);
			break;

		case CheckForRecalls:
			handleCheckForRecalls();
			break;

		default:
			Debug::trace("RecallManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void RecallManager::handleRead(Recall *msgRecall)
{
	if (!msgRecall || !dbAccess)
		return;

	Recall *lookedUpRecall = Recall::findRecall(msgRecall->getRecallName(), dbAccess);

	if (!lookedUpRecall || lookedUpRecall->getErrorCode() != RECORD_OK)		//	This recall does not exist
	{
			// Send error message to browser indicating Recall not found.
		if (mySession) 
			mySession->addDialogMessage(ErrRecallDev, RecallErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpRecall)
			delete lookedUpRecall;
		return;
	}

	// We have a new current Recall.
	setCurrentRecall(lookedUpRecall);
	Debug::trace("RecallManager.Recall", HobString("handleRead(): currentRecall = ") + HobString(Hex::toString((long long) currentRecall)));

	if (mySession)
	{
		// Send the Recall found back to the Browser.
		// Is this the behavior for ALL RecallRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a Recall, but doesn't want the data sent back?
		SessionMessage recallData(RecallDev, DtWrite);
		recallData.setFormatType(BrowserFormat);
		recallData.setHfcObject(currentRecall);

		// Send the Recall to the UI (Browser)
		mySession->send(&recallData, Messenger::DEST_UI);
	}
}


void RecallManager::handleReadAll()
{
	Debug::trace("RecallManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	AllRecalls recallList;
	recallList.findAll(dbAccess);

	// Send the recalls list to the UI (Browser)
	SessionMessage recallMsg(RecallDev, DtWriteAll);
	recallMsg.setFormatType(BrowserFormat);
	recallMsg.setHfcObject(&recallList);
	mySession->send(&recallMsg, Messenger::DEST_UI);
}


void RecallManager::handleWrite(Recall *modifiedRecall)
{
	if ((!modifiedRecall) || (!currentRecall))
		return;

	if (Debug::isTracing("RecallManager"))
		Debug::trace("RecallManager", HobString("handleWrite(): currentRecall = ") + HobString(Hex::toString((long long) currentRecall)) +
									  HobString("  , modifiedRecall = ") + HobString(Hex::toString((long long) modifiedRecall)));

	currentRecall->clearErrors();
  	currentRecall->updateRecall(modifiedRecall);
	if (mySession && Configuration::getInstance()->toBoolCfgI(CfgInternal::NOTIFYUIOFUPDATESTATUS))
	{
		SessionMessage updateStatusMsg(RecallDev, DtUpdateStatus);
		updateStatusMsg.setFormatType(BrowserFormat);
		HfcUpdateStatus stat(!currentRecall->hasErrors());
		updateStatusMsg.setHfcObject(&stat);
		mySession->send(&updateStatusMsg, Messenger::DEST_UI);
	}
}


void RecallManager::handleDelete(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	if (mySession->isRemoteSession())
	{
		Debug::trace("RecallManager", "handleAcknowledgeRecall(): Remote Console Sessions can't delete recalls!");
		return;
	}

	Recall *msgRecall = dynamic_cast<Recall *> (msg->getHfcObject());
	if (!msgRecall || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Recall *existingRecall = Recall::findRecall(msgRecall->getRecallName(), dbAccess);
		if (existingRecall)
		{
			existingRecall->deleteRecord();
			Debug::trace("RecallManager", "handleDelete - Recall deleted - recallName: " + msgRecall->getRecallName());
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				if (mySession && !mySession->isCommSession())
				{
					//	Create a blank recall
					Recall *newRecall = createNewRecall(0);
					// Make it the current Recall.
					setCurrentRecall(newRecall);
					//	Send the blank recall to the browser
					sendCurrentRecall();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingRecall, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingRecall;
		}
		else
			Debug::trace("RecallManager", "handleDelete - Recall to be deleted was not in the database to delete - recallName: " + msgRecall->getRecallName());
	}
}


void RecallManager::handleDeleteAll()
{
	Debug::trace("RecallManager", "handleDeleteAll - deleting all recalls");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
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

		DBRecallsRM *dbRecallsRM = dbAccess->getDBRecallsRM();
		if (!dbRecallsRM)
			return;

		dbRecallsRM->deleteAll();
		destroyQueriesAndIters();

		Debug::trace("RecallManager", "handleDeleteAll - will create reserved records next");
	}

	Debug::trace("RecallManager", "handleDeleteAll - leaving");
}


void RecallManager::handleUpdateAndSave(Recall *modifiedRecall)
{
	Debug::trace("RecallManager", HobString("handleUpdateAndSave(): currentRecall = ") + HobString(Hex::toString((long long) currentRecall)));
	if (currentRecall)
	{
		handleWrite(modifiedRecall);
		handleSaveRecord(currentRecall);
	}
}

void RecallManager::handleSaveRecord(Recall *recordToSave)
{
	Debug::trace("RecallManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave, getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			
			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void RecallManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Recall *restoreRecall = dynamic_cast<Recall *> (msg->getHfcObject());
	if (!restoreRecall || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreRecall->setDBAccess(dbAccess);

		Recall *usedRecall = (Recall *)NULL;
		Recall *existingRecall = Recall::findRecall(restoreRecall->getRecallName(), dbAccess);
		if (existingRecall)
		{
			if (existingRecall->isRecordValid())
			{
				existingRecall->updateRecall(restoreRecall);
				existingRecall->saveRecord();
				usedRecall = existingRecall;								
			}
			else
			{
				existingRecall->deleteRecord();	   
				restoreRecall->saveRecord();
				usedRecall = restoreRecall;								
			}
		}
		else
		{
			restoreRecall->saveRecord();
			usedRecall = restoreRecall;								
		}

		if (usedRecall && usedRecall->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedRecall->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedRecall->nextError();
			}
		}
		if (existingRecall)
			delete existingRecall;
	}
}


void RecallManager::handleCreate(Recall *msgRecall)
{
	if (!msgRecall)
		return;

	Recall *newRecall = createNewRecall(msgRecall->getRecallName());
	if (newRecall)
	{
		//	Set this new recall as the current recall.
		setCurrentRecall(newRecall);

		if (mySession)
		{
			// Send the new recall to the Browser.
			SessionMessage recallData(RecallDev, DtWrite);
			recallData.setFormatType(BrowserFormat);
			recallData.setHfcObject(currentRecall);

			mySession->send(&recallData, Messenger::DEST_UI);
		}
	}
}

Recall *RecallManager::createNewRecall(std::string recordName)
{
	Recall *newRecall = (Recall *) NULL;
	if (mySession)
	{
		// Send the current new Recall to the browser.
		newRecall = new Recall();
		if (newRecall)
		{
			newRecall->setDBAccess(dbAccess);
			newRecall->setRecallName(recordName);
		}
	}
	return newRecall;
}

void RecallManager::sendCurrentRecall()
{
	if (!currentRecall || !mySession)
	{
		Debug::trace("RecallManager", "sendCurrentRecall(): currentRecall or mySession is NULL!");
		return;
	}
	SessionMessage recallMsg(RecallDev, DtWrite);
	recallMsg.setFormatType(BrowserFormat);
	recallMsg.setHfcObject(currentRecall);
	mySession->send(&recallMsg, Messenger::DEST_UI);
}


void RecallManager::handleAcknowledgeRecall(Recall *msgRecall)
{
	if (!msgRecall || !mySession)
		return;

	if (!mySession->isPrimarySession())
	{
		Debug::trace("RecallManager", "handleAcknowledgeRecall(): Not the Primary Console Session, can't mark recall as acknowledged!");
		return;
	}

	Recall *existingRecall = Recall::findRecall(msgRecall->getRecallName(), dbAccess);
	if (existingRecall)
	{
		hfc::MultiLanguageText internationalize;
		MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("recallAcknowledged") + "  " + existingRecall->getReceivedDateTime().convertToDBFormat() + "  " + existingRecall->getRecallText());

		existingRecall->setAcknowledged(true);
		existingRecall->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Recalls))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, existingRecall, getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			
			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}

		delete existingRecall;

		SessionMessage msg(RecallDev, CheckForRecalls);
		msg.setFormatType(InternalFormat);
		mySession->send(&msg, Messenger::DEST_RTC);

		// Sending messages internally (DEST_RTC) won't send the message to the thread sending the message,
		// so I also have mySession handle the same message that I just dispatched directly above.
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
			s->handleMessage(&msg);
	}
	else
		Debug::trace("RecallManager", "handleAcknowledgeRecall - Recall to be acknowledged was not in the database - recallName: " + msgRecall->getRecallName());
}

void RecallManager::handleCheckForRecalls()
{
	if (!mySession)
		return;

	bool haveUnacknowledged = false;

	DBRecallsRM *dbRecallsRM = dbAccess->getDBRecallsRM();
	if (!dbRecallsRM)
		return;

	DBQuery<edb::DBRecall> *unackQuery = dbRecallsRM->retrieveUnacknowledged();
	if (!unackQuery)
		return;

	DBQuery<DBRecall>::const_iterator tempIter = unackQuery->begin();
	if (tempIter != unackQuery->end())
		haveUnacknowledged = true;

	delete unackQuery;

	SessionMessage recallMsg(RecallDev, NewRecallsExist);
	recallMsg.setFormatType(BrowserFormat);
	GenericHfcObject gho((haveUnacknowledged) ? "1" : "0");
	recallMsg.setHfcObject(&gho);
	mySession->send(&recallMsg, Messenger::DEST_UI);
}

void RecallManager::destroyQueriesAndIters()
{
	if (allRecallsIter)
	{
		delete allRecallsIter;
		allRecallsIter = (DBQuery<DBRecall>::const_iterator *)NULL;
	}

	if (allRecallsQuery)
	{
		delete allRecallsQuery;
		allRecallsQuery = (DBQuery<DBRecall> *)NULL;
	}
}
