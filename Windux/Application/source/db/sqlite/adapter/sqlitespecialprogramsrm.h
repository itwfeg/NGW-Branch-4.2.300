#ifndef SQLITESPECIALPROGRAMSRM_H
#define SQLITESPECIALPROGRAMSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSPECIALPROGRAMSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteSpecialProgramsRM : public DBSpecialProgramsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSpecialProgramsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSpecialProgramsRMI* d = 0);
			~SqliteSpecialProgramsRM();


			// --- Processing methods ---
			// Methods from DBSpecialProgramsRMI
			DBQueryI<DBSpecialProgramI>* retrieveAll();
			DBQueryI<DBSpecialProgramI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBSpecialProgramI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBSpecialProgramI* findNumber(const std::string& number, const std::string& deptNum);

			DBSpecialProgramI* createRecord();
			void saveRecord(const DBSpecialProgramI* rec);
			int deleteRecord(const DBSpecialProgramI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSpecialProgramsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSpecialProgramsRM(const SqliteSpecialProgramsRM& rm);
			// Do not allow the default assignment operator.
			SqliteSpecialProgramsRM operator=(const SqliteSpecialProgramsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESPECIALPROGRAMSRM_H
