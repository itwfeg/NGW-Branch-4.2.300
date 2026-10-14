#if !defined SUsbDescriptors_h
#define SUsbDescriptors_h

#if defined(LINUX_FIX)
#include <string>
#endif

#if defined(PAL_WIN32_RUN)
#include "../../3RDPartySoftware/WinDDK/inc/wxp/usb100.h" // MSVC2010
#elif   defined( __linux__ )
#include <usb.h>
#include <libusb-1.0/libusb.h>
#endif
#include <vector>

struct SUsbDescriptors
{
    std::string  descHolder;
    std::string  fullPath;

#if defined(PAL_WIN32_RUN)
    GUID guid;
    PUSB_DEVICE_DESCRIPTOR                  deviceDesc;
    PUSB_CONFIGURATION_DESCRIPTOR           configDesc;
    PUSB_INTERFACE_DESCRIPTOR               interfaceDesc;
    std::vector<PUSB_ENDPOINT_DESCRIPTOR>   endpointDescs;
#elif   defined( __linux__ )
    libusb_device              *dev;
    libusb_device_descriptor    dev_desc;
    libusb_config_descriptor   *dev_config;
#endif
    char buf[1024];
};

#endif //#if !defined SUsbDescriptors_h
