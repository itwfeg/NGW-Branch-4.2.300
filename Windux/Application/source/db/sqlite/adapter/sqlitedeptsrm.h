#ifndef SQLITEDEPTSRM_H
#define SQLITEDEPTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBDEPTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteDeptsRM : public DBDeptsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteDeptsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBDeptsRMI* d = 0);
			~SqliteDeptsRM();


			// --- Processing methods ---
			// Methods from DBDeptsRMI
			DBQueryI<DBDeptI>* retrieveAll();
			DBQueryI<DBDeptI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBDeptI* findNumber(const std::string& number);

			DBDeptI* createRecord();
			void saveRecord(const DBDeptI* rec);
			int deleteRecord(const DBDeptI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBDeptsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteDeptsRM(const SqliteDeptsRM& rm);
			// Do not allow the default assignment operator.
			SqliteDeptsRM operator=(const SqliteDeptsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEDEPTSRM_H
