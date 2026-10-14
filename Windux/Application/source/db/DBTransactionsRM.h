#ifndef DBTRANSACTIONSRM_H
#define DBTRANSACTIONSRM_H


#include <string>


#include "edb.h"
#include DBTRANSACTIONSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBTransaction;
	template <class DBEntity> class DBQuery;


	class DBTransactionsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBTransaction>;

		public:

			// Types
			typedef DBTransaction record_type;
			typedef DBQuery<DBTransaction> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBTransactionsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBTransaction>* retrieveAll();
			DBQuery<DBTransaction>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBTransaction>* retrieveReadAndClear(bool equal, int cleared, const std::string& deptNum);
			DBQuery<DBTransaction>* retrieveOperatorVoidable(const std::string& operatorId, int voidValue, const std::string& deptNum);
			DBQuery<DBTransaction>* retrieveSupervisorVoidable(const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& startTime, const std::string& endTime, int voidValue);
			DBQuery<DBTransaction>* retrieveFilteredTransactions(const std::string& startTime, const std::string& endTime, const std::string& transType, int voidValue,
																const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder = true);
			DBQuery<DBTransaction>* retrieveAllCutTestTransactions();	//	AWL-135, HA-134
			DBQuery<DBTransaction>* retrieveCutTestTransactionsForCutTest(const std::string& cutTestNumber);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the Transaction record".
			DBTransaction* find(const std::string& key1, const std::string& key2) const { return findTransaction(key1, key2); }
			DBTransaction* findTransaction(const std::string& scaleId, const std::string& transId) const;

			DBTransaction* createRecord(const std::string& scaleId, const std::string& transId = "") const;
			void saveRecord(const DBTransaction* rec) const;
			int deleteRecord(const DBTransaction* rec) const;   // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                               // returns # of existing records
			int calculateAverageSpace() const;                  // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);


			static const std::string DESCRIPTOR;


			std::string getNextTransactionId();


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBTransaction> > qsRegistry_;
			edb_adapter::DBTransactionsRMI* decorated_;


			// --- Private Constructor ---
			DBTransactionsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBTransactionsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBTransactionsRM(const DBTransactionsRM& rm);
			// Do not allow the default assignment operator.
			DBTransactionsRM operator=(const DBTransactionsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBTransactionsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBTransactionsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBTransactionsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBTransaction>* query);
			bool deregisterQ(DBQuery<DBTransaction>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTRANSACTIONSRM_H
