#ifndef DBCATEGORYSTATISTICSRM_H
#define DBCATEGORYSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBCATEGORYSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCategoryStatistic;
	class DBPresentationMethod;
	class DBCategory;
	template <class DBEntity> class DBQuery;


	class DBCategoryStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCategoryStatistic>;

		public:

			// Types
			typedef DBCategoryStatistic record_type;
			typedef DBQuery<DBCategoryStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCategoryStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCategoryStatistic>* retrieveAll();
			DBQuery<DBCategoryStatistic>* retrieveLinkedtoCategory(const DBCategory* cat);
			DBQuery<DBCategoryStatistic>* retrieveMostSelected(const std::string& language, int hour);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CategoryStatistic record".
			DBCategoryStatistic* find(const DBCategory* cat, const std::string& language, const std::string& hour) const;

			DBCategoryStatistic* createRecord(const DBCategory* cat) const;
			void saveRecord(const DBCategoryStatistic* rec) const;
			int deleteRecord(const DBCategoryStatistic* rec) const;   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCategoryStatistic> > qsRegistry_;
			edb_adapter::DBCategoryStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBCategoryStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoryStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCategoryStatisticsRM(const DBCategoryStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBCategoryStatisticsRM operator=(const DBCategoryStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCategoryStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCategoryStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCategoryStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCategoryStatistic>* query);
			bool deregisterQ(DBQuery<DBCategoryStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORYSTATISTICSRM_H
