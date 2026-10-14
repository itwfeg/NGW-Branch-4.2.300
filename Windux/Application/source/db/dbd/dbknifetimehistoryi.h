#ifndef DBKNIFETIMEHISTORYI_H
#define DBKNIFETIMEHISTORYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBKnifeTimeHistoryI
	{
		public:

			virtual ~DBKnifeTimeHistoryI() {}


			virtual DBKnifeTimeHistoryI* clone() const = 0;
			virtual const DBKnifeTimeHistoryI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getType() const = 0;
			virtual void setType(const std::string & type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual bool isTypeWritable() const { return true; }

			virtual int getKnifeTime() const = 0;
			virtual void setKnifeTime(int knifeTime) = 0;
			virtual bool isKnifeTimeNull() const { return false; }
			virtual void setKnifeTimeNull() = 0;
			virtual bool isKnifeTimeWritable() const { return true; }

			virtual std::string getTimeStampDateTime() const = 0;
			virtual void setTimeStampDateTime(const std::string& timeStamp) = 0;
			virtual bool isTimeStampDateTimeNull() const { return false; }
			virtual void setTimeStampDateTimeNull() = 0;
			virtual bool isTimeStampDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBKNIFETIMEHISTORYI_H
