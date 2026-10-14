/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ConfigurationTags.h"
#include "TransactionTags.h"
#include "CfgBase.h"
#include "Debug.h"
#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ConfigurationTags::dataType = 0;
std::string ConfigurationTags::dataTypeName("ConfigurationTags"); 
#endif

const std::string ConfigurationTags::emptyTag = "";

// CfgScale

const std::string ConfigurationTags::storeId                             = "si";
const std::string ConfigurationTags::storeIdN                            = "Store ID";
const std::string ConfigurationTags::storeName                           = "sn";
const std::string ConfigurationTags::storeNameN                          = "Store Name";
const std::string ConfigurationTags::storeGraphic                        = "sG";
const std::string ConfigurationTags::storeGraphicN                       = "Store Graphic";
// No longer used as of 9-19-17- claim back later - const std::string ConfigurationTags::deptName                            = "Dn";
// No longer used as of 9-19-17- claim back later - const std::string ConfigurationTags::deptNameN                           = "Department Name";

const std::string ConfigurationTags::primaryCountryCode                  = "C1";
const std::string ConfigurationTags::primaryCountryCodeN                 = "Primary Country Code";
const std::string ConfigurationTags::primaryLanguageCode                 = "P1";
const std::string ConfigurationTags::primaryLanguageCodeN                = "Primary Language Code";
const std::string ConfigurationTags::secondaryCountryCode                = "C2";
const std::string ConfigurationTags::secondaryCountryCodeN               = "Second Country Code";
const std::string ConfigurationTags::secondaryLanguageCode               = "P2";
const std::string ConfigurationTags::secondaryLanguageCodeN              = "Second Language Code";
const std::string ConfigurationTags::lengthUnits                         = "Lu";		//	Changed because of overlap with LEAP_Username_TAG
const std::string ConfigurationTags::lengthUnitsN                        = "Length Units";
const std::string ConfigurationTags::temperatureUnits                    = "tu";
const std::string ConfigurationTags::temperatureUnitsN                   = "Temperature Units";
const std::string ConfigurationTags::dateFormat                          = "df"; 
const std::string ConfigurationTags::dateFormatN                         = "Date Format"; 
const std::string ConfigurationTags::timeFormat                          = "Tf";
const std::string ConfigurationTags::timeFormatN                         = "Time Format";

//const std::string ConfigurationTags::pricePrecision                      = "td";
//const std::string ConfigurationTags::pricePrecisionN                     = "Price Precision";
const std::string ConfigurationTags::unitPriceSecurityLabel              = "AU";
const std::string ConfigurationTags::unitPriceSecurityLabelN             = "Minimum Unit Price for Security Label";
const std::string ConfigurationTags::totalPriceSecurityLabel             = "AT";
const std::string ConfigurationTags::totalPriceSecurityLabelN            = "Minimum Total Price for Security Label";
const std::string ConfigurationTags::fractionalPricingEnabled            = "fp";
const std::string ConfigurationTags::fractionalPricingEnabledN           = "Fractional Pricing Enabled";
const std::string ConfigurationTags::poundsForPricingEnabled             = "lp";
const std::string ConfigurationTags::poundsForPricingEnabledN            = "Pounds for Pricing Enabled";
const std::string ConfigurationTags::bycountCalcMethod                   = "bM";
const std::string ConfigurationTags::bycountCalcMethodN                  = "By Count Calculation Method";
const std::string ConfigurationTags::clearPluAfterPrinting               = "cP";
const std::string ConfigurationTags::clearPluAfterPrintingN              = "Clear Plu After Printing";

const std::string ConfigurationTags::isSoundBeepStableWeight             = "sb";
const std::string ConfigurationTags::isSoundBeepStableWeightN            = "Beep on Stable Weight";
const std::string ConfigurationTags::beeperVolumeLevel                   = "Bv";
const std::string ConfigurationTags::beeperVolumeLevelN                  = "Beeper Volume Level";

const std::string ConfigurationTags::fixedWeightUnits                    = "fT";
const std::string ConfigurationTags::fixedWeightUnitsN                   = "Fixed Weight Units";
const std::string ConfigurationTags::weigherAttached                     = "wa";
const std::string ConfigurationTags::weigherAttachedN                    = "Weigher Attached";
const std::string ConfigurationTags::tareSetback                         = "ts";
const std::string ConfigurationTags::tareSetbackN                        = "Tare Setback";
const std::string ConfigurationTags::maxTare                             = "mT";
const std::string ConfigurationTags::maxTareN                            = "Maximum Tare";
const std::string ConfigurationTags::platterTareEnabled                  = "pe";
const std::string ConfigurationTags::platterTareEnabledN                 = "Platter Tare Enabled";
const std::string ConfigurationTags::changeZeroTareOnlyOnce              = "zo";
const std::string ConfigurationTags::changeZeroTareOnlyOnceN             = "Change Zero Tare only once";
const std::string ConfigurationTags::changeForcedTareOnlyOnce            = "fo";
const std::string ConfigurationTags::changeForcedTareOnlyOnceN           = "Change Forced Tare only once";
const std::string ConfigurationTags::minWeightToPrint                    = "lb";
const std::string ConfigurationTags::minWeightToPrintN                   = "Minimum Weight to Print";
const std::string ConfigurationTags::scaleWeighMode                      = "pk";
const std::string ConfigurationTags::scaleWeighModeN                     = "Weigh Mode";
const std::string ConfigurationTags::weigherModel                        = "wm";
const std::string ConfigurationTags::weigherModelN                       = "Weigher Model";
const std::string ConfigurationTags::scaleMetricN                        = "Metric System";
const std::string ConfigurationTags::netWeightDualRangeLimitCheck        = "t6";
const std::string ConfigurationTags::netWeightDualRangeLimitCheckN       = "Enable 12lb Net Weight Threshold";
const std::string ConfigurationTags::externalWeightPrecision             = "Wp";
const std::string ConfigurationTags::externalWeightPrecisionN            = "External Weight Precision";
const std::string ConfigurationTags::tareEntryPrecision                  = "Tp";
const std::string ConfigurationTags::tareEntryPrecisionN                 = "Tare Entry Precision";
const std::string ConfigurationTags::userRezeroWeightLimit				 = "rz";
const std::string ConfigurationTags::userRezeroWeightLimitN				 = "User Rezero Weight Limit";
const std::string ConfigurationTags::weigherWakeupSensorAttached		 = "ws";
const std::string ConfigurationTags::weigherWakeupSensorAttachedN		 = "Weigher wakeup Sensor Attached";

const std::string ConfigurationTags::valueMaxEnabled                     = "vM";
const std::string ConfigurationTags::valueMaxEnabledN                    = "Value Max Enabled";
const std::string ConfigurationTags::valueMaxDiagCollectionDisabled      = "VD";
const std::string ConfigurationTags::valueMaxDiagCollectionDisabledN     = "Value Max Diagnostics Collection Disabled";

const std::string ConfigurationTags::loginRequired                       = "os";
const std::string ConfigurationTags::loginRequiredN                      = "Operator Login required";
const std::string ConfigurationTags::supervisorloginRequiredN            = "Supervisor Login required";
const std::string ConfigurationTags::inactivityAutoLogoutTime            = "IT";
const std::string ConfigurationTags::inactivityAutoLogoutTimeN           = "Remote Auto-Logout Time";
const std::string ConfigurationTags::autoLookupProduct                   = "al";
const std::string ConfigurationTags::autoLookupProductN                  = "Auto-Lookup Enabled";
const std::string ConfigurationTags::autoLookupNumDigits                 = "ld";
const std::string ConfigurationTags::autoLookupNumDigitsN                = "Auto-Lookup Digits";
const std::string ConfigurationTags::autoLookupOverrideDigit             = "Sd";
const std::string ConfigurationTags::autoLookupOverrideDigitN            = "Auto-Lookup Override Digit";
const std::string ConfigurationTags::autoDisplayProductNotes             = "DP";
const std::string ConfigurationTags::autoDisplayProductNotesN            = "Auto-Display Product Notes";

const std::string ConfigurationTags::autoDisplaySuggestiveSelling		 = "Ds";
const std::string ConfigurationTags::autoDisplaySuggestiveSellingN		 = "Auto-Display Suggestive Selling";
const std::string ConfigurationTags::autoDisplayCookingNotes			 = "Dc";
const std::string ConfigurationTags::autoDisplayCookingNotesN			 = "Auto-Display Cooking Notes";

const std::string ConfigurationTags::horizontalLabelSkew1                = "1s";
const std::string ConfigurationTags::horizontalLabelSkew1N               = "Horizontal Label1 Skew";
const std::string ConfigurationTags::horizontalLabelSkew2                = "2s";
const std::string ConfigurationTags::horizontalLabelSkew2N               = "Horizontal Label2 Skew";
const std::string ConfigurationTags::horizontalLabelSkew3                = "3s";
const std::string ConfigurationTags::horizontalLabelSkew3N               = "Horizontal Label3 Skew";

const std::string ConfigurationTags::isCollectingTransactions            = "Tt";
const std::string ConfigurationTags::isCollectingTransactionsN           = "Collect Transactions";
const std::string ConfigurationTags::collectingTransactionsDays          = "Td";
const std::string ConfigurationTags::collectingTransactionsDaysN         = "Collect Transactions Days";
const std::string ConfigurationTags::isCollectingProductTotals           = "pT";
const std::string ConfigurationTags::isCollectingProductTotalsN          = "Collect Product Totals";
const std::string ConfigurationTags::isCollectingHourlyTotals            = "ht";
const std::string ConfigurationTags::isCollectingHourlyTotalsN           = "Collect Hourly Totals";
const std::string ConfigurationTags::isCollectingOperatorTotals          = "ot";
const std::string ConfigurationTags::isCollectingOperatorTotalsN         = "Collect Operator Totals";
const std::string ConfigurationTags::trackingOperatorChanges			 = "TC";
const std::string ConfigurationTags::trackingOperatorChangesN			 = "Track Operator Changes";
const std::string ConfigurationTags::trackingSupervisorChanges			 = "TS";
const std::string ConfigurationTags::trackingSupervisorChangesN			 = "Track Supervisor Changes";
//const std::string ConfigurationTags::daysToKeepOperatorChanges           = "od";
//const std::string ConfigurationTags::daysToKeepOperatorChangesN          = "Days To Keep OpChanges";
const std::string ConfigurationTags::collectingCOOLTotals                = "ce";
const std::string ConfigurationTags::collectingCOOLTotalsN               = "Collect COOL Totals";

const std::string ConfigurationTags::opAccessToBrowserMode               = "AB";
const std::string ConfigurationTags::opAccessToBrowserModeN              = "Operator Access to Browser Mode";
const std::string ConfigurationTags::opAccessToDesktopMode               = "AD";
const std::string ConfigurationTags::opAccessToDesktopModeN              = "Operator Access to Desktop Mode";

const std::string ConfigurationTags::autoLoadOldFirmwareVersions         = "OF";
const std::string ConfigurationTags::autoLoadOldFirmwareVersionsN        = "Auto-Load old Firmware";
const std::string ConfigurationTags::autoSearchNewFirmwareVersions       = "SF";
const std::string ConfigurationTags::autoSearchNewFirmwareVersionsN      = "Auto-Search New Firmware";
const std::string ConfigurationTags::newFirmwareVersionSearchTime        = "ST";
const std::string ConfigurationTags::newFirmwareVersionSearchTimeN       = "Daily Firmware Search Time";
const std::string ConfigurationTags::forceUniqueProdClassNums            = "UN";
const std::string ConfigurationTags::forceUniqueProdClassNumsN           = "Unique Prod/Class Numbers";
const std::string ConfigurationTags::enablePowerSaverMode                = "PS";
const std::string ConfigurationTags::enablePowerSaverModeN               = "Power Saver Mode";
const std::string ConfigurationTags::powerSaverModeTimeout               = "PT";
const std::string ConfigurationTags::powerSaverModeTimeoutN              = "Power Saver Mode Timeout";
const std::string ConfigurationTags::defaultBrowserHomePage              = "BH";
const std::string ConfigurationTags::defaultBrowserHomePageN             = "Browser Home Page";
const std::string ConfigurationTags::defaultRunScreenHomePage			 = "RH";
const std::string ConfigurationTags::defaultRunScreenHomePageN			 = "RunScreen Home Page";
const std::string ConfigurationTags::defaultProductRecordType            = "Pt";
const std::string ConfigurationTags::defaultProductRecordTypeN           = "Default Product Type";

const std::string ConfigurationTags::defWrFilmWidth                      = "FW";
const std::string ConfigurationTags::defWrFilmWidthN                     = "Default Film Width";
const std::string ConfigurationTags::defWrFilmTension                    = "FT";
const std::string ConfigurationTags::defWrFilmTensionN                   = "Default Film Tension";
const std::string ConfigurationTags::defWrFilmTensionFrontToBack		 = "tF";
const std::string ConfigurationTags::defWrFilmTensionFrontToBackN		 = "Default Film Tension Front To Back";
const std::string ConfigurationTags::defWrFilmTensionLeftToRight		 = "tL";
const std::string ConfigurationTags::defWrFilmTensionLeftToRightN		 = "Default Film Tension Left To Right";
const std::string ConfigurationTags::defWrIntakeMode                     = "IM";
const std::string ConfigurationTags::defWrIntakeModeN                    = "Default Intake Mode";
const std::string ConfigurationTags::defWrLabelPlacement                 = "LP";
const std::string ConfigurationTags::defWrLabelPlacementN                = "Default Label Placement";
const std::string ConfigurationTags::defWrApplierRotation                = "LR";
const std::string ConfigurationTags::defWrApplierRotationN               = "Default Applier Rotation";
const std::string ConfigurationTags::defWrSecLabelPlacement				 = "Lp";
const std::string ConfigurationTags::defWrSecLabelPlacementN			 = "Default Secondary Label Placement";
const std::string ConfigurationTags::defWrSecApplierRotation			 = "Lr";
const std::string ConfigurationTags::defWrSecApplierRotationN			 = "Default Applier Secondary Rotation";
const std::string ConfigurationTags::wrSealerBeltTimeOnSun               = "B0";
const std::string ConfigurationTags::wrSealerBeltTimeOnSunN              = "Sealer Belt Time on Sunday";
const std::string ConfigurationTags::wrSealerBeltTimeOffSun              = "b0";
const std::string ConfigurationTags::wrSealerBeltTimeOffSunN             = "Sealer Belt Time off Sunday";
const std::string ConfigurationTags::wrSealerBeltTimeOnMon               = "B1";
const std::string ConfigurationTags::wrSealerBeltTimeOnMonN              = "Sealer Belt Time on Monday";
const std::string ConfigurationTags::wrSealerBeltTimeOffMon              = "b1";
const std::string ConfigurationTags::wrSealerBeltTimeOffMonN             = "Sealer Belt Time off Monday";
const std::string ConfigurationTags::wrSealerBeltTimeOnTue               = "B2";
const std::string ConfigurationTags::wrSealerBeltTimeOnTueN              = "Sealer Belt Time on Tuesday";
const std::string ConfigurationTags::wrSealerBeltTimeOffTue              = "b2";
const std::string ConfigurationTags::wrSealerBeltTimeOffTueN             = "Sealer Belt Time off Tuesday";
const std::string ConfigurationTags::wrSealerBeltTimeOnWed               = "B3";
const std::string ConfigurationTags::wrSealerBeltTimeOnWedN              = "Sealer Belt Time on Wednesday";
const std::string ConfigurationTags::wrSealerBeltTimeOffWed              = "b3";
const std::string ConfigurationTags::wrSealerBeltTimeOffWedN             = "Sealer Belt Time off Wednesday";
const std::string ConfigurationTags::wrSealerBeltTimeOnThu               = "B4";
const std::string ConfigurationTags::wrSealerBeltTimeOnThuN              = "Sealer Belt Time on Thursday";
const std::string ConfigurationTags::wrSealerBeltTimeOffThu              = "b4";
const std::string ConfigurationTags::wrSealerBeltTimeOffThuN             = "Sealer Belt Time off Thursday";
const std::string ConfigurationTags::wrSealerBeltTimeOnFri               = "B5";
const std::string ConfigurationTags::wrSealerBeltTimeOnFriN              = "Sealer Belt Time on Friday";
const std::string ConfigurationTags::wrSealerBeltTimeOffFri              = "b5";
const std::string ConfigurationTags::wrSealerBeltTimeOffFriN             = "Sealer Belt Time off Friday";
const std::string ConfigurationTags::wrSealerBeltTimeOnSat               = "B6";
const std::string ConfigurationTags::wrSealerBeltTimeOnSatN              = "Sealer Belt Time on Saturday";
const std::string ConfigurationTags::wrSealerBeltTimeOffSat              = "b6";
const std::string ConfigurationTags::wrSealerBeltTimeOffSatN             = "Sealer Belt Time off Saturday";

const std::string ConfigurationTags::diskWriteFrequency                  = "Dw";
const std::string ConfigurationTags::diskWriteFrequencyN                 = "Database Backup Frequency";
const std::string ConfigurationTags::autoCompressDB                      = "CD";
const std::string ConfigurationTags::autoCompressDBN                     = "Auto-Compress Database";
const std::string ConfigurationTags::compressDBTime                      = "CT";
const std::string ConfigurationTags::compressDBTimeN                     = "Compress Database Time";

const std::string ConfigurationTags::coolTrackingNumberEnabled           = "cT";
const std::string ConfigurationTags::coolTrackingNumberEnabledN          = "COOL Tracking Number Enabled";
const std::string ConfigurationTags::dontChangeCurrentCOOL				 = "dC";
const std::string ConfigurationTags::dontChangeCurrentCOOLN				 = "Don't change Current COOL via Comm";

const std::string ConfigurationTags::primaryExpirationPageNote           = "EN";
const std::string ConfigurationTags::primaryExpirationPageNoteN          = "Primary Expiration Page Note";
const std::string ConfigurationTags::secondExpirationPageNote            = "EO";
const std::string ConfigurationTags::secondExpirationPageNoteN           = "Second Expiration Page Note";

const std::string ConfigurationTags::checkingNetworkConnectivity         = "Cn";
const std::string ConfigurationTags::checkingNetworkConnectivityN        = "Check for Network Connectivity";
const std::string ConfigurationTags::offlineDateTime					 = "Ot";
const std::string ConfigurationTags::offlineDateTimeN					 = "Offline Date/Time";

const std::string ConfigurationTags::scaleType                           = "sT";
const std::string ConfigurationTags::scaleTypeN                          = "Scale Type";

const std::string ConfigurationTags::communicationHistory                = "CH";
const std::string ConfigurationTags::communicationHistoryN               = "Communication History";
const std::string ConfigurationTags::communicationItemReceived           = "CI";
const std::string ConfigurationTags::communicationItemReceivedN          = "Communication Item Received";

const std::string ConfigurationTags::popupKeypadEnabled                  = "PK";
const std::string ConfigurationTags::popupKeypadEnabledN                 = "Popup Keypad Enabled";
const std::string ConfigurationTags::popupKeypadStyle                    = "ks";
const std::string ConfigurationTags::popupKeypadStyleN                   = "Popup Keypad Style";
const std::string ConfigurationTags::popupKeypadLocation				 = "kL";
const std::string ConfigurationTags::popupKeypadLocationN				 = "Popup Keypad Location";

const std::string ConfigurationTags::operatorLoginKeyboardStyle          = "kl";
const std::string ConfigurationTags::operatorLoginKeyboardStyleN         = "Operator Login Keyboard Style";

const std::string ConfigurationTags::capacityBarLocation				 = "CL";
const std::string ConfigurationTags::capacityBarLocationN				 = "Capacity Bar Location";

const std::string ConfigurationTags::pingPrimary				         = "PR";
const std::string ConfigurationTags::pingPrimaryN				         = "Ping the Primary Scale";
const std::string ConfigurationTags::pingGateway				         = "PG";
const std::string ConfigurationTags::pingGatewayN				         = "Ping the Default Gateway";
const std::string ConfigurationTags::pingTarget					         = "pA";
const std::string ConfigurationTags::pingTargetN				         = "Ping target address";
const std::string ConfigurationTags::pingInterval						 = "PI";
const std::string ConfigurationTags::pingIntervalN						 = "Ping Interval";
const std::string ConfigurationTags::pingTargetIPAddress				 = "PA";
const std::string ConfigurationTags::pingTargetIPAddressN				 = "Ping Target IP Address";

const std::string ConfigurationTags::rebootUnprotectedAllowed            = "ru";
const std::string ConfigurationTags::rebootUnprotectedAllowedN           = "Reboot Unprotected Allowed";

const std::string ConfigurationTags::autoCreateRFIDOperator              = "oR";
const std::string ConfigurationTags::autoCreateRFIDOperatorN             = "Auto Create RFID Operator";

const std::string ConfigurationTags::firmwareVersion					 = "VE";
const std::string ConfigurationTags::firmwareVersionN					 = "Firmware Version";
const std::string ConfigurationTags::firmwareRevision					 = "Re";
const std::string ConfigurationTags::firmwareRevisionN					 = "Firmware Revision";

const std::string ConfigurationTags::ignoreUltimaFormatLabelTypes		 = "iU";
const std::string ConfigurationTags::ignoreUltimaFormatLabelTypesN		 = "Ignore Ultima/Quantum Labels";

const std::string ConfigurationTags::reportsOffsetTop					 = "Rt";
const std::string ConfigurationTags::reportsOffsetTopN					 = "Reports Offset Top";
const std::string ConfigurationTags::reportsOffsetBottom				 = "Rb";
const std::string ConfigurationTags::reportsOffsetBottomN				 = "Reports Offset Bottom";

const std::string ConfigurationTags::showDeleteAllFiles					 = "s1";
const std::string ConfigurationTags::showDeleteAllFilesN				 = "Show Delete All Files";
const std::string ConfigurationTags::showPrinterSettings				 = "s2";
const std::string ConfigurationTags::showPrinterSettingsN				 = "Show Printer Settings";

const std::string ConfigurationTags::remoteAccessPermitted				 = "ra";
const std::string ConfigurationTags::remoteAccessPermittedN				 = "Remote Access Permitted";
const std::string ConfigurationTags::remoteAccessRequiresLogin			 = "rl";
const std::string ConfigurationTags::remoteAccessRequiresLoginN			 = "Remote Access Requires Login";
const std::string ConfigurationTags::remoteAccessLoginMethod			 = "rL";
const std::string ConfigurationTags::remoteAccessLoginMethodN			 = "Remote Access Login Method";
const std::string ConfigurationTags::remoteAccessUsername				 = "rU";
const std::string ConfigurationTags::remoteAccessUsernameN				 = "Remote Access Username";
const std::string ConfigurationTags::remoteAccessPassword				 = "rP";
const std::string ConfigurationTags::remoteAccessPasswordN				 = "Remote Access Password";

const std::string ConfigurationTags::backdateByDays						 = "bD";
const std::string ConfigurationTags::backdateByDaysN					 = "Backdate By Days";

// Is this the same as ConfigurationTags::CurrencyDecimalSeparator???
const std::string ConfigurationTags::unitSeparator						 = "??";
const std::string ConfigurationTags::unitSeparatorN						 = "Unit Separator";

const std::string ConfigurationTags::systemFont							 = "sF";
const std::string ConfigurationTags::systemFontN						 = "System Font";
const std::string ConfigurationTags::startScreenHome					 = "sh";
const std::string ConfigurationTags::startScreenHomeN					 = "Start Screen Home";
const std::string ConfigurationTags::browserHasAddressBar				 = "ba";
const std::string ConfigurationTags::browserHasAddressBarN				 = "Browser Has Address Bar";
const std::string ConfigurationTags::defaultSearchMethod				 = "mS";
const std::string ConfigurationTags::defaultSearchMethodN				 = "Default Search Method";
const std::string ConfigurationTags::resetStatisticsDays				 = "sD";
const std::string ConfigurationTags::resetStatisticsDaysN				 = "Reset Statistics Days";

const std::string ConfigurationTags::leftRunscreenContent				= "Rl";
const std::string ConfigurationTags::leftRunscreenContentN				= "Left Runscreen Content";
const std::string ConfigurationTags::runscreenDynamicContent			= "Rd";
const std::string ConfigurationTags::runscreenDynamicContentN			= "Runscreen Dynamic Content";
const std::string ConfigurationTags::runscreenKeypadContent				= "Rk";
const std::string ConfigurationTags::runscreenKeypadContentN			= "Runscreen Keypad Content";
const std::string ConfigurationTags::rightScreenContent					= "Rr";
const std::string ConfigurationTags::rightScreenContentN				= "Right Screen Content";

const std::string ConfigurationTags::enableDisplayNetworkDetails        = "nd";
const std::string ConfigurationTags::enableDisplayNetworkDetailsN       = "Enable Display Network Details";
const std::string ConfigurationTags::remoteScaleMaintenanceEnabled		 = "RM";
const std::string ConfigurationTags::remoteScaleMaintenanceEnabledN		 = "Remote Scale Maintenance Enabled";
const std::string ConfigurationTags::remoteScaleMaintClrOutboxDays		 = "rd";
const std::string ConfigurationTags::remoteScaleMaintClrOutboxDaysN		 = "Days to clear the Outbox Directory";

const std::string ConfigurationTags::disableOperatorFlashkeyEditor      = "Of";
const std::string ConfigurationTags::disableOperatorFlashkeyEditorN     = "Disable Operator Flashkey Editor";
const std::string ConfigurationTags::disableLeftRunscreen               = "RL";
const std::string ConfigurationTags::disableLeftRunscreenN              = "Disable Left Runscreen";

const std::string ConfigurationTags::mostRecentRunMode                  = "rM";		// This is here for upgrade only, not for comm or backup/restore
const std::string ConfigurationTags::mostRecentRunModeN                 = "Most Recent Run Mode";

const std::string ConfigurationTags::displayTransTypeIndicator          = "iT";
const std::string ConfigurationTags::displayTransTypeIndicatorN         = "Display TransType Indicator";
const std::string ConfigurationTags::nowServingQueueIPAddress           = "nQ";
const std::string ConfigurationTags::nowServingQueueIPAddressN          = "Now serving queue IP address";
const std::string ConfigurationTags::nowServingSysType                  = "nT";
const std::string ConfigurationTags::nowServingSysTypeN                 = "Now serving system type";

//GCD
const std::string ConfigurationTags::customerDisplay                     = "gd";
const std::string ConfigurationTags::customerDisplayN                    = "Customer Display";

const std::string ConfigurationTags::gcdLayoutNumber					 = "GL";
const std::string ConfigurationTags::gcdLayoutNumberN					 = "GCD Layout Number";
const std::string ConfigurationTags::gcdColorSchemeNumber				 = "GO";
const std::string ConfigurationTags::gcdColorSchemeNumberN				 = "GCD Color Scheme Number";
const std::string ConfigurationTags::gcdAllowOperReset					 = "GR";
const std::string ConfigurationTags::gcdAllowOperResetN					 = "GCD Allow Operator Reset";

const std::string ConfigurationTags::gcdVolume					= "Gv";
const std::string ConfigurationTags::gcdVolumeN					= "GCD Volume Setting";
const std::string ConfigurationTags::gcdPlaybackWaitTime		= "Gw";
const std::string ConfigurationTags::gcdPlaybackWaitTimeN		= "GCD Idle Timeout";
const std::string ConfigurationTags::gcdMediaServerName			= "Gx";
const std::string ConfigurationTags::gcdMediaServerNameN		= "GCD Media Server Name";
const std::string ConfigurationTags::gcdMediaServerIP			= "Gi";
const std::string ConfigurationTags::gcdMediaServerNamIPN		= "GCD Media Server IP Address";
const std::string ConfigurationTags::gcdFTPUserName				= "Gu";
const std::string ConfigurationTags::gcdFTPUserNameN			= "GCD FTP User Name";
const std::string ConfigurationTags::gcdFTPPassword				= "Gp";
const std::string ConfigurationTags::gcdFTPPasswordN			= "GCD FTP Password";
const std::string ConfigurationTags::gcdFTPDirectory			= "Ge";
const std::string ConfigurationTags::gcdFTPDirectoryN			= "GCD Remote FTP Directory";
const std::string ConfigurationTags::gcdPlaylistFilename		= "Gf";
const std::string ConfigurationTags::gcdPlaylistFilenameN		= "GCD Play List File Name";
const std::string ConfigurationTags::gcdAutoSearch				= "Ga";
const std::string ConfigurationTags::gcdAutoSearchN				= "GCD Auto Search Flag";
const std::string ConfigurationTags::gcdSearchHour				= "Gg";
const std::string ConfigurationTags::gcdSearchHourN				= "GCD Auto Search Hour";
const std::string ConfigurationTags::gcdSearchMinute			= "Gm";
const std::string ConfigurationTags::gcdSearchMinuteN			= "GCD Auto Search Minute";
const std::string ConfigurationTags::gcdAutoAMPM				= "Gb";
const std::string ConfigurationTags::gcdAutoAMPMN				= "GCD Auto Search AM or PM";
const std::string ConfigurationTags::gcdDnsEnabled				= "Gz";
const std::string ConfigurationTags::gcdDnsEnabledN				= "GCD Use DNS To Resolve IP";
const std::string ConfigurationTags::gcdFTPProtocol				= "A4";
const std::string ConfigurationTags::gcdFTPProtocolN			= "GCD Media Update Protocol";
const std::string ConfigurationTags::gcdLanguage				= "GI";
const std::string ConfigurationTags::gcdLanguageN				= "GCD Language";
const std::string ConfigurationTags::printWithoutCustomerDisplay  = "gP";
const std::string ConfigurationTags::printWithoutCustomerDisplayN = "Print Without Customer Display";

const std::string ConfigurationTags::gcdUseTransactionalLayout		= "GX";
const std::string ConfigurationTags::gcdUseTransactionalLayoutN		= "GCD Use Transactional Layout";
const std::string ConfigurationTags::gcdIdleAdvertType				= "GA";
const std::string ConfigurationTags::gcdIdleAdvertTypeN				= "GCD Idle Advertisements Type";
const std::string ConfigurationTags::gcdIdleWebsiteAddress			= "GH";
const std::string ConfigurationTags::gcdIdleWebsiteAddressN			= "GCD Idle Website Address";
const std::string ConfigurationTags::gcdProductAdvertType			= "GV";
const std::string ConfigurationTags::gcdProductAdvertTypeN			= "GCD Product Advertisements Type";

//  CfgFrequentShopper
const std::string ConfigurationTags::discountMethod                      = "d$";
const std::string ConfigurationTags::discountMethodN                     = "Discount Method";
const std::string ConfigurationTags::frequentShopperRoundingMethod       = "fr";
const std::string ConfigurationTags::frequentShopperRoundingMethodN      = "Rounding Method";
const std::string ConfigurationTags::discountType                        = "dT";
const std::string ConfigurationTags::discountTypeN                       = "Default Discount Type";
const std::string ConfigurationTags::discountFormula                     = "DF";
const std::string ConfigurationTags::discountFormulaN                    = "Default Discount Formula";
const std::string ConfigurationTags::frequentShopperPerScaleN            = "Per Scale";
const std::string ConfigurationTags::totalPriceText1                     = "f1";
const std::string ConfigurationTags::totalPriceText1N                    = "Total Price Text 1";
const std::string ConfigurationTags::totalPriceText2                     = "f2";
const std::string ConfigurationTags::totalPriceText2N                    = "Total Price Text 2";
const std::string ConfigurationTags::savingsText1                        = "f3";
const std::string ConfigurationTags::savingsText1N                       = "Savings Text 1";
const std::string ConfigurationTags::savingsText2                        = "f4";
const std::string ConfigurationTags::savingsText2N                       = "Savings Text 2";
const std::string ConfigurationTags::unitPriceText1                      = "f5";
const std::string ConfigurationTags::unitPriceText1N                     = "Unit Price Text 1";
const std::string ConfigurationTags::unitPriceText2                      = "f6";
const std::string ConfigurationTags::unitPriceText2N                     = "Unit Price Text 2";
const std::string ConfigurationTags::fsRoundingWeightPrecision           = "fP";
const std::string ConfigurationTags::fsRoundingWeightPrecisionN          = "FS Rounding Weight Precision";

//  CfgOperatorAccess
const std::string ConfigurationTags::priceModifiable                     = "$m";
const std::string ConfigurationTags::priceModifiableN                    = "Price";
const std::string ConfigurationTags::byCountModifiable                   = "bm";
const std::string ConfigurationTags::byCountModifiableN                  = "By Count";
const std::string ConfigurationTags::tareModifiable                      = "tm";
const std::string ConfigurationTags::tareModifiableN                     = "Tare";
const std::string ConfigurationTags::fixedWeightModifiable               = "mf";
const std::string ConfigurationTags::fixedWeightModifiableN              = "Fixed Weight";
const std::string ConfigurationTags::shelfLifeModifiable                 = "ms";
const std::string ConfigurationTags::shelfLifeModifiableN                = "Shelf Life";
const std::string ConfigurationTags::productLifeModifiable               = "mL";
const std::string ConfigurationTags::productLifeModifiableN              = "Product Life";
const std::string ConfigurationTags::labelTypeModifiable                 = "lt";
const std::string ConfigurationTags::labelTypeModifiableN                = "Label Type";
const std::string ConfigurationTags::dateTimeModifiable                  = "md";
const std::string ConfigurationTags::dateTimeModifiableN                 = "Date/Time";
const std::string ConfigurationTags::portionsModifiable                  = "po";
const std::string ConfigurationTags::portionsModifiableN                 = "Portions";

const std::string ConfigurationTags::priceAccessLevel                    = "AP";
const std::string ConfigurationTags::priceAccessLevelN                   = "Price Access Level";
const std::string ConfigurationTags::byCountAccessLevel                  = "Ab";
const std::string ConfigurationTags::byCountAccessLevelN                 = "By Count Access Level";
const std::string ConfigurationTags::tareAccessLevel                     = "At";
const std::string ConfigurationTags::tareAccessLevelN                    = "Tare Access Level";
const std::string ConfigurationTags::fixedWeightAccessLevel              = "Aw";
const std::string ConfigurationTags::fixedWeightAccessLevelN             = "Fixed Weight Access Level";
const std::string ConfigurationTags::shelfLifeAccessLevel                = "As";
const std::string ConfigurationTags::shelfLifeAccessLevelN               = "Shelf Life Access Level";
const std::string ConfigurationTags::productLifeAccessLevel              = "Ap";
const std::string ConfigurationTags::productLifeAccessLevelN             = "Product Life Access Level";
const std::string ConfigurationTags::labelTypeAccessLevel                = "AL";
const std::string ConfigurationTags::labelTypeAccessLevelN               = "Label Type Access Level";
const std::string ConfigurationTags::dateTimeAccessLevel                 = "ad";
const std::string ConfigurationTags::dateTimeAccessLevelN                = "Date/Time Access Level";
const std::string ConfigurationTags::portionsAccessLevel                 = "AR";
const std::string ConfigurationTags::portionsAccessLevelN                = "Portions Access Level";

//  CfgPrintedFields
const std::string ConfigurationTags::sellByDatePrinted                   = "pS";
const std::string ConfigurationTags::sellByDatePrintedN                  = "Sell-by Date";
const std::string ConfigurationTags::sellByDateJulian                    = "js";
const std::string ConfigurationTags::sellByDateJulianN                   = "Julian Sell-by Date";
const std::string ConfigurationTags::packedOnDatePrinted                 = "pp";
const std::string ConfigurationTags::packedOnDatePrintedN                = "Packed-on Date";
const std::string ConfigurationTags::packedOnDateJulian                  = "jp";
const std::string ConfigurationTags::packedOnDateJulianN                 = "Julian Packed-on Date";
const std::string ConfigurationTags::productLifeDatePrinted              = "pL";
const std::string ConfigurationTags::productLifeDatePrintedN             = "Product Life Date";
const std::string ConfigurationTags::productLifeDateJulian               = "jl";
const std::string ConfigurationTags::productLifeDateJulianN              = "Julian Product Life Date";
const std::string ConfigurationTags::unitPricePrinted                    = "p$";
const std::string ConfigurationTags::unitPricePrintedN                   = "Unit Price";
const std::string ConfigurationTags::byCountPrinted                      = "pb";
const std::string ConfigurationTags::byCountPrintedN                     = "By Count";
const std::string ConfigurationTags::barCodePrinted                      = "pu";
const std::string ConfigurationTags::barCodePrintedN                     = "Barcode";
const std::string ConfigurationTags::currencySymbolPrinted               = "U$";
const std::string ConfigurationTags::currencySymbolPrintedN              = "Currency Symbol";
const std::string ConfigurationTags::textNamesPreprinted                 = "pg";
const std::string ConfigurationTags::textNamesPreprintedN                = "Pre-printed Text Names";
const std::string ConfigurationTags::weightOnByCountPrinted              = "wb";
const std::string ConfigurationTags::weightOnByCountPrintedN             = "Weight on By Count";
const std::string ConfigurationTags::oneOnByCountPrinted				 = "Pf";
const std::string ConfigurationTags::oneOnByCountPrintedN				 = "Print 1 on By count";
const std::string ConfigurationTags::usingSecondCheckDigit               = "P#";
const std::string ConfigurationTags::usingSecondCheckDigitN              = "Second Check Digit";
const std::string ConfigurationTags::printFirstLetterOfTotalType		 = "fL";
const std::string ConfigurationTags::printFirstLetterOfTotalTypeN		 = "Print first letter of TotalType";
const std::string ConfigurationTags::alwaysPrintNetWtOnFixedWt			 = "NW";
const std::string ConfigurationTags::alwaysPrintNetWtOnFixedWtN			 = "Always print NET WT";
const std::string ConfigurationTags::fixedWeightFormat					 = "FF";
const std::string ConfigurationTags::fixedWeightFormatN					 = "Fixed weight format";

// CfgSelfService
const std::string ConfigurationTags::gridXElements                       = "gx";
const std::string ConfigurationTags::gridXElementsN                      = "Grid X Elements";
const std::string ConfigurationTags::gridYElements                       = "gy";
const std::string ConfigurationTags::gridYElementsN                      = "Grid Y Elements";
const std::string ConfigurationTags::numOfMostCommonCategories           = "#M";
const std::string ConfigurationTags::numOfMostCommonCategoriesN          = "Most Common Categories";
const std::string ConfigurationTags::imageTypeOnStartScreen              = "It";
const std::string ConfigurationTags::imageTypeOnStartScreenN             = "Start Screen Image Type";
const std::string ConfigurationTags::initialStreamingGraphicNum          = "SI";
const std::string ConfigurationTags::initialStreamingGraphicNumN         = "Initial Slide Show Graphic";
const std::string ConfigurationTags::productStatsHoursMethod             = "sM";
const std::string ConfigurationTags::productStatsHoursMethodN            = "Accum. Statistics Method";
const std::string ConfigurationTags::selfServeInactivityTimeout          = "IS";
const std::string ConfigurationTags::selfServeInactivityTimeoutN         = "Inactivity Timeout";

const std::string ConfigurationTags::logoGraphicFileName                 = "";
const std::string ConfigurationTags::logoGraphicFileNameN                = "Logo Graphic Filename";
const std::string ConfigurationTags::noGraphicAvailableFileName1         = "I1";
const std::string ConfigurationTags::noGraphicAvailableFileName1N        = "No-Image Graphic 1";
const std::string ConfigurationTags::noGraphicAvailableFileName2         = "I2";
const std::string ConfigurationTags::noGraphicAvailableFileName2N        = "No-Image Graphic 2";
const std::string ConfigurationTags::previousGraphicFileName1            = "Ir";
const std::string ConfigurationTags::previousGraphicFileName1N           = "Previous Graphic 1";
const std::string ConfigurationTags::previousGraphicFileName2            = "IR";
const std::string ConfigurationTags::previousGraphicFileName2N           = "Previous Graphic 2";
const std::string ConfigurationTags::nextGraphicFileName1                = "In";
const std::string ConfigurationTags::nextGraphicFileName1N               = "Next Graphic Filename 1";
const std::string ConfigurationTags::nextGraphicFileName2                = "IN";
const std::string ConfigurationTags::nextGraphicFileName2N               = "Next Graphic Filename 2";


//  CfgNetwork

const std::string ConfigurationTags::ethernetInterfaceName               = "En";
const std::string ConfigurationTags::ethernetInterfaceNameN              = "Ethernet Interface";
const std::string ConfigurationTags::macAddressN                         = "MAC Address";
const std::string ConfigurationTags::iPAddress                           = "ip";
const std::string ConfigurationTags::iPAddressN                          = "IP Address";
const std::string ConfigurationTags::subnetMaskN                         = "Subnet Mask";
const std::string ConfigurationTags::networkSetupN                       = "Network Setup";
const std::string ConfigurationTags::iPPort                              = "IP";
const std::string ConfigurationTags::iPPortN                             = "Well-known Port";

const std::string ConfigurationTags::gatewayIPAddress1N                  = "Default Gateway";
const std::string ConfigurationTags::networkMask1N                       = "Default Network";
const std::string ConfigurationTags::gatewayIPAddress2N                  = "Gateway 1 IP Address";
const std::string ConfigurationTags::networkMask2N                       = "Network 1";
const std::string ConfigurationTags::gatewayIPAddress3N                  = "Gateway 2 IP Address";
const std::string ConfigurationTags::networkMask3N                       = "Network 2";

const std::string ConfigurationTags::dnsEnabledN                         = "DNS Enabled";
const std::string ConfigurationTags::dnsAutomaticN                       = "DNS Automatic";
const std::string ConfigurationTags::dnsHostNameN                        = "Host Name";
const std::string ConfigurationTags::dnsDomainNameN                      = "Domain Name";
const std::string ConfigurationTags::dnsIPAddress0N                      = "DNS 1 IP Address";
const std::string ConfigurationTags::dnsIPAddress1N                      = "DNS 2 IP Address";
const std::string ConfigurationTags::dnsIPAddress2N                      = "DNS 3 IP Address";

const std::string ConfigurationTags::tftpAutomaticN                      = "TFTP via DHCP";
const std::string ConfigurationTags::tftpIPAddressN                      = "TFTP Server";
const std::string ConfigurationTags::tftpBootFilePathN                   = "TFTP Boot File Path";

const std::string ConfigurationTags::hasWirelessCardN                    = "Has wireless card";
const std::string ConfigurationTags::wirelessCardNameN                   = "Wireless Card Name";
const std::string ConfigurationTags::wirelessCardConnectedN              = "Wireless Card Connected";
const std::string ConfigurationTags::accessPointId                       = "ei";
const std::string ConfigurationTags::accessPointIdN                      = "System ID";
const std::string ConfigurationTags::sharedKeyAuth                       = "wc";
const std::string ConfigurationTags::sharedKeyAuthN                      = "Shared Key Authentication";
const std::string ConfigurationTags::wepEnabled                          = "we";
const std::string ConfigurationTags::wepEnabledN                         = "WEP Enabled";
const std::string ConfigurationTags::activeKeyNum                        = "wk";
const std::string ConfigurationTags::activeKeyNumN                       = "Active Key";
const std::string ConfigurationTags::encryptionKey1                      = "w1";
const std::string ConfigurationTags::encryptionKey1SetN                  = "Encription Key 1 Set";
const std::string ConfigurationTags::encryptionKey2SetN                  = "Encription Key 2 Set";
const std::string ConfigurationTags::encryptionKey3SetN                  = "Encription Key 3 Set";
const std::string ConfigurationTags::encryptionKey4SetN                  = "Encription Key 4 Set";
const std::string ConfigurationTags::encryptionKey1N                     = "Encription Key 1";
const std::string ConfigurationTags::encryptionKey2                      = "w2";
const std::string ConfigurationTags::encryptionKey2N                     = "Encription Key 2";
const std::string ConfigurationTags::encryptionKey3                      = "w3";
const std::string ConfigurationTags::encryptionKey3N                     = "Encription Key 3";
const std::string ConfigurationTags::encryptionKey4                      = "w4";
const std::string ConfigurationTags::encryptionKey4N                     = "Encription Key 4";
const std::string ConfigurationTags::vpnEnabledN                         = "VPN Enabled";
const std::string ConfigurationTags::vpnIPAddressN                       = "VPN Server";
const std::string ConfigurationTags::leapAvailableN                      = "LEAP Available";
const std::string ConfigurationTags::leapEnabled                         = "ls";
const std::string ConfigurationTags::leapEnabledN                        = "LEAP Enabled";
const std::string ConfigurationTags::leapUserName                        = "lu";
const std::string ConfigurationTags::leapUserNameN                       = "LEAP Username";
const std::string ConfigurationTags::leapUserPassword                    = "lw";
const std::string ConfigurationTags::leapUserPasswordN                   = "LEAP Password";

const std::string ConfigurationTags::vpnCompression						 = "v1";
const std::string ConfigurationTags::vpnCompressionN					 = "VPN Compression";
const std::string ConfigurationTags::vpnDomain							 = "v2";
const std::string ConfigurationTags::vpnDomainN							 = "VPN Domain";
const std::string ConfigurationTags::vpnEncryptedPassword				 = "v3";
const std::string ConfigurationTags::vpnEncryptedPasswordN				 = "VPN Encrypted password";
const std::string ConfigurationTags::vpnUsername						 = "v4";
const std::string ConfigurationTags::vpnUsernameN						 = "VPN Username";
const std::string ConfigurationTags::vpnVerifyServerCertificate			 = "v5";
const std::string ConfigurationTags::vpnVerifyServerCertificateN		 = "VPN Verify server certificate";
const std::string ConfigurationTags::vpnNumberOfServers					 = "v6";
const std::string ConfigurationTags::vpnNumberOfServersN				 = "VPN Number of servers";
const std::string ConfigurationTags::vpnDeviceTypeDescriptor			 = "v7";
const std::string ConfigurationTags::vpnDeviceTypeDescriptorN			 = "VPN Device type descriptor";
const std::string ConfigurationTags::vpnUserClass						 = "v8";
const std::string ConfigurationTags::vpnUserClassN						 = "VPN User class";
const std::string ConfigurationTags::vpnMajorVerbVersion				 = "v9";
const std::string ConfigurationTags::vpnMajorVerbVersionN				 = "VPN Major verb version";
const std::string ConfigurationTags::vpnChooseFirstServerRandomly		 = "va";
const std::string ConfigurationTags::vpnChooseFirstServerRandomlyN		 = "VPN Choose first server randomly";
const std::string ConfigurationTags::vpnServerTCPPort					 = "vb";
const std::string ConfigurationTags::vpnServerTCPPortN					 = "VPN Server TCP port";
const std::string ConfigurationTags::vpnServerIPAddress1				 = "vc";
const std::string ConfigurationTags::vpnServerIPAddress1N				 = "VPN Server IP address1";
const std::string ConfigurationTags::vpnServerIPAddress2				 = "vd";
const std::string ConfigurationTags::vpnServerIPAddress2N				 = "VPN Server IP address2";
const std::string ConfigurationTags::vpnServerIPAddress3				 = "ve";
const std::string ConfigurationTags::vpnServerIPAddress3N				 = "VPN Server IP address3";
const std::string ConfigurationTags::vpnServerIPAddress4				 = "vf";
const std::string ConfigurationTags::vpnServerIPAddress4N				 = "VPN Server IP address4";
const std::string ConfigurationTags::vpnServerIPAddress5				 = "vg";
const std::string ConfigurationTags::vpnServerIPAddress5N				 = "VPN Server IP address5";
const std::string ConfigurationTags::vpnServerIPAddress6				 = "vh";
const std::string ConfigurationTags::vpnServerIPAddress6N				 = "VPN Server IP address6";
const std::string ConfigurationTags::vpnServerIPAddress7				 = "vi";
const std::string ConfigurationTags::vpnServerIPAddress7N				 = "VPN Server IP address7";
const std::string ConfigurationTags::vpnServerIPAddress8				 = "vj";
const std::string ConfigurationTags::vpnServerIPAddress8N				 = "VPN Server IP address8";

//  CfgInternal

//  CfgEmail
const std::string ConfigurationTags::from	                = "Am";
const std::string ConfigurationTags::fromN	                = "Send Outgoing Mail from";
const std::string ConfigurationTags::toList                 = "Al";
const std::string ConfigurationTags::toListN                = "Send Outgoing Mail to";
const std::string ConfigurationTags::smtpIpAddrOrDnsName    = "Aa";
const std::string ConfigurationTags::smtpIpAddrOrDnsNameN   = "Email Server";

//  CfgSynchronization
const std::string ConfigurationTags::scaleSynchronizationType   = "YT";
const std::string ConfigurationTags::scaleSynchronizationTypeN  = "Scale Synchronization Type";
const std::string ConfigurationTags::primaryScale               = "YP";
const std::string ConfigurationTags::primaryScaleN              = "Primary Scale";


const std::string ConfigurationTags::syncConfigurations					= "Za";
const std::string ConfigurationTags::syncConfigurationsN				= "Sync Configurations";

const std::string ConfigurationTags::syncEmailAlerts					= "Zb";
const std::string ConfigurationTags::syncEmailAlertsN					= "Sync Email Alerts";

const std::string ConfigurationTags::syncProductDefaults				= "Zc";
const std::string ConfigurationTags::syncProductDefaultsN				= "Sync Product Defaults";

const std::string ConfigurationTags::syncClasses						= "Zd";
const std::string ConfigurationTags::syncClassesN						= "Sync Classes";

const std::string ConfigurationTags::syncExpandedTexts					= "Ze";
const std::string ConfigurationTags::syncExpandedTextsN					= "Sync Expanded Texts";

const std::string ConfigurationTags::syncSpecialMessages				= "Zf";
const std::string ConfigurationTags::syncSpecialMessagesN				= "Sync Special Messages";

const std::string ConfigurationTags::syncNutritionFacts					= "Zg";
const std::string ConfigurationTags::syncNutritionFactsN				= "Sync Nutrition Facts";

const std::string ConfigurationTags::syncProductNotes					= "Zh";
const std::string ConfigurationTags::syncProductNotesN					= "Sync Product Notes";

const std::string ConfigurationTags::syncMarqueeTexts					= "Zi";
const std::string ConfigurationTags::syncMarqueeTextsN					= "Sync Marquee Texts";

const std::string ConfigurationTags::syncTransactions					= "Zj";
const std::string ConfigurationTags::syncTransactionsN					= "Sync Transactions";

const std::string ConfigurationTags::syncTransactionTypes				= "Zk";
const std::string ConfigurationTags::syncTransactionTypesN				= "Sync Transaction Types";

const std::string ConfigurationTags::syncCategories						= "Zl";
const std::string ConfigurationTags::syncCategoriesN					= "Sync Categories";

const std::string ConfigurationTags::syncSpecials						= "Zm";
const std::string ConfigurationTags::syncSpecialsN						= "Sync Specials";

const std::string ConfigurationTags::syncCoolCategories					= "Zn";
const std::string ConfigurationTags::syncCoolCategoriesN				= "Sync Categories";

const std::string ConfigurationTags::syncCoolCountries					= "Zo";
const std::string ConfigurationTags::syncCoolCountriesN					= "Sync Countries";

const std::string ConfigurationTags::syncCoolProductionSteps			= "Zp";
const std::string ConfigurationTags::syncCoolProductionStepsN			= "Sync Production Steps";

const std::string ConfigurationTags::syncCoolPreTexts					= "Zq";
const std::string ConfigurationTags::syncCoolPreTextsN					= "Sync Pre Texts";

const std::string ConfigurationTags::syncCoolTexts						= "Zr";
const std::string ConfigurationTags::syncCoolTextsN						= "Sync Cool Texts";

const std::string ConfigurationTags::syncCoolListTexts					= "Zs";
const std::string ConfigurationTags::syncCoolListTextsN					= "Sync List Texts";

const std::string ConfigurationTags::syncCoolLists						= "Zt";
const std::string ConfigurationTags::syncCoolListsN						= "Sync Cool Lists";

const std::string ConfigurationTags::syncCategoryCountries				= "Zu";
const std::string ConfigurationTags::syncCategoryCountriesN				= "Sync Category Countries";

const std::string ConfigurationTags::syncCategoryProductionSteps		= "Zv";
const std::string ConfigurationTags::syncCategoryProductionStepsN		= "Sync Category Production Steps";

const std::string ConfigurationTags::syncCategoryPreTexts				= "Zw";
const std::string ConfigurationTags::syncCategoryPreTextsN				= "Sync Category Pre Texts";

const std::string ConfigurationTags::syncProducts						= "Zx";
const std::string ConfigurationTags::syncProductsN						= "Sync Products";

const std::string ConfigurationTags::syncExpirationLabels				= "Zy";
const std::string ConfigurationTags::syncExpirationLabelsN				= "Sync Expiration Labels";

const std::string ConfigurationTags::syncLabels							= "Zz";
const std::string ConfigurationTags::syncLabelsN						= "Sync Labels";

const std::string ConfigurationTags::syncLabelTexts						= "ZA";
const std::string ConfigurationTags::syncLabelTextsN					= "Sync Label Texts";

const std::string ConfigurationTags::syncOperatorNotes					= "ZB";
const std::string ConfigurationTags::syncOperatorNotesN					= "Sync Operator Notes";

const std::string ConfigurationTags::syncOperators						= "ZC";
const std::string ConfigurationTags::syncOperatorsN						= "Sync Operators";

const std::string ConfigurationTags::syncSoftkeyGroups					= "ZD";
const std::string ConfigurationTags::syncSoftkeyGroupsN					= "Sync Softkey Groups";

const std::string ConfigurationTags::syncSoftkeyStyles					= "ZE";
const std::string ConfigurationTags::syncSoftkeyStylesN					= "Sync Softkey Styles";

const std::string ConfigurationTags::syncSoftkeyFeatures				= "ZF";
const std::string ConfigurationTags::syncSoftkeyFeaturesN				= "Sync Softkey Features";

const std::string ConfigurationTags::syncSoftkeys						= "ZG";
const std::string ConfigurationTags::syncSoftkeysN						= "Sync Softkeys";

const std::string ConfigurationTags::syncLabelGraphics					= "ZH";
const std::string ConfigurationTags::syncLabelGraphicsN					= "Sync Label Graphics";

const std::string ConfigurationTags::syncCategoryGraphics				= "ZI";
const std::string ConfigurationTags::syncCategoryGraphicsN				= "Sync Category Graphics";

const std::string ConfigurationTags::syncProductGraphics				= "ZJ";
const std::string ConfigurationTags::syncProductGraphicsN				= "Sync Product Graphics";

const std::string ConfigurationTags::syncSoftkeyGraphics				= "ZK";
const std::string ConfigurationTags::syncSoftkeyGraphicsN				= "Sync Softkey Graphics";

const std::string ConfigurationTags::syncSpecialsGraphics				= "ZL";
const std::string ConfigurationTags::syncSpecialsGraphicsN				= "Sync Specials Graphics";


//  CfgPrepack
const std::string ConfigurationTags::enableSellBy			= "DS";
const std::string ConfigurationTags::enableSellByN			= "Display Sell By Date";
const std::string ConfigurationTags::enableBestBefore		= "DB";
const std::string ConfigurationTags::enableBestBeforeN		= "Display Best Before Date";
const std::string ConfigurationTags::enableProdImg			= "DI";
const std::string ConfigurationTags::enableProdImgN			= "Display Product Image";
const std::string ConfigurationTags::enableUpcNumber		= "DU";
const std::string ConfigurationTags::enableUpcNumberN		= "Display UPC Number";
const std::string ConfigurationTags::enableFilmWidth		= "DW";
const std::string ConfigurationTags::enableFilmWidthN		= "Display Film Width";
const std::string ConfigurationTags::enableFilmTension		= "DN";
const std::string ConfigurationTags::enableFilmTensionN		= "Display Film Tension";
const std::string ConfigurationTags::enableWrapTypes		= "DR";
const std::string ConfigurationTags::enableWrapTypesN		= "Display Wrap Types";
const std::string ConfigurationTags::enableCountryOfOrigin	= "DC";
const std::string ConfigurationTags::enableCountryOfOriginN	= "Display COOL Information";
const std::string ConfigurationTags::enableTrackingNumber	= "DK";
const std::string ConfigurationTags::enableTrackingNumberN	= "Display Tracking Number";
const std::string ConfigurationTags::enableProductionEntry			= "DE";
const std::string ConfigurationTags::enableProductionEntryN			= "Enable Production Entry";
const std::string ConfigurationTags::productionEntryContinuousPrint			= "CP";
const std::string ConfigurationTags::productionEntryContinuousPrintN			= "Production Entry Continuous Print";
const std::string ConfigurationTags::enableProductionPlanning			= "EF";
const std::string ConfigurationTags::enableProductionPlanningN			= "Enable Production Planning";
const std::string ConfigurationTags::productionPlanningFrame			= "FS";
const std::string ConfigurationTags::productionPlanningFrameN			= "Production Planning Frame URL";
const std::string ConfigurationTags::productionPlanningUpdate			= "PU";
const std::string ConfigurationTags::productionPlanningUpdateN			= "Production Planning Update URL";
const std::string ConfigurationTags::enableSelfServe				= "sE";
const std::string ConfigurationTags::enableSelfServeN				= "Enable Self-Serve";
const std::string ConfigurationTags::pluSearchRange			= "pR";
const std::string ConfigurationTags::pluSearchRangeN		= "PLU Search Range";
const std::string ConfigurationTags::enableRunTotals		= "dr";
const std::string ConfigurationTags::enableRunTotalsN		= "Display Run Totals";
const std::string ConfigurationTags::enablePackedOn			= "Dp";
const std::string ConfigurationTags::enablePackedOnN		= "Display Packed On Date";
const std::string ConfigurationTags::rotOverrideSetting1	= "1S";
const std::string ConfigurationTags::rotOverrideSetting1N	= "Primary rotation override setting";
const std::string ConfigurationTags::rotOverrideSetting2	= "2S";
const std::string ConfigurationTags::rotOverrideSetting2N	= "Second rotation override setting";
const std::string ConfigurationTags::rotOverrideSetting3	= "3S";
const std::string ConfigurationTags::rotOverrideSetting3N	= "Third rotation override setting";
const std::string ConfigurationTags::rotOverrideValue1		= "1V";
const std::string ConfigurationTags::rotOverrideValue1N		= "Primary rotation override value";
const std::string ConfigurationTags::rotOverrideValue2		= "2V";
const std::string ConfigurationTags::rotOverrideValue2N		= "Second rotation override value";
const std::string ConfigurationTags::rotOverrideValue3		= "3V";
const std::string ConfigurationTags::rotOverrideValue3N		= "Third rotation override value";
const std::string ConfigurationTags::primaryLabelStockRotated    = "Pl";
const std::string ConfigurationTags::primaryLabelStockRotatedN   = "Primary Label Stock Reversed on Roll";
const std::string ConfigurationTags::secondaryLabelStockRotated  = "Sl";
const std::string ConfigurationTags::secondaryLabelStockRotatedN = "Secondary Label Stock Reversed on Roll";

const std::string ConfigurationTags::appendingPluNumberToName            = "pa";
const std::string ConfigurationTags::appendingPluNumberToNameN           = "Append PLU to Name";
const std::string ConfigurationTags::numOfDescCharsToUse                 = "#d";
const std::string ConfigurationTags::numOfDescCharsToUseN                = "Description Characters";

//  CfgMarquee
const std::string ConfigurationTags::permanentMarqueeNumber			= "Pm";  // Quantum uses tag 'Pm' to send the plu number containing the permanent marquee
const std::string ConfigurationTags::permanentMarqueeNumberN		= "Permanent Marquee Number";
const std::string ConfigurationTags::permanentMarqueeDisplayed		= "??";
const std::string ConfigurationTags::permanentMarqueeDisplayedN	    = "Permanent Marquee Displayed";
const std::string ConfigurationTags::productMarqueeDisplayed		= "pM";
const std::string ConfigurationTags::productMarqueeDisplayedN		= "Product Marquee Displayed";
const std::string ConfigurationTags::productDescMarqueeCounter		= "pd"; // Quantum sends data as follows: 0=never, 1-14= 1 to 14 times, 15=continuous
const std::string ConfigurationTags::productDescMarqueeCounterN	= "Product Desc Marquee Counter";

//  CfgFTP
const std::string ConfigurationTags::FTPUserName                            = "AN";
const std::string ConfigurationTags::FTPUserNameN                           = "FTP User Name";
const std::string ConfigurationTags::FTPPassword                            = "Ao";
const std::string ConfigurationTags::FTPPasswordN                           = "FTP Password";
const std::string ConfigurationTags::FTPSubDirectory                        = "AO";
const std::string ConfigurationTags::FTPSubDirectoryN                       = "FTP Sub Directory";
const std::string ConfigurationTags::FTPServerIPorHostName                  = "An";
const std::string ConfigurationTags::FTPServerIPorHostNameN                 = "FTP Server IP/Host Name";
const std::string ConfigurationTags::FTPUseAutomatic                        = "UA";
const std::string ConfigurationTags::FTPUseAutomaticN			            = "FTP Use Automatic";
const std::string ConfigurationTags::FTPScheduleStartTime                   = "UI";
const std::string ConfigurationTags::FTPScheduleStartTimeN                  = "FTP Schedule Start Time";
const std::string ConfigurationTags::FTPmonday                              = "UB";
const std::string ConfigurationTags::FTPmondayN                             = "FTP monday";
const std::string ConfigurationTags::FTPtuesday                             = "UC";
const std::string ConfigurationTags::FTPtuesdayN                            = "FTP tuesday";
const std::string ConfigurationTags::FTPwednesday                           = "UD";
const std::string ConfigurationTags::FTPwednesdayN                          = "FTP wednesday";
const std::string ConfigurationTags::FTPthursday                            = "UE";
const std::string ConfigurationTags::FTPthursdayN                           = "FTP thursday";
const std::string ConfigurationTags::FTPfriday                              = "UF";
const std::string ConfigurationTags::FTPfridayN                             = "FTP friday";
const std::string ConfigurationTags::FTPsaturday                            = "UG";
const std::string ConfigurationTags::FTPsaturdayN                           = "FTP saturday";
const std::string ConfigurationTags::FTPsunday                              = "UH";
const std::string ConfigurationTags::FTPsundayN                             = "FTP sunday";
const std::string ConfigurationTags::FTPdownloadFirmwareOnly	            = "UJ";
const std::string ConfigurationTags::FTPdownloadFirmwareOnlyN               = "FTP download Firmware Only";
const std::string ConfigurationTags::FTPdownloadInstallFirmware             = "UK";
const std::string ConfigurationTags::FTPdownloadInstallFirmwareN            = "FTP Download Install Firmware";
const std::string ConfigurationTags::FTPdailyUpdates                        = "U>";
const std::string ConfigurationTags::FTPdailyUpdatesN                       = "FTP Daily Updates";
const std::string ConfigurationTags::FTPhourlyUpdates                       = "UL";
const std::string ConfigurationTags::FTPhourlyUpdatesN                      = "FTP Hourly Updates";
const std::string ConfigurationTags::FTP15MinuteUpdates                     = "UM";
const std::string ConfigurationTags::FTP15MinuteUpdatesN                    = "FTP 15 Minute Updates";
const std::string ConfigurationTags::FTPUpdatePLUData                       = "UP";
const std::string ConfigurationTags::FTPUpdatePLUDataN                      = "FTP Update PLU Data";
const std::string ConfigurationTags::FTPUpdateLabelData                     = "UQ";
const std::string ConfigurationTags::FTPUpdateLabelDataN                    = "FTP Update Label Data";
const std::string ConfigurationTags::FTPUpdateFlashkeyData                  = "UR";
const std::string ConfigurationTags::FTPUpdateFlashkeyDataN                 = "FTP Update Flashkey Data";
const std::string ConfigurationTags::FTPUpdateOperatorData                  = "US";
const std::string ConfigurationTags::FTPUpdateOperatorDataN                 = "FTP Update Operator Data";
const std::string ConfigurationTags::FTPUpdatePlaylists                     = "UU";
const std::string ConfigurationTags::FTPUpdatePlaylistsN                    = "FTP Update Playlists";
const std::string ConfigurationTags::FTPUpdateAlerts                        = "UV";
const std::string ConfigurationTags::FTPUpdateAlertsN                       = "FTP Update Alerts";
const std::string ConfigurationTags::FTPUpdateScaleConfiguration            = "UW";
const std::string ConfigurationTags::FTPUpdateScaleConfigurationN           = "FTP Update Scale Configurqation";
const std::string ConfigurationTags::FTPUpdateSynchronizationConfiguration  = "UX";
const std::string ConfigurationTags::FTPUpdateSynchronizationConfigurationN = "FTP Update Synchronization Configuration";
const std::string ConfigurationTags::FTPUpdateStoreNameNumber               = "UY";
const std::string ConfigurationTags::FTPUpdateStoreNameNumberN              = "FTP Update Store Name Number";
const std::string ConfigurationTags::FTPUpdateVideos                        = "UZ";
const std::string ConfigurationTags::FTPUpdateVideosN                       = "FTP Update Videos";
const std::string ConfigurationTags::FTPUpdateImages                        = "U1";
const std::string ConfigurationTags::FTPUpdateImagesN                       = "FTP Update Images";
const std::string ConfigurationTags::FTPUpdateAudio                         = "U2";
const std::string ConfigurationTags::FTPUpdateAudioN                        = "FTP Update Audio";
const std::string ConfigurationTags::FTPUpdateFirmware                      = "U3";
const std::string ConfigurationTags::FTPUpdateFirmwareN                     = "FTP Update Firmware";
const std::string ConfigurationTags::FTPSendPLUData                         = "U4";
const std::string ConfigurationTags::FTPSendPLUDataN                        = "FTP Send PLU Data";
const std::string ConfigurationTags::FTPSendLabelData                       = "U5";
const std::string ConfigurationTags::FTPSendLabelDataN                      = "FTP Send Label Data";
const std::string ConfigurationTags::FTPSendFlashkeyData                    = "U6";
const std::string ConfigurationTags::FTPSendFlashkeyDataN                   = "FTP Send Flashkey Data";
const std::string ConfigurationTags::FTPSendOperatorData                    = "U7";
const std::string ConfigurationTags::FTPSendOperatorDataN                   = "FTP Send Operator Data";
const std::string ConfigurationTags::FTPSendPlaylists                       = "U8";
const std::string ConfigurationTags::FTPSendPlaylistsN                      = "FTP Send Playlists";
const std::string ConfigurationTags::FTPSendAlerts                          = "U9";
const std::string ConfigurationTags::FTPSendAlertsN                         = "FTP Send Alerts";
const std::string ConfigurationTags::FTPSendScaleConfiguration              = "U!";
const std::string ConfigurationTags::FTPSendScaleConfigurationN             = "FTP Send Scale Configurqation";
const std::string ConfigurationTags::FTPSendSynchronizationConfiguration    = "U@";
const std::string ConfigurationTags::FTPSendSynchronizationConfigurationN   = "FTP Send Synchronization Configuration";
const std::string ConfigurationTags::FTPSendStoreNameNumber                 = "U^";
const std::string ConfigurationTags::FTPSendStoreNameNumberN                = "FTP Send Store Name Number";
const std::string ConfigurationTags::FTPSendVideos                          = "U&";
const std::string ConfigurationTags::FTPSendVideosN                         = "FTP Send Videos";
const std::string ConfigurationTags::FTPSendImages                          = "U%";
const std::string ConfigurationTags::FTPSendImagesN                         = "FTP Send Images";
const std::string ConfigurationTags::FTPSendAudio                           = "U(";
const std::string ConfigurationTags::FTPSendAudioN                          = "FTP Send Audio";
const std::string ConfigurationTags::FTPSendFirmware                        = "U)";
const std::string ConfigurationTags::FTPSendFirmwareN                       = "FTP Send Firmware";
const std::string ConfigurationTags::FTPAllowOperatorSendToDept             = "U_";
const std::string ConfigurationTags::FTPAllowOperatorSendToDeptN            = "FTP Allow Operator Send To Dept";
const std::string ConfigurationTags::FTPAllowOperatorUpdateNow              = "U~";
const std::string ConfigurationTags::FTPAllowOperatorUpdateNowN             = "FTP Allow Operator Update Now";

//  CfgCommInterface
const std::string ConfigurationTags::CommProductClassTotalsClearTransactions        = "pt";
const std::string ConfigurationTags::CommProductClassTotalsClearTransactionsN       = "ProductClassTotals Clear Transactions";
const std::string ConfigurationTags::CommHourlyTotalsClearTransactions              = "hT";
const std::string ConfigurationTags::CommHourlyTotalsClearTransactionsN             = "HourlyTotals Clear Transactions";
const std::string ConfigurationTags::CommOperatorTotalsClearTransactions            = "oT";
const std::string ConfigurationTags::CommOperatorTotalsClearTransactionsN           = "OperatorTotals Clear Transactions";
const std::string ConfigurationTags::CommCOOLTotalsClearTransactions                = "lT";
const std::string ConfigurationTags::CommCOOLTotalsClearTransactionsN               = "COOLTotals Clear Transactions";
const std::string ConfigurationTags::CommOperatorChangesClearTransactions           = "oC";
const std::string ConfigurationTags::CommOperatorChangesClearTransactionsN          = "OperatorChanges Clear Transactions";

//  CfgCurrency
const std::string ConfigurationTags::CurrencyDecimalSeparator                       = "dc";
const std::string ConfigurationTags::CurrencyMonetaryRule                           = "MS";
const std::string ConfigurationTags::CurrencyTotalPriceTotalDigits                  = "t#";
const std::string ConfigurationTags::CurrencyUnitPriceTotalDigits                   = "U#";
const std::string ConfigurationTags::CurrencyRoundingFactor                         = "rf";
const std::string ConfigurationTags::CurrencyRoundingMethod                         = "rm";
const std::string ConfigurationTags::CurrencyPrimarySymbol                          = "$s";
const std::string ConfigurationTags::CurrencyPrimaryDecimalDigits                   = "td";
const std::string ConfigurationTags::CurrencySecondarySymbol                        = "SC";
const std::string ConfigurationTags::CurrencySecondaryDecimalDigits                 = "SD";
//const std::string ConfigurationTags::currencyUnits                       = "cu";
//const std::string ConfigurationTags::currencyUnitsN                      = "Currency Units";
//const std::string ConfigurationTags::monetarySymbol                      = "??";
//const std::string ConfigurationTags::monetarySymbolN                     = "????";
//const std::string ConfigurationTags::pricePrecision                      = "td";
//const std::string ConfigurationTags::pricePrecisionN                     = "Price Precision";

//  CfgAccumulateItems
const std::string ConfigurationTags::enableAccumulateItems           = "Im";  // same as Quantum
const std::string ConfigurationTags::enableAccumulateItemsN          = "Accumulate Items Enabled";
const std::string ConfigurationTags::enableItemStripMode             = "iS";  // same as Quantum
const std::string ConfigurationTags::enableItemStripModeN            = "Item Strip Mode Enabled";
const std::string ConfigurationTags::accumItemsReturnToStartScreen   = "iR";
const std::string ConfigurationTags::accumItemsReturnToStartScreenN  = "Accumulate Items Return To Start Screen";
const std::string ConfigurationTags::enableAccumItemsTotalsOnly      = "to";  // same as Quantum
const std::string ConfigurationTags::enableAccumItemsTotalsOnlyN     = "Print Totals Labels Only";
const std::string ConfigurationTags::accumItemsEANFormatString       = "If";  // same as Quantum
const std::string ConfigurationTags::accumItemsEANFormatStringN      = "Accumulate Items EAN Format String";
const std::string ConfigurationTags::accumItemsEANPrefix             = "Ip";  // same as Quantum
const std::string ConfigurationTags::accumItemsEANPrefixN            = "Accumulate Items EAN Prefix";
const std::string ConfigurationTags::accumItemsEANCode               = "iu";  // same as Quantum
const std::string ConfigurationTags::accumItemsEANCodeN              = "Accumulate Items EAN Code";
const std::string ConfigurationTags::accumItemsScaleId               = "sd";  // same as Quantum??
const std::string ConfigurationTags::accumItemsScaleIdN              = "Accumulate Items Scale Id";
//const std::string ConfigurationTags::accumItemsTransactionNumber     = ;  This is the same as the transaction id/number tag as defined in TransactionTags.h!
const std::string ConfigurationTags::accumItemsTransactionNumberN    = "Accumulate Items Transaction Number";
const std::string ConfigurationTags::accumItemsUPCProductNumber      = "iP";
const std::string ConfigurationTags::accumItemsUPCProductNumberN     = "Accumulate Items UPC Product Number";
const std::string ConfigurationTags::readTransactionsCompatibility   = "Tc";
const std::string ConfigurationTags::readTransactionsCompatibilityN  = "Read Transactions for Quantum Compatibility";
const std::string ConfigurationTags::accumBarcodeUseFirstItem        = "ab";  // same as Quantum
const std::string ConfigurationTags::accumBarcodeUseFirstItemN       = "Accumulate Barcode Use First Item";


const std::string ConfigurationTags::rwEanFormat		= "Rf";
const std::string ConfigurationTags::fwEanFormat		= "ff";
const std::string ConfigurationTags::bcEanFormat		= "bf";
const std::string ConfigurationTags::rwEanPrefix		= "Rp";
const std::string ConfigurationTags::fwEanPrefix		= "Fp";
const std::string ConfigurationTags::bcEanPrefix		= "bp";

//  Not saved to Config - Input only from Quantum. Will impact SoftkeyFeature default group #
const std::string ConfigurationTags::homeFlashkeyGroup		= "hf";

const std::string ConfigurationTags::virtualPrintingOverride2ndPrinter   = "V2";
const std::string ConfigurationTags::virtualPrintingOverride2ndPrinterN  = "Virtual Printing Override 2nd Printer";
const std::string ConfigurationTags::virtualPrintingOverride3rdPrinter   = "V3";
const std::string ConfigurationTags::virtualPrintingOverride3rdPrinterN  = "Virtual Printing Override 3rd Printer";

const std::string ConfigurationTags::displaySelfServeStartScreen         = "sH";
const std::string ConfigurationTags::displaySelfServeStartScreenN        = "Display SelfServe Start Screen";
const std::string ConfigurationTags::bestSellersOnSelfServeMainScreen    = "sR";
const std::string ConfigurationTags::bestSellersOnSelfServeMainScreenN   = "Best Sellers in Self Serve";

const std::string ConfigurationTags::enableLabelLength1_000      = "zA";
const std::string ConfigurationTags::enableLabelLength1_500      = "zB";
const std::string ConfigurationTags::enableLabelLength1_750      = "zC";
const std::string ConfigurationTags::enableLabelLength2_000      = "zD";
const std::string ConfigurationTags::enableLabelLength2_375      = "zE";
const std::string ConfigurationTags::enableLabelLength2_500      = "zF";
const std::string ConfigurationTags::enableLabelLength3_000      = "zG";
const std::string ConfigurationTags::enableLabelLength3_500      = "zH";
const std::string ConfigurationTags::enableLabelLength4_000      = "zI";
const std::string ConfigurationTags::enableLabelLength4_500      = "zJ";
const std::string ConfigurationTags::enableLabelLength5_000      = "zK";
const std::string ConfigurationTags::enableLabelLength5_500      = "zL";
const std::string ConfigurationTags::enableLabelLength6_000      = "zM";
const std::string ConfigurationTags::enableLabelLength6_500      = "zN";
const std::string ConfigurationTags::enableLabelLength7_000      = "zO";
const std::string ConfigurationTags::enableLabelLength7_500      = "zP";
const std::string ConfigurationTags::enableLabelLength8_000      = "zQ";
const std::string ConfigurationTags::enableLabelLength8_500      = "zR";
const std::string ConfigurationTags::enableLabelLength9_000      = "zS";
const std::string ConfigurationTags::enableLabelLength9_500      = "zT";
const std::string ConfigurationTags::enableLabelLength10_00      = "zU";
const std::string ConfigurationTags::enableLabelLengthContinuous = "zV";

const std::string ConfigurationTags::continuousStockIgnoreLabelTaken = "it";
const std::string ConfigurationTags::choseToUseContinuousStock       = "uC";

const std::string ConfigurationTags::weightRequiredForMeal       = "Wm";

const std::string ConfigurationTags::suggestiveSellStyle         = "sY";
const std::string ConfigurationTags::suggestiveSellFrequency     = "sQ";

const std::string ConfigurationTags::alwaysEnableVirtualKeyboard    = "vk";

const std::string ConfigurationTags::showSelectAllWhenVoidingTrans  = "VA";

const std::string ConfigurationTags::periodicOfflineBeep            = "ob";

const std::string ConfigurationTags::enableGlobalPercentOff         = "gp";
const std::string ConfigurationTags::globalPercentageOff            = "gd";
const std::string ConfigurationTags::enableAccumItemsAndFlashkeys   = "af";
// cuttesttodo
const std::string ConfigurationTags::cutTestOptionEnabled           = "OC";
const std::string ConfigurationTags::maxSuspendedCutTestsOverall    = "CM";
const std::string ConfigurationTags::maxSuspendedCutTestsOperator   = "Cm";
const std::string ConfigurationTags::secondsToAutoSuspendCutTest    = "Ca";
const std::string ConfigurationTags::nextCutTestNumber              = "CN";

const std::string ConfigurationTags::diagServiceContactInfo					= "d1";
const std::string ConfigurationTags::diagGatewayDeviceAddress				= "d2";
const std::string ConfigurationTags::diagGatewayDeviceStatusMsgLogLevel		= "d3";

const std::string ConfigurationTags::requireFixedWeightVerification   = "wv";

const std::string ConfigurationTags::autoBackupDBFrequency            = "Db";

const std::string ConfigurationTags::FPCVersion                       = "FV";
const std::string ConfigurationTags::FPCWeigherVersion                = "Fw";

const std::string ConfigurationTags::scaleModel                       = "Ms";

const std::string ConfigurationTags::bottomScreenContent            = "RB";


const std::string ConfigurationTags::enableSearchMenuButton           = "eS";

const std::string ConfigurationTags::enableHelpPrompts                = "eH";
const std::string ConfigurationTags::enableUseCharacterSearching	  = "eU";

const std::string ConfigurationTags::useSecondTare                  = "UT";

const std::string ConfigurationTags::variableShelfLifeTime          = "1B";

const std::string ConfigurationTags::selfServeWeightRequiredToSelectItem = "wr";
const std::string ConfigurationTags::selfServeByCountItemsAllowed        = "bs";


const std::string ConfigurationTags::printer1LabelLength            = "z1";
const std::string ConfigurationTags::printer2LabelLength            = "z2";
const std::string ConfigurationTags::printer3LabelLength            = "z3";

const std::string ConfigurationTags::defaultSoftkeyFontPointSize    = "FP";

const std::string ConfigurationTags::selfServePrintToScreen         = "Ps";				// Amazon GO - Print To Screen
const std::string ConfigurationTags::selfServeDisplayTareOnRW       = "dt";
const std::string ConfigurationTags::doNotPrintNutrifacts           = "np";
const std::string ConfigurationTags::selfServeIgnorePrinterErrors   = "ie";				// ORIG: ComboBox, BigY displayed: 0= Print Label, 1= Scan Label (ignore errors), 2= Scan and Print Label
const std::string ConfigurationTags::selfServeSearchKeypadFlashkeys = "sk";             // ORIG: Checkbox, BigY allowed Combine Search, Keypad, and Flashkeys
const std::string ConfigurationTags::selfServeDisplayPluLabel       = "dL";				// ORIG: Checkbox, BigY: print the label assigned to the PLU

const std::string ConfigurationTags::voidTransactionsViewHours		= "tv";

const std::string ConfigurationTags::selfServeEnablePlaylists       = "Ep";
const std::string ConfigurationTags::selfServePlaylistFile          = "Sp";

const std::string ConfigurationTags::mostRecentActiveDeptNum          = "aD";
const std::string ConfigurationTags::useCurrentDeptnumWhenNotReceived = "iD";

const std::string ConfigurationTags::digimarcEnabled				= "dE";
const std::string ConfigurationTags::digimarcDotDensity				= "dD";
const std::string ConfigurationTags::digimarcDotSize				= "dZ";
const std::string ConfigurationTags::digimarcWatermarkRes			= "dW";
const std::string ConfigurationTags::digimarcExpandedFreshEnabled	= "dF";

const std::string ConfigurationTags::reduceGS1Length				= "r0";
const std::string ConfigurationTags::disableHooks					= "DH";

const std::string ConfigurationTags::cameraSystemTrustLevel			= "Cr";

const std::string ConfigurationTags::unitPricePrintMethod			= "uP";

const std::string ConfigurationTags::priceChangedLabelIndicator     = "PC";

const std::string ConfigurationTags::autoUpdateUPCBarcodeOver99     = "bu";
const std::string ConfigurationTags::autoUpdateUPCBarcodeOver99N    = "Automatically update the UPC Barcode if Total Price > $99.99";
const std::string ConfigurationTags::barcodeTypeOver99              = "bv";
const std::string ConfigurationTags::barcodeTypeOver99N             = "New Auto Updated Barcode Type";

// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
const std::string ConfigurationTags::barcodeWidthOver99             = "bw";
const std::string ConfigurationTags::barcodeOffsetFromLeftOver99    = "bx";

const std::string ConfigurationTags::snmpTrapsIp                    = "sp";
const std::string ConfigurationTags::snmpTrapsPort                  = "sP";
const std::string ConfigurationTags::snmpVariablesPort              = "sv";
const std::string ConfigurationTags::snmpEnabled                    = "sV";

const std::string ConfigurationTags::cutterEnabled                  = "cE";
const std::string ConfigurationTags::codeCheckerEnabled             = "cC";

const std::string ConfigurationTags::handsFreeDeviceMinLoginPowerLevel = "HF";

const std::string ConfigurationTags::allowSupervisorDotWear			= "SW";
const std::string ConfigurationTags::allowOperatorDotWear			= "Ow";

const std::string ConfigurationTags::enableSearchOnKeypad           = "kS";
const std::string ConfigurationTags::enableEnhancedCenterScreen     = "ee";
const std::string ConfigurationTags::enhancedCenterHomePage         = "ec";
const std::string ConfigurationTags::enableCombinedVirtualPrint		= "cv";

const std::string ConfigurationTags::wrapperCameraEnabled           = "WC";

// CfgCommScheduled
const std::string ConfigurationTags::CommScheduledUseAutomaticMerges = "QA";
const std::string ConfigurationTags::CommScheduledUseAutomaticMergesN = "Comm Sched Use Automatic";
const std::string ConfigurationTags::CommScheduledMonday = "QB";
const std::string ConfigurationTags::CommScheduledMondayN = "Comm Sched Monday";
const std::string ConfigurationTags::CommScheduledTuesday = "QC";
const std::string ConfigurationTags::CommScheduledTuesdayN = "FTP tuesday";
const std::string ConfigurationTags::CommScheduledWednesday = "QD";
const std::string ConfigurationTags::CommScheduledWednesdayN = "Comm Sched Wednesday";
const std::string ConfigurationTags::CommScheduledThursday = "QE";
const std::string ConfigurationTags::CommScheduledThursdayN = "Comm Sched Thursday";
const std::string ConfigurationTags::CommScheduledFriday = "QF";
const std::string ConfigurationTags::CommScheduledFridayN = "Comm Sched Friday";
const std::string ConfigurationTags::CommScheduledSaturday = "QG";
const std::string ConfigurationTags::CommScheduledSaturdayN = "Comm Sched Saturday";
const std::string ConfigurationTags::CommScheduledSunday = "QH";
const std::string ConfigurationTags::CommScheduledSundayN = "Comm Sched Sunday";
const std::string ConfigurationTags::CommScheduledDailyUpdates = "QI";
const std::string ConfigurationTags::CommScheduledDailyUpdatesN = "Comm Sched Daily Updates";
const std::string ConfigurationTags::CommScheduledStartTime = "QJ";
const std::string ConfigurationTags::CommScheduledStartTimeN = "Comm Sched Schedule Start Time";
const std::string ConfigurationTags::CommScheduledHourlyUpdates = "QK";
const std::string ConfigurationTags::CommScheduledHourlyUpdatesN = "Comm Sched Hourly Updates";
const std::string ConfigurationTags::CommScheduledExecMergeAfterComm = "QL";
const std::string ConfigurationTags::CommScheduledExecMergeAfterCommN = "Comm Sched Exec Merge After Comm";
const std::string ConfigurationTags::CommScheduledMergeHistory = "QM";
const std::string ConfigurationTags::CommScheduledMergeHistoryN = "Comm Sched Merge History";

const std::string ConfigurationTags::selfServeFixedPlu              = "fU"; // PJR413 (SOF-4772)

const std::string ConfigurationTags::selfServeEnableFreshServe      = "g2";

const std::string ConfigurationTags::selfServeFlashkeyType0         = "F3";
const std::string ConfigurationTags::selfServeFlashkeyType1			= "F4";
const std::string ConfigurationTags::selfServeFlashkeyType2			= "F5";
const std::string ConfigurationTags::selfServeFlashkeyType3			= "F6";
const std::string ConfigurationTags::selfServeFlashkeyType4			= "F7";
const std::string ConfigurationTags::selfServeFlashkeyText0			= "F9";
const std::string ConfigurationTags::selfServeFlashkeyText1			= "j1";
const std::string ConfigurationTags::selfServeFlashkeyText2			= "j2";
const std::string ConfigurationTags::selfServeFlashkeyText3			= "j3";
const std::string ConfigurationTags::selfServeFlashkeyText4			= "j4";
const std::string ConfigurationTags::selfServeFlashkeyCatNum0		= "j5";
const std::string ConfigurationTags::selfServeFlashkeyCatNum1		= "j6";
const std::string ConfigurationTags::selfServeFlashkeyCatNum2		= "j7";
const std::string ConfigurationTags::selfServeFlashkeyCatNum3		= "j8";
const std::string ConfigurationTags::selfServeFlashkeyCatNum4		= "j9";
const std::string ConfigurationTags::selfServeFreshServeLabelOffset = "F8";
const std::string ConfigurationTags::selfServeFlashkeySecText0      = "J0";
const std::string ConfigurationTags::selfServeFlashkeySecText1      = "J1";
const std::string ConfigurationTags::selfServeFlashkeySecText2      = "J2";
const std::string ConfigurationTags::selfServeFlashkeySecText3      = "J3";
const std::string ConfigurationTags::selfServeFlashkeySecText4      = "J4";
const std::string ConfigurationTags::readBarcodeFromCloud = "Dl";
const std::string ConfigurationTags::readBarcodeFromCloudN = "Read Barcode From Cloud";
const std::string ConfigurationTags::barcodeServerAddress = "Dm";
const std::string ConfigurationTags::barcodeServerAddressN = "Barcode Server Address";

const std::string ConfigurationTags::enableConserveLinerLess		= "lc";
const std::string ConfigurationTags::enableUPTagPrefix				= "eu";

const std::string ConfigurationTags::minSweetheartWeight			= "sw";

const std::string ConfigurationTags::onlineOrderingEnabled			= "oo";
const std::string ConfigurationTags::onlineOrderingServer			= "oS";
const std::string ConfigurationTags::onlineOrderingZone			    = "oz";



const std::string ConfigurationTags::selfServeDisplayBarcodeLabel   = "Bs";				// ORIG: Checkbox, Delhaize: Display Barcode(ONLY) and Print button on screen at same time

const std::string ConfigurationTags::selfServeStartState			= "S4";
const std::string ConfigurationTags::selfServeItemEntryState		= "S5";
const std::string ConfigurationTags::selfServeSearchState			= "S6";
const std::string ConfigurationTags::selfServePrintState			= "S7";
const std::string ConfigurationTags::selfServeAfterPrintingState    = "S0";
const std::string ConfigurationTags::selfServeStartScreenClickable	= "S8";
const std::string ConfigurationTags::selfServeIncludeKeypad			= "S9";

const std::string ConfigurationTags::selfServeHideMenuBar = "hb";
const std::string ConfigurationTags::selfServePrintOnStableWeight = "Sw";
const std::string ConfigurationTags::enableVariableShelfLifeTime    = "1D";

const std::string ConfigurationTags::ignoreVendorAndProductNumTagsWhenUpcNumTag = "VP";

const std::string ConfigurationTags::allowKeyboardNetWeightItemAsRandomWeight = "TR";
const std::string ConfigurationTags::fieldExclusions = "fe";

const std::string ConfigurationTags::enableRfidPrinting  = "RP";
const std::string ConfigurationTags::maxPERfidVoidLabels = "RV";
const std::string ConfigurationTags::rfidEncodeMethod    = "RE";
const std::string ConfigurationTags::rfidIgnore          = "rI";

const std::string ConfigurationTags::emptyN = "";


bool ConfigurationTags::_tagsInitialized_ = false;
TagsList ConfigurationTags::configurationTags(endCfgFieldOffsets);
TagsList ConfigurationTags::configurationNames(endCfgFieldOffsets);
TagsMap  ConfigurationTags::fieldMap;	//example pair: r#,fieldOffsetCfgTextNum


ConfigurationTags::ConfigurationTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the Configuration class
		configurationTags[0]                                             = emptyTag;
		configurationNames[0]                                            = emptyN;
		configurationTags[fieldOffsetCfgStoreId]                            = storeId;
		configurationNames[fieldOffsetCfgStoreId]                           = storeIdN;
		configurationTags[fieldOffsetCfgStoreName]                          = storeName;
		configurationNames[fieldOffsetCfgStoreName]                         = storeNameN;
		configurationTags[fieldOffsetCfgStoreGraphic]                       = storeGraphic;
		configurationNames[fieldOffsetCfgStoreGraphic]                      = storeGraphicN;
		// No longer used as of 9-19-17- claim back later - configurationTags[fieldOffsetCfgDeptName]                           = deptName;
		// No longer used as of 9-19-17- claim back later - configurationNames[fieldOffsetCfgDeptName]                          = deptNameN;

		configurationTags[fieldOffsetCfgPrimaryCountryCode]                 = primaryCountryCode;
		configurationNames[fieldOffsetCfgPrimaryCountryCode]                = primaryCountryCodeN;
		configurationTags[fieldOffsetCfgPrimaryLanguageCode]                = primaryLanguageCode;
		configurationNames[fieldOffsetCfgPrimaryLanguageCode]               = primaryLanguageCodeN;
		configurationTags[fieldOffsetCfgSecondaryCountryCode]               = secondaryCountryCode;
		configurationNames[fieldOffsetCfgSecondaryCountryCode]              = secondaryCountryCodeN;
		configurationTags[fieldOffsetCfgSecondaryLanguageCode]              = secondaryLanguageCode;
		configurationNames[fieldOffsetCfgSecondaryLanguageCode]             = secondaryLanguageCodeN;
		configurationTags[fieldOffsetCfgLengthUnits]                        = lengthUnits;
		configurationNames[fieldOffsetCfgLengthUnits]                       = lengthUnitsN;
		configurationTags[fieldOffsetCfgTemperatureUnits]                   = temperatureUnits;
		configurationNames[fieldOffsetCfgTemperatureUnits]                  = temperatureUnitsN;
		configurationTags[fieldOffsetCfgDateFormat]                         = dateFormat;
		configurationNames[fieldOffsetCfgDateFormat]                        = dateFormatN;
		configurationTags[fieldOffsetCfgTimeFormat]                         = timeFormat;
		configurationNames[fieldOffsetCfgTimeFormat]                        = timeFormatN;

		configurationTags[fieldOffsetCfgUnitPriceSecurityLabel]             = unitPriceSecurityLabel;
		configurationNames[fieldOffsetCfgUnitPriceSecurityLabel]            = unitPriceSecurityLabelN;
		configurationTags[fieldOffsetCfgTotalPriceSecurityLabel]            = totalPriceSecurityLabel;
		configurationNames[fieldOffsetCfgTotalPriceSecurityLabel]           = totalPriceSecurityLabelN;
		configurationTags[fieldOffsetCfgFractionalPricingEnabled]           = fractionalPricingEnabled;
		configurationNames[fieldOffsetCfgFractionalPricingEnabled]          = fractionalPricingEnabledN;
		configurationTags[fieldOffsetCfgPoundsForPricingEnabled]            = poundsForPricingEnabled; 
		configurationNames[fieldOffsetCfgPoundsForPricingEnabled]           = poundsForPricingEnabledN; 
		configurationTags[fieldOffsetCfgBycountCalcMethod]                  = bycountCalcMethod;
		configurationNames[fieldOffsetCfgBycountCalcMethod]                 = bycountCalcMethodN;
		configurationTags[fieldOffsetCfgClearPluAfterPrinting]              = clearPluAfterPrinting;
		configurationNames[fieldOffsetCfgClearPluAfterPrinting]             = clearPluAfterPrintingN;

		configurationTags[fieldOffsetCfgBeepOnStableWeight]                 = isSoundBeepStableWeight;
		configurationNames[fieldOffsetCfgBeepOnStableWeight]                = isSoundBeepStableWeightN;
		configurationTags[fieldOffsetCfgBeeperVolumeLevel]                  = beeperVolumeLevel;
		configurationNames[fieldOffsetCfgBeeperVolumeLevel]                 = beeperVolumeLevelN;

		configurationTags[fieldOffsetCfgFixedWeightUnits]                   = fixedWeightUnits;
		configurationNames[fieldOffsetCfgFixedWeightUnits]                  = fixedWeightUnitsN;
		configurationTags[fieldOffsetCfgWeigherAttached]                    = weigherAttached;
		configurationNames[fieldOffsetCfgWeigherAttached]                   = weigherAttachedN;
		configurationTags[fieldOffsetCfgTareSetback]                        = tareSetback;
		configurationNames[fieldOffsetCfgTareSetback]                       = tareSetbackN;
		configurationTags[fieldOffsetCfgMaxTare]                            = maxTare;
		configurationNames[fieldOffsetCfgMaxTare]                           = maxTareN;
		configurationTags[fieldOffsetCfgPlatterTareEnabled]                 = platterTareEnabled;
		configurationNames[fieldOffsetCfgPlatterTareEnabled]                = platterTareEnabledN;
		configurationTags[fieldOffsetCfgChangeZeroTareOnlyOnce]             = changeZeroTareOnlyOnce;
		configurationNames[fieldOffsetCfgChangeZeroTareOnlyOnce]            = changeZeroTareOnlyOnceN;
		configurationTags[fieldOffsetCfgChangeForcedTareOnlyOnce]           = changeForcedTareOnlyOnce;
		configurationNames[fieldOffsetCfgChangeForcedTareOnlyOnce]          = changeForcedTareOnlyOnceN;
		configurationTags[fieldOffsetCfgMinWeightToPrintLabel]              = minWeightToPrint;
		configurationNames[fieldOffsetCfgMinWeightToPrintLabel]             = minWeightToPrintN;
		configurationTags[fieldOffsetCfgWeighMode]                          = scaleWeighMode;
		configurationNames[fieldOffsetCfgWeighMode]                         = scaleWeighModeN;
		configurationTags[fieldOffsetCfgWeigherModel]                       = weigherModel;
		configurationNames[fieldOffsetCfgWeigherModel]                      = weigherModelN;
		configurationNames[fieldOffsetCfgIsScaleMetric]                     = scaleMetricN;
		configurationTags[fieldOffsetCfgNetWeightDualRangeLimitCheck]       = netWeightDualRangeLimitCheck;
		configurationNames[fieldOffsetCfgNetWeightDualRangeLimitCheck]      = netWeightDualRangeLimitCheckN;
		configurationTags[fieldOffsetCfgExternalWeightPrecision]            = externalWeightPrecision;
		configurationNames[fieldOffsetCfgExternalWeightPrecision]           = externalWeightPrecisionN;
		configurationTags[fieldOffsetCfgTareEntryPrecision]                 = tareEntryPrecision;
		configurationNames[fieldOffsetCfgTareEntryPrecision]                = tareEntryPrecisionN;
		configurationTags[fieldOffsetCfgUserRezeroWeightLimit]              = userRezeroWeightLimit;
		configurationNames[fieldOffsetCfgUserRezeroWeightLimit]             = userRezeroWeightLimitN;
		configurationTags[fieldOffsetCfgWeigherWakeupSensorAttached]        = weigherWakeupSensorAttached;
		configurationNames[fieldOffsetCfgWeigherWakeupSensorAttached]       = weigherWakeupSensorAttachedN;

		configurationTags[fieldOffsetCfgOperateLoginRequired]               = loginRequired;
		configurationNames[fieldOffsetCfgOperateLoginRequired]              = loginRequiredN;
		configurationNames[fieldOffsetCfgSupervisorLoginRequired]           = supervisorloginRequiredN;
		configurationTags[fieldOffsetCfgInactivityAutoLogoutTime]           = inactivityAutoLogoutTime;
		configurationNames[fieldOffsetCfgInactivityAutoLogoutTime]          = inactivityAutoLogoutTimeN;
		configurationTags[fieldOffsetCfgAutoLookupProduct]                  = autoLookupProduct;
		configurationNames[fieldOffsetCfgAutoLookupProduct]                 = autoLookupProductN;
		configurationTags[fieldOffsetCfgAutoLookupNumOfDigits]              = autoLookupNumDigits;
		configurationNames[fieldOffsetCfgAutoLookupNumOfDigits]             = autoLookupNumDigitsN;
		configurationTags[fieldOffsetCfgAutoLookupOverrideDigit]            = autoLookupOverrideDigit;
		configurationNames[fieldOffsetCfgAutoLookupOverrideDigit]           = autoLookupOverrideDigitN;
		configurationTags[fieldOffsetCfgAutoDisplayProductNotes]            = autoDisplayProductNotes;
		configurationNames[fieldOffsetCfgAutoDisplayProductNotes]           = autoDisplayProductNotesN;
		configurationTags[fieldOffsetCfgAutoDisplaySuggestiveSelling]       = autoDisplaySuggestiveSelling;
		configurationNames[fieldOffsetCfgAutoDisplaySuggestiveSelling]      = autoDisplaySuggestiveSellingN;
		configurationTags[fieldOffsetCfgAutoDisplayCookingNotes]            = autoDisplayCookingNotes;
		configurationNames[fieldOffsetCfgAutoDisplayCookingNotes]           = autoDisplayCookingNotesN;

		configurationTags[fieldOffsetCfgHorizLabelSkew1]                    = horizontalLabelSkew1;
		configurationNames[fieldOffsetCfgHorizLabelSkew1]                   = horizontalLabelSkew1N;
		configurationTags[fieldOffsetCfgHorizLabelSkew2]                    = horizontalLabelSkew2;
		configurationNames[fieldOffsetCfgHorizLabelSkew2]                   = horizontalLabelSkew2N;
		configurationTags[fieldOffsetCfgHorizLabelSkew3]                    = horizontalLabelSkew3;
		configurationNames[fieldOffsetCfgHorizLabelSkew3]                   = horizontalLabelSkew3N;

		configurationTags[fieldOffsetCfgCollectingTransactions]             = isCollectingTransactions;
		configurationNames[fieldOffsetCfgCollectingTransactions]            = isCollectingTransactionsN;
		configurationTags[fieldOffsetCfgCollectingTransactionsDays]         = collectingTransactionsDays;
		configurationNames[fieldOffsetCfgCollectingTransactionsDays]        = collectingTransactionsDaysN;
		configurationTags[fieldOffsetCfgCollectingProductTotals]            = isCollectingProductTotals;
		configurationNames[fieldOffsetCfgCollectingProductTotals]           = isCollectingProductTotalsN;
		configurationTags[fieldOffsetCfgCollectingHourlyTotals]             = isCollectingHourlyTotals;
		configurationNames[fieldOffsetCfgCollectingHourlyTotals]            = isCollectingHourlyTotalsN;
		configurationTags[fieldOffsetCfgCollectingOperatorTotals]           = isCollectingOperatorTotals;
		configurationNames[fieldOffsetCfgCollectingOperatorTotals]          = isCollectingOperatorTotalsN;
		configurationTags[fieldOffsetCfgTrackingOperatorChanges]            = trackingOperatorChanges;
		configurationNames[fieldOffsetCfgTrackingOperatorChanges]           = trackingOperatorChangesN;
		configurationTags[fieldOffsetCfgTrackingSupervisorChanges]          = trackingSupervisorChanges;
		configurationNames[fieldOffsetCfgTrackingSupervisorChanges]         = trackingSupervisorChangesN;
//		configurationTags[fieldOffsetCfgDaysToKeepOperatorChanges]          = daysToKeepOperatorChanges;
//		configurationNames[fieldOffsetCfgDaysToKeepOperatorChanges]         = daysToKeepOperatorChangesN;
		configurationTags[fieldOffsetCfgCollectingCOOLTotals]               = collectingCOOLTotals;
		configurationNames[fieldOffsetCfgCollectingCOOLTotals]              = collectingCOOLTotalsN;

		configurationTags[fieldOffsetCfgOpAccessToBrowserMode]              = opAccessToBrowserMode;
		configurationNames[fieldOffsetCfgOpAccessToBrowserMode]             = opAccessToBrowserModeN;
		configurationTags[fieldOffsetCfgOpAccessToDesktopMode]              = opAccessToDesktopMode;
		configurationNames[fieldOffsetCfgOpAccessToDesktopMode]             = opAccessToDesktopModeN;

		configurationTags[fieldOffsetCfgAutoLoadOldFirmwareVersions]        = autoLoadOldFirmwareVersions;
		configurationNames[fieldOffsetCfgAutoLoadOldFirmwareVersions]       = autoLoadOldFirmwareVersionsN;
		configurationTags[fieldOffsetCfgAutoSearchNewFirmwareVersions]      = autoSearchNewFirmwareVersions;
		configurationNames[fieldOffsetCfgAutoSearchNewFirmwareVersions]     = autoSearchNewFirmwareVersionsN;
		configurationTags[fieldOffsetCfgNewFirmwareVersionSearchTime]       = newFirmwareVersionSearchTime;
		configurationNames[fieldOffsetCfgNewFirmwareVersionSearchTime]      = newFirmwareVersionSearchTimeN;
		configurationTags[fieldOffsetCfgForceUniqueProdClassNums]           = forceUniqueProdClassNums;
		configurationNames[fieldOffsetCfgForceUniqueProdClassNums]          = forceUniqueProdClassNumsN;
		configurationTags[fieldOffsetCfgEnablePowerSaverMode]               = enablePowerSaverMode;
		configurationNames[fieldOffsetCfgEnablePowerSaverMode]              = enablePowerSaverModeN;
		configurationTags[fieldOffsetCfgPowerSaverModeTimeout]              = powerSaverModeTimeout;
		configurationNames[fieldOffsetCfgPowerSaverModeTimeout]             = powerSaverModeTimeoutN;
		configurationTags[fieldOffsetCfgDefaultBrowserHomePage]             = defaultBrowserHomePage;
		configurationNames[fieldOffsetCfgDefaultBrowserHomePage]            = defaultBrowserHomePageN;
		configurationTags[fieldOffsetCfgDefaultRunScreenHomePage]           = defaultRunScreenHomePage;
		configurationNames[fieldOffsetCfgDefaultRunScreenHomePage]          = defaultRunScreenHomePageN;
		configurationTags[fieldOffsetCfgDefaultProductRecordType]           = defaultProductRecordType;
		configurationNames[fieldOffsetCfgDefaultProductRecordType]          = defaultProductRecordTypeN;

		configurationTags[fieldOffsetCfgDefWrFilmWidth]                     = defWrFilmWidth;
		configurationNames[fieldOffsetCfgDefWrFilmWidth]                    = defWrFilmWidthN;
		configurationTags[fieldOffsetCfgDefWrFilmTension]                   = defWrFilmTension;
		configurationNames[fieldOffsetCfgDefWrFilmTension]                  = defWrFilmTensionN;
		configurationTags[fieldOffsetCfgDefWrFilmTensionFrontToBack]        = defWrFilmTensionFrontToBack;
		configurationNames[fieldOffsetCfgDefWrFilmTensionFrontToBack]       = defWrFilmTensionFrontToBackN;
		configurationTags[fieldOffsetCfgDefWrFilmTensionLeftToRight]        = defWrFilmTensionLeftToRight;
		configurationNames[fieldOffsetCfgDefWrFilmTensionLeftToRight]       = defWrFilmTensionLeftToRightN;
		configurationTags[fieldOffsetCfgDefWrIntakeMode]                    = defWrIntakeMode;
		configurationNames[fieldOffsetCfgDefWrIntakeMode]                   = defWrIntakeModeN;
		configurationTags[fieldOffsetCfgDefWrLabelPlacement]                = defWrLabelPlacement;
		configurationTags[fieldOffsetCfgDefWrSecLabelPlacement]				= defWrSecLabelPlacement;
		configurationNames[fieldOffsetCfgDefWrLabelPlacement]               = defWrLabelPlacementN;
		configurationNames[fieldOffsetCfgDefWrSecLabelPlacement]			= defWrSecLabelPlacementN;
		configurationTags[fieldOffsetCfgDefWrApplierRotation]               = defWrApplierRotation;
		configurationTags[fieldOffsetCfgDefWrSecApplierRotation]            = defWrSecApplierRotation;
		configurationNames[fieldOffsetCfgDefWrApplierRotation]              = defWrApplierRotationN;
		configurationNames[fieldOffsetCfgDefWrSecApplierRotation]			= defWrSecApplierRotationN;

		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnSun]              = wrSealerBeltTimeOnSun;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnSun]             = wrSealerBeltTimeOnSunN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffSun]             = wrSealerBeltTimeOffSun;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffSun]            = wrSealerBeltTimeOffSunN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnMon]              = wrSealerBeltTimeOnMon;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnMon]             = wrSealerBeltTimeOnMonN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffMon]             = wrSealerBeltTimeOffMon;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffMon]            = wrSealerBeltTimeOffMonN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnTue]              = wrSealerBeltTimeOnTue;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnTue]             = wrSealerBeltTimeOnTueN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffTue]             = wrSealerBeltTimeOffTue;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffTue]            = wrSealerBeltTimeOffTueN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnWed]              = wrSealerBeltTimeOnWed;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnWed]             = wrSealerBeltTimeOnWedN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffWed]             = wrSealerBeltTimeOffWed;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffWed]            = wrSealerBeltTimeOffWedN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnThu]              = wrSealerBeltTimeOnThu;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnThu]             = wrSealerBeltTimeOnThuN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffThu]             = wrSealerBeltTimeOffThu;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffThu]            = wrSealerBeltTimeOffThuN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnFri]              = wrSealerBeltTimeOnFri;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnFri]             = wrSealerBeltTimeOnFriN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffFri]             = wrSealerBeltTimeOffFri;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffFri]            = wrSealerBeltTimeOffFriN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOnSat]              = wrSealerBeltTimeOnSat;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOnSat]             = wrSealerBeltTimeOnSatN;
		configurationTags[fieldOffsetCfgWrSealerBeltTimeOffSat]             = wrSealerBeltTimeOffSat;
		configurationNames[fieldOffsetCfgWrSealerBeltTimeOffSat]            = wrSealerBeltTimeOffSatN;

		configurationTags[fieldOffsetCfgDbBackupFrequency]                  = diskWriteFrequency;
		configurationNames[fieldOffsetCfgDbBackupFrequency]                 = diskWriteFrequencyN;
		configurationTags[fieldOffsetCfgAutoCompressDB]                     = autoCompressDB;
		configurationNames[fieldOffsetCfgAutoCompressDB]                    = autoCompressDBN;
		configurationTags[fieldOffsetCfgCompressDBTime]                     = compressDBTime;
		configurationNames[fieldOffsetCfgCompressDBTime]                    = compressDBTimeN;

		configurationTags[fieldOffsetCfgCOOLTrackingNumberEnabled]          = coolTrackingNumberEnabled;
		configurationNames[fieldOffsetCfgCOOLTrackingNumberEnabled]         = coolTrackingNumberEnabledN;
		configurationTags[fieldOffsetCfgDontChangeCurrentCOOL]              = dontChangeCurrentCOOL;
		configurationNames[fieldOffsetCfgDontChangeCurrentCOOL]             = dontChangeCurrentCOOLN;

		configurationTags[fieldOffsetCfgPrimaryExpirationPageNote]          = primaryExpirationPageNote;
		configurationNames[fieldOffsetCfgPrimaryExpirationPageNote]         = primaryExpirationPageNoteN;
		configurationTags[fieldOffsetCfgSecondExpirationPageNote]           = secondExpirationPageNote;
		configurationNames[fieldOffsetCfgSecondExpirationPageNote]          = secondExpirationPageNoteN;

		configurationTags[fieldOffsetCfgCheckNetworkConnectivity]           = checkingNetworkConnectivity;
		configurationNames[fieldOffsetCfgCheckNetworkConnectivity]          = checkingNetworkConnectivityN;
		configurationTags[fieldOffsetCfgOfflineDateTime]                    = offlineDateTime;
		configurationNames[fieldOffsetCfgOfflineDateTime]                   = offlineDateTimeN;

		configurationTags[fieldOffsetCfgScaleType]                          = scaleType;
		configurationNames[fieldOffsetCfgScaleType]                         = scaleTypeN;

		configurationTags[fieldOffsetCfgCommunicationHistory]               = communicationHistory;
		configurationNames[fieldOffsetCfgCommunicationHistory]              = communicationHistoryN;
		configurationTags[fieldOffsetCfgCommunicationItemReceived]          = communicationItemReceived;
		configurationNames[fieldOffsetCfgCommunicationItemReceived]         = communicationItemReceivedN;

		configurationTags[fieldOffsetCfgPopupKeypadEnabled]                 = popupKeypadEnabled;
		configurationNames[fieldOffsetCfgPopupKeypadEnabled]                = popupKeypadEnabledN;
		configurationTags[fieldOffsetCfgPopupKeypadStyle]                   = popupKeypadStyle;
		configurationNames[fieldOffsetCfgPopupKeypadStyle]                  = popupKeypadStyleN;
		configurationTags[fieldOffsetCfgPopupKeypadLocation]                = popupKeypadLocation;
		configurationNames[fieldOffsetCfgPopupKeypadLocation]               = popupKeypadLocationN;

		configurationTags[fieldOffsetCfgOperatorLoginKeyboardStyle]         = operatorLoginKeyboardStyle;
		configurationNames[fieldOffsetCfgOperatorLoginKeyboardStyle]        = operatorLoginKeyboardStyleN;

		configurationTags[fieldOffsetCfgCapacityBarLocation]                = capacityBarLocation;
		configurationNames[fieldOffsetCfgCapacityBarLocation]               = capacityBarLocationN;

		configurationTags[fieldOffsetCfgPingPrimary]						 = pingPrimary;
		configurationNames[fieldOffsetCfgPingPrimary]				         = pingPrimaryN;
		configurationTags[fieldOffsetCfgPingGateway]						 = pingGateway;
		configurationNames[fieldOffsetCfgPingGateway]				         = pingGatewayN;
		configurationTags[fieldOffsetCfgPingTarget]							 = pingTarget;
		configurationNames[fieldOffsetCfgPingTarget]				         = pingTargetN;
		configurationTags[fieldOffsetCfgPingInterval]						 = pingInterval;
		configurationNames[fieldOffsetCfgPingInterval]				         = pingIntervalN;
		configurationTags[fieldOffsetCfgPingTargetIPAddress]				 = pingTargetIPAddress;
		configurationNames[fieldOffsetCfgPingTargetIPAddress]		         = pingTargetIPAddressN;

		configurationTags[fieldOffsetCfgRebootUnprotectedAllowed]           = rebootUnprotectedAllowed;
		configurationNames[fieldOffsetCfgRebootUnprotectedAllowed]          = rebootUnprotectedAllowedN;

		configurationTags[fieldOffsetCfgAutoCreateRFIDOperator]             = autoCreateRFIDOperator;
		configurationNames[fieldOffsetCfgAutoCreateRFIDOperator]            = autoCreateRFIDOperatorN;
		
		configurationTags[fieldOffsetCfgFirmwareVersion]					 = firmwareVersion;
		configurationNames[fieldOffsetCfgFirmwareVersion]					 = firmwareVersionN;
		configurationTags[fieldOffsetCfgFirmwareRevision]					 = firmwareRevision;
		configurationNames[fieldOffsetCfgFirmwareRevision]					 = firmwareRevisionN;

		configurationTags[fieldOffsetCfgIgnoreUltimaFormatLabelTypes]		 = ignoreUltimaFormatLabelTypes;
		configurationNames[fieldOffsetCfgIgnoreUltimaFormatLabelTypes]		 = ignoreUltimaFormatLabelTypesN;

		configurationTags[fieldOffsetCfgReportsOffsetTop]					 = reportsOffsetTop;
		configurationNames[fieldOffsetCfgReportsOffsetTop]			         = reportsOffsetTopN;
		configurationTags[fieldOffsetCfgReportsOffsetBottom]				 = reportsOffsetBottom;
		configurationNames[fieldOffsetCfgReportsOffsetBottom]		         = reportsOffsetBottomN;

		configurationTags[fieldOffsetCfgShowDeleteAllFiles]                 = showDeleteAllFiles;
		configurationNames[fieldOffsetCfgShowDeleteAllFiles]                = showDeleteAllFilesN;
		configurationTags[fieldOffsetCfgShowPrinterSettings]                = showPrinterSettings;
		configurationNames[fieldOffsetCfgShowPrinterSettings]               = showPrinterSettingsN;

		configurationTags[fieldOffsetCfgRemoteAccessPermitted]             = remoteAccessPermitted;
		configurationNames[fieldOffsetCfgRemoteAccessPermitted]            = remoteAccessPermittedN;
		configurationTags[fieldOffsetCfgRemoteAccessRequiresLogin]         = remoteAccessRequiresLogin;
		configurationNames[fieldOffsetCfgRemoteAccessRequiresLogin]        = remoteAccessRequiresLoginN;
		configurationTags[fieldOffsetCfgRemoteAccessLoginMethod]           = remoteAccessLoginMethod;
		configurationNames[fieldOffsetCfgRemoteAccessLoginMethod]          = remoteAccessLoginMethodN;
		configurationTags[fieldOffsetCfgRemoteAccessUsername]              = remoteAccessUsername;
		configurationNames[fieldOffsetCfgRemoteAccessUsername]             = remoteAccessUsernameN;
		configurationTags[fieldOffsetCfgRemoteAccessPassword]              = remoteAccessPassword;
		configurationNames[fieldOffsetCfgRemoteAccessPassword]             = remoteAccessPasswordN;

//		configurationTags[fieldOffsetCfgIRemoteScaleMaintenanceEnabled]	   = remoteScaleMaintenanceEnabled;
//		configurationNames[fieldOffsetCfgIRemoteScaleMaintenanceEnabled]   = remoteScaleMaintenanceEnabledN;
//		configurationTags[fieldOffsetCfgIRemoteScaleMaintClrOutboxDays]	   = remoteScaleMaintClrOutboxDays;
//		configurationNames[fieldOffsetCfgIRemoteScaleMaintClrOutboxDays]   = remoteScaleMaintClrOutboxDaysN;

		configurationTags[fieldOffsetCfgBackdateByDays]                    = backdateByDays;
		configurationNames[fieldOffsetCfgBackdateByDays]                   = backdateByDaysN;

		configurationTags[fieldOffsetCfgUnitSeparator]                     = unitSeparator;
		configurationNames[fieldOffsetCfgUnitSeparator]                    = unitSeparatorN;

		configurationTags[fieldOffsetCfgSystemFont]                        = systemFont;
		configurationNames[fieldOffsetCfgSystemFont]                       = systemFontN;
		configurationTags[fieldOffsetCfgStartScreenHome]                   = startScreenHome;
		configurationNames[fieldOffsetCfgStartScreenHome]                  = startScreenHomeN;
		configurationTags[fieldOffsetCfgBrowserHasAddressBar]              = browserHasAddressBar;
		configurationNames[fieldOffsetCfgBrowserHasAddressBar]             = browserHasAddressBarN;
		configurationTags[fieldOffsetCfgDefaultSearchMethod]               = defaultSearchMethod;
		configurationNames[fieldOffsetCfgDefaultSearchMethod]              = defaultSearchMethodN;
		configurationTags[fieldOffsetCfgResetStatisticsDays]               = resetStatisticsDays;
		configurationNames[fieldOffsetCfgResetStatisticsDays]              = resetStatisticsDaysN;

		configurationTags[fieldOffsetCfgLeftRunscreenContent]              = leftRunscreenContent;
		configurationNames[fieldOffsetCfgLeftRunscreenContent]             = leftRunscreenContentN;
		configurationTags[fieldOffsetCfgMiddleRunscreenDynamicContent]     = runscreenDynamicContent;
		configurationNames[fieldOffsetCfgMiddleRunscreenDynamicContent]    = runscreenDynamicContentN;
		configurationTags[fieldOffsetCfgMiddleRunscreenKeypadContent]      = runscreenKeypadContent;
		configurationNames[fieldOffsetCfgMiddleRunscreenKeypadContent]     = runscreenKeypadContentN;
		configurationTags[fieldOffsetCfgRightScreenContent]                = rightScreenContent;
		configurationNames[fieldOffsetCfgRightScreenContent]               = rightScreenContentN;

		configurationTags[fieldOffsetCfgEnableDisplayNetworkDetails]       = enableDisplayNetworkDetails;
		configurationNames[fieldOffsetCfgEnableDisplayNetworkDetails]      = enableDisplayNetworkDetailsN;

		configurationTags[fieldOffsetCfgDisableOperatorFlashkeyEditor]     = disableOperatorFlashkeyEditor;
		configurationNames[fieldOffsetCfgDisableOperatorFlashkeyEditor]    = disableOperatorFlashkeyEditorN;
		configurationTags[fieldOffsetCfgDisableLeftRunscreen]              = disableLeftRunscreen;
		configurationNames[fieldOffsetCfgDisableLeftRunscreen]             = disableLeftRunscreenN;

		configurationTags[fieldOffsetCfgMostRecentRunMode]                 = mostRecentRunMode;
		configurationNames[fieldOffsetCfgMostRecentRunMode]                = mostRecentRunModeN;

		configurationTags[fieldOffsetCfgDisplayTransTypeIndicator]         = displayTransTypeIndicator;
		configurationNames[fieldOffsetCfgDisplayTransTypeIndicator]        = displayTransTypeIndicatorN;
		configurationTags[fieldOffsetCfgNowServingQueueIPAddress]          = nowServingQueueIPAddress;
		configurationNames[fieldOffsetCfgNowServingQueueIPAddress]         = nowServingQueueIPAddressN;
		configurationTags[fieldOffsetCfgNowServingSysType]                 = nowServingSysType;
		configurationNames[fieldOffsetCfgNowServingSysType]                = nowServingSysTypeN;

		//GCD
		configurationTags[fieldOffsetCfgGCDLayoutNumber]				 = gcdLayoutNumber;
		configurationNames[fieldOffsetCfgGCDLayoutNumber]				 = gcdLayoutNumberN;
		configurationTags[fieldOffsetCfgGCDColorSchemeNumber]			 = gcdColorSchemeNumber;
		configurationNames[fieldOffsetCfgGCDColorSchemeNumber]			 = gcdColorSchemeNumberN;
		configurationTags[fieldOffsetCfgGCDAllowOperReset]				 = gcdAllowOperReset;
		configurationNames[fieldOffsetCfgGCDAllowOperReset]				 = gcdAllowOperResetN;

		configurationTags[fieldOffsetCfgGCDVolume]						= gcdVolume;				
		configurationNames[fieldOffsetCfgGCDVolume]						= gcdVolumeN;			
		configurationTags[fieldOffsetCfgGCDPlaybackWaitTime]			= gcdPlaybackWaitTime;	
		configurationNames[fieldOffsetCfgGCDPlaybackWaitTime]			= gcdPlaybackWaitTimeN;	
		configurationTags[fieldOffsetCfgGCDMediaServerName]				= gcdMediaServerName;	
		configurationNames[fieldOffsetCfgGCDMediaServerName]			= gcdMediaServerNameN;	
		configurationTags[fieldOffsetCfgGCDMediaServerIP]				= gcdMediaServerIP;		
		configurationNames[fieldOffsetCfgGCDMediaServerIP]				= gcdMediaServerNamIPN;	
		configurationTags[fieldOffsetCfgGCDFtpUserName]					= gcdFTPUserName;		
		configurationNames[fieldOffsetCfgGCDFtpUserName]				= gcdFTPUserNameN;		
		configurationTags[fieldOffsetCfgGCDFtpPassword]					= gcdFTPPassword;		
		configurationNames[fieldOffsetCfgGCDFtpPassword]				= gcdFTPPasswordN;		
		configurationTags[fieldOffsetCfgGCDFtpDirectory]				= gcdFTPDirectory;		
		configurationNames[fieldOffsetCfgGCDFtpDirectory]				= gcdFTPDirectoryN;		
		configurationTags[fieldOffsetCfgGCDPlaylistFile]				= gcdPlaylistFilename;	
		configurationNames[fieldOffsetCfgGCDPlaylistFile]				= gcdPlaylistFilenameN;	
		configurationTags[fieldOffsetCfgGCDAutoSearch]					= gcdAutoSearch;			
		configurationNames[fieldOffsetCfgGCDAutoSearch]					= gcdAutoSearchN;				
		configurationTags[fieldOffsetCfgGCDSearchHour]					= gcdSearchHour;			
		configurationNames[fieldOffsetCfgGCDSearchHour]					= gcdSearchHourN;		
		configurationTags[fieldOffsetCfgGCDSearchMinute]				= gcdSearchMinute;			
		configurationNames[fieldOffsetCfgGCDSearchMinute]				= gcdSearchMinuteN;		
		configurationTags[fieldOffsetCfgGCDSearchAmOrPM]				= gcdAutoAMPM;			
		configurationNames[fieldOffsetCfgGCDSearchAmOrPM]				= gcdAutoAMPMN;			
		configurationTags[fieldOffsetCfgGCDIsDnsEnabled]				= gcdDnsEnabled;
		configurationNames[fieldOffsetCfgGCDIsDnsEnabled]				= gcdDnsEnabledN;		
		configurationTags[fieldOffsetCfgGCDDownloadProtocol]			= gcdFTPProtocol;	
		configurationNames[fieldOffsetCfgGCDDownloadProtocol]			= gcdFTPProtocolN;	
		configurationTags[fieldOffsetCfgGCDLanguage]					= gcdLanguage;	
		configurationNames[fieldOffsetCfgGCDLanguage]					= gcdLanguageN;	
		configurationTags[fieldOffsetCfgGCDPrintWithoutCustomerDisplay]	 = printWithoutCustomerDisplay;	
		configurationNames[fieldOffsetCfgGCDPrintWithoutCustomerDisplay] = printWithoutCustomerDisplayN;	

		configurationTags[fieldOffsetCfgGCDUseTransactionalLayout]      = gcdUseTransactionalLayout;
		configurationNames[fieldOffsetCfgGCDUseTransactionalLayout]     = gcdUseTransactionalLayoutN;
		configurationTags[fieldOffsetCfgGCDIdleAdvertType]              = gcdIdleAdvertType;
		configurationNames[fieldOffsetCfgGCDIdleAdvertType]             = gcdIdleAdvertTypeN;
		configurationTags[fieldOffsetCfgGCDIdleWebsiteAddress]          = gcdIdleWebsiteAddress;
		configurationNames[fieldOffsetCfgGCDIdleWebsiteAddress]         = gcdIdleWebsiteAddressN;
		configurationTags[fieldOffsetCfgGCDProductAdvertType]           = gcdProductAdvertType;
		configurationNames[fieldOffsetCfgGCDProductAdvertType]          = gcdProductAdvertTypeN;

// CfgFrequentShopper
		configurationTags[fieldOffsetCfgFSRoundingMethod]                = frequentShopperRoundingMethod; 
		configurationNames[fieldOffsetCfgFSRoundingMethod]               = frequentShopperRoundingMethodN; 
		configurationTags[fieldOffsetCfgFSDiscountMethod]                = discountMethod;	//	Original FS Method. Determines if perScale or perProduct
		configurationNames[fieldOffsetCfgFSDiscountMethod]               = discountMethodN;	//	Original FS Method. Determines if perScale or perProduct
		configurationTags[fieldOffsetCfgFSDiscountType]                  = discountType; 
		configurationNames[fieldOffsetCfgFSDiscountType]                 = discountTypeN; 
		configurationTags[fieldOffsetCfgFSDiscountFormula]               = discountFormula; 
		configurationNames[fieldOffsetCfgFSDiscountFormula]              = discountFormulaN;
		configurationNames[fieldOffsetCfgFSPerScale]                     = frequentShopperPerScaleN;
		configurationTags[fieldOffsetCfgFSTotalPriceText1]               = totalPriceText1; 
		configurationNames[fieldOffsetCfgFSTotalPriceText1]              = totalPriceText1N; 
		configurationTags[fieldOffsetCfgFSTotalPriceText2]               = totalPriceText2; 
		configurationNames[fieldOffsetCfgFSTotalPriceText2]              = totalPriceText2N; 
		configurationTags[fieldOffsetCfgFSSavingsText1]                  = savingsText1; 
		configurationNames[fieldOffsetCfgFSSavingsText1]                 = savingsText1N;
		configurationTags[fieldOffsetCfgFSSavingsText2]                  = savingsText2; 
		configurationNames[fieldOffsetCfgFSSavingsText2]                 = savingsText2N;
		configurationTags[fieldOffsetCfgFSUnitPriceText1]                = unitPriceText1; 
		configurationNames[fieldOffsetCfgFSUnitPriceText1]               = unitPriceText1N; 
		configurationTags[fieldOffsetCfgFSUnitPriceText2]                = unitPriceText2; 
		configurationNames[fieldOffsetCfgFSUnitPriceText2]               = unitPriceText2N; 
		configurationTags[fieldOffsetCfgFSRoundingWeightPrecision]       = fsRoundingWeightPrecision; 
		configurationNames[fieldOffsetCfgFSRoundingWeightPrecision]      = fsRoundingWeightPrecisionN; 

// CfgOperatorAccess
		configurationTags[fieldOffsetCfgOAPriceModifiable]               = priceModifiable;
		configurationNames[fieldOffsetCfgOAPriceModifiable]              = priceModifiableN;
		configurationTags[fieldOffsetCfgOAByCountModifiable]             = byCountModifiable;
		configurationNames[fieldOffsetCfgOAByCountModifiable]            = byCountModifiableN;
		configurationTags[fieldOffsetCfgOATareModifiable]                = tareModifiable;
		configurationNames[fieldOffsetCfgOATareModifiable]               = tareModifiableN;
		configurationTags[fieldOffsetCfgOAFixedWeightModifiable]         = fixedWeightModifiable;
		configurationNames[fieldOffsetCfgOAFixedWeightModifiable]        = fixedWeightModifiableN;
		configurationTags[fieldOffsetCfgOAShelfLifeModifiable]           = shelfLifeModifiable;
		configurationNames[fieldOffsetCfgOAShelfLifeModifiable]          = shelfLifeModifiableN;
		configurationTags[fieldOffsetCfgOAProductLifeModifiable]         = productLifeModifiable;
		configurationNames[fieldOffsetCfgOAProductLifeModifiable]        = productLifeModifiableN;
		configurationTags[fieldOffsetCfgOALabelTypeModifiable]           = labelTypeModifiable;
		configurationNames[fieldOffsetCfgOALabelTypeModifiable]          = labelTypeModifiableN;
		configurationTags[fieldOffsetCfgOADateTimeModifiable]            = dateTimeModifiable;
		configurationNames[fieldOffsetCfgOADateTimeModifiable]           = dateTimeModifiableN;
		configurationTags[fieldOffsetCfgOAPortionsModifiable]            = portionsModifiable;
		configurationNames[fieldOffsetCfgOAPortionsModifiable]           = portionsModifiableN;

		configurationTags[fieldOffsetCfgOAPriceAccessLevel]               = priceAccessLevel;
		configurationNames[fieldOffsetCfgOAPriceAccessLevel]              = priceAccessLevelN;
		configurationTags[fieldOffsetCfgOAByCountAccessLevel]             = byCountAccessLevel;
		configurationNames[fieldOffsetCfgOAByCountAccessLevel]            = byCountAccessLevelN;
		configurationTags[fieldOffsetCfgOATareAccessLevel]                = tareAccessLevel;
		configurationNames[fieldOffsetCfgOATareAccessLevel]               = tareAccessLevelN;
		configurationTags[fieldOffsetCfgOAFixedWeightAccessLevel]         = fixedWeightAccessLevel;
		configurationNames[fieldOffsetCfgOAFixedWeightAccessLevel]        = fixedWeightAccessLevelN;
		configurationTags[fieldOffsetCfgOAShelfLifeAccessLevel]           = shelfLifeAccessLevel;
		configurationNames[fieldOffsetCfgOAShelfLifeAccessLevel]          = shelfLifeAccessLevelN;
		configurationTags[fieldOffsetCfgOAProductLifeAccessLevel]         = productLifeAccessLevel;
		configurationNames[fieldOffsetCfgOAProductLifeAccessLevel]        = productLifeAccessLevelN;
		configurationTags[fieldOffsetCfgOALabelTypeAccessLevel]           = labelTypeAccessLevel;
		configurationNames[fieldOffsetCfgOALabelTypeAccessLevel]          = labelTypeAccessLevelN;
		configurationTags[fieldOffsetCfgOADateTimeAccessLevel]            = dateTimeAccessLevel;
		configurationNames[fieldOffsetCfgOADateTimeAccessLevel]           = dateTimeAccessLevelN;
		configurationTags[fieldOffsetCfgOAPortionsAccessLevel]            = portionsAccessLevel;
		configurationNames[fieldOffsetCfgOAPortionsAccessLevel]           = portionsAccessLevelN;


// CfgPrintedFields
		configurationTags[fieldOffsetCfgPFSellByDatePrinted]             = sellByDatePrinted;
		configurationNames[fieldOffsetCfgPFSellByDatePrinted]            = sellByDatePrintedN;
		configurationTags[fieldOffsetCfgPFSellByDateJulian]              = sellByDateJulian;
		configurationNames[fieldOffsetCfgPFSellByDateJulian]             = sellByDateJulianN;
		configurationTags[fieldOffsetCfgPFPackedOnDatePrinted]           = packedOnDatePrinted;
		configurationNames[fieldOffsetCfgPFPackedOnDatePrinted]          = packedOnDatePrintedN;
		configurationTags[fieldOffsetCfgPFPackedOnDateJulian]            = packedOnDateJulian;
		configurationNames[fieldOffsetCfgPFPackedOnDateJulian]           = packedOnDateJulianN;
		configurationTags[fieldOffsetCfgPFProductLifeDatePrinted]        = productLifeDatePrinted;
		configurationNames[fieldOffsetCfgPFProductLifeDatePrinted]       = productLifeDatePrintedN;
		configurationTags[fieldOffsetCfgPFProductLifeDateJulian]         = productLifeDateJulian;
		configurationNames[fieldOffsetCfgPFProductLifeDateJulian]        = productLifeDateJulianN;
		configurationTags[fieldOffsetCfgPFUnitPricePrinted]              = unitPricePrinted;
		configurationNames[fieldOffsetCfgPFUnitPricePrinted]             = unitPricePrintedN;
		configurationTags[fieldOffsetCfgPFByCountPrinted]                = byCountPrinted;
		configurationNames[fieldOffsetCfgPFByCountPrinted]               = byCountPrintedN;
		configurationTags[fieldOffsetCfgPFBarCodePrinted]                = barCodePrinted;
		configurationNames[fieldOffsetCfgPFBarCodePrinted]               = barCodePrintedN;
		configurationTags[fieldOffsetCfgPFCurrencySymbolPrinted]         = currencySymbolPrinted;
		configurationNames[fieldOffsetCfgPFCurrencySymbolPrinted]        = currencySymbolPrintedN;
		configurationTags[fieldOffsetCfgPFTextNamesPreprinted]           = textNamesPreprinted;
		configurationNames[fieldOffsetCfgPFTextNamesPreprinted]          = textNamesPreprintedN;
		configurationTags[fieldOffsetCfgPFWeightOnByCountPrinted]        = weightOnByCountPrinted;
		configurationNames[fieldOffsetCfgPFWeightOnByCountPrinted]       = weightOnByCountPrintedN;
		configurationTags[fieldOffsetCfgPFOneOnByCountPrinted]           = oneOnByCountPrinted;
		configurationNames[fieldOffsetCfgPFOneOnByCountPrinted]          = oneOnByCountPrintedN;
		configurationTags[fieldOffsetCfgPFSecondCheckDigit]              = usingSecondCheckDigit;
		configurationNames[fieldOffsetCfgPFSecondCheckDigit]             = usingSecondCheckDigitN;
		configurationTags[fieldOffsetCfgPFPrintFirstLetterOfTotalType]   = printFirstLetterOfTotalType;
		configurationNames[fieldOffsetCfgPFPrintFirstLetterOfTotalType]  = printFirstLetterOfTotalTypeN;
		configurationTags[fieldOffsetCfgPFNetWtOnFixedWeightAlways]  	 = alwaysPrintNetWtOnFixedWt;
		configurationNames[fieldOffsetCfgPFNetWtOnFixedWeightAlways]	 = alwaysPrintNetWtOnFixedWtN;
		configurationTags[fieldOffsetCfgPFFixedWeightFormat] 		 	 = fixedWeightFormat;
		configurationNames[fieldOffsetCfgPFFixedWeightFormat]			 = fixedWeightFormatN;

// CfgSelfService
		configurationTags[fieldOffsetCfgGridXElements]                      = gridXElements;
		configurationNames[fieldOffsetCfgGridXElements]                     = gridXElementsN;
		configurationTags[fieldOffsetCfgGridYElements]                      = gridYElements;
		configurationNames[fieldOffsetCfgGridYElements]                     = gridYElementsN;
		configurationTags[fieldOffsetCfgNumOfMostCommonCategories]          = numOfMostCommonCategories;
		configurationNames[fieldOffsetCfgNumOfMostCommonCategories]         = numOfMostCommonCategoriesN;
		configurationTags[fieldOffsetCfgImageTypeOnStartScreen]             = imageTypeOnStartScreen;
		configurationNames[fieldOffsetCfgImageTypeOnStartScreen]            = imageTypeOnStartScreenN;
		configurationTags[fieldOffsetCfgInitialStreamingGraphicNum]         = initialStreamingGraphicNum;
		configurationNames[fieldOffsetCfgInitialStreamingGraphicNum]        = initialStreamingGraphicNumN;
		configurationTags[fieldOffsetCfgProductStatsHoursMethod]            = productStatsHoursMethod;
		configurationNames[fieldOffsetCfgProductStatsHoursMethod]           = productStatsHoursMethodN;
		configurationTags[fieldOffsetCfgSelfServeInactivityTimeout]         = selfServeInactivityTimeout;
		configurationNames[fieldOffsetCfgSelfServeInactivityTimeout]        = selfServeInactivityTimeoutN;

		configurationTags[fieldOffsetCfgLogoGraphicFileName]                = logoGraphicFileName;
		configurationNames[fieldOffsetCfgLogoGraphicFileName]               = logoGraphicFileNameN;
		configurationTags[fieldOffsetCfgNoGraphicAvailableFileName1]        = noGraphicAvailableFileName1;
		configurationNames[fieldOffsetCfgNoGraphicAvailableFileName1]       = noGraphicAvailableFileName1N;
		configurationTags[fieldOffsetCfgNoGraphicAvailableFileName2]        = noGraphicAvailableFileName2;
		configurationNames[fieldOffsetCfgNoGraphicAvailableFileName2]       = noGraphicAvailableFileName2N;
		configurationTags[fieldOffsetCfgPreviousGraphicFileName1]           = previousGraphicFileName1;
		configurationNames[fieldOffsetCfgPreviousGraphicFileName1]          = previousGraphicFileName1N;
		configurationTags[fieldOffsetCfgPreviousGraphicFileName2]           = previousGraphicFileName2;
		configurationNames[fieldOffsetCfgPreviousGraphicFileName2]          = previousGraphicFileName2N;
		configurationTags[fieldOffsetCfgNextGraphicFileName1]               = nextGraphicFileName1;
		configurationNames[fieldOffsetCfgNextGraphicFileName1]              = nextGraphicFileName1N;
		configurationTags[fieldOffsetCfgNextGraphicFileName2]               = nextGraphicFileName2;
		configurationNames[fieldOffsetCfgNextGraphicFileName2]              = nextGraphicFileName2N;

//  CfgNetwork
/*
		//configurationTags[fieldOffsetCfgEthernetInterfaceName]            = ethernetInterfaceName;
		configurationNames[fieldOffsetCfgEthernetInterfaceName]             = ethernetInterfaceNameN;
		configurationNames[fieldOffsetCfgMacAddress]                        = macAddressN;
		//configurationTags[fieldOffsetCfgIpAddress]                        = iPAddress; 
		configurationNames[fieldOffsetCfgIpAddress]                         = iPAddressN; 
		configurationNames[fieldOffsetCfgSubnetMask]                        = subnetMaskN;
		configurationNames[fieldOffsetCfgNetworkSetup]                      = networkSetupN;
		configurationTags[fieldOffsetCfgWellKnownPort]                      = iPPort;
		configurationNames[fieldOffsetCfgWellKnownPort]                     = iPPortN;

		configurationNames[fieldOffsetCfgGatewayIpAddress1]                 = gatewayIPAddress1N;
		configurationNames[fieldOffsetCfgNetworkIpAddress1]                 = networkMask1N;
		configurationNames[fieldOffsetCfgGatewayIpAddress2]                 = gatewayIPAddress2N;
		configurationNames[fieldOffsetCfgNetworkIpAddress2]                 = networkMask2N;
		configurationNames[fieldOffsetCfgGatewayIpAddress3]                 = gatewayIPAddress3N;
		configurationNames[fieldOffsetCfgNetworkIpAddress3]                 = networkMask3N;

		configurationNames[fieldOffsetCfgDnsEnabled]                        = dnsEnabledN;
		configurationNames[fieldOffsetCfgDnsAutomatic]                      = dnsAutomaticN;
		configurationNames[fieldOffsetCfgDnsHostName]                       = dnsHostNameN;
		configurationNames[fieldOffsetCfgDnsDomainName]                     = dnsDomainNameN;
		configurationNames[fieldOffsetCfgDnsIpAddress0]                     = dnsIPAddress0N;
		configurationNames[fieldOffsetCfgDnsIpAddress1]                     = dnsIPAddress1N;
		configurationNames[fieldOffsetCfgDnsIpAddress2]                     = dnsIPAddress2N;

		configurationNames[fieldOffsetCfgTftpAutomatic]                     = tftpAutomaticN;
		configurationNames[fieldOffsetCfgTftpServerIpAddress]               = tftpIPAddressN;
		configurationNames[fieldOffsetCfgTftpBootFilePath]                  = tftpBootFilePathN;

		configurationNames[fieldOffsetCfgHasWirelessCard]                   = hasWirelessCardN;
		configurationNames[fieldOffsetCfgWirelessCardName]                  = wirelessCardNameN;
		configurationNames[fieldOffsetCfgWirelessCardConnected]             = wirelessCardConnectedN;
		configurationTags[fieldOffsetCfgAccessPointId]                      = accessPointId;
		configurationNames[fieldOffsetCfgAccessPointId]                     = accessPointIdN;
		configurationTags[fieldOffsetCfgUseSharedKeyAuth]                   = sharedKeyAuth;
		configurationNames[fieldOffsetCfgUseSharedKeyAuth]                  = sharedKeyAuthN;
		configurationTags[fieldOffsetCfgWepEnabled]                         = wepEnabled;
		configurationNames[fieldOffsetCfgWepEnabled]                        = wepEnabledN;
		configurationTags[fieldOffsetCfgActiveKeyNum]                       = activeKeyNum;
		configurationNames[fieldOffsetCfgActiveKeyNum]                      = activeKeyNumN;
		configurationNames[fieldOffsetCfgEncryptionKey1Set]                 = encryptionKey1SetN;
		configurationNames[fieldOffsetCfgEncryptionKey2Set]                 = encryptionKey2SetN;
		configurationNames[fieldOffsetCfgEncryptionKey3Set]                 = encryptionKey3SetN;
		configurationNames[fieldOffsetCfgEncryptionKey4Set]                 = encryptionKey4SetN;
		configurationTags[fieldOffsetCfgEncryptionKey1]                     = encryptionKey1;
		configurationNames[fieldOffsetCfgEncryptionKey1]                    = encryptionKey1N;
		configurationTags[fieldOffsetCfgEncryptionKey2]                     = encryptionKey2;
		configurationNames[fieldOffsetCfgEncryptionKey2]                    = encryptionKey2N;
		configurationTags[fieldOffsetCfgEncryptionKey3]                     = encryptionKey3;
		configurationNames[fieldOffsetCfgEncryptionKey3]                    = encryptionKey3N;
		configurationTags[fieldOffsetCfgEncryptionKey4]                     = encryptionKey4;
		configurationNames[fieldOffsetCfgEncryptionKey4]                    = encryptionKey4N;
		configurationNames[fieldOffsetCfgVpnEnabled]                        = vpnEnabledN;
		configurationNames[fieldOffsetCfgVpnIPAddress]                      = vpnIPAddressN;
		configurationNames[fieldOffsetCfgIsLeapAvailable]                   = leapAvailableN;
		configurationTags[fieldOffsetCfgLeapEnabled]                        = leapEnabled;
		configurationNames[fieldOffsetCfgLeapEnabled]                       = leapEnabledN;
		configurationTags[fieldOffsetCfgLeapUserName]                       = leapUserName;
		configurationNames[fieldOffsetCfgLeapUserName]                      = leapUserNameN;
		configurationTags[fieldOffsetCfgLeapUserPassword]                   = leapUserPassword;
		configurationNames[fieldOffsetCfgLeapUserPassword]                  = leapUserPasswordN;

		configurationTags[fieldOffsetCfgVpnCompression]					 = vpnCompression;
		configurationNames[fieldOffsetCfgVpnCompression]			 		 = vpnCompressionN;
		configurationTags[fieldOffsetCfgVpnDomain]			 				 = vpnDomain;
		configurationNames[fieldOffsetCfgVpnDomain]			 			 = vpnDomainN;
		configurationTags[fieldOffsetCfgVpnEncryptedPassword]			 	 = vpnEncryptedPassword;
		configurationNames[fieldOffsetCfgVpnEncryptedPassword]		 		 = vpnEncryptedPasswordN;
		configurationTags[fieldOffsetCfgVpnUsername]			 			 = vpnUsername;
		configurationNames[fieldOffsetCfgVpnUsername]			 			 = vpnUsernameN;
		configurationTags[fieldOffsetCfgVpnVerifyServerCertificate]		 = vpnVerifyServerCertificate;
		configurationNames[fieldOffsetCfgVpnVerifyServerCertificate]		 = vpnVerifyServerCertificateN;
		configurationTags[fieldOffsetCfgVpnNumberOfServers]			 	 = vpnNumberOfServers;
		configurationNames[fieldOffsetCfgVpnNumberOfServers]			 	 = vpnNumberOfServersN;
		configurationTags[fieldOffsetCfgVpnDeviceTypeDescriptor]			 = vpnDeviceTypeDescriptor;
		configurationNames[fieldOffsetCfgVpnDeviceTypeDescriptor]			 = vpnDeviceTypeDescriptorN;
		configurationTags[fieldOffsetCfgVpnUserClass]			 			 = vpnUserClass;
		configurationNames[fieldOffsetCfgVpnUserClass]			 			 = vpnUserClassN;
		configurationTags[fieldOffsetCfgVpnMajorVerbVersion]			 	 = vpnMajorVerbVersion;
		configurationNames[fieldOffsetCfgVpnMajorVerbVersion]			 	 = vpnMajorVerbVersionN;
		configurationTags[fieldOffsetCfgVpnChooseFirstServerRandomly]		 = vpnChooseFirstServerRandomly;
		configurationNames[fieldOffsetCfgVpnChooseFirstServerRandomly]		 = vpnChooseFirstServerRandomlyN;
		configurationTags[fieldOffsetCfgVpnServerTCPPort]			 		 = vpnServerTCPPort;
		configurationNames[fieldOffsetCfgVpnServerTCPPort]			 		 = vpnServerTCPPortN;
		configurationTags[fieldOffsetCfgVpnServerIPAddress1]			 	 = vpnServerIPAddress1;
		configurationNames[fieldOffsetCfgVpnServerIPAddress1]			 	 = vpnServerIPAddress1N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress2]			 	 = vpnServerIPAddress2;
		configurationNames[fieldOffsetCfgVpnServerIPAddress2]			 	 = vpnServerIPAddress2N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress3]			 	 = vpnServerIPAddress3;
		configurationNames[fieldOffsetCfgVpnServerIPAddress3]			 	 = vpnServerIPAddress3N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress4]			 	 = vpnServerIPAddress4;
		configurationNames[fieldOffsetCfgVpnServerIPAddress4]			 	 = vpnServerIPAddress4N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress5]			 	 = vpnServerIPAddress5;
		configurationNames[fieldOffsetCfgVpnServerIPAddress5]			 	 = vpnServerIPAddress5N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress6]			 	 = vpnServerIPAddress6;
		configurationNames[fieldOffsetCfgVpnServerIPAddress6]			 	 = vpnServerIPAddress6N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress7]			 	 = vpnServerIPAddress7;
		configurationNames[fieldOffsetCfgVpnServerIPAddress7]			 	 = vpnServerIPAddress7N;
		configurationTags[fieldOffsetCfgVpnServerIPAddress8]			 	 = vpnServerIPAddress8;
		configurationNames[fieldOffsetCfgVpnServerIPAddress8]			 	 = vpnServerIPAddress8N;
*/

//  CfgEmail
		configurationTags[fieldOffsetCfgFrom]                               = from;
		configurationNames[fieldOffsetCfgFrom]                              = fromN;
		configurationTags[fieldOffsetCfgToList]                             = toList;
		configurationNames[fieldOffsetCfgToList]                            = toListN;
		configurationTags[fieldOffsetCfgSmtpIpAddrOrDnsName]                = smtpIpAddrOrDnsName;
		configurationNames[fieldOffsetCfgSmtpIpAddrOrDnsName]               = smtpIpAddrOrDnsNameN;


//  CfgSynchronization
		configurationTags[fieldOffsetCfgScaleSynchronizationType]        = scaleSynchronizationType;
		configurationNames[fieldOffsetCfgScaleSynchronizationType]       = scaleSynchronizationTypeN;
		configurationTags[fieldOffsetCfgPrimaryScale]                    = primaryScale;
		configurationNames[fieldOffsetCfgPrimaryScale]                   = primaryScaleN;



		configurationTags[fieldOffsetCfgSyncConfigurations]						= syncConfigurations;
		configurationNames[fieldOffsetCfgSyncConfigurations]						= syncConfigurationsN;

		configurationTags[fieldOffsetCfgSyncEmailAlerts]							= syncEmailAlerts;
		configurationNames[fieldOffsetCfgSyncEmailAlerts]							= syncEmailAlertsN;

		configurationTags[fieldOffsetCfgSyncProductDefaults]						= syncProductDefaults;
		configurationNames[fieldOffsetCfgSyncProductDefaults]						= syncProductDefaultsN;

		configurationTags[fieldOffsetCfgSyncClasses]								= syncClasses;
		configurationNames[fieldOffsetCfgSyncClasses]								= syncClassesN;

		configurationTags[fieldOffsetCfgSyncExpandedTexts]							= syncExpandedTexts;
		configurationNames[fieldOffsetCfgSyncExpandedTexts]						    = syncExpandedTextsN;

		configurationTags[fieldOffsetCfgSyncSpecialMessages]						= syncSpecialMessages;
		configurationNames[fieldOffsetCfgSyncSpecialMessages]						= syncSpecialMessagesN;

		configurationTags[fieldOffsetCfgSyncNutritionFacts]						    = syncNutritionFacts;
		configurationNames[fieldOffsetCfgSyncNutritionFacts]						= syncNutritionFactsN;

		configurationTags[fieldOffsetCfgSyncProductNotes]							= syncProductNotes;
		configurationNames[fieldOffsetCfgSyncProductNotes]							= syncProductNotesN;

		configurationTags[fieldOffsetCfgSyncMarqueeTexts]							= syncMarqueeTexts;
		configurationNames[fieldOffsetCfgSyncMarqueeTexts]							= syncMarqueeTextsN;

		configurationTags[fieldOffsetCfgSyncTransactions]							= syncTransactions;
		configurationNames[fieldOffsetCfgSyncTransactions]							= syncTransactionsN;

		configurationTags[fieldOffsetCfgSyncTransactionTypes]						= syncTransactionTypes;
		configurationNames[fieldOffsetCfgSyncTransactionTypes]						= syncTransactionTypesN;

		configurationTags[fieldOffsetCfgSyncCategories]							= syncCategories;
		configurationNames[fieldOffsetCfgSyncCategories]							= syncCategoriesN;

		configurationTags[fieldOffsetCfgSyncSpecials]								= syncSpecials;
		configurationNames[fieldOffsetCfgSyncSpecials]								= syncSpecialsN;

		configurationTags[fieldOffsetCfgSyncCoolCategories]						    = syncCoolCategories;
		configurationNames[fieldOffsetCfgSyncCoolCategories]						= syncCoolCategoriesN;

		configurationTags[fieldOffsetCfgSyncCoolCountries]							= syncCoolCountries;
		configurationNames[fieldOffsetCfgSyncCoolCountries]						    = syncCoolCountriesN;

		configurationTags[fieldOffsetCfgSyncCoolProductionSteps]                    = syncCoolProductionSteps;
		configurationNames[fieldOffsetCfgSyncCoolProductionSteps]                   = syncCoolProductionStepsN;

		configurationTags[fieldOffsetCfgSyncCoolPreTexts]							= syncCoolPreTexts;
		configurationNames[fieldOffsetCfgSyncCoolPreTexts]							= syncCoolPreTextsN;

		configurationTags[fieldOffsetCfgSyncCoolTexts]								= syncCoolTexts;
		configurationNames[fieldOffsetCfgSyncCoolTexts]							    = syncCoolTextsN;

		configurationTags[fieldOffsetCfgSyncCoolListTexts]							= syncCoolListTexts;
		configurationNames[fieldOffsetCfgSyncCoolListTexts]						    = syncCoolListTextsN;

		configurationTags[fieldOffsetCfgSyncCoolLists]								= syncCoolLists;
		configurationNames[fieldOffsetCfgSyncCoolLists]							    = syncCoolListsN;

		configurationTags[fieldOffsetCfgSyncCategoryCountries]						= syncCategoryCountries;
		configurationNames[fieldOffsetCfgSyncCategoryCountries]					    = syncCategoryCountriesN;

		configurationTags[fieldOffsetCfgSyncCategoryProductionSteps]               = syncCategoryProductionSteps;
		configurationNames[fieldOffsetCfgSyncCategoryProductionSteps]              = syncCategoryProductionStepsN;

		configurationTags[fieldOffsetCfgSyncCategoryPreTexts]						= syncCategoryPreTexts;
		configurationNames[fieldOffsetCfgSyncCategoryPreTexts]						= syncCategoryPreTextsN;

		configurationTags[fieldOffsetCfgSyncProducts]								= syncProducts;
		configurationNames[fieldOffsetCfgSyncProducts]								= syncProductsN;

		configurationTags[fieldOffsetCfgSyncExpirationLabels]						= syncExpirationLabels;
		configurationNames[fieldOffsetCfgSyncExpirationLabels]						= syncExpirationLabelsN;

		configurationTags[fieldOffsetCfgSyncLabels]								= syncLabels;
		configurationNames[fieldOffsetCfgSyncLabels]								= syncLabelsN;

		configurationTags[fieldOffsetCfgSyncLabelTexts]							    = syncLabelTexts;
		configurationNames[fieldOffsetCfgSyncLabelTexts]							= syncLabelTextsN;

		configurationTags[fieldOffsetCfgSyncOperatorNotes]							= syncOperatorNotes;
		configurationNames[fieldOffsetCfgSyncOperatorNotes]						    = syncOperatorNotesN;

		configurationTags[fieldOffsetCfgSyncOperators]								= syncOperators;
		configurationNames[fieldOffsetCfgSyncOperators]							    = syncOperatorsN;

		configurationTags[fieldOffsetCfgSyncSoftkeyGroups]							= syncSoftkeyGroups;
		configurationNames[fieldOffsetCfgSyncSoftkeyGroups]						    = syncSoftkeyGroupsN;

		configurationTags[fieldOffsetCfgSyncSoftkeyStyles]							= syncSoftkeyStyles;
		configurationNames[fieldOffsetCfgSyncSoftkeyStyles]						    = syncSoftkeyStylesN;

		configurationTags[fieldOffsetCfgSyncSoftkeyFeatures]						= syncSoftkeyFeatures;
		configurationNames[fieldOffsetCfgSyncSoftkeyFeatures]						= syncSoftkeyFeaturesN;

		configurationTags[fieldOffsetCfgSyncSoftkeys]								= syncSoftkeys;
		configurationNames[fieldOffsetCfgSyncSoftkeys]								= syncSoftkeysN;

		configurationTags[fieldOffsetCfgSyncLabelGraphics]							= syncLabelGraphics;
		configurationNames[fieldOffsetCfgSyncLabelGraphics]						    = syncLabelGraphicsN;

		configurationTags[fieldOffsetCfgSyncCategoryGraphics]						= syncCategoryGraphics;
		configurationNames[fieldOffsetCfgSyncCategoryGraphics]						= syncCategoryGraphicsN;

		configurationTags[fieldOffsetCfgSyncProductGraphics]						= syncProductGraphics;
		configurationNames[fieldOffsetCfgSyncProductGraphics]						= syncProductGraphicsN;

		configurationTags[fieldOffsetCfgSyncSoftkeyGraphics]						= syncSoftkeyGraphics;
		configurationNames[fieldOffsetCfgSyncSoftkeyGraphics]						= syncSoftkeyGraphicsN;

		configurationTags[fieldOffsetCfgSyncSpecialsGraphics]						= syncSpecialsGraphics;
		configurationNames[fieldOffsetCfgSyncSpecialsGraphics]						= syncSpecialsGraphicsN;


//  CfgPrepack
		configurationTags[fieldOffsetCfgEnableSellBy]           = enableSellBy;
		configurationNames[fieldOffsetCfgEnableSellBy]          = enableSellByN;
		configurationTags[fieldOffsetCfgEnableBestBefore]       = enableBestBefore;
		configurationNames[fieldOffsetCfgEnableBestBefore]      = enableBestBeforeN;
		configurationTags[fieldOffsetCfgEnableProdImg]          = enableProdImg;
		configurationNames[fieldOffsetCfgEnableProdImg]         = enableProdImgN;
		configurationTags[fieldOffsetCfgEnableUpcNumber]        = enableUpcNumber;
		configurationNames[fieldOffsetCfgEnableUpcNumber]       = enableUpcNumberN;
		configurationTags[fieldOffsetCfgEnableFilmWidth]        = enableFilmWidth;
		configurationNames[fieldOffsetCfgEnableFilmWidth]       = enableFilmWidthN;
		configurationTags[fieldOffsetCfgEnableFilmTension]      = enableFilmTension;
		configurationNames[fieldOffsetCfgEnableFilmTension]     = enableFilmTensionN;
		configurationTags[fieldOffsetCfgEnableWrapTypes]        = enableWrapTypes;
		configurationNames[fieldOffsetCfgEnableWrapTypes]       = enableWrapTypesN;
		configurationTags[fieldOffsetCfgEnableCountryOfOrigin]  = enableCountryOfOrigin;
		configurationNames[fieldOffsetCfgEnableCountryOfOrigin] = enableCountryOfOriginN;
		configurationTags[fieldOffsetCfgEnableTrackingNumber]   = enableTrackingNumber;
		configurationNames[fieldOffsetCfgEnableTrackingNumber]  = enableTrackingNumberN;
		configurationTags[fieldOffsetCfgEnableProductionEntry]   = enableProductionEntry;
		configurationNames[fieldOffsetCfgEnableProductionEntry]  = enableProductionEntryN;
		configurationTags[fieldOffsetCfgProductionEntryContinuousPrint]   = productionEntryContinuousPrint;
		configurationNames[fieldOffsetCfgProductionEntryContinuousPrint]  = productionEntryContinuousPrintN;
		configurationTags[fieldOffsetCfgEnableProductionPlanning]   = enableProductionPlanning;
		configurationNames[fieldOffsetCfgEnableProductionPlanning]  = enableProductionPlanningN;
		configurationTags[fieldOffsetCfgProductionPlanningFrame]   = productionPlanningFrame;
		configurationNames[fieldOffsetCfgProductionPlanningFrame]  = productionPlanningFrameN;
		configurationTags[fieldOffsetCfgProductionPlanningUpdate]   = productionPlanningUpdate;
		configurationNames[fieldOffsetCfgProductionPlanningUpdate]  = productionPlanningUpdateN;
		configurationTags[fieldOffsetCfgEnableSelfServe]   = enableSelfServe;
		configurationNames[fieldOffsetCfgEnableSelfServe]  = enableSelfServeN;
		configurationTags[fieldOffsetCfgEnableRunTotals]        = enableRunTotals;
		configurationNames[fieldOffsetCfgEnableRunTotals]       = enableRunTotalsN;
		configurationTags[fieldOffsetCfgEnablePackedOn]         = enablePackedOn;
		configurationNames[fieldOffsetCfgEnablePackedOn]        = enablePackedOnN;
		configurationTags[fieldOffsetCfgPluSearchRange]         = pluSearchRange;
		configurationNames[fieldOffsetCfgPluSearchRange]        = pluSearchRangeN;

		configurationTags[fieldOffsetCfgRotationOverrideSetting1]  = rotOverrideSetting1;
		configurationNames[fieldOffsetCfgRotationOverrideSetting1] = rotOverrideSetting1N;
		configurationTags[fieldOffsetCfgRotationOverrideSetting2]  = rotOverrideSetting2;
		configurationNames[fieldOffsetCfgRotationOverrideSetting2] = rotOverrideSetting2N;
		configurationTags[fieldOffsetCfgRotationOverrideSetting3]  = rotOverrideSetting3;
		configurationNames[fieldOffsetCfgRotationOverrideSetting3] = rotOverrideSetting3N;
		configurationTags[fieldOffsetCfgRotationOverrideValue1]    = rotOverrideValue1;
		configurationNames[fieldOffsetCfgRotationOverrideValue1]   = rotOverrideValue1N;
		configurationTags[fieldOffsetCfgRotationOverrideValue2]    = rotOverrideValue2;
		configurationNames[fieldOffsetCfgRotationOverrideValue2]   = rotOverrideValue2N;
		configurationTags[fieldOffsetCfgRotationOverrideValue3]    = rotOverrideValue3;
		configurationNames[fieldOffsetCfgRotationOverrideValue3]   = rotOverrideValue3N;
		
		configurationTags[fieldOffsetCfgPrimaryLabelStockRotated]	   = primaryLabelStockRotated;
		configurationNames[fieldOffsetCfgPrimaryLabelStockRotated]     = primaryLabelStockRotatedN;
		configurationTags[fieldOffsetCfgSecondaryLabelStockRotated]	   = secondaryLabelStockRotated;
		configurationNames[fieldOffsetCfgSecondaryLabelStockRotated]   = secondaryLabelStockRotatedN;

		configurationTags[fieldOffsetCfgAppendingPluNumberToName]           = appendingPluNumberToName;
		configurationNames[fieldOffsetCfgAppendingPluNumberToName]          = appendingPluNumberToNameN;
		configurationTags[fieldOffsetCfgNumOfDescCharsToUse]                = numOfDescCharsToUse;
		configurationNames[fieldOffsetCfgNumOfDescCharsToUse]               = numOfDescCharsToUseN;


//  CfgMarquee
		configurationTags[fieldOffsetCfgMqPermanentMarqueeNumber]     = permanentMarqueeNumber;
		configurationNames[fieldOffsetCfgMqPermanentMarqueeNumber]    = permanentMarqueeNumberN;
		configurationTags[fieldOffsetCfgMqPermanentMarqueeDisplayed]  = permanentMarqueeDisplayed;
		configurationNames[fieldOffsetCfgMqPermanentMarqueeDisplayed] = permanentMarqueeDisplayedN;
		configurationTags[fieldOffsetCfgMqProductMarqueeDisplayed]    = productMarqueeDisplayed;
		configurationNames[fieldOffsetCfgMqProductMarqueeDisplayed]   = productMarqueeDisplayedN;
		configurationTags[fieldOffsetCfgMqProductDescMarqueeCounter]  = productDescMarqueeCounter;
		configurationNames[fieldOffsetCfgMqProductDescMarqueeCounter] = productDescMarqueeCounterN;

//  CfgFTP
		configurationTags[fieldOffsetCfgFTPServerIporHostName]                  = FTPServerIPorHostName;
		configurationNames[fieldOffsetCfgFTPServerIporHostName]                 = FTPServerIPorHostNameN;
		configurationTags[fieldOffsetCfgFTPUserName]                            = FTPUserName;
		configurationNames[fieldOffsetCfgFTPUserName]                           = FTPUserNameN;
		configurationTags[fieldOffsetCfgFTPPassword]                            = FTPPassword;
		configurationNames[fieldOffsetCfgFTPPassword]                           = FTPPasswordN;
		configurationTags[fieldOffsetCfgFTPSubDir]                              = FTPSubDirectory;
		configurationNames[fieldOffsetCfgFTPSubDir]                             = FTPSubDirectoryN;
		configurationTags[fieldOffsetCfgFTPstartTime]  			                = FTPScheduleStartTime;
		configurationNames[fieldOffsetCfgFTPstartTime] 			 	            = FTPScheduleStartTimeN;
		configurationTags[fieldOffsetCfgFTPuseAutomatic] 		 	            = FTPUseAutomatic;
		configurationNames[fieldOffsetCfgFTPuseAutomatic] 		 	            = FTPUseAutomaticN;
		configurationTags[fieldOffsetCfgFTPmonday] 			 	                = FTPmonday;
		configurationNames[fieldOffsetCfgFTPmonday] 			 	            = FTPmondayN;
		configurationTags[fieldOffsetCfgFTPtuesday] 			 	            = FTPtuesday;
		configurationNames[fieldOffsetCfgFTPtuesday] 			 	            = FTPtuesdayN;
		configurationTags[fieldOffsetCfgFTPwednesday]			 	            = FTPwednesday;
		configurationNames[fieldOffsetCfgFTPwednesday] 			 	            = FTPwednesdayN;
		configurationTags[fieldOffsetCfgFTPthursday] 			 	            = FTPthursday;
		configurationNames[fieldOffsetCfgFTPthursday] 			 	            = FTPthursdayN;
		configurationTags[fieldOffsetCfgFTPfriday] 			 	                = FTPfriday;
		configurationNames[fieldOffsetCfgFTPfriday] 			 	            = FTPfridayN;
		configurationTags[fieldOffsetCfgFTPsaturday] 			 	            = FTPsaturday;
		configurationNames[fieldOffsetCfgFTPsaturday] 			 	            = FTPsaturdayN;
		configurationTags[fieldOffsetCfgFTPsunday] 			 	                = FTPsunday;
		configurationNames[fieldOffsetCfgFTPsunday] 			 	            = FTPsundayN;
		configurationTags[fieldOffsetCfgFTPdownloadFirmwareOnly] 	 	        = FTPdownloadFirmwareOnly;
		configurationNames[fieldOffsetCfgFTPdownloadFirmwareOnly] 	 	        = FTPdownloadFirmwareOnlyN;
		configurationTags[fieldOffsetCfgFTPdownloadInstallFirmware]	 	        = FTPdownloadInstallFirmware;
		configurationNames[fieldOffsetCfgFTPdownloadInstallFirmware] 	 	    = FTPdownloadInstallFirmwareN;
		configurationTags[fieldOffsetCfgFTPdailyUpdates]                        = FTPdailyUpdates;
        configurationNames[fieldOffsetCfgFTPdailyUpdates]                       = FTPdailyUpdates;
		configurationTags[fieldOffsetCfgFTPhourlyUpdates]		 	            = FTPhourlyUpdates;
		configurationNames[fieldOffsetCfgFTPhourlyUpdates] 		 	            = FTPhourlyUpdatesN;
		configurationTags[fieldOffsetCfgFTP15MinuteUpdates] 		 	        = FTP15MinuteUpdates;
		configurationNames[fieldOffsetCfgFTP15MinuteUpdates]		 	        = FTP15MinuteUpdatesN;
		configurationTags[fieldOffsetCfgFTPUpdatePLUData] 		 	            = FTPUpdatePLUData;
		configurationNames[fieldOffsetCfgFTPUpdatePLUData] 		 	            = FTPUpdatePLUDataN;
		configurationTags[fieldOffsetCfgFTPUpdateLabelData]		 	            = FTPUpdateLabelData;
		configurationNames[fieldOffsetCfgFTPUpdateLabelData]		 	        = FTPUpdateLabelDataN;
		configurationTags[fieldOffsetCfgFTPUpdateFlashkeyData] 		 	        = FTPUpdateFlashkeyData;
		configurationNames[fieldOffsetCfgFTPUpdateFlashkeyData]		 	        = FTPUpdateFlashkeyDataN;
		configurationTags[fieldOffsetCfgFTPUpdateOperatorData] 		 	        = FTPUpdateOperatorData;
		configurationNames[fieldOffsetCfgFTPUpdateOperatorData] 	 	        = FTPUpdateOperatorDataN;
		configurationTags[fieldOffsetCfgFTPUpdatePlaylists] 		 	        = FTPUpdatePlaylists;
		configurationNames[fieldOffsetCfgFTPUpdatePlaylists] 		 	        = FTPUpdatePlaylistsN;
		configurationTags[fieldOffsetCfgFTPUpdateAlerts] 		 	            = FTPUpdateAlerts;
		configurationNames[fieldOffsetCfgFTPUpdateAlerts] 		 	            = FTPUpdateAlertsN;
		configurationTags[fieldOffsetCfgFTPUpdateScaleConfiguration] 	 	    = FTPUpdateScaleConfiguration;
		configurationNames[fieldOffsetCfgFTPUpdateScaleConfiguration] 	 	    = FTPUpdateScaleConfigurationN;
		configurationTags[fieldOffsetCfgFTPUpdateSynchronizationConfiguration]  = FTPUpdateSynchronizationConfiguration;
		configurationNames[fieldOffsetCfgFTPUpdateSynchronizationConfiguration] = FTPUpdateSynchronizationConfigurationN;
		configurationTags[fieldOffsetCfgFTPUpdateStoreNameNumber] 		        = FTPUpdateStoreNameNumber;
		configurationNames[fieldOffsetCfgFTPUpdateStoreNameNumber] 		        = FTPUpdateStoreNameNumberN;
		configurationTags[fieldOffsetCfgFTPUpdateVideos] 			            = FTPUpdateVideos;
		configurationNames[fieldOffsetCfgFTPUpdateVideos] 			            = FTPUpdateVideosN;
		configurationTags[fieldOffsetCfgFTPUpdateImages] 			            = FTPUpdateImages;
		configurationNames[fieldOffsetCfgFTPUpdateImages] 			            = FTPUpdateImagesN;
		configurationTags[fieldOffsetCfgFTPUpdateAudio] 			            = FTPUpdateAudio;
		configurationNames[fieldOffsetCfgFTPUpdateAudio] 			            = FTPUpdateAudioN;
		configurationTags[fieldOffsetCfgFTPUpdateFirmware] 			            = FTPUpdateFirmware;
		configurationNames[fieldOffsetCfgFTPUpdateFirmware] 			        = FTPUpdateFirmwareN;
		configurationTags[fieldOffsetCfgFTPSendPLUData] 			            = FTPSendPLUData;
		configurationNames[fieldOffsetCfgFTPSendPLUData] 			            = FTPSendPLUDataN;
		configurationTags[fieldOffsetCfgFTPSendLabelData] 			            = FTPSendLabelData;
		configurationNames[fieldOffsetCfgFTPSendLabelData] 			            = FTPSendLabelDataN;
		configurationTags[fieldOffsetCfgFTPSendFlashkeyData] 			        = FTPSendFlashkeyData;
		configurationNames[fieldOffsetCfgFTPSendFlashkeyData] 			        = FTPSendFlashkeyDataN;
		configurationTags[fieldOffsetCfgFTPSendOperatorData] 			        = FTPSendOperatorData;
		configurationNames[fieldOffsetCfgFTPSendOperatorData] 			        = FTPSendOperatorDataN;
		configurationTags[fieldOffsetCfgFTPSendPlaylists] 			            = FTPSendPlaylists;
		configurationNames[fieldOffsetCfgFTPSendPlaylists] 			            = FTPSendPlaylistsN;
		configurationTags[fieldOffsetCfgFTPSendAlerts] 				            = FTPSendAlerts;;
		configurationNames[fieldOffsetCfgFTPSendAlerts] 			            = FTPSendAlertsN;
		configurationTags[fieldOffsetCfgFTPSendScaleConfiguration] 		        = FTPSendScaleConfiguration;;
		configurationNames[fieldOffsetCfgFTPSendScaleConfiguration] 		    = FTPSendScaleConfigurationN;
		configurationTags[fieldOffsetCfgFTPSendSynchronizationConfiguration] 	= FTPSendSynchronizationConfiguration;
		configurationNames[fieldOffsetCfgFTPSendSynchronizationConfiguration] 	= FTPSendSynchronizationConfigurationN;
		configurationTags[fieldOffsetCfgFTPSendStoreNameNumber] 		        = FTPSendStoreNameNumber;
		configurationNames[fieldOffsetCfgFTPSendStoreNameNumber] 		        = FTPSendStoreNameNumberN;
		configurationTags[fieldOffsetCfgFTPSendVideos] 				            = FTPSendVideos;
		configurationNames[fieldOffsetCfgFTPSendVideos] 			            = FTPSendVideosN;
		configurationTags[fieldOffsetCfgFTPSendImages] 				            = FTPSendImages;
		configurationNames[fieldOffsetCfgFTPSendImages] 			            = FTPSendImagesN;
		configurationTags[fieldOffsetCfgFTPSendAudio] 				            = FTPSendAudio;
		configurationNames[fieldOffsetCfgFTPSendAudio] 				            = FTPSendAudioN;
		configurationTags[fieldOffsetCfgFTPSendFirmware] 			            = FTPSendFirmware;
		configurationNames[fieldOffsetCfgFTPSendFirmware] 			            = FTPSendFirmwareN;
		configurationTags[fieldOffsetCfgFTPAllowOperatorSendToDept] 		    = FTPAllowOperatorSendToDept;
		configurationNames[fieldOffsetCfgFTPAllowOperatorSendToDept] 		    = FTPAllowOperatorSendToDeptN;
		configurationTags[fieldOffsetCfgFTPAllowOperatorUpdateNow] 		        = FTPAllowOperatorUpdateNow;
		configurationNames[fieldOffsetCfgFTPAllowOperatorUpdateNow] 		    = FTPAllowOperatorUpdateNowN;
		
//  CfgCommInterface
		configurationTags[fieldOffsetCfgCommProductClassTotalsClearTransactions]           = CommProductClassTotalsClearTransactions;
		configurationNames[fieldOffsetCfgCommProductClassTotalsClearTransactions]          = CommProductClassTotalsClearTransactionsN;
		configurationTags[fieldOffsetCfgCommHourlyTotalsClearTransactions]                 = CommHourlyTotalsClearTransactions;
		configurationNames[fieldOffsetCfgCommHourlyTotalsClearTransactions]                = CommHourlyTotalsClearTransactionsN;
		configurationTags[fieldOffsetCfgCommOperatorTotalsClearTransactions]               = CommOperatorTotalsClearTransactions;
		configurationNames[fieldOffsetCfgCommOperatorTotalsClearTransactions]              = CommOperatorTotalsClearTransactionsN;
		configurationTags[fieldOffsetCfgCommCOOLTotalsClearTransactions]                   = CommCOOLTotalsClearTransactions;
		configurationNames[fieldOffsetCfgCommCOOLTotalsClearTransactions]                  = CommCOOLTotalsClearTransactionsN;
		configurationTags[fieldOffsetCfgCommOperatorChangesClearTransactions]              = CommOperatorChangesClearTransactions;
		configurationNames[fieldOffsetCfgCommOperatorChangesClearTransactions]             = CommOperatorChangesClearTransactionsN;

//  CfgCurrency
		configurationTags[fieldOffsetCfgCurrencyDecimalSeparator]                          = CurrencyDecimalSeparator;
		configurationTags[fieldOffsetCfgCurrencyMonetaryRule]                              = CurrencyMonetaryRule;
		configurationTags[fieldOffsetCfgCurrencyTotalPriceTotalDigits]                     = CurrencyTotalPriceTotalDigits;
		configurationTags[fieldOffsetCfgCurrencyUnitPriceTotalDigits]                      = CurrencyUnitPriceTotalDigits;
		configurationTags[fieldOffsetCfgCurrencyRoundingFactor]                            = CurrencyRoundingFactor;
		configurationTags[fieldOffsetCfgCurrencyRoundingMethod]                            = CurrencyRoundingMethod;
		configurationTags[fieldOffsetCfgCurrencyPrimarySymbol]                             = CurrencyPrimarySymbol;
		configurationTags[fieldOffsetCfgCurrencyPrimaryDecimalDigits]                      = CurrencyPrimaryDecimalDigits;
		configurationTags[fieldOffsetCfgCurrencySecondarySymbol]                           = CurrencySecondarySymbol;
		configurationTags[fieldOffsetCfgCurrencySecondaryDecimalDigits]                    = CurrencySecondaryDecimalDigits;
//		configurationTags[fieldOffsetCfgPricePrecision]                     = pricePrecision;
//		configurationNames[fieldOffsetCfgPricePrecision]                    = pricePrecisionN;

//  CfgAccumulateItems
		configurationTags[fieldOffsetCfgEnableAccumItems]                        = enableAccumulateItems;
		configurationNames[fieldOffsetCfgEnableAccumItems]                       = enableAccumulateItemsN;
		configurationTags[fieldOffsetCfgEnableItemStripMode]                     = enableItemStripMode;
		configurationNames[fieldOffsetCfgEnableItemStripMode]                    = enableItemStripModeN;
		configurationTags[fieldOffsetCfgAccumItemsReturnToStartScreen]           = accumItemsReturnToStartScreen;
		configurationNames[fieldOffsetCfgAccumItemsReturnToStartScreen]          = accumItemsReturnToStartScreenN;
		configurationTags[fieldOffsetCfgEnableAccumItemsTotalsOnly]              = enableAccumItemsTotalsOnly;
		configurationNames[fieldOffsetCfgEnableAccumItemsTotalsOnly]             = enableAccumItemsTotalsOnlyN;
		configurationTags[fieldOffsetCfgAccumItemsEANFormatString]               = accumItemsEANFormatString;
		configurationNames[fieldOffsetCfgAccumItemsEANFormatString]              = accumItemsEANFormatStringN;
		configurationTags[fieldOffsetCfgAccumItemsEANPrefix]                     = accumItemsEANPrefix;
		configurationNames[fieldOffsetCfgAccumItemsEANPrefix]                    = accumItemsEANPrefixN;
		configurationTags[fieldOffsetCfgAccumItemsEANCode]                       = accumItemsEANCode;
		configurationNames[fieldOffsetCfgAccumItemsEANCode]                      = accumItemsEANCodeN;
		configurationTags[fieldOffsetCfgAccumItemsScaleId]                       = accumItemsScaleId;
		configurationNames[fieldOffsetCfgAccumItemsScaleId]                      = accumItemsScaleIdN;
		configurationTags[fieldOffsetCfgAccumItemsTransactionNumber]             = hfc::TransactionTags::_transactionId_;
		configurationNames[fieldOffsetCfgAccumItemsTransactionNumber]            = accumItemsTransactionNumberN;
		configurationTags[fieldOffsetCfgAccumItemsUPCProductNumber]              = accumItemsUPCProductNumber;
		configurationNames[fieldOffsetCfgAccumItemsUPCProductNumber]             = accumItemsUPCProductNumberN;
		configurationTags[fieldOffsetCfgReadTransactionsCompatibility]           = readTransactionsCompatibility;
		configurationNames[fieldOffsetCfgReadTransactionsCompatibility]          = readTransactionsCompatibilityN;
		configurationTags[fieldOffsetCfgAccumBarcodeUseFirstItem]                = accumBarcodeUseFirstItem;
		configurationNames[fieldOffsetCfgAccumBarcodeUseFirstItem]               = accumBarcodeUseFirstItemN;

//  Not saved to Config - Input only from Quantum. Will impact SoftkeyFeature default group #
		configurationTags[fieldOffsetCfgHomeFlashkeyGroup]      = homeFlashkeyGroup;

//  Next 6 not saved to Config. Input only via UltimaTaggedFormat and will get saved to ProductDefaults.
		configurationTags[fieldOffsetCfgRwEanFormat]      = rwEanFormat;
		configurationTags[fieldOffsetCfgFwEanFormat]      = fwEanFormat;
		configurationTags[fieldOffsetCfgBcEanFormat]      = bcEanFormat;
		configurationTags[fieldOffsetCfgRwEanPrefix]      = rwEanPrefix;
		configurationTags[fieldOffsetCfgFwEanPrefix]      = fwEanPrefix;
		configurationTags[fieldOffsetCfgBcEanPrefix]      = bcEanPrefix;

		configurationTags[fieldOffsetCfgVirtualPrintingOverride2ndPrinter]  = virtualPrintingOverride2ndPrinter;
		configurationTags[fieldOffsetCfgVirtualPrintingOverride3rdPrinter]  = virtualPrintingOverride3rdPrinter;

		configurationTags[fieldOffsetCfgDisplaySelfServeStartScreen]        = displaySelfServeStartScreen;
		configurationTags[fieldOffsetCfgBestSellersOnSelfServeMainScreen]   = bestSellersOnSelfServeMainScreen;

		configurationTags[fieldOffsetCfgEnableLabelLength_1_000]            = enableLabelLength1_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_1_500]            = enableLabelLength1_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_1_750]            = enableLabelLength1_750;
		configurationTags[fieldOffsetCfgEnableLabelLength_2_000]            = enableLabelLength2_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_2_375]            = enableLabelLength2_375;
		configurationTags[fieldOffsetCfgEnableLabelLength_2_500]            = enableLabelLength2_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_3_000]            = enableLabelLength3_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_3_500]            = enableLabelLength3_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_4_000]            = enableLabelLength4_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_4_500]            = enableLabelLength4_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_5_000]            = enableLabelLength5_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_5_500]            = enableLabelLength5_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_6_000]            = enableLabelLength6_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_6_500]            = enableLabelLength6_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_7_000]            = enableLabelLength7_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_7_500]            = enableLabelLength7_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_8_000]            = enableLabelLength8_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_8_500]            = enableLabelLength8_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_9_000]            = enableLabelLength9_000;
		configurationTags[fieldOffsetCfgEnableLabelLength_9_500]            = enableLabelLength9_500;
		configurationTags[fieldOffsetCfgEnableLabelLength_10_00]            = enableLabelLength10_00;
		configurationTags[fieldOffsetCfgEnableLabelLength_Continuous]       = enableLabelLengthContinuous;

		configurationTags[fieldOffsetCfgContinuousStockIgnoreLabelTaken]    = continuousStockIgnoreLabelTaken;
		configurationTags[fieldOffsetCfgChoseToUseContinuousStock]          = choseToUseContinuousStock;

		configurationTags[fieldOffsetCfgWeightRequiredForMeal]              = weightRequiredForMeal;

		configurationTags[fieldOffsetCfgSuggestiveSellStyle]                = suggestiveSellStyle;
		configurationTags[fieldOffsetCfgSuggestiveSellFrequency]            = suggestiveSellFrequency;

		configurationTags[fieldOffsetCfgAlwaysEnableVirtualKeyboard]        = alwaysEnableVirtualKeyboard;

		configurationTags[fieldOffsetCfgShowSelectAllWhenVoidingTrans]      = showSelectAllWhenVoidingTrans;

		configurationTags[fieldOffsetCfgPeriodicOfflineBeep]                = periodicOfflineBeep;

		configurationTags[fieldOffsetCfgEnableGlobalPercentOff]             = enableGlobalPercentOff;
		configurationTags[fieldOffsetCfgGlobalPercentageOff]                = globalPercentageOff;

		configurationTags[fieldOffsetCfgEnableAccumItemsAndFlashkeys]       = enableAccumItemsAndFlashkeys;

		configurationTags[fieldOffsetCfgRequireFixedWeightVerification]     = requireFixedWeightVerification;

		configurationTags[fieldOffsetCfgAutoBackupDBFrequency]              = autoBackupDBFrequency;

		configurationTags[fieldOffsetCfgFPCVersion]                         = FPCVersion;
		configurationTags[fieldOffsetCfgFPCWeigherVersion]                  = FPCWeigherVersion;

		configurationTags[fieldOffsetCfgScaleModel]                         = scaleModel;

		configurationTags[fieldOffsetCfgCutTestOptionEnabled]               = cutTestOptionEnabled;
		configurationTags[fieldOffsetCfgMaxSuspendedCutTestsOverall]        = maxSuspendedCutTestsOverall;
		configurationTags[fieldOffsetCfgMaxSuspendedCutTestsOperator]       = maxSuspendedCutTestsOperator;
		configurationTags[fieldOffsetCfgSecondsToAutoSuspendCutTest]        = secondsToAutoSuspendCutTest;
		configurationTags[fieldOffsetCfgNextCutTestNumber]                  = nextCutTestNumber;

		configurationTags[fieldOffsetCfgBottomScreenContent]                = bottomScreenContent;
		
		configurationTags[fieldOffsetCfgEnableSearchMenuButton]             = enableSearchMenuButton;

        configurationTags[fieldOffsetCfgEnableHelpPrompts]					= enableHelpPrompts;
		configurationTags[fieldOffsetCfgEnableUseCharacterSearching]		= enableUseCharacterSearching;


		configurationTags[fieldOffsetCfgUseSecondTare]                      = useSecondTare;

		configurationTags[fieldOffsetCfgVariableShelfLifeTime]              = variableShelfLifeTime;
		
        configurationTags[fieldOffsetCfgSelfServeWeightRequiredToSelectItem] = selfServeWeightRequiredToSelectItem;
        configurationTags[fieldOffsetCfgSelfServeByCountItemsAllowed]        = selfServeByCountItemsAllowed;


		configurationTags[fieldOffsetCfgPrinter1LabelLength]                = printer1LabelLength;
		configurationTags[fieldOffsetCfgPrinter2LabelLength]                = printer2LabelLength;
		configurationTags[fieldOffsetCfgPrinter3LabelLength]                = printer3LabelLength;

		configurationTags[fieldOffsetCfgDefaultSoftkeyFontPointSize]         = defaultSoftkeyFontPointSize;

		configurationTags[fieldOffsetCfgDiagServiceContactInfo]					= diagServiceContactInfo;
		configurationTags[fieldOffsetCfgDiagGatewayDeviceAddress] 				= diagGatewayDeviceAddress;
		configurationTags[fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel]		= diagGatewayDeviceStatusMsgLogLevel;

		configurationTags[fieldOffsetCfgSelfServePrintToScreen]					= selfServePrintToScreen;
		configurationTags[fieldOffsetCfgVoidTransactionsViewHours]              = voidTransactionsViewHours;
		configurationTags[fieldOffsetCfgSelfServeDisplayTareOnRW]               = selfServeDisplayTareOnRW;

		configurationTags[fieldOffsetCfgVoidTransactionsViewHours]				= voidTransactionsViewHours;
		configurationTags[fieldOffsetDoNotPrintNutrifacts]                      = doNotPrintNutrifacts;

		configurationTags[fieldOffsetCfgSelfServeEnablePlaylists]               = selfServeEnablePlaylists;
		configurationTags[fieldOffsetCfgSelfServePlaylistFile]                  = selfServePlaylistFile;

		configurationTags[fieldOffsetCfgMostRecentActiveDeptNum]				= mostRecentActiveDeptNum;
		configurationTags[fieldOffsetCfgUseCurrentDeptnumWhenNotReceived]		= useCurrentDeptnumWhenNotReceived;

		configurationTags[fieldOffsetCfgDigimarcEnabled]						= digimarcEnabled;
		configurationTags[fieldOffsetCfgDigimarcDotDensity]						= digimarcDotDensity;
		configurationTags[fieldOffsetCfgDigimarcDotSize]						= digimarcDotSize;
		configurationTags[fieldOffsetCfgDigimarcWatermarkRes]					= digimarcWatermarkRes;
		configurationTags[fieldOffsetCfgDigimarcExpandedFreshEnabled]			= digimarcExpandedFreshEnabled;

		configurationTags[fieldOffsetCfgReduceGS1Length]						= reduceGS1Length;
		configurationTags[fieldOffsetCfgDisableHooks]							= disableHooks;

		configurationTags[fieldOffsetCfgCameraSystemTrustLevel]					= cameraSystemTrustLevel;

		configurationTags[fieldOffsetCfgUnitPricePrintMethod]					= unitPricePrintMethod;
		configurationTags[fieldOffsetCfgPriceChangedLabelIndicator]             = priceChangedLabelIndicator;

		configurationTags[fieldOffsetCfgAutoUpdateUPCBarcodeOver99]             = autoUpdateUPCBarcodeOver99;
		configurationNames[fieldOffsetCfgAutoUpdateUPCBarcodeOver99]            = autoUpdateUPCBarcodeOver99N;
		configurationTags[fieldOffsetCfgBarcodeTypeOver99]                      = barcodeTypeOver99;
		configurationNames[fieldOffsetCfgBarcodeTypeOver99]                     = barcodeTypeOver99N;

		// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
		//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
		//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
		configurationTags[fieldOffsetCfgBarcodeWidthOver99]                     = barcodeWidthOver99;
		configurationTags[fieldOffsetBarcodeOffsetFromLeftOver99]               = barcodeOffsetFromLeftOver99;

		configurationTags[fieldOffsetCfgValueMaxEnabled]						= valueMaxEnabled;
		configurationNames[fieldOffsetCfgValueMaxEnabled]                       = valueMaxEnabledN;
		configurationTags[fieldOffsetCfgValueMaxDiagCollectionDisabled]			= valueMaxDiagCollectionDisabled;
		configurationNames[fieldOffsetCfgValueMaxDiagCollectionDisabled]        = valueMaxDiagCollectionDisabledN;

		configurationTags[fieldOffsetCfgSNMPTrapsIp]							= snmpTrapsIp;
		configurationTags[fieldOffsetCfgSNMPTrapsPort]							= snmpTrapsPort;
		configurationTags[fieldOffsetCfgSNMPVariablesPort]						= snmpVariablesPort;
		configurationTags[fieldOffsetCfgSNMPEnabled]							= snmpEnabled;

		configurationTags[fieldOffsetCfgIsCutterEnabled]                        = cutterEnabled;
		configurationTags[fieldOffsetCfgIsCodeCheckerEnabled]                   = codeCheckerEnabled;

		configurationTags[fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel]      = handsFreeDeviceMinLoginPowerLevel;

		configurationTags[fieldOffsetCfgAllowSupervisorDotWear]					= allowSupervisorDotWear;
		configurationTags[fieldOffsetCfgAllowOperatorDotWear]					= allowOperatorDotWear;

		configurationTags[fieldOffsetCfgEnableSearchOnKeypad]					= enableSearchOnKeypad;
		configurationTags[fieldOffsetCfgEnableEnhancedCenterScreen]				= enableEnhancedCenterScreen;
		configurationTags[fieldOffsetCfgEnhancedCenterHomePage]					= enhancedCenterHomePage;
		configurationTags[fieldOffsetCfgVirtualCombinePrint]					= enableCombinedVirtualPrint;
		
		configurationTags[fieldOffsetCfgSelfServeIgnorePrinterErrors]           = selfServeIgnorePrinterErrors;
		configurationTags[fieldOffsetCfgSelfServeSearchKeypadFlashkeys]         = selfServeSearchKeypadFlashkeys;
		configurationTags[fieldOffsetCfgSelfServeDisplayPluLabel]               = selfServeDisplayPluLabel;
		        
        configurationTags[fieldOffsetCfgWrapperCameraEnabled]       = wrapperCameraEnabled;

        // CfgCommScheduled
		configurationTags[fieldOffsetCfgCommScheduledUseAutomaticMerges] = CommScheduledUseAutomaticMerges;
		configurationNames[fieldOffsetCfgCommScheduledUseAutomaticMerges] = CommScheduledUseAutomaticMergesN;
		configurationTags[fieldOffsetCfgCommScheduledMonday] = CommScheduledMonday;
		configurationNames[fieldOffsetCfgCommScheduledMonday] = CommScheduledMondayN;
		configurationTags[fieldOffsetCfgCommScheduledTuesday] = CommScheduledTuesday;
		configurationNames[fieldOffsetCfgCommScheduledTuesday] = CommScheduledTuesdayN;
		configurationTags[fieldOffsetCfgCommScheduledWednesday] = CommScheduledWednesday;
		configurationNames[fieldOffsetCfgCommScheduledWednesday] = CommScheduledWednesdayN;
		configurationTags[fieldOffsetCfgCommScheduledThursday] = CommScheduledThursday;
		configurationNames[fieldOffsetCfgCommScheduledThursday] = CommScheduledThursdayN;
		configurationTags[fieldOffsetCfgCommScheduledFriday] = CommScheduledFriday;
		configurationNames[fieldOffsetCfgCommScheduledFriday] = CommScheduledFridayN;
		configurationTags[fieldOffsetCfgCommScheduledSaturday] = CommScheduledSaturday;
		configurationNames[fieldOffsetCfgCommScheduledSaturday] = CommScheduledSaturdayN;
		configurationTags[fieldOffsetCfgCommScheduledSunday] = CommScheduledSunday;
		configurationNames[fieldOffsetCfgCommScheduledSunday] = CommScheduledSundayN;
		configurationTags[fieldOffsetCfgCommScheduledDailyUpdates] = CommScheduledDailyUpdates;
		configurationNames[fieldOffsetCfgCommScheduledDailyUpdates] = CommScheduledDailyUpdatesN;
		configurationTags[fieldOffsetCfgCommScheduledStartTime] = CommScheduledStartTime;
		configurationNames[fieldOffsetCfgCommScheduledStartTime] = CommScheduledStartTimeN;
        configurationTags[fieldOffsetCfgCommScheduledHourlyUpdates] = CommScheduledHourlyUpdates;
		configurationNames[fieldOffsetCfgCommScheduledHourlyUpdates] = CommScheduledHourlyUpdatesN;
		configurationTags[fieldOffsetCfgCommScheduledExecMergeAfterComm] = CommScheduledExecMergeAfterComm;
		configurationNames[fieldOffsetCfgCommScheduledExecMergeAfterComm] = CommScheduledExecMergeAfterCommN;
		configurationTags[fieldOffsetCfgCommScheduledMergeHistory] = CommScheduledMergeHistory;
		configurationNames[fieldOffsetCfgCommScheduledMergeHistory] = CommScheduledMergeHistoryN;

        configurationTags[fieldOffsetCfgIsConserveLinerLess]					= enableConserveLinerLess;
        configurationTags[fieldOffsetCfgEnableUPTagPrefix]						= enableUPTagPrefix;

		configurationTags[fieldOffsetCfgMinSweetheartWeight]					= minSweetheartWeight;

		configurationTags[fieldOffsetCfgOnlineOrderingEnabled]					= onlineOrderingEnabled;
		configurationTags[fieldOffsetCfgOnlineOrderingServer]					= onlineOrderingServer;
		configurationTags[fieldOffsetCfgOnlineOrderingZone]					    = onlineOrderingZone;


		configurationTags[fieldOffsetCfgSelfServeIgnorePrinterErrors]           = selfServeIgnorePrinterErrors;			// ORIG: ComboBox, BigY displayed: 0= Print Label, 1= Scan Label (ignore errors), 2= Scan and Print Label
		configurationTags[fieldOffsetCfgSelfServeSearchKeypadFlashkeys]         = selfServeSearchKeypadFlashkeys;		// ORIG: Checkbox, BigY allowed Combine Search, Keypad, and Flashkeys
		configurationTags[fieldOffsetCfgSelfServeDisplayPluLabel]               = selfServeDisplayPluLabel;				// ORIG: Checkbox, BigY: print the label assigned to the PLU

		configurationTags[fieldOffsetCfgSelfServeDisplayBarcodeLabel]           = selfServeDisplayBarcodeLabel;			// ORIG: Checkbox, Delhaize: Display Barcode(ONLY) and Print button on screen at same time
		configurationTags[fieldOffsetCfgSelfServeStartState]                    = selfServeStartState;
		configurationTags[fieldOffsetCfgSelfServeItemEntryState]				= selfServeItemEntryState;
		configurationTags[fieldOffsetCfgSelfServeSearchState]					= selfServeSearchState;
		configurationTags[fieldOffsetCfgSelfServePrintState]					= selfServePrintState;
		configurationTags[fieldOffsetCfgSelfServeAfterPrintingState]            = selfServeAfterPrintingState;
		configurationTags[fieldOffsetCfgSelfServeStartScreenClickable]			= selfServeStartScreenClickable;
		configurationTags[fieldOffsetCfgSelfServeIncludeKeypad]					= selfServeIncludeKeypad;
		configurationTags[fieldOffsetCfgSelfServeHideMenuBar]                   = selfServeHideMenuBar;
		configurationTags[fieldOffsetCfgSelfServePrintOnStableWeight]           = selfServePrintOnStableWeight;
		configurationTags[fieldOffsetCfgEnableVariableShelfLifeTime]            = enableVariableShelfLifeTime;			// PJR 420 - Flashkey Options to Toggle Extra Day

		configurationTags[fieldOffsetCfgSelfServeFixedPlu]              = selfServeFixedPlu;                            //PJR413 (SOF-4772)
		configurationTags[fieldOffsetCfgSelfServeEnableFreshServe]      = selfServeEnableFreshServe;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyType0]         = selfServeFlashkeyType0;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyType1]			= selfServeFlashkeyType1;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyType2]			= selfServeFlashkeyType2;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyType3]			= selfServeFlashkeyType3;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyType4]			= selfServeFlashkeyType4;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyText0]         = selfServeFlashkeyText0;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyText1]			= selfServeFlashkeyText1;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyText2]			= selfServeFlashkeyText2;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyText3]			= selfServeFlashkeyText3;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyText4]			= selfServeFlashkeyText4;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyCatNum0]       = selfServeFlashkeyCatNum0;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyCatNum1]		= selfServeFlashkeyCatNum1;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyCatNum2]		= selfServeFlashkeyCatNum2;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyCatNum3]		= selfServeFlashkeyCatNum3;
		configurationTags[fieldOffsetCfgSelfServeFlashkeyCatNum4]		= selfServeFlashkeyCatNum4;
		configurationTags[fieldOffsetCfgSelfServeFreshServeLabelOffset] = selfServeFreshServeLabelOffset;
		configurationTags[fieldOffsetCfgSelfServeFlashkeySecText0]      = selfServeFlashkeySecText0;
		configurationTags[fieldOffsetCfgSelfServeFlashkeySecText1]      = selfServeFlashkeySecText1;
		configurationTags[fieldOffsetCfgSelfServeFlashkeySecText2]      = selfServeFlashkeySecText2;
		configurationTags[fieldOffsetCfgSelfServeFlashkeySecText3]      = selfServeFlashkeySecText3;
		configurationTags[fieldOffsetCfgSelfServeFlashkeySecText4]      = selfServeFlashkeySecText4;

		configurationTags[fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag] = ignoreVendorAndProductNumTagsWhenUpcNumTag;

		configurationTags[fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight] = allowKeyboardNetWeightItemAsRandomWeight;
		configurationTags[fieldOffsetCfgFieldExclusions]               = fieldExclusions;
		
		configurationTags[fieldOffsetCfgEnableRfidPrinting]                             = enableRfidPrinting;
   		configurationTags[fieldOffsetCfgMaxPERfidVoidLabels]                            = maxPERfidVoidLabels;
		configurationTags[fieldOffsetCfgRfidEncodeMethod]                               = rfidEncodeMethod;
		configurationTags[fieldOffsetCfgIgnoreRFIDWhenHardwarePresentButRFIDIsDisabled] = rfidIgnore;


		_tagsInitialized_ = true;
	}

	if ( pFieldList )
	{
		putTagsInMap(pFieldList);
	}
}


ConfigurationTags::~ConfigurationTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

/*
const std::string& ConfigurationTags::getName(unsigned int index) const
{
	if ( configurationNames.empty() || index <= 0 || index > configurationNames.size() - 1 )
	{
		if ( configurationNames.empty() )
			Debug::trace("ConfigurationTags", 10, "Cannot retrieve tag name for index " +
					inttostring(index) + "(configurationNames is empty).");
		else
			Debug::trace("ConfigurationTags", 10, "Cannot retrieve tag name for index " +
					inttostring(index) + "(configurationNames size is " +
					inttostring(configurationNames.size()) + ").");

 		return configurationNames[0];
	}

	return configurationNames[index];
}
*/

const TagsList &ConfigurationTags::getMyTagsList() const
{
	return configurationTags;
}


TagsMap &ConfigurationTags::getMyTagsMap() const
{
	return fieldMap;
}

const std::string &ConfigurationTags::getFirmwareVersionTag()
{
	return firmwareVersion;
}
const std::string &ConfigurationTags::getFirmwareRevisionTag()
{
	return firmwareRevision;
}

const std::string &ConfigurationTags::getGCDPlaylistFilenameTag()
{
	return gcdPlaylistFilename;
}
