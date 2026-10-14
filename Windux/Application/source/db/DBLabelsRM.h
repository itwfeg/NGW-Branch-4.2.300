#ifndef DBLABELSRM_H
#define DBLABELSRM_H


#include <string>


#include "edb.h"
#include DBLABELSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBLabel;
	template <class DBEntity> class DBQuery;


	class DBLabelsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBLabel>;

		public:

			// Types
			typedef DBLabel record_type;
			typedef DBQuery<DBLabel> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBLabelsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBLabel>* retrieveAll();
			DBQuery<DBLabel>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBLabel>* retrieveLabelNumListForDept(const std::string& deptNum);
			DBQuery<DBLabel>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Label record".
			DBLabel* findNumber(const std::string& number, const std::string& deptNum) const;
			DBLabel* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBLabel* createRecord() const;
			void saveRecord(const DBLabel* rec) const;
			int deleteRecord(const DBLabel* rec) const;         // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBLabel> > qsRegistry_;
			edb_adapter::DBLabelsRMI* decorated_;


			// --- Private Constructor ---
			DBLabelsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBLabelsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBLabelsRM(const DBLabelsRM& rm);
			// Do not allow the default assignment operator.
			DBLabelsRM operator=(const DBLabelsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBLabelsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBLabelsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBLabelsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBLabel>* query);
			bool deregisterQ(DBQuery<DBLabel>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBLABELSRM_H
