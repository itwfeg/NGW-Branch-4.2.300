/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CutTestTotalManager.h"

#include "TransactionManager.h"
#include "Transaction.h"

#include "Session.h"
#include "SessionMessage.h"
#include "Configuration.h"

#include "HfcError.h"
#include "ProductManager.h"
#include "CutTestManager.h"
#include "CuttingTest.h"
#include "CutTestTotal.h"
#include "CuttingTestReport.h"
#include "messages/CutTestTotalEvent.h"

#include "DBAccess.h"
#include "DBException.h"
#include "CSmartPtr.h"

#include "DBTransactionsRM.h"
#include "DBTransaction.h"

#include "Long.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"

using namespace hfc;


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CutTestTotalManager::_dataType_ = 0;
	std::string CutTestTotalManager::_dataTypeName_("hfc::CutTestTotalManager"); 
#endif


CutTestTotalManager::CutTestTotalManager(hfc::HFCManagerContainer *container)
			: mySession_(0), mgrContainer_(container), cachedCutTestTotals_(), cutTestIterator_(), totalIterator_()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

CutTestTotalManager::~CutTestTotalManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void CutTestTotalManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	int event = msg->getEventType();
	if (Debug::isTracing("CutTestTotalManager"))
	{
		HobString msgDebug("handleMessage():  HfcObject = ");
		msgDebug.append(Hex::toString((long long) msg->getHfcObject()));
		msgDebug.append("   device = ").append(Integer::toString(msg->getDeviceType()));
		msgDebug.append("   event = ").append(Integer::toString(event));
		Debug::trace("CutTestTotalManager", msgDebug);
	}

	if ( msg->getDeviceType() == CutTestTotalDev )
	{
		switch(event)
		{
			case DtDeleteAll:		//	AWL-135, HA-134		(event used to be DeleteAllCutTestTotals in vxWorks)
			{
				TransactionManager* transManager = mgrContainer_->getTransactionManager();
				if ( !transManager )
					break;

				int count = 0;
				if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgCollectingTransactions) ||
					 Configuration::getInstance()->toBoolCfg(fieldOffsetCfgTrackingOperatorChanges) )
				{
					count = transManager->deleteAllCutTestTransactions(false);
					Debug::trace("CutTestTotalManager", "handleMessage(DtDeleteAll):  " + Integer::toString(count) + " Cutting Test Totals were cleared.");
				}
				else
				{
					count = transManager->deleteAllCutTestTransactions(true);
					Debug::trace("CutTestTotalManager", "handleMessage(DtDeleteAll):  " + Integer::toString(count) + " Cutting Test Totals were deleted.");
				}
				break;
			}
			default:
				Debug::trace("CutTestTotalManager", "handleMessage(" + Integer::toString(event) + "):  I don't recognize this event!");
				break;
		}
	}
}


int CutTestTotalManager::initializeAllCutTestTotalsQuery()
{
	Debug::trace("CutTestTotalManager", "initializeAllCutTestTotalsQuery()");

	TransactionManager* transManager = mgrContainer_->getTransactionManager();
	if ( !transManager || !mySession_ ) return 0;


	int totalCount = 0;
	clearCachedCutTestTotals();
	transManager->initializeNotClearedQuery(false);

	for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
			trans = transManager->getNextReadClearTransaction(false) )
	{
		if (( trans->getVoid() == Transaction::VoidNotVoided ) && ( trans->getCutTestNumber() > 0 ))	//	HA-134: No record if CutTest number is NULL!
		{
			Util::CSmartPtr<CutTestTotal> total = getCachedCutTestTotal(trans->getCutTestNumber(), trans->getProductNumber(), trans->getDeptNum());

			if ( total.notNull() )
			{
				total->incrementWeight(trans->getFinalNetWeight());
				total->incrementPrice(trans->getTotalValue());
				if ( trans->getByCount() )
					total->incrementItems(trans->getByCount());
				else
					total->incrementItems(1);
				total->incrementPackages(trans->getNumberPackages());
				total->incrementTransactions();
				++totalCount;
			}
		}
	}
	return totalCount;
}


Util::CSmartPtr<CutTestTotal> CutTestTotalManager::getFirstCutTestTotal()
{
	Debug::trace("CutTestTotalManager", "getFirstCutTestTotal()");

	for ( cutTestIterator_ = cachedCutTestTotals_.begin(); cutTestIterator_ != cachedCutTestTotals_.end(); ++cutTestIterator_ )
	{
		for ( totalIterator_ = cutTestIterator_->second->begin(); totalIterator_ != cutTestIterator_->second->end(); ++totalIterator_ )
		{
			if ( totalIterator_->second->getPackages() )
				return totalIterator_->second;
		}
	}

	return Util::CSmartPtr<CutTestTotal>(0);
}


Util::CSmartPtr<CutTestTotal> CutTestTotalManager::getNextCutTestTotal()
{
	Debug::trace("CutTestTotalManager", "getNextCutTestTotal()");

	++totalIterator_;

	while ( cutTestIterator_ != cachedCutTestTotals_.end() )
	{
		while ( totalIterator_ != cutTestIterator_->second->end() )
		{
			if ( totalIterator_->second->getPackages() )
				return totalIterator_->second;
			else
				++totalIterator_;
		}

		if ( ++cutTestIterator_ != cachedCutTestTotals_.end() )
			totalIterator_ = cutTestIterator_->second->begin();
	}

	return Util::CSmartPtr<CutTestTotal>(0);
}


void CutTestTotalManager::clearCachedCutTestTotals()
{
	Debug::trace("CutTestTotalManager", "clearCachedCutTestTotals()");

	cachedCutTestTotals_.clear();

	cutTestIterator_ = cachedCutTestTotals_.end();
}


Util::CSmartPtr<CutTestTotal> CutTestTotalManager::getCachedCutTestTotal(HobRecordNum cutTestNumber, HobRecordNum productNumber, HobRecordNum deptNum)
{
	Debug::trace("CutTestTotalManager", "getCachedCutTestTotal()");

	if ( cachedCutTestTotals_.find(cutTestNumber) == cachedCutTestTotals_.end() )
		cachedCutTestTotals_[cutTestNumber] = new cutTestTotalsByProductContainer_type();

	Util::CSmartPtr<cutTestTotalsByProductContainer_type> cutTestTotals = cachedCutTestTotals_[cutTestNumber];
	if ( cutTestTotals->find(productNumber) == cutTestTotals->end() )
		(*cutTestTotals)[productNumber] = new CutTestTotal(cutTestNumber, productNumber, deptNum);

	return (*cutTestTotals)[productNumber];
}


CuttingTestReport *CutTestTotalManager::createCutTestReport(CuttingTest* inputMessage, int& totalCount)
{
	Debug::trace("CutTestTotalManager", "createCutTestReport()");

	Session* session = dynamic_cast<Session*>(mySession_);
	TransactionManager* transManager = mgrContainer_->getTransactionManager();
	if ( !inputMessage || !session || !transManager ) return 0;

	Configuration *scaleConfig = Configuration::getInstance();
	if (!scaleConfig)
		return 0;

	bool traceOn = Debug::isTracing("CuttingTestReport");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "createCutTestReport():  ";

//	long productYield = 0;
	HobString result, description;
//	short pricePrecision = scaleConfig->getPricePrecision();
//	short weightPrecision = scaleConfig->getWeightPrecision();
//	short yieldPower = 4;
//	short yieldPrecision = 1;
//	unsigned int maxDesc = 25;
//	long yieldFactor = Power::power(10, yieldPower);

	inputMessage->show();
	CuttingTest *cutTest = CuttingTest::findCuttingTest(inputMessage->getTestNumber(), mySession_);
	if (cutTest && traceOn)
	{
		cutTest->show();
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("itemCost = ").append(Long::toString(cutTest->getPrimalCost()));
		msgDebug2.append("itemWeight = ").append(Long::toString(cutTest->getPrimalWeight()));
		Debug::trace("CuttingTestReport", msgDebug2);
	}

	/* Read transactions from the database. */
	clearCachedCutTestTotals();

	// query database for transactions
	std::string startDate( cutTest->getStartDate().convertToDBFormat() );
	std::string endDate( cutTest->getEndDate().convertToDBFormat() );

	transManager->initializeAllFilteredTransactionsQuery(
		startDate,
		endDate,
		"0",
		Transaction::VoidNotVoided,
		Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum),
		"",
		longtostring( cutTest->getTestNumber() ) );

	// process transactions
	Util::CSmartPtr<Transaction> trans = transManager->getFirstFilteredTransaction();
	totalCount = 0;

	while ( trans.notNull() )
	{
		TransactionType::AffectsProductionFlag affectsProduction = trans->getAffectsProductionFlag();

		/* Populate the cut test totals. */
		Util::CSmartPtr<CutTestTotal> total = getCachedCutTestTotal(trans->getCutTestNumber(), trans->getProductNumber(), trans->getDeptNum());

		if ( total.notNull() )
		{
			if ( affectsProduction == TransactionType::AffectsProductionNegatively )
			{
				total->incrementWeight(-trans->getFinalNetWeight());
				total->incrementPrice(-trans->getTotalValue());
				if ( trans->getByCount() )
					total->incrementItems(-trans->getByCount());
				else
					total->incrementItems(-1);
				total->incrementPackages(-trans->getNumberPackages());
				total->incrementTransactions(-1);
				--totalCount;
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
				++totalCount;
			}
		}
		trans = transManager->getNextFilteredTransaction();
	}

	if (totalCount > 0)
	{
		CuttingTestReport *ctReport = new CuttingTestReport(cutTest);
		if (ctReport)
		{
			Util::CSmartPtr<CutTestTotal> total = getFirstCutTestTotal();
			while ( total.notNull() )
			{
				ctReport->add(total, mySession_);
				total = getNextCutTestTotal();
			}
			ctReport->finalize();
		}
		return ctReport;
	}
	return 0;
}
