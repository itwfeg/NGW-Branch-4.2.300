#ifndef DBOPERATORSRM_H
#define DBOPERATORSRM_H


#include <string>


#include "edb.h"
#include DBOPERATORSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBOperator;
	template <class DBEntity> class DBQuery;


	class DBOperatorsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBOperator>;

		public:

			// Types
			typedef DBOperator record_type;
			typedef DBQuery<DBOperator> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBOperatorsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// Processing methods for retrieving data from the database
			DBQuery<DBOperator>* retrieveAll();
			DBQuery<DBOperator>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// Processing methods for other database operations
			DBOperator* findNumber(const std::string& number) const;
			DBOperator* find(const std::string& key) const { return findNumber(key); }
			DBOperator* findRFID(const std::string& id) const;

			DBOperator* createRecord() const;
			void saveRecord(const DBOperator* rec) const;
			int deleteRecord(const DBOperator* rec) const;      // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBOperator> > qsRegistry_;
			edb_adapter::DBOperatorsRMI* decorated_;


			// The constructor is only accessible by friend class DBFactory
			DBOperatorsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBOperatorsRMI* d);

			// Do not allow the default copy constructor.
			DBOperatorsRM(const DBOperatorsRM& rm);
			// Do not allow the default assignment operator.
			DBOperatorsRM operator=(const DBOperatorsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBOperatorsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBOperatorsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBOperatorsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBOperator>* query);
			bool deregisterQ(DBQuery<DBOperator>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBOPERATORSRM_H
