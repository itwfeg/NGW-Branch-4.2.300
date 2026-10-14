#ifndef LABELAPPLIEREVENT_H
#define LABELAPPLIEREVENT_H

	#include "Event.h"

	//Label Applier Device Generated Events - LabelApplier
	typedef enum LaEvent
	{
		LaWakeup,
		LaConfig,
		LaInquire,
		LaReset,
		LaStatus,
		LaEnable,
		LaDisable,
		LaMode,
		LaTest,
		LaOpenWindow,
		LaCloseWindow,
		LaLabelReady,
		LaSensors,
		LaPackageDischarge,//Alex Zavala 1/3/14
		LaSetup = 14, 
		LaOff,
		LaDefine,
		LaExecute,
		LaUpdateSensor,
		LaEmitter,
		LaEnableSensor,
		LaSysStatus = 21,
		LaPackageReady = 22,
		LaLabelTaken,
		LaOn,
		LaSysInfo,//Added SysInfo/Write/Erase, TestComplete Alex Zavala 9/5
		LaSysInfoWrite,
		LaSysInfoErase,
		LaTestComplete,
		LaReqSysInfo,//Added Alex Zavala 9/6
		LaUpdate,//Added Alex Zavala 9/23
		LaLoaderStatus,//Added Alex Zavala 9/26
		LaPrintPackage = 0x20,
		LaUltimaAutoMode,  //message received when first sensor is tripped
		LaReqInquire,
		LaReqStatus,
		LaReqTest,
		LaReqCloseWindow,
		LaReqSensors,
		LaReqOff,
		LaSensorsTracking,
		LaReqWakeup,		// Request Wakeup message from Peripheral
		LaReqKind = 0x30,
		LaKind,
		LaSndLabelPos,
		LaILACycle = 0x50,
		LaILAForceLabel,
		LaILAOpenWindow,
		LaILACloseWindow,
		LaILAOff,
		LaILAReqSpeed,
		LaILASpeed,
		LaEvolInquire = 100,
		LaEvolConfig,
		LaCLAUpgradeInProgress,//Added 9/23 Alex Zavala
		LaRemoveUpgradeFiles,
		LaReqSystemReset,
		LaSndTwoLabelPos
	} LaEvent_t;

	typedef enum LabelApplierErrors
	{
		LaErrUnknownError = LabelApplier * DEVICE_ERROR_OFFSET,
		LabelApplierErrCommLoss,
		LaErrHighVoltageError,
		LaErrPrinter1ArmOutError,
		LaErrPrinter2ArmOutError,
		LaErrPrinter1NoWakeupError,
		LaErrPrinter2NoWakeupError,
		LaErrWeigherHardwareError,
		LaErrPrinter1HardwareError,
		LaErrPrinter2HardwareError,
		LaErrPackageOverrunError,
		LaErrModeError,
		LaErrManagerDeleted,
		LaErrReserved1Error,
		LaErrReserved2Error,
		LaErrBakeryCLAError,
		LaErrBeginDeletingFirmware,//Alex Zavala 10/3
		LaErrFirmwareDeleted,//Alex Zavala 10/3
		LaErrWeigherConveyorBadHallReading,//Alex Zavala 10/10
		LaErrWeigherConveyorJammed,//Alex Zavala 10/10
		LaErrWeigherConveyorStall,//Alex Zavala 10/10
		LaErrWeigherConveyorPkgSensor,//Alex Zavala 10/10
		LaErrConveyorBadHallReading,//Alex Zavala 10/10
		LaErrConveyorJammed,//Alex Zavala 10/10
		LaErrConveyorStall,//Alex Zavala 10/10
		LaErrConveyorPkgSensor,//Alex Zavala 10/10
		LaErrConveyorPkgEdgeSensor,//Alex Zavala 10/10
		LaErrMotor1FuseBlown,//Alex Zavala 10/10
		LaErrMotor2FuseBlown,//Alex Zavala 10/10
		LaErrPkgWakeUpSensor,//Alex Zavala 10/10
		LaErrPkgGateSensor//Alex Zavala 1/8/14
	} LabelApplierErrors_t;

#endif // LABELAPPLIEREVENT_H
