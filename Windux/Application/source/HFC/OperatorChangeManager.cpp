#include "OperatorChangeManager.h"


#include "OperatorChangeSummary.h"
#include "OperatorManager.h"
#include "TransactionManager.h"
#include "Transaction.h"

#if defined(LINUX_FIX)
#include "OperatorChangeEvent.h"
#include "ReadOperatorChangeSummaryMessage.h"
#include "OperatorChangesSummaryMessage.h"
#include "ReadOperatorChangeGroupMessage.h"
#include "WriteOperatorChangeGroupMessage.h"
#else
#include "Messages/OperatorChangeEvent.h"
#include "Messages/ReadOperatorChangeSummaryMessage.h"
#include "Messages/OperatorChangesSummaryMessage.h"
#include "Messages/ReadOperatorChangeGroupMessage.h"
#include "Messages/WriteOperatorChangeGroupMessage.h"
#endif
#include "WriteSingleField.h"


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
	int OperatorChangeManager::dataType = 0;
	std::string OperatorChangeManager::dataTypeName("hfc::OperatorChangeManager"); 
#endif


	OperatorChangeManager::OperatorChangeManager(HFCManagerContainer * container)
			: mySession_(0), mgrContainer_(container)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
	}


	OperatorChangeManager::~OperatorChangeManager()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void OperatorChangeManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == OperatorChangesDev )
		{
			switch ( msg->getEventType() )
			{
				case ReadOperatorChangeSummary:
				{
					ReadOperatorChangeSummaryMessage* message = dynamic_cast<ReadOperatorChangeSummaryMessage*>(msg->getHfcObject());
					if ( !message ) return;

					handleReadOperatorChangeSummary(message);

					break;
				}
				case ReadOperatorChangesGroup:
				{
					ReadOperatorChangeGroupMessage* message = dynamic_cast<ReadOperatorChangeGroupMessage*>(msg->getHfcObject());
					if ( !message ) return;

					handleReadOperatorChangeGroup(message);

					break;
				}
				default:
				{
					Debug::trace("OperatorChangeManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));

					break;
				}
			}
		}
	}


	void OperatorChangeManager::handleReadOperatorChangeSummary(ReadOperatorChangeSummaryMessage* inputMessage)
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !inputMessage || !session || !transManager ) return;


		/* Read transactions from the database. */
		clearCachedOperatorSummaries();
		HobRecordNum transactionType = util_string::stringtolong(inputMessage->getTransactionType());
		std::string viewChangesWhereAt = inputMessage->getViewChangesWhere(); // init msg request - view operator or supervisor changes 
		if( viewChangesWhereAt == "" )
		{
			inputMessage->setViewChangesWhere("0");
			viewChangesWhereAt = "0";   // the request where either Operate(0) or Supervisor(1) types of changes
		}

		bool allProduction = transactionType == 0;
		TransactionType::AffectsProductionFlag noAffect = TransactionType::AffectsProductionNone;
		transManager->initializeAllFilteredTransactionsQuery(inputMessage->getStartDateTime(), inputMessage->getEndDateTime(),
			inputMessage->getTransactionType(), Transaction::VoidNotVoided, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum), inputMessage->getPLUNumber());

		Util::CSmartPtr<Transaction> trans = transManager->getFirstFilteredTransaction();

		while ( trans.notNull() )
		{
			bool useTransaction = false;
			TransactionType::AffectsProductionFlag affectsProduction = trans->getAffectsProductionFlag();
			std::string transModeChange = util_string::inttostring( trans->getModeChange() );				// where the change actually occurred. Operate(0) or Supervisor(1)

			if ( allProduction )
			{
				if ( affectsProduction )
					useTransaction = true;
				else if( affectsProduction == noAffect && viewChangesWhereAt == "1" && transModeChange == "1" )
					useTransaction = true;				// grabs transactions made in Supervisor mode if that's what Im requesting
			}
			else
			{
				if ( trans->getTransactionTypeNumber() == transactionType )
					useTransaction = true;
			}
			
			if( transModeChange != viewChangesWhereAt )
				useTransaction = false;					// even if the other code made it true, its false if we have the wrong change location.

			/* Populate the summary records. */
			if ( useTransaction && trans->getOperatorChangeType() != 0 )
			{
				Util::CSmartPtr<OperatorChangeSummary> summary = getCachedOperatorSummary(trans->getOperatorId(), trans->getOperatorName());

				if ( summary.notNull() )
				{
					if ( allProduction && affectsProduction == TransactionType::AffectsProductionNegatively )
					{
						summary->incrementTransactionQuantity(-1);
						summary->incrementOldTotal(-trans->getOldTotalValue());
						summary->incrementDelta(-(trans->getTotalValue() - trans->getOldTotalValue()));
					}
					else
					{
						summary->incrementTransactionQuantity();
						summary->incrementOldTotal(trans->getOldTotalValue());
						summary->incrementDelta(trans->getTotalValue() - trans->getOldTotalValue());
					}

					// FT-1392 - LAP - If the first transaction was processed when the operator did not have a name set up,
					// but they then put a name in the operator record, the summary record will still have a blank operator name.
					// We never checked for the name changing.  Now we will update the name in the summary record if it changes,
					// but not if it changes back to blank.  Note, that if the store reuses operator records, and so changes the operator name,
					// the most current operator name will be reported in the Operator Changes summary page, even though all the transactions
					// are not attributed to that person.
					if ( summary->getOperatorName() != trans->getOperatorName() && !trans->getOperatorName().empty() )
						summary->setOperatorName( trans->getOperatorName() );
				}
			}

			trans = transManager->getNextFilteredTransaction();
		}


		/* Send the information to the User Interface. */
		Util::CSmartPtr<OperatorChangesSummaryMessage> response(new OperatorChangesSummaryMessage());

		Util::CSmartPtr<OperatorChangeSummary> summary;
		for ( summary = getFirstOperatorSummary(); summary.notNull(); summary = getNextOperatorSummary() )
			response->addOperatorSummary(summary);

		SessionMessage outputMessage(OperatorChangesDev, WriteOperatorChangeSummary);
		outputMessage.setFormatType(BrowserFormat);
		outputMessage.setHfcObject(response.get());

		//PTR #771 -> //So we can see the "Getting operator change records. Please wait..." message
		PAL_absclass::PAL_sleep( 1000 );

		mySession_->send(&outputMessage, Messenger::DEST_UI);
	}


	void OperatorChangeManager::handleReadOperatorChangeGroup(ReadOperatorChangeGroupMessage* inputMessage)
	{
		/* This is the response message with the data desired (which may be empty). */
		Util::CSmartPtr<WriteOperatorChangeGroupMessage> response;
		switch ( inputMessage->getReadingGroup() )
		{
			case ReadOperatorChangeGroupMessage::GroupFirst:
			{
				/* For first group we move forward navigating the query from the beginning. */
				response = getFirstOperatorChangeGroup(inputMessage);

				break;
			}
			case ReadOperatorChangeGroupMessage::GroupNext:
			{
				/* For next group we move forward navigating the query. */
				response = getNextOperatorChangeGroup(inputMessage->getMaxQuantity(), util_string::stringtoint(inputMessage->getViewChangesWhere()) );

				break;
			}
			case ReadOperatorChangeGroupMessage::GroupPrevious:
			{
				/* For previous group we move backward navigating the query. */
				response = getPreviousOperatorChangeGroup(inputMessage->getMaxQuantity(), util_string::stringtoint(inputMessage->getViewChangesWhere()));

				break;
			}
			default: break;
		}

		/* Send the response to the User Interface. */
		if ( response.notNull() )
		{
			SessionMessage outputMessage(OperatorChangesDev, WriteOpertorChangesGroup);
			outputMessage.setFormatType(BrowserFormat);
			outputMessage.setHfcObject(response.get());

			mySession_->send(&outputMessage, Messenger::DEST_UI);
		}
	}


	void OperatorChangeManager::findFirstOperatorChange(TransactionManager* transManager, int viewWhatChanges)
	{
		topChangeGroupTrans_ = 0;
		topTransChangeType_ = OperatorChange::end_OpChangeFieldType;

		bottomChangeGroupTrans_ = transManager->getFirstFilteredTransaction();
		bottomTransChangeType_ = OperatorChange::OC_Unknown;

		findNextOperatorChange(transManager, viewWhatChanges);
	}


	void OperatorChangeManager::findNextOperatorChange(TransactionManager* transManager, int viewWhatChanges)
	{		
		TransactionType::AffectsProductionFlag noAffect = TransactionType::AffectsProductionNone;

		while ( bottomChangeGroupTrans_.notNull() )
		{
			if ( bottomChangeGroupTrans_->getOperatorChangeType() != 0 && bottomTransChangeType_ < OperatorChange::end_OpChangeFieldType )
			{
				TransactionType::AffectsProductionFlag affectsProduction = bottomChangeGroupTrans_->getAffectsProductionFlag();
				int transModeChange = bottomChangeGroupTrans_->getModeChange();		// for this trans, where the change occurr?. Operate(0) or Supervisor(1)
				bool useTransaction = false;

				if ( changeGroupTransType_ == 0 )
				{
					if ( bottomChangeGroupTrans_->getAffectsProductionFlag() )
						useTransaction = true;
					else if( affectsProduction == noAffect && viewWhatChanges == 1 && transModeChange == 1 )
						useTransaction = true;		// grabs transactions made in Supervisor mode if that's what Im requesting
				}
				else
				{
					if ( bottomChangeGroupTrans_->getTransactionTypeNumber() == changeGroupTransType_ )
						useTransaction = true;
				}

				if( transModeChange != viewWhatChanges )
					useTransaction = false;			// even if the other code made it true, its false if we have the wrong change location.

				if ( useTransaction )
				{
					if ( bottomTransChangeType_ < OperatorChange::end_OpChangeFieldType )
						bottomTransChangeType_ = (OperatorChange::OperatorChangeableField)(bottomTransChangeType_ + 1);

					while ( bottomTransChangeType_ < OperatorChange::end_OpChangeFieldType )
					{
						if ( bottomChangeGroupTrans_->isOperatorChangeType(bottomTransChangeType_) )
							return;

						if ( bottomTransChangeType_ < OperatorChange::end_OpChangeFieldType )
							bottomTransChangeType_ = (OperatorChange::OperatorChangeableField)(bottomTransChangeType_ + 1);
					}
				}
			}

			bottomChangeGroupTrans_ = transManager->getNextFilteredTransaction();
			bottomTransChangeType_ = OperatorChange::OC_Unknown;
		}

		return;
	}


	void OperatorChangeManager::findPreviousOperatorChange(TransactionManager* transManager, int viewWhatChanges)
	{
		TransactionType::AffectsProductionFlag noAffect = TransactionType::AffectsProductionNone;

		while ( topChangeGroupTrans_.notNull() )
		{
			if ( topChangeGroupTrans_->getOperatorChangeType() != 0 && topTransChangeType_ > OperatorChange::OC_Unknown )
			{
				TransactionType::AffectsProductionFlag affectsProduction = bottomChangeGroupTrans_->getAffectsProductionFlag();
				int transModeChange = bottomChangeGroupTrans_->getModeChange();		// for this trans, where the change occurr?. Operate(0) or Supervisor(1)
				bool useTransaction = false;

				if ( changeGroupTransType_ == 0 )
				{
					if ( topChangeGroupTrans_->getAffectsProductionFlag() )
						useTransaction = true;
					else if( affectsProduction == noAffect && viewWhatChanges == 1 && transModeChange == 1 )
						useTransaction = true;		// grabs transactions made in Supervisor mode if that's what Im requesting
				}
				else
				{
					if ( topChangeGroupTrans_->getTransactionTypeNumber() == changeGroupTransType_ )
						useTransaction = true;
				}

				if( transModeChange != viewWhatChanges )
					useTransaction = false;			// even if the other code made it true, its false if we have the wrong change location.

				if ( useTransaction )
				{
					if ( topTransChangeType_ > OperatorChange::OC_Unknown )
						topTransChangeType_ = (OperatorChange::OperatorChangeableField)(topTransChangeType_ - 1);

					while ( topTransChangeType_ > OperatorChange::OC_Unknown )
					{
						if ( topChangeGroupTrans_->isOperatorChangeType(topTransChangeType_) )
							return;

						if ( topTransChangeType_ > OperatorChange::OC_Unknown )
							topTransChangeType_ = (OperatorChange::OperatorChangeableField)(topTransChangeType_ - 1);
					}
				}
			}

			topChangeGroupTrans_ = transManager->getPreviousFilteredTransaction();
			topTransChangeType_ = OperatorChange::end_OpChangeFieldType;
		}

		return;
	}


	Util::CSmartPtr<WriteOperatorChangeGroupMessage> OperatorChangeManager::getFirstOperatorChangeGroup(ReadOperatorChangeGroupMessage* inputMessage)
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !inputMessage || !transManager ) return Util::CSmartPtr<WriteOperatorChangeGroupMessage>(0);


		/* Read transactions from the database. */
		changeGroupTransType_ = util_string::stringtolong(inputMessage->getTransactionType());
		changeGroupOperatorId_ = inputMessage->getOperatorId();

		HobOperator* oper = HobOperator::find(changeGroupOperatorId_, mgrContainer_);
		changeGroupOperatorName_.assign(oper ? oper->getOpName() : " ");

		transManager->initializeAllFilteredTransactionsQuery(inputMessage->getStartDateTime(), inputMessage->getEndDateTime(),
		inputMessage->getTransactionType(), Transaction::VoidNotVoided, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum), inputMessage->getPLUNumber(), "", changeGroupOperatorId_, false);

		int viewChangesFrom = util_string::stringtoint( inputMessage->getViewChangesWhere() );   // util_string::inttostring( 
		findFirstOperatorChange(transManager, viewChangesFrom);

		return getNextOperatorChangeGroup(inputMessage->getMaxQuantity(), viewChangesFrom);
	}


	Util::CSmartPtr<WriteOperatorChangeGroupMessage> OperatorChangeManager::getNextOperatorChangeGroup(int quantity, int viewChangesFrom)
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !session || !transManager ) return Util::CSmartPtr<WriteOperatorChangeGroupMessage>(0);


		transManager->resetPreviousFilteredTransaction();
		topChangeGroupTrans_ = bottomChangeGroupTrans_;
		topTransChangeType_ = bottomTransChangeType_;
		findPreviousOperatorChange(transManager, viewChangesFrom);

		Util::CSmartPtr<WriteOperatorChangeGroupMessage> group(new WriteOperatorChangeGroupMessage(changeGroupOperatorName_));
		while ( bottomChangeGroupTrans_.notNull() && quantity > 0 )
		{
			group->addOperatorChange(bottomChangeGroupTrans_, bottomTransChangeType_);
			--quantity;

			findNextOperatorChange(transManager, viewChangesFrom);
		}

		group->setPreviousGroupAvailable(topChangeGroupTrans_.notNull());
		group->setNextGroupAvailable(bottomChangeGroupTrans_.notNull());

		return group;
	}


	Util::CSmartPtr<WriteOperatorChangeGroupMessage> OperatorChangeManager::getPreviousOperatorChangeGroup(int quantity, int viewChangesFrom)
	{
		Session* session = dynamic_cast<Session*>(mySession_);
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !session || !transManager ) return Util::CSmartPtr<WriteOperatorChangeGroupMessage>(0);


		transManager->resetNextFilteredTransaction();
		bottomChangeGroupTrans_ = topChangeGroupTrans_;
		bottomTransChangeType_ = topTransChangeType_;
		findNextOperatorChange(transManager, viewChangesFrom);

		Util::CSmartPtr<WriteOperatorChangeGroupMessage> group(new WriteOperatorChangeGroupMessage(changeGroupOperatorName_, false));
		while ( topChangeGroupTrans_.notNull() && quantity > 0 )
		{
			group->addOperatorChange(topChangeGroupTrans_, topTransChangeType_);
			--quantity;

			findPreviousOperatorChange(transManager, viewChangesFrom);
		}

		group->setPreviousGroupAvailable(topChangeGroupTrans_.notNull());
		group->setNextGroupAvailable(bottomChangeGroupTrans_.notNull());

		return group;
	}


	int OperatorChangeManager::initializeAllOperatorChangesQuery()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return 0;


		transManager->initializeNotClearedQuery(false);

		return 0;
	}


	Util::CSmartPtr<OperatorChange> OperatorChangeManager::getFirstOperatorChange()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return Util::CSmartPtr<OperatorChange>(0);


		currentTransaction_ = transManager->getFirstReadClearTransaction(false);
		currentTransChangeType_ = (OperatorChange::OperatorChangeableField)(OperatorChange::OC_Unknown + 1);

		return getNextOperatorChange();
	}


	Util::CSmartPtr<OperatorChange> OperatorChangeManager::getNextOperatorChange()
	{
		TransactionManager* transManager = mgrContainer_->getTransactionManager();
		if ( !transManager ) return Util::CSmartPtr<OperatorChange>(0);


		Util::CSmartPtr<OperatorChange> change;

		while ( currentTransaction_.notNull() )
		{
			if ( currentTransaction_->getVoid() != Transaction::VoidVoiding && currentTransaction_->getOperatorChangeType() != 0 )
			{
				while ( currentTransChangeType_ < OperatorChange::end_OpChangeFieldType && change.null() )
				{
					if ( currentTransaction_->isOperatorChangeType(currentTransChangeType_) )
						change = new OperatorChange(currentTransaction_, currentTransChangeType_);

					currentTransChangeType_ = (OperatorChange::OperatorChangeableField)(currentTransChangeType_ + 1);
				}

				if ( change.notNull() )
					return change;
			}

			currentTransaction_ = transManager->getNextReadClearTransaction(false);
			currentTransChangeType_ = (OperatorChange::OperatorChangeableField)(OperatorChange::OC_Unknown + 1);
		}

		return Util::CSmartPtr<OperatorChange>(0);
	}

/*
	Util::CSmartPtr<OperatorChange> OperatorChangeManager::createOperatorChange(WriteSingleField *productUpdate, HobString& newValue)
	{
		bool traceOn = Debug::isTracing("OperatorChangeManager");
		std::string methodDebug("createOperatorChange() from WriteSingleField: ");
		Util::CSmartPtr<OperatorChange> opChange;

		if ( !productUpdate || !mgrContainer_->getOperatorManager() ) return opChange;


		if ( productUpdate->getFieldOrigValue() != newValue )
		{
			HobOperator *myOperator = mgrContainer_->getOperatorManager()->getLoggedInOperator();
			if ( !myOperator && traceOn )
				Debug::trace("OperatorChangeManager", methodDebug + "There is no current Operator!!!");

			//	Record the operator change, whether an operator is logged in or not.
			if ( productUpdate->getFieldCode() > 0 )
			{
				opChange = new OperatorChange(myOperator, productUpdate->getRecordNum(),
						productUpdate->getFieldCode(), newValue, productUpdate->getFieldOrigValue());

				if ( opChange.notNull() && (opChange->getChangedField() == OperatorChange::OC_DoNotRecord) )
				{
					if ( traceOn )
						Debug::trace("OperatorChangeManager", methodDebug + "(opChangeFieldType == OC_DoNotRecord) = " +
								util_string::booltostring(opChange->getChangedField() == OperatorChange::OC_DoNotRecord));

					//	We will not record this OpChange.
					opChange = 0;
				}
			}

			if ( traceOn )
			{
				Debug::trace("OperatorChangeManager", methodDebug + "   fieldCode = " +
					util_string::inttostring(productUpdate->getFieldCode()) + "   opChange = " + util_string::ptrtostring(opChange) +
					(opChange.notNull() ? "   opChangeFieldType = " + util_string::inttostring(opChange->getChangedField()) : string("")));
			}
		}

		return opChange;
	}
*/
/*
	Util::CSmartPtr<OperatorChange> OperatorChangeManager::createOperatorChange(HobRecordNum productNum,
			OperatorChange::OperatorChangeableField opChangeField, long origValue, long newValue)
	{
		Util::CSmartPtr<OperatorChange> opChange;

		if ( origValue != newValue )
		{
			HobOperator *myOperator = (mgrContainer_ && mgrContainer_->getOperatorManager()) ? mgrContainer_->getOperatorManager()->getLoggedInOperator() : 0;

			//	Record the operator change, whether an operator is logged in or not.
			opChange = new OperatorChange(myOperator, productNum, opChangeField, util_string::longtostring(newValue), util_string::longtostring(origValue));

			Debug::trace("OperatorManager", "createOperatorChange() from WriteCurrentTotalValues: " +
					string(myOperator ? "Creating an OperatorChange record." : "There is no current Operator!!!") +
					"    opChangeField = " + util_string::inttostring(opChangeField) +
					"    opChange = " + util_string::ptrtostring(opChange));
		}

		return opChange;
	}
*/

	void OperatorChangeManager::clearCachedOperatorSummaries()
	{
		cachedOperatorSummaries_.clear();

		summaryIterator_ = cachedOperatorSummaries_.end();
	}


	Util::CSmartPtr<OperatorChangeSummary> OperatorChangeManager::getCachedOperatorSummary(const std::string& id, const std::string& name)
	{
		operatorSummaryContainer_type::iterator iter = cachedOperatorSummaries_.find(id);

		if ( iter == cachedOperatorSummaries_.end() )
		{
			std::pair<operatorSummaryContainer_type::iterator, bool> result = cachedOperatorSummaries_.insert(std::make_pair(id, Util::CSmartPtr<OperatorChangeSummary>(new OperatorChangeSummary(id, name))));
			return result.second ? result.first->second : Util::CSmartPtr<OperatorChangeSummary>(0);
		}
		else
			return iter->second;
	}


	Util::CSmartPtr<OperatorChangeSummary> OperatorChangeManager::getFirstOperatorSummary()
	{
		summaryIterator_ = cachedOperatorSummaries_.begin();

		return summaryIterator_ != cachedOperatorSummaries_.end() ? summaryIterator_->second : Util::CSmartPtr<OperatorChangeSummary>(0);
	}


	Util::CSmartPtr<OperatorChangeSummary> OperatorChangeManager::getNextOperatorSummary()
	{
		++summaryIterator_;

		return summaryIterator_ != cachedOperatorSummaries_.end() ? summaryIterator_->second : Util::CSmartPtr<OperatorChangeSummary>(0);
	}


}
