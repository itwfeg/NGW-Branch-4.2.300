#ifndef SQLITECOOLCOUNTRIESRM_H
#define SQLITECOOLCOUNTRIESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLCOUNTRIESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolCountriesRM : public DBCoolCountriesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolCountriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCountriesRMI* d = 0);
			~SqliteCoolCountriesRM();


			// --- Processing methods ---
			// Methods from DBCoolCountriesRMI
			DBQueryI<DBCoolCountryI>* retrieveAll();
			DBQueryI<DBCoolCountryI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolCountryI>* retrieveAllCategories(const std::string& deptNum);

			DBCoolCountryI* findCountry(const std::string& name, const std::string& deptNum);
			DBCoolCountryI* find(const DBCoolCategoryCountryI* country);

			DBCoolCountryI* createRecord();
			void saveRecord(const DBCoolCountryI* rec);
			int deleteRecord(const DBCoolCountryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolCountriesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolCountriesRM(const SqliteCoolCountriesRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolCountriesRM operator=(const SqliteCoolCountriesRM& rhs); // SqliteCoolCountriesRM


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCOUNTRIESRM_H
