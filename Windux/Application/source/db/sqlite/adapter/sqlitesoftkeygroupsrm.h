#ifndef SQLITESOFTKEYGROUPSRM_H
#define SQLITESOFTKEYGROUPSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSOFTKEYGROUPSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteSoftKeyGroupsRM : public DBSoftKeyGroupsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSoftKeyGroupsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyGroupsRMI* d = 0);
			~SqliteSoftKeyGroupsRM();


			// --- Processing methods ---
			// Methods from DBSoftKeyGroupsRMI
			DBQueryI<DBSoftKeyGroupI>* retrieveAll();
			DBQueryI<DBSoftKeyGroupI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBSoftKeyGroupI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBSoftKeyGroupI* findNumber(const std::string& number, const std::string& deptNum);

			DBSoftKeyGroupI* createRecord(const std::string& number);
			void saveRecord(const DBSoftKeyGroupI* rec);
			int deleteRecord(const DBSoftKeyGroupI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSoftKeyGroupsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSoftKeyGroupsRM(const SqliteSoftKeyGroupsRM& rm);
			// Do not allow the default assignment operator.
			SqliteSoftKeyGroupsRM operator=(const SqliteSoftKeyGroupsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYGROUPSRM_H
