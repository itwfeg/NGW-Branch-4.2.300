;***    SETUP -- AMD LXDB800 Board Module Setup Configuration.
;
;1.     Functional Description.
;       This file contains custom code created by the BIOS adaptation
;       engineer to implement the custom setup screen option in GX3 based
;       projects.  The screen allows custom routing of the PCI interrupts,
;       assignment of COM and LPT ports to the CS5535 or an LPC based SIO.
;       This setup screen also allows configuration of the TFT panel if used.
;       The custom setup screen is configured by several structures which
;       are interpreted by the setup screen engine.  Several macros are
;       supplied to simplify the creation of these structures.  The display
;       for the menu item prompt as well as the option list and CMOS cell
;       name for the selection is created using the Dcustom macro.
;
;       The DCustom macro is defined in SYSTEM\BOARD.ASM, and its arguments
;       are fields, as follows:
;
;       tag, title, datawidth, keyrtn, disprtn, loval, hival, strtbl
;
;       The actual data fields are described in CustomTbl.  The tag field is a
;       label associated with the CUSTOMDEF entry in CustomTbl.  The
;       title is what is displayed on the custom SETUP screen itself.
;       Note that the order in which the arrow keys navigate through the
;       SETUP screen is a function of the order in which the fields appear
;       in the *CustomTbl array*, NOT their DCustom definitions.
;
;       CustomTbl is set up using the DEF_CUSTOM_CMOS macro.
;
;       Note that it is critical that the entries in CustomTbl be in the same
;       order and contain the same entries as the "CUSTOMDEF" table.
;       Otherwise, the fields won't align properly and BoardInitFields
;       and BoardSaveFields will set up the wrong field values for each entry.
;
;       As for the fields in this table, the first field defines a reference
;       we will be using to refer to the storage location of that field.  The
;       second field is how many bits of storage should be used.  The third
;       field is the default value to use for that storage.
;
;       For example, "DEF_CUSTOM_CMOS PCI_A, 4, 2" would create a new 4 bit
;       storage slot called CMOS_CUSTOM_PCI_A, which would default to
;       containing the value "2".
;
;       Management of the CMOS cell, offsets of bits in CMOS and the reading
;       and writing of custom setup screen entries are all handled
;       automatically for us.  We never have to manually manage the details
;       of bit packing for individual CMOS cells.  This makes it easy to add
;       or remove entries and re-define how values will be stored.
;
;       The current custom setup screen creates the following CMOS fields:
;
;           CMOS_CUSTOM_PCI_A,          4, 0    ; PCI INT A route.
;           CMOS_CUSTOM_PCI_B,          4, 0    ; PCI INT B route.
;           CMOS_CUSTOM_PCI_C,          4, 0    ; PCI INT C route.
;           CMOS_CUSTOM_PCI_D,          4, 0    ; PCI INT D route.
;
;           The above fields will contain an ordinal indicating the
;           interrupt number if a custom interrupt is selected using
;           the custom setup screen.
;
;           CMOS_CUSTOM_COM_1           3, 0    ; COM 1 routing.
;           CMOS_CUSTOM_COM_2           3, 0    ; COM 2 routing.
;           CMOS_CUSTOM_COM_3           3, 0    ; COM 3 routing.
;           CMOS_CUSTOM_COM_4           3, 0    ; COM 4 routing.
;
;           The above fields will contain an ordinal indicating the
;           assignment to either the integrated CS5535 UART or UART
;           contained in an LPC SIO.
;
;           CMOS_CUSTOM_LPT_1           2, 0    ; LPT 1 routing / enable.
;
;           The above field will contain a boolean that indicates whether
;           or not to enable LPT1:
;
;           CMOS_CUSTOM_VIDEO,          5, 0    ; Video mode.
;
;           The above field contains an ordinal selecting the TFT panel
;           size if a TFT panel is present.
;
;           These fields can be accessed using the provided
;           BoardHelpReadCustomCmosSetup procedure.
;
;       Note: This code is not supplied by General Software with the core system BIOS.
;
;2.     Modification History.
;       E. Khoruzhenko  04/07/27.       #5.3, replaced timer routine with BoardSetupCustomCtl.
;       E. Khoruzhenko  05/03/28.       #5.3, added VGA options.
;       R. Spiegel      05/05/05.       #5.3, fixed CAS display values.
;       K. C. Taylor    05/06/08.       #5.3.3, cleaned out extra serial port strings.
;       R. Spiegel      05/06/09.       #5.3.3, added 5535/5536 detection and ID.
;       R. Spiegel      05/08/08.       #5.3.3, changed speed strings to match new table.
;       S. Kaganovsky   05/08/25.       #5.3.3, added CONFIG_SETUP_CUSTOM_SPEEDS for CPU/MEM speeds.
;       E. Khoruzhenko  05/09/14.       #5.3.3, TFT support updated for GX3.
;       E. Khoruzhenko  05/09/27.       #5.3.3, added options for OTG and UDC.
;       E. Khoruzhenko  05/10/11.       #5.3.3, configure USB port 4 as Host or Device.
;       R. Spiegel      05/11/01.       #5.3.3, use OPTION_SETUP_CUSTOM_MEM_OPT.
;       M. J. Siler     05/11/02.       #5.3.3, updated branding info to LX.
;
;3.     Warnings.
;       None.
;
;4.     NOTICE: Copyright (C) 1998-2006 General Software, Inc.

        EXTRN   ProcessorNames:WORD

        IF      ((OPTION_SUPPORT_SETUP NE 0) AND (OPTION_SETUP_CUSTOM NE 0))

SETUP_  SEGMENT

        IF      OPTION_SETUP_CUSTOM
        EXTRN   NoStrs:WORD, EnabStrs:WORD      ; Defined in SETUPBAS.ASM.
        EXTRN   _p_KeyNull:NEAR, _p_KeyInt:NEAR, _p_DispStr:NEAR
        ENDIF   ; (OPTION_SETUP_CUSTOM)

;       Standard Custom Setup Screen definitions.

BIAS    =       CONFIG_START_BOARD_CMOS ; start of extended CMOS. (cell 40h if no power management).
BIAS_BIT                = 0
TOTAL_CUSTOM_CELLS      = 0
TOTAL_CUSTOM_ENTRIES    = 0

;       Custom CMOS settings.  Note that it is critical that this table be
;       in the same order and contain the same entries as the "CUSTOMDEF"
;       table.  Otherwise, the fields won't align properly and
;       BoardInitFields and BoardSaveFields will set up the wrong field
;       values for each entry.

;       As for the fields in this table, the first field defines a reference
;       we will be using to refer to the storage location of that field.  The
;       second field is how many bits of storage should be used.  The third
;       field is the default value to use for that storage.

;       For example, "DEF_CUSTOM_CMOS PCI_A, 4, 2" would create a new 4 bit
;       storage slot called CMOS_CUSTOM_PCI_A, which would default to
;       containing the value "2".

;       Management of the CMOS cell, offsets of bits in CMOS and the reading
;       and writing of custom setup screen entries are all handled
;       automatically for us.  We never have to manually manage the details
;       of bit packing for individual CMOS cells.  This makes it easy to add
;       or remove entries and re-define how values will be stored.

DEF_CUSTOM_CMOS PCI_A,  4, CONFIG_SETUP_CUSTOM_PCI_A    ; PCI INT A route.
DEF_CUSTOM_CMOS PCI_B,  4, CONFIG_SETUP_CUSTOM_PCI_B    ; PCI INT B route.
DEF_CUSTOM_CMOS PCI_C,  4, CONFIG_SETUP_CUSTOM_PCI_C    ; PCI INT C route.
DEF_CUSTOM_CMOS PCI_D,  4, CONFIG_SETUP_CUSTOM_PCI_D    ; PCI INT D route.

DEF_CUSTOM_CMOS COM_1,  3, CONFIG_SETUP_CUSTOM_COM_1    ; COM 1 routing.
DEF_CUSTOM_CMOS COM_2,  3, CONFIG_SETUP_CUSTOM_COM_2    ; COM 2 routing.
DEF_CUSTOM_CMOS COM_3,  3, CONFIG_SETUP_CUSTOM_COM_3    ; COM 3 routing.
DEF_CUSTOM_CMOS COM_4,  3, CONFIG_SETUP_CUSTOM_COM_4    ; COM 4 routing.

        IF      OPTION_BOARD_IRDA
DEF_CUSTOM_CMOS IRDA_,  3, CONFIG_SETUP_CUSTOM_IRDA     ; IRDA settings.
        ENDIF   ; (OPTION_BOARD_IRDA)

DEF_CUSTOM_CMOS LPT_1,  2, CONFIG_SETUP_CUSTOM_LPT_1    ; LPT 1 routing / enable.

DEF_CUSTOM_CMOS VGADEF, 2, CONFIG_SETUP_CUSTOM_VGADEF   ; default VGA controller.
DEF_CUSTOM_CMOS VGABUF, 3, CONFIG_SETUP_CUSTOM_VGABUF   ; VGA frame buffer size.

        IF      OPTION_BOARD_TFT
DEF_CUSTOM_CMOS VIDEO,  3, CONFIG_SETUP_CUSTOM_VIDEO    ; video mode.
DEF_CUSTOM_CMOS VRFSH,  3, CONFIG_SETUP_CUSTOM_RFSH     ; video refresh rate.
DEF_CUSTOM_CMOS VWDTH,  1, CONFIG_SETUP_CUSTOM_VWDTH    ; video data width.
DEF_CUSTOM_CMOS PANEL,  2, CONFIG_SETUP_CUSTOM_PANEL    ; video panel type.
        ENDIF   ; (OPTION_BOARD_TFT)

        IF      OPTION_SETUP_CUSTOM_MEM_OPT
DEF_CUSTOM_CMOS MEM_OPT, 1, CONFIG_SETUP_CUSTOM_MEM_OPT ; memory timings.
        ENDIF   ; (OPTION_SETUP_CUSTOM_MEM_OPT)

DEF_CUSTOM_CMOS LEGACY_USB, 2, CONFIG_SETUP_CUSTOM_LEGACY_USB ; support legacy USB.

        IF      OPTION_ATA_CUSTOM_SETUP_CABLE
DEF_CUSTOM_CMOS IDE80WIRE,  1, OPTION_BOARD_CMOS_IDE80WIRE    ; IDE 80-wire detection.
        ENDIF   ; (OPTION_ATA_CUSTOM_SETUP_CABLE)
        IF      OPTION_SETUP_CUSTOM_SLEEPTYPE
DEF_CUSTOM_CMOS SLEEPTYPE,  2, OPTION_BOARD_CMOS_SLEEPTYPE    ; sleep type.
        ENDIF   ; (OPTION_SETUP_CUSTOM_SLEEPTYPE)
;JRB
        IF      OPTION_SETUP_CUSTOM_FLOPPY_CONT
DEF_CUSTOM_CMOS FDC_SEL,    2, 1                        ; FDC selection.
        ENDIF   ; (OPTION_SETUP_CUSTOM_FLOPPY_CONT)
        IF      OPTION_GX3_ON_THE_GO
DEF_CUSTOM_CMOS USB_OTG, 1, CONFIG_SETUP_USB_OTG        ; support USB On-The-Go.
        ENDIF   ; (OPTION_GX3_ON_THE_GO)

DEF_CUSTOM_CMOS USB_UDC, 1, CONFIG_SETUP_USB_UDC        ; support USB Device Control.

DEF_CUSTOM_CMOS SPEEDS, 5, CONFIG_SETUP_CUSTOM_SPEEDS   ; CPU/MEM speeds.

DEF_CUSTOM_CMOS USB4_FUN, 2, CONFIG_SETUP_USB4_FUN      ; port 4 function.

        ENDSEG

;       The following table is designed specifically to allow us to unpack
;       custom CMOS entries from the table.

POST_   SEGMENT
        PUBLIC  CustomCmosBits
CustomCmosBits LABEL BYTE
CMOS_CUSTOM_INDEX = 0
        REPT    TOTAL_CUSTOM_ENTRIES
        IRP     index, <%CMOS_CUSTOM_INDEX>
        db      CMOS_CUSTOM_ENTRY_&&index&&_BIAS
        db      CMOS_CUSTOM_ENTRY_&&index&&_BIT
        db      (0FFh SHR (8 - CMOS_CUSTOM_ENTRY_&&index&&_SIZE))
CMOS_CUSTOM_INDEX = (index + 1)
        ENDM
        ENDM

        ENDSEG

        IF      ((OPTION_SUPPORT_CMOS NE 0) AND (OPTION_SETUP_CUSTOM NE 0))

;       The following table is created by the OEM to define fields in
;       the Custom SETUP screen, if enabled by the OEM.  The table is
;       an array of words, terminated by the value 0ffffh.  Each entry
;       contains a pointer to one of the records defined with the
;       DCustom macro (see below).

SETUP_  SEGMENT

        PUBLIC  CustomTbl
CustomTbl LABEL WORD

        CUSTOMDEF CS_PCIINTA,   PCIINTA
        CUSTOMDEF CS_PCIINTB,   PCIINTB
        CUSTOMDEF CS_PCIINTC,   PCIINTC
        CUSTOMDEF CS_PCIINTD,   PCIINTD

        CUSTOMDEF CS_COM1DEF,   COM1DEF
        CUSTOMDEF CS_COM2DEF,   COM2DEF
        CUSTOMDEF CS_COM3DEF,   COM3DEF
        CUSTOMDEF CS_COM4DEF,   COM4DEF

        IF      OPTION_BOARD_IRDA
        CUSTOMDEF CS_IRDA,      IRDA
        ENDIF   ; (OPTION_BOARD_IRDA)

        CUSTOMDEF CS_LPT1DEF,   LPT1DEF

        CUSTOMDEF CS_VGADEF,    VGADEF
        CUSTOMDEF CS_VGABUF,    VGABUF

        IF      OPTION_BOARD_TFT
        CUSTOMDEF CS_VID,       VID
        CUSTOMDEF CS_VRFSH,     VRFSH
        CUSTOMDEF CS_VWDTH,     VWDTH
        CUSTOMDEF CS_PANEL,     PANEL
        ENDIF   ; (OPTION_BOARD_TFT)

        IF      OPTION_SETUP_CUSTOM_MEM_OPT
        CUSTOMDEF CS_MEM_OPT, MEM_OPT
        ENDIF   ; (OPTION_SETUP_CUSTOM_MEM_OPT)

        CUSTOMDEF CS_LEGACY_USB, LEGACY_USB

        IF      OPTION_ATA_CUSTOM_SETUP_CABLE
        CUSTOMDEF CS_IDE80WIRE, IDE80WIRE
        ENDIF   ; (OPTION_ATA_CUSTOM_SETUP_CABLE)

        IF      OPTION_SETUP_CUSTOM_SLEEPTYPE
        CUSTOMDEF CS_SLEEPTYPE, SLEEPTYPE
        ENDIF   ; (OPTION_SETUP_CUSTOM_SLEEPTYPE)
;JRB
        IF      OPTION_SETUP_CUSTOM_FLOPPY_CONT
        CUSTOMDEF CS_FDCSEL,    FDCSEL
        ENDIF   ; (OPTION_SETUP_CUSTOM_FLOPPY_CONT)

        IF      OPTION_GX3_ON_THE_GO
        CUSTOMDEF CS_USB_OTG, USB_OTG
        ENDIF   ; (OPTION_GX3_ON_THE_GO)

        CUSTOMDEF CS_USB_UDC, USB_UDC

        CUSTOMDEF CS_SPEEDSEL,  SPEEDSEL
        CUSTOMDEF CS_USB4_FUN, USB4_FUN

        dw      0ffffh                  ; end of table marker.

;       The DCustom macro is defined in SYSTEM\BOARD.ASM, and its arguments
;       are fields, as follows:
;
;       tag, title, datawidth, keyrtn, disprtn, loval, hival, strtbl

;       Here are definitions for the custom setup screen fields, which
;       were declared in the CustomTbl table above.  The tag field is a
;       label associated with the CUSTOMDEF entry in the above table.  The
;       title is what is displayed on the custom SETUP screen itself.
;       Note that the order in which the arrow keys navigate through the
;       SETUP screen is a function of the order in which the fields appear
;       in the *CustomTbl array*, NOT their DCustom definitions.

;       First Column.

DCustom PCIINTA,  'PCI INT A Assignment  : ', 12, KeyInt, DispStr, 0, PCI_NUM_INTS, PciIrqStrs
DCustom PCIINTB,  'PCI INT B Assignment  : ', 12, KeyInt, DispStr, 0, PCI_NUM_INTS, PciIrqStrs
DCustom PCIINTC,  'PCI INT C Assignment  : ', 12, KeyInt, DispStr, 0, PCI_NUM_INTS, PciIrqStrs
DCustom PCIINTD,  'PCI INT D Assignment  : ', 12, KeyInt, DispStr, 0, PCI_NUM_INTS, PciIrqStrs

        EXTRN   SioUartList:WORD

DCustom COM1DEF,  'COM 1 UART (3F8/IRQ 4): ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_UART_MAX, SioUartList
DCustom COM2DEF,  'COM 2 UART (2F8/IRQ 3): ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_UART_MAX, SioUartList
DCustom COM3DEF,  'COM 3 UART (3E8/IRQ 4): ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_UART_MAX, SioUartList
DCustom COM4DEF,  'COM 4 UART (2E8/IRQ 3): ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_UART_MAX, SioUartList

        IF      OPTION_BOARD_IRDA
DCustom IRDA,     'IRDA                  : ', 12, KeyInt, DispStr, 0,  2, IrdaStrs
        ENDIF   ; (OPTION_BOARD_IRDA)

        EXTRN   SioParallelList:WORD
DCustom LPT1DEF,  'LPT1: (378/IRQ7)      : ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_PARALLEL_MAX, SioParallelList

DCustom VGADEF,   'Primary video device  : ', 12, KeyInt, DispStr, 0,  3, VgaDefStrs
DCustom VGABUF,   'Geode LX graphics     : ', 12, KeyInt, DispStr, 0,  7, VgaBuffStrs

        IF      OPTION_BOARD_TFT
DCustom VID,      'Video device mode     : ', 12, KeyInt, DispStr, 0,  7, TftStrs
DCustom VRFSH,    'Video refresh rate    : ', 12, KeyInt, DispStr, 0,  4, RfshStrs
DCustom VWDTH,    'Video data width      : ', 12, KeyInt, DispStr, 0,  1, WdthStrs
DCustom PANEL,    'Video panel type      : ', 12, KeyInt, DispStr, 0,  1, PanelStrs
        ENDIF   ; (OPTION_BOARD_TFT)

        IF      OPTION_SETUP_CUSTOM_MEM_OPT
DCustom MEM_OPT,  'Memory Timings        : ', 12, KeyInt, DispStr, 0, 1, MemTimingStrs
        ENDIF   ; (OPTION_SETUP_CUSTOM_MEM_OPT)

DCustom LEGACY_USB, 'Legacy USB support    : ', 12, KeyInt, DispStr, 0, 2, UsbStrs

        IF      OPTION_ATA_CUSTOM_SETUP_CABLE
DCustom IDE80WIRE, 'IDE cable type        : ', 12, KeyInt, DispStr, 0, 1, Ide80wireStrs
        ENDIF   ; (OPTION_ATA_CUSTOM_SETUP_CABLE)
        IF      OPTION_SETUP_CUSTOM_SLEEPTYPE
DCustom SLEEPTYPE, 'Suspend Method        : ', 12, KeyInt, DispStr, 0, 1, SleepStrs
        ENDIF   ; (OPTION_SETUP_CUSTOM_SLEEPTYPE)
;JRB
        IF      OPTION_SETUP_CUSTOM_FLOPPY_CONT
        EXTRN   SioFloppyList:WORD
DCustom FDCSEL,   'Floppy controller     : ', 12, KeyInt, DispStr, 0, CONFIG_SETUP_SIO_FLOPPY_MAX, SioFloppyList
        ENDIF   ; (OPTION_SETUP_CUSTOM_FLOPPY_CONT)
        IF      OPTION_GX3_ON_THE_GO
DCustom USB_OTG,  'USB OTG Controller    : ', 12, KeyInt, DispStr, 0, 1, EnabStrs
        ENDIF   ; (OPTION_GX3_ON_THE_GO)

DCustom USB_UDC,  'USB Device Controller : ', 12, KeyInt, DispStr, 0, 1, EnabStrs

DCustom SPEEDSEL, 'CPU/GLIU speed        : ', 12, KeyInt, DispStr, 0, 31, SpeedsStrs
DCustom USB4_FUN, 'USB Port 4 Function   : ', 12, KeyInt, DispStr, 0, 2, Usb4Strs

;       Custom setup strings.

Usb4Strs label  word
        _STR    'Disabled'
        _STR    'Host    '
        _STR    'Device  '

VgaDefStrs label word
        _STR    'None        '
        _STR    'Auto        '
        _STR    'LX Graphics '
        _STR    'PCI VGA card'

VgaBuffStrs label word
        _STR    'Disabled'
        _STR    '2 MB    '
        _STR    '4 MB    '
        _STR    '8 MB    '
        _STR    '16 MB   '
        _STR    '32 MB   '
        _STR    '64 MB   '
        _STR    '128 MB  '

        IF      OPTION_BOARD_TFT
TftStrs label   word
        _STR    'Disabled '
        _STR    '320x240  '
        _STR    '640x480  '
        _STR    '800x600  '
        _STR    '1024x768 '
        _STR    '1152x864 '
        _STR    '1280x1024'
        _STR    '1600x1200'

RfshStrs label  word
        _STR    '60 Hz '
        _STR    '70 Hz '
        _STR    '75 Hz '
        _STR    '85 Hz '
        _STR    '100 Hz'

WdthStrs label  word
        _STR    '1 pix/clk'
        _STR    '2 pix/clk'

PanelStrs label word
        _STR    'TFT '
        _STR    'LVDS'
        ENDIF   ; (OPTION_BOARD_TFT)

        IF      OPTION_BOARD_IRDA
IrdaStrs label   word
        _STR    'Disabled'
        _STR    'SIR     '
        _STR    'MIR     '
        ENDIF   ; (OPTION_BOARD_IRDA)

        IF      OPTION_SETUP_CUSTOM_MEM_OPT
MemTimingStrs label word
        _STR    'Optimal     '
        _STR    'Conservative'
        ENDIF   ; (OPTION_SETUP_CUSTOM_MEM_OPT)

UsbStrs label   word
        _STR    'Disabled'
        _STR    'Enabled '
        _STR    'Auto    '

        IF      OPTION_ATA_CUSTOM_SETUP_CABLE
Ide80wireStrs label word
        _STR    '40-Wire'
        _STR    '80-Wire'
        ENDIF   ; (OPTION_ATA_CUSTOM_SETUP_CABLE)

        IF      OPTION_SETUP_CUSTOM_SLEEPTYPE
SleepStrs       label word
        _STR    'S1'
        _STR    'S3'
        ENDIF   ; (OPTION_SETUP_CUSTOM_SLEEPTYPE)

SpeedsStrs      label word
        _STR    '200/200 MHz'
        _STR    '233/200 MHz'
        _STR    '233/233 MHz'
        _STR    '266/200 MHz'
        _STR    '266/233 MHz'
        _STR    '266/266 MHz'
        _STR    '300/200 MHz'
        _STR    '300/266 MHz'
        _STR    '300/300 MHz'
        _STR    '333/200 MHz'
        _STR    '333/266 MHz'
        _STR    '333/333 MHz'
        _STR    '366/200 MHz'
        _STR    '366/266 MHz'
        _STR    '366/333 MHz'
        _STR    '366/366 MHz'
        _STR    '400/266 MHz'
        _STR    '400/333 MHz'
        _STR    '400/366 MHz'
        _STR    '400/400 MHz'
        _STR    '433/266 MHz'
        _STR    '433/333 MHz'
        _STR    '433/366 MHz'
        _STR    '433/400 MHz'
        _STR    '466/300 MHz'
        _STR    '466/333 MHz'
        _STR    '466/366 MHz'
        _STR    '466/400 MHz'
        _STR    '500/300 MHz'
        _STR    '500/333 MHz'
        _STR    '500/366 MHz'
        _STR    '500/400 MHz'

;       PciIrqStrs, similar to the other string sets above, describes the
;       options available for any given PCI Interrupt.  The list is dynamically
;       generated using the IRQ_LIST above and CONFIG_PCI_IRQ_BITMAP.

PciIrqStrs label word
        _STR    'Auto        '

PCI_NUM_INTS = 0
DEFINE_IRQ_STRINGS <%IRQ_LIST>

;       Strings used to display the current CAS latency.

SDRCas  label   word
        _STR    'RSVD    '
        _STR    'RSVD    '
        _STR    '2 CLKS  '
        _STR    '3 CLKS  '
        _STR    '4 CLKS  '
        _STR    '5 CLKS  '
        _STR    '6 CLKS  '
        _STR    'RSVD    '

DDRCas  label   word
        _STR    'RSVD    '
        _STR    'RSVD    '
        _STR    '2 CLKS  '
        _STR    '3 CLKS  '
        _STR    '4 CLKS  '
        _STR    '1.5 CLKS'
        _STR    '2.5 CLKS'
        _STR    '3.5 CLKS'

PCI_NUM_INTS = 0
DEFINE_IRQ_STRINGS <%IRQ_LIST>

        ENDSEG  ; (close SETUP)

        ENDIF   ; ((OPTION_SUPPORT_CMOS NE 0) AND (OPTION_SETUP_CUSTOM NE 0))

SETUP_  SEGMENT

;       Setup an array of the available interrupt numbers to use when assigning
;       PCI interrupts to specific values based in the custom setup screen.
;       As with PciIrqStrs, this list is dynamically generated.

PciIrqNumber label byte
        db       0

DEFINE_IRQ_LIST <%IRQ_LIST>

        ENDSEG  ; (close SETUP)

POST_   SEGMENT

;       Here, we iterate through each entry field in CMOS.  This allows
;       us to do the bit packing, as defined by the above macro.

CMOS_CUSTOM_INDEX = 0
        REPT    TOTAL_CUSTOM_ENTRIES
        IRP     index, <%CMOS_CUSTOM_INDEX>
SET_CUSTOM_DEFAULT      %CMOS_CUSTOM_ENTRY_&&index&&_INDEX,\
                        %CMOS_CUSTOM_ENTRY_&&index&&_BIT,\
                        %CMOS_CUSTOM_ENTRY_&&index&&_DEFAULT
CMOS_CUSTOM_INDEX = (index + 1)
        ENDM
        ENDM

;       This is the actual table definition for the default custom CMOS table.
;       We will store all of the custom CMOS cells in order, from the first
;       to the last.

        PUBLIC  DefaultCmosData
DefaultCmosData label byte
CMOS_CUSTOM_INDEX = 0
        REPT    (TOTAL_CUSTOM_CELLS+1)
        IRP     index, <%CMOS_CUSTOM_INDEX>
        db      CMOS_CSET&&index&&_DEFAULT
CMOS_CUSTOM_INDEX = (index + 1)
        ENDM
        ENDM
        db      0       ; Padding, just in case.

        ENDSEG  ; (close POST)

        IF      ((OPTION_SUPPORT_CMOS NE 0) AND (OPTION_SETUP_CUSTOM NE 0))

SETUP_  SEGMENT

;***    BoardInitFields - Initialize Custom SETUP Fields.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called to initialize the scratch area used by
;       the "Custom" SETUP screen to the values stored in the CMOS RAM
;       cache area.  If the OEM defines a custom SETUP screen, then this
;       routine must be written to perform this function.
;
;       Because OEM CMOS cell assignments and packing/unpacking of bit
;       fields within those cells are completely up to the OEM, this
;       routine is not defaulted to perform any action except return.
;
;   MODIFICATION HISTORY.
;       M. B. Boland    04/01/16.       #5.2, Adapted from Krill board module.
;       B. J. Brashier  04/02/26.       #5.2, toggles BoardData custom flag.
;
;   WARNINGS.
;       It is up to the BIOS adaptation engineer to find space in CMOS
;       and to define the Custom fields used by SETUP for the Custom
;       configuration screen.  See INC\CMOS.INC for CMOS cell assignments.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       flags.

DefProc BoardInitFields, PUBLIC, OVERRIDE
        PUSHALL

        IF      OPTION_SETUP_SIO
        Gcall   SioSetupInitialize, SETUP ; initialize SIO setup tables.
        ENDIF   ; (OPTION_SETUP_SIO)

;       Enable display of updating parameters.

        USEEXTDATA ds
        mov     byte ptr [BoardData+1], 1       ; enable temperature, speed displays.

;       Perform bit unpacking.

        USEBOOTDATA     es
        mov     ax, POST_SEG
        mov     ds, ax
        ASSUME  ds:POST_GRP

        mov     si, 0                   ; start with entry 0.
        mov     di, 0                   ; start at index 0.
        mov     cx, TOTAL_CUSTOM_ENTRIES ; go through all the custom entries.

BoardInitFields_Loop:
        mov     ax, si                  ; (AX) = setting to retrieve.
        push    bx
        push    cx

;       First, calculate index into bit packing descriptor.

        mov     bx, ax                  ; (BX) = (AX).
        add     bx, bx                  ; (BX) = (AX) * 2.
        add     bx, ax                  ; (BX) = (AX) * 3.

;       Next, obtain the actual first byte of CMOS data.

        mov     al, CustomCmosBits [bx]   ; (AL) = CMOS cell.
        mov     cl, CustomCmosBits [bx+1] ; (CL) = bit shift.
        mov     ch, CustomCmosBits [bx+2] ; (CH) = mask.
        mov     bl, al                  ; (BL) = preserve cell value.
        inc     al                      ; read next byte.
        Gcall   ReadCmosCache, BIOS     ; (AL) = high byte.
        mov     ah, al                  ; move next byte into high byte.
        mov     al, bl                  ; read first byte.
        Gcall   ReadCmosCache, BIOS     ; (AX) = current word of CMOS.
        shr     ax, cl                  ; (AX) = bit corrected value.
        sub     ah, ah                  ; clear high byte.
        and     al, ch                  ; (AX) = final value, after bit mask.
        pop     cx
        pop     bx
        mov     word ptr es:FieldValues.[di], ax
        inc     si
        inc     di
        inc     di
        loop    BoardInitFields_Loop     ; keep going to retrieve them all.

        POPALL
EndProc BoardInitFields

;***    BoardSaveFields - Save Custom Fields from Scratch Area.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called to store the scratch area values edited
;       by the "Custom" SETUP screen into the battery-backed CMOS memory.
;       If the OEM defines a custom SETUP screen, then this routine must
;       be written to perform this function.
;
;       Because OEM CMOS cell assignments and packing/unpacking of bit
;       fields within those cells are completely up to the OEM, this
;       routine is not defaulted to perform any action except return.
;
;   MODIFICATION HISTORY.
;       M. B. Boland    04/01/16.       #5.2, Adapted from Krill board module.
;       B. J. Brashier  04/02/26.       #5.2, toggles BoardData custom flag.
;
;   WARNINGS.
;       It is up to the BIOS adaptation engineer to find space in CMOS
;       and to define the Custom fields used by SETUP for the Custom
;       configuration screen.  See INC\CMOS.INC for CMOS cell assignments.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       flags.

DefProc BoardSaveFields, PUBLIC, OVERRIDE
        PUSHALL
        USEBOOTDATA     es
        mov     ax, POST_SEG
        mov     ds, ax
        ASSUME  ds:POST_GRP

        mov     ax, 0                   ; start with entry 0.
        mov     di, 0                   ; start at index 0.
        mov     cx, TOTAL_CUSTOM_ENTRIES; go through all the custom entries.

BoardSaveFields_Loop:
        mov     bx, word ptr es:FieldValues.[di]
        push    ax
        push    bx
        push    cx
        push    si

;       First, calculate index into bit packing descriptor.

        mov     si, ax                  ; (SI) = (AX).
        add     si, si                  ; (SI) = (AX) * 2.
        add     si, ax                  ; (SI) = (AX) * 3.

;       Next, shift and mask the contents of (BX).

        mov     al, CustomCmosBits [si+2] ; (AL) = mask value.
        sub     ah, ah                  ; (AX) = mask value.
        and     bx, ax                  ; Mask off unstored bits.
        mov     cl, CustomCmosBits [si+1] ; (CL) = bit shift.
        shl     bx, cl                  ; (BX) = corrected value.

;       Calculate mask value for clearing bits.

        shl     ax, cl                  ; (AX) = corrected mask value.
        not     ax                      ; Invert mask.
        mov     cx, ax                  ; (CX) = inverted mask.

;       Read a word of CMOS, so we can read/modify/write.

        mov     al, CustomCmosBits [si] ; (AL) = base index.
        inc     al
        Gcall   ReadCmosCache, BIOS     ; (AL) = high byte.
        mov     ah, al
        mov     al, CustomCmosBits [si] ; (AL) = base index.
        Gcall   ReadCmosCache, BIOS     ; (AX) = current word of CMOS.

;       Modify the word, before we write it back to CMOS cache.

        and     ax, cx                  ; clear bits which must be 0.
        or      ax, bx                  ; set bits which must be 1.

;       Update the CMOS cache with our new value.

        push    ax
        mov     ah, al
        mov     al, CustomCmosBits [si] ; (AL) = base index.
        Gcall   WriteCmosCache, BIOS    ; write low byte of CMOS data.
        pop     ax
        mov     al, CustomCmosBits [si] ; (AL) = base index.
        inc     al
        Gcall   WriteCmosCache, BIOS    ; write high byte of CMOS data.

        pop     si
        pop     cx
        pop     bx
        pop     ax
        inc     ax                      ; next entry.
        inc     di
        inc     di
        loop    BoardSaveFields_Loop    ; keep going to store them all.

;       Disable display of updating parameters.

        USEEXTDATA ds
        mov     byte ptr [BoardData+1], 0

        POPALL
EndProc BoardSaveFields

;***   SetupHlpDrawHorLine - Draw Horizontal Line.
;
;   FUNCTIONAL DESCRIPTION.
;       This helper routine is called to draw a horizontal line to separate
;       the power management block in the "Custom" SETUP screen.
;
;   MODIFICATION HISTORY.
;       E. Khoruzhenko  03/06/26.       #5.2, new.
;       B. J. Brashier  04/02/26.       #5.2, uses YPOS.
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

DefProc SetupHlpDrawHorLine, GLOBAL
        push    ax
        push    cx
        push    dx

        IF      OPTION_GX3_ON_THE_GO
        mov     dx, ((YPOS SHL 8) OR (00)) ; next clear row, column 0.
        ELSE
        mov     dx, (((YPOS + 1) SHL 8) OR (00)) ; next clear row, column 0.
        ENDIF   ; (OPTION_GX3_ON_THE_GO)
        Gcall   GotoXy, BIOS
        mov     al, 199                 ; print left side of line character.
        Pcall   PrintCh

        mov     cx, 78
        mov     al, 196                 ; print line character.
DrawHorLine_Loop:
        Pcall   PrintCh
        loop    DrawHorLine_Loop

        IF      OPTION_GX3_ON_THE_GO
        mov     dx, ((YPOS SHL 8) OR (79)) ; current row, column 79.
        ELSE
        mov     dx, (((YPOS + 1) SHL 8) OR (79)) ; current row, column 79.
        ENDIF   ; (OPTION_GX3_ON_THE_GO)
        Gcall   GotoXy, BIOS
        mov     al, 182                 ; print right side of line character.
        Pcall   PrintCh

        IF      OPTION_GX3_ON_THE_GO
        mov     dx, ((YPOS SHL 8) OR (40)) ; current row, column 40.
        ELSE
        mov     dx, (((YPOS + 1) SHL 8) OR (40)) ; current row, column 40.
        ENDIF   ; (OPTION_GX3_ON_THE_GO)
        Gcall   GotoXy, BIOS
        mov     al, 197                 ; print split line character.
        Pcall   PrintCh

        pop     dx
        pop     cx
        pop     ax
EndProc SetupHlpDrawHorLine

;***    BoardSetupCustomCtl - Custom Setup Control Routine.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called from the Setup engine throuth the BoardPostControl
;       to display the dynamically changing parameters like the board/CPU
;       temperature while waiting for a keypress.
;
;   MODIFICATION HISTORY.
;       E. Khoruzhenko  04/07/27.       #5.3, new.
;       R. Spiegel      05/05/26.       #5.3.3, correctly identify 5535/5536 and its rev ID.
;       R. Spiegel      05/05/27.       #5.3.3, use GLCP2_GLD_MSR_CAP to identify chip and rev.
;       K. C. Taylor    05/06/08.       #5.3.3, fixed 5535 version display bug.
;       R. Spiegel      05/07/12.       #5.3.3, fixed 5536 version number bug.
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
;       All, flags.

DefProc BoardSetupCustomCtl, GLOBAL
        PUSHALLD

        USEEXTDATA ds

        cmp     byte ptr [BoardData+1], 0
        jz      BoardTimerTick_Exit

        Gcall   SetupHlpDrawHorLine, SETUP

;       Position the cursor to print the system information.

        IF      OPTION_GX3_ON_THE_GO
        mov     dx, (((YPOS+1) SHL 8) OR 2) ; set cursor to skip a line, col 2.
        ELSE
        mov     dx, (((YPOS+2) SHL 8) OR 2) ; set cursor to skip a line, col 2.
        ENDIF   ; (OPTION_GX3_ON_THE_GO)

;       Get the current temperature of the board.

        IF      OPTION_BOARD_TEMPERATURE
        Gcall   BoardGetTemp, USER2     ; call board routine to get temp.
        mov     bl, ah                  ; move board temp to (BX)
        mov     bh, 0
        mov     ah, 0                   ; clear upper bytes for printf.

;       Position the cursor and print the CPU temperature.

        Gcall   GotoXy, BIOS
        mov     cx, 248                 ; ASCII character for degree symbol.
        PRINTF  <CPU temp / Board temp : $u$cC / $u$cC >, <ax, cx, bx, cx>
        inc     dh                      ; go to the next line.
        ENDIF   ; (OPTION_BOARD_TEMPERATURE)

        mov     dl, 2
        push    dx                      ; save the position for the next line.
        Gcall   GotoXy, BIOS            ; position to start of item.
        Gcall   BoardGetSpeeds, USER2   ; get all of the board speeds.

;       Print the CPU speed in MHz.

        PRINTF  <Core CPU Frequency : $u MHz>, ax

        mov     ax, dx                  ; save off the PCI speed.
        pop     dx                      ; restore the X and Y positions.

        inc     dh                      ; move down a line.
        push    dx                      ; save them for the next line.
        Gcall   GotoXy, BIOS            ; position to start of item.

        push    ax                      ; save the PCI speed.
        push    bx                      ; save the memory speed
        push    cx                      ; save the SDR/DDR flag.

;       Get the CAS latency.

        mov     ecx, GLMC_CF8F_DATA     ; read the memory timing register.
        ASM_RDMSR
        shr     eax, GLMC_CF8F_CAS_LAT_L_S
        and     eax, 7                  ; mask off the CAS field.
        add     ax, ax                  ; times to for word offset.
        shl     eax, 16                 ; save off the CAS latency in upper bits.

        pop     cx                      ; restore the SDR/DDR flag.
        pop     bx                      ; restore the memory speed.
        pop     ax                      ; restore the PCI speed.

        PRINTF  <Memory Frequency   : $u MHz DDR>, bx
        mov     ebx, eax                ; we have the CAS latency tucked away.
        shr     ebx, 16                 ; get the CAS latency back.
        pop     dx                      ; restore the X and Y positions.

        inc     dh                      ; move down a line.
        push    dx                      ; save them for the next line.
        Gcall   GotoXy, BIOS            ; position to start of item.
        PRINTF  <CAS Latency        : >

        push    SETUP_SEG
        pop     ds                      ; (DS) = POST segment.
        ASSUME  DS:SETUP_GRP

        mov     si, OFFSET SETUP_GRP:DDRCas ; (SI) = ofs FWA, string table.
        mov     si, word ptr ds:[si+bx] ; (SI) = ofs FWA, string to display.
        PRINTF  <$s>, <ds, si>          ; display the CAS timing string.
        pop     dx                      ; restore the X and Y positions.

;       Print the PCI speed.

        dec     dh                      ; move up to the top of this col.
        dec     dh                      ; move up to the top of this col.
        add     dl, 42                  ; move over to the second col.

        Gcall   GotoXy, BIOS            ; position to start of item.
        PRINTF  <PCI bus Frequency  : $u MHz >, ax

;       Print the GX3 Revision number.

        inc     dh                      ; move down a line.
        push    dx                      ; save them for the next line.
        Gcall   GotoXy, BIOS            ; position to start of item.
        mov     ecx, GLCP1_CHIP_REV_ID  ; read the CPU revision.
        ASM_RDMSR
        add     al, (09h SHL 4)         ; convert major rev to A, B, ... .
        PRINTF  <LX Rev. Number     : $b>, <ax>
        pop     dx                      ; restore the X and Y positions.

;       Print the CS5535/6 Revision number.

        inc     dh                      ; move down a line.
        Gcall   GotoXy, BIOS            ; position to start of item.

        mov     bx, 5535h               ; initial chip type.
        mov     ecx, GLCP2_GLD_MSR_CAP
        ASM_RDMSR                       ; (EAX) = 00xxxxyy. xxxx = dev_id, yy = rev_id.
        mov     edx, eax                ; (AL) = rev_id.
        shr     edx, 8                  ; (DX) = dev_id.

;       Check if the South Bridge chip is CS5536. If so, do 5536 translation.

        cmp     dx, GLCP2_GLD_MSR_CAP_5536
        mov     ecx, GLCP2_GLD_CHIP_REV_ID
        ASM_RDMSR                       ; (AL) = rev ID.
        je      BoardSetupCustomCtl_5536 ; if 5536.
        add     al, ((0ah - 1) SHL 4)   ; translate major rev to A, B, ... .
        jmp     @f
BoardSetupCustomCtl_5536:
        add     al, (0ah SHL 4)         ; translate major rev to A, B, ... .
        inc     bx                      ; indicate 5536.
        cmp     al, 0a2h                ; A2 was renamed to B0.
        jc      @f                      ; not B+.
        add     al, 0eh                 ; (B0 - A2).
@@:     PRINTF  <CS$x Rev. Number : $b >, <bx, ax>

BoardTimerTick_Exit:
        POPALLD
EndProc BoardSetupCustomCtl

        ENDSEG ; (close SETUP segment)

        ENDIF   ; ((OPTION_SUPPORT_CMOS NE 0) AND (OPTION_SETUP_CUSTOM NE 0))

SETUP_  SEGMENT

;***   BoardHelpReadCustomCmosSetup - Board Specific CMOS Retrieval Procedure.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is used to extract a specific CMOS value, so that it
;       can be used in conjunction with the custom setup screen.
;
;   MODIFICATION HISTORY.
;       M. B. Boland    04/01/19.       #5.2, adapted from Geode package.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       (AX)    = Specific CMOS custom setup field desired.
;
;   EXIT.
;       (AX)    = Contents of CMOS cache value for custom setup field.
;
;   USES.
;       flags.

DefProc BoardHelpReadCustomCmosSetup, GLOBAL
        IF      OPTION_SETUP_CUSTOM
        push    bx
        push    cx
        push    es

;       First, calculate index into bit packing descriptor.

        mov     bx, ax                  ; (BX) = (AX).
        add     bx, bx                  ; (BX) = (AX) * 2.
        add     bx, ax                  ; (BX) = (AX) * 3.

;       Point ES at POST_SEG.

        mov     ax, POST_SEG            ; (AX) = post segment value.
        mov     es, ax                  ; (ES) = post segment value.
        ASSUME  ES:POST_GRP             ; assume (ES) points at post group.

;       Obtain the actual first byte of CMOS data.

        mov     al, CustomCmosBits [bx]   ; (AL) = CMOS cell.
        mov     cl, CustomCmosBits [bx+1] ; (CL) = bit shift.
        mov     ch, CustomCmosBits [bx+2] ; (CH) = mask.
        mov     bl, al                  ; (BL) = preserve cell value.
        inc     al                      ; read next byte.
        Gcall   ReadCmosCache, BIOS     ; (AL) = high byte.
        mov     ah, al                  ; move next byte into high byte.
        mov     al, bl                  ; read first byte.
        Gcall   ReadCmosCache, BIOS     ; (AX) = current word of CMOS.
        shr     ax, cl                  ; (AX) = bit corrected value.
        sub     ah, ah                  ; clear high byte.
        and     al, ch                  ; (AX) = final value, after bit mask.
        pop     es
        pop     cx
        pop     bx
        ENDIF   ; (OPTION_SETUP_CUSTOM)
EndProc BoardHelpReadCustomCmosSetup

;***   BoardHelpPciRoute - Set PCI interrupts to values in custom setup.
;
;   FUNCTIONAL DESCRIPTION.
;       This function takes the values in the custom setup fields to force PCI
;       interrupt routing if required.  It is called from BoardPciControl in the
;       board module.
;
;   MODIFICATION HISTORY.
;       M.B. Boland     03/08/17.       #5.2, new.
;       M.B. Boland     04/01/19.       #5.2, adapted to new custom setup.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       EDX contains the current PCI interrupt assignments.
;
;   EXIT.
;       EDX contains the corrected PCI interrupt assignments.
;
;   USES.
;       AX, BX, EDX, Flags.

DefProc BoardHelpPciRoute, GLOBAL
        IF      OPTION_SETUP_CUSTOM
        mov     ax, CMOS_CUSTOM_PCI_A   ; get INT assignment for PCI INTA.
        Pcall   BoardHelpReadCustomCmosSetup
        mov     bx, ax                  ; put the ordinal into BX for array index.
        mov     al, byte ptr PciIrqNumber [bx]
        or      al, al                  ; 0 means we leave it alone for auto route.
        jz      BoardHelpPciRoute_Pci1
        mov     dl, al

BoardHelpPciRoute_Pci1:
        mov     ax, CMOS_CUSTOM_PCI_B   ; get INT assignment for PCI INTB.
        Pcall   BoardHelpReadCustomCmosSetup
        mov     bx, ax                  ; put the ordinal into BX for array index.
        mov     al, byte ptr PciIrqNumber [bx]
        or      al, al                  ; 0 means we leave it alone for auto route.
        jz      BoardHelpPciRoute_Pci2
        mov     dh, al

BoardHelpPciRoute_Pci2:
        rol     edx, 16                 ; swap to allow rest of EDX to get set.
        mov     ax, CMOS_CUSTOM_PCI_C   ; get INT assignment for PCI INTC.
        Pcall   BoardHelpReadCustomCmosSetup
        mov     bx, ax                  ; put the ordinal into BX for array index.
        mov     al, byte ptr PciIrqNumber [bx]
        or      al, al                  ; 0 means we leave it alone for auto route.
        jz      BoardHelpPciRoute_Pci3
        mov     dl, al

BoardHelpPciRoute_Pci3:
        mov     ax, CMOS_CUSTOM_PCI_D   ; get INT assignment for PCI INTD.
        Pcall   BoardHelpReadCustomCmosSetup
        mov     bx, ax
        mov     al, byte ptr PciIrqNumber [bx]
        or      al, al                  ; 0 means we leave it alone for auto route.
        jz      BoardHelpPciRoutePci_Done
        mov     dh, al

BoardHelpPciRoutePci_Done:
        rol     edx, 16                 ; put edx back in the right order.
        ENDIF   ; (OPTION_SETUP_CUSTOM)
EndProc BoardHelpPciRoute

        ENDSEG ; (close SETUP segment)

POST_   SEGMENT

;***    BoardResetCmos - Board Specific Reset of CMOS to Vendor Specifications.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by ResetCmos when a need exist to refresh
;       CMOS to factory defaults.
;
;       This routine may either set the CMOS to appropriate values, or
;       may return with carry set; in which case ResetCmos will set the
;       CMOS to values which are contained in the table CmosData, which
;       is assembled into ResetCmos.
;
;       This routine should only be employed when some board specific
;       CODE is needed. Overrides should be placed in the project include
;       file when board specific VALUES are needed.
;
;   MODIFICATION HISTORY.
;       M. B. Boland    04/01/16.       #5.2, Adapted from Krill board module.
;       E. Khoruzhenko  04/06/25.       #5.3, clear all CMOS before writing defaults.
;
;   WARNINGS.
;       This routine can't modify BP because it is used for return linkage.
;
;   ENTRY.
;       none.
;
;   EXIT.
;       none.
;
;   USES.
;       ALL but BP.

DefHyb  BoardResetCmos, GLOBAL, OVERRIDE
        mov     di, bp                  ; (DI) = saved return address.

;       First clear all CMOS to zeros.
;       Make (CX) = number of CMOS cells to clear.
;       Make (AL) = starting offset, (AH) = 0.

        mov     cx, (CONFIG_MAX_CMOS_LOCATIONS - CONFIG_START_CMOS_CACHE)
        mov     ax, CONFIG_START_CMOS_CACHE
BoardResetCmos_ClearLoop:
        Rcall   PostWriteCmos
        inc     al
        loop    BoardResetCmos_ClearLoop

;       Write default data into CMOS.

        mov     si, OFFSET POST_GRP:DefaultCmosData
        mov     cx, (TOTAL_CUSTOM_CELLS + 1)
        mov     al, CONFIG_START_BOARD_CMOS

BoardResetCmos_TableLoop:
        mov     ah, byte ptr cs:[si]    ; (AH) = CMOS value.
        Rcall   PostWriteCmos
        inc     al
        inc     si
        loop    BoardResetCmos_TableLoop

        mov     bp, di                  ; (BP) = restored return address.
        stc                             ; board module has updated checksum.
EndHyb  BoardResetCmos

;***    BoardHelpReadCustomCmos - Board Specific CMOS Retrieval Procedure.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is stackless version of BoardHelpReadCustomCmosRcall used to
;       extract a specific CMOS value, so that it can be used in conjunction
;       with the custom setup screen early in the POST process.  This routine
;       is called with values established by the custom CMOS setup macros instead
;       of an index into a table of values as used in the stack version of this
;       routine.
;
;   MODIFICATION HISTORY.
;       M. B. Boland    04/01/21.       #5.2, new.
;       M. B. Boland    04/01/26.       #5.2, changed to not use CMOS index table.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       (AX)    = Custom CMOS reference.
;
;   EXIT.
;       (AX)    = Contents of CMOS cache value for custom setup field.
;
;   USES.
;       AX, BX, CX, EDX, flags.

DefRtn  BoardHelpReadCustomCmos, PUBLIC
        IF      OPTION_SETUP_CUSTOM
        mov     dx, bp                  ; save off our return address.
        shl     edx, 16
        mov     bp, ax                  ; (BP) = starting offset for lookup.
        add     bp, bp                  ; (BP) = (BP) * 2.
        add     bp, ax                  ; (BP) = starting index * 3.
        mov     al, cs:CustomCmosBits [bp] ; (AL) = starting CMOS cell.
        mov     cl, cs:CustomCmosBits [bp+1] ; (CL) = number of bits to read.
        mov     ch, cs:CustomCmosBits [bp+2] ; (CH) = bit mask.
        mov     bl, al                  ; (BL) = preserve cell value.
        inc     al                      ; read next byte.
        Rcall   PostReadCmos            ; (AL) = high byte.
        mov     ah, al                  ; move next byte into high byte.
        mov     al, bl                  ; read first byte.
        Rcall   PostReadCmos            ; (AL) = low byte.
        shr     ax, cl                  ; (AX) = bit corrected value.
        sub     ah, ah                  ; clear high byte.
        and     al, ch                  ; (AX) = final value, after bit mask.
        shr     edx, 16                 ; restore our return address.
        mov     bp, dx
        ENDIF   ; (OPTION_SETUP_CUSTOM)
EndRtn  BoardHelpReadCustomCmos

        ENDSEG  ; (close POST).

        ENDIF   ; ((OPTION_SUPPORT_SETUP NE 0) AND (OPTION_SETUP_CUSTOM NE 0))
