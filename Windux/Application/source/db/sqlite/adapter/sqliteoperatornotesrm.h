#ifndef SQLITEOPERATORNOTESRM_H
#define SQLITEOPERATORNOTESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBOPERATORNOTESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteOperatorNotesRM : public DBOperatorNotesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteOperatorNotesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBOperatorNotesRMI* d = 0);
			~SqliteOperatorNotesRM();


			// --- Processing methods ---
			// Methods from DBOperatorNotesRMI
			DBQueryI<DBOperatorNoteI>* retrieveAll();

			DBOperatorNoteI* findRecord();

			DBOperatorNoteI* createRecord();
			void saveRecord(const DBOperatorNoteI* rec);
			int deleteRecord(const DBOperatorNoteI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBOperatorNotesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteOperatorNotesRM(const SqliteOperatorNotesRM& rm);
			// Do not allow the default assignment operator.
			SqliteOperatorNotesRM operator=(const SqliteOperatorNotesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEOPERATORNOTESRM_H
