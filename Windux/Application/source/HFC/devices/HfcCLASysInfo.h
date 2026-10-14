#ifndef HFCCLASYSTEMINFO_H
#define HFCCLASYSTEMINFO_H

#include "CLASystemInfo.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcCLASystemInfoOffsets
{
		/*	EEP93C56Board	*/
		fieldOffsetCLAPartNumber = 1,
		fieldOffsetCLABoardRev,
		fieldOffsetCLADateOfManufacture,
		fieldOffsetCLASerialNumber,
		fieldOffsetCLABoardSectionChecksum,
		/*	EEP93C56AWS	*/
		fieldOffsetCLAAwsSerialNumber,
		fieldOffsetCLAAwsDateOfManufacture,
		fieldOffsetCLAAwsDateOfInstall,
		fieldOffsetCLAAwsModelNumber,
		fieldOffsetCLACycleCount,
		fieldOffsetCLAAwsSectionChecksum,
		fieldOffsetCLASystemInfoEnd
} HfcCLASystemInfoOffsets_t;


class HfcCLASysInfo : public CLASystemInfo, public HfcObject
{
	public:

		HfcCLASysInfo(const char *data);

		~HfcCLASysInfo();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:
		

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};





#endif
