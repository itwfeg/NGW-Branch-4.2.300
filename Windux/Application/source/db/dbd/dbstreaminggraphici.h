#ifndef DBSTREAMINGGRAPHICI_H
#define DBSTREAMINGGRAPHICI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBStreamingGraphicI
	{
		public:

			virtual ~DBStreamingGraphicI() {}


			virtual DBStreamingGraphicI* clone() const = 0;
			virtual const DBStreamingGraphicI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getImage() const = 0;
			virtual void setImage(const std::string& image) = 0;
			virtual bool isImageNull() const { return false; }
			virtual void setImageNull() = 0;
			virtual bool isImageWritable() const { return true; }

			virtual int getTime() const = 0;
			virtual void setTime(const int time) = 0;
			virtual bool isTimeNull() const { return false; }
			virtual void setTimeNull() = 0;
			virtual bool isTimeWritable() const { return true; }

			virtual int getActionType() const = 0;
			virtual void setActionType(const int type) = 0;
			virtual bool isActionTypeNull() const { return false; }
			virtual void setActionTypeNull() = 0;
			virtual bool isActionTypeWritable() const { return true; }

			virtual std::string getActionParameter() const = 0;
			virtual void setActionParameter(const std::string& par) = 0;
			virtual bool isActionParameterNull() const { return false; }
			virtual void setActionParameterNull() = 0;
			virtual bool isActionParameterWritable() const { return true; }

			virtual std::string getNext() const = 0;
			virtual void setNext(const std::string& next) = 0;
			virtual bool isNextNull() const { return false; }
			virtual void setNextNull() = 0;
			virtual bool isNextWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSTREAMINGGRAPHICI_H
