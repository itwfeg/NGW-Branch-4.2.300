#ifndef MESSAGEFORMAT_H
#define MESSAGEFORMAT_H

	/* The following are used for both SessionMessage formats and HfcObject formats.
	 * By default, the formats defined are applicable to both SessionMessages and HfcObjects,
	 * unless otherwise noted with the specific format.
	 *
	 * See HfcObject.h for additional information on this subject.
	 */
	/* The following comment is supposed to be in HfcObject.h
	 *
	 * The intent behind using this for both SessionMessages and HfcObjects came about as a way
	 * to override the default Browser format for weigh statuses.  In run mode, I want to send the
	 * corresponding weight; in Service mode, I want to send the raw count values.  Instead of
	 * creating another class to send the data in a different format, since the format was specified
	 * in the SessionMessage and not in the HfcObject itself, I am allowing the user to specify a
	 * format for the HfcObject which will override the format the SessionMessage specifies.  So as not
	 * to need to change any other code, the default value for the HfcObject format field will be
	 * 'UnknownFormat', thus indicating the format is specified according to the SessionMessage.
	 */
	typedef enum MessageFormat
	{
		UnknownFormat,			// SessionMessage and HfcObject
		InternalFormat,			// SessionMessage and HfcObject
		BrowserFormat,			// SessionMessage and HfcObject
		UltimaTaggedFormat,
		EvolutionTaggedFormat,
		SynchronizationTaggedFormat,
		DBIndependentFormatFormat,					//	 5

		// Hobart tagged format (Scale Management format)
		SMFormat,				// SessionMessage and HfcObject

		WeightCountsFormat,			// HfcObject
		OperatorMaintenanceFormat,	// HfcObject
		NetworkDnsFormat,
		NetworkLeapFormat,							//	10
		NetworkTcpIpFormat,
		NetworkWirelessCardFormat,
		NetworkWirelessCardWEPEnableFormat,
		NetworkWirelessCardEncryptionKeyFormat,
		AllGenericTextFormat,						//	15
		NormalScaleOpFormat,
		ExpirationLabelMessageFormat,
		CountrySettingsFormat,
		ServWrapSettingsFormat,
		WrapperTestSensorsFormat,					//  20
		SoftKeyListNoGraphics,
		NetworkTftpSetupFormat,
		UpgradeScaleFormat,
		EditSlideFormat,

		// Report formats
		PrinterReportFormat,						//  25
		DisplayReportFormat,

		StoreScaleInfoReportFormat,
		LocaleReportFormat,
		NormalScaleOperationReportFormat,
		SelfServiceReportFormat,					//	30
		OperatorModifiesReportFormat,
		BackgroundReportFormat,
		FieldsToPrintReportFormat,
		FrequentShopperReportFormat,
		NetworkReportFormat,						//	35
		WirelessNetworkReportFormat,
		EmailReportFormat,
		WeigherReportFormat,
		WrapperReportFormat,
		VpnNetworkSetupFormat,						//	40
		SoftkeyGroupFormat,
		ExecuteSoftkeyFormat,
		SoftkeyGroupSelfServeFormat,
		CurrentRunFormat,
		BrowserFormatPLUOperateMode,				//	45
		LoadGroupFormat,
		ViewLabelFormat,
		FTPReportFormat,
		SyncListFormat,
		CommInterfaceReportFormat,                  //  50
		CreateFormat,
		ScaleManagementFormat,
		ReportsSetupFormat,
		RotationOverrideFormat,

		GcdFormat,
		ReadTransactionCompatibility,

		ExternalPrinterSaveFormat,

		CSVStringFormat,

		BarFormat,

		HandsFreeDeviceFormat

	}   MessageFormat_t;

#endif // MESSAGEFORMAT_H
