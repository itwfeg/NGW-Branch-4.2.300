#ifndef DBPRODUCTSTATISTICI_H
#define DBPRODUCTSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductStatisticI
	{
		public:

			virtual ~DBProductStatisticI() {}


			virtual DBProductStatisticI* clone() const = 0;
			virtual const DBProductStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getLanguage() const = 0;
			virtual void setLanguage(const std::string& language) = 0;
			virtual bool isLanguageNull() const { return false; }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getHourNumber() const = 0;
			virtual void setHourNumber(const std::string& type) = 0;
			virtual bool isHourNumberNull() const { return false; }
			virtual bool isHourNumberWritable() const { return true; }

			virtual std::string getProductKey() const = 0;
			virtual void setProductKey(const std::string& plu) = 0;
			virtual bool isProductKeyNull() const { return false; }
			virtual bool isProductKeyWritable() const { return true; }

			virtual int getTotalSelections() const = 0;
			virtual void setTotalSelections(int selections) = 0;
			virtual void incrementTotalSelections() = 0;
			virtual bool isTotalSelectionsNull() const { return false; }
			virtual void setTotalSelectionsNull() = 0;
			virtual bool isTotalSelectionsWritable() const { return true; }

			virtual int getTimeToSelect() const = 0;
			virtual void setTimeToSelect(int time) = 0;
			virtual bool isTimeToSelectNull() const { return false; }
			virtual void setTimeToSelectNull() = 0;
			virtual bool isTimeToSelectWritable() const { return true; }

			virtual int getTotalLabelsPrinted() const = 0;
			virtual void setTotalLabelsPrinted(int printed) = 0;
			virtual void incrementTotalLabelsPrinted() = 0;
			virtual bool isTotalLabelsPrintedNull() const { return false; }
			virtual void setTotalLabelsPrintedNull() = 0;
			virtual bool isTotalLabelsPrintedWritable() const { return true; }

			virtual std::string getRecordedDateTime() const = 0;
			virtual void setRecordedDateTime(const std::string& rdt) = 0;
			virtual bool isRecordedDateTimeNull() const { return false; }
			virtual void setRecordedDateTimeNull() = 0;
			virtual bool isRecordedDateTimeWritable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTSTATISTICI_H
