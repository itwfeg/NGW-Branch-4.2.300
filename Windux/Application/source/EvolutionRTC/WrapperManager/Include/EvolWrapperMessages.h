#ifndef EVOLWRAPPERMESSAGES_H
#define EVOLWRAPPERMESSAGES_H

#include "Wrapper.h"
#include "EvolWrapperTest.h"

#ifdef __cplusplus
#pragma pack(1)
#endif

typedef enum EvolWrapperError
{
	eUnknown
} EvolWrapperError_t;

/* Generic */
typedef struct
{
	UINT16	  wrEvent;
} EvolWrGeneric;

typedef struct
{
	UINT16	  wrEvent;
	UINT16	  param;
} EvolWrGenericOneParam;


typedef struct
{
	UINT16	  wrEvent;
	UINT16	  nodeId;
	UINT16	  status;
	UINT16	  fault;
	UINT16	  updateFailureCount;	 
	UINT16	  softComponent;  
	UINT32	  addr;
} EvolWrLoaderStatus;

typedef struct
{
	UINT16	  wrEvent;
	UINT16	  node1;
	UINT16	  update1;
	UINT16	  node3;
	UINT16	  update3;
	UINT16	  node4;
	UINT16	  update4;
	UINT16	  node5;
	UINT16	  update5;
	UINT16	  node6;
	UINT16	  update6;
	UINT16	  node2;
	UINT16	  update2;
}WrUpdateWrapper;

typedef struct 
{
	UINT16 major;
	UINT16 minor;				
}ComponentRevision;

typedef struct
{
	ComponentRevision appFlash;
	ComponentRevision dataFlash;
	ComponentRevision bootFlash;				
}NodeVersion;

typedef enum 
{
	WrTypeUnknown,
	WrType57,
	WrType67
} WrBoardHWType;

typedef struct
{
	UINT16		nodeid;
	UINT16		bit_error;			//	  Unused
	UINT16		device_init_flags;	//	  Indicates if device init was error free
	NodeVersion nodeVersion;		//	  Wrapper's software version information
	UINT16		pad0;
	UINT32		error_flags;		//	  Results of power up self tests
//	UINT32		hwVersion;
}WrSysStatusInfo_Old;

typedef struct
{
	UINT16		nodeid;
	UINT16		config_version;		//	  Internal configuration #
	UINT16		device_init_flags;	//	  Indicates if device init was error free
	NodeVersion nodeVersion;		//	  Wrapper's software version information
	UINT16		pad0;
	UINT32		error_flags;		//	  Results of power up self tests
	UINT32		wrap_box_checksum;	//	  Checksum of the wrapper timings stored in DATA FLASH
}WrSysStatusInfo;


typedef struct
{
	UINT16		wrEvent;
	UINT16		pad0;
	UINT16		report_sys_error;
	UINT16		pad1;
	WrSysStatusInfo_Old  systems_status[6];
}WrSysStatus_Old;

typedef struct
{
	UINT16	  wrEvent;
	UINT16	  pad0;
	UINT16	  report_sys_error;
	UINT16	  pad1;
	WrSysStatusInfo	  systems_status[6];
}WrSysStatus;

/* Wakeup */
typedef struct
{
	UINT16 wrEvent;
	UINT16 allNodeAvailable;
	UINT16 wrCfgStatus;
	UINT16 wrapperOperation;	//	WrOperation
} EvolWrWakeup;

typedef struct
{
	UINT16 reportStatus;
	UINT16 openWindow;
	UINT16 limpHome;
	UINT16 wrapperStopped;
	UINT16 mainPower;
	UINT16 mainMotor;
	UINT16 wrapperState;
} EvolWrStatusField;

typedef struct
{
	UINT16 primaryConveyor;
	UINT16 secondaryConveyor;
	UINT16 secondaryShift;
	UINT16 elevator;
	UINT16 filmSupply;
	UINT16 gripper;
	UINT16 knife;
	UINT16 sideClamp;
	UINT16 sideFolder;
	UINT16 rearFolder;
	UINT16 frontFolder;
	UINT16 pusher;
	UINT16 sealerBelt;
	UINT16 labelApplier;
	UINT16 generalWrapper;
	UINT16 diagnosticsSupport;
} EvolWrErrorField;

#define MaxNodeBoards 5

typedef struct
{
	UINT16 nodeId;
	UINT16 temp;
	UINT16 statusBits;
} EvolWrNodeInternalTemperature;

typedef struct
{
	UINT16 sealerBeltTemperature;
	UINT16 sealerBeltSetting;
	UINT16 sealerBeltStatusBits;
	UINT16 sealerBeltHeaterEnabled;
	UINT16 filmTemperature;
	UINT16 filmSetting;
	UINT16 filmStatusBits;
	UINT16 filmTempEnabled;
	UINT16 ambientTemperature;
	UINT16 ambientTemperatureStatusBits;
	EvolWrNodeInternalTemperature nodeBoardTemperature[MaxNodeBoards];
	UINT16 internalTemperatureFailed;
} EvolWrTemperatureField;

/* Status */
typedef struct
{
	UINT16 wrEvent;
	EvolWrStatusField statusInfo;
	EvolWrErrorField currErrorInfo;
	EvolWrErrorField prevErrorInfo;
	UINT16 wrapperMode;
	EvolWrTemperatureField temperatureInfo;
} EvolWrStatus;

typedef struct
{
	UINT16	  wrEvent;
	UINT16	  intakeMode;
	UINT16	  packageId; 
	UINT16	  pullFilm;  
	UINT16	  packageWeight;
	UINT16		pad;
	UINT32		pluNumber;
} EvolWrWrap;

typedef struct
{
	UINT16	  wrEvent;
	UINT16	  packageId; 
	UINT16	  pullFilm;  
} EvolWrCycle, EvolWrJog;

typedef struct
{
	UINT16	wrEvent;
	UINT16	sealerBeltTempSetting;
	UINT16	sealerBeltHeaterEnabled;
	UINT16	filmSupplyTempSetting;
	UINT16	filmSupplyHeaterEnabled;
	UINT16	filmTension;
	UINT16	filmWidthSelection;
	INT16	filmTensionFrontToBack;
	INT16	filmTensionRightToLeft;
	UINT16	intakeMode;
} EvolWrControl;

typedef struct
{
	UINT16	wrEvent;
	UINT16	mode;
} EvolWrMode;

typedef struct
{
	UINT16	wrEvent;
	UINT16	filmWidthSelection;
} EvolWrLoad;

typedef struct
{
	UINT16	isUsed;
	INT16	maxTemp;
	INT16	minTemp;
	UINT16	defaultTempSetting;
} EvolHeaterCfg;

typedef struct
{
	UINT32			checksum;
	EvolHeaterCfg	filmHeater;
	EvolHeaterCfg	sealerHeater;
} EvolWrHeaterCfg;

typedef struct
{
	UINT32	checksum;
	UINT16	autoRotation;
	UINT16	priLabelSizeDefault;
	UINT16	priLabelWidth;
	UINT16	priLabelRotationDefault;
	UINT16	priLabelLocationDefault;
	INT16	priLabelOffsetOnPkg;
	UINT16	priRotationCalibrationOffset;	//	Used to fine tune the rotation.
	UINT16	secLabelSizeDefault;
	UINT16	secLabelWidth;
	UINT16	secLabelRotationDefault;
	UINT16	secLabelLocationDefault;
	INT16	secLabelOffsetOnPkg;
	UINT16	secRotationCalibrationOffset;	//	Used to fine tune the rotation.
	UINT16	secondCLAAttached;				//	Was: sideDischarge;	//	false = Front discharge.
	UINT16	sideDischargeType;				//	RightDischarge or LeftDischarge.
	UINT16	priWandCalibrationOffset;	//	Used to fine tune the wand home.
	UINT16	secWandCalibrationOffset;	//	Used to fine tune the wand home.
	UINT16	priRotationCalibrationOffset_90Deg;	//	Used to fine tune the rotation.
	UINT16	priRotationCalibrationOffset_180Deg;	//	Used to fine tune the rotation.
	UINT16	priRotationCalibrationOffset_270Deg;	//	Used to fine tune the rotation.
} EvolLaCfg;

typedef struct
{
	UINT16	  rollWidth;
	UINT16	  rollDiameter;
	UINT16	  remainingOnRoll;
} EvolFilmWidthDim;

typedef struct
{
	UINT32				checksum;
	EvolFilmWidthDim	topFilmRoll;
	EvolFilmWidthDim	bottomFilmRoll;
	INT16 				wrapperOperation;
	UINT16				filmTensionNetDefault;
	UINT16 				addOnHardware;		
	INT16				filmTensionServiceAdjust;
	UINT16				maxPackageWeight;				//not in use on AWS -- ats			
	UINT16				minPackageWeight;				//not in use on AWS -- ats			
	UINT16				maxPackageLength;				//not in use on AWS -- ats			
	UINT16				minPackageLength;				//not in use on AWS -- ats			
	UINT16				maxPackageWidth;				//not in use on AWS -- ats			
	UINT16				lowerFilmPullTension;
	UINT16				upperFilmPullTension;
	UINT16				sealerBeltPackageSensorType;
	UINT32				packageGrandTotal;
	UINT32				wrapperCycleTotal;
	UINT16				filmTensionFrontToBack;
	UINT16				filmTensionRightToLeft;
} EvolWrGenCfg;

typedef struct
{
	EvolWrHeaterCfg	heaterConfig;
	EvolWrGenCfg	wrapperGenConfig;
	EvolLaCfg		labelApplierConfig;
} EvolWrCfg;

typedef struct
{
	UINT16	  functionCode;
	UINT16	  disposition;
	UINT16	  configVersion;
	UINT16	  pad;
	EvolWrCfg config;
} EvolWrConfig, EvolWrInquire;

typedef struct
{
	UINT16 name[10];
	UINT16 pluNumHigh;
	UINT16 pluNumLow;
	UINT16 lengthLow;
	UINT16 lengthHigh;
	UINT16 widthLow;
	UINT16 widthHigh;
	UINT16 lipHeight;
} EvolWrWrapBox;

// SOF-6077 - Needed my own struct for 1P WITHOUT the 2P params
typedef struct
{
	UINT16 height;
	UINT16 percentStretch;
	UINT16 knifeFireHeight;
	UINT16 clampOpenHeight;
	UINT16 elevatorPlacement;
	UINT16 addTrayLength;
	INT16  gripperPullAdjustment;
	INT16  frontUnderfolderAdjustment;
	UINT16 wrapSpeed;
	UINT16 dischargeSpeed;
	UINT16 sideUnderfolderAdjustment;
	UINT16 heightRestriction;
	UINT16 sideUnderfolderReturnHome;
	UINT16 gripperAllowShortPull;
	UINT16 sideClampGap;
	UINT16 filmRollToUse;
	UINT16 rearUnderfolderTripDelay;
	UINT16 brakeTiming;
	UINT16 outsideGripperRelease;
	UINT16 insideGripperRelease;
} EvolWrHeightBox1P;

typedef struct
{
	UINT16 height;
	UINT16 percentStretch;
	UINT16 knifeFireHeight;
	UINT16 clampOpenHeight;
	UINT16 elevatorPlacement;
	UINT16 addTrayLength;
	INT16  gripperPullAdjustment;
	INT16  frontUnderfolderAdjustment;
	UINT16 wrapSpeed;
	UINT16 dischargeSpeed;
	UINT16 sideUnderfolderAdjustment;
	UINT16 heightRestriction;
	UINT16 sideUnderfolderReturnHome;
	UINT16 gripperAllowShortPull;
	UINT16 sideClampGap;
	UINT16 filmRollToUse;
	UINT16 rearUnderfolderTripDelay;
	UINT16 brakeTiming;
	UINT16 outsideGripperRelease;
	UINT16 insideGripperRelease;
	// SOF-6077: The SizeOf this data structure RIGHT HERE RIGHT NOW is 40.
	// We cant just add buffer += sizeOf(EvolWrHeightBox) because we have to consider 1P vs 2P
	// 
    // TODO VERTICAL OFFSETS - THIS NEEDS FIXED for 4.2.300 - 01-08-2023: The time to fix it has arrived
	// FIXED 01-10-2023 - USB WILL NEVER call CAN Messages so we better NEVER see these
	// FIX for: SOF-5475,5476 was to comment these out
	//
	UINT16 primaryLabelVerticalOffset;
	UINT16 secondaryLabelVerticalOffset;
	// SOF-6077: The SizeOf this data structure RIGHT HERE RIGHT NOW is 44 with the 2P Params added.
	// We cant just add buffer += sizeOf(EvolWrHeightBox) because we have to consider 1P(40) vs 2P(44)
	// These vars, the UINT16s and INT16s are each 2bytes each
} EvolWrHeightBox;

// SOF-6077 - these vars have taken over as being the size of the structure to use for READ/SAVE for Timings
static const UINT16 USE_1P_STRUCTURE_SIZE = 40;		// create memory of this size for 1P wrappers
static const UINT16 USE_2P_STRUCTURE_SIZE = 44;		// create memory of this size for 2P wrappers

typedef struct
{
	UINT16 wrEvent;
	UINT16 disposition;
	UINT16 wrapBoxes;
} EvolWrPackageTimingsRead;

typedef enum
{
	WRITE_PERMANENT = 0,
	WRITE_TEMPORARY = 1,
	WRITE_DEFAULT
} WRITE_DESTINATION;

typedef struct
{
	UINT16 wrEvent;
	UINT16 disposition;
	UINT16 wrapBoxes;
} EvolWrPackageTimingsWrite;

typedef struct
{
	UINT16	data1;
	UINT16	data2;
} EvolWrIntTestData;

typedef union
{
	EvolWrIntTestData	dataInt;
	UINT32				dataLong;  
} EvolWrTestData;

typedef struct
{
	UINT16			wrEvent;
	UINT16			testNum;
	EvolWrTestData	parameter;		//	Optional test info.
} EvolWrTest;

typedef struct
{
	UINT16			wrEvent;
	UINT16			testNum;
	EvolWrTestData	parameter;		//	Optional test info.
	UINT16			passed;			//	Result of the test.
} EvolWrTestComplete;

typedef struct
{
	UINT16	wrEvent;
	UINT16  id;	
	UINT16  length; 				//  int decimal form, ie 100 is 10.0 inches
	UINT16	width;					//	int decimal form, ie 100 is 10.0 inches
	UINT16  height;					//  int decimal form, ie 100 is 10.0 inches
	UINT16	lip;					//	int decimal form, ie 100 is 10.0 inches
	UINT16	filmWidthUsedToWrap;	//	film width used to wrap this package,  int decimal form, ie 100 is 10.0 inches		
	UINT16	wrapBoxIndex;
	UINT16  heightBoxIndex;
	UINT16  usedWrapBoxBestFit;		//	bool
	UINT16  modifiedNetStretch;		//	bool
} EvolWrPackageDesc;

typedef struct
{
	UINT32	primaryHomeSwitch			:1;		//		bit 1 (LSB)
	UINT32	shiftHomeSwitch				:1;
	UINT32	shiftLeftLimitSwitch		:1;
	UINT32	gripperHomeSwitch			:1;
	UINT32	sideFolderHomeSwitch		:1;		//		bit 5
	UINT32	pusherHomeSwitch			:1;
	UINT32	frontFolderHomeSwitch		:1;
	UINT32	rearFolderHomeSwitch		:1;
	UINT32	elevatorHomeSwitch			:1;
	UINT32	labelRotationHomeSwitch		:1;		//		bit 10
	UINT32	elevatorPkgSensor			:1;
	UINT32	transferPkgSensor			:1;
	UINT32	startSwitch					:1;
	UINT32	stopSwitch					:1;
	UINT32	sideClampRightSensor		:1;		//		bit 15
	UINT32	sideClampHomeSwitch			:1;
	UINT32	filmRollSensor				:1;
	UINT32	knifeUpSensor				:1;
/* SOF-3838 no longer used repurpose for second label applier
	UINT32	switched24V					:1;
	UINT32	highVoltageInrush			:1;		//		bit 20 */

	UINT32 secWandUpSensor				:1;
	UINT32 secWandDownSensor			:1;		//		bit 20

	UINT32	filmSelectorLever			:1;
/* SOF-3838 no longer used repurpose for second label applier	
	UINT32	printerPlateSwitch			:1; */

	UINT32	secApplierHandSensor		:1;

	UINT32	filmSelectorUpSensor		:1;
	UINT32	filmSelectorDownSensor		:1;
	UINT32	wandUpSensor				:1;		//		bit 25
	UINT32	wandDownSensor				:1;
	UINT32	knifeDownSensor				:1;
	UINT32	applierHandSensor			:1;
	UINT32	gripperNarrowSensor			:1;
	UINT32	gripperRightSensor			:1;		//		bit 30
	UINT32	gripperLeftSensor			:1;
	UINT32	sideClampLeftSensor			:1;
} EvolWrSwitchSensorBits;

typedef union
{
	EvolWrSwitchSensorBits	bits;
	UINT32					field;  
} EvolWrSwitchesAndSensors;

typedef struct
{
	UINT32 secLabelRotationHome		:1;
	UINT32 secLabelApplierSlideHome :1;
} EvolWrSwitchSensor_2Bits;

typedef struct
{
	EvolWrSwitchSensor_2Bits	bits;
	UINT32						field;
} EvolWrSwitchesAndSensors_2;

typedef struct
{
	UINT16	stopSwitch			:1;		//		bit 1 (LSB)
	UINT16	rightCanopy			:1;
	UINT16	rightSideCover		:1;
	UINT16	filmSupplyDoor		:1;
	UINT16	leftSideCover		:1;		//		bit 5
	UINT16	intakeCleanout		:1;
	UINT16	unused				:10;	//		bits 7 - 16 (MSB)
} EvolWrInterlockSwitchBits;

typedef union
{
	EvolWrInterlockSwitchBits	bits;
	UINT16						field;  
} EvolWrInterlockSwitches;

typedef struct
{
	UINT16						wrEvent;
	UINT16						pad;	
	EvolWrSwitchesAndSensors	switchesSensors;
	EvolWrInterlockSwitches		interlockSwitches;
	EvolWrSwitchesAndSensors_2	switchesSensors_2;
} EvolWrSensors;


//	Wait until they iron out the details of the messages.
typedef struct
{
	UINT16	wrEvent;
	UINT16 	camera;
	UINT8 	cameraImage[128];	
} EvolWrCameraScan;

typedef struct
{
	UINT16	wrEvent;
	UINT16 	cameraHeightScan;
} EvolWrCameraHeightScan;

typedef struct
{
	UINT16	wrEvent;
	UINT16	camera;
	UINT16 	calResult;
	UINT16 	scaleFactor;
	UINT16	firstPixel;
	UINT16 	lastPixel;
} EvolWrCameraCal;

typedef struct
{
	UINT16 wrEvent;
	UINT16 month;
	UINT16 day;
	UINT16 year;
	UINT16 hours;
	UINT16 minutes;
	UINT16 seconds;
} EvolWrDateTime;

typedef struct
{
	UINT16 wrEvent;
	UINT16 eventCaptureType;
	UINT16 packageId;
	UINT16 pad;
	UINT32 eventValue;
	UINT16 pad1;
	UINT16 errorNum;
} EvolWrEventCapture;

typedef struct
{
	UINT16 wrEvent;
	UINT16 wrError;
	UINT8 trayName[20];			//UINT16 trayName[5];
	UINT16 packageLength;	
	UINT16 packageWidth;
	UINT16 packageHeight;
	UINT16 filmRollUsed;
	UINT16 usedWrapBoxBestFit;		//	bool
	UINT16 modifiedNetStretch;		//	bool
} EvolWrErrorHistory;

typedef struct
{
	UINT16	sensorType;
	UINT16	detectorType;
	UINT16	emitterPowerLevel;
	UINT16	isOldRev;
	UINT16	sensorInError;
} EvolWrPkgSensorStatus;

typedef struct
{	
	UINT16 wrEvent;
	UINT16	pad;
	UINT32 cycleCount;		
} EvolWrCycleCount;

typedef struct
{
	UINT16 wrEvent;
	UINT16 cameraThreshold;
} EvolWrCameraThreshold;

typedef struct
{
	UINT16  wrEvent;
	UINT16	validBoardChecksum;		
	UINT8	motorBoardPartNumber[10];
	UINT8	motorBoardRev[2];
	UINT8	motorBoardDateOfManufacture[6];
	UINT16	validAwsChecksum;
	UINT8	awsSerialNumber[12];
	UINT8	awsDateOfManufacture[6];
	UINT8	awsDateOfInstall[6];
	UINT16	awsModelNumber;
	UINT16	pad0;
	UINT32	awsCycleCount;
} EvolWrSystemInfo;

typedef struct
{
	UINT16  wrEvent;
	UINT8	awsSerialNumber[12];
	UINT8	awsDateOfManufacture[6];
	UINT8	awsDateOfInstall[6];
	UINT16	awsModelNumber;
	UINT32	awsCycleCount;
} EvolWrSystemInfoWrite;

typedef struct
{
	UINT16  wrEvent;
	UINT16  bldcMechanism;
	UINT16	motorStatus;
	UINT16  motorCapacityUsage;
} EvolWrMotorProfile;

typedef struct
{
	UINT16  wrEvent;						// WR_DIAGNOSTICS_TEST
	UINT16	test;
} EvolWrDiagnosticsTest;

typedef struct
{
	UINT16	wrEvent;						// WR_DIAGNOSTICS_TEST_RESULT
    UINT16	test;
    UINT16	testResult;						// 1 - passed, 0 - failed
    UINT16	openLoopOverride;				// 1 - true, 0 - false : IF TRUE, MACHINE HAS DISABLED SENSOR FEEDBACK AND WILL PERFORM "OPEN LOOP"
    UINT16	diagRoutine;					// diagnostic routine run
    UINT16	diagRoutineExpectedResult;		// what the routine expected in return
    UINT32	diagRoutineActualResult;		// what the routine actually received in return
    UINT16	errorCode;						// diagnostic error code
} EvolWrDiagnosticsTestResult;

typedef struct 
{
	UINT16	wrEvent;
	UINT16	length;
	UINT16	width;
	UINT16	lip_height;
} EvolWrDimensionOverrides;

/*
	typedef struct
	{
	UINT8		wrEvent;
	UINT8		pad0;
	UINT8		test;
	UINT8		pad1;
	UINT16		data;
	} UltimaWrTest;

	typedef struct
	{
	UINT8		wrEvent;
	UINT8		pad0;
	UINT16		heaterReading;  
	UINT16		encoderCount;	
	UINT16		intakeCount; 

// MSB -- Most significant Byte (transmitted from the UWS)
// byte 0 lsb..msb
UINT32  stopSwitch					  :1;	  // lsb -- least significant bit
UINT32  undefined0					  :7;

// byte 1 lsb..msb
UINT32  narrowFilmSwitch		:1;	  // lsb -- least significant bit
UINT32  wideFilmSwitch		  :1;	 
UINT32  packageJamSwitch		:1;	 
UINT32  pusherHomeSwitch		:1;	 
UINT32  packageWidthSwitch	  :1;	 
UINT32  packageSenseSwitch	  :1;	 
UINT32  packageLengthSwitch	 :1;	 
UINT32  startSwitch					 :1;	 // msb -- most significant bit

// byte 2 lsb..msb
UINT32  dischargeMotorState	 :1;	  // lsb -- least significant bit
UINT32  mainMotorState		  :1;	 
UINT32  compressorState		 :1;	 
UINT32  heaterState					 :1;	 
UINT32  slideBlockState		 :1;	 
UINT32  undefined1					  :3;	 // ms 3 bits

// LSB -- Least Significant Byte  (transmitted from the UWS)
// byte 3 lsb..msb
UINT32  gripperState			:1;	  // lsb -- least significant byte
UINT32  rightSideClampstate	 :1;	 
UINT32  leftSideClampState	  :1;	 
UINT32  knifeState					  :1;	 

// 0 = 0x00 1 = 0x10 2 = 0x20 3 = 0x30
UINT32  filmWidthState		  :2;	 

// 0 = 0x00 1 = 0x40 2 = 0x80 3 = 0xC0
UINT32  filmLengthState		 :2;	 // ms 2 bits
} UltimaWrSensors;

typedef struct
{
UINT8							wrEvent;
UINT8							pad0;
WrErrPkgHistoryData	 data[MaxErrorHistoryBuffers];	
} UltimaWrErrorHistory;

typedef struct
{
UINT8					wrEvent;
UINT8					pad0;
WrStatusField	status;
WrErrorField	error;
WrErrorField	history;
UINT8					mode;
UINT8					pad1;
UINT16				  event;  
WrHeaterField	heater;
WrPackageField  package[NumWrapperPositions];
} UltimaWrStatus;

*/


#ifdef __cplusplus
#pragma pack()
#endif

#endif // EVOLWRAPPERMESSAGES_H
