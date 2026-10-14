/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTextManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "Configuration.h"
#include "ProductText.h"
#include "Product.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "util/e_string/utilstring.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTextManager::dataType = 0;
std::string ProductTextManager::dataTypeName("ProductTextManager"); 
#endif


ProductTextManager::ProductTextManager(ProdSupportTextType type)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	type_ = type;

}

ProductTextManager::ProductTextManager(ProdSupportTextType type, hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	type_ = type;
	dbAccess = container;
	mgrContainer_ = container;
}

ProductTextManager::~ProductTextManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentProductText)
		delete currentProductText;

	destroyQueriesAndIters();
}


void ProductTextManager::init()
{
	mySession = 0;

	type_ = psttUnknown;

	currentProductText = 0;
	
	allProductTextsQuery = 0;
	allProductTextsIter  = 0;
}


DBSession *ProductTextManager::getMySession()
{
	return mySession;
}
void ProductTextManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
ProductText *ProductTextManager::firstProductText()
{
	if (!dbAccess || type_ == psttUnknown)
		return 0;

	DBProductTextsRM *productTextsRM = dbAccess->getDBProductTextsRM(type_);
	if (!productTextsRM)
		return 0;

	// If the user calls firstProductText() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allProductTextsQuery = productTextsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductTexts, just return false.
	if (!allProductTextsQuery)
		return 0;

	edb::DBQuery<edb::DBProductText>::const_iterator tempIter = allProductTextsQuery->begin();
	if (tempIter == allProductTextsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProductText()
	 * method call below).
	 */
	allProductTextsIter = new DBQuery<DBProductText>::const_iterator(tempIter);
	DBProductText *t = new DBProductText(*(*allProductTextsIter));
	ProductText *prodText= new ProductText(t, dbAccess);
	return prodText;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProductText() method, the first call to this method will
 * result in the first record being returned.
 */
ProductText *ProductTextManager::nextProductText()
{
	if (!dbAccess || type_ == psttUnknown)
		return 0;

	if (allProductTextsQuery && !allProductTextsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allProductTextsQuery || !allProductTextsIter)
		return firstProductText();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductTextsIter);
	if (*allProductTextsIter == allProductTextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductTexts in the db. */
		destroyQueriesAndIters();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBProductText copy constructor in order for the Hfc ProductText to
	 * control the destruction of the DBProductText record that the iterator contains; otherwise, the DBProductText will
	 * be deleted when the iterator goes out of scope.  The DBProductText copy will be deleted by the ProductText record
	 * created below.
	 */
	DBProductText *t = new DBProductText(*(*allProductTextsIter));
	ProductText *prodText = new ProductText(t, dbAccess);
	return prodText;
}


/* Provide a countAll method so the caller doesn't need to know about a DBProductTextsRM.
 */
int ProductTextManager::countAllProductTexts()
{
	if (!dbAccess || type_ <= psttUnknown || type_ >= psttLastProductText)
		return -1;

	DBProductTextsRM *productTextsRM = dbAccess->getDBProductTextsRM(type_);
	if (!productTextsRM)
		return -1;

	return productTextsRM->countAll();
}


ProductText *ProductTextManager::getCurrentProductText()
{
	return currentProductText;
}
void ProductTextManager::setCurrentProductText(ProductText *newProductText)
{
	if (currentProductText)
		delete currentProductText; 
	currentProductText = newProductText;
}
void ProductTextManager::setCurrentProductTextFromProduct(ProductText *newProductText)
{
	//	The Product will take care of deleting the Product text. Don't do it here.
	currentProductText = newProductText;
}


void ProductTextManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductText *productTextMsg = dynamic_cast<ProductText*>( msg->getHfcObject() );

	UINT8 event = msg->getEventType();
	Debug::trace("ProductTextManager", "TextType: " + inttostring(type_) + "  handling event: " + inttostring(event));
	switch (event)
	{
		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtSave:
			if ( productTextMsg )
				handleSaveRecord( productTextMsg );
			break;

		default:
			Debug::trace("ProductTextManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void ProductTextManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductText *msgProductText = dynamic_cast<ProductText *> (msg->getHfcObject());
	if (!msgProductText || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(getRecordManagerType()))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		ProductText *existingText = ProductText::findRecord(msgProductText->getTextType(), msgProductText->getTextNum(), msgProductText->getDeptNum(), dbAccess);
		if (existingText)
		{
			existingText->deleteRecord();
			Debug::trace("ProductTextManager", "handleDelete - product text deleted - type:" + inttostring(msgProductText->getTextType()) +
			                                   "  num:" + longtostring(msgProductText->getTextNum()));
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(getRecordManagerType()))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingText, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingText;
		}
		else
			Debug::trace("ProductTextManager", "handleDelete - product text to be deleted was not in the database to delete - type:" +
			                                   inttostring(msgProductText->getTextType()) + "  num:" + longtostring(msgProductText->getTextNum()));
	}
}


void ProductTextManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void ProductTextManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("ProductTextManager", "handleDeleteAll - deleting all product texts  type:" + inttostring(type_));

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(getRecordManagerType()))
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

		DBProductTextsRM *dbRM = dbAccess->getDBProductTextsRM(type_);
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();
	}

	Debug::trace("ProductTextManager", "handleDeleteAll - leaving");
}


void ProductTextManager::handleSaveRecord(ProductText *recordToSave)
{
	Debug::trace("ProductTextManager", "handleSaveRecord(): type:" + inttostring(type_) + "  recordToSave:" + ptrtostring(recordToSave));
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(getRecordManagerType()))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void ProductTextManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductText *restoreText = dynamic_cast<ProductText *> (msg->getHfcObject());
	if (!restoreText || !dbAccess)
		return;

	Debug::trace("ProductTextManager", "handleRestore - attempting to restore text - mgrType:" + inttostring(type_) + "  recType:" + inttostring(restoreText->getTextType()) + "  num:" + longtostring(restoreText->getTextNum()));

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(getRecordManagerType()))
	{
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		if ( msg->getFormatType() == UltimaTaggedFormat )
		{
			std::string reserialization;
			serializeForSynchronization(reserialization, restoreText);

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
		restoreText->setDBAccess(dbAccess);

		ProductText *existingText = ProductText::findRecord(restoreText->getTextType(), restoreText->getTextNum(), restoreText->getDeptNum(), dbAccess);
		if (existingText)
		{
			Debug::trace("ProductTextManager", "handleRestore - text already existed");
			if (existingText->isRecordValid())
			{
				existingText->updateRecord(restoreText);
				existingText->saveRecord();								
				Debug::trace("ProductTextManager", "handleRestore - saved updated text");
			}
			else
			{
				existingText->deleteRecord();	   
				restoreText->saveRecord();
				Debug::trace("ProductTextManager", "handleRestore - deleted invalid record and saved this text");
			}

			delete existingText;
		}
		else
		{
			Debug::trace("ProductTextManager", "handleRestore - did not find text, going to save");
			restoreText->saveRecord();
			Debug::trace("ProductTextManager", "handleRestore - created and saved text");
		}
	}
}


void ProductTextManager::destroyQueriesAndIters()
{
	if (allProductTextsIter)
	{
		delete allProductTextsIter;
		allProductTextsIter = (DBQuery<DBProductText>::const_iterator *)NULL;
	}

	if (allProductTextsQuery)
	{
		delete allProductTextsQuery;
		allProductTextsQuery = (DBQuery<DBProductText> *)NULL;
	}
}
