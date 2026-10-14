#include "HourlyTotalManager.h"


#include "TransactionTypeManager.h"
#include "TransactionManager.h"
#include "Transaction.h"

#if defined(LINUX_FIX)
#include "HourlyTotalEvent.h"
#include "ReadFilteredHourlyTotalsMessage.h"
#include "WriteHourlyTotalsMessage.h"
#else
#include "Messages/HourlyTotalEvent.h"
#include "Messages/ReadFilteredHourlyTotalsMessage.h"
#include "Messages/WriteHourlyTotalsMessage.h"
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
	int HourlyTotalManager::dataType = 0;
	std::string HourlyTotalManager::dataTypeName("hfc::HourlyTotalManager"); 
#endif


	HourlyTotalManager::HourlyTotalManager(HFCManagerContainer * container)
			: mySession_(0), mgrContainer_(container), firstHour_(0), lastHour_(0), iterator_(0), cachedHourlyTotals_(HourlyTotal::_maxHour_)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}


	HourlyTotalManager::~HourlyTotalManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void HourlyTotalManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == HourlyTotalDev )
		{
			switch ( msg->getEventType() )
			{
				case ReadFilteredHourlyTotals:
				{
					ReadFilteredHourlyTotalsMessage* message = dynamic_cast<ReadFilteredHourlyTotalsMessage*>(msg->getHfcObject());
					if ( !message ) return;

					handleReadFilteredHourlyTotals(message);

					break;
				}
				default:
				{
					Debug::trace("HourlyTotalManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));

					break;
				}
			}
		}
	}


	void HourlyTotalManager::handleReadFilteredHourlyTotals(ReadFilteredHourlyTotalsMessage* inputMessage)
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !inputMessage || !session || !transManager ) return;

		clearCachedHourlyTotals();

		/* Read transactions from the database. */
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
				Util::CSmartPtr<HourlyTotal> total = getCachedHourlyTotal(trans->getCommittedDateTime().getHour());

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

		updateCachedHourlyTotalsIterator();


		/* Send the information to the User Interface. */
		Util::CSmartPtr<WriteHourlyTotalsMessage> response(new WriteHourlyTotalsMessage());

		/* For the UI we send intermediate hourly totals even if there are no transactions
		in them (notice the 'false' parameter to the 'getNextHourlyTotal' function call). */
		for ( Util::CSmartPtr<HourlyTotal> total = getFirstHourlyTotal(); total.notNull(); total = getNextHourlyTotal(false) )
			response->addHourlyTotal(total);

		SessionMessage outputMessage(HourlyTotalDev, WriteFilteredHourlyTotals);
		outputMessage.setFormatType(BrowserFormat);
		outputMessage.setHfcObject(response.get());

		mySession_->send(&outputMessage, Messenger::DEST_UI);
	}


	int HourlyTotalManager::initializeAllHourlyTotalsQuery()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return 0;


		clearCachedHourlyTotals();
		transManager->initializeNotClearedQuery(false);

		for ( Util::CSmartPtr<Transaction> trans = transManager->getFirstReadClearTransaction(false); trans.notNull();
				trans = transManager->getNextReadClearTransaction(false) )
		{
			TransactionType::AffectsProductionFlag production = trans->getAffectsProductionFlag();
		
			if ( trans->getVoid() == Transaction::VoidNotVoided && production != TransactionType::AffectsProductionNone )
			{
				Util::CSmartPtr<HourlyTotal> total = getCachedHourlyTotal(trans->getCommittedDateTime().getHour());

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


		updateCachedHourlyTotalsIterator();

		return 0;
	}


	Util::CSmartPtr<HourlyTotal> HourlyTotalManager::getFirstHourlyTotal()
	{
		iterator_ = firstHour_;

		if ( iterator_ < HourlyTotal::_maxHour_ )
			return cachedHourlyTotals_[iterator_];

		return Util::CSmartPtr<HourlyTotal>(0);
	}


	Util::CSmartPtr<HourlyTotal> HourlyTotalManager::getNextHourlyTotal(bool onlyNotEmpty)
	{
		if ( !onlyNotEmpty )
		{ // Whether there are items or not, we send the intermediate total record
			if ( ++iterator_ < lastHour_ )
				return cachedHourlyTotals_[iterator_];
		}
		else
		{ // only totals with items in them will be sent out
			while ( ++iterator_ < lastHour_ && !cachedHourlyTotals_[iterator_]->getItems() ) ;
			if ( iterator_ < lastHour_ )
				return cachedHourlyTotals_[iterator_];
		}

		return Util::CSmartPtr<HourlyTotal>(0);
	}


	void HourlyTotalManager::clearCachedHourlyTotals()
	{
		for ( int i = 0; i < HourlyTotal::_maxHour_; ++i )
			cachedHourlyTotals_[i] = new HourlyTotal(i);

		firstHour_ = lastHour_ = iterator_ = HourlyTotal::_maxHour_;
	}


	void HourlyTotalManager::updateCachedHourlyTotalsIterator()
	{
		for ( firstHour_ = 0; firstHour_ < HourlyTotal::_maxHour_ && cachedHourlyTotals_[firstHour_]->getTransactions() == 0; ++firstHour_ )
			;

		for ( lastHour_ = HourlyTotal::_maxHour_; lastHour_ > firstHour_ && cachedHourlyTotals_[lastHour_ - 1]->getTransactions() == 0; --lastHour_ )
			;

		iterator_ = firstHour_ - 1;
	}


	Util::CSmartPtr<HourlyTotal> HourlyTotalManager::getCachedHourlyTotal(short hour)
	{
		Util::CSmartPtr<HourlyTotal> total = cachedHourlyTotals_[hour];

		if ( total.null() )
		{
			total = new HourlyTotal(hour);
			cachedHourlyTotals_[hour] = total;
		}

		return total;
	}


}
