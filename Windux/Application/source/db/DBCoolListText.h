#ifndef DBCOOLLISTTEXT_H
#define DBCOOLLISTTEXT_H


#include <string>


#include "edb.h"
#include DBCOOLLISTTEXTI_INC
#if defined( __linux__ )
    #include DBCOOLLISTTEXTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCoolListText
	{
			// DBCoolListTextsRM class is a friend of DBCoolListText because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCoolListTextsRM;
			friend class DBQuery<DBCoolListText>;
			friend class DBCoolListsRM;
			friend class DBCoolList;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCoolListTextI interface_type;
			typedef DBCoolListTextsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCoolListText(const DBCoolListText& rec);
			~DBCoolListText();


			// Methods associated with DBCoolListTextI
			std::string getCoolTextNumber() const { return decorated_->getCoolTextNumber(); }
			void setCoolTextNumber(const std::string& text) { decorated_->setCoolTextNumber(text); }
			bool isCoolTextNumberNull() const { return decorated_->isCoolTextNumberNull(); }
			bool isCoolTextNumberWritable() const { return decorated_->isCoolTextNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			int getOrder() const { return decorated_->getOrder(); }
			void setOrder(int order) { decorated_->setOrder(order); }
			bool isOrderNull() const { return decorated_->isOrderNull(); }
			bool isOrderWritable() const { return decorated_->isOrderWritable(); }

			std::string getLinkNumber() const { return decorated_->getLinkNumber(); }
			void setLinkNumber(const std::string& number) { decorated_->setLinkNumber(number); }
			bool isLinkNumberNull() const { return decorated_->isLinkNumberNull(); }
			void setLinkNumberNull() { decorated_->setLinkNumberNull(); }
			bool isLinkNumberWritable() const { return decorated_->isLinkNumberWritable(); }

			std::string getNextLinkNumber() const { return decorated_->getNextLinkNumber(); }
			void setNextLinkNumber(const std::string& next) { decorated_->setNextLinkNumber(next); }
			bool isNextLinkNumberNull() const { return decorated_->isNextLinkNumberNull(); }
			void setNextLinkNumberNull() { decorated_->setNextLinkNumberNull(); }
			bool isNextLinkNumberWritable() const { return decorated_->isNextLinkNumberWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getCoolTextNumber(); }
			void setKey1(const std::string& type) { decorated_->setCoolTextNumber(type); }
			std::string getKey2() const { return decorated_->getCoolTextNumber(); }
			void setKey2(const std::string& key) { decorated_->setCoolTextNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCoolListText& rec);


		private:

			edb_adapter::DBCoolListTextI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCoolListText(edb_adapter::DBCoolListTextI* d);

			// Do not allow the default assignment operator.
			DBCoolListText operator=(const DBCoolListText& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLLISTTEXT_H
