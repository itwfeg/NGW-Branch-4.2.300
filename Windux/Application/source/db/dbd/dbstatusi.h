#ifndef DBSTATUSI_H
#define DBSTATUSI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBStatusI
	{
		public:

			virtual ~DBStatusI() {}


			virtual DBStatusI* clone() const = 0;
			virtual const DBStatusI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getName() const = 0;
			virtual void setName(const std::string& name) = 0;
			virtual bool isNameNull() const { return false; }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getValue() const = 0;
			virtual void setValue(const std::string& value) = 0;
			virtual bool isValueNull() const { return false; }
			virtual void setValueNull() = 0;
			virtual bool isValueWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSTATUSI_H
