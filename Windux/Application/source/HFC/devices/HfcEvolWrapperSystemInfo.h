#ifndef HFCEVOLWRAPPERSYSTEMINFO_H
#define HFCEVOLWRAPPERSYSTEMINFO_H

#include "EvolWrapperSystemInfo.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcEvolWrapperSystemInfoOffsets
{
		fieldOffsetBoardChecksumStatus = 1,
		fieldOffsetAwsChecksumStatus,
		fieldOffsetMotorBoardPartNumber,
		fieldOffsetMotorBoardRev,
		fieldOffsetMotorBoardDateOfManufacture,
		fieldOffsetAwsSerialNumber,
		fieldOffsetAwsDateOfManufacture,
		fieldOffsetAwsDateOfInstall,
		fieldOffsetAwsModelNumber,
		fieldOffsetSystemWrapperCycleCount,
		fieldOffsetEvolWrapperSystemInfoEnd
} HfcEvolWrapperSystemInfoOffsets_t;


class HfcEvolWrapperSystemInfo : public EvolWrapperSystemInfo, public HfcObject
{
	public:

		HfcEvolWrapperSystemInfo(const char *data = NULL);

		~HfcEvolWrapperSystemInfo();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};





#endif
