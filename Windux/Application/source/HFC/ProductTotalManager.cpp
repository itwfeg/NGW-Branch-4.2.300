#include "ProductTotalManager.h"
#include "TransactionManager.h"
#include "TransactionTypeManager.h"
#include "Transaction.h"

#include "ProductTotalEvent.h"
#include "ReadFilteredProductTotalsMessage.h"
#include "WriteProductTotalsMessage.h"

#include "Session.h"
#include "DBSession.h"
#include "SessionMessage.h"
#include "Configuration.h"
#include "ProductTransaction.h"
#include "ProductManager.h"
#include "LabelTypeManager.h"
#include "ProductChangeMsg.h"
#include "ProductUpdateMsg.h"
#include "ProductTransaction.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif


namespace hfc
{

#ifdef USE_ACCOUNTANT
int ProductTotalManager::dataType = 0;
std::string ProductTotalManager::dataTypeName("hfc::ProductTotalManager"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductTotalManager::ProductTotalManager(HFCManagerContainer * container) :
	mySession_(0),
	mgrContainer_(container),
	cachedProductTotals_(),
	productIterator_(),
	totalIterator_(),
	currentProductRunTotal_(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductTotalManager::~ProductTotalManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::handleMessage(SessionMessage* msg)
{
	if ( !msg )
		return;
	else if ( msg->getDeviceType() != ProductTotalDev )
		return;

	switch ( msg->getEventType() )
	{
	case ReadFilteredProductTotals:
	{
		ReadFilteredProductTotalsMessage* message = dynamic_cast<ReadFilteredProductTotalsMessage*>(msg->getHfcObject());
		if ( !message ) return;

		handleReadFilteredProductTotals(message);

		break;
	}

	default:
		Debug::trace("ProductTotalManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::handleReadFilteredProductTotals(ReadFilteredProductTotalsMessage* inputMessage)
{
	Session* session = dynamic_cast<Session*>(mySession_);
	TransactionManager* transManager = mgrContainer_->getTransactionManager();
	if ( !inputMessage || !session || !transManager ) return;


	/* Read transactions from the database. */
	clearCachedProductTotals();
	HobRecordNum transactionType = util_string::stringtolong(inputMessage->getTransactionType());
	bool allProduction = transactionType == 0;
	transManager->initializeAllFilteredTransactionsQuery(inputMessage->getStartDateTime(), inputMessage->getEndDateTime(),
		inputMessage->getTransactionType(), Transaction::VoidNotVoided, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum), inputMessage->getPLUNumber());
	Util::CSmartPtr<Transaction> trans = transManager->getFirstFilteredTransaction();
	while ( trans.notNull() )
	{
		bool useTransaction = false;
		TransactionType::AffectsProductionFlag affectsProduction = trans->getAffectsProductionFlag();

		if ( allProduction )
		{
			if ( affectsProduction )
				useTransaction = true;
		}
		else
		{
			if ( trans->getTransactionTypeNumber() == transactionType )
				useTransaction = true;
		}

		/* Populate the product totals. */
		if ( useTransaction )
		{
			Util::CSmartPtr<ProductTotal> total = getCachedProductTotal(trans->getDeptNum(), trans->getProductNumber(), trans->getTransactionTypeNumber());

			if ( total.notNull() )
			{
				if ( allProduction && affectsProduction == TransactionType::AffectsProductionNegatively )
				{
					total->incrementWeight(-trans->getFinalNetWeight());
					total->incrementPrice(-trans->getTotalValue());
					if ( trans->getByCount() )
						total->incrementItems(-trans->getByCount());
					else
						total->incrementItems(-1);
					total->incrementPackages(-trans->getNumberPackages());
					total->incrementTransactions(-1);
				}
				else
				{
					total->incrementWeight(trans->getFinalNetWeight());
					total->incrementPrice(trans->getTotalValue());
					if ( trans->getByCount() )
						total->incrementItems(trans->getByCount());
					else
						total->incrementItems(1);
					total->incrementPackages(trans->getNumberPackages());
					total->incrementTransactions();
				}
			}
		}

		trans = transManager->getNextFilteredTransaction();
	}


	/* Send the information to the User Interface. */
	Util::CSmartPtr<WriteProductTotalsMessage> response(new WriteProductTotalsMessage());

	Util::CSmartPtr<ProductTotal> total;
	for ( total = getFirstProductTotal(); total.notNull(); total = getNextProductTotal() )
		response->addProductTotal(total);

	SessionMessage outputMessage(ProductTotalDev, WriteFilteredProductTotals);
	outputMessage.setFormatType(BrowserFormat);
	outputMessage.setHfcObject(response.get());

	mySession_->send(&outputMessage, Messenger::DEST_UI);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ProductTotalManager::initializeAllProductTotalsQuery(const std::string& deptNum)
{
	TransactionManager* transManager = mgrContainer_->getTransactionManager();
	if ( !transManager || !mySession_ ) return 0;


	clearCachedProductTotals();
	transManager->initializeNotClearedQuery(false, deptNum);

	for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
			trans = transManager->getNextReadClearTransaction(false) )
	{
		TransactionType::AffectsProductionFlag production = trans->getAffectsProductionFlag();
	
		if ( trans->getVoid() == Transaction::VoidNotVoided && production != TransactionType::AffectsProductionNone )
		{
			Util::CSmartPtr<ProductTotal> total = getCachedProductTotal(trans->getDeptNum(), trans->getProductNumber(), trans->getTransactionTypeNumber());

			if ( total.notNull() )
			{
				if ( production == TransactionType::AffectsProductionPositively )
				{
					total->incrementWeight(trans->getFinalNetWeight());
					total->incrementPrice(trans->getTotalValue());
					if ( trans->getByCount() )
						total->incrementItems(trans->getByCount());
					else
						total->incrementItems(1);
					total->incrementPackages(trans->getNumberPackages());
					total->incrementTransactions();
				}
				else
				{
					total->incrementWeight(-trans->getFinalNetWeight());
					total->incrementPrice(-trans->getTotalValue());
					if ( trans->getByCount() )
						total->incrementItems(-trans->getByCount());
					else
						total->incrementItems(-1);
					total->incrementPackages(-trans->getNumberPackages());
					total->incrementTransactions(-1);
				}

				if ( total->getReportingClassNumber() == RECORD_NOT_FOUND )
				{
					// Find the product record
					Product* product = Product::findRecord(trans->getProductNumber(), trans->getDeptNum(), mySession_);
					if ( product )
						total->setReportingClassNumber(product->getReportingClassNum());

					// Find the transaction type record
					TransactionTypeManager* transTypeMgr = mgrContainer_->getTransactionTypeManager();
					if ( transTypeMgr )
					{
						Util::CSmartPtr<TransactionType> transType = transTypeMgr->findTransactionType(trans->getTransactionTypeNumber());
						if ( transType.notNull() )
							total->setFlags(transType->getTransactionTypeFlag());
					}
				}
			}
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Util::CSmartPtr<ProductTotal> ProductTotalManager::getFirstProductTotal()
{
	for ( productIterator_ = cachedProductTotals_.begin(); productIterator_ != cachedProductTotals_.end(); ++productIterator_ )
		for ( totalIterator_ = productIterator_->second->begin(); totalIterator_ != productIterator_->second->end(); ++totalIterator_ )
			if ( totalIterator_->second->getPackages() )
				return totalIterator_->second;

	return Util::CSmartPtr<ProductTotal>(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Util::CSmartPtr<ProductTotal> ProductTotalManager::getNextProductTotal()
{
	++totalIterator_;

	while ( productIterator_ != cachedProductTotals_.end() )
	{
		while ( totalIterator_ != productIterator_->second->end() )
		{
			if ( totalIterator_->second->getPackages() )
				return totalIterator_->second;
			else
				++totalIterator_;
		}

		if ( ++productIterator_ != cachedProductTotals_.end() )
			totalIterator_ = productIterator_->second->begin();
	}

	return Util::CSmartPtr<ProductTotal>(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::clearCachedProductTotals()
{
	cachedProductTotals_.clear();

	productIterator_ = cachedProductTotals_.end();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Util::CSmartPtr<ProductTotal> ProductTotalManager::getCachedProductTotal(HobRecordNum deptNum, HobRecordNum productNumber, HobRecordNum transTypeNumber)
{
	if ( cachedProductTotals_.find(productNumber) == cachedProductTotals_.end() )
		cachedProductTotals_[productNumber] = new productTotalsByTypeContainer_type();

	Util::CSmartPtr<productTotalsByTypeContainer_type> productTotals = cachedProductTotals_[productNumber];
	if ( productTotals->find(transTypeNumber) == productTotals->end() )
		(*productTotals)[transTypeNumber] = new ProductTotal(deptNum, productNumber, transTypeNumber);

	return (*productTotals)[transTypeNumber];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Util::CSmartPtr<ProductRunTotal> ProductTotalManager::createNewProductRunTotal(Transaction *prodTrans)
{
	if (!prodTrans)
		return Util::CSmartPtr<ProductRunTotal>(0);
	else
	{
		HobDateTime htd = prodTrans->getCommittedDateTime(); 
		return Util::CSmartPtr<ProductRunTotal>(new ProductRunTotal(htd, (HobRecordNum)prodTrans->getDeptNum(), (HobRecordNum)prodTrans->getProductNumber(), (HobRecordNum)prodTrans->getTransactionTypeNumber()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::updateAndDisplayProductRunTotal(Transaction *trans, bool isVoid)
{
	// in HTi-Access run totals are sent always
	if (/*!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableRunTotals) ||*/ !trans || !mySession_)
		return;

	Session *s = dynamic_cast<Session *> (mySession_);
	// FT-1779 - removed the check for "!Configuration::getInstance()->isPrepackScale() || " in the following line
	if (!s->isInOperateMode())
		return;

	if (trans)
	{
		HobRecordNum curPluNum = (currentProductRunTotal_.notNull() ? currentProductRunTotal_->getProductNumber() : 0);
		HobRecordNum curTransTypeNum = (currentProductRunTotal_.notNull() ? currentProductRunTotal_->getTransactionTypeNum() : 0);
		if (!isVoid && (curPluNum != trans->getProductNumber() || curTransTypeNum != trans->getTransactionTypeNumber()))
			currentProductRunTotal_ = createNewProductRunTotal(trans);
		
		if (currentProductRunTotal_.notNull())
		{
			currentProductRunTotal_->update(trans, isVoid);

			//	Now, send the current totals info to the UI
			SessionMessage totalData(TotalsDev, DtWrite);
			totalData.setFormatType(BrowserFormat);
			totalData.setHfcObject(currentProductRunTotal_);
			mySession_->send(&totalData, Messenger::DEST_UI);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::clearAndDisplayProductRunTotal()
{
	// in HTi-Access run totals are sent always
	if (/*!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableRunTotals) ||*/ !mySession_)
		return;

	ProductManager *prodMgr = mgrContainer_->getProductManager();
	if (!prodMgr)
		return;

	HobRecordNum pluNum = 0;
	HobRecordNum deptNum = RECORD_NOT_FOUND;
	HobRecordNum transTypeNum = 0;
	Product *currProd = prodMgr->getCurrentProduct();
	if (currProd)
	{
		deptNum = currProd->getDeptNum();
		pluNum = currProd->getProductNum();
		ProductTransaction *pTrans = currProd->getCurrentTransaction();
		if (pTrans)
			transTypeNum = pTrans->getTransactionTypeNumber();
	}

	HobDateTime now;
//	if (currentProductRunTotal_)
//		delete currentProductRunTotal_;

	currentProductRunTotal_ = new ProductRunTotal(now, deptNum, pluNum, transTypeNum);

	// Send cleared ProductRunTotal data to the browser.
	SessionMessage totalData(TotalsDev, DtWrite);
	totalData.setFormatType(BrowserFormat);
	totalData.setHfcObject(currentProductRunTotal_);
	mySession_->send(&totalData, Messenger::DEST_UI);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductTotalManager::customRunTotalsLabelExists() const
{
	Debug::trace( "ProductTotalManager", "Checking for custom run totals label" );

	ProductManager *pm = mgrContainer_->getProductManager();
	if ( !pm )
		return false;

	Product *currentProduct = pm->getCurrentProduct();
	if (( !currentProduct ) || ( currentProduct->getLabelBeingPrinted() != Product::PRINTING_NO_LABEL ))
		return false;

	if ( !mySession_ )
		return false;

	Session *s = dynamic_cast<Session*>( mySession_ );
	if ( !s )
		return false;

	LabelType *runTotalsLabelType = LabelType::findRecord( Configuration::getInstance()->toIntCfgI(CfgInternal::RUNTOTALSLABELFAMILY), currentProduct->getDeptNum(), mySession_ );

	if ( !runTotalsLabelType )
		return false;

	Debug::trace( "ProductTotalManager", "Custom run totals label EXISTS" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductTotalManager::handleProductPrintCustomRunTotalsLabel()
{
	const bool isTracing( Debug::isTracing( "ProductTotalManager" ) );

	Debug::trace( "ProductTotalManager", "handleProductPrintCustomRunTotalsLabel - begin" );

	ProductManager *pm = mgrContainer_->getProductManager();
	if ( !pm )
		return;

	Product *currentProduct = pm->getCurrentProduct();
	if (( !currentProduct ) || ( currentProduct->getLabelBeingPrinted() != Product::PRINTING_NO_LABEL ))
		return;

	if ( !mySession_ )
		return;

	Session *s = dynamic_cast<Session*>( mySession_ );
	if ( !s )
		return;
/*
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
*/
	if ( isTracing )
		Debug::trace("ProductTotalManager", "handleProductPrintCustomRunTotalsLabel - checking to see if we have a proper label type");

	//The following code to validate the label type originally came from ProductManager::labelTypeErrorTest();
	const int labelFamily = Configuration::getInstance()->toIntCfgI( CfgInternal::RUNTOTALSLABELFAMILY );
	const bool simulatePrinting = Configuration::getInstance()->toBoolCfgI( CfgInternal::SIMULATEPRINTING );

	bool haveLabelType = true;
	LabelType *runTotalsLabelType = LabelType::findRecord( labelFamily, currentProduct->getDeptNum(), mySession_ );

	if ( !runTotalsLabelType )
		haveLabelType = false;
	else
	{
		LabelType *correctedLabelType;
		LabelMediaReturn result = pm->labelTypeErrorTest( runTotalsLabelType, &correctedLabelType, FirstPrinter, currentProduct->getDeptNum() );

		if (( result == LabelOk ) ||
			( result == LabelCorrected ) ||
			( simulatePrinting ))
		{
			if ( result == LabelCorrected )
			{
				if ( isTracing )
					Debug::trace( "ProductTotalManager",
						HobString( "handleProductPrintCustomRunTotalsLabel - Label type - Found a corrected label in the family to fit stock: PrinterNum = " ) + HobString( Integer::toString( FirstPrinter ) ) +
						HobString("    OrigNum = ") + HobString( Long::toString( runTotalsLabelType->getLabelTypeNum() ) ) +
						HobString("    NewNum = ") + HobString( Long::toString( correctedLabelType->getLabelTypeNum() ) ) );

				delete runTotalsLabelType;

				runTotalsLabelType = correctedLabelType;
			}

			if (isTracing)
				Debug::trace( "ProductTotalManager",
					HobString( "handleProductPrintCustomRunTotalsLabel - setting run totals label type - " ) + HobString( Long::toString( runTotalsLabelType->getLabelTypeNum() ) ) );

			currentProduct->setRunTotalsLabel( runTotalsLabelType );
		}
		else
		{
			haveLabelType = false;
		}
	}

	if ( !haveLabelType )
	{
		if ( isTracing )
			Debug::trace( "ProductTotalManager", HobString( "handleProductPrintCustomRunTotalsLabel - Label type not OK or could not be corrected" ) );

		mySession_->addDialogMessage( ErrProductDev, ProductErrCannotPrintExpLabelInProdEntry, HfcResponse::RESPONSESTYLE_OK );
		return;
	}

	// ---- //

	if ( isTracing )
		Debug::trace( "ProductTotalManager", HobString( "handleProductPrintCustomRunTotalsLabel - about to print run totals label type" ) );

	currentProduct->setLabelBeingPrinted( Product::PRINTING_RUN_TOTALS_LABEL );

	// Sending the ProductChange message will cause the PrinterManager to re-render the label, and when it does,
	// the label rendered will be the run totals label; as a result of the call to setLabelBeingPrinted() above.
	if ( isTracing )
		Debug::trace( "ProductTotalManager", "handleProductPrintCustomRunTotalsLabel - sending ProductChange to RTC" );

	ProductChangeMsg printExpProductChangeMsg( mySession_, currentProduct );
	mySession_->send( &printExpProductChangeMsg, Messenger::DEST_RTC );

	// Send a ProductChangeSmart message to the RTC so it knows it is dealing with a new plu.
	ProductChangeMsg printExpProductChangeMsg2( mySession_, pm );
	mySession_->send( &printExpProductChangeMsg2, Messenger::DEST_RTC );

	// Now we need to send a ProductUpdate message to the RTC to indicate a label should be printed.
	if ( isTracing )
		Debug::trace( "ProductTotalManager", "handleProductPrintCustomRunTotalsLabel - sending ProductUpdate to RTC" );

	ProductUpdateMsg printExpProductUpdateMsg( mySession_, currentProduct->printCurrentTransaction() );
	mySession_->send( &printExpProductUpdateMsg, Messenger::DEST_RTC );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
