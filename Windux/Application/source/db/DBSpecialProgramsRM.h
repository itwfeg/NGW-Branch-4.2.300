#ifndef DBSPECIALPROGRAMSRM_H
#define DBSPECIALPROGRAMSRM_H


#include <string>


#include "edb.h"
#include DBSPECIALPROGRAMSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSpecialProgram;
	template <class DBEntity> class DBQuery;


	class DBSpecialProgramsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSpecialProgram>;

		public:

			// Types
			typedef DBSpecialProgram record_type;
			typedef DBQuery<DBSpecialProgram> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSpecialProgramsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBSpecialProgram>* retrieveAll();
			DBQuery<DBSpecialProgram>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBSpecialProgram>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the SpecialProgram record".
			DBSpecialProgram* findNumber(const std::string& number, const std::string& deptNum) const;
			DBSpecialProgram* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBSpecialProgram* createRecord() const;
			void saveRecord(const DBSpecialProgram* rec) const;
			int deleteRecord(const DBSpecialProgram* rec) const;      // returns # of deleted records
			int deleteAll() const;                                    // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                     // returns # of existing records
			int calculateAverageSpace() const;                        // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBSpecialProgram> > qsRegistry_;
			edb_adapter::DBSpecialProgramsRMI* decorated_;


			// --- Private Constructor ---
			DBSpecialProgramsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSpecialProgramsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBSpecialProgramsRM(const DBSpecialProgramsRM& rm);
			// Do not allow the default assignment operator.
			DBSpecialProgramsRM operator=(const DBSpecialProgramsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSpecialProgramsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSpecialProgramsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSpecialProgramsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSpecialProgram>* query);
			bool deregisterQ(DBQuery<DBSpecialProgram>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSPECIALPROGRAMSRM_H
