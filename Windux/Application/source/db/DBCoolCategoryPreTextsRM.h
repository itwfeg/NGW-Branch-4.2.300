#ifndef DBCOOLCATEGORYPRETEXTSRM_H
#define DBCOOLCATEGORYPRETEXTSRM_H


#include <string>
       

#include "edb.h"
#include DBCOOLCATEGORYPRETEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolCategoryPreText;
	template <class DBEntity> class DBQuery;


	class DBCoolCategoryPreTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolCategoryPreText>;

		public:

			// Types
			typedef DBCoolCategoryPreText record_type;
			typedef DBQuery<DBCoolCategoryPreText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolCategoryPreTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolCategoryPreText>* retrieveAll();
			DBQuery<DBCoolCategoryPreText>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolCategoryPreText>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQuery<DBCoolCategoryPreText>* retrieveLinkedtoCoolPreText(const std::string& pretext, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CoolCategoryPreText record".
			DBCoolCategoryPreText* find(const std::string& category, const std::string& pretext, const std::string& deptNum);

			DBCoolCategoryPreText* createRecord(const std::string& category, const std::string& pretext, const std::string& deptNum) const;
			void saveRecord(const DBCoolCategoryPreText* rec) const;
			int deleteRecord(const DBCoolCategoryPreText* rec) const;     // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolCategoryPreText> > qsRegistry_;
			edb_adapter::DBCoolCategoryPreTextsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolCategoryPreTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryPreTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolCategoryPreTextsRM(const DBCoolCategoryPreTextsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolCategoryPreTextsRM operator=(const DBCoolCategoryPreTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolCategoryPreTextsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolCategoryPreTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolCategoryPreTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolCategoryPreText>* query);
			bool deregisterQ(DBQuery<DBCoolCategoryPreText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYPRETEXTSRM_H
