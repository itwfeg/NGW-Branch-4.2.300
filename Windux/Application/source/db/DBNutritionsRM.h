#ifndef DBNUTRITIONSRM_H
#define DBNUTRITIONSRM_H


#include <string>


#include "edb.h"
#include DBNUTRITIONSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBNutrition;
	template <class DBEntity> class DBQuery;


	class DBNutritionsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBNutrition>;

		public:

			// Types
			typedef DBNutrition record_type;
			typedef DBQuery<DBNutrition> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBNutritionsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBNutrition>* retrieveAll();
			DBQuery<DBNutrition>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBNutrition>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Nutrition record".
			DBNutrition* findNumber(const std::string& number, const std::string& deptNum) const;
			DBNutrition* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBNutrition* createRecord() const;
			void saveRecord(const DBNutrition* rec) const;
			int deleteRecord(const DBNutrition* rec) const;     // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBNutrition> > qsRegistry_;
			edb_adapter::DBNutritionsRMI* decorated_;


			// --- Private Constructor ---
			DBNutritionsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBNutritionsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBNutritionsRM(const DBNutritionsRM& rm);
			// Do not allow the default assignment operator.
			DBNutritionsRM operator=(const DBNutritionsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBNutritionsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBNutritionsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBNutritionsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBNutrition>* query);
			bool deregisterQ(DBQuery<DBNutrition>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBNUTRITIONSRM_H
