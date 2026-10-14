#ifndef DBPRESENTATIONMETHODI_H
#define DBPRESENTATIONMETHODI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodI
	{
		public:

			virtual ~DBPresentationMethodI() {}


			virtual DBPresentationMethodI* clone() const = 0;
			virtual const DBPresentationMethodI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getType() const = 0;
			virtual void setType(const std::string& type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getDescription() const = 0;
			virtual void setDescription(const std::string& desc) = 0;
			virtual bool isDescriptionNull() const { return false; }
			virtual void setDescriptionNull() = 0;
			virtual bool isDescriptionWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODI_H
