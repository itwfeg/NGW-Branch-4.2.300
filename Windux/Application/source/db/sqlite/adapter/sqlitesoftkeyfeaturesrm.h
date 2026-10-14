#ifndef SQLITESOFTKEYFEATURESRM_H
#define SQLITESOFTKEYFEATURESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSOFTKEYFEATURESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteSoftKeyFeaturesRM : public DBSoftKeyFeaturesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSoftKeyFeaturesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyFeaturesRMI* d = 0);
			~SqliteSoftKeyFeaturesRM();


			// --- Processing methods ---
			// Methods from DBSoftKeyFeaturesRMI
			DBQueryI<DBSoftKeyFeatureI>* retrieveAll();
			DBQueryI<DBSoftKeyFeatureI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBSoftKeyFeatureI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBSoftKeyFeatureI* findNumber(const std::string& number, const std::string& deptNum);

			DBSoftKeyFeatureI* createRecord(const std::string& number);
			void saveRecord(const DBSoftKeyFeatureI* rec);
			int deleteRecord(const DBSoftKeyFeatureI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSoftKeyFeaturesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSoftKeyFeaturesRM(const SqliteSoftKeyFeaturesRM& rm);
			// Do not allow the default assignment operator.
			SqliteSoftKeyFeaturesRM operator=(const SqliteSoftKeyFeaturesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESOFTKEYFEATURESRM_H
