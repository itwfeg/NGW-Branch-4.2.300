#ifndef DBCOOLCATEGORYCOUNTRIESRM_H
#define DBCOOLCATEGORYCOUNTRIESRM_H


#include <string>
       

#include "edb.h"
#include DBCOOLCATEGORYCOUNTRIESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolCategoryCountry;
	class DBCoolCategory;
	class DBCoolCountry;
	template <class DBEntity> class DBQuery;


	class DBCoolCategoryCountriesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolCategoryCountry>;

		public:

			// Types
			typedef DBCoolCategoryCountry record_type;
			typedef DBQuery<DBCoolCategoryCountry> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolCategoryCountriesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolCategoryCountry>* retrieveAll();
			DBQuery<DBCoolCategoryCountry>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolCategoryCountry>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQuery<DBCoolCategoryCountry>* retrieveLinkedtoCoolCountry(const DBCoolCountry* country);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CoolCategoryCountry record".
			DBCoolCategoryCountry* find(const std::string& category, const DBCoolCountry* country);

			DBCoolCategoryCountry* createRecord(const std::string& category, const DBCoolCountry* country) const;
			void saveRecord(const DBCoolCategoryCountry* rec) const;
			int deleteRecord(const DBCoolCategoryCountry* rec) const;     // returns # of deleted records
			int deleteAll() const;                                        // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                         // returns # of existing records
			int calculateAverageSpace() const;                            // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBCoolCategoryCountry> > qsRegistry_;
			edb_adapter::DBCoolCategoryCountriesRMI* decorated_;


			// --- Private Constructor ---
			DBCoolCategoryCountriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryCountriesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolCategoryCountriesRM(const DBCoolCategoryCountriesRM& rm);
			// Do not allow the default assignment operator.
			DBCoolCategoryCountriesRM operator=(const DBCoolCategoryCountriesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolCategoryCountriesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolCategoryCountriesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolCategoryCountriesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolCategoryCountry>* query);
			bool deregisterQ(DBQuery<DBCoolCategoryCountry>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYCOUNTRIESRM_H
