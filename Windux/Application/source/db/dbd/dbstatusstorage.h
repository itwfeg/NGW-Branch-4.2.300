#ifndef DBSTATUSSTORAGE_H
#define DBSTATUSSTORAGE_H


#include <string>


#include "edb.h"
#include DBSTATUSI_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBStatusStorage : public edb_adapter::DBStatusI
	{
			friend class DBStatusStorageRM;

		public:

			// --- Types and subtypes ---


			// --- Constructors and destructor ---
			virtual ~DBStatusStorage();


			// Methods associated with DBStatusI
			std::string getName() const { return name_; }
			void setName(const std::string& name) { name_.assign(name); }
			bool isNameNull() const { return false; }
			bool isNameWritable() const { return true; }

			std::string getValue() const { return ( value_ == 0 ) ? std::string() : *value_; }
			void setValue(const std::string& value) { if ( !value_ ) value_ = new std::string(); value_->assign(value); }
			bool isValueNull() const { return (value_ == 0); }
			void setValueNull() { if ( value_ ) delete value_; value_ = 0; }
			bool isValueWritable() const { return true; }


			virtual bool isRecordValid() const { return true; }


			// Methods from DBStatusI
			DBStatusStorage* clone() const { return new DBStatusStorage(*this); }
			const DBStatusI* getDecorated() const { return 0; }
			std::string getField(const std::string& field) const;
			void setField(const std::string& field, const std::string& value);


		private:

			// --- Data members ---
			std::string name_;
			std::string* value_;


			// Constructors are private so that only friend classes can execute them
			DBStatusStorage();
			DBStatusStorage(const std::string& name, std::string* value);
			// The copy constructor is required for iterator interface.
			DBStatusStorage(const DBStatusStorage& status);  


			// Do not allow the default assignment operator.
			DBStatusStorage operator=(const DBStatusStorage& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTATUSSTORAGE_H
