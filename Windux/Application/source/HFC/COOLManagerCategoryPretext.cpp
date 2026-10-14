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
#include "COOLCategoryPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLCategoryCountry.h"
#include "COOLCategoryProdStep.h"
#include "COOLCategoryPretext.h"
#include "COOLPretext.h"

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
#include "COOLCategoryPretextManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLCategoryPretextMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLCategoryPretextMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevRestoreCategoryPretext:
			handleRestoreCOOLCategoryPretext(msg);
			break;

		case COOLDevDeleteCategoryPretext:
		case COOLDevBatchApplyDeleteCategoryPretext:
			handleDeleteCOOLCategoryPretext(msg);
			break;

		case COOLDevDeleteAllCategoryPretexts:
			handleDeleteAllCOOLCategoryPretexts();
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

COOLCategoryPretext *COOLManager::firstCOOLCategoryPretext()
{
	if (!dbAccess)
		return (COOLCategoryPretext *)NULL;

	DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolCategoryPretextsRM)
		return (COOLCategoryPretext *)NULL;

	// If the user calls firstCOOLCategoryPretext() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLCategoryPretextsQueryAndIter();

	allCoolCategoryPretextsQuery = dbCoolCategoryPretextsRM->retrieveAll();

	// Couldn't get the query from which to access the Pretexts, just return false.
	if (!allCoolCategoryPretextsQuery)
		return (COOLCategoryPretext *)NULL;

	DBQuery<DBCoolCategoryPreText>::const_iterator tempIter = allCoolCategoryPretextsQuery->begin();
	if (tempIter == allCoolCategoryPretextsQuery->end())
		return (COOLCategoryPretext *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLCategoryPretext()
	 * method call below).
	 */
	allCoolCategoryPretextsIter = new DBQuery<DBCoolCategoryPreText>::const_iterator(tempIter);
	DBCoolCategoryPreText *c = new DBCoolCategoryPreText(*(*allCoolCategoryPretextsIter));
	COOLCategoryPretext *newCatPretext = new COOLCategoryPretext(c, dbAccess);
	return newCatPretext;
}

COOLCategoryPretext *COOLManager::nextCOOLCategoryPretext()
{
	if (!dbAccess)
		return (COOLCategoryPretext *)NULL;

	if (allCoolCategoryPretextsQuery && !allCoolCategoryPretextsQuery->isEnabled())
		destroyCOOLCategoryPretextsQueryAndIter();

	if (!allCoolCategoryPretextsQuery || !allCoolCategoryPretextsIter)
		return firstCOOLCategoryPretext();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolCategoryPretextsIter);
	if (*allCoolCategoryPretextsIter == allCoolCategoryPretextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all COOLPretexts in the db. */
		destroyCOOLCategoryPretextsQueryAndIter();
		return (COOLCategoryPretext *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolCategoryPreText copy constructor in order for the Hfc COOLCategoryPretext to
	 * control the destruction of the DBCoolCategoryPreText record that the iterator contains; otherwise, the DBCoolCategoryPreText will
	 * be deleted when the iterator goes out of scope.  The DBCoolCategoryPreText copy will be deleted by the COOLCategoryPretext record
	 * created below.
	 */
	DBCoolCategoryPreText *c = new DBCoolCategoryPreText(*(*allCoolCategoryPretextsIter));
	COOLCategoryPretext *newCatPretext = new COOLCategoryPretext(c, dbAccess);
	return newCatPretext;
}


int COOLManager::countAllCOOLCategoryPretexts()
{
	if (!dbAccess)
		return -1;

	DBCoolCategoryPreTextsRM *dbCoolPreTextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
	if (!dbCoolPreTextsRM)
		return -1;

	return dbCoolPreTextsRM->countAll();
}


void COOLManager::handleDeleteCOOLCategoryPretext(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryPretext *msgCOOLCategoryPretext = dynamic_cast<COOLCategoryPretext *> (msg->getHfcObject());
	if (!msgCOOLCategoryPretext || !dbAccess)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLCategoryPretext");
	HobString myDebug("handleDeleteCOOLCategoryPretext():  ");

	DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();

	DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobRecordNum deptNum = msgCOOLCategoryPretext->getDeptNum();
		COOLPretext *coolPretext = COOLPretext::findCOOLPretext(msgCOOLCategoryPretext->getPretextNum(), deptNum, dbAccess);
		if (!coolPretext)
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Error!  Cannot find COOLPretext ").append(Long::toString(msgCOOLCategoryPretext->getPretextNum()));
				Debug::trace("COOLManager.COOLCategoryPretext", myDebug2);
			}
		}
		else
		{
			DBCoolCategoryPreText *dbCoolCategoryPreText = dbCoolCategoryPretextsRM->find(longtostring(msgCOOLCategoryPretext->getCategoryNum()), longtostring(msgCOOLCategoryPretext->getPretextNum()), longtostring(msgCOOLCategoryPretext->getDeptNum())  );
			if (!dbCoolCategoryPreText)
				Debug::trace("COOLManager.COOLCategoryPretext", "This DBCoolCategoryPreText does not exist.");
			else
			{
				COOLCategoryPretext existingCOOLCategoryPretext(dbCoolCategoryPreText, dbAccess);

				if (msg->getEventType() != COOLDevBatchApplyDeleteCategoryPretext)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, &existingCOOLCategoryPretext, myManager->getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
					}
				}

				existingCOOLCategoryPretext.deleteRecord();

				Debug::trace("COOLManager.COOLCategoryPretext", "Deleted the record.");
			}
			delete coolPretext;
		}
	}
}


void COOLManager::handleDeleteAllCOOLCategoryPretexts(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLCategoryPretexts - deleting all COOL CategoryPretexts");

	DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
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

		DBCoolCategoryPreTextsRM *dbRM = dbAccess->getDBCoolCategoryPreTextsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyCOOLPretextsQueryAndIter();
	}
}


void COOLManager::handleSaveRecordCOOLCategoryPretext(COOLCategoryPretext *recordToSave)
{
	Debug::trace("COOLManager.COOLCategoryPretext", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
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


void COOLManager::handleRestoreCOOLCategoryPretext(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryPretext *restoreCategoryPretext = dynamic_cast<COOLCategoryPretext *> (msg->getHfcObject());
	if (!restoreCategoryPretext || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreCategoryPretext, myManager->getWriteEvent());

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
			HobString myDebug("handleRestoreCOOLCategoryPretext, show COOLCategoryPretext:");
			Debug::trace("COOLManager", myDebug);
			restoreCategoryPretext->show();
		}

		// If we receive the category number with a Cool Pretext record,
		// check if that category exists (if 0 then the Pretext belongs to 'All Categories'),
		// create it if it doesn't exist

		COOLCategory *myCoolCategory = 0;
		HobRecordNum categoryNum = restoreCategoryPretext->getCategoryNum();
		HobRecordNum deptNum = restoreCategoryPretext->getDeptNum();
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
						Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryPretext() myCoolCategory::saveRecord() ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
				}
			}
			if (!myCoolCategory)
				return;
		}

		COOLPretext *myCoolPretext = COOLPretext::findCOOLPretext(restoreCategoryPretext->getPretextNum(), deptNum, dbAccess);
		if (!myCoolPretext)
		{
			//	We need to create and save the Pretext record to the database - even though we are only saving the pretext number element.
			COOLPretext *myCoolPretext = new COOLPretext();
			if (myCoolPretext)
			{
				myCoolPretext->setDBAccess(dbAccess);
				myCoolPretext->setDeptNum(deptNum);
				myCoolPretext->setRecordNum(restoreCategoryPretext->getPretextNum());
				myCoolPretext->setAllCategoriesFlag((categoryNum == 0) ? true : false);
				myCoolPretext->setErrorCode(RECORD_OK);
				try
				{
					myCoolPretext->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryPretext() myCoolPretext::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}
		if (!myCoolPretext)
		{
			if (myCoolCategory)
				delete myCoolCategory;
			return;
		}
		else
		{
			// Check if this pretext is supposed to be assigned to the ANY category, but it currently isn't.
			if (categoryNum == 0 && !myCoolPretext->isAllCategoriesFlag())
			{
				myCoolPretext->setAllCategoriesFlag(true);
				try
				{
					myCoolPretext->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryPretext() myCoolPretext::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}

		if (categoryNum != 0)
		{
			restoreCategoryPretext->setDBAccess(dbAccess);

			COOLCategoryPretext *usedCategoryPretext = 0;
			COOLCategoryPretext *existingCategoryPretext = COOLCategoryPretext::findCOOLCategoryPretext(restoreCategoryPretext->getCategoryNum(), restoreCategoryPretext->getPretextNum(), restoreCategoryPretext->getDeptNum(), dbAccess);
			if (existingCategoryPretext)
			{
				if (existingCategoryPretext->isRecordValid())
				{
					existingCategoryPretext->update(restoreCategoryPretext);
					existingCategoryPretext->saveRecord();
					usedCategoryPretext = existingCategoryPretext;								
				}
				else
				{
					existingCategoryPretext->deleteRecord();	   
					restoreCategoryPretext->saveRecord();
					usedCategoryPretext = restoreCategoryPretext;								
				}
			}
			else
			{
				restoreCategoryPretext->saveRecord();
				usedCategoryPretext = restoreCategoryPretext;								
			}

			if (usedCategoryPretext && usedCategoryPretext->hasErrors() &&
				 mySession && mySession->isCommSession())
			{
				SessionMessage errMsg(Error, DtWrite);
				errMsg.setFormatType(EvolutionTaggedFormat);
				HfcError *err = usedCategoryPretext->firstError();
				while (err)
				{
					errMsg.setHfcObject(err);
					mySession->send(&errMsg, Messenger::DEST_UI);

					err = usedCategoryPretext->nextError();
				}
			}
			
			if (existingCategoryPretext)
				delete existingCategoryPretext;
		}

		if (myCoolCategory)
			delete myCoolCategory;
		if (myCoolPretext)
			delete myCoolPretext;
	}
}
