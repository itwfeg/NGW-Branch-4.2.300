#ifndef DBPRESENTATIONMETHODSTATISTICSRM_H
#define DBPRESENTATIONMETHODSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBPRESENTATIONMETHODSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBPresentationMethodStatistic;
	class DBPresentationMethod;
	template <class DBEntity> class DBQuery;


	class DBPresentationMethodStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBPresentationMethodStatistic>;

		public:

			// Types
			typedef DBPresentationMethodStatistic record_type;
			typedef DBQuery<DBPresentationMethodStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBPresentationMethodStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBPresentationMethodStatistic>* retrieveAll();
			DBQuery<DBPresentationMethodStatistic>* retrieveLinkedtoPresentationMethod(const DBPresentationMethod* method);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the PresentationMethodStatistic record".
			DBPresentationMethodStatistic* find(const DBPresentationMethod* method, const std::string& lang, const std::string& hour) const;

			DBPresentationMethodStatistic* createRecord(const DBPresentationMethod* method) const;
			void saveRecord(const DBPresentationMethodStatistic* rec) const;
			int deleteRecord(const DBPresentationMethodStatistic* rec) const;    // returns # of deleted records
			int deleteAll() const;                                               // returns # of deleted records
			int countAll() const;                                                // returns # of existing records
			int calculateAverageSpace() const;                                   // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBPresentationMethodStatistic> > qsRegistry_;
			edb_adapter::DBPresentationMethodStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBPresentationMethodStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBPresentationMethodStatisticsRM(const DBPresentationMethodStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBPresentationMethodStatisticsRM operator=(const DBPresentationMethodStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBPresentationMethodStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBPresentationMethodStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBPresentationMethodStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBPresentationMethodStatistic>* query);
			bool deregisterQ(DBQuery<DBPresentationMethodStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHODSTATISTICSRM_H
