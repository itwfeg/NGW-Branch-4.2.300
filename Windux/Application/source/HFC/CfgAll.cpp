/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgAll.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Debug.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"

std::list<CfgFieldOffsets> CfgAll::evolFieldList;
std::list<CfgFieldOffsets> CfgAll::ultimaFieldList;
std::list<CfgFieldOffsets> CfgAll::synchronizationList;
std::list<CfgFieldOffsets> CfgAll::dbIndependentList;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgAll::dataType = 0;
std::string CfgAll::dataTypeName("CfgAll"); 
#endif



CfgAll::CfgAll()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

CfgAll::CfgAll(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	receivedData_.assign(data, len);
	receivedFormat_ = formatType;
}

CfgAll::~CfgAll()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


//private
void CfgAll::init()
{
	cfgSelfService          = Configuration::getInstance()->getCfgSelfService();
	cfgNetwork              = Configuration::getInstance()->getCfgNetwork();
	cfgEmail                = Configuration::getInstance()->getCfgEmail();
	cfgMarquee              = Configuration::getInstance()->getCfgMarquee();
	cfgCommInterface        = Configuration::getInstance()->getCfgCommInterface();
	cfgCurrency             = Configuration::getInstance()->getCfgCurrency();

	initFieldList();
}

//private
void CfgAll::initFieldList()
{
	if (evolFieldList.empty())
	{
// CfgScale
		evolFieldList.push_back(fieldOffsetCfgStoreId);
		evolFieldList.push_back(fieldOffsetCfgStoreName);
		evolFieldList.push_back(fieldOffsetCfgStoreGraphic);
		evolFieldList.push_back(fieldOffsetCfgDeptName_NOT_USED);

		evolFieldList.push_back(fieldOffsetCfgPrimaryCountryCode);
		evolFieldList.push_back(fieldOffsetCfgPrimaryLanguageCode);
		evolFieldList.push_back(fieldOffsetCfgSecondaryCountryCode);
		evolFieldList.push_back(fieldOffsetCfgSecondaryLanguageCode);
		evolFieldList.push_back(fieldOffsetCfgLengthUnits);
		evolFieldList.push_back(fieldOffsetCfgTemperatureUnits);
		evolFieldList.push_back(fieldOffsetCfgDateFormat);
		evolFieldList.push_back(fieldOffsetCfgTimeFormat);

		evolFieldList.push_back(fieldOffsetCfgUnitPriceSecurityLabel);
		evolFieldList.push_back(fieldOffsetCfgTotalPriceSecurityLabel);
		evolFieldList.push_back(fieldOffsetCfgFractionalPricingEnabled);
		evolFieldList.push_back(fieldOffsetCfgPoundsForPricingEnabled);
		evolFieldList.push_back(fieldOffsetCfgBycountCalcMethod);
		evolFieldList.push_back(fieldOffsetCfgClearPluAfterPrinting);

		evolFieldList.push_back(fieldOffsetCfgBeepOnStableWeight);
		evolFieldList.push_back(fieldOffsetCfgBeeperVolumeLevel);

		evolFieldList.push_back(fieldOffsetCfgExternalWeightPrecision);

		evolFieldList.push_back(fieldOffsetCfgFixedWeightUnits);
		evolFieldList.push_back(fieldOffsetCfgWeigherAttached);
		evolFieldList.push_back(fieldOffsetCfgTareSetback);
		evolFieldList.push_back(fieldOffsetCfgMaxTare);
		evolFieldList.push_back(fieldOffsetCfgPlatterTareEnabled);
		evolFieldList.push_back(fieldOffsetCfgChangeZeroTareOnlyOnce);
		evolFieldList.push_back(fieldOffsetCfgChangeForcedTareOnlyOnce);
		// evolFieldList.push_back(fieldOffsetCfgTareEntryPrecision);  as of April 2017, no longer configurable, per W&M
		evolFieldList.push_back(fieldOffsetCfgUserRezeroWeightLimit);
		evolFieldList.push_back(fieldOffsetCfgWeigherWakeupSensorAttached);

		evolFieldList.push_back(fieldOffsetCfgOperateLoginRequired);
		//fieldOffsetCfgSupervisorLoginRequired
		evolFieldList.push_back(fieldOffsetCfgInactivityAutoLogoutTime);
		evolFieldList.push_back(fieldOffsetCfgAutoLookupProduct);
		evolFieldList.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		evolFieldList.push_back(fieldOffsetCfgAutoLookupOverrideDigit);
		evolFieldList.push_back(fieldOffsetCfgAutoDisplayProductNotes);
		evolFieldList.push_back(fieldOffsetCfgAutoDisplaySuggestiveSelling);
		evolFieldList.push_back(fieldOffsetCfgAutoDisplayCookingNotes);

		//evolFieldList.push_back(fieldOffsetCfgHorizLabelSkew1);
		//evolFieldList.push_back(fieldOffsetCfgHorizLabelSkew2);
		//evolFieldList.push_back(fieldOffsetCfgHorizLabelSkew3);

		evolFieldList.push_back(fieldOffsetCfgCollectingTransactions);
		evolFieldList.push_back(fieldOffsetCfgCollectingTransactionsDays);
		evolFieldList.push_back(fieldOffsetCfgCollectingProductTotals);
		evolFieldList.push_back(fieldOffsetCfgCollectingHourlyTotals);
		evolFieldList.push_back(fieldOffsetCfgCollectingOperatorTotals);
		evolFieldList.push_back(fieldOffsetCfgTrackingOperatorChanges);
//		evolFieldList.push_back(fieldOffsetCfgDaysToKeepOperatorChanges);
		evolFieldList.push_back(fieldOffsetCfgCollectingCOOLTotals);

		evolFieldList.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		evolFieldList.push_back(fieldOffsetCfgOpAccessToDesktopMode);

		evolFieldList.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		evolFieldList.push_back(fieldOffsetCfgAutoSearchNewFirmwareVersions);
		evolFieldList.push_back(fieldOffsetCfgNewFirmwareVersionSearchTime);
		evolFieldList.push_back(fieldOffsetCfgForceUniqueProdClassNums);
		evolFieldList.push_back(fieldOffsetCfgEnablePowerSaverMode);
		evolFieldList.push_back(fieldOffsetCfgPowerSaverModeTimeout);
		evolFieldList.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		evolFieldList.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		evolFieldList.push_back(fieldOffsetCfgDefaultProductRecordType);

		evolFieldList.push_back(fieldOffsetCfgDefWrFilmWidth);
		evolFieldList.push_back(fieldOffsetCfgDefWrFilmTension);
		//evolFieldList.push_back(fieldOffsetCfgDefWrFilmTensionFrontToBack);
		//evolFieldList.push_back(fieldOffsetCfgDefWrFilmTensionLeftToRight);
		evolFieldList.push_back(fieldOffsetCfgDefWrIntakeMode);
		evolFieldList.push_back(fieldOffsetCfgDefWrLabelPlacement);
		evolFieldList.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		evolFieldList.push_back(fieldOffsetCfgDefWrApplierRotation);
		evolFieldList.push_back(fieldOffsetCfgDefWrSecApplierRotation);

		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSun);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSun);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnMon);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffMon);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnTue);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffTue);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnWed);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffWed);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnThu);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffThu);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnFri);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffFri);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSat);
		evolFieldList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSat);

		evolFieldList.push_back(fieldOffsetCfgDbBackupFrequency);
		evolFieldList.push_back(fieldOffsetCfgAutoCompressDB);
		evolFieldList.push_back(fieldOffsetCfgCompressDBTime);

		evolFieldList.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);
		evolFieldList.push_back(fieldOffsetCfgDontChangeCurrentCOOL);

		evolFieldList.push_back(fieldOffsetCfgPrimaryExpirationPageNote);
		evolFieldList.push_back(fieldOffsetCfgSecondExpirationPageNote);

		evolFieldList.push_back(fieldOffsetCfgCheckNetworkConnectivity);
		//evolFieldList.push_back(fieldOffsetCfgOfflineDateTime);

		evolFieldList.push_back(fieldOffsetCfgScaleType);

		evolFieldList.push_back(fieldOffsetCfgCommunicationHistory);
		evolFieldList.push_back(fieldOffsetCfgCommunicationItemReceived);

		evolFieldList.push_back(fieldOffsetCfgPopupKeypadEnabled);
		evolFieldList.push_back(fieldOffsetCfgPopupKeypadStyle);
		evolFieldList.push_back(fieldOffsetCfgPopupKeypadLocation);

		evolFieldList.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);

		evolFieldList.push_back(fieldOffsetCfgCapacityBarLocation);

		evolFieldList.push_back(fieldOffsetCfgPingPrimary);
		evolFieldList.push_back(fieldOffsetCfgPingGateway);
		evolFieldList.push_back(fieldOffsetCfgPingTarget);
		evolFieldList.push_back(fieldOffsetCfgPingInterval);
		evolFieldList.push_back(fieldOffsetCfgPingTargetIPAddress);

		evolFieldList.push_back(fieldOffsetCfgRebootUnprotectedAllowed);

		evolFieldList.push_back(fieldOffsetCfgAutoCreateRFIDOperator);

		evolFieldList.push_back(fieldOffsetCfgFirmwareVersion);			//	Serialize only
		evolFieldList.push_back(fieldOffsetCfgFirmwareRevision);		//	Serialize only

		evolFieldList.push_back(fieldOffsetCfgIgnoreUltimaFormatLabelTypes);

		evolFieldList.push_back(fieldOffsetCfgReportsOffsetTop);
		evolFieldList.push_back(fieldOffsetCfgReportsOffsetBottom);

		evolFieldList.push_back(fieldOffsetCfgShowDeleteAllFiles);
		evolFieldList.push_back(fieldOffsetCfgShowPrinterSettings);

		evolFieldList.push_back(fieldOffsetCfgRemoteAccessPermitted);
		evolFieldList.push_back(fieldOffsetCfgRemoteAccessLoginMethod);
		evolFieldList.push_back(fieldOffsetCfgRemoteAccessUsername);
		evolFieldList.push_back(fieldOffsetCfgRemoteAccessPassword);

		evolFieldList.push_back(fieldOffsetCfgBackdateByDays);

		evolFieldList.push_back(fieldOffsetCfgSystemFont);
		evolFieldList.push_back(fieldOffsetCfgStartScreenHome);
		evolFieldList.push_back(fieldOffsetCfgBrowserHasAddressBar);
		evolFieldList.push_back(fieldOffsetCfgDefaultSearchMethod);
		evolFieldList.push_back(fieldOffsetCfgResetStatisticsDays);

		evolFieldList.push_back(fieldOffsetCfgLeftRunscreenContent);
		evolFieldList.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		evolFieldList.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);

		evolFieldList.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		evolFieldList.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		evolFieldList.push_back(fieldOffsetCfgDisableLeftRunscreen);

		evolFieldList.push_back(fieldOffsetCfgRightScreenContent);
		evolFieldList.push_back(fieldOffsetCfgDisplayTransTypeIndicator);	//	FT-1466
		evolFieldList.push_back(fieldOffsetCfgNowServingQueueIPAddress);
		evolFieldList.push_back(fieldOffsetCfgNowServingSysType);

// CfgFrequentShopper
		evolFieldList.push_back(fieldOffsetCfgFSRoundingMethod);
		evolFieldList.push_back(fieldOffsetCfgFSDiscountMethod);
		evolFieldList.push_back(fieldOffsetCfgFSDiscountType);
		evolFieldList.push_back(fieldOffsetCfgFSDiscountFormula);
		evolFieldList.push_back(fieldOffsetCfgFSTotalPriceText1);
		evolFieldList.push_back(fieldOffsetCfgFSTotalPriceText2);
		evolFieldList.push_back(fieldOffsetCfgFSSavingsText1);
		evolFieldList.push_back(fieldOffsetCfgFSSavingsText2);
		evolFieldList.push_back(fieldOffsetCfgFSUnitPriceText1);
		evolFieldList.push_back(fieldOffsetCfgFSUnitPriceText2);
		evolFieldList.push_back(fieldOffsetCfgFSRoundingWeightPrecision);

// CfgOperatorAccess
		evolFieldList.push_back(fieldOffsetCfgOAPriceModifiable);
		evolFieldList.push_back(fieldOffsetCfgOAByCountModifiable);
		evolFieldList.push_back(fieldOffsetCfgOATareModifiable);
		evolFieldList.push_back(fieldOffsetCfgOAFixedWeightModifiable);
		evolFieldList.push_back(fieldOffsetCfgOAShelfLifeModifiable);
		evolFieldList.push_back(fieldOffsetCfgOAProductLifeModifiable);
		evolFieldList.push_back(fieldOffsetCfgOALabelTypeModifiable);
		evolFieldList.push_back(fieldOffsetCfgOADateTimeModifiable);
		evolFieldList.push_back(fieldOffsetCfgOAPortionsModifiable);

		evolFieldList.push_back(fieldOffsetCfgOAPriceAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOAByCountAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOATareAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOAFixedWeightAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOAShelfLifeAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOAProductLifeAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOALabelTypeAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOADateTimeAccessLevel);
		evolFieldList.push_back(fieldOffsetCfgOAPortionsAccessLevel);

// CfgPrintedFields
		evolFieldList.push_back(fieldOffsetCfgPFSellByDatePrinted);
		evolFieldList.push_back(fieldOffsetCfgPFSellByDateJulian);
		evolFieldList.push_back(fieldOffsetCfgPFPackedOnDatePrinted);
		evolFieldList.push_back(fieldOffsetCfgPFPackedOnDateJulian);
		evolFieldList.push_back(fieldOffsetCfgPFProductLifeDatePrinted);
		evolFieldList.push_back(fieldOffsetCfgPFProductLifeDateJulian);
		evolFieldList.push_back(fieldOffsetCfgPFUnitPricePrinted);
		evolFieldList.push_back(fieldOffsetCfgPFByCountPrinted);
		evolFieldList.push_back(fieldOffsetCfgPFBarCodePrinted);
		evolFieldList.push_back(fieldOffsetCfgPFCurrencySymbolPrinted);
		evolFieldList.push_back(fieldOffsetCfgPFTextNamesPreprinted);
		evolFieldList.push_back(fieldOffsetCfgPFWeightOnByCountPrinted);
		evolFieldList.push_back(fieldOffsetCfgPFOneOnByCountPrinted);
		evolFieldList.push_back(fieldOffsetCfgPFSecondCheckDigit);
		evolFieldList.push_back(fieldOffsetCfgPFPrintFirstLetterOfTotalType);
		evolFieldList.push_back(fieldOffsetCfgPFNetWtOnFixedWeightAlways);
		evolFieldList.push_back(fieldOffsetCfgPFFixedWeightFormat);

// CfgSelfService
		evolFieldList.push_back(fieldOffsetCfgGridXElements);
		evolFieldList.push_back(fieldOffsetCfgGridYElements);
		evolFieldList.push_back(fieldOffsetCfgNumOfMostCommonCategories);
		evolFieldList.push_back(fieldOffsetCfgImageTypeOnStartScreen);
		evolFieldList.push_back(fieldOffsetCfgInitialStreamingGraphicNum);
		evolFieldList.push_back(fieldOffsetCfgProductStatsHoursMethod);
		evolFieldList.push_back(fieldOffsetCfgSelfServeInactivityTimeout);
		evolFieldList.push_back(fieldOffsetCfgLogoGraphicFileName);
		evolFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName1);
		evolFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName2);
		evolFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName1);
		evolFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName2);
		evolFieldList.push_back(fieldOffsetCfgNextGraphicFileName1);
		evolFieldList.push_back(fieldOffsetCfgNextGraphicFileName2);

//  CfgNetwork
/*
		evolFieldList.push_back(fieldOffsetCfgAccessPointId);
		evolFieldList.push_back(fieldOffsetCfgUseSharedKeyAuth);
		evolFieldList.push_back(fieldOffsetCfgWepEnabled);
		evolFieldList.push_back(fieldOffsetCfgActiveKeyNum);
		evolFieldList.push_back(fieldOffsetCfgEncryptionKey1);
		evolFieldList.push_back(fieldOffsetCfgEncryptionKey2);
		evolFieldList.push_back(fieldOffsetCfgEncryptionKey3);
		evolFieldList.push_back(fieldOffsetCfgEncryptionKey4);

		evolFieldList.push_back(fieldOffsetCfgVpnEnabled);
		evolFieldList.push_back(fieldOffsetCfgVpnIPAddress);
		evolFieldList.push_back(fieldOffsetCfgVpnCompression);
		evolFieldList.push_back(fieldOffsetCfgVpnDomain);
		evolFieldList.push_back(fieldOffsetCfgVpnEncryptedPassword);
		evolFieldList.push_back(fieldOffsetCfgVpnUsername);
		evolFieldList.push_back(fieldOffsetCfgVpnVerifyServerCertificate);
		evolFieldList.push_back(fieldOffsetCfgVpnNumberOfServers);
		evolFieldList.push_back(fieldOffsetCfgVpnDeviceTypeDescriptor);
		evolFieldList.push_back(fieldOffsetCfgVpnUserClass);
		evolFieldList.push_back(fieldOffsetCfgVpnMajorVerbVersion);
		evolFieldList.push_back(fieldOffsetCfgVpnChooseFirstServerRandomly);
		evolFieldList.push_back(fieldOffsetCfgVpnServerTCPPort);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress1);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress2);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress3);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress4);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress5);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress6);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress7);
		evolFieldList.push_back(fieldOffsetCfgVpnServerIPAddress8);
*/
//  CfgEmail
		evolFieldList.push_back(fieldOffsetCfgFrom);
		evolFieldList.push_back(fieldOffsetCfgToList);
		evolFieldList.push_back(fieldOffsetCfgSmtpIpAddrOrDnsName);

//  CfgSynchronization
//		evolFieldList.push_back(fieldOffsetCfgScaleSynchronizationType);
//		evolFieldList.push_back(fieldOffsetCfgPrimaryScale);

//  CfgPrepack
		evolFieldList.push_back(fieldOffsetCfgEnableSellBy);
		evolFieldList.push_back(fieldOffsetCfgEnableBestBefore);
		evolFieldList.push_back(fieldOffsetCfgEnablePackedOn);
		evolFieldList.push_back(fieldOffsetCfgEnableProdImg);
		evolFieldList.push_back(fieldOffsetCfgEnableUpcNumber);
		evolFieldList.push_back(fieldOffsetCfgEnableFilmWidth);
		evolFieldList.push_back(fieldOffsetCfgEnableFilmTension);
		evolFieldList.push_back(fieldOffsetCfgEnableWrapTypes);
		evolFieldList.push_back(fieldOffsetCfgEnableCountryOfOrigin);
		evolFieldList.push_back(fieldOffsetCfgEnableTrackingNumber);
		evolFieldList.push_back(fieldOffsetCfgEnableRunTotals);
		evolFieldList.push_back(fieldOffsetCfgEnableProductionEntry);
		evolFieldList.push_back(fieldOffsetCfgProductionEntryContinuousPrint);
		evolFieldList.push_back(fieldOffsetCfgEnableProductionPlanning);
		evolFieldList.push_back(fieldOffsetCfgProductionPlanningFrame);
		evolFieldList.push_back(fieldOffsetCfgProductionPlanningUpdate);
		evolFieldList.push_back(fieldOffsetCfgEnableSelfServe);
		evolFieldList.push_back(fieldOffsetCfgPluSearchRange);
		evolFieldList.push_back(fieldOffsetCfgAppendingPluNumberToName);
		evolFieldList.push_back(fieldOffsetCfgNumOfDescCharsToUse);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideSetting1);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideValue1);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideSetting2);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideValue2);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideSetting3);
		evolFieldList.push_back(fieldOffsetCfgRotationOverrideValue3);
		evolFieldList.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		evolFieldList.push_back(fieldOffsetCfgSecondaryLabelStockRotated);		

//  CfgMarquee
		evolFieldList.push_back(fieldOffsetCfgMqPermanentMarqueeNumber);
		//evolFieldList.push_back(fieldOffsetCfgMqPermanentMarqueeDisplayed);
		evolFieldList.push_back(fieldOffsetCfgMqProductMarqueeDisplayed);
		evolFieldList.push_back(fieldOffsetCfgMqProductDescMarqueeCounter);

//  CfgFTP
		evolFieldList.push_back(fieldOffsetCfgFTPServerIporHostName);
		evolFieldList.push_back(fieldOffsetCfgFTPUserName);
		evolFieldList.push_back(fieldOffsetCfgFTPPassword);
		evolFieldList.push_back(fieldOffsetCfgFTPSubDir);
		// New update now fields below
		evolFieldList.push_back(fieldOffsetCfgFTPuseAutomatic);
		evolFieldList.push_back(fieldOffsetCfgFTPmonday);
		evolFieldList.push_back(fieldOffsetCfgFTPtuesday);
		evolFieldList.push_back(fieldOffsetCfgFTPwednesday);
		evolFieldList.push_back(fieldOffsetCfgFTPthursday);
		evolFieldList.push_back(fieldOffsetCfgFTPfriday);
		evolFieldList.push_back(fieldOffsetCfgFTPsaturday);
		evolFieldList.push_back(fieldOffsetCfgFTPsunday);
		evolFieldList.push_back(fieldOffsetCfgFTPstartTime);
		evolFieldList.push_back(fieldOffsetCfgFTPdownloadFirmwareOnly);
		evolFieldList.push_back(fieldOffsetCfgFTPdownloadInstallFirmware);
		evolFieldList.push_back(fieldOffsetCfgFTPdailyUpdates);
		evolFieldList.push_back(fieldOffsetCfgFTPhourlyUpdates);
		evolFieldList.push_back(fieldOffsetCfgFTP15MinuteUpdates);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdatePLUData);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateLabelData);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateFlashkeyData);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateOperatorData);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdatePlaylists);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateAlerts);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateScaleConfiguration);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateSynchronizationConfiguration);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateStoreNameNumber);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateVideos);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateImages);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateAudio);
		evolFieldList.push_back(fieldOffsetCfgFTPUpdateFirmware);
		evolFieldList.push_back(fieldOffsetCfgFTPSendPLUData);
		evolFieldList.push_back(fieldOffsetCfgFTPSendLabelData);
		evolFieldList.push_back(fieldOffsetCfgFTPSendFlashkeyData);
		evolFieldList.push_back(fieldOffsetCfgFTPSendOperatorData);
		evolFieldList.push_back(fieldOffsetCfgFTPSendPlaylists);
		evolFieldList.push_back(fieldOffsetCfgFTPSendAlerts);
		evolFieldList.push_back(fieldOffsetCfgFTPSendScaleConfiguration);
		evolFieldList.push_back(fieldOffsetCfgFTPSendSynchronizationConfiguration);
		evolFieldList.push_back(fieldOffsetCfgFTPSendStoreNameNumber);
		evolFieldList.push_back(fieldOffsetCfgFTPSendVideos);
		evolFieldList.push_back(fieldOffsetCfgFTPSendImages);
		evolFieldList.push_back(fieldOffsetCfgFTPSendAudio);
		evolFieldList.push_back(fieldOffsetCfgFTPSendFirmware);
		evolFieldList.push_back(fieldOffsetCfgFTPAllowOperatorSendToDept);
		evolFieldList.push_back(fieldOffsetCfgFTPAllowOperatorUpdateNow);

//  CfgCommInterface
		evolFieldList.push_back(fieldOffsetCfgCommProductClassTotalsClearTransactions);
		evolFieldList.push_back(fieldOffsetCfgCommHourlyTotalsClearTransactions);
		evolFieldList.push_back(fieldOffsetCfgCommOperatorTotalsClearTransactions);
		evolFieldList.push_back(fieldOffsetCfgCommCOOLTotalsClearTransactions);
		evolFieldList.push_back(fieldOffsetCfgCommOperatorChangesClearTransactions);

//  CfgCurrency
		evolFieldList.push_back(fieldOffsetCfgCurrencyDecimalSeparator);
		evolFieldList.push_back(fieldOffsetCfgCurrencyMonetaryRule);
		evolFieldList.push_back(fieldOffsetCfgCurrencyTotalPriceTotalDigits);
		evolFieldList.push_back(fieldOffsetCfgCurrencyUnitPriceTotalDigits);
		evolFieldList.push_back(fieldOffsetCfgCurrencyRoundingFactor);
		evolFieldList.push_back(fieldOffsetCfgCurrencyRoundingMethod);
		evolFieldList.push_back(fieldOffsetCfgCurrencyPrimarySymbol);
		evolFieldList.push_back(fieldOffsetCfgCurrencyPrimaryDecimalDigits);
		evolFieldList.push_back(fieldOffsetCfgCurrencySecondarySymbol);
		evolFieldList.push_back(fieldOffsetCfgCurrencySecondaryDecimalDigits);
//		evolFieldList.push_back(fieldOffsetCfgCurrencyUnits);
//		evolFieldList.push_back(fieldOffsetCfgMonetarySymbol);
//		evolFieldList.push_back(fieldOffsetCfgPricePrecision);

//  CfgGCD
		evolFieldList.push_back(fieldOffsetCfgCustomerDisplay);
		evolFieldList.push_back(fieldOffsetCfgGCDLayoutNumber);
		evolFieldList.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		evolFieldList.push_back(fieldOffsetCfgGCDAllowOperReset);
		evolFieldList.push_back(fieldOffsetCfgGCDVolume);
		evolFieldList.push_back(fieldOffsetCfgGCDPlaybackWaitTime);
		evolFieldList.push_back(fieldOffsetCfgGCDMediaServerName);
		evolFieldList.push_back(fieldOffsetCfgGCDMediaServerIP);
		evolFieldList.push_back(fieldOffsetCfgGCDFtpUserName);
		evolFieldList.push_back(fieldOffsetCfgGCDFtpPassword);
		evolFieldList.push_back(fieldOffsetCfgGCDFtpDirectory);
		evolFieldList.push_back(fieldOffsetCfgGCDPlaylistFile);
		evolFieldList.push_back(fieldOffsetCfgGCDAutoSearch);
		evolFieldList.push_back(fieldOffsetCfgGCDSearchHour);
		evolFieldList.push_back(fieldOffsetCfgGCDSearchMinute);
		evolFieldList.push_back(fieldOffsetCfgGCDSearchAmOrPM);
		evolFieldList.push_back(fieldOffsetCfgGCDIsDnsEnabled);
		evolFieldList.push_back(fieldOffsetCfgGCDDownloadProtocol);
		evolFieldList.push_back(fieldOffsetCfgGCDLanguage);
		evolFieldList.push_back(fieldOffsetCfgGCDUseTransactionalLayout);
		evolFieldList.push_back(fieldOffsetCfgGCDIdleAdvertType);
		evolFieldList.push_back(fieldOffsetCfgGCDIdleWebsiteAddress);
		evolFieldList.push_back(fieldOffsetCfgGCDProductAdvertType);

// CfgAccumulateItems
		evolFieldList.push_back(fieldOffsetCfgEnableAccumItems);
		evolFieldList.push_back(fieldOffsetCfgEnableItemStripMode);
		evolFieldList.push_back(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsReturnToStartScreen);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsEANFormatString);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsEANPrefix);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsEANCode);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsScaleId);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsTransactionNumber);
		evolFieldList.push_back(fieldOffsetCfgAccumItemsUPCProductNumber);
		evolFieldList.push_back(fieldOffsetCfgReadTransactionsCompatibility);
		evolFieldList.push_back(fieldOffsetCfgAccumBarcodeUseFirstItem);
		evolFieldList.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		evolFieldList.push_back(fieldOffsetCfgBarcodeServerAddress);

		evolFieldList.push_back(fieldOffsetCfgVirtualPrintingOverride2ndPrinter);
		evolFieldList.push_back(fieldOffsetCfgVirtualPrintingOverride3rdPrinter);

		evolFieldList.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		evolFieldList.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);

		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_1_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_1_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_1_750);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_2_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_2_375);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_2_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_3_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_3_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_4_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_4_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_5_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_5_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_6_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_6_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_7_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_7_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_8_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_8_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_9_000);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_9_500);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_10_00);
		evolFieldList.push_back(fieldOffsetCfgEnableLabelLength_Continuous);

		evolFieldList.push_back(fieldOffsetCfgContinuousStockIgnoreLabelTaken);
		evolFieldList.push_back(fieldOffsetCfgChoseToUseContinuousStock);

		evolFieldList.push_back(fieldOffsetCfgWeightRequiredForMeal);

		evolFieldList.push_back(fieldOffsetCfgSuggestiveSellStyle);
		evolFieldList.push_back(fieldOffsetCfgSuggestiveSellFrequency);

		evolFieldList.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);

		evolFieldList.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);

		evolFieldList.push_back(fieldOffsetCfgPeriodicOfflineBeep);

		evolFieldList.push_back(fieldOffsetCfgEnableGlobalPercentOff);
		evolFieldList.push_back(fieldOffsetCfgGlobalPercentageOff);

		evolFieldList.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);

		evolFieldList.push_back(fieldOffsetCfgRequireFixedWeightVerification);

		evolFieldList.push_back(fieldOffsetCfgAutoBackupDBFrequency);

		evolFieldList.push_back(fieldOffsetCfgFPCVersion);
		evolFieldList.push_back(fieldOffsetCfgFPCWeigherVersion);

		evolFieldList.push_back(fieldOffsetCfgScaleModel);

		evolFieldList.push_back(fieldOffsetCfgCutTestOptionEnabled);
		evolFieldList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOverall);
		evolFieldList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOperator);
		evolFieldList.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);
		evolFieldList.push_back(fieldOffsetCfgNextCutTestNumber);

		evolFieldList.push_back(fieldOffsetCfgBottomScreenContent);

		evolFieldList.push_back(fieldOffsetCfgEnableSearchMenuButton);
		
		evolFieldList.push_back(fieldOffsetCfgEnableHelpPrompts);
		evolFieldList.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		evolFieldList.push_back(fieldOffsetCfgUseSecondTare);
		evolFieldList.push_back(fieldOffsetCfgTrackingSupervisorChanges);
		evolFieldList.push_back(fieldOffsetCfgVariableShelfLifeTime);

		evolFieldList.push_back(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);
		evolFieldList.push_back(fieldOffsetCfgSelfServeByCountItemsAllowed);

		// Written out, but can't be brought into the scale; it will be ignored
		evolFieldList.push_back(fieldOffsetCfgPrinter1LabelLength);
		evolFieldList.push_back(fieldOffsetCfgPrinter2LabelLength);
		evolFieldList.push_back(fieldOffsetCfgPrinter3LabelLength);

		evolFieldList.push_back(fieldOffsetCfgDefaultSoftkeyFontPointSize);
		
		evolFieldList.push_back(fieldOffsetCfgDiagServiceContactInfo);
		evolFieldList.push_back(fieldOffsetCfgDiagGatewayDeviceAddress);
		evolFieldList.push_back(fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel);
		evolFieldList.push_back(fieldOffsetCfgValueMaxDiagCollectionDisabled);

		evolFieldList.push_back(fieldOffsetCfgSelfServePrintToScreen);
		evolFieldList.push_back(fieldOffsetCfgSelfServeDisplayTareOnRW);

		evolFieldList.push_back(fieldOffsetCfgVoidTransactionsViewHours);
		evolFieldList.push_back(fieldOffsetDoNotPrintNutrifacts);

		evolFieldList.push_back(fieldOffsetCfgDigimarcEnabled);
		evolFieldList.push_back(fieldOffsetCfgDigimarcDotDensity);
		evolFieldList.push_back(fieldOffsetCfgDigimarcDotSize);
		evolFieldList.push_back(fieldOffsetCfgDigimarcWatermarkRes);
		evolFieldList.push_back(fieldOffsetCfgDigimarcExpandedFreshEnabled);

		evolFieldList.push_back(fieldOffsetCfgReduceGS1Length);
		evolFieldList.push_back(fieldOffsetCfgDisableHooks);
		evolFieldList.push_back(fieldOffsetCfgCameraSystemTrustLevel);

		evolFieldList.push_back(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived);

		evolFieldList.push_back(fieldOffsetCfgUnitPricePrintMethod);

		evolFieldList.push_back(fieldOffsetCfgPriceChangedLabelIndicator);

		evolFieldList.push_back(fieldOffsetCfgSNMPTrapsIp);
		evolFieldList.push_back(fieldOffsetCfgSNMPTrapsPort);
		evolFieldList.push_back(fieldOffsetCfgSNMPVariablesPort);
		evolFieldList.push_back(fieldOffsetCfgSNMPEnabled);

		//SOF-5082 evolFieldList.push_back(fieldOffsetCfgIsCutterEnabled);
		evolFieldList.push_back(fieldOffsetCfgIsCodeCheckerEnabled);

		evolFieldList.push_back(fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel);

		evolFieldList.push_back(fieldOffsetCfgAllowSupervisorDotWear);
		evolFieldList.push_back(fieldOffsetCfgAllowOperatorDotWear);

		evolFieldList.push_back(fieldOffsetCfgSelfServeEnablePlaylists);
		evolFieldList.push_back(fieldOffsetCfgSelfServePlaylistFile);

		evolFieldList.push_back(fieldOffsetCfgEnableSearchOnKeypad);
		evolFieldList.push_back(fieldOffsetCfgEnableEnhancedCenterScreen);
		evolFieldList.push_back(fieldOffsetCfgEnhancedCenterHomePage);

		evolFieldList.push_back(fieldOffsetCfgAutoUpdateUPCBarcodeOver99);
		evolFieldList.push_back(fieldOffsetCfgBarcodeTypeOver99);
		// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
		//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
		//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
		evolFieldList.push_back(fieldOffsetCfgBarcodeWidthOver99);
		evolFieldList.push_back(fieldOffsetBarcodeOffsetFromLeftOver99);

		evolFieldList.push_back(fieldOffsetCfgVirtualCombinePrint);
		evolFieldList.push_back(fieldOffsetCfgIsConserveLinerLess);	//SOF-3875

		evolFieldList.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		evolFieldList.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		evolFieldList.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);
				
		evolFieldList.push_back(fieldOffsetCfgEnableUPTagPrefix);

		evolFieldList.push_back(fieldOffsetCfgMinSweetheartWeight);

		evolFieldList.push_back(fieldOffsetCfgOnlineOrderingEnabled);
		evolFieldList.push_back(fieldOffsetCfgOnlineOrderingServer);
		evolFieldList.push_back(fieldOffsetCfgOnlineOrderingZone);


		evolFieldList.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);
		evolFieldList.push_back(fieldOffsetCfgSelfServeStartState);
		evolFieldList.push_back(fieldOffsetCfgSelfServeItemEntryState);
		evolFieldList.push_back(fieldOffsetCfgSelfServeSearchState);
		evolFieldList.push_back(fieldOffsetCfgSelfServePrintState);
		evolFieldList.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		evolFieldList.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		evolFieldList.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		evolFieldList.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		evolFieldList.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);
		evolFieldList.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);								// PJR 420 - Flashkey Options to Toggle Extra Day

        // CfgCommScheduled
		evolFieldList.push_back(fieldOffsetCfgCommScheduledUseAutomaticMerges);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledMonday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledTuesday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledWednesday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledThursday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledFriday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledSaturday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledSunday);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledDailyUpdates);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledStartTime);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledHourlyUpdates);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledExecMergeAfterComm);
		evolFieldList.push_back(fieldOffsetCfgCommScheduledMergeHistory);

		evolFieldList.push_back(fieldOffsetCfgWrapperCameraEnabled);

		evolFieldList.push_back(fieldOffsetCfgSelfServeFixedPlu);
		evolFieldList.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		evolFieldList.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);

		evolFieldList.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		evolFieldList.push_back(fieldOffsetCfgBarcodeServerAddress);

		evolFieldList.push_back(fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag);

		evolFieldList.push_back(fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight);

		ConfigurationTags configurationTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCfgStoreId);
		ultimaFieldList.push_back(fieldOffsetCfgStoreName);
		ultimaFieldList.push_back(fieldOffsetCfgRwEanFormat);
		ultimaFieldList.push_back(fieldOffsetCfgFwEanFormat);
		ultimaFieldList.push_back(fieldOffsetCfgBcEanFormat);
		// ALV 
		ultimaFieldList.push_back(fieldOffsetCfgRwEan128cFormat);
		ultimaFieldList.push_back(fieldOffsetCfgFwEan128cFormat);
		ultimaFieldList.push_back(fieldOffsetCfgBcEan128cFormat);
		
		ultimaFieldList.push_back(fieldOffsetCfgRwEanPrefix);
		ultimaFieldList.push_back(fieldOffsetCfgFwEanPrefix);
		ultimaFieldList.push_back(fieldOffsetCfgBcEanPrefix);

		ConfigurationTags configurationTags((FieldOffsetsList *)&ultimaFieldList);
	}

	if (synchronizationList.empty())
	{
// CfgScale
		synchronizationList.push_back(fieldOffsetCfgStoreId);
		synchronizationList.push_back(fieldOffsetCfgStoreName);
		synchronizationList.push_back(fieldOffsetCfgStoreGraphic);
		synchronizationList.push_back(fieldOffsetCfgDeptName_NOT_USED);

		synchronizationList.push_back(fieldOffsetCfgPrimaryCountryCode);
		synchronizationList.push_back(fieldOffsetCfgPrimaryLanguageCode);
		synchronizationList.push_back(fieldOffsetCfgSecondaryCountryCode);
		synchronizationList.push_back(fieldOffsetCfgSecondaryLanguageCode);
		synchronizationList.push_back(fieldOffsetCfgLengthUnits);
		synchronizationList.push_back(fieldOffsetCfgTemperatureUnits);
		synchronizationList.push_back(fieldOffsetCfgDateFormat);
		synchronizationList.push_back(fieldOffsetCfgTimeFormat);

		synchronizationList.push_back(fieldOffsetCfgUnitPriceSecurityLabel);
		synchronizationList.push_back(fieldOffsetCfgTotalPriceSecurityLabel);
		synchronizationList.push_back(fieldOffsetCfgFractionalPricingEnabled);
		synchronizationList.push_back(fieldOffsetCfgPoundsForPricingEnabled);
		synchronizationList.push_back(fieldOffsetCfgBycountCalcMethod);
		synchronizationList.push_back(fieldOffsetCfgClearPluAfterPrinting);

		synchronizationList.push_back(fieldOffsetCfgBeepOnStableWeight);
		synchronizationList.push_back(fieldOffsetCfgBeeperVolumeLevel);

		synchronizationList.push_back(fieldOffsetCfgExternalWeightPrecision);

		synchronizationList.push_back(fieldOffsetCfgFixedWeightUnits);
		//fieldOffsetCfgWeigherAttached
		synchronizationList.push_back(fieldOffsetCfgTareSetback);
		synchronizationList.push_back(fieldOffsetCfgMaxTare);
		synchronizationList.push_back(fieldOffsetCfgPlatterTareEnabled);
		synchronizationList.push_back(fieldOffsetCfgChangeZeroTareOnlyOnce);
		synchronizationList.push_back(fieldOffsetCfgChangeForcedTareOnlyOnce);
		// synchronizationList.push_back(fieldOffsetCfgTareEntryPrecision);  as of April 2017, no longer configurable, per W&M
		synchronizationList.push_back(fieldOffsetCfgUserRezeroWeightLimit);
		synchronizationList.push_back(fieldOffsetCfgWeigherWakeupSensorAttached);

		synchronizationList.push_back(fieldOffsetCfgOperateLoginRequired);
		//fieldOffsetCfgSupervisorLoginRequired
		synchronizationList.push_back(fieldOffsetCfgInactivityAutoLogoutTime);
		synchronizationList.push_back(fieldOffsetCfgAutoLookupProduct);
		synchronizationList.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		synchronizationList.push_back(fieldOffsetCfgAutoLookupOverrideDigit);
		synchronizationList.push_back(fieldOffsetCfgAutoDisplayProductNotes);
		synchronizationList.push_back(fieldOffsetCfgAutoDisplaySuggestiveSelling);
		synchronizationList.push_back(fieldOffsetCfgAutoDisplayCookingNotes);

		synchronizationList.push_back(fieldOffsetCfgHorizLabelSkew1);
		synchronizationList.push_back(fieldOffsetCfgHorizLabelSkew2);
		synchronizationList.push_back(fieldOffsetCfgHorizLabelSkew3);

		synchronizationList.push_back(fieldOffsetCfgCollectingTransactions);
		synchronizationList.push_back(fieldOffsetCfgCollectingTransactionsDays);
		synchronizationList.push_back(fieldOffsetCfgCollectingProductTotals);
		synchronizationList.push_back(fieldOffsetCfgCollectingHourlyTotals);
		synchronizationList.push_back(fieldOffsetCfgCollectingOperatorTotals);
		synchronizationList.push_back(fieldOffsetCfgTrackingOperatorChanges);
//		synchronizationList.push_back(fieldOffsetCfgDaysToKeepOperatorChanges);
		synchronizationList.push_back(fieldOffsetCfgCollectingCOOLTotals);

		synchronizationList.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		synchronizationList.push_back(fieldOffsetCfgOpAccessToDesktopMode);

		synchronizationList.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		synchronizationList.push_back(fieldOffsetCfgAutoSearchNewFirmwareVersions);
		synchronizationList.push_back(fieldOffsetCfgNewFirmwareVersionSearchTime);
		synchronizationList.push_back(fieldOffsetCfgForceUniqueProdClassNums);
		synchronizationList.push_back(fieldOffsetCfgEnablePowerSaverMode);
		synchronizationList.push_back(fieldOffsetCfgPowerSaverModeTimeout);
		synchronizationList.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		synchronizationList.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		synchronizationList.push_back(fieldOffsetCfgDefaultProductRecordType);

		synchronizationList.push_back(fieldOffsetCfgDefWrFilmWidth);
		synchronizationList.push_back(fieldOffsetCfgDefWrFilmTension);
		//synchronizationList.push_back(fieldOffsetCfgDefWrFilmTensionFrontToBack);
		//synchronizationList.push_back(fieldOffsetCfgDefWrFilmTensionLeftToRight);
		synchronizationList.push_back(fieldOffsetCfgDefWrIntakeMode);
		synchronizationList.push_back(fieldOffsetCfgDefWrLabelPlacement);
		synchronizationList.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		synchronizationList.push_back(fieldOffsetCfgDefWrApplierRotation);
		synchronizationList.push_back(fieldOffsetCfgDefWrSecApplierRotation);

		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSun);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSun);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnMon);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffMon);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnTue);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffTue);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnWed);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffWed);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnThu);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffThu);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnFri);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffFri);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSat);
		synchronizationList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSat);

		synchronizationList.push_back(fieldOffsetCfgDbBackupFrequency);
		synchronizationList.push_back(fieldOffsetCfgAutoCompressDB);
		synchronizationList.push_back(fieldOffsetCfgCompressDBTime);

		synchronizationList.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);
		synchronizationList.push_back(fieldOffsetCfgDontChangeCurrentCOOL);

		synchronizationList.push_back(fieldOffsetCfgPrimaryExpirationPageNote);
		synchronizationList.push_back(fieldOffsetCfgSecondExpirationPageNote);

		synchronizationList.push_back(fieldOffsetCfgCheckNetworkConnectivity);
		//fieldOffsetCfgOfflineDateTime

		synchronizationList.push_back(fieldOffsetCfgScaleType);

		//fieldOffsetCfgCommunicationHistory
		//fieldOffsetCfgCommunicationItemReceived

		//fieldOffsetCfgPopupKeypadEnabled
		synchronizationList.push_back(fieldOffsetCfgPopupKeypadStyle);
		synchronizationList.push_back(fieldOffsetCfgPopupKeypadLocation);

		synchronizationList.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);

		synchronizationList.push_back(fieldOffsetCfgCapacityBarLocation);

		synchronizationList.push_back(fieldOffsetCfgPingPrimary);
		synchronizationList.push_back(fieldOffsetCfgPingGateway);
		synchronizationList.push_back(fieldOffsetCfgPingTarget);
		synchronizationList.push_back(fieldOffsetCfgPingInterval);
		synchronizationList.push_back(fieldOffsetCfgPingTargetIPAddress);

		synchronizationList.push_back(fieldOffsetCfgRebootUnprotectedAllowed);

		synchronizationList.push_back(fieldOffsetCfgAutoCreateRFIDOperator);

		synchronizationList.push_back(fieldOffsetCfgFirmwareVersion);			//	Serialize only
		synchronizationList.push_back(fieldOffsetCfgFirmwareRevision);		//	Serialize only

		synchronizationList.push_back(fieldOffsetCfgIgnoreUltimaFormatLabelTypes);

		synchronizationList.push_back(fieldOffsetCfgReportsOffsetTop);
		synchronizationList.push_back(fieldOffsetCfgReportsOffsetBottom);

		synchronizationList.push_back(fieldOffsetCfgShowDeleteAllFiles);
		synchronizationList.push_back(fieldOffsetCfgShowPrinterSettings);

		synchronizationList.push_back(fieldOffsetCfgRemoteAccessPermitted);
		synchronizationList.push_back(fieldOffsetCfgRemoteAccessLoginMethod);
		synchronizationList.push_back(fieldOffsetCfgRemoteAccessUsername);
		synchronizationList.push_back(fieldOffsetCfgRemoteAccessPassword);

		synchronizationList.push_back(fieldOffsetCfgBackdateByDays);

		synchronizationList.push_back(fieldOffsetCfgSystemFont);
		synchronizationList.push_back(fieldOffsetCfgStartScreenHome);
		synchronizationList.push_back(fieldOffsetCfgBrowserHasAddressBar);
		synchronizationList.push_back(fieldOffsetCfgDefaultSearchMethod);
		synchronizationList.push_back(fieldOffsetCfgResetStatisticsDays);

		synchronizationList.push_back(fieldOffsetCfgLeftRunscreenContent);
		synchronizationList.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		synchronizationList.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);
		synchronizationList.push_back(fieldOffsetCfgRightScreenContent);

		synchronizationList.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		synchronizationList.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		synchronizationList.push_back(fieldOffsetCfgDisableLeftRunscreen);

		synchronizationList.push_back(fieldOffsetCfgDisplayTransTypeIndicator);	//	FT-1466
		synchronizationList.push_back(fieldOffsetCfgNowServingQueueIPAddress);
		synchronizationList.push_back(fieldOffsetCfgNowServingSysType);

// CfgFrequentShopper
		synchronizationList.push_back(fieldOffsetCfgFSRoundingMethod);
		synchronizationList.push_back(fieldOffsetCfgFSDiscountMethod);
		synchronizationList.push_back(fieldOffsetCfgFSDiscountType);
		synchronizationList.push_back(fieldOffsetCfgFSDiscountFormula);
		synchronizationList.push_back(fieldOffsetCfgFSTotalPriceText1);
		synchronizationList.push_back(fieldOffsetCfgFSTotalPriceText2);
		synchronizationList.push_back(fieldOffsetCfgFSSavingsText1);
		synchronizationList.push_back(fieldOffsetCfgFSSavingsText2);
		synchronizationList.push_back(fieldOffsetCfgFSUnitPriceText1);
		synchronizationList.push_back(fieldOffsetCfgFSUnitPriceText2);
		synchronizationList.push_back(fieldOffsetCfgFSRoundingWeightPrecision);

// CfgOperatorAccess
		synchronizationList.push_back(fieldOffsetCfgOAPriceModifiable);
		synchronizationList.push_back(fieldOffsetCfgOAByCountModifiable);
		synchronizationList.push_back(fieldOffsetCfgOATareModifiable);
		synchronizationList.push_back(fieldOffsetCfgOAFixedWeightModifiable);
		synchronizationList.push_back(fieldOffsetCfgOAShelfLifeModifiable);
		synchronizationList.push_back(fieldOffsetCfgOAProductLifeModifiable);
		synchronizationList.push_back(fieldOffsetCfgOALabelTypeModifiable);
		synchronizationList.push_back(fieldOffsetCfgOADateTimeModifiable);
		synchronizationList.push_back(fieldOffsetCfgOAPortionsModifiable);

		synchronizationList.push_back(fieldOffsetCfgOAPriceAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOAByCountAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOATareAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOAFixedWeightAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOAShelfLifeAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOAProductLifeAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOALabelTypeAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOADateTimeAccessLevel);
		synchronizationList.push_back(fieldOffsetCfgOAPortionsAccessLevel);

// CfgPrintedFields
		synchronizationList.push_back(fieldOffsetCfgPFSellByDatePrinted);
		synchronizationList.push_back(fieldOffsetCfgPFSellByDateJulian);
		synchronizationList.push_back(fieldOffsetCfgPFPackedOnDatePrinted);
		synchronizationList.push_back(fieldOffsetCfgPFPackedOnDateJulian);
		synchronizationList.push_back(fieldOffsetCfgPFProductLifeDatePrinted);
		synchronizationList.push_back(fieldOffsetCfgPFProductLifeDateJulian);
		synchronizationList.push_back(fieldOffsetCfgPFUnitPricePrinted);
		synchronizationList.push_back(fieldOffsetCfgPFByCountPrinted);
		synchronizationList.push_back(fieldOffsetCfgPFBarCodePrinted);
		synchronizationList.push_back(fieldOffsetCfgPFCurrencySymbolPrinted);
		synchronizationList.push_back(fieldOffsetCfgPFTextNamesPreprinted);
		synchronizationList.push_back(fieldOffsetCfgPFWeightOnByCountPrinted);
		synchronizationList.push_back(fieldOffsetCfgPFOneOnByCountPrinted);
		synchronizationList.push_back(fieldOffsetCfgPFSecondCheckDigit);
		synchronizationList.push_back(fieldOffsetCfgPFPrintFirstLetterOfTotalType);
		synchronizationList.push_back(fieldOffsetCfgPFNetWtOnFixedWeightAlways);
		synchronizationList.push_back(fieldOffsetCfgPFFixedWeightFormat);

// CfgSelfService
		synchronizationList.push_back(fieldOffsetCfgGridXElements);
		synchronizationList.push_back(fieldOffsetCfgGridYElements);
		synchronizationList.push_back(fieldOffsetCfgNumOfMostCommonCategories);
		synchronizationList.push_back(fieldOffsetCfgImageTypeOnStartScreen);
		synchronizationList.push_back(fieldOffsetCfgInitialStreamingGraphicNum);
		synchronizationList.push_back(fieldOffsetCfgProductStatsHoursMethod);
		synchronizationList.push_back(fieldOffsetCfgSelfServeInactivityTimeout);
		synchronizationList.push_back(fieldOffsetCfgLogoGraphicFileName);
		synchronizationList.push_back(fieldOffsetCfgNoGraphicAvailableFileName1);
		synchronizationList.push_back(fieldOffsetCfgNoGraphicAvailableFileName2);
		synchronizationList.push_back(fieldOffsetCfgPreviousGraphicFileName1);
		synchronizationList.push_back(fieldOffsetCfgPreviousGraphicFileName2);
		synchronizationList.push_back(fieldOffsetCfgNextGraphicFileName1);
		synchronizationList.push_back(fieldOffsetCfgNextGraphicFileName2);

//  CfgEmail
		synchronizationList.push_back(fieldOffsetCfgFrom);
		synchronizationList.push_back(fieldOffsetCfgToList);
		synchronizationList.push_back(fieldOffsetCfgSmtpIpAddrOrDnsName);

//  CfgSynchronization - no fields
 
//  CfgPrepack
		synchronizationList.push_back(fieldOffsetCfgEnableSellBy);
		synchronizationList.push_back(fieldOffsetCfgEnableBestBefore);
		synchronizationList.push_back(fieldOffsetCfgEnablePackedOn);
		synchronizationList.push_back(fieldOffsetCfgEnableProdImg);
		synchronizationList.push_back(fieldOffsetCfgEnableUpcNumber);
		synchronizationList.push_back(fieldOffsetCfgEnableFilmWidth);
		synchronizationList.push_back(fieldOffsetCfgEnableFilmTension);
		synchronizationList.push_back(fieldOffsetCfgEnableWrapTypes);
		synchronizationList.push_back(fieldOffsetCfgEnableCountryOfOrigin);
		synchronizationList.push_back(fieldOffsetCfgEnableTrackingNumber);
		synchronizationList.push_back(fieldOffsetCfgEnableRunTotals);
		synchronizationList.push_back(fieldOffsetCfgEnableProductionEntry);
		synchronizationList.push_back(fieldOffsetCfgProductionEntryContinuousPrint);
		synchronizationList.push_back(fieldOffsetCfgEnableProductionPlanning);
		synchronizationList.push_back(fieldOffsetCfgProductionPlanningFrame);
		synchronizationList.push_back(fieldOffsetCfgProductionPlanningUpdate);
		synchronizationList.push_back(fieldOffsetCfgEnableSelfServe);
		synchronizationList.push_back(fieldOffsetCfgPluSearchRange);
		synchronizationList.push_back(fieldOffsetCfgAppendingPluNumberToName);
		synchronizationList.push_back(fieldOffsetCfgNumOfDescCharsToUse);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideSetting1);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideValue1);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideSetting2);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideValue2);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideSetting3);
		synchronizationList.push_back(fieldOffsetCfgRotationOverrideValue3);
		synchronizationList.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		synchronizationList.push_back(fieldOffsetCfgSecondaryLabelStockRotated);

//  CfgMarquee
		synchronizationList.push_back(fieldOffsetCfgMqPermanentMarqueeNumber);
		//synchronizationList.push_back(fieldOffsetCfgMqPermanentMarqueeDisplayed);
		synchronizationList.push_back(fieldOffsetCfgMqProductMarqueeDisplayed);
		synchronizationList.push_back(fieldOffsetCfgMqProductDescMarqueeCounter);

//  CfgFTP
		synchronizationList.push_back(fieldOffsetCfgFTPServerIporHostName);
		synchronizationList.push_back(fieldOffsetCfgFTPUserName);
		synchronizationList.push_back(fieldOffsetCfgFTPPassword);
		synchronizationList.push_back(fieldOffsetCfgFTPSubDir);
		// New Update Now fields below
		synchronizationList.push_back(fieldOffsetCfgFTPstartTime);
		synchronizationList.push_back(fieldOffsetCfgFTPuseAutomatic);
		synchronizationList.push_back(fieldOffsetCfgFTPmonday);
		synchronizationList.push_back(fieldOffsetCfgFTPtuesday);
		synchronizationList.push_back(fieldOffsetCfgFTPwednesday);
		synchronizationList.push_back(fieldOffsetCfgFTPthursday);
		synchronizationList.push_back(fieldOffsetCfgFTPfriday);
		synchronizationList.push_back(fieldOffsetCfgFTPsaturday);
		synchronizationList.push_back(fieldOffsetCfgFTPsunday);
		synchronizationList.push_back(fieldOffsetCfgFTPdownloadFirmwareOnly);
		synchronizationList.push_back(fieldOffsetCfgFTPdownloadInstallFirmware);
		synchronizationList.push_back(fieldOffsetCfgFTPdailyUpdates);
		synchronizationList.push_back(fieldOffsetCfgFTPhourlyUpdates);
		synchronizationList.push_back(fieldOffsetCfgFTP15MinuteUpdates);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdatePLUData);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateLabelData);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateFlashkeyData);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateOperatorData);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdatePlaylists);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateAlerts);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateScaleConfiguration);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateSynchronizationConfiguration);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateStoreNameNumber);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateVideos);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateImages);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateAudio);
		synchronizationList.push_back(fieldOffsetCfgFTPUpdateFirmware);
		synchronizationList.push_back(fieldOffsetCfgFTPSendPLUData);
		synchronizationList.push_back(fieldOffsetCfgFTPSendLabelData);
		synchronizationList.push_back(fieldOffsetCfgFTPSendFlashkeyData);
		synchronizationList.push_back(fieldOffsetCfgFTPSendOperatorData);
		synchronizationList.push_back(fieldOffsetCfgFTPSendPlaylists);
		synchronizationList.push_back(fieldOffsetCfgFTPSendAlerts);
		synchronizationList.push_back(fieldOffsetCfgFTPSendScaleConfiguration);
		synchronizationList.push_back(fieldOffsetCfgFTPSendSynchronizationConfiguration);
		synchronizationList.push_back(fieldOffsetCfgFTPSendStoreNameNumber);
		synchronizationList.push_back(fieldOffsetCfgFTPSendVideos);
		synchronizationList.push_back(fieldOffsetCfgFTPSendImages);
		synchronizationList.push_back(fieldOffsetCfgFTPSendAudio);
		synchronizationList.push_back(fieldOffsetCfgFTPSendFirmware);
		synchronizationList.push_back(fieldOffsetCfgFTPAllowOperatorSendToDept);
		synchronizationList.push_back(fieldOffsetCfgFTPAllowOperatorUpdateNow);

//  CfgCommInterface
		synchronizationList.push_back(fieldOffsetCfgCommProductClassTotalsClearTransactions);
		synchronizationList.push_back(fieldOffsetCfgCommHourlyTotalsClearTransactions);
		synchronizationList.push_back(fieldOffsetCfgCommOperatorTotalsClearTransactions);
		synchronizationList.push_back(fieldOffsetCfgCommCOOLTotalsClearTransactions);
		synchronizationList.push_back(fieldOffsetCfgCommOperatorChangesClearTransactions);

//  CfgCurrency
		synchronizationList.push_back(fieldOffsetCfgCurrencyDecimalSeparator);
		synchronizationList.push_back(fieldOffsetCfgCurrencyMonetaryRule);
		synchronizationList.push_back(fieldOffsetCfgCurrencyTotalPriceTotalDigits);
		synchronizationList.push_back(fieldOffsetCfgCurrencyUnitPriceTotalDigits);
		synchronizationList.push_back(fieldOffsetCfgCurrencyRoundingFactor);
		synchronizationList.push_back(fieldOffsetCfgCurrencyRoundingMethod);
		synchronizationList.push_back(fieldOffsetCfgCurrencyPrimarySymbol);
		synchronizationList.push_back(fieldOffsetCfgCurrencyPrimaryDecimalDigits);
		synchronizationList.push_back(fieldOffsetCfgCurrencySecondarySymbol);
		synchronizationList.push_back(fieldOffsetCfgCurrencySecondaryDecimalDigits);
//		synchronizationList.push_back(fieldOffsetCfgCurrencyUnits);
//		synchronizationList.push_back(fieldOffsetCfgMonetarySymbol);
//		synchronizationList.push_back(fieldOffsetCfgPricePrecision);

//  CfgGCD
		synchronizationList.push_back(fieldOffsetCfgCustomerDisplay);
		synchronizationList.push_back(fieldOffsetCfgGCDLayoutNumber);
		synchronizationList.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		synchronizationList.push_back(fieldOffsetCfgGCDAllowOperReset);
		synchronizationList.push_back(fieldOffsetCfgGCDVolume);
		synchronizationList.push_back(fieldOffsetCfgGCDPlaybackWaitTime);
		synchronizationList.push_back(fieldOffsetCfgGCDMediaServerName);
		synchronizationList.push_back(fieldOffsetCfgGCDMediaServerIP);
		synchronizationList.push_back(fieldOffsetCfgGCDFtpUserName);
		synchronizationList.push_back(fieldOffsetCfgGCDFtpPassword);
		synchronizationList.push_back(fieldOffsetCfgGCDFtpDirectory);
		synchronizationList.push_back(fieldOffsetCfgGCDPlaylistFile);
		synchronizationList.push_back(fieldOffsetCfgGCDAutoSearch);
		synchronizationList.push_back(fieldOffsetCfgGCDSearchHour);
		synchronizationList.push_back(fieldOffsetCfgGCDSearchMinute);
		synchronizationList.push_back(fieldOffsetCfgGCDSearchAmOrPM);
		synchronizationList.push_back(fieldOffsetCfgGCDIsDnsEnabled);
		synchronizationList.push_back(fieldOffsetCfgGCDDownloadProtocol);
		synchronizationList.push_back(fieldOffsetCfgGCDLanguage);
		synchronizationList.push_back(fieldOffsetCfgGCDUseTransactionalLayout);
		synchronizationList.push_back(fieldOffsetCfgGCDIdleAdvertType);
		synchronizationList.push_back(fieldOffsetCfgGCDIdleWebsiteAddress);
		synchronizationList.push_back(fieldOffsetCfgGCDProductAdvertType);

// CfgAccumulateItems
		synchronizationList.push_back(fieldOffsetCfgEnableAccumItems);
		synchronizationList.push_back(fieldOffsetCfgEnableItemStripMode);
		synchronizationList.push_back(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsReturnToStartScreen);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsEANFormatString);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsEANPrefix);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsEANCode);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsScaleId);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsTransactionNumber);
		synchronizationList.push_back(fieldOffsetCfgAccumItemsUPCProductNumber);
		synchronizationList.push_back(fieldOffsetCfgReadTransactionsCompatibility);
		synchronizationList.push_back(fieldOffsetCfgAccumBarcodeUseFirstItem);

		synchronizationList.push_back(fieldOffsetCfgSyncConfigurations);
		synchronizationList.push_back(fieldOffsetCfgSyncEmailAlerts);
		synchronizationList.push_back(fieldOffsetCfgSyncProductDefaults);
		synchronizationList.push_back(fieldOffsetCfgSyncClasses);
		synchronizationList.push_back(fieldOffsetCfgSyncExpandedTexts);

		synchronizationList.push_back(fieldOffsetCfgSyncSpecialMessages);
		synchronizationList.push_back(fieldOffsetCfgSyncNutritionFacts);
		synchronizationList.push_back(fieldOffsetCfgSyncProductNotes);
		synchronizationList.push_back(fieldOffsetCfgSyncMarqueeTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncTransactions);

		synchronizationList.push_back(fieldOffsetCfgSyncTransactionTypes);
		synchronizationList.push_back(fieldOffsetCfgSyncCategories);
		synchronizationList.push_back(fieldOffsetCfgSyncSpecials);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolCategories);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolCountries);

		synchronizationList.push_back(fieldOffsetCfgSyncCoolProductionSteps);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolPreTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolListTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncCoolLists);

		synchronizationList.push_back(fieldOffsetCfgSyncCategoryCountries);
		synchronizationList.push_back(fieldOffsetCfgSyncCategoryProductionSteps);
		synchronizationList.push_back(fieldOffsetCfgSyncCategoryPreTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncProducts);
		synchronizationList.push_back(fieldOffsetCfgSyncExpirationLabels);

		synchronizationList.push_back(fieldOffsetCfgSyncLabels);
		synchronizationList.push_back(fieldOffsetCfgSyncLabelTexts);
		synchronizationList.push_back(fieldOffsetCfgSyncOperatorNotes);
		synchronizationList.push_back(fieldOffsetCfgSyncOperators);
		synchronizationList.push_back(fieldOffsetCfgSyncSoftkeyGroups);

		synchronizationList.push_back(fieldOffsetCfgSyncSoftkeyStyles);
		synchronizationList.push_back(fieldOffsetCfgSyncSoftkeyFeatures);
		synchronizationList.push_back(fieldOffsetCfgSyncSoftkeys);
		synchronizationList.push_back(fieldOffsetCfgSyncLabelGraphics);
		synchronizationList.push_back(fieldOffsetCfgSyncCategoryGraphics);

		synchronizationList.push_back(fieldOffsetCfgSyncProductGraphics);
		synchronizationList.push_back(fieldOffsetCfgSyncSoftkeyGraphics);
		synchronizationList.push_back(fieldOffsetCfgSyncSpecialsGraphics);

		synchronizationList.push_back(fieldOffsetCfgVirtualPrintingOverride2ndPrinter);
		synchronizationList.push_back(fieldOffsetCfgVirtualPrintingOverride3rdPrinter);

		synchronizationList.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		synchronizationList.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);

		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_1_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_1_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_1_750);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_2_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_2_375);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_2_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_3_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_3_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_4_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_4_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_5_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_5_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_6_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_6_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_7_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_7_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_8_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_8_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_9_000);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_9_500);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_10_00);
		synchronizationList.push_back(fieldOffsetCfgEnableLabelLength_Continuous);

		synchronizationList.push_back(fieldOffsetCfgContinuousStockIgnoreLabelTaken);
		synchronizationList.push_back(fieldOffsetCfgChoseToUseContinuousStock);

		synchronizationList.push_back(fieldOffsetCfgWeightRequiredForMeal);

		synchronizationList.push_back(fieldOffsetCfgSuggestiveSellStyle);
		synchronizationList.push_back(fieldOffsetCfgSuggestiveSellFrequency);

		synchronizationList.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);

		synchronizationList.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);

		synchronizationList.push_back(fieldOffsetCfgPeriodicOfflineBeep);

		synchronizationList.push_back(fieldOffsetCfgEnableGlobalPercentOff);
		synchronizationList.push_back(fieldOffsetCfgGlobalPercentageOff);

		synchronizationList.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);

		synchronizationList.push_back(fieldOffsetCfgRequireFixedWeightVerification);

		synchronizationList.push_back(fieldOffsetCfgAutoBackupDBFrequency);

		synchronizationList.push_back(fieldOffsetCfgCutTestOptionEnabled);
		synchronizationList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOverall);
		synchronizationList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOperator);
		synchronizationList.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);
		synchronizationList.push_back(fieldOffsetCfgNextCutTestNumber);

		synchronizationList.push_back(fieldOffsetCfgBottomScreenContent);

		synchronizationList.push_back(fieldOffsetCfgEnableSearchMenuButton);

        synchronizationList.push_back(fieldOffsetCfgEnableHelpPrompts);
		synchronizationList.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		synchronizationList.push_back(fieldOffsetCfgUseSecondTare);
		synchronizationList.push_back(fieldOffsetCfgTrackingSupervisorChanges);
		synchronizationList.push_back(fieldOffsetCfgVariableShelfLifeTime);

		synchronizationList.push_back(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);
		synchronizationList.push_back(fieldOffsetCfgSelfServeByCountItemsAllowed);

		synchronizationList.push_back(fieldOffsetCfgPrinter1LabelLength);
		synchronizationList.push_back(fieldOffsetCfgPrinter2LabelLength);
		synchronizationList.push_back(fieldOffsetCfgPrinter3LabelLength);

		synchronizationList.push_back(fieldOffsetCfgDefaultSoftkeyFontPointSize);
		
		synchronizationList.push_back(fieldOffsetCfgDiagServiceContactInfo);
		synchronizationList.push_back(fieldOffsetCfgDiagGatewayDeviceAddress);
		synchronizationList.push_back(fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel);
		synchronizationList.push_back(fieldOffsetCfgValueMaxDiagCollectionDisabled);

		synchronizationList.push_back(fieldOffsetCfgSelfServePrintToScreen);
        synchronizationList.push_back(fieldOffsetCfgSelfServeDisplayTareOnRW);

		synchronizationList.push_back(fieldOffsetCfgVoidTransactionsViewHours);
		
		synchronizationList.push_back(fieldOffsetDoNotPrintNutrifacts);

		synchronizationList.push_back(fieldOffsetCfgDigimarcEnabled);
		synchronizationList.push_back(fieldOffsetCfgDigimarcDotDensity);
		synchronizationList.push_back(fieldOffsetCfgDigimarcDotSize);
		synchronizationList.push_back(fieldOffsetCfgDigimarcWatermarkRes);
		synchronizationList.push_back(fieldOffsetCfgDigimarcExpandedFreshEnabled);

		synchronizationList.push_back(fieldOffsetCfgReduceGS1Length);
		synchronizationList.push_back(fieldOffsetCfgDisableHooks);

		synchronizationList.push_back(fieldOffsetCfgCameraSystemTrustLevel);

		synchronizationList.push_back(fieldOffsetCfgUnitPricePrintMethod);

		synchronizationList.push_back(fieldOffsetCfgPriceChangedLabelIndicator);

		//SOF-5082 synchronizationList.push_back(fieldOffsetCfgIsCutterEnabled);
		synchronizationList.push_back(fieldOffsetCfgIsCodeCheckerEnabled);

		synchronizationList.push_back(fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel);

		synchronizationList.push_back(fieldOffsetCfgAllowSupervisorDotWear);
		synchronizationList.push_back(fieldOffsetCfgAllowOperatorDotWear);

		synchronizationList.push_back(fieldOffsetCfgSelfServeEnablePlaylists);
		synchronizationList.push_back(fieldOffsetCfgSelfServePlaylistFile);

		synchronizationList.push_back(fieldOffsetCfgEnableSearchOnKeypad);
		synchronizationList.push_back(fieldOffsetCfgEnableEnhancedCenterScreen);
		synchronizationList.push_back(fieldOffsetCfgEnhancedCenterHomePage);

		synchronizationList.push_back(fieldOffsetCfgAutoUpdateUPCBarcodeOver99);
		synchronizationList.push_back(fieldOffsetCfgBarcodeTypeOver99);
		// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
		//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
		//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
		synchronizationList.push_back(fieldOffsetCfgBarcodeWidthOver99);
		synchronizationList.push_back(fieldOffsetBarcodeOffsetFromLeftOver99);

		synchronizationList.push_back(fieldOffsetCfgVirtualCombinePrint);
		synchronizationList.push_back(fieldOffsetCfgIsConserveLinerLess);	//SOF-3875
		synchronizationList.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		synchronizationList.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		synchronizationList.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);
		
		synchronizationList.push_back(fieldOffsetCfgEnableUPTagPrefix);

		synchronizationList.push_back(fieldOffsetCfgMinSweetheartWeight);

		synchronizationList.push_back(fieldOffsetCfgOnlineOrderingEnabled);
		synchronizationList.push_back(fieldOffsetCfgOnlineOrderingServer);
		synchronizationList.push_back(fieldOffsetCfgOnlineOrderingZone);

		synchronizationList.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);
		synchronizationList.push_back(fieldOffsetCfgSelfServeStartState);
		synchronizationList.push_back(fieldOffsetCfgSelfServeItemEntryState);
		synchronizationList.push_back(fieldOffsetCfgSelfServeSearchState);
		synchronizationList.push_back(fieldOffsetCfgSelfServePrintState);
		synchronizationList.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		synchronizationList.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		synchronizationList.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		synchronizationList.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		synchronizationList.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);
		synchronizationList.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);							// PJR 420 - Flashkey Options to Toggle Extra Day

        // CfgCommScheduled
		synchronizationList.push_back(fieldOffsetCfgCommScheduledUseAutomaticMerges);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledMonday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledTuesday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledWednesday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledThursday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledFriday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledSaturday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledSunday);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledDailyUpdates);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledStartTime);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledHourlyUpdates);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledExecMergeAfterComm);
		synchronizationList.push_back(fieldOffsetCfgCommScheduledMergeHistory);

		synchronizationList.push_back(fieldOffsetCfgWrapperCameraEnabled);

		synchronizationList.push_back(fieldOffsetCfgSelfServeFixedPlu);
		synchronizationList.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyType4);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		synchronizationList.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);

		synchronizationList.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		synchronizationList.push_back(fieldOffsetCfgBarcodeServerAddress);

		synchronizationList.push_back(fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag);

		synchronizationList.push_back(fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight);

		ConfigurationTags configurationTags((FieldOffsetsList *)&synchronizationList);
	}

	if (dbIndependentList.empty())
	{
// CfgScale
		dbIndependentList.push_back(fieldOffsetCfgStoreId);
		dbIndependentList.push_back(fieldOffsetCfgStoreName);
		dbIndependentList.push_back(fieldOffsetCfgStoreGraphic);
		dbIndependentList.push_back(fieldOffsetCfgDeptName_NOT_USED);

		dbIndependentList.push_back(fieldOffsetCfgPrimaryCountryCode);
		dbIndependentList.push_back(fieldOffsetCfgPrimaryLanguageCode);
		dbIndependentList.push_back(fieldOffsetCfgSecondaryCountryCode);
		dbIndependentList.push_back(fieldOffsetCfgSecondaryLanguageCode);
		dbIndependentList.push_back(fieldOffsetCfgLengthUnits);
		dbIndependentList.push_back(fieldOffsetCfgTemperatureUnits);
		dbIndependentList.push_back(fieldOffsetCfgDateFormat);
		dbIndependentList.push_back(fieldOffsetCfgTimeFormat);

		dbIndependentList.push_back(fieldOffsetCfgUnitPriceSecurityLabel);
		dbIndependentList.push_back(fieldOffsetCfgTotalPriceSecurityLabel);
		dbIndependentList.push_back(fieldOffsetCfgFractionalPricingEnabled);
		dbIndependentList.push_back(fieldOffsetCfgPoundsForPricingEnabled);
		dbIndependentList.push_back(fieldOffsetCfgBycountCalcMethod);
		dbIndependentList.push_back(fieldOffsetCfgClearPluAfterPrinting);

		dbIndependentList.push_back(fieldOffsetCfgBeepOnStableWeight);
		dbIndependentList.push_back(fieldOffsetCfgBeeperVolumeLevel);

		dbIndependentList.push_back(fieldOffsetCfgExternalWeightPrecision);

		dbIndependentList.push_back(fieldOffsetCfgFixedWeightUnits);
		dbIndependentList.push_back(fieldOffsetCfgWeigherAttached);
		dbIndependentList.push_back(fieldOffsetCfgTareSetback);
		dbIndependentList.push_back(fieldOffsetCfgMaxTare);
		dbIndependentList.push_back(fieldOffsetCfgPlatterTareEnabled);
		dbIndependentList.push_back(fieldOffsetCfgChangeZeroTareOnlyOnce);
		dbIndependentList.push_back(fieldOffsetCfgChangeForcedTareOnlyOnce);
		// dbIndependentList.push_back(fieldOffsetCfgTareEntryPrecision);  as of April 2017, no longer configurable, per W&M
		dbIndependentList.push_back(fieldOffsetCfgUserRezeroWeightLimit);
		dbIndependentList.push_back(fieldOffsetCfgWeigherWakeupSensorAttached);

		dbIndependentList.push_back(fieldOffsetCfgOperateLoginRequired);
		//fieldOffsetCfgSupervisorLoginRequired
		dbIndependentList.push_back(fieldOffsetCfgInactivityAutoLogoutTime);
		dbIndependentList.push_back(fieldOffsetCfgAutoLookupProduct);
		dbIndependentList.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		dbIndependentList.push_back(fieldOffsetCfgAutoLookupOverrideDigit);
		dbIndependentList.push_back(fieldOffsetCfgAutoDisplayProductNotes);
		dbIndependentList.push_back(fieldOffsetCfgAutoDisplaySuggestiveSelling);
		dbIndependentList.push_back(fieldOffsetCfgAutoDisplayCookingNotes);

		dbIndependentList.push_back(fieldOffsetCfgHorizLabelSkew1);
		dbIndependentList.push_back(fieldOffsetCfgHorizLabelSkew2);
		dbIndependentList.push_back(fieldOffsetCfgHorizLabelSkew3);

		dbIndependentList.push_back(fieldOffsetCfgCollectingTransactions);
		dbIndependentList.push_back(fieldOffsetCfgCollectingTransactionsDays);
		dbIndependentList.push_back(fieldOffsetCfgCollectingProductTotals);
		dbIndependentList.push_back(fieldOffsetCfgCollectingHourlyTotals);
		dbIndependentList.push_back(fieldOffsetCfgCollectingOperatorTotals);
		dbIndependentList.push_back(fieldOffsetCfgTrackingOperatorChanges);
//		dbIndependentList.push_back(fieldOffsetCfgDaysToKeepOperatorChanges);
		dbIndependentList.push_back(fieldOffsetCfgCollectingCOOLTotals);

		dbIndependentList.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		dbIndependentList.push_back(fieldOffsetCfgOpAccessToDesktopMode);

		dbIndependentList.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		dbIndependentList.push_back(fieldOffsetCfgAutoSearchNewFirmwareVersions);
		dbIndependentList.push_back(fieldOffsetCfgNewFirmwareVersionSearchTime);
		dbIndependentList.push_back(fieldOffsetCfgForceUniqueProdClassNums);
		dbIndependentList.push_back(fieldOffsetCfgEnablePowerSaverMode);
		dbIndependentList.push_back(fieldOffsetCfgPowerSaverModeTimeout);
		dbIndependentList.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		dbIndependentList.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		dbIndependentList.push_back(fieldOffsetCfgDefaultProductRecordType);

		dbIndependentList.push_back(fieldOffsetCfgDefWrFilmWidth);
		dbIndependentList.push_back(fieldOffsetCfgDefWrFilmTension);
		dbIndependentList.push_back(fieldOffsetCfgDefWrFilmTensionFrontToBack);
		dbIndependentList.push_back(fieldOffsetCfgDefWrFilmTensionLeftToRight);
		dbIndependentList.push_back(fieldOffsetCfgDefWrIntakeMode);
		dbIndependentList.push_back(fieldOffsetCfgDefWrLabelPlacement);
		dbIndependentList.push_back(fieldOffsetCfgDefWrApplierRotation);
		dbIndependentList.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		dbIndependentList.push_back(fieldOffsetCfgDefWrSecApplierRotation);

		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSun);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSun);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnMon);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffMon);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnTue);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffTue);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnWed);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffWed);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnThu);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffThu);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnFri);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffFri);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOnSat);
		dbIndependentList.push_back(fieldOffsetCfgWrSealerBeltTimeOffSat);

		dbIndependentList.push_back(fieldOffsetCfgDbBackupFrequency);
		dbIndependentList.push_back(fieldOffsetCfgAutoCompressDB);
		dbIndependentList.push_back(fieldOffsetCfgCompressDBTime);

		dbIndependentList.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);
		dbIndependentList.push_back(fieldOffsetCfgDontChangeCurrentCOOL);

		dbIndependentList.push_back(fieldOffsetCfgPrimaryExpirationPageNote);
		dbIndependentList.push_back(fieldOffsetCfgSecondExpirationPageNote);

		dbIndependentList.push_back(fieldOffsetCfgCheckNetworkConnectivity);
		dbIndependentList.push_back(fieldOffsetCfgOfflineDateTime);

		dbIndependentList.push_back(fieldOffsetCfgScaleType);

		dbIndependentList.push_back(fieldOffsetCfgCommunicationHistory);
		dbIndependentList.push_back(fieldOffsetCfgCommunicationItemReceived);

		dbIndependentList.push_back(fieldOffsetCfgPopupKeypadEnabled);
		dbIndependentList.push_back(fieldOffsetCfgPopupKeypadStyle);
		dbIndependentList.push_back(fieldOffsetCfgPopupKeypadLocation);

		dbIndependentList.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);

		dbIndependentList.push_back(fieldOffsetCfgCapacityBarLocation);

		dbIndependentList.push_back(fieldOffsetCfgPingPrimary);
		dbIndependentList.push_back(fieldOffsetCfgPingGateway);
		dbIndependentList.push_back(fieldOffsetCfgPingTarget);
		dbIndependentList.push_back(fieldOffsetCfgPingInterval);
		dbIndependentList.push_back(fieldOffsetCfgPingTargetIPAddress);

		dbIndependentList.push_back(fieldOffsetCfgRebootUnprotectedAllowed);

		dbIndependentList.push_back(fieldOffsetCfgAutoCreateRFIDOperator);

		dbIndependentList.push_back(fieldOffsetCfgFirmwareVersion);			//	Serialize only
		dbIndependentList.push_back(fieldOffsetCfgFirmwareRevision);		//	Serialize only

		dbIndependentList.push_back(fieldOffsetCfgIgnoreUltimaFormatLabelTypes);

		dbIndependentList.push_back(fieldOffsetCfgReportsOffsetTop);
		dbIndependentList.push_back(fieldOffsetCfgReportsOffsetBottom);

		dbIndependentList.push_back(fieldOffsetCfgShowDeleteAllFiles);
		dbIndependentList.push_back(fieldOffsetCfgShowPrinterSettings);

		dbIndependentList.push_back(fieldOffsetCfgRemoteAccessPermitted);
		dbIndependentList.push_back(fieldOffsetCfgRemoteAccessLoginMethod);
		dbIndependentList.push_back(fieldOffsetCfgRemoteAccessUsername);
		dbIndependentList.push_back(fieldOffsetCfgRemoteAccessPassword);

		dbIndependentList.push_back(fieldOffsetCfgBackdateByDays);

		dbIndependentList.push_back(fieldOffsetCfgSystemFont);
		dbIndependentList.push_back(fieldOffsetCfgStartScreenHome);
		dbIndependentList.push_back(fieldOffsetCfgBrowserHasAddressBar);
		dbIndependentList.push_back(fieldOffsetCfgDefaultSearchMethod);
		dbIndependentList.push_back(fieldOffsetCfgResetStatisticsDays);

		dbIndependentList.push_back(fieldOffsetCfgLeftRunscreenContent);
		dbIndependentList.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		dbIndependentList.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);
		dbIndependentList.push_back(fieldOffsetCfgRightScreenContent);

		dbIndependentList.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		dbIndependentList.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		dbIndependentList.push_back(fieldOffsetCfgDisableLeftRunscreen);

		dbIndependentList.push_back(fieldOffsetCfgMostRecentRunMode);

		dbIndependentList.push_back(fieldOffsetCfgDisplayTransTypeIndicator);	//	FT-1466
		dbIndependentList.push_back(fieldOffsetCfgNowServingQueueIPAddress);
		dbIndependentList.push_back(fieldOffsetCfgNowServingSysType);

// CfgFrequentShopper
		dbIndependentList.push_back(fieldOffsetCfgFSRoundingMethod);
		dbIndependentList.push_back(fieldOffsetCfgFSDiscountMethod);
		dbIndependentList.push_back(fieldOffsetCfgFSDiscountType);
		dbIndependentList.push_back(fieldOffsetCfgFSDiscountFormula);
		dbIndependentList.push_back(fieldOffsetCfgFSTotalPriceText1);
		dbIndependentList.push_back(fieldOffsetCfgFSTotalPriceText2);
		dbIndependentList.push_back(fieldOffsetCfgFSSavingsText1);
		dbIndependentList.push_back(fieldOffsetCfgFSSavingsText2);
		dbIndependentList.push_back(fieldOffsetCfgFSUnitPriceText1);
		dbIndependentList.push_back(fieldOffsetCfgFSUnitPriceText2);
		dbIndependentList.push_back(fieldOffsetCfgFSRoundingWeightPrecision);

// CfgOperatorAccess
		dbIndependentList.push_back(fieldOffsetCfgOAPriceModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOAByCountModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOATareModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOAFixedWeightModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOAShelfLifeModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOAProductLifeModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOALabelTypeModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOADateTimeModifiable);
		dbIndependentList.push_back(fieldOffsetCfgOAPortionsModifiable);

		dbIndependentList.push_back(fieldOffsetCfgOAPriceAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOAByCountAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOATareAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOAFixedWeightAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOAShelfLifeAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOAProductLifeAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOALabelTypeAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOADateTimeAccessLevel);
		dbIndependentList.push_back(fieldOffsetCfgOAPortionsAccessLevel);

// CfgPrintedFields
		dbIndependentList.push_back(fieldOffsetCfgPFSellByDatePrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFSellByDateJulian);
		dbIndependentList.push_back(fieldOffsetCfgPFPackedOnDatePrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFPackedOnDateJulian);
		dbIndependentList.push_back(fieldOffsetCfgPFProductLifeDatePrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFProductLifeDateJulian);
		dbIndependentList.push_back(fieldOffsetCfgPFUnitPricePrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFByCountPrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFBarCodePrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFCurrencySymbolPrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFTextNamesPreprinted);
		dbIndependentList.push_back(fieldOffsetCfgPFWeightOnByCountPrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFOneOnByCountPrinted);
		dbIndependentList.push_back(fieldOffsetCfgPFSecondCheckDigit);
		dbIndependentList.push_back(fieldOffsetCfgPFPrintFirstLetterOfTotalType);
		dbIndependentList.push_back(fieldOffsetCfgPFNetWtOnFixedWeightAlways);
		dbIndependentList.push_back(fieldOffsetCfgPFFixedWeightFormat);

// CfgSelfService
		dbIndependentList.push_back(fieldOffsetCfgGridXElements);
		dbIndependentList.push_back(fieldOffsetCfgGridYElements);
		dbIndependentList.push_back(fieldOffsetCfgNumOfMostCommonCategories);
		dbIndependentList.push_back(fieldOffsetCfgImageTypeOnStartScreen);
		dbIndependentList.push_back(fieldOffsetCfgInitialStreamingGraphicNum);
		dbIndependentList.push_back(fieldOffsetCfgProductStatsHoursMethod);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeInactivityTimeout);
		dbIndependentList.push_back(fieldOffsetCfgLogoGraphicFileName);
		dbIndependentList.push_back(fieldOffsetCfgNoGraphicAvailableFileName1);
		dbIndependentList.push_back(fieldOffsetCfgNoGraphicAvailableFileName2);
		dbIndependentList.push_back(fieldOffsetCfgPreviousGraphicFileName1);
		dbIndependentList.push_back(fieldOffsetCfgPreviousGraphicFileName2);
		dbIndependentList.push_back(fieldOffsetCfgNextGraphicFileName1);
		dbIndependentList.push_back(fieldOffsetCfgNextGraphicFileName2);

//  CfgEmail
		dbIndependentList.push_back(fieldOffsetCfgFrom);
		dbIndependentList.push_back(fieldOffsetCfgToList);
		dbIndependentList.push_back(fieldOffsetCfgSmtpIpAddrOrDnsName);

//  CfgPrepack
		dbIndependentList.push_back(fieldOffsetCfgEnableSellBy);
		dbIndependentList.push_back(fieldOffsetCfgEnableBestBefore);
		dbIndependentList.push_back(fieldOffsetCfgEnablePackedOn);
		dbIndependentList.push_back(fieldOffsetCfgEnableProdImg);
		dbIndependentList.push_back(fieldOffsetCfgEnableUpcNumber);
		dbIndependentList.push_back(fieldOffsetCfgEnableFilmWidth);
		dbIndependentList.push_back(fieldOffsetCfgEnableFilmTension);
		dbIndependentList.push_back(fieldOffsetCfgEnableWrapTypes);
		dbIndependentList.push_back(fieldOffsetCfgEnableCountryOfOrigin);
		dbIndependentList.push_back(fieldOffsetCfgEnableTrackingNumber);
		dbIndependentList.push_back(fieldOffsetCfgEnableRunTotals);
		dbIndependentList.push_back(fieldOffsetCfgEnableProductionEntry);
		dbIndependentList.push_back(fieldOffsetCfgProductionEntryContinuousPrint);
		dbIndependentList.push_back(fieldOffsetCfgEnableProductionPlanning);
		dbIndependentList.push_back(fieldOffsetCfgProductionPlanningFrame);
		dbIndependentList.push_back(fieldOffsetCfgProductionPlanningUpdate);
		dbIndependentList.push_back(fieldOffsetCfgEnableSelfServe);
		dbIndependentList.push_back(fieldOffsetCfgPluSearchRange);
		dbIndependentList.push_back(fieldOffsetCfgAppendingPluNumberToName);
		dbIndependentList.push_back(fieldOffsetCfgNumOfDescCharsToUse);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideSetting1);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideValue1);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideSetting2);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideValue2);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideSetting3);
		dbIndependentList.push_back(fieldOffsetCfgRotationOverrideValue3);
		dbIndependentList.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		dbIndependentList.push_back(fieldOffsetCfgSecondaryLabelStockRotated);

//  CfgMarquee
		dbIndependentList.push_back(fieldOffsetCfgMqPermanentMarqueeNumber);
		//dbIndependentList.push_back(fieldOffsetCfgMqPermanentMarqueeDisplayed);
		dbIndependentList.push_back(fieldOffsetCfgMqProductMarqueeDisplayed);
		dbIndependentList.push_back(fieldOffsetCfgMqProductDescMarqueeCounter);

//  CfgFTP
		dbIndependentList.push_back(fieldOffsetCfgFTPServerIporHostName);
		dbIndependentList.push_back(fieldOffsetCfgFTPUserName);
		dbIndependentList.push_back(fieldOffsetCfgFTPPassword);
		dbIndependentList.push_back(fieldOffsetCfgFTPSubDir);
		//UPDATE NOW additions below

		dbIndependentList.push_back(fieldOffsetCfgFTPuseAutomatic);
		dbIndependentList.push_back(fieldOffsetCfgFTPmonday);
		dbIndependentList.push_back(fieldOffsetCfgFTPtuesday);
		dbIndependentList.push_back(fieldOffsetCfgFTPwednesday);
		dbIndependentList.push_back(fieldOffsetCfgFTPthursday);
		dbIndependentList.push_back(fieldOffsetCfgFTPfriday);
		dbIndependentList.push_back(fieldOffsetCfgFTPsaturday);
		dbIndependentList.push_back(fieldOffsetCfgFTPsunday);
		dbIndependentList.push_back(fieldOffsetCfgFTPstartTime);
		dbIndependentList.push_back(fieldOffsetCfgFTPdownloadFirmwareOnly);
		dbIndependentList.push_back(fieldOffsetCfgFTPdownloadInstallFirmware);
		dbIndependentList.push_back(fieldOffsetCfgFTPdailyUpdates);
		dbIndependentList.push_back(fieldOffsetCfgFTPhourlyUpdates);
		dbIndependentList.push_back(fieldOffsetCfgFTP15MinuteUpdates);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdatePLUData);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateLabelData);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateFlashkeyData);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateOperatorData);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdatePlaylists);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateAlerts);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateScaleConfiguration);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateSynchronizationConfiguration);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateStoreNameNumber);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateVideos);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateImages);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateAudio);
		dbIndependentList.push_back(fieldOffsetCfgFTPUpdateFirmware);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendPLUData);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendLabelData);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendFlashkeyData);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendOperatorData);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendPlaylists);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendAlerts);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendScaleConfiguration);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendSynchronizationConfiguration);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendStoreNameNumber);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendVideos);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendImages);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendAudio);
		dbIndependentList.push_back(fieldOffsetCfgFTPSendFirmware);
		dbIndependentList.push_back(fieldOffsetCfgFTPAllowOperatorSendToDept);
		dbIndependentList.push_back(fieldOffsetCfgFTPAllowOperatorUpdateNow);

//  CfgCommInterface
		dbIndependentList.push_back(fieldOffsetCfgCommProductClassTotalsClearTransactions);
		dbIndependentList.push_back(fieldOffsetCfgCommHourlyTotalsClearTransactions);
		dbIndependentList.push_back(fieldOffsetCfgCommOperatorTotalsClearTransactions);
		dbIndependentList.push_back(fieldOffsetCfgCommCOOLTotalsClearTransactions);
		dbIndependentList.push_back(fieldOffsetCfgCommOperatorChangesClearTransactions);

//  CfgCurrency
		dbIndependentList.push_back(fieldOffsetCfgCurrencyDecimalSeparator);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyMonetaryRule);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyTotalPriceTotalDigits);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyUnitPriceTotalDigits);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyRoundingFactor);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyRoundingMethod);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyPrimarySymbol);
		dbIndependentList.push_back(fieldOffsetCfgCurrencyPrimaryDecimalDigits);
		dbIndependentList.push_back(fieldOffsetCfgCurrencySecondarySymbol);
		dbIndependentList.push_back(fieldOffsetCfgCurrencySecondaryDecimalDigits);
//		dbIndependentList.push_back(fieldOffsetCfgCurrencyUnits);
//		dbIndependentList.push_back(fieldOffsetCfgMonetarySymbol);
//		dbIndependentList.push_back(fieldOffsetCfgPricePrecision);

//  CfgGCD
		dbIndependentList.push_back(fieldOffsetCfgCustomerDisplay);
		dbIndependentList.push_back(fieldOffsetCfgGCDLayoutNumber);
		dbIndependentList.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		dbIndependentList.push_back(fieldOffsetCfgGCDAllowOperReset);
		dbIndependentList.push_back(fieldOffsetCfgGCDVolume);
		dbIndependentList.push_back(fieldOffsetCfgGCDPlaybackWaitTime);
		dbIndependentList.push_back(fieldOffsetCfgGCDMediaServerName);
		dbIndependentList.push_back(fieldOffsetCfgGCDMediaServerIP);
		dbIndependentList.push_back(fieldOffsetCfgGCDFtpUserName);
		dbIndependentList.push_back(fieldOffsetCfgGCDFtpPassword);
		dbIndependentList.push_back(fieldOffsetCfgGCDFtpDirectory);
		dbIndependentList.push_back(fieldOffsetCfgGCDPlaylistFile);
		dbIndependentList.push_back(fieldOffsetCfgGCDAutoSearch);
		dbIndependentList.push_back(fieldOffsetCfgGCDSearchHour);
		dbIndependentList.push_back(fieldOffsetCfgGCDSearchMinute);
		dbIndependentList.push_back(fieldOffsetCfgGCDSearchAmOrPM);
		dbIndependentList.push_back(fieldOffsetCfgGCDIsDnsEnabled);
		dbIndependentList.push_back(fieldOffsetCfgGCDDownloadProtocol);
		dbIndependentList.push_back(fieldOffsetCfgGCDLanguage);
		dbIndependentList.push_back(fieldOffsetCfgGCDPrintWithoutCustomerDisplay);
		dbIndependentList.push_back(fieldOffsetCfgGCDUseTransactionalLayout);
		dbIndependentList.push_back(fieldOffsetCfgGCDIdleAdvertType);
		dbIndependentList.push_back(fieldOffsetCfgGCDIdleWebsiteAddress);
		dbIndependentList.push_back(fieldOffsetCfgGCDProductAdvertType);

// CfgAccumulateItems
		dbIndependentList.push_back(fieldOffsetCfgEnableAccumItems);
		dbIndependentList.push_back(fieldOffsetCfgEnableItemStripMode);
		dbIndependentList.push_back(fieldOffsetCfgEnableAccumItemsTotalsOnly);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsReturnToStartScreen);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsEANFormatString);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsEANPrefix);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsEANCode);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsScaleId);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsTransactionNumber);
		dbIndependentList.push_back(fieldOffsetCfgAccumItemsUPCProductNumber);
		dbIndependentList.push_back(fieldOffsetCfgReadTransactionsCompatibility);
		dbIndependentList.push_back(fieldOffsetCfgAccumBarcodeUseFirstItem);

//  CfgSynchronization
		dbIndependentList.push_back(fieldOffsetCfgScaleSynchronizationType);
		dbIndependentList.push_back(fieldOffsetCfgPrimaryScale);
		dbIndependentList.push_back(fieldOffsetCfgSyncConfigurations);
		dbIndependentList.push_back(fieldOffsetCfgSyncEmailAlerts);
		dbIndependentList.push_back(fieldOffsetCfgSyncProductDefaults);
		dbIndependentList.push_back(fieldOffsetCfgSyncClasses);
		dbIndependentList.push_back(fieldOffsetCfgSyncExpandedTexts);

		dbIndependentList.push_back(fieldOffsetCfgSyncSpecialMessages);
		dbIndependentList.push_back(fieldOffsetCfgSyncNutritionFacts);
		dbIndependentList.push_back(fieldOffsetCfgSyncProductNotes);
		dbIndependentList.push_back(fieldOffsetCfgSyncMarqueeTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncTransactions);

		dbIndependentList.push_back(fieldOffsetCfgSyncTransactionTypes);
		dbIndependentList.push_back(fieldOffsetCfgSyncCategories);
		dbIndependentList.push_back(fieldOffsetCfgSyncSpecials);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolCategories);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolCountries);

		dbIndependentList.push_back(fieldOffsetCfgSyncCoolProductionSteps);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolPreTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolListTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncCoolLists);

		dbIndependentList.push_back(fieldOffsetCfgSyncCategoryCountries);
		dbIndependentList.push_back(fieldOffsetCfgSyncCategoryProductionSteps);
		dbIndependentList.push_back(fieldOffsetCfgSyncCategoryPreTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncProducts);
		dbIndependentList.push_back(fieldOffsetCfgSyncExpirationLabels);

		dbIndependentList.push_back(fieldOffsetCfgSyncLabels);
		dbIndependentList.push_back(fieldOffsetCfgSyncLabelTexts);
		dbIndependentList.push_back(fieldOffsetCfgSyncOperatorNotes);
		dbIndependentList.push_back(fieldOffsetCfgSyncOperators);
		dbIndependentList.push_back(fieldOffsetCfgSyncSoftkeyGroups);

		dbIndependentList.push_back(fieldOffsetCfgSyncSoftkeyStyles);
		dbIndependentList.push_back(fieldOffsetCfgSyncSoftkeyFeatures);
		dbIndependentList.push_back(fieldOffsetCfgSyncSoftkeys);
		dbIndependentList.push_back(fieldOffsetCfgSyncLabelGraphics);
		dbIndependentList.push_back(fieldOffsetCfgSyncCategoryGraphics);

		dbIndependentList.push_back(fieldOffsetCfgSyncProductGraphics);
		dbIndependentList.push_back(fieldOffsetCfgSyncSoftkeyGraphics);
		dbIndependentList.push_back(fieldOffsetCfgSyncSpecialsGraphics);

		dbIndependentList.push_back(fieldOffsetCfgVirtualPrintingOverride2ndPrinter);
		dbIndependentList.push_back(fieldOffsetCfgVirtualPrintingOverride3rdPrinter);

		dbIndependentList.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		dbIndependentList.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);

		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_1_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_1_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_1_750);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_2_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_2_375);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_2_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_3_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_3_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_4_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_4_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_5_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_5_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_6_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_6_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_7_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_7_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_8_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_8_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_9_000);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_9_500);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_10_00);
		dbIndependentList.push_back(fieldOffsetCfgEnableLabelLength_Continuous);

		dbIndependentList.push_back(fieldOffsetCfgContinuousStockIgnoreLabelTaken);
		dbIndependentList.push_back(fieldOffsetCfgChoseToUseContinuousStock);

		dbIndependentList.push_back(fieldOffsetCfgWeightRequiredForMeal);

		dbIndependentList.push_back(fieldOffsetCfgSuggestiveSellStyle);
		dbIndependentList.push_back(fieldOffsetCfgSuggestiveSellFrequency);

		dbIndependentList.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);

		dbIndependentList.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);

		dbIndependentList.push_back(fieldOffsetCfgPeriodicOfflineBeep);

		dbIndependentList.push_back(fieldOffsetCfgEnableGlobalPercentOff);
		dbIndependentList.push_back(fieldOffsetCfgGlobalPercentageOff);

		dbIndependentList.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);

		dbIndependentList.push_back(fieldOffsetCfgRequireFixedWeightVerification);

		dbIndependentList.push_back(fieldOffsetCfgAutoBackupDBFrequency);

		dbIndependentList.push_back(fieldOffsetCfgFPCVersion);
		dbIndependentList.push_back(fieldOffsetCfgFPCWeigherVersion);

		dbIndependentList.push_back(fieldOffsetCfgCutTestOptionEnabled);
		dbIndependentList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOverall);
		dbIndependentList.push_back(fieldOffsetCfgMaxSuspendedCutTestsOperator);
		dbIndependentList.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);
		dbIndependentList.push_back(fieldOffsetCfgNextCutTestNumber);

		dbIndependentList.push_back(fieldOffsetCfgBottomScreenContent);

		dbIndependentList.push_back(fieldOffsetCfgEnableSearchMenuButton);

        dbIndependentList.push_back(fieldOffsetCfgEnableHelpPrompts);
		dbIndependentList.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		dbIndependentList.push_back(fieldOffsetCfgUseSecondTare);
		dbIndependentList.push_back(fieldOffsetCfgTrackingSupervisorChanges);
		dbIndependentList.push_back(fieldOffsetCfgVariableShelfLifeTime);

		dbIndependentList.push_back(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeByCountItemsAllowed);

		dbIndependentList.push_back(fieldOffsetCfgPrinter1LabelLength);
		dbIndependentList.push_back(fieldOffsetCfgPrinter2LabelLength);
		dbIndependentList.push_back(fieldOffsetCfgPrinter3LabelLength);

		dbIndependentList.push_back(fieldOffsetCfgDefaultSoftkeyFontPointSize);

		dbIndependentList.push_back(fieldOffsetCfgDiagServiceContactInfo);
		dbIndependentList.push_back(fieldOffsetCfgDiagGatewayDeviceAddress);
		dbIndependentList.push_back(fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel);
		dbIndependentList.push_back(fieldOffsetCfgValueMaxDiagCollectionDisabled);

		dbIndependentList.push_back(fieldOffsetCfgSelfServePrintToScreen);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeDisplayTareOnRW);

		dbIndependentList.push_back(fieldOffsetCfgVoidTransactionsViewHours);
		dbIndependentList.push_back(fieldOffsetDoNotPrintNutrifacts);

		dbIndependentList.push_back(fieldOffsetCfgMostRecentActiveDeptNum);
		dbIndependentList.push_back(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived);

		dbIndependentList.push_back(fieldOffsetCfgDigimarcEnabled);
		dbIndependentList.push_back(fieldOffsetCfgDigimarcDotDensity);
		dbIndependentList.push_back(fieldOffsetCfgDigimarcDotSize);
		dbIndependentList.push_back(fieldOffsetCfgDigimarcWatermarkRes);
		dbIndependentList.push_back(fieldOffsetCfgDigimarcExpandedFreshEnabled);

		dbIndependentList.push_back(fieldOffsetCfgReduceGS1Length);
		dbIndependentList.push_back(fieldOffsetCfgDisableHooks);

		dbIndependentList.push_back(fieldOffsetCfgCameraSystemTrustLevel);

		dbIndependentList.push_back(fieldOffsetCfgUnitPricePrintMethod);

		dbIndependentList.push_back(fieldOffsetCfgPriceChangedLabelIndicator);
		
		dbIndependentList.push_back(fieldOffsetCfgIsCutterEnabled);
		dbIndependentList.push_back(fieldOffsetCfgIsCodeCheckerEnabled);

		dbIndependentList.push_back(fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel);

		dbIndependentList.push_back(fieldOffsetCfgAllowSupervisorDotWear);
		dbIndependentList.push_back(fieldOffsetCfgAllowOperatorDotWear);

		dbIndependentList.push_back(fieldOffsetCfgSelfServeEnablePlaylists);
		dbIndependentList.push_back(fieldOffsetCfgSelfServePlaylistFile);

		dbIndependentList.push_back(fieldOffsetCfgEnableSearchOnKeypad);
		dbIndependentList.push_back(fieldOffsetCfgEnableEnhancedCenterScreen);
		dbIndependentList.push_back(fieldOffsetCfgEnhancedCenterHomePage);

		dbIndependentList.push_back(fieldOffsetCfgAutoUpdateUPCBarcodeOver99);
		dbIndependentList.push_back(fieldOffsetCfgBarcodeTypeOver99);
		// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
		//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
		//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
		dbIndependentList.push_back(fieldOffsetCfgBarcodeWidthOver99);
		dbIndependentList.push_back(fieldOffsetBarcodeOffsetFromLeftOver99);

		dbIndependentList.push_back(fieldOffsetCfgVirtualCombinePrint);
		dbIndependentList.push_back(fieldOffsetCfgIsConserveLinerLess);	//SOF-3875
		
		dbIndependentList.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);
		
		dbIndependentList.push_back(fieldOffsetCfgEnableUPTagPrefix);

		dbIndependentList.push_back(fieldOffsetCfgMinSweetheartWeight);

		dbIndependentList.push_back(fieldOffsetCfgOnlineOrderingEnabled);
		dbIndependentList.push_back(fieldOffsetCfgOnlineOrderingServer);
		dbIndependentList.push_back(fieldOffsetCfgOnlineOrderingZone);

		dbIndependentList.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeStartState);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeItemEntryState);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeSearchState);
		dbIndependentList.push_back(fieldOffsetCfgSelfServePrintState);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		dbIndependentList.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);
		dbIndependentList.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);								// PJR 420 - Flashkey Options to Toggle Extra Day

        // CfgCommScheduled
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledUseAutomaticMerges);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledMonday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledTuesday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledWednesday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledThursday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledFriday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledSaturday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledSunday);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledDailyUpdates);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledStartTime);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledHourlyUpdates);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledExecMergeAfterComm);
		dbIndependentList.push_back(fieldOffsetCfgCommScheduledMergeHistory);
		
        dbIndependentList.push_back(fieldOffsetCfgWrapperCameraEnabled);

		dbIndependentList.push_back(fieldOffsetCfgSelfServeFixedPlu);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyType4);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		dbIndependentList.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);

		dbIndependentList.push_back(fieldOffsetCfgReadBarcodeFromCloud);
		dbIndependentList.push_back(fieldOffsetCfgBarcodeServerAddress);

		dbIndependentList.push_back(fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag);

		dbIndependentList.push_back(fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight);

		ConfigurationTags configurationTags((FieldOffsetsList *)&dbIndependentList);
	}

}


void CfgAll::updateConfiguration()
{
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	switch (receivedFormat_)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			ConfigurationTags configTags;

			HobString evolData(receivedData_);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";
			int offset = 0;

			HobString model = "";
			cfgScale.getFieldAsString(fieldOffsetCfgIScaleName, model, BrowserFormat);
			
			bool model_FS = false;
			if (model.substr(0, 2) == "FS")
				model_FS = true;
			
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? configTags.getOffset(temp) : 0;

					// We no longer backup, nor restore, these 4 config fields.  They are set in Service in the Weigher Configuration option.
					// We have to ignore them here in case they were backed up with a previous version of the software.
					// Must not ignore fieldOffsetCfgWeigherModel, fieldOffsetCfgMinWeightToPrintLabel, and fieldOffsetCfgWeighMode to support EZ-USB weigher. (haberrc)

#if 1 // haberrc
					if (offset == fieldOffsetCfgSelfServeEnableFreshServe)
					{
						if (model_FS)
							config->updateField(offset, temp);

					}
					else if (offset      && offset !=  fieldOffsetCfgNetWeightDualRangeLimitCheck
									// and we also ignore these two fields except when we are restoring a database during an upgrade (that were backed up with a previous version of the software),
									&& ( offset != fieldOffsetCfgScaleSynchronizationType || receivedFormat_ == DBIndependentFormatFormat )
									&& ( offset != fieldOffsetCfgPrimaryScale || receivedFormat_ == DBIndependentFormatFormat )
									&& ( offset != fieldOffsetCfgHorizLabelSkew1 || receivedFormat_ == DBIndependentFormatFormat )
									&& ( offset != fieldOffsetCfgHorizLabelSkew2 || receivedFormat_ == DBIndependentFormatFormat )
									&& ( offset != fieldOffsetCfgHorizLabelSkew3 || receivedFormat_ == DBIndependentFormatFormat )
						// and we also don't restore these two fields (but they are backed up),
									&& ( offset != fieldOffsetCfgCommunicationHistory )
									&& offset != fieldOffsetCfgCommunicationItemReceived)
#else // original
					if (offset && offset != fieldOffsetCfgWeigherModel && offset != fieldOffsetCfgMinWeightToPrintLabel &&
										offset != fieldOffsetCfgWeighMode && offset !=  fieldOffsetCfgNetWeightDualRangeLimitCheck
									// and we also ignore these two fields (that were backed up with a previous version of the software),
									&& offset != fieldOffsetCfgScaleSynchronizationType && offset != fieldOffsetCfgPrimaryScale
									// and we also don't restore these two fields (but they are backed up),
									&&	offset != fieldOffsetCfgCommunicationHistory && offset != fieldOffsetCfgCommunicationItemReceived)
#endif
					{
						setReceived(offset, true);
						temp = token->getData();

						if ( offset < endCfgScaleFieldOffsets )
							config->updateField(offset, temp);
						else if (offset < endCfgFSFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgOAFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgPFFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgSSFieldOffsets)
							cfgSelfService.updateField(offset, temp);
						else if (offset < endCfgNetworkFieldOffsets)
							cfgNetwork.updateField(offset, temp);
						else if (offset < endCfgEmailFieldOffsets)
							cfgEmail.updateField(offset, temp);
						else if (offset < endCfgSynchronizationFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgSyncListFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgPrepackFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgMarqueeFieldOffsets)
							cfgMarquee.updateField(offset, temp);
						else if (offset < endCfgFTPFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgCommFieldOffsets)
							cfgCommInterface.updateField(offset, temp);
						else if (offset < endCfgCurrencyFieldOffsets)
							cfgCurrency.updateField(offset, temp);
						else if (offset < endCfgGCDFieldOffsets)
							config->updateField(offset, temp);
						else if (offset < endCfgAccumulateItemsFieldOffsets)
							config->updateField(offset, temp);

						else if ((offset == fieldOffsetCfgRwEanFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setRwEanFormat(temp);
						else if ((offset == fieldOffsetCfgFwEanFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setFwEanFormat(temp);
						else if ((offset == fieldOffsetCfgBcEanFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setBcEanFormat(temp);

						// ALV
						else if ((offset == fieldOffsetCfgRwEan128cFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setRwEan128cFormat(temp);
						else if ((offset == fieldOffsetCfgFwEan128cFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setFwEan128cFormat(temp);
						else if ((offset == fieldOffsetCfgBcEan128cFormat) && (receivedFormat_ == UltimaTaggedFormat))
							setBcEan128cFormat(temp);

						else if ((offset == fieldOffsetCfgRwEanPrefix) && (receivedFormat_ == UltimaTaggedFormat))
							setRwEanPrefix(temp);
						else if ((offset == fieldOffsetCfgFwEanPrefix) && (receivedFormat_ == UltimaTaggedFormat))
							setFwEanPrefix(temp);
						else if ((offset == fieldOffsetCfgBcEanPrefix) && (receivedFormat_ == UltimaTaggedFormat))
							setBcEanPrefix(temp);

						else if (offset < endCfgFieldOffsets)
							// Last ditch effort to update a config field.  This is here because we can't just insert new fields into their
							// "most appropriate" config set, because this can screw up updating any field based on its specific offset.
							config->updateField(offset, temp);
					}
				}
				delete token;
			}
			break;
		}

		default:
			break;
	}
}


void CfgAll::save(DBAccess *dbAccess)
{
	Debug::trace("Configuration.CfgAll", HobString("save()"));

	Configuration::getInstance()->save(dbAccess);

	cfgSelfService.save(dbAccess);
	cfgNetwork.save(dbAccess);
	cfgEmail.save(dbAccess);
	cfgMarquee.save(dbAccess);
	cfgCommInterface.save(dbAccess);
	cfgCurrency.save(dbAccess);
}


const char *CfgAll::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Debug::trace("Configuration.CfgAll", HobString("serialize():  format = ") + HobString(Integer::toString(format)));

	switch (format)
	{
		case EvolutionTaggedFormat:
			serializeUsingList(evolFieldList, EvolutionTaggedFormat);
			break;

		case DBIndependentFormatFormat:
			serializeUsingList(dbIndependentList, DBIndependentFormatFormat);
			break;

		case SynchronizationTaggedFormat:
			serializeUsingList(synchronizationList, SynchronizationTaggedFormat);
			break;

		case UltimaTaggedFormat:
			serializeUsingList(ultimaFieldList, UltimaTaggedFormat);
			break;

		case PrinterReportFormat:
		{
			// let's start with store/scale info
			serializedData.assign(cfgScale.serialize(StoreScaleInfoReportFormat)).append(2, '\n');

			// locale settings
			serializedData.append(cfgScale.serialize(LocaleReportFormat)).append(1, '\n');
			serializedData.append(cfgCurrency.serialize(LocaleReportFormat)).append(2, '\n');

			// normal scale operation
			serializedData.append(cfgScale.serialize(NormalScaleOperationReportFormat))/*.append(2, '\n')*/;

			// self service settings
			serializedData.append(cfgSelfService.serialize(SelfServiceReportFormat)).append(2, '\n');

			// operator operation
//			serializedData.append(cfgOperatorAccess.serialize(OperatorModifiesReportFormat)).append(2, '\n');

			// fields to print operation
			serializedData.append(cfgPrintedFields.serialize(FieldsToPrintReportFormat)).append(2, '\n');

			// Frequent Shopper operation
			serializedData.append(cfgFrequentShopper.serialize(FrequentShopperReportFormat)).append(2, '\n');

			// network settings
			serializedData.append(cfgNetwork.serialize(NetworkReportFormat));
			serializedData.append(cfgScale.serialize(BackgroundReportFormat)).append(1, '\n');
//			serializedData.append(cfgNetwork.serialize(WirelessNetworkReportFormat)).append(2, '\n');
			serializedData.append(cfgEmail.serialize(EmailReportFormat)).append(2, '\n');
			serializedData.append(cfgFTP.serialize(FTPReportFormat)).append(2, '\n');
			serializedData.append(cfgCommInterface.serialize(CommInterfaceReportFormat)).append(2, '\n');

			// weigher settings
			serializedData.append(cfgScale.serialize(WeigherReportFormat));   //.append(2, '\n');

			// wrapper settings
//			serializedData.append(cfgScale.serialize(WrapperReportFormat));

			break;
		}

		default:
			serializedData = HobString("CfgAll::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void CfgAll::serializeUsingList(const list<CfgFieldOffsets> fieldOffsetList, int format) const
{
	ConfigurationTags configTags;
	HobString tag = "";
	HobString fieldData = "";
			
	Configuration *config = Configuration::getInstance();

	list<CfgFieldOffsets>::const_iterator fieldIter = fieldOffsetList.begin();
	while (fieldIter != fieldOffsetList.end())
	{
		//	We will accept these fields by turning on Transactions, and so they are in the field offset lists,
		// but we will not write them back out, as we no longer have these fields in CfgScale [CCD MAY 2011].
		
		if (*fieldIter != fieldOffsetCfgCollectingProductTotals && *fieldIter != fieldOffsetCfgCollectingHourlyTotals &&
		    *fieldIter != fieldOffsetCfgCollectingOperatorTotals && *fieldIter != fieldOffsetCfgCollectingCOOLTotals &&
			// don't backup the flag indicating that the user chose to use continuous stock, so that it doesn't get propogated to another scale that may be using die-cut labels
			!( *fieldIter == fieldOffsetCfgChoseToUseContinuousStock && format == EvolutionTaggedFormat) &&
			// SOF-1072: don't backup the flags to force department import
			!( *fieldIter == fieldOffsetCfgMostRecentActiveDeptNum && format == EvolutionTaggedFormat) &&
			!( *fieldIter == fieldOffsetCfgUseCurrentDeptnumWhenNotReceived && format == EvolutionTaggedFormat) )
		{
			tag = configTags.getTag(*fieldIter);
			if (tag.size())
			{
				fieldData = "";

				if (*fieldIter < endCfgScaleFieldOffsets)
					cfgScale.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgFSFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgOAFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgPFFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgSSFieldOffsets)
					cfgSelfService.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgNetworkFieldOffsets)
					cfgNetwork.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgEmailFieldOffsets)
					cfgEmail.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgSynchronizationFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgSyncListFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgPrepackFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgMarqueeFieldOffsets)
					cfgMarquee.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgFTPFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter < endCfgCommFieldOffsets)
					cfgCommInterface.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgCurrencyFieldOffsets)
					cfgCurrency.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgGCDFieldOffsets)
					cfgGCD.getFieldAsString(*fieldIter, fieldData, format);
				else if (*fieldIter < endCfgAccumulateItemsFieldOffsets)
					fieldData = config->toStringCfg(*fieldIter);
				else if (*fieldIter == fieldOffsetCfgRwEanFormat)
					fieldData = getRwEanFormat();
				else if (*fieldIter == fieldOffsetCfgFwEanFormat)
					fieldData = getFwEanFormat();
				else if (*fieldIter == fieldOffsetCfgBcEanFormat)
					fieldData = getBcEanFormat();
					// ALV
				else if (*fieldIter == fieldOffsetCfgRwEan128cFormat)
					fieldData = getRwEan128cFormat();
				else if (*fieldIter == fieldOffsetCfgFwEan128cFormat)
					fieldData = getFwEan128cFormat();
				else if (*fieldIter == fieldOffsetCfgBcEan128cFormat)
					fieldData = getBcEan128cFormat();
				else if (*fieldIter == fieldOffsetCfgRwEanPrefix)
					fieldData = getRwEanPrefix();
				else if (*fieldIter == fieldOffsetCfgFwEanPrefix)
					fieldData = getFwEanPrefix();
				else if (*fieldIter == fieldOffsetCfgBcEanPrefix)
					fieldData = getBcEanPrefix();
				else if (*fieldIter < endCfgFieldOffsets)
					// Last ditch effort to get the data for a config field.  This is here because we can't just insert new fields into their
					// "most appropriate" config set, because this can screw up updating any field based on its specific offset.
					fieldData = config->toStringCfg(*fieldIter);

				serializedData.append(tag);
				serializedData.append(fieldData);
				serializedData.append(UnitSeparator);
			}
		}
		fieldIter++;
	}
}
