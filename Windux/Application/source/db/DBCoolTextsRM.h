#ifndef DBCOOLTEXTSRM_H
#define DBCOOLTEXTSRM_H


#include <string>


#include "edb.h"
#include DBCOOLTEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolText;
	template <class DBEntity> class DBQuery;


	class DBCoolTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolText>;

		public:

			// Types
			typedef DBCoolText record_type;
			typedef DBQuery<DBCoolText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolText>* retrieveAll();
			DBQuery<DBCoolText>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolText>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Text record".
			DBCoolText* findNumber(const std::string& number, const std::string& deptNum) const;
			DBCoolText* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBCoolText* createRecord() const;
			void saveRecord(const DBCoolText* rec) const;
			int deleteRecord(const DBCoolText* rec) const;      // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolText> > qsRegistry_;
			edb_adapter::DBCoolTextsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolTextsRM(const DBCoolTextsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolTextsRM operator=(const DBCoolTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolTextsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolText>* query);
			bool deregisterQ(DBQuery<DBCoolText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLTEXTSRM_H
