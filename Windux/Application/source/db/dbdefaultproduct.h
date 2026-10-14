#ifndef DBDEFAULTPRODUCT_H
#define DBDEFAULTPRODUCT_H



#include "edb.h"

#include "dbdefaultproducti.h"
#include "dbdefaultproductsrmi.h"

#include <string>


#ifndef NO_NAMESPACES
namespace edb
{
#endif

	template <class DBEntity> class DBQuery;

	class DBDefaultProduct
	{
			// DBDefaultProductsRM class is a friend of DBDefaultProduct because it needs
			// access to its "decorated" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBDefaultProductsRM;
			friend class DBQuery<DBDefaultProduct>;

		public:
			// Types and subtypes
			typedef edb_adapter::DBDefaultProductI interface_type;
			typedef DBDefaultProductsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBDefaultProduct(const DBDefaultProduct& p);
			~DBDefaultProduct();


			// Methods associated with DBDefaultProductI
			std::string getPresentationMethodType() const { return decorated->getPresentationMethodType(); }
			void setPresentationMethodType(const std::string& type) { decorated->setPresentationMethodType(type); }
			bool isPresentationMethodTypeNull() const { return decorated->isPresentationMethodTypeNull(); }
			bool isPresentationMethodTypeWritable() const { return decorated->isPresentationMethodTypeWritable(); }

			std::string getLanguageType() const { return decorated->getLanguageType(); }
			void setLanguageType(const std::string& lang) { decorated->setLanguageType(lang); }
			bool isLanguageTypeNull() const { return decorated->isLanguageTypeNull(); }
			bool isLanguageTypeWritable() const { return decorated->isLanguageTypeWritable(); }


			bool isRecordValid() const { return decorated->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated->getPresentationMethodType(); }
			void setKey1(const std::string& type) { decorated->setPresentationMethodType(type); }
			std::string getKey2() const { return decorated->getProductKey(); }
			void setKey2(const std::string& key) { decorated->setProductKey(key); }
			std::string getKey3() const { return decorated->getLanguageType(); }
			void setKey3(const std::string& type) { decorated->setLanguageType(type); }

			std::string getField(const std::string& field) const { return decorated->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBDefaultProduct& rec);


		private:
			edb_adapter::DBDefaultProductI* decorated;

			// Constructors are private so that only friend classes can execute them
			DBDefaultProduct(edb_adapter::DBDefaultProductI* d);

			// Do not allow the default assignment operator.
			DBDefaultProduct operator=(const DBDefaultProduct& rhs);

			std::string getProductKey() const { return decorated->getProductKey(); }
			void setProductKey(const std::string& plu) { decorated->setProductKey(plu); }
			bool isProductKeyNull() const { return decorated->isProductKeyNull(); }
			bool isProductKeyWritable() const { return decorated->isProductKeyWritable(); }
	};

#ifndef NO_NAMESPACES
} // namespace edb
#endif



#endif // DBDEFAULTPRODUCT_H

