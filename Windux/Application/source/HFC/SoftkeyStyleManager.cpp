/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyStyleManager.h"
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

int SoftkeyStyleManager::dataType = 0;
std::string SoftkeyStyleManager::dataTypeName("SoftkeyStyleManager"); 
#endif


SoftkeyStyleManager::SoftkeyStyleManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SoftkeyStyleManager::SoftkeyStyleManager(hfc::HFCManagerContainer *container)
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

SoftkeyStyleManager::~SoftkeyStyleManager()
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

	if (currentSoftkeyStyle)
		delete currentSoftkeyStyle;

	destroyQueriesAndIters();
}


void SoftkeyStyleManager::init()
{
	mySession = 0;

	currentSoftkeyStyle = 0;
	allSoftkeyStylesQuery = 0;
	allSoftkeyStylesIter  = 0;

//	restoreTrackingFlag = false;
}


DBSession *SoftkeyStyleManager::getMySession()
{
	return mySession;
}
void SoftkeyStyleManager::setMySession(DBSession *session)
{
	mySession = session;
}


void SoftkeyStyleManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();

	/* This is now only done in the SoftkeyManager
	if (mySession && mySession->isCommSession() && event == RestoreSoftkeyStyle)
		restoreTrackingFlag = true; */

	SoftkeyStyle *msgSoftkeyStyle = dynamic_cast<SoftkeyStyle *> (msg->getHfcObject());

	bool traceOn = Debug::isTracing("SoftkeyStyleManager");
	if (traceOn)
	{
		HobString myDebug = "handleMessage() - SoftkeyStyle event: ";
		myDebug.append(Integer::toString(event));
		Debug::trace("SoftkeyStyleManager", myDebug);
	}

	switch (event)
	{
		case ReadSoftkeyStyle:
			handleRead(msgSoftkeyStyle);
			break;

		case ReadAllSoftkeyStyles:
			handleReadAll();
			break;

		case WriteSoftkeyStyle:
			handleWrite(msgSoftkeyStyle);
			break;

		case DeleteSoftkeyStyle:
		case BatchApplyDeleteSoftkeyStyle:
			handleDelete(msg);
			break;

		case DeleteAllSoftkeyStyles:
			handleDeleteAll();
			break;

		case SaveSoftkeyStyle:
			handleUpdateAndSave(msgSoftkeyStyle);
			//handleSaveSoftkeyStyle(msgSoftkeyStyle);
			break;

		case RestoreSoftkeyStyle:
			handleRestore(msg);
			break;

		case CreateNewSoftkeyStyle:
			handleCreate(msgSoftkeyStyle);
			break;

		case ReadAllSoftkeyStyleNumbers:
			handleReadAllSoftkeyStyleNumbers();
			break;

		default:
			if (traceOn)
			{
				HobString myDebug = "handleMessage():  UNKNOWN SoftkeyStyle EVENT: ";
				myDebug.append(Integer::toString(event));
				Debug::trace("SoftkeyStyleManager", myDebug);
			}
			break;
	}
}


/* If the first record was not read, this method will return NULL.
 */
SoftkeyStyle *SoftkeyStyleManager::firstSoftkeyStyle()
{
	if (!dbAccess)
		return (SoftkeyStyle *)NULL;

	DBSoftKeyStylesRM *dbSoftkeyStylesRM = dbAccess->getDBSoftKeyStylesRM();
	if (!dbSoftkeyStylesRM)
		return (SoftkeyStyle *)NULL;

	// If the user calls firstSoftkeyStyle() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allSoftkeyStylesQuery = dbSoftkeyStylesRM->retrieveAll();

	// Couldn't get the query from which to access the SoftkeyStyles, just return false.
	if (!allSoftkeyStylesQuery)
		return (SoftkeyStyle *)NULL;

	DBQuery<DBSoftKeyStyle>::const_iterator tempIter = allSoftkeyStylesQuery->begin();
	if (tempIter == allSoftkeyStylesQuery->end())
		return (SoftkeyStyle *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextSoftkeyStyle()
	 * method call below).
	 */
	allSoftkeyStylesIter = new DBQuery<DBSoftKeyStyle>::const_iterator(tempIter);
	DBSoftKeyStyle *c = new DBSoftKeyStyle(*(*allSoftkeyStylesIter));
	SoftkeyStyle *newSoftkeyStyle = new SoftkeyStyle(c, dbAccess);
	return newSoftkeyStyle;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstSoftkeyStyle() method, the first call to this method will
 * result in the first record being returned.
 */
SoftkeyStyle *SoftkeyStyleManager::nextSoftkeyStyle()
{
	if (!dbAccess)
		return (SoftkeyStyle *)NULL;

	if (allSoftkeyStylesQuery && !allSoftkeyStylesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allSoftkeyStylesQuery || !allSoftkeyStylesIter)
		return firstSoftkeyStyle();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allSoftkeyStylesIter);
	if (*allSoftkeyStylesIter == allSoftkeyStylesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all SoftkeyStyles in the db. */
		destroyQueriesAndIters();
		return (SoftkeyStyle *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBSoftKeyStyle copy constructor in order for the Hfc SoftkeyStyle to
	 * control the destruction of the DBSoftKeyStyle record that the iterator contains; otherwise, the DBSoftKeyStyle will
	 * be deleted when the iterator goes out of scope.  The DBSoftKeyStyle copy will be deleted by the SoftkeyStyle record
	 * created below.
	 */
	DBSoftKeyStyle *c = new DBSoftKeyStyle(*(*allSoftkeyStylesIter));
	SoftkeyStyle *newSoftkeyStyle = new SoftkeyStyle(c, dbAccess);
	return newSoftkeyStyle;
}


/* Provide a countAll method so the caller doesn't need to know about a dbSoftkeyStylesRM.
 */
int SoftkeyStyleManager::countAllSoftkeyStyles()
{
	if (!dbAccess)
		return -1;

	DBSoftKeyStylesRM *dbSoftkeyStylesRM = dbAccess->getDBSoftKeyStylesRM();
	if (!dbSoftkeyStylesRM)
		return -1;

	return dbSoftkeyStylesRM->countAll();
}


SoftkeyStyle *SoftkeyStyleManager::getCurrentSoftkeyStyle()
{
	return currentSoftkeyStyle;
}
void SoftkeyStyleManager::setCurrentSoftkeyStyle(SoftkeyStyle *newSoftkeyStyle)
{
	if (currentSoftkeyStyle)
		delete currentSoftkeyStyle;
	currentSoftkeyStyle = newSoftkeyStyle;
}


void SoftkeyStyleManager::handleRead(SoftkeyStyle *msgSoftkeyStyle)
{
	if (!mySession || !msgSoftkeyStyle || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	SoftkeyStyle *readSoftkeyStyle = SoftkeyStyle::find(msgSoftkeyStyle->getStyleNumber(), dbAccess);
	if (!readSoftkeyStyle)		//	This softkey style does not exist
	{
		// Send error message to browser indicating SoftkeyStyle not found.
		if (s && s->isInSupervisor())
			s->addDialogMessage(ErrSoftkeyDev, SoftkeyStyleNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (readSoftkeyStyle)
			delete readSoftkeyStyle;
		return;
	}

	// We have a new current SoftkeyStyle.
	setCurrentSoftkeyStyle(readSoftkeyStyle);
#if 0
	if (Debug::isTracing("SoftkeyManager.SoftkeyStyle"))
	{
		HobString myDebug = "handleReadSoftkeyStyle(): currentSoftkeyStyle = ";
		myDebug.append(Hex::toString((long int) currentSoftkeyStyle));
		Debug::trace("SoftkeyManager.SoftkeyStyle", myDebug);
	}
#endif

	// Send the SoftkeyStyle found back to the Browser.
	// Is this the behavior for ALL SoftkeyStyleRead messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a SoftkeyStyle, but doesn't want the data sent back?
	SessionMessage softkeyData(SoftkeyDev, WriteSoftkeyStyle);
	softkeyData.setFormatType(BrowserFormat);
	softkeyData.setHfcObject(currentSoftkeyStyle);

	// Send the SoftkeyStyle to the UI (Browser)
	mySession->send(&softkeyData, Messenger::DEST_UI);
}


void SoftkeyStyleManager::handleReadAll()
{
	Debug::trace("SoftkeyStyleManager", "Unable to handle a ReadAllSoftkeyStyles event right now!");
}


void SoftkeyStyleManager::handleWrite(SoftkeyStyle *modifiedSoftkeyStyle)
{
	if ((!modifiedSoftkeyStyle) || (!currentSoftkeyStyle))
		return;

#if 0
	if (Debug::isTracing("SoftkeyStyleManager"))
	{
		HobString myDebug = "handleWriteSoftkeyStyle(): currentSoftkeyStyle = ";
		myDebug.append(Hex::toString((long int) currentSoftkeyStyle)).append("  , modifiedSoftkeyStyle = ").append(Hex::toString((long int) modifiedSoftkeyStyle));
		Debug::trace("SoftkeyStyleManager", myDebug);
	}
#endif
  	currentSoftkeyStyle->update(modifiedSoftkeyStyle);
}


void SoftkeyStyleManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyStyle *msgSoftkeyStyle = dynamic_cast<SoftkeyStyle *> (msg->getHfcObject());
	if (!msgSoftkeyStyle || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyStyles))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		SoftkeyStyle *existingSoftkeyStyle = SoftkeyStyle::find(msgSoftkeyStyle->getStyleNumber(), dbAccess);
		if (existingSoftkeyStyle)
		{
			existingSoftkeyStyle->deleteRecord();
#if 0
			if (Debug::isTracing("SoftkeyStyleManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyStyle - SoftkeyStyle deleted - styleNum: ";
				myDebug.append(Long::toString(msgSoftkeyStyle->getStyleNumber()));
				Debug::trace("SoftkeyStyleManager", myDebug);
			}
#endif
			if (msg->getEventType() != BatchApplyDeleteSoftkeyStyle)
			{
				if (mySession && !mySession->isCommSession())
				{
					//	Create a blank softkey
					SoftkeyStyle *newSoftkeyStyle = createNewSoftkeyStyle();
					// Make it the current SoftkeyStyle.
					setCurrentSoftkeyStyle(newSoftkeyStyle);
					//	Send the blank softkey to the browser
					sendCurrentSoftkeyStyle();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyStyles))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingSoftkeyStyle, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingSoftkeyStyle;
		}
		else
		{
#if 0
			if (Debug::isTracing("SoftkeyStyleManager"))
			{
				HobString myDebug = "handleDeleteSoftkeyStyle - SoftkeyStyle to be deleted was not found in the database - styleNum: ";
				myDebug.append(Long::toString(msgSoftkeyStyle->getStyleNumber()));
				Debug::trace("SoftkeyStyleManager", myDebug);
			}
#endif
		}
	}
}


void SoftkeyStyleManager::handleDeleteAll()
{
	Debug::trace("SoftkeyStyleManager", "handleDeleteAllSoftkeyStyles - deleting all SoftkeyStyles");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyStyles))
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

		DBSoftKeyStylesRM *dbSoftkeyStylesRM = dbAccess->getDBSoftKeyStylesRM();
		if (!dbSoftkeyStylesRM)
			return;

		dbSoftkeyStylesRM->deleteAll();

		destroyQueriesAndIters();

		Debug::trace("SoftkeyStyleManager", "handleDeleteAll - will create reserved records next");
		SoftkeyStyle::verifyReservedRecords(dbAccess);
	}

	Debug::trace("SoftkeyStyleManager", "handleDeleteAllSoftkeyStyles - leaving");
}


void SoftkeyStyleManager::handleUpdateAndSave(SoftkeyStyle *modifiedSoftkeyStyle)
{
	Debug::trace("SoftkeyStyleManager", HobString("handleUpdateAndSave(): currentSoftkeyStyle = ") + HobString(Hex::toString((long long) currentSoftkeyStyle)));
	if (currentSoftkeyStyle)
	{
		if (modifiedSoftkeyStyle)
		  	currentSoftkeyStyle->update(modifiedSoftkeyStyle);
		handleSaveRecord(currentSoftkeyStyle);
	}
}

void SoftkeyStyleManager::handleSaveRecord(SoftkeyStyle *recordToSave)
{
#if 0
	if (Debug::isTracing("SoftkeyStyleManager"))
	{
		HobString myDebug = "handleSaveSoftkeyStyle(): recordToSave = ";
		myDebug.append(Hex::toString((long) recordToSave));
		Debug::trace("SoftkeyStyleManager", myDebug);
	}
#endif
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyStyles))
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


void SoftkeyStyleManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	SoftkeyStyle *restoreSoftkeyStyle = dynamic_cast<SoftkeyStyle *> (msg->getHfcObject());
	if (!restoreSoftkeyStyle || !dbAccess)
		return;

	if (restoreSoftkeyStyle->hasErrors())
	{
		if (mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = restoreSoftkeyStyle->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = restoreSoftkeyStyle->nextError();
			}
		}
		return;
	}

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftKeyStyles))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreSoftkeyStyle->setDBAccess(dbAccess);

		SoftkeyStyle *usedSoftkeyStyle = (SoftkeyStyle *)NULL;
		SoftkeyStyle *existingSoftkeyStyle = SoftkeyStyle::find(restoreSoftkeyStyle->getStyleNumber(), dbAccess);
		if (existingSoftkeyStyle)
		{
			if (existingSoftkeyStyle->isRecordValid())
			{
				existingSoftkeyStyle->update(restoreSoftkeyStyle);
				existingSoftkeyStyle->saveRecord();
				usedSoftkeyStyle = existingSoftkeyStyle;
			}
			else
			{
				existingSoftkeyStyle->deleteRecord();
				restoreSoftkeyStyle->saveRecord();
				usedSoftkeyStyle = restoreSoftkeyStyle;
			}
		}
		else
		{
			restoreSoftkeyStyle->saveRecord();
			usedSoftkeyStyle = restoreSoftkeyStyle;
		}

		if (existingSoftkeyStyle)
			delete existingSoftkeyStyle;
	}
}


void SoftkeyStyleManager::handleCreate(SoftkeyStyle *msgSoftkeyStyle)
{
	if (!mySession || !msgSoftkeyStyle)
		return;

	SoftkeyStyle *newSoftkeyStyle = createNewSoftkeyStyle(msgSoftkeyStyle->getStyleNumber());
	if (newSoftkeyStyle)
	{
		//	Set this new style as the current style.
		setCurrentSoftkeyStyle(newSoftkeyStyle);

		// Send the new style to the Browser.
		SessionMessage softkeyData(SoftkeyDev, WriteSoftkeyStyle);
		softkeyData.setFormatType(BrowserFormat);
		softkeyData.setHfcObject(currentSoftkeyStyle);
		mySession->send(&softkeyData, Messenger::DEST_UI);
	}
}


void SoftkeyStyleManager::handleReadAllSoftkeyStyleNumbers()
{
	if (!mySession)
		return;

	HobList numberList;
	SoftkeyStyle *myStyle = firstSoftkeyStyle();
	while (myStyle)
	{
		numberList.push_back(Long::toString(myStyle->getStyleNumber()));
		myStyle = nextSoftkeyStyle();
	}

	// Send the SoftkeyGroup found back to the Browser.
	// Is this the behavior for ALL SoftkeyGroup Read messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a SoftkeyGroup, but doesn't want the data sent back?
	Debug::trace("SoftkeyStyleManager", "Sending WriteAllSoftkeyStyleNumbers msg");
	SessionMessage styleNumbers(SoftkeyDev, WriteAllSoftkeyStyleNumbers);
	styleNumbers.setFormatType(BrowserFormat);
	styleNumbers.setHfcObject(&numberList);

	// Send the SoftkeyFeature to the UI (Browser)
	mySession->send(&styleNumbers, Messenger::DEST_UI);
}


SoftkeyStyle *SoftkeyStyleManager::createNewSoftkeyStyle(HobRecordNum recordNum)
{
	SoftkeyStyle *newSoftkeyStyle = (SoftkeyStyle *) NULL;
	if (dbAccess)
	{
		// Send the current new SoftkeyStyle to the browser.
		newSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (newSoftkeyStyle)
			newSoftkeyStyle->setStyleNumber(recordNum);
	}
	return newSoftkeyStyle;
}

void SoftkeyStyleManager::sendCurrentSoftkeyStyle()
{
	if (!currentSoftkeyStyle || !mySession)
	{
		Debug::trace("SoftkeyStyleManager", "sendCurrentSoftkeyStyle(): currentSoftkeyStyle or mySession is NULL!");
		return;
	}
	SessionMessage softkeyMsg(SoftkeyDev, WriteSoftkeyStyle);
	softkeyMsg.setFormatType(BrowserFormat);
	softkeyMsg.setHfcObject(currentSoftkeyStyle);
	mySession->send(&softkeyMsg, Messenger::DEST_UI);
}


void SoftkeyStyleManager::destroyQueriesAndIters()
{
	Debug::trace("SoftkeyStyleManager", "destroySoftkeyStyleQueriesAndIters()");
	if (allSoftkeyStylesIter)
	{
		delete allSoftkeyStylesIter;
		allSoftkeyStylesIter = 0;
	}

	if (allSoftkeyStylesQuery)
	{
		delete allSoftkeyStylesQuery;
		allSoftkeyStylesQuery = 0;
	}
}
