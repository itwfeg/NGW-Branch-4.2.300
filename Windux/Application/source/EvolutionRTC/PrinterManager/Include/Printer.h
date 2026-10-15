#ifndef PRINTER_H
#define PRINTER_H

#include "PAL_platformheader.h" 

typedef enum PrinterDotStatus
{
	DotGood,
	DotBad,
	DotMarginal,
} PrinterDotStatus_t;

typedef enum DotWearHeadStatus{
	ALL_DOTS_GOOD,
	DOTS_MIXED,
	ALL_DOTS_BAD,
	DOT_FAILED
}DotWearHeadStatus_t;

typedef enum PrinterKind
{
	NoPrinter,
	EppPrinter,		//	Supports the EPP2 printer (without the LSMN board)
	EssPrinter,		//	Not supported.
	EscPrinter,		//	Not supported.
	ApcPrinter,		//	Service counter printer
	Epp2Printer,	//	Supports the EPP3 printer (Atmel-based SAM-BA device)
	USBPrinter,
	NetworkPrinter,
	FakePrinter,
	KPCPrinter,		// Support new CAN KPC (Weigher / Printer)
	ARKPrinterOnly, // Supports new CAN printer only
	KPPPrinter,		// Supports new CAN prepack printer
	SSKPCPrinter	// supports new CAN self service printer
}   PrinterKind_t;

typedef enum PrinterType
{
	FirstPrinter,
	SecondPrinter,
	ThirdPrinter,
	MaxPrinterTypes, // needs to be last
	ExtraPrinter_UsedForSelfServeScreenBarcodeLabel,   // PJR-383 - Needed another one, but I didn't want to put before Max... in case that would impact current functionality
	NewMaxPrinterTypes // PJR-383 - New Max created, needs to be last
}   PrinterType_t;

typedef enum LabelWidth 
{
    WideLabel,
    NarrowLabel,
    UFWLabel,
	ExternalLabel,   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
	UnknownLabel
}   LabelWidth_t;

#define UFWPrintHeadSize  640

#define UnknownLabelInches 0.00
#define NarrowLabelInches 1.50
#define WideLabelInches 2.25
#define UFWLabelInches 3.00

#define UnknownLabelMM 0.00
#define NarrowLabelMM (NarrowLabelInches * 25.4)
#define WideLabelMM (WideLabelInches * 25.4)
#define UFWLabelMM (UFWLabelInches * 25.4)

//label type widths in dots
// ESS uses a 56mm print head that can print up to 448 dots (56 * 8dpmm).
// This is the reason why WideLabelWidth is 448 instead of 457 (2.25 in *
// 25.4 * 8).
// EPP uses a 80mm print head that can print up to 640 dots (80 * 8dpmm).
// However a 3" label can only contain 609 dots (3 in * 25.4 * 8) at 8 dots
// per millimeter of resolution. This is the reason why we do not use 640,
// the full width of the print head.
typedef enum LabelWidthDots
{
	UnknownLabelWidth = 0,
	NarrowLabelWidth	= 304,	// 1.50 in.	= 304,	// 1.50 in.
	WideLabelWidth 		= 448,	// 2.25 in.	= 457,	// 2.25 in.
	UFWLabelWidth		= 609	// 3.00 in. = 609	// 3.00 in. 
}   LabelWidthDots_t;

typedef enum LabelLength
{
	UnknownLabelLength,
	Length1_000 = 1000,
	Length1_500 = 1500,
	Length1_750 = 1750,
	Length2_000 = 2000,
	Length2_375 = 2375,
	Length2_500 = 2500,
	Length3_000 = 3000,
	Length3_500 = 3500,
	Length4_000 = 4000,
	Length4_500 = 4500,
	Length5_000 = 5000,
	Length5_500 = 5500,
	Length6_000 = 6000,
	Length6_500 = 6500,
	Length7_000 = 7000,
	Length7_500 = 7500,
	Length8_000 = 8000,
	Length8_500 = 8500,
	Length9_000 = 9000,
	Length9_500 = 9500,
	Length10_000 = 10000
}   LabelLength_t;

typedef enum LabelLengthLines
{
	UnknownLabelLengthLines = 0,
	Length1_000_Lines = 203,
	Length1_500_Lines = 304,
	Length1_750_Lines = 355,
	Length2_000_Lines = 406,
	Length2_375_Lines = 482,
	Length2_500_Lines = 508,
	Length3_000_Lines = 609,
	Length3_500_Lines = 711,
	Length4_000_Lines = 812,
	Length4_500_Lines = 914,
	Length5_000_Lines = 1016,
	Length5_500_Lines = 1118,
	Length6_000_Lines = 1219,
	Length6_500_Lines = 1320,
	Length7_000_Lines = 1422,
	Length7_500_Lines = 1524,
	Length8_000_Lines = 1625,
	Length8_500_Lines = 1727,
	Length9_000_Lines = 1829,
	Length9_500_Lines = 1930,
	Length10_000_Lines = 2032,
	LengthContinuous_Lines = 4999
}   LabelLengthLines_t;

typedef enum PrTestLabelType
{
	PrNoTestLabel,
	PrVoidLabel,
	PrCheckerboard,
	PrHorizontalBars,
	PrVerticalLines,
	PrHighPower,
	PrLabelSensorLabel
}   PrTestLabelType_t;

typedef enum PrTestLabelQuantity
{
	PrOneTestLabel,
	PrStartContinuous,
	PrStopContinuous
}   PrTestLabelQuantity_t;

typedef enum PrConfigType
{
	PrPermanent = 0x0000,
	PrTemporary = 0x0001,
	PrDefault = 0x0002,
	// Bit definition used in the CONFIG message disposition to
	// indicate that weigher calibration is in progress.
	PrCalibInProgress = 0x8000
}   PrConfigType_t;

/* Printer Configuration Status */
typedef enum PrCfgStatus
{
	PrinterConfigValid,
	PrinterConfigError
}   PrCfgStatus_t;

typedef enum MediaSensorType
{
	PrReflective,
	PrShootThrough,
	PrMediaSensorError = 0xFFFF
}   MediaSensorType_t;

typedef enum PrinterHeadUpType
{
/*
	PrinterHeadDown = 0,
	PrinterHeadUpThreshold = 0x7F,
	PrinterHeadUp = 0xFF
*/

	PrinterHeadEngaged = 0,
	PrinterHeadDisengagedThreshold = 0x7F,
	PrinterHeadDisengaged = 0xFF
}   PrinterHeadUpType_t;

typedef enum PrinterStateType
{
	PrinterOff,
	PrinterOn
}   PrinterStateType_t;

typedef enum PrinterCommandOptions
{
	PrNoSecurityLabel,
	PrApplySecurityLabel
}   PrinterCommandOptions_t;

typedef enum PrinterIndirection
{
	PrNoIndirection,
	PrConfigWord0,
	PrConfigWord1,
	PrConfigWord2,
	PrConfigWord3,
	PrConfigWord4,
	PrConfigWord5,
	PrConfigWord6,
	PrConfigWord7,
	PrConfigWord8,
	PrConfigWord9,
	PrConfigWord10,
	PrConfigWord11,
	PrConfigWord12,
	PrConfigWord13,
	PrConfigWord14,
	PrNumRamLocations = 16,
	PrRam0 = 17,
	PrRam1,
	PrRam2,
	PrRam3,
	PrRam4,
	PrRam5,
	PrRam6,
	PrRam7,
	PrRam8,
	PrRam9,
	PrRam10,
	PrRam11,
	PrRam12,
	PrRam13,
	PrRam14,
	PrRam15,
	PrIndirectionEnd = 256  // This enum wants to be 16 bits. So, fake out the compiler!!
}   PrinterIndirection_t;

typedef struct
{
    UINT16	location;
    UINT16	value;
} IndirectDataItem;

typedef enum PrinterCalibrateType
{
	PrCalibratePrinthead,
	PrCalibrateMediaSensor,
	PrCalibrateLabelTaken
}   PrinterCalibrateType_t;

typedef enum LabelOrientation
{
	PortraitLabel,
	LandscapeLabel
}   LabelOrientation_t;

/* These have to do with Printer Commands */
// NOTE:  The Printer code only has room for commands 0 through 9!!!!!
typedef enum PrinterCommandType
{
	FindwoLabelTakenCommand = 1,
	BackupCommand,
	AdvanceCommand,
	PrintCommand,
	EjectCommand,
	SyncCommand,
	FindCommand,
	SizeCommand,
	CutRetractCommand,
	CalibrateCommand,
	VirtualLabelTaken,
	CleaningCommand = 13,				//SOF-5099
	CommandComplete = 0x80,
	FindwoLabelTakenCmdComplete,
	BackupCmdComplete,
	AdvanceCmdComplete,
	PrintCmdComplete,
	EjectCmdComplete,
	SyncCmdComplete,
	FindCmdComplete,
	SizeCmdComplete,
	CutRetractCmdComplete,
	VirtualLabelTakenComplete = 0x8B,
	CleaningCommandComplete = 0x8D,		//SOF-5099
	CalibrateCmdComplete = 0xFE,
	ResetCmdComplete = 0xFF
}   PrinterCommandType_t;

/**
* @enum PrExecuteCode
* @brief The list of printer execution codes
* 
*/
typedef enum _pr_execute_code_ {
    EXECUTE_ENABLE_PRINTER,                 /**<EXECUTE_ENABLE_PRINTER. Code for enabling the printer*/
    EXECUTE_DISABLE_PRINTER,                /**<EXECUTE_DISABLE_PRINTER. Code for disabling the printer*/
    EXECUTE_RESET,                          /**<EXECUTE_RESET. Code for resetting the printer*/
    EXECUTE_PRINTHEAD_ON,                   /**<EXECUTE_PRINTHEAD_ON. Code for turning the print-head voltage on*/
    EXECUTE_PRINTHEAD_OFF,                  /**<EXECUTE_PRINTHEAD_OFF. Code for turning the print-head voltage off*/
    EXECUTE_ENABLE_TAKEUP,                  /**<EXECUTE_ENABLE_TAKEUP. Code for enabling the take-up motor*/
    EXECUTE_DISABLE_TAKEUP,                 /**<EXECUTE_DISABLE_TAKEUP. Code for disabling the take-up motor*/
    EXECUTE_CALIBRATE_MEDIA_SENSOR,         /**<EXECUTE_CALIBRATE_MEDIA_SENSOR. Code for calibrating the media sensor*/
    EXECUTE_CALIBRATE_LABEL_TAKEN_SENSOR,   /**<EXECUTE_CALIBRATE_LABEL_TAKEN_SENSOR. Code for calibrating the label taken sensor*/
    EXECUTE_EJECT_LABEL,                    /**<EXECUTE_EJECT_LABEL. Code for ejecting a label*/
    EXECUTE_ADVANCE_LABEL,                  /**<EXECUTE_ADVANCE_LABEL. Code for advancing the label a little bit*/
    EXECUTE_SIZE_LABEL,                     /**<EXECUTE_SIZE_LABEL. Code for sizing the label stock*/
    EXECUTE_PRINT_LABEL,                    /**<EXECUTE_PRINT_LABEL. Code for signalling to start the print*/
    EXECUTE_FIND_LABEL,                     /**<EXECUTE_FIND_LABEL. Code for finding the next label using the label taken sensor*/
    EXECUTE_FINDWO_LABEL,                   /**<EXECUTE_FINDWO_LABEL. Code for finding the next label without using the label taken sensor*/
    EXECUTE_TEACH_UNSIZED_LABEL,            /**<EXECUTE_TEACH_UNSIZED_LABEL. Code for loading the unsized teach table on the printer*/
    EXECUTE_TEACH_CONTINUOUS_LABEL,         /**<EXECUTE_TEACH_CONTINUOUS_LABEL. Code for loading the continuous label teach table on the printer*/
    EXECUTE_TEACH_CONTINUOUS_STREAM_LABEL,  /**<EXECUTE_TEACH_CONTINUOUS_STREAM_LABEL. Code for loading the continuous streaming teach table on the printer*/
    EXECUTE_TEACH_DIE_CUT_LABEL,            /**<EXECUTE_TEACH_DIE_CUT_LABEL. Code for loading the die-cut label teach table on the printer*/
    EXECUTE_TEACH_DIE_CUT_STREAM_LABEL,     /**<EXECUTE_TEACH_DIE_CUT_STREAM_LABEL. Code for loading the die-cut streaming teach table on the printer*/
    EXECUTE_TEACH_PRINTER_CALIBRATION,      /**<EXECUTE_TEACH_PRINTER_CALIBRATION. Code for loading the calibration teach table on the printer*/
    EXECUTE_TEACH_SPEED_KEY_PRINT,          /**<EXECUTE_TEACH_SPEED_KEY_PRINT. Code for loading the speed key print teach table on the printer*/
} PrExecuteCode;

/**
* @enum PrRequestCode
* @brief The list of printer request codes
* 
*/
typedef enum _pr_request_code_ {
  REQUEST_WAKEUP, /**<REQUEST_WAKEUP. Code for requesting a wakeup message*/
  REQUEST_CONFIGURATION, /**<REQUEST_CONFIGURATION. Code for requesting a configuration message*/
  REQUEST_STATUS, /**<REQUEST_STATUS. Code for requesting a status message*/
  REQUEST_SENSORS, /**<REQUEST_SENSORS. Code for requesting a sensors message*/
  REQUEST_VERSION, /**<REQUEST_VERSION. Code for requesting a printer version message*/
  REQUEST_ERROR, /**<REQUEST_ERROR. Code for requesting an error message*/
} PrRequestCode;

typedef enum PrinterModeType
{
	UnsizedLabel,
	ContinuousLabelHead,
	ContinuousLabelHeel,
	ContinuousStreamHead,
	ContinuousStreamHeel,
	DieCutLabelHead,
	DieCutLabelHeel,
	DieCutStreamHead,
	DieCutStreamHeel,
	SpeedKeyPrint,
	PrinterCalibration,
	BakeryPrint,
	ContinuousLabelHeadCut,
	ContinuousLabelHeelCut,
	ContinuousStreamHeadCut,
	ContinuousStreamHeelCut,
	ContinuousLabelHeadCombined,
	ContinuousLabelHeelCombined,
	DieCutShootHead,
	DieCutShootHeel,
	DieCutShootStreamHead,
	DieCutShootStreamHeel,
	ContinuousShootLabelHeelCut

}   PrinterModeType_t;

/* Command Bits */

/* Sensor Bits */
#define MotorForwardBit		0x01
#define LabelTakenBit		0x02
#define SyncBarBit			0x04
#define SyncBarEdgeBit		0x08
#define WideLabelBit		0x10
#define ThermalOverloadBit	0x20 
#define OutOfMediaBit		0x40 
#define SynchedBit			0x80 

/* Sensor2 Bits */
#define OutOfDataBuffersBit	0x01
#define LowStock			0x02	//SOF-4608
#define OutOfStock			0x04	//SOF-4608

//	Printer Error Bits
//	These constitute errors when received in PrinterStatus!!
#define PrHeadUpBit				0x01
#define PrThermalShutdownBit	0x02
#define PrMediaShutdownBit		0x04
#define PrCassetteMissingBit	0x08
#define PrHardwareErrorBit		0x10

//	Programmable User Bits of StatusInfo
//	These constitute errors when received in PrinterStatus!!
#define MediaJammedBit		0x01
#define LabelMissingBit		0x02
#define OutOfBoundsBit		0x04
#define TakingBit			0x08

#define POUTTOVIN			0x01
#define POUTTOSIN			0x02
#define SOUTTOPIN			0x04

#endif // PRINTER_H
