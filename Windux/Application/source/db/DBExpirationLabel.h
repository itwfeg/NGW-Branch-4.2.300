#ifndef DBEXPIRATIONLABEL_H
#define DBEXPIRATIONLABEL_H


#include <string>


#include "edb.h"
#include DBEXPIRATIONLABELI_INC
#if defined( __linux__ )
    #include DBEXPIRATIONLABELSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



    class DBCoolList;
    template <class DBEntity> class DBQuery;


	class DBExpirationLabel
	{
			// DBExpirationLabelsRM class is a friend of DBExpirationLabel because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBExpirationLabelsRM;
			friend class DBQuery<DBExpirationLabel>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBExpirationLabelI interface_type;
			typedef DBExpirationLabelsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBExpirationLabel(const DBExpirationLabel& rec);
			~DBExpirationLabel();


			// Methods associated with DBExpirationLabelI
			std::string getSequenceNumber() const { return decorated_->getSequenceNumber(); }
			bool isSequenceNumberNull() const { return decorated_->isSequenceNumberNull(); }
			bool isSequenceNumberWritable() const { return decorated_->isSequenceNumberWritable(); }

			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getPLU() const { return decorated_->getPLU(); }
			void setPLU(const std::string& plu) { decorated_->setPLU(plu); }
			bool isPLUNull() const { return decorated_->isPLUNull(); }
			void setPLUNull() { decorated_->setPLUNull(); }
			bool isPLUWritable() const { return decorated_->isPLUWritable(); }

			std::string getProductDescription() const { return decorated_->getProductDescription(); }
			void setProductDescription(const std::string& desc) { decorated_->setProductDescription(desc); }
			bool isProductDescriptionNull() const { return decorated_->isProductDescriptionNull(); }
			void setProductDescriptionNull() { decorated_->setProductDescriptionNull(); }
			bool isProductDescriptionWritable() const { return decorated_->isProductDescriptionWritable(); }

			std::string getExpiration() const { return decorated_->getExpiration(); }
			void setExpiration(const std::string& exp) { decorated_->setExpiration(exp); }
			bool isExpirationNull() const { return decorated_->isExpirationNull(); }
			void setExpirationNull() { return decorated_->setExpirationNull(); }
			bool isExpirationWritable() const { return decorated_->isExpirationWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getPLU(); }
			void setKey(const std::string& key) { decorated_->setPLU(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBExpirationLabel& rec);


		private:

			edb_adapter::DBExpirationLabelI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBExpirationLabel(edb_adapter::DBExpirationLabelI* d);

			// Do not allow the default assignment operator.
			DBExpirationLabel operator=(const DBExpirationLabel& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBEXPIRATIONLABEL_H
