/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "COOLManager.h"
#include "SessionMessage.h"
#include "Session.h"
//#include "ResponseMessageManager.h"
#include "HfcError.h"

#include "DBAccess.h"

#include "COOLDevEvent.h"

#include "AllCoolCategories.h"
#include "AllCoolCountries.h"
#include "AllCoolProductionSteps.h"
#include "AllCoolPretexts.h"
#include "AllCoolTexts.h"
#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"

#include "DBQuery.h"
#include "DBCoolCategory.h"
#include "DBCoolCountry.h"
#include "DBCoolProductionStep.h"
#include "DBCoolPreText.h"
#include "DBCoolCategoryCountry.h"
#include "DBCoolCategoryProductionStep.h"
#include "DBCoolCategoryPreText.h"
#include "DBCoolText.h"
#include "DBCoolList.h"
#include "DBCoolListText.h"

#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"

#include "DBCoolListsRM.h"
#include "DBProductsRM.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"
#include "Configuration.h"
#include "COOLTextManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLTextMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLTextMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevReadCoolText:
		{
			COOLText *msgCoolText = dynamic_cast<COOLText *> (msg->getHfcObject());
			handleReadCOOLText(msgCoolText);
			break;
		}
		case COOLDevReadAllCoolText:
			handleReadAllCOOLTexts();
			break;
		case COOLDevWriteCoolText:
		{
			COOLText *msgCoolText = dynamic_cast<COOLText *> (msg->getHfcObject());
			handleWriteCOOLText(msgCoolText);
			break;
		}
		case COOLDevDeleteCoolText:
		case COOLDevPretextBatchApplyDelete:
			handleDeleteCOOLText(msg);
			break;
		case COOLDevDeleteAllCoolText:
		case COOLDevDeleteAllCountryTextForUltima:		//	Eventually change to COOLDevDeleteAllCoolTextForUltima
			handleDeleteAllCOOLTexts();
			break;
		case COOLDevSaveCoolText:
		{
			COOLText *msgCoolText = dynamic_cast<COOLText *> (msg->getHfcObject());
			handleSaveCOOLText(msgCoolText);
			break;
		}
		case COOLDevRestoreCoolText:
		{
			COOLText *msgCoolText = dynamic_cast<COOLText *> (msg->getHfcObject());
			if (msgCoolText)
				handleRestoreCOOLText(msg);
			else
			{
				COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
				if (msgPretext)
					handleRestoreCOOLPretext(msg);
			}
			break;
		}
		case COOLDevCreateNewCoolText:
			handleCreateCOOLText();
			break;

		default:
			if (Debug::isTracing("COOLManager"))
			{
				HobString myDebug("UNKNOWN EVENT: ");
				myDebug.append(Integer::toString(event)).append(" !!!");
				Debug::trace("COOLManager", myDebug);
			}
			break;
	}
}

COOLText *COOLManager::getCurrentCoolText()
{
	return currentCoolText;
}
void COOLManager::setCurrentCoolText(COOLText *text)
{
	if (currentCoolText)
		delete currentCoolText;
	currentCoolText = text;
}


COOLText *COOLManager::firstCOOLText()
{
	if (!dbAccess)
		return (COOLText *)NULL;

	DBCoolTextsRM *dbCoolTextsRM = dbAccess->getDBCoolTextsRM();
	if (!dbCoolTextsRM)
		return (COOLText *)NULL;

	// If the user calls firstCOOLText() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLTextsQueryAndIter();

	allCoolTextsQuery = dbCoolTextsRM->retrieveAll();

	// Couldn't get the query from which to access the Texts, just return false.
	if (!allCoolTextsQuery)
		return (COOLText *)NULL;

	DBQuery<DBCoolText>::const_iterator tempIter = allCoolTextsQuery->begin();
	if (tempIter == allCoolTextsQuery->end())
		return (COOLText *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLText()
	 * method call below).
	 */
	allCoolTextsIter = new DBQuery<DBCoolText>::const_iterator(tempIter);
	DBCoolText *c = new DBCoolText(*(*allCoolTextsIter));
	COOLText *newText = new COOLText(c, dbAccess);
	return newText;
}

COOLText *COOLManager::nextCOOLText()
{
	if (!dbAccess)
		return (COOLText *)NULL;

	if (allCoolTextsQuery && !allCoolTextsQuery->isEnabled())
		destroyCOOLTextsQueryAndIter();

	if (!allCoolTextsQuery || !allCoolTextsIter)
		return firstCOOLText();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolTextsIter);
	if (*allCoolTextsIter == allCoolTextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all COOLTexts in the db. */
		destroyCOOLTextsQueryAndIter();
		return (COOLText *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolText copy constructor in order for the Hfc COOLText to
	 * control the destruction of the DBCoolText record that the iterator contains; otherwise, the DBCoolText will
	 * be deleted when the iterator goes out of scope.  The DBCoolText copy will be deleted by the COOLText record
	 * created below.
	 */
	DBCoolText *c = new DBCoolText(*(*allCoolTextsIter));
	COOLText *newText = new COOLText(c, dbAccess);
	return newText;
}


int COOLManager::countAllCOOLTexts()
{
	if (!dbAccess)
		return -1;

	DBCoolTextsRM *dbCoolTextsRM = dbAccess->getDBCoolTextsRM();
	if (!dbCoolTextsRM)
		return -1;

	return dbCoolTextsRM->countAll();
}


void COOLManager::handleReadCOOLText(COOLText *msgCoolText)
{
	if (!msgCoolText || !dbAccess)
		return;

	if (msgCoolText->getRecordNum() == (HobRecordNum)RECORD_NOT_FOUND)
	{
		setCurrentCoolText((COOLText *) NULL);
		return;
	}
	COOLText *readCoolText = COOLText::findRecord(msgCoolText->getRecordNum(), msgCoolText->getDeptNum(), dbAccess);
	if (!readCoolText)
	{
		//  Typically, we would send an error message indicating
		COOLText *readCoolText = new COOLText();
		if (readCoolText)
		{
			readCoolText->setDeptNum(msgCoolText->getDeptNum());
			readCoolText->setRecordNum(0);
			readCoolText->setErrorCode(RECORD_OK);
			readCoolText->setDBAccess(dbAccess);
		}
	}
	setCurrentCoolText(readCoolText);

	if (mySession)
	{
		// Send the COOL Text found back to the Browser.
		SessionMessage coolData(COOLDev, COOLDevWriteCoolText);
		coolData.setFormatType(BrowserFormat);
		coolData.setHfcObject(currentCoolText);
		mySession->send(&coolData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLTexts()
{
	if (!mySession || !dbAccess)
		return;

	AllCoolTexts myList;
	myList.findAll(dbAccess);

	// Send the COOL Texts found back to the Browser.
	// Is this the behavior for ALL COOLCoolTextRead messages the Session receives, to send the data back to the Browser?
	// Is there any situation where something else reads a COOLText, but doesn't want the data sent back?
	SessionMessage ctData(COOLDev, COOLDevWriteAllCoolText);
	ctData.setFormatType(BrowserFormat);
	ctData.setHfcObject(&myList);
	mySession->send(&ctData, Messenger::DEST_UI);
}

void COOLManager::handleWriteCOOLText(COOLText *msgCoolText)
{
	if ((!msgCoolText) || (!currentCoolText))
		return;

	if (Debug::isTracing("COOLManager.COOLText"))
	{
		HobString myDebug("handleCOOLTextWrite(): currentCoolText = ");
		myDebug.append(Hex::toString((long long) currentCoolText)).append("  , msgCoolText = ").append(Hex::toString((long long) msgCoolText));
		Debug::trace("COOLManager.COOLText", myDebug);
	}
	currentCoolText->updateRecord(msgCoolText);
}

void COOLManager::handleDeleteCOOLText(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	COOLText *msgCoolText = dynamic_cast<COOLText *> (msg->getHfcObject());
	if (!msgCoolText)
	{
		if (currentCoolText)
			usingCurrent = true;
		else
			return;
	}

	DataManager *myManager = mgrContainer_->getCOOLTextManager();
	if (!myManager)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolTexts))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentCoolText, myManager->getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("COOLManager.COOLText");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "handleCOOLTextDelete(): CoolText # ";
			myDebug.append(Long::toString(msgCoolText->getRecordNum())).append(" : ").append(msgCoolText->getText());
			Debug::trace("COOLManager.COOLText", myDebug);
		}
		COOLText *existingCoolText = (usingCurrent ? currentCoolText : COOLText::findRecord(msgCoolText->getRecordNum(), msgCoolText->getDeptNum(), dbAccess));
		if (existingCoolText && existingCoolText->getRecordNum() != RECORD_NOT_FOUND)
		{
			existingCoolText->deleteRecord();
			if (traceOn)
			{
				myDebug.append(", has been deleted.");
				Debug::trace("COOLManager.COOLText", myDebug);
			}

			if (msg->getEventType() != COOLDevCoolTextBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingCoolText, myManager->getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
				}
			}
		}
		else
		{
			if (traceOn)
			{
				myDebug.append(", not found in DB so it could not be deleted.");
				Debug::trace("COOLManager.COOLText", myDebug);
			}
		}

		if (usingCurrent)
			setCurrentCoolText((COOLText *) NULL);
		else
			delete existingCoolText;
	}
}

void COOLManager::handleDeleteAllCOOLTexts(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLTexts - deleting all COOL Texts");

	DataManager *myManager = mgrContainer_->getCOOLTextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolTexts))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(myManager->getDevice(), myManager->getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyCOOLTextsQueryAndIter();
	}
}

void COOLManager::handleSaveCOOLText(COOLText *msgCoolText)
{
	if (Debug::isTracing("COOLManager.COOLText"))
	{
		HobString myDebug("handleCOOLTextSave(): currentCoolText = ");
		myDebug.append(Hex::toString((long long) currentCoolText));
		Debug::trace("COOLManager.COOLText", myDebug);
	}
	if (currentCoolText)
	{
		if (msgCoolText)
			currentCoolText->updateRecord(msgCoolText);
		currentCoolText->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLTextManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolTexts))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, msgCoolText, myManager->getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), synchSer)));
		}
	}
}

void COOLManager::handleRestoreCOOLText(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLText *restoreText = dynamic_cast<COOLText *> (msg->getHfcObject());
	if (!restoreText || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLTextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolTexts))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreText, myManager->getWriteEvent());

			receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), reserialization);
		}
		else
			receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreText->setDBAccess(dbAccess);

		COOLText *usedText = 0;
		COOLText *existingText = COOLText::findRecord(restoreText->getRecordNum(), restoreText->getDeptNum(), dbAccess);
		if (existingText)
		{
			if (existingText->isRecordValid())
			{
				existingText->updateRecord(restoreText);
				existingText->saveRecord();
				usedText = existingText;								
			}
			else
			{
				existingText->deleteRecord();	   
				restoreText->saveRecord();
				usedText = restoreText;								
			}
		}
		else
		{
			restoreText->saveRecord();
			usedText = restoreText;								
		}

		if (usedText && usedText->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedText->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedText->nextError();
			}
		}
		if (existingText)
			delete existingText;
	}
}

void COOLManager::handleCreateCOOLText()
{
	if (!dbAccess)
		return;

	HobRecordNum textNum = findNewCoolTextNumber();
	if ((textNum > 0) && (textNum <= 999999))
	{
		if (Debug::isTracing("COOLManager.COOLText"))
		{
			HobString myDebug("handleCOOLTextCreate(): New COOL text record number = ");
			myDebug.append(Long::toString(textNum));
			Debug::trace("COOLManager.COOLText", myDebug);
		}
		//  Create a new COOL text from the database.
		COOLText *newCoolText = new COOLText();
		if (newCoolText)
		{
			Debug::trace("COOLManager.COOLText", "handleCOOLTextCreate(): Setting up the new record.");
			newCoolText->setDeptNum( Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
			newCoolText->setRecordNum(textNum);
			newCoolText->setErrorCode(RECORD_OK);
			newCoolText->setDBAccess(dbAccess);

			Debug::trace("COOLManager.COOLText", "handleCOOLTextCreate(): Setting the current record.");
			//  Set this new category as the current category.
			setCurrentCoolText(newCoolText);

			if (mySession)
			{
				Debug::trace("COOLManager.COOLText", "handleCOOLTextCreate(): Sending the record to the Client.");
				// Send the new product to the Browser.
				SessionMessage catData(COOLDev, COOLDevCreateNewCoolText);
				catData.setFormatType(BrowserFormat);
				catData.setHfcObject(currentCoolText);
				mySession->send(&catData, Messenger::DEST_UI);
			}
		}
	}
}

HobRecordNum COOLManager::findNewCoolTextNumber()
{
	HobRecordNum newNumber = 0;

	if (!dbAccess)
		return newNumber;

	DBCoolTextsRM *dbCoolTextsRM = dbAccess->getDBCoolTextsRM();
	if (!dbCoolTextsRM)
		return newNumber;

	bool traceOn = Debug::isTracing("COOLManager");
	HobString myDebug("findNewCoolTextNumber():  ");
	try
	{
		newNumber = 1;
		DBQuery<DBCoolText>* allCoolTextsQuery = dbCoolTextsRM->retrieveAllForDept( Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
		if (!allCoolTextsQuery)
		{
			Debug::trace("COOLManager", "No COOL text at all. Start number at 1.");
		}
		else
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("  Count = ").append(Integer::toString((int)allCoolTextsQuery->size()));
				Debug::trace("COOLManager", myDebug2);
			}
			DBQuery<DBCoolText>::const_iterator tempIter = allCoolTextsQuery->end();
			--tempIter;
			Debug::trace("COOLManager", "Decremented tempIter");
			if (tempIter != allCoolTextsQuery->begin())
			{
				HobString theNumberStr;
				newNumber = Long::valueOf(tempIter->getNumber()) + 1;
				Debug::trace("COOLManager", "Got a number from the iter.");
				if (newNumber > 999999)
					newNumber = 1;
			}
			delete allCoolTextsQuery;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLManager::findNewCoolTextNumber() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("  Validating the new number: ").append(Long::toString(newNumber));
		Debug::trace("COOLManager", myDebug2);
	}
	COOLText *myCoolText = (COOLText *) NULL;
	do
	{
		if (myCoolText)
			delete myCoolText;

		COOLText *myCoolText = COOLText::findRecord(newNumber, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("  Number = ").append(Long::toString(newNumber));
			myDebug2.append("  myCoolText = ").append(Hex::toString((long long) myCoolText));
			Debug::trace("COOLManager", myDebug2);
		}
	} while(myCoolText && (++newNumber <= 999999));

	if ((newNumber < 0) || (newNumber > 999999))
		newNumber = 0;
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("  The new number is ").append(Long::toString(newNumber));
		Debug::trace("COOLManager", myDebug2);
	}
	return newNumber;
}
