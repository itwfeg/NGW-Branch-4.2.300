#ifndef HFCEVOLWRAPPERSYSTEMINFOWRITE_H
#define HFCEVOLWRAPPERSYSTEMINFOWRITE_H

#include "EvolWrapperSystemInfoWrite.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcEvolWrapperSystemInfoWriteOffsets
{
		
	fieldOffsetWriteAwsSerialNumber = 1,
	fieldOffsetWriteAwsDateOfManufacture,
	fieldOffsetWriteAwsDateOfInstall,
	fieldOffsetWriteAwsModelNumber,
	fieldOffsetWriteSystemWrapperCycleCount,
	fieldOffsetEvolWrapperSystemInfoWriteEnd
} HfcEvolWrapperSystemInfoWriteOffsets_t;


class HfcEvolWrapperSystemInfoWrite : public EvolWrapperSystemInfoWrite, public HfcObject
{
	public:

		HfcEvolWrapperSystemInfoWrite(const char *data, int len, int formatType);

		~HfcEvolWrapperSystemInfoWrite();

		virtual const char *serialize(int format = BrowserFormat)const;

		void processDate(HobString &theDate);

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif	//	HFCEVOLWRAPPERSYSTEMINFOWRITE_H
