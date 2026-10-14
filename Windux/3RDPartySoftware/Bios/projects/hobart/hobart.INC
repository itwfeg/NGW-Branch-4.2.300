;***    SALSA.INC -- Embedded BIOS Meta Include File, AMD "SALSA" Dev Platform.
;
;1.     Functional Description.
;       This include file defines CONFIG.INC & OPTIONS.INC overrides
;       for the AMD "SALSA" platform, known externally as the AMD LXDB800
;       (Geode GX3/CS5536) Development Platform.
;
;       The AMD LXDB800 Development Platform showcases the scalable GX3
;       integrated processor and CS5535 I/O companion device for the WebPAD
;       and Thin Client markets. The LXDB800 was developed by AMD to enable
;       developers to write and test code and simulate hardware for a variety
;       of Geode GX3 designs in a stable and observable environment.
;
;       In order to demonstrate and expand the capabilities of the LXDB800
;       platform and the GX3 processor, General Software has replaced the 16-bit
;       proprietary VSA2 with the full 32-bit capabilities of Firmbase(R).
;
;       Standard Firmbase support is included for:
;       - Legacy USB keyboard and mouse
;       - USB Mass Storage for HDD, FDD, CDROM, CF readers, DOK, Zip drives
;         and more
;       - High-availability framework
;       GX3 specific Firmbase components include:
;       - SoftVGA
;       - Hardware emulation (Virtual PCI)
;       - Power management
;
;       The development kit consists of an evaluation board with the GX3
;       processor bonded for either CRT or TFT displays, technical documentation,
;       and expansion hardware. The LXDB800 platform allows the GX3 processor, the
;       memory, and PCI frequency to be scaled respectivelly from 166MHz to 600MHz
;       from 166MHz to 400MHz  and 33MHz to 66MHz through Custom Setup.
;
;       The baseboard's 184-pin DDR SDRAM slot support up to 1GB of memory.
;       The one 3.3V PCI expansion slot support one PCI device at 33MHz or
;       66MHz. Additional connectors include, one 40-pin IDE header supporting
;       up to two UDMA4 devices, one floppy header, one serial conector, one parallel
;       conector, one VGA connector, three USB 1.1 ports for Legacy USB and
;       boot from USB Mass Storage devices, one 40-pin TFT flat-panel header,
;       audio jacks, and a large selection of configuration jumpers.
;
;       These are the features implemented in this adaptation:
;       - APM (power button, soft off);
;       - programable CPU and memory frequencies;
;       - USB legacy devices;
;       - USB mass storage devices;
;       - PS2 keyboard and mouse detection and support;
;       - configurable support for 4 COM ports: board has four 16650 compatible
;         UARTS and two IrDA UART. Any of those six UARTS can be assigned to any
;         COM port;
;       - Supports video with integrated video controller or with add-on PCI
;         video card; or supports console redirection over serial port;
;       - automatic multiple SIO detection, configuration, and power management;
;       - Firmbase instrumentation selectable to COM1 or COM2;
;       - UDMA 0, 1, 2, 3, and 4;
;       - PnP (provided in GX3 package);
;       - PCI IRQ routing;
;       - Shadow control in 16K blocks;
;       - legacy devices such as floppy, parallel port, DMA, PIC, timers;
;
;2.     Modification History.
;       R. Spiegel      05/04/27.       #5.3, new.
;       R. Spiegel      05/06/08.       #5.3.3, enabled power and ACPI, fixed shadow.
;       R. Spiegel      05/06/09.       #5.3.3, enabled early cache.
;       R. Spiegel      05/06/21.       #5.3.3, default SPD is OEM, fixed to DIM SPD.
;       R. Spiegel      05/07/13.       #5.3.3, added Firmbase feature defaults.
;       R. Spiegel      05/07/14.       #5.3.3, display build # in BIOS_LICENSEE.
;       R. Spiegel      05/09/15.       #5.3.3, new value for GLCP1_DEL_CTRLS
;       E. Khoruzhenko  05/09/22.       #5.3.3, added E820UPD package and USB20 option.
;       E. Khoruzhenko  05/10/11.       #5.3.3, updated BIOS_LICENSEE string.
;
;3.     NOTICE:  Copyright (C) 2003-2005 General Software, Inc.

;*******************************************************************************
;*      The following equates select the board, chipset and CPU modules        *
;*      to use, as well as the BIOS licensee string.                           *
;*******************************************************************************

CPUCLASS        EQU     <GX3>           ; selected CPU module.
CHIPSET         EQU     <GX3>           ; selected chipset module.
BOARD           EQU     <hobart>         ; selected board module.
BIOS_LICENSEE   EQU     'AMD LX DB800 Development Platform, ', CONFIG_BOARD_BUILD_STR

;*******************************************************************************
;*      Evaluation board messages and screens.                                 *
;*******************************************************************************

OPTION_SUPPORT_DEMO_MSG         = 1     ; display extra message for demos.
OPTION_SUPPORT_REGISTRATION     = 0     ; enable platform registration.
CONFIG_REGSCREEN_DELAY          = 10    ; delay this many seconds during POST.

PLATFORM_ID     AMDSAL1                 ; AMD SALSA, build 1.
OPTION_SMM_HTTPSRV              = 1     ; support Firmbase HTTPSRV proxy server.

;*******************************************************************************
;*      CPU configuration parameters.                                          *
;*******************************************************************************

CONFIG_CPU_TYPE                 = CPU_586
OPTION_SUPPORT_NPX              = 1     ; build numeric coprocessor code.

;*******************************************************************************
;*      BIOS size and segmentation configuration parameters.                   *
;*******************************************************************************

CONFIG_SYSTEM_BIOS_SIZE         = 128   ; set BIOS size to 128k.
OPTION_SUPPORT_MINI_DOS         = 0     ; build code to boot Mini-DOS in ROM.
CONFIG_HIGH_ROM_SCAN            = 0DC00h ; 1st seg addr outside ROM scan area.

; It is suggested that you don't relocate any features manually unless
; autorelocation does not work for your particular project.  Try and let
; the BIOS build take care of relocation automatically if possible. Please
; note that the 'USERx' segments are not affected by autorelocation.
;
;                       Feature         Location
;                       --------------- ---------------
  RELOCATE_FEATURE      ATA,            SEG_E000
  RELOCATE_FEATURE      BRO,            SEG_E000
  RELOCATE_FEATURE      BOOTBIN,        SEG_E000
  RELOCATE_FEATURE      DEBUG,          SEG_E000
  RELOCATE_FEATURE      DIAG,           SEG_E000
  RELOCATE_FEATURE      MEMMGR,         SEG_E000
  RELOCATE_FEATURE      PCI,            SEG_L1000
  RELOCATE_FEATURE      PNP,            SEG_E000
  RELOCATE_FEATURE      REGSCR,         SEG_E000
  RELOCATE_FEATURE      SETUP,          SEG_L1000
  RELOCATE_FEATURE      SMB,            SEG_E000
  RELOCATE_FEATURE      PMM,            SEG_E000
  RELOCATE_FEATURE      SMM,            SEG_E000     ; core SMM relocation.
  RELOCATE_FEATURE      SMP,            SEG_E000
  RELOCATE_FEATURE      SPLASH,         SEG_L1000
  RELOCATE_FEATURE      SIO,            SEG_E000
  RELOCATE_FEATURE      USER0,          SEG_E000
  RELOCATE_FEATURE      USER1,          SEG_E000
  RELOCATE_FEATURE      USER2,          SEG_E000
  RELOCATE_FEATURE      USER3,          SEG_E000

OPTION_SUPPORT_LOW_RELOCATION   = 1     ; support relocating BIOS code low.

;*******************************************************************************
;*      PCI support and related options.                                       *
;*******************************************************************************

PACKAGE PCI534_B 
CONFIG_PCI_MEM_BASE_32  =       080000000h ; 32 bit base address. 
CONFIG_PCI_MEM_LIMIT_32 =       0efffffffh ; 32 bit limit address. 
CONFIG_PCI_IO_LIMIT_16  =       0fcffh     ; 16 bit I/O port limit. 

OPTION_SUPPORT_PCI              = 1     ; build code to support PCI & PCIAPI.
OPTION_SUPPORT_BIOS32           = 1     ; supports separate code for BIOS32.

;                                 FEDCBA9876543210  ; IRQ bit helper line.
CONFIG_PCI_IRQ_BITMAP           = 0000110000100000b ; allowed IRQs for PCI use.
CONFIG_PCI_LATENCY              = 040h   ; latency timers value.
CONFIG_PCI_IO_PORT_BASE         = 0fc00h ; first I/O range assigned top down.
CONFIG_PCI_MEM_AVAIL            = 0c000h ; phys addr - prefetch mem region.
CONFIG_PCI_MMIO_AVAIL           = 0e000h ; phys addr - non-prefetch mem region.
CONFIG_PCI_ROM_MAP              = 0ff00h ; ROM ext map phys addr (high 16 bits).

;*******************************************************************************
;*      Memory configuration parameters.                                       *
;*******************************************************************************

CONFIG_MAX_EXT_MEMORY           = ((1024-1)*16) ; 1GB limit.
CONFIG_TESTBASE_SIZE            = 16    ; low memory test size in KB.
CONFIG_WAIT_COUNT               = 4     ; delay between block checks.
CONFIG_HIGH_ROM_SCAN            = 0dc00h ; 1st seg outside scan range (SMM).

;       Regions of address space to reserve from the address manager during
;       PostInitAdmgr.  Regions must be reserved when non-RAM regions of
;       address space are present in the system, such as flash arrays or ROMs
;       (excluding BIOS ROMs at the top of the address space and just below
;       the 1MB boundary).
;
;                Address     Address      Length (KB)
;                High        Low
;                ----------  -----------  ---------
  ADDRESS_REGION 000000000h, 0000dc000h, 00000010h ; 16 KB reserved for SMM.
  ADDRESS_REGION 000000000h, 0f0000000h, 0003f000h ; roughly 256MB reserved for internal decodes.
 
;*******************************************************************************
;*      Media Control Layer configuration parameters.                          *
;*******************************************************************************

;       Note that we have 3 overlapping regions defined in the media region
;       table.  The first region is the only one that will be called directly
;       by the media layer.  In this case, a "User" media driver is being
;       used to identify the type of Firmware hub and it routes calls to the
;       proper Flash specific driver.

;               Starting   Ending      Technology
;               Phys Addr  Phys Addr   Driver Name
;               ---------  ----------  -----------
  MEDIA_REGION 0fff00000h, 0ffffffffh, User     ; Part identifier and router.
  MEDIA_REGION 0fff00000h, 0ffffffffh, Amd8_1   ; SST uses AMD Flash algorithm.
  MEDIA_REGION 0fff00000h, 0ffffffffh, Int8_1   ; Intel Flash algorithm.

;*******************************************************************************
;*      File system table and related parameters.                              *
;*******************************************************************************

;       !!! NOTE !!!
;       CDROM boot requires the presence of a floppy drive FILE_SYSTEM entry.
;       This is necessary because the bootable CDROM is emulated as a floppy
;       disk.

;               Type  Device    Start addr  Length      SETUP name (unique)
;               ----  --------  ----------  ----------  ------------------
  FILE_SYSTEM   Soft, Floppy,           0h,         0h, "Floppy 0        "
  FILE_SYSTEM   Soft, Floppy,           1h,         0h, "Floppy 1        "
  FILE_SYSTEM   Soft, Fb,       000020000h,         0h, "USB Floppy      "
  FILE_SYSTEM   Hard, Ide,              0h,         0h, "Ide 0/Pri Master"
  FILE_SYSTEM   Hard, Ide,              1h,         0h, "Ide 1/Pri Slave "
  FILE_SYSTEM   Hard, Fb,       000040000h,         0h, "USB Hard Drive  "
  FILE_SYSTEM   Auto, Cdrom,    001f00000h, 00e0003f6h, "CDROM Pri Master"
  FILE_SYSTEM   Auto, Cdrom,    001f00001h, 00e0103f6h, "CDROM Pri Slave "

;       ATA/IDE related options.

OPTION_IDE_ADVANCED_MODES       = 1     ; enable support for advanced transfer modes.

;       USB Mass Storage related options.

OPTION_SUPPORT_USB_CDROM        = 1     ; enable USB CDROM support.

;       Init Loop Delay Values / Retry Values for ATA/ATAPI initialization.

CONFIG_CDROM_CHK_RETRIES        = 30    ; number of retries of media check.
CONFIG_WAIT_ATA_RESET           = 30    ; ATAPI Device Reset cmd timeout (sec).
CONFIG_WAIT_IDE_INIT            = 30     ; IDE Soft Reset protocol timeout (sec).

;       IDE controller Bus Master UDMA settings. On this platform buffers for
;       UDMA transfers must be DWORD alligned.

OPTION_IDE_BMDMA                = 1     ; enable PCI bus master DMA support.
CONFIG_IDE_BMDMA_ALIGN          = 4     ; required buffer align for bus master.

OPTION_CMOS_IDE_P_UDMA          = 1     ; enable/disable UDMA on primary channel.
OPTION_CMOS_IDE_S_UDMA          = 0     ; enable/disable UDMA on secondary channel.

;       IDE related CMOS defaults.

CONFIG_CMOS_FLOPPY_0            = DRIVE_144  ; device type (see DEFINES.INC).
CONFIG_CMOS_FLOPPY_1            = DRIVE_NONE ; device type (see DEFINES.INC).
CONFIG_CMOS_IDE_0               = IDE_LBA    ; drive type for 1st IDE.
CONFIG_CMOS_IDE_1               = IDE_LBA    ; drive type for 2nd IDE.
CONFIG_CMOS_IDE_2               = IDE_LBA    ; drive type for 3rd IDE.
CONFIG_CMOS_IDE_3               = IDE_LBA    ; drive type for 4th IDE.

CONFIG_CMOS_BOOT_0              = BOOT_DRIVEC ; 1st boot action.
CONFIG_CMOS_BOOT_1              = BOOT_DRIVED  ; 2nd boot action.
CONFIG_CMOS_BOOT_2              = BOOT_BROWSER  ; 3rd boot action.

CONFIG_CMOS_ASSIGN_A            = 0     ; logical drive A: assignment.
CONFIG_CMOS_ASSIGN_C            = 6     ; logical drive C: assignment. (USB Hard drive)
CONFIG_CMOS_ASSIGN_D            = 4     ; logical drive D: assignment. (ide1 pri/master)


;*******************************************************************************
;*      Windows CE and boot image configuration related parameters.            *
;*******************************************************************************

OPTION_SUPPORT_WINCE            = 0     ; enable WINCE jump entry.
OPTION_CMOS_LOAD_WINCE          = 0     ; 1=set default to load Windows CE.
CONFIG_BOOTBIN_CACHE_SECS       = 48    ; number of sectors to allocate for cache.
CONFIG_WINCE_BAUD               = 1     ; set debugging port to 115200 baud.
OPTION_WINCE_CLR_MEM            = 1     ; set to clear memory before load WinCE.

CONFIG_WINCE_CLR_MEM_START      = 00200000h ; start clear memory addr, 0=default 1MB.
CONFIG_WINCE_CLR_MEM_SIZE       = 02000000h ; memory size to clear, 0=all ext memory.

;               File Name       Type   Start addr  Entry Pt.
;               --------------  -----  ----------  ----------
  LOAD_IMAGE    "NK.BIN",       WinCe, 000000000h, 000000000h

;*******************************************************************************
;*      Cache configuration parameters.                                        *
;*******************************************************************************

;       Note that the caching code in the CPU module is disabled.  This was
;       done because the code to support the CPU cache is actually in the
;       board and chipset modules for the board.

OPTION_SUPPORT_CACHE            = 1     ; build L2 cache support (not CPU cache).
OPTION_CACHE_BOARD              = 0     ; use board module call for ext cache.
OPTION_CACHE_CHIPSET            = 1     ; used chipset call for cache control.
OPTION_CACHE_EARLY              = 1     ; turn on cache early in POST for performance.

;*******************************************************************************
;*      8042, A20, reboot, and I/O timing configuration parameters.            *
;*******************************************************************************

;       A20 related options.

OPTION_A20_8042                 = 0     ; program 8042 to en/dis A20.
OPTION_A20_FAILMEM              = 0     ; ignore extended mem if A20 fails.
OPTION_A20_PORT92               = 0     ; use I/O port 92h to en/dis A20.
OPTION_A20_CHIPSET              = 1     ; program chipset to enab/disab A20.

;       Reboot options.

OPTION_REBOOT_8042              = 0     ; reboot by pulsing 8042 line.
OPTION_REBOOT_BOARD             = 1     ; reboot with board module call.
OPTION_REBOOT_PORT92            = 1     ; set bit 0 in port 92 to reboot.
OPTION_REBOOT_CHIPSET           = 1     ; reboot with chipset module.

;       Enable 7us wait after reading 8042 status register.

OPTION_8042_PS2                 = 1

;       Amount of "time" to wait for 8042 controller.

CONFIG_WAIT_8042                = 3000h

;       Amount of "time" to wait for 8042 controller to settle.

CONFIG_SETTLE_8042              = 3000h

CONFIG_POWER_ON_DELAY           = 0     ; # iterations through a loop $ loop.

;*******************************************************************************
;*      Keyboard and Mouse related parameters.                                 *
;*******************************************************************************

OPTION_SUPPORT_PS2MOUSE         = 0     ; build code to support a PS/2 mouse.
CONFIG_PS2_MOUSE_IRQ            = 12    ; IRQ number used by the PS/2 mouse.
CONFIG_PS2_MOUSE_LOOP           = 00f00h ; wait for 8042 to report status.

OPTION_SUPPORT_KEYBOARD         = 1     ; build PC/XT or PC/AT kbd support code.
OPTION_KEYBOARD_CHIPSET         = 0     ; enable chipset keyboard code.
OPTION_KEYBOARD_MATRIX          = 0     ; handle PrtSrn/Sysreq/Pause/Brk differently.
OPTION_KEYBOARD_PCAT            = 1     ; use PC/AT style keyboard.
OPTION_KEYBOARD_PCXT            = 0     ; use PC/XT style keyboard.

OPTION_SETUP_SHAD_C000          = 1     ; C000 segment shadow option.
OPTION_SETUP_SHAD_C400          = 1     ; C400 segment shadow option.
OPTION_SETUP_SHAD_C800          = 1     ; C800 segment shadow option.
OPTION_SETUP_SHAD_CC00          = 1     ; CC00 segment shadow option.
OPTION_SETUP_SHAD_D000          = 1     ; D000 segment shadow option.
OPTION_SETUP_SHAD_D400          = 1     ; D400 segment shadow option.
OPTION_SETUP_SHAD_D800          = 1     ; D800 segment shadow option.
OPTION_SETUP_SHAD_DC00          = 1     ; DC00 segment shadow option.
OPTION_SETUP_SHAD_E000          = 1     ; E000 segment shadow option.
OPTION_SETUP_SHAD_E400          = 1     ; E400 segment shadow option.
OPTION_SETUP_SHAD_E800          = 1     ; E800 segment shadow option.
OPTION_SETUP_SHAD_EC00          = 1     ; EC00 segment shadow option.
OPTION_SETUP_SHAD_F000          = 1     ; F000 segment shadow option.

OPTION_CMOS_SHADOW_DC00         = 1     ; enable shadowing at 0DC00h segment.

;*******************************************************************************
;*      Video related parameters.                                              *
;*******************************************************************************

OPTION_SUPPORT_VIDEO            = 1     ; build code to support mono/color/ega.
OPTION_SUPPORT_VIDEO_BOARDS     = 1     ; build code to support video boards.

OPTION_VIDEO_6845               = 1     ; use 6845 mono/color CRT controller.
OPTION_VIDEO_MODE_REDIR         = 0     ; redirect video mode calls.
OPTION_VIDEO_STDFONT            = 1     ; use standard PC/AT compatible fonts.
OPTION_VIDEO_VIDEOMEM           = 1     ; video memory available (test enabled).

;*******************************************************************************
;*      Console redirection and related parameters.                            *
;*******************************************************************************

OPTION_SUPPORT_CON_REDIRECTOR   = 0     ; build console redirection code.
OPTION_CON_REDIR_AUTO           = 1     ; 1=enable automatic console redirection.
CONFIG_CON_REDIR_DEBUG          = 1     ; (BCPA) 0=use PC video/keyboard.
CONFIG_CON_REDIR_SETUP          = 1     ; (BCPA) 0=use PC video/keyboard.
CONFIG_CON_REDIR_STD            = 1     ; (BCPA) 0=use PC video/keyboard.
CONFIG_CON_REDIR_DELAY          = 0     ; do not delay for console redirection.

OPTION_HARDERR_VIDEO            = 0     ; 1=fail POST if video test fails.
OPTION_VIDEO_DUPLICATE          = 0     ; copy redirected INT 10h output to CRT.

;*******************************************************************************
;*      PMM, Plug-n-Play, and SMBIOS (DMI) configuration.                      *
;*******************************************************************************

OPTION_SUPPORT_PMM              = 1     ; build POST memory manager.
OPTION_CMOS_PMM                 = 0     ; enable/disable PMM by default.

OPTION_SUPPORT_SMBIOS           = 1     ; build SMBIOS tables and code.
OPTION_CMOS_SMBIOS              = 1     ; enable/disable SMBIOS by default.

OPTION_SUPPORT_PNP              = 1     ; build Plug-n-Play manager.
OPTION_CMOS_PNP                 = 1
OPTION_CMOS_PNPOS               = 1

OPTION_PNP_API                  = 1     ; enable ISA API functions.
OPTION_PNP_ISA                  = 0     ; enable ISA PnP device isolation.

;       Enable any of the following options to create PnP device nodes
;       for SYSTEM BOARD devices.  That is, ONLY devices that are built
;       into the system's mainboard, NOT PnP-aware devices.  These controls
;       enable code in the core BIOS that calls PnP manager APIs to create
;       system device nodes.

;       !!! Note !!!
;       The OEM can disable these default core devnodes and then replace them
;       in the board module BoardInit4, BoardInit6, or BoardInit8 procedures
;       with PnP manager API calls to create custom system device nodes.

OPTION_PNP_NODE_8042            = 0     ; create ISA 8042 devnode(60h,64h/IRQ1).
OPTION_PNP_NODE_PS2MOUSE        = 0     ; create ISA PS/2 mouse device node.

;*******************************************************************************
;*      System Management Mode (SMM) configuration.                            *
;*******************************************************************************

OPTION_SUPPORT_SMM              = 1     ; build SMM (Firmbase) support.
OPTION_CMOS_SMM                 = 1     ; enable/disable SMM by default.

OPTION_SMM_LOCK_SMRAM           = 0     ; lock SMRAM after initialization.
CONFIG_SMM_POOL_RESERVE         = 4096  ; kilobytes to reserve for Firmbase.
OPTION_CMOS_FB_DEBUG            = 0     ; enable/disable Firmbase instrumentation.
CONFIG_CMOS_FIRMBASE_CONSOLE    = 1     ; FB console (0=none, 1=3f8h, 2=2f8h, 3=OEM).

;       These SMM parameters override the standard core definitions.
;       See CSPM SMMCTRL.ASM documentation for more details.

CONFIG_SMM_BASE                 = 0d000h ; seg ptr, SMM base address segment.
CONFIG_SMM_ENTRY                = 0c000h ; ofs ptr, code entry on SMM.
CONFIG_SMM_STACK16              = 0e500h ; ofs ptr, TOS rel to SMM_BASE for 16bit stack.
CONFIG_SMM_STACK32              = 0fc00h ; ofs ptr, TOS rel to SMM_BASE for 32bit stack.
CONFIG_SMM_DATA                 = 0fd00h ; ofs ptr, SMMDATA relative to SMM_BASE.
CONFIG_SMM_SAVE                 = 0fe00h ; ofs ptr, SMM save state area.

;*******************************************************************************
;*      Multiprocessing configuration.                                         *
;*******************************************************************************

OPTION_SUPPORT_SMP              = 0     ; build Symmetric Multi Processing code.
OPTION_CMOS_SMP                 = 0     ; enable/disable SMP by default.

;*******************************************************************************
;*      Power management configuration.                                        *
;*******************************************************************************

;       Advanced Configuration & Power Interface options.

OPTION_SUPPORT_ACPI             = 1     ; build code to support ACPI.
OPTION_CMOS_ACPI                = 1     ; enable/disable ACPI by default.

OPTION_ACPI_LEVEL_20            = 0     ; 0=support ACPI 1.0, 1=support ACPI 2.0.

;       Advanced Power Management options.

OPTION_SUPPORT_APM              = 1     ; build code to support APM interface.
OPTION_CMOS_APM                 = 1     ; enable/disable APM by default.
OPTION_APM_CPU_BUSY             = 0     ; OS usually does not call CpuBusy function.

;       This bitmask defines the IRQs that are allowed to wake up the system.

;                               FEDCBA9876543210  ; IRQ bit helper line.
CONFIG_APM_WAKE_IRQS    =       0000000000000010b ; allow IRQ 1 (keyboard).

; APM Device Class                Number of devices in a class
; ----------------------          ----------------------------

; APM_CLASS_SYSTEM_DEV            = 2     ; system devices (APM BIOS, system as a whole).
; APM_CLASS_DISPLAY_DEV           = 1     ; display adapters.
; APM_CLASS_STORAGE_DEV           = 2     ; hard drives.
; APM_CLASS_PARALLEL_DEV          = 2     ; parallel ports.
; APM_CLASS_SERIAL_DEV            = 4     ; serial ports.
; APM_CLASS_NETWORK_DEV           = 0     ; network adapters.
; APM_CLASS_PCMCIA_DEV            = 0     ; PCMCIA cards.
; APM_CLASS_BATTERY_DEV           = 0     ; batteries.
; APM_CLASS_OEM_DEV               = 0     ; OEM defined devices.

OPTION_SUPPORT_POWER            = 1     ; support power management.

;*******************************************************************************
;*      Manufacturing mode related parameters.                                 *
;*******************************************************************************

OPTION_SUPPORT_MFGMODE          = 0     ; build code to support manufacturing mode.

;       Manufacturing mode baud rate setting.
;       0 = 115200 baud.
;       1 = 57600  baud.
;       2 = 38400  baud.
;       3 = 28800  baud.
;       4 = 19200  baud.
;       5 = 9600   baud.

CONFIG_MFGMODE_BAUD             = 1     ; 57600 baud connection.
CONFIG_MFGMODE_PROGRESS_PORT    = 80h   ; I/O port to write MFGMODE codes to.
CONFIG_MFGMODE_TIMEOUT          = 36    ; 18.2Hz ticks 'till msg timeout.

;*******************************************************************************
;*      Setup screen configuration.                                            *
;*******************************************************************************

OPTION_QUERY_ENTERSETUP         = 1     ; enable query to enter setup if errors.

OPTION_SETUP_CUSTOM             = 1     ; build SETUP screen for custom options.
OPTION_SETUP_DEBUGGER           = 1     ; build SETUP option to break into debugger.
OPTION_SETUP_DEMO               = 1     ; build SETUP screen for demo boards.
OPTION_SETUP_DIAGNOSTICS        = 1     ; build SETUP screen for std diagnostics.
OPTION_SETUP_FEATURES           = 1     ; build SETUP screen for the features.
OPTION_SETUP_MFGMODE            = 0     ; build SETUP screen for manufacturing mode.
OPTION_SETUP_OEM                = 0     ; build SETUP screen for OEM configuration.
OPTION_SETUP_PASSWORD           = 0     ; build SETUP password screen.
OPTION_SETUP_PNP                = 1     ; build SETUP screen for PnP configuration.
OPTION_SETUP_RAMDISK            = 0     ; build SETUP option to format RAM disk.
OPTION_SETUP_REGSCREEN          = 1     ; build SETUP eval board registration.
OPTION_SETUP_RFDDISK            = 0     ; build SETUP option to format RFD disk.
OPTION_SETUP_SHADOW             = 1     ; build SETUP screen for shadowing/cache.

;*******************************************************************************
;*      Debugger configuration.                                                *
;*******************************************************************************

OPTION_SUPPORT_DEBUGGER         = 0     ; enable/disable built-in debugger.

OPTION_QUERY_FIRMBASE           = 0     ; enable query to load Firmbase.
OPTION_QUERY_DEBUG              = 0     ; enable query to enter debugger in POST.

OPTION_DEBUG_ADMGR              = 0     ; commands for address manager.
OPTION_DEBUG_ASSEMBLY           = 1     ; enable opcode disassembler in debugger.
OPTION_DEBUG_BIOS               = 1     ; enable BIOS-related commands.
OPTION_DEBUG_CHIPSET            = 0     ; build code to support CSW and CSR commands.
OPTION_DEBUG_FLASH              = 0     ; enable Flash debugging support.
OPTION_DEBUG_HELP_ADMGR         = 0     ; verbose help for addr mgr dbg cmds.
OPTION_DEBUG_HELP_DETAIL        = 0     ; enable detail help display.
OPTION_DEBUG_HELP_SUMMARY       = 1     ; enable summary help display.
OPTION_DEBUG_HOTKEY             = 1     ; use CTRL+SHIFT to break-into the debugger.
OPTION_DEBUG_INTERNAL           = 0     ; enable internal GS debugging.
OPTION_DEBUG_MEMMGR             = 0     ; enable memory manager debugger commands.
OPTION_DEBUG_NMI                = 0     ; enable NMI breakins to debugger.
OPTION_DEBUG_OLDCMDNAMES        = 1     ; enable obsolete debug command names.
OPTION_DEBUG_PCMCIA             = 0     ; enable PCMCIA debugger commands.
OPTION_DEBUG_PNP                = 0     ; enable general PnP debugging commands.
OPTION_DEBUG_PNPCDRBMP          = 0     ; enable PnP CDR bitmap commands.
OPTION_DEBUG_PNPISA             = 0     ; PnP ISA debugging commands.
OPTION_DEBUG_REHOOK             = 0     ; build debug vectors REHOOK command.
OPTION_DEBUG_SHADOW             = 0     ; enable shadow RAM debugger commands.
OPTION_DEBUG_SMP                = 0     ; enable multiprocessing debugging.
OPTION_DEBUG_WATCHINT           = 0     ; enable software interrupt watchpoints.
OPTION_DEBUG_SECURITY           = 0     ; enable security debugging.
OPTION_DEBUG_SIO                = 0     ; enable SIO debugging commands.
OPTION_DEBUG_QUICK_BOOT         = 0     ; enable quick-boot debugging.

;*******************************************************************************
;*      Debugging aid (board bringup) configuration.                           *
;*******************************************************************************

OPTION_CMOS_BREAKPOINTS         = 1     ; enable/disable breakpoints at boot.

OPTION_DEBUGAID_CPRINTF         = 0     ; enable CPRINTF remote console display.
OPTION_DEBUGAID_DPRINTF         = 0     ; enable a DPRINTF package.
OPTION_DEBUGAID_POST_CODES      = 1     ; support POST codes to progress port.
OPTION_DEBUGAID_POST_STRINGS    = 0     ; support POST strings to COM port.
OPTION_DEBUGAID_PRINTF_REDIR    = 0     ; enable PRINTF \0 and \1 redir codes.

OPTION_DIAGNOSE_APM             = 0     ; enable debug output in APM module.
OPTION_DIAGNOSE_SMM             = 0     ; enable I/O port LPT.b0 for SMM tracing.
OPTION_DIAGNOSE_SMM_L0          = 0     ; enable debugging of basic SMI delivery.
OPTION_DIAGNOSE_INT13SRV        = 0     ; enable debugging of INT13SRV I/O path.
OPTION_DIAGNOSE_BOOTBIN         = 0     ; BOOTBIN debugging -> CPRINTF.
OPTION_DIAGNOSE_DINIT           = 0     ; driver initialization calls -> CPRINTF.
OPTION_DIAGNOSE_DREADY          = 0     ; driver ready calls -> CPRINTF.
OPTION_DIAGNOSE_DMESSAGE        = 0     ; dump messages ordinals -> CPRINTF.
OPTION_DIAGNOSE_PNPINIT         = 0     ; PnP device node initialization.
OPTION_DIAGNOSE_PNPCDR          = 0     ; dump PnP CDR progress -> CPRINTF.
OPTION_DIAGNOSE_PNPRES          = 0     ; dump PnP resource streams -> CPRINTF.
OPTION_DIAGNOSE_PNPISA          = 0     ; dump Pnp ISA progress -> CPRINTF.
OPTION_DIAGNOSE_RFD             = 0     ; dump RFD progress -> CPRINTF.
OPTION_DIAGNOSE_SMP             = 0     ; dump SMP progress -> CPRINTF.
OPTION_DIAGNOSE_HUGEMEM         = 0     ; dump huge memory test progress -> CPRINTF.
OPTION_DIAGNOSE_ELTORITO        = 0     ; diagnose El-Torito functions.
OPTION_DIAGNOSE_CPU             = 0     ; enable CPM debugging.
OPTION_DIAGNOSE_CHIPSET         = 0     ; enable CSPM debugging.
OPTION_DIAGNOSE_BOARD           = 0     ; enable BPM debugging.
OPTION_DIAGNOSE_REG             = 0     ; enable registration screen debugging.
OPTION_DIAGNOSE_RESMGR          = 0     ; resource manager debugging -> CPRINTF.
OPTION_DIAGNOSE_MCL             = 0     ; media control layer debugging -> CPRINTF.
OPTION_DIAGNOSE_SMBUS           = 0     ; enable debug output in SMBUS module.
OPTION_DIAGNOSE_A20             = 0     ; halt the system with a POST code if A20 failed.
OPTION_DIAGNOSE_MEMCONFIG       = 0     ; enable XxxMemConfig debugging.
OPTION_DIAGNOSE_SHADOWAREA      = 0     ; enable XxxShadowArea debugging.
OPTION_DIAGNOSE_SHADOWWRITECTL  = 0     ; enable XxxShadowWriteCtl debugging.
OPTION_DIAGNOSE_SPD             = 0     ; enable memory SPD dump & debug.
OPTION_DIAGNOSE_LOADPCITABLE    = 0     ; enable PCI Table load debugging.
OPTION_DIAGNOSE_SDRAMINIT       = 0     ; enable SDRAM initialization debugging.
OPTION_DIAGNOSE_PCI             = 0     ; enable PCI debugging.
OPTION_DIAGNOSE_ATA             = 0     ; enable advanced IDE debugging.
OPTION_DIAGNOSE_AUTOREDIR       = 0     ; enable AutoRedirection debugging.
OPTION_DIAGNOSE_CUSTOMSETUP     = 0     ; enable Custom Setup debugging.
OPTION_DIAGNOSE_PCIWRITETABLE   = 0     ; enable diagnosis of PCI table writes.
OPTION_DIAGNOSE_INT13SRV        = 0     ; enable debugging of INT13SRV I/O path.
OPTION_DIAGNOSE_FLOPPY          = 0     ; enable debugging of Floppy.
OPTION_DIAGNOSE_PE              = 0     ; enable PE loader debugging -> CPRINTF.
OPTION_DIAGNOSE_SECURITY        = 0     ; enable security instrumentation -> CPRINTF.

CONFIG_POST_PROGRESS_BAUD       = COM_BAUD_115K ; other bauds defined in IO.INC.
CONFIG_POST_PROGRESS_COM        = CONFIG_COM1_BASE ; base IO addr of COM port.
CONFIG_POST_PROGRESS_PORT       = 80h   ; I/O port to write POST codes to.
CONFIG_WAIT_PROGRESS_COM        = 0     ; delay between POSTCODECOM characters.

;*******************************************************************************
;*      Diagnostic screen configuration.                                       *
;*******************************************************************************

DIAGNOSTIC "CPU Core                 : ", ProcCpu, 1
DIAGNOSTIC "BIOS Video Services      : ", ProcInt10, 1
DIAGNOSTIC "BIOS Equipment Services  : ", ProcInt11, 1
DIAGNOSTIC "BIOS Low Memory Size     : ", ProcInt12, 1
DIAGNOSTIC "Low Memory (<1MB)        : ", ProcLow, O_EXH
DIAGNOSTIC "BIOS Block Disk Services : ", ProcInt13, 1
DIAGNOSTIC "Extended Memory (>1MB)   : ", ProcHigh, O_PMEXH
DIAGNOSTIC "BIOS Serial Services     : ", ProcInt14, O_SER
DIAGNOSTIC "DMA Controller(s)        : ", ProcDma, 1
DIAGNOSTIC "BIOS System Services     : ", ProcInt15, 1
DIAGNOSTIC "Real-Time Clock          : ", ProcRtc, O_RTC
DIAGNOSTIC "BIOS Parallel Services   : ", ProcInt17, O_PAR
DIAGNOSTIC "BIOS Time/Date Services  : ", ProcInt1a, O_TIME
DIAGNOSTIC "BIOS User Timer Tick     : ", ProcInt1c, 1
DIAGNOSTIC "Floppy Disk I/O          : ", ProcFloppy, O_FLOPPY
DIAGNOSTIC "IDE Disk I/O             : ", ProcIde, O_IDE
DIAGNOSTIC "CMOS RAM & Battery       : ", ProcCmos, O_CMOS

;*******************************************************************************
;*      User interface and splash screen support options.                      *
;*******************************************************************************

OPTION_SUPPORT_EXTRES           = 1     ; 1 = external resource extraction.

OPTION_SUPPORT_SPLASHSCR        = 1     ; build code for splash screens.
CONFIG_SPLASH_BOOTS             = 100   ; # boot cycles splash stays disabled.

;JRB
SPLASH_TABLE    EVENT_SPLASH_INIT, RESOURCE_ID_SPLASH, SPLASH_CENTER, SPLASH_CENTER
SPLASH_TABLE    EVENT_SPLASH_INIT, 0FF12h, 0, 0
SPLASH_TABLE    EVENT_SPLASH_INIT, RESOURCE_ID_ADVERT_1, SPLASH_CENTER, SPLASH_CENTER
SPLASH_TABLE    EVENT_SPLASH_ICON, SPLASH_ICON_RIGHT+20h, 250, 9500

;SPLASH_TABLE    EVENT_SPLASH_INIT, RESOURCE_ID_SPLASH, SPLASH_CENTER, SPLASH_CENTER
;SPLASH_TABLE    EVENT_SPLASH_INIT, RESOURCE_ID_ADVERT_1, 2030, 6670
;SPLASH_TABLE    EVENT_SPLASH_ICON, SPLASH_ICON_RIGHT+20h, 250, 9500

;*******************************************************************************
;*      Architected platform specific overrides.                               *
;*******************************************************************************

OPTION_SUPPORT_QUICK_BOOT       = 1     ; enable the Quick Boot feature.
OPTION_CMOS_QUICK_BOOT          = 0     ; set to quick boot by default.
OPTION_REFRESH_8237             = 0     ; enable 8237A/8254 DRAM refresh.

;       Enable D&R and custom setup screen option to set conservative timings.

OPTION_SUPPORT_DR               = 1     ; diagnostics and recovery feature.
OPTION_GX3_MEM_OPT              = 0     ; disable conservative memory option.

;*******************************************************************************
;*      Build tool specific parameters.                                        *
;*******************************************************************************

;       Build parameters dealing with avoiding build tool bugs, such
;       as the symbol table allocation/overflow problem in MASM.

CONFIG_BUILD_EXTRA_SEGMENTS     = 1     ; number of extra segments to define.
CONFIG_BUILD_EXTRA_CHARACTERS   = 2575  ; fix MASM build issues.

;*******************************************************************************
;*      Un-architected Board, Chipset or CPU specific parameters.              *
;*******************************************************************************

OPTION_SUPPORT_SPD              = 1     ; support SPDs; disable to enable OEM tables.
OPTION_GX3_DEBUG_SMM            = 0     ; enable SMM debug port 80 output.
OPTION_BOARD_TEMPERATURE        = 0     ; support temperature display in setup screen.
OPTION_BOARD_DISPLAY_BOOTTIME   = 0     ; display boot time.
OPTION_BOARD_DEBUG_BOOTTIME     = 0     ; boot time debugging output.
OPTION_BOARD_LPC_SIO            = 1     ; LPC SIO is installed on the board.

;       These overrides are to match AMD's PRS values for GLCP1_DEL_CTRLS.

GLCP1_DEL_CTRLS_H_1DS           = 0f27100ffh
GLCP1_DEL_CTRLS_H_1DL           = 0f27100ffh
GLCP1_DEL_CTRLS_H_2DS           = 0f27100ffh
GLCP1_DEL_CTRLS_H_2DL           = 0f27100ffh

;       Override requested by AMD for 400MHz read timing specific for Salsa.

CONFIG_GLMC_400MHZ_RD_TMG       = 4

;       Overrides for diagnostic menu and custom setup menu.

        IF      OPTION_SETUP_DIAGNOSTICS ; include overrides for diag.asm.
OVERRIDE_PROCEDURE      TestDiskOp
OVERRIDE_PROCEDURE      WriteTestInProg
OVERRIDE_PROCEDURE      RunTests
OVERRIDE_PROCEDURE      SetupScreenDiag
        ENDIF   ; (OPTION_SETUP_DIAGNOSTICS)

;       To handle the AMD specific Int15h subfunctions we implement the custom
;       Int15h handler. The custom ISR supports AMD OEM subfunctions of Int15h
;       functions 0BEh and 0BFh.

DEFINE_ISR_PROC _15, BoardInt15Isr      ; Use custom INT 15h handler.

OPTION_IDE_DWORD_IO             = 1     ; rd/wr sectors with dwords, not words.

PACKAGE GX3
PACKAGE SIOPWR2

;       Generic SIO configuration options.

OPTION_SETUP_SIO                = 1     ; enable SIO setup support.

OPTION_SUPPORT_SIO_CORE         = 1     ; build SIO core support code.
OPTION_SIO_BOARD_ENABLES        = 0     ; enable board calls to enable/disable.
OPTION_SIO_PROGRAM_BUS          = 1     ; enable programming of upstream bus.
OPTION_SIO_BUS_LPC              = 1     ; enable SIO LPC bus support.
OPTION_SIO_BUS_ISA              = 0     ; enable SIO ISA bus support.
OPTION_SIO_BUS_PCI              = 0     ; enable SIO PCI bus support.
OPTION_SIO_BUS_INTERNAL         = 1     ; enable internal bus support.
OPTION_SIO_MERGE_UART_IRDA      = 1     ; merge UART and IRDA config spaces.
OPTION_SIO_PNP                  = 1     ; enable automatic PnP node creation.
OPTION_SIO_SOFTERR              = 0     ; enable soft error on failure.
OPTION_SIO_CONFIGURE            = 1     ; enable SIO configuration helper code.
OPTION_SIO_CONFIGURE_UART       = 1     ; enable UART configuration support.
OPTION_SIO_CONFIGURE_PARALLEL   = 1     ; enable parallel configuration support.
OPTION_SIO_CONFIGURE_KEYBOARD   = 1     ; enable keyboard configuration support.
OPTION_SIO_CONFIGURE_MOUSE      = 1     ; enable mouse configuration support.
OPTION_SIO_CONFIGURE_FLOPPY     = 1     ; enable floppy configuration support.

;       Disable SIO devices by default, they will be enabled dynamically
;       through the Custom Setup Screen.

CONFIG_W83627_FLOPPY_DEFAULT_EN = 0     ; disable floppy by default.
CONFIG_W83627_LPT_DEFAULT_EN    = 0     ; disable LPT by default.
CONFIG_W83627_UART1_DEFAULT_EN  = 0     ; disable UART1 by default.
CONFIG_W83627_UART2_DEFAULT_EN  = 0     ; disable UART2 by default.

;       Configuration specific to included SIO parts.

OPTION_W83627_FLOPPY            = 1     ; enable dynamic FDC programming.
OPTION_W83627_PARALLEL          = 1     ; enable dynamic parallel config.
OPTION_W83627_UART1             = 1     ; enable dynamic UART1 config.
OPTION_W83627_UART2             = 1     ; enable dynamic UART2 config.
OPTION_W83627_PS2               = 1     ; enable dynamic PS/2 config.
OPTION_GX553X_UART1             = 0     ; UART1 is IRDA.
OPTION_GX553X_IRDA2             = 0     ; UART2 is not IRDA.

;       Enable Setup Screen support for devices.

OPTION_SETUP_SIO_FLOPPY         = 1     ; enable floppy setup support.
CONFIG_SETUP_SIO_UART_MAX       = 4     ; maximum number of UARTs to support.
CONFIG_SETUP_SIO_PARALLEL_MAX   = 1     ; maximum number of parallel ports to support.
CONFIG_SETUP_SIO_KEYBOARD_MAX   = 1     ; maximum number of keyboard configurations.
CONFIG_SETUP_SIO_MOUSE_MAX      = 1     ; maximum number of mouse configurations.
CONFIG_SETUP_SIO_FLOPPY_MAX     = 1     ; maximum number of floppy configurations.

;       List of supported SIO parts included in build.

;SIO_INCLUDE w83627                      ; include SIO support for Winbond W83627.
SIO_INCLUDE Gx553X                      ; include SIO support for 5535/5536.

;       The BIOS build number parameter needs to be incremented to update
;       the build number string displayed.

CONFIG_BOARD_BUILD_NUM          = 48

;       Define Firmbase feature set for the Features Setup Screen.

FIRMBASE_FEATURE UsbMassStorage
FIRMBASE_FEATURE AtaMassStorage
FIRMBASE_FEATURE Usb20
FIRMBASE_FEATURE User
FIRMBASE_FEATURE Network
FIRMBASE_FEATURE Shell
FIRMBASE_FEATURE Applications

;       Bits in CONFIG_FB_FEATURE_DEFAULT follow the order as they appear in
;       the FIRMBASE_FEATURE defines above. So bit 0 is for first line parameter,
;       bit 1 for second line parameter, and so on. OR the features that need
;       to be enabled by default.

CONFIG_FB_FEATURE_DEFAULT       = (BIT0 OR BIT5) ; enable UsbMassStorage and Shell.

PACKAGE CMOSDFLT                        ; include the CMOSDFLT package.
PACKAGE E820UPD
