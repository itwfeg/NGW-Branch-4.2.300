#ifndef DBRECALL_H
#define DBRECALL_H


#include <string>


#include "edb.h"
#include DBRECALLI_INC
#if defined( __linux__ )
    #include DBRECALLSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBRecall
	{
			// DBRecallsRM class is a friend of DBRecall because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBRecallsRM;
			friend class DBQuery<DBRecall>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBRecallI interface_type;
			typedef DBRecallsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrieved
			// record instead of looking it up too many times.
			DBRecall(const DBRecall& rec);   
			~DBRecall();


			// Methods from DBRecallI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			std::string getName() const { return decorated_->getName(); }
			void setName(const std::string& name) { decorated_->setName(name); }
			bool isNameNull() const { return decorated_->isNameNull(); }
			void setNameNull() { decorated_->setNameNull(); }
			bool isNameWritable() const { return decorated_->isNameWritable(); }

			std::string getText() const { return decorated_->getText(); }
			void setText(const std::string&  text) { decorated_->setText(text); }
			bool isTextNull() const { return decorated_->isTextNull(); }
			void setTextNull() { decorated_->setTextNull(); }
			bool isTextWritable() const { return decorated_->isTextWritable(); }
			
			std::string getReceivedDateTime() const { return decorated_->getReceivedDateTime(); }
			void setReceivedDateTime(const std::string& date) { decorated_->setReceivedDateTime(date); }
			bool isReceivedDateTimeNull() const { return decorated_->isReceivedDateTimeNull(); }
			void setReceivedDateTimeNull() { decorated_->setReceivedDateTimeNull(); }
			bool isReceivedDateTimeWritable() const { return decorated_->isReceivedDateTimeWritable(); }

			bool getAcknowledged() const { return decorated_->getAcknowledged(); }
			void setAcknowledged(bool ack) { decorated_->setAcknowledged(ack); }
			bool isAcknowledgedNull() const { return decorated_->isAcknowledgedNull(); }
			void setAcknowledgedNull() { decorated_->setAcknowledgedNull(); }
			bool isAcknowledgedWritable() const { return decorated_->isAcknowledgedWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer. 
			friend std::ostream& operator<<(std::ostream& os, const DBRecall& rec);


		private:

			edb_adapter::DBRecallI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBRecall(edb_adapter::DBRecallI* d);

			// Do not allow the default assignment operator.
			DBRecall operator=(const DBRecall& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBRECALL_H
