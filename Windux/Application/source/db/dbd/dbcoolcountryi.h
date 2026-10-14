#ifndef DBCOOLCOUNTRYI_H
#define DBCOOLCOUNTRYI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCountryI
	{
		public:

			virtual ~DBCoolCountryI() {}


			virtual DBCoolCountryI* clone() const = 0;
			virtual const DBCoolCountryI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getCountryKey() const = 0;
			virtual void setCountryKey(const std::string& key) = 0;
			virtual bool isCountryKeyNull() const { return false; }
			virtual bool isCountryKeyWritable() const { return true; }

			virtual std::string getName1() const = 0;
			virtual void setName1(const std::string& name) = 0;
			virtual bool isName1Null() const { return false; }
			virtual void setName1Null() = 0;
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const = 0;
			virtual void setName2(const std::string& name) = 0;
			virtual bool isName2Null() const { return false; }
			virtual void setName2Null() = 0;
			virtual bool isName2Writable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual bool getAllCategories() const = 0;
			virtual void setAllCategories(bool all) = 0;
			virtual bool isAllCategoriesNull() const { return false; }
			virtual void setAllCategoriesNull() = 0;
			virtual bool isAllCategoriesWritable() const { return true; }

			virtual std::string getCountryNumber() const = 0;
			virtual void setCountryNumber(const std::string& key) = 0;
			virtual bool isCountryNumberNull() const { return false; }
			virtual bool isCountryNumberWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLCOUNTRYI_H
