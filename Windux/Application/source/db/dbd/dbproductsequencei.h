#ifndef DBPRODUCTSEQUENCEI_H
#define DBPRODUCTSEQUENCEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductSequenceI
	{
		public:

			virtual ~DBProductSequenceI() {}


			virtual DBProductSequenceI* clone() const = 0;
			virtual const DBProductSequenceI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getCharacterSequence() const = 0;
			virtual void setCharacterSequence(const std::string& number) = 0;
			virtual bool isCharacterSequenceNull() const { return false; }
			virtual bool isCharacterSequenceWritable() const { return true; }

			virtual std::string getCharacterSequenceLanguage() const = 0;
			virtual void setCharacterSequenceLanguage(const std::string& language) = 0;
			virtual bool isCharacterSequenceLanguageNull() const { return false; }
			virtual bool isCharacterSequenceLanguageWritable() const { return true; }

			virtual std::string getProductKey() const = 0;
			virtual void setProductKey(const std::string& id) = 0;
			virtual bool isProductKeyNull() const { return false; }
			virtual bool isProductKeyWritable() const { return true; }

			virtual std::string getProductName() const = 0;
			virtual void setProductName(const std::string& name) = 0;
			virtual bool isProductNameNull() const { return false; }
			virtual void setProductNameNull() = 0;
			virtual bool isProductNameWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTSEQUENCEI_H
