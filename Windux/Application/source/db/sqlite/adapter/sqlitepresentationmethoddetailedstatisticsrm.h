#ifndef SQLITEPRESENTATIONMETHODDETAILEDSTATISTICSRM_H
#define SQLITEPRESENTATIONMETHODDETAILEDSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRESENTATIONMETHODDETAILEDSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqlitePresentationMethodDetailedStatisticsRM : public DBPresentationMethodDetailedStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqlitePresentationMethodDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodDetailedStatisticsRMI* d = 0);
			~SqlitePresentationMethodDetailedStatisticsRM();


			// --- Processing methods ---
			// Methods from DBPresentationMethodDetailedStatisticsRMI
			DBQueryI<DBPresentationMethodDetailedStatisticI>* retrieveAll();
			DBQueryI<DBPresentationMethodDetailedStatisticI>* retrieveLinkedtoPresentationMethodStatistic(const DBPresentationMethodStatisticI* stat);

			DBPresentationMethodDetailedStatisticI* find(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat);

			DBPresentationMethodDetailedStatisticI* createRecord(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat);
			void saveRecord(const DBPresentationMethodDetailedStatisticI* rec);
			int deleteRecord(const DBPresentationMethodDetailedStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBPresentationMethodDetailedStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqlitePresentationMethodDetailedStatisticsRM(const SqlitePresentationMethodDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqlitePresentationMethodDetailedStatisticsRM operator=(const SqlitePresentationMethodDetailedStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHODDETAILEDSTATISTICSRM_H
