//
// File : PAL_platformheader.h
//
// This header is used for including PAL's platforms specific 
// headers based on PAL_WIN32_RUN ifdef.
//
/// @file PAL_platformheader.h
/// Includes further headers based on ifdef settings.
/// This header is used for including PAL's platforms specific 
/// headers based on PAL_WIN32_RUN ifdef.
#ifndef _PAL_PLATFORM_HEADER_H_
#define _PAL_PLATFORM_HEADER_H_

#if defined( _WIN32 )
typedef long long long64;
#endif

#ifndef DOXYGEN
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif      // UNUSED
// this might not need to be hidden from Doxygen
#if defined ( __linux__ )
# define UNREFERENCED_PARAMETER(x)
#endif
#endif      // DOYGEN

#define UNUSED1(x)

#define MIN_WARN_FIX        //this is a temp define used to test fixes for compile time warnings

#ifndef LINUX_FIX
#define LINUX_FIX           //to track portability changes
#endif      // LINUX_FIX

#define LOG_FILE_SIZE	0xC350	// 50K

#if     defined( _WIN32 )
#pragma warning(disable:4201)
#pragma warning(disable:4200)
#pragma warning(disable:4355)

#define PAL_WIN32_RUN       // The code for Win32 only
#define PAL_NON_USED_CODE   // The code is implemented under Win32, but never be used.

// Windows dependent header file(s)
#include <fstream>
#include "PAL_win32header.h"
#include "PAL_typedef.h"

#define HOBART_SYSTEM_PREFIX    "C:\\Program Files\\Hobart\\"
#define NEW_INTERNAL_CONFIG_FILE_NAME       HOBART_SYSTEM_PREFIX "vmTmp\\SC\\config.dat"
#define INSTALLATION_CUSTOM_INTERNAL_CONFIG_FILE_NAME  HOBART_SYSTEM_PREFIX "vmTmp\\SC\\configc.dat"
#define DEFAULT_INTERNAL_CONFIG_FILE_NAME   HOBART_SYSTEM_PREFIX "config.dat"
#define DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME HOBART_SYSTEM_PREFIX "configc.dat"
#define DEBUG_CONFIG_FILE_NAME              HOBART_SYSTEM_PREFIX "configdebug.dat"
#define LOG_INSTALL_FILE                    HOBART_SYSTEM_PREFIX "install.log"
#define RESERVED_SOFTKEY_GRAPHICS_PATH      HOBART_SYSTEM_PREFIX "System\\Flashkey Graphics"
#define DEFAULT_NETWORK_CONFIG_FILE_NAME    HOBART_SYSTEM_PREFIX "bootParams.cfg"
#define APPLAUNCHER_FILE_NAME               _T(HOBART_SYSTEM_PREFIX) _T("appLauncher.exe")
#define SYSTEM_USER_GRAPHICS_ROOT_DIR		"C:\\Program Files\\Hobart"

#define PAL_DATE_FORMAT_DEFAULT             _T("ddd',' MMM dd yy")
#define PAL_DATE_FORMAT_DDMMYY_LOG          _T("ddMMyy")
#define PAL_DATE_FORMAT_YYYYMMDD            _T("yyyy-MM-dd")
#define PAL_DATE_FORMAT_MMDDYYYY            _T("MM-dd-yyyy") 

#define PAL_TIME_FORMAT_DEFAULT             _T("HH':'mm':'ss")
#define PAL_TIME_FORMAT_HHMMSS_LOG          _T("HHmmss")
#define PAL_TIME_FORMAT_HHMMSS              _T(" HH:mm:ss")

#define PAL_DEFAULT_UPGRADE_DIRECTORY		HOBART_SYSTEM_PREFIX "Access Scale\\upgrade"
#define PAL_DEFAULT_DOWNLOAD_DIRECTORY		HOBART_SYSTEM_PREFIX "Download"
#define WERUNTIME_FILE						"C:\\WERUNTIME.INI"

#elif   defined( __linux__ )
// Linux dependent header file(s)

// by including the path here we only have to have
// PAL in the include search path not PAL_linux. We
// could do the same for WIN32, but why bother.
#include "PAL_linux/PAL_linuxheader.h"
#include "PAL_linux/PAL_typedef.h"
#include "PAL_linux/PAL_tmp.h"

#include <unistd.h>

#define HOBART_SYSTEM_PREFIX    "/home/Hobart/"
#define NEW_INTERNAL_CONFIG_FILE_NAME       HOBART_SYSTEM_PREFIX "vmTmp/SC/config.dat"
#define INSTALLATION_CUSTOM_INTERNAL_CONFIG_FILE_NAME  HOBART_SYSTEM_PREFIX "vmTmp/SC/configc.dat"
#define DEFAULT_INTERNAL_CONFIG_FILE_NAME   "/etc/hobart/config_linux.dat"
#define DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME "/etc/hobart/configc.dat"
#define DEBUG_CONFIG_FILE_NAME              "/etc/hobart/configdebug.dat"
#define LOG_FILE_HOME_DIRECTORY				HOBART_SYSTEM_PREFIX "AccessScale/"
#define ACCESS_SCALE_DIRECTORY				HOBART_SYSTEM_PREFIX "AccessScale/"
#define LOG_STARTUP_SHUTDOWN_FILE           LOG_FILE_HOME_DIRECTORY "shutdown.dat"
#define LOG_SCHEDULED_EVENTS_FILE			LOG_FILE_HOME_DIRECTORY "scheduledEvents.log"
#define LOG_MAINTENANCE_EVENTS_FILE			LOG_FILE_HOME_DIRECTORY "dataMaintenance.log"
#define LOG_SCALE_EVENTS_FILE				LOG_FILE_HOME_DIRECTORY "scaleEvents.log"
#define LOG_INFO_FILE						LOG_FILE_HOME_DIRECTORY "info.log"
#define LOG_WARNINGS_FILE					LOG_FILE_HOME_DIRECTORY "warnings.log"
#define LOG_ERRORS_FILE						LOG_FILE_HOME_DIRECTORY "errors.log"
#define OLD_LOG_STARTUP_SHUTDOWN_FILE       LOG_FILE_HOME_DIRECTORY "shutdown1.dat"
#define OLD_LOG_SCHEDULED_EVENTS_FILE		LOG_FILE_HOME_DIRECTORY "scheduledEvents1.log"
#define OLD_LOG_MAINTENANCE_EVENTS_FILE		LOG_FILE_HOME_DIRECTORY "dataMaintenance1.log"
#define OLD_LOG_SCALE_EVENTS_FILE			LOG_FILE_HOME_DIRECTORY "scaleEvents1.log"
#define OLD_LOG_INFO_FILE					LOG_FILE_HOME_DIRECTORY "info1.log"
#define OLD_LOG_WARNINGS_FILE				LOG_FILE_HOME_DIRECTORY "warnings1.log"
#define OLD_LOG_ERRORS_FILE					LOG_FILE_HOME_DIRECTORY "errors1.log"
#define REMOTE_MAINTENANCE_INPUT_DIR		"/home/Hobart/AccessScale/inbox"
#define REMOTE_MAINTENANCE_OUTPUT_DIR		"/home/Hobart/AccessScale/outbox"
#define REMOTE_MAINTENANCE_PROCESSING_DIR	"/home/Hobart/AccessScale/pending"
#define LOG_INSTALL_FILE                    "/var/log/hobart/install.log"
#define RESERVED_SOFTKEY_GRAPHICS_PATH      "/usr/share/hobart/system/flashkey_graphics"
#define DEFAULT_NETWORK_CONFIG_FILE_NAME    "/etc/hobart/bootParams.cfg"
#define APPLAUNCHER_FILE_NAME               _T("/usr/bin/appLauncher")
#define SYSTEM_USER_GRAPHICS_ROOT_DIR		"/usr/local/hobart"

#define PAL_DATE_FORMAT_DEFAULT             _T("ddd, MMM dd yy")
#define PAL_DATE_FORMAT_DDMMYY_LOG          _T("ddMMyy")
#define PAL_DATE_FORMAT_YYYYMMDD            _T("yyyy-MM-dd")
#define PAL_DATE_FORMAT_MMDDYYYY            _T("MM-dd-yyyy") 

#define PAL_TIME_FORMAT_DEFAULT             _T("HH:mm:ss")
#define PAL_TIME_FORMAT_HHMMSS_LOG          _T("HHmmss")
#define PAL_TIME_FORMAT_HHMMSS              _T(" HH:mm:ss")

#define PAL_DEFAULT_UPGRADE_DIRECTORY		HOBART_SYSTEM_PREFIX "AccessScale/upgrade"
#define PAL_DEFAULT_DOWNLOAD_DIRECTORY		HOBART_SYSTEM_PREFIX "Download"
#define WERUNTIME_FILE						"/home/WERUNTIME.INI"

#elif   defined( SOLARIS )
// Here we have SOLARIS header files

#elif   defined( VXWORKS )
// Here we have VXWORKS header files

#else
#error "Unknown system"

#endif


#include "PAL_absclass.h"
#include "PAL_errcode.h"

















#endif//_PAL_PLATFORM_HEADER_H_

