/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERSENSORSTRACKING_H
#define HFCLABELAPPLIERSENSORSTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "LabelApplierSensorsTracking.h"

typedef enum HfcLaSensorTrackingFieldOffsets
{
	fieldOffsetLaSensorsTracking = 1,
	endHfcLaLaSensorTrackingFieldOffsets	//	MUST be last!
} HfcLaSensorTrackingFieldOffsets_t;

class HfcLabelApplierSensorsTracking : public LabelApplierSensorsTracking, public HfcObject
{
	public:
		HfcLabelApplierSensorsTracking(const char *data, int len, int formatType);
		~HfcLabelApplierSensorsTracking();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCLABELAPPLIERSENSORSTRACKING_H
