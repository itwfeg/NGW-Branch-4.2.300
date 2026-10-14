/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductDefaultManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"
#include "HobList.h"
#include "ProductDevEvent.h"

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

int ProductDefaultManager::_dataType_ = 0;
std::string ProductDefaultManager::_dataTypeName_("ProductDefaultManager"); 
#endif


ProductDefaultManager::ProductDefaultManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

ProductDefaultManager::ProductDefaultManager(hfc::HFCManagerContainer *container)
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

ProductDefaultManager::~ProductDefaultManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (currentProductDefault)
		delete currentProductDefault;

	destroyQueriesAndIters();
}


void ProductDefaultManager::init()
{
	mySession = 0;

	currentProductDefault = 0;

	allProductDefaultsQuery = 0;
	allProductDefaultsIter  = 0;
}


DBSession *ProductDefaultManager::getMySession()
{
	return mySession;
}
void ProductDefaultManager::setMySession(DBSession *session)
{
	mySession = session;
}


void ProductDefaultManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();

	ProductDefault *msgProductDefault = dynamic_cast<ProductDefault *> (msg->getHfcObject());

	Debug::trace("ProductDefaultManager", HobString("handling device: ") + HobString(Integer::toString(msg->getDeviceType())) + HobString("   event: ") + HobString(Integer::toString(event)));

	switch (msg->getDeviceType())
	{
		case ProductDev:
			switch (event)
			{
				case ProductReadDefault:
					handleRead(msgProductDefault);
					break;

				case ProductWriteDefault:
					handleWrite(msgProductDefault);
					break;

				case ProductSaveDefault:
					handleUpdateAndSave(msgProductDefault);
					break;

				default:
					Debug::trace("ProductManager", HobString("UNKNOWN ProductDev EVENT: ") + HobString(Integer::toString(event)));
					break;
			}
			break;

		case ProductDefaultsDev:
			switch (event)
			{
				case DtRead:
					handleRead(msgProductDefault);
					break;

				case DtWrite:
					handleWrite(msgProductDefault);
					break;

				case DtSave:
					handleUpdateAndSave(msgProductDefault);
					break;

				case DtRestore:
					handleRestore(msg);
					break;

				default:
					Debug::trace("ProductManager", HobString("UNKNOWN ProductDefaultsDev EVENT: ") + HobString(Integer::toString(event)));
					break;
			}
			break;

		default:
			Debug::trace("ProductDefaultManager", HobString("UNKNOWN DEVICE: ") + HobString(Integer::toString(msg->getDeviceType())));
			break;
	}
}


ProductDefault *ProductDefaultManager::getCurrentProductDefault()
{
	return currentProductDefault;
}
void ProductDefaultManager::setCurrentProductDefault(ProductDefault *newProductDefault)
{
	if (currentProductDefault)
		delete currentProductDefault;
	currentProductDefault = newProductDefault;
}


/* If the first record was not read, this method will return NULL.
 */
ProductDefault *ProductDefaultManager::firstProductDefault()
{
	if (!dbAccess)
		return (ProductDefault *)NULL;

	DBProductDefaultsRM	*dbProductDefaultsRM = dbAccess->getDBProductDefaultsRM();
	if (!dbProductDefaultsRM)
		return (ProductDefault *)NULL;

	// If the user calls firstProductDefault() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allProductDefaultsQuery = dbProductDefaultsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductDefaults, just return false.
	if (!allProductDefaultsQuery)
		return (ProductDefault *)NULL;

	DBQuery<DBProductDefault>::const_iterator tempIter = allProductDefaultsQuery->begin();
	if (tempIter == allProductDefaultsQuery->end())
		return (ProductDefault *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProductDefault()
	 * method call below).
	 */
	allProductDefaultsIter = new DBQuery<DBProductDefault>::const_iterator(tempIter);
	DBProductDefault *p = new DBProductDefault(*(*allProductDefaultsIter));
	ProductDefault *pd = new ProductDefault(p, dbAccess);
	return pd;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProductDefault() method, the first call to this method will
 * result in the first record being returned.
 */
ProductDefault *ProductDefaultManager::nextProductDefault()
{
	if (!dbAccess)
		return (ProductDefault *)NULL;

	if (allProductDefaultsQuery && !allProductDefaultsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allProductDefaultsQuery || !allProductDefaultsIter)
		return firstProductDefault();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductDefaultsIter);
	if (*allProductDefaultsIter == allProductDefaultsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductDefaults in the db. */
		destroyQueriesAndIters();
		return (ProductDefault *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBProductDefault copy constructor in order for the Hfc ProductDefault to
	 * control the destruction of the DBProductDefault record that the iterator contains; otherwise, the DBProductDefault will
	 * be deleted when the iterator goes out of scope.  The DBProductDefault copy will be deleted by the ProductDefault record
	 * created below.
	 */
	DBProductDefault *p = new DBProductDefault(*(*allProductDefaultsIter));
	ProductDefault *pd = new ProductDefault(p, dbAccess);
	return pd;
}


/* Provide a countAll method so the caller doesn't need to know about a DbProductDefaultsRM.
 */
int ProductDefaultManager::countAllProductDefaults()
{
	if (!dbAccess)
		return -1;

	DBProductDefaultsRM	*dbProductDefaultsRM = dbAccess->getDBProductDefaultsRM();
	if (!dbProductDefaultsRM)
		return -1;

	return dbProductDefaultsRM->countAll();
}


void ProductDefaultManager::handleRead(ProductDefault *msgProductDefault)
{
	if (!msgProductDefault || !dbAccess)
		return;

	ProductDefault *productDefault = ProductDefault::findRecord(msgProductDefault->getProductType(), msgProductDefault->getDeptNum(), dbAccess);
	if (Debug::isTracing("New.ProductDefault"))
		Debug::trace("New.ProductDefault", HobString("    ProductDefault: ") + HobString(Hex::toString((long long) productDefault)) + HobString(" ProductManager::handleProductDefaultRead"));

	if (!productDefault || productDefault->getErrorCode() != RECORD_OK)
	{
		// Send error message to browser indicating product_not_found.
		if (mySession)
			mySession->addDialogMessage(ErrProductDev, ProductErr, HfcResponse::RESPONSESTYLE_OK);

		if (productDefault)
		{
			delete productDefault;
			productDefault = (ProductDefault *)NULL;
		}
	}
	else
	{
		setCurrentProductDefault(productDefault);

		if (mySession)
		{
			// Send the product default found back to the Browser.
			// Is this the behavior for ALL ProductRead messages the Session receives, to send the data back to the Browser?
			// Is there any situation where something else reads a PLU, but doesn't want the data sent back?
			SessionMessage defaultData(ProductDev, ProductWriteDefault);
			defaultData.setFormatType(BrowserFormat);
			defaultData.setHfcObject(currentProductDefault);

			// Send the product to the UI (Browser)
			mySession->send(&defaultData, Messenger::DEST_UI);
		}
	}
}


void ProductDefaultManager::handleWrite(ProductDefault *msgProductDefault)
{
	if (!msgProductDefault)
		return;

	if (currentProductDefault)
	  	currentProductDefault->updateRecord(msgProductDefault, mySession);
}


void ProductDefaultManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void ProductDefaultManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("ProductDefaultManager", "handleDeleteAllProductDefaults - deleting all product defaults");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ProductDefaults))
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

		DBProductDefaultsRM	*dbRM = dbAccess->getDBProductDefaultsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		setCurrentProductDefault((ProductDefault *) NULL);

		Debug::trace("ProductDefaultManager", "handleDeleteAllProductDefaults - will create reserved ProductDefault records next");
		ProductDefault::verifyReservedRecords(dbAccess);
	}

	Debug::trace("ProductDefaultManager", "handleDeleteAllProductDefaults - leaving");
}


void ProductDefaultManager::handleUpdateAndSave(ProductDefault *modifiedProductDefault)
{
	Debug::trace("ProductDefaultManager", HobString("handleUpdateAndSave(): currentProductDefault = ") + HobString(Hex::toString((long long) currentProductDefault)));
	if (currentProductDefault)
	{
		if (modifiedProductDefault)
		  	currentProductDefault->updateRecord(modifiedProductDefault, mySession);
		handleSaveRecord(currentProductDefault);
	}
}


void ProductDefaultManager::handleSaveRecord(ProductDefault *recordToSave)
{
	Debug::trace("ProductDefaultManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ProductDefaults))
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


void ProductDefaultManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductDefault *restoreProductDefault = dynamic_cast<ProductDefault *> (msg->getHfcObject());
	if (!restoreProductDefault || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ProductDefaults))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreProductDefault->setDBAccess(dbAccess);

		ProductDefault *existingProductDefault = ProductDefault::findRecord(restoreProductDefault->getProductType(), restoreProductDefault->getDeptNum(), dbAccess);
		if (existingProductDefault)
		{
			if (existingProductDefault->isRecordValid())
			{
				existingProductDefault->updateRecord(restoreProductDefault, 0);
				existingProductDefault->saveRecord();
			}
			else
			{
				existingProductDefault->deleteRecord();
				restoreProductDefault->saveRecord();
			}

			delete existingProductDefault;
		}
		else
			restoreProductDefault->saveRecord();
	}
}


void ProductDefaultManager::destroyQueriesAndIters()
{
	if (allProductDefaultsIter)
	{
		delete allProductDefaultsIter;
		allProductDefaultsIter = (DBQuery<DBProductDefault>::const_iterator *)NULL;
	}

	if (allProductDefaultsQuery)
	{
		delete allProductDefaultsQuery;
		allProductDefaultsQuery = (DBQuery<DBProductDefault> *)NULL;
	}
}
