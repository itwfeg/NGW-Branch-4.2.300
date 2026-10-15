/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CONFIGURATIONTAGS_H
#define CONFIGURATIONTAGS_H

#include "Tags.h"

class ConfigurationTags : public Tags
{
	public:
		ConfigurationTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ConfigurationTags();

//		const std::string& getName(unsigned int index) const;
		static const std::string &getFirmwareVersionTag();
		static const std::string &getFirmwareRevisionTag();
		static const std::string &getGCDPlaylistFilenameTag();

		static const std::string emptyTag;

// CfgScale
		static const std::string storeId;
		static const std::string storeIdN;
		static const std::string storeName;
		static const std::string storeNameN;
		static const std::string storeGraphic;
		static const std::string storeGraphicN;

		// No longer used as of 9-19-17- claim back later - static const std::string deptName;
		// No longer used as of 9-19-17- claim back later - static const std::string deptNameN;

		static const std::string primaryCountryCode;
		static const std::string primaryCountryCodeN;
		static const std::string primaryLanguageCode;
		static const std::string primaryLanguageCodeN;
		static const std::string secondaryCountryCode;
		static const std::string secondaryCountryCodeN;
		static const std::string secondaryLanguageCode;
		static const std::string secondaryLanguageCodeN;
		static const std::string lengthUnits;
		static const std::string lengthUnitsN;
		static const std::string temperatureUnits;
		static const std::string temperatureUnitsN;
		static const std::string dateFormat;
		static const std::string dateFormatN;
		static const std::string timeFormat;
		static const std::string timeFormatN;

		static const std::string unitPriceSecurityLabel;
		static const std::string unitPriceSecurityLabelN;
		static const std::string totalPriceSecurityLabel;
		static const std::string totalPriceSecurityLabelN;
		static const std::string fractionalPricingEnabled;
		static const std::string fractionalPricingEnabledN;
		static const std::string poundsForPricingEnabled;
		static const std::string poundsForPricingEnabledN;
		static const std::string bycountCalcMethod;
		static const std::string bycountCalcMethodN;
		static const std::string clearPluAfterPrinting;
		static const std::string clearPluAfterPrintingN;

		static const std::string isSoundBeepStableWeight;
		static const std::string isSoundBeepStableWeightN;
		static const std::string beeperVolumeLevel;
		static const std::string beeperVolumeLevelN;

		static const std::string fixedWeightUnits;
		static const std::string fixedWeightUnitsN;
		static const std::string weigherAttached;
		static const std::string weigherAttachedN;
		static const std::string tareSetback;
		static const std::string tareSetbackN;
		static const std::string maxTare;
		static const std::string maxTareN;
		static const std::string platterTareEnabled;
		static const std::string platterTareEnabledN;
		static const std::string changeZeroTareOnlyOnce;
		static const std::string changeZeroTareOnlyOnceN;
		static const std::string changeForcedTareOnlyOnce;
		static const std::string changeForcedTareOnlyOnceN;
		static const std::string minWeightToPrint;
		static const std::string minWeightToPrintN;
		static const std::string scaleWeighMode;
		static const std::string scaleWeighModeN;
		static const std::string weigherModel;
		static const std::string weigherModelN;
		static const std::string scaleMetricN;
		static const std::string netWeightDualRangeLimitCheck;
		static const std::string netWeightDualRangeLimitCheckN;
		static const std::string externalWeightPrecision;
		static const std::string externalWeightPrecisionN;
		static const std::string tareEntryPrecision;
		static const std::string tareEntryPrecisionN;
		static const std::string userRezeroWeightLimit;
		static const std::string userRezeroWeightLimitN;
		static const std::string weigherWakeupSensorAttached;
		static const std::string weigherWakeupSensorAttachedN;
		static const std::string valueMaxEnabled;
		static const std::string valueMaxEnabledN;
		static const std::string valueMaxDiagCollectionDisabled;
		static const std::string valueMaxDiagCollectionDisabledN;


		static const std::string loginRequired;
		static const std::string loginRequiredN;
		static const std::string supervisorloginRequiredN;
		static const std::string inactivityAutoLogoutTime;
		static const std::string inactivityAutoLogoutTimeN;
		static const std::string autoLookupProduct;
		static const std::string autoLookupProductN;
		static const std::string autoLookupNumDigits;
		static const std::string autoLookupNumDigitsN;
		static const std::string autoLookupOverrideDigit;
		static const std::string autoLookupOverrideDigitN;
		static const std::string autoDisplayProductNotes;
		static const std::string autoDisplayProductNotesN;
		static const std::string autoDisplaySuggestiveSelling;
		static const std::string autoDisplaySuggestiveSellingN;
		static const std::string autoDisplayCookingNotes;
		static const std::string autoDisplayCookingNotesN;

		static const std::string horizontalLabelSkew1;
		static const std::string horizontalLabelSkew1N;
		static const std::string horizontalLabelSkew2;
		static const std::string horizontalLabelSkew2N;
		static const std::string horizontalLabelSkew3;
		static const std::string horizontalLabelSkew3N;

		static const std::string isCollectingTransactions;
		static const std::string isCollectingTransactionsN;
		static const std::string collectingTransactionsDays;
		static const std::string collectingTransactionsDaysN;
		static const std::string isCollectingProductTotals;		//	Support for legacy database backups.
		static const std::string isCollectingProductTotalsN;
		static const std::string isCollectingHourlyTotals;		//	Support for legacy database backups.
		static const std::string isCollectingHourlyTotalsN;
		static const std::string isCollectingOperatorTotals;	//	Support for legacy database backups.
		static const std::string isCollectingOperatorTotalsN;
		static const std::string trackingOperatorChanges;
		static const std::string trackingOperatorChangesN;
		static const std::string trackingSupervisorChanges;
		static const std::string trackingSupervisorChangesN;
//		static const std::string daysToKeepOperatorChanges;
//		static const std::string daysToKeepOperatorChangesN;
		static const std::string collectingCOOLTotals;		//	Support for legacy database backups.
		static const std::string collectingCOOLTotalsN;

		static const std::string opAccessToBrowserMode;
		static const std::string opAccessToBrowserModeN;
		static const std::string opAccessToDesktopMode;
		static const std::string opAccessToDesktopModeN;

		static const std::string autoLoadOldFirmwareVersions;
		static const std::string autoLoadOldFirmwareVersionsN;
		static const std::string autoSearchNewFirmwareVersions;
		static const std::string autoSearchNewFirmwareVersionsN;
		static const std::string newFirmwareVersionSearchTime;
		static const std::string newFirmwareVersionSearchTimeN;
		static const std::string forceUniqueProdClassNums;
		static const std::string forceUniqueProdClassNumsN;
		static const std::string enablePowerSaverMode;
		static const std::string enablePowerSaverModeN;
		static const std::string powerSaverModeTimeout;
		static const std::string powerSaverModeTimeoutN;
		static const std::string defaultBrowserHomePage;
		static const std::string defaultBrowserHomePageN;
		static const std::string defaultRunScreenHomePage;
		static const std::string defaultRunScreenHomePageN;
		static const std::string defaultProductRecordType;
		static const std::string defaultProductRecordTypeN;

		static const std::string defWrFilmWidth;
		static const std::string defWrFilmWidthN;
		static const std::string defWrFilmTension;
		static const std::string defWrFilmTensionN;
		static const std::string defWrFilmTensionFrontToBack;
		static const std::string defWrFilmTensionFrontToBackN;
		static const std::string defWrFilmTensionLeftToRight;
		static const std::string defWrFilmTensionLeftToRightN;
		static const std::string defWrIntakeMode;
		static const std::string defWrIntakeModeN;
		static const std::string defWrLabelPlacement;
		static const std::string defWrLabelPlacementN;
		static const std::string defWrApplierRotation;
		static const std::string defWrApplierRotationN;

		static const std::string defWrSecLabelPlacement;
		static const std::string defWrSecLabelPlacementN;
		static const std::string defWrSecApplierRotation;
		static const std::string defWrSecApplierRotationN;



		static const std::string wrSealerBeltTimeOnSun;
		static const std::string wrSealerBeltTimeOnSunN;
		static const std::string wrSealerBeltTimeOffSun;
		static const std::string wrSealerBeltTimeOffSunN;
		static const std::string wrSealerBeltTimeOnMon;
		static const std::string wrSealerBeltTimeOnMonN;
		static const std::string wrSealerBeltTimeOffMon;
		static const std::string wrSealerBeltTimeOffMonN;
		static const std::string wrSealerBeltTimeOnTue;
		static const std::string wrSealerBeltTimeOnTueN;
		static const std::string wrSealerBeltTimeOffTue;
		static const std::string wrSealerBeltTimeOffTueN;
		static const std::string wrSealerBeltTimeOnWed;
		static const std::string wrSealerBeltTimeOnWedN;
		static const std::string wrSealerBeltTimeOffWed;
		static const std::string wrSealerBeltTimeOffWedN;
		static const std::string wrSealerBeltTimeOnThu;
		static const std::string wrSealerBeltTimeOnThuN;
		static const std::string wrSealerBeltTimeOffThu;
		static const std::string wrSealerBeltTimeOffThuN;
		static const std::string wrSealerBeltTimeOnFri;
		static const std::string wrSealerBeltTimeOnFriN;
		static const std::string wrSealerBeltTimeOffFri;
		static const std::string wrSealerBeltTimeOffFriN;
		static const std::string wrSealerBeltTimeOnSat;
		static const std::string wrSealerBeltTimeOnSatN;
		static const std::string wrSealerBeltTimeOffSat;
		static const std::string wrSealerBeltTimeOffSatN;

		static const std::string diskWriteFrequency;
		static const std::string diskWriteFrequencyN;
		static const std::string autoCompressDB;
		static const std::string autoCompressDBN;
		static const std::string compressDBTime;
		static const std::string compressDBTimeN;

		static const std::string coolTrackingNumberEnabled;
		static const std::string coolTrackingNumberEnabledN;
		static const std::string dontChangeCurrentCOOL;
		static const std::string dontChangeCurrentCOOLN;
		// No fieldOffset in CfgBase.h for the next one
		//static const std::string forcedCOOLVerification;
		//static const std::string forcedCOOLVerificationN;

		static const std::string primaryExpirationPageNote;
		static const std::string primaryExpirationPageNoteN;
		static const std::string secondExpirationPageNote;
		static const std::string secondExpirationPageNoteN;
		
		static const std::string checkingNetworkConnectivity;
		static const std::string checkingNetworkConnectivityN;
		static const std::string offlineDateTime;
		static const std::string offlineDateTimeN;

		static const std::string scaleType;
		static const std::string scaleTypeN;

		static const std::string communicationHistory;
		static const std::string communicationHistoryN;
		static const std::string communicationItemReceived;
		static const std::string communicationItemReceivedN;

		static const std::string popupKeypadEnabled;
		static const std::string popupKeypadEnabledN;
		static const std::string popupKeypadStyle;
		static const std::string popupKeypadStyleN;
		static const std::string popupKeypadLocation;
		static const std::string popupKeypadLocationN;

		static const std::string operatorLoginKeyboardStyle;
		static const std::string operatorLoginKeyboardStyleN;

		static const std::string capacityBarLocation;
		static const std::string capacityBarLocationN;

		static const std::string pingPrimary;
		static const std::string pingPrimaryN;
		static const std::string pingGateway;
		static const std::string pingGatewayN;
		static const std::string pingTarget;
		static const std::string pingTargetN;
		static const std::string pingInterval;
		static const std::string pingIntervalN;
		static const std::string pingTargetIPAddress;					  // 121
		static const std::string pingTargetIPAddressN;					  // 121

		static const std::string rebootUnprotectedAllowed;
		static const std::string rebootUnprotectedAllowedN;

		static const std::string autoCreateRFIDOperator;
		static const std::string autoCreateRFIDOperatorN;

		static const std::string firmwareVersion;
		static const std::string firmwareVersionN;
		static const std::string firmwareRevision;
		static const std::string firmwareRevisionN;

		static const std::string ignoreUltimaFormatLabelTypes;
		static const std::string ignoreUltimaFormatLabelTypesN;

		static const std::string reportsOffsetTop;				  // 105
		static const std::string reportsOffsetTopN;				  // 105
		static const std::string reportsOffsetBottom;
		static const std::string reportsOffsetBottomN;

		static const std::string showDeleteAllFiles;
		static const std::string showDeleteAllFilesN;
		static const std::string showPrinterSettings;
		static const std::string showPrinterSettingsN;

		static const std::string remoteAccessPermitted;
		static const std::string remoteAccessPermittedN;
		static const std::string remoteAccessRequiresLogin;
		static const std::string remoteAccessRequiresLoginN;
		static const std::string remoteAccessLoginMethod;
		static const std::string remoteAccessLoginMethodN;
		static const std::string remoteAccessUsername;
		static const std::string remoteAccessUsernameN;
		static const std::string remoteAccessPassword;
		static const std::string remoteAccessPasswordN;

		static const std::string backdateByDays;
		static const std::string backdateByDaysN;

		static const std::string unitSeparator;
		static const std::string unitSeparatorN;

		static const std::string systemFont;
		static const std::string systemFontN;
		static const std::string startScreenHome;
		static const std::string startScreenHomeN;
		static const std::string browserHasAddressBar;
		static const std::string browserHasAddressBarN;
		static const std::string defaultSearchMethod;
		static const std::string defaultSearchMethodN;
		static const std::string resetStatisticsDays;
		static const std::string resetStatisticsDaysN;

		static const std::string leftRunscreenContent;
		static const std::string leftRunscreenContentN;
		static const std::string runscreenDynamicContent;
		static const std::string runscreenDynamicContentN;
		static const std::string runscreenKeypadContent;
		static const std::string runscreenKeypadContentN;
		static const std::string rightScreenContent;
		static const std::string rightScreenContentN;

		static const std::string remoteScaleMaintenanceEnabled;
		static const std::string remoteScaleMaintenanceEnabledN;
		static const std::string remoteScaleMaintClrOutboxDays;
		static const std::string remoteScaleMaintClrOutboxDaysN;

		static const std::string homeFlashkeyGroup;		//	This won't ever go into a report because it is not saved in configuration.

		static const std::string enableDisplayNetworkDetails;
		static const std::string enableDisplayNetworkDetailsN;

		static const std::string disableOperatorFlashkeyEditor;
		static const std::string disableOperatorFlashkeyEditorN;
		static const std::string disableLeftRunscreen;
		static const std::string disableLeftRunscreenN;

		static const std::string mostRecentRunMode;
		static const std::string mostRecentRunModeN;

		static const std::string displayTransTypeIndicator;
		static const std::string displayTransTypeIndicatorN;
		static const std::string nowServingQueueIPAddress;
		static const std::string nowServingQueueIPAddressN;
		static const std::string nowServingSysType;
		static const std::string nowServingSysTypeN;

		//GCD
		static const std::string customerDisplay;
		static const std::string customerDisplayN;

		static const std::string gcdLayoutNumber;
		static const std::string gcdLayoutNumberN;
		static const std::string gcdColorSchemeNumber;
		static const std::string gcdColorSchemeNumberN;
		static const std::string gcdAllowOperReset;
		static const std::string gcdAllowOperResetN;

		static const std::string gcdVolume;				
		static const std::string gcdVolumeN;			
		static const std::string gcdPlaybackWaitTime;	
		static const std::string gcdPlaybackWaitTimeN;	
		static const std::string gcdMediaServerName;	
		static const std::string gcdMediaServerNameN;	
		static const std::string gcdMediaServerIP;		
		static const std::string gcdMediaServerNamIPN;	
		static const std::string gcdFTPUserName;		
		static const std::string gcdFTPUserNameN;		
		static const std::string gcdFTPPassword;		
		static const std::string gcdFTPPasswordN;		
		static const std::string gcdFTPDirectory;		
		static const std::string gcdFTPDirectoryN;		
		static const std::string gcdPlaylistFilename;	
		static const std::string gcdPlaylistFilenameN;	
		static const std::string gcdAutoSearch;			
		static const std::string gcdAutoSearchN;		
		static const std::string gcdSearchHour;			
		static const std::string gcdSearchHourN;			
		static const std::string gcdSearchMinute;		
		static const std::string gcdSearchMinuteN;		
		static const std::string gcdAutoAMPM;			
		static const std::string gcdAutoAMPMN;			
		static const std::string gcdDnsEnabled;			
		static const std::string gcdDnsEnabledN;		
		static const std::string gcdFTPProtocol;			
		static const std::string gcdFTPProtocolN;		
		static const std::string gcdLanguage;
		static const std::string gcdLanguageN;
		static const std::string printWithoutCustomerDisplay;
		static const std::string printWithoutCustomerDisplayN;

		static const std::string gcdUseTransactionalLayout;
		static const std::string gcdUseTransactionalLayoutN;
		static const std::string gcdIdleAdvertType;
		static const std::string gcdIdleAdvertTypeN;
		static const std::string gcdIdleWebsiteAddress;
		static const std::string gcdIdleWebsiteAddressN;
		static const std::string gcdProductAdvertType;
		static const std::string gcdProductAdvertTypeN;
		// end GCD


//  CfgFrequentShopper
		static const std::string frequentShopperRoundingMethod;
		static const std::string frequentShopperRoundingMethodN;
		static const std::string discountMethod;
		static const std::string discountMethodN;
		static const std::string discountType;
		static const std::string discountTypeN;
		static const std::string discountFormula;
		static const std::string discountFormulaN;
		static const std::string frequentShopperPerScaleN;
		static const std::string totalPriceText1;
		static const std::string totalPriceText1N;
		static const std::string totalPriceText2;
		static const std::string totalPriceText2N;
		static const std::string savingsText1;
		static const std::string savingsText1N;
		static const std::string savingsText2;
		static const std::string savingsText2N;
		static const std::string unitPriceText1;
		static const std::string unitPriceText1N;
		static const std::string unitPriceText2;
		static const std::string unitPriceText2N;
		static const std::string fsRoundingWeightPrecision;
		static const std::string fsRoundingWeightPrecisionN;


//  CfgOperatorAccess
		static const std::string priceModifiable;
		static const std::string priceModifiableN;
		static const std::string byCountModifiable;
		static const std::string byCountModifiableN;
		static const std::string tareModifiable;
		static const std::string tareModifiableN;
		static const std::string fixedWeightModifiable;
		static const std::string fixedWeightModifiableN;
		static const std::string shelfLifeModifiable;
		static const std::string shelfLifeModifiableN;
		static const std::string productLifeModifiable;
		static const std::string productLifeModifiableN;
		static const std::string labelTypeModifiable;
		static const std::string labelTypeModifiableN;
		static const std::string dateTimeModifiable;
		static const std::string dateTimeModifiableN;
		static const std::string portionsModifiable;
		static const std::string portionsModifiableN;

		static const std::string priceAccessLevel;
		static const std::string priceAccessLevelN;
		static const std::string byCountAccessLevel;
		static const std::string byCountAccessLevelN;
		static const std::string tareAccessLevel;
		static const std::string tareAccessLevelN;
		static const std::string fixedWeightAccessLevel;
		static const std::string fixedWeightAccessLevelN;
		static const std::string shelfLifeAccessLevel;
		static const std::string shelfLifeAccessLevelN;
		static const std::string productLifeAccessLevel;
		static const std::string productLifeAccessLevelN;
		static const std::string labelTypeAccessLevel;
		static const std::string labelTypeAccessLevelN;
		static const std::string dateTimeAccessLevel;
		static const std::string dateTimeAccessLevelN;
		static const std::string portionsAccessLevel;
		static const std::string portionsAccessLevelN;


//  CfgPrintedFields
		static const std::string sellByDatePrinted;
		static const std::string sellByDatePrintedN;
		static const std::string sellByDateJulian;
		static const std::string sellByDateJulianN;
		static const std::string packedOnDatePrinted;
		static const std::string packedOnDatePrintedN;
		static const std::string packedOnDateJulian;
		static const std::string packedOnDateJulianN;
		static const std::string productLifeDatePrinted;
		static const std::string productLifeDatePrintedN;
		static const std::string productLifeDateJulian;
		static const std::string productLifeDateJulianN;
		static const std::string unitPricePrinted;
		static const std::string unitPricePrintedN;
		static const std::string byCountPrinted;
		static const std::string byCountPrintedN;
		static const std::string barCodePrinted;
		static const std::string barCodePrintedN;
		static const std::string currencySymbolPrinted;
		static const std::string currencySymbolPrintedN;
		static const std::string textNamesPreprinted;
		static const std::string textNamesPreprintedN;
		static const std::string weightOnByCountPrinted;
		static const std::string weightOnByCountPrintedN;
		static const std::string oneOnByCountPrinted;
		static const std::string oneOnByCountPrintedN;
		static const std::string usingSecondCheckDigit;
		static const std::string usingSecondCheckDigitN;
		static const std::string printFirstLetterOfTotalType;
		static const std::string printFirstLetterOfTotalTypeN;
		static const std::string alwaysPrintNetWtOnFixedWt;
		static const std::string alwaysPrintNetWtOnFixedWtN;
		static const std::string fixedWeightFormat;
		static const std::string fixedWeightFormatN;


// CfgSelfService
		static const std::string gridXElements;
		static const std::string gridXElementsN;
		static const std::string gridYElements;
		static const std::string gridYElementsN;
		static const std::string numOfMostCommonCategories;
		static const std::string numOfMostCommonCategoriesN;
		static const std::string imageTypeOnStartScreen;
		static const std::string imageTypeOnStartScreenN;
		static const std::string initialStreamingGraphicNum;
		static const std::string initialStreamingGraphicNumN;
		static const std::string productStatsHoursMethod;
		static const std::string productStatsHoursMethodN;
		static const std::string selfServeInactivityTimeout;
		static const std::string selfServeInactivityTimeoutN;

		static const std::string logoGraphicFileName;
		static const std::string logoGraphicFileNameN;
		static const std::string noGraphicAvailableFileName1;
		static const std::string noGraphicAvailableFileName1N;
		static const std::string noGraphicAvailableFileName2;
		static const std::string noGraphicAvailableFileName2N;
		static const std::string previousGraphicFileName1;
		static const std::string previousGraphicFileName1N;
		static const std::string previousGraphicFileName2;
		static const std::string previousGraphicFileName2N;
		static const std::string nextGraphicFileName1;
		static const std::string nextGraphicFileName1N;
		static const std::string nextGraphicFileName2;
		static const std::string nextGraphicFileName2N;


//  CfgNetwork
		static const std::string ethernetInterfaceName;
		static const std::string ethernetInterfaceNameN;
		static const std::string macAddressN;
		static const std::string iPAddress;
		static const std::string iPAddressN;
		static const std::string subnetMaskN;
		static const std::string networkSetupN;
		static const std::string iPPort;
		static const std::string iPPortN;

		static const std::string gatewayIPAddress1N;
		static const std::string networkMask1N;
		static const std::string gatewayIPAddress2N;
		static const std::string networkMask2N;
		static const std::string gatewayIPAddress3N;
		static const std::string networkMask3N;

		static const std::string dnsEnabledN;
		static const std::string dnsAutomaticN;
		static const std::string dnsHostNameN;
		static const std::string dnsDomainNameN;
		static const std::string dnsIPAddress0N;
		static const std::string dnsIPAddress1N;
		static const std::string dnsIPAddress2N;

		static const std::string tftpAutomaticN;
		static const std::string tftpIPAddressN;
		static const std::string tftpBootFilePathN;

		static const std::string hasWirelessCardN;
		static const std::string wirelessCardNameN;
		static const std::string wirelessCardConnectedN;
		static const std::string accessPointId;
		static const std::string accessPointIdN;
		static const std::string sharedKeyAuth;
		static const std::string sharedKeyAuthN;
		static const std::string wepEnabled;
		static const std::string wepEnabledN;
		static const std::string activeKeyNum;
		static const std::string activeKeyNumN;
		static const std::string encryptionKey1SetN;
		static const std::string encryptionKey2SetN;
		static const std::string encryptionKey3SetN;
		static const std::string encryptionKey4SetN;
		static const std::string encryptionKey1;
		static const std::string encryptionKey1N;
		static const std::string encryptionKey2;
		static const std::string encryptionKey2N;
		static const std::string encryptionKey3;
		static const std::string encryptionKey3N;
		static const std::string encryptionKey4;
		static const std::string encryptionKey4N;

		static const std::string vpnEnabledN;
		static const std::string vpnIPAddressN;
		static const std::string leapAvailableN;
		static const std::string leapEnabled;
		static const std::string leapEnabledN;
		static const std::string leapUserName;
		static const std::string leapUserNameN;
		static const std::string leapUserPassword;
		static const std::string leapUserPasswordN;

		static const std::string vpnCompression;
		static const std::string vpnCompressionN;
		static const std::string vpnDomain;
		static const std::string vpnDomainN;
		static const std::string vpnEncryptedPassword;
		static const std::string vpnEncryptedPasswordN;
		static const std::string vpnUsername;
		static const std::string vpnUsernameN;
		static const std::string vpnVerifyServerCertificate;
		static const std::string vpnVerifyServerCertificateN;
		static const std::string vpnNumberOfServers;
		static const std::string vpnNumberOfServersN;
		static const std::string vpnDeviceTypeDescriptor;
		static const std::string vpnDeviceTypeDescriptorN;
		static const std::string vpnUserClass;
		static const std::string vpnUserClassN;
		static const std::string vpnMajorVerbVersion;
		static const std::string vpnMajorVerbVersionN;
		static const std::string vpnChooseFirstServerRandomly;
		static const std::string vpnChooseFirstServerRandomlyN;
		static const std::string vpnServerTCPPort;
		static const std::string vpnServerTCPPortN;
		static const std::string vpnServerIPAddress1;
		static const std::string vpnServerIPAddress1N;
		static const std::string vpnServerIPAddress2;
		static const std::string vpnServerIPAddress2N;
		static const std::string vpnServerIPAddress3;
		static const std::string vpnServerIPAddress3N;
		static const std::string vpnServerIPAddress4;
		static const std::string vpnServerIPAddress4N;
		static const std::string vpnServerIPAddress5;
		static const std::string vpnServerIPAddress5N;
		static const std::string vpnServerIPAddress6;
		static const std::string vpnServerIPAddress6N;
		static const std::string vpnServerIPAddress7;
		static const std::string vpnServerIPAddress7N;
		static const std::string vpnServerIPAddress8;
		static const std::string vpnServerIPAddress8N;

//  CfgEmail
		static const std::string from;
		static const std::string fromN;
		static const std::string toList;
		static const std::string toListN;
		static const std::string smtpIpAddrOrDnsName;
		static const std::string smtpIpAddrOrDnsNameN;

		static const std::string emptyN;

//  CfgSynchronization
		static const std::string scaleSynchronizationType;
		static const std::string scaleSynchronizationTypeN;
		static const std::string primaryScale;
		static const std::string primaryScaleN;

		static const std::string syncConfigurations;
		static const std::string syncConfigurationsN;

		static const std::string syncEmailAlerts;
		static const std::string syncEmailAlertsN;

		static const std::string syncProductDefaults;
		static const std::string syncProductDefaultsN;

		static const std::string syncClasses;
		static const std::string syncClassesN;

		static const std::string syncExpandedTexts;
		static const std::string syncExpandedTextsN;

		static const std::string syncSpecialMessages;
		static const std::string syncSpecialMessagesN;

		static const std::string syncNutritionFacts;
		static const std::string syncNutritionFactsN;

		static const std::string syncProductNotes;
		static const std::string syncProductNotesN;

		static const std::string syncMarqueeTexts;
		static const std::string syncMarqueeTextsN;

		static const std::string syncTransactions;
		static const std::string syncTransactionsN;

		static const std::string syncTransactionTypes;
		static const std::string syncTransactionTypesN;

		static const std::string syncCategories;
		static const std::string syncCategoriesN;

		static const std::string syncSpecials;
		static const std::string syncSpecialsN;

		static const std::string syncCoolCategories;
		static const std::string syncCoolCategoriesN;

		static const std::string syncCoolCountries;
		static const std::string syncCoolCountriesN;

		static const std::string syncCoolProductionSteps;
		static const std::string syncCoolProductionStepsN;

		static const std::string syncCoolPreTexts;
		static const std::string syncCoolPreTextsN;

		static const std::string syncCoolTexts;
		static const std::string syncCoolTextsN;

		static const std::string syncCoolListTexts;
		static const std::string syncCoolListTextsN;

		static const std::string syncCoolLists;
		static const std::string syncCoolListsN;

		static const std::string syncCategoryCountries;
		static const std::string syncCategoryCountriesN;

		static const std::string syncCategoryProductionSteps;
		static const std::string syncCategoryProductionStepsN;

		static const std::string syncCategoryPreTexts;
		static const std::string syncCategoryPreTextsN;

		static const std::string syncProducts;
		static const std::string syncProductsN;

		static const std::string syncExpirationLabels;
		static const std::string syncExpirationLabelsN;

		static const std::string syncLabels;
		static const std::string syncLabelsN;

		static const std::string syncLabelTexts;
		static const std::string syncLabelTextsN;

		static const std::string syncOperatorNotes;
		static const std::string syncOperatorNotesN;

		static const std::string syncOperators;
		static const std::string syncOperatorsN;

		static const std::string syncSoftkeyGroups;
		static const std::string syncSoftkeyGroupsN;

		static const std::string syncSoftkeyStyles;
		static const std::string syncSoftkeyStylesN;

		static const std::string syncSoftkeyFeatures;
		static const std::string syncSoftkeyFeaturesN;

		static const std::string syncSoftkeys;
		static const std::string syncSoftkeysN;

		static const std::string syncLabelGraphics;
		static const std::string syncLabelGraphicsN;

		static const std::string syncCategoryGraphics;
		static const std::string syncCategoryGraphicsN;

		static const std::string syncProductGraphics;
		static const std::string syncProductGraphicsN;

		static const std::string syncSoftkeyGraphics;
		static const std::string syncSoftkeyGraphicsN;

		static const std::string syncSpecialsGraphics;
		static const std::string syncSpecialsGraphicsN;


//  CfgPrepack
		static const std::string enableSellBy;
		static const std::string enableSellByN;
		static const std::string enableBestBefore;
		static const std::string enableBestBeforeN;
		static const std::string enableProdImg;
		static const std::string enableProdImgN;
		static const std::string enableUpcNumber;
		static const std::string enableUpcNumberN;
		static const std::string enableFilmWidth;
		static const std::string enableFilmWidthN;
		static const std::string enableFilmTension;
		static const std::string enableFilmTensionN;
		static const std::string enableWrapTypes;
		static const std::string enableWrapTypesN;
		static const std::string enableCountryOfOrigin;
		static const std::string enableCountryOfOriginN;
		static const std::string enableTrackingNumber;
		static const std::string enableTrackingNumberN;
		static const std::string enableProductionEntry;
		static const std::string enableProductionEntryN;
		static const std::string productionEntryContinuousPrint;
		static const std::string productionEntryContinuousPrintN;
		static const std::string enableProductionPlanning;
		static const std::string enableProductionPlanningN;
		static const std::string productionPlanningFrame;
		static const std::string productionPlanningFrameN;
		static const std::string productionPlanningUpdate;
		static const std::string productionPlanningUpdateN;
		static const std::string enableSelfServe;
		static const std::string enableSelfServeN;

		static const std::string enableRunTotals;
		static const std::string enableRunTotalsN;
		static const std::string enablePackedOn;
		static const std::string enablePackedOnN;
		static const std::string rotOverrideSetting1;
		static const std::string rotOverrideSetting1N;
		static const std::string rotOverrideSetting2;
		static const std::string rotOverrideSetting2N;
		static const std::string rotOverrideSetting3;
		static const std::string rotOverrideSetting3N;
		static const std::string rotOverrideValue1;
		static const std::string rotOverrideValue1N;
		static const std::string rotOverrideValue2;
		static const std::string rotOverrideValue2N;
		static const std::string rotOverrideValue3;
		static const std::string rotOverrideValue3N;
		static const std::string pluSearchRange;
		static const std::string pluSearchRangeN;
		static const std::string appendingPluNumberToName;
		static const std::string appendingPluNumberToNameN;
		static const std::string numOfDescCharsToUse;
		static const std::string numOfDescCharsToUseN;
		static const std::string primaryLabelStockRotated;
		static const std::string primaryLabelStockRotatedN;
		static const std::string secondaryLabelStockRotated;
		static const std::string secondaryLabelStockRotatedN;

		static const std::string rwEanFormat;		//	This won't ever go into a report because it is not saved in configuration.
		static const std::string fwEanFormat;		//	This won't ever go into a report because it is not saved in configuration.
		static const std::string bcEanFormat;		//	This won't ever go into a report because it is not saved in configuration.
		static const std::string rwEanPrefix;		//	This won't ever go into a report because it is not saved in configuration.
		static const std::string fwEanPrefix;		//	This won't ever go into a report because it is not saved in configuration.
		static const std::string bcEanPrefix;		//	This won't ever go into a report because it is not saved in configuration.

//  CfgMarquee
		static const std::string permanentMarqueeNumber;
		static const std::string permanentMarqueeNumberN;
		static const std::string permanentMarqueeDisplayed;
		static const std::string permanentMarqueeDisplayedN;
		static const std::string productMarqueeDisplayed;
		static const std::string productMarqueeDisplayedN;
		static const std::string productDescMarqueeCounter;
		static const std::string productDescMarqueeCounterN;

//  CfgFTP
		static const std::string FTPUserName;
		static const std::string FTPUserNameN;
		static const std::string FTPPassword;
		static const std::string FTPPasswordN;
		static const std::string FTPSubDirectory;
		static const std::string FTPSubDirectoryN;
		static const std::string FTPServerIPorHostName;
		static const std::string FTPServerIPorHostNameN;
		// Update now below
		static const std::string FTPScheduleStartTime;
		static const std::string FTPScheduleStartTimeN;
		static const std::string FTPUseAutomatic;
		static const std::string FTPUseAutomaticN;
		static const std::string FTPmonday;
		static const std::string FTPmondayN;
		static const std::string FTPtuesday;
		static const std::string FTPtuesdayN;
		static const std::string FTPwednesday;
		static const std::string FTPwednesdayN;
		static const std::string FTPthursday;
		static const std::string FTPthursdayN;
		static const std::string FTPfriday;
		static const std::string FTPfridayN;
		static const std::string FTPsaturday;
		static const std::string FTPsaturdayN;
		static const std::string FTPsunday;
		static const std::string FTPsundayN;
		static const std::string FTPdownloadFirmwareOnly;
		static const std::string FTPdownloadInstallFirmware;
		static const std::string FTPdownloadFirmwareOnlyN;
		static const std::string FTPdownloadInstallFirmwareN;
		static const std::string FTPdailyUpdates;
		static const std::string FTPdailyUpdatesN;
		static const std::string FTPhourlyUpdates;
		static const std::string FTPhourlyUpdatesN;
		static const std::string FTP15MinuteUpdates;
		static const std::string FTP15MinuteUpdatesN;
		static const std::string FTPUpdatePLUData;
		static const std::string FTPUpdatePLUDataN;
		static const std::string FTPUpdateLabelData;
		static const std::string FTPUpdateLabelDataN;
		static const std::string FTPUpdateFlashkeyData;
		static const std::string FTPUpdateFlashkeyDataN;
		static const std::string FTPUpdateOperatorData;
		static const std::string FTPUpdateOperatorDataN;
		static const std::string FTPUpdatePlaylists;
		static const std::string FTPUpdatePlaylistsN;
		static const std::string FTPUpdateAlerts;
		static const std::string FTPUpdateAlertsN;
		static const std::string FTPUpdateScaleConfiguration;
		static const std::string FTPUpdateScaleConfigurationN;
		static const std::string FTPUpdateSynchronizationConfiguration;
		static const std::string FTPUpdateSynchronizationConfigurationN;
		static const std::string FTPUpdateStoreNameNumber;
		static const std::string FTPUpdateStoreNameNumberN;
		static const std::string FTPUpdateVideos;
		static const std::string FTPUpdateVideosN;
		static const std::string FTPUpdateImages;
		static const std::string FTPUpdateImagesN;
		static const std::string FTPUpdateAudio;
		static const std::string FTPUpdateAudioN;
		static const std::string FTPUpdateFirmware;
		static const std::string FTPUpdateFirmwareN;
		static const std::string FTPSendPLUData;
		static const std::string FTPSendPLUDataN;
		static const std::string FTPSendLabelData;
		static const std::string FTPSendLabelDataN;
		static const std::string FTPSendFlashkeyData;
		static const std::string FTPSendFlashkeyDataN;
		static const std::string FTPSendOperatorData;
		static const std::string FTPSendOperatorDataN;
		static const std::string FTPSendPlaylists;
		static const std::string FTPSendPlaylistsN;
		static const std::string FTPSendAlerts;
		static const std::string FTPSendAlertsN;
		static const std::string FTPSendScaleConfiguration;
		static const std::string FTPSendScaleConfigurationN;
		static const std::string FTPSendSynchronizationConfiguration;
		static const std::string FTPSendSynchronizationConfigurationN;
		static const std::string FTPSendStoreNameNumber;
		static const std::string FTPSendStoreNameNumberN;
		static const std::string FTPSendVideos;
		static const std::string FTPSendVideosN;
		static const std::string FTPSendImages;
		static const std::string FTPSendImagesN;
		static const std::string FTPSendAudio;
		static const std::string FTPSendAudioN;
		static const std::string FTPSendFirmware;
		static const std::string FTPSendFirmwareN;
		static const std::string FTPAllowOperatorSendToDept;
		static const std::string FTPAllowOperatorSendToDeptN;
		static const std::string FTPAllowOperatorUpdateNow;
		static const std::string FTPAllowOperatorUpdateNowN;


//  CfgCommInterface
		static const std::string CommProductClassTotalsClearTransactions;
		static const std::string CommProductClassTotalsClearTransactionsN;
		static const std::string CommHourlyTotalsClearTransactions;
		static const std::string CommHourlyTotalsClearTransactionsN;
		static const std::string CommOperatorTotalsClearTransactions;
		static const std::string CommOperatorTotalsClearTransactionsN;
		static const std::string CommCOOLTotalsClearTransactions;
		static const std::string CommCOOLTotalsClearTransactionsN;
		static const std::string CommOperatorChangesClearTransactions;
		static const std::string CommOperatorChangesClearTransactionsN;

//  CfgCurrency
		static const std::string CurrencyDecimalSeparator;
		static const std::string CurrencyMonetaryRule;
		static const std::string CurrencyTotalPriceTotalDigits;
		static const std::string CurrencyUnitPriceTotalDigits;
		static const std::string CurrencyRoundingFactor;
		static const std::string CurrencyRoundingMethod;
		static const std::string CurrencyPrimarySymbol;
		static const std::string CurrencyPrimaryDecimalDigits;
		static const std::string CurrencySecondarySymbol;
		static const std::string CurrencySecondaryDecimalDigits;
		//static const std::string currencyUnits;
		//static const std::string currencyUnitsN;
		//static const std::string monetarySymbol;
		//static const std::string monetarySymbolN;
		//static const std::string pricePrecision;
		//static const std::string pricePrecisionN;

//  CfgAccumulateItems
		static const std::string enableAccumulateItems;
		static const std::string enableAccumulateItemsN;
		static const std::string enableItemStripMode;
		static const std::string enableItemStripModeN;
		static const std::string accumItemsReturnToStartScreen;
		static const std::string accumItemsReturnToStartScreenN;
		static const std::string enableAccumItemsTotalsOnly;
		static const std::string enableAccumItemsTotalsOnlyN;
		static const std::string accumItemsEANFormatString;
		static const std::string accumItemsEANFormatStringN;
		static const std::string accumItemsEANPrefix;
		static const std::string accumItemsEANPrefixN;
		static const std::string accumItemsEANCode;
		static const std::string accumItemsEANCodeN;
		static const std::string accumItemsScaleId;
		static const std::string accumItemsScaleIdN;
		//static const std::string accumItemsTransactionNumber;  not needed; we use the tag from TransactionTags for the transaction id/number.
		static const std::string accumItemsTransactionNumberN;
		static const std::string accumItemsUPCProductNumber;
		static const std::string accumItemsUPCProductNumberN;
		static const std::string readTransactionsCompatibility;
		static const std::string readTransactionsCompatibilityN;
		static const std::string accumBarcodeUseFirstItem;
		static const std::string accumBarcodeUseFirstItemN;

		static const std::string virtualPrintingOverride2ndPrinter;
		static const std::string virtualPrintingOverride2ndPrinterN;
		static const std::string virtualPrintingOverride3rdPrinter;
		static const std::string virtualPrintingOverride3rdPrinterN;

		static const std::string displaySelfServeStartScreen;
		static const std::string displaySelfServeStartScreenN;
		static const std::string bestSellersOnSelfServeMainScreen;
		static const std::string bestSellersOnSelfServeMainScreenN;
		
		static const std::string enableLabelLength1_000;
		static const std::string enableLabelLength1_500;
		static const std::string enableLabelLength1_750;
		static const std::string enableLabelLength2_000;
		static const std::string enableLabelLength2_375;
		static const std::string enableLabelLength2_500;
		static const std::string enableLabelLength3_000;
		static const std::string enableLabelLength3_500;
		static const std::string enableLabelLength4_000;
		static const std::string enableLabelLength4_500;
		static const std::string enableLabelLength5_000;
		static const std::string enableLabelLength5_500;
		static const std::string enableLabelLength6_000;
		static const std::string enableLabelLength6_500;
		static const std::string enableLabelLength7_000;
		static const std::string enableLabelLength7_500;
		static const std::string enableLabelLength8_000;
		static const std::string enableLabelLength8_500;
		static const std::string enableLabelLength9_000;
		static const std::string enableLabelLength9_500;
		static const std::string enableLabelLength10_00;
		static const std::string enableLabelLengthContinuous;
		
		static const std::string continuousStockIgnoreLabelTaken;
		static const std::string choseToUseContinuousStock;

		static const std::string weightRequiredForMeal;

		static const std::string suggestiveSellStyle;
		static const std::string suggestiveSellFrequency;

		static const std::string alwaysEnableVirtualKeyboard;

		static const std::string showSelectAllWhenVoidingTrans;

		static const std::string periodicOfflineBeep;

		static const std::string enableGlobalPercentOff;
		static const std::string globalPercentageOff;

		static const std::string cutTestOptionEnabled;
		static const std::string maxSuspendedCutTestsOverall;
		static const std::string maxSuspendedCutTestsOperator;
		static const std::string secondsToAutoSuspendCutTest;
		static const std::string nextCutTestNumber;

		static const std::string bottomScreenContent;

		static const std::string useSecondTare;
		
		static const std::string variableShelfLifeTime;

		static const std::string diagServiceContactInfo;
		static const std::string diagGatewayDeviceAddress;
		static const std::string diagGatewayDeviceStatusMsgLogLevel;

		//  ??
		static const std::string enableAccumItemsAndFlashkeys;

		static const std::string requireFixedWeightVerification;

		static const std::string autoBackupDBFrequency;

		static const std::string FPCVersion;
		static const std::string FPCWeigherVersion;

		static const std::string scaleModel;

		static const std::string enableSearchMenuButton;

        static const std::string enableHelpPrompts;
		static const std::string enableUseCharacterSearching;

		static const std::string selfServeWeightRequiredToSelectItem;
		static const std::string selfServeByCountItemsAllowed;
		
		static const std::string printer1LabelLength;
		static const std::string printer2LabelLength;
		static const std::string printer3LabelLength;

		static const std::string defaultSoftkeyFontPointSize;

		static const std::string selfServePrintToScreen;
		static const std::string selfServeIgnorePrinterErrors;
		static const std::string selfServeSearchKeypadFlashkeys;

		// pjr383 - phase2 - super config refresh
		static const std::string selfServeStartState;
		static const std::string selfServeItemEntryState;
		static const std::string selfServeSearchState;
		static const std::string selfServePrintState;
		static const std::string selfServeAfterPrintingState;
		static const std::string selfServeStartScreenClickable;
		static const std::string selfServeIncludeKeypad;
		static const std::string selfServeHideMenuBar;
		static const std::string selfServePrintOnStableWeight;

		static const std::string selfServeDisplayPluLabel;

		static const std::string selfServeDisplayTareOnRW;
		static const std::string doNotPrintNutrifacts;

		static const std::string voidTransactionsViewHours;

		static const std::string selfServeEnablePlaylists;
		static const std::string selfServePlaylistFile;

		static const std::string mostRecentActiveDeptNum;
		static const std::string useCurrentDeptnumWhenNotReceived;

		static const std::string digimarcEnabled;
		static const std::string digimarcDotDensity;
		static const std::string digimarcDotSize;
		static const std::string digimarcWatermarkRes;
		static const std::string digimarcExpandedFreshEnabled;

		static const std::string reduceGS1Length;
		static const std::string disableHooks;

		static const std::string cameraSystemTrustLevel;

		static const std::string unitPricePrintMethod;

		static const std::string priceChangedLabelIndicator;

		static const std::string autoUpdateUPCBarcodeOver99;
		static const std::string autoUpdateUPCBarcodeOver99N;
		static const std::string barcodeTypeOver99;
		static const std::string barcodeTypeOver99N;

		static const std::string snmpTrapsIp;
		static const std::string snmpTrapsPort;
		static const std::string snmpVariablesPort;
		static const std::string snmpEnabled;

		static const std::string cutterEnabled;
		static const std::string codeCheckerEnabled;

		static const std::string handsFreeDeviceMinLoginPowerLevel;

		static const std::string allowSupervisorDotWear;
		static const std::string allowOperatorDotWear;	

		static const std::string enableSearchOnKeypad;
		static const std::string enableEnhancedCenterScreen;
		static const std::string enhancedCenterHomePage;

		static const std::string enableCombinedVirtualPrint;

        static const std::string wrapperCameraEnabled;

		//  CfgCommScheduled
		static const std::string CommScheduledUseAutomaticMerges;
		static const std::string CommScheduledUseAutomaticMergesN;
		static const std::string CommScheduledMonday;
		static const std::string CommScheduledMondayN;
		static const std::string CommScheduledTuesday;
		static const std::string CommScheduledTuesdayN;
		static const std::string CommScheduledWednesday;
		static const std::string CommScheduledWednesdayN;
		static const std::string CommScheduledThursday;
		static const std::string CommScheduledThursdayN;
		static const std::string CommScheduledFriday;
		static const std::string CommScheduledFridayN;
		static const std::string CommScheduledSaturday;
		static const std::string CommScheduledSaturdayN;
		static const std::string CommScheduledSunday;
		static const std::string CommScheduledSundayN;
		static const std::string CommScheduledDailyUpdates;
		static const std::string CommScheduledDailyUpdatesN;
		static const std::string CommScheduledStartTime;
		static const std::string CommScheduledStartTimeN;
		static const std::string CommScheduledHourlyUpdates;
		static const std::string CommScheduledHourlyUpdatesN;
		static const std::string CommScheduledExecMergeAfterComm;
		static const std::string CommScheduledExecMergeAfterCommN;
		static const std::string CommScheduledMergeHistory;
		static const std::string CommScheduledMergeHistoryN;

		static const std::string enableConserveLinerLess;
		static const std::string enableUPTagPrefix;

		static const std::string minSweetheartWeight;

		static const std::string onlineOrderingEnabled;
		static const std::string onlineOrderingServer;
		static const std::string onlineOrderingZone;


		static const std::string selfServeDisplayBarcodeLabel;
		static const std::string enableVariableShelfLifeTime; 

		static const std::string selfServeFixedPlu;
		static const std::string selfServeEnableFreshServe;
		static const std::string selfServeFlashkeyType0;
		static const std::string selfServeFlashkeyType1;
		static const std::string selfServeFlashkeyType2;
		static const std::string selfServeFlashkeyType3;
		static const std::string selfServeFlashkeyType4;
		static const std::string selfServeFlashkeyText0;
		static const std::string selfServeFlashkeyText1;
		static const std::string selfServeFlashkeyText2;
		static const std::string selfServeFlashkeyText3;
		static const std::string selfServeFlashkeyText4;
		static const std::string selfServeFlashkeyCatNum0;
		static const std::string selfServeFlashkeyCatNum1;
		static const std::string selfServeFlashkeyCatNum2;
		static const std::string selfServeFlashkeyCatNum3;
		static const std::string selfServeFlashkeyCatNum4;
		static const std::string selfServeFreshServeLabelOffset;
		static const std::string selfServeFlashkeySecText0;
		static const std::string selfServeFlashkeySecText1;
		static const std::string selfServeFlashkeySecText2;
		static const std::string selfServeFlashkeySecText3;
		static const std::string selfServeFlashkeySecText4;
		static const std::string readBarcodeFromCloud;
		static const std::string readBarcodeFromCloudN;
		static const std::string barcodeServerAddress;
		static const std::string barcodeServerAddressN;

		static const std::string ignoreVendorAndProductNumTagsWhenUpcNumTag;

		// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
		//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
		//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
		static const std::string barcodeWidthOver99;
		static const std::string barcodeOffsetFromLeftOver99;

		static const std::string allowKeyboardNetWeightItemAsRandomWeight;

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;


	private:

		static bool _tagsInitialized_;
		static TagsList configurationTags;
		static TagsList configurationNames;
		static TagsMap  fieldMap;	

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//CONFIGURATIONTAGS_H
