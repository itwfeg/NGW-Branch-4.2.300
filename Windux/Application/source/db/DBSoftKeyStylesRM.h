#ifndef DBSOFTKEYSTYLESRM_H
#define DBSOFTKEYSTYLESRM_H


#include <string>


#include "edb.h"
#include DBSOFTKEYSTYLESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBSoftKeyStyle;
	template <class DBEntity> class DBQuery;


	class DBSoftKeyStylesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBSoftKeyStyle>;

		public:

			// Types
			typedef DBSoftKeyStyle record_type;
			typedef DBQuery<DBSoftKeyStyle> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBSoftKeyStylesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBSoftKeyStyle>* retrieveAll();
			DBQuery<DBSoftKeyStyle>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// findSoftKeyStyle: a null return means "unable to find the SoftKeyStyle record".
			DBSoftKeyStyle* findNumber(const std::string& number) const;
			DBSoftKeyStyle* find(const std::string& number) const { return findNumber(number); }

			DBSoftKeyStyle* createRecord(const std::string& number) const;
			void saveRecord(const DBSoftKeyStyle* rec) const;
			int deleteRecord(const DBSoftKeyStyle* rec) const;        // returns # of deleted records
			int deleteAll() const;                                    // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBSoftKeyStyle> > qsRegistry_;
			edb_adapter::DBSoftKeyStylesRMI* decorated_;


			// --- Private Constructor ---
			DBSoftKeyStylesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyStylesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBSoftKeyStylesRM(const DBSoftKeyStylesRM& rm);
			// Do not allow the default assignment operator.
			DBSoftKeyStylesRM operator=(const DBSoftKeyStylesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBSoftKeyStylesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBSoftKeyStylesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBSoftKeyStylesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBSoftKeyStyle>* query);
			bool deregisterQ(DBQuery<DBSoftKeyStyle>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSOFTKEYSTYLESRM_H
