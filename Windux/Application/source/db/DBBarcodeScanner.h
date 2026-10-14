#ifndef DBBARCODESCANNER_H
#define DBBARCODESCANNER_H


#include <string>


#include "edb.h"
#include DBBARCODESCANNERI_INC
#if defined( __linux__ )
    #include DBBARCODESCANNERSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBBarcodeScanner
	{
			// DBBarcodeScannersRM class is a friend of DBBarcodeScanner because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBBarcodeScannersRM;
			friend class DBQuery<DBBarcodeScanner>;
			//friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBBarcodeScannerI interface_type;
			typedef DBBarcodeScannersRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBBarcodeScanner(const DBBarcodeScanner& rec);  
			~DBBarcodeScanner();


			// Methods associated with DBBarcodeScannerI
			std::string getScannerCode() const { return decorated_->getScannerCode(); }
			void setScannerCode(const std::string& num) { decorated_->setScannerCode(num); }
			bool isScannerCodeNull() const { return decorated_->isScannerCodeNull(); }
			void setScannerCodeNull() { decorated_->setScannerCodeNull(); }
			bool isScannerCodeWritable() const { return decorated_->isScannerCodeWritable(); }

			std::string getScannerProductNum() const { return decorated_->getScannerProductNum(); }
			void setScannerProductNum(const std::string& num) { decorated_->setScannerProductNum(num); }
			bool isScannerProductNumNull() const { return decorated_->isScannerProductNumNull(); }
			bool isScannerProductNumWritable() const { return decorated_->isScannerProductNumWritable(); }

			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getScannerCode(); }
			void setKey(const std::string& key) { decorated_->setScannerCode(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBBarcodeScanner& rec);


		private:

			edb_adapter::DBBarcodeScannerI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBBarcodeScanner(edb_adapter::DBBarcodeScannerI* d);

			// Do not allow the default assignment operator.
			DBBarcodeScanner operator=(const DBBarcodeScanner& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBBARCODESCANNER_H
