#ifndef DBSTATUS_H
#define DBSTATUS_H


#include <string>


#include "edb.h"
#include DBSTATUSI_INC
#if defined( __linux__ )
    #include DBSTATUSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBStatus
	{
			// DBStatusRM class is a friend of DBStatus because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBStatusRM;
			friend class DBQuery<DBStatus>;

		public:

			// --- Types and subtypes ---
			typedef edb_adapter::DBStatusI interface_type;
			typedef DBStatusRM manager_type;


			// --- Constructors and destructor ---
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBStatus(const DBStatus& rec);  
			~DBStatus();


			// Methods associated with DBStatusI
			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			std::string getValue() const { return decorated_->getValue(); }
			void setValue(const std::string& value) { decorated_->setValue(value); }
			bool isValueNull() const { return decorated_->isValueNull(); }
			void setValueNull() { decorated_->setValueNull(); }
			bool isValueWritable() const { return decorated_->isNameWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getName(); }
			void setKey(const std::string& key) { decorated_->setName(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBStatus& rec);


		private:

			edb_adapter::DBStatusI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBStatus(edb_adapter::DBStatusI* d);
			DBStatus(const std::string& name, std::string* value);

			// Do not allow the default assignment operator.
			DBStatus operator=(const DBStatus& rhs);
	};

 
#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTATUS_H
