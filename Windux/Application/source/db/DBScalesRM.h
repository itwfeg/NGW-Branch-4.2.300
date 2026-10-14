#ifndef DBSCALESRM_H
#define DBSCALESRM_H


#include <string>


#include "edb.h"
#include DBSCALESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBScale;
	template <class DBEntity> class DBQuery;


	class DBScalesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBScale>;

		public:

			// Types
			typedef DBScale record_type;
			typedef DBQuery<DBScale> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBScalesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBScale>* retrieveAll();
			DBQuery<DBScale>* retrieveType(int type);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Scale record".
			DBScale* findIdentifier(const std::string& identifier) const;
			DBScale* findHostName(const std::string& hostname) const;
			DBScale* findIPAddress(const std::string& ipAddress) const;
			DBScale* findType(int type) const;
			DBScale* find(const std::string& key) const { return findIdentifier(key); }

			DBScale* createRecord() const;
			void saveRecord(const DBScale* rec) const;
			int deleteRecord(const DBScale* rec) const;         // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int countAll() const;                               // returns # of existing records
			int calculateAverageSpace() const;                  // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);

			void ensureAutoIncrementValue(int value);


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBScale> > qsRegistry_;
			edb_adapter::DBScalesRMI* decorated_;


			// --- Private Constructor ---
			DBScalesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBScalesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBScalesRM(const DBScalesRM& rm);
			// Do not allow the default assignment operator.
			DBScalesRM operator=(const DBScalesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBScalesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBScalesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBScalesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBScale>* query);
			bool deregisterQ(DBQuery<DBScale>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSCALESRM_H
