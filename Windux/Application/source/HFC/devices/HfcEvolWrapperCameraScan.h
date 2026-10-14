/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERCAMERASCAN_H
#define HFCEVOLWRAPPERCAMERASCAN_H

#include "evolution.h"
#include "HfcObject.h"
#include "../../EvolutionRTC/WrapperManager/Include/EvolWrapperCameraScan.h"

typedef enum HfcEvolWrapperCameraScanFieldOffsets
{
	fieldOffsetEvolWrCameraScanCameraNum = 1,
	fieldOffsetEvolWrCameraScanImageType,
	fieldOffsetEvolWrCameraScanImage,
	endHfcEvolWrCameraScanFieldOffsets	//	MUST be last!
} HfcEvolWrapperCameraScanFieldOffsets_t;

class HfcEvolWrapperCameraScan : public EvolWrapperCameraScan, public HfcObject
{
	public:
		HfcEvolWrapperCameraScan(const char *data);

		~HfcEvolWrapperCameraScan();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERCAMERASCAN_H
