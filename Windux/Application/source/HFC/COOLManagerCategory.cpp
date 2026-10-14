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
#include "COOLCategoryManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLCategoryMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLCategoryMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevReadCategory:
		{
			COOLCategory *msgCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
			handleReadCOOLCategory(msgCategory);
			break;
		}
		case COOLDevReadAllCategories:
			handleReadAllCOOLCategories();
			break;
		case COOLDevWriteCategory:
		{
			COOLCategory *msgCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
			handleWriteCOOLCategory(msgCategory);
			break;
		}
		case COOLDevDeleteCategory:
		case COOLDevCategoryBatchApplyDelete:
			handleDeleteCOOLCategory(msg);
			break;
		case COOLDevDeleteAllCategories:
			handleDeleteAllCOOLCategories();
			break;
		case COOLDevSaveCategory:
		{
			COOLCategory *msgCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
			handleSaveCOOLCategory(msgCategory);
			break;
		}
		case COOLDevRestoreCategory:
			handleRestoreCOOLCategory(msg);
			break;
		case COOLDevCreateNewCategory:
		{
			COOLCategory *msgCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
			handleCreateCOOLCategory(msgCategory);
			break;
		}
		case COOLDevReadAllCategoriesByCountry:
		{
			AllCoolCategories *msgCategory = dynamic_cast<AllCoolCategories *> (msg->getHfcObject());
			handleReadAllCOOLCategoriesByCountry(msgCategory);
			break;
		}
		case COOLDevReadAllCategoriesByProdStep:
		{
			AllCoolCategories *msgCategory = dynamic_cast<AllCoolCategories *> (msg->getHfcObject());
			handleReadAllCOOLCategoriesByProdStep(msgCategory);
			break;
		}
		case COOLDevReadAllCategoriesByPretext:
		{
			AllCoolCategories *msgCategory = dynamic_cast<AllCoolCategories *> (msg->getHfcObject());
			handleReadAllCOOLCategoriesByPretext(msgCategory);
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


COOLCategory *COOLManager::getCurrentCategory()
{
	return currentCategory;
}
void COOLManager::setCurrentCategory(COOLCategory *category)
{
	if (currentCategory)
		delete currentCategory;
	currentCategory = category;
}


/* If the first record was not read, this method will return NULL.
 */
COOLCategory *COOLManager::firstCOOLCategory()
{
	if (!dbAccess)
		return 0;

	DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
	if (!dbCoolCategoriesRM)
		return 0;

	// If the user calls firstCOOLCategory() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLCategoriesQueryAndIter();

	// Only used by backuprestore, and backuprestore needs all from all depts, so we dont pass deptNum
	allCoolCategoriesQuery = dbCoolCategoriesRM->retrieveAll();

	// Couldn't get the query from which to access the Categories, just return false.
	if (!allCoolCategoriesQuery)
		return 0;

	DBQuery<DBCoolCategory>::const_iterator tempIter = allCoolCategoriesQuery->begin();
	if (tempIter == allCoolCategoriesQuery->end())
		return 0;

	//coolCategoryCount = 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLCategory()
	 * method call below).
	 */
	allCoolCategoriesIter = new DBQuery<DBCoolCategory>::const_iterator(tempIter);
	DBCoolCategory *c = new DBCoolCategory(*(*allCoolCategoriesIter));
	COOLCategory *newCategory = new COOLCategory(c, dbAccess);
	return newCategory;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstCOOLCategory() method, the first call to this method will
 * result in the first record being returned.
 */
COOLCategory *COOLManager::nextCOOLCategory()
{
	if (!dbAccess)
		return 0;

	if (allCoolCategoriesQuery && !allCoolCategoriesQuery->isEnabled())
		destroyCOOLCategoriesQueryAndIter();

	if (!allCoolCategoriesQuery || !allCoolCategoriesIter)
		return firstCOOLCategory();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolCategoriesIter);
	if (*allCoolCategoriesIter == allCoolCategoriesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Categories in the db. */
		destroyCOOLCategoriesQueryAndIter();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolCategory copy constructor in order for the Hfc COOLCategory to
	 * control the destruction of the DBCoolCategory record that the iterator contains; otherwise, the DBCoolCategory will
	 * be deleted when the iterator goes out of scope.  The DBCoolCategory copy will be deleted by the COOLCategory record
	 * created below.
	 */
	DBCoolCategory *c = new DBCoolCategory(*(*allCoolCategoriesIter));
	COOLCategory *newCategory = new COOLCategory(c, dbAccess);
	return newCategory;
}


/* Provide a countAll method so the caller doesn't need to know about a dbCoolCategoriesRM.
 */
int COOLManager::countAllCOOLCategories()
{
	if (!dbAccess)
		return -1;

	DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
	if (!dbCoolCategoriesRM)
		return -1;

	return dbCoolCategoriesRM->countAll();
}


void COOLManager::handleReadCOOLCategory(COOLCategory *msgCategory)
{
	if (!msgCategory || !dbAccess)
		return;

	if (msgCategory->getRecordNum() == (HobRecordNum)RECORD_NOT_FOUND)
	{
		setCurrentCategory((COOLCategory *) NULL);
		return;
	}

	COOLCategory *readCategory = COOLCategory::findCOOLCategory(msgCategory->getRecordNum(), msgCategory->getDeptNum(), dbAccess);

	if (!readCategory)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
		{
			if (s->getCurrentMode() == OpModeSupervisor)
			{
				// Send error message to browser indicating Cool category not found.
				mySession->addDialogMessage(ErrCOOLDev, COOLDevErrCategoryNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

				if (readCategory)
					delete readCategory;
				return;
			}
		}
	}
	setCurrentCategory(readCategory);

	if (mySession)
	{
		// Send the COOL category found back to the Browser.
		SessionMessage coolData(COOLDev, COOLDevWriteCategory);
		coolData.setFormatType(BrowserFormat);
		coolData.setHfcObject(currentCategory);

		// Send the product to the UI (Browser)
		mySession->send(&coolData, Messenger::DEST_UI);
	}
}


void COOLManager::handleReadAllCOOLCategories()
{
	if (!dbAccess)
		return;

	AllCoolCategories *myCategories = new AllCoolCategories();
	if (!myCategories)
		return;

	myCategories->findAllCategories(dbAccess);

	if (Debug::isTracing("COOLManager.COOLCategory"))
	{
		HobString myDebug("handling event: ");
		myDebug.append("handleCOOLCategoryReadAll(): myCategories size = ").append(Integer::toString((int)myCategories->getNamesList()->size()));
		Debug::trace("COOLManager.COOLCategory", myDebug);
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLCategoryRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLCategory, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteAllCategories);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myCategories);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	delete myCategories;
}

void COOLManager::handleWriteCOOLCategory(COOLCategory *msgCategory)
{
	if ((!msgCategory) || (!currentCategory))
		return;

	if (Debug::isTracing("COOLManager.COOLCategory"))
	{
		HobString myDebug("handleCOOLCategoryWrite(): currentCategory = ");
		myDebug.append(Hex::toString((long long) currentCategory)).append("  , msgCategory = ").append(Hex::toString((long long) msgCategory));
		Debug::trace("COOLManager.COOLCategory", myDebug);
	}
	currentCategory->update(msgCategory);
}

void COOLManager::handleDeleteCOOLCategory(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	COOLCategory *msgCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
	if (!msgCategory)
	{
		if (currentCategory)
			usingCurrent = true;
		else
			return;
	}

	DataManager *myManager = mgrContainer_->getCOOLCategoryManager();
	if (!myManager)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategories))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentCategory, myManager->getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("COOLManager.COOLCategory");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "handleCOOLCategoryDelete(): Category # ";
			myDebug.append(Long::toString(msgCategory->getRecordNum())).append(" : ").append(msgCategory->getName1());
			Debug::trace("COOLManager.COOLCategory", myDebug);
		}
		COOLCategory *existingCategory = (usingCurrent ? currentCategory : COOLCategory::findCOOLCategory(msgCategory->getRecordNum(), msgCategory->getDeptNum(), dbAccess));
		if (existingCategory)
		{
			if (existingCategory->getRecordNum() != RECORD_NOT_FOUND)
			{
				existingCategory->deleteRecord();
				if (traceOn)
				{
					myDebug.append(", has been deleted.");
					Debug::trace("COOLManager.COOLCategory", myDebug);
				}

				if (msg->getEventType() != COOLDevCategoryBatchApplyDelete)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategories))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, existingCategory, myManager->getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
					}
				}
			}
		}
		else
		{
			if (traceOn)
			{
				myDebug.append(", not found in DB so it could not be deleted.");
				Debug::trace("COOLManager.COOLCategory", myDebug);
			}
		}

		if (existingCategory)
		{
			if (usingCurrent)
				setCurrentCategory((COOLCategory *) NULL);
			else
				delete existingCategory;
		}
	}
}

void COOLManager::handleDeleteAllCOOLCategories(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLCategories - deleting all COOL categories");

	DataManager *myManager = mgrContainer_->getCOOLCategoryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategories))
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

		DBCoolCategoriesRM *dbRM = dbAccess->getDBCoolCategoriesRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		// NOTE:  We do not delete the bridge table records for CoolCategoryCountries,
		//        CoolCategoryProductionSteps, or CoolCategoryPretexts when we delete all
		//        Categories.  These bridge table records are deleted when their corresponding
		//        Country, ProductionStep, or Pretext record is deleted.

		destroyCOOLCategoriesQueryAndIter();
	}
}

void COOLManager::handleSaveCOOLCategory(COOLCategory *msgCategory)
{
	if (Debug::isTracing("COOLManager.COOLCategory"))
	{
		HobString myDebug("handleCOOLCategorySave(): currentCategory = ");
		myDebug.append(Hex::toString((long long) currentCategory));
		Debug::trace("COOLManager.COOLCategory", myDebug);
	}
	if (currentCategory)
	{
		if (msgCategory)
			currentCategory->update(msgCategory);
		currentCategory->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLCategoryManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategories))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, msgCategory, myManager->getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), synchSer)));
		}
	}
}

void COOLManager::handleRestoreCOOLCategory(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategory *restoreCategory = dynamic_cast<COOLCategory *> (msg->getHfcObject());
	if (!restoreCategory || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCategoryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategories))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreCategory, myManager->getWriteEvent());

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
		restoreCategory->setDBAccess(dbAccess);

		COOLCategory *usedCategory = (COOLCategory *)NULL;
		COOLCategory *existingCategory = COOLCategory::findCOOLCategory(restoreCategory->getRecordNum(), restoreCategory->getDeptNum(), dbAccess);
		if (existingCategory)
		{
			if (existingCategory->isRecordValid())
			{
				existingCategory->update(restoreCategory);
				existingCategory->saveRecord();
				usedCategory = existingCategory;								
			}
			else
			{
				existingCategory->deleteRecord();	   
				restoreCategory->saveRecord();
				usedCategory = restoreCategory;								
			}
		}
		else
		{
			restoreCategory->saveRecord();
			usedCategory = restoreCategory;								
		}

		if (usedCategory && usedCategory->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedCategory->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedCategory->nextError();
			}
		}
		if (existingCategory)
			delete existingCategory;
	}
}

void COOLManager::handleCreateCOOLCategory(COOLCategory *msgCategory)
{
	if (!msgCategory || !dbAccess)
		return;

	//  Create a new category from the database based on the category number from the message.
	COOLCategory *newCategory = new COOLCategory();
	if (newCategory)
	{
		newCategory->setDeptNum(msgCategory->getDeptNum());
		newCategory->setRecordNum(msgCategory->getRecordNum());
		newCategory->setErrorCode(RECORD_OK);
		newCategory->setDBAccess(dbAccess);

		//  Set this new category as the current category.
		setCurrentCategory(newCategory);

		if (mySession)
		{
			// Send the new product to the Browser.
			SessionMessage catData(COOLDev, COOLDevWriteCategory);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentCategory);
			mySession->send(&catData, Messenger::DEST_UI);
		}
	}
}

void COOLManager::handleReadAllCOOLCategoriesByCountry(AllCoolCategories *msgCategory)
{
	if (!dbAccess || !msgCategory)
		return;

	msgCategory->findAllCategoriesLinkedToCountry(dbAccess);

	if (Debug::isTracing("COOLManager.COOLCategory"))
	{
		HobString myDebug("handling event: ");
		myDebug.append("handleCOOLCategoryReadAllByCountry(): country = ").append(msgCategory->getLinkTo());
		myDebug.append("   msgCategory size = ").append(Integer::toString((int)msgCategory->getRecordNumList()->size()));
		Debug::trace("COOLManager.COOLCategory", myDebug);
	}

	if (mySession)
	{
		// Send the list to the Browser.
		SessionMessage ctData(COOLDev, COOLDevWriteAllCategoriesByCountry);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(msgCategory);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLCategoriesByProdStep(AllCoolCategories *msgCategory)
{
	if (!dbAccess || !msgCategory)
		return;

	msgCategory->findAllCategoriesLinkedToProductionStep(dbAccess);

	if (Debug::isTracing("COOLManager.COOLProductionStep"))
	{
		HobString myDebug("handling event: ");
		myDebug.append("handleCOOLCategoryReadAllByProdStep(): country = ").append(msgCategory->getLinkTo());
		myDebug.append("   msgCategory size = ").append(Integer::toString((int)msgCategory->getRecordNumList()->size()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug);
	}

	if (mySession)
	{
		// Send the list to the Browser.
		SessionMessage ctData(COOLDev, COOLDevWriteAllCategoriesByProdStep);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(msgCategory);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLCategoriesByPretext(AllCoolCategories *msgCategory)
{
	if (!dbAccess || !msgCategory)
		return;

	msgCategory->findAllCategoriesLinkedToPretext(dbAccess);

	if (Debug::isTracing("COOLManager.COOLPretext"))
	{
		HobString myDebug("handling event: ");
		myDebug.append("handleCOOLCategoryReadAllByPretext(): country = ").append(msgCategory->getLinkTo());
		myDebug.append("   msgCategory size = ").append(Integer::toString((int)msgCategory->getRecordNumList()->size()));
		Debug::trace("COOLManager.COOLPretext", myDebug);
	}

	if (mySession)
	{
		// Send the list to the Browser.
		SessionMessage ctData(COOLDev, COOLDevWriteAllCategoriesByPretext);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(msgCategory);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
}
