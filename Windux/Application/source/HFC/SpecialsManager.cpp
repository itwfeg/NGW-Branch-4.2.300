/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SpecialsManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "GraphicFileNames.h"
#include "HobList.h"
#include "HfcError.h"
#include "Special.h"
#include "AllSpecials.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SpecialsManager::dataType = 0;
std::string SpecialsManager::dataTypeName("SpecialsManager"); 
#endif


SpecialsManager::SpecialsManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SpecialsManager::SpecialsManager(hfc::HFCManagerContainer *container)
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

SpecialsManager::~SpecialsManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentSpecial)
		delete currentSpecial;

	destroyQueriesAndIters();
}


void SpecialsManager::init()
{
	mySession = 0;

	currentSpecial = 0;

	allSpecialsQuery = 0;
	allSpecialsIter  = 0;
}


DBSession *SpecialsManager::getMySession()
{
	return mySession;
}
void SpecialsManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Special *SpecialsManager::firstSpecial()
{
	if (!dbAccess)
		return (Special *)NULL;

	DBSpecialProgramsRM *dbSpecialsRM = dbAccess->getDBSpecialsRM();
	if (!dbSpecialsRM)
		return (Special *)NULL;

	// If the user calls firstSpecial() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allSpecialsQuery = dbSpecialsRM->retrieveAll();

	// Couldn't get the query from which to access the Specials, just return false.
	if (!allSpecialsQuery)
		return (Special *)NULL;

	DBQuery<DBSpecialProgram>::const_iterator tempIter = allSpecialsQuery->begin();
	if (tempIter == allSpecialsQuery->end())
		return (Special *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextSpecial()
	 * method call below).
	 */
	allSpecialsIter = new DBQuery<DBSpecialProgram>::const_iterator(tempIter);
	DBSpecialProgram *sp = new DBSpecialProgram(*(*allSpecialsIter));
	Special *special = new Special(sp, dbAccess);
	return special;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstSpecial() method, the first call to this method will
 * result in the first record being returned.
 */
Special *SpecialsManager::nextSpecial()
{
	if (!dbAccess)
		return (Special *)NULL;

	if (allSpecialsQuery && !allSpecialsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allSpecialsQuery || !allSpecialsIter)
		return firstSpecial();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allSpecialsIter);
	if (*allSpecialsIter == allSpecialsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Specials in the db. */
		destroyQueriesAndIters();
		return (Special *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBSpecialProgram copy constructor in order for the Hfc Special to
	 * control the destruction of the DBSpecialProgram record that the iterator contains; otherwise, the DBSpecialProgram will
	 * be deleted when the iterator goes out of scope.  The DBSpecialProgram copy will be deleted by the Special record
	 * created below.
	 */
	DBSpecialProgram *sp = new DBSpecialProgram(*(*allSpecialsIter));
	Special *special = new Special(sp, dbAccess);
	return special;
}


/* Provide a countAll method so the caller doesn't need to know about a dbSpecialsRM.
 */
int SpecialsManager::countAllSpecials()
{
	if (!dbAccess)
		return -1;

	DBSpecialProgramsRM *dbSpecialsRM = dbAccess->getDBSpecialsRM();
	if (!dbSpecialsRM)
		return -1;

	return dbSpecialsRM->countAll();
}


Special *SpecialsManager::getCurrentSpecial()
{
	return currentSpecial;
}
void SpecialsManager::setCurrentSpecial(Special *newSpecial)
{
	if (currentSpecial)
		delete currentSpecial;
	currentSpecial = newSpecial;
}



void SpecialsManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than SpecialsDev.
	if (!msg)
		return;

	Special *msgSpecial = dynamic_cast<Special *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("SpecialsManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgSpecial);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgSpecial);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgSpecial);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgSpecial);
			break;

		case ReadAllGraphics:
			handleReadAllGraphicNames();
			break;

		default:
			Debug::trace("SpecialsManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}

}


void SpecialsManager::handleRead(Special *sp)
{
	if (!sp || !dbAccess)
		return;

	Special *lookedUpSpecial = Special::findRecord(sp->getSpecialNum(), sp->getDeptNum(), dbAccess);

	if (!lookedUpSpecial || lookedUpSpecial->getErrorCode() != RECORD_OK)
	{
		// Send error message to browser indicating Special not found.
		if (mySession)
			mySession->addDialogMessage(ErrSpecialsDev, SpecialsErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpSpecial)
			delete lookedUpSpecial;
		return;
	}

	// We have a new current Special.
	setCurrentSpecial(lookedUpSpecial);

	if (mySession)
	{
		SessionMessage writeSpecialMsg(SpecialsDev, DtWrite);
		writeSpecialMsg.setFormatType(BrowserFormat);
		writeSpecialMsg.setHfcObject(currentSpecial);

		mySession->send(&writeSpecialMsg, Messenger::DEST_UI);
	}
}


void SpecialsManager::handleReadAll()
{
	Debug::trace("SpecialsManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	AllSpecials all;
	all.findAll(dbAccess);

	SessionMessage allSpecialsMsg(SpecialsDev, DtWriteAll);
	allSpecialsMsg.setFormatType(BrowserFormat);
	allSpecialsMsg.setHfcObject(&all);
	mySession->send(&allSpecialsMsg, Messenger::DEST_UI);
}


void SpecialsManager::handleWrite(Special *sp)
{
	if ((!sp) || (!currentSpecial))
		return;

  	currentSpecial->updateRecord(sp);
}


void SpecialsManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Special *msgSpecial = dynamic_cast<Special *> (msg->getHfcObject());
	if (!msgSpecial || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialPrograms))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Special *existingSpecial = Special::findRecord(msgSpecial->getSpecialNum(), msgSpecial->getDeptNum(), dbAccess);
		if (existingSpecial && existingSpecial->getSpecialNum() != RECORD_NOT_FOUND)
		{
			existingSpecial->deleteRecord();
			Debug::trace("SpecialsManager", HobString("handleDelete - Special deleted - specialNum:") + longtostring(msgSpecial->getSpecialNum()) + HobString("  deptNum:") + longtostring(msgSpecial->getDeptNum()));
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				if (existingSpecial->getSpecialNum() == currentSpecial->getSpecialNum())
				{
					delete currentSpecial;
					currentSpecial = (Special*) NULL;
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialPrograms))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingSpecial, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}
		}
		else
			Debug::trace("SpecialsManager", HobString("handleDelete - special to be deleted was not in the database to delete - special num:") +
														HobString((existingSpecial) ? Long::toString((long)msgSpecial->getSpecialNum()) : "???"));

		if (existingSpecial)
			delete existingSpecial;
	}
}


void SpecialsManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void SpecialsManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("SpecialsManager", "handleDeleteAll - deleting all specials");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialPrograms))
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

		DBSpecialProgramsRM *dbRM = dbAccess->getDBSpecialsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		Debug::trace("SpecialsManager", "handleDeleteAll - will create reserved records next");
		Special::verifyReservedRecords(dbAccess);
	}

	Debug::trace("SpecialsManager", "handleDeleteAll - leaving");

	// How does this affect currentSpecial???
}


void SpecialsManager::handleUpdateAndSave(Special *modifiedSpecial)
{
	Debug::trace("SpecialsManager", HobString("handleUpdateAndSave(): currentSpecial = ") + HobString(Hex::toString((long long) currentSpecial)));
	if (currentSpecial)
	{
		if (modifiedSpecial)
		  	currentSpecial->updateRecord(modifiedSpecial);
		handleSaveRecord(currentSpecial);
	}
}

void SpecialsManager::handleSaveRecord(Special *recordToSave)
{
	Debug::trace("SpecialsManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialPrograms))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void SpecialsManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Special *restoreSp = dynamic_cast<Special *> (msg->getHfcObject());
	if (!restoreSp || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialPrograms))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreSp->setDBAccess(dbAccess);

		Special *existingSp = Special::findRecord(restoreSp->getSpecialNum(), restoreSp->getDeptNum(), dbAccess);
		if (existingSp)
		{
			if (existingSp->isRecordValid())
			{
				existingSp->updateRecord(restoreSp);
				existingSp->saveRecord();
			}
			else
			{
				existingSp->deleteRecord();
				restoreSp->saveRecord();
			}

			delete existingSp;
		}
		else
			restoreSp->saveRecord();
	}
}


void SpecialsManager::handleCreate(Special *sp)
{
	if (!sp)
		return;

	Special *newSpecial = new Special();

	if (newSpecial)
	{
		newSpecial->setDeptNum(sp->getDeptNum());
		newSpecial->setSpecialNum(sp->getSpecialNum());
#if 0
		newSpecial->setSpecialDescription("Sale");
		newSpecial->setImageName1("None");
		newSpecial->setImageName2("None");
#endif
		newSpecial->setErrorCode(RECORD_OK);
		newSpecial->setDBAccess(dbAccess);
		handleSaveRecord(newSpecial);

		//	Set this new product as the current product.
		setCurrentSpecial(newSpecial);

		if (mySession)
		{
			// Send the new product to the Browser.
			SessionMessage spData(SpecialsDev, DtWrite);
			spData.setFormatType(BrowserFormat);
			spData.setHfcObject(currentSpecial);

			// Send the product to the UI (Browser)
			mySession->send(&spData, Messenger::DEST_UI);
		}
	}
}

void SpecialsManager::handleReadAllGraphicNames()
{
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(Graphic::TYPE_SPECIALSGRAPHIC);
	if (gfn)
	{
		if (gfn->getListCount() > 0)
		{
			// Send the Graphic file names found back to the Browser.
			SessionMessage nameData(SpecialsDev, ReadAllGraphics);
			nameData.setFormatType(BrowserFormat);
			nameData.setHfcObject(gfn);

			mySession->send(&nameData, Messenger::DEST_UI);
		}
		else
		{
			//	Send a message indicating no files were found in this directory.
		}
		delete gfn;
	}
}

void SpecialsManager::destroyQueriesAndIters()
{
	if (allSpecialsIter)
	{
		delete allSpecialsIter;
		allSpecialsIter = (DBQuery<DBSpecialProgram>::const_iterator *)NULL;
	}

	if (allSpecialsQuery)
	{
		delete allSpecialsQuery;
		allSpecialsQuery = (DBQuery<DBSpecialProgram> *)NULL;
	}
}
