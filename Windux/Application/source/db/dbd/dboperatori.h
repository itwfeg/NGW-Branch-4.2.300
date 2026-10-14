#ifndef DBOPERATORI_H
#define DBOPERATORI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBOperatorI
	{
		public:

			virtual ~DBOperatorI() {}


			virtual DBOperatorI* clone() const = 0;
			virtual const DBOperatorI* getDecorated() const = 0;


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

			virtual std::string getLanguage() const = 0;
			virtual void setLanguage(const std::string& lang) = 0;
			virtual bool isLanguageNull() const { return false; }
			virtual void setLanguageNull() = 0;
			virtual bool isLanguageWritable() const { return true; }
			
			virtual std::string getRFID() const = 0;
			virtual void setRFID(const std::string& id) = 0;
			virtual bool isRFIDNull() const { return false; }
			virtual void setRFIDNull() = 0;
			virtual bool isRFIDWritable() const { return true; }

			virtual int getClassification() const = 0;
			virtual void setClassification(int c) = 0;
			virtual bool isClassificationNull() const { return false; }
			virtual void setClassificationNull() = 0;
			virtual bool isClassificationWritable() const { return true; }

			virtual bool getSawNote() const = 0;
			virtual void setSawNote(bool saw) = 0;
			virtual bool isSawNoteNull() const { return false; }
			virtual void setSawNoteNull() = 0;
			virtual bool isSawNoteWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBOPERATORI_H
