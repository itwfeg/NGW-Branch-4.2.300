#ifndef SQLITECUTTINGTESTSRM_H
#define SQLITECUTTINGTESTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCUTTINGTESTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;

	
	class SqliteCuttingTestsRM : public DBCuttingTestsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCuttingTestsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCuttingTestsRMI* d = 0);
			~SqliteCuttingTestsRM();


			// --- Processing methods ---
			// Methods from DBCuttingTestsRMI
			DBQueryI<DBCuttingTestI>* retrieveAll();
			DBQueryI<DBCuttingTestI>* retrieveByOperator(const std::string& opNum);

			DBCuttingTestI* find(const std::string& id);

			DBCuttingTestI* createRecord(const std::string& testNumber);
			void saveRecord(const DBCuttingTestI* rec);
			int deleteRecord(const DBCuttingTestI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCuttingTestsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCuttingTestsRM(const SqliteCuttingTestsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCuttingTestsRM operator=(const SqliteCuttingTestsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECUTTINGTESTSRM_H
