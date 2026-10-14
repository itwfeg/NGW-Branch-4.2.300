#ifndef DBNETWORKERRORSRM_H
#define DBNETWORKERRORSRM_H


#include <string>


#include "edb.h"
#include DBNETWORKERRORSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBNetworkError;
	template <class DBEntity> class DBQuery;


	class DBNetworkErrorsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBNetworkError>;

		public:

			// Types
			typedef DBNetworkError record_type;
			typedef DBQuery<DBNetworkError> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBNetworkErrorsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// Processing methods for retrieving data from the database
			DBQuery<DBNetworkError>* retrieveAll();

			// Processing methods for other database operations
			DBNetworkError* findID(const std::string& id) const;
			DBNetworkError* find(const std::string& key) const { return findID(key); }

			DBNetworkError* createRecord() const;
			void saveRecord(const DBNetworkError* rec) const;
			int deleteRecord(const DBNetworkError* rec) const;  // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBNetworkError> > qsRegistry_;
			edb_adapter::DBNetworkErrorsRMI* decorated_;


			// The constructor is only accessible by friend class DBFactory
			DBNetworkErrorsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBNetworkErrorsRMI* d);

			// Do not allow the default copy constructor.
			DBNetworkErrorsRM(const DBNetworkErrorsRM& os);
			// Do not allow the default assignment operator.
			DBNetworkErrorsRM operator=(const DBNetworkErrorsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBNetworkErrorsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBNetworkErrorsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBNetworkErrorsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBNetworkError>* query);
			bool deregisterQ(DBQuery<DBNetworkError>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBNETWORKERRORSRM_H
