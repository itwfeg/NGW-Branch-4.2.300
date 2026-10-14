#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H



#include <string>



#include "DataManager.h"

#include "Transaction.h"

#include "DBTransactionsRM.h"
#include "DBTransaction.h"
#include "DBQuery.h"


#include "CSmartPtr.h"
#include "MessageLogger.h"

class DBSession;
class Product;
class HobOperator;


namespace hfc
{


	class HFCManagerContainer;
	class ReadTransactionGroupMessage;
	class WriteTransactionGroupMessage;
	class VoidTransactionsMessage;


	class TransactionManager : public ::DataManager
	{
		public:
			/* --- Types and subtypes --- */
			class NotClearedTransactionsModule
			{
				public:
					NotClearedTransactionsModule(TransactionManager* manager, edb::DBTransactionsRM* transactionsRM,
							bool equal = false, Transaction::ReadClearFlag flag = Transaction::ReadClearCleared, const std::string& deptNum = std::string(""));
					~NotClearedTransactionsModule();

					Util::CSmartPtr<Transaction> getNextTransaction(edb::DBTransactionsRM* transactionsRM, bool markRead, bool markCleared);
					bool empty() const { return query_->begin() == query_->end(); }
					int size() const { return query_.notNull() ? (int)query_->size() : 0; }

					void destroyQueriesAndIters();

				private:
					TransactionManager* manager_;
					Util::CSmartPtr< edb::DBQuery<edb::DBTransaction> > query_;
					Util::CSmartPtr< edb::DBQuery<edb::DBTransaction>::iterator > iterator_;

#ifdef USE_ACCOUNTANT
					static int _dataType_;
					static std::string _dataTypeName_;
#endif
			};

			class VoidableTransactionsModule
			{
				public:
					VoidableTransactionsModule(TransactionManager* manager, edb::DBTransactionsRM* transactionsRM, bool operatorMode,
						const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& start, const std::string& end);
					~VoidableTransactionsModule();

					int getQuantityRequested() const { return quantityRequested_; }
					void setQuantityRequested(int qty) { quantityRequested_ = qty; }

					Util::CSmartPtr<WriteTransactionGroupMessage> getNextTransactionGroup(int quantity);
					Util::CSmartPtr<WriteTransactionGroupMessage> getSameTransactionGroup();
					Util::CSmartPtr<WriteTransactionGroupMessage> getPreviousTransactionGroup(int quantity);

					void rebuildQuery();
					void handleVoidedTransaction(Util::CSmartPtr<Transaction> transaction);
					void destroyQueriesAndIters();

				private:
					Util::CSmartPtr<Transaction> getNextTransaction();
					Util::CSmartPtr<Transaction> getPreviousTransaction();

					TransactionManager* manager_;
					int quantityRequested_;
					Util::CSmartPtr< edb::DBQuery<edb::DBTransaction> > query_;
					Util::CSmartPtr< edb::DBQuery<edb::DBTransaction>::iterator > previousIterator_, nextIterator_;

					edb::DBTransactionsRM* transactionsRM_;
					HobDateTime hdtStart_;
					HobDateTime hdtEnd_;
					bool operatorMode_;
					std::string operatorId_;
					std::string deptNum_;
					std::string pluNumber_;

#ifdef USE_ACCOUNTANT
					static int _dataType_;
					static std::string _dataTypeName_;
#endif
			};


			/* --- Constructors and destructor --- */
			TransactionManager(hfc::HFCManagerContainer* container);
			~TransactionManager();


			EvolDeviceType getDevice() const { return TransactionDev; }
			/* --- Getter and setter members --- */
			DBSession * getMySession() { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }

		
			/* --- Instance function members --- */
			void commitTransaction(Transaction* transaction);

			Util::CSmartPtr<Transaction> createProductionEntryTransaction(Transaction* trans);

			Util::CSmartPtr<Transaction> findTransactionByIdentifier(int id);
			Util::CSmartPtr<Transaction> findTransactionByIdentifier(const std::string & transId, const std::string & scaleId = "0");

			int initializeAllTransactionsQuery(bool countRecords = true);
			Util::CSmartPtr<Transaction> getFirstTransaction();
			Util::CSmartPtr<Transaction> getNextTransaction();

			bool initializeAllFilteredTransactionsQuery(const std::string& start, const std::string& end, const std::string& transType,
			Transaction::VoidFlag voidFlag, const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber = "", const std::string& operatorId = "", bool defaultOrder = true);
			Util::CSmartPtr<Transaction> getFirstFilteredTransaction();
			Util::CSmartPtr<Transaction> getPreviousFilteredTransaction();
			Util::CSmartPtr<Transaction> getNextFilteredTransaction();
			void resetPreviousFilteredTransaction();
			void resetNextFilteredTransaction();

			void clearReadTransactions();
			void clearNotClearedTransactions();
			int initializeNotClearedQuery(bool countRecords = true, const std::string& deptNum = std::string("")) { return initializeReadClearQuery(false, Transaction::ReadClearCleared, countRecords, deptNum); }
			int initializeReadQuery(bool countRecords = true, const std::string& deptNum = std::string("")) { return initializeReadClearQuery(true, Transaction::ReadClearRead, countRecords, deptNum); }
			int initializeReadClearQuery(bool equal, Transaction::ReadClearFlag flag, bool countRecords = true, const std::string& deptNum = std::string(""));
			Util::CSmartPtr<Transaction> getFirstReadClearTransaction(bool markRead = true, bool markCleared = false);
			Util::CSmartPtr<Transaction> getNextReadClearTransaction(bool markRead = true, bool markCleared = false);

//			Util::CSmartPtr<Transaction> createTransaction(int type, ::Product* prod);
			void voidTransaction(Util::CSmartPtr<Transaction> transaction, HobOperator* oper);
			void handleSaveTransaction(Transaction* trans);
			int deleteTransaction(Transaction* trans);
			int deleteAllTransactions(HobRecordNum deptNum = RECORD_NOT_FOUND);
			void purgeOldTransactions(int daysToKeepTransactions, MessageLogger::LogFileIndicator logToFile);  // Added FHA when log files were added for Saffeway

			void handleMessage(SessionMessage* msg);
			void destroyQueriesAndIters();

			int deleteAllCutTestTransactions(bool removeTransaction = true);	//	AWL-135, HA-134

			int countAllRecords();


		private:
			friend class VoidableTransactionsModule;
			friend class TransactionUnitTester;

			DBSession * mySession_;
			hfc::HFCManagerContainer* mgrContainer_;
			Util::CSmartPtr< edb::DBQuery<edb::DBTransaction> > allTransactionsQuery_;
			Util::CSmartPtr< edb::DBQuery<edb::DBTransaction>::iterator > allTransactionsIterator_;
			Util::CSmartPtr< edb::DBQuery<edb::DBTransaction> > allFilteredTransactionsQuery_;
			Util::CSmartPtr< edb::DBQuery<edb::DBTransaction>::iterator > allFilteredTransactionsPreviousIterator_, allFilteredTransactionsNextIterator_;

			Util::CSmartPtr<NotClearedTransactionsModule> notClearedTransactions_;
			Util::CSmartPtr<VoidableTransactionsModule> voidableTransactions_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			void handleDeleteAll();
			void handleDeleteAll(HobRecordNum deptNum);
			void handleClearTotals(UINT8 event);
			void handleReadNonVoidTransactions(ReadTransactionGroupMessage* message);
			void handleVoidTransactions(VoidTransactionsMessage* message);
			void handleReadDates();
			void saveTransaction(Transaction* trans);

			Util::CSmartPtr<Transaction> createTransaction(const edb::DBTransaction * dbTransactionRec);
			Util::CSmartPtr<Transaction> createVoidingTransaction(Util::CSmartPtr<Transaction> transaction, HobOperator* oper);

        #if defined(LINUX_FIX)
			HobRecordNum generateNextTransactionId();
        #else
			HobRecordNum TransactionManager::generateNextTransactionId();
        #endif
	};
}



#endif /* TRANSACTIONMANAGER_H */
