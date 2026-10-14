#ifndef WRAPPER_H
#define WRAPPER_H

#include "System.h"

	#define NumberPackCodes				UndefinedPackCode
	#define PackCodeDescriptionLength	25
	#define MaxErrorHistoryBuffers		10 /* size of ring buffer that holds err & pkg history */

	#define MarkPosition 17

	#define CameraMaxImageSize		128
	#define MaxCameraImagePerTrans	64

#ifdef __cplusplus
	#pragma pack(1)
#endif

	typedef enum WrapperKind
	{
		WrNoWrapper,
		WrUltimaWrapper,
		WrEvolWrapper,
		WrAWS3Wrapper,
		WrNGW1Wrapper
	}   WrapperKind_t;

	typedef enum WrConfigType
	{
		WrPermanent = 0x0000,
		WrTemporary = 0x0001,
		WrDefault = 0x0002,
		// Bit definition used in the CONFIG message disposition to
		// indicate that weigher calibration is in progress.
		WrCalibInProgress = 0x8000
	}   WrConfigType_t;

	/* Printer Configuration Status */
	typedef enum WrCfgStatus
	{
		WrapperConfigValid,
		WrapperConfigError
	}   WrCfgStatus_t;

	typedef enum WrFilmTension
	{
		LowFilmTension,
		MediumFilmTension,
		HighFilmTension,
		NoneSelected
	}   WrFilmTension_t;

	typedef enum WrFilmWidthSelection
	{
		UpperRoll,
		LowerRoll,
		Automatic,
		NoFilmWidthSelected
	}   WrFilmWidthSelection_t;

	typedef enum WrPackLength
	{
	    ShortPackage,
	    MediumPackage,
	    LongPackage,
	    FilmLengthIdle,
	    AutoLengthSelect
	}   WrPackLength_t;

	typedef enum WrPackWidth
	{
	    NarrowPackage,
	    WidePackage,
	    FilmWidthIdle,
	    AutoWidthSelect
	}   WrPackWidth_t;

	typedef struct
	{
	    UINT16	minLength;
	    UINT16	maxLength;
	    UINT16	minWidth;
	    UINT16	maxWidth;
	} WrPackSize;

	typedef enum WrGripperPwm
	{
	    Pwm100Gripper,
	    Pwm074Gripper,
	    Pwm055Gripper,
	    Pwm038Gripper,
	    IgnoreGripperPwm 
	}   WrGripperPwm_t;

	typedef struct
	{
		UINT8 min;
		UINT8 value;
		UINT8 max;
		UINT8 pad0;
	} WrTime;

	typedef struct
	{
	    WrTime     on;
	    WrTime     off;
	} WrTiming;

	typedef struct
	{
	    WrTime	setback0;
	    UINT8	pwm0;
		UINT8	pad0;
	    WrTime	setback1;
	    UINT8	pwm1;
		UINT8	pad1;
	    WrTime	setback2;
	    UINT8	pwm2;
		UINT8	pad2;
	    WrTime	setback3;
	    UINT8	pwm3;
		UINT8	pad3;
	} WrGripperTiming;

	typedef enum WrPackCode
	{
	    PackCode0,    /* Wrapper Default   */
	    PackCode1,    /* Wrapper Default   */
	    PackCode2,    /* Wrapper Default   */
	    PackCode3,    /* Wrapper Default   */ 
	    PackCode4,    /* Wrapper Default   */
	    PackCode5,    /* Wrapper Default   */
	    PackCode6,    /* User Programmable */    
	    PackCode7,    /* User Programmable */ 
	    PackCode8,    /* User Programmable */ 
	    PackCode9,    /* User Programmable */ 
	    PackCode10,   /* User Programmable */ 
	    PackCode11,   /* User Programmable */ 
	    UndefinedPackCode    
	}   WrPackCode_t;

	typedef struct
	{
		UINT16	undefined_0	:6;
		UINT16	booster		:1;
		UINT16	slideBlock	:1;
		UINT16	undefined_1	:8;
	} WrPackBits;

	typedef struct
	{
	    UINT8		description[PackCodeDescriptionLength]; 
	    UINT8		packCode;
	    UINT8		packWidth;
	    UINT8		packLength;
	    WrPackSize	packSize;
	    WrTiming	sideClamp; 
	    WrTiming	gripper;    
	    WrTiming	knife;  
	    WrPackBits	options;
		UINT32		packageTotal;
	    UINT32		jamTotal;
	} WrPackCodeInfo;

	typedef struct
	{
	    UINT8			deviceSignature[SystemDevSignatureSize];
	    UINT8			pauseTimeout;  
	    UINT8			idleTimeout;   
	    UINT8			filmTension;   
	    UINT8			packWidth;    
		UINT32			packageGrandTotal;    
	    UINT32			wrapperCycleTotal;    
		UINT16			elevatorCollapseTotal;
	    UINT16			intakeJamTotal;
	    UINT16			lengthJamTotal;
	    UINT16			widthJamTotal;
		UINT16			coverOpenTotal;
	    UINT16			sealerBeltOpenTotal;
	    UINT16			packageTooTallTotal;
		UINT16			pusherHomeErrorTotal;
	    UINT16			filmPullErrorTotal;
		UINT16			heater_enable			:1;
		UINT16			narrow_film_test_enable	:1;
		UINT16			wide_film_test_enable	:1;
		UINT16			celsius_enable			:1;
		UINT16			reserved		  		:12;
		UINT16			heaterSetting; 
		UINT16			packageLengthAdjustment; 
	    UINT16			packageWidthAdjustment; 
	   	UINT32			errorTotalsDate; 
	   	UINT32			packCodeTotalsDate; 
	    WrTiming		heartbeat;
	    WrGripperTiming	gripper;    
		WrPackCodeInfo	packCode[NumberPackCodes];
	} UltimaWrCfg;

	typedef enum WrOpMode
	{
	    WrOperateMode,
	    WrServiceMode,
		WrTestMode,
		WrMfgMode,
		WrContinuousMode,
		WrBoardMfgMode
	}   WrOpMode_t;

	typedef enum WrPackagePosition
	{
	    IntakePosition,      
	    ElevatorPosition,   
	    SealerBeltPosition, 
	    DischargePosition,  
	    NumWrapperPositions      
	}   WrPackagePosition_t;

// should be fixed
	typedef struct
	{
		UINT32	booster			:1;
		UINT32	slideBlock		:1;
		UINT32	intakeMode		:1;
		UINT32	quickDelay		:1;
		UINT32	slowDelay		:1;
		UINT32	ghostPackage	:1;
		UINT32	pullFilm		:1;
		UINT32	present			:1;

		UINT32	undefined		:8;

		UINT32	packWidth		:2;
		UINT32	packLength		:2;
		UINT32	packCode		:4;

		UINT32	packageId		:8;

		UINT16	length;
		UINT16	width;				  
	} WrPackageBits;

	typedef union
	{
	    WrPackageBits	bits;
	    UINT32			field[2];  
	} WrPackageField;

	typedef struct
	{
	    UINT32	temperature	: 15;
		UINT32	celsius		: 1 ;

		UINT32	setting		: 15;
		UINT32	state		: 1 ;
	} WrHeaterBits;

	typedef union
	{
	    WrHeaterBits	bits;
	    UINT32			field;
	} WrHeaterField;

	typedef enum WrapperState
	{
		WrSwitching,
		WrIdle,
		WrPaused,
		WrRunning,
		WrDisabled,
		WrBoardTest,
		WrStandBy,
		WrUnknown
	}   WrapperState_t;

	typedef enum WrapperEvent
	{
	    WrapperCount000Event,
	    WrapperCount255Event = 255,
	    WrapperUndefinedEvent,
	    NumWrapperEvents
	}   WrapperEvent_t;

	typedef enum WrErrorCode
	{
		WrNoErrorCode,
		WrPackageTooLarge,
		WrHeaterFailure,
		WrEepWriteFailure,
		WrTooTallInterlock,
		WrFilmDoorInterlock,
		WrHandCrankDoorInterlock,
		WrScalePlatterInterlock,
		WrElevatorInterlock,
		WrSealerBeltInterlock,
		WrGeneralFault2,
		WrGeneralFault1,
		WrLeftCanopyInterlock,
		WrRightCanopyInterlock,
		WrFrontCoverInterlock,
		WrLeftSideCoverInterlock,
		WrRightSideCoverInterlock,
		WrWideFilmSupplyFailure,
		WrNarrowFilmSupplyFailure,
		WrPackageJammed,
		WrMainMotorStalled,
		WrIntakeHomeFailure,
		WrIntakeWidthFailure,
		WrIntakeLengthFailure,
		WrIntakeMotorJammed,
		WrIntakePackagePresent,
		WrIntakeOutOfSync,
		WrStopSwitchInterlock
	}   WrErrorCode_t;

// should be fixed
	typedef struct
	{
		UINT32   packageTooLarge			:1;
		UINT32   heaterFailure				:1;
		UINT32   eepWriteFailure			:1;
		UINT32   undefined					:5;

		UINT32   tooTallInterlock			:1;
		UINT32   filmDoorInterlock			:1;
		UINT32   handCrankDoorInterlock		:1; 
		UINT32   scalePlatterInterlock		:1;
		UINT32   elevatorInterlock			:1; 
		UINT32   sealerBeltInterlock		:1;
		UINT32   generalFault2				:1;
		UINT32   generalFault1				:1;

		UINT32   packageJammed				:1;
		UINT32   narrowFilmSupplyFailure	:1;
		UINT32   wideFilmSupplyFailure		:1;
		UINT32   rightSideCoverInterlock	:1;
		UINT32   leftSideCoverInterlock		:1;
		UINT32   frontCoverInterlock		:1;
		UINT32   rightCanopyInterlock		:1;
		UINT32   leftCanopyInterlock		:1;

		UINT32   stopSwitchInterlock		:1;
		UINT32   intakeOutOfSync			:1;
		UINT32   intakePackagePresent		:1;
		UINT32   intakeMotorJammed			:1;
		UINT32   intakeLengthFailure		:1;
		UINT32   intakeWidthFailure			:1;
		UINT32   intakeHomeFailure			:1;
		UINT32   mainMotorStalled			:1;
	} WrErrorBits;

	typedef union 
	{
	    WrErrorBits	bits;
	    UINT32		field;  
	} WrErrorField;

	typedef struct
	{
	    WrErrorField	error;
	    WrPackageField	package;
		UINT32			index;
	} WrErrPkgHistoryData;

	typedef struct
	{
	    UINT32	state                   :4;
	    UINT32	film_tension            :3;
	    UINT32	pack_width              :2;
	    UINT32	limp_home               :1;
	    UINT32	main_motor              :1;
	    UINT32	main_power              :1;
	    UINT32	open_window             :1;
	    UINT32	rate_analysis           :1;
	    UINT32	pusher_home             :1;
	    UINT32	report_status           :1;
	    UINT32	rate                    :16;
	} WrStatusBits;

	typedef union
	{
	    WrStatusBits	bits;
	    UINT32			field;  
	} WrStatusField;

	typedef struct
	{
	    UINT16	rollWidth;
	    UINT16	rollDiameter;
	    UINT16	remainingOnRoll;
	} WrFilmWidthDimensions;


	typedef enum
	{
		wrSlaveBootInit,
		wrSlaveBootReady,
		wrSlaveBootFault,
		wrSlaveBootDone
	}WrSlaveLoaderState;

	typedef enum
	{
		wrSlaveNone,
		wrSlaveTimeout,
		wrSlaveWriteFailure,
		wrSlaveCheckSum,
		wrSlaveUnknownRecordType,
		wrSlaveEraseFailureBank1,
		wrSlaveEraseFailureBank2,
		wrSlaveComponentNotSet,
		wrSlaveNodeImageMismatch,
		wrSlaveNodeIdError
	}WrSlaveLoaderFaults;

	typedef enum
	{
		wrSlaveNVData,
		wrSlaveBootLoader,
		wrSlaveProgramFlash
	}WrSlaveSoftwareComponent;

	typedef enum 
	{
		wrApp,
		wrData,
		wrBoot,
		wrAppData,
		wrAppBoot,
		wrDataBoot,
		wrAll	
	}WrComponentUpdate;

	typedef enum
	{
		RightCamera,
		LeftCamera
	} WrapperCamera;

	typedef enum
	{
		DigitalImage,
		AnalogImage	
	} WrCameraImageType;

	typedef enum
	{
		AutomaticOperation,
		ManualOperation	
	} WrOperation;

	typedef enum
	{
        KnifeUpEvent,
        KnifeDownEvent,
        GripperNarrowCloseEvent,
        GripperNarrowOpenEvent,
        GripperWideLeftCloseEvent,
        GripperWideLeftOpenEvent,
        GripperWideRightCloseEvent,
        GripperWideRightOpenEvent,
        SideClampLeftCloseEvent,
        SideClampLeftOpenEvent,
        SideClampRightCloseEvent,
        SideClampRightOpenEvent,
        WandUpEvent,
        WandDownEvent,
        FilmSelectorUpEvent,
        FilmSelectorDownEvent,
        TankPressureReadingEvent,
        RegulatedPressureReadingEvent,
        ApplierVacuumReadingEvent,
        ApplierHandHomeEvent,
        SystemPressureDropReadingEvent,
        UpperFilmRollEncoderReadingEvent,
        LowerFilmRollEncoderReadingEvent,
        MainACVoltageReadingEvent,
		// SOF-3838 Support second label applier
 		SecondWandUpEvent,
		SecondWandDownEvent,
		SecondApplierHandHomeEvent,
		SecondApplierVacuumReadingEvent
	}WrEventCaptureType;
	
	typedef enum
	{
		NoAddOn,
		SecondPrinterApplier		
	}WrHardwareAddOn;
	



#ifdef __cplusplus
	#pragma pack()
#endif

#endif // WRAPPER_H
