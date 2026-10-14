#ifndef DBOPERATORNOTEI_H
#define DBOPERATORNOTEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBOperatorNoteI
	{
		public:

			virtual ~DBOperatorNoteI() {}


			virtual DBOperatorNoteI* clone() const = 0;
			virtual const DBOperatorNoteI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


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


#endif // DBOPERATORNOTEI_H
