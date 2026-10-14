/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperSensors.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSensors::_dataType_ = 0;
std::string HfcEvolWrapperSensors::_dataTypeName_("HfcEvolWrapperSensors"); 
#endif

HfcEvolWrapperSensors::HfcEvolWrapperSensors(const char *data) : EvolWrapperSensors(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperSensors", "Constructor()");
}

HfcEvolWrapperSensors::~HfcEvolWrapperSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperSensors::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperSensors", "serialize()");

	const EvolWrapperGeneralSensors genSen( getGeneralSensors() );
	const EvolWrapperInterlockSensors interSen( getInterlockSensors() );
	const EvolWrapperGeneralSensors_2 genSen_2( getGeneralSensors_2() );

	HfcObject::serializedData = "";

	switch (format)
	{
	case BrowserFormat:
	{
		HfcObject::serializedData = Integer::toString((int)getDeviceType());
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Integer::toString((int)getEventType()));

		for ( size_t loopCnt( 1 ); loopCnt < endHfcEvolWrSensorsFieldOffsets; loopCnt++ )
		{
			HfcObject::serializedData.append(BrowserFormatSeparator);

			switch (loopCnt)
			{
			case fieldOffsetEvolWrSensorPrimaryHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isPrimaryHome()));
				break;
			case fieldOffsetEvolWrSensorShiftHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isShiftHome()));
				break;
			case fieldOffsetEvolWrSensorShiftLeftLimit:
				HfcObject::serializedData.append(Bool::toString(genSen.isShiftLeftLimit()));
				break;
			case fieldOffsetEvolWrSensorGripperHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isGripperHome()));
				break;
			case fieldOffsetEvolWrSensorSideFolderHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isSideFolderHome()));
				break;
			case fieldOffsetEvolWrSensorPusherHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isPusherHome()));
				break;
			case fieldOffsetEvolWrSensorFrontFolderHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isFrontFolderHome()));
				break;
			case fieldOffsetEvolWrSensorRearFolderHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isRearFolderHome()));
				break;
			case fieldOffsetEvolWrSensorElevatorHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isElevatorHome()));
				break;
			case fieldOffsetEvolWrSensorLabelRotationHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isLabelRotationHome()));
				break;
			case fieldOffsetEvolWrSensorElevatorPkg:
				HfcObject::serializedData.append(Bool::toString(genSen.isElevatorPkg()));
				break;
			case fieldOffsetEvolWrSensorTransferPkg:
				HfcObject::serializedData.append(Bool::toString(genSen.isTransferPkg()));
				break;
			case fieldOffsetEvolWrSensorStart:
				HfcObject::serializedData.append(Bool::toString(genSen.isStart()));
				break;
			case fieldOffsetEvolWrSensorStop:
				HfcObject::serializedData.append(Bool::toString(genSen.isStop()));
				break;
			case fieldOffsetEvolWrSensorSideClampRight:
				HfcObject::serializedData.append(Bool::toString(genSen.getSideClampRight()));
				break;
			case fieldOffsetEvolWrSensorSideClampHome:
				HfcObject::serializedData.append(Bool::toString(genSen.isSideClampHome()));
				break;
			case fieldOffsetEvolWrSensorFilmRoll:
				HfcObject::serializedData.append(Integer::toString(genSen.getFilmRoll()));
				break;
			case fieldOffsetEvolWrSensorKnifeUp:
				HfcObject::serializedData.append(Integer::toString(genSen.getKnifeUp()));
				break;

			case fieldOffsetEvolWrSensorInterlockStop:
				HfcObject::serializedData.append(Bool::toString(interSen.isStopSwitch()));
				break;
			case fieldOffsetEvolWrSensorInterlockRightCanopy:
				HfcObject::serializedData.append(Bool::toString(interSen.isRightCanopySwitch()));
				break;
			case fieldOffsetEvolWrSensorInterlockRightSideCover:
				HfcObject::serializedData.append(Bool::toString(interSen.isRightSideCover()));
				break;
			case fieldOffsetEvolWrSensorInterlockFilmSupplyDoor:
				HfcObject::serializedData.append(Bool::toString(interSen.isFilmSupplyDoor()));
				break;
			case fieldOffsetEvolWrSensorInterlockLeftSideCover:
				HfcObject::serializedData.append(Bool::toString(interSen.isLeftSideCover()));
				break;
			case fieldOffsetEvolWrSensorInterlockIntakeCleanout:
				HfcObject::serializedData.append(Bool::toString(interSen.isIntakeCleanout()));
				break;
			/* SOF-3838  no longer used repurpose for second label applier
			case fieldOffsetEvolWrSensorSwitched24V:
				HfcObject::serializedData.append(Integer::toString(genSen.getSwitched24V()));
				break;
			case fieldOffsetEvolWrSensorHighVoltageInrush:
				HfcObject::serializedData.append(Integer::toString(genSen.getHighVoltageInrush()));
				break;
			*/
			case fieldOffsetEvolWrSensorSecWandUp:
				HfcObject::serializedData.append(Integer::toString(genSen.getSecWandUp()));
				break;
			case fieldOffsetEvolWrSensorSecWandDown:
				HfcObject::serializedData.append(Integer::toString(genSen.getSecWandDown()));
				break;

			case fieldOffsetEvolWrSensorFilmSelectorLever:
				HfcObject::serializedData.append(Integer::toString(genSen.getFilmSelectorLever()));
				break;
			/* SOF-3838  no longer used repurpose for second label applier	
			case fieldOffsetEvolWrSensorPrinterPlate:
				HfcObject::serializedData.append(Integer::toString(genSen.getPrinterPlate()));
				break;
			*/
			case fieldOffsetEvolWrSensorSecApplierHand:
				HfcObject::serializedData.append(Integer::toString(genSen.getSecApplierHand()));
				break;

			case fieldOffsetEvolWrSensorFilmSelectorUp:
				HfcObject::serializedData.append(Integer::toString(genSen.getFilmSelectorUp()));
				break;
			case fieldOffsetEvolWrSensorFilmSelectorDown:
				HfcObject::serializedData.append(Integer::toString(genSen.getFilmSelectorDown()));
				break;
			case fieldOffsetEvolWrSensorWandUp:
				HfcObject::serializedData.append(Integer::toString(genSen.getWandUp()));
				break;
			case fieldOffsetEvolWrSensorWandDown:
				HfcObject::serializedData.append(Integer::toString(genSen.getWandDown()));
				break;
			case fieldOffsetEvolWrSensorKnifeDown:
				HfcObject::serializedData.append(Integer::toString(genSen.getKnifeDown()));
				break;
			case fieldOffsetEvolWrSensorApplierHand:
				HfcObject::serializedData.append(Integer::toString(genSen.getApplierHand()));
				break;
			case fieldOffsetEvolWrSensorGripperNarrow:
				HfcObject::serializedData.append(Integer::toString(genSen.getGripperNarrow()));
				break;
			case fieldOffsetEvolWrSensorGripperRight:
				HfcObject::serializedData.append(Integer::toString(genSen.getGripperRight()));
				break;
			case fieldOffsetEvolWrSensorGripperLeft:
				HfcObject::serializedData.append(Integer::toString(genSen.getGripperLeft()));
				break;
			case fieldOffsetEvolWrSensorSideClampLeft:
				HfcObject::serializedData.append(Integer::toString(genSen.getSideClampLeft()));
				break;

			case fieldOffsetEvolWrSensorSecLabelRotationHome:
				HfcObject::serializedData.append(Integer::toString(genSen_2.isSecLabelRotationHome()));
				break;

			case fieldOffsetEvolWrSensorSecLabelSlideHome:
				HfcObject::serializedData.append(Integer::toString(genSen_2.isSecLabelApplierSlideHome()));
				break;

			default:
				break;
			}
		} // for

		break;
	}

	default:
		Debug::trace("HfcEvolWrapperSensors", "serialize(Invalid format!!!)");
		break;
	}

	return HfcObject::serializedData.data();
}
