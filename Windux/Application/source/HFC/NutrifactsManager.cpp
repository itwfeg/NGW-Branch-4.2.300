/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NutrifactsManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NutrifactsManager::dataType = 0;
std::string NutrifactsManager::dataTypeName("NutrifactsManager"); 
#endif


NutrifactsManager::NutrifactsManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

NutrifactsManager::NutrifactsManager(hfc::HFCManagerContainer *container)
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

NutrifactsManager::~NutrifactsManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentNutrifacts)
		delete currentNutrifacts;

	destroyQueriesAndIters();
}


void NutrifactsManager::init()
{
	mySession = 0;

	currentNutrifacts = 0;
	
	allNutrifactsQuery = 0;
	allNutrifactsIter  = 0;
}


DBSession *NutrifactsManager::getMySession()
{
	return mySession;
}
void NutrifactsManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Nutrifacts *NutrifactsManager::firstNutrifacts()
{
	if (!dbAccess)
		return (Nutrifacts *)NULL;

	DBNutritionsRM *dbNutritionsRM = dbAccess->getDBNutritionsRM();
	if (!dbNutritionsRM)
		return (Nutrifacts *)NULL;

	// If the user calls firstNutrifacts() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allNutrifactsQuery = dbNutritionsRM->retrieveAll();

	// Couldn't get the query from which to access the Nutrifacts, just return false.
	if (!allNutrifactsQuery)
		return (Nutrifacts *)NULL;

	DBQuery<DBNutrition>::const_iterator tempIter = allNutrifactsQuery->begin();
	if (tempIter == allNutrifactsQuery->end())
		return (Nutrifacts *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextNutrifacts()
	 * method call below).
	 */
	allNutrifactsIter = new DBQuery<DBNutrition>::const_iterator(tempIter);
	DBNutrition *n = new DBNutrition(*(*allNutrifactsIter));
	Nutrifacts *nutri= new Nutrifacts(n, dbAccess);
	return nutri;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstNutrifacts() method, the first call to this method will
 * result in the first record being returned.
 */
Nutrifacts *NutrifactsManager::nextNutrifacts()
{
	if (!dbAccess)
		return (Nutrifacts *)NULL;

	if (allNutrifactsQuery && !allNutrifactsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allNutrifactsQuery || !allNutrifactsIter)
		return firstNutrifacts();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allNutrifactsIter);
	if (*allNutrifactsIter == allNutrifactsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Nutrifacts in the db. */
		destroyQueriesAndIters();

		return (Nutrifacts *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBNutrition copy constructor in order for the Hfc Nutrifacts to
	 * control the destruction of the DBNutrition record that the iterator contains; otherwise, the DBNutrition will
	 * be deleted when the iterator goes out of scope.  The DBNutrition copy will be deleted by the Nutrifacts record
	 * created below.
	 */
	DBNutrition *n = new DBNutrition(*(*allNutrifactsIter));
	Nutrifacts *nutri = new Nutrifacts(n, dbAccess);
	return nutri;
}


/* Provide a countAll method so the caller doesn't need to know about a dbNutritionsRM.
 */
int NutrifactsManager::countAllNutrifacts()
{
	if (!dbAccess)
		return -1;

	DBNutritionsRM *dbNutritionsRM = dbAccess->getDBNutritionsRM();
	if (!dbNutritionsRM)
		return -1;

	return dbNutritionsRM->countAll();
}


Nutrifacts *NutrifactsManager::getCurrentNutrifacts()
{
	return currentNutrifacts;
}
void NutrifactsManager::setCurrentNutrifacts(Nutrifacts *newNutrifacts)
{
	if (currentNutrifacts)
		delete currentNutrifacts; 
	currentNutrifacts = newNutrifacts;
}
void NutrifactsManager::setCurrentNutrifactsFromProduct(Nutrifacts *newNutrifacts)
{
	//	The Product will take care of deleting the Nutrifacts. Don't do it here.
	currentNutrifacts = newNutrifacts;
}


void NutrifactsManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

#if defined(LINUX_FIX)
    // msgNutrifacts: unused variable
#else
	Nutrifacts *msgNutrifacts = dynamic_cast<Nutrifacts *> (msg->getHfcObject());
#endif
	UINT8 event = msg->getEventType();
	Debug::trace("NutrifactsManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		default:
			Debug::trace("NutrifactsManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void NutrifactsManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Nutrifacts *msgNutrifacts = dynamic_cast<Nutrifacts *> (msg->getHfcObject());
	if (!msgNutrifacts || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Nutritions))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Nutrifacts *existingFacts = Nutrifacts::findRecord(msgNutrifacts->getTextNum(), msgNutrifacts->getDeptNum(), dbAccess);
		if (existingFacts)
		{
			existingFacts->deleteRecord();
			Debug::trace("NutrifactsManager", string("handleDelete - nutrifacts deleted - txnum:") + longtostring(msgNutrifacts->getTextNum()) + string("  deptNum:") + longtostring(msgNutrifacts->getDeptNum()) );
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Nutritions))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingFacts, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingFacts;
		}
		else
			Debug::trace("NutrifactsManager", HobString("handleDelete - nutrifacts to be deleted was not in the database to delete - txnum:") +
														HobString(Long::toString((long)msgNutrifacts->getTextNum())));
	}
}


void NutrifactsManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void NutrifactsManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("NutrifactsManager", "handleDeleteAll - deleting all Nutrifacts");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Nutritions))
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

		DBNutritionsRM *dbRM = dbAccess->getDBNutritionsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

#if 0
// This code existed when the SessionManager was responsible for handling the deleteAll in which we replaced the db file(s)
// for the record type with those from the empty database.  With the db's "zap" interface, this is no longer required.

	// Let the SessionManager know that this is to be done.
	if (mySession)
	{
		SessionMessage delAllMsg(NutrifactsDev, DtDeleteAll);
		delAllMsg.setFormatType(InternalFormat);
		mySession->send(&delAllMsg, Messenger::DEST_RTC);
	}

#endif
	}

	Debug::trace("NutrifactsManager", "handleDeleteAll - leaving");
}


void NutrifactsManager::handleSaveRecord(Nutrifacts *recordToSave)
{
	Debug::trace("NutrifactsManager", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Nutritions))
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


void NutrifactsManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Nutrifacts *restoreFacts = dynamic_cast<Nutrifacts *> (msg->getHfcObject());
	if (!restoreFacts || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Nutritions))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreFacts);

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
		restoreFacts->setDBAccess(dbAccess);

		Nutrifacts *existingFacts = Nutrifacts::findRecord(restoreFacts->getTextNum(), restoreFacts->getDeptNum(), dbAccess);
		if (existingFacts)
		{
			if (existingFacts->isRecordValid())
			{
				existingFacts->updateRecord(restoreFacts);
				existingFacts->saveRecord();								
			}
			else
			{
				existingFacts->deleteRecord();	   
				restoreFacts->saveRecord();
			}

			delete existingFacts;
		}
		else
			restoreFacts->saveRecord();
	}
}


void NutrifactsManager::destroyQueriesAndIters()
{
	if (allNutrifactsIter)
	{
		delete allNutrifactsIter;
		allNutrifactsIter = (DBQuery<DBNutrition>::const_iterator *)NULL;
	}

	if (allNutrifactsQuery)
	{
		delete allNutrifactsQuery;
		allNutrifactsQuery = (DBQuery<DBNutrition> *)NULL;
	}
}
