/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGBASEFIELDOFFSETS_H
#define CFGBASEFIELDOFFSETS_H


#define INTERNAL_CONFIGURATION_FIRST_FIELDCODE 1001


/* NOTE:  The following list contains 'holder' fieldOffsetCfgs for the end of a
 *        particular configuration's list of fieldOffsetCfgs.  This allows me to
 *        construct one list of offsets, and use it in different classes.
 *        These 'holder' fieldOffsetCfgs are:

 * These individual configuration list of fieldOffsetCfgs are primarily used by
 * CfgAll.  The order of the sections is important.  For example, the CfgOperatorAccess
 * fieldOffsetCfgs needs to be right before the CfgPrintedFields fieldOffsetCfgs.
 * The CfgAll constructor with parameters data, len, and formatType checks the end of
 * each section in order.  If the order changes, this constructor will fail to work properly.
 * LAP
 */
typedef enum CfgFieldOffsets
{
// CfgScale
	fieldOffsetCfgUnknown = 0,

	fieldOffsetCfgStoreId = 1,
	fieldOffsetCfgStoreName,
	fieldOffsetCfgStoreGraphic,
	fieldOffsetCfgDeptName_NOT_USED,

	fieldOffsetCfgPrimaryCountryCode,
	fieldOffsetCfgPrimaryLanguageCode,
	fieldOffsetCfgSecondaryCountryCode,
	fieldOffsetCfgSecondaryLanguageCode,
	fieldOffsetCfgLengthUnits,
	fieldOffsetCfgTemperatureUnits,
	fieldOffsetCfgDateFormat,
	fieldOffsetCfgTimeFormat,            

	fieldOffsetCfgUnitPriceSecurityLabel,
	fieldOffsetCfgTotalPriceSecurityLabel,
	fieldOffsetCfgFractionalPricingEnabled,
	fieldOffsetCfgPoundsForPricingEnabled,
	fieldOffsetCfgBycountCalcMethod,      
	fieldOffsetCfgClearPluAfterPrinting,

	fieldOffsetCfgBeepOnStableWeight,
	fieldOffsetCfgBeeperVolumeLevel,

	fieldOffsetCfgFixedWeightUnits,
	fieldOffsetCfgWeigherAttached,
	fieldOffsetCfgTareSetback,
	fieldOffsetCfgMaxTare,
	fieldOffsetCfgPlatterTareEnabled,
	fieldOffsetCfgChangeZeroTareOnlyOnce,
	fieldOffsetCfgChangeForcedTareOnlyOnce,
	
	fieldOffsetCfgMinWeightToPrintLabel,  // valid now only for EZ USB Weigher
	fieldOffsetCfgWeighMode,              // valid now only for EZ USB Weigher
	fieldOffsetCfgWeigherModel,           // valid now only for EZ USB Weigher
	fieldOffsetCfgIsScaleMetric,
	fieldOffsetCfgNetWeightDualRangeLimitCheck,
	
	fieldOffsetCfgExternalWeightPrecision,
	fieldOffsetCfgTareEntryPrecision,
	fieldOffsetCfgUserRezeroWeightLimit,
	fieldOffsetCfgWeigherWakeupSensorAttached,

	fieldOffsetCfgOperateLoginRequired,
	fieldOffsetCfgSupervisorLoginRequired,
	fieldOffsetCfgInactivityAutoLogoutTime,
	fieldOffsetCfgAutoLookupProduct,
	fieldOffsetCfgAutoLookupNumOfDigits,
	fieldOffsetCfgAutoLookupOverrideDigit,
	fieldOffsetCfgAutoDisplayProductNotes,
	fieldOffsetCfgAutoDisplaySuggestiveSelling,
	fieldOffsetCfgAutoDisplayCookingNotes,

	fieldOffsetCfgHorizLabelSkew1,
	fieldOffsetCfgHorizLabelSkew2,
	fieldOffsetCfgHorizLabelSkew3,

	fieldOffsetCfgCollectingTransactions,
	fieldOffsetCfgCollectingTransactionsDays,
	fieldOffsetCfgCollectingProductTotals,							//	Import only from vxWorks Access db.
	fieldOffsetCfgCollectingHourlyTotals,							//	Import only from vxWorks Access db.
	fieldOffsetCfgCollectingOperatorTotals,							//	Import only from vxWorks Access db.
	fieldOffsetCfgTrackingOperatorChanges,
//	fieldOffsetCfgDaysToKeepOperatorChanges,
	fieldOffsetCfgCollectingCOOLTotals,								//	Import only from vxWorks Access db.

	fieldOffsetCfgOpAccessToBrowserMode,
	fieldOffsetCfgOpAccessToDesktopMode,    

	fieldOffsetCfgAutoLoadOldFirmwareVersions,
	fieldOffsetCfgAutoSearchNewFirmwareVersions,
	fieldOffsetCfgNewFirmwareVersionSearchTime,
	fieldOffsetCfgForceUniqueProdClassNums,
	fieldOffsetCfgEnablePowerSaverMode,
	fieldOffsetCfgPowerSaverModeTimeout,
	fieldOffsetCfgDefaultBrowserHomePage,
	fieldOffsetCfgDefaultRunScreenHomePage,
	fieldOffsetCfgDefaultProductRecordType,      

	fieldOffsetCfgDefWrFilmWidth,
	fieldOffsetCfgDefWrFilmTension,
	fieldOffsetCfgDefWrFilmTensionFrontToBack,
	fieldOffsetCfgDefWrFilmTensionLeftToRight,
	fieldOffsetCfgDefWrIntakeMode,
	fieldOffsetCfgDefWrLabelPlacement,
	fieldOffsetCfgDefWrApplierRotation,

	fieldOffsetCfgWrSealerBeltTimeOnSun,
	fieldOffsetCfgWrSealerBeltTimeOffSun,
	fieldOffsetCfgWrSealerBeltTimeOnMon,
	fieldOffsetCfgWrSealerBeltTimeOffMon,
	fieldOffsetCfgWrSealerBeltTimeOnTue,
	fieldOffsetCfgWrSealerBeltTimeOffTue,
	fieldOffsetCfgWrSealerBeltTimeOnWed,
	fieldOffsetCfgWrSealerBeltTimeOffWed,
	fieldOffsetCfgWrSealerBeltTimeOnThu,
	fieldOffsetCfgWrSealerBeltTimeOffThu,
	fieldOffsetCfgWrSealerBeltTimeOnFri,
	fieldOffsetCfgWrSealerBeltTimeOffFri,
	fieldOffsetCfgWrSealerBeltTimeOnSat,
	fieldOffsetCfgWrSealerBeltTimeOffSat,

	fieldOffsetCfgDbBackupFrequency,
	fieldOffsetCfgAutoCompressDB,
	fieldOffsetCfgCompressDBTime,

	fieldOffsetCfgCOOLTrackingNumberEnabled,
	fieldOffsetCfgDontChangeCurrentCOOL,
                                             
	fieldOffsetCfgPrimaryExpirationPageNote,
	fieldOffsetCfgSecondExpirationPageNote,

	fieldOffsetCfgCheckNetworkConnectivity,
	fieldOffsetCfgOfflineDateTime,

	fieldOffsetCfgScaleType,  // for synchronization

	fieldOffsetCfgCommunicationHistory,
	fieldOffsetCfgCommunicationItemReceived,

	fieldOffsetCfgPopupKeypadEnabled,
	fieldOffsetCfgPopupKeypadStyle,
	fieldOffsetCfgPopupKeypadLocation,

	fieldOffsetCfgOperatorLoginKeyboardStyle,

	fieldOffsetCfgCapacityBarLocation,

	fieldOffsetCfgPingPrimary,
	fieldOffsetCfgPingGateway,     
	fieldOffsetCfgPingTarget,
	fieldOffsetCfgPingInterval,
	fieldOffsetCfgPingTargetIPAddress,

	fieldOffsetCfgRebootUnprotectedAllowed,

	fieldOffsetCfgAutoCreateRFIDOperator,

	fieldOffsetCfgFirmwareVersion,
	fieldOffsetCfgFirmwareRevision,

	fieldOffsetCfgIgnoreUltimaFormatLabelTypes,

	fieldOffsetCfgReportsOffsetTop,
	fieldOffsetCfgReportsOffsetBottom,

	fieldOffsetCfgShowDeleteAllFiles,
	fieldOffsetCfgShowPrinterSettings,

	fieldOffsetCfgRemoteAccessPermitted,
	fieldOffsetCfgRemoteAccessRequiresLogin,	// from HLX
	fieldOffsetCfgRemoteAccessLoginMethod,  // Fastrack adapted fieldOffsetCfgRemoteAccessRequiresLogin to have a possibility of 3 values, so I created a new field offset
	fieldOffsetCfgRemoteAccessUsername,
	fieldOffsetCfgRemoteAccessPassword,

	fieldOffsetCfgBackdateByDays,

	fieldOffsetCfgUnitSeparator,

	fieldOffsetCfgSystemFont,            // string
	fieldOffsetCfgStartScreenHome,       // int
	fieldOffsetCfgBrowserHasAddressBar,  // bool
	fieldOffsetCfgDefaultSearchMethod,   // int
	fieldOffsetCfgResetStatisticsDays,   // int

	fieldOffsetCfgLeftRunscreenContent,
	fieldOffsetCfgMiddleRunscreenDynamicContent,
	fieldOffsetCfgMiddleRunscreenKeypadContent,
	fieldOffsetCfgRightScreenContent,

	fieldOffsetCfgMostRecentPlanogramGraphic,
	fieldOffsetCfgMostRecentPlanogramGraphicCRC,
	fieldOffsetCfgMostRecentPlanogramAcknowledged,

	fieldOffsetCfgDatabaseContainsOperators,

	fieldOffsetCfgEnableDisplayNetworkDetails,

	fieldOffsetCfgLicensedPackages,

	fieldOffsetCfgDisableOperatorFlashkeyEditor,
	fieldOffsetCfgDisableLeftRunscreen,

	fieldOffsetCfgMostRecentRunMode,

	fieldOffsetCfgDisplayTransTypeIndicator,
	fieldOffsetCfgNowServingQueueIPAddress,
	fieldOffsetCfgNowServingSysType,

	fieldOffsetCfgDefWrSecLabelPlacement,
	fieldOffsetCfgDefWrSecApplierRotation,
	endCfgScaleFieldOffsets,

// CfgFrequentShopper
	fieldOffsetCfgFSRoundingMethod,
	fieldOffsetCfgFSDiscountMethod,
	fieldOffsetCfgFSDiscountType,
	fieldOffsetCfgFSDiscountFormula,
	fieldOffsetCfgFSPerScale,
	fieldOffsetCfgFSTotalPriceText1,
	fieldOffsetCfgFSTotalPriceText2,
	fieldOffsetCfgFSSavingsText1,
	fieldOffsetCfgFSSavingsText2,
	fieldOffsetCfgFSUnitPriceText1,
	fieldOffsetCfgFSUnitPriceText2,
	fieldOffsetCfgFSRoundingWeightPrecision,

	endCfgFSFieldOffsets,

// CfgOperatorAccess
	fieldOffsetCfgOAPriceModifiable,
	fieldOffsetCfgOAByCountModifiable,
	fieldOffsetCfgOATareModifiable,
	fieldOffsetCfgOAFixedWeightModifiable,
	fieldOffsetCfgOAShelfLifeModifiable,
	fieldOffsetCfgOAProductLifeModifiable,
	fieldOffsetCfgOALabelTypeModifiable,
	fieldOffsetCfgOADateTimeModifiable,
	fieldOffsetCfgOAPortionsModifiable,

	fieldOffsetCfgOAPriceAccessLevel,
	fieldOffsetCfgOAByCountAccessLevel,
	fieldOffsetCfgOATareAccessLevel,
	fieldOffsetCfgOAFixedWeightAccessLevel,
	fieldOffsetCfgOAShelfLifeAccessLevel,
	fieldOffsetCfgOAProductLifeAccessLevel,
	fieldOffsetCfgOALabelTypeAccessLevel,
	fieldOffsetCfgOADateTimeAccessLevel,
	fieldOffsetCfgOAPortionsAccessLevel,

	endCfgOAFieldOffsets,

// CfgPrintedFields
	fieldOffsetCfgPFSellByDatePrinted,
	fieldOffsetCfgPFSellByDateJulian,
	fieldOffsetCfgPFPackedOnDatePrinted,
	fieldOffsetCfgPFPackedOnDateJulian,
	fieldOffsetCfgPFProductLifeDatePrinted,
	fieldOffsetCfgPFProductLifeDateJulian,
	fieldOffsetCfgPFUnitPricePrinted,
	fieldOffsetCfgPFByCountPrinted,
	fieldOffsetCfgPFBarCodePrinted,
	fieldOffsetCfgPFCurrencySymbolPrinted,
	fieldOffsetCfgPFTextNamesPreprinted,
	fieldOffsetCfgPFWeightOnByCountPrinted,
	fieldOffsetCfgPFOneOnByCountPrinted,
	fieldOffsetCfgPFSecondCheckDigit,
	fieldOffsetCfgPFPrintFirstLetterOfTotalType,
	fieldOffsetCfgPFNetWtOnFixedWeightAlways,
	fieldOffsetCfgPFFixedWeightFormat,
	endCfgPFFieldOffsets,

// CfgSelfService
	fieldOffsetCfgGridXElements,
	fieldOffsetCfgGridYElements,
	fieldOffsetCfgNumOfMostCommonCategories,
	fieldOffsetCfgImageTypeOnStartScreen,
	fieldOffsetCfgInitialStreamingGraphicNum,
	fieldOffsetCfgProductStatsHoursMethod,
	fieldOffsetCfgSelfServeInactivityTimeout,
	fieldOffsetCfgProductGraphicsDirectoryName,
	fieldOffsetCfgProductGraphicsDefaultExtension,
	fieldOffsetCfgCategoryGraphicsDirectoryName,
	fieldOffsetCfgCategoryGraphicsDefaultExtension,
	fieldOffsetCfgSpecialsGraphicsDirectoryName,
	fieldOffsetCfgSpecialsGraphicsDefaultExtension,
	fieldOffsetCfgSlideShowGraphicsDirectoryName,
	fieldOffsetCfgSlideShowGraphicsDefaultExtension,
	fieldOffsetCfgSelfServeGraphicsDirectoryName,
	fieldOffsetCfgSelfServeGraphicsDefaultExtension,
	fieldOffsetCfgLogoGraphicFileName,
	fieldOffsetCfgNoGraphicAvailableFileName1,
	fieldOffsetCfgNoGraphicAvailableFileName2,
	fieldOffsetCfgPreviousGraphicFileName1,
	fieldOffsetCfgPreviousGraphicFileName2,
	fieldOffsetCfgNextGraphicFileName1,
	fieldOffsetCfgNextGraphicFileName2,
	fieldOffsetCfgSoftkeyGraphicsDirectoryName,
	fieldOffsetCfgSoftkeyGraphicsDefaultExtension,

	endCfgSSFieldOffsets,

//  CfgNetwork
/*
	fieldOffsetCfgEthernetInterfaceName,
	fieldOffsetCfgMacAddress,					// read-only
	fieldOffsetCfgNetworkSetup,
	fieldOffsetCfgIpAddress,
	fieldOffsetCfgSubnetMask,
*/
	fieldOffsetCfgWellKnownPort,

/*
	fieldOffsetCfgGatewayIpAddress1,
	fieldOffsetCfgGatewayIpAddress2,
	fieldOffsetCfgGatewayIpAddress3,						
	fieldOffsetCfgNetworkIpAddress1,
	fieldOffsetCfgNetworkIpAddress2,						
	fieldOffsetCfgNetworkIpAddress3,
	fieldOffsetCfgDnsEnabled,
	fieldOffsetCfgDnsAutomatic,							
	fieldOffsetCfgDnsHostName,
	fieldOffsetCfgDnsDomainName,							
	fieldOffsetCfgDnsIpAddress0,
	fieldOffsetCfgDnsIpAddress1,
	fieldOffsetCfgDnsIpAddress2,							
*/
	fieldOffsetCfgNetExternalCommSetting,
	fieldOffsetCfgNetServerIpAddress,
	fieldOffsetCfgNetServerHostName,
	fieldOffsetCfgNetServerAppIpAddress,
	fieldOffsetCfgNetServerAppHostName,
/*
	fieldOffsetCfgWirelessCardName,
	fieldOffsetCfgWirelessCardConnected,	//read-only 
	fieldOffsetCfgAccessPointId,
	fieldOffsetCfgUseSharedKeyAuth,
	fieldOffsetCfgWepEnabled,								
	fieldOffsetCfgActiveKeyNum,
	fieldOffsetCfgEncryptionKey1Set,						
	fieldOffsetCfgEncryptionKey2Set,
	fieldOffsetCfgEncryptionKey3Set,
	fieldOffsetCfgEncryptionKey4Set,						
	fieldOffsetCfgEncryptionKey1,
	fieldOffsetCfgEncryptionKey2,							
	fieldOffsetCfgEncryptionKey3,
	fieldOffsetCfgEncryptionKey4,
	fieldOffsetCfgTftpAutomatic,							
	fieldOffsetCfgTftpServerIpAddress,
	fieldOffsetCfgTftpBootFilePath,
	fieldOffsetCfgVpnEnabled,
	fieldOffsetCfgVpnIPAddress,
	fieldOffsetCfgLeapEnabled,								
	fieldOffsetCfgLeapUserName,
	fieldOffsetCfgLeapUserPassword,

	fieldOffsetCfgVpnCompression,
	fieldOffsetCfgVpnDomain,
	fieldOffsetCfgVpnEncryptedPassword,					
	fieldOffsetCfgVpnUsername,
	fieldOffsetCfgVpnVerifyServerCertificate,
	fieldOffsetCfgVpnNumberOfServers,
	fieldOffsetCfgVpnDeviceTypeDescriptor,
	fieldOffsetCfgVpnUserClass,							
	fieldOffsetCfgVpnMajorVerbVersion,
	fieldOffsetCfgVpnChooseFirstServerRandomly,
	fieldOffsetCfgVpnServerTCPPort,
	fieldOffsetCfgVpnServerIPAddress1,
	fieldOffsetCfgVpnServerIPAddress2,					
	fieldOffsetCfgVpnServerIPAddress3,
	fieldOffsetCfgVpnServerIPAddress4,
	fieldOffsetCfgVpnServerIPAddress5,
	fieldOffsetCfgVpnServerIPAddress6,
	fieldOffsetCfgVpnServerIPAddress7,					
	fieldOffsetCfgVpnServerIPAddress8,
*/
	endCfgNetworkFieldOffsets,

//  CfgInternal

//  CfgEmail
	fieldOffsetCfgFrom,
	fieldOffsetCfgToList,
	fieldOffsetCfgSmtpIpAddrOrDnsName,

	endCfgEmailFieldOffsets,

//	CfgSynchronization
	fieldOffsetCfgScaleSynchronizationType,
	fieldOffsetCfgPrimaryScale,

	endCfgSynchronizationFieldOffsets,

//	CfgSyncCheckList
	fieldOffsetCfgSyncConfigurations,
	fieldOffsetCfgSyncEmailAlerts,
	fieldOffsetCfgSyncProductDefaults,
	fieldOffsetCfgSyncClasses,
	fieldOffsetCfgSyncExpandedTexts,
	fieldOffsetCfgSyncSpecialMessages,
	fieldOffsetCfgSyncNutritionFacts,
	fieldOffsetCfgSyncProductNotes,
	fieldOffsetCfgSyncMarqueeTexts,
	fieldOffsetCfgSyncTransactions,
	fieldOffsetCfgSyncTransactionTypes,
	fieldOffsetCfgSyncCategories,
	fieldOffsetCfgSyncSpecials,
	fieldOffsetCfgSyncCoolCategories,
	fieldOffsetCfgSyncCoolCountries,
	fieldOffsetCfgSyncCoolProductionSteps,
	fieldOffsetCfgSyncCoolPreTexts,
	fieldOffsetCfgSyncCoolTexts,
	fieldOffsetCfgSyncCoolListTexts,
	fieldOffsetCfgSyncCoolLists,
	fieldOffsetCfgSyncCategoryCountries,
	fieldOffsetCfgSyncCategoryProductionSteps,
	fieldOffsetCfgSyncCategoryPreTexts,
	fieldOffsetCfgSyncProducts,
	fieldOffsetCfgSyncExpirationLabels,
	fieldOffsetCfgSyncLabels,
	fieldOffsetCfgSyncLabelTexts,
	fieldOffsetCfgSyncOperatorNotes,
	fieldOffsetCfgSyncOperators,
	fieldOffsetCfgSyncSoftkeyGroups,
	fieldOffsetCfgSyncSoftkeyStyles,
	fieldOffsetCfgSyncSoftkeyFeatures,
	fieldOffsetCfgSyncSoftkeys,
	fieldOffsetCfgSyncLabelGraphics,
	fieldOffsetCfgSyncCategoryGraphics,
	fieldOffsetCfgSyncProductGraphics,
	fieldOffsetCfgSyncSoftkeyGraphics,
	fieldOffsetCfgSyncSpecialsGraphics,

	endCfgSyncListFieldOffsets,

//	CfgPrepack
	fieldOffsetCfgEnableSellBy,
	fieldOffsetCfgEnableBestBefore,
	fieldOffsetCfgEnablePackedOn,
	fieldOffsetCfgEnableProdImg,
	fieldOffsetCfgEnableUpcNumber,
	fieldOffsetCfgEnableFilmWidth,
	fieldOffsetCfgEnableFilmTension,
	fieldOffsetCfgEnableWrapTypes,
	fieldOffsetCfgEnableCountryOfOrigin,
	fieldOffsetCfgEnableTrackingNumber,
	fieldOffsetCfgEnableRunTotals,
	fieldOffsetCfgEnableProductionEntry,
	fieldOffsetCfgProductionEntryContinuousPrint,
	fieldOffsetCfgEnableProductionPlanning,     
	fieldOffsetCfgProductionPlanningFrame,
	fieldOffsetCfgProductionPlanningUpdate,
	fieldOffsetCfgEnableSelfServe,
	fieldOffsetCfgPluSearchRange,
	fieldOffsetCfgAppendingPluNumberToName,
	fieldOffsetCfgNumOfDescCharsToUse,
	fieldOffsetCfgRotationOverrideSetting1,
	fieldOffsetCfgRotationOverrideValue1,
	fieldOffsetCfgRotationOverrideSetting2,
	fieldOffsetCfgRotationOverrideValue2,
	fieldOffsetCfgRotationOverrideSetting3,
	fieldOffsetCfgRotationOverrideValue3,       
	fieldOffsetCfgPrimaryLabelStockRotated,					// SOF-4283 - 08-14-23 - no longer using, may in future, formerly fieldOffsetCfgPrimaryLabelPrePrint
	fieldOffsetCfgSecondaryLabelStockRotated,				// SOF-4283 - 08-14-23 - now being used as secLabelStockRotated - formerly fieldOffsetCfgSecondaryLabelPrePrint
	endCfgPrepackFieldOffsets,

//	CfgMarquee
	fieldOffsetCfgMqPermanentMarqueeNumber,
	fieldOffsetCfgMqPermanentMarquee,
	fieldOffsetCfgMqPermanentMarqueeDisplayed,
	fieldOffsetCfgMqProductMarqueeDisplayed,
	fieldOffsetCfgMqProductDescMarqueeCounter,
	endCfgMarqueeFieldOffsets,

//	cfgFTP
	fieldOffsetCfgFTPServerIporHostName,
	fieldOffsetCfgFTPUserName,
	fieldOffsetCfgFTPPassword,
	fieldOffsetCfgFTPSubDir,
	endCfgFTPFieldOffsets,

//  cfgCommInterface
	fieldOffsetCfgCommProductClassTotalsClearTransactions,
	fieldOffsetCfgCommHourlyTotalsClearTransactions,
	fieldOffsetCfgCommOperatorTotalsClearTransactions,
	fieldOffsetCfgCommCOOLTotalsClearTransactions,
	fieldOffsetCfgCommOperatorChangesClearTransactions,
	endCfgCommFieldOffsets,

//	cfgCurrency
	fieldOffsetCfgCurrencyDecimalSeparator,
	fieldOffsetCfgCurrencyMonetaryRule,
	fieldOffsetCfgCurrencyTotalPriceTotalDigits,
	fieldOffsetCfgCurrencyUnitPriceTotalDigits,
	fieldOffsetCfgCurrencyRoundingFactor,
	fieldOffsetCfgCurrencyRoundingMethod,
	fieldOffsetCfgCurrencyPrimarySymbol,
	fieldOffsetCfgCurrencyPrimaryDecimalDigits,
	fieldOffsetCfgCurrencySecondarySymbol,
	fieldOffsetCfgCurrencySecondaryDecimalDigits,
//	fieldOffsetCfgCurrencyUnits,                       //        To be removed
//	fieldOffsetCfgMonetarySymbol,                      //        To be removed
//	fieldOffsetCfgPricePrecision,                      //        To be removed

	endCfgCurrencyFieldOffsets,

//  Graphical Customer Display
	fieldOffsetCfgCustomerDisplay,
	fieldOffsetCfgGCDLayoutNumber,
	fieldOffsetCfgGCDColorSchemeNumber,
	fieldOffsetCfgGCDAllowOperReset,
	fieldOffsetCfgGCDVolume,
	fieldOffsetCfgGCDPlaybackWaitTime,
	fieldOffsetCfgGCDMediaServerName,
	fieldOffsetCfgGCDMediaServerIP,
	fieldOffsetCfgGCDFtpUserName,
	fieldOffsetCfgGCDFtpPassword,
	fieldOffsetCfgGCDFtpDirectory,
	fieldOffsetCfgGCDPlaylistFile,
	fieldOffsetCfgGCDAutoSearch,
	fieldOffsetCfgGCDSearchHour,
	fieldOffsetCfgGCDSearchMinute,
	fieldOffsetCfgGCDSearchAmOrPM,
	fieldOffsetCfgGCDIsDnsEnabled,
	fieldOffsetCfgGCDDownloadProtocol,
	fieldOffsetCfgGCDLanguage,
	fieldOffsetCfgGCDPrintWithoutCustomerDisplay,
	fieldOffsetCfgGCDUseTransactionalLayout,
	fieldOffsetCfgGCDIdleAdvertType,
	fieldOffsetCfgGCDIdleWebsiteAddress,
	fieldOffsetCfgGCDProductAdvertType,

	endCfgGCDFieldOffsets,

// CfgAccumulateItems
	fieldOffsetCfgEnableAccumItems,               // bool
	fieldOffsetCfgEnableItemStripMode,            // bool
	fieldOffsetCfgAccumItemsReturnToStartScreen,  // bool
	fieldOffsetCfgEnableAccumItemsTotalsOnly,     // bool
	fieldOffsetCfgAccumItemsEANFormatString,      // string
	fieldOffsetCfgAccumItemsEANPrefix,            // string
	fieldOffsetCfgAccumItemsEANCode,              // string
	fieldOffsetCfgAccumItemsScaleId,              // int
	fieldOffsetCfgAccumItemsTransactionNumber,    // int?
	fieldOffsetCfgAccumItemsUPCProductNumber,     // int
	fieldOffsetCfgReadTransactionsCompatibility,  // bool
	fieldOffsetCfgAccumBarcodeUseFirstItem,       // bool

	endCfgAccumulateItemsFieldOffsets,


//  Used in CfgScale but not needed in CfgAll
	fieldOffsetCfgIsRemoteKeyboardAttached,
	fieldOffsetCfgIsUSBKeyboardAttached,
	fieldOffsetCfgSelfServiceScaleFlag,
	fieldOffsetCfgIsLeapAvailable,
	fieldOffsetCfgIsServiceJumperSet,
	fieldOffsetCfgHasWirelessCard,
	fieldOffsetCfgIsTestMessagesEnabled,
	fieldOffsetCfgHomeFlashkeyGroup,                     //       Import only from Quantum db.

//  Used in CfgEmail but not needed in CfgAll
	fieldOffsetCfgIsDnsEnabled,

	fieldOffsetCfgRFIDHWPresent,

	fieldOffsetCfgRwEanFormat,										// From UltimaTaggedFormat
	fieldOffsetCfgFwEanFormat,										// From UltimaTaggedFormat
	fieldOffsetCfgBcEanFormat,										// From UltimaTaggedFormat
	fieldOffsetCfgRwEanPrefix,										// From UltimaTaggedFormat
	fieldOffsetCfgFwEanPrefix,										// From UltimaTaggedFormat
	fieldOffsetCfgBcEanPrefix,										// From UltimaTaggedFormat
	fieldOffsetCfgRwEan128cFormat,									// From UltimaTaggedFormat						
	fieldOffsetCfgFwEan128cFormat,									// From UltimaTaggedFormat						
	fieldOffsetCfgBcEan128cFormat,									//	From UltimaTaggedFormat						

// New configuration parameters

	fieldOffsetCfgVirtualPrintingOverride2ndPrinter,
	fieldOffsetCfgVirtualPrintingOverride3rdPrinter,

	fieldOffsetCfgDisplaySelfServeStartScreen,
	fieldOffsetCfgBestSellersOnSelfServeMainScreen,

	fieldOffsetCfgEnableLabelLength_1_000,
	fieldOffsetCfgEnableLabelLength_1_500,
	fieldOffsetCfgEnableLabelLength_1_750,
	fieldOffsetCfgEnableLabelLength_2_000,
	fieldOffsetCfgEnableLabelLength_2_375,
	fieldOffsetCfgEnableLabelLength_2_500,
	fieldOffsetCfgEnableLabelLength_3_000,
	fieldOffsetCfgEnableLabelLength_3_500,
	fieldOffsetCfgEnableLabelLength_4_000,
	fieldOffsetCfgEnableLabelLength_4_500,
	fieldOffsetCfgEnableLabelLength_5_000,
	fieldOffsetCfgEnableLabelLength_5_500,
	fieldOffsetCfgEnableLabelLength_6_000,
	fieldOffsetCfgEnableLabelLength_6_500,
	fieldOffsetCfgEnableLabelLength_7_000,
	fieldOffsetCfgEnableLabelLength_7_500,
	fieldOffsetCfgEnableLabelLength_8_000,
	fieldOffsetCfgEnableLabelLength_8_500,
	fieldOffsetCfgEnableLabelLength_9_000,
	fieldOffsetCfgEnableLabelLength_9_500,
	fieldOffsetCfgEnableLabelLength_10_00,
	fieldOffsetCfgEnableLabelLength_Continuous,

	fieldOffsetCfgContinuousStockIgnoreLabelTaken,

	fieldOffsetCfgWeightRequiredForMeal,         // BuildAMeal (Walmart)

	fieldOffsetCfgSuggestiveSellStyle,           // numeric, 0-2, 0 default, 0 = no button, 1 = okay button, 2 = yes/no buttons (Walmart)
	fieldOffsetCfgSuggestiveSellFrequency,       // numeric, 1-99, 1 default, this is how often suggestive sell automatically pops to the operator (Walmart)

	fieldOffsetCfgAlwaysEnableVirtualKeyboard,     // POS 30, for Stater Bros.

	fieldOffsetCfgShowSelectAllWhenVoidingTrans,   // Part of POS 32, Delhaize

	fieldOffsetCfgPeriodicOfflineBeep,             // POS 33, Walmart, in seconds, integer (0 - 999)

	fieldOffsetCfgEnableGlobalPercentOff,          // POS 55 - Global Frequent Shopper Percent Discount
	fieldOffsetCfgGlobalPercentageOff,             // POS 55 - Global Frequent Shopper Percent Discount

	fieldOffsetCfgIsSingleRangeWeigher,				// This is not stored in the database; it is just a field offset to be able to let the UIL know if we have a single-range weigher, via CfgScale

	fieldOffsetCfgEnableAccumItemsAndFlashkeys,			// POS 42.1, allow dismissal of Accumulate Items dialog to use flashkeys

	fieldOffsetCfgRequireFixedWeightVerification,		// POS 42.1, fixed weight verification

	// The frequency is in "# of times per day", so: 0 = never auto-backup, 1 = once, at midnight, 2 = twice, at noon and midnight, 24 = every hour, on the hour
	// 48 = every hour and every half hour, 96 = every 15 minutes, 1440 = every minute
	fieldOffsetCfgAutoBackupDBFrequency,

	fieldOffsetCfgFPCVersion,

	fieldOffsetCfgScaleModel,

	fieldOffsetCfgCutTestOptionEnabled,				// POS 26, Cut Test Interruption
	fieldOffsetCfgMaxSuspendedCutTestsOverall,		// int
	fieldOffsetCfgMaxSuspendedCutTestsOperator,		// int
	fieldOffsetCfgSecondsToAutoSuspendCutTest,		// int
	fieldOffsetCfgNextCutTestNumber,				// int

	fieldOffsetCfgBottomScreenContent,				// int

	fieldOffsetCfgEnableSearchMenuButton,           // POS - 68 Remove Search Capability  

	fieldOffsetCfgEnableHelpPrompts,				// bool - Walmart Canada Request char by char search
	fieldOffsetCfgEnableUseCharacterSearching,		// bool - Walmart Canada Request char by char search

	fieldOffsetCfgUseSecondTare,
	fieldOffsetCfgTrackingSupervisorChanges,
	fieldOffsetCfgVariableShelfLifeTime,			// POS 52.1: HT-365

	//Addtional UPDATE NOW fields
	fieldOffsetCfgFTPstartTime,
	fieldOffsetCfgFTPuseAutomatic,
	fieldOffsetCfgFTPmonday,
	fieldOffsetCfgFTPtuesday,
	fieldOffsetCfgFTPwednesday,
	fieldOffsetCfgFTPthursday,
	fieldOffsetCfgFTPfriday,
	fieldOffsetCfgFTPsaturday,
	fieldOffsetCfgFTPsunday,
	fieldOffsetCfgFTPdownloadFirmwareOnly,
	fieldOffsetCfgFTPdownloadInstallFirmware,
	fieldOffsetCfgFTPdailyUpdates,
	fieldOffsetCfgFTPhourlyUpdates,
	fieldOffsetCfgFTP15MinuteUpdates,
	fieldOffsetCfgFTPUpdatePLUData,
	fieldOffsetCfgFTPUpdateLabelData,
	fieldOffsetCfgFTPUpdateFlashkeyData,
	fieldOffsetCfgFTPUpdateOperatorData,
	fieldOffsetCfgFTPUpdatePlaylists,
	fieldOffsetCfgFTPUpdateAlerts,
	fieldOffsetCfgFTPUpdateScaleConfiguration,
	fieldOffsetCfgFTPUpdateSynchronizationConfiguration,
	fieldOffsetCfgFTPUpdateStoreNameNumber,
	fieldOffsetCfgFTPUpdateVideos,
	fieldOffsetCfgFTPUpdateImages,
	fieldOffsetCfgFTPUpdateAudio,
	fieldOffsetCfgFTPUpdateFirmware,
	fieldOffsetCfgFTPSendPLUData,
	fieldOffsetCfgFTPSendLabelData,
	fieldOffsetCfgFTPSendFlashkeyData,
	fieldOffsetCfgFTPSendOperatorData,
	fieldOffsetCfgFTPSendPlaylists,
	fieldOffsetCfgFTPSendAlerts,
	fieldOffsetCfgFTPSendScaleConfiguration,
	fieldOffsetCfgFTPSendSynchronizationConfiguration,
	fieldOffsetCfgFTPSendStoreNameNumber,
	fieldOffsetCfgFTPSendVideos,
	fieldOffsetCfgFTPSendImages,
	fieldOffsetCfgFTPSendAudio,
	fieldOffsetCfgFTPSendFirmware,
	fieldOffsetCfgFTPAllowOperatorSendToDept,
	fieldOffsetCfgFTPAllowOperatorUpdateNow,
	//End additional UPDATE NOW fields
	
	fieldOffsetCfgSelfServeWeightRequiredToSelectItem,
	fieldOffsetCfgSelfServeByCountItemsAllowed,

	fieldOffsetCfgPrinter1LabelLength,
	fieldOffsetCfgPrinter2LabelLength,
	fieldOffsetCfgPrinter3LabelLength,

	fieldOffsetCfgDefaultSoftkeyFontPointSize,

	fieldOffsetCfgDiagServiceContactInfo,
	fieldOffsetCfgDiagGatewayDeviceAddress,
	fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel,

	fieldOffsetCfgSelfServePrintToScreen,				// Amazon GO - Print To Screen
    fieldOffsetCfgSelfServeDisplayTareOnRW,
	fieldOffsetDoNotPrintNutrifacts,

	fieldOffsetCfgVoidTransactionsViewHours,			// SOF-43

	fieldOffsetCfgMostRecentActiveDeptNum,

	fieldOffsetCfgDigimarcEnabled,
	fieldOffsetCfgDigimarcDotDensity,
	fieldOffsetCfgDigimarcDotSize,
	fieldOffsetCfgDigimarcWatermarkRes,
	fieldOffsetCfgDigimarcExpandedFreshEnabled,

	fieldOffsetCfgReduceGS1Length,
	fieldOffsetCfgDisableHooks,

	fieldOffsetCfgCameraSystemTrustLevel,

	fieldOffsetCfgUseCurrentDeptnumWhenNotReceived, 

	fieldOffsetCfgChoseToUseContinuousStock, 

	fieldOffsetCfgUnitPricePrintMethod,

	// This is not for enabling/disabling this feature; it is for allowing the user to turn on/off the checking of the total price and/or weight before printing, via a flashkey action
	// Thus, it is not saved to the database, nor sent via comm/backup/restore/upgrade.
	// It is always set to false when the scale boots up. (per Cherie B. 10/1/2018)
    fieldOffsetCfgPick5Active,                          // 507

	fieldOffsetCfgPriceChangedLabelIndicator,
	fieldOffsetCfgValueMaxEnabled,
	fieldOffsetCfgValueMaxDiagCollectionDisabled,

	fieldOffsetCfgSNMPTrapsIp,							// SOF-1681
	fieldOffsetCfgSNMPTrapsPort,
	fieldOffsetCfgSNMPVariablesPort,
	fieldOffsetCfgSNMPEnabled,

    fieldOffsetCfgIsCutterEnabled,
	fieldOffsetCfgIsCodeCheckerEnabled,					// 515

	fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel,

	fieldOffsetCfgAllowSupervisorDotWear,
    fieldOffsetCfgAllowOperatorDotWear,

    fieldOffsetCfgSelfServeEnablePlaylists,
	fieldOffsetCfgSelfServePlaylistFile,

	fieldOffsetCfgEnableSearchOnKeypad,
	fieldOffsetCfgEnableEnhancedCenterScreen,
	fieldOffsetCfgEnhancedCenterHomePage,

	fieldOffsetCfgFPCWeigherVersion,

	fieldOffsetCfgAutoUpdateUPCBarcodeOver99,
	fieldOffsetCfgBarcodeTypeOver99,
	fieldOffsetCfgVirtualCombinePrint,					// PDD-146 SOF-3847
	fieldOffsetCfgIsConserveLinerLess,					// SOF-3875
	fieldOffsetCfgSelfServeIgnorePrinterErrors,				// SOF-3950 ComboBox, BigY displayed: 0= Print Label, 1= Scan Label (ignore errors), 2= Scan and Print Label
	fieldOffsetCfgSelfServeSearchKeypadFlashkeys,			// SOF-3950 Checkbox, BigY allowed Combine Search, Keypad, and Flashkeys (stop using this one)
	fieldOffsetCfgSelfServeDisplayPluLabel,					// SOF-3950 Checkbox, BigY: print the label assigned to the PLU
	
	fieldOffsetCfgEnableUPTagPrefix,

	fieldOffsetCfgMinSweetheartWeight,		// SOF-4016 - PJR260

	fieldOffsetCfgOnlineOrderingEnabled,

	fieldOffsetCfgSelfServeDisplayBarcodeLabel,				// Checkbox, Display the barcode label to the screen
	fieldOffsetCfgSelfServeStartState,
	fieldOffsetCfgSelfServeItemEntryState,
	fieldOffsetCfgSelfServeSearchState,
	fieldOffsetCfgSelfServePrintState,
	fieldOffsetCfgSelfServeAfterPrintingState,
	fieldOffsetCfgSelfServeStartScreenClickable,
	fieldOffsetCfgSelfServeIncludeKeypad,
	fieldOffsetCfgSelfServeHideMenuBar,
    fieldOffsetCfgSelfServePrintOnStableWeight,
    
	fieldOffsetCfgOnlineOrderingServer,
	fieldOffsetCfgOnlineOrderingZone,

    fieldOffsetCfgEnableVariableShelfLifeTime,				// PJR 420 - Flashkey Options to Toggle Extra Day

    // (SOF-4769) Comm Interface: Scheduling
	fieldOffsetCfgCommScheduledUseAutomaticMerges,
	fieldOffsetCfgCommScheduledMonday,
	fieldOffsetCfgCommScheduledTuesday,
	fieldOffsetCfgCommScheduledWednesday,
	fieldOffsetCfgCommScheduledThursday,
	fieldOffsetCfgCommScheduledFriday,
	fieldOffsetCfgCommScheduledSaturday,
	fieldOffsetCfgCommScheduledSunday,
	fieldOffsetCfgCommScheduledDailyUpdates,
	fieldOffsetCfgCommScheduledStartTime,
	fieldOffsetCfgCommScheduledHourlyUpdates,
	fieldOffsetCfgCommScheduledExecMergeAfterComm,
	fieldOffsetCfgCommScheduledMergeHistory,
	fieldOffsetCfgWrapperCameraEnabled,

	//PJR413 (SOF-4772)
	fieldOffsetCfgSelfServeFixedPlu,					// Checkbox, Walmart: Go straight to weight, no Plu selection
    fieldOffsetCfgSelfServeEnableFreshServe,
	fieldOffsetCfgSelfServeFlashkeyType0,
	fieldOffsetCfgSelfServeFlashkeyType1,
	fieldOffsetCfgSelfServeFlashkeyType2,
	fieldOffsetCfgSelfServeFlashkeyType3,
	fieldOffsetCfgSelfServeFlashkeyType4,
	fieldOffsetCfgSelfServeFlashkeyText0,
	fieldOffsetCfgSelfServeFlashkeyText1,
	fieldOffsetCfgSelfServeFlashkeyText2,
	fieldOffsetCfgSelfServeFlashkeyText3,
	fieldOffsetCfgSelfServeFlashkeyText4,
	fieldOffsetCfgSelfServeFlashkeyCatNum0,
	fieldOffsetCfgSelfServeFlashkeyCatNum1,
	fieldOffsetCfgSelfServeFlashkeyCatNum2,
	fieldOffsetCfgSelfServeFlashkeyCatNum3,
	fieldOffsetCfgSelfServeFlashkeyCatNum4,
	fieldOffsetCfgSelfServeFreshServeLabelOffset,
	fieldOffsetCfgSelfServeFlashkeySecText0,
	fieldOffsetCfgSelfServeFlashkeySecText1,
	fieldOffsetCfgSelfServeFlashkeySecText2,
	fieldOffsetCfgSelfServeFlashkeySecText3,
	fieldOffsetCfgSelfServeFlashkeySecText4,
	fieldOffsetCfgReadBarcodeFromCloud,
	fieldOffsetCfgBarcodeServerAddress,

	fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag,		// SOF-5893, related to SOF-3880

	// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
	//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
	//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
	fieldOffsetCfgBarcodeWidthOver99,
	fieldOffsetBarcodeOffsetFromLeftOver99, 

	fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight,

	endCfgFieldOffsets,                                 // END



//	NOTE:	HfcObject's receivedFlags array is currently set to a size of 40.
//			That means that a maximum number of 320 field offsets can exist.
//			Currently, Configuration contains the largest number of fields, which are defined here.
//	SO:	You may need to increase maxFieldOffsetReceivedBytes in HfcObject.h if these field offsets
//			go past 320. Otherwise, things won't work they way you think they should. (JRB 3/23/2008)



	fieldOffsetCfgIScaleModel = INTERNAL_CONFIGURATION_FIRST_FIELDCODE,	 // ScaleModel, from Weigher.h
	fieldOffsetCfgIScaleName,						   // HobString
	fieldOffsetCfgIScaleServiceName,					// HobString
	fieldOffsetCfgIWebServiceName,					  // HobString
	fieldOffsetCfgIAppInstallerExe,					 // HobString

	fieldOffsetCfgISystemGraphicsLocation,				// HobString
	fieldOffsetCfgIUserGraphicsLocation,				// HobString
	fieldOffsetCfgIImagesLocation,						// HobString
	fieldOffsetCfgISystemUserRootDir,					// HobString
	fieldOffsetCfgIGraphicsExtensions,					// HobString
	fieldOffsetCfgIGraphicFileSpecifiers,				// HobString
	fieldOffsetCfgINoGraphicSelectedName,				// HobString
	fieldOffsetCfgISystemGraphicsTar,					// HobString
	fieldOffsetCfgIUserGraphicsTar,						// HobString
	fieldOffsetCfgIProductGraphicsTar,					// HobString
	fieldOffsetCfgIProductGraphicsDir,					// HobString
	fieldOffsetCfgISystemProductGraphicsDir,			// HobString
	fieldOffsetCfgIProductGraphicsDefaultExtension,		// HobString
	fieldOffsetCfgICategoryGraphicsTar,					// HobString
	fieldOffsetCfgICategoryGraphicsDir,					// HobString
	fieldOffsetCfgISystemCategoryGraphicsDir,			// HobString
	fieldOffsetCfgICategoryGraphicsDefaultExtension,	// HobString
	fieldOffsetCfgISpecialsGraphicsTar,					// HobString
	fieldOffsetCfgISpecialsGraphicsDir,					// HobString
	fieldOffsetCfgISpecialsGraphicsDefaultExtension,	// HobString
	fieldOffsetCfgIStreamingGraphicsTar,				// HobString
	fieldOffsetCfgIStreamingGraphicsDir,				// HobString
	fieldOffsetCfgIStreamingGraphicsDefaultExtension,   // HobString
	fieldOffsetCfgISoftkeyGraphicsTar,					// HobString
	fieldOffsetCfgISoftkeyGraphicsDir,					// HobString
	fieldOffsetCfgISoftkeyGraphicsDefaultExtension,		// HobString
	fieldOffsetCfgIReservedSoftkeyGraphicsDir,			// HobString
	fieldOffsetCfgISelfServeGraphicsDir,				// HobString
	fieldOffsetCfgISystemSelfServeGraphicsDir,			// HobString
	fieldOffsetCfgISelfServeGraphicsDefaultExtension,   // HobString
	fieldOffsetCfgIPlanogramGraphicsDir,
	fieldOffsetCfgIPlanogramGraphicsDefaultExtension,
	fieldOffsetCfgIVideosDir,
	fieldOffsetCfgIVideosDefaultExtension,

	fieldOffsetCfgILangFileName,

	fieldOffsetCfgIWebPagesRootDir,					 // HobString
	fieldOffsetCfgIGeneralWebPageDirName, 		  // HobString
	fieldOffsetCfgILinuxWebPageDirName,			  // HobString
	fieldOffsetCfgILanguageFilesDir,					// HobString
	fieldOffsetCfgIPythonScriptsDir,

	fieldOffsetCfgIKeyMountDir,						 // HobString

	fieldOffsetCfgIServiceLoginId,					  // HobString
	fieldOffsetCfgIServiceKeyFile,					  // HobString
	fieldOffsetCfgIServiceKeyRemoveTimeout,			 // int
	fieldOffsetCfgISuperKeyFile,						// HobString
	fieldOffsetCfgISelfServeKeyFile,					// HobString

	fieldOffsetCfgIDirectorySeparator,				  // HobString
	fieldOffsetCfgITempDBDir,						   // HobString
	fieldOffsetCfgIDbLocation,						  // HobString
	fieldOffsetCfgIEmptyDbLocation,					 // HobString
	fieldOffsetCfgIDbDefaultsFile,					  // HobString
	fieldOffsetCfgIDbBackupEnabled,                   // bool
	fieldOffsetCfgIDbBackupLocation,					// HobString
	fieldOffsetCfgIDbMirrorLocation,					// HobString
	fieldOffsetCfgIDbCheckBeforeShutdown,			   // bool
	fieldOffsetCfgIDbCheckIntegrity,
	fieldOffsetCfgIDbCheckIntegrityFrequency,
	fieldOffsetCfgICheckIndicesBeforeOpenDatabase,	  // bool
	fieldOffsetCfgIRebuildIndicesEnabled,			   // bool
	fieldOffsetCfgIVersionIndependentBackupFileName,	// HobString
	fieldOffsetCfgIAutoLoadVersionIndependentData,	  // bool
	fieldOffsetCfgIBackupVersionIndependentDataNow,	  // bool
	fieldOffsetCfgIMakeCopyOfVersionIndependentData,	  // bool

	fieldOffsetCfgIAutoBackupDatabaseLocation,
	fieldOffsetCfgIAutoBackupPLUDataFile,
	fieldOffsetCfgIAutoBackupFlashkeyDataFile,
	fieldOffsetCfgIAutoBackupLabelDataFile,
	fieldOffsetCfgIAutoBackupOperatorDataFile,
	fieldOffsetCfgIAutoBackupStoreNameNumberFile,
	fieldOffsetCfgIAutoBackupConfigFile,
	fieldOffsetCfgIAutoBackupSynchConfigFile,

	fieldOffsetCfgIPrimaryConsoleSessionPriority,	   // int
	fieldOffsetCfgIRemoteConsoleSessionPriority,		// int
	fieldOffsetCfgIMaxRemoteConsoleSessions,			// int
	fieldOffsetCfgIMaxConsecutiveRTCMessages,		   // short
	fieldOffsetCfgIEventManagerEnabled,				 // bool
	fieldOffsetCfgISessionManagerEnabled,			   // bool
	fieldOffsetCfgISessionManagerStartupDelay,		// int, in msec
	fieldOffsetCfgISessionManagerPeriodicTimer,
	fieldOffsetCfgIRealTimeControllerEnabled,		   // bool
	fieldOffsetCfgITimerManagerEnabled,				 // bool
	fieldOffsetCfgIVersionManagerEnabled,			   // bool
	fieldOffsetCfgIUSBEnabledAtBootup,				  // bool
	fieldOffsetCfgIInterruptXferTimeout,				// unsigned long
	fieldOffsetCfgIBulkXferTimeout,					 // unsigned long
	fieldOffsetCfgIExpiredRecordsCheckFrequency,     // int

	fieldOffsetCfgIFakeWeigherManagerEnabled,		   // bool
	fieldOffsetCfgIFakeWeigherFrequency,				// int
	fieldOffsetCfgIFakePrinterManagerEnabled,		   // bool
	fieldOffsetCfgIFakeCLAManagerEnabled,			   // bool
	fieldOffsetCfgIFakeWrapperManagerEnabled,		   // bool
	fieldOffsetCfgITestBrowserEnabled,				  // bool
	fieldOffsetCfgITestBrowserMsgDelay,				 // int
	fieldOffsetCfgITestMessagesEnabled,				 // bool

	fieldOffsetCfgIRFIDEnabled,						 // bool
	fieldOffsetCfgIRFIDDebounceTime,					// unsigned long
	fieldOffsetCfgIRFIDComPort,						 // unsigned long
	fieldOffsetCfgIRFIDHWPresent,					   // bool

	fieldOffsetCfgIDiagnosticsManagerEnabled,			// bool
	fieldOffsetCfgIDiagnosticsDBName,					// HobString
	fieldOffsetCfgIDiagnosticsDBBackupName,				// HobString
	fieldOffsetCfgIDiagnosticsDBOldName,				// HobString

	fieldOffsetCfgIPrinterProbeTime,					// unsigned long
	fieldOffsetCfgIWeigherProbeTime,					// unsigned long
	fieldOffsetCfgICustDispProbeTime,				   // unsigned long
	fieldOffsetCfgILabelApplierProbeTime,			   // unsigned long
	fieldOffsetCfgIWrapperProbeTime,					// unsigned long

	fieldOffsetCfgIUSBBulkDriveOnlyMode,				// bool
	fieldOffsetCfgIUSBBulkDriveName,					// HobString
	fieldOffsetCfgICompactFlashDriveName,			   // HobString
	fieldOffsetCfgIPermanentRamDriveName,			   // HobString
	fieldOffsetCfgITemporaryRamDriveName,			   // HobString
	fieldOffsetCfgIEvolutionDirectory,				  // HobString
	fieldOffsetCfgIBootParametersFileName,			  // HobString
	fieldOffsetCfgIDefaultNetCfgDir,					// HobString
	fieldOffsetCfgITmpNetCfgDir,						// HobString
	fieldOffsetCfgIFirmwareDirectory,				   // HobString
	fieldOffsetCfgIFirmwareGenericFilename,			  // HobString
	fieldOffsetCfgIFirmwareFilenames,			  // HobString

	fieldOffsetCfgIVmTempDir,						   // HobString
	fieldOffsetCfgIVmForceUpgrade,					  // bool
	fieldOffsetCfgIDefaultUpgradeFileName,			  // HobString
	fieldOffsetCfgIInstalLogFile,					   // HobString
	fieldOffsetCfgIVersionInfoFileName,				 // HobString
	fieldOffsetCfgIDefaultUpgradeDirectory,
	fieldOffsetCfgIUpgradeZipFile,
	fieldOffsetCfgIUpgradeIniFile,

	fieldOffsetCfgIMaxGraphicsPerProduct,			   // short
	fieldOffsetCfgIProductNameMaxLength,             // int
	fieldOffsetCfgIUpdatePrinterConfig,				 // bool
	fieldOffsetCfgIUseOnlyPrimaryLanguageForProductStatistics,	// bool
	fieldOffsetCfgIPercentagePrecision,				 // short
	fieldOffsetCfgIMaxLifeDays,						 // short
	fieldOffsetCfgIMaxLifeHours,						// short
	fieldOffsetCfgIMaxBonusPoints,					  // int
	fieldOffsetCfgIQuarterPoundFor,					 // short
	fieldOffsetCfgIHalfPoundFor,						// short
	fieldOffsetCfgIHundredGramsFor,					 // short
	fieldOffsetCfgIContinuousStatisticsDelay,		   // int
	fieldOffsetCfgILostPeripheralsCheckTime,			// int

	fieldOffsetCfgIKeyboardNetWeightPrecision,

	fieldOffsetCfgISubtractDescent,					 // bool
	fieldOffsetCfgIMaxLabelNumber,					  // int
	fieldOffsetCfgILabelFeedHeelFirst,				  // bool
	fieldOffsetCfgITestLabelType,					   // PrTestLabelType, from Printer.h
	fieldOffsetCfgIIgnoreLabelFit,					  // bool
	fieldOffsetCfgILabelHorizontalOffset,			   // short
	fieldOffsetCfgIDefaultNutriLabelTemplate,		   // HobString
	fieldOffsetCfgIShelfLifeExpLabelFamily,			 // int
	fieldOffsetCfgIExpirationLabelExpiredLimit,		 // int
	fieldOffsetCfgIRunTotalsLabelFamily,			 // int

	fieldOffsetCfgIFBWFActivated,
	fieldOffsetCfgIUsingRestrictedLogins,
	fieldOffsetCfgIAppInstallerIsService,
	fieldOffsetCfgIStartupDelay,
	fieldOffsetCfgISimulatePrinting,					// bool
	fieldOffsetCfgIMonitorMemoryInterval,		//	int

	fieldOffsetCfgIDualRangeLimitAvoir,				 // int
	fieldOffsetCfgIDualRangeLimitMetric,				// int
	fieldOffsetCfgIRezeroWeightLimitAvoir,			  // int
	fieldOffsetCfgIRezeroWeightLimitMetric,			 // int

	fieldOffsetCfgIConnectionIdentificationTimer,	   // int
	fieldOffsetCfgIConnectionIdentificationDelay,	   // int
	fieldOffsetCfgICommSessionReadMessageTimer,		 // int
	fieldOffsetCfgICommSessionReadMessageDelay,		 // int	 
	fieldOffsetCfgIWaitBeforeConnectingTimer,		   // int
	fieldOffsetCfgIWaitBeforeConnectingDelay,		   // int

	fieldOffsetCfgIPingConnectivityFrequency,		   // int
	fieldOffsetCfgIPingConnectivityAttempts,			// int
	fieldOffsetCfgIOfflinePingConnectivityFrequency,
	fieldOffsetCfgIOfflinePingConnectivityAttempts,

	fieldOffsetCfgISynchronizationEnabled,			  // bool
	fieldOffsetCfgISynchronizeAllSecondariesTimer,	  // int
	fieldOffsetCfgINotifyLocalBatchesTimer,			 // int
	fieldOffsetCfgIRecordsPerSyncIteration,			 // int
	fieldOffsetCfgIDelayPerSyncIteration,			   // int
	fieldOffsetCfgIMaximumNumberRetries,				// int
	fieldOffsetCfgIRetryCommandTimer,				   // int
	fieldOffsetCfgIHoursBeforeDeregistering,			// int
	fieldOffsetCfgIDelayPurgingBatches,				 // int
	fieldOffsetCfgIHoursBeforeRemovingDeletedBatches,   // int
	fieldOffsetCfgIRecordsPerApplyIteration,			// int
	fieldOffsetCfgIDelayPerApplyIteration,			  // int
	fieldOffsetCfgIDelayPerDeleteIteration,			 // int
	fieldOffsetCfgISynchronizationStatusMessagesEnabled,

	fieldOffsetCfgIUseProductionPlanningDialog,						// bool
	fieldOffsetCfgIHTTPRequestMethod,								// int
	fieldOffsetCfgIHTTPRequestContentType,							// HobString
	fieldOffsetCfgIHTTPWaitingDelay,								// int
	fieldOffsetCfgIHTTPReadResponseTimer,							// int
	fieldOffsetCfgIHTTPReadSomeResponseDelay,						// int
	fieldOffsetCfgIHTTPReadNoResponseDelay,							// int
	fieldOffsetCfgIHTTPMaxResponseLineLength,						// int
	fieldOffsetCfgIHTTPReportErrors,								// bool
	fieldOffsetCfgIHTTPQueryStore,									// HobString
	fieldOffsetCfgIHTTPQueryDept,									// HobString
	fieldOffsetCfgIHTTPQueryModel,									// HobString
	fieldOffsetCfgIHTTPQueryPLU,									// HobString
	fieldOffsetCfgIHTTPQueryWeight,									// HobString
	fieldOffsetCfgIHTTPQueryQuantity,								// HobString
	fieldOffsetCfgIHTTPQueryLanguage,								// HobString
	fieldOffsetCfgIHTTPUseOfflineSpool,								// bool
	fieldOffsetCfgIHTTPOfflineSpoolRetry,							// int
	fieldOffsetCfgIHTTPOfflineSpoolMaxRequestAge,					// int

	fieldOffsetCfgIExpandedTextWidth,				   // int
	fieldOffsetCfgIExpandedTextHeight,				  // int
	fieldOffsetCfgIExpandedTextFontName,				// HobString
	fieldOffsetCfgIExpandedTextPointSize,			   // int
	fieldOffsetCfgISpecialMessageWidth,				 // int
	fieldOffsetCfgISpecialMessageHeight,				// int
	fieldOffsetCfgISpecialMessageFontName,			  // HobString
	fieldOffsetCfgISpecialMessagePointSize,			 // int
	fieldOffsetCfgIProductNoteWidth,				   // int
	fieldOffsetCfgIProductNoteHeight,				  // int
	fieldOffsetCfgIProductNoteFontName,				// HobString
	fieldOffsetCfgIProductNotePointSize,			   // int
	fieldOffsetCfgIMarqueeTextWidth,					   // int
	fieldOffsetCfgIMarqueeTextHeight,					  // int
	fieldOffsetCfgIMarqueeTextFontName,					// HobString
	fieldOffsetCfgIMarqueeTextPointSize,				   // int
	fieldOffsetCfgIProductText5Width,				   // int
	fieldOffsetCfgIProductText5Height,				  // int
	fieldOffsetCfgIProductText5FontName,				// HobString
	fieldOffsetCfgIProductText5PointSize,			   // int
	fieldOffsetCfgIProductText6Width,				   // int
	fieldOffsetCfgIProductText6Height,				  // int
	fieldOffsetCfgIProductText6FontName,				// HobString
	fieldOffsetCfgIProductText6PointSize,			   // int
	fieldOffsetCfgIProductText7Width,				   // int
	fieldOffsetCfgIProductText7Height,				  // int
	fieldOffsetCfgIProductText7FontName,				// HobString
	fieldOffsetCfgIProductText7PointSize,			   // int
	fieldOffsetCfgIProductText8Width,				   // int
	fieldOffsetCfgIProductText8Height,				  // int
	fieldOffsetCfgIProductText8FontName,				// HobString
	fieldOffsetCfgIProductText8PointSize,			   // int
	fieldOffsetCfgIProductText9Width,				   // int
	fieldOffsetCfgIProductText9Height,				  // int
	fieldOffsetCfgIProductText9FontName,				// HobString
	fieldOffsetCfgIProductText9PointSize,			   // int

	fieldOffsetCfgIProdDescriptionWidth,				// int
	fieldOffsetCfgIProdDescriptionHeight,			   // int
	fieldOffsetCfgIProdDescriptionFontName,			 // HobString
	fieldOffsetCfgIProdDescriptionPointSize,			// int
	fieldOffsetCfgICOOLTrackingTextWidth,			   // int
	fieldOffsetCfgICOOLTrackingTextHeight,			  // int
	fieldOffsetCfgICOOLTrackingTextFontName,			// HobString
	fieldOffsetCfgICOOLTrackingTextPointSize,		   // int

	// The following are used by PrinterStockMetrics.cpp, .h
	fieldOffsetCfgIMaxLabelLengthPct,
	fieldOffsetCfgIDimBLength1_000,
	fieldOffsetCfgIDimBLength1_500,
	fieldOffsetCfgIDimBLength1_750,
	fieldOffsetCfgIDimBLength2_000,
	fieldOffsetCfgIDimBLength2_375,
	fieldOffsetCfgIDimBLength2_500,
	fieldOffsetCfgIDimBLength3_000,
	fieldOffsetCfgIDimBLength3_500,
	fieldOffsetCfgIDimBLength4_000,
	fieldOffsetCfgIDimBLength4_500,
	fieldOffsetCfgIDimBLength5_000,
	fieldOffsetCfgIDimBLength5_500,
	fieldOffsetCfgIDimBLength6_000,
	fieldOffsetCfgIDimBLength6_500,
	fieldOffsetCfgIDimBLength7_000,
	fieldOffsetCfgIDimBLength7_500,
	fieldOffsetCfgIDimBLength8_000,
	fieldOffsetCfgIDimBLength8_500,
	fieldOffsetCfgIDimBLength9_000,
	fieldOffsetCfgIDimBLength9_500,
	fieldOffsetCfgIDimBLength10_00,
	fieldOffsetCfgIDimBLengthContinuous,

	// GCD on/off
	fieldOffsetCfgIGCDEnabled,							// bool
	fieldOffsetCfgIGCDEnabledStandalone,			// bool
	fieldOffsetCfgIDesktopStoreLogoPath,
	fieldOffsetCfgIDesktopStoreLogoFileName,
	fieldOffsetCfgIDeviceStoreLogoPath,
	fieldOffsetCfgIDeviceStoreLogoFileName,
	fieldOffsetCfgIDeviceApplicationUpdatesDir,
	fieldOffsetCfgIDeviceApplicationUpdatesSourceFileName,
	fieldOffsetCfgIDevicePlaylistUpdatesDir,
	fieldOffsetCfgILocalTempDir,
	fieldOffsetCfgIAdapterDescSearchRNDIS,
	fieldOffsetCfgIIpSubnetMask,
	fieldOffsetCfgIIpAddressHLX,
	fieldOffsetCfgIIpAddressGCD,
	fieldOffsetCfgIGcdServerPort, // int
	fieldOffsetCfgIGcdDisplayObjectXmlPathFormat,
	fieldOffsetCfgIGcdDiscoveryMessage,
	fieldOffsetCfgIGcdDiscoveryPort, //int

	fieldOffsetCfgIGoodBeepEnabled,
	fieldOffsetCfgIGoodBeepFrequency,
	fieldOffsetCfgIGoodBeepPeriod,
	fieldOffsetCfgIGoodBeepDelay,
	fieldOffsetCfgIBadBeepEnabled,
	fieldOffsetCfgIBadBeepFrequency,
	fieldOffsetCfgIBadBeepPeriod,
	fieldOffsetCfgIBadBeepDelay,

	fieldOffsetCfgIWebPagesDir,						 // HobString

	fieldOffsetCfgIUpKeyboardXCoordinate,			  // int
	fieldOffsetCfgIUpKeyboardYCoordinate,			  // int
	fieldOffsetCfgIUpKeyboardWidth,				  // int
	fieldOffsetCfgIUpKeyboardHeight,				  // int
	fieldOffsetCfgIDownKeyboardXCoordinate,				// int
	fieldOffsetCfgIDownKeyboardYCoordinate,				// int
	fieldOffsetCfgIDownKeyboardWidth,					// int
	fieldOffsetCfgIDownKeyboardHeight,					// int
	fieldOffsetCfgIMiddleKeyboardXCoordinate,				 // int
	fieldOffsetCfgIMiddleKeyboardYCoordinate,				 // int
	fieldOffsetCfgIMiddleKeyboardWidth,					 // int
	fieldOffsetCfgIMiddleKeyboardHeight,					 // int

	fieldOffsetCfgIProductDiscountLevel1Label,
	fieldOffsetCfgIProductDiscountLevel2Label,
	fieldOffsetCfgIProductDiscountLevel3Label,


	fieldOffsetCfgIDefaultFontName,
	fieldOffsetCfgINoFontSelectedName,
	fieldOffsetCfgIFontExtensions,
	fieldOffsetCfgISystemFontsDir,
	fieldOffsetCfgIUserFontsDir,

	fieldOffsetCfgINotifyUIOfUpdateStatus,

	fieldOffsetCfgIDelayBetweenManagerThreadMessages,
	fieldOffsetCfgIDelayBetweenSessionMessages,

	fieldOffsetCfgIParameterNotFoundThrowsException,

	fieldOffsetCfgINumberOfProductTexts,

	fieldOffsetCfgIAccumItemsLabelFamilyStart,
	fieldOffsetCfgIAccumItemsItemLabelType,
	fieldOffsetCfgIAccumItemsTotalsOnlyLabelFamilyStart,
	fieldOffsetCfgIAccumItemsItemCountRenderNotify,
	fieldOffsetCfgIAccumItemsSimulatePrinting,

	fieldOffsetCfgIPrinterStatusNonRepeatingMsgsToUI,

	fieldOffsetCfgIContinuousNetworkStatusDelay,
	fieldOffsetCfgINetworkConnectivityOnlineWaitPeriodSecs,
	fieldOffsetCfgINetworkConnectivityOfflineWaitPeriodSecs,
	fieldOffsetCfgINetworkConnectivityOfflineRetransmissionPeriodSecs,
	fieldOffsetCfgINetworkConnectivityInterfaceDetectionTime,

	fieldOffsetCfgIAutoTesterNeedsLabelTaken,

	fieldOffsetCfgIPlanogramsEnabled,

	fieldOffsetCfgIMaxBestSellerSoftkeys,
	fieldOffsetCfgIMaxBestSellerSoftkeysFromFeature,

	fieldOffsetCfgISoftKeyPluNumberBeforeName,  // if true then plu number comes before name (prepend), else if false then plu number comes after name (appended)
	fieldOffsetCfgISoftKeyNewlineBetweenPluNumberAndName,

	fieldOffsetCfgIMessageLoggingDirectory,					//	Added so we could change it for Safeway to use the same directory as Windux
	fieldOffsetCfgIMessageLoggingInfoFilename,
	fieldOffsetCfgIMessageLoggingWarningsFilename,
	fieldOffsetCfgIMessageLoggingErrorsFilename,
	fieldOffsetCfgIMessageLoggingStartupShutdownFilename,
	fieldOffsetCfgIMessageLoggingScaleEventsFilename,       // Access
	fieldOffsetCfgIMessageLoggingDataMaintenanceFilename,   // Access
	fieldOffsetCfgIMessageLoggingScheduledEventsFilename,   // Access
	fieldOffsetCfgIMessageLoggingMaxFileSize,				// Access: # of bytes. Used to be defined in PAL_platformheader.h.

	fieldOffsetCfgIRemoteScaleMaintenanceEnabled,   		// Access
	fieldOffsetCfgIRemoteScaleMaintClrOutboxDays,   		// Access

	fieldOffsetCfgIOperAccessLevelsForProductsEnabled,

	fieldOffsetCfgIRecallsEnabled,

	fieldOffsetCfgIProductSupportTextLabelNum,
	fieldOffsetCfgIProductUINutriLabelNum,

	fieldOffsetCfgIUnDimScreenOnZeroWeight,
	fieldOffsetCfgIUnDimScreenOnNonZeroWeight,
	fieldOffsetCfgIUnDimScreenOnTapPlatter,

	fieldOffsetCfgICommRequiresDataTypeTag,

	fieldOffsetCfgIDownloadDir,
	fieldOffsetCfgIDownloadFileSpecifiers,
	fieldOffsetCfgIDownloadCheckingIntervalTimer,
	fieldOffsetCfgIDownloadCheckingUpdateTimer,
	fieldOffsetCfgIDownloadProcessingDir,
	fieldOffsetCfgIDownloadOutputDir,

	fieldOffsetCfgIMergeDir,
	fieldOffsetCfgIMergeFileSpecifiers,

	fieldOffsetCfgIKeepCopyOfProcessedData,
	fieldOffsetCfgIMakeProcessedFilenameUnique,
	fieldOffsetCfgIProcessedDir,

	fieldOffsetCfgIPythonEnabled,
	fieldOffsetCfgISmBackendPort,
	fieldOffsetCfgIPythonScriptPort,
	fieldOffsetCfgIPythonSftpUpdatePort,
	fieldOffsetCfgIPythonDBBackupResponsePort,
	fieldOffsetCfgIPythonDeviceEventMessageScriptName,

	fieldOffsetCfgIScaleManagementHandlesUpgrade,

	fieldOffsetCfgICoreDumpEnabled,

	fieldOffsetCfgIDefaultPingInterval,

	fieldOffsetCfgISwitchToRunScreenOnStableWeight,

	fieldOffsetCfgISelfServeAlphabeticSearchQuery,

	fieldOffsetCfgILicensePackageStrings,
	fieldOffsetCfgILicensedPackageStrings,

	fieldOffsetCfgIUSBPrinterOutputFilename,
	fieldOffsetCfgIUSBPrinterXCoordinate,
	fieldOffsetCfgIUSBPrinterYCoordinate,

	fieldOffsetCfgINetworkPrinterDriversDirectory,
	fieldOffsetCfgIKnownUsbPrinters,
	fieldOffsetCfgIKnownUsbPrinterDrivers,
	fieldOffsetCfgIKnownLanPrinters,
	fieldOffsetCfgIKnownLanPrinterDrivers,

	fieldOffsetCfgIExtPrintersPDFExtensions,
	fieldOffsetCfgIExtPrintersDOCExtensions,
	fieldOffsetCfgIDefaultGhostScriptDevice,
	fieldOffsetCfgIGhostScriptDevices,

	fieldOffsetCfgIMfgTestServer,

	fieldOffsetCfgIStagingDir,

	fieldOffsetCfgIStaticProductDataTarballName,

    fieldOffsetCfgIScannerEnabled,            // bool

	fieldOffsetCfgIMaxCutTestNumber,		// int
	fieldOffsetCfgIRequireMeatSpecSheet,	// bool

	fieldOffsetCfgICat3AuditTrailDefaultFileName,
	fieldOffsetCfgICat3WeigherConfigFileName,

	fieldOffsetCfgISendSmBackendMessageScriptName,

	fieldOffsetCfgISelfServePrintToScreenFilename,
	fieldOffsetCfgISelfServePrintToScreenLabelType,
	fieldOffsetCfgISelfServePrintToScreenLabelTypeFS,

	fieldOffsetCfgIConserveContinuousStockByNotSizing,

	fieldOffsetCfgIUnitPricePrintOptionsEnabled,
	fieldOffsetCfgIunitPricePrintExactCalcBackupMethod,

	fieldOffsetCfgISaveLabel,
	fieldOffsetCfgIIgnoreVendorAndProductNumTagsWhenUpcNumTag,		// SOF-3880
	fieldOffsetCfgIForceNonZeroByCountForFixedWeightItems,			// SOF-3882

	fieldOffsetCfgIAllowProductionEntryModeOnWrapper,

	fieldOffsetCfgIScaleSettingsFile,

	fieldOffsetCfgIfpmManagerNotATeamPlayer,					// Used for laptop/desktop development. Fixes startup crash

	fieldOffsetCfgITemplateLabelBarcodeOffset,
	fieldOffsetCfgITemplateLabelShowOperatorDialogs,

	fieldOffsetCfgIUtilizeCustomerHookForOperateModeOnly,

	fieldOffsetCfgIHFPDDIpAddress,
	fieldOffsetCfgIHFPDDPort,
	fieldOffsetCfgIConsecutiveTimesClosestToBeLoggedIn,
	fieldOffsetCfgIGetDevicesInLoginRangeInterval,
	fieldOffsetCfgIGetDevicesInLoginRangeAge,
	fieldOffsetCfgIGetDevicesInRangeInterval,
	fieldOffsetCfgIGetDevicesInRangeAge,
	fieldOffsetCfgIIgnoreLabelTakenDieCut,
	
	fieldOffsetCfgIBAMNumOfSidesRepresentingUnlimitedSides,

	fieldOffsetCfgIBackupRomLabels,

	fieldOffsetCfgIPrintToScreenBarcodeLabelType,
	fieldOffsetCfgIPrintToScreenBarcodeDirname,
	fieldOffsetCfgIPrintToScreenBarcodeFilename,
	fieldOffsetCfgIPrintToScreenBarcodeExtension,
	fieldOffsetCfgISelfServeWeightFieldWidth,
	fieldOffsetCfgISelfServeWeightFieldHeight,
	fieldOffsetCfgISelfServeWeightFieldFontSize,

	fieldOffsetCfgICameraSystemPredictWaitTime,
	fieldOffsetCfgICameraSystemPredictLoops,
	fieldOffsetCfgICameraSystemAllowedInOperate,
	fieldOffsetCfgICameraSystemAllowedInItemScreen,

	// SOF-4216
	fieldOffsetCfgILimitRunInMax,


	fieldOffsetCfgIMaxLifeMinutes,					// short
	fieldOffsetSelfServeTimeout,
	fieldOffsetCfgBarcodeFormatToUseOffline,
    fieldOffsetCfgBarcodeRequestTimeout,

	//fieldOffsetCfgI
	endCfgInternalFieldOffsets

}   CfgFieldOffsets_t;

#endif //CFGBASEFIELDOFFSETS_H
