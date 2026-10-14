#include "OperatorTotalManager.h"


#include "OperatorManager.h"
#include "TransactionTypeManager.h"
#include "TransactionManager.h"
#include "Transaction.h"

#if defined(LINUX_FIX)
#include "OperatorTotalEvent.h"
#include "ReadFilteredOperatorTotalsMessage.h"
#include "WriteOperatorTotalsMessage.h"
#else
#include "Messages/OperatorTotalEvent.h"
#include "Messages/ReadFilteredOperatorTotalsMessage.h"
#include "Messages/WriteOperatorTotalsMessage.h"
#endif

#include "Session.h"
#include "DBSession.h"
#include "SessionMessage.h"
#include "Configuration.h"



#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorTotalManager::dataType = 0;
	std::string OperatorTotalManager::dataTypeName("hfc::OperatorTotalManager"); 
#endif


	OperatorTotalManager::OperatorTotalManager(HFCManagerContainer * container)
			: mySession_(0), mgrContainer_(container), cachedOperatorTotals_(), iterator_()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}


	OperatorTotalManager::~OperatorTotalManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void OperatorTotalManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == OperatorTotalDev )
		{
			switch ( msg->getEventType() )
			{
				case ReadFilteredOperatorTotals:
				{
					ReadFilteredOperatorTotalsMessage* message = dynamic_cast<ReadFilteredOperatorTotalsMessage*>(msg->getHfcObject());
					if ( !message ) return;

					handleReadFilteredOperatorTotals(message);

					break;
				}
				default:
				{
					Debug::trace("OperatorTotalManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));

					break;
				}
			}
		}
	}


	void OperatorTotalManager::handleReadFilteredOperatorTotals(ReadFilteredOperatorTotalsMessage* inputMessage)
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		OperatorManager* operatorManager = mgrContainer_->getOperatorManager();
		if ( !inputMessage || !session || !transManager || !operatorManager ) return;

		std::map<std::string, std::string> names;


		/* Read operators from the database. */
		Util::CSmartPtr<HobOperator> oper(operatorManager->getFirstRecord());
		while ( oper.notNull() )
		{
			names[oper->getOpNum()] = oper->getOpName();
			oper = operatorManager->getNextRecord();
		}


		/* Read transactions from the database. */
		clearCachedOperatorTotals();
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

			/* Populate the hourly totals. */
			if ( useTransaction )
			{
				Util::CSmartPtr<OperatorTotal> total = getCachedOperatorTotal(trans->getOperatorId());

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
		Util::CSmartPtr<WriteOperatorTotalsMessage> response(new WriteOperatorTotalsMessage());

		Util::CSmartPtr<OperatorTotal> total;
		for ( total = getFirstOperatorTotal(); total.notNull(); total = getNextOperatorTotal() )
			response->addOperatorTotal(total, names[total->getOperatorId()]);

		SessionMessage outputMessage(OperatorTotalDev, WriteFilteredOperatorTotals);
		outputMessage.setFormatType(BrowserFormat);
		outputMessage.setHfcObject(response.get());

		mySession_->send(&outputMessage, Messenger::DEST_UI);
	}


	int OperatorTotalManager::initializeAllOperatorTotalsQuery()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return 0;


		clearCachedOperatorTotals();
		transManager->initializeNotClearedQuery(false);

		for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
				trans = transManager->getNextReadClearTransaction(false) )
		{
			TransactionType::AffectsProductionFlag production = trans->getAffectsProductionFlag();
		
			if ( trans->getVoid() == Transaction::VoidNotVoided && production != TransactionType::AffectsProductionNone )
			{
				Util::CSmartPtr<OperatorTotal> total = getCachedOperatorTotal(trans->getOperatorId());

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

					if ( !total->isFlagsSet() )
					{
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


	Util::CSmartPtr<OperatorTotal> OperatorTotalManager::getFirstOperatorTotal()
	{
		iterator_ = cachedOperatorTotals_.begin();

		return iterator_ != cachedOperatorTotals_.end() ? iterator_->second : Util::CSmartPtr<OperatorTotal>(0);
	}


	Util::CSmartPtr<OperatorTotal> OperatorTotalManager::getNextOperatorTotal()
	{
		++iterator_;

		return iterator_ != cachedOperatorTotals_.end() ? iterator_->second : Util::CSmartPtr<OperatorTotal>(0);
	}


	void OperatorTotalManager::clearCachedOperatorTotals()
	{
		cachedOperatorTotals_.clear();

		iterator_ = cachedOperatorTotals_.end();
	}


	Util::CSmartPtr<OperatorTotal> OperatorTotalManager::getCachedOperatorTotal(const std::string& id)
	{
		operatorTotalsContainer_type::iterator iter = cachedOperatorTotals_.find(id);

		if ( iter == cachedOperatorTotals_.end() )
		{
			std::pair<operatorTotalsContainer_type::iterator, bool> result = cachedOperatorTotals_.insert(std::make_pair(id, Util::CSmartPtr<OperatorTotal>(new OperatorTotal(id))));
			return result.second ? result.first->second : Util::CSmartPtr<OperatorTotal>(0);
		}
		else
			return iter->second;
	}


}
