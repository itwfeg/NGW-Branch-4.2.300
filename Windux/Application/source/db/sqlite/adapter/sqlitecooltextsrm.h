#ifndef SQLITECOOLTEXTSRM_H
#define SQLITECOOLTEXTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLTEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolTextsRM : public DBCoolTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolTextsRMI* d = 0);
			~SqliteCoolTextsRM();


			// --- Processing methods ---
			// Methods from DBCoolTextsRMI
			DBQueryI<DBCoolTextI>* retrieveAll();
			DBQueryI<DBCoolTextI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBCoolTextI* findNumber(const std::string& number, const std::string& deptNum);

			DBCoolTextI* createRecord();
			void saveRecord(const DBCoolTextI* rec);
			int deleteRecord(const DBCoolTextI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolTextsRM(const SqliteCoolTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolTextsRM operator=(const SqliteCoolTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLTEXTSRM_H
