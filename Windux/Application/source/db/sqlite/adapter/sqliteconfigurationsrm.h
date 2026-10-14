#ifndef SQLITECONFIGURATIONSRM_H
#define SQLITECONFIGURATIONSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCONFIGURATIONSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteConfigurationsRM : public DBConfigurationsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteConfigurationsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBConfigurationsRMI* d = 0);
			~SqliteConfigurationsRM();


			// --- Processing methods ---
			// Methods from DBConfigurationsRMI
			DBQueryI<DBConfigurationI>* retrieveAll();

			DBConfigurationI* findName(const std::string& name);

			DBConfigurationI* createRecord();
			void saveRecord(const DBConfigurationI* rec);
			int deleteRecord(const DBConfigurationI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBConfigurationsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteConfigurationsRM(const SqliteConfigurationsRM& rm);
			// Do not allow the default assignment operator.
			SqliteConfigurationsRM operator=(const SqliteConfigurationsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECONFIGURATIONSRM_H
