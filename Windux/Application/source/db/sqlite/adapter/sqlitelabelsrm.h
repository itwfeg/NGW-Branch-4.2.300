#ifndef SQLITELABELSRM_H
#define SQLITELABELSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBLABELSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteLabelsRM : public DBLabelsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBLabelsRMI* d = 0);
			~SqliteLabelsRM();


			// --- Processing methods ---
			// Methods from DBLabelsRMI
			DBQueryI<DBLabelI>* retrieveAll();
			DBQueryI<DBLabelI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBLabelI>* retrieveLabelNumListForDept(const std::string& deptNum);
			DBQueryI<DBLabelI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBLabelI* findNumber(const std::string& number, const std::string& deptNum);

			DBLabelI* createRecord();
			void saveRecord(const DBLabelI* rec);
			int deleteRecord(const DBLabelI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBLabelsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteLabelsRM(const SqliteLabelsRM& rm);
			// Do not allow the default assignment operator.
			SqliteLabelsRM operator=(const SqliteLabelsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITELABELSRM_H
