#ifndef SQLITEIGNOREWORDSRM_H
#define SQLITEIGNOREWORDSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBIGNOREWORDSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteIgnoreWordsRM : public DBIgnoreWordsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteIgnoreWordsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBIgnoreWordsRMI* d = 0);
			~SqliteIgnoreWordsRM();


			// --- Processing methods ---
			// Methods from DBIgnoreWordsRMI
			DBQueryI<DBIgnoreWordI>* retrieveAll();

			DBIgnoreWordI* find(const std::string& language, const std::string& name);

			DBIgnoreWordI* createRecord();
			void saveRecord(const DBIgnoreWordI* rec);
			int deleteRecord(const DBIgnoreWordI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBIgnoreWordsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteIgnoreWordsRM(const SqliteIgnoreWordsRM& rm);
			// Do not allow the default assignment operator.
			SqliteIgnoreWordsRM operator=(const SqliteIgnoreWordsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEIGNOREWORDSRM_H
