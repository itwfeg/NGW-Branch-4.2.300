
#ifndef _USB_USER_H
#define _USB_USER_H

#if defined(PAL_WIN32_RUN)
#include <initguid.h>

#if 0
#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef WORD
#define WORD unsigned int
#endif
#endif

#define DEVICE_SIG_NAME_LEN	6

//#if defined(PAL_WIN32_RUN)	***** WARNING: re-enable while we already have DEFINE_GUID in Linux
// {004965C5-EF0A-4779-B42A-BA8AD7C98E60}
DEFINE_GUID(GUID_CLASS_WEIGHING_USB, 
0x004965c5, 0xef0a, 0x4779, 0xb4, 0x2a, 0xba, 0x8a, 0xd7, 0xc9, 0x8e, 0x60);

DEFINE_GUID(GUID_CLASS_ATMEL_SAMBA, 
0xE6EF7DCD, 0x1795, 0x4A08, 0x9F, 0xBF, 0xAA, 0x78, 0x42, 0x3C, 0x26, 0xF0);

DEFINE_GUID(GUID_CLASS_WINDOWS_DISK_DEVICE,
			0x53F56307, 0xB6BF, 0x11D0, 0x94, 0xF2, 0x00, 0xA0, 0xC9, 0x1E, 0xFB, 0x8B);
//#endif      // PAL_WIN32_RUN

#define USB_IOCTL_INDEX					0x0000

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

#define IOCTL_USB_GET_DEVICE_DESCRIPTOR	CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     USB_IOCTL_INDEX + 3, \
                                                     METHOD_BUFFERED,     \
                                                   FILE_ANY_ACCESS)

#endif

#endif      // PAL_WIN32_RUN
