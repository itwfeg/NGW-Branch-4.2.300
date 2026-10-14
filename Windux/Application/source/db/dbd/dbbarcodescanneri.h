#ifndef DBBARCODESCANNERI_H
#define DBBARCODESCANNERI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBBarcodeScannerI
	{
		public:

			virtual ~DBBarcodeScannerI() {}


			virtual DBBarcodeScannerI* clone() const = 0;
			virtual const DBBarcodeScannerI* getDecorated() const = 0;

			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;

			virtual std::string getScannerCode() const = 0;
			virtual void setScannerCode(const std::string& code) = 0;
			virtual bool isScannerCodeNull() const { return false; }
			virtual void setScannerCodeNull() = 0;
			virtual bool isScannerCodeWritable() const { return true; }

			virtual std::string getScannerProductNum() const = 0;
			virtual void setScannerProductNum(const std::string& num) = 0;
			virtual bool isScannerProductNumNull() const { return false; }
			virtual bool isScannerProductNumWritable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBBARCODESCANNERI_H
