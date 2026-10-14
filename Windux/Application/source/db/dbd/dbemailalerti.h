#ifndef DBEMAILALERTI_H
#define DBEMAILALERTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBEmailAlertI
	{
		public:

			virtual ~DBEmailAlertI() {}


			virtual DBEmailAlertI* clone() const = 0;
			virtual const DBEmailAlertI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual bool getNotifyOnFailure() const = 0;
			virtual void setNotifyOnFailure(bool notify) = 0;
			virtual bool isNotifyOnFailureNull() const { return false; }
			virtual void setNotifyOnFailureNull() = 0;
			virtual bool isNotifyOnFailureWritable() const { return true; }

			virtual bool getNotifyOnResolution() const = 0;
			virtual void setNotifyOnResolution(bool notify) = 0;
			virtual bool isNotifyOnResolutionNull() const { return false; }
			virtual void setNotifyOnResolutionNull() = 0;
			virtual bool isNotifyOnResolutionWritable() const { return true; }

			virtual bool getNotifyHandsFreeDeviceOnFailure() const = 0;
			virtual void setNotifyHandsFreeDeviceOnFailure(bool notify) = 0;
			virtual bool isNotifyHandsFreeDeviceOnFailureNull() const { return false; }
			virtual void setNotifyHandsFreeDeviceOnFailureNull() = 0;
			virtual bool isNotifyHandsFreeDeviceOnFailureWritable() const { return true; }

			virtual bool getNotifyHandsFreeDeviceOnResolution() const = 0;
			virtual void setNotifyHandsFreeDeviceOnResolution(bool notify) = 0;
			virtual bool isNotifyHandsFreeDeviceOnResolutionNull() const { return false; }
			virtual void setNotifyHandsFreeDeviceOnResolutionNull() = 0;
			virtual bool isNotifyHandsFreeDeviceOnResolutionWritable() const { return true; }

			virtual bool getFailureSent() const = 0;
			virtual void setFailureSent(bool notify) = 0;
			virtual bool isFailureSentNull() const { return false; }
			virtual void setFailureSentNull() = 0;
			virtual bool isFailureSentWritable() const { return true; }

			virtual int getDelay() const = 0;
			virtual void setDelay(int delay) = 0;
			virtual bool isDelayNull() const { return false; }
			virtual void setDelayNull() = 0;
			virtual bool isDelayWritable() const { return true; }

			virtual std::string getSubject1() const = 0;
			virtual void setSubject1(const std::string& subject) = 0;
			virtual bool isSubject1Null() const { return false; }
			virtual void setSubject1Null() = 0;
			virtual bool isSubject1Writable() const { return true; }

			virtual std::string getSubject2() const = 0;
			virtual void setSubject2(const std::string& subject) = 0;
			virtual bool isSubject2Null() const { return false; }
			virtual void setSubject2Null() = 0;
			virtual bool isSubject2Writable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBEMAILALERTI_H
