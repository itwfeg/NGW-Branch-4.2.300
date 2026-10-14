/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERSENSORS_H
#define HFCLABELAPPLIERSENSORS_H

#include "evolution.h"
#include "HfcObject.h"
//#include "LabelApplierSensors.h"
#include "CompactLabelApplierSensors.h"

typedef enum HfcLabelApplierSensorsFieldOffsets
{
	/*
//	fieldOffsetLAWakeupSensor = 1,
//	fieldOffsetLAScaleSensor,
//	fieldOffsetLAPriApplierOrLeadEdgeSensor,
//	fieldOffsetLAPriPrinterPositionSwitch,
//	fieldOffsetLAPriPrinterOrTrailEdgeSensor,
//	fieldOffsetLAPriGateSwitch,
//	fieldOffsetLASecApplierOrLeadEdgeSensor,
//	fieldOffsetLASecPrinterPositionSwitch,
//	fieldOffsetLASecPrinterOrTrailEdgeSensor,
//	fieldOffsetLASecGateSwitch,
*/
	fieldOffsetLAWakeupSensor = 1,
	fieldOffsetLAWeigherEntrySensor,
	fieldOffsetLACLAEntrySensor,
	fieldOffsetLACLAPkgEdgeDetectSensor,
	fieldOffsetLAStartSwitch,
	fieldOffsetLAStopSwitch,
	fieldOffsetLACLAGateSwitch,
	fieldOffsetLACLAPrinterPostSwitch,
	//fieldOffsetLAGenericSwitches,
	endHfcLabelApplierSensorsFieldOffsets		//	MUST be last!
} HfcLabelApplierSensorsFieldOffsets_t;

//class HfcLabelApplierSensors : public LabelApplierSensors, public HfcObject
class HfcLabelApplierSensors : public CompactLabelApplierSensors, public HfcObject
{
	public:
		HfcLabelApplierSensors(const char *data);
		~HfcLabelApplierSensors();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCLABELAPPLIERSENSORS_H
