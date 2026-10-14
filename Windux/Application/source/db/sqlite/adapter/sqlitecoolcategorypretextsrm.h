#ifndef SQLITECOOLCATEGORYPRETEXTSRM_H
#define SQLITECOOLCATEGORYPRETEXTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLCATEGORYPRETEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolCategoryPreTextsRM : public DBCoolCategoryPreTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolCategoryPreTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryPreTextsRMI* d = 0);
			~SqliteCoolCategoryPreTextsRM();


			// --- Processing methods ---
			// Methods from DBCoolCategoryPreTextsRMI
			DBQueryI<DBCoolCategoryPreTextI>* retrieveAll();
			DBQueryI<DBCoolCategoryPreTextI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolCategoryPreTextI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQueryI<DBCoolCategoryPreTextI>* retrieveLinkedtoCoolPreText(const std::string& pretext, const std::string& deptNum);

			DBCoolCategoryPreTextI* find(const std::string& category, const std::string& pretext, const std::string& deptNum);

			DBCoolCategoryPreTextI* createRecord(const std::string& category, const std::string& pretext, const std::string& deptNum);
			void saveRecord(const DBCoolCategoryPreTextI* rec);
			int deleteRecord(const DBCoolCategoryPreTextI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolCategoryPreTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryPreTextsRM(const SqliteCoolCategoryPreTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryPreTextsRM operator=(const SqliteCoolCategoryPreTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYPRETEXTSRM_H
