/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERSENSORS_H
#define HFCEVOLWRAPPERSENSORS_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperSensors.h"

typedef enum HfcEvolWrapperSensorsFieldOffsets
{
	fieldOffsetEvolWrSensorPrimaryHome = 1,
	fieldOffsetEvolWrSensorShiftHome,
	fieldOffsetEvolWrSensorShiftLeftLimit,
	fieldOffsetEvolWrSensorGripperHome,
	fieldOffsetEvolWrSensorSideFolderHome,
	fieldOffsetEvolWrSensorPusherHome,
	fieldOffsetEvolWrSensorFrontFolderHome,
	fieldOffsetEvolWrSensorRearFolderHome,
	fieldOffsetEvolWrSensorElevatorHome,
	fieldOffsetEvolWrSensorLabelRotationHome,
	fieldOffsetEvolWrSensorElevatorPkg,
	fieldOffsetEvolWrSensorTransferPkg,
	fieldOffsetEvolWrSensorStart,
	fieldOffsetEvolWrSensorStop,
	fieldOffsetEvolWrSensorSideClampRight,
	fieldOffsetEvolWrSensorSideClampHome,
	fieldOffsetEvolWrSensorFilmRoll,
	fieldOffsetEvolWrSensorKnifeUp,

	fieldOffsetEvolWrSensorInterlockStop,
	fieldOffsetEvolWrSensorInterlockRightCanopy,
	fieldOffsetEvolWrSensorInterlockRightSideCover,
	fieldOffsetEvolWrSensorInterlockFilmSupplyDoor,
	fieldOffsetEvolWrSensorInterlockLeftSideCover,
	fieldOffsetEvolWrSensorInterlockIntakeCleanout,

	/* SOF-3838  no longer used repurpose for second label applier
	fieldOffsetEvolWrSensorSwitched24V,
	fieldOffsetEvolWrSensorHighVoltageInrush,
	*/
	fieldOffsetEvolWrSensorSecWandUp,
	fieldOffsetEvolWrSensorSecWandDown,
	
	fieldOffsetEvolWrSensorFilmSelectorLever,
	
	/* SOF-3838  no longer used repurpose for second label applier
	fieldOffsetEvolWrSensorPrinterPlate,
	*/
	fieldOffsetEvolWrSensorSecApplierHand,

	fieldOffsetEvolWrSensorFilmSelectorUp,
	fieldOffsetEvolWrSensorFilmSelectorDown,
	fieldOffsetEvolWrSensorWandUp,
	fieldOffsetEvolWrSensorWandDown,
	fieldOffsetEvolWrSensorKnifeDown,
	fieldOffsetEvolWrSensorApplierHand,
	fieldOffsetEvolWrSensorGripperNarrow,
	fieldOffsetEvolWrSensorGripperRight,
	fieldOffsetEvolWrSensorGripperLeft,
	fieldOffsetEvolWrSensorSideClampLeft,

	fieldOffsetEvolWrSensorSecLabelRotationHome,			// secLabelRotationHome

	fieldOffsetEvolWrSensorSecLabelSlideHome,               // secLabelApplierSlideHome

	endHfcEvolWrSensorsFieldOffsets	//	MUST be last!

} HfcEvolWrapperSensorsFieldOffsets_t;

class HfcEvolWrapperSensors : public EvolWrapperSensors, public HfcObject
{
	public:
		HfcEvolWrapperSensors(const char *data);

		~HfcEvolWrapperSensors();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLWRAPPERSENSORS_H
