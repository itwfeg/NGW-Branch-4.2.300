#ifndef DBCONFIGURATIONSRM_H
#define DBCONFIGURATIONSRM_H


#include <string>


#include "edb.h"
#include DBCONFIGURATIONSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBConfiguration;
	template <class DBEntity> class DBQuery;


	class DBConfigurationsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBConfiguration>;

		public:

			// Types
			typedef DBConfiguration record_type;
			typedef DBQuery<DBConfiguration> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBConfigurationsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBConfiguration>* retrieveAll();

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the Configuration record".
			DBConfiguration* findName(const std::string& name) const;
			DBConfiguration* find(const std::string& key) const { return findName(key); }

			DBConfiguration* createRecord() const;
			void saveRecord(const DBConfiguration* rec) const;
			int deleteRecord(const DBConfiguration* rec) const;    // returns # of deleted records
			int deleteAll() const;                                 // returns # of deleted records
			int countAll() const;                                  // returns # of existing records
			int calculateAverageSpace() const;                     // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBConfiguration> > qsRegistry_;
			edb_adapter::DBConfigurationsRMI* decorated_;


			// --- Private Constructor ---
			DBConfigurationsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBConfigurationsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBConfigurationsRM(const DBConfigurationsRM& rm);
			// Do not allow the default assignment operator.
			DBConfigurationsRM operator=(const DBConfigurationsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBConfigurationsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBConfigurationsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBConfigurationsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBConfiguration>* query);
			bool deregisterQ(DBQuery<DBConfiguration>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCONFIGURATIONSRM_H
