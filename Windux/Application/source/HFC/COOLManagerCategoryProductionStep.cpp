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
#include "COOLCategoryProdStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLCategoryCountry.h"
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
#include "COOLCategoryProdStepManager.h"

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


using namespace util_string;
//using namespace edb;

void COOLManager::handleCOOLCategoryProductionStepMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLCategoryProductionStepMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevRestoreCategoryProductionStep:
			handleRestoreCOOLCategoryProductionStep(msg);
			break;

		case COOLDevDeleteCategoryProductionStep:
		case COOLDevBatchApplyDeleteCategoryProductionStep:
			handleDeleteCOOLCategoryProductionStep(msg);
			break;

		case COOLDevDeleteAllCategoryProductionSteps:
			handleDeleteAllCOOLCategoryProductionSteps();
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

COOLCategoryProdStep *COOLManager::firstCOOLCategoryProductionStep()
{
	if (!dbAccess)
		return (COOLCategoryProdStep *)NULL;

	DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProductionStepsRM)
		return (COOLCategoryProdStep *)NULL;

	// If the user calls firstCOOLCategoryProductionStep() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLCategoryProductionStepsQueryAndIter();

	allCoolCategoryProductionStepsQuery = dbCoolCategoryProductionStepsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductionSteps, just return false.
	if (!allCoolCategoryProductionStepsQuery)
		return (COOLCategoryProdStep *)NULL;

	DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = allCoolCategoryProductionStepsQuery->begin();
	if (tempIter == allCoolCategoryProductionStepsQuery->end())
		return (COOLCategoryProdStep *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLCategoryProductionStep()
	 * method call below).
	 */
	allCoolCategoryProductionStepsIter = new DBQuery<DBCoolCategoryProductionStep>::const_iterator(tempIter);
	DBCoolCategoryProductionStep *c = new DBCoolCategoryProductionStep(*(*allCoolCategoryProductionStepsIter));
	COOLCategoryProdStep *newProductionStep = new COOLCategoryProdStep(c, dbAccess);
	return newProductionStep;
}

COOLCategoryProdStep *COOLManager::nextCOOLCategoryProductionStep()
{
	if (!dbAccess)
		return (COOLCategoryProdStep *)NULL;

	if (allCoolCategoryProductionStepsQuery && !allCoolCategoryProductionStepsQuery->isEnabled())
		destroyCOOLCategoryProductionStepsQueryAndIter();

	if (!allCoolCategoryProductionStepsQuery || !allCoolCategoryProductionStepsIter)
		return firstCOOLCategoryProductionStep();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolCategoryProductionStepsIter);
	if (*allCoolCategoryProductionStepsIter == allCoolCategoryProductionStepsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductionSteps in the db. */
		destroyCOOLCategoryProductionStepsQueryAndIter();
		return (COOLCategoryProdStep *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolCategoryProductionStep copy constructor in order for the Hfc COOLCategoryProdStep to
	 * control the destruction of the DBCoolCategoryProductionStep record that the iterator contains; otherwise, the DBCoolCategoryProductionStep will
	 * be deleted when the iterator goes out of scope.  The DBCoolCategoryProductionStep copy will be deleted by the COOLCategoryProdStep record
	 * created below.
	 */
	DBCoolCategoryProductionStep *c = new DBCoolCategoryProductionStep(*(*allCoolCategoryProductionStepsIter));
	COOLCategoryProdStep *newProductionStep = new COOLCategoryProdStep(c, dbAccess);
	return newProductionStep;
}


int COOLManager::countAllCOOLCategoryProductionSteps()
{
	if (!dbAccess)
		return -1;

	DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProductionStepsRM)
		return -1;

	return dbCoolCategoryProductionStepsRM->countAll();
}


void COOLManager::handleDeleteCOOLCategoryProductionStep(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryProdStep *msgCOOLCategoryProductionStep = dynamic_cast<COOLCategoryProdStep *> (msg->getHfcObject());
	if (!msgCOOLCategoryProductionStep || !dbAccess)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLCategoryProductionStep");
	HobString myDebug("handleDeleteCOOLCategoryProductionStep():  ");

	DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();

	DataManager *myManager = mgrContainer_->getCOOLCategoryProdStepManager();
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobRecordNum deptNum = msgCOOLCategoryProductionStep->getDeptNum();
		COOLProductionStep *coolProductionStep = COOLProductionStep::findCOOLProductionStep(msgCOOLCategoryProductionStep->getProductionStep(), longtostring(deptNum), dbAccess);
		if (!coolProductionStep)
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("Error!  Cannot find COOLProductionStep ").append(msgCOOLCategoryProductionStep->getProductionStep());
				Debug::trace("COOLManager.COOLCategoryProductionStep", myDebug2);
			}
		}
		else
		{
			DBCoolCategoryProductionStep *dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->find(longtostring(msgCOOLCategoryProductionStep->getCategoryNum()), longtostring(msgCOOLCategoryProductionStep->getDeptNum()), coolProductionStep->getMyDbRecord());
			if (!dbCoolCategoryProductionStep)
				Debug::trace("COOLManager.COOLCategoryProductionStep", "This DBCoolCategoryProductionStep does not exist.");
			else
			{
				COOLCategoryProdStep existingCOOLCategoryProductionStep(dbCoolCategoryProductionStep, dbAccess);

				if (msg->getEventType() != COOLDevBatchApplyDeleteCategoryProductionStep)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, &existingCOOLCategoryProductionStep, myManager->getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
					}
				}

				existingCOOLCategoryProductionStep.deleteRecord();

				Debug::trace("COOLManager.COOLCategoryProductionStep", "Deleted the record.");
			}
			delete coolProductionStep;
		}
	}
}


void COOLManager::handleDeleteAllCOOLCategoryProductionSteps(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLCategoryProductionSteps - deleting all COOL CategoryProductionSteps");

	DataManager *myManager = mgrContainer_->getCOOLCategoryProdStepManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
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

		DBCoolCategoryProductionStepsRM *dbRM = dbAccess->getDBCoolCategoryProductionStepsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyCOOLProductionStepsQueryAndIter();
	}
}


void COOLManager::handleSaveRecordCOOLCategoryProductionStep(COOLCategoryProdStep *recordToSave)
{
	Debug::trace("COOLManager.COOLCategoryProductionStep", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLCategoryProdStepManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
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


void COOLManager::handleRestoreCOOLCategoryProductionStep(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLCategoryProdStep *restoreCategoryProdStep = dynamic_cast<COOLCategoryProdStep *> (msg->getHfcObject());
	if (!restoreCategoryProdStep || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCategoryProdStepManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreCategoryProdStep, myManager->getWriteEvent());

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
			HobString myDebug("handleRestoreCOOLCategoryProductionStep, show COOLCategoryProdStep:");
			Debug::trace("COOLManager", myDebug);
			restoreCategoryProdStep->show();
		}
		
		HobString stepName = restoreCategoryProdStep->getProductionStep();
		HobRecordNum deptNum = restoreCategoryProdStep->getDeptNum();

		stepName.stripChar(_LF_);
		stepName.stripChar(_CR_);

		// If we receive the category number with a Cool Pretext record,
		// check if that category exists (if 0 then the Pretext belongs to 'All Categories'),
		// create it if it doesn't exist

		COOLCategory *myCoolCategory = 0;
		HobRecordNum categoryNum = restoreCategoryProdStep->getCategoryNum();
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
						Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryProductionStep() myCoolCategory::saveRecord() ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
				}
			}
			if (!myCoolCategory)
				return;
		}

		COOLProductionStep *myCoolProdStep = COOLProductionStep::findCOOLProductionStep(stepName, longtostring(deptNum), dbAccess);
		if (!myCoolProdStep)
		{
			//	We need to create and save the ProductionStep record to the database - even though we are only saving the name1 element.
			myCoolProdStep = new COOLProductionStep();
			if (myCoolProdStep)
			{
				myCoolProdStep->setDBAccess(dbAccess);
				myCoolProdStep->setDeptNum(deptNum);
				myCoolProdStep->setName1(stepName);
				myCoolProdStep->setAllCategoriesFlag((categoryNum == 0) ? true : false);
				myCoolProdStep->setErrorCode(RECORD_OK);
				try
				{
					myCoolProdStep->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryProductionStep() myCoolProdStep::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}
		if (!myCoolProdStep)
		{
			if (myCoolCategory)
				delete myCoolCategory;
			return;
		}
		else
		{
			// Check if this production step is supposed to be assigned to the ANY category, but it currently isn't.
			if (categoryNum == 0 && !myCoolProdStep->isAllCategoriesFlag())
			{
				myCoolProdStep->setAllCategoriesFlag(true);
				try
				{
					myCoolProdStep->saveRecord();
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "COOLManager::handleRestoreCOOLCategoryProductionStep() myCoolProdStep::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
			}
		}

		if (categoryNum != 0)
		{
			restoreCategoryProdStep->setDBAccess(dbAccess);

			COOLCategoryProdStep *usedCategoryProdStep = 0;
			COOLCategoryProdStep *existingCategoryProdStep = COOLCategoryProdStep::findCOOLCategoryProductionStep(restoreCategoryProdStep->getCategoryNum(), restoreCategoryProdStep->getDeptNum(), myCoolProdStep->getMyDbRecord(), dbAccess);
			if (existingCategoryProdStep)
			{
				if (existingCategoryProdStep->isRecordValid())
				{
					existingCategoryProdStep->update(restoreCategoryProdStep);
					existingCategoryProdStep->saveRecord();
					usedCategoryProdStep = existingCategoryProdStep;								
				}
				else
				{
					existingCategoryProdStep->deleteRecord();	   
					restoreCategoryProdStep->saveRecord();
					usedCategoryProdStep = restoreCategoryProdStep;								
				}
			}
			else
			{
				restoreCategoryProdStep->saveRecord();
				usedCategoryProdStep = restoreCategoryProdStep;								
			}

			if (usedCategoryProdStep && usedCategoryProdStep->hasErrors() &&
				 mySession && mySession->isCommSession())
			{
				SessionMessage errMsg(Error, DtWrite);
				errMsg.setFormatType(EvolutionTaggedFormat);
				HfcError *err = usedCategoryProdStep->firstError();
				while (err)
				{
					errMsg.setHfcObject(err);
					mySession->send(&errMsg, Messenger::DEST_UI);

					err = usedCategoryProdStep->nextError();
				}
			}
			
			if (existingCategoryProdStep)
				delete existingCategoryProdStep;
		}

		if (myCoolCategory)
			delete myCoolCategory;
		if (myCoolProdStep)
			delete myCoolProdStep;
	}
}
