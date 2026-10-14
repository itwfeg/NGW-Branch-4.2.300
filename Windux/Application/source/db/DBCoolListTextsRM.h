#ifndef DBCOOLLISTTEXTSRM_H
#define DBCOOLLISTTEXTSRM_H

#include <evolution.h>
#include <string>
       

#include "edb.h"
#include DBCOOLLISTTEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolListText;
	class DBCoolList;
	template <class DBEntity> class DBQuery;


	class DBCoolListTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolListText>;

		public:

			// Types
			typedef DBCoolListText record_type;
			typedef DBQuery<DBCoolListText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolListTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolListText>* retrieveAll();
			DBQuery<DBCoolListText>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolListText>* retrieveLink(const std::string& link);
			DBQuery<DBCoolListText>* retrieveLinkedtoCoolList(const DBCoolList* list);
			DBQuery<DBCoolListText>* retrievePreviousCoolListTexts(const std::string& link);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CoolListText record".
			DBCoolListText* find(const std::string& text, const std::string& deptNum, const DBCoolList* list);

			DBCoolListText* createRecord(const std::string& text, const DBCoolList* list, HobRecordNum deptNum) const;
			DBCoolListText* createRecordInList(const std::string& text, const DBCoolListText* listText) const;
			void saveRecord(const DBCoolListText* rec) const;
			int deleteRecord(const DBCoolListText* rec) const;       // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolListText> > qsRegistry_;
			edb_adapter::DBCoolListTextsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolListTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolListTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolListTextsRM(const DBCoolListTextsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolListTextsRM operator=(const DBCoolListTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolListTextsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolListTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolListTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolListText>* query);
			bool deregisterQ(DBQuery<DBCoolListText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLLISTTEXTSRM_H
