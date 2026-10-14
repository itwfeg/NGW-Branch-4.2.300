#ifndef SQLITECOOLCATEGORYCOUNTRIESRM_H
#define SQLITECOOLCATEGORYCOUNTRIESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLCATEGORYCOUNTRIESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolCategoryCountriesRM : public DBCoolCategoryCountriesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolCategoryCountriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryCountriesRMI* d = 0);
			~SqliteCoolCategoryCountriesRM();


			// --- Processing methods ---
			// Methods from DBCoolCategoryCountriesRMI
			DBQueryI<DBCoolCategoryCountryI>* retrieveAll();
			DBQueryI<DBCoolCategoryCountryI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolCategoryCountryI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQueryI<DBCoolCategoryCountryI>* retrieveLinkedtoCoolCountry(const DBCoolCountryI* country);

			DBCoolCategoryCountryI* find(const std::string& category, const DBCoolCountryI* country);

			DBCoolCategoryCountryI* createRecord(const std::string& category, const DBCoolCountryI* country);
			void saveRecord(const DBCoolCategoryCountryI* rec);
			int deleteRecord(const DBCoolCategoryCountryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolCategoryCountriesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryCountriesRM(const SqliteCoolCategoryCountriesRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryCountriesRM operator=(const SqliteCoolCategoryCountriesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYCOUNTRIESRM_H
