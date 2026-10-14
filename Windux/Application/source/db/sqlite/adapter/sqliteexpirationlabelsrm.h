#ifndef SQLITEEXPIRATIONLABELSRM_H
#define SQLITEEXPIRATIONLABELSRM_H


#include <string>
#include <vector>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBEXPIRATIONLABELSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteExpirationLabelsRM : public DBExpirationLabelsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteExpirationLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBExpirationLabelsRMI* d = 0);
			~SqliteExpirationLabelsRM();


			// --- Processing methods ---
			// Methods from DBExpirationLabelsRMI
			DBQueryI<DBExpirationLabelI>* retrieveAll();
			DBQueryI<DBExpirationLabelI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBExpirationLabelI>* retrieveLinkedToPLU(const std::string& plu, const std::string& deptNum);

			DBExpirationLabelI* find(const std::string& key, const std::string& deptNum);

			DBExpirationLabelI* createRecord();
			void saveRecord(const DBExpirationLabelI* rec);
			int deleteRecord(const DBExpirationLabelI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBExpirationLabelsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteExpirationLabelsRM(const SqliteExpirationLabelsRM& rm);
			// Do not allow the default assignment operator.
			SqliteExpirationLabelsRM operator=(const SqliteExpirationLabelsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};

	
#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEEXPIRATIONLABELSRM_H
