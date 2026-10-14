//
// File : PAL_typedef.h
//
// Header declares user defined types needed by PAL to abstract any
// type specific paltform dependencies.
//
#ifndef _PAL_TYPEDEF_H_
#define _PAL_TYPEDEF_H_

#include<windows.h>
#include<winnt.h>
//#include<crtdefs.h>
#include<setupapi.h>
#include<wininet.h>

// #include <curl\curl.h>
typedef void CURL;

typedef HANDLE						PAL_FILE_HANDLE;
typedef HANDLE						PAL_HINSTANCE;
typedef HANDLE	 					PAL_MUTEX;
typedef HANDLE						PAL_PSEM;
typedef SECURITY_ATTRIBUTES 		PAL_MUTEX_ATTRIB;
typedef LPSECURITY_ATTRIBUTES 		PAL_PMUTEX_ATTRIB;
typedef LPSECURITY_ATTRIBUTES 		PAL_PSEM_ATTRIB;
typedef LPSECURITY_ATTRIBUTES 		PAL_PEVENT_ATTRIB;
typedef LPSECURITY_ATTRIBUTES 		PAL_FILESEC_ATTRIB;

typedef EXCEPTION_POINTERS          PAL_EXCEPTION_POINTERS, *PAL_LPEXCEPTION_POINTERS;

typedef HBITMAP                     PAL_HBITMAP;
typedef HFONT                       PAL_HFONT;
typedef HDC                         PAL_HDC;
typedef TEXTMETRIC                  PAL_TEXTMETRIC;
typedef LPSECURITY_ATTRIBUTES       PAL_PTR_SEC_ATTRIBS;
typedef HANDLE			 			PAL_PROC_HNDL;
typedef CRITICAL_SECTION            PAL_CRITICAL_SECTION, *PAL_LPCRITICAL_SECTION;
typedef SIZE_T                      PAL_SIZE_T;
typedef LPTHREAD_START_ROUTINE      PAL_THREAD_START;
typedef HDEVNOTIFY                  PAL_HDEVNOTIFY;
typedef FILETIME                    PAL_FILETIME, *PAL_LPFILETIME;
typedef VS_FIXEDFILEINFO            PAL_VS_FIXEDFILEINFO;
typedef WIN32_FIND_DATAA            PAL_FIND_DATAA;

#define PAL_HMODULE                 HMODULE
#define PAL_PTIMERAPCROUTINE        PTIMERAPCROUTINE
#define PAL_API                     WINAPI

// These need to be LPCSTR, trust me
#define PAL_INET_STR                LPCSTR

#define PAL_DEVICE_NOTIFY_SERVICE_HANDLE    DEVICE_NOTIFY_SERVICE_HANDLE
#define PAL_DEVICE_NOTIFY_ALL_INTERFACE_CLASSES DEVICE_NOTIFY_ALL_INTERFACE_CLASSES

////////////////////////////////
//standard C types
#define PAL_INET_HANDLE_FAIL        NULL                    // Windows uses NULL as fail value of InternetOpen()
#define PAL_INVALID_HANDLE_VALUE    INVALID_HANDLE_VALUE

#define PAL_SOCKET                  SOCKET

#define PAL_THREAD_CREATE_SUSPENDED CREATE_SUSPENDED
#define PAL_TLS_OUT_OF_INDEXES      TLS_OUT_OF_INDEXES

#define PAL_FTP_TRANSFER_TYPE_BINARY    FTP_TRANSFER_TYPE_BINARY
#define PAL_INTERNET_OPEN_TYPE_DIRECT   INTERNET_OPEN_TYPE_DIRECT
#define PAL_INTERNET_DEFAULT_FTP_PORT   INTERNET_DEFAULT_FTP_PORT
#define PAL_INTERNET_DEFAULT_SFTP_PORT   22  // Also not in wininet
#define PAL_INTERNET_SERVICE_FTP        INTERNET_SERVICE_FTP
#define PAL_INTERNET_SERVICE_SFTP	0x0F0F // Not derived from windows so not orininally in wininet.h. Set to high, positive and sort of random to maybe avoid collisions in the future
#define PAL_FILE_GEN_RD             GENERIC_READ            // Flags to open file
#define PAL_FILE_GEN_WR             GENERIC_WRITE
#define PAL_FILE_GEN_EXE            GENERIC_EXECUTE
#define PAL_FILE_GEN_ALL            GENERIC_ALL

#define PAL_FILE_SHARE_RD           FILE_SHARE_READ         // Flags to open file
#define PAL_FILE_SHARE_WR           FILE_SHARE_WRITE
#define PAL_FILE_SHARE_RM           FILE_SHARE_DELETE

#define PAL_FILE_CREATE             CREATE_NEW
#define PAL_FILE_CREATE_FORCE       CREATE_ALWAYS
#define PAL_FILE_OPEN               OPEN_EXISTING
#define PAL_FILE_OPEN_CREATE        OPEN_ALWAYS
#define PAL_FILE_TRUNC              TRUNCATE_EXISTING

#define PAL_FILE_ATTR_RDONLY        FILE_ATTRIBUTE_READONLY
#define PAL_FILE_ATTR_HIDDEN        FILE_ATTRIBUTE_HIDDEN
#define PAL_FILE_ATTR_SYSTEM        FILE_ATTRIBUTE_SYSTEM
#define PAL_FILE_ATTR_DIR           FILE_ATTRIBUTE_DIRECTORY
#define PAL_FILE_ATTR_ARCHIVE       FILE_ATTRIBUTE_ARCHIVE
#define PAL_FILE_ATTR_DEVICE        FILE_ATTRIBUTE_DEVICE
#define PAL_FILE_ATTR_NORMAL        FILE_ATTRIBUTE_NORMAL
#define PAL_FILE_ATTR_TEMP          FILE_ATTRIBUTE_TEMPORARY
#define PAL_FILE_ATTR_SPARSE        FILE_ATTRIBUTE_SPARSE_FILE
#define PAL_FILE_ATTR_REPARSE       FILE_ATTRIBUTE_REPARSE_POINT
#define PAL_FILE_ATTR_COMPRESS      FILE_ATTRIBUTE_COMPRESSED
#define PAL_FILE_ATTR_OFFLINE       FILE_ATTRIBUTE_OFFLINE
#define PAL_FILE_ATTR_INDEX         FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
#define PAL_FILE_ATTR_ENCRYPT       FILE_ATTRIBUTE_ENCRYPTED

#define PAL_FILE_FLAG_WRITE_THROUGH             FILE_FLAG_WRITE_THROUGH
#define PAL_FILE_FLAG_OVERLAPPED                FILE_FLAG_OVERLAPPED
#define PAL_FILE_FLAG_NO_BUFFERING              FILE_FLAG_NO_BUFFERING
#define PAL_FILE_FLAG_RANDOM_ACCESS             FILE_FLAG_RANDOM_ACCESS
#define PAL_FILE_FLAG_SEQUENTIAL_SCAN           FILE_FLAG_SEQUENTIAL_SCAN
#define PAL_FILE_FLAG_DELETE_ON_CLOSE           FILE_FLAG_DELETE_ON_CLOSE
#define PAL_FILE_FLAG_BACKUP_SEMANTICS          FILE_FLAG_BACKUP_SEMANTICS
#define PAL_FILE_FLAG_POSIX_SEMANTICS           FILE_FLAG_POSIX_SEMANTICS
#define PAL_FILE_FLAG_OPEN_REPARSE_POINT        FILE_FLAG_OPEN_REPARSE_POINT
#define PAL_FILE_FLAG_OPEN_NO_RECALL            FILE_FLAG_OPEN_NO_RECALL
#define PAL_FILE_FLAG_FIRST_PIPE_INSTANCE       FILE_FLAG_FIRST_PIPE_INSTANCE

#define PAL_FILE_BEGIN      FILE_BEGIN
#define PAL_FILE_CURRENT    FILE_CURRENT
#define PAL_FILE_END        FILE_END

#define PAL_OVERLAPPED              OVERLAPPED              // Flags to open file
#define PAL_POVERLAPPED             LPOVERLAPPED

#define PAL_FIONBIO                 FIONBIO
#define PAL_FIONREAD                FIONREAD

#define PAL_LPMEMORYSTATUS          LPMEMORYSTATUS
#define PAL_MEMORYSTATUS            MEMORYSTATUS

typedef HINTERNET                   PAL_INET_HANDLE;

typedef __int32						int32_t;
typedef unsigned __int32			uint32_t;
typedef unsigned __int64            uint64_t;

typedef FILETIME                    PAL_TIME_64BIT, *PAL_PTIME_64BIT;   // as time_t in Linux
typedef SYSTEMTIME                  PAL_TIME_STRUCT, *PAL_PTIME_STRUCT; // as "struct tm" in Linux
typedef TIME_ZONE_INFORMATION		PAL_TIME_ZONE_STRUCT, *PAL_PTIME_ZONE_STRUCT;
#define PAL_TIME_ZONE_INVALID		TIME_ZONE_ID_INVALID

typedef _locale_t                   PAL_LOCALE_TYPE;
#define PAL_LOCALE_SYS_DEFAULT      LOCALE_SYSTEM_DEFAULT



typedef DWORD                       PAL_ERRNO_TYPE;
#define CPlatformException          CNTException


// ***** WARNING: We may not need this if UsbManager (EvolutionRTC/USBManager/Source/UsbManager.cpp) is re-written for Linux
typedef PSP_DEVICE_INTERFACE_DATA           PAL_DEVICE_INTERFACE_DATA;


// ***** WARNING: We may not need this if Smbus is separated into different files for differnet systems.
typedef SP_DEVICE_INTERFACE_DETAIL_DATA    PAL_DEVICE_INTERFACE_DETAIL_DATA;
typedef PSP_DEVICE_INTERFACE_DETAIL_DATA   PAL_PDEVICE_INTERFACE_DETAIL_DATA;

/// Holds values related to creating an FTP session
typedef struct _internetSession {
	/// String of Agent to pass to website.
	std::string lpszAgent;
	/// String of proxy to connect to.
	std::string lpszProxyName;
	/// String of websites to bypass on proxy.
	std::string lpszProxyBypass;
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

	_ftpSession()
	{
		memory			=NULL;
		lpszSiteAddress = "";
		curl			= NULL;
		size			= 0;
	}
} ftpSession;

typedef struct _internetHandle {
	DWORD dwSize;
	union
	{
		internetSession *inet;
		ftpSession *ftp;
	};
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
#endif//_PAL_TYPEDEF_H_

