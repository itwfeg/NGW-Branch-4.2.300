#ifndef DBPRODUCTDEFAULTSRM_H
#define DBPRODUCTDEFAULTSRM_H


#include <string>


#include "edb.h"
#include DBPRODUCTDEFAULTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProductDefault;
	template <class DBEntity> class DBQuery;


	class DBProductDefaultsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProductDefault>;

		public:

			// Types
			typedef DBProductDefault record_type;
			typedef DBQuery<DBProductDefault> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductDefaultsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProductDefault>* retrieveAll();
			DBQuery<DBProductDefault>* retrieveAllForDept(const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the ProductDefault record".
			DBProductDefault* findType(const std::string& type, const std::string& deptNum) const;
			DBProductDefault* find(const std::string& key, const std::string& deptNum) const { return findType(key, deptNum); }

			DBProductDefault* createRecord() const;
			void saveRecord(const DBProductDefault* rec) const;
			int deleteRecord(const DBProductDefault* rec) const;  // returns # of deleted records
			int deleteAll() const;                                // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                 // returns # of existing records
			int calculateAverageSpace() const;                    // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBProductDefault> > qsRegistry_;
			edb_adapter::DBProductDefaultsRMI* decorated_;


			// --- Private Constructor ---
			DBProductDefaultsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductDefaultsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductDefaultsRM(const DBProductDefaultsRM& rm);
			// Do not allow the default assignment operator.
			DBProductDefaultsRM operator=(const DBProductDefaultsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductDefaultsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductDefaultsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductDefaultsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProductDefault>* query);
			bool deregisterQ(DBQuery<DBProductDefault>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTDEFAULTSRM_H
