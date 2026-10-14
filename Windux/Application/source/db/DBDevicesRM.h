#ifndef DBDEVICESRM_H
#define DBDEVICESRM_H


#include <string>


#include "edb.h"
#include DBDEVICESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBDevice;
	template <class DBEntity> class DBQuery;


	class DBDevicesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBDevice>;

		public:

			// Types
			typedef DBDevice record_type;
			typedef DBQuery<DBDevice> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBDevicesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBDevice>* retrieveAll();
			DBQuery<DBDevice>* retrieveType(const std::string& type);
			DBQuery<DBDevice>* retrieveCheckPresence(bool check);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the Device record".
			DBDevice* findTypeSubtype(const std::string& type, const std::string& subtype) const;
			DBDevice* find(const std::string& key1, const std::string& key2) const { return findTypeSubtype(key1, key2); }

			DBDevice* createRecord() const;
			void saveRecord(const DBDevice* rec) const;
			int deleteRecord(const DBDevice* rec) const;        // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBDevice> > qsRegistry_;
			edb_adapter::DBDevicesRMI* decorated_;


			// --- Private Constructor ---
			DBDevicesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBDevicesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBDevicesRM(const DBDevicesRM& rm);
			// Do not allow the default assignment operator.
			DBDevicesRM operator=(const DBDevicesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBDevicesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBDevicesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBDevicesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBDevice>* query);
			bool deregisterQ(DBQuery<DBDevice>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBDEVICESRM_H
