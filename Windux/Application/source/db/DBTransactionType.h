#ifndef DBTRANSACTIONTYPE_H
#define DBTRANSACTIONTYPE_H


#include <string>


#include "edb.h"
#include DBTRANSACTIONTYPEI_INC
#if defined( __linux__ )
    #include DBTRANSACTIONTYPESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBTransactionType
	{
			// DBTransactionTypesRM class is a friend of DBTransactionType because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBTransactionTypesRM;
			friend class DBQuery<DBTransactionType>;
			friend class DBTransactionssRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBTransactionTypeI interface_type;
			typedef DBTransactionTypesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBTransactionType(const DBTransactionType& rec);
			~DBTransactionType();


			// Methods associated with DBTransactionTypeI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getName1() const { return decorated_->getName1(); }
			void setName1(const std::string& name) { decorated_->setName1(name); }
			bool isName1Null() const { return decorated_->isName1Null(); }
			bool isName1Writable() const { return decorated_->isName1Writable(); }

			std::string getName2() const { return decorated_->getName2(); }
			void setName2(const std::string& name) { decorated_->setName2(name); }
			bool isName2Null() const { return decorated_->isName2Null(); }
			bool isName2Writable() const { return decorated_->isName2Writable(); }

			int getAffectingProduction() const { return decorated_->getAffectingProduction(); }
			void setAffectingProduction(int affects) { decorated_->setAffectingProduction(affects); }
			bool isAffectingProductionNull() const { return decorated_->isAffectingProductionNull(); }
			void setAffectingProductionNull() { decorated_->setAffectingProductionNull(); }
			bool isAffectingProductionWritable() const { return decorated_->isAffectingProductionWritable(); }

			bool getAllowingBackDating() const { return decorated_->getAllowingBackDating(); }
			void setAllowingBackDating(bool fbc) { decorated_->setAllowingBackDating(fbc); }
			bool isAllowingBackDatingNull() const { return decorated_->isAllowingBackDatingNull(); }
			void setAllowingBackDatingNull() { decorated_->setAllowingBackDatingNull(); }
			bool isAllowingBackDatingWritable() const { return decorated_->isAllowingBackDatingWritable(); }

			bool getManuallyChangingUPCDigit() const { return decorated_->getManuallyChangingUPCDigit(); }
			void setManuallyChangingUPCDigit(bool fbc) { decorated_->setManuallyChangingUPCDigit(fbc); }
			bool isManuallyChangingUPCDigitNull() const { return decorated_->isManuallyChangingUPCDigitNull(); }
			void setManuallyChangingUPCDigitNull() { decorated_->setManuallyChangingUPCDigitNull(); }
			bool isManuallyChangingUPCDigitWritable() const { return decorated_->isManuallyChangingUPCDigitWritable(); }

			bool getAutomaticallyChangingUPCDigit() const { return decorated_->getAutomaticallyChangingUPCDigit(); }
			void setAutomaticallyChangingUPCDigit(bool fbc) { decorated_->setAutomaticallyChangingUPCDigit(fbc); }
			bool isAutomaticallyChangingUPCDigitNull() const { return decorated_->isAutomaticallyChangingUPCDigitNull(); }
			void setAutomaticallyChangingUPCDigitNull() { decorated_->setAutomaticallyChangingUPCDigitNull(); }
			bool isAutomaticallyChangingUPCDigitWritable() const { return decorated_->isAutomaticallyChangingUPCDigitWritable(); }

			std::string getUPCDigit() const { return decorated_->getUPCDigit(); }
			void setUPCDigit(const std::string& bc) { decorated_->setUPCDigit(bc); }
			bool isUPCDigitNull() const { return decorated_->isUPCDigitNull(); }
			void setUPCDigitNull() { decorated_->setUPCDigitNull(); }
			bool isUPCDigitWritable() const { return decorated_->isUPCDigitWritable(); }

			bool getUseBackgroundColor() const { return decorated_->getUseBackgroundColor(); }
			void setUseBackgroundColor(bool fbc) { decorated_->setUseBackgroundColor(fbc); }
			bool isUseBackgroundColorNull() const { return decorated_->isUseBackgroundColorNull(); }
			void setUseBackgroundColorNull() { decorated_->setUseBackgroundColorNull(); }
			bool isUseBackgroundColorWritable() const { return decorated_->isUseBackgroundColorWritable(); }

			int getBackgroundColor() const { return decorated_->getBackgroundColor(); }
			void setBackgroundColor(int affects) { decorated_->setBackgroundColor(affects); }
			bool isBackgroundColorNull() const { return decorated_->isBackgroundColorNull(); }
			void setBackgroundColorNull() { decorated_->setBackgroundColorNull(); }
			bool isBackgroundColorWritable() const { return decorated_->isBackgroundColorWritable(); }

			std::string getTextForLabel() const { return decorated_->getTextForLabel(); }
			void setTextForLabel(const std::string& t) { decorated_->setTextForLabel(t); }
			bool isTextForLabelNull() const { return decorated_->isTextForLabelNull(); }
			void setTextForLabelNull() { decorated_->setTextForLabelNull(); }
			bool isTextForLabelWritable() const { return decorated_->isTextForLabelWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBTransactionType& rec);


		private:

			edb_adapter::DBTransactionTypeI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBTransactionType(edb_adapter::DBTransactionTypeI* d);

			// Do not allow the default assignment operator.
			DBTransactionType operator=(const DBTransactionType& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTRANSACTIONTYPE_H
