#ifndef DBNUTRITION_H
#define DBNUTRITION_H


#include <string>


#include "edb.h"
#include DBNUTRITIONI_INC
#if defined( __linux__ )
    #include DBNUTRITIONSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBNutrition
	{
			// DBNutritionsRM class is a friend of DBNutrition because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBNutritionsRM;
			friend class DBQuery<DBNutrition>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBNutritionI interface_type;
			typedef DBNutritionsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBNutrition(const DBNutrition& rec);
			~DBNutrition();


			// Methods associated with DBNutritionI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getNutritionColumn1() const { return decorated_->getNutritionColumn1(); }
			void setNutritionColumn1(const std::string& col1) { decorated_->setNutritionColumn1(col1); }
			bool isNutritionColumn1Null() const { return decorated_->isNutritionColumn1Null(); }
			void setNutritionColumn1Null() { decorated_->setNutritionColumn1Null(); }
			bool isNutritionColumn1Writable() const { return decorated_->isNutritionColumn1Writable(); }

			std::string getNutritionColumn2() const { return decorated_->getNutritionColumn2(); }
			void setNutritionColumn2(const std::string& col2) { decorated_->setNutritionColumn2(col2); }
			bool isNutritionColumn2Null() const { return decorated_->isNutritionColumn2Null(); }
			void setNutritionColumn2Null() { decorated_->setNutritionColumn2Null(); }
			bool isNutritionColumn2Writable() const { return decorated_->isNutritionColumn2Writable(); }

			std::string getNutritionColumn3() const { return decorated_->getNutritionColumn3(); }
			void setNutritionColumn3(const std::string& col3) { decorated_->setNutritionColumn3(col3); }
			bool isNutritionColumn3Null() const { return decorated_->isNutritionColumn3Null(); }
			void setNutritionColumn3Null() { decorated_->setNutritionColumn3Null(); }
			bool isNutritionColumn3Writable() const { return decorated_->isNutritionColumn3Writable(); }

			std::string getNutritionColumn4() const { return decorated_->getNutritionColumn4(); }
			void setNutritionColumn4(const std::string& col4) { decorated_->setNutritionColumn4(col4); }
			bool isNutritionColumn4Null() const { return decorated_->isNutritionColumn4Null(); }
			void setNutritionColumn4Null() { decorated_->setNutritionColumn4Null(); }
			bool isNutritionColumn4Writable() const { return decorated_->isNutritionColumn4Writable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBNutrition& rec);


		private:

			edb_adapter::DBNutritionI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBNutrition(edb_adapter::DBNutritionI* d);

			// Do not allow the default assignment operator.
			DBNutrition operator=(const DBNutrition& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBNUTRITION_H
