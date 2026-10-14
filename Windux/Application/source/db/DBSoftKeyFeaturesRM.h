#ifndef DBSOFTKEYFEATURESRM_H
#define DBSOFTKEYFEATURESRM_H


#include <string>


#include "edb.h"
#include DBSOFTKEYFEATURESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSoftKeyFeature;
	template <class DBEntity> class DBQuery;


	class DBSoftKeyFeaturesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSoftKeyFeature>;

		public:

			// Types
			typedef DBSoftKeyFeature record_type;
			typedef DBQuery<DBSoftKeyFeature> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSoftKeyFeaturesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBSoftKeyFeature>* retrieveAll();
			DBQuery<DBSoftKeyFeature>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBSoftKeyFeature>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findSoftKeyFeature: a null return means "unable to find the SoftKeyFeature record".
			DBSoftKeyFeature* findNumber(const std::string& number, const std::string& deptNum) const;
			DBSoftKeyFeature* find(const std::string& number, const std::string& deptNum) const { return findNumber(number, deptNum); }

			DBSoftKeyFeature* createRecord(const std::string& number) const;
			void saveRecord(const DBSoftKeyFeature* rec) const;
			int deleteRecord(const DBSoftKeyFeature* rec) const;        // returns # of deleted records
			int deleteAll() const;                                      // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                       // returns # of existing records
			int calculateAverageSpace() const;                          // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBSoftKeyFeature> > qsRegistry_;
			edb_adapter::DBSoftKeyFeaturesRMI* decorated_;


			// --- Private Constructor ---
			DBSoftKeyFeaturesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyFeaturesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBSoftKeyFeaturesRM(const DBSoftKeyFeaturesRM& rm);
			// Do not allow the default assignment operator.
			DBSoftKeyFeaturesRM operator=(const DBSoftKeyFeaturesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSoftKeyFeaturesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSoftKeyFeaturesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSoftKeyFeaturesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSoftKeyFeature>* query);
			bool deregisterQ(DBQuery<DBSoftKeyFeature>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYFEATURESRM_H
