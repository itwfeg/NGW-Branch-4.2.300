#ifndef DBSPECIALPROGRAMI_H
#define DBSPECIALPROGRAMI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSpecialProgramI
	{
		public:

			virtual ~DBSpecialProgramI() {}


			virtual DBSpecialProgramI* clone() const = 0;
			virtual const DBSpecialProgramI* getDecorated() const = 0;


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

			virtual std::string getDescription() const = 0;
			virtual void setDescription(const std::string& desc) = 0;
			virtual bool isDescriptionNull() const { return false; }
			virtual void setDescriptionNull() = 0;
			virtual bool isDescriptionWritable() const { return true; }

			virtual int getDefaultLength() const = 0;
			virtual void setDefaultLength(int length) = 0;
			virtual bool isDefaultLengthNull() const { return false; }
			virtual void setDefaultLengthNull() = 0;
			virtual bool isDefaultLengthWritable() const { return true; }

			virtual std::string getImage1() const = 0;
			virtual void setImage1(const std::string& image) = 0;
			virtual bool isImage1Null() const { return false; }
			virtual void setImage1Null() = 0;
			virtual bool isImage1Writable() const { return true; }

			virtual std::string getImage2() const = 0;
			virtual void setImage2(const std::string& image) = 0;
			virtual bool isImage2Null() const { return false; }
			virtual void setImage2Null() = 0;
			virtual bool isImage2Writable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSPECIALPROGRAMI_H
