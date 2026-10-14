#ifndef DBKNIFETIMEHISTORYRM_H
#define DBKNIFETIMEHISTORYRM_H


#include <string>


#include "edb.h"
#include DBKNIFETIMEHISTORYRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBKnifeTimeHistory;
	template <class DBEntity> class DBQuery;


	class DBKnifeTimeHistoryRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBKnifeTimeHistory>;

		public:

			// Types
			typedef DBKnifeTimeHistory record_type;
			typedef DBQuery<DBKnifeTimeHistory> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBKnifeTimeHistoryRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBKnifeTimeHistory>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the KnifeTimeHistory record".
			DBKnifeTimeHistory* find(const std::string& number) const;

			DBKnifeTimeHistory* createRecord() const;
			void saveRecord(const DBKnifeTimeHistory* rec) const;
			int deleteRecord(const DBKnifeTimeHistory* rec)const;
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
			util_registry::Registry<DBQuery<DBKnifeTimeHistory> > qsRegistry_;
			edb_adapter::DBKnifeTimeHistoryRMI* decorated_;


			// --- Private Constructor ---
			DBKnifeTimeHistoryRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBKnifeTimeHistoryRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBKnifeTimeHistoryRM(const DBKnifeTimeHistoryRM& rm);
			// Do not allow the default assignment operator.
			DBKnifeTimeHistoryRM operator=(const DBKnifeTimeHistoryRM& rhs);


			// Registry methods
			static util_registry::Registry<DBKnifeTimeHistoryRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBKnifeTimeHistoryRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBKnifeTimeHistoryRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBKnifeTimeHistory>* query);
			bool deregisterQ(DBQuery<DBKnifeTimeHistory>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBKNIFETIMEHISTORYRM_H
