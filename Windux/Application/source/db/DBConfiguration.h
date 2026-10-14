#ifndef DBCONFIGURATION_H
#define DBCONFIGURATION_H


#include <string>


#include "edb.h"
#include DBCONFIGURATIONI_INC
#if defined( __linux__ )
    #include DBCONFIGURATIONSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBConfiguration
	{
			// DBConfigurationsRM class is a friend of DBConfiguration because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBConfigurationsRM;
			friend class DBQuery<DBConfiguration>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBConfigurationI interface_type;
			typedef DBConfigurationsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBConfiguration(const DBConfiguration& rec);
			~DBConfiguration();


			// Methods associated with DBConfigurationI
			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			std::string getValue() const { return decorated_->getValue(); }
			void setValue(const std::string& value) { decorated_->setValue(value); }
			bool isValueNull() const { return decorated_->isValueNull(); }
			void setValueNull() { decorated_->setValueNull(); }
			bool isValueWritable() const { return decorated_->isValueWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getName(); }
			void setKey(const std::string& key) { decorated_->setName(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBConfiguration& rec);


		private:

			edb_adapter::DBConfigurationI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBConfiguration(edb_adapter::DBConfigurationI* d);

			// Do not allow the default assignment operator.
			DBConfiguration operator=(const DBConfiguration& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCONFIGURATION_H
