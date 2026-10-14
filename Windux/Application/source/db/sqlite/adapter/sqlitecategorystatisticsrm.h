#ifndef SQLITECATEGORYSTATISTICSRM_H
#define SQLITECATEGORYSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCATEGORYSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCategoryStatisticsRM : public DBCategoryStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCategoryStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoryStatisticsRMI* d = 0);
			~SqliteCategoryStatisticsRM();


			// --- Processing methods ---
			// Methods from DBCategoryStatisticsRMI
			DBQueryI<DBCategoryStatisticI>* retrieveAll();
			DBQueryI<DBCategoryStatisticI>* retrieveLinkedtoCategory(const DBCategoryI* cat);
			DBQueryI<DBCategoryStatisticI>* retrieveMostSelected(const std::string& language, int hour);

			DBCategoryStatisticI* find(const DBCategoryI* cat, const std::string& language, const std::string& hour);

			DBCategoryStatisticI* createRecord(const DBCategoryI* cat);
			void saveRecord(const DBCategoryStatisticI* rec);
			int deleteRecord(const DBCategoryStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCategoryStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCategoryStatisticsRM(const SqliteCategoryStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCategoryStatisticsRM operator=(const SqliteCategoryStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORYSTATISTICSRM_H
