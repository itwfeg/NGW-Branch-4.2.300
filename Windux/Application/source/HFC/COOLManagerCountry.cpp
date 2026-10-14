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
#include "COOLCategoryCountry.h"
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

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"
#include "Configuration.h"
#include "COOLCountryManager.h"
#include "COOLCategoryCountryManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLCountryMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLCountryMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevReadCountry:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleReadCOOLCountry(msgCountry);
			break;
		}
		case COOLDevReadAllCountries:
		{
			AllCoolCountries *msgAll = dynamic_cast<AllCoolCountries *> (msg->getHfcObject());
			handleReadAllCOOLCountries(msgAll);
			break;
		}
		case COOLDevWriteCountry:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleWriteCOOLCountry(msgCountry);
			break;
		}
		case COOLDevDeleteCountry:
		case COOLDevCountryBatchApplyDelete:
			handleDeleteCOOLCountry(msg);
			break;
		case COOLDevDeleteAllCountries:
		case COOLDevDeleteAllCountryTextForUltima:
			handleDeleteAllCOOLCountries();
			break;
		case COOLDevSaveCountry:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleUpdateAndSaveCOOLCountry(msgCountry);
			break;
		}
		case COOLDevRestoreCountry:
			handleRestoreCOOLCountry(msg);
			break;
		case COOLDevCreateNewCountry:
			handleCreateCOOLCountry();
			break;
		case COOLDevReadByCategoryOnlyCountries:
		{
			AllCoolCountries *msgAll = dynamic_cast<AllCoolCountries *> (msg->getHfcObject());
			handleReadCOOLCountriesByOneCategory(msgAll);
			break;
		}
		case COOLDevAddCountryToCategory:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleAddCOOLCountryToCategory(msgCountry);
			break;
		}
		case COOLDevDeleteCountryFromCategory:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleDeleteCOOLCountryFromCategory(msgCountry);
			break;
		}
		case COOLDevCreateNewCountryAndSave:
		{
			COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
			handleCreateCOOLCountryAndSave(msgCountry);
			break;
		}
		case COOLDevReadByAllAndByCategoryCountries:
		{
			AllCoolCountries *msgAll = dynamic_cast<AllCoolCountries *> (msg->getHfcObject());
			handleReadCOOLCountriesByOneCategoryAndAllCategories(msgAll);
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


COOLCountry *COOLManager::getCurrentCountry()
{
	return currentCountry;
}
void COOLManager::setCurrentCountry(COOLCountry *country)
{
	if (currentCountry)
		delete currentCountry;
	currentCountry = country;
}


COOLCountry *COOLManager::firstCOOLCountry()
{
	if (!dbAccess)
		return (COOLCountry *)NULL;

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
		return (COOLCountry *)NULL;

	// If the user calls firstCOOLCountry() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLCountriesQueryAndIter();

	// Only used by backuprestore, and backuprestore needs all from all depts, so we dont pass deptNum
	allCoolCountriesQuery = dbCoolCountriesRM->retrieveAll();

	// Couldn't get the query from which to access the Countries, just return false.
	if (!allCoolCountriesQuery)
		return (COOLCountry *)NULL;

	DBQuery<DBCoolCountry>::const_iterator tempIter = allCoolCountriesQuery->begin();
	if (tempIter == allCoolCountriesQuery->end())
		return (COOLCountry *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLCountry()
	 * method call below).
	 */
	allCoolCountriesIter = new DBQuery<DBCoolCountry>::const_iterator(tempIter);
	DBCoolCountry *c = new DBCoolCountry(*(*allCoolCountriesIter));
	COOLCountry *newCountry = new COOLCountry(c, dbAccess);
	return newCountry;
}

COOLCountry *COOLManager::nextCOOLCountry()
{
	if (!dbAccess)
		return (COOLCountry *)NULL;

	if (allCoolCountriesQuery && !allCoolCountriesQuery->isEnabled())
		destroyCOOLCountriesQueryAndIter();

	if (!allCoolCountriesQuery || !allCoolCountriesIter)
		return firstCOOLCountry();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolCountriesIter);
	if (*allCoolCountriesIter == allCoolCountriesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Countries in the db. */
		destroyCOOLCountriesQueryAndIter();
		return (COOLCountry *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolCountry copy constructor in order for the Hfc COOLCountry to
	 * control the destruction of the DBCoolCountry record that the iterator contains; otherwise, the DBCoolCountry will
	 * be deleted when the iterator goes out of scope.  The DBCoolCountry copy will be deleted by the COOLCountry record
	 * created below.
	 */
	DBCoolCountry *c = new DBCoolCountry(*(*allCoolCountriesIter));
	COOLCountry *newCountry = new COOLCountry(c, dbAccess);
	return newCountry;
}


int COOLManager::countAllCOOLCountries()
{
	if (!dbAccess)
		return -1;

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
		return -1;

	return dbCoolCountriesRM->countAll();
}


int COOLManager::countAllCOOLCountriesForAllCategories()
{
	if (!dbAccess)
		return -1;

	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString msgDebug;
	if (traceOn)
		HobString msgDebug = "countAllCOOLCountriesForAllCategories():  ";

	int totalCount = -1;

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (dbCoolCountriesRM)
	{
		// DBQuery<DBCoolCountry> *myQuery = dbCoolCountriesRM->retrieveAll();
		DBQuery<DBCoolCountry> *myQuery = dbCoolCountriesRM->retrieveAllForDept(Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));

		if (myQuery)
		{
			DBQuery<DBCoolCountry>::const_iterator tempIter = myQuery->begin();
			totalCount = 0;
			while (tempIter != myQuery->end())
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  getName1() = ").append(tempIter->getName1());
					msgDebug2.append("  getAllCategories() = ").append(Bool::toString(tempIter->getAllCategories()));
					msgDebug2.append("  getCountryNumber() = ").append(tempIter->getCountryNumber());
					Debug::trace("COOLManager.COOLCountry", msgDebug2);
				}
				if (!tempIter->isAllCategoriesNull() && tempIter->getAllCategories())
					++totalCount;
				++tempIter;
			}
			delete myQuery;
			myQuery = 0;
		}
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("totalCount = ").append(Integer::toString(totalCount));
		Debug::trace("COOLManager.COOLCountry", msgDebug2);
	}

	return totalCount;
}

void COOLManager::handleReadCOOLCountry(COOLCountry *msgCountry)
{
	if (!msgCountry || !dbAccess)
		return;

	if (msgCountry->getName1() == "")
	{
		setCurrentCountry((COOLCountry *) NULL);
		return;
	}
	COOLCountry *readCountry = COOLCountry::findCOOLCountry(msgCountry->getName1(), msgCountry->getDeptNum(), dbAccess);
	if (!readCountry)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
		{
			if (s->getCurrentMode() == OpModeSupervisor)
			{
				// Send error message to browser indicating Cool category not found.
				mySession->addDialogMessage(ErrCOOLDev, COOLDevErrCountryNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

				if (readCountry)
					delete readCountry;
				return;
			}
		}
	}
	setCurrentCountry(readCountry);

	if (mySession)
	{
		// Send the COOL category found back to the Browser.
		SessionMessage coolData(COOLDev, COOLDevWriteCountry);
		coolData.setFormatType(BrowserFormat);
		coolData.setHfcObject(currentCountry);

		// Send the product to the UI (Browser)
		mySession->send(&coolData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLCountries(AllCoolCountries * /* msgAll */)
{
#if 1
	if (!dbAccess)
		return;
#else
	if (!dbAccess || !msgAll)
		return;

	if (msgAll->doExcludeAllCategories())
		handleReadCOOLCountriesByNotAllCategories(msgAll);
	else
	{
#endif
		AllCoolCountries *myList = new AllCoolCountries();
		if (!myList)
			return;

		myList->findAllCountries(dbAccess);

		if (Debug::isTracing("COOLManager.COOLCountry"))
		{
			HobString myDebug("handling event: ");
				myDebug.append("handleReadAllCOOLCountries(): myList size = ").append(Integer::toString((int)myList->size()));
			Debug::trace("COOLManager.COOLCountry", myDebug);
		}

		if (mySession)
		{
			// Send the class found back to the Browser.
			// Is this the behavior for ALL COOLCountryRead messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a COOLCountry, but doesn't want the data sent back?
			SessionMessage ctData(COOLDev, COOLDevWriteAllCountries);
			ctData.setFormatType(BrowserFormat);
			ctData.setHfcObject(myList);

			// Send the product to the UI (Browser)
			mySession->send(&ctData, Messenger::DEST_UI);
		}
		delete myList;
#if 0
	}
#endif
}

void COOLManager::handleWriteCOOLCountry(COOLCountry *msgCountry)
{
	if ((!msgCountry) || (!currentCountry))
		return;

	if (Debug::isTracing("COOLManager.COOLCountry"))
	{
		HobString myDebug("handleCOOLCountryWrite(): currentCountry = ");
		myDebug.append(Hex::toString((long long) currentCountry)).append("  , msgCountry = ").append(Hex::toString((long long) msgCountry));
		Debug::trace("COOLManager.COOLCountry", myDebug);
	}
	currentCountry->update(msgCountry);
}

void COOLManager::handleDeleteCOOLCountry(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	COOLCountry *msgCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
	if (!msgCountry)
	{
		if (currentCountry)
			usingCurrent = true;
		else
			return;
	}

	DataManager *myManager = mgrContainer_->getCOOLCountryManager();
	if (!myManager)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentCountry, myManager->getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "handleCOOLCountryDelete(): Name 1: ";
			myDebug.append(msgCountry->getName1());
			Debug::trace("COOLManager.COOLCountry", myDebug);
		}
		COOLCountry *existingCountry = (usingCurrent ? currentCountry : COOLCountry::findCOOLCountry(msgCountry->getName1(), msgCountry->getDeptNum(), dbAccess));
		if (existingCountry)
		{
			// Before deleting the CoolCountry record, delete all CoolCategoryCountry records
			// associated with this CoolCountry.
			DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
			if (dbCoolCategoryCountriesRM)
			{
				DBQuery<DBCoolCategoryCountry>* allCoolCategoryCountriesQuery = dbCoolCategoryCountriesRM->retrieveLinkedtoCoolCountry(existingCountry->getMyDbRecord());
				if (allCoolCategoryCountriesQuery)
				{
					const DBCoolCategoryCountry *coolCategoryCountry = (DBCoolCategoryCountry *) NULL;
					DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = allCoolCategoryCountriesQuery->begin();
					while (tempIter != allCoolCategoryCountriesQuery->end())
					{
						coolCategoryCountry = new DBCoolCategoryCountry(*tempIter);
						++tempIter;
						if (coolCategoryCountry)
						{
							dbCoolCategoryCountriesRM->deleteRecord(coolCategoryCountry);
							delete coolCategoryCountry;
						}
					}
					delete allCoolCategoryCountriesQuery;
				}
			}

			existingCountry->deleteRecord();
			if (traceOn)
			{
				myDebug.append(", has been deleted.");
				Debug::trace("COOLManager.COOLCountry", myDebug);
			}

			if (msg->getEventType() != COOLDevCountryBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingCountry, myManager->getDeleteEvent());

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
				Debug::trace("COOLManager.COOLCountry", myDebug);
			}
		}

		if (usingCurrent)
			setCurrentCountry((COOLCountry *) NULL);
		else
			delete existingCountry;
	}
}

void COOLManager::handleDeleteAllCOOLCountries(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLCountries - deleting all COOL countries");

	DataManager *myManager = mgrContainer_->getCOOLCountryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
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
			DBCoolCountriesRM *dbRM = dbAccess->getDBCoolCountriesRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		// When deleting all CoolCountries, we also delete all CoolCategoryCountry bridge table records.
		// We don't delete the Cool Category records here.
		{
			DBCoolCategoryCountriesRM *dbRM = dbAccess->getDBCoolCategoryCountriesRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		destroyCOOLCountriesQueryAndIter();
	}
}


void COOLManager::handleUpdateAndSaveCOOLCountry(COOLCountry *msgCountry)
{
	Debug::trace("COOLManager.COOLCountry", "handleUpdateAndSaveCOOLCountry(): currentCountry = " + ptrtostring(currentCountry));

	if (currentCountry)
	{
		if (msgCountry)
		  	currentCountry->update(msgCountry);
		handleSaveRecordCOOLCountry(currentCountry);
	}
}

void COOLManager::handleSaveRecordCOOLCountry(COOLCountry *recordToSave)
{
	Debug::trace("COOLManager.COOLCountry", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLCountryManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
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


void COOLManager::handleRestoreCOOLCountry(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCountry *restoreCountry = dynamic_cast<COOLCountry *> (msg->getHfcObject());
	if (!restoreCountry || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCountryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCountries))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreCountry, myManager->getWriteEvent());

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
		restoreCountry->setDBAccess(dbAccess);

		COOLCountry *usedCountry = (COOLCountry *)NULL;
		COOLCountry *existingCountry = COOLCountry::findCOOLCountry(restoreCountry->getName1(), restoreCountry->getDeptNum(), dbAccess);
		if (existingCountry)
		{
			if (existingCountry->isRecordValid())
			{
				existingCountry->update(restoreCountry);
				existingCountry->saveRecord();
				usedCountry = existingCountry;								
			}
			else
			{
				existingCountry->deleteRecord();	   
				restoreCountry->saveRecord();
				usedCountry = restoreCountry;								
			}
		}
		else
		{
			restoreCountry->saveRecord();
			usedCountry = restoreCountry;								
		}

		// Cool Country records received in Ultima format also contain the category number.
		// So check if that category exists (if 0 then the Country belongs to 'All Categories'),
		// create it if it doesn't exist, and create the record (COLCaCs db record) that links
		// the Category with this Country record.
		// Note:  If a country number (tag c5) is passed in with the country and the country
		// belongs to 'All Categories', the country number will be lost/ignored, since we won't be
		// creating the bridge table record (in colcacs) for countries that belong to 'All Categories'.
		if (restoreCountry->isReceived(fieldOffsetCOOLCountryCategory) &&
			 restoreCountry->getCategoryNum() > 0)
		{
			HobRecordNum lDept = restoreCountry->getDeptNum();
			COOLCategory *coolCategory = COOLCategory::findCOOLCategory(restoreCountry->getCategoryNum(), lDept, dbAccess);
			if (coolCategory)
				delete coolCategory;
			else
			{
				// Have to create the Cool Category for this Country.  It doesn't yet exist.
				coolCategory = new COOLCategory();
				if (coolCategory)
				{
					coolCategory->setDeptNum(lDept);
					coolCategory->setRecordNum(restoreCountry->getCategoryNum());
					coolCategory->setName1(COOLCategory::autoGeneratedName + HobString(Long::toString(coolCategory->getRecordNum())));
					coolCategory->setErrorCode(RECORD_OK);
					coolCategory->setDBAccess(dbAccess);
					coolCategory->saveRecord();
					delete coolCategory;
				}
			}
			handleAddCOOLCountryToCategory(restoreCountry);
		}

		if (usedCountry && usedCountry->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedCountry->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedCountry->nextError();
			}
		}
		if (existingCountry)
			delete existingCountry;
	}
}

void COOLManager::handleCreateCOOLCountry()
{
	if (!dbAccess)
		return;

	//  Create a new category from the database based on the category number from the message.
	COOLCountry *newCountry = new COOLCountry();
	if (newCountry)
	{
		Debug::trace("COOLManager.COOLCountry", "Creating a new COOLCountry with no name1");

		newCountry->setErrorCode(RECORD_OK);
		newCountry->setDBAccess(dbAccess);

		//  Set this new category as the current category.
		setCurrentCountry(newCountry);

		if (mySession)
		{
			// Send the new product to the Browser.
			SessionMessage catData(COOLDev, COOLDevWriteCountry);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentCountry);
			mySession->send(&catData, Messenger::DEST_UI);
		}
	}
}

void COOLManager::handleReadCOOLCountriesByOneCategory(AllCoolCountries *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLCountriesByOneCategory():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolCountries();
		deleteMyList = true;
		myList->findAllCountriesLinkedToAllCategories(dbAccess);
	}
	else
	{
		myList->findAllCountriesLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLCountryRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLCountry, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteCountriesByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLCountry", myDebug + "Deleted myList!");
	}
}

void COOLManager::handleAddCOOLCountryToCategory(COOLCountry *msgCountry)
{
	if (!msgCountry || !dbAccess)
		return;

	DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString myDebug("handleAddCOOLCountryToCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgCountry = ").append(Hex::toString((long long) msgCountry));
		myDebug2.append("   Country = ").append(msgCountry->getName1());
		myDebug2.append("   CountryNum = ").append(Long::toString(msgCountry->getCountryNum()));
		myDebug2.append("   Category = ").append(Long::toString(msgCountry->getCategoryNum()));
		myDebug2.append("   allCategories = ").append(Bool::toString(msgCountry->isAllCategoriesFlag()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}
	COOLCountry *coolCountry = COOLCountry::findCOOLCountry(msgCountry->getName1(), msgCountry->getDeptNum(), dbAccess);
	if (!coolCountry)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLCountry ").append(msgCountry->getName1());
			Debug::trace("COOLManager.COOLCountry", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryCountry *dbCoolCategoryCountry = 0;

		if (msgCountry->isAllCategoriesFlag())
		{
#ifdef MAKE_MUTUALLY_EXCLUSIVE
			DBQuery<DBCoolCategoryCountry>* coolCategoryCountriesQuery = dbCoolCategoryCountriesRM->retrieveLinkedtoCoolCountry(coolCountry->getMyDbRecord());
			if (coolCategoryCountriesQuery)
			{
				if (coolCategoryCountriesQuery->size() && traceOn)
					Debug::trace("COOLManager.COOLCountry", myDebug + "Deleting DBCoolCategoryCountry records because allCategoriesFlag is True.");

				DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = coolCategoryCountriesQuery->begin();
				while (tempIter != coolCategoryCountriesQuery->end())
				{
					dbCoolCategoryCountry = new DBCoolCategoryCountry(*tempIter);
					++tempIter;
					if (dbCoolCategoryCountry)
					{
						dbCoolCategoryCountriesRM->deleteRecord(dbCoolCategoryCountry);
						delete dbCoolCategoryCountry;
					}
				}
				delete coolCategoryCountriesQuery;
			}
#endif
		}
		else
		{
			dbCoolCategoryCountry = dbCoolCategoryCountriesRM->find(Long::toString(msgCountry->getCategoryNum()), coolCountry->getMyDbRecord());
			if (dbCoolCategoryCountry)
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLCountry", myDebug + "This DBCoolCategoryCountry record already exists.");
			}
			else
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLCountry", myDebug + "Creating a new DBCoolCategoryCountry record.");

				dbCoolCategoryCountry = dbCoolCategoryCountriesRM->createRecord(Long::toString(msgCountry->getCategoryNum()), coolCountry->getMyDbRecord());
				if (dbCoolCategoryCountry)
				{
					// Remember that this newCOOLCategoryCountry takes ownership of the dbCoolCategoryCountry record, so don't delete it after it is saved.
					COOLCategoryCountry newCOOLCategoryCountry(dbCoolCategoryCountry, dbAccess);
					if (traceOn)
						Debug::trace("COOLManager.COOLCountry", myDebug + "Saving the new DBCoolCategoryCountry record.");

					handleSaveRecordCOOLCategoryCountry(&newCOOLCategoryCountry);
				}
			}
		}
		if (coolCountry->isAllCategoriesFlag() != msgCountry->isAllCategoriesFlag())
		{
			if (traceOn)
				Debug::trace("COOLManager.COOLCountry", myDebug + "Updating the allCategoriesFlag in the coolCountry record.");
			coolCountry->setAllCategoriesFlag(msgCountry->isAllCategoriesFlag());
			handleSaveRecordCOOLCountry(coolCountry);
		}
		delete coolCountry;
	}
}

void COOLManager::handleDeleteCOOLCountryFromCategory(COOLCountry *msgCountry)
{
	if (!msgCountry || !dbAccess)
		return;

	DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString myDebug("handleCOOLCountryDeleteFromCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgCountry = ").append(Hex::toString((long long) msgCountry));
		myDebug2.append("   Country = ").append(msgCountry->getName1());
		myDebug2.append("   Category = ").append(Long::toString(msgCountry->getCategoryNum()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}
	COOLCountry *coolCountry = COOLCountry::findCOOLCountry(msgCountry->getName1(), msgCountry->getDeptNum(), dbAccess);
	if (!coolCountry)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLCountry ").append(msgCountry->getName1());
			Debug::trace("COOLManager.COOLCountry", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryCountry *dbCoolCategoryCountry = dbCoolCategoryCountriesRM->find(Long::toString(msgCountry->getCategoryNum()), coolCountry->getMyDbRecord());
		if (!dbCoolCategoryCountry)
		{
			Debug::trace("COOLManager.COOLCountry", "This DBCoolCategoryCountry does not exist.");

			// We still want to remove this Country from being in the "Any" category.
			coolCountry->setAllCategoriesFlag(false);
			handleSaveRecordCOOLCountry(coolCountry);
		}
		else
		{
			COOLCategoryCountry newCOOLCategoryCountry(dbCoolCategoryCountry, dbAccess);

			DataManager *myManager = mgrContainer_->getCOOLCategoryCountryManager();
			if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
			{
				// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
				HobString serString = "";
				serializeForSynchronization(serString, &newCOOLCategoryCountry, myManager->getDeleteEvent());

				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

				if (proxy)
					proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
						hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString)));
			}

			newCOOLCategoryCountry.deleteRecord();

			Debug::trace("COOLManager.COOLCountry", "Deleting the record.");
		}
		delete coolCountry;
	}
}

void COOLManager::handleCreateCOOLCountryAndSave(COOLCountry *msgCountry)
{
	if (!dbAccess || !msgCountry)
		return;

	COOLCountry *newCountry = new COOLCountry(msgCountry);
	if (newCountry)
	{
		newCountry->setDBAccess(dbAccess);

		//	Save the new record
		handleSaveRecordCOOLCountry(newCountry);

		//  Set this new country as the current country.
		setCurrentCountry(newCountry);
		Debug::trace("COOLManager.COOLCountry", "Created and saved the new Country. It is now the currentCountry.");
/*
	I don't think we want to send anything back right now.
		if (mySession)
		{
			// Send the new product to the Browser.
			SessionMessage catData(COOLDev, COOLDevWriteCountry);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentCountry);
			mySession->send(&catData, Messenger::DEST_UI);
		}
*/
	}
}


void COOLManager::handleReadCOOLCountriesByOneCategoryAndAllCategories(AllCoolCountries *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLCountriesByOneCategoryAndAllCategories():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolCountries();
		deleteMyList = true;
	}

	if (myList)
	{
		myList->findAllCountriesLinkedToAllCategories(dbAccess);
		if (myList->getCategoryNum() > 0)
			myList->findAllCountriesLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}
#ifndef MAKE_MUTUALLY_EXCLUSIVE
	myList->getMyList().unique();
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   After calling unique() on the list, myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}
#endif

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLCountryRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLCountry, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteCountriesByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLCountry", myDebug + "Deleted myList!");
	}
}


void COOLManager::handleReadCOOLCountriesByNotAllCategories(AllCoolCountries *myList)
{
//	This method will read all Countries, then eliminate the Countries that have the allCategories flag set.
	bool traceOn = Debug::isTracing("COOLManager.COOLCountry");
	HobString myDebug;
	if (traceOn)
		myDebug = "handleReadCOOLCountriesByNotAllCategories():  ";

	if (!dbAccess)
		return;

	AllCoolCountries *allCategoriesList = new AllCoolCountries();
	if (!allCategoriesList)
		return;

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolCountries();
		deleteMyList = true;
	}
	if (!myList)
	{
		delete allCategoriesList;
		return;
	}

	myList->findAllCountries(dbAccess);
	allCategoriesList->findAllCountriesLinkedToAllCategories(dbAccess);

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		myDebug2.append("   allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
		Debug::trace("COOLManager.COOLCountry", myDebug2);
	}

	if (allCategoriesList->size())
	{
		//	Go through the lists and remove any countries from myList that are in allCategoriesList.
		HobString country;
		int returnVal;
		HobList_type tempList = myList->getMyList();

		returnVal = allCategoriesList->first(country);
		while (returnVal != -1)
		{
			tempList.remove(country);
			returnVal = allCategoriesList->next(country);
		}
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("After the removal of allCategories countries,  allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
			Debug::trace("COOLManager.COOLCountry", myDebug2);
		}
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLCountryRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLCountry, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteCountriesByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLCountry", myDebug + "Deleted myList!");
	}
	delete allCategoriesList;
}
