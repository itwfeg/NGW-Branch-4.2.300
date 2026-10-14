#ifndef DBSYNCHRONIZATIONCHANGESRM_H
#define DBSYNCHRONIZATIONCHANGESRM_H


#include <string>


#include "edb.h"
#include DBSYNCHRONIZATIONCHANGESRMI_INC
#include DBSYNCHRONIZATIONCHANGE_INC
#include DBRECORDMANAGER_INC
#include DBQUERY_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSynchronizationChange;
	class DBBatch;
	template <class DBEntity> class DBQuery;


	class DBSynchronizationChangesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSynchronizationChange>;

		public:

			// Types
			typedef DBSynchronizationChange record_type;
			typedef DBQuery<DBSynchronizationChange> query_type;
			typedef query_type::const_iterator iterator_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSynchronizationChangesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// Processing methods for retrieving data from the database
			DBQuery<DBSynchronizationChange>* retrieveAll();
			DBQuery<DBSynchronizationChange>* retrieveLinkedToBatch(const DBBatch* batch);
			DBQuery<DBSynchronizationChange>* retrieveLinkedToBatch(const std::string& batchNumber);

			// Processing methods for other database operations
			DBSynchronizationChange* findIdentifier(const std::string& id) const;
			DBSynchronizationChange* find(const std::string& key) const { return findIdentifier(key); }

			DBSynchronizationChange* createRecord(const DBBatch* batch) const;
			DBSynchronizationChange* createRecord(const std::string& batchNumber) const;
			void saveRecord(const DBSynchronizationChange* rec) const;
			int deleteRecord(const DBSynchronizationChange* rec) const;  // returns # of deleted records
			int deleteAll() const;                                       // returns # of deleted records
			int countAll() const;                                        // returns # of existing records
			int calculateAverageSpace() const;                           // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBSynchronizationChange> > qsRegistry_;
			edb_adapter::DBSynchronizationChangesRMI* decorated_;


			// The constructor is only accessible by friend class DBFactory
			DBSynchronizationChangesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSynchronizationChangesRMI* d);

			// Do not allow the default copy constructor.
			DBSynchronizationChangesRM(const DBSynchronizationChangesRM& rm);
			// Do not allow the default assignment operator.
			DBSynchronizationChangesRM operator=(const DBSynchronizationChangesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSynchronizationChangesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSynchronizationChangesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSynchronizationChangesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSynchronizationChange>* query);
			bool deregisterQ(DBQuery<DBSynchronizationChange>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSYNCHRONIZATIONCHANGESRM_H
