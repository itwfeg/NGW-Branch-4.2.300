/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "EmailAlertsManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "AllEmailAlerts.h"
#include "HobList.h"
#include "HobEmailAlert.h"
#include "DataTypeEvent.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


//#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EmailAlertsManager::dataType = 0;
std::string EmailAlertsManager::dataTypeName("EmailAlertsManager"); 
#endif


EmailAlertsManager::EmailAlertsManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

EmailAlertsManager::EmailAlertsManager(hfc::HFCManagerContainer *container)
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


EmailAlertsManager::~EmailAlertsManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentEmailAlert)
		delete currentEmailAlert;

	destroyQueriesAndIters();
}


void EmailAlertsManager::init()
{
	mySession = 0;

	currentEmailAlert	= 0;

	allEmailAlertsQuery = 0;
	allEmailAlertsIter  = 0;
}


DBSession *EmailAlertsManager::getMySession()
{
	return mySession;
}
void EmailAlertsManager::setMySession(DBSession *session)
{
	mySession = session;
}


void EmailAlertsManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than EmailAlertsDev.
	if (!msg)
		return;

	HobEmailAlert * msgEmailAlert = dynamic_cast<HobEmailAlert *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("EmailAlertsManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgEmailAlert);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgEmailAlert);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		default:
			Debug::trace("EmailAlertsManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


// If the first record was not read, this method will return NULL.
HobEmailAlert *EmailAlertsManager::firstEmailAlert()
{
	Debug::trace("EmailAlertsManager", HobString("firstEmailAlert()"));

	if (!dbAccess)
		return (HobEmailAlert *)NULL;

	DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
	if (!dbEmailAlertsRM)
		return (HobEmailAlert *)NULL;

	// If the user calls firstEmailAlert() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	Debug::trace("EmailAlertsManager", HobString("firstEmailAlert():   Calling retrieveAll."));
	allEmailAlertsQuery = dbEmailAlertsRM->retrieveAll();
	Debug::trace("EmailAlertsManager", HobString("firstEmailAlert():   allEmailAlertsQuery = ") + HobString(Hex::toString((long long) allEmailAlertsQuery)));

	// Couldn't get the query from which to access the products, just return false.
	if (!allEmailAlertsQuery)
		return (HobEmailAlert *)NULL;

	DBQuery<DBEmailAlert>::const_iterator tempIter = allEmailAlertsQuery->begin();
	if (tempIter == allEmailAlertsQuery->end())
	{
		Debug::trace("EmailAlertsManager", HobString("firstEmailAlert():   No emailAlerts in the Query."));
		return (HobEmailAlert *)NULL;
	}

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextEmailAlert()
	 * method call below).
	 */
	allEmailAlertsIter = new DBQuery<DBEmailAlert>::const_iterator(tempIter);
	DBEmailAlert *e = new DBEmailAlert(*(*allEmailAlertsIter));
	HobEmailAlert *email = new HobEmailAlert(e, dbAccess);

	Debug::trace("EmailAlertsManager", HobString("firstEmailAlert():   emailAlert = ") + HobString(Hex::toString((long long) email)));
	return email;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstEmailAlert() method, the first call to this method will
 * result in the first record being returned.
 */
HobEmailAlert *EmailAlertsManager::nextEmailAlert()
{
	if (!dbAccess)
		return (HobEmailAlert *)NULL;

	if (allEmailAlertsQuery && !allEmailAlertsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allEmailAlertsQuery || !allEmailAlertsIter)
		return firstEmailAlert();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allEmailAlertsIter);
	if (*allEmailAlertsIter == allEmailAlertsQuery->end())
	{
		Debug::trace("EmailAlertsManager", HobString("nextEmailAlert():   We are at the end of the Query."));
		/* reclaim the memory from the query and the iterator used to traverse all products in the db. */
		destroyQueriesAndIters();
		return (HobEmailAlert *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBEmailAlert copy constructor in order for the HobEmailAlert to
	 * control the destruction of the DBEmailAlert record that the iterator contains; otherwise, the DBEmailAlert will
	 * be deleted when the iterator goes out of scope.  The DBEmailAlert copy will be deleted by the HobEmailAlert record
	 * created below.
	 */
	DBEmailAlert *e = new DBEmailAlert(*(*allEmailAlertsIter));
	HobEmailAlert *email = new HobEmailAlert(e, dbAccess);
	Debug::trace("EmailAlertsManager", HobString("nextEmailAlert():   emailAlert = ") + HobString(Hex::toString((long long) email)));
	return email;
}


/* Provide a countAll method so the caller doesn't need to know about a DBEmailAlertsRM.
 */
int EmailAlertsManager::countAllEmailAlerts()
{
	if (!dbAccess)
		return -1;

	DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
	if (!dbEmailAlertsRM)
		return -1;

	return dbEmailAlertsRM->countAll();
}


void EmailAlertsManager::setCurrentEmailAlert(HobEmailAlert *emailAlert)
{
	if (currentEmailAlert)
		delete currentEmailAlert;
	currentEmailAlert = emailAlert;
}


void EmailAlertsManager::handleRead(HobEmailAlert *msgEmailAlert)
{
	if (!msgEmailAlert || !dbAccess)
		return;

	HobEmailAlert *lookedUpEmailAlert = HobEmailAlert::findEmailAlert(msgEmailAlert->getNum(), dbAccess);

	if (!lookedUpEmailAlert || lookedUpEmailAlert->getErrorCode() != RECORD_OK)
	{
		if (mySession)
		{
			//	Do nothing.
		}

		if (lookedUpEmailAlert)
			delete lookedUpEmailAlert;
		return;
	}

	// We have a new current EmailAlert.
	setCurrentEmailAlert(lookedUpEmailAlert);

	if (mySession)
	{
		// Send the EmailAlert found back to the Browser.
		SessionMessage eaData(EmailAlertsDev, DtWrite);
		eaData.setFormatType(BrowserFormat);
		eaData.setHfcObject(currentEmailAlert);

		// Send the product to the UI (Browser)
		mySession->send(&eaData, Messenger::DEST_UI);
	}
}


void EmailAlertsManager::handleReadAll()
{
	Debug::trace("EmailAlertsManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	AllEmailAlerts allEmailAlerts;
	allEmailAlerts.findAll(dbAccess);

	SessionMessage allEmailAlertsMsg(EmailAlertsDev, DtWriteAll);
	allEmailAlertsMsg.setFormatType(BrowserFormat);
	allEmailAlertsMsg.setHfcObject(&allEmailAlerts);
	mySession->send(&allEmailAlertsMsg, Messenger::DEST_UI);
}


void EmailAlertsManager::handleWrite(HobEmailAlert *modifiedEmailAlert)
{
	Debug::trace("EmailAlertsManager", HobString("handleWrite(): currentEmailAlert = ") + HobString(Hex::toString((long long) currentEmailAlert)) +
										HobString("  , modifiedEmailAlert = ") + HobString(Hex::toString((long long) modifiedEmailAlert)));

	if ((!modifiedEmailAlert) || (!currentEmailAlert))
		return;

  	currentEmailAlert->update(modifiedEmailAlert);
}


void EmailAlertsManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	HobEmailAlert * msgEmailAlert = dynamic_cast<HobEmailAlert *> (msg->getHfcObject());
	if (!msgEmailAlert || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::EmailAlerts))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobEmailAlert *existingEmailAlert = HobEmailAlert::findEmailAlert(msgEmailAlert->getNum(), dbAccess);
		if (existingEmailAlert)
		{
			existingEmailAlert->deleteRecord();
			Debug::trace("EmailAlertsManager", HobString("handleDelete - EmailAlert deleted - emailAlert Num:") +
														HobString(Long::toString((long)msgEmailAlert->getNum())));
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::EmailAlerts))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingEmailAlert, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingEmailAlert;
		}
		else
			Debug::trace("EmailAlertsManager", HobString("handleDelete - EmailAlert to be deleted was not in the database to delete - categoryNum:") +
														HobString(Long::toString((long)msgEmailAlert->getNum())));
	}
}


void EmailAlertsManager::handleDeleteAll()
{
	Debug::trace("EmailAlertsManager", "handleDeleteAll - deleting all emailAlerts");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::EmailAlerts))
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

		DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
		if (!dbEmailAlertsRM)
			return;

		dbEmailAlertsRM->deleteAll();

		destroyQueriesAndIters();

		Debug::trace("EmailAlertsManager", "handleDeleteAll - will create reserved records next");
		HobEmailAlert::verifyReservedRecords(dbAccess);
	}

	Debug::trace("EmailAlertsManager", "handleDeleteAll - leaving");
}


void EmailAlertsManager::handleUpdateAndSave(HobEmailAlert *msgEmailAlert)
{
	Debug::trace("EmailAlertsManager", HobString("handleUpdateAndSave(): currentEmailAlert = ") + HobString(Hex::toString((long long) currentEmailAlert)));
	if (currentEmailAlert)
	{
		if (msgEmailAlert)
		  	currentEmailAlert->update(msgEmailAlert);
		handleSaveRecord(currentEmailAlert);
	}
}

void EmailAlertsManager::handleSaveRecord(HobEmailAlert *recordToSave)
{
	if (Debug::isTracing("EmailAlertsManager"))
	{
		HobString myDebug("handleSave(): recordToSave = ");
		myDebug.append(Hex::toString((long long) recordToSave));
		Debug::trace("EmailAlertsManager", myDebug);
	}
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::EmailAlerts))
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


void EmailAlertsManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	HobString myDebug("handleRestore(): ");

	HobEmailAlert *restoreEmailAlert = dynamic_cast<HobEmailAlert *> (msg->getHfcObject());
	if (!restoreEmailAlert || !dbAccess)
	{
		if (Debug::isTracing("EmailAlertsManager"))
		{
			if (!restoreEmailAlert)
				myDebug.append("  restoreEmailAlert is NULL!");
			if (!dbAccess)
				myDebug.append("  dbAccess is NULL!");
			Debug::trace("EmailAlertsManager", myDebug);
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::EmailAlerts))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Debug::trace("EmailAlertsManager", HobString("handleRestore(): restoreEmailAlert number = ") + HobString(Long::toString(restoreEmailAlert->getNum())));
		restoreEmailAlert->setDBAccess(dbAccess);

		HobEmailAlert *existingEmailAlert = HobEmailAlert::findEmailAlert(restoreEmailAlert->getNum(), dbAccess);
		Debug::trace("EmailAlertsManager", HobString("handleRestore(): existingEmailAlert = ") + HobString(Hex::toString((long long) existingEmailAlert)));
		if (existingEmailAlert)
		{
			Debug::trace("EmailAlertsManager", HobString("handleRestore(): existingEmailAlert->isRecordValid() = ") + HobString(Bool::toString(existingEmailAlert->isRecordValid())));
			if (existingEmailAlert->isRecordValid())
			{
				existingEmailAlert->update(restoreEmailAlert);
				existingEmailAlert->saveRecord();
			}
			else
			{
				existingEmailAlert->deleteRecord();
				restoreEmailAlert->saveRecord();
			}

			delete existingEmailAlert;
		}
		else
			restoreEmailAlert->saveRecord();
	}
}


#if 0
void EmailAlertsManager::handleEmailAlertWriteMessage(HobEmailAlert *modifiedHobEmailAlert)
{
	if ((!modifiedProduct) || (!currentEmailAlert))
		return;

	Debug::trace("EmailAlertsManager.HobEmailAlert", HobString("handleProductWriteMessage(): currentEmailAlert = ") + HobString(Hex::toString((long int) currentEmailAlert)) + HobString("  , modifiedProduct = ") + HobString(Hex::toString((long int) modifiedProduct)));
  	currentEmailAlert->updateProduct(modifiedProduct, mySession, true);

	if (mySession && mySession->isInOperateMode() && mySession->isPrimarySession())
	{
		// Send a HobEmailAlert Change message to the RTC so it knows it is dealing with a modified plu record.
		SessionMessage eaData(ProductDev, ProductChange);
		eaData.setFormatType(InternalFormat);
		HfcProductChangeMsg hpcm(mySession, currentEmailAlert);
		eaData.setHfcObject(&hpcm);

		// Send the product to the UI (Browser)
		mySession->send(&eaData, Messenger::DEST_RTC);
	}
}
#endif

void EmailAlertsManager::showAllEmailAlerts()
{
	int totalRecords = countAllEmailAlerts();
	if ((totalRecords > 0) && Debug::isTracing("EmailAlertsManager"))
	{
		Debug::trace("EmailAlertsManager", HobString("    showAllEmailAlerts():   totalRecords = ") + HobString(Integer::toString(totalRecords)));
		DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
		if (dbEmailAlertsRM)
		{
			DBQuery<DBEmailAlert> *myQuery = dbEmailAlertsRM->retrieveAll();
			if (myQuery)
			{
				DBQuery<DBEmailAlert>::const_iterator tempIter = myQuery->begin();
				while (tempIter != myQuery->end())
				{
//					DBQuery<DBEmailAlert> *myIter = new DBQuery<DBEmailAlert>::const_iterator(tempIter);
					DBEmailAlert *e = new DBEmailAlert(*tempIter);
					if (e)
					{
						HobEmailAlert *email = new HobEmailAlert(e, dbAccess);
						if (email)
						{
							email->show();
							delete email;
						}
						else
							delete e;
					}
					++(tempIter);
				}
				delete myQuery;
			}
		}
	}
}


void EmailAlertsManager::destroyQueriesAndIters()
{
	if (allEmailAlertsIter)
	{
		delete allEmailAlertsIter;
		allEmailAlertsIter = (DBQuery<DBEmailAlert>::const_iterator *)NULL;
	}

	if (allEmailAlertsQuery)
	{
		delete allEmailAlertsQuery;
		allEmailAlertsQuery = (DBQuery<DBEmailAlert> *)NULL;
	}
}
