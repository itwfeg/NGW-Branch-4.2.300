#ifndef DBWRAPPERERRORHISTORYI_H
#define DBWRAPPERERRORHISTORYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBWrapperErrorHistoryI
	{
		public:

			virtual ~DBWrapperErrorHistoryI() {}


			virtual DBWrapperErrorHistoryI* clone() const = 0;
			virtual const DBWrapperErrorHistoryI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual int getErrorCode() const = 0;
			virtual void setErrorCode(int errorCode) = 0;
			virtual bool isErrorCodeNull() const { return false; }
			virtual void setErrorCodeNull() = 0;
			virtual bool isErrorCodeWritable() const { return true; }

			virtual std::string getErrorDateTime() const = 0;
			virtual void setErrorDateTime(const std::string& errorDateTime) = 0;
			virtual bool isErrorDateTimeNull() const { return false; }
			virtual void setErrorDateTimeNull() = 0;
			virtual bool isErrorDateTimeWritable() const { return true; }

			virtual int getErrorPkgLength() const = 0;
			virtual void setErrorPkgLength(int pkgLength) = 0;
			virtual bool isErrorPkgLengthNull() const { return false; }
			virtual void setErrorPkgLengthNull() = 0;
			virtual bool isErrorPkgLengthWritable() const { return true; }

			virtual int getErrorPkgWidth() const = 0;
			virtual void setErrorPkgWidth(int pkgWidth) = 0;
			virtual bool isErrorPkgWidthNull() const { return false; }
			virtual void setErrorPkgWidthNull() = 0;
			virtual bool isErrorPkgWidthWritable() const { return true; }

			virtual int getErrorPkgHeight() const = 0;
			virtual void setErrorPkgHeight(int pkgHeight) = 0;
			virtual bool isErrorPkgHeightNull() const { return false; }
			virtual void setErrorPkgHeightNull() = 0;
			virtual bool isErrorPkgHeightWritable() const { return true; }

			virtual int getErrorFilmUsed() const = 0;
			virtual void setErrorFilmUsed(int filmUsed) = 0;
			virtual bool isErrorFilmUsedNull() const { return false; }
			virtual void setErrorFilmUsedNull() = 0;
			virtual bool isErrorFilmUsedWritable() const { return true; }

			virtual std::string getErrorTrayName() const = 0;
			virtual void setErrorTrayName(const std::string& trayName) = 0;
			virtual bool isErrorTrayNameNull() const { return false; }
			virtual void setErrorTrayNameNull() = 0;
			virtual bool isErrorTrayNameWritable() const { return true; }

			virtual bool getErrorBestFit() const = 0;
			virtual void setErrorBestFit(bool bestFit) = 0;
			virtual bool isErrorBestFitNull() const { return false; }
			virtual void setErrorBestFitNull() = 0;
			virtual bool isErrorBestFitWritable() const { return true; }

			virtual bool getErrorPerNet() const = 0;
			virtual void setErrorPerNet(bool perNet) = 0;
			virtual bool isErrorPerNetNull() const { return false; }
			virtual void setErrorPerNetNull() = 0;
			virtual bool isErrorPerNetWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBWRAPPERERRORHISTORYI_H
