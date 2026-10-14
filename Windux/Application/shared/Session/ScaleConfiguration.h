//! ScaleConfiguration.h
/*!
 *  ScaleConfiguration contains all of the events and devices for all messages in the scale.
 *  Also contains the methods that use to exist in mainAppletDataPage.asp.
 *  In the Scale Application multiple classes represent multiple devices and events all spread
 *  across multiple classes. Those same events and devices are represented here in a single class
 *  file. The events and devices in this file match the Applications structure concerning numbers
 *  and names for easy mapping and ease of use. This file replaces all of the forms that previously
 *  existed in domOperate, domSuper, domService, and mainAppletDataPage. This specific file contains
 *  all of the methods from MADP. All of the devices and events are represented as enums in the .h file.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef SCALECONFIGURATION_H
#define SCALECONFIGURATION_H

#include "Session.h"
#include "Messages.h"

#include <datastore.h>
#include <mapdataobjectcontainer.h>
#include "../sessionHandlerWidgets/softkeytogglemanager.h"
#include "../platform/python.h"

#include <QMap>
#include <QObject>

namespace hobart {
class dynamicContentWidget;
class numericLineEdit;
class refuseBinWidget;
class softkeyButton;
class softkeyButtonListWidget;
class softkeysWidget;
}

class ScaleConfiguration : public QObject
{
    // mainAppletDataPage.asp
    Q_OBJECT

public:
    ScaleConfiguration( Session *session );

    /* These methods in the Legacy code existed in mainAppletDataPage.asp, and ScaleConfiguration.h/cpp         */
    /* corresponds to the very same file. However, in an effort to better organize the code, we should          */
    /* make an effort to move at least some of these methods to places like Session.cpp and UiUtilities.cpp     */

    QString getFPPLanguage();
    QString getLanguageAndCountry();
    bool getScaleIsMetric();
    int  getScaleModel();
    int  getScaleMode();
    QString getCountryCode();
    QString getLanguageCode();
    QString getSecCountryCode();
    QString getSecLanguageCode();
    QString getDateStyle();
    QString getTimeFormat();
    bool getFeatureInstalled( const QString& feature, bool *exists = NULL );
    bool getIsPick5Active();
    bool getIsPick5Enabled();
    void setIsPick5Active(bool isActive);
    void setIsPick5Enabled(bool isEnabled);

    void setIsChangeLanguageTurnedOn(bool isEnabled);
    int  setOpenFrame();
    void setZeroFlag(int zeroFlagStatus, QString capIndicator);
    void setDateStyle(QString newDateFormat);
    void setTimeFormat(QString newTimeFormat);
    void setFeatureInstalled( const MadpLicensedPackages& m );
    void setFeatureInstalled( const QStringList& features );
    void setFeatureInstalled( const QString& feature, bool installed );
    void setConfiguredHomepage(QString home);
    QString getConfiguredHomepage();
    QString getRelativeHelpManualFile();

    void okayMsgs();
    void denyMsgs();

    void sendBeep();
    bool sendFeatureRequest( const QString& feature );
    void sendOperatorDisplayConfig();

    QString getScaleModelName(bool fetch = false);        // get the actual scale name, not model number
    bool isWrapperTwoPrinter();
    bool isWrapperRightDischarge();

    void printProductComplete( int pluNum );

    QDate currentDate() const;
    QTime currentTime() const;
    QDateTime now() const;

    bool getIsOperatorLoggedIn();
    void setIsOperatorLoggedIn(bool isLoggedIn);

    bool getAreDialogsTurnedOff();
    bool setAreDialogsTurnedOff(bool isTurnedOff);

    QString getLoggedOperatorId();
    void setLoggedOperatorId(QString id);

    QString getLoggedOperatorName();
    void setLoggedOperatorName( QString name );

    int getLoggedOperatorAccess();
    void setLoggedOperatorAccess( int level );

    // Variable Declarations - from MainAppletDataPage.asp
    DomCurrentPLU currentPLU;
    DomCurrentPLU previousPLU;
    DomDefaultCurrentPLU defaultCurrentPLU;
    DomDateAndTime dateAndTime;
    DomLocalVars domlocalvars;
    DomEvolWrapperStatus evolWrapperStatus;
    DomEvolWrapperConfig evolWrapperConfig;
    //DomEvolWrapperHeaterConfig evolWrapperHeaterConfig;
    DomEvolWrapperSystemInformation evolWrapperSystemInformation;
    DomNormalScaleOperation normalScaleOperation;
    DomPluSupport pluSupport;
    DomPrinterConfigRead printerConfigRead;
    DomPrinterStockMetrics printerStockMetrics;
    DomPrWriteAllExternalPrinter allExternalPrinters;
    DomStoreInformation storeInfo;
    DomWeigherConfigRead weigherConfigRead;
    DomWeigherInfoRead weigherInfoRead;
    DomWeighModeTotalType weighModeTotalType;
    DomAuditLogInformation auditLogInformation;
    DomWeigherSwitch weigherSwitch;
    DomCuttingTest cuttingTestWrite;
    DomCuttingTestTotals cuttingTestTotals;                 // used later to display the last cut test run
    DomCuttingTestHistoryTotals cuttingTestHistoryTotals;   // used later to display the cutting test history
    DomPrintSpecialFields printSpecialFields;               // fields to print
    DomCurrentDept currentDept;
    DomLabelApplierConfig labelApplierConfig;
    DomEvolLabelApplierConfig evolLabelApplierConfig;
    DomCurrentScaleSetup currentScaleSetup;

    DomSoftkeyFeature softkeyFeature;
    DomSoftkey softkeyItem;
    MadpCurrencySettings currencySettings;
    MadpForcedFieldFlags forcedFieldFlags;
    MadpFrequentShopper frequentShopper;
    MadpInternalConfig internalConfig;                       // has to be declared after the struct
    MadpWrapperTimingsTestResults wrTimingsTestResults;
    MadpNetworkConnectivity networkConnectivity;
    MadpOperatorCanModify operatorCanModify;
    MadpOperatorDisplayFields operatorDisplayFields;
    MadpAccumItemsConfig accumulateItemsConfig;
    MadpPeripheralList peripheralList;
    MadpScaleConfig scaleConfig;
    MadpWeightData weightData;
    MadpUpdateCurrentPLUTotals currentPLUTotals;
    MadpKeyActivation keyActivation;
    MadpLostPeripheralList lostPeripheralList_;
    MadpUpgradeLists upgradeList;
    MadpCutTestPlu cutTestPlu;
    DomAutomaticUpdate automaticUpdate;
    DomBoardSiteTestsProgressMsg boardSiteProgress;
    DomUsbWeigherPowerupTests weigherPowerupTests;
    DomPrinterPowerupTests printerPowerupTests;

    hobart::softkeyToggleManager *toggleChangeLabel;

    int myConsole;                              // primary=1, remote=2, intranet=4
    int currentLanguage;                        // default is English
    int changeLanguage;                         // are we in the process of changing the language?
    QString currentEXT;                         // default is enUS
    bool rxMsgsOK;                              // are incoming messages proessed?
    bool pluMsgOK;                              // are PLU messages processed
    bool suspendMsgs;                           // are all messages suspended? Moved from scale_scripts
    bool quietDialogs;                          // even tho dialogs are triggered, dont pop them
    bool displayRecipes;                        // display them in the FPP windown
    bool printKeyPressed;                       // print key was pressed

    int arrayLen;                               // used in handleMessage, how long is the vector?
    int SessionDevice;                          // used in handleMessage, what is the device
    int SessionEvent;                           // used in handleMessage, what is the event

    bool receivedUpgradeList;                   // used in PeripheralUpgradeList msg, true if we have received upgradeList
    bool operatorLoggedIn;						//	HA-185: We need to know for the operatemodepanel if the Operator is logged in or not.
    bool operatorLoggedInForCutTest;            // Operator logged in to do a cut test
    bool opLoggedInForCutTestHistory;           // Operator has to log in for the cut test history bc of access rights
    bool askOperatorToLoginToStartCutTest;      // request the operator to log in
    bool pickFiveActive_;                       // flagged when we have an active PLU WITH min and max fields limits in them. Go orange
    bool pickFiveEnabled_;                      // indicates when pick 5 has been turned on from the menu, but there may not be an active PLU
    bool startingCutTest;                       // flagged when we press the cutting test history flashkey
    bool cuttingTestActive;                     // copy of sessionmainwindow cuttingTestActive_
    bool cuttingTestDialogActive;               // copy of sessionmainwindwo cuttingTestDialogActive_
    bool cuttingTestHistoryActive;              // is the cut test history dialog on the screen
    bool cuttingTestMeatSpecActive;             // do we have this loaded?
    int areDialogsTurnedOff;                   // Can be set when dialogs are turned off. And then can be used to turn off other stuff
    bool meatSheetScreenTimedOut;               // track if we hit the inactivity timeout on meat spec screen
    bool resumeIncompleteTest;                  // if we are resuming a suspended test and there is still data to add to it

    int wrapperPrinterCount;

    // why do we have these?                    // use these for both the changeProduct and ProductDefaults
    int primaryLabelPlacement;                  // save local data (plu or defaultPLU) data to them just b4
    int secondaryLabelPlacement;                // going to LabelView - and then when returning from label
    int primaryLabelRotation;                   // view - save back to LOCAL.  Now if on LabelView screen
    int secondaryLabelRotation;                 // and we encounter None value - go get Defaults from Config

    QString loggedOperatorId;                   // The id for the current operator id
    QString loggedOperatorName;                 // The name of the logged in operator

    int loggedOperatorAccess;                   // The access level of the currently logged in operator
    int returnHomeLocation;                     // What screen the Cutting Test Details Option1 returns to when done.

    int  charSearchSoftkeyCount;                // How many flashkeys came in from the application?

    int toggledRow;
    int toggledCol;
    int toggledFixed;
    int toggledLastRow;
    int toggledLastCol;
    bool toggledState;

    QString printerOneVer;
    QString printerTwoVer;
    QString printerTreVer;

    // TODO PJR420 - now that I am adding a 2nd set of these, and then prolly a 3rd,
    // I need to make a class, put these vars in it, and access the class
    // 09-15-2022: I made a class - start moving these vars into it. softkeyToggleManager()

    bool toggleExtraDayNow;                     // Toggled Add Days flashkey, true until Rx 136,22 redraws flashkeykey, ON OR OFF
    bool toggleExtraLeftPanelDayNow;            // Toggled Add Days flashkey in LEFT PANEL, true until Rx 136,22 redraws flashkeykey, ON OR OFF

    bool extraShelfLifeDayAvail;
    bool toggleExtraDayON;                      // toggled in flashkey action, stays true the entire time the key is toggled - any key in any feature
    int toggleExtraDayRow;                      // where it is located. Once set, can really just remain set
    int toggleExtraDayCol;                      // where it is located. Once set, can really just remain set
    QString toggledExtraDayImage;

    bool toggleExtraDayNumPadON;                // toggled in flashkey action, stays true the entire time the key is toggled
    int toggleExtraDayRowNumPad;                // where it is located. Once set, can really just remain set
    int toggleExtraDayColNumPad;                // where it is located. Once set, can really just remain set
    QString toggledExtraDayImageNumPad;

    bool toggleExtraDayLeftON;                // toggled in flashkey action, stays true the entire time the key is toggled
    int toggleExtraDayRowLeft;                // where it is located. Once set, can really just remain set
    int toggleExtraDayColLeft;                // where it is located. Once set, can really just remain set
    QString toggledExtraDayImageLeft;

    bool tareFlashkeysDoExist;
    bool showToggled;
    bool gen2UIActive;

    //python *pythonProcess;

    bool isCLARefresh_;
    bool isCLA_;

    // SOF-5996: used in manufacturing test
    QString cpuCount, boardType, sdCardSize,hostName_,macAddressFromScript,initialScaleModelName;
    QStringList flashInfo,netInfo;
    QStringList scriptsToExec,scriptsToUpdate;

    void updateScriptData(int state);


    bool isWifiModulePresent_;

    // label data, graphic data, etc...

    class metaData
    {
    public:
        QString width;
        QString height;
        bool custom;
        QString rotationOnStock;
    };

    dataStore<mapDataObjectContainer> dataStore_;

    QMap<QString, metaData> graphicMetaDataMap_;
    QMap<int, metaData> labelMetaDataMap_;

private:

    QMutex m_;

    Session *session_;

    static const int featureRequestWaitTime_;
    QMap<QString, bool> features_;
    QString configuredHomePage_;

    void execScript(const QString name);

private slots:
    void onHandleScript();

};

#endif // SCALECONFIGURATION_H
