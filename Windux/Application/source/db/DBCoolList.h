#ifndef DBCOOLLIST_H
#define DBCOOLLIST_H


#include <string>


#include "edb.h"
#include DBCOOLLISTI_INC
#if defined( __linux__ )
    #include DBCOOLLISTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBCoolListText;
	template <class DBEntity> class DBQuery;


	class DBCoolList
	{
			// DBCoolListsRM class is a friend of DBCoolList because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolListsRM;
			friend class DBQuery<DBCoolList>;
			friend class DBProduct;             // Needs access to decorated_ for cross-referencing
			friend class DBProductsRM;          // Needs access to decorated_ for cross-referencing
			friend class DBCoolListTextsRM;     // Needs access to decorated_

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolListI interface_type;
			typedef DBCoolListsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolList(const DBCoolList& rec);  
			~DBCoolList();


			// Methods associated with DBCoolListI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			void setNumberNull() { decorated_->setNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getFirstLinkNumber() const { return decorated_->getFirstLinkNumber(); }
			void setFirstLinkNumber(const std::string& first) { decorated_->setFirstLinkNumber(first); }
			bool isFirstLinkNumberNull() const { return decorated_->isFirstLinkNumberNull(); }
			void setFirstLinkNumberNull() { decorated_->setFirstLinkNumberNull(); }
			bool isFirstLinkNumberWritable() const { return decorated_->isFirstLinkNumberWritable(); }

			bool containsCoolListText(const DBCoolListText* text) const;


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolList& rec);


		private:

			edb_adapter::DBCoolListI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolList(edb_adapter::DBCoolListI* d);

			// Do not allow the default assignment operator.
			DBCoolList operator=(const DBCoolList& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLLIST_H
