/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCCLASYSINFOWRITE_H
#define HFCCLASYSINFOWRITE_H

#include "evolution.h"
#include "CLASystemInfoWrite.h"
#include "HfcObject.h"
												
typedef enum HfcLabelApplierSysInfoWriteFieldOffsets
{
	fieldOffsetLASerialNumber = 1,
	fieldOffsetDateOfManufacture,
	fieldOffsetDateOfInstall,
	fieldOffsetModelNumber,
	fieldOffsetCycleCount,
	fieldOffsetSectionChecksum,
	endHfcCLASysInfoWriteFieldOffsets	//	MUST be last!
} HfcLabelApplierSysInfoWriteFieldOffsets_t;

class HfcCLASysInfoWrite : public CLASystemInfoWrite, public HfcObject
{
	public:

		HfcCLASysInfoWrite(const char *data, int len, int formatType);

		~HfcCLASysInfoWrite();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

		void processDate(HobString & date);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCCLASYSINFOWRITE_H
