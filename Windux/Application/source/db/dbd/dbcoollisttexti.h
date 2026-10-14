#ifndef DBCOOLLISTTEXTI_H
#define DBCOOLLISTTEXTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListTextI
	{
		public:

			virtual ~DBCoolListTextI() {}


			virtual DBCoolListTextI* clone() const = 0;
			virtual const DBCoolListTextI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getCoolTextNumber() const = 0;
			virtual void setCoolTextNumber(const std::string& text) = 0;
			virtual bool isCoolTextNumberNull() const { return false; }
			virtual bool isCoolTextNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getOrder() const = 0;
			virtual void setOrder(int order) = 0;
			virtual bool isOrderNull() const { return false; }
			virtual bool isOrderWritable() const { return true; }

			virtual std::string getLinkNumber() const = 0;
			virtual void setLinkNumber(const std::string& number) = 0;
			virtual bool isLinkNumberNull() const { return false; }
			virtual void setLinkNumberNull() = 0;
			virtual bool isLinkNumberWritable() const { return true; }

			virtual std::string getNextLinkNumber() const = 0;
			virtual void setNextLinkNumber(const std::string& next) = 0;
			virtual bool isNextLinkNumberNull() const { return false; }
			virtual void setNextLinkNumberNull() = 0;
			virtual bool isNextLinkNumberWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLLISTTEXTI_H
