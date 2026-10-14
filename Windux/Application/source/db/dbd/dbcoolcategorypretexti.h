#ifndef DBCOOLCATEGORYPRETEXTI_H
#define DBCOOLCATEGORYPRETEXTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryPreTextI
	{
		public:

			virtual ~DBCoolCategoryPreTextI() {}


			virtual DBCoolCategoryPreTextI* clone() const = 0;
			virtual const DBCoolCategoryPreTextI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getCategoryNumber() const = 0;
			virtual void setCategoryNumber(const std::string& category) = 0;
			virtual bool isCategoryNumberNull() const { return false; }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getPreTextNumber() const = 0;
			virtual void setPreTextNumber(const std::string& step) = 0;
			virtual bool isPreTextNumberNull() const { return false; }
			virtual bool isPreTextNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLCATEGORYPRETEXTI_H
