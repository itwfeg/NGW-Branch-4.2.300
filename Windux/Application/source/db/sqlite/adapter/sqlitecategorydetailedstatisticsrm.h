#ifndef SQLITECATEGORYDETAILEDSTATISTICSRM_H
#define SQLITECATEGORYDETAILEDSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCATEGORYDETAILEDSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCategoryDetailedStatisticsRM : public DBCategoryDetailedStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCategoryDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoryDetailedStatisticsRMI* d = 0);
			~SqliteCategoryDetailedStatisticsRM();


			// --- Processing methods ---
			// Methods from DBCategoryDetailedStatisticsRMI
			DBQueryI<DBCategoryDetailedStatisticI>* retrieveAll();
			DBQueryI<DBCategoryDetailedStatisticI>* retrieveLinkedtoCategoryStatistic(const DBCategoryStatisticI* stat);

			DBCategoryDetailedStatisticI* find(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat);

			DBCategoryDetailedStatisticI* createRecord(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat);
			void saveRecord(const DBCategoryDetailedStatisticI* rec);
			int deleteRecord(const DBCategoryDetailedStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCategoryDetailedStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCategoryDetailedStatisticsRM(const SqliteCategoryDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCategoryDetailedStatisticsRM operator=(const SqliteCategoryDetailedStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORYDETAILEDSTATISTICSRM_H
