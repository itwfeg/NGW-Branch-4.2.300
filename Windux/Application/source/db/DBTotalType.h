#ifndef DBTOTALTYPE_H
#define DBTOTALTYPE_H


#include <string>


#include "edb.h"
#include DBTOTALTYPEI_INC
#if defined( __linux__ )
    #include DBTOTALTYPESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;
	

	class DBTotalType
	{
			// DBTotalTypesRM class is a friend of DBTotalType because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBTotalTypesRM;
			friend class DBQuery<DBTotalType>;
			friend class DBProductTotalsRM;     // Needs access to decorated_ for cross-referencing.
			friend class DBClassTotalsRM;       // Needs access to decorated_ for cross-referencing.
			friend class DBCoolTotalsRM;        // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBTotalTypeI interface_type;
			typedef DBTotalTypesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBTotalType(const DBTotalType& rec);
			~DBTotalType();


			// Methods associated with DBTotalTypeI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			bool getReportsToPrepackTotals() const { return decorated_->getReportsToPrepackTotals(); }
			void setReportsToPrepackTotals(bool fbc) { decorated_->setReportsToPrepackTotals(fbc); }
			bool isReportsToPrepackTotalsNull() const { return decorated_->isReportsToPrepackTotalsNull(); }
			void setReportsToPrepackTotalsNull() { decorated_->setReportsToPrepackTotalsNull(); }
			bool isReportsToPrepackTotalsWritable() const { return decorated_->isReportsToPrepackTotalsWritable(); }

			bool getAllowBackDating() const { return decorated_->getAllowBackDating(); }
			void setAllowBackDating(bool fbc) { decorated_->setAllowBackDating(fbc); }
			bool isAllowBackDatingNull() const { return decorated_->isAllowBackDatingNull(); }
			void setAllowBackDatingNull() { decorated_->setAllowBackDatingNull(); }
			bool isAllowBackDatingWritable() const { return decorated_->isAllowBackDatingWritable(); }

			bool getManuallyChangeUPCDigit() const { return decorated_->getManuallyChangeUPCDigit(); }
			void setManuallyChangeUPCDigit(bool fbc) { decorated_->setManuallyChangeUPCDigit(fbc); }
			bool isManuallyChangeUPCDigitNull() const { return decorated_->isManuallyChangeUPCDigitNull(); }
			void setManuallyChangeUPCDigitNull() { decorated_->setManuallyChangeUPCDigitNull(); }
			bool isManuallyChangeUPCDigitWritable() const { return decorated_->isManuallyChangeUPCDigitWritable(); }

			bool getAutomaticallyChangeUPCDigit() const { return decorated_->getAutomaticallyChangeUPCDigit(); }
			void setAutomaticallyChangeUPCDigit(bool fbc) { decorated_->setAutomaticallyChangeUPCDigit(fbc); }
			bool isAutomaticallyChangeUPCDigitNull() const { return decorated_->isAutomaticallyChangeUPCDigitNull(); }
			void setAutomaticallyChangeUPCDigitNull() { decorated_->setAutomaticallyChangeUPCDigitNull(); }
			bool isAutomaticallyChangeUPCDigitWritable() const { return decorated_->isAutomaticallyChangeUPCDigitWritable(); }

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


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBTotalType& rec);


		private:

			edb_adapter::DBTotalTypeI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBTotalType(edb_adapter::DBTotalTypeI* d);

			// Do not allow the default assignment operator.
			DBTotalType operator=(const DBTotalType& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTOTALTYPE_H
