/*
*   Copyright (c) 2000 Microsoft Corporation
*
* USB device driver for Hobart Weighing system devices.
*
*/

#include <initguid.h>
#include <wdm.h>
#include <wmilib.h>
#include <wmistr.h>
#include "usbdi.h"
#include "usbdlib.h"

#ifndef _WEIGHUSB_H
#define _WEIGHUSB_H

#define INTRTAG (ULONG) 'RtnI'

#undef ExAllocatePool
#define ExAllocatePool(type, size) \
    ExAllocatePoolWithTag(type, size, INTRTAG);


enum
{
     DBG_ERROR=2
    ,DBG_WARN
    ,DBG_INFO
    ,DBG_FUNCTION
    ,DBG_VERBOSE
};

// initial value of DebugLevel
#define DEBUGLEVEL DBG_ERROR

// crude approximation of "debug zones"
// set zone<=DEBUGLEVEL to enable msgs
#define DBG_WAITWAKE  DBG_INFO
#define DBG_QUEUE     DBG_INFO
#define DBG_DISPATCH  DBG_INFO
#define DBG_DUMPEXT   DBG_INFO
#define DBG_REFCOUNT  DBG_INFO
#define DBG_POWER     DBG_INFO


extern ULONG DebugLevel;
void _internalDbgPrint(const char* fmt, ...);

#if DBG

#define wDbgPrint(level, _x_) \
            if((level) <= DebugLevel) { \
                _internalDbgPrint  _x_  ; \
            }
#else

#define wDbgPrint(level, _x_)

#endif

typedef struct _GLOBALS {
    UNICODE_STRING Usb_RegistryPath;
} GLOBALS;

#define IDLE_INTERVAL 5000

typedef enum _DEVSTATE {

    NotStarted,
    Stopped,
    Working,
    PendingStop,
    PendingRemove,
    SurpriseRemoved,
    Removed

} DEVSTATE;

typedef enum _QUEUE_STATE {

    HoldRequests,
    AllowRequests,
    FailRequests

} QUEUE_STATE;

typedef enum _WDM_VERSION {

    WinXpOrBetter,
    Win2kOrBetter,
    WinMeOrBetter,
    Win98OrBetter

} WDM_VERSION;

#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DeviceState =  NotStarted;\
        (_Data_)->PrevDevState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PrevDevState =  (_Data_)->DeviceState;\
        (_Data_)->DeviceState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DeviceState =   (_Data_)->PrevDevState;


#define USB_MAX_TRANSFER_SIZE   8192
#define USB_TEST_BOARD_TRANSFER_BUFFER_SIZE (64 *1024 )

#define USB_REGISTRY_PARAMETERS_PATH  \
    L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\WeighingUsb\\Parameters"

typedef struct _USB_PIPE_CONTEXT
{
    BOOLEAN PipeOpen;

} USB_PIPE_CONTEXT, *PUSB_PIPE_CONTEXT;


typedef struct _DEVICE_EXTENSION {

    PDEVICE_OBJECT                  FunctionalDeviceObject;
    PDEVICE_OBJECT                  TopOfStackDeviceObject;
    PDEVICE_OBJECT                  PhysicalDeviceObject;
    UNICODE_STRING                  InterfaceName;
    DEVICE_CAPABILITIES             DeviceCapabilities;
    PUSB_CONFIGURATION_DESCRIPTOR   UsbConfigurationDescriptor;
    PUSB_DEVICE_DESCRIPTOR          UsbDeviceDescriptor;
    PUSBD_INTERFACE_INFORMATION     UsbInterface;
    PUSB_PIPE_CONTEXT               PipeContext;
    DEVSTATE                        DeviceState;
    DEVSTATE                        PrevDevState;
    KSPIN_LOCK                      DevStateLock;
    SYSTEM_POWER_STATE              SysPower;
    DEVICE_POWER_STATE              DevPower;
    QUEUE_STATE                     QueueState;
    LIST_ENTRY                      NewRequestsQueue;
    KSPIN_LOCK                      QueueLock;
    KEVENT                          RemoveEvent;
    KEVENT                          StopEvent;
    ULONG                           OutStandingIO;
    KSPIN_LOCK                      IOCountLock;
    PIRP                            InterruptIrp;
    PIRP                            PollingIrp;
    PURB                            PollingUrb;
    UCHAR                           IntData[64];
    BOOLEAN                         pollpending;
    KSPIN_LOCK                      polllock;
    IO_REMOVE_LOCK                  RemoveLock;
    USBD_PIPE_HANDLE                PipeHandle;
    LONG                            SSEnable;
    LONG                            SSRegistryEnable;
    PUSB_IDLE_CALLBACK_INFO         IdleCallbackInfo;
    PIRP                            PendingIdleIrp;
    LONG                            IdleReqPend;
    LONG                            FreeIdleIrpCount;
    KSPIN_LOCK                      IdleReqStateLock;
    KEVENT                          NoIdleReqPendEvent;
    ULONG                           PowerDownLevel;
    PIRP                            WaitWakeIrp;
    LONG                            FlagWWCancel;
    LONG                            FlagWWOutstanding;
    LONG                            FlagWWDispatched;
    LONG                            WaitWakeEnable;
    LONG                            OpenHandleCount;
    KTIMER                          Timer;
    KDPC                            DeferredProcCall;
    KEVENT                          NoDpcWorkItemPendingEvent;
    WDM_VERSION                     WdmVersion;
    BOOLEAN                         DebugDumpFlag;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct _IRP_COMPLETION_CONTEXT
{
    PDEVICE_EXTENSION DeviceExtension;
    PKEVENT Event;

} IRP_COMPLETION_CONTEXT, *PIRP_COMPLETION_CONTEXT;

extern GLOBALS Globals;

#include "WghDispatch.h"
#include "WghIoctl.h"
#include "WghPower.h"

#endif //_WEIGHUSB_H
