#ifndef DBCOOLCATEGORYPRODUCTIONSTEPI_H
#define DBCOOLCATEGORYPRODUCTIONSTEPI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryProductionStepI
	{
		public:

			virtual ~DBCoolCategoryProductionStepI() {}


			virtual DBCoolCategoryProductionStepI* clone() const = 0;
			virtual const DBCoolCategoryProductionStepI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;

			virtual std::string getCategoryNumber() const = 0;
			virtual void setCategoryNumber(const std::string& category) = 0;
			virtual bool isCategoryNumberNull() const { return false; }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getProductionStepKey() const = 0;
			virtual void setProductionStepKey(const std::string& stepKey) = 0;
			virtual bool isProductionStepKeyNull() const { return false; }
			virtual bool isProductionStepKeyWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLCATEGORYPRODUCTIONSTEPI_H
