        TITLE   PCI - PCI Device Manager.

;***    PCI -- PCI Device Manager.
;
;1.     Functional Description.
;       This module contains the code called during POST to scan for
;       PCI devices, copy their option ROMs to shadow memory, and then
;       pass control to them during system initialization.  This is
;       *not* an implementation of Plug-N-Play.  Instead, it is a simpler
;       non-patented mechanism to initialize an embedded PCI system.
;
;       Note that at this time the PCI code does not support configuration of
;       CardBus PCI bridges.
;
;2.     Modification History.
;       K. C. Taylor    06/01/10.       #5.3.4, new PCI support package.
;       K. C. Taylor    07/02/07.       #5.3.3, fixed CardBus support.
;
;3.     NOTICE: Copyright (C) 1997-2006 General Software, Inc.

        include ..\inc\pciapi.inc

        IF      OPTION_SUPPORT_PCI

        IF      BIOS_16
        EXTRN   PciIrqTbl:BYTE          ; This table holds the IRQ route info.
        EXTRN   PciIrqTblEnd:BYTE       ; This is the end of the table.
        ENDIF

BIOS_   SEGMENT

        EXTRN   BCPA_HiRom:WORD         ; in TABLES.ASM.

        ENDSEG

PCI_    SEGMENT
        EXTRN   PciNameTbl:PNTT         ; in PCI.ASM.

;       The following tables contain PCI header descriptions for each type
;       of PCI device.

PCIHDR_DESC     MACRO   index, typeflags, function
        dw      typeflags
        dw      index
        dw      OFFSET PCI_GRP:_p_&function
        ENDM

        PUBLIC  PciHeaderDeviceTypeTbl
PciHeaderDeviceTypeTbl label PCIHDR
        PCIHDR_DESC PCI_CONFIG_BIST, PCIHDR_FLAG_BIST, PciAssignBist
        PCIHDR_DESC PCI_CONFIG_BAR_0, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_1, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_2, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_3, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_4, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_5, PCIHDR_FLAG_BAR_32BIT, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_ROM_BAR, PCIHDR_FLAG_OPTROM, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_CACHE_LINE_SIZE, PCIHDR_FLAG_CACHE_LINE, PciAssignCacheLine
        PCIHDR_DESC PCI_CONFIG_LATENCY_TIMER, PCIHDR_FLAG_LATENCY, PciAssignLatency
        PCIHDR_DESC PCI_CONFIG_CAP_PTR, PCIHDR_FLAG_CAP_LIST, PciAssignCapList
        PCIHDR_DESC PCI_CONFIG_INT_ASSIGN, PCIHDR_FLAG_INT, PciAssignInt
        PCIHDR_DESC PCI_CONFIG_COMMAND_STATUS, PCIHDR_FLAG_COMMAND, PciAssignCommand
        dw      0                       ; end of table marker.

        PUBLIC  PciHeaderBridgeTypeTbl
PciHeaderBridgeTypeTbl label PCIHDR
        PCIHDR_DESC PCI_CONFIG_BIST, PCIHDR_FLAG_BIST, PciAssignBistBus
        PCIHDR_DESC PCI_CONFIG_BAR_0, PCIHDR_FLAG_BAR_SET, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_BAR_1, PCIHDR_FLAG_BAR_32BIT, PciAssignBar
        PCIHDR_DESC PCI_CONFIG_IO_BASE, PCIHDR_FLAG_IO, PciAssignBusIo
        PCIHDR_DESC PCI_CONFIG_MEMORY_BASE, PCIHDR_FLAG_BAR_NOT_IO, PciAssignBusRanges
        PCIHDR_DESC PCI_CONFIG_CACHE_LINE_SIZE, PCIHDR_FLAG_CACHE_LINE, PciAssignCacheLine
        PCIHDR_DESC PCI_CONFIG_LATENCY_TIMER, PCIHDR_FLAG_LATENCY, PciAssignLatency
        PCIHDR_DESC PCI_CONFIG_CAP_PTR, PCIHDR_FLAG_CAP_LIST, PciAssignCapList
        PCIHDR_DESC PCI_CONFIG_INT_ASSIGN, PCIHDR_FLAG_INT, PciAssignInt
        PCIHDR_DESC PCI_CONFIG_COMMAND_STATUS, PCIHDR_FLAG_COMMAND, PciAssignBusCommand
        dw      0                       ; end of table marker.

        TEXTSEG

;       The following GDT is used by the protected mode switch routines.

        ALIGN   8                      ; align GDT to 8 byte boundary.
gdt     DESC    <GDTSIZE-1, PCI_GRP:gdt, PCI_SEG SHR 12>
data386 DESC    <       -1, 0,             00h, 93h, 8fh>
data286 DESC    <       -1, 0, PCI_SEG SHR 12, 93h, 0>
GDTSIZE =       $-gdt

        ENDSEG

;***   PciDisableVgaRoms - Disable VGA ROMs After Primary VGA Initialization.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by PostPciScanVideo after initializing the
;       primary VGA device.  It ensures that all ROMs are disabled to avoid
;       conflicts with memory maps or drivers or other hardware limitations.
;
;   MODIFICATION HISTORY.
;       F. E. Kerr      99/09/28.       #4.3, new.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       all but SS, SP.

        IF      OPTION_SUPPORT_VIDEO_BOARDS
DefProc PciDisableVgaRoms, PUBLIC, OVERRIDE
        sub     bx, bx                  ; start at bus 0, device 0, function 0.

PciDisableVgaRoms_Loop:
        mov     di, PCI_CONFIG_FULL_CLASS_ID ; Read Class Code/ Sub Class Code.
        PciFunc READ_CONFIG_DWORD       ; (ECX) = result.
        jc      PciDisableVgaRoms_Next  ;

;       Is this device a VGA controller?  If so, make sure its ROM is disabled.

        shr     ecx, 16
        cmp     cx, 300h                ; is it a VGA?
        jne     PciDisableVgaRoms_Next  ; if not, keep looping.

;       Disable the VGA ROM if it is enabled.

        mov     di, PCI_CONFIG_ROM_BAR  ; option ROM register offset.
        PciFunc READ_CONFIG_DWORD       ; initialize option ROM register.

        and     cl, 0FEh                ; clear the ROM enable bit.

        mov     di, PCI_CONFIG_ROM_BAR  ; option ROM register offset.
        PciFunc WRITE_CONFIG_DWORD      ; initialize option ROM register.

PciDisableVgaRoms_Next:
        Gcall   PciFindNextDevice, PCI  ; (BL) = next device/function.
        jnc     PciDisableVgaRoms_Loop  ; check the next device / function.

        inc     bh
        Gcall   PciHelpLastBus, POST    ; (AL) = count of buses found so far.
        cmp     bh, al
        jbe     PciDisableVgaRoms_Loop  ; check the next bus.

EndProc PciDisableVgaRoms
        ENDIF   ; (OPTION_SUPPORT_VIDEO_BOARDS)

;***   PostPciScanVideo - POST Routine For Initialization of Video Cards.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by mainline POST to find a video card on
;       the PCI bus and initialize it as the primary video adapter.
;
;   MODIFICATION HISTORY.
;       A. Nemirovsky   98/11/16.       #4.1.5, new
;       K. C. Taylor    99/01/05.       #4.2, integrated & reformatted.
;       F. E. Kerr      99/02/06.       #4.2, conditionalize on video boards.
;       S. E. Jones     99/03/30.       #4.2, add ASSUME ES:NOTHING.
;       F. E. Kerr      99/07/23.       #4.21, code review.
;       D. W. Tarr      99/08/03.       #4.21, use highest # VGA device on bus 0.
;       F. E. Kerr      99/10/26.       #4.3, use BoardPciControl policy hook.
;       K. C. Taylor    99/12/14.       #4.3, allow BoardPciControl to use lower.
;       K. C. Taylor    00/01/04.       #4.3, turn off VGA pallette snooping.
;       B. J. Brashier  00/05/17.       #4.4, added BoardPciControl hook.
;       M. B. Hammer    04/12/08.       #5.3, removed PCIBI_Bitmap reference.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       all but SS, SP.

        IF      OPTION_SUPPORT_VIDEO_BOARDS
DefProc PostPciScanVideo, GLOBAL, OVERRIDE
        PUSHALLD
        sub     dl, dl                  ; (DL) = # pages for later code, below.

;       If a VGA device was found (and allowed by BoardPciControl) during bus
;       enumeration, its location was stored in PciVgaLocation.  Bus enumeration
;       does not check bus 0, so if we haven't found a PCI VGA device yet, we
;       must check for VGA on a downstream bus and if not present, search for
;       the highest number VGA device on bus 0. Note that BoardPciControl may
;       modify the policy for assigning PciVgaLocation in bus enumeration, and
;       it gets a final chance to approve or reject (replace) the current value
;       in PciVgaLocation at PostPciScanVideo_EnableVga.

        USEEXTDATA ds
        mov     bx, PciVgaLocation
        cmp     bx, 0ffffh              ; Check for VGA on Downstream Bus.
        jne     PostPciScanVideo_EnableVga

;       Search for a VGA on bus 0.  Use the last VGA device on the bus to allow
;       drop in cards to override built in VGA controllers.

        sub     bx, bx                  ; (BX) = 0 (select bus 0, device 0, func 0).
        mov     di, PCI_CONFIG_VENDOR_DEVICE_ID
        PciFunc READ_CONFIG_DWORD       ; read vendor ID/device ID.
        inc     ecx                     ; if (ECX) == 0ffffffffh, set ECX to 0.
        jnz     PostPciScanVideo_NoIncDev ; if valid, don't increment device #.

;       VGA scan loop for bus 0.  Scan for VGA devices, updating PciVgaLocation
;       whenever one is found.  When all devices on bus 0 have been checked,
;       use the highest numbered VGA device.

PostPciScanVideo_Bus0Loop:
        Gcall   PciFindNextDevice, PCI  ; (BL) = next device/function.
        jc      PostPciScanVideo_EnableVga ; if no more.

PostPciScanVideo_NoIncDev:              ; skip device/function increment.
        mov     di, PCI_CONFIG_FULL_CLASS_ID ; read Class Code/ Sub Class Code.
        PciFunc READ_CONFIG_DWORD       ; (ECX) = result.

;       Is this device a VGA controller?  If so, update PciVgaLocation.

        shr     ecx, 16
        cmp     cx, 300h                ; is it a VGA?
        jne     PostPciScanVideo_Bus0Loop ; if not, keep looping.

;       We have found a PCI VGA device on PCI bus 0.  In the default case, we
;       are searching for the PCI VGA that is the largest device number.  Report
;       the new PCI VGA device that we have found to BoardPciControl, and assign
;       it to PciVgaLocation if BoardPciControl allows it by not changing the
;       value passed to it in (AX).

        mov     ax, PCI_CTL_VGA_SCAN0
        shl     eax, 16
        mov     ax, bx
        Gcall   BoardPciControl, POST   ; default action returns (AX) unchanged.
        USEEXTDATA ds
        cmp     ax, 0ffffh              ; Should we skip this update?
        je      PostPciScanVideo_Bus0Loop ; Don't update if it did.
        mov     PciVgaLocation, ax      ; update last VGA device found.
        jmp     PostPciScanVideo_Bus0Loop

;       If there is a VGA card, enable it.  This is where BoardPciControl can
;       override policy by returning a desired VGA location that was previously
;       passed to it by BoardPciControl with PCI_CTL_VGA_SCAN0 (for the bus 0
;       scan) or PCI_CTL_VGA_SCAN (for the non-bus 0 scan, from largest PCI bus
;       number found to PCI bus #1).

PostPciScanVideo_EnableVga:
        mov     ax, PCI_CTL_GET_VGA_LOC
        shl     eax, 16
        USEEXTDATA ds
        mov     ax, PciVgaLocation
        Gcall   BoardPciControl, POST   ; default action returns (AX) unchanged.
        mov     bx, ax
        cmp     bx, 0ffffh              ; did we find any VGA devices?
        je      PostPciScanVideo_Exit   ; if not.

;       Check if we found a VGA device on a downstream bus.  This will only
;       happen if BoardPciControl has modified the default behavior.

        or      bh, bh                  ; (BH) = bus number of selected PCI VGA.
        jnz     PostPciScanVideo_EnableDownstream

;       We found a VGA card on bus 0, device/function is in BL.

        push    bx                      ; save VGA location.
        mov     di, PCI_CONFIG_COMMAND_STATUS
        PciFunc READ_CONFIG_DWORD

        or      ecx, 0ffff0007h         ; clear status. Mem, I/O enable.
        PciFunc WRITE_CONFIG_DWORD
        pop     bx                      ; restore VGA location.
        jmp     PostPciScanVideo_FoundDevice     ; Continue.

PostPciScanVideo_EnableDownstream:
        push    bx                      ; save VGA location
        mov     di, PCI_CONFIG_COMMAND_STATUS
        PciFunc READ_CONFIG_DWORD

        or      ecx, 0ffff0007h         ; clear status. Mem, I/O enable.
        PciFunc WRITE_CONFIG_DWORD

;       Starting at the Bus that the VGA is on, move upstream and set
;       VGA ENABLE in every PCI/PCI bridge on the way up to Bus 0.

PostPciScanVideo_NextBridgeEnable:
        Pcall   PciFindMapEntry         ; (ES:DI) = ptr, the entry.
        ASSUME  ES:NOTHING
        mov     bx, WORD PTR es:PCIBI_BridgeLocation.[di]

;       Now we have (BX) = PCI Bus/Dev/Function of upstream Bridge.

        mov     ax, PCI_CTL_VGA_BRIDGE  ; allow board/chipset to intervene.
        shl     eax, 16
        clc                             ; default does not change carry.
        Gcall   BoardPciControl, POST
        jc      PostPciScanVideo_NextBridge ; if control was taken, do not set registers.

        mov     di, PCI_CONFIG_BRIDGE_CONTROL ; bridge Control Reg.
        PciFunc READ_CONFIG_BYTE

        or      cl, 08h                 ; set VGA enable bit.
        PciFunc WRITE_CONFIG_BYTE

;       Enable VGA Memory and I/O cycles to pass through bridge.

        mov     di, PCI_CONFIG_COMMAND_STATUS ; command and status.
        PciFunc READ_CONFIG_DWORD

        or      ecx, 0ffff0007h         ; Clear status. Memory, master enabled.
        PciFunc WRITE_CONFIG_DWORD

PostPciScanVideo_NextBridge:
        cmp     bh, 0
        jne     PostPciScanVideo_NextBridgeEnable

        pop     bx                      ; Restore VGA location

PostPciScanVideo_FoundDevice:
        Gcall   PciDeviceMapRom, PCI    ; map ROM for device in (BX).

PostPciScanVideo_Exit:
        Pcall   PciDisableVgaRoms       ; disable all other VGA ROMs.

        mov     ax, PCI_CTL_SCAN_VIDEO_DONE
        shl     eax, 16
        Gcall   BoardPciControl, POST   ; indicate that PCI VGA scan is done.
        POPALLD
EndProc PostPciScanVideo
        ENDIF   ; (OPTION_SUPPORT_VIDEO_BOARDS)

;***   PciCopyTempIrqTbl - Copy the PciIrqTbl into temporary RAM copy.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called to copy the PciIrqTbl into a temporary RAM
;       copy in a scratch buffer.  It is called at the beginning of
;       PCI enumeration and might be called by a board module if the shadowed
;       table has been modified because of PCI bridging.
;
;   MODIFICATION HISTORY.
;       F. E. Kerr      00/02/10.       #4.3, new.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       BX      -       allowed IRQ bitmap for PCI bus.
;
;   USES.
;       all.

DefProc PciCopyTempIrqTbl, PUBLIC, OVERRIDE

;       Get the available PCI IRQs for assignment, and store them in the
;       POST scratch buffer for later use during device initialization.

;       Initialize the temporary buffer with a  header, used by the
;       PCIAPI call to GET_IRQ_ROUTING_OPTIONS through INT 1ah.
;       This buffer is used to hold the bus map and resources required
;       by each bus and device during PCI initialization.  This
;       segment is a temporary  scratch memory region and the data it
;       holds will be destroyed later in POST, after PCI enumeration and
;       initialization.

        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax                  ; (ES) = table data segment.
        ASSUME  ES: NOTHING

        xor     di, di                  ; (DI) = table offset.
        mov     word ptr es:[di], 07ff0h; store size of table.
        mov     word ptr es:[di+2], 6   ; store beginning of data.
        mov     word ptr es:[di+4], ax  ; store table segment.

        mov     ax, POST_SEG            ; (AX) = Post Segment.
        mov     ds, ax                  ; get PCI IRQ assumes DS is Post seg.
        ASSUME  DS:POST_GRP

        sub     bx, bx                  ; per the spec (unused).

;       Copy the PciIrqTbl into a memory buffer at buffer:6
;       (after the buffer header of 6 bytes defined above).

        PciFunc GET_IRQ_ROUTING_OPTIONS ; call PCI BIOS, setting CY and BX.
EndProc PciCopyTempIrqTbl

;***   PostPciEnumerate - POST Routine For Enumerating PCI space.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by mainline POST to enable and enumerate PCI
;       space. This involves the following steps:
;
;       1. Configure the System PCI IRQ to ISA IRQ bridge to map the four
;          PCI INT to ISA IRQs.
;
;       2. Find all the PCI-PCI bridges in the systems to create a "PCI bus
;          map" and enable PCI config cycles to pass through the bridges.
;
;       3. Determine and assign all of the PCI devices resources requirements
;          to the PCI devices and PCI-PCI bridges.
;
;   MODIFICATION HISTORY.
;       A. Nemirovsky   98/10/20.       #4.1.5, New code.
;       K. C. Taylor    99/02/23.       #4.2, Changed how PciBusTotal works.
;       F. E. Kerr      99/02/25.       #4.2, further integration.
;       S. E. Jones     99/03/30.       #4.2, removed PciBusTotal.
;       K. C. Taylor    99/03/31.       #4.2, fixed support for bridge trees.
;       F. E. Kerr      99/04/13.       #4.21, add ASSUME ES:NOTHING.
;       F. E. Kerr      99/07/23.       #4.21, code review.
;       F. E. Kerr      99/11/16.       #4.3, add BoardPciControl policy hook.
;       F. E. Kerr      00/02/10.       #4.3, add more hooks (BRIDGES_FOUND).
;       B. J. Brashier  00/05/17.       #4.4, add more hooks (FIND_BRIDGE, etc).
;       K. C. Taylor    00/09/13.       #5.0, updated INT line support.
;       S. E. Jones     01/06/06.       #5.0, don't use IRQs already assigned by PnP.
;       S. E. Jones     01/08/14.       #5.0, use POST scratch buffer.
;       B. J. Brashier  04/07/21.       #5.3, added call to PciDumpHeaders.
;       K. C. Taylor    06/01/10.       #5.3.4, new allocation support.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       flags.

DefProc PostPciEnumerate, GLOBAL, OVERRIDE
        POSTCODE POST_STATUS_PCIENUM
        PUSHALL

;       Get the available PCI IRQs for assignment, and store them in the
;       POST scratch buffer for later use during device initialization.
;       The second half of the buffer is used to hold the bus map and resources
;       required by each bus and device during PCI initialization.  This
;       segment is a temporary  scratch memory region and the data it
;       holds will be destroyed later in POST, after PCI enumeration and
;       initialization.

        Pcall   PciCopyTempIrqTbl
        jc      PostPciEnumerate_Exit   ; if IRQ lookup failed; PCI fails.

        USEEXTDATA ds

;       If PnP ISA is running, it has stored the IRQs it has already allocated
;       in the EBDA field, PnPIrqsUsed, where a 1 in this field indicates that
;       the IRQ is assigned to PnP and cannot be assigned to PCI.  From our bitmask
;       in (BX) from PciCopyTempIrqTbl, mask-off these bits now.

        IF      ((OPTION_SUPPORT_PNP NE 0) AND (OPTION_PNP_ISA NE 0))

        mov     ax, PnpIrqsUsed         ; (AX) = IRQs *used* by PnP.
        not     ax                      ; (AX) = IRQs *not* used by PnP.
        and     bx, ax                  ; (BX) = IRQs remaining for PCI allocation.

        ENDIF   ; ((OPTION_SUPPORT_PNP NE 0) AND (OPTION_PNP_ISA NE 0))

;       (BX) = allowed IRQ bitmap for PCI bus.

        mov     PciIrqs, bx             ; store the IRQ map.
        mov     PciIrqsBackup, bx       ; store backup, in case we run out.

;       Clear the INT to IRQ assignment table prior to use.

        mov     cx, CONFIG_PCI_INT_LINES ; clear all IRQs.
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax
        ASSUME  ES:NOTHING
        mov     di, CONFIG_PCI_BM_OFFSET
        mov     al, 0ffh                ; IRQ not yet assigned.
        rep     stosb                   ; clear all asignments.

;       We now have an initial assignment of System IRQs to PCI INT routing in
;       the temporary table segment at the beginning of the bus information
;       table set.  Report this state to the board module, so the board module
;       has a chance to alter the IRQ pre-assignments.

        mov     ax, PCI_CTL_INIT_PCI_IRQ
        shl     eax, 16
        mov     di, CONFIG_PCI_BM_OFFSET
        mov     edx, dword ptr es:[di]  ; for backwards compatibility.
        Gcall   BoardPciControl, POST   ; report the initial assignment.
        mov     dword ptr es:[di], edx  ; for backwards compatibility.

;       Find All PCI-PCI Bridges, create PCI bus map and enable PCI space.
;       (BH) = bus #, (BL) = device/function.

        sub     bx, bx                  ; start at Bus 0, device 0, function 0.

;       Zero out PCI Bus Map entry for PCI Bus 0 (BH).

        Pcall   PciClearBusMapEntry

;       Call PciFindBridge to recursively find all PCI-PCI bridges
;       and build the PCI Bus map.

        mov     al, bh                  ; (AL) = bus count (bus zero).
        Gcall   PciHelpWriteLastBus, POST ; start with 0 as highest known bus #.

        mov     ax, PCI_CTL_FIND_BRIDGE ; tell BoardPciControl which function to perform.
        shl     eax, 16                 ; the entry code must be in top half of eax.
        Gcall   BoardPciControl, POST   ; allow board/chipset bus enumeration.
        cmp     al, 1                   ; if board or chipset enumerated, (AL) = 1.
        je      PostPciEnumerate_BridgesFound   ; if so, don't enumerate again.

        Pcall   PciFindBridge           ; enumerate bridges if board/chipset did not.

;       Inform the board module that bridges have been enumerated and mapped.
;       PciIrqTbl and its temporary copy may need to be updated if bridges
;       on add-in cards cause devices to move from default locations.

PostPciEnumerate_BridgesFound:
        PUSHALLD
        mov     ax, PCI_CTL_BRIDGES_FOUND
        shl     eax, 16
        Gcall   BoardPciControl, POST

        mov     ax, SHADOW_CTL_WRITABLE ; (AX) = write-en all shadow regions.
        Gcall   BoardShadowWriteCtl, POST ; enable writes to shadow RAM.

        Pcall   PciUpdateBridgeMap      ; update the Bridge Map table.
        Pcall   PciUpdatePciIrqTbl      ; update the PCI IRQ Routing table.

        sub     ax, ax                  ; (AX) = write-protect function.
        Gcall   BoardShadowWriteCtl, POST ; disable writes to shadow RAM.

        Pcall   PciCopyTempIrqTbl       ; update the temporary copy.
        POPALLD

        Pcall   PciBridgeIntRoute       ; determine default barber polling.
        Pcall   PciVgaBackScan          ; scan busses in reverse for VGA.

        PUSHALLD
        Pcall   PciAssignAllocInit      ; initialize bus 0 allocation area.
        sub     bh, bh                  ; start with bus 0.
        IF      OPTION_PCI_HUGE_ASSIGNMENTS
        mov     si, (PCIHDR_FLAG_BAR_SET OR PCIHDR_FLAG_BIST OR PCIHDR_FLAG_OPTROM)
        ELSE    ; (OPTION_PCI_HUGE_ASSIGNMENTS)
        mov     si, (PCIHDR_FLAG_BAR_SET OR PCIHDR_FLAG_MEM_COMBINE_64_32 OR \
                     PCIHDR_FLAG_BIST OR PCIHDR_FLAG_OPTROM)
        ENDIF   ; (OPTION_PCI_HUGE_ASSIGNMENTS)
        Pcall   PciAssignResources      ; assign resources to bus and device.
        sub     bh, bh                  ; start with bus 0 again.
        mov     si, (PCIHDR_FLAG_MISC AND (NOT PCIHDR_FLAG_BIST)) ; remaining registers require extra pass.
        Pcall   PciAssignResources      ; assign resources to bus and device.
        POPALLD

;       Assign all IRQs to devices that require them.  Now that the buses
;       have been enabled, we can assign the IRQs in the chipset and in the
;       PCI buses.

        Pcall   PciConfigIrqs
        clc

PostPciEnumerate_Exit:
        pushf
        IF      ((OPTION_DIAGNOSE_PCI NE 0) AND (OPTION_DEBUGAID_CPRINTF NE 0))
        Pcall   PciDumpHeaders
        ENDIF   ; ((OPTION_DIAGNOSE_PCI NE 0) AND (OPTION_DEBUGAID_CPRINTF NE 0))

        mov     ax, PCI_CTL_ENUM_DONE
        shl     eax, 16
        Gcall   BoardPciControl, POST   ; indicate that PCI enumeration is done.
        popf
        POPALL
EndProc PostPciEnumerate

;***   PciFindBridge - POST Routine to find a PCI-PCI bridge.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called from PostPciEnumerate and recursively calls
;       itself to find all PCI-PCI bridges.  It also places entries into the
;       "PCI Bus Map" to give bridge location in PCI space.  This table is then
;       used to configure the bridge hierarchy to allow all PCI device config
;       space to be accessible.
;
;   MODIFICATION HISTORY.
;       A. Nemirovsky   98/10/20.       #4.1.5, New code
;       K. C. Taylor    99/01/13.       #4.2, revised and integrated.
;       F. E. Kerr      99/02/25.       #4.2, further integration.
;       S. E. Jones     99/03/30.       #4.2, removed PciBusTotal.
;       S. E. Jones     99/03/30.       #4.2, add ASSUME ES:NOTHING.
;       K. C. Taylor    99/03/31.       #4.2, fixed support for bridge trees.
;       F. E. Kerr      99/10/07.       #4.3, set latency timers.
;       K. C. Taylor    00/02/08.       #4.3, add basic cardbus support.
;       E. Khoruzhenko  00/02/24.       #4.3, removed basic cardbus support.
;       B. J. Brashier  00/05/18.       #4.4, added PCI_CTL_IS_BRIDGE hook.
;       B. J. Brashier  00/05/29.       #4.3, changed barber-pole routine.
;       K. C. Taylor    00/09/13.       #5.0, updated barber-pole routine.
;       M. B. Hammer    04/12/08.       #5.3, removed PCIBI_Bitmap reference.
;       K. C. Taylor    06/01/12.       #5.3.4, new allocation support.
;
;   WARNINGS.
;       This routine is called recursively.
;
;   ENTRY.
;       BH      - Bus Number.
;       BL      - [7:4] Device number.
;               - [3:0] Function #.
;
;   EXIT.
;       none.
;
;   USES.
;       All.

DefProc PciFindBridge, GLOBAL, OVERRIDE

;       Check to see if this device # is valid on this bus by attempting to read
;       the device/vendor ID.  If the read fails or returns an invalid value
;       then this device is not valid.

PciFindBridge_DevLoop:
        mov     di, PCI_CONFIG_VENDOR_ID
        PciFunc READ_CONFIG_WORD
        jc      PciFindBridge_Exit      ; exit if it fails.

        cmp     cx, 0ffffh              ; check to see if device exists.
        je      PciFindBridge_NextFnc

;       Found a valid PCI device.  Test to see if it is a bridge.

        mov     di, PCI_CONFIG_HEADER_TYPE
        PciFunc READ_CONFIG_BYTE        ; call PCI services.
        jc      PciFindBridge_Exit      ; if failure, skip the initialization.
        and     cl, PCI_CONFIG_HEADER_TYPE_MASK
        cmp     cl, PCI_CONFIG_HEADER_TYPE_CARDBUS
        je      PciFindBridge_Found
        cmp     cl, PCI_CONFIG_HEADER_TYPE_BRIDGE
        je      PciFindBridge_Found     ; found a bridge.

;       Now allow chipset or board intervention to be sure that this is not a bridge.

        mov     ax, PCI_CTL_IS_BRIDGE   ; ask BoardPciControl if this is a bridge.
        shl     eax, 16                 ; PCI_CTL goes in top half EAX.
        mov     al, PCI_CTL_NO_BRIDGE_FOUND ; assume no bridge will be found.
        Gcall   BoardPciControl, POST   ; allow board or chipset intervention.

        cmp     al, PCI_CTL_NO_BRIDGE_FOUND ; is this device actually a bridge?
        je      PciFindBridge_NextFnc   ; skip if not a bridge.

;       We have found a PCI-PCI bridge.  Clear Bridge Status bits, turn off
;       mem, i/o, and master enable, and initialize bridge registers.

PciFindBridge_Found:

;       Run built in self test on bridge and skip if it fails.

        mov     di, PCI_CONFIG_BIST     ; built in self test register.
        Pcall   PciAssignBist           ; run built in self test.
        jc      PciFindBridge_NextFnc   ; skip to next function on failure.

;       Disable the bridge by clearing the command register.

        mov     di, PCI_CONFIG_COMMAND  ; index command and status registers.
        mov     ecx, 0ffff0000h         ; clear command, set all status bits.
        PciFunc WRITE_CONFIG_DWORD      ; call PCI services.

;       Set the latency timers for this bridge.

        mov     di, PCI_CONFIG_LATENCY_TIMER
        mov     cl, CONFIG_PCI_LATENCY
        PciFunc WRITE_CONFIG_BYTE       ; initialize latency timer register.

        mov     di, PCI_CONFIG_SECONDARY_LATENCY
        mov     cl, CONFIG_PCI_LATENCY
        PciFunc WRITE_CONFIG_BYTE       ; initialize latency timer register.

;       Store parent bus # for this bridge in the primary bus # register of this
;       bridge.

        mov     di, PCI_CONFIG_BUS_PRIMARY ; index 18h = Primary Bus No.
        mov     cl, bh                  ; (CL) = the primary Bus (we're scanning).
        PciFunc WRITE_CONFIG_BYTE       ; call PCI services.
        jc      PciFindBridge_Exit      ; if failure skip init.

;       Increment the number of buses.  This is the Secondary bus number.

        Gcall   PciHelpLastBus, POST    ; (AL) = count of buses found so far.
        inc     al                      ; (AL) = incremented count.
        Gcall   PciHelpWriteLastBus, POST
        mov     cl, al                  ; (CL) = Secondary bus number.

;       Store the secondary bus # for this bridge in the secondary bus #
;       register of this bridge.

        mov     di, PCI_CONFIG_BUS_SECONDARY
        PciFunc WRITE_CONFIG_BYTE       ; call PCI services.
        jc      PciFindBridge_Exit      ; if failure skip init.

;       Configure the subordinate's bus number.

        mov     di, PCI_CONFIG_BUS_SUBORDINATE
        PciFunc WRITE_CONFIG_BYTE       ; call PCI services.
        jc      PciFindBridge_Exit      ; if failure skip init.

;       If we are scanning the primary PCI bus, then we don't need to
;       configure an upstream bus, because there are none by definition.

        cmp     bh, 0                   ; are we scanning the primary bus?
        je      PciFindBridge_WriteEntry; if so, there is no "upstream".

;       Starting at the current bus (BH), walk up the bridges all the way up
;       to bus 0, assigning "subordinate numbers" to each secondary bus,
;       and then their parent, and so on.

        push    bx                      ; save bus, device, function number.
PciFindBridge_UpstreamLoop:
        cmp     bh, 0                   ; are we done scanning upstream?
        je      PciFindBridge_UpstreamDone ; if so.

;       Find the structure in our table for bus (BH) that represents
;       its device node on the upstream bus.

        Pcall   PciFindMapEntry         ; (ES:DI) = ptr to the PCIBI entry.
        ASSUME  ES:NOTHING
        mov     bx, es:PCIBI_BridgeLocation.[di]

;       Set up the subordinate bus for our parent bus.

        mov     di, PCI_CONFIG_BUS_SUBORDINATE
        PciFunc WRITE_CONFIG_BYTE       ; call PCI services.
        jmp     PciFindBridge_UpstreamLoop

PciFindBridge_UpstreamDone:
        pop     bx                      ; restore bus no/dev no.

;       Prepare to scan the new secondary bus that we have found, and then
;       call this routine recursively to find any buses that it may declare.

PciFindBridge_WriteEntry:

;       Clear out the information for the subordinate bus that we are about
;       to scan.  Note that (CL) contains the value for the found bus.

        push    bx                      ; preserve the current bus/dev no.
        mov     bh, cl                  ; set the bus to the subordinate.
        Pcall   PciClearBusMapEntry

;       Save Current Bus/Dev # of this bridge (upstream side) into PCI Bus Map.

        Pcall   PciFindMapEntry         ; (ES:DI) = ptr, the entry.
        ASSUME  ES:NOTHING
        pop     bx                      ; restore the current bus/dev no.

;       Write the current bus/dev we are scanning into the information structure
;       for the next subordinate bus.  Note that (ES:DI) points to the
;       information for the downstream bus, not the bus we're scanning.

        mov     es:PCIBI_BridgeLocation.[di], bx

;       Save Primary Bus No before recursing.  Then, make the primary bus
;       number and next bus number equal to the next bus to search.  Please
;       note that (BH) is still the max # buses from the above call to
;       PciHelpLastBus.

        push    bx                      ; Preserve current bus number.
        mov     bh, cl                  ; (BH) = The next subordinate bus.
        sub     bl, bl                  ; Start scan at device 0.
        Pcall   PciFindBridge
        pop     bx                      ; Restore current bus/device number.

PciFindBridge_NextFnc:
        Gcall   PciFindNext, PCI        ; find next PCI device/function on bus.
        jnc     PciFindBridge_DevLoop

PciFindBridge_Exit:
EndProc  PciFindBridge

;***   PciUpdatePciIrqTbl - Update the PCI IRQ Table.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine updates the PCI IRQ Table to account for any PCI-to-PCI
;       Bridge cards that may have been plugged into PCI slots. In case if the
;       bridge specified in the PciBridgeTbl table cannot be found invalidate
;       entries for all devices behind that bridge.
;
;   MODIFICATION HISTORY.
;       B. J. Brashier  03/02/26.       #5.2, new.
;       K. C. Taylor    03/04/22.       #5.2, added "exit if no table".
;       E. Khoruzhenko  05/03/14.       #5.3, handle the "bridge disappeared" case.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;       K. C. Taylor    06/02/14.       #5.3.4, bus 0 (host) has no PCI-to-PCI bridge.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       All.

        IF      ORN_p_PciUpdatePciIrqTbl
DefProc PciUpdatePciIrqTbl, PUBLIC, OVERRIDE
        Gcall   BoardGetBridgeMap, POST ; (DS:SI) = ptr to PCI Bridge Map table.
        jc      PciUpdatePciIrqTbl_Exit
        mov     ax, POST_SEG
        mov     es, ax
        ASSUME  ES:NOTHING
        mov     bp, offset POST_GRP:PciIrqTbl ; (ES:BP) = ptr to PCI IRQ table.
        mov     dx, offset POST_GRP:PciIrqTblEnd ; (DX) = ptr to end of table.
        mov     di, PCI_CONFIG_BUS_SECONDARY ; read secondary bus number.

;       Process each entry in the PCI IRQ routing table correcting the
;       default bus number to the one read from the PCI bridge that hosts the
;       device corresponding to this entry.

PciUpdatePciIrqTbl_Loop:
        mov     al, es:[bp]             ; (AL) = default bus number.
        test    al, al                  ; check for host bridge reference.
        jz      PciUpdatePciIrqTbl_Cont ; never change entries on host bus.
        movzx   bx, al
        add     bl, al
        add     bl, al                  ; (BX) = ptr to entry for this bus.
        add     bx, si                  ; calculate entry index.
        cmp     bx, dx                  ; verify against end of table.
        jae     PciUpdatePciIrqTbl_Cont ; skip if out of range.
        mov     bx, ds:[bx+1]           ; (BX) = dev/func, bus for this bridge.
        PciFunc READ_CONFIG_BYTE        ; (CL) = bus number this bridge creates.

;       Check if the target bridge was disabled as it happens with the AGP
;       bridges. If we read 0ffh from it then invalidate the whole entry.

        cmp     cl, 0ffh
        je      PciUpdatePciIrqTbl_NotFound
        mov     es:[bp], cl             ; store as the corrected bus number.
        jmp     PciUpdatePciIrqTbl_Cont

PciUpdatePciIrqTbl_NotFound:

;       Clear 16 bytes of the PCI IRQ table entry.

        sub     eax, eax
        mov     es:[bp], eax
        mov     es:[bp+4], eax
        mov     es:[bp+8], eax
        mov     es:[bp+12], eax

PciUpdatePciIrqTbl_Cont:
        add     bp, 16                  ; go to next PCI IRQ entry.
        cmp     bp, dx                  ; have we corrected all entries?
        jb      PciUpdatePciIrqTbl_Loop ; if not, keep correcting.

PciUpdatePciIrqTbl_Exit:
EndProc PciUpdatePciIrqTbl
        ENDIF   ; (ORN_p_PciUpdatePciIrqTbl)

;***   PciBridgeIntRoute - Find Int Routes for Bridges.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is intended to find the default barber-pole setup for
;       each PCI BUS in the system.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    00/09/22.       #5.0, updated barber-pole routine.
;       S. E. Jones     01/08/14.       #5.0, use POST scratch buffer.
;       K. C. Taylor    02/12/18.       #5.2, fixed irq routing for deep bridges.
;       K. C. Taylor    06/02/14.       #5.3.4, force invalid default for bus 0.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       All.

DefProc PciBridgeIntRoute, PUBLIC, OVERRIDE
        mov     bx, 0                   ; start at bus 0, device 0.
        Pcall   PciFindMapEntry         ; (ES:DI) = ptr to bus map info.
        ASSUME  ES:NOTHING
        sub     edx, edx                ; clear barber polling.
        dec     edx                     ; use definitely invalid routing.
        push    bx                      ; keep stack balanced.
        jmp     PciBridgeIntRoute_FindIrqDone ; done with bus 0.

;       Top of bridge initialization loop.

PciBridgeIntRoute_Loop:

;       Start by finding the next bus to configure.

        inc     bh
        Gcall   PciHelpLastBus, POST    ; (AL) = count of buses found so far.
        cmp     bh, al                  ; Have we checked the last bus?
        jbe     PciBridgeIntRoute_Setup ; Set up this bus.

        ret                             ; All done.

;       Standard IRQ routing is done in a barber-pole fashion.  For example,
;       INT line A for device 0 will be attached to INT line A of the chipset,
;       but INT line A for device 1 will be attached to INT line B of the
;       chipset and for device 2 INT line A will be attached to INT line C.
;
;       The routine below retrieves the INT lines assigned to each INT pin
;       of the device the bridge is plugged into and stores it in a table.

PciBridgeIntRoute_Setup:
        Pcall   PciFindMapEntry         ; (ES:DI) = ptr to bus map info.

        push    bx                      ; save bx.
        mov     bx, es:PCIBI_BridgeLocation.[di] ; Get the current bridge dev.
        and     bl, 0f8h                ; look only at bus and device number.
        sub     edx, edx                ; blank default barber polling.
        sub     si, si                  ; PCI IRQ table header at offset 0.

        mov     di, word ptr es:[si+2]  ; (DI) = temporary PciIrqTbl offset.
        mov     ax, word ptr es:[si+4]  ; (AX) = PciIrqTbl segment.
        mov     si, word ptr es:[si]    ; (SI) = PciIrqTbl length.
        mov     es, ax                  ; set segment for PciIrqTbl.
        ASSUME  ES:NOTHING
        add     si, di                  ; (SI) = last entry in table.

;       Check to see if there is a PciIrqTbl entry for this device.

PciBridgeIntRoute_FindIrqTbl:
        mov     ax, es:[di]
        xchg    ah, al                  ; (AX) = bus/dev of device in table.
        cmp     ax, bx                  ; is this the entry we're looking for?
        je      PciBridgeIntRoute_FindIrqTblEntry ; if so, process this entry.
        add     di, 16                  ; otherwise, move to next entry.
        cmp     di, si                  ; was that the last entry?
        jb      PciBridgeIntRoute_FindIrqTbl ; if not, keep searching.

        cmp     bh, 00h                 ; is this bus 0?
        jne     PciBridgeIntRoute_FindIrqNotBus0 ; if not, read array.

;       We've determined that we are on bus 0, so the array will not have
;       valid data.  Also, we know that a PciIrqTbl entry does not exist
;       for this device.  We then must use the standard barber-pole method
;       to create an IRQ list.

        sub     edx, edx                ; zero EDX to make it usable.
        mov     cl, 4                   ; set 4 iterations in this loop.
        shr     bl, 3                   ; (BL) = device #.
PciBridgeIntRoute_FindIrqBus0:
        and     bl, 3h                  ; make sure INT line is between 0 & 3.
        or      dl, bl                  ; set this INT for use.
        inc     bl                      ; (BL) = next INT.
        ror     edx, 8                  ; roll EDX to accept next INT line.
        loop    PciBridgeIntRoute_FindIrqBus0
        jmp     PciBridgeIntRoute_FindIrqDone ; we're done, put EDX in array.

;       We did not find a PciIrqTbl entry and this isn't bus 0 so we are
;       free to use the value previously stored in the array for the parent
;       bus as our starting point for finding the barber polling for this bus.

PciBridgeIntRoute_FindIrqNotBus0:
        movzx   si, bh                  ; (SI) = beginning of offset for parent bus.
        shl     si, 2                   ; (SI) = offset of parent bus.
        add     si, CONFIG_PCI_BIRQ_OFFSET ; (SI) = pointer to IRQ array.
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax                  ; (ES) = segment of stored array.
        ASSUME  ES:NOTHING
        mov     edx, DWORD PTR es:[si]  ; (EDX) = INT lines for this dev/bus.
        mov     cl, bl                  ; (CL) = device # of current bridge.
        and     cl, 18h                 ; (CL) = INT barb-pole shift amount.
        ror     edx, cl                 ; (EDX) = INT lines for bridge.
        jmp     PciBridgeIntRoute_FindIrqDone ; we're done, put EDX in array.

;       We found an entry for this device.  Use it to find the INT's.

PciBridgeIntRoute_FindIrqTblEntry:
        add     di, 2                   ; (DI) = first INT for this entry.
        sub     ch, ch                  ; (CH) = loop counter.
PciBridgeIntRoute_FindIrqTblEntryLoop:
        mov     cl, BYTE PTR es:[di]    ; (CL) = avail INT for this slot.
        dec     cl                      ; put CL in 0-N format.
        or      dl, cl                  ; store this INT in EDX.
        ror     edx, 8                  ; roll EDX to accept next INT.
        add     di, 3                   ; (CH) = ptr to next INT in entry.
        inc     ch                      ; increment loop counter.
        cmp     ch, 4                   ; have we input all 4 INT's?
        jb      PciBridgeIntRoute_FindIrqTblEntryLoop ; if not, loop.

;       Now we're done.  Save EDX into the array so that devices after this
;       bridge can use it to find their available INT's.

PciBridgeIntRoute_FindIrqDone:
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax                  ; (ES) = segment of stored array.
        ASSUME  es:NOTHING
        pop     bx                      ; restore bus number of current bridge.
        movzx   si, bh                  ; (SI) = index of bus number in array.
        shl     si, 2                   ; (SI) = byte offset of routing in array.
        add     si, CONFIG_PCI_BIRQ_OFFSET ; (SI) = pointer to IRQ array.
        mov     DWORD PTR es:[si], edx  ; store EDX in the array.
        jmp     PciBridgeIntRoute_Loop  ; keep going.
EndProc PciBridgeIntRoute

;***   PciVgaBackScan - Scan Busses Backwards for VGA parts.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine scans PCI busses backwards for VGA parts, to determine
;       which VGA part will be the primary part.  This must execute prior to
;       the code for assigning resources, because the resource assignment
;       code is also part of this process if there are no VGA devices on
;       higher numberred busses.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    05/04/28.       #5.3, new.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       flags.

DefProc PciVgaBackScan, GLOBAL, OVERRIDE
        PUSHALLD
        USEEXTDATA ds

        cmp     PciVgaLocation, 0ffffh  ; have we already found a VGA device?
        jne     PciVgaBackScan_Exit     ; if VGA previously found, we are done.

;       Start at the highest numbered bus and scan backwards to bus 0.

        push    ax
        Gcall   PciHelpLastBus, POST    ; (AL) = number of buses.
        mov     bh, al                  ; (BH) = number of buses.
        pop     ax

;       Main bus scanning loop.  For each bus, scan until we find a VGA device
;       and call BoardPciControl if we do find a device.  If the board
;       module indicates that we should skip that device, scan until we find
;       another device.

PciVgaBackScan_BusLoop:
        cmp     bh, 00h                 ; is this bus 0?
        je      PciVgaBackScan_Exit     ; if so, we're done.

        Pcall   PciFindMapEntry         ; (ES:DI) = ptr to PCIBI entry for this bus.
        ASSUME  ES:NOTHING

        mov     di, PCI_CONFIG_VENDOR_DEVICE_ID
        PciFunc READ_CONFIG_DWORD
        inc     ecx                     ; if (ECX) == 0ffffffffh, set ECX to 0.
        jz      PciVgaBackScan_DeviceLoop ; find next device if no device 0.

;       Found a valid PCI device 0.  Test to see if it is a bridge.

        mov     di, PCI_CONFIG_HEADER_TYPE
        PciFunc READ_CONFIG_BYTE        ; call PCI services.
        and     cl, PCI_CONFIG_HEADER_TYPE_MASK
        cmp     cl, PCI_CONFIG_HEADER_TYPE_CARDBUS
        je      PciVgaBackScan_DeviceLoop
        cmp     cl, PCI_CONFIG_HEADER_TYPE_BRIDGE
        jne     PciVgaBackScan_SkipFind

;       Device scan loop.  Scan all the devices on this bus to see if any are
;       VGA parts.

PciVgaBackScan_DeviceLoop:
        Gcall   PciFindNextDevice, PCI  ; (BL) = next device or function.
        jc      PciVgaBackScan_NextBus

PciVgaBackScan_SkipFind:

;       Check for VGA cards.  If we have already found one, skip this section.
;       Otherwise, see if this device is a VGA controller and, if so, record
;       it's location (bus, device, function) in PciVgaLocation.

        USEEXTDATA ds

;       We have not found a VGA device yet.  Determine if this device is a VGA
;       controller.

        mov     di, PCI_CONFIG_FULL_CLASS_ID
        PciFunc READ_CONFIG_DWORD
        shr     ecx, 16                 ; ECX = Class Code, Sub Class Code.

        cmp     ecx, 0300h              ; is it a VGA chip?
        jne     PciVgaBackScan_DeviceLoop ; if not, we're done.

;       In the default case, this is the first VGA device found, starting from
;       the largest PCI bus number.  Report this to BoardPciControl and record
;       its location in PciVgaLocation if BoardPciControl allows it by not
;       changing the value passed to it in (AX) to 0ffffh.

        mov     ax, PCI_CTL_VGA_SCAN
        shl     eax, 16
        mov     ax, bx
        Gcall   BoardPciControl, POST   ; default action returns (AX) unchanged.
        cmp     ax, 0ffffh              ; did board reset VGA device?
        je      PciVgaBackScan_DeviceLoop ; continue if it did.
        mov     PciVgaLocation, ax      ; store selected VGA device.
        jmp     PciVgaBackScan_Exit     ; done.

;       Now we're ready to transition to the next bus.

PciVgaBackScan_NextBus:
        dec     bh                      ; next bus.
        jmp     PciVgaBackScan_BusLoop

PciVgaBackScan_Exit:
        POPALLD
EndProc PciVgaBackScan

;***   PciRecordRegion - Record PCI BAR Region Request.
;
;   FUNCTIONAL DESCRIPTION.
;       This function records a request for a specific region assignment
;       for a particular PCI BAR.  The request for allocation will be filled
;       on an opportunistic basis once we know what resources are required.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       DI      - PCI configuration index for desired configuration.
;       DL      - type of resource request.
;       CL      - power of 2 base of required size and alignment.
;
;   EXIT.
;       CY      - set on failure, clear on success.
;
;   USES.
;       none.

DefProc PciRecordRegion, GLOBAL
        PUSHALLD

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciRecordRegion: Device $x requests type $b of size $b at $x.\n>, <bx, dx, cx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        mov     ax, di                  ; keep config index in (AX) for now.
        Pcall   PciGetAllocStructure    ; get current allocation structure.
        ASSUME  DS:NOTHING
        mov     si, ds:pcialloc_nextfree.[di] ; (SI) = address of new region.
        cmp     si, (0-(SIZE PCIBAR))   ; is there space?
        jb      PciRecordRegion_StartAllocation
        stc                             ; indicate failure.
        jmp     PciRecordRegion_Exit    ; exit with failure.

;       Start the process of allocating resource request.

PciRecordRegion_StartAllocation:
        lea     bp, ds:pcialloc_iosizearray.[di] ; start by assuming I/O array.
        cmp     dl, 16                  ; is this an I/O array request?
        je      PciRecordRegion_Check   ; check to be sure range in limit.
        lea     bp, ds:pcialloc_mem32sizearray.[di] ; assume 32 bit memory size array.
        cmp     dl, 32                  ; is this an 32 memory array request?
        je      PciRecordRegion_Check   ; check to be sure range in limit.
        lea     bp, ds:pcialloc_mem64sizearray.[di] ; definitely a 64 bit memory size array.

;       Check to be sure range size is within limit.

PciRecordRegion_Check:
        cmp     cl, dl                  ; is the bit in range?
        jb      PciRecordRegion_FindAndWrite ; find index and write values.
        stc                             ; size is out of allowed range.
        jmp     PciRecordRegion_Exit    ; exit at once.

;       Find actual word index and setup for writing values.

PciRecordRegion_FindAndWrite:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciRecordRegion: Record starts at $x, >, <si>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        add     ds:pcialloc_nextfree.[di], (SIZE PCIBAR) ; allocate BAR.

        IF      OPTION_DIAGNOSE_PCI
        mov     dx, ds:pcialloc_nextfree.[di]
        CPRINTF <ends at $x.\n>, <dx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        sub     ch, ch                  ; clear high byte.
        shl     cx, 1                   ; multiply by 2 for word index.
        add     bp, cx                  ; add to base to find BAR record for size.
        mov     ds:pcibar_device.[si], bx ; set device.
        mov     ds:pcibar_register.[si], ax ; set configuration index.
        mov     ax, word ptr ds:[bp]    ; get old BAR record for given size.
        mov     ds:pcibar_next.[si], ax ; link old BAR record to new BAR record.
        mov     word ptr ds:[bp], si    ; set link to new BAR.
        clc                             ; indicate success.

PciRecordRegion_Exit:
        POPALLD
EndProc PciRecordRegion

;***   PciAssignNewAlloc - Allocate New Assignment Area.
;
;   FUNCTIONAL DESCRIPTION.
;       This function allocates a new temporary assignment region to replace
;       the current temporary assignment region.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       CY      - set on failure, clear on success.
;
;   USES.
;       none.

DefProc PciAssignNewAlloc, GLOBAL
        PUSHALLD
        Pcall   PciGetAllocStructure    ; get current allocation structure.
        ASSUME  DS:NOTHING
        mov     si, di                  ; save current object.
        mov     di, ds:pcialloc_nextfree.[di] ; get next free byte.
        cmp     di, (0-(SIZE PCIALLOC)) ; is (DI) about to overflow?
        jb      PciAssignNewAlloc_ClearPciAlloc
        stc                             ; indicate failure due to lack of RAM.
        jmp     PciAssignNewAlloc_Exit  ; exit at once.

;       Clear and initialize newly allocated object.

PciAssignNewAlloc_ClearPciAlloc:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignNewAlloc: Object at $x:$x, prior=$x.\n>, <ds, di, si>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        mov     ax, ds                  ; (AX) = allocation segment.
        mov     es, ax                  ; (ES) = allocation segment.
        ASSUME  ES:NOTHING
        cld                             ; clear forward.
        mov     cx, (pcialloc_iosizearray) ; copy up to allocation area.
        push    di                      ; preserve base address.
        push    si
        rep     movsb                   ; copy the structure up to iosizearray.
        sub     al, al                  ; clear to 0.
        mov     cx, ((SIZE PCIALLOC) - pcialloc_iosizearray) ; remainder.
        rep     stosb                   ; clear remainder to 0.
        pop     si
        pop     di
        add     ds:pcialloc_nextfree.[di], (SIZE PCIALLOC) ; include new struct.
        mov     ds:pcialloc_prior.[di], si ; store as prior object.

        push    si
        sub     si, si                  ; (DS:SI) = ptr to 1st PCIALLOC in segment.
        mov     ds:pcialloc_current.[si], di ; set new object as current.
        pop     si

;       For each of the decode ranges, we arrange for the top of the range to
;       start out one byte below the bottom of the range.  This deliberate
;       decision allows us to detect when the range has not been used.  For
;       PCI to PCI bridges, if the decode range is unused the limit will be
;       set to a larger value than the base.  This prevents the decoder from
;       decoding any address space for an unused range.

        mov     ax, ds:pcialloc_botio.[si] ; get bottom I/O port.
        dec     ax                      ; start top at one less then bottom.
        mov     ds:pcialloc_topio.[di], ax ; store top I/O port.
        mov     eax, ds:pcialloc_bot32.[si] ; get bottom of 32 bit range.
        dec     eax                     ; start top at one less than bottom.
        mov     ds:pcialloc_top32.[di], eax ; store top 32 bit address.
        mov     eax, ds:pcialloc_bot64low.[si] ; get bottom low 32 bits of 64 bit base.
        mov     edx, ds:pcialloc_bot64high.[si] ; get bottom high 32 bits of 56 bit base.
        dec     eax                     ; decrement by one.
        jnc     PciAssignNewAlloc_Store64Bit ; go store 64 bit address.
        dec     edx                     ; carry to (EDX).
PciAssignNewAlloc_Store64Bit:
        mov     ds:pcialloc_top64low.[di], eax ; store top low 32 bits of 64 bit limit.
        mov     ds:pcialloc_top64high.[di], edx ; store top high 32 bits of 64 bit limit.
        clc                             ; indicate success.

PciAssignNewAlloc_Exit:
        POPALLD
EndProc PciAssignNewAlloc

;***   PciAssignDealloc - Drop Back one Level from Current Allocation.
;
;   FUNCTIONAL DESCRIPTION.
;       This function drops back one level
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       CY      - set on failure, clear on success.
;
;   USES.
;       none.

DefProc PciAssignDealloc, GLOBAL
        PUSHALLD
        Pcall   PciGetAllocStructure    ; get currently allocated structure.
        ASSUME  DS:NOTHING
        mov     si, di                  ; keep pointer to original structure.
        mov     di, ds:pcialloc_prior.[si] ; point to prior structure.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignDealloc: Object at $x:$x, prior=$x.\n>, <ds, si, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    si
        sub     si, si                  ; (DS:SI) = ptr to 1st structure in segment.
        mov     word ptr ds:pcialloc_current.[si], di ; switch structure back up one.
        pop     si

        mov     ax, ds:pcialloc_botio.[si] ; get source I/O range bottom.
        mov     ds:pcialloc_botio.[di], ax ; update old source I/O bit range bottom.
        mov     eax, ds:pcialloc_bot32.[si] ; get source 32 bit range bottom.
        mov     ds:pcialloc_bot32.[di], eax ; update old source 32 bit range bottom.
        mov     eax, ds:pcialloc_bot64low.[si] ; get source 64 bit range bottom.
        mov     edx, ds:pcialloc_bot64high.[si] ; get source 64 bit range bottom.
        mov     ds:pcialloc_bot64low.[di], eax ; update old source 64 bit range bottom.
        mov     ds:pcialloc_bot64high.[di], edx ; update old source 64 bit range bottom.

        POPALLD
EndProc PciAssignDealloc

;***   PciGetAllocStructure - Get Current Allocation Structure.
;
;   FUNCTIONAL DESCRIPTION.
;       This function locates the current PCI allocation structure and returns
;       the address of the structure.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       DS:DI   -       returned pointer to 1st PCIALLOC structure in segment.
;
;   USES.
;       none.

DefProc PciGetAllocStructure, GLOBAL
        push    ax
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        add     ax, PCI_ALLOC_SEGMENT_OFFSET
        mov     ds, ax
        ASSUME  DS:NOTHING
        sub     di, di                  ; (DS:DI) = ptr, 1st structure.
        mov     di, word ptr ds:pcialloc_current.[di] ; (DS:DI) = ptr, current structure.
        pop     ax
EndProc PciGetAllocStructure

;***   PciAssignAllocInit - Initialize First Assignment Allocation Area.
;
;   FUNCTIONAL DESCRIPTION.
;       This function initializes the first PCI assignment allocation
;       initialization area, describing bus 0.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       none.

DefProc PciAssignAllocInit, GLOBAL
        PUSHALLD
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        add     ax, PCI_ALLOC_SEGMENT_OFFSET
        mov     es, ax
        ASSUME  ES:NOTHING
        sub     di, di                  ; initialize offset to 0.
        mov     cx, (SIZE PCIALLOC)     ; root PCI allocation structure.
        cld                             ; clear direction.
        sub     al, al                  ; clear to 0.
        push    di
        rep     stosb                   ; clear structure to 0.
        pop     di

;       Initialize first assignment allocation structure set.

        mov     es:pcialloc_nextfree.[di], (SIZE PCIALLOC) ; next free byte.
        mov     es:pcialloc_topio.[di], CONFIG_PCI_IO_LIMIT_16
        mov     es:pcialloc_botio.[di], CONFIG_PCI_IO_LIMIT_16
        mov     dword ptr es:pcialloc_top32.[di], CONFIG_PCI_MEM_LIMIT_32
        mov     dword ptr es:pcialloc_bot32.[di], CONFIG_PCI_MEM_LIMIT_32
        mov     dword ptr es:pcialloc_top64low.[di], CONFIG_PCI_MEM_LIMIT_64_LO
        mov     dword ptr es:pcialloc_top64high.[di], CONFIG_PCI_MEM_LIMIT_64_HI
        mov     dword ptr es:pcialloc_bot64low.[di], CONFIG_PCI_MEM_LIMIT_64_LO
        mov     dword ptr es:pcialloc_bot64high.[di], CONFIG_PCI_MEM_LIMIT_64_HI
        POPALLD
EndProc PciAssignAllocInit

;***   PciAssignOpportunistic - Assign PCI Resource Opportunistically.
;
;   FUNCTIONAL DESCRIPTION.
;       This function initializes the first PCI assignment allocation
;       initialization area, describing bus 0.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;       K. C. Taylor    06/02/01.       #5.3.4, fixed 64 bit assignment bug.
;       K. C. Taylor    06/11/08.       #6.0, fixed bug in memory overflow handler.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       SI      - current enumeration functionality mask.
;
;   EXIT.
;       none.
;
;   USES.
;       all.

DefProc PciAssignOpportunistic, GLOBAL
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: flags=$x\n>, <si>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        shl     esi, 16                 ; keep original flag set in top.
        Pcall   PciGetAllocStructure    ; get current allocation structure.
        ASSUME  DS:NOTHING

;       Opportunistically assign I/O resources.

PciAssignOpportunistic_IoLoop:
        mov     dx, pcialloc_botio.[di] ; get current bottom (top for next assignment).
        cmp     dx, CONFIG_PCI_IO_BASE_16 ; check against bottom.
        jbe     PciAssignOpportunistic_ClearIo ; clear remaining I/O assignments.

;       Seek matching alligned I/O request.

        bsf     cx, dx                  ; find bit index for current I/O.
        jz      PciAssignOpportunistic_ClearIo ; clear remaining I/O assignments.
        lea     bp, ds:pcialloc_iosizearray.[di] ; start with I/O array base.
        add     bp, cx                  ; start with byte.
        add     bp, cx                  ; move to word.

;       Starting with the best, seek the largest request whose end will align
;       with proposed ending I/O port.

PciAssignOpportunistic_SubIoLoop:
        mov     si, word ptr ds:[bp]    ; get current proposed I/O request.
        test    si, si                  ; is there an I/O request of this size?
        jnz     PciAssignOpportunistic_TryIoAssignment
        jcxz    PciAssignOpportunistic_SubIoLoopUp ; if (CX) already 0.
        sub     bp, 2                   ; try prior size.
        loop    PciAssignOpportunistic_SubIoLoop ; continue until we run out.

;       Loop upward to find smallest miss-aligned I/O port.

PciAssignOpportunistic_SubIoLoopUp:
        add     bp, 2                   ; next larger size.
        inc     cx                      ; next larger size.
        cmp     cx, 16                  ; check against range limit.
        jae     PciAssignOpportunistic_Mem32Loop ; continue with 32 bit memory.
        mov     si, word ptr ds:[bp]    ; get current proposed I/O request.
        test    si, si                  ; is there an I/O request of this size?
        jz      PciAssignOpportunistic_SubIoLoopUp ; continue until there is.

;       Try to make I/O assignment.

PciAssignOpportunistic_TryIoAssignment:
        IF      OPTION_PCI_ISA_IO_ALIAS
        cmp     cx, 9                   ; can I/O request fit in allowed range?
        jae     PciAssignOpportunistic_NoIoAlias
        mov     ax, dx                  ; (AX) = current top of range.
        dec     ax                      ; (AX) = literal last byte in range.
        test    ax, 0300h               ; check 1k alignment.
        jz      PciAssignOpportunistic_NoIoAlias
        dec     dx                      ; find absolute top.
        and     dx, (NOT 0300h)         ; align down.
        or      dl, 0ffh                ; round correctly.
        inc     dx                      ; adjust back up.
        mov     ds:pcialloc_botio.[di], dx ; update I/O size array.
        jmp     PciAssignOpportunistic_IoLoop ; restart with updated alignment.
        ENDIF   ; (OPTION_PCI_ISA_IO_ALIAS)

;       No alias detected.  Handle resource allocation.

PciAssignOpportunistic_NoIoAlias:
        mov     ax, 1
        shl     ax, cl                  ; (AX) = initial I/O decode size.
        sub     dx, ax                  ; account for initial decode size.
        dec     ax                      ; mask.
        not     ax                      ; alignment mask.
        and     dx, ax                  ; align to I/O region size.
        cmp     dx, CONFIG_PCI_IO_BASE_16 ; check against bottom.
        jb      PciAssignOpportunistic_ClearIo ; clear remaining I/O ranges.
        mov     ds:pcialloc_botio.[di], dx ; store updated I/O base.
        movzx   ecx, dx                 ; (ECX) = I/O base address.
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        mov     di, ds:pcibar_register.[si] ; get device register.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: IoWrite $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        pop     di
        jc      PciAssignOpportunistic_Exit ; exit on failure.

;       Remove I/O port from list.

        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     word ptr ds:[bp], si    ; write to head pointer to unlink.
        jmp     PciAssignOpportunistic_IoLoop ; continue to next assignment.

;       Clear all remaining I/O port assignments.

PciAssignOpportunistic_ClearIo:
        lea     bp, ds:pcialloc_iosizearray.[di] ; start with I/O array base.
        mov     cx, 16                  ; clear all 16 regions.

PciAssignOpportunistic_ClearIoLoop:
        mov     si, word ptr ds:[bp]    ; get current BAR index.
        test    si, si                  ; is pointer null?
        jz      PciAssignOpportunistic_ClearIoNext ; move on to next.
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: IoClear $x, $b.\n>, <bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        mov     di, PCI_CONFIG_COMMAND  ; start with command register byte.
        push    cx
        PciFunc READ_CONFIG_BYTE
        and     cl, (NOT BIT0)          ; disable device I/O decodes.
        PciFunc WRITE_CONFIG_BYTE
        pop     cx
        pop     di
        jc      PciAssignOpportunistic_Exit ; exit on failure.
        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     word ptr ds:[bp], si    ; write to head pointer to unlink.
        jmp     PciAssignOpportunistic_ClearIoLoop

;       Move on to next I/O BAR.

PciAssignOpportunistic_ClearIoNext:
        add     bp, 2                   ; advance to next I/O object size.
        loop    PciAssignOpportunistic_ClearIoLoop

;       Opportunistically assign 32 bit memory resources.

PciAssignOpportunistic_Mem32Loop:
        mov     eax, ds:pcialloc_bot32.[di] ; get current 32 bit base.
        cmp     eax, CONFIG_PCI_MEM_BASE_32
        jb      PciAssignOpportunistic_ClearMem32 ; out of space, clear ranges.

;       Seek matching aligned memory request.

        bsf     ecx, eax                ; find bit index.
        jz      PciAssignOpportunistic_ClearMem32 ; clear ranges.
        mov     bp, cx                  ; (BP) = byte index.
        shl     bp, 1                   ; (BP) = word index.
        add     bp, di                  ; base on PCIALLOC object.

;       Starting with the best, seek the largest request whose end will align
;       with proposed ending range.

PciAssignOpportunstic_SubMem32Loop:
        cmp     ds:pcialloc_mem32sizearray.[bp], 0 ; is range valid?
        jne     PciAssignOpportunistic_TryMem32Assignment
        test    esi, (PCIHDR_FLAG_MEM_COMBINE_64_32 SHL 16) ; combined mode?
        jz      PciAssignOpportunistic_SubMem32Next
        cmp     ds:pcialloc_mem64sizearray.[bp], 0 ; is range valid?
        jne     PciAssignOpportunistic_TryMem3264Assignment

;       Find next smaller size.

PciAssignOpportunistic_SubMem32Next:
        jcxz    PciAssignOpportunistic_SubMem32LoopUp ; if (CX) already 0.
        sub     bp, 2                   ; next smaller size.
        loop    PciAssignOpportunstic_SubMem32Loop ; continue.

;       Loop upward to find smallest missaligned memory range.

PciAssignOpportunistic_SubMem32LoopUp:
        add     bp, 2                   ; next word up.
        inc     cx                      ; next size scale up.
        cmp     cx, 32                  ; verify range.
        jae     PciAssignOpportunistic_Mem64Loop
        cmp     ds:pcialloc_mem32sizearray.[bp], 0 ; is range valid?
        jne     PciAssignOpportunistic_TryMem32Assignment
        test    esi, (PCIHDR_FLAG_MEM_COMBINE_64_32 SHL 16) ; combined mode?
        jz      PciAssignOpportunistic_SubMem32LoopUp
        cmp     ds:pcialloc_mem64sizearray.[bp], 0 ; is range valid?
        jne     PciAssignOpportunistic_TryMem3264Assignment
        jmp     PciAssignOpportunistic_SubMem32LoopUp ; continue to try.

;       Opportunistically assign proposed range.

PciAssignOpportunistic_TryMem32Assignment:
        mov     si, ds:pcialloc_mem32sizearray.[bp]
        mov     edx, eax                ; (EDX) = current base top.
        mov     eax, 1
        shl     eax, cl                 ; (EAX) = size of current region.
        sub     edx, eax                ; account for decode size.
        dec     eax                     ; (EAX) = size mask.
        not     eax                     ; (EAX) = alignment mask.
        and     edx, eax                ; apply alignment mask.
        cmp     edx, CONFIG_PCI_MEM_BASE_32 ; check against base line.
        jb      PciAssignOpportunistic_ClearMem32 ; remaining assignments.
        mov     ds:pcialloc_bot32.[di], edx ; store aligned base address.
        mov     ecx, edx                ; (ECX) = aligned base address.
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        mov     di, ds:pcibar_register.[si] ; get device register.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: M32Write $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        pop     di
        jc      PciAssignOpportunistic_Exit ; exit on failure.

;       Remove BAR request from list.

        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     ds:pcialloc_mem32sizearray.[bp], si
        jmp     PciAssignOpportunistic_Mem32Loop ; find next range.

PciAssignOpportunistic_TryMem3264Assignment:
        mov     si, ds:pcialloc_mem64sizearray.[bp]
        mov     edx, eax                ; (EDX) = current base top.
        mov     eax, 1
        shl     eax, cl                 ; (EAX) = size of current region.
        sub     edx, eax                ; account for decode size.
        dec     eax                     ; (EAX) = size mask.
        not     eax                     ; (EAX) = alignment mask.
        and     edx, eax                ; apply alignment mask.
        cmp     edx, CONFIG_PCI_MEM_BASE_32 ; check against base line.
        jb      PciAssignOpportunistic_ClearMem32 ; remaining assignments.
        mov     ds:pcialloc_bot32.[di], edx ; store aligned base address.
        mov     ecx, edx                ; (ECX) = aligned base address.
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        mov     di, ds:pcibar_register.[si] ; get device register.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: M3264Write $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        sub     ecx, ecx                ; clear high dword.
        add     di, 4                   ; next dword.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: M3264Write $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        PciFunc WRITE_CONFIG_DWORD      ; clear high 32 bits.

        pop     di

;       Remove BAR request from list.

        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     ds:pcialloc_mem64sizearray.[bp], si
        jmp     PciAssignOpportunistic_Mem32Loop ; find next range.

;       Clear remaining 32 bit BARs.

PciAssignOpportunistic_ClearMem32:
        mov     bp, di                  ; base at PCIALLOC structure.
        mov     cx, 32                  ; run for 32 bits.

PciAssignOpportunistic_ClearMem32Loop:
        mov     si, ds:pcialloc_mem32sizearray.[bp] ; get current memarray.
        test    si, si                  ; verify that entry is valid.
        jz      PciAssignOpportunistic_ClearMem32Next

;       Clear 32 bit memory region.

        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: ClearMem32 $x, $b.\n>, <bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        mov     di, PCI_CONFIG_COMMAND  ; start with command byte.
        push    cx
        PciFunc READ_CONFIG_BYTE        ; read command byte.
        and     cl, (NOT BIT1)          ; disable memory decodes.
        PciFunc WRITE_CONFIG_BYTE       ; write to configuration register.
        pop     cx
        pop     di
        jc      PciAssignOpportunistic_Exit ; exit on failure.

        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     ds:pcialloc_mem32sizearray.[bp], si ; update head pointer.
        jmp     PciAssignOpportunistic_ClearMem32Loop

;       Move on to next I/O BAR.

PciAssignOpportunistic_ClearMem32Next:
        add     bp, 2                   ; advance to next class/size.
        loop    PciAssignOpportunistic_ClearMem32Loop

;       Assign all remaining 64 bit address ranges.

PciAssignOpportunistic_Mem64Loop:
        test    esi, (PCIHDR_FLAG_MEM_COMBINE_64_32 SHL 16) ; combined mode?
        jnz     PciAssignOpportunistic_ClearMem64
        mov     edx, pcialloc_bot64high.[di] ; (EDX) = high part of bottom.
        mov     eax, pcialloc_bot64low.[di] ; (EAX) = low part of bottom.
        cmp     edx, CONFIG_PCI_MEM_BASE_64_HI ; compare high base address.
        jb      PciAssignOpportunistic_ClearMem64
        ja      PciAssignOpportunistic_SeekMem64 ; skip directly to seek.
        cmp     eax, CONFIG_PCI_MEM_BASE_64_LO ; compare low base address.
        jbe     PciAssignOpportunistic_ClearMem64

;       Seek matching range.

PciAssignOpportunistic_SeekMem64:
        bsf     ecx, eax                ; find least significant bit.
        jnz     PciAssignOpportunistic_SubMem64 ; setup for sub mem 64 loop.
        bsf     ecx, edx                ; find least significant bit.
        jz      PciAssignOpportunistic_ClearMem64
        add     cl, 32                  ; high 32 bits.

PciAssignOpportunistic_SubMem64:
        mov     bp, cx                  ; (BP) = current base.
        shl     bp, 1                   ; (BP) = word index.
        lea     bp, ds:pcialloc_mem64sizearray.[bp+di]

;       Starting with the best, seek the largest request whose end will align
;       with proposed ending range.

PciAssignOpportunistic_SubMem64Loop:
        mov     si, word ptr ds:[bp]    ; get current 64 bit array index.
        test    si, si                  ; has entry been found?
        jnz     PciAssignOpportunistic_TryMem64Assignment
        jcxz    PciAssignOpportunistic_SubMem64LoopUp ; if (CX) already 0.
        sub     bp, 2                   ; try next lower size.
        loop    PciAssignOpportunistic_SubMem64Loop ; continue until found.

;       Loop upward to find smallest miss-aligned 64 bit region.

PciAssignOpportunistic_SubMem64LoopUp:
        add     bp, 2                   ; next larger size.
        inc     cx                      ; next larger size.
        cmp     cx, 64                  ; check against range limit.
        jae     PciAssignOpportunistic_Success ; finished, return success.
        mov     si, word ptr ds:[bp]    ; get current proposed I/O request.
        test    si, si                  ; is there an I/O request of this size?
        jz      PciAssignOpportunistic_SubMem64LoopUp ; continue until there is.

;       Attempt to assign 64 bit decode range.

PciAssignOpportunistic_TryMem64Assignment:
        cmp     cx, 32                  ; is region 4 gig or greater?
        jae     PciAssignOpportunistic_TryMem64HighAssignment
        mov     ebx, 1
        shl     ebx, cl                 ; (EBX) = final size.
        sub     eax, ebx                ; subtract size
        sbb     edx, 0                  ; subtract size.
        dec     ebx                     ; (EBX) = low mask.
        not     ebx                     ; (EBX) = alignment mask.
        and     eax, ebx                ; (EAX) = alignment mask.
        jmp     PciAssignOpportunistic_TryMem64Verify

PciAssignOpportunistic_TryMem64HighAssignment:
        sub     cx, 32                  ; work on high 32 bits.
        mov     ebx, 1
        shl     ebx, cl                 ; (EBX) = high 32 bits of length.
        sub     edx, ebx                ; account for size.
        jc      PciAssignOpportunistic_ClearMem64 ; if wrapped, out of memory.
        dec     ebx                     ; (EBX) = low mask.
        not     ebx                     ; (EBX) = alignment mask.
        sub     eax, eax                ; low dword must be 0.

PciAssignOpportunistic_TryMem64Verify:
        cmp     edx, CONFIG_PCI_MEM_BASE_64_HI ; compare high base address.
        jb      PciAssignOpportunistic_ClearMem64
        ja      PciAssignOpportunistic_SetMem64
        cmp     eax, CONFIG_PCI_MEM_BASE_64_LO ; compare low base address.
        jb      PciAssignOpportunistic_ClearMem64

PciAssignOpportunistic_SetMem64:
        mov     ds:pcialloc_bot64low.[di], eax ; set low dword.
        mov     ds:pcialloc_bot64high.[di], edx ; set high dword.
        mov     ecx, eax                ; write low dword.
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        mov     di, ds:pcibar_register.[si] ; get device register.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic:M64Write $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        mov     ecx, edx                ; (ECX) = high dword to write.
        add     di, 4                   ; next configuration index.
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignOpportunistic: M64Write $lx to $x, $b.\n>, <ecx, bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        pop     di
        jc      PciAssignOpportunistic_Exit

;       Remove BAR request from list.

        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     word ptr ds:[bp], si    ; remove BAR from list.
        jmp     PciAssignOpportunistic_Mem64Loop ; handle next 64 bit range.

;       Clear all remaining 64 bit BARs.

PciAssignOpportunistic_ClearMem64:
        mov     cx, 64                  ; go through all 64 ranges.
        lea     bp, ds:pcialloc_mem64sizearray.[di]

PciAssignOpportunistic_ClearMem64Loop:
        mov     si, word ptr ds:[bp]    ; get current BAR.
        test    si, si                  ; is there a BAR request?
        jz      PciAssignOpportunistic_ClearMem64Next
        mov     bx, ds:pcibar_device.[si] ; get device assignment.
        push    di
        mov     di, PCI_CONFIG_COMMAND
        push    cx
        PciFunc READ_CONFIG_BYTE        ; read command register.
        and     cl, (NOT BIT1)          ; disable memory decodes.
        PciFunc WRITE_CONFIG_BYTE       ; write back command register.
        pop     cx
        pop     di
        mov     si, ds:pcibar_next.[si] ; get next BAR in list.
        mov     word ptr ds:[bp], si    ; remove BAR from list.
        jmp     PciAssignOpportunistic_ClearMem64Loop

PciAssignOpportunistic_ClearMem64Next:
        add     bp, 2                   ; next word index.
        loop    PciAssignOpportunistic_ClearMem64Loop ; clear next size.

;       Finished with opportunistic assignments.

PciAssignOpportunistic_Success:
        clc

PciAssignOpportunistic_Exit:
EndProc PciAssignOpportunistic

;***   PciCheckForCardBus - Check for CardBus Controller.
;
;   FUNCTIONAL DESCRIPTION.
;       This function is used by bridge initialization code that needs to
;       follow a seperate path when configuring CardBus bridges to identify
;       when a specific device is a CardBus bridge.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    07/02/07.       #5.3.3, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;
;   EXIT.
;       CY      - set if device is a CardBus bridge, clear otherwise.
;
;   USES.
;       Flags.

DefProc PciCheckForCardBus
        push    cx
        push    di
        mov     di, PCI_CONFIG_HEADER_TYPE ; check for Cardbus bridge.
        PciFunc READ_CONFIG_BYTE        ; read header type.
        pop     di
        and     cl, PCI_CONFIG_HEADER_TYPE_MASK ; ignore function flag.
        cmp     cl, PCI_CONFIG_HEADER_TYPE_CARDBUS ; if cardbus, handle one way.
        pop     cx
        stc                             ; assume CardBus controller found.
        je      @f                      ; skip clearing carry if found.
        clc                             ; indicate CardBus controller not found.
@@:
EndProc PciCheckForCardBus

;***   PciAssignBist - Handle BIST Resource Assignment.
;
;   FUNCTIONAL DESCRIPTION.
;       This function handles the built in self test as a PCI resource to
;       be assigned.  If the built in self test is supported, this function
;       will start the built in self test and run it to completion.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if BIST fails, clear otherwise.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignBist, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBist: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        IF      (CONFIG_PCI_BIST_TIMEOUT NE 0)
        PciFunc READ_CONFIG_BYTE
        jc      PciAssignBist_Exit      ; exit on failure.
        test    cl, PCI_CONFIG_BIST_SUPPORTED ; is BIST supported?
        clc                             ; indicate success.
        jz      PciAssignBist_Exit      ; exit with success if not supported.
        or      cl, PCI_CONFIG_BIST_START ; set BIST start bit.
        PciFunc WRITE_CONFIG_BYTE       ; start built in self test.
        jc      PciAssignBist_Exit      ; exit on failure.
        Gcall   DelayInit, BIOS         ; (CX) = delay init value.

;       Wait until BIST passes or timeout occurs.

PciAssignBist_Loop:
        push    cx                      ; preserve delay init value.
        PciFunc READ_CONFIG_BYTE        ; read BIST from PCI.
        jc      PciAssignBist_Exit      ; exit on failure.
        test    cl, PCI_CONFIG_BIST_STATUS ; check status for in progress.
        pop     cx                      ; restore delay init value at end.
        clc                             ; indicate success.
        jz      PciAssignBist_Exit      ; exit after successful BIST.
        Gcall   DelayComp, BIOS         ; check time limit.
        cmp     ax, (CONFIG_PCI_BIST_TIMEOUT * 19) ; has timeout expired?
        jb      PciAssignBist_Loop      ; continue if not.
        stc                             ; indicate failure due to timeout.
        ELSE    ; (CONFIG_PCI_BIST_TIMEOUT NE 0)
        clc                             ; indicate BIST passed.
        ENDIF   ; (CONFIG_PCI_BIST_TIMEOUT NE 0)

;       Built in self test is complete.

PciAssignBist_Exit:
EndProc PciAssignBist

;***   PciAssignBistBus - Handle Distribution of BIST to Bridges.
;
;   FUNCTIONAL DESCRIPTION.
;       This function is responsible for running the built in self test on
;       devices found on the far side of bridges.  We do not run the BIST
;       on the bridge itself, because the BIST for each bridge has already
;       been run during PciFindBridge.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/12.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if BIST fails, clear otherwise.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignBistBus, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBistBus: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    bx
        push    bp
        push    si
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, PCIHDR_FLAG_BIST    ; only run the BIST for devices.
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     si
        pop     bp
        pop     bx
EndProc PciAssignBistBus

;***   PciAssignBar - Handle PCI Base Address Register Assignment.
;
;   FUNCTIONAL DESCRIPTION.
;       This function handles the assigment of all PCI base address registers.
;       It operates by first determining the type of BAR and then assigning
;       a resource to the BAR if the BAR parameters are acceptable.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to assign BAR fails, clear on success.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignBar, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBar: Device,Register=$x, $b.\n>, <bx, di>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

;       Find the BAR type and handle appropriately.

        test    PCI_GRP:pcihdr_flags.[bp], PCIHDR_FLAG_OPTROM
        jnz     PciAssignBar_AssignMemory ; skip directly to memory range.

        PciFunc READ_CONFIG_BYTE        ; read low byte to determine type.
        jc      PciAssignBar_Exit       ; exit on failure.
        test    cl, PCI_CONFIG_BAR_IO   ; is this an I/O BAR?
        jnz     PciAssignBar_CheckIo    ; check I/O bit and assign.
        test    cl, PCI_CONFIG_BAR_PREFETCH ; check for prefetchable type.
        jnz     PciAssignBar_TypePrefetch ; handle prefetchable.
        and     cl, PCI_CONFIG_BAR_TYPE_MASK ; check type only.
        mov     dx, PCIHDR_FLAG_32BIT   ; assume 32 bit assignment.
        cmp     cl, PCI_CONFIG_BAR_TYPE_32BIT ; if simple 32 bit, handle it.
        je      PciAssignBar_CheckMemory ; assign 32 bit region.
        mov     dx, PCIHDR_FLAG_32LEG   ; handle 32 bit legacy assign.
        cmp     cl, PCI_CONFIG_BAR_TYPE_LEGACY ; is it a legacy assignment?
        je      PciAssignBar_CheckLegacy ; assign legacy region.
        mov     dx, PCIHDR_FLAG_64BIT   ; 64 bit region assignment.
        cmp     cl, PCI_CONFIG_BAR_TYPE_64BIT ; is it a 64 bit assignment?
        je      PciAssignBar_CheckMemory64 ; assign 64 bit region.
        stc                             ; indicate unsupported type.
        jmp     PciAssignBar_Exit       ; exit with unsupported type.

;       Check for type for prefetchable regions.

PciAssignBar_TypePrefetch:
        and     cl, PCI_CONFIG_BAR_TYPE_MASK ; check type only.
        mov     dx, PCIHDR_FLAG_32BIT_PREFETCH ; assume 32 bit assignment.
        cmp     cl, PCI_CONFIG_BAR_TYPE_32BIT ; if simple 32 bit, handle it.
        je      PciAssignBar_CheckMemory ; assign 32 bit region.
        mov     dx, PCIHDR_FLAG_32LEG_PREFETCH ; handle 32 bit legacy assign.
        cmp     cl, PCI_CONFIG_BAR_TYPE_LEGACY ; is it a legacy assignment?
        je      PciAssignBar_CheckLegacy ; assign legacy region.
        mov     dx, PCIHDR_FLAG_64BIT_PREFETCH ; 64 bit region assignment.
        cmp     cl, PCI_CONFIG_BAR_TYPE_64BIT ; is it a 64 bit assignment?
        je      PciAssignBar_CheckMemory64 ; assign 64 bit region.
        stc                             ; indicate unsupported type.
        jmp     PciAssignBar_Exit       ; exit with unsupported type.

;       Check to see if memory type specified by (DX) mask should be
;       enumerated at this time.

PciAssignBar_CheckMemory:
        test    si, dx                  ; is enumeration of (DX) supported?
        clc                             ; indicate success.
        jz      PciAssignBar_Exit       ; resource successfully not assigned.

;       Determine and record memory resource requirements.

PciAssignBar_AssignMemory:
        mov     ecx, 0fffffff0h         ; 32 bit region assignment.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        jc      PciAssignBar_Exit       ; exit on failure.
        PciFunc READ_CONFIG_DWORD       ; read those bits that stick.
        jc      PciAssignBar_Exit       ; exit on failure.
        push    ecx
        mov     ecx, CONFIG_PCI_MEM_BASE_32 ; move BAR out of the way.
        PciFunc WRITE_CONFIG_DWORD
        pop     ecx
        and     ecx, 0fffffff0h         ; mask out type bits.
        not     ecx                     ; invert as part of finding size.
        inc     ecx                     ; (ECX) = size and alignment.
        jz      PciAssignBar_OutOfRange ; assignment out of range.

;       Only one bit should be set in (ECX), indicating size and alignment.
;       Find the bit, verify that it is the only bit set and store the bit.

        bsf     edx, ecx                ; find least significant bit.
        bsr     ecx, ecx                ; find most significant bit.
        cmp     cl, dl                  ; verify that only one bit is set.
        jne     PciAssignBar_FixedBar   ; bar has fixed assignment.
        mov     dl, 32                  ; indicate 32 bit region.
        Pcall   PciRecordRegion         ; record memory region request.
        jmp     PciAssignBar_Exit       ; region recorded, exit assignment.

;       Check to see if legacy memory type specified by (DX) mask should be
;       enumerated at this time.

PciAssignBar_CheckLegacy:
        test    si, dx                  ; is enumeration of (DX) supported?
        clc                             ; indicate success.
        jz      PciAssignBar_Exit       ; resource successfully not assigned.

;       Note that this enumeration code does not currently support 32 bit
;       legacy memory assignment requests.  We will simply assign 0 when
;       such requests are encounterred.

        sub     ecx, ecx                ; clear to 0.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        jmp     PciAssignBar_Exit       ; exit on success or failure.

;       Check to see if 64 bit memory regions are supported by the controller.

PciAssignBar_CheckMemory64:
        test    si, dx                  ; is enumeration of (DX) supported?
        clc                             ; indicate success.
        jz      PciAssignBar_SkipNext   ; skip next resource check.
        test    word ptr PCI_GRP:pcihdr_flags.[bp], \
                (PCIHDR_FLAG_64BIT OR PCIHDR_FLAG_64BIT_PREFETCH)
        clc                             ; indicate success.
        jz      PciAssignBar_CheckMemory ; bar at end of range, must be 32 bits.

;       Determine region size requirement.

        mov     ecx, 0fffffff0h         ; 32 bit region assignment.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        jc      PciAssignBar_Exit       ; exit on failure.
        PciFunc READ_CONFIG_DWORD       ; read those bits that stick.
        jc      PciAssignBar_Exit       ; exit on failure.
        push    ecx
        mov     ecx, CONFIG_PCI_MEM_BASE_32 ; move BAR out of the way.
        PciFunc WRITE_CONFIG_DWORD
        pop     ecx
        and     ecx, 0fffffff0h         ; mask out type bits.
        not     ecx                     ; invert as part of finding size.
        inc     ecx                     ; (ECX) = size and alignment.
        jz      PciAssignBar_CheckTop   ; check top address.

;       Check to see if region is in range.

        bsf     edx, ecx                ; find least significant bit.
        bsr     ecx, ecx                ; find most significant bit.
        cmp     cl, dl                  ; verify that only one bit is set.
        jne     PciAssignBar_SkipNext   ; bar has fixed assignment.
        mov     dl, 64                  ; indicate 64 bit region.
        Pcall   PciRecordRegion         ; record memory region request.
        jmp     PciAssignBar_SkipNext   ; skip next resource check.

;       Check top 32 bits of 64 bit region.

PciAssignBar_CheckTop:
        add     di, 4                   ; advance to next region.
        mov     ecx, 0ffffffffh         ; top 64 bit region assignment.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        jc      PciAssignBar_Exit       ; exit on failure.
        PciFunc READ_CONFIG_DWORD       ; read those bits that stick.
        jc      PciAssignBar_Exit       ; exit on failure.
        not     ecx                     ; invert as part of finding size.
        inc     ecx                     ; (ECX) = size and alignment.
        jz      PciAssignBar_SkipNext   ; skip region if too large.
        bsf     edx, ecx                ; find least significant bit.
        bsr     ecx, ecx                ; find most significant bit.
        cmp     cl, dl                  ; verify that only one bit is set.
        jne     PciAssignBar_SkipNext   ; bar has fixed assignment.
        add     cl, 32                  ; adjust for low 32 bits.
        mov     dl, 64                  ; indicate 64 bit region.
        sub     di, 4                   ; restore original base address.
        Pcall   PciRecordRegion         ; record memory region request.

;       Arrange to skip parsing next BAR as the current BAR is 64 bits.

PciAssignBar_SkipNext:
        pushf                           ; save flags.
        add     bp, (SIZE PCIHDR)       ; skip header.
        popf                            ; restore flags.
        jmp     PciAssignBar_Exit       ; finished BAR resource.

;       Check to see if 16 bit I/O ranges are being assigned.

PciAssignBar_CheckIo:
        test    si, PCIHDR_FLAG_IO      ; check for current I/O allocations.
        clc                             ; indicate success.
        jz      PciAssignBar_Exit       ; resource successfully not assigned.

;       Find 16 bit I/O requirements.

        mov     ecx, 00000fff8h         ; maximum I/O decode request size.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration I/O space.
        jc      PciAssignBar_Exit       ; exit if I/O range cannot be assigned.
        PciFunc READ_CONFIG_DWORD       ; read value written to I/O port.
        jc      PciAssignBar_Exit       ; exit if I/O range cannot be assigned.
        push    ecx
        mov     ecx, CONFIG_PCI_IO_BASE_16 ; move BAR out of the way.
        PciFunc WRITE_CONFIG_DWORD
        pop     ecx
        and     ecx, 00000fff8h         ; mask to find only relevant bits.
        not     cx                      ; invert as part of finding size.
        inc     cx                      ; (ECX) = size and alignment.
        jz      PciAssignBar_OutOfRange ; skip if size is too large.
        bsf     dx, cx                  ; find least significant bit.
        bsr     cx, cx                  ; find most significant bit.
        cmp     cl, dl                  ; verify bit sizes match.
        jne     PciAssignBar_FixedBar   ; bar has fixed assignment.
        mov     dl, 16                  ; region type is I/O.
        Pcall   PciRecordRegion         ; record memory region request.
        jmp     PciAssignBar_Exit       ; exit.

;       PCI BAR request was out of range.

PciAssignBar_OutOfRange:
        clc                             ; for now, just report success.
        jmp     PciAssignBar_Exit

;       PCI BAR had non-compliant implementation, so exit with success.

PciAssignBar_FixedBar:
        clc                             ; for now, just report success.
        jmp     PciAssignBar_Exit

PciAssignBar_Exit:
EndProc PciAssignBar

;***   PciAssignCacheLine - Assign Value to Cache Line.
;
;   FUNCTIONAL DESCRIPTION.
;       This function assigns a value to the PCI cache line size in the
;       configuration header of the device.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to write cache line register fails.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignCacheLine, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignCacheLine: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        Gcall   CpuGetCacheLineSize, POST ; (AL) = cache line size in bytes.
        shr     al, 2                   ; (AL) = cache line size in dwords.
        mov     cl, al                  ; (CL) = cache line size in dwords.
        PciFunc WRITE_CONFIG_BYTE       ; write value to register.
EndProc PciAssignCacheLine

;***   PciAssignLatency - Assign Latency Value.
;
;   FUNCTIONAL DESCRIPTION.
;       This function fills in the bus master latency register setting for
;       a PCI device.  For now, the function simply fills in a fixed value.
;       In the future, this function may become responsible for recording
;       master latency information, to allow the best master latency value
;       to be determined dynamically.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to write master latency register fails.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignLatency, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignLatency: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        mov     cl, CONFIG_PCI_LATENCY
        PciFunc WRITE_CONFIG_BYTE       ; initialize latency timer register.
EndProc PciAssignLatency

;***   PciAssignCapList - Handle Capability List Assignment.
;
;   FUNCTIONAL DESCRIPTION.
;       This function is designed to parse the capability list, if needed.
;       The functionality is reserved for a future implementation.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to walk capability list fails.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignCapList, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignCapList: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        clc                             ; always return success.
EndProc PciAssignCapList

;***   PciAssignInt - Handle PCI Interrupt Assignment.
;
;   FUNCTIONAL DESCRIPTION.
;       This function determines if a device has an interrupt requirement,
;       and handles the allocation or assignment of the interrupt as needed.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to assign interrupt failed.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignInt, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignInt: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        PUSHALL
        Pcall   PciFindIrq              ; find the requirements for IRQ.
        POPALL
        clc                             ; return success.
EndProc PciAssignInt

;***   PciCheckBarCommand - Check BAR and Update Command Register.
;
;   FUNCTIONAL DESCRIPTION.
;       This function checks a BAR register and updates a command register
;       value, depending on whether or not the BAR has a valid final setting.
;       Also note that this function can be used to detect if a BAR is
;       actually implemented.  This is used for the bridge decode functions,
;       that determine whether or not to check internal range decodes based
;       on whether the BARs are implemented.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/02/01.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       ECX     - proposed PCI command register value.
;       DI      - PCI index for BAR to be checked.
;       DH      - set depending on BAR types implemented so far.
;
;   EXIT.
;       ECX     - may be changed if BAR is invalid.
;       DH      - BIT0 set if I/O found, BIT1 set if memory found.
;       DL      - type bits from PCI BAR.
;       CY      - set if attempt to detect BAR type failed.
;
;   USES.
;       DL, Flags.

DefProc PciCheckBarCommand, PUBLIC
        push    ecx
        PciFunc READ_CONFIG_DWORD       ; read current BAR value.
        jc      PciCheckBarCommand_Exit
        mov     dl, cl                  ; (DL) = BAR type.
        and     cl, 0f0h                ; clear type bits from BAR check.
        test    ecx, ecx                ; check for assignment.
        jnz     PciCheckBarCommand_ValidBar ; exit with success, mark type.
        test    dl, 0fh                 ; check BAR type.
        jz      PciCheckBarCommand_CheckMmio32 ; check 32 bit MMIO.

;       Depending on BAR type, clear appropriate command enable bit.

PciCheckBarCommand_ClearOnType:
        test    dl, PCI_CONFIG_BAR_IO   ; is this an I/O BAR?
        jz      PciCheckBarCommand_MemType ; handle as memory BAR.

;       Clear IO command enable bit.

PciCheckBarCommand_ClearIo:
        pop     ecx
        and     cl, (NOT BIT0)          ; clear I/O decode enable.
        push    ecx
        jmp     PciCheckBarCommand_ValidBar ; exit with success.

;       Check for 64 bit memory decode.

PciCheckBarCommand_MemType:
        test    dl, PCI_CONFIG_BAR_TYPE_64BIT ; is this BAR 64 bit?
        jnz     PciCheckBarCommand_Mem64Type ; handle as 64 bit type.

;       Clear memory command enable bit.

PciCheckBarCommand_ClearMem:
        pop     ecx
        and     cl, (NOT BIT1)          ; clear memory decode enable.
        push    ecx
        jmp     PciCheckBarCommand_ValidBar ; exit with success.

;       Double check high 32 bits of command register.

PciCheckBarCommand_Mem64Type:
        add     di, 4                   ; check high dword.
        PciFunc READ_CONFIG_DWORD       ; read high assignment.
        jc      PciCheckBarCommand_Exit
        sub     di, 4                   ; restore original BAR addr.
        test    ecx, ecx                ; check for assignment.
        jz      PciCheckBarCommand_ClearMem ; clear mem bit.
        jmp     PciCheckBarCommand_ValidBar ; exit with success.

;       32 bit MMIO BARs are a special case as there is no obvious
;       difference between a zero assignment and an unimplemented BAR.

PciCheckBarCommand_CheckMmio32:
        mov     ecx, 0fffffff0h         ; write test decoder implementation.
        PciFunc WRITE_CONFIG_DWORD      ; write all ones for test.
        jc      PciCheckBarCommand_Exit
        PciFunc READ_CONFIG_DWORD       ; read value back.
        jc      PciCheckBarCommand_Exit
        push    ecx                     ; save sticky bits.
        sub     ecx, ecx                ; clear back to 0.
        PciFunc WRITE_CONFIG_DWORD      ; write 0 back to register.
        pop     ecx                     ; restore sticky bits.
        jc      PciCheckBarCommand_Exit
        test    ecx, ecx                ; did any bits stick in register?
        jnz     PciCheckBarCommand_ClearMem ; clear memory enable if so.
        jmp     PciCheckBarCommand_Success

;       BAR definitely found, set proper bit in DH based on type.

PciCheckBarCommand_ValidBar:
        test    dl, PCI_CONFIG_BAR_IO   ; is this an I/O BAR?
        jz      PciCheckBarCommand_ValidMem
        or      dh, BIT0                ; valid I/O BAR found.
        jmp     PciCheckBarCommand_Success

PciCheckBarCommand_ValidMem:
        or      dh, BIT1                ; valid memory BAR found.

;       Exit with success.

PciCheckBarCommand_Success:
        clc                             ; indicate success.

;       Check complete on current BAR.

PciCheckBarCommand_Exit:
        pop     ecx
EndProc PciCheckBarCommand

;***   PciAssignCommand - Handle PCI Command Register Assignment.
;
;   FUNCTIONAL DESCRIPTION.
;       This function handles setting the command register for a specific
;       PCI device, depending on the device type.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;       K. C. Taylor    06/02/01.       #5.3.4, check BARs and only enable valid types.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to set command register failed.
;
;   USES.
;       EAX, ECX, EDX, Flags.

DefProc PciAssignCommand, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignCommand: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    di                      ; save command/status index.
        mov     di, PCI_CONFIG_SUB_CLASS ; get class and sub-class.
        PciFunc READ_CONFIG_WORD
        pop     di                      ; restore command/status index.
        jc      PciAssignCommand_Exit
        cmp     cx, 0300h               ; is this a VGA controller?
        clc                             ; indicate success in skipping.
        je      PciAssignCommand_Exit   ; exit if it is.

        PciFunc READ_CONFIG_DWORD
        jc      PciAssignCommand_Exit
        or      ecx, 0ffff0007h         ; clr status. I/O, Mem, master enabled.

;       Read and check each BAR to be sure resources were assigned properly.
;       Clear enable bits for which no assignment has been made.

        push    di                      ; save command register index.
        mov     di, PCI_CONFIG_BAR_0    ; start with BAR 0.

PciAssignCommand_BarLoop:
        Pcall   PciCheckBarCommand      ; check BAR and update (ECX).
        jc      PciAssignCommand_FinalCheck ; skip all on failure.
        add     di, 4                   ; next BAR.
        test    dl, PCI_CONFIG_BAR_IO   ; is this an I/O BAR?
        jnz     PciAssignCommand_CheckLastBar ; check for last BAR.
        test    dl, PCI_CONFIG_BAR_TYPE_64BIT ; is this a 64 bit BAR?
        jz      PciAssignCommand_CheckLastBar ; check for last BAR.
        add     di, 4                   ; add extra decode.

;       Check to see if all BARs have been checked.

PciAssignCommand_CheckLastBar:
        cmp     di, PCI_CONFIG_BAR_5    ; limit is BAR 5.
        jbe     PciAssignCommand_BarLoop ; continue until last BAR.
        clc                             ; indicate success.
PciAssignCommand_FinalCheck:
        pop     di                      ; restore command register index.
        jc      PciAssignCommand_Exit
        PciFunc WRITE_CONFIG_DWORD      ; write final command register to device.

;       Exit from command update.

PciAssignCommand_Exit:
EndProc PciAssignCommand

;***   PciAssignBusIo - Assign Bus I/O Ranges.
;
;   FUNCTIONAL DESCRIPTION.
;       This function assigns all the I/O resources to devices and bridges
;       on the secondary bus exposed by the specified bridge device.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to assign interrupt failed.
;
;   USES.
;       EAX, ECX, EDX, DI, Flags.

DefProc PciAssignBusIo, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusIo: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

;       Select new allocation structure.

        Pcall   PciAssignNewAlloc       ; get a new assignment allocation structure.
        jc      PciAssignBusIo_Exit     ; exit with failure if out of space.

        push    bx
        push    bp
        push    si
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, PCIHDR_FLAG_IO      ; do only I/O allocation.
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     si
        pop     bp
        pop     bx
        Pcall   PciGetAllocStructure    ; DS:DI = allocation structure.

;       Determine if a range has been assigned and handle if it has.

        mov     cx, ds:pcialloc_topio.[di] ; get top of I/O region.
        mov     dx, ds:pcialloc_botio.[di] ; get bottom of I/O region.
        cmp     cx, dx                  ; does top I/O differ from bottom?
        ja      PciAssignBusIo_ApplyIoMasks ; skip no I/O handler.

;       No I/O assigned to bridge, handle appropriately.

PciAssignBusIo_NoIoAssigned:
        Pcall   PciCheckForCardBus
        jnc     PciAssignBusIo_CheckBridge

;       If CardBus bridge, assume minimum I/O requrement.

        IF      (CONFIG_PCI_CARDBUS_MIN_IO NE 0)
        sub     dx, CONFIG_PCI_CARDBUS_MIN_IO
        jmp     PciAssignBusIo_ApplyIoMasks ; apply mask normally.
        ELSE    ; (CONFIG_PCI_CARDBUS_MIN_IO NE 0)
        sub     cx, cx                  ; top is 0.
        mov     dx, 0ffffh              ; bottom is all the way up.
        jmp     PciAssignBusIo_WriteToBridge
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_IO NE 0)

PciAssignBusIo_CheckBridge:
        IF      (CONFIG_PCI_PCIBRIDGE_MIN_IO NE 0)
        sub     dx, CONFIG_PCI_PCIBRIDGE_MIN_IO
        jmp     PciAssignBusIo_ApplyIoMasks ; apply mask normally.
        ELSE    ; (CONFIG_PCI_PCIBRIDGE_MIN_IO NE 0)
        sub     cx, cx                  ; top is 0.
        mov     dx, 0ffffh              ; bottom is all the way up.
        jmp     PciAssignBusIo_WriteToBridge
        ENDIF   ; (CONFIG_PCI_PCIBRIDGE_MIN_IO NE 0)

;       Apply range size masks to I/O ranges.

PciAssignBusIo_ApplyIoMasks:
        or      cx, 00fffh              ; lower 12 bits of top always 1.
        mov     ds:pcialloc_topio.[di], cx ; make sure this is reflected.
        and     dx, 0f000h              ; lower 12 bits of bottom always 0.
        cmp     dx, CONFIG_PCI_IO_BASE_16 ; double check bottom of range.
        jae     PciAssignBusIo_AssignBottom
        mov     dx, CONFIG_PCI_IO_BASE_16 ; avoid overrun on range.

PciAssignBusIo_AssignBottom:
        mov     ds:pcialloc_botio.[di], dx ; make sure this is reflected.

;       Write defined range to bridge registers.

PciAssignBusIo_WriteToBridge:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusIo: Assigning IO Range $x to $x.\n>, <dx, cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        Pcall   PciCheckForCardBus      ; handle CardBus bridges with special code.
        jc      PciAssignBusIo_WriteToCardBus
        mov     cl, dh                  ; (CL) = I/O base.
        mov     di, PCI_CONFIG_IO_BASE  ; set I/O base.
        PciFunc WRITE_CONFIG_BYTE       ; write to configuration byte.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        PciFunc READ_CONFIG_BYTE        ; read from configuration byte.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        test    cl, PCI_CONFIG_IO_BASE_32BIT ; is I/O base 32 bit base?
        jz      PciAssignBusIo_WriteIoLimit
        push    cx                      ; save limit.
        sub     cx, cx                  ; top 16 bits should be 0.
        mov     di, PCI_CONFIG_TOP_IO_BASE ; register for top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write top of I/O base.
        pop     cx                      ; restore limit.
        jc      PciAssignBusIo_Exit     ; exit on failure.

;       Write I/O range limit to bridge register.

PciAssignBusIo_WriteIoLimit:
        mov     cl, ch                  ; (CL) = I/O limit.
        mov     di, PCI_CONFIG_IO_LIMIT ; set I/O limit.
        PciFunc WRITE_CONFIG_BYTE       ; write to configuration byte.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        PciFunc READ_CONFIG_BYTE        ; read I/O limit.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        test    cl, PCI_CONFIG_IO_LIMIT_32BIT ; is I/O limit 32 bits?
        jz      PciAssignBusIo_Cleanup  ; go perform final cleanup.
        sub     cx, cx                  ; clear top 16 bits to 0.
        mov     di, PCI_CONFIG_TOP_IO_LIMIT ; write to top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write top of I/O limit.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        jmp     PciAssignBusIo_Cleanup  ; handle cleanup.

;       Write I/O range base and limit to CardBus bridge.

PciAssignBusIo_WriteToCardBus:
        movzx   ecx, cx                 ; clear high bits of limit.
        push    di
        mov     di, PCI_CONFIG_CARDBUS_IO0_LIMIT 
        PciFunc WRITE_CONFIG_DWORD      ; write to CardBus bridge.
        pop     di
        jc      PciAssignBusIo_Exit     ; exit on failure.
        movzx   ecx, dx                 ; (ECX) = range base.
        push    di
        mov     di, PCI_CONFIG_CARDBUS_IO0_BASE
        PciFunc WRITE_CONFIG_DWORD      ; write to CardBus bridge.
        pop     di
        jc      PciAssignBusIo_Exit     ; exit on failure.

        IF      (CONFIG_PCI_CARDBUS_MIN_IO NE 0)
        mov     dx, cx                  ; new base starts at prior base.
        dec     cx                      ; new base is 1 < prior regions base.
        sub     dx, CONFIG_PCI_CARDBUS_MIN_IO
        or      cx, 00fffh              ; lower 12 bits of top always 1.
        and     dx, 0f000h              ; lower 12 bits of bottom always 0.
        cmp     dx, CONFIG_PCI_IO_BASE_16 ; double check bottom of range.
        jae     PciAssignBusIo_CardBusAssignBottom
        mov     dx, CONFIG_PCI_IO_BASE_16 ; avoid overrun on range.

PciAssignBusIo_CardBusAssignBottom:
        mov     ds:pcialloc_botio.[di], dx ; make sure this is reflected.
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusIo: Assigning IO Range $x to $x.\n>, <dx, cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        ELSE    ; (CONFIG_PCI_CARDBUS_MIN_IO NE 0)
        sub     ecx, ecx                ; clear I/O limit 1 to 0.
        sub     dx, dx                  ; (DX) = 0.
        dec     dx                      ; I/O base 1 = 00000ffffh.
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_IO NE 0)

        mov     di, PCI_CONFIG_CARDBUS_IO1_LIMIT ; prepare to write I/O 0 limit.
        PciFunc WRITE_CONFIG_DWORD      ; write to CardBus bridge.
        jc      PciAssignBusIo_Exit     ; exit on failure.
        mov     cx, dx                  ; write new limit.
        mov     di, PCI_CONFIG_CARDBUS_IO1_BASE
        PciFunc WRITE_CONFIG_DWORD      ; write to CardBus bridge.
        jc      PciAssignBusIo_Exit     ; exit on failure.

PciAssignBusIo_Cleanup:
        Pcall   PciAssignDealloc        ; finished with bus allocation.
        clc                             ; indicate successful assignment.

PciAssignBusIo_Exit:
EndProc PciAssignBusIo

;***   PciAssignBusRanges - Assign Ranges to Devices on Bus.
;
;   FUNCTIONAL DESCRIPTION.
;       This function sets memory mapped I/O and prefetchable ranges for
;       both 32 bit and 64 bit bridge ranges (as supported) on the secondary
;       bus exposed by the specified bridge device.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to assign interrupt failed.
;
;   USES.
;       EAX, ECX, EDX, DI, Flags.

DefProc PciAssignBusRanges, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    si                      ; preserve flags.

;       Determine allocation steps required for specified bridge.

        test    si, PCIHDR_FLAG_MEM_COMBINE_64_32 ; are we combining 64 and 32 bit modes?
        jnz     PciAssignBusRanges_CombinedRegions ; handle combined regions.

;       Check for CardBus bridges, as they do not support 64 bit regions.

        Pcall   PciCheckForCardBus      ; check for CardBus bridge.
        jc      PciAssignBusRanges_CombinedRegions ; go to combined mode.

;       Discover if current bridge supports 64 bit prefetchable regions.

        mov     di, PCI_CONFIG_PREFETCH_MEMORY_BASE ; read prefetch base register.
        PciFunc READ_CONFIG_WORD        ; read configuration for prefetch base.
        test    cx, PCI_CONFIG_PREFETCH_MEMORY_64BIT ; check for 64 bit decodes.
        jz      PciAssignBusRanges_CombinedRegions ; go to fully combined mode.

;       Combine 32 bit memory and 32 bit prefetchable memory regions and
;       try to allocate 64 bit prefetchable memory regions separately.

        test    si, (PCIHDR_FLAG_32BIT OR PCIHDR_FLAG_32BIT_PREFETCH OR PCIHDR_FLAG_OPTROM)
        jz      PciAssignBusRanges_64BitPrefetch

        Pcall   PciAssignNewAlloc       ; get a new assignment allocation structure.
        jc      PciAssignBusRanges_Exit ; exit with failure if out of space.
        push    bx
        push    bp
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, (PCIHDR_FLAG_32BIT OR PCIHDR_FLAG_32BIT_PREFETCH OR \
                     PCIHDR_FLAG_64BIT OR PCIHDR_FLAG_MEM_COMBINE_64_32)
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     bp
        pop     bx
        pop     si                      ; restore (SI).
        push    si
        call    PciAssignBusRanges_AssignMemBase
        Pcall   PciAssignDealloc        ; finished with bus allocation.

;       Handle 64 bit prefetch region assignments as solitary assignments.

PciAssignBusRanges_64BitPrefetch:
        test    si, (PCIHDR_FLAG_64BIT OR PCIHDR_FLAG_64BIT_PREFETCH)
        clc                             ; return success.
        jz      PciAssignBusRanges_Exit
        Pcall   PciAssignNewAlloc       ; get a new assignment allocation structure.
        jc      PciAssignBusRanges_Exit ; exit with failure if out of space.
        push    bx
        push    bp
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, PCIHDR_FLAG_64BIT_PREFETCH
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     bp
        pop     bx
        Pcall   PciGetAllocStructure    ; DS:DI = allocation structure.

;       Verify that assignment is within allowed range.

        mov     ecx, pcialloc_bot64low.[di]
        mov     edx, pcialloc_bot64high.[di]
        cmp     edx, CONFIG_PCI_MEM_BASE_64_HI
        ja      PciAssignBusRange_64BitHighSet
        jb      PciAssignBusRange_64BitResetBot
        cmp     ecx, CONFIG_PCI_MEM_BASE_64_LO
        jae     PciAssignBusRange_64BitHighSet

;       64 bit range limit exceeded, reset range bottom variable.

PciAssignBusRange_64BitResetBot:
        mov     ecx, CONFIG_PCI_MEM_BASE_64_LO
        mov     edx, CONFIG_PCI_MEM_BASE_64_HI
        mov     pcialloc_bot64low.[di], ecx
        mov     pcialloc_bot64high.[di], edx

PciAssignBusRange_64BitHighSet:
        mov     ecx, ds:pcialloc_top64high.[di] ; get high 32 bits.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: High part of top of 64bit range=$lx.\n>, <ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    di
        mov     di, PCI_CONFIG_TOP_PREFETCH_LIMIT ; set high 32 bits.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        pop     di
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     ecx, ds:pcialloc_bot64high.[di] ; get high 32 bits.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: High part of bottom of 64bit range=$lx.\n>, <ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    di
        mov     di, PCI_CONFIG_TOP_PREFETCH_BASE ; set high 32 bits.
        PciFunc WRITE_CONFIG_DWORD      ; write to configuration register.
        pop     di
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     ecx, ds:pcialloc_top64low.[di] ; get low 32 bits limit.
        mov     edx, ds:pcialloc_bot64low.[di] ; get low 32 bits base.
        or      ecx, 0000fffffh         ; low 20 bits limit are always 1.
        mov     ds:pcialloc_top64low.[di], ecx ; set low 32 bits.
        and     edx, 0fff00000h         ; low 20 bits base are always 0.
        mov     ds:pcialloc_bot64low.[di], edx ; set low 32 bits.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: 64bit low part=$lx to $lx.\n>, <edx, ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        mov     di, PCI_CONFIG_PREFETCH_MEMORY_LIMIT
        shr     ecx, 16                 ; write only top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write to register.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     ecx, edx                ; (ECX) = base value.
        shr     ecx, 16                 ; write only top 16 bits.
        mov     di, PCI_CONFIG_PREFETCH_MEMORY_BASE
        PciFunc WRITE_CONFIG_WORD       ; write to register.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        Pcall   PciAssignDealloc        ; finished with bus allocation.
        clc                             ; return success.
        jmp     PciAssignBusRanges_Exit ; exit with success.

;       32 and 64 bit memory range assignments are combined, possibly
;       including prefetchable, although that may be handled independently.

PciAssignBusRanges_CombinedRegions:
        test    si, (PCIHDR_FLAG_32BIT OR PCIHDR_FLAG_64BIT OR PCIHDR_FLAG_OPTROM) ; MMIO range check.
        jz      PciAssignBusRanges_32BitPrefetch ; handle 32 bit prefetch.
        Pcall   PciAssignNewAlloc       ; get a new assignment allocation structure.
        jc      PciAssignBusRanges_Exit ; exit with failure if out of space.
        push    bx
        push    bp
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, (PCIHDR_FLAG_32BIT OR PCIHDR_FLAG_64BIT OR \
                     PCIHDR_FLAG_OPTROM OR PCIHDR_FLAG_MEM_COMBINE_64_32)
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     bp
        pop     bx
        pop     si                      ; restore (SI).
        push    si
        call    PciAssignBusRanges_AssignMemBase
        Pcall   PciAssignDealloc        ; finished with bus allocation.
        jmp     PciAssignBusRanges_32BitPrefetch

;       Handle common portion of assigning 32 bit memory base.

PciAssignBusRanges_AssignMemBase:
        Pcall   PciGetAllocStructure    ; (DS:DI) = allocation structure.
        mov     ecx, ds:pcialloc_top32.[di] ; get high 32 bits.
        mov     edx, ds:pcialloc_bot32.[di] ; get low 32 bits.
        cmp     ecx, edx                ; check for range assignment.
        ja      PciAssignBusRange_ApplyMemMask

;       Memory base was not adjusted.  Handle based on the header type.

PciAssignBusRanges_NoMemBase:
        Pcall   PciCheckForCardBus      ; check for CardBus bridge.
        jnc     PciAssignBusRanges_MemCheckBridge ; assign CardBus differently.

;       If this is a CardBus bridge and a minimum memory allocation should
;       be made, make the minimum allocation here.

        IF      (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
        sub     edx, CONFIG_PCI_CARDBUS_MIN_MEM ; allocate minimum amount.
        jmp     PciAssignBusRange_ApplyMemMask
        ELSE    ; (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
        sub     ecx, ecx                ; clear limit to 0.
        sub     edx, edx                ; clear base to 0.
        dec     edx                     ; base becomes 0ffffffffh.
        jmp     PciAssignBusRange_AssignMemResource ; write to registers.
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
PciAssignBusRanges_MemCheckBridge:
        IF      (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)
        sub     edx, CONFIG_PCI_PCIBRIDGE_MIN_MEM ; allocate minimum amount.
        jmp     PciAssignBusRange_ApplyMemMask
        ELSE    ; (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)
        sub     ecx, ecx                ; clear limit to 0.
        sub     edx, edx                ; clear base to 0.
        dec     edx                     ; base becomes 0ffffffffh. 
        jmp     PciAssignBusRange_AssignMemResource ; write to registers.
        ENDIF   ; (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)

;       Apply memory masks and configure ranges.

PciAssignBusRange_ApplyMemMask:
        or      ecx, 0000fffffh         ; low 20 bits of limit are always 1.
        mov     ds:pcialloc_top32.[di], ecx ; store updated range top.
        and     edx, 0fff00000h         ; low 20 bits of base are always 0.
        cmp     edx, CONFIG_PCI_MEM_BASE_32 ; check against base address.
        jae     PciAssignBusRange_ApplyFinalMemMask
        mov     edx, CONFIG_PCI_MEM_BASE_32 ; check against base address.
PciAssignBusRange_ApplyFinalMemMask:
        mov     ds:pcialloc_bot32.[di], edx ; store updated range bottom.

PciAssignBusRange_AssignMemResource:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: Memory=$lx to $lx.\n>, <edx, ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        Pcall   PciCheckForCardBus      ; check for CardBus bridge.
        jc      PciAssignBusRange_MemWriteCardBus ; assign CardBus differently.
        mov     di, PCI_CONFIG_MEMORY_LIMIT ; write limit first.
        shr     ecx, 16                 ; write only top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write limit register.
        mov     ecx, edx                ; (ECX) = base value.
        mov     di, PCI_CONFIG_MEMORY_BASE ; write base second.
        shr     ecx, 16                 ; write only top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write limit register.
        ret                             ; return from common code.

;       Write first the limit and then the base for memory range 0 on a
;       CardBus bridge.

PciAssignBusRange_MemWriteCardBus:
        mov     di, PCI_CONFIG_CARDBUS_MEM0_LIMIT
        PciFunc WRITE_CONFIG_DWORD      ; write to bridge.
        mov     ecx, edx                ; (ECX) = base to write to device.
        mov     di, PCI_CONFIG_CARDBUS_MEM0_BASE
        PciFunc WRITE_CONFIG_DWORD      ; write to bridge.
        ret                             ; return from common code.

;       Handle 32 bit and 64 bit combined prefetchable assignments.

PciAssignBusRanges_32BitPrefetch:
        test    si, (PCIHDR_FLAG_32BIT_PREFETCH OR PCIHDR_FLAG_64BIT_PREFETCH)
        clc                             ; indicate success.
        jz      PciAssignBusRanges_Exit ; exit with success.
        Pcall   PciAssignNewAlloc       ; get a new assignment allocation structure.
        jc      PciAssignBusRanges_Exit ; exit with failure if out of space.
        push    bx
        push    bp
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        mov     si, (PCIHDR_FLAG_32BIT_PREFETCH OR \
                     PCIHDR_FLAG_64BIT_PREFETCH OR \
                     PCIHDR_FLAG_MEM_COMBINE_64_32)
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     bp
        pop     bx

;       Assign final prefetchable region.

        Pcall   PciGetAllocStructure    ; DS:DI = allocation structure.
        mov     ecx, ds:pcialloc_top32.[di] ; get high 32 bits.
        mov     edx, ds:pcialloc_bot32.[di] ; get low 32 bits.
        cmp     ecx, edx                ; check for range assignment.
        ja      PciAssignBusRange_ApplyPrefetchMask

;       Handle range defaults if no resources allocated.

        Pcall   PciCheckForCardBus      ; check for CardBus bridge.
        jnc     PciAssignBusRanges_PrefetchCheckBridge ; assign CardBus differently.

;       If this is a CardBus bridge and a minimum memory allocation should
;       be made, make the minimum allocation here.

        IF      (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
        sub     edx, CONFIG_PCI_CARDBUS_MIN_MEM ; allocate minimum amount.
        jmp     PciAssignBusRange_ApplyPrefetchMask
        ELSE    ; (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
        sub     ecx, ecx                ; clear limit to 0.
        sub     edx, edx                ; clear base to 0.
        dec     edx                     ; base becomes 0ffffffffh.
        jmp     PciAssignBusRange_WritePrefetchRange ; write to registers.
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_MEM NE 0)
PciAssignBusRanges_PrefetchCheckBridge:
        IF      (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)
        sub     edx, CONFIG_PCI_PCIBRIDGE_MIN_MEM ; allocate minimum amount.
        jmp     PciAssignBusRange_ApplyPrefetchMask
        ELSE    ; (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)
        sub     ecx, ecx                ; clear limit to 0.
        sub     edx, edx                ; clear base to 0.
        dec     edx                     ; base becomes 0ffffffffh. 
        jmp     PciAssignBusRange_WritePrefetchRange ; write to registers.
        ENDIF   ; (CONFIG_PCI_PCIBRIDGE_MIN_MEM NE 0)

;       Apply appropriate masks and range checks.

PciAssignBusRange_ApplyPrefetchMask:
        or      ecx, 0000fffffh         ; low 20 bits of limit are always 1.
        mov     ds:pcialloc_top32.[di], ecx ; store updated range top.
        and     edx, 0fff00000h         ; low 20 bits of base are always 0.
        cmp     edx, CONFIG_PCI_MEM_BASE_32 ; check against base of range.
        jae     PciAssignBusRange_ApplyFinalPrefetchMask
        mov     edx, CONFIG_PCI_MEM_BASE_32 ; prevent range overrun.

;       Store final range base.

PciAssignBusRange_ApplyFinalPrefetchMask:
        mov     ds:pcialloc_bot32.[di], edx ; store updated range bottom.

PciAssignBusRange_WritePrefetchRange:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusRanges: Prefetch=$lx to $lx.\n>, <edx, ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        Pcall   PciCheckForCardBus      ; check for CardBus bridges.
        jc      PciAssignBusRange_PrefetchWriteCardBus ; assign CardBus differently.
        mov     di, PCI_CONFIG_PREFETCH_MEMORY_LIMIT ; write limit first.
        shr     ecx, 16                 ; write only top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write limit register.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     ecx, edx                ; (ECX) = base value.
        mov     di, PCI_CONFIG_PREFETCH_MEMORY_BASE ; write base second.
        shr     ecx, 16                 ; write only top 16 bits.
        PciFunc WRITE_CONFIG_WORD       ; write limit register.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        sub     ecx, ecx                ; clear top 32 bits of 64 bit ranges.
        mov     di, PCI_CONFIG_TOP_PREFETCH_LIMIT ; set limit first.
        PciFunc WRITE_CONFIG_DWORD      ; write all 32 bits.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     di, PCI_CONFIG_TOP_PREFETCH_BASE ; set base second.
        PciFunc WRITE_CONFIG_DWORD      ; write all 32 bits.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        Pcall   PciAssignDealloc        ; finished with bus allocation.
        jmp     PciAssignBusRanges_Exit ; exit from handler.

;       Write prefetch range for CardBus bridges.

PciAssignBusRange_PrefetchWriteCardBus:
        mov     di, PCI_CONFIG_CARDBUS_MEM1_LIMIT
        PciFunc WRITE_CONFIG_DWORD      ; write CardBus prefetch range limit.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        mov     ecx, edx                ; (ECX) = CardBus prefetch base.
        mov     di, PCI_CONFIG_CARDBUS_MEM1_BASE
        PciFunc WRITE_CONFIG_DWORD      ; write CardBus prefetch range limit.
        jc      PciAssignBusRanges_Exit ; exit on failure.
        Pcall   PciAssignDealloc        ; finished with bus allocation.

PciAssignBusRanges_Exit:
        pop     si
EndProc PciAssignBusRanges

;***   PciAssignBusCommand - Write to Bridge Command Registers.
;
;   FUNCTIONAL DESCRIPTION.
;       This function writes the final enable value to each bridge in turn,
;       and recursively calls PciAssignResources to do the same.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/11.       #5.3.4, New.
;       K. C. Taylor    06/02/01.       #5.3.4, only enable for valid BARs and ranges.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BX      - PCI bus/device/function number of device.
;       SI      - current enumeration functionality mask.
;       DI      - PCI configuration index for desired configuration.
;       CS:BP   - current offset in device header description table.
;
;   EXIT.
;       CY      - set if attempt to assign interrupt failed.
;
;   USES.
;       EAX, ECX, EDX, DI, Flags.

DefProc PciAssignBusCommand, PUBLIC
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: Device=$x.\n>, <bx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        push    esi
        push    di
        push    bx
        push    bp
        mov     di, PCI_CONFIG_BUS_SECONDARY ; get bus number assigned.
        PciFunc READ_CONFIG_BYTE        ; read secondary bus number.
        mov     bh, cl                  ; set bus number.
        and     si, (PCIHDR_FLAG_MISC AND (NOT PCIHDR_FLAG_BIST)) ; BIST is separate.
        Pcall   PciAssignResources      ; recurse into PCI assign resources.
        pop     bp
        pop     bx
        pop     di
        PciFunc READ_CONFIG_DWORD
        or      ecx, 0ffff0007h         ; Clr status. I/O, Mem, master enabled.

;       Always enable the full command register for CardBus bridges.

        Pcall   PciCheckForCardBus      ; check for CardBus bridges.
        jnc     PciAssignBusCommand_VerifyBars ; only verify BARs if not CardBus bridge.
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: Final write $x, $lx\n>, <di, ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        PciFunc WRITE_CONFIG_DWORD      ; write back to CardBus bridge control.
        jmp     PciAssignBusCommand_Exit; exit with proper status.

;       Verify that local bridge BARs are correct, then verify that decode
;       ranges are required prior to programming the command register.

PciAssignBusCommand_VerifyBars:
        push    di
        mov     di, PCI_CONFIG_BAR_0    ; BAR 0 is allowed.
        sub     dh, dh                  ; keep track of implemented BAR types.

PciAssignBusCommand_BarLoop:
        Pcall   PciCheckBarCommand      ; check BAR and update (ECX).
        jc      PciAssignBusCommand_FinalCheck ; skip all on failure.
        add     di, 4                   ; next BAR.
        test    dl, PCI_CONFIG_BAR_IO   ; is this an I/O BAR?
        jnz     PciAssignBusCommand_CheckLastBar ; check for last BAR.
        test    dl, PCI_CONFIG_BAR_TYPE_64BIT ; is this a 64 bit BAR?
        jz      PciAssignBusCommand_CheckLastBar ; check for last BAR.
        add     di, 4                   ; add extra decode.

;       Check to see if all BARs have been checked.

PciAssignBusCommand_CheckLastBar:
        cmp     di, PCI_CONFIG_BAR_1    ; limit is BAR 1.
        jbe     PciAssignBusCommand_BarLoop ; continue until last BAR.
        clc                             ; indicate success.
PciAssignBusCommand_FinalCheck:
        pop     di                      ; restore command register index.
        jc      PciAssignBusCommand_Exit

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: Device $x after BAR: $lx, $x\n>, <bx, ecx, dx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    di
        push    ecx
        test    dh, BIT0                ; was the I/O BAR implemented?
        jnz     PciAssignBusCommand_CheckMemory ; check memory BARs if it was.

;       No I/O BAR was implemented, check the range register.

        mov     di, PCI_CONFIG_IO_BASE
        PciFunc READ_CONFIG_WORD        ; read base and limit at once.
        test    cx, cx                  ; is the range completely 0?
        jz      PciAssignBusCommand_DisableIo ; disable I/O if it is.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: IO range $x\n>, <cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        cmp     cl, ch                  ; verify range.
        jbe     PciAssignBusCommand_CheckMemory ; check memory BARs if okay.

;       No I/O range was implemented or invalid range was assigned.

PciAssignBusCommand_DisableIo:
        pop     ecx                     ; restore command register.
        and     cl, (NOT BIT0)          ; disable range.
        push    ecx                     ; restore command register.

;       Check the memory region to see if it is valid.  Note that if the
;       memory range is valid, there is no need to check prefetchable ranges.

PciAssignBusCommand_CheckMemory:
        test    dh, BIT1                ; was a memory BAR found?
        jnz     PciAssignBusCommand_CommandFound ; found final command value.
        mov     di, PCI_CONFIG_MEMORY_BASE ; read memory base.
        PciFunc READ_CONFIG_WORD        ; read the memory base.
        mov     si, cx                  ; save base in (SI).
        mov     di, PCI_CONFIG_MEMORY_LIMIT ; read memory limit.
        PciFunc READ_CONFIG_WORD        ; read the limit.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: memory $x $x\n>, <si, cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        cmp     si, cx                  ; verify that limit is programmed.
        jbe     PciAssignBusCommand_CommandFound

;       Check the pre-fetchable region to see if it is valid.

PciAssignBusCommand_CheckPrefetch:
        mov     di, PCI_CONFIG_TOP_PREFETCH_BASE
        PciFunc READ_CONFIG_DWORD       ; read top 32 of prefetchable range.
        mov     esi, ecx                ; (ESI) = top 32 of prefetchable range.
        mov     di, PCI_CONFIG_TOP_PREFETCH_LIMIT ; read top 32 of limit.
        PciFunc READ_CONFIG_DWORD       ; read top 32 of prefetchable range.
        cmp     esi, ecx                ; check top of range.
        jb      PciAssignBusCommand_CommandFound ; if top is valid, all 64 valid.
        mov     di, PCI_CONFIG_PREFETCH_MEMORY_BASE ; check low 32 bits.
        PciFunc READ_CONFIG_WORD        ; read top 12 of low 32 bits.
        mov     si, cx                  ; save low 32 bits.
        mov     di, PCI_CONFIG_PREFETCH_MEMORY_LIMIT ; check low 32 bits.
        PciFunc READ_CONFIG_WORD        ; read top 12 of low 32 bits.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: prefetch $x $x\n>, <si, cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        cmp     si, cx                  ; check range.
        jbe     PciAssignBusCommand_CommandFound ; low range is valid, all is valid.

;       Clear the memory enable, since there is no memory request requirement.

        pop     ecx                     ; restore original command value.
        and     cl, (NOT BIT1)          ; clear memory enable bit.
        push    ecx                     ; store original command value.

PciAssignBusCommand_CommandFound:
        pop     ecx                     ; restore final command register.
        pop     di                      ; restore command index.
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignBusCommand: Final write $x, $lx\n>, <di, ecx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
        PciFunc WRITE_CONFIG_DWORD
PciAssignBusCommand_Exit:
        pop     esi
EndProc PciAssignBusCommand

;***   PciAssignResources - Routine to assign resources to all PCI devices.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called from PostPciEnumerate as well as recursively
;       from PciAssignBridgeRange to assign resources to all types of PCI
;       device (including bridges).
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    06/01/10.       #5.3.4, reworked for full resource assingment.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BH      - current PCI bus number.
;       SI      - resource assignment type mask (passed to sub functions).
;
;   EXIT.
;       none.
;
;   USES.
;       all.

DefProc PciAssignResources, GLOBAL, OVERRIDE

;       Start allocation process from device 0, function 0 on current bus.

        sub     bl, bl

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignResources: Starting at $x, flags=$x.\n>, <bx, si>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

;       Assign resources to all devices on current bus.

PciAssignResources_Loop:
        mov     di, PCI_CONFIG_VENDOR_DEVICE_ID ; read vendor/device ID.
        PciFunc READ_CONFIG_DWORD       ; read from current device.
        inc     ecx                     ; non-device = 0ffffffffh.
        jz      PciAssignResources_NextFunc ; possible multi-function on fail.

        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciAssignResources: Device found at $x, flags=$x.\n>, <bx, si>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

;       Find the header type table for the specified PCI device.

        mov     di, PCI_CONFIG_HEADER_TYPE ; read header type.
        PciFunc READ_CONFIG_BYTE        ; read header type from device.
        and     cl, PCI_CONFIG_HEADER_TYPE_MASK
        mov     bp, OFFSET PCI_GRP:PciHeaderDeviceTypeTbl
        cmp     cl, PCI_CONFIG_HEADER_TYPE_DEVICE
        je      PciAssignResources_Assign
        mov     bp, OFFSET PCI_GRP:PciHeaderBridgeTypeTbl
        cmp     cl, PCI_CONFIG_HEADER_TYPE_BRIDGE
        je      PciAssignResources_Assign
        cmp     cl, PCI_CONFIG_HEADER_TYPE_CARDBUS
        jne     PciAssignResources_Next ; type not supported, next device.

;       Handle resource allocation for specified device.

PciAssignResources_Assign:
        mov     ax, PCI_CTL_DEV_TYPE_TABLE
        shl     eax, 16                 ; move to top 16 bits.
        mov     ax, bp                  ; (AX) = proposed table address.
        Gcall   BoardPciControl, POST   ; call board module.
        mov     bp, ax                  ; select new table if table changed.

;       For each supported resource type in the resource table, call the
;       specified resource control function.

PciAssignResources_ResourceLoop:
        test    PCI_GRP:pcihdr_flags.[bp], si ; check support flags.
        jz      PciAssignResources_NextResource ; move to next resource.
        mov     di, word ptr PCI_GRP:pcihdr_index.[bp] ; read config index.
        call    word ptr PCI_GRP:pcihdr_function.[bp] ; make the call.
        jc      PciAssignResources_Exit ; exit on failure.

;       Advance to next resource in resource header definition.

PciAssignResources_NextResource:
        add     bp, (SIZE PCIHDR)       ; advance to next PCI header entry.
        cmp     PCI_GRP:pcihdr_flags.[bp], 0 ; end of table?
        jne     PciAssignResources_ResourceLoop ; continue if not at end.

;       Advance to next device or function on the current bus.

PciAssignResources_Next:
        test    bl, 7                   ; is this a function?
        jnz     PciAssignResources_NextFunc ; if so, enumerate next function.
        mov     di, PCI_CONFIG_HEADER_TYPE ; read header type.
        PciFunc READ_CONFIG_BYTE        ; read from device config space.
        test    cl, PCI_CONFIG_HEADER_TYPE_MULTI ; is it multifunction?
        jnz     PciAssignResources_NextFunc ; if so, enumerate next function.
        add     bl, 7                   ; skip to function boundary.

;       Advance to next function.

PciAssignResources_NextFunc:
        inc     bx                      ; advance bus/device/function.
        test    bl, bl                  ; did we advance to next bus?
        jnz     PciAssignResources_Loop ; continue to assign resources if not.
        test    si, PCIHDR_FLAG_BAR_SET ; check for BAR initialization.
        jz      PciAssignResources_Success ; report success.
        Pcall   PciAssignOpportunistic  ; make opportunistic resource assignments.

PciAssignResources_Success:
        clc                             ; indicate success.

PciAssignResources_Exit:
        IF      OPTION_DIAGNOSE_PCI
        pushf
        jnc     PciAssignResources_NoFailure
        CPRINTF <PciAssignResources: Assignment Failure.\n>
PciAssignResources_NoFailure:
        popf
        ENDIF   ; (OPTION_DIAGNOSE_PCI)
EndProc PciAssignResources

;***   GoProtPci - Switch to 32-bit Protected Mode (PCI Helper).
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module only to switch
;       to 32-bit protected mode with CS mapped to a 16-bit code segment
;       for the BIOS code, and DS mapped to physical address 0 with a 32-bit
;       (4k granular) segment that can address all of memory.
;
;   MODIFICATION HISTORY.
;       S. E. Jones     97/01/28.       #4.0, original.
;       D. K. Gibson    97/07/09.       #4.0, enable A20 using std routine.
;
;   WARNINGS.
;       Exits with interrupts disabled; must stay disabled until GoRealPci returns.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       DS, ES  - 32-bit r/w selector mapped to phys address 00000000h.
;
;   USES.
;       flags.

DefProc GoProtPci, PUBLIC, OVERRIDE
        cli                             ; BEGIN HARD CRITICAL SECTION.
        push    eax
        Gcall   EnableA20, BIOS         ; enable A20.
        lgdt    fword ptr gdt
        mov     eax, cr0
        or      al, 01h                 ; set to enable protected mode.
        mov     cr0, eax                ; enable protected mode.
        DELAY   1                       ; flush instruction pipeline.
        mov     ax, data386-gdt
        mov     ds, ax
        mov     es, ax                  ; load DS/ES with 4K granular selector.
        pop     eax
EndProc GoProtPci

;***   GoRealPci - Switch back to Real Mode (PCI Helper).
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module only to switch
;       back to real mode from the 32-bit protected mode.
;
;   MODIFICATION HISTORY.
;       S. E. Jones     97/01/28.       #4.0, original.
;       D. K. Gibson    97/07/09.       #4.0, disable A20 using std routine.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       DS, ES  - 64KB segment at F000h.
;
;   USES.
;       flags.

DefProc GoRealPci, PUBLIC, OVERRIDE
        push    eax
        mov     ax, data286-gdt
        mov     ds, ax                  ; address just 64K of ROM memory.
        mov     es, ax                  ; address just 64K of ROM memory.
        mov     eax, cr0
        and     al, not 01h             ; set to disable protected mode.
        mov     cr0, eax                ; disable protected mode.
        DELAY   1                       ; flush instruction pipeline.
        Gcall   DisableA20, BIOS        ; disable A20.
        pop     eax
        sti                             ; END HARD CRITICAL SECTION.
EndProc GoRealPci

;***   MapRomPci - Map Option ROM for PCI Device.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module only to
;       map a device's option ROM to a high physical address.
;
;   MODIFICATION HISTORY.
;       S. E. Jones     97/01/28.       #4.0, original.
;          M. Shepos    98/03/29.       #4.1, add integrated device mapping.
;       K. C. Taylor    98/06/02.       #4.1, made mapping more standard.
;       K. C. Taylor    99/02/03.       #4.2, PCI Bridging changes.
;       F. E. Kerr      99/08/30.       #4.21, use WHILE for integrated mapping.
;       M. B. Hammer    05/01/07.       #5.3, test for 0xAA55 ROM signature.
;       K. C. Taylor    06/01/12.       #5.3.4, enable ROM decodes during check.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BH      - bus number.
;       BL      - device number in top 5 bits, bottom 3 bits = function number.
;
;   EXIT.
;      ESI      - physical address for ROM extension mapping.
;       CY      - set if failure (no option ROM), else clear if success.
;
;   USES.
;       flags.

DefProc MapRomPci, GLOBAL, OVERRIDE
        push eax
        push ebx
        push ecx
        push edx

;       Initialize any integrated PCI ROM expansion BIOS defined in the
;       project's PCI_ROM_MAP table (using the PCI_ROM macro).  This is
;       required when the system BIOS device also holds the expansion BIOS ROM
;       for an on-board PCI device, since the ROM must be called with (AX)
;       containing the PCI bus number in (AH), and PCI device in (AL:7-3),
;       and PCI function number in (AL:2-0).

PCI_ROMCNT_COUNT = 0                    ; counter for WHILE loop.
        WHILE   (PCI_ROMCNT_COUNT LE PCI_ROMCNT)

        IRP     index, <%PCI_ROMCNT_COUNT> ;; index = count (force substitution).
        IFDEF   PCI_ROM_MAP&&index

        mov     ax, ((PCI_ROM_BUS&&index SHL 8) OR      \
                     (PCI_ROM_DEV&&index SHL 3) OR      \
                      PCI_ROM_FUN&&index)
        mov     esi, PCI_ROM_MAP&&index
        cmp     ax, bx                  ;; integrated PCI device?
        je      MapRomPci_Exit          ;; if match found, return (NC).

        ENDIF   ;; (PCI_ROM_MAP&&index)
        ENDM    ;; (IRP)

PCI_ROMCNT_COUNT = PCI_ROMCNT_COUNT + 1
        ENDM    ; (WHILE PCI_ROMCNT_COUNT LE PCI_ROMCNT)

;       Attempt to read the option ROM address from the expansion ROM base
;       address register.  If the PCI read fails or if 0 is read, then
;       return with (CY) set, indicating failure--no option ROM.  If a non-zero
;       value is read, clear carry and strip off the enable bit, and return
;       the physical address for ROM extension mapping in (ESI).

        mov     di, PCI_CONFIG_ROM_BAR
        PciFunc READ_CONFIG_DWORD       ; read ROM Ext Base register.
        jc      MapRomPci_Exit          ; if failure.
        or      ecx, ecx                ; is there an option ROM?
        stc                             ; assume not, therefore failure.
        jz      MapRomPci_Exit          ; if no option ROM, return failure.
        or      cl, BIT0                ; enable option ROM decode.
        PciFunc WRITE_CONFIG_DWORD
        and     cl, (NOT BIT0)          ; clear enable bit.

;       Now determine if area mapped is a real expansion ROM.  Do this by
;       looking for the 0xAA55 signature.

        Pcall   GoProtPci               ; (ES) & (DS) are 4K selectors now.
        mov     edx, DWORD PTR ds:[ecx] ; (EDX) = xxxxaa55h.
        Pcall   GoRealPci               ; back to real mode.
        cmp     dx, 0AA55h
        stc                             ; assume failure.
        jne     MapRomPci_Exit          ; fail if signature missing.

        clc                             ; flag success.
        mov     esi, ecx                ; return the ROM mapping.

;       Return with (CY) set if failure, else clear if success.

MapRomPci_Exit:
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
EndProc MapRomPci

;***   ShadowRegionPci - Allocate Shadow Memory for Region.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module only to
;       call the board's shadowing function to map shadow RAM through
;       a series of 512-byte pages so that it can be used to store
;       an option ROM.
;
;   MODIFICATION HISTORY.
;       S. E. Jones     97/01/28.       #4.0, original.
;       S. R. Tembreull 98/05/28.       #4.1, added board shadow enable.
;       K. C. Taylor    99/03/11.       #4.2, takes start offset into account.
;       D. K. Chalfant  02/04/29.       #5.1, fix OPROM shadowing issue.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       DL      - number 512 byte pages to shadow from PCI OPROM header byte 3.
;
;   EXIT.
;       CY      - set if failure, else clear if success.
;
;   USES.
;       flags.

DefProc ShadowRegionPci, PUBLIC, OVERRIDE
        PUSHALL
        USEEXTDATA ds

;       Calculate OPROM ending segment address.

        sub     ax, ax                  ; (AX) = initialize to zero.
        mov     al, dl                  ; (AL) = num 512-byte pages.
        shl     ax, 5                   ; (AX) = convert to paragraph units.
        add     ax, PciRomSeg           ; (AX) = OPROM ending segment addr.

;       Convert to shadow ordinal for the end of the OPROM memory region by
;       rounding down to nearest 16k shadow region boundary below the end of
;       the OPROM image, shifting the least significant digit to bit 0, and
;       masking off all bits higher than bit 3.

        and     ax, (NOT 03FFh)         ; (AX) = round down to 16k shad reg.
        shr     ax, 10                  ; (AX) = shift least sig digit to bit 0.
        and     al, 0Fh                 ; (AL) = ending shad region ordinal.

;       Calculate starting shadow region ordinal.

        mov     dx, PciRomSeg           ; (DX) = OPROM starting segment addr.
        and     dx, (NOT 03FFh)         ; (DX) = round down to 16k shad region.
        shr     dx, 10                  ; (DX) = shift least sig digit to bit 0.
        and     dl, 0Fh                 ; (DL) = starting shad region ordinal.

;       Load lowest shadow region ordinal as seed value for region loop.

        movzx   di, dl                  ; (DI) = shadow ordinal low to high.

;       Calculate number of shadow regions.

        sub     al, dl                  ; (AL) = number 16k regions to shadow.
        inc     al                      ; (AL) = normalize to one-based result.
        movzx   cx, al                  ; (CX) = loop counter.

ShadowRegionPci_Loop:
        Gcall   ShadowRegion, BIOS      ; shadow the region.
        inc     di                      ; (DI) = next shadow region ordinal.
        loop    ShadowRegionPci_Loop    ; shadow next higher region.

;       Write-enable all shadow RAM regions via a call to the chipset module.

        mov     ax, SHADOW_CTL_WRITABLE ; (AX) = write-en all shadow regions.
        Gcall   BoardShadowWriteCtl, POST ; enable writes to shadow RAM.
        POPALL                          ; don't disturb CY flag from call above.
EndProc ShadowRegionPci

;***   PciFindIrq - Works out which IRQ and line a device will require.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module to
;       allocate a system IRQ level and PCI interrupt line to
;       the function as requested by the bus/device/function.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    99/02/03.       #4.2, New for PCI Bridging support.
;       S. E. Jones     99/03/30.       #4.2, add ASSUME ES:NOTHING.
;       F. E. Kerr      99/11/16.       #4.3, add BoardPciControl policy hook.
;       B. J. Brashier  00/05/29.       #4.3, changed barber-pole routine.
;       K. C. Taylor    00/09/13.       #5.0, updated barber-pole and IRQs.
;       K. C. Taylor    00/09/25.       #5.0, fix for default barber pole.
;       S. E. Jones     01/08/14.       #5.0, use POST scratch buffer.
;       B. J. Brashier  02/04/25.       #5.1, fix check for IRQ already assigned.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;       K. C. Taylor    06/02/14.       #5.3.4, predictable results if invalid line.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BH      - bus number.
;       BL      - [7:3] (top 5) = device number, [2:0] (bott 3) = func number.
;
;   EXIT.
;       CY      - set if failure, else clear if success.
;       BX      - preserved, just as entry parameters.
;
;   USES.
;       All but BX.

DefProc PciFindIrq, PUBLIC, OVERRIDE
        push    bx
        USEEXTDATA ds

;       Ask the device/function what interrupt pin it needs (A#,B#,C#,D#).

        mov     di, PCI_CONFIG_INT_REQUEST
        PciFunc READ_CONFIG_BYTE        ; call PCI services.
        jc      PciFindIrq_Fail         ; if failure, skip the initialization.
        or      cl, cl                  ; is any interrupt pin requested?
        jz      PciFindIrq_Fail         ; if none requested, just return AL=0ffh.

;       Scan the PCI function table to see if there is a special IRQ mask or
;       INT line assignment for this device.

        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax                  ; (ES) = IO table data segment.
        ASSUME  ES:NOTHING
        xor     si, si
        mov     di, word ptr es:[si+2]  ; (DI) = table offset.
        mov     ax, word ptr es:[si+4]  ; (AX) = table segment.
        mov     si, word ptr es:[si]    ; (SI) = table length.
        mov     es, ax                  ; get data segment.
        ASSUME  ES:NOTHING
        add     si, di                  ; (SI) = last entry in table.

PciFindIrq_ScanTable:
        cmp     di, si                  ; have we hit the end of the table?
        jae     PciFindIrq_DoDefault    ; perform default initialization.
        mov     ax, word ptr es:[di]    ; get bus and device number.
        xchg    ah, al                  ; move bus/device to correct orientation.
        push    bx                      ; save Device and Function Number.
        and     bx, 0fff8h              ; mask off Function number.
        cmp     ax, bx                  ; are they the same?
        pop     bx                      ; restore Dev and Func Number.
        je      PciFindIrq_ScanEntry    ; scan the entry if so.
        add     di, 16                  ; find next entry.
        jmp     PciFindIrq_ScanTable    ; scan the table.

;       This entry matches the device, so get its IRQ info for PCI IRQ request.

PciFindIrq_ScanEntry:
        add     di, 2                   ; start at beginning of IRQ list.
        xor     ch, ch                  ; start with first entry.
        dec     cx                      ; correct to make (CX) = Offset / 3.
        add     di, cx
        add     di, cx
        add     di, cx                  ; (DI) = beginning of entry info.
        inc     cx                      ; return (CL) to original state.
        mov     al, byte ptr es:[di]    ; get the IRQ line we are using.
        xor     ah, ah
        test    ax, ax                  ; can the device be assigned this line?
        jz      PciFindIrq_Fail         ; nope, so fail.
        dec     ax
        mov     si, ax                  ; (SI) = offset to PCI IRQ line.
        mov     ax, word ptr es:[di+1]  ; get the IRQ entry info.
        add     si, CONFIG_PCI_BM_OFFSET; get location of actual INT assign.
        mov     cl, byte ptr es:[si]    ; get the assignment for the IRQ.
        cmp     cl, 0ffh                ; has it been assigned yet?
        je      PciFindIrq_AssignToLine ; if not, perform assignment now.
        bt      ax, cx                  ; Check to see if this is a valid IRQ.
        jc      PciFindIrq_DevLineFound ; Found the correct line.

PciFindIrq_AssignToLine:
        test    ax, PciIrqs             ; check to see if we can mask IRQs.
        jnz     PciFindIrq_MaskAssign   ; mask against already assigned.
        push    ax
        mov     ax, PciIrqsBackup       ; reset assignable to backups.
        mov     PciIrqs, ax             ; restore IRQ assignable bitmask.
        pop     ax
        test    ax, PciIrqs             ; double check if we can do it.
        jz      PciFindIrq_MaskAssignDone ; don't perform mask if not.

PciFindIrq_MaskAssign:
        and     ax, PciIrqs             ; mask out already used IRQs.

PciFindIrq_MaskAssignDone:

;       Because this device requires a specific IRQ that isn't assigned to the
;       line it's requesting, find a good line and assign it to the device.

        mov     cx, 15                  ; start at the top.
PciFindIrq_FindLoop:
        bt      ax, cx                  ; have we found an IRQ?
        jnc     PciFindIrq_FindLoopNext ; search till it's found.

;       We found a conflict for this device (in BX, bus/dev/func) due to its
;       entry in the PciIrqTbl.  We are about to re-route the assignment of this
;       IRQ.  Report this to the board module with BoardPciControl.  This is
;       mainly intended for debugging purposes, but a return from the routine is
;       accepted which overrides the assignment here.  Changing the value in
;       (AL) in BoardPciControl is discouraged, since it should probably be
;       fixed in the PciIrqTbl, but there may be dynamic POST requirements
;       where this is impractical.

        mov     ax, PCI_CTL_IRQ_REROUTE
        shl     eax, 16
        mov     ah, byte ptr es:[si]    ; (AH) = current routing.
        mov     al, cl                  ; (AL) = new routing.
        Gcall   BoardPciControl, POST   ; inform that an IRQ is being re-routed.

        mov     es:[si], al             ; we should assign this IRQ to this line.

;       Mask off this IRQ from the list of available IRQs to prevent other
;       INT lines from being assigned the same IRQ if we can manage to
;       prevent it.

        mov     cl, al                  ; (CL) = amount to shift by.
        mov     ax, 1                   ; (AX) = IRQ 0 bit.
        shl     ax, cl                  ; find actual IRQ bit.
        not     ax                      ; invert to create bit mask.
        and     PciIrqs, ax             ; mask out used IRQ.

PciFindIrq_DevLineFound:
        mov     ax, si                  ; continue with correct IRQ assigned.
        jmp     PciFindIrq_Found        ; found it, keep going.

PciFindIrq_FindLoopNext:
        loop    PciFindIrq_FindLoop     ; keep going till it's found.
        jmp     PciFindIrq_Fail         ; we failed.  Finish it off.

;       We failed to find the device in the table, so handle it with the
;       default IRQ line mapping.

PciFindIrq_DoDefault:

;       Default PCI 2.1 behavior is a barber pole ordering.
;       (((Device Number) + (pin number - 1)) % 4) = INT pin number.
;       0 = A, 1 = B, 2 = C, 3 = D.  This number is the shift applied to the
;       INT's (A,B,C,&D) given to the bridge it is behind (the bridge, as
;       a device, also applies a shift, and, going through all bridges, we
;       eventually arrive at the upstream slot, which has IRQ's as listed on
;       the PciIrqTbl, or uses the default barber-pole ordering).

        xor     ah, ah                  ; (AH) = 0.
        mov     al, bl                  ; (AX) = device/function.
        shr     ax, 3                   ; (AX) = device number only.
        dec     cx                      ; subtract 1 from line request.
        add     ax, cx                  ; add in the line request.
        and     ax, 3                   ; (AX) = (AX) % 4.

;       Now we have the shift to be applied to the available IRQs for this
;       slot.  Retrieve the available IRQs, then apply the shift to come
;       to the final IRQ to be given.

        movzx   di, bh                  ; (DI) = bus #.
        shl     di, 2                   ; (DI) = index to array.
        add     di, CONFIG_PCI_BIRQ_OFFSET ; (DI) = ptr to array entry.
        mov     edx, dword ptr es:[di]  ; (EDX) = entry for this bus.
        mov     cl, al                  ; (CL) = shift amount (above).
        shl     cl, 3                   ; (CL) = # bits to shift.
        ror     edx, cl                 ; (DL) = INT line attached.
        mov     al, dl
        cmp     dl, CONFIG_PCI_INT_LINES ; is the INT line valid?
        ja      PciFindIrq_Found

;       Check to see if an IRQ has been assigned to this INT line.
;       If no IRQ has been assigned, we will need to assign one.

        movzx   si, dl
        add     si, CONFIG_PCI_BM_OFFSET; find offset of INT line in table.
        mov     ax, CONFIG_PCI_IRQ_BITMAP ; IRQs that may be assigned.
        cmp     BYTE PTR es:[si], 0ffh  ; have we assigned an IRQ to this line?
        je      PciFindIrq_AssignToLine ; assign an IRQ to line if not.
        mov     al, dl

;       Store the PCI INT assignment which is in (AL) into PCIBI_DeviceIrqLine
;       for later assignment by the PciConfigIrqs procedure.  Note that not all
;       devices request IRQs to be assigned to PCI INT lines, so this will have
;       to be checked again in the PciConfigIrqs procedure.
;
;       The PCIBI structure contains the state associated with a supported
;       PCI bus in the system.  One of its elements, PCIBI_DeviceIrqLine is an
;       array of 32 words for the 32 possible devices on the bus, with 2 bits
;       allocated for each possible function to store the interrupt line
;       that may be requested by that function.

PciFindIrq_Found:
        push    ax
        mov     di, PCI_CONFIG_INT_REQUEST
        PciFunc READ_CONFIG_BYTE        ; call PCI services.
        dec     cl                      ; (CL) = INT line requested.
        pop     ax

        Pcall   PciFindMapEntry         ; (ES:DI) = ptr, the entry.
        ASSUME  ES:NOTHING
        xor     bh, bh                  ; clear the high bits.
        and     bl, 0f8h                ; isolate the device number.
        shr     bx, 1                   ; find offset of INT line entry.
        add     bl, cl                  ; add the offset of specific INT line.

;       Store the interrupt line this device is requesting into the location
;       allocated for it in the PCIBI_DeviceIrqLine entry.

        mov     BYTE PTR es:PCIBI_DeviceIrqLine.[bx+di], al
        clc                             ; indicate success.
        jmp     PciFindIrq_Exit         ; we're done.

;       Return with (CY) set if failure, else clear if success.

PciFindIrq_Fail:
        stc                             ; indicate failure (we're out of IRQs).

PciFindIrq_Exit:
        pop     bx
EndProc PciFindIrq

;***   PciConfigIrqs - Assign IRQ to PCI Device/Function.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by routines in this module to
;       allocate a system IRQ level and PCI interrupt line to
;       the function as requested by the device/function.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    99/02/03.       #4.2, rewrote for PCI Bridging support.
;       K. C. Taylor    99/02/12.       #4.2, fixed PCI Bridging IRQ assign bug.
;       F. E. Kerr      99/03/24.       #4.2, IRQ line for function # is 2 bits.
;       S. E. Jones     99/03/30.       #4.2, removed PciBusTotal.
;       S. E. Jones     99/03/30.       #4.2, add ASSUME ES:NOTHING.
;       F. E. Kerr      99/11/16.       #4.3, add BoardPciControl policy hook.
;       F. E. Kerr      00/02/11.       #4.3, add another hook (DEV_IRQ_ASSIGN).
;       K. C. Taylor    00/09/13.       #5.0, changed PCI IRQ assignment.
;       S. E. Jones     01/08/14.       #5.0, use POST scratch buffer.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;       K. C. Taylor    06/02/09.       #5.3.4, alternate next device method.
;       K. C. Taylor    06/02/14.       #5.3.4, predictable results if invalid line.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       None.
;
;   EXIT.
;       None.
;
;   USES.
;       All.

DefProc PciConfigIrqs, PUBLIC, OVERRIDE
        USEEXTDATA ds

;       We enter this routine with the final assignment of System IRQs to
;       PCI INTs in PciIrqAssign.  Report this list to the board module using
;       BoardPciControl and give it a final chance to modify the list.

        mov     bx, 0                   ; Start with bus 0, device 0.

        mov     ax, PCI_CTL_FINAL_PCI_IRQ
        shl     eax, 16
        Gcall   GetPostScratchSegAx, BIOS ; (AX) = seg ptr, POST scratch buffer.
        mov     es, ax
        mov     di, CONFIG_PCI_BM_OFFSET

        mov     edx, dword ptr es:[di]  ; for backwards compatibility.
        Gcall   BoardPciControl, POST   ; report the final assignment.
        mov     dword ptr es:[di], edx  ; for backwards compatibility.

PciConfigIrqs_Loop:
        push    bx
        mov     di, PCI_CONFIG_INT_REQUEST
        PciFunc READ_CONFIG_BYTE        ; call PCI services.
        jc      PciConfigIrqs_Next      ; if failure, skip the initialization.
        test    cl, cl                  ; is any interrupt pin requested?
        jz      PciConfigIrqs_Next      ; if none requested, go to next.
        cmp     cl, 4                   ; is the request in range?
        ja      PciConfigIrqs_Next      ; skip if too large.

;       The device is requesting a line, so we need to figure out which line
;       will be assigned to it so we know what IRQ to tell it to use.

        Pcall   PciFindMapEntry         ; (ES:DI) = ptr, the entry.
        ASSUME  ES:NOTHING

        and     bx, 0f8h                ; mask out for device without function.
        shr     bx, 1                   ; calculate offset in table.
        dec     cl
        sub     ch, ch
        add     bx, cx                  ; add the 0 based INT line request.
        mov     al, BYTE PTR es:PCIBI_DeviceIrqLine.[bx+di]
        pop     bx                      ; restore Bus/Device/Function.
        push    bx

;       Verify that the INT line is a valid line.

        sub     cl, cl                  ; (CL) = assign invalid IRQ 0.
        cmp     al, CONFIG_PCI_INT_LINES; is the INT line valid?
        ja      PciConfigIrqs_Found

;       Get the actual IRQ assigned to the line.  The top bit of each byte
;       in the list of IRQs is used to indicate that it is actually used, there
;       was at least one PCI device that requested an IRQ assigned to that PCI
;       INT line.  This will be used later to determine if a call to
;       BoardAssignPciIrq is needed.

        movzx   si, al                  ; (SI) = PCI INT line (0=A, 1=B, ...).
        add     si, CONFIG_PCI_BM_OFFSET
        mov     cl, es:[si]             ; (CL[0:6]) = IRQ for this device's line.

;       Report an IRQ is being assigned to a device to the board module
;       so it knows if needed for MP table.

PciConfigIrqs_Found:
        IF      OPTION_DIAGNOSE_PCI
        CPRINTF <PciConfigIrqs: Device/function $x INT $b, assigned IRQ $b.\n>, <bx, ax, cx>
        ENDIF   ; (OPTION_DIAGNOSE_PCI)

        push    eax
        mov     ax, PCI_CTL_DEV_IRQ_ASSIGN
        shl     eax, 16
        Gcall   BoardPciControl, POST   ; (BX)=dev/func, (CL)=IRQ assigned.
        pop     eax

        mov     di, PCI_CONFIG_INT_ASSIGN
        PciFunc WRITE_CONFIG_BYTE       ; write the PCI configuration register.

;       Advance to the next device/function on this bus.

PciConfigIrqs_Next:
        pop     bx
        test    bl, 7                   ; is the function already set?
        jnz     PciConfigIrqs_NextFunction ; advance function.
        mov     di, PCI_CONFIG_HEADER_TYPE ; select header type register.
        PciFunc READ_CONFIG_BYTE        ; read header type register.
        cmp     cl, 0ffh                ; does device/function exist?
        je      PciConfigIrqs_NextDevice ; advance by device.
        test    cl, PCI_CONFIG_HEADER_TYPE_MULTI
        jnz     PciConfigIrqs_NextFunction ; advance by function.

;       Advance to the function boundary.

PciConfigIrqs_NextDevice:
        add     bl, 7                   ; advance to edge of device.

;       Advance function for multi-function devices.

PciConfigIrqs_NextFunction:
        inc     bx                      ; next function.
        test    bl, bl                  ; did we just wrap to next bus?
        jnz     PciConfigIrqs_Loop      ; continue to enumerate bus if not.

;       Check next bus to be sure we are still within the bus limit.

PciConfigIrqs_CheckBusLimit:
        Gcall   PciHelpLastBus, POST    ; (AL) = total number of buses.
        cmp     bh, al
        jbe     PciConfigIrqs_Loop      ; keep going till we run out.

;       Now it is time to request the board module to assign the System IRQ
;       to PCI INT routing.  The list of IRQs is stored in PciIrqAssign, and the
;       top bit of each byte indicates that at least one PCI device requested
;       an IRQ from the associated line.  PciIrqAssign[0] contains the IRQ and
;       usage bit for PCI INTA, PciIrqAssign[1] contains the IRQ and usage bit
;       for PCI INTB, etc.

        sub     si, si
PciConfigIrqs_AssignRouting:
        mov     ax, si                  ; (AL) = PCI INT line (0=A, 1=B, ...).
        mov     ah, al                  ; move the IRQ line into (AH).
        mov     al, es:[si+CONFIG_PCI_BM_OFFSET] ; get the IRQ assigned to PCI INT (AL).
        cmp     al, 0ffh                ; check for usage.
        jz      PciConfigIrqs_Unused    ; if unused, don't call BoardAssignPciIrq.
        Gcall   PciAssignPciIrq, POST   ; call on the board to assign it.

PciConfigIrqs_Unused:
        inc     si
        cmp     si, CONFIG_PCI_INT_LINES
        jb      PciConfigIrqs_AssignRouting

EndProc PciConfigIrqs


;***   PostPciDisplayDevices - POST Routine For displaying all configured devices.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by mainline POST to find all the devices on
;       the bus, and display information about them.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    99/01/07.       #4.2, New.
;       K. C. Taylor    99/02/09.       #4.2, Upgraded amount of information.
;       S. E. Jones     99/03/30.       #4.2, removed PciBusTotal.
;       K. C. Taylor    00/02/08.       #4.3, add basic cardbus support.
;       B. J. Brashier  00/05/17.       #4.4, added BoardPciControl hook.
;       K. C. Taylor    02/01/14.       #5.0, CardBus bridges get assigned IRQ.
;       B. J. Brashier  03/07/14.       #5.2, now builds if debugger is on.
;       K. C. Taylor    06/01/12.       #5.3.4, use new register constants.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       all but SS, SP.

DefProc PostPciDisplayDevices, GLOBAL, OVERRIDE
        IF      ((OPTION_SUPPORT_PCI_POSTMSGS NE 0) OR (OPTION_SUPPORT_DEBUGGER NE 0))
        PUSHALL
        USEEXTDATA ds

;       Scan through the list all the devices, and display their information.

        PRINTF <\nPCI Device Table.\n>
        PRINTF <Bus Dev Func VendID DevID Class              Irq\n>

        sub     bx, bx                  ; Start with Bus 0, Device 0, Function 0.
PostPciDisplayDevices_Start:

;       Read the Vendor ID to see if the device exists.

        mov     di, PCI_CONFIG_VENDOR_ID
        PciFunc READ_CONFIG_WORD

        cmp     cx, 0FFFFh              ; Is this a valid device?
        je      PostPciDisplayDevices_NextDevice

;       Print the basic device information.

        push    bx
        mov     ax, bx
        mov     cl, bh                  ; (CX) = Bus number.
        sub     ch, ch
        and     bx, 7                   ; (BX) = function number.
        ISHR    ax, 3
        and     ax, 1Fh                 ; (AX) = device number.

        PRINTF < $b  $b  $b   >, <cx, ax, bx>

        pop     bx

;       Obtain the vendor and device ID fields from the PCI device.

        mov     di, PCI_CONFIG_VENDOR_DEVICE_ID
        PciFunc READ_CONFIG_DWORD       ; Perform the read.
        mov     dx, cx                  ; (DX) = vendor ID.
        shr     ecx, 16                 ; (CX) = device ID.

        PRINTF <$x   $x  >, <dx, cx>

;       Obtain the device class.

        mov     di, PCI_CONFIG_CLASS
        PciFunc READ_CONFIG_BYTE
        mov     dh, cl
        mov     di, PCI_CONFIG_SUB_CLASS
        PciFunc READ_CONFIG_BYTE
        mov     dl, cl
        mov     di, PCI_CONFIG_INTERFACE
        PciFunc READ_CONFIG_BYTE

;       (DH) = device class code.
;       (DL) = sub device class code.
;       (CL) = interface.

;       Find the text string for this device.

        mov     si, 0                   ; Start with first element in table.
        sub     si, 5                   ; Compensate for first action.

PostPciDisplayDevices_NextClass:
        add     si, 5                   ; Go to next entry in table.

;       Check to see if this is the device we're looking for, and check next
;       class if it isn't.

;       Start by checking the Base class.

        cmp     PciNameTbl.PNTT_Base.[si], 0FFh
        je      @f
        cmp     PciNameTbl.PNTT_Base.[si], dh
        jne     PostPciDisplayDevices_NextClass

;       Next check the Sub-class.

@@:     cmp     PciNameTbl.PNTT_Sub.[si], 0FFh
        je      @f
        cmp     PciNameTbl.PNTT_Sub.[si], dl
        jne     PostPciDisplayDevices_NextClass

;       Last, check the interface type.

@@:     cmp     PciNameTbl.PNTT_Interface.[si], 0FFh
        je      @f
        cmp     PciNameTbl.PNTT_Interface.[si], cl
        jne     PostPciDisplayDevices_NextClass

;       If we reach this point, we have either reached the end of the table,
;       or we've found our device, so print it.

@@:     mov     cx, PciNameTbl.PNTT_Text.[si]
        PRINTF  <$s >,<cs,cx>

;       Read in the IRQ assignment, and print it if there is one.

        mov     di, PCI_CONFIG_INT_REQUEST
        PciFunc READ_CONFIG_BYTE

        test    cl, cl                  ; Did the device request an IRQ?
        jz      @f                      ; Skip if it didn't.
        cmp     cl, 4                   ; Was it in range?
        ja      @f                      ; Skip if it wasn't.

        mov     di, PCI_CONFIG_INT_ASSIGN
        PciFunc READ_CONFIG_BYTE

        sub     ch, ch                  ; (CH) = 0.
        PRINTF <$d>, <cx>               ; Print out the IRQ number.
@@:

PostPciDisplayDevices_DonePrinting:

;       At this point we are done printing all the information about this
;       device.  Print a new line, and carry on.

        PRINTF <\n>

PostPciDisplayDevices_NextDevice:
        Pcall   PciFindNext             ; Find next device/function/bus device.
        jnc     PostPciDisplayDevices_Start

;       Well, we ran out of devices.  Now move on to the next bus.

@@:     inc     bh                      ; Next bus.
        sub     bl, bl                  ; (BL) = 0.
        push    ax
        Gcall   PciHelpLastBus, POST    ; (AL) = number of buses.
        cmp     bh, al
        pop     ax
        jbe     PostPciDisplayDevices_Start

        POPALL
        ENDIF   ; ((OPTION_SUPPORT_PCI_POSTMSGS NE 0) OR (OPTION_SUPPORT_DEBUGGER NE 0))
EndProc PostPciDisplayDevices

;***   PciFindNextDevice - Find Next Device or Function.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine scans a bus to find the next device/function number on that
;       bus, and either returns the next function on this device, the next
;       device, or returns with failure (carry set).
;
;       Note that this routine skips bridge devices.
;
;   MODIFICATION HISTORY.
;       K. C. Taylor    99/10/11.       #4.3, New.
;       K. C. Taylor    00/02/08.       #4.3, add basic cardbus support.
;       B. J. Brashier  00/05/11.       #4.4, added BoardPciControl call.
;       K. C. Taylor    00/10/31.       #5.0, moved to PCI, made GLOBAL.
;       K. C. Taylor    02/01/14.       #5.0, CardBus bridge change.
;       K. C. Taylor    06/01/12.       #5.3.4, support CardBus properly.
;
;   WARNINGS.
;       This code is only used in the 16 bit BIOS during initialization.
;
;   ENTRY.
;       BH - Entry Index, PCI Bus No.
;       BL - PCI Device/Function No.
;
;   EXIT.
;       BH - maintained.
;       BL - next PCI device/function if CY is clear.
;       CY - clear if successful, set if no next device available.

        IF      BIOS_16
DefProc PciFindNextDevice, GLOBAL, OVERRIDE
        push    di
        push    cx
        push    ax

PciFindNextDevice_Loop:
        Pcall   PciFindNext             ; Get next Item.
        jc      PciFindNextDevice_Exit  ; Exit if we run out.

;       We have a valid PCI device.  Test to see if it's a bridge.  Note that
;       CardBus bridges are accounted for outside of this routine.

        mov     di, 000eh               ; (DI) = 000eh, Header Type register.
        PciFunc READ_CONFIG_BYTE
        jc      PciFindNextDevice_Loop  ; if call failed, skip this device.
        and     cl, 7fh                 ; mask of multi function bit.
        test    cl, 03h                 ; is this a PCI-PCI bridge or CardBus?
        jnz     PciFindNextDevice_Loop  ; if so, skip it.

;       We've tested for standard PCI-PCI bridges; now test for chipset-specific
;       bridges, such as extra host-bridges with PCI busses attached.

        mov     ax, PCI_CTL_IS_BRIDGE   ; ask BoardPciControl if this is a bridge.
        shl     eax, 16                 ; PCI_CTL goes in top half EAX.
        mov     al, PCI_CTL_NO_BRIDGE_FOUND     ; assume no bridge will be found.
        Gcall   BoardPciControl, POST   ; allow board or chipset intervention.

        cmp     al, PCI_CTL_NO_BRIDGE_FOUND     ; is this device actually a bridge?
        jne     PciFindNextDevice_Loop  ; if so, skip to next device.

PciFindNextDevice_Found:
        clc                             ; indicate success.

PciFindNextDevice_Exit:
        pop     ax
        pop     cx
        pop     di
EndProc PciFindNextDevice

        ENDSEG  ; (PCI)

POST_   SEGMENT

;***   PciWriteByteNoStack - Write a BYTE register anywhere in PCI Cfg space.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine uses the PCI mechanism to write a BYTE register
;       anywhere in the PCI configuration space.
;
;       The routine is optimized for stackless usage by minimizing the
;       number of registers modified during operation.  The I/O instructions
;       are performed with the same registers used to pass register offset on
;       entry.  Caller will have to reload the PFA and register offset each call
;       because these values are destroyed.
;
;       Consider use of PciFunc macros after stack is available as this
;       uses Int 1Ah conventions and returns data in CL/CX/ECX eliminating
;       the need to reload PFA and register offset every time the routine
;       is called.
;
;   MODIFICATION HISTORY.
;       B. J. Brashier  02/11/19.       #5.2, new.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       AH      - bus number of the PCI device being queried.
;       AL      - [7:3] = device number, [2:0] = function number.
;       BL      - Byte data to be written to register.
;       CL      - Index of the register to access.
;
;   EXIT.
;       none.
;
;   USES.
;       EAX, CL, EDX, Flags.

        IF      OPTION_PCI_STACKLESS
DefRtn  PciWriteByteNoStack, PUBLIC, OVERRIDE
        movzx   eax, ax                 ; zero extend AX entry param to 32 bits.
        shl     eax, 8                  ; shift bits to appropriate position.
        or      eax, 080000000h         ; set PCI CSAM Enable Bit.
        mov     al, cl                  ; enter the register to be accessed.
        and     al, (NOT 03h)           ; calculate DWORD address.

        mov     dx, 0cf8h               ; (DX) = PCI Configuration Address port.
        out     dx, eax                 ; point PCI Cfg Data port to this addr.

        mov     dx, 0cfch               ; (DX) = PCI Configuration Data port.
        and     cl, 03h                 ; (CL) = byte shift within DWORD.
        add     dl, cl
        mov     al, bl
        out     dx, al
EndRtn  PciWriteByteNoStack
        ENDIF   ; (OPTION_PCI_STACKLESS)

;***   PciWriteWordNoStack - Write a WORD register anywhere in PCI Cfg space.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine uses the PCI mechanism to write a WORD register
;       anywhere in the PCI configuration space.
;
;       The routine is optimized for stackless usage by minimizing the
;       number of registers modified during operation.  The I/O instructions
;       are performed with the same registers used to pass register offset on
;       entry.  Caller will have to reload the PFA and register offset each call
;       because these values are destroyed.
;
;       Consider use of PciFunc macros after stack is available as this
;       uses Int 1Ah conventions and returns data in CL/CX/ECX eliminating
;       the need to reload PFA and register offset every time the routine
;       is called.
;
;   MODIFICATION HISTORY.
;       B. J. Brashier  02/11/19.       #5.2, new.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       AH      - bus number of the PCI device being queried.
;       AL      - [7:3] = device number, [2:0] = function number.
;       BX      - Word data to be written to register.
;       CL      - Index of the register to access.
;
;   EXIT.
;       none.
;
;   USES.
;       EAX, CL, EDX, Flags.

        IF      OPTION_PCI_STACKLESS
DefRtn  PciWriteWordNoStack, PUBLIC, OVERRIDE
        movzx   eax, ax                 ; zero extend AX entry param to 32 bits.
        shl     eax, 8                  ; shift bits to appropriate position.
        or      eax, 080000000h         ; set PCI CSAM Enable Bit.
        mov     al, cl                  ; enter the register to be accessed.
        and     al, (NOT 03h)           ; calculate DWORD address.

        mov     dx, 0cf8h               ; (DX) = PCI Configuration Address port.
        out     dx, eax                 ; point PCI Cfg Data port to this addr.

        mov     dx, 0cfch               ; (DX) = PCI Configuration Data port.
        and     cl, 03h                 ; (CL) = byte shift within DWORD.
        add     dl, cl
        mov     ax, bx
        out     dx, ax
EndRtn  PciWriteWordNoStack
        ENDIF   ; (OPTION_PCI_STACKLESS)

        ENDSEG  ; (POST)

        ENDIF   ; (OPTION_SUPPORT_PCI)
