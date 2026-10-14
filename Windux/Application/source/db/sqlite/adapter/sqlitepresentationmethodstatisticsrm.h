#ifndef SQLITEPRESENTATIONMETHODSTATISTICSRM_H
#define SQLITEPRESENTATIONMETHODSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRESENTATIONMETHODSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqlitePresentationMethodStatisticsRM : public DBPresentationMethodStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqlitePresentationMethodStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodStatisticsRMI* d = 0);
			~SqlitePresentationMethodStatisticsRM();


			// --- Processing methods ---
			// Methods from DBPresentationMethodStatisticsRMI
			DBQueryI<DBPresentationMethodStatisticI>* retrieveAll();
			DBQueryI<DBPresentationMethodStatisticI>* retrieveLinkedtoPresentationMethod(const DBPresentationMethodI* method);

			DBPresentationMethodStatisticI* find(const DBPresentationMethodI* method, const std::string& lang, const std::string& hour);

			DBPresentationMethodStatisticI* createRecord(const DBPresentationMethodI* method);
			void saveRecord(const DBPresentationMethodStatisticI* rec);
			int deleteRecord(const DBPresentationMethodStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBPresentationMethodStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqlitePresentationMethodStatisticsRM(const SqlitePresentationMethodStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqlitePresentationMethodStatisticsRM operator=(const SqlitePresentationMethodStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHODSTATISTICSRM_H
