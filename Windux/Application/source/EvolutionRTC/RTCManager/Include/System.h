#ifndef SYSTEM_H
#define SYSTEM_H

#include "PAL_platformheader.h"

typedef enum SysConfigType
{
	SysPermanent = 0x0000,
	SysTemporary = 0x0001,
	SysDefault = 0x0002,
	SysCalibration = 0x1000
}   SysConfigType_t;

/* System Configuration Status */
typedef enum SysCfgStatus
{
	SystemConfigValid,
	SystemConfigError
}   SysCfgStatus_t;

/* Scale Status operating modes */
typedef enum SystemModeType
{
	SystemOperateMode,
	SystemTestMode
}   SystemModeType_t;

typedef UINT16 DeviceAddress;

#define SystemDevSignatureSize 6
#define SystemPartNumberSize 10

//Alex Zavala 8/21
#define PART_NUMBER_SIZE			10
#define REVISION_SIZE				2
#define SERIAL_NUMBER_SIZE			6		
#define AWS_SERIAL_NUMBER_SIZE		12
#define DATE_SIZE					6

#ifdef __cplusplus
	#pragma pack(1)
#endif

/* System Configuration Structure. */

	/* Generic */
    typedef struct
	{
        UINT8	function_code;
	    UINT8	pad0;
	} SysGenericMsg;

	typedef struct
	{
        UINT8	function_code;
	    UINT8	pad0;
		UINT8	sysCfgStatus;
	} SysWakeupMsg;

	typedef struct
	{
	    UINT8			device_signature[SystemDevSignatureSize];
	    DeviceAddress	station_address;
	    DeviceAddress	controller_address;
	} SysConfigInfo;

	typedef struct
	{
	    UINT8			function_code;
	    UINT8		   	pad0;
	    UINT16  		configType;
	    SysConfigInfo	system_config;
	} SysConfigMsg;

	typedef struct
	{
	    UINT8	 		function_code;
	    UINT8		 	pad0;
	    UINT16			configType;
	    SysConfigInfo	system_config;
	} SysInquireMsg;

	typedef struct
	{
	    UINT16	bit_error;
	    UINT16	address_error;
	    UINT16	rom_checksum;
	    UINT32	flags;
	    UINT8	partno[SystemPartNumberSize];
	    UINT16	system_checksum;
	    UINT32	init_flags;
	} SysStatusInfo;

	typedef struct
	{
	    UINT8			function_code;
	    UINT8 			pad0;
	    SysStatusInfo	system_status;
	} SysStatusMsg;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8 	pad0;
	    UINT8	calib_switch_state;
	    UINT8 	pad1;
	} SysCalibSwitchMsg;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8 	pad0;
	    UINT16	BootCodeMajorRevision;
	    UINT16 	BootCodeMinorRevision;
	    UINT16 	BootStoredChecksum;
	    UINT16 	BootCalculatedChecksum;
	    UINT16 	FLUD1MajorRevision;
	    UINT16 	FLUD1MinorRevision;
	    UINT16 	FLUD1StoredChecksum;
	    UINT16 	FLUD1CalculatedChecksum;
	    UINT16 	FLUD2MajorRevision;
	    UINT16 	FLUD2MinorRevision;
	    UINT16 	FLUD2StoredChecksum;
	    UINT16 	FLUD2CalculatedChecksum;
	    UINT16 	PrinterMajorRevision;
	    UINT16 	PrinterMinorRevision;
	    UINT16 	PrinterStoredChecksum;
	    UINT16 	PrinterCalculatedChecksum;
	    UINT8 	FlashManufacturerID;
	    UINT8 	FlashDeviceID;
	    UINT8 	SelectedFLUD;
	    UINT8 	FLUD1Status;
	    UINT8 	FLUD2Status;
	    UINT8 	BPUpdateFailures;
	    UINT8 	F1F2UpdateFailures;
	    UINT8 	SysConfigStatus;
	} SysFlashStatusMsg;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif //SYSTEM
