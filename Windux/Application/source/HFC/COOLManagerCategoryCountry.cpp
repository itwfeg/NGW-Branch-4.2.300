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
#include "COOLCategoryProdStep.h"
#include "COOLCategoryPretext.h"

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
#include "COOLCategoryCountryManager.h"

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


using namespace util_string;


void COOLManager::handleCOOLCategoryCountryMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLCategoryCountryMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevRestoreCategoryCountry:
			handleRestoreCOOLCategoryCountry(msg);
			break;

		case COOLDevDeleteCategoryCountry:
		case COOLDevBatchApplyDeleteCategoryCountry:
			handleDeleteCOOLCategoryCountry(msg);
			break;

		case COOLDevDeleteAllCategoryCountries:
			handleDeleteAllCOOLCategoryCountries();
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

COOLCategoryCountry *COOLManager::firstCOOLCategoryCountry()
{
	if (!dbAccess)
		return (COOLCategoryCountry *)NULL;

	DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
		return (COOLCategoryCountry *)NULL;

	// If the user calls firstCOOLCategoryCountry() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLCategoryCountriesQueryAndIter();

	allCoolCategoryCountriesQuery = dbCoolCategoryCountriesRM->retrieveAll();

	// Couldn't get the query from which to access the Countries, just return false.
	if (!allCoolCategoryCountriesQuery)
		return (COOLCategoryCountry *)NULL;

	DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = allCoolCategoryCountriesQuery->begin();
	if (tempIter == allCoolCategoryCountriesQuery->end())
		return (COOLCategoryCountry *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLCategoryCountry()
	 * method call below).
	 */
	allCoolCategoryCountriesIter = new DBQuery<DBCoolCategoryCountry>::const_iterator(tempIter);
	DBCoolCategoryCountry *c = new DBCoolCategoryCountry(*(*allCoolCategoryCountriesIter));
	COOLCategoryCountry *newCatCountry = new COOLCategoryCountry(c, dbAccess);
	return newCatCountry;
}

COOLCategoryCountry *COOLManager::nextCOOLCategoryCountry()
{
	if (!dbAccess)
		return (COOLCategoryCountry *)NULL;

	if (allCoolCategoryCountriesQuery && !allCoolCategoryCountriesQuery->isEnabled())
		destroyCOOLCategoryCountriesQueryAndIter();

	if (!allCoolCategoryCountriesQuery || !allCoolCategoryCountriesIter)
		return firstCOOLCategoryCountry();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolCategoryCountriesIter);
	if (*allCoolCategoryCountriesIter == allCoolCategoryCountriesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all CategoryCountries in the db. */
		destroyCOOLCategoryCountriesQueryAndIter();
		return (COOLCategoryCountry *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolCategoryCountry copy constructor in order for the Hfc COOLCategoryCountry to
	 * control the destruction of the DBCoolCategoryCountry record that the iterator contains; otherwise, the DBCoolCategoryCountry will
	 * be deleted when the iterator goes out of scope.  The DBCoolCategoryCountry copy will be deleted by the COOLCategoryCountry record
	 * created below.
	 */
	DBCoolCategoryCountry *c = new DBCoolCategoryCountry(*(*allCoolCategoryCountriesIter));
	COOLCategoryCountry *newCatCountry = new COOLCategoryCountry(c, dbAccess);
	return newCatCountry;
}


int COOLManager::countAllCOOLCategoryCountries()
{
	if (!dbAccess)
		return -1;

	DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
		return -1;

	return dbCoolCategoryCountriesRM->countAll();
}


void COOLManager::handleDeleteCOOLCategoryCountry(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryCountry *msgCOOLCategoryCountry = dynamic_cast<COOLCategoryCountry *> (msg->getHfcObject());
	if (!msgCOOLCategoryCountry || !dbAccess)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLCategoryCountry");
	HobString myDebug("handleDeleteCOOLCategoryCountry():  ");

	DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();

	DataManager *myManager = mgrContainer_->getCOOLCategoryCountryManager();
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobRecordNum deptNum = msgCOOLCategoryCountry->getDeptNum();
		COOLCountry *coolCountry = COOLCountry::findCOOLCountry(msgCOOLCategoryCountry->getCountry(), deptNum, dbAccess);
		
		if (!coolCountry)
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Error!  Cannot find COOLCountry ").append(msgCOOLCategoryCountry->getCountry());
				Debug::trace("COOLManager.COOLCategoryCountry", myDebug2);
			}
		}
		else
		{
			DBCoolCategoryCountry *dbCoolCategoryCountry = dbCoolCategoryCountriesRM->find(Long::toString(msgCOOLCategoryCountry->getCategoryNum()), coolCountry->getMyDbRecord());
			if (!dbCoolCategoryCountry)
				Debug::trace("COOLManager.COOLCategoryCountry", "This DBCoolCategoryCountry does not exist.");
			else
			{
				COOLCategoryCountry existingCOOLCategoryCountry(dbCoolCategoryCountry, dbAccess);

				if (msg->getEventType() != COOLDevBatchApplyDeleteCategoryCountry)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, &existingCOOLCategoryCountry, myManager->getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
					}
				}

				existingCOOLCategoryCountry.deleteRecord();

				Debug::trace("COOLManager.COOLCategoryCountry", "Deleted the record.");
			}
			delete coolCountry;
		}
	}
}


void COOLManager::handleDeleteAllCOOLCategoryCountries(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLCategoryCountries - deleting all COOL CategoryCountries");

	DataManager *myManager = mgrContainer_->getCOOLCategoryCountryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
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

		DBCoolCategoryCountriesRM *dbRM = dbAccess->getDBCoolCategoryCountriesRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyCOOLCountriesQueryAndIter();
	}
}


void COOLManager::handleSaveRecordCOOLCategoryCountry(COOLCategoryCountry *recordToSave)
{
	Debug::trace("COOLManager.COOLCategoryCountry", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLCategoryCountryManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
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


void COOLManager::handleRestoreCOOLCategoryCountry(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryCountry *restoreCategoryCountry = dynamic_cast<COOLCategoryCountry *> (msg->getHfcObject());
	if (!restoreCategoryCountry || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCategoryCountryManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryCountries))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreCategoryCountry, myManager->getWriteEvent());

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
		if (Debug::isTracing("COOLManager"))
		{
			HobString myDebug("handleRestoreCOOLCategoryCountry, show COOLCategoryCountry:");
			Debug::trace("COOLManager", myDebug);
			restoreCategoryCountry->show();
		}
		
		HobString countryName = restoreCategoryCountry->getCountry();
		HobRecordNum deptNum = restoreCategoryCountry->getDeptNum();
		
		countryName.stripChar(_LF_);
		countryName.stripChar(_CR_);
		
		// If we receive the category number with a Cool Pretext record,
		// check if that category exists (if 0 then the Pretext belongs to 'All Categories'),
		// create it if it doesn't exist

		COOLCategory *myCoolCategory = 0;
		HobRecordNum categoryNum = restoreCategoryCountry->getCategoryNum();
		if (categoryNum != 0)
		{
			myCoolCategory = COOLCategory::findCOOLCategory(categoryNum, deptNum, dbAccess);

			if (!myCoolCategory)
			{
				myCoolCategory = new COOLCategory();
				if (myCoolCategory)
				{
					myCoolCategory->setDBAccess(dbAccess);
					myCoolCategory->setDeptNum(deptNum);
					myCoolCategory->setRecordNum(categoryNum);
					myCoolCategory->setName1(COOLCategory::autoGeneratedName + HobString(Long::toString(categoryNum)));
					myCoolCategory->setErrorCode(RECORD_OK);
					try
					{
						myCoolCategory->saveRecord();
					}
					catch (DBException &dbe)
					{
						Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryCountry() myCoolCategory::saveRecord() ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
				}
			}
			if (!myCoolCategory)
				return;
		}

		COOLCountry *myCoolCountry = COOLCountry::findCOOLCountry(countryName, deptNum, dbAccess);
		if (!myCoolCountry)
		{
			//	We need to create and save the Country record to the database - even though we are only saving the name1 element.
			myCoolCountry = new COOLCountry();
			if (myCoolCountry)
			{
				myCoolCountry->setDBAccess(dbAccess);
				myCoolCountry->setDeptNum(deptNum);
				myCoolCountry->setName1(countryName);
				myCoolCountry->setAllCategoriesFlag((categoryNum == 0) ? true : false);
				myCoolCountry->setErrorCode(RECORD_OK);
				try
				{
					myCoolCountry->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryCountry() myCoolCountry::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}
		if (!myCoolCountry)
		{
			if (myCoolCategory)
				delete myCoolCategory;
			return;
		}
		else
		{
			// Check if this country is supposed to be assigned to the ANY category, but it currently isn't.
			if (categoryNum == 0 && !myCoolCountry->isAllCategoriesFlag())
			{
				myCoolCountry->setAllCategoriesFlag(true);
				try
				{
					myCoolCountry->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryCountry() myCoolCountry::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}

		if (categoryNum != 0)
		{
			restoreCategoryCountry->setDBAccess(dbAccess);

			COOLCategoryCountry *usedCategoryCountry = 0;
			COOLCategoryCountry *existingCategoryCountry = COOLCategoryCountry::findCOOLCategoryCountry(restoreCategoryCountry->getCategoryNum(), myCoolCountry->getMyDbRecord(), restoreCategoryCountry->getDeptNum(), dbAccess);
			if (existingCategoryCountry)
			{
				if (existingCategoryCountry->isRecordValid())
				{
					existingCategoryCountry->update(restoreCategoryCountry);
					existingCategoryCountry->saveRecord();
					usedCategoryCountry = existingCategoryCountry;								
				}
				else
				{
					existingCategoryCountry->deleteRecord();	   
					restoreCategoryCountry->saveRecord();
					usedCategoryCountry = restoreCategoryCountry;								
				}
			}
			else
			{
				restoreCategoryCountry->saveRecord();
				usedCategoryCountry = restoreCategoryCountry;								
			}

			if (usedCategoryCountry && usedCategoryCountry->hasErrors() &&
				 mySession && mySession->isCommSession())
			{
				SessionMessage errMsg(Error, DtWrite);
				errMsg.setFormatType(EvolutionTaggedFormat);
				HfcError *err = usedCategoryCountry->firstError();
				while (err)
				{
					errMsg.setHfcObject(err);
					mySession->send(&errMsg, Messenger::DEST_UI);

					err = usedCategoryCountry->nextError();
				}
			}

			if (existingCategoryCountry)
				delete existingCategoryCountry;
		}
			
		if (myCoolCategory)
			delete myCoolCategory;
		if (myCoolCountry)
			delete myCoolCountry;
	}
}
