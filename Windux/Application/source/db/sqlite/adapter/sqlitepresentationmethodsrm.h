#ifndef SQLITEPRESENTATIONMETHODSRM_H
#define SQLITEPRESENTATIONMETHODSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRESENTATIONMETHODSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqlitePresentationMethodsRM : public DBPresentationMethodsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqlitePresentationMethodsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodsRMI* d = 0);
			~SqlitePresentationMethodsRM();


			// --- Processing methods ---
			// Methods from DBPresentationMethodsRMI
			DBQueryI<DBPresentationMethodI>* retrieveAll();

			DBPresentationMethodI* find(const std::string& type);

			DBPresentationMethodI* createRecord();
			void saveRecord(const DBPresentationMethodI* rec);
			int deleteRecord(const DBPresentationMethodI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBPresentationMethodsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqlitePresentationMethodsRM(const SqlitePresentationMethodsRM& rm);
			// Do not allow the default assignment operator.
			SqlitePresentationMethodsRM operator=(const SqlitePresentationMethodsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHODSRM_H
