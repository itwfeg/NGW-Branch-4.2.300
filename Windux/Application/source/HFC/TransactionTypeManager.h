#ifndef TRANSACTIONTYPEMANAGER_H
#define TRANSACTIONTYPEMANAGER_H



#include "DataManager.h"


#include "TransactionType.h"
#include "DBTransactionTypesRM.h"
#include "DBTransactionType.h"
#include "DBQuery.h"


#include "CSmartPtr.h"

#include "evolution.h"

class DBSession;

class SessionMessage;


namespace hfc
{


	class HFCManagerContainer;


	class TransactionTypeManager : public DataManager
	{
		public:
			/* --- Types and subtypes --- */
			static const short _MaximumRecords_	= 99;


			/* --- Constructors and destructor --- */
			TransactionTypeManager(hfc::HFCManagerContainer * container);
			~TransactionTypeManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }

			Util::CSmartPtr<TransactionType> getSupervisorModeTransactionType();
			void setSupervisorModeTransactionType(Util::CSmartPtr<TransactionType> newTransactionType) { supervisorModeTransactionType_ = newTransactionType; }
			void clearSupervisorModeTransactionType() { supervisorModeTransactionType_ = 0; }

			Util::CSmartPtr<TransactionType> getOperatorModeTransactionType();
			void setOperatorModeTransactionType(Util::CSmartPtr<TransactionType> newTransactionType) { operatorModeTransactionType_ = newTransactionType; }
			void clearOperatorModeTransactionType() { operatorModeTransactionType_ = 0; }


			/* --- Instance function members --- */
			/* DataManager virtual methods. */
			TransactionType* getFirstRecord() { return firstTransactionType(); }
			TransactionType* getNextRecord()  { return nextTransactionType(); }
			EvolDeviceType getDevice() const { return TransactionTypeDev; }

			/* Message handling methods */
			void handleMessage(SessionMessage * msg);
			void handleDeleteAll();

			/* Record navigation methods */
			int initializeAllTransactionTypesQuery();
        #if defined( __linux__ )
			Util::CSmartPtr<TransactionType> getFirstTransactionType() { return Util::CSmartPtr<TransactionType>(firstTransactionType()); }
			Util::CSmartPtr<TransactionType> getNextTransactionType() { return Util::CSmartPtr<TransactionType>(nextTransactionType()); }
        #else
			Util::CSmartPtr<TransactionType> TransactionTypeManager::getFirstTransactionType() { return Util::CSmartPtr<TransactionType>(firstTransactionType()); }
			Util::CSmartPtr<TransactionType> TransactionTypeManager::getNextTransactionType() { return Util::CSmartPtr<TransactionType>(nextTransactionType()); }
        #endif
			int countAllRecords();
			void destroyQueriesAndIters();

			/* Database access methods */
			Util::CSmartPtr<TransactionType> findTransactionType(HobRecordNum transactionTypeNum);
			void saveRecord(Util::CSmartPtr<TransactionType> transType);
			int deleteRecord(Util::CSmartPtr<TransactionType> transType);
			void verifyReservedRecords();

			bool currentTypeAllowsBackdating();

			//	AWL-52
			void tempSwitchToTransactionType(HobRecordNum totalTypeNum);
			void revertToPrevTransactionType();

			void clearPrevTransactionTypeNum() {prevTransTypeNum = 0;}


		private:
			DBSession * mySession_;
			hfc::HFCManagerContainer * mgrContainer_;

			// The reason we need more than one "supervisorModeTransactionType_" is because when returning to
			// Operator Mode we need to know which transaction type was current there when we left it.
			Util::CSmartPtr<TransactionType> supervisorModeTransactionType_;
			Util::CSmartPtr<TransactionType> operatorModeTransactionType_;		// Transaction type selected in Operate mode

			Util::CSmartPtr< edb::DBQuery<edb::DBTransactionType> > allTransactionTypesQuery_;
			Util::CSmartPtr< edb::DBQuery<edb::DBTransactionType>::const_iterator > allTransactionTypesIterator_;


			/* --- Instance function members --- */
			void init();

			TransactionType* createTransactionType(const edb::DBTransactionType * dbTransactionTypeRec);
			TransactionType *firstTransactionType();
			TransactionType *nextTransactionType();

			void handleRead(TransactionType *msgTransactionType);
			void handleWrite(TransactionType *msgTransactionType);
			void handleDelete(SessionMessage *msg);
			void handleUpdateAndSave(TransactionType *modifiedTransactionType);
			void handleSaveRecord(Util::CSmartPtr<TransactionType> recordToSave);
			void handleRestore(SessionMessage *msg);
			void handleCreate();
			void handleReadAllTransactionTypeNames();
			void handleSetCurrentType(TransactionType *msgTransactionType);
			void handleSetCurrentRunModeType(TransactionType *msgTransactionType);
			void handleReadCurrentType();
			void handleWriteCurrentType(HobRecordNum number);

			HobRecordNum prevTransTypeNum;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};
}



#endif //TRANSACTIONTYPEMANAGER_H
