//
// File : PAL_tmp.h
//
// Temporary definitions needed for compiling linux
//
//
/// @file PAL_tmp.h
/// Temporary definitions needed for compiling linux. 
/// These should be moved
/// to an appropriate header file if they are determined to be permanent.

#ifndef _PAL_TMP_H_
#define _PAL_TMP_H_

/// @cond HIDE 
#define MIN_REMOVE

#define __stdcall

#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_TIME_CRITICAL  15

#define NOFBWF

#define DECLSPEC_ALIGN(x)
/// @endcond

//typedef struct DECLSPEC_ALIGN(16) _CONTEXT {
//} CONTEXT, *PCONTEXT;

/// @name Exception temporaries
//@{
// See WinBase.h
enum PAL_EXP
{
    EXCEPTION_ACCESS_VIOLATION=-20,
    EXCEPTION_BREAKPOINT,
    EXCEPTION_DATATYPE_MISALIGNMENT,
    EXCEPTION_SINGLE_STEP,
    EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
    EXCEPTION_FLT_DENORMAL_OPERAND,
    EXCEPTION_FLT_DIVIDE_BY_ZERO,
    EXCEPTION_FLT_INEXACT_RESULT,
    EXCEPTION_FLT_INVALID_OPERATION,
    EXCEPTION_FLT_OVERFLOW,
    EXCEPTION_FLT_STACK_CHECK,
    EXCEPTION_FLT_UNDERFLOW,
    EXCEPTION_INT_DIVIDE_BY_ZERO,
    EXCEPTION_INT_OVERFLOW,
    EXCEPTION_PRIV_INSTRUCTION,
    EXCEPTION_NONCONTINUABLE_EXCEPTION
};

#if 0
#define EXCEPTION_EXECUTE_HANDLER 1

/// maximum number of exception parameters
#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct {
    DWORD    ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
    } EXCEPTION_RECORD;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS
{
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

typedef EXCEPTION_POINTERS          PAL_EXCEPTION_POINTERS, *PAL_LPEXCEPTION_POINTERS;

typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;
//@}

typedef struct WINDOWINFO
{
    // ***** WARNING: structure for Linux
    SIZE_T  cbSize;
}   WINDOWINFO_TYPE;

// ***** WARNING: We may not need this if UsbManager (EvolutionRTC/USBManager/Source/UsbManager.cpp) is re-written for Linux
typedef PVOID HDEVINFO;
typedef struct _PAL_DEVICE_INTERFACE_DATA
{
    // ***** WARNING: structure for Linux
    // See PlatformSDK/Include/SetupAPI.h
    //
} PAL_DEVICE_INTERFACE_DATA, *PAL_PDEVICE_INTERFACE_DATA;


// ***** WARNING: We may not need this if Smbus is separated into different files for differnet systems.
typedef struct _PAL_DEVICE_INTERFACE_DETAIL_DATA
{
    // ***** WARNING: structure for Linux
    // See PlatformSDK/Include/SetupAPI.h
    //
} PAL_DEVICE_INTERFACE_DETAIL_DATA, *PAL_PDEVICE_INTERFACE_DETAIL_DATA;
#endif

#if 0
// *****  WARNING: Linux might need to handle these differently
typedef struct _SERVICE_STATUS {
    DWORD   dwServiceType;
    DWORD   dwCurrentState;
    DWORD   dwControlsAccepted;
    DWORD   dwWin32ExitCode;
    DWORD   dwServiceSpecificExitCode;
    DWORD   dwCheckPoint;
    DWORD   dwWaitHint;
} SERVICE_STATUS, *LPSERVICE_STATUS;

typedef HANDLE SERVICE_STATUS_HANDLE;
#endif

#if 0
// ***** WARNING: We may not need this structure once we rewrite the code for device accessing.
// ----- VS8/VC/PlatformSDK/Include/Guiddef.h
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    BYTE           Data4[ 8 ];
} GUID;
typedef GUID *LPGUID;


#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#define DECLSPEC_SELECTANY
// We could only have one GUID initialization, real assignment is defined in linux_guid.cpp for that application (source/app/)
#if 0
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name \
    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID FAR name
#endif

#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#ifdef __cplusplus
#define REFGUID const GUID &
#else
#define REFGUID const GUID * __MIDL_CONST
#endif
#endif

#if !defined (__midl)
#if !defined (_SYS_GUID_OPERATORS_)
#define _SYS_GUID_OPERATORS_

// Faster (but makes code fatter) inline version...use sparingly
#ifdef __cplusplus
__inline int InlineIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
   return (
      ((unsigned long *) &rguid1)[0] == ((unsigned long *) &rguid2)[0] &&
      ((unsigned long *) &rguid1)[1] == ((unsigned long *) &rguid2)[1] &&
      ((unsigned long *) &rguid1)[2] == ((unsigned long *) &rguid2)[2] &&
      ((unsigned long *) &rguid1)[3] == ((unsigned long *) &rguid2)[3]);
}

__inline int IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

#else   // ! __cplusplus

#define InlineIsEqualGUID(rguid1, rguid2)  \
        (((unsigned long *) rguid1)[0] == ((unsigned long *) rguid2)[0] &&   \
        ((unsigned long *) rguid1)[1] == ((unsigned long *) rguid2)[1] &&    \
        ((unsigned long *) rguid1)[2] == ((unsigned long *) rguid2)[2] &&    \
        ((unsigned long *) rguid1)[3] == ((unsigned long *) rguid2)[3])

#define IsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(GUID)))

#endif  // __cplusplus

#ifdef __INLINE_ISEQUAL_GUID
#undef IsEqualGUID
#define IsEqualGUID(rguid1, rguid2) InlineIsEqualGUID(rguid1, rguid2)
#endif

// Same type, different name

#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)


#if !defined _SYS_GUID_OPERATOR_EQ_ && !defined _NO_SYS_GUID_OPERATOR_EQ_
#define _SYS_GUID_OPERATOR_EQ_
// A couple of C++ helpers

#ifdef __cplusplus
__inline int operator==(REFGUID guidOne, REFGUID guidOther)
{
    return IsEqualGUID(guidOne,guidOther);
}

__inline int operator!=(REFGUID guidOne, REFGUID guidOther)
{
    return !(guidOne == guidOther);
}
#endif
#endif  // _SYS_GUID_OPERATOR_EQ_
#endif  // _SYS_GUID_OPERATORS_
#endif  // __midl

// ----- VS8/VC/PlatformSDK/Include/Dbt.h
#define DBT_DEVICEARRIVAL               0x8000  // system detected a new device
#define DBT_DEVICEQUERYREMOVE           0x8001  // wants to remove, may fail
#define DBT_DEVICEQUERYREMOVEFAILED     0x8002  // removal aborted
#define DBT_DEVICEREMOVEPENDING         0x8003  // about to remove, still avail.
#define DBT_DEVICEREMOVECOMPLETE        0x8004  // device is gone
#define DBT_DEVICETYPESPECIFIC          0x8005  // type specific event

#define DBT_DEVTYP_DEVICEINTERFACE      0x00000005  // device interface class
#define DBT_DEVTYP_HANDLE               0x00000006  // file system handle

#define DEVICE_NOTIFY_SERVICE_HANDLE         0x00000001
#define DEVICE_NOTIFY_ALL_INTERFACE_CLASSES  0x00000004

#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define LANG_NEUTRAL                     0x00
#define SUBLANG_NEUTRAL                  0x00    // language neutral
#define SUBLANG_DEFAULT                  0x01    // user default
#endif

#if 0
/*
 * Device broadcast header
 */

#define DBTFAR

struct _DEV_BROADCAST_HDR {     /* */
    DWORD       dbch_size;
    DWORD       dbch_devicetype;
    DWORD       dbch_reserved;
};

typedef struct  _DEV_BROADCAST_HDR      DEV_BROADCAST_HDR;
typedef struct  _DEV_BROADCAST_HDR      DBTFAR *PDEV_BROADCAST_HDR;

typedef struct _DEV_BROADCAST_DEVICEINTERFACE_A {
    DWORD       dbcc_size;
    DWORD       dbcc_devicetype;
    DWORD       dbcc_reserved;
    GUID        dbcc_classguid;
    char        dbcc_name[1];
} DEV_BROADCAST_DEVICEINTERFACE_A, *PDEV_BROADCAST_DEVICEINTERFACE_A;

typedef struct _DEV_BROADCAST_DEVICEINTERFACE_W {
    DWORD       dbcc_size;
    DWORD       dbcc_devicetype;
    DWORD       dbcc_reserved;
    GUID        dbcc_classguid;
    wchar_t     dbcc_name[1];
} DEV_BROADCAST_DEVICEINTERFACE_W, *PDEV_BROADCAST_DEVICEINTERFACE_W;

#ifdef UNICODE
typedef DEV_BROADCAST_DEVICEINTERFACE_W   DEV_BROADCAST_DEVICEINTERFACE;
typedef PDEV_BROADCAST_DEVICEINTERFACE_W  PDEV_BROADCAST_DEVICEINTERFACE;
#else
typedef DEV_BROADCAST_DEVICEINTERFACE_A   DEV_BROADCAST_DEVICEINTERFACE;
typedef PDEV_BROADCAST_DEVICEINTERFACE_A  PDEV_BROADCAST_DEVICEINTERFACE;
#endif
#endif


#if 0
// ----- VS8/VC/PlatformSDK/Include/winsock2.h -----
#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
#ifdef _WIN64
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char FAR *              lpVendorInfo;
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
#else
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char FAR *              lpVendorInfo;
#endif
} WSADATA, * LPWSADATA;
#endif

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF

#define ERROR_INVALID_HANDLE             6L
#define ERROR_IO_PENDING                 997L    // dderror

//
//  Code Page Default Values.
//
#define CP_ACP                    0           ///< default to ANSI code page
#define CP_OEMCP                  1           ///< default to OEM  code page
#define CP_MACCP                  2           ///< default to MAC  code page
#define CP_THREAD_ACP             3           ///< current thread's ANSI code page
#define CP_SYMBOL                 42          ///< SYMBOL translations

#define CP_UTF7                   65000       ///< UTF-7 translation
#define CP_UTF8                   65001       ///< UTF-8 translation


/// Endpoint direction bit, stored in address
#define USB_ENDPOINT_DIRECTION_MASK               0x80

/// test direction bit in the bEndpointAddress field of an endpoint descriptor.
#define USB_ENDPOINT_DIRECTION_OUT(addr)          (!((addr) & USB_ENDPOINT_DIRECTION_MASK))
/// test direction bit in the bEndpointAddress field of an endpoint descriptor.
#define USB_ENDPOINT_DIRECTION_IN(addr)           ((addr) & USB_ENDPOINT_DIRECTION_MASK)


#endif  // _PAL_TMP_H_
