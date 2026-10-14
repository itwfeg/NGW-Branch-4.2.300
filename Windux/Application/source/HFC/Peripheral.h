/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "evolution.h"
#include "Event.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>

class DBAccess;
namespace edb
{
class DBDevice;
}


// The intention is that these will match up with EvolDeviceType defined in event.h
typedef enum PeripheralType
{
	Console = 80,
	HandWrapStation,
	HandLabelApplier,
	WirelessCard
}   PeripheralType_t;


typedef enum PeripheralFieldOffsets
{
	fieldOffsetPeripheralType = 1,
	fieldOffsetPeripheralSubType,
	fieldOffsetPeripheralKind,
	fieldOffsetPeripheralSerialNumber,
	fieldOffsetPeripheralInstallationDate,
	fieldOffsetPeripheralServiceHistory,
	fieldOffsetPeripheralHardwareVersion,
	fieldOffsetPeripheralSoftwareVersion,
	fieldOffsetPeripheralCheckPresence,
	endPeripheralFieldOffsets
}   PeripheralFieldOffsets_t;


class Peripheral : public HfcObject
{
	public:
		Peripheral();
		Peripheral(const char *data, int len, int formatType);
		Peripheral(edb::DBDevice *dbDevice, DBAccess *access = (DBAccess *)NULL);

		~Peripheral();


		EvolDeviceType getType() const;
		void setType(EvolDeviceType dev);

		short getSubType() const;
		void setSubType(short sub);

		short getKind() const;
		void setKind(short peripheralKind);

		const HobString &getSerialNumber() const;
		void setSerialNumber(const HobString &serNum);

		const HobDateTime &getInstallationDate() const;
		void setInstallationDate(const HobDateTime &date);

		const HobString &getServiceHistory() const;
		void setServiceHistory(const HobString &history);

		const HobString &getHardwareVersion() const;
		void setHardwareVersion(const HobString &ver);

		const HobString &getSoftwareVersion() const;
		void setSoftwareVersion(const HobString &ver);

		bool isCheckingPresence() const;
		void setCheckingPresence(bool checking);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		bool isFieldQuoted(int fieldOffset) const;

		int updateField(int fieldCode, const HobString &token);

		void updatePeripheral(const Peripheral *newPeripheral);

		virtual const char *serialize(int format) const;

		void getFieldAsString(int fieldCode, HobString &result, int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBDevice *getMyDbRecord() const;

		bool isRecordValid();
		
		int deleteRecord();
		void saveRecord(); 

		// overrides from HfcObject class
		virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;

		static Peripheral *findPeripheral(EvolDeviceType peripheralType, short peripheralSubType, DBAccess *dbAccess);

		static void initFieldList();


	private:

		EvolDeviceType type;
		short subType;
		short kind;
		HobString serialNumber;
		HobDateTime installationDate;
		HobString serviceHistory;
		HobString hardwareVersion;
		HobString softwareVersion;
		bool checkingPresence;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBDevice *myDbRecord; 

		static std::list<PeripheralFieldOffsets> evolFieldList, browserFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PERIPHERAL_H




