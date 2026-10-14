#ifndef SQLITESTREAMINGGRAPHICSRM_H
#define SQLITESTREAMINGGRAPHICSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSTREAMINGGRAPHICSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteStreamingGraphicsRM : public DBStreamingGraphicsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteStreamingGraphicsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBStreamingGraphicsRMI* d = 0);
			~SqliteStreamingGraphicsRM();


			// --- Processing methods ---
			// Methods from DBStreamingGraphicsRMI
			DBQueryI<DBStreamingGraphicI>* retrieveAll();
			DBQueryI<DBStreamingGraphicI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBStreamingGraphicI* findNumber(const std::string& number);
			DBStreamingGraphicI* findNext(const DBStreamingGraphicI* sg);

			DBStreamingGraphicI* createRecord();
			void saveRecord(const DBStreamingGraphicI* rec);
			int deleteRecord(const DBStreamingGraphicI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBStreamingGraphicsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteStreamingGraphicsRM(const SqliteStreamingGraphicsRM& rm);
			// Do not allow the default assignment operator.
			SqliteStreamingGraphicsRM operator=(const SqliteStreamingGraphicsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESTREAMINGGRAPHICSRM_H
