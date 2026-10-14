#ifndef DBROMLABELI_H
#define DBROMLABELI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBROMLabelI
	{
		public:

			virtual ~DBROMLabelI() {}


			virtual DBROMLabelI* clone() const = 0;
			virtual const DBROMLabelI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getBinary() const = 0;
			virtual void setBinary(const std::string& binary) = 0;
			virtual bool isBinaryNull() const { return false; }
			virtual void setBinaryNull() = 0;
			virtual bool isBinaryWritable() const { return true; }

			virtual std::string getRotation() const = 0;
			virtual void setRotation(const std::string& rotation) = 0;
			virtual bool isRotationNull() const { return false; }
			virtual void setRotationNull() = 0;
			virtual bool isRotationWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBROMLABELI_H
