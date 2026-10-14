// Header file included by ANTLR UniLexer
//sjjc #include <tchar.h>
#ifndef _BASENT_H_
#define _BASENT_H_

#if defined ( _WIN32 )
//sjjc typedef unsigned __int8	UINT8;  // already defined in BASETSD.H
//sjjc typedef unsigned __int16	UINT16; // already defined in BASETSD.H

#if _MSC_VER == 1100
typedef unsigned __int32	UINT32; // already defined in BASETSD.H
#endif
//sjjc typedef unsigned __int64	UINT64; // already defined in BASETSD.H

// Need to define the following directives in the project
// For intel machines
//		_X86_
//		_WIN32_WINNT=0x0400
//		_WIN32_LEAN_AND_MEAN


// For Alphas 
//		_WIN32_WINNT=0x0400
//		_ALPHA_
//		_WIN32_LEAN_AND_MEAN

// Disable warnings due to the length of names
//		these warnings are generated when using STL as the identifiers used are
//		ofter greater than 256 characters.
#pragma warning(disable : 4786)
#pragma warning(disable : 4996)

// we don't build Dlls
#pragma warning(disable: 4275)
//#pragma warning(disable : 4503)

// Win32 and WinSock API
// Make sure that the MMI code does not pull this in.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

//	#include "CSTLChecker.h"

#ifdef _MEMORY_CHECK_
#include "memorycheck.h"
#else
#ifndef _WINDOWS_
#include <windows.h>
#if _MSC_VER > 1100
#include <basetsd.h>
#endif
#endif
#endif



#endif
#endif
