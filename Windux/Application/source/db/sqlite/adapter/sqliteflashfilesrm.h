#ifndef SQLITEFLASHFILESRM_H
#define SQLITEFLASHFILESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBFLASHFILESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteFlashFilesRM : public DBFlashFilesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteFlashFilesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBFlashFilesRMI* d = 0);
			~SqliteFlashFilesRM();


			// --- Processing methods ---
			// Methods from DBFlashFilesRMI
			DBQueryI<DBFlashFileI>* retrieveAll();

			DBFlashFileI* findName(const std::string& name);

			DBFlashFileI* createRecord();
			void saveRecord(const DBFlashFileI* rec);
			int deleteRecord(const DBFlashFileI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBFlashFilesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteFlashFilesRM(const SqliteFlashFilesRM& rm);
			// Do not allow the default assignment operator.
			SqliteFlashFilesRM operator=(const SqliteFlashFilesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEFLASHFILESRM_H
