/*
*   Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
*
* USB device driver for Hobart Weighing system devices.
* Power mgmt request handlers.
*

Module Name:

    WghPower.h

Abstract:

    The power management related processing.

    The Power Manager uses IRPs to direct drivers to change system
    and device power levels, to respond to system wake-up events,
    and to query drivers about their devices. All power IRPs have
    the major function code IRP_MJ_POWER.

    Most function and filter drivers perform some processing for
    each power IRP, then pass the IRP down to the next lower driver
    without completing it. Eventually the IRP reaches the bus driver,
    which physically changes the power state of the device and completes
    the IRP.

    When the IRP has been completed, the I/O Manager calls any
    IoCompletion routines set by drivers as the IRP traveled
    down the device stack. Whether a driver needs to set a completion
    routine depends upon the type of IRP and the driver's individual
    requirements.

    This code is not USB specific. It is essential for every WDM driver
    to handle power irps.

Environment:

    Kernel mode

--*/

#ifndef _USB_POWER_H
#define _USB_POWER_H

typedef struct _POWER_COMPLETION_CONTEXT {
    PDEVICE_OBJECT DeviceObject;
    PIRP           SIrp;
} POWER_COMPLETION_CONTEXT, *PPOWER_COMPLETION_CONTEXT;

typedef struct _WORKER_THREAD_CONTEXT {
    PDEVICE_OBJECT DeviceObject;
    PIRP           Irp;
    PIO_WORKITEM   WorkItem;
} WORKER_THREAD_CONTEXT, *PWORKER_THREAD_CONTEXT;

IO_WORKITEM_ROUTINE HoldIoRequestsWorkerRoutine;
DRIVER_CANCEL CancelQueued;
IO_COMPLETION_ROUTINE SysPoCompletionRoutine;
IO_COMPLETION_ROUTINE WaitWakeCompletionRoutine;
IO_COMPLETION_ROUTINE FinishDevPoUpIrp;
IO_COMPLETION_ROUTINE FinishDevPoDnIrp;
DRIVER_DISPATCH DispatchPower;

NTSTATUS
HandleSystemQueryPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleSystemSetPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleDeviceQueryPower(
    PDEVICE_OBJECT DeviceObject,
    PIRP           Irp
    );

VOID
SendDeviceIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
DevPoCompletionRoutine(
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

NTSTATUS
HandleDeviceSetPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );


NTSTATUS
SetDeviceFunctional(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp,
    IN PDEVICE_EXTENSION DeviceExtension
    );


NTSTATUS
HoldIoRequests(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
QueueRequest(
    IN OUT PDEVICE_EXTENSION DeviceExtension,
    IN PIRP                  Irp
    );

NTSTATUS
IssueWaitWake(
    IN PDEVICE_EXTENSION DeviceExtension
    );

VOID
CancelWaitWake(
    IN PDEVICE_EXTENSION DeviceExtension
    );

VOID
WaitWakeCallback(
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

PCHAR
PowerMinorFunctionString (
    IN UCHAR MinorFunction
    );

#endif

