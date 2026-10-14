/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "COOLManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"

#include "DBAccess.h"

#include "COOLDevEvent.h"

#include "AllCoolCategories.h"
#include "AllCoolCountries.h"
#include "AllCoolProductionSteps.h"
#include "AllCoolPretexts.h"
#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLCategoryPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLCategoryCountry.h"
#include "COOLCategoryPretext.h"
#include "COOLCategoryProdStep.h"

#include "DBQuery.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"
#include "Configuration.h"
#include "COOLPretextManager.h"
#include "COOLCategoryPretextManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

void COOLManager::handleCOOLPretextMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLPretextMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
/*      **********  Pretexts  **********        */

		case COOLDevReadPretext:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleReadCOOLPretext(msgPretext);
			break;
		}
		case COOLDevReadAllPretexts:
		{
			AllCoolPretexts *msgAll = dynamic_cast<AllCoolPretexts *> (msg->getHfcObject());
			handleReadAllCOOLPretexts(msgAll);
			break;
		}
		case COOLDevWritePretext:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleWriteCOOLPretext(msgPretext);
			break;
		}
		case COOLDevDeletePretext:
		case COOLDevPretextBatchApplyDelete:
			handleDeleteCOOLPretext(msg);
			break;
		case COOLDevDeleteAllPretexts:
		case COOLDevDeleteAllCountryTextForUltima:
			handleDeleteAllCOOLPretexts();
			break;
		case COOLDevSavePretext:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleUpdateAndSaveCOOLPretext(msgPretext);
			break;
		}
		case COOLDevRestorePretext:
			handleRestoreCOOLPretext(msg);
			break;
		case COOLDevCreateNewPretext:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleCreateCOOLPretext(msgPretext);
			break;
		}
		case COOLDevReadByCategoryOnlyPretexts:
		{
			AllCoolPretexts *msgAll = dynamic_cast<AllCoolPretexts *> (msg->getHfcObject());
			handleReadCOOLPretextsByOneCategory(msgAll);
			break;
		}
		case COOLDevAddPretextToCategory:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleAddCOOLPretextToCategory(msgPretext);
			break;
		}
		case COOLDevDeletePretextFromCategory:
		{
			COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
			handleDeleteCOOLPretextFromCategory(msgPretext);
			break;
		}
		case COOLDevReadByAllAndByCategoryPretexts:
		{
			AllCoolPretexts *msgAll = dynamic_cast<AllCoolPretexts *> (msg->getHfcObject());
			handleReadCOOLPretextsByOneCategoryAndAllCategories(msgAll);
			break;
		}

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

/**************************************************************************************************
 **************************************************************************************************
 ***                                                                                            ***
 ***                         COOL Pretext                                                       ***
 ***                                                                                            ***
 **************************************************************************************************
 **************************************************************************************************/

COOLPretext *COOLManager::getCurrentPretext()
{
	return currentPretext;
}
void COOLManager::setCurrentPretext(COOLPretext *country)
{
	if (currentPretext)
		delete currentPretext;
	currentPretext = country;
}


COOLPretext *COOLManager::firstCOOLPretext()
{
	if (!dbAccess)
		return (COOLPretext *)NULL;

	DBCoolPreTextsRM *dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPretextsRM)
		return (COOLPretext *)NULL;

	// If the user calls firstCOOLPretext() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLPretextsQueryAndIter();

	// Only used by backuprestore, and backuprestore needs all from all depts, so we dont pass deptNum
	allCoolPretextsQuery = dbCoolPretextsRM->retrieveAll();

	// Couldn't get the query from which to access the Pretexts, just return false.
	if (!allCoolPretextsQuery)
		return (COOLPretext *)NULL;

	DBQuery<DBCoolPreText>::const_iterator tempIter = allCoolPretextsQuery->begin();
	if (tempIter == allCoolPretextsQuery->end())
		return (COOLPretext *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLPretext()
	 * method call below).
	 */
	allCoolPretextsIter = new DBQuery<DBCoolPreText>::const_iterator(tempIter);
	DBCoolPreText *c = new DBCoolPreText(*(*allCoolPretextsIter));
	COOLPretext *newPretext = new COOLPretext(c, dbAccess);
	return newPretext;
}

COOLPretext *COOLManager::nextCOOLPretext()
{
	if (!dbAccess)
		return (COOLPretext *)NULL;

	if (allCoolPretextsQuery && !allCoolPretextsQuery->isEnabled())
		destroyCOOLPretextsQueryAndIter();

	if (!allCoolPretextsQuery || !allCoolPretextsIter)
		return firstCOOLPretext();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolPretextsIter);
	if (*allCoolPretextsIter == allCoolPretextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all COOLPretexts in the db. */
		destroyCOOLPretextsQueryAndIter();
		return (COOLPretext *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolPreText copy constructor in order for the Hfc COOLPretext to
	 * control the destruction of the DBCoolPreText record that the iterator contains; otherwise, the DBCoolPreText will
	 * be deleted when the iterator goes out of scope.  The DBCoolPreText copy will be deleted by the COOLPretext record
	 * created below.
	 */
	DBCoolPreText *c = new DBCoolPreText(*(*allCoolPretextsIter));
	COOLPretext *newPretext = new COOLPretext(c, dbAccess);
	return newPretext;
}


int COOLManager::countAllCOOLPretexts()
{
	if (!dbAccess)
		return -1;

	DBCoolPreTextsRM *dbCoolPreTextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPreTextsRM)
		return -1;

	return dbCoolPreTextsRM->countAll();
}


void COOLManager::handleReadCOOLPretext(COOLPretext *msgPretext)
{
	if (!msgPretext || !dbAccess)
		return;

	if (msgPretext->getRecordNum() == (HobRecordNum)RECORD_NOT_FOUND)
	{
		setCurrentPretext((COOLPretext *) NULL);
		return;
	}

	COOLPretext *readPretext = COOLPretext::findCOOLPretext(msgPretext->getRecordNum(), msgPretext->getDeptNum(), dbAccess);
	if (!readPretext)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
		{
			if (s->getCurrentMode() == OpModeSupervisor)
			{
				// Send error message to browser indicating Cool category not found.
				mySession->addDialogMessage(ErrCOOLDev, COOLDevErrPretextNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

				if (readPretext)
					delete readPretext;
				return;
			}
		}
	}
	setCurrentPretext(readPretext);

	if (mySession)
	{
		// Send the COOL Pretext found back to the Browser.
		SessionMessage coolData(COOLDev, COOLDevWritePretext);
		coolData.setFormatType(BrowserFormat);
		coolData.setHfcObject(currentPretext);
		mySession->send(&coolData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLPretexts(AllCoolPretexts * /* msgAll */)
{
#if 1
	if (!dbAccess)
		return;
#else
	if (!dbAccess || !msgAll)
		return;

	if (msgAll->doExcludeAllCategories())
		handleReadCOOLPretextsByNotAllCategories(msgAll);
	else
	{
#endif
		AllCoolPretexts *myList = new AllCoolPretexts();
		if (!myList)
			return;

		myList->findAllPretexts(dbAccess);

		if (Debug::isTracing("COOLManager.COOLPretext"))
		{
			HobString myDebug("handling event: ");
			myDebug.append("handleCOOLPretextReadAll(): myList size = ").append(Integer::toString((int)myList->getRecordNumList()->size()));
			Debug::trace("COOLManager.COOLPretext", myDebug);
		}

		if (mySession)
		{
			// Send the class found back to the Browser.
			// Is this the behavior for ALL COOLPretextRead messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a COOLPretext, but doesn't want the data sent back?
			SessionMessage ctData(COOLDev, COOLDevWriteAllPretexts);
			ctData.setFormatType(BrowserFormat);
			ctData.setHfcObject(myList);
			mySession->send(&ctData, Messenger::DEST_UI);
		}
		delete myList;
#if 0
	}
#endif
}

void COOLManager::handleWriteCOOLPretext(COOLPretext *msgPretext)
{
	if ((!msgPretext) || (!currentPretext))
		return;

	if (Debug::isTracing("COOLManager.COOLPretext"))
	{
		HobString myDebug("handleCOOLPretextWrite(): currentPretext = ");
		myDebug.append(Hex::toString((long long) currentPretext)).append("  , msgPretext = ").append(Hex::toString((long long) msgPretext));
		Debug::trace("COOLManager.COOLPretext", myDebug);
	}
	currentPretext->update(msgPretext);
}

void COOLManager::handleDeleteCOOLPretext(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	COOLPretext *msgPretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
	if (!msgPretext)
	{
		if (currentPretext)
			usingCurrent = true;
		else
			return;
	}

	DataManager *myManager = mgrContainer_->getCOOLPretextManager();
	if (!myManager)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolPreTexts))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentPretext, myManager->getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "handleCOOLPretextDelete(): Pretext # ";
			myDebug.append(Long::toString(msgPretext->getRecordNum())).append(" : ").append(msgPretext->getName1());
			Debug::trace("COOLManager.COOLPretext", myDebug);
		}

		HobRecordNum lDept = msgPretext->getDeptNum();
		COOLPretext *existingPretext = (usingCurrent ? currentPretext : COOLPretext::findCOOLPretext(msgPretext->getRecordNum(), lDept, dbAccess));
		if (existingPretext && existingPretext->getRecordNum() != RECORD_NOT_FOUND)
		{
			// Before deleting the COOLPretext record, delete all CoolCategoryPretext records
			// associated with this COOLPretext.
			DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
			if (dbCoolCategoryPretextsRM)
			{
				DBQuery<DBCoolCategoryPreText>* allCoolCategoryPretextsQuery = dbCoolCategoryPretextsRM->retrieveLinkedtoCoolPreText(Long::toString(existingPretext->getRecordNum()), Long::toString(existingPretext->getDeptNum()));
				if (allCoolCategoryPretextsQuery)
				{
					const DBCoolCategoryPreText *coolCategoryPretext = (DBCoolCategoryPreText *) NULL;
					DBQuery<DBCoolCategoryPreText>::const_iterator tempIter = allCoolCategoryPretextsQuery->begin();
					while (tempIter != allCoolCategoryPretextsQuery->end())
					{
						coolCategoryPretext = new DBCoolCategoryPreText(*tempIter);
						++tempIter;
						if (coolCategoryPretext)
						{
							dbCoolCategoryPretextsRM->deleteRecord(coolCategoryPretext);
							delete coolCategoryPretext;
						}
					}
					delete allCoolCategoryPretextsQuery;
				}
			}

			existingPretext->deleteRecord();
			if (traceOn)
			{
				myDebug.append(", has been deleted.");
				Debug::trace("COOLManager.COOLPretext", myDebug);
			}

			if (msg->getEventType() != COOLDevPretextBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolPreTexts))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingPretext, myManager->getDeleteEvent());

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
				Debug::trace("COOLManager.COOLPretext", myDebug);
			}
		}

		if (usingCurrent)
			setCurrentPretext((COOLPretext *) NULL);
		else
			delete existingPretext;
	}
}

void COOLManager::handleDeleteAllCOOLPretexts(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLPretexts - deleting all COOL Pretexts");

	DataManager *myManager = mgrContainer_->getCOOLPretextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolPreTexts))
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

		{
			DBCoolPreTextsRM *dbRM = dbAccess->getDBCoolPreTextsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		// When deleting all CoolPreTexts, we also delete all CoolCategoryPreText bridge table records.
		// We don't delete the Cool Category records here.
		{
			DBCoolCategoryPreTextsRM *dbRM = dbAccess->getDBCoolCategoryPreTextsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		destroyCOOLPretextsQueryAndIter();
	}
}

void COOLManager::handleUpdateAndSaveCOOLPretext(COOLPretext *msgPretext)
{
	Debug::trace("COOLManager.COOLPretext", "handleUpdateAndSaveCOOLPretext(): currentPretext = " + ptrtostring(currentPretext));

	if (currentPretext)
	{
		if (msgPretext)
		  	currentPretext->update(msgPretext);
		handleSaveRecordCOOLPretext(currentPretext);
	}
}

void COOLManager::handleSaveRecordCOOLPretext(COOLPretext *recordToSave)
{
	Debug::trace("COOLManager.COOLPretext", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLPretextManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolPreTexts))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave, myManager->getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), synchSer)));
		}
	}
}


void COOLManager::handleRestoreCOOLPretext(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLPretext *restorePretext = dynamic_cast<COOLPretext *> (msg->getHfcObject());
	if (!restorePretext || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLPretextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolPreTexts))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restorePretext, myManager->getWriteEvent());

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
		restorePretext->setDBAccess(dbAccess);

		COOLPretext *usedPretext = 0;
		COOLPretext *existingPretext = COOLPretext::findCOOLPretext(restorePretext->getRecordNum(), restorePretext->getDeptNum(), dbAccess);
		if (existingPretext)
		{
			if (existingPretext->isRecordValid())
			{
				existingPretext->update(restorePretext);
				existingPretext->saveRecord();
				usedPretext = existingPretext;								
			}
			else
			{
				existingPretext->deleteRecord();	   
				restorePretext->saveRecord();
				usedPretext = restorePretext;								
			}
		}
		else
		{
			restorePretext->saveRecord();
			usedPretext = restorePretext;								
		}

		// If we receive the category number with a Cool Pretext record,
		// check if that category exists (if 0 then the Pretext belongs to 'All Categories'),
		// create it if it doesn't exist, and create the record (COLCaPs db record) that links
		// the Category with this Pretext record.
		if (restorePretext->isReceived(fieldOffsetCOOLPretextCategory) &&
			 restorePretext->getCategoryNum() > 0)
		{
			HobRecordNum lDept = restorePretext->getDeptNum();
			COOLCategory *coolCategory = COOLCategory::findCOOLCategory(restorePretext->getCategoryNum(), lDept, dbAccess);
			if (coolCategory)
				delete coolCategory;
			else
			{
				// Have to create the Cool Category for this Pretext.  It doesn't yet exist.
				coolCategory = new COOLCategory();
				if (coolCategory)
				{
					coolCategory->setDeptNum(lDept);
					coolCategory->setRecordNum(restorePretext->getCategoryNum());
					coolCategory->setName1(COOLCategory::autoGeneratedName + HobString(Long::toString(coolCategory->getRecordNum())));
					coolCategory->setErrorCode(RECORD_OK);
					coolCategory->setDBAccess(dbAccess);
					coolCategory->saveRecord();
					delete coolCategory;
				}
			}

			handleAddCOOLPretextToCategory(restorePretext);
		}

		if (usedPretext && usedPretext->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedPretext->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedPretext->nextError();
			}
		}
		if (existingPretext)
			delete existingPretext;
	}
}

void COOLManager::handleCreateCOOLPretext(COOLPretext *msgPretext)
{
	if (!msgPretext || !dbAccess)
		return;

	//  Create a new category from the database based on the category number from the message.
	COOLPretext *newPretext = new COOLPretext();
	if (newPretext)
	{
		newPretext->setDeptNum(msgPretext->getDeptNum());
		newPretext->setRecordNum(msgPretext->getRecordNum());
		newPretext->setErrorCode(RECORD_OK);
		newPretext->setDBAccess(dbAccess);

		//  Set this new category as the current category.
		setCurrentPretext(newPretext);

		if (mySession)
		{
			// Send the new product to the Browser.
			SessionMessage catData(COOLDev, COOLDevWritePretext);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentPretext);
			mySession->send(&catData, Messenger::DEST_UI);
		}
	}
}

void COOLManager::handleReadCOOLPretextsByOneCategory(AllCoolPretexts *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLPretextsByOneCategory():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolPretexts();
		deleteMyList = true;
		myList->findAllPretextsLinkedToAllCategories(dbAccess);
	}
	else
	{
		myList->findAllPretextsLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLPretextRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLPretext, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWritePretextsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLPretext", myDebug + "Deleted myList!");
	}
}

void COOLManager::handleAddCOOLPretextToCategory(COOLPretext *msgPretext)
{
	if (!msgPretext || !dbAccess)
		return;

	DBCoolCategoryPreTextsRM *dbCoolCategoryPreTextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolCategoryPreTextsRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
	HobString myDebug("handleCOOLPretextAddToCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgPretext = ").append(Hex::toString((long long) msgPretext));
		myDebug2.append("   Pretext = ").append(msgPretext->getName1());
		myDebug2.append("   Pretext # = ").append(Integer::toString(msgPretext->getRecordNum()));
		myDebug2.append("   Category = ").append(Long::toString(msgPretext->getCategoryNum()));
		myDebug2.append("   allCategories = ").append(Bool::toString(msgPretext->isAllCategoriesFlag()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}

	HobRecordNum lDept = msgPretext->getDeptNum();
	COOLPretext *coolPretext = COOLPretext::findCOOLPretext(msgPretext->getRecordNum(), lDept, dbAccess);
	if (!coolPretext)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLPretext ").append(msgPretext->getName1());
			Debug::trace("COOLManager.COOLPretext", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryPreText *dbCoolCategoryPretext = 0;

		if (msgPretext->isAllCategoriesFlag())
		{
#ifdef MAKE_MUTUALLY_EXCLUSIVE
			DBQuery<DBCoolCategoryPreText>* coolCategoryPretextsQuery = dbCoolCategoryPreTextsRM->retrieveLinkedtoCoolPreText(Long::toString(coolPretext->getRecordNum()));
			if (coolCategoryPretextsQuery)
			{
				if (coolCategoryPretextsQuery->size() && traceOn)
					Debug::trace("COOLManager.COOLPretext", myDebug + "Deleting DBCoolCategoryPreText records because allCategoriesFlag is True.");

				DBQuery<DBCoolCategoryPreText>::const_iterator tempIter = coolCategoryPretextsQuery->begin();
				while (tempIter != coolCategoryPretextsQuery->end())
				{
					dbCoolCategoryPretext = new DBCoolCategoryPreText(*tempIter);
					++tempIter;
					if (dbCoolCategoryPretext)
					{
						dbCoolCategoryPreTextsRM->deleteRecord(dbCoolCategoryPretext);
						delete dbCoolCategoryPretext;
					}
				}
				delete coolCategoryPretextsQuery;
			}
#endif
		}
		else
		{
			dbCoolCategoryPretext = dbCoolCategoryPreTextsRM->find(Long::toString(msgPretext->getCategoryNum()), Integer::toString(msgPretext->getRecordNum()), Integer::toString(msgPretext->getDeptNum()));
			if (dbCoolCategoryPretext)
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLPretext", myDebug + "This DBCoolCategoryPreText record already exists.");
			}
			else
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLPretext", myDebug + "Creating a new DBCoolCategoryPreText record.");

				dbCoolCategoryPretext = dbCoolCategoryPreTextsRM->createRecord(Long::toString(msgPretext->getCategoryNum()), Integer::toString(msgPretext->getRecordNum()), Long::toString(msgPretext->getDeptNum()));
				if (dbCoolCategoryPretext)
				{
					// Remember that this newCOOLCategoryPretext takes ownership of the dbCoolCategoryPretext record, so don't delete it after it is saved.
					COOLCategoryPretext newCOOLCategoryPretext(dbCoolCategoryPretext, dbAccess);
					if (traceOn)
						Debug::trace("COOLManager.COOLPretext", "Saving the new DBCoolCategoryPreText record.");

					handleSaveRecordCOOLCategoryPretext(&newCOOLCategoryPretext);
				}
			}
		}
		if (coolPretext->isAllCategoriesFlag() != msgPretext->isAllCategoriesFlag())
		{
			if (traceOn)
				Debug::trace("COOLManager.COOLCountry", myDebug + "Updating the allCategoriesFlag in the coolPretext record.");
			coolPretext->setAllCategoriesFlag(msgPretext->isAllCategoriesFlag());
			handleSaveRecordCOOLPretext(coolPretext);
		}
		delete coolPretext;
	}
}

void COOLManager::handleDeleteCOOLPretextFromCategory(COOLPretext *msgPretext)
{
	if (!msgPretext || !dbAccess)
		return;

	DBCoolCategoryPreTextsRM *dbCoolCategoryPreTextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolCategoryPreTextsRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
	HobString myDebug("handleCOOLPretextDeleteFromCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgPretext = ").append(Hex::toString((long long) msgPretext));
		myDebug2.append("   Pretext # = ").append(Integer::toString(msgPretext->getRecordNum()));
		myDebug2.append("   Category = ").append(Long::toString(msgPretext->getCategoryNum()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}

	COOLPretext *coolPretext = COOLPretext::findCOOLPretext(msgPretext->getRecordNum(), msgPretext->getDeptNum(), dbAccess);
	if (!coolPretext)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLPretext ").append(Long::toString(msgPretext->getRecordNum()));
			Debug::trace("COOLManager.COOLPretext", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryPreText *dbCoolCategoryPretext = dbCoolCategoryPreTextsRM->find(longtostring(msgPretext->getCategoryNum()), longtostring(msgPretext->getRecordNum()), longtostring(msgPretext->getDeptNum()));
		if (!dbCoolCategoryPretext)
		{
			Debug::trace("COOLManager.COOLPretext", "This DBCoolCategoryPreText does not exist.");

			// We still want to remove this Country from being in the "Any" category.
			coolPretext->setAllCategoriesFlag(false);
			handleSaveRecordCOOLPretext(coolPretext);
		}
		else
		{
			COOLCategoryPretext newCOOLCategoryPretext(dbCoolCategoryPretext, dbAccess);

			DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
			if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
			{
				// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
				HobString serString = "";
				serializeForSynchronization(serString, &newCOOLCategoryPretext, myManager->getDeleteEvent());

				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

				if (proxy)
					proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
						hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString)));
			}

			newCOOLCategoryPretext.deleteRecord();

			Debug::trace("COOLManager.COOLPretext", "Deleting the record.");
		}
		delete coolPretext;
	}
}

void COOLManager::handleReadCOOLPretextsByOneCategoryAndAllCategories(AllCoolPretexts *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLPretextsByOneCategoryAndAllCategories():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolPretexts();
		deleteMyList = true;
	}

	if (myList)
	{
		myList->findAllPretextsLinkedToAllCategories(dbAccess);
		if (myList->getCategoryNum() > 0)
			myList->findAllPretextsLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}
#ifndef MAKE_MUTUALLY_EXCLUSIVE
	myList->getMyList().unique();
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   After calling unique() on the list, myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}
#endif

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLPretextRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLPretext, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWritePretextsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLPretext", myDebug + "Deleted myList!");
	}
}

void COOLManager::handleReadCOOLPretextsByNotAllCategories(AllCoolPretexts *myList)
{
//	This method will read all Pretexts, then eliminate the Pretexts that have the allCategories flag set.
	bool traceOn = Debug::isTracing("COOLManager.COOLPretext");
	HobString myDebug;
	if (traceOn)
		myDebug = "handleReadCOOLPretextsByOneCategoryAndAllCategories():  ";

	if (!dbAccess)
		return;

	AllCoolPretexts *allCategoriesList = new AllCoolPretexts();
	if (!allCategoriesList)
		return;

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolPretexts();
		deleteMyList = true;
	}
	if (!myList)
	{
		delete allCategoriesList;
		return;
	}

	myList->findAllPretexts(dbAccess);
	allCategoriesList->findAllPretextsLinkedToAllCategories(dbAccess);

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		myDebug2.append("   allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
		Debug::trace("COOLManager.COOLPretext", myDebug2);
	}

	if (allCategoriesList->size())
	{
		//	Go through the lists and remove any countries from myList that are in allCategoriesList.
		HobString pretext;
		int returnVal;
		HobList_type tempList = myList->getMyList();

		returnVal = allCategoriesList->first(pretext);
		while (returnVal != -1)
		{
			tempList.remove(pretext);
			returnVal = allCategoriesList->next(pretext);
		}
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("After the removal of allCategories pretexts,  allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
			Debug::trace("COOLManager.COOLPretext", myDebug2);
		}
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLPretextRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLPretext, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWritePretextsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLPretext", myDebug + "Deleted myList!");
	}
	delete allCategoriesList;
}
