;***    SALSA.INC -- Board-level Include File for AMD LXDB800 Development Platform.
;
;1.     Functional Description.
;       This include file defines board-level options for the AMD
;       LXDB800 "Salsa" Development Platform featuring the Geode GX3
;       processor with the CS5535 companion chip.
;
;       Please refer to BOARDS\SALSA\SALSA.ASM for a detailed discussion of
;       motherboard capabilities versus BIOS implementation and support.
;
;2.     Modification History.
;       E. Khoruzhenko  05/03/28.       #5.3, added VGA options.
;       R. Spiegel      05/06/08.       #5.3.3, added POWER parameters and macros.
;       K. C. Taylor    05/06/10.       #5.3.3, added call to BoardHelpStrSave.
;       R. Spiegel      05/07/11.       #5.3.3, added table for AMD's version #.
;       E. Khoruzhenko  05/07/14.       #5.3.3, updated OEM version display.
;       S. Kaganovsky   05/08/25.       #5.3.3, added CONFIG_SETUP_CUSTOM_SPEEDS for CPU/MEM speeds.
;       E. Khoruzhenko  05/09/14.       #5.3.3, TFT settings updated for GX3.
;       E. Khoruzhenko  05/09/27.       #5.3.3, added options for OTG and UDC.
;       E. Khoruzhenko  05/10/11.       #5.3.3, added CONFIG_SETUP_USB4_FUN.
;       R. Spiegel      05/11/01.       #5.3.3, added OPTION_SETUP_CUSTOM_MEM_OPT.
;
;3.     NOTICE: Copyright (C) 2001-2005 General Software, Inc.

;***    Board-Specific Build Configuration Options and Parameters.
;
;       The following BPM-specific build parameters may be overridden
;       in the project file to control the BIOS build.

;       The following are default CMOS values for the custom setup screen.

        IFNDEF  CONFIG_SETUP_CUSTOM_PCI_A
CONFIG_SETUP_CUSTOM_PCI_A = 0           ; default to "Auto".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_PCI_A)

        IFNDEF  CONFIG_SETUP_CUSTOM_PCI_B
CONFIG_SETUP_CUSTOM_PCI_B = 0           ; default to "Auto".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_PCI_B)

        IFNDEF  CONFIG_SETUP_CUSTOM_PCI_C
CONFIG_SETUP_CUSTOM_PCI_C = 0           ; default to "Auto".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_PCI_C)

        IFNDEF  CONFIG_SETUP_CUSTOM_PCI_D
CONFIG_SETUP_CUSTOM_PCI_D = 0           ; default to "Auto".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_PCI_D)

;JRB
        IFNDEF  CONFIG_SETUP_CUSTOM_COM_1
CONFIG_SETUP_CUSTOM_COM_1 = 2           ; default to "fourth UART discovered".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_COM_1)

        IFNDEF  CONFIG_SETUP_CUSTOM_COM_2
CONFIG_SETUP_CUSTOM_COM_2 = 1           ; default to "3rd UART discovered".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_COM_2)

        IFNDEF  CONFIG_SETUP_CUSTOM_COM_3
CONFIG_SETUP_CUSTOM_COM_3 = 0           ; default to "Disable".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_COM_3)

        IFNDEF  CONFIG_SETUP_CUSTOM_COM_4
CONFIG_SETUP_CUSTOM_COM_4 = 0           ; default to "Disable".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_COM_4)

        IFNDEF  CONFIG_SETUP_CUSTOM_IRDA
CONFIG_SETUP_CUSTOM_IRDA  = 0           ; default to "Disable".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_IRDA)

        IFNDEF  CONFIG_SETUP_CUSTOM_LPT_1
CONFIG_SETUP_CUSTOM_LPT_1 = 0           ; default to "Disable".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_LPT_1)

        IFNDEF  CONFIG_SETUP_CUSTOM_VGADEF
CONFIG_SETUP_CUSTOM_VGADEF = 1          ; default to "Auto".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_VGADEF)

        IFNDEF  CONFIG_SETUP_CUSTOM_VGABUF
CONFIG_SETUP_CUSTOM_VGABUF = 4          ; default to 16 MB.
        ENDIF   ; (CONFIG_SETUP_CUSTOM_VGABUF)
;JRB
        IFNDEF  CONFIG_SETUP_CUSTOM_VIDEO
CONFIG_SETUP_CUSTOM_VIDEO = 3           ; default to "Disable".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_VIDEO)

        IFNDEF  CONFIG_SETUP_CUSTOM_RFSH
CONFIG_SETUP_CUSTOM_RFSH = 0            ; default video refresh (60 HZ).
        ENDIF   ; (CONFIG_SETUP_CUSTOM_RFSH)

        IFNDEF  CONFIG_SETUP_CUSTOM_VWDTH
CONFIG_SETUP_CUSTOM_VWDTH = 0           ; default video data width (8 bit).
        ENDIF   ; (CONFIG_SETUP_CUSTOM_VWDTH)

        IFNDEF  CONFIG_SETUP_CUSTOM_PANEL
CONFIG_SETUP_CUSTOM_PANEL = 0           ; default video panel type (TFT).
        ENDIF   ; (CONFIG_SETUP_CUSTOM_PANEL)

        IFNDEF  CONFIG_SETUP_CUSTOM_LEGACY_USB
CONFIG_SETUP_CUSTOM_LEGACY_USB = 1      ; default Legacy USB to "Enabled".
        ENDIF   ; (CONFIG_SETUP_CUSTOM_LEGACY_USB)

;JRB
        IFNDEF  CONFIG_SETUP_CUSTOM_SPEEDS
CONFIG_SETUP_CUSTOM_SPEEDS      = 31     ; default to 200/200 MHz CPU/MEM speeds.
        ENDIF   ; (CONFIG_SETUP_CUSTOM_SPEEDS)

        IFNDEF  OPTION_BOARD_PNP
OPTION_BOARD_PNP        = 1             ; default to "Enable".
        ENDIF   ; (OPTION_BOARD_PNP)

        IFNDEF  OPTION_BOARD_IRDA
OPTION_BOARD_IRDA       = 1             ; default IRDA to "SIR".
        ENDIF   ; (OPTION_BOARD_IRDA)

        IFNDEF  OPTION_BOARD_TFT
OPTION_BOARD_TFT        = 1             ; default TFT to "TFT 640".
        ENDIF   ; (OPTION_BOARD_TFT)
;JRB
        IFNDEF  OPTION_BOARD_CMOS_IDE80WIRE
OPTION_BOARD_CMOS_IDE80WIRE = 1         ; default IDE cable type is 80-wire.
        ENDIF   ; (OPTION_BOARD_CMOS_IDE80WIRE)

        IFNDEF OPTION_BOARD_CMOS_SLEEPTYPE
OPTION_BOARD_CMOS_SLEEPTYPE = 0         ; default sleep type to S1.
        ENDIF   ; (OPTION_BOARD_CMOS_SLEEPTYPE)

        IFNDEF  OPTION_BOARD_DISPLAY_BOOTTIME
OPTION_BOARD_DISPLAY_BOOTTIME = 0       ; do not display boot time by default.
        ENDIF   ; (OPTION_BOARD_DISPLAY_BOOTTIME)

        IFNDEF  OPTION_BOARD_TEMPERATURE
OPTION_BOARD_TEMPERATURE  = 1           ; support temperature display in setup screen.
        ENDIF   ; (OPTION_BOARD_TEMPERATURE)

;       If OPTION_SETUP_CUSTOM_MEM_OPT is set to 0, it's the developer job to
;       also set OPTION_GX3_MEM_OPT  to 0 or there will be a compile error.

        IFNDEF  OPTION_SETUP_CUSTOM_MEM_OPT
OPTION_SETUP_CUSTOM_MEM_OPT = 1         ; build the MEM_OPT setup question.
        ENDIF   ; (OPTION_SETUP_CUSTOM_MEM_OPT)

        IFNDEF  CONFIG_BOARD_PWR_LED_DUTY_RATE
CONFIG_BOARD_PWR_LED_DUTY_RATE = 03200190h ; duty/rate good value.
        ENDIF   ; (CONFIG_BOARD_PWR_LED_DUTY_RATE)

        IFNDEF  OPTION_SETUP_CUSTOM_SLEEPTYPE
OPTION_SETUP_CUSTOM_SLEEPTYPE = 0
        ENDIF   ; (OPTION_SETUP_CUSTOM_SLEEPTYPE)

        IFNDEF  CONFIG_SETUP_CUSTOM_MEM_OPT
CONFIG_SETUP_CUSTOM_MEM_OPT   = 0       ; default to "Optimal" memory settings.
        ENDIF   ; (CONFIG_SETUP_CUSTOM_MEM_OPT)
;JRB
        IFNDEF  OPTION_SETUP_CUSTOM_FLOPPY_CONT
OPTION_SETUP_CUSTOM_FLOPPY_CONT = 0
        ENDIF   ; (OPTION_SETUP_CUSTOM_FLOPPY_CONT)

        IFNDEF  CONFIG_SETUP_USB_OTG
CONFIG_SETUP_USB_OTG          = 0
        ENDIF   ; (CONFIG_SETUP_USB_OTG)

        IFNDEF  CONFIG_SETUP_USB_UDC
CONFIG_SETUP_USB_UDC          = 0
        ENDIF   ; (CONFIG_SETUP_USB_UDC)

        IFNDEF  CONFIG_SETUP_USB4_FUN
CONFIG_SETUP_USB4_FUN         = 1       ; assign USB port 4 to Host by default.
        ENDIF   ; (CONFIG_SETUP_USB4_FUN)

;       The following five paramaters allow configuration of specific known
;       addresses that are used as part of comunication with the GX3 driver
;       while managing power.  They should not be changed without also
;       changing the configuration of the GX3 driver.

        IFNDEF  CONFIG_BOARD_S3_STACK_SIZE
CONFIG_BOARD_S3_STACK_SIZE = 1000h      ; fixed size of stack for S3.
        ENDIF   ; (CONFIG_BOARD_S3_STACK_SIZE)

        IFNDEF  CONFIG_BOARD_S3_STACK_ADDRESS
CONFIG_BOARD_S3_STACK_ADDRESS = 7000h   ; fixed address of stack end for S3.
        ENDIF   ; (CONFIG_BOARD_S3_STACK_ADDRESS)

        IFNDEF  CONFIG_BOARD_VGA_BUFFER_ADDRESS
CONFIG_BOARD_VGA_BUFFER_ADDRESS = 040000h ; fixed VGA S3 buffer address.
        ENDIF   ; (CONFIG_BOARD_VGA_BUFFER_ADDRESS)

;       The following two parameters configure the location of fixed entry
;       points used by the GX3 driver to call real mode code as part of
;       entering and leaving S3 (suspend to RAM).  These fixed entry points
;       are effectively added to the list of known entry points by the
;       BOARD_EXTEND_ENTRIES macro declared below.  See BIOS.ASM for details.

;       These entry points cannot be placed in the E0000h segment.  They must
;       land in free space somewhere between FE000h and FFFFFh. If current address
;       is changed, changes need to be done on macro BOARD_EXTEND_ENTRIES.

        IFNDEF  CONFIG_BOARD_S3_CONTINUE_ENTRY_PT
CONFIG_BOARD_S3_CONTINUE_ENTRY_PT = 0fe060h ; fixed entry point for continue.
        ENDIF   ; (CONFIG_BOARD_S3_CONTINUE_ENTRY_PT)

        IFNDEF  CONFIG_BOARD_SET_POWER_ENTRY_PT
CONFIG_BOARD_SET_POWER_ENTRY_PT = 0fe080h ; fixed entry point for setting power.
        ENDIF   ; (CONFIG_BOARD_SET_POWER_ENTRY_PT)

;***    BOARD_PREPOST Macro.
;
;       The following macro is inserted in routine BootStrap to initialize the
;       board within the top 1KB at power-on.  This code operates with
;       interrupts disabled in a stackless environment.

BOARD_PREPOST   MACRO
        mov     al, 070h                ;; indicate early ROM access.
        out     80h, al
        ENDM

;       Defines for the base address of the LPC XBUS extension registers.

LPC_XBUS_BASE                   = 05000h
LPC_XBUS_XZCNF2                 = (LPC_XBUS_BASE + 0Dh)
LPC_XBUS_XZCNF2_PORT80          = 010h  ; value to set XBUS CS2 to port 80.

;       Defines used to set CS5535 integrated UARTS.

GX3_UART0               = 0             ; select UART 0.
GX3_UART1               = BIT14         ; select UART 1.
GX3_COM1_BAR            = 0000011b      ; COM1.
GX3_COM2_BAR            = 0000001b      ; COM2.
GX3_COM3_BAR            = 0000010b      ; COM3.
GX3_COM4_BAR            = 0000000b      ; COM4
GX3_UART_ENABLE         = (BIT8 OR BIT2) ; enable UART.

;***    WRLPC - Write to LPC SIO Register.
;
;       USES: Registers ax, dx.
;
;       This macro writes to a register on the LPC SIO part.

WRLPC   MACRO   index, data
        mov     dx, LPC_INDEX_PORT
        mov     al, index               ;; select LPC SIO register.
        out     dx, al
        DELAY   1
        mov     dx, LPC_DATA_PORT
        mov     al, data                ;; program this register.
        out     dx, al
        DELAY   1
        ENDM    ;; (WRLPC)

;***    RDLPC - Read from LPC SIO Register.
;
;       USES: Registers ax, dx.
;
;       This macro reads from a register on the LPC SIO part.

RDLPC   MACRO   index
        mov     dx, LPC_INDEX_PORT
        mov     al, index               ;; select LPC SIO register.
        out     dx, al
        DELAY   1
        mov     dx, LPC_DATA_PORT
        in      al, dx                  ;; read this register.
        ENDM    ;; (RDLPC)

;       The parameter below specifies which IRQs are routed to LPC.

        IFNDEF  CONFIG_DIVIL_PIC_IRQM_LPC
;                                 FEDCBA9876543210
CONFIG_DIVIL_PIC_IRQM_LPC       = 0001000011011010b
        ENDIF   ; (CONFIG_DIVIL_PIC_IRQM_LPC)

        IFNDEF  CONFIG_GX3_VGA_FB_SIZE
CONFIG_GX3_VGA_FB_SIZE          = 8192  ; VGA frame buffer size in KB.
        ENDIF   ; (CONFIG_GX3_VGA_FB_SIZE)

;       The following data field uses board specific storage in the extended
;       BIOS data area to store the type of Firmware hub being used in this
;       design.  Calls to the media layer will be routed through the user
;       MTD, specified in this board module.  The user MTD will then check
;       this variable to see which MTD to route to.  If the field is not
;       specifically defined, the user driver will attempt to auto-detect the
;       type of media being used.

FwhPartSel      EQU     byte ptr [BoardData+0] ; one byte at this address.

FWH_IS_INTEL            = 0A0h          ; symbol to indicate Intel FWH.
FWH_IS_SST              = 0A1h          ; symbol to indicate SST FWH.

;       Flat Panel Configuration Parameters.

;       TFT refresh rate.
;       0 = 60 Hz
;       1 = 65 Hz
;       2 = 70 Hz
;       3 = 75 Hz
;       4 = 85 Hz
;       5 = 100 Hz

        IFNDEF  CONFIG_TFT_REFR_RATE
CONFIG_TFT_REFR_RATE    = 0             ; default refresh rate.
        ENDIF   ; (CONFIG_TFT_REFR_RATE)

        IFNDEF  CONFIG_TFT_VSYNC_POL
CONFIG_TFT_VSYNC_POL    = BIT12
        ENDIF   ; (CONFIG_TFT_VSYNC_POL)

        IFNDEF  CONFIG_TFT_HSYNC_POL
CONFIG_TFT_HSYNC_POL    = BIT11
        ENDIF   ; (CONFIG_TFT_HSYNC_POL)

;       Bus width.

        IFNDEF  CONFIG_TFT_BUS_WIDTH
CONFIG_TFT_BUS_WIDTH    = 0             ; default bus width.
        ENDIF   ; (CONFIG_TFT_BUS_WIDTH)

;       Flat panel type.
;       0 = TFT
;       1 = LVDS

        IFNDEF  CONFIG_TFT_FP_TYPE
CONFIG_TFT_FP_TYPE      = 0             ; default flat panel type.
        ENDIF   ; (CONFIG_TFT_FP_TYPE)

CONFIG_TFT_FP_DEFAULT   = (CONFIG_TFT_REFR_RATE OR CONFIG_TFT_VSYNC_POL OR \
                          CONFIG_TFT_HSYNC_POL OR CONFIG_TFT_BUS_WIDTH OR \
                          CONFIG_TFT_FP_TYPE)

;       Create the BIOS version string to be displayed on the splash screen.

CONFIG_BOARD_SPLASH_VER EQU     'Embedded BIOS (R) 2000 v', \
    BIOS_MAJOR_VERSION +'0', '.', BIOS_MINOR_VERSION +'0'

;       The following macro is used to define two known entry points that
;       are used by the Firmbase GX3 driver to make real mode calls outside
;       of SMM as well as to set the current power management state.

BOARD_EXTEND_ENTRIES MACRO

        local   BOARD_EXTEND_ENTRIES_LOOP
        include ..\chipsets\gx3\gx3.inc
        include ..\chipsets\gx3\cs5536.inc

;       Write virtual register to set power state 0. This generates an SMI
;       and control is passed to the Firmbase GX3 driver.  This unblocks a
;       thread in the driver that handles suspend and resume.  Do not change
;       CONFIG_BOARD_S3_CONTINUE_ENTRY_PT without making a corresponding
;       change to the GX3 driver configuration.  This code will be called as
;       a result of the temporary stack initialized by the GX3 driver when
;       it calls other real mode code.

        IF      ((OPTION_SUPPORT_ACPI NE 0) OR (OPTION_SUPPORT_APM NE 0)\
                OR (OPTION_SUPPORT_POWER NE 0))

        ORG     (OFFSET_FF00-(0fff00h-CONFIG_BOARD_S3_CONTINUE_ENTRY_PT))
        BEGINUSED
        mov     dx, VSA2_VR_INDEX       ; (DX) = virtual IO index port.
        mov     ax, VSA2_VR_UNLOCK      ; (AX) = unlock code.
        out     dx, ax                  ; unlock virtual register.
        mov     ax, ((VSA2_VR_CLASS_PM SHL 8) OR VSA2_VR_PM_STATE)
        out     dx, ax                  ; set index.
        mov     dx, VSA2_VR_DATA        ; (DX) = virtual IO data port.
        mov     ax, 0                   ; (AX) = set power state 0.
        out     dx, ax                  ; set data.
        jmp     $                       ; we should never reach this point.
        ENDUSED

;       Write to power management control register.  This provides the power
;       management driver with the ability to cleanly exit SMM before going
;       to a specific power state.

        ORG     (OFFSET_FF00-(0fff00h-CONFIG_BOARD_SET_POWER_ENTRY_PT))
        BEGINUSED
        push    ax
        sub     ax, ax                  ; (AX) = 0.
        mov     es, ax                  ; (ES) = "Stack" segment.
        mov     edi, CONFIG_BOARD_S3_STACK_ADDRESS ; location of table.
        Gcall   BoardHelpStrSave, POST  ; last minute critical MSR state save.
        pop     ax
        wbinvd                          ; writeback invalidate the cache.
        mov     dx, (DIVIL_ACPI_BASE + DIVIL_ACPI_PM1_CNT)
        out     dx, ax                  ; set power management state.
        sub     cx, cx                  ; setup for delay loop.
BOARD_EXTEND_ENTRIES_LOOP:
        in      al, 080h
        loop    BOARD_EXTEND_ENTRIES_LOOP ; delay briefly.
        iret                            ; only reached resuming S1.
        ENDUSED

        ENDIF   ; ((OPTION_SUPPORT_ACPI NE 0) OR (OPTION_SUPPORT_APM NE 0)\
                ; OR (OPTION_SUPPORT_POWER NE 0))

        ORG     (OFFSET_FF00-(0ff00h-0e2d0h))
        BEGINUSED

        db      '$VER'                  ; core version.
        db      BIOS_MAJOR_VERSION +'0', '.', BIOS_MINOR_VERSION +'0', 0

        db      '$TIM'                  ; build time.
        db      bldtime, 0

        db      '$DAT'                  ; build date.
        db      blddate, 0

        db      '$CPR'                  ; copyright string.
        db      "(C) 2003-2005 General Software, Inc.", 0

        db      '$REL'                  ; release type.
        db      'Engineering release', 0

        db      '$PLT'                  ; target platform.
        db      'Salsa', 0

        db      '$USR'                  ; user-displayed BIOS version.
        db      CONFIG_BOARD_BUILD_STR, 0

        ENDUSED

        ENDM
