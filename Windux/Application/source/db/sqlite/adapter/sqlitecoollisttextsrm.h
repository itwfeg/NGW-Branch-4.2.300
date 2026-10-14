#ifndef SQLITECOOLLISTTEXTSRM_H
#define SQLITECOOLLISTTEXTSRM_H


#include <string>
#include <evolution.h>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLLISTTEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolListTextsRM : public DBCoolListTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolListTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolListTextsRMI* d = 0);
			~SqliteCoolListTextsRM();


			// --- Processing methods ---
			// Methods from DBCoolListTextsRMI
			DBQueryI<DBCoolListTextI>* retrieveAll();
			DBQueryI<DBCoolListTextI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolListTextI>* retrieveLink(const std::string& link);
			DBQueryI<DBCoolListTextI>* retrieveLinkedtoCoolList(const DBCoolListI* list);
			DBQueryI<DBCoolListTextI>* retrievePreviousCoolListTexts(const std::string& link);

			DBCoolListTextI* find(const std::string& text, const std::string& deptNum, const DBCoolListI* list);

			DBCoolListTextI* createRecord(const std::string& text, const DBCoolListI* list, HobRecordNum deptNum);
			DBCoolListTextI* createRecordInList(const std::string& text, const DBCoolListTextI* listText);
			void saveRecord(const DBCoolListTextI* rec);
			int deleteRecord(const DBCoolListTextI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolListTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolListTextsRM(const SqliteCoolListTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolListTextsRM operator=(const SqliteCoolListTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLLISTTEXTSRM_H
