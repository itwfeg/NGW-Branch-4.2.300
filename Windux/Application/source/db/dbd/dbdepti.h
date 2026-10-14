#ifndef DBDEPTI_H
#define DBDEPTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBDeptI
	{
		public:

			virtual ~DBDeptI() {}


			virtual DBDeptI* clone() const = 0;
			virtual const DBDeptI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

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


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBDEPTI_H
