#ifndef DBSOFTKEYGROUPSRM_H
#define DBSOFTKEYGROUPSRM_H


#include <string>


#include "edb.h"
#include DBSOFTKEYGROUPSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSoftKeyGroup;
	template <class DBEntity> class DBQuery;


	class DBSoftKeyGroupsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSoftKeyGroup>;

		public:

			// Types
			typedef DBSoftKeyGroup record_type;
			typedef DBQuery<DBSoftKeyGroup> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSoftKeyGroupsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBSoftKeyGroup>* retrieveAll();
			DBQuery<DBSoftKeyGroup>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBSoftKeyGroup>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findSoftKeyGroup: a null return means "unable to find the SoftKeyGroup record".
			DBSoftKeyGroup* findNumber(const std::string& number, const std::string& deptNum) const;
			DBSoftKeyGroup* find(const std::string& number, const std::string& deptNum) const { return findNumber(number, deptNum); }

			DBSoftKeyGroup* createRecord(const std::string& number) const;
			void saveRecord(const DBSoftKeyGroup* rec) const;
			int deleteRecord(const DBSoftKeyGroup* rec) const;         // returns # of deleted records
			int deleteAll() const;                                     // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                      // returns # of existing records
			int calculateAverageSpace() const;                         // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBSoftKeyGroup> > qsRegistry_;
			edb_adapter::DBSoftKeyGroupsRMI* decorated_;


			// --- Private Constructor ---
			DBSoftKeyGroupsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyGroupsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBSoftKeyGroupsRM(const DBSoftKeyGroupsRM& rm);
			// Do not allow the default assignment operator.
			DBSoftKeyGroupsRM operator=(const DBSoftKeyGroupsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSoftKeyGroupsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSoftKeyGroupsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSoftKeyGroupsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSoftKeyGroup>* query);
			bool deregisterQ(DBQuery<DBSoftKeyGroup>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYGROUPSRM_H
