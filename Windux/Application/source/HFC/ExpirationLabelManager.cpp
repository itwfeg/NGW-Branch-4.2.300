/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ExpirationLabelManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "AllExpirationLabels.h"
#include "Configuration.h"
#include "ProductManager.h"
#include "LabelTypeManager.h"
#include "ProductChangeMsg.h"
#include "ProductUpdateMsg.h"
#include "ProductTransaction.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"

#include "MessageLogger.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ExpirationLabelManager::_dataType_ = 0;
std::string ExpirationLabelManager::_dataTypeName_("ExpirationLabelManager"); 
#endif


ExpirationLabelManager::ExpirationLabelManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

ExpirationLabelManager::ExpirationLabelManager(hfc::HFCManagerContainer *container)
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

ExpirationLabelManager::~ExpirationLabelManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (currentExpirationLabel)
		delete currentExpirationLabel;

	destroyQueriesAndIters();
}


void ExpirationLabelManager::init()
{
	mySession = 0;

	currentExpirationLabel = 0;

	allExpirationLabelsQuery = 0;
	allExpirationLabelsIter  = 0;
}


DBSession *ExpirationLabelManager::getMySession()
{
	return mySession;
}
void ExpirationLabelManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
ExpirationLabel *ExpirationLabelManager::firstExpirationLabel()
{
	if (!dbAccess)
		return 0;

	edb::DBExpirationLabelsRM *dbExpirationLabelsRM = dbAccess->getDBExpirationLabelsRM();
	if (!dbExpirationLabelsRM)
		return 0;

	// If the user calls firstExpirationLabel() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	// Do not use retrieveAll(deptNum) because this method is used by BackupRestoreManager and we want to backup all expiration label records, not just for a dept
	allExpirationLabelsQuery = dbExpirationLabelsRM->retrieveAll();

	// Couldn't get the query from which to access the ExpirationLabels, just return false.
	if (!allExpirationLabelsQuery)
		return 0;

	edb::DBQuery<edb::DBExpirationLabel>::const_iterator tempIter = allExpirationLabelsQuery->begin();
	if (tempIter == allExpirationLabelsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextExpirationLabel()
	 * method call below).
	 */
	allExpirationLabelsIter = new edb::DBQuery<edb::DBExpirationLabel>::const_iterator(tempIter);
	edb::DBExpirationLabel *el = new edb::DBExpirationLabel(*(*allExpirationLabelsIter));
	ExpirationLabel *expLabel = new ExpirationLabel(el, dbAccess);
	return expLabel;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstExpirationLabel() method, the first call to this method will
 * result in the first record being returned.
 */
ExpirationLabel *ExpirationLabelManager::nextExpirationLabel()
{
	if (!dbAccess)
		return 0;

	if (allExpirationLabelsQuery && !allExpirationLabelsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allExpirationLabelsQuery || !allExpirationLabelsIter)
		return firstExpirationLabel();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allExpirationLabelsIter);
	if (*allExpirationLabelsIter == allExpirationLabelsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all ExpirationLabels in the db. */
		destroyQueriesAndIters();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBExpirationLabel copy constructor in order for the Hfc ExpirationLabel to
	 * control the destruction of the DBExpirationLabel record that the iterator contains; otherwise, the DBExpirationLabel will
	 * be deleted when the iterator goes out of scope.  The DBExpirationLabel copy will be deleted by the ExpirationLabel record
	 * created below.
	 */
	edb::DBExpirationLabel *el = new edb::DBExpirationLabel(*(*allExpirationLabelsIter));
	ExpirationLabel *expLabel = new ExpirationLabel(el, dbAccess);
	return expLabel;
}


/* Provide a countAll method so the caller doesn't need to know about a dbExpirationLabelsRM.
 */
int ExpirationLabelManager::countAllExpirationLabels()
{
	if (!dbAccess)
		return -1;

	edb::DBExpirationLabelsRM *dbExpirationLabelsRM = dbAccess->getDBExpirationLabelsRM();
	if (!dbExpirationLabelsRM)
		return -1;

	return dbExpirationLabelsRM->countAll();
}


ExpirationLabel *ExpirationLabelManager::getCurrentExpirationLabel()
{
	return currentExpirationLabel;
}
void ExpirationLabelManager::setCurrentExpirationLabel(ExpirationLabel *newEL)
{
	if (currentExpirationLabel)
		delete currentExpirationLabel;
	currentExpirationLabel = newEL;

	if (Debug::isTracing("ExpirationLabelManager"))
	{
		Debug::trace("ExpirationLabelManager", "setCurrentExpirationLabel():  currentExpirationLabel = " +
				util_string::ptrtostring(currentExpirationLabel));
	}
}


void ExpirationLabelManager::handleMessage(SessionMessage *msg)
{
	// Valid as long as no process calls this method with a message containing
	// some other device than ExpirationLabel.
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	Debug::trace("ExpirationLabelManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case ProductPrintNewExpirationLabel:
		case ProductPrintExistingExpirationLabel:
		{
			handleProductPrintExpirationLabel(event);
			break;
		}

		case DtRead:
		{
			ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			handleRead(msgExpirationLabel);
			break;
		}

		case DtReadAll:
		case ProductReadAllExpirationLabels:
			handleReadAll();
			break;

		case DtWrite:
		{
			ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			handleWrite(msgExpirationLabel);
			break;
		}

		case DtDelete:
		{
			ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			handleDelete(msgExpirationLabel);
			break;
		}

		case DtDeleteAll:
		case ProductDeleteAllExpirationLabels:
			handleDeleteAll();
			break;

		case DtSave:
		{
			ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			handleSave(msgExpirationLabel);
			break;
		}

		case DtRestore:
		case ProductRestoreExpirationLabel:
		{
			Debug::trace("ProductManager", "handling ProductDev | ProductRestoreExpirationLabel");
			ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			if (msgExpirationLabel)
				handleRestore(msgExpirationLabel);
			break;
		}

		case DtCreateNew:
			//ExpirationLabel *msgExpirationLabel = dynamic_cast<ExpirationLabel *> (msg->getHfcObject());
			//handleCreate(msgExpirationLabel);
			break;

		default:
			Debug::trace("ExpirationLabelManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}



void ExpirationLabelManager::handleProductPrintExpirationLabel(UINT8 newOrExisting)
{
	bool isTracing = Debug::isTracing("PrintExpirationLabel");

	ProductManager *pm = mgrContainer_->getProductManager();
	if (!pm)
		return;

	Product *currentProduct = pm->getCurrentProduct();
	if (!currentProduct || currentProduct->getLabelBeingPrinted() != Product::PRINTING_NO_LABEL)
		return;

	if (!mySession)
		return;
	Session *s = dynamic_cast<Session *> (mySession);
	if (!s)
		return;

	if (s->getCurrentMode() == OpModeProductionEntry || s->isInAnAccumulatingMode())
	{
		// Can't print expiration labels in Production Entry mode.  Just display a message to the user of this.
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintExpLabelInProdEntry, HfcResponse::RESPONSESTYLE_OK);
		return;
	}
	else if (s->getCurrentMode() == OpModePrintMultipleLabels)
	{
		// Can't print expiration labels in Print Multiple Labels mode.  Just display a message to the user of this.
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintExpLabelInPrintMultiple, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (isTracing)
		Debug::trace("PrintExpirationLabel", "handleProductPrintExpirationLabel - checking to see if we have a proper expiration label type");

	//The following code to validate the label type originally came from ProductManager::labelTypeErrorTest();
	bool haveLabelType = true;
	LabelType *shelfLifeExpirationLabelType = LabelType::findRecord(Configuration::getInstance()->toIntCfgI(CfgInternal::SHELFLIFEEXPLABELFAMILY), currentProduct->getDeptNum(), mySession);
	if (!shelfLifeExpirationLabelType)
		haveLabelType = false;
	else
	{
		LabelType *correctedLabelType;
		LabelMediaReturn result = pm->labelTypeErrorTest(shelfLifeExpirationLabelType, &correctedLabelType, FirstPrinter, currentProduct->getDeptNum());
		if (result == LabelOk || result == LabelCorrected || Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
		{
			if (result == LabelCorrected)
			{
				if (isTracing)
					Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - Label type - Found a corrected label in the family to fit stock: PrinterNum = ") + HobString(Integer::toString(FirstPrinter)) + HobString("    OrigNum = ") + HobString(Long::toString(shelfLifeExpirationLabelType->getLabelTypeNum())) + HobString("    NewNum = ") + HobString(Long::toString(correctedLabelType->getLabelTypeNum())));
				delete shelfLifeExpirationLabelType;
				shelfLifeExpirationLabelType = correctedLabelType;
			}

			if (isTracing)
				Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - setting expiration label type - ") + HobString(Long::toString(shelfLifeExpirationLabelType->getLabelTypeNum())));
			currentProduct->setExpirationLabel(shelfLifeExpirationLabelType);
		}
		else
			haveLabelType = false;
	}

	if (!haveLabelType)
	{
		if (isTracing)
			Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - Label type not OK or could not be corrected"));
		mySession->addDialogMessage(ErrProductDev, ProductErrCannotPrintExpLabelInProdEntry, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (isTracing)
		Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - have an expiration label type, event:") + HobString(Integer::toString(newOrExisting)));

	if (newOrExisting == ProductPrintExistingExpirationLabel)
	{
		if (dbAccess)
		{
			// Look for an existing expiration label record for this plu.
			// If it doesn't exist, can't finish printing for the existing expiration label record, since it doesn't exist.
			ExpirationLabel *myExpirationLabel = ExpirationLabel::findRecord(currentProduct->getProductNum(), currentProduct->getDeptNum(), dbAccess);
			if (!myExpirationLabel)
			{
				// Send a message to the browser indicating that there is no existing record for which to print an expiration label.
				if (isTracing)
					Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - There is no shelf life expiration record for which to print an existing label, ask if want to add new"));
				mySession->addDialogMessage(ErrProductDev, ProductErrNoExistingExpirationRecordForPrintExisting, HfcResponse::RESPONSESTYLE_OKCANCEL);
				return;
			}
			else
			{
				// Check if the current record is expired.  If it is, ask the user if they still want to print an expiration label for the expired product.
				HobDateTime now;
				if (myExpirationLabel->getExpirationDateTime() < now)
				{
					// Send a message to the browser indicating that the existing record is expired. Ask operator if they still want to print it.
					if (isTracing)
						Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - There is no shelf life expiration record for which to print an existing label, ask if want to add new"));
					mySession->addDialogMessage(ErrProductDev, ProductErrExistingExpirationRecordIsExpired, HfcResponse::RESPONSESTYLE_OKCANCEL);
					return;
				}

				delete myExpirationLabel;

				handleProductPrintExistingExpirationLabel(currentProduct);
			}
		}
	}
	else  // ProductPrintNewExpirationLabel
	{
		if (dbAccess)
		{
			// Look for an existing expiration label record for this plu.
			// If it exists and it's not expired, ask the user if they want to overwrite the previous one with this new one
			ExpirationLabel *myExpirationLabel = ExpirationLabel::findRecord(currentProduct->getProductNum(), currentProduct->getDeptNum(), dbAccess);
			if (myExpirationLabel)
			{
				HobDateTime now;
				HobDateTime expires = myExpirationLabel->getExpirationDateTime();
				delete myExpirationLabel;

				if (now < expires)
				{
					// Send a message to the browser indicating that there is an existing, non-expired record. Ask operator if they want to overwrite it with a new one.
					if (isTracing)
						Debug::trace("PrintExpirationLabel", HobString("handleProductPrintExpirationLabel - Trying to print a new expiration label for which one already exists, ask if they want to overwrite it with a new one"));
					mySession->addDialogMessage(ErrProductDev, ProductErrNewExpirationRecordAlreadyExists, HfcResponse::RESPONSESTYLE_OKCANCEL);
					return;
				}
			}

			handlePromptForExpirationDays();
		}
	}
}

void ExpirationLabelManager::handlePromptForExpirationDays()
{
#if defined(LINUX_FIX)
	// isTracing: unused variable
#else
	bool isTracing = Debug::isTracing("PrintExpirationLabel");
#endif

	ProductManager *pm = mgrContainer_->getProductManager();
	if (!pm)
		return;

	Product *currentProduct = pm->getCurrentProduct();
	if (!currentProduct)
		return;

	if (!mySession)
		return;
	
	// SOF-5076 save these off so we can restore if user cancels
	oldProductExpirationDays = currentProduct->getExpirationDays();
	oldProductExpirationDaysForced = currentProduct->isExpirationDaysForced();

	// Send a ProductWrite message (101,7) to the browser with the number of expiration days forced.
	// When the operator enters the number of days, the application will be notified via a WriteSingleField message.
	// We will then initiate the printing within handling that message.
	currentProduct->setExpirationDaysForced(true);

	SessionMessage pluData(ProductDev, ProductWrite);
	currentProduct->setFormatType(BrowserFormatPLUOperateMode);
	pluData.setFormatType(BrowserFormat);
	pluData.setHfcObject(currentProduct);
	mySession->send(&pluData, Messenger::DEST_UI);
}

void ExpirationLabelManager::handleProductPrintNewExpirationLabel(Product *currentProduct)
{
	bool isTracing = Debug::isTracing("PrintExpirationLabel");

	if (isTracing)
		Debug::trace("PrintExpirationLabel", "begin handleProductPrintNewExpirationLabel");

	// SOF-5076
	// if current product expiration days is -1, the user cancelled out of the dialog
	// so reset the days and forced field status for this prod
	if (currentProduct->getExpirationDays() == -1)
	{
		currentProduct->setExpirationDays(oldProductExpirationDays);
		currentProduct->setExpirationDaysForced(oldProductExpirationDaysForced);
	  
	  	SessionMessage pluData(ProductDev, ProductWrite);
		currentProduct->setFormatType(BrowserFormatPLUOperateMode);
		pluData.setFormatType(BrowserFormat);
		pluData.setHfcObject(currentProduct);
		mySession->send(&pluData, Messenger::DEST_UI);
	  
		return;
	}
	
	currentProduct->setLabelBeingPrinted(Product::PRINTING_NEW_EXPIRATION_LABEL);

	// Sending the ProductChange message will cause the PrinterManager to re-render the label, and when it does,
	// the label rendered will be the shelf life expiration label; as a result of the call to setLabelBeingPrinted() above.
	if (isTracing)
		Debug::trace("PrintExpirationLabel", "handleProductPrintNewExpirationLabel - sending ProductChange to RTC");
	ProductChangeMsg printExpProductChangeMsg(mySession, currentProduct);
	mySession->send(&printExpProductChangeMsg, Messenger::DEST_RTC);

	// Send a ProductChangeSmart message to the RTC so it knows it is dealing with a new plu.
	ProductManager *pm = mgrContainer_->getProductManager();
	if (pm)
	{
		ProductChangeMsg printExpProductChangeMsg2(mySession, pm);
		mySession->send(&printExpProductChangeMsg2, Messenger::DEST_RTC);
	}

	// Now we need to send a ProductUpdate message to the RTC to indicate a label should be printed.
	ProductTransaction *transaction = currentProduct->printCurrentTransaction();
	if (transaction)
	{
		HobDateTime expDateTime;
		expDateTime.incrementDays(currentProduct->getExpirationDays());
		transaction->setExpirationLabelDateTime(expDateTime);
	}

	if (isTracing)
		Debug::trace("PrintExpirationLabel", "handleProductPrintNewExpirationLabel - sending ProductUpdate to RTC");
	ProductUpdateMsg printExpProductUpdateMsg(mySession, transaction);
	mySession->send(&printExpProductUpdateMsg, Messenger::DEST_RTC);

// The following is needed when testing on a PC, when no label will be printed.
//	HfcProductCompleteMsg productComplete(mySession, transaction);
//	productComplete.setPrintOk();
//	handleProductComplete(&productComplete);
}

void ExpirationLabelManager::handleProductPrintExistingExpirationLabel(Product *currentProduct)
{
	if (!currentProduct)
		return;

	bool isTracing = Debug::isTracing("PrintExpirationLabel");

	if (isTracing)
		Debug::trace("PrintExpirationLabel", "begin handleProductPrintExistingExpirationLabel");

	currentProduct->setLabelBeingPrinted(Product::PRINTING_EXISTING_EXPIRATION_LABEL);

	// Sending the ProductChange message will cause the PrinterManager to re-render the label, and when it does,
	// the label rendered will be the shelf life expiration label; as a result of the call to setLabelBeingPrinted() above.
	if (isTracing)
		Debug::trace("PrintExpirationLabel", "handleProductPrintExistingExpirationLabel - sending ProductChange to RTC");
	ProductChangeMsg printExpProductChangeMsg(mySession, currentProduct);
	mySession->send(&printExpProductChangeMsg, Messenger::DEST_RTC);

	// Send a ProductChangeSmart message to the RTC so it knows it is dealing with a new plu.
	ProductManager *pm = mgrContainer_->getProductManager();
	if (pm)
	{
		ProductChangeMsg printExpProductChangeMsg2(mySession, pm);
		mySession->send(&printExpProductChangeMsg2, Messenger::DEST_RTC);
	}

	// Now we need to send a ProductUpdate message to the RTC to indicate a label should be printed.
	ProductTransaction *transaction = currentProduct->printCurrentTransaction();
	if (transaction)
	{
		// This better exist, cause we just found it earlier.
		ExpirationLabel *myExpirationLabel = ExpirationLabel::findRecord(currentProduct->getProductNum(), currentProduct->getDeptNum(), dbAccess);
		if (myExpirationLabel)
		{
			transaction->setExpirationLabelDateTime(myExpirationLabel->getExpirationDateTime());
			delete myExpirationLabel;

			if (isTracing)
				Debug::trace("PrintExpirationLabel", "handleProductPrintExistingExpirationLabel - sending ProductUpdate to RTC");
			ProductUpdateMsg printExpProductUpdateMsg(mySession, transaction);
			mySession->send(&printExpProductUpdateMsg, Messenger::DEST_RTC);

			// The following is needed when testing on a PC, when no label will be printed.
			//	HfcProductCompleteMsg productComplete(mySession, transaction);
			//	productComplete.setPrintOk();
			//	handleProductComplete(&productComplete);
		}
		else
			currentProduct->setLabelBeingPrinted(Product::PRINTING_NO_LABEL);
	}
	else
		currentProduct->setLabelBeingPrinted(Product::PRINTING_NO_LABEL);
}


// This method updates, or creates if it doesn't exist, the Shelf Life Expiration Label record for the product whose label was just printed.
// Currently, we are only keeping one expirationLabel record for each product.
void ExpirationLabelManager::handleProductCompleteMessage(short whichExpirationLabel, HobRecordNum productNum, HobRecordNum deptNum, const HobString &productDesc, const HobDateTime &shelfLifeDateTime)
{
	Debug::trace("ExpirationLabelManager", "handleProductCompleteMessage - Printed Shelf Life Expiration Label successfully");
	// Save the fact that the user printed a shelf life expiration label into the elabels db file.

	if (whichExpirationLabel == Product::PRINTING_NEW_EXPIRATION_LABEL)
	{
		if (dbAccess)
		{
			// Look for an existing expiration label record for this plu.
			// If it doesn't exist, create one.
			ExpirationLabel *myExpirationLabel = ExpirationLabel::findRecord(productNum, deptNum, dbAccess);
			if (!myExpirationLabel)
				myExpirationLabel = new ExpirationLabel(dbAccess);

			// Update the printed date and product description.
			if (myExpirationLabel)
			{
				myExpirationLabel->setDeptNum(deptNum);
				myExpirationLabel->setPluNumber(productNum);
				myExpirationLabel->setPluDescription(productDesc);
				myExpirationLabel->setExpirationDateTime(shelfLifeDateTime);

				handleSave(myExpirationLabel);

				Debug::trace("ExpirationLabelManager", "handleProductCompleteMessage - saved Shelf Life Expiration Label record");
				delete myExpirationLabel;
			}
		}
	}
}


void ExpirationLabelManager::handleRead(ExpirationLabel *el)
{
	if (!el || !dbAccess)
		return;
/*
	ExpirationLabel *lookedUpExpirationLabel = ExpirationLabel::find(op->getOpNum(), dbAccess);

	if (!lookedUpExpirationLabel || lookedUpExpirationLabel->getErrorCode() != RECORD_OK)
	{
		if (mySession)
				mySession->addDialogMessage(ErrOperatorDev, OperatorErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
		if (lookedUpExpirationLabel)
			delete lookedUpExpirationLabel;
		return;
	}
	// We have a new current ExpirationLabel.
	setCurrentOperator(lookedUpExpirationLabel);

	if (mySession)
	{
		SessionMessage writeOperMsg(OperatorDev, DtWrite);
		writeOperMsg.setFormatType(BrowserFormat);
		currentOperator->setFormatType(OperatorMaintenanceFormat);
		writeOperMsg.setHfcObject(currentOperator);

		mySession->send(&writeOperMsg, Messenger::DEST_UI);
	}
*/
}


void ExpirationLabelManager::handleReadAll()
{
	if (!mySession || !dbAccess)
		return;

	// Read the expiration labels records for display on the Expiration Label History report in Operate Mode (accessed via the Settings Menu entry).
	AllExpirationLabels allExpirationLabels;
	allExpirationLabels.findAll(dbAccess);

	SessionMessage allExpLabelsMsg(ProductDev, ProductWriteAllExpirationLabels);
	allExpLabelsMsg.setFormatType(BrowserFormat);
	allExpLabelsMsg.setHfcObject(&allExpirationLabels);
	mySession->send(&allExpLabelsMsg, Messenger::DEST_UI);
}


void ExpirationLabelManager::handleWrite(ExpirationLabel *el)
{
	if (!el || !currentExpirationLabel)
		return;

  	currentExpirationLabel->updateRecord(el);
}


void ExpirationLabelManager::handleDelete(ExpirationLabel *el)
{
	if (!el || !dbAccess)
		return;
/*
	HobString opNumToDelete = op->getOpNum();

	HobOperator *existingOp = HobOperator::find(op->getOpNum(), op->getDeptNum(), dbAccess);
	if (existingOp && existingOp->getOpNum() == opNumToDelete)
	{
		existingOp->deleteRecord();

		if (currentOperator && existingOp->getOpNum() == currentOperator->getOpNum())
		{
			setCurrentOperator((HobOperator*) NULL);

			// todo - is this sufficient to delete the current operator?  Should the UI be notified that the current
			// operator no longer exists - thus they shouldn't be allowed to run any more packages?
		}
		delete existingOp;
		Debug::trace("OperatorManager", HobString("handleDelete - operator deleted - opnum:") + opNumToDelete);
	}
	else
		Debug::trace("OperatorManager", HobString("handleDelete - operator to be deleted was not in the database to delete - opnum:") + opNumToDelete);
*/
}


void ExpirationLabelManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void ExpirationLabelManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("ExpirationLabelManager", "handleDeleteAll - deleting all ExpirationLabels");

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ExpirationLabels))
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

		DBExpirationLabelsRM *dbRM = dbAccess->getDBExpirationLabelsRM();
		if (!dbRM)
			return;

		setCurrentExpirationLabel(0);

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));

		destroyQueriesAndIters();
	}

	Debug::trace("ExpirationLabelManager", "handleDeleteAll - leaving");
}

// Not using the "current" Expiration Label to save.  Just save the one passed in.
void ExpirationLabelManager::handleSave(ExpirationLabel *el)
{
	if (el)
	{
		el->saveRecord();
	
		Debug::trace("ExpirationLabelManager", "handleSave - getScaleSynchronizationType " + inttostring((int)Configuration::getInstance()->getScaleSynchronizationType()) );
		Debug::trace("ExpirationLabelManager", "handleSave - isSynchronized(ExpirationLabels) " + booltostring(mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ExpirationLabels)));

		if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ExpirationLabels))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, el);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}
	}
}


void ExpirationLabelManager::handleRestore(ExpirationLabel *restoreEl)
{
	if (!restoreEl || !dbAccess)
		return;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ExpirationLabels))
	{
		// batch the record being restored; don't actually write it to the database right now.
		std::string synchSer = "";
		serializeForSynchronization(synchSer, restoreEl);

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		restoreEl->setDBAccess(dbAccess);
		Debug::trace("ProductManager", HobString("handleRestoreExpirationLabel - PluNumber:") + HobString(Long::toString(restoreEl->getPluNumber())));

		ExpirationLabel *existingEl = ExpirationLabel::findRecord(restoreEl->getPluNumber(), restoreEl->getDeptNum(), dbAccess);
		if (existingEl)
		{
			if (existingEl->isRecordValid())
			{
				existingEl->updateRecord(restoreEl);
				existingEl->saveRecord();
			}
			else
			{
				existingEl->deleteRecord();
				restoreEl->saveRecord();
			}

			delete existingEl;
		}
		else
			restoreEl->saveRecord();
	}
}

/*
void ExpirationLabelManager::handleCreate(ExpirationLabel *el)
{
	if (!el || !dbAccess)
		return;

	ExpirationLabel *newExpirationLabel = new ExpirationLabel();
	if (newExpirationLabel)
	{
		newExpirationLabel->setOpNum(el->getxxx());
		newExpirationLabel->setErrorCode(RECORD_OK);
		newExpirationLabel->setDBAccess(dbAccess);

		//	Set this new ExpirationLabel as the current one.
		setCurrentExpirationLabel(newExpirationLabel);

		if (mySession)
		{
			// Send the new operator to the Browser.
			SessionMessage elData(OperatorDev, DtWrite);
			opData.setFormatType(BrowserFormat);
			currentOperator->setFormatType(OperatorMaintenanceFormat);
			opData.setHfcObject(currentOperator);
			mySession->send(&opData, Messenger::DEST_UI);
		}
	}
}
*/

void ExpirationLabelManager::destroyQueriesAndIters()
{
	if (allExpirationLabelsIter)
	{
		delete allExpirationLabelsIter;
		allExpirationLabelsIter = (DBQuery<DBExpirationLabel>::const_iterator *)NULL;
	}

	if (allExpirationLabelsQuery)
	{
		delete allExpirationLabelsQuery;
		allExpirationLabelsQuery = (DBQuery<DBExpirationLabel> *)NULL;
	}
}


void ExpirationLabelManager::purgeOldExpirationLabels()
{
    // Check for shelf life expiration labels that have been expired for longer than the configured time (in days) to keep them around.

	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Time to purge old Expiration labels.");

	if (!dbAccess)
		return;

	DBExpirationLabelsRM *dbExpirationLabelsRM = dbAccess->getDBExpirationLabelsRM();
	if (!dbExpirationLabelsRM)
		return;

	bool isTracing = Debug::isTracing("PurgeOldRecords");
	util_time::Time t0 = util_time::Time();
	if (isTracing)
		Debug::trace("PurgeOldRecords", t0.operator std::string() + " - begin of purgeOldExpirationLabels()");

	// Do not use retrieveAll(deptNum) because we want to purge old records for any/all depts
	DBQuery<DBExpirationLabel>* allExpirationLabelsQuery = dbExpirationLabelsRM->retrieveAll();
	if (allExpirationLabelsQuery)
	{
		DBExpirationLabel *dbExpirationLabel = 0;

		HobString timeString;
		HobDateTime nowDateTime;
		HobDateTime expDateTime;
		long expiredLimit = Configuration::getInstance()->toIntCfgI(CfgInternal::EXPIRATIONLABELEXPIREDLIMIT) * HobDateTime::SecondsPerDay;
		bool loop = true;
		while (loop)
		{
			// assume we will break out, will reset this if we are supposed to keep going (we delete an expiration label record).
			loop = false;

			DBQuery<DBExpirationLabel>::const_iterator tempIter = allExpirationLabelsQuery->begin();
			if (tempIter != allExpirationLabelsQuery->end())
			{
				dbExpirationLabel = new DBExpirationLabel(*tempIter);
				if (dbExpirationLabel)
				{
					if (dbExpirationLabel->isExpirationNull())
					{
						if (isTracing)
							Debug::trace("PurgeOldRecords", "expiration time is null, deleting expiration label record  plu#:" + dbExpirationLabel->getPLU());
						dbExpirationLabelsRM->deleteRecord(dbExpirationLabel);   //delete it;
						loop = true;
					}
					else
					{
						expDateTime.convertFromDBFormat(dbExpirationLabel->getExpiration());

						if (nowDateTime >= expDateTime && nowDateTime.timeDiff(expDateTime) >= expiredLimit)
						{
							if (isTracing)
								Debug::trace("PurgeOldRecords", "expiration time is past limit, deleting expiration label record  plu#:" + dbExpirationLabel->getPLU() + "  expiration:" + dbExpirationLabel->getExpiration());
							dbExpirationLabelsRM->deleteRecord(dbExpirationLabel);   //delete it;
							loop = true;
						}
					}

					delete dbExpirationLabel;
					dbExpirationLabel = 0;
				}
			}
		}

		delete allExpirationLabelsQuery;
		allExpirationLabelsQuery = 0;
	}

	if (isTracing)
	{
		util_time::Time t1 = util_time::Time();
		Debug::trace("PurgeOldRecords", " - end of purgeOldExpirationLabels() - elapsed time: " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
	}
	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Finished purging old Expiration labels.");
}
