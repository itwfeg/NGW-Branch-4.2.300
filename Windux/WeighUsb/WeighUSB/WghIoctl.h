/*
*   Copyright (c) 2000 Microsoft Corporation
*
* USB device driver for Hobart Weighing system devices.
* Definitions for device I/O Control support.
*/

#ifndef _USB_USER_H
#define _USB_USER_H

#include <initguid.h>

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef WORD
#define WORD unsigned int
#endif

#define DEVICE_SIG_NAME_LEN 6

// {004965C5-EF0A-4779-B42A-BA8AD7C98E60}
DEFINE_GUID(GUID_CLASS_WEIGHING_USB,
0x4965c5, 0xef0a, 0x4779, 0xb4, 0x2a, 0xba, 0x8a, 0xd7, 0xc9, 0x8e, 0x60);

/* Weigher Status Bit Definitions */

#define WEIGHER_DISABLED                0x0800
#define CALIBRATION_CHECK_FAILED        0x0400
#define WEIGHER_EEP_READ_FAILED         0x0200
#define UNDER_RANGE                     0x0100
#define NEW_ZERO                        0x0080
#define SCALE_WARMUP                    0x0040
#define ZERO_OUT_OF_MAINTENANCE_ZONE    0x0020
#define OVER_GROSS_WEIGHT_LIMIT         0x0010
#define WITHIN_QUARTER_GRAD_OF_ZERO     0x0008
#define NEGATIVE_GROSS_WEIGHT           0x0004
#define LARGE_MOTION                    0x0002
#define SMALL_MOTION                    0x0001

/* Message Definitions */

enum wg_message_code { WG_WAKEUP,
                       WG_CONFIG,
                       WG_INQUIRE,
                       WG_MODE,
                       WG_RESET,
                       WG_STATUS,
                       WG_ENABLE,
                       WG_DISABLE,
                       WG_REZERO,
                       WG_CALIBRATE,
                       WG_CHECK };

typedef enum wg_message_code WgMessageCode;

#define USB_IOCTL_INDEX                 0x0000

#define IOCTL_USB_GET_CONFIG_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     USB_IOCTL_INDEX,     \
                                                     METHOD_BUFFERED,     \
                                                     FILE_ANY_ACCESS)

#define IOCTL_USB_RESET_DEVICE          CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     USB_IOCTL_INDEX + 1, \
                                                     METHOD_BUFFERED,     \
                                                     FILE_ANY_ACCESS)

#define IOCTL_USB_RESET_PIPE            CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     USB_IOCTL_INDEX + 2, \
                                                     METHOD_BUFFERED,     \
                                                     FILE_ANY_ACCESS)

#define IOCTL_USB_GET_DEVICE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     USB_IOCTL_INDEX + 3, \
                                                     METHOD_BUFFERED,     \
                                                     FILE_ANY_ACCESS)

struct wg_generic
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_generic WgGeneric;


/* WakeUp */

struct wg_wakeup
{
    UCHAR       function_code;
    UCHAR       pad0;
};
typedef struct wg_wakeup WgWakeup;


/* Config */

struct wg_configuration
{
    UCHAR       device_signature [DEVICE_SIG_NAME_LEN];
    DWORD       center_of_maintenance_zone;
    DWORD       scale_factor;
    DWORD       max_weight;
    DWORD       gain_factor;
    DWORD       initialize_zero_time;
    DWORD       auto_zero_maintenance_time;
    DWORD       rate_option_limit;
    DWORD       small_motion_limit;
    DWORD       large_motion_limit;
    DWORD       large_motion_count;
    DWORD       zero_tracking_option;
    DWORD       negative_weight_option;
};
typedef struct wg_configuration WgConfiguration;

struct wg_config
{
    UCHAR           function_code;
    UCHAR           pad0;
    DWORD           disposition;
    WgConfiguration config;
};
typedef struct wg_config WgConfig;

struct wg_inquire
{
    UCHAR           function_code;
    UCHAR           pad0;
    DWORD           disposition;
    WgConfiguration config;
};
typedef struct wg_inquire WgInquire;


/* Mode */

enum weigher_mode { SCALE_PREPACK_MODE,
                    SCALE_WEIGHT_TRACKING_MODE,
                    SCALE_DUMB_MODE,
                    SCALE_MANUAL_MODE };
typedef enum weigher_mode WeigherMode;

struct wg_mode
{
    UCHAR           function_code;
    UCHAR           pad0;
    UCHAR           mode;
    UCHAR           pad1;
};
typedef struct wg_mode WgMode;


/* Reset */

struct wg_reset
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_reset WgReset;


/* Status */

struct wg_status
{
    UCHAR           function_code;
    UCHAR           pad0;
    WORD            status;
    DWORD           target;
    DWORD           raw_counts;
};
typedef struct wg_status WgStatus;


/* Enable */

struct wg_enable
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_enable WgEnable;


/* Disable */

struct wg_disable
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_disable WgDisable;


/* Re-zero */

struct wg_rezero
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_rezero WgRezero;


/* Calibrate */

struct wg_calibrate
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_calibrate WgCalibrate;


/* Check */

struct wg_check
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_check WgCheck;


/* Weigher Messages */

union wg_message
{
    WgGeneric   generic;
    WgWakeup    wakeup;
    WgConfig    config;
    WgInquire   inquire;
    WgMode      mode;
    WgReset     reset;
    WgStatus    status;
    WgEnable    enable;
    WgDisable   disable;
    WgRezero    rezero;
    WgCalibrate calibrate;
    WgCheck check;
};
typedef union wg_message WgMessage;

struct wg_request
{
    UCHAR           function_code;
    UCHAR           pad0;
};
typedef struct wg_request WgRequest;

typedef struct
{
    UCHAR       Request;
    USHORT      Value;
    USHORT      Index;
    USHORT      Length;

} HostToDevice, *pHostToDevice;

#endif

