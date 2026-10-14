#ifndef DBPRESENTATIONMETHOD_H
#define DBPRESENTATIONMETHOD_H


#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODI_INC
#if defined( __linux__ )
    #include DBPRESENTATIONMETHODSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBPresentationMethod
	{
			// DBPresentationMethodsRM class is a friend of DBPresentationMethod because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBPresentationMethodsRM;
			friend class DBQuery<DBPresentationMethod>;
			friend class DBCategoryStatisticsRM;
			friend class DBPresentationMethodStatisticsRM;
			friend class DBProductDetailedStatisticsRM;
			friend class DBCategoryDetailedStatisticsRM;
			friend class DBPresentationMethodDetailedStatisticsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBPresentationMethodI interface_type;
			typedef DBPresentationMethodsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBPresentationMethod(const DBPresentationMethod& rec);
			~DBPresentationMethod();


			// Methods associated with DBPresentationMethodI
			std::string getType() const { return decorated_->getType(); }
			void setType(const std::string& type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getDescription() const { return decorated_->getDescription(); }
			void setDescription(const std::string& desc) { decorated_->setDescription(desc); }
			bool isDescriptionNull() const { return decorated_->isDescriptionNull(); }
			void setDescriptionNull() { decorated_->setDescriptionNull(); }
			bool isDescriptionWritable() const { return decorated_->isDescriptionWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getType(); }
			void setKey(const std::string& type) { decorated_->setType(type); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBPresentationMethod& rec);


		private:

			edb_adapter::DBPresentationMethodI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBPresentationMethod(edb_adapter::DBPresentationMethodI* d);

			// Do not allow the default assignment operator.
			DBPresentationMethod operator=(const DBPresentationMethod& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHOD_H
