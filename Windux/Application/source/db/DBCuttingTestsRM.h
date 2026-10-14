#ifndef DBCUTTINGTESTSRM_H
#define DBCUTTINGTESTSRM_H


#include <string>


#include "edb.h"
#include DBCUTTINGTESTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCuttingTest;
	class DBOperator;
	template <class DBEntity> class DBQuery;


	class DBCuttingTestsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCuttingTest>;

		public:

			// Types
			typedef DBCuttingTest record_type;
			typedef DBQuery<DBCuttingTest> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCuttingTestsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCuttingTest>* retrieveAll();
			DBQuery<DBCuttingTest>* retrieveByOperator(const std::string& opNum);

			// Processing methods for other database operations
			DBCuttingTest* find(const std::string& id) const;

			DBCuttingTest* createRecord(const std::string& operatorNumber) const;
			void saveRecord(const DBCuttingTest* rec) const;
			int deleteRecord(const DBCuttingTest* rec) const;   // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int countAll() const;                               // returns # of existing records
			int calculateAverageSpace() const;                  // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBCuttingTest> > qsRegistry_;
			edb_adapter::DBCuttingTestsRMI* decorated_;


			// --- Private Constructor ---
			DBCuttingTestsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCuttingTestsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCuttingTestsRM(const DBCuttingTestsRM& rm);
			// Do not allow the default assignment operator.
			DBCuttingTestsRM operator=(const DBCuttingTestsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCuttingTestsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCuttingTestsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCuttingTestsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCuttingTest>* query);
			bool deregisterQ(DBQuery<DBCuttingTest>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCUTTINGTESTSRM_H
