#ifndef DBPRODUCTTEXT_H
#define DBPRODUCTTEXT_H


#include <string>


#include "edb.h"
#include DBPRODUCTTEXTI_INC
#if defined( __linux__ )
    #include DBPRODUCTTEXTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBProductText
	{
			// DBProductTextsRM class is a friend of DBProductText because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBProductTextsRM;
			friend class DBQuery<DBProductText>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBProductTextI interface_type;
			typedef DBProductTextsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBProductText(const DBProductText& rec);  
			~DBProductText();


			// Methods associated with DBProductTextI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getType() const { return decorated_->getType(); }
			void setType(const std::string& type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getText() const { return decorated_->getText(); }
			void setText(const std::string& desc) { decorated_->setText(desc); }
			bool isTextNull() const { return decorated_->isTextNull(); }
			void setTextNull() { decorated_->setTextNull(); }
			bool isTextWritable() const { return decorated_->isTextWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBProductText& rec);


		private:

			edb_adapter::DBProductTextI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBProductText(edb_adapter::DBProductTextI* d);

			// Do not allow the default assignment operator.
			DBProductText operator=(const DBProductText& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTTEXT_H
