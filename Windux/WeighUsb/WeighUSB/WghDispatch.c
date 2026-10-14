/*
*   Copyright (c) 2000 Microsoft Corporation
*
* USB device driver for Hobart Weighing system devices.
* Request dispatch, handlers, lower (USBD) interface, power mgmt.
*
*/

#include "WghUsb.h"

#if DBG

void DUMP_DEVICE_EXTENSION(PDEVICE_EXTENSION ext)
{
    if (ext->DebugDumpFlag)
    {
        wDbgPrint(DBG_DUMPEXT, ("device extension @ 0x%x\n",(ULONG)ext));
        wDbgPrint(DBG_DUMPEXT, ("   TopOfStackDeviceObject       0x%08x\n",ext->TopOfStackDeviceObject     ));
        wDbgPrint(DBG_DUMPEXT, ("   PhysicalDeviceObject         0x%08x\n",ext->PhysicalDeviceObject       ));
        //wDbgPrint(DBG_DUMPEXT, ("   UsbConfigurationDescriptor   0x%08x\n",ext->UsbConfigurationDescriptor ));
        //wDbgPrint(DBG_DUMPEXT, ("   UsbDeviceDescriptor          0x%08x\n",ext->UsbDeviceDescriptor        ));
        //wDbgPrint(DBG_DUMPEXT, ("   UsbInterface                 0x%08x\n",ext->UsbInterface               ));
        //wDbgPrint(DBG_DUMPEXT, ("   PipeContext                  0x%08x\n",ext->PipeContext                ));
        wDbgPrint(DBG_DUMPEXT, ("   QueueState                   0x%08x\n",(ULONG)ext->QueueState                 ));
        wDbgPrint(DBG_DUMPEXT, ("   OutStandingIO                0x%08x\n",ext->OutStandingIO              ));
        //wDbgPrint(DBG_DUMPEXT, ("   InterruptIrp                 0x%08x\n",ext->InterruptIrp               ));
        //wDbgPrint(DBG_DUMPEXT, ("   PollingIrp                   0x%08x\n",ext->PollingIrp                 ));
        //wDbgPrint(DBG_DUMPEXT, ("   PollingUrb                   0x%08x\n",ext->PollingUrb                 ));
        //wDbgPrint(DBG_DUMPEXT, ("   PipeHandle                   0x%08x\n",ext->PipeHandle                 ));
        //wDbgPrint(DBG_DUMPEXT, ("   IdleCallbackInfo             0x%08x\n",ext->IdleCallbackInfo           ));
        wDbgPrint(DBG_DUMPEXT, ("   PendingIdleIrp               0x%08x\n",ext->PendingIdleIrp             ));
        //wDbgPrint(DBG_DUMPEXT, ("   IdleReqPend                  0x%08x\n",ext->IdleReqPend                ));
        //wDbgPrint(DBG_DUMPEXT, ("   FreeIdleIrpCount             0x%08x\n",ext->FreeIdleIrpCount           ));
        //wDbgPrint(DBG_DUMPEXT, ("   WaitWakeIrp                  0x%08x\n",ext->WaitWakeIrp                ));
        wDbgPrint(DBG_DUMPEXT, ("   FlagWWCancel                 0x%08x\n",ext->FlagWWCancel               ));
        wDbgPrint(DBG_DUMPEXT, ("   FlagWWOutstanding            0x%08x\n",ext->FlagWWOutstanding          ));
        //wDbgPrint(DBG_DUMPEXT, ("   WaitWakeEnable               0x%08x\n",ext->WaitWakeEnable             ));
        wDbgPrint(DBG_DUMPEXT, ("   OpenHandleCount              0x%08x\n",ext->OpenHandleCount            ));
    }
}

void VERIFY_DEVICE_EXTENSION(PDEVICE_EXTENSION ext)
{
    if (ext == NULL)
    {
        wDbgPrint(DBG_ERROR, ("VERIFY_DEVICE_EXTENSION: Device extension is NULL\n"));
        KdBreakPoint();
        return;
    }
    if (SurpriseRemoved == ext->DeviceState)
    {
        ext->DebugDumpFlag = TRUE;
        wDbgPrint(DBG_ERROR, ("VERIFY_DEVICE_EXTENSION: DeviceState=SurpriseRemoved\n"));
        if (FailRequests != ext->QueueState)
        {
            wDbgPrint(DBG_ERROR, ("VERIFY_DEVICE_EXTENSION: QueueState!=FailRequests in SurpriseRemoved state\n"));
        }
        DUMP_DEVICE_EXTENSION(ext);
    }
    else
    {
        if (FailRequests == ext->QueueState)
        {
            wDbgPrint(DBG_ERROR, ("VERIFY_DEVICE_EXTENSION: QueueState=FailRequests, but not in SurpriseRemoved state\n"));
            DUMP_DEVICE_EXTENSION(ext);
        }
    }
}


#else
  #define VERIFY_DEVICE_EXTENSION(ext)
  #define DUMP_DEVICE_EXTENSION(ext)
#endif

PUSB_PIPE_CONTEXT
PipeWithName(   IN PDEVICE_OBJECT  DeviceObject,
                IN PUNICODE_STRING FileName)
/*++

Routine Description:

    This routine will pass the string pipe name and
    fetch the pipe number.

Arguments:

    DeviceObject - pointer to DeviceObject
    FileName - string pipe name

Return Value:

    The device extension maintains a pipe context for
    the pipes on the USB device.
    This routine returns the pointer to this context in
    the device extension for the "FileName" pipe.

--*/
{
    LONG                  ix;
    ULONG                 uval;
    ULONG                 nameLength;
    ULONG                 umultiplier;
    PDEVICE_EXTENSION     deviceExtension;
    PUSB_PIPE_CONTEXT     pipeContext;

    wDbgPrint(DBG_FUNCTION, ("PipeWithName: device object 0x%x\n",DeviceObject));
    //
    // initialize variables
    //
    pipeContext = NULL;
    //
    // typedef WCHAR *PWSTR;
    //
    nameLength = (FileName->Length / sizeof(WCHAR));
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    if(nameLength != 0)
    {
        wDbgPrint(DBG_INFO, ("PipeWithName: Filename '%ws' nameLength %u\n", FileName->Buffer, nameLength));

        //
        // Parse the pipe#
        //
        ix = nameLength - 1;

        // if last char isn't digit, decrement it.
        while( (ix > -1)
        && (    (FileName->Buffer[ix] < (WCHAR) '0')
             || (FileName->Buffer[ix] > (WCHAR) '9')) )
        {
            ix--;
        }

        if(ix > -1)
        {
            uval = 0;
            umultiplier = 1;

            // traversing least to most significant digits.
            while(  (ix > -1)
              && (FileName->Buffer[ix] >= (WCHAR) '0')
              && (FileName->Buffer[ix] <= (WCHAR) '9'))
            {
                uval += (umultiplier * (ULONG)(FileName->Buffer[ix] - (WCHAR) '0'));
                ix--;
                umultiplier *= 10;
            }
        }

        if(uval < 6 && deviceExtension->PipeContext)
            pipeContext = &deviceExtension->PipeContext[uval];
    }
    else
    {

    }


    wDbgPrint(DBG_FUNCTION, ("PipeWithName - ends\n"));
    return pipeContext;
}

NTSTATUS
DispatchReadWrite(  IN PDEVICE_OBJECT DeviceObject,
                    IN PIRP           Irp)
/*++

Routine Description:

    Dispatch routine for read and write.
    This routine creates a BULKUSB_RW_CONTEXT for a read/write.
    This read/write is performed in stages of BULKUSB_MAX_TRANSFER_SIZE.
    once a stage of transfer is complete, then the irp is circulated again,
    until the requested length of tranfer is performed.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
    PMDL                    mdl;
    PURB                    urb;
    ULONG                   totalLength;
    ULONG                   stageLength;
    ULONG                   urbFlags;
    BOOLEAN                 read;
    NTSTATUS                ntStatus;
    ULONG_PTR               virtualAddress;
    PFILE_OBJECT            fileObject;
    PDEVICE_EXTENSION       deviceExtension;
    PIO_STACK_LOCATION      irpStack;
    PIO_STACK_LOCATION      nextStack;
    PUSB_RW_CONTEXT         rwContext;
    PUSBD_PIPE_INFORMATION  pipeInformation;


    //
    // initialize variables
    //

    urb = NULL;
    mdl = NULL;
    rwContext = NULL;
    totalLength = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fileObject = irpStack->FileObject;
    read = (irpStack->MajorFunction == IRP_MJ_READ) ? TRUE : FALSE;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if ( deviceExtension->DeviceState == SurpriseRemoved
        || deviceExtension->DeviceState == Removed)
    {
        ntStatus = STATUS_DEVICE_NOT_CONNECTED;
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite (%c): device object 0x%x disconnected, fail with 0x%0x\n",read ? 'R':'W',DeviceObject, ntStatus));
        goto DispatchReadWrite_Exit;
    }

    if(deviceExtension->DeviceState != Working)
    {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite (%c): device object 0x%x invalid device state %u, fail with 0x%0x\n",read ? 'R':'W',DeviceObject,deviceExtension->DeviceState, ntStatus));
        goto DispatchReadWrite_Exit;
    }

    wDbgPrint(DBG_FUNCTION, ("DispatchReadWrite (%c): device object 0x%x irp 0x%x\n",read ? 'R':'W',DeviceObject,Irp));
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // It is true that the client driver cancelled the selective suspend
    // request in the dispatch routine for create Irps.
    // But there is no guarantee that it has indeed completed.
    // so wait on the NoIdleReqPendEvent and proceed only if this event
    // is signalled.
    //

    //
    // make sure that the selective suspend request has been completed.
    //

    if(deviceExtension->SSEnable)
    {
        wDbgPrint(DBG_INFO, ("DispatchReadWrite: Waiting on NoIdleReqPendEvent\n"));
        KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }

    if(fileObject && fileObject->FsContext)
    {
        pipeInformation = fileObject->FsContext;

        if((UsbdPipeTypeBulk != pipeInformation->PipeType) &&
           (UsbdPipeTypeInterrupt != pipeInformation->PipeType))
        {
            wDbgPrint(DBG_ERROR, ("DispatchReadWrite: pipe type not bulk or intr, returning STATUS_INVALID_HANDLE\n"));
            ntStatus = STATUS_INVALID_HANDLE;
            goto DispatchReadWrite_Exit;
        }
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: Invalid handle, returning STATUS_INVALID_HANDLE\n"));
        ntStatus = STATUS_INVALID_HANDLE;
        goto DispatchReadWrite_Exit;
    }

    rwContext = (PUSB_RW_CONTEXT)ExAllocatePool(NonPagedPool, sizeof(USB_RW_CONTEXT));

    if(rwContext == NULL)
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: Failed to alloc mem for rwContext, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto DispatchReadWrite_Exit;
    }

    if(Irp->MdlAddress)
        totalLength = MmGetMdlByteCount(Irp->MdlAddress);

    if(totalLength > USB_TEST_BOARD_TRANSFER_BUFFER_SIZE)
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: Transfer length > circular buffer, returning STATUS_INVALID_PARAMETER\n"));
        ntStatus = STATUS_INVALID_PARAMETER;
        ExFreePool(rwContext);
        rwContext = NULL;
        goto DispatchReadWrite_Exit;
    }

    if(totalLength == 0)
    {
        wDbgPrint(DBG_VERBOSE, ("DispatchReadWrite: Transfer data length = 0, returning STATUS_SUCCESS\n"));
        ntStatus = STATUS_SUCCESS;
        ExFreePool(rwContext);
        rwContext = NULL;
        goto DispatchReadWrite_Exit;
    }

    urbFlags = USBD_SHORT_TRANSFER_OK;
    virtualAddress = (ULONG_PTR) MmGetMdlVirtualAddress(Irp->MdlAddress);

    if(read)
    {
        urbFlags |= USBD_TRANSFER_DIRECTION_IN;
        wDbgPrint(DBG_VERBOSE, ("DispatchReadWrite: Read operation\n"));
    }
    else
    {
        urbFlags |= USBD_TRANSFER_DIRECTION_OUT;
        wDbgPrint(DBG_VERBOSE, ("DispatchReadWrite: Write operation\n"));
    }

    //
    // the transfer request is for totalLength.
    // we can perform a max of BULKUSB_MAX_TRANSFER_SIZE
    // in each stage.
    //
    if(totalLength > USB_MAX_TRANSFER_SIZE)
    {
        stageLength = USB_MAX_TRANSFER_SIZE;
    }
    else
        stageLength = totalLength;

    mdl = IoAllocateMdl((PVOID) virtualAddress,
                        totalLength,
                        FALSE,
                        FALSE,
                        NULL);

    if (mdl == NULL)
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: Alloc memory for MDL failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        ExFreePool(rwContext);
        rwContext = NULL;
        goto DispatchReadWrite_Exit;
    }

    //
    // map the portion of user-buffer described by an mdl to another mdl
    //
    IoBuildPartialMdl(Irp->MdlAddress,
                      mdl,
                      (PVOID) virtualAddress,
                      stageLength);

    urb = ExAllocatePool(NonPagedPool,  sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

    if(urb == NULL)
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: Alloc memory for URB failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        ExFreePool(rwContext);
        rwContext = NULL;
        IoFreeMdl(mdl);
        goto DispatchReadWrite_Exit;
    }

    UsbBuildInterruptOrBulkTransferRequest( urb,
                                            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                            pipeInformation->PipeHandle,
                                            NULL,
                                            mdl,
                                            stageLength,
                                            urbFlags,
                                            NULL);

    //
    // set BULKUSB_RW_CONTEXT parameters.
    //

    rwContext->Urb             = urb;
    rwContext->Mdl             = mdl;
    rwContext->Length          = totalLength - stageLength;
    rwContext->Numxfer         = 0;
    rwContext->VirtualAddress  = virtualAddress + stageLength;
    rwContext->DeviceExtension = deviceExtension;

// Send the IOCTL_INTERNAL_USB_SUBMIT_URB request to submit a URB to
// the bus driver. The USB hub PDO is the target of this request.
//
//  Input
//    Parameters.Others.Argument1 points to the URB, a variable-length structure.
//    The UrbHeader.Function member of the URB specifies the URB type, and the
//    UrbHeader.Length member specifies the size in bytes of the URB. The length
//    of URB, as well as the meaning of any additional members depends on the
//    value of UrbHeader.Function.
//
//  Output
//    Parameters.Others.Argument1 points to the URB structure. The UrbHeader.Status
//    contains a USB status code for the requested operation. Any additional output
//    depends on the UrbHeader.Function member of the URB submitted.
//
//  I/O Status Block
//    The lower-level drivers will set Irp->IoStatus.Status to STATUS_SUCCESS if the
//    URB can be successfully processed. Otherwise, the bus driver will set it to
//    the appropriate error condition.

    //
    // use the original read/write irp as an internal device control irp
    //

    nextStack = IoGetNextIrpStackLocation(Irp);
    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    nextStack->Parameters.Others.Argument1 = (PVOID) urb;
    nextStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;

    //ntStatus =
    IoSetCompletionRoutine(
                    //DeviceObject,
                    Irp,
                    (PIO_COMPLETION_ROUTINE)ReadWriteCompletion,
                    rwContext,
                    TRUE,
                    TRUE,
                    TRUE);

    //
    // since we return STATUS_PENDING call IoMarkIrpPending.
    // This is the boiler plate code.
    // This may cause extra overhead of an APC for the Irp completion
    // but this is the correct thing to do.
    //

    IoMarkIrpPending(Irp);

    wDbgPrint(DBG_VERBOSE, ("DispatchReadWrite:\n"));
    Usb_IoIncrement(deviceExtension);

    wDbgPrint(DBG_INFO, ("DispatchReadWrite: forwarding Irp 0x%x\n",Irp));
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_ERROR, ("DispatchReadWrite: IoCallDriver failed, status 0x%x\n", ntStatus));

        //
        // if the device was yanked out, then the pipeInformation
        // field is invalid.
        // similarly if the request was cancelled, then we need not
        // invoked reset pipe/device.
        //
        if(  (ntStatus != STATUS_CANCELLED)
          && (ntStatus != STATUS_DEVICE_NOT_CONNECTED))
        {
            wDbgPrint(DBG_ERROR, ("DispatchReadWrite: resetting pipe\n"));
            ntStatus = ResetPipe(DeviceObject, pipeInformation);

            if(!NT_SUCCESS(ntStatus))
            {
                wDbgPrint(DBG_ERROR, ("DispatchReadWrite: ResetPipe failed with status 0x%x, resetting device\n",ntStatus));
                ntStatus = ResetDevice(DeviceObject);
            }
        }
    }
    //
    // we return STATUS_PENDING and not the status returned by the lower layer.
    //
    wDbgPrint(DBG_INFO, ("DispatchReadWrite: Irp 0x%x resulted in status 0x%x. Returning STATUS_PENDING\n",Irp, ntStatus));
    return STATUS_PENDING;

DispatchReadWrite_Exit:
    wDbgPrint(DBG_WARN, ("DispatchReadWrite: completing Irp 0x%x with status 0x%x\n",Irp,ntStatus));
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    wDbgPrint(DBG_FUNCTION, ("DispatchReadWrite - ends\n"));
    return ntStatus;
}


NTSTATUS
ReadWriteCompletion(    IN PDEVICE_OBJECT DeviceObject,
                        IN PIRP           Irp,
                        IN PVOID          Context)
/*++

Routine Description:

    This is the completion routine for reads/writes.

    1. If the irp completes with success, we check if we need to recirculate this
       irp for another stage of transfer. If so, return STATUS_MORE_PROCESSING_REQUIRED.
    2. If the irp completes with an error, free all memory allocs and return the status.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    Context - context passed to the completion routine.

Return Value:

    NT status value

--*/
{
    ULONG               stageLength;
    NTSTATUS            ntStatus;
    PIO_STACK_LOCATION  nextStack;
    PUSB_RW_CONTEXT     rwContext;
    PDEVICE_EXTENSION   deviceExtension;

    //
    // initialize variables
    //
    rwContext = (PUSB_RW_CONTEXT) Context;
    ntStatus = Irp->IoStatus.Status;
    wDbgPrint(DBG_FUNCTION, ("ReadWriteCompletion: device object 0x%x, irp 0x%x, status 0x%x\n",DeviceObject,Irp,ntStatus));

    //UNREFERENCED_PARAMETER(DeviceObject);

    //
    // successfully performed a stageLength of transfer.
    // check if we need to recirculate the irp.
    //
    if(NT_SUCCESS(ntStatus))
    {
        if(rwContext)
        {
            rwContext->Numxfer += rwContext->Urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

            if(rwContext->Length)
            {
                //
                // another stage transfer
                //
                if(rwContext->Length > USB_MAX_TRANSFER_SIZE)
                {
                    stageLength = USB_MAX_TRANSFER_SIZE;
                }
                else
                    stageLength = rwContext->Length;

                wDbgPrint(DBG_ERROR, ("ReadWriteCompletion: reusing URB for Irp 0x%x\n",Irp));
                // the source MDL is not mapped and so when the lower driver
                // calls MmGetSystemAddressForMdl(Safe) on Urb->Mdl (target Mdl),
                // system PTEs are used.
                // IoFreeMdl calls MmPrepareMdlForReuse to release PTEs (unlock
                // VA address before freeing any Mdl
                // Rather than calling IoFreeMdl and IoAllocateMdl each time,
                // just call MmPrepareMdlForReuse
                // Not calling MmPrepareMdlForReuse will leak system PTEs
                //
                MmPrepareMdlForReuse(rwContext->Mdl);

                IoBuildPartialMdl(Irp->MdlAddress,
                                  rwContext->Mdl,
                                  (PVOID) rwContext->VirtualAddress,
                                  stageLength);

                //
                // reinitialize the urb
                //
                rwContext->Urb->UrbBulkOrInterruptTransfer.TransferBufferLength
                                                                  = stageLength;
                rwContext->VirtualAddress += stageLength;
                rwContext->Length -= stageLength;

                nextStack = IoGetNextIrpStackLocation(Irp);
                nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
                nextStack->Parameters.Others.Argument1 = rwContext->Urb;
                nextStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
                //ntStatus =
                IoSetCompletionRoutine(
                    //DeviceObject,
                    Irp,
                    ReadWriteCompletion,
                    rwContext,
                    TRUE,
                    TRUE,
                    TRUE);
                IoCallDriver(rwContext->DeviceExtension->TopOfStackDeviceObject, Irp);
                wDbgPrint(DBG_ERROR, ("ReadWriteCompletion: returning STATUS_MORE_PROCESSING_REQUIRED for Irp 0x%x\n",Irp));
                return STATUS_MORE_PROCESSING_REQUIRED;
            }
            else
            {
                //
                // this is the last transfer
                //

                Irp->IoStatus.Information = rwContext->Numxfer;
                wDbgPrint(DBG_ERROR, ("ReadWriteCompletion: last transfer, returning 0x%x for Irp 0x%x\n",ntStatus, Irp));
            }
        }
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("ReadWriteCompletion: Irp 0x%x failed, status 0x%x\n", Irp, ntStatus));
    }


    if(rwContext)
    {
        //
        // dump rwContext
        //
        wDbgPrint(DBG_INFO, ("ReadWriteCompletion: rwContext @ 0x%x\n", rwContext));
        wDbgPrint(DBG_INFO, ("     Urb             = 0x%x\n", rwContext->Urb));
        wDbgPrint(DBG_INFO, ("     Mdl             = 0x%x\n", rwContext->Mdl));
        wDbgPrint(DBG_INFO, ("     Length          =   %u\n", rwContext->Length));
        wDbgPrint(DBG_INFO, ("     Numxfer         =   %u\n", rwContext->Numxfer));
        wDbgPrint(DBG_INFO, ("     VirtualAddress  = 0x%x\n", rwContext->VirtualAddress));
        wDbgPrint(DBG_INFO, ("     DeviceExtension = 0x%x\n", rwContext->DeviceExtension));

        Usb_IoDecrement(rwContext->DeviceExtension);
        ExFreePool(rwContext->Urb);
        IoFreeMdl(rwContext->Mdl);
        ExFreePool(rwContext);
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("ReadWriteCompletion: rwContext was NULL for Irp 0x%x)\n",Irp));
    }


    wDbgPrint(DBG_FUNCTION, ("ReadWriteCompletion - ends\n"));
    return ntStatus;
}


//
//  WeigherVendorRequest
//

NTSTATUS
WeigherVendorRequest(
    IN PDEVICE_OBJECT           DeviceObject,
    IN PVOID                    ioBuffer,
    IN ULONG                    ioBufferSize,
    IN USHORT                   reqfunction,
    IN USBD_PIPE_HANDLE         hPipe,
    IN ULONG                    urbFlag
    )
{
    NTSTATUS        ntStatus;
    PURB            urb;
    pHostToDevice   pHostDevice;
    PDEVICE_EXTENSION   deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("WeigherVendorRequest: device object 0x%x\n",DeviceObject));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

    if (!urb)
    {
        wDbgPrint(DBG_ERROR, ("WeigherVendorRequest: alloc URB failed.\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    pHostDevice = (pHostToDevice)ioBuffer;

    urb->UrbBulkOrInterruptTransfer.Hdr.Function            = URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
    urb->UrbBulkOrInterruptTransfer.Hdr.Length              = (USHORT)sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb->UrbBulkOrInterruptTransfer.PipeHandle              = hPipe;

    UsbBuildVendorRequest( urb,
                           reqfunction,
                           sizeof( struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST),
                           urbFlag,             // TransferFlags
                           0,                   // ReservedBits
                           pHostDevice->Request,// Request
                           pHostDevice->Value,  // Value
                           pHostDevice->Index,  // Index
                           ioBuffer,            // Transfer Buffer (Optional)
                           NULL,                // Transfer Buffer MDL (Optional)
                           pHostDevice->Length, // Transfer Buffer Length
                           NULL);               // Link (Optional)

    if ((ntStatus = CallUSBD(DeviceObject, urb)) != STATUS_SUCCESS)
    {
        wDbgPrint(DBG_ERROR, ("WeigherVendorRequest: CallUSBD returned 0x%x\n",ntStatus));
    }
    ExFreePool(urb);
    wDbgPrint(DBG_FUNCTION, ("WeigherVendorRequest - ends\n"));
    return ntStatus;
}

NTSTATUS
DispatchCreate( IN PDEVICE_OBJECT DeviceObject,
                IN PIRP           Irp)
/*++

Routine Description:

    Dispatch routine for create.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet.

Return Value:

    NT status value

--*/
{
    ULONG                       i;
    NTSTATUS                    ntStatus;
    PFILE_OBJECT                fileObject;
    PDEVICE_EXTENSION           deviceExtension;
    PIO_STACK_LOCATION          irpStack;
    PUSB_PIPE_CONTEXT           pipeContext;
    PUSBD_INTERFACE_INFORMATION winterface;

    PAGED_CODE();

    wDbgPrint(DBG_FUNCTION, ("DispatchCreate: device object 0x%x, irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fileObject = irpStack->FileObject;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if (deviceExtension->DeviceState != Working)
    {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        wDbgPrint(DBG_ERROR, ("DispatchCreate: invalid device state %u, fail with 0x%x\n",deviceExtension->DeviceState, ntStatus));
        goto DispatchCreate_Exit;
    }

    VERIFY_DEVICE_EXTENSION(deviceExtension);

    if (deviceExtension->UsbInterface)
    {
        winterface = deviceExtension->UsbInterface;
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("DispatchCreate: UsbInterface NULL\n"));
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        goto DispatchCreate_Exit;
    }

    //
    // FsContext is Null for the device
    //
    if (fileObject)
    {
        fileObject->FsContext = NULL;
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("DispatchCreate: file object NULL\n"));
        ntStatus = STATUS_INVALID_PARAMETER;
        goto DispatchCreate_Exit;
    }

    if(fileObject->FileName.Length == 0)
    {
        //
        // opening a device as opposed to pipe.
        //
        ntStatus = STATUS_SUCCESS;
        InterlockedIncrement(&deviceExtension->OpenHandleCount);

        //
        // the device is idle if it has no open handles or pending PnP Irps
        // since we just received an open handle request, cancel idle req.
        //
        if (deviceExtension->SSEnable)
            CancelSelectSuspend(deviceExtension);

        goto DispatchCreate_Exit;
    }

    pipeContext = PipeWithName(DeviceObject, &fileObject->FileName);

    if(pipeContext == NULL)
    {
        wDbgPrint(DBG_ERROR, ("DispatchCreate: pipeContext NULL\n"));
        ntStatus = STATUS_INVALID_PARAMETER;
        goto DispatchCreate_Exit;
    }

    ntStatus = STATUS_INVALID_PARAMETER;

    for(i=0; i<winterface->NumberOfPipes; i++)
    {
        if(pipeContext == &deviceExtension->PipeContext[i])
        {
            //
            // found a match
            //
            wDbgPrint(DBG_INFO, ("DispatchCreate: opening USB pipe %d\n", i));
            fileObject->FsContext = &winterface->Pipes[i];
            ASSERT(fileObject->FsContext);
            pipeContext->PipeOpen = TRUE;

            ntStatus = STATUS_SUCCESS;
            //
            // increment OpenHandleCounts
            //
            InterlockedIncrement(&deviceExtension->OpenHandleCount);

            //
            // the device is idle if it has no open handles or pending PnP Irps
            // since we just received an open handle request, cancel idle req.
            //
            if(deviceExtension->SSEnable)
            {
                CancelSelectSuspend(deviceExtension);
            }
        }
    }

DispatchCreate_Exit:
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    wDbgPrint(DBG_FUNCTION, ("DispatchCreate - ends\n"));
    return ntStatus;
}

NTSTATUS
DispatchClose(  IN PDEVICE_OBJECT DeviceObject,
                IN PIRP           Irp)
/*++

Routine Description:

    Dispatch routine for close.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
    NTSTATUS                ntStatus;
    PFILE_OBJECT            fileObject;
    PDEVICE_EXTENSION       deviceExtension;
    PIO_STACK_LOCATION      irpStack;
    PUSB_PIPE_CONTEXT       pipeContext;
    PUSBD_PIPE_INFORMATION  pipeInformation;

    PAGED_CODE();

    wDbgPrint(DBG_FUNCTION, ("DispatchClose: device object 0x%x, irp 0x%x\n",DeviceObject,Irp));

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fileObject = irpStack->FileObject;
    pipeContext = NULL;
    pipeInformation = NULL;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    if(fileObject && fileObject->FsContext)
    {
        pipeInformation = fileObject->FsContext;

        if(0 != fileObject->FileName.Length)
            pipeContext = PipeWithName( DeviceObject, &fileObject->FileName);

        if(pipeContext && pipeContext->PipeOpen)
            pipeContext->PipeOpen = FALSE;
    }

    Irp->IoStatus.Status = ntStatus = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    InterlockedDecrement(&deviceExtension->OpenHandleCount);
    wDbgPrint(DBG_FUNCTION, ("DispatchClose - ends\n"));
    return ntStatus;
}

NTSTATUS
DispatchDevCtrl(IN PDEVICE_OBJECT DeviceObject,
                IN PIRP Irp)
/*++

Routine Description:

    Dispatch routine for IRP_MJ_DEVICE_CONTROL

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
    ULONG               code;
    PVOID               ioBuffer;
    ULONG               inputBufferLength;
    ULONG               outputBufferLength;
    ULONG               info;
    UCHAR               Ep;
    UCHAR               i;
    NTSTATUS            ntStatus;
    PDEVICE_EXTENSION   deviceExtension;
    PIO_STACK_LOCATION  irpStack;

    wDbgPrint(DBG_FUNCTION, ("DispatchDevCtrl: device object 0x%x, irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    info = 0;
    irpStack        = IoGetCurrentIrpStackLocation(Irp);
    code            = irpStack->Parameters.DeviceIoControl.IoControlCode;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if(deviceExtension->DeviceState != Working)
    {
        Irp->IoStatus.Status = ntStatus = STATUS_INVALID_DEVICE_STATE;
        Irp->IoStatus.Information = info;
        wDbgPrint(DBG_ERROR, ("DispatchDevCtrl: invalid device state %u, fail with 0x%x\n",deviceExtension->DeviceState, ntStatus));
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return ntStatus;
    }

    VERIFY_DEVICE_EXTENSION(deviceExtension);

    Usb_IoIncrement(deviceExtension);

    //
    // It is true that the client driver cancelled the selective suspend
    // request in the dispatch routine for create.
    // But there is no guarantee that it has indeed been completed.
    // so wait on the NoIdleReqPendEvent and proceed only if this event
    // is signalled.
    //

    //
    // make sure that the selective suspend request has been completed.
    //

    if(deviceExtension->SSEnable)
    {
        //wDbgPrint(DBG_INFO, ("DispatchDevCtrl: Waiting on the NoIdleReqPendEvent\n"));
        KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, Executive, KernelMode, FALSE, NULL);
    }

    switch(code)
    {
        case IOCTL_USB_RESET_PIPE:
        {
            PFILE_OBJECT           fileObject = NULL;
            PUSBD_PIPE_INFORMATION pipe = NULL;


            //
            // FileObject is the address of the kernel file object to
            // which the IRP is directed. Drivers use the FileObject
            // to correlate IRPs in a queue.
            //
            if((fileObject = irpStack->FileObject) == NULL)
            {
                wDbgPrint(DBG_ERROR, ("DispatchDevCtrl: fileObject = NULL\n"));
                ntStatus = STATUS_INVALID_PARAMETER;
                break;
            }

            pipe = (PUSBD_PIPE_INFORMATION) fileObject->FsContext;
            if(pipe == NULL)
            {
                wDbgPrint(DBG_ERROR, ("DispatchDevCtrl: pipe = NULL\n"));
                ntStatus = STATUS_INVALID_PARAMETER;
            }
            else
            {
                ntStatus = ResetPipe(DeviceObject, pipe);
                if (ntStatus == STATUS_SUCCESS)
                {
                    wDbgPrint(DBG_INFO, ("DispatchDevCtrl: reset pipe on EP %u\n", pipe->EndpointAddress));
                }
                else
                {
                    wDbgPrint(DBG_INFO, ("DispatchDevCtrl: reset pipe on EP %u failed, 0x%x\n", pipe->EndpointAddress, ntStatus));
                }
            }

            break;
        }

        case IOCTL_USB_GET_CONFIG_DESCRIPTOR:
        {
            ULONG length;

            ntStatus = STATUS_UNSUCCESSFUL;

            if(deviceExtension->UsbConfigurationDescriptor)
            {
                length = deviceExtension->UsbConfigurationDescriptor->wTotalLength;

                if(outputBufferLength >= length)
                {
                    RtlCopyMemory(ioBuffer, deviceExtension->UsbConfigurationDescriptor, length);
                    info = length;
                    wDbgPrint(DBG_INFO, ("DispatchDevCtrl: read configuration descriptor for VID 0x%x PID 0x%x\n",
                        deviceExtension->UsbDeviceDescriptor->idVendor,
                        deviceExtension->UsbDeviceDescriptor->idProduct ));
                    ntStatus = STATUS_SUCCESS;
                }
                else
                {
                    wDbgPrint(DBG_ERROR, ("DispatchDevCtrl: buffer too small for config descriptor\n"));
                    ntStatus = STATUS_BUFFER_TOO_SMALL;
                }
            }
            break;
        }

        case IOCTL_USB_GET_DEVICE_DESCRIPTOR:
        {
            ULONG length;

            ntStatus = STATUS_UNSUCCESSFUL;

            if(deviceExtension->UsbDeviceDescriptor)
            {
                length = deviceExtension->UsbDeviceDescriptor->bLength;
                ntStatus = STATUS_BUFFER_TOO_SMALL;

                if(outputBufferLength >= length)
                {
                    RtlCopyMemory(ioBuffer, deviceExtension->UsbDeviceDescriptor, length);
                    info = length;
                    wDbgPrint(DBG_INFO, ("DispatchDevCtrl: read device descriptor for VID 0x%x PID 0x%x\n",
                        deviceExtension->UsbDeviceDescriptor->idVendor,
                        deviceExtension->UsbDeviceDescriptor->idProduct ));
                    ntStatus = STATUS_SUCCESS;
                }
                else
                {
                    wDbgPrint(DBG_ERROR, ("DispatchDevCtrl: buffer too small for device descriptor\n"));
                }
            }
            break;
        }

        case IOCTL_USB_RESET_DEVICE:
        {
            wDbgPrint(DBG_INFO, ("DispatchDevCtrl: reset device VID 0x%x PID 0x%x\n",
                deviceExtension->UsbDeviceDescriptor->idVendor,
                deviceExtension->UsbDeviceDescriptor->idProduct ));
            ntStatus = ResetDevice(DeviceObject);
            if (ntStatus == STATUS_SUCCESS)
            {
                wDbgPrint(DBG_INFO, ("DispatchDevCtrl: reset device with VID 0x%x PID 0x%x\n",
                        deviceExtension->UsbDeviceDescriptor->idVendor,
                        deviceExtension->UsbDeviceDescriptor->idProduct ));
            }
            else
            {
                wDbgPrint(DBG_INFO, ("DispatchDevCtrl: reset device with VID 0x%x PID 0x%x failed, 0x%x\n",
                        deviceExtension->UsbDeviceDescriptor->idVendor,
                        deviceExtension->UsbDeviceDescriptor->idProduct, ntStatus ));
            }
            break;
        }

        default :
        {
            wDbgPrint(DBG_INFO, ("DispatchDevCtrl: invalid device request for VID 0x%x PID 0x%x\n",
                deviceExtension->UsbDeviceDescriptor->idVendor,
                deviceExtension->UsbDeviceDescriptor->idProduct ));
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = info;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    wDbgPrint(DBG_FUNCTION, ("DispatchDevCtrl - end\n"));
    Usb_IoDecrement(deviceExtension);
    return ntStatus;
}


NTSTATUS
ResetPipe(  IN PDEVICE_OBJECT         DeviceObject,
            IN PUSBD_PIPE_INFORMATION PipeInfo)
/*++

Routine Description:

    This routine synchronously submits a URB_FUNCTION_RESET_PIPE
    request down the stack.

Arguments:

    DeviceObject - pointer to device object
    PipeInfo - pointer to PipeInformation structure
               to retrieve the pipe handle

Return Value:

    NT status value

--*/
{
    PURB              urb;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("ResetPipe: device object 0x%x, PipeInfo 0x%x\n",DeviceObject,PipeInfo));

    urb = NULL;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_PIPE_REQUEST));

    if(urb)
    {
        urb->UrbHeader.Length = (USHORT) sizeof(struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle = PipeInfo->PipeHandle;
        ntStatus = CallUSBD(DeviceObject, urb);
        ExFreePool(urb);
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

   if(NT_SUCCESS(ntStatus))
    {
        //wDbgPrint(DBG_INFO, ("ResetPipe - success\n"));
        ntStatus = STATUS_SUCCESS;
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("ResetPipe - failed, 0x%x\n",ntStatus));
    }

    return ntStatus;
}

NTSTATUS
ResetDevice(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine invokes BulkUsb_ResetParentPort to reset the device

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
    NTSTATUS ntStatus;
    ULONG    portStatus;

    wDbgPrint(1, ("ResetDevice: \n"));

    ntStatus = GetPortStatus(DeviceObject, &portStatus);

    if(  (NT_SUCCESS(ntStatus))
     && (!(portStatus & USBD_PORT_ENABLED))
     && (portStatus & USBD_PORT_CONNECTED) )
    {
        ntStatus = ResetParentPort(DeviceObject);
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("ResetDevice: ResetParentPort failed, error 0x%x\n",ntStatus));

    }

    return ntStatus;
}

NTSTATUS
GetPortStatus(  IN PDEVICE_OBJECT DeviceObject,
                IN OUT PULONG     PortStatus)
/*++

Routine Description:

    This routine retrieves the status value

Arguments:

    DeviceObject - pointer to device object
    PortStatus - port status

Return Value:

    NT status value

--*/
{
    NTSTATUS           ntStatus;
    KEVENT             event;
    PIRP               irp;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION  deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("GetPortStatus: device object 0x%x, PortStatus 0x%x\n",DeviceObject,PortStatus));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    *PortStatus = 0;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(    IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                                            deviceExtension->TopOfStackDeviceObject,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            TRUE,
                                            &event,
                                            &ioStatus);

    if(NULL == irp)
    {
        wDbgPrint(DBG_ERROR, ("GetPortStatus: Memory alloc for irp failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);
    nextStack->Parameters.Others.Argument1 = PortStatus;
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(STATUS_PENDING == ntStatus)
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    else
        ioStatus.Status = ntStatus;

    ntStatus = ioStatus.Status;
    wDbgPrint(DBG_FUNCTION, ("GetPortStatus - ends\n"));
    return ntStatus;
}

NTSTATUS
ResetParentPort(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine sends an IOCTL_INTERNAL_USB_RESET_PORT
    synchronously down the stack.

Arguments:

Return Value:

--*/
{
    NTSTATUS           ntStatus;
    KEVENT             event;
    PIRP               irp;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION  deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("ResetParentPort: device object 0x%x\n",DeviceObject));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(    IOCTL_INTERNAL_USB_RESET_PORT,
                                            deviceExtension->TopOfStackDeviceObject,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            TRUE,
                                            &event,
                                            &ioStatus);

    if(NULL == irp)
    {
        wDbgPrint(DBG_ERROR, ("memory alloc for irp failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(STATUS_PENDING == ntStatus)
    {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    }
    else
    {
        ioStatus.Status = ntStatus;
    }

    ntStatus = ioStatus.Status;
    wDbgPrint(DBG_FUNCTION, ("ResetParentPort - ends\n"));
    return ntStatus;
}

NTSTATUS
SubmitIdleRequestIrp(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    This routine builds an idle request irp with an associated callback routine
    and a completion routine in the driver and passes the irp down the stack.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    PIRP                    irp;
    NTSTATUS                ntStatus;
    KIRQL                   oldIrql;
    PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;
    PIO_STACK_LOCATION      nextStack;

    wDbgPrint(DBG_FUNCTION, ("SubmitIdleRequestIrp: begins\n"));

    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    irp = NULL;
    idleCallbackInfo = NULL;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if(PowerDeviceD0 != DeviceExtension->DevPower)
    {
        wDbgPrint(DBG_INFO, ("SubmitIdleRequestIrp: not power state D0, invalid\n"));
        ntStatus = STATUS_POWER_STATE_INVALID;
        goto SubmitIdleRequestIrp_Exit;
    }

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

    if(InterlockedExchange(&DeviceExtension->IdleReqPend, 1))
    {
        wDbgPrint(DBG_INFO, ("SubmitIdleRequestIrp: Idle request pending...\n"));
        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);
        ntStatus = STATUS_DEVICE_BUSY;
        goto SubmitIdleRequestIrp_Exit;
    }

    //
    // clear the NoIdleReqPendEvent because we are about
    // to submit an idle request. Since we are so early
    // to clear this event, make sure that if we fail this
    // request we set back the event.
    //
    KeClearEvent(&DeviceExtension->NoIdleReqPendEvent);
    idleCallbackInfo = ExAllocatePool(NonPagedPool, sizeof(struct _USB_IDLE_CALLBACK_INFO));

    if(idleCallbackInfo)
    {
        idleCallbackInfo->IdleCallback = IdleNotificationCallback;
        idleCallbackInfo->IdleContext = (PVOID)DeviceExtension;
        ASSERT(DeviceExtension->IdleCallbackInfo == NULL);
        DeviceExtension->IdleCallbackInfo = idleCallbackInfo;
        //
        // we use IoAllocateIrp to create an irp to selectively suspend the
        // device. This irp lies pending with the hub driver. When appropriate
        // the hub driver will invoked callback, where we power down. The completion
        // routine is invoked when we power back.
        //
        irp = IoAllocateIrp(DeviceExtension->TopOfStackDeviceObject->StackSize, FALSE);

        if(irp == NULL)
        {
            wDbgPrint(DBG_ERROR, ("SubmitIdleRequestIrp: alloc idle request irp failed\n"));
            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
            InterlockedExchange(&DeviceExtension->IdleReqPend, 0);
            KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);
            ExFreePool(idleCallbackInfo);
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto SubmitIdleRequestIrp_Exit;
        }

    //  Drivers use the IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION request to inform the
    //  USB bus driver that a device is idle and can be suspended. When sending this IOCTL,
    //  caller must furnish a callback routine that does the actual suspension of the device.
    //  The USB bus driver will call this routine at PASSIVE_LEVEL when it is safe for the
    //  device to be powered down. If the device supports remote wake up and has no
    //  Wait/Wake IRP already pending, the callback routine should submit a Wait/Wake IRP to
    //  the bus driver for the device, before powering it down.  The USB hub PDO is the target
    //  of this request. The IOCTL must be sent at an IRQL of PASSIVE_LEVEL.
    //
        nextStack = IoGetNextIrpStackLocation(irp);
        nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        nextStack->Parameters.DeviceIoControl.IoControlCode     = IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION;
        nextStack->Parameters.DeviceIoControl.Type3InputBuffer  = idleCallbackInfo;
        nextStack->Parameters.DeviceIoControl.InputBufferLength = sizeof(struct _USB_IDLE_CALLBACK_INFO);

        //ntStatus =
        IoSetCompletionRoutine(
                        //DeviceExtension->FunctionalDeviceObject,
                        irp,
                        IdleNotificationRequestComplete,
                        DeviceExtension,
                        TRUE, // invoke on success
                        TRUE, // invoke on error
                        TRUE);// invoke on cancel

        DeviceExtension->PendingIdleIrp = irp;

        //
        // we initialize the count to 2.
        // The reason is, if the CancelSelectSuspend routine manages
        // to grab the irp from the device extension, then the last of the
        // CancelSelectSuspend routine/IdleNotificationRequestComplete routine
        // to execute will free this irp. We need to have this schema so that
        // 1. completion routine does not attempt to touch the irp freed by
        //    CancelSelectSuspend routine.
        // 2. CancelSelectSuspend routine doesnt wait for ever for the completion
        //    routine to complete!
        //
        DeviceExtension->FreeIdleIrpCount = 2;
        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

        //
        // check if the device is idle.
        // A check here ensures that a race condition did not
        // completely reverse the call sequence of SubmitIdleRequestIrp
        // and CancelSelectiveSuspend
        //

        if(!CanDeviceSuspend(DeviceExtension) || PowerDeviceD0 != DeviceExtension->DevPower)
        {
            //
            // IRPs created using IoBuildDeviceIoControlRequest should be
            // completed by calling IoCompleteRequest and not merely
            // deallocated.
            //

            wDbgPrint(DBG_INFO, ("SubmitIdleRequestIrp: device is not idle\n"));

            KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);
            DeviceExtension->IdleCallbackInfo   = NULL;
            DeviceExtension->PendingIdleIrp     = NULL;

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
            InterlockedExchange(&DeviceExtension->IdleReqPend, 0);
            KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

            if(idleCallbackInfo)
                ExFreePool(idleCallbackInfo);

            //
            // it is still safe to touch the local variable "irp" here.
            // the irp has not been passed down the stack, the irp has
            // no cancellation routine. The worse position is that the
            // CancelSelectSuspend has run after we released the spin
            // lock above. It is still essential to free the irp.
            //
            if(irp)
                IoFreeIrp(irp);

            goto SubmitIdleRequestIrp_Exit;
        }

        wDbgPrint(DBG_INFO, ("SubmitIdleRequestIrp: canceling the timer\n"));
        //
        // Cancel the timer so that the DPCs are no longer fired.
        // Thus, we are making judicious usage of our resources.
        // we do not need DPCs because we already have an idle irp pending.
        // The timers are re-initialized in the completion routine.
        //
        KeCancelTimer(&DeviceExtension->Timer);

        ntStatus = IoCallDriver(DeviceExtension->TopOfStackDeviceObject, irp);

        if(!NT_SUCCESS(ntStatus))
        {
            wDbgPrint(DBG_ERROR, ("SubmitIdleRequestIrp: IoCallDriver failed, 0x%x\n",ntStatus));
            goto SubmitIdleRequestIrp_Exit;
        }
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("Memory alloc idleCallbackInfo failed\n"));
        KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
        InterlockedExchange(&DeviceExtension->IdleReqPend, 0);
        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

SubmitIdleRequestIrp_Exit:
    wDbgPrint(DBG_FUNCTION, ("SubmitIdleRequest - ends\n"));
    return ntStatus;
}

VOID
IdleNotificationCallback(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

  "A pointer to a callback function in your driver is passed down the stack with
   this IOCTL, and it is this callback function that is called by USBHUB when it
   safe for your device to power down."

  "When the callback in your driver is called, all you really need to do is to
   to first ensure that a WaitWake Irp has been submitted for your device, if
   remote wake is possible for your device and then request a SetD2 (or DeviceWake)"

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    NTSTATUS                ntStatus;
    POWER_STATE             powerState;
    KEVENT                  irpCompletionEvent;
    PIRP_COMPLETION_CONTEXT irpContext;

    wDbgPrint(DBG_FUNCTION, ("IdleNotificationCallback - begins\n"));

    //
    // Dont idle, if the device was just disconnected or being stopped
    // i.e. return for the following DeviceState(s)
    // NotStarted, Stopped, PendingStop, PendingRemove, SurpriseRemoved, Removed
    //

    if (DeviceExtension->DeviceState != Working)
    {
        wDbgPrint(1, ("IdleNotificationCallback: invalid device state %u\n",(ULONG)DeviceExtension->DeviceState));
        return;
    }

    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    //
    // If there is not already a WW IRP pending, submit one now
    //
    if(DeviceExtension->WaitWakeEnable)
    {
        wDbgPrint(1, ("IdleNotificationCallback: issuing wait wake\n"));
        IssueWaitWake(DeviceExtension);
    }
    else
    {
        wDbgPrint(1, ("IdleNotificationCallback: wait wake not enabled\n"));
    }

    //
    // power down the device
    //

    // allocate a context for our IRP
    irpContext = (PIRP_COMPLETION_CONTEXT)ExAllocatePool(NonPagedPool, sizeof(IRP_COMPLETION_CONTEXT));

    if(!irpContext)
    {
        wDbgPrint(DBG_ERROR, ("IdleNotificationCallback: alloc irpContext failed\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        //
        // increment the count. In the HoldIoRequestWorkerRoutine, the
        // count is decremented twice (one for the system Irp and the
        // other for the device Irp. An increment here compensates for
        // the sytem irp..The decrement corresponding to this increment
        // is in the completion function
        //

        Usb_IoIncrement(DeviceExtension);

        // load the requested power level

        powerState.DeviceState = DeviceExtension->PowerDownLevel;

        // initialize an event which is signaled by the IRP completion
        // function when it runs

        KeInitializeEvent(&irpCompletionEvent, NotificationEvent, FALSE);

        // load the IRP context with our devext and event handle

        irpContext->DeviceExtension = DeviceExtension;
        irpContext->Event = &irpCompletionEvent;

        // Call PoRequestPowerIrp to allocate and send the IRP to the top
        // of the device stack. On completion, the PoIrpCompletionFunc
        // callback is called with the irpContext.

        ntStatus = PoRequestPowerIrp(   DeviceExtension->PhysicalDeviceObject,
                                        IRP_MN_SET_POWER,
                                        powerState,
                                        (PREQUEST_POWER_COMPLETE) PoIrpCompletionFunc,
                                        irpContext,
                                        NULL);

        if(STATUS_PENDING == ntStatus)
        {
            // If PoRequestPowerIrp returns a status value of STATUS_PENDING,
            // it successfully allocated and sent a device power IRP to the top
            // of the device stack.
            //

            wDbgPrint(DBG_INFO, ("IdleNotificationCallback:: waiting for the power irp to complete\n"));

            // Wait (forever?) on the completion routine to signal the event.
            KeWaitForSingleObject(&irpCompletionEvent, Executive, KernelMode, FALSE, NULL);
        }
        else
        {
            // If PoRequestPowerIrp returns a status other than STATUS_PENDING, the
            // routine did not send a device power IRP and the CompletionFunction
            // is not called.
        }
    }

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_INFO, ("IdleNotificationCallback:: send power irp failed\n"));
        if(irpContext)
        {
            ExFreePool(irpContext);
        }
    }
    wDbgPrint(DBG_FUNCTION, ("IdleNotificationCallback - ends\n"));
}

//
//  IdleNotificationRequestComplete
//

NTSTATUS
IdleNotificationRequestComplete(    IN PDEVICE_OBJECT    DeviceObject,
                                    IN PIRP              Irp,
                                    IN PVOID             Context)
/*++

Routine Description:

  Completion routine for idle notification irp

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    NTSTATUS                ntStatus;
    POWER_STATE             powerState;
    KIRQL                   oldIrql;
    LARGE_INTEGER           dueTime;
    PIRP                    idleIrp;
    PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;
    PDEVICE_EXTENSION       DeviceExtension = (PDEVICE_EXTENSION)Context;

    wDbgPrint(DBG_FUNCTION, ("IdleNotificationRequestComplete: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    idleIrp = NULL;

    //
    // check the Irp status
    //

    ntStatus = Irp->IoStatus.Status;

    if(!NT_SUCCESS(ntStatus) && ntStatus != STATUS_NOT_SUPPORTED)
    {
        switch(ntStatus)
        {
            case STATUS_INVALID_DEVICE_REQUEST:
                wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: irp completed with STATUS_INVALID_DEVICE_REQUEST\n"));
                break;

            case STATUS_CANCELLED:
                //wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: irp completed with STATUS_CANCELLED\n"));
                break;

            case STATUS_POWER_STATE_INVALID:
                wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: irp completed with STATUS_POWER_STATE_INVALID\n"));
                goto IdleNotificationRequestComplete_Exit;

            case STATUS_DEVICE_BUSY:
                wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: irp completed with STATUS_DEVICE_BUSY\n"));
                break;

            default:
                wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: irp completed with status 0x%x\n", ntStatus));
                break;
        }

        //
        // if in error, issue a SetD0 (only when not in D0)
        //

        if(PowerDeviceD0 != DeviceExtension->DevPower)
        {
            Usb_IoIncrement(DeviceExtension);

            powerState.DeviceState = PowerDeviceD0;

            ntStatus = PoRequestPowerIrp(
                                      DeviceExtension->PhysicalDeviceObject,
                                      IRP_MN_SET_POWER,
                                      powerState,
                                      (PREQUEST_POWER_COMPLETE) PoIrpAsyncCompletionFunc,
                                      DeviceExtension,
                                      NULL);

            if(!NT_SUCCESS(ntStatus))
                wDbgPrint(DBG_ERROR, ("IdleNotificationRequestComplete: PoRequestPowerIrp failed, 0x%x\n",ntStatus));
            }
    }

IdleNotificationRequestComplete_Exit:

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);
    idleCallbackInfo = DeviceExtension->IdleCallbackInfo;
    DeviceExtension->IdleCallbackInfo = NULL;

    idleIrp = (PIRP) InterlockedExchangePointer(&DeviceExtension->PendingIdleIrp, NULL);
    InterlockedExchange(&DeviceExtension->IdleReqPend, 0);
    KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

    if(idleCallbackInfo)
        ExFreePool(idleCallbackInfo);


    //
    // since the irp was created using IoAllocateIrp,
    // the Irp needs to be freed using IoFreeIrp.
    // Also return STATUS_MORE_PROCESSING_REQUIRED so that
    // the kernel does not reference this in the near future.
    //

    if(idleIrp)
    {
        wDbgPrint(DBG_INFO, ("IdleNotificationRequestComplete: completion routine has a valid irp and frees it\n"));
        IoFreeIrp(Irp);
        KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
    }
    else
    {

        //
        // The CancelSelectiveSuspend routine has grabbed the Irp from the device
        // extension. Now the last one to decrement the FreeIdleIrpCount should
        // free the irp.
        //
        if(0 == InterlockedDecrement(&DeviceExtension->FreeIdleIrpCount))
        {
            wDbgPrint(DBG_INFO, ("IdleNotificationRequestComplete: completion routine frees the irp\n"));
            IoFreeIrp(Irp);
            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
        }
    }

    if(DeviceExtension->SSEnable)
    {
        wDbgPrint(DBG_INFO, ("IdleNotificationRequestComplete: Enabling DPC timer\n"));
        dueTime.QuadPart = -10000 * IDLE_INTERVAL;
        KeSetTimerEx(&DeviceExtension->Timer, dueTime, IDLE_INTERVAL, &DeviceExtension->DeferredProcCall);
    }
    wDbgPrint(DBG_FUNCTION, ("IdleNotificationRequestComplete - ends\n"));
    return STATUS_MORE_PROCESSING_REQUIRED;
}


VOID
CancelSelectSuspend(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    This routine is invoked to cancel selective suspend request.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    None.

--*/
{
    PIRP  irp = NULL;
    KIRQL oldIrql;

    wDbgPrint(DBG_FUNCTION, ("CancelSelectSuspend:\n"));
    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

    if(!CanDeviceSuspend(DeviceExtension))
    {
        wDbgPrint(DBG_INFO, ("CancelSelectSuspend: device not idle, will try to cancel PendingIdleIrp\n"));
        irp = (PIRP) InterlockedExchangePointer(&DeviceExtension->PendingIdleIrp, NULL);
    }
    else
    {
        wDbgPrint(DBG_INFO, ("CancelSelectSuspend: device is idle,\n"));
    }

    KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

    //
    // since we have a valid Irp ptr,
    // we can call IoCancelIrp on it,
    // without the fear of the irp
    // being freed underneath us.
    //
    if (irp)
    {
        //
        // This routine has the irp pointer.
        // It is safe to call IoCancelIrp because we know that
        // the completion routine will not free this irp unless...
        //
        //
        wDbgPrint(DBG_INFO, ("CancelSelectSuspend: canceling PendingIdleIrp 0x%x\n",irp));
        if (IoCancelIrp(irp))
        {
            wDbgPrint(DBG_INFO, ("CancelSelectSuspend: IoCancelIrp 0x%x succeeded, PendingIdleIrp cancelled\n",irp));
        }
        else
        {
            wDbgPrint(DBG_ERROR, ("CancelSelectSuspend: IoCancelIrp 0x%x failed, PendingIdleIrp not cancelled\n",irp));
        }

        //
        // ....we decrement the FreeIdleIrpCount from 2 to 1.
        // if completion routine runs ahead of us, then this routine
        // decrements the FreeIdleIrpCount from 1 to 0 and hence shall
        // free the irp.
        //
        if (0 == InterlockedDecrement(&DeviceExtension->FreeIdleIrpCount))
        {
            IoFreeIrp(irp);
            wDbgPrint(DBG_INFO, ("CancelSelectSuspend: PendingIdleIrp 0x%x freed\n",irp));
            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent, IO_NO_INCREMENT, FALSE);
        }
        else
        {
            wDbgPrint(DBG_ERROR, ("CancelSelectSuspend: FreeIdleIrpCount not 0, PendingIdleIrp was not freed\n"));
        }
    }
    else
    {
        wDbgPrint(DBG_INFO, ("CancelSelectSuspend: PendingIdleIrp is NULL, no action taken\n",irp));
    }
    wDbgPrint(DBG_FUNCTION, ("CancelSelectSuspend - ends\n"));
    return;
}



//
//  PREQUEST_POWER_COMPLETE
VOID
PoIrpCompletionFunc(    IN PDEVICE_OBJECT DeviceObject,
                        IN UCHAR MinorFunction,
                        IN POWER_STATE PowerState,
                        IN PVOID Context,
                        IN PIO_STATUS_BLOCK IoStatus)
/*++

Routine Description:

    Completion routine for power irp PoRequested in
    IdleNotificationCallback.

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.

Return Value:

    None

 NOTE: may be called at IRQL=PASSIVE_LEVEL or IRQL=DISPATCH_LEVEL!

--*/
{
    PIRP_COMPLETION_CONTEXT irpContext;
    PDEVICE_EXTENSION DeviceExtension;

    wDbgPrint(DBG_FUNCTION, ("PoIrpCompletionFunc: device object 0x%x, Irp 0x%x\n",DeviceObject,Context));

    DeviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    irpContext = NULL;

    if (Context)
    {
        irpContext = (PIRP_COMPLETION_CONTEXT) Context;
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("PoIrpCompletionFunc: Context is NULL\n"));
    }


    //
    // all we do is set the event and decrement the count
    //

    if(irpContext)
    {
        KeSetEvent(irpContext->Event, 0, FALSE);
        wDbgPrint(DBG_INFO, ("PoIrpCompletionFunc: before io decrement\n"));
        VERIFY_DEVICE_EXTENSION(irpContext->DeviceExtension);
        Usb_IoDecrement(irpContext->DeviceExtension);
        ExFreePool(irpContext);
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("PoIrpCompletionFunc: irpContext is NULL\n"));
    }
    wDbgPrint(DBG_FUNCTION, ("PoIrpCompletionFunc - ends\n"));
    return;
}

//  PREQUEST_POWER_COMPLETE
VOID
PoIrpAsyncCompletionFunc(   IN PDEVICE_OBJECT DeviceObject,
                            IN UCHAR MinorFunction,
                            IN POWER_STATE PowerState,
                            IN PVOID Context,
                            IN PIO_STATUS_BLOCK IoStatus)
/*++

Routine Description:

    Completion routine for power irp PoRequested in IdleNotification
    RequestComplete routine.

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION DeviceExtension;

    wDbgPrint(DBG_FUNCTION, ("PoIrpAsyncCompletionFunc - begins\n"));
    UNREFERENCED_PARAMETER( MinorFunction );
    UNREFERENCED_PARAMETER( PowerState );
    UNREFERENCED_PARAMETER( IoStatus );

    DeviceExtension = (PDEVICE_EXTENSION) Context;
    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    //
    // all we do is decrement the count
    //

    Usb_IoDecrement(DeviceExtension);
    wDbgPrint(DBG_FUNCTION, ("PoIrpAsyncCompletionFunc - ends\n"));
    return;
}

//  PREQUEST_POWER_COMPLETE
//  WWIrpCompletionFunc
//

VOID
WWIrpCompletionFunc(IN PDEVICE_OBJECT DeviceObject,
                    IN UCHAR MinorFunction,
                    IN POWER_STATE PowerState,
                    IN PVOID Context,
                    IN PIO_STATUS_BLOCK IoStatus)
/*++

Routine Description:

    Completion routine for PoRequest wait wake irp

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION DeviceExtension;

    DeviceExtension = (PDEVICE_EXTENSION) Context;
    wDbgPrint(DBG_FUNCTION, ("WWIrpCompletionFunc: device object 0x%x\n",DeviceObject));
    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    //
    // all we do is decrement the count
    //

    Usb_IoDecrement(DeviceExtension);
    wDbgPrint(DBG_FUNCTION, ("WWIrpCompletionFunc - ends\n"));
    return;
}


NTSTATUS
DispatchPnP(IN PDEVICE_OBJECT DeviceObject,
            IN PIRP           Irp)
/*++

Routine Description:

    The plug and play dispatch routines.
    Most of these requests the driver will completely ignore.
    In all cases it must pass on the IRP to the lower driver.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - pointer to an I/O Request Packet.

Return Value:

    NT status value

--*/
{
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION  deviceExtension;
//    KEVENT             startDeviceEvent;
    NTSTATUS           ntStatus;

    wDbgPrint(DBG_FUNCTION, ("DispatchPnP: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    deviceExtension = DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // if the device is removed, fail the Irp.
    //

    if(Removed == deviceExtension->DeviceState)
    {
        ntStatus = STATUS_DELETE_PENDING;
        wDbgPrint(DBG_DISPATCH, ("DispatchPnP: device object 0x%x, device state is Removed, returning 0x%x\n",DeviceObject, ntStatus));
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return ntStatus;
    }

    Usb_IoIncrement(deviceExtension);

    if(irpStack->MinorFunction == IRP_MN_START_DEVICE)
    {
        ASSERT(deviceExtension->IdleReqPend == 0);
    }
    else
    {
        if(deviceExtension->SSEnable)
            CancelSelectSuspend(deviceExtension);
    }

    wDbgPrint(2, (PnPMinorFunctionString(irpStack->MinorFunction)));

    switch(irpStack->MinorFunction)
    {
        case IRP_MN_START_DEVICE:
            ntStatus = HandleStartDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_START_DEVICE returning 0x%x\n",ntStatus));
            break;

        case IRP_MN_QUERY_STOP_DEVICE:

            //
            // if we cannot stop the device, we fail the query stop irp
            //

            ntStatus = CanStopDevice(DeviceObject, Irp);
            if(NT_SUCCESS(ntStatus))
            {
                ntStatus = HandleQueryStopDevice(DeviceObject, Irp);
                wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_QUERY_STOP_DEVICE returning 0x%x\n",ntStatus));
                return ntStatus;
            }
            break;

        case IRP_MN_CANCEL_STOP_DEVICE:
            ntStatus = HandleCancelStopDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_CANCEL_STOP_DEVICE returning 0x%x\n",ntStatus));
            Usb_IoDecrement(deviceExtension);
            return ntStatus;

        case IRP_MN_STOP_DEVICE:
            ntStatus = HandleStopDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_STOP_DEVICE returning 0x%x\n",ntStatus));
            Usb_IoDecrement(deviceExtension);
            return ntStatus;

        case IRP_MN_QUERY_REMOVE_DEVICE:

            //
            // if we cannot remove the device, fail the query remove irp
            //
            ntStatus = HandleQueryRemoveDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_QUERY_REMOVE_DEVICE returning 0x%x\n",ntStatus));
            return ntStatus;

        case IRP_MN_CANCEL_REMOVE_DEVICE:
            ntStatus = HandleCancelRemoveDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_CANCEL_REMOVE_DEVICE returning 0x%x\n",ntStatus));
            Usb_IoDecrement(deviceExtension);
            return ntStatus;

        case IRP_MN_SURPRISE_REMOVAL:
            deviceExtension->DebugDumpFlag = TRUE;
            ntStatus = HandleSurpriseRemoval(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_SURPRISE_REMOVAL returning 0x%x\n",ntStatus));
            Usb_IoDecrement(deviceExtension);
            return ntStatus;

        case IRP_MN_REMOVE_DEVICE:
            deviceExtension->DebugDumpFlag = TRUE;
            ntStatus = HandleRemoveDevice(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_REMOVE_DEVICE returning 0x%x\n",ntStatus));
            return ntStatus;

        case IRP_MN_QUERY_CAPABILITIES:
            ntStatus = HandleQueryCapabilities(DeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: IRP_MN_QUERY_CAPABILITIES returning 0x%x\n",ntStatus));
            break;

        default:
            IoSkipCurrentIrpStackLocation(Irp);
            ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
            wDbgPrint(DBG_DISPATCH, ("DispatchPnP: minor %u, returning 0x%x\n",irpStack->MinorFunction, ntStatus));
            Usb_IoDecrement(deviceExtension);
            return ntStatus;
        } // switch

    //
    // complete request
    //

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //
    // decrement count
    //
    wDbgPrint(DBG_DISPATCH, ("DispatchPnP: device object 0x%x, returning 0x%x\n",DeviceObject,ntStatus));
    Usb_IoDecrement(deviceExtension);
    wDbgPrint(DBG_FUNCTION, ("DispatchPnP - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleStartDevice(  IN PDEVICE_OBJECT   DeviceObject,
                    IN PIRP             Irp)
/*++

Routine Description:

    This is the dispatch routine for IRP_MN_START_DEVICE

Arguments:

    DeviceObject - pointer to a device object.

    Irp - I/O request packet

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    KEVENT            startDeviceEvent;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;
    LARGE_INTEGER     dueTime;

    wDbgPrint(DBG_FUNCTION, ("HandleStartDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    deviceExtension->UsbConfigurationDescriptor = NULL;
    deviceExtension->UsbDeviceDescriptor = NULL;
    deviceExtension->UsbInterface = NULL;
    deviceExtension->PipeContext = NULL;

    //
    // We cannot touch the device (send it any non pnp irps) until a
    // start device has been passed down to the lower drivers.
    // first pass the Irp down
    //

    KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);
    IoCopyCurrentIrpStackLocationToNext(Irp);

    //ntStatus =
    IoSetCompletionRoutine(
                        //DeviceObject,
                        Irp,
                        (PIO_COMPLETION_ROUTINE)IrpCompletionRoutine,
                        (PVOID)&startDeviceEvent,
                        TRUE, // invoke on success
                        TRUE, // invoke on error
                        TRUE);// invoke on cancel

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(ntStatus == STATUS_PENDING)
    {
        KeWaitForSingleObject(&startDeviceEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        ntStatus = Irp->IoStatus.Status;
    }

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_ERROR, ("HandleStartDevice: Lower drivers failed this Irp, status 0x%x\n",ntStatus));
        return ntStatus;
    }

    //
    // Read the device descriptor, configuration descriptor
    // and select the interface descriptors
    //

    ntStatus = ReadandSelectDescriptors(DeviceObject);

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_ERROR, ("HandleStartDevice: ReadandSelectDescriptors failed, status 0x%x\n",ntStatus));
        return ntStatus;
    }

    //
    // enable the symbolic links for system components to open
    // handles to the device
    //

    ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, TRUE);

    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_ERROR, ("HandleStartDevice: IoSetDeviceInterfaceState(TRUE) failed, status 0x%x\n",ntStatus));
        return ntStatus;
    }

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    SET_NEW_PNP_STATE(deviceExtension, Working);
    deviceExtension->QueueState = AllowRequests;
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // initialize wait wake outstanding flag to false.
    // and issue a wait wake.

    deviceExtension->FlagWWOutstanding = 0;

    // clear the wait wake cancel flag
    deviceExtension->FlagWWCancel = 0;

    // init the wait wake IRP pointer
    deviceExtension->WaitWakeIrp = NULL;

    if(deviceExtension->WaitWakeEnable)
    {
        wDbgPrint(DBG_INFO, ("HandleStartDevice: issuing wait wake\n"));
        IssueWaitWake(deviceExtension);
    }

    ProcessQueuedRequests(deviceExtension);

    if(WinXpOrBetter == deviceExtension->WdmVersion)
    {
        deviceExtension->SSEnable = deviceExtension->SSRegistryEnable;

        //
        // set timer.for selective suspend requests
        //

        if(deviceExtension->SSEnable)
        {
            dueTime.QuadPart = -10000 * IDLE_INTERVAL;
            KeSetTimerEx(&deviceExtension->Timer, dueTime, IDLE_INTERVAL, &deviceExtension->DeferredProcCall);
            deviceExtension->FreeIdleIrpCount = 0;
        }
    }
    wDbgPrint(DBG_FUNCTION, ("HandleStartDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
ReadandSelectDescriptors(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine configures the USB device.
    In this routines we get the device descriptor,
    the configuration descriptor and select the
    configuration descriptor.

Arguments:

    DeviceObject - pointer to a device object

Return Value:

    NTSTATUS - NT status value.

--*/
{
    PURB                   urb;
    ULONG                  siz;
    NTSTATUS               ntStatus;
    PDEVICE_EXTENSION      deviceExtension;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor;

    wDbgPrint(DBG_FUNCTION, ("ReadandSelectDescriptors: device object 0x%x\n",DeviceObject));

    urb = NULL;
    deviceDescriptor = NULL;

    deviceExtension = DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // 1. Read the device descriptor
    //

    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if(urb)
    {
        siz = sizeof(USB_DEVICE_DESCRIPTOR);
        deviceDescriptor = ExAllocatePool(NonPagedPool, siz);

        if(deviceDescriptor)
        {
            UsbBuildGetDescriptorRequest(   urb,
                                            (USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                            USB_DEVICE_DESCRIPTOR_TYPE,
                                            0,
                                            0,
                                            deviceDescriptor,
                                            NULL,
                                            siz,
                                            NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(NT_SUCCESS(ntStatus))
            {
                ASSERT(deviceDescriptor->bNumConfigurations);
                ntStatus = ConfigureDevice(DeviceObject);
            }

            if (deviceExtension->UsbDeviceDescriptor)
            {
                ExFreePool(deviceExtension->UsbDeviceDescriptor);
                deviceExtension->UsbDeviceDescriptor = 0;
            }

            deviceExtension->UsbDeviceDescriptor = deviceDescriptor;
            ExFreePool(urb);
        }
        else
        {
                wDbgPrint(DBG_ERROR, ("ReadandSelectDescriptors: Failed to alloc deviceDescriptor, returning STATUS_INSUFFICIENT_RESOURCES\n"));
                ExFreePool(urb);
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            }
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("ReadandSelectDescriptors: Failed to alloc urb, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    wDbgPrint(DBG_FUNCTION, ("ReadandSelectDescriptors - ends\n"));
    return ntStatus;
}

NTSTATUS
ConfigureDevice(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This helper routine reads the configuration descriptor
    for the device in couple of steps.

Arguments:

    DeviceObject - pointer to a device object

Return Value:

    NTSTATUS - NT status value

--*/
{
    PURB                          urb;
    ULONG                         siz;
    NTSTATUS                      ntStatus;
    PDEVICE_EXTENSION             deviceExtension;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;

    wDbgPrint(DBG_FUNCTION, ("ConfigureDevice: device object 0x%x\n",DeviceObject));

    //
    // initialize the variables
    //

    urb = NULL;
    configurationDescriptor = NULL;
    deviceExtension = DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // Read the first configuration descriptor
    // This requires two steps:
    // 1. Read the fixed sized configuration desciptor (CD)
    // 2. Read the CD with all embedded interface and endpoint descriptors
    //

    urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if(urb)
    {
        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR);
        configurationDescriptor = ExAllocatePool(NonPagedPool, siz);

        if(configurationDescriptor)
        {
            UsbBuildGetDescriptorRequest(   urb,
                                            (USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                            USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                            0,
                                            0,
                                            configurationDescriptor,
                                            NULL,
                                            sizeof(USB_CONFIGURATION_DESCRIPTOR),
                                            NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(!NT_SUCCESS(ntStatus))
            {
                wDbgPrint(DBG_ERROR, ("ConfigureDevice: CallUSBD failed on get short config descriptor, status 0x%x\n",ntStatus));
                goto ConfigureDevice_Exit;
            }
        }
        else
        {
            wDbgPrint(DBG_ERROR, ("ConfigureDevice: alloc short config Descriptor failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto ConfigureDevice_Exit;
        }

        siz = configurationDescriptor->wTotalLength;

        ExFreePool(configurationDescriptor);

        configurationDescriptor = ExAllocatePool(NonPagedPool, siz);

        if(configurationDescriptor)
        {
            UsbBuildGetDescriptorRequest(   urb,
                                            (USHORT)sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                            USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                            0,
                                            0,
                                            configurationDescriptor,
                                            NULL,
                                            siz,
                                            NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(!NT_SUCCESS(ntStatus))
            {
                wDbgPrint(DBG_ERROR, ("ConfigureDevice: CallUSBD failed on get full config descriptor, status 0x%x\n", ntStatus));
                goto ConfigureDevice_Exit;
            }
        }
        else
        {
            wDbgPrint(DBG_ERROR, ("ConfigureDevice: alloc full config Descriptor failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto ConfigureDevice_Exit;
        }
    }
    else
    {

        wDbgPrint(DBG_ERROR, ("ConfigureDevice: urb alloc failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto ConfigureDevice_Exit;
    }
    if(configurationDescriptor)
    {
        //
        // save a copy of configurationDescriptor in deviceExtension
        // remember to free it later.
        //

        if (deviceExtension->UsbConfigurationDescriptor)
        {
            // if already allocated, free existing
            ExFreePool(deviceExtension->UsbConfigurationDescriptor);
            deviceExtension->UsbConfigurationDescriptor = 0;
        }

        deviceExtension->UsbConfigurationDescriptor = configurationDescriptor;

        if(configurationDescriptor->bmAttributes & REMOTE_WAKEUP_MASK)
        {
            //
            // this configuration supports remote wakeup
            //
            deviceExtension->WaitWakeEnable = 1;
        }
        else
        {
            deviceExtension->WaitWakeEnable = 0;
        }

        ntStatus = SelectInterfaces(DeviceObject, configurationDescriptor);
    }
    else
        deviceExtension->UsbConfigurationDescriptor = NULL;

ConfigureDevice_Exit:

    if(urb)
        ExFreePool(urb);

    wDbgPrint(DBG_FUNCTION, ("ConfigureDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
SelectInterfaces(
    IN PDEVICE_OBJECT                DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/*++

Routine Description:

    This helper routine selects the configuration

Arguments:

    DeviceObject - pointer to device object
    ConfigurationDescriptor - pointer to the configuration
    descriptor for the device

Return Value:

    NT status value

--*/
{
    LONG                        numberOfInterfaces,
                                interfaceNumber,
                                interfaceindex;
    ULONG                       i;
    PURB                        urb;
    PUCHAR                      pInf;
    NTSTATUS                    ntStatus;
    PDEVICE_EXTENSION           deviceExtension;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    PUSBD_INTERFACE_LIST_ENTRY  interfaceList,
                                tmp;
    PUSBD_INTERFACE_INFORMATION Interface;

    wDbgPrint(DBG_FUNCTION, ("SelectInterfaces: device object 0x%x\n",DeviceObject));

    //
    // initialize the variables
    //

    urb = NULL;
    Interface = NULL;
    interfaceDescriptor = NULL;
    deviceExtension = DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    numberOfInterfaces = ConfigurationDescriptor->bNumInterfaces;
    interfaceindex = interfaceNumber = 0;

    //
    // Parse the configuration descriptor for the interface;
    //

    tmp = interfaceList = ExAllocatePool(NonPagedPool, sizeof(USBD_INTERFACE_LIST_ENTRY) * (numberOfInterfaces + 1));

    if(!tmp)
    {
        wDbgPrint(DBG_ERROR, ("SelectInterfaces: interfaceList alloc failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    while(interfaceNumber < numberOfInterfaces)
    {
        interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                                            ConfigurationDescriptor,
                                            ConfigurationDescriptor,
                                            interfaceindex,
                                            0, -1, -1, -1);

        if(interfaceDescriptor)
        {
            interfaceList->InterfaceDescriptor = interfaceDescriptor;
            interfaceList->Interface = NULL;
            interfaceList++;
            interfaceNumber++;
        }

        interfaceindex++;
    }

    interfaceList->InterfaceDescriptor = NULL;
    interfaceList->Interface = NULL;
    urb = USBD_CreateConfigurationRequestEx(ConfigurationDescriptor, tmp);

    if(urb)
    {
        Interface = &urb->UrbSelectConfiguration.Interface;

        for(i = 0; i< Interface->NumberOfPipes; i++)
        {
            //
            // perform pipe initialization here
            // set the transfer size and any pipe flags we use
            // USBD sets the rest of the Interface struct members
            //

            Interface->Pipes[i].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
        }

        ntStatus = CallUSBD(DeviceObject, urb);

        if(NT_SUCCESS(ntStatus))
        {
            //
            // save a copy of interface information in the device extension.
            //
            deviceExtension->UsbInterface = ExAllocatePool(NonPagedPool, Interface->Length);

            if(deviceExtension->UsbInterface)
            {
                RtlCopyMemory(deviceExtension->UsbInterface, Interface, Interface->Length);
            }
            else
            {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                wDbgPrint(DBG_ERROR, ("SelectInterfaces: UsbInterface alloc failed, status STATUS_INSUFFICIENT_RESOURCES\n"));
            }

            //
            // Dump the interface to the debugger
            //

            Interface = &urb->UrbSelectConfiguration.Interface;

            wDbgPrint(DBG_INFO, ("NumberOfPipes %u\n", Interface->NumberOfPipes));
            wDbgPrint(DBG_INFO, ("Length %u\n", Interface->Length));
            wDbgPrint(DBG_INFO, ("Alt Setting %u\n", Interface->AlternateSetting));
            wDbgPrint(DBG_INFO, ("Interface Number %u\n", Interface->InterfaceNumber));
            wDbgPrint(DBG_INFO, ("Class, subclass, protocol 0x%x 0x%x 0x%x\n", Interface->Class, Interface->SubClass, Interface->Protocol));
            //
            // Initialize the PipeContext
            // Dump the pipe info
            //

            deviceExtension->PipeContext = ExAllocatePool(NonPagedPool,  Interface->NumberOfPipes * sizeof(USB_PIPE_CONTEXT));

            if(deviceExtension->PipeContext)
            {
                for(i=0; i<Interface->NumberOfPipes; i++)
                    deviceExtension->PipeContext[i].PipeOpen = FALSE;
            }
            else
            {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                wDbgPrint(DBG_ERROR, ("SelectInterfaces: UsbInterface alloc failed, status STATUS_INSUFFICIENT_RESOURCES\n"));
            }

            for(i=0; i<Interface->NumberOfPipes; i++)
            {
                wDbgPrint(DBG_INFO, ("PipeType 0x%x\n",        (ULONG)Interface->Pipes[i].PipeType));
                wDbgPrint(DBG_INFO, ("EndpointAddress 0x%x\n", Interface->Pipes[i].EndpointAddress));
                wDbgPrint(DBG_INFO, ("MaxPacketSize 0x%x\n",   Interface->Pipes[i].MaximumPacketSize));
                wDbgPrint(DBG_INFO, ("Interval 0x%x\n",        Interface->Pipes[i].Interval));
                wDbgPrint(DBG_INFO, ("Handle 0x%x\n",          Interface->Pipes[i].PipeHandle));
                wDbgPrint(DBG_INFO, ("MaximumTransferSize 0x%x\n", Interface->Pipes[i].MaximumTransferSize));
            }
        }
        else
            wDbgPrint(DBG_ERROR, ("SelectInterfaces: Failed to select an interface\n"));
    }
    else
    {
        wDbgPrint(DBG_ERROR, ("SelectInterfaces: USBD_CreateConfigurationRequestEx failed\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if(tmp)
        ExFreePool(tmp);

    if(urb)
        ExFreePool(urb);

    wDbgPrint(DBG_FUNCTION, ("SelectInterfaces - ends\n"));
    return ntStatus;
}


NTSTATUS
DeconfigureDevice(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is invoked when the device is removed or stopped.
    This routine de-configures the usb device.

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
    PURB     urb;
    ULONG    siz;
    NTSTATUS ntStatus;

    wDbgPrint(DBG_FUNCTION, ("DeconfigureDevice - begins\n"));
    //
    // initialize variables
    //

    siz = sizeof(struct _URB_SELECT_CONFIGURATION);
    urb = ExAllocatePool(NonPagedPool, siz);

    if(urb)
    {
        UsbBuildSelectConfigurationRequest(urb, (USHORT)siz, NULL);

        ntStatus = CallUSBD(DeviceObject, urb);

        if(!NT_SUCCESS(ntStatus))
            wDbgPrint(DBG_ERROR, ("DeconfigureDevice: Failed to deconfigure device, status 0x%x\n",ntStatus));

        ExFreePool(urb);
    }
    else
    {

        wDbgPrint(DBG_ERROR, ("DeconfigureDevice: urb alloc failed, STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    wDbgPrint(DBG_FUNCTION, ("DeconfigureDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
CallUSBD(   IN PDEVICE_OBJECT DeviceObject,
            IN PURB           Urb)
/*++

Routine Description:

    This routine synchronously submits an urb down the stack.

Arguments:

    DeviceObject - pointer to device object
    Urb - USB request block

Return Value:

--*/
{
    PIRP               irp;
    KEVENT             event;
    NTSTATUS           ntStatus;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION  deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("CallUSBD: device object 0x%x, Urb 0x%x\n",DeviceObject,Urb));

    //
    // initialize the variables
    //

    irp = NULL;
    deviceExtension = DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB,
                                        deviceExtension->TopOfStackDeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,
                                        &event,
                                        &ioStatus);

    if(!irp)
    {
        wDbgPrint(DBG_ERROR, ("CallUSBD: IoBuildDeviceIoControlRequest failed, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);
    nextStack->Parameters.Others.Argument1 = Urb;

    Usb_IoIncrement(deviceExtension);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(ntStatus == STATUS_PENDING)
    {
        LARGE_INTEGER  Timeout;
        NTSTATUS EventStatus;

        Timeout.QuadPart = -500*1000*10; // 500 ms

        wDbgPrint(DBG_FUNCTION, ("CallUSBD: waiting for IoCallDriver to return\n"));
        EventStatus = KeWaitForSingleObject(&event,   //IN PVOID  Object,
                          Executive,    //IN KWAIT_REASON  WaitReason,
                          KernelMode,   //IN KPROCESSOR_MODE  WaitMode,
                          FALSE,        //IN BOOLEAN  Alertable,
                          &Timeout);        //IN PLARGE_INTEGER  Timeout  OPTIONAL

        if (STATUS_TIMEOUT == EventStatus)
        {
            //A time-out occurred before the object was set to a signaled state.
            wDbgPrint(DBG_ERROR, ("CallUSBD: wait timeout\n"));
            ntStatus = STATUS_IO_TIMEOUT;
        }
        else if (NT_SUCCESS(EventStatus))
        {
            ntStatus = ioStatus.Status;
            wDbgPrint(DBG_INFO, ("CallUSBD: wait status 0x%x, IoCallDriver status 0x%x\n",EventStatus,ntStatus));
        }
        else
        {
            wDbgPrint(DBG_ERROR, ("CallUSBD: wait error 0x%x\n",ntStatus));
        }
    }
    else
    {
        wDbgPrint(DBG_INFO, ("CallUSBD: IoCallDriver returned immediately with status 0x%x\n", ntStatus));
    }

    wDbgPrint(DBG_FUNCTION, ("CallUSBD: returning 0x%x\n", ntStatus));
    Usb_IoDecrement(deviceExtension);
    return ntStatus;
}

NTSTATUS
HandleQueryStopDevice(  IN PDEVICE_OBJECT DeviceObject,
                        IN PIRP           Irp)
/*++

Routine Description:

    This routine services the Irps of minor type IRP_MN_QUERY_STOP_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleQueryStopDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // If we can stop the device, we need to set the QueueState to
    // HoldRequests so further requests will be queued.
    //

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    SET_NEW_PNP_STATE(deviceExtension, PendingStop);
    deviceExtension->QueueState = HoldRequests;
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // wait for the existing ones to be finished.
    // first, decrement this operation
    //

    wDbgPrint(DBG_INFO, ("HandleQueryStopDevice \n"));
    Usb_IoDecrement(deviceExtension);

    KeWaitForSingleObject(&deviceExtension->StopEvent, Executive, KernelMode, FALSE, NULL);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    wDbgPrint(DBG_FUNCTION, ("HandleQueryStopDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleCancelStopDevice( IN PDEVICE_OBJECT DeviceObject,
                        IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_CANCEL_STOP_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT value

--*/
{
    KIRQL             oldIrql;
    KEVENT            event;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleCancelStopDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);
    //
    // First check to see whether you have received cancel-stop
    // without first receiving a query-stop. This could happen if someone
    // above us fails a query-stop and passes down the subsequent
    // cancel-stop.
    //

    if(PendingStop == deviceExtension->DeviceState)
    {
        // spurious Irp
        //
        // If the device is already in an active state when the driver
        // receives this IRP, a function driver simply sets status to
        // success and passes the IRP to the next driver.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
    }
    //
    // Send this IRP down and wait for it to come back.
    // Set the QueueState flag to AllowRequests,
    // and process all the previously queued up IRPs.
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);
    //ntStatus =
    IoSetCompletionRoutine(
                    //DeviceObject,
                    Irp,
                    (PIO_COMPLETION_ROUTINE)IrpCompletionRoutine,
                    (PVOID)&event,
                    TRUE, // invoke on success
                    TRUE, // invoke on error
                    TRUE);// invoke on cancel

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(ntStatus == STATUS_PENDING)
    {
        KeWaitForSingleObject(&event,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
        ntStatus = Irp->IoStatus.Status;
    }

    if(NT_SUCCESS(ntStatus))
    {
        KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
        RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        deviceExtension->QueueState = AllowRequests;
        ASSERT(deviceExtension->DeviceState == Working);
        KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);
        ProcessQueuedRequests(deviceExtension);
    }

    //
    // Complete the IRP that our completion routine pended
    //
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    wDbgPrint(DBG_FUNCTION, ("HandleCancelStopDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleStopDevice(   IN PDEVICE_OBJECT DeviceObject,
                    IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_STOP_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleStopDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    if(WinXpOrBetter == deviceExtension->WdmVersion)
    {
        if(deviceExtension->SSEnable)
        {
            //
            // Cancel the timer so that the DPCs are no longer fired.
            // Thus, we are making judicious usage of our resources.
            // we do not need DPCs because the device is stopping.
            // The timers are re-initialized while handling the start
            // device irp.
            //

            KeCancelTimer(&deviceExtension->Timer);

            //
            // after the device is stopped, it can be surprise removed.
            // we set this to 0, so that we do not attempt to cancel
            // the timer while handling surprise remove or remove irps.
            // when we get the start device request, this flag will be
            // reinitialized.
            //
            deviceExtension->SSEnable = 0;

            //
            // make sure that if a DPC was fired before we called cancel timer,
            // then the DPC and work-time have run to their completion
            //
            KeWaitForSingleObject(&deviceExtension->NoDpcWorkItemPendingEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            //
            // make sure that the selective suspend request has been completed.
            //
            KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        }
    }

    //
    // after the stop Irp is sent to the lower driver object,
    // the driver must not send any more Irps down that touch
    // the device until another Start has occurred.
    //

    if(deviceExtension->WaitWakeEnable)
        CancelWaitWake(deviceExtension);

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    SET_NEW_PNP_STATE(deviceExtension, Stopped);
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // This is the right place to actually give up all the resources used
    //

    ReleaseMemory(DeviceObject);

    ntStatus = DeconfigureDevice(DeviceObject);

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    wDbgPrint(DBG_FUNCTION, ("HandleStopDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleQueryRemoveDevice(    IN PDEVICE_OBJECT DeviceObject,
                            IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_QUERY_REMOVE_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleQueryRemoveDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // If we can allow removal of the device, we should set the QueueState
    // to HoldRequests so further requests will be queued. This is required
    // so that we can process queued up requests in cancel-remove just in
    // case somebody else in the stack fails the query-remove.
    //

    ntStatus = CanRemoveDevice(DeviceObject, Irp);

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

    deviceExtension->QueueState = HoldRequests;
    SET_NEW_PNP_STATE(deviceExtension, PendingRemove);

    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    wDbgPrint(DBG_INFO, ("HandleQueryRemoveDevice\n"));
    Usb_IoDecrement(deviceExtension);

    //
    // wait for all the requests to be completed
    //

    KeWaitForSingleObject(&deviceExtension->StopEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    wDbgPrint(DBG_FUNCTION, ("HandleQueryRemoveDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleCancelRemoveDevice(   IN PDEVICE_OBJECT DeviceObject,
                            IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_CANCEL_REMOVE_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    KEVENT            kevent;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleCancelRemoveDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // We need to reset the QueueState flag to ProcessRequest,
    // since the device resume its normal activities.
    //

    //
    // First check to see whether you have received cancel-remove
    // without first receiving a query-remove. This could happen if
    // someone above us fails a query-remove and passes down the
    // subsequent cancel-remove.
    //

    if(PendingRemove == deviceExtension->DeviceState)
    {
        KeInitializeEvent(&kevent, NotificationEvent, FALSE);

        IoCopyCurrentIrpStackLocationToNext(Irp);
        //ntStatus =
        IoSetCompletionRoutine(
                        //DeviceObject,
                        Irp,
                        (PIO_COMPLETION_ROUTINE)IrpCompletionRoutine,
                        (PVOID)&kevent,
                        TRUE, // invoke on success
                        TRUE, // invoke on error
                        TRUE);// invoke on cancel

        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        if(ntStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&kevent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            ntStatus = Irp->IoStatus.Status;
        }

        if(NT_SUCCESS(ntStatus))
        {
            KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
            deviceExtension->QueueState = AllowRequests;
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
            KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);
            //
            // process the queued requests that arrive between
            // QUERY_REMOVE and CANCEL_REMOVE
            //

            ProcessQueuedRequests(deviceExtension);
        }
        //
        // Complete the IRP that our completion routine pended
        //
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    }
    else
    {
        //
        // spurious cancel-remove
        //
        // If the device is already started when the driver receives
        // this IRP, the driver simply sets status to success and passes
        // the IRP to the next driver.  For such a cancel-remove IRP, a
        // function driver need not set a completion routine. The device
        // may not be in the remove-pending state, because, for example,
        // the driver failed the previous IRP_MN_QUERY_REMOVE_DEVICE.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        IoSkipCurrentIrpStackLocation(Irp);
        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    }
    wDbgPrint(DBG_FUNCTION, ("HandleCancelRemoveDevice - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleSurpriseRemoval(  IN PDEVICE_OBJECT DeviceObject,
                        IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_SURPRISE_REMOVAL

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    PFILE_OBJECT                fileObject;
    PIO_STACK_LOCATION          irpStack;


    wDbgPrint(DBG_FUNCTION, ("HandleSurpriseRemoval: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    // examine the associated FILE_OBJECT
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    fileObject = irpStack->FileObject;

    // 1. Determine if the device has been removed. The driver must always attempt
    // to determine if the device is still connected. If it is, the driver must
    // attempt to stop the device and disable it.

    // No action required, USB device

    // 2. Release the device's hardware resources--interrupts, I/O ports, memory
    // registers, and DMA channels.

    // nothing to do.

    // 3. In a parent bus driver, power down the bus slot if the driver is capable
    // of doing so. Call PoSetPowerState to notify the power manager.

    // not applicable

    // 4. Prevent any new I/O operations on the device.  A driver should process
    // subsequent IRP_MJ_CLEANUP, IRP_MJ_CLOSE, IRP_MJ_POWER, and IRP_MJ_PNP requests,
    // but the driver must prevent any new I/O operations. A driver must fail any
    // subsequent IRPs that the driver would have handled if the device were present,
    // besides close, clean-up, and PnP IRPs.

    wDbgPrint(DBG_INFO, ("HandleSurpriseRemoval: setting device state to SurpriseRemoved, queue state to FailRequests\n"));
    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    deviceExtension->QueueState = FailRequests;
    SET_NEW_PNP_STATE(deviceExtension, SurpriseRemoved);
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);


    // 5. Fail or cancel outstanding I/O requests on the device.

    // cancel wait wake request
    if(deviceExtension->WaitWakeEnable)
        CancelWaitWake(deviceExtension);

    // cancel (wait for completion) selective suspend request
    if(WinXpOrBetter == deviceExtension->WdmVersion)
    {
        if(deviceExtension->SSEnable)
        {
            //
            // Cancel the timer so that the DPCs are no longer fired.
            // we do not need DPCs because the device has been surprise
            // removed
            //

            KeCancelTimer(&deviceExtension->Timer);
            deviceExtension->SSEnable = 0;

            //
            // make sure that if a DPC was fired before we called cancel timer,
            // then the DPC and work item have run to completion
            //
            KeWaitForSingleObject(&deviceExtension->NoDpcWorkItemPendingEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            //
            // make sure that the selective suspend request has been completed.
            //
            KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        }
    }

    // cancel queued requests
    ProcessQueuedRequests(deviceExtension);

    // 6. Continue to pass down any IRPs that the driver does not handle for the device.
    // ??

    // 7. Disable device interfaces with IoSetDeviceInterfaceState.

    ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, FALSE);

    if(!NT_SUCCESS(ntStatus))
        wDbgPrint(DBG_ERROR, ("HandleSurpriseRemoval: IoSetDeviceInterfaceState(disable) failed, 0x%x\n",ntStatus));

    // 8. Clean up any device-specific allocations, memory, events, or other system resources. A driver
    // could postpone such clean-up until it receives the subsequent IRP_MN_REMOVE_DEVICE request, but
    // if a legacy component has an open handle that cannot be closed, the remove IRP will never be sent.


    ntStatus = AbortPipes(DeviceObject);
    if(!NT_SUCCESS(ntStatus))
        wDbgPrint(DBG_ERROR, ("HandleSurpriseRemoval: AbortPipes failed, 0x%x\n",ntStatus));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    wDbgPrint(DBG_INFO, ("HandleSurpriseRemoval: returning 0x%x\n",ntStatus));
    wDbgPrint(DBG_FUNCTION, ("HandleSurpriseRemoval - ends\n"));
    return ntStatus;
}

NTSTATUS
HandleRemoveDevice( IN PDEVICE_OBJECT DeviceObject,
                    IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_REMOVE_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    KEVENT            event;
    ULONG             requestCount;
    NTSTATUS          ntStatus;
    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("HandleRemoveDevice: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // The Plug & Play system has dictated the removal of this device.  We
    // have no choice but to detach and delete the device object.
    //

    if(SurpriseRemoved != deviceExtension->DeviceState)
    {
        //
        // we are here after QUERY_REMOVE
        //

        wDbgPrint(DBG_INFO, ("HandleRemoveDevice: not previously surprised removed\n",DeviceObject,Irp));
        KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
        deviceExtension->QueueState = FailRequests;
        KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

        if(deviceExtension->WaitWakeEnable)
        {
            CancelWaitWake(deviceExtension);
        }

        if(WinXpOrBetter == deviceExtension->WdmVersion)
        {
            if(deviceExtension->SSEnable)
            {
                //
                // Cancel the timer so that the DPCs are no longer fired.
                // we do not need DPCs because the device has been removed
                //
                KeCancelTimer(&deviceExtension->Timer);
                deviceExtension->SSEnable = 0;

                //
                // make sure that if a DPC was fired before we called cancel timer,
                // then the DPC and work-time have run to their completion
                //
                KeWaitForSingleObject(&deviceExtension->NoDpcWorkItemPendingEvent, Executive, KernelMode, FALSE, NULL);
                //
                // make sure that the selective suspend request has been completed.
                //
                KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, Executive, KernelMode, FALSE, NULL);
            }
        }

        ProcessQueuedRequests(deviceExtension);

        ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, FALSE);

        if(!NT_SUCCESS(ntStatus))
        {
            wDbgPrint(DBG_ERROR, ("HandleRemoveDevice: IoSetDeviceInterfaceState failed, 0x%x\n",ntStatus));
        }
#ifdef ENABLE_WMI_SUPPORT
        RtlFreeUnicodeString(&deviceExtension->InterfaceName);
        WmiDeRegistration(deviceExtension);
#endif
        ntStatus = AbortPipes(DeviceObject);
        if(!NT_SUCCESS(ntStatus))
            wDbgPrint(DBG_ERROR, ("HandleRemoveDevice: AbortPipes failed, 0x%x\n",ntStatus));
    }
    else
    {
        // surprise removal already performed
        wDbgPrint(DBG_INFO, ("HandleRemoveDevice: device was already surprised-removed\n"));
    }

    //
    // set device PNP state to Removed
    //
    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    SET_NEW_PNP_STATE(deviceExtension, Removed);
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // need 2 decrements to reach 0
    //

    requestCount = Usb_IoDecrement(deviceExtension);
    ASSERT(requestCount == 1);
    requestCount = Usb_IoDecrement(deviceExtension);

    // wait on the event that was signaled in Usb_IoDecrement()
    KeWaitForSingleObject(&deviceExtension->RemoveEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    // free allocated objects in device extension
    ReleaseMemory(DeviceObject);

    //
    // We need to send the remove down the stack before we detach,
    // but we don't need to wait for the completion of this operation
    // or register a completion routine.
    //

    //
    // report success and "complete" the IRP
    // by calling the next lower driver
    //
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    if(!NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_ERROR, ("HandleRemoveDevice: final IoCallDriver failed with 0x%x\n",ntStatus));
    }

    //
    // Detach our FDO from the device stack
    //
    IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
    IoDeleteDevice(DeviceObject);

    wDbgPrint(DBG_FUNCTION, ("HandleRemoveDevice - ends\n"));
    wDbgPrint(DBG_INFO, ("HandleRemoveDevice: device object 0x%x is removed\n",DeviceObject));
    return STATUS_SUCCESS;
}

NTSTATUS
HandleQueryCapabilities(    IN PDEVICE_OBJECT DeviceObject,
                            IN PIRP           Irp)
/*++

Routine Description:

    This routine services Irp of minor type IRP_MN_QUERY_CAPABILITIES

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    ULONG                i;
    KEVENT               kevent;
    NTSTATUS             ntStatus;
    PDEVICE_EXTENSION    deviceExtension;
    PDEVICE_CAPABILITIES pdc;
    PIO_STACK_LOCATION   irpStack;

    wDbgPrint(DBG_FUNCTION, ("HandleQueryCapabilities: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;



    if(pdc->Version < 1 || pdc->Size < sizeof(DEVICE_CAPABILITIES))
    {

        ntStatus = STATUS_UNSUCCESSFUL;
        wDbgPrint(DBG_ERROR, ("HandleQueryCapabilities: DEVICE_CAPABILITIES structure wrong version or size, 0x%x\n",ntStatus));
        return ntStatus;
    }

    //
    // Add in the SurpriseRemovalOK bit before passing it down.
    //
    pdc->SurpriseRemovalOK = TRUE;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    KeInitializeEvent(&kevent, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);
    //ntStatus =
    IoSetCompletionRoutine(
                        //DeviceObject,
                        Irp,
                        (PIO_COMPLETION_ROUTINE)IrpCompletionRoutine,
                        (PVOID)&kevent,
                        TRUE, // invoke on success
                        TRUE, // invoke on error
                        TRUE);// invoke on cancel

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(ntStatus == STATUS_PENDING)
    {
        KeWaitForSingleObject(&kevent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
        ntStatus = Irp->IoStatus.Status;
    }

    //
    // initialize PowerDownLevel to disabled
    //

    deviceExtension->PowerDownLevel = PowerDeviceUnspecified;

    if(NT_SUCCESS(ntStatus))
    {
        deviceExtension->DeviceCapabilities = *pdc;

        for(i = PowerSystemSleeping1; i <= PowerSystemSleeping3; i++)
        {
            if(deviceExtension->DeviceCapabilities.DeviceState[i] < PowerDeviceD3)
                deviceExtension->PowerDownLevel = deviceExtension->DeviceCapabilities.DeviceState[i];
        }
        //
        // since its safe to surprise-remove this device, we shall
        // set the SurpriseRemoveOK flag to supress any dialog to
        // user.
        //

        pdc->SurpriseRemovalOK = 1;
    }

    if(deviceExtension->PowerDownLevel == PowerDeviceUnspecified ||
       deviceExtension->PowerDownLevel <= PowerDeviceD0)
    {
        // if D0 was not requested, go to to D2
        deviceExtension->PowerDownLevel = PowerDeviceD2;
    }

    wDbgPrint(DBG_FUNCTION, ("HandleQueryCapabilities - ends\n"));
    return ntStatus;
}


VOID
DpcRoutine( IN PKDPC Dpc,
            IN PVOID DeferredContext,
            IN PVOID SystemArgument1,
            IN PVOID SystemArgument2)
/*++

Routine Description:

    DPC routine triggered by the timer to check the idle state
    of the device and submit an idle request for the device.

Arguments:

    DeferredContext - context for the dpc routine.
                      DeviceObject in our case.

Return Value:

    None

--*/
{
    NTSTATUS          ntStatus;
    PDEVICE_OBJECT    deviceObject;
    PDEVICE_EXTENSION deviceExtension;
    PIO_WORKITEM      item;

    wDbgPrint(DBG_FUNCTION, ("DpcRoutine: device object 0x%x\n",DeferredContext));

    deviceObject = (PDEVICE_OBJECT)DeferredContext;
    deviceExtension = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    //
    // Clear this event since a DPC has been fired!
    //
    KeClearEvent(&deviceExtension->NoDpcWorkItemPendingEvent);

    if(CanDeviceSuspend(deviceExtension))
    {
        wDbgPrint(DBG_INFO, ("DpcRoutine: Device is Idle\n"));

        item = IoAllocateWorkItem(deviceObject);

        if(item)
        {
            IoQueueWorkItem(item, IdleRequestWorkerRoutine, DelayedWorkQueue, item);
            ntStatus = STATUS_PENDING;
        }
        else
        {
            wDbgPrint(DBG_INFO, ("DpcRoutine; Cannot alloc memory for work item\n"));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;

            //
            // signal the NoDpcWorkItemPendingEvent.
            //
            KeSetEvent(&deviceExtension->NoDpcWorkItemPendingEvent, IO_NO_INCREMENT,  FALSE);
        }
    }
    else
    {
        //
        // signal the NoDpcWorkItemPendingEvent.
        //
        wDbgPrint(DBG_INFO, ("DpcRoutine: Idle event not signaled\n"));
        KeSetEvent(&deviceExtension->NoDpcWorkItemPendingEvent, IO_NO_INCREMENT, FALSE);
    }

    wDbgPrint(DBG_FUNCTION, ("DpcRoutine - ends\n"));
}

VOID
IdleRequestWorkerRoutine(   IN PDEVICE_OBJECT DeviceObject,
                            IN PVOID          Context)
/*++

Routine Description:

    This is the work item fired from the DPC.
    This workitem checks the idle state of the device
    and submits an idle request.

Arguments:

    DeviceObject - pointer to device object
    Context - context for the work item.

Return Value:

    None

--*/
{
    PIRP                   irp;
    NTSTATUS               ntStatus;
    PDEVICE_EXTENSION      deviceExtension;
    PIO_WORKITEM           workItem;

    wDbgPrint(DBG_FUNCTION, ("IdleRequestWorkerRoutine: device object 0x%x, workItem 0x%x\n",DeviceObject,Context));

    //
    // initialize variables
    //
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);
    workItem = (PIO_WORKITEM) Context;

    if(CanDeviceSuspend(deviceExtension))
    {
        wDbgPrint(DBG_INFO, ("IdleRequestWorkerRoutine: Device is idle\n"));
        ntStatus = SubmitIdleRequestIrp(deviceExtension);

        if(!NT_SUCCESS(ntStatus))
            wDbgPrint(DBG_ERROR, ("IdleRequestWorkerRoutine: SubmitIdleRequestIrp failed, 0x%x\n",ntStatus));
    }
    else
        wDbgPrint(DBG_INFO, ("IdleRequestWorkerRoutine: Device is not idle\n"));

    IoFreeWorkItem(workItem);
    //
    // signal the NoDpcWorkItemPendingEvent.
    //
    KeSetEvent(&deviceExtension->NoDpcWorkItemPendingEvent, IO_NO_INCREMENT, FALSE);
    wDbgPrint(DBG_FUNCTION, ("IdleRequestsWorkerRoutine - ends\n"));
}


VOID
ProcessQueuedRequests(IN OUT PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    Remove and process the entries in the queue. If this routine is called
    when processing IRP_MN_CANCEL_STOP_DEVICE, IRP_MN_CANCEL_REMOVE_DEVICE
    or IRP_MN_START_DEVICE, the requests are passed to the next lower driver.
    If the routine is called when IRP_MN_REMOVE_DEVICE is received, the IRPs
    are complete with STATUS_DELETE_PENDING

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    None

--*/
{
    KIRQL       oldIrql;
    PIRP        nextIrp,
                cancelledIrp;
    PVOID       cancelRoutine;
    LIST_ENTRY  cancelledIrpList;
    PLIST_ENTRY listEntry;
    NTSTATUS    ntStatus;

    wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests - begins\n"));

    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    //
    // initialize variables
    //

    cancelRoutine = NULL;
    InitializeListHead(&cancelledIrpList);

    //
    // 1.  dequeue the entries in the queue
    // 2.  reset the cancel routine
    // 3.  process them
    // 3a. if the device is active, send them down
    // 3b. else complete with STATUS_DELETE_PENDING
    //

    while(1)
    {
        KeAcquireSpinLock(&DeviceExtension->QueueLock, &oldIrql);

        if(IsListEmpty(&DeviceExtension->NewRequestsQueue))
        {
            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);
            wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: new requests list is empty\n"));
            break;
        }

        //
        // Remove a request from the queue
        //

        listEntry = RemoveHeadList(&DeviceExtension->NewRequestsQueue);
        nextIrp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);

        //
        // set the cancel routine to NULL
        //

        cancelRoutine = IoSetCancelRoutine(nextIrp, NULL);

        if(nextIrp->Cancel)
        {
            if(cancelRoutine)
            {
                //
                // the cancel routine for this IRP hasnt been called yet
                // so queue the IRP in the cancelledIrp list and complete
                // after releasing the lock
                //

                InsertTailList(&cancelledIrpList, listEntry);
                wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: moved irp 0x%x to cancel list\n",nextIrp));
            }
            else
            {
                //
                // the cancel routine has run
                // it must be waiting to hold the queue lock
                // so initialize the IRPs listEntry
                //

                InitializeListHead(listEntry);
                wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: irp 0x%x cancel routine already ran\n",nextIrp));
            }

            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);
        }
        else
        {

            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);

            if(FailRequests == DeviceExtension->QueueState)
            {
                wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: queue state is FailRequests! completing irp 0x%x with STATUS_DELETE_PENDING\n",nextIrp));
                nextIrp->IoStatus.Information = 0;
                nextIrp->IoStatus.Status = STATUS_DELETE_PENDING;
                IoCompleteRequest(nextIrp, IO_NO_INCREMENT);
            }
            else
            {
                //PIO_STACK_LOCATION irpStack;
                wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: sending IRP 0x%x using IoCallDriver\n",nextIrp));
                Usb_IoIncrement(DeviceExtension);
                IoSkipCurrentIrpStackLocation(nextIrp);
                ntStatus = IoCallDriver(DeviceExtension->TopOfStackDeviceObject, nextIrp);
                if (!NT_SUCCESS(ntStatus))
                {
                    wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: IoCallDriver returned 0x%x for irp 0x%x\n",ntStatus, nextIrp));
                }
                Usb_IoDecrement(DeviceExtension);
            }
        }
    } // while loop

    //
    // walk through the cancelledIrp list and cancel them
    //

    while(!IsListEmpty(&cancelledIrpList))
    {
        PLIST_ENTRY listEntry = RemoveHeadList(&cancelledIrpList);

        cancelledIrp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);
        wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests: completing irp 0x%x with STATUS_CANCELLED\n",cancelledIrp));
        cancelledIrp->IoStatus.Status = STATUS_CANCELLED;
        cancelledIrp->IoStatus.Information = 0;
        IoCompleteRequest(cancelledIrp, IO_NO_INCREMENT);
    }

    wDbgPrint(DBG_QUEUE, ("ProcessQueuedRequests - ends\n"));
    return;
}

NTSTATUS
GetRegistryDword(   IN     PWCHAR RegPath,
                    IN     PWCHAR ValueName,
                    IN OUT PULONG Value)
/*++

Routine Description:

    This routine reads the specified reqistry value.

Arguments:

    RegPath - registry path
    ValueName - property to be fetched from the registry
    Value - corresponding value read from the registry.

Return Value:

    NT status value

--*/
{
    ULONG                    defaultData;
    WCHAR                    buffer[MAXIMUM_FILENAME_LENGTH];
    NTSTATUS                 ntStatus;
    UNICODE_STRING           regPath;
    RTL_QUERY_REGISTRY_TABLE paramTable[2];

    wDbgPrint(DBG_FUNCTION, ("GetRegistryDword - begins\n"));

    regPath.Length = 0;
    regPath.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);
    regPath.Buffer = buffer;

    RtlZeroMemory(regPath.Buffer, regPath.MaximumLength);
    RtlMoveMemory(regPath.Buffer, RegPath, wcslen(RegPath) * sizeof(WCHAR));
    RtlZeroMemory(paramTable, sizeof(paramTable));

    paramTable[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    paramTable[0].Name          = ValueName;
    paramTable[0].EntryContext  = Value;
    paramTable[0].DefaultType   = REG_DWORD;
    paramTable[0].DefaultData   = &defaultData;
    paramTable[0].DefaultLength = sizeof(ULONG);

    ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL, regPath.Buffer, paramTable, NULL, NULL);

    if(NT_SUCCESS(ntStatus))
    {
        wDbgPrint(DBG_VERBOSE, ("GetRegistryDword: success, Value = %u\n", *Value));
        ntStatus = STATUS_SUCCESS;
    }
    else
    {
        *Value = 0;
        wDbgPrint(DBG_ERROR, ("GetRegistryDword: failed, returning STATUS_UNSUCCESSFUL\n"));
        ntStatus = STATUS_UNSUCCESSFUL;
    }

    wDbgPrint(DBG_FUNCTION, ("GetRegistryDword - ends\n"));
    return ntStatus;
}

NTSTATUS
DispatchClean(  IN PDEVICE_OBJECT DeviceObject,
                IN PIRP           Irp)
/*++

Routine Description:

    Dispatch routine for IRP_MJ_CLEANUP

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager

Return Value:

    NT status value

--*/
{
    PDEVICE_EXTENSION  deviceExtension;
    KIRQL              oldIrql;
    LIST_ENTRY         cleanupList;
    PLIST_ENTRY        thisEntry,
                       nextEntry,
                       listHead;
    PIRP               pendingIrp;
    PIO_STACK_LOCATION pendingIrpStack,
                       irpStack;
    NTSTATUS           ntStatus;

    wDbgPrint(DBG_FUNCTION, ("DispatchClean: device object 0x%x, Irp 0x%x\n",DeviceObject,Irp));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    InitializeListHead(&cleanupList);

    Usb_IoIncrement(deviceExtension);

    //
    // acquire queue lock
    //
    KeAcquireSpinLock(&deviceExtension->QueueLock, &oldIrql);

    //
    // remove all Irp's that belong to input Irp's fileobject
    //

    listHead = &deviceExtension->NewRequestsQueue;

    for(thisEntry = listHead->Flink, nextEntry = thisEntry->Flink;
        thisEntry != listHead;
        thisEntry = nextEntry, nextEntry = thisEntry->Flink)
    {
        pendingIrp = CONTAINING_RECORD(thisEntry, IRP, Tail.Overlay.ListEntry);
        pendingIrpStack = IoGetCurrentIrpStackLocation(pendingIrp);

        if(irpStack->FileObject == pendingIrpStack->FileObject)
        {
            RemoveEntryList(thisEntry);

            //
            // set the cancel routine to NULL
            //
            if(NULL == IoSetCancelRoutine(pendingIrp, NULL))
            {
                InitializeListHead(thisEntry);
            }
            else
                InsertTailList(&cleanupList, thisEntry);
        }
    }

    //
    // Release the spin lock
    //

    KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);

    //
    // walk thru the cleanup list and cancel all the Irps
    //

    while(!IsListEmpty(&cleanupList))
    {
        //
        // complete the Irp
        //
        thisEntry = RemoveHeadList(&cleanupList);

        pendingIrp = CONTAINING_RECORD(thisEntry, IRP, Tail.Overlay.ListEntry);
        pendingIrp->IoStatus.Information = 0;
        pendingIrp->IoStatus.Status = STATUS_CANCELLED;
        IoCompleteRequest(pendingIrp, IO_NO_INCREMENT);
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    wDbgPrint(DBG_FUNCTION, ("DispatchClean - ends\n"));
    Usb_IoDecrement(deviceExtension);
    return STATUS_SUCCESS;
}

BOOLEAN
CanDeviceSuspend(IN PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    This is the routine where we check if the device
    can selectively suspend.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    TRUE - if the device can suspend
    FALSE - otherwise.

--*/
{
    VERIFY_DEVICE_EXTENSION(DeviceExtension);

    if((DeviceExtension->OpenHandleCount == 0) &&
        (DeviceExtension->OutStandingIO == 1))
    {
        wDbgPrint(DBG_INFO, ("CanDeviceSuspend: TRUE\n"));
        return TRUE;
    }
    else
    {
        wDbgPrint(DBG_INFO, ("CanDeviceSuspend: FALSE\n"));
        return FALSE;
    }
}

NTSTATUS
AbortPipes(IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description

    sends an abort pipe request for open pipes.

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
    PURB                        urb;
    ULONG                       i;
    NTSTATUS                    ntStatus;
    PDEVICE_EXTENSION           deviceExtension;
    PUSB_PIPE_CONTEXT           pipeContext;
//    PUSBD_PIPE_INFORMATION      pipeInformation;
    PUSBD_INTERFACE_INFORMATION interfaceInfo;

    wDbgPrint(DBG_FUNCTION, ("AbortPipes: device object 0x%x\n",DeviceObject));

    //
    // initialize variables
    //

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);
    pipeContext = deviceExtension->PipeContext;
    interfaceInfo = deviceExtension->UsbInterface;

    if(interfaceInfo == NULL || pipeContext == NULL)
    {
        wDbgPrint(DBG_WARN, ("AbortPipes: no action taken, interfaceInfo 0x%x pipeContext 0x%x\n",interfaceInfo,pipeContext));
        return STATUS_SUCCESS;
    }

    for(i=0; i<interfaceInfo->NumberOfPipes; i++)
    {
        if(pipeContext[i].PipeOpen)
        {
            wDbgPrint(DBG_ERROR, ("AbortPipes: Aborting open pipe %u\n", i));

            urb = ExAllocatePool(NonPagedPool, sizeof(struct _URB_PIPE_REQUEST));

            if(urb)
            {
                urb->UrbHeader.Length = sizeof(struct _URB_PIPE_REQUEST);
                urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
                urb->UrbPipeRequest.PipeHandle = interfaceInfo->Pipes[i].PipeHandle;
                ntStatus = CallUSBD(DeviceObject, urb);
                ExFreePool(urb);

            }
            else
            {
                wDbgPrint(DBG_ERROR, ("AbortPipes: aborting pipe %u, failed to alloc urb\n"));
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                return ntStatus;
            }
            if(NT_SUCCESS(ntStatus))
            {
                pipeContext[i].PipeOpen = FALSE;
            }
            //else
            {
                wDbgPrint(DBG_WARN, ("AbortPipes: pipe %u, CallUSBD returned 0x%x \n",i, ntStatus));
            }
        }
        else
        {
            wDbgPrint(DBG_WARN, ("AbortPipes: pipe %u not open\n",i));
        }
    }
    wDbgPrint(DBG_FUNCTION, ("AbortPipes - ends\n"));
    return STATUS_SUCCESS;
}

NTSTATUS
IrpCompletionRoutine(   IN PDEVICE_OBJECT DeviceObject,
                        IN PIRP           Irp,
                        IN PVOID          Context)
/*++

Routine Description:

    This routine is a completion routine.
    In this routine we set an event.

    Since the completion routine returns
    STATUS_MORE_PROCESSING_REQUIRED, the Irps,
    which set this routine as the completion routine,
    should be marked pending.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    Context -

Return Value:

    NT status value

--*/
{
    PKEVENT kevent = Context;
    ASSERT(kevent);
    KeSetEvent(kevent, 0, FALSE);
    wDbgPrint(DBG_VERBOSE, ("IrpCompletionRoutine: returning STATUS_MORE_PROCESSING_REQUIRED for Irp 0x%x\n",Irp));
    return STATUS_MORE_PROCESSING_REQUIRED;
}

LONG
Usb_IoIncrement(IN OUT PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    This routine bumps up the I/O count.
    This routine is typically invoked when any of the
    dispatch routines handle new irps for the driver.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    new value

--*/
{
    LONG  result = 0;
    KIRQL oldIrql;

    wDbgPrint(DBG_REFCOUNT, ("Usb_IoIncrement: entry\n"));

    //VERIFY_DEVICE_EXTENSION(DeviceExtension);

    KeAcquireSpinLock(&DeviceExtension->IOCountLock, &oldIrql);
    result = InterlockedIncrement(&DeviceExtension->OutStandingIO);

    //
    // when OutStandingIO bumps from 1 to 2, clear the StopEvent
    //

    if(result == 2)
    {
        wDbgPrint(DBG_REFCOUNT, ("Usb_IoIncrement: clearing StopEvent\n"));
        KeClearEvent(&DeviceExtension->StopEvent);
    }

    KeReleaseSpinLock(&DeviceExtension->IOCountLock, oldIrql);
    wDbgPrint(DBG_REFCOUNT, ("Usb_IoIncrement: exit, OutStandingIO = %u\n", result));
    return result;
}

LONG
Usb_IoDecrement(IN OUT PDEVICE_EXTENSION DeviceExtension)
/*++

Routine Description:

    This routine decrements the outstanding I/O count
    This is typically invoked after the dispatch routine
    has finished processing the irp.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    new value

--*/
{
    LONG  result = 0;
    KIRQL oldIrql;

    wDbgPrint(DBG_REFCOUNT, ("Usb_IoDecrement: entry\n", result));
    //VERIFY_DEVICE_EXTENSION(DeviceExtension);

    KeAcquireSpinLock(&DeviceExtension->IOCountLock, &oldIrql);
    result = InterlockedDecrement(&DeviceExtension->OutStandingIO);

    if(result == 1)
    {
        wDbgPrint(DBG_REFCOUNT, ("Usb_IoDecrement: setting StopEvent\n"));
        KeSetEvent(&DeviceExtension->StopEvent, IO_NO_INCREMENT, FALSE);
        DUMP_DEVICE_EXTENSION(DeviceExtension);
    }

    if(result == 0)
    {
        ASSERT(Removed == DeviceExtension->DeviceState);
        wDbgPrint(DBG_REFCOUNT, ("Usb_IoIncrement: setting RemoveEvent\n"));
        KeSetEvent(&DeviceExtension->RemoveEvent, IO_NO_INCREMENT, FALSE);
        DUMP_DEVICE_EXTENSION(DeviceExtension);
    }

    KeReleaseSpinLock(&DeviceExtension->IOCountLock, oldIrql);
    wDbgPrint(DBG_REFCOUNT, ("Usb_IoDecrement: exit, OutStandingIO = %u\n", result));
    return result;
}

NTSTATUS
CanStopDevice(  IN PDEVICE_OBJECT DeviceObject,
                IN PIRP           Irp)
/*++

Routine Description:

    This routine determines whether the device can be safely stopped.
Arguments:

    DeviceObject - pointer to the device object.

    Irp - pointer to the current IRP.

Return Value:

    STATUS_SUCCESS if the device can be safely stopped, an appropriate
    NT Status if not.

--*/
{
   //
   // We assume we can stop the device
   //

   UNREFERENCED_PARAMETER(DeviceObject);
   UNREFERENCED_PARAMETER(Irp);
   wDbgPrint(DBG_FUNCTION, ("CanStopDevice\n"));
   return STATUS_SUCCESS;
}

NTSTATUS
CanRemoveDevice(    IN PDEVICE_OBJECT DeviceObject,
                    IN PIRP           Irp)
/*++

Routine Description:

    This routine determines whether the device can be safely removed.


Arguments:

    DeviceObject - pointer to the device object.

    Irp - pointer to the current IRP.

Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate
    NT Status if not.

--*/
{
   //
   // We assume we can remove the device
   //

   UNREFERENCED_PARAMETER(DeviceObject);
   UNREFERENCED_PARAMETER(Irp);
   wDbgPrint(DBG_FUNCTION, ("CanRemoveDevice\n"));
   return STATUS_SUCCESS;
}

NTSTATUS
ReleaseMemory(  IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine returns all the memory allocations acquired during
    device startup.

Arguments:

    DeviceObject - pointer to the device object.


Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate
    NT Status if not.

--*/
{

    PDEVICE_EXTENSION deviceExtension;

    wDbgPrint(DBG_FUNCTION, ("ReleaseMemory: device object 0x%x\n",DeviceObject));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    VERIFY_DEVICE_EXTENSION(deviceExtension);

    if(deviceExtension->UsbConfigurationDescriptor)
    {
        ExFreePool(deviceExtension->UsbConfigurationDescriptor);
        deviceExtension->UsbConfigurationDescriptor = NULL;
    }

    if(deviceExtension->UsbDeviceDescriptor)
    {
        ExFreePool(deviceExtension->UsbDeviceDescriptor);
        deviceExtension->UsbDeviceDescriptor = NULL;
    }

    if(deviceExtension->UsbInterface)
    {
        ExFreePool(deviceExtension->UsbInterface);
        deviceExtension->UsbInterface = NULL;
    }

    if(deviceExtension->PipeContext)
    {
        ExFreePool(deviceExtension->PipeContext);
        deviceExtension->PipeContext = NULL;
    }
    DUMP_DEVICE_EXTENSION(deviceExtension);
    wDbgPrint(DBG_FUNCTION, ("ReleaseMemory - ends\n"));
    return STATUS_SUCCESS;
}

PCHAR
PnPMinorFunctionString (UCHAR MinorFunction)
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    switch (MinorFunction)
    {
        case IRP_MN_START_DEVICE:
            return "IRP_MN_START_DEVICE\n";

        case IRP_MN_QUERY_REMOVE_DEVICE:
            return "IRP_MN_QUERY_REMOVE_DEVICE\n";

        case IRP_MN_REMOVE_DEVICE:
            return "IRP_MN_REMOVE_DEVICE\n";

        case IRP_MN_CANCEL_REMOVE_DEVICE:
            return "IRP_MN_CANCEL_REMOVE_DEVICE\n";

        case IRP_MN_STOP_DEVICE:
            return "IRP_MN_STOP_DEVICE\n";

        case IRP_MN_QUERY_STOP_DEVICE:
            return "IRP_MN_QUERY_STOP_DEVICE\n";

        case IRP_MN_CANCEL_STOP_DEVICE:
            return "IRP_MN_CANCEL_STOP_DEVICE\n";

        case IRP_MN_QUERY_DEVICE_RELATIONS:
            return "IRP_MN_QUERY_DEVICE_RELATIONS\n";

        case IRP_MN_QUERY_INTERFACE:
            return "IRP_MN_QUERY_INTERFACE\n";

        case IRP_MN_QUERY_CAPABILITIES:
            return "IRP_MN_QUERY_CAPABILITIES\n";

        case IRP_MN_QUERY_RESOURCES:
            return "IRP_MN_QUERY_RESOURCES\n";

        case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
            return "IRP_MN_QUERY_RESOURCE_REQUIREMENTS\n";

        case IRP_MN_QUERY_DEVICE_TEXT:
            return "IRP_MN_QUERY_DEVICE_TEXT\n";

        case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
            return "IRP_MN_FILTER_RESOURCE_REQUIREMENTS\n";

        case IRP_MN_READ_CONFIG:
            return "IRP_MN_READ_CONFIG\n";

        case IRP_MN_WRITE_CONFIG:
            return "IRP_MN_WRITE_CONFIG\n";

        case IRP_MN_EJECT:
            return "IRP_MN_EJECT\n";

        case IRP_MN_SET_LOCK:
            return "IRP_MN_SET_LOCK\n";

        case IRP_MN_QUERY_ID:
            return "IRP_MN_QUERY_ID\n";

        case IRP_MN_QUERY_PNP_DEVICE_STATE:
            return "IRP_MN_QUERY_PNP_DEVICE_STATE\n";

        case IRP_MN_QUERY_BUS_INFORMATION:
            return "IRP_MN_QUERY_BUS_INFORMATION\n";

        case IRP_MN_DEVICE_USAGE_NOTIFICATION:
            return "IRP_MN_DEVICE_USAGE_NOTIFICATION\n";

        case IRP_MN_SURPRISE_REMOVAL:
            return "IRP_MN_SURPRISE_REMOVAL\n";

        case 0x18: //IRP_MN_QUERY_LEGACY_BUS_INFORMATION:
            return "IRP_MN_QUERY_LEGACY_BUS_INFORMATION\n";

        default:
            return "IRP_MN_?????\n";
    }
}




