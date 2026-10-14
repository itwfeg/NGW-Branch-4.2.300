#ifndef SQLITERECALLSRM_H
#define SQLITERECALLSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBRECALLSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteRecallsRM : public DBRecallsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteRecallsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBRecallsRMI* d = 0);
			~SqliteRecallsRM();


			// --- Processing methods ---
			// Methods from DBRecallsRMI
			DBQueryI<DBRecallI>* retrieveAll();
			DBQueryI<DBRecallI>* retrieveAcknowledged();
			DBQueryI<DBRecallI>* retrieveUnacknowledged();
			//DBQueryI<DBRecallI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBRecallI* findNumber(const std::string& number);
			DBRecallI* findName(const std::string& name);

			DBRecallI* createRecord();
			void saveRecord(const DBRecallI* rec);
			int deleteRecord(const DBRecallI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBRecallsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteRecallsRM(const SqliteRecallsRM& rm);
			// Do not allow the default assignment operator.
			SqliteRecallsRM operator=(const SqliteRecallsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITERECALLSRM_H
