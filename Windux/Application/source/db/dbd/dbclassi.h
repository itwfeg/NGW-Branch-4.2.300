#ifndef DBCLASSI_H
#define DBCLASSI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBClassI
	{
		public:

			virtual ~DBClassI() {}


			virtual DBClassI* clone() const = 0;
			virtual const DBClassI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getReportingClass() const = 0;
			virtual void setReportingClass(const std::string& rc) = 0;
			virtual bool isReportingClassNull() const { return false; }
			virtual void setReportingClassNull() = 0;
			virtual bool isReportingClassWritable() const { return true; }

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


#endif // DBCLASSI_H
