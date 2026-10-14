#ifndef SQLITEOPERATORSRM_H
#define SQLITEOPERATORSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBOPERATORSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteOperatorsRM : public DBOperatorsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteOperatorsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBOperatorsRMI* d = 0);
			~SqliteOperatorsRM();


			// --- Processing methods ---
			// Methods from DBOperatorsRMI
			DBQueryI<DBOperatorI>* retrieveAll();
			DBQueryI<DBOperatorI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBOperatorI* findNumber(const std::string& number);
			DBOperatorI* findRFID(const std::string& id);

			DBOperatorI* createRecord();
			void saveRecord(const DBOperatorI* rec);
			int deleteRecord(const DBOperatorI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBOperatorsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteOperatorsRM(const SqliteOperatorsRM& rm);
			// Do not allow the default assignment operator.
			SqliteOperatorsRM operator=(const SqliteOperatorsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEOPERATORSRM_H
