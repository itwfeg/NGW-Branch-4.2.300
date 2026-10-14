#ifndef DBDEVICE_H
#define DBDEVICE_H


#include <string>


#include "edb.h"
#include DBDEVICEI_INC
#if defined( __linux__ )
    #include DBDEVICESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBDevice
	{
			// DBDevicesRM class is a friend of DBDevice because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBDevicesRM;
			friend class DBQuery<DBDevice>;
			friend class DBSoftKeysRM;              // Needs access to decorated_.

		public:

			// Types and subtypes
			typedef edb_adapter::DBDeviceI interface_type;
			typedef DBDevicesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBDevice(const DBDevice& rec);
			~DBDevice();


			// Methods associated with DBDeviceI
			std::string getType() const { return decorated_->getType(); }
			void setType(const std::string& type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getSubtype() const { return decorated_->getSubtype(); }
			void setSubtype(const std::string& subtype) { decorated_->setSubtype(subtype); }
			bool isSubtypeNull() const { return decorated_->isSubtypeNull(); }
			bool isSubtypeWritable() const { return decorated_->isSubtypeWritable(); }

			int getKind() const { return decorated_->getKind(); }
			void setKind(int kind) { decorated_->setKind(kind); }
			bool isKindNull() const { return decorated_->isKindNull(); }
			void setKindNull() { decorated_->setKindNull(); }
			bool isKindWritable() const { return decorated_->isKindWritable(); }

			std::string getSerialNumber() const { return decorated_->getSerialNumber(); }
			void setSerialNumber(const std::string& sn) { decorated_->setSerialNumber(sn); }
			bool isSerialNumberNull() const { return decorated_->isSerialNumberNull(); }
			void setSerialNumberNull() { decorated_->setSerialNumberNull(); }
			bool isSerialNumberWritable() const { return decorated_->isSerialNumberWritable(); }

			std::string getHardwareVersion() const { return decorated_->getHardwareVersion(); }
			void setHardwareVersion(const std::string& version) { decorated_->setHardwareVersion(version); }
			bool isHardwareVersionNull() const { return decorated_->isHardwareVersionNull(); }
			void setHardwareVersionNull() { decorated_->setHardwareVersionNull(); }
			bool isHardwareVersionWritable() const { return decorated_->isHardwareVersionWritable(); }

			std::string getSoftwareVersion() const { return decorated_->getSoftwareVersion(); }
			void setSoftwareVersion(const std::string& version) { decorated_->setSoftwareVersion(version); }
			bool isSoftwareVersionNull() const { return decorated_->isSoftwareVersionNull(); }
			void setSoftwareVersionNull() { decorated_->setSoftwareVersionNull(); }
			bool isSoftwareVersionWritable() const { return decorated_->isSoftwareVersionWritable(); }

			bool getCheckPresence() const { return decorated_->getCheckPresence(); }
			void setCheckPresence(bool check) { decorated_->setCheckPresence(check); }
			bool isCheckPresenceNull() const { return decorated_->isCheckPresenceNull(); }
			void setCheckPresenceNull() { decorated_->setCheckPresenceNull(); }
			bool isCheckPresenceWritable() const { return decorated_->isCheckPresenceWritable(); }

			std::string getInstallationDate() const { return decorated_->getInstallationDate(); }
			void setInstallationDate(const std::string& date) { decorated_->setInstallationDate(date); }
			bool isInstallationDateNull() const { return decorated_->isInstallationDateNull(); }
			void setInstallationDateNull() { return decorated_->setInstallationDateNull(); }
			bool isInstallationDateWritable() const { return decorated_->isInstallationDateWritable(); }

			std::string getServiceHistory() const { return decorated_->getServiceHistory(); }
			void setServiceHistory(const std::string& history) { decorated_->setServiceHistory(history); }
			bool isServiceHistoryNull() const { return decorated_->isServiceHistoryNull(); }
			void setServiceHistoryNull() { decorated_->setServiceHistoryNull(); }
			bool isServiceHistoryWritable() const { return decorated_->isServiceHistoryWritable(); }

			std::string getUserName() const { return decorated_->getUserName(); }
			void setUserName(const std::string& name) { decorated_->setUserName(name); }
			bool isUserNameNull() const { return decorated_->isUserNameNull(); }
			void setUserNameNull() { decorated_->setUserNameNull(); }
			bool isUserNameWritable() const { return decorated_->isUserNameWritable(); }

			std::string getSystemName() const { return decorated_->getSystemName(); }
			void setSystemName(const std::string& systemName) { decorated_->setSystemName(systemName); }
			bool isSystemNameNull() const { return decorated_->isSystemNameNull(); }
			void setSystemNameNull() { decorated_->setSystemNameNull(); }
			bool isSystemNameWritable() const { return decorated_->isSystemNameWritable(); }

			std::string getIPAddress() const { return decorated_->getIPAddress(); }
			void setIPAddress(const std::string& ipAddress) { decorated_->setIPAddress(ipAddress); }
			bool isIPAddressNull() const { return decorated_->isIPAddressNull(); }
			void setIPAddressNull() { decorated_->setIPAddressNull(); }
			bool isIPAddressWritable() const { return decorated_->isIPAddressWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getType(); }
			void setKey1(const std::string& key) { decorated_->setType(key); }
			std::string getKey2() const { return decorated_->getSubtype(); }
			void setKey2(const std::string& key) { decorated_->setSubtype(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBDevice& rec);


		private:

			edb_adapter::DBDeviceI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBDevice(edb_adapter::DBDeviceI* d);

			// Do not allow the default assignment operator.
			DBDevice operator=(const DBDevice& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBDEVICE_H
