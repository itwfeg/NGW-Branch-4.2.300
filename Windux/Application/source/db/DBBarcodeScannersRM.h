/**
 * @file HfcScannerProductRead.h
 * @brief HfcScannerProductRead tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup DBBarcodeScannerRMCsu
 * @{
*/
#ifndef DBBARCODESCANNERSRM_H
#define DBBARCODESCANNERSRM_H


#include <string>


#include "edb.h"
#include DBBARCODESCANNERSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBBarcodeScanner;
	template <class DBEntity> class DBQuery;


	class DBBarcodeScannersRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBBarcodeScanner>;

		public:

			// Types
			typedef DBBarcodeScanner record_type;
			typedef DBQuery<DBBarcodeScanner> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBBarcodeScannersRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBBarcodeScanner>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the HfcScannerProductRead record".
			//DBBarcodeScanner* findNumber(int type = 0) const;
			DBBarcodeScanner* findNumber(const std::string& number) const;
			DBBarcodeScanner* find(const std::string& key) const { return findNumber(key); }

			DBBarcodeScanner* createRecord(const std::string& number) const;
			void saveRecord(const DBBarcodeScanner* rec) const;
			int deleteRecord(const DBBarcodeScanner* rec) const;      // returns # of deleted records
			int deleteAll() const;                                    // returns # of deleted records
			int countAll() const;                                     // returns # of existing records
			int calculateAverageSpace() const;                        // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);

			int getType() const { return type_; }
			void setType(int type) { type_ = type; }


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBBarcodeScanner> > qsRegistry_;
			edb_adapter::DBBarcodeScannersRMI* decorated_;

			int type_;


			// --- Private Constructor ---
			DBBarcodeScannersRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBBarcodeScannersRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBBarcodeScannersRM(const DBBarcodeScannersRM& rm);
			// Do not allow the default assignment operator.
			DBBarcodeScannersRM operator=(const DBBarcodeScannersRM& rhs);


			// Registry methods
			static util_registry::Registry<DBBarcodeScannersRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBBarcodeScannersRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBBarcodeScannersRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBBarcodeScanner>* query);
			bool deregisterQ(DBQuery<DBBarcodeScanner>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBBARCODESCANNERSRM_H
