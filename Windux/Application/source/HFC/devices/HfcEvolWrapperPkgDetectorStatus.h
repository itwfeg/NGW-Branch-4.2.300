#ifndef HFCEVOLWRAPPERPKGDETECTORSTATUS_H
#define HFCEVOLWRAPPERPKGDETECTORSTATUS_H

#include "EvolWrapperPkgDetectorStatus.h"
#include "HfcObject.h"

typedef enum HfcEvolWrapperPkgDetectorStatusFieldOffsets
{
	fieldOffsetEvolSensorLocation = 1,
	fieldOffsetEvolDetectorType,
	fieldOffsetEvolEmitterPowerLevel,
	fieldOffsetEvolSensorOperational,
	fieldOffsetEvolIsOldDetector,
	fieldOffsetEvolDetectorDescription,
	endHfcEvolPkgDetectorStatusFieldOffsets	//	MUST be last!
} HfcEvolWrapperPkgDetectorStatusFieldOffsets_t;

class HfcEvolWrapperPkgDetectorStatus : public EvolWrapperPkgDetectorStatus, public HfcObject
{
	public:
		HfcEvolWrapperPkgDetectorStatus(const char *data);

		~HfcEvolWrapperPkgDetectorStatus();

		void show();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
