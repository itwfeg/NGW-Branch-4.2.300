#ifndef SQLITEPRODUCTDETAILEDSTATISTICSRM_H
#define SQLITEPRODUCTDETAILEDSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTDETAILEDSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductDetailedStatisticsRM : public DBProductDetailedStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductDetailedStatisticsRMI* d = 0);
			~SqliteProductDetailedStatisticsRM();


			// --- Processing methods ---
			// Methods from DBProductDetailedStatisticsRMI
			DBQueryI<DBProductDetailedStatisticI>* retrieveAll();
			DBQueryI<DBProductDetailedStatisticI>* retrieveLinkedtoProductStatistic(const DBProductStatisticI* stat);

			DBProductDetailedStatisticI* find(const DBPresentationMethodI* method, const DBProductStatisticI* stat);

			DBProductDetailedStatisticI* createRecord();
			void saveRecord(const DBProductDetailedStatisticI* rec);
			int deleteRecord(const DBProductDetailedStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductDetailedStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductDetailedStatisticsRM(const SqliteProductDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductDetailedStatisticsRM operator=(const SqliteProductDetailedStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTDETAILEDSTATISTICSRM_H
