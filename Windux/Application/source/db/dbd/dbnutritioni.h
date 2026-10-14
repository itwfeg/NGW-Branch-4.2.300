#ifndef DBNUTRITIONI_H
#define DBNUTRITIONI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBNutritionI
	{
		public:

			virtual ~DBNutritionI() {}


			virtual DBNutritionI* clone() const = 0;
			virtual const DBNutritionI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getNutritionColumn1() const = 0;
			virtual void setNutritionColumn1(const std::string& col1) = 0;
			virtual bool isNutritionColumn1Null() const { return false; }
			virtual void setNutritionColumn1Null() = 0;
			virtual bool isNutritionColumn1Writable() const { return true; }

			virtual std::string getNutritionColumn2() const = 0;
			virtual void setNutritionColumn2(const std::string& col2) = 0;
			virtual bool isNutritionColumn2Null() const { return false; }
			virtual void setNutritionColumn2Null() = 0;
			virtual bool isNutritionColumn2Writable() const { return true; }

			virtual std::string getNutritionColumn3() const = 0;
			virtual void setNutritionColumn3(const std::string& col3) = 0;
			virtual bool isNutritionColumn3Null() const { return false; }
			virtual void setNutritionColumn3Null() = 0;
			virtual bool isNutritionColumn3Writable() const { return true; }

			virtual std::string getNutritionColumn4() const = 0;
			virtual void setNutritionColumn4(const std::string& col4) = 0;
			virtual bool isNutritionColumn4Null() const { return false; }
			virtual void setNutritionColumn4Null() = 0;
			virtual bool isNutritionColumn4Writable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBNUTRITIONI_H
