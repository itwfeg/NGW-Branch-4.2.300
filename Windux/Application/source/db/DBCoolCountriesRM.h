#ifndef DBCOOLCOUNTRIESRM_H
#define DBCOOLCOUNTRIESRM_H


#include <string>


#include "edb.h"
#include DBCOOLCOUNTRIESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolCountry;
	class DBCoolCategoryCountry;
	template <class DBEntity> class DBQuery;


	class DBCoolCountriesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolCountry>;

		public:

			// Types
			typedef DBCoolCountry record_type;
			typedef DBQuery<DBCoolCountry> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolCountriesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolCountry>* retrieveAll();
			DBQuery<DBCoolCountry>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolCountry>* retrieveAllCategories(const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the CoolCountry record".
			DBCoolCountry* findCountry(const std::string& name, const std::string& deptNum) const;
			DBCoolCountry* find(const std::string& key, const std::string& deptNum) const { return findCountry(key, deptNum); }
			DBCoolCountry* find(const DBCoolCategoryCountry* country);

			DBCoolCountry* createRecord() const;
			void saveRecord(const DBCoolCountry* rec) const;
			int deleteRecord(const DBCoolCountry* rec) const;   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolCountry> > qsRegistry_;
			edb_adapter::DBCoolCountriesRMI* decorated_;


			// --- Private Constructor ---
			DBCoolCountriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCountriesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolCountriesRM(const DBCoolCountriesRM& rm);
			// Do not allow the default assignment operator.
			DBCoolCountriesRM operator=(const DBCoolCountriesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolCountriesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;   
			static bool registerRM(DBCoolCountriesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolCountriesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolCountry>* query);
			bool deregisterQ(DBQuery<DBCoolCountry>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCOUNTRIESRM_H
