#ifndef DBCOOLPRETEXT_H
#define DBCOOLPRETEXT_H


#include <string>


#include "edb.h"
#include DBCOOLPRETEXTI_INC
#if defined( __linux__ )
    #include DBCOOLPRETEXTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolPreText
	{
			// DBCoolPreTextsRM class is a friend of DBCoolPreText because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolPreTextsRM;
			friend class DBQuery<DBCoolPreText>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolPreTextI interface_type;
			typedef DBCoolPreTextsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolPreText(const DBCoolPreText& rec);  
			~DBCoolPreText();


			// Methods associated with DBCoolPreTextI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

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


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolPreText& rec);


		private:

			edb_adapter::DBCoolPreTextI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolPreText(edb_adapter::DBCoolPreTextI* d);

			// Do not allow the default assignment operator.
			DBCoolPreText operator=(const DBCoolPreText& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLPRETEXT_H
