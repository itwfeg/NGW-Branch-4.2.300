#ifndef DBNETWORKERRORI_H
#define DBNETWORKERRORI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBNetworkErrorI
	{
		public:

			virtual ~DBNetworkErrorI() {}


			virtual DBNetworkErrorI* clone() const = 0;
			virtual const DBNetworkErrorI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNetworkErrorKey() const = 0;
			virtual void setNetworkErrorKey(const std::string& errorKey) = 0;
			virtual bool isNetworkErrorKeyNull() const { return false; }
			virtual bool isNetworkErrorKeyWritable() const { return true; }
			
			virtual std::string getErrorCode() const = 0;
			virtual void setErrorCode(const std::string& code) = 0;
			virtual bool isErrorCodeNull() const { return false; }
			virtual void setErrorCodeNull() = 0;
			virtual bool isErrorCodeWritable() const { return true; }

			virtual int getRecordType() const = 0;
			virtual void setRecordType(int type) = 0;
			virtual bool isRecordTypeNull() const { return false; }
			virtual void setRecordTypeNull() = 0;
			virtual bool isRecordTypeWritable() const { return true; }

			virtual std::string getDateTime() const = 0;
			virtual void setDateTime(const std::string& date) = 0;
			virtual bool isDateTimeNull() const { return false; }
			virtual void setDateTimeNull() = 0;
			virtual bool isDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBNETWORKERRORI_H
