//! Devices.h
/*! Devices contains every single device in the user interface that may be able to receive a message. Every single message
 *  in the scale is represented by a device and event. The EvolDeviceType was copied straight from Event.h in the current
 *  scale application.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef DEVICES_H
#define DEVICES_H


#define DISPOSE_ERROR 255
#define DEVICE_ERROR_OFFSET 1000

//! EvolDeviceType.
/*! Device Generated Events, both Hardware and Software. All known deveices are listed. */
typedef enum EvolDeviceType
{
    UnknownDevice,			// 0
    Weigher,
    WeigherSystem,
    Event,
    OperationMode,
    LabelApplier,			// 5
    LabelApplierSystem,
    Printer,
    PrinterSystem,
    Wrapper,
    WrapperSystem,			// 10
    System,
    LSMNtoUSB,
    Renderer,
    TouchScreen,
    RFIDReader,				// 15
    BarcodeScanner,
    NextGenWrapper,
    RS485Mgr,
    KPCSystem,
    KPCBootloader,          // 20
    KPCBootUpdater,         // 21
    HandsFreeDevice,
    BoardSite,              // 23

    SessionDev = 100,		// 100
    ProductDev,
    OperatorDev,
    Error,
    TotalTypeDev,           // not used but should remain here
    GraphicDev,				// 105
    ExpandedTextDev,
    SpecialMessageDev,
    ProductNotesDev,
    BackupRestoreDev,
    ClassDev,				// 110
    LabelTypeDev,
    ConfigurationDev,
    FontDev,
    UltimaDiskDev,
    VirtualPeripheral,		// 115
    ReportManagerDev,
    NutrifactsDev,
    TimerDev,
    GenericTextDev,
    VersionManagerDev,		// 120
    CategoryDev,
    NetworkDev,
    TotalsDev,
    SpecialsDev,
    PresentationMethodDev,	// 125
    ProductStatisticsDev,
    CategoryStatisticsDev,
    InitialProductDev,
    StreamingGraphicDev,
    OperatorNotesDev,		// 130
    OperatorChangesDev,
    ProductDefaultsDev,
    EmailManagerDev,
    EmailAlertsDev,
    COOLDev,				// 135
    SoftkeyDev,
    MarqueeTextDev,
    PeripheralDev,
    Numeric,
    Marquee,				//140
    ESCPrinter,
    NumericSystem,
    MarqueeSystem,
    ImportDev,
    NetworkConnectivityDev, //145
    SynchronizationDev,
    SynchronizationChangeDev,
    ScaleDev,
    BatchDev,
    CGICallManagerDev,      //150
    TransactionDev,
    TransactionTypeDev,
    HourlyTotalDev,
    OperatorTotalDev,
    ClassTotalDev,          //155
    ProductTotalDev,
    COOLTotalDev,
    WindowsConfigurationDev,
    CutTestDev,
	CutTestTotalDev,		// 160
    GCDSessionDev,
    GCDDev,
    HIDDev,
    MfgTestDev,
    CookingInstructionsDev, // 165
    RecipeDev,
    ExpandedText2Dev,
    SuggestiveSellingDev,
    AllergensDev,
    RecallsDev,             // 170
    DeviceDev,
    ProductTextCfgDev,
    Diagnostics,
    SupervisorChangesDev,
    DeptDev,                // 175

    JobsDevice = 250  // This is temporary, but do not remove!!
} _EvolDeviceType_et;


#endif // EVENT_H
