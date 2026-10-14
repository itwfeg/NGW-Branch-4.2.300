#ifndef DBCOOLLISTI_H
#define DBCOOLLISTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListTextI;


	class DBCoolListI
	{
		public:

			virtual ~DBCoolListI() {}


			virtual DBCoolListI* clone() const = 0;
			virtual const DBCoolListI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getListKey() const = 0;
			virtual void setListKey(const std::string& key) = 0;
			virtual bool isListKeyNull() const { return false; }
			virtual bool isListKeyWritable() const { return true; }

			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual void setNumberNull() = 0;
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getFirstLinkNumber() const = 0;
			virtual void setFirstLinkNumber(const std::string& first) = 0;
			virtual bool isFirstLinkNumberNull() const { return false; }
			virtual void setFirstLinkNumberNull() = 0;
			virtual bool isFirstLinkNumberWritable() const { return true; }

			virtual bool containsCoolListText(const DBCoolListTextI* text) const = 0;


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCOOLLISTI_H
