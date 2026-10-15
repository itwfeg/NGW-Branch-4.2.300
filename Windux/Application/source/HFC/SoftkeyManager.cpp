/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "GenericHfcObject.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "HobTokenizerCSV.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "SoftkeyStyleManager.h"
#include "SoftkeyFeatureManager.h"
#include "SoftkeyGroupManager.h"

#include "SoftkeyDevEvent.h"
#include "Softkey.h"
#include "SoftkeyBasics.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyStylesRM.h"
#include "DBProductsRM.h"
#include "DBCategoriesRM.h"
#include "DBProductStatisticsRM.h"


#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"

#include "ProductManager.h"
#include "ProductTotalManager.h"
#include "Product.h"
#include "OperatorManager.h"
#include "CutTestManager.h"
#include "Category.h"
#include "Special.h"
#include "TransactionType.h"
#include "TransactionTypeManager.h"
#include "ConfigurationManager.h"

#include "HfcStartReport.h"
#include "ReportEvent.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"

#include "WriteSingleField.h"
#include "Product.h"

#include "HobList.h"
#include "LabelApplier.h"
#include "LabelApplierPositionInfo.h"
#include "ListCameraSystemPredictions.h"
#include "NGWDimensionOverrides.h"

#include <locale>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyManager::dataType = 0;
std::string SoftkeyManager::dataTypeName("SoftkeyManager"); 
#endif


SoftkeyManager::SoftkeyManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SoftkeyManager::SoftkeyManager(hfc::HFCManagerContainer *container)
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

SoftkeyManager::~SoftkeyManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentSoftkey)
		delete currentSoftkey;

	if (currentGridData)
		delete currentGridData;

	if (writeTareValue)
		delete writeTareValue;

	destroyQueriesAndIters();
}


void SoftkeyManager::init()
{
	mySession = 0;

	currentSoftkey = 0;
	allSoftkeysQuery = 0;
	allSoftkeysIter  = 0;

	currentGridData = 0;

	writeTareValue = 0;
	accumItemsPrintKeyHit = false;
}


DBSession *SoftkeyManager::getMySession()
{
	return mySession;
}
void SoftkeyManager::setMySession(DBSession *session)
{
	mySession = session;
}

   
void SoftkeyManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();

	if (event == RestoreSoftkeyFeature || event == RestoreSoftkeyStyle || event == RestoreSoftkeyGroup ||
		 event == RestoreSoftkey || event == RestoreFlashkeyAsSoftkey)
	{
		if (mySession)
			mySession->setCommSessionSoftkeyReload();		
	}

	switch (event)
	{
		case ReadSoftkey:
		case ReadAllSoftkeys:
		case WriteSoftkey:
		case DeleteSoftkey:
		case DeleteAllSoftkeys:
		case SaveSoftkey:
		case RestoreSoftkey:
		case CreateNewSoftkey:
		case RestoreFlashkeyAsSoftkey:
		case ExecuteSoftkey:
		case BatchApplyDeleteSoftkey:
		case ReadSoftkeyScripts:
		case ListCameraSystemPredictions:
		case ReadCameraSystemPredictions:
			handleSoftkeyMessage(msg);
			break;

		case ReadSoftkeyFeature:
		case ReadAllSoftkeyFeatures:
		case WriteSoftkeyFeature:
		case DeleteSoftkeyFeature:
		case DeleteAllSoftkeyFeatures:
		case SaveSoftkeyFeature:
		case RestoreSoftkeyFeature:
		case CreateNewSoftkeyFeature:
		case BatchApplyDeleteSoftkeyFeature:
			mgrContainer_->getSoftkeyFeatureManager()->handleMessage(msg);
			break;

		case ReadSoftkeyStyle:
		case ReadAllSoftkeyStyles:
		case WriteSoftkeyStyle:
		case DeleteSoftkeyStyle:
		case DeleteAllSoftkeyStyles:
		case SaveSoftkeyStyle:
		case RestoreSoftkeyStyle:
		case CreateNewSoftkeyStyle:
		case ReadAllSoftkeyStyleNumbers:
		case BatchApplyDeleteSoftkeyStyle:
			mgrContainer_->getSoftkeyStyleManager()->handleMessage(msg);
			break;

		case ReadSoftkeyGroup:
		case ReadAllSoftkeyGroups:
		case WriteSoftkeyGroup:
		case DeleteSoftkeyGroup:
		case DeleteAllSoftkeyGroups:
		case SaveSoftkeyGroup:
		case RestoreSoftkeyGroup:
		case CreateNewSoftkeyGroup:
		case LoadSoftkeyGroup:
		case LoadSoftkeyGroupForEditor:
		case LoadAllSoftkeyGroups:
		case BatchApplyDeleteSoftkeyGroup:
			mgrContainer_->getSoftkeyGroupManager()->handleMessage(msg);
			break;

		default:
			if (Debug::isTracing("SoftkeyManager"))
			{
				HobString myDebug = "handleMessage():  UNKNOWN EVENT: ";
				myDebug.append(Integer::toString(event));
				Debug::trace("SoftkeyManager", myDebug);
			}
			break;
	}
}


void SoftkeyManager::handleSoftkeyMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	bool traceOn = Debug::isTracing("SoftkeyManager");
	if (traceOn)
	{
		HobString myDebug = "handleSoftkeyMessage() event: ";
		myDebug.append(Integer::toString(event));
		Debug::trace("SoftkeyManager", myDebug);
	}

	Softkey *msgSoftkey = dynamic_cast<Softkey *> (msg->getHfcObject());
	if (msgSoftkey)
		msgSoftkey->show();
	else if (traceOn)
		Debug::trace("SoftkeyManager", "Softkey object in message passed to handleSoftkeyMessage() is null!");

	switch (event)
	{
		case ReadSoftkey:
			handleRead(msgSoftkey);
			break;

		case ReadAllSoftkeys:
			handleReadAll();
			break;

		case WriteSoftkey:
			handleWrite(msgSoftkey);
			break;

		case DeleteSoftkey:
		case BatchApplyDeleteSoftkey:
			handleDelete(msg);
			break;

		case DeleteAllSoftkeys:
			handleDeleteAll();
			break;

		case SaveSoftkey:
			handleUpdateAndSave(msgSoftkey);
			break;

		case RestoreSoftkey:
			handleRestore(msg);
			break;

		case CreateNewSoftkey:
			handleCreate(msgSoftkey);
			break;

		case RestoreFlashkeyAsSoftkey:
			handleRestoreFlashkeyAsSoftkey(msgSoftkey);
			break;

		case ExecuteSoftkey:
			handleExecute(msgSoftkey);
			break;

		case ReadSoftkeyScripts:
		    SoftkeyScriptManager::getInstance().sendScriptList();
			break;

		case ListCameraSystemPredictions:
			saveCameraSystemPredictions( dynamic_cast<hfc::ListCameraSystemPredictionsMessage*>( msg->getHfcObject() ) );
			// drop through...

		case ReadCameraSystemPredictions:
			buildCameraSystemPredictions();
			break;

		default:
			if (traceOn)
			{
				HobString myDebug = "handleSoftkeyMessage():  UNKNOWN Softkey EVENT: ";
				myDebug.append(Integer::toString(event));
				Debug::trace("SoftkeyManager", myDebug);
			}
			break;
	}
}



/*	===========================================================================================
	============================		Start Softkey section		===========================
	===========================================================================================
*/
/* If the first record was not read, this method will return NULL.
 */
Softkey *SoftkeyManager::firstSoftkey()
{
	if (!dbAccess)
		return (Softkey *)NULL;

	DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftkeysRM)
		return (Softkey *)NULL;

	// If the user calls firstSoftkey() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allSoftkeysQuery = dbSoftkeysRM->retrieveAll();

	// Couldn't get the query from which to access the Softkeys, just return false.
	if (!allSoftkeysQuery)
		return (Softkey *)NULL;

	DBQuery<DBSoftKey>::const_iterator tempIter = allSoftkeysQuery->begin();
	if (tempIter == allSoftkeysQuery->end())
		return (Softkey *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextSoftkey()
	 * method call below).
	 */
	allSoftkeysIter = new DBQuery<DBSoftKey>::const_iterator(tempIter);
	DBSoftKey *c = new DBSoftKey(*(*allSoftkeysIter));
	Softkey *newSoftkey = new Softkey(c, dbAccess);
	
	if (newSoftkey && Debug::isTracing("Softkey.test"))
	{
		HobString msgDebug("SoftkeyManager::firstSoftkey():");
		msgDebug.append("\n*******************************************************");
		msgDebug.append("\n   query size:     ").append(Long::toString((long)allSoftkeysQuery->size()));
		msgDebug.append("\n   groupNumber:     ").append(Long::toString(newSoftkey->getGroupNumber()));
		msgDebug.append("\n   hPosition:       ").append(Integer::toString(newSoftkey->getHorizontalPosition()));
		msgDebug.append("\n   vPosition:       ").append(Integer::toString(newSoftkey->getVerticalPosition()));
		msgDebug.append("\n*******************************************************");
		Debug::trace("Softkey.test", msgDebug);
	}
	
	return newSoftkey;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstSoftkey() method, the first call to this method will
 * result in the first record being returned.
 */
Softkey *SoftkeyManager::nextSoftkey()
{
	if (!dbAccess)
		return (Softkey *)NULL;

	if (allSoftkeysQuery && !allSoftkeysQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allSoftkeysQuery || !allSoftkeysIter)
		return firstSoftkey();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allSoftkeysIter);
	if (*allSoftkeysIter == allSoftkeysQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Softkeys in the db. */
		destroyQueriesAndIters();
		return (Softkey *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBSoftKey copy constructor in order for the Hfc Softkey to
	 * control the destruction of the DBSoftKey record that the iterator contains; otherwise, the DBSoftKey will
	 * be deleted when the iterator goes out of scope.  The DBSoftKey copy will be deleted by the Softkey record
	 * created below.
	 */
	DBSoftKey *c = new DBSoftKey(*(*allSoftkeysIter));
	Softkey *newSoftkey = new Softkey(c, dbAccess);

	if (newSoftkey && Debug::isTracing("Softkey.test"))
	{
		HobString msgDebug("SoftkeyManager::nextSoftkey():");
		msgDebug.append("\n*******************************************************");
		msgDebug.append("\n   groupNumber:     ").append(Long::toString(newSoftkey->getGroupNumber()));
		msgDebug.append("\n   hPosition:       ").append(Integer::toString(newSoftkey->getHorizontalPosition()));
		msgDebug.append("\n   vPosition:       ").append(Integer::toString(newSoftkey->getVerticalPosition()));
		msgDebug.append("\n*******************************************************");
		Debug::trace("Softkey.test", msgDebug);
	}

	return newSoftkey;
}


/* Provide a countAll method so the caller doesn't need to know about a dbSoftkeysRM.
 */
int SoftkeyManager::countAllSoftkeys()
{
	if (!dbAccess)
		return -1;

	DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftkeysRM)
		return -1;

	return dbSoftkeysRM->countAll();
}

/* Provide a countAll method so the caller doesn't need to know about a dbSoftkeysRM.
 */
int SoftkeyManager::countAllSoftkeys(bool notDeleted)
{
	int recordCount = -1;

	if (!dbAccess)
		return -1;

	DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftkeysRM)
		return -1;

	bool traceOn = Debug::isTracing("RecordCount");

	if (notDeleted)
	{
		DBQuery<DBSoftKey> *myQuery = dbSoftkeysRM->retrieveAll();
		if (myQuery)
		{
			recordCount = (int)myQuery->size();
			delete myQuery;
		}
	}
	else
	{
		recordCount = dbSoftkeysRM->countAll();
	}
	if (traceOn)
		Debug::trace("RecordCount", "countAllSoftkeys(" + booltostring(notDeleted) + "):  recordCount = " + inttostring(recordCount));

	return recordCount;
}

Softkey *SoftkeyManager::getCurrentSoftkey()
{
	return currentSoftkey;
}
void SoftkeyManager::setCurrentSoftkey(Softkey *newSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager.Softkey");
	HobString myDebug = "setCurrentSoftkey():   ";

	if (currentSoftkey)
		delete currentSoftkey;
	currentSoftkey = newSoftkey;

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("currentSoftkey set to: ").append(Hex::toString((long long) currentSoftkey));
		Debug::trace("SoftkeyManager.Softkey", myDebug2);
	}
}


void SoftkeyManager::handleRead(Softkey *msgSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager.Softkey");
	HobString myDebug = "handleReadSoftkey():   ";

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("ENTER  dbAccess = ").append(Hex::toString((long long) dbAccess));
		myDebug2.append("   msgSoftkey = ").append(Hex::toString((long long) msgSoftkey));
		Debug::trace("SoftkeyManager.Softkey", myDebug2);
	}

	if (!msgSoftkey || !dbAccess || !mySession)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	short currentLang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
	HobRecordNum long_test_dept = msgSoftkey->getDeptNum();
	Softkey *readSoftkey = Softkey::find(msgSoftkey->getGroupNumber(), msgSoftkey->getDeptNum(), msgSoftkey->getHorizontalPosition(), msgSoftkey->getVerticalPosition(), dbAccess, currentLang);
	if (readSoftkey)
		readSoftkey->setupSoftkeyDefaultValues(currentLang, SoftkeyDataIncludingDefaultValues);
	else
	{
		//	This softkey does not exist
		if (s && s->isInSupervisor())
		{
			// Send error message to browser indicating Softkey not found.
			mySession->addDialogMessage(ErrSoftkeyDev, SoftkeyNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		}
		if (readSoftkey)
			delete readSoftkey;
		return;
	}

	// We have a new current Softkey.
	setCurrentSoftkey(readSoftkey);
	if (traceOn)
	{
		HobString myDebug = "handleReadSoftkey(): currentSoftkey = ";
		myDebug.append(Hex::toString((long long) currentSoftkey));
		Debug::trace("SoftkeyManager.Softkey", myDebug);
	}

	// Send the Softkey found back to the Browser.
	// Is this the behavior for ALL SoftkeyRead messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a Softkey, but doesn't want the data sent back?
	SessionMessage softkeyData(SoftkeyDev, WriteSoftkey);
	softkeyData.setFormatType(BrowserFormat);
	softkeyData.setHfcObject(currentSoftkey);

	// Send the Softkey to the UI (Browser)
	mySession->send(&softkeyData, Messenger::DEST_UI);
}


void SoftkeyManager::handleReadAll()
{
	Debug::trace("SoftkeyManager", "Unable to handle a ReadAllSoftkeys event right now!");
}


void SoftkeyManager::handleWrite(Softkey *modifiedSoftkey)
{
	if (!mySession || !modifiedSoftkey || !currentSoftkey)
		return;

	if (Debug::isTracing("SoftkeyManager"))
	{
		HobString myDebug = "handleWriteSoftkey(): currentSoftkey = ";
		myDebug.append(Hex::toString((long long) currentSoftkey)).append("  , modifiedSoftkey = ").append(Hex::toString((long long) modifiedSoftkey));
		Debug::trace("SoftkeyManager", myDebug);
	}
  	bool softkeyChanged = currentSoftkey->update(modifiedSoftkey);
	if (softkeyChanged)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		short currentLang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

		currentSoftkey->setupSoftkeyDefaultValues(currentLang, SoftkeyDataIncludingDefaultValues);

		// Send the updated Softkey back to the Browser.
		SessionMessage softkeyData(SoftkeyDev, WriteSoftkey);
		softkeyData.setFormatType(BrowserFormat);
		softkeyData.setHfcObject(currentSoftkey);

		// Send the Softkey to the UI (Browser)
		mySession->send(&softkeyData, Messenger::DEST_UI);
	}
}


void SoftkeyManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Softkey *msgSoftkey = dynamic_cast<Softkey *> (msg->getHfcObject());
	if (!msgSoftkey || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Softkey *existingSoftkey = Softkey::find(msgSoftkey->getGroupNumber(), msgSoftkey->getDeptNum(), msgSoftkey->getHorizontalPosition(), msgSoftkey->getVerticalPosition(), dbAccess);
		if (existingSoftkey)
		{
			existingSoftkey->deleteRecord();
			if (Debug::isTracing("SoftkeyManager"))
			{
				HobString myDebug = "handleDeleteSoftkey - Softkey deleted - groupNum: ";
				myDebug.append(Long::toString((long)msgSoftkey->getGroupNumber())).append("   deptNum = ").append(Integer::toString(msgSoftkey->getDeptNum())).append("   hPos = ").append(Integer::toString(msgSoftkey->getHorizontalPosition())).append("   vPos = ").append(Integer::toString(msgSoftkey->getVerticalPosition()));
				Debug::trace("SoftkeyManager", myDebug);
			}

			if (msg->getEventType() != BatchApplyDeleteSoftkey)
			{
				if (mySession && !mySession->isCommSession())
				{
					//	Create a blank softkey
					Softkey *newSoftkey = createNewSoftkey();
					// Make it the current Softkey.
					setCurrentSoftkey(newSoftkey);
					//	Send the blank softkey to the browser
					sendCurrentSoftkey();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingSoftkey, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingSoftkey;
		}
		else
		{
			if (Debug::isTracing("SoftkeyManager"))
			{
				HobString myDebug = "handleDeleteSoftkey - Softkey to be deleted was not found in the database - groupNum: ";
				myDebug.append(Long::toString((long)msgSoftkey->getGroupNumber())).append("   hPos = ").append(Integer::toString(msgSoftkey->getHorizontalPosition())).append("   vPos = ").append(Integer::toString(msgSoftkey->getVerticalPosition()));
				Debug::trace("SoftkeyManager", myDebug);
			}
		}
	}
}


void SoftkeyManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void SoftkeyManager::handleDeleteAll(HobRecordNum deptNum)
{
//	Debug::trace("SoftkeyManager", "handleDeleteAllSoftkeys - deleting all Softkeys");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
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

		DBSoftKeysRM *dbRM = dbAccess->getDBSoftKeysRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();
	}

//	Debug::trace("SoftkeyManager", "handleDeleteAllSoftkeys - leaving");
}


void SoftkeyManager::handleUpdateAndSave(Softkey *modifiedSoftkey)
{
	Debug::trace("SoftkeyManager", HobString("handleUpdateAndSave(): currentSoftkey = ") + HobString(Hex::toString((long long) currentSoftkey)));
	if (currentSoftkey)
	{
		if (modifiedSoftkey)
		  	currentSoftkey->update(modifiedSoftkey);
		handleSaveRecord(currentSoftkey);
	}
}

void SoftkeyManager::handleSaveRecord(Softkey *recordToSave)
{
	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "";

	if (traceOn)
	{
		myDebug = "handleSaveSoftkey():  ";
		HobString myDebug2 = myDebug;
		myDebug2.append("recordToSave = ").append(Hex::toString((long long) recordToSave));
		Debug::trace("SoftkeyManager", myDebug2);
	}
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
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


void SoftkeyManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	bool traceOn = Debug::isTracing("SoftkeyManager.Softkey");
	HobString myDebug = "";

	Softkey *restoreSoftkey = dynamic_cast<Softkey *> (msg->getHfcObject());

	//Check if softkey has "old style" background image, if so, update it
	detectAndConvertOldStyleBackground(restoreSoftkey);
	//	Update the old Action_VoidTransaction
	detectAndConvertVoidTransactionAction(restoreSoftkey);

	if (!restoreSoftkey || !dbAccess)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Exiting!!! One of these is null!    restoreSoftkey = ").append(Hex::toString((long long) restoreSoftkey));
			myDebug2.append("   dbAccess = ").append(Hex::toString((long long) dbAccess));
			Debug::trace("SoftkeyManager.Softkey", myDebug2);
		}
		return;
	}

	if (restoreSoftkey->hasErrors())
	{
		if (mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = restoreSoftkey->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = restoreSoftkey->nextError();
			}
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
		handleRestoreSoftkey(restoreSoftkey);
}


void SoftkeyManager::handleRestoreSoftkey(Softkey *restoreSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager.Softkey");
	HobString myDebug = "";
	if (traceOn)
	{
		myDebug = "handleRestoreSoftkey()";
		Debug::trace("SoftkeyManager.Softkey", myDebug);
		myDebug.append(":   ");
	}

	restoreSoftkey->setDBAccess(dbAccess);

	Softkey *usedSoftkey = (Softkey *)NULL;
	
	HobRecordNum long_test_dept = restoreSoftkey->getDeptNum();
	Softkey *existingSoftkey = Softkey::find(restoreSoftkey->getGroupNumber(), restoreSoftkey->getDeptNum(), restoreSoftkey->getHorizontalPosition(), restoreSoftkey->getVerticalPosition(), dbAccess);
	if (existingSoftkey)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Softkey exists.");
			Debug::trace("SoftkeyManager.Softkey", myDebug2);
		}
		if (existingSoftkey->isRecordValid())
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Softkey is valid. Calling update()");
				Debug::trace("SoftkeyManager.Softkey", myDebug2);
			}
			existingSoftkey->update(restoreSoftkey);
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Calling saveRecord()");
				Debug::trace("SoftkeyManager.Softkey", myDebug2);
			}
			existingSoftkey->saveRecord();
			usedSoftkey = existingSoftkey;
		}
		else
		{
			existingSoftkey->deleteRecord();
			restoreSoftkey->saveRecord();
			usedSoftkey = restoreSoftkey;
		}
	}
	else
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Softkey DOES NOT exist. Saving this data.");
			Debug::trace("SoftkeyManager.Softkey", myDebug2);
		}
		restoreSoftkey->saveRecord();
		usedSoftkey = restoreSoftkey;
	}

	if (existingSoftkey)
		delete existingSoftkey;
}


void SoftkeyManager::handleRestoreFlashkeyAsSoftkey(Softkey *restoreSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager.FromQuantum");
	if (!restoreSoftkey || !dbAccess)
		return;

	HobRecordNum groupNumber = restoreSoftkey->setSoftkeyDataFromFlashkeyData();
	HobRecordNum lDept = restoreSoftkey->getDeptNum();

	HobString tempStr = Long::toString(groupNumber);
	if (traceOn)
		Debug::trace("SoftkeyManager.FromQuantum", "handleRestoreFlashkeyAsSoftkey(): for Group # " + tempStr);

	//	See if this group exists in the database. If not, create it.
	if (groupNumber > 0)
	{
		SoftkeyGroup *mySoftkeyGroup = SoftkeyGroup::find(groupNumber, lDept, dbAccess);
		if (!mySoftkeyGroup)
		{
			mySoftkeyGroup = new SoftkeyGroup(dbAccess);
			if (mySoftkeyGroup)
			{
				//	Quantum & Ultima flashkeys are in a 4 by 4 grid.
				//	For Evolution, allow some spacing.
				mySoftkeyGroup->setHorizontalGridSize(4);
				mySoftkeyGroup->setVerticalGridSize(4);
				mySoftkeyGroup->setHorizontalSpacing(2);
				mySoftkeyGroup->setVerticalSpacing(2);
				mySoftkeyGroup->setFeatureNumber(Reserved_ProductGrid);
				mySoftkeyGroup->setGroupNumber(groupNumber);
				mySoftkeyGroup->setDeptNum(lDept);
				mySoftkeyGroup->setDBAccess(dbAccess);

				SessionMessage skGroupMsg(SoftkeyDev, RestoreSoftkeyGroup);
				skGroupMsg.setFormatType(EvolutionTaggedFormat);
				skGroupMsg.setHfcObject(mySoftkeyGroup);
				mgrContainer_->getSoftkeyGroupManager()->handleMessage(&skGroupMsg);

				if (traceOn)
					Debug::trace("SoftkeyManager.FromQuantum", "handleRestoreFlashkeyAsSoftkey(): Created Group # " + tempStr);
			}
		}
		if (mySoftkeyGroup)
			delete mySoftkeyGroup;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeys))
	{
		// batch the record being restored; don't actually write it to the database right now.
		std::string synchSer = "";
		serializeForSynchronization(synchSer, restoreSoftkey);

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
		//	Now that the Flashkey data has been converted to Softkey data, restore the softkey.
		handleRestoreSoftkey(restoreSoftkey);
}

void SoftkeyManager::handleCreate(Softkey *msgSoftkey)
{
	if (!mySession || !msgSoftkey)
		return;

	Softkey *newSoftkey = createNewSoftkey(msgSoftkey->getGroupNumber(), msgSoftkey->getHorizontalPosition(), msgSoftkey->getVerticalPosition());
	if (newSoftkey)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		short currentLang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

		newSoftkey->update(msgSoftkey);
		newSoftkey->setupSoftkeyDefaultValues(currentLang, SoftkeyDataIncludingDefaultValues, msgSoftkey->getFeatureNumber());

		//	Set this new softkey as the current softkey.
		setCurrentSoftkey(newSoftkey);

		// Send the new softkey to the Browser.
		SessionMessage softkeyData(SoftkeyDev, WriteSoftkey);
		softkeyData.setFormatType(BrowserFormat);
		softkeyData.setHfcObject(currentSoftkey);

		// Send the product to the UI (Browser)
		mySession->send(&softkeyData, Messenger::DEST_UI);

		handleSaveRecord(newSoftkey);
	}
}


Softkey *SoftkeyManager::createNewSoftkey(HobRecordNum recordNum, int horizPos, int verticalPos)
{
	Softkey *newSoftkey = (Softkey *) NULL;
	if (dbAccess)
	{
		// Send the current new Softkey to the browser.
		newSoftkey = new Softkey(dbAccess);
		if (newSoftkey)
		{
			newSoftkey->setGroupNumber(recordNum);
			newSoftkey->setHorizontalPosition(horizPos);
			newSoftkey->setVerticalPosition(verticalPos);
		}
	}
	return newSoftkey;
}

void SoftkeyManager::sendCurrentSoftkey()
{
	if (!currentSoftkey || !mySession)
	{
		Debug::trace("SoftkeyManager", "sendCurrentSoftkey(): currentSoftkey or mySession is NULL!");
		return;
	}
	SessionMessage softkeyMsg(SoftkeyDev, WriteSoftkey);
	softkeyMsg.setFormatType(BrowserFormat);
	softkeyMsg.setHfcObject(currentSoftkey);
	mySession->send(&softkeyMsg, Messenger::DEST_UI);
}

void SoftkeyManager::handleExecute(Softkey *msgSoftkey)
{
	if (!msgSoftkey || !mySession || !dbAccess)
	{
		Debug::trace("SoftkeyManager", "handleExecuteSoftkey(): msgSoftkey or mySession or dbAccess is NULL!");
		return;
	}
	SessionMessage msg(UnknownDevice, 0);
	msg.setFormatType(BrowserFormat);

	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "";
	if (traceOn)
	{
		myDebug = "handleExecuteSoftkey():  actionNumber = ";
		myDebug.append(Integer::toString(msgSoftkey->getAction()));
		Debug::trace("SoftkeyManager", myDebug);
		myDebug.append(",  ");
	}

	switch (msgSoftkey->getAction())
	{
		case Action_PrintLabel:
		{
			msg.setDeviceType(SessionDev);
			msg.setEventType(SnPrintKeyPressed);
			//ProductManager *myProductMgr = mySession->getProductManager();
			//if (!myProductMgr)
			//	return;
			//myProductMgr->handleMessage(&msg);
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
				s->handleMessage(&msg);
			break;
		}


		// pjr-383 - Need to use the fixed row/column?  Yes
		case Action_LoadSoftkeyGroup:
		{
			SoftkeyGroup *mySoftkeyGroup = new SoftkeyGroup(dbAccess);
			if (!mySoftkeyGroup)
				return;

			mySoftkeyGroup->setFeatureNumber(msgSoftkey->getActionFeatureNumber());
			mySoftkeyGroup->setGroupNumber(msgSoftkey->getActionIntegerParam());

			SessionMessage msg(SoftkeyDev, LoadSoftkeyGroup);
			msg.setFormatType(SoftkeyGroupFormat);
			msg.setHfcObject(mySoftkeyGroup);

			handleMessage(&msg);

			delete mySoftkeyGroup;
			break;
		}

		// pjr-383 - Need to use the fixed row/column?  Yes
		case Action_LoadCategory_First:
		case Action_LoadCategory_Next:
		case Action_LoadCategory_Previous:
			//if (Configuration::getInstance()->isSelfServiceScale())
			//	handleSelfServeLoadCategorySoftkeyGroup(msgSoftkey);
			//else
			handleLoadCategorySoftkeyGroup(msgSoftkey);
			break;

		// pjr-383 - Need to use the fixed row/column?  Yes
		case Action_LoadSpecials_First:
		case Action_LoadSpecials_Next:
		case Action_LoadSpecials_Previous:
			if (Configuration::getInstance()->isSelfServiceScale())
				handleSelfServeLoadSpecialsSoftkeyGroup(msgSoftkey);
			else
				handleLoadSpecialsSoftkeyGroup(msgSoftkey);
			break;

		// pjr-383 - Need to use the fixed row/column?  Yes, since a "Load Best Sellers button could be in the fixed row/col of a group in feature Reserved_FixedRowColGrid
		case Action_LoadMostCommon_First:
		case Action_LoadMostCommon_Next:
		case Action_LoadMostCommon_Previous:
			handleLoadMostCommonSoftkeyGroup(msgSoftkey);
			break;

		// obsolete � From HLX, HT uses Action_LoadAlphabeticSearch_First
		case Action_LoadSearch_First:
		case Action_LoadSearch_Next:
		case Action_LoadSearch_Previous:
			handleLoadProductSearchSoftkeyGroup(msgSoftkey);
			break;

		// pjr-383 - Need to use the fixed row/column?  No, since a different Softkey Feature is used to display the results, not Reserved_FixedRowColGrid
		case Action_LoadAlphabeticSearch_First:
		case Action_LoadAlphabeticSearch_Next:
		case Action_LoadAlphabeticSearch_Previous:
			handleLoadAlphabeticSearch(msgSoftkey);
			break;

		case Action_PrintNewExpirationLabel:
		{
			msg.setDeviceType(ProductDev);
			msg.setEventType(ProductPrintNewExpirationLabel);
			//ProductManager *myProductMgr = mySession->getProductManager();
			//if (!myProductMgr)
			//	return;
			//myProductMgr->handleMessage(&msg);
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
				s->handleMessage(&msg);
			break;
		}

		case Action_PrintExistingExpirationLabel:
		{
			msg.setDeviceType(ProductDev);
			msg.setEventType(ProductPrintExistingExpirationLabel);
			//ProductManager *myProductMgr = mySession->getProductManager();
			//if (!myProductMgr)
			//	return;
			//myProductMgr->handleMessage(&msg);
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
				s->handleMessage(&msg);
			break;
		}

		case Action_ChangeLanguage:
		{
			msg.setDeviceType(SessionDev);
			msg.setEventType(SnSelectCurrentLanguage);

			std::string tmpMsg(Integer::toString(msgSoftkey->getActionIntegerParam()));
			GenericHfcObject gho(tmpMsg);
			msg.setHfcObject(&gho);

			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
				s->handleMessage(&msg);
			break;
		}

		case Action_LoginOperator:
			handleOperatorLogin(msgSoftkey);
			break;

		case Action_ApplyDiscount_CentsOff:
			handleProductDiscount(msgSoftkey, ProductApplyDiscountCentsOff);
			break;
		case Action_ApplyDiscount_PercentOff:
			handleProductDiscount(msgSoftkey, ProductApplyDiscountPercentOff);
			break;
#if 0
			//		case Action_ApplyDiscount_PerPackage:
			//			handleProductDiscount(msgSoftkey, ProductApplyDiscountPerPackage);
			//			break;
#endif
		case Action_ResetPrice:
			handleProductResetPrice(msgSoftkey);
			break;

		case Action_VoidTransaction:
		{
			/*			msg.setDeviceType(ProductDev);
			msg.setEventType(ProductVoidLastTransaction);
			//ProductManager *myProductMgr = mySession->getProductManager();
			//if (!myProductMgr)
			//	return;
			//myProductMgr->handleMessage(&msg);
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			s->handleMessage(&msg);
			*/			break;
		}

		case Action_SetPlatterTare:
		{
			SessionMessage ptMsg(SessionDev, SnRequestPlatterTare);
			ptMsg.setFormatType(BrowserFormat);

			// Send the PlatterTare msg to the UI (Browser)
			mySession->send(&ptMsg, Messenger::DEST_UI);
			
			if (traceOn)
				Debug::trace("SoftkeyManager", myDebug + "SetPlatterTare - Sending request for weigher status");

			// This requestWeigherStatus() call was originally located in the Softkey Action PlatterTare action. When in Operate Mode, it worked fine, but
			// when we go to Self Service Mode, the Tare field would be blank when we pressed platter tare and their was a weight already located on the
			// platter.  In Operate Mode, if there was a weight on the platter, that weight would shift to the tare field upon pressing Platter Tare. It
			// turns out that the bit in the weigherStatus, largeMotion, was being set to true.  There is no motion occurring and we could not for
			// sure determine at this time, why the bit was set to true when it should not be.  Self Serv and Operate appears to be executing the same code.
			// This block of code below, also fixes the problem.  We made the scale update it status and the flag was reset back to false. Also performing 
			// a requestWeigherStatus() below also fixed the problem, but for some reason, from the Softkey itself, it failed for SS.
			// So we moved this call to Session::turnPlatterTareOn()
			//Session * s = dynamic_cast<Session *>(mySession);
			//if (s)
			//	s->requestAWeigherStatus();

			break;
		}

		case Action_ChangeScreen:
		case Action_BrowseWeb:
		case Action_FPP_URL:
		case Action_Application_URL:
		case Action_PlayVideo:
		case Action_LaunchExternalApp:
			if (traceOn)
			{
				HobString myDebug = "handleExecuteSoftkey():  Cannot handle action # ";
				myDebug.append(Integer::toString(msgSoftkey->getAction())).append(" just yet!");
				Debug::trace("SoftkeyManager", myDebug);
			}
			break;

		case Action_PrintMultipleLabels:
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				SessionMessage msg(ProductDev, ProductPrintMultipleLabels);
				msg.setFormatType(BrowserFormat);
				s->handleMessage(&msg);
			}
			break;
		}

		case Action_ToggleProductionEntryMode:
		{
			if (mySession)
			{
				Session *s = dynamic_cast<Session *> (mySession);
				if (s)
				{
					if (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableProductionEntry))
						mySession->addDialogMessage(ErrSessionDev, SessionDevProductionEntryNotEnabled, HfcResponse::RESPONSESTYLE_OK);
					else
					{
						int newMode = OpModeProductionEntry;
						if (s->getCurrentMode() == OpModeProductionEntry)
							newMode = OpModeManualWeigh;		// leave Production Entry mode

						SessionMessage msg(OperationMode, newMode);
						msg.setFormatType(BrowserFormat);
						s->handleMessage(&msg);

						//	Send the Weigh Mode / Total Type message back to the Browser, so it can update the screen.
						SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
						s->handleMessage(&updateWeighModeMsg);
					}
				}
			}
			break;
		}

		case Action_RedisplayCurrentGrid:
		{
			if (currentGridData)
			{
				// Send the current SoftkeyGroup back to the Browser.
				Debug::trace("SoftkeyManager", "Sending the current grid, at the current position, back to the browser");

				Session *s = dynamic_cast<Session *> (mySession);
				short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
				currentGridData->setLanguageSelection(langNum);

				SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
				sgMsg.setFormatType(BrowserFormat);
				currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
				sgMsg.setHfcObject(currentGridData);
				mySession->send(&sgMsg, Messenger::DEST_UI);
			}
			break;
		}

		case Action_ToggleAccumulateItemsMode:
		{
			if (mySession)
			{
				Session *s = dynamic_cast<Session *> (mySession);
				if (s)
				{
					if (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableAccumItems))
						mySession->addDialogMessage(ErrSessionDev, SessionDevAccumulateItemsNotEnabled, HfcResponse::RESPONSESTYLE_OK);
					else
					{
						int newMode = OpModeAccumulateItems;
						if (s->getCurrentMode() == OpModeAccumulateItems)
						{
							newMode = OpModeManualWeigh;		// leave Accumulate Items mode
						}

						Debug::trace("SoftkeyManager", HobString("handling Action_ToggleAccumulateItemsMode  newMode=") + HobString(Integer::toString(newMode)));
						SessionMessage msg(OperationMode, newMode);
						msg.setFormatType(BrowserFormat);
						s->handleMessage(&msg);

						//	Send the Weigh Mode / Total Type message back to the Browser, so it can update the screen.
						SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
						s->handleMessage(&updateWeighModeMsg);
					}
				}
			}
			break;
		}

		case Action_AddItemToAccumulatedItems:
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				SessionMessage msg(ProductDev, ProductAddItemToAccumulatedItems);
				msg.setFormatType(BrowserFormat);
				resetAccumItemsPrintKeyHit(); // Added for Jira GCDR-199 to determine if printing totals or individual labels for RFID return
				s->handleMessage(&msg);
			}
			break;
		}

		case Action_RemoveLastItemFromAccumulatedItems:
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				SessionMessage msg(ProductDev, ProductRemoveLastItemFromAccumulatedItems);
				msg.setFormatType(BrowserFormat);
				s->handleMessage(&msg);
			}
			break;
		}

		case Action_ShowAccumulatedItems:
		{
			/*
				This Softkey action has been deprecated.  It has been changed to a ChangePage action!!

			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				SessionMessage msg(ProductDev, ProductShowAccumulatedItems);
				msg.setFormatType(BrowserFormat);
				s->handleMessage(&msg);
			}
			*/
			break;
		}

		case Action_RemoveAllAccumulatedItems:
		{
			if (mySession)
				mySession->addDialogMessage(ErrProductDev, ProductErrAccumulateItemsRemoveAllAreYouSure, HfcResponse::RESPONSESTYLE_OKCANCEL);
			break;
		}

		case Action_PrintAccumulatedItems:
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				SessionMessage msg(ProductDev, ProductPrintAccumulatedItems);
				msg.setFormatType(BrowserFormat);
				setAccumItemsPrintKeyHit(); // Added for Jira GCDR-199 to determine if printing totals or individual labels for RFID return
				s->handleMessage(&msg);
			}
			break;
		}

		case Action_SelectProduct:
			if (traceOn)
				Debug::trace("SoftkeyManager", myDebug + "SelectProduct - Joe will send a 101,5 instead.");		
			//break;
		case Action_AddSideItemToMeal:
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				// Check number of side items already in the meal.  Don't add more if at the limit.

				HobRecordNum pluNum = Long::valueOf( msgSoftkey->getActionParameter() );
				if (traceOn)
					Debug::trace( "SoftkeyManager", myDebug + "Action_AddSideItemToMeal - plu number:" + inttostring((int)pluNum) );

				Product prod;
				prod.setProductNum(pluNum);
				SessionMessage selectSideItem(ProductDev, ProductReadAndAddSideItem);
				selectSideItem.setFormatType(BrowserFormat);
				selectSideItem.setHfcObject(&prod);
				s->handleMessage(&selectSideItem);

				resetAccumItemsPrintKeyHit(); // Added for Jira GCDR-199 to determine if printing totals or individual labels for RFID return
			}
			break;
		}

		case Action_AddSideItemToMeatSpecSheet:
		{
			// JHC Cutting Test TODO: This is little more than a stub at this point. See case above for direction
            Session *s = dynamic_cast<Session *> (mySession);
			if (s)
			{
				HobRecordNum pluNum = Long::valueOf( msgSoftkey->getActionParameter() );
				if (traceOn)
					Debug::trace( "SoftkeyManager", myDebug + "Action_AddSideItemToMeatSpecSheet - plu number:" + inttostring((int)pluNum) );
			}
			break;
		}
		case Action_NoAction:
			if (traceOn)
				Debug::trace("SoftkeyManager", myDebug + "NoAction - No action specified on this softkey.");
			break;
		
		case Action_BeginCutTest:
		case Action_EndCutTest:
		case Action_CutTestHistory:
		case Action_CutTestSuspend:
		case Action_CutTestResume:
		{
			if (traceOn)
				Debug::trace("SoftkeyManager", myDebug + "Execute CutTest softkey.");

			if (mgrContainer_)
				mgrContainer_->getCutTestManager()->handleSoftkeyMessage(msgSoftkey);
			break;
		}

		case Action_SelectAnyProduct:
		{	
			if (traceOn)
				Debug::trace("SoftkeyManager", myDebug + "Action_SelectAnyProduct - Handled by Browser using popup keypad for Product number.");
			break;
		}

		case Action_PrintRunTotals:
		{
			ProductTotalManager *ptm = mgrContainer_->getProductTotalManager();

			// SOF-3399
			if (( ptm ) && ( ptm->customRunTotalsLabelExists() ))
			{
				if ( traceOn )
					Debug::trace( "SoftkeyManager", myDebug + "Action_PrintRunTotals - Have custom run totals label type" );

				ptm->handleProductPrintCustomRunTotalsLabel();
			}
			else
			{
				if ( traceOn )
					Debug::trace( "SoftkeyManager", myDebug + "Action_PrintRunTotals - Sending StartReport command" );

				if ( !mySession )
					return;

				Session *s = dynamic_cast<Session*>( mySession );
				if ( !s )
					return;

				HfcStartReport rpt;
				rpt.setPrinterType( FirstPrinter );
				rpt.setReportType( RunTotalsReportType );

				SessionMessage reportMsg( ReportManagerDev, StartReport );
				reportMsg.setHfcObject( &rpt );
				reportMsg.setFormatType( BrowserFormat );

				s->handleMessage( &reportMsg );
			}

			break;
		}
		
		case Action_SwitchTotalType:
		{
			Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType");

			//	AWL-52
			hfc::TransactionTypeManager *transTypeManager( mySession->getTransactionTypeManager() );

			if ( !transTypeManager )
				Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - No transaction type manager!");
			else
			{
				const int newTotalType( msgSoftkey->getActionIntegerParam() );

				bool revertTotalType( false );

				// check what total type we are currently in
				Session *s( dynamic_cast<Session*>( mySession ) );

				if ( !s )
					Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - No session!");
				else if ( !s->isInOperateMode() )
					Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - Not in operate mode!");
				else
				{
					Util::CSmartPtr<hfc::TransactionType> current( transTypeManager->getOperatorModeTransactionType() );

					if ( !current )
						Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - No current transaction type!");
					else
					{
						const int currentTotalType( current->getNumber() );

						Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - Current total type is " + inttostring(currentTotalType) );
						Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - Softkey (new) total type is " + inttostring(newTotalType) );

						if ( currentTotalType == newTotalType )
							revertTotalType = true;
					}
				}

				// SOF-3841
				// toggle total type modes when softkey used
				if ( revertTotalType )
				{
					// revert to previous total type
					transTypeManager->revertToPrevTransactionType();

					// when previous total type was same then force total type 0
					Util::CSmartPtr<hfc::TransactionType> current( transTypeManager->getOperatorModeTransactionType() );

					if ( !current )
						Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - No current transaction type after revert!");
					else
					{
						const int currentTotalType( current->getNumber() );

						if ( currentTotalType == newTotalType )
						{
							int toggleTotalType( -1 );

							// find total type 0 (or lowest) and revert to that
							for ( int i( 0 ); i < 64; ++i )
							{
								Util::CSmartPtr<hfc::TransactionType> toggle( transTypeManager->findTransactionType( i ) );

								if ( toggle )
								{
									toggleTotalType = i;
									break;
								}
							}

							Debug::trace("SoftkeyManager", myDebug + "Action_SwitchTotalType - Toggle total type is " + inttostring(toggleTotalType) );

							if ( 0 <= toggleTotalType )
							{
								transTypeManager->tempSwitchToTransactionType( toggleTotalType );
								transTypeManager->clearPrevTransactionTypeNum();
							}
						}
					}
				}
				else
				{
					transTypeManager->tempSwitchToTransactionType( newTotalType );
				}
			}

			break;
		}	
		
		case Action_LoadTareSoftkey:
		{
			ProductManager *myProductMgr = mySession->getProductManager();
			if (myProductMgr)
			{
				Product *curProd = myProductMgr->getCurrentProduct();
				if (curProd)
				{
					// FT-541
					// Fixup tare flashkey value to current weight precision (stored as 3 decimal places).
					double tareValue = (double) msgSoftkey->getActionIntegerParam();
					
					if ( Configuration::getInstance()->getEffectiveWeightPrecision() == 2 )
					{
						// Need to round the preset tare flashkey's value to 2 decimal places, then make it 3 implied decimal places again.
						// The subsequent call to formatWeight will get rid of the third decimal place.
						tareValue = MetSig::roundWeightTo2DecimalsThenBackTo3(tareValue);
						Debug::trace("SoftkeyManager.Tare", "Action_LoadTareSoftkey - weigher is in SINGLE RANGE mode.");
					}
					else
					{
						Debug::trace("SoftkeyManager.Tare", "Action_LoadTareSoftkey - weigher is in DUAL RANGE mode.");
					}
					long tareValueLong = (long) Rounding::hobround(tareValue);

					HobString tareStr;
					Configuration::getInstance()->formatWeight(tareValueLong, tareStr);

					if ( writeTareValue )
						delete writeTareValue;
					writeTareValue = new WriteSingleField(curProd->getProductNum(), Product::fieldOffsetTareAValue, tareStr);

					SessionMessage msg(ProductDev, ProductWriteSingleField);
					msg.setFormatType(BrowserFormat);
					msg.setHfcObject(writeTareValue);
					myProductMgr->handleMessage(&msg);
				}	
			}
			break;
		}	

		case Action_ChangeLabelPlacementAndPosition:
		{
			handleChangeLabelPlacementAndPosition(msgSoftkey);
			break;
		}

		case Action_ToggleLabelType:
		{
			handleToggleLabelType(msgSoftkey);
			break;
		}

		case Action_ToggleMode:
		{
			if (mySession)
			{
				Session *s = dynamic_cast<Session *> (mySession);
				if (s)
				{
					if (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableWrapTypes))
					{
						mySession->addDialogMessage(ErrSessionDev, SessionDevToggleModeNotEnabled, HfcResponse::RESPONSESTYLE_OK);
					}
					else
					{
						int newMode = OpModeAutoManualApply;
						if (s->getCurrentMode() == OpModeAutoManualApply)
							newMode = OpModeManualWeigh;		// leave Production Entry mode
						SessionMessage msg(OperationMode, newMode);
						msg.setFormatType(BrowserFormat);
						s->handleMessage(&msg);
				        
						SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
						s->handleMessage(&updateWeighModeMsg);
				
						/*
						int newMode = OpModeProductionEntry;
						if (s->getCurrentMode() == OpModeProductionEntry)
							newMode = ;		// leave Production Entry mode

						SessionMessage msg(OperationMode, newMode);
						msg.setFormatType(BrowserFormat);
						s->handleMessage(&msg);

						//	Send the Weigh Mode / Total Type message back to the Browser, so it can update the screen.
						SessionMessage updateWeighModeMsg(SessionDev, SnReadWeighModeTransactionType);
						s->handleMessage(&updateWeighModeMsg);
						*/
					}
				}
			}
			break;
		}

		case Action_TogglePick5Active:
		{
			Configuration *cfg = Configuration::getInstance();
			if (cfg)
			{
				// If It's active, make it inactive.  It's inactive, make it active.
				bool isActive = cfg->toBoolCfg(fieldOffsetCfgPick5Active);
				Debug::trace("Pick5", string("Pick5 mode is currently ") + booltostring(isActive) + ", changing to " + booltostring(!isActive));
				cfg->setCfg( fieldOffsetCfgPick5Active, Bool::toString(!isActive) );

				// Send the scale config message to the UI so it knows if it's on or off.
				if (mySession)
				{
					Session *s = dynamic_cast<Session *> (mySession);
					if (s)
					{
						if ( s->isInOperateMode() && s->getProductManager() && s->getProductManager()->getCurrentProduct() )
						{
							if (isActive)
								// Only clear this if we're changing to Pick5 inactive (which means it's currently active).
								s->getProductManager()->clearPick5ValueOutOfRange();

							// For the 'label types by mode' project, we have a current product and we're changing from Pick5 on to off, or vice versa.
							// Pick5 isn't a mode as Manual or Prepack modes are, but it is similar to a mode, so we need to re-evaluate the label types and see if they need to change.
							s->getProductManager()->resetUpLabelTypes( s->getCurrentMode() );
						}

						if ( s->getConfigurationManager( ) )
							s->getConfigurationManager()->dispatchConfigurationSaved();

						HobList active;
						active.push_back((!isActive) ? " entered" : " exited");
						mySession->addDialogMessage(ErrSessionDev, SessionDevPick5ActiveStatus, HfcResponse::RESPONSESTYLE_OK, &active);

					}
				}
			}
			break;
		}

		case Action_ToggleExtraShelfLifeDay:
		{
			// PJR 420 - Flashkey Options to Toggle Extra Day

			Configuration *cfg = Configuration::getInstance();
			if (cfg)
			{
				// If It's active, make it inactive.  It's inactive, make it active.
				bool isActive = cfg->toBoolCfg(fieldOffsetCfgEnableVariableShelfLifeTime);
				Debug::trace("Enable Shelf Life Time", string("Enable Shelf Life Time mode is currently ") + booltostring(isActive) + ", changing to " + booltostring(!isActive));
				//cfg->setCfg(fieldOffsetCfgEnableVariableShelfLifeTime, Bool::toString(!isActive));
			}
			break;
		}

        case Action_ChangeDepartment:
		{
			break;
		}

		case Action_ForcedCameraOverride:
		{
			// comment out for now, process the flashkey irregardless     if ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgWrapperCameraEnabled) )
			{
				Debug::trace("ForcedCameraOverride", "SoftkeyManager::handleExecute - Action_ForcedCameraOverride");

				HobString traySize = msgSoftkey->getActionStringParam();
				HobTokenizerCSV tok(&traySize, "_");
				HobString length, width, height;
				if (tok.next(&length) != -1 && tok.next(&width) != -1 && tok.next(&height) != -1)
				{
					Debug::trace("ForcedCameraOverride", "length=" + length + "  width=" + width + "   height=" + height);

					//Make function call to WrapperManager to set the length, width, and height of the tray
					NGWDimensionOverrides dimO;
					dimO.setOverrideLength(stringtoint(length));
					dimO.setOverrideWidth(stringtoint(width));
					dimO.setOverrideLipHeight(stringtoint(height));
					mySession->dispatchEvent(&dimO);
				}

			}
			break;
		}

		case Action_2ndLabelApplyToggle:
		{
			//SOF-5092 Disable secondary label Flashkey
			if (mySession && msgSoftkey)
			{
				Debug::trace("2ndLabelApplyToggle", string("Received Toggle") );
				
				bool current_state;
				Session *s = dynamic_cast<Session *> (mySession);
				ProductManager *myProductMgr = mySession->getProductManager();
	
				if (s && myProductMgr)
				{
					if ( s->isInOperateMode() )
					{
						current_state = s->getProductManager()->getSecondayLabelTypeEnable();
						if (current_state)
							// disable 2nd label
							s->getProductManager()->setSecondayLabelTypeEnable(false);
						else
							// Enable 2nd label
							s->getProductManager()->setSecondayLabelTypeEnable(true);
						
						//	Pass this event directly to the Product Manager
						SessionMessage msg(ProductDev, Product2ndLabelApplyToggle);
						HobString tmpMsg (Integer::toString(msgSoftkey->getActionIntegerParam()));
						GenericHfcObject gho(tmpMsg);
						msg.setHfcObject(&gho);
						myProductMgr->handleMessage(&msg);
					
					}

					HobList active;
					active.push_back((current_state) ? "Disabled" : "Enabled");
					mySession->addDialogMessage(ErrSessionDev, SessionDevSecLabelEnableToggleStatus, HfcResponse::RESPONSESTYLE_OK, &active);

				}
			}

			break;
		}

		default:
			if (traceOn)
			{
				HobString myDebug = "handleExecuteSoftkey():  Invalid action # ";
				myDebug.append(Integer::toString(msgSoftkey->getAction())).append("!");
				Debug::trace("SoftkeyManager", myDebug);
			}
			break;
	}
}

void SoftkeyManager::handleOperatorLogin(Softkey *msgSoftkey)
{
	if (!mySession || !msgSoftkey)
		return;

	//OperatorManager *myOperatorMgr = mySession->getOperatorManager();
	//if (!myOperatorMgr)
	//	return;
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	HobOperator *myHobOp = new HobOperator();
	if (!myHobOp)
		return;
	myHobOp->setOpNum(msgSoftkey->getActionOperatorNumber());

	SessionMessage msg(OperatorDev, OpLogin);
	msg.setFormatType(BrowserFormat);
	msg.setHfcObject(myHobOp);

	//myOperatorMgr->handleMessage(&msg);
	s->handleMessage(&msg);
	delete myHobOp;
}

void SoftkeyManager::handleProductDiscount(Softkey *msgSoftkey, int msgEvent)
{
	if (!mySession || !msgSoftkey)
		return;

	//ProductManager *myProductMgr = mySession->getProductManager();
	//if (!myProductMgr)
	//	return;
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	//	***********************************************************************************
	//	Modify this so we can send the DiscountType and the Discount to the ProductManager.
	Product *myProduct = new Product();
	if (!myProduct)
		return;

	DiscountType myDiscountType = INVALID_DISCOUNT_TYPE;
	switch (msgSoftkey->getAction())
	{
		case Action_ApplyDiscount_CentsOff:
			myDiscountType = Cents_off_unit_price;
			break;
		case Action_ApplyDiscount_PercentOff:
			myDiscountType = Percent_off_net_value;
			break;
#if 0
//		case Action_ApplyDiscount_PerPackage:
			myDiscountType = Discount_per_package;
			break;
#endif
	}
	if (myDiscountType != INVALID_DISCOUNT_TYPE)
	{
		myProduct->setDiscountType(myDiscountType);
		myProduct->setDiscountValue1(msgSoftkey->getActionIntegerParam());

		SessionMessage msg(ProductDev, msgEvent);
		msg.setFormatType(BrowserFormat);
		msg.setHfcObject(myProduct);

		//myProductMgr->handleMessage(&msg);
		s->handleMessage(&msg);
	}
	delete myProduct;
}

void SoftkeyManager::handleProductResetPrice(Softkey *msgSoftkey)
{
	if (!mySession || !msgSoftkey)
		return;

	//ProductManager *myProductMgr = mySession->getProductManager();
	//if (!myProductMgr)
	//	return;
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	SessionMessage msg(ProductDev, ProductResetUnitPrice);
	msg.setFormatType(BrowserFormat);

	//myProductMgr->handleMessage(&msg);
	s->handleMessage(&msg);
}

void SoftkeyManager::handleProductRead(Softkey *msgSoftkey)
{
	if (!msgSoftkey)
		return;

	//ProductManager *myProductMgr = mySession->getProductManager();
	//if (!myProductMgr)
	//	return;
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	Product *myProduct = new Product();
	if (!myProduct)
		return;
	myProduct->setProductNum(msgSoftkey->getActionIntegerParam());

	SessionMessage msg(ProductDev, ProductRead);
	msg.setFormatType(BrowserFormat);
	msg.setHfcObject(myProduct);

	//myProductMgr->handleMessage(&msg);
	s->handleMessage(&msg);
	delete myProduct;
}

void SoftkeyManager::destroyQueriesAndIters()
{
	Debug::trace("SoftkeyManager", "destroyQueriesAndIters()");
	if (allSoftkeysIter)
	{
		delete allSoftkeysIter;
		allSoftkeysIter = 0;
	}

	if (allSoftkeysQuery)
	{
		delete allSoftkeysQuery;
		allSoftkeysQuery = 0;
	}
}



void SoftkeyManager::handleLoadAlphabeticSearch(Softkey *msgSoftkey)
{
//	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "handleLoadAlphabeticSearch():   ";

	if (!mySession || !msgSoftkey || !dbAccess)
		return;

	SoftkeyFeature *feature = 0;
	SoftkeyStyle *style = 0;

	switch (msgSoftkey->getAction())
	{
		case Action_LoadAlphabeticSearch_Previous:
		{
			if (currentGridData)
				currentGridData->gotoPreviousPage();
			break;
		}

		case Action_LoadAlphabeticSearch_Next:
		{
			if (currentGridData)
				currentGridData->gotoNextPage();
			break;
		}

		default:
		{
			// Shouldn't this be 999013, since that is where the results are going?????????????????????????????????????????????????
			HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
			HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
			if (!feature)
				return;

			SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
			if (!style)
				return;

			// new
			HobRecordNum groupNumber = feature->getDefaultGroupNumber();
			SoftkeyGroup *readSoftkeyGroup = SoftkeyGroup::find(groupNumber, lDept, dbAccess);
			int ssHorzGridSize = 0;
			int ssVertGridSize = 0;
			int ssHorzSpacing = 0;
			int ssVertSpacing = 0;
			int cellCount = 0;

			Configuration *config = Configuration::getInstance();

			if ( ( config->isSelfServiceScale() ) && ( ReservedSK_SelfservCategoryResultsButtons == featureNum ) && ( config->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys)) )
			{
				ssHorzGridSize = readSoftkeyGroup->getHorizontalGridSize();
				ssVertGridSize = readSoftkeyGroup->getVerticalGridSize();
				cellCount = ssHorzGridSize * ssVertGridSize;

				delete readSoftkeyGroup;
				readSoftkeyGroup = NULL;

				if (cellCount <= 0)
				{
					delete feature;
					delete style;
					return;
				}
			}
			else
			{
				ssHorzGridSize = feature->getDefaultHorizontalGridSize();
				ssVertGridSize = feature->getDefaultVerticalGridSize();
				ssHorzSpacing  = 1;
				cellCount = ssHorzGridSize * ssVertGridSize;
			}

			ssHorzSpacing = 1;
			ssVertSpacing = 1;

			DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;
			DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;

			DBProduct *p = (DBProduct *)NULL;
			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			if (!dbProductsRM)
			{
				delete feature;
				delete style;
				return;
			}

			if (currentGridData)
			{
				delete currentGridData;
				currentGridData = 0;
			}

			currentGridData = new SoftkeyGroup(dbAccess);
			if (!currentGridData)
			{
				delete feature;
				delete style;
				return;
			}

			currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
			currentGridData->setSoftkeyFeature(feature);
			currentGridData->setSoftkeyStyle(style);
			currentGridData->setDeptNum(feature->getDeptNum());

			Session *s = dynamic_cast<Session *> (mySession);
			LanguageSelection langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
			currentGridData->setLanguageSelection(langNum);

			int hGridSize = ssHorzGridSize;
			int vGridSize = ssVertGridSize;
			int hSpacingSize = ssHorzSpacing;
			int vSpacingSize = ssVertSpacing;
			int hCellSize = 1;
			int vCellSize = 1;

			currentGridData->setHorizontalGridSize(hGridSize);
			currentGridData->setVerticalGridSize(vGridSize);
			currentGridData->setHorizontalSpacing(hSpacingSize);
			currentGridData->setVerticalSpacing(vSpacingSize);
			currentGridData->setGridCellCount(cellCount);

//			int gridSize = feature->getDefaultHorizontalGridSize() * feature->getDefaultVerticalGridSize();

			int hPos = 0, vPos = 0;


			HobString lookupString = msgSoftkey->getActionStringParam();
			lookupString.unQuoteForCSV();
			HobTokenizerCSV tok(&lookupString, BrowserFormatSeparator);
			HobString lookupStringElement;

			bool shortNamesBeingUsed = dbProductsRM->isProductShortNameUsed(langNum);

			while (tok.next(&lookupStringElement) != -1)
			{
				// for Fastrack self service the A-Z search buttons are "starts with"... not "contains"...
				if ((Configuration::getInstance()->isSelfServiceScale()) && (ReservedSK_SelfservCategoryResultsButtons == featureNum))
				{
					if ( shortNamesBeingUsed )
					{
						//Debug::trace("SoftkeyManager.search", "handleLoadAlphabeticSearch - product query using short name, langNum=" + inttostring(langNum) + ", words beginning with " + lookupStringElement);
						//productsQuery = dbProductsRM->retrieveNameStartsWith(lookupStringElement, langNum);

						Debug::trace("SoftkeyManager.search", "handleLoadAlphabeticSearch - product query using description and short name, langNum=" + inttostring(langNum) + ", words beginning with " + lookupStringElement);
						productsQuery = dbProductsRM->retrieveDescriptionOrNameStartsWith(lookupStringElement, langNum, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
					}
					else
					{
						Debug::trace("SoftkeyManager.search", "handleLoadAlphabeticSearch - product query using description, words beginning with " + lookupStringElement);
						productsQuery = dbProductsRM->retrieveDescriptionStartsWith(lookupStringElement, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
					}
				}
				else
				{
					productsQuery = dbProductsRM->retrieveKeywordInName(lookupStringElement);
					if ( !productsQuery || productsQuery->empty() )
					{
						Debug::trace("SoftkeyManager.search", "handleLoadAlphabeticSearch - product query for short name was empty. Going to execute KeywordInDescription query");
						productsQuery = dbProductsRM->retrieveKeywordInDescription(lookupStringElement, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
					}
				}

				if (productsQuery)
				{
					DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
					productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);

					bool flashkeyAddedToGrid = true;
					while (productsIter && *productsIter != productsQuery->end())
					{
						p = new DBProduct(*(*productsIter));
						if (p)
						{
							if ((Configuration::getInstance()->isSelfServiceScale()) && (p->getPrice() <= 0))
								flashkeyAddedToGrid = false;
							else
								flashkeyAddedToGrid = currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, 0 /*dbSourceCategory*/, false);
							delete p;
							p = 0;
						}

						++(*productsIter);

						if (flashkeyAddedToGrid)
						{
							if (++hPos == hGridSize)
							{
								hPos = 0;
								if (++vPos == vGridSize)
									vPos = 0;
							}
						}
						else
							flashkeyAddedToGrid = true;  // reset the assumption that a flashkey will be added.
					}

					delete productsQuery;
					productsQuery = 0;
					if (productsIter)
					{
						delete productsIter;
						productsIter = 0;
					}
				}
			}

			//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
			feature = (SoftkeyFeature *) NULL;
			style = (SoftkeyStyle *) NULL;

			if (currentGridData)
				currentGridData->gotoFirstPage();
			break;
		}
	}

	if (currentGridData)
	{
		// Send the SoftkeyGroup found back to the Browser.
		// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
		Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
		SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
		sgMsg.setFormatType(BrowserFormat);
		currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
		sgMsg.setHfcObject(currentGridData);

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send(&sgMsg, Messenger::DEST_UI);
	}

	if (feature)
		delete feature;
	if (style)
		delete style;
}

void SoftkeyManager::handleLoadProductSearchSoftkeyGroup(Softkey *msgSoftkey)
{
//	bool traceOn = Debug::isTracing("SoftkeyManager");
//	HobString myDebug = "handleLoadProductSearchSoftkeyGroup():   ";
	Debug::trace("SoftkeyManager", "handleLoadProductSearchSoftkeyGroup()");

	if (!msgSoftkey || !dbAccess)
		return;

	HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
	HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
	SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
	if (!feature)
		return;

	SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);

	if (style)
	{
		HobRecordNum categoryNum = msgSoftkey->getActionCategoryNumber();

		switch (msgSoftkey->getAction())
		{
			case Action_LoadSearch_Previous:
			{
				if (currentGridData)
					currentGridData->gotoPreviousPage();
				break;
			}

			case Action_LoadSearch_Next:
			{
				if (currentGridData)
					currentGridData->gotoNextPage();
				break;
			}

			default:
			{
				DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();

				if (dbProductsRM)
				{
					DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;

					productsQuery = dbProductsRM->retrieveKeywordInName(msgSoftkey->getActionParameter());
					if ( !productsQuery || productsQuery->size() == 0 )
					{
						Debug::trace("SoftkeyManager.search", "It was empty. Goint to execute KeywordInDescription query");
						productsQuery = dbProductsRM->retrieveKeywordInDescription( msgSoftkey->getActionParameter(), Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
					}
					if (productsQuery)
					{
						buildSoftkeyGridData(feature, style, productsQuery, categoryNum);

						delete productsQuery;
						productsQuery = 0;

						//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
						feature = (SoftkeyFeature *) NULL;
						style = (SoftkeyStyle *) NULL;
					}

					if (currentGridData)
						currentGridData->gotoFirstPage();
					break;
				}
			}
		}
		if (mySession && currentGridData)
		{
			// Send the SoftkeyGroup found back to the Browser.
			// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
			Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
			SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
			sgMsg.setFormatType(BrowserFormat);
			currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
			sgMsg.setHfcObject(currentGridData);

			// Send the SoftkeyFeature to the UI (Browser)
			mySession->send(&sgMsg, Messenger::DEST_UI);
		}
	}
	if (feature)
		delete feature;
	if (style)
		delete style;
}

void SoftkeyManager::handleChangeLabelPlacementAndPosition(Softkey *msgSoftkey)
{
	if (!mySession || !msgSoftkey)
		return;

	Debug::trace("SoftkeyManager.Placement", "handleChangeLabelPlacementAndPosition()");
	msgSoftkey->show();

	ProductManager *myProductMgr = mySession->getProductManager();
	if (!myProductMgr)
		return;

	//	Pass this info directly to the Product Manager, so we can handle the notification to the Operator.
	SessionMessage msg(ProductDev, ProductChangeRotationPlacement);
	HobString tmpMsg(Integer::toString(msgSoftkey->getActionIntegerParam()));
	GenericHfcObject gho(tmpMsg);
	msg.setHfcObject(&gho);
	myProductMgr->handleMessage(&msg);
}

void SoftkeyManager::handleToggleLabelType(Softkey *msgSoftkey)
{
	if (!mySession || !msgSoftkey)
		return;

	Debug::trace("SoftkeyManager.Clare", "handleToggleLabelType()");
	msgSoftkey->show();

	ProductManager *myProductMgr = mySession->getProductManager();
	if (!myProductMgr)
		return;

	//	Pass this info directly to the Product Manager, so we can handle the notification to the Operator.
	SessionMessage msg(ProductDev, ProductToggleLabelType);
	HobString tmpMsg(msgSoftkey->getActionParameter());
	GenericHfcObject gho(tmpMsg);
	msg.setHfcObject(&gho);
	myProductMgr->handleMessage(&msg);
}

// CAMERA
void SoftkeyManager::saveCameraSystemPredictions( hfc::ListCameraSystemPredictionsMessage *msg )
{
	Debug::trace("SoftkeyManager.Camera", "saveCameraSystemPredictions");

	if ( !msg )
	{
		Debug::trace("SoftkeyManager.Camera", "bad message");
		return;
	}

	const HobList products( msg->getProducts() );

	Debug::trace("SoftkeyManager.Camera", "have " + Integer::toString( products.size() ) + " products" );
	cameraPredictions = products;
}

// CAMERA
void SoftkeyManager::buildCameraSystemPredictions()
{
	Debug::trace("SoftkeyManager.Camera", "buildCameraSystemPredictions");

	Session *s( mySession ? dynamic_cast<Session*>( mySession ) : NULL );

	if (( !s ) || ( !dbAccess ))
		return;
	else if ( !cameraPredictions.size() )
	{
		Debug::trace("SoftkeyManager.Camera", "no predictions exists");
		return;
	}

    const bool haveProduct(( s->getProductManager() ) && ( s->getProductManager()->getCurrentProduct() ));

	if ( haveProduct )
	{
		// dont prompt when product already on screen
		Debug::trace("SoftkeyManager.Camera", "product already selected");
		return;
	}

	Configuration *config( Configuration::getInstance() );

	const HobRecordNum featureNum( Reserved_CameraSystemPredictions );
	const HobRecordNum lDept( config->toIntCfg( fieldOffsetCfgMostRecentActiveDeptNum ) );

	SoftkeyFeature *feature( SoftkeyFeature::find( featureNum, lDept, dbAccess ) );
	SoftkeyStyle *style( feature ? SoftkeyStyle::find( feature->getStyleNumber(), dbAccess ) : NULL );

	if ( !style )
	{
		if ( feature )
			delete feature;

		Debug::trace("SoftkeyManager.Camera", "error reading softkey style or missing");
		return;
	}

	int horzGridSize( 0 );
	int vertGridSize( 0 );
	int horzSpacing( 0 );
	int vertSpacing( 0 );
	int horzCellSize( 1 );
	int vertCellSize( 1 );

	// check for group settings
	{
		const HobRecordNum groupNumber( feature->getDefaultGroupNumber() );

		const SoftkeyGroup *group( SoftkeyGroup::find( groupNumber, lDept, dbAccess ) );

		if ( group )
		{
			Debug::trace("SoftkeyManager.Camera", "using group " + Integer::toString( groupNumber));

			horzGridSize = group->getHorizontalGridSize();
			vertGridSize = group->getVerticalGridSize();
			horzSpacing = group->getHorizontalSpacing();
			vertSpacing = group->getVerticalSpacing();

			delete group;
		}
		else
		{
			horzGridSize = feature->getDefaultHorizontalGridSize();
			vertGridSize = feature->getDefaultVerticalGridSize();
			horzSpacing = feature->getHorizontalSpacing();
			vertSpacing = feature->getVerticalSpacing();
		}
	}

	// language
	const LanguageSelection langNum( PrimaryLanguage );
	const HobString lang( config->toStringCfg( fieldOffsetCfgPrimaryLanguageCode ) );

	// ---- //

	DBProductsRM *dbProductsRM( dbAccess->getDBProductsRM() );
	
	int hPos( 0 );
	int vPos( 0 );

	SoftkeyGroup *gridData( new SoftkeyGroup( dbAccess ) );

	gridData->setGroupNumber( feature->getDefaultGroupNumber() );
	gridData->setSoftkeyFeature( feature );
	gridData->setSoftkeyStyle( style );
	gridData->setDeptNum( lDept );

	gridData->setLanguageSelection( langNum );

	gridData->setHorizontalGridSize( horzGridSize );
	gridData->setVerticalGridSize( vertGridSize );
	gridData->setHorizontalSpacing( horzSpacing );
	gridData->setVerticalSpacing( vertSpacing );
	gridData->setGridCellCount( horzGridSize * vertGridSize );

	for ( size_t i( 0 ); i < cameraPredictions.size(); ++i )
	{
		const HobString pnum( cameraPredictions.getElementAtPosition( i ) );
		const HobString dnum( Integer::toString( lDept ) );

		DBProduct *p( dbProductsRM->findPLU( pnum, dnum, true ) );

		if ( !p )
		{
			Debug::trace("SoftkeyManager.Camera", "no product found for " + pnum );
			continue;
		}

		// add to grid
		bool flashkeyAddedToGrid( true );

		if (( Configuration::getInstance()->isSelfServiceScale() ) && ( p->getPrice() <= 0 ))
		{
			Debug::trace("SoftkeyManager.Camera", "bad price " + Integer::toString( p->getPrice() ) + " for " + pnum );
			flashkeyAddedToGrid = false;
		}
		else if ( dnum != p->getDeptNumber() )
		{
			Debug::trace("SoftkeyManager.Camera", "dept mismatch for " + pnum );
			flashkeyAddedToGrid = false;
		}
		else
		{
			Debug::trace("SoftkeyManager.Camera", "adding to camera results grid " + pnum );
			gridData->addProductSoftkey( p, featureNum, langNum, hPos, vPos, horzCellSize, vertCellSize, 0 );
		}

		// cleanup
		delete p;

		// increment position
		if ( flashkeyAddedToGrid )
		{
			if ( horzGridSize == ++hPos )
			{
				hPos = 0;

				if ( vertGridSize == ++vPos )
				{
					Debug::trace("SoftkeyManager.Camera", "reached max grid size!" );
					break;
				}
			}
		}
	}

	// give result to UI
	if (( 0 == hPos ) && ( 0 == vPos ))
		Debug::trace("SoftkeyManager.Camera", "no products in grid!" );
	else if ( gridData )
	{
		Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");

		gridData->setFormatType( SoftkeyGroupSelfServeFormat );

		SessionMessage msg( SoftkeyDev, LoadSoftkeyGroup );
		msg.setFormatType( BrowserFormat );
		msg.setHfcObject( gridData );

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send( &msg, Messenger::DEST_UI );
	}

	// cleanup
	if ( gridData )
		delete gridData;

/*
	no need to delete style and feature, they will be deleted by grid...

	if ( style )
		delete style;

	if ( feature )
		delete feature;
*/
}


void SoftkeyManager::detectAndConvertOldStyleBackground(Softkey *msgSoftkey)
{
	if (!strcmp(msgSoftkey->getBackgroundImage().c_str(), "blank129x40.gif")) // old style regular size 
	{
		msgSoftkey->setBackgroundImage("182x60_blue.png");
	}
	else if (!strcmp(msgSoftkey->getBackgroundImage().c_str(), "blank129x90green.gif")) //old style "print" size
	{
		msgSoftkey->setBackgroundImage("182x124_green.png");
	}
	else if (!strcmp(msgSoftkey->getBackgroundImage().c_str(), "orange129x40.gif")) //old style "orange" background
	{
		msgSoftkey->setBackgroundImage("Orange1.png");
	}
	else if (!strcmp(msgSoftkey->getBackgroundImage().c_str(), "yellow129x40.gif")) //old style "orange" background
	{
		msgSoftkey->setBackgroundImage("Yellow1.png");
	}
}

void SoftkeyManager::detectAndConvertVoidTransactionAction(Softkey *msgSoftkey)
{
	//	PT 441 - The 'Void Transaction' action is no longer valid after the switch to Transactions.
	//			 This action needs to change to 'Change screen - Void Transactions'.
	//			 This is necessary to support vxWorks Access databases.
	if (msgSoftkey->getAction() == Action_VoidTransaction)
	{
		msgSoftkey->setAction(Action_ChangeScreen);
		msgSoftkey->setActionIntegerParam(15);
	}
}

void SoftkeyManager::setAccumItemsPrintKeyHit()
{
	accumItemsPrintKeyHit = true;
}
void SoftkeyManager::resetAccumItemsPrintKeyHit()
{
	accumItemsPrintKeyHit = false;
}

bool SoftkeyManager::isAccumItemsPrintKeyHit() const
{
	return accumItemsPrintKeyHit;
}



/**************************************************************************************************************/
/**************************************************************************************************************/
/**************************************************************************************************************/


void SoftkeyManager::buildSoftkeyGridData(SoftkeyFeature *feature, SoftkeyStyle *style, DBQuery<DBProduct> *productsQuery, HobRecordNum categoryNum)
{
	if (!mySession || !dbAccess)
		return;

	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "buildSoftkeyGridData():   ";

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	DBCategory *dbCategory = 0;

	DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;

	if (feature && style && productsQuery)
	{
		DBProduct *p = 0;

		HobString categoryBgColor;

		int ip = 0;
		HobRecordNum featureNum = feature->getFeatureNumber();

		Session *s = dynamic_cast<Session *> (mySession);
		short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

		int specialsHorzGridSize = 0;
		int specialsVertGridSize = 0;

		if (currentGridData)
		{
			delete currentGridData;
			currentGridData = 0;
		}


		SoftkeyGroup *fixedSoftkeyGroup = 0;
		FixedRowColMethod fixedRowColMethod = feature->getFixedRowColMethod();
		if (fixedRowColMethod != FRCM_None)
		{
			fixedSoftkeyGroup = SoftkeyGroupManager::loadFixedRowColSoftkeyGroup(RECORD_NOT_FOUND, langNum, dbAccess);
			if (!fixedSoftkeyGroup)
				return;

			SoftkeyGroup *variableSoftkeyGroup = SoftkeyGroupManager::prepareToLoadSoftkeyGroup(featureNum, RECORD_NOT_FOUND, langNum, dbAccess);
			if (!variableSoftkeyGroup)
				return;
				
			//Change size, if the fixed row/col method is specified
			if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
				variableSoftkeyGroup->setVerticalGridSize(variableSoftkeyGroup->getVerticalGridSize() - 1);
			else  // is FRCM_FirstColumn || FRCM_LastColumn
				variableSoftkeyGroup->setHorizontalGridSize(variableSoftkeyGroup->getHorizontalGridSize() - 1);

			specialsHorzGridSize = variableSoftkeyGroup->getHorizontalGridSize();
			specialsVertGridSize = variableSoftkeyGroup->getVerticalGridSize();

			delete variableSoftkeyGroup;
			variableSoftkeyGroup = 0;
		}

		currentGridData = new SoftkeyGroup(dbAccess);
		if (currentGridData)
		{
			currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
			currentGridData->setSoftkeyFeature(feature);
			currentGridData->setSoftkeyStyle(style);
			currentGridData->setDeptNum(feature->getDeptNum());

			currentGridData->setLanguageSelection(langNum);

			int hGridSize = feature->getDefaultHorizontalGridSize();
			int vGridSize = feature->getDefaultVerticalGridSize();
			int hCellSize = 1;
			int vCellSize = 1;

			currentGridData->setHorizontalSpacing(hCellSize);
			currentGridData->setVerticalSpacing(vCellSize);
			
   			if (fixedRowColMethod == FRCM_None)
   			{
				currentGridData->setHorizontalGridSize(hGridSize);
				currentGridData->setVerticalGridSize(vGridSize);
			} 
			else
			{
				currentGridData->setHorizontalGridSize(specialsHorzGridSize);
				currentGridData->setVerticalGridSize(specialsVertGridSize);
				hGridSize = specialsHorzGridSize;
				vGridSize = specialsVertGridSize;
			}
			
			currentGridData->setGridCellCount(hGridSize * vGridSize);

			Configuration *config = Configuration::getInstance();

			Softkey prevSoftkey;
			Filename previousFileName;
			previousFileName.setPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
			previousFileName.setFileName((langNum == 1) ? config->getPreviousGraphicFileName1() : config->getPreviousGraphicFileName2());
			if (previousFileName.getFileNameExtension().empty())
				previousFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));

			prevSoftkey.setBackgroundImage(previousFileName.getFullPath());
			prevSoftkey.setAction(Action_LoadCategory_Previous);
			prevSoftkey.setActionCategoryNumber(categoryNum);
			prevSoftkey.setActionFeatureNumber(featureNum);
			prevSoftkey.setName1(" ");		//	We don't want a name showing up over or under the graphic.
			prevSoftkey.setName2(" ");

			Softkey nextSoftkey;
			Filename nextFileName;
			nextFileName.setPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
			nextFileName.setFileName((langNum == 1) ? config->getNextGraphicFileName1() : config->getNextGraphicFileName2());
			if (nextFileName.getFileNameExtension().empty())
				nextFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));

			nextSoftkey.setBackgroundImage(nextFileName.getFullPath());
			
			Debug::trace("Softkey", "SoftkeyManager::buildSoftkeyGridData(): nextSoftkey.setAction(Action_LoadCategory_Next);");

			nextSoftkey.setAction(Action_LoadCategory_Next);
			nextSoftkey.setActionCategoryNumber(categoryNum);
			nextSoftkey.setActionFeatureNumber(featureNum);
			nextSoftkey.setName1(" ");		//	We don't want a name showing up over or under the graphic.
			nextSoftkey.setName2(" ");

	   		int gridSize = (fixedRowColMethod == FRCM_None) ? ( feature->getDefaultHorizontalGridSize() * feature->getDefaultVerticalGridSize() ) : ( hGridSize * vGridSize );

			int groupNum = 0;
			int prevCell = 0;
			int nextCell = prevCell + gridSize - 1;
			int totalCells = (int)productsQuery->size();
			int cellNum = 0;
			int hPos = 0, vPos = 0;

			DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
			productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);

			while (productsIter && *productsIter != productsQuery->end())
			{
				bool flashkeyAddedToGrid = true;
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("cellNum = ").append(Integer::toString(cellNum));
					myDebug2.append("   totalCells = ").append(Integer::toString(totalCells));
					myDebug2.append("   product number = ").append((*productsIter)->getPLU());
					Debug::trace("SoftkeyManager", myDebug2);
				}
				if (cellNum > 0 && cellNum == prevCell)
				{
					//	Add the Previous button.
					currentGridData->add(prevSoftkey, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);
					if (traceOn)
					{
						HobString myDebug2 = myDebug;
						myDebug2.append("Added a 'Previous' flashkey at ").append(Integer::toString(hPos));
						myDebug2.append(",").append(Integer::toString(vPos));
						Debug::trace("SoftkeyManager", myDebug2);
					}
				}
				else if (cellNum < totalCells && cellNum == nextCell)
				{
					//	Add the Next button.
					currentGridData->add(nextSoftkey, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);
					//	Increment the groupNum.
					++groupNum;
					//	Recalculate the previous & next cells.
					prevCell = nextCell + 1;
					nextCell = prevCell + gridSize - 1;
					if (traceOn)
					{
						HobString myDebug2 = myDebug;
						myDebug2.append("Added a 'Next' flashkey at ").append(Integer::toString(hPos));
						myDebug2.append(",").append(Integer::toString(vPos));
						Debug::trace("SoftkeyManager", myDebug2);
					}
				}
				else
				{
					//	Add the Product.

					p = new DBProduct(*(*productsIter));
					if (p)
					{
						if (dbCategoriesRM)
							dbCategory = dbCategoriesRM->findNumber(p->getCategory(), p->getDeptNumber());
						flashkeyAddedToGrid = currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, dbCategory, false);
						if (dbCategory)
						{
							delete dbCategory;
							dbCategory = 0;
						}
						ip++;
						delete p;
						p = 0;

						++(*productsIter);
						if (traceOn)
						{
							HobString myDebug2 = myDebug;
							if (productsIter && *productsIter != productsQuery->end())
								myDebug2.append("Next iter:    product number = ").append((*productsIter)->getPLU());
							else
								myDebug2.append("Next iter:    End of the list!");
							Debug::trace("SoftkeyManager", myDebug2);
						}
					}
				}

				if (flashkeyAddedToGrid)
				{
					++cellNum;
					if (++hPos == hGridSize)
					{
						hPos = 0;
						if (++vPos == vGridSize)
							vPos = 0;
					}
				}
			}
		}

		if (productsIter)
		{
			delete productsIter;
		}

  		if (fixedSoftkeyGroup)
  			currentGridData->concatenateSoftkeyGroup(fixedSoftkeyGroup, fixedRowColMethod);

	}
}


void SoftkeyManager::buildSelfServeSoftkeyGridData(SoftkeyFeature *feature, SoftkeyStyle *style, DBQuery<DBProduct> *productsQuery, HobRecordNum UNUSED(categoryNum))
{
	if (!mySession || !dbAccess)
		return;

	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "buildSelfServeSoftkeyGridData():   ";

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	DBCategory *dbCategory = 0;

	DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;

	if (feature && style && productsQuery)
	{
		DBProduct *p = 0;

		HobString categoryBgColor;

		int ip = 0;
		HobRecordNum featureNum = feature->getFeatureNumber();

		Session *s = dynamic_cast<Session *> (mySession);
		short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
			
		int specialsHorzGridSize = 0;
		int specialsVertGridSize = 0;

 		if (currentGridData)
		{
			delete currentGridData;
			currentGridData = 0;
		}
		
		
		SoftkeyGroup *fixedSoftkeyGroup = 0;
		FixedRowColMethod fixedRowColMethod = feature->getFixedRowColMethod();
		if (fixedRowColMethod != FRCM_None)
		{
			fixedSoftkeyGroup = SoftkeyGroupManager::loadFixedRowColSoftkeyGroup(RECORD_NOT_FOUND, langNum, dbAccess);
			if (!fixedSoftkeyGroup)
				return;

			SoftkeyGroup *variableSoftkeyGroup = SoftkeyGroupManager::prepareToLoadSoftkeyGroup(featureNum, RECORD_NOT_FOUND, langNum, dbAccess);
			if (!variableSoftkeyGroup)
				return;
				
			//Change size, if the fixed row/col method is specified
			if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
				variableSoftkeyGroup->setVerticalGridSize(variableSoftkeyGroup->getVerticalGridSize() - 1);
			else  // is FRCM_FirstColumn || FRCM_LastColumn
				variableSoftkeyGroup->setHorizontalGridSize(variableSoftkeyGroup->getHorizontalGridSize() - 1);

			specialsHorzGridSize = variableSoftkeyGroup->getHorizontalGridSize();
			specialsVertGridSize = variableSoftkeyGroup->getVerticalGridSize();

			delete variableSoftkeyGroup;
			variableSoftkeyGroup = 0;
		}

		currentGridData = new SoftkeyGroup(dbAccess);
		if (currentGridData)
		{
			currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
			currentGridData->setSoftkeyFeature(feature);
			currentGridData->setSoftkeyStyle(style);
			currentGridData->setDeptNum(feature->getDeptNum());

			currentGridData->setLanguageSelection(langNum);

			int hGridSize = feature->getDefaultHorizontalGridSize();
			int vGridSize = feature->getDefaultVerticalGridSize();
			int hCellSize = 1;
			int vCellSize = 1;

			currentGridData->setHorizontalSpacing(hCellSize);
			currentGridData->setVerticalSpacing(vCellSize);
			
   			if (fixedRowColMethod == FRCM_None)
   			{
				currentGridData->setHorizontalGridSize(hGridSize);
				currentGridData->setVerticalGridSize(vGridSize);
			} 
			else
			{
				currentGridData->setHorizontalGridSize(specialsHorzGridSize);
				currentGridData->setVerticalGridSize(specialsVertGridSize);
				hGridSize = specialsHorzGridSize;
				vGridSize = specialsVertGridSize;
			}
			
			currentGridData->setGridCellCount(hGridSize * vGridSize);

//			int gridSize = feature->getDefaultHorizontalGridSize() * feature->getDefaultVerticalGridSize();
//			int groupNum = 0;
//			int prevCell = 0;
//			int nextCell = prevCell + gridSize - 1;
			int totalCells = (int)productsQuery->size();
			int cellNum = 0;
			int hPos = 0, vPos = 0;

			DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
			productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);

			bool flashkeyAddedToGrid = true;
			while (productsIter && *productsIter != productsQuery->end())
			{
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("cellNum = ").append(Integer::toString(cellNum));
					myDebug2.append("   totalCells = ").append(Integer::toString(totalCells));
					myDebug2.append("   product number = ").append((*productsIter)->getPLU());
					Debug::trace("SoftkeyManager", myDebug2);
				}

				//	Add the Product.
				p = new DBProduct(*(*productsIter));
				if (p)
				{
					if (p->getPrice() <= 0)
						flashkeyAddedToGrid = false;
					else
					{
						if (dbCategoriesRM)
							dbCategory = dbCategoriesRM->findNumber(p->getCategory(), p->getDeptNumber());
						currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, dbCategory);
						if (dbCategory)
						{
							delete dbCategory;
							dbCategory = 0;
						}
						ip++;
					}
					delete p;
					p = 0;

					++(*productsIter);
					if (traceOn)
					{
						HobString myDebug2 = myDebug;
						if (productsIter && *productsIter != productsQuery->end())
							myDebug2.append("Next iter:    product number = ").append((*productsIter)->getPLU());
						else
							myDebug2.append("Next iter:    End of the list!");
						Debug::trace("SoftkeyManager", myDebug2);
					}
				}

				if (flashkeyAddedToGrid)
				{
					++cellNum;
					if (++hPos == hGridSize)
					{
						hPos = 0;
						if (++vPos == vGridSize)
							vPos = 0;
					}
				}
				else
					flashkeyAddedToGrid = true;  // reset the assumption that a flashkey will be added.
			}
		}

		if (productsIter)
			delete productsIter;

  		if (fixedSoftkeyGroup)
  			currentGridData->concatenateSoftkeyGroup(fixedSoftkeyGroup, fixedRowColMethod);

	}
}


/**************************************************************************************************************/
/**************************************************************************************************************/
/**************************************************************************************************************/


void SoftkeyManager::handleLoadCategorySoftkeyGroup(Softkey *msgSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "";
	if (traceOn)
		myDebug = "handleLoadCategorySoftkeyGroup():   ";

	if (!mySession || !msgSoftkey || !dbAccess)
		return;

	HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
	HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);

	SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
	if (!feature)
		return;

	if (traceOn)
		Debug::trace("SoftkeyManager", myDebug + "featureNum = " + longtostring(featureNum));

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();

	SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
	if (style && dbCategoriesRM)
	{
		HobRecordNum categoryNum = msgSoftkey->getActionCategoryNumber();

		switch (msgSoftkey->getAction())
		{
			case Action_LoadCategory_Previous:
			{
				if (currentGridData)
					currentGridData->gotoPreviousPage();
				break;
			}

			case Action_LoadCategory_Next:
			{
				if (currentGridData)
					currentGridData->gotoNextPage();
				break;
			}

			default:
			{
				// Find the 'source' category number record in the database.
				if (traceOn)
					Debug::trace("SoftkeyManager", myDebug + "categoryNum = " + longtostring(categoryNum));

				DBCategory *dbSourceCategory = dbCategoriesRM->findNumber(longtostring(categoryNum), longtostring(msgSoftkey->getDeptNum()));
				if (dbSourceCategory)
				{
					if (traceOn)
						Debug::trace("SoftkeyManager", myDebug + "We have a dbSourceCategory.");

					DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;
					DBQuery<DBCategory> *categoriesQuery = (DBQuery<DBCategory> *)NULL;
					DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;
					DBQuery<DBCategory>::const_iterator *categoriesIter = (DBQuery<DBCategory>::const_iterator *)NULL;

					DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
					productsQuery = dbProductsRM->retrieveLinkedtoCategory(dbSourceCategory);
					if (productsQuery)
					{
						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "We have a productsQuery.  size = " + inttostring(productsQuery->size()));

						DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
						productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);

						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "productsIter = " + Hex::toString((long long)productsIter));
					}

					categoriesQuery = dbCategoriesRM->retrieveChildCategories(dbSourceCategory);
					if (categoriesQuery)
					{
						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "We have a categoriesQuery.  size = " + inttostring(categoriesQuery->size()));

						DBQuery<DBCategory>::const_iterator tempIter = categoriesQuery->begin();
						categoriesIter = new DBQuery<DBCategory>::const_iterator(tempIter);

						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "categoriesIter = " + Hex::toString((long long)categoriesIter));
					}

					DBProduct *p = (DBProduct *)NULL;
					DBCategory *c = (DBCategory *)NULL;

					HobString prodName("");
					HobString catName("");

					HobString categoryBgColor;
					if (!dbSourceCategory->isBackgroundColorNull())
						categoryBgColor = Hex::toHTMLColorString((long)dbSourceCategory->getBackgroundColor());

					int ip = 0, ic = 0;
					short whichIsNext = 0;  // 0 = initial state (none have been processed), 1 = product is next, 2 = category is next


					int categoryHorzGridSize = 0;
					int categoryVertGridSize = 0;

					Session *s = dynamic_cast<Session *> (mySession);
					short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

					SoftkeyGroup *fixedSoftkeyGroup = 0;
					FixedRowColMethod fixedRowColMethod = feature->getFixedRowColMethod();
					if (fixedRowColMethod != FRCM_None)
					{
						fixedSoftkeyGroup = SoftkeyGroupManager::loadFixedRowColSoftkeyGroup(RECORD_NOT_FOUND, langNum, dbAccess);
						if (!fixedSoftkeyGroup)
						{
							delete feature;
							feature = 0;
							delete style;
							style = 0;
							return;
						}

						SoftkeyGroup *variableSoftkeyGroup = SoftkeyGroupManager::prepareToLoadSoftkeyGroup(featureNum, RECORD_NOT_FOUND, langNum, dbAccess);
						if (!variableSoftkeyGroup)
						{
							delete feature;
							feature = 0;
							delete style;
							style = 0;
							return;
						}
						else
						{
							//Change size, if the fixed row/col method is specified
							if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
								variableSoftkeyGroup->setVerticalGridSize(variableSoftkeyGroup->getVerticalGridSize() - 1);
							else  // is FRCM_FirstColumn || FRCM_LastColumn
								variableSoftkeyGroup->setHorizontalGridSize(variableSoftkeyGroup->getHorizontalGridSize() - 1);

							categoryHorzGridSize = variableSoftkeyGroup->getHorizontalGridSize();
							categoryVertGridSize = variableSoftkeyGroup->getVerticalGridSize();

							delete variableSoftkeyGroup;
							variableSoftkeyGroup = 0;
						}
					}


					if (currentGridData)
					{
						delete currentGridData;
						currentGridData = 0;
					}
					currentGridData = new SoftkeyGroup(dbAccess);
					if (currentGridData)
					{
						currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
						currentGridData->setSoftkeyFeature(feature);
						currentGridData->setSoftkeyStyle(style);
						currentGridData->setDeptNum(feature->getDeptNum());
						currentGridData->setLanguageSelection(langNum);

						int hGridSize = feature->getDefaultHorizontalGridSize();
						int vGridSize = feature->getDefaultVerticalGridSize();

						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "hGridSize = " + inttostring(hGridSize) + ",  vGridSize = " + inttostring(vGridSize));

						int hCellSize = 1;
						int vCellSize = 1;
						currentGridData->setHorizontalSpacing(hCellSize);
						currentGridData->setVerticalSpacing(vCellSize);

						if (fixedRowColMethod == FRCM_None)
						{
							currentGridData->setHorizontalGridSize(hGridSize);
							currentGridData->setVerticalGridSize(vGridSize);
						} 
						else
						{
							currentGridData->setHorizontalGridSize(categoryHorzGridSize);
							currentGridData->setVerticalGridSize(categoryVertGridSize);
							hGridSize = categoryHorzGridSize;
							vGridSize = categoryVertGridSize;
						}
						currentGridData->setGridCellCount(hGridSize * vGridSize);

						Configuration *config = Configuration::getInstance();

						Softkey prevSoftkey;
						Filename previousFileName;
						previousFileName.setPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
						previousFileName.setFileName(config->getPreviousGraphicFileName1());
						if (previousFileName.getFileNameExtension().empty())
							previousFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));

						prevSoftkey.setBackgroundImage(previousFileName.getFullPath());
						prevSoftkey.setAction(Action_LoadCategory_Previous);
						prevSoftkey.setActionCategoryNumber(categoryNum);
						prevSoftkey.setActionFeatureNumber(featureNum);
						prevSoftkey.setName1(" ");		//	We don't want a name showing up over or under the graphic.
						prevSoftkey.setName2(" ");

						Softkey nextSoftkey;
						Filename nextFileName;
						nextFileName.setPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
						nextFileName.setFileName(config->getNextGraphicFileName1());
						if (nextFileName.getFileNameExtension().empty())
							nextFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));

						nextSoftkey.setBackgroundImage(nextFileName.getFullPath());
						nextSoftkey.setAction(Action_LoadCategory_Next);
						nextSoftkey.setActionCategoryNumber(categoryNum);
						nextSoftkey.setActionFeatureNumber(featureNum);
						nextSoftkey.setName1(" ");		//	We don't want a name showing up, over or under the graphic.
						nextSoftkey.setName2(" ");

						int gridSize = (fixedRowColMethod == FRCM_None) ? ( feature->getDefaultHorizontalGridSize() * feature->getDefaultVerticalGridSize() ) : ( hGridSize * vGridSize );

						if (traceOn)
							Debug::trace("SoftkeyManager", myDebug + "gridSize = " + inttostring(gridSize));

						int groupNum = 0;
						int prevCell = 0;
						int nextCell = prevCell + gridSize - 1;
						int totalCells = (int)productsQuery->size() + (int)categoriesQuery->size();
						int cellNum = 0;
						int hPos = 0, vPos = 0;

						while ((productsIter && *productsIter != productsQuery->end()) || (categoriesIter && *categoriesIter != categoriesQuery->end()))
						{
							if (traceOn)
							{
								HobString myDebug2 = myDebug;
								myDebug2.append("cellNum = ").append(Integer::toString(cellNum));
								myDebug2.append("   totalCells = ").append(Integer::toString(totalCells));
								Debug::trace("SoftkeyManager", myDebug2);
							}
							if (cellNum > 0 && cellNum == prevCell)
							{
								//	Add the Previous button.
								currentGridData->add(prevSoftkey, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);

								// if we add a Prev Cell to the group, we have to increase the totalCells by +1
								// HT-145 Categories are not drawing all assigned items
								totalCells = totalCells + 1;

								if (traceOn)
								{
									HobString myDebug2 = myDebug;
									myDebug2.append("Added a 'Previous' flashkey at ").append(Integer::toString(hPos));
									myDebug2.append(",").append(Integer::toString(vPos));
									Debug::trace("SoftkeyManager", myDebug2);
								}
							}
							else if (cellNum < totalCells && cellNum == nextCell)
							{
								//	Add the Next button.
								currentGridData->add(nextSoftkey, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);
								//	Increment the groupNum.
								++groupNum;

								//	Recalculate the previous & next cells.
								prevCell = nextCell + 1;
								nextCell = prevCell + gridSize - 1;

								// if we add a Next Cell to the group, we have to increase the totalCells by +1
								// HT-145 Categories are not drawing all assigned items
								totalCells = totalCells + 1;

								if (traceOn)
								{
									HobString myDebug2 = myDebug;
									myDebug2.append("Added a 'Next' flashkey at ").append(Integer::toString(hPos));
									myDebug2.append(",").append(Integer::toString(vPos));
									Debug::trace("SoftkeyManager", myDebug2);
								}
							}
							else
							{
								//	Add the Category or the Product.

								if (productsIter && (!categoriesIter || *categoriesIter == categoriesQuery->end()))
								{
									p = new DBProduct(*(*productsIter));
									whichIsNext = 1;
								}
								else if (categoriesIter && (!productsIter || *productsIter == productsQuery->end()))
								{
									c = new DBCategory(*(*categoriesIter));
									whichIsNext = 2;
								}
								else
								{
									/* If the last time through this loop, we selected a product to add to the grid, then we processed the product and incremented
									* the products iterator; so we need to get the new product that the iterator points to.  The category c remains the same as
									* it was before, so just use it.  If the last time through this loop, we selected a category, then the opposite is true; we
									* need to get the new category that the categories iterator now points to and the product remains the same as it was the last
									* time through the loop.
									*/
									if (whichIsNext == 1 || whichIsNext == 0)
									{
										p = new DBProduct(*(*productsIter));
										prodName = "";
										if (p)
										{
											prodName = Softkey::getProductSoftkeyName(p, langNum);
											util_string::toupperL(prodName);
										}
									}
									if (whichIsNext == 2 || whichIsNext == 0)
									{
										c = new DBCategory(*(*categoriesIter));
										catName = "";
										if (c)
										{
											catName = Softkey::getCategorySoftkeyName(c, langNum);
											util_string::toupperL(catName);

										}
									}


									if (std::locale()(catName, prodName))
									{

										whichIsNext = 2;
									}
									else
									{

										whichIsNext = 1;
									}

								}

								if (whichIsNext == 1)
								{
									if (p)
									{
										currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, dbSourceCategory);
										ip++;
										delete p;
										p = 0;
									}
									++(*productsIter);
								}
								else if (whichIsNext == 2)
								{
									if (c)
									{
										currentGridData->addCategorySoftkey(c, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);
										ic++;
										delete c;
										c = 0;
									}
									++(*categoriesIter);
								}
							}
							++cellNum;
							if (++hPos == hGridSize)
							{
								hPos = 0;
								if (++vPos == vGridSize)
									vPos = 0;
							}
						}
						//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
						feature = (SoftkeyFeature *)NULL;
						style = (SoftkeyStyle *)NULL;
					}

					if (productsIter)
					{
						delete productsIter;
					}
					if (productsQuery)
					{
						delete productsQuery;
					}
					productsQuery = (DBQuery<DBProduct>*) NULL;

					if (categoriesIter)
					{
						delete categoriesIter;
					}
					if (categoriesQuery)
					{
						delete categoriesQuery;
					}
					categoriesQuery = (DBQuery<DBCategory>*) NULL;

					if (fixedSoftkeyGroup)
						currentGridData->concatenateSoftkeyGroup(fixedSoftkeyGroup, fixedRowColMethod);

				}
				if (dbSourceCategory)
				{
					delete dbSourceCategory;
				}

				if (currentGridData)
					currentGridData->gotoFirstPage();
				break;
			}
		}
		if (currentGridData)
		{
			// Send the SoftkeyGroup found back to the Browser.
			// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
			Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
			SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
			sgMsg.setFormatType(BrowserFormat);
			currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
			sgMsg.setHfcObject(currentGridData);

			// Send the SoftkeyFeature to the UI (Browser)
			mySession->send(&sgMsg, Messenger::DEST_UI);
		}
	}
	if (feature)
		delete feature;
	if (style)
		delete style;
}


void SoftkeyManager::handleSelfServeLoadCategorySoftkeyGroup(Softkey *msgSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "handleSelfServeLoadCategorySoftkeyGroup():   ";

	if (!mySession || !msgSoftkey || !dbAccess)
		return;

	SoftkeyFeature *feature = 0;
	SoftkeyStyle *style = 0;

	switch (msgSoftkey->getAction())
	{
	case Action_LoadCategory_Previous:
	{
		if (currentGridData)
			currentGridData->gotoPreviousPage();
		break;
	}

	case Action_LoadCategory_Next:
	{
		if (currentGridData)
			currentGridData->gotoNextPage();
		break;
	}

	default:
	{
		HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
		HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
		if (!feature)
			return;

		DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
		if (!dbCategoriesRM)
			return;

		SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
		if (!style)
			return;

		HobRecordNum categoryNum = msgSoftkey->getActionCategoryNumber();

		// Find the 'source' category number record in the database.
		DBCategory *dbSourceCategory = dbCategoriesRM->findNumber(longtostring(categoryNum), longtostring(msgSoftkey->getDeptNum()));
		if (dbSourceCategory)
		{
			DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;
			DBQuery<DBCategory> *categoriesQuery = (DBQuery<DBCategory> *)NULL;
			DBQuery<DBProduct>::const_iterator *productsIter = (DBQuery<DBProduct>::const_iterator *)NULL;
			DBQuery<DBCategory>::const_iterator *categoriesIter = (DBQuery<DBCategory>::const_iterator *)NULL;

			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			productsQuery = dbProductsRM->retrieveLinkedtoCategory(dbSourceCategory);
			if (productsQuery)
			{
				DBQuery<DBProduct>::const_iterator tempIter = productsQuery->begin();
				productsIter = new DBQuery<DBProduct>::const_iterator(tempIter);
			}

			categoriesQuery = dbCategoriesRM->retrieveChildCategories(dbSourceCategory);
			if (categoriesQuery)
			{
				DBQuery<DBCategory>::const_iterator tempIter = categoriesQuery->begin();
				categoriesIter = new DBQuery<DBCategory>::const_iterator(tempIter);
			}

			DBProduct *p = (DBProduct *)NULL;
			DBCategory *c = (DBCategory *)NULL;

			HobString prodName("");
			HobString catName("");

			HobString categoryBgColor;
			if (!dbSourceCategory->isBackgroundColorNull())
				categoryBgColor = Hex::toHTMLColorString((long)dbSourceCategory->getBackgroundColor());

			int ip = 0, ic = 0;
			short whichIsNext = 0;  // 0 = initial state (none have been processed), 1 = product is next, 2 = category is next

			if (currentGridData)
			{
				delete currentGridData;
				currentGridData = 0;
			}
			currentGridData = new SoftkeyGroup(dbAccess);
			if (currentGridData)
			{
				currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
				currentGridData->setSoftkeyFeature(feature);
				currentGridData->setSoftkeyStyle(style);
				currentGridData->setDeptNum(feature->getDeptNum());

				Session *s = dynamic_cast<Session *> (mySession);
				short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
				currentGridData->setLanguageSelection(langNum);

				int hGridSize = feature->getDefaultHorizontalGridSize();
				int vGridSize = feature->getDefaultVerticalGridSize();
				int hCellSize = 1;
				int vCellSize = 1;

				currentGridData->setHorizontalGridSize(hGridSize);
				currentGridData->setVerticalGridSize(vGridSize);
				currentGridData->setHorizontalSpacing(hCellSize);
				currentGridData->setVerticalSpacing(vCellSize);
				currentGridData->setGridCellCount(hGridSize * vGridSize);

				//					int gridSize = feature->getDefaultHorizontalGridSize() * feature->getDefaultVerticalGridSize();
				//					int groupNum = 0;
				//					int prevCell = 0;
				//					int nextCell = prevCell + gridSize - 1;
				int totalCells = (int)productsQuery->size() + (int)categoriesQuery->size();
				int cellNum = 0;
				int hPos = 0, vPos = 0;

				// Assume either a product or category will be added to the grid with this iteration of the loop.
				bool flashkeyAddedToGrid = true;
				while ((productsIter && *productsIter != productsQuery->end()) || (categoriesIter && *categoriesIter != categoriesQuery->end()))
				{
					if (traceOn)
					{
						HobString myDebug2 = myDebug;
						myDebug2.append("cellNum = ").append(Integer::toString(cellNum));
						myDebug2.append("   totalCells = ").append(Integer::toString(totalCells));
						Debug::trace("SoftkeyManager", myDebug2);
					}

					//	Add the Category or the Product.

					if (productsIter && (!categoriesIter || *categoriesIter == categoriesQuery->end()))
					{
						p = new DBProduct(*(*productsIter));
						whichIsNext = 1;
					}
					else if (categoriesIter && (!productsIter || *productsIter == productsQuery->end()))
					{
						c = new DBCategory(*(*categoriesIter));
						whichIsNext = 2;
					}
					else
					{
						/* If the last time through this loop, we selected a product to add to the grid, then we processed the product and incremented
						* the products iterator; so we need to get the new product that the iterator points to.  The category c remains the same as
						* it was before, so just use it.  If the last time through this loop, we selected a category, then the opposite is true; we
						* need to get the new category that the categories iterator now points to and the product remains the same as it was the last
						* time through the loop.
						*/
						if (whichIsNext == 1 || whichIsNext == 0)
						{
							p = new DBProduct(*(*productsIter));
							prodName = "";
							if (p)
							{
								prodName = Softkey::getProductSoftkeyName(p, langNum);
								util_string::toupperL(prodName);
							}
						}
						if (whichIsNext == 2 || whichIsNext == 0)
						{
							c = new DBCategory(*(*categoriesIter));
							catName = "";
							if (c)
							{
								catName = Softkey::getCategorySoftkeyName(c, langNum);
								util_string::toupperL(catName);
							}
						}

						if (std::locale()(catName, prodName))
							whichIsNext = 2;
						else
							whichIsNext = 1;
					}

					if (whichIsNext == 1)
					{
						if (p)
						{
							if (p->getPrice() <= 0)
								flashkeyAddedToGrid = false;
							else
							{
								currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, dbSourceCategory);
								ip++;
							}
							delete p;
							p = 0;
						}
						++(*productsIter);
					}
					else if (whichIsNext == 2)
					{
						if (c)
						{
							currentGridData->addCategorySoftkey(c, featureNum, langNum, hPos, vPos, hCellSize, vCellSize);
							ic++;
							delete c;
							c = 0;
						}
						++(*categoriesIter);
					}

					if (flashkeyAddedToGrid)
					{
						++cellNum;
						if (++hPos == hGridSize)
						{
							hPos = 0;
							if (++vPos == vGridSize)
								vPos = 0;
						}
					}
					else
						flashkeyAddedToGrid = true;  // reset the assumption that a flashkey will be added.
				}
				//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
				feature = (SoftkeyFeature *)NULL;
				style = (SoftkeyStyle *)NULL;
			}

			if (productsIter)
			{
				delete productsIter;
			}
			if (productsQuery)
			{
				delete productsQuery;
			}
			productsQuery = (DBQuery<DBProduct>*) NULL;

			if (categoriesIter)
			{
				delete categoriesIter;
			}
			if (categoriesQuery)
			{
				delete categoriesQuery;
			}
			categoriesQuery = (DBQuery<DBCategory>*) NULL;
		}
		if (dbSourceCategory)
		{
			delete dbSourceCategory;
		}
		if (currentGridData)
			currentGridData->gotoFirstPage();
		break;
	}
	}

	if (currentGridData)
	{
		// Send the SoftkeyGroup found back to the Browser.
		// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
		Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
		SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
		sgMsg.setFormatType(BrowserFormat);
		currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
		sgMsg.setHfcObject(currentGridData);

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send(&sgMsg, Messenger::DEST_UI);
	}
	if (feature)
		delete feature;
	if (style)
		delete style;
}


/**************************************************************************************************************/
/**************************************************************************************************************/
/**************************************************************************************************************/


void SoftkeyManager::handleLoadSpecialsSoftkeyGroup(Softkey *msgSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "handleLoadSpecialsSoftkeyGroup():   ";

	if (!msgSoftkey || !dbAccess)
		return;

	HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
	HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
	SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
	if (!feature)
		return;

	SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
	if (style)
	{
		switch (msgSoftkey->getAction())
		{
			case Action_LoadSpecials_Previous:
			{
				if (currentGridData)
					currentGridData->gotoPreviousPage();
				break;
			}

			case Action_LoadSpecials_Next:
			{
				if (currentGridData)
					currentGridData->gotoNextPage();
				break;
			}

			default:
			{
				DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
				DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;

				DBSpecialProgramsRM *dbSpecialProgramsRM = 0;
				DBSpecialProgram *dbSpecialProgram = 0;

				HobRecordNum specialsNum = Long::valueOf(msgSoftkey->getActionParameter());
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("specialsNum = ").append(Integer::toString(specialsNum));
					Debug::trace("SoftkeyManager", myDebug2);
				}
				if (specialsNum > 0)
				{
					dbSpecialProgramsRM = dbAccess->getDBSpecialsRM();
					if (dbSpecialProgramsRM)
					{
						dbSpecialProgram = dbSpecialProgramsRM->findNumber( longtostring(specialsNum), longtostring(msgSoftkey->getDeptNum()) );
						if (dbSpecialProgram)
						{
							productsQuery = dbProductsRM->retrieveSpecials(dbSpecialProgram);
							delete dbSpecialProgram;
						}
					}
				}
				else
				{
					productsQuery = dbProductsRM->retrieveAllSpecials( longtostring(msgSoftkey->getDeptNum()) );
				}

				if (productsQuery)
				{
					buildSoftkeyGridData(feature, style, productsQuery);

					delete productsQuery;
					productsQuery = 0;

					//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
					feature = 0;
					style = 0;
				}
				
				if (currentGridData)
					currentGridData->gotoFirstPage();
				break;
			}
		}
		if (mySession && currentGridData)
		{
			// Send the SoftkeyGroup found back to the Browser.
			// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
			Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
			SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
			sgMsg.setFormatType(BrowserFormat);
			currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
			sgMsg.setHfcObject(currentGridData);

			// Send the SoftkeyFeature to the UI (Browser)
			mySession->send(&sgMsg, Messenger::DEST_UI);
		}
	}
	if (feature)
		delete feature;
	if (style)
		delete style;
}


void SoftkeyManager::handleSelfServeLoadSpecialsSoftkeyGroup(Softkey *msgSoftkey)
{
	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "handleSelfServeLoadSpecialsSoftkeyGroup():   ";

	if (!mySession || !msgSoftkey || !dbAccess)
		return;

	SoftkeyFeature *feature = 0;
	SoftkeyStyle *style = 0;

	switch (msgSoftkey->getAction())
	{
		case Action_LoadSpecials_Previous:
		{
			if (currentGridData)
				currentGridData->gotoPreviousPage();
			break;
		}

		case Action_LoadSpecials_Next:
		{
			if (currentGridData)
				currentGridData->gotoNextPage();
			break;
		}

		default:
		{
			HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
			HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
			if (!feature)
				return;

			SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
			if (!style)
				return;

			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			DBQuery<DBProduct> *productsQuery = (DBQuery<DBProduct> *)NULL;

			DBSpecialProgramsRM *dbSpecialProgramsRM = 0;
			DBSpecialProgram *dbSpecialProgram = 0;

			HobRecordNum specialsNum = Long::valueOf(msgSoftkey->getActionParameter());
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("specialsNum = ").append(Integer::toString(specialsNum));
				Debug::trace("SoftkeyManager", myDebug2);
			}
			if (specialsNum > 0)
			{
				dbSpecialProgramsRM = dbAccess->getDBSpecialsRM();
				if (dbSpecialProgramsRM)
				{
					dbSpecialProgram = dbSpecialProgramsRM->findNumber( longtostring(specialsNum), longtostring(msgSoftkey->getDeptNum()) );
					if (dbSpecialProgram)
					{
						productsQuery = dbProductsRM->retrieveSpecials(dbSpecialProgram);
						delete dbSpecialProgram;
					}
				}
			}
			else
			{
				productsQuery = dbProductsRM->retrieveAllSpecials( longtostring(msgSoftkey->getDeptNum()) );
			}

			if (productsQuery)
			{
				buildSelfServeSoftkeyGridData(feature, style, productsQuery);

				delete productsQuery;
				productsQuery = 0;

				//	Clear both pointers as the SoftkeyGroup has control of them and we don't want to delete them twice.
				feature = 0;
				style = 0;
			}
			
			if (currentGridData)
				currentGridData->gotoFirstPage();
			break;
		}
	}

	if (currentGridData)
	{
		// Send the SoftkeyGroup found back to the Browser.
		// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
		Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
		SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
		sgMsg.setFormatType(BrowserFormat);
		currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
		sgMsg.setHfcObject(currentGridData);

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send(&sgMsg, Messenger::DEST_UI);
	}

	if (feature)
		delete feature;
	if (style)
		delete style;
}


/**************************************************************************************************************/
/**************************************************************************************************************/
/**************************************************************************************************************/


// pjr-383 - Need to use the fixed row/column?  Yes, since a "Load Best Sellers button could be in the fixed row/col of a group in feature Reserved_FixedRowColGrid
void SoftkeyManager::handleLoadMostCommonSoftkeyGroup(Softkey *msgSoftkey)
{
//	bool traceOn = Debug::isTracing("SoftkeyManager");
	HobString myDebug = "handleLoadMostCommonSoftkeyGroup():   ";

	if (!mySession || !msgSoftkey || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	SoftkeyFeature *feature = 0;
	SoftkeyStyle *style = 0;

	switch (msgSoftkey->getAction())
	{
		case Action_LoadMostCommon_Previous:
		{
			if (currentGridData)
				currentGridData->gotoPreviousPage();
			break;
		}

		case Action_LoadMostCommon_Next:
		{
			if (currentGridData)
				currentGridData->gotoNextPage();
			break;
		}

		default:
		{
			HobRecordNum featureNum = msgSoftkey->getActionFeatureNumber();
			HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			SoftkeyFeature *feature = SoftkeyFeature::find(featureNum, lDept, dbAccess);
			if (!feature)
				return;

			SoftkeyStyle *style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
			if (!style)
			{
				delete feature;
				feature = 0;
				return;
			}

			int bestSellersHorzGridSize = 0;
			int bestSellersVertGridSize = 0;
			int bestSellersHorzSpacing = 0;
			int bestSellersVertSpacing = 0;

			SoftkeyGroup *fixedSoftkeyGroup = 0;
			FixedRowColMethod fixedRowColMethod = feature->getFixedRowColMethod();
			if (fixedRowColMethod != FRCM_None)
			{
				Session *s = dynamic_cast<Session *> (mySession);
				short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

				fixedSoftkeyGroup = SoftkeyGroupManager::loadFixedRowColSoftkeyGroup(RECORD_NOT_FOUND, langNum, dbAccess);
				if (!fixedSoftkeyGroup)
				{
					delete feature;
					feature = 0;
					delete style;
					style = 0;
					return;
				}
				
				SoftkeyGroup *variableSoftkeyGroup = SoftkeyGroupManager::prepareToLoadSoftkeyGroup(featureNum, RECORD_NOT_FOUND, langNum, dbAccess);
				if (!variableSoftkeyGroup)
				{
					delete feature;
					feature = 0;
					delete style;
					style = 0;
					return;
				}
				else
				{
					//Change size, if the fixed row/col method is specified
					if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
						variableSoftkeyGroup->setVerticalGridSize(variableSoftkeyGroup->getVerticalGridSize() - 1);
					else  // is FRCM_FirstColumn || FRCM_LastColumn
						variableSoftkeyGroup->setHorizontalGridSize(variableSoftkeyGroup->getHorizontalGridSize() - 1);

					bestSellersHorzGridSize = variableSoftkeyGroup->getHorizontalGridSize();
					bestSellersVertGridSize = variableSoftkeyGroup->getVerticalGridSize();
					bestSellersHorzSpacing = variableSoftkeyGroup->getHorizontalSpacing();
					bestSellersVertSpacing = variableSoftkeyGroup->getVerticalSpacing();

					delete variableSoftkeyGroup;
					variableSoftkeyGroup = 0;
				}

			}
			else
			{
				HobRecordNum groupNumber = feature->getDefaultGroupNumber();
				SoftkeyGroup *readSoftkeyGroup = SoftkeyGroup::find(groupNumber, lDept, dbAccess);
				if (readSoftkeyGroup)
				{
					bestSellersHorzGridSize = readSoftkeyGroup->getHorizontalGridSize();
					bestSellersVertGridSize = readSoftkeyGroup->getVerticalGridSize();
					bestSellersHorzSpacing = readSoftkeyGroup->getHorizontalSpacing();
					bestSellersVertSpacing = readSoftkeyGroup->getVerticalSpacing();

					delete readSoftkeyGroup;
					readSoftkeyGroup = NULL;
				}
				else
				{
					bestSellersHorzGridSize = feature->getDefaultHorizontalGridSize();
					bestSellersVertGridSize = feature->getDefaultVerticalGridSize();
					bestSellersHorzSpacing = feature->getHorizontalSpacing();
					bestSellersVertSpacing = feature->getVerticalSpacing();
				}
			}

			Configuration *config = Configuration::getInstance();

			int cellCount = 0;
			if (config->toBoolCfgI(CfgInternal::MAXBESTSELLERSOFTKEYSFROMFEATURE))
				cellCount = bestSellersHorzGridSize * bestSellersVertGridSize;
			else
				cellCount = config->toIntCfgI(CfgInternal::MAXBESTSELLERSOFTKEYS);

			Debug::trace("SoftkeyManager", myDebug + HobString(" - cellCount:") + HobString(Integer::toString(cellCount)));
			if (cellCount <= 0)
			{
				delete feature;
				delete style;
				return;
			}

			/*******************************************************************************************
			 *  Get the most common products for the current language and 'hour'.
			 ******************************************************************************************/
			DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
			DBProductStatisticsRM *psRM = dbAccess->getDBProductStatisticsRM();
			if (!dbProductsRM || !psRM)
			{
				delete feature;
				delete style;
				return;
			}

			Debug::trace("SoftkeyManager", myDebug + HobString(" - using MostCommon presentation method"));

			int hour;
			LanguageSelection langNum;
			std::string lang;
/*
			if (config->isSelfServiceScale())
			{
				// determine the hour for the statistics record to be updated
				hour = config->determineHourForQuery();
				langNum = (s && !config->toBoolCfgI(CfgInternal::ONLYPRILANGFORPRODSTATS)) ? s->getCurrentLanguageSelection() : PrimaryLanguage;
				lang = (langNum == PrimaryLanguage) ? config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode) : config->toStringCfg(fieldOffsetCfgSecondaryLanguageCode);
			}
			else
			{
*/
				//starting with Fastrack, these statistics aren't just for self-serve anymore

				// For Fastrack, we're not using a productStatistics hour method, use 0, which means to just accumulate all product stats into one bucket (for the plu)
				hour = 0;
				// For Fastrack, (non-selfServe), we're not separating the statistics for the language, so just use the primary language.
				langNum = PrimaryLanguage;
				lang = config->toStringCfg(fieldOffsetCfgPrimaryLanguageCode);
//			}

			Debug::trace("SoftkeyManager", myDebug + HobString("handleLoadMostCommonSoftkeyGroup - using language: ") + lang);
			Debug::trace("SoftkeyManager", myDebug + HobString("handleLoadMostCommonSoftkeyGroup - using hour: ") + HobString(Integer::toString(hour)));

			DBQuery<DBProductStatistic> *prodStatsQuery = psRM->retrieveMostPrinted(lang, hour);
			if (!prodStatsQuery)
			{
				Debug::trace("SoftkeyManager", myDebug + HobString(" - prodStatsQuery is null or empty"));
				// Don't have any items to send.
				// Should I send an 'error' message to indicate this?
				return;
			}
			else
				Debug::trace("SoftkeyManager", myDebug + HobString(" - prodStatsQuery is not null"));


			if (currentGridData)
			{
				delete currentGridData;
				currentGridData = 0;
			}
			currentGridData = new SoftkeyGroup(dbAccess);
			if (currentGridData)
			{
				currentGridData->setGroupNumber(feature->getDefaultGroupNumber());
				currentGridData->setSoftkeyFeature(feature);
				currentGridData->setSoftkeyStyle(style);
				currentGridData->setDeptNum(lDept);

				currentGridData->setLanguageSelection(langNum);

				int hGridSize = bestSellersHorzGridSize;
				int vGridSize = bestSellersVertGridSize;
				int hSpacingSize = bestSellersHorzSpacing;
				int vSpacingSize = bestSellersVertSpacing;

				int hCellSize = 1;
				int vCellSize = 1;

				currentGridData->setHorizontalGridSize(hGridSize);
				currentGridData->setVerticalGridSize(vGridSize);
				currentGridData->setHorizontalSpacing(hSpacingSize);
				currentGridData->setVerticalSpacing(vSpacingSize);
				currentGridData->setGridCellCount(hGridSize * vGridSize);

//				int groupNum = 0;
//				int prevCell = 0;
//				int nextCell = prevCell + gridSize - 1;
				int hPos = 0, vPos = 0;

				int i = 0;
				Debug::trace("SoftkeyManager", myDebug + HobString(" - before getting iterator"));
				DBQuery<DBProductStatistic>::const_iterator prodStatsQueryIter = prodStatsQuery->begin();
				//DBQuery<DBProduct>::const_iterator prodStatsQueryIter = prodStatsQuery->begin();

				bool flashkeyAddedToGrid = true;
				while (i < cellCount && (prodStatsQueryIter != prodStatsQuery->end()))
				{
					Debug::trace("SoftkeyManager", myDebug + HobString(" - next loop through the query"));
					DBProduct* p = dbProductsRM->findLinkedRecord(&*prodStatsQueryIter);
					Debug::trace("SoftkeyManager", myDebug + HobString(" - after finding dbProduct record from the statistics record"));
					//DBProduct *p = new DBProduct(*prodStatsQueryIter);

					++prodStatsQueryIter;

					if (p)
					{
						if (Configuration::getInstance()->isSelfServiceScale() && p->getPrice() <= 0)
							flashkeyAddedToGrid = false;
						else if ( p->getDeptNumber() != Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) )
							flashkeyAddedToGrid = false;
						else
						{
							i++;
							currentGridData->addProductSoftkey(p, featureNum, langNum, hPos, vPos, hCellSize, vCellSize, 0);
						}
						delete p;
					}
					else
						Debug::trace("SoftkeyManager", myDebug + HobString(" - didn't get the dbproduct record"));

					if (flashkeyAddedToGrid)
					{
						if (++hPos == hGridSize)
						{
							hPos = 0;
							if (++vPos == vGridSize)
								vPos = 0;
						}
					}
					else
						flashkeyAddedToGrid = true;  // reset the assumption that a flashkey will be added.
				}

				//currentGridData->blankFillLists(cellCount);


				if (fixedSoftkeyGroup)
					currentGridData->concatenateSoftkeyGroup(fixedSoftkeyGroup, fixedRowColMethod);


				Debug::trace("SoftkeyManager", myDebug + HobString(" - done processing query - i:") + HobString(Integer::toString(i)));
			}

			if (prodStatsQuery)
				delete prodStatsQuery;

			//	Clear both pointers as the currentGridData has control of them and we don't want to delete them twice.
			feature = 0;
			style = 0;

			if (currentGridData)
				currentGridData->gotoFirstPage();
			break;
		}
	}

	if (currentGridData)
	{
		// Send the SoftkeyGroup found back to the Browser.
		// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
		Debug::trace("SoftkeyManager", "Sending LoadSoftkeyGroup msg");
		SessionMessage sgMsg(SoftkeyDev, LoadSoftkeyGroup);
		sgMsg.setFormatType(BrowserFormat);
		currentGridData->setFormatType(SoftkeyGroupSelfServeFormat);
		currentGridData->setLanguageSelection(s->getCurrentLanguageSelection());
		sgMsg.setHfcObject(currentGridData);

		// Send the SoftkeyFeature to the UI (Browser)
		mySession->send(&sgMsg, Messenger::DEST_UI);
	}
	if (feature)
		delete feature;
	if (style)
		delete style;
}
