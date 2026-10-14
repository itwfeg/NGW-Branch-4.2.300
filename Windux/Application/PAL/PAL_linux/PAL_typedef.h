//
// File : PAL_typedef.h
//
// Header defines user defined types needed by PAL to abstract any
// type specific paltform dependencies.
//
/// @file PAL_absclass.h
/// Header defines user defined types needed by PAL to abstract any
/// type specific paltform dependencies.
#ifndef _PAL_TYPEDEF_H_
#define _PAL_TYPEDEF_H_

#include <chrono>

#define far
#define FAR far
#define near
#define NEAR near

#define CONST const

#define PAL_API
#define _stdcall

//use to change thrd dat
//as needed

typedef int32_t PAL_ERRNO_TYPE;
#define CPlatformException          CLinuxException
#define PAL_REPORT_ERROR            PAL_SoftwareException(__FILE__, __LINE__)
#define PAL_REPORT_ERRNO(err)       PAL_SoftwareException(__FILE__, __LINE__, err)
#define PAL_ASSERT(cond)            if (!(cond)) { PAL_REPORT_ERROR; }

#define MIN(a,b) (((a)<(b))?(a):(b))

//Misc microsoft mappings


/////////////////////////////////////
//c standard type mapping

/// @name Bit based mappings
//@{
typedef uint8_t BYTE;               // PlatformSDK/Include/WinDef.h
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32, UINT, *UINT_PTR;
typedef uint16_t WORD;
typedef uint32_t DWORD;             ///< 32bit unsigned
typedef uint64_t long64, UINT64;

typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int16_t __int16;
typedef int32_t __int32;
typedef int64_t __int64;
//@}

/// @name short based mappings
//@{
typedef short SHORT;
typedef unsigned short USHORT;  // PlatformSDK/Include/WinDef.h
typedef USHORT *PUSHORT;        // PlatformSDK/Include/WinDef.h
//@}

/// @name char/byte based mappings
//@{
typedef BYTE near *PBYTE;
typedef BYTE far *LPBYTE;
typedef char CHAR;
typedef unsigned char UCHAR;    // PlatformSDK/Include/WinDef.h
typedef CHAR *PCHAR;
typedef UCHAR *PUCHAR;          // PlatformSDK/Include/WinDef.h
//@}

/// @name wchar_t based mappings
//@{
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
//@}

/// @name long based mappings
//@{
typedef long LONG, *PLONG;
typedef long LONG_PTR, *PLONG_PTR;      // PlatformSDK/Include/BaseTsd.h
typedef unsigned long ULONG, *PULONG;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef LONG *LPLONG;
typedef ULONG *LPULONG;
typedef LONG_PTR LPARAM;
//@}

/// @name 64bit based mappings
//@{
typedef __int64 LONGLONG;
typedef long64 ULONGLONG;
typedef ULONGLONG  DWORDLONG;
//@}

/// @name bool based mappings
//@{
typedef bool BOOLEAN, BOOL, *LPBOOL;
//@}

/// @name void based mappings
//@{
typedef void VOID;
typedef void far *LPVOID;               // PlatformSDK/Include/WinDef.h
typedef CONST void *LPCVOID;
//@}

typedef DWORD COLORREF;                 // PlatformSDK/Include/WinDef.h

/// a 64bit signed integer
#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
#endif //MIDL_PASS
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

// Thread support
typedef void *PAL_PTR_SEC_ATTRIBS;
typedef DWORD (*PAL_THREAD_START)(LPVOID args);
typedef void *(*PAL_LINUX_THREAD_START)(LPVOID args);

typedef size_t SIZE_T;
typedef DWORD near *PDWORD, *PDWORD_PTR;
typedef DWORD far *LPDWORD;

typedef WORD far *LPWORD;

// File support

#define PAL_FILE_SHARE_READ                 0x00000001  
#define PAL_FILE_ATTRIBUTE_NORMAL           0x00000080  

#define UNICODE

#ifdef UNICODE
//
//space between L and the string
//is needed in order process compiler
//macros like __FILE__
//
#define _T(s)                           L##"" s
#define PAL_StrLen(n)                   wcslen(n)
#define PAL_StrCmp(s1, s2, len)         wcsncmp(s1, s2, len)
#define PAL_StrCpy(s1, s2, len)         wcsncpy(s1, s2, len)
#else
#define _T(s)                           s
#define PAL_StrLen(n)                   strlen(n)
#define PAL_StrCmp(s1, s2, len)         strncmp(s1, s2, len)
#define PAL_StrCpy(s1, s2, len)         strncpy(s1, s2, len)
#endif

typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character

/// @name Mappings for strings
//@{
typedef char *LPSTR;
typedef const char *LPCSTR;
typedef const wchar_t *LPCWSTR;
typedef wchar_t *LPWSTR;
typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
//@}

#ifdef UNICODE
typedef wchar_t TCHAR;
typedef LPCWSTR LPCTSTR;
typedef LPWSTR PTSTR, LPTSTR;
#else
typedef char TCHAR;
typedef LPCSTR LPCTSTR;
typedef LPSTR PTSTR, LPTSTR, PUTSTR, LPUTSTR;
#endif

//
// UNICODE (Wide Character) types
//

///////////////////////////////////
//generic string defs
#define _strtod_l               strtod_l
#define _strftime_l             strftime_l
#define _locale_t               locale_t
#define sscanf_s                sscanf

///////////////////////////////////
//Misc typedef

typedef void *PVOID;
typedef void *HANDLE;
typedef HANDLE *LPHANDLE;
typedef HANDLE HWND;

typedef pid_t PAL_PROC_HNDL;
typedef pid_t *PAL_PPROC_HNDL;
typedef pthread_mutex_t PAL_MUTEX;
typedef pthread_mutex_t *PAL_PMUTEX;
typedef pthread_mutexattr_t PAL_MUTEX_ATTRIB;
typedef pthread_mutexattr_t *PAL_PMUTEX_ATTRIB;

typedef pthread_t PAL_THREAD;
typedef pthread_t *PAL_PTHREAD;

typedef sem_t PAL_SEM;
typedef sem_t *PAL_PSEM;
typedef HANDLE PAL_PSEM_ATTRIB;
typedef HANDLE PAL_PEVENT_ATTRIB;

typedef HANDLE PAL_HBITMAP;     // ***** WARNING: May need to re-define
typedef HANDLE PAL_HFONT;       // ***** WARNING: May need to re-define
typedef HANDLE PAL_HDC;         // ***** WARNING: May need to re-define
typedef HANDLE PAL_HINSTANCE;
typedef HANDLE PAL_HMODULE;
typedef HANDLE PAL_CRITICAL_SECTION, PAL_LPCRITICAL_SECTION;
typedef HANDLE PAL_LPCRITICAL_SECTION;

typedef PVOID PAL_HDEVNOTIFY;

typedef VOID (PAL_API *PAL_PTIMERAPCROUTINE)(
        LPVOID lpArgToCompletionRoutine,
        DWORD dwTimerLowValue,
        DWORD dwTimerHighValue
        );
#define PAL_DEVICE_NOTIFY_WINDOW_HANDLE          0x00000000
#define PAL_DEVICE_NOTIFY_SERVICE_HANDLE         0x00000001
#define PAL_DEVICE_NOTIFY_ALL_INTERFACE_CLASSES  0x00000004

// PlasformSDK/Include/WinInet.h
typedef int PAL_INET_HANDLE;        ///< Internet handle type
typedef const char * PAL_INET_STR;  ///< Internet value string type
typedef DWORD INTERNET_PORT;        ///< Internet port type

#define PAL_INET_HANDLE_FAIL            (-1)        ///< Linux uses -1 as fail value of socket()
#define INVALID_HANDLE_VALUE        ((HANDLE)-1)    ///< Replace for INVALID_HANDLE_VALUE defined by Win

#define PAL_INTERNET_SERVICE_FTP        1
#define PAL_INTERNET_DEFAULT_FTP_PORT   21          ///< default for FTP servers
#define PAL_INTERNET_OPEN_TYPE_DIRECT   1           ///< direct to the net
#define PAL_FTP_TRANSFER_TYPE_BINARY    0x00000002  ///< Binary transfers
#define PAL_INTERNET_DEFAULT_SFTP_PORT   22  // Also not in wininet
#define PAL_INTERNET_SERVICE_SFTP	0x0F0F // Not derived from windows so not orininally in wininet.h. Set to high, positive and sort of random to maybe avoid collisions in the future

typedef struct sockaddr SOCKADDR, *PSOCKADDR, FAR *LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN, FAR *LPSOCKADDR_IN;
typedef int        PAL_SOCKET;
typedef struct hostent *LPHOSTENT ;

#define ERROR_SUCCESS                    0L         ///< No error
#define ERROR_OUTOFMEMORY                14L
#define ERROR_BUFFER_OVERFLOW            111L       ///< Buffer overflow
#define ERROR_DUP_DOMAINNAME			 1221L


#define ERROR_INTERNET_EXTENDED_ERROR    120003L    ///< Extended error

#define SOCKET_ERROR    (-1)                        ///< Socket error

#define PAL_FIONREAD    0x0             // for linux we'll handle this differently then WIN32
#define PAL_FIONBIO     0x1             // for linux we'll handle this differently then WIN32

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.

#define NETWORK_ALIVE_LAN               1

/// Holds a network address as a char arry
struct _IP_ADDRESS_STRING {
    char String[4 * 4];     ///< An array of chars representing a network address
};
/// An IP Address
typedef struct _IP_ADDRESS_STRING  IP_ADDRESS_STRING;
/// A pointer to an IP Adresss
typedef struct _IP_ADDRESS_STRING *PIP_ADDRESS_STRING;
/// An IP Mask
typedef struct _IP_ADDRESS_STRING IP_MASK_STRING;
/// A pointer to an IP Mask
typedef struct _IP_ADDRESS_STRING *PIP_MASK_STRING;

/// Singly linked structure that holds an address string an a mask string
struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING *Next;
    IP_ADDRESS_STRING      IpAddress;
    IP_MASK_STRING         IpMask;
    DWORD                  Context;
};

typedef struct _IP_ADDR_STRING IP_ADDR_STRING;
typedef struct _IP_ADDR_STRING *PIP_ADDR_STRING;

/// Contains network adapter information
typedef struct _IP_ADAPTER_INFO {
    struct _IP_ADAPTER_INFO *Next;
    DWORD                   ComboIndex;
    char                    AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char                    Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT                    AddressLength;
    BYTE                    Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD                   Index;
    UINT                    Type;
    UINT                    DhcpEnabled;
    PIP_ADDR_STRING         CurrentIpAddress;
    IP_ADDR_STRING          IpAddressList;
    IP_ADDR_STRING          GatewayList;
    IP_ADDR_STRING          DhcpServer;
    BOOL                    HaveWins;
    IP_ADDR_STRING          PrimaryWinsServer;
    IP_ADDR_STRING          SecondaryWinsServer;
    time_t                  LeaseObtained;
    time_t                  LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

typedef ULONG   IPAddr, IPMask;

// these help in converting win32 time to unix time and back
#define TICKSPERSEC        10000000
#define SECSPERDAY         86400
/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
#define EPOCH_DIFF 0x019DB1DED53E8000LL /* 116444736000000000 nsecs */
#define RATE_DIFF 10000000 /* 100 nsecs */

#define MAX_PATH        PATH_MAX        // PATH_MAX is defined in <linux/limits.h>
#define _MAX_DRIVE      PATH_MAX
#define _MAX_DIR        PATH_MAX
#define _MAX_FNAME      PATH_MAX

// VS8/VC/PlatformSDK/Include/WinBase.h
#define PAL_THREAD_CREATE_SUSPENDED     (0x00000004)
#define PAL_TLS_OUT_OF_INDEXES          ((DWORD)0xFFFFFFFF)

//
// For PAL_openFile() with Windows compatibility.
//
// VS8/VC/PlatformSDK/Include/WinNT.h
#define PAL_FILE_GEN_RD                 (0x80000000)
#define PAL_FILE_GEN_WR                 (0x40000000)
#define PAL_FILE_GEN_EXE                (0x20000000)
#define PAL_FILE_GEN_ALL                (0x10000000)

#define PAL_FILE_SHARE_RD               (0x00000001)        // Read
#define PAL_FILE_SHARE_WR               (0x00000002)        // Write
#define PAL_FILE_SHARE_RM               (0x00000004)        // Remove/Delete

#define PAL_FILE_CREATE                 (1)
#define PAL_FILE_CREATE_FORCE           (2)
#define PAL_FILE_OPEN                   (3)
#define PAL_FILE_OPEN_CREATE            (4)
#define PAL_FILE_TRUNC                  (5)

//#define PAL_FILE_ATTR_RDONLY            (0x00000001)
//#define PAL_FILE_ATTR_HIDDEN            (0x00000002)
//#define PAL_FILE_ATTR_SYSTEM            (0x00000004)
//#define PAL_FILE_ATTR_DIR               (0x00000010)        // Un-documented
#define PAL_FILE_ATTR_ARCHIVE           (0x00000020)
//#define PAL_FILE_ATTR_DEVICE            (0x00000040)        // Un-documented
#define PAL_FILE_ATTR_NORMAL            (0x00000080)
//#define PAL_FILE_ATTR_TEMP              (0x00000100)
//#define PAL_FILE_ATTR_SPARSE            (0x00000200)        // Un-documented
//#define PAL_FILE_ATTR_REPARSE           (0x00000400)        // Un-documented
//#define PAL_FILE_ATTR_COMPRESS          (0x00000800)        // Un-documented
//#define PAL_FILE_ATTR_OFFLINE           (0x00001000)
//#define PAL_FILE_ATTR_INDEX             (0x00002000)        // Un-documented
//#define PAL_FILE_ATTR_ENCRYPT           (0x00004000)

// VS8/VC/PlatformSDK/Include/WinBase.h
//#define PAL_FILE_FLAG_WRITE_THROUGH             (0x80000000)
#define PAL_FILE_FLAG_OVERLAPPED                (0x40000000)
//#define PAL_FILE_FLAG_NO_BUFFERING              (0x20000000)
//#define PAL_FILE_FLAG_RANDOM_ACCESS             (0x10000000)
#define PAL_FILE_FLAG_SEQUENTIAL_SCAN           (0x08000000)
//#define PAL_FILE_FLAG_DELETE_ON_CLOSE           (0x04000000)
//#define PAL_FILE_FLAG_BACKUP_SEMANTICS          (0x02000000)
//#define PAL_FILE_FLAG_POSIX_SEMANTICS           (0x01000000)
//#define PAL_FILE_FLAG_OPEN_REPARSE_POINT        (0x00200000)
//#define PAL_FILE_FLAG_OPEN_NO_RECALL            (0x00100000)
//#define PAL_FILE_FLAG_FIRST_PIPE_INSTANCE       (0x00080000)

#define PAL_FILE_BEGIN      SEEK_SET    // beginning of file
#define PAL_FILE_CURRENT    SEEK_CUR    // current file position
#define PAL_FILE_END        SEEK_END    // end of file

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
/*
typedef struct _PAL_OVERLAPPED          // We are not going to use it in Linux
{
#if 0
    union
    {
        struct
        {
            DWORD   Offset;
            DWORD   OffsetHigh;        
        };
        PVOID   Pointer;    
    };
#endif
    HANDLE  hEvent;
}   PAL_OVERLAPPED, *PAL_POVERLAPPED;
*/

typedef struct _PAL_TEXTMETRIC  // ***** WARNING: Maybe we need to re-write FONT rendering functions
{
    // ***** WARNING: structure for Linux
    // check TEXTMETRIC structure for Win
    //
} PAL_TEXTMETRIC, *PAL_PTEXTMETRIC;

typedef struct usb_device_descriptor USB_DEVICE_DESCRIPTOR,
        *PUSB_DEVICE_DESCRIPTOR;
typedef struct usb_config_descriptor USB_CONFIGURATION_DESCRIPTOR,
        *PUSB_CONFIGURATION_DESCRIPTOR;
typedef struct usb_interface_descriptor USB_INTERFACE_DESCRIPTOR,
        *PUSB_INTERFACE_DESCRIPTOR;
//typedef struct usb_endpoint_descriptor USB_ENDPOINT_DESCRIPTOR,
//        *PUSB_ENDPOINT_DESCRIPTOR;

/*
typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bEndpointAddress;
    UCHAR bmAttributes;
    USHORT wMaxPacketSize;
    UCHAR bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_PIPE_INFO {  
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
    ULONG ScheduleOffset;        
} USB_PIPE_INFO, *PUSB_PIPE_INFO;

typedef enum _USB_HUB_NODE {
    UsbHub,
    UsbMIParent
} USB_HUB_NODE;
*/

///////////////////////////////////
//  PlatformSDK/Include/WinDef.h
//
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))

#define DWORD_PTR           DWORD       // PlatformSDK/Include/HtmlHelp.h

#define Int32x32To64(a, b) ((LONGLONG)((LONG)(a)) * (LONGLONG)((LONG)(b)))


///////////////////////////////////
//  PlatformSDK/Include/BaseTsd.h
//

// (not WIN64)
#define HandleToULong( h ) ((ULONG)(ULONG_PTR)(h) )
#define HandleToLong( h )  ((LONG)(LONG_PTR) (h) )
#define ULongToHandle( ul ) ((HANDLE)(ULONG_PTR) (ul) )
#define LongToHandle( h )   ((HANDLE)(LONG_PTR) (h) )
#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )
#define PtrToLong( p )  ((LONG)(LONG_PTR) (p) )
#define PtrToUint( p ) ((UINT)(UINT_PTR) (p) )
#define PtrToInt( p )  ((INT)(INT_PTR) (p) )


///////////////////////////////////
//  PlatformSDK/Include/WinGDI.h
//
/* Ternary raster operations */
#define SRCCOPY             (DWORD)0x00CC0020   /* dest = source                   */
#define SRCPAINT            (DWORD)0x00EE0086   /* dest = source OR dest           */
#define SRCAND              (DWORD)0x008800C6   /* dest = source AND dest          */
#define SRCINVERT           (DWORD)0x00660046   /* dest = source XOR dest          */
#define SRCERASE            (DWORD)0x00440328   /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY          (DWORD)0x00330008   /* dest = (NOT source)             */
#define NOTSRCERASE         (DWORD)0x001100A6   /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY           (DWORD)0x00C000CA   /* dest = (source AND pattern)     */
#define MERGEPAINT          (DWORD)0x00BB0226   /* dest = (NOT source) OR dest     */
#define PATCOPY             (DWORD)0x00F00021   /* dest = pattern                  */
#define PATPAINT            (DWORD)0x00FB0A09   /* dest = DPSnoo                   */
#define PATINVERT           (DWORD)0x005A0049   /* dest = pattern XOR dest         */
#define DSTINVERT           (DWORD)0x00550009   /* dest = (NOT dest)               */
#define BLACKNESS           (DWORD)0x00000042   /* dest = BLACK                    */
#define WHITENESS           (DWORD)0x00FF0062   /* dest = WHITE                    */

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

///////////////////////////////////
//Mapped routines
#ifdef UNICODE
#define _snwprintf      swprintf
//#define _stprintf       wprintf       // Linux has no equivlent function call, use _snwprintf() instead
#define _ftprintf       fwprintf
#define _ultot          PAL_absclass::PAL_ultow
#define _tcsclen        wcslen
#define _tcsicmp        wcscasecmp
#define _tprintf        wprintf
#define _tcsncmp        wcsncmp
#define _wsplitpath     PAL_absclass::PAL_wsplitpath

#else
#define _stprintf       printf
#define _ftprintf       fprintf
#define _ultot          PAL_absclass::PAL_ultoa
#define _tcsclen        strlen
#define _tcsicmp        strcasecmp
#define _tprintf        printf
#define _tcsncmp        strncmp
#define _wsplitpath     PAL_absclass::PAL_splitpath

#endif

/////////////////////////////////////
//Genaric defs
#define TRUE                    true
#define FALSE                   false

#define INFINITE                0xFFFFFFFF
#define DUPLICATE_SAME_ACCESS   0x00000002

#define CALLBACK
#define IN
#define OUT

////////////////////////////////////
//WaitOnXXXObject return values
#define WAIT_OBJECT_0           0x00000000L
#define WAIT_FAILED             ((DWORD) 0xFFFFFFFF)
#define WAIT_ABANDONED          0x00000080L
#define WAIT_ABANDONED_0        0x00000080L
#define WAIT_TIMEOUT            258L

/////////////////////////////////////
// equivalent locale categories
enum
{
    WIN_LC_ALL = 0,
    WIN_LC_COLLATE,
    WIN_LC_CTYPE,
    WIN_LC_MONETARY,
    WIN_LC_NUMERIC,
    WIN_LC_TIME,
};
#define LC_MIN                  WIN_LC_ALL
#define LC_MAX                  WIN_LC_TIME

#define PAL_LC_ALL              WIN_LC_ALL
#define PAL_LC_COLLATE          WIN_LC_COLLATE
#define PAL_LC_CTYPE            WIN_LC_CTYPE
#define PAL_LC_MONETARY         WIN_LC_MONETARY
#define PAL_LC_NUMERIC          WIN_LC_NUMERIC
#define PAL_LC_TIME             WIN_LC_TIME

typedef locale_t PAL_LOCALE_TYPE;
typedef locale_t *PAL_PLOCALE_TYPE;

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

typedef FILE *PAL_FILE_HANDLE;

/// structure that represents a FILETIME
typedef struct _PAL_FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} PAL_FILETIME, *PAL_LPFILETIME; // as FILETIME in Win


typedef enum    _PAL_DATETIME_FORMAT_PATTERN
{
    // http://msdn.microsoft.com/en-us/library/dd317787%28v=VS.85%29.aspx
    PAL_DATETIME_FORMAT_YEAR,           // y, yy, yyyy, yyyyy
    PAL_DATETIME_FORMAT_MONTH,          // M, MM, MMM, MMMM
    PAL_DATETIME_FORMAT_DAY,            // d, dd, ddd, dddd

    // http://msdn.microsoft.com/en-us/library/dd318131%28v=VS.85%29.aspx
    PAL_DATETIME_FORMAT_HOUR12,         // h, hh
    PAL_DATETIME_FORMAT_HOUR24,         // H, HH
    PAL_DATETIME_FORMAT_MINUTE,         // m, mm
    PAL_DATETIME_FORMAT_SECOND,         // s, ss
    PAL_DATETIME_FORMAT_AMPM,           // t, tt

    PAL_DATETIME_FORMAT_NULL
} PAL_DATETIME_FORMAT_PATTERN;

// for PAL_getDateFormat()
#define PAL_LOCALE_LONG_MONTHNAME_1     "January"
#define PAL_LOCALE_LONG_MONTHNAME_2     "February"
#define PAL_LOCALE_LONG_MONTHNAME_3     "March"
#define PAL_LOCALE_LONG_MONTHNAME_4     "April"
#define PAL_LOCALE_LONG_MONTHNAME_5     "May"
#define PAL_LOCALE_LONG_MONTHNAME_6     "June"
#define PAL_LOCALE_LONG_MONTHNAME_7     "July"
#define PAL_LOCALE_LONG_MONTHNAME_8     "August"
#define PAL_LOCALE_LONG_MONTHNAME_9     "September"
#define PAL_LOCALE_LONG_MONTHNAME_10    "October"
#define PAL_LOCALE_LONG_MONTHNAME_11    "November"
#define PAL_LOCALE_LONG_MONTHNAME_12    "December"

#define PAL_LOCALE_SHORT_MONTHNAME_1    "Jan"
#define PAL_LOCALE_SHORT_MONTHNAME_2    "Feb"
#define PAL_LOCALE_SHORT_MONTHNAME_3    "Mar"
#define PAL_LOCALE_SHORT_MONTHNAME_4    "Apr"
#define PAL_LOCALE_SHORT_MONTHNAME_5    "May"
#define PAL_LOCALE_SHORT_MONTHNAME_6    "Jun"
#define PAL_LOCALE_SHORT_MONTHNAME_7    "Jul"
#define PAL_LOCALE_SHORT_MONTHNAME_8    "Aug"
#define PAL_LOCALE_SHORT_MONTHNAME_9    "Sep"
#define PAL_LOCALE_SHORT_MONTHNAME_10   "Oct"
#define PAL_LOCALE_SHORT_MONTHNAME_11   "Nov"
#define PAL_LOCALE_SHORT_MONTHNAME_12   "Dec"

#define PAL_LOCALE_LONG_DAYNAME_0       "Sunday"
#define PAL_LOCALE_LONG_DAYNAME_1       "Monday"
#define PAL_LOCALE_LONG_DAYNAME_2       "Tuesday"
#define PAL_LOCALE_LONG_DAYNAME_3       "Wednesday"
#define PAL_LOCALE_LONG_DAYNAME_4       "Thursday"
#define PAL_LOCALE_LONG_DAYNAME_5       "Friday"
#define PAL_LOCALE_LONG_DAYNAME_6       "Satday"

#define PAL_LOCALE_SHORT_DAYNAME_0      "Sun"
#define PAL_LOCALE_SHORT_DAYNAME_1      "Mon"
#define PAL_LOCALE_SHORT_DAYNAME_2      "Tue"
#define PAL_LOCALE_SHORT_DAYNAME_3      "Wed"
#define PAL_LOCALE_SHORT_DAYNAME_4      "Thu"
#define PAL_LOCALE_SHORT_DAYNAME_5      "Fri"
#define PAL_LOCALE_SHORT_DAYNAME_6      "Sat"

/// a structure that contains a date string and the size of that string
typedef struct  _PAL_DATEFORMAT_DATA_STRU
{
    const char* str_date;
    int         str_size;
} PAL_DATETIME_DATA_STRU;

/// a structure that represents a SYSTEMTIME value
typedef struct _PAL_SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} PAL_TIME_STRUCT, *PAL_PTIME_STRUCT;   // as SYSTEMTIME in Win

/// a structure that represents a TIME_ZONE_INFORMATION value
typedef struct _PAL_TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    PAL_TIME_STRUCT StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    PAL_TIME_STRUCT DaylightDate;
    LONG DaylightBias;
} PAL_TIME_ZONE_STRUCT, *PAL_PTIME_ZONE_STRUCT;   // as TIME_ZONE_INFORMATION in Win

#define PAL_TIME_ZONE_INVALID		((DWORD)0xFFFFFFFF)

#define STILL_ACTIVE                ((DWORD)0x00000103L)

typedef int32_t *PAL_OLD_THRD_DAT;  // this typedef allows

/// Holds thread exectution values
struct PAL_CThreadData
{
    int32_t m_errno;
    UINT m_exitcode;
    DWORD m_uservalue;

    /// Constructor
    PAL_CThreadData() :
    	m_errno(0),
		m_exitcode(0),
		m_uservalue(0)
    {
    }

};

//
// Cross function thread data saved in global link-list
//
// We use linked-list instead of Array structure here
//
//              Linked-List     v.s.    Array
//              ----------              ----------
//  Insert      to Head (fast)          Lookup empty slot
//  Delete      Lookup Handle           Lookup Handle
//  Search      Lookup                  Lookup
//
/// Holds parameters needed for suspended threads
struct _CREATE_SUSPENDED
{
    /// a protective mutex
    pthread_mutex_t     mutex;
    /// a thread conditional on which we wait
    pthread_cond_t      cond;
};

/// Structure for global linked list of thread data
typedef struct _PAL_GLOBAL_THREAD_DATA
{
	typedef std::chrono::system_clock clock_t;
	typedef std::chrono::time_point<clock_t> time_point_t;

    /// Thread handle
    HANDLE  hThread;

    /// structure for threads created in suspended mode
    struct _CREATE_SUSPENDED CREATE_SUSPENDED;

    /// Exit code for the thread
    DWORD   m_exitcode;

    /// Exit time for the thread
    time_point_t m_exittime;

    /// Constructor
	_PAL_GLOBAL_THREAD_DATA() :
		hThread(NULL),
		m_exitcode(0)
	{

	}

	/// Set thread exit code
    void setExitCode( UINT value )
    {
    	m_exitcode = value;
    	m_exittime = clock_t::now();
    }

    /// Check if thread running
    bool isRunning() const
    {
    	return (( STILL_ACTIVE == m_exitcode ) || ( time_point_t() == m_exittime ));
    }

    /// Retrieve ms since thread exited
    UINT millisecondsSinceExit() const
    {
    	if ( isRunning() )
    		return 0;

    	const std::chrono::milliseconds delta(
   			std::chrono::duration_cast<std::chrono::milliseconds>( clock_t::now() - m_exittime ) );

    	return delta.count();
    }

} PAL_GLOBAL_THREAD_DATA, *PAL_PGLOBAL_THREAD_DATA;

/// Structure for PAL threads
typedef struct _PAL_WRAPPER_THREAD_INFO
{
	_PAL_WRAPPER_THREAD_INFO() :
		creation_flags(0),
		start_addr(NULL),
		param(NULL),
		ptr(NULL),
		pGlobalData(NULL)
	{

	}

    DWORD               creation_flags; ///> [in] Pass flag into wrapper thread to decide to call pthread_cond_wait() or not.
    PAL_THREAD_START    start_addr;     ///> [in] Pass thread function address into wrapper thread
    LPVOID              param;          ///> [in] Pass thread parameter into wrapper thread
    void                *ptr;
    sem_t               sem;
    PAL_PGLOBAL_THREAD_DATA pGlobalData;

} PAL_WRAPPER_THREAD_INFO, *PAL_PWRAPPER_THREAD_INFO;

/// Holds values about physical and virtual memory.
typedef struct _PAL_MEMORYSTATUS {
    /// The size of the MEMORYSTATUS data structure, in bytes.
    DWORD dwLength;
    /// A number between 0 and 100 that specifies the approximate percentage of physical memory that is in use
    DWORD dwMemoryLoad;
    /// The amount of actual physical memory, in bytes.
    SIZE_T dwTotalPhys;
    /// The amount of physical memory currently available, in bytes.
    SIZE_T dwAvailPhys;
    /// The current size of the committed memory limit, in bytes.
    SIZE_T dwTotalPageFile;
    /// The maximum amount of memory the current process can commit, in bytes.
    SIZE_T dwAvailPageFile;
    /// The size of the user-mode portion of the virtual address space of the calling process, in bytes.
    SIZE_T dwTotalVirtual;
    /// The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes.
    SIZE_T dwAvailVirtual;
} PAL_MEMORYSTATUS, *PAL_LPMEMORYSTATUS;

typedef DWORD LCID;

#define PAL_LOCALE_SYS_DEFAULT      0x0800

#ifndef WSABASEERR

#define WSABASEERR              10000

/*
 * Sockets definitions of regular Microsoft C error constants
 */
#define WSAEINTR                (WSABASEERR+4)
#define WSAEBADF                (WSABASEERR+9)
#define WSAEACCES               (WSABASEERR+13)
#define WSAEFAULT               (WSABASEERR+14)
#define WSAEINVAL               (WSABASEERR+22)
#define WSAEMFILE               (WSABASEERR+24)

/*
 * Sockets definitions of regular Berkeley error constants
 */
#define WSAEWOULDBLOCK          (WSABASEERR+35)
#define WSAEINPROGRESS          (WSABASEERR+36)
#define WSAEALREADY             (WSABASEERR+37)
#define WSAENOTSOCK             (WSABASEERR+38)
#define WSAEDESTADDRREQ         (WSABASEERR+39)
#define WSAEMSGSIZE             (WSABASEERR+40)
#define WSAEPROTOTYPE           (WSABASEERR+41)
#define WSAENOPROTOOPT          (WSABASEERR+42)
#define WSAEPROTONOSUPPORT      (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT      (WSABASEERR+44)
#define WSAEOPNOTSUPP           (WSABASEERR+45)
#define WSAEPFNOSUPPORT         (WSABASEERR+46)
#define WSAEAFNOSUPPORT         (WSABASEERR+47)
#define WSAEADDRINUSE           (WSABASEERR+48)
#define WSAEADDRNOTAVAIL        (WSABASEERR+49)
#define WSAENETDOWN             (WSABASEERR+50)
#define WSAENETUNREACH          (WSABASEERR+51)
#define WSAENETRESET            (WSABASEERR+52)
#define WSAECONNABORTED         (WSABASEERR+53)
#define WSAECONNRESET           (WSABASEERR+54)
#define WSAENOBUFS              (WSABASEERR+55)
#define WSAEISCONN              (WSABASEERR+56)
#define WSAENOTCONN             (WSABASEERR+57)
#define WSAESHUTDOWN            (WSABASEERR+58)
#define WSAETOOMANYREFS         (WSABASEERR+59)
#define WSAETIMEDOUT            (WSABASEERR+60)
#define WSAECONNREFUSED         (WSABASEERR+61)
#define WSAELOOP                (WSABASEERR+62)
#define WSAENAMETOOLONG         (WSABASEERR+63)
#define WSAEHOSTDOWN            (WSABASEERR+64)
#define WSAEHOSTUNREACH         (WSABASEERR+65)
#define WSAENOTEMPTY            (WSABASEERR+66)
#define WSAEPROCLIM             (WSABASEERR+67)
#define WSAEUSERS               (WSABASEERR+68)
#define WSAEDQUOT               (WSABASEERR+69)
#define WSAESTALE               (WSABASEERR+70)
#define WSAEREMOTE              (WSABASEERR+71)

/*
 * Extended Sockets error constant definitions
 */
#define WSASYSNOTREADY          (WSABASEERR+91)
#define WSAVERNOTSUPPORTED      (WSABASEERR+92)
#define WSANOTINITIALISED       (WSABASEERR+93)
#define WSAEDISCON              (WSABASEERR+101)
#define WSAENOMORE              (WSABASEERR+102)
#define WSAECANCELLED           (WSABASEERR+103)
#define WSAEINVALIDPROCTABLE    (WSABASEERR+104)
#define WSAEINVALIDPROVIDER     (WSABASEERR+105)
#define WSAEPROVIDERFAILEDINIT  (WSABASEERR+106)
#define WSASYSCALLFAILURE       (WSABASEERR+107)
#define WSASERVICE_NOT_FOUND    (WSABASEERR+108)
#define WSATYPE_NOT_FOUND       (WSABASEERR+109)
#define WSA_E_NO_MORE           (WSABASEERR+110)
#define WSA_E_CANCELLED         (WSABASEERR+111)
#define WSAEREFUSED             (WSABASEERR+112)
#endif //

/// Holds values related to creating an FTP session
typedef struct _internetSession {
	/// String of Agent to pass to website.
	std::string lpszAgent;
	/// String of proxy to connect to.
	std::string lpszProxyName;
	/// String of websites to bypass on proxy.
	std::string lpszProxyBypass;
	_internetSession() : lpszAgent(""), lpszProxyName(""), lpszProxyBypass("")
	{
	}
} internetSession;

typedef struct _ftpSession {
	/// String of ftp site, address and directory
	std::string lpszSiteAddress;
	/// a pointer to our internet functionality.
	CURL *curl;
	/// a pointer to memory we store network data.
	char *memory;
	/// the size of the memory pointer.
	size_t size;
	_ftpSession() : lpszSiteAddress(""), curl(0), memory(NULL), size(0)
	{
	}
} ftpSession;

typedef struct _internetHandle {
	DWORD dwSize;
	union
	{
		internetSession *inet;
		ftpSession *ftp;
	};
	_internetHandle() : dwSize(0), inet(NULL)
	{
	}
} internetHandle;

/// Holds information on a file in an FTP directory
struct ftpFileInfo
{
    bool bDir;
    std::string name;
    ftpFileInfo() : bDir(false), name("")
    {
    }
};

/// Holds information about network routing
struct route_info
{
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};

//-----------------------------------------------------------------------------

DWORD PAL_mapErrnoLinux2Win32(int linux_errno);
void PAL_SoftwareException(LPCSTR err_file, unsigned long err_line, int err_errno = errno);

#endif //_PAL_TYPEDEF_H_

