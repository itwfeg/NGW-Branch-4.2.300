#ifndef HFCCLASYSTEMSTATUS_H
#define HFCCLASYSTEMSTATUS_H

#include "CLASystemStatus.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcCLASystemStatusOffsets
{
		/*	CLASysInfo	*/
		fieldOffsetCLAConfigurationChecksum = 1,
		fieldOffsetCLAAppFlash,
		fieldOffsetCLADataFlash,
		fieldOffsetCLABootFlash,
		///*	EEP93C56Board	*/
		//fieldOffsetCLAPartNumber,
		//fieldOffsetCLABoardRev,
		//fieldOffsetCLADateOfManufacture,
		//fieldOffsetCLASerialNumber,
		//fieldOffsetCLABoardSectionChecksum,
		///*	EEP93C56AWS	*/
		//fieldOffsetCLAAwsSerialNumber,
		//fieldOffsetCLAAwsDateOfManufacture,
		//fieldOffsetCLAAwsDateOfInstall,
		//fieldOffsetCLAAwsModelNumber,
		//fieldOffsetCLACycleCount,
		//fieldOffsetCLAAwsSectionChecksum,
		fieldOffsetCLASystemStatusEnd
} HfcCLASystemStatusOffsets_t;


class HfcCLASysStatus : public CLASystemStatus, public HfcObject
{
	public:

		HfcCLASysStatus(const char *data);

		~HfcCLASysStatus();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};





#endif
