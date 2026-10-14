#ifndef SQLITEROMLABELSRM_H
#define SQLITEROMLABELSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBROMLABELSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteROMLabelsRM : public DBROMLabelsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteROMLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBROMLabelsRMI* d = 0);
			~SqliteROMLabelsRM();


			// --- Processing methods ---
			// Methods from DBROMLabelsRMI
			DBQueryI<DBROMLabelI>* retrieveAll();
			DBQueryI<DBROMLabelI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBROMLabelI* findNumber(const std::string& number);

			DBROMLabelI* createRecord();
			void saveRecord(const DBROMLabelI* rec);
			int deleteRecord(const DBROMLabelI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBROMLabelsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteROMLabelsRM(const SqliteROMLabelsRM& rm);
			// Do not allow the default assignment operator.
			SqliteROMLabelsRM operator=(const SqliteROMLabelsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEROMLABELSRM_H
