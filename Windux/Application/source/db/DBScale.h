#ifndef DBSCALE_H
#define DBSCALE_H


#include <string>


#include "edb.h"
#include DBSCALEI_INC
#if defined( __linux__ )
    #include DBSCALESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBScale
	{
			// DBScalesRM class is a friend of DBScale because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBScalesRM;
			friend class DBQuery<DBScale>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBScaleI interface_type;
			typedef DBScalesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBScale(const DBScale& rec);  
			~DBScale();


			// Methods associated with DBScaleI
			std::string getIdentifier() const { return decorated_->getIdentifier(); }
			bool isIdentifierNull() const { return decorated_->isIdentifierNull(); }
			bool isIdentifierWritable() const { return decorated_->isIdentifierWritable(); }

			int getType() const { return decorated_->getType(); }
			void setType(int type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getHostName() const { return decorated_->getHostName(); }
			void setHostName(const std::string& hostname) { decorated_->setHostName(hostname); }
			bool isHostNameNull() const { return decorated_->isHostNameNull(); }
			void setHostNameNull() { decorated_->setHostNameNull(); }
			bool isHostNameWritable() const { return decorated_->isHostNameWritable(); }

			std::string getIPAddress() const { return decorated_->getIPAddress(); }
			void setIPAddress(const std::string& ipAddress) { decorated_->setIPAddress(ipAddress); }
			bool isIPAddressNull() const { return decorated_->isIPAddressNull(); }
			void setIPAddressNull() { decorated_->setIPAddressNull(); }
			bool isIPAddressWritable() const { return decorated_->isIPAddressWritable(); }

			std::string getCreationDateTime() const { return decorated_->getCreationDateTime(); }
			void setCreationDateTime(const std::string& creation) { decorated_->setCreationDateTime(creation); }
			bool isCreationDateTimeNull() const { return decorated_->isCreationDateTimeNull(); }
			void setCreationDateTimeNull() { decorated_->setCreationDateTimeNull(); }
			bool isCreationDateTimeWritable() const { return decorated_->isCreationDateTimeWritable(); }

			int getState() const { return decorated_->getState(); }
			void setState(int state) { decorated_->setState(state); }
			bool isStateNull() const { return decorated_->isStateNull(); }
			void setStateNull() { decorated_->setStateNull(); }
			bool isStateWritable() const { return decorated_->isStateWritable(); }

			std::string getStateChangedDateTime() const { return decorated_->getStateChangedDateTime(); }
			void setStateChangedDateTime(const std::string& dt) { decorated_->setStateChangedDateTime(dt); }
			bool isStateChangedDateTimeNull() const { return decorated_->isStateChangedDateTimeNull(); }
			void setStateChangedDateTimeNull() { decorated_->setStateChangedDateTimeNull(); }
			bool isStateChangedDateTimeWritable() const { return decorated_->isStateChangedDateTimeWritable(); }

			bool isRegistering() const { return decorated_->isRegistering(); }
			void setRegistering(bool registering) { decorated_->setRegistering(registering); }
			bool isRegisteringNull() const { return decorated_->isRegisteringNull(); }
			void setRegisteringNull() { decorated_->setRegisteringNull(); }
			bool isRegisteringWritable() const { return decorated_->isRegisteringWritable(); }

			bool isDeregistering() const { return decorated_->isDeregistering(); }
			void setDeregistering(bool deregistering) { decorated_->setDeregistering(deregistering); }
			bool isDeregisteringNull() const { return decorated_->isDeregisteringNull(); }
			void setDeregisteringNull() { decorated_->setDeregisteringNull(); }
			bool isDeregisteringWritable() const { return decorated_->isDeregisteringWritable(); }

			bool isNotifiedBatches() const { return decorated_->isNotifiedBatches(); }
			void setNotifiedBatches(bool notifyBatches) { decorated_->setNotifiedBatches(notifyBatches); }
			bool isNotifiedBatchesNull() const { return decorated_->isNotifiedBatchesNull(); }
			void setNotifiedBatchesNull() { decorated_->setNotifiedBatchesNull(); }
			bool isNotifiedBatchesWritable() const { return decorated_->isNotifiedBatchesWritable(); }

			std::string getNotifiedBatchesDateTime() const { return decorated_->getNotifiedBatchesDateTime(); }
			void setNotifiedBatchesDateTime(const std::string& creation) { decorated_->setNotifiedBatchesDateTime(creation); }
			bool isNotifiedBatchesDateTimeNull() const { return decorated_->isNotifiedBatchesDateTimeNull(); }
			void setNotifiedBatchesDateTimeNull() { decorated_->setNotifiedBatchesDateTimeNull(); }
			bool isNotifiedBatchesDateTimeWritable() const { return decorated_->isNotifiedBatchesDateTimeWritable(); }

			bool isNotifiedOnline() const { return decorated_->isNotifiedOnline(); }
			void setNotifiedOnline(bool notifyOnline) { decorated_->setNotifiedOnline(notifyOnline); }
			bool isNotifiedOnlineNull() const { return decorated_->isNotifiedOnlineNull(); }
			void setNotifiedOnlineNull() { decorated_->setNotifiedOnlineNull(); }
			bool isNotifiedOnlineWritable() const { return decorated_->isNotifiedOnlineWritable(); }

			std::string getNotifiedOnlineDateTime() const { return decorated_->getNotifiedOnlineDateTime(); }
			void setNotifiedOnlineDateTime(const std::string& creation) { decorated_->setNotifiedOnlineDateTime(creation); }
			bool isNotifiedOnlineDateTimeNull() const { return decorated_->isNotifiedOnlineDateTimeNull(); }
			void setNotifiedOnlineDateTimeNull() { decorated_->setNotifiedOnlineDateTimeNull(); }
			bool isNotifiedOnlineDateTimeWritable() const { return decorated_->isNotifiedOnlineDateTimeWritable(); }

			int getRetryCount() const { return decorated_->getRetryCount(); }
			void setRetryCount(int retryCount) { decorated_->setRetryCount(retryCount); }
			bool isRetryCountNull() const { return decorated_->isRetryCountNull(); }
			void setRetryCountNull() { decorated_->setRetryCountNull(); }
			bool isRetryCountWritable() const { return decorated_->isRetryCountWritable(); }

			int getVersion() const { return decorated_->getVersion(); }
			void setVersion(int version) { decorated_->setVersion(version); }
			bool isVersionNull() const { return decorated_->isVersionNull(); }
			void setVersionNull() { decorated_->setVersionNull(); }
			bool isVersionWritable() const { return decorated_->isVersionWritable(); }

			int getVersionReceived() const { return decorated_->getVersionReceived(); }
			void setVersionReceived(int version) { decorated_->setVersionReceived(version); }
			bool isVersionReceivedNull() const { return decorated_->isVersionReceivedNull(); }
			void setVersionReceivedNull() { decorated_->setVersionReceivedNull(); }
			bool isVersionReceivedWritable() const { return decorated_->isVersionReceivedWritable(); }

			std::string getVersionReceivedDateTime() const { return decorated_->getVersionReceivedDateTime(); }
			void setVersionReceivedDateTime(const std::string& versionReceived) { decorated_->setVersionReceivedDateTime(versionReceived); }
			bool isVersionReceivedDateTimeNull() const { return decorated_->isVersionReceivedDateTimeNull(); }
			void setVersionReceivedDateTimeNull() { decorated_->setVersionReceivedDateTimeNull(); }
			bool isVersionReceivedDateTimeWritable() const { return decorated_->isVersionReceivedDateTimeWritable(); }

			virtual std::string getLastSuccessfulCommDateTime() const { return decorated_->getLastSuccessfulCommDateTime(); }
			virtual void setLastSuccessfulCommDateTime(const std::string& dt) { decorated_->setLastSuccessfulCommDateTime(dt); }
			virtual bool isLastSuccessfulCommDateTimeNull() const { return decorated_->isLastSuccessfulCommDateTimeNull(); }
			virtual void setLastSuccessfulCommDateTimeNull() { decorated_->setLastSuccessfulCommDateTimeNull(); }
			virtual bool isLastSuccessfulCommDateTimeWritable() const { return decorated_->isLastSuccessfulCommDateTimeWritable(); }

			virtual std::string getFirstFailedCommDateTime() const { return decorated_->getFirstFailedCommDateTime(); }
			virtual void setFirstFailedCommDateTime(const std::string& dt) { decorated_->setFirstFailedCommDateTime(dt); }
			virtual bool isFirstFailedCommDateTimeNull() const { return decorated_->isFirstFailedCommDateTimeNull(); }
			virtual void setFirstFailedCommDateTimeNull() { decorated_->setFirstFailedCommDateTimeNull(); }
			virtual bool isFirstFailedCommDateTimeWritable() const { return decorated_->isFirstFailedCommDateTimeWritable(); }

			virtual int getTotalSuccessfulCommunications() const { return decorated_->getTotalSuccessfulCommunications(); }
			virtual void setTotalSuccessfulCommunications(int total) { decorated_->setTotalSuccessfulCommunications(total); }
			virtual bool isTotalSuccessfulCommunicationsNull() const { return decorated_->isTotalSuccessfulCommunicationsNull(); }
			virtual void setTotalSuccessfulCommunicationsNull() { decorated_->setTotalSuccessfulCommunicationsNull(); }
			virtual bool isTotalSuccessfulCommunicationsWritable() const { return decorated_->isTotalSuccessfulCommunicationsWritable(); }

			virtual int getTotalFailedCommunications() const { return decorated_->getTotalFailedCommunications(); }
			virtual void setTotalFailedCommunications(int total) { decorated_->setTotalFailedCommunications(total); }
			virtual bool isTotalFailedCommunicationsNull() const { return decorated_->isTotalFailedCommunicationsNull(); }
			virtual void setTotalFailedCommunicationsNull() { decorated_->setTotalFailedCommunicationsNull(); }
			virtual bool isTotalFailedCommunicationsWritable() const { return decorated_->isTotalFailedCommunicationsWritable(); }

			virtual int getTotalBusyCommunications() const { return decorated_->getTotalBusyCommunications(); }
			virtual void setTotalBusyCommunications(int total) { decorated_->setTotalBusyCommunications(total); }
			virtual bool isTotalBusyCommunicationsNull() const { return decorated_->isTotalBusyCommunicationsNull(); }
			virtual void setTotalBusyCommunicationsNull() { decorated_->setTotalBusyCommunicationsNull(); }
			virtual bool isTotalBusyCommunicationsWritable() const { return decorated_->isTotalBusyCommunicationsWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getIdentifier(); }
			void setKey(const std::string& /*key*/) { /*decorated_->setIdentifier(key);*/ }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBScale& rec);


		private:

			edb_adapter::DBScaleI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBScale(edb_adapter::DBScaleI* d);

			// Do not allow the default assignment operator.
			DBScale operator=(const DBScale& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSCALE_H
