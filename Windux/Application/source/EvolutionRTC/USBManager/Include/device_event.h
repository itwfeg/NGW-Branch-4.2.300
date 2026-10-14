#ifndef _HOB_EVENT_H_
#define _HOB_EVENT_H_

#include "PAL_platformheader.h"

#define HOB_DEVICE_ATTACHED     1
#define HOB_DEVICE_DETACHED     0

//
//abstracts platform specific data struture
//required for handling device attachment &
//removal
//
typedef struct EVENT_INFO
{
    public:
        DWORD  event_type;
#ifdef PAL_WIN32_RUN
        HANDLE service;
        LPVOID  event_dat;
#else
        std::string dev_path;
#endif

}*PEVENT_INFO;

//
//abstracts platform specific data struture
//required for retrieving device descriptors
//
struct DEVICE_INFO
{
    public:
#ifdef PAL_WIN32_RUN
        HANDLE dev_info;
        SP_DEVICE_INTERFACE_DATA dev_if_dat;
        SP_DEVINFO_DATA dev_info_dat;
#else
        struct usb_device *dev;
        struct usb_bus *bus;
#endif
};

#endif      //_HOB_EVENT_H_
