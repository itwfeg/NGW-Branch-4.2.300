#ifndef SQLITECOOLPRETEXTSRM_H
#define SQLITECOOLPRETEXTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLPRETEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolPreTextsRM : public DBCoolPreTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolPreTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolPreTextsRMI* d = 0);
			~SqliteCoolPreTextsRM();


			// --- Processing methods ---
			// Methods from DBCoolPreTextsRMI
			DBQueryI<DBCoolPreTextI>* retrieveAll();
			DBQueryI<DBCoolPreTextI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolPreTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQueryI<DBCoolPreTextI>* retrieveAllCategories(const std::string& deptNum);

			DBCoolPreTextI* findNumber(const std::string& number, const std::string& deptNum);

			DBCoolPreTextI* createRecord();
			void saveRecord(const DBCoolPreTextI* rec);
			int deleteRecord(const DBCoolPreTextI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolPreTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolPreTextsRM(const SqliteCoolPreTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolPreTextsRM operator=(const SqliteCoolPreTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLPRETEXTSRM_H
