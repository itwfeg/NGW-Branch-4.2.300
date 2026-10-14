//
// File : PAL_win32header.h
//
// This file, included by PAL_platformheader.h, includes Windows platform depended header files.
//
#ifndef _WIN32_HEADER_H_
#define _WIN32_HEADER_H_

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "BaseNT.h"

#include <dbt.h>                        // WinDDK/inc/wxp
#include <initguid.h>                   // initguid.h & guiddef.h are the same as VS8
#include <setupapi.h>                   // MSFT modified
#include <shellapi.h>                   // WinDDK/inc/wxp
// To prevent windows.h include winsock.h. Either #include <winsock2.h>, or #define _WINSOCKAPI_ in front of windows.h.
#include <winsock2.h>                   // WinDDK/inc/wxp
#include <windows.h>                    // WinDDK/inc/wxp, WinSDK/Include
#include <tchar.h>
#include <windowsx.h>
#include <Wininet.h>                    // Visual Studio
#include <Sensapi.h>
#include <Iphlpapi.h>                   // adapter info
#include <sys/stat.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Sensapi.lib")	












#endif  //_WIN32_HEADER_H_
