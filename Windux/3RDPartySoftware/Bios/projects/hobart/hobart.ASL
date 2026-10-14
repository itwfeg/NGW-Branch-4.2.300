//
// FILE NAME.
//      SALSA.ASL - Vendor-Specific ASL code for the AMD LXDB800 "Salsa" platform.
//
// FUNCTIONAL DESCRIPTION.
//      This file contains the ACPI Source Language (ASL) code supporting the
//      ACPI Differentiated System Description Table (DSDT) table for the
//      AMD Salsa motherboard.
//
//      The DSDT table specified by the ASL code in this file supports the
//      Salsa uniprocessor board design with the 8259 PIC interrupt handling
//      model.  The DSDT table currently supports the S0, S1, and S5 sleep
//      states.
//
//      A single DSDT table is provided as defined in this file as no other
//      Definition Block tables are required to support the Salsa design.
//
//      This ASL file resides in the SALSA project folder and is compiled by
//      invoking the 'GSMAKE ACPI' command from the EB2K533 projects directory.
//      This command invokes the standard Microsoft ASL Assembler to assemble
//      the ASL code in this file into a corresponding ACPI Machine Language
//      (AML) binary named SALSA.AML.  The GSMERGE utility can subsequently
//      incorporate the AML binary into the BIOS ROM image during the BIOS
//      build process.
//
//      ACPI BIOS Background Information:
//      The ACPI Specification defines interfaces between ACPI compliant
//      hardware, BIOS software, and Operating System software.  The role of
//      the ACPI BIOS is to provide a series of ACPI data structures or tables
//      as indicated by the ACPI Specification.  These tables provide
//      information about a particular platform's hardware.
//
//      These BIOS supplied ACPI tables fall into one of two categories:  fixed
//      tables and Definition Blocks.
//
//      The following mandatory fixed tables are defined in the board module:
//      - RSDP:  Root System Description Pointer.
//               Provides a pointer to the RSDT.
//      - RSDT:  Root System Description Table.
//               Provides pointers to other System Description Tables.
//      - FACP:  Fixed ACPI Description Table (also known as FADT).
//               Provides fixed information regarding ACPI hardware and also a
//               pointer to the DSDT table described below.
//
//      A Definition Block contains information about system hardware
//      implementation and configuration details in the form of data and
//      methods, which are compiled into ACPI Machine Language (AML) code
//      and incorporated into a BIOS ROM image during the General Software
//      build process.
//
//      The BIOS MUST supply one or more Definition Block's in the ACPI tables.
//      The only mandatory Definition Block is the DSDT table which describes
//      the base system.  All other Definition Block's are optional.
//
//      The ACPI namespace is a hierarchical tree structure residing in OS
//      controlled system memory which contains named objects.  The ACPI OS
//      dynamically creates the namespace at run time by loading definition
//      blocks from the ACPI tables provided by the ACPI compliant system BIOS.
//
//      The ACPI aware Operating System dynamically creates the ACPI namespace
//      at runtime by loading the DSDT table which contains the Differentiated
//      Definition Block.  The OS always inserts the DSDT information into the
//      ACPI namespace at boot time and never removes it.  The OS may
//      subsequently insert and remove other Definition Block's (if provided by
//      the BIOS) into the ACPI namespace dynamically.
//
//      General Software BIOS provides one or more ACPI Definition Block
//      tables as described below:
//      - DSDT:  Differentiated System Description Table.
//               This mandatory table describes the base system.
//      - SSDT:  Secondary System Description Table.
//               Optional table used to provide additional system information
//               beyond the base system described in the DSDT.  A common usage
//               of the SSDT is to describe docking station hardware.
//      - MADT:  Multiple APIC Description Table.
//               Optional table supplied for systems supporting multiple
//               processors or uniprocessor systems using I/O APIC architecture
//               for interrupt handling.
//      - SBST:  Smart Battery Subsystem Table.
//               Optional table describing Smart Battery system if present.
//
//      ACPI Source Language (ASL) code is used to create the DSDT and SSDT
//      tables.  The MADT table, if present, is provided as a series of static
//      data structures and is defined in the board module.
//
//      The DSDT consists of ASL code serving the following purposes as
//      indicated in the ACPI Specification Section 3 Overview:
//      - System Power Management:
//        > Specify the chipset dependent values necessary for the O/S ACPI
//          driver to place the system in various sleep states using _Sx sleep
//          state package objects.
//      - Device Power Management:
//        > Describe all motherboard devices, their power states, the power
//          planes they are connected to, and control methods for placing the
//          devices in various power states.
//      - Plug and Play:
//        > Provide hierarchically organized description of motherboard devices
//          allowing the OS to enumerate and configure devices which are not
//          accessible to the OS via industry-standard bus configuration
//          mechanisms.
//      - General Purpose Event Handling:
//        > Provide general purpose event handling methods allowing the OS to
//          handle various chipset and board related events such as thermal,
//          power management, and other events.
//
//      A key point in regards to resource configuration is specified in the
//      ACPI Specification Section 3.6 Plug and Play.  This section indicates
//      the BIOS DSDT is used only to enumerate and configure motherboard
//      devices that do not have other hardware standards for enumeration and
//      configuration.
//
//      We DO NOT need to report devices which are visible to the Operating
//      System drivers for buses such as PCI and USB which have industry
//      standard enumeration mechanisms.
//
//      We DO need to report information about system chipset devices, board
//      routing information, and embedded motherboard devices which is not
//      visible to the OS via industry standard mechanisms.  This typically
//      includes the following:
//      - Amount of physical memory (chipset specific).
//      - PCI  interrupt routing information (board specific).
//      - APIC interrupt routing information (chipset specific).
//      - Report resources consumed by motherboard devices such as North
//        Bridge, South Bridge, Super I/O, and any other embedded motherboard
//        devices.
//      - Chipset and board specific event handling information such as GPIO
//        support for various system events.
//      - Describe Super I/O logical devices such as FDC, Parallel Port,
//        Serial Port(s), Real Time Clock, Keyboard and Mouse Controllers, etc.
//
// MODIFICATION HISTORY.
//      R. Spiegel      05/05/16.       #5.3.3, new.
//      R. Spiegel      05/06/08.       #5.3.3, fixed IRQ routing table.
//      R. Spiegel      05/06/09.       #5.3.3, added VGA IRQ routing.
//      R. Spiegel      05/06/10.       #5.3.3, removed unused PCI slots from IRQ table.
//
// NOTICE:  Copyright (C) 2005 General Software, Inc.
//

//
// Differentiated System Description Table Template.
//
// This is the header for all ASL code -- the "main()" routine.
// Of the parameters listed below, only the last 3 (OEMID, TABLE ID,
// and OEM Revision) should be modified.  The output file is renamed
// by GSMERGE before it is added to the binary image.
//

DefinitionBlock (
    "hobart\\hobart.aml",                 // Output filename - std build process.
    "DSDT",                             // Signature.
    0x1,                                // DSDT Revision.
    "GEN_SW",                           // OEMID.
    "DSDT0001",                         // TABLE ID.
    0x1                                 // OEM Revision.
    )
{

    //
    // Equates for board-dependent base addresses.
    //
    // All of these base addresses MUST match their corresponding equates in
    // the project file or board module definitions.  The project file equate
    // names are provided in the line comments below to assist with OEM
    // modification by providing added visibility for text search tools.
    //

    //
    // LPC Card Super I/O Module.
    //

//    Name (BSIO, 0x002E)                 // LPC_INDEX_PORT.
    Name (CPWS, 0)                      // current system power state.

    //
    // Standard include files.
    //

    Include ("..\\acpi\\misc\\meta.asi")
    Include ("..\\acpi\\lib\\resdesc.asi")  // PnP descriptor library.

    //
    // Processor Object(s).
    //
    // Provide an entry for one Geode GX3 processor core.
    // We do not currently implement processor power management.
    //

    Scope (\_PR) {
        Processor (                     // BP CPU.
            CPU0,                       // ProcessorName.
            0x00,                       // ProcessorId.
            0x00000000,                 // PBlockAddress = (BACP+00h).
            0x00                        // PBlockLength (in bytes).
        ) {} // Processor object
    } // PR scope

    //
    // System sleep state packages.
    //
    // The PM1a_CNT.SLP_TYP registers are supported in the Geode CS5535
    // silicon.  The values for different sleep states are fixed as described
    // in the Geode CS5535 datasheet.
    //
    // The PM1b_CNT.SLP_TYP registers are not currently used.
    //

    Name (\_S0, Package (0x04) {
        0x00,                           // PM1a_CNT.SLP_TYP value for state.
        0x00,                           // PM1b_CNT.SLP_TYP value for state.
        0x00,                           // reserved.
        0x00                            // reserved.
    })

    Name (\_S1, Package (0x04) {
        0x01,                           // PM1a_CNT.SLP_TYP value for state.
        0x00,                           // PM1b_CNT.SLP_TYP h/w not used.
        0x00,
        0x00
    })

    Name (\_S3, Package (0x04) {
        0x03,                           // PM1a_CNT.SLP_TYP value for state.
        0x00,                           // PM1b_CNT.SLP_TYP h/w not used.
        0x00,
        0x00
    })

    Name (\_S4, Package (0x04) {
        0x04,                           // PM1a_CNT.SLP_TYP value for state.
        0x00,                           // PM1b_CNT.SLP_TYP h/w not used.
        0x00,
        0x00
    })

    Name (\_S5, Package (0x04) {
        0x05,                           // PM1a_CNT.SLP_TYP value for state.
        0x00,                           // PM1b_CNT.SLP_TYP h/w not used.
        0x00,
        0x00
    })

    //
    // General Purpose Event Model OEM Control Methods.
    //
    // The CS5535 does not implement any pre-defined general purpose wake
    // events.
    //
    // Refer to the ACPI Specification for more detailed information on the
    // GP event model, when to add or remove these methods, naming
    // conventions, et cetera.
    //

    Scope (\_GPE) {
        Method (_L1F, 0, NotSerialized) {

            //
            // Send a message to Firmbase GX3 driver to help us clear the
            // sleep button.
            //

            Store (0x00030001, \_SB_.PCI0.FCMD) // clear Sleep Button status.

            if (LGreater (CPWS, Zero)) {
                Notify (\_SB.SLPB, 0x02) // system woke up, notify the OS.
            } else {
                Notify (\_SB.SLPB, 0x80) // send the suspend request to the OS.
            }
        }
    } // _GPE scope

    //
    // Prepare To Sleep.
    // Executed by OS prior to entering sleep state.
    //
    // This method carries out required activities when the OS indicates
    // that we are about to sleep.  On most systems, nothing need be done,
    // but the method is left here in case there is something special
    // on your system.
    //
    // Arg0 = Sleep state requested (e.g. 1=S1, 4=S4, 5=S5).
    //

    Method (_PTS, 1, NotSerialized) {
        Store (Arg0, PT80)              // store code to debug port 80h.
        Store (Arg0, CPWS)              // save power state system goes into.
    } // _PTS method

    //
    // System Wake.
    // OS calls this method immediately after waking from sleep state.
    //
    // Like _PTS, this method carries out activities that are implementation
    // specific, and most platforms have little need for code here.  On
    // the other hand, anything that needs to be checked periodically
    // would not have been during sleep, so will need to be upon awakening.
    // Therefore, thermal management and power control may very well have
    // need of checking here.  As above, arg0 = sleep state # returning from.
    //
    // Arg0 = Integer value of sleep state just woken from.
    //

    Method (_WAK, 1, NotSerialized) {
        Notify (\_SB.PWRB, 0x02)        // notify power button of wake event.
        Or (Arg0, 0x30, PT80)           // store code to debug port 80h.
        Store (0, CPWS)                 // save power state S0.
        Return (Zero)                   // indicate success.
    } // _WAK method

    //
    // System Bus
    // Contains PCI Bus 0 and all downstream devices.
    //
    // This is the root bus for your system, and will typically have
    // PCI under it, and ISA under that.  Additionally, devices that have
    // special value added hardware may have definition here, but most
    // will follow the plug & play standards, so won't have to be defined.
    //
    // This should be the last scope in the ASL file.
    //

    Scope (\_SB_) {

        //
        // DEVICE NAME.
        //      PWRB - Generic Hardware Programming Model Power Button.
        //
        // FUNCTIONAL DESCRIPTION.
        //      This device definition provides a control method power
        //      button object for use with the generic hardware programming
        //      model.
        //
        //      For proper operation this control method must be coordinated
        //      with the FADT PWR_BUTTON bit in the Fixed Feature Flags field.
        //      Refer to the FADT Fixed Feature Flags section in the ACPI
        //      Specification for further information.
        //
        //      Refer to the following ACPI Specification Sections:
        //      - Section 4.7.2.2.1   Power Button
        //      - Section 4.7.2.2.1.1 Fixed Power Button.
        //      - Section 4.7.2.2.1.2 Control Method Power Button.
        //      - Section 5.2.8       Fixed ACPI Description Table (FADT).
        //
        // MANDATORY METHODS.
        //      _HID - Hardware ID.
        //      _STA - return power button status.
        //
        // OPTIONAL METHODS.
        //      None.
        //
        // MODIFICATION HISTORY.
        //      D. K. Chalfant  03/10/31.       #5.3, new.
        //

        Device (PWRB) {
            Name (_HID, EISAID ("PNP0C0C")) // Hardware ID.

            //
            // Status.
            //

            Method (_STA, 0) {
                Return (0x0B)           // show the device works properly.
            }
        } // PWRB device

        //
        // Sleep button device.
        //

        Device (SLPB) {
           Name (_HID, EisaId ("PNP0C0E"))
            Name (_PRW, Package (0x02) {0x1F, 0x03}) // GPE 31, sleep state up to S3.
           Method (_STA, 0, NotSerialized) {
              Return (0x0F)             // show the device works properly.
          }
        }

        //
        // Geode GX3 Processor.
        //
        // The ACPI Source Include file defines the PCI0 host bridge and GX3
        // associated objects.
        //

        Include ("..\\acpi\\imc\\geode\\gx3\\gx3.asi")

        //
        // Include additional code under PCI0 device as necessary.
        //

        Scope (\_SB_.PCI0) {

            Name (_PRW, Package (0x02) {0x1F, One})

            //
            // Current Resource Settings.
            //

            Method (_CRS, 0, NotSerialized) {

                //
                // Host Bridge _CRS method resource buffer.
                // This is the buffer _CRS method returns to the Operating
                // System ACPI driver.
                //

                Name (HBRS, ResourceTemplate () {

                    //
                    // Host Bridge PCI Buses.
                    // This device consumes PCI buses 0-255.
                    // WORD Address Descriptor with ResourceType = BusNumber.
                    //

                    WordBusNumber (     // Offset 0x00.
                        ResourceConsumer,
                        MinNotFixed,
                        MaxNotFixed,
                        PosDecode,
                        0x0000,         // Byte[07:06] _GRA.
                        0x0000,         // Byte[09:08] _MIN.
                        0x00FF,         // Byte[0B:0A] _MAX.
                        0x0000,         // Byte[0D:0C] _TRA.
                        0x0100)         // Byte[0F:0E] _LEN.

                    //
                    // System IO Below PCI CSAM ports.
                    // WORD Address Descriptor with ResourceType = I/O.
                    //

                    WordIO (            // Offset 0x10.
                        ResourceProducer,
                        MinFixed,       // flags.
                        MaxFixed,
                        PosDecode,
                        EntireRange,
                        0x0000,         // Byte[17:16] _GRA.
                        0x0000,         // Byte[19:18] _MIN.
                        0x0CF7,         // Byte[1B:1A] _MAX.
                        0x0000,         // Byte[1D:1C] _TRA.
                        0x0CF8)         // Byte[1F:1E] _LEN.

                    //
                    // PCI CSAM IO registers 0x0CF8-0x0CFC.
                    // Resource Consumers must use PNPISA descriptors.
                    // IO Port Descriptor.
                    //

                    IO (                // Offset 0x20.
                        Decode16,       // Byte[21:21] _DEC.
                        0x0CF8,         // Byte[23:22] _MIN.
                        0x0CF8,         // Byte[25:24] _MAX.
                        0x01,           // Byte[26:26] _ALN.
                        0x08)           // Byte[27:27] _LEN.

                    //
                    // System IO above PCI CSAM to 1000h.
                    // WORD Address Descriptor with ResourceType = I/O.
                    //

                    WordIO (            // Offset 0x28.
                        ResourceProducer,
                        MinFixed,
                        MaxFixed,
                        PosDecode,
                        EntireRange,
                        0x0000,         // Byte[2F:2E] _GRA.
                        0x0D00,         // Byte[31:30] _MIN.
                        0xFFFF,         // Byte[33:32] _MAX.
                        0x0000,         // Byte[35:34] _TRA.
                        0xF300)         // Byte[37:36] _LEN.

                    //
                    // VGA Upper Memory Area decoding.
                    // Address A0000h - BFFFFh decoded by PCI.
                    // DWORD Address Descriptor with ResourceType = Memory.
                    //

                    DWordMemory (       // Offset 0x38.
                        ResourceProducer,
                        PosDecode,
                        MinFixed,
                        MaxFixed,
                        Cacheable,
                        ReadWrite,
                        0x00000000,     // Byte[41:3E] _GRA.
                        0x000A0000,     // Byte[45:42] _MIN.
                        0x000BFFFF,     // Byte[49:46] _MAX.
                        0x00000000,     // Byte[4D:4A] _TRA.
                        0x00020000)     // Byte[51:4E] _LEN.

                    //
                    // Shadow memory.
                    // DWORD Address Descriptor with ResourceType = Memory.
                    //

                    DWordMemory (       // Offset 0x52.
                        ResourceProducer,
                        PosDecode,
                        MinFixed,
                        MaxFixed,
                        Cacheable,
                        ReadWrite,
                        0x00000000,     // Byte[5B:58] _GRA.
                        0x000C8000,     // Byte[5F:5C] _MIN.
                        0x000DBFFF,     // Byte[63:60] _MAX.
                        0x00000000,     // Byte[67:64] _TRA.
                        0x00014000)     // Byte[6B:68] _LEN.

                    //
                    // Report memory range for PCI resources.
                    // DWORD Address Descriptor with ResourceType = Memory.
                    //
                    // We know that limit for the amount of RAM on this board
                    // is 1GB, so we can claim addresses above 1GB as PCI
                    // memory space.
                    //

                    DWordMemory (       // Offset 0x6C.
                        ResourceProducer,
                        PosDecode,
                        MinFixed,
                        MaxFixed,
                        Cacheable,
                        ReadWrite,
                        0x00000000,     // Byte[75:72] _GRA.
                        0x40000000,     // Byte[79:76] _MIN.
                        0xEFFFFFFF,     // Byte[7D:7A] _MAX.
                        0x00000000,     // Byte[81:7E] _TRA.
                        0xB0000000)     // Byte[85:82] _LEN.
                }) // PCI0._CRS.HBRS object

                Return (HBRS)
            } // PCI0._CRS method

            //
            // PCI Routing Table.
            //

            Method (_PRT, 0, NotSerialized) {

                //
                // 8259 PIC Mode PCI Routing Table Information.
                //

                Name (PICD, Package () { // 8259 mode routing table.

                    //
                    // GX3 Host Bridge and VGA Graphics.
                    // Bus 0, Device 01h.
                    //

                    Package (4) {0x0001ffff, 0, \_SB_.PCI0.ISA_.LN_E, 0},

                    //
                    // Network controller.
                    // Bus 0, Device 0dh (IDSEL = 23).
                    //

                    Package (4) {0x000dffff, 0, \_SB_.PCI0.ISA_.LN_D, 0},
                    Package (4) {0x000dffff, 1, \_SB_.PCI0.ISA_.LN_A, 0},


                    //
                    // This entry is reserved for future extentions.
                    // Bus 0, Device 0ch (IDSEL = 22).
                    //

                    Package (4) {0x000cffff, 0, \_SB_.PCI0.ISA_.LN_A, 0},
                    Package (4) {0x000cffff, 1, \_SB_.PCI0.ISA_.LN_B, 0},
                    Package (4) {0x000cffff, 2, \_SB_.PCI0.ISA_.LN_C, 0},
                    Package (4) {0x000cffff, 3, \_SB_.PCI0.ISA_.LN_D, 0},

                    //
                    // Geode CS5535: 2 USB controllers, IDE, Audio.
                    // Bus 0, Device 0fh.
                    //

                    Package (4) {0x000fffff, 0, \_SB_.PCI0.ISA_.LN_F, 0},
                    Package (4) {0x000fffff, 1, \_SB_.PCI0.ISA_.LN_F, 0},
                    Package (4) {0x000fffff, 2, \_SB_.PCI0.ISA_.LN_G, 0},
                    Package (4) {0x000fffff, 3, \_SB_.PCI0.ISA_.LN_G, 0},

                    //
                    // PCI Slot 0.
                    // Bus 0, Device 0eh (IDSEL = 24).
                    //

                    Package (4) {0x000effff, 0, \_SB_.PCI0.ISA_.LN_C, 0},
                    Package (4) {0x000effff, 1, \_SB_.PCI0.ISA_.LN_D, 0},
                    Package (4) {0x000effff, 2, \_SB_.PCI0.ISA_.LN_A, 0},
                    Package (4) {0x000effff, 3, \_SB_.PCI0.ISA_.LN_B, 0},

                    //
                    // This entry is reserved for future extentions.
                    // Bus 0, Device 0bh (IDSEL = 21).
                    //

                    Package (4) {0x000bffff, 0, \_SB_.PCI0.ISA_.LN_D, 0},
                    Package (4) {0x000bffff, 1, \_SB_.PCI0.ISA_.LN_A, 0},
                    Package (4) {0x000bffff, 2, \_SB_.PCI0.ISA_.LN_B, 0},
                    Package (4) {0x000bffff, 3, \_SB_.PCI0.ISA_.LN_C, 0}
                }) // _PRT.PICD object

                Return (PICD)           // 8259 PIC mode information.
            } // _PRT method

            //
            // Geode CS5535 ASL Module.
            //

            Include ("hobart\\cs5535.asi")

            //
            // Geode LPC Adapter Module.
            //

//            Scope (ISA_) {
//                Include ("..\\acpi\\sio\\winbond\\w83627hf.asi")
//            } // ISA scope

        } // SB.PCI0 scope
    } // SB device
} // DSDT definition block
