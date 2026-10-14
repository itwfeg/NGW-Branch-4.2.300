/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "OperatorManager.h"
#include "OperatorNoteManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "ConfigurationEvent.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "OperationModeService.h"
#include "HobList.h"
#include "AllOperators.h"
//#include "HobAllOpChanges.h"
#include "WriteSingleField.h"
#include "GenericHfcObject.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBManager.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"
#include "BatchManager.h"
#include "Batch.h"

#include "HandsFreeDeviceEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatorManager::_dataType_ = 0;
std::string OperatorManager::_dataTypeName_("OperatorManager"); 
#endif


OperatorManager::OperatorManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

OperatorManager::OperatorManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

OperatorManager::~OperatorManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	finishedProcessingChanges();

	if (loggedInOperator)
		delete loggedInOperator;

	if (currentOperator)
		delete currentOperator;

	if (idList)
		delete idList;

	if (accessLevelList)
		delete accessLevelList;

	if (nameList)
		delete nameList;

	if (languageList)
		delete languageList;

	destroyQueriesAndIters();
}


void OperatorManager::init()
{
	mySession = 0;

	loggedInOperator = 0;
	currentOperator = 0;

	allOperatorsQuery = 0;
	allOperatorsIter  = 0;

//	allOpChangesQuery = 0;
//	allOpChangesIter  = 0;

	idList = 0;
	accessLevelList = 0;
	nameList = 0;
	languageList = 0;
}


DBSession *OperatorManager::getMySession()
{
	return mySession;
}
void OperatorManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
HobOperator *OperatorManager::firstOperator()
{
	if (!dbAccess)
		return (HobOperator *)NULL;

	edb::DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
	if (!dbOperatorsRM)
		return (HobOperator *)NULL;

	// If the user calls firstOperator() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allOperatorsQuery = dbOperatorsRM->retrieveAll();

	// Couldn't get the query from which to access the Operators, just return false.
	if (!allOperatorsQuery)
		return (HobOperator *)NULL;

	edb::DBQuery<edb::DBOperator>::const_iterator tempIter = allOperatorsQuery->begin();
	if (tempIter == allOperatorsQuery->end())
		return (HobOperator *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextOperator()
	 * method call below).
	 */
	allOperatorsIter = new edb::DBQuery<edb::DBOperator>::const_iterator(tempIter);
	edb::DBOperator *o = new edb::DBOperator(*(*allOperatorsIter));
	HobOperator *oper = new HobOperator(o, dbAccess);
	return oper;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstOperator() method, the first call to this method will
 * result in the first record being returned.
 */
HobOperator *OperatorManager::nextOperator()
{
	if (!dbAccess)
		return (HobOperator *)NULL;

	if (allOperatorsQuery && !allOperatorsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allOperatorsQuery || !allOperatorsIter)
		return firstOperator();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allOperatorsIter);
	if (*allOperatorsIter == allOperatorsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Operators in the db. */
		destroyQueriesAndIters();
		return (HobOperator *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBOperator copy constructor in order for the Hfc HobOperator to
	 * control the destruction of the DBOperator record that the iterator contains; otherwise, the DBOperator will
	 * be deleted when the iterator goes out of scope.  The DBOperator copy will be deleted by the HobOperator record
	 * created below.
	 */
	edb::DBOperator *o = new edb::DBOperator(*(*allOperatorsIter));
	HobOperator *oper = new HobOperator(o, dbAccess);
	return oper;
}


/* Provide a countAll method so the caller doesn't need to know about a dbOperatorsRM.
 */
int OperatorManager::countAllOperators()
{
	if (!dbAccess)
		return -1;

	edb::DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
	if (!dbOperatorsRM)
		return -1;

	return dbOperatorsRM->countAll();
}


/**
 * Allows anyone to get the currently logged in operator.
 * Note:  The currently logged in operator has scope to the Session;
 * in other words, each Session, local and remote, can have a different
 * logged in user.
 */
HobOperator *OperatorManager::getCurrentOperator()
{
	return currentOperator;
}
void OperatorManager::setCurrentOperator(HobOperator *newOp)
{
	if (currentOperator)
		delete currentOperator;

	currentOperator = newOp;

	if (Debug::isTracing("OperatorManager"))
		Debug::trace("OperatorManager", "setCurrentOperator():  currentOperator = " + ptrtostring(currentOperator));
}

HobOperator *OperatorManager::getLoggedInOperator()
{
	return loggedInOperator;
}
void OperatorManager::setLoggedInOperator(HobOperator *newOp)
{
	if (loggedInOperator)
		delete loggedInOperator;
	loggedInOperator = newOp;
}


void OperatorManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than OperatorDev.
	if (!msg)
		return;

	HobOperator * msgOperator = dynamic_cast<HobOperator *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("OperatorManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgOperator);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgOperator);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgOperator);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgOperator);
			break;

		case OpLogin:
			handleLogin(msgOperator);
			break;

		case OpLoginRFID:
			handleLoginRFID(msgOperator);
			break;

		case OpLogout:
		case OpLogoutRFID:
			handleLogout();
			break;

		case OpDoesRFIDExist:
			checkIfRFIDExists(msg);
			break;

		case OpDoesIDExist:
			checkIfIDExists(msg);
			break;

		case OpPromptForIDOrRFID:
		{
			//	HA-185: If we receive this from the UI, send it back to force a prompt for an Operator ID.
			msg->setFormatType(BrowserFormat);
			mySession->send(msg, Messenger::DEST_UI);
			break;
		}

		case OpReadNote:
		case OpReadNoteForEditing:
		case OpWriteNote:
		case OpDeleteNote:
		case OpSaveNote:
		case OpRestoreNote:
			mgrContainer_->getOperatorNoteManager()->handleMessage(msg);
			break;

		case OpRenumberOperator:
		{
			GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
			if (msgData)
				renumberCurrentOperator(msgData->getData());
			break;
		}

		default:
			Debug::trace("OperatorManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}

	/* Any of these events could possibly change the requirement that the operator login.
	 * So, just assume that the 'login required' flags need to be updated in the browser.
	 * If the event is DtRestore, then we are a comm session or a backup/restore session, so we can just
	 * save the fact that the operators db file has been updated, and send the CfgSaved message at the
	 * end (when the comm session or backup/restore session is deleted).
	 */
	if ( mySession && !mySession->isPrimarySession() && !mySession->isRemoteSession() )
	{
		mySession->setCommSessionOperatorReload();		
	}
	else if (mySession && (event == DtWrite || event == DtDelete || event == DtDeleteAll || event == DtSave))
	{
		// This section handles any message that comes from the UI that affects the operators db file.
		// We can't assume that any additional message will follow that will affect the operators db file,
		// thus, possibly affecting the login required flags, so just dispatch and handle the message that
		// will refresh the configuration in the browser (madp).
		
		SessionMessage msg(ConfigurationDev, CfgSaved);
		if (mgrContainer_)
			mgrContainer_->dispatchMessage(&msg);

		// Because with the Windows version, you can't send a message to yourself,
		// just tell my Session to handle the message that was sent to everyone else.
		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
			s->handleMessage(&msg);

		// Just In case the handsfree device id was updated for any operator, dispatch a message to the
		// SESSIONTYPE_HANDSFREEDEVICEMGR Session so it can update the hfpdd daemon with a new opName/OpRFID list
		if (mgrContainer_)
		{
			EvolMessage hfmsg(HandsFreeDevice, HFDSendOpNameList);
			mgrContainer_->dispatchMessage(&hfmsg);
		}
	}
}


void OperatorManager::handleRead(HobOperator *op)
{
	if (!op || !dbAccess)
		return;

	HobOperator *lookedUpOperator = HobOperator::find(op->getOpNum(), dbAccess);

	if (!lookedUpOperator || lookedUpOperator->getErrorCode() != RECORD_OK)
	{
		if (mySession)
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		if (lookedUpOperator)
			delete lookedUpOperator;
		return;
	}
	// We have a new current operator.
	setCurrentOperator(lookedUpOperator);

	if (mySession)
	{
		SessionMessage writeOperMsg(OperatorDev, DtWrite);
		writeOperMsg.setFormatType(BrowserFormat);
		currentOperator->setFormatType(OperatorMaintenanceFormat);
		writeOperMsg.setHfcObject(currentOperator);

		mySession->send(&writeOperMsg, Messenger::DEST_UI);
	}
}


void OperatorManager::handleReadAll()
{
	if (!mySession || !dbAccess)
		return;

	AllOperators allOperators;
	allOperators.findAll(dbAccess);

	SessionMessage allOpersMsg(OperatorDev, DtWriteAll);
	allOpersMsg.setFormatType(BrowserFormat);
	allOpersMsg.setHfcObject(&allOperators);
	mySession->send(&allOpersMsg, Messenger::DEST_UI);
}


void OperatorManager::handleWrite(HobOperator *op)
{
	if ((!op) || (!currentOperator))
		return;

  	currentOperator->updateOperator(op);
}


void OperatorManager::renumberCurrentOperator(const HobString &newOperatorNumber)
{
	if (!currentOperator || newOperatorNumber.size() == 0)
		return;

	// Check if an operator already exists with that number.
	HobOperator *existingOp = HobOperator::find(HobString(newOperatorNumber), dbAccess);
	if (existingOp)
	{
		delete existingOp;
		if (mySession)
			mySession->addDialogMessage(ErrOperatorDev, OperatorErrOpNumAlreadyExists, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	// Create a new HobOperator.
	HobOperator *newOp = new HobOperator(*currentOperator);
	newOp->setOpNum(newOperatorNumber);
	newOp->setDBAccess(dbAccess);

	// Delete the old HobOperator.
	SessionMessage delOper(OperatorDev, DtDelete);
	delOper.setHfcObject(currentOperator);
	handleDelete(&delOper);

	//	Set this new operator as the current operator.
	setCurrentOperator(newOp);
	handleSaveRecord(newOp);

	// Reread the list of operators for the table in the UI.
	handleReadAll();

	// Send the new operator data back to the UI as the new current operator.
	if (mySession)
	{
		// Send the new operator to the Browser.
		SessionMessage opData(OperatorDev, DtWrite);
		opData.setFormatType(BrowserFormat);
		currentOperator->setFormatType(OperatorMaintenanceFormat);
		opData.setHfcObject(currentOperator);
		mySession->send(&opData, Messenger::DEST_UI);
	}
}


void OperatorManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	HobOperator * msgOperator = dynamic_cast<HobOperator *> (msg->getHfcObject());
	if (!msgOperator || !dbAccess)
		return;

	HobString opNumToDelete = msgOperator->getOpNum();

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		HobOperator *existingOp = HobOperator::find(msgOperator->getOpNum(), dbAccess);
		if (existingOp)
		{
			if (existingOp->getOpNum() == opNumToDelete)
			{
				existingOp->deleteRecord();

				if (currentOperator && existingOp->getOpNum() == currentOperator->getOpNum())
				{
					setCurrentOperator((HobOperator*) NULL);

					// todo - is this sufficient to delete the current operator?  Should the UI be notified that the current
					// operator no longer exists - thus they shouldn't be allowed to run any more packages?
				}
				Debug::trace("OperatorManager", HobString("handleDelete - operator deleted - opnum:") + opNumToDelete);

				if (msg->getEventType() != DtBatchApplyDelete)
				{
					// if received via UI, delete it and batch the record being deleted.
					if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, existingOp, getDeleteEvent());

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
								hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
					}
				}
			}

			delete existingOp;
		}
		else
			Debug::trace("OperatorManager", HobString("handleDelete - operator to be deleted was not in the database to delete - opnum:") + opNumToDelete);
	}
}


void OperatorManager::handleDeleteAll()
{
	Debug::trace("OperatorManager", "handleDeleteAll - deleting all operators");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
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

		DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
		if (!dbOperatorsRM)
			return;

		setLoggedInOperator((HobOperator *)NULL);
		setCurrentOperator((HobOperator*) NULL);

		dbOperatorsRM->deleteAll();

		destroyQueriesAndIters();
	}
	// todo - Should operator changes be deleted as well??

	Debug::trace("OperatorManager", "handleDeleteAll - leaving");
}


void OperatorManager::handleUpdateAndSave(HobOperator *modifiedOper)
{
	Debug::trace("OperatorManager", "handleUpdateAndSave(): currentOperator = " + ptrtostring(currentOperator));

	if (currentOperator)
	{
		if (modifiedOper)
		  	currentOperator->updateOperator(modifiedOper);
		handleSaveRecord(currentOperator);
	}
}

void OperatorManager::handleSaveRecord(HobOperator *recordToSave)
{
	Debug::trace("OperatorManager", "handleSaveRecord(): recordToSave = " + ptrtostring(recordToSave));

	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, currentOperator);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void OperatorManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	HobOperator * restoreOp = dynamic_cast<HobOperator *> (msg->getHfcObject());
	if (!restoreOp || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Operators))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreOp);

			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
		}
		else
			receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

		// batch the record being restored; don't actually write it to the database right now.
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreOp->setDBAccess(dbAccess);

		HobOperator *existingOp = HobOperator::find(restoreOp->getOpNum(), dbAccess);
		if (existingOp)
		{
			if (existingOp->isRecordValid())
			{
				existingOp->updateOperator(restoreOp);
				existingOp->saveRecord();
			}
			else
			{
				existingOp->deleteRecord();
				restoreOp->saveRecord();
			}

			delete existingOp;
		}
		else
			restoreOp->saveRecord();
	}
}


void OperatorManager::handleCreate(HobOperator *op)
{
	if (!op || !dbAccess)
		return;

	HobOperator *newOperator = new HobOperator();
	if(newOperator)
	{
		newOperator->setOpNum(op->getOpNum());
		newOperator->setOpRFID(op->getOpRFID());
		newOperator->setErrorCode(RECORD_OK);
		newOperator->setDBAccess(dbAccess);

		//	Set this new operator as the current operator.
		setCurrentOperator(newOperator);

		if (mySession)
		{
			// Send the new operator to the Browser.
			SessionMessage opData(OperatorDev, DtWrite);
			opData.setFormatType(BrowserFormat);
			currentOperator->setFormatType(OperatorMaintenanceFormat);
			opData.setHfcObject(currentOperator);
			mySession->send(&opData, Messenger::DEST_UI);
		}
	}
}


void OperatorManager::handleLogin(HobOperator *op)
{
	Debug::trace("OperatorManager", "handling LOGIN Message");

	Configuration *config = Configuration::getInstance();

	if (!op || !config || !mySession)
		return;

	HobOperator *loggingInOp = (HobOperator *)NULL;

	/* Check if logging into Service Mode, since the Service userid is not kept in the database with the other users. */
	if (op->getCurrentMode() == HobOperator::CURRENTMODE_SERVICE)
	{
		if (op->getOpNum() != config->getServiceLoginId())
		{
			Debug::trace("OperatorManager", "Invalid Service ID passed in when trying to login to Service Mode.");

			if (mySession)
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrInvalidServiceLoginId, HfcResponse::RESPONSESTYLE_OK);
			return;
		}
	}
	else
	{
		/* If on the login screen, and all the operators are deleted from the database, allow the user
		 * to login using any operator ID.  But still have to send a message back to the browser to get
		 * it to continue on to the Operate or Supervisor page.
		 */

		//	if we are not the primary session and remoteAccess requires a login via OperatorID, they MUST enter a valid ID regardless.
		//	If there are no Operator ID's, then they cannot use the scale remotely. [CCD - MAR 2011]
		if (!mySession->isPrimarySession() && 
			(!config->toBoolCfg(fieldOffsetCfgRemoteAccessPermitted) || 
			 ( (config->toBoolCfg(fieldOffsetCfgRemoteAccessRequiresLogin) || config->toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) == RemoteAccessLoginMethod_OperatorID) && countAllOperators() == 0 )
			)
		   )
		{
			mySession->addDialogMessage(ErrOperatorDev, OperatorErrLoginNotFound, HfcResponse::RESPONSESTYLE_OK);
			return;
		}

		bool loginDummyOperator = false;

		if ( countAllOperators() == 0 || (op->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR && !isSupervisorLoginRequired(dbAccess)) )
		{
			setLoggedInOperator((HobOperator *)NULL);

			/* The Browser needs a response in order to continue on into the next screens;
			 * to build the appropriate menus.  So send back the operator, defaulting
			 * the name to be blank, and access level 9.
			 */
			SessionMessage response(OperatorDev, OpLogin);
			response.setFormatType(BrowserFormat);

			op->setOpName("");
			op->setOpLang("");
			op->setAccessLevel(9);
			op->setSawNote(true);
			response.setHfcObject(op);

			mySession->send(&response, Messenger::DEST_UI);
			return;
		}

		if (dbAccess)
			loggingInOp = HobOperator::find(op->getOpNum(), dbAccess);
		if (!dbAccess || !loggingInOp || loggingInOp->getErrorCode() != RECORD_OK)
		{
			// operator is not in the database (or db error occurred)
			// Don't allow login to continue.
			// Send error message to browser indicating operator_not_found.
			if (config->isRFIDHWPresent())
				setLoggedInOperator((HobOperator *)NULL);
			if (mySession)
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrLoginNotFound, HfcResponse::RESPONSESTYLE_OK);
			return;
		}
		if (op->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR && loggingInOp->getAccessLevel() == 1)
		{
			// operators with access level of 1 are not allowed into supervisor
			// Don't allow login to continue. Send error message to browser.
			if (mySession)
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrAccessNotAllowed, HfcResponse::RESPONSESTYLE_OK);
			return;
		}
	}

	/* If someone already logged in, write a WARNING log message that a
	 * logout message was not received before another operator is logging in.
	 * Automatically logout the previous operator, then log the new one in.
	 * (as opposed to just not allowing the new operator to log in).
	 */
	handleLogout();

	//SessionMessage keypadOff(SessionDev, SnDisplayingModalPopupKeyPadOff);
	//keypadOff.setFormatType(BrowserFormat);
	//mySession->send(&keypadOff, Messenger::DEST_UI);

	if (op->getCurrentMode() == HobOperator::CURRENTMODE_SERVICE)
	{
		if (mySession)
		{
			/* The Browser needs a response in order to continue on into the Service screens;
			 * to build the appropriate menus.  So send back the operator (ServiceID), defaulting
			 * the name to be the same as the ID, and access level 9.  In the future, we could have
			 * different service logins with different levels.
			 */
			SessionMessage response(OperatorDev, OpLogin);
			response.setFormatType(BrowserFormat);

			op->setOpName(op->getOpNum());
			op->setAccessLevel(9);
			response.setHfcObject(op);

			mySession->send(&response, Messenger::DEST_UI);
		}
	}
	else
	{
		setLoggedInOperator(loggingInOp);
		loggedInOperator->setLoggedIn(true);
		loggedInOperator->setLoginMethod(HobOperator::LOGINMETHOD_OPNUM);
		loggedInOperator->setCurrentMode(op->getCurrentMode());

		// write LOGIN message.
		if (Debug::isTracing("OperatorManager"))
		{
			HobString myDebug("handleLogin():");
			myDebug.append("   Operator logged in = ").append(loggedInOperator->toString());
			myDebug.append("   OperatorID = ").append(loggedInOperator->getOpNum());
			Debug::trace("OperatorManager", myDebug);
		}

		// todo - finish anything else needing done when an operator logs in?
		if (mySession)
		{
			SessionMessage response(OperatorDev, OpLogin);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(loggedInOperator);

			mySession->send(&response, Messenger::DEST_UI);

			if (loggedInOperator->getCurrentMode() == HobOperator::CURRENTMODE_OPERATE &&
				 !loggedInOperator->hasSeenNote())
			{
				SessionMessage readNoteMsg(OperatorDev, OpReadNote);
				mgrContainer_->getOperatorNoteManager()->handleMessage(&readNoteMsg);
			}
		}
	}
}


void OperatorManager::handleLoginRFID(HobOperator *op)
{
	Debug::trace("OperatorManager", "handling OpLoginRFID Message");

	if (!op || !dbAccess)
		return;

	// Can't login to Service mode using an RFID tag.
	if (op->getCurrentMode() == HobOperator::CURRENTMODE_SERVICE)
		return;

	// Don't allow logging into Supervisor if someone is already in Supervisor and Supervisor login is required.
	// Just ignore this rfid tag.
	if (op->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR &&
		 loggedInOperator && loggedInOperator->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR &&
		 isSupervisorLoginRequired(dbAccess))
		return;

	// If the rfid tag of the user trying to login, is the same as the currently logged in user, then just skip the login; it's the same person.
	if (loggedInOperator && loggedInOperator->getOpRFID().size() && loggedInOperator->getOpRFID() == op->getOpRFID())
		return;

	HobOperator *loggingInOp = (HobOperator *)NULL;
	loggingInOp = HobOperator::findRFID(op->getOpRFID(), dbAccess);
	if (!loggingInOp || loggingInOp->getErrorCode() != RECORD_OK)
	{
		// Operator is not in the database (or db error occurred)
		// We need to create an operator with this RFID.
		if (!Configuration::getInstance()->isAutoCreateRFIDOperator())
		{
			// Scale is configured not to automatically create new operators when a new RFID tag is detected for the first time.
			// Send an error dialog to the scale indicating an unidentified RFID has been detected.
			if (mySession)
			{
				HobList operText;
				operText.push_back(op->getOpRFID());
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrRFIDOperatorNotFound, HfcResponse::RESPONSESTYLE_OK, &operText);
			}
			if (loggingInOp)
				delete loggingInOp;
			return;
		}
		else
		{
			if (loggingInOp)
			{
				delete loggingInOp;
				loggingInOp = 0;
			}

			loggingInOp = new HobOperator();
			if (loggingInOp)
			{
				loggingInOp->setDBAccess(dbAccess);
				loggingInOp->setOpNum(op->getOpRFID());
				// pre-db  loggingInOp->setOpNum(op->getOpRFID().substr(12,4));
				loggingInOp->setOpRFID(op->getOpRFID());
				loggingInOp->setAccessLevel(1);
				loggingInOp->setOpLang(op->getOpLang());
				loggingInOp->setErrorCode(RECORD_OK);
				handleSaveRecord(loggingInOp);
			}
			else
				return;
		}
	}

	// If the operator is trying to login to supervisor with an access level of 1 when supervisor login is required, don't let them in.
	// If supervisor login isn't required, and the user has access level of 1, it's ok to login to supervisor mode with rfid.
	if (op->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR && loggingInOp->getAccessLevel() == 1 && isSupervisorLoginRequired(dbAccess))
	{
		// operators with access level of 1 are not allowed into supervisor, when supervisor login is required.
		// Don't allow login to continue. Send error message to browser.
		if (mySession)
			mySession->addDialogMessage(ErrOperatorDev, OperatorErrAccessNotAllowed, HfcResponse::RESPONSESTYLE_OK);
		if (loggingInOp)
		{
			delete loggingInOp;
			loggingInOp = 0;
		}
		return;
	}

	/* If someone already logged in, write a WARNING log message that a
	 * logout message was not received before another operator is logging in.
	 * Automatically logout the previous operator, then log the new one in.
	 * (as opposed to just not allowing the new operator to log in).
	 */
	handleLogout();

	SessionMessage keypadOff(SessionDev, SnDisplayingModalPopupKeyPadOff);
	keypadOff.setFormatType(BrowserFormat);
	mySession->send(&keypadOff, Messenger::DEST_UI);

	setLoggedInOperator(loggingInOp);
	loggedInOperator->setLoggedIn(true);
	loggedInOperator->setLoginMethod(HobOperator::LOGINMETHOD_RFID);
	loggedInOperator->setCurrentMode(op->getCurrentMode());

	// write LOGIN message.
	if (Debug::isTracing("OperatorManager"))
	{
		HobString myDebug("handleLoginRFID():");
		myDebug.append("   Operator logged in = ").append(loggedInOperator->toString());
		myDebug.append("   OperatorID = ").append(loggedInOperator->getOpNum());
		Debug::trace("OperatorManager", myDebug);
	}

	// todo - finish anything else needing done when an operator logs in?
	if (mySession)
	{
		SessionMessage response(OperatorDev, OpLogin);
		response.setFormatType(BrowserFormat);
		response.setHfcObject(loggedInOperator);

		mySession->send(&response, Messenger::DEST_UI);

		if (loggedInOperator->getCurrentMode() == HobOperator::CURRENTMODE_OPERATE && !loggedInOperator->hasSeenNote())
		{
			SessionMessage readNoteMsg(OperatorDev, OpReadNote);
			mgrContainer_->getOperatorNoteManager()->handleMessage(&readNoteMsg);
		}
	}
}


void OperatorManager::handleLogout()
{
	Debug::trace("OperatorManager", "handling LOGOUT Message");

	if (!loggedInOperator || !loggedInOperator->isLoggedIn() || !dbAccess)
		return;

	// write LOGOUT message.
	Debug::trace("OperatorManager", HobString("Operator logged out: ") + HobString(loggedInOperator->toString()));

	// todo - finish any undone business with the current operator?

	setLoggedInOperator((HobOperator *)NULL);

	if (!mySession)
		Debug::trace("OperatorManager", "no seesion for LOGOUT Message");
	else
	{
		Debug::trace("OperatorManager", "sending LOGOUT Message to UI");

		SessionMessage clearOperMsg(OperatorDev, OpLogin);
		clearOperMsg.setFormatType(BrowserFormat);
		HobOperator emptyOper;
		clearOperMsg.setHfcObject(&emptyOper);
		mySession->send(&clearOperMsg, Messenger::DEST_UI);
	}
}


void OperatorManager::checkIfRFIDExists(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	bool duplicate = false;
	GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (msgdata)
	{
		HobOperator *oper = HobOperator::findRFID(msgdata->getData(), dbAccess);
		if (oper)
		{
			if (!currentOperator || currentOperator->getOpNum() != oper->getOpNum())
				duplicate = true;
		}

		if (duplicate)
		{
/*
Do not send error message anymore... UI is determine to prompt user or not about this RFID... not the backend code.

			HobList operText;
			operText.setQuotingElements(true);
			HobString op = HobString(oper->getOpNum()) + HobString(" - ") + oper->getOpName();
			operText.push_back(op);

			mySession->addDialogMessage(ErrOperatorDev, OperatorErrOpRFIDAlreadyExists, HfcResponse::RESPONSESTYLE_OK, &operText);
*/
			SessionMessage existsMsg(OperatorDev, OpRFIDExists);
			existsMsg.setFormatType(BrowserFormat);
			mySession->send(&existsMsg, Messenger::DEST_UI);
		}
		else
		{
			// operator not found with that rfid, send message to browser that it's ok to use that one
			SessionMessage notexistsMsg(OperatorDev, OpRFIDDoesntExist);
			notexistsMsg.setFormatType(BrowserFormat);
			mySession->send(&notexistsMsg, Messenger::DEST_UI);
		}
	}
}

void OperatorManager::checkIfIDExists(SessionMessage *msg)
{
	if (!msg || !dbAccess)
		return;

	GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (msgdata)
	{
		HobOperator *lookedUpOperator = HobOperator::find(msgdata->getData(), dbAccess);

		if (!lookedUpOperator || lookedUpOperator->getErrorCode() != RECORD_OK)
		{
			// operator not found with that id, send message to browser that it's ok to use that one
			SessionMessage notexistsMsg(OperatorDev, OpIDDoesntExist);
			notexistsMsg.setFormatType(BrowserFormat);
			mySession->send(&notexistsMsg, Messenger::DEST_UI);
		}
		else
		{
			SessionMessage existsMsg(OperatorDev, OpIDExists);
			existsMsg.setFormatType(BrowserFormat);
			mySession->send(&existsMsg, Messenger::DEST_UI);
		}
	}
}

void OperatorManager::destroyQueriesAndIters()
{
	if (allOperatorsIter)
	{
		delete allOperatorsIter;
		allOperatorsIter = (DBQuery<DBOperator>::const_iterator *)NULL;
	}

	if (allOperatorsQuery)
	{
		delete allOperatorsQuery;
		allOperatorsQuery = (DBQuery<DBOperator> *)NULL;
	}
}


// static
bool OperatorManager::isSupervisorLoginRequired()
{
    DBManager *myDatabase = DBManager::getInstance();
    if (!myDatabase)
		return false;

    bool rc( false );

	try
	{
		DBOperatorsRM *opRM = myDatabase->createOperators();
		if ( checkSupervisorLogin(opRM) )
			rc = true;
		delete opRM;
	}
	catch (...)
	{
	}
	return rc;
}

// static
bool OperatorManager::isSupervisorLoginRequired(DBManager *myDatabase)
{
    //DBManager *myDatabase = DBManager::getInstance();
    if (!myDatabase)
		return false;

	try
	{
		DBOperatorsRM *opRM = myDatabase->createOperators();
		return checkSupervisorLogin(opRM);
	}
	catch (...)
	{
	}
	return false;
}

// static
bool OperatorManager::isSupervisorLoginRequired(DBAccess *dbAccess)
{
 	if (!dbAccess)
		return false;

	try
	{
		DBOperatorsRM *opRM = dbAccess->getDBOperatorsRM();
		return checkSupervisorLogin(opRM);
	}
	catch (...)
	{
	}
	return false;
}

// static
bool OperatorManager::checkSupervisorLogin(DBOperatorsRM *opRM)
{
    if (!opRM)
		return false;

	bool result = false;
	try
	{
		DBQuery<DBOperator> *opQuery = opRM->retrieveAll();
		if (opQuery)
		{
			DBQuery<DBOperator>::const_iterator iter = opQuery->begin();
			while (iter != opQuery->end())
			{
				DBOperator *o = new DBOperator(*iter);
				if (o)
				{
				 	if (o->isRecordValid() && o->getClassification() == HobOperator::ACCESSLEVEL_SUPERVISOR)
						result = true;
					delete o;
					if (result)
						break;
				}
				iter++;
			}
			delete opQuery;
		}
	}
	catch (...)
	{
	}
	return result;
}

