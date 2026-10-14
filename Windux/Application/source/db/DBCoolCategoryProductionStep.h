#ifndef DBCOOLCATEGORYPRODUCTIONSTEP_H
#define DBCOOLCATEGORYPRODUCTIONSTEP_H


#include <string>


#include "edb.h"
#include DBCOOLCATEGORYPRODUCTIONSTEPI_INC
#if defined( __linux__ )
    #include DBCOOLCATEGORYPRODUCTIONSTEPSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolCategoryProductionStep
	{
			// DBCoolCategoryProductionStepsRM class is a friend of DBCoolCategoryProductionStep because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolCategoryProductionStepsRM;
			friend class DBCoolProductionStepsRM;                // Needs access to decorated_
			friend class DBQuery<DBCoolCategoryProductionStep>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolCategoryProductionStepI interface_type;
			typedef DBCoolCategoryProductionStepsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolCategoryProductionStep(const DBCoolCategoryProductionStep& rec);
			~DBCoolCategoryProductionStep();


			// Methods associated with DBCoolCategoryProductionStepI
			std::string getCategoryNumber() const { return decorated_->getCategoryNumber(); }
			void setCategoryNumber(const std::string& category) { decorated_->setCategoryNumber(category); }
			bool isCategoryNumberNull() const { return decorated_->isCategoryNumberNull(); }
			bool isCategoryNumberWritable() const { return decorated_->isCategoryNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getCategoryNumber(); }
			void setKey1(const std::string& type) { decorated_->setCategoryNumber(type); }
			std::string getKey2() const { return decorated_->getCategoryNumber(); }
			void setKey2(const std::string& key) { decorated_->setCategoryNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolCategoryProductionStep& rec);


		private:

			edb_adapter::DBCoolCategoryProductionStepI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolCategoryProductionStep(edb_adapter::DBCoolCategoryProductionStepI* d);

			// Do not allow the default assignment operator.
			DBCoolCategoryProductionStep operator=(const DBCoolCategoryProductionStep& rhs);


			std::string getProductionStepKey() const { return decorated_->getProductionStepKey(); }
			void setProductionStepKey(const std::string& stepKey) { decorated_->setProductionStepKey(stepKey); }
			bool isProductionStepKeyNull() const { return decorated_->isProductionStepKeyNull(); }
			bool isProductionStepKeyWritable() const { return decorated_->isProductionStepKeyWritable(); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYPRODUCTIONSTEP_H
