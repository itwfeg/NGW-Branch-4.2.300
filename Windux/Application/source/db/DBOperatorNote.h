#ifndef DBOPERATORNOTE_H
#define DBOPERATORNOTE_H


#include <string>


#include "edb.h"
#include DBOPERATORNOTEI_INC
#if defined( __linux__ )
    #include DBOPERATORNOTESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBOperatorNote
	{
			// DBOperatorNotesRM class is a friend of DBOperatorNote because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBOperatorNotesRM;
			friend class DBQuery<DBOperatorNote>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBOperatorNoteI interface_type;
			typedef DBOperatorNotesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBOperatorNote(const DBOperatorNote& rec);  
			~DBOperatorNote();


			// Methods associated with DBOperatorNoteI
//			std::string getNumber() const { return decorated_->getNumber(); }
//			void setNumber(const std::string& number) { decorated_->setNumber(number); }
//			bool isNumberNull() const { return decorated_->isNumberNull(); }
//			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getText() const { return decorated_->getText(); }
			void setText(const std::string& text) { decorated_->setText(text); }
			bool isTextNull() const { return decorated_->isTextNull(); }
			void setTextNull() { decorated_->setTextNull(); }
			bool isTextWritable() const { return decorated_->isTextWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return ""; }
			void setKey(const std::string& /*key*/) { }	 /* (SJJC port 6/19/06) */

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBOperatorNote& rec);


		private:

			edb_adapter::DBOperatorNoteI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBOperatorNote(edb_adapter::DBOperatorNoteI* d);

			// Do not allow the default assignment operator.
			DBOperatorNote operator=(const DBOperatorNote& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBOPERATORNOTE_H
