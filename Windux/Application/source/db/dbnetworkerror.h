#ifndef DBNETWORKERROR_H
#define DBNETWORKERROR_H


#include <string>


#include "edb.h"
#include DBNETWORKERRORI_INC
#if defined( __linux__ )
    #include DBNETWORKERRORSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBNetworkError
	{
			// DBNetworkErrorsRM class is a friend of DBNetworkError because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBNetworkErrorsRM;
			friend class DBQuery<DBNetworkError>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBNetworkErrorI interface_type;
			typedef DBNetworkErrorsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBNetworkError(const DBNetworkError& rec);
			~DBNetworkError();


			// Methods from DBNetworkErrorI
			std::string getErrorCode() const { return decorated_->getErrorCode(); }
			void setErrorCode(const std::string& code) { decorated_->setErrorCode(code); }
			bool isErrorCodeNull() const { return decorated_->isErrorCodeNull(); }
			void setErrorCodeNull() { decorated_->isErrorCodeNull(); }
			bool isErrorCodeWritable() const { return decorated_->isErrorCodeWritable(); }

			int getRecordType() const { return decorated_->getRecordType(); }
			void setRecordType(int type) { decorated_->setRecordType(type); }
			bool isRecordTypeNull() const { return decorated_->isRecordTypeNull(); }
			void setRecordTypeNull() { decorated_->setRecordTypeNull(); }
			bool isRecordTypeWritable() const { return decorated_->isRecordTypeWritable(); }

			std::string getDateTime() const { return decorated_->getDateTime(); }
			void setDateTime(const std::string& date) { decorated_->setDateTime(date); }
			bool isDateTimeNull() const { return decorated_->isDateTimeNull(); }
			void setDateTimeNull() { decorated_->isDateTimeNull(); }
			bool isDateTimeWritable() const { return decorated_->isDateTimeWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getDateTime(); }
			void setKey(const std::string& key) { decorated_->setDateTime(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBNetworkError& rec);


		private:

			edb_adapter::DBNetworkErrorI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBNetworkError(edb_adapter::DBNetworkErrorI* d);

			// Do not allow the default assignment operator.
			DBNetworkError operator=(const DBNetworkError& rhs);


			std::string getNetworkErrorKey() const { return decorated_->getNetworkErrorKey(); }
			void setNetworkErrorKey(const std::string& errorKey) { decorated_->setNetworkErrorKey(errorKey); }
			bool isNetworkErrorKeyNull() const { return decorated_->isNetworkErrorKeyNull(); }
			bool isNetworkErrorKeyWritable() const { return decorated_->isNetworkErrorKeyWritable(); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBNETWORKERROR_H
