#ifndef DBRECALLI_H
#define DBRECALLI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBRecallI
	{
		public:

			virtual ~DBRecallI() {}


			virtual DBRecallI* clone() const = 0;
			virtual const DBRecallI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getName() const = 0;
			virtual void setName(const std::string& name) = 0;
			virtual bool isNameNull() const { return false; }
			virtual void setNameNull() = 0;
			virtual bool isNameWritable() const { return true; }

			virtual std::string getText() const = 0;
			virtual void setText(const std::string& text) = 0;
			virtual bool isTextNull() const { return false; }
			virtual void setTextNull() = 0;
			virtual bool isTextWritable() const { return true; }
			
			virtual std::string getReceivedDateTime() const = 0;
			virtual void setReceivedDateTime(const std::string& date) = 0;
			virtual bool isReceivedDateTimeNull() const { return false; }
			virtual void setReceivedDateTimeNull() = 0;
			virtual bool isReceivedDateTimeWritable() const { return true; }

			virtual bool getAcknowledged() const = 0;
			virtual void setAcknowledged(bool ack) = 0;
			virtual bool isAcknowledgedNull() const { return false; }
			virtual void setAcknowledgedNull() = 0;
			virtual bool isAcknowledgedWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBRECALLI_H
