#ifndef DBTRANSACTIONTYPEI_H
#define DBTRANSACTIONTYPEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBTransactionTypeI
	{
		public:

			virtual ~DBTransactionTypeI() {}


			virtual DBTransactionTypeI* clone() const = 0;
			virtual const DBTransactionTypeI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getName1() const = 0;
			virtual void setName1(const std::string& name) = 0;
			virtual bool isName1Null() const { return false; }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const = 0;
			virtual void setName2(const std::string& name) = 0;
			virtual bool isName2Null() const { return false; }
			virtual bool isName2Writable() const { return true; }

			virtual int getAffectingProduction() const = 0;
			virtual void setAffectingProduction(int affects) = 0;
			virtual bool isAffectingProductionNull() const { return false; }
			virtual void setAffectingProductionNull() = 0;
			virtual bool isAffectingProductionWritable() const { return true; }

			virtual bool getAllowingBackDating() const = 0;
			virtual void setAllowingBackDating(bool abd) = 0;
			virtual bool isAllowingBackDatingNull() const { return false; }
			virtual void setAllowingBackDatingNull() = 0;
			virtual bool isAllowingBackDatingWritable() const { return true; }

			virtual bool getManuallyChangingUPCDigit() const = 0;
			virtual void setManuallyChangingUPCDigit(bool mcut) = 0;
			virtual bool isManuallyChangingUPCDigitNull() const { return false; }
			virtual void setManuallyChangingUPCDigitNull() = 0;
			virtual bool isManuallyChangingUPCDigitWritable() const { return true; }

			virtual bool getAutomaticallyChangingUPCDigit() const = 0;
			virtual void setAutomaticallyChangingUPCDigit(bool acut) = 0;
			virtual bool isAutomaticallyChangingUPCDigitNull() const { return false; }
			virtual void setAutomaticallyChangingUPCDigitNull() = 0;
			virtual bool isAutomaticallyChangingUPCDigitWritable() const { return true; }

			virtual std::string getUPCDigit() const = 0;
			virtual void setUPCDigit(const std::string& ut) = 0;
			virtual bool isUPCDigitNull() const { return false; }
			virtual void setUPCDigitNull() = 0;
			virtual bool isUPCDigitWritable() const { return true; }

			virtual bool getUseBackgroundColor() const = 0;
			virtual void setUseBackgroundColor(bool acut) = 0;
			virtual bool isUseBackgroundColorNull() const { return false; }
			virtual void setUseBackgroundColorNull() = 0;
			virtual bool isUseBackgroundColorWritable() const { return true; }

			virtual int getBackgroundColor() const = 0;
			virtual void setBackgroundColor(int affects) = 0;
			virtual bool isBackgroundColorNull() const { return false; }
			virtual void setBackgroundColorNull() = 0;
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual std::string getTextForLabel() const = 0;
			virtual void setTextForLabel(const std::string& ut) = 0;
			virtual bool isTextForLabelNull() const { return false; }
			virtual void setTextForLabelNull() = 0;
			virtual bool isTextForLabelWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBTRANSACTIONTYPEI_H
