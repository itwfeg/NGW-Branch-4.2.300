#ifndef DBLABELTEXTI_H
#define DBLABELTEXTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBLabelTextI
	{
		public:

			virtual ~DBLabelTextI() {}


			virtual DBLabelTextI* clone() const = 0;
			virtual const DBLabelTextI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getText() const = 0;
			virtual void setText(const std::string& text) = 0;
			virtual bool isTextNull() const { return false; }
			virtual void setTextNull() = 0;
			virtual bool isTextWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBLABELTEXTI_H
