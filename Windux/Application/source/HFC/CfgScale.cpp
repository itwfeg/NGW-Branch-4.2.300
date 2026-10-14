/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgScale.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobDateTime.h"
#include "Graphic.h"
#include "HobTokenizerCSV.h"
#include "DBManager.h"
#include "DBQuery.h"
#include "DBConfigurationsRM.h"
#include "DBConfiguration.h"
#include "OperatorManager.h"
#include "DBOperatorsRM.h"
#include "DBOperator.h"
#include "HobOperator.h"
#include "ServiceSpeaker.h"
#include "LabelApplier.h"
#include "VersionManager.h"
#include "HobComplexVersion.h"

#include "SessionManager.h"
#include "Session.h"

#include "MultiLanguageText.h"

#include "PythonScripter.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"


using namespace edb;
using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgScale::dataType = 0;
std::string CfgScale::dataTypeName("CfgScale"); 
#endif

std::list<CfgFieldOffsets> CfgScale::basicScaleConfigList_;
std::list<CfgFieldOffsets> CfgScale::normalScaleOpList_;
std::list<CfgFieldOffsets> CfgScale::upgradeScaleList_;
std::list<CfgFieldOffsets> CfgScale::synchronizationList_;
std::list<CfgFieldOffsets> CfgScale::scaleManagementList_;
std::list<CfgFieldOffsets> CfgScale::reportsSetupList_;



CfgScale::CfgScale()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();
}

CfgScale::CfgScale(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	initFieldList();

	receivedData_.assign(data, len);
	receivedFormat_ = formatType;
}

CfgScale::~CfgScale()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// static
void CfgScale::initFieldList()		//added ats 10012002
{
	if (basicScaleConfigList_.empty())
	{
		basicScaleConfigList_.push_back(fieldOffsetCfgPrimaryCountryCode);
		basicScaleConfigList_.push_back(fieldOffsetCfgPrimaryLanguageCode);
		basicScaleConfigList_.push_back(fieldOffsetCfgSecondaryCountryCode);
		basicScaleConfigList_.push_back(fieldOffsetCfgSecondaryLanguageCode);
		basicScaleConfigList_.push_back(fieldOffsetCfgDateFormat);
		basicScaleConfigList_.push_back(fieldOffsetCfgTimeFormat);
		basicScaleConfigList_.push_back(fieldOffsetCfgFractionalPricingEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgPoundsForPricingEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgFixedWeightUnits);
		basicScaleConfigList_.push_back(fieldOffsetCfgPlatterTareEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgChangeZeroTareOnlyOnce);
		basicScaleConfigList_.push_back(fieldOffsetCfgChangeForcedTareOnlyOnce);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsScaleMetric);
		basicScaleConfigList_.push_back(fieldOffsetCfgOperateLoginRequired);
		basicScaleConfigList_.push_back(fieldOffsetCfgSupervisorLoginRequired);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoLookupProduct);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoDisplayProductNotes);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsRemoteKeyboardAttached);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsUSBKeyboardAttached);
		basicScaleConfigList_.push_back(fieldOffsetCfgPFWeightOnByCountPrinted);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServiceScaleFlag);	// comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgWeigherAttached);
		basicScaleConfigList_.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		basicScaleConfigList_.push_back(fieldOffsetCfgIProductGraphicsDir);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgIProductGraphicsDefaultExtension);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgICategoryGraphicsDir);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgICategoryGraphicsDefaultExtension);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgISpecialsGraphicsDir);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgISpecialsGraphicsDefaultExtension);  // comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoLookupOverrideDigit);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsLeapAvailable);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsServiceJumperSet);
		basicScaleConfigList_.push_back(fieldOffsetCfgHasWirelessCard);
		basicScaleConfigList_.push_back(fieldOffsetCfgITestMessagesEnabled);	// comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgIScaleModel); ////fieldOffsetCfgVpnEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgIScaleName);	// comes from Internal Config
		basicScaleConfigList_.push_back(fieldOffsetCfgPFSecondCheckDigit);
		basicScaleConfigList_.push_back(fieldOffsetCfgPopupKeypadEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefWrLabelPlacement);
		basicScaleConfigList_.push_back(fieldOffsetCfgIStreamingGraphicsDir);
		basicScaleConfigList_.push_back(fieldOffsetCfgIStreamingGraphicsDefaultExtension);
		basicScaleConfigList_.push_back(fieldOffsetCfgISelfServeGraphicsDir);
		basicScaleConfigList_.push_back(fieldOffsetCfgISelfServeGraphicsDefaultExtension);
		basicScaleConfigList_.push_back(fieldOffsetCfgISoftkeyGraphicsDir);
		basicScaleConfigList_.push_back(fieldOffsetCfgISoftkeyGraphicsDefaultExtension);
		basicScaleConfigList_.push_back(fieldOffsetCfgOpAccessToDesktopMode);
		basicScaleConfigList_.push_back(fieldOffsetCfgTareEntryPrecision);
		basicScaleConfigList_.push_back(fieldOffsetCfgRFIDHWPresent);
		basicScaleConfigList_.push_back(fieldOffsetCfgPopupKeypadStyle);
		basicScaleConfigList_.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryModel);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryPLU);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryWeight);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryQuantity);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryLanguage);
		basicScaleConfigList_.push_back(fieldOffsetCfgRebootUnprotectedAllowed);
		basicScaleConfigList_.push_back(fieldOffsetCfgCustomerDisplay);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoCreateRFIDOperator);
		basicScaleConfigList_.push_back(fieldOffsetCfgGCDLayoutNumber);
		basicScaleConfigList_.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		basicScaleConfigList_.push_back(fieldOffsetCfgGCDLanguage);
		basicScaleConfigList_.push_back(fieldOffsetCfgGCDAllowOperReset);
		basicScaleConfigList_.push_back(fieldOffsetCfgShowDeleteAllFiles);
		basicScaleConfigList_.push_back(fieldOffsetCfgShowPrinterSettings);
		basicScaleConfigList_.push_back(fieldOffsetCfgRemoteAccessPermitted);
		basicScaleConfigList_.push_back(fieldOffsetCfgRemoteAccessLoginMethod);
		basicScaleConfigList_.push_back(fieldOffsetCfgWeigherWakeupSensorAttached);
		basicScaleConfigList_.push_back(fieldOffsetCfgBackdateByDays);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoDisplaySuggestiveSelling);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoDisplayCookingNotes);
		basicScaleConfigList_.push_back(fieldOffsetCfgStartScreenHome);
		basicScaleConfigList_.push_back(fieldOffsetCfgBrowserHasAddressBar);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefaultSearchMethod);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnableAccumItems);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		basicScaleConfigList_.push_back(fieldOffsetCfgLeftRunscreenContent);
		basicScaleConfigList_.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		basicScaleConfigList_.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);
		basicScaleConfigList_.push_back(fieldOffsetCfgIPlanogramGraphicsDir);
		basicScaleConfigList_.push_back(fieldOffsetCfgIPlanogramGraphicsDefaultExtension);
		basicScaleConfigList_.push_back(fieldOffsetCfgDatabaseContainsOperators);
		basicScaleConfigList_.push_back(fieldOffsetCfgIVideosDir);
		basicScaleConfigList_.push_back(fieldOffsetCfgIVideosDefaultExtension);
		basicScaleConfigList_.push_back(fieldOffsetCfgISwitchToRunScreenOnStableWeight);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		basicScaleConfigList_.push_back(fieldOffsetCfgLicensedPackages);
		basicScaleConfigList_.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		basicScaleConfigList_.push_back(fieldOffsetCfgDisableLeftRunscreen);
		basicScaleConfigList_.push_back(fieldOffsetCfgRightScreenContent);
		basicScaleConfigList_.push_back(fieldOffsetCfgDisplayTransTypeIndicator);	//	FT-1466
		basicScaleConfigList_.push_back(fieldOffsetCfgIKeyboardNetWeightPrecision);
		basicScaleConfigList_.push_back(fieldOffsetCfgNowServingSysType);
		basicScaleConfigList_.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		basicScaleConfigList_.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);

		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryStore);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgIHTTPQueryDept);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellStyle);
		basicScaleConfigList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellFrequency);
		basicScaleConfigList_.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);
		basicScaleConfigList_.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsSingleRangeWeigher);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnableAccumItemsAndFlashkeys);

		basicScaleConfigList_.push_back(fieldOffsetCfgCutTestOptionEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);

		basicScaleConfigList_.push_back(fieldOffsetCfgBottomScreenContent);

		basicScaleConfigList_.push_back(fieldOffsetCfgIRequireMeatSpecSheet);
		

		basicScaleConfigList_.push_back(fieldOffsetCfgEnableSearchMenuButton);
	
	    basicScaleConfigList_.push_back(fieldOffsetCfgEnableHelpPrompts);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		basicScaleConfigList_.push_back(fieldOffsetCfgUseSecondTare);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPAllowOperatorUpdateNow);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPAllowOperatorSendToDept);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUserName );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPPassword );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPServerIporHostName );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSubDir );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdatePLUData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateLabelData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateFlashkeyData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateOperatorData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdatePlaylists );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateAlerts );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateScaleConfiguration );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateSynchronizationConfiguration );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateStoreNameNumber );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateVideos );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateImages );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateAudio );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPUpdateFirmware );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendPLUData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendLabelData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendFlashkeyData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendOperatorData );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendPlaylists );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendAlerts );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendScaleConfiguration );
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendSynchronizationConfiguration);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendStoreNameNumber);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendVideos);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendImages);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendAudio);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPSendFirmware);
		basicScaleConfigList_.push_back(fieldOffsetCfgScaleSynchronizationType);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPuseAutomatic);
	    basicScaleConfigList_.push_back(fieldOffsetCfgFTPmonday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPtuesday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPwednesday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPthursday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPfriday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPsaturday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPsunday);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPstartTime);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPdownloadFirmwareOnly);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPdownloadInstallFirmware);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPdailyUpdates);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTPhourlyUpdates);
		basicScaleConfigList_.push_back(fieldOffsetCfgFTP15MinuteUpdates);
		basicScaleConfigList_.push_back(fieldOffsetCfgAutoBackupDBFrequency);

		basicScaleConfigList_.push_back(fieldOffsetCfgDefaultSoftkeyFontPointSize);

		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServePrintToScreen);

		basicScaleConfigList_.push_back(fieldOffsetCfgVoidTransactionsViewHours);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeDisplayTareOnRW);
		basicScaleConfigList_.push_back(fieldOffsetDoNotPrintNutrifacts);
		basicScaleConfigList_.push_back(fieldOffsetCfgPick5Active);
		basicScaleConfigList_.push_back(fieldOffsetCfgIAllowProductionEntryModeOnWrapper);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsCutterEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsCodeCheckerEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgAllowSupervisorDotWear);
		basicScaleConfigList_.push_back(fieldOffsetCfgAllowOperatorDotWear);

		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeEnablePlaylists);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServePlaylistFile);

		basicScaleConfigList_.push_back(fieldOffsetCfgEnableSearchOnKeypad);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnableEnhancedCenterScreen);
		basicScaleConfigList_.push_back(fieldOffsetCfgEnhancedCenterHomePage);
		basicScaleConfigList_.push_back(fieldOffsetCfgVirtualCombinePrint);
		basicScaleConfigList_.push_back(fieldOffsetCfgIsConserveLinerLess);		//SOF-3875
		
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);

		basicScaleConfigList_.push_back(fieldOffsetCfgISelfServeWeightFieldWidth);
		basicScaleConfigList_.push_back(fieldOffsetCfgISelfServeWeightFieldHeight);
		basicScaleConfigList_.push_back(fieldOffsetCfgISelfServeWeightFieldFontSize);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);

		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeStartState);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeItemEntryState);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeSearchState);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServePrintState);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);

		basicScaleConfigList_.push_back(fieldOffsetCfgOnlineOrderingEnabled);
		basicScaleConfigList_.push_back(fieldOffsetCfgOnlineOrderingServer);

		basicScaleConfigList_.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		basicScaleConfigList_.push_back(fieldOffsetCfgDefWrSecApplierRotation);

        basicScaleConfigList_.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		basicScaleConfigList_.push_back(fieldOffsetCfgSecondaryLabelStockRotated);
		basicScaleConfigList_.push_back(fieldOffsetCfgWrapperCameraEnabled);

		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFixedPlu);
		
		basicScaleConfigList_.push_back(fieldOffsetCfgOnlineOrderingZone);
		basicScaleConfigList_.push_back(fieldOffsetCfgGCDPrintWithoutCustomerDisplay);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyType4);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		basicScaleConfigList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);

	}

	if (normalScaleOpList_.empty())
	{
		normalScaleOpList_.push_back(fieldOffsetCfgCollectingTransactions);
		normalScaleOpList_.push_back(fieldOffsetCfgCollectingTransactionsDays);
		normalScaleOpList_.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoLookupProduct);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		normalScaleOpList_.push_back(fieldOffsetCfgBeepOnStableWeight);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoDisplayProductNotes);
		normalScaleOpList_.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		normalScaleOpList_.push_back(fieldOffsetCfgForceUniqueProdClassNums);  //10
		normalScaleOpList_.push_back(fieldOffsetCfgPopupKeypadEnabled);
//		normalScaleOpList_.push_back(fieldOffsetCfgDaysToKeepOperatorChanges);
		normalScaleOpList_.push_back(fieldOffsetCfgPopupKeypadStyle);
		normalScaleOpList_.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);

		normalScaleOpList_.push_back(fieldOffsetCfgEnablePowerSaverMode);
		normalScaleOpList_.push_back(fieldOffsetCfgPowerSaverModeTimeout);
		normalScaleOpList_.push_back(fieldOffsetCfgInactivityAutoLogoutTime);
		normalScaleOpList_.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		normalScaleOpList_.push_back(fieldOffsetCfgDefaultProductRecordType);
		normalScaleOpList_.push_back(fieldOffsetCfgFixedWeightUnits);
		normalScaleOpList_.push_back(fieldOffsetCfgMaxTare);  // 20
		normalScaleOpList_.push_back(fieldOffsetCfgTareSetback);
		normalScaleOpList_.push_back(fieldOffsetCfgBeeperVolumeLevel);
		normalScaleOpList_.push_back(fieldOffsetCfgBycountCalcMethod);
		normalScaleOpList_.push_back(fieldOffsetCfgExternalWeightPrecision);
		normalScaleOpList_.push_back(fieldOffsetCfgTareEntryPrecision);
		normalScaleOpList_.push_back(fieldOffsetCfgClearPluAfterPrinting);
		normalScaleOpList_.push_back(fieldOffsetCfgUserRezeroWeightLimit);

		normalScaleOpList_.push_back(fieldOffsetCfgDefWrFilmWidth);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrFilmTension);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrFilmTensionFrontToBack);  //30
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrFilmTensionLeftToRight);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrIntakeMode);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrLabelPlacement);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrApplierRotation);

		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnSun);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffSun);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnMon);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffMon);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnTue);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffTue);  //40
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnWed);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffWed);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnThu);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffThu);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnFri);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffFri);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnSat);
		normalScaleOpList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffSat);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoCompressDB);
		normalScaleOpList_.push_back(fieldOffsetCfgCompressDBTime);  // 50
		normalScaleOpList_.push_back(fieldOffsetCfgAutoLookupOverrideDigit);
		normalScaleOpList_.push_back(fieldOffsetCfgCheckNetworkConnectivity);
		normalScaleOpList_.push_back(fieldOffsetCfgOpAccessToDesktopMode);
		normalScaleOpList_.push_back(fieldOffsetCfgPingPrimary);
		normalScaleOpList_.push_back(fieldOffsetCfgPingGateway);

		// jhc gcd to WINDUX merge: Am I putting these in the correct order with the other items that follow?
		//GCD
		normalScaleOpList_.push_back(fieldOffsetCfgGCDLayoutNumber);
		normalScaleOpList_.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		normalScaleOpList_.push_back(fieldOffsetCfgGCDAllowOperReset);

		// jhc gcd to WINDUX merge: These items were already here when I added the gcd stuff above
		normalScaleOpList_.push_back(fieldOffsetCfgShowDeleteAllFiles);
		normalScaleOpList_.push_back(fieldOffsetCfgShowPrinterSettings);  // 60
		normalScaleOpList_.push_back(fieldOffsetCfgRemoteAccessPermitted);
		normalScaleOpList_.push_back(fieldOffsetCfgRemoteAccessLoginMethod);
		normalScaleOpList_.push_back(fieldOffsetCfgUnitPriceSecurityLabel);
		normalScaleOpList_.push_back(fieldOffsetCfgTotalPriceSecurityLabel);
		normalScaleOpList_.push_back(fieldOffsetCfgBackdateByDays);
		normalScaleOpList_.push_back(fieldOffsetCfgPingTarget);
		normalScaleOpList_.push_back(fieldOffsetCfgPingTargetIPAddress);
		normalScaleOpList_.push_back(fieldOffsetCfgPingInterval);
		
		normalScaleOpList_.push_back(fieldOffsetCfgAutoDisplaySuggestiveSelling);
		normalScaleOpList_.push_back(fieldOffsetCfgAutoDisplayCookingNotes);  //70
		normalScaleOpList_.push_back(fieldOffsetCfgGCDPlaybackWaitTime);
		normalScaleOpList_.push_back(fieldOffsetCfgRemoteAccessUsername);
		normalScaleOpList_.push_back(fieldOffsetCfgRemoteAccessPassword);
		normalScaleOpList_.push_back(fieldOffsetCfgSystemFont);
		normalScaleOpList_.push_back(fieldOffsetCfgStartScreenHome);
		normalScaleOpList_.push_back(fieldOffsetCfgBrowserHasAddressBar);
		normalScaleOpList_.push_back(fieldOffsetCfgResetStatisticsDays);
		normalScaleOpList_.push_back(fieldOffsetCfgDefaultSearchMethod);

		normalScaleOpList_.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		normalScaleOpList_.push_back(fieldOffsetCfgLeftRunscreenContent);
		normalScaleOpList_.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		normalScaleOpList_.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);

		normalScaleOpList_.push_back(fieldOffsetCfgTrackingOperatorChanges);

		normalScaleOpList_.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		normalScaleOpList_.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		normalScaleOpList_.push_back(fieldOffsetCfgDisableLeftRunscreen);
		normalScaleOpList_.push_back(fieldOffsetCfgRightScreenContent);

		normalScaleOpList_.push_back(fieldOffsetCfgDisplayTransTypeIndicator);	//	FT-1466
		normalScaleOpList_.push_back(fieldOffsetCfgNowServingQueueIPAddress);
		normalScaleOpList_.push_back(fieldOffsetCfgNowServingSysType);

		normalScaleOpList_.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		normalScaleOpList_.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);

		normalScaleOpList_.push_back((CfgFieldOffsets)fieldOffsetCfgWeightRequiredForMeal);

		normalScaleOpList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellStyle);
		normalScaleOpList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellFrequency);

		normalScaleOpList_.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);
		normalScaleOpList_.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);
		normalScaleOpList_.push_back(fieldOffsetCfgPeriodicOfflineBeep);

		normalScaleOpList_.push_back(fieldOffsetCfgRequireFixedWeightVerification);

		normalScaleOpList_.push_back(fieldOffsetCfgCutTestOptionEnabled);
		normalScaleOpList_.push_back(fieldOffsetCfgMaxSuspendedCutTestsOverall);
		normalScaleOpList_.push_back(fieldOffsetCfgMaxSuspendedCutTestsOperator);
		normalScaleOpList_.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);

		normalScaleOpList_.push_back(fieldOffsetCfgBottomScreenContent);

		normalScaleOpList_.push_back(fieldOffsetCfgEnableSearchMenuButton);
	
	    normalScaleOpList_.push_back(fieldOffsetCfgEnableHelpPrompts);
		normalScaleOpList_.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		normalScaleOpList_.push_back(fieldOffsetCfgUseSecondTare);
		normalScaleOpList_.push_back(fieldOffsetCfgTrackingSupervisorChanges);
		normalScaleOpList_.push_back(fieldOffsetCfgVariableShelfLifeTime);

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeByCountItemsAllowed);

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServePrintToScreen);
		normalScaleOpList_.push_back(fieldOffsetCfgVoidTransactionsViewHours);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeDisplayTareOnRW);
		normalScaleOpList_.push_back(fieldOffsetDoNotPrintNutrifacts);

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeEnablePlaylists);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServePlaylistFile);

		normalScaleOpList_.push_back(fieldOffsetCfgEnableSearchOnKeypad);
		normalScaleOpList_.push_back(fieldOffsetCfgEnableEnhancedCenterScreen);
		normalScaleOpList_.push_back(fieldOffsetCfgEnhancedCenterHomePage);

		normalScaleOpList_.push_back(fieldOffsetCfgAutoUpdateUPCBarcodeOver99);
		normalScaleOpList_.push_back(fieldOffsetCfgBarcodeTypeOver99);
		normalScaleOpList_.push_back(fieldOffsetCfgVirtualCombinePrint);
		normalScaleOpList_.push_back(fieldOffsetCfgIsConserveLinerLess);	//SOF-3875

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);

		normalScaleOpList_.push_back(fieldOffsetCfgMinSweetheartWeight);

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeStartState);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeItemEntryState);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeSearchState);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServePrintState);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);
		normalScaleOpList_.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);						// PJR 420 - Flashkey Options to Toggle Extra Day
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		normalScaleOpList_.push_back(fieldOffsetCfgDefWrSecApplierRotation);

		normalScaleOpList_.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		normalScaleOpList_.push_back(fieldOffsetCfgSecondaryLabelStockRotated);
		normalScaleOpList_.push_back(fieldOffsetCfgWrapperCameraEnabled);

		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFixedPlu);
		normalScaleOpList_.push_back(fieldOffsetSelfServeTimeout);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyType4);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		normalScaleOpList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);
	}

	if (upgradeScaleList_.empty())
	{
		upgradeScaleList_.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		upgradeScaleList_.push_back(fieldOffsetCfgAutoSearchNewFirmwareVersions);
		upgradeScaleList_.push_back(fieldOffsetCfgNewFirmwareVersionSearchTime);
	}

	if (synchronizationList_.empty())
	{
// CfgScale
		synchronizationList_.push_back(fieldOffsetCfgStoreId);
		synchronizationList_.push_back(fieldOffsetCfgStoreName);
		synchronizationList_.push_back(fieldOffsetCfgStoreGraphic);
		synchronizationList_.push_back(fieldOffsetCfgDeptName_NOT_USED);

		synchronizationList_.push_back(fieldOffsetCfgPrimaryCountryCode);
		synchronizationList_.push_back(fieldOffsetCfgPrimaryLanguageCode);
		synchronizationList_.push_back(fieldOffsetCfgSecondaryCountryCode);
		synchronizationList_.push_back(fieldOffsetCfgSecondaryLanguageCode);
		synchronizationList_.push_back(fieldOffsetCfgLengthUnits);
		synchronizationList_.push_back(fieldOffsetCfgTemperatureUnits);
//		synchronizationList_.push_back(fieldOffsetCfgCurrencyUnits);
//		synchronizationList_.push_back(fieldOffsetCfgMonetarySymbol);
		synchronizationList_.push_back(fieldOffsetCfgDateFormat);
		synchronizationList_.push_back(fieldOffsetCfgTimeFormat);

		synchronizationList_.push_back(fieldOffsetCfgUnitPriceSecurityLabel);
		synchronizationList_.push_back(fieldOffsetCfgTotalPriceSecurityLabel);
		synchronizationList_.push_back(fieldOffsetCfgFractionalPricingEnabled);
		synchronizationList_.push_back(fieldOffsetCfgPoundsForPricingEnabled);
		synchronizationList_.push_back(fieldOffsetCfgBycountCalcMethod);

		synchronizationList_.push_back(fieldOffsetCfgBeepOnStableWeight);
		synchronizationList_.push_back(fieldOffsetCfgBeeperVolumeLevel);

		synchronizationList_.push_back(fieldOffsetCfgExternalWeightPrecision);
		//synchronizationList_.push_back(fieldOffsetCfgTareEntryPrecision);  as of April 2017, no longer configurable, per W&M
		synchronizationList_.push_back(fieldOffsetCfgFixedWeightUnits);
		synchronizationList_.push_back(fieldOffsetCfgTareSetback);
		synchronizationList_.push_back(fieldOffsetCfgMaxTare);
		synchronizationList_.push_back(fieldOffsetCfgPlatterTareEnabled);
		synchronizationList_.push_back(fieldOffsetCfgChangeZeroTareOnlyOnce);
		synchronizationList_.push_back(fieldOffsetCfgChangeForcedTareOnlyOnce);

		synchronizationList_.push_back(fieldOffsetCfgOperateLoginRequired);
		//fieldOffsetCfgSupervisorLoginRequired
		synchronizationList_.push_back(fieldOffsetCfgInactivityAutoLogoutTime);
		synchronizationList_.push_back(fieldOffsetCfgAutoLookupProduct);
		synchronizationList_.push_back(fieldOffsetCfgAutoLookupNumOfDigits);
		synchronizationList_.push_back(fieldOffsetCfgAutoDisplayProductNotes);

		synchronizationList_.push_back(fieldOffsetCfgDbBackupFrequency);

		synchronizationList_.push_back(fieldOffsetCfgHorizLabelSkew1);
		synchronizationList_.push_back(fieldOffsetCfgHorizLabelSkew2);
		synchronizationList_.push_back(fieldOffsetCfgHorizLabelSkew3);

		synchronizationList_.push_back(fieldOffsetCfgCollectingTransactions);
		synchronizationList_.push_back(fieldOffsetCfgCollectingTransactionsDays);

		synchronizationList_.push_back(fieldOffsetCfgOpAccessToBrowserMode);
		synchronizationList_.push_back(fieldOffsetCfgAutoLoadOldFirmwareVersions);
		synchronizationList_.push_back(fieldOffsetCfgAutoSearchNewFirmwareVersions);
		synchronizationList_.push_back(fieldOffsetCfgNewFirmwareVersionSearchTime);
		synchronizationList_.push_back(fieldOffsetCfgForceUniqueProdClassNums);
		synchronizationList_.push_back(fieldOffsetCfgEnablePowerSaverMode);
		synchronizationList_.push_back(fieldOffsetCfgPowerSaverModeTimeout);
		synchronizationList_.push_back(fieldOffsetCfgDefaultBrowserHomePage);
		synchronizationList_.push_back(fieldOffsetCfgDefaultProductRecordType);

		synchronizationList_.push_back(fieldOffsetCfgDefWrFilmWidth);
		synchronizationList_.push_back(fieldOffsetCfgDefWrFilmTension);
		synchronizationList_.push_back(fieldOffsetCfgDefWrIntakeMode);
		synchronizationList_.push_back(fieldOffsetCfgDefWrLabelPlacement);
		synchronizationList_.push_back(fieldOffsetCfgDefWrApplierRotation);

		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnSun);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffSun);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnMon);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffMon);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnTue);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffTue);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnWed);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffWed);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnThu);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffThu);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnFri);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffFri);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOnSat);
		synchronizationList_.push_back(fieldOffsetCfgWrSealerBeltTimeOffSat);

		synchronizationList_.push_back(fieldOffsetCfgAutoCompressDB);
		synchronizationList_.push_back(fieldOffsetCfgCompressDBTime);
		synchronizationList_.push_back(fieldOffsetCfgAutoLookupOverrideDigit);

		synchronizationList_.push_back(fieldOffsetCfgCOOLTrackingNumberEnabled);

		//fieldOffsetCfgPopupKeypadEnabled

		synchronizationList_.push_back(fieldOffsetCfgPrimaryExpirationPageNote);
		synchronizationList_.push_back(fieldOffsetCfgSecondExpirationPageNote);

		synchronizationList_.push_back(fieldOffsetCfgCheckNetworkConnectivity);
		synchronizationList_.push_back(fieldOffsetCfgOpAccessToDesktopMode);
		synchronizationList_.push_back(fieldOffsetCfgPingPrimary);
		synchronizationList_.push_back(fieldOffsetCfgPingGateway);
		synchronizationList_.push_back(fieldOffsetCfgScaleType);

		synchronizationList_.push_back(fieldOffsetCfgClearPluAfterPrinting);
		//fieldOffsetCfgCommunicationHistory
		//fieldOffsetCfgCommunicationItemReceived

//		synchronizationList_.push_back(fieldOffsetCfgDaysToKeepOperatorChanges);
		synchronizationList_.push_back(fieldOffsetCfgPopupKeypadStyle);
		synchronizationList_.push_back(fieldOffsetCfgOperatorLoginKeyboardStyle);

		synchronizationList_.push_back(fieldOffsetCfgRebootUnprotectedAllowed);

		synchronizationList_.push_back(fieldOffsetCfgCustomerDisplay);

		synchronizationList_.push_back(fieldOffsetCfgAutoCreateRFIDOperator);

		// jhc gcd to WINDUX merge: Am I putting these in the correct order with the other items that follow?
		// GCD
		synchronizationList_.push_back(fieldOffsetCfgGCDLayoutNumber);
		synchronizationList_.push_back(fieldOffsetCfgGCDColorSchemeNumber);
		synchronizationList_.push_back(fieldOffsetCfgGCDAllowOperReset);
		synchronizationList_.push_back(fieldOffsetCfgGCDUseTransactionalLayout);
		synchronizationList_.push_back(fieldOffsetCfgGCDIdleAdvertType);
		synchronizationList_.push_back(fieldOffsetCfgGCDIdleWebsiteAddress);
		synchronizationList_.push_back(fieldOffsetCfgGCDProductAdvertType);

		// jhc gcd to WINDUX merge: These items were already here when I added the gcd stuff above
		synchronizationList_.push_back(fieldOffsetCfgUserRezeroWeightLimit);

		synchronizationList_.push_back(fieldOffsetCfgDefaultRunScreenHomePage);
		synchronizationList_.push_back(fieldOffsetCfgLeftRunscreenContent);
		synchronizationList_.push_back(fieldOffsetCfgMiddleRunscreenDynamicContent);
		synchronizationList_.push_back(fieldOffsetCfgMiddleRunscreenKeypadContent);

		synchronizationList_.push_back(fieldOffsetCfgTrackingOperatorChanges);
		synchronizationList_.push_back(fieldOffsetCfgRightScreenContent);
		
		synchronizationList_.push_back(fieldOffsetCfgEnableDisplayNetworkDetails);
		synchronizationList_.push_back(fieldOffsetCfgDisableOperatorFlashkeyEditor);
		synchronizationList_.push_back(fieldOffsetCfgDisableLeftRunscreen);

		synchronizationList_.push_back(fieldOffsetCfgNowServingQueueIPAddress);
		synchronizationList_.push_back(fieldOffsetCfgNowServingSysType);
		///// I ADDED ALL OF THESE.  I ADDED them so at least the list from NowServingSysType would be full
		// all of the way to the end where I added fieldOffsetCfgTrackingSupervisorChanges.
		// There are quite a few fields missing here between Operator Changes and the end of this list
		// Talk to Larry about this and see if we need to add more fields to match Normal Scale Operations.
		synchronizationList_.push_back(fieldOffsetCfgDisplaySelfServeStartScreen);
		synchronizationList_.push_back(fieldOffsetCfgBestSellersOnSelfServeMainScreen);

		synchronizationList_.push_back((CfgFieldOffsets)fieldOffsetCfgWeightRequiredForMeal);

		synchronizationList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellStyle);
		synchronizationList_.push_back((CfgFieldOffsets)fieldOffsetCfgSuggestiveSellFrequency);

		synchronizationList_.push_back(fieldOffsetCfgAlwaysEnableVirtualKeyboard);
		synchronizationList_.push_back(fieldOffsetCfgShowSelectAllWhenVoidingTrans);
		synchronizationList_.push_back(fieldOffsetCfgPeriodicOfflineBeep);

		synchronizationList_.push_back(fieldOffsetCfgRequireFixedWeightVerification);

		synchronizationList_.push_back(fieldOffsetCfgCutTestOptionEnabled);
		synchronizationList_.push_back(fieldOffsetCfgMaxSuspendedCutTestsOverall);
		synchronizationList_.push_back(fieldOffsetCfgMaxSuspendedCutTestsOperator);
		synchronizationList_.push_back(fieldOffsetCfgSecondsToAutoSuspendCutTest);

		synchronizationList_.push_back(fieldOffsetCfgBottomScreenContent);

		synchronizationList_.push_back(fieldOffsetCfgEnableSearchMenuButton);
	
	    synchronizationList_.push_back(fieldOffsetCfgEnableHelpPrompts);
		synchronizationList_.push_back(fieldOffsetCfgEnableUseCharacterSearching);

		synchronizationList_.push_back(fieldOffsetCfgUseSecondTare);
		//// THESE WERE ADDED SO STOP HERE. The next line is brand spanking new.  Its not even dusty yet. 
		synchronizationList_.push_back(fieldOffsetCfgTrackingSupervisorChanges);
		synchronizationList_.push_back(fieldOffsetCfgVariableShelfLifeTime);

		synchronizationList_.push_back(fieldOffsetCfgIsCutterEnabled);
		synchronizationList_.push_back(fieldOffsetCfgIsCodeCheckerEnabled);

		synchronizationList_.push_back(fieldOffsetCfgAllowSupervisorDotWear);
		synchronizationList_.push_back(fieldOffsetCfgAllowOperatorDotWear);

		synchronizationList_.push_back(fieldOffsetCfgAutoUpdateUPCBarcodeOver99);
		synchronizationList_.push_back(fieldOffsetCfgBarcodeTypeOver99);

		synchronizationList_.push_back(fieldOffsetCfgSelfServeIgnorePrinterErrors);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeSearchKeypadFlashkeys);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeDisplayPluLabel);

		synchronizationList_.push_back(fieldOffsetCfgSelfServeDisplayBarcodeLabel);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeStartState);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeItemEntryState);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeSearchState);
		synchronizationList_.push_back(fieldOffsetCfgSelfServePrintState);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeAfterPrintingState);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeStartScreenClickable);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeIncludeKeypad);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeHideMenuBar);
		synchronizationList_.push_back(fieldOffsetCfgSelfServePrintOnStableWeight);
		synchronizationList_.push_back(fieldOffsetCfgEnableVariableShelfLifeTime);							// PJR 420 - Flashkey Options to Toggle Extra Day
		synchronizationList_.push_back(fieldOffsetCfgDefWrSecLabelPlacement);
		synchronizationList_.push_back(fieldOffsetCfgDefWrSecApplierRotation);
        synchronizationList_.push_back(fieldOffsetCfgPrimaryLabelStockRotated);
		synchronizationList_.push_back(fieldOffsetCfgSecondaryLabelStockRotated);

		synchronizationList_.push_back(fieldOffsetCfgSelfServeFixedPlu);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeEnableFreshServe);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyType0);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyType1);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyType2);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyType3);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyType4);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyText0);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyText1);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyText2);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyText3);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyText4);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum0);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum1);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum2);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum3);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeyCatNum4);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFreshServeLabelOffset);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText0);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText1);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText2);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText3);
		synchronizationList_.push_back(fieldOffsetCfgSelfServeFlashkeySecText4);

		ConfigurationTags configurationTags((FieldOffsetsList *)&synchronizationList_);
	}

	if (scaleManagementList_.empty())
	{
		scaleManagementList_.push_back(fieldOffsetCfgIgnoreUltimaFormatLabelTypes);
		scaleManagementList_.push_back(fieldOffsetCfgDontChangeCurrentCOOL);
		scaleManagementList_.push_back(fieldOffsetCfgEnableUPTagPrefix);
	}

	if (reportsSetupList_.empty())
	{
		reportsSetupList_.push_back(fieldOffsetCfgReportsOffsetTop);
		reportsSetupList_.push_back(fieldOffsetCfgReportsOffsetBottom);
	}
}


void CfgScale::updateConfiguration() const
{
	Configuration *config = Configuration::getInstance();

	// Text fields are quoted when received from UI, so can't use the default implementation of parsing
	// the input string and updating config as would be done on the next line.
	//	Configuration::getInstance()->updateConfigurationFromUI(browserFieldList_, receivedData_);

	switch (receivedFormat_)
	{
		case BrowserFormat:
			updateConfigurationUsingList(basicScaleConfigList_);
			break;
		case NormalScaleOpFormat:
			updateConfigurationUsingList(normalScaleOpList_);
			break;
		case UpgradeScaleFormat:
			updateConfigurationUsingList(upgradeScaleList_);
			break;
		case ScaleManagementFormat:
			updateConfigurationUsingList(scaleManagementList_);
			break;
		case ReportsSetupFormat:
			updateConfigurationUsingList(reportsSetupList_);
			break;
		case ExpirationLabelMessageFormat:
		{
			Configuration *config = Configuration::getInstance();
			HobString temp(receivedData_);

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			if (tok.next(&token) != -1 && token.size())
				config->updateField(fieldOffsetCfgPrimaryExpirationPageNote, token);
				
			if (tok.next(&token) != -1 && token.size())
				config->updateField(fieldOffsetCfgSecondExpirationPageNote, token);
			
			break;
		}
		default:	
			break;
	}

	config->validateTareEntryPrecision();

	//	Don't allow tare setback to be greater than the maxTare.
	if (config->toIntCfg(fieldOffsetCfgTareSetback) > config->toIntCfg(fieldOffsetCfgMaxTare))
		config->setTareSetback(config->toStringCfg(fieldOffsetCfgMaxTare));
}


void CfgScale::updateConfigurationUsingList(const list<CfgFieldOffsets>& fieldOffsetList) const
{
	Configuration *config = Configuration::getInstance();

	HobString temp(receivedData_);
	list<CfgFieldOffsets>::const_iterator fieldIter = fieldOffsetList.begin();

	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;

	while ((tok.next(&token) != -1) && (fieldIter != fieldOffsetList.end()))
	{
		if (token.size())
			config->updateField(*fieldIter, token);
		fieldIter++;
	}
}


bool CfgScale::isSaveNeeded() const
{
	return Configuration::getInstance()->isSaveNeeded(synchronizationList_);
}


const char *CfgScale::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	Configuration *config = Configuration::getInstance();

	list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;
	HobString fieldData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			fieldIter = basicScaleConfigList_.begin();
			while (fieldIter != basicScaleConfigList_.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				if (*fieldIter == fieldOffsetCfgOperateLoginRequired)
				{
					/* Additional requirement when sending this flag to the browser:
					 * If there aren't any operators in the database, then the OperateLoginRequired
					 * is false, regardless of its setting in the database.
					 */
					if (Configuration::getInstance()->isSelfServiceScale())
						fieldData = Bool::toString(false);
					else
					{
						DBManager *myDatabase = DBManager::getInstance();
						if (myDatabase)
						{
							DBOperatorsRM *opRM = myDatabase->createOperators();
							if (opRM)
							{
								int opCount = opRM->countAll();
								delete opRM;
								if (opCount == 0)
									fieldData = Bool::toString(false);
							}
							DBManager::destroyInstance(myDatabase);
						}
					}
				}
				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case NormalScaleOpFormat:
			serializeUsingList(normalScaleOpList_, NormalScaleOpFormat);
			break;
		case UpgradeScaleFormat:
			serializeUsingList(upgradeScaleList_, UpgradeScaleFormat);
			break;
		case ScaleManagementFormat:
			serializeUsingList(scaleManagementList_, ScaleManagementFormat);
			break;
		case ReportsSetupFormat:
			serializeUsingList(reportsSetupList_, ReportsSetupFormat);
			break;
			
		case ExpirationLabelMessageFormat:
		{
			fieldData = config->toStringCfg(fieldOffsetCfgPrimaryExpirationPageNote);
			fieldData.quoteForCSV();
			serializedData.append(fieldData);
			
			serializedData.append(BrowserFormatSeparator);
			
			fieldData = config->toStringCfg(fieldOffsetCfgSecondExpirationPageNote);
			fieldData.quoteForCSV();
			serializedData.append(fieldData);

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = synchronizationList_.begin();
			while (fieldIter != synchronizationList_.end())
			{
				tag = configTags.getTag(*fieldIter);
				if (tag.size())
				{
					fieldData = "";
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		case StoreScaleInfoReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			HobString tempValue;

			serializedData.assign(internationalize("ConfigReport.StoreInfo")).append(1, '\n');
//			serializedData.assign("Store/Department Information\n");

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgStoreName))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgStoreName, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgStoreId))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgStoreId, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgStoreGraphic))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgStoreGraphic, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgDeptName_NOT_USED))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgDeptName_NOT_USED, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			break;
		}

		case LocaleReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			HobString tempValue;

			serializedData.assign(internationalize("ConfigReport.CountryLanguage")).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPrimaryLanguageCode))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgPrimaryLanguageCode, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPrimaryCountryCode))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgPrimaryCountryCode, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgSecondaryLanguageCode))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgSecondaryLanguageCode, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgSecondaryCountryCode))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgSecondaryCountryCode, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgTimeFormat))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgTimeFormat, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgLengthUnits))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgLengthUnits, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgTemperatureUnits))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgTemperatureUnits, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCurrencyUnits))).append(2, '\t');
//			getFieldAsString(fieldOffsetCfgCurrencyUnits, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgMonetarySymbol))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgMonetarySymbol, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

			break;
		}

		case NormalScaleOperationReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			HobString tempValue;

			serializedData.assign(internationalize("ConfigReport.NormalScaleOps")).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgSupervisorLoginRequired))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgSupervisorLoginRequired, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgOperateLoginRequired))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgOperateLoginRequired, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoDisplayProductNotes))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgAutoDisplayProductNotes, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgInactivityAutoLogoutTime))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgInactivityAutoLogoutTime, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(" min.").append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgEnablePowerSaverMode))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgEnablePowerSaverMode, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPowerSaverModeTimeout))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgPowerSaverModeTimeout, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoLookupProduct))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgAutoLookupProduct, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoLookupNumOfDigits))).append(4, '\t');
			getFieldAsString(fieldOffsetCfgAutoLookupNumOfDigits, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoLookupOverrideDigit))).append(2, '\t');
//			getFieldAsString(fieldOffsetCfgAutoLookupOverrideDigit, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(2, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCollectingTransactions))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgCollectingTransactions, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCollectingTransactionsDays))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgCollectingTransactionsDays, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(" days").append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgUnitPriceSecurityLabel))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgUnitPriceSecurityLabel, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgTotalPriceSecurityLabel))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgTotalPriceSecurityLabel, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFractionalPricingEnabled))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgFractionalPricingEnabled, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPoundsForPricingEnabled))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgPoundsForPricingEnabled, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgBycountCalcMethod))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgBycountCalcMethod, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgFixedWeightUnits))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgFixedWeightUnits, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgDefaultProductRecordType))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgDefaultProductRecordType, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgForceUniqueProdClassNums))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgForceUniqueProdClassNums, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgHorizLabelSkew1))).append(3, '\t');
//			getFieldAsString(fieldOffsetCfgHorizLabelSkew1, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgHorizLabelSkew2))).append(3, '\t');
//			getFieldAsString(fieldOffsetCfgHorizLabelSkew2, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgHorizLabelSkew3))).append(3, '\t');
//			getFieldAsString(fieldOffsetCfgHorizLabelSkew3, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(2, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgOpAccessToBrowserMode))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgOpAccessToBrowserMode, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgDefaultBrowserHomePage))).append(4, '\t');
			getFieldAsString(fieldOffsetCfgDefaultBrowserHomePage, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgBeeperVolumeLevel))).append(3, '\t');
//			getFieldAsString(fieldOffsetCfgBeeperVolumeLevel, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgIsUSBKeyboardAttached))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgIsUSBKeyboardAttached, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPingGateway))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgPingGateway, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(4, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPingTarget))).append(2, '\t');
			getFieldAsString(fieldOffsetCfgPingTarget, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			break;
		}

		case BackgroundReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			HobString tempValue;

//			serializedData.assign("Background Processing Settings\n");

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoLoadOldFirmwareVersions))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgAutoLoadOldFirmwareVersions, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.assign(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoSearchNewFirmwareVersions))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgAutoSearchNewFirmwareVersions, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgNewFirmwareVersionSearchTime))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgNewFirmwareVersionSearchTime, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgDbBackupFrequency))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgDbBackupFrequency, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(" min.").append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgAutoCompressDB))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgAutoCompressDB, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgCompressDBTime))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgCompressDBTime, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

			break;
		}

		case WeigherReportFormat:
		{
			hfc::MultiLanguageText internationalize;
			ConfigurationTags configTags;

			HobString tempValue;

			serializedData.assign(internationalize("ConfigReport.Weigher")).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgWeigherAttached))).append(3, '\t');
			getFieldAsString(fieldOffsetCfgWeigherAttached, tempValue, EvolutionTaggedFormat);
			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			if ( config->toBoolCfg(fieldOffsetCfgWeigherAttached) )
			{
				serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgWeigherModel))).append(4, '\t');
				getFieldAsString(fieldOffsetCfgWeigherModel, tempValue, EvolutionTaggedFormat);
				serializedData.append(tempValue).append(1, '\n');
			}

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgIsScaleMetric))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgIsScaleMetric, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgPlatterTareEnabled))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgPlatterTareEnabled, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgMaxTare))).append(4, '\t');
			getFieldAsString(fieldOffsetCfgMaxTare, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgMinWeightToPrintLabel))).append(1, '\t');
			getFieldAsString(fieldOffsetCfgMinWeightToPrintLabel, tempValue, EvolutionTaggedFormat);
			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgWeighMode))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgWeighMode, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgTareSetback))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgTareSetback, tempValue, EvolutionTaggedFormat);
//			serializedData.append(tempValue).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgBeepOnStableWeight))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgBeepOnStableWeight, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgChangeZeroTareOnlyOnce))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgChangeZeroTareOnlyOnce, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

//			serializedData.append(2, ' ').append(internationalize("ConfigReport", configTags.getTag(fieldOffsetCfgChangeForcedTareOnlyOnce))).append(1, '\t');
//			getFieldAsString(fieldOffsetCfgChangeForcedTareOnlyOnce, tempValue, EvolutionTaggedFormat);
//			serializedData.append(yesNoValue(tempValue)).append(1, '\n');

			break;
		}

		default:
			serializedData = HobString("CfgScale::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.c_str();
}


void CfgScale::serializeUsingList(const list<CfgFieldOffsets> fieldOffsetList, int format) const
{
	bool appendSeparator = false;
	HobString fieldData = "";
	
	list<CfgFieldOffsets>::const_iterator fieldIter = fieldOffsetList.begin();
	while (fieldIter != fieldOffsetList.end())
	{
		if (appendSeparator)
			serializedData.append(BrowserFormatSeparator);
		else
			appendSeparator = true;

		getFieldAsString(*fieldIter, fieldData, format);
		
		if (isFieldQuoted(*fieldIter))
			fieldData.quoteForCSV();
			
		serializedData.append(fieldData);
		fieldIter++;
	}
}


bool CfgScale::isFieldQuoted(int fieldOffsetCfg) const
{
	switch (fieldOffsetCfg)
	{
		case fieldOffsetCfgStoreName:
		case fieldOffsetCfgDeptName_NOT_USED:
		case fieldOffsetCfgPrimaryCountryCode:
		case fieldOffsetCfgPrimaryLanguageCode:
		case fieldOffsetCfgSecondaryCountryCode:
		case fieldOffsetCfgSecondaryLanguageCode:
		case fieldOffsetCfgDefaultBrowserHomePage:
		case fieldOffsetCfgDefaultRunScreenHomePage:
		case fieldOffsetCfgPrimaryExpirationPageNote:
		case fieldOffsetCfgSecondExpirationPageNote:
		case fieldOffsetCfgTareSetback:
		case fieldOffsetCfgMaxTare:
		case fieldOffsetCfgMinSweetheartWeight:
		case fieldOffsetCfgRemoteAccessUsername:
		case fieldOffsetCfgRemoteAccessPassword:

		case fieldOffsetCfgSystemFont:
			return true;
			break;

		default:
			break;
	}
	return false;
}


bool CfgScale::isFieldScaleOnly(int fieldCode)
{
	if (fieldCode == fieldOffsetCfgWeigherAttached)
		return true;
	else
		return false;
}


void CfgScale::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	result = "";

	Configuration *config = Configuration::getInstance();

	if ( fieldCode == fieldOffsetCfgIScaleName )
	{
		string modelName;
		if ( config->toBoolCfgI(CfgInternal::PYTHONENABLED) )
		{
			const string command( PythonScripter::buildPythonCall("getModelName.py") );
			string modelNameOutput;
			PAL_absclass::PAL_executeWithResults(command, modelNameOutput);
			Debug::trace("CfgScale", "getting model name from python script - Data received from getModelName.py: " + modelNameOutput);

			if ( !modelNameOutput.empty() )
				modelName = modelNameOutput;
		}

		result = modelName;
		return;
	}
	else if ( fieldCode == fieldOffsetCfgLicensedPackages )
	{
		HobString licensedPackages = config->getLicensedPackages();
		licensedPackages.quoteForCSV();
		result = licensedPackages;
		return;
	}
	else if ( fieldCode == fieldOffsetCfgSelfServeWeightRequiredToSelectItem )
	{
		bool autoSS = config->toBoolCfg(fieldOffsetCfgSelfServeWeightRequiredToSelectItem);
		result = Bool::toString(autoSS);
		return;
	}
	else if ( fieldCode == fieldOffsetCfgSelfServeEnablePlaylists )
	{
		bool listSS = config->toBoolCfg(fieldOffsetCfgSelfServeEnablePlaylists);
		result = Bool::toString(listSS);
		return;
	}
	else if ( fieldCode == fieldOffsetCfgSelfServePrintToScreen )
	{
		bool autoSS = config->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen);
		result = Bool::toString(autoSS);
		return;
	}
	else if (fieldCode == fieldOffsetCfgSelfServeDisplayTareOnRW)
	{
		bool autoSS = config->toBoolCfg(fieldOffsetCfgSelfServeDisplayTareOnRW);
		result = Bool::toString(autoSS);
		return;
	}
	else if (fieldCode >= INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
	{
		result = config->toStringCfgI(fieldCode);
		if ( "T" == result || "t" == result || "F" == result || "f" == result )
			result = Bool::toString(config->toBoolCfgI(fieldCode));
		return;
	}

	switch (fieldCode)
	{
		case fieldOffsetCfgUnitPriceSecurityLabel:
			result = config->formatHobPrice(config->toIntCfg(fieldOffsetCfgUnitPriceSecurityLabel), ((format == BrowserFormat) || (format == NormalScaleOpFormat)));
			break;

		case fieldOffsetCfgTotalPriceSecurityLabel:
			result = config->formatHobPrice(config->toIntCfg(fieldOffsetCfgTotalPriceSecurityLabel), ((format == BrowserFormat) || (format == NormalScaleOpFormat)));
			break;

		case fieldOffsetCfgTareSetback:
			config->formatWeight( config->toIntCfg(fieldOffsetCfgTareSetback), result );
			break;

		case fieldOffsetCfgMaxTare:
			config->formatWeight( config->toIntCfg(fieldOffsetCfgMaxTare), result );
			break;

		case fieldOffsetCfgMinSweetheartWeight:
			config->formatWeight( config->toIntCfg(fieldOffsetCfgMinSweetheartWeight), result );
			break;

		case fieldOffsetCfgMinWeightToPrintLabel:
			config->formatWeight(config->toIntCfg(fieldOffsetCfgMinWeightToPrintLabel), result, ((format == BrowserFormat) || (format == NormalScaleOpFormat)));
			break;

		case fieldOffsetCfgUserRezeroWeightLimit:
			config->formatWeight(config->toIntCfg(fieldOffsetCfgUserRezeroWeightLimit), result, ((format == BrowserFormat) || (format == NormalScaleOpFormat)));
			break;
		
		case fieldOffsetCfgIsScaleMetric:
			result = Bool::toString( MetSig::isScaleMetric() );
			break;
			
		case fieldOffsetCfgSupervisorLoginRequired:
			result = Bool::toString(OperatorManager::isSupervisorLoginRequired());
			break;

		case fieldOffsetCfgSelfServiceScaleFlag:
			result = Bool::toString(config->isSelfServiceScale());
			break;

		case fieldOffsetCfgTareEntryPrecision:
			result = Integer::toString(config->getTareEntryPrecision());
			break;

		case fieldOffsetCfgRemoteAccessPermitted:
		{
			if (config->toBoolCfg(fieldOffsetCfgRemoteAccessPermitted) && format == BrowserFormat &&
			    (config->toBoolCfg(fieldOffsetCfgRemoteAccessRequiresLogin) || config->toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) == RemoteAccessLoginMethod_OperatorID))
			{
				//	Since remoteAccessRequiresLogin or remoteAccessLoginMethod=RemoteAccessLoginMethod_OperatorID, we need Operator ID's to login.
				//	If we don't have any, then remoteAccess is NOT permitted.
				bool haveOps = false;
				DBManager *myDatabase = DBManager::getInstance();
				if (myDatabase)
				{
					DBOperatorsRM *opRM = myDatabase->createOperators();
					if (opRM)
					{
						int count = opRM->countAll();
						if (count)
							haveOps = true;
						delete opRM;
					}
					DBManager::destroyInstance(myDatabase);
				}
				result = Bool::toString(haveOps);
			}
			else
				result = config->toStringCfg(fieldOffsetCfgRemoteAccessPermitted);
			break;
		}
		
		case fieldOffsetCfgFirmwareVersion:
		{
			HobComplexVersion *myVersion = VersionManager::getInstance()->getSystemControllerSoftwareVersion();
			if (myVersion)
				result = Integer::toString(myVersion->getMajorRevision());
			break;
		}
		case fieldOffsetCfgFirmwareRevision:
		{
			HobComplexVersion *myVersion = VersionManager::getInstance()->getSystemControllerSoftwareVersion();
			if (myVersion)
			{
				result.append(Integer::toString(myVersion->getMinorRevision()));
				result.append(".");
				result.append(Integer::toString(myVersion->getEngRevision()));
				//result.append(Integer::toString(myVersion->getBuildNumber()));
			}
			break;
		}

		case fieldOffsetCfgRFIDHWPresent:
			result = Bool::toString(config->isRFIDHWPresent());
			break;

		case fieldOffsetCfgGCDLanguage:
			switch ( config->toIntCfg(fieldOffsetCfgGCDLanguage) )
			{
				case GcdLanguage_French:
					result = "Fr";
					break;
				case GcdLanguage_Spanish:
					result = "Sp";
					break;
				default: // default to English
					result = "En";
					break;
			}
			break;

		case fieldOffsetCfgDatabaseContainsOperators:
		{
			bool haveOps = false;
			DBManager *myDatabase = DBManager::getInstance();
			if (myDatabase)
			{
				DBOperatorsRM *opRM = myDatabase->createOperators();
				if (opRM)
				{
					int count = opRM->countAll();
					if (count > 0)
						haveOps = true;
					delete opRM;
				}
				DBManager::destroyInstance(myDatabase);
			}
			result = Bool::toString(haveOps);
			break;
		}

		case fieldOffsetCfgIsSingleRangeWeigher:
		{
			result = Bool::toString(MetSig::isWeigherConfiguredForSingleRange());
			break;
		}

		case fieldOffsetCfgVirtualCombinePrint:
		{
			result = Bool::toString(config->isCombinedVirtualEnabled());	
			break;
		}
		case fieldOffsetCfgIsConserveLinerLess:			//SOF-3875
		{
			result = Bool::toString( config->isConserveLinerLessEnabled() );	
			break;
		} 
		default:
			result = config->toStringCfg(fieldCode);
			break;
	}
}
