;***    PCI.INC -- Embedded BIOS PCI Definitions.
;
;1.     Functional Description.
;       This include file defines function codes and other manifest
;       constants for controlling various PCI operations while in POST
;       or in PCI enumeration.
;
;2.     Modification History.
;       K. C. Taylor    06/01/12.       #5.3.4, new config constants.
;       S. E. Jones     06/01/17.       #5.3.4, change STRUCT -> STRUC, tidy up.
;       K. C. Taylor    06/01/19.       #5.3.4, packaged for 5.3.3.
;       K. C. Taylor    07/01/07.       #5.3.3, fixed CardBus support.
;
;3.     NOTICE: Copyright (C) 1999-2006 General Software, Inc.

;       Function codes to pass to BoardPciControl in the top half of EAX.
;       BoardPciControl must clear carry but return with registers unchanged
;       if unknown codes are received, in order to remain forward-compatible.

PCI_CTL_VGA_SCAN        =       0       ; scan found VGA, not on PCI bus 0.
PCI_CTL_ENUM_DONE       =       1       ; PCI enumeration completed.
PCI_CTL_VGA_SCAN0       =       2       ; scan found VGA on PCI bus 0.
PCI_CTL_GET_VGA_LOC     =       3       ; return desired boot VGA bus/dev/func.
PCI_CTL_SCAN_VIDEO_DONE =       4       ; video scan completed.
PCI_CTL_INIT_PCI_IRQ    =       5       ; (EDX) = initial IRQ to PCI INT routing.
PCI_CTL_IRQ_REROUTE     =       6       ; re-routing PCI INT line due to PciIrqTbl.
PCI_CTL_FINAL_PCI_IRQ   =       7       ; (EDX) = final IRQ to PCI INT routing.
PCI_CTL_BRIDGES_FOUND   =       8       ; PCI buses have been enumerated.
PCI_CTL_DEV_IRQ_ASSIGN  =       9       ; an IRQ is being assigned to a device.
PCI_CTL_FIND_BRIDGE     =       10      ; alternate PciFindBridge.
PCI_CTL_WRITE_BRIDGE_REGS =     11      ; an alternate register-write path.
PCI_CTL_IS_BRIDGE       =       12      ; is this device/func a bridge?
PCI_CTL_VGA_BRIDGE      =       13      ; enabling VGA on upstream bridges.
PCI_CTL_RES_ASSIGNED    =       14      ; resources assigned for this bridge.
PCI_CTL_DEV_TYPE_TABLE  =       15      ; device type table specified.

PCI_CTL_BRIDGE_FOUND    =       1       ; positive return value for PCI_CTL_FIND_BRIDGE.
PCI_CTL_NO_BRIDGE_FOUND =       0       ; negative return value for PCI_CTL_FIND_BRIDGE.

;       The following values are PCI header type codes, used to identify
;       specific indicies within a PCI device configuration space.

PCI_CONFIG_VENDOR_ID    =       000h    ; vendor ID index (word).
PCI_CONFIG_DEVICE_ID    =       002h    ; device ID index (word).
PCI_CONFIG_VENDOR_DEVICE_ID =   000h    ; device and vendor ID combined (dword).
PCI_CONFIG_COMMAND      =       004h    ; command register (word).
PCI_CONFIG_STATUS       =       006h    ; status register (word).
PCI_CONFIG_COMMAND_STATUS =     004h    ; command and status combined (dword).
PCI_CONFIG_REVISION_ID  =       008h    ; revision ID for device revision (byte).
PCI_CONFIG_INTERFACE    =       009h    ; device interface (byte).
PCI_CONFIG_SUB_CLASS    =       00ah    ; device sub-class (byte).
PCI_CONFIG_CLASS        =       00bh    ; device class (byte).
PCI_CONFIG_FULL_CLASS_ID =      008h    ; combined rev/interface/class (dword).
PCI_CONFIG_CACHE_LINE_SIZE =    00ch    ; cache line size (byte).
PCI_CONFIG_LATENCY_TIMER =      00dh    ; master latency timer (byte).
PCI_CONFIG_HEADER_TYPE  =       00eh    ; header type (byte).
PCI_CONFIG_HEADER_TYPE_MASK =   07fh    ; header type mask.
PCI_CONFIG_HEADER_TYPE_DEVICE = 000h    ; type 0 header for devices.
PCI_CONFIG_HEADER_TYPE_BRIDGE = 001h    ; type 1 header for bridges.
PCI_CONFIG_HEADER_TYPE_CARDBUS = 002h   ; type 2 header for cardbus bridges.
PCI_CONFIG_HEADER_TYPE_MULTI =  080h    ; multifunction device flag.
PCI_CONFIG_BIST         =       00fh    ; built in self test (byte).
PCI_CONFIG_BIST_SUPPORTED =     BIT7    ; set to one if BIST is supported.
PCI_CONFIG_BIST_START   =       BIT6    ; set to one to start BIST.
PCI_CONFIG_BIST_STATUS  =       BIT6    ; set to one while BIST in progress.
PCI_CONFIG_BAR_0        =       010h    ; base address register 0 (dword).
PCI_CONFIG_BAR_1        =       014h    ; base address register 1 (dword).
PCI_CONFIG_BAR_2        =       018h    ; base address register 2 (dword).
PCI_CONFIG_BAR_3        =       01ch    ; base address register 3 (dword).
PCI_CONFIG_BAR_4        =       020h    ; base address register 4 (dword).
PCI_CONFIG_BAR_5        =       024h    ; base address register 5 (dword).
PCI_CONFIG_BAR_IO       =       BIT0    ; BAR is I/O type.
PCI_CONFIG_BAR_TYPE_MASK =      (BIT1 OR BIT2) ; mask for range type.
PCI_CONFIG_BAR_TYPE_32BIT =     0       ; BAR is 32 bits.
PCI_CONFIG_BAR_TYPE_LEGACY =    BIT1    ; BAR is legacy BAR (must be < 1MB).
PCI_CONFIG_BAR_TYPE_64BIT =     BIT2    ; BAR is 64 bits.
PCI_CONFIG_BAR_PREFETCH =       BIT3    ; BAR is prefetchable.
PCI_CONFIG_CARBUS_CIS   =       028h    ; cardbus card information structure (dword).
PCI_CONFIG_SUB_VENDOR_ID =      02ch    ; subsystem vendor ID (word).
PCI_CONFIG_SUB_DEVICE_ID =      02eh    ; subsystem device ID (word).
PCI_CONFIG_SUB_VENDOR_DEVICE_ID = 02ch  ; subsystem vendor and device ID (dword).
PCI_CONFIG_ROM_BAR      =       030h    ; ROM base address (dword).
PCI_CONFIG_CAP_PTR      =       034h    ; capability pointer (byte).
PCI_CONFIG_INT_ASSIGN   =       03ch    ; interrupt assignment (byte).
PCI_CONFIG_INT_REQUEST  =       03dh    ; interrupt pin request (byte).
PCI_CONFIG_MIN_GRANT    =       03eh    ; minimum grant requirement (byte).
PCI_CONFIG_MAX_LATENCY  =       03fh    ; maxumum latency between grants (byte).

;       The following values are PCI header type codes used to identify
;       specific additional fields in a type 1 header (PCI to PCI bridge).

PCI_CONFIG_BUS_PRIMARY  =       018h    ; primary bus number (byte).
PCI_CONFIG_BUS_SECONDARY =      019h    ; secondary bus number (byte).
PCI_CONFIG_BUS_SUBORDINATE =    01ah    ; subordinate bus number (byte).
PCI_CONFIG_SECONDARY_LATENCY =  01bh    ; secondary latency (byte).
PCI_CONFIG_IO_BASE      =       01ch    ; I/O base (byte).
PCI_CONFIG_IO_BASE_32BIT =      BIT0    ; if set, I/O base is 32 bits.
PCI_CONFIG_IO_LIMIT     =       01dh    ; I/O limit (byte).
PCI_CONFIG_IO_LIMIT_32BIT =     BIT0    ; if set, I/O limit is 32 bits.
PCI_CONFIG_SECONDARY_STATUS =   01eh    ; secondary status (word).
PCI_CONFIG_MEMORY_BASE  =       020h    ; memory base (word).
PCI_CONFIG_MEMORY_LIMIT =       022h    ; memory limit (word).
PCI_CONFIG_PREFETCH_MEMORY_BASE = 024h  ; prefetchable memory base (word).
PCI_CONFIG_PREFETCH_MEMORY_64BIT = BIT0 ; 64 bit decodes supported.
PCI_CONFIG_PREFETCH_MEMORY_LIMIT = 026h ; prefetchable memory limit (word).
PCI_CONFIG_TOP_PREFETCH_BASE =  028h    ; top 32 bits of prefetchable base (dword).
PCI_CONFIG_TOP_PREFETCH_LIMIT = 02ch    ; top 32 bits of prefetchable limit (dword).
PCI_CONFIG_TOP_IO_BASE  =       030h    ; top 16 bits of I/O base (word).
PCI_CONFIG_TOP_IO_LIMIT =       032h    ; top 16 bits of I/O limit (word).
PCI_CONFIG_BRIDGE_ROM_BAR =     038h    ; alternate ROM BAR for bridges (dword).
PCI_CONFIG_BRIDGE_CONTROL =     03eh    ; bridge control register (word).

;       The following values are PCI header type codes used to assign resources
;       to CardBus bridges.

PCI_CONFIG_CARDBUS_MEM0_BASE =  01ch    ; base address for memory range 0.
PCI_CONFIG_CARDBUS_MEM0_LIMIT = 020h    ; limit address for memory range 0.
PCI_CONFIG_CARDBUS_MEM1_BASE =  024h    ; base address for memory range 1.
PCI_CONFIG_CARDBUS_MEM1_LIMIT = 028h    ; limit address for memory range 1.
PCI_CONFIG_CARDBUS_IO0_BASE =   02ch    ; I/O base address for I/O range 0.
PCI_CONFIG_CARDBUS_IO0_LIMIT =  030h    ; I/O limit address for I/O range 0.
PCI_CONFIG_CARDBUS_IO1_BASE =   034h    ; I/O base address for I/O range 1.
PCI_CONFIG_CARDBUS_IO1_LIMIT =  038h    ; I/O limit address for I/O range 1.

;***    PCIHDR  - PCI Header Configuration Table Entry.
;
;       The following structure is used to declare an entry in a PCI header
;       configuration table.  PCI header configuration tables are used to
;       describe PCI device headers to PCI enumeration code.  Note that any
;       table containing a PCI header description must contain at least two
;       entries with the last entry always having the flag field set to 0.

PCIHDR          STRUC
pcihdr_flags    dw      ?               ; supported data types.

PCIHDR_FLAG_IO          =       BIT0    ; header entry supports I/O.
PCIHDR_FLAG_32BIT       =       BIT1    ; header entry supports 32 bit memory.
PCIHDR_FLAG_32LEG       =       BIT2    ; header entry supports 32 bit legacy.
PCIHDR_FLAG_64BIT       =       BIT3    ; header entry supports 64 bit memory.
PCIHDR_FLAG_32BIT_PREFETCH =    BIT4    ; header entry supports 32 bit memory, prefetch.
PCIHDR_FLAG_32LEG_PREFETCH =    BIT5    ; header entry supports 32 bit legacy, prefetch.
PCIHDR_FLAG_64BIT_PREFETCH =    BIT6    ; header entry supports 64 bit memory, prefetch.

PCIHDR_FLAG_BAR_SET     = (PCIHDR_FLAG_IO OR PCIHDR_FLAG_32BIT OR \
                           PCIHDR_FLAG_32LEG OR PCIHDR_FLAG_64BIT OR \
                           PCIHDR_FLAG_32BIT_PREFETCH OR \
                           PCIHDR_FLAG_32LEG_PREFETCH OR \
                           PCIHDR_FLAG_64BIT_PREFETCH) ; full BAR flag set.
PCIHDR_FLAG_BAR_32BIT   = (PCIHDR_FLAG_IO OR PCIHDR_FLAG_32BIT OR \
                           PCIHDR_FLAG_32LEG OR PCIHDR_FLAG_32BIT_PREFETCH OR \
                           PCIHDR_FLAG_32LEG_PREFETCH) ; 32 bit BAR flag set.

PCIHDR_FLAG_BAR_NOT_IO  = (PCIHDR_FLAG_32BIT OR PCIHDR_FLAG_32LEG OR \
                           PCIHDR_FLAG_64BIT OR PCIHDR_FLAG_32BIT_PREFETCH OR \
                           PCIHDR_FLAG_32LEG_PREFETCH OR \
                           PCIHDR_FLAG_64BIT_PREFETCH) ; BAR flag set, less I/O.

PCIHDR_FLAG_OPTROM      =       BIT7    ; option ROM region assignment.
PCIHDR_FLAG_COMMAND     =       BIT8    ; command register assignment.
PCIHDR_FLAG_LATENCY     =       BIT9    ; master latency assignment.
PCIHDR_FLAG_INT         =       BIT10   ; interrupt assignment.
PCIHDR_FLAG_BUS_ASSIGN  =       BIT11   ; bus assignment.
PCIHDR_FLAG_BIST    =       BIT12   ; built-in self-test handler.
PCIHDR_FLAG_CAP_LIST    =       BIT13   ; extended capability list.
PCIHDR_FLAG_CACHE_LINE  =       BIT14   ; cache line size.
PCIHDR_FLAG_MEM_COMBINE_64_32 = BIT15   ; combine 64-bit regions into 32-bit regions.

PCIHDR_FLAG_MISC        =       (PCIHDR_FLAG_COMMAND OR \
                                 PCIHDR_FLAG_LATENCY OR \
                                 PCIHDR_FLAG_INT OR \
                                 PCIHDR_FLAG_BIST OR \
                                 PCIHDR_FLAG_CAP_LIST OR \
                                 PCIHDR_FLAG_CACHE_LINE) ; misscellaneous flags.

pcihdr_index    dw      ?               ; base configuration index.
pcihdr_function dw      ?               ; handler function.
PCIHDR          ENDS

;***    PCIBAR  - PCI Base Address Allocation Entry.
;
;       The PCIBAR structure is used to describe a base address register
;       allocation request.  These structures are used to optimize the
;       PCI enumeration code to allow on the fly opportunistic allocation of
;       addresss space to PCI devices.

PCIBAR          STRUC
pcibar_device   dw      ?               ; requesting device.
pcibar_register dw      ?               ; BAR register being set.
pcibar_next     dw      ?               ; next request of same size in list.
PCIBAR          ENDS

;***    PCIALLOC - PCI Resource Allocation Map Entry.
;
;       The PCIALLOC structure creates an allocation map of PCI resources
;       as they are allocated.  There is always at least one PCIALLOC
;       structure in the temporary address space, followed by multiple
;       PCIBAR and PCIREGION structures that are specific to the bus
;       currently under enumeration.

PCIALLOC                STRUC
pcialloc_current        dw      ?       ; current PCI allocation structure.
pcialloc_nextfree       dw      ?       ; local next free address.
pcialloc_prior          dw      ?       ; prior PCI allocation structure.
pcialloc_buslimit       dw      ?       ; bus number limit.
pcialloc_topio          dw      ?       ; top of used I/O space.
pcialloc_botio          dw      ?       ; bottom of used I/O space.
pcialloc_top32          dd      ?       ; top of 32 bit address.
pcialloc_bot32          dd      ?       ; bottom of 32 bit address.
pcialloc_top64low       dd      ?       ; low portion, top 64 bit address.
pcialloc_top64high      dd      ?       ; high portion, top 64 bit address.
pcialloc_bot64low       dd      ?       ; low portion, bottom 64 bit address.
pcialloc_bot64high      dd      ?       ; high portoin, bottom 64 bit address.
pcialloc_iosizearray    dw      16 dup (?) ; I/O BAR size request array.
pcialloc_mem32sizearray dw      32 dup (?) ; 32 bit memory size array.
pcialloc_mem64sizearray dw      64 dup (?) ; 64 bit memory size array.
PCIALLOC                ENDS

;       The PCI_ALLOC_SEGMENT_OFFSET variable is used to calculate a value to
;       add to the scratch segment when calculating the segment address of the
;       resource allocation scratch pad.  The address used for the PCI
;       allocation scratch pad must not overlap with any other PCI allocation
;       information being maintained by the resource allocation code.  There
;       may be at most 256 busses in the system, so we will assume a maxumum
;       of 256 PCIBI entries in the PCIBI map.

PCI_ALLOC_SEGMENT_OFFSET = ((((SIZE PCIBI) * 256) + CONFIG_PCI_BM_OFFSET + 010fh) / 16)

OVERRIDE_PROCEDURE PciDisableVgaRoms
OVERRIDE_PROCEDURE PostPciScanVideo
OVERRIDE_PROCEDURE PciCopyTempIrqTbl
OVERRIDE_PROCEDURE PostPciEnumerate
OVERRIDE_PROCEDURE PciWriteByteNoStack
OVERRIDE_PROCEDURE PciWriteWordNoStack

;       Note that some designs override PciUpdatePciIrqTbl independantly.
;       When this happens, we must avoid overriding that procedure in this
;       package.  To do this, we detect if PciUpdatePciIrqTbl is overridden
;       already and set ORN_p_PciUpdateIrqTbl based on that detection.

IFNDEF OR_p_PciUpdatePciIrqTbl
OVERRIDE_PROCEDURE PciUpdatePciIrqTbl
ORN_p_PciUpdatePciIrqTbl = 1            ; signal override for this package.
ELSE
IFNDEF ORN_p_PciUpdatePciIrqTbl
ORN_p_PciUpdatePciIrqTbl = 0            ; signal override elsewhere.
ENDIF
ENDIF

OVERRIDE_PROCEDURE PciBridgeIntRoute
OVERRIDE_PROCEDURE PciVgaBackScan
OVERRIDE_PROCEDURE PciAssignResources
OVERRIDE_PROCEDURE GoProtPci
OVERRIDE_PROCEDURE GoRealPci
OVERRIDE_PROCEDURE MapRomPci
OVERRIDE_PROCEDURE ShadowRegionPci
OVERRIDE_PROCEDURE PciFindIrq
OVERRIDE_PROCEDURE PciConfigIrqs
OVERRIDE_PROCEDURE PostPciDisplayDevices
OVERRIDE_PROCEDURE PciFindNextDevice
OVERRIDE_PROCEDURE PciFindBridge

        IFNDEF  CONFIG_PCI_MEM_BASE_64_HI
CONFIG_PCI_MEM_BASE_64_HI =     000008000h ; high 32 bits of 64-bit base.
        ENDIF   ; (CONFIG_PCI_MEM_BASE_64_HI)

        IFNDEF  CONFIG_PCI_MEM_BASE_64_LO
CONFIG_PCI_MEM_BASE_64_LO =     000000000h ; low 32 bits of 64-bit base.
        ENDIF   ; (CONFIG_PCI_MEM_BASE_64_LO)

        IFNDEF  CONFIG_PCI_MEM_LIMIT_64_HI
CONFIG_PCI_MEM_LIMIT_64_HI =    00000ffffh ; high 32 bits of 64-bit limit.
        ENDIF   ; (CONFIG_PCI_MEM_LIMIT_64_HI)

        IFNDEF  CONFIG_PCI_MEM_LIMIT_64_LO
CONFIG_PCI_MEM_LIMIT_64_LO =    0ffffffffh ; low 32 bits of 64-bit limit.
        ENDIF   ; (CONFIG_PCI_MEM_LIMIT_64_LO)

        IFNDEF  CONFIG_PCI_MEM_BASE_32
        IF      (CONFIG_PCI_MEM_AVAIL LT CONFIG_PCI_MMIO_AVAIL)
CONFIG_PCI_MEM_BASE_32 = (CONFIG_PCI_MEM_AVAIL SHL 16)
        ELSE    ; (CONFIG_PCI_MEM_AVAIL LT CONFIG_PCI_MMIO_AVAIL)
CONFIG_PCI_MEM_BASE_32 = (CONFIG_PCI_MMIO_AVAIL SHL 16)
        ENDIF   ; (CONFIG_PCI_MEM_AVAIL LT CONFIG_PCI_MMIO_AVAIL)
        ENDIF   ; (CONFIG_PCI_MEM_BASE_32)

        IFNDEF  CONFIG_PCI_MEM_LIMIT_32
        IF      ((CONFIG_PCI_MEM_END NE 0) OR (CONFIG_PCI_MMIO_END NE 0))
        IF      (CONFIG_PCI_MEM_END GT CONFIG_PCI_MMIO_END)
CONFIG_PCI_MEM_LIMIT_32 = (CONFIG_PCI_MEM_END SHL 16)
        ELSE    ; (CONFIG_PCI_MEM_END GT CONFIG_PCI_MMIO_END)
CONFIG_PCI_MEM_LIMIT_32 = (CONFIG_PCI_MMIO_END SHL 16)
        ENDIF   ; (CONFIG_PCI_MEM_END GT CONFIG_PCI_MMIO_END)
        ELSE    ; ((CONFIG_PCI_MEM_END NE 0) OR (CONFIG_PCI_MMIO_END NE 0))
CONFIG_PCI_MEM_LIMIT_32 =       0febfffffh ; 32 bit limit address.
        ENDIF   ; ((CONFIG_PCI_MEM_END NE 0) OR (CONFIG_PCI_MMIO_END NE 0))
        ENDIF   ; (CONFIG_PCI_MEM_LIMIT_32)

        IFNDEF  CONFIG_PCI_IO_BASE_16
        IF      (CONFIG_PCI_IO_PORT_BOTTOM NE 04200h)
CONFIG_PCI_IO_BASE_16 = CONFIG_PCI_IO_PORT_BOTTOM
        ELSE    ; (CONFIG_PCI_IO_PORT_BOTTOM NE 04200h)
CONFIG_PCI_IO_BASE_16 =         04000h  ; 16 bit I/O port base.
        ENDIF   ; (CONFIG_PCI_IO_PORT_BOTTOM NE 04200h)
        ENDIF   ; (CONFIG_PCI_IO_BASE_16)

        IFNDEF  CONFIG_PCI_IO_LIMIT_16
        IF      (CONFIG_PCI_IO_PORT_BASE NE 0FC00h)
CONFIG_PCI_IO_LIMIT_16 = CONFIG_PCI_IO_PORT_BASE
        ELSE    ; (CONFIG_PCI_IO_PORT_BASE NE 0FC00h)
CONFIG_PCI_IO_LIMIT_16 =        0ffffh  ; 16 bit I/O port limit.
        ENDIF   ; (CONFIG_PCI_IO_PORT_BASE NE 0FC00h)
        ENDIF   ; (CONFIG_PCI_IO_LIMIT_16)

        IFNDEF  OPTION_PCI_HUGE_ASSIGNMENTS
OPTION_PCI_HUGE_ASSIGNMENTS =   0       ; allow assignments above 4 gigabytes.
        ENDIF   ; (OPTION_PCI_HUGE_ASSIGNMENTS)

        IFNDEF  CONFIG_PCI_BIST_TIMEOUT
CONFIG_PCI_BIST_TIMEOUT =       2       ; wait roughly 2 seconds for BIST.
        ENDIF   ; (CONFIG_PCI_BIST_TIMEOUT)

        IFNDEF  CONFIG_PCI_CARDBUS_MIN_IO
CONFIG_PCI_CARDBUS_MIN_IO =     01000h  ; minimum I/O assigned to CardBus.
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_IO)

        IFNDEF  CONFIG_PCI_PCIBRIDGE_MIN_IO
CONFIG_PCI_PCIBRIDGE_MIN_IO =   0       ; minimum I/O assigned to PCI bridges.
        ENDIF   ; (CONFIG_PCI_PCIBRIDGE_MIN_IO)

        IFNDEF  CONFIG_PCI_CARDBUS_MIN_MEM
CONFIG_PCI_CARDBUS_MIN_MEM =    0200000h; minimum memory assigned to CardBus.
        ENDIF   ; (CONFIG_PCI_CARDBUS_MIN_MEM)

        IFNDEF  CONFIG_PCI_PCIBRIDGE_MIN_MEM
CONFIG_PCI_PCIBRIDGE_MIN_MEM =  0       ; minimum memory assigned to PCI bridges.
        ENDIF   ; (CONFIG_PCI_PCIBRIDGE_MIN_MEM)
