#ifndef SQLITEDEVICE_H
#define SQLITEDEVICE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBDEVICEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDevice : public DBDeviceI
	{
		public:

			// Types and subtypes
			typedef DBDeviceI interface_type;

			
			// Constructors and destructor
			SqliteDevice(SqliteRecord* r, DBDeviceI* d = 0);
			~SqliteDevice();


			// Think about replacing this method with two friend functions from the
			// SqliteDevicesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBDeviceI
			virtual DBDeviceI* clone() const { return new SqliteDevice(new SqliteRecord(*rec_)); }
			virtual const DBDeviceI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(const std::string& type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getSubtype() const { return rec_->getField(SUBTYPE_FIELD); }
			virtual void setSubtype(const std::string& type) { rec_->setField(SUBTYPE_FIELD, type); }
			virtual bool isSubtypeNull() const { return rec_->isFieldNull(SUBTYPE_FIELD); }
			virtual bool isSubtypeWritable() const { return true; }

			virtual int getKind() const { return rec_->getField(KIND_FIELD); }
			virtual void setKind(int kind) { rec_->setField(KIND_FIELD, kind); }
			virtual bool isKindNull() const { return rec_->isFieldNull(KIND_FIELD); }
			virtual void setKindNull() { rec_->setFieldNull(KIND_FIELD); }
			virtual bool isKindWritable() const { return true; }

			virtual std::string getSerialNumber() const { return rec_->getField(SERIALNUMBER_FIELD); }
			virtual void setSerialNumber(const std::string& sn) { rec_->setField(SERIALNUMBER_FIELD, sn); }
			virtual bool isSerialNumberNull() const { return rec_->isFieldNull(SERIALNUMBER_FIELD); }
			virtual void setSerialNumberNull() { rec_->setFieldNull(SERIALNUMBER_FIELD); }
			virtual bool isSerialNumberWritable() const { return true; }

			virtual std::string getHardwareVersion() const { return rec_->getField(HARDWAREVERSION_FIELD); }
			virtual void setHardwareVersion(const std::string& sn) { rec_->setField(HARDWAREVERSION_FIELD, sn); }
			virtual bool isHardwareVersionNull() const { return rec_->isFieldNull(HARDWAREVERSION_FIELD); }
			virtual void setHardwareVersionNull() { rec_->setFieldNull(HARDWAREVERSION_FIELD); }
			virtual bool isHardwareVersionWritable() const { return true; }

			virtual std::string getSoftwareVersion() const { return rec_->getField(SOFTWAREVERSION_FIELD); }
			virtual void setSoftwareVersion(const std::string& sn) { rec_->setField(SOFTWAREVERSION_FIELD, sn); }
			virtual bool isSoftwareVersionNull() const { return rec_->isFieldNull(SOFTWAREVERSION_FIELD); }
			virtual void setSoftwareVersionNull() { rec_->setFieldNull(SOFTWAREVERSION_FIELD); }
			virtual bool isSoftwareVersionWritable() const { return true; }

			virtual bool getCheckPresence() const { return util_string::stringtobool(rec_->getField(CHECKPRESENCE_FIELD)); }
			virtual void setCheckPresence(bool check) { rec_->setField(CHECKPRESENCE_FIELD, Bool::toString(check)); }
			virtual bool isCheckPresenceNull() const { return rec_->isFieldNull(CHECKPRESENCE_FIELD); }
			virtual void setCheckPresenceNull() { rec_->setFieldNull(CHECKPRESENCE_FIELD); }
			virtual bool isCheckPresenceWritable() const { return true; }

			virtual std::string getInstallationDate() const { return rec_->getField(INSTALLATIONDATE_FIELD); }
			virtual void setInstallationDate(const std::string& id) { rec_->setField(INSTALLATIONDATE_FIELD, id); }
			virtual bool isInstallationDateNull() const { return rec_->isFieldNull(INSTALLATIONDATE_FIELD); }
			virtual void setInstallationDateNull() { rec_->setFieldNull(INSTALLATIONDATE_FIELD); }
			virtual bool isInstallationDateWritable() const { return true; }

			virtual std::string getServiceHistory() const { return rec_->getField(SERVICEHISTORY_FIELD); }
			virtual void setServiceHistory(const std::string& sh) { rec_->setField(SERVICEHISTORY_FIELD, sh); }
			virtual bool isServiceHistoryNull() const { return rec_->isFieldNull(SERVICEHISTORY_FIELD); }
			virtual void setServiceHistoryNull() { rec_->setFieldNull(SERVICEHISTORY_FIELD); }
			virtual bool isServiceHistoryWritable() const { return true; }

			virtual std::string getUserName() const { return rec_->getField(USERNAME_FIELD); }
			virtual void setUserName(const std::string& userName) { rec_->setField(USERNAME_FIELD, userName); }
			virtual bool isUserNameNull() const { return rec_->isFieldNull(USERNAME_FIELD); }
			virtual void setUserNameNull() { rec_->setFieldNull(USERNAME_FIELD); }
			virtual bool isUserNameWritable() const { return true; }

			virtual std::string getSystemName() const { return rec_->getField(SYSTEMNAME_FIELD); }
			virtual void setSystemName(const std::string& systemName) { rec_->setField(SYSTEMNAME_FIELD, systemName); }
			virtual bool isSystemNameNull() const { return rec_->isFieldNull(SYSTEMNAME_FIELD); }
			virtual void setSystemNameNull() { rec_->setFieldNull(SYSTEMNAME_FIELD); }
			virtual bool isSystemNameWritable() const { return true; }

			virtual std::string getIPAddress() const { return rec_->getField(IPADDRESS_FIELD); }
			virtual void setIPAddress(const std::string& ipAddress) { rec_->setField(IPADDRESS_FIELD, ipAddress); }
			virtual bool isIPAddressNull() const { return rec_->isFieldNull(IPADDRESS_FIELD); }
			virtual void setIPAddressNull() { rec_->setFieldNull(IPADDRESS_FIELD); }
			virtual bool isIPAddressWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string TYPE_FIELD;
			static const std::string SUBTYPE_FIELD;
			static const std::string KIND_FIELD;
			static const std::string SERIALNUMBER_FIELD;
			static const std::string HARDWAREVERSION_FIELD;
			static const std::string SOFTWAREVERSION_FIELD;
			static const std::string CHECKPRESENCE_FIELD;
			static const std::string INSTALLATIONDATE_FIELD;
			static const std::string SERVICEHISTORY_FIELD;
			static const std::string USERNAME_FIELD;
			static const std::string SYSTEMNAME_FIELD;
			static const std::string IPADDRESS_FIELD;


		private:

			SqliteRecord* rec_;
			DBDeviceI* decorated_;

			// Do not allow the default copy constructor.
			SqliteDevice(const SqliteDevice& rec);
			// Do not allow the default assignment operator.
			SqliteDevice operator=(const SqliteDevice& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEDEVICE_H
