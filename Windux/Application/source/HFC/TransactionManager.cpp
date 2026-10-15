#include "TransactionManager.h"

#include "TransactionDevEvent.h"
#include "ReadTransactionGroupMessage.h"
#include "WriteTransactionGroupMessage.h"
#include "VoidTransactionsMessage.h"
#include "TransactionDatesMessage.h"


#include "Session.h"
#include "DBSession.h"
#include "HFCManagerContainer.h"
#include "Transaction.h"
#include "OperatorManager.h"
#include "HobOperator.h"
#include "LabelType.h"

#include "SessionMessage.h"
#include "Configuration.h"
#include "ProductTotalManager.h"


#include "DBTransactionsRM.h"
#include "DBTransaction.h"
#include "DBException.h"

#include "SessionManager.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Debug.h"


#include "util/e_string/utilstring.h"
#include "util/time/datetime.h"
#include "MessageLogger.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



using namespace edb;



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int TransactionManager::_dataType_ = 0;
	string TransactionManager::_dataTypeName_("hfc::TransactionManager");

	int TransactionManager::NotClearedTransactionsModule::_dataType_ = 0;
	string TransactionManager::NotClearedTransactionsModule::_dataTypeName_("hfc::TransactionManager::NotClearedTransactionsModule");

	int TransactionManager::VoidableTransactionsModule::_dataType_ = 0;
	string TransactionManager::VoidableTransactionsModule::_dataTypeName_("hfc::TransactionManager::VoidableTransactionsModule");
#endif


TransactionManager::TransactionManager(hfc::HFCManagerContainer* container)
		: mySession_(0), mgrContainer_(container)
{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif
}


TransactionManager::~TransactionManager()
{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
}


void TransactionManager::handleMessage(SessionMessage* msg)
{
	if ( !msg ) return;

	if ( msg->getDeviceType() == TransactionDev )
	{
		switch ( msg->getEventType() )
		{
			case DtDeleteAll:
			{
				handleDeleteAll();
				break;
			}
			case DtRestore:
			{
				Transaction* transaction = dynamic_cast<Transaction*>(msg->getHfcObject());
				if ( !transaction ) return;

				if ( msg->getFormatType() == SynchronizationTaggedFormat || msg->getFormatType() == DBIndependentFormatFormat)
				{
					saveTransaction(transaction);

					Debug::trace("TransactionManager", "In upgrading the db received a transaction record, created it and saved it");
				}

				break;
			}
			case TransactionReadNonVoidGroup:
			{
				ReadTransactionGroupMessage* message = dynamic_cast<ReadTransactionGroupMessage*>(msg->getHfcObject());
				if ( !message ) return;

				handleReadNonVoidTransactions(message);

				break;
			}
			case TransactionVoidTransactions:
			{
				VoidTransactionsMessage* message = dynamic_cast<VoidTransactionsMessage*>(msg->getHfcObject());
				if ( !message ) return;

				handleVoidTransactions(message);

				break;
			}
			case TransactionReadDates:
			{
				handleReadDates();

				break;
			}
			case ClearProductAndClassTotals:
			case ClearOperatorTotals:
			case ClearHourlyTotals:
			case ClearCOOLTotals:
			case ClearAllTotals:
			case ClearOperatorChanges:
			{
				handleClearTotals(msg->getEventType());
				break;
			}
			default:
			{
				Debug::trace("TransactionManager", "UNKNOWN EVENT: " + util_string::uinttostring(msg->getEventType()));
				break;
			}
		}
	}

	return;
}


/* Use Case 3.5: Processing a delete transactions command. */
void TransactionManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void TransactionManager::handleDeleteAll(HobRecordNum deptNum)
{
	int records = deleteAllTransactions(deptNum);

	Debug::trace("TransactionManager", "Delete all transactions: " + util_string::inttostring(records) + " records.");
}


/* Use Case 3.2: Processing a clear totals command. */
void TransactionManager::handleClearTotals(UINT8 event)
{
	bool clearTransactions = false;
	Configuration* config = Configuration::getInstance();


	/* (uc3.2) Check whether this clear totals command is selected in configuration to clear
	transactions. If not, then end the execution of this use case. */
	switch ( event )
	{
		case ClearAllTotals:
			// This one doesn't take any configuration flags into consideration when clearing totals, as the others do directly below.
			clearTransactions = true;
			break;
		case ClearProductAndClassTotals:
		{
			if ( config->isClearProductClassTotalsClearsTransactions() )
				clearTransactions = true;

			break;
		}
		case ClearOperatorTotals:
		{
			if ( config->isClearOperatorTotalsClearsTransactions() )
				clearTransactions = true;

			break;
		}
		case ClearHourlyTotals:
		{
			if ( config->isClearHourlyTotalsClearsTransactions() )
				clearTransactions = true;

			break;
		}
		case ClearCOOLTotals:
		{
			if ( config->isClearCOOLTotalsClearsTransactions() )
				clearTransactions = true;

			break;
		}
		case ClearOperatorChanges:
		{
			if ( config->isClearOperatorChangesClearsTransactions() )
				clearTransactions = true;

			break;
		}
	}
	if ( !clearTransactions ) 
		return;

	/* (uc3.2) Otherwise, retrieve all not-yet-cleared transactions from the database. */
	/* (uc3.2) Navigate all the transactions marking them as cleared Read&Clear flag set to
	2) and saving them back to the database. */
	clearNotClearedTransactions();
}


void TransactionManager::commitTransaction(Transaction* transaction)
{
	Debug::trace("TransactionManager.commit", "Going to commit transaction @" + util_string::ptrtostring(transaction));

	if ( transaction->getTransactionId() == RECORD_NOT_FOUND )
		transaction->setTransactionId(generateNextTransactionId());

	transaction->setCommittedDateTime();

	Debug::trace("TransactionManager.commit", "Commited trans id: " + Long::toString(transaction->getTransactionId()));
	Debug::trace("TransactionManager.commit", "Commited date/time: " + transaction->getCommittedDateTimeAsString());
}


Util::CSmartPtr<Transaction> TransactionManager::createProductionEntryTransaction(Transaction* trans)
{
	Util::CSmartPtr<Transaction> prodEntryTransaction;
	if ( !trans )
		return prodEntryTransaction;


	prodEntryTransaction = new Transaction(*trans);
	prodEntryTransaction->setTransactionId(generateNextTransactionId());
	prodEntryTransaction->setTransactionGroupNumber(trans->getTransactionId());
	prodEntryTransaction->clearVoid();
	prodEntryTransaction->clearReadClearFlag();
	prodEntryTransaction->setNumberPackages(1);
	prodEntryTransaction->setTotalValue(trans->getFinalPrice());
	prodEntryTransaction->setOldTotalValue(trans->getOldFinalPrice());

	return prodEntryTransaction;
}

/* We want to retrieve all locally-generated, non-void transactions associated with an operator. */
void TransactionManager::handleReadNonVoidTransactions(ReadTransactionGroupMessage* inputMessage)
{
	Session* session = dynamic_cast<Session*>(mySession_);
	if ( !inputMessage || !session ) return;

	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM ) return;


	/* If this is the first group, then we need to setup the query. */
	if ( inputMessage->getReadingGroup() == ReadTransactionGroupMessage::GroupFirst )
	{
		bool operatorMode = session->isInOperateMode();
		std::string operatorId;

		if ( operatorMode && mySession_->getOperatorManager() )
		{
			/* For the query, if there is a logged in operator we pass her operatorId;
			otherwise, we pass an empty operatorId to indicate transactions not
			associated with a particular operator. */
			HobOperator* oper = mySession_->getOperatorManager()->getLoggedInOperator();
			if ( oper )
				operatorId = oper->getOpNum();
		}
		else
			operatorId = inputMessage->getOperatorID();

		//string deptNum = (!inputMessage->getPLUNumber().empty()) ? Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) : "";
		string deptNum = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		voidableTransactions_ = new VoidableTransactionsModule(this, transactionsRM, operatorMode, operatorId,
																deptNum, inputMessage->getPLUNumber(), inputMessage->getStartDateTime(), inputMessage->getEndDateTime());
	}


	/* This is the response message with the data desired (which may be empty). */
	Util::CSmartPtr<WriteTransactionGroupMessage> response;
	switch ( inputMessage->getReadingGroup() )
	{
		case ReadTransactionGroupMessage::GroupFirst:
		case ReadTransactionGroupMessage::GroupNext:
		{
			/* For first and next groups we move forward navigating the query. */
			response = voidableTransactions_->getNextTransactionGroup(inputMessage->getMaxQuantity());

			break;
		}
		case ReadTransactionGroupMessage::GroupPrevious:
		{
			/* For previous group we move backward navigating the query. */
			response = voidableTransactions_->getPreviousTransactionGroup(inputMessage->getMaxQuantity());

			break;
		}
		default: break;
	}

	/* Send the response to the User Interface. */
	SessionMessage outputMessage(TransactionDev, TransactionWriteNonVoidGroup);
	outputMessage.setFormatType(BrowserFormat);
	outputMessage.setHfcObject(response.get());

	mySession_->send(&outputMessage, Messenger::DEST_UI);
}


void TransactionManager::handleVoidTransactions(VoidTransactionsMessage* inputMessage)
{
	if ( !inputMessage || !mySession_ ) return;

	HobString id;
	HobOperator* oper = 0;
	HobList& ids = inputMessage->getTaskIdentifiers();

	if ( mySession_->getOperatorManager() )
		oper = mySession_->getOperatorManager()->getLoggedInOperator();

	if ( !oper && Configuration::getInstance()->isLoginRequired() )
	{
/* TODO:SJJC			waitingForLoginBeforePrinting = true;
		mySession->addDialogMessage(ErrOperatorDev, OperatorErrEnterIDorRFID, HfcResponse::RESPONSESTYLE_PROMPTOKCANCEL);
		return; */
	}

	while ( ids.next(id) != -1 )
	{
		Util::CSmartPtr<Transaction> transaction(findTransactionByIdentifier(id));
		if ( transaction.notNull() )
			voidTransaction(transaction, oper);
		else
			Debug::trace("TransactionManager.void", "Unable to find transaction with id " + id + " when trying to void it.");
	}

	/* Send the response to the User Interface. */
	voidableTransactions_->rebuildQuery();
	Util::CSmartPtr<WriteTransactionGroupMessage> response = voidableTransactions_->getNextTransactionGroup(voidableTransactions_->getQuantityRequested());

	SessionMessage outputMessage(TransactionDev, TransactionWriteNonVoidGroup);
	outputMessage.setFormatType(BrowserFormat);
	outputMessage.setHfcObject(response.get());

	mySession_->send(&outputMessage, Messenger::DEST_UI);
}


void TransactionManager::voidTransaction(Util::CSmartPtr<Transaction> transaction, HobOperator* oper)
{
	if ( transaction.null() )
		return;

	if ( transaction->getVoid() != Transaction::VoidNotVoided )
	{
		Debug::trace("TransactionManager.void", "The transaction to void cannot be voided (void flag = " +
				transaction->getVoidAsString() + ")");

		return;
	}

	//	Update the current ProductRunTotal here.
	mgrContainer_->getProductTotalManager()->updateAndDisplayProductRunTotal((Transaction*) transaction, true);

	Util::CSmartPtr<Transaction> voidingTransaction = createVoidingTransaction(transaction, oper);
	if ( voidingTransaction.notNull() )
	{
		handleSaveTransaction(voidingTransaction.get());
		transaction->setVoid(Transaction::VoidVoided);
		handleSaveTransaction(transaction.get());
		if ( voidableTransactions_.notNull() )
			voidableTransactions_->handleVoidedTransaction(transaction);
	}
}


void TransactionManager::handleReadDates()
{
	if ( !mySession_ ) return;

	int transactionDays = Configuration::getInstance()->toIntCfg(fieldOffsetCfgCollectingTransactionsDays);
	util_time::Date currentDate;                  // today's date
	TransactionDatesMessage response;


	/* For each day the application maintains transactions we need an option in the UI.
	Days 2 through 6 (day 0 is 'today' and day 1 is 'yesterday', which the UI displays
	automatically), if any, are sent to the UI as a numeric day of the week (Sunday = 0,
	Monday = 1, etc.), which the UI converts into a localized day of the week. Days 7
	through 15 (if any) are sent to the UI as a date (which gets displayed as is). For
	each of the days (from day 0 to the last one) we also send the corresponding date,
	which the UI uses to request data from the Application. */
	response.addDate(currentDate--);              // Add today's date (not displayed on the UI)
	response.addDate(currentDate--);              // Add yesterday's date (not displayed on the UI)
	for ( int i = 2; i <= 6 && i <= transactionDays; ++i, --currentDate )
	{
		response.addWeekDay(currentDate.getWeekDay());
		response.addDate(currentDate);
	}

	for ( int i = 7; i <= transactionDays; ++i, --currentDate )
	{
		response.addDateDisplay(HobDateTime::createDateTime(currentDate, FormatDate_YYYYMMDD).formatDate(Configuration::getInstance()->getDateDisplayFormat()));
		response.addDate(currentDate);
	}


	/* Send the response to the User Interface. */
	SessionMessage outputMessage(TransactionDev, TransactionWriteDates);
	outputMessage.setFormatType(BrowserFormat);
	outputMessage.setHfcObject(&response);

	mySession_->send(&outputMessage, Messenger::DEST_UI);
}


Util::CSmartPtr<Transaction> TransactionManager::createVoidingTransaction(Util::CSmartPtr<Transaction> transaction, HobOperator* oper)
{
	Util::CSmartPtr<Transaction> voidingTransaction(new Transaction(transaction->getTransactionTypeNumber()));

	// scaleId is assigned automatically
	voidingTransaction->setTransactionId(generateNextTransactionId());
	// typeNumber was assigned in the constructor
	// committedDateTime is assigned automatically when saving the transaction (it is committed then)
	voidingTransaction->setVoid(Transaction::VoidVoiding);
	voidingTransaction->setAffectsProductionFlag(transaction->getAffectsProductionFlag());
	// readClear is not assigned since the new transaction has not been read
	voidingTransaction->setNumberPackages(transaction->getNumberPackages());
	voidingTransaction->setPrintedSecurityLabel(transaction->isPrintedSecurityLabel());
	voidingTransaction->setCOOLTransaction(transaction->isCOOLTransaction());
	voidingTransaction->setTotalValue(transaction->getTotalValue());
	voidingTransaction->setOldTotalValue(transaction->getOldTotalValue());
	voidingTransaction->setDeptNum(transaction->getDeptNum());
	voidingTransaction->setProductNumber(transaction->getProductNumber());
	voidingTransaction->setReportingClass(transaction->getReportingClass());
	voidingTransaction->setProductType(transaction->getProductType());
	voidingTransaction->setProductionPlanningType(transaction->getProductionPlanningType());
	voidingTransaction->setProductDescription(transaction->getProductDescription());
	voidingTransaction->setPriceModifier(transaction->getPriceModifier());
	voidingTransaction->setBarcodePrefix(transaction->getBarcodePrefix());
	voidingTransaction->setBarcodeNumber(transaction->getBarcodeNumber());
	voidingTransaction->setUnitPrice(transaction->getUnitPrice());
	voidingTransaction->setExceptionPrice(transaction->getExceptionPrice());
	voidingTransaction->setFinalPrice(transaction->getFinalPrice());
	voidingTransaction->setTare(transaction->getTare());
	voidingTransaction->setProportionalTare(transaction->getProportionalTare());
	voidingTransaction->setFinalTare(transaction->getFinalTare());
	voidingTransaction->setByCount(transaction->getByCount());
	voidingTransaction->setFinalNetWeight(transaction->getFinalNetWeight());
	voidingTransaction->setFixedWeight(transaction->getFixedWeight());
	voidingTransaction->setShelfLifeDays(transaction->getShelfLifeDays());
	voidingTransaction->setShelfLifeHours(transaction->getShelfLifeHours());
	voidingTransaction->setShelfLifeMinutes(transaction->getShelfLifeMinutes());
	voidingTransaction->setProductLifeDays(transaction->getProductLifeDays());
	voidingTransaction->setProductLifeHours(transaction->getProductLifeHours());
	voidingTransaction->setProductLifeMinutes(transaction->getProductLifeMinutes());
	voidingTransaction->setLabel1(transaction->getLabel1());
	voidingTransaction->setLabel2(transaction->getLabel2());
	voidingTransaction->setLabel3(transaction->getLabel3());
	voidingTransaction->setLabel4(transaction->getLabel4());
	voidingTransaction->setGraphicName1(transaction->getGraphicName1());
	voidingTransaction->setGraphicName2(transaction->getGraphicName2());
	voidingTransaction->setGraphicName3(transaction->getGraphicName3());
	voidingTransaction->setGraphicName4(transaction->getGraphicName4());
	voidingTransaction->setCOOLText(transaction->getCOOLText());
	voidingTransaction->setCOOLTrackingText(transaction->getCOOLTrackingText());
	// old values are not assigned, need to look at the original transaction

	if ( oper )
	{
		voidingTransaction->setOperatorId(oper->getOpNum());
		voidingTransaction->setOperatorRFID(oper->getOpRFID());
		voidingTransaction->setOperatorName(oper->getOpName());
		// the void transaction does not change anything (no operator changes), simply voids the other transaction
	}

	return voidingTransaction;
}


Util::CSmartPtr<Transaction> TransactionManager::findTransactionByIdentifier(int id)
{
	return findTransactionByIdentifier(util_string::inttostring(id));
}


Util::CSmartPtr<Transaction> TransactionManager::findTransactionByIdentifier(const std::string & transId, const std::string & scaleId)
{
	Util::CSmartPtr<Transaction> retrievedTransaction;
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();

	if ( scaleId.length() == 0 || transId.length() == 0 || !transactionsRM )
		return retrievedTransaction;

	try
	{
		Util::CSmartPtr<edb::DBTransaction> dbTransactionRec(transactionsRM->findTransaction(scaleId, transId));
		if ( dbTransactionRec.notNull() )
		{
			retrievedTransaction = createTransaction(dbTransactionRec.get());
			if ( retrievedTransaction.notNull() )
			{
				if ( retrievedTransaction->getErrorCode() == RECORD_CORRUPT )
				{
					transactionsRM->deleteRecord(dbTransactionRec.get());
					retrievedTransaction = 0;
				}
				else
					Debug::trace("Transaction", "findTransactionByIdentifier() - found transaction record in the db");
			}
		}
	}
	catch ( DBException &dbe )
	{
		if ( Debug::isTracing("DBAccess.exception") )
		{
			Debug::trace("DBAccess.exception", "Transaction::findTransactionByIdentifier() ********** database exception **********");
			Debug::trace("DBAccess.exception", "     DBException: type:" +
					util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
		}

		retrievedTransaction = 0;
	}

	return retrievedTransaction;
}


int TransactionManager::initializeAllTransactionsQuery(bool countRecords)
{
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM )
		return 0;

	allTransactionsQuery_ = transactionsRM->retrieveAll();

	if ( countRecords )
		return (int)allTransactionsQuery_->size();
	else
		return 0;
}


Util::CSmartPtr<Transaction> TransactionManager::getFirstTransaction()
{
	if ( allTransactionsQuery_.null() && !initializeAllTransactionsQuery() )
		return Util::CSmartPtr<Transaction>(0);

	allTransactionsIterator_ = new DBQuery<DBTransaction>::iterator(allTransactionsQuery_->begin());

	return *allTransactionsIterator_ != allTransactionsQuery_->end() ? createTransaction(&*(*allTransactionsIterator_)) : Util::CSmartPtr<Transaction>(0);
}


Util::CSmartPtr<Transaction> TransactionManager::getNextTransaction()
{
	if ( allTransactionsQuery_.null() || allTransactionsIterator_.null() )
		return getFirstTransaction();

	++*allTransactionsIterator_;

	return *allTransactionsIterator_ != allTransactionsQuery_->end() ? createTransaction(&*(*allTransactionsIterator_)) : Util::CSmartPtr<Transaction>(0);
}


void TransactionManager::clearReadTransactions()
{
	initializeReadQuery(false);

	Util::CSmartPtr<Transaction> transaction = getFirstReadClearTransaction(false, true);
	while ( transaction.notNull() )
		transaction = getNextReadClearTransaction(false, true);
}


void TransactionManager::clearNotClearedTransactions()
{
	initializeNotClearedQuery(false);

	Util::CSmartPtr<Transaction> transaction = getFirstReadClearTransaction(false, true);
	while ( transaction.notNull() )
		transaction = getNextReadClearTransaction(false, true);
}


bool TransactionManager::initializeAllFilteredTransactionsQuery(const std::string& start, const std::string& end,
		const std::string& transType, Transaction::VoidFlag voidFlag, const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder)
{
	Debug::trace("TransactionManager", "initializeAllFilteredTransactionsQuery():  cutTestNumber = " + cutTestNumber);
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM )
		return 0;

	HobDateTime hdtStart;
	hdtStart.convertFromDBFormat(start);
	HobDateTime hdtEnd;
	hdtEnd.convertFromDBFormat(end);
	allFilteredTransactionsQuery_ = transactionsRM->retrieveFilteredTransactions(hdtStart.convertToDBFormat(), hdtEnd.convertToDBFormat(), transType, voidFlag, deptNum, pluNumber, cutTestNumber, operatorId, defaultOrder);

	return allFilteredTransactionsQuery_->begin() != allFilteredTransactionsQuery_->end();
}


Util::CSmartPtr<Transaction> TransactionManager::getFirstFilteredTransaction()
{
	if ( allFilteredTransactionsQuery_.null() /*&& !initializeAllFilteredTransactionsQuery() we don't have the parameters needed to call it*/ )
		return Util::CSmartPtr<Transaction>(0);

	allFilteredTransactionsNextIterator_ = new DBQuery<DBTransaction>::iterator(allFilteredTransactionsQuery_->begin());
	allFilteredTransactionsPreviousIterator_ = new DBQuery<DBTransaction>::iterator(allFilteredTransactionsQuery_->end());

	return *allFilteredTransactionsNextIterator_ != allFilteredTransactionsQuery_->end() ?
			createTransaction(&*(*allFilteredTransactionsNextIterator_)) : Util::CSmartPtr<Transaction>(0);
}


Util::CSmartPtr<Transaction> TransactionManager::getPreviousFilteredTransaction()
{
	if ( allFilteredTransactionsQuery_.null() || allFilteredTransactionsPreviousIterator_.null() ||
			*allFilteredTransactionsPreviousIterator_ == allFilteredTransactionsQuery_->begin() )
		return Util::CSmartPtr<Transaction>(0);

	--*allFilteredTransactionsPreviousIterator_;

	return *allFilteredTransactionsPreviousIterator_ != allFilteredTransactionsQuery_->end() ?
			createTransaction(&*(*allFilteredTransactionsPreviousIterator_)) : Util::CSmartPtr<Transaction>(0);
}


Util::CSmartPtr<Transaction> TransactionManager::getNextFilteredTransaction()
{
	if ( allFilteredTransactionsQuery_.null() || allFilteredTransactionsNextIterator_.null() ||
			*allFilteredTransactionsNextIterator_ == allFilteredTransactionsQuery_->end() )
		return Util::CSmartPtr<Transaction>(0);

	++*allFilteredTransactionsNextIterator_;

	return *allFilteredTransactionsNextIterator_ != allFilteredTransactionsQuery_->end() ?
			createTransaction(&*(*allFilteredTransactionsNextIterator_)) : Util::CSmartPtr<Transaction>(0);
}


void TransactionManager::resetPreviousFilteredTransaction()
{
	allFilteredTransactionsPreviousIterator_ = new DBQuery<DBTransaction>::iterator(allFilteredTransactionsNextIterator_);
}


void TransactionManager::resetNextFilteredTransaction()
{
	allFilteredTransactionsNextIterator_ = new DBQuery<DBTransaction>::iterator(allFilteredTransactionsPreviousIterator_);
}


int TransactionManager::initializeReadClearQuery(bool equal, Transaction::ReadClearFlag flag, bool countRecords, const std::string& deptNum)
{
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM ) return 0;

	notClearedTransactions_ = new NotClearedTransactionsModule(this, transactionsRM, equal, flag, deptNum);

	if ( countRecords )
		return notClearedTransactions_->size();
	else
		return !notClearedTransactions_->empty();
}


Util::CSmartPtr<Transaction> TransactionManager::getFirstReadClearTransaction(bool markRead, bool markCleared)
{
	if ( notClearedTransactions_.null() /* && !initializeNotClearedQuery() don't know what parameters to pass */ )
		return Util::CSmartPtr<Transaction>(0);

	edb::DBTransactionsRM* transactionsRM = 0;
	if ( markRead || markCleared )
	{
		transactionsRM = mgrContainer_->getDBTransactionsRM();
		if ( !transactionsRM ) return Util::CSmartPtr<Transaction>(0);
	}

	return notClearedTransactions_->getNextTransaction(transactionsRM, markRead, markCleared);
}


Util::CSmartPtr<Transaction> TransactionManager::getNextReadClearTransaction(bool markRead, bool markCleared)
{
	if ( notClearedTransactions_.null() /* && !initializeNotClearedQuery() don't know what parameters to pass */ )
		return Util::CSmartPtr<Transaction>(0);

	edb::DBTransactionsRM* transactionsRM = 0;
	if ( markRead || markCleared )
	{
		transactionsRM = mgrContainer_->getDBTransactionsRM();
		if ( !transactionsRM ) return Util::CSmartPtr<Transaction>(0);
	}

	return notClearedTransactions_->getNextTransaction(transactionsRM, markRead, markCleared);
}


/*	Util::CSmartPtr<Transaction> TransactionManager::createTransaction(int type, ::Product* prod)
{
	Util::CSmartPtr<Transaction> trans(new Transaction(type));

	if ( trans )
	{
		trans->setProductNumber(prod->getProductNum());
		trans->setReportingClass(prod->getReportingClassNum());
		trans->setProductType(prod->getProductType());
		trans->setProductionPlanningType(prod->getProductionPlanningType());
		trans->setProductDescription(prod->getDescription());
		trans->setPriceModifier(prod->getPriceModifier());
		trans->setBarcodePrefix(util_string::stringtoint(prod->getBarCodeNum().getPrefix()));
//			trans->setBarcodeNumber(prod->getBarCodeNum().getBarcodeString());
		trans->setUnitPrice(prod->getUnitPrice());
		trans->setExceptionPrice(prod->getExceptionPrice());
//			trans->setTotalPrice();
		trans->setTare(prod->getActiveTare().getValue());
		trans->setProportionalTare(prod->getProportionalTare());
//			trans->setFinalTare();
		if ( !(prod->isRandomWeight() || prod->isByCountWithWeight()) )
			trans->setFinalNetWeight(MetSig::BRConvertFixedWeightToTotalWeight(prod->getFixedWeight(), prod->isFluidOunce()));
		trans->setFixedWeight(prod->getFixedWeight());
		trans->setShelfLifeDays(prod->getShelfLifeDays());
		trans->setShelfLifeHours(prod->getShelfLifeHours());
		trans->setProductLifeDays(prod->getProductLifeDays());
		trans->setProductLifeHours(prod->getProductLifeHours());
		if ( prod->getLabelType(0) )
			trans->setLabel1(prod->getLabelType(0)->getLabelTypeNum());
		if ( prod->getLabelType(1) )
			trans->setLabel2(prod->getLabelType(1)->getLabelTypeNum());
		if ( prod->getLabelType(2) )
			trans->setLabel3(prod->getLabelType(2)->getLabelTypeNum());
		if ( prod->getLabelType(3) )
			trans->setLabel4(prod->getLabelType(3)->getLabelTypeNum());
		trans->setGraphicName1(prod->getGraphicName(0));
		trans->setGraphicName2(prod->getGraphicName(1));
		trans->setGraphicName3(prod->getGraphicName(2));
		trans->setGraphicName4(prod->getGraphicName(3));
		trans->setCOOLText(prod->getCOOLTextToPrint());
		trans->setCOOLTrackingText(prod->getCoolTrackingText());
	}

	return trans;
}
*/

Util::CSmartPtr<Transaction> TransactionManager::createTransaction(const edb::DBTransaction * dbTransactionRec)
{
	if ( dbTransactionRec )
	{
		Util::CSmartPtr<Transaction> transaction(new Transaction(dbTransactionRec->getTransactionType()));

		if ( dbTransactionRec->isRecordValid() )
		{
			transaction->setScaleId(util_string::stringtoint(dbTransactionRec->getScaleID()));
			transaction->setTransactionId(util_string::stringtoint(dbTransactionRec->getTransactionID()));
			if ( dbTransactionRec->isTransactionGroupNull() )
				transaction->setTransactionGroupNumber(RECORD_NOT_FOUND);
			else
				transaction->setTransactionGroupNumber(util_string::stringtoint(dbTransactionRec->getTransactionGroup()));
			transaction->setTransactionTypeNumber(dbTransactionRec->getTransactionType());
			transaction->setCommittedDateTime(dbTransactionRec->getTransactionDateTime());
			transaction->setVoid((Transaction::VoidFlag)dbTransactionRec->getTransactionVoid());
			transaction->setAffectsProductionFlag((TransactionType::AffectsProductionFlag)dbTransactionRec->getTransactionAffectsProduction());
			transaction->setReadClearFlag((Transaction::ReadClearFlag)dbTransactionRec->getTransactionReadCleared());
			transaction->setNumberPackages(dbTransactionRec->getTransactionPackages());
			transaction->setPrintedSecurityLabel(dbTransactionRec->getTransactionUsedSecurityLabel());
			transaction->setCOOLTransaction(dbTransactionRec->getTransactionCOOLFlag());
			transaction->setTotalValue(dbTransactionRec->getTransactionTotalValue());
			transaction->setOldTotalValue(dbTransactionRec->getTransactionOldTotalValue());

			transaction->setCutTestNumber(util_string::stringtolong(dbTransactionRec->getCutTestNumber()));
			Debug::trace("TransactionManager", "createTransaction():  CutTestNumber = " + longtostring(transaction->getCutTestNumber()));
			transaction->setPercentOfPrimalWeight(dbTransactionRec->getPercentOfPrimalWeight());
			transaction->setPercentOfPrimalCost(dbTransactionRec->getPercentOfPrimalCost());
			transaction->setModeChange(dbTransactionRec->getModeChange());

			if ( !dbTransactionRec->isProductNumberNull() )
			{
				transaction->setDeptNum(util_string::stringtolong(dbTransactionRec->getDeptNumber()));
				transaction->setProductNumber(util_string::stringtolong(dbTransactionRec->getProductNumber()));
				transaction->setReportingClass(util_string::stringtolong(dbTransactionRec->getReportingClass()));
				transaction->setProductType((Product::ProductType)dbTransactionRec->getProductType());
				transaction->setProductionPlanningType((Product::ProductionPlanningType)dbTransactionRec->getProductionPlanningType());
				transaction->setProductDescription(dbTransactionRec->getProductDescription());
				transaction->setPriceModifier(dbTransactionRec->getPriceFactor());
				transaction->setBarcodePrefix(dbTransactionRec->getBarCodePrefix());
				transaction->setBarcodeNumber(dbTransactionRec->getBarCodeNumber());
				transaction->setUnitPrice(dbTransactionRec->getUnitPrice());
				transaction->setExceptionPrice(dbTransactionRec->getExceptionPrice());
				transaction->setFinalPrice(dbTransactionRec->getFinalPrice());
				transaction->setTare(dbTransactionRec->getTare());
				transaction->setTareB(dbTransactionRec->getTare());
				transaction->setProportionalTare(dbTransactionRec->getProportionalTare());
				transaction->setFinalTare(dbTransactionRec->getFinalTare());
				transaction->setByCount(dbTransactionRec->getByCount());
				transaction->setFinalNetWeight(dbTransactionRec->getFinalNetWeight());
				transaction->setFixedWeight(dbTransactionRec->getFixedWeight());
				transaction->setShelfLifeDays(dbTransactionRec->getShelfLifeDays());
				transaction->setShelfLifeHours(dbTransactionRec->getShelfLifeHours());
				transaction->setShelfLifeMinutes(dbTransactionRec->getShelfLifeMinutes());
				transaction->setProductLifeDays(dbTransactionRec->getProductLifeDays());
				transaction->setProductLifeHours(dbTransactionRec->getProductLifeHours());
				transaction->setProductLifeMinutes(dbTransactionRec->getProductLifeMinutes());

				if ( dbTransactionRec->isLabel1Null() )
					transaction->setLabel1(RECORD_NOT_FOUND);
				else
					transaction->setLabel1(util_string::stringtolong(dbTransactionRec->getLabel1()));
				if ( dbTransactionRec->isLabel2Null() )
					transaction->setLabel2(RECORD_NOT_FOUND);
				else
					transaction->setLabel2(util_string::stringtolong(dbTransactionRec->getLabel2()));
				if ( dbTransactionRec->isLabel3Null() )
					transaction->setLabel3(RECORD_NOT_FOUND);
				else
					transaction->setLabel3(util_string::stringtolong(dbTransactionRec->getLabel3()));
				if ( dbTransactionRec->isLabel4Null() )
					transaction->setLabel4(RECORD_NOT_FOUND);
				else
					transaction->setLabel4(util_string::stringtolong(dbTransactionRec->getLabel4()));

				transaction->setGraphicName1(dbTransactionRec->getGraphic1());
				transaction->setGraphicName2(dbTransactionRec->getGraphic2());
				transaction->setGraphicName3(dbTransactionRec->getGraphic3());
				transaction->setGraphicName4(dbTransactionRec->getGraphic4());
				transaction->setCOOLText(dbTransactionRec->getCoolText());
				transaction->setCOOLTrackingText(dbTransactionRec->getCoolTrackingText());

				transaction->setOperatorChangeType((unsigned int)dbTransactionRec->getOperatorChangeType());
				if ( transaction->getOperatorChangeType() )
				{
					OperatorChange::OperatorChangeableField field = (OperatorChange::OperatorChangeableField)(OperatorChange::OC_Unknown + 1);
					while ( field < OperatorChange::end_OpChangeFieldType )
					{
						if ( transaction->isOperatorChangeType(field) )
						{
							switch ( field )
							{
								case OperatorChange::OC_UnitPrice:
									transaction->setOldUnitPrice(dbTransactionRec->getOldUnitPrice()); break;
								case OperatorChange::OC_Tare:
									transaction->setOldTare(dbTransactionRec->getOldTare()); break;
								case OperatorChange::OC_TareB:
									transaction->setOldTareB(dbTransactionRec->getOldTare()); break;
								case OperatorChange::OC_ByCount:
									transaction->setOldByCount(dbTransactionRec->getOldByCount()); break;
								case OperatorChange::OC_ShelfLifeDays:
									transaction->setOldShelfLifeDays(dbTransactionRec->getOldShelfLifeDays()); break;
								case OperatorChange::OC_ProductLifeDays:
									transaction->setOldProductLifeDays(dbTransactionRec->getOldProductLifeDays()); break;
								case OperatorChange::OC_PrimaryLabel:
									transaction->setOldLabel1(util_string::stringtolong(dbTransactionRec->getOldLabel1())); break;
								case OperatorChange::OC_SecondLabel:
									transaction->setOldLabel2(util_string::stringtolong(dbTransactionRec->getOldLabel2())); break;
								case OperatorChange::OC_FixedWeight:
									transaction->setOldFixedWeight(dbTransactionRec->getOldFixedWeight()); break;
								case OperatorChange::OC_UpcDigit:
									transaction->setOldBarcodeDigit(dbTransactionRec->getOldBarCodeDigit()); break;
								case OperatorChange::OC_TotalPrice:
									transaction->setOldFinalPrice(dbTransactionRec->getOldFinalPrice()); break;
								case OperatorChange::OC_Graphic1:
									transaction->setOldGraphicName1(dbTransactionRec->getOldGraphic1()); break;
								case OperatorChange::OC_Graphic2:
									transaction->setOldGraphicName2(dbTransactionRec->getOldGraphic2()); break;
								case OperatorChange::OC_Graphic3:
									transaction->setOldGraphicName3(dbTransactionRec->getOldGraphic3()); break;
								case OperatorChange::OC_Graphic4:
									transaction->setOldGraphicName4(dbTransactionRec->getOldGraphic4()); break;
								case OperatorChange::OC_ThirdLabel:
									transaction->setOldLabel3(util_string::stringtolong(dbTransactionRec->getOldLabel3())); break;
								case OperatorChange::OC_ShelfLifeHours:
									transaction->setOldShelfLifeHours(dbTransactionRec->getOldShelfLifeHours()); break;
								case OperatorChange::OC_ProductLifeHours:
									transaction->setOldProductLifeHours(dbTransactionRec->getOldProductLifeHours()); break;
								// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet
								//case OperatorChange::OC_ShelfLifeMinutes:
								//	transaction->setOldShelfLifeMinutes(dbTransactionRec->getOldShelfLifeMinutes()); break;
								//case OperatorChange::OC_ProductLifeMinutes:
								//	transaction->setOldProductLifeMinutes(dbTransactionRec->getOldProductLifeMinutes()); break;
								default:
									break;
							}
						}

						field = (OperatorChange::OperatorChangeableField)(field + 1);
					}
				}
			}

			if ( !dbTransactionRec->isOperatorIDNull() )
			{
				transaction->setOperatorId(dbTransactionRec->getOperatorID());
				transaction->setOperatorRFID(dbTransactionRec->getOperatorRFID());
				transaction->setOperatorName(dbTransactionRec->getOperatorName());
				transaction->setOperatorChangeType(dbTransactionRec->getOperatorChangeType());
			}

			transaction->setErrorCode(RECORD_OK);

			Debug::trace("TransactionManager", "Read Transaction db record OK");
		}
		else
		{
			transaction->setErrorCode(RECORD_CORRUPT);

			Debug::trace("TransactionManager", "Read Transaction db record CORRUPT!!");
		}

		return transaction;
	}
	else
		Debug::trace("TransactionManager", "Constructor passing in a db - dbrec IS NULL");

	return Util::CSmartPtr<Transaction>(0);
}

void TransactionManager::handleSaveTransaction(Transaction* transaction)
{
	Debug::trace("TransactionManager", "handleSaveTransaction(): recordToSave = " + ptrtostring(transaction));

	if (transaction)
	{
		// SOF-3561
		// generate transaction id when none assigned so it can be syncronized properly
		// otherwise we get two records... one from the initial save and a second from syncronization batch application
		if ( RECORD_NOT_FOUND == transaction->getTransactionId() )
		{
			Debug::trace("TransactionManager", "handleSaveTransaction(): no transaction id... generating one...");
			transaction->setTransactionId(generateNextTransactionId());
		}

		saveTransaction(transaction);

		// Usually, the following if condition will be like
		//     if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone &&
		// however, it is changed here, in a temporary fashion, to address the problem where the Primary scale puts its own
		// transaction records into a batch that is later synchronized to secondaries, which reject those batches due to
		// the fact that those transaction records are not sent and so the batch's signature does not match what the Secondary
		// scale receives.
		// The longer term fix would be to have mgrContainer_ define a 'isConsolidatedOnly' attribute for each type of record
		// and if the scale is a primary scale then check that attribute, something like
		//     mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Transactions) &&
		//     (Configuration::getInstance()->getScaleSynchronizationType() == ScaleSynchronizationTypeSecondary ||
		//         !mgrContainer_->isConsolidatedOnly(hfc::HFCManagerContainer::Transactions)) )
		// Then, this new style should be used in all HFC Managers, although at this point Transactions is the only one that
		// is consolidated only.
		if ( Configuration::getInstance()->getScaleSynchronizationType() == ScaleSynchronizationTypeSecondary &&
			mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Transactions))
		{
			std::string synchSer = "";
			serializeForSynchronization(synchSer, transaction);

			Debug::trace("TransactionManager", std::string("sync transaction data: ") + synchSer);

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
		}

	}
}

void TransactionManager::saveTransaction(Transaction* transaction)
{
	Util::CSmartPtr<DBTransaction> dbTransaction;
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM || !transaction )
		return;


	if ( transaction->getTransactionId() != RECORD_NOT_FOUND )
	{
		dbTransaction = transactionsRM->findTransaction(util_string::longtostring(transaction->getScaleId()),
				util_string::longtostring(transaction->getTransactionId()));
	}

	if ( dbTransaction.null() )
	{
		//	This must be a new record for the Database
		Debug::trace("TransactionManager", "saveRecord() - don't have a db record, creating it...");

		if ( transaction->getTransactionId() != RECORD_NOT_FOUND )
			dbTransaction = transactionsRM->createRecord(util_string::longtostring(transaction->getScaleId()),
					util_string::longtostring(transaction->getTransactionId()));
		else
			dbTransaction = transactionsRM->createRecord(util_string::longtostring(transaction->getScaleId()));
	}
	
	if ( dbTransaction.notNull() )
	{
//			Debug::trace("TransactionManager", "Going to set data elements");

		/* We always have transaction and transaction ids because the first time a
		record is created a transaction id is assigned to it and when the record
		is first saved a transaction id is assigned to it. */
		if ( transaction->getTransactionGroupNumber() != RECORD_NOT_FOUND )
			dbTransaction->setTransactionGroup(util_string::inttostring(transaction->getTransactionGroupNumber()));
		else
			dbTransaction->setTransactionGroupNull();
		/* We always have a transaction type because when a transaction is
		created for the first time a type is supplied. */
		dbTransaction->setTransactionType(transaction->getTransactionTypeNumber());
		/* We don't want to have a transaction in the db without a committed
		date/time or we may never be able to dispose of it. */
		if ( !transaction->isTransactionCommitted() )
			transaction->setCommittedDateTime();
		dbTransaction->setTransactionDateTime(transaction->getCommittedDateTimeAsString());
		dbTransaction->setTransactionVoid(transaction->getVoid());
		dbTransaction->setTransactionAffectsProduction(transaction->getAffectsProductionFlag());
		dbTransaction->setTransactionReadCleared(transaction->getReadClearFlag());
		dbTransaction->setTransactionPackages(transaction->getNumberPackages());
		dbTransaction->setTransactionUsedSecurityLabel(transaction->isPrintedSecurityLabel());
		dbTransaction->setTransactionCOOLFlag(transaction->isCOOLTransaction());
		dbTransaction->setTransactionTotalValue(transaction->getTotalValue());
		dbTransaction->setTransactionOldTotalValue(transaction->getOldTotalValue());

		dbTransaction->setCutTestNumber(util_string::longtostring(transaction->getCutTestNumber()));
		Debug::trace("TransactionManager", "saveTransaction():  CutTestNumber = " + dbTransaction->getCutTestNumber());
		dbTransaction->setPercentOfPrimalWeight(transaction->getPercentOfPrimalWeight());
		dbTransaction->setPercentOfPrimalCost(transaction->getPercentOfPrimalCost());
		dbTransaction->setModeChange(transaction->getModeChange());

		if ( transaction->getProductNumber() != RECORD_NOT_FOUND )
		{
			dbTransaction->setDeptNumber(util_string::longtostring(transaction->getDeptNum()));
			dbTransaction->setProductNumber(util_string::longtostring(transaction->getProductNumber()));
			dbTransaction->setReportingClass(util_string::longtostring(transaction->getReportingClass()));
			dbTransaction->setProductType(transaction->getProductType());
			dbTransaction->setProductionPlanningType(transaction->getProductionPlanningType());
			if ( transaction->getProductDescription().size() )
			{
				// Deal with a possibly too long description. Currently the db can store 100 chars
				if ( transaction->getProductDescription().size() > 100 )
				{
					std::string shortString = transaction->getProductDescription().substr(0, 100);
					dbTransaction->setProductDescription(shortString);
				}
				else
					dbTransaction->setProductDescription(transaction->getProductDescription());
			}
			else
				dbTransaction->setProductDescriptionNull();
			dbTransaction->setPriceFactor(transaction->getPriceModifier());
			dbTransaction->setBarCodePrefix(transaction->getBarcodePrefix());
			if ( transaction->getBarcodeNumber().size() )
			{
				// Deal with a possibly too long barcode. Currently the db can store 40 chars
				if ( transaction->getBarcodeNumber().size() > 40 )
				{
					std::string shortString = transaction->getBarcodeNumber().substr(0, 40);
					dbTransaction->setBarCodeNumber(shortString);
				}
				else
					dbTransaction->setBarCodeNumber(transaction->getBarcodeNumber());
			}
			else
				dbTransaction->setBarCodeNumberNull();
			dbTransaction->setUnitPrice(transaction->getUnitPrice());
			if ( transaction->getExceptionPrice() )
				dbTransaction->setExceptionPrice(transaction->getExceptionPrice());
			else
				dbTransaction->setExceptionPriceNull();
			if ( transaction->getFinalPrice() )
				dbTransaction->setFinalPrice(transaction->getFinalPrice());
			else
				dbTransaction->setFinalPriceNull();
			
			int opChange = transaction->getOperatorChangeType();
			if ( ( transaction->getTare() )      && ( transaction->isOperatorChangeType(hfc::OperatorChange::OC_Tare ) ) )
				dbTransaction->setTare( transaction->getTare() );			// otherwise we may have a tareB. What if multiples?
			else if ( (transaction->getTareB() ) && ( transaction->isOperatorChangeType(hfc::OperatorChange::OC_TareB ) ) )
				dbTransaction->setTare( transaction->getTareB() );		// otherwise we may have a tareB. What if multiples?
			else
				dbTransaction->setTareNull();							// set it to null now, but we might change it if tareB

			if ( transaction->getProportionalTare() )
				dbTransaction->setProportionalTare(transaction->getProportionalTare());
			else
				dbTransaction->setProportionalTareNull();
			if ( transaction->getFinalTare() )
				dbTransaction->setFinalTare(transaction->getFinalTare());
			else
				dbTransaction->setFinalTareNull();

			// bool isbcforced = transaction->getIsByCountForced();		// jhc 1
			bool isbcforced = transaction->getByCountForced();			// jhc 2
			if( isbcforced )
				dbTransaction->setByCount(0);							// jhc I need to do this bc if we set a plu as forced the bc value is 1 in the db in the trans and prod record 
			else
				dbTransaction->setByCount(transaction->getByCount());

			if ( transaction->getFinalNetWeight() )
				dbTransaction->setFinalNetWeight(transaction->getFinalNetWeight());
			else
				dbTransaction->setFinalNetWeightNull();

			if ( transaction->getSweetheartWeight() )
				dbTransaction->setSweetheartWeight( transaction->getSweetheartWeight() );
			else
				dbTransaction->setSweetheartWeightNull();

			if ( transaction->getFixedWeight() )
				dbTransaction->setFixedWeight(transaction->getFixedWeight());
			else
				dbTransaction->setFixedWeightNull();

			dbTransaction->setShelfLifeDays(transaction->getShelfLifeDays());
			dbTransaction->setShelfLifeHours(transaction->getShelfLifeHours());
			dbTransaction->setShelfLifeMinutes(transaction->getShelfLifeMinutes());
			dbTransaction->setProductLifeDays(transaction->getProductLifeDays());
			dbTransaction->setProductLifeHours(transaction->getProductLifeHours());
			dbTransaction->setProductLifeMinutes(transaction->getProductLifeMinutes());

			if ( transaction->getLabel1() != RECORD_NOT_FOUND )
				dbTransaction->setLabel1(util_string::longtostring(transaction->getLabel1()));
			else
				dbTransaction->setLabel1Null();
			if ( transaction->getLabel2() != RECORD_NOT_FOUND )
				dbTransaction->setLabel2(util_string::longtostring(transaction->getLabel2()));
			else
				dbTransaction->setLabel2Null();
			if ( transaction->getLabel3() != RECORD_NOT_FOUND )
				dbTransaction->setLabel3(util_string::longtostring(transaction->getLabel3()));
			else
				dbTransaction->setLabel3Null();
			if ( transaction->getLabel4() != RECORD_NOT_FOUND )
				dbTransaction->setLabel4(util_string::longtostring(transaction->getLabel4()));
			else
				dbTransaction->setLabel4Null();

			if ( transaction->getGraphicName1().size() )
				dbTransaction->setGraphic1(transaction->getGraphicName1());
			else
				dbTransaction->setGraphic1Null();
			if ( transaction->getGraphicName2().size() )
				dbTransaction->setGraphic2(transaction->getGraphicName2());
			else
				dbTransaction->setGraphic2Null();
			if ( transaction->getGraphicName3().size() )
				dbTransaction->setGraphic3(transaction->getGraphicName3());
			else
				dbTransaction->setGraphic3Null();
			if ( transaction->getGraphicName4().size() )
				dbTransaction->setGraphic4(transaction->getGraphicName4());
			else
				dbTransaction->setGraphic4Null();
			if ( transaction->getCOOLText().size() )
				dbTransaction->setCoolText(transaction->getCOOLText());
			else
				dbTransaction->setCoolTextNull();
			if ( transaction->getCOOLTrackingText().size() )
				dbTransaction->setCoolTrackingText(transaction->getCOOLTrackingText());
			else
				dbTransaction->setCoolTrackingTextNull();

			dbTransaction->setOperatorChangeType(transaction->getOperatorChangeType());
			if ( transaction->getOperatorChangeType() )
			{
				OperatorChange::OperatorChangeableField field = (OperatorChange::OperatorChangeableField)(OperatorChange::OC_Unknown + 1);
				while ( field < OperatorChange::end_OpChangeFieldType )
				{
					if ( transaction->isOperatorChangeType(field) )
					{
						switch ( field )
						{
							case OperatorChange::OC_UnitPrice:
								dbTransaction->setOldUnitPrice(transaction->getOldUnitPrice()); break;
							case OperatorChange::OC_Tare:
								dbTransaction->setOldTare(transaction->getOldTare()); break;
							case OperatorChange::OC_TareB:
								dbTransaction->setOldTare(transaction->getOldTareB()); break;
							case OperatorChange::OC_ByCount:
								dbTransaction->setOldByCount(transaction->getOldByCount()); break;
							case OperatorChange::OC_ShelfLifeDays:
								dbTransaction->setOldShelfLifeDays(transaction->getOldShelfLifeDays()); break;
							case OperatorChange::OC_ProductLifeDays:
								dbTransaction->setOldProductLifeDays(transaction->getOldProductLifeDays()); break;
							case OperatorChange::OC_PrimaryLabel:
								dbTransaction->setOldLabel1(util_string::longtostring(transaction->getOldLabel1())); break;
							case OperatorChange::OC_SecondLabel:
								dbTransaction->setOldLabel2(util_string::longtostring(transaction->getOldLabel2())); break;
							case OperatorChange::OC_FixedWeight:
								dbTransaction->setOldFixedWeight(transaction->getOldFixedWeight()); break;
							case OperatorChange::OC_UpcDigit:
								dbTransaction->setOldBarCodeDigit(transaction->getOldBarcodeDigit()); break;
							case OperatorChange::OC_TotalPrice:
								dbTransaction->setOldFinalPrice(transaction->getOldFinalPrice()); break;
							case OperatorChange::OC_Graphic1:
								dbTransaction->setOldGraphic1(transaction->getOldGraphicName1()); break;
							case OperatorChange::OC_Graphic2:
								dbTransaction->setOldGraphic2(transaction->getOldGraphicName2()); break;
							case OperatorChange::OC_Graphic3:
								dbTransaction->setOldGraphic3(transaction->getOldGraphicName3()); break;
							case OperatorChange::OC_Graphic4:
								dbTransaction->setOldGraphic4(transaction->getOldGraphicName4()); break;
							case OperatorChange::OC_ThirdLabel:
								dbTransaction->setOldLabel3(util_string::longtostring(transaction->getOldLabel3())); break;
							case OperatorChange::OC_ShelfLifeHours:
								dbTransaction->setOldShelfLifeHours(transaction->getOldShelfLifeHours()); break;
							case OperatorChange::OC_ProductLifeHours:
								dbTransaction->setOldProductLifeHours(transaction->getOldProductLifeHours()); break;
							// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet
							//case OperatorChange::OC_ShelfLifeMinutes:
							//	dbTransaction->setOldShelfLifeMinutes(transaction->getOldShelfLifeMinutes()); break;
							//case OperatorChange::OC_ProductLifeMinutes:
							//	dbTransaction->setOldProductLifeMinutes(transaction->getOldProductLifeMinutes()); break;
							default: break;
						}
					}

					field = (OperatorChange::OperatorChangeableField)(field + 1);
				}
			}
		}
		else
		{
			dbTransaction->setDeptNumberNull();
			dbTransaction->setProductNumberNull();
			dbTransaction->setReportingClassNull();
			dbTransaction->setProductTypeNull();
			dbTransaction->setProductionPlanningTypeNull();
			dbTransaction->setProductDescriptionNull();
			dbTransaction->setPriceFactorNull();
			dbTransaction->setBarCodePrefixNull();
			dbTransaction->setBarCodeNumberNull();
			dbTransaction->setUnitPriceNull();
			dbTransaction->setExceptionPriceNull();
			dbTransaction->setFinalPriceNull();
			dbTransaction->setTareNull();
			dbTransaction->setProportionalTareNull();
			dbTransaction->setFinalTareNull();
			dbTransaction->setFinalNetWeightNull();
			dbTransaction->setFixedWeightNull();
			dbTransaction->setShelfLifeDaysNull();
			dbTransaction->setShelfLifeHoursNull();
			dbTransaction->setShelfLifeMinutesNull();
			dbTransaction->setProductLifeDaysNull();
			dbTransaction->setProductLifeHoursNull();
			dbTransaction->setProductLifeMinutesNull();
			dbTransaction->setLabel1Null();
			dbTransaction->setLabel2Null();
			dbTransaction->setLabel3Null();
			dbTransaction->setLabel4Null();
			dbTransaction->setGraphic1Null();
			dbTransaction->setGraphic2Null();
			dbTransaction->setGraphic3Null();
			dbTransaction->setGraphic4Null();
			dbTransaction->setCoolTextNull();
			dbTransaction->setCoolTrackingTextNull();
		}

		//Debug::trace("TransactionManager", "transaction->getOperatorId(): " + transaction->getOperatorId() );
		if ( transaction->getOperatorId().size() )
		{
			//Debug::trace("TransactionManager", "dbTransaction->getOperatorId() before setting operator: " + dbTransaction->getOperatorID());
			dbTransaction->setOperatorID(transaction->getOperatorId());
			Debug::trace("TransactionManager", "dbTransaction->getOperatorId() after setting operator: " + dbTransaction->getOperatorID());

			if ( transaction->getOperatorRFID().size() )
				dbTransaction->setOperatorRFID(transaction->getOperatorRFID());
			else
				dbTransaction->setOperatorRFIDNull();

			Debug::trace("TransactionManager", "transaction->getOperatorName() " + transaction->getOperatorName());
			//Debug::trace("TransactionManager", "transaction->getOperatorName().size(): " + util_string::inttostring(transaction->getOperatorName().size()));

			if ( transaction->getOperatorName().size() )
			{
				// Deal with a possibly too long operator name. Currently the db can store 40 chars
				if ( transaction->getOperatorName().size() > 40 )
				{
					std::string shortString = transaction->getOperatorName().substr(0, 40);
					//Debug::trace("TransactionManager", "dbTransaction->getOperatorName() if before > 40" + dbTransaction->getOperatorName());
					dbTransaction->setOperatorName(shortString);
					Debug::trace("TransactionManager", "dbTransaction->getOperatorName() if after > 40" + dbTransaction->getOperatorName());
				}
				else
				{
					//Debug::trace("TransactionManager", "ELSE dbTransaction->getOperatorName() else before > 40" + dbTransaction->getOperatorName());
					dbTransaction->setOperatorName(transaction->getOperatorName());
					Debug::trace("TransactionManager", "dbTransaction->getOperatorName() else after > 40 " + dbTransaction->getOperatorName());
				}
			}
			else
			{
				Debug::trace("TransactionManager", "ELSE SET dbTransaction.OperatorName to NULL " );
				dbTransaction->setOperatorNameNull();
		    }
		}
		else
		{
			Debug::trace("TransactionManager", "ELSE SET ALL of dbTransaction to NULL ");

			dbTransaction->setOperatorIDNull();
			dbTransaction->setOperatorRFIDNull();
			dbTransaction->setOperatorNameNull();
		}

		Debug::trace("TransactionManager", "Saving Transaction (" + util_string::longtostring(transaction->getScaleId()) +
				", " + util_string::longtostring(transaction->getTransactionId()) + ")");

		try
		{
			transactionsRM->saveRecord(dbTransaction.get());

		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "Transaction::saveRecord() ********** database exception **********");
			Debug::trace("DBAccess.exception", "     DBException: type:" +
					util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
		}

//			Debug::trace("TransactionManager", "Saved in the database");
	}
}


int TransactionManager::deleteTransaction(Transaction* transaction)
{
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transaction || !transactionsRM )
		return 0;

	Debug::trace("TransactionManager", "Going to delete transaction (" + util_string::longtostring(transaction->getScaleId()) +
		", " + util_string::longtostring(transaction->getTransactionId()) + ")");

	Util::CSmartPtr<DBTransaction> dbTransaction(transactionsRM->findTransaction(util_string::longtostring(transaction->getTransactionId()),
			util_string::longtostring(transaction->getScaleId())));
	
	if ( dbTransaction.notNull() )   //No need to delete if there is no record
		return transactionsRM->deleteRecord(dbTransaction.get());

	Debug::trace("TransactionManager", "Could not find transaction in database");

	return 0;
}


int TransactionManager::deleteAllTransactions(HobRecordNum deptNum)
{
	edb::DBTransactionsRM* dbRM = mgrContainer_->getDBTransactionsRM();
	if ( !dbRM)
		return 0;

	Debug::trace("TransactionManager", "Going to delete all transactions");

	if (deptNum == RECORD_NOT_FOUND)
		return dbRM->deleteAll();
	else
		return dbRM->deleteAllForDept(longtostring(deptNum));

}


void TransactionManager::purgeOldTransactions(int daysToKeepTransactions, MessageLogger::LogFileIndicator logToFile)
{
	util_time::Date cutOffDate;
	cutOffDate -= daysToKeepTransactions;
	util_time::DateTime cutOffDateTime(cutOffDate), transDT;
	string logMessage = "";

	Debug::trace("TransactionManager", "purgeOldTransactions(" + util_string::inttostring(daysToKeepTransactions) +
			") cut-off date is " + cutOffDateTime.operator std::string());

	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Time to purge old Transaction records.");
	try
	{
		edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
		if ( !transactionsRM )
		{
			MessageLogger::logMessage(logToFile, "TransactionManager.purgeOldTransactions(): no transaction database found");
			return;
		}
		Util::CSmartPtr<edb::DBTransactionsRM::query_type> query(transactionsRM->retrieveAll());
		if ( query.null() )
		{
			MessageLogger::logMessage(logToFile, "TransactionManager.purgeOldTransactions(): no transaction records found");
			return;
		}
		edb::DBTransactionsRM::query_type::iterator iterator = query->begin();
		edb::DBTransactionsRM::query_type::iterator endIterator = query->end();
		while ( iterator != endIterator )
		{
			// I've seen a couple of times where the transaction stored in the db has empty date as in
			// '  /  /    ,14:21:41'. This becomes an invalid date as we process it and it causes an
			// exception to be thrown. Until we find the problem the following ugly code will be here.
			try { transDT = util_string::stringdbtodatetime(iterator->getTransactionDateTime()); }
			catch ( const util_time::Date::InvalidDate& )
			{
				Debug::trace("TransactionManager", "purgeOldTransactions() Got an invalid date exception");

				// Skip the record so we can purge the rest of the transactions.
				++iterator;
				continue;
			}

			if ( transDT < cutOffDateTime )
			{
				Util::CSmartPtr<edb::DBTransaction> oldTransaction(new edb::DBTransaction(*iterator));

				++iterator;

				if ( !transactionsRM->deleteRecord(oldTransaction.get()) )
				{
					Debug::trace("TransactionManager", "purgeOldTransactions(): Tried but failed to delete transaction");
					if (logToFile != MessageLogger::endLogFileIndicator)
					{
						MessageLogger::logMessage(logToFile,
							 "TransactionManager.purgeOldTransactions(): Tried but failed to delete transaction " + oldTransaction->getTransactionID());
					}		
					Util::CSmartPtr<hfc::Transaction> trans = createTransaction(oldTransaction.get());
					trans->show();
				}
				else
					Debug::trace("TransactionManager", "purgeOldTransactions(): purged transaction (" +
							oldTransaction->getScaleID() + ", " + oldTransaction->getTransactionID() + ")");
				
				if (logToFile != MessageLogger::endLogFileIndicator)
				{
					MessageLogger::logMessage(logToFile,
						"TransactionManager.purgeOldTransactions(): purged transaction" + oldTransaction->getScaleID() + oldTransaction->getTransactionID());
				}
			}
			else
				break;
		}
	}
	catch (DBException &dbe)
	{
		if ( Debug::isTracing("DBAccess.exception") )
		{
			Debug::trace("DBAccess.exception", "TransactionManager::purgeOldTransactions() ********** database exception **********");
			Debug::trace("DBAccess.exception", "     DBException: type: " + util_string::inttostring(dbe.getType()) + "  info: " + dbe.info());
		}
		else
		{
			Debug::trace("TransactionManager", "purgeOldTransactions() *************** database exception **************");
			Debug::trace("TransactionManager", "     DBException: type: " + util_string::inttostring(dbe.getType()) + "  info: " + dbe.info());
		}
		if (logToFile != MessageLogger::endLogFileIndicator)
		{
			logMessage = "TransactionManager.purgeOldTransactions() *************** database exception\n DBException: type: " + util_string::inttostring(dbe.getType()) + "  info: " + dbe.info();
			MessageLogger::logMessage(logToFile, logMessage);
		}
	}
	MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, "Finished purging old Transaction records.");
}


void TransactionManager::destroyQueriesAndIters()
{
	/* When this function is called, coming out of a db compress, it is safe to remove all
	queries and iterators. */

	if ( notClearedTransactions_.notNull() )
		notClearedTransactions_->destroyQueriesAndIters();

	if ( voidableTransactions_.notNull() )
		voidableTransactions_->destroyQueriesAndIters();

	/* Destroy first the iterators. */
	allTransactionsIterator_ = 0; allFilteredTransactionsPreviousIterator_ = 0; allFilteredTransactionsNextIterator_ = 0;

	/* Destroy now the queries. */
	allTransactionsQuery_ = 0; allFilteredTransactionsQuery_ = 0;
}


HobRecordNum TransactionManager::generateNextTransactionId()
{
	std::string transactionId;
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();

	if ( transactionsRM )
	{
		transactionId = transactionsRM->getNextTransactionId();

		Debug::trace("TransactionManager", "generateNextTransactionId(): New transaction Id generated: " + transactionId);

		return util_string::stringtolong(transactionId);
	}
	else
	{
		Debug::trace("TransactionManager", "generateNextTransactionId(): Failed to generate new transaction Id!!!");

		return RECORD_NOT_FOUND;
	}
}


TransactionManager::NotClearedTransactionsModule::NotClearedTransactionsModule(TransactionManager* manager, edb::DBTransactionsRM* transactionsRM,
	bool equal, Transaction::ReadClearFlag flag, const std::string& deptNum)
	: manager_(manager)
{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif

	query_ = transactionsRM->retrieveReadAndClear(equal, flag, deptNum);
	if ( query_.notNull() )
		iterator_ = new DBQuery<DBTransaction>::iterator(query_->begin());
}


TransactionManager::NotClearedTransactionsModule::~NotClearedTransactionsModule()
{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
}


Util::CSmartPtr<Transaction> TransactionManager::NotClearedTransactionsModule::getNextTransaction(edb::DBTransactionsRM* transactionsRM, bool markRead, bool markCleared)
{
	bool transactionOk = false;
	Util::CSmartPtr<Transaction> transaction;

	if ( query_.null() || iterator_.null() || *iterator_ == query_->end() )
		return transaction;


	do
	{
		transaction = manager_->createTransaction(&*(*iterator_));
		transactionOk = transaction.notNull() && transaction->isRecordValid();

		if ( transactionOk && transactionsRM )
		{
			/* Mark the transaction as read. */
			if ( markRead )
				(*iterator_)->setTransactionReadCleared(Transaction::ReadClearRead);
			if ( markCleared )
				(*iterator_)->setTransactionReadCleared(Transaction::ReadClearCleared);
			if ( markRead || markCleared )
				transactionsRM->saveRecord(&*(*iterator_));
		}

		++*iterator_;
	} while ( !transactionOk && *iterator_ != query_->end() );


	return transactionOk ? transaction : Util::CSmartPtr<Transaction>(0);
}


void TransactionManager::NotClearedTransactionsModule::destroyQueriesAndIters()
{
	/* Destroy first the iterators. */
	iterator_ = 0;

	/* Destroy now the queries. */
	query_ = 0;
}


TransactionManager::VoidableTransactionsModule::VoidableTransactionsModule(TransactionManager* manager,
	edb::DBTransactionsRM* transactionsRM, bool operatorMode, const std::string& operatorId,
	const std::string& deptNum, const std::string& pluNumber, const std::string& start, const std::string& end)
	: manager_(manager)
{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif

	transactionsRM_ = transactionsRM;
	operatorMode_ = operatorMode;
	operatorId_ = operatorId;
	deptNum_ = deptNum;
	pluNumber_ = pluNumber;

	hdtStart_.convertFromDBFormat(start);
	hdtEnd_.convertFromDBFormat(end);

	rebuildQuery();
}

TransactionManager::VoidableTransactionsModule::~VoidableTransactionsModule()
{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
}


void TransactionManager::VoidableTransactionsModule::rebuildQuery()
{
	destroyQueriesAndIters();

	query_ = operatorMode_ ? transactionsRM_->retrieveOperatorVoidable(operatorId_, Transaction::VoidNotVoided, deptNum_) :
		transactionsRM_->retrieveSupervisorVoidable(operatorId_, deptNum_, pluNumber_, hdtStart_.convertToDBFormat(), hdtEnd_.convertToDBFormat(), Transaction::VoidNotVoided);

	previousIterator_ = new DBQuery<DBTransaction>::iterator(query_->end());
	nextIterator_ = new DBQuery<DBTransaction>::iterator(query_->begin());
}


Util::CSmartPtr<WriteTransactionGroupMessage> TransactionManager::VoidableTransactionsModule::getPreviousTransactionGroup(int quantity)
{
	setQuantityRequested(quantity);

	if ( quantityRequested_ <= 0 ) return Util::CSmartPtr<WriteTransactionGroupMessage>(0);

	edb::DBQuery<edb::DBTransaction>::iterator tempIterator_ = *previousIterator_;
	Util::CSmartPtr<WriteTransactionGroupMessage> group(new WriteTransactionGroupMessage(false));

	for ( int i = 0; i < quantityRequested_; ++i )
	{
		Util::CSmartPtr<Transaction> transaction = getPreviousTransaction();
		if ( transaction.notNull() )
			group->addTransaction(transaction);
		else break;
	}

	if ( tempIterator_ != query_->end() )
		nextIterator_ = new DBQuery<DBTransaction>::iterator(++tempIterator_);
	else
		nextIterator_ = new DBQuery<DBTransaction>::iterator(query_->begin());

	group->setPreviousGroupAvailable(*previousIterator_ != query_->end());
	group->setNextGroupAvailable(*nextIterator_ != query_->end());

	return group;
}


Util::CSmartPtr<WriteTransactionGroupMessage> TransactionManager::VoidableTransactionsModule::getSameTransactionGroup()
{
	if ( *previousIterator_ != query_->end() )
	{
		nextIterator_ = new DBQuery<DBTransaction>::iterator(*previousIterator_);
		++*nextIterator_;
		if ( *nextIterator_ == query_->end() )
			return getPreviousTransactionGroup(getQuantityRequested());
	}
	else
		nextIterator_ = new DBQuery<DBTransaction>::iterator(query_->begin());

	return getNextTransactionGroup(getQuantityRequested());
}


Util::CSmartPtr<WriteTransactionGroupMessage> TransactionManager::VoidableTransactionsModule::getNextTransactionGroup(int quantity)
{
	setQuantityRequested(quantity);

	// The next line was here for HLX, but has been commented out for HTi.
	// The HLX has a next/previous button to get to the next/prevous page of transactions;
	// the HTi does not have these buttons, so all transactions are shown and the user uses the scroll bar to go through them.
	// So for the HTi, we use a pased-in quantity of 0 to indicate, 'give me all transactions'.
	//if ( quantityRequested_ <= 0 ) return Util::CSmartPtr<WriteTransactionGroupMessage>(0);

	edb::DBQuery<edb::DBTransaction>::iterator tempIterator_ = *nextIterator_;
	Util::CSmartPtr<WriteTransactionGroupMessage> group(new WriteTransactionGroupMessage());

	int i = (quantityRequested_ == 0) ? -1 : 0;
	while ( i < quantityRequested_ )
	{
		Util::CSmartPtr<Transaction> transaction = getNextTransaction();
		if ( transaction.notNull() )
			group->addTransaction(transaction);
		else break;

		if (quantityRequested_)
			i++;
	}

	if ( tempIterator_ != query_->end() )
		previousIterator_ = new DBQuery<DBTransaction>::iterator(--tempIterator_);
	else
		previousIterator_ = new DBQuery<DBTransaction>::iterator(--query_->end());

	group->setPreviousGroupAvailable(*previousIterator_ != query_->end());
	group->setNextGroupAvailable(*nextIterator_ != query_->end());

	return group;
}


Util::CSmartPtr<Transaction> TransactionManager::VoidableTransactionsModule::getNextTransaction()
{
	Util::CSmartPtr<Transaction> transaction(0);

	if ( query_.null() || nextIterator_.null() || *nextIterator_ == query_->end() )
		return transaction;

	do
	{
		transaction = manager_->createTransaction(&*(*nextIterator_));

		++*nextIterator_;
	} while ( !transaction->isRecordValid() && *nextIterator_ != query_->end() );

	return transaction.notNull() && transaction->isRecordValid() ? transaction : Util::CSmartPtr<Transaction>(0);
}


Util::CSmartPtr<Transaction> TransactionManager::VoidableTransactionsModule::getPreviousTransaction()
{
	Util::CSmartPtr<Transaction> transaction(0);

	if ( query_.null() || previousIterator_.null() || *previousIterator_ == query_->end() )
		return transaction;

	do
	{
		transaction = manager_->createTransaction(&*(*previousIterator_));

		--*previousIterator_;
	} while ( !transaction->isRecordValid() && *previousIterator_ != query_->end() );

	return transaction.notNull() && transaction->isRecordValid() ? transaction : Util::CSmartPtr<Transaction>(0);
}


void TransactionManager::VoidableTransactionsModule::handleVoidedTransaction(Util::CSmartPtr<Transaction> transaction)
{
	if ( query_.notNull() )
	{
		if ( previousIterator_.notNull() && *previousIterator_ != query_->end() )
		{
			if ( util_string::stringtolong((*previousIterator_)->getScaleID()) == transaction->getScaleId() &&
					util_string::stringtolong((*previousIterator_)->getTransactionID()) == transaction->getTransactionId() )
				--*previousIterator_;
		}

		if ( nextIterator_.notNull() && *nextIterator_ != query_->end() )
		{
			if ( util_string::stringtolong((*nextIterator_)->getScaleID()) == transaction->getScaleId() &&
					util_string::stringtolong((*nextIterator_)->getTransactionID()) == transaction->getTransactionId() )
				++*nextIterator_;
		}
	}
}


void TransactionManager::VoidableTransactionsModule::destroyQueriesAndIters()
{
	/* Destroy first the iterators. */
	previousIterator_ = 0; nextIterator_ = 0;

	/* Destroy now the queries. */
	query_ = 0;
}


int TransactionManager::countAllRecords()
{
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM )
		return -1;

	return transactionsRM->countAll();
}

int TransactionManager::deleteAllCutTestTransactions(bool removeTransaction)	//	AWL-135, HA-134
{
	edb::DBTransactionsRM* transactionsRM = mgrContainer_->getDBTransactionsRM();
	if ( !transactionsRM )
		return 0;

	std::string action = removeTransaction ? "delete" : "clear";
	Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Going to " + action + " all CuttingTest transactions.");

	int count = 0;

	Util::CSmartPtr<edb::DBTransactionsRM::query_type> query(transactionsRM->retrieveAllCutTestTransactions());
	edb::DBTransactionsRM::query_type::iterator iterator = query->begin();
	edb::DBTransactionsRM::query_type::iterator endIterator = query->end();
	Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Found " + HobString(Integer::toString(query->size())) + " CuttingTest transactions.");
	
	while ( iterator != endIterator )
	{
		Util::CSmartPtr<edb::DBTransaction> ctTransSmart(new edb::DBTransaction(*iterator));

		++iterator;

		edb::DBTransaction *ctTransRec = ctTransSmart.get();
		if ( !ctTransRec )
			Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Unable to get the DBTransaction record!");
		else
		{
			std::string scaleId = ctTransRec->getScaleID();
			std::string transId = ctTransRec->getTransactionID();
			std::string cutTestNum = ctTransRec->getCutTestNumber();

			if (removeTransaction)
			{
				if ( !transactionsRM->deleteRecord(ctTransRec) )
				{
					Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Tried but failed to delete the CuttingTest transaction");
					Util::CSmartPtr<hfc::Transaction> trans = createTransaction(ctTransSmart.get());
					trans->show();
				}
				else
				{
					++count;
					Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Removed CuttingTest transaction (" +
							scaleId + ", " + transId + ", For Cut test # " + cutTestNum + ")");
				}
			}
			else
			{
				std::string scaleId = ctTransRec->getScaleID();
				std::string transId = ctTransRec->getTransactionID();
				std::string cutTestNum = ctTransRec->getCutTestNumber();

				ctTransRec->setCutTestNumberNull();
				transactionsRM->saveRecord(ctTransRec);

				++count;
				Debug::trace("TransactionManager", "deleteAllCutTestTransactions():  Cleared CuttingTest transaction (" +
						scaleId + ", " + transId + ", For Cut test # " + cutTestNum + ")");
			}
		}
	}
	return count;
}

}

