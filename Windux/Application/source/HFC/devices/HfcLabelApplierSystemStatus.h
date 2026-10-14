/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERSYSTEMSTATUS_H
#define HFCLABELAPPLIERSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "LabelApplierSystemStatus.h"

typedef enum HfcLASysStatusFieldOffsets
{
	fieldOffsetHfcLASysStatusSystemChecksum = 1,
	fieldOffsetHfcLASysStatusRomChecksum,
	fieldOffsetHfcLASysStatusIsEepromError,
	fieldOffsetHfcLASysStatusIsBitTestError,
	//fieldOffsetHfcLASysStatusBitTestAddress,
	fieldOffsetHfcLASysStatusIsAddressError,
	//fieldOffsetHfcLASysStatusAddressTestAddress,

	fieldOffsetHfcLASysStatusSlave1SystemChecksum,
	fieldOffsetHfcLASysStatusSlave1RomChecksum,
	fieldOffsetHfcLASysStatusSlave1IsEepromError,
	fieldOffsetHfcLASysStatusSlave1IsBitTestError,
	//fieldOffsetHfcLASysStatusSlave1BitTestAddress,
	fieldOffsetHfcLASysStatusSlave1IsAddressError,
	//fieldOffsetHfcLASysStatusSlave1AddressTestAddress,

	fieldOffsetHfcLASysStatusSlave2SystemChecksum,
	fieldOffsetHfcLASysStatusSlave2RomChecksum,
	fieldOffsetHfcLASysStatusSlave2IsEepromError,
	fieldOffsetHfcLASysStatusSlave2IsBitTestError,
	//fieldOffsetHfcLASysStatusSlave2BitTestAddress,
	fieldOffsetHfcLASysStatusSlave2IsAddressError,
	//fieldOffsetHfcLASysStatusSlave2AddressTestAddress,

	endHfcLASysStatusFieldOffsets	//	MUST be last!
} HfcLASysStatusFieldOffsets_t;

class HfcLabelApplierSystemStatus : public LabelApplierSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcLabelApplierSystemStatus(const char *data);

		~HfcLabelApplierSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *sysStatusInfo, bool trueOrFalse);

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCLABELAPPLIERSYSTEMSTATUS_H
