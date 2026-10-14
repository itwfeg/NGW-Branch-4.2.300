#ifndef HFCEVOLWRAPPERCAMERATHRESHOLD_H
#define HFCEVOLWRAPPERCAMERATHRESHOLD_H

#include "EvolWrapperCameraThreshold.h"
#include "HfcObject.h"

typedef enum HfcEvolWrapperCameraThresholdFieldOffsets
{
	fieldOffsetHfcEvolCameraThresholdCount = 1,
	endHfcEvolCameraThresholdCountFieldOffsets	//	MUST be last!
} HfcEvolWrapperCameraThresholdFieldOffsets_t;

class HfcEvolWrapperCameraThreshold : public EvolWrapperCameraThreshold, public HfcObject
{
	public:
		HfcEvolWrapperCameraThreshold(const char *data);
		HfcEvolWrapperCameraThreshold(const char *data, int len, int formatType);
		
		~HfcEvolWrapperCameraThreshold();

		void show();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};




#endif
