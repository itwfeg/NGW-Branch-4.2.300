#ifndef DBCOOLPRETEXTSRM_H
#define DBCOOLPRETEXTSRM_H


#include <string>


#include "edb.h"
#include DBCOOLPRETEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolPreText;
	class DBCoolCategory;
	template <class DBEntity> class DBQuery;


	class DBCoolPreTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolPreText>;

		public:

			// Types
			typedef DBCoolPreText record_type;
			typedef DBQuery<DBCoolPreText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolPreTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolPreText>* retrieveAll();
			DBQuery<DBCoolPreText>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolPreText>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQuery<DBCoolPreText>* retrieveAllCategories(const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Category record".
			DBCoolPreText* findNumber(const std::string& number, const std::string& deptNum) const;
			DBCoolPreText* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBCoolPreText* createRecord() const;
			void saveRecord(const DBCoolPreText* rec) const;
			int deleteRecord(const DBCoolPreText* rec) const;   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolPreText> > qsRegistry_;
			edb_adapter::DBCoolPreTextsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolPreTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolPreTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolPreTextsRM(const DBCoolPreTextsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolPreTextsRM operator=(const DBCoolPreTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolPreTextsRM>* _registry_;
			static bool registerRM(DBCoolPreTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolPreTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();
			// The following mutex is to prevent the creation of two registry objects
			static util_thread::Mutex<> _registryMutex_;   

			void disable();
			bool registerQ(DBQuery<DBCoolPreText>* query);
			bool deregisterQ(DBQuery<DBCoolPreText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLPRETEXTSRM_H
