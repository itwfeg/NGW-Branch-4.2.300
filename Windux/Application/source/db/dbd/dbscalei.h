#ifndef DBSCALEI_H
#define DBSCALEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBScaleI
	{
		public:

			virtual ~DBScaleI() {}


			virtual DBScaleI* clone() const = 0;
			virtual const DBScaleI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getIdentifier() const = 0;
			virtual bool isIdentifierNull() const { return false; }
			virtual bool isIdentifierWritable() const { return true; }

			virtual int getType() const = 0;
			virtual void setType(int type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getHostName() const = 0;
			virtual void setHostName(const std::string& hostname) = 0;
			virtual bool isHostNameNull() const { return false; }
			virtual void setHostNameNull() = 0;
			virtual bool isHostNameWritable() const { return true; }

			virtual std::string getIPAddress() const = 0;
			virtual void setIPAddress(const std::string& ipAddress) = 0;
			virtual bool isIPAddressNull() const { return false; }
			virtual void setIPAddressNull() = 0;
			virtual bool isIPAddressWritable() const { return true; }

			virtual std::string getCreationDateTime() const = 0;
			virtual void setCreationDateTime(const std::string& creation) = 0;
			virtual bool isCreationDateTimeNull() const { return false; }
			virtual void setCreationDateTimeNull() = 0;
			virtual bool isCreationDateTimeWritable() const { return true; }

			virtual int getState() const = 0;
			virtual void setState(int state) = 0;
			virtual bool isStateNull() const { return false; }
			virtual void setStateNull() = 0;
			virtual bool isStateWritable() const { return true; }

			virtual std::string getStateChangedDateTime() const = 0;
			virtual void setStateChangedDateTime(const std::string& dt) = 0;
			virtual bool isStateChangedDateTimeNull() const { return false; }
			virtual void setStateChangedDateTimeNull() = 0;
			virtual bool isStateChangedDateTimeWritable() const { return true; }

			virtual bool isRegistering() const = 0;
			virtual void setRegistering(bool registering) = 0;
			virtual bool isRegisteringNull() const { return false; }
			virtual void setRegisteringNull() = 0;
			virtual bool isRegisteringWritable() const { return true; }

			virtual bool isDeregistering() const = 0;
			virtual void setDeregistering(bool deregistering) = 0;
			virtual bool isDeregisteringNull() const { return false; }
			virtual void setDeregisteringNull() = 0;
			virtual bool isDeregisteringWritable() const { return true; }

			virtual bool isNotifiedBatches() const = 0;
			virtual void setNotifiedBatches(bool notifyBatches) = 0;
			virtual bool isNotifiedBatchesNull() const { return false; }
			virtual void setNotifiedBatchesNull() = 0;
			virtual bool isNotifiedBatchesWritable() const { return true; }

			virtual std::string getNotifiedBatchesDateTime() const = 0;
			virtual void setNotifiedBatchesDateTime(const std::string& dt) = 0;
			virtual bool isNotifiedBatchesDateTimeNull() const { return false; }
			virtual void setNotifiedBatchesDateTimeNull() = 0;
			virtual bool isNotifiedBatchesDateTimeWritable() const { return true; }

			virtual bool isNotifiedOnline() const = 0;
			virtual void setNotifiedOnline(bool notifyOnline) = 0;
			virtual bool isNotifiedOnlineNull() const { return false; }
			virtual void setNotifiedOnlineNull() = 0;
			virtual bool isNotifiedOnlineWritable() const { return true; }

			virtual std::string getNotifiedOnlineDateTime() const = 0;
			virtual void setNotifiedOnlineDateTime(const std::string& dt) = 0;
			virtual bool isNotifiedOnlineDateTimeNull() const { return false; }
			virtual void setNotifiedOnlineDateTimeNull() = 0;
			virtual bool isNotifiedOnlineDateTimeWritable() const { return true; }

			virtual int getRetryCount() const = 0;
			virtual void setRetryCount(int retryCount) = 0;
			virtual bool isRetryCountNull() const { return false; }
			virtual void setRetryCountNull() = 0;
			virtual bool isRetryCountWritable() const { return true; }

			virtual int getVersion() const = 0;
			virtual void setVersion(int version) = 0;
			virtual bool isVersionNull() const { return false; }
			virtual void setVersionNull() = 0;
			virtual bool isVersionWritable() const { return true; }

			virtual int getVersionReceived() const = 0;
			virtual void setVersionReceived(int version) = 0;
			virtual bool isVersionReceivedNull() const { return false; }
			virtual void setVersionReceivedNull() = 0;
			virtual bool isVersionReceivedWritable() const { return true; }

			virtual std::string getVersionReceivedDateTime() const = 0;
			virtual void setVersionReceivedDateTime(const std::string& versionReceived) = 0;
			virtual bool isVersionReceivedDateTimeNull() const { return false; }
			virtual void setVersionReceivedDateTimeNull() = 0;
			virtual bool isVersionReceivedDateTimeWritable() const { return true; }

			virtual std::string getLastSuccessfulCommDateTime() const = 0;
			virtual void setLastSuccessfulCommDateTime(const std::string& dt) = 0;
			virtual bool isLastSuccessfulCommDateTimeNull() const { return false; }
			virtual void setLastSuccessfulCommDateTimeNull() = 0;
			virtual bool isLastSuccessfulCommDateTimeWritable() const { return true; }

			virtual std::string getFirstFailedCommDateTime() const = 0;
			virtual void setFirstFailedCommDateTime(const std::string& dt) = 0;
			virtual bool isFirstFailedCommDateTimeNull() const { return false; }
			virtual void setFirstFailedCommDateTimeNull() = 0;
			virtual bool isFirstFailedCommDateTimeWritable() const { return true; }

			virtual int getTotalSuccessfulCommunications() const = 0;
			virtual void setTotalSuccessfulCommunications(int total) = 0;
			virtual bool isTotalSuccessfulCommunicationsNull() const { return false; }
			virtual void setTotalSuccessfulCommunicationsNull() = 0;
			virtual bool isTotalSuccessfulCommunicationsWritable() const { return true; }

			virtual int getTotalFailedCommunications() const = 0;
			virtual void setTotalFailedCommunications(int total) = 0;
			virtual bool isTotalFailedCommunicationsNull() const { return false; }
			virtual void setTotalFailedCommunicationsNull() = 0;
			virtual bool isTotalFailedCommunicationsWritable() const { return true; }

			virtual int getTotalBusyCommunications() const = 0;
			virtual void setTotalBusyCommunications(int total) = 0;
			virtual bool isTotalBusyCommunicationsNull() const { return false; }
			virtual void setTotalBusyCommunicationsNull() = 0;
			virtual bool isTotalBusyCommunicationsWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSCALEI_H
