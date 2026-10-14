#include "COOLTotalManager.h"


#include "TransactionManager.h"
#include "Transaction.h"


#include "Event.h"


#include "Session.h"
#include "DBSession.h"
#include "SessionMessage.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int COOLTotalManager::dataType = 0;
	std::string COOLTotalManager::dataTypeName("hfc::COOLTotalManager"); 
#endif


	COOLTotalManager::COOLTotalManager(hfc::HFCManagerContainer *container)
			: mySession_(0), mgrContainer_(container)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}


	COOLTotalManager::~COOLTotalManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void COOLTotalManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == COOLTotalDev )
		{
//			switch ( msg->getEventType() )
			{
//				default:
				{
					Debug::trace("COOLTotalManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));

//					break;
				}
			}
		}
	}


	int COOLTotalManager::initializeAllCOOLTotalsQuery()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return 0;


		clearCachedCOOLTotals();
		transManager->initializeNotClearedQuery(false);

		for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
				trans = transManager->getNextReadClearTransaction(false) )
		{
			if ( trans->getVoid() == Transaction::VoidNotVoided && trans->isCOOLTransaction() )
			{
				Util::CSmartPtr<COOLTotal> total = getCachedCOOLTotal(trans->getProductNumber(),
					trans->getTransactionTypeNumber(), trans->getCOOLTrackingText(), trans->getCOOLText());

				if ( total.notNull() )
				{
//					if ( !total->isCoolTextVaries() )
//					{   // For the first transaction simply copy the text, for subsequent ones see if it changes
//						if ( total->getTransactions() == 0 )
//							total->setCoolText(trans->getCOOLText());
//						else if ( total->getCoolText() != trans->getCOOLText() )
//						{
//							total->setCoolTextVaries();
//							total->setCoolText("");
//						}
//					}

					if ( total->getTransactions() )
						total->setDateTime(trans->getCommittedDateTime());
					else
					{
						total->setFirstDateTime(trans->getCommittedDateTime());
						total->setLastDateTime(trans->getCommittedDateTime());
					}
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


		return 0;
	}


	Util::CSmartPtr<COOLTotal> COOLTotalManager::getFirstCOOLTotal()
	{
		for ( pluCoolIterator_ = cachedCOOLTotals_.begin(); pluCoolIterator_ != cachedCOOLTotals_.end(); ++pluCoolIterator_ )
		{
			pluCoolTotals_ = pluCoolIterator_->second;
			for ( totalByCodeIterator_ = pluCoolTotals_->begin(); totalByCodeIterator_ != pluCoolTotals_->end(); ++totalByCodeIterator_ )
			{
				totalsByCode_ = totalByCodeIterator_->second;
				for ( totalIterator_ = totalsByCode_->begin(); totalIterator_ != totalsByCode_->end(); ++totalIterator_ )
					if ( totalIterator_->second->getPackages() )
						return totalIterator_->second;
			}
		}

		return Util::CSmartPtr<COOLTotal>(0);
	}


	Util::CSmartPtr<COOLTotal> COOLTotalManager::getNextCOOLTotal()
	{
		++totalIterator_;

		while ( pluCoolIterator_ != cachedCOOLTotals_.end() )
		{
			while ( totalByCodeIterator_ != pluCoolTotals_->end() )
			{
				while ( totalIterator_ != totalsByCode_->end() )
				{
					if ( totalIterator_->second->getPackages() )
						return totalIterator_->second;
					else
						++totalIterator_;
				}

				if ( ++totalByCodeIterator_ != pluCoolTotals_->end() )
				{
					totalsByCode_ = totalByCodeIterator_->second;
					totalIterator_ = totalsByCode_->begin();
				}
			}

			if ( ++pluCoolIterator_ != cachedCOOLTotals_.end() )
			{
				pluCoolTotals_ = pluCoolIterator_->second;
				totalByCodeIterator_ = pluCoolTotals_->begin();
				if ( totalByCodeIterator_ != pluCoolTotals_->end() )
				{
					totalsByCode_ = totalByCodeIterator_->second;
					totalIterator_ = totalsByCode_->begin();
				}
			}
		}

		return Util::CSmartPtr<COOLTotal>(0);
	}


	Util::CSmartPtr<COOLTotal> COOLTotalManager::createCOOLTotal(Util::CSmartPtr<Transaction> trans)
	{
		if ( trans.notNull() )
		{
			TransactionType::AffectsProductionFlag production = trans->getAffectsProductionFlag();
		
			if ( trans->getVoid() == Transaction::VoidNotVoided && production != TransactionType::AffectsProductionNone )
			{
				Util::CSmartPtr<COOLTotal> total(new COOLTotal(trans->getProductNumber(), trans->getTransactionTypeNumber(), trans->getCOOLTrackingText()));

				if ( total.notNull() )
				{
					total->setDateTime(trans->getCommittedDateTime());
					total->setCoolText(trans->getCOOLText());
					total->setTrackingText(trans->getCOOLTrackingText());

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

					return total;
				}
			}
		}

		return Util::CSmartPtr<COOLTotal>(0);
	}


	void COOLTotalManager::clearCachedCOOLTotals()
	{
		cachedCOOLTotals_.clear();

		pluCoolIterator_ = cachedCOOLTotals_.end();
	}


	Util::CSmartPtr<COOLTotal> COOLTotalManager::getCachedCOOLTotal(HobRecordNum productNumber, HobRecordNum transactionTypeNumber, std::string trackingCode, std::string coolText)
	{
		// We want to make sure that both, the cool text and the tracking number affect the cool total
		// so that if either one changes, then we get a new total record.
		std::string coolData = trackingCode + coolText;

		if ( cachedCOOLTotals_.find(productNumber) == cachedCOOLTotals_.end() )
			cachedCOOLTotals_[productNumber] = new pluCoolTotalsContainer_type();

		Util::CSmartPtr<pluCoolTotalsContainer_type> pluCoolTotals = cachedCOOLTotals_[productNumber];
		if ( pluCoolTotals->find(transactionTypeNumber) == pluCoolTotals->end() )
			(*pluCoolTotals)[transactionTypeNumber] = new coolTotalsByCodeContainer_type();

		Util::CSmartPtr<coolTotalsByCodeContainer_type> coolTotals = (*pluCoolTotals)[transactionTypeNumber];
		if ( coolTotals->find(coolData) == coolTotals->end() )
			(*coolTotals)[coolData] = new COOLTotal(productNumber, transactionTypeNumber, trackingCode, coolText);

		return (*coolTotals)[coolData];
	}


}
