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
;       R. Spiegel      05/04/27.       #5.3, new for Salsa board.
;
;3.     Warnings.
;       None.
;
;4.     NOTICE: Copyright (C) 1998-2005 General Software, Inc.

;       The following table defines the ordering of the fields
;       on the Custom Configuration Setup screen.  The fields are
;       grouped in pairs, the first field is displayed on the left
;       side of the screen and the second is displayed on the right
;       side of the screen.

CUSTOMDEF MACRO fieldname, fieldlabel
fieldname =     ($-CustomTbl)
        dw      OFFSET SETUP_GRP:fieldlabel
        ENDM

;       The following macro is used to define the data structure used
;       to represent a field on the Custom Configuration SETUP screen.
;       It is the BIOS adaptation engineers job to define the fields
;       that will be displayed on the Custom Configuration screen.
;       Example fields are provided in the TEMPLATE.ASM modules.

        XPOS    = 2                     ; first usable column.
        YPOS    = 4                     ; first usable row.

DCustom MACRO   tag, title, datawidth, keyrtn, disprtn, loval, hival, strtbl
        LOCAL   len

PUBLIC  tag
tag     label   word
        _STR    title, len              ;; offset of title string.
        dw      len                     ;; length of string (less zero byte).
        db      XPOS                    ;; column to start title at.
        db      YPOS                    ;; row to start title at.
        db      (XPOS+len)              ;; col to start data at.
        db      YPOS                    ;; row to start data at.
        dw      datawidth               ;; width of data field to highlight.
        dw      OFFSET SETUP_GRP:_p_&keyrtn ;; offset of routine to call when a key is hit.
        dw      OFFSET SETUP_GRP:_p_&disprtn ;; offset of update routine.
        dw      loval                   ;; lowest acceptable value.
        dw      hival                   ;; highest acceptable value.
        dw      OFFSET SETUP_GRP:strtbl ;; ofs FWA, table of strings to display.

        IF      (XPOS EQ 2)
XPOS    = 42
        ELSE    ; (XPOS NE 2)
XPOS    = 2
        IRP     index, <%YPOS>
YPOS    = (index + 1)
        ENDM
        ENDIF   ; (XPOS NE 2)

        ENDM

;       The DEF_CUSTOM_CMOS macro is used to define individual bit packed
;       fields in the custom setup screen.  It also makes it easy to extract
;       the fields later by identifier.

DEF_CUSTOM_CMOS MACRO name, numbits, default

        IRP     index, <%TOTAL_CUSTOM_ENTRIES>
CMOS_CUSTOM_&name =                     index
CMOS_CUSTOM_BIAS_&name =                BIAS
CMOS_CUSTOM_BIT_&name =                 BIAS_BIT
CMOS_CUSTOM_SIZE_&name =                (0FFh SHR (8 - numbits))
CMOS_CUSTOM_DEFAULT_&name =             default
        IRP     value, <%TOTAL_CUSTOM_CELLS>
CMOS_CUSTOM_ENTRY_&&index&&_INDEX =     value
        ENDM
CMOS_CUSTOM_ENTRY_&&index&&_BIAS =      BIAS
CMOS_CUSTOM_ENTRY_&&index&&_BIT =       BIAS_BIT
CMOS_CUSTOM_ENTRY_&&index&&_SIZE =      numbits
CMOS_CUSTOM_ENTRY_&&index&&_DEFAULT =   default
TOTAL_CUSTOM_ENTRIES = (index + 1)
        ENDM

        IRP     index, <%TOTAL_CUSTOM_CELLS>
CMOS_CSET&&index = BIAS
        ENDM

        IF      ((numbits+BIAS_BIT) GT 7)

        IRP     index, <%TOTAL_CUSTOM_CELLS>
TOTAL_CUSTOM_CELLS = (index + 1)
        ENDM

BIAS    =       (BIAS + 1)

        IRP     index, <%TOTAL_CUSTOM_CELLS>
CMOS_CSET&&index = BIAS
CMOS_CSET&&index&&_DEFAULT = 0
        ENDM

        ENDIF

        IRP     index, <%BIAS_BIT>
BIAS_BIT = ((index+numbits) AND 7)
        ENDM
        ENDM

;       The SET_CUSTOM_DEFAULT macro actually creates the defaults for each
;       field from the table used to set them.

SET_CUSTOM_DEFAULT MACRO base, bit, defval
baseplusone = (base + 1)
        IFDEF   CMOS_CSET&base&_DEFAULT
CMOS_CSET&base&_DEFAULT = (CMOS_CSET&base&_DEFAULT OR (0FFh AND (defval SHL bit)))
        ELSE
CMOS_CSET&base&_DEFAULT = (0FFh AND (defval SHL bit))
        ENDIF
        IRP     value, <%baseplusone>
CMOS_CSET&&value&&_DEFAULT = ((0FF00h AND (defval SHL bit)) SHR 8)
        ENDM
        ENDM

;       IRQ_LIST lists all IRQs that could possibly ever be usable by PCI.
;       This list does not need to be modified, because it uses
;       CONFIG_PCI_IRQ_BITMAP in the project file as a reference.

IRQ_LIST        EQU     <3, 4, 5, 7, 9, 10, 11, 12, 13, 14, 15>

;       The DEFINE_IRQ_STRINGS macro dynamically determines which IRQs are
;       able to be used for PCI based on the CONFIG_PCI_IRQ_BITMAP in the
;       project file, and creates the strings required for each when
;       choosing them from the Custom Setup screen.

DEFINE_IRQ_STRINGS MACRO list
        IRP     irq, <list>
        LOCAL   str
        IF      (CONFIG_PCI_IRQ_BITMAP AND (01b SHL irq))
        TEXTSEG
        IF      (irq GT 9)
str     db      'IRQ 1', (30h + irq - 10), 0    ;; ASCIIZ string.
len     EQU    ($-str-1)                ;; don't include 0-byte in length.
        ELSE    ; (irq GT 9)
str     db      'IRQ ', (30h + irq), ' ', 0     ;; ASCIIZ string.
len     EQU    ($-str-1)                ;; don't include 0-byte in length.
        ENDIF   ; (irq GT 9)
        ENDSEG
        CURSEG
        IRP     name, <%@CurSeg>
         dw     offset name&&&GRP:str    ;; loc of title string.
        ENDM
PCI_NUM_INTS = (PCI_NUM_INTS+1)
        ENDIF
        ENDM
        ENDM

;       The DEFINE_IRQ_LIST macro creates a list in the ROM of the available
;       IRQs for PCI.  As with DEFINE_IRQ_STRINGS, it determines this list
;       dynamically based on CONFIG_PCI_IRQ_BITMAP.

DEFINE_IRQ_LIST MACRO list
        IRP     irq, <list>
        IF      (CONFIG_PCI_IRQ_BITMAP AND (01b SHL irq))
        db      irq
        ENDIF
        ENDM
        ENDM


