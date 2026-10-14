#ifndef WEIGHER_H
#define WEIGHER_H

#include "System.h"

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

#define _Max_MinWeightToPrint_ 9999

// Different Kinds of Weighers
typedef enum WeigherKind
{
	WgNoWeigher,
	WgUltima,
	WgUsbEvol, // Ez-USB Weighing Device
	WgUsbApc,
	WgUsbAWS,   // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	WgCanKpc,
	WgCanPrepack,
}   WeigherKind_t;

typedef enum WeigherType
{
	FirstWeigher,
	SecondWeigher
}   WeigherType_t;

typedef enum WeighMode
{
	WeighMode_Metric,
	WeighMode_Avoir
}   WeighMode_t;

typedef enum WeigherModel
{
	WmNone,
	WmESS,			//	was WmEVOSS
	WmU2000S1,
	WmU2000S3,
	WmUWS,
	WmQuantHC,		//	was WmEVOHC
	WmESC,
	WmEPSA,
	WmHLX1,
	WmEzUSB,		//	Cypress-based Ez-USB weigher for the AWS
	WmAWS,			//	Atmel-based USB weigher for the AWS, AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	WmEPSA2,  		//	Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
	WmAWS2,		  	//	Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
	WmEPSAC2,		//	New USB Compact Label Applier
	WmSASCALE1,		//  SASCALE-1 for SASCALE
	WmSASCALE2,		//  SASCALE-2 for SASCALE
	WmKPCWeigher,   //  Kinetis based CAN Weigher for HT series scales and NGS, NGW Wrappers
	endWeigherModel	// KEEP THIS LAST IN THE LIST
}   WeigherModel_t;

/* Weigh Configuration Status */
typedef enum WgCfgStatus
{
	WeigherConfigValid,
	WeigherConfigError
}   WgCfgStatus_t;

typedef enum LoadCellId
{
	LoadCell15kg,
	LoadCell20kg,
	LoadCell30kg,
	LoadCell45kg,
	LoadCell150kg,
	LoadCell200kg
}   LoadCellId_t;

/* Scale Status operating modes */
typedef enum WgOperatingMode
{
	ScalePrepackMode,
	ScaleWeightTrackingMode,
	ScaleDumbMode,
	ScaleManualMode
}   WgOperatingMode_t;

/* Configuration Filter Speed Definitions. */
typedef enum WgFilterSpeed
{
	NormalFilterSpeed,
	SlowFilterSpeed,
	FastFilterSpeed
}   WgFilterSpeed_t;

typedef enum WgConfigType
{
	WgPermanent = 0x0000,
	WgTemporary = 0x0001,
	WgDefault = 0x0002,
	// Bit definition used in the CONFIG message disposition to
	// indicate that weigher calibration is in progress.
	WgCalibInProgress = 0x8000
}   WgConfigType_t;

typedef enum WgDualRangeDirection
{
	DualRangeUp,
	DualRangeDown
}   WgDualRangeDirection_t;

#define ntep_ccSize 8

/* Weigher Configuration Block */
/* Configuration */
typedef struct _ultimaWgCfg
{
	UINT8	  device_signature[SystemDevSignatureSize];
	UINT32	  center_of_maintenance_zone;
	UINT32	  scale_factor;
	UINT32	  max_weight;					/* in calibrated counts */
	UINT32	  gain_factor;
	UINT32	  zero_reference;
	UINT32	  calibration_reference;
	UINT16	  rate_option_time;				/* in seconds */
	UINT16	  auto_zero_maintenance_limit;
	UINT16	  prepack_motion_count;   
	UINT16	  initialize_zero_time;			/* in seconds */
	UINT16	  auto_zero_time;				/* in seconds */
	UINT16	  rate_option_limit;
	UINT16	  zero_reference_limit;   
	UINT16	  calibration_reference_limit;   
	UINT16	  small_motion_limit;
	UINT16	  large_motion_limit;
	UINT16	  large_motion_count;
	UINT16	  small_motion_count;
	UINT16	  no_motion_count;
	UINT16	  zero_tracking_option;
	UINT16	  negative_weight_option;
	UINT16	  powerup_maintenance_zone_limit;
	UINT16	  operate_maintenance_zone_limit;
	UINT8	  filter_speed;
	UINT8	  reference_voltage;
	UINT32	  last_calibration_date;
	UINT16	  number_of_calibrations;
	UINT32	  last_configuration_date;
	UINT16	  number_of_configurations;
	UINT8	  ntep_cc[ntep_ccSize];
#if defined ( __linux__ )
	_ultimaWgCfg() :
		scale_factor(0),
		max_weight(0),
		gain_factor(0),
		zero_reference(0),
		calibration_reference(0),
		rate_option_time(0),
		auto_zero_maintenance_limit(0),
		prepack_motion_count(0),   
		initialize_zero_time(0),
		auto_zero_time(0),	 
		rate_option_limit(0),
		zero_reference_limit(0),   
		calibration_reference_limit(0),   
		small_motion_limit(0),
		large_motion_limit(0),
		large_motion_count(0),
		small_motion_count(0),
		no_motion_count(0),
		zero_tracking_option(0),
		negative_weight_option(0),
		powerup_maintenance_zone_limit(0),
		operate_maintenance_zone_limit(0),
		filter_speed(0),
		reference_voltage(0),
		last_calibration_date(0),
		number_of_calibrations(0),
		last_configuration_date(0),
		number_of_configurations(0)
	{
		for (int i=0;i<SystemDevSignatureSize;i++)
			device_signature[i] = 0;
		for (int i=0;i<ntep_ccSize;i++)
			ntep_cc[i] = 0;
	}
#endif	  // __linux__
} UltimaWgCfg;

/* Weigher Configuration Block */
/* EZ-USB (Usb): First generation USB weigher */
/* Configuration */
typedef struct _usbWgCfg
{
	UltimaWgCfg	ultimaWgCfg;
	UINT32		calibration_gain_coefficient;
	UINT32		calibration_offset_coefficient;
	UINT8		weigher_type;
	UINT8		load_cell_id;
#if defined ( __linux__ )
	_usbWgCfg():
		calibration_gain_coefficient(0),
		calibration_offset_coefficient(0),
		weigher_type(0),
		load_cell_id(0) {}
#endif
} UsbWgCfg;

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
// Dual-Range Support (haberrc)
/* Atmel (Usb2): Second generation USB weigher */
/* Configuration */
typedef struct _usb2WgCfg
{
	UltimaWgCfg	ultimaWgCfg;
	UINT32		calibration_gain_coefficient;
	UINT32		calibration_offset_coefficient;
	UINT8		weigher_type;
	UINT8		load_cell_id;
	UINT8		flags;
	UINT32		min_weight_to_print;
	UINT8		weigher_model;
#if defined ( __linux__ )
	_usb2WgCfg():
		calibration_gain_coefficient(0),
		calibration_offset_coefficient(0),
		weigher_type(0),
		load_cell_id(0),
		flags(0),
		min_weight_to_print(0),
		weigher_model(0) {}
#endif
} Usb2WgCfg;

/* Generic */
typedef struct _ultimaWgGeneric
{
	UINT8	   function_code;
	UINT8	   pad0;
#if defined ( __linux__ )
	_ultimaWgGeneric(): function_code(0), pad0(0) {}
#endif
} UltimaWgGeneric;

/* Wakeup */
typedef struct _ultimaWgWakeup
{
	UINT8	   function_code;
	UINT8	   pad0;
	UINT8	   wgCfgStatus;
#if defined ( __linux__ )
	_ultimaWgWakeup(): function_code(0), pad0(0), wgCfgStatus(0) {}
#endif
} UltimaWgWakeup;

/* Status */
typedef struct _ultimaWgStatus
{
	UINT8	function_code;
	UINT8	pad0;
	UINT16	status;
	UINT32	counts;
	UINT32	raw_counts;
#if defined ( __linux__ )
	_ultimaWgStatus(): function_code(0), pad0(0), status(0), counts(0), raw_counts(0) {}
#endif
} UltimaWgStatus;

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
// Reference: Proposed QX2 Counts Processing document. Date: 1/29/2008 (Tom Fink)
/* Status */
typedef struct _usb2WgStatus
{
	UINT8	function_code;
	UINT8	pad0;
	UINT16	status;
	UINT32	counts;
	UINT32	raw_counts;
	UINT32	filtered_uncalibrated_counts;
	UINT32	AD_counts;
#if defined ( __linux__ )
	_usb2WgStatus(): 
		function_code(0), 
		pad0(0), 
		status(0),
		counts(0),
		raw_counts(0),
		filtered_uncalibrated_counts(0),
		AD_counts(0) {}
#endif
} Usb2WgStatus;

typedef struct
{
	UINT8	function_code;
	UINT8	pad0;
	UINT8	saved;
}Usb2WgFactoryDefaultsCmplt;

typedef struct
{
	UINT8	function_code;
	UINT8	pad0;
	UINT8	totalNumPages;
	UINT8 	totalRecordsPerPage;
	UINT8 	currentPage;
	UINT8 	pad1;
	UINT16 	totalFreeRecords;
	UINT16 	totalRecordedEvents;
	UINT16	totalCalibrationRecords;
	UINT16	totalConfigurationRecords;
	   /* sealable parameter counters (requirement 8.24c, 8.24d ) */
	UINT16 	gainCoefficientCounter;         /* Total number of times gain coefficent has changed */
	UINT16  offsetCoefficientCounter;       /* Total number of times offset coefficent has changed */
	UINT16  gainFactorCounter;              /* Total number of times gain factor has changed */
	UINT16  centerOfMainCounter;            /* Total number of times center of maintenance has changed */
	UINT16  zeroReferenceCounter;           /* Total number of times zero reference has changed */
	UINT16  weigherModelCounter;            /* Total number of times weigher model has changed */
    UINT16  minWeightPrintCounter;          /* Total number of times min weight to print has changed */
    UINT16  maxWeightCounter;               /* Total number of times max weight has changed */
    UINT16  filterSpeedCounter;             /* Total number of times filter speed has changed */
    UINT16  divisionSizeCounter;            /* Total number of times division size has changed */
    UINT16  weigherModeCounter;				/* Total number of times weigh mode has changed */
} Usb2WgCat3Statistics;

typedef struct
{
	UINT8	function_code;
    UINT8   pad0;
    UINT8   pageNumber;			/* page number of the records */
    UINT8   index;          	/* record index of the first record */
    UINT8   records[320];
}Usb2WgCat3Records;

typedef struct
{
	UINT8	function_code;
    UINT8   pad0;
    UINT8   event;
    UINT8   pad1;
    UINT8   parameterId;
    UINT8   pad2;
    UINT8   eventType;
    UINT8 	pad3;
    UINT32  epochTime;
    UINT32  newValue;
    UINT32  oldValue;
}Usb2WgCat3WriteRecord;

typedef struct
{
	UINT8       function_code;
	UINT8       pad0;
	UINT8       pad1;
	UINT8       pad2;
	UINT32      epochTime;
}Usb2WgCat3DateTime;


typedef struct
{
	UINT8       function_code;
    UINT8       pad0;
    UINT8       saved;          /* record saved to Cat3 audit trail */
}Usb2WgCat3RecordStatus;

typedef struct
{
	UINT8       function_code;
	UINT8       pad0;
	UINT8       pageNumber;		/* which page to read records from */
	UINT8       index;          /* record index 0 - 128 */
}Usb2WgReqCat3Page;

#if 0

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    unsigned char       totalPages;                     /* Total number of Cat3 audit pages */
    unsigned char       totalRecordsPerPage;            /* Total number of records per page */
    unsigned char       currentPage;                    /* Audit managers current working page */
    unsigned short      totalFreeRecords;               /* Total number of free records in the serial flash */
    unsigned short      totalRecordedEvents;            /* Total number of CAT3 events saved */
    unsigned short      totalCalibrationRecords;        /* Total number of saved calibration events */
    unsigned short      totalConfigurationRecords;      /* Total number of saved configuration events */
    /* sealable parameter counters (requirement 8.24c, 8.24d ) */
    unsigned short      gainCoefficientCounter;         /* Total number of times gain coefficent has changed */
    unsigned short      offsetCoefficientCounter;       /* Total number of times offset coefficent has changed */
    unsigned short      gainFactorCounter;              /* Total number of times gain factor has changed */
    unsigned short      centerOfMainCounter;            /* Total number of times center of maintenance has changed */
    unsigned short      zeroReferenceCounter;           /* Total number of times zero reference has changed */
    unsigned short      weigherModelCounter;            /* Total number of times weigher model has changed */
    unsigned short      minWeightPrintCounter;          /* Total number of times min weight to print has changed */
    unsigned short      maxWeightCounter;               /* Total number of times max weight has changed */
    unsigned short      filterSpeedCounter;             /* Total number of times filter speed has changed */
    unsigned short      divisionSizeCounter;            /* Total number of times division size has changed */
}WgCat3Statistics;

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    unsigned char       pageNumber;
    unsigned char       index;          /* record index 0 - 128 */
}WgReqCat3Page;

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    unsigned char       pageNumber;
    unsigned char       index;          /* record index of the first record */
    unsigned char       records[320];
}WgCat3Records;

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    unsigned char       event;
    unsigned char       pad1;
    unsigned char       parameterId;
    unsigned char       pad2;
    unsigned char       eventType;
    unsigned long       epochTime;
    unsigned long       newValue;
    unsigned long       oldValue;
}WgCat3WriteRecord;

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    //unsigned char       date[10];       /* same size as in cat3Audit.h */
    //unsigned char       time[7];        /* same size as in cat3Audit.h */
    unsigned long       epochTime;
}WgCat3DateTime;

typedef struct
{
    WgMessageCode       function_code;
    unsigned char       pad0;
    BOOL                saved;          /* record saved to Cat3 audit trail */
}WgCat3RecordStatus;

#endif

/* Inquire */
typedef struct _ultimaWgInquire
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	UltimaWgCfg	config;
#if defined ( __linux__ )
	_ultimaWgInquire(): function_code(0), pad0(0), configType(0) {}
#endif
} UltimaWgInquire;

/* Config */
typedef struct _ultimaWgConfig
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	UltimaWgCfg	config;
#if defined ( __linux__ )
	_ultimaWgConfig(): function_code(0), pad0(0), configType(0) {}
#endif
} UltimaWgConfig;

/* Inquire */
typedef struct _usbWgInquire
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	UsbWgCfg	config;
#if defined ( __linux__ )
	_usbWgInquire(): function_code(0), pad0(0), configType(0) {}
#endif
} UsbWgInquire;

/* Config */
typedef struct _usbWgConfig
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	UsbWgCfg	config;
#if defined ( __linux__ )
	_usbWgConfig(): function_code(0), pad0(0), configType(0) {}
#endif
} UsbWgConfig;

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
// Dual-Range Support (haberrc)
/* Inquire */
typedef struct _usb2WgInquire
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	Usb2WgCfg	config;
#if defined ( __linux__ )
	_usb2WgInquire(): function_code(0), pad0(0), configType(0) {}
#endif
} Usb2WgInquire;

/* Config */
typedef struct _usb2WgConfig
{
	UINT8		function_code;
	UINT8		pad0;
	UINT16		configType;
	Usb2WgCfg	config;
#if defined ( __linux__ )
	_usb2WgConfig(): function_code(0), pad0(0), configType(0) {}
#endif
} Usb2WgConfig;

/* Mode */
typedef struct _ultimaWgMode
{
	UINT8	   function_code;
	UINT8	   pad0;
	UINT8	   mode;
	UINT8	   pad1;
#if defined ( __linux__ )
	_ultimaWgMode():function_code(0), pad0(0), mode(0), pad1(0){}
#endif
} UltimaWgMode;

/* A/D control register */
typedef struct _ultimaWgControl
{
	UINT8	   function_code;
	UINT8	   pad0;
	UINT8	   control;
	UINT8	   pad1;
#if defined ( __linux__ )
	_ultimaWgControl():function_code(0), pad0(0), control(0), pad1(0){}
#endif
} UltimaWgControl;

typedef struct _usbWgSysStatus
{
	UINT8 function_code;
	UINT8 pad0;
	UINT32 flags;
	UINT32 init_flags;
	UINT32 serial_number;
	UINT16 software_major_revision;
	UINT16 software_minor_revision;
	UINT8 hardware_major_revision;  // Device ID
	UINT8 hardware_minor_revision;			  
#if defined ( __linux__ )
	_usbWgSysStatus():
		function_code(0),
		pad0(0),
		flags(0),
		init_flags(0),
		serial_number(0),
		software_major_revision(0),
		software_minor_revision(0),
		hardware_major_revision(0),
		hardware_minor_revision(0) {}
#endif
} UsbWgSysStatus;

// Fix PT #3576, AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
/* Some structure members were removed during EZ-USB weigher development for
   messages sent to the Evolution. These changes, made by myself, while working
   with S. Manger occurred on 6/17/2004. The older, original structure will now
   be restored and utilized with the new (Atmel-based) weigher. (haberrc)
   */   

#define PART_NO_SIZE 10

typedef struct _usb2WgSysStatus
{
	UINT8 function_code;
	UINT8 pad0;
	UINT16 bit_error;
	UINT16 address_error;
	UINT16 rom_checksum;
	UINT32 flags;
	UINT8 partno[PART_NO_SIZE];
	UINT16 system_checksum;
	UINT32 init_flags;
	UINT16 software_major_revision;
	UINT16 software_minor_revision;
	UINT8 hardware_major_revision;  // Device ID
	UINT8 hardware_minor_revision;			  
	UINT32 weigher_serial_num;   
	UINT32 DebugFlags;	 
#if defined ( __linux__ )
	_usb2WgSysStatus():
		function_code(0),
		pad0(0),
		bit_error(0),
		address_error(0),
		rom_checksum(0),
		flags(0),
		system_checksum(0),
		init_flags(0),
		software_major_revision(0),
		software_minor_revision(0),
		hardware_major_revision(0),
		hardware_minor_revision(0),
		weigher_serial_num(0),
		DebugFlags(0)
	{
		for (int i=0;i<PART_NO_SIZE;i++)
			partno[i]=0;
	}
#endif
} Usb2WgSysStatus;


typedef enum
{
	CALIBRATION,
	CONFIGURATION,
	UPGRADE
}AuditEvent;

/*configuration fields that can change */
typedef enum
{
	CENTER_OF_MAINTENANCE_ZONE,
	SCALE_FACTOR,
	MAX_WEIGHT,
	GAIN_FACTOR,
	ZERO_REFERENCE,
	CAL_GAIN_COEFFICIENT,
	CAL_OFFSET_COEFFICIENT,
	FILTER_SPEED,
	MIN_WEIGHT_PRINT,
	WEIGHER_MODEL,
	DIVISION_SIZE,
	SOFTWARE_UPDATE,
	GRAVITY_CONSTANT,
	FRACTIONAL_PRICING,
	POUNDS_FOR_MULTIPLIER,
	SERVICE_SWITCH_PRESSED,
	WEIGHER_MODE,
	DECIMAL_SEPARATOR,		//17
	MONETARY_RULE,			//18
	TOTAL_PRICE_DIGITS,		//19
	UNIT_PRICE_DIGITS,		//20
	ROUNDING_FACTOR,		//21
	ROUNDING_METHOD,		//22
	PRIMARY_MONETARY_SYMBOL,//23
	PRIMARY_MONETARY_DECIMAL_DIGITS,	//24
	SECONDARY_MONETARY_SYMBOL,			//25
	SECONDARY_MONETARY_DECIMAL_DIGITS	//26

}SealableParamID;

typedef enum
{
	LOCAL_EVENT,
	REMOTE_EVENT
}AuditType;

typedef struct
{
	unsigned char   pad0;
	unsigned char   event;
	unsigned char   parameterId;
	unsigned char 	pad1;
	unsigned long   epochTime;
	unsigned char   eventType;
	unsigned char	unused[8];
	unsigned long   oldParamValue;
	unsigned long   newParamValue;
	unsigned long   checksum;
}Cat3AuditRecord;

#define CAT3_AUDIT_RECORDS_PER_MESSAGE 10
#define CAT3_MAX_VIEWABLE_AUDIT_RECORDS 1000


#ifdef __cplusplus
#pragma pack()
#endif

#endif //WEIGHER_H
