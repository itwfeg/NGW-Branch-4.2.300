#ifndef DBDEVICEI_H
#define DBDEVICEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBDeviceI
	{
		public:

			virtual ~DBDeviceI() {}


			virtual DBDeviceI* clone() const = 0;
			virtual const DBDeviceI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getType() const = 0;
			virtual void setType(const std::string& type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getSubtype() const = 0;
			virtual void setSubtype(const std::string& subtype) = 0;
			virtual bool isSubtypeNull() const { return false; }
			virtual bool isSubtypeWritable() const { return true; }

			virtual int getKind() const = 0;
			virtual void setKind(int kind) = 0;
			virtual bool isKindNull() const { return false; }
			virtual void setKindNull() = 0;
			virtual bool isKindWritable() const { return true; }

			virtual std::string getSerialNumber() const = 0;
			virtual void setSerialNumber(const std::string& sn) = 0;
			virtual bool isSerialNumberNull() const { return false; }
			virtual void setSerialNumberNull() = 0;
			virtual bool isSerialNumberWritable() const { return true; }

			virtual std::string getHardwareVersion() const = 0;
			virtual void setHardwareVersion(const std::string& version) = 0;
			virtual bool isHardwareVersionNull() const { return false; }
			virtual void setHardwareVersionNull() = 0;
			virtual bool isHardwareVersionWritable() const { return true; }

			virtual std::string getSoftwareVersion() const = 0;
			virtual void setSoftwareVersion(const std::string& version) = 0;
			virtual bool isSoftwareVersionNull() const { return false; }
			virtual void setSoftwareVersionNull() = 0;
			virtual bool isSoftwareVersionWritable() const { return true; }

			virtual bool getCheckPresence() const = 0;
			virtual void setCheckPresence(bool check) = 0;
			virtual bool isCheckPresenceNull() const { return false; }
			virtual void setCheckPresenceNull() = 0;
			virtual bool isCheckPresenceWritable() const { return true; }

			virtual std::string getInstallationDate() const = 0;
			virtual void setInstallationDate(const std::string& date) = 0;
			virtual bool isInstallationDateNull() const { return false; }
			virtual void setInstallationDateNull() = 0;
			virtual bool isInstallationDateWritable() const { return true; }

			virtual std::string getServiceHistory() const = 0;
			virtual void setServiceHistory(const std::string& history) = 0;
			virtual bool isServiceHistoryNull() const { return false; }
			virtual void setServiceHistoryNull() = 0;
			virtual bool isServiceHistoryWritable() const { return true; }

			virtual std::string getUserName() const = 0;
			virtual void setUserName(const std::string& userName) = 0;
			virtual bool isUserNameNull() const { return false; }
			virtual void setUserNameNull() = 0;
			virtual bool isUserNameWritable() { return true; }

			virtual std::string getSystemName() const = 0;
			virtual void setSystemName(const std::string& systemName) = 0;
			virtual bool isSystemNameNull() const { return false; }
			virtual void setSystemNameNull() = 0;
			virtual bool isSystemNameWritable() const { return true; }

			virtual std::string getIPAddress() const = 0;
			virtual void setIPAddress(const std::string& ipAddress) = 0;
			virtual bool isIPAddressNull() const { return false; }
			virtual void setIPAddressNull() = 0;
			virtual bool isIPAddressWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBDEVICEI_H
 