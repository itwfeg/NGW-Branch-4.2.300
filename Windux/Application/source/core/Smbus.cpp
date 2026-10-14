/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

//
//  Device I/O Control mechanism is different from Win & Linux
//  It should be better to rewirte function instead of wrappering Win32 APIs
//
#if defined(PAL_WIN32_RUN)
    #include    "Smbus_win.cpp"

#elif   defined( __linux__ )
    #include    "Smbus_linux.cpp"

#else
    #error  "Implement Smbus functions for system"

#endif
