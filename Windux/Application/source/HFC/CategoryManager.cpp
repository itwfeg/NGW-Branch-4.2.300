/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CategoryManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "PresentationMethod.h"
#include "ReadSoftKeyParams.h"
#include "SoftKeyLists.h"
#include "Softkey.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "GraphicFileNames.h"
#include "AllCategories.h"
#include "HfcError.h"
#include "HfcUpdateStatus.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCategoriesRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CategoryManager::_dataType_ = 0;
std::string CategoryManager::_dataTypeName_("CategoryManager"); 
#endif


CategoryManager::CategoryManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

CategoryManager::CategoryManager(hfc::HFCManagerContainer *container)
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


CategoryManager::~CategoryManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (currentGridData)
		delete currentGridData;

	destroyQueriesAndIters();
}


void CategoryManager::init()
{
	mySession = 0;

	currentCategory = 0;

	currentGridData = 0;

	allCategoriesQuery = 0;
	allCategoriesIter  = 0;
}


DBSession *CategoryManager::getMySession()
{
	return mySession;
}
void CategoryManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Category *CategoryManager::firstCategory()
{
	if (!dbAccess)
		return (Category *)NULL;

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	if (!dbCategoriesRM)
		return (Category *)NULL;

	// If the user calls firstCategory() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allCategoriesQuery = dbCategoriesRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allCategoriesQuery)
		return (Category *)NULL;

	DBQuery<DBCategory>::const_iterator tempIter = allCategoriesQuery->begin();
	if (tempIter == allCategoriesQuery->end())
		return (Category *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCategory()
	 * method call below).
	 */
	allCategoriesIter = new DBQuery<DBCategory>::const_iterator(tempIter);
	DBCategory *c = new DBCategory(*(*allCategoriesIter));
	Category *newCategory = new Category(c, dbAccess);
	return newCategory;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstCategory() method, the first call to this method will
 * result in the first record being returned.
 */
Category *CategoryManager::nextCategory()
{
	if (!dbAccess)
		return (Category *)NULL;

	if (allCategoriesQuery && !allCategoriesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allCategoriesQuery || !allCategoriesIter)
		return firstCategory();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCategoriesIter);
	if (*allCategoriesIter == allCategoriesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Categories in the db. */
		destroyQueriesAndIters();
		return (Category *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCategory copy constructor in order for the Hfc Category to
	 * control the destruction of the DBCategory record that the iterator contains; otherwise, the DBCategory will
	 * be deleted when the iterator goes out of scope.  The DBCategory copy will be deleted by the Category record
	 * created below.
	 */
	DBCategory *c = new DBCategory(*(*allCategoriesIter));
	Category *newCategory = new Category(c, dbAccess);
	return newCategory;
}


/* Provide a countAll method so the caller doesn't need to know about a dbCategoriesRM.
 */
int CategoryManager::countAllCategories()
{
	if (!dbAccess)
		return -1;

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	if (!dbCategoriesRM)
		return -1;

	return dbCategoriesRM->countAll();
}


Category *CategoryManager::getCurrentCategory()
{
	return currentCategory;
}
void CategoryManager::setCurrentCategory(Category *newCategory)
{
	if (currentCategory)
		delete currentCategory;
	currentCategory = newCategory;
}


void CategoryManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	Category *msgCategory = dynamic_cast<Category *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("CategoryManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgCategory);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgCategory);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgCategory);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgCategory);
			break;

		case CategoryReadSoftKeyList:
//			handleReadSoftKeyList(msg);
			break;

		case CategoryReadSubCategoriesList:
//			handleReadSubCategoriesList(msg);
			break;

		case CategoryReadAllCatGraphicNames:
			handleReadAllGraphicNames();
			break;

		case CategoryReadAllMainCategories:
			handleReadAllMainCategories();
			break;

		case CategoryChangeFeaturedCategory:
//			handleChangeFeaturedCategory(msg);
			break;

		default:
			Debug::trace("CategoryManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void CategoryManager::handleRead(Category *msgCategory)
{
	if (!msgCategory || !dbAccess)
		return;

	Category *lookedUpCategory = Category::findRecord(msgCategory->getCategoryNum(), msgCategory->getDeptNum(), dbAccess);

	if (!lookedUpCategory || lookedUpCategory->getErrorCode() != RECORD_OK)		//	This category does not exist
	{
			// Send error message to browser indicating Category not found.
		if (mySession) 
			mySession->addDialogMessage(ErrCategoryDev, CategoryErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpCategory)
			delete lookedUpCategory;
		return;
	}

	// We have a new current Category.
	setCurrentCategory(lookedUpCategory);
	Debug::trace("CategoryManager.Category", HobString("handleRead(): currentCategory = ") + HobString(Hex::toString((long long) currentCategory)));

	if (mySession)
	{
		// Send the Category found back to the Browser.
		// Is this the behavior for ALL CategoryRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a Category, but doesn't want the data sent back?
		SessionMessage categoryData(CategoryDev, DtWrite);
		categoryData.setFormatType(BrowserFormat);
		categoryData.setHfcObject(currentCategory);

		// Send the Category to the UI (Browser)
		mySession->send(&categoryData, Messenger::DEST_UI);
	}
}


void CategoryManager::handleReadAll()
{
	Debug::trace("CategoryManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	LanguageSelection lang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

	AllCategories categoryList;
	categoryList.findAll(dbAccess, lang);

	// Send the categories list to the UI
	SessionMessage mclMsg(CategoryDev, DtWriteAll);
	mclMsg.setFormatType(BrowserFormat);
	mclMsg.setHfcObject(&categoryList);
	mySession->send(&mclMsg, Messenger::DEST_UI);
}

void CategoryManager::handleReadAllMainCategories()
{
	Debug::trace("CategoryManager", "handling CategoryReadAllMainCategories event ...");
	if (!mySession || !dbAccess)
		return;

	Session *s = dynamic_cast<Session *> (mySession);
	LanguageSelection lang = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

	AllCategories categoryList;
	categoryList.findAllMainCategories(dbAccess, lang);

	// Send the categories list to the UI
	SessionMessage mclMsg(CategoryDev, CategoryWriteAllMainCategories);
	mclMsg.setFormatType(BrowserFormat);
	mclMsg.setHfcObject(&categoryList);
	mySession->send(&mclMsg, Messenger::DEST_UI);
}


void CategoryManager::handleWrite(Category *modifiedCategory)
{
	if ((!modifiedCategory) || (!currentCategory))
		return;

	if (Debug::isTracing("CategoryManager"))
		Debug::trace("CategoryManager", HobString("handleWrite(): currentCategory = ") + HobString(Hex::toString((long long) currentCategory)) +
									  HobString("  , modifiedCategory = ") + HobString(Hex::toString((long long) modifiedCategory)));

	currentCategory->clearErrors();
  	currentCategory->updateRecord(modifiedCategory);
	if (mySession && Configuration::getInstance()->toBoolCfgI(CfgInternal::NOTIFYUIOFUPDATESTATUS))
	{
		SessionMessage updateStatusMsg(CategoryDev, DtUpdateStatus);
		updateStatusMsg.setFormatType(BrowserFormat);
		HfcUpdateStatus stat(!currentCategory->hasErrors());
		updateStatusMsg.setHfcObject(&stat);
		mySession->send(&updateStatusMsg, Messenger::DEST_UI);
	}
}


void CategoryManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Category *msgCategory = dynamic_cast<Category *> (msg->getHfcObject());
	if (!msgCategory || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Categories))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		Category *existingCategory = Category::findRecord(msgCategory->getCategoryNum(), msgCategory->getDeptNum(), dbAccess);
		if (existingCategory)
		{
			existingCategory->deleteRecord();
			Debug::trace("CategoryManager", HobString("handleDelete - Category deleted - categoryNum:") +
														HobString(Long::toString((long)msgCategory->getCategoryNum())));
			if (msg->getEventType() != DtBatchApplyDelete)
			{
				if (mySession && !mySession->isCommSession())
				{
					//	Create a blank category
					Category *newCategory = createNewCategory(0);
					// Make it the current Category.
					setCurrentCategory(newCategory);
					//	Send the blank category to the browser
					sendCurrentCategory();
				}

				// if received via UI, delete it and batch the record being deleted.
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Categories))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, existingCategory, getDeleteEvent());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getDeleteEvent(), synchSer)));
				}
			}

			delete existingCategory;
		}
		else
			Debug::trace("CategoryManager", HobString("handleDelete - Category to be deleted was not in the database to delete - categoryNum:") +
														HobString(Long::toString((long)msgCategory->getCategoryNum())));
	}
}


void CategoryManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void CategoryManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("CategoryManager", "handleDeleteAll - deleting all categories");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Categories))
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

		DBCategoriesRM *dbRM = dbAccess->getDBCategoriesRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();

		Debug::trace("CategoryManager", "handleDeleteAll - will create reserved records next");
		Category::verifyReservedRecords(dbAccess);
	}

	Debug::trace("CategoryManager", "handleDeleteAll - leaving");
}


void CategoryManager::handleUpdateAndSave(Category *modifiedCategory)
{
	Debug::trace("CategoryManager", HobString("handleUpdateAndSave(): currentCategory = ") + HobString(Hex::toString((long long) currentCategory)));
	if (currentCategory)
	{
		handleWrite(modifiedCategory);
		handleSaveRecord(currentCategory);
	}
}

void CategoryManager::handleSaveRecord(Category *recordToSave)
{
	Debug::trace("CategoryManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
	{
		recordToSave->saveRecord();

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Categories))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, recordToSave, getWriteEvent());

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			
			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void CategoryManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Category *restoreCategory = dynamic_cast<Category *> (msg->getHfcObject());
	if (!restoreCategory || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Categories))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff()));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreCategory->setDBAccess(dbAccess);

		Category *usedCategory = (Category *)NULL;
		Category *existingCategory = Category::findRecord(restoreCategory->getCategoryNum(), restoreCategory->getDeptNum(), dbAccess);
		if (existingCategory)
		{
			if (existingCategory->isRecordValid())
			{
				existingCategory->updateRecord(restoreCategory);
				existingCategory->saveRecord();
				usedCategory = existingCategory;								
			}
			else
			{
				existingCategory->deleteRecord();	   
				restoreCategory->saveRecord();
				usedCategory = restoreCategory;								
			}
		}
		else
		{
			restoreCategory->saveRecord();
			usedCategory = restoreCategory;								
		}

		if (usedCategory && usedCategory->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedCategory->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedCategory->nextError();
			}
		}
		if (existingCategory)
			delete existingCategory;
	}
}


void CategoryManager::handleCreate(Category *msgCategory)
{
	if (!msgCategory)
		return;

	Category *newCategory = createNewCategory(msgCategory->getCategoryNum());
	if (newCategory)
	{
		//	Set this new category as the current category.
		setCurrentCategory(newCategory);

		if (mySession)
		{
			// Send the new category to the Browser.
			SessionMessage categoryData(CategoryDev, DtWrite);
			categoryData.setFormatType(BrowserFormat);
			categoryData.setHfcObject(currentCategory);

			mySession->send(&categoryData, Messenger::DEST_UI);
		}
	}
}

#if 0
void CategoryManager::handleReadSoftKeyList(SessionMessage *msg)
{
	Debug::trace("CategoryManager", "handling CategoryReadSoftKeyList event ...");
	if (!msg || !mySession || !dbAccess)
		return;

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	if (!dbCategoriesRM)
		return;

	ReadSoftKeyParams *rskp = dynamic_cast<ReadSoftKeyParams *> (msg->getHfcObject());
	if (!rskp)
		return;

	HobRecordNum sourceCatNum = rskp->getRecordNum();
	if (!rskp->isReceived(fieldOffsetRSKPRecordNum) || sourceCatNum == 0)
		sourceCatNum = Category::RootCategoryNum;
	Debug::trace("CategoryManager", HobString("handleReadSoftKeyList() - sourceCategoryNum:") + HobString(Long::toString(sourceCatNum)));

	int cellCount = Configuration::getInstance()->getNumOfMostCommonCategories();
	Debug::trace("CategoryManager", HobString("handleReadSoftKeyList() - cellCount:") + HobString(Integer::toString(cellCount)));
	if (cellCount <= 0)
		return;

	// Find the 'source' category number record in the database.
	DBCategory *dbSourceCategory = dbCategoriesRM->findNumber(Long::toString(sourceCatNum));
	if (!dbSourceCategory)
	{
		Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - couldn't find the source category"));
		return;
	}
			
	//DBQuery<DBCategory> *categoriesQuery = dbCategoriesRM->retrieveChildCategories(dbSourceCategory);
	DBQuery<DBCategory> *categoriesQuery = dbCategoriesRM->retrieveDisplayCategories();
	if (!categoriesQuery)
	{
		Debug::trace("CategoryManager", HobString("handleReadSoftKeyList() - received back a null pointer for the query retrieveDisplayCategories"));
		delete dbSourceCategory;
		return;
	}
			
	SoftKeyLists categoryData;

	Category *cellCategory = (Category *)NULL;
	int i = 0;
	DBQuery<DBCategory>::const_iterator categoriesQueryIter = categoriesQuery->begin();
	while (i < cellCount && (categoriesQueryIter != categoriesQuery->end()))
	{
		DBCategory *c = new DBCategory(*categoriesQueryIter);
		if (c)
		{
			Debug::trace("CategoryManager", HobString("handleReadSoftKeyList() - have a category for a cell:") + HobString(c->getNumber()));

			cellCategory = new Category(c, dbAccess);
			if (cellCategory && (cellCategory->getDisplayOrder() > 0))
			{
				i++;
				Session *s = dynamic_cast<Session *> (mySession);
				categoryData.add(*cellCategory, ((s) ? s->getCurrentLanguageSelection() : PrimaryLanguage));
				delete cellCategory;
			}
			else
				delete c;
		}

		++categoriesQueryIter;
	}

	Debug::trace("CategoryManager", HobString("handleReadSoftKeyList() - read this number of categories:") + HobString(Integer::toString(i)));

	//categoryData.blankFillLists(cellCount);

	// Send the softkey lists to the UI (Browser)
	SessionMessage sklMsg(CategoryDev, CategoryWriteSoftKeyList);
	sklMsg.setFormatType(BrowserFormat);
	categoryData.setFormatType(SoftKeyListNoGraphics);
	sklMsg.setHfcObject(&categoryData);
	mySession->send(&sklMsg, Messenger::DEST_UI);

	if (categoriesQuery)
		delete categoriesQuery;
	delete dbSourceCategory;
}

void CategoryManager::handleReadSubCategoriesList(SessionMessage *msg)
{
	Debug::trace("CategoryManager", "handling ReadSubCategoriesList event ...");
	if (!msg || !mySession || !dbAccess)
		return;

	DBCategoriesRM *dbCategoriesRM = dbAccess->getDBCategoriesRM();
	if (!dbCategoriesRM)
		return;

	ReadSoftKeyParams *rskp = dynamic_cast<ReadSoftKeyParams *> (msg->getHfcObject());
	if (!rskp)
		return;

	HobRecordNum sourceCatNum = rskp->getRecordNum();
	if (!rskp->isReceived(fieldOffsetRSKPRecordNum) || sourceCatNum == 0)
		sourceCatNum = Category::RootCategoryNum;
	Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - sourceCategoryNum:") + HobString(Long::toString(sourceCatNum)));

	int cellCount = Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements();
	Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - cellCount:") + HobString(Integer::toString(cellCount)));
	if (cellCount <= 0)
		return;

	switch (rskp->getDataType())
	{
		case PresentationMethod::Type_Previous:
		{
			if (!currentGridData)
				return;

			currentGridData->gotoPreviousPage();
			break;
		}

		case PresentationMethod::Type_Next:
		{
			if (!currentGridData)
				return;

			currentGridData->gotoNextPage();
			break;
		}

		default:
		{
			// Find the 'source' category number record in the database.
			DBCategory *dbSourceCategory = dbCategoriesRM->findNumber(Long::toString(sourceCatNum));
			if (!dbSourceCategory)
			{
				Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - couldn't find the source category"));
				return;
			}
					
			DBQuery<DBCategory> *currentCategoriesQuery = dbCategoriesRM->retrieveChildCategories(dbSourceCategory);
			delete dbSourceCategory;
			if (!currentCategoriesQuery)
			{
				Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - received back a null pointer for the query retrieveChildCategories"));
				return;
			}

			if (currentGridData)
			{
				delete currentGridData;
				currentGridData = (SoftKeyLists *)NULL;
			}
			currentGridData = new SoftKeyLists();

			Category *cellCategory = (Category *)NULL;
			int i = 0;
			Session *s = dynamic_cast<Session *> (mySession);
			short langNum = (s) ? s->getCurrentLanguageSelection() : PrimaryLanguage;

			DBQuery<DBCategory>::const_iterator currentCategoriesQueryIter = currentCategoriesQuery->begin();
			while (currentCategoriesQueryIter != currentCategoriesQuery->end())
			{
				DBCategory *c = new DBCategory(*currentCategoriesQueryIter);
				if (c)
				{
					Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - have a dbcategory for a cell:") + HobString(c->getNumber()));

					cellCategory = new Category(c, dbAccess);
					if (cellCategory)
					{
						Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - have a category for a cell:") + HobString(Long::toString(cellCategory->getCategoryNum())));
						currentGridData->add(*cellCategory, langNum);
						delete cellCategory;
						cellCategory = (Category *)NULL;
					}
					else
						delete c;
				}
				++currentCategoriesQueryIter;
			}

			if (currentCategoriesQuery)
				delete currentCategoriesQuery;

			currentGridData->insertNextandPreviousCells(langNum);
			currentGridData->gotoFirstPage();

			Debug::trace("CategoryManager", HobString("handleCategoryReadSubCategoriesList() - read this number of categories:") + HobString(Integer::toString(i)));

			//categoryData.blankFillLists(cellCount);
			break;
		}
	}
		

	// Send the softkey lists to the UI (Browser)
	SessionMessage sklMsg(CategoryDev, CategoryWriteSubCategoriesList);
	sklMsg.setFormatType(BrowserFormat);
	sklMsg.setHfcObject(currentGridData);
	mySession->send(&sklMsg, Messenger::DEST_UI);
}
#endif


void CategoryManager::handleReadAllGraphicNames()
{
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(Graphic::TYPE_CATEGORYGRAPHIC);
	if (gfn)
	{
		if (gfn->getListCount() > 0)
		{
			// Send the Graphic file names found back to the Browser.
			SessionMessage nameData(CategoryDev, CategoryWriteAllCatGraphicNames);
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


Category *CategoryManager::createNewCategory(HobRecordNum recordNum)
{
	Category *newCategory = (Category *) NULL;
	if (mySession)
	{
		// Send the current new Category to the browser.
		newCategory = new Category();
		if (newCategory)
		{
			newCategory->setDBAccess(dbAccess);
			newCategory->setCategoryNum(recordNum);
		}
	}
	return newCategory;
}

void CategoryManager::sendCurrentCategory()
{
	if (!currentCategory || !mySession)
	{
		Debug::trace("CategoryManager", "sendCurrentCategory(): currentCategory or mySession is NULL!");
		return;
	}
	SessionMessage categoryMsg(CategoryDev, DtWrite);
	categoryMsg.setFormatType(BrowserFormat);
	categoryMsg.setHfcObject(currentCategory);
	mySession->send(&categoryMsg, Messenger::DEST_UI);
}

#if 0
void CategoryManager::handleChangeFeaturedCategory(SessionMessage *msg)
{
	if (!msg || !dbAccess)
		return;

	FeaturedCategory *msgFeatured = dynamic_cast<FeaturedCategory *> (msg->getHfcObject());
	Debug::trace("CategoryManager.FeaturedCategory", HobString("handleChangeFeaturedCategory(): msgFeatured = ") + HobString(Hex::toString((long) msgFeatured)));
	if (!msgFeatured)
		return;

	Debug::trace("CategoryManager.FeaturedCategory", HobString("handleChangeFeaturedCategory()  ~~~~~~~~~~~~~~~~~~~~~Start~~~~~~~~~~~~~~~~~~~~~~~~"));
	int originalDisplayOrder = 0;
	int newDisplayOrder = msgFeatured->getDisplayOrder();
	Category *prevFeaturedCategory = Category::findCategory(msgFeatured->getOldCategoryNum(), dbAccess);
	Category *newFeaturedCategory = Category::findCategory(msgFeatured->getNewCategoryNum(), dbAccess);

	if (prevFeaturedCategory)	//	Clear the display order here
	{
		originalDisplayOrder = prevFeaturedCategory->getDisplayOrder();
		Debug::trace("CategoryManager.FeaturedCategory", HobString("  prevFeaturedCategory DisplayOrder was ") + HobString(Integer::toString(prevFeaturedCategory->getDisplayOrder())));
		prevFeaturedCategory->clearDisplayOrder();
		prevFeaturedCategory->saveRecord();
		Debug::trace("CategoryManager.FeaturedCategory", HobString("  prevFeaturedCategory DisplayOrder now is ") + HobString(Integer::toString(prevFeaturedCategory->getDisplayOrder())));

		delete prevFeaturedCategory;
	}

	if (originalDisplayOrder != newDisplayOrder)
		Debug::trace("CategoryManager.FeaturedCategory", HobString("handleChangeFeaturedCategory(): Display orders do not match!!!    newDisplayOrder = ") + HobString(Integer::toString(newDisplayOrder)) + "    originalDisplayOrder = " + HobString(Integer::toString(originalDisplayOrder)));

	if (newFeaturedCategory)	//	Set the display order for this category
	{
		newFeaturedCategory->setDisplayOrder(newDisplayOrder);
		newFeaturedCategory->saveRecord();
		Debug::trace("CategoryManager.FeaturedCategory", HobString("  newFeaturedCategory DisplayOrder now is ") + HobString(Integer::toString(newFeaturedCategory->getDisplayOrder())));

		delete newFeaturedCategory;
	}
	Debug::trace("CategoryManager.FeaturedCategory", HobString("handleChangeFeaturedCategory()  ~~~~~~~~~~~~~~~~~~~~~End~~~~~~~~~~~~~~~~~~~~~~~~"));
}
#endif

void CategoryManager::destroyQueriesAndIters()
{
	if (allCategoriesIter)
	{
		delete allCategoriesIter;
		allCategoriesIter = (DBQuery<DBCategory>::const_iterator *)NULL;
	}

	if (allCategoriesQuery)
	{
		delete allCategoriesQuery;
		allCategoriesQuery = (DBQuery<DBCategory> *)NULL;
	}
}
