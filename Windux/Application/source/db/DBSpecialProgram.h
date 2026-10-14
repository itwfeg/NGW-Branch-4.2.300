#ifndef DBSPECIALPROGRAM_H
#define DBSPECIALPROGRAM_H


#include <string>


#include "edb.h"
#include DBSPECIALPROGRAMI_INC
#if defined( __linux__ )
    #include DBSPECIALPROGRAMSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBSpecialProgram
	{
			// DBSpecialProgramsRM class is a friend of DBSpecialProgram because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSpecialProgramsRM;
			friend class DBQuery<DBSpecialProgram>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBSpecialProgramI interface_type;
			typedef DBSpecialProgramsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSpecialProgram(const DBSpecialProgram& rec);
			~DBSpecialProgram();


			// Methods associated with DBSpecialProgramI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getDescription() const { return decorated_->getDescription(); }
			void setDescription(const std::string& desc) { decorated_->setDescription(desc); }
			bool isDescriptionNull() const { return decorated_->isDescriptionNull(); }
			void setDescriptionNull() { decorated_->setDescriptionNull(); }
			bool isDescriptionWritable() const { return decorated_->isDescriptionWritable(); }

			int getDefaultLength() const { return decorated_->getDefaultLength(); }
			void setDefaultLength(int length) { decorated_->setDefaultLength(length); }
			bool isDefaultLengthNull() const { return decorated_->isDefaultLengthNull(); }
			void setDefaultLengthNull() { decorated_->setDefaultLengthNull(); }
			bool isDefaultLengthWritable() const { return decorated_->isDefaultLengthWritable(); }

			std::string getImage1() const { return decorated_->getImage1(); }
			void setImage1(const std::string& image) { decorated_->setImage1(image); }
			bool isImage1Null() const { return decorated_->isImage1Null(); }
			void setImage1Null() { decorated_->setImage1Null(); }
			bool isImage1Writable() const { return decorated_->isImage1Writable(); }

			std::string getImage2() const { return decorated_->getImage2(); }
			void setImage2(const std::string& image) { decorated_->setImage2(image); }
			bool isImage2Null() const { return decorated_->isImage2Null(); }
			void setImage2Null() { decorated_->setImage2Null(); }
			bool isImage2Writable() const { return decorated_->isImage2Writable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSpecialProgram& rec);


		private:

			edb_adapter::DBSpecialProgramI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSpecialProgram(edb_adapter::DBSpecialProgramI* d);

			// Do not allow the default assignment operator.
			DBSpecialProgram operator=(const DBSpecialProgram& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSPECIALPROGRAM_H
