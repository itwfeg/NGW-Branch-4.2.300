#ifndef DBCOOLCOUNTRY_H
#define DBCOOLCOUNTRY_H


#include <string>


#include "edb.h"
#include DBCOOLCOUNTRYI_INC
#if defined( __linux__ )
    #include DBCOOLCOUNTRIESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolCountry
	{
			// DBCoolCountriesRM class is a friend of DBCoolCountry because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolCountriesRM;
			friend class DBQuery<DBCoolCountry>;
			friend class DBCoolCategoryCountriesRM;      // Needs access to decorated_

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolCountryI interface_type;
			typedef DBCoolCountriesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolCountry(const DBCoolCountry& rec);  
			~DBCoolCountry();


			// Methods associated with DBCoolCountryI
			std::string getCountryKey() const { return decorated_->getCountryKey(); }
			void setCountryKey(const std::string& key) { decorated_->setCountryKey(key); }
			bool isCountryKeyNull() const { return decorated_->isCountryKeyNull(); }
			bool isCountryKeyWritable() const { return decorated_->isCountryKeyWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getName1() const { return decorated_->getName1(); }
			void setName1(const std::string& desc) { decorated_->setName1(desc); }
			bool isName1Null() const { return decorated_->isName1Null(); }
			void setName1Null() { decorated_->setName1Null(); }
			bool isName1Writable() const { return decorated_->isName1Writable(); }

			std::string getName2() const { return decorated_->getName2(); }
			void setName2(const std::string& desc) { decorated_->setName2(desc); }
			bool isName2Null() const { return decorated_->isName2Null(); }
			void setName2Null() { decorated_->setName2Null(); }
			bool isName2Writable() const { return decorated_->isName2Writable(); }

			bool getAllCategories() const { return decorated_->getAllCategories(); }
			void setAllCategories(bool all) { decorated_->setAllCategories(all); }
			bool isAllCategoriesNull() const { return decorated_->isAllCategoriesNull(); }
			void setAllCategoriesNull() { decorated_->setAllCategoriesNull(); }
			bool isAllCategoriesWritable() const { return decorated_->isAllCategoriesWritable(); }

			std::string getCountryNumber() const { return decorated_->getCountryNumber(); }
			void setCountryNumber(const std::string& key) { decorated_->setCountryNumber(key); }
			bool isCountryNumberNull() const { return decorated_->isCountryNumberNull(); }
			bool isCountryNumberWritable() const { return decorated_->isCountryNumberWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getName1(); }
			void setKey(const std::string& key) { decorated_->setName1(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolCountry& rec);


		private:

			edb_adapter::DBCoolCountryI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolCountry(edb_adapter::DBCoolCountryI* d);

			// Do not allow the default assignment operator.
			DBCoolCountry operator=(const DBCoolCountry& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCOUNTRY_H
