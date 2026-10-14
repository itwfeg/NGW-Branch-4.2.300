/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "TotalsManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "Configuration.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "HobList.h"
#include "HfcError.h"
#include "ProductTotal.h"
#include "ProductTotalAllTotalTypes.h"
#include "OperatorTotal.h"
#include "OperatorTotalsAll.h"
#include "HourlyTotal.h"
#include "HourlyTotalsAll.h"
#include "Class.h"
#include "ClassTotal.h"
#include "ClassTotalAllTotalTypes.h"
#include "RunModeTotal.h"
#include "ProductManager.h"
#include "ResponseMessageManager.h"
#include "CoolTotal.h"

#include "Long.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#include "DBAccess.h"
#include "DBException.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TotalsManager::dataType = 0;
std::string TotalsManager::dataTypeName("TotalsManager"); 
#endif


TotalsManager::TotalsManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

TotalsManager::TotalsManager(hfc::HFCManagerContainer *container)
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

TotalsManager::~TotalsManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	destroyQueriesAndIters();

	if (currentProductTotal)
		delete currentProductTotal;

	if (currentClassTotal)
		delete currentClassTotal;

	if (currentOperatorTotal)
		delete currentOperatorTotal;

	if (currentHourlyTotal)
		delete currentHourlyTotal;
}


void TotalsManager::init()
{
	mySession = 0;

	currentProductTotal = 0;
	allProductTotalsQuery = 0;
	allProductTotalsIter  = 0;

	currentClassTotal = 0;
	allClassTotalsQuery = 0;
	allClassTotalsIter  = 0;

	currentOperatorTotal = 0;
	allOperatorTotalsQuery = 0;
	allOperatorTotalsIter  = 0;

	currentHourlyTotal = 0;
	allHourlyTotalsQuery = 0;
	allHourlyTotalsIter  = 0;
}


DBSession *TotalsManager::getMySession()
{
	return mySession;
}
void TotalsManager::setMySession(DBSession *session)
{
	mySession = session;
}


void TotalsManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	int event = msg->getEventType();
	if (Debug::isTracing("TotalsManager"))
	{
		HobString msgDebug(" handleMessage():  HfcObject = ");
		msgDebug.append(Hex::toString((long) msg->getHfcObject()));
		msgDebug.append("   device = ").append(Integer::toString(msg->getDeviceType()));
		msgDebug.append("   event = ").append(Integer::toString(event));
		Debug::trace("TotalsManager", msgDebug);
	}

	switch(msg->getDeviceType())
	{
		case Error:
		{
			if (event == ErrRemoveAnyModalDialog)
			{
				HfcResponse *response = dynamic_cast<HfcResponse *> (msg->getHfcObject());
				if (!response || !mySession)
					return;
				if (response->getCode() == TotalsErrProductTotalNotFound)
				{
					Debug::trace("TotalsManager", "handling TotalsErrProductTotalNotFound: currentProductTotal = " + HobString(Long::toString((long) currentProductTotal)));
					if (currentProductTotal)
					{
						SessionMessage pluTotalData(TotalsDev, WriteProductTotalAllTypes);
						pluTotalData.setFormatType(BrowserFormat);
						pluTotalData.setHfcObject(currentProductTotal);

						// Send the product to the UI (Browser)
						mySession->send(&pluTotalData, Messenger::DEST_UI);
					}
				}
				return;
			}
		}
		break;

		case TotalsDev:
		{
			switch(event)
			{
				case DtDeleteAll:
					handleDeleteAllTotals();
					break;

				case ReadProductTotal:
					handleReadProductTotal(msg);
					break;
				case WriteProductTotal:
					handleWriteProductTotal(msg);
					break;
				case DeleteProductTotal:
					handleDeleteProductTotal(msg);
					break;
				case DeleteAllProductTotals:
					handleDeleteAllProductTotals();
					break;
				case SaveProductTotal:
					handleSaveProductTotal(msg);
					break;
				case RestoreProductTotal:
					handleRestoreProductTotal(msg);
					break;
				case VoidProductTotal:
					handleVoidProductTotal(msg);
					break;
				case ReadProductTotalAllTypes:
					handleReadProductTotalAllTypes(msg);
					break;
				case DeleteProductTotalsAllTypes:
					handleDeleteProductTotalAllTypes(msg);
					break;

				case ReadClassTotal:
					handleReadClassTotal(msg);
					break;
				case WriteClassTotal:
					handleWriteClassTotal(msg);
					break;
				case DeleteClassTotal:
					handleDeleteClassTotal(msg);
					break;
				case DeleteAllClassTotals:
					handleDeleteAllClassTotals();
					break;
				case SaveClassTotal:
					handleSaveClassTotal(msg);
					break;
				case RestoreClassTotal:
					handleRestoreClassTotal(msg);
					break;
				case VoidClassTotal:
					handleVoidClassTotal(msg);
					break;
				case ReadClassTotalAllTypes:
					handleReadClassTotalAllTypes(msg);
					break;
				case DeleteClassTotalsAllTypes:
					handleDeleteClassTotalAllTypes(msg);
					break;

				case ReadOperatorTotal:
					handleReadOperatorTotal(msg);
					break;
				case ReadAllOperatorTotals:
					handleReadAllOperatorTotals();
					break;
				case WriteOperatorTotal:
					handleWriteOperatorTotal(msg);
					break;
				case DeleteOperatorTotal:
					handleDeleteOperatorTotal(msg);
					break;
				case DeleteAllOperatorTotals:
					handleDeleteAllOperatorTotals();
					break;
				case SaveOperatorTotal:
					handleSaveOperatorTotal(msg);
					break;
				case RestoreOperatorTotal:
					handleRestoreOperatorTotal(msg);
					break;
				case VoidOperatorTotal:
					handleVoidOperatorTotal(msg);
					break;

				case ReadHourlyTotal:
					handleReadHourlyTotal(msg);
					break;
				case ReadAllHourlyTotals:
					handleReadAllHourlyTotals();
					break;
				case WriteHourlyTotal:
					handleWriteHourlyTotal(msg);
					break;
				case DeleteHourlyTotal:
					handleDeleteHourlyTotal(msg);
					break;
				case DeleteAllHourlyTotals:
					handleDeleteAllHourlyTotals();
					break;
				case SaveHourlyTotal:
					handleSaveHourlyTotal(msg);
					break;
				case RestoreHourlyTotal:
					handleRestoreHourlyTotal(msg);
					break;
				case VoidHourlyTotal:
					handleVoidHourlyTotal(msg);
					break;

				case DeleteAllProductAndClassTotals:
					handleDeleteAllProductAndClassTotals();
					break;

				default:
					Debug::trace("TotalsManager", "TotalsDev - I don't recognize this event!");
					break;
			}
		}
		break;

		default:
			Debug::trace("TotalsManager", "I don't recognize this Device!");
		break;
	}
}


void TotalsManager::handleDeleteAllTotals()
{
	handleDeleteAllProductTotals();
	handleDeleteAllOperatorTotals();
	handleDeleteAllHourlyTotals();
	handleDeleteAllClassTotals();
	handleDeleteAllCOOLTotals();
}

void TotalsManager::handleDeleteAllProductAndClassTotals()
{
	handleDeleteAllProductTotals();
	handleDeleteAllClassTotals();
}

void TotalsManager::addTotalsToDatabase(RunModeTotal *currentTotal)
{
	if (!currentTotal || (currentTotal->getErrorCode() != RECORD_OK))
		return;

	saveToProductTotals(currentTotal);
	saveToHourlyTotals(currentTotal);
	saveToOperatorTotals(currentTotal);
}




/**************************************************************************************************
 **************************************************************************************************
 ***                                                                                            ***
 ***                         Product Totals                                                     ***
 ***                                                                                            ***
 **************************************************************************************************
 **************************************************************************************************/

ProductTotal *TotalsManager::getCurrentProductTotal()
{
	return currentProductTotal;
}
void TotalsManager::setCurrentProductTotal(ProductTotal *newProductTotal)
{
	if (currentProductTotal)
		delete currentProductTotal;
	currentProductTotal = newProductTotal;
}


/* If the first record was not read, this method will return NULL.
 */
ProductTotal *TotalsManager::firstProductTotal()
{
	if (!dbAccess)
		return (ProductTotal *)NULL;

	DBProductTotalsRM	*dbProductTotalsRM = dbAccess->getDBProductTotalsRM();
	if (!dbProductTotalsRM)
		return (ProductTotal *)NULL;

	// If the user calls firstProductTotal() after they have already called it,
	// then delete the old query and create a new one.
	destroyProductTotalsQueryAndIter();

	allProductTotalsQuery = dbProductTotalsRM->retrieveAll();

	// Couldn't get the query from which to access the ProductTotals, just return false.
	if (!allProductTotalsQuery)
		return (ProductTotal *)NULL;

	DBQuery<DBProductTotal>::const_iterator tempIter = allProductTotalsQuery->begin();
	if (tempIter == allProductTotalsQuery->end())
		return (ProductTotal *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextProductTotal()
	 * method call below).
	 */
	allProductTotalsIter = new DBQuery<DBProductTotal>::const_iterator(tempIter);
	DBProductTotal *pt = new DBProductTotal(*(*allProductTotalsIter));
	ProductTotal *newProductTotal = new ProductTotal(pt, dbAccess);
	return newProductTotal;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstProductTotal() method, the first call to this method will
 * result in the first record being returned.
 */
ProductTotal *TotalsManager::nextProductTotal()
{
	if (!dbAccess)
		return (ProductTotal *)NULL;

	if (allProductTotalsQuery && !allProductTotalsQuery->isEnabled())
		destroyProductTotalsQueryAndIter();

	if (!allProductTotalsQuery || !allProductTotalsIter)
		return firstProductTotal();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allProductTotalsIter);
	if (*allProductTotalsIter == allProductTotalsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ProductTotals in the db. */
		destroyProductTotalsQueryAndIter();
		return (ProductTotal *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBProductTotal copy constructor in order for the Hfc ProductTotal to
	 * control the destruction of the DBProductTotal record that the iterator contains; otherwise, the DBProductTotal will
	 * be deleted when the iterator goes out of scope.  The DBProductTotal copy will be deleted by the ProductTotal record
	 * created below.
	 */
	DBProductTotal *pt = new DBProductTotal(*(*allProductTotalsIter));
	ProductTotal *newProductTotal = new ProductTotal(pt, dbAccess);
	return newProductTotal;
}


/* Provide a countAll method so the caller doesn't need to know about a dbProductTotalsRM.
 */
int TotalsManager::countAllProductTotals()
{
	if (!dbAccess)
		return -1;

	DBProductTotalsRM	*dbProductTotalsRM = dbAccess->getDBProductTotalsRM();
	if (!dbProductTotalsRM)
		return -1;

	return dbProductTotalsRM->countAll();
}


void TotalsManager::saveToProductTotals(RunModeTotal *currentTotal)
{
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveToProductTotals()  ";
		Debug::trace("TotalsManager", msgDebug);
	}

	if (!currentTotal || !Configuration::getInstance()->isCollectingProductTotals() || !dbAccess)
		return;

	Product *theProduct = Product::findPLU(currentTotal->getProductNum(), dbAccess);
	if (!theProduct)
		return;

	DBTotalTypesRM	*dbTotalTypesRM = dbAccess->getDBTotalTypesRM();
	if (dbTotalTypesRM)
	{
		HobString typeString = Long::toString(currentTotal->getTotalTypeNum());
		DBTotalType *theDbTotalType = dbTotalTypesRM->findNumber(typeString);
		if (theDbTotalType)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("ProductTotal # ").append(Long::toString(currentTotal->getProductNum()));
				msgDebug2.append("  TotalType # ").append(typeString);
				msgDebug2.append("  ReportsToPrepackTotals = ").append(Bool::toString(theDbTotalType->getReportsToPrepackTotals()));
				Debug::trace("TotalsManager", msgDebug2);
			}
			saveToProductTotalsPerType(currentTotal, theProduct->getMyDbRecord(), theDbTotalType);

			if (theDbTotalType->getReportsToPrepackTotals())
			{
				typeString = Long::toString(TotalType::PREPACK_TOTALTYPENUM);
				DBTotalType *prepackDbTotalType = dbTotalTypesRM->findNumber(typeString);
				if (prepackDbTotalType)
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("ProductTotal # ").append(Long::toString(currentTotal->getProductNum()));
						msgDebug2.append("  TotalType # ").append(typeString);
						Debug::trace("TotalsManager", msgDebug2);
					}
					saveToProductTotalsPerType(currentTotal, theProduct->getMyDbRecord(), prepackDbTotalType);
					delete prepackDbTotalType;
				}
			}

			//	Now save the Class totals
			saveToClassTotals(theProduct->getReportingClassNum(), currentTotal, theDbTotalType);
			delete theDbTotalType;
		}
	}
	delete theProduct;
}

void TotalsManager::saveToProductTotalsPerType(RunModeTotal *currentTotal, DBProduct* dbProduct, DBTotalType* dbTotalType)
{
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveToProductTotalsPerType()  ";
		Debug::trace("TotalsManager", msgDebug + "START");

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("ProductTotal # ").append(dbProduct->getPLU());
		msgDebug2.append("  TotalType # ").append(dbTotalType->getNumber());
		Debug::trace("TotalsManager", msgDebug2);
	}

	if (!(currentTotal && dbProduct && dbTotalType))
		return;

	if (!dbAccess)
		return;

	ProductTotal *lookedUpTotal = ProductTotal::findProductTotal(dbProduct, dbTotalType, dbAccess);
	if (!lookedUpTotal)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Creating new ProductTotal # ").append(dbProduct->getPLU());
			Debug::trace("TotalsManager", msgDebug2);
		}
		lookedUpTotal = new ProductTotal(currentTotal, dbAccess);
		if (lookedUpTotal)
			lookedUpTotal->setTotalTypeNum(Long::valueOf(dbTotalType->getNumber()));
	}
	else
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Updating ProductTotal # ").append(dbProduct->getPLU());
			Debug::trace("TotalsManager", msgDebug2);
		}
		lookedUpTotal->update(currentTotal);
	}
	if (lookedUpTotal)
	{
		lookedUpTotal->show();
		lookedUpTotal->saveRecord();
		delete lookedUpTotal;
	}
	if (traceOn)
		Debug::trace("TotalsManager", msgDebug + "END");
}


void TotalsManager::handleReadProductTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (msgTotal->getProductNum() > 0)
	{
		ProductTotal *lookedUpTotal = ProductTotal::findProductTotal(msgTotal->getProductNum(), msgTotal->getTotalTypeNum(), dbAccess);
		if (!lookedUpTotal)
		{
			//	Set up a blank ProductTotal
			lookedUpTotal = new ProductTotal();
			lookedUpTotal->setProductNum(msgTotal->getProductNum());
			lookedUpTotal->setTotalTypeNum(msgTotal->getTotalTypeNum());
			lookedUpTotal->setDBAccess(dbAccess);
		}
		if (lookedUpTotal)
		{
			// Send the Total found back to the Browser.
			SessionMessage totalData(TotalsDev, WriteProductTotal);
			totalData.setFormatType(BrowserFormat);
			totalData.setHfcObject(lookedUpTotal);

			// Send the Total to the UI (Browser)
			mySession->send(&totalData, Messenger::DEST_UI);

			// We have a new current total type.
			setCurrentProductTotal(lookedUpTotal);
		}
	}
}

void TotalsManager::handleWriteProductTotal(SessionMessage *msg)
{
	if (!msg)
		return;

	if (currentProductTotal)
	{
		ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
		if (!msgTotal)
			return;

		currentProductTotal->update(msgTotal);
	}
}

void TotalsManager::handleDeleteProductTotal(SessionMessage *msg)
{
	bool usingCurrent = false;
	ProductTotal *msgTotal = (ProductTotal *) NULL;

	if (!msg || !dbAccess)
		return;

	msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgTotal)
	{
		if (currentProductTotal)
			usingCurrent = true;
		else
			return;
	}
	ProductTotal *existingProductTotal = (usingCurrent ? currentProductTotal : ProductTotal::findProductTotal(msgTotal->getProductNum(), msgTotal->getTotalTypeNum(), dbAccess));
	if (existingProductTotal && existingProductTotal->getProductNum() != RECORD_NOT_FOUND)
	{
		existingProductTotal->deleteRecord();
		Debug::trace("TotalsManager", HobString("handleDelete - product total deleted - productnum:") +
														HobString(Long::toString((long)existingProductTotal->getProductNum())));
		if (usingCurrent)
			setCurrentProductTotal((ProductTotal*) NULL);
		else
			delete existingProductTotal;
	}
	else
		Debug::trace("TotalsManager", HobString("handleDelete - product total to be deleted was not in the database to delete - productnum:") +
														HobString((existingProductTotal) ? Long::toString((long)existingProductTotal->getProductNum()) : "???"));
}

void TotalsManager::handleDeleteAllProductTotals()
{
	if (!dbAccess)
		return;

	DBProductTotalsRM	*dbProductTotalsRM = dbAccess->getDBProductTotalsRM();
	if (!dbProductTotalsRM)
		return;

	dbProductTotalsRM->deleteAll();

	destroyProductTotalsQueryAndIter();
}

void TotalsManager::handleSaveProductTotal(SessionMessage *msg)
{
	if (currentProductTotal)
	{
		if (msg)
		{
			ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
			if (msgTotal)
				currentProductTotal->update(msgTotal);
		}
		currentProductTotal->saveRecord();
	}
}

/* This method is only used when restoring product total records into an empty
 * database after a firmware upgrade that indicates a new version of the database
 * is being installed.  There shouldn't be any product total records in the database
 * with which to update the data being restored.  Thus, we don't need to look for an
 * existing record as is done for other datatypes.  Just go ahead and save the product total
 * record that is received.
 */
void TotalsManager::handleRestoreProductTotal(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	Debug::trace("TotalsManager", HobString("handleRestoreProductTotal - prnum:") + HobString(Long::toString(msgTotal->getProductNum())) +
								HobString("  ttypenum:") + HobString(Long::toString(msgTotal->getTotalTypeNum())));
	msgTotal->setDBAccess(dbAccess);
	msgTotal->saveRecord();
}

void TotalsManager::handleVoidProductTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (currentProductTotal)
	{
		currentProductTotal->voidTotal(msgTotal);
		currentProductTotal->saveRecord();

		// Send the Modified Total back to the Browser.
		SessionMessage totalData(TotalsDev, WriteProductTotal);
		totalData.setFormatType(BrowserFormat);
		totalData.setHfcObject(currentProductTotal);

		// Send the Total to the UI (Browser)
		mySession->send(&totalData, Messenger::DEST_UI);
	}
}

void TotalsManager::handleReadProductTotalAllTypes(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ProductTotal *msgProductTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgProductTotal || !dbAccess)
		return;

	Debug::trace("TotalsManager", "handleReadProductTotalAllTypes");
	if (msgProductTotal->getProductNum() != RECORD_NOT_FOUND)
	{
		ProductTotalAllTotalTypes *allTypesProductTotal = new ProductTotalAllTotalTypes(msgProductTotal->getProductNum(), dbAccess);
		if (allTypesProductTotal && (allTypesProductTotal->getListCount() > 0))
		{
			// Send the ProductTotal data found back to the Browser.
			SessionMessage productTotalsData(TotalsDev, WriteProductTotalAllTypes);
			productTotalsData.setFormatType(BrowserFormat);
			productTotalsData.setHfcObject(allTypesProductTotal);

			// Send the TotalType to the UI (Browser)
			mySession->send(&productTotalsData, Messenger::DEST_UI);
		}
		else
		{
			//	Totals record not found.
			mySession->addDialogMessage(ErrTotalsDev, TotalsErrProductTotalNotFound, HfcResponse::RESPONSESTYLE_OK);
		}
	}
}

void TotalsManager::handleDeleteProductTotalAllTypes(SessionMessage *msg)
{
	if (!msg)
		return;

	ProductTotal *msgTotal = dynamic_cast<ProductTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

	DBProductTotalsRM	*dbProductTotalsRM = dbAccess->getDBProductTotalsRM();
	if (!dbProductTotalsRM)
		return;

	HobRecordNum recordNum = msgTotal->getProductNum();
	if (recordNum != RECORD_NOT_FOUND)
	{
		DBProduct *dbProduct = dbProductsRM->findPLU(Long::toString(recordNum));
		if (dbProduct)
		{
			DBQuery<DBProductTotal>* recordQuery = dbProductTotalsRM->retrieveLinkedtoProduct(dbProduct);
			if (recordQuery)
			{
				DBQuery<DBProductTotal>::const_iterator tempIter = recordQuery->begin();
				while (tempIter != recordQuery->end())
				{
					dbProductTotalsRM->deleteRecord(&*tempIter);
					tempIter++;;
				}
				delete recordQuery;
			}
			delete dbProduct;
		}
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***                                                                                            ***
 ***                         Class Totals                                                       ***
 ***                                                                                            ***
 **************************************************************************************************
 **************************************************************************************************/

ClassTotal *TotalsManager::getCurrentClassTotal()
{
	return currentClassTotal;
}
void TotalsManager::setCurrentClassTotal(ClassTotal *newClassTotal)
{
	if (currentClassTotal)
		delete currentClassTotal;
	currentClassTotal = newClassTotal;
}


/* If the first record was not read, this method will return NULL.
 */
ClassTotal *TotalsManager::firstClassTotal()
{
	if (!dbAccess)
		return (ClassTotal *)NULL;

	DBClassTotalsRM *dbClassTotalsRM = dbAccess->getDBClassTotalsRM();
	if (!dbClassTotalsRM)
		return (ClassTotal *)NULL;

	// If the user calls firstClassTotal() after they have already called it,
	// then delete the old query and create a new one.
	destroyClassTotalsQueryAndIter();

	allClassTotalsQuery = dbClassTotalsRM->retrieveAll();

	// Couldn't get the query from which to access the ClassTotals, just return false.
	if (!allClassTotalsQuery)
		return (ClassTotal *)NULL;

	DBQuery<DBClassTotal>::const_iterator tempIter = allClassTotalsQuery->begin();
	if (tempIter == allClassTotalsQuery->end())
		return (ClassTotal *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextClassTotal()
	 * method call below).
	 */
	allClassTotalsIter = new DBQuery<DBClassTotal>::const_iterator(tempIter);
	DBClassTotal *ct = new DBClassTotal(*(*allClassTotalsIter));
	ClassTotal *newClassTotal = new ClassTotal(ct, dbAccess);
	return newClassTotal;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstClassTotal() method, the first call to this method will
 * result in the first record being returned.
 */
ClassTotal *TotalsManager::nextClassTotal()
{
	if (!dbAccess)
		return (ClassTotal *)NULL;

	if (allClassTotalsQuery && !allClassTotalsQuery->isEnabled())
		destroyClassTotalsQueryAndIter();

	if (!allClassTotalsQuery || !allClassTotalsIter)
		return firstClassTotal();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allClassTotalsIter);
	if (*allClassTotalsIter == allClassTotalsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ClassTotals in the db. */
		destroyClassTotalsQueryAndIter();
		return (ClassTotal *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBClassTotal copy constructor in order for the Hfc ClassTotal to
	 * control the destruction of the DBClassTotal record that the iterator contains; otherwise, the DBClassTotal will
	 * be deleted when the iterator goes out of scope.  The DBClassTotal copy will be deleted by the ClassTotal record
	 * created below.
	 */
	DBClassTotal *ct = new DBClassTotal(*(*allClassTotalsIter));
	ClassTotal *newClassTotal = new ClassTotal(ct, dbAccess);
	return newClassTotal;
}


/* Provide a countAll method so the caller doesn't need to know about a dbClassTotalsRM.
 */
int TotalsManager::countAllClassTotals()
{
	if (!dbAccess)
		return -1;

	DBClassTotalsRM *dbClassTotalsRM = dbAccess->getDBClassTotalsRM();
	if (!dbClassTotalsRM)
		return -1;

	return dbClassTotalsRM->countAll();
}


void TotalsManager::saveToClassTotals(HobRecordNum classNum, RunModeTotal *currentTotal, DBTotalType *theDbTotalType)
{
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveToClassTotals()  ";
		Debug::trace("TotalsManager", msgDebug + "START");

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("ClassTotal # ").append(Integer::toString(classNum));
		msgDebug2.append("  TotalType # ").append(theDbTotalType->getNumber());
		Debug::trace("TotalsManager", msgDebug2);
	}

	if (!currentTotal || !theDbTotalType || !Configuration::getInstance()->isCollectingProductTotals() || !dbAccess)
		return;

	DBTotalTypesRM	*dbTotalTypesRM = dbAccess->getDBTotalTypesRM();
	if (!dbTotalTypesRM)
		return;

	HobRecordNum repClassNum = classNum;
	Class *theClass = (Class *) NULL;

	DBTotalType *prepackDbTotalType = (DBTotalType *) NULL;

	if (theDbTotalType->getReportsToPrepackTotals())
		prepackDbTotalType = dbTotalTypesRM->findNumber(HobString(Long::toString(TotalType::PREPACK_TOTALTYPENUM)));

	while ((repClassNum != RECORD_NOT_FOUND) && (repClassNum <= GRAND_TOTALS_CLASS))
	{
		theClass = Class::findClass(repClassNum, dbAccess);
		if (!theClass)
			repClassNum = RECORD_NOT_FOUND;
		else
		{
			saveToClassTotalsPerType(currentTotal, theClass->getMyDbRecord(), theDbTotalType);
			if (prepackDbTotalType)		//	Report to Prepack
				saveToClassTotalsPerType(currentTotal, theClass->getMyDbRecord(), prepackDbTotalType);

			if (repClassNum == GRAND_TOTALS_CLASS)
				repClassNum = RECORD_NOT_FOUND;
			else
				repClassNum = theClass->getReportingClassNum();

			delete theClass;
		}
	}
	if (prepackDbTotalType)
		delete prepackDbTotalType;

	if (traceOn)
		Debug::trace("TotalsManager", msgDebug + "END");
}


void TotalsManager::saveToClassTotalsPerType(RunModeTotal *currentTotal, DBClass* theDbClass, DBTotalType* dbTotalType)
{
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveToClassTotalsPerType()   ";
		Debug::trace("TotalsManager", msgDebug + "START");

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("ClassTotal # ").append(theDbClass->getNumber());
		msgDebug2.append("  TotalType # ").append(Integer::toString(currentTotal->getTotalTypeNum()));
		Debug::trace("TotalsManager", msgDebug2);
	}

	if (!(currentTotal && theDbClass && dbTotalType))
		return;

	if (!dbAccess)
		return;

	ClassTotal *lookedUpTotal = ClassTotal::findClassTotal(theDbClass, dbTotalType, dbAccess);
	if (!lookedUpTotal)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Creating new ClassTotal # ").append(theDbClass->getNumber());
			Debug::trace("TotalsManager", msgDebug2);
		}
		lookedUpTotal = new ClassTotal(currentTotal, Long::valueOf(theDbClass->getNumber()), dbAccess);
		if (lookedUpTotal)
			lookedUpTotal->setTotalTypeNum(Long::valueOf(dbTotalType->getNumber()));
	}
	else
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Updating ClassTotal # ").append(theDbClass->getNumber());
			Debug::trace("TotalsManager", msgDebug2);
		}
		lookedUpTotal->update(currentTotal);
	}
	if (lookedUpTotal)
	{
		lookedUpTotal->show();
		lookedUpTotal->saveRecord();
		delete lookedUpTotal;
	}
	if (traceOn)
		Debug::trace("TotalsManager", msgDebug + "END");
}


void TotalsManager::handleReadClassTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ClassTotal *msgTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (msgTotal->getClassNum() > 0)
	{
		ClassTotal *lookedUpTotal = ClassTotal::findClassTotal(msgTotal->getClassNum(), msgTotal->getTotalTypeNum(), dbAccess);
		if (!lookedUpTotal)
		{
			//	Set up a blank ClassTotal
			lookedUpTotal = new ClassTotal();
			lookedUpTotal->setClassNum(msgTotal->getClassNum());
			lookedUpTotal->setTotalTypeNum(msgTotal->getTotalTypeNum());
			lookedUpTotal->setDBAccess(dbAccess);
		}
		if (lookedUpTotal)
		{
			// Send the Total found back to the Browser.
			SessionMessage totalData(TotalsDev, WriteClassTotal);
			totalData.setFormatType(BrowserFormat);
			totalData.setHfcObject(lookedUpTotal);

			// Send the Total to the UI (Browser)
			mySession->send(&totalData, Messenger::DEST_UI);

			// We have a new current total type.
			setCurrentClassTotal(lookedUpTotal);
		}
	}
}

void TotalsManager::handleWriteClassTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

void TotalsManager::handleDeleteClassTotal(SessionMessage *msg)
{
	bool usingCurrent = false;
	ClassTotal *msgTotal = (ClassTotal *) NULL;

	if (!msg || !dbAccess)
		return;

	msgTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgTotal)
	{
		if (currentClassTotal)
			usingCurrent = true;
		else
			return;
	}
	ClassTotal *existingClassTotal = (usingCurrent ? currentClassTotal : ClassTotal::findClassTotal(msgTotal->getClassNum(), msgTotal->getTotalTypeNum(), dbAccess));
	if (existingClassTotal && existingClassTotal->getClassNum() != RECORD_NOT_FOUND)
	{
		existingClassTotal->deleteRecord();
		Debug::trace("TotalsManager", HobString("handleDelete - class total deleted - classnum:") +
														HobString(Long::toString((long)existingClassTotal->getClassNum())));
		if (usingCurrent)
			setCurrentClassTotal((ClassTotal*) NULL);
		else
			delete existingClassTotal;
	}
	else
		Debug::trace("TotalsManager", HobString("handleDelete - class total to be deleted was not in the database to delete - classnum:") +
														HobString((existingClassTotal) ? Long::toString((long)existingClassTotal->getClassNum()) : "???"));
}

void TotalsManager::handleDeleteAllClassTotals()
{
	if (!dbAccess)
		return;

	DBClassTotalsRM *dbClassTotalsRM = dbAccess->getDBClassTotalsRM();
	if (!dbClassTotalsRM)
		return;

	dbClassTotalsRM->deleteAll();

	destroyClassTotalsQueryAndIter();

	//	When deleting Class totals, delete the reserved Class records also.
	//	That way, the created date will change for the totals.
	Class::recreateReservedRecords(dbAccess);
}

void TotalsManager::handleSaveClassTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

/* This method is only used when restoring class total records into an empty
 * database after a firmware upgrade that indicates a new version of the database
 * is being installed.  There shouldn't be any class total records in the database
 * with which to update the data being restored.  Thus, we don't need to look for an
 * existing record as is done for other datatypes.  Just go ahead and save the class total
 * record that is received.
 */
void TotalsManager::handleRestoreClassTotal(SessionMessage *msg)
{
	if (!msg)
		return;

	ClassTotal *msgTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (Debug::isTracing("TotalsManager"))
	{
		HobString msgDebug("handleRestoreClassTotal - clnum: ");
		msgDebug.append(Long::toString(msgTotal->getClassNum())).append("  ttypenum: ").append(Long::toString(msgTotal->getTotalTypeNum()));
		Debug::trace("TotalsManager", msgDebug);
	}
	msgTotal->setDBAccess(dbAccess);
	msgTotal->saveRecord();
}

void TotalsManager::handleVoidClassTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ClassTotal *msgTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (currentClassTotal)
	{
		currentClassTotal->voidTotal(msgTotal);
		currentClassTotal->saveRecord();

		// Send the Modified Total back to the Browser.
		SessionMessage totalData(TotalsDev, WriteClassTotal);
		totalData.setFormatType(BrowserFormat);
		totalData.setHfcObject(currentClassTotal);

		// Send the Total to the UI (Browser)
		mySession->send(&totalData, Messenger::DEST_UI);
	}
}

void TotalsManager::handleReadClassTotalAllTypes(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	ClassTotal *msgClassTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgClassTotal || !dbAccess)
		return;

	Debug::trace("TotalsManager", "handleReadClassTotalAllTypes");
	if (msgClassTotal->getClassNum() != RECORD_NOT_FOUND)
	{
		ClassTotalAllTotalTypes *allTypesClassTotal = new ClassTotalAllTotalTypes(msgClassTotal->getClassNum(), dbAccess);
		if (allTypesClassTotal && (allTypesClassTotal->getListCount() > 0))
		{
			// Send the ClassTotal data found back to the Browser.
			SessionMessage classTotalsData(TotalsDev, WriteClassTotalAllTypes);
			classTotalsData.setFormatType(BrowserFormat);
			classTotalsData.setHfcObject(allTypesClassTotal);

			// Send the TotalType to the UI (Browser)
			mySession->send(&classTotalsData, Messenger::DEST_UI);
		}
	}
}

void TotalsManager::handleDeleteClassTotalAllTypes(SessionMessage *msg)
{
	if (!msg)
		return;

	ClassTotal *msgTotal = dynamic_cast<ClassTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	DBClassesRM *dbClassesRM = dbAccess->getDBClassesRM();
	if (!dbClassesRM)
		return;

	DBClassTotalsRM *dbClassTotalsRM = dbAccess->getDBClassTotalsRM();
	if (!dbClassTotalsRM)
		return;

	HobRecordNum recordNum = msgTotal->getClassNum();
	if (recordNum != RECORD_NOT_FOUND)
	{
		DBClass *theDbClass = dbClassesRM->findNumber(Long::toString(recordNum));
		if (theDbClass)
		{
			DBQuery<DBClassTotal>* recordQuery = dbClassTotalsRM->retrieveLinkedtoClass(theDbClass);
			if (recordQuery)
			{
				DBQuery<DBClassTotal>::const_iterator tempIter = recordQuery->begin();
				while (tempIter != recordQuery->end())
				{
					dbClassTotalsRM->deleteRecord(&*tempIter);
					tempIter++;;
				}
				delete recordQuery;
			}
			delete theDbClass;
		}
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***                                                                                            ***
 ***                         Operator Totals                                                    ***
 ***                                                                                            ***
 **************************************************************************************************
 **************************************************************************************************/

OperatorTotal *TotalsManager::getCurrentOperatorTotal()
{
	return currentOperatorTotal;
}
void TotalsManager::setCurrentOperatorTotal(OperatorTotal *newOperatorTotal)
{
	if (currentOperatorTotal)
		delete currentOperatorTotal;
	currentOperatorTotal = newOperatorTotal;
}


/* If the first record was not read, this method will return NULL.
 */
OperatorTotal *TotalsManager::firstOperatorTotal()
{
	if (!dbAccess)
		return (OperatorTotal *)NULL;

	DBOperatorTotalsRM *dbOperatorTotalsRM = dbAccess->getDBOperatorTotalsRM();
	if (!dbOperatorTotalsRM)
		return (OperatorTotal *)NULL;

	// If the user calls firstOperatorTotal() after they have already called it,
	// then delete the old query and create a new one.
	destroyOperatorTotalsQueryAndIter();

	allOperatorTotalsQuery = dbOperatorTotalsRM->retrieveAll();

	// Couldn't get the query from which to access the OperatorTotals, just return false.
	if (!allOperatorTotalsQuery)
		return (OperatorTotal *)NULL;

	DBQuery<DBOperatorTotal>::const_iterator tempIter = allOperatorTotalsQuery->begin();
	if (tempIter == allOperatorTotalsQuery->end())
		return (OperatorTotal *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextOperatorTotal()
	 * method call below).
	 */
	allOperatorTotalsIter = new DBQuery<DBOperatorTotal>::const_iterator(tempIter);
	DBOperatorTotal *ot = new DBOperatorTotal(*(*allOperatorTotalsIter));
	OperatorTotal *newOperatorTotal = new OperatorTotal(ot, dbAccess);
	return newOperatorTotal;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstOperatorTotal() method, the first call to this method will
 * result in the first record being returned.
 */
OperatorTotal *TotalsManager::nextOperatorTotal()
{
	if (!dbAccess)
		return (OperatorTotal *)NULL;

	if (allOperatorTotalsQuery && !allOperatorTotalsQuery->isEnabled())
		destroyOperatorTotalsQueryAndIter();

	if (!allOperatorTotalsQuery || !allOperatorTotalsIter)
		return firstOperatorTotal();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allOperatorTotalsIter);
	if (*allOperatorTotalsIter == allOperatorTotalsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all OperatorTotals in the db. */
		destroyOperatorTotalsQueryAndIter();
		return (OperatorTotal *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBOperatorTotal copy constructor in order for the Hfc OperatorTotal to
	 * control the destruction of the DBOperatorTotal record that the iterator contains; otherwise, the DBOperatorTotal will
	 * be deleted when the iterator goes out of scope.  The DBOperatorTotal copy will be deleted by the OperatorTotal record
	 * created below.
	 */
	DBOperatorTotal *ot = new DBOperatorTotal(*(*allOperatorTotalsIter));
	OperatorTotal *newOperatorTotal = new OperatorTotal(ot, dbAccess);
	return newOperatorTotal;
}


/* Provide a countAll method so the caller doesn't need to know about a dbOperatorTotalsRM.
 */
int TotalsManager::countAllOperatorTotals()
{
	if (!dbAccess)
		return -1;

	DBOperatorTotalsRM *dbOperatorTotalsRM = dbAccess->getDBOperatorTotalsRM();
	if (!dbOperatorTotalsRM)
		return -1;

	return dbOperatorTotalsRM->countAll();
}


void TotalsManager::saveToOperatorTotals(RunModeTotal *currentTotal)
{
	if (!currentTotal || !Configuration::getInstance()->isCollectingOperatorTotals() || !dbAccess)
		return;

	Debug::trace("TotalsManager", "*****  saveToOperatorTotals()  *****");
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug("saveToOperatorTotals():  ");

	if (currentTotal->getOperatorId().size() > 0)
	{
		OperatorTotal *lookedUpTotal = OperatorTotal::findOperatorTotal(currentTotal->getOperatorId(), dbAccess);
		if (!lookedUpTotal)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("New OperatorTotal # ").append(currentTotal->getOperatorId());
				Debug::trace("TotalsManager", msgDebug2);
			}
			lookedUpTotal = new OperatorTotal(currentTotal, dbAccess);
		}
		else
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Updating OperatorTotal # ").append(currentTotal->getOperatorId());
				Debug::trace("TotalsManager", msgDebug2);
			}
			lookedUpTotal->update(currentTotal);
		}
		if (lookedUpTotal)
		{
			lookedUpTotal->show();
			lookedUpTotal->saveRecord();
			delete lookedUpTotal;
		}
	}
}


void TotalsManager::handleReadOperatorTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	OperatorTotal *msgTotal = dynamic_cast<OperatorTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (msgTotal->getOperatorId().size() > 0)
	{
		OperatorTotal *lookedUpTotal = OperatorTotal::findOperatorTotal(msgTotal->getOperatorId(), dbAccess);
		if (!lookedUpTotal)
		{
			//	Set up a blank OperatorTotal
			lookedUpTotal = new OperatorTotal();
			lookedUpTotal->setOperatorId(msgTotal->getOperatorId());
			lookedUpTotal->setDBAccess(dbAccess);
		}
		if (lookedUpTotal)
		{
			// Send the Total found back to the Browser.
			SessionMessage totalData(TotalsDev, WriteOperatorTotal);
			totalData.setFormatType(BrowserFormat);
			totalData.setHfcObject(lookedUpTotal);

			// Send the Total to the UI (Browser)
			mySession->send(&totalData, Messenger::DEST_UI);

			// We have a new current total type.
			setCurrentOperatorTotal(lookedUpTotal);
		}
	}
}

void TotalsManager::handleReadAllOperatorTotals()
{
	if (!mySession || !dbAccess)
		return;

	OperatorTotalsAll *allOperatorTotals = new OperatorTotalsAll(mySession->getOperatorManager(), dbAccess);
	if (allOperatorTotals && (allOperatorTotals->getListCount() > 0))
	{
		// Send the ProductTotal data found back to the Browser.
		SessionMessage operTotalsData(TotalsDev, WriteAllOperatorTotals);
		operTotalsData.setFormatType(BrowserFormat);
		operTotalsData.setHfcObject(allOperatorTotals);

		// Send the TotalType to the UI (Browser)
		mySession->send(&operTotalsData, Messenger::DEST_UI);

		delete allOperatorTotals;
	}
}

void TotalsManager::handleWriteOperatorTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

void TotalsManager::handleDeleteOperatorTotal(SessionMessage *msg)
{
	bool usingCurrent = false;
	OperatorTotal *msgTotal = (OperatorTotal *) NULL;

	if (!msg || !dbAccess)
		return;

	msgTotal = dynamic_cast<OperatorTotal *> (msg->getHfcObject());
	if (!msgTotal)
	{
		if (currentOperatorTotal)
			usingCurrent = true;
		else
			return;
	}
	OperatorTotal *existingOperatorTotal = (usingCurrent ? currentOperatorTotal : OperatorTotal::findOperatorTotal(msgTotal->getOperatorId(), dbAccess));
	if (existingOperatorTotal && existingOperatorTotal->getOperatorId().size() != 0)
	{
		existingOperatorTotal->deleteRecord();
		Debug::trace("TotalsManager", HobString("handleDelete - operator total deleted - operatorId: ") + existingOperatorTotal->getOperatorId());
		if (usingCurrent)
			setCurrentOperatorTotal((OperatorTotal*) NULL);
		else
			delete existingOperatorTotal;
	}
	else
		Debug::trace("TotalsManager", HobString("handleDelete - operator total to be deleted was not in the database to delete - operatorId: ") +
														HobString((existingOperatorTotal) ? existingOperatorTotal->getOperatorId() : "???"));
}

void TotalsManager::handleDeleteAllOperatorTotals()
{
	if (!dbAccess)
		return;

	DBOperatorTotalsRM *dbOperatorTotalsRM = dbAccess->getDBOperatorTotalsRM();
	if (!dbOperatorTotalsRM)
		return;

	dbOperatorTotalsRM->deleteAll();

	destroyOperatorTotalsQueryAndIter();
}

void TotalsManager::handleSaveOperatorTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

/* This method is only used when restoring operator total records into an empty
 * database after a firmware upgrade that indicates a new version of the database
 * is being installed.  There shouldn't be any operator total records in the database
 * with which to update the data being restored.  Thus, we don't need to look for an
 * existing record as is done for other datatypes.  Just go ahead and save the operator total
 * record that is received.
 */
void TotalsManager::handleRestoreOperatorTotal(SessionMessage *msg)
{
	if (!msg)
		return;

	OperatorTotal *msgTotal = dynamic_cast<OperatorTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	Debug::trace("TotalsManager", HobString("handleRestoreOperatorTotal - opnum:") + msgTotal->getOperatorId());
	msgTotal->setDBAccess(dbAccess);
	msgTotal->saveRecord();
}

void TotalsManager::handleVoidOperatorTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	OperatorTotal *msgTotal = dynamic_cast<OperatorTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (currentOperatorTotal)
	{
		currentOperatorTotal->voidTotal(msgTotal);
		currentOperatorTotal->saveRecord();

		// Send the Modified Total back to the Browser.
		SessionMessage totalData(TotalsDev, WriteOperatorTotal);
		totalData.setFormatType(BrowserFormat);
		totalData.setHfcObject(currentOperatorTotal);

		// Send the Total to the UI (Browser)
		mySession->send(&totalData, Messenger::DEST_UI);
	}
}



/**************************************************************************************************
 **************************************************************************************************
 ***                                                                                            ***
 ***                         Hourly Totals                                                      ***
 ***                                                                                            ***
 **************************************************************************************************
 **************************************************************************************************/

HourlyTotal *TotalsManager::getCurrentHourlyTotal()
{
	return currentHourlyTotal;
}
void TotalsManager::setCurrentHourlyTotal(HourlyTotal *newHourlyTotal)
{
	if (currentHourlyTotal)
		delete currentHourlyTotal;
	currentHourlyTotal = newHourlyTotal;
}


/* If the first record was not read, this method will return NULL.
 */
HourlyTotal *TotalsManager::firstHourlyTotal()
{
	if (!dbAccess)
		return (HourlyTotal *)NULL;

	DBHourlyTotalsRM *dbHourlyTotalsRM = dbAccess->getDBHourlyTotalsRM();
	if (!dbHourlyTotalsRM)
		return (HourlyTotal *)NULL;

	// If the user calls firstHourlyTotal() after they have already called it,
	// then delete the old query and create a new one.
	destroyHourlyTotalsQueryAndIter();

	allHourlyTotalsQuery = dbHourlyTotalsRM->retrieveAll();

	// Couldn't get the query from which to access the HourlyTotals, just return false.
	if (!allHourlyTotalsQuery)
		return (HourlyTotal *)NULL;

	DBQuery<DBHourlyTotal>::const_iterator tempIter = allHourlyTotalsQuery->begin();
	if (tempIter == allHourlyTotalsQuery->end())
		return (HourlyTotal *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextHourlyTotal()
	 * method call below).
	 */
	allHourlyTotalsIter = new DBQuery<DBHourlyTotal>::const_iterator(tempIter);
	DBHourlyTotal *ht = new DBHourlyTotal(*(*allHourlyTotalsIter));
	HourlyTotal *newHourlyTotal = new HourlyTotal(ht, dbAccess);
	return newHourlyTotal;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstHourlyTotal() method, the first call to this method will
 * result in the first record being returned.
 */
HourlyTotal *TotalsManager::nextHourlyTotal()
{
	if (!dbAccess)
		return (HourlyTotal *)NULL;

	if (allHourlyTotalsQuery && !allHourlyTotalsQuery->isEnabled())
		destroyHourlyTotalsQueryAndIter();

	if (!allHourlyTotalsQuery || !allHourlyTotalsIter)
		return firstHourlyTotal();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allHourlyTotalsIter);
	if (*allHourlyTotalsIter == allHourlyTotalsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all HourlyTotals in the db. */
		destroyHourlyTotalsQueryAndIter();
		return (HourlyTotal *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBHourlyTotal copy constructor in order for the Hfc HourlyTotal to
	 * control the destruction of the DBHourlyTotal record that the iterator contains; otherwise, the DBHourlyTotal will
	 * be deleted when the iterator goes out of scope.  The DBHourlyTotal copy will be deleted by the HourlyTotal record
	 * created below.
	 */
	DBHourlyTotal *ht = new DBHourlyTotal(*(*allHourlyTotalsIter));
	HourlyTotal *newHourlyTotal = new HourlyTotal(ht, dbAccess);
	return newHourlyTotal;
}


/* Provide a countAll method so the caller doesn't need to know about a dbHourlyTotalsRM.
 */
int TotalsManager::countAllHourlyTotals()
{
	if (!dbAccess)
		return -1;

	DBHourlyTotalsRM *dbHourlyTotalsRM = dbAccess->getDBHourlyTotalsRM();
	if (!dbHourlyTotalsRM)
		return -1;

	return dbHourlyTotalsRM->countAll();
}


void TotalsManager::saveToHourlyTotals(RunModeTotal *currentTotal)
{
	if (!currentTotal || !Configuration::getInstance()->isCollectingHourlyTotals() || !dbAccess)
		return;

	Debug::trace("TotalsManager", "*****  saveToHourlyTotals()  *****");
	bool traceOn = Debug::isTracing("TotalsManager");
	HobString msgDebug("saveToHourlyTotals():  ");

	if (currentTotal->getHour() != RECORD_NOT_FOUND)
	{
		HourlyTotal *lookedUpTotal = HourlyTotal::findHourlyTotal(currentTotal->getHour(), dbAccess);
		if (!lookedUpTotal)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("New HourlyTotal # ").append(Integer::toString(currentTotal->getHour()));
				Debug::trace("TotalsManager", msgDebug2);
			}
			lookedUpTotal = new HourlyTotal(currentTotal, dbAccess);
		}
		else
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Updating HourlyTotal # ").append(Integer::toString(currentTotal->getHour()));
				Debug::trace("TotalsManager", msgDebug2);
			}
			lookedUpTotal->update(currentTotal);
		}
		if (lookedUpTotal)
		{
			lookedUpTotal->show();
			lookedUpTotal->saveRecord();
			delete lookedUpTotal;
		}
	}
}


void TotalsManager::handleReadHourlyTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	HourlyTotal *msgTotal = dynamic_cast<HourlyTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (msgTotal->getHour() > 0)
	{
		HourlyTotal *lookedUpTotal = HourlyTotal::findHourlyTotal(msgTotal->getHour(), dbAccess);
		if (!lookedUpTotal)
		{
			//	Set up a blank HourlyTotal
			lookedUpTotal = new HourlyTotal();
			lookedUpTotal->setHour(msgTotal->getHour());
			lookedUpTotal->setDBAccess(dbAccess);
		}
		if (lookedUpTotal)
		{
			// Send the Total found back to the Browser.
			SessionMessage totalData(TotalsDev, WriteHourlyTotal);
			totalData.setFormatType(BrowserFormat);
			totalData.setHfcObject(lookedUpTotal);

			// Send the Total to the UI (Browser)
			mySession->send(&totalData, Messenger::DEST_UI);

			// We have a new current total type.
			setCurrentHourlyTotal(lookedUpTotal);
		}
	}
}

void TotalsManager::handleReadAllHourlyTotals()
{
	if (!mySession || !dbAccess)
		return;

	HourlyTotalsAll *allHourlyTotals = new HourlyTotalsAll(dbAccess);
	if (allHourlyTotals)
	{
		// Send the ProductTotal data found back to the Browser.
		SessionMessage operTotalsData(TotalsDev, WriteAllHourlyTotals);
		operTotalsData.setFormatType(BrowserFormat);
		operTotalsData.setHfcObject(allHourlyTotals);

		// Send the TotalType to the UI (Browser)
		mySession->send(&operTotalsData, Messenger::DEST_UI);

		delete allHourlyTotals;
	}
}

void TotalsManager::handleWriteHourlyTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

void TotalsManager::handleDeleteHourlyTotal(SessionMessage *msg)
{
	bool usingCurrent = false;
	HourlyTotal *msgTotal = (HourlyTotal *) NULL;

	if (!msg || !dbAccess)
		return;

	msgTotal = dynamic_cast<HourlyTotal *> (msg->getHfcObject());
	if (!msgTotal)
	{
		if (currentHourlyTotal)
			usingCurrent = true;
		else
			return;
	}
	HourlyTotal *existingHourlyTotal = (usingCurrent ? currentHourlyTotal : HourlyTotal::findHourlyTotal(msgTotal->getHour(), dbAccess));
	if (existingHourlyTotal && existingHourlyTotal->getHour() != RECORD_NOT_FOUND)
	{
		existingHourlyTotal->deleteRecord();
		Debug::trace("TotalsManager", HobString("handleDelete - hourly total deleted - hour: ") +
														HobString(Long::toString((long)existingHourlyTotal->getHour())));
		if (usingCurrent)
			setCurrentHourlyTotal((HourlyTotal*) NULL);
		else
			delete existingHourlyTotal;
	}
	else
		Debug::trace("TotalsManager", HobString("handleDelete - hourly total to be deleted was not in the database to delete - hour: ") +
														HobString((existingHourlyTotal) ? Long::toString((long)existingHourlyTotal->getHour()) : "???"));
}

void TotalsManager::handleDeleteAllHourlyTotals()
{
	if (!dbAccess)
		return;

	DBHourlyTotalsRM *dbHourlyTotalsRM = dbAccess->getDBHourlyTotalsRM();
	if (!dbHourlyTotalsRM)
		return;

	dbHourlyTotalsRM->deleteAll();

	destroyHourlyTotalsQueryAndIter();
}

void TotalsManager::handleSaveHourlyTotal(SessionMessage *msg)
{
	if (!msg)
		return;
}

/* This method is only used when restoring operator total records into an empty
 * database after a firmware upgrade that indicates a new version of the database
 * is being installed.  There shouldn't be any operator total records in the database
 * with which to update the data being restored.  Thus, we don't need to look for an
 * existing record as is done for other datatypes.  Just go ahead and save the operator total
 * record that is received.
 */
void TotalsManager::handleRestoreHourlyTotal(SessionMessage *msg)
{
	if (!msg)
		return;

	HourlyTotal *msgTotal = dynamic_cast<HourlyTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	Debug::trace("TotalsManager", HobString("handleRestoreHourlyTotal - hour:") + HobString(Long::toString(msgTotal->getHour())));
	msgTotal->setDBAccess(dbAccess);
	msgTotal->saveRecord();
}

void TotalsManager::handleVoidHourlyTotal(SessionMessage *msg)
{
	if (!msg || !mySession)
		return;

	HourlyTotal *msgTotal = dynamic_cast<HourlyTotal *> (msg->getHfcObject());
	if (!msgTotal || !dbAccess)
		return;

	if (currentHourlyTotal)
	{
		currentHourlyTotal->voidTotal(msgTotal);
		currentHourlyTotal->saveRecord();

		// Send the Modified Total back to the Browser.
		SessionMessage totalData(TotalsDev, WriteHourlyTotal);
		totalData.setFormatType(BrowserFormat);
		totalData.setHfcObject(currentHourlyTotal);

		// Send the Total to the UI (Browser)
		mySession->send(&totalData, Messenger::DEST_UI);
	}
}


void TotalsManager::destroyQueriesAndIters()
{
	destroyProductTotalsQueryAndIter();
	destroyClassTotalsQueryAndIter();
	destroyOperatorTotalsQueryAndIter();
	destroyHourlyTotalsQueryAndIter();
}

void TotalsManager::destroyProductTotalsQueryAndIter()
{
	if (allProductTotalsIter)
	{
		delete allProductTotalsIter;
		allProductTotalsIter = (DBQuery<DBProductTotal>::const_iterator *)NULL;
	}

	if (allProductTotalsQuery)
	{
		delete allProductTotalsQuery;
		allProductTotalsQuery = (DBQuery<DBProductTotal> *)NULL;
	}
}

void TotalsManager::destroyClassTotalsQueryAndIter()
{
	if (allClassTotalsIter)
	{
		delete allClassTotalsIter;
		allClassTotalsIter = (DBQuery<DBClassTotal>::const_iterator *)NULL;
	}

	if (allClassTotalsQuery)
	{
		delete allClassTotalsQuery;
		allClassTotalsQuery = (DBQuery<DBClassTotal> *)NULL;
	}
}

void TotalsManager::destroyOperatorTotalsQueryAndIter()
{
	if (allOperatorTotalsIter)
	{
		delete allOperatorTotalsIter;
		allOperatorTotalsIter = (DBQuery<DBOperatorTotal>::const_iterator *)NULL;
	}

	if (allOperatorTotalsQuery)
	{
		delete allOperatorTotalsQuery;
		allOperatorTotalsQuery = (DBQuery<DBOperatorTotal> *)NULL;
	}
}

void TotalsManager::destroyHourlyTotalsQueryAndIter()
{
	if (allHourlyTotalsIter)
	{
		delete allHourlyTotalsIter;
		allHourlyTotalsIter = (DBQuery<DBHourlyTotal>::const_iterator *)NULL;
	}

	if (allHourlyTotalsQuery)
	{
		delete allHourlyTotalsQuery;
		allHourlyTotalsQuery = (DBQuery<DBHourlyTotal> *)NULL;
	}
}


void TotalsManager::saveToCoolTotals(RunModeTotal *currentTotal)
{
	if (!currentTotal || !Configuration::getInstance()->isCollectingCOOLTotals() || !dbAccess)
		return;

	Debug::trace("TotalsManager", "*****  saveToCoolTotals()  *****");

	/*	Since this will not be an accumulated total, there is no need to look up the dbProduct and dbTotalType
	*/
	COOLTotal *lookedUpTotal = new COOLTotal(currentTotal, dbAccess);
	if (lookedUpTotal)
	{
		lookedUpTotal->show();
		lookedUpTotal->saveRecord();
		delete lookedUpTotal;
	}
	Debug::trace("TotalsManager", "*****  END saveToCoolTotals()  *****");
}


void TotalsManager::handleDeleteAllCOOLTotals()
{
	if (!dbAccess)
		return;

	DBCoolTotalsRM	*dbCoolTotalsRM = dbAccess->getDBCoolTotalsRM();
	if (!dbCoolTotalsRM)
		return;

	dbCoolTotalsRM->deleteAll();
}

//	AWL-52: Merged in from vxWorks for Costco
void TotalsManager::setCurrentRunModeTotalType(TotalType *newTotalType)
{
	//	This is the Total type selected in Operate mode
	if (currentRunModeTotalType)
		delete currentRunModeTotalType;
	currentRunModeTotalType = newTotalType;
}

void TotalsManager::tempSwitchToThisTotalType(HobRecordNum totalTypeNum)
{
	Debug::trace("TotalsManager", "tempSwitchToThisTotalType():  totalTypeNum = " + inttostring(totalTypeNum) + "    prevTotalTypeNum = " + inttostring(prevTotalTypeNum));

	//	Do we need to make sure we are in Operate mode here?
	if (mySession && mySession->isInOperateMode() && totalTypeNum && (totalTypeNum <= TotalType::TOTALTYPE_MaxTypes))
	{
		HobRecordNum currTotalTypeNum = 0;
		if (currentRunModeTotalType)
			currTotalTypeNum = currentRunModeTotalType->getTotalTypeNum();
		if (currTotalTypeNum == 0)
		{
			Debug::trace("TotalsManager", "tempSwitchToThisTotalType():  currTotalTypeNum was 0. Set to 1.");
			currTotalTypeNum = 1;		//	Assume Production total type.
		}
		Debug::trace("TotalsManager", "tempSwitchToThisTotalType():  currTotalTypeNum = " + inttostring(currTotalTypeNum));
		if (totalTypeNum != currTotalTypeNum)
		{
			//	Load the new total type.
			TotalType *newTotalType = TotalType::findTotalType(totalTypeNum, dbAccess);
			if (newTotalType)
			{
				Debug::trace("TotalsManager", "tempSwitchToThisTotalType():  We have a new TotalType # " + inttostring(totalTypeNum));

				setCurrentRunModeTotalType(newTotalType);
				prevTotalTypeNum = currTotalTypeNum;

				//	Send the Weigh mode / Total type message back to the Browser with the new Total type.
				SessionMessage updateTotalTypeMsg(SessionDev, SnReadWeighModeTotalType);
				mySession->handleMessage(&updateTotalTypeMsg);

				// Notify the ProductManager that the Total type changed.
				SessionMessage notifyMsg(ProductDev, ProductTotalTypeChanged);
				notifyMsg.setFormatType(BrowserFormat);
				mySession->getProductManager()->handleMessage(&notifyMsg);
			}
		}
	}
}

void TotalsManager::revertBackToPrevTotalType()
{
	Debug::trace("TotalsManager", "revertBackToPrevTotalType():  prevTotalTypeNum = " + inttostring(prevTotalTypeNum));

	//	Do we need to make sure we are in Operate mode here?
	if (mySession && mySession->isInOperateMode() && prevTotalTypeNum && (prevTotalTypeNum <= TotalType::TOTALTYPE_MaxTypes))
	{
		TotalType *newTotalType = TotalType::findTotalType(prevTotalTypeNum, dbAccess);
		if (newTotalType)
		{
			Debug::trace("TotalsManager", "revertBackToPrevTotalType():  We have a new TotalType # " + inttostring(prevTotalTypeNum));

			setCurrentRunModeTotalType(newTotalType);
			prevTotalTypeNum = 0;

			//	Send the Weigh mode / Total type message back to the Browser with the new Total type.
			SessionMessage updateTotalTypeMsg(SessionDev, SnReadWeighModeTotalType);
			mySession->handleMessage(&updateTotalTypeMsg);

			// Notify the ProductManager that the Total type changed.
			SessionMessage notifyMsg(ProductDev, ProductTotalTypeChanged);
			notifyMsg.setFormatType(BrowserFormat);
			mySession->getProductManager()->handleMessage(&notifyMsg);
		}
	}
}
