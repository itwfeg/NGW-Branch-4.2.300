#ifndef SYSTEMDIAGNOSTICS_H
#define SYSTEMDIAGNOSTICS_H
/**
 * @file mainwindow.h
 * @brief Widget to set system configuration of the scale.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @ported Jobb
  * @addtogroup Settings Wizard CSU
  * @{
*/

/*
PROLOGUE: Sytem Diagnostics
Date: Q4 2014
AUTHOR: Todd Davis
COMPANY: Hobart

PURPOSE: This application serves to provide a System settings diganostics wizard
         for the HT Series scales to configure the system hardware's settings for:

         - Time & Date
         - Sound Volume
         - Network Configuration
         - Touchscreen Calibration
*/

#include "screenwidget.h"
#include <QIcon>

#include "../platform/network.h"
#include <QDir>
// QT5 PORT
#include <QAudioOutput>
#include <QMediaObject>
#include <QMediaPlayer>

#include <videoplayerwidget.h>

#include "../platform/python.h"
#include "../dialogs/messageboxdialog.h"
#include "../dialogs/sessionerrordialog.h"

class QIntValidator;

namespace Ui {
class systemDiagnostics;
}

class systemDiagnostics : public ScreenWidget{
    Q_OBJECT

    typedef systemDiagnostics _Myt;
    typedef ScreenWidget _Mybase;



public:
    explicit systemDiagnostics(SessionMainWindow *mainWindow, ModeWidget *parent = 0);
    virtual ~systemDiagnostics();

    QString mode;
    static const QString ServicePassword;

    static const bool USE_BACKDOOR;
    static const QString USERNAME;
    static const QString PASSWORD;
    static const QString PASSPHRASE;
    static void sleep(long iSleepTime);

    bool cancelPing;
    bool advcancelPing;
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
    void advstartPingBut_click(bool);
    void addNetworkBut_click();
    void stopPingBut_click();
    //void advstopPingBut_click();
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
    void advprintPingOutput();
    void advprintMemoryOutput();
    void advprintTracerouteOutput();
    void readyReadStandardError();
    void finishedProcess(int value, QProcess::ExitStatus exitStatus);
    void advreadyReadStandardError();
    void advfinishedProcess(int value, QProcess::ExitStatus exitStatus);
    void advMemoryReadyReadStandardError();
    void advfinishedMemoryProcess(int value, QProcess::ExitStatus exitStatus);
    void advreadyReadStandardErrorTraceroute();
    void advfinishedProcessTraceroute(int value, QProcess::ExitStatus exitStatus);
    void printNetworkOutput();
    void readyReadNetworkStandardError();
    void finishedNetworkProcess(int value, QProcess::ExitStatus exitStatus);
    void internalSpeakerClicked(bool);
    void externalSpeakerClicked(bool);
    void signingRequestButClicked();
    void setWirelessFrames(int);
    void advReportsSaveAllBut_clicked();
    void memoryStatesBut_click();
    void memoryClearBut_click();

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
    void advNetworkingCloseBut_clicked();
    void advEtherApplyBut_clicked();
    void advEtherCancelBut_clicked();
    void advEtherDefaultsBut_clicked();
    void advEtherLinkRateCombo_changed(int);
    void advWifiStationInfoBut_clicked();
    void advWifiScanAPsBut_clicked();
    void advReportsNetworkConfigUSBBut_clicked();
    void advReportsNetworkAPInfoUSBBut_clicked();
    void advReportsNetworkScreenshotUSBBut_clicked();
    void toolsTracerouteBut_clicked();
    void toolsClearBut_clicked();
    void toolsScanWirelessAPsBut_clicked();
    void statusEthernetConnectionDetailsBut_clicked();
    void statusLinkStatesBut_clicked();
    void statusNICConfigurationsBut_clicked();
    void statusWirelessConnectionDetailsBut_clicked();
    void statusClearBut_clicked();
    void advWifiScanAPsBut_pressed();
    void toolsScanWirelessAPsBut_pressed();
    void reportsSaveMemoryStatesBut_click();
    void wirelessRefreshBut_pressed();
    void wirelessRefreshBut_clicked();
    void debugBut_clicked();
    void closeDebugBut_clicked();
    void wirelessTWCellPressed(int row, int column);
    void wirelessHeaderClicked(int);
    void toolsWirelessLoggingBut_toggled(bool);

private:
    Ui::systemDiagnostics *ui;
    bool updateDB(QString key, QString value);
    QProcess *pingProcess;
    QProcess *advpingProcess;
    QProcess *advTracerouteProcess;
    QProcess *memoryProcess;
    bool connectionEstablished;
    QProcess *saveNetworkConfigurationProcess;
    QString output;
    // QT5 PORT
    //Phonon::AudioOutput *audioOutput;
    //Phonon::MediaObject *mediaObject;
    QAudioOutput *audioOutput;
    QMediaPlayer *mediaObject;
    videoPlayer *player;                // actually think I am going to completely use this
    //QMediaObject *mediaObject;
    QStringList stereoTestFiles;
    int currentPlaybackIndex;
    void saveConfigsNow();

    bool tryingAHiddenSSID;
    bool isReadyToTranslate;

    QString dbKey;
    QString dbValue;
    bool adapterAlreadyConnected;
    QString m_correctText;

    bool shouldChangeTime;
    bool shouldChangeNTP;
    bool mustTurnOnDialogs;
    bool visDemoMode;
    
    python *networkPythonProcess;
    python *NTPTimezoneProcess;
    void execPythonCommand(QString command,
                           QStringList arguments=QStringList());

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
    QString dbWirelessLogging;
    bool firstLoad;

    int linkRateIndex;
    int channelIndex;
    bool automaticLinkRate;

    int etherLinkRateIndex;
    bool etherAutomaticLinkRate;

    QStringList APList;
    bool dbDisplayTab;
    bool dbCustomerSoundTab;
    bool dbOperatorSoundTab;
    bool dbTimeTab;
    bool dbNetworkTab;
    bool dbHostTab;
    int selectedConnectionRow;
    bool foundSelectedConnection;
    void loadDisplayBrightnessValues(QString display);
    void loadSoundVolumeValues(QString device);
    void changeTimeAfterServiceKeyRemoved();
    void endNetworkConnection();
    QString alertsText;

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
    void changeConnectedIcons(int rowToConnect, bool connectionOKAY);

   // void getConnectionStatus();
    void powerDevices(QString states);
    QString getDBValue(QString key);
    QStringList getWirelessAPChartInfo();


    bool finalIsWireless;

    bool networkTabAlreadyLoaded;
    void loadNetworkTab();
    void loadHostsTab();
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
    void deleteCert(QString certName, QString certType);
    void deleteActiveCert(QString certName);
    bool checkIfActiveCert(QString certName, QString certType);
    void clearWpaSupplicantConfAndDisconnect();
    void refreshNetwork_click();
    void importCertFiles(QString certToCopy, QString certType);

    int statusAnchorNumber;
    int toolsAnchorNumber;
    int memoryAnchorNumber;
    int col1Count;
    int col2Count;
    int col3Count;
    int col4Count;
    int col5Count;
    int col6Count;
    int col7Count;
    int col8Count;
    int col9Count;
    int col10Count;
    int col11Count;
    int col12Count;


};

#endif // SYSTEMDIAGNOSTICS_H
