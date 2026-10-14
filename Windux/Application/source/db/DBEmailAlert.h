#ifndef DBEMAILALERT_H
#define DBEMAILALERT_H


#include <string>


#include "edb.h"
#include DBEMAILALERTI_INC
#if defined( __linux__ )
    #include DBEMAILALERTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBEmailAlert
	{
			// DBEmailAlertsRM class is a friend of DBEmailAlert because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBEmailAlertsRM;
			friend class DBQuery<DBEmailAlert>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBEmailAlertI interface_type;
			typedef DBEmailAlertsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBEmailAlert(const DBEmailAlert& rec);
			~DBEmailAlert();


			// Methods associated with DBEmailAlertI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			bool getNotifyOnFailure() const { return decorated_->getNotifyOnFailure(); }
			void setNotifyOnFailure(bool notify) { decorated_->setNotifyOnFailure(notify); }
			bool isNotifyOnFailureNull() const { return decorated_->isNotifyOnFailureNull(); }
			void setNotifyOnFailureNull() { decorated_->setNotifyOnFailureNull(); }
			bool isNotifyOnFailureWritable() const { return decorated_->isNotifyOnFailureWritable(); }

			bool getNotifyOnResolution() const { return decorated_->getNotifyOnResolution(); }
			void setNotifyOnResolution(bool notify) { decorated_->setNotifyOnResolution(notify); }
			bool isNotifyOnResolutionNull() const { return decorated_->isNotifyOnResolutionNull(); }
			void setNotifyOnResolutionNull() { decorated_->setNotifyOnResolutionNull(); }
			bool isNotifyOnResolutionWritable() const { return decorated_->isNotifyOnResolutionWritable(); }

			bool getNotifyHandsFreeDeviceOnFailure() const { return decorated_->getNotifyHandsFreeDeviceOnFailure(); }
			void setNotifyHandsFreeDeviceOnFailure(bool notify) { decorated_->setNotifyHandsFreeDeviceOnFailure(notify); }
			bool isNotifyHandsFreeDeviceOnFailureNull() const { return decorated_->isNotifyHandsFreeDeviceOnFailureNull(); }
			void setNotifyHandsFreeDeviceOnFailureNull() { decorated_->setNotifyHandsFreeDeviceOnFailureNull(); }
			bool isNotifyHandsFreeDeviceOnFailureWritable() const { return decorated_->isNotifyHandsFreeDeviceOnFailureWritable(); }

			bool getNotifyHandsFreeDeviceOnResolution() const { return decorated_->getNotifyHandsFreeDeviceOnResolution(); }
			void setNotifyHandsFreeDeviceOnResolution(bool notify) { decorated_->setNotifyHandsFreeDeviceOnResolution(notify); }
			bool isNotifyHandsFreeDeviceOnResolutionNull() const { return decorated_->isNotifyHandsFreeDeviceOnResolutionNull(); }
			void setNotifyHandsFreeDeviceOnResolutionNull() { decorated_->setNotifyHandsFreeDeviceOnResolutionNull(); }
			bool isNotifyHandsFreeDeviceOnResolutionWritable() const { return decorated_->isNotifyHandsFreeDeviceOnResolutionWritable(); }

			int getDelay() const { return decorated_->getDelay(); }
			void setDelay(int delay) { decorated_->setDelay(delay); }
			bool isDelayNull() const { return decorated_->isDelayNull(); }
			void setDelayNull() { decorated_->setDelayNull(); }
			bool isDelayWritable() const { return decorated_->isDelayWritable(); }

			bool getFailureSent() const { return decorated_->getFailureSent(); }
			void setFailureSent(bool sent) { decorated_->setFailureSent(sent); }
			bool isFailureSentNull() const { return decorated_->isFailureSentNull(); }
			void setFailureSentNull() { decorated_->setFailureSentNull(); }
			bool isFailureSentWritable() const { return decorated_->isFailureSentWritable(); }

			std::string getSubject1() const { return decorated_->getSubject1(); }
			void setSubject1(const std::string& subject) { decorated_->setSubject1(subject); }
			bool isSubject1Null() const { return decorated_->isSubject1Null(); }
			void setSubject1Null() { decorated_->setSubject1Null(); }
			bool isSubject1Writable() const { return decorated_->isSubject1Writable(); }

			std::string getSubject2() const { return decorated_->getSubject2(); }
			void setSubject2(const std::string& subject) { decorated_->setSubject2(subject); }
			bool isSubject2Null() const { return decorated_->isSubject2Null(); }
			void setSubject2Null() { decorated_->setSubject2Null(); }
			bool isSubject2Writable() const { return decorated_->isSubject2Writable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& key) { decorated_->setNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBEmailAlert& rec);


		private:

			edb_adapter::DBEmailAlertI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBEmailAlert(edb_adapter::DBEmailAlertI* d);

			// Do not allow the default assignment operator.
			DBEmailAlert operator=(const DBEmailAlert& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBEMAILALERT_H
