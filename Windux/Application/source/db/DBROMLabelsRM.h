#ifndef DBROMLABELSRM_H
#define DBROMLABELSRM_H


#include <string>


#include "edb.h"
#include DBROMLABELSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBROMLabel;
	template <class DBEntity> class DBQuery;


	class DBROMLabelsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBROMLabel>;

		public:

			// Types
			typedef DBROMLabel record_type;
			typedef DBQuery<DBROMLabel> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBROMLabelsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBROMLabel>* retrieveAll();
			DBQuery<DBROMLabel>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the ROMLabel record".
			DBROMLabel* findNumber(const std::string& number) const;
			DBROMLabel* find(const std::string& key) const { return findNumber(key); }

			DBROMLabel* createRecord() const;
			void saveRecord(const DBROMLabel* rec) const;
			int deleteRecord(const DBROMLabel* rec) const;      // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBROMLabel> > qsRegistry_;
			edb_adapter::DBROMLabelsRMI* decorated_;


			// --- Private Constructor ---
			DBROMLabelsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBROMLabelsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBROMLabelsRM(const DBROMLabelsRM& rm);
			// Do not allow the default assignment operator.
			DBROMLabelsRM operator=(const DBROMLabelsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBROMLabelsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBROMLabelsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBROMLabelsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBROMLabel>* query);
			bool deregisterQ(DBQuery<DBROMLabel>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBROMLABELSRM_H
