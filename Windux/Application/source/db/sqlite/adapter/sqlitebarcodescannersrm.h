#ifndef SQLITEBARCODESCANNERSRM_H
#define SQLITEBARCODESCANNERSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"


#include DBBARCODESCANNERSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteBarcodeScannersRM : public DBBarcodeScannersRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteBarcodeScannersRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBBarcodeScannersRMI* d = 0);
			~SqliteBarcodeScannersRM();


			// --- Processing methods ---
			// Methods from DBBarcodeScannersRMI
			DBQueryI<DBBarcodeScannerI>* retrieveAll();

			DBBarcodeScannerI* findNumber(const std::string& code);
			//DBBarcodeScannerI* findName(const std::string& name);		// jhc ss elev: I feel like I will really need this

			DBBarcodeScannerI* createRecord(const std::string& code);

			void saveRecord(const DBBarcodeScannerI* rec);
			int deleteRecord(const DBBarcodeScannerI* rec);
			int deleteAll();
			int countAll();
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBBarcodeScannersRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteBarcodeScannersRM(const SqliteBarcodeScannersRM& rm);
			// Do not allow the default assignment operator.
			SqliteBarcodeScannersRM operator=(const SqliteBarcodeScannersRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEBARCODESCANNERSRM_H
