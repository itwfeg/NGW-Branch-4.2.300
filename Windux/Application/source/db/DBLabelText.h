#ifndef DBLABELTEXT_H
#define DBLABELTEXT_H


#include <string>


#include "edb.h"
#include DBLABELTEXTI_INC
#if defined( __linux__ )
    #include DBLABELTEXTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBLabelText
	{
			// DBLabelTextsRM class is a friend of DBLabelText because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBLabelTextsRM;
			friend class DBQuery<DBLabelText>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBLabelTextI interface_type;
			typedef DBLabelTextsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBLabelText(const DBLabelText& rec);
			~DBLabelText();


			// Methods associated with DBLabelTextI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getText() const { return decorated_->getText(); }
			void setText(const std::string& text) { decorated_->setText(text); }
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
			friend std::ostream& operator<<(std::ostream& os, const DBLabelText& rec);


		private:

			edb_adapter::DBLabelTextI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBLabelText(edb_adapter::DBLabelTextI* d);

			// Do not allow the default assignment operator.
			DBLabelText operator=(const DBLabelText& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBLABELTEXT_H
