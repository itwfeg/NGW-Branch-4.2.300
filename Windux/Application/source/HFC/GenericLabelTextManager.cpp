/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "GenericLabelTextManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"
#include "AllGenericLabelTexts.h"
#include "HfcError.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBLabelTextsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericLabelTextManager::dataType = 0;
std::string GenericLabelTextManager::dataTypeName("GenericLabelTextManager"); 
#endif

GenericLabelTextManager::GenericLabelTextManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

GenericLabelTextManager::GenericLabelTextManager(hfc::HFCManagerContainer *container)
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

GenericLabelTextManager::~GenericLabelTextManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentGenericLabelText)
		delete currentGenericLabelText;

	destroyQueriesAndIters();
}


void GenericLabelTextManager::init()
{
	mySession = 0;

	currentGenericLabelText = 0;
	maxLabelTexts = Configuration::getInstance()->getMaxLabelTexts();
//	allLabelTexts.insert(allLabelTexts.begin(), maxLabelTexts, (GenericLabelText *) 0);
	
	allGenericLabelTextsQuery = 0;
	allGenericLabelTextsIter  = 0;
}


DBSession *GenericLabelTextManager::getMySession()
{
	return mySession;
}
void GenericLabelTextManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
GenericLabelText *GenericLabelTextManager::firstGenericLabelText()
{
	if (!dbAccess)
		return (GenericLabelText *)NULL;

	DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
	if (!dbLabelTextsRM)
		return (GenericLabelText *)NULL;

	// If the user calls firstGenericLabelText() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allGenericLabelTextsQuery = dbLabelTextsRM->retrieveAll();

	// Couldn't get the query from which to access the GenericLabelTexts, just return false.
	if (!allGenericLabelTextsQuery)
		return (GenericLabelText *)NULL;

	DBQuery<DBLabelText>::const_iterator tempIter = allGenericLabelTextsQuery->begin();
	if (tempIter == allGenericLabelTextsQuery->end())
		return (GenericLabelText *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextGenericLabelText()
	 * method call below).
	 */
	allGenericLabelTextsIter = new DBQuery<DBLabelText>::const_iterator(tempIter);
	DBLabelText *text = new DBLabelText(*(*allGenericLabelTextsIter));
	GenericLabelText *genericText= new GenericLabelText(text, dbAccess);
	return genericText;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstGenericLabelText() method, the first call to this method will
 * result in the first record being returned.
 */
GenericLabelText *GenericLabelTextManager::nextGenericLabelText()
{
	if (!dbAccess)
		return (GenericLabelText *)NULL;

	if (allGenericLabelTextsQuery && !allGenericLabelTextsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allGenericLabelTextsQuery || !allGenericLabelTextsIter)
		return firstGenericLabelText();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allGenericLabelTextsIter);
	if (*allGenericLabelTextsIter == allGenericLabelTextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all GenericLabelTexts in the db. */
		destroyQueriesAndIters();
		return (GenericLabelText *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBLabelText copy constructor in order for the Hfc GenericLabelText to
	 * control the destruction of the DBLabelText record that the iterator contains; otherwise, the DBLabelText will
	 * be deleted when the iterator goes out of scope.  The DBLabelText copy will be deleted by the GenericLabelText record
	 * created below.
	 */
	DBLabelText *text = new DBLabelText(*(*allGenericLabelTextsIter));
	GenericLabelText *genericText = new GenericLabelText(text, dbAccess);
	return genericText;
}


/*
void GenericLabelTextManager::loadAllGenericLabelTexts()
{
	for (int i = 0, textNum = 1; i < maxLabelTexts; i++, textNum++)
	{
		if (allLabelTexts[i])
			delete allLabelTexts[i];
		allLabelTexts[i] = GenericLabelText::findLabelText(textNum, database, myDbRM);
	}
}
*/


/* Provide a countAll method so the caller doesn't need to know about a myDbRM.
 */
int GenericLabelTextManager::countAllGenericLabelTexts()
{
	if (!dbAccess)
		return -1;

	DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
	if (!dbLabelTextsRM)
		return -1;

	return dbLabelTextsRM->countAll();
}


GenericLabelText *GenericLabelTextManager::getCurrentGenericLabelText()
{
	return currentGenericLabelText;
}
void GenericLabelTextManager::setCurrentGenericLabelText(GenericLabelText *newGenericLabelText)
{
	if (currentGenericLabelText)
		delete currentGenericLabelText; 
	currentGenericLabelText = newGenericLabelText;
}


void GenericLabelTextManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	GenericLabelText *msgGenericLabelText = dynamic_cast<GenericLabelText *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("GenericLabelTextManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			//	Read the record, send it as GenericLabelTextWrite to the browser.
			handleRead(msgGenericLabelText);
			break;

		case DtReadAll:
		case AppletViewerReadAll:
			//	Read the record, send it as GenericLabelTextWrite to the browser.
			handleReadAll(msg->getEventType());
			break;

		case DtWrite:
			handleWrite(msgGenericLabelText);
			break;

		case DtDelete:
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgGenericLabelText);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgGenericLabelText);
			break;

		default:
			Debug::trace("GenericLabelTextManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void GenericLabelTextManager::handleRead(GenericLabelText *msgGenericText)
{
	if (!msgGenericText || !dbAccess)
		return;

	HobRecordNum textNum = msgGenericText->getTextNum();
	if (textNum == (HobRecordNum)RECORD_NOT_FOUND)
	{
		setCurrentGenericLabelText((GenericLabelText *) 0);
		return;
	}

	GenericLabelText *genericText = GenericLabelText::findLabelText(textNum, dbAccess);

	if (!genericText || genericText->getErrorCode() != RECORD_OK)
	{
		if (mySession)
		{
/*  Don't send an error, instead, send "" if not found.
			// Send error message to browser indicating product_not_found.
			SessionMessage errMsg(Error, ErrGenericTextDev);
			errMsg.setFormatType(BrowserFormat);

			HfcError hfcError = HfcError(GenericTextErr, HfcResponse::RESPONSESTYLE_NODIALOG,
											msgGenericText->getTextNum(),
											(genericText ? genericText->getErrorCode() : RECORD_NOT_FOUND));
			errMsg.setHfcObject(&hfcError);

			const char *msgdata;
			msgdata = hfcError.serialize(BrowserFormat);
			int msglen;
			msglen = hfcError.getSerializedLength();

			// Send the error message to the UI (Browser)
			mySession->send(&errMsg, Messenger::DEST_UI);
*/

			GenericLabelText glt;
			glt.setTextNum(textNum);
			glt.setText("");
			SessionMessage genLabelText(GenericTextDev, DtWrite);
			genLabelText.setFormatType(BrowserFormat);
			genLabelText.setHfcObject(&glt);

			// Send the product to the UI (Browser)
			mySession->send(&genLabelText, Messenger::DEST_UI);
		}

		if (genericText)
			delete genericText;
		return;
	}

	setCurrentGenericLabelText(genericText);

	if (mySession)
	{
		SessionMessage genLabelText(GenericTextDev, DtWrite);
		genLabelText.setFormatType(BrowserFormat);
		genLabelText.setHfcObject(genericText);

		// Send the product to the UI (Browser)
		mySession->send(&genLabelText, Messenger::DEST_UI);
	}
}


void GenericLabelTextManager::handleReadAll(UINT8 event)
{
	Debug::trace("GenericLabelTextManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	AllGenericLabelTexts allGenericTexts;
	allGenericTexts.findAll(dbAccess);

	SessionMessage allGenLabelTexts(GenericTextDev, ((event == (UINT8)AppletViewerReadAll) ? (UINT8)AppletViewerWriteAll : (UINT8)DtWriteAll));
	allGenLabelTexts.setFormatType(BrowserFormat);
	allGenLabelTexts.setHfcObject(&allGenericTexts);

	// Send the product to the UI (Browser)
	mySession->send(&allGenLabelTexts, Messenger::DEST_UI);
}


void GenericLabelTextManager::handleWrite(GenericLabelText *modifiedText)
{
	if ((!modifiedText) || (!currentGenericLabelText))
		return;

	if (Debug::isTracing("GenericLabelTextManager"))
		Debug::trace("GenericLabelTextManager", HobString("handleWrite(): recordToWrite = ") + HobString(Hex::toString((long int) modifiedText->getTextNum())));

	currentGenericLabelText->updateText(modifiedText);
}


void GenericLabelTextManager::handleDeleteAll()
{
	Debug::trace("GenericLabelTextManager", "handleDeleteAll - deleting all Generic Label Texts");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::LabelTexts))
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

		DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
		if (!dbLabelTextsRM)
			return;

		int result = dbLabelTextsRM->deleteAll();

		destroyQueriesAndIters();

		if (Debug::isTracing("GenericLabelTextManager"))
		{
			Debug::trace("GenericLabelTextManager", HobString("Deleted ") + HobString(Integer::toString(result)) + HobString(" GenericLabelText records."));
			Debug::trace("GenericLabelTextManager", "handleDeleteAll - About to create Default Generic Text.");
		}
		GenericLabelText::verifyDefaultRecords(dbAccess);
	}

	Debug::trace("GenericLabelTextManager", "handleDeleteAll - leaving");
}


void GenericLabelTextManager::handleUpdateAndSave(GenericLabelText *modifiedText)
{
	Debug::trace("GenericLabelTextManager", "handleUpdateAndSave(): currentGenericLabelText = " + ptrtostring(currentGenericLabelText));

	if (currentGenericLabelText)
	{
		if (modifiedText)
		  	currentGenericLabelText->updateText(modifiedText);
		handleSaveRecord(currentGenericLabelText);
	}

}

void GenericLabelTextManager::handleSaveRecord(GenericLabelText *recordToSave)
{
	Debug::trace("GenericLabelTextManager", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::LabelTexts))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}

		Configuration::getInstance()->setLabelText(recordToSave->getTextNum(), GenericLabelText::findLabelText(recordToSave->getTextNum(), dbAccess));
	}
}


void GenericLabelTextManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;


	GenericLabelText *restoreText = dynamic_cast<GenericLabelText *> (msg->getHfcObject());
	if (!restoreText || !dbAccess)
		return;

	if (Debug::isTracing("GenericLabelTextManager"))
		Debug::trace("GenericLabelTextManager", HobString("handleRestore(): recordToRestore = ") + HobString(Hex::toString((long int) restoreText->getTextNum())));

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::LabelTexts))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreText);

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
		restoreText->setDBAccess(dbAccess);

		GenericLabelText *existingText = GenericLabelText::findLabelText(restoreText->getTextNum(), dbAccess);
		if (existingText)
		{
			if (existingText->isRecordValid())
			{
				existingText->updateText(restoreText);
				existingText->saveRecord();								
			}
			else
			{
				existingText->deleteRecord();	   
				restoreText->saveRecord();
			}

			delete existingText;
		}
		else
			restoreText->saveRecord();

		GenericLabelText *t( GenericLabelText::findLabelText(restoreText->getTextNum(), dbAccess) );

		if ( t )
		{
			Configuration::getInstance()->setLabelText( restoreText->getTextNum(), t );

			// SOF-2955: do not delete pointer, managed by configuration
			//delete t;
		}
	}
}


void GenericLabelTextManager::handleCreate(GenericLabelText *modifiedText)
{
	if (!modifiedText || !dbAccess)
		return;

	GenericLabelText *genericText = new GenericLabelText();
	if (genericText)
	{
		genericText->setTextNum(modifiedText->getTextNum());
		genericText->setDBAccess(dbAccess);
		setCurrentGenericLabelText(genericText);

		if (mySession)
		{

			SessionMessage genLabelText(GenericTextDev, DtWrite);
			genLabelText.setFormatType(BrowserFormat);
			genLabelText.setHfcObject(genericText);
			mySession->send(&genLabelText, Messenger::DEST_UI);
		}
	}
}


void GenericLabelTextManager::destroyQueriesAndIters()
{
	if (allGenericLabelTextsIter)
	{
		delete allGenericLabelTextsIter;
		allGenericLabelTextsIter = (DBQuery<DBLabelText>::const_iterator *)NULL;
	}

	if (allGenericLabelTextsQuery)
	{
		delete allGenericLabelTextsQuery;
		allGenericLabelTextsQuery = (DBQuery<DBLabelText> *)NULL;
	}
}
