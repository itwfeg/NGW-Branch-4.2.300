#ifndef DBPRESENTATIONMETHODSTATISTICI_H
#define DBPRESENTATIONMETHODSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodStatisticI
	{
		public:

			virtual ~DBPresentationMethodStatisticI() {}


			virtual DBPresentationMethodStatisticI* clone() const = 0;
			virtual const DBPresentationMethodStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getPresentationMethodType() const = 0;
			virtual void setPresentationMethodType(const std::string& type) = 0;
			virtual bool isPresentationMethodTypeNull() const { return false; }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual std::string getLanguage() const = 0;
			virtual void setLanguage(const std::string& lang) = 0;
			virtual bool isLanguageNull() const { return false; }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getHourNumber() const = 0;
			virtual void setHourNumber(const std::string& number) = 0;
			virtual bool isHourNumberNull() const { return false; }
			virtual bool isHourNumberWritable() const { return true; }

			virtual int getTotalMethodSelections() const = 0;
			virtual void setTotalMethodSelections(int selections) = 0;
			virtual void incrementTotalMethodSelections() = 0;
			virtual bool isTotalMethodSelectionsNull() const { return false; }
			virtual void setTotalMethodSelectionsNull() = 0;
			virtual bool isTotalMethodSelectionsWritable() const { return true; }

			virtual int getTotalCategorySelections() const = 0;
			virtual void setTotalCategorySelections(int selections) = 0;
			virtual void incrementTotalCategorySelections() = 0;
			virtual bool isTotalCategorySelectionsNull() const { return false; }
			virtual void setTotalCategorySelectionsNull() = 0;
			virtual bool isTotalCategorySelectionsWritable() const { return true; }

			virtual int getTotalProductSelections() const = 0;
			virtual void setTotalProductSelections(int selections) = 0;
			virtual void incrementTotalProductSelections() = 0;
			virtual bool isTotalProductSelectionsNull() const { return false; }
			virtual void setTotalProductSelectionsNull() = 0;
			virtual bool isTotalProductSelectionsWritable() const { return true; }

			virtual int getTotalLabelsPrinted() const = 0;
			virtual void setTotalLabelsPrinted(int selections) = 0;
			virtual void incrementTotalLabelsPrinted() = 0;
			virtual bool isTotalLabelsPrintedNull() const { return false; }
			virtual void setTotalLabelsPrintedNull() = 0;
			virtual bool isTotalLabelsPrintedWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODSTATISTICI_H
