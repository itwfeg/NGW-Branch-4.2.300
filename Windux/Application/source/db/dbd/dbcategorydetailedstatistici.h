#ifndef DBCATEGORYDETAILEDSTATISTICI_H
#define DBCATEGORYDETAILEDSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryDetailedStatisticI
	{
		public:

			virtual ~DBCategoryDetailedStatisticI() {}


			virtual DBCategoryDetailedStatisticI* clone() const = 0;
			virtual const DBCategoryDetailedStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getPresentationMethodType() const = 0;
			virtual void setPresentationMethodType(const std::string& type) = 0;
			virtual bool isPresentationMethodTypeNull() const { return false; }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual std::string getCategoryStatisticLanguage() const = 0;
			virtual void setCategoryStatisticLanguage(const std::string& language) = 0;
			virtual bool isCategoryStatisticLanguageNull() const { return false; }
			virtual bool isCategoryStatisticLanguageWritable() const { return true; }

			virtual std::string getCategoryStatisticHourNumber() const = 0;
			virtual void setCategoryStatisticHourNumber(const std::string& hour) = 0;
			virtual bool isCategoryStatisticHourNumberNull() const { return false; }
			virtual bool isCategoryStatisticHourNumberWritable() const { return true; }

			virtual std::string getCategoryNumber() const = 0;
			virtual void setCategoryNumber(const std::string& cat) = 0;
			virtual bool isCategoryNumberNull() const { return false; }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual int getSelections() const = 0;
			virtual void setSelections(int selections) = 0;
			virtual void incrementSelections() = 0;
			virtual bool isSelectionsNull() const { return false; }
			virtual void setSelectionsNull() = 0;
			virtual bool isSelectionsWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCATEGORYDETAILEDSTATISTICI_H
