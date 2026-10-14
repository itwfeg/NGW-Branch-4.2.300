#ifndef SQLITEBARCODESCANNER_H
#define SQLITEBARCODESCANNER_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBBARCODESCANNERI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteBarcodeScanner : public DBBarcodeScannerI
	{
		public:

			// Types and subtypes
			typedef DBBarcodeScannerI interface_type;


			// Constructors and destructor
			SqliteBarcodeScanner(SqliteRecord* r, DBBarcodeScannerI* d = 0);
			~SqliteBarcodeScanner();


			// Think about replacing this method with two friend functions from the
			// SqliteProductTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductTextI
			virtual DBBarcodeScannerI* clone() const { return new SqliteBarcodeScanner(new SqliteRecord(*rec_)); }
			virtual const DBBarcodeScannerI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getScannerCode() const { return rec_->getField(SCANNERCODE_FIELD); }
			virtual void setScannerCode(const std::string& num) { rec_->setField(SCANNERCODE_FIELD, num); }
			virtual bool isScannerCodeNull() const { return rec_->isFieldNull(SCANNERCODE_FIELD); }
			virtual void setScannerCodeNull() { rec_->setFieldNull(SCANNERCODE_FIELD); }
			virtual bool isScannerCodeWritable() const { return true; }
			
			virtual std::string getScannerProductNum() const { return rec_->getField(SCANNERPLU_FIELD); }
			virtual void setScannerProductNum(const std::string& type) { rec_->setField(SCANNERPLU_FIELD, type); }
			virtual bool isScannerProductNumNull() const { return rec_->isFieldNull(SCANNERPLU_FIELD); }
			virtual void setScannerProductNumNull() { rec_->setFieldNull(SCANNERCODE_FIELD); }
			virtual bool isScannerProductNumWritable() const { return true; }

			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string SCANNERCODE_FIELD;
			static const std::string SCANNERPLU_FIELD;


		private:

			SqliteRecord* rec_;
			DBBarcodeScannerI* decorated_;

			// Do not allow the default copy constructor.
			SqliteBarcodeScanner(const SqliteBarcodeScanner& rec);
			// Do not allow the default assignment operator.
			SqliteBarcodeScanner operator=(const SqliteBarcodeScanner& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEBARCODESCANNER_H
