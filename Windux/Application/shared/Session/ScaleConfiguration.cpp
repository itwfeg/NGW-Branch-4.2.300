//! ScaleConfiguration.cpp
/*!
 *  ScaleConfiguration contains all of the events and devices for all messages in the scale.
 *  Also contains the methods that use to exist in mainAppletDataPage.asp.
 *  In the Scale Application multiple classes represent multiple devices and events all spread
 *  across multiple classes. Those same events and devices are represented here in a single class
 *  file. The events and devices in this file match the Applications structure concerning numbers
 *  and names for easy mapping and ease of use. This file replaces all of the forms that previously
 *  existed in domOperate, domSuper, domService, and mainAppletDataPage. This specific file contains
 *  all of the methods from MADP. All of the devices and events are represented as enums in the .h file.
*/

#include "ScaleConfiguration.h"
#include "UiUtilities.h"
#include "MessageParser.h"
#include "Session.h"
#include "Devices.h"                                                // why cant I put these in ScaleConfiguration.h? I get errors
#include "Events.h"
#include "../sessionHandlerWidgets/softkeybutton.h"
#include "../sessionHandlerWidgets/softkeytogglemanager.h"
#include "../platform/python.h"
#include "logmanager.h"

#include <QApplication>

// Amount of time to wait for backend to respond to feature request.
const int ScaleConfiguration::featureRequestWaitTime_( 2500 );

ScaleConfiguration::ScaleConfiguration( Session *session ) :
    session_( session )
{
    /* MainAppletDataPage.asp */

    quietDialogs   = false;                                         // allow them to be noisy except for special cases.
    displayRecipes = false;
    changeLanguage = 0;                                             // in the process of changing the language
    currentLanguage = 1;                                            // default is primary

    currentEXT = "enUS";
    rxMsgsOK = 1;
    pluMsgOK = 1;
    printKeyPressed = false;
    receivedUpgradeList = false;

    internalConfig.scaleMode = 0;
    internalConfig.ssactive_lock = false;                           // by default, we dont go to self service on a boot unless told to
    internalConfig.ssactive_tenative_lock = false;                  // by default until we get a 100,10
    internalConfig.activate_selfservice = false;
    internalConfig.updateSelfServiceFlashkeys = true;               // default true so it can be requested the first time in
    internalConfig.selfServicePreventClicks = false;
    internalConfig.clickedPrintLabelFromFinishScreen = false;       // Pressed Print Label on Print or Scan Screen (Finish Screen)
    internalConfig.selfServiceSendReimageLabelsMsg = false;
    internalConfig.updateOperateFlashkeyData = false;
    internalConfig.updateNavigateHomeFlashkeys = false;             // if going home update flashkeys
    internalConfig.updateOperateLabelData = false;
    internalConfig.defaultFontName = "Hobart";
    internalConfig.defaultFontSize = 5;
    internalConfig.defaultLabelHeight = 100.0;
    internalConfig.defaultLabelWidth = 400.0;
    internalConfig.scaleBootComplete = false;                       // indicates that the OK button has not yet been pressed at boot
    internalConfig.didNotSendInitialReadWeighMode = true;           // at boot we need to determine what mode we are in
    internalConfig.isMissingPerifAtBoot = false;                    // set to true when a missing perif message comes in at boot up
    internalConfig.hasExternalPrinterSettingsChanged = false;       // do we have external printers? Did their settings in Setup Utility change?
    internalConfig.needsWrapperUpgrade = false;                     // set to true when a Wrapper upgrade comes in at boot up
    internalConfig.needsCLAUpgrade = false;                         // set to true when a CLA upgrade comes in at boot up
    internalConfig.needsNextGenWrapperUpgrade = false;              // set to true when a Wrapper upgrade comes in at boot up
    internalConfig.needsKPCLoaderUpgrade = false;
    internalConfig.needsKPCUpgraderUpgrade = false;
    internalConfig.stifleWriteAll = false;
    internalConfig.isScaleCalibrating = false;

    internalConfig.timingsUpdatesInProgress = false;
    internalConfig.timingsUpdatesAreComplete = false;
    internalConfig.dialogSuppressed = false;
    internalConfig.messagesSuppressed = false;
    internalConfig.sysInfoUpdatesInProgress = false;
    internalConfig.sysInfoUpdatesAreComplete = false;
    internalConfig.sysInfoDialogSuppressed = false;
    internalConfig.scaleWentOutOfTilt = false;
    internalConfig.isLeftPanelActivated = false;
    internalConfig.selfservInvalidPlu = false;                      // self serv requested a PLU with an invalid label

    forcedFieldFlags.pluIsForcedCool = false;                       // used as an individual check in checkForForcedCool()

    cutTestPlu.pluNumber = 0;                                       // need value from the plu even when the primary plu is not loaded
    cutTestPlu.meatSpecSheet = 0;                                   // any longer and we are processing sub primal PLUs for the cut test.
    cutTestPlu.cutTestNumber = 0;                                   // tie the primal plu to a meat spec sheet too.

    pickFiveActive_  = false;
    pickFiveEnabled_ = false;
    startingCutTest = false;
    cuttingTestActive = false;
    cuttingTestDialogActive = false;
    cuttingTestHistoryActive = false;
    cuttingTestMeatSpecActive = false;
    areDialogsTurnedOff = 0;                                        // Can be set when dialogs are turned off. And then can be used to turn off other stuff
    meatSheetScreenTimedOut = false;
    resumeIncompleteTest = false;

    operatorLoggedIn = false;										// HA-185: We need to know for the operatemodepanel if the Operator is logged in or not.
    loggedOperatorId = "";
    operatorLoggedInForCutTest = false;                             // log in for intial comments
    opLoggedInForCutTestHistory = false;                            // log in to see the history
    askOperatorToLoginToStartCutTest = false;
    returnHomeLocation = 0;

    charSearchSoftkeyCount = 0;                                     // by default we came from the Cut Test History Screen
    toggledRow = 0;
    toggledCol = 0;
    toggledFixed = 0;
    toggledLastRow = 0;                                             // horizontal size of current group
    toggledLastCol = 0;                                             // vertical size of current group
    toggledState = false;                                           // default false, set = false when flashkey clicked, set to true when specific flashkeys are clicked (Most Common, Cats, etc) or a 136,42 is received

    printerOneVer = "0.0.0";
    printerTwoVer = "0.0.0";
    printerTreVer = "0.0.0";

    // TODO PJR420 - now that I am adding a 2nd set of these, and then a 3rd,
    // I need to make a class, put these vars in it, and use the class
    // 3-14-22 - OK I made a class, sellBySoftkeyButton, need to start using it
    extraShelfLifeDayAvail = false;                                 // indicates we have Toggle Extra Shelf Life Day Flashkey active in a group
    toggleExtraDayNow = false;                                      // Toggled Add Days key, Stays true until Rx 136,22 and udpated, ON or OFF
    toggleExtraLeftPanelDayNow = false;

    toggleExtraDayON = false;                                       // flips back and forth depending if the extra day flashkey is clicked
    toggleExtraDayRow = 0;                                          // where the flashkey is located - update every time flashkey is clicked
    toggleExtraDayCol = 0;                                          // where the flashkey is located
    toggledExtraDayImage = "";                                      // Keep track and save the image name so we can use it anytime we want it

    // Finish these and get them all in a class
    toggleExtraDayNumPadON = false;                                 // toggled in flashkey action, stays true the entire time the key is toggled
    toggleExtraDayRowNumPad = 0;                                    // where it is located. Once set, can really just remain set
    toggleExtraDayColNumPad = 0;                                    // where it is located. Once set, can really just remain set
    toggledExtraDayImageNumPad = "";                                // Keep track and save the image name so we can use it anytime we want it

    // Finish these and get them all in a class
    toggleExtraDayLeftON = false;                                   // toggled in flashkey action, stays true the entire time the key is toggled
    toggleExtraDayRowLeft = 0;                                      // where it is located. Once set, can really just remain set
    toggleExtraDayColLeft = 0;                                      // where it is located. Once set, can really just remain set
    toggledExtraDayImageLeft = "";                                  // Keep track and save the image name so we can use it anytime we want it

    tareFlashkeysDoExist = false;
    showToggled = true;                                             // default true, set to false in flashkey editor so we dont see toggled cells there                                     
    gen2UIActive = false;

    toggleChangeLabel = NULL;                                       // Need to implemnt this class and clean up code

    isCLARefresh_ = false;
    isCLA_ = false;

    internalConfig.isClaModelInitialization = false;                // SOF-4929: Indicates user selected 'CLA' button on Model Selection dialog
    internalConfig.isWrapperModelInitialization = false;            // SOF-4929: Indicates user selected a model on the Model Selection dialog
    internalConfig.isOkToLookUpPlu = true;							// By default we can look up a PLU
    internalConfig.poppedRezero = false;                            // did we pop a dialog?
    internalConfig.isOkDroppedPLU = 0;
    internalConfig.printerCycleCounts.append("0");                  // set the default cycle counts for the 3 printers
    internalConfig.printerCycleCounts.append( "0" );
    internalConfig.printerCycleCounts.append( "0" );

    // 2 is no label placement
    primaryLabelPlacement = 2;
    secondaryLabelPlacement = 2;
    primaryLabelPlacementDefault = 2;
    secondaryLabelPlacementDefault = 2;

    primaryLabelRotation = 0;
    secondaryLabelRotation = 0;
    primaryLabelRotationDefault = 0;
    secondaryLabelRotationDefault = 0;

    primaryPlacementSystem = 0;
    secondaryPlacementSystem = 1;

    primaryDefaultLabelType = 0;
    secondaryDefaultLabelType = 0;


    scriptsToExec << "sdCardSize" << "getBoardType" << "cpuCount" << "getMac" << "getModelName" << "getSerialFlashInfo"
    << "getNetworkInfo" << "getHostname";

    scriptsToUpdate << "getMac" << "getModelName" << "getSerialFlashInfo" << "getNetworkInfo" << "getHostname";

    updateScriptData(0);
}

/************************************************************************/
/* GETTERS AND SETTERS SECTION.                                         */
/* Need to determine if this is still the best way to do this           */
/* to set and get these struct vars. Pub .v Pri                         */
/************************************************************************/

//! getFPPLanguage()
/*!
    getFPPLanguage() gets the language and country locale for FPP
    \  params:  none
    \  returns: the language and country as enUS format
    \  Bugs\Issues: None
*/
QString ScaleConfiguration::getFPPLanguage()
{
    QString langAndCountry = "";

    if(currentLanguage == 1)
        langAndCountry = scaleConfig.languageCode().toUpper() + "." + scaleConfig.countryCode().toUpper();
    else
        langAndCountry = scaleConfig.secLanguageCode().toUpper() + "." + scaleConfig.secCountryCode().toUpper();

    return langAndCountry;
}

//! getLanguageAndCountry()
/*!
    getLanguageAndCountry() gets the language and country locale for regular forms being loaded. This method
    was previously know by "determineLangAndCountry()"
    \  params:  none
    \  returns: the current language and country as enUS format.
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getLanguageAndCountry()
{
    QString langAndCountry = "";

    if(currentLanguage == 1)
        langAndCountry = "&languageCode=" + scaleConfig.languageCode() + "&countryCode=" + scaleConfig.countryCode();
    else
        langAndCountry = "&languageCode=" + scaleConfig.secLanguageCode() + "&countryCode=" + scaleConfig.secCountryCode();

    return langAndCountry;
}

//! getScaleModel()
/*!
    getScaleModel() gets the current scale model. scaleModel=1 is a selfserv scale, scaleModel=2 is a
    prepack scale, and scaleMode=3 is a service counter scale.
    \  params:  none
    \  returns: the current scale Model
    \  Bugs\Issues: None
 */
int ScaleConfiguration::getScaleModel()
{
    return scaleConfig.scaleModel().toInt();
}

//! getCountryCode()
/*!
    getCountryCode() gets the primary country code
    \  params:  none
    \  returns: the current primary country code
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getCountryCode()
{
    return scaleConfig.countryCode();
}

//! getLanguageCode()
/*!
    getLanguageCode() gets the primary language code
    \  params:  none
    \  returns: the primary language code
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getLanguageCode()
{
    return scaleConfig.languageCode();
}

//! getSecCountryCode()
/*!
    getSecCountryCode() gets the secondary country code
    \  returns: the current secondary country code
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getSecCountryCode()
{
    return scaleConfig.secCountryCode();
}

//! getSecLanguageCode()
/*!
    getSecLanguageCode() gets the secondary language code
    \  params:  none
    \  returns: the current secondary language code
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getSecLanguageCode()
{
    return scaleConfig.secLanguageCode();
}

//! dateStyle()
/*!
    dateStyle() gets the system date format style for the scale
    \  params:  none
    \  returns: the current date format
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getDateStyle()
{
    return QString::number( scaleConfig.dateStyle() );
}

//! getTimeFormat()
/*!
    getTimeFormat() gets the system time format style for the scale
    \  params:  none
    \  returns: the current time format
    \  Bugs\Issues: None
 */
QString ScaleConfiguration::getTimeFormat()
{
    return QString::number( scaleConfig.timeFormat() );
}

//! getFeatureInstalled()
/*!
    getFeatureInstalled() checks if feature is installed
    \  params:  feature
    \  returns: true if feature installed
 */
bool ScaleConfiguration::getFeatureInstalled( const QString& feature, bool *exists )
{
    QMutexLocker guard( &m_ );

    bool found( features_.contains( feature ) );

    if ( exists )
        (*exists) = found;

    return (( found ) && ( features_[feature] ));
}

//! getScaleMode()
/*!
    getScaleMode() gets the current scale mode. ScaleMode==1 is Operate Mode, ScaleMode==2 is
    Supervisor mode, and ScaleMode==3 is Service Mode.
    \  params:  none
    \  returns: the scaleMode
    \  Bugs\Issues: None
 */
int ScaleConfiguration::getScaleMode()
{
    return internalConfig.scaleMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScaleConfiguration::getIsPick5Enabled()
{
    return pickFiveEnabled_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScaleConfiguration::getIsPick5Active()
{
    return pickFiveActive_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleConfiguration::setIsPick5Enabled(bool isEnabled)
{
    pickFiveEnabled_ = isEnabled;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleConfiguration::setIsPick5Active(bool isEnabled)
{
    pickFiveActive_ = isEnabled;
}

//	Originally called by Client.java
//! getScaleIsMetric()
/*!

    getScaleIsMetric() determines if the scale is in metric mode, or not.
    \  params:  none
    \  returns: if the scale is in metric mode
    \  Bugs\Issues: None
 */
bool ScaleConfiguration::getScaleIsMetric()
{
    return scaleConfig.isScaleMetric();
}

//! dateStyle()
/*!
    dateStyle() gets the system date format style for the scale
    \  params:  none
    \  returns: the current date format
    \  Bugs\Issues: None
 */
    void ScaleConfiguration::setDateStyle(QString newDateFormat)
    {
        scaleConfig.dateStyleRaw() = newDateFormat;
    }

//! getTimeFormat()
/*!
    getTimeFormat() gets the system time format style for the scale
    \  params:  none
    \  returns: the current time format
    \  Bugs\Issues: None
 */
void ScaleConfiguration::setTimeFormat(QString newTimeFormat)
{
    scaleConfig.timeFormatRaw() = newTimeFormat;
}

void ScaleConfiguration::setConfiguredHomepage( QString home )
{
    configuredHomePage_ = home;
}

QString ScaleConfiguration::getConfiguredHomepage()
{
    return configuredHomePage_;
}

void ScaleConfiguration::setFeatureInstalled( const MadpLicensedPackages& m )
{
    // nothing to do
    if ( m.features().isEmpty() )
        return;

    // save off licensing
    setFeatureInstalled( m.features().split( "," ) );
}

void ScaleConfiguration::setFeatureInstalled( const QStringList& features )
{
    foreach ( const QString& f, features )
        setFeatureInstalled( f, true );
}

void ScaleConfiguration::setFeatureInstalled( const QString& feature, bool installed )
{
    QMutexLocker guard( &m_ );

    // update feature
    features_[ feature ] = installed;
}

//! setIsChangeLanguageTurnedOn()
/*!
    setIsChangeLanguageTurnedOn() sets the value that indicates if an operator can change the language.
    \  params:  a boolean that indicates if the operator can change the language feature is turned on
    \  returns: none
    \  Bugs\Issues: None
 */
void ScaleConfiguration::setIsChangeLanguageTurnedOn(bool isEnabled)
{
    internalConfig.changeLangIsEnabled = isEnabled;
}

/****************************************************************************************************/
/* ACTION METHOD SECTION.                                                                           */
/* SECTION where action occurs. Lets get something done.                                            */
/****************************************************************************************************/

//! sendBeep()
/*!
    sendBeep is used to send a message to the application to emit a beep.
    \  params: none
    \  returns: none
    \  Bugs\Issues:
 */
void ScaleConfiguration::sendBeep()
{
    // This SessionDev msg actually sends the beep to the speaker.
    session_->send( SessionDev, SnSoundBeep, "1" );
}

bool ScaleConfiguration::sendFeatureRequest( const QString& feature )
{
    bool existsPrev;
    bool installedPrev( false );

    {
        QMutexLocker guard( &m_ );

        // remove from mapping
        if ( (existsPrev = features_.contains( feature )) )
        {
            // save off current value
            installedPrev = features_[feature];

            features_.remove( feature );
        }
    }

    // request feature status from backend
    session_->send( SessionDev, SnReadLicensedPackages );

    // wait for response
    QTime timeout( QTime::currentTime() );
    timeout.addMSecs( featureRequestWaitTime_ );

    do
    {
        // process pending events
        QApplication::sendPostedEvents();
        QApplication::flush();

        QApplication::processEvents();

        // check if we have the feature state from backend yet
        bool exists;
        bool installed( getFeatureInstalled( feature, &exists ) );

        // success
        if ( exists )
            return installed;

    } while ( QTime::currentTime() < timeout );

    // timeout waiting for result
    if ( existsPrev )
    {
        QMutexLocker guard( &m_ );

        // one final check...
        if ( features_.contains( feature ) )
            return features_[feature];

        // add previous value to mapping
        features_[feature] = installedPrev;
    }

    return installedPrev;
}

void ScaleConfiguration::sendOperatorDisplayConfig()
{
    MadpOperatorDisplayFields operatorDisplayFields;
    displayRecipes = true;

    operatorDisplayFields.prodPlanningUpdateRaw() = "";

    QString msg = operatorDisplayFields.enableSellByRaw() + "," +
    operatorDisplayFields.enableBestBeforeRaw() + "," +
    operatorDisplayFields.enableProdImgRaw() + "," +
    operatorDisplayFields.enableUpcNumberRaw() + "," +
    operatorDisplayFields.enableFilmWidthRaw() + "," +
    operatorDisplayFields.enableFilmTensionRaw() + "," +
    operatorDisplayFields.enableWrapTypesRaw() + "," +
    operatorDisplayFields.enableCountryOfOrginRaw() + "," +
    operatorDisplayFields.enableTrackingNumberRaw() + "," +
    operatorDisplayFields.enableProdEntryRaw() + "," +
    operatorDisplayFields.prodEntryContinuousPrintRaw() + "," +
    operatorDisplayFields.enableProdPlanRaw() + "," +
    "\"" + operatorDisplayFields.prodPlanningFrame() + "\"" + "," +
    "\"" + operatorDisplayFields.prodPlanningUpdate() + "\"" + "," +
    operatorDisplayFields.appendingPluNumberToNameRaw() + "," +
    operatorDisplayFields.numberOfCharactersRaw() + "," +
    operatorDisplayFields.enableRunTotalsRaw() + "," +
    operatorDisplayFields.enablePackedOnRaw() + "," +
    operatorDisplayFields.dcaStartScreenTypeRaw() + "," +
    operatorDisplayFields.dcaRunScreenTypeRaw() + "," +
    operatorDisplayFields.dcaKeypadAreaTypeRaw() + "," +
    operatorDisplayFields.defaultFlashkeyFontPointsizeRaw();

    session_->send(ConfigurationDev, CfgPrepackSave, msg);     // 112,142 -> save Prepack Config (operator display fields)
    session_->send(ConfigurationDev, CfgPrepackRead ); // update ui
}

QDate ScaleConfiguration::currentDate() const
{
    return QDate( dateAndTime.hobYear(), dateAndTime.hobMonth(), dateAndTime.hobDay() );
}

QTime ScaleConfiguration::currentTime() const
{
    int h( dateAndTime.hobHour() );

    if (( "PM" == dateAndTime.hobAMPM() ) && ( h < 12 ))
        h += 12;

    return QTime( h, dateAndTime.hobMinute() );
}

QDateTime ScaleConfiguration::now() const
{
    return QDateTime( currentDate(), currentTime() );
}

//----------------------------------------
// Get the Scale Model: HTi, HTx, HTs, ...
//----------------------------------------
QString ScaleConfiguration::getScaleModelName(bool fetch)
{
    QString scaleName = session_->scaleConfig->scaleConfig.scaleName();

    if (!fetch && !scaleName.isEmpty() ) return scaleName;

    else
    {
        QStringList args;
        QString scaleModel;

        scaleModel = "";

        python pythonProcess;

        if ( pythonProcess.execCommand("getModelName", args, false, false, ( 30 * 1000 ) ) )
        {
            if ( pythonProcess.getExitCode() == 0 )
            {
                scaleModel = pythonProcess.readStdOut();
            }
        }
        return scaleModel;
    }
}

bool ScaleConfiguration::isWrapperTwoPrinter()
{
    QString modelName = getScaleModelName();
    modelName = modelName.trimmed();

    if( "NGW" == modelName.left( 3 ).toUpper() )
    {
        if( ( "2P" == modelName.right( 2 ).toUpper() ) || ( "CP" == modelName.right( 2 ).toUpper() ) ) // SOF-4782: Costco 2nd printer will have suffix of "CP"
        {
            return true;
        }
    }

    return false;
}

void ScaleConfiguration::printProductComplete( int plu )
{
    LOG_CLASS_INFO("scaleConfiguration", "ProductComplete");

    LOG_INFO("ScaleConfig case ProductComplete and DroppedPLU > 0 so autolookup plu : %d", plu );
}

bool ScaleConfiguration::isWrapperRightDischarge()
{
    QString modelName = getScaleModelName();

    if( modelName.contains( "NGW" ), Qt::CaseInsensitive )
    {
        if( modelName.contains( "LR" ), Qt::CaseInsensitive )
        {
            return true;
        }
    }

    return false;
}

void ScaleConfiguration::setIsOperatorLoggedIn(bool isLoggedIn)
{
    operatorLoggedIn = isLoggedIn;
}

bool ScaleConfiguration::getIsOperatorLoggedIn()
{
    return operatorLoggedIn;
}

void ScaleConfiguration::setLoggedOperatorId(QString id)
{
    loggedOperatorId = id;
}

bool ScaleConfiguration::getAreDialogsTurnedOff()
{
    return (0 < areDialogsTurnedOff);
}

bool ScaleConfiguration::setAreDialogsTurnedOff(bool isTurnedOff)
{
    // return true if they should send SnStop or SnRestart dialogs message
    bool result( false );

    // This is set to TRUE, if dialogs are disabled
    if ( isTurnedOff )
        result = (0 == areDialogsTurnedOff++);
    else
        result = (0 == --areDialogsTurnedOff);

    return result;
}

QString ScaleConfiguration::getLoggedOperatorId()
{
    return loggedOperatorId;
}

void ScaleConfiguration::setLoggedOperatorName(QString id)
{
    loggedOperatorName = id;
}

QString ScaleConfiguration::getLoggedOperatorName()
{
    return loggedOperatorName;
}

void ScaleConfiguration::setLoggedOperatorAccess(int level)
{
    loggedOperatorAccess = level;
}

void ScaleConfiguration::execScript(const QString name)
{
    LOG_CLASS_INFO("scaleConfiguration", "execScript");

    LOG_DEBUG("Launching python script %s in scale config", qPrintable(name) );

    python *pyProcess = new python();
    pyProcess->setNonBlocking();
    connect(pyProcess, SIGNAL(readyReadScript()), SLOT(onHandleScript()));

    QStringList args;

    if (name == "getSerialFlashInfo")
        args.append("--mfg");

    pyProcess->execCommand(name, args, false, false, ( 30 * 1000 ) );
}

/*
 * These are scripts whose data can change based on application changes
 * For example, we need to refresh the data used from the script getNetworkInfo
 * if we changed networks in setupUtility.
 */
void ScaleConfiguration::updateScriptData(int state)
{
    QStringList scriptsToUse;

    // initialization
    if (state == 0)
        scriptsToUse = scriptsToExec;
    else
        scriptsToUse = scriptsToUpdate;

    foreach (const QString &iter, scriptsToUse)
    {
        execScript(iter);
    }
}

void ScaleConfiguration::onHandleScript()
{
    LOG_CLASS_INFO("scaleConfiguration", "onHandleScript");

    python* theProc = dynamic_cast<python*>(sender());

    if (theProc)
    {
        QString result = theProc->readStdOut();
        QString scriptName = theProc->scriptName();

        LOG_DEBUG("Handling %s output in scale configuration with result %s", qPrintable(result), qPrintable(scriptName) );

        if(scriptName == "sdCardSize")
            sdCardSize = result;

        else if(scriptName == "getBoardType")
            boardType = result;

        else if(scriptName == "cpuCount")
            cpuCount = result;

        else if(scriptName == "getMac")
        {
            QString macAddress(result);
            QStringList macAddresses;
            macAddress = macAddress.mid(0,macAddress.length()-1);
            macAddressFromScript = macAddress;

            if(macAddress.size() > 0)
            {
                macAddresses = macAddress.split("|");
                if(macAddresses[1].startsWith("No",Qt::CaseInsensitive)) isWifiModulePresent_ = false;
                else isWifiModulePresent_ = true;
            }
        }
        else if(scriptName == "getSerialFlashInfo")
            flashInfo = result.split( "," );

        else if (scriptName == "getNetworkInfo")
            netInfo = result.split(",");
        else if (scriptName == "getHostname")
            hostName_ = result;
        else if (scriptName == "getModelName")
            initialScaleModelName = result;

        delete theProc;
    }

}

int ScaleConfiguration::getLoggedOperatorAccess()
{
    return loggedOperatorAccess;
}
