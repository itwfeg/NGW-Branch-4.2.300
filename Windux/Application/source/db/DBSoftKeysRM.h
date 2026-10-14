#ifndef DBSOFTKEYSRM_H
#define DBSOFTKEYSRM_H


#include <string>


#include "edb.h"
#include DBSOFTKEYSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSoftKey;
	class DBSoftKeyGroup;
	class DBProduct;
	template <class DBEntity> class DBQuery;


	class DBSoftKeysRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSoftKey>;

		public:

			// Types
			typedef DBSoftKey record_type;
			typedef DBQuery<DBSoftKey> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSoftKeysRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBSoftKey>* retrieveAll();
			DBQuery<DBSoftKey>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBSoftKey>* retrieveGroup(const std::string&  groupNumber, const std::string& deptNum);
			DBQuery<DBSoftKey>* retrieveGroup(const DBSoftKeyGroup* skg);

			// --- Processing methods for other database operations ---
			// findSoftKey: a null return means "unable to find the SoftKey record".
			DBSoftKey* find(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp) const;

			DBSoftKey* createRecord(int groupNumber, int deptNumber, int horizontalPosition, int verticalPosition) const;
			void saveRecord(const DBSoftKey* rec) const;
			int deleteRecord(const DBSoftKey* rec) const;       // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
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
			util_registry::Registry< DBQuery<DBSoftKey> > qsRegistry_;
			edb_adapter::DBSoftKeysRMI* decorated_;


			// --- Private Constructor ---
			DBSoftKeysRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeysRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBSoftKeysRM(const DBSoftKeysRM& rm);
			// Do not allow the default assignment operator.
			DBSoftKeysRM operator=(const DBSoftKeysRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSoftKeysRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSoftKeysRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSoftKeysRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSoftKey>* query);
			bool deregisterQ(DBQuery<DBSoftKey>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYSRM_H
