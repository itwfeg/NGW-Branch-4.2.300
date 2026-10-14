#ifndef DBROMLABEL_H
#define DBROMLABEL_H


#include <string>


#include "edb.h"
#include DBROMLABELI_INC
#if defined( __linux__ )
    #include DBROMLABELSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBROMLabel
	{
			// DBROMLabelsRM class is a friend of DBROMLabel because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBROMLabelsRM;
			friend class DBQuery<DBROMLabel>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBROMLabelI interface_type;
			typedef DBROMLabelsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBROMLabel(const DBROMLabel& rec);
			~DBROMLabel();


			// Methods associated with DBROMLabelI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getBinary() const { return decorated_->getBinary(); }
			void setBinary(const std::string& binary) { decorated_->setBinary(binary); }
			bool isBinaryNull() const { return decorated_->isBinaryNull(); }
			void setBinaryNull() { decorated_->setBinaryNull(); }
			bool isBinaryWritable() const { return decorated_->isBinaryWritable(); }

			std::string getRotation() const { return decorated_->getRotation(); }
			void setRotation(const std::string& rotation) { decorated_->setRotation(rotation); }
			bool isRotationNull() const { return decorated_->isRotationNull(); }
			void setRotationNull() { decorated_->setRotationNull(); }
			bool isRotationWritable() const { return decorated_->isRotationWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBROMLabel& rec);


		private:

			edb_adapter::DBROMLabelI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBROMLabel(edb_adapter::DBROMLabelI* d);

			// Do not allow the default assignment operator.
			DBROMLabel operator=(const DBROMLabel& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBROMLABEL_H
