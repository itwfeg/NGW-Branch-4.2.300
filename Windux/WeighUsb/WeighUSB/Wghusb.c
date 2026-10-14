/*
*   Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
*
* USB device driver for Hobart Weighing system devices.
* Main driver functions:
*   DriverEntry
*   DriverUnload
*   AddDevice
*/

#include "WghUsb.h"

GLOBALS Globals;
ULONG   DebugLevel = DEBUGLEVEL;


//NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING UniRegistryPath);
//NTSTATUS AddDevice(IN PDRIVER_OBJECT DriverObject, IN PDEVICE_OBJECT PhysicalDeviceObject);
DRIVER_UNLOAD DriverUnload;
DRIVER_ADD_DEVICE AddDevice;
DRIVER_INITIALIZE DriverEntry;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    )

/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to driver object

    RegistryPath - pointer to a unicode string representing the path to driver
                   specific key in the registry.

Return Values:

    NT status value

--*/
{
    NTSTATUS        ntStatus;
    PUNICODE_STRING registryPath;

    PAGED_CODE();

    KdPrint(("WeighUsb.sys: built on %s %s\n",__DATE__, __TIME__));

#if 0 //DBG
    KdBreakPoint();  //_asm int 3
#endif

    registryPath                = &Globals.Usb_RegistryPath;

    //
    // Allocate pool to hold a null-terminated copy of the path.
    // Safe in paged pool since all registry routines execute at
    // PASSIVE_LEVEL.
    //

    registryPath->MaximumLength = UniRegistryPath->Length + sizeof(UNICODE_NULL);
    registryPath->Length        = UniRegistryPath->Length;
    registryPath->Buffer        = ExAllocatePool(PagedPool, registryPath->MaximumLength);

    if (!registryPath->Buffer)
    {
        wDbgPrint(DBG_ERROR, ("Failed to allocate memory for registryPath\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto DriverEntry_Exit;
    }

    RtlZeroMemory (registryPath->Buffer,
                   registryPath->MaximumLength);
    RtlMoveMemory (registryPath->Buffer,
                   UniRegistryPath->Buffer,
                   UniRegistryPath->Length);

    ntStatus = STATUS_SUCCESS;

    //
    // Initialize the driver object with this driver's entry points.
    //
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDevCtrl;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_PNP]            = DispatchPnP;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = DispatchClean;
    DriverObject->MajorFunction[IRP_MJ_READ]           =
    DriverObject->MajorFunction[IRP_MJ_WRITE]          = DispatchReadWrite;
#ifdef ENABLE_WMI_SUPPORT
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = DispatchSysCtrl;
#endif
    DriverObject->DriverUnload                         = DriverUnload;
    DriverObject->DriverExtension->AddDevice           = (PDRIVER_ADD_DEVICE)AddDevice;

DriverEntry_Exit:

    return ntStatus;
}


VOID
DriverUnload(IN PDRIVER_OBJECT DriverObject)
/*++

Description:

    This function is called just before driver module is unloaded.
    Frees the driver globals allocated in DriverEntry .

Arguments:

    DriverObject - pointer to driver object

Return:

    None

--*/
{
    PUNICODE_STRING registryPath;
    PAGED_CODE();

    registryPath = &Globals.Usb_RegistryPath;

    if(registryPath->Buffer)
    {
        wDbgPrint(1, ("DriverUnload: freeing registry buffer at 0x%x\n",registryPath->Buffer));
        ExFreePool(registryPath->Buffer);
        registryPath->Buffer = NULL;
    }
    else
    {
        wDbgPrint(1, ("DriverUnload: error - error, registry buffer NULL\n"));
    }

    wDbgPrint(1, ("DriverUnload: unloaded PDRIVER_OBJECT 0x%x\n",DriverObject));
    return;
}

//
//  AddDevice
//  Called each time a hardware instance is added to the system.
//  Create the device object and device extension.
//  Initialize the device extension members.
//  Register the device interface.
//  Load parameters from the registry.

NTSTATUS
AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Description:

Arguments:

    DriverObject - Store the pointer to the object representing us.

    PhysicalDeviceObject - Pointer to the device object created by the
                           undelying bus driver.

Return:

    STATUS_SUCCESS - if successful
    STATUS_UNSUCCESSFUL - otherwise

--*/
{
    NTSTATUS          ntStatus;
    PDEVICE_OBJECT    deviceObject;
    PDEVICE_EXTENSION deviceExtension;
    POWER_STATE       state;


    deviceObject = NULL;

    ntStatus = IoCreateDevice(
                    DriverObject,                   // our driver object
                    sizeof(DEVICE_EXTENSION),       // extension size for us
                    NULL,                           // name for this device
                    FILE_DEVICE_UNKNOWN,
                    FILE_AUTOGENERATED_DEVICE_NAME, // device characteristics
                    FALSE,                          // Not exclusive
                    &deviceObject);                 // Our device object

    if(!NT_SUCCESS(ntStatus))
    {
        // returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //
        wDbgPrint(1, ("Failed to create device object, %u\n", ntStatus));
        return ntStatus;
    }

    //
    // Initialize the device extension
    //

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
    deviceExtension->FunctionalDeviceObject = deviceObject;
    deviceExtension->PhysicalDeviceObject   = PhysicalDeviceObject;
    deviceObject->Flags |= DO_DIRECT_IO;

    //
    // initialize the device state lock and set the device state
    //

    KeInitializeSpinLock(&deviceExtension->DevStateLock);
    INITIALIZE_PNP_STATE(deviceExtension);

    //
    //initialize OpenHandleCount
    //
    deviceExtension->OpenHandleCount = 0;

    //
    // Initialize the selective suspend variables
    //
    KeInitializeSpinLock(&deviceExtension->IdleReqStateLock);
    deviceExtension->IdleReqPend = 0;
    deviceExtension->PendingIdleIrp = NULL;

    //
    // Hold requests until the device is started
    //

    deviceExtension->QueueState = HoldRequests;

    //
    // Initialize the queue and the queue spin lock
    //

    InitializeListHead(&deviceExtension->NewRequestsQueue);
    KeInitializeSpinLock(&deviceExtension->QueueLock);

    //
    // Initialize the remove event to not-signaled.
    //

    KeInitializeEvent(&deviceExtension->RemoveEvent, SynchronizationEvent, FALSE);

    //
    // Initialize the stop event to signaled.
    // This event is signaled when the OutstandingIO becomes 1
    //

    KeInitializeEvent(&deviceExtension->StopEvent, SynchronizationEvent, TRUE);

    //
    // OutstandingIo count biased to 1.
    // Transition to 0 during remove device means IO is finished.
    // Transition to 1 means the device can be stopped
    //

    deviceExtension->OutStandingIO = 1;
    KeInitializeSpinLock(&deviceExtension->IOCountLock);

#ifdef ENABLE_WMI_SUPPORT
    //
    // Delegating to WMILIB
    //
    ntStatus = WmiRegistration(deviceExtension);

    if(!NT_SUCCESS(ntStatus)) {

        wDbgPrint(1, ("WmiRegistration failed with %X\n", ntStatus));
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }
#endif
    //
    // set the flags as underlying PDO
    //

    if(PhysicalDeviceObject->Flags & DO_POWER_PAGABLE)
    {
        deviceObject->Flags |= DO_POWER_PAGABLE;
    }

    //
    // Typically, the function driver for a device is its
    // power policy owner, although for some devices another
    // driver or system component may assume this role.
    // Set the initial power state of the device, if known, by calling
    // PoSetPowerState.
    //

    deviceExtension->DevPower = PowerDeviceD0;
    deviceExtension->SysPower = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState(deviceObject, DevicePowerState, state);

    deviceExtension->TopOfStackDeviceObject = IoAttachDeviceToDeviceStack(deviceObject,
                                                PhysicalDeviceObject);
    if (NULL == deviceExtension->TopOfStackDeviceObject)
    {
#ifdef ENABLE_WMI_SUPPORT
        WmiDeRegistration(deviceExtension);
#endif
        IoDeleteDevice(deviceObject);
        wDbgPrint(1, ("Failed to attach to device stack, %u\n",STATUS_NO_SUCH_DEVICE));
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Register device interfaces
    //

    ntStatus = IoRegisterDeviceInterface(deviceExtension->PhysicalDeviceObject,
                                         &GUID_CLASS_WEIGHING_USB,
                                         NULL,
                                         &deviceExtension->InterfaceName);

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(1, ("Failed to register device interface, %u\n",ntStatus));
#ifdef ENABLE_WMI_SUPPORT
        WmiDeRegistration(deviceExtension);
#endif
        IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    if(RtlIsNtDdiVersionAvailable(NTDDI_WINXP))
    {
        deviceExtension->WdmVersion = WinXpOrBetter;
    }
    else
    {
        wDbgPrint(1, ("this driver requires Windows XP or higher, aborting\n"));
        return STATUS_UNSUCCESSFUL;
    }

    deviceExtension->SSRegistryEnable = 0;
    deviceExtension->SSEnable = 0;

    //
    // check the registry flag -
    // whether the device should selectively
    // suspend when idle
    //

    GetRegistryDword(USB_REGISTRY_PARAMETERS_PATH, L"WeighingUsbEnable", &deviceExtension->SSRegistryEnable);

    if(deviceExtension->SSRegistryEnable)
    {
        wDbgPrint(1, ("AddDevice: enable flag set in registry. Initializing DPC and related items\n"));
        //
        // initialize DPC
        //
        KeInitializeDpc(&deviceExtension->DeferredProcCall, DpcRoutine, deviceObject);

            //
            // initialize the timer.
            // the DPC and the timer in conjunction,
            // monitor the state of the device to
            // selectively suspend the device.
            //
        KeInitializeTimerEx(&deviceExtension->Timer, NotificationTimer);

            //
            // Initialize the NoDpcWorkItemPendingEvent to signaled state.
            // This event is cleared when a Dpc is fired and signaled
            // on completion of the work-item.
            //
        KeInitializeEvent(&deviceExtension->NoDpcWorkItemPendingEvent, NotificationEvent, TRUE);

            //
            // Initialize the NoIdleReqPendEvent to ensure that the idle request
            // is indeed complete before we unload the drivers.
            //
        KeInitializeEvent(&deviceExtension->NoIdleReqPendEvent, NotificationEvent, TRUE);
    }

    //
    // Clear the DO_DEVICE_INITIALIZING flag.
    // Note: Do not clear this flag until the driver has set the
    // device power state and the power DO flags.
    //

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    wDbgPrint(1, ("AddDevice completed for PDRIVER_OBJECT=0x%x PDEVICE_OBJECT=0x%x\n",DriverObject,PhysicalDeviceObject));

    return ntStatus;
}

#include <stdio.h>
#include <stdarg.h>

void _internalDbgPrint(const char* fmt, ...)
{
    char sout[256];
    va_list argptr;
    size_t cnt = sizeof(sout);

    va_start(argptr, fmt);
    _vsnprintf(sout, cnt, fmt, argptr);
    va_end(argptr);
    DbgPrint("WeighUsb:%s", sout);
}

