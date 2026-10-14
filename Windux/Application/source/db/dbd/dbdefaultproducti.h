#ifndef DBDEFAULTPRODUCTI_H
#define DBDEFAULTPRODUCTI_H



#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

	class DBDefaultProductI
	{
		public:
			virtual ~DBDefaultProductI() {}

			virtual DBDefaultProductI* clone() const = 0;
			virtual const DBDefaultProductI* getDecorated() const = 0;

			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;

			virtual std::string getPresentationMethodType() const = 0;
			virtual void setPresentationMethodType(const std::string& type) = 0;
			virtual bool isPresentationMethodTypeNull() const { return false; }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual std::string getProductKey() const = 0;
			virtual void setProductKey(const std::string& plu) = 0;
			virtual bool isProductKeyNull() const { return false; }
			virtual bool isProductKeyWritable() const { return true; }

			virtual std::string getLanguageType() const = 0;
			virtual void setLanguageType(const std::string& lang) = 0;
			virtual bool isLanguageTypeNull() const { return false; }
			virtual bool isLanguageTypeWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};

#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif



#endif // DBDEFAULTPRODUCTI_H

