#ifndef DBCOOLLISTSRM_H
#define DBCOOLLISTSRM_H


#include <string>


#include "edb.h"
#include DBCOOLLISTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProduct;
	class DBCoolList;
	class DBCoolListText;
	template <class DBEntity> class DBQuery;


	class DBCoolListsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolList>;

		public:

			// Types
			typedef DBCoolList record_type;
			typedef DBQuery<DBCoolList> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolListsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolList>* retrieveAll();
			DBQuery<DBCoolList>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolList>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the CoolList record".
			DBCoolList* findNumber(const std::string& number, const std::string& deptNum) const;
			DBCoolList* findFirstLink(const std::string& link) const;
			DBCoolList* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }
			DBCoolList* find(const DBProduct* prod) const;
			DBCoolList* find(const DBCoolListText* text) const;

			DBCoolList* createRecord() const;
			void saveRecord(const DBCoolList* rec) const;
			int deleteRecord(const DBCoolList* rec) const;           // returns # of deleted records
			int deleteAll() const;                                   // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                    // returns # of existing records
			int calculateAverageSpace() const;                       // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBCoolList> > qsRegistry_;
			edb_adapter::DBCoolListsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolListsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolListsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolListsRM(const DBCoolListsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolListsRM operator=(const DBCoolListsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolListsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolListsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolListsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolList>* query);
			bool deregisterQ(DBQuery<DBCoolList>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLLISTSRM_H
