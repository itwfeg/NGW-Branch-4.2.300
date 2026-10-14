#ifndef DBCOOLPRODUCTIONSTEPI_H
#define DBCOOLPRODUCTIONSTEPI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolProductionStepI
	{
		public:

			virtual ~DBCoolProductionStepI() {}


			virtual DBCoolProductionStepI* clone() const = 0;
			virtual const DBCoolProductionStepI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getProductionStepKey() const = 0;
			virtual void setProductionStepKey(const std::string& key) = 0;
			virtual bool isProductionStepKeyNull() const { return false; }
			virtual bool isProductionStepKeyWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

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

			virtual bool getAllCategories() const = 0;
			virtual void setAllCategories(bool all) = 0;
			virtual bool isAllCategoriesNull() const { return false; }
			virtual void setAllCategoriesNull() = 0;
			virtual bool isAllCategoriesWritable() const { return true; }

			virtual std::string getProductionStepNumber() const = 0;
			virtual void setProductionStepNumber(const std::string& key) = 0;
			virtual bool isProductionStepNumberNull() const { return false; }
			virtual bool isProductionStepNumberWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLPRODUCTIONSTEPI_H
