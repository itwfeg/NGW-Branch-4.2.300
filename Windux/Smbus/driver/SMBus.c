/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    Incomplete2.c

Abstract:

    Incomplete2.c is the second stage of the Toaster sample function driver. This
    stage builds upon the support implemented in the first stage, Incomplete1.c.

    New features implemented in this stage of the function driver:

    -Dispatch routines to support IRP_MJ_READ, IRP_MJ_WRITE, and
     IRP_MN_DEVICE_CONTROL. These routines are named SMBCreate, SMBClose,
     SMBDispatchIO, and SMBDispatchIoctl.

    -A driver-managed IRP queue to hold IRPs dispatched to the function driver.
     These routines are named SMBQueueRequest and SMBProcessQueuedRequests.

    -A StartDevice routine to start a toaster instance’s device hardware. This
     routine is named SMBStartDevice.

    -Routines to synchronize the processing of IRP_MN_QUERY_STOP_DEVICE,
     IRP_MN_QUERY_REMOVE_DEVICE and IRP_MN_REMOVE_DEVICE when stopping or removing
     hardware instances. These routines are named SMBIoIncrement and
     SMBIoDecrement.

    This code is for learning purposes only. For additional code that supports
    other features, for example, Power Management and Windows Management
    Instrumentation (WMI), see the Featured1 stage.

Environment:

    Kernel mode

Revision History:

    Eliyas Yakub - 10-Oct-2002

    Kevin Shirley - 05-May-2003 - Commented for tutorial and learning purposes

--*/
#include "SMBUS.h"
#include "SMBUSRegs.h"
#include "SMBUSInterface.h"

ULONG DebugLevel = INFO;
#define _DRIVER_NAME_ "Incomplete2"

#ifdef ALLOC_PRAGMA
//
// Add the routines implemented in this stage to the PAGE section.
//
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, SMBAddDevice)
#pragma alloc_text (PAGE, SMBCreate)
#pragma alloc_text (PAGE, SMBClose)
#pragma alloc_text (PAGE, SMBDispatchIoctl)
#pragma alloc_text (PAGE, SMBDispatchPnp)
#pragma alloc_text (PAGE, SMBStartDevice)
#pragma alloc_text (PAGE, SMBUnload)
#pragma alloc_text (PAGE, SMBSendIrpSynchronously)
#pragma alloc_text (PAGE, SMBDispatchPower)
#pragma alloc_text (PAGE, SMBSystemControl)
#endif

 
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Updated Routine Description:
    The dispatch routines implemented in this stage of the function driver are
    connected to the MajorFunction array member of the DriverObject parameter.

--*/
{
    SMBDebugPrint(TRACE, "Entered DriverEntry of "_DRIVER_NAME_"  version "
                                                         "built on " __DATE__" at "__TIME__ "\n");

    DriverObject->DriverExtension->AddDevice           = SMBAddDevice;

    DriverObject->DriverUnload                         = SMBUnload;

	DriverObject->DriverStartIo						   = SMBStartIO;

    DriverObject->MajorFunction[IRP_MJ_PNP]            = SMBDispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = SMBDispatchPower;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = SMBSystemControl;

    //
    // Connect the new dispatch routines implemented in this stage of the function
    // driver that support communication from applications. SMBDispatchIO
    // initially processes read, write, and device control operations. These
    // operations are the result of user-mode calls to ReadFile, WriteFile, or
    // DeviceIoControl.
    //
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = SMBCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = SMBClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SMBDispatchIoctl;

    return STATUS_SUCCESS;
}

 
NTSTATUS
SMBAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Updated Routine Description:
    SMBAddDevice initializes additional members of the device extension,
    including those for the driver-managed IRP queue. The driver-managed IRP queue
    mechanism uses the following three members of the device extension:


    SMBAddDevice also initializes the kernel event members of the device
    extension that are used to synchronize the processing of
    IRP_MN_QUERY_STOP_DEVICE, IRP_MN_QUERY_REMOVE_DEVICE and IRP_MN_REMOVE_DEVICE.
    The mechanism to synchronize the processing of these IRPs uses the
    RemoveEvent, StopEvent, and OutstandingIO members of the device extension.

    The OutstandingIO member keeps the count of uncompleted IRPs that the system
    has dispatched to the function driver. OutstandingIO controls when the
    RemoveEvent and StopEvent kernel events are signaled.

    SMBAddDevice initializes OutstandingIO to 1. That is, when there are zero
    uncompleted IRPs to process, OutstandingIo equals 1, not zero. The function
    driver calls SMBIoIncrement to increment OutstandingIO every time the
    system dispatches a new IRP to the function driver. The function driver calls
    SMBIoDecrement to decrement OutstandingIO when the function driver
    completes a dispatched IRP. The function driver must make an equal number of
    calls to increment and decrement OutstandingIO, except when SMBDispatchPnp
    processes IRP_MN_REMOVE_DEVICE. When the function driver processes
    IRP_MN_REMOVE_DEVICE, the function driver must call SMBIoDecrement twice
    in order to signal RemoveEvent. The extra call to SMBIoDecrement is the
    only time OutstandingIO is decremented to 0.

    SMBDispatchPnp uses StopEvent to synchronize the processing of
    IRP_MN_QUERY_STOP_DEVICE and IRP_MN_QUERY_REMOVE_DEVICE with any other threads
    that might be processing other IRPs in the function driver. Any thread that
    processes IRP_MN_QUERY_STOP_DEVICE or IRP_MN_QUERY_REMOVE_DEVICE is blocked
    from completing its respective IRP until SMBIoDecrement signals StopEvent.

    SMBDispatchPnp uses RemoveEvent to synchronize the processing of
    IRP_MN_REMOVE_DEVICE with any other threads that might be processing other IRPs
    in the function driver. Any thread that processes IRP_MN_REMOVE_DEVICE is
    blocked from completing the IRP until SMBIoDecrement signals RemoveEvent.

--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PFDO_DATA               fdoData;

    //
    // Call the PAGED_CODE macro because this routine must only execute at
    // IRQL = PASSIVE_LEVEL. The macro halts the system in the checked build
    // of Windows if IRQL >= DISPATCH_LEVEL, because then the Dispatcher cannot
    // be invoked, causing a system crash.
    //
    PAGED_CODE();

    SMBDebugPrint(TRACE, "AddDevice PDO (0x%p)\n", PhysicalDeviceObject);

    status = IoCreateDevice (DriverObject,
                             sizeof (FDO_DATA),
                             NULL,
                             FILE_DEVICE_UNKNOWN,
                             FILE_DEVICE_SECURE_OPEN,
                             FALSE,
                             &deviceObject);

    if (!NT_SUCCESS (status))
    {
        return status;
    }

    SMBDebugPrint(INFO, "AddDevice FDO (0x%p)\n", deviceObject);

    fdoData = (PFDO_DATA) deviceObject->DeviceExtension;

    fdoData->Signature = TOASTER_FDO_INSTANCE_SIGNATURE;

    INITIALIZE_PNP_STATE(fdoData);

    fdoData->UnderlyingPDO = PhysicalDeviceObject;


    fdoData->Self = deviceObject;
    fdoData->NextLowerDriver = NULL;



    deviceObject->Flags |= DO_POWER_PAGABLE;
    deviceObject->Flags |= DO_BUFFERED_IO;

    fdoData->NextLowerDriver = IoAttachDeviceToDeviceStack (deviceObject,
                                                       fdoData->UnderlyingPDO);
    if(NULL == fdoData->NextLowerDriver)
    {
        IoDeleteDevice(deviceObject);

        return STATUS_NO_SUCH_DEVICE;
    }

	fdoData->retryInterval = RtlConvertLongToLargeInteger( POLLING_INTERVAL_MICRO_SECS * -10);
	KeInitializeTimer(
		&fdoData->retryTimer);
	KeInitializeDpc (
		&fdoData->retryDPC,
		SMBRetryTimerDPC,
		(PVOID)fdoData->Self);


    status = IoRegisterDeviceInterface (
                PhysicalDeviceObject,
                (LPGUID) &GUID_DEVINTERFACE_SMBUS,
                NULL,
                &fdoData->InterfaceName);

    if (!NT_SUCCESS (status))
    {
        SMBDebugPrint(ERROR,
            "AddDevice: IoRegisterDeviceInterface failed (%x)\n", status);
        IoDetachDevice(fdoData->NextLowerDriver);
        IoDeleteDevice (deviceObject);
        return status;
    }

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return status;
}

 
VOID
SMBUnload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Updated Routine Description:
    SMBUnload does not change in this stage of the function driver.

--*/
{
    PAGED_CODE();

    ASSERT(NULL == DriverObject->DeviceObject);

    SMBDebugPrint(TRACE, "unload\n");

    return;
}

 
NTSTATUS
SMBDispatchPnp (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Updated Routine Description:
    SMBDispatchPnp calls SMBIoIncrement when the system dispatches a PnP
    IRP to it. SMBDispatchPnp later calls SMBIoDecrement when it completes
    the IRP. SMBDispatchPnp also updates QueueState to control the operation
    of the driver-managed IRP queue.

    When the system dispatches a new IRP to SMBDispatchPnp to be processed,
    SMBDispatchPnp calls SMBIoIncrement to increment the count of
    uncompleted IRPs. When SMBDispatchPnp later completes the IRP, it calls
    SMBIoDecrement to decrement the count of uncompleted IRPs.
    SMBDispatchPnp only calls SMBIoDecrement once during the lifetime of
    the IRP, except with processing IRP_MN_REMOVE_DEVICE. When SMBDispatchPnp
    processes IRP_MN_REMOVE_DEVICE, it must call SMBIoDecrement two times in
    order to signal RemoveEvent.

    SMBDispatchPnp also processes the PnP IRPs that were only processed in a
    limited capacity in the previous stage of the function driver, including:
    -IRP_MN_QUERY_STOP_DEVICE
    -IRP_MN_CANCEL_STOP_DEVICE
    -IRP_MN_STOP_DEVICE
    -IRP_MN_QUERY_REMOVE_DEVICE
    -IRP_MN_CANCEL_REMOVE_DEVICE

    Some of the PnP IRPs are completed directly in the switch case statement, and
    then immediately return to the caller. Other PnP IRPs break out of the switch
    case to be completed at the end of SMBDispatchPnp.

--*/
{
    PFDO_DATA               fdoData;
    PIO_STACK_LOCATION      stack;
    NTSTATUS                status = STATUS_SUCCESS;
    PPNP_DEVICE_STATE       deviceState;

    PAGED_CODE();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    stack = IoGetCurrentIrpStackLocation (Irp);

    SMBDebugPrint(TRACE, "FDO %s \n",
                PnPMinorFunctionString(stack->MinorFunction));


    if (Deleted == fdoData->DevicePnPState)
    {
        Irp->IoStatus.Status = STATUS_NO_SUCH_DEVICE;

        IoCompleteRequest (Irp, IO_NO_INCREMENT);

        return STATUS_NO_SUCH_DEVICE;
    }

    switch (stack->MinorFunction)
    {
    case IRP_MN_START_DEVICE:
        status = SMBSendIrpSynchronously(fdoData->NextLowerDriver, Irp);

        if (NT_SUCCESS (status))
        {
            //
            // After SMBSendIrpSynchronously returns, the underlying bus driver
            // has completed IRP_MN_START_DEVICE. The IRP now contains the data
            // required by the function driver to start the hardware instance, such
            // as the I/O ports, I/O memory ranges, and interrupts to use.
            //
            // Start the toaster instance’s hardware. The code that started the
            // hardware in the previous stage of the function driver has been moved
            // into the SMBStartDevice routine.
            //
            status = SMBStartDevice (fdoData, Irp);
        }

        //
        // Break out of the switch case. The function driver stopped processing the
        // IRP in SMBDispatchPnpComplete where the IRP’s status was set to
        // STATUS_MORE_PROCESSING_REQUIRED. SMBDispatchPnp completes the IRP
        // outside the switch case.
        //
        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        SET_NEW_PNP_STATE(fdoData, StopPending);

        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        return status;

   case IRP_MN_CANCEL_STOP_DEVICE:
        //
        // The underlying bus driver must process IRP_MN_CANCEL_STOP_DEVICE before
        // the function driver can because the bus driver must reallocate the same
        // hardware resources that the toaster instance was using before the function
        // driver received and processed IRP_MN_QUERY_STOP_DEVICE.
        // 
        // The function driver calls SMBSendIrpSynchronously to have the bus
        // driver process the IRP before the function driver processes it.
        // SMBSendIrpSynchronously passes the IRP down the device stack to be
        // processed by the bus driver, and does not return to the caller until the
        // bus driver completes the IRP.
        //
        status = SMBSendIrpSynchronously(fdoData->NextLowerDriver,Irp);

        if(NT_SUCCESS(status))
        {
            RESTORE_PREVIOUS_PNP_STATE(fdoData);

 
            //
            // Test the assumption that the previous hardware state equaled Started.
            // The previous hardware state should be Started because that should have
            // been the state when SMBDispatchPnp processed
            // IRP_MN_QUERY_STOP_DEVICE.
            //
            ASSERT(Started == fdoData->DevicePnPState);


        }
        else
        {
            //
            // If SMBSendIrpSynchronously returns a failure that is because
            // a lower driver in the device stack failed IRP_MN_CANCEL_STOP_DEVICE.
            // That is a fatal error.
            //
            ASSERTMSG("Cancel stop failed. Fatal error!", FALSE);
            SMBDebugPrint(WARNING, "Failure statute = 0x%x\n", status);
        }

        break;

    case IRP_MN_STOP_DEVICE:
        //
        // If the function driver is unable to stop the hardware instance without
        // losing any data when it processed an earlier IRP_MN_QUERY_STOP_DEVICE,
        // then the function driver should have failed IRP_MN_QUERY_STOP_DEVICE
        // with STATUS_UNSUCCESSFUL. 
        //

        SET_NEW_PNP_STATE(fdoData, Stopped);

        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        return status;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        SET_NEW_PNP_STATE(fdoData, RemovePending);


        SMBDebugPrint(INFO, "Query - remove holding requests...\n");


        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        return status;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        //
        // The underlying bus driver must process IRP_MN_CANCEL_REMOVE_DEVICE before
        // the function driver can because the bus driver must reallocate the same
        // hardware resources that the toaster instance was using before the function
        // driver received and processed IRP_MN_QUERY_REMOVE_DEVICE.
        // 
        // The function driver calls SMBSendIrpSynchronously to have the bus
        // driver process the IRP before the function driver processes it.
        // SMBSendIrpSynchronously passes the IRP down the device stack to be
        // processed by the bus driver, and does not return to the caller until the
        // bus driver completes the IRP.
        //
        status = SMBSendIrpSynchronously(fdoData->NextLowerDriver,Irp);

        if(NT_SUCCESS(status))
        {
            RESTORE_PREVIOUS_PNP_STATE(fdoData);


        }
        else
        {
            //
            // If SMBSendIrpSynchronously returns a failure that is because
            // a lower driver in the device stack failed IRP_MN_CANCEL_STOP_DEVICE.
            // That is a fatal error.
            //
            ASSERTMSG("Cancel remove failed. Fatal error!", FALSE);
            SMBDebugPrint(WARNING, "Failure status = 0x%x\n", status);
        }

        break;

   case IRP_MN_SURPRISE_REMOVAL:
        SET_NEW_PNP_STATE(fdoData, SurpriseRemovePending);



        status = IoSetDeviceInterfaceState(&fdoData->InterfaceName, FALSE);

        if (!NT_SUCCESS (status))
        {
            SMBDebugPrint(ERROR, 
                "IoSetDeviceInterfaceState failed: 0x%x\n", status);
        }

        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        return status;

   case IRP_MN_REMOVE_DEVICE:
        //
        // If the function driver is unable to stop the hardware instance without
        // losing any data when it processed an earlier IRP_MN_QUERY_REMOVE_DEVICE,
        // then the function driver should have failed IRP_MN_QUERY_REMOVE_DEVICE
        // with STATUS_UNSUCCESSFUL
        //

        SET_NEW_PNP_STATE(fdoData, Deleted);

        if(SurpriseRemovePending != fdoData->PreviousPnPState)
        {
 
            status = IoSetDeviceInterfaceState(&fdoData->InterfaceName, FALSE);

            if (!NT_SUCCESS (status))
            {
                SMBDebugPrint(ERROR,
                        "IoSetDeviceInterfaceState failed: 0x%x\n", status);
            }
        }


        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        IoDetachDevice (fdoData->NextLowerDriver);

        RtlFreeUnicodeString(&fdoData->InterfaceName);

        IoDeleteDevice (fdoData->Self);

        return status;

    default:
        //
        // Pass all unprocessed IRPs down the device stack. All PnP IRPs, as well as
        // all power and WMI IRPSs, must be passed down the device stack so that the
        // underlying bus driver can process them, regardless of whether the function
        // driver processes them.
        //

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (fdoData->NextLowerDriver, Irp);

        return status;
    }

    //
    // The IRP_MN_START_DEVICE, IRP_MN_CANCEL_STOP_DEVICE, and
    // IRP_MN_CANCEL_REMOVE_DEVICE switch cases break out of the switch case to
    // have their respective IRPs completed here. Set the IRP’s status to the value
    // returned from SMBSendIrpSynchronously.
    //
    Irp->IoStatus.Status = status;

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    //
    // Return the status value returned from SMBSendIrpSynchronously to the
    // caller.
    //
    return status;
}

 
NTSTATUS
SMBDispatchPnpComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Updated Routine Description:
    SMBDispatchPnpComplete does not change in this stage of the function
    driver.

--*/
{
    PKEVENT             event = (PKEVENT)Context;

    UNREFERENCED_PARAMETER (DeviceObject);

    if (TRUE == Irp->PendingReturned)
    {
        KeSetEvent (event, IO_NO_INCREMENT, FALSE);
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}

 
 
NTSTATUS
SMBCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

New Routine Description:
    The system dispatches IRP_MJ_CREATE IRPs to SMBCreate. SMBCreate
    processes user-mode CreateFile calls.

    The presence of a driver-implemented routine in the IRP_MN_CREATE MajorFunction
    array entry of the DriverObject parameter in DriverEntry allows the system to
    successfully return a handle to the hardware instance to the CreateFile call.
    Without a driver implemented routine in this array entry, the system fails
    any calls to CreateFile.

    A DispatchCreate routine for PnP hardware is not usually required to perform
    any special tasks to create and return a handle to the caller, except return
    STATUS_SUCCESS to the caller.

    In the case of the Toaster sample function driver, it is not necessary to
    access the toaster instance’s hardware to process a create request. The
    hardware state does not need to be changed, and the driver-managed IRP queue
    does not need to be manipulated.

Parameters Description:
    DeviceObject
    DeviceObject represents the hardware instance that is associated with the
    incoming Irp parameter. DeviceObject is a FDO created earlier in
    SMBAddDevice.

    Irp
    Irp describes the handle creation operation to perform on the hardware
    instance described by the DeviceObject parameter.

Return Value Description:
    SMBCreate returns STATUS_NO_SUCH_DEVICE if the hardware instance
    represented by DeviceObject has been removed. Otherwise SMBCreate returns
    STATUS_SUCCESS to indicate that the create handle operation succeeded.

--*/
{
    PFDO_DATA    fdoData;

    PAGED_CODE();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    SMBDebugPrint(TRACE, "Create \n");


    if (Deleted == fdoData->DevicePnPState)
    {
        Irp->IoStatus.Status = STATUS_NO_SUCH_DEVICE;

        IoCompleteRequest (Irp, IO_NO_INCREMENT);

        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Set the IRP’s IoStatus.Information member with the number of bytes transferred
    // during the create operation. Because it is not necessary to access the toaster
    // instance’s hardware to process the create request, the number of bytes
    // transferred is 0.
    //
    Irp->IoStatus.Information = 0;

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

 
NTSTATUS
SMBClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

New Routine Description:
    The system dispatches IRP_MJ_CLOSE IRPs to SMBClose. SMBClose
    processes user-mode CloseHandle calls.

    The presence of a driver implemented routine in the IRP_MN_CLOSE MajorFunction
    array entry of the DriverObject parameter in DriverEntry allows the system to
    successfully close a handle to the hardware instance to the CloseHandle call.
    Without a driver implemented routine in this array entry, the system fails
    any calls to CloseHandle.

    The DispatchClose routine for PnP hardware is not usually required to perform
    any special tasks to close a handle and return to the caller, except return
    STATUS_SUCCESS to the caller.

    In the case of the Toaster sample function driver, it is not necessary to
    access the toaster instance’s hardware to process a close request. The
    hardware state does not need to be changed, and the driver-managed IRP queue
    does not need to be manipulated.

    Note that SMBClose does not check to determine if the hardware instance
    has been deleted. On Windows 2000 and later, the system does not send
    IRP_MJ_CLOSE after it sends IRP_MN_REMOVE_DEVICE (although it is possible that
    a 3rd party driver higher in the device stack could fake one). However,
    Windows 9x does send IRP_MJ_CLOSE after it sends IRP_MN_REMOVE_DEVICE.

    In addition, Windows 2000 sends IRP_MN_SURPRISE_REMOVAL and then waits for all
    handles to the hardware instance to close before it sends IRP_MN_REMOVE_DEVICE.
    However, Windows 9x immediately sends IRP_MN_REMOVE_DEVICE, even if there are
    open handles to the hardware instance. Therefore, to prevent leaking memory,
    SMBClose does not check if the hardware instance has been deleted, and
    proceeds to close all handles even after the hardware instance has been
    deleted.

Parameters Description:
    DeviceObject
    DeviceObject represents the hardware instance that is associated with the
    incoming Irp parameter. DeviceObject is a FDO created earlier in
    SMBAddDevice.

    Irp
    Irp describes the close handle operation to perform on the hardware instance
    described by the DeviceObject parameter.

Return Value Description:
    SMBClose returns STATUS_SUCCESS to indicate the close handle operation
    succeeded. Close IRPs can be pended but cannot be failed.

--*/
{
    PFDO_DATA    fdoData;
    NTSTATUS     status;

    PAGED_CODE();

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    SMBDebugPrint(TRACE, "Close \n");

    status = STATUS_SUCCESS;

    Irp->IoStatus.Information = 0;

    Irp->IoStatus.Status = status;

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}

 
NTSTATUS
SMBDispatchIoctl(
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++

New Routine Description:
    SMBDispatchIO dispatches IRP_MJ_DEVICE_CONTROL IRPs to
    SMBDispatchIoctl. SMBDispatchIoctl processes user-mode DeviceIoControl
    calls.

    This stage of the function driver does not implement support for any IOCTLs.
    The Featured1 stage of the function driver implements support to hide the
    hardware instance from displaying in the Device Manager, unless the
    "Show hidden devices" menu item is checked.

Parameters Description:
    DeviceObject
    DeviceObject represents the hardware instance that is associated with the
    incoming Irp parameter. DeviceObject is a FDO created earlier in
    SMBAddDevice.

    Irp    
    Irp describes the device control operation to perform on the hardware instance
    described by the DeviceObject parameter, such as to change the operating mode
    of the hardware instance.

Return Value Description:
    SMBDispatchIoctl returns STATUS_INVALID_DEVICE_REQUEST for all incoming
    device control operations in this stage of the function driver.

--*/
{
    PIO_STACK_LOCATION      stack;
    NTSTATUS                status= STATUS_SUCCESS;
    PFDO_DATA               fdoData;
	ULONG					inSize = 0;
	ULONG					outSize = 0;
    PAGED_CODE();

    SMBDebugPrint(TRACE, "Ioctl called\n");

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    //
    // Get the parameters of the IRP from the function driver’s location in the IRP’s
    // I/O stack. The results of the function driver’s processing of the IRP, if any,
    // are then stored back in the same I/O stack location.
    // 
    // A device control’s parameters are stored in the IRP’s
    // Parameters.DeviceIoControl member. Drivers can change the members of the
    // device control operation as the IRP is processed and passed down and back up
    // the device stack, if necessary.
    //
    stack = IoGetCurrentIrpStackLocation (Irp);

	inSize = stack->Parameters.DeviceIoControl.InputBufferLength;
	outSize = stack->Parameters.DeviceIoControl.OutputBufferLength;

	SMBDebugPrint(INFO,
		"SMBDispatchIoctl: control code: %x\n", stack->Parameters.DeviceIoControl.IoControlCode);
	SMBDebugPrint(INFO,
		"SMBDispatchIoctl: insize: %x\n", inSize);
	SMBDebugPrint(INFO,
		"SMBDispatchIoctl: outSize: %x\n", outSize);

    switch (stack->Parameters.DeviceIoControl.IoControlCode)
    {

	case IOCTL_SMBUS_BYTE_READ:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			if (inSize < sizeof (SMBUS_IOCTL_HEADER))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{

				pInBuffer->length = 1;
				if (outSize < pInBuffer->length)
				{
					status = STATUS_INVALID_BUFFER_SIZE;

				}
				else
				{
					pInBuffer->command = 0;
					pInBuffer->type = IOCTL_TYPE_BYTE_READ;

					IoMarkIrpPending (Irp);
					IoStartPacket (DeviceObject, Irp, 0, NULL);
					return STATUS_PENDING;

				}
			}
		}
		break;
	case IOCTL_SMBUS_BYTE_DATA_READ:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			if (inSize < sizeof (SMBUS_IOCTL_HEADER))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				pInBuffer->length = 1;
				if (outSize < pInBuffer->length)
				{
					status = STATUS_INVALID_BUFFER_SIZE;

				}
				else
				{
					pInBuffer->type = IOCTL_TYPE_BYTE_DATA_READ;

					IoMarkIrpPending (Irp);
					IoStartPacket (DeviceObject, Irp, 0, NULL);
					return STATUS_PENDING;

				}
			}
		}
		break;
	case IOCTL_SMBUS_WORD_DATA_READ:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			if (inSize < sizeof (SMBUS_IOCTL_HEADER))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				pInBuffer->length = 2;
				if (outSize < pInBuffer->length)
				{
					status = STATUS_INVALID_BUFFER_SIZE;

				}
				else
				{
					pInBuffer->type = IOCTL_TYPE_WORD_DATA_READ;

					IoMarkIrpPending (Irp);
					IoStartPacket (DeviceObject, Irp, 0, NULL);
					return STATUS_PENDING;

				}
			}
		}
		break;
	case IOCTL_SMBUS_BLOCK_DATA_READ:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;
			if (inSize < sizeof (SMBUS_IOCTL_HEADER))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				if (outSize < pInBuffer->length)
				{
					status = STATUS_INVALID_BUFFER_SIZE;

				}
				else
				{
					pInBuffer->type = IOCTL_TYPE_BLOCK_DATA_READ;

					IoMarkIrpPending (Irp);
					IoStartPacket (DeviceObject, Irp, 0, NULL);
					return STATUS_PENDING;

				}
			}
		}
		break;


	case IOCTL_SMBUS_QUICK_WRITE:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			pInBuffer->length = 0;
			if (inSize < (sizeof (SMBUS_IOCTL_HEADER) + pInBuffer->length))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				pInBuffer->command = 0;
				pInBuffer->type = IOCTL_TYPE_QUICK_WRITE;

				IoMarkIrpPending (Irp);
				IoStartPacket (DeviceObject, Irp, 0, NULL);
				return STATUS_PENDING;

			}
		}
		break;
	case IOCTL_SMBUS_BYTE_WRITE:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			pInBuffer->length = 1;
			if (inSize < (sizeof (SMBUS_IOCTL_HEADER) + pInBuffer->length))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				
				pInBuffer->type = IOCTL_TYPE_BYTE_WRITE;

				IoMarkIrpPending (Irp);
				IoStartPacket (DeviceObject, Irp, 0, NULL);
				return STATUS_PENDING;

			}
		}
		break;
	case IOCTL_SMBUS_BYTE_DATA_WRITE:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			pInBuffer->length = 1;
			if (inSize < (sizeof (SMBUS_IOCTL_HEADER) + pInBuffer->length))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				
				pInBuffer->type = IOCTL_TYPE_BYTE_DATA_WRITE;

				IoMarkIrpPending (Irp);
				IoStartPacket (DeviceObject, Irp, 0, NULL);
				return STATUS_PENDING;

			}
		}
		break;
	case IOCTL_SMBUS_WORD_DATA_WRITE:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			pInBuffer->length = 2;
			if (inSize < (sizeof (SMBUS_IOCTL_HEADER) + pInBuffer->length))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				
				pInBuffer->type = IOCTL_TYPE_WORD_DATA_WRITE;

				IoMarkIrpPending (Irp);
				IoStartPacket (DeviceObject, Irp, 0, NULL);
				return STATUS_PENDING;

			}
		}
		break;
	case IOCTL_SMBUS_BLOCK_DATA_WRITE:
		{
			PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)Irp->AssociatedIrp.SystemBuffer;

			if (inSize < (sizeof (SMBUS_IOCTL_HEADER) + pInBuffer->length))
			{
				status = STATUS_INVALID_BUFFER_SIZE;
			}
			else
			{
				
				pInBuffer->type = IOCTL_TYPE_BLOCK_DATA_WRITE;

				IoMarkIrpPending (Irp);
				IoStartPacket (DeviceObject, Irp, 0, NULL);
				return STATUS_PENDING;

			}
		}
		break;
    default:
        //
        // Fail all device control requests in this stage of the function driver.
        //
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    Irp->IoStatus.Status = status;

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}

 

NTSTATUS
SMBStartDevice (
    IN PFDO_DATA     FdoData,
    IN PIRP             Irp
    )
/*++

New Routine Description:
    SMBDispatchPnp calls SMBStartDevice after the underlying bus driver
    has processed IRP_MN_START_DEVICE. SMBStartDevice performs the operations
    that were processed by SMBDispatchPnp in the IRP_MN_START_DEVICE switch
    case in the Incomplete1 stage of the function driver, such as enabling the
    device interface instance, and updating the hardware state.

    After SMBStartDevice has performed whatever initialization is needed to
    start the hardware instance, it calls SMBProcessQueuedRequests to process
    any IRPs that the system might have dispatched to the function driver before
    the hardware instance was started.

Parameters Description:
    FdoData
    FdoData represents the device extension of the FDO of the hardware instance to
    start.

    Irp
    Irp represents the IRP_MN_START_DEVICE that now contains the data required by
    the function driver to start the hardware instance, such as I/O port
    resources, I/O memory ranges, or interrupt.

Return Value Description:
    SMBStartDevice returns STATUS_SUCCESS to indicate the device hardware was
    successfully started. Otherwise SMBStartDevice returns the status returned
    by IoSetDeviceInterfaceState if an error occurred.

--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    PIO_STACK_LOCATION      stack;

    PAGED_CODE();

    stack = IoGetCurrentIrpStackLocation (Irp);

    status = IoSetDeviceInterfaceState(&FdoData->InterfaceName, TRUE);

    if (!NT_SUCCESS (status))
    {
        SMBDebugPrint(ERROR, "IoSetDeviceInterfaceState failed: 0x%x\n",
                                status);
        return status;
    }

	SMBGetIoBase(&FdoData->ioBase);

	if (FdoData->ioBase == (PUCHAR) 0)
	{
        SMBDebugPrint(ERROR,
            "SMBStartDevice: SMBGetIoBase failed \n");

        return STATUS_UNSUCCESSFUL;

	}

	status = SMBProbeController (FdoData);
    if (!NT_SUCCESS (status))
    {
        SMBDebugPrint(ERROR,
            "SMBStartDevice: SMBProbeController failed (%x)\n", status);

        return status;
    }

	SMBResetController (FdoData);

	SMBDebugPrint(INFO, "SMBUS reg0 a:(0x%p) d:(0x%X)\n", FdoData->ioBase, READ_PORT_SDA(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg1 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 1, READ_PORT_STS(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg2 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 2, READ_PORT_CSTS(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg3 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 3, READ_PORT_CTRL1(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg4 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 4, READ_PORT_ADDR(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg5 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 5, READ_PORT_CTRL2(FdoData));
	SMBDebugPrint(INFO, "SMBUS reg6 a:(0x%p) d:(0x%X)\n", FdoData->ioBase + 6, READ_PORT_CTRL3(FdoData));


    SET_NEW_PNP_STATE(FdoData, Started);

 
    return status;
}

 
NTSTATUS
SMBSendIrpSynchronously (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Updated Routine Description:
    SMBSendIrpSynchronously does not change in this stage of the function
    driver.

--*/
{
    KEVENT   event;
    NTSTATUS status;

    PAGED_CODE();

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);

    IoSetCompletionRoutine(Irp,
                           SMBDispatchPnpComplete,
                           &event,
                           TRUE,
                           TRUE,
                           TRUE
                           );

    status = IoCallDriver(DeviceObject, Irp);

    if (STATUS_PENDING == status)
    {
       KeWaitForSingleObject(&event,
                             Executive,
                             KernelMode,
                             FALSE,
                             NULL
                             );
       status = Irp->IoStatus.Status;
    }

    return status;
}

 
NTSTATUS
SMBDispatchPower (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Updated Routine Description:
    SMBDispatchPower calls SMBIoIncrement to increment OutstandingIO.
    SMBDispatchPower calls SMBIoDecrement to decrement OutstandingIO after
    it completes the incoming power IRP.

    SMBDispatchPower fails the incoming power IRP if it determines that the
    hardware instance has been removed.

    Power IRPs are always system-managed and must not be queued in the
    driver-managed IRP queue. A driver must never queue power IRPs. Otherwise, the
    system could stop responding.

    The function driver must immediately process power IRPs and then pass them
    down the device stack to be processed by the underlying bus driver. If the
    function driver cannot immediately process a power IRP, then it should mark
    the power IRP as pending, change the driver-managed IRP queue to begin
    queuing any new non-power IRPs, and then return STATUS_PENDING to the system
    until the function driver completes the pending power IRP.

    This stage of the function driver does not implement support pending power
    IRPs. The Featured1 stage of the function driver implements support for
    pending power IRPs.

Updated Return Value Description:
    SMBDispatchPower returns STATUS_NO_SUCH_DEVICE if the hardware instance
    represented by DeviceObject has been removed. Otherwise, SMBDispatchPower
    returns the value returned by the underlying bus driver.

--*/
{
    PFDO_DATA           fdoData;
    NTSTATUS            status;

    PAGED_CODE();

    SMBDebugPrint(TRACE, "Entered SMBDispatchPower\n");

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    if (Deleted == fdoData->DevicePnPState)
    {
        PoStartNextPowerIrp (Irp);

        status = STATUS_NO_SUCH_DEVICE;

        Irp->IoStatus.Status = status;

        IoCompleteRequest (Irp, IO_NO_INCREMENT);

        return status;
    }

    PoStartNextPowerIrp(Irp);

    IoSkipCurrentIrpStackLocation(Irp);

    status = PoCallDriver(fdoData->NextLowerDriver, Irp);
    
    return status;
}

 
NTSTATUS
SMBSystemControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++

Updated Routine Description:
    SMBSystemControl calls SMBIoIncrement to increment OutstandingIO.
    SMBSystemControl calls SMBIoDecrement to decrement OutstandingIO after
    it completes the incoming power IRP.

    SMBSystemControl fails the incoming WMI IRP if it determines that the
    hardware instance has been removed.

Updated Return Value Description:
    SMBSystemControl returns STATUS_NO_SUCH_DEVICE if the hardware instance
    represented by DeviceObject has been removed. Otherwise, SMBSystemControl
    returns the value returned by the underlying bus driver.

--*/
{
    PFDO_DATA               fdoData;
    NTSTATUS                status;

    PAGED_CODE();

    SMBDebugPrint(TRACE, "FDO received WMI IRP\n");

    fdoData = (PFDO_DATA) DeviceObject->DeviceExtension;

    if (Deleted == fdoData->DevicePnPState)
    {
        status = STATUS_NO_SUCH_DEVICE;

        Irp->IoStatus.Status = status;

        IoCompleteRequest (Irp, IO_NO_INCREMENT);

        return status;
    }

    IoSkipCurrentIrpStackLocation (Irp);

    status = IoCallDriver (fdoData->NextLowerDriver, Irp);

    return status;
}

void SMBResetController (IN PFDO_DATA fdoData)
{
	UCHAR tmp = 0;

	WRITE_PORT_CTRL2(fdoData, 0x70);//freq 16 clock cycles
	WRITE_PORT_CTRL1(fdoData, 0); //turn off interrupts.
	WRITE_PORT_ADDR(fdoData, 0);//disable slave addresses.

	//enable smbus
	tmp = READ_PORT_CTRL2(fdoData);
	tmp |= BITMASK_SMBUS_CTRL2_ENABLE;
	WRITE_PORT_CTRL2(fdoData, tmp);

	//free stall after start
	tmp = READ_PORT_CTRL1(fdoData);
	tmp &= ~(BITMASK_SMBUS_CTRL1_STASTRE | BITMASK_SMBUS_CTRL1_NMINTE);
	WRITE_PORT_CTRL1(fdoData, tmp);

	//send a stop.
	tmp = READ_PORT_CTRL1(fdoData);
	tmp |= BITMASK_SMBUS_CTRL1_STOP;
	WRITE_PORT_CTRL1(fdoData, tmp);

	//clear BER, NGACK, and STASTR bits
	WRITE_PORT_STS(fdoData, BITMASK_SMBUS_STS_BER|BITMASK_SMBUS_STS_NEGACK|BITMASK_SMBUS_STS_STASTR);

	//clear bb bit
	tmp = READ_PORT_CSTS(fdoData);
	tmp |= BITMASK_SMBUS_CSTS_BB;
	WRITE_PORT_CSTS(fdoData, tmp);
}

NTSTATUS SMBProbeController (IN PFDO_DATA fdoData)
{
	UCHAR tmp = 0;

	WRITE_PORT_CTRL2(fdoData, 0x70);
	tmp = READ_PORT_CTRL2(fdoData);
	if (tmp != 0x70)
	{
		SMBDebugPrint(INFO,
				"SMBProbeController: readback failed\n");

		return STATUS_UNSUCCESSFUL;
	}
	
	tmp = READ_PORT_CTRL1(fdoData);
	tmp |= BITMASK_SMBUS_CTRL1_NMINTE;
	WRITE_PORT_CTRL1(fdoData, tmp);

	tmp = READ_PORT_CTRL1(fdoData);
	if (tmp)
	{
		SMBDebugPrint(INFO,
				"SMBProbeController: disabled. ctrl1=0x%02x\n", tmp);
		return STATUS_UNSUCCESSFUL;
	}

	tmp = READ_PORT_CTRL2(fdoData);
	tmp |= BITMASK_SMBUS_CTRL2_ENABLE;
	WRITE_PORT_CTRL2(fdoData, tmp);

	tmp = READ_PORT_CTRL1(fdoData);
	tmp |= BITMASK_SMBUS_CTRL1_NMINTE;
	WRITE_PORT_CTRL1(fdoData, tmp);

	tmp = READ_PORT_CTRL1(fdoData);
	if ((tmp & BITMASK_SMBUS_CTRL1_NMINTE) != BITMASK_SMBUS_CTRL1_NMINTE)
	{
		SMBDebugPrint(INFO,
				"SMBProbeController: enabled but NMINTE wont be set. ctrl1=0x%02x\n", tmp);
		return STATUS_UNSUCCESSFUL;
	}

	SMBDebugPrint(INFO,
				"SMBProbeController: probe successfull\n");
	return STATUS_SUCCESS;

}

VOID SMBDoState (IN PFDO_DATA fdoData)
{
	UCHAR tmp = 0;

	switch (fdoData->smbState)
	{
	case SMBUS_STATE_IDLE:
		SMBDebugPrint(INFO,
				"SMBDoState: State idle\n");
		break;
	case SMBUS_STATE_ADDRESS:
		{
			UCHAR addr = fdoData->deviceAddress & ~1;
			SMBDebugPrint(INFO,
					"SMBDoState: State address = %x\n", addr);
			WRITE_PORT_SDA(fdoData, addr);

			fdoData->smbState = SMBUS_STATE_COMMAND;
		}
		break;
	case SMBUS_STATE_COMMAND:
		SMBDebugPrint(INFO,
				"SMBDoState: State command = %x\n", fdoData->command);
		WRITE_PORT_SDA(fdoData, fdoData->command);

		if (fdoData->deviceAddress & 0x01)
		{
			fdoData->smbState = SMBUS_STATE_REPEAT_START;
		}
		else
		{
			fdoData->smbState = SMBUS_STATE_WRITE;
		}

		break;

	case SMBUS_STATE_REPEAT_START:
		{
			SMBDebugPrint(INFO,
					"SMBDoState: State repeat start!\n");

			tmp = READ_PORT_CTRL1(fdoData);
			tmp |= BITMASK_SMBUS_CTRL1_START;
			WRITE_PORT_CTRL1(fdoData, tmp);

		}//fall trhough
	case SMBUS_STATE_QUICK:
		{
			SMBDebugPrint(INFO,
					"SMBDoState: State quick\n");

			if (fdoData->deviceAddress & 0x01)
			{
				if (fdoData->processIrpCounter == 1)
				{
					tmp = READ_PORT_CTRL1(fdoData);
					tmp |= BITMASK_SMBUS_CTRL1_ACK;
					WRITE_PORT_CTRL1(fdoData, tmp);					
				}
				else
				{
					tmp = READ_PORT_CTRL1(fdoData);
					tmp &= ~BITMASK_SMBUS_CTRL1_ACK;
					WRITE_PORT_CTRL1(fdoData, tmp);	

				}

				fdoData->smbState = SMBUS_STATE_READ;
			}
			else
			{

				fdoData->smbState = SMBUS_STATE_WRITE;

			}

			SMBDebugPrint(INFO,
					"SMBDoState: address = %x\n", fdoData->deviceAddress);

			WRITE_PORT_SDA(fdoData, fdoData->deviceAddress);
		}
		break;

	case SMBUS_STATE_READ:
		{
			SMBDebugPrint(INFO,
					"SMBDoState: State read\n" );

			if (fdoData->processIrpCounter == 0)
			{

				fdoData->smbState = SMBUS_STATE_IDLE;

				tmp = READ_PORT_CTRL1(fdoData);
				tmp |= BITMASK_SMBUS_CTRL1_STOP;
				WRITE_PORT_CTRL1(fdoData, tmp);
			}
			else
			{
				if (fdoData->processIrpCounter == 1)
				{
					SMBDebugPrint(INFO,
							"SMBDoState: Negack\n" );

					tmp = READ_PORT_CTRL1(fdoData);
					tmp |= BITMASK_SMBUS_CTRL1_ACK;
					WRITE_PORT_CTRL1(fdoData, tmp);					
				}
				else
				{
					tmp = READ_PORT_CTRL1(fdoData);
					tmp &= ~BITMASK_SMBUS_CTRL1_ACK;
					WRITE_PORT_CTRL1(fdoData, tmp);	

				}

				*fdoData->processIrpPtr = READ_PORT_SDA(fdoData);

				SMBDebugPrint(INFO,
						"SMBDoState: val = %x\n", *fdoData->processIrpPtr);

				fdoData->processIrpPtr++;
				fdoData->processIrpCounter--;

			}
		}
		break;
	case SMBUS_STATE_WRITE:
		{
			SMBDebugPrint(INFO,
					"SMBDoState: State write\n");

			if (fdoData->processIrpCounter == 0)
			{

				fdoData->smbState = SMBUS_STATE_IDLE;

				tmp = READ_PORT_CTRL1(fdoData);
				tmp |= BITMASK_SMBUS_CTRL1_STOP;
				WRITE_PORT_CTRL1(fdoData, tmp);


			}
			else
			{
				SMBDebugPrint(INFO,
					"SMBDoState: val = %x\n", *fdoData->processIrpPtr);

				WRITE_PORT_SDA(fdoData, *fdoData->processIrpPtr);
				fdoData->processIrpPtr++;
				fdoData->processIrpCounter--;
			}

		}
		break;
	}
}

VOID SMBAttemptState (IN PDEVICE_OBJECT pDevObj)
{
	PFDO_DATA fdoData = (PFDO_DATA) pDevObj->DeviceExtension;
	PIRP pIrp = pDevObj->CurrentIrp;
	UCHAR tmp = 0, status = 0;

	status = READ_PORT_STS(fdoData);

	if (status & BITMASK_SMBUS_STS_BER) 
	{
		fdoData->smbState = SMBUS_STATE_IDLE;

		fdoData->error = SMBUS_ERROR_BUSERROR;
		SMBDebugPrint(ERROR,
				"SMBAttemptState: bus error.\n");

		SMBResetController(fdoData);

	}
	else if (status & BITMASK_SMBUS_STS_NEGACK)
	{

		fdoData->smbState = SMBUS_STATE_IDLE;
		fdoData->error = SMBUS_ERROR_NEGACK;

		tmp = READ_PORT_CTRL1(fdoData);
		tmp |= BITMASK_SMBUS_CTRL1_STOP;
		WRITE_PORT_CTRL1(fdoData, tmp);

		WRITE_PORT_STS(fdoData, BITMASK_SMBUS_STS_NEGACK | BITMASK_SMBUS_STS_STASTR);

		SMBDebugPrint(ERROR,
				"SMBAttemptState: NEGACK error detected.\n");

	}
	else if (!(status & BITMASK_SMBUS_STS_SDAST))
	{
		fdoData->attemptCounter++;

		if (fdoData->attemptCounter >= 100)
		{
			fdoData->smbState = SMBUS_STATE_IDLE;
			fdoData->error = SMBUS_ERROR_TIMEOUT;

			SMBDebugPrint(ERROR,
					"SMBAttemptState: Timeout error detected.\n");

			SMBResetController(fdoData);

		}

	}
	else if (!(status & BITMASK_SMBUS_STS_MASTER))
	{
		fdoData->smbState = SMBUS_STATE_IDLE;
		fdoData->error = SMBUS_ERROR_NOTMASTER;
		SMBDebugPrint(ERROR,
				"SMBAttemptState: Not master error.\n");

		SMBResetController(fdoData);

	}
	else
	{
		fdoData->attemptCounter = 0;
		SMBDoState(fdoData);

	}

	SMBCompleteState(pDevObj);

}

VOID SMBCompleteState (IN PDEVICE_OBJECT pDevObj)
{
	PFDO_DATA fdoData = (PFDO_DATA) pDevObj->DeviceExtension;
	PIRP pIrp = pDevObj->CurrentIrp;

	if (fdoData->smbState == SMBUS_STATE_IDLE)
	{

		SMBDebugPrint(INFO,
				"SMBCompleteState: completing irp\n");

		if (fdoData->error == SMBUS_ERROR_NONE)
		{
			pIrp->IoStatus.Status = STATUS_SUCCESS;
			pIrp->IoStatus.Information = fdoData->origProcessIrpCounter;
			IoCompleteRequest( pIrp, IO_NO_INCREMENT);
			IoStartNextPacket( pDevObj, FALSE);
		}
		else if ( (fdoData->error == SMBUS_ERROR_NEGACK) && (fdoData->negAckCntr < 10))
		{
			KeSetTimer (&fdoData->retryTimer, fdoData->retryInterval, &fdoData->retryDPC);
		}
		else
		{
			pIrp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			pIrp->IoStatus.Information = 0;
			IoCompleteRequest( pIrp, IO_NO_INCREMENT);
			IoStartNextPacket( pDevObj, FALSE);
		}
	}
	else
	{
		SMBDebugPrint(INFO,
				"SMBCompleteState: pDevExt->attemptCounter = %d\n", fdoData->attemptCounter);
	}
}

VOID SMBRetryTimerDPC (
    IN PKDPC pDpc,
	IN PVOID pContext,
	IN PVOID pSysArg1,
	IN PVOID pSysArg2)
{
	UCHAR tmp = 0;
	PDEVICE_OBJECT pDevObj = (PDEVICE_OBJECT) pContext;
	PFDO_DATA pDevExt = (PFDO_DATA) pDevObj->DeviceExtension;

	pDevExt->negAckCntr++;

	pDevExt->error = SMBUS_ERROR_NONE;
	pDevExt->attemptCounter = 0;
	pDevExt->processIrpPtr = pDevExt->origProcessIrpPtr;
	pDevExt->processIrpCounter = pDevExt->origProcessIrpCounter;
	pDevExt->smbState = pDevExt->origSmbState;

	SMBDebugPrint(INFO,
		"SMBRetryTimerDPC: retry cntr = : %x\n", pDevExt->negAckCntr);

	tmp = READ_PORT_CTRL1(pDevExt);
	tmp |= BITMASK_SMBUS_CTRL1_START;
	WRITE_PORT_CTRL1(pDevExt, tmp);

	while (pDevExt->smbState != SMBUS_STATE_IDLE)
		SMBAttemptState(pDevObj);

}

VOID SMBStartIO (
    IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	PFDO_DATA pDevExt = (PFDO_DATA) pDevObj->DeviceExtension;
	PSMBUS_IOCTL_HEADER pInBuffer = (PSMBUS_IOCTL_HEADER)pIrp->AssociatedIrp.SystemBuffer;
	PSMBUS_IOCTL_WRITE pWriteBuffer = (PSMBUS_IOCTL_WRITE)pIrp->AssociatedIrp.SystemBuffer;
	UCHAR tmp;

	pDevExt->negAckCntr = 0;
	pDevExt->attemptCounter = 0;
	pDevExt->error = SMBUS_ERROR_NONE;
	pDevExt->origProcessIrpCounter = pDevExt->processIrpCounter = pInBuffer->length;
	pDevExt->deviceAddress = pInBuffer->deviceAddress;
	pDevExt->command = pInBuffer->command;

	switch (pInBuffer->type)
	{

	case IOCTL_TYPE_QUICK_WRITE:
		pDevExt->smbState = SMBUS_STATE_QUICK;
		pDevExt->processIrpPtr = NULL;
		break;
	case IOCTL_TYPE_BYTE_READ:
		pDevExt->deviceAddress |= 0x01;
		pDevExt->smbState = SMBUS_STATE_QUICK;
		pDevExt->processIrpPtr = pIrp->AssociatedIrp.SystemBuffer;//buffered method
		break;
	case IOCTL_TYPE_BYTE_WRITE:
		pDevExt->smbState = SMBUS_STATE_QUICK;	
		pDevExt->processIrpPtr = &pInBuffer->command;

		SMBDebugPrint(INFO,
			"SMBStartIO: write buffer: %x\n", pDevExt->processIrpPtr[0]);

		break;
	case IOCTL_TYPE_BYTE_DATA_READ:
		pDevExt->deviceAddress |= 0x01;
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = pIrp->AssociatedIrp.SystemBuffer;
		break;
	case IOCTL_TYPE_BYTE_DATA_WRITE:
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = &pWriteBuffer->body[0];

		SMBDebugPrint(INFO,
			"SMBStartIO: write buffer: %x\n", pDevExt->processIrpPtr[0]);

		break;
	case IOCTL_TYPE_WORD_DATA_READ:
		pDevExt->deviceAddress |= 0x01;
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = pIrp->AssociatedIrp.SystemBuffer;
		break;
	case IOCTL_TYPE_WORD_DATA_WRITE:
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = &pWriteBuffer->body[0];

		SMBDebugPrint(INFO,
			"SMBStartIO: write buffer: %x:%x\n", pDevExt->processIrpPtr[0], pDevExt->processIrpPtr[1]);

		break;
	case IOCTL_TYPE_BLOCK_DATA_READ:
		pDevExt->deviceAddress |= 0x01;
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = pIrp->AssociatedIrp.SystemBuffer;
		break;
	case IOCTL_TYPE_BLOCK_DATA_WRITE:
		pDevExt->smbState = SMBUS_STATE_ADDRESS;
		pDevExt->processIrpPtr = &pWriteBuffer->body[0];

		SMBDebugPrint(INFO,
			"SMBStartIO: write buffer: %x:%x\n", pDevExt->processIrpPtr[0], pDevExt->processIrpPtr[1]);

		break;
	}

	pDevExt->origProcessIrpPtr = pDevExt->processIrpPtr;
	pDevExt->origSmbState = pDevExt->smbState;

	SMBDebugPrint(INFO,
		"SMBStartIO: irp type: %x\n", pInBuffer->type);
	SMBDebugPrint(INFO,
		"SMBStartIO: irp length: %x\n", pDevExt->processIrpCounter);
	SMBDebugPrint(INFO,
		"SMBStartIO: irp deviceAddress: %x\n", pDevExt->deviceAddress);
	SMBDebugPrint(INFO,
		"SMBStartIO: irp command: %x\n", pDevExt->command);
	SMBDebugPrint(INFO,
		"SMBStartIO: state: %x\n", pDevExt->smbState);


	tmp = READ_PORT_CTRL1(pDevExt);
	tmp |= BITMASK_SMBUS_CTRL1_START;
	WRITE_PORT_CTRL1(pDevExt, tmp);

	//JRB
	while (pDevExt->smbState != SMBUS_STATE_IDLE)
		SMBAttemptState(pDevObj);

	SMBDebugPrint(INFO,
		"SMBStartIO: Packet complete!\n");

}

VOID SMBGetIoBase(PUCHAR * pIoBase)
{
PCI_SLOT_NUMBER      slotData;
PCI_COMMON_CONFIG    pciData;
unsigned int        busNumber = 0;

//amd companion chip 5536 pci to isa bridge is on bus=0, device=15, fun=0
	slotData.u.bits.Reserved = 0;
	slotData.u.bits.DeviceNumber = 15;
	slotData.u.bits.FunctionNumber = 0;

    if (HalGetBusData( PCIConfiguration,
                       busNumber,                  
                       slotData.u.AsULONG,
                      &pciData,
                       sizeof(PCI_COMMON_CONFIG)))
    {
		*pIoBase = (PUCHAR)(pciData.u.type0.BaseAddresses[0] & ~3);//it is an io address
	}
	else
		*pIoBase = 0;

	SMBDebugPrint(INFO,
		"SMBGetIoBase: iobase: %x\n", *pIoBase);
}


VOID
SMBDebugPrint    (
    IN ULONG   DebugPrintLevel,
    IN PCCHAR  DebugMessage,
    ...
    )

/*++

Updated Routine Description:
    SMBDebugPrint does not change in this stage of the function driver.

--*/
{
#define     TEMP_BUFFER_SIZE        1024
    va_list    list;
    UCHAR      debugMessageBuffer[TEMP_BUFFER_SIZE];
    NTSTATUS status;
    
    va_start(list, DebugMessage);
    
    if (DebugMessage)
    {
        status = RtlStringCbVPrintfA(debugMessageBuffer, sizeof(debugMessageBuffer), 
                                    DebugMessage, list);
        if(!NT_SUCCESS(status))
        {
            KdPrint ((_DRIVER_NAME_": RtlStringCbVPrintfA failed %x \n", status));
            return;
        }
        if (DebugPrintLevel <= DebugLevel)
        {
            KdPrint ((_DRIVER_NAME_": %s", debugMessageBuffer));
        }        
    }
    va_end(list);

    return;
}

 
PCHAR
PnPMinorFunctionString (
    UCHAR MinorFunction
)
/*++

Updated Routine Description:
    PnPMinorFunctionString does not change in this stage of the function driver.

--*/
{
    switch (MinorFunction)
    {
    case IRP_MN_START_DEVICE:
        return "IRP_MN_START_DEVICE";
    case IRP_MN_QUERY_REMOVE_DEVICE:
        return "IRP_MN_QUERY_REMOVE_DEVICE";
    case IRP_MN_REMOVE_DEVICE:
        return "IRP_MN_REMOVE_DEVICE";
    case IRP_MN_CANCEL_REMOVE_DEVICE:
        return "IRP_MN_CANCEL_REMOVE_DEVICE";
    case IRP_MN_STOP_DEVICE:
        return "IRP_MN_STOP_DEVICE";
    case IRP_MN_QUERY_STOP_DEVICE:
        return "IRP_MN_QUERY_STOP_DEVICE";
    case IRP_MN_CANCEL_STOP_DEVICE:
        return "IRP_MN_CANCEL_STOP_DEVICE";
    case IRP_MN_QUERY_DEVICE_RELATIONS:
        return "IRP_MN_QUERY_DEVICE_RELATIONS";
    case IRP_MN_QUERY_INTERFACE:
        return "IRP_MN_QUERY_INTERFACE";
    case IRP_MN_QUERY_CAPABILITIES:
        return "IRP_MN_QUERY_CAPABILITIES";
    case IRP_MN_QUERY_RESOURCES:
        return "IRP_MN_QUERY_RESOURCES";
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
        return "IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
    case IRP_MN_QUERY_DEVICE_TEXT:
        return "IRP_MN_QUERY_DEVICE_TEXT";
    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
        return "IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
    case IRP_MN_READ_CONFIG:
        return "IRP_MN_READ_CONFIG";
    case IRP_MN_WRITE_CONFIG:
        return "IRP_MN_WRITE_CONFIG";
    case IRP_MN_EJECT:
        return "IRP_MN_EJECT";
    case IRP_MN_SET_LOCK:
        return "IRP_MN_SET_LOCK";
    case IRP_MN_QUERY_ID:
        return "IRP_MN_QUERY_ID";
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
        return "IRP_MN_QUERY_PNP_DEVICE_STATE";
    case IRP_MN_QUERY_BUS_INFORMATION:
        return "IRP_MN_QUERY_BUS_INFORMATION";
    case IRP_MN_DEVICE_USAGE_NOTIFICATION:
        return "IRP_MN_DEVICE_USAGE_NOTIFICATION";
    case IRP_MN_SURPRISE_REMOVAL:
        return "IRP_MN_SURPRISE_REMOVAL";
    case IRP_MN_QUERY_LEGACY_BUS_INFORMATION:
        return "IRP_MN_QUERY_LEGACY_BUS_INFORMATION";
    default:
        return "unknown_pnp_irp";
    }
}

