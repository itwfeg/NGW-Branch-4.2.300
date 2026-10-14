#ifndef SQLITECLASSESRM_H
#define SQLITECLASSESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCLASSESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteClassesRM : public DBClassesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteClassesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBClassesRMI* d = 0);
			~SqliteClassesRM();


			// --- Processing methods ---
			// Methods from DBClassesRMI
			DBQueryI<DBClassI>* retrieveAll();
			DBQueryI<DBClassI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBClassI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQueryI<DBClassI>* retrieveChildClasses(const std::string& number, const std::string& deptNum);

			DBClassI* findNumber(const std::string& number, const std::string& deptNum);

			DBClassI* createRecord();
			void saveRecord(const DBClassI* rec);
			int deleteRecord(const DBClassI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBClassesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteClassesRM(const SqliteClassesRM& rm);
			// Do not allow the default assignment operator.
			SqliteClassesRM operator=(const SqliteClassesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECLASSESRM_H
