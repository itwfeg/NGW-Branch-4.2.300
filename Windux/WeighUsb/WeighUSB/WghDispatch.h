/*
*   Copyright (c) 2000 Microsoft Corporation
*
* USB device driver for Hobart Weighing system devices.
* Request dispatch, handlers, lower (USBD) interface.
*/

#ifndef _USB_DISPATCH_H
#define _USB_DISPATCH_H

#define REMOTE_WAKEUP_MASK 0x20

typedef struct _USB_RW_CONTEXT {

    PURB              Urb;
    PMDL              Mdl;
    ULONG             Length;
    ULONG             Numxfer;
    ULONG_PTR         VirtualAddress;
    PDEVICE_EXTENSION DeviceExtension;

} USB_RW_CONTEXT, * PUSB_RW_CONTEXT;

PUSB_PIPE_CONTEXT
PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    );

NTSTATUS
DispatchReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
ReadWriteCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );
NTSTATUS
DispatchPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleStartDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleQueryStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleQueryRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleCancelRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleSurpriseRemoval(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleCancelStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleQueryCapabilities(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
ReadandSelectDescriptors(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
ConfigureDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
SelectInterfaces(
    IN PDEVICE_OBJECT                DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );

NTSTATUS
DeconfigureDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB           Urb
    );

VOID
ProcessQueuedRequests(
    IN OUT PDEVICE_EXTENSION DeviceExtension
    );

NTSTATUS
GetRegistryDword(
    IN     PWCHAR RegPath,
    IN     PWCHAR ValueName,
    IN OUT PULONG Value
    );

NTSTATUS
DispatchClean(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
DpcRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
IdleRequestWorkerRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID          Context
    );

NTSTATUS
AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );

NTSTATUS
CanStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
CanRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
ReleaseMemory(
    IN PDEVICE_OBJECT DeviceObject
    );

LONG
Usb_IoIncrement(
    IN OUT PDEVICE_EXTENSION DeviceExtension
    );

LONG
Usb_IoDecrement(
    IN OUT PDEVICE_EXTENSION DeviceExtension
    );

BOOLEAN
CanDeviceSuspend(
    IN PDEVICE_EXTENSION DeviceExtension
    );

PCHAR
PnPMinorFunctionString (
    IN UCHAR MinorFunction
    );

NTSTATUS
DispatchCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
DispatchClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
DispatchDevCtrl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
ResetPipe(
    IN PDEVICE_OBJECT         DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    );

NTSTATUS
ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    );

NTSTATUS
ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
SubmitIdleRequestIrp(
    IN PDEVICE_EXTENSION DeviceExtension
    );

VOID
IdleNotificationCallback(
    IN PDEVICE_EXTENSION DeviceExtension
    );

NTSTATUS
IdleNotificationRequestComplete(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp,
    IN PVOID             Context
    );

VOID
CancelSelectSuspend(
    IN PDEVICE_EXTENSION DeviceExtension
    );

//
// PoRequestPowerIrp callbacks.
// Called when the power IRP has completed.
//
// Parameters are as follows:
//
//  DeviceObject
//  pointer to the target DEVICE_OBJECT for the completed power IRP.
//  MinorFunction
//      Specifies the minor function code in the power IRP.
//  PowerState
//      Specifies the device power state that is passed to PoRequestPowerIrp.
//  Context
//      pointer to the context that is passed to PoRequestPowerIrp.
//  IoStatus
//      pointer to the IO_STATUS_BLOCK structure for the completed IRP.
//
//  For complete documentation, see PoRequestPowerIrp on MSDN.
VOID
PoIrpCompletionFunc(
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

VOID
PoIrpAsyncCompletionFunc(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

VOID
WWIrpCompletionFunc(
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

VOID
WaitWakeCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus);

NTSTATUS
IssueWaitWake(
    IN PDEVICE_EXTENSION DeviceExtension);

VOID
CancelWaitWake(
    IN PDEVICE_EXTENSION DeviceExtension);

#endif
