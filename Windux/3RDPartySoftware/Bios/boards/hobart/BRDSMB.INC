;***    BRDSMB.INC -- Embedded BIOS System Management BIOS Board Module for
;                     AMD LXDB800 Development Platform.
;
;1.     Functional Description.
;       This include file defines board specific defaults for the SMBIOS
;       tables.  This implementation conforms to the System Management BIOS
;       Reference Specification, version 2.3.1.
;
;       This include file defines values for SMBIOS structures for the AMD
;       LXDB800 "Salsa" Development Platform.
;
;       Please refer to BOARDS\SALSA\SALSA.ASM for a detailed discussion of
;       motherboard capabilities versus BIOS implementation and support.
;
;2.     Modification History.
;       R. Spiegel      05/04/27.       #5.3, new for Salsa board.
;
;3.     NOTICE: Copyright (C) 2000-2005 General Software, Inc.
;
;       Define the values used in the SMBIOS structures.  There is a macro call
;       for each SMBIOS structure defined in this file.  Parameters to the
;       macro call are used to construct the SMBIOS structure field values
;       used when making the SMBIOS structure table.
;
;       There are many parameters for each macro call, and in general each
;       parameter has it's own line for readability.  Since there cannot be
;       comments on the macro call, definitions of these parameters are
;       provided before each macro call.  There are several things to be aware
;       of when passing values to the macros.
;
;       Only numeric values or strings can be passed through the structure
;       definition macros, not equations.  If a value needs to be calculated,
;       it should either be given a separate symbol before the macro call or
;       prefixed with the expression evaluator symbol (%).  For example, the
;       equation in:
;
;       SMB_DEFINE_STRUCTURE_X \
;         ((CONFIG_VALUE_ONE * CONFIG_VALUE_TWO) + 400h), \
;         0
;
;       cannot be passed directly as a macro parameter.  It must instead be
;       specified as one of the following:
;
;       BrdSmbValx = ((CONFIG_VALUE_ONE * CONFIG_VALUE_TWO) + 400h)
;
;       SMB_DEFINE_STRUCTURE_X \
;         BrdSmbValx, \
;         0
;
;       or
;
;       SMB_DEFINE_STRUCTURE_X \
;         %((CONFIG_VALUE_ONE * CONFIG_VALUE_TWO) + 400h), \
;         0
;
;       Also, all strings must be at least 5 characters long.  Any strings less
;       than 5 characters will be interpreted by the assembler as numeric data
;       and will produce build errors.  If a desired string is less than 5
;       characters, spaces can be used as padding.
;
;       A general knowledge of the SMBIOS spec is necessary for this SMBIOS
;       implementation.  For more details about SMBIOS structure fields/values,
;       see INC\SMBIOS.INC and the System Management BIOS Reference
;       Specification, v2.3.1.
;
;       The first SMBIOS structure, BIOS Information, is given below as an
;       example.  For more examples, see the file BOARDS\NOBOARD\BRDSMB.INC.
;
;       Define SMBIOS structure 0, BIOS Information.
;
;line 1: Macro call to define SMBIOS structure type 0.
;line 2: size of BIOS ROM (not BIOS image) in bytes (64k*(n+1)).
;          example: for a 128k BIOS ROM, use a value of n=1.
;line 3: BIOS characteristics (PCI, PCMCIA, Flash, etc)
;line 4: BIOS characteristics extension.
;
; Sample structure type 0 define:
;
;SMB_DEFINE_STRUCTURE_0 \
;       1, \
;       SMB_BIOSINFO_CHAR_NONE, \
;       0
;
;       Salsa defines.  BIOS ROM is 512K => 7.

BrdSmbVal1 =    (SMB_BIOSINFO_CHAR_DEFAULT OR SMB_BIOSINFO_CHAR_SOCKET)

SMB_DEFINE_STRUCTURE_0 \
        7, \
        BrdSmbVal1, \
        SMB_BIOSINFO_CHAREXT_DEFAULT

;       Define SMBIOS structure 1, System Information.

;line 1: Macro call to define SMBIOS structure type 1.
;line 2: default value, wake state.
;line 3: UUID (4 dwords, highest to lowest)
;        example: 4000300020001000h = 4000h, 3000h, 2000h, 1000h
;line 4: string 1: system manufacturer.
;line 5: string 2: system product name.
;line 6: string 3: system product version.
;line 7: string 4: system serial #.

SMB_DEFINE_STRUCTURE_1 \
        SMB_SYSINFO_WAKE_UNKNOWN, \
        0, 0, 0, 0, \
        'Advanced Micro Devices', \
        'AMD "LXDB800" Geode GX3/CS5535 Development System', \
        'No System Version', \
        'No System Serial Number'

;       Define SMBIOS structure 2, Base Board Information.

;line 1: Macro call to define SMBIOS structure type 2.
;line 2: string 1: board manufacturer.
;line 3: string 2: board product name.
;line 4: string 3: board version.
;line 5: string 4: board serial #.

SMB_DEFINE_STRUCTURE_2 \
        'AMD  ', \
        'LXDB800 Customer Development Platform', \
        'Board Revision 1.0', \
        'No Board Serial Number'

;       Define SMBIOS structure 3, System Enclosure.

;line 1: Macro call to define SMBIOS structure type 3.
;line 2: enclosure/chassis type.
;line 3: default value, state of enclosure on last boot.
;line 4: default value, state of power supply on last boot.
;line 5: default value, thermal state on last boot.
;line 6: default value, physical security status on last boot.
;line 7: BIOS or OEM specific information.
;line 8: string 1: enclosure manufacturer.
;line 9: string 2: enclosure version.
;line 10: string 3: enclosure serial number.
;line 11: string 4: enclosure asset tag number.

SMB_DEFINE_STRUCTURE_3 \
        SMB_SYSENC_ENCTYPE_OTHER, \
        SMB_SYSENC_BOOT_UNKNOWN, \
        SMB_SYSENC_BOOT_UNKNOWN, \
        SMB_SYSENC_BOOT_UNKNOWN, \
        SMB_SYSENC_SECUR_UNKNOWN, \
        0, \
        'No Enclosure Manufacturer', \
        'No Enclosure Version', \
        'No Enclosure Serial Number', \
        'No Enclosure Asset Tag'

;       Defaults for SMBIOS structure 4, Processor Information (1st processor).

;line 1: Macro call to define SMBIOS structure type 4.
;line 2: structure # of type 4 (0 for first Procinfo structure, 1 for 2nd, etc).
;line 3: processor type.
;line 4: processor family.
;line 5: default value, raw processor ID data.
;line 6: default value, voltage.
;line 7: default value, external clock frequency (MHz), 0=unkown.
;line 8: max internal processor speed supported by system (0=unknown).
;line 9: default value, current internal processor speed (0=unknown).
;line 10: default value, processor status.
;line 11: processor upgrade.
;line 12: cache structure # for this processor's L1 cache (0FFFFh = none).
;line 13: cache structure # for this processor's L2 cache (0FFFFh = none).
;line 14: cache structure # for this processor's L3 cache (0FFFFh = none).
;line 15: string 1: socket designation.
;line 16: string 2: processor manufacturer.
;line 17: string 3: processor version.
;line 18: string 4: serial number.
;line 19: string 5: asset tag number.
;line 20: string 6: part number.

BrdSmbVal2 =    (SMB_PROCINFO_STATUS_SOCKET OR SMB_PROCINFO_STATUS_CPU_EN)

SMB_DEFINE_STRUCTURE_4 \
        0, \
        SMB_PROCINFO_PROCTYPE_CPU, \
        SMB_PROCINFO_FAMILY_OTHER, \
        0, \
        SMB_PROCINFO_VOLT_50V, \
        33, \
        0, \
        0, \
        BrdSmbVal2, \
        SMB_PROCINFO_UPGRADE_UNKNOWN, \
        0, \
        1, \
        -1, \
        'No Socket', \
        'AMD Corporation', \
        'Geode GX3 Processor', \
        'No Serial#', \
        'No Asset Tag', \
        'No Part#'

;       Defaults for SMBIOS structure 7, Cache Information (1st cache struc).

;line 1: Macro call to define SMBIOS structure type 7.
;line 2: structure # of type 7 (0 for first Cacheinfo structure, 1 for 2nd, etc).
;line 3: default value, cache configuration.
;line 4: maximum cache size that can be installed.
;line 5: default value, currently installed cache size.
;line 6: supported SRAM types.
;line 7: default value, current SRAM type.
;line 8: default value, cache module speed (in ns), 0=unknown.
;line 9: supported error correction scheme.
;line 10: system cache type.
;line 11: associativity of cache.
;line 12: string 1: cache socket designation.

BrdSmbVal3 =    (SMB_CACHEINFO_CFG_MODE_WB OR SMB_CACHEINFO_CFG_EN OR \
        SMB_CACHEINFO_CFG_LOC_INT OR 000h)      ; 000h specifies L1 cache.

SMB_DEFINE_STRUCTURE_7 \
        0, \
        BrdSmbVal3, \
        32, \
        32, \
        SMB_CACHEINFO_SRAM_UNKNOWN, \
        SMB_CACHEINFO_SRAM_UNKNOWN, \
        0, \
        SMB_CACHEINFO_ERR_UNKNOWN, \
        SMB_CACHEINFO_CACHETYPE_UNKNOWN, \
        SMB_CACHEINFO_ASSOC_UNKNOWN, \
        'Processor L1 Cache'

BrdSmbVal4 =    (SMB_CACHEINFO_CFG_MODE_WB OR SMB_CACHEINFO_CFG_EN OR \
        SMB_CACHEINFO_CFG_LOC_INT OR 001h)      ; 001h specifies L2 cache.

SMB_DEFINE_STRUCTURE_7 \
        1, \
        BrdSmbVal4, \
        512, \
        512, \
        SMB_CACHEINFO_SRAM_UNKNOWN, \
        SMB_CACHEINFO_SRAM_UNKNOWN, \
        0, \
        SMB_CACHEINFO_ERR_UNKNOWN, \
        SMB_CACHEINFO_CACHETYPE_UNKNOWN, \
        SMB_CACHEINFO_ASSOC_UNKNOWN, \
        'Processor L2 Cache'

;       Defaults for SMBIOS structure 9, System Slots (1st slot).

;line 1: Macro call to define SMBIOS structure type 9.
;line 2: structure # of type 9 (0 for 1st Syslot structure, 1 for 2nd, etc).
;line 3: slot type.
;line 4: slot data bus width.
;line 5: default value, current usage.
;line 6: slot length.
;line 7: slot ID.
;line 8: slot characteristics 1.
;line 9: slot characteristics 2.
;line 10: string 1: slot designation.

;       PCI slot 1.

SMB_DEFINE_STRUCTURE_9 \
        0, \
        SMB_SYSLOT_STYPE_PCI, \
        SMB_SYSLOT_BUSWIDTH_32, \
        SMB_SYSLOT_CURR_UNKNOWN, \
        SMB_SYSLOT_SLEN_UNKNOWN, \
        0, \
        SMB_SYSLOT_CHAR1_33V, \
        0, \
        'PCI slot 1'

;       PCI slot 2.

SMB_DEFINE_STRUCTURE_9 \
        1, \
        SMB_SYSLOT_STYPE_PCI, \
        SMB_SYSLOT_BUSWIDTH_32, \
        SMB_SYSLOT_CURR_UNKNOWN, \
        SMB_SYSLOT_SLEN_UNKNOWN, \
        1, \
        SMB_SYSLOT_CHAR1_33V, \
        0, \
        'PCI slot 2'

;       PCI slot 3.

SMB_DEFINE_STRUCTURE_9 \
        2, \
        SMB_SYSLOT_STYPE_PCI, \
        SMB_SYSLOT_BUSWIDTH_32, \
        SMB_SYSLOT_CURR_UNKNOWN, \
        SMB_SYSLOT_SLEN_UNKNOWN, \
        2, \
        SMB_SYSLOT_CHAR1_33V, \
        0, \
        'PCI slot 3'

;       PCI slot 4.

SMB_DEFINE_STRUCTURE_9 \
        3, \
        SMB_SYSLOT_STYPE_PCI, \
        SMB_SYSLOT_BUSWIDTH_32, \
        SMB_SYSLOT_CURR_UNKNOWN, \
        SMB_SYSLOT_SLEN_UNKNOWN, \
        3, \
        SMB_SYSLOT_CHAR1_33V, \
        0, \
        'PCI slot 4'

;       Defaults for SMBIOS structure 16, Physical Memory Array (1st array).

;line 1: Macro call to define SMBIOS structure type 16.
;line 2: structure # of type 16 (0 for 1st PMA structure, 1 for 2nd, etc).
;line 3: default value, physical location of memory array.
;line 4: default value, function for which array is being used.
;line 5: primary error correction/detection.
;line 6: maximum memory capacity (KB).
;line 7: # of slots or sockets in this array.

;       Defaults for SMBIOS structure 16, Physical Memory Array (1st array).
;       System memory (RAM).  Default to all of addressible memory on the
;       Salsa board.  The exact value can be updated during POST.

BrdSmbVal5 =    (CONFIG_MAX_EXT_MEMORY+400h)

SMB_DEFINE_STRUCTURE_16 \
        0, \
        SMB_PMA_LOC_BOARD, \
        SMB_PMA_USE_SYS, \
        SMB_PMA_ERRDETECT_UNKNOWN, \
        BrdSmbVal5, \
        2

;       Defaults for SMBIOS structure 17, Memory Device (1st device).

;line 1: Macro call to define SMBIOS structure type 17.
;line 2: structure # of type 17 (0 for first MEMDEV structure, 1 for 2nd, etc).
;line 3: PMA structure number for this device.
;line 4: default value, total width in bits of this device.
;line 5: default value, data width in bits of this device.
;line 6: default value, size of device (bit 15=0=>MB, bit 15=1=>KB).
;line 7: form factor for this device.
;line 8: default value, set this memory device belongs to.
;line 9: default value, type of memory in this device.
;line 10: default value, details on this memory type.
;line 11: default value, speed of this device in MHz.
;line 12: string 1: device location.
;line 13: string 2: bank this device is on.
;line 14: string 3: device manufacturer.
;line 15: string 4: device serial number.
;line 16: string 5: asset tag.
;line 17: string 6: part number.

;       First SDRAM slot.  We don't know how this is populated yet; default
;       to unknown.

BrdSmbVal6 =    SMB_MEMDEV_WIDTH_UNKNOWN
BrdSmbVal7 =    SMB_MEMDEV_WIDTH_UNKNOWN
BrdSmbVal8 =    SMB_MEMDEV_WIDTH_UNKNOWN
BrdSmbVal9 =    SMB_MEMDEV_FORM_UNKNOWN
BrdSmbVal10 =   SMB_MEMDEV_SET_UNKNOWN
BrdSmbVal11 =   SMB_MEMDEV_MEMTYPE_UNKNOWN
BrdSmbVal12 =   SMB_MEMDEV_TYPEDTL_UNKNOWN
BrdSmbVal13 =   SMB_MEMDEV_SPEED_UNKNOWN

SMB_DEFINE_STRUCTURE_17 \
        0, \
        0, \
        BrdSmbVal6, \
        BrdSmbVal7, \
        BrdSmbVal8, \
        BrdSmbVal9, \
        BrdSmbVal10, \
        BrdSmbVal11, \
        BrdSmbVal12, \
        BrdSmbVal13, \
        'No Device Location', \
        'No Bank', \
        'No Device Manufacturer', \
        'No Device Serial Number', \
        'No Device Asset Tag', \
        'No Device Part Number'

;       Second SDRAM slot.  We don't know how this is populated yet; default
;       to unknown.

SMB_DEFINE_STRUCTURE_17 \
        1, \
        0, \
        BrdSmbVal6, \
        BrdSmbVal7, \
        BrdSmbVal8, \
        BrdSmbVal9, \
        BrdSmbVal10, \
        BrdSmbVal11, \
        BrdSmbVal12, \
        BrdSmbVal13, \
        'No Device Location', \
        'No Bank', \
        'No Device Manufacturer', \
        'No Device Serial Number', \
        'No Device Asset Tag', \
        'No Device Part Number'

;       Defaults for SMBIOS structure 19, Memory Array Mapped Address (1st range).

;line 1: Macro call to define SMBIOS structure type 19.
;line 2: structure # of type 19 (0 for first MAMA structure, 1 for 2nd, etc).
;line 3: physical starting address.
;line 4: physical ending address.
;line 5: PMA structure # for this entry (line 2 of associated PMA structure).
;line 6: # of mem devices in one row of memory for this address partition.

;       Memory address range for system RAM.

SMB_DEFINE_STRUCTURE_19 \
        0, \
        0, \
        %((CONFIG_MAX_EXT_MEMORY + 400h)*400h), \
        0, \
        1

;       Defaults for SMBIOS structure 20, Memory Device Mapped Address (1st range).

;line 1: Macro call to define SMBIOS structure type 20.
;line 2: structure # of type 20 (0 for first MDMA structure, 1 for 2nd, etc).
;line 3: physical starting address.
;line 4: physical ending address.
;line 5: memory device structure # for this structure.
;line 6: MAMA structure # this device is mapped to.
;line 7: default value, partition row position of mem device.
;line 8: default value, interleave position of this device.
;line 9: default value, interleaved data depth.

;       First RAM device (slot 0).
;       MDMA struc #0, unknown start & end addresses, MEMDEV struc #0, MAMA
;       struc #0, row position 1 (only device in row), non-interleaved, non-
;       interleaved.

SMB_DEFINE_STRUCTURE_20 \
        0, \
        0, \
        0, \
        0, \
        0, \
        1, \
        0, \
        0

;       Second RAM device (slot 1).
;       MDMA struc #1, unknown start & end addresses, MEMDEV struc #1, MAMA
;       struc #0, row position 1 (only device in row), non-interleaved, non-
;       interleaved.

SMB_DEFINE_STRUCTURE_20 \
        1, \
        0, \
        0, \
        1, \
        0, \
        1, \
        0, \
        0

;       Defaults for SMBIOS structure 32, System Boot Information.

;line 1: Macro call to define SMBIOS structure type 32.
;line 2: default value, system boot status.

SMB_DEFINE_STRUCTURE_32 \
        SMB_BOOTINFO_STATUS_NOERR

