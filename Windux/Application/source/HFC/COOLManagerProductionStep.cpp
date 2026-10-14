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
#include "COOLProductionStepManager.h"
#include "COOLCategoryProdStepManager.h"
#include "COOLCategoryProdStep.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCoolProductionStepMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCoolProductionStepMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
/*      **********  ProductionSteps  **********     */

		case COOLDevReadProductionStep:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleReadCOOLProductionStep(msgProductionStep);
			break;
		}
		case COOLDevReadAllProductionSteps:
		{
			AllCoolProductionSteps *msgAll = dynamic_cast<AllCoolProductionSteps *> (msg->getHfcObject());
			handleReadAllCOOLProductionSteps(msgAll);
			break;
		}
		case COOLDevWriteProductionStep:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleWriteCOOLProductionStep(msgProductionStep);
			break;
		}
		case COOLDevDeleteProductionStep:
		case COOLDevProductionStepBatchApplyDelete:
			handleDeleteCOOLProductionStep(msg);
			break;
		case COOLDevDeleteAllProductionSteps:
			handleDeleteAllCOOLProductionSteps();
			break;
		case COOLDevSaveProductionStep:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleUpdateAndSaveCOOLProductionStep(msgProductionStep);
			break;
		}
		case COOLDevRestoreProductionStep:
			handleRestoreCOOLProductionStep(msg);
			break;
		case COOLDevCreateNewProductionStep:
			handleCreateCOOLProductionStep();
			break;
		case COOLDevReadByCategoryOnlyProductionSteps:
		{
			AllCoolProductionSteps *msgAll = dynamic_cast<AllCoolProductionSteps *> (msg->getHfcObject());
			handleReadCOOLProductionStepsByOneCategory(msgAll);
			break;
		}
		case COOLDevAddProductionStepToCategory:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleAddCOOLProductionStepToCategory(msgProductionStep);
			break;
		}
		case COOLDevDeleteProductionStepFromCategory:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleDeleteCOOLProductionStepFromCategory(msgProductionStep);
			break;
		}
		case COOLDevCreateNewProductionStepAndSave:
		{
			COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
			handleCreateCOOLProductionStepAndSave(msgProductionStep);
			break;
		}
		case COOLDevReadByAllAndByCategoryProductionSteps:
		{
			AllCoolProductionSteps *msgAll = dynamic_cast<AllCoolProductionSteps *> (msg->getHfcObject());
			handleReadCOOLProdStepsByOneCategoryAndAllCategories(msgAll);
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


COOLProductionStep *COOLManager::getCurrentProductionStep()
{
	return currentProductionStep;
}
void COOLManager::setCurrentProductionStep(COOLProductionStep *country)
{
	if (currentProductionStep)
		delete currentProductionStep;
	currentProductionStep = country;
}


COOLProductionStep *COOLManager::firstCOOLProductionStep()
{
	if (!dbAccess)
		return (COOLProductionStep *)NULL;

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return (COOLProductionStep *)NULL;

	// If the user calls firstCOOLProductionStep() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLProductionStepsQueryAndIter();

	// Only used by backuprestore, and backuprestore needs all from all depts, so we dont pass deptNum
	allCoolProductionStepsQuery = dbCoolProductionStepsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductionSteps, just return false.
	if (!allCoolProductionStepsQuery)
		return (COOLProductionStep *)NULL;

	DBQuery<DBCoolProductionStep>::const_iterator tempIter = allCoolProductionStepsQuery->begin();
	if (tempIter == allCoolProductionStepsQuery->end())
		return (COOLProductionStep *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLProductionStep()
	 * method call below).
	 */
	allCoolProductionStepsIter = new DBQuery<DBCoolProductionStep>::const_iterator(tempIter);
	DBCoolProductionStep *c = new DBCoolProductionStep(*(*allCoolProductionStepsIter));
	COOLProductionStep *newProductionStep = new COOLProductionStep(c, dbAccess);
	return newProductionStep;
}

COOLProductionStep *COOLManager::nextCOOLProductionStep()
{
	if (!dbAccess)
		return (COOLProductionStep *)NULL;

	if (allCoolProductionStepsQuery && !allCoolProductionStepsQuery->isEnabled())
		destroyCOOLProductionStepsQueryAndIter();

	if (!allCoolProductionStepsQuery || !allCoolProductionStepsIter)
		return firstCOOLProductionStep();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolProductionStepsIter);
	if (*allCoolProductionStepsIter == allCoolProductionStepsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductionSteps in the db. */
		destroyCOOLProductionStepsQueryAndIter();
		return (COOLProductionStep *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolProductionStep copy constructor in order for the Hfc COOLProductionStep to
	 * control the destruction of the DBCoolProductionStep record that the iterator contains; otherwise, the DBCoolProductionStep will
	 * be deleted when the iterator goes out of scope.  The DBCoolProductionStep copy will be deleted by the COOLProductionStep record
	 * created below.
	 */
	DBCoolProductionStep *c = new DBCoolProductionStep(*(*allCoolProductionStepsIter));
	COOLProductionStep *newProductionStep = new COOLProductionStep(c, dbAccess);
	return newProductionStep;
}


int COOLManager::countAllCOOLProductionSteps()
{
	if (!dbAccess)
		return -1;

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return -1;

	return dbCoolProductionStepsRM->countAll();
}


int COOLManager::countAllCOOLProductionStepsForAllCategories()
{
	if (!dbAccess)
		return -1;

	int totalCount = -1;

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (dbCoolProductionStepsRM)
	{
		DBQuery<DBCoolProductionStep> *myQuery = dbCoolProductionStepsRM->retrieveAll();
		if (myQuery)
		{
			DBQuery<DBCoolProductionStep>::const_iterator tempIter = myQuery->begin();
			totalCount = 0;
			while (tempIter != myQuery->end())
			{
				if (!tempIter->isAllCategoriesNull() && tempIter->getAllCategories())
					++totalCount;
				++tempIter;
			}
			delete myQuery;
			myQuery = 0;
		}
	}

	return totalCount;
}


void COOLManager::handleReadCOOLProductionStep(COOLProductionStep *msgProductionStep)
{
	if (!msgProductionStep || !dbAccess)
		return;

	if (msgProductionStep->getName1() == "")
	{
		setCurrentProductionStep((COOLProductionStep *) NULL);
		return;
	}
	
	COOLProductionStep cps;
	COOLProductionStep *readProductionStep = COOLProductionStep::findCOOLProductionStep(msgProductionStep->getName1(), longtostring(msgProductionStep->getDeptNum()), dbAccess);


	if (!readProductionStep)
	{
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
		{
			if (s->getCurrentMode() == OpModeSupervisor)
			{
				// Send error message to browser indicating Cool Production Step not found.
				mySession->addDialogMessage(ErrCOOLDev, COOLDevErrProductionStepNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

				if (readProductionStep)
					delete readProductionStep;
				return;
			}
		}
	}
	setCurrentProductionStep(readProductionStep);

	if (mySession)
	{
		// Send the COOL Production Step found back to the Browser.
		SessionMessage coolData(COOLDev, COOLDevWriteProductionStep);
		coolData.setFormatType(BrowserFormat);
		coolData.setHfcObject(currentProductionStep);
		mySession->send(&coolData, Messenger::DEST_UI);
	}
}

void COOLManager::handleReadAllCOOLProductionSteps(AllCoolProductionSteps * /* msgAll */)
{
#if 1
	if (!dbAccess)
		return;
#else
	if (!dbAccess || !msgAll)
		return;

	if (msgAll->doExcludeAllCategories())
		handleReadCOOLProdStepsByNotAllCategories(msgAll);
	else
	{
#endif
		AllCoolProductionSteps *myList = new AllCoolProductionSteps();
		if (!myList)
			return;

		myList->findAllProductionSteps(dbAccess);

		if (Debug::isTracing("COOLManager.COOLProductionStep"))
		{
			HobString myDebug("handling event: ");
			myDebug.append("handleCOOLProductionStepReadAll(): myList size = ").append(Integer::toString((int)myList->size()));
			Debug::trace("COOLManager.COOLProductionStep", myDebug);
		}

		if (mySession)
		{
			// Send the class found back to the Browser.
			// Is this the behavior for ALL COOLProductionStepsRead messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a COOLProductionStep, but doesn't want the data sent back?
			SessionMessage ctData(COOLDev, COOLDevWriteAllProductionSteps);
			ctData.setFormatType(BrowserFormat);
			ctData.setHfcObject(myList);
			mySession->send(&ctData, Messenger::DEST_UI);
		}
		delete myList;
#if 0
	}
#endif
}

void COOLManager::handleWriteCOOLProductionStep(COOLProductionStep *msgProductionStep)
{
	if ((!msgProductionStep) || (!currentProductionStep))
		return;

	if (Debug::isTracing("COOLManager.COOLProductionStep"))
	{
		HobString myDebug("handleCOOLProductionStepWrite(): currentProductionStep = ");
		myDebug.append(Hex::toString((long long) currentProductionStep)).append("  , msgProductionStep = ").append(Hex::toString((long long) msgProductionStep));
		Debug::trace("COOLManager.COOLProductionStep", myDebug);
	}
	currentProductionStep->update(msgProductionStep);
}

void COOLManager::handleDeleteCOOLProductionStep(SessionMessage *msg)
{
	if (!msg)
		return;

	if (!dbAccess)
		return;

	bool usingCurrent = false;
	COOLProductionStep *msgProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
	if (!msgProductionStep)
	{
		if (currentProductionStep)
			usingCurrent = true;
		else
			return;
	}

	DataManager *myManager = mgrContainer_->getCOOLProductionStepManager();
	if (!myManager)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolProductionSteps))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		HobString serString = "";
		if (usingCurrent)
			serializeForSynchronization(serString, currentProductionStep, myManager->getDeleteEvent());
		else
			serString = msg->getOriginalBuff();
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "handleCOOLProductionStepDelete(): Name 1: ";
			myDebug.append(msgProductionStep->getName1());
			Debug::trace("COOLManager.COOLProductionStep", myDebug);
		}

		COOLProductionStep *existingStep = (usingCurrent ? currentProductionStep : COOLProductionStep::findCOOLProductionStep(msgProductionStep->getName1(), longtostring(msgProductionStep->getDeptNum()), dbAccess));
		if (existingStep)
		{
			// Before deleting the CoolProductionStep record, delete all CoolCategoryProductionStep records
			// associated with this CoolProductionStep.
			DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
			if (dbCoolCategoryProductionStepsRM)
			{
				DBQuery<DBCoolCategoryProductionStep>* allCoolCategoryProductionStepsQuery = dbCoolCategoryProductionStepsRM->retrieveLinkedtoCoolProductionStep(existingStep->getMyDbRecord());
				if (allCoolCategoryProductionStepsQuery)
				{
					const DBCoolCategoryProductionStep *coolCategoryProductionStep = (DBCoolCategoryProductionStep *) NULL;
					DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = allCoolCategoryProductionStepsQuery->begin();
					while (tempIter != allCoolCategoryProductionStepsQuery->end())
					{
						coolCategoryProductionStep = new DBCoolCategoryProductionStep(*tempIter);
						++tempIter;
						if (coolCategoryProductionStep)
						{
							dbCoolCategoryProductionStepsRM->deleteRecord(coolCategoryProductionStep);
							delete coolCategoryProductionStep;
						}
					}
					delete allCoolCategoryProductionStepsQuery;
				}
			}

			existingStep->deleteRecord();
			if (traceOn)
			{
				myDebug.append(", has been deleted.");
				Debug::trace("COOLManager.COOLProductionStep", myDebug);
			}

			if (msg->getEventType() != COOLDevProductionStepBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolProductionSteps))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingStep, myManager->getDeleteEvent());

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
				Debug::trace("COOLManager.COOLProductionStep", myDebug);
			}
		}

		if (usingCurrent)
			setCurrentProductionStep((COOLProductionStep *) NULL);
		else
			delete existingStep;
	}
}

void COOLManager::handleDeleteAllCOOLProductionSteps(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLProductionSteps - deleting all COOL ProductionSteps");

	DataManager *myManager = mgrContainer_->getCOOLProductionStepManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolProductionSteps))
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
			DBCoolProductionStepsRM *dbRM = dbAccess->getDBCoolProductionStepsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		// When deleting all CoolProductionSteps, we also delete all CoolCategoryProductionStep bridge table records.
		// We don't delete the Cool Category records here.
		{
			DBCoolCategoryProductionStepsRM *dbRM = dbAccess->getDBCoolCategoryProductionStepsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		destroyCOOLProductionStepsQueryAndIter();
	}
}


void COOLManager::handleUpdateAndSaveCOOLProductionStep(COOLProductionStep *msgProductionStep)
{
	Debug::trace("COOLManager.COOLProductionStep", "handleUpdateAndSaveCOOLProductionStep(): currentProductionStep = " + ptrtostring(currentProductionStep));

	if (currentProductionStep)
	{
		if (msgProductionStep)
		  	currentProductionStep->update(msgProductionStep);
		handleSaveRecordCOOLProductionStep(currentProductionStep);
	}
}

void COOLManager::handleSaveRecordCOOLProductionStep(COOLProductionStep *recordToSave)
{
	Debug::trace("COOLManager.COOLProductionStep", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		DataManager *myManager = mgrContainer_->getCOOLProductionStepManager();
		if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolProductionSteps))
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


void COOLManager::handleRestoreCOOLProductionStep(SessionMessage *msg)
{
	if (!msg)
		return;

	COOLProductionStep *restoreProductionStep = dynamic_cast<COOLProductionStep *> (msg->getHfcObject());
	if (!restoreProductionStep || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLProductionStepManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolProductionSteps))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreProductionStep, myManager->getWriteEvent());

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
		restoreProductionStep->setDBAccess(dbAccess);

		COOLProductionStep *usedProductionStep = 0;
		COOLProductionStep *existingProductionStep = COOLProductionStep::findCOOLProductionStep(restoreProductionStep->getName1(), longtostring(restoreProductionStep->getDeptNum()), dbAccess);
		if (existingProductionStep)
		{
			if (existingProductionStep->isRecordValid())
			{
				existingProductionStep->update(restoreProductionStep);
				existingProductionStep->saveRecord();
				usedProductionStep = existingProductionStep;								
			}
			else
			{
				existingProductionStep->deleteRecord();	   
				restoreProductionStep->saveRecord();
				usedProductionStep = restoreProductionStep;								
			}
		}
		else
		{
			restoreProductionStep->saveRecord();
			usedProductionStep = restoreProductionStep;								
		}

		// Cool Production Step records received in Ultima format also contain the category number.
		// So check if that category exists (if 0 then the Step belongs to 'All Categories'),
		// create it if it doesn't exist, and create the record (colcass db record) that links
		// the Category with this Production Step record.
		// Note:  If a Production Step number (tag c3) is passed in with the Production Step and the Production Step
		// belongs to 'All Categories', the Production Step number will be lost/ignored, since we won't be
		// creating the bridge table record (in colcass) for Production Steps that belong to 'All Categories'.
		if (restoreProductionStep->isReceived(fieldOffsetCOOLProductionStepCategory) &&
			 restoreProductionStep->getCategoryNum() > 0)
		{
			HobRecordNum lDept = restoreProductionStep->getDeptNum();
			COOLCategory *coolCategory = COOLCategory::findCOOLCategory(restoreProductionStep->getCategoryNum(), lDept, dbAccess);
			if (coolCategory)
				delete coolCategory;
			else
			{
				// Have to create the Cool Category for this Production Step.  It doesn't yet exist.
				coolCategory = new COOLCategory();
				if (coolCategory)
				{
					coolCategory->setDeptNum(lDept);
					coolCategory->setRecordNum(restoreProductionStep->getCategoryNum());
					coolCategory->setName1(COOLCategory::autoGeneratedName + HobString(Long::toString(coolCategory->getRecordNum())));
					coolCategory->setErrorCode(RECORD_OK);
					coolCategory->setDBAccess(dbAccess);
					coolCategory->saveRecord();
					delete coolCategory;
				}
			}

			handleAddCOOLProductionStepToCategory(restoreProductionStep);
		}

		if (usedProductionStep && usedProductionStep->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedProductionStep->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedProductionStep->nextError();
			}
		}
		if (existingProductionStep)
			delete existingProductionStep;
	}
}

void COOLManager::handleCreateCOOLProductionStep()
{
	if (!dbAccess)
		return;

	//  Create a new ProductionStep from the database based on the category number from the message.
	COOLProductionStep *newProductionStep = new COOLProductionStep();
	if (newProductionStep)
	{
		Debug::trace("COOLManager.COOLProductionStep", "Creating a new COOLProductionStep with no name1");

		newProductionStep->setErrorCode(RECORD_OK);
		newProductionStep->setDBAccess(dbAccess);

		//  Set this new ProductionStep as the current ProductionStep.
		setCurrentProductionStep(newProductionStep);

		if (mySession)
		{
			// Send the new ProductionStep to the Browser.
			SessionMessage catData(COOLDev, COOLDevWriteProductionStep);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentProductionStep);
			mySession->send(&catData, Messenger::DEST_UI);
		}
	}
}

void COOLManager::handleReadCOOLProductionStepsByOneCategory(AllCoolProductionSteps *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLProductionStepsByOneCategory():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolProductionSteps();
		deleteMyList = true;
		myList->findAllProductionStepsLinkedToAllCategories(dbAccess);
	}
	else
	{
		myList->findAllProductionStepsLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLProductionStepsRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLProductionStep, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteProductionStepsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLProductionStep", myDebug + "Deleted myList!");
	}
}

void COOLManager::handleAddCOOLProductionStepToCategory(COOLProductionStep *msgProductionStep)
{
	if (!msgProductionStep || !dbAccess)
		return;

	DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProductionStepsRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
	HobString myDebug("handleCOOLProductionStepAddToCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgProductionStep = ").append(Hex::toString((long long) msgProductionStep));
		myDebug2.append("   ProductionStep = ").append(msgProductionStep->getName1());
		myDebug2.append("   CountryNum = ").append(Long::toString(msgProductionStep->getProductionStepNum()));
		myDebug2.append("   Category = ").append(Long::toString(msgProductionStep->getCategoryNum()));
		myDebug2.append("   allCategories = ").append(Bool::toString(msgProductionStep->isAllCategoriesFlag()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}

	COOLProductionStep *coolProductionStep = COOLProductionStep::findCOOLProductionStep(msgProductionStep->getName1(), longtostring(msgProductionStep->getDeptNum()), dbAccess);
	if (!coolProductionStep)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLProductionStep ").append(msgProductionStep->getName1());
			Debug::trace("COOLManager.COOLProductionStep", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryProductionStep *dbCoolCategoryProductionStep = 0;

		if (msgProductionStep->isAllCategoriesFlag())
		{
#ifdef MAKE_MUTUALLY_EXCLUSIVE
			DBQuery<DBCoolCategoryProductionStep>* coolCategoryProductionStepsQuery = dbCoolCategoryProductionStepsRM->retrieveLinkedtoCoolProductionStep(coolProductionStep->getMyDbRecord());
			if (coolCategoryProductionStepsQuery)
			{
				if (coolCategoryProductionStepsQuery->size() && traceOn)
					Debug::trace("COOLManager.COOLProductionStep", myDebug + "Deleting DBCoolCategoryProductionStep records because allCategoriesFlag is True.");

				DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = coolCategoryProductionStepsQuery->begin();
				while (tempIter != coolCategoryProductionStepsQuery->end())
				{
					dbCoolCategoryProductionStep = new DBCoolCategoryProductionStep(*tempIter);
					++tempIter;
					if (dbCoolCategoryProductionStep)
					{
						dbCoolCategoryProductionStepsRM->deleteRecord(dbCoolCategoryProductionStep);
						delete dbCoolCategoryProductionStep;
					}
				}
				delete coolCategoryProductionStepsQuery;
			}
#endif
		}
		else
		{
			dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->find(longtostring(msgProductionStep->getCategoryNum()), longtostring(msgProductionStep->getDeptNum()), coolProductionStep->getMyDbRecord());
			if (dbCoolCategoryProductionStep)
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLProductionStep", myDebug + "This DBCoolCategoryProductionStep record already exists.");
			}
			else
			{
				if (traceOn)
					Debug::trace("COOLManager.COOLProductionStep", myDebug + "Creating a new DBCoolCategoryProductionStep record.");

				dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->createRecord(Long::toString(msgProductionStep->getCategoryNum()), coolProductionStep->getMyDbRecord());
				if (dbCoolCategoryProductionStep)
				{
					// Remember that this newCOOLCategoryProductionStep takes ownership of the dbCoolCategoryProductionStep record, so don't delete it after it is saved.
					COOLCategoryProdStep newCOOLCategoryProductionStep(dbCoolCategoryProductionStep, dbAccess);
					if (traceOn)
						Debug::trace("COOLManager.COOLProductionStep", myDebug + "Saving the new DBCoolCategoryProductionStep record.");

					handleSaveRecordCOOLCategoryProductionStep(&newCOOLCategoryProductionStep);
				}
			}
		}
		if (coolProductionStep->isAllCategoriesFlag() != msgProductionStep->isAllCategoriesFlag())
		{
			if (traceOn)
				Debug::trace("COOLManager.COOLProductionStep", myDebug + "Updating the allCategoriesFlag in the coolProductionStep record.");
			coolProductionStep->setAllCategoriesFlag(msgProductionStep->isAllCategoriesFlag());
			handleSaveRecordCOOLProductionStep(coolProductionStep);
		}
		delete coolProductionStep;
	}
}

void COOLManager::handleDeleteCOOLProductionStepFromCategory(COOLProductionStep *msgProductionStep)
{
	if (!msgProductionStep || !dbAccess)
		return;

	DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
	if (!dbCoolCategoryProductionStepsRM)
		return;

	bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
	HobString myDebug("handleCOOLProductionStepDeleteFromCategory():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("msgProductionStep = ").append(Hex::toString((long long) msgProductionStep));
		myDebug2.append("   ProductionStep = ").append(msgProductionStep->getName1());
		myDebug2.append("   Category = ").append(Long::toString(msgProductionStep->getCategoryNum()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}

	COOLProductionStep *coolProductionStep = COOLProductionStep::findCOOLProductionStep(msgProductionStep->getName1(), longtostring(msgProductionStep->getDeptNum()), dbAccess);
	if (!coolProductionStep)
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("Error!  Cannot find COOLProductionStep ").append(msgProductionStep->getName1());
			Debug::trace("COOLManager.COOLProductionStep", myDebug2);
		}
	}
	else
	{
		DBCoolCategoryProductionStep *dbCoolCategoryProductionStep = dbCoolCategoryProductionStepsRM->find(longtostring(msgProductionStep->getCategoryNum()), longtostring(msgProductionStep->getDeptNum()), coolProductionStep->getMyDbRecord());
		if (!dbCoolCategoryProductionStep)
		{
			Debug::trace("COOLManager.COOLProductionStep", "This DBCoolCategoryProductionStep does not exist.");

			// We still want to remove this Production Step from being in the "Any" category.
			coolProductionStep->setAllCategoriesFlag(false);
			handleSaveRecordCOOLProductionStep(coolProductionStep);
		}
		else
		{
			COOLCategoryProdStep newCOOLCategoryProductionStep(dbCoolCategoryProductionStep, dbAccess);

			DataManager *myManager = mgrContainer_->getCOOLCategoryProdStepManager();
			if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryProductionSteps))
			{
				// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
				HobString serString = "";
				serializeForSynchronization(serString, &newCOOLCategoryProductionStep, myManager->getDeleteEvent());

				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

				if (proxy)
					proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
						hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), serString)));
			}

			newCOOLCategoryProductionStep.deleteRecord();

			Debug::trace("COOLManager.COOLProductionStep", "Deleting the record.");
		}
		delete coolProductionStep;
	}
}

void COOLManager::handleCreateCOOLProductionStepAndSave(COOLProductionStep *msgProductionStep)
{
	if (!dbAccess || !msgProductionStep)
		return;

	COOLProductionStep *newStep = new COOLProductionStep(msgProductionStep);
	if (newStep)
	{
		newStep->setDBAccess(dbAccess);

		//	Save the new record
		handleSaveRecordCOOLProductionStep(newStep);

		//  Set this new ProductionStep as the current ProductionStep.
		setCurrentProductionStep(newStep);
		Debug::trace("COOLManager.COOLProductionStep", "Created and saved the new ProductionStep. It is now the currentProductionStep.");
/*
	I don't think we want to send anything back right now.
		if (mySession)
		{
			// Send the new ProductionStep to the Browser.
			SessionMessage catData(COOLDev, COOLDevWriteProductionStep);
			catData.setFormatType(BrowserFormat);
			catData.setHfcObject(currentProductionStep);
			mySession->send(&catData, Messenger::DEST_UI);
		}
*/
	}
}

void COOLManager::handleReadCOOLProdStepsByOneCategoryAndAllCategories(AllCoolProductionSteps *myList)
{
	if (!dbAccess)
		return;
	bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
	HobString myDebug;

	if (traceOn)
		myDebug = "handleReadCOOLProdStepsByOneCategoryAndAllCategories():  ";

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolProductionSteps();
		deleteMyList = true;
	}

	if (myList)
	{
		myList->findAllProductionStepsLinkedToAllCategories(dbAccess);
		if (myList->getCategoryNum() > 0)
			myList->findAllProductionStepsLinkedToCategory(dbAccess);
	}

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("category = ").append(myList->getCategory());
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}
#ifndef MAKE_MUTUALLY_EXCLUSIVE
	myList->getMyList().unique();
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   After calling unique() on the list, myList size = ").append(Integer::toString((int)myList->size()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}
#endif

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLProductionStepsRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLProductionStep, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteProductionStepsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLProductionStep", myDebug + "Deleted myList!");
	}
}

void COOLManager::handleReadCOOLProdStepsByNotAllCategories(AllCoolProductionSteps *myList)
{
//	This method will read all ProductionSteps, then eliminate the ProductionSteps that have the allCategories flag set.
	bool traceOn = Debug::isTracing("COOLManager.COOLProductionStep");
	HobString myDebug;
	if (traceOn)
		myDebug = "handleReadCOOLProdStepsByNotAllCategories():  ";

	if (!dbAccess)
		return;

	AllCoolProductionSteps *allCategoriesList = new AllCoolProductionSteps();
	if (!allCategoriesList)
		return;

	bool deleteMyList = false;
	if (!myList)
	{
		myList = new AllCoolProductionSteps();
		deleteMyList = true;
	}
	if (!myList)
	{
		delete allCategoriesList;
		return;
	}

	myList->findAllProductionSteps(dbAccess);
	allCategoriesList->findAllProductionStepsLinkedToAllCategories(dbAccess);

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("   myList size = ").append(Integer::toString((int)myList->size()));
		myDebug2.append("   allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
		Debug::trace("COOLManager.COOLProductionStep", myDebug2);
	}

	if (allCategoriesList->size())
	{
		//	Go through the lists and remove any countries from myList that are in allCategoriesList.
		HobString prodStep;
		int returnVal;
		HobList_type tempList = myList->getMyList();

		returnVal = allCategoriesList->first(prodStep);
		while (returnVal != -1)
		{
			tempList.remove(prodStep);
			returnVal = allCategoriesList->next(prodStep);
		}
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("After the removal of allCategories productionSteps,  allCategoriesList size = ").append(Integer::toString((int)allCategoriesList->size()));
			Debug::trace("COOLManager.COOLPretext", myDebug2);
		}
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLProductionStepsRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLProductionStep, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteProductionStepsByCategory);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		mySession->send(&ctData, Messenger::DEST_UI);
	}
	if (deleteMyList)
	{
		delete myList;
		if (traceOn)
			Debug::trace("COOLManager.COOLProductionStep", myDebug + "Deleted myList!");
	}
	delete allCategoriesList;
}
