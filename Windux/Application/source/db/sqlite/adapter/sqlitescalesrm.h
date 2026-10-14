#ifndef SQLITESCALESRM_H
#define SQLITESCALESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSCALESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteScalesRM : public DBScalesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteScalesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBScalesRMI* d = 0);
			~SqliteScalesRM();


			// --- Processing methods ---
			// Methods from DBScalesRMI
			DBQueryI<DBScaleI>* retrieveAll();
			DBQueryI<DBScaleI>* retrieveType(int type);

			DBScaleI* findIdentifier(const std::string& identifier);
			DBScaleI* findHostName(const std::string& hostname);
			DBScaleI* findIPAddress(const std::string& ipAddress);
			DBScaleI* findType(int type);

			DBScaleI* createRecord();
			void saveRecord(const DBScaleI* rec);
			int deleteRecord(const DBScaleI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }

			void ensureAutoIncrementValue(int value);


		private:

			const SqliteDatabase& sdb_;
			DBScalesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteScalesRM(const SqliteScalesRM& rm);
			// Do not allow the default assignment operator.
			SqliteScalesRM operator=(const SqliteScalesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESCALESRM_H
