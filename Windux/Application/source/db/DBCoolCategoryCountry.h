#ifndef DBCOOLCATEGORYCOUNTRY_H
#define DBCOOLCATEGORYCOUNTRY_H


#include <string>


#include "edb.h"
#include DBCOOLCATEGORYCOUNTRYI_INC
#if defined( __linux__ )
    #include DBCOOLCATEGORYCOUNTRIESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolCategoryCountry
	{
			// DBCoolCategoryCountriesRM class is a friend of DBCoolCategoryCountry because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolCategoryCountriesRM;
			friend class DBCoolCountriesRM;               // Needs access to decorated_
			friend class DBQuery<DBCoolCategoryCountry>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolCategoryCountryI interface_type;
			typedef DBCoolCategoryCountriesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolCategoryCountry(const DBCoolCategoryCountry& rec);
			~DBCoolCategoryCountry();


			// Methods associated with DBCoolCategoryCountryI
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
			friend std::ostream& operator<<(std::ostream& os, const DBCoolCategoryCountry& rec);


		private:

			edb_adapter::DBCoolCategoryCountryI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolCategoryCountry(edb_adapter::DBCoolCategoryCountryI* d);

			// Do not allow the default assignment operator.
			DBCoolCategoryCountry operator=(const DBCoolCategoryCountry& rhs);


			std::string getCountryKey() const { return decorated_->getCountryKey(); }
			void setCountryKey(const std::string& countryKey) { decorated_->setCountryKey(countryKey); }
			bool isCountryKeyDNull() const { return decorated_->isCountryKeyNull(); }
			bool isCountryKeyWritable() const { return decorated_->isCountryKeyWritable(); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYCOUNTRY_H
