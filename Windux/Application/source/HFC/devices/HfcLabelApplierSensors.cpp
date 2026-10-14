/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierSensors.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"//Alex Zavala 8/26

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierSensors::dataType = 0;
std::string HfcLabelApplierSensors::dataTypeName("HfcLabelApplierSensors"); 
#endif

//HfcLabelApplierSensors::HfcLabelApplierSensors(const char *data) : LabelApplierSensors(data)
HfcLabelApplierSensors::HfcLabelApplierSensors(const char *data) : CompactLabelApplierSensors(data)
{
	Debug::trace("HfcLabelApplierSensors", "HfcLabelApplierSensors(const char *data)");
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcLabelApplierSensors::~HfcLabelApplierSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcLabelApplierSensors::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcLabelApplierSensorsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					/*
					case fieldOffsetLAWakeupSensor:
						HfcObject::serializedData.append(Integer::toString(getExpectPackage()));
						break;
					case fieldOffsetLAScaleSensor:
						HfcObject::serializedData.append(Integer::toString(getWeigherEntry()));
						break;
					case fieldOffsetLAPriApplierOrLeadEdgeSensor:
						HfcObject::serializedData.append(Integer::toString(getPr1Entry()));
						break;
					case fieldOffsetLAPriPrinterPositionSwitch:
						HfcObject::serializedData.append(Integer::toString(getPr1Valid()));
						break;
					case fieldOffsetLAPriPrinterOrTrailEdgeSensor:
						HfcObject::serializedData.append(Integer::toString(getPr1LabelPosition()));
						break;
					case fieldOffsetLAPriGateSwitch:
						HfcObject::serializedData.append(Integer::toString(getPr1Gate()));
						break;
					case fieldOffsetLASecApplierOrLeadEdgeSensor:
						HfcObject::serializedData.append(Integer::toString(getPr2Entry()));
						break;
					case fieldOffsetLASecPrinterPositionSwitch:
						HfcObject::serializedData.append(Integer::toString(getPr2Valid()));
						break;
					case fieldOffsetLASecPrinterOrTrailEdgeSensor:
						HfcObject::serializedData.append(Integer::toString(getPr2LabelPosition()));
						break;
					case fieldOffsetLASecGateSwitch:
						HfcObject::serializedData.append(Integer::toString(getPr2Gate()));
						break;
					*/
					case fieldOffsetLAWakeupSensor:
						HfcObject::serializedData.append(Bool::toString(getWakeUp()));
						Debug::trace("HfcLabelApplierSensors", HobString("WakeUp: ") + HobString(Bool::toString(getWakeUp())));
						break;
					case fieldOffsetLAWeigherEntrySensor:
						HfcObject::serializedData.append(Bool::toString(getWeigherEntry()));
						Debug::trace("HfcLabelApplierSensors", HobString("WeigherEntry: ") + HobString(Bool::toString(getWeigherEntry())));
						break;
					case fieldOffsetLACLAEntrySensor:
						HfcObject::serializedData.append(Bool::toString(getCLAEntry()));
						Debug::trace("HfcLabelApplierSensors", HobString("CLAEntry: ") + HobString(Bool::toString(getCLAEntry())));
						break;
					case fieldOffsetLACLAPkgEdgeDetectSensor:
						HfcObject::serializedData.append(Bool::toString(getCLAPkgEdgeDetect()));
						Debug::trace("HfcLabelApplierSensors", HobString("CLAPkgEdgeDetect: ") + HobString(Bool::toString(getCLAPkgEdgeDetect())));
						break;
					case fieldOffsetLAStartSwitch:
						HfcObject::serializedData.append(Bool::toString(getStartSwitch()));
						Debug::trace("HfcLabelApplierSensors", HobString("StartSwitch: ") + HobString(Bool::toString(getStartSwitch())));
						break;
					case fieldOffsetLAStopSwitch:
						HfcObject::serializedData.append(Bool::toString(getStopSwitch()));
						Debug::trace("HfcLabelApplierSensors", HobString("StopSwitch: ") + HobString(Bool::toString(getStopSwitch())));
						break;
					case fieldOffsetLACLAGateSwitch:
						HfcObject::serializedData.append(Bool::toString(getCLAGateSwitch()));
						Debug::trace("HfcLabelApplierSensors", HobString("WeigherEntry: ") + HobString(Bool::toString(getCLAGateSwitch())));
						break;
					case fieldOffsetLACLAPrinterPostSwitch:
						HfcObject::serializedData.append(Bool::toString(getCLAPrinterPostSwitch()));
						Debug::trace("HfcLabelApplierSensors", HobString("CLAPrinterPostSwitch: ") + HobString(Bool::toString(getCLAPrinterPostSwitch())));
						break;
					/*case fieldOffsetLAGenericSwitches:
						HfcObject::serializedData.append(Integer::toString(getGenericSwitches()));
						Debug::trace("HfcLabelApplierSensors", HobString("GenericSwitches: ") + HobString(Integer::toString(getGenericSwitches())));
						break;*/
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
