#ifndef SQLITEPRODUCTSEQUENCESRM_H
#define SQLITEPRODUCTSEQUENCESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTSEQUENCESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductSequencesRM : public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductSequencesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductSequencesRMI* d = 0);
			~SqliteProductSequencesRM();


			// --- Processing methods ---
			// Methods from DBProductSequencesRMI
			DBQueryI<DBProductSequenceI>* retrieveAll();
			DBQueryI<DBProductSequenceI>* retrieveLinkedToProduct(const DBProductI* prod);
			DBQueryI<DBProductSequenceI>* retrieveCharacterSequence(const std::string& sequence, const std::string& language);

			DBProductSequenceI* findProductSequence(const DBProductI* prod, const std::string& sequence, const std::string& language);

			DBProductSequenceI* createRecord(const DBProductI* prod, const std::string& sequence, const std::string& language);
			void saveRecord(const DBProductSequenceI* rec);
			int deleteRecord(const DBProductSequenceI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductSequencesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductSequencesRM(const SqliteProductSequencesRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductSequencesRM operator=(const SqliteProductSequencesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTSEQUENCESRM_H
