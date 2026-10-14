/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTextCfgManager.h"
#include "ProductTextCfgEvent.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "HfcUpdateStatus.h"

#include "SessionManager.h"
//#include "ScaleSynchronizationType.h"
//#include "SynchronizationManagerProxy.h"
//#include "SynchronizationChange.h"
#include "CSmartPtr.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductTextCfgsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTextCfgManager::_dataType_ = 0;
std::string ProductTextCfgManager::_dataTypeName_("ProductTextCfgManager"); 
#endif


ProductTextCfgManager::ProductTextCfgManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

ProductTextCfgManager::ProductTextCfgManager(hfc::HFCManagerContainer *container)
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


ProductTextCfgManager::~ProductTextCfgManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();

	if (receivedProductTextCfgRestore)
		ProductTextCfg::validateDefaultRecords(dbAccess);
}


void ProductTextCfgManager::init()
{
	mySession = 0;

	currentProductTextCfg = 0;

	allProductTextCfgsQuery = 0;
	allProductTextCfgsIter  = 0;

	receivedProductTextCfgRestore = false;
}


DBSession *ProductTextCfgManager::getMySession()
{
	return mySession;
}
void ProductTextCfgManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
ProductTextCfg *ProductTextCfgManager::firstProductTextCfg()
{
	if (!dbAccess)
		return (ProductTextCfg *)NULL;

	DBProductTextCfgsRM *myRM = dbAccess->getDBProductTextCfgsRM();
	if (!myRM)
		return (ProductTextCfg *)NULL;

	// If the user calls firstProductTextCfg() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allProductTextCfgsQuery = myRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allProductTextCfgsQuery)
		return (ProductTextCfg *)NULL;

	DBQuery<DBProductTextCfg>::const_iterator tempIter = allProductTextCfgsQuery->begin();
	if (tempIter == allProductTextCfgsQuery->end())
		return (ProductTextCfg *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProductTextCfg()
	 * method call below).
	 */
	allProductTextCfgsIter = new DBQuery<DBProductTextCfg>::const_iterator(tempIter);
	DBProductTextCfg *p = new DBProductTextCfg(*(*allProductTextCfgsIter));
	ProductTextCfg *newProductTextCfg = new ProductTextCfg(p, dbAccess);
	return newProductTextCfg;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProductTextCfg() method, the first call to this method will
 * result in the first record being returned.
 */
ProductTextCfg *ProductTextCfgManager::nextProductTextCfg()
{
	if (!dbAccess)
		return (ProductTextCfg *)NULL;

	if (allProductTextCfgsQuery && !allProductTextCfgsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allProductTextCfgsQuery || !allProductTextCfgsIter)
		return firstProductTextCfg();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductTextCfgsIter);
	if (*allProductTextCfgsIter == allProductTextCfgsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductTextCfgs in the db. */
		destroyQueriesAndIters();
		return (ProductTextCfg *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBProductTextCfg copy constructor in order for the Hfc ProductTextCfg to
	 * control the destruction of the DBProductTextCfg record that the iterator contains; otherwise, the DBProductTextCfg will
	 * be deleted when the iterator goes out of scope.  The DBProductTextCfg copy will be deleted by the ProductTextCfg record
	 * created below.
	 */
	DBProductTextCfg *p = new DBProductTextCfg(*(*allProductTextCfgsIter));
	ProductTextCfg *newProductTextCfg = new ProductTextCfg(p, dbAccess);
	return newProductTextCfg;
}


/* Provide a countAll method so the caller doesn't need to know about a dbProductTextCfgsRM.
 */
int ProductTextCfgManager::countAllProductTextCfgs()
{
	if (!dbAccess)
		return -1;

	DBProductTextCfgsRM *myRM = dbAccess->getDBProductTextCfgsRM();
	if (!myRM)
		return -1;

	return myRM->countAll();
}


ProductTextCfg *ProductTextCfgManager::getCurrentProductTextCfg()
{
	return currentProductTextCfg;
}
void ProductTextCfgManager::setCurrentProductTextCfg(ProductTextCfg *newProductTextCfg)
{
	if (currentProductTextCfg)
		delete currentProductTextCfg;
	currentProductTextCfg = newProductTextCfg;
}


void ProductTextCfgManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductTextCfg *msgProductTextCfg = dynamic_cast<ProductTextCfg *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("ProductTextCfgManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgProductTextCfg);
			break;

		case DtWrite:
			handleWrite(msgProductTextCfg);
			break;

		case DtDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgProductTextCfg);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgProductTextCfg);
			break;

		default:
			Debug::trace("ProductTextCfgManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void ProductTextCfgManager::handleRead(ProductTextCfg *msgRec)
{
	if (!msgRec || !dbAccess)
		return;

	ProductTextCfg *lookedUpRec = ProductTextCfg::findProductTextCfg(msgRec->getTextType(), dbAccess);

	if (!lookedUpRec || lookedUpRec->getErrorCode() != RECORD_OK)		//	This productTextCfg does not exist
	{
			// Send error message to browser indicating ProductTextCfg not found.
		if (mySession) 
			mySession->addDialogMessage(ErrProductTextCfgDev, ProductTextCfgErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpRec)
		{
			delete lookedUpRec;
			lookedUpRec = 0;
		}
		return;
	}

	// We have a new current ProductTextCfg.
	setCurrentProductTextCfg(lookedUpRec);
	Debug::trace("ProductTextCfgManager.ProductTextCfg", HobString("handleRead(): currentProductTextCfg = ") + HobString(Hex::toString((long long) currentProductTextCfg)));

	if (mySession)
	{
		// Send the ProductTextCfg found back to the Browser.
		// Is this the behavior for ALL ProductTextCfgRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a ProductTextCfg, but doesn't want the data sent back?
		SessionMessage productTextCfgData(ProductTextCfgDev, DtWrite);
		productTextCfgData.setFormatType(BrowserFormat);
		productTextCfgData.setHfcObject(currentProductTextCfg);

		// Send the ProductTextCfg to the UI (Browser)
		mySession->send(&productTextCfgData, Messenger::DEST_UI);
	}
}


void ProductTextCfgManager::handleWrite(ProductTextCfg *modifiedRec)
{
	if ((!modifiedRec) || (!currentProductTextCfg))
		return;

	if (Debug::isTracing("ProductTextCfgManager"))
		Debug::trace("ProductTextCfgManager", HobString("handleWrite(): currentProductTextCfg = ") + HobString(Hex::toString((long long) currentProductTextCfg)) +
									  HobString("  , modifiedProductTextCfg = ") + HobString(Hex::toString((long long) modifiedRec)));

	currentProductTextCfg->clearErrors();
  	currentProductTextCfg->updateProductTextCfg(modifiedRec);
	if (mySession && Configuration::getInstance()->toBoolCfgI(CfgInternal::NOTIFYUIOFUPDATESTATUS))
	{
		SessionMessage updateStatusMsg(ProductTextCfgDev, DtUpdateStatus);
		updateStatusMsg.setFormatType(BrowserFormat);
		HfcUpdateStatus stat(!currentProductTextCfg->hasErrors());
		updateStatusMsg.setHfcObject(&stat);
		mySession->send(&updateStatusMsg, Messenger::DEST_UI);
	}
}


void ProductTextCfgManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductTextCfg *msgRec = dynamic_cast<ProductTextCfg *> (msg->getHfcObject());
	if (!msgRec || !dbAccess)
		return;

	ProductTextCfg *existingRec = ProductTextCfg::findProductTextCfg(msgRec->getTextType(), dbAccess);
	if (existingRec)
	{
		existingRec->deleteRecord();
		Debug::trace("ProductTextCfgManager", HobString("handleDelete - ProductTextCfg deleted - textType:") + HobString(Integer::toString((int)msgRec->getTextType())));
		delete existingRec;
		existingRec = 0;
	}
	else
		Debug::trace("ProductTextCfgManager", HobString("handleDelete - ProductTextCfg to be deleted was not in the database to delete - textType:") + HobString(Integer::toString((int)msgRec->getTextType())));
}


void ProductTextCfgManager::handleDeleteAll()
{
	Debug::trace("ProductTextCfgManager", "handleDeleteAll - deleting all ProductTextCfgs");

	if (!dbAccess)
		return;

	DBProductTextCfgsRM *myRM = dbAccess->getDBProductTextCfgsRM();
	if (!myRM)
		return;

	myRM->deleteAll();

	destroyQueriesAndIters();

	Debug::trace("ProductTextCfgManager", "handleDeleteAll - will create default records next");
	ProductTextCfg::verifyDefaultRecords(dbAccess);

	Debug::trace("ProductTextCfgManager", "handleDeleteAll - leaving");
}


void ProductTextCfgManager::handleUpdateAndSave(ProductTextCfg *modifiedRec)
{
	Debug::trace("ProductTextCfgManager", HobString("handleUpdateAndSave(): currentProductTextCfg = ") + HobString(Hex::toString((long long) currentProductTextCfg)));
	if (currentProductTextCfg)
	{
		handleWrite(modifiedRec);
		handleSaveRecord(currentProductTextCfg);
	}
}

void ProductTextCfgManager::handleSaveRecord(ProductTextCfg *recordToSave)
{
	Debug::trace("ProductTextCfgManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
		recordToSave->saveRecord();
}


void ProductTextCfgManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductTextCfg *restoreRec = dynamic_cast<ProductTextCfg *> (msg->getHfcObject());
	if (!restoreRec || !dbAccess)
		return;

	receivedProductTextCfgRestore = true;

	restoreRec->setDBAccess(dbAccess);

	ProductTextCfg *usedRec = (ProductTextCfg *)NULL;
	ProductTextCfg *existingRec = ProductTextCfg::findProductTextCfg(restoreRec->getTextType(), dbAccess);
	if (existingRec)
	{
		if (existingRec->isRecordValid())
		{
			existingRec->updateProductTextCfg(restoreRec);
			existingRec->saveRecord();
			usedRec = existingRec;								
		}
		else
		{
			existingRec->deleteRecord();	   
			restoreRec->saveRecord();
			usedRec = restoreRec;								
		}
	}
	else
	{
		restoreRec->saveRecord();
		usedRec = restoreRec;								
	}

	if (usedRec && usedRec->hasErrors() && mySession && mySession->isCommSession())
	{
		SessionMessage errMsg(Error, DtWrite);
		errMsg.setFormatType(EvolutionTaggedFormat);
		HfcError *err = usedRec->firstError();
		while (err)
		{
			errMsg.setHfcObject(err);
			mySession->send(&errMsg, Messenger::DEST_UI);

			err = usedRec->nextError();
		}
	}

	if (existingRec)
		delete existingRec;
}


void ProductTextCfgManager::handleCreate(ProductTextCfg *msgRec)
{
	if (!msgRec)
		return;

	ProductTextCfg *newRec = createNewProductTextCfg(msgRec->getTextType());
	if (newRec)
	{
		//	Set this new productTextCfg as the current productTextCfg.
		setCurrentProductTextCfg(newRec);

		if (mySession)
		{
			// Send the new productTextCfg to the Browser.
			SessionMessage msg(ProductTextCfgDev, DtWrite);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(currentProductTextCfg);

			mySession->send(&msg, Messenger::DEST_UI);
		}
	}
}

ProductTextCfg *ProductTextCfgManager::createNewProductTextCfg(ProdSupportTextType type)
{
	ProductTextCfg *newRec = (ProductTextCfg *) NULL;
	if (mySession)
	{
		// Send the current new ProductTextCfg to the browser.
		newRec = new ProductTextCfg(type);
		if (newRec)
			newRec->setDBAccess(dbAccess);
	}
	return newRec;
}

void ProductTextCfgManager::destroyQueriesAndIters()
{
	if (allProductTextCfgsIter)
	{
		delete allProductTextCfgsIter;
		allProductTextCfgsIter = (DBQuery<DBProductTextCfg>::const_iterator *)NULL;
	}

	if (allProductTextCfgsQuery)
	{
		delete allProductTextCfgsQuery;
		allProductTextCfgsQuery = (DBQuery<DBProductTextCfg> *)NULL;
	}
}
