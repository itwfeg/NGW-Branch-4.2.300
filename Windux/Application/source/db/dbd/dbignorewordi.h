#ifndef DBIGNOREWORDI_H
#define DBIGNOREWORDI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBIgnoreWordI
	{
		public:

			virtual ~DBIgnoreWordI() {}


			virtual DBIgnoreWordI* clone() const = 0;
			virtual const DBIgnoreWordI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getName() const = 0;
			virtual void setName(const std::string& name) = 0;
			virtual bool isNameNull() const { return false; }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getLanguage() const = 0;
			virtual void setLanguage(const std::string& language) = 0;
			virtual bool isLanguageNull() const { return false; }
			virtual bool isLanguageWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBIGNOREWORDI_H
