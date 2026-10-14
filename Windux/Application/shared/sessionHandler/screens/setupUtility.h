/**
 * @file mainwindow.h
 * @brief Widget to set system configuration of the scale.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Settings Wizard CSU
  * @{
*/

/*
PROLOGUE: Setup Utility (aka Setup Wizard)
SRD: 117
Date: Q4 2012
AUTHOR: Todd Davis
COMPANY: Hobart

PURPOSE: This application serves the REQ for a System settings software wizard
         for the QM Series scales to configure the system hardware's settings for:

         - Time & Date
         - Sound Volume
         - Network Configuration
         - Touchscreen Calibration
         - Administration: Users
         - Timestamp of Last Settings
*/


#ifndef SETUPUTILITY_H
#define SETUPUTILITY_H

#include "screenwidget.h"

#include "../dialogs/messageboxdialog.h"
#include "../dialogs/sessionerrordialog.h"

#include "../platform/network.h"
#include "../platform/python.h"

#include <videoplayerwidget.h>

#include <QtGlobal>

#include <QDir>
#include <QIcon>
#include <QProcess>
#include <QElapsedTimer>
#include <QIcon>
#include <QProcess>

class QIntValidator;
class DomConfigWriteSingleField;
class DomPrWriteAllUsbDriverNames;
class DomPrWriteAllExternalPrinter;
class DomPrWriteAllProductTextPrintParams;
class DomPrWriteAllNetworkPrinterDriverNames;
class MadpInternalConfig;

namespace Ui {
class setupUtility;
}

class setupUtility : public ScreenWidget{
    Q_OBJECT

    typedef setupUtility _Myt;
    typedef ScreenWidget _Mybase;
    


public:
    explicit setupUtility(SessionMainWindow *mainWindow, ModeWidget *parent = 0);
    virtual ~setupUtility();

    QString mode;
    static const QString ServicePassword; 

    static const bool USE_BACKDOOR;
    static const QString USERNAME;
    static const QString PASSWORD;
    static const QString PASSPHRASE;
    static void sleep(long iSleepTime);

    bool cancelPing;
    int volume;
    int networkCount;

    QDate *pickedDate;
    Network *commonNetwork;
    QString machineID;

   // void changeEvent(QEvent);
 //   void retranslateUI();
    void loadSavedNetwork(Network* network);

    // reboot vars below
    bool changedTimezone;
    bool changedHostname;
    bool changedSpeakers;
    bool changedAstronomical;

    QString initialHostname;
    QString initialTimeZone;
    bool initialAstronomical;

    void connectDB();
    void disconnectDB();
    enum State
    {
        Paused,
        Run
    };

    State state;
    void manualPause();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

signals:
    void outlogtext(QString ver);
protected:
  //  void paintEvent(QPaintEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    virtual bool eventFilter( QObject *obj, QEvent *event );


public slots:
//    void volumeSliderValueChanged(int);
//    void loginLanguageComboValueChange(int);
    void currentChangedSlot(int);
    void brightnessSpinBoxValueChanged(int);
    void volumeSpinBoxValueChanged(int);
    void changedOperatorDisplayBrightnessButtons();
    void changedCustomerDisplayBrightnessButtons();
    void changedCustomerSoundButtons();
    void changedOperatorSoundButtons();
    void datePicked(QDate selectionDate);
    void fillTimeZones();
    void astronomicalCheckboxValueChanged(int);
    void setTimeTab();
    void setTimezone();
    QStringList getWirelessAPs();
    void getUIMACAddress();
    void rebootScale();
    void getUIHostname();
    void revealNetworkGroupBox(int, int);
    void revealNetworkGroupBoxPrefilled(int isWireless);
    void checkAgainstPoorServiceTraining(QString);
    void checkAgainstEmptyPassword(QString);
    void networkTableWidgetSelectionChanged(int);
    void setWirelessNetworkEncryptionCombo(int);
    void loadWirelessSecurityMethodsCombo();
    void setCommonNetwork(Network* network);
    QStringList buildSaveConfigurationOptions(Network* network);
    QStringList buildImportConfigurationOptions(Network* network);
    QStringList buildExportConfigurationOptions();
    void setRunState();
    // void loadNetworkConnectionsTableView();

private slots:
    QString modelName();
 //   void loginBut_click();
 //   void logoutBut_click();
 //   void addUserBut_click();
 //   void deleteUserBut_click();
    void setVolumeBut_click();
    void setDateTimeBut_click();
    void setNtpDateTimeBut_click();
//    void cancelNetworkBut_click();
    void saveNetworkBut_click();
    void setHostnameBut_click();
    void setScalePasswordBut_click();
//    void applyNetworkBut_click();
    void calibrateScreenBut_click();
    void setBrightnessBut_click();
    void startPingBut_click();
    void addNetworkBut_click();
    void stopPingBut_click();
//    void exportBut_click();
//    void importBut_click();
    void testVolumeBut_click();
    void volumeIndicator0_click();
    void volumeIndicator1_click();
    void volumeIndicator2_click();
    void volumeIndicator3_click();
    void volumeIndicator4_click();
    void volumeIndicator5_click();
    void volumeIndicator6_click();
    void volumeIndicator7_click();
    void volumeIndicator8_click();
    void volumeIndicator9_click();
    void volumeIndicator10_click();
    void dhcpRadio_disableIPs();
    void staticIPRadio_enableIPs();
    void autoDNSRadio_click();
    void manualDNSRadio_click();
    void refreshNetworksButton_click();
    void readStdOutput();
    void printPingOutput();
    void readyReadStandardError();
    void finishedProcess(int value, QProcess::ExitStatus exitStatus);
    void printEAPOutput();
    void readyReadEAPStandardError();
    void finishedEAPProcess(int value, QProcess::ExitStatus exitStatus);
    void printNetworkOutput();
    void readyReadNetworkStandardError();
    void finishedNetworkProcess(int value, QProcess::ExitStatus exitStatus);
    void internalSpeakerClicked(bool);
    void externalSpeakerClicked(bool);
    void signingRequestButClicked();
    void setWirelessFrames(int);

    void doPreset(int);
    void importNetworkBut_click();
    void exportNetworkBut_click();
    void resizeControls();

    void playAudioFile();

    void gatewayYesSlot();
    void gatewayNoSlot();
//    QString encrypt(QString);
    void setVolumeIndicators(int);
    void setBrightnessIndicators(int);
    void muteButChanged(bool);
 //   bool useVPNConnection();
//    bool useWirelessConnection();
//    QStringList buildNetworkCommandOptions(Network* network);
 //   void loadSavedNetwork(Network *network);
    void gcdSliderChanged(int);
    void newWirelessNetworkBut_click();
    void newWiredNetworkBut_click();

    void hourBut_clicked();
    void minuteBut_clicked();
    void secondBut_clicked();
    void ampmBut_clicked();
    void setHostnamefromDHCPToggled(int newVal);
    void timeEditTimeChanged(QTime);

    void ntpHourBut_clicked();
    void ntpMinuteBut_clicked();
    void ntpSecondBut_clicked();
    void ntpAmpmBut_clicked();
    void ntpTimeEditTimeChanged(QTime);


    void updateCharacterCount(QString incomingString);
    /**
     * Slot to set the date and time string.
     */
    void setDateTimeText();
    void ntpFrequencyChanged(int index);
    void keepAppAlive();

    void serverCertButClicked();
    void chainFileButClicked();
    void clientCertDeleteButClicked();
    void serverCertDeleteButClicked();
    void clientCertButClicked();
    void updateServerCertFilename(int userSelection);
    void updateClientCertFilename(int userSelection);
    void searchClientCertButClicked();
    void searchTrustedRootCertButClicked();
    void closeCertInfoButClicked();
    void deleteServerCertYesBut_clicked();
    void deleteServerCertCancelBut_clicked();
    void deleteClientCertYesBut_clicked();
    void deleteClientCertCancelBut_clicked();
    void refreshNetworksPressed();
    void advancedNetworkingBut_clicked();
    void advancedNetworkingApplyBut_clicked();
    void advancedNetworkingCancelBut_clicked();
    void advancedNetworkingDefaultsBut_clicked();
    void advancedNetworkingLinkRateCombo_changed(int);

    // SLOTS for Printer Tab
    void txtPrinterSettingsChanged();
    void txtPrinterIpSettingsChanged();
    void cboPrinterDriverChanged(int index);
    void printerTypeChangedOne(int);
    void printerTypeChangedTwo(int);
    void testExternalPrinterOne();
    void testExternalPrinterTwo();
    void saveExternalPrinterSettings();
    void saveProductTextPrintParams();
    void updateSettingsPageComboNames(int, int);
    void gotoPrinterDataScreen();
    void gotoPrinterSettings();
    void chkPrinterSettingsOneChanged(int state);
    void chkPrinterSettingsTwoChanged(int state);
    void setTheDayOfMonth(int first, int second);
    void importNetworkSettings();
   // void revertToOriginalDate();

    void snmpManagerBut_clicked();

    void on_saveSNMPManagerBut_clicked();

    void on_snmpStopBut_clicked();

    void on_pushButton_3_clicked();

    void on_installationDateNoBut_clicked();

    void on_installationDateYesBut_clicked();

private:
    QElapsedTimer *masterTimer;
    QString importFilename;
    bool isInstallationDateSet();
    void manageInstallationDate();

    Ui::setupUtility *ui;
    bool updateDB(QString key, QString value);
    QProcess *pingProcess;
    bool connectionEstablished;
    QProcess *saveNetworkConfigurationProcess;
    QProcess *saveEAPConfigurationProcess;
    QString output;
    bool finishNetworkScreen(QString output);

    videoPlayer *player;

    QStringList stereoTestFiles;
    int currentPlaybackIndex;
    void saveConfigsNow();
    void saveNetConfigurationToScaleSettings();
    bool killStartupNetworking();
    DomPrWriteAllExternalPrinter *writeAllExternalPrinter_;
    DomPrWriteAllProductTextPrintParams *writeAllProdTxtPrintParams_;
    DomPrWriteAllProductTextPrintParams *promptness_;
    DomPrWriteAllExternalPrinter *extPrintSettings_;
    DomPrWriteAllUsbDriverNames *writeAllUsbDriverNames_;
    DomPrWriteAllNetworkPrinterDriverNames *writeAllNetworkDriverNames_;
    DomConfigWriteSingleField *configWriteSingleField;
    MadpInternalConfig *internalConfigData_;

    QDate currentUserDate;
    QDate lastDateThatWasSet;
    int tempDayOfMonth;

    bool tryingAHiddenSSID;
    bool isReadyToTranslate;
    bool linkRatewasAdded;
    bool linkRateAutomaticWasAdded;
    bool channelWasAdded;
    bool DNSSuffix1WasAdded;
    bool DNSSuffix2WasAdded;
    bool DNSSuffix3WasAdded;
    bool DNSSuffix4WasAdded;
    bool DNSSuffix5WasAdded;
    bool DNSSuffix6WasAdded;
    bool DHCPHostnameWasAdded;
    bool SNMPVariablesPortWasAdded;
    bool SNMPTrapsPortWasAdded;
    bool SNMPTrapsIPWasAdded;
    bool SNMPEnabledWasAdded;

    bool serverCertFilenameWasAdded;
    bool chainFilenameWasAdded;
    bool clientCertFilenameWasAdded;
    bool csrFilenameWasAdded;
    bool csrKeyFilenameWasAdded;
    bool clientKeyFilenameWasAdded;
    bool clientCertUsersFilenameWasAdded;
    bool clientCertPasswordWasAdded;
    bool isTimeTabFirstTime;
    bool isSoundTabFirstTime;

    bool isDHCPCheckedBoxd_Fix;
    bool isHostNameChecked_Fix;
    bool isWiredConnection_Fix;
    bool hostnameStayChecked_FIX;

    QString dbKey;
    QString dbValue;
    bool adapterAlreadyConnected;
    QString m_correctText;

    bool shouldChangeTime;
    bool shouldChangeNTP;
    bool mustTurnOnDialogs;
    bool visDemoMode;

    void execPythonCommand(QString command,
                           QStringList arguments=QStringList());
    void lookatNetworkAgain(QString output);
    int originalTimeZoneComboBoxIndex;
    int originalNTPTimeZoneComboBoxIndex;

    QString hostnamePromptText;
    QString customerBrightness, operatorBrightness;
    QString customerVolume, operatorVolume, isAstronomical;
    QString soundDevice;
    QString dbConnectionName;
    QString dbConnectionType;
    QString dbGCD;
    QString dbTimezone;
    QString dbisStatic;
    QString dbgateway;
    QString dbnetmask;
    QString dbipAddress;
    QString dbautoDNS;
    QString dbdns1;
    QString dbdns2;
    QString dbdns3;
    QString dbsecurity;
    QString dbencryption;
    QString dbfirstTimeExport;
    QString dbNTPServer;
    QString dbNTPFrequency;
    QString dbNTPDayOfWeek;
    QString dbNTPTimeOfDay;
    QString dbNTPTimeZone;
    QString dbNTPAntiMeridian;
    QString dbTimeKeeper;
    QString dbWpaPassphrase;
    QString dbEapUsername;
    QString dbEapPassword;
    QString dbSpeakerOutput;
    QString dbServerCertFilename;
    QString dbChainFilename;
    QString dbClientCertFilename;
    QString dbCSRFilename;
    QString dbCSRKeyFilename;
    QString dbAuthentication;
    QString dbClientKeyFilename;
    QString dbClientKeyPassword;
    QString dbClientCertUsersFilename;
    QString dbClientCertPassword;
    QString UsersTrustedRoot;
    QString UsersClientCert;
    QString dbLinkRate;
    QString dbAutomaticLinkRate;
    QString dbChannel;
    QString dbDNSSuffix1;
    QString dbDNSSuffix2;
    QString dbDNSSuffix3;
    QString dbDNSSuffix4;
    QString dbDNSSuffix5;
    QString dbDNSSuffix6;
    QString dbDHCPHostname;
    QString dbSNMPVariablesPort;
    QString dbSNMPTrapsPort;
    QString dbSNMPTrapsIP;
    QString dbSNMPEnabled;

    int linkRateIndex;
    int channelIndex;
    bool automaticLinkRate;
    QString DNSSuffix1Memory;
    QString DNSSuffix2Memory;
    QString DNSSuffix3Memory;
    QString DNSSuffix4Memory;
    QString DNSSuffix5Memory;
    QString DNSSuffix6Memory;

    QString SNMPVariablesPort;
    QString SNMPTrapsPort;
    QString SNMPTrapsIP;
    QString SNMPEnabled;

    bool dbDisplayTab;
    bool dbCustomerSoundTab;
    bool dbOperatorSoundTab;
    bool dbTimeTab;
    bool dbNetworkTab;
    bool dbHostTab;
    bool dbPrinterTab;
    int selectedConnectionRow;
    bool foundSelectedConnection;
    void loadDisplayBrightnessValues(QString display);
    void loadSoundVolumeValues(QString device);
    void changeTimeAfterServiceKeyRemoved();
    void endNetworkConnection();
    QString alertsText;
    void syncHostname();

    void shutdown();
    void reboot();
    void turnOffDisplays();

    QDir dir;

    bool updateBrightness(int percentage, QString device);
    bool firstTimeCustomerBrightnessDisplay;
    bool firstTimeCustomerVolume;
    QString displayDevice;

    QString databaseFileLocation;

    void bringUpNICs();
    void clearNetworkData();
    void controlNICs(QString device, QString direction);

    QString connectionType;
    void loadNetworkConnections();
    int wiredSpeed();
    QStringList buildNetworkCommandOptionsNew(Network *locNetwork);
    QStringList buildNetworkCommandOptions(Network *locNetwork);
    //    QStringList buildImportConfigurationOptions(Network * locNetwork);
    bool networkProcessConnected;
    bool eapProcessConnected;
    void changeConnectedIcons(int rowToConnect, bool connectionOKAY);

    // void getConnectionStatus();
    void powerDevices(QString states);
    QString getDBValue(QString key);

    bool finalIsWireless;

    bool exportNetworkSettings;
    bool exportHostnameSettings;
    bool exportNTPSettings;
    bool exportNTPRemoveSettings;

    bool networkTabAlreadyLoaded;
    void loadNetworkTab();
    void loadHostsTab();
    void loadSNMPTab();
    bool weHaveServiceMode;
    bool connectedSuccessfully;
    QIntValidator *ipOctet_;
    void beginTheNetworkConnection();
    QStringList populateWEPKeys();
    void setTheStoredNetworkFields();
    QString userSelectedConnectionName;
    QString m_originalTimeEditStyleSheet;

    bool connectionTimeout;
    QString noConnectionPhrase;

    QIcon passIcon;
    QIcon failIcon;

    bool setTheCustomerSound;
    bool setTheOperatorSound;
    bool isLinux();
    int howMany;

    QString newConnectionFlag;
    void saveNTP2DB();
    bool mergeNewDBEntries(QString key, QString value);
    QStringList timezoneHoursList;
    bool settingTime;
    bool processingAButton;
    bool needToReboot;
    bool USBKeyDialogIsVisible;
    SessionErrorDialog *dialog;
    QString initialSpeakerSetting;
    bool rebootingAlready;
    bool respondToTableViewClick;

    // Printer Tab variables
    QMap<int, QString> prDataNamesMap;
    QMap<QString, int> prDataIntsMap;
    QMap<QString, int> prDataPromptMapIndex;
    QMap<QString, QString> prDataNameToType;
    QMap<int, QString> prDataModelMap;
    QMap<int, QString> prDataPromptMap;
    bool printerDriverChanged;
    bool printerTabAlreadyLoaded;
    bool blockOnTranslate;

    //EAP-TLS additions
    QString getFileSelected(QString dialogBanner);
    QString getDirSelected(QString dialogBanner);
    QString serverCertFilename;
    QString chainFilename;
    QString clientCertFilename;
    QString CSRFilename;
    QString CSRKeyFilename;
    QString getSerialNumber();
    void copyCertsFiles(QString certToCopy);
    void clearCertificateInfoFromScaleSettings();
    QStringList getStoredCerts(QString type);
    void loadTrustedRootCertsCombo();
    void loadClientCertsCombo();
    void changeCertFilesToUse();
    void finishedNetworkProcess();
    void finishedEAPProcess();
    void deleteCert(QString certName, QString certType);
    void deleteActiveCert(QString certName);
    bool checkIfActiveCert(QString certName, QString certType);
    void clearWpaSupplicantConfAndDisconnect();
    void refreshNetwork_click();
    void importCertFiles(QString certToCopy, QString certType);
    void clearWindowsWlanConfandDisconnect();

    // Printer Tab Methods
    QString getScaleModel();
    void setupPrinterTab();
    void continueSetupPrinterTab();
    void updateUsbComboNames(const SessionMessage &msg);
    void updateNetworkComboNames(const SessionMessage &msg);
    void updatePrinterTab(const SessionMessage &msg);
    void updatePrinterTabData(const SessionMessage &msg);
    void updatePrinterCheckboxes(const SessionMessage &msg);
    void refreshPrinterTabData();
    void setEnablednessOfPrinterButtons(QPushButton *widget, bool isEnabled);
    void setCheckBoxVisibility();


    // Enum section
    enum PrinterDataNames
    {
        PrTypeUnknown,                      //psttUnknown
        PrExpandedText,                     //psttExpandedText,
        PrSpecialMessage,                   //psttSpecialMessage,
        PrProductNote,                      //psttProductNote,
        PrMarqueeText,                      //psttMarqueeText,
        PrCookingInstructions,              //psttProductText5
        PrRecipes,                          //psttProductText6  
        PrExpandedText2,                    //psttProductText7  
        PrSuggestiveSelling,                //psttProductText8  
        PrAllergens,                        //psttProductText9  
        PrNutrifacts        = 20            //psttNutrifacts
    };

    enum PrinterModel
    {
        NoPrinter,                          // 0
        EppPrinter,                         // not used, no parts for this EPP - EOL
        EssPrinter,                         // not used
        EscPrinter,                         // not used
        ApcPrinter,                         // not used
        Epp2Printer,                        // 5 This is the EPP 2nd printer will use
        UsbPrinter,                         // 6
        NetworkPrinter                      // 7
    };
};


#endif // SETUPUTILITY_H
