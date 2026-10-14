#ifndef DBCATEGORIESRM_H
#define DBCATEGORIESRM_H


#include <string>


#include "edb.h"
#include DBCATEGORIESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCategory;
	template <class DBEntity> class DBQuery;


	class DBCategoriesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCategory>;

		public:

			// Types
			typedef DBCategory record_type;
			typedef DBQuery<DBCategory> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCategoriesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCategory>* retrieveAll();
			DBQuery<DBCategory>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCategory>* retrieveAllByName();
			DBQuery<DBCategory>* retrieveAllByName(const std::string& deptNum);
			DBQuery<DBCategory>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQuery<DBCategory>* retrieveChildCategories(const DBCategory* ca);
			DBQuery<DBCategory>* retrieveAllSubcategories(const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Category record".
			DBCategory* findNumber(const std::string& number, const std::string& deptNum) const;
			DBCategory* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBCategory* createRecord() const;
			void saveRecord(const DBCategory* rec) const;
			int deleteRecord(const DBCategory* rec) const;      // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCategory> > qsRegistry_;
			edb_adapter::DBCategoriesRMI* decorated_;


			// --- Private Constructor ---
			DBCategoriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoriesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCategoriesRM(const DBCategoriesRM& rm);
			// Do not allow the default assignment operator.
			DBCategoriesRM operator=(const DBCategoriesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCategoriesRM>* _registry_;
			static bool registerRM(DBCategoriesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCategoriesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();
			// The following mutex is to prevent the creation of two registry objects
			static util_thread::Mutex<> _registryMutex_;   

			void disable();
			bool registerQ(DBQuery<DBCategory>* query);
			bool deregisterQ(DBQuery<DBCategory>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORIESRM_H
