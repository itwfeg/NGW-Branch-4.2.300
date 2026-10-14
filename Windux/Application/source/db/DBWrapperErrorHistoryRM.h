#ifndef DBWRAPPERERRORHISTORYRM_H
#define DBWRAPPERERRORHISTORYRM_H


#include <string>


#include "edb.h"
#include DBWRAPPERERRORHISTORYRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBWrapperErrorHistory;
	template <class DBEntity> class DBQuery;


	class DBWrapperErrorHistoryRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBWrapperErrorHistory>;

		public:

			// Types
			typedef DBWrapperErrorHistory record_type;
			typedef DBQuery<DBWrapperErrorHistory> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBWrapperErrorHistoryRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBWrapperErrorHistory>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the WrapperErrorHistory record".
			DBWrapperErrorHistory* find(const std::string& number) const;

			DBWrapperErrorHistory* createRecord() const;
			void saveRecord(const DBWrapperErrorHistory* rec) const;
			int deleteRecord(const DBWrapperErrorHistory* rec)const;
			int deleteAll() const;
			int countAll() const;
			int calculateAverageSpace() const;
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
			util_registry::Registry<DBQuery<DBWrapperErrorHistory> > qsRegistry_;
			edb_adapter::DBWrapperErrorHistoryRMI* decorated_;


			// --- Private Constructor ---
			DBWrapperErrorHistoryRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBWrapperErrorHistoryRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBWrapperErrorHistoryRM(const DBWrapperErrorHistoryRM& rm);
			// Do not allow the default assignment operator.
			DBWrapperErrorHistoryRM operator=(const DBWrapperErrorHistoryRM& rhs);


			// Registry methods
			static util_registry::Registry<DBWrapperErrorHistoryRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBWrapperErrorHistoryRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBWrapperErrorHistoryRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBWrapperErrorHistory>* query);
			bool deregisterQ(DBQuery<DBWrapperErrorHistory>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBWRAPPERERRORHISTORYRM_H
