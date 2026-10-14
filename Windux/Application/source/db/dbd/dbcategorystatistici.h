#ifndef DBCATEGORYSTATISTICI_H
#define DBCATEGORYSTATISTICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryStatisticI
	{
		public:

			virtual ~DBCategoryStatisticI() {}


			virtual DBCategoryStatisticI* clone() const = 0;
			virtual const DBCategoryStatisticI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getHourNumber() const = 0;
			virtual void setHourNumber(const std::string& type) = 0;
			virtual bool isHourNumberNull() const { return false; }
			virtual bool isHourNumberWritable() const { return true; }

			virtual std::string getLanguage() const = 0;
			virtual void setLanguage(const std::string& language) = 0;
			virtual bool isLanguageNull() const { return false; }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getCategoryNumber() const = 0;
			virtual void setCategoryNumber(const std::string& cat) = 0;
			virtual bool isCategoryNumberNull() const { return false; }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual int getTotalSelections() const = 0;
			virtual void setTotalSelections(int selections) = 0;
			virtual void incrementTotalSelections() = 0;
			virtual bool isTotalSelectionsNull() const { return false; }
			virtual void setTotalSelectionsNull() = 0;
			virtual bool isTotalSelectionsWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCATEGORYSTATISTICI_H
