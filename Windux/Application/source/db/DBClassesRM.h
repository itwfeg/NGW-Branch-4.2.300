#ifndef DBCLASSESRM_H
#define DBCLASSESRM_H


#include <string>


#include "edb.h"
#include DBCLASSESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBClass;
	template <class DBEntity> class DBQuery;


	class DBClassesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBClass>;

		public:

			// Types
			typedef DBClass record_type;
			typedef DBQuery<DBClass> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBClassesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBClass>* retrieveAll();
			DBQuery<DBClass>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBClass>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQuery<DBClass>* retrieveChildClasses(const DBClass* rec);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Class record".
			DBClass* findNumber(const std::string& number, const std::string& deptNum) const;
			DBClass* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBClass* createRecord() const;
			void saveRecord(const DBClass* rec) const;
			int deleteRecord(const DBClass* rec) const;         // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBClass> > qsRegistry_;
			edb_adapter::DBClassesRMI* decorated_;


			// --- Private Constructor ---
			DBClassesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBClassesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBClassesRM(const DBClassesRM& rm);
			// Do not allow the default assignment operator.
			DBClassesRM operator=(const DBClassesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBClassesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBClassesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBClassesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBClass>* query);
			bool deregisterQ(DBQuery<DBClass>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCLASSESRM_H
