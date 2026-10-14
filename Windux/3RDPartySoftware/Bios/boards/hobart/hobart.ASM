        TITLE   SALSA - Board module for AMD LXDB800 "Salsa" Board.

;***    SALSA -- Board module for AMD LXDB800 GX3/CS5535 "Salsa" Board.
;
;1.     Functional Description.
;       This file contains custom code created by the BIOS adaptation
;       engineer to handle board-level options.  This code is not supplied
;       by General Software with the core system BIOS.  This module supports
;       the AMD LXDB800 GX3/CS5535 ("Salsa") development platform.
;
;       This adaptation of Embedded BIOS 2000 supports this board with the
;       GX3 chipset module and the GX3 CPU module.
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
;
;       GX3 specific Firmbase components include:
;       - SoftVGA
;       - Hardware emulation (Virtual PCI)
;       - Power management
;
;       The development kit consists of an evaluation board with the GX3
;       processor bonded for either CRT or TFT displays, technical documentation,
;       and expansion hardware. The LXDB800 platform allows the GX3 processor and
;       memory frequency to be scaled from 166MHz to 600MHz and 166MHz to 400MHz
;       respectively by simply changing values on custom setup.
;
;       The baseboard's one 184-pin DDR SDRAM slots support up to 512 MB of memory.
;       The one 3.3V PCI expansion slots support one PCI devices at 33MHz or
;       66MHz. Additional connectors include, one 40-pin IDE header supporting
;       up to two UDMA4 devices, two USB 1.1 ports for Legacy USB and boot
;       from USB Mass Storage devices, one 40-pin TFT flat-panel header, one
;       LPC expansion slot, and audio jacks.
;
;       This adaptation of Embedded BIOS 2000 supports this board with
;       the GEODE GX3 chipset module, the GEODE GX3 package, the GX3 CPU module,
;       automatic loading of Firmbase, USB, and the GX3 SMM software.
;
;       These are the features implemented in this adaptation:
;       - APM (power button, standby, suspend, soft off);
;       - ACPI (S0, S1, S3, S4, S5);
;       - Wake events for APM and ACPI: keyboard (only S1/standby), mouse
;         (only S1/standby), RTC, power button;
;       - Virtual audio controller (AC97 compatible) (Firmbase GX3 driver);
;       - VSA2 legacy (virtual registers used by OS drivers) (Firmbase GX3 driver);
;       - USB legacy devices;
;       - USB mass storage devices;
;       - PS2 keyboard and mouse detection and support;
;       - configurable support for 4 COM ports: board has four 16650 compatible
;         UARTS and two IrDA UART. Any of those six UARTS can be assigned to any
;         COM port;
;       - IrDA capable of SIR or MIR;
;       - Supports video with integrated video controller or with add-on PCI
;         video card; or supports console redirection over serial port;
;       - automatic multiple SIO detection, configuration, and power management;
;       - UDMA 0, 1, 2, 3, and 4;
;       - PnP (provided by GX3 package);
;       - PCI IRQ routing;
;       - Shadow control in 16K blocks;
;       - Firmbase timer defaults to 64 ms, can be programed from 1 to 255 ms.
;
;       Board-level modules contain replacement routines for the standard
;       ones found in SYSTEM\BOARD.ASM.  Do not just copy the contents of
;       BOARD.ASM to this file, because it contains statements which allow
;       it to define default board routines *only if* you do not specify
;       your own routines here.
;
;       Note some routines call the corresponding chipset module routines
;       or CPU module routines.  This allows you to intercept these calls
;       and perform actions before or after the various calls, as necessary,
;       to accommodate special situations.  If you want to perform something
;       similar to what the standard CPU or chipset module does, but not
;       exactly, then one option for you is to disable the call to the
;       chipset or CPU module in that routine, and then copy the code from
;       the called module and apply the changes locally in the board routine.
;       Please resist the temptation to edit the chipset or CPU modules
;       that come with Embedded BIOS, because modifications will later
;       become a problem when you want to take advantage of software updates.
;
;       Because this module is INCLUDED by module BOARD.ASM, and because
;       BOARD.ASM automatically includes all of the header files, this
;       file does not include the standard header files.
;
;2.     Modification History.
;       R. Spiegel      05/04/27.       #5.3, new.
;       E. Khoruzhenko  05/07/14.       #5.3.3, fixed late display of OEM version.
;
;3.     Warnings.
;       None.
;
;4.     NOTICE: Copyright (C) 2001-2005 General Software, Inc.

        include ..\inc\pciapi.inc

        IRP     name, <%BOARD>                  ;; force substitution!
        include ..\boards\&name&\&name&.inc
        include ..\boards\&name&\setup.inc      ;; include custom setup menu.
        include ..\boards\&name&\setup.asm      ;; include custom setup menu.
        ENDM

        IRP     name, <%CHIPSET>
        include ..\chipsets\&name&\&name&.inc
        include ..\chipsets\&name&\cs5536.inc
        ENDM

POST_   SEGMENT
OemVerSplash    db      CONFIG_BOARD_SPLASH_VER, 0
OemVerSplash_Len = $ - OemVerSplash
OemVerSplash1   db      BIOS_LICENSEE, 0
OemVerSplash1_Len = $ - OemVerSplash1

;***   BoardInit0 - Initialize Board Hardware, First Entrypoint.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called by POST to initialize board (CPU & chipset)
;       hardware that must be set up before POST starts (before the CMOS
;       shutdown byte is tested).
;
;       On all GX3-based systems employing an AMD CS5535 I/O companion,
;       we must tell the GX3 and the CS5535 to start talking to each other,
;       and then initialize key MSRs on both the GX3 and CS5535 to allow
;       CsInit0 to run. This is handled by this routine first in an MSR
;       table load.
;
;       IMPORTANT:  This is necessary for the DIP or PLCC BIOS sockets
;       on the Wren card to operate properly; otherwise, BIOS must be
;       located on the Firmware Hub socket on the Salsa board.
;
;       This routine also determines whether we've booted cold or warm.
;       Because this is handled differently on different board designs
;       (simple memory test as this does, or testing 8042 bit, etc.),
;       this can't be done as part of the POST mainline code.
;
;   MODIFICATION HISTORY.
;       R. Spiegel      05/04/27.       #5.3, new.
;
;   WARNINGS.
;       This routine can't modify BP because it is used for return linkage.
;
;   ENTRY.
;       DS      - BIOSDATA segment (0040h)
;       SP      - 0 if cold boot, -1 if maybe warm boot.
;
;   EXIT.
;       SP      - 0 if force cold boot, -1 if maybe warm boot.
;
;   USES.
;       all except DS, SP, BP, INCLUDING SS.

DefRtn  BoardInit0, PUBLIC, OVERRIDE
        ASSUME  DS:BIOSDATA
        mov     gs, bp

        invd

;       Enable cache right here to run faster if OPTION_CACHE_EARLY is enabled
;       in the project file.

        IF      OPTION_CACHE_EARLY
        mov     eax, cr0
        and     eax, (NOT (GX3CPU_CR0_DISABLECACHE OR GX3CPU_CR0_NOWRTHROUGH))
        mov     cr0, eax                ; enable the cache.
        ENDIF   ; (OPTION_CACHE_EARLY)

;       Start Timer1 to allow to use port 61h.

        mov     al, 056h
        out     TIMER_PORT_CMD, al
        mov     al, 012h
        out     TIMER_PORT_1, al

;       Check if this is a warm boot and if so, skip any further initialization
;       and go straight to chipset module, which will handle it appropriately.

        mov     ecx, GLIU0_DESC_P2D_SC  ; (ECX) = "swiss cheese" register.
        ASM_RDMSR                       ; (EAX) = swiss cheese register contents.
        and     eax, 0FFFF0000h         ; are the read enables set anywhere?
        jz      BoardInit0_Cold         ; if not, it is a cold boot.

        Rcall   PostTestResetValue      ; test cause of reset & branch to rtn.
        jmp     BoardInit0_Warm         ; go to warm boot path if we return.

BoardInit0_Cold:
        Rcall   PostStartCmos           ; Init CMOS to known state.

;       Load the table of MSRs that need to be programmed at this point.
;       This must be done first, because it programs the GX3 and CS5535
;       to start communicating, and we can't talk to the CS5535 South
;       Bridge unless we've done this.

        mov     si, OFFSET POST_GRP:MsrTblBoardInit0
        Rcall   CsHelpLoadMsrTable

;       Program and start timer 5 to use as a time to boot timer.

        mov     dx, ( DIVIL_MFGPT_BASE + \
                      DIVIL_MFGPT_BASE5 + \
                      DIVIL_MFGPT_CMP2 )
        mov     ax, DIVIL_MFGPT_SETUP_MAX_COUNT
        out     dx, ax                  ; set the compare register to max.
        mov     dx, (DIVIL_MFGPT_BASE + DIVIL_MFGPT_BASE5 + DIVIL_MFGPT_SETUP)
        mov     ax, DIVIL_MFGPT_SETUP_MAX_COUNT
        out     dx, ax                  ; config and start the counter.

;       Now that we've got the south bridge setup properly, we have a valid
;       GPIO_BASE and can set up the pins properly to allow the board's SMBUS
;       to function properly. The SMBUS is needed by CsMemConfig to read the
;       memory SPD information.

        mov     eax, (DIVIL_GPIO14_SET OR DIVIL_GPIO15_SET)
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_OUT_AUX1_SEL)
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_OUT_EN) ; enable the output pins.
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_IN_AUX1_SEL) ; route the input pins.
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_IN_EN)  ; enable the pins for input.
        out     dx, eax

;       MFGPT timer 7 is used to blink the Power LED in the low power states.

        mov     dx, (CONFIG_GX3_MFGPT_BASE+DIVIL_MFGPT_BASE7) ; (DX) = MFGPT 7 base address.
        mov     eax, CONFIG_BOARD_PWR_LED_DUTY_RATE
        out     dx, eax
        add     dx, DIVIL_MFGPT_SETUP
        mov     ax, 8085h               ; turn the timer on.
        out     dx, ax

;       Program GPIO27 to be open drain output to control the Power LED.

        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOH_OUT_VAL)
        mov     eax, DIVIL_GPIO27_CLR
        out     dx, eax
        add     dx, (DIVIL_GPIOH_OUT_EN-DIVIL_GPIOH_OUT_VAL)
        mov     eax, DIVIL_GPIO27_SET
        out     dx, eax
        add     dx, (DIVIL_GPIOH_OUT_OD_EN-DIVIL_GPIOH_OUT_EN)
        out     dx, eax

;       Enable PC Speaker.

        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_OUT_AUX1_SEL)
        mov     eax, DIVIL_GPIO01_SET
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOL_OUT_EN)
        out     dx, eax

;       Power Button Setup (GPIO28).

        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOH_IN_FIL_EN)
        mov     eax, DIVIL_GPIO28_SET   ; enable debounce.
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIO_FE6)
        mov     al, 28                  ; filter GPIO28.
        out     dx, al
        mov     eax, 40020000h          ; 4 seconds no lock.
        mov     dx, (CONFIG_GX3_PMS_BASE+40h)
        out     dx, eax
        in      eax, dx                 ; work around CS5535 write-post problem.

;       Setup GPIO24 as it is the external signal for CS5535 vsb_work_aux
;       which controls all voltage rails except Vstandby & Vmem.  We need
;       to enable, OUT_AUX1 and OUTPUT_ENABLE in this order.  If GPIO24
;       is not enabled, then soft-off will not work.

        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOH_OUT_AUX1_SEL)
        mov     eax, DIVIL_GPIO24_SET
        out     dx, eax
        mov     dx, (CONFIG_GX3_GPIO_BASE+DIVIL_GPIOH_OUT_EN)
        out     dx, eax

;       If SIO core support is enabled, call the SIO core to initialize SIOs.

        IF      OPTION_SUPPORT_SIO_CORE
        Rcall   SioInitialize

;       Enable port 80h display on Wren Card very early for debugging.

        mov     dx, LPC_XBUS_XZCNF2
        mov     al, LPC_XBUS_XZCNF2_PORT80
        out     dx, al

;       This code executes before PostCodeComInit is called in system\POST.ASM.
;       To enable POSTCODECOM debugging before that point, call it here to
;       enable POSTCODECOM output.  Note that POSTCODECOM and variants may
;       hang if CONFIG_WAIT_PROGRESS_COM = 0 if the UART isn't configured
;       since it will endlessly loop on the status flag in the UART.

        Rcall   BoardHelpRoutePorts

        mov     di, SIO_INTERFACE_SET_ENABLE
        mov     si, SIO_DEVICE_TYPE_KEYBOARD
        sub     cx, cx                  ; first controller found.
        mov     bx, 1                   ; enable.
        Rcall   SioInterface            ; enable first KBC found.

        mov     di, SIO_INTERFACE_SET_ENABLE
        mov     si, SIO_DEVICE_TYPE_MOUSE
        sub     cx, cx                  ; first controller found.
        mov     bx, 1                   ; enable.
        Rcall   SioInterface            ; enable first mouse controller found.

        ENDIF   ; OPTION_SUPPORT_SIO_CORE

        IF      OPTION_SUPPORT_POSTCODES_COM
        Rcall   BoardPostCodeComInit
        ENDIF   ; (OPTION_SUPPORT_POSTCODES_COM)

;       Give the CPU and chipset modules a chance to initialize their
;       hardware. These routines may set SP to a boot indicator value,
;       which if nonzero, means we have memory and can test the low
;       memory boot flag.  If it is zero, then we can't check it.

BoardInit0_Warm:
        Rcall   CpuInit0                ; initialize the CPU module.
        Rcall   CsInit0                 ; initialize the chipset module.

;       After returning from CsInit0, our GX3 PLL divisors are programmed.
;       Now (SP) = 0 for cold boot, else nonzero for warm boot, from CsInit0.

        mov     bp, gs
EndRtn  BoardInit0

;***   BoardPostControl - Handling POST Control Functions.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine handles POST control functions that handle special policies
;       dealing with POST.  For example, how alarms should be handled as
;       bootable actions, etc.
;
;   MODIFICATION HISTORY.
;       R. Spiegel      05/04/27.       #5.3, new.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       AX -  function control code, as follows:
;               POST_CTL_ALARM          - process alarm boot action.
;               POST_CTL_BANNER1        - display power on banner1 to banner3.
;               POST_CTL_BANNER2        - display power on banner4.
;               POST_CTL_BANNER3        - display BIOS ID string.
;               POST_CTL_BANNER4        - display "Wait.....".
;               POST_CTL_BOOT_DESKTOP   - boot Firmbase desktop.
;               POST_CTL_BOOT_EFI       - boot to Extensible Firmware Interface.
;               POST_CTL_BOOT_NETWORK   - process network boot action.
;               POST_CTL_CFGBOX         - display the system config box.
;               POST_CTL_HIMEM1         - display initial high memory address.
;               POST_CTL_HIMEM2         - display current high memory address.
;               POST_CTL_HIMEM3         - click speaker.
;               POST_CTL_HIMEM4         - display total high memory size.
;               POST_CTL_HITDEL         - display "hit <Del>....".
;               POST_CTL_INT18H         - display no-boot INT 18h.
;               POST_CTL_LOWMEM1        - display initial low memory address.
;               POST_CTL_LOWMEM2        - display current low memory address.
;               POST_CTL_LOWMEM3        - click speaker.
;               POST_CTL_LOWMEM4        - display total low memory size.
;               POST_CTL_MAINT          - process maintenance boot action.
;               POST_CTL_PCIDUMP        - display PCI table dump.
;               POST_CTL_POSTCODE       - process 16-bit(driver) post code.
;               POST_CTL_POWEROFF       - process poweroff boot action.
;               POST_CTL_REBOOT         - process reboot boot action.
;               POST_CTL_REG_ERASE      - erase platform registration data.
;               POST_CTL_REG_GETID      - retrieve platform ID data.
;               POST_CTL_REG_GETINFO    - retrieve platform registration data.
;               POST_CTL_REG_PUTINFO    - store platform registration data.
;               POST_CTL_REG_SETID      - store platform ID data.
;               POST_CTL_SOFTERR1       - display soft error message.
;               POST_CTL_SOFTERR2       - display soft error message.
;               POST_CTL_SOFTERR3       - display "Press Any Key to Continue..."
;               POST_CTL_WINCE_SETUP    - prepare to call Windows CE.
;
;       POST_CTL_ALARM:
;               none.
;
;       POST_CTL_BANNER1:
;               none.
;
;       POST_CTL_BANNER2:
;               none.
;
;       POST_CTL_BANNER3:
;               none.
;
;       POST_CTL_BANNER4:
;               none.
;
;       POST_CTL_BOOT_DESKTOP:
;               none.
;
;       POST_CTL_BOOT_EFI:
;               none.
;
;       POST_CTL_BOOT_NETWORK:
;               none.
;
;       POST_CTL_CFGBOX:
;               none.
;
;       POST_CTL_HIMEM1:
;               none.
;
;       POST_CTL_HIMEM2:
;               none.
;
;       POST_CTL_HIMEM3:
;               none.
;
;       POST_CTL_HIMEM4:
;               none.
;
;       POST_CTL_HITDEL:
;               none.
;
;       POST_CTL_INT18H:
;               none.
;
;       POST_CTL_LOWMEM1:
;               none.
;
;       POST_CTL_LOWMEM2:
;               none.
;
;       POST_CTL_LOWMEM3:
;               none.
;
;       POST_CTL_LOWMEM4:
;               none.
;
;       POST_CTL_MAINT:
;               none.
;
;       POST_CTL_PCIDUMP:
;               none.
;
;       POST_CTL_POSTCODE:
;               BX      - postcode being displayed (all 16 bits valid).
;
;       POST_CTL_POWEROFF:
;               none.
;
;       POST_CTL_REBOOT:
;               none.
;
;       POST_CTL_REG_ERASE:
;               none.
;
;       POST_CTL_REG_GETID:
;               ES:BX   - pointer to SYSID structure to be filled-in.
;
;       POST_CTL_REG_GETINFO:
;               ES:BX   - pointer to REGDATA structure to be filled-in.
;               CX      - maximum size of REGDATA structure in bytes.
;
;       POST_CTL_REG_PUTINFO:
;               ES:BX   - pointer to REGDATA structure containing data to write.
;               CX      - size of data to be saved in bytes.
;
;       POST_CTL_REG_SETID:
;               ES:BX   - pointer to SYSID structure containing system ID.
;
;       POST_CTL_SOFTERR1:
;               none.
;
;       POST_CTL_SOFTERR2:
;               none.
;
;       POST_CTL_SOFTERR3:
;               none.
;
;       POST_CTL_WINCE_SETUP:
;               ES:DI   - pointer to WCEARGS structure.
;               EDX     - Windows CE entry point.
;
;   EXIT.
;       POST_CTL_ALARM:
;               none.
;
;       POST_CTL_BANNER1:
;               none.
;
;       POST_CTL_BANNER2:
;               none.
;
;       POST_CTL_BANNER3:
;               none.
;
;       POST_CTL_BANNER4:
;               none.
;
;       POST_CTL_BOOT_DESKTOP:
;               none.
;
;       POST_CTL_BOOT_EFI:
;               none.
;
;       POST_CTL_BOOT_NETWORK:
;               none.
;
;       POST_CTL_CFGBOX:
;               none.
;
;       POST_CTL_HIMEM1:
;               none.
;
;       POST_CTL_HIMEM2:
;               none.
;
;       POST_CTL_HIMEM3:
;               none.
;
;       POST_CTL_HIMEM4:
;               none.
;
;       POST_CTL_HITDEL:
;               none.
;
;       POST_CTL_INT18H:
;               none.
;
;       POST_CTL_LOWMEM1:
;               none.
;
;       POST_CTL_LOWMEM2:
;               none.
;
;       POST_CTL_LOWMEM3:
;               none.
;
;       POST_CTL_LOWMEM4:
;               none.
;
;       POST_CTL_MAINT:
;               none.
;
;       POST_CTL_PCIDUMP:
;               none.
;
;       POST_CTL_POSTCODE:
;               none.
;
;       POST_CTL_POWEROFF:
;               none.
;
;       POST_CTL_REBOOT:
;               none.
;
;       POST_CTL_REG_ERASE:
;               CY      - set if failure, else clear if success.
;
;       POST_CTL_REG_GETID:
;               CY      - set if failure, else clear if success.
;
;       POST_CTL_REG_GETINFO:
;               CY      - set if failure, else clear if success.
;               CX      - if success, actual amount of data read.
;
;       POST_CTL_REG_PUTINFO:
;               CY      - set if failure, else clear if success.
;               CX      - if success, actual amount of data written.
;
;       POST_CTL_REG_SETID:
;               CY      - set if failure, else clear if success.
;
;       POST_CTL_SOFTERR1:
;               none.
;
;       POST_CTL_SOFTERR2:
;               none.
;
;       POST_CTL_SOFTERR3:
;               none.
;
;       POST_CTL_WINCE_SETUP:
;               ES:DI   - pointer to WCEARGS structure.
;               EDX     - Windows CE entry point.
;               CY      - clear if video mode has been set.
;
;   USES.
;       AX, Flags.

DefProc BoardPostControl, GLOBAL, OVERRIDE
        cmp     ax, POST_CTL_ALARM              ; alarm boot action?
        je      BoardPostControl_Alarm          ; if so.
        cmp     ax, POST_CTL_MAINT              ; maintenance boot action?
        je      BoardPostControl_Maint          ; if so.
        cmp     ax, POST_CTL_POWEROFF           ; power off boot action?
        je      BoardPostControl_PowerOff       ; if so.
        cmp     ax, POST_CTL_REBOOT             ; reboot boot action?
        je      BoardPostControl_Reboot         ; if so.
        cmp     ax, POST_CTL_POSTCODE           ; postcode to display?
        je      BoardPostControl_Postcode       ; if so.
        cmp     ax, POST_CTL_REG_GETINFO        ; get registration data?
        je      BoardPostControl_GetReg         ; if so.
        cmp     ax, POST_CTL_REG_PUTINFO        ; put registration data?
        je      BoardPostControl_PutReg         ; if so.
        cmp     ax, POST_CTL_REG_ERASE          ; erase registration data?
        je      BoardPostControl_EraseReg       ; if so.
        cmp     ax, POST_CTL_REG_SETID          ; set system ID?
        je      BoardPostControl_SetId          ; if so.
        cmp     ax, POST_CTL_REG_GETID          ; get system ID?
        je      BoardPostControl_GetId          ; if so.
        cmp     ax, POST_CTL_SETUP_READ_LOOP    ; setup waits for a keypress?
        je      BoardPostControl_SetupLoop      ; if so.
        cmp     ax, POST_CTL_BOOT_DESKTOP       ; boot DESKTOP action?
        je      BoardPostControl_Desktop        ; if so.
        cmp     ax, POST_CTL_BOOT_EFI           ; boot EFI action?
        je      BoardPostControl_Efi            ; if so.
        cmp     ax, POST_CTL_BOOT_NETWORK       ; boot NETWORK action?
        je      BoardPostControl_Network        ; if so.
        cmp     ax, POST_CTL_BANNER1
        je      BoardPostControl_Banner1

;       We don't recognize this function.  Handle it at the chipset level.

        Pcall   CsPostControl                   ; handle this at the chipset level.
        ret

BoardPostControl_Banner1:
        IF      OPTION_SUPPORT_SPLASHSCR
        PUSHALL

        USEEXTDATA ds

        IF      OPTION_SUPPORT_DEBUGGER
        or      DebugFlags, DebugFlags_InUse ; force splash screen to support INT 10h.
        ENDIF   ; (OPTION_SUPPORT_DEBUGGER)

        push    cs
        pop     es
        ASSUME  ES:NOTHING

;       If the Splash screen is ON, display the BIOS OEM version strings.

        test    SplashInt10Flags, SplashFlag_Active
        jz      @f                      ; if no splash screen going.
        SETPOS  1, ((80-OemVerSplash_Len)/2) ; set cursor position for graphics mode.
        mov     di, OFFSET POST_GRP:OemVerSplash
        mov     bx, 000fh
        Pcall   PrintString
        SETPOS  2, ((80-OemVerSplash1_Len)/2) ; set cursor position for graphics mode.
        mov     di, OFFSET POST_GRP:OemVerSplash1
        mov     bx, 000fh
        Pcall   PrintString
@@:
        IF      OPTION_SUPPORT_DEBUGGER
        and     DebugFlags, (NOT DebugFlags_InUse) ; restore flags.
        ENDIF   ; (OPTION_SUPPORT_DEBUGGER)

        POPALL
        ENDIF   ; (OPTION_SUPPORT_SPLASHSCR)
        stc                             ; display the default banner.
        ret

;       Call a setup screen update function while waiting for a keystroke.

BoardPostControl_SetupLoop:
        IF      ((OPTION_SUPPORT_SETUP NE 0) AND (OPTION_SETUP_CUSTOM NE 0))
        Gcall   BoardSetupCustomCtl, SETUP
        ENDIF   ; ((OPTION_SUPPORT_SETUP NE 0) AND (OPTION_SETUP_CUSTOM NE 0))
        ret

;       Handle platform registration get/put/erase functions below.

;       Read registration data from nonvolatile storage.  The supplied buffer is
;       pointed to by (ES:BX), and we have the maximum size of the buffer in (CX).

BoardPostControl_GetReg:
        stc                                     ; assume failure to read.

        IF      OPTION_SUPPORT_REGISTRATION

        cmp     cx, CONFIG_REGSCREEN_INFO_MAX_LEN
        jb      @f                              ; if buffer wouldn't overflow.
        mov     cx, CONFIG_REGSCREEN_INFO_MAX_LEN ; truncate.
@@:

;       If the CMOS index is specified, read the data from CMOS.

        IF      (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)
        push    ax
        push    bx
        push    cx
        push    dx
        mov     dl, CONFIG_REGSCREEN_INFO_CMOS_INDEX ; (DL) = starting cell #.
@@:     mov     al, dl                          ; (AL) = cell number to read.
        Gcall   ReadCmos, BIOS                  ; read cell (AL) into (AL).
        mov     byte ptr es:[bx], al            ; write byte into user's buffer.
        inc     dl                              ; (DL) = next cell to read.
        inc     bx                              ; (ES:BX) = ptr, next buffer byte.
        loop    @b                              ; do them all.
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        clc                                     ; we did the work.
        ENDIF   ; (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)

;       If the Media Address (MA) is specified, read the data using MCL.

        IF      (CONFIG_REGSCREEN_INFO_MA NE 0)
        push    ax
        push    dx
        mov     dx, (CONFIG_REGSCREEN_INFO_MA SHR 16)
        mov     ax, (CONFIG_REGSCREEN_INFO_MA AND 0000ffffh)
        Gcall   MediaReadBlock, BIOS            ; read from Flash.
        pop     dx
        pop     ax
        ENDIF   ; (CONFIG_REGSCREEN_INFO_MA NE 0)

        ENDIF   ; (OPTION_SUPPORT_REGISTRATION)
        ret

;       Write registration data into nonvolatile storage.  The supplied buffer is
;       pointed to by (ES:BX), and we have the maximum size of the buffer in (CX).

BoardPostControl_PutReg:
        stc                                     ; assume failure to write.

        IF      OPTION_SUPPORT_REGISTRATION

        cmp     cx, CONFIG_REGSCREEN_INFO_MAX_LEN
        jb      @f                              ; if buffer wouldn't overflow.
        mov     cx, CONFIG_REGSCREEN_INFO_MAX_LEN ; truncate.
@@:

;       If the CMOS index is specified, write the data to CMOS.

        IF      (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)
        push    ax
        push    bx
        push    cx
        mov     al, CONFIG_REGSCREEN_INFO_CMOS_INDEX ; (AL) = starting cell #.
@@:     mov     ah, byte ptr es:[bx]            ; (AH) = next byte to write.
        Gcall   WriteCmos, BIOS                 ; write cell (AL) with contents (AH).
        inc     al                              ; (AL) = next cell to write.
        inc     bx                              ; (ES:BX) = ptr, next byte.
        loop    @b                              ; do them all.
        pop     cx
        pop     bx
        pop     ax
        clc                                     ; we did the work.
        ENDIF   ; (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)

;       If the Media Address (MA) is specified, write the data using MCL.

        IF      (CONFIG_REGSCREEN_INFO_MA NE 0)
        push    ax
        push    dx
        mov     dx, (CONFIG_REGSCREEN_INFO_MA SHR 16)
        mov     ax, (CONFIG_REGSCREEN_INFO_MA AND 0000ffffh)
        Gcall   MediaWriteBlock, BIOS           ; write to Flash.
        pop     dx
        pop     ax
        ENDIF   ; (CONFIG_REGSCREEN_INFO_MA NE 0)

        ENDIF   ; (OPTION_SUPPORT_REGISTRATION)
        ret

;       Erase registration data in nonvolatile storage.

BoardPostControl_EraseReg:
        stc                                     ; assume failure to erase.

        IF      OPTION_SUPPORT_REGISTRATION

;       If the CMOS index is specified, erase the data in CMOS.

        IF      (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)
        push    ax
        push    cx
        mov     cx, CONFIG_REGSCREEN_INFO_MAX_LEN ; # bytes to clear.
        mov     al, CONFIG_REGSCREEN_INFO_CMOS_INDEX ; (AL) = starting cell #.
        sub     ah, ah
@@:     Gcall   WriteCmos, BIOS                 ; write cell (AL) with contents (AH).
        inc     al                              ; (AL) = next cell to write.
        loop    @b                              ; do them all.
        pop     cx
        pop     ax
        clc                                     ; we did the work.
        ENDIF   ; (CONFIG_REGSCREEN_INFO_CMOS_INDEX NE 0)

;       If the Media Address (MA) is specified, erase the data using MCL.

        IF      (CONFIG_REGSCREEN_INFO_MA NE 0)
        push    ax
        push    dx
        mov     dx, (CONFIG_REGSCREEN_INFO_MA SHR 16)
        mov     ax, (CONFIG_REGSCREEN_INFO_MA AND 0000ffffh)
        Gcall   MediaEraseBlock, BIOS
        pop     dx
        pop     ax
        ENDIF   ; (CONFIG_REGSCREEN_INFO_MA NE 0)
        ENDIF   ; (OPTION_SUPPORT_REGISTRATION)

        ret

;       Set system ID by writing it to nonvolatile storage.  The system ID to
;       be saved is stored in an 8-byte buffer at (ES:BX).

BoardPostControl_SetId:
        stc                                     ; assume failure to write.

        IF      OPTION_SUPPORT_REGISTRATION

;       If the CMOS index is specified, write the data to CMOS.

        IF      (CONFIG_REGSCREEN_SYSID_CMOS_INDEX NE 0)
        push    ax
        push    bx
        push    cx
        mov     cx, (SIZE SYSID)                ; (CX) = # bytes in system ID.
        mov     al, CONFIG_REGSCREEN_SYSID_CMOS_INDEX ; (AL) = starting cell #.
@@:     mov     ah, byte ptr es:[bx]            ; (AH) = next byte to write.
        Gcall   WriteCmos, BIOS                 ; write cell (AL) with contents (AH).
        inc     al                              ; (AL) = next cell to write.
        inc     bx                              ; (ES:BX) = ptr, next byte.
        loop    @b                              ; do them all.
        pop     cx
        pop     bx
        pop     ax
        clc                                     ; we did the work.
        ENDIF   ; (CONFIG_REGSCREEN_SYSID_CMOS_INDEX NE 0)

;       If the Media Address (MA) is specified, write the data using MCL.
;       If you are not using the BIOS Flash part, but a separate part that
;       would need to be erased before writing, then clone BoardPostControl
;       and uncomment the erase block call below.

        IF      (CONFIG_REGSCREEN_SYSID_MA NE 0)
        push    ax
        push    cx
        push    dx
        mov     cx, (SIZE SYSID)                ; (CX) = # bytes in system ID.
        mov     dx, (CONFIG_REGSCREEN_SYSID_MA SHR 16)
        mov     ax, (CONFIG_REGSCREEN_SYSID_MA AND 0000ffffh)
;       Gcall   MediaEraseBlock, BIOS           ; erase Flash if needed.
        Gcall   MediaWriteBlock, BIOS           ; write to Flash.
        pop     dx
        pop     cx
        pop     ax
        ENDIF   ; (CONFIG_REGSCREEN_SYSID_MA NE 0)
        ENDIF   ; (OPTION_SUPPORT_REGISTRATION)
        ret

;       Get system ID by reading it from nonvolatile storage.  The system ID to
;       be loaded is copied to an 8-byte buffer at (ES:BX).

BoardPostControl_GetId:
        stc                                     ; assume failure to read.

        IF      OPTION_SUPPORT_REGISTRATION

;       If the CMOS index is specified, read the data from CMOS.

        IF      (CONFIG_REGSCREEN_SYSID_CMOS_INDEX NE 0)
        push    ax
        push    bx
        push    cx
        push    dx
        mov     cx, (SIZE SYSID)                ; (CX) = # bytes in system ID.
        mov     dl, CONFIG_REGSCREEN_SYSID_CMOS_INDEX ; (DL) = starting cell #.
@@:     mov     al, dl                          ; (AL) = cell number to read.
        Gcall   ReadCmos, BIOS                  ; read cell (AL) into (AL).
        mov     byte ptr es:[bx], al            ; write byte into user's buffer.
        inc     dl                              ; (DL) = next cell to read.
        inc     bx                              ; (ES:BX) = ptr, next buffer byte.
        loop    @b                              ; do them all.
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        clc                                     ; we did the work.
        ENDIF   ; (CONFIG_REGSCREEN_SYSID_CMOS_INDEX NE 0)

;       If the Media Address (MA) is specified, read the data using MCL.

        IF      (CONFIG_REGSCREEN_SYSID_MA NE 0)
        push    ax
        push    cx
        push    dx
        mov     cx, (SIZE SYSID)                ; (CX) = # bytes in system ID.
        mov     dx, (CONFIG_REGSCREEN_SYSID_MA SHR 16)
        mov     ax, (CONFIG_REGSCREEN_SYSID_MA AND 0000ffffh)
        Gcall   MediaReadBlock, BIOS            ; read from Flash.
        pop     dx
        pop     cx
        pop     ax
        ENDIF   ; (CONFIG_REGSCREEN_SYSID_MA NE 0)
        ENDIF   ; (OPTION_SUPPORT_REGISTRATION)
        ret

;       Handle a boot alarm. By default, invoke Firmbase & beep 10 times.

BoardPostControl_Alarm:
        stc                                     ; Assume failure.
        SMMEVENT SMM_INT_POST_BEGIN_ALARM
        IF      OPTION_SUPPORT_SOUND
        mov     cx, 10
@@:     push    cx
        Gcall   Tone, BIOS
        pop     cx
        loop    @b
        ENDIF   ; (OPTION_SUPPORT_SOUND)
        SMMEVENT SMM_INT_POST_END_ALARM
        clc                                     ; indicate this was handled.
        ret

;       Handle a maint mode boot action. By default, invoke Firmbase.  The OEM
;       may want to add code inside of these two calls.  Firmbase should be
;       told when maintenance mode is being entered and exited.

BoardPostControl_Maint:
        SMMEVENT SMM_INT_POST_BEGIN_MAINT
        SMMEVENT SMM_INT_POST_END_MAINT
        clc                                     ; indicate this was handled.
        ret

;       Handle a power off boot action. By default, invoke Firmbase.

BoardPostControl_PowerOff:
        push    ax
        SMMEVENT SMM_INT_POST_POWEROFF_ACTION
        pop     ax                              ; (AX) = function code.
        Pcall   CsPostControl                   ; handle this at the chipset level.
        ret

;       Handle a postcode action. By default, do nothing.

BoardPostControl_Postcode:
        Pcall   CsPostControl                   ; handle this at the chipset level.
        ret

;       Handle a reboot boot action. By default, invoke Firmbase, then call Reboot.

BoardPostControl_Reboot:
        SMMEVENT SMM_INT_POST_REBOOT_ACTION
        Gcall   Reboot, BIOS                    ; will probably call BoardReboot.
        clc                                     ; indicate this was handled.

;       Handle a boot to DESKTOP action.

BoardPostControl_Desktop:
        IF      OPTION_SUPPORT_DESKTOP
        SMMEVENT SMM_INT_POST_BEGIN_DESKTOP
        Gcall   StartDesktop, SMM               ; launch the Firmbase desktop.
        SMMEVENT SMM_INT_POST_END_DESKTOP
        ENDIF   ; (OPTION_SUPPORT_DESKTOP)
        clc
        ret

;       Handle a boot EFI action. By default, call the core BIOS EFI loader.

BoardPostControl_Efi:
        IF      OPTION_SUPPORT_EFI
        push    ebx
        push    edx

        mov     ebx, CONFIG_EFI_LOAD_ADDRESS    ; (EBX) = start of EFI image in main memory.
        mov     edx, CONFIG_EFI_BOOT_ENTRY_POINT ; (EDX) = EFI entry point in main memory.

        Gcall   StartEfi, BOOTBIN               ; launch the EFI image.

        pop     edx
        pop     ebx
        ENDIF   ; (OPTION_SUPPORT_EFI)
        ret

;       Handle a NETWORK boot action. By default, invoke Firmbase.  The OEM
;       may want to add code inside of these two calls.  Firmbase should be
;       told when NETWORK mode is being entered and exited.

BoardPostControl_Network:
        IF      OPTION_SUPPORT_NETWORK
        SMMEVENT SMM_INT_POST_BEGIN_NETWORK
        Gcall   StartNetwork, SMM
        SMMEVENT SMM_INT_POST_END_NETWORK
        ENDIF   ; (OPTION_SUPPORT_NETWORK)
        clc                                     ; indicate this was handled.
        ret
EndProc BoardPostControl

;***   PrintString - Print String with Color Attributes.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called to print a string with the specified color
;       attributes. Int10h services are used to output a carachter and advance
;       the cursor position.
;
;   MODIFICATION HISTORY.
;       E. Khoruzhenko  04/03/19.       #5.3, new.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       BL      - color attribute.
;       BH      - display page.
;       ES:DI   - string address.
;
;   EXIT.
;       CY      - set if failure, else clear if success.
;
;   USES.
;       flags.

DefProc PrintString
        PUSHALL

PrintString_Loop:
        mov     al, byte ptr es:[di]    ; (AL) = character read from the string.
        or      al, al                  ; is it 0 terminator?
        jz      PrintString_Exit        ; if so, exit.
        mov     ah, 09                  ; (AH) = Int10 function number.
        mov     cx, 1                   ; output 1 char.
        int     10h

;       Advance the cursor position.

        push    bx
        mov     ah, 3                   ; (AH) = get cursor position function.
        sub     bh, bh
        int     10h                     ; (DX) = current cursor position.
        inc     dl
        mov     ah, 2                   ; (AH) = set cursor position function.
        int     10h                     ; reset to the new position.
        pop     bx

        inc     di
        jmp     PrintString_Loop

PrintString_Exit:
        POPALL
EndProc PrintString

;***   BoardHelpSetPwrLed - Set Power LED Helper Function.
;
;   FUNCTIONAL DESCRIPTION.
;       This routine is called from BoardSmmControl to set the Power LED mode
;       based on the power state the system is currently entering. The LED
;       stays on in the Full On power state, goes off in the Soft Off state
;       and starts blinking in the Standby and Suspend states.
;
;   MODIFICATION HISTORY.
;       E. Khoruzhenko  04/06/27.       #5.3, new.
;       R. Spiegel      05/04/27.       #5.3, cloned for Salsa.
;       K. C. Taylor    05/06/09.       #5.3, simplified and corrected.
;
;   WARNINGS.
;       none.
;
;   ENTRY.
;       EDX     - GPIO/MFGPT base address.
;       CL      - power state the system is entering.
;
;   EXIT.
;       none.
;
;   USES.
;       flags.

DefProc BoardHelpSetPwrLed
        push    eax
        push    ebx
        push    edx

;       The GPIO base is in the top half of (EDX).

        shr     edx, 16                 ; (DX) = GPIO base I/O address.

;       Disconnect the GPIO27 output from the MFGPT7 by default so the LED
;       is not blinking.

        add     dx, DIVIL_GPIOH_OUT_AUX1_SEL
        mov     eax, DIVIL_GPIO27_CLR   ; turn the LED on.
        out     dx, eax
        sub     dx, DIVIL_GPIOH_OUT_AUX1_SEL

;       Dispatch on the power state the system is entering.

        or      cl, cl                  ; is power state = 0 (Full On)?
        jz      BoardHelpSetPwrLed_Exit ; LED is already on, exit.

;       In the Soft Off state turn the Power LED off.

        cmp     cl, 5                   ; is power state = 5 (Soft Off)?
        jne     BoardHelpSetPwrLed_Blink
        add     dx, DIVIL_GPIOH_OUT_VAL
        mov     eax, DIVIL_GPIO27_SET   ; turn the LED off.
        out     dx, eax
        jmp     BoardHelpSetPwrLed_Exit

;       In other low power staes set the Power LED to BLINK.

BoardHelpSetPwrLed_Blink:
        add     dx, DIVIL_GPIOH_OUT_AUX1_SEL
        mov     eax, DIVIL_GPIO27_SET
        out     dx, eax

BoardHelpSetPwrLed_Exit:
        pop     edx
        pop     ebx
        pop     eax
EndProc BoardHelpSetPwrLed

        ENDSEG  ; (Close POST segment.)

;       Include the Board Module functions from the GX3 package. This line
;       must be the last line in the module, so that functions defined in
;       the package can be overridden in this module. If it is necessary to
;       override a function from the package, just implement this function
;       in this module before the include statement below.

        include ..\packages\gx3\boardpkg.asm
