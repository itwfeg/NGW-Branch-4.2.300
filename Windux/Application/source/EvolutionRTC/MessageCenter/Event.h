#ifndef EVENT_H
#define EVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project

	// Device Generated Events, both Hardware and Software
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
		BarcodeScannerDev,
		NGWrapper,
		RS485Mgr,
		KPCSystem,
		KPCBootloader,			// 20
		KPCBootUpdater,
		HandsFreeDevice,
		BoardSite,

		SessionDev = 100,		// 100
		ProductDev, //Plu
		OperatorDev,
		Error,
		TotalTypeDev,           // not used but should remain here
		GraphicDev,				// 105
		ExpandedTextDev,
		SpecialMessageDev,
		ProductNotesDev,
		BackupRestore,
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
		CutTestTotalDev,		//160
		GCDSessionDev,
		GCDDev,
		HIDDev,
		MfgTestDev,
		ProductText5Dev,		// 165 - CookingInstructions
		ProductText6Dev,		// Recipe
		ProductText7Dev,		// ExpandedText
		ProductText8Dev,        // SuggestiveSelling
		ProductText9Dev,	    // Allergens
		RecallDev,				// Recalls

		DeviceDev,
		ProductTextCfgDev,
		Diagnostics,
		SupervisorChangesDev,	    // for operator changes made in Supervisor mode

		DeptDev,					// for Multi-Dept project

		JoesDevice = 250            // This is temporary, but do not remove!!
	}   EvolDeviceType_t;

	typedef enum ErrorEvent
	{
		ErrUnknown,
		ErrWeigher,
		ErrEvent = Event,
		ErrOperationMode = OperationMode,    // NOTE: Other errors for OperationMode are defined in EvolutionRTC/OperatingModes/Include/OperationModeStatus.h
		ErrLabelApplier = LabelApplier,
		ErrPrinter = Printer,
		ErrWrapper = Wrapper,
		ErrSystem = System,
		ErrLSMNtoUSB = LSMNtoUSB,
		ErrRenderer = Renderer,
		ErrTouchScreen = TouchScreen,
		ErrBarcodeScannerDev = BarcodeScannerDev,

		ErrSessionDev = SessionDev,
		ErrProductDev = ProductDev,
		ErrOperatorDev = OperatorDev,
		ErrGraphicDev = GraphicDev,
		ErrExpandedTextDev = ExpandedTextDev,
		ErrSpecialMessageDev = SpecialMessageDev,
		ErrProductNotesDev = ProductNotesDev,
		ErrBackupRestore = BackupRestore,
		ErrClassDev = ClassDev,
		ErrLabelTypeDev = LabelTypeDev,
		ErrConfigurationDev = ConfigurationDev,
		ErrFontDev = FontDev,
		ErrUltimaDiskDev = UltimaDiskDev,
		ErrVirtualPeripheral = VirtualPeripheral,
		ErrReportManagerDev = ReportManagerDev,
		ErrNutrifactsDev = NutrifactsDev,
		ErrTimerDev = TimerDev,
		ErrGenericTextDev = GenericTextDev,
		ErrVersionManagerDev = VersionManagerDev,
		ErrCategoryDev = CategoryDev,
		ErrNetworkDev = NetworkDev,
		ErrTotalsDev = TotalsDev,
		ErrSpecialsDev = SpecialsDev,
		ErrEmailManagerDev = EmailManagerDev,
		ErrCOOLDev = COOLDev,
		ErrSoftkeyDev = SoftkeyDev,
		ErrMarqueeTextDev = MarqueeTextDev,
		ErrMarquee = Marquee,
		ErrNumeric = Numeric,
		ErrPeripheralDev = PeripheralDev,
		ErrImportDev = ImportDev,
		ErrNetworkConnectivityDev = NetworkConnectivityDev,
		ErrSynchronizationDev = SynchronizationDev,
		ErrCGICallManagerDev = CGICallManagerDev,
		ErrTransactionDev = TransactionDev,
		ErrTransactionTypeDev = TransactionTypeDev,
		ErrWindowsConfigurationDev = WindowsConfigurationDev,
		ErrCutTestDev = CutTestDev,
		ErrCutTestTotalDev = CutTestTotalDev,
		ErrGCDSessionDev = GCDSessionDev,

		ErrProductText5Dev = ProductText5Dev,
		ErrProductText6Dev = ProductText6Dev,
		ErrProductText7Dev = ProductText7Dev,
		ErrProductText8Dev = ProductText8Dev,
		ErrProductText9Dev = ProductText9Dev,
		ErrRecallDev = RecallDev,

		ErrDeviceDev = DeviceDev,
		ErrProductTextCfgDev = ProductTextCfgDev,

		ErrDeptDev = DeptDev,		// for Multi-Dept project

		ErrEOF = 254,
		ErrRemoveAnyModalDialog = 255
	}   ErrorEvent_t;

	

/* This offset is used to define how many errors, for each device, can be defined.
 * The first error is defined as the Device's enum multiplied by the following number.
 * For example, if ProductDev is 10, then the first ProductDevError is 10000.
 */
#define DISPOSE_ERROR 255

#define DEVICE_ERROR_OFFSET 1000

#define InvalidEvent 255

#endif // EVENT_H
