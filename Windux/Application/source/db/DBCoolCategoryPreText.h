#ifndef DBCOOLCATEGORYPRETEXT_H
#define DBCOOLCATEGORYPRETEXT_H


#include <string>


#include "edb.h"
#include DBCOOLCATEGORYPRETEXTI_INC
#if defined( __linux__ )
    #include DBCOOLCATEGORYPRETEXTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolCategoryPreText
	{
			// DBCoolCategoryPreTextsRM class is a friend of DBCoolCategoryPreText because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolCategoryPreTextsRM;
			friend class DBQuery<DBCoolCategoryPreText>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolCategoryPreTextI interface_type;
			typedef DBCoolCategoryPreTextsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolCategoryPreText(const DBCoolCategoryPreText& rec);
			~DBCoolCategoryPreText();


			// Methods associated with DBCoolCategoryPreTextI
			std::string getCategoryNumber() const { return decorated_->getCategoryNumber(); }
			void setCategoryNumber(const std::string& category) { decorated_->setCategoryNumber(category); }
			bool isCategoryNumberNull() const { return decorated_->isCategoryNumberNull(); }
			bool isCategoryNumberWritable() const { return decorated_->isCategoryNumberWritable(); }

			std::string getPreTextNumber() const { return decorated_->getPreTextNumber(); }
			void setPreTextNumber(const std::string& step) { decorated_->setPreTextNumber(step); }
			bool isPreTextNumberNull() const { return decorated_->isPreTextNumberNull(); }
			bool isPreTextNumberWritable() const { return decorated_->isPreTextNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getCategoryNumber(); }
			void setKey1(const std::string& type) { decorated_->setCategoryNumber(type); }
			std::string getKey2() const { return decorated_->getPreTextNumber(); }
			void setKey2(const std::string& key) { decorated_->setPreTextNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolCategoryPreText& rec);


		private:

			edb_adapter::DBCoolCategoryPreTextI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolCategoryPreText(edb_adapter::DBCoolCategoryPreTextI* d);

			// Do not allow the default assignment operator.
			DBCoolCategoryPreText operator=(const DBCoolCategoryPreText& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYPRETEXT_H
