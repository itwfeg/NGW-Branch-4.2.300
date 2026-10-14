#ifndef HFCEVOLWRAPPEREVENTCAPTURE
#define HFCEVOLWRAPPEREVENTCAPTURE

#include "../../EvolutionRTC/WrapperManager/Include/EvolWrapperEventCapture.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcEvolWrapperEventCaptureOffsets
{
		fieldOffsetCaptureType = 1,
		fieldOffsetPackageId,
		fieldOffsetValue,
		fieldOffsetErrorNumber,
		fieldOffsetEventMonth,
		fieldOffsetEventDay,
		fieldOffsetEventYear,
		fieldOffsetEventHour,
		fieldOffsetEventMinutes,
		fieldOffsetEventSeconds,
		fieldOffsetEvolWrapperEventCaptureEnd
} HfcEvolWrapperKnifeTimeOffsets_t;


class HfcEvolWrapperEventCapture : public EvolWrapperEventCapture, public HfcObject
{
	public:

		HfcEvolWrapperEventCapture(const char *data);

		~HfcEvolWrapperEventCapture();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif
