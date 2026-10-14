//
// File : linux_guid.cpp
//
// GUID definition is a little tricky in Win32
// In other files, we just have it's definition. This file defines its body.
//

#include "PAL_platformheader.h"

#if defined( __linux__ )

#if defined(DEFINE_GUID)
#undef  DEFINE_GUID
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name \
    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

// {004965C5-EF0A-4779-B42A-BA8AD7C98E60}
DEFINE_GUID(GUID_CLASS_WEIGHING_USB, 
0x4965c5, 0xef0a, 0x4779, 0xb4, 0x2a, 0xba, 0x8a, 0xd7, 0xc9, 0x8e, 0x60);

DEFINE_GUID(GUID_CLASS_ATMEL_SAMBA, 
0xE6EF7DCD, 0x1795, 0x4A08, 0x9F, 0xBF, 0xAA, 0x78, 0x42, 0x3C, 0x26, 0xF0);

DEFINE_GUID(GUID_CLASS_WINDOWS_DISK_DEVICE,
			0x53F56307, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B);

#endif
