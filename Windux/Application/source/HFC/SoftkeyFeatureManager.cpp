/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyFeatureManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcError.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Softkey.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyFeatureManager::dataType = 0;
std::string SoftkeyFeatureManager::dataTypeName("SoftkeyFeatureManager"); 
#endif


SoftkeyFeatureManager::SoftkeyFeatureManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SoftkeyFeatureManager::SoftkeyFeatureManager(hfc::HFCManagerContainer *container)
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

SoftkeyFeatureManager::~SoftkeyFeatureManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	/* This is now done only in the SoftkeyManager
	if (restoreTrackingFlag && mySession)
	{
		//	Force a Config message to all Sessions (no super login required).
		SessionMessage msg(SoftkeyDev, LoadAllSoftkeyGroups);
		msg.setFormatType(InternalFormat);
		mySession->send(&msg, Messenger::DEST_RTC);
	} */

	if (currentSoftkeyFeature)
		delete currentSoftkeyFeature;

	destroyQueriesAndIters();
}


void SoftkeyFeatureManager::init()
{
	mySession = 0;

	currentSoftkeyFeature = 0;
	allSoftkeyFeaturesQuery = 0;
	allSoftkeyFeaturesIter  = 0;

//	restoreTrackingFlag = false;
}


DBSession *SoftkeyFeatureManager::getMySession()
{
	return mySession;
}
void SoftkeyFeatureManager::setMySession(DBSession *session)
{
	mySession = session;
}


void SoftkeyFeatureManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();

	/* This is now only done in the SoftkeyManager
	if (mySession && mySession->isCommSession() && event == RestoreSoftkeyFeature)
		restoreTrackingFlag = true; */

	SoftkeyFeature *msgSoftkeyFeature = dynamic_cast<SoftkeyFeature *> (msg->getHfcObject());

	bool traceOn = Debug::isTracing("SoftkeyFeatureManager");
	if (traceOn)
	{
		HobString myDebug = "handleMessage() - SoftkeyFeature event: ";
		myDebug.append(Integer::toString(event));
		Debug::trace("SoftkeyFeatureManager", myDebug);
	}

	switch (event)
	{
		case ReadSoftkeyFeature:
			handleRead(msgSoftkeyFeature);
			break;

		case ReadAllSoftkeyFeatures:
			handleReadAll();
			break;

		case WriteSoftkeyFeature:
			handleWrite(msgSoftkeyFeature);
			break;

		case DeleteSoftkeyFeature:
		case BatchApplyDeleteSoftkeyFeature:
			handleDelete(msg);
			break;

		case DeleteAllSoftkeyFeatures:
			handleDeleteAll();
			break;

		case SaveSoftkeyFeature:
			handleUpdateAndSave(msgSoftkeyFeature);
			break;

		case RestoreSoftkeyFeature:
			handleRestore(msg);
			break;

		case CreateNewSoftkeyFeature:
			handleCreate(msgSoftkeyFeature);
			break;

		default:
			if (traceOn)
			{
				HobString myDebug = "handleMessage():  UNKNOWN SoftkeyFeature EVENT: ";
				myDebug.append(Integer::toString(event));
				Debug::trace("SoftkeyFeatureManager", myDebug);
			}
			break;
	}
}


/* If the first record was not read, this method will return NULL.
 */
SoftkeyFeature *SoftkeyFeatureManager::firstSoftkeyFeature()
{
	if (!dbAccess)
		return (SoftkeyFeature *)NULL;

	DBSoftKeyFeaturesRM *dbSoftkeyFeaturesRM = dbAccess->getDBSoftKeyFeaturesRM();
	if (!dbSoftkeyFeaturesRM)
		return (SoftkeyFeature *)NULL;

	// If the user calls firstSoftkeyFeature() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allSoftkeyFeaturesQuery = dbSoftkeyFeaturesRM->retrieveAll();

	// Couldn't get the query from which to access the SoftkeyFeatures, just return false.
	if (!allSoftkeyFeaturesQuery)
		return (SoftkeyFeature *)NULL;

	DBQuery<DBSoftKeyFeature>::const_iterator tempIter = allSoftkeyFeaturesQuery->begin();
	if (tempIter == allSoftkeyFeaturesQuery->end())
		return (SoftkeyFeature *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextSoftkeyFeature()
	 * method call below).
	 */
	allSoftkeyFeaturesIter = new DBQuery<DBSoftKeyFeature>::const_iterator(tempIter);
	DBSoftKeyFeature *c = new DBSoftKeyFeature(*(*allSoftkeyFeaturesIter));
	SoftkeyFeature *newSoftkeyFeature = new SoftkeyFeature(c, dbAccess);
	return newSoftkeyFeature;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstSoftkeyFeature() method, the first call to this method will
 * result in the first record being returned.
 */
SoftkeyFeature *SoftkeyFeatureManager::nextSoftkeyFeature()
{
	if (!dbAccess)
		return (SoftkeyFeature *)NULL;

	if (allSoftkeyFeaturesQuery && !allSoftkeyFeaturesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allSoftkeyFeaturesQuery || !allSoftkeyFeaturesIter)
		return firstSoftkeyFeature();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allSoftkeyFeaturesIter);
	if (*allSoftkeyFeaturesIter == allSoftkeyFeaturesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all SoftkeyFeatures in the db. */
		destroyQueriesAndIters();
		return (SoftkeyFeature *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBSoftKeyFeature copy constructor in order for the Hfc SoftkeyFeature to
	 * control the destruction of the DBSoftKeyFeature record that the iterator contains; otherwise, the DBSoftKeyFeature will
	 * be deleted when the iterator goes out of scope.  The DBSoftKeyFeature copy will be deleted by the SoftkeyFeature record
	 * created below.
	 */
	DBSoftKeyFeature *c = new DBSoftKeyFeature(*(*allSoftkeyFeaturesIter));
	SoftkeyFeature *newSoftkeyFeature = new SoftkeyFeature(c, dbAccess);
	return newSoftkeyFeature;
}


/* Provide a countAll method so the caller doesn't need to know about a dbSoftkeyFeaturesRM.
 */
int SoftkeyFeatureManager::countAllSoftkeyFeatures()
{
	if (!dbAccess)
		return -1;

	DBSoftKeyFeaturesRM *dbSoftkeyFeaturesRM = dbAccess->getDBSoftKeyFeaturesRM();
	if (!dbSoftkeyFeaturesRM)
		return -1;

	return dbSoftkeyFeaturesRM->countAll();
}


SoftkeyFeature *SoftkeyFeatureManager::getCurrentSoftkeyFeature()
{
	return currentSoftkeyFeature;
}
void SoftkeyFeatureManager::setCurrentSoftkeyFeature(SoftkeyFeature *newSoftkeyFeature)
{
	if (currentSoftkeyFeature)
		delete currentSoftkeyFeature;
	currentSoftkeyFeature = newSoftkeyFeature;
}


void SoftkeyFeatureManager::handleRead(SoftkeyFeature *msgSoftkeyFeature)
{
	if (!mySession || !msgSoftkeyFeature || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	HobRecordNum test_long_dept = msgSoftkeyFeature->getDeptNum();
	SoftkeyFeature *readSoftkeyFeature = SoftkeyFeature::find(msgSoftkeyFeature->getFeatureNumber(), msgSoftkeyFeature->getDeptNum(), dbAccess);
	if (!readSoftkeyFeature)		//	This softkeyFeature does not exist
	{
		// Send error message to browser indicating SoftkeyFeature not found.
		if (s->isInSupervisor())
			mySession->addDialogMessage(ErrSoftkeyDev, SoftkeyFeatureNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		return;
	}

	// We have a new current SoftkeyFeature.
	setCurrentSoftkeyFeature(readSoftkeyFeature);
#if 0
	if (Debug::isTracing("SoftkeyFeatureManager"))
	{
		HobString myDebug = "handleReadSoftkeyFeature(): currentSoftkeyFeature = ";
		myDebug.append(Hex::toString((long int) currentSoftkeyFeature));
		Debug::trace("SoftkeyFeatureManager", myDebug);
	}
#endif

	// Send the SoftkeyFeature found back to the Browser.
	// Is this the behavior for ALL SoftkeyFeatureRead messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a SoftkeyFeature, but doesn't want the data sent back?
	SessionMessage featureData(SoftkeyDev, WriteSoftkeyFeature);
	featureData.setFormatType(BrowserFormat);
	featureData.setHfcObject(currentSoftkeyFeature);

	// Send the SoftkeyFeature to the UI (Browser)
	mySession->send(&featureData, Messenger::DEST_UI);
}


void SoftkeyFeatureManager::handleReadAll()
{
	Debug::trace("SoftkeyFeatureManager", "Unable to handle a ReadAllSoftkeyFeatures event right now!");
}


void SoftkeyFeatureManager::handleWrite(SoftkeyFeature *modifiedSoftkeyFeature)
{
	if ((!modifiedSoftkeyFeature) || (!currentSoftkeyFeature))
		return;

#if 0
	if (Debug::isTracing("SoftkeyFeatureManager"))
	{
		HobString myDebug = "handleWriteSoftkeyFeature(): currentSoftkeyFeature = ";
		myDebug.append(Hex::toString((long int) currentSoftkeyFeature)).append("  , modifiedSoftkeyFeature = ").append(Hex::toString((long int) modifiedSoftkeyFeature));
		Debug::trace("SoftkeyFeatureManager", myDebug);
	}
#endif
  	currentSoftkeyFeature->update(modifiedSoftkeyFeature);
}


void SoftkeyFeatureManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyFeature *msgSoftkeyFeature = dynamic_cast<SoftkeyFeature *> (msg->getHfcObject());
	if (!msgSoftkeyFeature || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyFeatures))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobRecordNum test_long_dept = msgSoftkeyFeature->getDeptNum();
		SoftkeyFeature *existingSoftkeyFeature = SoftkeyFeature::find(msgSoftkeyFeature->getFeatureNumber(), msgSoftkeyFeature->getDeptNum(), dbAccess);
		if (existingSoftkeyFeature)
		{
			existingSoftkeyFeature->deleteRecord();
#if 0
			if (Debug::isTracing("SoftkeyFeatureManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyFeature - SoftkeyFeature deleted - featureNum: ";
				myDebug.append(Long::toString(msgSoftkeyFeature->getFeatureNumber()));
				Debug::trace("SoftkeyFeatureManager", myDebug);
			}
#endif

			if (msg->getEventType() != BatchApplyDeleteSoftkeyFeature)
			{
				if (mySession && !mySession->isCommSession())
				{
					//	Create a blank category
					SoftkeyFeature *newSoftkeyFeature = createNewSoftkeyFeature(0);
					// Make it the current SoftkeyFeature.
					setCurrentSoftkeyFeature(newSoftkeyFeature);
					//	Send the blank category to the browser
					sendCurrentSoftkeyFeature();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyFeatures))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingSoftkeyFeature, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingSoftkeyFeature;
		}
		else
		{
#if 0
			if (Debug::isTracing("SoftkeyFeatureManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyFeature - SoftkeyFeature to be deleted was not found in the database - featureNum: ";
				myDebug.append(Long::toString(msgSoftkeyFeature->getFeatureNumber()));
				Debug::trace("SoftkeyFeatureManager", myDebug);
			}
#endif
		}
	}
}


void SoftkeyFeatureManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void SoftkeyFeatureManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("SoftkeyFeatureManager", "handleDeleteAllSoftkeyFeatures - deleting all SoftkeyFeatures");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyFeatures))
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

		DBSoftKeyFeaturesRM *dbRM = dbAccess->getDBSoftKeyFeaturesRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		Debug::trace("SoftkeyFeatureManager", "handleDeleteAll - will create reserved records next");
		SoftkeyFeature::verifyReservedRecords(dbAccess);
	}

	Debug::trace("SoftkeyFeatureManager", "handleDeleteAllSoftkeyFeatures - leaving");
}


void SoftkeyFeatureManager::handleUpdateAndSave(SoftkeyFeature *modifiedSoftkeyFeature)
{
	Debug::trace("SoftkeyFeatureManager", HobString("handleUpdateAndSave(): currentSoftkeyFeature = ") + HobString(Hex::toString((long long) currentSoftkeyFeature)));
	if (currentSoftkeyFeature)
	{
		if (modifiedSoftkeyFeature)
		  	currentSoftkeyFeature->update(modifiedSoftkeyFeature);
		handleSaveRecord(currentSoftkeyFeature);
	}
}

void SoftkeyFeatureManager::handleSaveRecord(SoftkeyFeature *recordToSave)
{
#if 0
	if (Debug::isTracing("SoftkeyFeatureManager"))
	{
		HobString myDebug = "handleSaveSoftkeyFeature(): currentSoftkeyFeature = ";
		myDebug.append(Hex::toString((long int) currentSoftkeyFeature));
		Debug::trace("SoftkeyFeatureManager", myDebug);
	}
#endif
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyFeatures))
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


void SoftkeyFeatureManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyFeature *restoreSoftkeyFeature = dynamic_cast<SoftkeyFeature *> (msg->getHfcObject());
	if (!restoreSoftkeyFeature || !dbAccess)
		return;

	if (restoreSoftkeyFeature->hasErrors())
	{
		if (mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = restoreSoftkeyFeature->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = restoreSoftkeyFeature->nextError();
			}
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyFeatures))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreSoftkeyFeature->setDBAccess(dbAccess);

		SoftkeyFeature *usedSoftkeyFeature = (SoftkeyFeature *)NULL;
		HobRecordNum test_long_dept = restoreSoftkeyFeature->getDeptNum();
		SoftkeyFeature *existingSoftkeyFeature = SoftkeyFeature::find(restoreSoftkeyFeature->getFeatureNumber(), restoreSoftkeyFeature->getDeptNum(), dbAccess);
		if (existingSoftkeyFeature)
		{
			if (existingSoftkeyFeature->isRecordValid())
			{
				existingSoftkeyFeature->update(restoreSoftkeyFeature);
				existingSoftkeyFeature->saveRecord();
				usedSoftkeyFeature = existingSoftkeyFeature;
			}
			else
			{
				existingSoftkeyFeature->deleteRecord();
				restoreSoftkeyFeature->saveRecord();
				usedSoftkeyFeature = restoreSoftkeyFeature;
			}
		}
		else
		{
			restoreSoftkeyFeature->saveRecord();
			usedSoftkeyFeature = restoreSoftkeyFeature;
		}

		if (existingSoftkeyFeature)
			delete existingSoftkeyFeature;
	}
}


void SoftkeyFeatureManager::handleCreate(SoftkeyFeature *msgSoftkeyFeature)
{
	if (!mySession || !msgSoftkeyFeature)
		return;

	SoftkeyFeature *newSoftkeyFeature = createNewSoftkeyFeature(msgSoftkeyFeature->getFeatureNumber());
	if (newSoftkeyFeature)
	{
		//	Set this new feature as the current feature.
		setCurrentSoftkeyFeature(newSoftkeyFeature);

		// Send the new feature to the Browser.
		SessionMessage featureData(SoftkeyDev, WriteSoftkeyFeature);
		featureData.setFormatType(BrowserFormat);
		featureData.setHfcObject(currentSoftkeyFeature);
		mySession->send(&featureData, Messenger::DEST_UI);
	}
}


SoftkeyFeature *SoftkeyFeatureManager::createNewSoftkeyFeature(HobRecordNum recordNum)
{
	SoftkeyFeature *newSoftkeyFeature = (SoftkeyFeature *) NULL;
	if (dbAccess)
	{
		// Send the current new SoftkeyFeature to the browser.
		newSoftkeyFeature = new SoftkeyFeature(dbAccess);
		if (newSoftkeyFeature)
			newSoftkeyFeature->setFeatureNumber(recordNum);
	}
	return newSoftkeyFeature;
}

void SoftkeyFeatureManager::sendCurrentSoftkeyFeature()
{
	if (!currentSoftkeyFeature || !mySession)
	{
		Debug::trace("SoftkeyFeatureManager", "sendCurrentSoftkeyFeature(): currentSoftkeyFeature or mySession is NULL!");
		return;
	}
	SessionMessage categoryMsg(SoftkeyDev, WriteSoftkeyFeature);
	categoryMsg.setFormatType(BrowserFormat);
	categoryMsg.setHfcObject(currentSoftkeyFeature);
	mySession->send(&categoryMsg, Messenger::DEST_UI);
}

void SoftkeyFeatureManager::destroyQueriesAndIters()
{
	Debug::trace("SoftkeyFeatureManager", "destroySoftkeyFeatureQueriesAndIters()");
	if (allSoftkeyFeaturesIter)
	{
		delete allSoftkeyFeaturesIter;
		allSoftkeyFeaturesIter = 0;
	}

	if (allSoftkeyFeaturesQuery)
	{
		delete allSoftkeyFeaturesQuery;
		allSoftkeyFeaturesQuery = 0;
	}
}
