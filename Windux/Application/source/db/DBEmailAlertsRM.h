#ifndef DBEMAILALERTSRM_H
#define DBEMAILALERTSRM_H


#include <string>


#include "edb.h"
#include DBEMAILALERTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBEmailAlert;
	template <class DBEntity> class DBQuery;


	class DBEmailAlertsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBEmailAlert>;

		public:

			// Types
			typedef DBEmailAlert record_type;
			typedef DBQuery<DBEmailAlert> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBEmailAlertsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBEmailAlert>* retrieveAll();
			DBQuery<DBEmailAlert>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the EmailAlert record".
			DBEmailAlert* findNumber(const std::string& number) const;
			DBEmailAlert* find(const std::string& key) const { return findNumber(key); }

			DBEmailAlert* createRecord() const;
			void saveRecord(const DBEmailAlert* rec) const;
			int deleteRecord(const DBEmailAlert* rec) const;    // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBEmailAlert> > qsRegistry_;
			edb_adapter::DBEmailAlertsRMI* decorated_;


			// --- Private Constructor ---
			DBEmailAlertsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBEmailAlertsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBEmailAlertsRM(const DBEmailAlertsRM& rm);
			// Do not allow the default assignment operator.
			DBEmailAlertsRM operator=(const DBEmailAlertsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBEmailAlertsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;    /* (SJJC port 6/19/06) */
			static bool registerRM(DBEmailAlertsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBEmailAlertsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBEmailAlert>* query);
			bool deregisterQ(DBQuery<DBEmailAlert>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBEMAILALERTSRM_H
