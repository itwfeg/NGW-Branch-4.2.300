/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DEVICE_H
#define DEVICE_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBDevice;
}

typedef enum DeviceFieldOffsets
{
	fieldOffsetDeviceType = 1,
	fieldOffsetDeviceSubType,
	fieldOffsetKind,
	fieldOffsetSerialNumber,
	fieldOffsetHardwareVersion,
	fieldOffsetSoftwareVersion,
	fieldOffsetCheckPresence,
	fieldOffsetInstalledDate,
	fieldOffsetServiceHistory,
	fieldOffsetUserName,
	fieldOffsetSystemName,
	fieldOffsetIPAddress,       
	endDeviceFieldOffsets
}   DeviceFieldOffsets_t;



class Device : public HfcObject
{
	public:

		Device();
		Device(const char *data, int len, int formatType);
		Device(edb::DBDevice *dbDevice, DBAccess *access = (DBAccess *)NULL);

		~Device();

		int getDeviceType() const { return deviceType; }
		void setDeviceType(int type) { deviceType = type; }

		int getDeviceSubType() const { return deviceSubType; }
		void setDeviceSubType(int subType) { deviceSubType = subType; }

		int getKind() const { return kind; }
		void setKind(int k) { kind = k; }

		HobString getSerialNumber() const { return serialNumber; }
		void setSerialNumber(const HobString &serialNum) { serialNumber = serialNum; }

		HobString getHardwareVersion() const { return hardwareVersion; }
		void setHardwareVersion(const HobString &version) { hardwareVersion = version; }

		HobString getSoftwareVersion() const { return softwareVersion; }
		void setSoftwareVersion(const HobString &version) { softwareVersion = version; }

		bool isCheckPresence() const { return checkPresence; }
		void setCheckPresence(bool check) { checkPresence = check; }

		HobDateTime getInstalledDate() const { return installedDate; }
		void setInstalledDate(HobDateTime hdt) { installedDate = hdt; }

		HobString getServiceHistory() const { return serviceHistory; }
		void setServiceHistory(const HobString &history) { serviceHistory = history; }

		HobString getUserName() const { return userName; }
		void setUserName(const HobString &name) { userName = name; }

		HobString getSystemName() const { return systemName; }
		void setSystemName(const HobString &name) { systemName = name; }

		HobString getIPAddress() const { return ipAddress; }
		void setIPAddress(const HobString &address) { ipAddress = address; }

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		void updateDevice(Device *newDevice);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBDevice *getMyDbRecord();

		bool isRecordValid();
		
		int deleteRecord();
		void saveRecord(); 

		static Device *findDevice(int type, int subType, DBAccess *dbAccess);

		static void initFieldList();


		bool isANonHobartLabelPrinter() const;
		bool isAHobartLabelPrinter() const;
		bool isANetworkPrinter() const;
		bool isANetworkReportPrinter() const;
		bool isARealPrinter() const;


	private:

		int         deviceType;
		int         deviceSubType;
		int         kind;
		HobString   serialNumber;
		HobString   hardwareVersion;
		HobString   softwareVersion;
		bool        checkPresence;
		HobDateTime installedDate;
		HobString   serviceHistory;
		HobString   userName;
		HobString   systemName;
		HobString   ipAddress;     

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBDevice *myDbRecord; 

		static std::list<DeviceFieldOffsets> fieldList, externalPrinterFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //DEVICE_H




