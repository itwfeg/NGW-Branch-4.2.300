#ifndef DBDEPTSRM_H
#define DBDEPTSRM_H


#include <string>


#include "edb.h"
#include DBDEPTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBDept;
	template <class DBEntity> class DBQuery;


	class DBDeptsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBDept>;

		public:

			// Types
			typedef DBDept record_type;
			typedef DBQuery<DBDept> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBDeptsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBDept>* retrieveAll();
			DBQuery<DBDept>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Dept record".
			DBDept* findNumber(const std::string& number) const;
			DBDept* find(const std::string& key) const { return findNumber(key); }

			DBDept* createRecord() const;
			void saveRecord(const DBDept* rec) const;
			int deleteRecord(const DBDept* rec) const;      // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBDept> > qsRegistry_;
			edb_adapter::DBDeptsRMI* decorated_;


			// --- Private Constructor ---
			DBDeptsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBDeptsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBDeptsRM(const DBDeptsRM& rm);
			// Do not allow the default assignment operator.
			DBDeptsRM operator=(const DBDeptsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBDeptsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBDeptsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBDeptsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBDept>* query);
			bool deregisterQ(DBQuery<DBDept>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBDEPTSRM_H
