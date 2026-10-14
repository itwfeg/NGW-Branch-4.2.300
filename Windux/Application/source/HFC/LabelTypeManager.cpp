/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "LabelTypeManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"
#include "HfcPrinterStockMetrics.h"
#include "LabelStockDescriptor.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelTypeManager::dataType = 0;
std::string LabelTypeManager::dataTypeName("LabelTypeManager"); 
#endif


LabelTypeManager::LabelTypeManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

LabelTypeManager::LabelTypeManager(hfc::HFCManagerContainer *container)
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


LabelTypeManager::~LabelTypeManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (partialLabelType)
		delete partialLabelType;

	destroyQueriesListAndIters();
}


void LabelTypeManager::init()
{
	mySession = 0;

	partialLabelType = 0;

	allLabelTypesQuery = 0;
	allLabelTypesIter  = 0;

	allLabelTypesListIter = 0;
}


DBSession *LabelTypeManager::getMySession()
{
	return mySession;
}
void LabelTypeManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
LabelType *LabelTypeManager::firstLabelType(HobRecordNum deptNum)
{
	try
	{
		if (!dbAccess)
			return 0;

		buildLabelList(deptNum);

		if (!allLabelTypesList.empty())
		{
			if (allLabelTypesListIter)
				delete allLabelTypesListIter;
			allLabelTypesListIter = new std::list<HobRecordNum>::iterator(allLabelTypesList.begin());
			if (allLabelTypesListIter && *allLabelTypesListIter != allLabelTypesList.end())
			{
				long ltNum = **allLabelTypesListIter;
				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
		}
		return 0;
	}
	catch (DBException &dbe)
	{
		Debug::trace("LabelTypeManager", " - firstLabelType got a database exception ********************************************************");
		DBException::Type t = dbe.getType();
		Debug::trace("LabelTypeManager", HobString("Database Exception thrown  type:") + HobString(Integer::toString(t)) +
												HobString("  info:") + dbe.info());
		return 0;
	}
}


/* If the first record was not read, this method will return NULL.
 */
LabelType *LabelTypeManager::lastLabelType(HobRecordNum deptNum)
{
	try
	{
		if (!dbAccess)
			return 0;

		buildLabelList(deptNum);

		if (!allLabelTypesList.empty())
		{
			if (allLabelTypesListIter)
				delete allLabelTypesListIter;
			allLabelTypesListIter = new std::list<HobRecordNum>::iterator(allLabelTypesList.end());
			--(*allLabelTypesListIter);
			if (allLabelTypesListIter && *allLabelTypesListIter != allLabelTypesList.end())
			{
				long ltNum = **allLabelTypesListIter;
				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
		}
		return 0;
	}
	catch (DBException &dbe)
	{
		Debug::trace("LabelTypeManager", " - lastLabelType got a database exception ********************************************************");
		DBException::Type t = dbe.getType();
		Debug::trace("LabelTypeManager", HobString("Database Exception thrown  type:") + HobString(Integer::toString(t)) +
												HobString("  info:") + dbe.info());
		return 0;
	}
}


HobRecordNum LabelTypeManager::setPositionAtLabelType(HobRecordNum beginFromLabelTypeNum)
{
	try
	{
		if (!allLabelTypesList.empty() && allLabelTypesListIter)
		{
			if (beginFromLabelTypeNum != 0)
			{
				// If the iterator is at the one that is being requested, then we don't have to do anything; we're already there.
				if (**allLabelTypesListIter == beginFromLabelTypeNum)
					return beginFromLabelTypeNum;
				else if (**allLabelTypesListIter > beginFromLabelTypeNum)
				{
					// If the iterator is already beyond the one being requested, then go back to the beginning and start over.
					delete allLabelTypesListIter;
					allLabelTypesListIter = new std::list<HobRecordNum>::iterator(allLabelTypesList.begin());
				}

				// Find the one being requested, but stop if we are past it (which means it's not in the list) or we go to the end.
				while ( *allLabelTypesListIter != allLabelTypesList.end() && **allLabelTypesListIter < beginFromLabelTypeNum )
					++(*allLabelTypesListIter);

				if ( *allLabelTypesListIter != allLabelTypesList.end() )
					return **allLabelTypesListIter;
			}
		}

		// Return that we either don't have a list, or we couldn't find the one being requested and there's nothing after it either.
		return -1;
	}
	catch (DBException &dbe)
	{
		Debug::trace("LabelTypeManager", " - setPositionAtLabelType got a database exception ********************************************************");
		DBException::Type t = dbe.getType();
		Debug::trace("LabelTypeManager", HobString("Database Exception thrown  type:") + HobString(Integer::toString(t)) +
												HobString("  info:") + dbe.info());
		return 0;
	}
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstLabelType() method, the first call to this method will
 * result in the first record being returned.
 */
LabelType *LabelTypeManager::nextLabelType(bool shouldLoop, HobRecordNum beginFromLabelTypeNum, HobRecordNum deptNum)
{
	if (!dbAccess)
		return 0;

	if (beginFromLabelTypeNum == (HobRecordNum)0)
	{
		Debug::trace("LabelTypeManager.next", 20, "nextLabelType: reference label number is 0.");

		if ( allLabelTypesList.empty() || !allLabelTypesListIter )
		{
			Debug::trace("LabelTypeManager.next", 20, "nextLabelType: we will get the first label.");
			return firstLabelType(deptNum);
		}
		else
		{
			Debug::trace("LabelTypeManager.next", 20, "nextLabelType: we will get the next to the current label.");

			++(*allLabelTypesListIter);
			if ( *allLabelTypesListIter == allLabelTypesList.end() )
			{
				if (shouldLoop)
					return firstLabelType(deptNum);
				else
					return 0;
			}
			else
			{
				long ltNum = **allLabelTypesListIter;
				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
		}
	}
	else
	{
		Debug::trace("LabelTypeManager.next", 20, "nextLabelType: reference label number is " + longtostring(beginFromLabelTypeNum) + ". Going to position on it.");

		if ( allLabelTypesList.empty() || !allLabelTypesListIter )
			// If there is no list yet, just get the first label type, but don't return it.  Still need to position into the list based on beginFromLabelTypeNum.
			firstLabelType(deptNum);

		HobRecordNum ltNum = setPositionAtLabelType(beginFromLabelTypeNum);

		Debug::trace("LabelTypeManager.next", 20, "nextLabelType: setPositionAtLabelType returned " + longtostring(ltNum));

		if (ltNum >= 0)
		{
			/* If the label type found has a label type number greater than the one requested,
			 * then this is the next one, go ahead and return it.
			 */
			if ( ltNum > beginFromLabelTypeNum )
			{
				Debug::trace("LabelTypeManager.next", 20, "nextLabelType: found and returning label number " + longtostring(ltNum) + ".");

				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
			else
			{
				// Found the label type that we wanted to position at, so we need to return the next one.
				Debug::trace("LabelTypeManager.next", 20, "nextLabelType: found reference label. Going to get next one.");

				++(*allLabelTypesListIter);
				if (*allLabelTypesListIter == allLabelTypesList.end())
				{
					if (shouldLoop)
						return firstLabelType(deptNum);
					else
						return 0;
				}
				else
				{
					long ltNum = **allLabelTypesListIter;
					LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
					return lt;
				}
			}
		}
		else
		{
			Debug::trace("LabelTypeManager.next", 20, "nextLabelType: reference label not found.");

			if (shouldLoop)
			{
				Debug::trace("LabelTypeManager.next", 20, "nextLabelType: will loop.");
				return firstLabelType(deptNum);
			}
			else
			{
				Debug::trace("LabelTypeManager.next", 20, "nextLabelType: return empty handed.");
				return 0;
			}
		}
	}
}


/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstLabelType() method, the first call to this method will
 * result in the first record being returned.
 */
LabelType *LabelTypeManager::previousLabelType(bool shouldLoop, HobRecordNum beginFromLabelTypeNum, HobRecordNum deptNum)
{
	if (!dbAccess)
		return 0;

	if (allLabelTypesList.empty())
		return lastLabelType(deptNum);

	if ( beginFromLabelTypeNum == (HobRecordNum)0 )
	{
		Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: reference label number is 0.");

		if (!allLabelTypesListIter)
		{
			Debug::trace("LabelTypeManager.next", 20, "nextLabelType: we will get the first label.");
			return lastLabelType(deptNum);
		}
		else
		{
			Debug::trace("LabelTypeManager.next", 20, "nextLabelType: we will get the next to the current label.");

			--(*allLabelTypesListIter);
			if (*allLabelTypesListIter == allLabelTypesList.end())
			{
				if (shouldLoop)
					return lastLabelType(deptNum);
				else
					return 0;
			}
			else
			{
				long ltNum = **allLabelTypesListIter;
				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
		}
	}
	else
	{
		Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: reference label number is " + longtostring(beginFromLabelTypeNum) + ". Going to position on it.");

		HobRecordNum ltNum = setPositionAtLabelType(beginFromLabelTypeNum);

		Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: setPositionAtLabelType returned " + longtostring(ltNum));

		if (ltNum >= 0)
		{
			/* If the label type found has a label type number less than the one requested,
			 * then this is the next one, go ahead and return it.
			 */
			if (ltNum < beginFromLabelTypeNum )
			{
				Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: found and returning label number " + longtostring(ltNum) + ".");

				LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
				return lt;
			}
			else
			{
				Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: found reference label " + longtostring(ltNum) + ". Going to get previous one.");

				if (*allLabelTypesListIter == allLabelTypesList.begin())
				{
					if (shouldLoop)
						return lastLabelType(deptNum);
					else
						return 0;
				}
				else
				{
					--(*allLabelTypesListIter);
					long ltNum = **allLabelTypesListIter;
					LabelType *lt = LabelType::findRecord(ltNum, deptNum, dbAccess);
					return lt;
				}
			}
		}
		else
		{
			Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: reference label not found.");

			if (shouldLoop)
			{
				Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: will loop.");

				/* If we didn't find one from the setPositionAt...(), then we went past the end before finding
				 * the one requested, so return the last one.
				 */
				return lastLabelType(deptNum);
			}
			else
			{
				Debug::trace("LabelTypeManager.previous", 20, "previousLabelType: return empty handed.");

				return 0;
			}
		}
	}
}


int LabelTypeManager::countAllCustomLabelTypes()
{
	if (!dbAccess)
		return -1;

	DBLabelsRM *dbLabelsRM = dbAccess->getDBLabelsRM();
	if (!dbLabelsRM)
		return -1;

	return dbLabelsRM->countAll();
}

/* Provide a countAll method so the caller doesn't need to know about a dbLabelsRM.
 */
int LabelTypeManager::countAllLabelTypes()
{
	int count = 0;

	LabelType* lt = firstLabelType();
	while ( lt )
	{
		++count;
		delete lt;
		lt = nextLabelType(false);
	}

	return count;
}


void LabelTypeManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	Debug::trace("LabelTypeManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtReadAll:
			//tester();
			handleReadAll(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtFirst:
			handleFirst(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtLast:
			handleLast(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtNext:
			handleNext(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		case DtPrevious:
			handlePrevious(dynamic_cast<LabelType *> (msg->getHfcObject()));
			break;

		default:
			Debug::trace("LabelTypeManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}

void LabelTypeManager::tester()
{
	Debug::trace("LabelTypeManager", "tester ...");
	if (!mySession || !dbAccess)
		return;

	HobRecordNum deptNum = 0;
	LabelType *label = 0;

	destroyListAndIter();
	label = nextLabelType(false, 21, deptNum);


	destroyListAndIter();
	label = previousLabelType(false, 21, deptNum);


	destroyListAndIter();

	label = firstLabelType(deptNum);
	while (label)
	{
		HobRecordNum ltNum = label->getLabelTypeNum();
		delete label;
		label = 0;
		label = nextLabelType(false, ltNum, deptNum);
	}


	label = lastLabelType(deptNum);
	while (label)
	{
		HobRecordNum ltNum = label->getLabelTypeNum();
		delete label;
		label = 0;
		label = previousLabelType(false, ltNum, deptNum);
	}


	label = firstLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = nextLabelType(false);
	}


	label = lastLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = previousLabelType(false);
	}

	deptNum = 1;
	label = firstLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = nextLabelType(false);
	}

	label = lastLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = previousLabelType(false);
	}

	deptNum = 2;
	label = firstLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = nextLabelType(false);
	}

	label = lastLabelType(deptNum);
	while (label)
	{
		delete label;
		label = 0;
		label = previousLabelType(false);
	}

	destroyListAndIter();
}


void LabelTypeManager::handleRead(LabelType *labelType)
{
	if (!labelType || !dbAccess || !mySession)
		return;

	LabelType *label = LabelType::findRecord(labelType->getLabelTypeNum(), labelType->getDeptNum(), dbAccess);
	if (!label)
		label = new LabelType((HobRecordNum)0);

	if (labelType->doIncludeEmbeddedLabels())
		label->generateEmbeddedLabels();
	label->setFormatType(ViewLabelFormat);

	// Send the LabelType to the UI (Browser)
	SessionMessage ltMsg(LabelTypeDev, DtWrite);
	ltMsg.setFormatType(BrowserFormat);
	ltMsg.setHfcObject(label);
	mySession->send(&ltMsg, Messenger::DEST_UI);

	if (label)
		delete label;
}


void LabelTypeManager::handleReadAll(LabelType *labelType)
{
	Debug::trace("LabelTypeManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	HobRecordNum deptNum = (labelType) ? labelType->getDeptNum() : RECORD_NOT_FOUND;
	LabelType *label = 0;

	label = firstLabelType(deptNum);
	while (label)
	{
		label->setFormatType(ViewLabelFormat);
		SessionMessage labelMsg(LabelTypeDev, DtWrite);
		labelMsg.setFormatType(BrowserFormat);
		labelMsg.setHfcObject(label);
		mySession->send(&labelMsg, Messenger::DEST_UI);

		label = nextLabelType(false, label->getLabelTypeNum(), deptNum);
	}
}


void LabelTypeManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	LabelType *msgLabelType = dynamic_cast<LabelType *> (msg->getHfcObject());
	if (!msgLabelType || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Labels))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		LabelType *existingLabelType = LabelType::findRecord(msgLabelType->getLabelTypeNum(), msgLabelType->getDeptNum(), dbAccess);
		// Only delete it if it is in the dept being asked for in the message's LabelType object, not if it ended up finding it in the default dept
		if (existingLabelType && existingLabelType->getDeptNum() == msgLabelType->getDeptNum())
		{
			existingLabelType->deleteRecord();
			Debug::trace("LabelTypeManager", HobString("handleDelete - label type deleted - ltnum:") +
															HobString(Long::toString((long)msgLabelType->getLabelTypeNum())));
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Labels))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingLabelType, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingLabelType;
		}
		else
			Debug::trace("LabelTypeManager",
					"handleDelete - label type to be deleted was not in the database to delete - ltnum:" +
					HobString(Long::toString((long)msgLabelType->getLabelTypeNum())));
	}
}


void LabelTypeManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void LabelTypeManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("LabelTypeManager", "handleDeleteAll - deleting all custom label types");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Labels))
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

		DBLabelsRM *dbRM = dbAccess->getDBLabelsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		// Delete just the the query and iterator for the custom/user labels.
		destroyQueriesListAndIters();
	}

	Debug::trace("LabelTypeManager", "handleDeleteAll - leaving");
}


void LabelTypeManager::handleUpdateAndSave(LabelType *labelType)
{
	if (!labelType)
		return;

	LabelType *existingLabelType = LabelType::findRecord(labelType->getLabelTypeNum(), labelType->getDeptNum(), dbAccess);
	// Only update/delete it if it is in the dept being asked for in the message's LabelType object, not if it ended up finding it in the default dept
	if ( existingLabelType && existingLabelType->getDeptNum() == labelType->getDeptNum() && existingLabelType->getRotationOnLabelStock() != labelType->getRotationOnLabelStock() )
	{
		if (existingLabelType->isRecordValid())
		{
			Debug::trace("LabelTypeManager", "handleRestore - found existing valid LabelType, will update it - ");
			existingLabelType->setRotationOnLabelStock(labelType->getRotationOnLabelStock());
			existingLabelType->saveRecord();
		}
	}
}


void LabelTypeManager::handleRestore(SessionMessage *msg)
{
	LabelType *restoreLabelType = dynamic_cast<LabelType *> (msg->getHfcObject());
	if (!restoreLabelType || !dbAccess)
		return;

	if ( partialLabelType &&
			partialLabelType->getLabelTypeNum() != restoreLabelType->getLabelTypeNum() )
	{
		// Hey, we never received the remaining parts of the partial label type we are tracking.
		// Just dump the unfinished one on the floor.
		Debug::trace("LabelTypeManager", "handleRestore - had a partial label, but the label number changed "
			"before it completed - partial labelNum: " + longtostring(partialLabelType->getLabelTypeNum()) +
			"  new labelNum: " + longtostring(restoreLabelType->getLabelTypeNum()));

		delete partialLabelType;
		partialLabelType = 0;
	}

	if ( restoreLabelType->getSequenceMax() >= 1 )
	{
		if ( !partialLabelType )
		{
			if ( restoreLabelType->getSequence() == 1 )
			{
				msg->setHfcObjectCreatedInternally(false);
				partialLabelType = restoreLabelType;

				if ( restoreLabelType->getSequenceMax() > 1 )
				{
					Debug::trace("LabelTypeManager", "handleRestore - have a new partial label - partial labelNum:" +
						longtostring(partialLabelType->getLabelTypeNum()) + "  partial sequenceMax:" +
						inttostring(partialLabelType->getSequenceMax()) + "  partial sequence:" + inttostring(partialLabelType->getSequence()));
				}
				// In the case of one single piece label it is reported as complete below.
			}
			else
			{
				Debug::trace("LabelTypeManager", "handleRestore - dropped received label packet for partial labelNum:" +
					longtostring(restoreLabelType->getLabelTypeNum()) + "  partial sequence:" +
					inttostring(restoreLabelType->getSequence()) + " since we don't have a partial label with initial sequences");

				return;
			}
		}
		else
		{
			if ( partialLabelType->getSequence() + 1 == restoreLabelType->getSequence() )
			{
				Debug::trace("LabelTypeManager", "handleRestore - have a partial label, and the next part came in - "
					"partial labelNum:" + longtostring(partialLabelType->getLabelTypeNum()) + "  partial sequenceMax:" +
					inttostring(partialLabelType->getSequenceMax()) + "  partial sequence:" +
					inttostring(partialLabelType->getSequence()) + "  new sequence:" + inttostring(restoreLabelType->getSequence()));

				partialLabelType->setSequence(restoreLabelType->getSequence());
				partialLabelType->setResourceLength(partialLabelType->getResourceLength() + restoreLabelType->getResourceLength());
				std::string tempData(partialLabelType->getOrigBinaryData());
				tempData.append(restoreLabelType->getOrigBinaryData());
				partialLabelType->setOrigBinaryData(tempData);
			}
			else
			{
				Debug::trace("LabelTypeManager", "handleRestore - dropped received label packet for partial labelNum:" +
                    longtostring(restoreLabelType->getLabelTypeNum()) + "  partial sequence:" +
					inttostring(restoreLabelType->getSequence()) + " since we only have a partial label with sequence " +
					inttostring(partialLabelType->getSequence()));

				return;
			}
		}
	}
	else
	{
		// SJJC: Error: the SequenceMax should be 1 or greater
		return;
	}


	if ( partialLabelType->getSequence() == partialLabelType->getSequenceMax() )
	{
		HobString temp = "labelNum:";
		temp.append(Long::toString(partialLabelType->getLabelTypeNum()));
		temp.append("  sequenceMax:");
		temp.append(Integer::toString(partialLabelType->getSequenceMax()));
		temp.append("  sequence:");
		temp.append(Integer::toString(partialLabelType->getSequence()));
		Debug::trace("LabelTypeManager", "handleRestore - have a full label - " + temp);


		if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Labels))
		{
			// batch the record being restored; don't actually write it to the database right now.
			std::string synchSer = "";
			serializeForSynchronization(synchSer, partialLabelType);

			Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer));
			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			if ( proxy )
				proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
		}
		else
		{
			//	Do not save this if it is an old format label. Check to see if it is.
			bool saveToDatabase = true;
			HobartLabel * hobLabel = 0;
			Blob * labelBlob = new Blob(partialLabelType->getOrigBinaryData());
			if ( labelBlob )
			{
				// In Windux and before, we store the un-nibblized (binary) data for the label type in the database.
				// For Fastrack, we are storing the nibblized data in the database,
				// so we need to unnibblize it to get the binary representation of the label type.
				labelBlob->unNibblizeBlob();

				hobLabel = new HobartLabel(labelBlob, true);
				if ( hobLabel && hobLabel->isFormatChanged() )
					saveToDatabase = false;

				if ( !hobLabel->isBinaryValid() ) //if label isn't bare minimum size of 138 bytes, we don't want to save this
					saveToDatabase = false;

				delete labelBlob;
			}

			if ( saveToDatabase )
			{
				recordIgnored_ = false;

				partialLabelType->setDBAccess(dbAccess);

				LabelType *existingLabelType = LabelType::findRecord(partialLabelType->getLabelTypeNum(), partialLabelType->getDeptNum(), dbAccess);
				// Only update/delete it if it is in the dept being asked for in the message's LabelType object, not if it ended up finding it in the default dept
				if (existingLabelType && existingLabelType->getDeptNum() == partialLabelType->getDeptNum())
				{
					if (existingLabelType->isRecordValid())
					{
						Debug::trace("LabelTypeManager", "handleRestore - found existing valid LabelType, will update it - ");
						existingLabelType->updateRecord(partialLabelType);
						existingLabelType->saveRecord();
					}
					else
					{
						Debug::trace("LabelTypeManager", "handleRestore - found existing LabelType but it is corrupt, will just save new record - ");
						existingLabelType->deleteRecord();	   
						partialLabelType->saveRecord();
					}

					delete existingLabelType;
				}
				else
				{
					Debug::trace("LabelTypeManager", "handleRestore - don't have existing LabelType, will just save new record - ");
					partialLabelType->saveRecord();
				}
			}
			else
				recordIgnored_ = true;
	
			if (hobLabel)
				delete hobLabel;

			Debug::trace("LabelTypeManager.restore", "handleRestore():  recordIgnored = " + booltostring(recordIgnored_));
		}

		Debug::trace("LabelTypeManager", "handleRestore - deleting partialLabelType - ");
		delete partialLabelType;
		partialLabelType = 0;
	}
}

/* This method creates a list of all label types that can be used for the current dept.
   Those label types are in the current dept, in the default dept if they don't exist in the current dept,
   and finally in the ROM labels if they don't exist in either of the previous two.
 */
void LabelTypeManager::buildLabelList(HobRecordNum deptNum)
{
	try
	{
		bool isTracing = Debug::isTracing("LabelTypeManager");

		if (!dbAccess || deptNum == RECORD_NOT_FOUND)
			return;

		destroyListAndIter();

		DBLabelsRM *dbLabelsRM = dbAccess->getDBLabelsRM();
		if (!dbLabelsRM)
			return;

		// Get all label types for the requested department.  Put the label type numbers in the list.
		allLabelTypesQuery = dbLabelsRM->retrieveAllForDept(longtostring(deptNum));
		if (allLabelTypesQuery)
		{
			if (isTracing)
				Debug::trace("LabelTypeManager", string("buildLabelList - reading custom labels for dept: ") + longtostring(deptNum));
			allLabelTypesIter = new DBQuery<DBLabel>::const_iterator(allLabelTypesQuery->begin());
			if (allLabelTypesIter)
			{
				while (*allLabelTypesIter != allLabelTypesQuery->end())
				{
					if (isTracing)
						Debug::trace("LabelTypeManager", "Current custom label is Label# " + (*allLabelTypesIter)->getNumber());
					allLabelTypesList.push_back(stringtolong((*allLabelTypesIter)->getNumber()));
					++(*allLabelTypesIter);
				}
				delete allLabelTypesIter;
				allLabelTypesIter = 0;
			}
			delete allLabelTypesQuery;
			allLabelTypesQuery = 0;
		}

		// Get all label types for the default department (0).  Append these label type numbers to the list.
		if (deptNum != 0)
		{
			allLabelTypesQuery = dbLabelsRM->retrieveAllForDept("0");
			if (allLabelTypesQuery)
			{
				if (isTracing)
					Debug::trace("LabelTypeManager", string("buildLabelList - reading custom labels for dept: ") + longtostring(deptNum));
				allLabelTypesIter = new DBQuery<DBLabel>::const_iterator(allLabelTypesQuery->begin());
				if (allLabelTypesIter)
				{
					while (*allLabelTypesIter != allLabelTypesQuery->end())
					{
						if (isTracing)
							Debug::trace("LabelTypeManager", "Current custom label is Label# " + (*allLabelTypesIter)->getNumber());
						allLabelTypesList.push_back(stringtolong((*allLabelTypesIter)->getNumber()));
						++(*allLabelTypesIter);
					}
					delete allLabelTypesIter;
					allLabelTypesIter = 0;
				}
				delete allLabelTypesQuery;
				allLabelTypesQuery = 0;
			}
		}

		// Get all label types for the ROM labels.  Append these label type numbers to the list.  This is the master list.
		DBROMLabelsRM *dbROMLabelsRM = dbAccess->getDBROMLabelsRM();
		if (dbROMLabelsRM)
		{
			edb::DBQuery<edb::DBROMLabel> *allROMLabelTypesQuery = dbROMLabelsRM->retrieveAll();
			if (allROMLabelTypesQuery)
			{
				if (isTracing)
					Debug::trace("LabelTypeManager", "buildLabelList - reading ROM labels");
				edb::DBQuery<edb::DBROMLabel>::const_iterator *allROMLabelTypesIter = new DBQuery<DBROMLabel>::const_iterator(allROMLabelTypesQuery->begin());
				if (allROMLabelTypesIter)
				{
					while (*allROMLabelTypesIter != allROMLabelTypesQuery->end())
					{

						if (isTracing)
							Debug::trace("LabelTypeManager", "Current rom label is Label# " + (*allROMLabelTypesIter)->getNumber());
						allLabelTypesList.push_back(stringtolong((*allROMLabelTypesIter)->getNumber()));
						++(*allROMLabelTypesIter);
					}
					delete allROMLabelTypesIter;
					allROMLabelTypesIter = 0;
				}
				delete allROMLabelTypesQuery;
				allROMLabelTypesQuery = 0;
			}
		}

		// Use the sort method to put all of them in numerical order.
		allLabelTypesList.sort();
		// Use the unique method to remove all duplicate entries from the list.  Note that you have to sort the list first for unique to remove the duplicates.
		allLabelTypesList.unique();
	}
	catch (DBException &dbe)
	{
		Debug::trace("LabelTypeManager", " - buildLabelList got a database exception ********************************************************");
		DBException::Type t = dbe.getType();
		Debug::trace("LabelTypeManager", HobString("Database Exception thrown  type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		return;
	}
}

void LabelTypeManager::handleFirst(LabelType *labelType)
{
	if (!mySession || !dbAccess)
		return;

	LabelType *label = firstLabelType( (labelType) ? labelType->getDeptNum() : RECORD_NOT_FOUND );
	if (!label)
		label = new LabelType((HobRecordNum)0);

	if (labelType && labelType->doIncludeEmbeddedLabels())
		label->generateEmbeddedLabels();
	label->setFormatType(ViewLabelFormat);

	// Send the LabelType to the UI (Browser)
	SessionMessage ltMsg(LabelTypeDev, DtWrite);
	ltMsg.setFormatType(BrowserFormat);
	ltMsg.setHfcObject(label);
	mySession->send(&ltMsg, Messenger::DEST_UI);

	if (label)
		delete label;
}

void LabelTypeManager::handleLast(LabelType *labelType)
{
	if (!mySession || !dbAccess)
		return;

	LabelType *label = lastLabelType( (labelType) ? labelType->getDeptNum() : RECORD_NOT_FOUND );
	if (!label)
		label = new LabelType((HobRecordNum)0);

	if (labelType && labelType->doIncludeEmbeddedLabels())
		label->generateEmbeddedLabels();
	label->setFormatType(ViewLabelFormat);

	// Send the LabelType to the UI (Browser)
	SessionMessage ltMsg(LabelTypeDev, DtWrite);
	ltMsg.setFormatType(BrowserFormat);
	ltMsg.setHfcObject(label);
	mySession->send(&ltMsg, Messenger::DEST_UI);

	if (label)
		delete label;
}

void LabelTypeManager::handleNext(LabelType *referenceLabel)
{
	if (!mySession || !dbAccess)
		return;

	LabelType *label = nextLabelType( true, ( (referenceLabel) ? referenceLabel->getLabelTypeNum() : 0 ), ( (referenceLabel) ? referenceLabel->getDeptNum() : RECORD_NOT_FOUND) );
	if (!label)
		label = new LabelType((HobRecordNum)0);

	if (referenceLabel && referenceLabel->doIncludeEmbeddedLabels())
		label->generateEmbeddedLabels();
	label->setFormatType(ViewLabelFormat);

	// Send the LabelType to the UI (Browser)
	SessionMessage ltMsg(LabelTypeDev, DtWrite);
	ltMsg.setFormatType(BrowserFormat);
	ltMsg.setHfcObject(label);
	mySession->send(&ltMsg, Messenger::DEST_UI);

	if (label)
		delete label;
}

void LabelTypeManager::handlePrevious(LabelType *referenceLabel)
{
	if (!mySession || !dbAccess)
		return;

	LabelType *label = previousLabelType( true, ( (referenceLabel) ? referenceLabel->getLabelTypeNum() : 0 ), ( (referenceLabel) ? referenceLabel->getDeptNum() : RECORD_NOT_FOUND ) );
	if (!label)
		label = new LabelType((HobRecordNum)0);

	if (referenceLabel && referenceLabel->doIncludeEmbeddedLabels())
		label->generateEmbeddedLabels();
	label->setFormatType(ViewLabelFormat);

	// Send the LabelType to the UI (Browser)
	SessionMessage ltMsg(LabelTypeDev, DtWrite);
	ltMsg.setFormatType(BrowserFormat);
	ltMsg.setHfcObject(label);
	mySession->send(&ltMsg, Messenger::DEST_UI);

	if (label)
		delete label;
}


// static
// If a better fit label is determined to exist, the labelTypeNum parameter to this method is changed to be the new label type number.
// Additionally, if the better fit label type is in a different dept (for example, maybe the better-fit label is in the default dept (dept 0),
// the deptNum parameter to this method is changed to be the new dept number.
// The deptNum passed in has to be the deptNum of the PLU and can't come from the labelType object,
// since the label type may not exist in the PLU's dept but it exists in the default dept; so if the deptNum cam from the labelType, it would be 0.
// But when looking through the family structure, we want to start with the PLU's dept.

LabelMediaReturn LabelTypeManager::validateLabelTypeWithStock(LabelStockDescriptor *labelTypeLSD, HfcPrinterStockMetrics *printerStockMetrics, bool exactFit, HobRecordNum *labelTypeNum, HobRecordNum *deptNum, HobRecordNum deptNumOfProduct, DBAccess *dbAccess)
{
	if ( !printerStockMetrics || !labelTypeNum || !deptNum || !dbAccess )
		return LabelWontFit;

	bool traceOn = Debug::isTracing("LabelTypeManager.Family");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = string("validateLabelTypeWithStock():  labelTypeNum: ") + longtostring(*labelTypeNum) + "  deptNUm: " + longtostring(*deptNum);
	}

	LabelMediaReturn result = LabelInvalid;

	if (labelTypeLSD && labelTypeLSD->isExactMatch(printerStockMetrics) == true)
		result = LabelOk;
	else
	{
		//	The label type does not match the size of the label stock loaded.
		//	Are we looking for an exact match?  If not, then look in the family for the best-fitting label.
		if (exactFit == false)
		{
			if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::IGNORELABELFIT) )  // for internal testing purposes only
				result = LabelOk;
			else
			{
				//  No, search the Label family for a label to fit this label stock.
				//  Pass the label type number and dept number parameters to the findMatch... method as pointers,
				//  since that method will ultimately determine any new label type and dept numbers.
				result = findMatchInLabelFamily(labelTypeNum, deptNum, deptNumOfProduct, printerStockMetrics, dbAccess);

				// If there was no label stock descriptor passed in, which would occur when the label type record does not exist for a label type assigned
				// to a PLU, and no best fit label type exists in the database, change the return value from findMatchInLabelFamily() from LabelWontFit to
				//LabelInvalid so that the appropriate error message is displayed (in the method that called this one).
				if ( !labelTypeLSD && result == LabelWontFit )
					result = LabelInvalid;

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug + "   returned labelTypeNum: " + longtostring(*labelTypeNum) + "  deptNum: " + longtostring(*deptNum);
					Debug::trace("LabelTypeManager.Family", msgDebug2);
				}
			}
		}
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;

		if (result == LabelOk)
			msgDebug2.append(" is OK.");
		else if (result == LabelCorrected)
			msgDebug2.append(" has been corrected.");
		else if (result == LabelWontFit)
			msgDebug2.append(" won't fit on the label stock.");
		else
			msgDebug2.append(" is invalid.");
		
		Debug::trace("LabelTypeManager.Family", msgDebug2);
	}
	return result;
}

// static
LabelMediaReturn LabelTypeManager::findMatchInLabelFamily(HobRecordNum *labelTypeNum, HobRecordNum *deptNum, HobRecordNum deptNumOfProduct, HfcPrinterStockMetrics *printerStockMetrics, DBAccess *dbAccess)
{
	if (!labelTypeNum || !deptNum || !printerStockMetrics || !dbAccess)
		return LabelWontFit;

	bool traceOn = Debug::isTracing("LabelTypeManager.Family");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "findMatchInLabelFamily():  ";

	HobRecordNum currentDeptNum = deptNumOfProduct;

	HobRecordNum familyStart = (*labelTypeNum / 10) * 10;		//	eg. (labelTypeNum == 113), start looking at label 110
	HobRecordNum bestFitLabelNum = 0, bestFitDeptNum = 0, searchNum = 0;
	LabelType *label = 0;
	int longestLength = 0, widestWidth = 0;
	bool haveAnExactMatch = false;


	// First check the labelTypeNum | deptNum passed in to see if it is an exact match, or if it even fits on the label stock.
	// If it's not an exact fit, but fits on the label stock, then we'll look in the family for the best fit.

	//	If labelTypeNum is smaller than the current label stock, set up the current label dimensions as the best fit so far.
	//	If no other labelType is larger that will fit on the label stock, then labelTypeNum will be used.
	//		(Problems would occur if a family had labelTypes that were the same size but smaller than the current label stock.
	//		If labelTypes 1,2 & 3 were all the same size and labelType 2 was chosen, labelType 1 would be selected because
	//		it was the first best fit labelType found.)

	// We are looking through the family using the normal/traditional lookup scheme that looks for the label type number in the plu's dept,
	// then default dept, then ROM, and returns it wherever it exists in the hierarchy.

	label = LabelType::findRecord(*labelTypeNum, currentDeptNum, dbAccess);
	if (label)
	{
		if (label->getErrorCode() == RECORD_OK)
		{
			if (label->getLabelStockDescriptor()->isExactMatch(printerStockMetrics))
			{
				//	The labelTypeNum | deptNum passed in is an exact match to the label stock. Stop now!
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("An exact match!!! labelTypeNum = ").append(Long::toString(label->getLabelTypeNum())).append("  deptnum = ").append(Long::toString(label->getDeptNum()));
					Debug::trace("LabelTypeManager.Family", msgDebug2);
				}
				bestFitLabelNum = label->getLabelTypeNum();
				bestFitDeptNum = label->getDeptNum();
				haveAnExactMatch = true;
			}
			else
			{
				if ((label->getWidthDots() <= printerStockMetrics->getLabelWidthInDots()) && (label->getLength() <= printerStockMetrics->getLabelLengthInLines()))
				{
					widestWidth = label->getWidthDots();
					longestLength = label->getLength();
					bestFitLabelNum = label->getLabelTypeNum();
					bestFitDeptNum = label->getDeptNum();
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("Selected label # ").append(Long::toString(bestFitLabelNum));
						Debug::trace("LabelTypeManager.Family", msgDebug2 + " is smaller than the label stock.");

						HobString msgDebug3 = msgDebug2;
						msgDebug3.append("  widestWidth = ").append(Integer::toString(widestWidth));
						msgDebug3.append("  longestLength = ").append(Integer::toString(longestLength));
						Debug::trace("LabelTypeManager.Family", msgDebug3);
					}
				}
			}
		}
		delete label;
		label = 0;
	}

	// If the labelTypeNum | deptNum passed in is not an exact match to the label stock, then try and find one that is closer, or is an exact match.
	if (!haveAnExactMatch)
	{
		try
		{
			for (searchNum = familyStart; searchNum < familyStart + 10; ++searchNum)
			{
				label = LabelType::findRecord(searchNum, currentDeptNum, dbAccess);
				if (label)
				{
					if (label->getErrorCode() == RECORD_OK)
					{
						if (label->getLabelStockDescriptor()->isExactMatch(printerStockMetrics) == true)
						{
							//	We have an exact match to the label stock. Stop now!
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("An exact match!!! searchNum = ").append(Long::toString(label->getLabelTypeNum())).append("  deptnum = ").append(Long::toString(label->getDeptNum()));
								Debug::trace("LabelTypeManager.Family", msgDebug2);
							}
							bestFitLabelNum = label->getLabelTypeNum();
							bestFitDeptNum = label->getDeptNum();
							haveAnExactMatch = true;
						}
						else
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("searchNum = ").append(Long::toString(searchNum)).append("\n");
								msgDebug2.append("  Label stock width = ").append(Integer::toString(printerStockMetrics->getLabelWidthInDots()));
								msgDebug2.append("  Label type width = ").append(Integer::toString(label->getWidthDots()));
								msgDebug2.append("  widestWidth = ").append(Integer::toString(widestWidth)).append("\n");
								msgDebug2.append("  Label stock length = ").append(Integer::toString(printerStockMetrics->getLabelLengthInLines()));
								msgDebug2.append("  Label type length = ").append(Integer::toString(label->getLength()));
								msgDebug2.append("  longestLength = ").append(Integer::toString(longestLength)).append("\n");
								Debug::trace("LabelTypeManager.Family", msgDebug2);
							}

							//	Step 1: Will this label type fit on the label stock?
							if ((label->getWidthDots() <= printerStockMetrics->getLabelWidthInDots()) && (label->getLength() <= printerStockMetrics->getLabelLengthInLines()))
							{
								//	Step 2: Is this label type bigger than the biggest label type so far?
								if ((label->getWidthDots() > widestWidth) || (label->getLength() > longestLength))
								{
									widestWidth = label->getWidthDots();
									longestLength = label->getLength();
									bestFitLabelNum = searchNum;
									bestFitDeptNum = label->getDeptNum();
									if (traceOn)
									{
										HobString msgDebug2 = msgDebug;
										msgDebug2.append("Best fit so far = ").append(Long::toString(bestFitLabelNum));
										msgDebug2.append("  widestWidth = ").append(Integer::toString(widestWidth));
										msgDebug2.append("  longestLength = ").append(Integer::toString(longestLength));
										Debug::trace("LabelTypeManager.Family", msgDebug2);
									}
								}
							}
						}
					}

					delete label;
					label = 0;
					if (haveAnExactMatch)
						break;
				}
			}
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", HobString("LabelTypeManager::findMatchInLabelFamily() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			return LabelWontFit;
		}
	}


/*  The following, commented-out code looks for a label type in the family differently; it first checks the family in the plu's dept first
    for an exact or best fit label before checking the default dept, and before checking the ROM labels.  James Meyer chose not to use this algorithm,
	instead wanting to go through the family using the normal lookup scheme that looks for the label type number in the plu's dept,
	then default dept, then ROM, and returns it wherever it exists in the hierarchy.

	I'm leaving this code here in case we ever want this alternative method, sinc it worked.



	// This method is changing due to the changes for the hierarchical structure of label types, as a result of the multi-department project.
	// To use the label families, we will look for the the best-fit label in the current dept first.  If a best-fit label isn't found in the
	// current dept, then we'll look for one in the family in the default dept (dept 0).  If one still isn't found, then we'll look in the
	// ROM labels db table.  Even if a better-fitting label type exists in the default dept, that is the same label type number as the one
	// we are starting with, or another in the family is a better fit in the default dept than the one found in the current dept, we will
	// use the best-fit label type found in the current dept.

	bool needToCheckAnotherDept = true;
	while (needToCheckAnotherDept)
	{
		//	If labelTypeNum is smaller than the current label stock, set up the current label dimensions as the best fit so far.
		//	If no other labelType is larger that will fit on the label stock, then labelTypeNum will be used.
		//		(Problems would occur if a family had labelTypes that were the same size but smaller than the current label stock.
		//		If labelTypes 1,2 & 3 were all the same size and labelType 2 was chosen, labelType 1 would be selected because
		//		it was the first best fit labelType found.)

		if (currentDeptNum == RECORD_NOT_FOUND)
			label = LabelType::findROMRecord(*labelTypeNum, dbAccess);
		else
			label = LabelType::findCustomRecord(*labelTypeNum, currentDeptNum, dbAccess);

		if (label)
		{
			if (label->getErrorCode() == RECORD_OK)
			{
				if (label->getLabelStockDescriptor()->isExactMatch(printerStockMetrics))
				{
					//	The labelTypeNum | deptNum passed in is an exact match to the label stock. Stop now!
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("An exact match!!! labelTypeNum = ").append(Long::toString(label->getLabelTypeNum())).append("  deptnum = ").append(Long::toString(label->getDeptNum()));
						Debug::trace("LabelTypeManager.Family", msgDebug2);
					}
					bestFitLabelNum = label->getLabelTypeNum();
					bestFitDeptNum = label->getDeptNum();
					haveAnExactMatch = true;
				}
				else
				{
					if ((label->getWidthDots() <= printerStockMetrics->getLabelWidthInDots()) && (label->getLength() <= printerStockMetrics->getLabelLengthInLines()))
					{
						widestWidth = label->getWidthDots();
						longestLength = label->getLength();
						bestFitLabelNum = label->getLabelTypeNum();
						bestFitDeptNum = label->getDeptNum();
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("Selected label # ").append(Long::toString(bestFitLabelNum));
							Debug::trace("LabelTypeManager.Family", msgDebug2 + " is smaller than the label stock.");

							HobString msgDebug3 = msgDebug2;
							msgDebug3.append("  widestWidth = ").append(Integer::toString(widestWidth));
							msgDebug3.append("  longestLength = ").append(Integer::toString(longestLength));
							Debug::trace("LabelTypeManager.Family", msgDebug3);
						}
					}
				}
			}
			delete label;
			label = 0;
		}

		if (!haveAnExactMatch)
		{
			try
			{
				for (searchNum = familyStart; searchNum < familyStart + 10; ++searchNum)
				{
					if (currentDeptNum == RECORD_NOT_FOUND)
						label = LabelType::findROMRecord(searchNum, dbAccess);
					else
						label = LabelType::findCustomRecord(searchNum, currentDeptNum, dbAccess);

					if (label)
					{
						if (label->getErrorCode() == RECORD_OK)
						{
							if (label->getLabelStockDescriptor()->isExactMatch(printerStockMetrics) == true)
							{
								//	We have an exact match to the label stock. Stop now!
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("An exact match!!! searchNum = ").append(Long::toString(searchNum));
									Debug::trace("LabelTypeManager.Family", msgDebug2);
								}
								bestFitLabelNum = searchNum;
								bestFitDeptNum = currentDeptNum;
								haveAnExactMatch = true;
							}
							else
							{
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("searchNum = ").append(Long::toString(searchNum)).append("\n");
									msgDebug2.append("  Label stock width = ").append(Integer::toString(printerStockMetrics->getLabelWidthInDots()));
									msgDebug2.append("  Label type width = ").append(Integer::toString(label->getWidthDots()));
									msgDebug2.append("  widestWidth = ").append(Integer::toString(widestWidth)).append("\n");
									msgDebug2.append("  Label stock length = ").append(Integer::toString(printerStockMetrics->getLabelLengthInLines()));
									msgDebug2.append("  Label type length = ").append(Integer::toString(label->getLength()));
									msgDebug2.append("  longestLength = ").append(Integer::toString(longestLength)).append("\n");
									Debug::trace("LabelTypeManager.Family", msgDebug2);
								}

								//	Step 1: Will this label type fit on the label stock?
								if ((label->getWidthDots() <= printerStockMetrics->getLabelWidthInDots()) && (label->getLength() <= printerStockMetrics->getLabelLengthInLines()))
								{
									//	Step 2: Is this label type bigger than the biggest label type so far?
									if ((label->getWidthDots() > widestWidth) || (label->getLength() > longestLength))
									{
										widestWidth = label->getWidthDots();
										longestLength = label->getLength();
										bestFitLabelNum = searchNum;
										bestFitDeptNum = currentDeptNum;
										if (traceOn)
										{
											HobString msgDebug2 = msgDebug;
											msgDebug2.append("Best fit so far = ").append(Long::toString(bestFitLabelNum));
											msgDebug2.append("  widestWidth = ").append(Integer::toString(widestWidth));
											msgDebug2.append("  longestLength = ").append(Integer::toString(longestLength));
											Debug::trace("LabelTypeManager.Family", msgDebug2);
										}
									}
								}
							}
						}

						delete label;
						label = 0;
						if (haveAnExactMatch)
							break;
					}
				}
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", HobString("LabelTypeManager::findMatchInLabelFamily() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				return LabelWontFit;
			}
		}

		if (haveAnExactMatch || bestFitLabelNum > 0 || currentDeptNum == RECORD_NOT_FOUND)
			needToCheckAnotherDept = false;
		else
		{
			if (currentDeptNum > 0)
				currentDeptNum = 0;
			else if (currentDeptNum == 0)
				currentDeptNum = RECORD_NOT_FOUND;
		}
	}
*/	

	LabelMediaReturn result = LabelWontFit;
	if (bestFitLabelNum > 0)
	{
		if (*labelTypeNum == bestFitLabelNum && *deptNum == bestFitDeptNum)
			result = LabelOk;
		else
		{
			*deptNum = bestFitDeptNum;
			*labelTypeNum = bestFitLabelNum;
			result = LabelCorrected;
		}
	}

	return result;

}


LabelType *LabelTypeManager::firstCustomLabelType()
{
	if (!dbAccess)
		return 0;

	DBLabelsRM *dbLabelsRM = dbAccess->getDBLabelsRM();
	if (!dbLabelsRM)
		return 0;

	// If the user calls firstCustomLabelType() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allLabelTypesQuery = dbLabelsRM->retrieveAll();
	if (!allLabelTypesQuery)
		return 0;

	DBQuery<DBLabel>::const_iterator tempIter = allLabelTypesQuery->begin();
	if (tempIter == allLabelTypesQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	* the iterator for the entire use of the query (which extends beyond this function call) (see nextCustomLabelType()
	* method call below).
	*/
	allLabelTypesIter = new DBQuery<DBLabel>::const_iterator(tempIter);
	DBLabel *dbRec = new DBLabel(*(*allLabelTypesIter));
	LabelType *rec = new LabelType(dbRec, dbAccess);
	return rec;
}

LabelType *LabelTypeManager::nextCustomLabelType()
{
	if (!dbAccess)
		return 0;

	if (allLabelTypesQuery && !allLabelTypesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allLabelTypesQuery || !allLabelTypesIter)
		return firstCustomLabelType();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	* If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	* the query, without needing to use the copy constructor on the iterator for each record in the query.
	*/
	++(*allLabelTypesIter);
	if (*allLabelTypesIter == allLabelTypesQuery->end())
	{
		destroyQueriesAndIters();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBRecall copy constructor in order for the Hfc Recall to
	* control the destruction of the DBRecall record that the iterator contains; otherwise, the DBRecall will
	* be deleted when the iterator goes out of scope.  The DBRecall copy will be deleted by the Recall record
	* created below.
	*/
	DBLabel *dbRec = new DBLabel(*(*allLabelTypesIter));
	LabelType *rec = new LabelType(dbRec, dbAccess);
	return rec;
}


void LabelTypeManager::destroyQueriesAndIters()
{
	if (allLabelTypesIter)
	{
		delete allLabelTypesIter;
		allLabelTypesIter = 0;
	}
	if (allLabelTypesQuery)
	{
		delete allLabelTypesQuery;
		allLabelTypesQuery = 0;
	}
}

void LabelTypeManager::destroyListAndIter()
{
	if (allLabelTypesListIter)
	{
		delete allLabelTypesListIter;
		allLabelTypesListIter = 0;
	}
	allLabelTypesList.clear();
}

void LabelTypeManager::destroyQueriesListAndIters()
{
	destroyQueriesAndIters();
	destroyListAndIter();
}
