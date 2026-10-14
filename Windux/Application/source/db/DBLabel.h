#ifndef DBLABEL_H
#define DBLABEL_H


#include <string>


#include "edb.h"
#include DBLABELI_INC
#if defined( __linux__ )
    #include DBLABELSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBLabel
	{
			// DBLabelsRM class is a friend of DBLabel because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBLabelsRM;
			friend class DBQuery<DBLabel>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBLabelI interface_type;
			typedef DBLabelsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBLabel(const DBLabel& rec); 
			~DBLabel();


			// Methods associated with DBLabelI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

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
			friend std::ostream& operator<<(std::ostream& os, const DBLabel& rec);


		private:

			edb_adapter::DBLabelI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBLabel(edb_adapter::DBLabelI* d);

			// Do not allow the default assignment operator.
			DBLabel operator=(const DBLabel& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBLABEL_H
