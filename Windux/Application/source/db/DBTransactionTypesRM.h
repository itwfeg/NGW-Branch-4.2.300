#ifndef DBTRANSACTIONTYPESRM_H
#define DBTRANSACTIONTYPESRM_H


#include <string>


#include "edb.h"
#include DBTRANSACTIONTYPESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBTransactionType;
	class DBTransaction;
	template <class DBEntity> class DBQuery;


	class DBTransactionTypesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBTransactionType>;

		public:

			// Types
			typedef DBTransactionType record_type;
			typedef DBQuery<DBTransactionType> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBTransactionTypesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBTransactionType>* retrieveAll();
			DBQuery<DBTransactionType>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the TransactionType record".
			DBTransactionType* findNumber(const std::string& number) const;
			DBTransactionType* find(const std::string& key) const { return findNumber(key); }
			DBTransactionType* findName1(const std::string& name) const;
			DBTransactionType* findName2(const std::string& name) const;
			DBTransactionType* findLinkedRecord(const DBTransaction* transaction) const;

			DBTransactionType* createRecord() const;
			void saveRecord(const DBTransactionType* rec) const;
			int deleteRecord(const DBTransactionType* rec) const;     // returns # of deleted records
			int deleteAll() const;                                    // returns # of deleted records
			int countAll() const;                                     // returns # of existing records
			int calculateAverageSpace() const;                        // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBTransactionType> > qsRegistry_;
			edb_adapter::DBTransactionTypesRMI* decorated_;


			// --- Private Constructor ---
			DBTransactionTypesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBTransactionTypesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBTransactionTypesRM(const DBTransactionTypesRM& rm);
			// Do not allow the default assignment operator.
			DBTransactionTypesRM operator=(const DBTransactionTypesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBTransactionTypesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBTransactionTypesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBTransactionTypesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBTransactionType>* query);
			bool deregisterQ(DBQuery<DBTransactionType>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTRANSACTIONTYPESRM_H
