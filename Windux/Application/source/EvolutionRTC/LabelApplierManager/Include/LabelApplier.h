#ifndef LABELAPPLIER_H
#define LABELAPPLIER_H



	// SMM -- ERRATA!!!
	/* PACKING OF STRUCTS!!!
	From http://www.delorie.com/djgpp/v2faq/faq22_12.html

	Q: When I use struct ffblk from the header dir.h in a C++ program, I get garbage in some members of the structure! 



	A: There is a known bug in GCC 2.7.2: the C++ compiler effectively ignores the __attribute__((packed)) directives, so the structures end up being not packed. GCC versions 2.7.2.1 and later corrected that bug, so upgrade. As a work-around, surround the declaration of the structure that needs to be packed with #pragma pack, like this: 

	  #ifdef __cplusplus
	  #pragma pack(1)
	  #endif
	  .
	  .
	  .
	  #ifdef __cplusplus
	  #pragma pack()
	  #endif
	*/

#ifdef __cplusplus
	#pragma pack(1)
#endif



	typedef enum LabelApplierKind
	{
		NoLabelApplier,
		ILA,
		CompactLabelApplier,
		EvolutionLabelApplier,
		SecurityLabelApplier,
		CompactLabelApplier_EPSAC2
	}   LabelApplierKind_t;

	/* Transfer Station Types */
	typedef enum TransferStationType
	{
		NSW,
		HandWrapper,
		Bakery,
		ESW,
		Mini
	}   TransferStationType_t;

	/* CLA Operation Modes */
	typedef enum LaOperatingMode
	{
	    Manual,
	    SemiAuto,
	    Auto,
	    Conveyor,
	    Simulation,
	    CService
	}   LaOperatingMode_t;

	typedef enum ConveyorSensitivity
	{
		ConveyorSensitivityLow = 0x10,
		ConveyorSensitivityMedium = 0x38,
		ConveyorSensitivityHigh = 0x60,
	}   ConveyorSensitivity_t;

	#define MAX_CLA_LABEL_OFFSET            30
	#define DEFAULT_CLA_LABEL_OFFSET         8
	#define MAX_CLA_JUSTIFY_DISTANCE       200
	#define DEFAULT_CLA_JUSTIFY_DISTANCE    75
	#define MAX_CLA_DIAG_OFFSET             15
	#define DEFAULT_CLA_DIAG_OFFSET          0
	//Alex Zavala 8/20
	#define DEFAULT_LABEL_PLACEMENT			 0
	#define DEFAULT_OVERDRIVE				10

	typedef enum LabelApplierMotorSpeed
	{
		SlowMotorSpeed = 0x40,
		FastMotorSpeed = 0x60
	}   LabelApplierMotorSpeed_t;

	typedef enum LabelApplierMotorDirection
	{
		ForwardDirection =  1,
		ReverseDirection = -1
	}   LabelApplierMotorDirection_t;

	typedef enum LaConfigType
	{
		LaTemporary = 0x0000,
		LaPermanent = 0x0001,
		LaDefault = 0x0002,
	}   LaConfigType_t;

	typedef enum LaStationID
	{
	    WeigherStation,
	    PrimaryPrinterStation,
	    SecondaryPrinterStation,
	    ThirdPrinterStation
	}   LaStationID_t;

	typedef enum LaRotation
	{
		Rotate0,
		Rotate45,
		Rotate90,
		Rotate135,
		Rotate180,
		Rotate225,
		Rotate270,
		Rotate315,
		NoRotationSelected
	}   LaRotation_t;

	typedef enum LaTestID
	{
		NoService,	//Added 9/12 Alex Zavala
		EnablePrinterMotor,
		EnableWeigherMotor,
		//DisablePrinterMotor,
		//DisableWeigherMotor,
		//BrakeWeigherMotor,
		//BrakePrinterMotor,
		//ReleaseWeigherMotor,
		//ReleasePrinterMotor,
		//TorqueToWeigher,
		//TorqueToPrinter,
		EnableCompressor,
		DisableCompressor,
		GateUp,
		GateDown,
		ActivateBelts,
		ActivateRollers,
		ActivateWand,
		ReleaseWand,
		ActivateApplier,
		ReleaseApplier,
		//RotateWand,
		//EnableDriveSignals,
		//DisableDriveSignals,
		//SlaveInput,
		//SlaveOutput,
		EnableWandVacuum,
		DisableWandVaccum,
		ResetMechs,	//Added 9/12 Alex Zavala
		EnableRunIn,//Added 9/18 Alex Zavala
	}   LaTestID_t;

	/* Note: Do Not Change the order of the PR_1 and PR_2 sensors  */
	/* Before adding a new sensor examine function ProcessLOUpdateSensor  */
	/* and function BuildLOEmitter in module mcla_msg.c. */

	/*  Sensor Control  */
	typedef enum LaSensorID
	{
	    ExpectPackage,
	    WeigherEntry,
	    Pr1Entry,
	    Pr1Gate,
	    Pr1LabelPosition,
	    Pr1Valid,
	    Pr2Entry,
	    Pr2Gate,
	    Pr2LabelPosition,
	    Pr2Valid,
	    LastSensor
	}   LaSensorID_t;

	// Evolution wrapper's LocationRight is 0 and LocationLeft is 1
	typedef enum LaPosLoc
	{
	   	LocRight,
		LocLeft,
		LocNoneSelected,
		LocLowerRight,
		LocLowerLeft,
		LocRightOfPrimary,
		LocLeftOfPrimary,
		LocTopOfPrimary,
		LocBottomOfPrimary
	}   LaPosLoc_t;
	//	LaPlacement is used for Ultima type label appliers. We still need to determine Leading or Trailing
	//	edge in Service but will convert to the apply position of Right or Left above (LaPosLoc) for Operate mode.
	typedef enum LaPlacement
	{
		LaLeadingEdge,
		LaTrailingEdge,
		LaNoneSelected
	}LaPlacement_t;

	typedef enum LaSideDischarge
	{
		RightDischarge,
	   	LeftDischarge
	}   LaSideDischarge_t;

	typedef enum 
	{
		laApp,
		laData,
		laBoot,
		laAppData,
		laAppBoot,
		laDataBoot,
		laAll	
	}LaComponentUpdate;

	typedef enum
	{
		laSlaveBootInit,
		laSlaveBootReady,
		laSlaveBootFault,
		laSlaveBootDone
	}LaSlaveLoaderState;

	typedef enum
	{
		laSlaveNone,
		laSlaveTimeout,
		laSlaveWriteFailure,
		laSlaveCheckSum,
		laSlaveUnknownRecordType,
		laSlaveEraseFailureBank1,
		laSlaveEraseFailureBank2,
		laSlaveComponentNotSet,
		laSlaveNodeImageMismatch,
		laSlaveNodeIdError
	}LaSlaveLoaderFaults;

	typedef enum
	{
		claData,
		claBoot,
		claApp
	}LaSoftwareComponent;


#ifdef __cplusplus
	#pragma pack()
#endif



#endif //LABELAPPLIER_H
