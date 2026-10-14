#ifndef DBCLASS_H
#define DBCLASS_H


#include <string>


#include "edb.h"
#include DBCLASSI_INC
#if defined( __linux__ )
    #include DBCLASSESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBClass
	{
			// DBClassesRM class is a friend of DBClass because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBClassesRM;
			friend class DBQuery<DBClass>;
			friend class DBProductsRM;     // Needs access to decorated_ for cross-referencing.
			friend class DBNutritionsRM;   // Needs access to decorated_ for cross-referencing.
			friend class DBClassTotalsRM;  // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBClassI interface_type;
			typedef DBClassesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBClass(const DBClass& rec);  
			~DBClass();


			// Methods associated with DBClassI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getReportingClass() const { return decorated_->getReportingClass(); }
			void setReportingClass(const std::string& rc) { decorated_->setReportingClass(rc); }
			bool isReportingClassNull() const { return decorated_->isReportingClassNull(); }
			void setReportingClassNull() { decorated_->setReportingClassNull(); }
			bool isReportingClassWritable() const { return decorated_->isReportingClassWritable(); }

			std::string getDescription() const { return decorated_->getDescription(); }
			void setDescription(const std::string& desc) { decorated_->setDescription(desc); }
			bool isDescriptionNull() const { return decorated_->isDescriptionNull(); }
			void setDescriptionNull() { decorated_->setDescriptionNull(); }
			bool isDescriptionWritable() const { return decorated_->isDescriptionWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBClass& rec);


		private:

			edb_adapter::DBClassI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBClass(edb_adapter::DBClassI* d);

			// Do not allow the default assignment operator.
			DBClass operator=(const DBClass& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCLASS_H
