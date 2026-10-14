#ifndef DBCOOLCATEGORYCOUNTRYI_H
#define DBCOOLCATEGORYCOUNTRYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryCountryI
	{
		public:

			virtual ~DBCoolCategoryCountryI() {}


			virtual DBCoolCategoryCountryI* clone() const = 0;
			virtual const DBCoolCategoryCountryI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getCategoryNumber() const = 0;
			virtual void setCategoryNumber(const std::string& category) = 0;
			virtual bool isCategoryNumberNull() const { return false; }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getCountryKey() const = 0;
			virtual void setCountryKey(const std::string& countryKey) = 0;
			virtual bool isCountryKeyNull() const { return false; }
			virtual bool isCountryKeyWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }



			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLCATEGORYCOUNTRYI_H
