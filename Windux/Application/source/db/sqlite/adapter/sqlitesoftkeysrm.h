#ifndef SQLITESOFTKEYSRM_H
#define SQLITESOFTKEYSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSOFTKEYSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteSoftKeysRM : public DBSoftKeysRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSoftKeysRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeysRMI* d = 0);
			~SqliteSoftKeysRM();


			// --- Processing methods ---
			// Methods from DBSoftKeysRMI
			DBQueryI<DBSoftKeyI>* retrieveAll();
			DBQueryI<DBSoftKeyI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBSoftKeyI>* retrieveGroup(const std::string& groupNumber, const std::string& deptNumber);
			DBQueryI<DBSoftKeyI>* retrieveGroup(const DBSoftKeyGroupI* skg);

			DBSoftKeyI* findSoftKey(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp);

			DBSoftKeyI* createRecord(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp);
			void saveRecord(const DBSoftKeyI* rec);
			int deleteRecord(const DBSoftKeyI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSoftKeysRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSoftKeysRM(const SqliteSoftKeysRM& rm);
			// Do not allow the default assignment operator.
			SqliteSoftKeysRM operator=(const SqliteSoftKeysRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYSRM_H
