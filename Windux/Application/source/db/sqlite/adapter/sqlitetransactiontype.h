#ifndef SQLITETRANSACTIONTYPE_H
#define SQLITETRANSACTIONTYPE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBTRANSACTIONTYPEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteTransactionType : public DBTransactionTypeI
	{
		public:

			// Types and subtypes
			typedef DBTransactionTypeI interface_type;


			// Constructors and destructor
			SqliteTransactionType(SqliteRecord* r, DBTransactionTypeI* d = 0);
			~SqliteTransactionType();


			// Think about replacing this method with two friend functions from the
			// SqliteTransactionTypesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBTransactionTypeI
			virtual DBTransactionTypeI* clone() const { return new SqliteTransactionType(new SqliteRecord(*rec_)); }
			virtual const DBTransactionTypeI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getName1() const { return rec_->getField(NAME1_FIELD); }
			virtual void setName1(const std::string& name) { rec_->setField(NAME1_FIELD, name); }
			virtual bool isName1Null() const { return rec_->isFieldNull(NAME1_FIELD); }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const { return rec_->getField(NAME2_FIELD); }
			virtual void setName2(const std::string& name) { rec_->setField(NAME2_FIELD, name); }
			virtual bool isName2Null() const { return rec_->isFieldNull(NAME2_FIELD); }
			virtual bool isName2Writable() const { return true; }

			virtual int getAffectingProduction() const { return rec_->getField(AFFECTINGPRODUCTION_FIELD); }
			virtual void setAffectingProduction(int affects) { rec_->setField(AFFECTINGPRODUCTION_FIELD, affects); }
			virtual bool isAffectingProductionNull() const { return rec_->isFieldNull(AFFECTINGPRODUCTION_FIELD); }
			virtual void setAffectingProductionNull() { rec_->setFieldNull(AFFECTINGPRODUCTION_FIELD); }
			virtual bool isAffectingProductionWritable() const { return true; }

			virtual bool getAllowingBackDating() const { return util_string::stringtobool(rec_->getField(ALLOWINGBACKDATING_FIELD)); }
			virtual void setAllowingBackDating(bool fp) { rec_->setField(ALLOWINGBACKDATING_FIELD, Bool::toString(fp)); }
			virtual bool isAllowingBackDatingNull() const { return rec_->isFieldNull(ALLOWINGBACKDATING_FIELD); }
			virtual void setAllowingBackDatingNull() { rec_->setFieldNull(ALLOWINGBACKDATING_FIELD); }
			virtual bool isAllowingBackDatingWritable() const { return true; }

			virtual bool getAutomaticallyChangingUPCDigit() const { return util_string::stringtobool(rec_->getField(AUTOMATICALLYCHANGINGUPCDIGIT_FIELD)); }
			virtual void setAutomaticallyChangingUPCDigit(bool fp) { rec_->setField(AUTOMATICALLYCHANGINGUPCDIGIT_FIELD, Bool::toString(fp)); }
			virtual bool isAutomaticallyChangingUPCDigitNull() const { return rec_->isFieldNull(AUTOMATICALLYCHANGINGUPCDIGIT_FIELD); }
			virtual void setAutomaticallyChangingUPCDigitNull() { rec_->setFieldNull(AUTOMATICALLYCHANGINGUPCDIGIT_FIELD); }
			virtual bool isAutomaticallyChangingUPCDigitWritable() const { return true; }

			virtual bool getManuallyChangingUPCDigit() const { return util_string::stringtobool(rec_->getField(MANUALLYCHANGINGUPCDIGIT_FIELD)); }
			virtual void setManuallyChangingUPCDigit(bool fp) { rec_->setField(MANUALLYCHANGINGUPCDIGIT_FIELD, Bool::toString(fp)); }
			virtual bool isManuallyChangingUPCDigitNull() const { return rec_->isFieldNull(MANUALLYCHANGINGUPCDIGIT_FIELD); }
			virtual void setManuallyChangingUPCDigitNull() { rec_->setFieldNull(MANUALLYCHANGINGUPCDIGIT_FIELD); }
			virtual bool isManuallyChangingUPCDigitWritable() const { return true; }

			virtual std::string getUPCDigit() const { return rec_->getField(UPCDIGIT_FIELD); }
			virtual void setUPCDigit(const std::string& upct) { rec_->setField(UPCDIGIT_FIELD, upct); }
			virtual bool isUPCDigitNull() const { return rec_->isFieldNull(UPCDIGIT_FIELD); }
			virtual void setUPCDigitNull() { rec_->setFieldNull(UPCDIGIT_FIELD); }
			virtual bool isUPCDigitWritable() const { return true; }

			virtual bool getUseBackgroundColor() const { return util_string::stringtobool(rec_->getField(USEBACKGROUNDCOLOR_FIELD)); }
			virtual void setUseBackgroundColor(bool fp) { rec_->setField(USEBACKGROUNDCOLOR_FIELD, Bool::toString(fp)); }
			virtual bool isUseBackgroundColorNull() const { return rec_->isFieldNull(USEBACKGROUNDCOLOR_FIELD); }
			virtual void setUseBackgroundColorNull() { rec_->setFieldNull(USEBACKGROUNDCOLOR_FIELD); }
			virtual bool isUseBackgroundColorWritable() const { return true; }

			virtual int getBackgroundColor() const { return rec_->getField(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColor(int affects) { rec_->setField(BACKGROUNDCOLOR_FIELD, affects); }
			virtual bool isBackgroundColorNull() const { return rec_->isFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual void setBackgroundColorNull() { rec_->setFieldNull(BACKGROUNDCOLOR_FIELD); }
			virtual bool isBackgroundColorWritable() const { return true; }

			virtual std::string getTextForLabel() const { return rec_->getField(TEXTFORLABEL_FIELD); }
			virtual void setTextForLabel(const std::string& t) { rec_->setField(TEXTFORLABEL_FIELD, t); }
			virtual bool isTextForLabelNull() const { return rec_->isFieldNull(TEXTFORLABEL_FIELD); }
			virtual void setTextForLabelNull() { rec_->setFieldNull(TEXTFORLABEL_FIELD); }
			virtual bool isTextForLabelWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string AFFECTINGPRODUCTION_FIELD;
			static const std::string ALLOWINGBACKDATING_FIELD;
			static const std::string AUTOMATICALLYCHANGINGUPCDIGIT_FIELD;
			static const std::string MANUALLYCHANGINGUPCDIGIT_FIELD;
			static const std::string UPCDIGIT_FIELD;
			static const std::string USEBACKGROUNDCOLOR_FIELD;
			static const std::string BACKGROUNDCOLOR_FIELD;
			static const std::string TEXTFORLABEL_FIELD;


		private:

			SqliteRecord* rec_;
			DBTransactionTypeI* decorated_;

			// Do not allow the default copy constructor.
			SqliteTransactionType(const SqliteTransactionType& rec);
			// Do not allow the default assignment operator.
			SqliteTransactionType operator=(const SqliteTransactionType& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITETRANSACTIONTYPE_H
