
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__97CDB548_328F_11D2_B97C_00805FA39725__INCLUDED_)
#define AFX_STDAFX_H__97CDB548_328F_11D2_B97C_00805FA39725__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// MSVC2010
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Header file included by ANTLR UniLexer
#include <tchar.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__97CDB548_328F_11D2_B97C_00805FA39725__INCLUDED_)
