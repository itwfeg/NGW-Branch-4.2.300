#ifndef SQLITEPRODUCTSTATISTICSRM_H
#define SQLITEPRODUCTSTATISTICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTSTATISTICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductStatisticsRM : public DBProductStatisticsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductStatisticsRMI* d = 0);
			~SqliteProductStatisticsRM();


			// --- Processing methods ---
			// Methods from DBProductStatisticsRMI
			DBQueryI<DBProductStatisticI>* retrieveAll();
			DBQueryI<DBProductStatisticI>* retrieveLinkedtoProduct(const DBProductI* prod);
			DBQueryI<DBProductStatisticI>* retrieveMostSelected(const std::string& language, int hour);
			DBQueryI<DBProductStatisticI>* retrieveMostPrinted(const std::string& language, int hour);
			DBQueryI<DBProductStatisticI>* retrieveDateTimeOrder();

			DBProductStatisticI* find(const DBProductI* prod, const std::string& language, const std::string& hour);

			DBProductStatisticI* createRecord(const DBProductI* prod);
			void saveRecord(const DBProductStatisticI* rec);
			int deleteRecord(const DBProductStatisticI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductStatisticsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductStatisticsRM(const SqliteProductStatisticsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductStatisticsRM operator=(const SqliteProductStatisticsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTSTATISTICSRM_H
