#ifndef DBFLASHFILEI_H
#define DBFLASHFILEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBFlashFileI
	{
		public:

			virtual ~DBFlashFileI() {}


			virtual DBFlashFileI* clone() const = 0;
			virtual const DBFlashFileI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getName() const = 0;
			virtual void setName(const std::string& name) = 0;
			virtual bool isNameNull() const { return false; }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getDate() const = 0;
			virtual void setDate(const std::string& value) = 0;
			virtual bool isDateNull() const { return false; }
			virtual void setDateNull() = 0;
			virtual bool isDateWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBFLASHFILEI_H
