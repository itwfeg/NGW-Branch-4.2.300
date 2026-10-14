#include "ClassTotalManager.h"


#include "TransactionTypeManager.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Class.h"
#include "Configuration.h"


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
	int ClassTotalManager::dataType = 0;
	std::string ClassTotalManager::dataTypeName("hfc::ClassTotalManager"); 
#endif


	ClassTotalManager::ClassTotalManager(HFCManagerContainer * container)
			: mySession_(0), mgrContainer_(container), cachedClassTotals_(), classIterator_(), totalIterator_()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}


	ClassTotalManager::~ClassTotalManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void ClassTotalManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == ClassTotalDev )
		{
//			switch ( msg->getEventType() )
			{
//				default:
				{
					Debug::trace("ClassTotalManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));

//					break;
				}
			}
		}
	}


	int ClassTotalManager::initializeAllClassTotalsQuery(const std::string& deptNum)
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return 0;


		clearCachedClassTotals();
		transManager->initializeNotClearedQuery(false, deptNum);

		for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
				trans = transManager->getNextReadClearTransaction(false) )
		{
			TransactionType::AffectsProductionFlag production = trans->getAffectsProductionFlag();
		
			if ( trans->getVoid() == Transaction::VoidNotVoided && production != TransactionType::AffectsProductionNone )
			{
				Util::CSmartPtr<ClassTotal> total = getCachedClassTotal(trans->getDeptNum(), trans->getReportingClass(), trans->getTransactionTypeNumber());

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
#if 1
						// Find the class record and get its reporting class. [PT 437]
						Class* repClass = Class::findRecord(trans->getReportingClass(), trans->getDeptNum(), mySession_);
						if ( repClass )
							total->setReportingClassNumber(repClass->getReportingClassNum());
#else
						// Find the product record
						Product* product = Product::findPLU(trans->getProductNumber(), mySession_);
						if ( product )
							total->setReportingClassNumber(product->getReportingClassNum());
#endif
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


	Util::CSmartPtr<ClassTotal> ClassTotalManager::getFirstClassTotal()
	{
		for ( classIterator_ = cachedClassTotals_.begin(); classIterator_ != cachedClassTotals_.end(); ++classIterator_ )
			for ( totalIterator_ = classIterator_->second->begin(); totalIterator_ != classIterator_->second->end(); ++totalIterator_ )
				if ( totalIterator_->second->getPackages() )
					return totalIterator_->second;

		return Util::CSmartPtr<ClassTotal>(0);
	}


	Util::CSmartPtr<ClassTotal> ClassTotalManager::getNextClassTotal()
	{
		++totalIterator_;

		while ( classIterator_ != cachedClassTotals_.end() )
		{
			while ( totalIterator_ != classIterator_->second->end() )
			{
				if ( totalIterator_->second->getPackages() )
					return totalIterator_->second;
				else
					++totalIterator_;
			}

			if ( ++classIterator_ != cachedClassTotals_.end() )
				totalIterator_ = classIterator_->second->begin();
		}

		return Util::CSmartPtr<ClassTotal>(0);
	}


	void ClassTotalManager::clearCachedClassTotals()
	{
		cachedClassTotals_.clear();

		classIterator_ = cachedClassTotals_.end();
	}


	Util::CSmartPtr<ClassTotal> ClassTotalManager::getCachedClassTotal(HobRecordNum deptNum, HobRecordNum classNumber, HobRecordNum transTypeNumber)
	{
		if ( cachedClassTotals_.find(classNumber) == cachedClassTotals_.end() )
			cachedClassTotals_[classNumber] = new classTotalsByTypeContainer_type();

		Util::CSmartPtr<classTotalsByTypeContainer_type> classTotals = cachedClassTotals_[classNumber];
		if ( classTotals->find(transTypeNumber) == classTotals->end() )
			(*classTotals)[transTypeNumber] = new ClassTotal(deptNum, classNumber, transTypeNumber);

		return (*classTotals)[transTypeNumber];
	}


}
