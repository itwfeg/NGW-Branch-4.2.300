#ifndef DBRECALLSRM_H
#define DBRECALLSRM_H


#include <string>


#include "edb.h"
#include DBRECALLSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBRecall;
	template <class DBEntity> class DBQuery;


	class DBRecallsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBRecall>;

		public:

			// Types
			typedef DBRecall record_type;
			typedef DBQuery<DBRecall> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBRecallsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// Processing methods for retrieving data from the database
			DBQuery<DBRecall>* retrieveAll();
			DBQuery<DBRecall>* retrieveAcknowledged();
			DBQuery<DBRecall>* retrieveUnacknowledged();

			//DBQuery<DBRecall>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// Processing methods for other database operations
			DBRecall* findNumber(const std::string& number) const;
			DBRecall* findName(const std::string& name) const;
			DBRecall* find(const std::string& key) const { return findNumber(key); }

			DBRecall* createRecord() const;
			void saveRecord(const DBRecall* rec) const;
			int deleteRecord(const DBRecall* rec) const;        // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBRecall> > qsRegistry_;
			edb_adapter::DBRecallsRMI* decorated_;


			// The constructor is only accessible by friend class DBFactory
			DBRecallsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBRecallsRMI* d);

			// Do not allow the default copy constructor.
			DBRecallsRM(const DBRecallsRM& rm);
			// Do not allow the default assignment operator.
			DBRecallsRM operator=(const DBRecallsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBRecallsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBRecallsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBRecallsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBRecall>* query);
			bool deregisterQ(DBQuery<DBRecall>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBRECALLSRM_H
