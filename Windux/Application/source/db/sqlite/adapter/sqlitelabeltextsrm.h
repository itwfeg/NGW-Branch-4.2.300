#ifndef SQLITELABELTEXTSRM_H
#define SQLITELABELTEXTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBLABELTEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteLabelTextsRM : public DBLabelTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteLabelTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBLabelTextsRMI* d = 0);
			~SqliteLabelTextsRM();


			// --- Processing methods ---
			// Methods from DBLabelTextsRMI
			DBQueryI<DBLabelTextI>* retrieveAll();
			DBQueryI<DBLabelTextI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBLabelTextI* findNumber(const std::string& number);

			DBLabelTextI* createRecord();
			void saveRecord(const DBLabelTextI* rec);
			int deleteRecord(const DBLabelTextI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBLabelTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteLabelTextsRM(const SqliteLabelTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteLabelTextsRM operator=(const SqliteLabelTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITELABELTEXTSRM_H
