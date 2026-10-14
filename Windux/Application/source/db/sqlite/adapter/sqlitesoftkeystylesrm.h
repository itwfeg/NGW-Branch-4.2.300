#ifndef SQLITESOFTKEYSTYLESRM_H
#define SQLITESOFTKEYSTYLESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSOFTKEYSTYLESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteSoftKeyStylesRM : public DBSoftKeyStylesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSoftKeyStylesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyStylesRMI* d = 0);
			~SqliteSoftKeyStylesRM();


			// --- Processing methods ---
			// Methods from DBSoftKeyStylesRMI
			DBQueryI<DBSoftKeyStyleI>* retrieveAll();
			DBQueryI<DBSoftKeyStyleI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBSoftKeyStyleI* findNumber(const std::string& number);

			DBSoftKeyStyleI* createRecord(const std::string& number);
			void saveRecord(const DBSoftKeyStyleI* rec);
			int deleteRecord(const DBSoftKeyStyleI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSoftKeyStylesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSoftKeyStylesRM(const SqliteSoftKeyStylesRM& rm);
			// Do not allow the default assignment operator.
			SqliteSoftKeyStylesRM operator=(const SqliteSoftKeyStylesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYSTYLESRM_H
