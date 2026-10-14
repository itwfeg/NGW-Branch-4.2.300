#ifndef DBPRODUCTDETAILEDSTATISTICI_H
#define DBPRODUCTDETAILEDSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductDetailedStatisticI
	{
		public:

			virtual ~DBProductDetailedStatisticI() {}


			virtual DBProductDetailedStatisticI* clone() const = 0;
			virtual const DBProductDetailedStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getPresentationMethodNumber() const = 0;
			virtual void setPresentationMethodNumber(const std::string& number) = 0;
			virtual bool isPresentationMethodNumberNull() const { return false; }
			virtual bool isPresentationMethodNumberWritable() const { return true; }

			virtual std::string getProductStatisticLanguage() const = 0;
			virtual void setProductStatisticLanguage(const std::string& language) = 0;
			virtual bool isProductStatisticLanguageNull() const { return false; }
			virtual bool isProductStatisticLanguageWritable() const { return true; }

			virtual std::string getProductStatisticHourNumber() const = 0;
			virtual void setProductStatisticHourNumber(const std::string& hour) = 0;
			virtual bool isProductStatisticHourNumberNull() const { return false; }
			virtual bool isProductStatisticHourNumberWritable() const { return true; }

			virtual std::string getProductKey() const = 0;
			virtual void setProductKey(const std::string& plu) = 0;
			virtual bool isProductKeyNull() const { return false; }
			virtual bool isProductKeyWritable() const { return true; }

			virtual int getSelections() const = 0;
			virtual void setSelections(int selections) = 0;
			virtual void incrementSelections() = 0;
			virtual bool isSelectionsNull() const { return false; }
			virtual void setSelectionsNull() = 0;
			virtual bool isSelectionsWritable() const { return true; }

			virtual int getLabelsPrinted() const = 0;
			virtual void setLabelsPrinted(int printed) = 0;
			virtual void incrementLabelsPrinted() = 0;
			virtual bool isLabelsPrintedNull() const { return false; }
			virtual void setLabelsPrintedNull() = 0;
			virtual bool isLabelsPrintedWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTDETAILEDSTATISTICI_H
