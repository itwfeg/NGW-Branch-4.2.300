#ifndef SQLITECOOLLISTSRM_H
#define SQLITECOOLLISTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLLISTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolListsRM : public DBCoolListsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolListsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolListsRMI* d = 0);
			~SqliteCoolListsRM();


			// --- Processing methods ---
			// Methods from DBCoolListsRMI
			DBQueryI<DBCoolListI>* retrieveAll();
			DBQueryI<DBCoolListI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolListI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBCoolListI* findNumber(const std::string& number, const std::string& deptNum);
			DBCoolListI* find(const DBProductI* prod);
			DBCoolListI* find(const DBCoolListTextI* text);
			DBCoolListI* findFirstLink(const std::string& link);

			DBCoolListI* createRecord();
			void saveRecord(const DBCoolListI* rec);
			int deleteRecord(const DBCoolListI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolListsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolListsRM(const SqliteCoolListsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolListsRM operator=(const SqliteCoolListsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLLISTSRM_H
