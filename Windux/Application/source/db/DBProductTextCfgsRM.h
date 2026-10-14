#ifndef DBPRODUCTTEXTCFGSRM_H
#define DBPRODUCTTEXTCFGSRM_H


#include <string>


#include "edb.h"
#include DBPRODUCTTEXTCFGSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProductTextCfg;
	template <class DBEntity> class DBQuery;


	class DBProductTextCfgsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProductTextCfg>;

		public:

			// Types
			typedef DBProductTextCfg record_type;
			typedef DBQuery<DBProductTextCfg> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductTextCfgsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProductTextCfg>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the ProductTextCfg record".
			DBProductTextCfg* findNumber(int type = 0) const;
			//DBProductTextCfg* find(const std::string& key) const { return findNumber(key); }

			DBProductTextCfg* createRecord(int type = 0) const;
			void saveRecord(const DBProductTextCfg* rec) const;
			int deleteRecord(const DBProductTextCfg* rec) const;      // returns # of deleted records
			int deleteAll() const;                                    // returns # of deleted records
			int countAll() const;                                     // returns # of existing records
			int calculateAverageSpace() const;                     // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);

			int getType() const { return type_; }
			void setType(int type) { type_ = type; }


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBProductTextCfg> > qsRegistry_;
			edb_adapter::DBProductTextCfgsRMI* decorated_;

			int type_;


			// --- Private Constructor ---
			DBProductTextCfgsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductTextCfgsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductTextCfgsRM(const DBProductTextCfgsRM& rm);
			// Do not allow the default assignment operator.
			DBProductTextCfgsRM operator=(const DBProductTextCfgsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductTextCfgsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductTextCfgsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductTextCfgsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProductTextCfg>* query);
			bool deregisterQ(DBQuery<DBProductTextCfg>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTTEXTCFGSRM_H
