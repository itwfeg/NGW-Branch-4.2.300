;***    BRD1632 -- Board Module 16/32 bit PCI Code for the AMD LXDB800 Platform.
;
;1.     Functional Description.
;       This is the default 16/32 bit PCI code for the LXDB800 "Salsa" board.
;       It is used to produce both the 16-bit PCIAPI code and the 32-bit PCIAPI
;       code.
;
;       The Salsa platform supports either four 32-bit 33MHz PCI slots or two
;       32-bit 66MHz slots, selectable via jumper J67 located on the baseboard.
;
;       The following table lists the PCI devices on the AMD LXDB800
;       "Salsa" Development Platform:
;
;       PCI Device                               Bus   Dev
;       -------------------------------------   ----  ----
;       Geode GX3 Host Bridge                    00h   01h
;       Network controller                       00h   0Ch
;       PCI 32-bit, 66MHz Slot J12               00h   0Dh
;       Geode CS5535 IO Companion                00h   0Fh
;
;2.     Modification History.
;       R. Spiegel      05/04/27.       #5.3, updated for Salsa board.
;       R. Spiegel      05/06/10.       #5.3.3, removed unused PCI slots from IRQ table.
;
;3.     Warnings.
;
;4.     NOTICE: Copyright (C) 2001-2005 General Software, Inc.

;       The following table defines the PCI IRQ routing that the
;       board supports.  One entry in the table defines a PCI device.
;       Documentation here in the source code is not sufficient.
;       You need to know the PCI API spec and build a routing table
;       that can be returned by INT 1ah function b1h subfunction 0eh.
;
;       Note that we only need the 16 bit version.  The 32-bit build is
;       linked by GSMERGE to the 16 bit table.

        IF      OPTION_SUPPORT_PCI

;       PCI_IRQ contains the list (bitmap) of allowed PCI IRQs.

public  PCI_IRQ
PCI_IRQ         = CONFIG_PCI_IRQ_BITMAP

;       !!! DO NOT CHANGE THESE EQUATES !!!
;       These equates represent the chipset PCIIRQ routing capabilities.
;
;       The numeric values may be set to any arbitrary value although in our
;       case these are set equal to the (chipset index value-10h) for the
;       corresponding interrupt to simplify troubleshooting PCI Interrupt
;       routing tables in memory.

NO_CONNECT      = 0

;       CS5535 PCI INT[A-D] Interrupt Routing lines.

CS_PCI_INTA     = 1                     ; PCI INTA.
CS_PCI_INTB     = 2                     ; PCI INTB.
CS_PCI_INTC     = 3                     ; PCI INTC.
CS_PCI_INTD     = 4                     ; PCI INTD.

;       !! These equates should be customized for each board implementation !!
;       The equate names should be changed to logical names for each specific
;       board implementation.  PCI embedded devices are traditionally assigned
;       an ID of 0ffh.

ID_SLOT_PCI_NET         = 1             ; Ethernet on Salsa schematics.
ID_SLOT_PCI_RSVD1       = 2             ; reserved entry 1.
ID_SLOT_PCI_SLOT0       = 3             ; PCI slot 0, J12.
ID_SLOT_PCI_RSVD2       = 4             ; reserved entry 2.
ID_EMBED_PCI            = 0ffh          ; onboard PCI device, not a slot.

;       Open PCI 16 bit code.

        IF      BIOS_16

;       PCI Routing Table.
;       The PciIrqTable is designed as an interface to the Operating System
;       describing the PCI interrupts in the system.  Each entry describes
;       one PCI device and the 4 interrupts that this device could request.
;       The first interrupt listed is the interrupt a function would get if
;       it requested INTA.  The next is INTB and so on.  This request is made
;       via PCI header register 05Ch, and the final IRQ given is in 05Dh.

PciIrqTbl label byte

;       Geode GX3 Host Bridge and VGA Graphics.
;       Bus 0, Device 01h.

        PCIIRQENT       0,\
                        01h,\
                        CS_PCI_INTA, PCI_IRQ,\
                        NO_CONNECT, PCI_IRQ,\
                        NO_CONNECT, PCI_IRQ,\
                        NO_CONNECT, PCI_IRQ,\
                        ID_EMBED_PCI

;       Realtec RTL8110SB Network controller.
;       Bus 0, Device 0dh (IDSEL = 23).

        PCIIRQENT       0,\
                        0dh,\
                        CS_PCI_INTD, PCI_IRQ,\
                        CS_PCI_INTA, PCI_IRQ,\
                        CS_PCI_INTB, PCI_IRQ,\
                        CS_PCI_INTC, PCI_IRQ,\
                        ID_SLOT_PCI_NET

;       This entry is reserved for future extentions.
;       Bus 0, Device 0ch (IDSEL = 22).

        PCIIRQENT       0,\
                        0ch,\
                        CS_PCI_INTA, PCI_IRQ,\
                        CS_PCI_INTB, PCI_IRQ,\
                        CS_PCI_INTC, PCI_IRQ,\
                        CS_PCI_INTD, PCI_IRQ,\
                        ID_SLOT_PCI_RSVD1

;       Geode CS5535/CS5536 IO Companion: USB controllers, IDE, Audio.
;       Bus 0, Device 0fh.

        PCIIRQENT       0,\
                        0fh,\
                        CS_PCI_INTA, PCI_IRQ,\
                        CS_PCI_INTB, PCI_IRQ,\
                        CS_PCI_INTC, PCI_IRQ,\
                        CS_PCI_INTD, PCI_IRQ,\
                        ID_EMBED_PCI

;       PCI Slot 0, J12.
;       Bus 0, Device 0eh (IDSEL = 24).

        PCIIRQENT       0,\
                        0eh,\
                        CS_PCI_INTC, PCI_IRQ,\
                        CS_PCI_INTD, PCI_IRQ,\
                        CS_PCI_INTA, PCI_IRQ,\
                        CS_PCI_INTB, PCI_IRQ,\
                        ID_SLOT_PCI_SLOT0

;       This entry is cardbus example.
;       Bus 0, Device 0bh (IDSEL = 21).

        PCIIRQENT       0,\
                        0bh,\
                        CS_PCI_INTD, PCI_IRQ,\
                        CS_PCI_INTA, PCI_IRQ,\
                        CS_PCI_INTB, PCI_IRQ,\
                        CS_PCI_INTC, PCI_IRQ,\
                        ID_SLOT_PCI_RSVD2

PCI_TABLE_INFO_SIZE = ($ - PciIrqTbl)   ; size of table in bytes.
PciIrqTblEnd label byte
        ENDIF   ; (BIOS_16)

;***   BoardAssignPciIrq - Steer PCI Interrupt Line to System IRQ Channel.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called from the PCI Configuration Manager in the core
;       BIOS to map or steer a PCI interrupt line to a specific ISA IRQ
;       channel supported by the 8259 Programmable Interrupt Controller.
;
;   MODIFICATION HISTORY.
;       R. Spiegel      05/04/27.       #5.3.3, New.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       AH      - PCI Interrupt Assignment: 0=INTA#, 1=INTB#, 2=INTC#, 3=INTD#.
;       AL      - System IRQ Level (0-15, 16=disable).
;
;   EXIT.
;       CY      - set if assignment failed, clear if success.
;
;   USES.
;       Flags.

DefProc BoardAssignPciIrq, PUBLIC, OVERRIDE
        PUSHALL

;       If this is a disable request, then change that to 0000b, since
;       the latter value is used as the programming value in the South Bridge.

        cmp     al, 16                  ; is this the "disable" value?
        jb      @f                      ; if so, skip.
        sub     al, al                  ; if so, change to 0.
@@:

;       Determine which register is to be programmed.

        mov     di, 005ch               ; (DI) = register for INTB/INTA.
        cmp     ah, 2                   ; is this for lines C or D?
        jb      @f                      ; if not, skip.
        mov     di, 005dh               ; (DI) = register for INTD/INTC.
@@:

;       Compute shift count so that we can position the interrupt level
;       over the right nibble in the register determined above.

        mov     cl, 0                   ; (CL) = shift count.
        mov     bl, 11110000b           ; (BL) = mask.
        test    ah, 01h                 ; is this for INTD or INTB?
        jz      @f                      ; if not, skip.
        mov     cl, 4
        mov     bl, 00001111b           ; (BL) = mask.
@@:

;       Now program the register with a read/modify/write sequence.

        mov     bh, al                  ; (BL) = value to program.
        shl     bh, cl                  ; (BH) = normalized field.

;       Set (CL) = existing PCI register contents.

        push    bx
        sub     bh, bh                  ; (BH) = bus number 0.
        mov     bl, 01111000b           ; (BL) = device 0Fh, function 0.
        PciFunc READ_CONFIG_BYTE        ; read the configuration byte.
        pop     bx
        jc      BoardAssignPciIrq_Exit  ; if the service failed.

        and     cl, bl                  ; mask-out the field to be updated.
        or      cl, bh                  ; OR-in the new contents.

;       Write the updated value back.

        sub     bh, bh                  ; (BH) = bus number 0.
        mov     bl, 01111000b           ; (BL) = device 0Fh, function 0.
        PciFunc WRITE_CONFIG_BYTE
        jc      BoardAssignPciIrq_Exit  ; if the service failed.
        clc                             ; indicate success.

;       Return with status in (CY).

BoardAssignPciIrq_Exit:
        POPALL
EndProc BoardAssignPciIrq
        ENDIF   ; (OPTION_SUPPORT_PCI)
