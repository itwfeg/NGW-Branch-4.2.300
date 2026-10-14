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

#include <QtGui>
#include <QtCore>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <QTimer>
#include <UiUtilities.h>
#include <filesystem.h>

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

//#include <QWidget>
#include "setupUtility.h"
#include "ui_setupUtility.h"
#include "Devices.h"
#include "Events.h"
#include "Messages.h"
#include "../dialogs/exportnetworkdialog.h"
#include "../dialogs/filedialog.h"
#include "../dialogs/messageboxdialog.h"

#include <QCryptographicHash>

#include "../platform/platform.h"
#include "../platform/network.h"
#include "../platform/python.h"

#include <logmanager.h>
#include <ScaleConfiguration.h>

#include <QStringList>
#include <QDebug>

// Hobart
// HT-228
#include "../../source/HFC/CfgBaseFieldOffsets.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

const QString setupUtility::ServicePassword = "hobart45374";
const QString setupUtility::PASSPHRASE = "woozal";
const bool convertUppercase_ = true;
const bool TESTING = false;


extern bool isDemoMode;

//////////////////////////////////////////////////////
//
//        C O N S T R U C T O R
//
//////////////////////////////////////////////////////
/*class BackgroundDelegate : public QStyledItemDelegate
{
public:
    explicit BackgroundDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        // Fill the background before calling the base class paint
        // otherwise selected cells would have a white background
        QVariant background = index.data(Qt::BackgroundRole);
        if (background.canConvert<QBrush>())
            painter->fillRect(option.rect, background.value<QBrush>());

        QStyledItemDelegate::paint(painter, option, index);

        // To draw a border on selected cells

        if(option.state & QStyle::State_Selected)
        {
            painter->save();
            QPen pen(Qt::black, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
            int w = pen.width()/2;
            painter->setPen(pen);
            painter->drawRect(option.rect.adjusted(w,w,-w,-w));
            painter->restore();
        }

    }
};
*/

setupUtility::setupUtility(SessionMainWindow *mainWindow, ModeWidget *parent) :
  _Mybase( mainWindow, parent ),
  ui(new Ui::setupUtility),
  player( NULL ),
  writeAllExternalPrinter_(NULL),
  writeAllProdTxtPrintParams_(NULL),
  promptness_(NULL),
  extPrintSettings_(NULL),
  writeAllUsbDriverNames_(NULL),
  writeAllNetworkDriverNames_(NULL),
  configWriteSingleField(NULL)
{

     LOG_CLASS_INFO("setupUtility", "setupUtility");
     setObjectName( "setupUtility" );

  
     //masterTimer = new QElapsedTimer();
     //masterTimer->start();

    ///////////////////////////
    // SOME GENERAL INITIALIZING
    ///////////////////////////
    LOG_DEBUG( "\n\n\n\n\nSetup Utility Constructor", NULL );

    failIcon = QIcon(":/images/screens/fail.png");
    passIcon = QIcon(":/images/screens/pass.png");
    //qDebug() << "Before setupUI: " << masterTimer->elapsed();
    ui->setupUi(this);
    //scaleScreen();
    // Takes .988 ms
    //qDebug() << "After setupUI: " << masterTimer->elapsed();
    ui->junkTextEdit->hide();
    ui->USBGroupbox->setGeometry( scaleRectForScreen( QRect( 0, 0, 1024, 600 ) ) );
    ui->frameDeleteClientCertPrompt->setGeometry(0,0,917,474);
    ui->gbAdvancedNetworking->setGeometry(1,0,916,469);
    ui->gbCertInfo->setGeometry(1,1,916,466);
    ui->gbNetworkLoading->setGeometry(225,120,451,201);
    ui->USBGroupbox->hide();
    processingAButton = true;
    mustTurnOnDialogs = false;
    shouldChangeTime = false;
    respondToTableViewClick = true;
    tryingAHiddenSSID = false;
    networkTabAlreadyLoaded = false;
    printerTabAlreadyLoaded = false;
    blockOnTranslate = false;
    weHaveServiceMode = false;
    connectedSuccessfully = false;
    connectionTimeout = false;
    dbDisplayTab = false;
    dbCustomerSoundTab = false;
    dbOperatorSoundTab = false;
    dbTimeTab = false;
    dbNetworkTab = false;
    dbHostTab = false;
    dbPrinterTab = false;
    setTheCustomerSound = false;
    setTheOperatorSound = false;

    hostnameStayChecked_FIX = false;
    isDHCPCheckedBoxd_Fix = false;
    isHostNameChecked_Fix = false;
    isWiredConnection_Fix = false;

    newConnectionFlag = "";
    dbTimezone = "";
    dbTimeKeeper = "";
    dbNTPServer = "";
    dbNTPFrequency = "";
    dbNTPDayOfWeek = "";
    dbNTPTimeOfDay = "";
    dbNTPTimeZone = "";
    dbCSRFilename = "";
    dbCSRKeyFilename = "";
    dbDHCPHostname = "0";
    dbSNMPEnabled = "0";
    dbSNMPTrapsIP = "";
    dbSNMPTrapsPort = "162";
    dbSNMPVariablesPort = "161";
    settingTime = false;
    processingAButton = false;
    USBKeyDialogIsVisible = false;
    rebootingAlready = false;
    serverCertFilename = "";
    clientCertFilename = "";
    chainFilename = "";
    CSRFilename = "";
    CSRKeyFilename = "";
    ui->serverCertLabel->hide();
    ui->clientCertLabel->hide();
    ui->gbCertInfo->hide();
    ui->frameDeleteServerCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
    linkRateIndex = 0;
    automaticLinkRate = true;
    channelIndex = 0;
    DNSSuffix1Memory = "";
    DNSSuffix2Memory = "";
    DNSSuffix3Memory = "";
    DNSSuffix4Memory = "";
    DNSSuffix5Memory = "";
    DNSSuffix6Memory = "";
    linkRatewasAdded = false;
    linkRateAutomaticWasAdded = false;
    channelWasAdded = false;
    serverCertFilenameWasAdded = false;
    chainFilenameWasAdded = false;
    clientCertFilenameWasAdded = false;
    csrFilenameWasAdded = false;
    csrKeyFilenameWasAdded = false;
    clientKeyFilenameWasAdded = false;
    clientCertUsersFilenameWasAdded = false;
    clientCertPasswordWasAdded = false;
    isTimeTabFirstTime = true;
    isSoundTabFirstTime = true;
    DNSSuffix1WasAdded = false;
    DNSSuffix2WasAdded = false;
    DNSSuffix3WasAdded = false;
    DNSSuffix4WasAdded = false;
    DNSSuffix5WasAdded = false;
    DNSSuffix6WasAdded = false;
    DHCPHostnameWasAdded = false;
    SNMPVariablesPortWasAdded = false;
    SNMPTrapsPortWasAdded = false;
    SNMPTrapsIPWasAdded = false;

    SNMPVariablesPort = "161";
    SNMPTrapsPort = "162";
    SNMPTrapsIP = "";
    SNMPEnabled = "0";


    //ui->gbDeleteClientCertPrompt->hide();
    //ui->gbDeleteServerCertPrompt->hide();
    this->manageInstallationDate();
    this->setWirelessNetworkEncryptionCombo(0);
    this->ui->networkLoadingStatusLabel->hide();
    this->ui->gbNetworkLoading->hide();
    this->ui->gbAdvancedNetworking->hide();
    //ui->muteBut->hide(); //justin wanted the button removed late in the game, after user's manual has a screenshot of it.
    this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
    //ui->alertsLineEdit->setText(tr("Please wait...retrieving settings"));
    hostnamePromptText = tr("   You have entered an invalid Host Computer Name.\n") +
            tr("The following rules apply to Host Computer Name entry:\n") +
            tr("  - Name must begin with an alphabet character.\n") +
            tr("  - Name cannot end with a hyphen or decimal point.\n") +
            tr("  - Name can be no longer than 15 characters.\n") +
            tr("  - Name cannot contain the following characters:\n") +
               "           `~!@#$^&*()=+[]{}\\|;:'\",<>/?";

    //this->update();
    //qApp->processEvents();
    //ui->networkStatusLabel->hide();

    //connect(&this->FutureWatcher, SIGNAL(finished()), this, SLOT(progressBarFinished()));

    isDemoMode = false;
    cancelPing = true;
    this->ui->alertsLineEdit->setAlignment(Qt::AlignCenter);
    this->connect(this->ui->mainTab,SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

    ///////////////////////////
    // GET DB SETTINGS
    ///////////////////////////

    connectDB();
    bool redo = false;
 // set tabwidget signal

    if(mergeNewDBEntries("timeKeeper","Manual")) redo = true;
    if(mergeNewDBEntries("ntpServer", "0.0.0.0")) redo = true;
    if(mergeNewDBEntries("ntpFrequency","0")) redo = true;
    if(mergeNewDBEntries("ntpDayOfWeek","0")) redo = true;
    if(mergeNewDBEntries("ntpTimeOfDay","03:30:00")) redo = true;
    if(mergeNewDBEntries("ntpTimeZone","America/New_York")) redo = true;
    if(mergeNewDBEntries("ntpAntiMeridian","AM")) redo = true;
    if(mergeNewDBEntries("wpaPassphrase","")) redo = true;
    if(mergeNewDBEntries("eapUsername","")) redo = true;
    if(mergeNewDBEntries("eapPassword","")) redo = true;
    if(mergeNewDBEntries("speakerOutput", "0")) redo = true;
    if(mergeNewDBEntries("serverCertFilename", ""))
    {
        serverCertFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("chainFilename", ""))
    {
        chainFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("clientCertFilename", ""))
    {
        clientCertFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("csrFilename", ""))
    {
        clientCertFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("csrKeyFilename", ""))
    {
        csrKeyFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("clientKeyFilename", ""))
    {
        clientKeyFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("clientCertUsersFilename", ""))
    {
        clientCertUsersFilenameWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("clientCertPassword", ""))
    {
        clientCertPasswordWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("linkRate", "auto"))
    {
        linkRatewasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("automaticLinkRate", "1"))
    {
        linkRateAutomaticWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("channel","auto"))
    {
        channelWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix1",""))
    {
        DNSSuffix1WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix2",""))
    {
        DNSSuffix2WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix3",""))
    {
        DNSSuffix3WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix4",""))
    {
        DNSSuffix4WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix5",""))
    {
        DNSSuffix5WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DNSSuffix6",""))
    {
        DNSSuffix6WasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("DHCPHostname","0"))
    {
        DHCPHostnameWasAdded = true;
        redo = true;
    }

    if(mergeNewDBEntries("snmpVariablesPort","161"))
    {
        SNMPVariablesPortWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("snmpTrapsIP",""))
    {
        SNMPTrapsIPWasAdded = true;
        redo = true;
    }
    if(mergeNewDBEntries("snmpTrapsPort","162"))
    {
        SNMPTrapsPortWasAdded = true;
        redo = true;
    }

    if(mergeNewDBEntries("snmpEnabled","0"))
    {
        SNMPEnabledWasAdded = true;
        redo = true;
    }

    if(redo) connectDB();

    if(dbDisplayTab) ui->mainTab->setTabIcon(0, passIcon);
    if( dbOperatorSoundTab && dbCustomerSoundTab) ui->mainTab->setTabIcon(1, passIcon);
    if(dbTimeTab) ui->mainTab->setTabIcon(2, passIcon);
    if(dbNetworkTab) ui->mainTab->setTabIcon(3, passIcon);
    if(dbHostTab) ui->mainTab->setTabIcon(4, passIcon);
    if(dbPrinterTab) ui->mainTab->setTabIcon(5, passIcon);

    // now set icons of completed tabs
    //if()

    dbConnectionType == 0 ? finalIsWireless = false : finalIsWireless = true;

    //////////////////////////
    // GET PYTHON & PING PROCESSES READY
    ///////////////////////////

    pingProcess = new QProcess(this);
    saveNetworkConfigurationProcess = new QProcess(this);
    saveEAPConfigurationProcess = new QProcess(this);

    networkProcessConnected = false;
    eapProcessConnected = false;
    ///////////////////////////
    // APP QUITER FOR DEBUGGIN'
    // Just hit the Quit button
    ///////////////////////////

    if(!isDemoMode)
    {
        this->ui->quitBut->hide();
    }

    ///////////////////////////
    // SET DISPLAY TAB
    ///////////////////////////

    firstTimeCustomerBrightnessDisplay = true;
    this->loadDisplayBrightnessValues("o");
    this->setBrightnessIndicators(this->ui->brightnessSpinBox->value());
    this->ui->operatorScreenBut->setIcon(QIcon(":/images/screens/operatorDisplay.png"));
    this->ui->operatorScreenBut->setIconSize(QSize(91,54));

    if (!session_->isPrepack())
    {
        this->ui->customerScreenBut->setIcon(QIcon(":/images/screens/customer.png"));
        this->ui->customerScreenBut->setIconSize(QSize(86,54));
    }
                                                                                                              //background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(170, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));
    this->ui->operatorScreenBut->setStyleSheet("QPushButton#operatorScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(170, 255, 0, 255), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    if (!session_->isPrepack())
        this->ui->customerScreenBut->setStyleSheet("QPushButton#customerScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(170, 255, 0, 255), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");

    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    if (!session_->isPrepack())
        this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    this->ui->gcdSlider->setValue(dbGCD.toInt());
    this->gcdSliderChanged(dbGCD.toInt());

    this->connect(this->ui->gcdSlider, SIGNAL(valueChanged(int)), this, SLOT(gcdSliderChanged(int)));
    this->connect(this->ui->operatorScreenBut, SIGNAL(clicked()),this, SLOT(changedOperatorDisplayBrightnessButtons()));
    if (!session_->isPrepack())
        this->connect(this->ui->customerScreenBut, SIGNAL(clicked()),this, SLOT(changedCustomerDisplayBrightnessButtons()));
    this->connect(this->ui->calibrateScreenBut, SIGNAL(clicked()), this, SLOT(calibrateScreenBut_click()));
    this->connect(this->ui->setBrightnessBut, SIGNAL(clicked()), this, SLOT(setBrightnessBut_click()));
    this->connect(this->ui->brightnessSpinBox,SIGNAL(valueChanged(int)), this, SLOT(brightnessSpinBoxValueChanged(int)));

    if (session_->isPrepack())
    {
        //  HA-72: Hide all Customer Display related items
        ui->customerDisplayButText->hide();
        ui->customerDisplayIndicator->hide();
        ui->customerScreenBut->hide();

        ui->operatorScreenBut->setChecked(true);
        ui->customerScreenBut->setChecked(false);
    }

    ///////////////////////////
    // SET SOUND TAB
    ///////////////////////////

    stereoTestFiles.append("leftChannel.aac");
    stereoTestFiles.append("rightChannel.aac");
    stereoTestFiles.append("bothChannels.aac");
    firstTimeCustomerVolume = true;

    QFont soundPercentageFont = ui->volumeLabel->font();

    //set reboot flag for speaker change

    if("EN.US" == mainWindow_->language()) // English
    {
        soundPercentageFont.setPointSize(58);
    }
    else soundPercentageFont.setPointSize(25);
    ui->volumeLabel->setFont(soundPercentageFont);

    this->ui->butInternalSpeaker->setStyleSheet("QPushButton#butInternalSpeaker:checked {color: rgb(0,0,0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    this->ui->butExternalUSBSpeaker->setStyleSheet("QPushButton#butExternalUSBSpeaker:checked {color: rgb(0, 0, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");

    // select the correct speaker button
    if(dbSpeakerOutput == "0")
    {
        ui->butExternalUSBSpeaker->setChecked(false);
        ui->butInternalSpeaker->setChecked(true);
    }
    else if(dbSpeakerOutput == "1")
    {
        ui->butInternalSpeaker->setChecked(false);
        ui->butExternalUSBSpeaker->setChecked(true);
    }

    initialSpeakerSetting = dbSpeakerOutput;

    connect(this->ui->butInternalSpeaker, SIGNAL(clicked(bool)), this, SLOT(internalSpeakerClicked(bool)));
    connect(this->ui->butExternalUSBSpeaker, SIGNAL(clicked(bool)), this, SLOT(externalSpeakerClicked(bool)));
    changedOperatorSoundButtons();
    //this->loadSoundVolumeValues("o");
    //this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    this->ui->operatorSoundBut->setStyleSheet("QPushButton#operatorSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");

    // HTA-62 Add the speaker volume back in and change label to "Speakers"
    //if (!session_->isPrepack())
        this->ui->customerSoundBut->setStyleSheet("QPushButton#customerSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");

#if 1
    this->loadSoundVolumeValues("o");
    this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    this->ui->operatorSoundBut->setStyleSheet("QPushButton#operatorSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    this->ui->customerSoundBut->setStyleSheet("QPushButton#customerSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
#else
// HTA-62 ?    this->loadSoundVolumeValues("o");
// HTA-62 ?    this->setVolumeIndicators(this->ui->volumeSpinBox->value());
// HTA-62 ?    this->ui->operatorSoundBut->setStyleSheet("QPushButton#operatorSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
// HTA-62 ?    this->ui->customerSoundBut->setStyleSheet("QPushButton#customerSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
#endif

    player = videoPlayer::create( videoPlayerDevice::Operator, this );
    player->setVisible(false);
    connect( player, SIGNAL( finished() ), SLOT( playAudioFile() ) );

    this->connect(this->ui->operatorSoundBut, SIGNAL(clicked()),this, SLOT(changedOperatorSoundButtons()));
    // HTA-62
    //if (!session_->isPrepack())
        this->connect(this->ui->customerSoundBut, SIGNAL(clicked()),this, SLOT(changedCustomerSoundButtons()));
    this->connect(this->ui->setVolumeBut, SIGNAL(clicked()), this, SLOT(setVolumeBut_click()));
    this->connect(this->ui->testVolumeBut, SIGNAL(clicked()), this, SLOT(testVolumeBut_click()));
    this->connect(this->ui->muteBut, SIGNAL(toggled(bool)),this, SLOT(muteButChanged(bool)));
    this->connect(this->ui->volumeIndicator0, SIGNAL(clicked()), this, SLOT(volumeIndicator0_click()));
    this->connect(this->ui->volumeIndicator1, SIGNAL(clicked()), this, SLOT(volumeIndicator1_click()));
    this->connect(this->ui->volumeIndicator2, SIGNAL(clicked()), this, SLOT(volumeIndicator2_click()));
    this->connect(this->ui->volumeIndicator3, SIGNAL(clicked()), this, SLOT(volumeIndicator3_click()));
    this->connect(this->ui->volumeIndicator4, SIGNAL(clicked()), this, SLOT(volumeIndicator4_click()));
    this->connect(this->ui->volumeIndicator5, SIGNAL(clicked()), this, SLOT(volumeIndicator5_click()));
    this->connect(this->ui->volumeIndicator6, SIGNAL(clicked()), this, SLOT(volumeIndicator6_click()));
    this->connect(this->ui->volumeIndicator7, SIGNAL(clicked()), this, SLOT(volumeIndicator7_click()));
    this->connect(this->ui->volumeIndicator8, SIGNAL(clicked()), this, SLOT(volumeIndicator8_click()));
    this->connect(this->ui->volumeIndicator9, SIGNAL(clicked()), this, SLOT(volumeIndicator9_click()));
    this->connect(this->ui->volumeIndicator10, SIGNAL(clicked()), this, SLOT(volumeIndicator10_click()));
    this->connect(this->ui->volumeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(volumeSpinBoxValueChanged(int)));

    // HTA-62
    /*if (session_->isPrepack())
    {
        //  HA-72: Hide all Customer Display related items
        ui->customerSoundButText->hide();
        ui->customerSoundIndicator->hide();
        ui->customerSoundBut->hide();

        ui->operatorSoundBut->setChecked(true);
        ui->customerSoundBut->setChecked(false);
//    }
    */

    ////////////////////////////
    // SET TIME TAB
    ////////////////////////////

    session_->send(SessionDev,SnReadDateTime);

    pickedDate = new QDate();
    this->changedAstronomical = false;
    this->changedTimezone = false;
    //qApp->processEvents();
    this->fillTimeZones();

    //Set the main system time setting at the top
    //sd

    this->ui->timeZoneCombo->setCurrentIndex(ui->timeZoneCombo->findData(dbTimezone));
    this->setTimeTab();
    this->update();
    originalTimeZoneComboBoxIndex = ui->timeZoneCombo->currentIndex();
    originalNTPTimeZoneComboBoxIndex = ui->ntpTimeZoneCombo->currentIndex();
    //connect(ui->keepAliveBut, SIGNAL(clicked), this, SLOT(keepAppAlive()));
    //connect(ui->keepAliveManual, SIGNAL(clicked), this, SLOT(keepAppAlive()));

    // For ADP team - handles all of the time settings.
    this->connect(this->ui->setDateTimeBut, SIGNAL(clicked()), this, SLOT(setDateTimeBut_click()));
    this->connect(this->ui->dateEdit, SIGNAL(clicked(QDate)), this, SLOT(datePicked(QDate)));
    this->connect(this->ui->dateEdit, SIGNAL(currentPageChanged(int,int)),this,SLOT(setTheDayOfMonth(int,int)));
   // this->connect(this->ui->CurrentDateTimeGroupBox, SIGNAL(clicked()), this, SLOT(revertToOriginalDate()));

    this->connect(this->ui->astronomicalCheckBox, SIGNAL(stateChanged(int)), this, SLOT(astronomicalCheckboxValueChanged(int)));

    // MANUAL TIME
    this->connect(this->ui->hourBut, SIGNAL(clicked()), this, SLOT(hourBut_clicked()));
    this->connect(this->ui->minuteBut, SIGNAL(clicked()), this, SLOT(minuteBut_clicked()));
    this->connect(this->ui->secondBut, SIGNAL(clicked()), this, SLOT(secondBut_clicked()));
    this->connect(this->ui->ampmBut, SIGNAL(clicked()), this, SLOT(ampmBut_clicked()));
    this->connect(this->ui->timeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(timeEditTimeChanged(QTime)));

    // NTP TIME
    this->connect(this->ui->ntpHourBut, SIGNAL(clicked()), this, SLOT(ntpHourBut_clicked()));
    this->connect(this->ui->ntpMinuteBut, SIGNAL(clicked()), this, SLOT(ntpMinuteBut_clicked()));
    this->connect(this->ui->ntpSecondBut, SIGNAL(clicked()), this, SLOT(ntpSecondBut_clicked()));
    this->connect(this->ui->ntpAmpmBut, SIGNAL(clicked()), this, SLOT(ntpAmpmBut_clicked()));
    this->connect(this->ui->ntpTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(ntpTimeEditTimeChanged(QTime)));
    this->connect(this->ui->setNtpBut, SIGNAL(clicked()), this, SLOT(setNtpDateTimeBut_click()));
    this->connect(this->ui->ntpFrequencyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ntpFrequencyChanged(int)));

    //ui->setDateTimeBut->installEventFilter(this);
    ui->dateEdit->installEventFilter(this);
    ui->astronomicalCheckBox->installEventFilter(this);
    ui->timeZoneCombo->installEventFilter(this);

    // this->connect(this->ui->timeEdit, SIGNAL()

    //this->ui->timeEdit->installEventFilter(this);
    foreach(QObject *child, ui->dateEdit->children())
    {
        if (child->isWidgetType())
        {
            child->installEventFilter(this);
        }
    }

    //setTimeTab();



    ////////////////////////////
    // SET SYSTEM TAB
    ////////////////////////////

    this->changedHostname = false;
    //this->controlNICs("both", "up");
    /* this->getUIMACAddress();
    this->getUIHostname();

    QRegExp validHostnameRegEx("^([a-zA-Z]+[a-zA-Z0-9\\-\\.]*[a-zA-Z0-9]+)|([a-zA-Z])$");
    QValidator* hostnameValidator;
    hostnameValidator = new QRegExpValidator(validHostnameRegEx, this);
    ui->hostnameLineEdit->setValidator(hostnameValidator);
    //QStringList specialCharArray = { '`', '~', '!', '@', '#', '$', '^', '&', '*', '(', ')', '=', '+', '[', ']', '{', '}', '\\', '|', ';', ':', '\'', '"', ',', '<', '>', '/', '?' };

    this->connect(this->ui->hostnameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAgainstPoorServiceTraining(QString)));
    //this->ui->pingIPLineEdit->setText("www.hobartcorp.com");
    this->connect(this->ui->setHostnameBut, SIGNAL(clicked()), this, SLOT(setHostnameBut_click()));
    this->connect(this->ui->startPingBut, SIGNAL(clicked()), this, SLOT(startPingBut_click()));
    this->connect(this->ui->stopPingBut, SIGNAL(clicked()), this, SLOT(stopPingBut_click()));
    this->connect(pingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printPingOutput()));
    this->connect(pingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int,QProcess::ExitStatus)));
    this->connect(pingProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
*/

    ///////////////////////////
    // SET NETWORK TAB
    ///////////////////////////

    if(mainWindow->isServiceMode())
        weHaveServiceMode = true;
    else
        weHaveServiceMode = false;

    // Do not display the Printer tab for Pre-Pack
    if( session_->isPrepack() )
        ui->mainTab->removeTab( ui->mainTab->indexOf( ui->printerTab ) );

    if(mainWindow->isSupervisorMode())
    {
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

        if (!settings->rebootUnprotectedAllowed() )
        {
            ui->mainTab->removeTab( ui->mainTab->indexOf( ui->snmpTab ) );

            ui->mainTab->removeTab( ui->mainTab->indexOf( ui->hostTab ) );
            ui->mainTab->removeTab( ui->mainTab->indexOf( ui->networkConfigurationTab ) );

            // Remove NTP tab on Date/Time tab
            ui->timeTabWidget->removeTab( ui->timeTabWidget->indexOf( ui->timeNtpSubTab ) );
            ui->timeTabWidget->setCurrentIndex( 0 );

            // set a stylesheet to increase width
            //ui->timeTabWidget->setStyleSheet("QTabWidget#timeTabWidget{ background:  transparent; }  QTabWidget#timeTabWidget::pane{ border-top: 1px solid rgb(180,180,180);  border: 1px solid rgb(139,209,0);  background:  url(images/carbonFiberBackground.png); margin-bottom:0px;  margin-top: -11px; }  QTabWidget#timeTabWidget::tab-bar{ } QTabBar::tab{  border-color: rgb(139,209,0); border-bottom-color: rgb(139,209,0); height: 41px; min-width: 170px; margin-top: 2px; background: rgb(227, 227, 227); padding-left: 10px; } QTabWidget#timeTabWidget::tab-bar::tab:selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); color:rgb(139,209,0); margin-bottom:0px; margin-top: 2px; border-bottom-color: rgb(139,209,0); height:41px; } QTabWidget#timeTabWidget::tab-bar::tab:!selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); color:rgb(255,255,255); height: 41px; margin-top: 2px;  margin-bottom:0px; } QTabBar::tab:first:selected{ min-width: 353px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-left: 0px; height: 41px; margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));} QTabBar::tab:first:!selected{  min-width: 353px;  border-top-left-radius: 8px; border-top-right-radius: 8px;  margin-left: 0px;  height: 41px; margin-top: 2px; margin-bottom:0px;  border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));}  QTabBar::tab:last:selected{ margin-right: 0px; min-width: 170px; border-top-left-radius: 8px;  border-top-right-radius: 8px; height: 41px;  margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));} QTabBar::tab:last:!selected{  margin-right: 0px; min-width: 170px; border-top-left-radius: 8px; border-top-right-radius: 8px;  height: 41px; margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));}");

            ui->gcdLabel->hide();
            ui->gcdSlider->setDisabled(true);
            ui->gcdSlider->setEnabled(false);
            ui->gcdSlider->hide();
            ui->gcdNoLabel->hide();
            ui->gcdYesLabel->hide();
        }
        else
        {
            this->ui->DHCPHostnameWarning->hide();
            this->loadHostsTab();
            this->loadSNMPTab();
            ui->gcdLabel->hide();
            ui->gcdSlider->setDisabled(true);
            ui->gcdSlider->setEnabled(false);
            ui->gcdSlider->hide();
            ui->gcdNoLabel->hide();
            ui->gcdYesLabel->hide();
            //networkTabAlreadyLoaded = false;
        }
    }
    else if(mainWindow->isServiceMode())
    {
        this->loadHostsTab();
        this->loadSNMPTab();
        ui->gcdLabel->hide();
        ui->gcdSlider->setDisabled(true);
        ui->gcdSlider->setEnabled(false);
        ui->gcdSlider->hide();
        ui->gcdNoLabel->hide();
        ui->gcdYesLabel->hide();
    }
    else // reassure gcd slider is gone
    {
        ui->gcdLabel->hide();
        ui->gcdSlider->setDisabled(true);
        ui->gcdSlider->setEnabled(false);
        ui->gcdSlider->hide();
        ui->gcdNoLabel->hide();
        ui->gcdYesLabel->hide();
    }

    scaleScreen();

    //set the tab spacing for the Main tab widget and tab bar
    expandTabs(ui->mainTab);

    //special exception for the time tab because why not put an info box inside a tab, amirite?
    int tabCount = ui->timeTabWidget->count();
    int currentDateTimeWidth = ui->CurrentDateTimeGroupBox->width();
    QString timeTabWidthStyleSheet = "";


    if(tabCount == 2){ //Only showing Manual + Date/Time tabs
        QWidget *invisible = new QWidget();
        ui->timeTabWidget->addTab(invisible, "");
        ui->timeTabWidget->setTabEnabled(2,false);
        invisible->setEnabled(false);
        invisible->setDisabled(true);
    }

    timeTabWidthStyleSheet = QString("QTabBar::tab{ min-width: %1px;} QTabWidget#timeTabWidget::tab-bar{ height: %2px; } ").arg(currentDateTimeWidth).arg(scaleHeightForScreen(41));

    ui->timeTabWidget->setStyleSheet(ui->timeTabWidget->styleSheet().append(timeTabWidthStyleSheet));

    // The buttons/images/sizing are all done with stylesheets ( >_> ) so lets make them scale dynamically
    // This should work regardless of screen size
    // spinBox
    // comboBox
    int spinBoxHeight = ui->brightnessSpinBox->contentsRect().height();
    int spinBoxWidth =  ui->brightnessSpinBox->contentsRect().width();

    QString spinBoxDimensions = QString("QSpinBox{ min-width: %1px; height: %2px}").arg(spinBoxWidth).arg(spinBoxHeight);
    QString spinBoxSubdivisions = QString(" QSpinBox::up-button {min-width: %1px; height: %2px}  QSpinBox::down-button {min-width: %1px; height: %2px}").arg(spinBoxWidth).arg(spinBoxHeight/2);
    ui->brightnessSpinBox->setStyleSheet(ui->brightnessSpinBox->styleSheet().append(spinBoxDimensions + spinBoxSubdivisions));
    ui->volumeSpinBox->setStyleSheet(ui->volumeSpinBox->styleSheet().append(spinBoxDimensions + spinBoxSubdivisions));

    //quick fix for the time edit scaling on all resolutions. So few of them I couldn't justify making a
    //screenwidget function for it and having to import QTimeEdit
    int timeEditHeight = ui->timeEdit->contentsRect().height();
    QString timeEditDimensions = QString("QTimeEdit::up-button { min-width: %1px; height: %1px }  QTimeEdit::down-button {min-width: %1px; height: %1px }").arg(timeEditHeight/2);
    ui->timeEdit->setStyleSheet(ui->timeEdit->styleSheet().append(timeEditDimensions));

    timeEditHeight = ui->ntpTimeEdit->contentsRect().height();
    timeEditDimensions = QString("QTimeEdit::up-button{ min-width: %1px; height: %1px } QTimeEdit::down-button { min-width: %1px; height: %1px }").arg(timeEditHeight/2);
    ui->ntpTimeEdit->setStyleSheet(ui->ntpTimeEdit->styleSheet().append(timeEditDimensions));

    fixComboBoxStyleSheet(ui->timeZoneCombo);
    fixComboBoxStyleSheet(ui->cboPrinterData0);
    fixComboBoxStyleSheet(ui->cboPrinterData1);
    fixComboBoxStyleSheet(ui->cboPrinterData2);
    fixComboBoxStyleSheet(ui->cboPrinterData3);
    fixComboBoxStyleSheet(ui->cboPrinterData4);
    fixComboBoxStyleSheet(ui->cboPrinterDriverOne);
    fixComboBoxStyleSheet(ui->cboPrinterDriverTwo);
    fixComboBoxStyleSheet(ui->cboPrinterTypeOne);
    fixComboBoxStyleSheet(ui->cboPrinterTypeTwo);
    fixComboBoxStyleSheet(ui->ntpFrequencyCombo);
    fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    fixComboBoxStyleSheet(ui->ntpTimeZoneCombo);
    fixComboBoxStyleSheet(ui->encryptionCombo);
    fixComboBoxStyleSheet(ui->securityCombo);
    fixComboBoxStyleSheet(ui->clientCertStoreCombo);
    fixComboBoxStyleSheet(ui->trustedRootStoreCombo);

    //CalendarEdit needed an extra nudge
    QSize calendarSize = scaleSizeForScreen(QSize(441, 341));
    ui->dateEdit->resize(calendarSize);
    ui->dateEdit->setStyleSheet(ui->dateEdit->styleSheet().append(QString("QSpinBox{ min-width: %1px; ").arg(100)));


    ///////////////////////////
    // SET PRINTER TAB
    ///////////////////////////

    if ( mainWindow_->isServiceMode() )
    {
        promptness_ = &dynamic_cast<ServiceMode*>( mode_ )->promptness_;
        extPrintSettings_ = &dynamic_cast<ServiceMode*>( mode_ )->extPrintSettings_;
    }

    if ( mainWindow_->isSupervisorMode() )
    {
        promptness_ = &dynamic_cast<SupervisorMode*>( mode_ )->promptness_;
        extPrintSettings_ = &dynamic_cast<SupervisorMode*>( mode_ )->extPrintSettings_;
    }

    ui->boxPrinterDataChrome->setVisible(false);

    if ( !(printerTabAlreadyLoaded) )
    {
        this->connect(this->ui->txtPrinterNameOne,          SIGNAL(editingFinished()),        this, SLOT(txtPrinterSettingsChanged()));
        this->connect(this->ui->txtPrinterNameTwo,          SIGNAL(editingFinished()),        this, SLOT(txtPrinterSettingsChanged()));
        this->connect(this->ui->txtPrinterIpAddressOne,     SIGNAL(editingFinished()),        this, SLOT(txtPrinterIpSettingsChanged()));
        this->connect(this->ui->txtPrinterIpAddressTwo,     SIGNAL(editingFinished()),        this, SLOT(txtPrinterIpSettingsChanged()));
        this->connect(this->ui->cboPrinterDriverOne,        SIGNAL(currentIndexChanged(int)), this, SLOT(cboPrinterDriverChanged(int)));
        this->connect(this->ui->cboPrinterDriverTwo,        SIGNAL(currentIndexChanged(int)), this, SLOT(cboPrinterDriverChanged(int)));

        this->connect(this->ui->cboPrinterTypeOne,          SIGNAL(currentIndexChanged(int)), this, SLOT(printerTypeChangedOne(int)));
        this->connect(this->ui->cboPrinterTypeTwo,          SIGNAL(currentIndexChanged(int)), this, SLOT(printerTypeChangedTwo(int)));
        this->connect(this->ui->butPrinterTestOne,          SIGNAL(clicked()),                this, SLOT(testExternalPrinterOne()));
        this->connect(this->ui->butPrinterTestTwo,          SIGNAL(clicked()),                this, SLOT(testExternalPrinterTwo()));
        this->connect(this->ui->butSavePrinterData,         SIGNAL(clicked()),                this, SLOT(saveExternalPrinterSettings()));
        this->connect(this->ui->butAssignPrinterData,       SIGNAL(clicked()),                this, SLOT(gotoPrinterDataScreen()));
        this->connect(this->ui->butReturnToPrinterSettings, SIGNAL(clicked()),                this, SLOT(gotoPrinterSettings()));

        this->connect(this->ui->chkPrintPluLabelOne,        SIGNAL(stateChanged(int)),        this, SLOT(chkPrinterSettingsOneChanged(int)));
        this->connect(this->ui->chkPrintPluLabelTwo,        SIGNAL(stateChanged(int)),        this, SLOT(chkPrinterSettingsTwoChanged(int)));

    }

    //////////////////////////
    // FINAL STUFF
    //////////////////////////

    isReadyToTranslate = true;
    this->ui->mainTab->setCurrentIndex(0);
    //dialog = new MessageBoxDialog(this );

    changedSpeakers = false;

    // HW-25: for windows hide speaker configuration
#ifdef Q_OS_WIN
    ui->label_57->hide();
    ui->butInternalSpeaker->hide();
    ui->butExternalUSBSpeaker->hide();
#endif
    this->setDateTimeText();
    processingAButton = false;
    //ui->alertsLineEdit->setText(dbClientCertUsersFilename);

    //Fix GUI operator volume text
    /*QString oVolume = getDBValue("oVolume");
    //ui->testVolumeBut->hide();
    //ui->volumeLabel->setText(oVolume);
    ui->lblPercentSymbol->hide();
    //ui->alertsLineEdit->setText(oVolume);
    //Check operator volume and reset it
    if(oVolume == "on")
    {
        ui->muteBut->setChecked(true);
        ui->volumeSpinBox->setValue(1);
        ui->volumeLabel->setText(tr("on"));
        operatorVolume = "on";
    }
    else
    {
        ui->muteBut->setChecked(false);
        ui->volumeSpinBox->setValue(0);
        ui->volumeLabel->setText(tr("off"));
        operatorVolume = "off";
    }*/
//    ui->operatorSoundBut->setChecked(true);

    session_->send(SessionDev,SnReadDateTime);
    this->update();
    translate();

        this->loadSoundVolumeValues("o");

    //ui->alertsLineEdit->setText(dbNTPDayOfWeek + ":" + dbNTPFrequency + ":" + dbNTPAntiMeridian + ":xxx" + dbNTPTimeOfDay );
    if (dbNTPFrequency.toInt() > 0)
    {
        ui->ntpDayOfWeekCombo->setDisabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        ui->ntpDayOfWeekCombo->setCurrentIndex(-1);
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setCurrentIndex(dbNTPDayOfWeek.toInt());
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px;border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
    ui->ntpFrequencyCombo->setCurrentIndex(dbNTPFrequency.toInt());

    // only approx .2 more seconds to load rest of screen after setupUI()
    //qDebug() << "Final Master Timer: " << masterTimer->elapsed();
}

void setupUtility::setTheDayOfMonth(int newYear, int newMonth)
{
    int day = 1;
    //int month = 1;

    QDate newDate;
    // Same so set it back
    if(newYear == currentUserDate.year() && newMonth == currentUserDate.month())
    {
        day = tempDayOfMonth;
    }

    // year and month is changed so set the day

    else if(tempDayOfMonth == 29)
    {
        if(newMonth == 2 && ((newYear - 2012)%4 == 0))
        {
            day = 29;
        }
        else if(newMonth == 2)
        {
            day = 28;
        }
        else
        {
            day = 29;
        }
    }
    else if(tempDayOfMonth == 30 && newMonth == 2)
    {
        if((newYear - 2012)%4 == 0)
        {
            day = 29;
        }
        else
        {
            day = 28;
        }
    }
    else if(tempDayOfMonth == 31)
    {
        if(newMonth == 2)
        {
            if((newYear - 2012)%4 == 0)
            {
                day = 29;
            }
            else
            {
                day = 28;
            }
        }
        else if(newMonth == 4 || newMonth == 6 || newMonth == 9 || newMonth == 11)
        {
            day = 30;
        }
        else
        {
            day = 31;
        }
    }
    else
    {
        day = tempDayOfMonth;
    }

    newDate.setDate(newYear,newMonth,day);
    ui->dateEdit->setSelectedDate(newDate);
    tempDayOfMonth = day;
}

void setupUtility::internalSpeakerClicked(bool)
{
    LOG_CLASS_INFO( "setupUtility", "internalSpeakerClicked" );
    LOG_DEBUG( "internalSpeakerClicked()", NULL );

  //  ui->butExternalUSBSpeaker->setChecked(false);
    // change external stylesheet
   // this->ui->butExternalUSBSpeaker->setStyleSheet("color: rgb(0,0,0); background-color: rgb(241,241,241);");
  //  this->ui->butInternalSpeaker->setStyleSheet("color: rgb(0, 0, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);");

    // immediately change the speaker output source to internal through python ;
    // call python directly
    //QProcess
    //processingAButton = true;
    python pythonProcess;

    QStringList options;
    options.clear();
    options.append("internal");
    if (pythonProcess.execCommand("setAudioDevice", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            updateDB("speakerOutput", "0");
            dbSpeakerOutput = "0";
            if(initialSpeakerSetting == "0")
            {
                changedSpeakers = false;
                this->ui->alertsLineEdit->setText(tr("Internal speakers set."));
            }
            else if(initialSpeakerSetting == "1")
            {
                changedSpeakers = true;
                this->ui->alertsLineEdit->setText(tr("Internal speakers set. System restart required."));
            }
        }
        else
        {
            ;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }

   // processingAButton = false;
}

void setupUtility::externalSpeakerClicked(bool)
{
    LOG_CLASS_INFO( "setupUtility", "externalSpeakerClicked" );
    LOG_DEBUG( "externalSpeakerClicked()", NULL );

    python pythonProcess;

    QStringList options;
    options.clear();
    options.append("external");
    if (pythonProcess.execCommand("setAudioDevice", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            updateDB("speakerOutput","1");
            dbSpeakerOutput = "1";
            if(initialSpeakerSetting == "1")
            {
                changedSpeakers = false;
                this->ui->alertsLineEdit->setText(tr("USB speakers set."));
            }
            else if(initialSpeakerSetting == "0")
            {
                changedSpeakers = true;
                this->ui->alertsLineEdit->setText(tr("USB speakers set. System restart required."));
            }
        }
        else
        {
            ;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }
    //processingAButton = false;
}

void setupUtility::keepAppAlive()
{
    this->update();
}

void setupUtility::saveNTP2DB()
{
    LOG_CLASS_INFO( "setupUtility", "saveNTP2DB" );
    LOG_DEBUG( "saveNTP2DB()", NULL );

    //processingAButton = true;
    // Save off all NTP data to the setup utility db file
    updateDB("ntpServer",ui->ntpServer->text());
    updateDB("ntpFrequency",QString::number(ui->ntpFrequencyCombo->currentIndex()));
    updateDB("ntpDayOfWeek",QString::number(ui->ntpDayOfWeekCombo->currentIndex()));
    updateDB("ntpTimeOfDay", ui->ntpHourBut->text() + ":" + ui->ntpMinuteBut->text() + ":" + ui->ntpSecondBut->text());

    updateDB("ntpTimeZone", ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString());
    updateDB("timezone", ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString());
    updateDB("timeKeeper","NTP");
    updateDB("ntpAntiMeridian", ui->ntpAmpmBut->text());
    //processingAButton = false;
}

void setupUtility::ntpFrequencyChanged(int index)
{
    LOG_CLASS_INFO( "setupUtility", "ntpFrequencyChanged" );
    LOG_DEBUG( "ntpFrequencyChanged()", NULL );

    if(index > 0)
    {
        ui->ntpDayOfWeekCombo->setDisabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {   border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        ui->ntpDayOfWeekCombo->setCurrentIndex(-1);
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
        //ui->ntpDayOfWeekCombo->lineEdit()->setStyleSheet("color: black; background: rgb(200,200,200);");
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        if(ui->ntpDayOfWeekCombo->currentIndex() < 0) ui->ntpDayOfWeekCombo->setCurrentIndex(0);
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
        //ui->ntpDayOfWeekCombo->lineEdit()->setStyleSheet("color: black; background: rgb(238,255,211);");
    }
}

// PRINTER SETTINGS TAB METHODS

void setupUtility::txtPrinterSettingsChanged()
{
    LOG_CLASS_INFO( "setupUtility", "txtPrinterSettingsChanged" );
    LOG_DEBUG( "txtPrinterSettingsChanged()", NULL );

    printerDriverChanged = true;

    QStringList badChars;
    badChars << " " << "`" << "~" << "!" << "@" << "#" << "$" << "^" << "&" << "*" << "(" << ")" << "=" << "+" << "[" << "]" << "{" << "}" << "\\" << "|" << ";" << ":" << "\"" << "," << "<" << ">" << "/" << "?";

    QString name1 = ui->txtPrinterNameOne->text();
    QString name2 = ui->txtPrinterNameTwo->text();

    if ( name1.contains(" ") )
    {
        this->ui->alertsLineEdit->setText(tr("Printer names cannot contain a space"));       // update status
        session_->send(Error, Printer, ",7027,5");
        name1.remove(" ");
    }

    if ( name2.contains(" ") )
    {
        this->ui->alertsLineEdit->setText(tr("Printer names cannot contain a space"));       // update status
        session_->send(Error, Printer, ",7027,5");
        name2.remove(" ");
    }


    for (int j = 0; j < badChars.size(); j++)
    {
        if ( name1.contains(badChars.at(j)) )
        {
            this->ui->alertsLineEdit->setText(tr("Name cannot contain @`~!#$^&*()=+[]{}\\|;:\\,<>/?"));
            session_->send(Error, Printer, ",7028,5");
            name1.remove(badChars.at(j));
        }

        if ( name2.contains(badChars.at(j)) )
        {
            this->ui->alertsLineEdit->setText(tr("Name cannot contain @`~!#$^&*()=+[]{}\\|;:\\,<>/?"));
            session_->send(Error, Printer, ",7028,5");
            name2.remove(badChars.at(j));
        }
    }

    ui->txtPrinterNameOne->setText( name1 );
    ui->txtPrinterNameTwo->setText( name2 );
}

void setupUtility::txtPrinterIpSettingsChanged()
{
    LOG_CLASS_INFO( "setupUtility", "txtPrinterIpSettingsChanged" );

    printerDriverChanged = true;
}

void setupUtility::cboPrinterDriverChanged(int index)
{
    LOG_CLASS_INFO( "setupUtility", "cboPrinterDriverChanged" );

    Q_UNUSED( index );

    if( blockOnTranslate )
        return;

    printerDriverChanged = true;

    setCheckBoxVisibility();
}

void setupUtility::printerTypeChangedOne(int index)
{
    LOG_CLASS_INFO( "setupUtility", "printerTypeChangedOne" );

    // Need to decide if we display the IP field, USB, enable test label
    // 0 = None
    // 1 = USB
    // 2 = Network or does not exist
    // 3 = EPP if all present or = 2 if Network does not exist or does not exist at all

    if( blockOnTranslate )
        return;

    bool doDisplayIPS = false;
    //bool doDisplayEPP = false;

    if ( 0 == index )
    {
        setEnablednessOfPrinterButtons(ui->butPrinterTestOne, false);           // if None selected
        if ( ui->cboPrinterTypeTwo->currentIndex() == 0 )
            setEnablednessOfPrinterButtons(ui->butAssignPrinterData, false);    // Find out if printer 2 is is also set to None
    }
    else
    {
        setEnablednessOfPrinterButtons(ui->butPrinterTestOne,    true);
        setEnablednessOfPrinterButtons(ui->butAssignPrinterData, true);
    }

    int cboNetIndex  = ui->cboPrinterTypeOne->findText("Network");

    if( index == cboNetIndex )
        doDisplayIPS = true;

    ui->lblPrinterIpAddressOne->setVisible(doDisplayIPS);
    ui->txtPrinterIpAddressOne->setVisible(doDisplayIPS);
    ui->lblPrinterDriverOne->setVisible(doDisplayIPS);
    ui->cboPrinterDriverOne->setVisible(doDisplayIPS);

    // Clear them all -> Now we need to decide what to display on setup page
    updateSettingsPageComboNames(index, ui->cboPrinterTypeTwo->currentIndex());

    // Cannot have 2 EPP printers
    int cboIndexOtherPrinter = ui->cboPrinterTypeTwo->currentIndex();           // get index of other printer
    QString cboText = ui->cboPrinterTypeOne->itemText(index);

    if ( index > 0 && index == cboIndexOtherPrinter && cboText == "EPP")
    {
        session_->send( Error, ErrPrinterDevice, "7558,5" );                    // Tell the user that 2 EPP printers cannot be selected
        ui->cboPrinterTypeOne->setCurrentIndex(0);
    }


    setCheckBoxVisibility();

    printerDriverChanged = true;
}

void setupUtility::printerTypeChangedTwo(int index)
{
    LOG_CLASS_INFO( "setupUtility", "printerTypeChangedTwo" );

    // Need to decide if we display the IP field, USB, enable test label
    // 0 = None
    // 1 = USB
    // 2 = Network or does not exist
    // 3 = EPP if all present or = 2 if Network does not exist or does not exist at all

    if( blockOnTranslate )
        return;

    bool doDisplayIPS = false;

    if ( 0 == index )
    {
        setEnablednessOfPrinterButtons(ui->butPrinterTestTwo, false);           // if None selected
        if ( ui->cboPrinterTypeOne->currentIndex() == 0 )
            setEnablednessOfPrinterButtons(ui->butAssignPrinterData, false);    // Find out if printer 2 is is also set to None
    }
    else
    {
        setEnablednessOfPrinterButtons(ui->butPrinterTestTwo,    true);
        setEnablednessOfPrinterButtons(ui->butAssignPrinterData, true);
    }

    int cboNetIndex  = ui->cboPrinterTypeTwo->findText("Network");

    if(index == cboNetIndex )
        doDisplayIPS = true;

    ui->lblPrinterIpAddressTwo->setVisible(doDisplayIPS);
    ui->txtPrinterIpAddressTwo->setVisible(doDisplayIPS);
    ui->lblPrinterDriverTwo->setVisible(doDisplayIPS);
    ui->cboPrinterDriverTwo->setVisible(doDisplayIPS);

    // Clear them all -> Now we need to decide what to display on setup page
    updateSettingsPageComboNames(ui->cboPrinterTypeOne->currentIndex(), index);

    // Cannot have 2 EPP printers
    int cboIndexOtherPrinter = ui->cboPrinterTypeOne->currentIndex();           // get index of other printer
    QString cboText = ui->cboPrinterTypeOne->itemText(index);

    if ( index > 0 && index == cboIndexOtherPrinter  && cboText == "EPP" )
    {
        session_->send( Error, ErrPrinterDevice, "7558,5" );                    // Tell the user that the printers cannot be the same
        ui->cboPrinterTypeTwo->setCurrentIndex(0);
    }

    setCheckBoxVisibility();

    printerDriverChanged = true;
}

void setupUtility::setEnablednessOfPrinterButtons(QPushButton *widget, bool isEnabled)
{
    if ( isEnabled )
    {
        widget->setEnabled(true);
        widget->setStyleSheet("border-radius: 12px; color: rgb(255,255,255); background: rgb(0,0,0); border: 2px solid gray; color:white;");
    }
    else
    {
        widget->setEnabled(false);
        widget->setStyleSheet("border-radius: 12px; color: rgb(255,255,255); background: rgb(0,0,0); border: 2px solid gray; color:gray;");
    }
}

void setupUtility::setCheckBoxVisibility()
{
    // Now need to decide if I need to display the Print Label Report checkboxes

    bool printReportCheckboxOne = false;
    bool printReportCheckboxTwo = false;

    QString printerNumOne           = "0";
    QString printerNumTwo           = "0";
    QString printerDriverOne        = "";
    QString printerDriverTwo        = "";

    int printerTypeOne              = NoPrinter;
    int printerTypeTwo              = NoPrinter;
    QString printerDriverNameOne    = "";
    QString printerDriverNameTwo    = "";

    if ( ui->cboPrinterTypeOne->currentIndex() > 0 )
    {
        QString tmpType = ui->cboPrinterTypeOne->currentText();
        printerNumOne  = prDataNameToType[tmpType];
        printerDriverOne = ui->cboPrinterDriverOne->currentText();
    }

    if ( ui->cboPrinterTypeTwo->currentIndex() > 0 )
    {
        QString tmpType = ui->cboPrinterTypeTwo->currentText();
        printerNumTwo  = prDataNameToType[tmpType];
        printerDriverTwo = ui->cboPrinterDriverTwo->currentText();
    }

    if ( printerNumOne.toInt() > 0 )
    {
        printerTypeOne  = printerNumOne.toInt();
        printerDriverNameOne = printerDriverOne;
    }

    if (printerNumTwo.toInt() > 0 )
    {
        printerTypeTwo  = printerNumTwo.toInt();
        printerDriverNameTwo = printerDriverTwo;
    }

    if ( printerTypeOne > NoPrinter )
    {
        // may have to show them
        if ( printerTypeOne != NetworkPrinter)
            printReportCheckboxOne = true;
        else if( printerTypeOne == NetworkPrinter && printerDriverNameOne.contains("zebra", Qt::CaseInsensitive) )
            printReportCheckboxOne = true;

        if( printerTypeOne == NetworkPrinter && !printerDriverNameOne.contains("zebra", Qt::CaseInsensitive) )
            ui->chkPrintPluLabelOne->setChecked(false);
    }

    if ( printerTypeTwo > NoPrinter )
    {
        // may have to show them
        if ( printerTypeTwo != NetworkPrinter)
            printReportCheckboxTwo = true;
        else if( printerTypeTwo == NetworkPrinter && printerDriverNameTwo.contains("zebra", Qt::CaseInsensitive) )
            printReportCheckboxTwo = true;

        if( printerTypeTwo == NetworkPrinter && !printerDriverNameTwo.contains("zebra", Qt::CaseInsensitive) )
            ui->chkPrintPluLabelTwo->setChecked(false);
    }

    ui->chkPrintPluLabelOne->setVisible(printReportCheckboxOne);
    ui->chkPrintPluLabelTwo->setVisible(printReportCheckboxTwo);
}

void setupUtility::testExternalPrinterOne()
{
    this->ui->alertsLineEdit->setText(tr("Printing test label for external printer 1 ..."));       // update status

    if( printerDriverChanged )
        saveExternalPrinterSettings();

    session_->send(Printer ,PrTestLabel, "1,2,0");                  // printerNum, checkerboard, howMany
}

void setupUtility::testExternalPrinterTwo()
{
    this->ui->alertsLineEdit->setText(tr("Printing test label for external printer 2 ..."));       // update status

    if( printerDriverChanged )
        saveExternalPrinterSettings();

    session_->send(Printer ,PrTestLabel, "2,2,0");                  // printerNum, checkerboard, howMany
}

void setupUtility::gotoPrinterSettings()
{
    saveProductTextPrintParams();

    this->ui->alertsLineEdit->setText(tr("Configure external printers"));                   // update status
    ui->boxPrinterSettingsChrome->setVisible(true);                                         // Displays the data settings
    ui->boxPrinterDataChrome->setVisible(false);                                            // Hides the data settings
}

void setupUtility::saveExternalPrinterSettings()
{ 
    txtPrinterSettingsChanged();                                    // clean up the names first

    internalConfigData_->hasExternalPrinterSettingsChanged = true;
    QString printerNameOne = ui->txtPrinterNameOne->text();
    QString printerNameTwo = ui->txtPrinterNameTwo->text();
    QString selectedText = ui->cboPrinterTypeOne->currentText();
    QString printerType = prDataNameToType[selectedText];
    QString printerAddr = ui->txtPrinterIpAddressOne->text();

    // Update Printer Names on other pages if need be
    // Update Printer Names in Printer Data Page 2 Combos if EXT1 names are not original
    // These go on page 2 for the Printer Prompt Options Per field - Primary,EXT1, EXT2, Prompt
    updateSettingsPageComboNames(ui->cboPrinterTypeOne->currentIndex(), ui->cboPrinterTypeTwo->currentIndex());

    QString driverName = "";                                        // is it USB, Network, or EPP?
    if ( selectedText == "Network" )
        driverName = ui->cboPrinterDriverOne->currentText();

    QString msg = "1," + printerNameOne + "," + printerType + "," + printerAddr + "," + driverName;
    session_->send(Printer, PrSaveExternalPrinter, msg);

    selectedText = ui->cboPrinterTypeTwo->currentText();
    printerType = prDataNameToType[selectedText];
    printerAddr = ui->txtPrinterIpAddressTwo->text();

    driverName = "";
    if ( selectedText == "Network" )
        driverName = ui->cboPrinterDriverTwo->currentText();

    msg = "2," + printerNameTwo + "," + printerType + "," + printerAddr + "," + driverName;
    session_->send(Printer, PrSaveExternalPrinter, msg);
    this->ui->alertsLineEdit->setText(tr("Saved external printer data ..."));       // update status

    printerDriverChanged = false;
}

void setupUtility::chkPrinterSettingsOneChanged(int state)
{
    if ( state == 0 )
        session_->send(ConfigurationDev , CfgSaveSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride2ndPrinter ) + ",0" );
    else
        session_->send(ConfigurationDev , CfgSaveSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride2ndPrinter ) + ",1" );
}

void setupUtility::chkPrinterSettingsTwoChanged(int state)
{
    if ( state == 0)
        session_->send(ConfigurationDev , CfgSaveSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride3rdPrinter ) + ",0" );
    else
        session_->send(ConfigurationDev , CfgSaveSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride3rdPrinter ) + ",1" );
}

void setupUtility::updateSettingsPageComboNames(int indexOne, int indexTwo)
{
    ui->cboPrinterTypeOne->blockSignals( true );
    ui->cboPrinterTypeTwo->blockSignals( true );
    ui->cboPrinterDriverOne->blockSignals( true );
    ui->cboPrinterDriverTwo->blockSignals( true );

    ui->cboPrinterData0->clear();                                   // clear everything before adding to it.
    ui->cboPrinterData1->clear();
    ui->cboPrinterData2->clear();
    ui->cboPrinterData3->clear();
    ui->cboPrinterData4->clear();

    ui->cboPrinterData0->addItem(tr("Primary"), QVariant( "0" ) );
    ui->cboPrinterData1->addItem(tr("Primary"), QVariant( "0" ) );
    ui->cboPrinterData2->addItem(tr("Primary"), QVariant( "0" ) );
    ui->cboPrinterData3->addItem(tr("Primary"), QVariant( "0" ) );
    ui->cboPrinterData4->addItem(tr("Primary"), QVariant( "0" ) );

    if( indexOne > 0 )
    {
        QString tmpNameOne = ui->txtPrinterNameOne->text();

        ui->cboPrinterData0->addItem(tmpNameOne, QVariant( "1" ) );
        ui->cboPrinterData1->addItem(tmpNameOne, QVariant( "1" ) );
        ui->cboPrinterData2->addItem(tmpNameOne, QVariant( "1" ) );
        ui->cboPrinterData3->addItem(tmpNameOne, QVariant( "1" ) );
        ui->cboPrinterData4->addItem(tmpNameOne, QVariant( "1" ) );
    }

    if( indexTwo > 0)
    {
        QString tmpNameTwo = ui->txtPrinterNameTwo->text();

        ui->cboPrinterData0->addItem(tmpNameTwo, QVariant( "2" ) );
        ui->cboPrinterData1->addItem(tmpNameTwo, QVariant( "2" ) );
        ui->cboPrinterData2->addItem(tmpNameTwo, QVariant( "2" ) );
        ui->cboPrinterData3->addItem(tmpNameTwo, QVariant( "2" ) );
        ui->cboPrinterData4->addItem(tmpNameTwo, QVariant( "2" ) );
    }

    ui->cboPrinterData0->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData1->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData2->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData3->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData4->addItem(tr("Prompt"), QVariant( "9" ) );

    refreshPrinterTabData();

    ui->cboPrinterTypeOne->blockSignals( false );
    ui->cboPrinterTypeTwo->blockSignals( false );
    ui->cboPrinterDriverOne->blockSignals( false );
    ui->cboPrinterDriverTwo->blockSignals( false );

}

void setupUtility::saveProductTextPrintParams()
{
    QString name        = QString::number( prDataIntsMap[ui->lblPrinterData0->text()] );
    QString label       = ui->txtPrinterData0->text();
    int index           = ui->cboPrinterData0->currentIndex();
    QString prompt      = ui->cboPrinterData0->itemData(index).toString(); // prDataPromptMap[index];
    QString msg = "";
    msg = name + "," + prompt + "," + label;                        // Name, Promptabiltiness, LabelNum
    session_->send(Printer, PrSaveProductTextPrintParam, msg);

    name        = QString::number( prDataIntsMap[ui->lblPrinterData1->text()] );
    label       = ui->txtPrinterData1->text();
    index       = ui->cboPrinterData1->currentIndex();
    prompt      = ui->cboPrinterData1->itemData(index).toString();  // prDataPromptMap[index];
    msg = name + "," + prompt + "," + label;                        // Name, Promptabiltiness, LabelNum
    session_->send(Printer, PrSaveProductTextPrintParam, msg);

    name        = QString::number( prDataIntsMap[ui->lblPrinterData2->text()] );
    label       = ui->txtPrinterData2->text();
    index       = ui->cboPrinterData2->currentIndex();
    prompt      = ui->cboPrinterData2->itemData(index).toString();  // prDataPromptMap[index];
    msg = name + "," + prompt + "," + label;                        // Name, Promptabiltiness, LabelNum
    session_->send(Printer, PrSaveProductTextPrintParam, msg);

    name        = QString::number( prDataIntsMap[ui->lblPrinterData3->text()] );
    label       = ui->txtPrinterData3->text();
    index       = ui->cboPrinterData3->currentIndex();
    prompt      = ui->cboPrinterData3->itemData(index).toString();  // prDataPromptMap[index];
    msg = name + "," + prompt + "," + label;                        // Name, Promptabiltiness, LabelNum
    session_->send(Printer, PrSaveProductTextPrintParam, msg);

    name        = QString::number( prDataIntsMap[ui->lblPrinterData4->text()] );
    label       = ui->txtPrinterData4->text();
    index       = ui->cboPrinterData4->currentIndex();
    prompt      = ui->cboPrinterData4->itemData(index).toString();  // prDataPromptMap[index];
    msg = name + "," + prompt + "," + label;                        // Name, Promptabiltiness, LabelNum
    session_->send(Printer, PrSaveProductTextPrintParam, msg);

    // Now that I have saved them all off, I need to re-request the data myself
    // so if an operator goes to the prompt page again, I have most recent states

    // 7,131 reguest Printer Prompt field param information for pluinfo operate page
    session_->send(Printer, PrReadAllProductTextPrintParams);
}

void setupUtility::gotoPrinterDataScreen()
{
    this->ui->alertsLineEdit->setText(tr("Assign data to print on external printers"));     // update status

    // Need to update any name changes for combo boxes
    int indexOne = ui->cboPrinterTypeOne->currentIndex();
    int indexTwo = ui->cboPrinterTypeTwo->currentIndex();

    // Clear them all -> Now we need to decide what to display on setup page
    updateSettingsPageComboNames(indexOne, indexTwo);

    ui->boxPrinterDataChrome->setVisible(true);                                             // Displays the data settings
    ui->boxPrinterSettingsChrome->setVisible(false);                                        // Hides the data settings
}

void setupUtility::setupPrinterTab()
{
    // This is stuff you want to do once.

    QRegExpValidator *v = new QRegExpValidator(this);
    QRegExp rx("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.){3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
    v->setRegExp(rx);

    ui->txtPrinterIpAddressOne->setValidator(v);
    ui->txtPrinterIpAddressTwo->setValidator(v);

    prDataNamesMap[PrTypeUnknown]           = "Unknown";
    prDataNamesMap[PrExpandedText]          = "Expanded Text";
    prDataNamesMap[PrSpecialMessage]        = "Special Message";
    prDataNamesMap[PrProductNote]           = "Product Note";
    prDataNamesMap[PrMarqueeText]           = "Marquee Text";
    prDataNamesMap[PrCookingInstructions]   = "Cooking Instructions";
    prDataNamesMap[PrRecipes]               = "Recipe";
    prDataNamesMap[PrExpandedText2]         = "Expanded Text 2";
    prDataNamesMap[PrSuggestiveSelling]     = "Suggestive Selling";
    prDataNamesMap[PrAllergens]             = "Allergens";
    prDataNamesMap[PrNutrifacts]            = "Nutrifacts";

    prDataIntsMap["Unknown"]                = PrTypeUnknown;
    prDataIntsMap["Expanded Text"]          = PrExpandedText;
    prDataIntsMap["Special Message"]        = PrSpecialMessage;
    prDataIntsMap["Product Note"]           = PrProductNote;
    prDataIntsMap["Marquee Text"]           = PrMarqueeText;
    prDataIntsMap["Cooking Instructions"]   = PrCookingInstructions;
    prDataIntsMap["Recipe"]                 = PrRecipes;
    prDataIntsMap["Expanded Text 2"]        = PrExpandedText2;
    prDataIntsMap["Suggestive Selling"]     = PrSuggestiveSelling;
    prDataIntsMap["Allergens"]              = PrAllergens;
    prDataIntsMap["Nutrifacts"]             = PrNutrifacts;

    prDataModelMap[NoPrinter]               = "None";
    prDataModelMap[UsbPrinter]              = "USB";
    prDataModelMap[NetworkPrinter]          = "Network";
    prDataModelMap[EppPrinter]              = "EPP";                // EPP 1 - NOT SUPPORTED
    //prDataModelMap[Epp2Printer]             = "EPP";                // EPP 2 - SUPPORTED

    prDataPromptMap[0]                      = "0";                  // Primary
    prDataPromptMap[1]                      = "1";                  // EXT1
    prDataPromptMap[2]                      = "2";                  // EXT2
    prDataPromptMap[3]                      = "9";                  // Prompt

    prDataPromptMapIndex["0"]               = 0;
    prDataPromptMapIndex["1"]               = 1;
    prDataPromptMapIndex["2"]               = 2;
    prDataPromptMapIndex["9"]               = 3;

    prDataNameToType["None"]                = QString::number(NoPrinter);
    prDataNameToType["USB"]                 = QString::number(UsbPrinter);
    prDataNameToType["Network"]             = QString::number(NetworkPrinter);
    prDataNameToType["EPP"]                 = QString::number(EppPrinter);      // EPP 1 - NOT SUPPORTED
    //prDataNameToType["EPP"]                 = QString::number(Epp2Printer);      // EPP 2 - SUPPORTED
}

void setupUtility::continueSetupPrinterTab()
{
    // These things you want to do every time
    session_->send(SessionDev , SnRestartDialogs);

    session_->send(ConfigurationDev , CfgReadSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride2ndPrinter ) );
    session_->send(ConfigurationDev , CfgReadSingleField, "," + QString::number( fieldOffsetCfgVirtualPrintingOverride3rdPrinter ) );

    this->ui->alertsLineEdit->setText(tr("Configure external printers"));

    setEnablednessOfPrinterButtons(ui->butAssignPrinterData, false);
    setEnablednessOfPrinterButtons(ui->butPrinterTestOne,    false);
    setEnablednessOfPrinterButtons(ui->butPrinterTestTwo,    false);

    ui->lblPrinterIpAddressOne->setVisible(false);      // Hide these until we get back a msg on their status
    ui->txtPrinterIpAddressOne->setVisible(false);
    ui->lblPrinterIpAddressTwo->setVisible(false);
    ui->txtPrinterIpAddressTwo->setVisible(false);
    ui->lblPrinterDriverOne->setVisible(false);
    ui->cboPrinterDriverOne->setVisible(false);
    ui->lblPrinterDriverTwo->setVisible(false);
    ui->cboPrinterDriverTwo->setVisible(false);
    ui->chkPrintPluLabelOne->setVisible(false);
    ui->chkPrintPluLabelTwo->setVisible(false);

    // TODO - 2nd printer testing - DELETE WHEN DONE
    // updatePrinterTabData();
}

void setupUtility::updatePrinterCheckboxes(const SessionMessage &msg)
{
    DomConfigWriteSingleField m( msg );
    if ( configWriteSingleField )
       *configWriteSingleField = m;

    int offset   = m.offsetIndex().toInt();
    int checkboxState = m.newValue().toInt();

    if ( offset == fieldOffsetCfgVirtualPrintingOverride2ndPrinter )
        ui->chkPrintPluLabelOne->setChecked(checkboxState);
    else if ( offset == fieldOffsetCfgVirtualPrintingOverride3rdPrinter )
        ui->chkPrintPluLabelTwo->setChecked(checkboxState);
}

void setupUtility::updateUsbComboNames(const SessionMessage &msg)
{
    DomPrWriteAllUsbDriverNames m( msg );
    if ( writeAllUsbDriverNames_ )
       *writeAllUsbDriverNames_ = m;

    // 7,113 reguest Network Driver Names - moved to where I used to request USB Drivers
    session_->send(Printer, PrReadAllNetworkPrinterDriverNames);    // Request Printer NETWORK Driver Names
}

void setupUtility::updateNetworkComboNames(const SessionMessage &msg)
{
    LOG_CLASS_INFO( "setupUtility", "updateNetworkComboNames" );
    LOG_DEBUG( "updateNetworkComboNames() START", NULL );

    DomPrWriteAllNetworkPrinterDriverNames m( msg );
    if ( writeAllNetworkDriverNames_ )
       *writeAllNetworkDriverNames_ = m;

    int networkNameCount        = m.networkPrinterNamesListCount();
    QStringList networkNameList = m.networkPrinterNamesList().split(",");
    ui->cboPrinterDriverOne->clear();
    ui->cboPrinterDriverTwo->clear();

    // determine which Network labels and combos to display.
    // we dont know if the network item sits in slot1, slot2, or both.
    if ( networkNameCount > 0 )
    {
        ui->cboPrinterDriverOne->addItems( networkNameList );
        ui->cboPrinterDriverTwo->addItems( networkNameList );
    }

    // 7, 121 - request all printer information for external printers
    session_->send(Printer, PrReadAllExternalPrinters);             // Request Printer External Printer Settings
}

void setupUtility::updatePrinterTab(const SessionMessage &msg)
{
    LOG_CLASS_INFO( "setupUtility", "updatePrinterTab" );

    DomPrWriteAllExternalPrinter m( msg );

    if ( writeAllExternalPrinter_ )
       *writeAllExternalPrinter_ = m;                               // lets me use and access this message in other locations/methods

    if( extPrintSettings_ )
        *extPrintSettings_ = m;

    ui->cboPrinterTypeOne->blockSignals( true );
    ui->cboPrinterTypeTwo->blockSignals( true );
    ui->cboPrinterDriverOne->blockSignals( true );
    ui->cboPrinterDriverTwo->blockSignals( true );

    bool isEppLicensed              = false;
    bool isOfficeLicensed           = false;

    QString htModelName             = "";
    QString printerNameOne          = "";
    QString printerNameTwo          = "";
    int printerTypeOne              = 0;
    int printerTypeTwo              = 0;

    QString printerDriverNameOne    = "";
    QString printerDriverNameTwo    = "";
    QString printerTypeIpAddressOne = "";
    QString printerTypeIpAddressTwo = "";

    int numOfPrinters               = 0;
    int numOfPrinterNames           = 0;
    int numOfPrinterTypes           = 0;
    int numOfDriversNames           = 0;        // name of usb or network driver
    int numOfIps                    = 0;        // addresses

    QStringList printerNumberList;                                  // This is what printer at what location (could just set up #2)
    QStringList printerNamesList;                                   // The names of the pritners (EXT1)
    QStringList printerTypeList;                                    // what type of printer is it? USB, EPP, etc?
    QStringList printerDriversList;                                 // Name of USB port driver or network driver
    QStringList printerIpAddressList;                               // IP address where network printer is located

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString licenses = settings->licensedFeatures();

    numOfPrinters        = m.extPrinterNumListCount();
    printerNumberList    = m.extPrinterNumList().split(",");
    numOfPrinterNames    = m.extPrinterNamesListCount();
    printerNamesList     = m.extPrinterNamesList().split(",");
    numOfPrinterTypes    = m.extPrinterTypeCount();
    printerTypeList      = m.extPrinterTypeList().split(",");
    numOfDriversNames    = m.extPrinterDriverCount();
    printerDriversList   = m.extPrinterDriverList().split(",");
    numOfIps             = m.extPrinterIpAddressCount();
    printerIpAddressList = m.extPrinterIpAddressList().split(",");

    if (licenses.contains( "SCALE_PRINTER_EPP" ))
        isEppLicensed = true;

    if ( licenses.contains( "SCALE_OFFICE_VIEWER"))
        isOfficeLicensed = true;

    LOG_DEBUG( "EPP %d, OFFICE %d", isEppLicensed, isOfficeLicensed );

    htModelName = getScaleModel();                                  // after we get the model name, peel off first 3 chars
    htModelName.truncate(3);

    LOG_DEBUG( "Model %s", qPrintable(htModelName) );

    ui->cboPrinterData0->clear();                                   // clear everything before adding to it.
    ui->cboPrinterData1->clear();
    ui->cboPrinterData2->clear();
    ui->cboPrinterData3->clear();
    ui->cboPrinterData4->clear();

    if (numOfPrinterNames >= 1 && printerNumberList.at(0) == "1")
    {
        // SET UP FIRST COLUMN - PRINTER NAME 1 is Printer 1
        // ( bc we may have 1 printer as the 2nd printer )
        printerNameOne   = printerNamesList.at(0);                  // could be blank

        if ( printerNameOne == "" )                                 // Test if blank
            printerNameOne = "EXT1";

        ui->txtPrinterNameOne->setText(printerNameOne);             // set printer name 1
    }
    else if (numOfPrinterNames >= 1 && printerNumberList.at(0) == "2")
    {
        // SET UP FIRST COLUMN - PRINTER NAME 1 is Printer 2
        // ( bc we may have 1 printer as the 2nd printer )
        printerNameTwo   = printerNamesList.at(0);                  // could be blank

        if ( printerNameTwo == "" )                                 // Test if blank
            printerNameOne = "EXT2";

        ui->txtPrinterNameTwo->setText(printerNameTwo);             // set printer name 1
    }

    if ( numOfPrinterNames == 2 )
    {
        printerNameTwo   = printerNamesList.at(1);                  // could have one and [0] is blank - test and protect it

        if ( printerNameTwo == "" )                                 // Test if blank
            printerNameTwo = "EXT2";

        ui->txtPrinterNameTwo->setText(printerNameTwo);             // set printer name 2
    }

    // Update Printer Names on other pages if need be
    // Update Printer Names in Printer Data Page 2 Combos if EXT1 names are not original
    // These go on page 2 for the Printer Prompt Options Per field - Primary,EXT1, EXT2, Prompt
    if( numOfPrinterNames >= 1 )
    {
        ui->cboPrinterData0->addItem(printerNameOne, QVariant( "1" ) );
        ui->cboPrinterData1->addItem(printerNameOne, QVariant( "1" ) );
        ui->cboPrinterData2->addItem(printerNameOne, QVariant( "1" ) );
        ui->cboPrinterData3->addItem(printerNameOne, QVariant( "1" ) );
        ui->cboPrinterData4->addItem(printerNameOne, QVariant( "1" ) );
    }

    // Update Printer Names in Printer Data Page 2 Combos if EXT2 names are not original
    // These go on page 2 for the Printer Prompt Options Per field - Primary,EXT1, EXT2, Prompt
    if( numOfPrinterNames == 2 )
    {  
        ui->cboPrinterData0->addItem(printerNameTwo, QVariant( "1" ) );
        ui->cboPrinterData1->addItem(printerNameTwo, QVariant( "1" ) );
        ui->cboPrinterData2->addItem(printerNameTwo, QVariant( "1" ) );
        ui->cboPrinterData3->addItem(printerNameTwo, QVariant( "1" ) );
        ui->cboPrinterData4->addItem(printerNameTwo, QVariant( "1" ) );
    }

    // Add the Prompt selection
    ui->cboPrinterData0->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData1->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData2->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData3->addItem(tr("Prompt"), QVariant( "9" ) );
    ui->cboPrinterData4->addItem(tr("Prompt"), QVariant( "9" ) );

    // SETUP SECOND ROW - PRINTER TYPES - What is allowed due to license and model
    // Add Network Printer options if allowed and not already added
    int ipIndex = ui->cboPrinterTypeOne->findText("Network");
    LOG_DEBUG( "Network index %d", ipIndex );

    // see if network is already in the list of options
    if ( ipIndex == -1 )
    {
        if (( htModelName == "HTm" ) ||
            ( htModelName == "HTx" ) ||
            ( htModelName == "HTi" && isOfficeLicensed ))
        {
            // We can add a "Network" option if Model is HTi and we have Office license and hasn't been previously added
            ui->cboPrinterTypeOne->addItem( tr("Network"));
            ui->cboPrinterTypeTwo->addItem( tr("Network"));
        }
    }

    // Add EPP printer if scale is licensed for it and not already added
    int eppIndex = ui->cboPrinterTypeOne->findText("EPP");
    LOG_DEBUG( "EPP index %d", ipIndex );

    if ( isEppLicensed && eppIndex == -1 )
    {
        // We can add EPP option if EPP is licensed printer option and hasn't been previously added
        ui->cboPrinterTypeOne->addItem( tr("EPP"));
        ui->cboPrinterTypeTwo->addItem( tr("EPP"));
    }

    LOG_DEBUG( "Number of Printers %d", numOfPrinters );

    if ( numOfPrinters == 0)
    {
        ui->cboPrinterTypeOne->blockSignals( false );
        ui->cboPrinterTypeTwo->blockSignals( false );
        ui->cboPrinterDriverOne->blockSignals( false );
        ui->cboPrinterDriverTwo->blockSignals( false );

        // 7,131 reguest Printer Prompt field param information for pluinfo operate page
        session_->send(Printer, PrReadAllProductTextPrintParams);   // 7,131 - Request Printer Data Settings

        return;                                                     // nothing else to do if no printers
    }

    // We have now set up WHAT KIND of types we can have
    // NOW SET THE TYPES SELECTED
    if ( numOfPrinterTypes >= 1)
    {
        if (printerTypeList.at(0).size() > 0)
            printerTypeOne  = printerTypeList.at(0).toInt();
        else
            printerTypeOne = 0;                                     // if field was blank (should at least be 0)

        if ( numOfPrinterTypes == 2)
        {
            if (printerTypeList.at(1).size() > 0)
                printerTypeTwo  = printerTypeList.at(1).toInt();
            else
                printerTypeTwo = 0;                                 // if field was blank default to 0
        }

        if ( printerNumberList.at(0) == "1" )
        {
            // Have to do this in case the user set up a single printer but in 2nd column
            // In this case, they user set up a single printer in first column
            QString tmpData = prDataModelMap[printerTypeOne];
            int typeIndexOne =  ui->cboPrinterTypeOne->findText(tmpData);
            ui->cboPrinterTypeOne->setCurrentIndex(typeIndexOne);
        }
        else if ( printerNumberList.at(0) == "2" )
        {
            // Have to do this in case the user set up a single printer but in 2nd column
            // In this case the user set up a single printer in the 2nd column
            QString tmpData = prDataModelMap[printerTypeOne];
            int typeIndexTwo =  ui->cboPrinterTypeOne->findText(tmpData);
            ui->cboPrinterTypeTwo->setCurrentIndex(typeIndexTwo);
        }

        if ( printerNumberList.at(0) != "2" )
        {
            int typeIndexTwo =  ui->cboPrinterTypeTwo->findText(prDataModelMap[printerTypeTwo]);
            ui->cboPrinterTypeTwo->setCurrentIndex(typeIndexTwo);
        }
    }

    // SETUP THIRD ROW - IP ADDRESSES
    if ( numOfIps >= 1 )                                             // Process the IP addresses if there are any
    {
        if ( printerNumberList.at(0) == "1" )
            printerTypeIpAddressOne = printerIpAddressList.at(0);
        else if ( printerNumberList.at(0) == "2" )
            printerTypeIpAddressTwo = printerIpAddressList.at(0);

        if( prDataModelMap[printerTypeOne] == "Network" && printerNumberList.at(0) == "1")
        {
            // Update the IP addresses if printer 1 is a printer 1
            ui->lblPrinterIpAddressOne->setVisible(true);
            ui->txtPrinterIpAddressOne->setVisible(true);
            ui->txtPrinterIpAddressOne->setText(printerTypeIpAddressOne);
        }
        else
        {
            ui->txtPrinterIpAddressOne->setText("");
            ui->lblPrinterIpAddressOne->setVisible(false);
            ui->txtPrinterIpAddressOne->setVisible(false);

        }

        if( prDataModelMap[printerTypeOne] == "Network" && printerNumberList.at(0) == "2")
        {
            // Update the IP addresses if printer 1 is actually a printer 2
            ui->lblPrinterIpAddressTwo->setVisible(true);
            ui->txtPrinterIpAddressTwo->setVisible(true);
            ui->txtPrinterIpAddressTwo->setText(printerTypeIpAddressTwo);
        }
        else
        {
            ui->txtPrinterIpAddressTwo->setText("");
            ui->lblPrinterIpAddressTwo->setVisible(false);
            ui->txtPrinterIpAddressTwo->setVisible(false);
        }

        // This condition will only be true if we have a printer 1 at printer 1 and a 2nd printer too
        if ( numOfIps == 2)
        {
            // Update the IP addresses if we have network printer
            printerTypeIpAddressTwo = printerIpAddressList.at(1);

            if( prDataModelMap[printerTypeTwo] == "Network" )
            {
                ui->lblPrinterIpAddressTwo->setVisible(true);
                ui->txtPrinterIpAddressTwo->setVisible(true);
                ui->txtPrinterIpAddressTwo->setText(printerTypeIpAddressTwo);
            }
            else
            {
                ui->txtPrinterIpAddressTwo->setText("");
                ui->lblPrinterIpAddressTwo->setVisible(false);
                ui->txtPrinterIpAddressTwo->setVisible(false);
            }
        }
    }

    // SETUP FORTH ROW - NETWORK DRIVER NAME
    if ( numOfDriversNames >= 1 )
    {
        // Update the USB port assignment
        // List will be 0, or "driver1,driver2", or "driver1,blank" or "blank,driver2"
        if ( printerNumberList.at(0) == "1" )
            printerDriverNameOne = printerDriversList.at(0);              // This is the name that BETTER match one of the names in combo
        else if ( printerNumberList.at(0) == "2" )
            printerDriverNameTwo = printerDriversList.at(0);

        if( prDataModelMap[printerTypeOne] == "Network" && printerNumberList.at(0) == "1" )
        {
            // if user set up a printer 1 in the printer 2 slot
            int ipDriverOne =  ui->cboPrinterDriverOne->findText(printerDriverNameOne);
            ui->cboPrinterDriverOne->setCurrentIndex(ipDriverOne);
            ui->lblPrinterDriverOne->setVisible(true);
            ui->cboPrinterDriverOne->setVisible(true);
        }
        else if( prDataModelMap[printerTypeOne] == "Network" && printerNumberList.at(0) == "2" )
        {
            // if user set up a printer 1 in the printer 2 slot
            int ipDriverTwo =  ui->cboPrinterDriverTwo->findText(printerDriverNameTwo);
            ui->cboPrinterDriverTwo->setCurrentIndex(ipDriverTwo);
            ui->lblPrinterDriverTwo->setVisible(true);
            ui->cboPrinterDriverTwo->setVisible(true);
        }

        if ( numOfDriversNames == 2 )
        {
            // Update the USB port assignment

            printerDriverNameTwo = printerDriversList.at(1);

            if( prDataModelMap[printerTypeTwo] == "Network" )
            {
                int ipTypeIndexTwo =  ui->cboPrinterDriverTwo->findText(printerDriverNameTwo);
                ui->cboPrinterDriverTwo->setCurrentIndex(ipTypeIndexTwo);
                ui->lblPrinterDriverTwo->setVisible(true);
                ui->cboPrinterDriverTwo->setVisible(true);
            }
        }
    }

    setCheckBoxVisibility();

    int printerNumOne = 0;
    int printerNumTwo = 0;

    if ( printerNumberList.length() == 1 )
    {
        printerNumOne = printerNumberList.at(0).toInt();
        setEnablednessOfPrinterButtons(ui->butAssignPrinterData, true);
    }
    else if( printerNumberList.length() == 2 )
    {
        // if we have 2 printers, go ahead and enable everything
        printerNumOne = printerNumberList.at(0).toInt();
        printerNumTwo = printerNumberList.at(1).toInt();
        setEnablednessOfPrinterButtons(ui->butAssignPrinterData, true);
        setEnablednessOfPrinterButtons(ui->butPrinterTestOne,    true);
        setEnablednessOfPrinterButtons(ui->butPrinterTestTwo,    true);
    }


    if ( printerNumOne > 0 && printerNumTwo == 0 )
    {
        // If we have one printer, it may be set up in the first slot or second slot.
        // So now we know we have a printer, now find out if it's EXT1 or EXT 2

        if ( 1 == printerNumOne )
        {
            // indicates the single printer attached is in the EXT 1 slot
            setEnablednessOfPrinterButtons(ui->butPrinterTestOne, true);
            setEnablednessOfPrinterButtons(ui->butPrinterTestTwo, false);
        }
        else if ( 2 == printerNumOne )
        {
            // indicates the single printer attached is in the EXT 2 slot
            setEnablednessOfPrinterButtons(ui->butPrinterTestOne, false);
            setEnablednessOfPrinterButtons(ui->butPrinterTestTwo, true);
        }
    }

    ui->cboPrinterTypeOne->blockSignals( false );
    ui->cboPrinterTypeTwo->blockSignals( false );
    ui->cboPrinterDriverOne->blockSignals( false );
    ui->cboPrinterDriverTwo->blockSignals( false );

    // 7,131 reguest Printer Prompt field param information for pluinfo operate page
    session_->send(Printer, PrReadAllProductTextPrintParams);       // 7,131 - Request Printer Data Settings
}

void setupUtility::updatePrinterTabData(const SessionMessage& msg )
{

    DomPrWriteAllProductTextPrintParams m( msg );

    if ( writeAllProdTxtPrintParams_ )
       *writeAllProdTxtPrintParams_ = m;                            // lets me use and access this message in other locations/methods

    if( promptness_ )
        *promptness_ = m;                                                // save for later use

    QString     approvedStrOfTexts  = "1,5,6,9,20";
    QStringList approvedListOfTexts = approvedStrOfTexts.split(",");

    QString tmpNames   = m.extPrinterDataNamesNumList();
    QString tmpPrompts = m.extPrinterDataLabelsPrompt();
    QString tmpLabels  = m.extPrinterDataLabelNumList();
    int namesListCount = m.extPrinterDataNamesNumCount();

    // TODO - 2nd printer testing - DELETE WHEN DONE
    //QString tmpNames   = "1,2,3,4,5,6,7,8,9,20";     // field enum offset num
    //QString tmpPrompts = "9,0,0,0,9,9,0,0,9,9";      // user prompt action
    //QString tmpLabels  = "10098,10098,10098,10098,10098,10098,10098,10098,10098,10080";     // label ID numbers
    //int namesListCount     = 10;

    QStringList namesListStr  = tmpNames.split(",");     // field enum offset num
    QStringList promptListStr = tmpPrompts.split(",");   // user prompt action
    QStringList lblNumListStr = tmpLabels.split(",");    // label ID numbers

    QStringList namesList;
    QStringList promptList;
    QStringList lblNumList;
    QString tmpStr = "";

    for(int j = 0; j < namesListCount; j++)
    {
        tmpStr = namesListStr.at(j);
        if ( approvedListOfTexts.indexOf(tmpStr) >= 0 )
        {
            namesList.append(  namesListStr.at(j)  );
            promptList.append( promptListStr.at(j) );
            lblNumList.append( lblNumListStr.at(j) );
        }
    }

    // These could of all been placed and updated in the for loop above as
    // each name was found just place the found items in each of these
    // locations but it was made to look really overly complicated so I did this.

    // Set Printer Data for field identifiers on 2nd page
    ui->lblPrinterData0->setText(prDataNamesMap[namesList.at(0).toInt()]);
    ui->lblPrinterData1->setText(prDataNamesMap[namesList.at(1).toInt()]);
    ui->lblPrinterData2->setText(prDataNamesMap[namesList.at(2).toInt()]);
    ui->lblPrinterData3->setText(prDataNamesMap[namesList.at(3).toInt()]);
    ui->lblPrinterData4->setText(prDataNamesMap[namesList.at(4).toInt()]);

    // Set Printer Data for labels data on 2nd page
    ui->txtPrinterData0->setText(lblNumList.at(0));
    ui->txtPrinterData1->setText(lblNumList.at(1));
    ui->txtPrinterData2->setText(lblNumList.at(2));
    ui->txtPrinterData3->setText(lblNumList.at(3));
    ui->txtPrinterData4->setText(lblNumList.at(4));

    // update the combos for the prompt data and set them. Use to do it here but
    // too many other methods needed it, so I pulled it out so everybody can use it
    int indexOne = ui->cboPrinterTypeOne->currentIndex();
    int indexTwo = ui->cboPrinterTypeTwo->currentIndex();

    // Clear them all -> Now we need to decide what to display on setup page
    updateSettingsPageComboNames(indexOne, indexTwo);
}

void setupUtility::refreshPrinterTabData()
{
    // This method is used to reset and set the combo box values of the Settings page
    // after the user Saves Drivers, Adds a driver, or just in general updates the prompt
    // This method was doing too much.  I just need it to fill and select the prompts correctly.

    QString     approvedStrOfTexts  = "1,5,6,9,20";
    QStringList approvedListOfTexts = approvedStrOfTexts.split(",");

    QString tmpNames   = promptness_->extPrinterDataNamesNumList();
    QString tmpPrompts = promptness_->extPrinterDataLabelsPrompt();      // user prompt action
    int numOfPrompts   = promptness_->extPrinterDataLabelsPromptCount();

    // TODO - 2nd printer testing - DELETE WHEN DONE
    //QString tmpNames   = "1,2,3,4,5,6,7,8,9,20";
    //QString tmpPrompts = "9,0,0,0,9,9,0,0,9,9";      // user prompt action
    //int numOfPrompts = 10;

    QStringList namesListStr  = tmpNames.split(",");
    QStringList promptListStr = tmpPrompts.split(",");     // user prompt action
    QStringList promptList;

    QString tmpStr = "";

    for(int j = 0; j < numOfPrompts; j++)
    {
        tmpStr = namesListStr.at(j);
        if ( approvedListOfTexts.indexOf(tmpStr) >= 0 )
            promptList.append( promptListStr.at(j) );
    }

    // Set Printer Data for label actions on combos on 2nd page like Primary, or EXT1, or Prompty

    if ( promptList.count() == 0 )
        return;         // if there isn't a list jump out

    QString tmpPrompt0 = promptList.at(0);
    QString tmpPrompt1 = promptList.at(1);
    QString tmpPrompt2 = promptList.at(2);
    QString tmpPrompt3 = promptList.at(3);
    QString tmpPrompt4 = promptList.at(4);

    int tmpIndex0 = ui->cboPrinterData0->findData( tmpPrompt0 );
    int tmpIndex1 = ui->cboPrinterData0->findData( tmpPrompt1 );
    int tmpIndex2 = ui->cboPrinterData0->findData( tmpPrompt2 );
    int tmpIndex3 = ui->cboPrinterData0->findData( tmpPrompt3 );
    int tmpIndex4 = ui->cboPrinterData0->findData( tmpPrompt4 );

    ui->cboPrinterData0->setCurrentIndex( tmpIndex0 );
    ui->cboPrinterData1->setCurrentIndex( tmpIndex1 );
    ui->cboPrinterData2->setCurrentIndex( tmpIndex2 );
    ui->cboPrinterData3->setCurrentIndex( tmpIndex3 );
    ui->cboPrinterData4->setCurrentIndex( tmpIndex4 );
}

//----------------------------------------
// Get the Scale Model: HTi, HTx, HTs, ...
//----------------------------------------
QString setupUtility::getScaleModel()
{
    LOG_CLASS_INFO( "setupUtility", "getScaleModel" );

    QStringList args;
    QString scaleModel;

    scaleModel = "";

    python pythonProcess;

    if ( pythonProcess.execCommand("getModelName", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            scaleModel = pythonProcess.readStdOut();

            LOG_DEBUG( "Initial Scale Model is %s", qPrintable( scaleModel ) );
        }
        else
        {
            LOG_DEBUG( "Cannot get Scale Model. Exit code = %d", pythonProcess.getExitCode() );
        }
    }
    return scaleModel;
}

// PRINTER SETTINGS TAB METHODS END

bool setupUtility::eventFilter( QObject *obj, QEvent *event )
{
    LOG_CLASS_INFO( "setupUtility", "eventFilter" );
    //LOG_DEBUG( "eventFilter() START", NULL );

    if(QEvent::FocusIn == event->type())
    {
        //qDebug() << "object is" << obj->parent()->objectName();
        if((obj == ui->astronomicalCheckBox || obj == ui->dateEdit || obj == ui->timeZoneCombo || /*obj == ui->setDateTimeBut ||*/ obj->parent()->objectName() == "dateEdit"))
        {
            // un-highlight the timeedit buttons
            ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
            ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
            ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
            ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
        }
        else if(obj == ui->wepKey1LineEdit)
        {
            ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(ui->wepKey1LineEdit->text().count()) + tr(" char."));
        }
        else if(obj == ui->wepKey2LineEdit)
        {
            ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(ui->wepKey2LineEdit->text().count()) + tr(" char."));

        }
        else if(obj == ui->wepKey3LineEdit)
        {
            ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(ui->wepKey3LineEdit->text().count()) + tr(" char."));
        }
        else if(obj == ui->wepKey4LineEdit)
        {
            ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(ui->wepKey4LineEdit->text().count()) + tr(" char."));
        }
    }
    return _Mybase::eventFilter( obj, event );
}

void setupUtility::loadSNMPTab()
{
    LOG_CLASS_INFO( "setupUtility", "loadSNMPTab" );
    LOG_DEBUG( "loadSNMPTab() START", NULL );

    this->ui->snmpManagerVariablesPort->setText(dbSNMPVariablesPort);
    this->ui->snmpManagerTrapsPort->setText(dbSNMPTrapsPort);
    this->ui->snmpManagerTrapsIP->setText(dbSNMPTrapsIP);

    if(dbSNMPEnabled == "1")
    {
        ui->snmpStatusLabel->setText(tr("This SNMP Agent is enabled."));
        ui->snmpStatusLabel->setStyleSheet("color: rgb(26, 105, 26);");
        LOG_DEBUG( "snmp enabled is %s", qPrintable( dbSNMPEnabled ) );
    }
    else
    {
        ui->snmpStatusLabel->setText(tr("This SNMP Agent is disabled."));
        ui->snmpStatusLabel->setStyleSheet("color: rgb(179, 6, 6);");
        LOG_DEBUG( "enabled error is %s", qPrintable( dbSNMPEnabled ) );
    }

    // this->ui->saveSNMPManagerBut->setAutoFillBackground(true);
    // this->ui->saveSNMPManagerBut->setStyleSheet("background-color: rgb(168, 21, 21); color: rgb(255, 255, 255);");
    //  this->connect(this->ui->saveSNMPManagerBut, SIGNAL(clicked()), this, SLOT(snmpManagerBut_clicked()));
}

void setupUtility::loadHostsTab()
{
    LOG_CLASS_INFO( "setupUtility", "loadHostsTab" );
    LOG_DEBUG( "loadHostsTab() START", NULL );

    //processingAButton = true;

    ////////////////////////////
    // SET SYSTEM TAB
    ////////////////////////////
    this->changedHostname = false;
    //this->controlNICs("both", "up");
    this->getUIMACAddress();
    this->getUIHostname();

    ui->setScalePasswordBut->setEnabled( false ); // not enabled until they type something

    QRegExp validHostnameRegEx("^([a-zA-Z]+[a-zA-Z0-9\\-\\.]*[a-zA-Z0-9]+)|([a-zA-Z])$");
    QValidator* hostnameValidator;
    hostnameValidator = new QRegExpValidator(validHostnameRegEx, this);
    ui->hostnameLineEdit->setValidator(hostnameValidator);
    //QStringList specialCharArray = { '`', '~', '!', '@', '#', '$', '^', '&', '*', '(', ')', '=', '+', '[', ']', '{', '}', '\\', '|', ';', ':', '\'', '"', ',', '<', '>', '/', '?' };

    this->connect(this->ui->hostnameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAgainstPoorServiceTraining(QString)));
    //this->ui->pingIPLineEdit->setText("www.hobartcorp.com");
    this->connect(this->ui->setHostnameBut, SIGNAL(clicked()), this, SLOT(setHostnameBut_click()));
    this->connect(this->ui->startPingBut, SIGNAL(clicked()), this, SLOT(startPingBut_click()));
    this->connect(this->ui->stopPingBut, SIGNAL(clicked()), this, SLOT(stopPingBut_click()));
    this->connect(pingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printPingOutput()));
    this->connect(pingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int,QProcess::ExitStatus)));
    this->connect(pingProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
    this->connect(this->ui->scalePasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAgainstEmptyPassword(QString)));
    this->connect(this->ui->setScalePasswordBut, SIGNAL(clicked()), this, SLOT(setScalePasswordBut_click()));
    this->connect(this->ui->DHCPHostnameCheckbox, SIGNAL(stateChanged(int)), this, SLOT(setHostnamefromDHCPToggled(int)));
    //processingAButton = false;

    if(dbDHCPHostname == "1")
    {
        LOG_DEBUG( "loadHostsTab() hostnameLineEdit->setEnabled(false)", NULL  );
        ui->hostnameLineEdit->setEnabled(false);
    }
    else
    {
        LOG_DEBUG( "loadHostsTab() hostnameLineEdit->setEnabled(true)", NULL  );
        ui->hostnameLineEdit->setEnabled(true);
    }
}

void setupUtility::setHostnamefromDHCPToggled(int newVal)
{
    LOG_CLASS_INFO( "setupUtility", "setHostnamefromDHCPToggled" );
    LOG_DEBUG( "setHostnamefromDHCPToggled() START", NULL );
    Q_UNUSED( newVal );

    if( this->ui->DHCPHostnameCheckbox->isChecked() )
    {
        LOG_DEBUG( "hostnameLineEdit->setEnabled(false)", NULL );
        this->ui->hostnameLineEdit->setEnabled(false);
        this->ui->setHostnameBut->setEnabled(false);
        this->ui->DHCPHostnameWarning->show();
    }
    else
    {
        LOG_DEBUG( "hostnameLineEdit->setEnabled(true)", NULL );
        this->ui->hostnameLineEdit->setEnabled(true);
        this->ui->setHostnameBut->setEnabled(true);
        this->ui->DHCPHostnameWarning->hide();
    }
}

void setupUtility::hourBut_clicked()
{
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(0);
}

void setupUtility::minuteBut_clicked()
{
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->timeEdit->setCurrentSectionIndex(1);
}

void setupUtility::secondBut_clicked()
{
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(2);
}

void setupUtility::ampmBut_clicked()
{
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(3);
}

void setupUtility::timeEditTimeChanged(QTime)
{
    if(ui->timeEdit->currentSection() == ui->timeEdit->HourSection)
    {
        // hour changed
        ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->HourSection));
        hourBut_clicked();
    }
    else if(ui->timeEdit->currentSection() == ui->timeEdit->MinuteSection)
    {
        // minute changed
        ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->MinuteSection));
        minuteBut_clicked();
    }
    else if(ui->timeEdit->currentSection() == ui->timeEdit->SecondSection)
    {
        // second changed
        ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->SecondSection));
        secondBut_clicked();
    }
    else if(ui->timeEdit->currentSection() == ui->timeEdit->AmPmSection)
    {
        // hour changed
        ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->AmPmSection));
        ampmBut_clicked();
    }
}

void setupUtility::ntpHourBut_clicked()
{
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(0);
}

void setupUtility::ntpMinuteBut_clicked()
{
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpTimeEdit->setCurrentSectionIndex(1);
}

void setupUtility::ntpSecondBut_clicked()
{
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(2);
}

void setupUtility::ntpAmpmBut_clicked()
{
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(3);
}

void setupUtility::ntpTimeEditTimeChanged(QTime)
{
    if(ui->ntpTimeEdit->currentSection() == ui->ntpTimeEdit->HourSection)
    {
        // hour changed
        ui->ntpHourBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->HourSection));
        ntpHourBut_clicked();
    }
    else if(ui->ntpTimeEdit->currentSection() == ui->ntpTimeEdit->MinuteSection)
    {
        // minute changed
        ui->ntpMinuteBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->MinuteSection));
        ntpMinuteBut_clicked();
    }
    else if(ui->ntpTimeEdit->currentSection() == ui->ntpTimeEdit->SecondSection)
    {
        // second changed
        ui->ntpSecondBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->SecondSection));
        ntpSecondBut_clicked();
    }
    else if(ui->ntpTimeEdit->currentSection() == ui->ntpTimeEdit->AmPmSection)
    {
        // hour changed
        ui->ntpAmpmBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->AmPmSection));
        ntpAmpmBut_clicked();
    }
}

//////////////////////////////////////////////////////
//
//        D E S T R U C T O R
//
//////////////////////////////////////////////////////

setupUtility::~setupUtility()
{
    LOG_CLASS_INFO( "setupUtility", "setupUtility" );
    LOG_DEBUG( "setupUtility() DESTRUCTOR START", NULL );

    //takeDownNICs();
    if(mustTurnOnDialogs)
    {
       session_->send(SessionDev , SnRestartDialogs);
      // session_->send( NetworkConnectivityDev, RequestCurrentStatus );
       mustTurnOnDialogs = false;
    }

    if(changedHostname /*|| changedSpeakers*/)
    {
        LOG_WARN("shutdown button has been pressed", NULL);

        if (isDemoMode)
        {
            LOG_INFO("closing main window...", NULL);
            //mainWindow_->close();
            delete ui;
        }
        else
        {
            //QString body("100047,5");
            //LOG_DEBUG("sending shutdown message...", NULL);

            //session_->send(Error, ErrSessionDevice, body);
            shouldChangeTime = false;
            QString msg = "120080,3";
            session_->send(Error, ErrVersionManagerDevice, msg);
            // Session::send(Error,ErrDevi)
        }
    }
    else
    {
        delete ui;
        player->deleteLater();
    }

    // update script data used in manufacturing test
    session_->scaleConfig->updateScriptData(1);
}

void setupUtility::updateCharacterCount(QString incomingString)
{
    ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(incomingString.count()) + tr(" char."));
}

void setupUtility::loadNetworkTab()
{
    LOG_CLASS_INFO( "setupUtility", "loadNetworkTab" );
    LOG_DEBUG( "loadNetworkTab() START", NULL );

    processingAButton = true;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    if(weHaveServiceMode || settings->rebootUnprotectedAllowed())
    {

        this->ui->networkLoadingLabel->setText(tr("Network\nLoading"));
        this->ui->noGatewayBut->hide();
        this->ui->yesGatewayBut->hide();
        this->ui->newWiredNetworkBut->hide();
        this->ui->newWirelessNetworktBut->hide();
        this->ui->networkLoadingStatusLabel->show();
        this->ui->gbNetworkLoading->show();
        this->ui->networkStatusLabel->show();
        qApp->processEvents();
        ui->gcdLabel->hide();
        ui->gcdSlider->setDisabled(true);
        ui->gcdSlider->setEnabled(false);
        ui->gcdSlider->hide();
        ui->gcdNoLabel->hide();
        ui->gcdYesLabel->hide();
        this->powerDevices("11");
        commonNetwork = new Network();
        if(isLinux())
        {
            //QPixmap pixmap(":/images/screens/refresh.png");
            //QIcon refreshButtonIcon(pixmap);
            //this->ui->refreshNetworksBut->setIcon(refreshButtonIcon);
            this->ui->refreshNetworksBut->setStyleSheet("	border-image: url(:/images/screens/refresh.png) 0 0 0 0 stretch stretch;");
        }
        else
        {
            //windows draws buttons slightly larger so
            //QPixmap pixmapWin(":/images/screens/refreshHTx.png");
            //QIcon refreshButtonIconWin(pixmapWin);
           // this->ui->refreshNetworksBut->setIcon(refreshButtonIconWin);
            this->ui->refreshNetworksBut->setStyleSheet("	border-image: url(:/images/screens/refresh.png) 0 0 0 0 stretch stretch;");
        }
        this->networkCount = 2;
        this->howMany = 0;

        //VALIDATORS FOR IP OCTET BLOCKS
        ui->ipAddressLineEdit->setValidator(ipOctet_ = new QIntValidator(0, INT_MAX, this));
        ui->ipAddressLineEdit_2->setValidator(ipOctet_);
        ui->ipAddressLineEdit_3->setValidator(ipOctet_);
        ui->ipAddressLineEdit_4->setValidator(ipOctet_);

        ui->subnetMaskLineEdit->setValidator(ipOctet_);
        ui->subnetMaskLineEdit_2->setValidator(ipOctet_);
        ui->subnetMaskLineEdit_3->setValidator(ipOctet_);
        ui->subnetMaskLineEdit_4->setValidator(ipOctet_);

        ui->gatewayLineEdit->setValidator(ipOctet_);
        ui->gatewayLineEdit_2->setValidator(ipOctet_);
        ui->gatewayLineEdit_3->setValidator(ipOctet_);
        ui->gatewayLineEdit_4->setValidator(ipOctet_);

        ui->DNS1LineEdit->setValidator(ipOctet_);
        ui->DNS1LineEdit_2->setValidator(ipOctet_);
        ui->DNS1LineEdit_3->setValidator(ipOctet_);
        ui->DNS1LineEdit_4->setValidator(ipOctet_);

        ui->DNS2LineEdit->setValidator(ipOctet_);
        ui->DNS2LineEdit_2->setValidator(ipOctet_);
        ui->DNS2LineEdit_3->setValidator(ipOctet_);
        ui->DNS2LineEdit_4->setValidator(ipOctet_);

        ui->DNS3LineEdit->setValidator(ipOctet_);
        ui->DNS3LineEdit_2->setValidator(ipOctet_);
        ui->DNS3LineEdit_3->setValidator(ipOctet_);
        ui->DNS3LineEdit_4->setValidator(ipOctet_);

        ui->ipAddressLineEdit->setDecimalPlaces(0);
        ui->ipAddressLineEdit->setInputWrapping(true);

        ui->ipAddressLineEdit_2->setDecimalPlaces(0);
        ui->ipAddressLineEdit_2->setInputWrapping(true);

        ui->ipAddressLineEdit_3->setDecimalPlaces(0);
        ui->ipAddressLineEdit_3->setInputWrapping(true);

        ui->ipAddressLineEdit_4->setDecimalPlaces(0);
        ui->ipAddressLineEdit_4->setInputWrapping(true);

        ui->subnetMaskLineEdit->setDecimalPlaces(0);
        ui->subnetMaskLineEdit->setInputWrapping(true);

        ui->subnetMaskLineEdit_2->setDecimalPlaces(0);
        ui->subnetMaskLineEdit_2->setInputWrapping(true);

        ui->subnetMaskLineEdit_3->setDecimalPlaces(0);
        ui->subnetMaskLineEdit_3->setInputWrapping(true);

        ui->subnetMaskLineEdit_4->setDecimalPlaces(0);
        ui->subnetMaskLineEdit_4->setInputWrapping(true);

        ui->gatewayLineEdit->setDecimalPlaces(0);
        ui->gatewayLineEdit->setInputWrapping(true);

        ui->gatewayLineEdit_2->setDecimalPlaces(0);
        ui->gatewayLineEdit_2->setInputWrapping(true);

        ui->gatewayLineEdit_3->setDecimalPlaces(0);
        ui->gatewayLineEdit_3->setInputWrapping(true);

        ui->gatewayLineEdit_4->setDecimalPlaces(0);
        ui->gatewayLineEdit_4->setInputWrapping(true);

        ui->DNS1LineEdit->setDecimalPlaces(0);
        ui->DNS1LineEdit->setInputWrapping(true);

        ui->DNS1LineEdit_2->setDecimalPlaces(0);
        ui->DNS1LineEdit_2->setInputWrapping(true);

        ui->DNS1LineEdit_3->setDecimalPlaces(0);
        ui->DNS1LineEdit_3->setInputWrapping(true);

        ui->DNS1LineEdit_4->setDecimalPlaces(0);
        ui->DNS1LineEdit_4->setInputWrapping(true);

        ui->DNS2LineEdit->setDecimalPlaces(0);
        ui->DNS2LineEdit->setInputWrapping(true);

        ui->DNS2LineEdit_2->setDecimalPlaces(0);
        ui->DNS2LineEdit_2->setInputWrapping(true);

        ui->DNS2LineEdit_3->setDecimalPlaces(0);
        ui->DNS2LineEdit_3->setInputWrapping(true);

        ui->DNS2LineEdit_4->setDecimalPlaces(0);
        ui->DNS2LineEdit_4->setInputWrapping(true);

        ui->DNS3LineEdit->setDecimalPlaces(0);
        ui->DNS3LineEdit->setInputWrapping(true);

        ui->DNS3LineEdit_2->setDecimalPlaces(0);
        ui->DNS3LineEdit_2->setInputWrapping(true);

        ui->DNS3LineEdit_3->setDecimalPlaces(0);
        ui->DNS3LineEdit_3->setInputWrapping(true);

        ui->DNS3LineEdit_4->setDecimalPlaces(0);
        ui->DNS3LineEdit_4->setInputWrapping(true);

        ipOctet_->setTop(255);
        ipOctet_->setBottom(0);

        this->adapterAlreadyConnected = false;

        //this->getWirelessAPs();
        this->loadNetworkConnections();
        this->ui->securityCombo->addItem("WPA2-Enterprise (LEAP)",QString("WPA2-Enterprise"));
        this->loadWirelessSecurityMethodsCombo();
        this->ui->autoDNSRadio->setChecked(true);
        this->ui->dhcpRadio->setChecked(true);

        this->connect(this->ui->noGatewayBut, SIGNAL(clicked()), this, SLOT(gatewayNoSlot()));
        this->connect(this->ui->yesGatewayBut, SIGNAL(clicked()), this, SLOT(gatewayYesSlot()));



        this->connect(saveEAPConfigurationProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printEAPOutput()));
        this->connect(saveEAPConfigurationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedEAPProcess(int,QProcess::ExitStatus)));
        this->connect(saveEAPConfigurationProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadEAPStandardError()));

        this->connect(saveNetworkConfigurationProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printNetworkOutput()));
        this->connect(saveNetworkConfigurationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedNetworkProcess(int,QProcess::ExitStatus)));
        this->connect(saveNetworkConfigurationProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadNetworkStandardError()));
        this->connect(this->ui->refreshNetworksBut, SIGNAL(clicked()), this, SLOT(refreshNetworksButton_click()));
        this->connect(this->ui->quitBut, SIGNAL(clicked()), qApp, SLOT(quit()));
        this->connect(this->ui->exportBut, SIGNAL(clicked()), this, SLOT(exportNetworkBut_click()));
        //this->connect(this->ui->importBut, SIGNAL(clicked()), this, SLOT(importNetworkBut_click()));
        this->connect(this->ui->importBut, SIGNAL(clicked()), this, SLOT(importNetworkSettings()));
        this->connect(this->ui->addNetworkBut, SIGNAL(clicked()), this, SLOT(addNetworkBut_click()));
        this->connect(this->ui->networkSaveBut, SIGNAL(clicked()), this, SLOT(saveNetworkBut_click()));
        this->connect(this->ui->securityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setWirelessNetworkEncryptionCombo(int)));
        this->connect(this->ui->encryptionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setWirelessFrames(int)));
        this->connect(this->ui->dhcpRadio, SIGNAL(clicked()), this, SLOT(dhcpRadio_disableIPs()));
        this->connect(this->ui->dhcpRadio, SIGNAL(toggled(bool)), this, SLOT(dhcpRadio_disableIPs()));
        this->connect(this->ui->staticIPRadio, SIGNAL(clicked()), this, SLOT(staticIPRadio_enableIPs()));
        this->connect(this->ui->autoDNSRadio, SIGNAL(clicked()), this, SLOT(autoDNSRadio_click()));
        this->connect(this->ui->autoDNSRadio, SIGNAL(toggled(bool)), this, SLOT(autoDNSRadio_click()));
        this->connect(this->ui->manualDNSRadio, SIGNAL(clicked()), this, SLOT(manualDNSRadio_click()));
        this->connect(this->ui->availableConnectionsTableView, SIGNAL(cellPressed(int,int)), this, SLOT(networkTableWidgetSelectionChanged(int)));
        this->connect(this->ui->newWiredNetworkBut, SIGNAL(clicked()), this, SLOT(newWiredNetworkBut_click()));
        this->connect(this->ui->newWirelessNetworktBut, SIGNAL(clicked()), this, SLOT(newWirelessNetworkBut_click()));
        this->connect(this->ui->wepKey1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateCharacterCount(QString)));
        this->connect(this->ui->wepKey2LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateCharacterCount(QString)));
        this->connect(this->ui->wepKey3LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateCharacterCount(QString)));
        this->connect(this->ui->wepKey4LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateCharacterCount(QString)));
        this->connect(this->ui->serverCertBut, SIGNAL(clicked()), this, SLOT(serverCertButClicked()));
        //this->connect(this->ui->chainFileBut, SIGNAL(clicked()), this, SLOT(chainFileButClicked()));
        this->connect(this->ui->clientCertBut, SIGNAL(clicked()), this, SLOT(clientCertButClicked()));
        this->connect(this->ui->serverCertDeleteBut, SIGNAL(clicked()), this, SLOT(serverCertDeleteButClicked()));
        this->connect(this->ui->deleteClientCertYesBut,SIGNAL(clicked()), this, SLOT(deleteClientCertYesBut_clicked()));
        this->connect(this->ui->deleteClientCertCancelBut,SIGNAL(clicked()), this, SLOT(deleteClientCertCancelBut_clicked()));
        this->connect(this->ui->deleteServerCertYesBut, SIGNAL(clicked()), this, SLOT(deleteServerCertYesBut_clicked()));
        this->connect(this->ui->deleteServerCertCancelBut,SIGNAL(clicked()), this, SLOT(deleteServerCertCancelBut_clicked()));
        //this->connect(this->ui->chainFileBut, SIGNAL(clicked()), this, SLOT(chainFileButClicked()));
        this->connect(this->ui->clientCertDeleteBut, SIGNAL(clicked()), this, SLOT(clientCertDeleteButClicked()));
      //  this->connect(this->ui->signingRequestBut, SIGNAL(clicked()), this, SLOT(signingRequestButClicked()));
        this->connect(this->ui->trustedRootStoreCombo, SIGNAL(currentIndexChanged(int)) , this, SLOT(updateServerCertFilename(int)));
        this->connect(this->ui->clientCertStoreCombo, SIGNAL(currentIndexChanged(int)) , this, SLOT(updateClientCertFilename(int)));
        this->connect(this->ui->searchTrustedRootCertBut, SIGNAL(clicked()), this, SLOT(searchTrustedRootCertButClicked()));
        this->connect(this->ui->searchClientCertBut, SIGNAL(clicked()), this, SLOT(searchClientCertButClicked()));
        this->connect(this->ui->closeCertInfoBut, SIGNAL(clicked()), this, SLOT(closeCertInfoButClicked()));
        this->connect(this->ui->refreshNetworksBut, SIGNAL(pressed()), this, SLOT(refreshNetworksPressed()));
        this->connect(this->ui->networkAdvancedBut, SIGNAL(clicked()), this, SLOT(advancedNetworkingBut_clicked()));
        this->connect(this->ui->advancedNetworkingApplyBut, SIGNAL(clicked()), this, SLOT(advancedNetworkingApplyBut_clicked()));
        this->connect(this->ui->advancedNetworkingDefaultsBut, SIGNAL(clicked()), this, SLOT(advancedNetworkingDefaultsBut_clicked()));
        this->connect(this->ui->advancedNetworkingCancelBut, SIGNAL(clicked()), this, SLOT(advancedNetworkingCancelBut_clicked()));
        this->connect(this->ui->cbTransferRate, SIGNAL(currentIndexChanged(int)), this, SLOT(advancedNetworkingLinkRateCombo_changed(int)));

        if(isDemoMode)
        {
            this->ui->availableConnectionsTableView->selectRow(0);

            this->revealNetworkGroupBox(3,0);
        }
        //this->getConnectionStatus();

        ui->cbTransferRate->clear();
        ui->cbTransferRate->addItem(" Automatic",QString("auto"));
        ui->cbTransferRate->addItem(" 1 Mbps",QString("1M"));
        ui->cbTransferRate->addItem(" 11 Mbps  - Mode B",QString("11M"));
        ui->cbTransferRate->addItem(" 24 Mbps",QString("24M"));
        ui->cbTransferRate->addItem(" 36 Mbps",QString("36M"));
        ui->cbTransferRate->addItem(" 48 Mbps",QString("48M"));
        ui->cbTransferRate->addItem(" 54 Mbps  - Mode G ",QString("54M"));
        ui->cbTransferRate->addItem(" 150 Mbps - Mode N", QString("149M"));

        ui->cbChannel->clear();
        ui->cbChannel->addItem(tr("Auto : Roaming"), "auto");
        ui->cbChannel->addItem(" Channel 1 : 2.412 GHz", "2412");
        ui->cbChannel->addItem(" Channel 2 : 2.417 GHz", "2417");
        ui->cbChannel->addItem(" Channel 3 : 2.422 GHz", "2422");
        ui->cbChannel->addItem(" Channel 4 : 2.427 GHz", "2427");
        ui->cbChannel->addItem(" Channel 5 : 2.432 GHz", "2432");
        ui->cbChannel->addItem(" Channel 6 : 2.437 GHz", "2437");
        ui->cbChannel->addItem(" Channel 7 : 2.442 GHz", "2442");
        ui->cbChannel->addItem(" Channel 8 : 2.447 GHz", "2447");
        ui->cbChannel->addItem(" Channel 9 : 2.452 GHz", "2452");
        ui->cbChannel->addItem(" Channel 10 : 2.457 GHz", "2457");
        ui->cbChannel->addItem(" Channel 11 : 2.462 GHz", "2462");
        ui->cbChannel->addItem(" Channel 36 : 5.180 GHz", "5180");
        ui->cbChannel->addItem(" Channel 38 : 5.190 GHz", "5190");
        ui->cbChannel->addItem(" Channel 40 : 5.200 GHz", "5200");
        ui->cbChannel->addItem(" Channel 42 : 5.210 GHz", "5210");
        ui->cbChannel->addItem(" Channel 44 : 5.220 GHz", "5220");
        ui->cbChannel->addItem(" Channel 46 : 5.230 GHz", "5230");
        ui->cbChannel->addItem(" Channel 48 : 5.240 GHz", "5240");
        ui->cbChannel->addItem(" Channel 50 : 5.250 GHz", "5250");
        ui->cbChannel->addItem(" Channel 52 : 5.260 GHz", "5260");
        ui->cbChannel->addItem(" Channel 54 : 5.270 GHz", "5270");
        ui->cbChannel->addItem(" Channel 56 : 5.280 GHz", "5280");
        ui->cbChannel->addItem(" Channel 58 : 5.290 GHz", "5290");
        ui->cbChannel->addItem(" Channel 60 : 5.300 GHz", "5300");
        ui->cbChannel->addItem(" Channel 62 : 5.310 GHz", "5310");
        ui->cbChannel->addItem(" Channel 64 : 5.320 GHz", "5320");
        ui->cbChannel->addItem(" Channel 100 : 5.500 GHz", "5500");
        ui->cbChannel->addItem(" Channel 102 : 5.510 GHz", "5510");
        ui->cbChannel->addItem(" Channel 104 : 5.520 GHz", "5520");
        ui->cbChannel->addItem(" Channel 106 : 5.530 GHz", "5530");
        ui->cbChannel->addItem(" Channel 108 : 5.540 GHz", "5540");
        ui->cbChannel->addItem(" Channel 110 : 5.550 GHz", "5550");
        ui->cbChannel->addItem(" Channel 112 : 5.560 GHz", "5560");
        ui->cbChannel->addItem(" Channel 114 : 5.570 GHz", "5570");
        ui->cbChannel->addItem(" Channel 116 : 5.580 GHz", "5580");
        ui->cbChannel->addItem(" Channel 118 : 5.590 GHz", "5590");
        ui->cbChannel->addItem(" Channel 120 : 5.600 GHz", "5600");
        ui->cbChannel->addItem(" Channel 122 : 5.610 GHz", "5610");
        ui->cbChannel->addItem(" Channel 124 : 5.620 GHz", "5620");
        ui->cbChannel->addItem(" Channel 126 : 5.630 GHz", "5630");
        ui->cbChannel->addItem(" Channel 128 : 5.640 GHz", "5640");
        ui->cbChannel->addItem(" Channel 132 : 5.660 GHz", "5660");
        ui->cbChannel->addItem(" Channel 134 : 5.670 GHz", "5670");
        ui->cbChannel->addItem(" Channel 136 : 5.680 GHz", "5680");
        ui->cbChannel->addItem(" Channel 138 : 5.690 GHz", "5690");
        ui->cbChannel->addItem(" Channel 140 : 5.700 GHz", "5700");
        ui->cbChannel->addItem(" Channel 142 : 5.710 GHz", "5710");
        ui->cbChannel->addItem(" Channel 144 : 5.720 GHz", "5720");
        ui->cbChannel->addItem(" Channel 149 : 5.745 GHz", "5745");
        ui->cbChannel->addItem(" Channel 151 : 5.755 GHz", "5755");
        ui->cbChannel->addItem(" Channel 153 : 5.765 GHz", "5765");
        ui->cbChannel->addItem(" Channel 155 : 5.775 GHz", "5775");
        ui->cbChannel->addItem(" Channel 157 : 5.785 GHz", "5785");
        ui->cbChannel->addItem(" Channel 159 : 5.795 GHz", "5795");
        ui->cbChannel->addItem(" Channel 161 : 5.805 GHz", "5805");
        ui->cbChannel->addItem(" Channel 165 : 5.825 GHz", "5825");

        this->getUIMACAddress();

        //this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
        if(dbConnectionType == "0")
        {
            this->revealNetworkGroupBox(1,0);
            finalIsWireless =false;
        }
        else
        {
            finalIsWireless = true;
            this->revealNetworkGroupBox(3,0);
            ui->SSIDLineEdit->setText(dbConnectionName);
            //TBD preselect based on security and encryption type too
        }

        QStringList IPAddressParts;
        QStringList SubnetMaskParts;
        QStringList GatewayParts;
        QStringList DNS1Parts;
        QStringList DNS2Parts;
        QStringList DNS3Parts;

        if(dbipAddress != "0.0.0.0" && dbipAddress != "64641964" && dbipAddress != "")
        {
            IPAddressParts = dbipAddress.split(".", QString::KeepEmptyParts);

            if(IPAddressParts.size() == 4)
            {
                ui->ipAddressLineEdit->setText(IPAddressParts.at(0));
                ui->ipAddressLineEdit_2->setText(IPAddressParts.at(1));
                ui->ipAddressLineEdit_3->setText(IPAddressParts.at(2));
                ui->ipAddressLineEdit_4->setText(IPAddressParts.at(3));
            }
        }
        else
        {
            ui->ipAddressLineEdit->setText("");
            ui->ipAddressLineEdit_2->setText("");
            ui->ipAddressLineEdit_3->setText("");
            ui->ipAddressLineEdit_4->setText("");
        }

        if(dbnetmask != "0.0.0.0" && dbnetmask != "64641964" && dbnetmask != "")
        {
            SubnetMaskParts = dbnetmask.split(".", QString::KeepEmptyParts);

            if(SubnetMaskParts.size() == 4)
            {
                ui->subnetMaskLineEdit->setText(SubnetMaskParts.at(0));
                ui->subnetMaskLineEdit_2->setText(SubnetMaskParts.at(1));
                ui->subnetMaskLineEdit_3->setText(SubnetMaskParts.at(2));
                ui->subnetMaskLineEdit_4->setText(SubnetMaskParts.at(3));
            }
        }
        else
        {
            ui->subnetMaskLineEdit->setText("");
            ui->subnetMaskLineEdit_2->setText("");
            ui->subnetMaskLineEdit_3->setText("");
            ui->subnetMaskLineEdit_4->setText("");
        }

        if(dbgateway != "0.0.0.0" && dbgateway != "64641964" && dbgateway != "")
        {
            GatewayParts = dbgateway.split(".", QString::KeepEmptyParts);

            if(GatewayParts.size() == 4)
            {
                ui->gatewayLineEdit->setText(GatewayParts.at(0));
                ui->gatewayLineEdit_2->setText(GatewayParts.at(1));
                ui->gatewayLineEdit_3->setText(GatewayParts.at(2));
                ui->gatewayLineEdit_4->setText(GatewayParts.at(3));
            }
        }
        else
        {
            ui->gatewayLineEdit->setText("");
            ui->gatewayLineEdit_2->setText("");
            ui->gatewayLineEdit_3->setText("");
            ui->gatewayLineEdit_4->setText("");
        }

        if(dbdns1 != "0.0.0.0" && dbdns1 != "64641964" && dbdns1 != "")
        {
            DNS1Parts = dbdns1.split(".", QString::KeepEmptyParts);

            if(DNS1Parts.size() == 4)
            {
                ui->DNS1LineEdit->setText(DNS1Parts.at(0));
                ui->DNS1LineEdit_2->setText(DNS1Parts.at(1));
                ui->DNS1LineEdit_3->setText(DNS1Parts.at(2));
                ui->DNS1LineEdit_4->setText(DNS1Parts.at(3));
            }
        }
        else
        {
            ui->DNS1LineEdit->setText("");
            ui->DNS1LineEdit_2->setText("");
            ui->DNS1LineEdit_3->setText("");
            ui->DNS1LineEdit_4->setText("");
        }

        if(dbdns2 != "0.0.0.0" && dbdns2 != "64641964" && dbdns2 != "")
        {
            DNS2Parts = dbdns2.split(".", QString::KeepEmptyParts);

            if(DNS2Parts.size() == 4)
            {
                ui->DNS2LineEdit->setText(DNS2Parts.at(0));
                ui->DNS2LineEdit_2->setText(DNS2Parts.at(1));
                ui->DNS2LineEdit_3->setText(DNS2Parts.at(2));
                ui->DNS2LineEdit_4->setText(DNS2Parts.at(3));
            }
        }
        else
        {
            ui->DNS2LineEdit->setText("");
            ui->DNS2LineEdit_2->setText("");
            ui->DNS2LineEdit_3->setText("");
            ui->DNS2LineEdit_4->setText("");
        }

        if(dbdns3 != "0.0.0.0" && dbdns3 != "64641964" && dbdns3 != "")
        {
            DNS3Parts = dbdns3.split(".", QString::KeepEmptyParts);

            if(DNS3Parts.size() == 4)
            {
                ui->DNS3LineEdit->setText(DNS3Parts.at(0));
                ui->DNS3LineEdit_2->setText(DNS3Parts.at(1));
                ui->DNS3LineEdit_3->setText(DNS3Parts.at(2));
                ui->DNS3LineEdit_4->setText(DNS3Parts.at(3));
            }
        }
        else
        {
            ui->DNS3LineEdit->setText("");
            ui->DNS3LineEdit_2->setText("");
            ui->DNS3LineEdit_3->setText("");
            ui->DNS3LineEdit_4->setText("");
        }

        if (dbisStatic == "1")
        {
            ui->staticIPRadio->setChecked(true);
            ui->ipAddressLineEdit->setEnabled(true);
            ui->ipAddressLineEdit_2->setEnabled(true);
            ui->ipAddressLineEdit_3->setEnabled(true);
            ui->ipAddressLineEdit_4->setEnabled(true);

            ui->subnetMaskLineEdit->setEnabled(true);
            ui->subnetMaskLineEdit_2->setEnabled(true);
            ui->subnetMaskLineEdit_3->setEnabled(true);
            ui->subnetMaskLineEdit_4->setEnabled(true);

            ui->gatewayLineEdit->setEnabled(true);
            ui->gatewayLineEdit_2->setEnabled(true);
            ui->gatewayLineEdit_3->setEnabled(true);
            ui->gatewayLineEdit_4->setEnabled(true);

            ui->autoDNSRadio->setEnabled(false);
            //LOG_DEBUG( "loadNetworkTab() DHCPHostnameCheckbox->setEnabled(false) 1", NULL );
            ui->DHCPHostnameCheckbox->setEnabled(false);
            ui->DHCPHostnameWarning->hide();
            //LOG_DEBUG( "loadNetworkTab() DHCPHostnameCheckbox->setChecked(false) 1", NULL );
            ui->DHCPHostnameCheckbox->setChecked(false);
            dhcpRadio_disableIPs();
        }
        else
        {
            ui->dhcpRadio->setChecked(true);
            ui->ipAddressLineEdit->setDisabled(true);
            ui->ipAddressLineEdit_2->setDisabled(true);
            ui->ipAddressLineEdit_3->setDisabled(true);
            ui->ipAddressLineEdit_4->setDisabled(true);

            ui->subnetMaskLineEdit->setDisabled(true);
            ui->subnetMaskLineEdit_2->setDisabled(true);
            ui->subnetMaskLineEdit_3->setDisabled(true);
            ui->subnetMaskLineEdit_4->setDisabled(true);

            ui->gatewayLineEdit->setDisabled(true);
            ui->gatewayLineEdit_2->setDisabled(true);
            ui->gatewayLineEdit_3->setDisabled(true);
            ui->gatewayLineEdit_4->setDisabled(true);

            ui->autoDNSRadio->setEnabled(true);
            ui->manualDNSRadio->setEnabled(true);
            //LOG_DEBUG( "loadNetworkTab() DHCPHostnameCheckbox->setEnabled(true) 2", NULL );
            ui->DHCPHostnameCheckbox->setEnabled(true);

            if(dbDHCPHostname == "1")
            {
                //LOG_DEBUG( "loadNetworkTab() DHCPHostnameCheckbox->setChecked(true) 2", NULL );
                ui->DHCPHostnameCheckbox->setChecked(true);
                ui->DHCPHostnameWarning->show();
                ui->setHostnameBut->setEnabled(false);
                LOG_DEBUG( "hostnameLineEdit->setEnabled(false)", NULL  );
                ui->hostnameLineEdit->setEnabled(false);
            }
            else
            {
                //LOG_DEBUG( "loadNetworkTab() DHCPHostnameCheckbox->setChecked(false) 3", NULL );
                ui->DHCPHostnameCheckbox->setChecked(false);
                ui->DHCPHostnameWarning->hide();
                ui->setHostnameBut->setEnabled(true);
                LOG_DEBUG( "hostnameLineEdit->setEnabled(true)", NULL  );
                ui->hostnameLineEdit->setEnabled(true);
            }
            dhcpRadio_disableIPs();
        }

        //qDebug() << "dn1 is: " << dbdns1;
        //qDebug() << "dn2 is: " << dbdns2;
        //qDebug() << "dn3 is: " << dbdns3;
        //ui->DNS1LineEdit->setText(dbdns1);
        //ui->DNS2LineEdit->setText(dbdns2);
        //ui->DNS3LineEdit->setText(dbdns3);

        if(dbautoDNS == "1")
        {
            ui->autoDNSRadio->setChecked(true);
            ui->DNS1LineEdit->setDisabled(true);
            ui->DNS1LineEdit_2->setDisabled(true);
            ui->DNS1LineEdit_3->setDisabled(true);
            ui->DNS1LineEdit_4->setDisabled(true);

            ui->DNS1LineEdit->setEnabled(false);
            ui->DNS1LineEdit_2->setEnabled(false);
            ui->DNS1LineEdit_3->setEnabled(false);
            ui->DNS1LineEdit_4->setEnabled(false);

            ui->DNS2LineEdit->setDisabled(true);
            ui->DNS2LineEdit_2->setDisabled(true);
            ui->DNS2LineEdit_3->setDisabled(true);
            ui->DNS2LineEdit_4->setDisabled(true);

            ui->DNS2LineEdit->setEnabled(false);
            ui->DNS2LineEdit_2->setEnabled(false);
            ui->DNS2LineEdit_3->setEnabled(false);
            ui->DNS2LineEdit_4->setEnabled(false);

            ui->DNS3LineEdit->setDisabled(true);
            ui->DNS3LineEdit_2->setDisabled(true);
            ui->DNS3LineEdit_3->setDisabled(true);
            ui->DNS3LineEdit_4->setDisabled(true);

            ui->DNS3LineEdit->setEnabled(false);
            ui->DNS3LineEdit_2->setEnabled(false);
            ui->DNS3LineEdit_3->setEnabled(false);
            ui->DNS3LineEdit_4->setEnabled(false);

            autoDNSRadio_click();
        }
        else
        {
            ui->manualDNSRadio->setChecked(true);
            ui->DNS1LineEdit->setDisabled(false);
            ui->DNS1LineEdit_2->setDisabled(false);
            ui->DNS1LineEdit_3->setDisabled(false);
            ui->DNS1LineEdit_4->setDisabled(false);

            ui->DNS1LineEdit->setEnabled(true);
            ui->DNS1LineEdit_2->setEnabled(true);
            ui->DNS1LineEdit_3->setEnabled(true);
            ui->DNS1LineEdit_4->setEnabled(true);

            ui->DNS2LineEdit->setDisabled(false);
            ui->DNS2LineEdit_2->setDisabled(false);
            ui->DNS2LineEdit_3->setDisabled(false);
            ui->DNS2LineEdit_4->setDisabled(false);

            ui->DNS2LineEdit->setEnabled(true);
            ui->DNS2LineEdit_2->setEnabled(true);
            ui->DNS2LineEdit_3->setEnabled(true);
            ui->DNS2LineEdit_4->setEnabled(true);

            ui->DNS3LineEdit->setDisabled(false);
            ui->DNS3LineEdit_2->setDisabled(false);
            ui->DNS3LineEdit_3->setDisabled(false);
            ui->DNS3LineEdit_4->setDisabled(false);

            ui->DNS3LineEdit->setEnabled(true);
            ui->DNS3LineEdit_2->setEnabled(true);
            ui->DNS3LineEdit_3->setEnabled(true);
            ui->DNS3LineEdit_4->setEnabled(true);

            autoDNSRadio_click();
        }

       // if(linkRateAutomaticWasAdded || chainFilenameWasAdded || linkRatewasAdded)
            //l,ook at cert was added
        if(serverCertFilenameWasAdded || chainFilenameWasAdded || clientCertFilenameWasAdded || csrFilenameWasAdded || csrKeyFilenameWasAdded || clientKeyFilenameWasAdded || clientCertUsersFilenameWasAdded || clientCertPasswordWasAdded)
        {
            if(dbsecurity.toInt() > 1 && dbsecurity.toInt() < 5)
            {
                ui->securityCombo->setCurrentIndex(dbsecurity.toInt() + 1);
                updateDB("security", QString::number(ui->securityCombo->currentIndex()));
            }
            else if(dbsecurity.toInt() >4 )
            {
                ui->securityCombo->setCurrentIndex(dbsecurity.toInt() + 2);
                updateDB("security", QString::number(ui->securityCombo->currentIndex()));
            }
        }
        else
        {
            ui->securityCombo->setCurrentIndex(dbsecurity.toInt());
            updateDB("security", QString::number(ui->securityCombo->currentIndex()));

        }

        ui->encryptionCombo->setCurrentIndex(dbencryption.toInt());
        if(ui->securityCombo->currentIndex() == 0 && ui->encryptionCombo->currentIndex() == 1)
        {
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->hide();
            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            //this->ui->wirelessAuthenticationDefaultKey1Radio->setTabOrder();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->show();
        }

        QStringList tempFilename = dbServerCertFilename.split("/", QString::KeepEmptyParts);
        //if(tempFilename.at(tempFilename.size() - 1) != "")
        //    ui->serverCertLabel->setStyleSheet("color: rgb(24, 122, 24);");
        //else
        //    ui->serverCertLabel->setStyleSheet("color: rgb(165, 25, 27);");
        ui->serverCertLabel->setText(tempFilename.at(tempFilename.size() - 1));

        tempFilename.clear();
        tempFilename = dbChainFilename.split("/", QString::KeepEmptyParts);
        //ui->chainFileLabel->setText(tempFilename.at(tempFilename.size() - 1));

        tempFilename.clear();
        tempFilename = dbClientCertFilename.split("/", QString::KeepEmptyParts);
        //if(tempFilename.at(tempFilename.size() - 1) != "")
        //    ui->clientCertLabel->setStyleSheet("color: rgb(24, 122, 24);");
        //else
        //    ui->clientCertLabel->setStyleSheet("color: rgb(165, 25, 27);");
        ui->clientCertLabel->setText(tempFilename.at(tempFilename.size() - 1));

        UsersClientCert = dbClientCertUsersFilename;
        UsersTrustedRoot = dbServerCertFilename;

        loadClientCertsCombo();
        loadTrustedRootCertsCombo();

        // Now select the correct certs from the two combo boxes.
        // trusted root cert
        if(UsersTrustedRoot == "")
        {
            ui->trustedRootStoreCombo->setCurrentIndex(0);
        }
        else
        {
            if(ui->trustedRootStoreCombo->findText(UsersTrustedRoot) > -1)
            {
                ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(UsersTrustedRoot));
            }
            else
            {
                ui->trustedRootStoreCombo->setCurrentIndex(0);
            }
        }
        // client cert
        if(UsersClientCert == "")
        {
            ui->clientCertStoreCombo->setCurrentIndex(0);
        }
        else
        {
            if(ui->clientCertStoreCombo->findText(UsersClientCert) > -1)
            {
                ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(UsersClientCert));
            }
            else
            {
                ui->clientCertStoreCombo->setCurrentIndex(0);
            }
        }
        //ui->alertsLineEdit->setText(dbClientCertUsersFilename);

      //  if(dbCSRFilename != "") ui->signingRequestLabel->setText(tr("Completed."));
     //   else ui->signingRequestLabel->setText("");

        // populate the wepkeys
        if(dbsecurity.toInt() == 0) // must be OPEN WEP
        {
            LOG_CLASS_INFO("SettingsWizard", "Loading OPEN WEP KEYS");

                //mainWindow_->close();

            QStringList WEPkeys = populateWEPKeys();

            if(WEPkeys.size() > 0)
            {
                //Set the default key
                //LOG_INFO( "default key '%s'...", qPrintable( WEPkeys.at(1).toInt() ) );
                LOG_INFO( "default key '%d'...", WEPkeys.at(1).toInt() );

                switch (WEPkeys.at(1).toInt())
                {
                    case 0:
                        ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
                        break;
                    case 1:
                        ui->wirelessAuthenticationDefaultKey2Radio->setChecked(true);
                        break;
                    case 2:
                        ui->wirelessAuthenticationDefaultKey3Radio->setChecked(true);
                        break;
                    case 3:
                        ui->wirelessAuthenticationdefaultKey4Radio->setChecked(true);
                        break;
                }

                // set the keys values
                if(WEPkeys.size() > 2)
                {
                    for(int i = 0; i < ((WEPkeys.size()-2) / 2) ; i++)
                    {
                        switch (WEPkeys.at((i*2) + 2).toInt())
                        {
                            case 1:
                                this->ui->wepKey1LineEdit->setText(WEPkeys.at((i*2) + 3));
                                break;
                            case 2:
                                this->ui->wepKey2LineEdit->setText(WEPkeys.at((i*2) + 3));
                                break;
                            case 3:
                                this->ui->wepKey3LineEdit->setText(WEPkeys.at((i*2) + 3));
                                break;
                            case 4:
                                this->ui->wepKey4LineEdit->setText(WEPkeys.at((i*2) + 3));
                                break;
                        }
                    }
                }
            }

        }

        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
        // for example, how can we enalbed EXPORT when we have never done it before.
        //if(dbfirstTimeExport != "1")
        //{
        //    //this->ui->exportBut->setEnabled(false);
        //    //this->ui->exportBut->setDisabled(true);
        //
        //}
        //else
        //{
        //    this->ui->exportBut->setEnabled(true);
        //    this->ui->exportBut->setDisabled(false);
        //}

        this->resizeControls();

        if(this->ui->wpaPassPhraseFrame->isVisible())
        {
            this->ui->wpaPassphraseLineEdit->setText(dbWpaPassphrase);
        }
        else if(this->ui->wpaEAPTLSPassPhraseFrame->isVisible())
        {
            this->ui->leClientKeyPassword->setText(dbClientCertPassword);
        }

        this->ui->eapPasswordLineEdit->setText(dbEapPassword);
        this->ui->eapUsernameLineEdit->setText(dbEapUsername);
        this->ui->networkLoadingStatusLabel->hide();
        this->ui->gbNetworkLoading->hide();
        this->update();
        this->ui->wepKey1LineEdit->installEventFilter(this);
        this->ui->wepKey2LineEdit->installEventFilter(this);
        this->ui->wepKey3LineEdit->installEventFilter(this);
        this->ui->wepKey4LineEdit->installEventFilter(this);
        howMany = 0;

        if(dbConnectionType == "1")
        {
           // this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
           // this->ui->alertsLineEdit->setText("LinkRate index:" + QString::number(ui->cbTransferRate->findData(dbLinkRate)) + " Channelindex:" + QString::number(ui->cbChannel->findData(dbChannel)));


            ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(dbChannel));
            //////////////
            channelIndex = ui->cbChannel->currentIndex();

            ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(dbLinkRate));
            linkRateIndex = ui->cbTransferRate->currentIndex();

                    if (dbAutomaticLinkRate == "0")
                    {
                        ui->chkboxTransferRateAuto->setChecked(false);
                        automaticLinkRate = false;
                    }
                    else
                    {
                        ui->chkboxTransferRateAuto->setChecked(true);
                        automaticLinkRate = true;
                    }

            //////////////////
            /*if(ui->cbChannel->currentIndex() < 0)
            {
                ui->cbChannel->setCurrentIndex(0);
            }
            if(dbChannel == "auto" || dbChannel == "")
            {
                channelIndex = 0;
            }
            else
            {
                channelIndex = ui->cbChannel->currentIndex();
            }

            ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(dbLinkRate));
            if(ui->cbTransferRate->currentIndex() < 0)
            {
                ui->cbTransferRate->setCurrentIndex(0);
            }

            if(dbLinkRate == "auto" || dbLinkRate == "")
            {
                linkRateIndex = 0;
            }
            else
            {
                linkRateIndex = ui->cbTransferRate->currentIndex();
            }

            if (dbAutomaticLinkRate == "0")
            {
                ui->chkboxTransferRateAuto->setChecked(false);
                automaticLinkRate = false;
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(true);
                automaticLinkRate = true;
            }
    */
            if(ui->cbTransferRate->currentIndex() == 0 && ui->cbChannel->currentIndex() == 0 && dbDNSSuffix1 == "" && dbDNSSuffix2 == "" && dbDNSSuffix3 == "" && dbDNSSuffix4 == "" && dbDNSSuffix5 == "" && dbDNSSuffix6 == "")
            {
                // default settings so no advanced config
                ui->chkboxTransferRateAuto->setChecked(true);
                ui->networkAdvancedBut->setStyleSheet("");
                ui->networkAdvancedBut->setIcon(QIcon());
            }
            else
            {
                //advanced config has been set
                ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
                howMany = 0;
                if(ui->cbTransferRate->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(!ui->chkboxTransferRateAuto->isChecked())
                {
                    howMany = howMany + 1;
                }
                if(ui->cbChannel->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(dbDNSSuffix1 != "" || dbDNSSuffix2 != "" || dbDNSSuffix3 != "" || dbDNSSuffix4 != "" || dbDNSSuffix5 != "" || dbDNSSuffix6 != "")
                {
                    howMany = howMany + 1;
                }

                ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));

            }

        }
        else
        {
            ui->cbChannel->setCurrentIndex(0);
            ui->cbTransferRate->setCurrentIndex(0);
            ui->chkboxTransferRateAuto->setChecked(true);
            if(ui->leDNSSuffix1->text().length() > 0 || ui->leDNSSuffix2->text().length() > 0 || ui->leDNSSuffix3->text().length() > 0)
            {
                ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
                howMany = howMany + 1;
                ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
            }
            else
            {
                ui->networkAdvancedBut->setStyleSheet("");
                ui->networkAdvancedBut->setIcon(QIcon());
            }
        }
        DNSSuffix1Memory = dbDNSSuffix1;
        DNSSuffix2Memory = dbDNSSuffix2;
        DNSSuffix3Memory = dbDNSSuffix3;
        DNSSuffix4Memory = dbDNSSuffix4;
        DNSSuffix5Memory = dbDNSSuffix5;
        DNSSuffix6Memory = dbDNSSuffix6;

        if(dbDNSSuffix1 != "" || dbDNSSuffix2 != "" || dbDNSSuffix3 != "" || dbDNSSuffix4 != "" || dbDNSSuffix5 != "" || dbDNSSuffix6 != "")
        {
            //show the badge and populate
            ui->leDNSSuffix1->setText(dbDNSSuffix1);
            ui->leDNSSuffix2->setText(dbDNSSuffix2);
            ui->leDNSSuffix3->setText(dbDNSSuffix3);
            ui->leDNSSuffix4->setText(dbDNSSuffix4);
            ui->leDNSSuffix5->setText(dbDNSSuffix5);
            ui->leDNSSuffix6->setText(dbDNSSuffix6);
        }
        if(ui->cbTransferRate->currentIndex() == 0 && ui->cbChannel->currentIndex() == 0 && dbDNSSuffix1 == "" && dbDNSSuffix2 == "" && dbDNSSuffix3 == "" && dbDNSSuffix4 == "" && dbDNSSuffix5 == "" && dbDNSSuffix6 == "")
        {
            // default settings so no advanced config
            ui->chkboxTransferRateAuto->setChecked(true);
            ui->networkAdvancedBut->setStyleSheet("");
            ui->networkAdvancedBut->setIcon(QIcon());
        }
        else
        {
            //advanced config has been set
            ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            howMany = 0;

            if(dbConnectionType == "1")
            {
                if(ui->cbTransferRate->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(!ui->chkboxTransferRateAuto->isChecked())
                {
                    howMany = howMany + 1;
                }
                if(ui->cbChannel->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
            }
            if(dbDNSSuffix1 != "" || dbDNSSuffix2 != "" || dbDNSSuffix3 != "" || dbDNSSuffix4 != "" || dbDNSSuffix5 != "" || dbDNSSuffix6 != "")
            {
                howMany = howMany + 1;
            }

            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));

        }

    }
    networkTabAlreadyLoaded = true;
    processingAButton = false;

    LOG_DEBUG( "loadNetworkTab() END", NULL );
}

void setupUtility::closeCertInfoButClicked()
{
    ui->teCertInfo->setText("");
    ui->lblCertInfoSourceFilename->setText("");
    ui->teCertInfo->setText("");
    ui->gbCertInfo->hide();
}

void setupUtility::snmpManagerBut_clicked()
{
    //save to DB
    updateDB("snmpTrapsIP", ui->snmpManagerTrapsIP->text());
    updateDB("snmpTrapsPort", ui->snmpManagerTrapsPort->text());
    updateDB("snmpVariablesPort", ui->snmpManagerVariablesPort->text());
    // restart the snmp agent
    python pythonProcess;
    QStringList options;
    options.append("");
    if (pythonProcess.execCommand("snmpReload.py", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            ui->alertsLineEdit->setText(tr("SNMP has been started with new settings."));
        }
        else
        {
            // TODO:
            ui->alertsLineEdit->setText(tr("SNMP set."));
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }


}

void setupUtility::advancedNetworkingBut_clicked()
{
    if (ui->availableConnectionsTableView->currentRow() == 0)
    {
        ui->gbWirelessChannel->setEnabled(false);
        ui->gbTransferRate->setEnabled(false);

    }
    else
    {
        ui->gbWirelessChannel->setEnabled(true);
        ui->gbTransferRate->setEnabled(true);
    }
    ui->gbAdvancedNetworking->show();
}

void setupUtility::advancedNetworkingCancelBut_clicked()
{
    ui->cbTransferRate->setCurrentIndex(linkRateIndex);
    ui->chkboxTransferRateAuto->setChecked(automaticLinkRate);
    ui->cbChannel->setCurrentIndex(channelIndex);
    ui->leDNSSuffix1->setText(DNSSuffix1Memory);
    ui->leDNSSuffix2->setText(DNSSuffix2Memory);
    ui->leDNSSuffix3->setText(DNSSuffix3Memory);
    ui->leDNSSuffix4->setText(DNSSuffix4Memory);
    ui->leDNSSuffix5->setText(DNSSuffix5Memory);
    ui->leDNSSuffix6->setText(DNSSuffix6Memory);

    if(ui->cbTransferRate->currentIndex() == 0 && ui->cbChannel->currentIndex() == 0 && ui->leDNSSuffix1->text() == "" && ui->leDNSSuffix2->text() == "" && ui->leDNSSuffix3->text() == "" && ui->leDNSSuffix4->text() == "" && ui->leDNSSuffix5->text() == "" && ui->leDNSSuffix6->text() == "")
    {
        // default settings so no advanced config
        ui->chkboxTransferRateAuto->setChecked(true);
        ui->networkAdvancedBut->setStyleSheet("");
        ui->networkAdvancedBut->setIcon(QIcon());
    }
    else
    {
        //advanced config has been set
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        howMany = 0;
        if(ui->cbTransferRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxTransferRateAuto->isChecked())
        {
            howMany = howMany + 1;
        }
        if(ui->cbChannel->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(ui->leDNSSuffix1->text() != "" || ui->leDNSSuffix2->text() != "" || ui->leDNSSuffix3->text() != "" || ui->leDNSSuffix4->text() != "" || ui->leDNSSuffix5->text() != "" || ui->leDNSSuffix6->text() != "")
        {
            howMany = howMany + 1;
        }
        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
    }

    ui->gbAdvancedNetworking->hide();
}

void setupUtility::advancedNetworkingLinkRateCombo_changed(int index)
{
    if(index == 0)
    {
        ui->chkboxTransferRateAuto->setChecked(true);
        ui->chkboxTransferRateAuto->setEnabled(false);
    }
    else
    {
        ui->chkboxTransferRateAuto->setEnabled(true);
    }
}

void setupUtility::advancedNetworkingApplyBut_clicked()
{
    linkRateIndex = ui->cbTransferRate->currentIndex();
    automaticLinkRate = ui->chkboxTransferRateAuto->isChecked();
    channelIndex = ui->cbChannel->currentIndex();
    DNSSuffix1Memory = ui->leDNSSuffix1->text();
    DNSSuffix2Memory = ui->leDNSSuffix2->text();
    DNSSuffix3Memory = ui->leDNSSuffix3->text();
    DNSSuffix4Memory = ui->leDNSSuffix4->text();
    DNSSuffix5Memory = ui->leDNSSuffix5->text();
    DNSSuffix6Memory = ui->leDNSSuffix6->text();

    if(ui->cbTransferRate->currentIndex() == 0 && ui->cbChannel->currentIndex() == 0 && ui->leDNSSuffix1->text() == "" && ui->leDNSSuffix2->text() == "" && ui->leDNSSuffix3->text() == "" && ui->leDNSSuffix4->text() == "" && ui->leDNSSuffix5->text() == "" && ui->leDNSSuffix6->text() == "")
    {
        // default settings so no advanced config
        ui->chkboxTransferRateAuto->setChecked(true);
        ui->networkAdvancedBut->setStyleSheet("");
        ui->networkAdvancedBut->setIcon(QIcon());
    }
    else
    {
        //advanced config has been set
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        howMany = 0;
        if(ui->cbTransferRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxTransferRateAuto->isChecked())
        {
            howMany = howMany + 1;
        }
        if(ui->cbChannel->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(ui->leDNSSuffix1->text() != "" || ui->leDNSSuffix2->text() != "" || ui->leDNSSuffix3->text() != "" || ui->leDNSSuffix4->text() != "" || ui->leDNSSuffix5->text() != "" || ui->leDNSSuffix6->text() != "")
        {
            howMany = howMany + 1;
        }

        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
    }
    ui->gbAdvancedNetworking->hide();
}

void setupUtility::advancedNetworkingDefaultsBut_clicked()
{
    ui->chkboxTransferRateAuto->setChecked(true);
    ui->cbTransferRate->setCurrentIndex(0);
    ui->cbChannel->setCurrentIndex(0);
    ui->leDNSSuffix1->setText("");
    ui->leDNSSuffix2->setText("");
    ui->leDNSSuffix3->setText("");
    ui->leDNSSuffix4->setText("");
    ui->leDNSSuffix5->setText("");
    ui->leDNSSuffix6->setText("");
}

void setupUtility::searchTrustedRootCertButClicked()
{

    ///////xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    QString result;
    QString source = ui->trustedRootStoreCombo->itemData(ui->trustedRootStoreCombo->currentIndex()).toString();
    python pythonProcess;
    QStringList args;
    QString certFile = "settingswizard/TRUSTEDROOTSTORE/" + source;

    if( TESTING )
        certFile = "C:/hobart/settingswizard/TRUSTEDROOTSTORE/" + source;

    source = filesystem::absoluteFilePath(certFile, folderType::DBs);
    args.clear();
    args.append("-S");
    args.append(source);
    args.append("-P");
    //args.append(ui->leClientKeyPassword->text());
    args.append("");
    args.append("-T");
    args.append("ca");

    if (pythonProcess.execCommand("outputCertInfo", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
        }
        else
        {
            result = tr("\n\n\n\n       No certificate information can be shown.");
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        // TODO:
        // What do we do here?
    }
        ////qDebug() << "result APs is : " << result;
        ///
    ui->teCertInfo->setText(result);
    ui->lblCertInfoSourceFilename->setText(ui->trustedRootStoreCombo->itemData(ui->trustedRootStoreCombo->currentIndex()).toString());
    ui->gbCertInfo->show();
}

void setupUtility::clientCertDeleteButClicked()
{
    //this->ui->gbDeleteClientCertPrompt->show();
    // Check if cert is currently being used and active wireless connection type.
    if(ui->clientCertStoreCombo->currentIndex() > 0)
    {
        bool showSpecialPrompt = false;
        showSpecialPrompt = checkIfActiveCert(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()), "client");

        if(showSpecialPrompt)
        {
            ui->graphicsViewClientCertWarning->show();
            ui->deleteClientCertLabel->setGeometry( scaleRectForScreen( QRect( 74, 18, 451, 121 ) ) );
            ui->deleteClientCertLabel->setText(tr("The current network configuration requires this certificate. If you delete this certificate you will be disconnected from the network."));
        }
        else
        {
             ui->graphicsViewClientCertWarning->hide();
             ui->deleteClientCertLabel->setGeometry( scaleRectForScreen( QRect( 48, 18, 451, 121 ) ) );
             ui->deleteClientCertLabel->setText(tr("This cert will be deleted and removed from the list of choices for Client Certificates if you click the Delete button."));
        }
        ui->deleteClientCertNameLabel->setText(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
        ui->frameDeleteClientCertPrompt->raise();
        ui->frameDeleteClientCertPrompt->show();
    }
}

void setupUtility::clearWindowsWlanConfandDisconnect()
{
    python pythonProcess;
    if (pythonProcess.execCommand("clearWlanConfAndDisconnect"))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            ;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }
}

void setupUtility::clearWpaSupplicantConfAndDisconnect()
{
    python pythonProcess;
    if (pythonProcess.execCommand("clearWPASupplicantConfAndDisconnect"))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            ;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }
}

void setupUtility::deleteClientCertYesBut_clicked()
{
    if(ui->clientCertStoreCombo->currentIndex() > 0)
    {
        if(checkIfActiveCert(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()), "client"))
        {
            // Have to delete the active cert from /settingswizard dir for both trustedroot and client certs also
            QString serialNumber = getSerialNumber();
            deleteActiveCert(serialNumber + ".key");
            deleteActiveCert(serialNumber + ".pem");
            deleteActiveCert(getDBValue("serverCertFilename"));

            updateDB("serverCertFilename", "");
            updateDB("clientCertFilename", "");
            updateDB("clientCertUsersFilename", "");

            //take system offline
            if (isLinux()) clearWpaSupplicantConfAndDisconnect();
            else clearWindowsWlanConfandDisconnect();
        }

        deleteCert(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()), "client");

        loadClientCertsCombo();
        ui->clientCertStoreCombo->setCurrentIndex(0);
    }
    //ui->gbDeleteClientCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
}

void setupUtility::deleteClientCertCancelBut_clicked()
{
    //ui->gbDeleteClientCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
}

void setupUtility::serverCertDeleteButClicked()
{
    if(ui->trustedRootStoreCombo->currentIndex() > 0)
    {
        // Check if cert is currently being used and active wireless connection type.
        bool showSpecialPrompt = false;
        showSpecialPrompt = checkIfActiveCert(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()), "ca");

        if(showSpecialPrompt)
        {
             ui->graphicsViewServerCertWarning->show();
             ui->deleteServerCertLabel->setGeometry( scaleRectForScreen( QRect( 74, 18, 451, 121 ) ) );
             ui->deleteServerCertLabel->setText(tr("The current network configuration requires this certificate. If you delete this certificate you will be disconnected from the network."));
        }
        else
        {
            ui->graphicsViewServerCertWarning->hide();
            ui->deleteServerCertLabel->setGeometry( scaleRectForScreen( QRect( 48, 18, 451, 121 ) ) );
            ui->deleteServerCertLabel->setText(tr("This cert will be deleted and removed from the list of choices for Trusted Root Certificates if you click the Delete button."));
        }
        ui->deleteServerCertNameLabel->setText(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
        ui->frameDeleteServerCertPrompt->raise();
        ui->frameDeleteServerCertPrompt->show();
    }
}

void setupUtility::deleteServerCertYesBut_clicked()
{
    // Check if cert is currently being used and active wireless connection type.
    if(ui->trustedRootStoreCombo->currentIndex() > 0)
    {
        if(checkIfActiveCert(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()), "ca"))
        {
            QString serialNumber = getSerialNumber();
            deleteActiveCert(serialNumber + ".key");
            deleteActiveCert(serialNumber + ".pem");
            deleteActiveCert(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));

            updateDB("serverCertFilename", "");
            updateDB("clientCertFilename", "");
            updateDB("clientCertUsersFilename", "");

            //take system offline
            if(isLinux()) clearWpaSupplicantConfAndDisconnect();
            else clearWindowsWlanConfandDisconnect();
        }
        deleteCert(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()), "ca");
        loadTrustedRootCertsCombo();
        ui->trustedRootStoreCombo->setCurrentIndex(0);
    }
    //ui->gbDeleteServerCertPrompt->hide();
    ui->frameDeleteServerCertPrompt->hide();
}

void setupUtility::deleteServerCertCancelBut_clicked()
{
    //ui->gbDeleteServerCertPrompt->hide();
    ui->frameDeleteServerCertPrompt->hide();
}


void setupUtility::deleteCert(QString certName, QString certType)
{
    python pythonProcess;
    QStringList args;
    //bool worked = true;
    // CA Certificate Authority
    args.append("-S");
    args.append(certName);
    args.append("-T");
    args.append(certType);
    if (pythonProcess.execCommand("deleteCert", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            this->ui->alertsLineEdit->setText(tr("Certificate removed successfully."));
        }
        else
        {
            this->ui->alertsLineEdit->setText(tr("Certificate not removed successfully."));
        }
    }
    else
    {
        ;
        // TODO:
        // What do we do here?
    }
}

void setupUtility::deleteActiveCert(QString certName)
{
    python pythonProcess;
    QStringList args;
    //bool worked = true;
    // CA Certificate Authority
    args.append("-C");
    args.append(certName);
    if (pythonProcess.execCommand("deleteActiveCert", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            this->ui->alertsLineEdit->setText(tr("Certificate removed successfully."));
        }
        else
        {
            this->ui->alertsLineEdit->setText(tr("Certificate not removed successfully."));
        }
    }
    else
    {
        ;
        // TODO:
        // What do we do here?
    }
}


bool setupUtility::checkIfActiveCert(QString certName, QString certType)
{
    bool answer = false;

    if (certType == "ca")
    {
        python pythonProcess;
        QStringList args;
        //bool worked = true;
        // CA Certificate Authority
        args.append("-C");
        args.append(certName);
        if (pythonProcess.execCommand("activeCertCheck", args))
        {
            if (pythonProcess.getExitCode() == 50) // 50 means the cert is actively being used
            {
                answer = true;
                //this->ui->alertsLineEdit->setText(tr("Certificate removed successfully."));
            }
            else if( pythonProcess.getExitCode() == 0) // 0 means the cert is not actively being used
            {
                answer = false;
                //this->ui->alertsLineEdit->setText(tr("Certificate not removed successfully."));
            }
        }
        else
        {
            ;
            // TODO:
            // What do we do here?
        }
    }
    else if (certType == "client")
    {
        if(certName == getDBValue("clientCertUsersFilename"))
        {
            answer = true;
        }
        else
        {
            answer = false;
        }
    }

    return answer;
}

void setupUtility::searchClientCertButClicked()
{

    ///////xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    QString result;
    QString source = ui->clientCertStoreCombo->itemData(ui->clientCertStoreCombo->currentIndex()).toString();
    python pythonProcess;
    QStringList args;
    QString certFile = "settingswizard/CLIENTCERTSTORE/" + source;

    if( TESTING )
        certFile = "C:/hobart/settingswizard/CLIENTCERTSTORE/" + source;

    source = filesystem::absoluteFilePath(certFile, folderType::DBs);
    args.clear();
    args.append("-S");
    args.append(source);
    args.append("-P");
    args.append(ui->leClientKeyPassword->text());
    args.append("-T");
    args.append("client");

    if (pythonProcess.execCommand("outputCertInfo", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
        }
        else
        {
            result = tr("\n\n\n\n       No certificate information can be shown.");
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        // TODO:
        // What do we do here?
    }
        ////qDebug() << "result APs is : " << result;
        ///
    ui->teCertInfo->setText(result);
    ui->lblCertInfoSourceFilename->setText(ui->clientCertStoreCombo->itemData(ui->clientCertStoreCombo->currentIndex()).toString());
    ui->gbCertInfo->show();
}

void setupUtility::loadTrustedRootCertsCombo()
{
    QStringList rootCertsList = getStoredCerts("TRUSTEDROOTSTORE");
    ui->trustedRootStoreCombo->clear();
    ui->trustedRootStoreCombo->addItem(tr("Choose a certificate"), "");
    for (int i = 0; i < rootCertsList.size(); i++)
    {
        ui->trustedRootStoreCombo->addItem(rootCertsList.at(i),rootCertsList.at(i));
    }

}

void setupUtility::updateServerCertFilename(int userSelection)
{
    dbServerCertFilename = ui->trustedRootStoreCombo->itemData(userSelection).toString();
}

void setupUtility::updateClientCertFilename(int userSelection)
{
    dbClientCertUsersFilename = ui->clientCertStoreCombo->itemData(userSelection).toString();
}

void setupUtility::loadClientCertsCombo()
{
    QStringList clientCertsList = getStoredCerts("CLIENTCERTSTORE");
    ui->clientCertStoreCombo->clear();
    ui->clientCertStoreCombo->addItem(tr("Choose a certificate"), "");
    for (int i = 0; i < clientCertsList.size(); i++)
    {
        ui->clientCertStoreCombo->addItem(clientCertsList.at(i),clientCertsList.at(i));
    }
}

void setupUtility::setWirelessFrames(int)
{
    if(ui->securityCombo->currentIndex() == 0 && ui->encryptionCombo->currentIndex() == 1)
    {
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        //this->ui->wirelessAuthenticationDefaultKey1Radio->setTabOrder();
        this->ui->wepKeyFrame->hide();
        this->ui->openNoneFrame->show();
    }
    else if(ui->securityCombo->currentIndex() == 0 && ui->encryptionCombo->currentIndex() == 0)
    {
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        //this->ui->wirelessAuthenticationDefaultKey1Radio->setTabOrder();
        this->ui->openNoneFrame->hide();
        this->ui->wepKeyFrame->show();
    }
}

void setupUtility::signingRequestButClicked()
{
    QString csrFilename = getSerialNumber();
    //ui->alertsLineEdit->setText(csrFilename);
    CSRFilename = csrFilename;
    dbCSRFilename = CSRFilename + "_req.pem";
    dbCSRKeyFilename = CSRFilename + "_key.pem";

    QString csrDestination = "";
    csrDestination = getDirSelected(tr("Directory for CSR"));

    if(csrDestination != "")
    {
        python pythonProcess;
        QStringList args;
        args.append("-D");
        args.append(csrDestination + "/");
        args.append("-F");
        args.append(csrFilename);
        args.append("-E");
        args.append("1096");
        args.append("-P");
        args.append("woozal");

        if (pythonProcess.execCommand("createCertificateSigningRequest", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ui->alertsLineEdit->setText(tr("CSR successfully saved to removable drive"));
                //ui->signingRequestLabel->setText(tr("Completed."));//
                updateDB("csrFilename", dbCSRFilename);
                updateDB("csrKeyFilename", dbCSRKeyFilename);
            }
            else
            {
                ;//worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            ;//worked = worked + 1;
            // TODO:
            // What do we do here?
        }
    }
}

void setupUtility::importCertFiles(QString certToCopy, QString certType)
{
    python pythonProcess;
    QStringList args;
    QString certDir = "";

    QString databaseFileName = "settingswizard/";

    if( TESTING )
        databaseFileName = "C:/hobart/settingswizard/";

    int worked = 0;

    if(certType == "Trusted Root Cert")
    {
        databaseFileName = databaseFileName + "TRUSTEDROOTSTORE/";
        // CA Certificate Authority
        args.append("-S");
        args.append(certToCopy);
        args.append("-D");
        databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
        args.append(databaseFileLocation);
        args.append("-T");
        args.append("ca");
        args.append("-P");
        args.append(ui->leClientKeyPassword->text());
        args.append("-U");
        args.append("0");
        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
               // updateDB("serverCertFilename", dbServerCertFilename);//
            }
            else
            {
                worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            worked = worked + 1;
            // TODO:
            // What do we do here?
        }
    }
    else if (certType == "Client Cert")
    {
        // Client cert and key in a p12 format only for Delhaize

        args.clear();
        args.append("-S");
        args.append(certToCopy);
        args.append("-D");
        databaseFileName = databaseFileName + "CLIENTCERTSTORE/";
        databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
        //args.append(databaseFileLocation);
        args.append(getSerialNumber());
        args.append("-T");
        args.append("client");
        args.append("-P");
        args.append(ui->leClientKeyPassword->text());
        args.append("-U");
        args.append("0");

        QStringList certName = clientCertFilename.split("/", QString::KeepEmptyParts);

        QString justCertName = certName.at(certName.size() -1);

        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ;//updateDB("clientCertFilename", dbClientCertFilename);//
                //updateDB("clientCertUsersFilename", dbClientCertUsersFilename);
            }
            else
            {
               worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            worked = worked + 1;
            // TODO:
            // What do we do here?
        }
    }

    if(worked > 0)
    {
        ;//this->ui->alertsLineEdit->setText(tr("Problem installing certificates."));
       // this->ui->signingRequestLabel->setText(tr("Error."));
    }
    else
    {
       ; //this->ui->alertsLineEdit->setText(tr("Certificates installed successfully."));
       // this->ui->signingRequestLabel->setText(tr("Completed."));
    }
}

void setupUtility::copyCertsFiles(QString certToCopy)
{
    python pythonProcess;
    QStringList args;
    QString databaseFileName = "settingswizard/" + dbServerCertFilename;

    if( TESTING )
        databaseFileName = "C:/hobart/settingswizard/" + dbServerCertFilename;

    int worked = 0;

    if(certToCopy == "Trusted Root Cert")
    {
        // CA Certificate Authority
        args.append("-S");
        args.append(serverCertFilename);
        args.append("-D");
        databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
        args.append(databaseFileLocation);
        args.append("-T");
        args.append("ca");
        args.append("-P");
        args.append(ui->leClientKeyPassword->text());
        args.append("-U");
        args.append("0");
        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
               // updateDB("serverCertFilename", dbServerCertFilename);//
                QString response = pythonProcess.readStdOut();
                if(response.contains("no file"))
                {
                    //file not existant
                    ui->alertsLineEdit->setText(tr("File not found. No certificate saved."));
                }
                else if (response.contains("invalid password"))
                {
                    ui->alertsLineEdit->setText("Invalid password. Certificate was saved.");
                }
            }
            else
            {
                worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            worked = worked + 1;
            // TODO:
            // What do we do here?
        }
    }
    else if (certToCopy == "Client Cert")
    {
        // Client cert and key in a p12 format only for Delhaize

        args.clear();
        args.append("-S");
        args.append(clientCertFilename);
        args.append("-D");
        databaseFileName = "settingswizard/" + dbClientCertFilename;

        if( TESTING )
            databaseFileName = "C:/hobart/settingswizard/" + dbClientCertFilename;

        databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
        //args.append(databaseFileLocation);
        args.append(getSerialNumber());
        args.append("-T");
        args.append("client");
        args.append("-P");
        args.append(ui->leClientKeyPassword->text());
        args.append("-U");
        args.append("0");

        QStringList certName = clientCertFilename.split("/", QString::KeepEmptyParts);

        QString justCertName = certName.at(certName.size() -1);

        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                //updateDB("clientCertFilename", dbClientCertFilename);//
                //updateDB("clientCertUsersFilename", dbClientCertUsersFilename);
                QString response = pythonProcess.readStdOut();
                if(response.contains("no file"))
                {
                    //file not existant
                    ui->alertsLineEdit->setText(tr("File not found. No certificate saved."));
                }
                else if (response.contains("invalid password") || "Incorrect Password")
                {
                    ui->alertsLineEdit->setText("Invalid password. Certificate was saved.");
                }
            }
            else
            {
               worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            worked = worked + 1;
            // TODO:
            // What do we do here?
        }

        // FUTURE chain File
        /*args.clear();
        args.append("-S");
        args.append(chainFilename);
        args.append("-D");
        databaseFileName = "settingswizard/" + dbChainFilename;
        databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
        args.append(databaseFileLocation);
        args.append("-T");
        args.append("client");
        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                updateDB("chainFilename", dbChainFilename);//
            }
            else
            {
                worked = worked + 1;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            worked = worked + 1;
            // TODO:
            // What do we do here?
        }
        */
    }

    if(worked > 0)
    {
        //updateDB("clientKeyFilename",dbClientKeyFilename);
        this->ui->alertsLineEdit->setText(tr("Problem installing certificates."));
       // this->ui->signingRequestLabel->setText(tr("Error."));
    }
    else
    {
        this->ui->alertsLineEdit->setText(tr("Certificates installed successfully."));
       // this->ui->signingRequestLabel->setText(tr("Completed."));
    }
}

QString setupUtility::getFileSelected(QString dialogBanner)
{
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName;
    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    QStringList folders;

#if defined( Q_OS_LINUX )
    folders.push_back( "/media/thumb" );
    folders.push_back( "/media" );
#elif defined( Q_OS_WIN )
    folders.push_back( "D:\\" );
    folders.push_back( "E:\\" );
    folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( startDirStr );

    dialog.setWindowTitle(dialogBanner);
    dialog.setDirectory( startDirStr );
    dialog.setFilter( formatStr );
    dialog.setOptions( QFileDialog::DontConfirmOverwrite );
    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            fileName = selected[0] ;
    }

    if(fileName.isNull())
    {
        //user clicked cancel
        fileName = tr("None");
    }

    return fileName;
}

QString setupUtility::getDirSelected(QString dialogBanner)
{
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName;
    FileDialog dialog( mainWindow_ );

    QStringList folders;

#if defined( Q_OS_LINUX )
    folders.push_back( "/media/thumb" );
    folders.push_back( "/media" );
#elif defined( Q_OS_WIN )
    folders.push_back( "D:\\" );
    folders.push_back( "E:\\" );
    folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( startDirStr );

    dialog.setWindowTitle(dialogBanner);
    dialog.setDirectory( startDirStr );
    dialog.setFilter( formatStr );
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOptions( QFileDialog::DontConfirmOverwrite);
    if ( QDialog::Accepted == dialog.exec() )
    {
        qApp->processEvents();
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            fileName = selected[0] ;
    }

    if(fileName.isNull())
    {
        //user clicked cancel
        fileName = tr("None");
    }

    return fileName;
}

void setupUtility::serverCertButClicked()
{
    // copy the serverCert to storage
    serverCertFilename = getFileSelected(tr("Install Trusted Root Certificate"));
    if(serverCertFilename != tr("None") && serverCertFilename != "")
    {
        QStringList justFilename = serverCertFilename.split("/", QString::KeepEmptyParts);
        ui->serverCertLabel->setStyleSheet("color: rgb(24,122,24);");
        ui->serverCertLabel->setText(justFilename.at(justFilename.size()-1));
        dbServerCertFilename = justFilename.at(justFilename.size()-1);
        //ui->alertsLineEdit->setText(dbServerCertFilename);
        copyCertsFiles("Trusted Root Cert");
        loadTrustedRootCertsCombo();
        //ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findData(UsersTrustedRoot));
        ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(justFilename.at(justFilename.size()-1)));
        //Comment out alert below
        //ui->alertsLineEdit->setText(dbServerCertFilename);
    }
  /*  else
    {
        ui->serverCertLabel->setStyleSheet("color: rgb(165,25,27);");
        ui->serverCertLabel->setText("No Trusted Root Certificate");
        dbServerCertFilename = "";
    }
   */ //ui->alertsLineEdit->setText(serverCertFilename);
}

void setupUtility::chainFileButClicked()
{
    // copy the chainFile to storage
    chainFilename = getFileSelected(tr("Install Chain File Certificate"));
    if(chainFilename != tr("None") && chainFilename != "")
    {
        QStringList justFilename = chainFilename.split("/", QString::KeepEmptyParts);
        //ui->chainFileLabel->setText(justFilename.at(justFilename.size()-1));
        dbChainFilename = justFilename.at(justFilename.size()-1);
    }

    //ui->alertsLineEdit->setText(chainFilename);
}

void setupUtility::clientCertButClicked()
{
    // copy the clientCert to storage
    clientCertFilename = getFileSelected(tr("Install Client Certificate"));
    if(clientCertFilename != tr("None") && clientCertFilename != "")
    {
        QStringList justFilename = clientCertFilename.split("/", QString::KeepEmptyParts);
        ui->clientCertLabel->setStyleSheet("color: rgb(22,124,22);");
        ui->clientCertLabel->setText(justFilename.at(justFilename.size()-1));
        dbClientCertFilename =  justFilename.at(justFilename.size()-1);
        dbClientCertUsersFilename = dbClientCertFilename;
        copyCertsFiles("Client Cert");
        loadClientCertsCombo();
        ui->leClientKeyPassword->setText("");
        //Comment alert below
        //ui->alertsLineEdit->setText(QString::number(justFilename.size()) + ":" + justFilename.at(justFilename.size() -1));
       // ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findData(UsersClientCert));
        ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(justFilename.at(justFilename.size() - 1)));
    }
/*    else
    {
        ui->clientCertLabel->setStyleSheet("color: rgb(165,25,27);");
        ui->clientCertLabel->setText("No Client p12 Certificate");
        dbServerCertFilename = "";
        dbClientCertFilename = "";
        dbClientCertUsersFilename = "";
    }
*/
    //ui->alertsLineEdit->setText(clientCertFilename);
}

QStringList setupUtility::populateWEPKeys()
{
    LOG_CLASS_INFO( "setupUtility", "populateWEPKeys" );

    processingAButton = true;
    //open etcwpa_supplicant.conf file and parse wepkeys
    QString wpaSupplicantFilename = "";
    if( isLinux() )
        wpaSupplicantFilename = "/etc/wpa_supplicant.conf";
    else
        wpaSupplicantFilename = "C:\\Program Files\\Hobart\\Db\\settingswizard\\NetKeys.txt";

    if( TESTING )
        wpaSupplicantFilename = "C:/hobart/settingswizard/NetKeys.txt";

    QFile file(wpaSupplicantFilename);
    QStringList wepKeys;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        processingAButton = false;

        return wepKeys;
    }
    QStringList theLine;
    QTextStream in(&file);

    while (!in.atEnd())
    {

        QString line = in.readLine();

       // LOG_INFO( "setting language to '%s'", value.toLatin1().constData() );
        // records the full line form the WEPKeys QStringList

        LOG_INFO( "NetKeys: '%s'...", line.toLatin1().constData() );

        theLine = line.split("=", QString::KeepEmptyParts);
        QString keyValue;
        if(theLine.size() > 1)
        {

            keyValue = theLine.at(0);
            keyValue = keyValue.replace(" ","");

                if(keyValue == "wep_tx_keyidx")
                {
                    wepKeys.append("ID");
                    wepKeys.append(theLine.at(1));
                }
                else if(keyValue == "wep_key0")
                {
                    wepKeys.append("1");
                    wepKeys.append(theLine.at(1));
                }
                else if(keyValue == "wep_key1")
                {
                    wepKeys.append("2");
                    wepKeys.append(theLine.at(1));
                }
                else if(keyValue == "wep_key2")
                {
                    wepKeys.append("3");
                    wepKeys.append(theLine.at(1));
                }
                else if(keyValue == "wep_key3")
                {
                    wepKeys.append("4");
                    wepKeys.append(theLine.at(1));
                }

        }
    }
    file.close();
    processingAButton = false;
    return wepKeys;
}

void setupUtility::checkAgainstPoorServiceTraining(QString text)
{
    //^([a-zA-Z]+[a-zA-Z0-9\-]*[a-zA-Z0-9]+)$
    //ui->hostnameLabel->setAlignment(Qt::AlignHCenter);
    //ui->hostnameLabel->setText(tr("Hostname"));

    ui->hostnameLabel->setText("");
    if(ui->hostnameLineEdit->text() == "" || text == "")
    {
        ui->setHostnameBut->setEnabled(false);
    }
    else
    {
        ui->setHostnameBut->setEnabled(true);
    }
}

void setupUtility::checkAgainstEmptyPassword(QString text)
{
    ui->setScalePasswordBut->setEnabled( text.isEmpty() ? false : true );
}

void setupUtility::shutdown() // antiquated
{
    LOG_CLASS_INFO("SettingsWizard", "ChangedTimezone-Hostname-AstronomicalClock");
    LOG_WARN("shutdown button has been pressed", NULL);
    processingAButton = true;
    if (isDemoMode)
    {
        LOG_INFO("closing main window...", NULL);
        //mainWindow_->close();
        delete ui;
    }
    else
    {
        // Tell backend we are rebooting so database can be gracefully prepared
        // Backend needs to report that the DB has had time to gracefully shutdown.
        //100,97,5 reboot
        //100,97,6 shutdown
        //
        //session_->send(100,97,"5"); // reboot
        session_->send(SessionDev, SnShutdownRequest, "5");
    }
    //mediaObject->deleteLater();
    //audioOutput->deleteLater();
    // wait for return message

}

void setupUtility::setDateTimeText()
{
    QString text( session_->uiUtilities->updateDateTime() );

    // convert case if required
    if ( convertUppercase_ )
        text = text.toUpper();

    ui->CurrentDateTimeTimeLabel->setText(text);
    //ui->ntpCurrentTimeLabel->setText( text );
    //ui->timeTabWidget->setTabText(1, text);
}

void setupUtility::onSessionMessage( const SessionMessage& msg )
{
   // qDebug() << "shouldChangeTime = " << shouldChangeTime;
   // qDebug() << "shouldChangeNTP = " << shouldChangeNTP;
   // qDebug() << "msg1= " << msg[0];
   // qDebug() << "msg2= " << msg[1];
   // if(msg.count() > 2) qDebug() << "msg3= " << msg[2];
   // if(msg.count() > 3) qDebug() << "msg4= " << msg[3];
   // if(msg.count() > 4) qDebug() << "msg5= " << msg[4];

    if (Error == msg[0].toInt() &&  ErrNetworkConnectivityDevice == msg[1].toInt() && 145001 == msg[2].toInt())
    {
        ;//do nothing suppress popup
    }
    else if (Error == msg[0].toInt() && ErrVersionManagerDevice == msg[1].toInt() && 120080 == msg[2].toInt())
    {
        //103,120,120080,3
       // qDebug() << "USB Key present" << msg[0] + "," + msg[1] + "," + msg[2];
        //session_->send(Error,ErrRemoveAnyModalDialogDevice,"120080,5");   // remove message saying updating
       // USBKeyDialogIsVisible = true;

        // Provide dialog now
        //dialog = new SessionErrorDialog( msg, this);
        ui->USBGroupbox->show();
    }
    else if (Error == msg[0].toInt() && 255 == msg[1].toInt() && 120080 == msg[2].toInt() && 3 == msg[3].toInt())
    {
        //103,255,120080,3
        //qDebug() << "No USB Key " << msg[0] + "," + msg[1] + "," + msg[2];
        //session_->send(Error,ErrRemoveAnyModalDialogDevice,"120080,5");   // remove message saying updating
        //session_->send(Error,ErrRemoveAnyModalDialogDevice,"120080,5");

//if mainWindow_->sessionError_->
        ui->USBGroupbox->hide();
        if(shouldChangeTime || shouldChangeNTP)
        {
            changeTimeAfterServiceKeyRemoved();
        }
        if(changedHostname || changedSpeakers)
        {
            this->shutdown();
        }
    }
    // date and time
    else if (SessionDev == msg[0].toInt()  && SnWriteDateTime == msg[1].toInt(0))
    {
        this->setDateTimeText();
    }
    else if(Printer == msg[0].toInt() && PrWriteAllUsbDriverNames == msg[1].toInt())
    {
        updateUsbComboNames(msg);       // 7,112
    }
    else if(Printer == msg[0].toInt() && PrWriteAllNetworkPrinterDriverNames == msg[1].toInt())
    {
        updateNetworkComboNames(msg);   // 7,114
    }
    else if(Printer == msg[0].toInt() && PrWriteAllExternalPrinter == msg[1].toInt())
    {
        updatePrinterTab(msg);          // 7,123
    }
    else if(Printer == msg[0].toInt() && PrWriteAllProductTextPrintParams == msg[1].toInt())
    {
        updatePrinterTabData(msg);      // 7,133
    }
    else if( ( ConfigurationDev == msg[0].toInt() ) && ( CfgWriteSingleField == msg[1].toInt() ) )
    {
        updatePrinterCheckboxes(msg);
    }
}

bool setupUtility::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The reply that the Service key is removed is handled by the Setup Utility screen.
    //103,255,120080
    if ( Error == errorMsg[0].toInt() && 255 == errorMsg[1].toInt() && 120080 == errorMsg[2].toInt() && 3 == errorMsg[3].toInt())
    {
        return true;// qDebug() << "No USB Key " << errorMsg[0] + "," + errorMsg[1] + "," + errorMsg[2];
        /*if(shouldChangeTime || shouldChangeNTP)
        {
            changeTimeAfterServiceKeyRemoved();
            return true;
        }
        else if(changedHostname)
        {
            shutdown();
            return true;
        }
        */
        //return true;
//        qDebug() << "got the 255";
//        if(shouldChangeTime) this->changeTimeAfterServiceKeyRemoved();
//        return true;l
    }
    else if (Error == errorMsg[0].toInt() && ErrVersionManagerDevice == errorMsg[1].toInt() && 120080 == errorMsg[2].toInt())
    {
        return true;
    }

    return _Mybase::isErrorMessageHandler( errorMsg );
}

void setupUtility::reboot()
{
    processingAButton = true;
    LOG_CLASS_INFO("SettingsWizard", "ChangedTimezone-Hostname-AstronomicalClock");
    LOG_WARN("shutdown button has been pressed", NULL);

    if (isDemoMode)
    {
        LOG_INFO("closing main window...", NULL);
        //mainWindow_->close();
        delete ui;
    }
    else
    {
        // turn brightness down on both displays
        //100,97,5 reboot
        //100,97,6 shutdown
        session_->send(SessionDev, SnShutdownRequest, "5"); // reboot
    }
    //this->turnOffDisplays();
    //mediaObject->deleteLater();
    //audioOutput->deleteLater();
    //settingTime = false;
}

void setupUtility::gcdSliderChanged(int value)
{
    // update db file
    this->updateDB("gcd", QString::number(value));
    //send message
    QString args = "";

    if(value)
    {
        args.append("1");
    }
    else
    {
        args.append("0");
    }

    session_->send(ConfigurationDev, CfgGCDReload, args);
    if(!value)
    {
        // turn it red
        this->ui->gcdSlider->setStyleSheet("QSlider#gcdSlider { width: 240px } QSlider#gcdSlider::groove:horizontal {border: 1px solid #999999; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #ffffff); height: 30px; margin: 0 0 0 0;} QSlider#gcdSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgb(255,0,0), stop:1 rgb(172, 199, 208)); border: 1px solid #5c5c5c; width: 50px; margin: -5px 0 -5px 0; border-radius: 3px;}");
    }
    else
    {
        //turn it green
        this->ui->gcdSlider->setStyleSheet("QSlider#gcdSlider { width: 240px } QSlider#gcdSlider::groove:horizontal {border: 1px solid #999999; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #ffffff); height: 30px; margin: 0 0 0 0;} QSlider#gcdSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 rgb(170,255,0), stop:1 rgb(172, 199, 208)); border: 1px solid #5c5c5c; width: 50px; margin: -5px 0 -5px 0; border-radius: 3px;}");
    }
/*
    // Add gcd call here
    python pythonProcess;

    QStringList Xargs;

    Xargs.append("customer");

    if(value)
    {
        Xargs.append(getDBValue("cBrightness"));
    }
    else
    {
        Xargs.append("0");
    }
    if (pythonProcess.execCommand("setBrightness", Xargs))
    {
        if (pythonProcess.getExitCode() == 0)
        {
           // ui->alertsLineEdit->setText(alertsText);
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }
*/
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   I M P O R T   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList setupUtility::buildImportConfigurationOptions(Network * locNetwork) {

    // Load the commonNetwork to the App first
    //processingAButton = true;

    this->setCommonNetwork(locNetwork);

    QStringList options;

    //QString databaseFileName = "settingswizard/scalesettings";
    //databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, filesystem::DBs);

   // QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/CHOOSE/THIS/DIRECTORY");

    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName = "";
    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    QStringList folders;

#if defined( Q_OS_LINUX )
    folders.push_back( "/media/thumb" );
    folders.push_back( "/media" );
#elif defined( Q_OS_WIN )
    folders.push_back( "D:\\" );
    folders.push_back( "E:\\" );
    folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( startDirStr );

    dialog.setWindowTitle( tr( "Import Network Settings" ) );
    dialog.setDirectory( startDirStr );
    dialog.setFilter( formatStr );
    dialog.setOptions( QFileDialog::DontConfirmOverwrite );
    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            fileName = selected[0] ;
    }


    if(fileName.isEmpty()) {//user clicked cancel
        processingAButton = false;

        return options;
    }

    //options.append("python/qmSettings/network/importConfigs.py");
    options.append(filesystem::absoluteFilePath("importConfigs.py",folderType::Python));
    options.append("-F");
    options.append(fileName);

    importFilename = fileName;
    options.append("-Z");

    //PASSPHRASE DEFINED HERE
    options.append(PASSPHRASE);
    //processingAButton = false;

    return options;
}

//////////////////////////////////////////////////////
//
//     D H C P   R A D I O   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::dhcpRadio_disableIPs()
{
    LOG_CLASS_INFO( "setupUtility", "dhcpRadio_disableIPs" );
    LOG_DEBUG( "dhcpRadio_disableIPs() START", NULL );

    if(this->ui->dhcpRadio->isChecked())
    {
        //LOG_DEBUG( "dhcpRadio_disableIPs() DHCPHostnameCheckbox->setEnabled(true) 1", NULL );
        this->ui->DHCPHostnameCheckbox->setEnabled(true);
        this->ui->subnetMaskLineEdit->setEnabled(false);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_2->setEnabled(false);
        this->ui->subnetMaskLineEdit_2->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_3->setEnabled(false);
        this->ui->subnetMaskLineEdit_3->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_4->setEnabled(false);
        this->ui->subnetMaskLineEdit_4->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->ipAddressLineEdit->setEnabled(false);
        this->ui->ipAddressLineEdit->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_2->setEnabled(false);
        this->ui->ipAddressLineEdit_2->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_3->setEnabled(false);
        this->ui->ipAddressLineEdit_3->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_4->setEnabled(false);
        this->ui->ipAddressLineEdit_4->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->gatewayLineEdit->setEnabled(false);
        this->ui->gatewayLineEdit->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_2->setEnabled(false);
        this->ui->gatewayLineEdit_2->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_3->setEnabled(false);
        this->ui->gatewayLineEdit_3->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_4->setEnabled(false);
        this->ui->gatewayLineEdit_4->setStyleSheet("color:black;background: rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->autoDNSRadio->setEnabled(true);
    }
    else // brute force enable to be sure
    {
        if( hostnameStayChecked_FIX )
        {
            LOG_DEBUG( "dhcpRadio_disableIPs() HostnameCB->setChecked(false) 1 but we are going to IGNORE IT bc hostnameStayChecked_FIX is TRUE", NULL );
        }
        else
        {
            LOG_DEBUG( "dhcpRadio_disableIPs() HostnameCB->setChecked(false) 1 go ahead and DO IT bc hostnameStayChecked_FIX is FALSE", NULL );
            this->ui->DHCPHostnameCheckbox->setChecked(false);
        }

        this->ui->subnetMaskLineEdit->setEnabled(true);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_2->setEnabled(true);
        this->ui->subnetMaskLineEdit_2->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_3->setEnabled(true);
        this->ui->subnetMaskLineEdit_3->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_4->setEnabled(true);
        this->ui->subnetMaskLineEdit_4->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->ipAddressLineEdit->setEnabled(true);
        this->ui->ipAddressLineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_2->setEnabled(true);
        this->ui->ipAddressLineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_3->setEnabled(true);
        this->ui->ipAddressLineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_4->setEnabled(true);
        this->ui->ipAddressLineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->gatewayLineEdit->setEnabled(true);
        this->ui->gatewayLineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_2->setEnabled(true);
        this->ui->gatewayLineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_3->setEnabled(true);
        this->ui->gatewayLineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_4->setEnabled(true);
        this->ui->gatewayLineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
    }

    LOG_DEBUG( "dhcpRadio_disableIPs() END", NULL );
}

void setupUtility::controlNICs(QString device, QString direction)
{
    LOG_CLASS_INFO( "setupUtility", "controlNICs" );
    LOG_DEBUG( "controlNICs() START", NULL );

    // device is eth0 | wlan0 | both
    // direction is up | down
    //processingAButton = true;

    python pythonProcess;
    QStringList args;
    QStringList devices;

        args.clear();
        args.append(device);
        args.append(direction);

        if (pythonProcess.execCommand("netInterface", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ;
            }
            else
            {
                ;// TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }

      //  processingAButton = false;

}

//////////////////////////////////////////////////////
//
//     S T A T I C   I P   R A D I O   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::staticIPRadio_enableIPs()
{
    LOG_CLASS_INFO( "setupUtility", "staticIPRadio_enableIPs" );
    LOG_DEBUG( "staticIPRadio_enableIPs() START", NULL );

    if(this->ui->staticIPRadio->isChecked())
    {
        //LOG_DEBUG( "staticIPRadio_enableIPs() DHCPHostnameCheckbox->setChecked(false) 1", NULL );
        this->ui->DHCPHostnameCheckbox->setChecked(false);
        //LOG_DEBUG( "staticIPRadio_enableIPs() DHCPHostnameCheckbox->setEnabled(false) 1", NULL );
        this->ui->DHCPHostnameCheckbox->setEnabled(false);
        this->ui->subnetMaskLineEdit->setEnabled(true);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_2->setEnabled(true);
        this->ui->subnetMaskLineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_3->setEnabled(true);
        this->ui->subnetMaskLineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_4->setEnabled(true);
        this->ui->subnetMaskLineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->ipAddressLineEdit->setEnabled(true);
        this->ui->ipAddressLineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_2->setEnabled(true);
        this->ui->ipAddressLineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_3->setEnabled(true);
        this->ui->ipAddressLineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_4->setEnabled(true);
        this->ui->ipAddressLineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->gatewayLineEdit->setEnabled(true);
        this->ui->gatewayLineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_2->setEnabled(true);
        this->ui->gatewayLineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_3->setEnabled(true);
        this->ui->gatewayLineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_4->setEnabled(true);
        this->ui->gatewayLineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->manualDNSRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(false);
        this->ui->autoDNSRadio->setEnabled(false);
        //this->ui->autoDNSRadio->isHidden();
    }
    else // brute force disable to be sure
    {
        this->ui->subnetMaskLineEdit->setEnabled(false);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_2->setEnabled(false);
        this->ui->subnetMaskLineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_3->setEnabled(false);
        this->ui->subnetMaskLineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->subnetMaskLineEdit_4->setEnabled(false);
        this->ui->subnetMaskLineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->ipAddressLineEdit->setEnabled(false);
        this->ui->ipAddressLineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_2->setEnabled(false);
        this->ui->ipAddressLineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_3->setEnabled(false);
        this->ui->ipAddressLineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->ipAddressLineEdit_4->setEnabled(false);
        this->ui->ipAddressLineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->gatewayLineEdit->setEnabled(false);
        this->ui->gatewayLineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_2->setEnabled(false);
        this->ui->gatewayLineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_3->setEnabled(false);
        this->ui->gatewayLineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->gatewayLineEdit_4->setEnabled(false);
        this->ui->gatewayLineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->autoDNSRadio->isVisible();
    }
}

void setupUtility::setTheStoredNetworkFields()
{
    LOG_CLASS_INFO( "setupUtility", "setTheStoredNetworkFields" );
    LOG_DEBUG( "setTheStoredNetworkFields() START", NULL );

    //processingAButton = true;

    this->connectDB();
    if(dbConnectionType == "0")
    {
        this->revealNetworkGroupBox(1,0);
        finalIsWireless =false;
    }
    else
    {
        finalIsWireless = true;
        this->revealNetworkGroupBox(3,0);
        ui->SSIDLineEdit->setText(dbConnectionName);
        //TBD preselect based on security and encryption type too
    }

    QStringList IPAddressParts;
    QStringList SubnetMaskParts;
    QStringList GatewayParts;
    QStringList DNS1Parts;
    QStringList DNS2Parts;
    QStringList DNS3Parts;

    if(dbipAddress != "0.0.0.0" && dbipAddress != "64641964" && dbipAddress != "")
    {
        IPAddressParts = dbipAddress.split(".", QString::KeepEmptyParts);

        if(IPAddressParts.size() == 4)
        {
            ui->ipAddressLineEdit->setText(IPAddressParts.at(0));
            ui->ipAddressLineEdit_2->setText(IPAddressParts.at(1));
            ui->ipAddressLineEdit_3->setText(IPAddressParts.at(2));
            ui->ipAddressLineEdit_4->setText(IPAddressParts.at(3));
        }
    }
    else
    {
        ui->ipAddressLineEdit->setText("");
        ui->ipAddressLineEdit_2->setText("");
        ui->ipAddressLineEdit_3->setText("");
        ui->ipAddressLineEdit_4->setText("");
    }

    if(dbnetmask != "0.0.0.0" && dbnetmask != "64641964" && dbnetmask != "")
    {
        SubnetMaskParts = dbnetmask.split(".", QString::KeepEmptyParts);

        if(SubnetMaskParts.size() == 4)
        {
            ui->subnetMaskLineEdit->setText(SubnetMaskParts.at(0));
            ui->subnetMaskLineEdit_2->setText(SubnetMaskParts.at(1));
            ui->subnetMaskLineEdit_3->setText(SubnetMaskParts.at(2));
            ui->subnetMaskLineEdit_4->setText(SubnetMaskParts.at(3));
        }
    }
    else
    {
        ui->subnetMaskLineEdit->setText("");
        ui->subnetMaskLineEdit_2->setText("");
        ui->subnetMaskLineEdit_3->setText("");
        ui->subnetMaskLineEdit_4->setText("");
    }

    if(dbgateway != "0.0.0.0" && dbgateway != "64641964" && dbgateway != "")
    {
        GatewayParts = dbgateway.split(".", QString::KeepEmptyParts);

        if(GatewayParts.size() == 4)
        {
            ui->gatewayLineEdit->setText(GatewayParts.at(0));
            ui->gatewayLineEdit_2->setText(GatewayParts.at(1));
            ui->gatewayLineEdit_3->setText(GatewayParts.at(2));
            ui->gatewayLineEdit_4->setText(GatewayParts.at(3));
        }
    }
    else
    {
        ui->gatewayLineEdit->setText("");
        ui->gatewayLineEdit_2->setText("");
        ui->gatewayLineEdit_3->setText("");
        ui->gatewayLineEdit_4->setText("");
    }

    if(dbdns1 != "0.0.0.0" && dbdns1 != "64641964" && dbdns1 != "")
    {
        DNS1Parts = dbdns1.split(".", QString::KeepEmptyParts);

        if(DNS1Parts.size() == 4)
        {
            ui->DNS1LineEdit->setText(DNS1Parts.at(0));
            ui->DNS1LineEdit_2->setText(DNS1Parts.at(1));
            ui->DNS1LineEdit_3->setText(DNS1Parts.at(2));
            ui->DNS1LineEdit_4->setText(DNS1Parts.at(3));
        }
    }
    else
    {
        ui->DNS1LineEdit->setText("");
        ui->DNS1LineEdit_2->setText("");
        ui->DNS1LineEdit_3->setText("");
        ui->DNS1LineEdit_4->setText("");
    }

    if(dbdns2 != "0.0.0.0" && dbdns2 != "64641964" && dbdns2 != "")
    {
        DNS2Parts = dbdns2.split(".", QString::KeepEmptyParts);

        if(DNS2Parts.size() == 4)
        {
            ui->DNS2LineEdit->setText(DNS2Parts.at(0));
            ui->DNS2LineEdit_2->setText(DNS2Parts.at(1));
            ui->DNS2LineEdit_3->setText(DNS2Parts.at(2));
            ui->DNS2LineEdit_4->setText(DNS2Parts.at(3));
        }
    }
    else
    {
        ui->DNS2LineEdit->setText("");
        ui->DNS2LineEdit_2->setText("");
        ui->DNS2LineEdit_3->setText("");
        ui->DNS2LineEdit_4->setText("");
    }

    if(dbdns3 != "0.0.0.0" && dbdns3 != "64641964" && dbdns3 != "")
    {
        DNS3Parts = dbdns3.split(".", QString::KeepEmptyParts);

        if(DNS3Parts.size() == 4)
        {
            ui->DNS3LineEdit->setText(DNS3Parts.at(0));
            ui->DNS3LineEdit_2->setText(DNS3Parts.at(1));
            ui->DNS3LineEdit_3->setText(DNS3Parts.at(2));
            ui->DNS3LineEdit_4->setText(DNS3Parts.at(3));
        }
    }
    else
    {
        ui->DNS3LineEdit->setText("");
        ui->DNS3LineEdit_2->setText("");
        ui->DNS3LineEdit_3->setText("");
        ui->DNS3LineEdit_4->setText("");
    }

    if (dbisStatic == "1")
    {
        ui->staticIPRadio->setChecked(true);
        ui->ipAddressLineEdit->setEnabled(true);
        ui->ipAddressLineEdit_2->setEnabled(true);
        ui->ipAddressLineEdit_3->setEnabled(true);
        ui->ipAddressLineEdit_4->setEnabled(true);

        ui->subnetMaskLineEdit->setEnabled(true);
        ui->subnetMaskLineEdit_2->setEnabled(true);
        ui->subnetMaskLineEdit_3->setEnabled(true);
        ui->subnetMaskLineEdit_4->setEnabled(true);

        ui->gatewayLineEdit->setEnabled(true);
        ui->gatewayLineEdit_2->setEnabled(true);
        ui->gatewayLineEdit_3->setEnabled(true);
        ui->gatewayLineEdit_4->setEnabled(true);

        ui->autoDNSRadio->setEnabled(false);
        dhcpRadio_disableIPs();
    }
    else
    {
        ui->dhcpRadio->setChecked(true);
        ui->ipAddressLineEdit->setDisabled(true);
        ui->ipAddressLineEdit_2->setDisabled(true);
        ui->ipAddressLineEdit_3->setDisabled(true);
        ui->ipAddressLineEdit_4->setDisabled(true);

        ui->subnetMaskLineEdit->setDisabled(true);
        ui->subnetMaskLineEdit_2->setDisabled(true);
        ui->subnetMaskLineEdit_3->setDisabled(true);
        ui->subnetMaskLineEdit_4->setDisabled(true);

        ui->gatewayLineEdit->setDisabled(true);
        ui->gatewayLineEdit_2->setDisabled(true);
        ui->gatewayLineEdit_3->setDisabled(true);
        ui->gatewayLineEdit_4->setDisabled(true);

        ui->autoDNSRadio->setEnabled(true);
        ui->manualDNSRadio->setEnabled(true);
        dhcpRadio_disableIPs();
    }

    //qDebug() << "dn1 is: " << dbdns1;
    //qDebug() << "dn2 is: " << dbdns2;
    //qDebug() << "dn3 is: " << dbdns3;
    //ui->DNS1LineEdit->setText(dbdns1);
    //ui->DNS2LineEdit->setText(dbdns2);
    //ui->DNS3LineEdit->setText(dbdns3);

    if(dbautoDNS == "1")
    {
        ui->autoDNSRadio->setChecked(true);
        ui->DNS1LineEdit->setDisabled(true);
        ui->DNS1LineEdit_2->setDisabled(true);
        ui->DNS1LineEdit_3->setDisabled(true);
        ui->DNS1LineEdit_4->setDisabled(true);

        ui->DNS1LineEdit->setEnabled(false);
        ui->DNS1LineEdit_2->setEnabled(false);
        ui->DNS1LineEdit_3->setEnabled(false);
        ui->DNS1LineEdit_4->setEnabled(false);

        ui->DNS2LineEdit->setDisabled(true);
        ui->DNS2LineEdit_2->setDisabled(true);
        ui->DNS2LineEdit_3->setDisabled(true);
        ui->DNS2LineEdit_4->setDisabled(true);

        ui->DNS2LineEdit->setEnabled(false);
        ui->DNS2LineEdit_2->setEnabled(false);
        ui->DNS2LineEdit_3->setEnabled(false);
        ui->DNS2LineEdit_4->setEnabled(false);

        ui->DNS3LineEdit->setDisabled(true);
        ui->DNS3LineEdit_2->setDisabled(true);
        ui->DNS3LineEdit_3->setDisabled(true);
        ui->DNS3LineEdit_4->setDisabled(true);

        ui->DNS3LineEdit->setEnabled(false);
        ui->DNS3LineEdit_2->setEnabled(false);
        ui->DNS3LineEdit_3->setEnabled(false);
        ui->DNS3LineEdit_4->setEnabled(false);

        autoDNSRadio_click();
    }
    else
    {
        ui->manualDNSRadio->setChecked(true);
        ui->DNS1LineEdit->setDisabled(false);
        ui->DNS1LineEdit_2->setDisabled(false);
        ui->DNS1LineEdit_3->setDisabled(false);
        ui->DNS1LineEdit_4->setDisabled(false);

        ui->DNS1LineEdit->setEnabled(true);
        ui->DNS1LineEdit_2->setEnabled(true);
        ui->DNS1LineEdit_3->setEnabled(true);
        ui->DNS1LineEdit_4->setEnabled(true);

        ui->DNS2LineEdit->setDisabled(false);
        ui->DNS2LineEdit_2->setDisabled(false);
        ui->DNS2LineEdit_3->setDisabled(false);
        ui->DNS2LineEdit_4->setDisabled(false);

        ui->DNS2LineEdit->setEnabled(true);
        ui->DNS2LineEdit_2->setEnabled(true);
        ui->DNS2LineEdit_3->setEnabled(true);
        ui->DNS2LineEdit_4->setEnabled(true);

        ui->DNS3LineEdit->setDisabled(false);
        ui->DNS3LineEdit_2->setDisabled(false);
        ui->DNS3LineEdit_3->setDisabled(false);
        ui->DNS3LineEdit_4->setDisabled(false);

        ui->DNS3LineEdit->setEnabled(true);
        ui->DNS3LineEdit_2->setEnabled(true);
        ui->DNS3LineEdit_3->setEnabled(true);
        ui->DNS3LineEdit_4->setEnabled(true);

        autoDNSRadio_click();
    }

    ui->securityCombo->setCurrentIndex(dbsecurity.toInt());
    ui->encryptionCombo->setCurrentIndex(dbencryption.toInt());

    // populate the wepkeys
    if(dbsecurity.toInt() == 0) // must be OPEN WEP
    {
        QStringList WEPkeys = populateWEPKeys();

        if(WEPkeys.size() > 0)
        {
            //Set the default key
            switch (WEPkeys.at(1).toInt())
            {
                case 0:
                    ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
                    break;
                case 1:
                    ui->wirelessAuthenticationDefaultKey2Radio->setChecked(true);
                    break;
                case 2:
                    ui->wirelessAuthenticationDefaultKey3Radio->setChecked(true);
                    break;
                case 3:
                    ui->wirelessAuthenticationdefaultKey4Radio->setChecked(true);
                    break;
            }

            // set the keys values
            if(WEPkeys.size() > 2)
            {
                for(int i = 0; i < ((WEPkeys.size()-2) / 2) ; i++)
                {
                    switch (WEPkeys.at((i*2) + 2).toInt())
                    {
                        case 1:
                            this->ui->wepKey1LineEdit->setText(WEPkeys.at((i*2) + 3));
                            break;
                        case 2:
                            this->ui->wepKey2LineEdit->setText(WEPkeys.at((i*2) + 3));
                            break;
                        case 3:
                            this->ui->wepKey3LineEdit->setText(WEPkeys.at((i*2) + 3));
                            break;
                        case 4:
                            this->ui->wepKey4LineEdit->setText(WEPkeys.at((i*2) + 3));
                            break;
                    }
                }
            }
        }
    }

    ui->eapPasswordLineEdit->setText(this->dbEapPassword);
    ui->eapUsernameLineEdit->setText(this->dbEapUsername);
    ui->wpaPassphraseLineEdit->setText(this->dbWpaPassphrase);

    ui->serverCertLabel->setText(dbServerCertFilename);
    if(dbServerCertFilename != "" || dbServerCertFilename.length() > 0)
    {
        ui->serverCertLabel->setStyleSheet("color: rgb(24,122,24);");
    }
    else
    {
        ui->serverCertLabel->setStyleSheet("color: rgb(165,25,27);");
    }
    //ui->chainFileLabel->setText(dbChainFilename);
    ui->clientCertLabel->setText(dbClientCertFilename);
    if(dbClientCertFilename != "" || dbClientCertFilename.length() > 0)
    {
        ui->clientCertLabel->setStyleSheet("color: rgb(24,122,24);");
    }
    else
    {
        ui->clientCertLabel->setStyleSheet("color: rgb(165,25,27);");
    }
    //processingAButton = false;
    LOG_DEBUG( "setTheStoredNetworkFields() END", NULL );
}

//////////////////////////////////////////////////////
//
//      N E T W O R K    T A B L E W I D G E T    S E L E C T I O N    C H A N G E D   S L O T
//
//////////////////////////////////////////////////////

void setupUtility::networkTableWidgetSelectionChanged(int row)
{
    LOG_CLASS_INFO( "setupUtility", "networkTableWidgetSelectionChanged" );
    LOG_DEBUG( "networkTableWidgetSelectionChanged() START", NULL );

    if(respondToTableViewClick || (ui->availableConnectionsTableView->selectedItems()).size() > 0)
    {
        QTableWidgetItem *wirelessItem;
        wirelessItem = this->ui->availableConnectionsTableView->item(row,1);

        userSelectedConnectionName = wirelessItem->text();

        tryingAHiddenSSID = false;
        if(row == 0) // WIRED - Has to be wired is always provided and at row 0
        {
            this->revealNetworkGroupBox(1, row);
        }
        else if(false /*this->useVPNConnection()*/ && row == 1) // VPN - User chose the VPN Connection
        {
            this->revealNetworkGroupBox(2, row);
        }
        else // WIRELESS - all else
        {
            this->revealNetworkGroupBox(3, row);
        }

        this->ui->availableConnectionsTableView->selectRow(row);

        QString connectionTypeLoc = getDBValue("connectionType");

        if(row == 0)
        {
            if(connectionTypeLoc == "0")
            {
                // Load values
                this->setTheStoredNetworkFields();
            }
            this->ui->SSIDLineEdit->setText("");
        }
        else if(row > 0)
        {
            QString connectionNameLoc = getDBValue("connectionName");

            //this->ui->SSIDLineEdit->setText(wirelessItem->text());

            if(connectionNameLoc == wirelessItem->text())
            {
                //load values
                this->setTheStoredNetworkFields();
            }
            else clearNetworkData();
            this->ui->SSIDLineEdit->setText(wirelessItem->text());

        }
        //QTableWidgetItem *wirelessItem;
        //wirelessItem = this->ui->availableConnectionsTableView->item(row,1);

        if(wirelessItem->text() == getDBValue("connectionName"))
        {
            //LOG_DEBUG( "networkTableWidgetSelectionChanged() IF wirelessItem->text() == getDBValue(connectionName)1", NULL );
            ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(getDBValue("channel")));
            ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(getDBValue("linkRate")));

            if(getDBValue("DHCPHostname") == "1")
            {
                //LOG_DEBUG( "networkTableWidgetSelectionChanged() IF getDBValue(DHCPHostname) == 1", NULL );

                ui->DHCPHostnameCheckbox->setEnabled(true);
                //LOG_DEBUG( "networkTableWidgetSelectionChanged() DHCPHostnameCheckbox->setChecked(true) 1", NULL );
                ui->DHCPHostnameCheckbox->setChecked(true);
                ui->DHCPHostnameWarning->show();
                ui->setHostnameBut->setEnabled(false);
                LOG_DEBUG( "hostnameLineEdit->setEnabled(false)", NULL );
                ui->hostnameLineEdit->setEnabled(false);
            }
            else
            {
                //LOG_DEBUG( "networkTableWidgetSelectionChanged() ELSE getDBValue(DHCPHostname) == 1", NULL );
                if(getDBValue("isStatic") == "1")
                {
                    //LOG_DEBUG( "networkTableWidgetSelectionChanged() IF isStatic == 1", NULL );
                    ui->DHCPHostnameCheckbox->setEnabled(false);
                    //LOG_DEBUG( "networkTableWidgetSelectionChanged() DHCPHostnameCheckbox->setChecked(false) 2", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);
                    ui->DHCPHostnameWarning->hide();
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true) 1", NULL );
                    ui->hostnameLineEdit->setEnabled(true);
                    ui->setHostnameBut->setEnabled(true);
                }
                else
                {
                    //LOG_DEBUG( "networkTableWidgetSelectionChanged() ELSE isStatic == 1", NULL );
                    ui->DHCPHostnameCheckbox->setEnabled(true);
                    //LOG_DEBUG( "networkTableWidgetSelectionChanged() DHCPHostnameCheckbox->setChecked(false) 3", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);
                    ui->DHCPHostnameWarning->hide();
                    ui->setHostnameBut->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true) 2", NULL );
                    //ui->hostnameLineEdit->setEnabled(true);
                }
            }

            if(getDBValue("automaticLinkRate") == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }

            if(ui->cbChannel->currentIndex() == 0 && ui->cbTransferRate->currentIndex() == 0)
            {
                ui->networkAdvancedBut->setIcon(QIcon());
                ui->networkAdvancedBut->setStyleSheet("");
            }
            else
            {

                ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
                howMany = 0;

                if(ui->cbTransferRate->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }

                if(!ui->chkboxTransferRateAuto->isChecked())
                {
                    howMany = howMany + 1;
                }

                if(ui->cbChannel->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }

                ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
            }
        }

        //processingAButton = false;
        if(ui->availableConnectionsTableView->currentRow() == 0)
        {
            ui->cbChannel->setCurrentIndex(0);
            ui->cbTransferRate->setCurrentIndex(0);
            ui->chkboxTransferRateAuto->setChecked(true);
            ui->networkAdvancedBut->setIcon(QIcon());
            ui->networkAdvancedBut->setStyleSheet("");

        }
    }
    respondToTableViewClick = true;
    LOG_DEBUG( "networkTableWidgetSelectionChanged() END", NULL );
}


//////////////////////////////////////////////////////
//
//      S E T   C O M M O N    N E T W O R K
//
//////////////////////////////////////////////////////
void setupUtility::setCommonNetwork(Network *locNetwork)
{
    LOG_CLASS_INFO( "setupUtility", "setCommonNetwork" );
    LOG_DEBUG( "setCommonNetwork() START", NULL );

    // Set global network settings.
    //processingAButton = true;

    locNetwork->setOperatorID(0);

    locNetwork->setMachineID(0);

    if(this->ui->ipAddressLineEdit->text() != "" && ui->ipAddressLineEdit_2->text() != "" && ui->ipAddressLineEdit_3->text() != "" && ui->ipAddressLineEdit_4->text() != "")
    {
        locNetwork->setIPAddress(this->ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
    }
    else
    {
        locNetwork->setIPAddress("64641964");
    }
    //qDebug() << "ipaddress is: " << locNetwork->IPAddress();
    locNetwork->setIsStatic(this->ui->staticIPRadio->isChecked());

    locNetwork->setIsAutoDNS(this->ui->autoDNSRadio->isChecked());

    if(this->ui->subnetMaskLineEdit->text() != "" &&  ui->subnetMaskLineEdit_2->text() != "" && ui->subnetMaskLineEdit_3->text() != "" && ui->subnetMaskLineEdit_4->text() != "")
    {
        locNetwork->setSubnetMask(this->ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
    }
    else
    {
        locNetwork->setSubnetMask("0"); //EAP
    }

    if(this->ui->gatewayLineEdit->text() != "" && ui->gatewayLineEdit_2->text() != "" && ui->gatewayLineEdit_3->text() != "" && ui->gatewayLineEdit_4->text() != "")
    {
        locNetwork->setGateway(this->ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text() + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
    }
    else
    {
        locNetwork->setGateway("0"); //EAP
    }

    if(this->ui->DNS1LineEdit->text() != "" && ui->DNS1LineEdit_2->text() != "" && ui->DNS1LineEdit_3->text() != "" && ui->DNS1LineEdit_4->text() != "")
    {
        locNetwork->setPrimaryDNS(this->ui->DNS1LineEdit->text() + "." + ui->DNS1LineEdit_2->text() + "." + ui->DNS1LineEdit_3->text() + "." + ui->DNS1LineEdit_4->text());
    }
    else
    {
        locNetwork->setPrimaryDNS("0"); //EAP
    }

    if(this->ui->DNS2LineEdit->text() != "" && ui->DNS2LineEdit_2->text() != "" && ui->DNS2LineEdit_3->text() != "" && ui->DNS2LineEdit_4->text() != "")
    {
        locNetwork->setSecondaryDNS(this->ui->DNS2LineEdit->text() + "." + ui->DNS2LineEdit_2->text() + "." + ui->DNS2LineEdit_3->text() + "." + ui->DNS2LineEdit_4->text());
    }
    else
    {
        locNetwork->setSecondaryDNS("0"); //EAP
    }


    if(this->ui->DNS3LineEdit->text() != "" && ui->DNS3LineEdit_2->text() != "" && ui->DNS3LineEdit_3->text() != "" && ui->DNS3LineEdit_4->text() != "")
    {
        locNetwork->setTertiaryDNS(this->ui->DNS3LineEdit->text() + "." + ui->DNS3LineEdit_2->text() + "." + ui->DNS3LineEdit_3->text() + "." + ui->DNS3LineEdit_4->text());
    }
    else
    {
        locNetwork->setTertiaryDNS("0"); //EAP
    }

    locNetwork->setIsWireless(this->ui->wirelessAdapterGroupBox->isVisible());

    locNetwork->setSSID(this->ui->SSIDLineEdit->text());

    locNetwork->setSecurityMethod(this->ui->securityCombo->currentText());

    locNetwork->setEncryption(this->ui->encryptionCombo->currentText());

    locNetwork->setVPNUsername("0");

    locNetwork->setVPNPassword("0");

    locNetwork->setIsVPN(false /*this->ui->vpnGroupBox->isVisible()*/);

    if(this->ui->wepKey1LineEdit->text() != "")
    {
        locNetwork->setWEPKey1(this->ui->wepKey1LineEdit->text());
    }
    else
    {
        locNetwork->setWEPKey1("0");
    }

    //qDebug() << "ui is:" << this->ui->wepKey1LineEdit->text();
    //qDebug() << "locnetwork key1 is: " << locNetwork->WEPKey1();

    if(this->ui->wepKey2LineEdit->text() != "")
    {
        locNetwork->setWEPKey2(this->ui->wepKey2LineEdit->text());
    }
    else
    {
        locNetwork->setWEPKey2("0");
    }
    if(this->ui->wepKey3LineEdit->text() != "")
    {
        locNetwork->setWEPKey3(this->ui->wepKey3LineEdit->text());
    }
    else
    {
        locNetwork->setWEPKey3("0");
    }
    if(this->ui->wepKey4LineEdit->text() != "")
    {
        locNetwork->setWEPKey4(this->ui->wepKey4LineEdit->text());
    }
    else
    {
        locNetwork->setWEPKey4("0");
    }
    // Let's get a fail-safe value in case no radio is checked
    locNetwork->setDefaultKey("1");

    if (this->ui->wirelessAuthenticationDefaultKey2Radio->isChecked())
    {
        locNetwork->setDefaultKey("2");
    }
    else if (this->ui->wirelessAuthenticationDefaultKey3Radio->isChecked())
    {
        locNetwork->setDefaultKey("3");
    }
    else if (this->ui->wirelessAuthenticationdefaultKey4Radio->isChecked())
    {
        locNetwork->setDefaultKey("4");
    }
    if(this->ui->wpaPassphraseLineEdit->text() != "" && this->ui->wpaPassPhraseFrame->isVisible())
    {
        locNetwork->setWPAPassPhrase(this->ui->wpaPassphraseLineEdit->text());
    }
    //else if(this->ui->wpaEAPPassphraseLineEdit->text() != "" && this->ui->wpaEAPTLSPassPhraseFrame->isVisible())
    //{
        //locNetwork->setWPAPassPhrase(this->ui->wpaEAPPassphraseLineEdit->text());
    //}
    if(this->ui->eapUsernameLineEdit->text() != "")
    {
        locNetwork->setEAPUsername(this->ui->eapUsernameLineEdit->text());
    }
    if(this->ui->eapPasswordLineEdit->text() != "")
    {
        locNetwork->setEAPPassword(this->ui->eapPasswordLineEdit->text());
    }
    //EAP-TLS
    if(this->ui->trustedRootStoreCombo->currentIndex() != 0)
    {
        locNetwork->setServerCertFilename(this->ui->trustedRootStoreCombo->itemData(this->ui->trustedRootStoreCombo->currentIndex()).toString());
    }
    if(this->ui->clientCertStoreCombo->currentIndex()!= 0)
    {
        locNetwork->setClientCertFilename(this->ui->clientCertStoreCombo->itemData(this->ui->clientCertStoreCombo->currentIndex()).toString());
    }

    /*if(this->ui->serverCertLabel->text() != "")
    {
        locNetwork->setServerCertFilename(this->ui->serverCertLabel->text());
    }
    //if(this->ui->chainFileLabel->text() != "")
    //{
       // locNetwork->setChainFilename(this->ui->chainFileLabel->text());
    //}
    if(this->ui->clientCertLabel->text() != "")
    {
        locNetwork->setClientCertFilename(this->ui->clientCertLabel->text());
    }
    */


    //processingAButton = false;
    //LOG_DEBUG( "setCommonNetwork() END", NULL );
}

void setupUtility::clearNetworkData()
{
    LOG_CLASS_INFO( "setupUtility", "clearNetworkData" );
    LOG_DEBUG( "clearNetworkData() START", NULL );

    this->ui->SSIDLineEdit->setText("");
    this->ui->securityCombo->setCurrentIndex(0);
    this->ui->encryptionCombo->setCurrentIndex(0);
    this->ui->wepKey1LineEdit->setText("");
    this->ui->wepKey2LineEdit->setText("");
    this->ui->wepKey3LineEdit->setText("");
    this->ui->wepKey4LineEdit->setText("");

    this->ui->DNS1LineEdit->setText("");
    this->ui->DNS1LineEdit_2->setText("");
    this->ui->DNS1LineEdit_3->setText("");
    this->ui->DNS1LineEdit_4->setText("");

    this->ui->DNS2LineEdit->setText("");
    this->ui->DNS2LineEdit_2->setText("");
    this->ui->DNS2LineEdit_3->setText("");
    this->ui->DNS2LineEdit_4->setText("");

    this->ui->DNS3LineEdit->setText("");
    this->ui->DNS3LineEdit_2->setText("");
    this->ui->DNS3LineEdit_3->setText("");
    this->ui->DNS3LineEdit_4->setText("");

    this->ui->autoDNSRadio->setChecked(false);
    this->ui->dhcpRadio->setChecked(false);     // emits signal dhcpRadio_disableIPs() on clicked/toggled

    // Per Darryl Needler on 8/12/2013 set default static and manual DNS
    this->ui->staticIPRadio->setChecked(true);
    //LOG_DEBUG( "clearNetworkData() DHCPHostnameCheckbox->setChecked(false) 1", NULL );
    this->ui->DHCPHostnameCheckbox->setChecked(false);
    this->ui->DHCPHostnameCheckbox->setEnabled(false);
    this->ui->manualDNSRadio->setChecked(true);

    this->ui->eapPasswordLineEdit->setText("");
    this->ui->eapUsernameLineEdit->setText("");

    this->ui->gatewayLineEdit->setText("");
    this->ui->gatewayLineEdit_2->setText("");
    this->ui->gatewayLineEdit_3->setText("");
    this->ui->gatewayLineEdit_4->setText("");

    this->ui->ipAddressLineEdit->setText("");
    this->ui->ipAddressLineEdit_2->setText("");
    this->ui->ipAddressLineEdit_3->setText("");
    this->ui->ipAddressLineEdit_4->setText("");

    this->ui->subnetMaskLineEdit->setText("");
    this->ui->subnetMaskLineEdit_2->setText("");
    this->ui->subnetMaskLineEdit_3->setText("");
    this->ui->subnetMaskLineEdit_4->setText("");

    this->ui->wpaPassphraseLineEdit->setText("");
    this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
    this->ui->serverCertLabel->setText("");
    //this->ui->chainFileLabel->setText("");
    this->ui->clientCertLabel->setText("");
    this->ui->cbChannel->setCurrentIndex(0);
    this->ui->cbTransferRate->setCurrentIndex(0);
    this->ui->chkboxTransferRateAuto->setChecked(true);
    this->ui->networkAdvancedBut->setIcon(QIcon());
    this->ui->networkAdvancedBut->setStyleSheet("");
    ui->trustedRootStoreCombo->setCurrentIndex(0);
    ui->clientCertStoreCombo->setCurrentIndex(0);

    //LOG_DEBUG( "clearNetworkData() END", NULL );
}

void setupUtility::addNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "addNetworkBut_click" );
    LOG_DEBUG( "addNetworkBut_click() START", NULL );

    ui->networkLoadingLabel->setText("Select\nDevice");

    ui->yesGatewayBut->hide();
    ui->noGatewayBut->hide();
    ui->newWiredNetworkBut->raise();
    ui->newWirelessNetworktBut->raise();
    ui->newWiredNetworkBut->show();
    ui->newWirelessNetworktBut->show();
    ui->gbNetworkLoading->raise();
    ui->gbNetworkLoading->show();
    //LOG_DEBUG( "addNetworkBut_click() END", NULL );
}

void setupUtility::newWirelessNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "newWirelessNetworkBut_click" );
    LOG_DEBUG( "newWirelessNetworkBut_click() START", NULL );

    newConnectionFlag = "Wireless";
    this->clearNetworkData();
    tryingAHiddenSSID = true;
    this->ui->availableConnectionsTableView->clearSelection();
    ui->cbTransferRate->setCurrentIndex(0);
    ui->chkboxTransferRateAuto->setChecked(true);
    ui->networkAdvancedBut->setIcon(QIcon());
    ui->networkAdvancedBut->setStyleSheet("");
    this->ui->cbChannel->setCurrentIndex(0);

    this->revealNetworkGroupBox(4,0);

    ui->yesGatewayBut->hide();
    ui->noGatewayBut->hide();
    ui->newWiredNetworkBut->lower();
    ui->newWirelessNetworktBut->lower();
    ui->newWiredNetworkBut->hide();
    ui->newWirelessNetworktBut->hide();

    ui->trustedRootStoreCombo->setCurrentIndex(0);
    ui->clientCertStoreCombo->setCurrentIndex(0);
   // ui->gbNetworkLoading->lower();
    ui->gbNetworkLoading->hide();

    //LOG_DEBUG( "newWirelessNetworkBut_click() END", NULL );
}

void setupUtility::newWiredNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "newWiredNetworkBut_click" );
    LOG_DEBUG( "newWiredNetworkBut_click() START", NULL );

    newConnectionFlag = "Wire";
    this->clearNetworkData();
    tryingAHiddenSSID = false;
    this->ui->availableConnectionsTableView->clearSelection();
    this->ui->availableConnectionsTableView->selectRow(0);
    ui->cbTransferRate->setCurrentIndex(0);
    ui->chkboxTransferRateAuto->setChecked(true);
    ui->networkAdvancedBut->setIcon(QIcon());
    ui->networkAdvancedBut->setStyleSheet("");
    this->ui->cbChannel->setCurrentIndex(0);
    ui->trustedRootStoreCombo->setCurrentIndex(0);
    ui->clientCertStoreCombo->setCurrentIndex(0);

    this->revealNetworkGroupBox(1,0);

    ui->yesGatewayBut->hide();
    ui->noGatewayBut->hide();
    ui->newWiredNetworkBut->lower();
    ui->newWirelessNetworktBut->lower();
    ui->newWiredNetworkBut->hide();
    ui->newWirelessNetworktBut->hide();

    //ui->gbNetworkLoading->lower();
    ui->gbNetworkLoading->hide();

    //LOG_DEBUG( "newWiredNetworkBut_click() END", NULL );
}

//////////////////////////////////////////////////////
//
//      R E V E A L   N E T W O R K   G R O U P B O X
//
//////////////////////////////////////////////////////

void setupUtility::revealNetworkGroupBox(int box, int row)
{
    LOG_CLASS_INFO( "setupUtility", "revealNetworkGroupBox" );
    LOG_DEBUG( "revealNetworkGroupBox() START", NULL );

    if( hostnameStayChecked_FIX )
    {
        LOG_DEBUG( "revealNetworkGroupBox() hostnameStayChecked_FIX is TRUE so do nothing", NULL );
    }
    else
    {
        LOG_DEBUG( "revealNetworkGroupBox() hostnameStayChecked_FIX is FALSE so clear hostname checkbox", NULL );
        this->clearNetworkData();
    }

    switch (box)
    {
    case 1: //WIRED
        //LOG_DEBUG( "revealNetworkGroupBox() case 1", NULL );
        // this->ui->dnsGroupBox->hide();
        // this->ui->vpnGroupBox->hide();
        // this->ui->wiredAdapterGroupBox->setGeometry(320,7,331,161);
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 214, 256, 209 ) ) );
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 1, 256, 209 ) ) );

       // this->ui->macAddressFixedLabel->setGeometry(494, 326, 101, 20);
       // this->ui->macAddressLabel->setGeometry(595, 327, 138,20);

        this->ui->wirelessAdapterGroupBox->hide();
        this->ui->wirelessAuthenticationGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();

        this->ui->staticIPRadio->setChecked(true);
        this->ui->manualDNSRadio->setChecked(true);
        this->ui->autoDNSRadio->setEnabled(false);

        break;
    case 2:  // VPN
        this->ui->wiredAdapterGroupBox->hide();
        this->ui->wirelessAdapterGroupBox->hide();
        this->ui->dnsGroupBox->hide();
        this->ui->wirelessAuthenticationGroupBox->hide();
      //  this->ui->vpnGroupBox->show();
      //  this->ui->macAddressFixedLabel->setGeometry(494, 240, 101, 20);
        //this->ui->macAddressLabel->setGeometry(595, 241, 138, 20);

        break;
    case 3: // WIRELESS
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 1, 256, 209 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 214, 256, 201 ) ) );

        //this->ui->vpnGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->wirelessAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
        this->ui->wirelessAuthenticationGroupBox->show();

        this->ui->securityCombo->setCurrentIndex(0);
        this->ui->encryptionCombo->setCurrentIndex(0);

      //  this->ui->macAddressFixedLabel->setGeometry(314, 326, 101, 20);
        //this->ui->macAddressLabel->setGeometry(415, 327, 138,20);

        QTableWidgetItem *wirelessItem;
        wirelessItem = this->ui->availableConnectionsTableView->item(row,1);

        this->ui->SSIDLineEdit->setText(wirelessItem->text());
        if(wirelessItem->text() == getDBValue("connectionName"))
        {
            ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(getDBValue("channel")));
            ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(getDBValue("linkRate")));
            if(getDBValue("automaticLinkRate") == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }
            if(ui->cbChannel->currentIndex() == 0 && ui->cbTransferRate->currentIndex() == 0)
            {
                ui->networkAdvancedBut->setIcon(QIcon());
                ui->networkAdvancedBut->setStyleSheet("");
            }
            else
            {

                ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
                howMany = 0;
                if(ui->cbTransferRate->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(!ui->chkboxTransferRateAuto->isChecked())
                {
                    howMany = howMany + 1;
                }
                if(ui->cbChannel->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
            }
        }
        this->ui->dhcpRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(true);
        break;
    case 4: // HIDDEN OR NEW WIRELESS
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 1, 256, 209 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 214, 256, 201 ) ) );

        //this->ui->vpnGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->wirelessAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
        this->ui->wirelessAuthenticationGroupBox->show();

        this->ui->securityCombo->setCurrentIndex(0);
        this->ui->encryptionCombo->setCurrentIndex(0);

        //this->ui->macAddressFixedLabel->setGeometry(314, 326, 101, 20);
        //this->ui->macAddressLabel->setGeometry(415, 327, 138,20);

        //QTableWidgetItem *wirelessItem;
        //wirelessItem = this->ui->availableConnectionsTableView->item(row,1);

        this->ui->SSIDLineEdit->setText("");
        this->ui->dhcpRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(true);
        break;
    case 5: // HIDDEN OR NEW WIRELESS
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 1, 256, 209 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 214, 256, 201 ) ) );

        //this->ui->vpnGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->wirelessAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
        this->ui->wirelessAuthenticationGroupBox->show();

        //this->ui->securityCombo->setCurrentIndex(0);
        //this->ui->encryptionCombo->setCurrentIndex(0);

        //this->ui->macAddressFixedLabel->setGeometry(314, 326, 101, 20);
        //this->ui->macAddressLabel->setGeometry(415, 327, 138,20);

        //QTableWidgetItem *wirelessItem;
        //wirelessItem = this->ui->availableConnectionsTableView->item(row,1);

        //this->ui->SSIDLineEdit->setText("");
        //this->ui->dhcpRadio->setChecked(true);
        //this->ui->autoDNSRadio->setChecked(true);

        //loop through avaiableNetworks table and select the connection

        /*bool foundIt = false;
        for(int i=0; i < ui->availableConnectionsTableView->rowCount() && !foundIt; i++)
        {

        }
        break;
        */
    }

    //LOG_DEBUG( "revealNetworkGroupBox() END", NULL );
}

void setupUtility::revealNetworkGroupBoxPrefilled(int isWireless)
{
    LOG_CLASS_INFO( "setupUtility", "revealNetworkGroupBoxPrefilled" );
    LOG_DEBUG( "revealNetworkGroupBoxPrefilled() START", NULL );

    //this->clearNetworkData();
    switch (isWireless)
    {
    case 1: //WIRED
        //this->ui->dnsGroupBox->hide();
        //this->ui->vpnGroupBox->hide();
//        this->ui->wiredAdapterGroupBox->setGeometry(320,7,331,161);
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 214, 256, 201 ) ) );
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 1, 256, 209 ) ) );

       // this->ui->macAddressFixedLabel->setGeometry(494, 326, 101, 20);
       // this->ui->macAddressLabel->setGeometry(595, 327, 138,20);

        this->ui->wirelessAdapterGroupBox->hide();
        this->ui->wirelessAuthenticationGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
    //    this->ui->macAddressFixedLabel->setGeometry(340, 179, 121, 20);
   //     this->ui->macAddressLabel->setGeometry(468, 180, 171, 20);
        //this->ui->staticIPRadio->setChecked(true);
        //this->ui->manualDNSRadio->setChecked(true);
        //this->ui->vpnGroupBox->hide();
        //this->ui->wirelessAuthenticationBut->hide();


        break;
    case 2:  // VPN
        this->ui->wiredAdapterGroupBox->hide();
        this->ui->wirelessAdapterGroupBox->hide();
        this->ui->dnsGroupBox->hide();
        this->ui->wirelessAuthenticationGroupBox->hide();
      //  this->ui->vpnGroupBox->show();
      //  this->ui->macAddressFixedLabel->setGeometry(494, 240, 101, 20);
        //this->ui->macAddressLabel->setGeometry(595, 241, 138, 20);

        break;
    case 3: // WIRELESS
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 1, 256, 209 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 214, 256, 201 ) ) );

        //this->ui->vpnGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->wirelessAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
        this->ui->wirelessAuthenticationGroupBox->show();
        break;
    }

    //LOG_DEBUG( "revealNetworkGroupBoxPrefilled() END", NULL );
}

//////////////////////////////////////////////////////
//
//      L O A D   W I R E L E S S   S E C U R I T Y   M E T H O D S   C O M B O
//
//////////////////////////////////////////////////////

void setupUtility::loadWirelessSecurityMethodsCombo()
{
    LOG_CLASS_INFO( "setupUtility", "loadWirelessSecurityMethodsCombo" );
    LOG_DEBUG( "loadWirelessSecurityMethodsCombo() START", NULL );

    this->ui->securityCombo->clear();
    this->ui->securityCombo->addItem("OPEN", QString("Open"));
    this->ui->securityCombo->addItem("WPA-Personal", QString("WPA-Personal"));
    this->ui->securityCombo->addItem("WPA EAP-TLS", QString("WPA EAP-TLS"));
    this->ui->securityCombo->addItem("WPA-Enterprise (PEAP)", QString("WPA-Enterprise (PEAP)"));
    this->ui->securityCombo->addItem("WPA-Enterprise (LEAP)", QString("WPA-Enterprise (LEAP)"));
    this->ui->securityCombo->addItem("WPA2-Personal", QString("WPA2-Personal"));
    this->ui->securityCombo->addItem("WPA2 EAP-TLS", QString("WPA2 EAP-TLS"));
    this->ui->securityCombo->addItem("WPA2-Enterprise (PEAP)", QString("WPA2-Enterprise (PEAP)"));
    this->ui->securityCombo->addItem("WPA2-Enterprise (LEAP)", QString("WPA2-Enterrpise (LEAP)"));
    this->ui->securityCombo->addItem("802.1X (PEAP)", QString("802.1X (PEAP)"));
    this->ui->securityCombo->addItem("802.1X (LEAP)", QString("802.1X (LEAP)"));

    this->setWirelessNetworkEncryptionCombo(0);
    this->ui->securityCombo->setCurrentIndex(0);
    this->ui->encryptionCombo->setCurrentIndex(0);

    //LOG_DEBUG( "loadWirelessSecurityMethodsCombo() END", NULL );
}

//////////////////////////////////////////////////////
//
//      D A T E   S E L E C T I O N   S L O T
//
//////////////////////////////////////////////////////

void setupUtility::datePicked(QDate selectionDate)
{
    pickedDate->setDate(selectionDate.year(),selectionDate.month(),selectionDate.day());  // ->setDate(selectionDate.currentDate()); //->setDate(selectionDate);
    // Need to set the User choice of day so as the calendar advances it uses the previous User choice.
    // When decrementing the day because the month does have that many days,
    // the new lower day is used across all months as the User changes the months in
    // the calendar widget.

    tempDayOfMonth = selectionDate.day();
}

//////////////////////////////////////////////////////
//
//      S E T   W I R E L E S S   N E T W O R K   E N C R Y P T I O N   C O M B O
//
//////////////////////////////////////////////////////

void setupUtility::setWirelessNetworkEncryptionCombo(int securityMethodItemNum)
{
    ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication"));

    switch (securityMethodItemNum)
    {
    case 0:
    {
        // Open
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->encryptionCombo->addItem("None",QString("None"));
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        //this->ui->wirelessAuthenticationDefaultKey1Radio->setTabOrder();
        this->ui->wepKeyFrame->show();

    }
        break;
    case 1:
    {
        // WPA-Personal
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->wpaPassPhraseFrame->show();
    }
        break;
    case 2:
    {
        // WPA EAP-TLS
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->show();
        this->ui->openNoneFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();

    }
        break;
    case 3:
    {
        // WPA-Enterprise (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;
    case 4:
    {
        // WPA-Enterprise (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;
    case 5:
    {
        // WPA2-Personal
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->wpaPassPhraseFrame->show();
    }
        break;
    case 6:
    {
        // WPA2 EAP-TLS
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->show();
        this->ui->openNoneFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();

    }
        break;
    case 7:
    {
        // WPA2-Enterprise (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();

    }
        break;
    case 8:
    {
        // WPA2-Enterprise (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();

    }
        break;
    case 9:
    {
        // 802.1X (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;
    case 10:
    {
        // 802.1X (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->wpaEAPTLSPassPhraseFrame->hide();
        this->ui->openNoneFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;

    }

    this->ui->encryptionCombo->setCurrentIndex(0);

    //pickedDate->setDate(selectionDate.year(),selectionDate.month(),selectionDate.day());  // ->setDate(selectionDate.currentDate()); //->setDate(selectionDate);
}

//////////////////////////////////////////////////////
//
//   A S T R O N O M I C A L   C H E C K B O X   S L O T
//
//          1 2  or  2 4   H O U R   C L O C K
//
//////////////////////////////////////////////////////

void setupUtility::astronomicalCheckboxValueChanged(int value)
{
    if(value == 0) // Unchecked
    {
        this->ui->timeEdit->setDisplayFormat("hh:mm:ss AP");
        ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->HourSection));
        ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->AmPmSection));
        ui->ampmBut->show();
    }
    else // checked
    {
        this->ui->timeEdit->setDisplayFormat("HH:mm:ss");
        ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->HourSection));
        ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->AmPmSection));
        ui->ampmBut->hide();
    }
   // this->ui->dateEdit->setFocus();
}

//////////////////////////////////////////////////////
//
//       C A L I B R A T E   S C R E E N    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::calibrateScreenBut_click()
{
   //this->ui->brightnessSpinBox->setFocus();
    //processingAButton = true;
    QProcess calibrateProcess;
    QStringList options;
    //options.append("python/qmSettings/display/qmSettingsCalibrateTouchscreen.py");
    options.append(filesystem::absoluteFilePath("calibrateTouchscreen.py",folderType::Python));
    calibrateProcess.start("python", options);
    if(!calibrateProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        // Start configuration error.
        //return;
    }

    if(!calibrateProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
        //return "";
        // Finish configuration error.
    }
    ui->alertsLineEdit->setText(tr("Touch screen has been calibrated."));
    updateDB("displayTab", "1");
    ui->mainTab->setTabIcon(0,passIcon);
    //processingAButton = false;

    // force screen repaint
    repaint();
}

//////////////////////////////////////////////////////
//
//      T E S T    V O L U M E    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::testVolumeBut_click()
{
    //processingAButton = true;
    if (this->ui->operatorSoundBut->isChecked())
    {
        displayDevice = "operator";
        // Use setBeeper.py

        // Set backlight of both displays to max
        python pythonProcess;
        QStringList args;

        args.append(QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55)));

        ////qDebug() << " operator volume is: " << QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55));
        if (pythonProcess.execCommand("sampleBeep", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ui->alertsLineEdit->setText(tr("Operator volume successfully set."));
                //TBD - updateCustomerVolume();
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }
    }
    else if(this->ui->customerSoundBut->isChecked())
    {
        displayDevice = "customer";
        // use setVolume.py
        python pythonProcess;

        // Set backlight of both displays to max
        QStringList args;

        args.append(ui->volumeLabel->text());

        ////qDebug() << "customer volume is: " << ui->volumeLabel->text();

        if (pythonProcess.execCommand("setVolume", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                currentPlaybackIndex = 0;

                playAudioFile();//TBD - updateCustomerVolume();
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }
    }
    else
    {
        //processingAButton = false;

        return;
    }

    /* //this->ui->volumeSpinBox->setFocus();
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    // make sure to create using new, since effect has to be alive as long as the target widget is using it.
    opacityEffect->setOpacity(1.0); // initially widget should be visible
    this->ui->volumeResultsLabel->setGraphicsEffect(opacityEffect);
    QPropertyAnimation* anim = new QPropertyAnimation(this);
    anim->setTargetObject(opacityEffect);
    anim->setPropertyName("opacity");
    anim->setDuration(4500);
    anim->setStartValue(opacityEffect->opacity());
    anim->setEndValue(0);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
*/
    //processingAButton = false;
}

void setupUtility::playAudioFile()
{
    //processingAButton = true;

    if (currentPlaybackIndex < stereoTestFiles.length())
    {
        QString media( filesystem::absoluteFilePath(stereoTestFiles.at(currentPlaybackIndex),folderType::Audio) );

        player->load( media, false );
        player->setVolume( 1.0 );

        player->play();
        currentPlaybackIndex++;
    }
    else
    {
       player->stop();
       //mediaObject->deleteLater();
       //audioOutput->deleteLater();
    }
    //processingAButton = false;
}

//////////////////////////////////////////////////////
//
//       S E T   B R I G H T N E S S   I N D I C A T O R S
//
//////////////////////////////////////////////////////

void setupUtility::setBrightnessIndicators(int value)
{
    QString objectName;
    for(int i=0; i <=10; i++)
    {
        objectName = "brightnessIndicator" + QString::number(i);
        if(i <=value/10 && value != 0)
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(:/images/screens/brightnessLED.png);");
        }
        else
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(:/images/screens/brightnessLEDoff.png);");
        }
    }
}

//////////////////////////////////////////////////////
//
//       S E T   V O L U M E   I N D I C A T O R S
//
//////////////////////////////////////////////////////

void setupUtility::setVolumeIndicators(int value)
{
    QString objectName;
    for(int i = 0; i <= 10; i++)
    {
        objectName = "volumeIndicator" + QString::number(i);

        if (i <= value/10 && value != 0) //turn on
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);border-image:url(:/images/screens/brightnessLED.png) 0 0 0 0 stretch stretch;");
        //    this->ui->volumeResultsLabel->setText(tr("Volume is on"));

        }
        else // turn off
        {
            if(value == 0)
            {
               this->ui->volumeResultsLabel->setText(tr("Muted"));
            }
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(130, 130, 130);border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");

        }
       /* QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
        // make sure to create using new, since effect has to be alive as long as the target widget is using it.
        opacityEffect->setOpacity(1.0); // initially widget should be visible
        this->ui->volumeResultsLabel->setGraphicsEffect(opacityEffect);
        QPropertyAnimation* anim = new QPropertyAnimation(this);
        anim->setTargetObject(opacityEffect);
        anim->setPropertyName("opacity");
        anim->setDuration(4500);
        anim->setStartValue(opacityEffect->opacity());
        anim->setEndValue(0);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        */
    }
    this->ui->volumeResultsLabel->setText("");
}

//////////////////////////////////////////////////////
//
//        M U T E   B U T   C H A N G E D
//
//////////////////////////////////////////////////////

void setupUtility::muteButChanged(bool checked)
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    //volume = this->ui->volumeSpinBox->value();
    if(checked)
    {
        this->ui->muteBut->setStyleSheet("border-image:url(:/images/screens/volumeOnSmall.png) 0 0 0 0 stretch stretch;");
        this->ui->mutedResultsLabel->setText("");

        if(volume == 0)
        {
            this->ui->volumeSpinBox->setValue(volume+1);
            if(ui->operatorSoundBut->isChecked())
            {
                this->ui->volumeLabel->setText(tr("on"));
            }
            else
            {
                this->ui->volumeLabel->setText(QString::number(volume+1));

            }
        }
        if(!ui->operatorSoundBut->isChecked())
        {
            //this->ui->volumeLabel->setText(QString::number(this->ui->volumeSpinBox->value()));
            this->ui->volumeLabel->setText(QString::number(volume));
            this->ui->volumeSpinBox->setValue(volume);
        }
        else
        {
            //operator volume
            this->ui->volumeLabel->setText(tr("on"));
            this->ui->volumeSpinBox->setValue(1);
        }
        this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    }
    else
    {
        this->ui->muteBut->setStyleSheet("border-image:url(:/images/screens/volumeOffSmall.png) 0 0 0 0 stretch stretch;");
        this->ui->mutedResultsLabel->setText(tr("Muted"));
        if(ui->operatorSoundBut->isChecked())
        {
            ui->volumeLabel->setText(tr("off"));
        }
        else
        {
            volume = this->ui->volumeSpinBox->value();
            this->ui->volumeLabel->setText("0");
        }
        this->ui->volumeSpinBox->setValue(0);
        this->setVolumeIndicators(0);
    }
}

//////////////////////////////////////////////////////
//
//       C H A N G E   O P E R A T O R   D I S P L A Y
//
//         B R I G H T N E S S   B U T T O N S
//
//////////////////////////////////////////////////////

void setupUtility::changedOperatorSoundButtons()
{
    this->ui->operatorSoundBut->setChecked(true);
    this->ui->operatorSoundButText->setStyleSheet("color: black;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLED.png) 0 0 0 0 stretch stretch;");
    this->ui->customerSoundButText->setStyleSheet("color: black;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    this->ui->customerSoundBut->setChecked(false);
    loadSoundVolumeValues("o");
    this->ui->volumeSpinBox->setMinimum(0);
    this->ui->volumeSpinBox->setMaximum(1);
    this->ui->testVolumeBut->hide();
}

void setupUtility::changedCustomerSoundButtons()
{
    ui->volumeSpinBox->setMinimum(0);
    ui->volumeSpinBox->setMaximum(100);
    this->ui->customerSoundBut->setChecked(true);
    this->ui->customerSoundButText->setStyleSheet("color: black;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLED.png) 0 0 0 0 stretch stretch;");
    this->ui->operatorSoundButText->setStyleSheet("color: black;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    this->ui->operatorSoundBut->setChecked(false);
    loadSoundVolumeValues("c");
    firstTimeCustomerVolume = false;
    this->ui->testVolumeBut->show();
}

void setupUtility::changedOperatorDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->operatorScreenBut->setChecked(true);
    this->ui->operatorDisplayButText->setStyleSheet("color: black;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLED.png) 0 0 0 0 stretch stretch;");
    if (!session_->isPrepack())
    {
        this->ui->customerDisplayButText->setStyleSheet("color: black;");
        this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    }
    this->ui->customerScreenBut->setChecked(false);
    loadDisplayBrightnessValues("o");
}

//////////////////////////////////////////////////////
//
//       C H A N G E   C U S T O M E R   D I S P L A Y
//
//         B R I G H T N E S S   B U T T O N S
//
//////////////////////////////////////////////////////

void setupUtility::changedCustomerDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->customerScreenBut->setChecked(true);
    this->ui->customerDisplayButText->setStyleSheet("color: white;");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLED.png) 0 0 0 0 stretch stretch;");
    this->ui->operatorDisplayButText->setStyleSheet("color: black;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;border-image:url(:/images/screens/brightnessLEDoff.png) 0 0 0 0 stretch stretch;");
    this->ui->operatorScreenBut->setChecked(false);
    loadDisplayBrightnessValues("c");
    /*
    if(firstTimeCustomerBrightnessDisplay)
    {
        loadDisplayBrightnessValues("c");
        firstTimeCustomerBrightnessDisplay = false;
    }
    */
}

//////////////////////////////////////////////////////
//
//       V O L U M E   S P I N B O X    V A L U E
//
//                 C H A N G E D
//
//////////////////////////////////////////////////////

void setupUtility::volumeSpinBoxValueChanged(int percentage)
{
  //  this->ui->lumeBut->setText(tr("Set volume to ") + QString::number(percentage) + tr("%"));
    // It's always a number so does not need translated

   // qDebug() << "percentage is: " << percentage;
   // qDebug() << "volume     is: " << volume;


    if(ui->operatorSoundBut->isChecked())
    {
        if(ui->volumeSpinBox->value() == 1)
        {
            ui->volumeLabel->setText(tr("on"));
        }
        else
        {
            ui->volumeLabel->setText(tr("off"));
        }

        QFont soundPercentageFont = ui->volumeLabel->font();
        if("EN.US" == mainWindow_->language()) // English
        {
            soundPercentageFont.setPointSize(58);
        }
        else soundPercentageFont.setPointSize(25);
        ui->volumeLabel->setFont(soundPercentageFont);
    }
    else
    {
        volume = percentage;
        this->ui->volumeLabel->setText(QString::number(percentage));
        QFont soundPercentageFont = ui->volumeLabel->font();
        soundPercentageFont.setPointSize(58);
        ui->volumeLabel->setFont(soundPercentageFont);
    }
    setVolumeIndicators(percentage);
    if(percentage > 0)
    {
        this->ui->muteBut->setChecked(true);
    }
    else
    {
        this->ui->muteBut->setChecked(false);
    }
}

//////////////////////////////////////////////////////
//
//       R E T R A N S L A T E   U I
//
//////////////////////////////////////////////////////

void setupUtility::resizeControls()
{
    QFont newFont = ui->calibrateScreenBut->font();
    QFont soundPercentageFont = ui->volumeLabel->font();

    if("EN.US" == mainWindow_->language()) // English
    {
        newFont.setPointSize(24);

    }
    else newFont.setPointSize(16);

    ui->calibrateScreenBut->setFont(newFont);
    ui->setBrightnessBut->setFont(newFont);
    ui->setHostnameBut->setFont(newFont);

    if("EN.US" == mainWindow_->language()) // English
    {
        soundPercentageFont.setPointSize(58);
    }
    else newFont.setPointSize(25);
    QLocale::Language lang = QLocale::English;
    if("EN.US" == mainWindow_->language())
    {
        lang = QLocale::English;
    }
    else if("EN.CA" == mainWindow_->language()) //Could be different than English
    {
        lang = QLocale::English;
    }
    else if("ES.MX" == mainWindow_->language()) // Spanish
    {
        lang = QLocale::Spanish;
    }
    else if("FR.CA" == mainWindow_->language()) // French
    {
        lang = QLocale::French;
    }

    ui->dateEdit->setLocale(lang);

    update();

}

void setupUtility::translate()
{
    blockOnTranslate= true;

    ui->retranslateUi(this);
    this->resizeControls();

    // update connections to update translations in table view
    if(isReadyToTranslate)
    {
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
        if( weHaveServiceMode || settings->rebootUnprotectedAllowed() )
        {
            this->ui->availableConnectionsTableView->clear();
            loadNetworkConnections();
        }

        this->ui->alertsLineEdit->setText(tr("Change Display Settings"));       // set to default text

        // The font size needs to update as well and immediately for the ON/OFF fields when
        // the language option is pressed. There is a possibility that the word ON may overflow its bounds

        int curPercent = this->ui->volumeLabel->text().toInt();
        volumeSpinBoxValueChanged(curPercent);
    }

    QLocale::Language lang = QLocale::English;
    if("EN.US" == mainWindow_->language())
    {
        lang = QLocale::English;
    }
    else if("EN.CA" == mainWindow_->language()) //Could be different than English
    {
        lang = QLocale::English;
    }
    else if("ES.MX" == mainWindow_->language()) // Spanish
    {
        lang = QLocale::Spanish;
    }
    else if("FR.CA" == mainWindow_->language()) // French
    {
        lang = QLocale::French;
    }

    ui->dateEdit->setLocale(lang);

    update();

    blockOnTranslate = false;

    // Login
    //this->ui->label_2->setText(tr("Username:"));
   // this->ui->label_3->setText(tr("Password:"));
   // this->ui->label_13->setText(tr("Language:"));
   // this->ui->loginBut->setText(tr("Login"));
   // this->ui->loginGroupBox->setTitle(tr("L O G I N"));


    // Sound Volume
   // this->ui->setVolumeBut->setText(tr("Set Volume"));

    // logout
    //  this->ui->logoutBut->setText(tr("Logout"));
   // this->ui->quitBut->setText(tr("X"));

    // Date/Time
  //  this->ui->setDateTimeBut->setText(tr("Set Date/Time"));

    // Status Labels on Top
    //this->ui->label->setText(tr("Operator ID:"));
    //this->ui->label_4->setText(tr("Timezone:"));

    // Admin Tab
    //this->ui->addUserResultsLabel->setText(tr("Add or Delete a User"));
    //this->ui->label_5->setText(tr("Username"));
    //this->ui->label_7->setText(tr("Password"));
    //this->ui->adminLabel->setText(tr("Administrator"));
    //this->ui->yesAdminLabel->setText(tr("YES"));
    //this->ui->noAdminLabel->setText(tr("NO"));

   // this->ui->deleteUserBut->setText(tr("Delete User"));
  //  this->ui->addUserBut->setText(tr("Add User"));
}


//////////////////////////////////////////////////////
//
//       L O G I N   L A N G U A G E  C O M B O
//
//             C H A N G E D
//
//////////////////////////////////////////////////////

/*void MainWindow::loginLanguageComboValueChange(int localIndex)
{
    switch(localIndex)
    {
    case 0: // English
        qApp->removeTranslator(translator1);
        qApp->removeTranslator(translator2);
        break;
    case 1: // Spanish
        translator2->load("qmsettings_sp", ".");
        qApp->installTranslator(translator2);
        break;
    case 2: // French
        translator1->load("qmsettings_fr", ".");
        qApp->installTranslator(translator1);
        break;
    }
    retranslateUI();
}
*/
//////////////////////////////////////////////////////
//
//       R E A D   S T D   O U T P U T
//
//////////////////////////////////////////////////////

void setupUtility::readStdOutput()
{
    //   QString chunk = readAllStandardOutput();
    //  this->ui->networkResultsLabel->setText(chunk);
}

bool setupUtility::mergeNewDBEntries(QString key, QString value)
{
    //processingAButton = true;

    QString databaseFileName = "settingswizard/scalesettings";

    if( TESTING )
        databaseFileName = "C:/hobart/settingswizard/scalesettings";

    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

    QFile file(databaseFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        processingAButton = false;

        return false;
    }
    QStringList theLine;
    QTextStream in(&file);
    bool needToAddIt = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        theLine = line.split("=", QString::KeepEmptyParts);
        if(theLine.at(0) == key) needToAddIt = false;
    }
    file.close();

    if(needToAddIt)
    {
        if(!file.open(QIODevice::Append | QIODevice::Text))
        {
            processingAButton = false;
            return false;
        }
        QTextStream out2(&file);
        out2 << key + "=" + value << endl;
    }
    file.close();
    //processingAButton = false;

    return needToAddIt;
}

QString setupUtility::getDBValue(QString key)
{
    // key must be one of:
    // "cBrightness"
    // "oBrightness"
    // "oVolume"
    // "cVolume"
    // "isAstronomical"  (this is 24 hour clock)
    // "connectionType"
    // "connectionName"
    // "gcd"
    //processingAButton = true;

    QString databaseFileName = "settingswizard/scalesettings";

    if( TESTING )
        databaseFileName = "C:/hobart/settingswizard/scalesettings";

    QString value = "";
    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

    QFile file(databaseFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //processingAButton = false;

        return "";
    }
    QStringList theLine;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        theLine = line.split("=", QString::KeepEmptyParts);
        if(theLine.at(0) == key) value = theLine.at(1);
    }
    file.close();
    //processingAButton = false;

    return value;
}

QStringList setupUtility::getStoredCerts(QString type)
{
    QStringList certsList;
    //if(isLinux())
    {
        //get Directory Listing
        QString certsDirName = "settingswizard/" + type;

        if( TESTING )
            certsDirName = "C:/hobart/settingswizard/" + type;

        QString certsLocation = filesystem::absoluteFilePath(certsDirName, folderType::DBs);

        QDir certsDir(certsLocation);
        certsDir.setFilter(QDir::Files);
        certsDir.setSorting(QDir::Name | QDir::IgnoreCase);
        certsList = certsDir.entryList();
    }
    return certsList;
}

void setupUtility::connectDB()
{
    LOG_CLASS_INFO( "setupUtility", "connectDB" );
    LOG_DEBUG( "connectDB START", NULL );

    //processingAButton = true;

   QString databaseFileName = "settingswizard/scalesettings";

   if( TESTING )
       databaseFileName = "C:/hobart/settingswizard/scalesettings";

   databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

   QFile file(databaseFileLocation);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       processingAButton = false;

       return;
    }
   QStringList theLine;
   QTextStream in(&file);
   while (!in.atEnd()) {
       QString line = in.readLine();
       theLine = line.split("=", QString::KeepEmptyParts);
       if(theLine.size() > 1)
       {
           if(theLine.at(0) == "cBrightness") customerBrightness = theLine.at(1);
           else if(theLine.at(0) == "oBrightness") operatorBrightness = theLine.at(1);
           else if(theLine.at(0) == "oVolume") operatorVolume = theLine.at(1);
           else if(theLine.at(0) == "cVolume") customerVolume = theLine.at(1);
           else if(theLine.at(0) == "isAstronomical") isAstronomical = theLine.at(1);
           else if(theLine.at(0) == "connectionType") dbConnectionType = theLine.at(1);
           else if(theLine.at(0) == "connectionName") dbConnectionName = theLine.at(1);
           else if(theLine.at(0) == "gcd") dbGCD = theLine.at(1);
           else if(theLine.at(0) == "timezone") dbTimezone = theLine.at(1);
           else if(theLine.at(0) == "isStatic") dbisStatic = theLine.at(1);
           else if(theLine.at(0) == "gateway") dbgateway = theLine.at(1);
           else if(theLine.at(0) == "netmask") dbnetmask = theLine.at(1);
           else if(theLine.at(0) == "ipAddress") dbipAddress = theLine.at(1);
           else if(theLine.at(0) == "autoDNS") dbautoDNS = theLine.at(1);
           else if(theLine.at(0) == "dns1") dbdns1 = theLine.at(1);
           else if(theLine.at(0) == "dns2") dbdns2 = theLine.at(1);
           else if(theLine.at(0) == "dns3") dbdns3 = theLine.at(1);
           else if(theLine.at(0) == "security") dbsecurity = theLine.at(1);
           else if(theLine.at(0) == "encryption") dbencryption = theLine.at(1);
           else if(theLine.at(0) == "firstTimeExport") dbfirstTimeExport = theLine.at(1);
           else if(theLine.at(0) == "displayTab") dbDisplayTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "customerSoundTab") dbCustomerSoundTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "operatorSoundTab") dbOperatorSoundTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "timeTab") dbTimeTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "networkTab") dbNetworkTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "hostTab") dbHostTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "printerTab") dbPrinterTab = theLine.at(1) == "1" ? true : false;
           else if(theLine.at(0) == "ntpServer") dbNTPServer = theLine.at(1);
           else if(theLine.at(0) == "ntpFrequency") dbNTPFrequency = theLine.at(1);
           else if(theLine.at(0) == "ntpDayOfWeek") dbNTPDayOfWeek = theLine.at(1);
           else if(theLine.at(0) == "ntpTimeOfDay") dbNTPTimeOfDay = theLine.at(1);
           else if(theLine.at(0) == "ntpTimeZone") dbNTPTimeZone = theLine.at(1);
           else if(theLine.at(0) == "ntpAntiMeridian") dbNTPAntiMeridian = theLine.at(1);
           else if(theLine.at(0) == "timeKeeper") dbTimeKeeper = theLine.at(1);
           else if(theLine.at(0) == "eapPassword") dbEapPassword = theLine.at(1);
           else if(theLine.at(0) == "eapUsername") dbEapUsername = theLine.at(1);
           else if(theLine.at(0) == "wpaPassphrase") dbWpaPassphrase = theLine.at(1);
           else if(theLine.at(0) == "speakerOutput") dbSpeakerOutput = theLine.at(1);
           else if(theLine.at(0) == "serverCertFilename") dbServerCertFilename = theLine.at(1);
           else if(theLine.at(0) == "chainFilename") dbChainFilename = theLine.at(1);
           else if(theLine.at(0) == "clientCertFilename") dbClientCertFilename = theLine.at(1);
           else if(theLine.at(0) == "csrFilename") dbCSRFilename = theLine.at(1);
           else if(theLine.at(0) == "csrKeyFilename") dbCSRKeyFilename = theLine.at(1);
           else if(theLine.at(0) == "clientKeyFilename") dbCSRKeyFilename = theLine.at(1);
           else if(theLine.at(0) == "clientCertUsersFilename") dbClientCertUsersFilename = theLine.at(1);
           else if(theLine.at(0) == "clientCertPassword") dbClientCertPassword = theLine.at(1);
           else if(theLine.at(0) == "linkRate") dbLinkRate = theLine.at(1);
           else if(theLine.at(0) == "automaticLinkRate") dbAutomaticLinkRate = theLine.at(1);
           else if(theLine.at(0) == "channel") dbChannel = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix1") dbDNSSuffix1 = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix2") dbDNSSuffix2 = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix3") dbDNSSuffix3 = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix4") dbDNSSuffix4 = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix5") dbDNSSuffix5 = theLine.at(1);
           else if(theLine.at(0) == "DNSSuffix6") dbDNSSuffix6 = theLine.at(1);
           else if(theLine.at(0) == "DHCPHostname")
           {
               LOG_DEBUG( "read DHCPHostname in file", NULL );
               dbDHCPHostname = theLine.at(1);
           }
           else if(theLine.at(0) == "DHCPHostname")
           {
               LOG_DEBUG( "read DHCPHostname in file", NULL );
               dbDHCPHostname = theLine.at(1);                 // why are there 2 here?
           }
           else if(theLine.at(0) == "snmpVariablesPort") dbSNMPVariablesPort = theLine.at(1);
           else if(theLine.at(0) == "snmpTrapsPort") dbSNMPTrapsPort = theLine.at(1);
           else if(theLine.at(0) == "snmpTrapsIP") dbSNMPTrapsIP = theLine.at(1);
           else if(theLine.at(0) == "snmpEnabled") dbSNMPEnabled = theLine.at(1);



       }
       else
       {
           if(theLine.at(0) == "cBrightness") customerBrightness = "100";
           else if(theLine.at(0) == "oBrightness") operatorBrightness = "100";
           else if(theLine.at(0) == "oVolume") operatorVolume = "on";
           else if(theLine.at(0) == "cVolume") customerVolume = "70";
           else if(theLine.at(0) == "isAstronomical") isAstronomical = "0";
           else if(theLine.at(0) == "connectionType") dbConnectionType = "0";
           else if(theLine.at(0) == "connectionName") dbConnectionName = "WiredConnection";
           else if(theLine.at(0) == "gcd") dbGCD = "1";
           else if(theLine.at(0) == "timezone") dbTimezone = "America/New_York";
           else if(theLine.at(0) == "isStatic") dbisStatic = "1";
           else if(theLine.at(0) == "gateway") dbgateway = "";
           else if(theLine.at(0) == "netmask") dbnetmask = "";
           else if(theLine.at(0) == "ipAddress") dbipAddress = "";
           else if(theLine.at(0) == "autoDNS") dbautoDNS = "0";
           else if(theLine.at(0) == "dns1") dbdns1 = "";
           else if(theLine.at(0) == "dns2") dbdns2 = "";
           else if(theLine.at(0) == "dns3") dbdns3 = "";
           else if(theLine.at(0) == "security") dbsecurity = "0";
           else if(theLine.at(0) == "encryption") dbencryption = "0";
           else if(theLine.at(0) == "firstTimeExport") dbfirstTimeExport = "0";
           else if(theLine.at(0) == "displayTab") dbDisplayTab = false;
           else if(theLine.at(0) == "customerSoundTab") dbCustomerSoundTab = false;
           else if(theLine.at(0) == "operatorSoundTab") dbOperatorSoundTab = false;
           else if(theLine.at(0) == "timeKeeper") dbTimeKeeper = "Manual";
           else if(theLine.at(0) == "timeTab") dbTimeTab = false;
           else if(theLine.at(0) == "networkTab") dbNetworkTab = false;
           else if(theLine.at(0) == "hostTab") dbHostTab = false;
           else if(theLine.at(0) == "printerTab") dbPrinterTab = false;
           else if(theLine.at(0) == "ntpServer") dbNTPServer = "0.0.0.0";
           else if(theLine.at(0) == "ntpFrequency") dbNTPFrequency = "0";
           else if(theLine.at(0) == "ntpDayOfWeek") dbNTPDayOfWeek = "0";
           else if(theLine.at(0) == "ntpTimeOfDay") dbNTPTimeOfDay = "03:30:00";
           else if(theLine.at(0) == "ntpTimeZone") dbNTPTimeZone = "America/New_York";
           else if(theLine.at(0) == "ntpAntiMeridian") dbNTPAntiMeridian = "AM";
           else if(theLine.at(0) == "eapPassword") dbEapPassword = "";
           else if(theLine.at(0) == "eapUsername") dbEapUsername = "";
           else if(theLine.at(0) == "wpaPassphrase") dbWpaPassphrase = "";
           else if(theLine.at(0) == "speakerOutput") dbSpeakerOutput = "0"; //Default to Internal
           else if(theLine.at(0) == "serverCertFilename") dbServerCertFilename = "";
           else if(theLine.at(0) == "chainFilename") dbChainFilename = "";
           else if(theLine.at(0) == "clientCertFilename") dbClientCertFilename = "";
           else if(theLine.at(0) == "csrFilename") dbCSRFilename = "";
           else if(theLine.at(0) == "csrKeyFilename") dbCSRKeyFilename = "";
           else if(theLine.at(0) == "clientKeyFilename") dbCSRKeyFilename = "";
           else if(theLine.at(0) == "clientCertUsersFilename") dbClientCertUsersFilename = "";
           else if(theLine.at(0) == "clientCertPassword") dbClientCertPassword = "";
           else if(theLine.at(0) == "linkRate") dbLinkRate = "auto";
           else if(theLine.at(0) == "automaticLinkRate") dbAutomaticLinkRate = "1";
           else if(theLine.at(0) == "channel") dbChannel = "auto";
           else if(theLine.at(0) == "DNSSuffix1") dbDNSSuffix1 = "";
           else if(theLine.at(0) == "DNSSuffix2") dbDNSSuffix2 = "";
           else if(theLine.at(0) == "DNSSuffix3") dbDNSSuffix3 = "";
           else if(theLine.at(0) == "DNSSuffix4") dbDNSSuffix4 = "";
           else if(theLine.at(0) == "DNSSuffix5") dbDNSSuffix5 = "";
           else if(theLine.at(0) == "DNSSuffix6") dbDNSSuffix6 = "";
           else if(theLine.at(0) == "DHCPHostname")
           {
               LOG_DEBUG( "DID NOT read DHCPHostname set it to 0", NULL );
               dbDHCPHostname = "0";
           }
           else if(theLine.at(0) == "snmpVariablesPort") dbSNMPVariablesPort = "161";
           else if(theLine.at(0) == "snmpTrapsPort") dbSNMPTrapsPort = "162";
           else if(theLine.at(0) == "snmpTrapsIP") dbSNMPTrapsIP = "";
           else if(theLine.at(0) == "snmpEnabled") dbSNMPEnabled = "0";
       }
   }
   file.close();

   //processingAButton = false;
}

void setupUtility::disconnectDB()
{
    //database.close();
}

void setupUtility::loadSoundVolumeValues(QString device)
{
    //Read volume values from db again.
    if(device == "o")
    {
        QString oVolume = getDBValue("oVolume");
        ui->testVolumeBut->hide();
        ui->volumeLabel->setText(oVolume);
        ui->lblPercentSymbol->hide();
        if(oVolume == "on")
        {
            ui->volumeSpinBox->setValue(1);
            ui->volumeLabel->setText(tr("on"));
            ui->muteBut->setChecked(true);
            operatorVolume = "on";
        }
        else
        {
            ui->volumeSpinBox->setValue(0);
            ui->volumeLabel->setText(tr("off"));
            ui->muteBut->setChecked(false);
            operatorVolume = "off";
        }
        ui->operatorSoundBut->setChecked(true);
    }
    else	// HTA-62 if (!session_->isPrepack())
    {
        QString cVolume = getDBValue("cVolume");
        volume = cVolume.toInt();
        ui->lblPercentSymbol->show();
        ui->testVolumeBut->show();
        ui->volumeLabel->setText(cVolume);
        ui->volumeSpinBox->setValue(cVolume.toInt());
        ui->customerSoundBut->setChecked(true);
        customerVolume = cVolume;
    }

    if(ui->volumeSpinBox->value() > 0)
    {
        this->ui->muteBut->setChecked(true);
        this->ui->muteBut->setStyleSheet("border-image:url(:/images/screens/volumeOnSmall.png) 0 0 0 0 stretch stretch;");
    }
    else
    {
        this->ui->muteBut->setChecked(false);
        this->ui->muteBut->setStyleSheet("border-image:url(:/images/screens/volumeOffSmall.png) 0 0 0 0 stretch stretch;");
    }
}

void setupUtility::loadDisplayBrightnessValues(QString display)
{
    // Get DB display brightness values
    if(display == "o")
    {

        this->ui->brightnessLabel->setText(operatorBrightness);
        this->ui->brightnessSpinBox->setValue(operatorBrightness.toInt());
        this->ui->operatorScreenBut->setChecked(true);
    }
    else if (!session_->isPrepack())
    {
        this->ui->brightnessLabel->setText(customerBrightness);
        this->ui->brightnessSpinBox->setValue(customerBrightness.toInt());
        this->ui->customerScreenBut->setChecked(true);
    }
}


/////////////////////////////////////////
//
//  L O A D   S A V E D   N E T W O R K
//
/////////////////////////////////////////
/*
void MainWindow::loadSavedNetwork(Network *locNetwork)
{
    // Get DB values

    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");
    if(!database->open())
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
    }
    else
    {
        // Get all from qmsettingsnetwork table and load
        //QSqlQuery query;

        // Update the single row in the table
        query.exec("select operatorid, machineid, isconnected, isstatic, isautodns, iswireless, isvpn, ipaddress, gateway, primarydns, secondarydns, tertiarydns, securitymethod, encryption, occurrence, ssid, subnetmask, wepkey1, wepkey2, wepkey3, wepkey4, vpnusername, vpnpassword, eapusername, eappassword,defaultkey,wpapassphrase from qmsettingsnetwork");
        while (query.next())
        {
           locNetwork->setOperatorID(query.value(0).toInt());
           locNetwork->setMachineID(query.value(1).toString());
           locNetwork->setIsConnected(query.value(2).toBool());
           locNetwork->setIsStatic(query.value(3).toBool());
           locNetwork->setIsAutoDNS(query.value(4).toBool());
           locNetwork->setIsWireless(query.value(5).toBool());
           locNetwork->setIsVPN(query.value(6).toBool());
           locNetwork->setIPAddress(query.value(7).toString());
           locNetwork->setGateway(query.value(8).toString());
           locNetwork->setPrimaryDNS(query.value(9).toString());
           locNetwork->setSecondaryDNS(query.value(10).toString());
           locNetwork->setTertiaryDNS(query.value(11).toString());
           locNetwork->setSecurityMethod(query.value(12).toString());
           locNetwork->setEncryption(query.value(13).toString());
           locNetwork->setOccurrence(query.value(14).toDateTime());
           locNetwork->setSSID(query.value(15).toString());
           locNetwork->setWEPKey1(query.value(16).toString());
           locNetwork->setWEPKey2(query.value(17).toString());
           locNetwork->setWEPKey3(query.value(18).toString());
           locNetwork->setWEPKey4(query.value(19).toString());
           locNetwork->setVPNUsername(query.value(20).toString());
           locNetwork->setVPNPassword(query.value(21).toString());
           locNetwork->setEAPUsername(query.value(22).toString());
           locNetwork->setEAPPassword(query.value(23).toString());
           locNetwork->setDefaultKey(query.value(24).toString());
           locNetwork->setWPAPassPhrase(query.value(25).toString());
        }
    }
    database->close();
    if(locNetwork->isVPN())
    {
        this->revealNetworkGroupBox(2, 1);
        this->ui->availableConnectionsTableView->selectRow(1);
    }
    else if(locNetwork->isWireless())
    {
        this->revealNetworkGroupBox(3, networkCount);
        bool networkFound = false;
        QTableWidgetItem* iterationItem;
        for(int i = networkCount; i < this->ui->availableConnectionsTableView->rowCount() && !networkFound; i++)
        {

            QString testing = (this->ui->availableConnectionsTableView->itemAt(i,1))->text();
            iterationItem = this->ui->availableConnectionsTableView->item(i, 1);

            if(iterationItem->text() == locNetwork->SSID())
            {
                this->ui->availableConnectionsTableView->selectRow(i);
                networkFound = true;
            }

        }
        if(!networkFound)
        {
            this->ui->alertsLineEdit->setText(tr("Previous Network Not Found"));
        }
    }
    else // wired
    {
        this->revealNetworkGroupBox(1, 0);
        this->ui->availableConnectionsTableView->selectRow(0);
    }
    //Do network DNS and IP settings here which are common to both wired and wireless connections.
    // These system calls involve dhclient or 'ip route add' execuables for the Qmx's Linux base OS


}
*/
///////////////////////////////////////////////////////////////////////////////////////
//
//     A P P L Y   N E T W O R K   C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
/*void MainWindow::applyNetworkBut_click()
{

    // Call Python script here,must be located in network folder on target OS
    // TBD define network folder

    QString theIP = "255.255.255.0";
    QStringList options;
    options.append("python/qmSettings/network/network.py");
    options.append("-c");
    options.append("true");

    QProcess applyNetworkConfiguration;

    applyNetworkConfiguration.start("python", options);
    applyNetworkConfiguration.waitForFinished(-1);

    this->ui->alertsLineEdit->setText(applyNetworkConfiguration.readAllStandardOutput());

    this->ui->networkResultsLabel->setText(this->ui->alertsLineEdit->text());

    // Shows the results of this button action and fades results
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    // make sure to create using new, since effect has to be alive as long as the target widget is using it.
    opacityEffect->setOpacity(1.0); // initially widget should be visible
    this->ui->networkResultsLabel->setGraphicsEffect(opacityEffect);
    QPropertyAnimation* anim = new QPropertyAnimation(this);
    anim->setTargetObject(opacityEffect);
    anim->setPropertyName("opacity");
    anim->setDuration(4500);
    anim->setStartValue(opacityEffect->opacity());
    anim->setEndValue(0);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
*/
////////////////////////////////
//
//  I S   W I R E D   C O N N E C T E D
//
/////////////////////////////////
int setupUtility::wiredSpeed()
{
    //processingAButton = true;

    int speed = 100;
    python pythonProcess;
    QStringList args;
    if (pythonProcess.execCommand("getWiredSpeed", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            speed = pythonProcess.readStdOut().toInt();
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
     }
     else
     {
        // TODO:
        // What do we do here?
     }

    // result = true;
    //processingAButton = false;
    return speed;
}

void setupUtility::refreshNetwork_click()
{
    QList<QTableWidgetItem *> holder = ui->availableConnectionsTableView->selectedItems();
    QString networkNameLoc = "";
    if(!holder.empty())
    {
    QTableWidgetItem *wirelessItem;
    wirelessItem = this->ui->availableConnectionsTableView->item(ui->availableConnectionsTableView->currentRow(),1);
        networkNameLoc = wirelessItem->text();
    }

    this->loadNetworkConnections();

    bool foundIt = false;
    for(int i =0; i < this->ui->availableConnectionsTableView->rowCount() && !foundIt; i++)
    {

        QTableWidgetItem *netItem;
        netItem = this->ui->availableConnectionsTableView->item(i,1);
        QString thisNetName = netItem->text();
       // qDebug() << "thisNetName is: " << thisNetName + " = " + networkNameLoc;
        //qDebug() << "loc is        : " << networkNameLoc;
        if(thisNetName == networkNameLoc )
        {
            this->ui->availableConnectionsTableView->selectRow(i);
            foundIt = true;
        }
    }
    holder = ui->availableConnectionsTableView->selectedItems();
    if(!holder.empty())
    {
        networkTableWidgetSelectionChanged(this->ui->availableConnectionsTableView->currentRow());
    }
    //ui->networkStatusLabel->hide();
    ui->networkStatusLabel->setText("");
    update();
    processingAButton = false;
}

void setupUtility::refreshNetworksPressed()
{
    ui->networkStatusLabel->show();
    ui->networkStatusLabel->setText(tr("Scanning networks...wait"));
    update();
}

void setupUtility::refreshNetworksButton_click()
{
    processingAButton = true;
    //ui->networkStatusLabel->show();
    //ui->networkStatusLabel->setText(tr("Scanning networks...wait"));
    //update();
    refreshNetwork_click();
}

////////////////////////////////
//
//  L O A D    N E T W O R K    C O N N E C T I O N S
//
/////////////////////////////////
void setupUtility::loadNetworkConnections()
{
    LOG_CLASS_INFO( "setupUtility", "loadNetworkConnections" );
    LOG_DEBUG( "loadNetworkConnections() START", NULL );

    //processingAButton = true;
    isReadyToTranslate = false;

    foundSelectedConnection = false;
    if(false /*this->useVPNConnection() if ever implemented in future*/)
    {
        this->networkCount = 2;
    }
    else
    {
        this->networkCount = 1;
    }


    this->ui->availableConnectionsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->availableConnectionsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    this->ui->availableConnectionsTableView->clearContents();
    //this->ui->availableConnectionsTableView->setStyleSheet("QTableWidget{background-color: white;border: 1px solid gray;} QTableWidget::item{ background-color: white;border: 0px solid white;} QTableWidget::item:selected{background: rgb(255, 255, 255); color: rgb(91, 153, 25); border-top: 6px solid rgb(135, 202, 0); border-bottom: 6px solid rgb(135, 202, 0);} QTableWidget::item:focus {border: 0px; }");
    this->ui->availableConnectionsTableView->setStyleSheet("QTableWidget{background-color: white;border: 1px solid gray;} QTableWidget::item{ background-color: white;border: 0px solid white;} QTableWidget::item:selected{background: rgb(255, 255, 255); color:rgb(0,0,0); border-top: 6px solid rgb(135, 202, 0); border-bottom: 6px solid rgb(135, 202, 0);} QTableWidget::item:focus {border: 0px; }");

    QStringList APs = this->getWirelessAPs();

    // first load the wired connection
    QString lightFileName = ":/images/screens/disconnectedLightFinal.png";
    QString adapterIconFileName = ":/images/screens/wiredIconFinal.png";
    QString strengthIconFileName = ":/images/screens/strength5.png";

    this->ui->availableConnectionsTableView->setRowCount(APs.count()/4 + networkCount); // account for wired and vpn

    QTableWidgetItem *connectionNameNStatusItem = new QTableWidgetItem;
    QString nameNstatus = tr("Wired Ethernet");
    QString strengthRatio = "";
    QString signalNoise = "";
    int signalNoiseValue;

    int signalStrength = this->wiredSpeed();
    // Show connected icon and wording

    // for wired I changed the connection type below to 0, it was 1
    if(dbConnectionType == "0")
    {
        finalIsWireless = false;
        this->adapterAlreadyConnected = true;
        lightFileName = ":/images/screens/connectedLightFinal.png";
        selectedConnectionRow = 0;
        foundSelectedConnection = true;
        // li
    }
    else
    {
        finalIsWireless = true;
        this->adapterAlreadyConnected = false;
    }

    connectionNameNStatusItem->setText(nameNstatus);

    QPixmap lightPic(lightFileName);
    QIcon lightIcon;

    lightIcon.actualSize(QSize(35,69));
    lightIcon.addPixmap(lightPic, QIcon::Normal);
    lightIcon.addPixmap(lightPic, QIcon::Selected);

    QPixmap adapterIconPic(adapterIconFileName);
    QIcon adapterIcon;

    adapterIcon.addPixmap(adapterIconPic, QIcon::Normal);
    adapterIcon.addPixmap(adapterIconPic, QIcon::Selected);

    QTableWidgetItem *connectionIconItem = new QTableWidgetItem;
    connectionIconItem->setSizeHint(QSize(35,69));
    connectionIconItem->setIcon(lightIcon);

    QTableWidgetItem *adapterIconItem = new QTableWidgetItem;
    adapterIconItem->setIcon(adapterIcon);

    // Ethernet Speed
    // TBD get live signal strength percentage here
    QTableWidgetItem *strengthIconItem = new QTableWidgetItem;

    strengthIconFileName = ":/images/screens/" + QString::number(signalStrength) +  "Mbps.png";

    QPixmap strengthIconPic(strengthIconFileName);
    QIcon strengthIcon;
    strengthIcon.addPixmap(strengthIconPic,QIcon::Normal);
    strengthIcon.addPixmap(strengthIconPic,QIcon::Selected);
    strengthIconItem->setIcon(strengthIcon);
    strengthIconItem->setTextAlignment(Qt::AlignLeft);

    // wired section only
    // Noted VPN connections are gone now

    QTableWidgetItem *wiredWirelessGroupBoxTypeItem = new QTableWidgetItem;
    wiredWirelessGroupBoxTypeItem->setText("0");

    // Set column widths to allowable table width of network connections. Theming will govern this real estate
    this->ui->availableConnectionsTableView->setItem( 0, 0, connectionIconItem);
    this->ui->availableConnectionsTableView->setItem( 0, 1, connectionNameNStatusItem);
    this->ui->availableConnectionsTableView->setItem( 0, 2, adapterIconItem);
    this->ui->availableConnectionsTableView->setItem( 0, 3, strengthIconItem);
    this->ui->availableConnectionsTableView->setItem( 0, 4, wiredWirelessGroupBoxTypeItem);
    this->ui->availableConnectionsTableView->setRowHeight( 0, scaleHeightForScreen( 69 ) );
    this->ui->availableConnectionsTableView->setColumnWidth( 0, scaleWidthForScreen( 35 ) );
    this->ui->availableConnectionsTableView->setColumnWidth( 1, scaleWidthForScreen( 136 ) );
    this->ui->availableConnectionsTableView->setColumnWidth( 2, scaleWidthForScreen( 38 ) );
    this->ui->availableConnectionsTableView->setColumnWidth( 3, scaleWidthForScreen( 53 ) );
    this->ui->availableConnectionsTableView->setColumnWidth( 4, 0);

    //The wireless is set to go by now

//////////////////////////////////////////
    // TBD add VPN connection

    /*if(false  this->useVPNConnection())
    {
        QTableWidgetItem *vpnAdapterIconItem = new QTableWidgetItem;
        QTableWidgetItem *vpnConnectionNameNStatusItem = new QTableWidgetItem;
        QTableWidgetItem *vpnLightIconItem = new QTableWidgetItem;
        QString vpnAdapterIconFileName = ":/images/screens/VPNIconFinal.png";

        QPixmap vpnAdapterIconPic(vpnAdapterIconFileName);

        QIcon vpnAdapterIcon;

        vpnAdapterIcon.actualSize(QSize(35,69));
        vpnAdapterIcon.addPixmap(vpnAdapterIconPic,QIcon::Normal);
        vpnAdapterIcon.addPixmap(vpnAdapterIconPic,QIcon::Selected);

        vpnAdapterIconItem->setIcon(vpnAdapterIcon);

        // TBD get connected or not;
        if(this->adapterAlreadyConnected)
        {
            lightFileName = ":/images/screens/disconnectedLight.png";
        }
        else
        {
            lightFileName = ":/images/screens/connectedLight.png";
            this->adapterAlreadyConnected = true;
        }

        QPixmap lightIconPic(lightFileName);
        QIcon lightIcon;

        lightIcon.actualSize(QSize(35,69));
        lightIcon.addPixmap(lightIconPic,QIcon::Normal);
        lightIcon.addPixmap(lightIconPic,QIcon::Selected);

        vpnLightIconItem->setIcon(lightIcon);
        nameNstatus = "VPN";

        vpnConnectionNameNStatusItem->setText(nameNstatus);

        // Ethernet Speed
        // TBD get live signal strength percentage here
        QTableWidgetItem *vpnStrengthIconItem = new QTableWidgetItem;

        //TBD get VPN speed
        signalStrength = 100;
        strengthIconFileName = ":/images/screens/" + QString::number(signalStrength) +  "Mbps.png";

        QPixmap vpnStrengthIconPic(strengthIconFileName);
        QIcon strengthIcon;
        strengthIcon.addPixmap(vpnStrengthIconPic,QIcon::Normal);
        strengthIcon.addPixmap(vpnStrengthIconPic,QIcon::Selected);

        vpnStrengthIconItem->setIcon(strengthIcon);
        vpnStrengthIconItem->setTextAlignment(Qt::AlignLeft);
        QTableWidgetItem *vpnGroupBoxTypeItem = new QTableWidgetItem;
        vpnGroupBoxTypeItem->setText("1");

        this->ui->availableConnectionsTableView->setRowHeight(1, 69);
        this->ui->availableConnectionsTableView->setItem(1,0, vpnLightIconItem); //TBD
        this->ui->availableConnectionsTableView->setItem(1,1, vpnConnectionNameNStatusItem);
        this->ui->availableConnectionsTableView->setItem(1,2, vpnAdapterIconItem);
        this->ui->availableConnectionsTableView->setItem(1,3, vpnStrengthIconItem);
        this->ui->availableConnectionsTableView->setItem(1,4, vpnGroupBoxTypeItem);
    }
    */

    adapterIconFileName = ":/images/screens/wirelessIconFinal.png";

    QFont fontStrengthRatioNoDBM;
    fontStrengthRatioNoDBM.setPointSize(11);
    fontStrengthRatioNoDBM.setFamily("Sans Serif");
    QFont fontStrengthRatio;
    fontStrengthRatio.setPointSize(9);
    fontStrengthRatio.setFamily("Sans Serif");
    QBrush goodSignalStrengthColor(Qt::black);
    QBrush badSignalStrengthColor(Qt::darkRed);
    bool foundIt = false;

    //Loop through all the wireless Access Points detected and add to rows of tablewidget
    for(int i = 0; i < APs.count()/4; i++)
    {

        QTableWidgetItem *adapterIconItem = new QTableWidgetItem;
        QTableWidgetItem *connectionNameNStatusItem = new QTableWidgetItem;
        QTableWidgetItem *lightIconItem = new QTableWidgetItem;
        QTableWidgetItem *strengthIconItem = new QTableWidgetItem;

        nameNstatus = APs.at(i*4 + 3);
        strengthRatio = APs.at(i*4 + 1) + "%";

        if(isLinux())
        {
            signalNoise = APs.at(i*4 + 2) + "dBm";
            signalNoiseValue = APs.at(i*4 + 2).toInt();
        }
        else
        {
            signalNoise = "";
            signalNoiseValue = 0;
        }

        if (signalNoiseValue < -69 || (!isLinux() && APs.at(i*4 + 1).toInt() < 20))
        {
            strengthIconItem->setForeground(badSignalStrengthColor);
            strengthIconItem->setTextColor(QColor(144, 0, 0));
        }
        else
        {
            strengthIconItem->setForeground(goodSignalStrengthColor);
            strengthIconItem->setForeground(QColor(0,0,0));
        }
        //signalNoise = APs.at(i*4 + 2) + "dBm";
        //signalNoiseValue = APs.at(i*4 + 2).toInt();


        //if (signalNoiseValue < -69)
        //{
        //    strengthIconItem->setForeground(badSignalStrengthColor);
        //    strengthIconItem->setTextColor(QColor(144, 0, 0));
        //}
        //else
        //{
        //    strengthIconItem->setForeground(goodSignalStrengthColor);
        //    strengthIconItem->setForeground(QColor(0,0,0));
        //}

        strengthIconFileName = ":/images/screens/wirelessIcon" + APs.at(i*4) + ".png";
        QPixmap adapterIconPic(strengthIconFileName);

        QIcon adapterIcon;
        adapterIcon.addPixmap(adapterIconPic,QIcon::Normal);
        adapterIcon.addPixmap(adapterIconPic,QIcon::Selected);

        adapterIconItem->setIcon(adapterIcon);

        if(dbConnectionName == nameNstatus && !foundSelectedConnection)
        {
            foundIt = true;
            lightFileName = ":/images/screens/connectedLightFinal.png";
            this->adapterAlreadyConnected = true;
            selectedConnectionRow = i+1;
            ui->SSIDLineEdit->setText(dbConnectionName);
            foundSelectedConnection = true;
        }
        else
        {
            lightFileName = ":/images/screens/disconnectedLightFinal.png";
        }

        QPixmap lightIconPic(lightFileName);
        QIcon lightIcon;


        lightIcon.actualSize(QSize(35,69));
        lightIcon.addPixmap(lightIconPic,QIcon::Normal);
        lightIcon.addPixmap(lightIconPic,QIcon::Selected);

        lightIconItem->setIcon(lightIcon);
        connectionNameNStatusItem->setText(nameNstatus);

        QPixmap strengthIconPic(strengthIconFileName);
        QIcon strengthIcon;
        strengthIcon.addPixmap(strengthIconPic,QIcon::Normal);
        strengthIcon.addPixmap(strengthIconPic,QIcon::Selected);

        if(isLinux())
        {
            strengthIconItem->setFont(fontStrengthRatio);
            strengthIconItem->setText(strengthRatio + "\n" + signalNoise);
        }
        else
        {
            strengthIconItem->setFont(fontStrengthRatio);
            strengthIconItem->setText(strengthRatio);
        }

        //strengthIconItem->setFont(fontStrengthRatio);
        //strengthIconItem->setText(strengthRatio + "\n" + signalNoise);
        strengthIconItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *wirelessGroupBoxTypeItem = new QTableWidgetItem;
        wirelessGroupBoxTypeItem->setText("2");

        this->ui->availableConnectionsTableView->setRowHeight( i+this->networkCount, scaleHeightForScreen( 69 ) );
        this->ui->availableConnectionsTableView->setItem( i+this->networkCount, 0, lightIconItem ); //TBD
        this->ui->availableConnectionsTableView->setItem( i+this->networkCount, 1, connectionNameNStatusItem );
        this->ui->availableConnectionsTableView->setItem( i+this->networkCount, 2, adapterIconItem );
        this->ui->availableConnectionsTableView->setItem( i+this->networkCount, 3, strengthIconItem );
        this->ui->availableConnectionsTableView->setItem( i+this->networkCount, 4, wirelessGroupBoxTypeItem );

        if(foundIt)
        {
            ui->availableConnectionsTableView->scrollToItem(lightIconItem, QAbstractItemView::PositionAtTop);
            foundIt = false;
        }
    }

    if(isDemoMode)
        this->ui->availableConnectionsTableView->selectRow(1);
    else
    {
        // not demo version
        if(foundSelectedConnection)
        {
                this->ui->availableConnectionsTableView->selectRow(selectedConnectionRow);
                //selectedRow is wired

            	if(selectedConnectionRow == 0)
            	{
                	networkTableWidgetSelectionChanged(0);
            	}
        }
        else
        {
            //did not find the users selected connection so clear all selections
            this->ui->availableConnectionsTableView->clearSelection();
        }
    }
    if(dbConnectionType == "1")
    {
        //wireless connection
        finalIsWireless = true;
        ui->SSIDLineEdit->setText(dbConnectionName);
    }
    else
    {
        //wired connection
        finalIsWireless = false;
    }
    isReadyToTranslate = true;

    //LOG_DEBUG( "loadNetworkConnections() END", NULL );
}

void setupUtility::powerDevices(QString states)
{
    // 10 = wire on  / wifi off
    // 01 = wire off / wifi on
    // 11 = wire on / wifi on
    // 00 = wire off / wifi off
    //processingAButton = true;
    QStringList options;
    options.append("-C");
    options.append(states);

    if(!isDemoMode)
    {
        python pythonProcess;

        pythonProcess.execCommand("setNetwork", options);
        if (pythonProcess.getExitCode() == 0)
        {
            ; //result = pythonProcess.readStdOut();
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    //processingAButton = false;
}

////////////////////////////////
//
//  G E T  W I R E L E S S   A P s
//
/////////////////////////////////
QStringList setupUtility::getWirelessAPs()
{
    //this->ui->alertsLineEdit->setText(tr("Scanning wireless access points..."));

    //processingAButton = true;
    QProcess workConfiguration;
    QStringList APs;
    QString result;
    QStringList options;
    //options.append("python/qmSettings/network/qmSettingsNetwork.py");
      //ui->alertsLineEdit->setText(alertsText);

    //options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));
    options.append("-X");
    options.append("1");

    if(!isDemoMode)
    {
        python pythonProcess;
        if (pythonProcess.execCommand("setNetwork", options))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                result = pythonProcess.readStdOut();
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }
    }
    else //marketing Tablet
    {
        result = "4|foodRouter1|3|WAP-deli|1|Bakery-Router|5|Hobart|5|Manager-AP|3|ProduceRouter|2|Marketing-AP|2|";
    }

    ////qDebug() << "result APs is : " << result;
    APs = result.split("|", QString::SkipEmptyParts);

//    in case wireless device reports an SSID with a blank name, Hobart does not want to list it
    //for (int i = 0; i < APs.count(); i++)
    //{
    //    if(APs.at(i).trimmed().isEmpty() || APs.at(i).trimmed().isNull())
    //    {
    //        APs.removeAt(i);
    //    }
    //}

    // the save was successful
    //processingAButton = false;
    return APs;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     U S E   W I R E L E S S   C O N N E C T I O N
//
///////////////////////////////////////////////////////////////////////////////////////
/*
bool MainWindow::useWirelessConnection()
{
    bool result = true; // failsafe
    int dbSetting = 0;

    // read setting from DB
    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");
    if(!database->open()) // Can't open the DB
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
    }
    else
    {
        QSqlQuery query;

        query.exec("select iswireless from qmsettingsnetwork");
        while (query.next())
        {
            dbSetting = query.value(0).toInt();
        }
    }
    database->close();

    // Set the widgets Look and Feel
    if(dbSetting == 0)
    {
        result = false;
    }

    // return if Wireless connection is to be used
    return result;
}
*/


void setupUtility::printEAPOutput()
{
    LOG_CLASS_INFO("setupUtility","printEAPOutput");
    ////qDebug() << networkProcessConnected;
    ////qDebug() << "made it in network print process";
    //int newRow = -1;
    //bool changeLights = false;
    LOG_DEBUG("EAP CONNECTION OUTPUT ", NULL);

    //bool doLookAgain = false;
    if(eapProcessConnected)
    {

        QString allStandardOutput = this->saveEAPConfigurationProcess->readAllStandardOutput();

                  QStringList outputLines = allStandardOutput.split('\n');

        for (QStringList::Iterator it = outputLines.begin(); it != outputLines.end(); ++ it)
        {

            QString output = it->trimmed();
            LOG_DEBUG("OUTPUT is: %s", output.toLatin1().constData());

            if(output == "CONNECTED")
            {
                this->ui->alertsLineEdit->setText(tr("CONNECTED"));
                this->ui->connectingProgressBar->setValue(0);
            }
            else
            {
                ;//this->ui->alertsLineEdit->setText(output);
                //this->ui->connectingProgressBar->setValue(50);
            }
        }
    }
}

//////////////////////////////////////////////////////
//
//       P R I N T   N E T W O R K   C O N N E C T I N G   O U T P U T
//
//////////////////////////////////////////////////////

void setupUtility::printNetworkOutput()
{
    LOG_CLASS_INFO("setupUtility","printNetworkOutput");
    LOG_DEBUG("printNetworkOutput() START", NULL);

    int newRow = -1;
    bool changeLights = false;

    bool doLookAgain = false;
    if(networkProcessConnected)
    {
        //LOG_DEBUG("printNetworkOutput() IF networkProcessConnected is TRUE", NULL);

        QString allStandardOutput = this->saveNetworkConfigurationProcess->readAllStandardOutput();

        QStringList outputLines = allStandardOutput.split('\n');

        for (QStringList::Iterator it = outputLines.begin(); it != outputLines.end(); ++ it)
        {
            QString output = it->trimmed();

            if(connectionTimeout)
            {
                output = noConnectionPhrase;
            }

            if(output == "1")
            {
                this->ui->alertsLineEdit->setText(tr("Removing DHCP leases..."));
                this->ui->connectingProgressBar->setValue(5);
            }
            else if(output == "2")
            {
                this->ui->alertsLineEdit->setText(tr("Restarting DHCP executable..."));
                this->ui->connectingProgressBar->setValue(10);
            }
            else if(output == "3")
            {
                if(ui->availableConnectionsTableView->currentRow() == 0)
                {
                    this->ui->alertsLineEdit->setText(tr("Restarting the wired device..."));
                }
                else
                {
                    this->ui->alertsLineEdit->setText(tr("Restarting the wireless device..."));
                }
                this->ui->connectingProgressBar->setValue(20);
            }
            else if(output == "4")
            {
                this->ui->alertsLineEdit->setText(tr("Configuring authentication..."));
                this->ui->connectingProgressBar->setValue(30);
            }
            else if(output == "5")
            {

                if(newConnectionFlag == "Wire")
                {
                    ;
                    //HW-84 confusing alerts
                    //this->ui->alertsLineEdit->setText(tr("Enabling the wired device..."));
                }
                else if (newConnectionFlag == "Wireless")
                {
                    ;
                    //HW-84 confusing alerts
                    //this->ui->alertsLineEdit->setText(tr("Enabling the wireless device..."));
                }
                else
                {
                    if(ui->availableConnectionsTableView->currentRow() == 0)
                    {
                        ;
                        //HW-84 confusing alerts
                        //this->ui->alertsLineEdit->setText(tr("Enabling the wired device..."));
                    }
                    else
                    {
                        ;
                        //HW-84 confusing alerts
                        //this->ui->alertsLineEdit->setText(tr("Enabling the wireless device..."));
                    }
                }
                newConnectionFlag = "";
                this->ui->connectingProgressBar->setValue(40);
            }
            else if(output == "6")
            {
                this->ui->alertsLineEdit->setText(tr("Requesting DHCP lease..."));
                this->ui->connectingProgressBar->setValue(50);
            }
            else if(output == "7")
            {
                this->ui->alertsLineEdit->setText(tr("Verifying connection..."));
                this->ui->connectingProgressBar->setValue(60);
            }
            else if(output == "8")
            {
                this->ui->alertsLineEdit->setText(tr("Setting IP Address and Subnet Mask..."));

                this->ui->connectingProgressBar->setValue(70);
            }
            else if(output == "9")
            {
                this->ui->alertsLineEdit->setText(tr("Setting Default Gateway..."));
                this->ui->connectingProgressBar->setValue(80);
            }
            else if(output == "10")
            {
                this->ui->alertsLineEdit->setText(tr("Configuring DNS servers..."));
                this->ui->connectingProgressBar->setValue(85);
            }
            else if(output == "55")
            {
                this->ui->alertsLineEdit->setText(tr("Generating Wireless Profile..."));
                //this->ui->connectingProgressBar->setValue(85);
            }
            else if(output == "11")
            {
                this->ui->alertsLineEdit->setText(tr("Testing connection..."));
                this->ui->connectingProgressBar->setValue(90);
            }
            else if(output == "12")
            {
                if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
                {
                    this->ui->alertsLineEdit->setText(tr("Processing certificates..."));
                    this->ui->connectingProgressBar->setValue(10);
                }
                else
                {
                    this->ui->alertsLineEdit->setText(tr("Generating encrypted passphrase..."));
                    this->ui->connectingProgressBar->setValue(33);
                }
            }
            else if(output == "13")
            {
                if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
                {
                    this->ui->alertsLineEdit->setText(tr("Processing certificates..."));
                    this->ui->connectingProgressBar->setValue(40);
                }
                else
                {
                    this->ui->alertsLineEdit->setText(tr("Associating with access point..."));
                    this->ui->connectingProgressBar->setValue(36);
                }
            }
            else if(output == "50")
            {
                this->ui->alertsLineEdit->setText(tr("Processing certificates..."));
                this->ui->connectingProgressBar->setValue(45);
            }
            else if(output == "YES")
            {
                connectionEstablished = true;
                connectedSuccessfully = true;
                changeLights = true;

                if(ui->wirelessAdapterGroupBox->isVisible())
                {
                    dbConnectionType = "1";
                    dbConnectionName = ui->SSIDLineEdit->text();
                    finalIsWireless = true;
                    isWiredConnection_Fix = false;
                }
                else
                {
                    dbConnectionType = "0";
                    dbConnectionName = "WiredEthernet";
                    finalIsWireless = false;
                    isWiredConnection_Fix = true;
                    newRow = 0;
                }

                if(this->ui->availableConnectionsTableView->currentRow() > 0 || ui->wirelessAdapterGroupBox->isVisible())
                {
                    dbConnectionType = "1";
                    dbConnectionName = ui->SSIDLineEdit->text();
                    finalIsWireless = true;
                    isWiredConnection_Fix = false;

                    // it must be wireless
                    this->updateDB("connectionType", "1");
                    finalIsWireless = true;
                    this->updateDB("connectionName", this->ui->SSIDLineEdit->text());
                    bool found = false;
                    QTableWidgetItem *wirelessItem;

                    //this->ui->SSIDLineEdit->setText(wirelessItem->text());
                    for(int i = 0;i < this->ui->availableConnectionsTableView->rowCount() && !found; i++)
                    {
                        wirelessItem = this->ui->availableConnectionsTableView->item(i,1);
                        ////qDebug() << "connection name is: " << wirelessItem->text();
                        if(wirelessItem->text() == this->ui->SSIDLineEdit->text())
                        {
                            this->ui->availableConnectionsTableView->selectRow(i);
                            newRow = i;
                            found = true;
                        }
                    }
                }
                else
                {
                    if(ui->wirelessAdapterGroupBox->isVisible())
                    {
                        newRow = -1;
                        isWiredConnection_Fix = false;
                    }
                    else
                    {
                        newRow = 0;
                        isWiredConnection_Fix = true;
                    }

                    if(newRow == 0)
                    {
                        // wired
                        this->updateDB("connectionType", "0");
                        this->updateDB("connectionName", "WiredEthernet");
                        this->dbConnectionName = "WiredEthernet";
                        dbConnectionType = "0";
                        finalIsWireless = false;
                        isWiredConnection_Fix = true;
                    }
                    else if(newRow < 0 && ui->wiredAdapterGroupBox->isVisible())
                    {
                        //it's hidden
                        this->updateDB("connectionType", "1");
                        this->updateDB("connectionName", ui->SSIDLineEdit->text());
                        dbConnectionName = ui->SSIDLineEdit->text();
                        dbConnectionType = "1";
                        finalIsWireless = true;
                        isWiredConnection_Fix = false;
                    }
                }

                this->ui->connectingProgressBar->hide();
                this->ui->connectingProgressBar->lower();
                this->ui->availableConnectionsTableView->setEnabled(true);
                this->ui->addNetworkBut->setEnabled(true);
                //this->ui->exportBut->setEnabled(true);        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
                this->ui->importBut->setEnabled(true);
                this->ui->networkSaveBut->setEnabled(true);
                this->ui->refreshNetworksBut->setEnabled(true);
                this->ui->serverCertBut->setEnabled(true);
                this->ui->clientCertBut->setEnabled(true);
                this->ui->searchTrustedRootCertBut->setEnabled(true);
                this->ui->searchClientCertBut->setEnabled(true);
                this->ui->serverCertDeleteBut->setEnabled(true);
                this->ui->clientCertDeleteBut->setEnabled(true);
                this->ui->trustedRootStoreCombo->setEnabled(true);
                this->ui->clientCertStoreCombo->setEnabled(true);
                this->ui->networkAdvancedBut->setEnabled(true);

                saveConfigsNow();
            }
            else if(output == "NO")
            {
                //LOG_DEBUG( "output after is NO", NULL );
                QProcess retestNetworkProcess;
                QStringList options;
                options.append("-D");

                if (ui->wirelessAdapterGroupBox->isVisible())
                {
                    options.append("1");
                }
                else
                {
                    options.append("0");
                }

                options.append(filesystem::absoluteFilePath("getNetworkStatus.py",folderType::Python));
                retestNetworkProcess.start("python", options);

                if(!retestNetworkProcess.waitForStarted())
                {
                    this->ui->alertsLineEdit->setText(tr("Start configuration error."));
                }

                if(!retestNetworkProcess.waitForFinished())
                {
                    ;
                }

                QStringList networkStatuses;
                QByteArray result = retestNetworkProcess.readAll();
                QString status(result);
                status = status.mid(0,status.length()-1);

                //qDebug() << "total string is:" << macAddress;
                // added in a string length check
                if(status.size() > 0)
                {
                    networkStatuses = status.split("|", QString::SkipEmptyParts);
                    if(networkStatuses[1] == "None")
                    {
                        //no connection
                        //noConnectionPhrase = "NO";
                        //printNetworkOutput();
                        noConnectionPhrase = "OUTPUT|None";
                        connectedSuccessfully = false;
                    }
                    else
                    {
                        // got conenction
                        networkProcessConnected = true;
                        connectionTimeout = true;
                        noConnectionPhrase = status;
                        connectedSuccessfully = true;
                        LOG_DEBUG("YESSO CONNECTION and output is: %s", status.toLatin1().constData());
                    }
                }

                if (noConnectionPhrase == "OUTPUT|None")
                {
                    connectionEstablished = false;
                    connectedSuccessfully = false;
                    changeLights = true;
                    //LOG_DEBUG("printNetworkOutput() Not connected... 1 noConnectionPhrasae == OUTPUT|None", NULL);
                    this->ui->alertsLineEdit->setText(tr("Not connected...Try again."));

                    // this->ui->exportBut->setEnabled(true);       // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
                    this->ui->importBut->setEnabled(true);
                    this->ui->networkSaveBut->setEnabled(true);
                    this->ui->refreshNetworksBut->setEnabled(true);
                    this->ui->addNetworkBut->setEnabled(true);
                    this->ui->serverCertBut->setEnabled(true);
                    this->ui->clientCertBut->setEnabled(true);
                    this->ui->searchTrustedRootCertBut->setEnabled(true);
                    this->ui->searchClientCertBut->setEnabled(true);
                    this->ui->serverCertDeleteBut->setEnabled(true);
                    this->ui->clientCertDeleteBut->setEnabled(true);
                    this->ui->trustedRootStoreCombo->setEnabled(true);
                    this->ui->clientCertStoreCombo->setEnabled(true);
                    this->ui->availableConnectionsTableView->setEnabled(true);
                    this->ui->networkAdvancedBut->setEnabled(true);
                    this->ui->connectingProgressBar->hide();
                    this->ui->connectingProgressBar->lower();

                    //////////////////////

                    if(ui->gatewayLineEdit->text() == "" && ui->gatewayLineEdit_2->text() == "" && ui->gatewayLineEdit_3->text() == "" && ui->gatewayLineEdit_4->text() == "")
                    {
                        this->updateDB("gateway", "64641964");
                    }
                    else
                    {
                        this->updateDB("gateway",ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text()  + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
                    }

                    this->updateDB("netmask", ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
                    this->updateDB("ipAddress", ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
                    this->updateDB("dns1", ui->DNS1LineEdit->text() + "." + ui->DNS1LineEdit_2->text() + "." + ui->DNS1LineEdit_3->text() + "." + ui->DNS1LineEdit_4->text());
                    this->updateDB("dns2", ui->DNS2LineEdit->text() + "." + ui->DNS2LineEdit_2->text() + "." + ui->DNS2LineEdit_3->text() + "." + ui->DNS2LineEdit_4->text());
                    this->updateDB("dns3", ui->DNS3LineEdit->text() + "." + ui->DNS3LineEdit_2->text() + "." + ui->DNS3LineEdit_3->text() + "." + ui->DNS3LineEdit_4->text());
                    this->updateDB("security", QString::number(ui->securityCombo->currentIndex()));
                    this->updateDB("encryption", QString::number(ui->encryptionCombo->currentIndex()));

                    if(this->ui->wirelessAdapterGroupBox->isVisible())
                    {
                        this->updateDB("connectionType", "1");
                        this->updateDB("connectionName", ui->SSIDLineEdit->text());
                    }
                    else
                    {
                        this->updateDB("connectionType", "0");
                        this->updateDB("connectionName", "WiredEthernet");
                    }

                    if(ui->dhcpRadio->isChecked())
                    {
                        //LOG_DEBUG("printNetworkOutput() updateDB(isStatic,0)", NULL);
                        updateDB("isStatic","0");
                    }
                    else
                    {
                        //LOG_DEBUG("printNetworkOutput() updateDB(isStatic,1)", NULL);
                        updateDB("isStatic","1");
                    }

                    if(ui->autoDNSRadio->isChecked())
                    {
                        updateDB("autoDNS","1");
                    }
                    else
                    {
                        updateDB("autoDNS","0");
                    }

                    // Update the DB file with passwords too
                    if(ui->wpaPassPhraseFrame->isVisible())
                    {
                        updateDB("wpaPassphrase", ui->wpaPassphraseLineEdit->text());
                    }
                    else if(ui->eapUserPassFrame->isVisible())
                    {
                        updateDB("eapUsername", ui->eapUsernameLineEdit->text());
                        updateDB("eapPassword", ui->eapPasswordLineEdit->text());
                    }
                    else if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
                    {
                        //updateDB("")

                        updateDB("serverCertFilename", ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
                        updateDB("chainFilename", dbChainFilename);
                        updateDB("clientCertFilename", getSerialNumber());
                        updateDB("CSRFilename", dbCSRFilename);
                        updateDB("CSRKeyFilename", dbCSRKeyFilename);
                        updateDB("clientKeyFilename", dbClientKeyFilename);
                        updateDB("clientCertUsersFilename", ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
                        updateDB("clientCertPassword", ui->leClientKeyPassword->text());

                    }

                    ////////////////////////
                    doLookAgain = true;
                }
                else
                {
                    changeLights = finishNetworkScreen(noConnectionPhrase);
                }
                if(ui->eapUserPassFrame->isVisible())
                {
                    updateDB("eapUsername", ui->eapUsernameLineEdit->text());
                    updateDB("eapPassword", ui->eapPasswordLineEdit->text());
                }

                this->ui->networkAdvancedBut->setEnabled(true);
                saveConfigsNow();
            }
            else if(output.mid(0,6) == "OUTPUT")
            {
                //LOG_DEBUG( "output after is OUTPUT - This is where it will CONNECT if possible", NULL );

                QStringList args;
                args = output.split("|",QString::KeepEmptyParts);

                if(args.size() > 2)
                {
                    //LOG_DEBUG("printNetworkOutput() we are connected. But how.", NULL);
                    connectionEstablished = true;
                    connectedSuccessfully = true;
                    changeLights = true;

                    if(ui->dhcpRadio->isChecked())
                    {
                        isDHCPCheckedBoxd_Fix = true;
                        commonNetwork->setIsStatic(false);
                        commonNetwork->setIPAddress(args[1]);
                        commonNetwork->setGateway(args[2]);
                        commonNetwork->setSubnetMask(args[3]);

                        QStringList IPList;
                        IPList = args[1].split(".",QString::KeepEmptyParts);

                        if(IPList.size() == 4)
                        {
                            ui->ipAddressLineEdit->setText(IPList.at(0));
                            ui->ipAddressLineEdit_2->setText(IPList.at(1));
                            ui->ipAddressLineEdit_3->setText(IPList.at(2));
                            ui->ipAddressLineEdit_4->setText(IPList.at(3));
                        }
                        else
                        {
                            ui->ipAddressLineEdit->setText("");
                            ui->ipAddressLineEdit_2->setText("");
                            ui->ipAddressLineEdit_3->setText("");
                            ui->ipAddressLineEdit_4->setText("");
                        }

                        QStringList GatewayList;
                        GatewayList = args[2].split(".",QString::KeepEmptyParts);

                        if(GatewayList.size() == 4)
                        {
                            ui->gatewayLineEdit->setText(GatewayList.at(0));
                            ui->gatewayLineEdit_2->setText(GatewayList.at(1));
                            ui->gatewayLineEdit_3->setText(GatewayList.at(2));
                            ui->gatewayLineEdit_4->setText(GatewayList.at(3));
                        }
                        else
                        {
                            ui->gatewayLineEdit->setText("");
                            ui->gatewayLineEdit_2->setText("");
                            ui->gatewayLineEdit_3->setText("");
                            ui->gatewayLineEdit_4->setText("");
                        }

                        QStringList SubnetList;
                        SubnetList = args[3].split(".",QString::KeepEmptyParts);

                        if(SubnetList.size() == 4)
                        {
                            ui->subnetMaskLineEdit->setText(SubnetList.at(0));
                            ui->subnetMaskLineEdit_2->setText(SubnetList.at(1));
                            ui->subnetMaskLineEdit_3->setText(SubnetList.at(2));
                            ui->subnetMaskLineEdit_4->setText(SubnetList.at(3));
                        }
                        else
                        {
                            ui->subnetMaskLineEdit->setText("");
                            ui->subnetMaskLineEdit_2->setText("");
                            ui->subnetMaskLineEdit_3->setText("");
                            ui->subnetMaskLineEdit_4->setText("");
                        }
                    }
                    else
                    {
                        commonNetwork->setIsStatic(true);
                        commonNetwork->setIPAddress(ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
                        commonNetwork->setGateway(ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text() + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
                        commonNetwork->setSubnetMask(ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
                    }

                    if(ui->autoDNSRadio->isChecked())
                    {
                        commonNetwork->setIsAutoDNS(true);
                        if(args.size() > 4)
                        {
                            commonNetwork->setPrimaryDNS(args[4]);
                            QStringList D1List;
                            D1List = args[4].split(".",QString::KeepEmptyParts);
                            ui->DNS1LineEdit->setText(D1List.at(0));
                            ui->DNS1LineEdit_2->setText(D1List.at(1));
                            ui->DNS1LineEdit_3->setText(D1List.at(2));
                            ui->DNS1LineEdit_4->setText(D1List.at(3));
                        }
                        else
                        {
                            commonNetwork->setPrimaryDNS("");
                            ui->DNS1LineEdit->setText("");
                            ui->DNS1LineEdit_2->setText("");
                            ui->DNS1LineEdit_3->setText("");
                            ui->DNS1LineEdit_4->setText("");
                        }
                        if(args.size() > 5)
                        {
                            commonNetwork->setSecondaryDNS(args[5]);
                            QStringList D2List;
                            D2List = args[5].split(".",QString::KeepEmptyParts);
                            ui->DNS2LineEdit->setText(D2List.at(0));
                            ui->DNS2LineEdit_2->setText(D2List.at(1));
                            ui->DNS2LineEdit_3->setText(D2List.at(2));
                            ui->DNS2LineEdit_4->setText(D2List.at(3));
                        }
                        else
                        {
                            commonNetwork->setSecondaryDNS("");
                            ui->DNS2LineEdit->setText("");
                            ui->DNS2LineEdit_2->setText("");
                            ui->DNS2LineEdit_3->setText("");
                            ui->DNS2LineEdit_4->setText("");
                        }
                        if(args.size() > 6)
                        {
                            commonNetwork->setTertiaryDNS(args[6]);
                            QStringList D3List;
                            D3List = args[6].split(".",QString::KeepEmptyParts);
                            ui->DNS3LineEdit->setText(D3List.at(0));
                            ui->DNS3LineEdit_2->setText(D3List.at(1));
                            ui->DNS3LineEdit_3->setText(D3List.at(2));
                            ui->DNS3LineEdit_4->setText(D3List.at(3));
                        }
                        else
                        {
                            commonNetwork->setTertiaryDNS("");
                            ui->DNS3LineEdit->setText("");
                            ui->DNS3LineEdit_2->setText("");
                            ui->DNS3LineEdit_2->setText("");
                            ui->DNS3LineEdit_2->setText("");
                        }
                        ui->alertsLineEdit->setText(ui->alertsLineEdit->text() + tr(" Your IP is : ") + args[1]);
                    }

                    //LOG_DEBUG( "output after is OUTPUT - CONNECTED! YEA!!!!", NULL );
                    if ( isDHCPCheckedBoxd_Fix && isHostNameChecked_Fix && isWiredConnection_Fix )
                    {
                        //LOG_DEBUG( "output after is OUTPUT Connected and now KEEP Hostname checked and hostnameStayChecked is set TRUE", NULL );
                        hostnameStayChecked_FIX = true;
                    }
                    else
                    {
                        //LOG_DEBUG( "output after is OUTPUT Connected and hostnameStayChecked_FIX is FALSE", NULL );
                        hostnameStayChecked_FIX = false;
                    }

                    ui->alertsLineEdit->setText(tr("Connected!") + tr(" Your IP is : ") + args[1]);
                    this->update();
                }
                else
                {
                    LOG_DEBUG("printNetworkOutput() Not connected... 2", NULL);
                    this->ui->alertsLineEdit->setText(tr("Not connected... Try again."));
                    this->update();
                }


                // Set the ip and dns info in the DB whether ther is a connection or not.
                // The system may be setup for future use in a store before the network
                // infrastructure has been installed int he store.


                // scaleSettings DB Values affected
                /*
                    isStatic=1
                    gateway=0.0.0.0
                    netmask=0.0.0.0
                    ipAddress=0.0.0.0
                    autoDNS=0
                    dns1=0.0.0.0
                    dns2=0.0.0.0
                    dns3=0.0.0.0
                */

                // Still need to set all of the values in the DB if the connection was not successful
                // When notso connected, then store the User's values in the DB.

                if(ui->gatewayLineEdit->text() == "" && ui->gatewayLineEdit_2->text() == "" && ui->gatewayLineEdit_3->text() == "" && ui->gatewayLineEdit_4->text() == "")
                {
                    this->updateDB("gateway", "64641964");
                }
                else
                {
                    this->updateDB("gateway",ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text()  + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
                }

                this->updateDB("netmask", ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
                this->updateDB("ipAddress", ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
                this->updateDB("dns1", ui->DNS1LineEdit->text() + "." + ui->DNS1LineEdit_2->text() + "." + ui->DNS1LineEdit_3->text() + "." + ui->DNS1LineEdit_4->text());
                this->updateDB("dns2", ui->DNS2LineEdit->text() + "." + ui->DNS2LineEdit_2->text() + "." + ui->DNS2LineEdit_3->text() + "." + ui->DNS2LineEdit_4->text());
                this->updateDB("dns3", ui->DNS3LineEdit->text() + "." + ui->DNS3LineEdit_2->text() + "." + ui->DNS3LineEdit_3->text() + "." + ui->DNS3LineEdit_4->text());
                this->updateDB("security", QString::number(ui->securityCombo->currentIndex()));
                this->updateDB("encryption", QString::number(ui->encryptionCombo->currentIndex()));

                if(ui->securityCombo->currentIndex() == 0)
                {
                    // OPEN
                    this->updateDB("eapUsername", "");
                    this->updateDB("eapPassword", "");
                    this->updateDB("wpaPassphrase", "");
                    this->updateDB("serverCertFilename","");
                    this->updateDB("clientCertFilename", "");
                    this->updateDB("clientCertUsersFilename", "");
                    this->updateDB("clientCertPassword", "");
                    this->ui->wpaPassphraseLineEdit->setText("");
                    ui->eapPasswordLineEdit->setText("");
                    ui->eapUsernameLineEdit->setText("");
                    ui->leClientKeyPassword->setText("");

                    if(ui->encryptionCombo->currentIndex() == 1)
                    {
                        // open none
                        ui->wepKey1LineEdit->setText("");
                        ui->wepKey2LineEdit->setText("");
                        ui->wepKey3LineEdit->setText("");
                        ui->wepKey4LineEdit->setText("");
                    }
                }
                else if(ui->securityCombo->currentIndex() == 1 || ui->securityCombo->currentIndex() == 5)
                {
                    this->updateDB("eapUsername", "");
                    this->updateDB("eapPassword", "");
                    this->updateDB("wpaPassphrase", ui->wpaPassphraseLineEdit->text());
                    this->updateDB("serverCertFilename","");
                    this->updateDB("clientCertFilename", "");
                    this->updateDB("clientCertUsersFilename", "");
                    this->updateDB("clientCertPassword", "");
                    ui->eapPasswordLineEdit->setText("");
                    ui->eapUsernameLineEdit->setText("");
                    ui->wepKey1LineEdit->setText("");
                    ui->wepKey2LineEdit->setText("");
                    ui->wepKey3LineEdit->setText("");
                    ui->wepKey4LineEdit->setText("");
                    ui->leClientKeyPassword->setText("");
                }
                else if(ui->securityCombo->currentIndex() == 2 || ui->securityCombo->currentIndex() == 6)
                {
                    this->updateDB("eapUsername", "");
                    this->updateDB("eapPassword", "");
                    this->updateDB("wpaPassphrase", "");
                    this->updateDB("clientCertPassword",ui->leClientKeyPassword->text());
                    this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());

                    ui->eapPasswordLineEdit->setText("");
                    ui->eapUsernameLineEdit->setText("");
                    ui->wepKey1LineEdit->setText("");
                    ui->wepKey2LineEdit->setText("");
                    ui->wepKey3LineEdit->setText("");
                    ui->wepKey4LineEdit->setText("");
                    ui->wpaPassphraseLineEdit->setText("");

                    updateDB("serverCertFilename", ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
                    updateDB("chainFilename", dbChainFilename);
                    //updateDB("clientCertFilename", getSerialNumber());
                    updateDB("CSRFilename", dbCSRFilename);
                    updateDB("CSRKeyFilename", dbCSRKeyFilename);
                    updateDB("clientKeyFilename", dbClientKeyFilename);
                    updateDB("clientCertUsersFilename", ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
                    updateDB("clientCertPassword", ui->leClientKeyPassword->text());
                }
                else if (ui->eapUserPassFrame->isVisible())
                {
                    this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());
                    this->updateDB("eapPassword", ui->eapPasswordLineEdit->text());
                }
                else
                {
                    ;//////qDebug() << "XX: " << output;
                    this->updateDB("serverCertFilename","");
                    this->updateDB("clientCertFilename", "");
                    this->updateDB("clientCertUsersFilename", "");
                    this->updateDB("clientCertPassword", "");
                    ui->wepKey1LineEdit->setText("");
                    ui->wepKey2LineEdit->setText("");
                    ui->wepKey3LineEdit->setText("");
                    ui->wepKey4LineEdit->setText("");
                    ui->leClientKeyPassword->setText("");
                    ui->wpaPassphraseLineEdit->setText("");
                }

                //this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());
                //this->updateDB("eapPassword",ui->eapPasswordLineEdit->text());
                //this->updateDB("wpaPassphrase", ui->wpaPassphraseLineEdit->text());

                saveConfigsNow();
                this->ui->connectingProgressBar->hide();
                this->ui->connectingProgressBar->lower();
                //this->ui->exportBut->setEnabled(true);        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
                this->ui->importBut->setEnabled(true);
                this->ui->refreshNetworksBut->setEnabled(true);
                this->ui->availableConnectionsTableView->setEnabled(true);
                this->ui->networkSaveBut->setEnabled(true);
                this->ui->addNetworkBut->setEnabled(true);
                this->ui->serverCertBut->setEnabled(true);
                this->ui->clientCertBut->setEnabled(true);
                this->ui->searchTrustedRootCertBut->setEnabled(true);
                this->ui->searchClientCertBut->setEnabled(true);
                this->ui->serverCertDeleteBut->setEnabled(true);
                this->ui->clientCertDeleteBut->setEnabled(true);
                this->ui->trustedRootStoreCombo->setEnabled(true);
                this->ui->clientCertStoreCombo->setEnabled(true);
                this->ui->networkAdvancedBut->setEnabled(true);

                if(newConnectionFlag == "Wire" || newConnectionFlag == "Wireless")
                {
                    changeLights = true;
                    if(newConnectionFlag == "Wire")
                    {
                        this->ui->availableConnectionsTableView->selectRow(0);
                    }
                    newConnectionFlag == "";
                }

            }
            else
            {
                ;//////qDebug() << "XX: " << output;
            }
        }
        //qApp->processEvents();

        if(changeLights)
        {
            //this->ui->exportBut->setEnabled(true);        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
            this->ui->importBut->setEnabled(true);
            this->ui->refreshNetworksBut->setEnabled(true);
            this->ui->availableConnectionsTableView->setEnabled(true);

            if(newConnectionFlag == "Wireless")
            {
                bool found = false;
                for(int i = 0;i < this->ui->availableConnectionsTableView->rowCount() && !found; i++)
                {
                    QTableWidgetItem *wirelessItem;
                    wirelessItem = this->ui->availableConnectionsTableView->item(i,1);
                    ////qDebug() << "connection name is: " << wirelessItem->text();
                    if(wirelessItem->text() == this->ui->SSIDLineEdit->text())
                    {
                        //this->ui->availableConnectionsTableView->selectRow(i);
                        newRow = i;
                        found = true;
                    }
                }
            }
            else if(ui->wirelessAdapterGroupBox->isVisible() == false)
            {
                newRow = 0;
            }

            this->changeConnectedIcons(newRow,connectionEstablished);
            this->ui->connectingProgressBar->hide();
            this->ui->connectingProgressBar->lower();
            this->ui->networkSaveBut->setEnabled(true);
            this->ui->addNetworkBut->setEnabled(true);
            this->ui->serverCertBut->setEnabled(true);
            this->ui->clientCertBut->setEnabled(true);
            this->ui->searchTrustedRootCertBut->setEnabled(true);
            this->ui->searchClientCertBut->setEnabled(true);
            this->ui->serverCertDeleteBut->setEnabled(true);
            this->ui->clientCertDeleteBut->setEnabled(true);
            this->ui->trustedRootStoreCombo->setEnabled(true);
            this->ui->clientCertStoreCombo->setEnabled(true);
            this->ui->networkAdvancedBut->setEnabled(true);

        }

    }

    if (doLookAgain)
        lookatNetworkAgain("");

    LOG_DEBUG("printNetworkOutput() END", NULL);
}

bool setupUtility::finishNetworkScreen(QString output)
{
    QStringList args;
    bool changeLights;
    //ui->alertsLineEdit->setText(output);
    //qDebug() << "output is : " << output;
    args = output.split("|",QString::KeepEmptyParts);
    //if(args.size() > 2 && connectionEstablished)
    if(args.size() > 2)
    {
        connectionEstablished = true;
        connectedSuccessfully = true;
        changeLights = true;
        //ui->alertsLineEdit->setText(output);
        if(ui->dhcpRadio->isChecked())
        {
            commonNetwork->setIsStatic(false);
            commonNetwork->setIPAddress(args[1]);
            commonNetwork->setGateway(args[2]);
            commonNetwork->setSubnetMask(args[3]);

            QStringList IPList;
            //qDebug() << "output is : " << output;
            IPList = args[1].split(".",QString::KeepEmptyParts);

            if(IPList.size() == 4)
            {
                ui->ipAddressLineEdit->setText(IPList.at(0));
                ui->ipAddressLineEdit_2->setText(IPList.at(1));
                ui->ipAddressLineEdit_3->setText(IPList.at(2));
                ui->ipAddressLineEdit_4->setText(IPList.at(3));
            }
            else
            {
                ui->ipAddressLineEdit->setText("");
                ui->ipAddressLineEdit_2->setText("");
                ui->ipAddressLineEdit_3->setText("");
                ui->ipAddressLineEdit_4->setText("");
            }

            QStringList GatewayList;
            //qDebug() << "output is : " << output;
            GatewayList = args[2].split(".",QString::KeepEmptyParts);

            if(GatewayList.size() == 4)
            {
                ui->gatewayLineEdit->setText(GatewayList.at(0));
                ui->gatewayLineEdit_2->setText(GatewayList.at(1));
                ui->gatewayLineEdit_3->setText(GatewayList.at(2));
                ui->gatewayLineEdit_4->setText(GatewayList.at(3));
            }
            else
            {
                ui->gatewayLineEdit->setText("");
                ui->gatewayLineEdit_2->setText("");
                ui->gatewayLineEdit_3->setText("");
                ui->gatewayLineEdit_4->setText("");
            }
                //ui->gatewayLineEdit->setText(args[2]);

            QStringList SubnetList;
            //qDebug() << "output is : " << output;
            SubnetList = args[3].split(".",QString::KeepEmptyParts);

            if(SubnetList.size() == 4)
            {
                ui->subnetMaskLineEdit->setText(SubnetList.at(0));
                ui->subnetMaskLineEdit_2->setText(SubnetList.at(1));
                ui->subnetMaskLineEdit_3->setText(SubnetList.at(2));
                ui->subnetMaskLineEdit_4->setText(SubnetList.at(3));
            }
            else
            {
                ui->subnetMaskLineEdit->setText("");
                ui->subnetMaskLineEdit_2->setText("");
                ui->subnetMaskLineEdit_3->setText("");
                ui->subnetMaskLineEdit_4->setText("");
            }
            //ui->subnetMaskLineEdit->setText(args[3]);
        }
        else
        {
            commonNetwork->setIsStatic(true);
            commonNetwork->setIPAddress(ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
            commonNetwork->setGateway(ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text() + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
            commonNetwork->setSubnetMask(ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
        }

        if(ui->autoDNSRadio->isChecked())
        {
            commonNetwork->setIsAutoDNS(true);
            if(args.size() > 4)
            {
                commonNetwork->setPrimaryDNS(args[4]);
                QStringList D1List;
                //qDebug() << "output is : " << output;
                D1List = args[4].split(".",QString::KeepEmptyParts);
                ui->DNS1LineEdit->setText(D1List.at(0));
                ui->DNS1LineEdit_2->setText(D1List.at(1));
                ui->DNS1LineEdit_3->setText(D1List.at(2));
                ui->DNS1LineEdit_4->setText(D1List.at(3));
            }
            else
            {
                commonNetwork->setPrimaryDNS("");
                ui->DNS1LineEdit->setText("");
                ui->DNS1LineEdit_2->setText("");
                ui->DNS1LineEdit_3->setText("");
                ui->DNS1LineEdit_4->setText("");
            }
            if(args.size() > 5)
            {
                commonNetwork->setSecondaryDNS(args[5]);
                QStringList D2List;
                //qDebug() << "output is : " << output;
                D2List = args[5].split(".",QString::KeepEmptyParts);
                ui->DNS2LineEdit->setText(D2List.at(0));
                ui->DNS2LineEdit_2->setText(D2List.at(1));
                ui->DNS2LineEdit_3->setText(D2List.at(2));
                ui->DNS2LineEdit_4->setText(D2List.at(3));

                //ui->DNS2LineEdit->setText(args[5]);
            }
            else
            {
                commonNetwork->setSecondaryDNS("");
                ui->DNS2LineEdit->setText("");
                ui->DNS2LineEdit_2->setText("");
                ui->DNS2LineEdit_3->setText("");
                ui->DNS2LineEdit_4->setText("");
            }
            if(args.size() > 6)
            {
                commonNetwork->setTertiaryDNS(args[6]);
                QStringList D3List;
                //qDebug() << "output is : " << output;
                D3List = args[6].split(".",QString::KeepEmptyParts);
                ui->DNS3LineEdit->setText(D3List.at(0));
                ui->DNS3LineEdit_2->setText(D3List.at(1));
                ui->DNS3LineEdit_3->setText(D3List.at(2));
                ui->DNS3LineEdit_4->setText(D3List.at(3));
            }
            else
            {
                commonNetwork->setTertiaryDNS("");
                ui->DNS3LineEdit->setText("");
                ui->DNS3LineEdit_2->setText("");
                ui->DNS3LineEdit_2->setText("");
                ui->DNS3LineEdit_2->setText("");
            }
            ui->alertsLineEdit->setText(ui->alertsLineEdit->text() + tr(" Your IP is : ") + args[1]);
        }
        ui->alertsLineEdit->setText(tr("Connected!") + tr(" Your IP is : ") + args[1]);
        this->update();
    }
    else
    {
        ui->alertsLineEdit->setText(tr("Not connected... Try again."));
        //ui->ipAddressLineEdit->setText("");
        //ui->ipAddressLineEdit_2->setText("");
        //ui->ipAddressLineEdit_3->setText("");
        //ui->ipAddressLineEdit_4->setText("");

        //ui->subnetMaskLineEdit->setText("");
        //ui->subnetMaskLineEdit_2->setText("");
        //ui->subnetMaskLineEdit_3->setText("");
        //ui->subnetMaskLineEdit_4->setText("");

        //ui->gatewayLineEdit->setText("");
        //ui->gatewayLineEdit_2->setText("");
        //ui->gatewayLineEdit_3->setText("");
        //ui->gatewayLineEdit_4->setText("");

        //ui->DNS1LineEdit->setText("");
        //ui->DNS1LineEdit_2->setText("");
        //ui->DNS1LineEdit_3->setText("");
        //ui->DNS1LineEdit_4->setText("");

        //ui->DNS2LineEdit->setText("");
        //ui->DNS2LineEdit_2->setText("");
        //ui->DNS2LineEdit_3->setText("");
        //ui->DNS2LineEdit_4->setText("");

        //ui->DNS3LineEdit->setText("");
        //ui->DNS3LineEdit_2->setText("");
        //ui->DNS3LineEdit_3->setText("");
        //ui->DNS3LineEdit_4->setText("");

        //XXXXXXXXXX MAY 20

        this->update();
    }


        // Set the ip and dns info in the DB whether ther is a connection or not.
    // The system may be setup for future use in a store before the network
    // infrastructure has been installed int he store.


    // scaleSettings DB Values affected
    /*
        isStatic=1
        gateway=0.0.0.0
        netmask=0.0.0.0
        ipAddress=0.0.0.0
        autoDNS=0
        dns1=0.0.0.0
        dns2=0.0.0.0
        dns3=0.0.0.0
    */

    // Still need to set all of the values in the DB if the connection was not successful
    // When notso connected, then store the User's values in the DB.

    if(ui->gatewayLineEdit->text() == "" && ui->gatewayLineEdit_2->text() == "" && ui->gatewayLineEdit_3->text() == "" && ui->gatewayLineEdit_4->text() == "")
    {
        this->updateDB("gateway", "64641964");
    }
    else
    {
        this->updateDB("gateway",ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text()  + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
    }
    this->updateDB("netmask", ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
    this->updateDB("ipAddress", ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
    this->updateDB("dns1", ui->DNS1LineEdit->text() + "." + ui->DNS1LineEdit_2->text() + "." + ui->DNS1LineEdit_3->text() + "." + ui->DNS1LineEdit_4->text());
    this->updateDB("dns2", ui->DNS2LineEdit->text() + "." + ui->DNS2LineEdit_2->text() + "." + ui->DNS2LineEdit_3->text() + "." + ui->DNS2LineEdit_4->text());
    this->updateDB("dns3", ui->DNS3LineEdit->text() + "." + ui->DNS3LineEdit_2->text() + "." + ui->DNS3LineEdit_3->text() + "." + ui->DNS3LineEdit_4->text());
    this->updateDB("security", QString::number(ui->securityCombo->currentIndex()));
    this->updateDB("encryption", QString::number(ui->encryptionCombo->currentIndex()));

    if(ui->securityCombo->currentIndex() == 0)
    {
        // OPEN
        this->updateDB("eapUsername", "");
        this->updateDB("eapPassword", "");
        this->updateDB("wpaPassphrase", "");
        this->updateDB("serverCertFilename","");
        this->updateDB("clientCertFilename", "");
        this->updateDB("clientCertUsersFilename", "");
        this->updateDB("clientCertPassword", "");
        this->ui->wpaPassphraseLineEdit->setText("");
        ui->eapPasswordLineEdit->setText("");
        ui->eapUsernameLineEdit->setText("");
        ui->leClientKeyPassword->setText("");
        if(ui->encryptionCombo->currentIndex() == 1)
        {
            // open none
            ui->wepKey1LineEdit->setText("");
            ui->wepKey2LineEdit->setText("");
            ui->wepKey3LineEdit->setText("");
            ui->wepKey4LineEdit->setText("");
        }
    }
    else if(ui->securityCombo->currentIndex() == 1 || ui->securityCombo->currentIndex() == 5)
    {
        this->updateDB("eapUsername", "");
        this->updateDB("eapPassword", "");
        this->updateDB("wpaPassphrase", ui->wpaPassphraseLineEdit->text());
        this->updateDB("serverCertFilename","");
        this->updateDB("clientCertFilename", "");
        this->updateDB("clientCertUsersFilename", "");
        this->updateDB("clientCertPassword", "");
        ui->eapPasswordLineEdit->setText("");
        ui->eapUsernameLineEdit->setText("");
        ui->wepKey1LineEdit->setText("");
        ui->wepKey2LineEdit->setText("");
        ui->wepKey3LineEdit->setText("");
        ui->wepKey4LineEdit->setText("");
        ui->leClientKeyPassword->setText("");
    }
    else if(ui->securityCombo->currentIndex() == 2 || ui->securityCombo->currentIndex() == 6)
    {
        this->updateDB("eapUsername", "");
        this->updateDB("eapPassword", "");
        this->updateDB("wpaPassphrase", "");
        this->updateDB("clientCertPassword",ui->leClientKeyPassword->text());
        this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());

        ui->eapPasswordLineEdit->setText("");
        ui->eapUsernameLineEdit->setText("");
        ui->wepKey1LineEdit->setText("");
        ui->wepKey2LineEdit->setText("");
        ui->wepKey3LineEdit->setText("");
        ui->wepKey4LineEdit->setText("");
        ui->wpaPassphraseLineEdit->setText("");

        updateDB("serverCertFilename", ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
        updateDB("chainFilename", dbChainFilename);
        //updateDB("clientCertFilename", getSerialNumber());
        updateDB("CSRFilename", dbCSRFilename);
        updateDB("CSRKeyFilename", dbCSRKeyFilename);
        updateDB("clientKeyFilename", dbClientKeyFilename);
        updateDB("clientCertUsersFilename", ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
        updateDB("clientCertPassword", ui->leClientKeyPassword->text());
    }
    else
    {
        ;//////qDebug() << "XX: " << output;
        this->updateDB("serverCertFilename","");
        this->updateDB("clientCertFilename", "");
        this->updateDB("clientCertUsersFilename", "");
        this->updateDB("clientCertPassword", "");
        ui->wepKey1LineEdit->setText("");
        ui->wepKey2LineEdit->setText("");
        ui->wepKey3LineEdit->setText("");
        ui->wepKey4LineEdit->setText("");
        ui->leClientKeyPassword->setText("");
        ui->wpaPassphraseLineEdit->setText("");
    }

    //this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());
    //this->updateDB("eapPassword",ui->eapPasswordLineEdit->text());
    //this->updateDB("wpaPassphrase", ui->wpaPassphraseLineEdit->text());

    saveConfigsNow();
    this->ui->connectingProgressBar->hide();
    this->ui->connectingProgressBar->lower();
    // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
    // for example, how can we enalbed EXPORT when we have never done it before.
    //this->ui->exportBut->setEnabled(true);
    this->ui->importBut->setEnabled(true);
    this->ui->refreshNetworksBut->setEnabled(true);
    this->ui->availableConnectionsTableView->setEnabled(true);
    this->ui->networkSaveBut->setEnabled(true);
    this->ui->addNetworkBut->setEnabled(true);
    this->ui->serverCertBut->setEnabled(true);
    this->ui->clientCertBut->setEnabled(true);
    this->ui->searchTrustedRootCertBut->setEnabled(true);
    this->ui->searchClientCertBut->setEnabled(true);
    this->ui->serverCertDeleteBut->setEnabled(true);
    this->ui->clientCertDeleteBut->setEnabled(true);
    this->ui->trustedRootStoreCombo->setEnabled(true);
    this->ui->clientCertStoreCombo->setEnabled(true);
    this->ui->networkAdvancedBut->setEnabled(true);

    if(newConnectionFlag == "Wire" || newConnectionFlag == "Wireless")
    {
        changeLights = true;
        if(newConnectionFlag == "Wire")
        {
            this->ui->availableConnectionsTableView->selectRow(0);
        }
        newConnectionFlag == "";
    }
    return changeLights;

}

void setupUtility::lookatNetworkAgain(QString output)
{
    LOG_CLASS_INFO( "setupUtility", "lookatNetworkAgain" );
    //LOG_DEBUG( "lookatNetworkAgain() START", NULL );

    //int newRow = -1;
    //bool changeLights = false;

    //get the output

   // dsd
            QProcess macAddressProcess;
            QStringList options;
            options.append(filesystem::absoluteFilePath("lookAtNetworkAgain.py",folderType::Python));

            macAddressProcess.start("python", options);

            if(!macAddressProcess.waitForStarted())
            {
                this->ui->alertsLineEdit->setText(tr("Start configuration error."));
            }

            if(!macAddressProcess.waitForFinished())
            {
                ;
            }

            QStringList macAddresses;
            QByteArray result = macAddressProcess.readAll();
            QString macAddress(result);
            macAddress = macAddress.mid(0,macAddress.length()-1);
            output = macAddress;
            //qDebug() << "total string is:" << macAddress;
            // added in a string length check
            if(macAddress.size() > 0)
            {
                macAddresses = macAddress.split("|", QString::SkipEmptyParts);
                this->ui->ethernetMACAddressLabel->setText((macAddresses[0]).toUpper());
                this->ui->wirelessMACAddressLabel->setText((macAddresses[1]).toUpper());
                this->ui->wireMAClbl->setText((macAddresses[0]).toUpper());
                this->ui->wifiMAClbl->setText((macAddresses[1]).toUpper());
            }


    if(output.mid(0,6) == "OUTPUT")
    {
        //endNetworkConnection();
        //saveNetworkConfigurationProcess->kill();
        // we have connection info like gateway, netmask, etc.
        //order is ip|gateway|metmask|dn1|dns2|dn3
        QStringList args;
        //ui->alertsLineEdit->setText(output);
        //qDebug() << "output is : " << output;
        args = output.split("|",QString::KeepEmptyParts);
        //if(args.size() > 2 && connectionEstablished)
        if(args.size() > 2)
        {
            connectionEstablished = true;
            connectedSuccessfully = true;
            //changeLights = true;
            //ui->alertsLineEdit->setText(output);
            if(ui->dhcpRadio->isChecked())
            {
                commonNetwork->setIsStatic(false);
                commonNetwork->setIPAddress(args[1]);
                commonNetwork->setGateway(args[2]);
                commonNetwork->setSubnetMask(args[3]);

                QStringList IPList;
                //qDebug() << "output is : " << output;
                IPList = args[1].split(".",QString::KeepEmptyParts);

                if(IPList.size() == 4)
                {
                    ui->ipAddressLineEdit->setText(IPList.at(0));
                    ui->ipAddressLineEdit_2->setText(IPList.at(1));
                    ui->ipAddressLineEdit_3->setText(IPList.at(2));
                    ui->ipAddressLineEdit_4->setText(IPList.at(3));
                }
                else
                {
                    ui->ipAddressLineEdit->setText("");
                    ui->ipAddressLineEdit_2->setText("");
                    ui->ipAddressLineEdit_3->setText("");
                    ui->ipAddressLineEdit_4->setText("");
                }

                QStringList GatewayList;
                //qDebug() << "output is : " << output;
                GatewayList = args[2].split(".",QString::KeepEmptyParts);

                if(GatewayList.size() == 4)
                {
                    ui->gatewayLineEdit->setText(GatewayList.at(0));
                    ui->gatewayLineEdit_2->setText(GatewayList.at(1));
                    ui->gatewayLineEdit_3->setText(GatewayList.at(2));
                    ui->gatewayLineEdit_4->setText(GatewayList.at(3));
                }
                else
                {
                    ui->gatewayLineEdit->setText("");
                    ui->gatewayLineEdit_2->setText("");
                    ui->gatewayLineEdit_3->setText("");
                    ui->gatewayLineEdit_4->setText("");
                }
                    //ui->gatewayLineEdit->setText(args[2]);

                QStringList SubnetList;
                //qDebug() << "output is : " << output;
                SubnetList = args[3].split(".",QString::KeepEmptyParts);

                if(SubnetList.size() == 4)
                {
                    ui->subnetMaskLineEdit->setText(SubnetList.at(0));
                    ui->subnetMaskLineEdit_2->setText(SubnetList.at(1));
                    ui->subnetMaskLineEdit_3->setText(SubnetList.at(2));
                    ui->subnetMaskLineEdit_4->setText(SubnetList.at(3));
                }
                else
                {
                    ui->subnetMaskLineEdit->setText("");
                    ui->subnetMaskLineEdit_2->setText("");
                    ui->subnetMaskLineEdit_3->setText("");
                    ui->subnetMaskLineEdit_4->setText("");
                }
                //ui->subnetMaskLineEdit->setText(args[3]);
            }
            else
            {
                commonNetwork->setIsStatic(true);
                commonNetwork->setIPAddress(ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());
                commonNetwork->setGateway(ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text() + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());
                commonNetwork->setSubnetMask(ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());
            }

            if(ui->autoDNSRadio->isChecked())
            {
                commonNetwork->setIsAutoDNS(true);
                if(args.size() > 4)
                {
                    commonNetwork->setPrimaryDNS(args[4]);
                    QStringList D1List;
                    //qDebug() << "output is : " << output;
                    D1List = args[4].split(".",QString::KeepEmptyParts);
                    ui->DNS1LineEdit->setText(D1List.at(0));
                    ui->DNS1LineEdit_2->setText(D1List.at(1));
                    ui->DNS1LineEdit_3->setText(D1List.at(2));
                    ui->DNS1LineEdit_4->setText(D1List.at(3));
                }
                else
                {
                    commonNetwork->setPrimaryDNS("");
                    ui->DNS1LineEdit->setText("");
                    ui->DNS1LineEdit_2->setText("");
                    ui->DNS1LineEdit_3->setText("");
                    ui->DNS1LineEdit_4->setText("");
                }
                if(args.size() > 5)
                {
                    commonNetwork->setSecondaryDNS(args[5]);
                    QStringList D2List;
                    //qDebug() << "output is : " << output;
                    D2List = args[5].split(".",QString::KeepEmptyParts);
                    ui->DNS2LineEdit->setText(D2List.at(0));
                    ui->DNS2LineEdit_2->setText(D2List.at(1));
                    ui->DNS2LineEdit_3->setText(D2List.at(2));
                    ui->DNS2LineEdit_4->setText(D2List.at(3));

                    //ui->DNS2LineEdit->setText(args[5]);
                }
                else
                {
                    commonNetwork->setSecondaryDNS("");
                    ui->DNS2LineEdit->setText("");
                    ui->DNS2LineEdit_2->setText("");
                    ui->DNS2LineEdit_3->setText("");
                    ui->DNS2LineEdit_4->setText("");
                }
                if(args.size() > 6)
                {
                    commonNetwork->setTertiaryDNS(args[6]);
                    QStringList D3List;
                    //qDebug() << "output is : " << output;
                    D3List = args[6].split(".",QString::KeepEmptyParts);
                    ui->DNS3LineEdit->setText(D3List.at(0));
                    ui->DNS3LineEdit_2->setText(D3List.at(1));
                    ui->DNS3LineEdit_3->setText(D3List.at(2));
                    ui->DNS3LineEdit_4->setText(D3List.at(3));
                }
                else
                {
                    commonNetwork->setTertiaryDNS("");
                    ui->DNS3LineEdit->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_2->setText("");
                }
                ui->alertsLineEdit->setText(ui->alertsLineEdit->text() + tr(" Your IP is : ") + args[1]);
            }
            ui->alertsLineEdit->setText(tr("Connected!") + tr(" Your IP is : ") + args[1]);
            this->update();
        }
    }
    else
    {
        //LOG_DEBUG( "lookatNetworkAgain() Not connected...", NULL );
        ui->alertsLineEdit->setText(tr("Not connected... Try again."));
        this->update();

    }
    //LOG_DEBUG( "lookatNetworkAgain() END", NULL );
}

void setupUtility::saveConfigsNow()
{
    LOG_CLASS_INFO("setupUtility","saveConfigsNow");
    LOG_DEBUG("saveConfigsNow() START", NULL);

    //processingAButton = true;
    python pythonProcess;

    LOG_DEBUG( "saveConfigsNow() - Now call saveConfigs()", NULL );

    if (pythonProcess.execCommand("saveConfigs", this->buildSaveConfigurationOptions(commonNetwork)))
       {
           // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
           // for example, how can we enalbed EXPORT when we have never done it before.
           if (pythonProcess.getExitCode() == 0)
           {
               updateDB("firstTimeExport","1");
               //ui->exportBut->setEnabled(true);
               //ui->exportBut->setDisabled(false);
               //TBD - updateCustomerVolume();
           }
           else
           {
               //ui->exportBut->setEnabled(true);
               //ui->exportBut->setDisabled(false);
               // TODO:
               // what do we do here?
               //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
           }
       }
       else
       {
           // TODO:
           // What do we do here?
       }
    //processingAButton = false;
    LOG_DEBUG( "saveConfigsNow() DONE", NULL );
}

void setupUtility::changeConnectedIcons(int rowToConnect, bool connectionOKAY)
{
    // set disconnected icons across the board
    QString lightFileName = ":/images/screens/disconnectedLightFinal.png";
    for(int i=0; i<this->ui->availableConnectionsTableView->rowCount(); i++)
    {
        QPixmap lightPic(lightFileName);
        QIcon lightIcon;

        lightIcon.actualSize(QSize(35,69));
        lightIcon.addPixmap(lightPic,QIcon::Normal);
        lightIcon.addPixmap(lightPic,QIcon::Selected);

        QTableWidgetItem *connectionIconItem = new QTableWidgetItem;
        connectionIconItem->setSizeHint(QSize(35,69));
        connectionIconItem->setIcon(lightIcon);

        this->ui->availableConnectionsTableView->setItem(i, 0, connectionIconItem);
    }
    // set connected icon
    if(connectionOKAY)
    {
        lightFileName = ":/images/screens/connectedLightFinal.png";
        QPixmap clightPic(lightFileName);
        QIcon clightIcon;

        clightIcon.actualSize(QSize(35,69));
        clightIcon.addPixmap(clightPic,QIcon::Normal);
        clightIcon.addPixmap(clightPic,QIcon::Selected);
        QTableWidgetItem *cconnectionIconItem = new QTableWidgetItem;
        cconnectionIconItem->setSizeHint(QSize(35,69));
        cconnectionIconItem->setIcon(clightIcon);

        this->ui->availableConnectionsTableView->setItem(rowToConnect, 0, cconnectionIconItem);
    }
}


void setupUtility::readyReadNetworkStandardError()
{
    LOG_CLASS_INFO( "setupUtility", "readyReadNetworkStandardError" );
    //LOG_DEBUG( "readyReadNetworkStandardError() START", NULL );

    QString output = this->saveNetworkConfigurationProcess->readAllStandardError();

    //////qDebug() << output;
    /*if(networkProcessConnected)
    {
        QString output = this->saveNetworkConfigurationProcess->readAllStandardError();
         this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
        //saveNetworkConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
    }
    networkProcessConnected = false;
    */

    //LOG_DEBUG( "readyReadNetworkStandardError() END", NULL );
}

void setupUtility::readyReadEAPStandardError()
{
    LOG_CLASS_INFO( "setupUtility", "readyReadEAPStandardError" );
    //LOG_DEBUG( "readyReadEAPStandardError() START", NULL );

    QString output = this->saveNetworkConfigurationProcess->readAllStandardError();

    //////qDebug() << output;
    /*if(networkProcessConnected)
    {
        QString output = this->saveNetworkConfigurationProcess->readAllStandardError();
         this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
        //saveNetworkConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
    }
    networkProcessConnected = false;
    */

    //LOG_DEBUG( "readyReadEAPStandardError() END", NULL );
}

void setupUtility::endNetworkConnection()
{
    LOG_CLASS_INFO( "setupUtility", "endNetworkConnection" );
    //LOG_DEBUG( "endNetworkConnection() START", NULL );

    //////qDebug() << "finished";

    //XXX COMMENT LINE BELOW AFTER TESTING
   // this->ui->alertsLineEdit->setText("Finished Process");

    this->ui->mainTab->setEnabled(true);
    if(mustTurnOnDialogs)
    {
       //previous
       //;// session_->send(SessionDev , SnRestartDialogs);
       //new
        session_->send(SessionDev , SnRestartDialogs);
    }
    if(networkProcessConnected)
    {
        saveNetworkConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
        //this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);
    }
    networkProcessConnected = false;
    if(!connectedSuccessfully)
    {
        ;
    }
    this->ui->connectingProgressBar->hide();
    this->ui->connectingProgressBar->lower();
    this->ui->connectingProgressBar->setValue(0);
    // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
    //this->ui->exportBut->setEnabled(true);
    this->ui->importBut->setEnabled(true);
    this->ui->refreshNetworksBut->setEnabled(true);
    this->ui->availableConnectionsTableView->setEnabled(true);
    this->ui->networkSaveBut->setEnabled(true);
    this->ui->addNetworkBut->setEnabled(true);
    this->ui->serverCertBut->setEnabled(true);
    this->ui->clientCertBut->setEnabled(true);
    this->ui->searchTrustedRootCertBut->setEnabled(true);
    this->ui->searchClientCertBut->setEnabled(true);
    this->ui->serverCertDeleteBut->setEnabled(true);
    this->ui->clientCertDeleteBut->setEnabled(true);
    this->ui->trustedRootStoreCombo->setEnabled(true);
    this->ui->clientCertStoreCombo->setEnabled(true);

    newConnectionFlag = "";
    // Blank this flag so other connections won't try to display wrong message
    processingAButton = false;

    //XXX ADDED THIS 10/14/2014
    //saveNetworkConfigurationProcess->kill();

    //LOG_DEBUG( "endNetworkConnection() END", NULL );
}

void setupUtility::finishedNetworkProcess(int value, QProcess::ExitStatus exitStatus)
{
    LOG_CLASS_INFO( "setupUtility", "finishedNetworkProcess" );
    LOG_DEBUG( "finishedNetworkProcess() START", NULL );

    //processingAButton = true;

    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    //////qDebug() << "finished";

    //XXX COMMENT LINE BELOW AFTER TESTING
   // this->ui->alertsLineEdit->setText("Finished Process");

    //this->ui->mainTab->setEnabled(true);
    if(mustTurnOnDialogs)
    {
       ;// session_->send(SessionDev , SnRestartDialogs);
    }
    if(networkProcessConnected)
    {
        saveNetworkConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
        //this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);

    }
    networkProcessConnected = false;
    if(!connectedSuccessfully)
    {
        //LOG_DEBUG( "finishedNetworkProcess() Not connected...", NULL );
        this->ui->alertsLineEdit->setText(tr("Not connected... Try again."));
        if(ui->wirelessAdapterGroupBox->isVisible())
        {
            python pythonProcess;
            QStringList myOptions;
            if (pythonProcess.execCommand("stopWireless", myOptions))
            {
                if (pythonProcess.getExitCode() == 0)
                {
                    ;//ui->alertsLineEdit->setText("GOOD");
                }
                else
                {
                    ;//ui->alertsLineEdit->setText("BAD");
                }
            }
        }
    }
    this->ui->connectingProgressBar->hide();
    this->ui->connectingProgressBar->lower();
    this->ui->connectingProgressBar->setValue(0);
    // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
    //this->ui->exportBut->setEnabled(true);
    this->ui->importBut->setEnabled(true);
    this->ui->refreshNetworksBut->setEnabled(true);
    this->ui->availableConnectionsTableView->setEnabled(true);
    this->ui->networkSaveBut->setEnabled(true);
    this->ui->addNetworkBut->setEnabled(true);
    this->ui->serverCertBut->setEnabled(true);
    this->ui->clientCertBut->setEnabled(true);
    this->ui->searchTrustedRootCertBut->setEnabled(true);
    this->ui->searchClientCertBut->setEnabled(true);
    this->ui->serverCertDeleteBut->setEnabled(true);
    this->ui->clientCertDeleteBut->setEnabled(true);
    this->ui->trustedRootStoreCombo->setEnabled(true);
    this->ui->clientCertStoreCombo->setEnabled(true);

    newConnectionFlag = "";
    // Blank this flag so other connections won't try to display wrong message
    processingAButton = false;
    //is->update();
    //XXX ADDED THIS 10/14/2014
    //saveNetworkConfigurationProcess->kill();

    //LOG_DEBUG( "finishedNetworkProcess() END", NULL );
}

void setupUtility::finishedEAPProcess(int value, QProcess::ExitStatus exitStatus)
{
    LOG_CLASS_INFO( "setupUtility", "finishedEAPProcess" );
    //LOG_DEBUG( "finishedEAPProcess() START", NULL );

    //processingAButton = true;

    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    //////qDebug() << "finished";

    //XXX COMMENT LINE BELOW AFTER TESTING
   // this->ui->alertsLineEdit->setText("Finished Process");

    //this->ui->mainTab->setEnabled(true);
    if(mustTurnOnDialogs)
    {
       ;// session_->send(SessionDev , SnRestartDialogs);
    }
    if(eapProcessConnected)
    {
        saveEAPConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
        this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);

    }
    eapProcessConnected = false;
    if(!connectedSuccessfully)
    {
        //LOG_DEBUG( "finishedEAPProcess() Notso connected...", NULL );
        this->ui->alertsLineEdit->setText(tr("Not connected... Try again."));
        if(ui->wirelessAdapterGroupBox->isVisible())
        {
            python pythonProcess;
            QStringList myOptions;
            if (pythonProcess.execCommand("stopWireless", myOptions))
            {
                if (pythonProcess.getExitCode() == 0)
                {
                    ;//ui->alertsLineEdit->setText("GOOD");
                }
                else
                {
                    ;//ui->alertsLineEdit->setText("BAD");
                }
            }
        }
    }
    this->ui->connectingProgressBar->hide();
    this->ui->connectingProgressBar->lower();
    this->ui->connectingProgressBar->setValue(0);
    this->ui->exportBut->setEnabled(true);
    this->ui->importBut->setEnabled(true);
    this->ui->refreshNetworksBut->setEnabled(true);
    this->ui->availableConnectionsTableView->setEnabled(true);
    this->ui->networkSaveBut->setEnabled(true);
    this->ui->addNetworkBut->setEnabled(true);
    this->ui->serverCertBut->setEnabled(true);
    this->ui->clientCertBut->setEnabled(true);
    this->ui->searchTrustedRootCertBut->setEnabled(true);
    this->ui->searchClientCertBut->setEnabled(true);
    this->ui->serverCertDeleteBut->setEnabled(true);
    this->ui->clientCertDeleteBut->setEnabled(true);
    this->ui->trustedRootStoreCombo->setEnabled(true);
    this->ui->clientCertStoreCombo->setEnabled(true);

    newConnectionFlag = "";
    // Blank this flag so other connections won't try to display wrong message
    processingAButton = false;
    //is->update();
    //XXX ADDED THIS 10/14/2014
    //saveNetworkConfigurationProcess->kill();

    //LOG_DEBUG( "finishedEAPProcess() END", NULL );
}

bool setupUtility::onScreenChanging()
{
    LOG_CLASS_INFO( "setupUtility", "onScreenChanging" );
    //LOG_DEBUG( "onScreenChanging() START", NULL );

    //saveProduct();

    //if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
    //    session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    // Set internet connection to only the last one shown
    if(rebootingAlready)
    {
        return false;
    }
    else
    {
        //return false;

        if(settingTime || processingAButton) return false;
        else if(changedHostname || changedSpeakers || changedAstronomical)
        {
            rebootingAlready = true;
            if(networkTabAlreadyLoaded && mustTurnOnDialogs)
                    {
                       session_->send(SessionDev , SnRestartDialogs);
                       mustTurnOnDialogs = false;
                    }
            shouldChangeTime = false;
            QString msg = "120080,3";
            session_->send(Error, ErrVersionManagerDevice, msg);
            //this->reboot();
            return false;
        }
        else if(networkTabAlreadyLoaded)
        {
            if(mustTurnOnDialogs)
            {
               session_->send(SessionDev , SnRestartDialogs);
               //session_->send( NetworkConnectivityDev, RequestCurrentStatus );
               mustTurnOnDialogs = false;
            }

            QProcess powerNetworkDevicesProcess;
            QStringList options;
            options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));

            options.append("-C");

            if(finalIsWireless) options.append("03");
            else options.append("30");

            powerNetworkDevicesProcess.start("python", options);

            if(!powerNetworkDevicesProcess.waitForStarted())
            {
                this->ui->alertsLineEdit->setText(tr("Start configuration error."));
            }

            if(!powerNetworkDevicesProcess.waitForFinished())
            {
                ;
            }
            return true;
        }
        else
        {
            //session_->send( NetworkConnectivityDev, RequestCurrentStatus );
            return _Mybase::onScreenChanging();
        }
    }
    //LOG_DEBUG( "onScreenChanging() END", NULL );
}

void setupUtility::beginTheNetworkConnection()
{
    LOG_CLASS_INFO( "setupUtility", "beginTheNetworkConnection" );
    LOG_DEBUG( "beginTheNetworkConnection() START", NULL );

    if(ui->wpaEAPTLSPassPhraseFrame->isVisible() && (ui->trustedRootStoreCombo->currentIndex() == 0 || ui->clientCertStoreCombo->currentIndex() == 0))
    {
        //LOG_DEBUG( "beginTheNetworkConnection() IF ui->wpaEAPTLSPassPhraseFrame->isVisible()", NULL );
        ; // Do nothing because a cert was not chosen.
    }
    else
    {
        //LOG_DEBUG( "beginTheNetworkConnection() ELSE ui->wpaEAPTLSPassPhraseFrame->isVisible()", NULL );

        processingAButton = true;
        this->ui->connectingProgressBar->setValue(0);
        this->ui->connectingProgressBar->raise();
        this->ui->connectingProgressBar->show();
        this->ui->addNetworkBut->hide();

        this->update();
        updateDB("networkTab", "1");

        ui->mainTab->setTabIcon(3, passIcon);

        session_->send(SessionDev, SnStopDialogs);
        mustTurnOnDialogs = true;

        networkProcessConnected = true;
        this->ui->networkSaveBut->setEnabled(false);

        // this->ui->exportBut->setEnabled(false);
        // HW-146 Setup Utility Export disabled - leaving b\c I think we will add it back in some form
        // this->ui->importBut->setEnabled(false);

        this->ui->refreshNetworksBut->setEnabled(false);
        this->ui->availableConnectionsTableView->setEnabled(false);
        this->ui->addNetworkBut->setEnabled(false);
        this->ui->serverCertBut->setEnabled(false);
        this->ui->clientCertBut->setEnabled(false);
        this->ui->searchTrustedRootCertBut->setEnabled(false);
        this->ui->searchClientCertBut->setEnabled(false);
        this->ui->serverCertDeleteBut->setEnabled(false);
        this->ui->clientCertDeleteBut->setEnabled(false);
        this->ui->trustedRootStoreCombo->setEnabled(false);
        this->ui->clientCertStoreCombo->setEnabled(false);
        this->ui->networkAdvancedBut->setEnabled(false);

        if(newConnectionFlag == "Wire")
        {
            respondToTableViewClick = false;
        }

        //this->ui->mainTab->setDisabled(true);

        if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
        {
            this->ui->alertsLineEdit->setText(tr("Processing certificates..."));
            this->ui->connectingProgressBar->setValue(20);
            //qApp->processEvents();
            changeCertFilesToUse();
        }
        else
        {
            this->ui->alertsLineEdit->setText(tr("Trying connection..."));
            //qApp->processEvents();
        }

        finalIsWireless = ui->SSIDLineEdit->isVisible();

        //  OLD AND USABLE

        if(ui->wirelessAdapterGroupBox->isVisible())
        {
            dbConnectionType = "1";
            dbConnectionName = ui->SSIDLineEdit->text();
            updateDB("connectionType",dbConnectionType);
            updateDB("connectionName",dbConnectionName);
        }
        else
        {
            isWiredConnection_Fix = true;
            dbConnectionType = "0";
            dbConnectionName = "WiredConnection";
            updateDB("connectionType",dbConnectionType);
            updateDB("connectionName",dbConnectionName);
        }

        QStringList options = this->buildNetworkCommandOptions(commonNetwork);

        ///////////////////////////////////
        ////
        ///
        ///
        /// /*

        if(ui->eapUserPassFrame->isVisible())
        {
            // start a listener for the connection to be made
           //saveEAPConfigurationProcess->waitForStarted(100);
           QStringList eapargs;
           eapargs.clear();
           eapargs.append("r");

           //saveEAPConfigurationProcess->start("C:\Program Files\Hobart\bin\Hobart-EAPUtility.exe", eapargs);

        }

        saveNetworkConfigurationProcess->waitForStarted(100);
        //options.clear();
        //options.append("connectStartupNetwork.py");
        saveNetworkConfigurationProcess->start("python", options);
        //LOG_DEBUG( "starting Network commands '%s'", options.toLatin1().constData() );

        if(!saveNetworkConfigurationProcess->waitForFinished(60000)) // was 60000
        {
            connectionTimeout = true;

            //Try one more time because of windows
            if(!isLinux())
            {
                // tell the User the connection didn't succeed
                //ui->alertsLineEdit->setText("giving more time");
                this->update();
                QProcess retestNetworkProcess;
                QStringList options;
                options.append("-D");
                if (ui->wirelessAdapterGroupBox->isVisible())
                {
                    options.append("1");
                }
                else
                {
                    options.append("0");
                }
                options.append(filesystem::absoluteFilePath("getNetworkStatus.py",folderType::Python));

                retestNetworkProcess.start("python", options);

                if(!retestNetworkProcess.waitForStarted())
                {
                    this->ui->alertsLineEdit->setText(tr("Start configuration error."));
                }

                if(!retestNetworkProcess.waitForFinished())
                {
                    ;
                }

                QStringList networkStatuses;
                QByteArray result = retestNetworkProcess.readAll();
                QString status(result);
                status = status.mid(0,status.length()-1);

                //qDebug() << "total string is:" << macAddress;
                // added in a string length check
                if(status.size() > 0)
                {
                    networkStatuses = status.split("|", QString::SkipEmptyParts);
                    if(networkStatuses[0] == "None")
                    {
                        noConnectionPhrase = "OUTPUT|None";
                        connectedSuccessfully = false;
                    }
                    else
                    {
                        // got conenction
                        networkProcessConnected = true;
                        connectionTimeout = true;
                        noConnectionPhrase = status;
                        connectedSuccessfully = true;
                        LOG_DEBUG("YESSO CONNECTION and output is: %s", status.toLatin1().constData());
                    }
                }
            }
            else
            {
                noConnectionPhrase = "NO";
                noConnectionPhrase = "OUTPUT|None";
                connectedSuccessfully = false;
            }

            //LOG_DEBUG( "beginTheNetworkConnection() call printNetworkOutput()", NULL );
            printNetworkOutput();

            connectionTimeout = false;
            if(networkProcessConnected)
            {
                saveNetworkConfigurationProcess->kill();
                networkProcessConnected = false;
            }

#ifdef Q_OS_WIN

            if(ui->wirelessAdapterGroupBox->isVisible())
            {
                python pythonProcess;
                QStringList myOptions;
                if (pythonProcess.execCommand("stopWireless", myOptions))
                {
                    if (pythonProcess.getExitCode() == 0)
                    {
                        ;//ui->alertsLineEdit->setText("GOOD");
                    }
                    else
                    {
                        ;//ui->alertsLineEdit->setText("BAD");
                    }
                }
            }
#endif
        }

        // Start saving the configurations

        if(ui->staticIPRadio->isChecked())
        {
            //LOG_DEBUG("beginTheNetworkConnection() updateDB(isStatic,1)", NULL);
            updateDB("isStatic","1");
        }
        else
        {
            //LOG_DEBUG("beginTheNetworkConnection() updateDB(isStatic,0)", NULL);
            updateDB("isStatic","0");
        }

        if(ui->autoDNSRadio->isChecked())
        {
            updateDB("autoDNS", "1");
        }
        else
        {
            updateDB("autoDNS", "0");
        }

        UsersTrustedRoot = dbServerCertFilename;
        UsersClientCert = dbClientCertUsersFilename;
        updateDB("serverCertFilename", dbServerCertFilename);

        if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
        {
            updateDB("serverCertFilename", ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
            updateDB("chainFilename", dbChainFilename);
            updateDB("clientCertFilename", getSerialNumber());
            updateDB("CSRFilename", dbCSRFilename);
            updateDB("CSRKeyFilename", dbCSRKeyFilename);
            updateDB("clientKeyFilename", dbClientKeyFilename);
            updateDB("clientCertUsersFilename", ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
            updateDB("clientCertPassword", ui->leClientKeyPassword->text());
        }
        else
        {
            updateDB("serverCertFilename","");
            updateDB("chainFilename", "");
            updateDB("clientCertFilename", "");
            updateDB("CSRFilename", "");
            updateDB("CSRKeyFilename", "");
            updateDB("clientKeyFilename", "");
            updateDB("clientCertUsersFilename", "");
            updateDB("clientCertPassword", "");
            ui->trustedRootStoreCombo->setCurrentIndex(0);
            ui->clientCertStoreCombo->setCurrentIndex(0);
            ui->leClientKeyPassword->setText("");
        }

        loadNetworkConnections();

        if(ui->availableConnectionsTableView->currentRow() == 0)
        {
            updateDB("linkRate", "auto");
            updateDB("automaticLinkRate","1");
            updateDB("channel", "auto");
            ui->cbTransferRate->setCurrentIndex(0);
            ui->cbChannel->setCurrentIndex(0);
            ui->chkboxTransferRateAuto->setChecked(true);
            ui->networkAdvancedBut->setIcon(QIcon());
            ui->networkAdvancedBut->setStyleSheet("");
            updateDB("connectionType", "0");
            ui->availableConnectionsTableView->showRow(0);
        }

        if(ui->availableConnectionsTableView->currentRow() > 0)
        {
            updateDB("linkRate", ui->cbTransferRate->itemData(ui->cbTransferRate->currentIndex()).toString());
            if(ui->chkboxTransferRateAuto->isChecked()) updateDB("automaticLinkRate","1");
            else updateDB("automaticLinkRate","0");
            updateDB("channel",ui->cbChannel->itemData(ui->cbChannel->currentIndex()).toString());
            updateDB("connectionType", "1");
            ui->availableConnectionsTableView->showRow(ui->availableConnectionsTableView->currentRow());

        }

        updateDB("DNSSuffix1",ui->leDNSSuffix1->text());
        updateDB("DNSSuffix2",ui->leDNSSuffix2->text());
        updateDB("DNSSuffix3",ui->leDNSSuffix3->text());
        updateDB("DNSSuffix4",ui->leDNSSuffix4->text());
        updateDB("DNSSuffix5",ui->leDNSSuffix5->text());
        updateDB("DNSSuffix6",ui->leDNSSuffix6->text());

        if( ui->DHCPHostnameCheckbox->isChecked() )
        {
            //LOG_DEBUG( "beginTheNetworkConnection() IF ( ui->DHCPHostnameCheckbox->isChecked() )", NULL );
            updateDB("DHCPHostname","1");
        }
        else
        {
            //LOG_DEBUG( "beginTheNetworkConnection() ELSE ( ui->DHCPHostnameCheckbox->isChecked() )", NULL );
            updateDB("DHCPHostname","0");
        }

        saveConfigsNow();
        connectDB();

        updateDB("DNSSuffix1", ui->leDNSSuffix1->text());
        updateDB("DNSSuffix2", ui->leDNSSuffix2->text());
        updateDB("DNSSuffix3", ui->leDNSSuffix3->text());

        connectDB();

        //XXX October 20, 2014 Uncomment this if dialogs are required
        //session_->send( NetworkConnectivityDev, RequestCurrentStatus );
        //loadNetworkConnections();

        processingAButton = false;
    }

    //set the hostname fully

    this->syncHostname();
    this->getUIHostname();

    session_->send(SessionDev, SnRestartDialogs);

    //LOG_DEBUG( "beginTheNetworkConnection() END", NULL );
}

void setupUtility::saveNetConfigurationToScaleSettings()
{
    LOG_CLASS_INFO( "setupUtility", "saveNetConfigurationToScaleSettings" );
    //LOG_DEBUG( "saveNetConfigurationToScaleSettings() START", NULL );

    // This routine saves all network settings to file
    if(ui->dhcpRadio->isChecked())
    {
        //LOG_DEBUG("saveNetConfigurationToScaleSettings() updateDB(isStatic,0)", NULL);
        this->updateDB("isStatic","0");
    }
    else
    {
        //LOG_DEBUG("saveNetConfigurationToScaleSettings() updateDB(isStatic,1)", NULL);
        this->updateDB("isStatic","1");
    }

    updateDB("ipAddress",this->ui->ipAddressLineEdit->text()+"."+this->ui->ipAddressLineEdit_2->text()+"."+this->ui->ipAddressLineEdit_3->text()+ "." + this->ui->ipAddressLineEdit_4->text());
    updateDB("gateway", this->ui->gatewayLineEdit->text()+"."+this->ui->gatewayLineEdit_2->text()+"."+this->ui->gatewayLineEdit_3->text()+"."+this->ui->gatewayLineEdit_4->text());
    updateDB("netmask",this->ui->subnetMaskLineEdit->text()+"."+this->ui->subnetMaskLineEdit_2->text()+"."+this->ui->subnetMaskLineEdit_3->text()+"."+this->ui->subnetMaskLineEdit_4->text());

    if(this->ui->autoDNSRadio->isChecked()) updateDB("autoDNS","1");
    else this->updateDB("autoDNS","0");

    updateDB("dns1",ui->DNS1LineEdit->text()+"."+ui->DNS1LineEdit_2->text()+"."+ui->DNS1LineEdit_3->text()+"."+ui->DNS1LineEdit_4->text());
    updateDB("dns2",ui->DNS2LineEdit->text()+"."+ui->DNS2LineEdit_2->text()+"."+ui->DNS2LineEdit_3->text()+"."+ui->DNS2LineEdit_4->text());
    updateDB("dns3",ui->DNS3LineEdit->text()+"."+ui->DNS3LineEdit_2->text()+"."+ui->DNS3LineEdit_3->text()+"."+ui->DNS3LineEdit_4->text());

    updateDB("connectionName",ui->SSIDLineEdit->text());
    updateDB("security",QString::number(this->ui->securityCombo->currentIndex()));
    updateDB("encryption",QString::number(this->ui->encryptionCombo->currentIndex()));


        // so save keys only if it is on windows
        if( !isLinux() )
        {
            QString databaseFileName = "settingswizard/NetKeys.txt";

            if( TESTING )
                databaseFileName = "C:/hobart/settingswizard/NetKeys.txt";

            databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

            QFile file(databaseFileLocation);
            if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
            {
                processingAButton = false;
            }
            else
            {
                QStringList theLine;
                QTextStream out(&file);

                // OPEN WEP so save four keys only if it is on windows
                if(ui->securityCombo->currentIndex() == 0 && ui->encryptionCombo->currentIndex() == 0)
                {
                    out << "wep_tx_keyidx=";
                     if(ui->wirelessAuthenticationDefaultKey1Radio->isChecked()) out << "0\n";
                    if(ui->wirelessAuthenticationDefaultKey2Radio->isChecked()) out << "1\n";
                    if(ui->wirelessAuthenticationDefaultKey3Radio->isChecked()) out << "2\n";
                    if(ui->wirelessAuthenticationdefaultKey4Radio->isChecked()) out << "3\n";

                    QProcess storeKeysProcess;
                    QStringList options;
                    options.clear();

                    out << "wep_key0=" + ui->wepKey1LineEdit->text() + "\n";
                    /*if(ui->wepKey1LineEdit->text().length() > 0)
                    {
                        options.append(filesystem::absoluteFilePath("encryptDecryptText.py",folderType::Python));
                        options.append("-T");
                        options.append("E");
                        options.append("-P");
                        options.append(ui->wepKey1LineEdit->text());
                        options.append("-Z");
                        options.append(PASSPHRASE);

                        storeKeysProcess.start("python", options);

                        if(!storeKeysProcess.waitForStarted())
                        {
                            ;
                        }

                        if(!storeKeysProcess.waitForFinished())
                        {
                            ;
                        }

                        QByteArray result = storeKeysProcess.readAll();
                        QString encryptedText(result);
                        encryptedText = encryptedText.mid(0,encryptedText.length()-1);

                        //qDebug() << "total string is:" << macAddress;
                        // added in a string length check
                        if(encryptedText.size() > 0)
                        {
                            out << encryptedText;
                        }
                    }
                    out << "\n";



                    options.clear();
                    */
                    out << "wep_key1=" + ui->wepKey2LineEdit->text() + "\n";
                    /*if (ui->wepKey2LineEdit->text().length() > 0)
                    {
                        options.append(filesystem::absoluteFilePath("encryptDecryptText.py",folderType::Python));
                        options.append("-T");
                        options.append("E");
                        options.append("-P");
                        options.append(ui->wepKey2LineEdit->text());
                        options.append("-Z");
                        options.append(PASSPHRASE);

                        storeKeysProcess.start("python", options);

                        if(!storeKeysProcess.waitForStarted())
                        {
                            ;
                        }

                        if(!storeKeysProcess.waitForFinished())
                        {
                            ;
                        }

                        QByteArray result = storeKeysProcess.readAll();
                        QString encryptedText(result);
                        encryptedText = encryptedText.mid(0,encryptedText.length()-1);

                        //qDebug() << "total string is:" << macAddress;
                        // added in a string length check
                        if(encryptedText.size() > 0)
                        {
                            out << encryptedText;
                        }

                    }
                    out << "\n";


                    options.clear();
                    */
                    out << "wep_key2=" + ui->wepKey3LineEdit->text() + "\n";
                    /*if (ui->wepKey3LineEdit->text().length() > 0)
                    {
                        options.append(filesystem::absoluteFilePath("encryptDecryptText.py",folderType::Python));
                        options.append("-T");
                        options.append("E");
                        options.append("-P");
                        options.append(ui->wepKey3LineEdit->text());
                        options.append("-Z");
                        options.append(PASSPHRASE);

                        storeKeysProcess.start("python", options);

                        if(!storeKeysProcess.waitForStarted())
                        {
                            ;
                        }

                        if(!storeKeysProcess.waitForFinished())
                        {
                            ;
                        }

                        QByteArray result = storeKeysProcess.readAll();
                        QString encryptedText(result);
                        encryptedText = encryptedText.mid(0,encryptedText.length()-1);

                        //qDebug() << "total string is:" << macAddress;
                        // added in a string length check
                        if(encryptedText.size() > 0)
                        {
                            out << encryptedText;
                        }
                    }
                    out << "\n";

                    options.clear();

                    */
                    out << "wep_key3=" + ui->wepKey4LineEdit->text() + "\n";
                    /*if (ui->wepKey4LineEdit->text().length() > 0)
                    {
                        options.append(filesystem::absoluteFilePath("encryptDecryptText.py",folderType::Python));
                        options.append("-T");
                        options.append("E");
                        options.append("-P");
                        options.append(ui->wepKey4LineEdit->text());
                        options.append("-Z");
                        options.append(PASSPHRASE);

                        storeKeysProcess.start("python", options);

                        if(!storeKeysProcess.waitForStarted())
                        {
                            ;
                        }

                        if(!storeKeysProcess.waitForFinished())
                        {
                            ;
                        }

                        QByteArray result = storeKeysProcess.readAll();
                        QString encryptedText(result);
                        encryptedText = encryptedText.mid(0,encryptedText.length()-1);

                        //qDebug() << "total string is:" << macAddress;
                        // added in a string length check
                        if(encryptedText.size() > 0)
                        {
                            out << encryptedText;
                        }

                    }
                    out << "\n";
                    */
                }
                /*else if (ui->securityCombo->currentIndex() == 1 || ui->securityCombo->currentIndex() == 5)
                {
                    //WPA types
                    QProcess storeKeysProcess;
                    QStringList options;
                    options.clear();
                    options.append(filesystem::absoluteFilePath("encryptDecryptText.py",folderType::Python));
                    options.append("-T");
                    options.append("E");
                    options.append("-P");
                    options.append(ui->wepKey4LineEdit->text());
                    options.append("-Z");
                    options.append(PASSPHRASE);

                    storeKeysProcess.start("python", options);

                    if(!storeKeysProcess.waitForStarted())
                    {
                        ;
                    }

                    if(!storeKeysProcess.waitForFinished())
                    {
                        ;
                    }

                    QByteArray result = storeKeysProcess.readAll();
                    QString encryptedText(result);
                    encryptedText = encryptedText.mid(0,encryptedText.length()-1);

                    //qDebug() << "total string is:" << macAddress;
                    // added in a string length check
                    if(encryptedText.size() > 0)
                    {
                        out << encryptedText;
                    }
                    out << "\n";

                }
                 */
            file.close();
            // Add the encrypted keys to NetKes.txt for windows.

            }
            //processingAButton = false
        }
    ;
    //LOG_DEBUG( "saveNetConfigurationToScaleSettings() END", NULL );
}

void setupUtility::gatewayYesSlot()
{
    LOG_CLASS_INFO( "setupUtility", "gatewayYesSlot" );
    //LOG_DEBUG( "gatewayYesSlot() START", NULL );

    this->ui->gbNetworkLoading->hide();
    qApp->processEvents();
    this->update();
    beginTheNetworkConnection();
    //LOG_DEBUG( "gatewayYesSlot() END", NULL );
}

void setupUtility::gatewayNoSlot()
{
    LOG_CLASS_INFO( "setupUtility", "gatewayNoSlot" );
    //LOG_DEBUG( "gatewayNoSlot() START", NULL );

    this->ui->gbNetworkLoading->hide();

    //LOG_DEBUG( "gatewayNoSlot() END", NULL );
}

bool setupUtility::killStartupNetworking()
{
    LOG_CLASS_INFO( "setupUtility", "killStartupNetworking" );
    //LOG_DEBUG( "killStartupNetworking() START", NULL );

    this->ui->gbNetworkLoading->hide();

    python pythonProcess;
    QStringList options;
    options.clear();

    if (pythonProcess.execCommand("killStartupNetworking", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
             return true;                   //TBD - updateCustomerVolume();
        }
        else
        {
            return false;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        return false;
        // TODO:
        // What do we do here?
    }
    //processingAButton = false;

    //LOG_DEBUG( "killStartupNetworking() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     S A V E   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void setupUtility::saveNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "saveNetworkBut_click" );
    LOG_DEBUG( "saveNetworkBut_click() START", NULL );
    //LOG_DEBUG( "saveNetworkBut_click() hostnameStayChecked_FIX reset to FALSE", NULL );

    isDHCPCheckedBoxd_Fix   = false;
    isHostNameChecked_Fix   = false;
    isWiredConnection_Fix   = false;
    hostnameStayChecked_FIX = false;

    // first, let's change the /etc/dhcp.conf file to rerquest or ignore the hostname
    QString hostnameFromDHCP = "0";

    if(ui->DHCPHostnameCheckbox->isChecked())
    {
        //for this to be checked we must be using dhcpcd
        LOG_DEBUG( "saveNetworkBut_click() ui->DHCPHostnameCheckbox->isChecked() is TRUE", NULL );
        hostnameFromDHCP = "1";
        isHostNameChecked_Fix = true;
    }
    else
    {
        LOG_DEBUG( "saveNetworkBut_click() ui->DHCPHostnameCheckbox->isChecked() is FALSE", NULL );
        hostnameFromDHCP = "0";
    }

    if(ui->dhcpRadio->isChecked())
    {
        LOG_DEBUG( "saveNetworkBut_click() ui->dhcpRadio->isChecked() is TRUE", NULL );
        isDHCPCheckedBoxd_Fix = true;
        python pythonProcess;
        QStringList options;
        options.clear();
        options.append("-H");
        options.append(hostnameFromDHCP);

        if (pythonProcess.execCommand("changeDHCPConf", options))
        {
            LOG_DEBUG( "saveNetworkBut_click() IF pythonProcess.execCommand", NULL );

            if (pythonProcess.getExitCode() == 0)
            {
                LOG_DEBUG( "saveNetworkBut_click() IF getExitCode() == 0", NULL );
                ;                   //TBD ;
            }
            else
            {
                LOG_DEBUG( "saveNetworkBut_click() ELSE getExitCode() != 0", NULL );
                ;
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            LOG_DEBUG( "saveNetworkBut_click() ELSE pythonProcess.execCommand", NULL );
            ;
            // TODO:
            // What do we do here?
        }
    }

    // we have to suppress the lost connection dialog
    if(ui->gatewayLineEdit->text() == "" && ui->gatewayLineEdit_2->text() == "" && ui->gatewayLineEdit_3->text() == "" && ui->gatewayLineEdit_4->text() == "" && ui->staticIPRadio->isChecked())
    {
        LOG_DEBUG( "saveNetworkBut_click() IF suppress the lost connection dialog", NULL );

        //popup blank gateway notice
        QFont tempFont = ui->networkLoadingLabel->font();
        tempFont.setPointSize(12);
        ui->networkLoadingLabel->setFont(tempFont);
        ui->networkLoadingLabel->setText(tr("You have selected a static ip configuration\nwithout entering a default gateway. In order\nto show connected in the Setup Utility, you\nmust enter a valid IP address in that field.\n\nDo you want to continue?"));

        ui->newWiredNetworkBut->lower();
        ui->newWirelessNetworktBut->lower();
        ui->newWiredNetworkBut->hide();
        ui->newWirelessNetworktBut->hide();

        ui->noGatewayBut->raise();
        ui->yesGatewayBut->raise();
        ui->noGatewayBut->show();
        ui->yesGatewayBut->show();
        ui->gbNetworkLoading->show();

    }
    else
    {
        LOG_DEBUG( "saveNetworkBut_click() ELSE suppress the lost connection dialog", NULL );

        this->ui->connectingProgressBar->setValue(0);

        killStartupNetworking();

        this->update();

        beginTheNetworkConnection();
    }

    // SOF-2760
    this->ui->addNetworkBut->show();

    LOG_DEBUG( "saveNetworkBut_click() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   S A V E   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList setupUtility::buildSaveConfigurationOptions(Network *locNetwork)
{
    LOG_CLASS_INFO( "setupUtility", "buildSaveConfigurationOptions" );
    LOG_DEBUG( "buildSaveConfigurationOptions() START", NULL );

    //processingAButton = true;
    QStringList options;
    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    //options.append("python/qmSettings/network/saveConfigs.py");
    options.append(filesystem::absoluteFilePath("saveConfigs.py",folderType::Python));

    options.append("-S"); // SSID name
    options.append(locNetwork->SSID());

    options.append("-M"); // security method
    options.append(locNetwork->SecurityMethod());

    options.append("-E"); // encryption algorithm
    options.append(locNetwork->Encryption());

    if(locNetwork->SecurityMethod() == "WPA-Personal" || locNetwork->SecurityMethod() == "WPA2-Personal") {
        options.append("-A");
        options.append(locNetwork->WPAPassPhrase());
    }
    else if(locNetwork->SecurityMethod() == "OPEN") {

        options.append("-K"); //default key
        if(this->ui->wirelessAuthenticationDefaultKey1Radio->isChecked())
        {
            options.append("1");
        }
        else if(this->ui->wirelessAuthenticationDefaultKey2Radio->isChecked())
        {
            options.append("2");
        }
        else if(this->ui->wirelessAuthenticationDefaultKey3Radio->isChecked())
        {
            options.append("3");
        }
        else if(this->ui->wirelessAuthenticationdefaultKey4Radio->isChecked())
        {
            options.append("4");
        }

        options.append("-w");
        options.append(locNetwork->WEPKey1());

        options.append("-x");
        options.append(locNetwork->WEPKey2());

        options.append("-y");
        options.append(locNetwork->WEPKey3());

        options.append("-z");
        options.append(locNetwork->WEPKey4());
    }
    else if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
    {
        options.append("-a");
        options.append(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
        options.append("-b");
        options.append(getSerialNumber());
        options.append("-c");
        options.append(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
        options.append("-d");
        options.append(ui->leClientKeyPassword->text());
    }
    else {
        options.append("-N");
        options.append(locNetwork->EAPUsername());

        options.append("-n");
        options.append(locNetwork->EAPPassword());
    }

    options.append("-s");
    if(this->ui->staticIPRadio->isChecked())
        options.append("1");
    else options.append("0");

    options.append("-I");
    // 64641964 is a sentinel showing the entry is blank
    if(ui->ipAddressLineEdit->text() == "" || ui->ipAddressLineEdit_2->text() == "" || ui->ipAddressLineEdit_3->text() == "" || ui->ipAddressLineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->ipAddressLineEdit->text() + "." + ui->ipAddressLineEdit_2->text() + "." + ui->ipAddressLineEdit_3->text() + "." + ui->ipAddressLineEdit_4->text());

    options.append("-B");
    if(ui->subnetMaskLineEdit->text() == "" || ui->subnetMaskLineEdit_2->text() == "" || ui->subnetMaskLineEdit_3->text() == "" || ui->subnetMaskLineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->subnetMaskLineEdit->text() + "." + ui->subnetMaskLineEdit_2->text() + "." + ui->subnetMaskLineEdit_3->text() + "." + ui->subnetMaskLineEdit_4->text());

    options.append("-G");
    if(ui->gatewayLineEdit->text() == "" || ui->gatewayLineEdit_2->text() == "" || ui->gatewayLineEdit_3->text() == "" || ui->gatewayLineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->gatewayLineEdit->text() + "." + ui->gatewayLineEdit_2->text() + "." + ui->gatewayLineEdit_3->text() + "." + ui->gatewayLineEdit_4->text());

    options.append("-D");
    if(this->ui->autoDNSRadio->isChecked())
        options.append("1");
    else options.append("0");

    options.append("-1");
    if(ui->DNS1LineEdit->text() == "" || ui->DNS1LineEdit_2->text() == "" || ui->DNS1LineEdit_3->text() == "" || ui->DNS1LineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->DNS1LineEdit->text() + "." + ui->DNS1LineEdit_2->text() + "." + ui->DNS1LineEdit_3->text() + "." + ui->DNS1LineEdit_4->text());

    options.append("-2");
    if(ui->DNS2LineEdit->text() == "" || ui->DNS2LineEdit_2->text() == "" || ui->DNS2LineEdit_3->text() == "" || ui->DNS2LineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->DNS2LineEdit->text() + "." + ui->DNS2LineEdit_2->text() + "." + ui->DNS2LineEdit_3->text() + "." + ui->DNS2LineEdit_4->text());

    options.append("-3");
    if(ui->DNS3LineEdit->text() == "" || ui->DNS3LineEdit_2->text() == "" || ui->DNS3LineEdit_3->text() == "" || ui->DNS3LineEdit_4->text() == "") options.append("64641964");
    else options.append(ui->DNS3LineEdit->text() + "." + ui->DNS3LineEdit_2->text() + "." + ui->DNS3LineEdit_3->text() + "." + ui->DNS3LineEdit_4->text());

    if(ui->cbTransferRate->currentIndex() < 1)
    {
        options.append("-e");
        options.append("auto");
        options.append("-f");
        options.append("1");
    }
    else
    {
        options.append("-e");
        options.append(ui->cbTransferRate->itemData(ui->cbTransferRate->currentIndex()).toString());
        options.append("-f");
        if(ui->chkboxTransferRateAuto->isChecked())
        {
            options.append("1");
        }
        else
        {
            options.append("0");
        }
    }

    if(ui->cbChannel->currentIndex() < 1)
    {
        options.append("-g");
        options.append("auto");
    }
    else
    {
        options.append("-g");
        options.append(ui->cbChannel->itemData(ui->cbChannel->currentIndex()).toString());
    }

    ;

    if(ui->leDNSSuffix1->text() != "")
    {
        options.append("-5");
        options.append(ui->leDNSSuffix1->text());
    }
    else
    {
        options.append("-5");
        options.append("");
    }

    if(ui->leDNSSuffix2->text() != "")
    {
        options.append("-6");
        options.append(ui->leDNSSuffix2->text());
    }
    else
    {
        options.append("-6");
        options.append("");
    }

    if(ui->leDNSSuffix3->text() != "")
    {
        options.append("-7");
        options.append(ui->leDNSSuffix3->text());
    }
    else
    {
        options.append("-7");
        options.append("");
    }

    if(ui->leDNSSuffix4->text() != "")
    {
        options.append("-8");
        options.append(ui->leDNSSuffix4->text());
    }
    else
    {
        options.append("-8");
        options.append("");
    }
    if(ui->leDNSSuffix5->text() != "")
    {
        options.append("-9");
        options.append(ui->leDNSSuffix5->text());
    }
    else
    {
        options.append("-9");
        options.append("");
    }
    if(ui->leDNSSuffix6->text() != "")
    {
        options.append("-4");
        options.append(ui->leDNSSuffix6->text());
    }
    else
    {
        options.append("-4");
        options.append("");
    }


    options.append("-Z");

    //PASSPHRASE DEFINED HERE
    options.append(this->PASSPHRASE);
    //options.append("-L");
    //options.append("");


    // Add hostname and time settings
    options.append("--hostname");
  //  options.append("samply");
    if(exportHostnameSettings)
        {
        options.append(ui->hostnameLineEdit->text() );
        //ui->alertsLineEdit->setText("has hostname");
        }
    else {
            options.append("-1");
         //ui->alertsLineEdit->setText("no hostname");
       }

    options.append("--timeKeeper");

    if(ui->timeTabWidget->currentIndex() == 0 && !exportNTPRemoveSettings)
        options.append("Manual");
    else if(exportNTPSettings) options.append("NTP");
    else if(exportNTPRemoveSettings) options.append("REMOVE");
    else options.append("DONOTHING");
    options.append("--ntpServer");
    options.append(ui->ntpServer->text());
    options.append("--ntpFrequency");
    options.append(QString::number(ui->ntpFrequencyCombo->currentIndex()));
    options.append("--ntpDayOfWeek");
    options.append(QString::number(ui->ntpDayOfWeekCombo->currentIndex()));
    options.append("--ntpTimeOfDay");

    //options.append("-H"); // hour
    QString timeOfDay = "";
    //timeOfDay = QString::number(this->ui->timeEdit->time().hour()) + QString::number(ui->timeEdit->time().minute()) + QString::number(ui->timeEdit->time().second());
    timeOfDay = ui->ntpHourBut->text() + ":" + ui->ntpMinuteBut->text() + ":" + ui->ntpSecondBut->text();
    options.append(timeOfDay);
    options.append("--ntpTimeZone");
    if(ui->ntpTimeZoneCombo->currentIndex() < 0)
    {
        options.append("America/New_York");
    }
    else
    {
        options.append(ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString());
    }
    options.append("--ntpAntiMeridian");
    options.append(ui->ntpAmpmBut->text());
    options.append("--dhcpHostname");

    if(ui->DHCPHostnameCheckbox->isChecked())
        options.append("1");
    else
        options.append("0");

    for (QStringList::iterator it = options.begin();
         it != options.end(); ++it)
    {
        QString current = *it;
        qDebug() << "[[" << current << "]]";
        LOG_DEBUG( "options() %s", current.toLatin1().constData() );
    }

    LOG_DEBUG( "buildSaveConfigurationOptions() END", NULL );

    return options;
}

QString setupUtility::getSerialNumber()
{
    QString serialNumberAnswer = "";
    LOG_CLASS_INFO( "setupUtility", "getSerialNumber" );

    QStringList args;

    python pythonProcess;

	args.append("--mfg");

    if ( pythonProcess.execCommand("getSerialFlashInfo", args, false, false, ( 30 * 1000 ) ) )
    {
        if ( pythonProcess.getExitCode() == 0 )
        {
            LOG_DEBUG( "getSerialFlashInfo successful", NULL );

            QString result = pythonProcess.readStdOut();
            QStringList flashInfo = result.split( "," );
            QString serialNumber = flashInfo[ 1 ];

            if( !(serialNumber.isEmpty() ) )
                serialNumberAnswer = serialNumber;
        }
        else
        {
            LOG_DEBUG( "Cannot get Flash Info. Exit code = %d", pythonProcess.getExitCode() );
        }
    }

    return serialNumberAnswer;
}


void setupUtility::importNetworkSettings()
{
    LOG_CLASS_INFO( "setupUtility", "importNetworkSettings" );
    //LOG_DEBUG( "importNetworkSettings() START", NULL );

    int advancedNetworkCount = 0;
    bool alreadyAddedDNSSuffixCount = false;
    bool mustAddTrustedRootCert = false;
    bool mustAddClientCert = false;
    bool isWireless = false;

    QString cipherText = "";
    QProcess importConfigs;
    QString security = "";
    QString encryption = "";

    QString trustedRootCertFilename = "";
    QString clientCertFilename = "";
    QString clientCertPassword = "";

    QStringList myOptions = this->buildImportConfigurationOptions(commonNetwork);
    if(myOptions.isEmpty()) {
        return;
    }
    importConfigs.start("python", myOptions);

    if(!importConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
    }

    if(!importConfigs.waitForFinished())
    {
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
    }

    QByteArray result = importConfigs.readAll();
    QString configs(result);
    QStringList inList = configs.split(QRegExp("[\r\n]"),configs.SkipEmptyParts);

    if(inList.at(0) == "NO")
    {
        ui->alertsLineEdit->setText(tr("Import file format error."));
    }
    QStringList resultLine;

    for (int i = 0; i < inList.size(); i++)
    {
        resultLine.clear();
        LOG_DEBUG( "AAA", NULL );

        resultLine = inList[i].split(" : ",QString::KeepEmptyParts);
        QString key = resultLine[0].replace(" ","");
        LOG_DEBUG( "BBB", NULL );
        QString value = "";
        if(resultLine.size() > 1)
        {
            value = resultLine[1];
            if (value == "64641964") value = "";
        }
        LOG_DEBUG( "CCC", NULL );
        LOG_DEBUG( "key is %s", qPrintable( key ) );
        LOG_DEBUG( "value is %s", qPrintable( value ) );

        if(key == "AUTOMATICLINKRATE")
        {
            if(value == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else if (value == "0")
            {
                ui->chkboxTransferRateAuto->setChecked(false);
                advancedNetworkCount += 1;
            }
            LOG_DEBUG( "AUTOMATICLINKRATE", NULL );

        }
        else if(key == "LINKRATE")
        {
            if(value.length() > 0)
            {
                ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(value));
            }
            else
            {
                ui->cbTransferRate->setCurrentIndex(0);
            }
            if (ui->cbTransferRate->currentIndex() > 0)
            {
                advancedNetworkCount += 1;
            }
            LOG_DEBUG( "LINKRATE", NULL );

        }
        else if(key == "CHANNEL")
        {
            if(value.length() > 0)
            {
                ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(value));
            }
            else
            {
                ui->cbChannel->setCurrentIndex(0);
            }
            if(ui->cbChannel->currentIndex() > 0)
            {
                advancedNetworkCount += 1;
            }
            LOG_DEBUG( "CHANNEL", NULL );

        }
        else if(key == "DNSSUFFIX1")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix1->setText(value);
            }
            else
            {
                ui->leDNSSuffix1->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix1->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX1", NULL );

        }
        else if(key == "DNSSUFFIX2")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix2->setText(value);
            }
            else
            {
                ui->leDNSSuffix2->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix2->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX2", NULL );
        }
        else if(key == "DNSSUFFIX3")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix3->setText(value);
            }
            else
            {
                ui->leDNSSuffix3->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix3->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX3", NULL );

        }
        else if(key == "DNSSUFFIX4")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix4->setText(value);
            }
            else
            {
                ui->leDNSSuffix4->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix4->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX4", NULL );

        }
        else if(key == "DNSSUFFIX5")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix5->setText(value);
            }
            else
            {
                ui->leDNSSuffix5->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix5->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX5", NULL );

        }
        else if(key == "DNSSUFFIX6")
        {
            if(value.length() > 0)
            {
                ui->leDNSSuffix6->setText(value);
            }
            else
            {
                ui->leDNSSuffix6->setText(value);
            }
            if(!alreadyAddedDNSSuffixCount && ui->leDNSSuffix6->text() != "")
            {
                advancedNetworkCount += 1;
                alreadyAddedDNSSuffixCount = true;
            }
            LOG_DEBUG( "DNSSUFFIX6", NULL );

        }
        else if(key == "SSID")
        {
            if(value.length() > 0)
            {
                ui->SSIDLineEdit->setText(value);
                isWireless = true;
            }
            else ui->SSIDLineEdit->setText("");
            LOG_DEBUG( "SSID", NULL );

        }
        else if(key == "AUTHENTICATION")
        {
             if(value.length() > 0)
             {
                 ui->securityCombo->setCurrentIndex(ui->securityCombo->findText(value));
                 security = value;

             }

             /*else ui->securityCombo->setCurrentIndex(0);

             if(value.contains("EAP-TLS"))
             {
                 setWirelessNetworkEncryptionCombo(2);
             }
             else if(value.contains("Personal"))
             {
                 setWirelessNetworkEncryptionCombo(1);
             }
             else if(value.contains("Enterprise"))
             {
                 setWirelessNetworkEncryptionCombo(3);
             }
             else if(value.contains("802.1X"))
             {
                 setWirelessNetworkEncryptionCombo(9);
             }
             else if(value.contains("OPEN") || value.contains("Open"))
             {
                 setWirelessNetworkEncryptionCombo(0);
             }
             */
             LOG_DEBUG( "SECURITY", NULL );

        }
        else if(key == "CIPHER")
        {
            if(value.length() > 0)
            {
                //ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findText(value));
                cipherText = value;
                encryption = value;
            }
            else ui->encryptionCombo->setCurrentIndex(0);
            LOG_DEBUG( "ENCRYPTION", NULL );

        }
        else if(key == "DHCPHOSTNAME")
        {
            if(value.length() > 0)
            {
                if(value == "1")
                {
                    ui->DHCPHostnameCheckbox->setEnabled(true);
                    LOG_DEBUG( "importNetworkSettings() DHCPHostnameCheckbox->setChecked(true) 1", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(true);
                    ui->setHostnameBut->setEnabled(false);
                    ui->DHCPHostnameWarning->show();
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(false)", NULL );
                    ui->hostnameLineEdit->setEnabled(false);
                }
                else
                {
                    LOG_DEBUG( "importNetworkSettings() DHCPHostnameCheckbox->setChecked(false) 2", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);

                    ui->DHCPHostnameWarning->hide();
                    ui->setHostnameBut->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true)", NULL );
                    ui->hostnameLineEdit->setEnabled(true);
                }
            }
        }
        else if(key == "DEFAULT")
        {
            if(value.length() > 0)
            {
               if(value == "1")
               {
                   ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
               }
               if(value == "2")
               {
                   ui->wirelessAuthenticationDefaultKey2Radio->setChecked(true);
               }
               if(value == "3")
               {
                   ui->wirelessAuthenticationDefaultKey3Radio->setChecked(true);
               }
               if(value == "4")
               {
                   ui->wirelessAuthenticationdefaultKey4Radio->setChecked(true);
               }
            }
            LOG_DEBUG( "DEFAULT", NULL );

        }
        else if(key == "KEY1")
        {
            if(value.length() > 0) ui->wepKey1LineEdit->setText(value);
            else ui->wepKey1LineEdit->setText("");
            LOG_DEBUG( "KEY1", NULL );

        }
        else if(key == "KEY2")
        {
            if(value.length() > 0) ui->wepKey2LineEdit->setText(value);
            else ui->wepKey2LineEdit->setText("");
            LOG_DEBUG( "KEY2", NULL );
        }
        else if(key == "KEY3")
        {
            if(value.length() > 0) ui->wepKey3LineEdit->setText(value);
            else ui->wepKey3LineEdit->setText("");
            LOG_DEBUG( "KEY3", NULL );

        }
        else if(key == "KEY4")
        {
            if(value.length() > 0) ui->wepKey4LineEdit->setText(value);
            else ui->wepKey4LineEdit->setText("");
            LOG_DEBUG( "KEY4", NULL );

        }
        else if(key == "ISSTATIC")
        {
            if(value.length() > 0)
            {
                if( value == "1")
                {
                    ui->dhcpRadio->setChecked(false);
                    ui->staticIPRadio->setChecked(true);

                    LOG_DEBUG( "importNetworkSettings() DHCPHostnameCheckbox->setChecked(false) 3", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);
                    ui->DHCPHostnameCheckbox->setEnabled(false);
                    ui->setHostnameBut->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true) 2", NULL );
                    ui->hostnameLineEdit->setEnabled(true);
                    ui->DHCPHostnameWarning->hide();
                }
                else if(value == "0")
                {
                    LOG_DEBUG( "importNetworkSettings() DHCPHostnameCheckbox->setChecked(false) 4", NULL );
                    ui->staticIPRadio->setChecked(false);
                    ui->dhcpRadio->setChecked(true);
                    ui->DHCPHostnameCheckbox->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(false) 2", NULL );
                    // I commnted this because I am going to let dhcphostname handle this
                    //ui->hostnameLineEdit->setEnabled(false);
                }
             }
            else ui->staticIPRadio->setChecked(true);
            LOG_DEBUG( "ISSTATIC", NULL );

        }
        else if(key == "IPADDRESS")
        {
            // SOF-1828 HTi/s - IP address is Imported on a Network Import
            // Description: IP address is displayed once an import of network settings is done.
            // IP Address should not populate on an import of network settings.
            // I thought about clearing these fields completely, but a kiosk or setup box may want them
            /*
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->ipAddressLineEdit->setText(IP[0]);
                ui->ipAddressLineEdit_2->setText(IP[1]);
                ui->ipAddressLineEdit_3->setText(IP[2]);
                ui->ipAddressLineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->ipAddressLineEdit->setText("");
                ui->ipAddressLineEdit_2->setText("");
                ui->ipAddressLineEdit_3->setText("");
                ui->ipAddressLineEdit_4->setText("");
            }
            */
            LOG_DEBUG( "IPADDRESS No longer imported", NULL );
        }
        else if(key == "SUBNET")
        {
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->subnetMaskLineEdit->setText(IP[0]);
                ui->subnetMaskLineEdit_2->setText(IP[1]);
                ui->subnetMaskLineEdit_3->setText(IP[2]);
                ui->subnetMaskLineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->subnetMaskLineEdit->setText("");
                ui->subnetMaskLineEdit_2->setText("");
                ui->subnetMaskLineEdit_3->setText("");
                ui->subnetMaskLineEdit_4->setText("");
            }
            LOG_DEBUG( "SUBNET", NULL );

        }
        else if(key == "GATEWAY")
        {
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->gatewayLineEdit->setText(IP[0]);
                ui->gatewayLineEdit_2->setText(IP[1]);
                ui->gatewayLineEdit_3->setText(IP[2]);
                ui->gatewayLineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->gatewayLineEdit->setText("");
                ui->gatewayLineEdit_2->setText("");
                ui->gatewayLineEdit_3->setText("");
                ui->gatewayLineEdit_4->setText("");
            }
            LOG_DEBUG( "GATEWAY", NULL );

        }
        else if(key == "AUTODNS")
        {
            if(value.length() > 0)
            {
                if( value == "0")
                {
                    ui->autoDNSRadio->setChecked(false);
                    ui->manualDNSRadio->setChecked(true);
                }
                else if( value == "1")
                {
                    ui->manualDNSRadio->setChecked(false);
                    ui->autoDNSRadio->setChecked(true);
                }
            }
            else
            {
                ui->autoDNSRadio->setChecked(false);
                ui->manualDNSRadio->setChecked(true);
            }
            LOG_DEBUG( "AUTODNS", NULL );

        }
        else if(key == "PRIMARY")
        {
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->DNS1LineEdit->setText(IP[0]);
                ui->DNS1LineEdit_2->setText(IP[1]);
                ui->DNS1LineEdit_3->setText(IP[2]);
                ui->DNS1LineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->DNS1LineEdit->setText("");
                ui->DNS1LineEdit_2->setText("");
                ui->DNS1LineEdit_3->setText("");
                ui->DNS1LineEdit_4->setText("");
            }
            LOG_DEBUG( "PRIMARY", NULL );

        }
        else if(key == "SECONDARY")
        {
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->DNS2LineEdit->setText(IP[0]);
                ui->DNS2LineEdit_2->setText(IP[1]);
                ui->DNS2LineEdit_3->setText(IP[2]);
                ui->DNS2LineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->DNS2LineEdit->setText("");
                ui->DNS2LineEdit_2->setText("");
                ui->DNS2LineEdit_3->setText("");
                ui->DNS2LineEdit_4->setText("");
            }
            LOG_DEBUG( "SECONDARY", NULL );

        }
        else if(key == "TERTIARY")
        {
            if(value.length() > 0)
            {
                QStringList IP = value.split(".");
                ui->DNS3LineEdit->setText(IP[0]);
                ui->DNS3LineEdit_2->setText(IP[1]);
                ui->DNS3LineEdit_3->setText(IP[2]);
                ui->DNS3LineEdit_4->setText(IP[3]);
            }
            else
            {
                ui->DNS3LineEdit->setText("");
                ui->DNS3LineEdit_2->setText("");
                ui->DNS3LineEdit_3->setText("");
                ui->DNS3LineEdit_4->setText("");
            }
            LOG_DEBUG( "TERTIARY", NULL );

        }
        else if(key == "PASSPHRASE")
        {
            if(value.length() > 0)
            {
                ui->wpaPassphraseLineEdit->setText(value);
            }
            else
            {
                ui->wpaPassphraseLineEdit->setText("");
            }
            LOG_DEBUG( "PASSPHRASE", NULL );

        }
        else if(key == "TRUSTEDROOTFILENAME")
        {
            if(value.length() > 0)
            {
                trustedRootCertFilename = value;
                ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(value));
                if(ui->trustedRootStoreCombo->currentIndex() < 1)
                {
                    mustAddTrustedRootCert = true;
                }
                else
                {
                    mustAddTrustedRootCert = false;
                    ui->trustedRootStoreCombo->setCurrentIndex(0);
                }
            }
            else
            {
                ui->trustedRootStoreCombo->setCurrentIndex(0);
                mustAddTrustedRootCert = false;
            }
            LOG_DEBUG( "TRUSTEDROOTFILENAME", NULL );

        }
        else if(key == "CLIENTCERTFILENAME")
        {
            if(value.length() > 0)
            {
                clientCertFilename = value;
                ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(value));
                if(ui->clientCertStoreCombo->currentIndex() < 1)
                {
                    mustAddClientCert = true;
                }
                else
                {
                    mustAddClientCert = false;
                    ui->clientCertStoreCombo->setCurrentIndex(0);
                }
            }
            else
            {
                mustAddClientCert = false;
                ui->clientCertStoreCombo->setCurrentIndex(0);
            }
            LOG_DEBUG( "CLIENTCERTFILENAME", NULL );

        }
        else if(key == "CLIENTCERTPASSWORD")
        {
            if(value.length() > 0)
            {
                clientCertPassword = value;

                ui->leClientKeyPassword->setText(value);
            }
            else
            {
                ui->leClientKeyPassword->setText("");
            }
            LOG_DEBUG( "CLIENTCERTPASSWORD", NULL );

        }
        else if(key == "USERNAME")
        {
            if(value.length() > 0)
            {
                ui->eapUsernameLineEdit->setText(value);
            }
            else
            {
                ui->eapUsernameLineEdit->setText("");
            }
            LOG_DEBUG( "USERNAME", NULL );

        }
        else if(key == "PASSWORD")
        {
            if(value.length() > 0)
            {
                ui->eapPasswordLineEdit->setText(value);
            }
            else
            {
                ui->eapPasswordLineEdit->setText("");
            }
            LOG_DEBUG( "PASSWORD", NULL );

        }

    }

    //CLEANUP THE GUI NOW
    if(advancedNetworkCount > 0)
    {
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(advancedNetworkCount) + ".png"));
    }
    else
    {
        ui->networkAdvancedBut->setIcon(QIcon());
        ui->networkAdvancedBut->setStyleSheet("");
    }

    if(isWireless) revealNetworkGroupBoxPrefilled(3);
    else revealNetworkGroupBoxPrefilled(1);

    ui->securityCombo->setCurrentIndex(ui->securityCombo->findText(security));
    this->setWirelessNetworkEncryptionCombo(ui->securityCombo->findText(security));
    LOG_DEBUG( "EEE", NULL );

    ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findText(encryption));
    LOG_DEBUG( "FFF", NULL );

    //if(cipherText != "")
    //{
    //    ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findText(cipherText));
    //}

    if(ui->autoDNSRadio->isChecked())
    {
        ui->DNS1LineEdit->setText("");
        ui->DNS1LineEdit_2->setText("");
        ui->DNS1LineEdit_3->setText("");
        ui->DNS1LineEdit_4->setText("");
        ui->DNS2LineEdit->setText("");
        ui->DNS2LineEdit_2->setText("");
        ui->DNS2LineEdit_3->setText("");
        ui->DNS2LineEdit_4->setText("");
        ui->DNS3LineEdit->setText("");
        ui->DNS3LineEdit_2->setText("");
        ui->DNS3LineEdit_3->setText("");
        ui->DNS3LineEdit_4->setText("");
    }
    if(ui->dhcpRadio->isChecked())
    {
        ui->ipAddressLineEdit->setText("");
        ui->ipAddressLineEdit_2->setText("");
        ui->ipAddressLineEdit_3->setText("");
        ui->ipAddressLineEdit_4->setText("");
        ui->subnetMaskLineEdit->setText("");
        ui->subnetMaskLineEdit_2->setText("");
        ui->subnetMaskLineEdit_3->setText("");
        ui->subnetMaskLineEdit_4->setText("");
        ui->gatewayLineEdit->setText("");
        ui->gatewayLineEdit_2->setText("");
        ui->gatewayLineEdit_3->setText("");
        ui->gatewayLineEdit_4->setText("");
    }
    LOG_DEBUG( "GGG", NULL );
    if(security == "WPA EAP-TLS" || security == "WPA2 EAP-TLS")
    {
        ;
        // Get directory of import file
        QFileInfo info = QFileInfo(importFilename);
        // Get the directory
        QString dir_name = info.path();
        // Construct the path to the video file
        QString trustedRootCertFullPath = dir_name + "/" + trustedRootCertFilename;
        QString clientCertFullPath = dir_name + "/" + clientCertFilename;

        // It needs certs copied
        LOG_DEBUG( "trustedRootFilename is %s", qPrintable( trustedRootCertFilename ) );
        LOG_DEBUG( "clientCertFilename is %s", qPrintable( clientCertFilename ) );
        LOG_DEBUG( "clientCertPassword is %s", qPrintable( clientCertPassword ) );
        LOG_DEBUG( "trustedRootFullPath is %s", qPrintable( trustedRootCertFullPath ) );
        LOG_DEBUG( "clientCertFullPath is %s", qPrintable( clientCertFullPath ) );

        python pythonProcess;
        QStringList args;
        args.clear();
        args.append("-U");
        args.append("0");
        args.append("-S");
        args.append(trustedRootCertFullPath);
        args.append("-T");
        args.append("ca");
        if (pythonProcess.execCommand("installCert", args))
        {
          ;
        }
        this->loadTrustedRootCertsCombo();
        ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(trustedRootCertFilename));
        args.clear();
        args.append("-U");
        args.append("0");
        args.append("-S");
        args.append(clientCertFullPath);
        args.append("-T");
        args.append("client");
        if (pythonProcess.execCommand("installCert", args))
        {
          ;
        }
        this->loadClientCertsCombo();
        ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(clientCertFilename));
        this->ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(trustedRootCertFilename));
        this->ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(clientCertFilename));
        this->ui->leClientKeyPassword->setText(clientCertPassword);
    }

    //LOG_DEBUG( "importNetworkSettings() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     I M P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void setupUtility::importNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "importNetworkBut_click" );
    //LOG_DEBUG( "importNetworkBut_click() START", NULL );

    //processingAButton = true;
    QProcess importConfigs;
    QStringList myOptions = this->buildImportConfigurationOptions(commonNetwork);
    if(myOptions.isEmpty()) {
        //processingAButton = false;
        return;
    }
    importConfigs.start("python", myOptions);

    //Wait for import to finish
    if(!importConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        // Start configuration error.
        //return;
    }

    if(!importConfigs.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
        // return;
        // Finish configuration error.
    }

    //Get stdout from importConfigs
    QByteArray result = importConfigs.readAll();
    QString configs(result);
    QStringList res = configs.split(QRegExp("[\r\n]"),configs.SkipEmptyParts);

    //Use regex to split results
    //QRegExp configurationFormat("[\\w\.\(\)\-]+ : (\\w*)");
    if(res.at(0) == "NO")
    {
        // bad file reject it
        ui->alertsLineEdit->setText(tr("Import file format error."));
       // this->ui->alertsLineEdit->setStyleSheet("color:red;");
    }
    else
    {
        QRegExp configurationFormat("[^ ] : (.*)");
        QStringList bestResults;
        //int j = 0;

        //Go through the file format and extract the connection information

        /////OLD
        bool foundLinkRate = false;
        bool foundAutomaticLinkRate = false;
        bool foundChannel = false;
        QStringList resultLine;



        for (int i = 0; i < res.size(); i++)
        {
            resultLine = res[i].split(" : ",QString::KeepEmptyParts);
            if(resultLine[0].contains("AUTOMATICLINKRATE",Qt::CaseSensitive))
            {
                foundAutomaticLinkRate = true;
                bestResults.append(resultLine[1]);
            }
            else if(resultLine[0].contains("LINKRATE",Qt::CaseSensitive))
            {
                foundLinkRate = true;
                bestResults.append(resultLine[1]);
            }
            else if(resultLine[0].contains("CHANNEL",Qt::CaseSensitive))
            {
                foundChannel = true;
                bestResults.append(resultLine[1]);
            }
            else if(resultLine[0].contains("ISSTATIC",Qt::CaseSensitive))
            {
                if(!foundLinkRate)
                {
                    bestResults.append("auto");
                }
                if(!foundAutomaticLinkRate)
                {
                    bestResults.append("1");
                }
                if(!foundChannel)
                {
                    bestResults.append("auto");
                }
                bestResults.append(resultLine[1]);
            }
            else bestResults.append(resultLine[1]);


        }
/*        while(j < res.size()) {
            configurationFormat.indexIn(res[j]);
            if(configurationFormat.capturedTexts()[0].contains("AUTOMATICLINKRATE",Qt::CaseSensitive))
            {
                foundAutomaticLinkRate = true;
                bestResults.append(configurationFormat.capturedTexts()[1]);
            }
            else if(configurationFormat.capturedTexts()[0].contains("LINKRATE",Qt::CaseSensitive))
            {
                foundLinkRate = true;
                bestResults.append(configurationFormat.capturedTexts()[1]);
            }
            else if(configurationFormat.capturedTexts()[0].contains("CHANNEL",Qt::CaseSensitive))
            {
                foundChannel = true;
                bestResults.append(configurationFormat.capturedTexts()[1]);
            }
            else if(configurationFormat.capturedTexts()[0].contains("ISSTATIC",Qt::CaseSensitive))
            {
                if(!foundLinkRate)
                {
                    bestResults.append("auto");
                }
                if(!foundAutomaticLinkRate)
                {
                    bestResults.append("1");
                }
                if(!foundChannel)
                {
                    bestResults.append("auto");
                }
                bestResults.append(configurationFormat.capturedTexts()[1]);
            }
            else bestResults.append(configurationFormat.capturedTexts()[1]);

            j++;
        }
*/


        //for(int i = 0; i < res.size(); i++)
        //{
        //    if(res[i])
        //}

        /// END OLD
        ///

        //for

        //Debug loop
        /*j=0;
        while(j < bestResults.size()) {
            qDebug() << bestResults[j];
            j++;
        }
*/
        //Fill in the fields using the set functions
        QString secMeth;
        if((!bestResults.isEmpty()) && (bestResults[0] != ""))
        {
            secMeth = bestResults[1];
  //          secMeth = secMeth.replace("\"", "");
        }

        bool doSecondAssignments = true;
        bool isWireless = true;

       // qDebug() << "made it here";
        if(bestResults.isEmpty()) {
            //if it gets here, then regex is wrong
          //  qDebug() << "Not setting fields correctly";
            //processingAButton = false;
            return;
        }
        else if(bestResults[0] == "") // WIRED
        {
            doSecondAssignments = false;
            isWireless = false;
            if(bestResults[11] == "0")
            {
                // dhcp
                ui->dhcpRadio->setChecked(true);
                ui->ipAddressLineEdit->setText("");
                ui->ipAddressLineEdit_2->setText("");
                ui->ipAddressLineEdit_3->setText("");
                ui->ipAddressLineEdit_4->setText("");

                ui->subnetMaskLineEdit->setText("");
                ui->subnetMaskLineEdit_2->setText("");
                ui->subnetMaskLineEdit_3->setText("");
                ui->subnetMaskLineEdit_4->setText("");

                ui->gatewayLineEdit->setText("");
                ui->gatewayLineEdit_2->setText("");
                ui->gatewayLineEdit_3->setText("");
                ui->gatewayLineEdit_4->setText("");
            }
            else
            {
                //static
                ui->dhcpRadio->setChecked(false);
                ui->staticIPRadio->setChecked(true);
                if(bestResults[12] == "0" || bestResults[12] == "64641964")
                {
                    ui->ipAddressLineEdit->setText("");
                    ui->ipAddressLineEdit_2->setText("");
                    ui->ipAddressLineEdit_3->setText("");
                    ui->ipAddressLineEdit_4->setText("");
                }
                else
                {
                    QStringList IPAddressParts;
                    IPAddressParts = bestResults[12].split(".", QString::KeepEmptyParts);

                    if(IPAddressParts.size() == 4)
                    {
                        // Reverse commented these lines below on August 19, 2013
                        ui->ipAddressLineEdit->setText("");
                        ui->ipAddressLineEdit_2->setText("");
                        ui->ipAddressLineEdit_3->setText("");
                        ui->ipAddressLineEdit_4->setText("");
                       // ui->ipAddressLineEdit->setText(IPAddressParts.at(0));
                       // ui->ipAddressLineEdit_2->setText(IPAddressParts.at(1));
                       // ui->ipAddressLineEdit_3->setText(IPAddressParts.at(2));
                       // ui->ipAddressLineEdit_4->setText(IPAddressParts.at(3));
                    }
                    //ui->ipAddressLineEdit->setText(bestResults[9]);
                }
                if(bestResults[13] == "0" || bestResults[13] == "64641964")
                {
                    ui->subnetMaskLineEdit->setText("");
                    ui->subnetMaskLineEdit_2->setText("");
                    ui->subnetMaskLineEdit_3->setText("");
                    ui->subnetMaskLineEdit_4->setText("");
                }
                else
                {
                    QStringList SubnetParts;
                    SubnetParts = bestResults[13].split(".", QString::KeepEmptyParts);

                    if(SubnetParts.size() == 4)
                    {
                        ui->subnetMaskLineEdit->setText(SubnetParts.at(0));
                        ui->subnetMaskLineEdit_2->setText(SubnetParts.at(1));
                        ui->subnetMaskLineEdit_3->setText(SubnetParts.at(2));
                        ui->subnetMaskLineEdit_4->setText(SubnetParts.at(3));
                    }
                    //ui->subnetMaskLineEdit->setText(bestResults[10]);
                }
                if(bestResults[14] == "0" || bestResults[14] == "64641964")
                {
                    ui->gatewayLineEdit->setText("");
                    ui->gatewayLineEdit_2->setText("");
                    ui->gatewayLineEdit_3->setText("");
                    ui->gatewayLineEdit_4->setText("");
                }
                else
                {
                    QStringList GatewayParts;
                    GatewayParts = bestResults[14].split(".", QString::KeepEmptyParts);

                    if(GatewayParts.size() == 4)
                    {
                        ui->gatewayLineEdit->setText(GatewayParts.at(0));
                        ui->gatewayLineEdit_2->setText(GatewayParts.at(1));
                        ui->gatewayLineEdit_3->setText(GatewayParts.at(2));
                        ui->gatewayLineEdit_4->setText(GatewayParts.at(3));
                    }
                    //ui->gatewayLineEdit->setText(bestResults[11]);
                }
                this->ui->cbTransferRate->setCurrentIndex(0);

                ui->chkboxTransferRateAuto->setChecked(true);
                this->ui->cbChannel->setCurrentIndex(0);

                revealNetworkGroupBoxPrefilled(3);

            }

            if(bestResults[15] == "1" || bestResults[15] == "" || bestResults[15] == "64641964")
            {
                //autodns
                ui->manualDNSRadio->setChecked(false);
                ui->autoDNSRadio->setChecked(true);
                ui->DNS1LineEdit->setText("");
                ui->DNS1LineEdit_2->setText("");
                ui->DNS1LineEdit_3->setText("");
                ui->DNS1LineEdit_4->setText("");

                ui->DNS2LineEdit->setText("");
                ui->DNS2LineEdit_2->setText("");
                ui->DNS2LineEdit_3->setText("");
                ui->DNS2LineEdit_4->setText("");

                ui->DNS3LineEdit->setText("");
                ui->DNS3LineEdit_2->setText("");
                ui->DNS3LineEdit_3->setText("");
                ui->DNS3LineEdit_4->setText("");
            }
            else
            {
                //manual
                ui->dhcpRadio->setChecked(false);
                ui->manualDNSRadio->setChecked(true);
                if(bestResults[16] == "0" || bestResults[16] == "64641964")
                {
                    ui->DNS1LineEdit->setText("");
                    ui->DNS1LineEdit_2->setText("");
                    ui->DNS1LineEdit_3->setText("");
                    ui->DNS1LineEdit_4->setText("");
                }
                else
                {
                    QStringList D1Parts;
                    D1Parts = bestResults[16].split(".", QString::KeepEmptyParts);

                    if(D1Parts.size() == 4)
                    {
                        ui->DNS1LineEdit->setText(D1Parts.at(0));
                        ui->DNS1LineEdit_2->setText(D1Parts.at(1));
                        ui->DNS1LineEdit_3->setText(D1Parts.at(2));
                        ui->DNS1LineEdit_4->setText(D1Parts.at(3));
                    }
                    //ui->DNS1LineEdit->setText(bestResults[13]);
                }

                if(bestResults[17] == "0" || bestResults[17] == "64641964")
                {
                    ui->DNS2LineEdit->setText("");
                    ui->DNS2LineEdit_2->setText("");
                    ui->DNS2LineEdit_3->setText("");
                    ui->DNS2LineEdit_4->setText("");
                }
                else
                {
                    QStringList D2Parts;
                    D2Parts = bestResults[17].split(".", QString::KeepEmptyParts);

                    if(D2Parts.size() == 4)
                    {
                        ui->DNS2LineEdit->setText(D2Parts.at(0));
                        ui->DNS2LineEdit_2->setText(D2Parts.at(1));
                        ui->DNS2LineEdit_3->setText(D2Parts.at(2));
                        ui->DNS2LineEdit_4->setText(D2Parts.at(3));
                    }
                    //ui->DNS2LineEdit->setText(bestResults[13]);
                }

                if(bestResults[18] == "0" || bestResults[18] == "64641964")
                {
                    ui->DNS3LineEdit->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_3->setText("");
                    ui->DNS3LineEdit_4->setText("");
                }
                else
                {
                    QStringList D3Parts;
                    D3Parts = bestResults[18].split(".", QString::KeepEmptyParts);

                    if(D3Parts.size() == 4)
                    {
                        ui->DNS3LineEdit->setText(D3Parts.at(0));
                        ui->DNS3LineEdit_2->setText(D3Parts.at(1));
                        ui->DNS3LineEdit_3->setText(D3Parts.at(2));
                        ui->DNS3LineEdit_4->setText(D3Parts.at(3));
                    }
                    //ui->DNS1LineEdit->setText(bestResults[13]);
                }
            }
            revealNetworkGroupBoxPrefilled(1);
        }

        else if(bestResults.size() == 21 || bestResults.size() == 15) { //wpa

        //else if(bestResults.size() == 15) { //wpa
            commonNetwork->setSSID(bestResults[0]);

            commonNetwork->setSecurityMethod(secMeth);
            commonNetwork->setEncryption(bestResults[2]);
            commonNetwork->setWPAPassPhrase(bestResults[3]);

            //Update SSID Info
            this->ui->SSIDLineEdit->setText(bestResults[0]);
            setWirelessNetworkEncryptionCombo(1);
            //Update Combo Boxes
            //qDebug() << " secMeth is: " << secMeth;
            int pos = this->ui->securityCombo->findData(secMeth);
            //qDebug() << "pos is :" << pos;
            if(pos == -1) {
                //////qDebug() << bestResults[1];
                //////qDebug() << "couldnt find security Combo here 0";
                //processingAButton = false;
                return;
            }
            else {
                this->ui->securityCombo->setCurrentIndex(pos);
            }


            pos = this->ui->encryptionCombo->findData(bestResults[2]);
            if(pos == -1) {
                //////qDebug() << bestResults[2];
                //////qDebug() << "couldnt find encryption Combo here 1";
                //processingAButton = false;
                return;
            }
            else
            {
                this->ui->encryptionCombo->setCurrentIndex(pos);
            }

            this->ui->wpaPassphraseLineEdit->setText(bestResults[3]);
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[4]));
            if(bestResults[5] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[6]));

            revealNetworkGroupBoxPrefilled(3);
            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            this->ui->wpaPassPhraseFrame->show();
            this->ui->eapUserPassFrame->hide();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->hide();

        }
        else if(bestResults.size() == 22 || bestResults.size() == 16) {//leapPeap
        //else if(bestResults.size() == 16) {//leapPeap
            commonNetwork->setSSID(bestResults[0]);
            commonNetwork->setSecurityMethod(secMeth);
            commonNetwork->setEncryption(bestResults[2]);
            commonNetwork->setEAPUsername(bestResults[3]);
            commonNetwork->setEAPPassword(bestResults[4]);

            //Update SSID Info
            this->ui->SSIDLineEdit->setText(bestResults[0]);

            //Update Combo Boxes
            loadWirelessSecurityMethodsCombo();

            ui->securityCombo->setCurrentIndex(ui->securityCombo->findData(bestResults[1]));

            if(bestResults[1].contains("8")) {
                setWirelessNetworkEncryptionCombo(9);
                this->ui->encryptionCombo->setCurrentIndex(0);
            }
            else {
                setWirelessNetworkEncryptionCombo(7);
                int pos = this->ui->encryptionCombo->findData(bestResults[2]);
                if(pos == -1) {
                    //////qDebug() << bestResults[2];
                    //////qDebug() << "couldnt find encryption Combo here 4";
                    //processingAButton = false;
                    return;
                }
                else {
                    this->ui->encryptionCombo->setCurrentIndex(pos);
                }
            }

            //ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findData(bestResults[2]));
            this->ui->eapUsernameLineEdit->setText(bestResults[3]);
            this->ui->eapPasswordLineEdit->setText(bestResults[4]);
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[5]));
            if(bestResults[6] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[7]));

            revealNetworkGroupBoxPrefilled(3);
            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->show();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->hide();

        }
        else if(bestResults[4] == "None")
        {
            // Open None
            this->ui->SSIDLineEdit->setText(bestResults[0]);
            this->ui->securityCombo->setCurrentIndex(ui->securityCombo->findText(bestResults[1]));
            this->ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findText(bestResults[2]));
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[3]));
            if(bestResults[4] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[5]));

            revealNetworkGroupBoxPrefilled(3);


            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->hide();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->show();
        }
        else if(bestResults.size() == 24 || bestResults.size() == 18)
//        else if(bestResults.size() == 18)
        {
            //EAP-TLS
            commonNetwork->setSSID(bestResults[0]);
            commonNetwork->setSecurityMethod(secMeth);
            commonNetwork->setEncryption(bestResults[2]);

            //Update SSID Info
            this->ui->SSIDLineEdit->setText(bestResults[0]);

            this->ui->securityCombo->setCurrentIndex(ui->securityCombo->findText(bestResults[1]));
            this->ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findText(bestResults[2]));

            // Copy the certs now
            for(int i = 0; i < myOptions.size(); i++)
            {
                if(myOptions[i].contains("/media/",Qt::CaseSensitive))
                {
                    // this has the path
                    QString pathToCert = "/";
                    QStringList pathArray = myOptions[i].split("/",QString::KeepEmptyParts);
                    for (int j = 0; j < pathArray.size()-1; j++)
                    {
                        pathToCert += pathArray[j] + "/";
                    }
                    QString pathToTrustedRootCert;
                    QString pathToClientCert;
                    pathToTrustedRootCert = pathToCert + bestResults[3];
                    pathToClientCert = pathToCert + bestResults[5];
                    // call python install now
                    //ui->alertsLineEdit->setText(pathToTrustedRootCert);
                    python pythonProcess;
                    QStringList args;
                    args.clear();
                    args.append("-U");
                    args.append("0");
                    args.append("-S");
                    args.append(pathToTrustedRootCert);
                    args.append("-T");
                    args.append("ca");
                    if (pythonProcess.execCommand("installCert", args))
                    {
                      ;
                    }
                    this->loadTrustedRootCertsCombo();
                    ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(bestResults[3]));
                    args.clear();
                    args.append("-U");
                    args.append("0");
                    args.append("-S");
                    args.append(pathToClientCert);
                    args.append("-T");
                    args.append("client");
                    if (pythonProcess.execCommand("installCert", args))
                    {
                      ;
                    }
                    this->loadClientCertsCombo();
                    ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(bestResults[5]));

                }
            }

            this->ui->trustedRootStoreCombo->setCurrentIndex(ui->trustedRootStoreCombo->findText(bestResults[3]));
            this->ui->clientCertStoreCombo->setCurrentIndex(ui->clientCertStoreCombo->findText(bestResults[5]));
            this->ui->leClientKeyPassword->setText(bestResults[6]);
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[7]));
            if(bestResults[8] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }

            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[9]));
            revealNetworkGroupBoxPrefilled(3);
            this->ui->wpaEAPTLSPassPhraseFrame->show();
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->hide();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->hide();
        }
        else {//wep
            commonNetwork->setSSID(bestResults[0]);
            commonNetwork->setSecurityMethod(secMeth);
            commonNetwork->setEncryption(bestResults[2]);
            commonNetwork->setDefaultKey(bestResults[3]);
            if(bestResults[4] == "0")
            {
                commonNetwork->setWEPKey1("");
                ui->wepKey1LineEdit->setText("");
            }
            else
            {
                commonNetwork->setWEPKey1(bestResults[4]);
                ui->wepKey1LineEdit->setText(bestResults[4]);
            }
            if(bestResults[5] == "0")
            {
                commonNetwork->setWEPKey2("");
                ui->wepKey2LineEdit->setText("");
            }
            else
            {
                commonNetwork->setWEPKey2(bestResults[5]);
                ui->wepKey2LineEdit->setText(bestResults[5]);
            }
            if(bestResults[6] == "0")
            {
                commonNetwork->setWEPKey3("");
                ui->wepKey3LineEdit->setText("");
            }
            else
            {
                commonNetwork->setWEPKey3(bestResults[6]);
                ui->wepKey3LineEdit->setText(bestResults[6]);
            }
            if(bestResults[7] == "0")
            {
                commonNetwork->setWEPKey4("");
                ui->wepKey4LineEdit->setText("");
            }
            else
            {
                commonNetwork->setWEPKey4(bestResults[7]);
                ui->wepKey4LineEdit->setText(bestResults[7]);
            }

                /* Update GUI */

            //Update SSID Info
            this->ui->SSIDLineEdit->setText(bestResults[0]);

            //Update Combo Boxes
            loadWirelessSecurityMethodsCombo();

            //Update Default Key and key Buttons
            if(bestResults[3] == "1")
            {
                this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
            }
            else if(bestResults[3] == "2")
            {
                this->ui->wirelessAuthenticationDefaultKey2Radio->setChecked(true);
            }
            else if(bestResults[3] == "3")
            {
                this->ui->wirelessAuthenticationDefaultKey3Radio->setChecked(true);
            }
            else if(bestResults[3] == "4")
            {
                this->ui->wirelessAuthenticationdefaultKey4Radio->setChecked(true);
            }
            //this->ui->wepKey1LineEdit->setText(this->commonNetwork->WEPKey1());
            //this->ui->wepKey2LineEdit->setText(this->commonNetwork->WEPKey2());
            //this->ui->wepKey3LineEdit->setText(this->commonNetwork->WEPKey3());
            //this->ui->wepKey4LineEdit->setText(this->commonNetwork->WEPKey4());
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[8]));
            if(bestResults[9] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[10]));
            revealNetworkGroupBoxPrefilled(3);
            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->hide();
            this->ui->wepKeyFrame->show();
            this->ui->openNoneFrame->hide();

        }

        if(doSecondAssignments)
        {
            if(bestResults.size() < 20 && bestResults.size() > 14)
            {
                // it is pre 2.4.0 version

                if(bestResults[bestResults.size() - 8] == "1" )
                {
                    //static
                    ui->dhcpRadio->setChecked(false);
                    ui->staticIPRadio->setChecked(true);
                    //LOG_DEBUG( "importNetworkBut_click() DHCPHostnameCheckbox->setChecked(false) 1", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);
                    ui->DHCPHostnameCheckbox->setEnabled(false);
                    ui->DHCPHostnameWarning->hide();
                    ui->setHostnameBut->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true) 3", NULL );
                    ui->hostnameLineEdit->setEnabled(true);
                    ui->ipAddressLineEdit->setEnabled(true);
                    ui->ipAddressLineEdit_2->setEnabled(true);
                    ui->ipAddressLineEdit_3->setEnabled(true);
                    ui->ipAddressLineEdit_4->setEnabled(true);
                    ui->subnetMaskLineEdit->setEnabled(true);
                    ui->subnetMaskLineEdit_2->setEnabled(true);
                    ui->subnetMaskLineEdit_3->setEnabled(true);
                    ui->subnetMaskLineEdit_4->setEnabled(true);
                    ui->gatewayLineEdit->setEnabled(true);
                    ui->gatewayLineEdit_2->setEnabled(true);
                    ui->gatewayLineEdit_3->setEnabled(true);
                    ui->gatewayLineEdit_4->setEnabled(true);

                    if(bestResults[bestResults.size() - 7] == "64641964" || bestResults[bestResults.size() - 7] == "" || bestResults[bestResults.size() - 7] == "0.0.0.0")
                    {
                        ui->ipAddressLineEdit->setText("");
                        ui->ipAddressLineEdit_2->setText("");
                        ui->ipAddressLineEdit_3->setText("");
                        ui->ipAddressLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList IPAddressParts;
                        IPAddressParts = bestResults[bestResults.size() - 7].split(".", QString::KeepEmptyParts);

                        if(IPAddressParts.size() == 4)
                        {
                            //Reverse commented lines below on August 19, 2013
                            ui->ipAddressLineEdit->setText("");
                            ui->ipAddressLineEdit_2->setText("");
                            ui->ipAddressLineEdit_3->setText("");
                            ui->ipAddressLineEdit_4->setText("");

                            //ui->ipAddressLineEdit->setText(IPAddressParts.at(0));
                            //ui->ipAddressLineEdit_2->setText(IPAddressParts.at(1));
                            //ui->ipAddressLineEdit_3->setText(IPAddressParts.at(2));
                            //ui->ipAddressLineEdit_4->setText(IPAddressParts.at(3));
                        }

                        //ui->ipAddressLineEdit->setText(bestResults[bestResults.size() - 7]);
                    }
                    if(bestResults[bestResults.size() - 6] == "64641964" || bestResults[bestResults.size() - 6] == "64641964" || bestResults[bestResults.size() - 6] == "0.0.0.0")
                    {
                        ui->subnetMaskLineEdit->setText("");
                        ui->subnetMaskLineEdit_2->setText("");
                        ui->subnetMaskLineEdit_3->setText("");
                        ui->subnetMaskLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList SubnetParts;
                        SubnetParts = bestResults[bestResults.size() - 6].split(".", QString::KeepEmptyParts);

                        if(SubnetParts.size() == 4)
                        {
                            ui->subnetMaskLineEdit->setText(SubnetParts.at(0));
                            ui->subnetMaskLineEdit_2->setText(SubnetParts.at(1));
                            ui->subnetMaskLineEdit_3->setText(SubnetParts.at(2));
                            ui->subnetMaskLineEdit_4->setText(SubnetParts.at(3));
                        }
                        //ui->subnetMaskLineEdit->setText(bestResults[bestResults.size() - 6]);
                    }

                    if(bestResults[bestResults.size() - 5] == "64641964" || bestResults[bestResults.size() - 5] == "" || bestResults[bestResults.size() - 5] == "0.0.0.0")
                    {
                        ui->gatewayLineEdit->setText("");
                        ui->gatewayLineEdit_2->setText("");
                        ui->gatewayLineEdit_3->setText("");
                        ui->gatewayLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList GatewayParts;
                        GatewayParts = bestResults[bestResults.size() - 5].split(".", QString::KeepEmptyParts);

                        if(GatewayParts.size() == 4)
                        {
                            ui->gatewayLineEdit->setText(GatewayParts.at(0));
                            ui->gatewayLineEdit_2->setText(GatewayParts.at(1));
                            ui->gatewayLineEdit_3->setText(GatewayParts.at(2));
                            ui->gatewayLineEdit_4->setText(GatewayParts.at(3));
                        }
                        //ui->gatewayLineEdit->setText(bestResults[bestResults.size() - 5]);
                    }

                    ui->ipAddressLineEdit->setEnabled(true);
                    ui->ipAddressLineEdit_2->setEnabled(true);
                    ui->ipAddressLineEdit_3->setEnabled(true);
                    ui->ipAddressLineEdit_4->setEnabled(true);

                    ui->subnetMaskLineEdit->setEnabled(true);
                    ui->subnetMaskLineEdit_2->setEnabled(true);
                    ui->subnetMaskLineEdit_3->setEnabled(true);
                    ui->subnetMaskLineEdit_4->setEnabled(true);

                    ui->gatewayLineEdit->setEnabled(true);
                    ui->gatewayLineEdit_2->setEnabled(true);
                    ui->gatewayLineEdit_3->setEnabled(true);
                    ui->gatewayLineEdit_4->setEnabled(true);
                }
                else
                {
                    ui->staticIPRadio->setChecked(false);
                    ui->dhcpRadio->setChecked(true);
                    ui->DHCPHostnameCheckbox->setEnabled(true);

                    ui->ipAddressLineEdit->setText("");
                    ui->ipAddressLineEdit_2->setText("");
                    ui->ipAddressLineEdit_3->setText("");
                    ui->ipAddressLineEdit_4->setText("");

                    ui->subnetMaskLineEdit->setText("");
                    ui->subnetMaskLineEdit_2->setText("");
                    ui->subnetMaskLineEdit_3->setText("");
                    ui->subnetMaskLineEdit_4->setText("");

                    ui->gatewayLineEdit->setText("");
                    ui->gatewayLineEdit_2->setText("");
                    ui->gatewayLineEdit_3->setText("");
                    ui->gatewayLineEdit_4->setText("");

                    ui->ipAddressLineEdit->setEnabled(false);
                    ui->ipAddressLineEdit_2->setEnabled(false);
                    ui->ipAddressLineEdit_3->setEnabled(false);
                    ui->ipAddressLineEdit_4->setEnabled(false);

                    ui->subnetMaskLineEdit->setEnabled(false);
                    ui->subnetMaskLineEdit_2->setEnabled(false);
                    ui->subnetMaskLineEdit_3->setEnabled(false);
                    ui->subnetMaskLineEdit_4->setEnabled(false);

                    ui->gatewayLineEdit->setEnabled(false);
                    ui->gatewayLineEdit_2->setEnabled(false);
                    ui->gatewayLineEdit_3->setEnabled(false);
                    ui->gatewayLineEdit_4->setEnabled(false);
                }

                if(bestResults[bestResults.size() - 4] == "1")
                {
                    ui->manualDNSRadio->setChecked(false);
                    ui->autoDNSRadio->setChecked(true);
                    ui->DNS1LineEdit->setText("");
                    ui->DNS1LineEdit_2->setText("");
                    ui->DNS1LineEdit_3->setText("");
                    ui->DNS1LineEdit_4->setText("");

                    ui->DNS2LineEdit->setText("");
                    ui->DNS2LineEdit_2->setText("");
                    ui->DNS2LineEdit_3->setText("");
                    ui->DNS2LineEdit_4->setText("");

                    ui->DNS3LineEdit->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_3->setText("");
                    ui->DNS3LineEdit_4->setText("");

                    ui->DNS1LineEdit->setEnabled(false);
                    ui->DNS1LineEdit_2->setEnabled(false);
                    ui->DNS1LineEdit_3->setEnabled(false);
                    ui->DNS1LineEdit_4->setEnabled(false);

                    ui->DNS2LineEdit->setEnabled(false);
                    ui->DNS2LineEdit_2->setEnabled(false);
                    ui->DNS2LineEdit_3->setEnabled(false);
                    ui->DNS2LineEdit_4->setEnabled(false);

                    ui->DNS3LineEdit->setEnabled(false);
                    ui->DNS3LineEdit_2->setEnabled(false);
                    ui->DNS3LineEdit_3->setEnabled(false);
                    ui->DNS3LineEdit_4->setEnabled(false);
                }
                else
                {
                    ui->autoDNSRadio->setChecked(false);
                    ui->manualDNSRadio->setChecked(true);

                    ui->DNS1LineEdit->setEnabled(true);
                    ui->DNS1LineEdit_2->setEnabled(true);
                    ui->DNS1LineEdit_3->setEnabled(true);
                    ui->DNS1LineEdit_4->setEnabled(true);

                    ui->DNS2LineEdit->setEnabled(true);
                    ui->DNS2LineEdit_2->setEnabled(true);
                    ui->DNS2LineEdit_3->setEnabled(true);
                    ui->DNS2LineEdit_4->setEnabled(true);

                    ui->DNS3LineEdit->setEnabled(true);
                    ui->DNS3LineEdit_2->setEnabled(true);
                    ui->DNS3LineEdit_3->setEnabled(true);
                    ui->DNS3LineEdit_4->setEnabled(true);
                    if(bestResults[bestResults.size() - 3] == "64641964" || bestResults[bestResults.size() - 3] == "" || bestResults[bestResults.size() - 3] == "0.0.0.0")
                    {
                        ui->DNS1LineEdit->setText("");
                        ui->DNS1LineEdit_2->setText("");
                        ui->DNS1LineEdit_3->setText("");
                        ui->DNS1LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D1Parts;
                        D1Parts = bestResults[bestResults.size() -3].split(".", QString::KeepEmptyParts);

                        if(D1Parts.size() == 4)
                        {
                            ui->DNS1LineEdit->setText(D1Parts.at(0));
                            ui->DNS1LineEdit_2->setText(D1Parts.at(1));
                            ui->DNS1LineEdit_3->setText(D1Parts.at(2));
                            ui->DNS1LineEdit_4->setText(D1Parts.at(3));
                        }

                      //  ui->DNS1LineEdit->setText(bestResults[bestResults.size() - 3]);
                    }
                    if(bestResults[bestResults.size() - 2] == "64641964" || bestResults[bestResults.size() - 2] == "" || bestResults[bestResults.size() - 2] == "0.0.0.0")
                    {
                        ui->DNS2LineEdit->setText("");
                        ui->DNS2LineEdit_2->setText("");
                        ui->DNS2LineEdit_3->setText("");
                        ui->DNS2LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D2Parts;
                        D2Parts = bestResults[bestResults.size() - 2].split(".", QString::KeepEmptyParts);

                        if(D2Parts.size() == 4)
                        {
                            ui->DNS2LineEdit->setText(D2Parts.at(0));
                            ui->DNS2LineEdit_2->setText(D2Parts.at(1));
                            ui->DNS2LineEdit_3->setText(D2Parts.at(2));
                            ui->DNS2LineEdit_4->setText(D2Parts.at(3));
                        }
                        //ui->DNS2LineEdit->setText(bestResults[bestResults.size() - 2]);
                    }
                    if(bestResults[bestResults.size() - 1] == "64641964" || bestResults[bestResults.size() - 1] == "" || bestResults[bestResults.size() - 1] == "0.0.0.0")
                    {
                        ui->DNS3LineEdit->setText("");
                        ui->DNS3LineEdit_2->setText("");
                        ui->DNS3LineEdit_3->setText("");
                        ui->DNS3LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D3Parts;
                        D3Parts = bestResults[bestResults.size() - 1].split(".", QString::KeepEmptyParts);

                        if(D3Parts.size() == 4)
                        {
                            ui->DNS3LineEdit->setText(D3Parts.at(0));
                            ui->DNS3LineEdit_2->setText(D3Parts.at(1));
                            ui->DNS3LineEdit_3->setText(D3Parts.at(2));
                            ui->DNS3LineEdit_4->setText(D3Parts.at(3));
                        }
                        //ui->DNS3LineEdit->setText(bestResults[bestResults.size() - 1]);
                    }
                }


               /*&
                if(bestResults[bestResults.size() - 3] == "64641964" || bestResults[bestResults.size() - 3] == "" || bestResults[bestResults.size() - 3] == "0.0.0.0")
                {
                    ui->leDNSSuffix1->setText("");
                }
                else
                {
                    ui->leDNSSuffix1->setText(bestResults[bestResults.size() - 3]);
                }
                if(bestResults[bestResults.size() - 2] == "64641964" || bestResults[bestResults.size() - 2] == "" || bestResults[bestResults.size() - 2] == "0.0.0.0")
                {
                    ui->leDNSSuffix2->setText("");
                }
                else
                {
                    ui->leDNSSuffix2->setText(bestResults[bestResults.size() - 2]);
                }
                if(bestResults[bestResults.size() - 1] == "64641964" || bestResults[bestResults.size() - 1] == "" || bestResults[bestResults.size() - 1] == "0.0.0.0")
                {
                    ui->leDNSSuffix3->setText("");
                }
                else
                {
                    ui->leDNSSuffix3->setText(bestResults[bestResults.size() - 1]);
                }

                ui->leDNSSuffix4->setText("");
                ui->leDNSSuffix5->setText("");
                ui->leDNSSuffix6->setText("");
                */
            }
            else
            {
                // 2.4.0 LATEST VERSION
                if(bestResults[bestResults.size() - 14] == "1" )
                {
                    ui->dhcpRadio->setChecked(false);
                    ui->staticIPRadio->setChecked(true);
                    ui->DHCPHostnameCheckbox->setEnabled(false);
                    //LOG_DEBUG( "importNetworkBut_clicked() DHCPHostnameCheckbox->setChecked(false) 2", NULL );
                    ui->DHCPHostnameCheckbox->setChecked(false);
                    ui->setHostnameBut->setEnabled(true);
                    LOG_DEBUG( "hostnameLineEdit->setEnabled(true) 4", NULL );
                    ui->hostnameLineEdit->setEnabled(true);
                    ui->DHCPHostnameWarning->hide();
                    ui->ipAddressLineEdit->setEnabled(true);
                    ui->ipAddressLineEdit_2->setEnabled(true);
                    ui->ipAddressLineEdit_3->setEnabled(true);
                    ui->ipAddressLineEdit_4->setEnabled(true);
                    ui->subnetMaskLineEdit->setEnabled(true);
                    ui->subnetMaskLineEdit_2->setEnabled(true);
                    ui->subnetMaskLineEdit_3->setEnabled(true);
                    ui->subnetMaskLineEdit_4->setEnabled(true);
                    ui->gatewayLineEdit->setEnabled(true);
                    ui->gatewayLineEdit_2->setEnabled(true);
                    ui->gatewayLineEdit_3->setEnabled(true);
                    ui->gatewayLineEdit_4->setEnabled(true);

                    if(bestResults[bestResults.size() - 14] == "64641964" || bestResults[bestResults.size() - 14] == "" || bestResults[bestResults.size() - 14] == "0.0.0.0")
                    {
                        ui->ipAddressLineEdit->setText("");
                        ui->ipAddressLineEdit_2->setText("");
                        ui->ipAddressLineEdit_3->setText("");
                        ui->ipAddressLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList IPAddressParts;
                        IPAddressParts = bestResults[bestResults.size() - 14].split(".", QString::KeepEmptyParts);

                        if(IPAddressParts.size() == 4)
                        {
                            //Reverse commented lines below on August 19, 2013
                            ui->ipAddressLineEdit->setText("");
                            ui->ipAddressLineEdit_2->setText("");
                            ui->ipAddressLineEdit_3->setText("");
                            ui->ipAddressLineEdit_4->setText("");

                            //ui->ipAddressLineEdit->setText(IPAddressParts.at(0));
                            //ui->ipAddressLineEdit_2->setText(IPAddressParts.at(1));
                            //ui->ipAddressLineEdit_3->setText(IPAddressParts.at(2));
                            //ui->ipAddressLineEdit_4->setText(IPAddressParts.at(3));
                        }

                        //ui->ipAddressLineEdit->setText(bestResults[bestResults.size() - 7]);
                    }
                    if(bestResults[bestResults.size() - 12] == "64641964" || bestResults[bestResults.size() - 12] == "64641964" || bestResults[bestResults.size() - 12] == "0.0.0.0")
                    {
                        ui->subnetMaskLineEdit->setText("");
                        ui->subnetMaskLineEdit_2->setText("");
                        ui->subnetMaskLineEdit_3->setText("");
                        ui->subnetMaskLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList SubnetParts;
                        SubnetParts = bestResults[bestResults.size() - 12].split(".", QString::KeepEmptyParts);

                        if(SubnetParts.size() == 4)
                        {
                            ui->subnetMaskLineEdit->setText(SubnetParts.at(0));
                            ui->subnetMaskLineEdit_2->setText(SubnetParts.at(1));
                            ui->subnetMaskLineEdit_3->setText(SubnetParts.at(2));
                            ui->subnetMaskLineEdit_4->setText(SubnetParts.at(3));
                        }
                        //ui->subnetMaskLineEdit->setText(bestResults[bestResults.size() - 6]);
                    }

                    if(bestResults[bestResults.size() - 11] == "64641964" || bestResults[bestResults.size() - 11] == "" || bestResults[bestResults.size() - 11] == "0.0.0.0")
                    {
                        ui->gatewayLineEdit->setText("");
                        ui->gatewayLineEdit_2->setText("");
                        ui->gatewayLineEdit_3->setText("");
                        ui->gatewayLineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList GatewayParts;
                        GatewayParts = bestResults[bestResults.size() - 11].split(".", QString::KeepEmptyParts);

                        if(GatewayParts.size() == 4)
                        {
                            ui->gatewayLineEdit->setText(GatewayParts.at(0));
                            ui->gatewayLineEdit_2->setText(GatewayParts.at(1));
                            ui->gatewayLineEdit_3->setText(GatewayParts.at(2));
                            ui->gatewayLineEdit_4->setText(GatewayParts.at(3));
                        }
                        //ui->gatewayLineEdit->setText(bestResults[bestResults.size() - 5]);
                    }

                    ui->ipAddressLineEdit->setEnabled(true);
                    ui->ipAddressLineEdit_2->setEnabled(true);
                    ui->ipAddressLineEdit_3->setEnabled(true);
                    ui->ipAddressLineEdit_4->setEnabled(true);

                    ui->subnetMaskLineEdit->setEnabled(true);
                    ui->subnetMaskLineEdit_2->setEnabled(true);
                    ui->subnetMaskLineEdit_3->setEnabled(true);
                    ui->subnetMaskLineEdit_4->setEnabled(true);

                    ui->gatewayLineEdit->setEnabled(true);
                    ui->gatewayLineEdit_2->setEnabled(true);
                    ui->gatewayLineEdit_3->setEnabled(true);
                    ui->gatewayLineEdit_4->setEnabled(true);
                }
                else
                {
                    ui->staticIPRadio->setChecked(false);
                    ui->dhcpRadio->setChecked(true);
                    ui->DHCPHostnameCheckbox->setEnabled(true);
                    ui->ipAddressLineEdit->setText("");
                    ui->ipAddressLineEdit_2->setText("");
                    ui->ipAddressLineEdit_3->setText("");
                    ui->ipAddressLineEdit_4->setText("");

                    ui->subnetMaskLineEdit->setText("");
                    ui->subnetMaskLineEdit_2->setText("");
                    ui->subnetMaskLineEdit_3->setText("");
                    ui->subnetMaskLineEdit_4->setText("");

                    ui->gatewayLineEdit->setText("");
                    ui->gatewayLineEdit_2->setText("");
                    ui->gatewayLineEdit_3->setText("");
                    ui->gatewayLineEdit_4->setText("");

                    ui->ipAddressLineEdit->setEnabled(false);
                    ui->ipAddressLineEdit_2->setEnabled(false);
                    ui->ipAddressLineEdit_3->setEnabled(false);
                    ui->ipAddressLineEdit_4->setEnabled(false);

                    ui->subnetMaskLineEdit->setEnabled(false);
                    ui->subnetMaskLineEdit_2->setEnabled(false);
                    ui->subnetMaskLineEdit_3->setEnabled(false);
                    ui->subnetMaskLineEdit_4->setEnabled(false);

                    ui->gatewayLineEdit->setEnabled(false);
                    ui->gatewayLineEdit_2->setEnabled(false);
                    ui->gatewayLineEdit_3->setEnabled(false);
                    ui->gatewayLineEdit_4->setEnabled(false);
                }

                if(bestResults[bestResults.size() - 11] == "1")
                {
                    ui->manualDNSRadio->setChecked(false);
                    ui->autoDNSRadio->setChecked(true);
                    ui->DNS1LineEdit->setText("");
                    ui->DNS1LineEdit_2->setText("");
                    ui->DNS1LineEdit_3->setText("");
                    ui->DNS1LineEdit_4->setText("");

                    ui->DNS2LineEdit->setText("");
                    ui->DNS2LineEdit_2->setText("");
                    ui->DNS2LineEdit_3->setText("");
                    ui->DNS2LineEdit_4->setText("");

                    ui->DNS3LineEdit->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_3->setText("");
                    ui->DNS3LineEdit_4->setText("");

                    ui->DNS1LineEdit->setEnabled(false);
                    ui->DNS1LineEdit_2->setEnabled(false);
                    ui->DNS1LineEdit_3->setEnabled(false);
                    ui->DNS1LineEdit_4->setEnabled(false);

                    ui->DNS2LineEdit->setEnabled(false);
                    ui->DNS2LineEdit_2->setEnabled(false);
                    ui->DNS2LineEdit_3->setEnabled(false);
                    ui->DNS2LineEdit_4->setEnabled(false);

                    ui->DNS3LineEdit->setEnabled(false);
                    ui->DNS3LineEdit_2->setEnabled(false);
                    ui->DNS3LineEdit_3->setEnabled(false);
                    ui->DNS3LineEdit_4->setEnabled(false);
                }
                else
                {
                    ui->autoDNSRadio->setChecked(false);
                    ui->manualDNSRadio->setChecked(true);

                    ui->DNS1LineEdit->setEnabled(true);
                    ui->DNS1LineEdit_2->setEnabled(true);
                    ui->DNS1LineEdit_3->setEnabled(true);
                    ui->DNS1LineEdit_4->setEnabled(true);

                    ui->DNS2LineEdit->setEnabled(true);
                    ui->DNS2LineEdit_2->setEnabled(true);
                    ui->DNS2LineEdit_3->setEnabled(true);
                    ui->DNS2LineEdit_4->setEnabled(true);

                    ui->DNS3LineEdit->setEnabled(true);
                    ui->DNS3LineEdit_2->setEnabled(true);
                    ui->DNS3LineEdit_3->setEnabled(true);
                    ui->DNS3LineEdit_4->setEnabled(true);
                    if(bestResults[bestResults.size() - 10] == "64641964" || bestResults[bestResults.size() - 10] == "" || bestResults[bestResults.size() - 10] == "0.0.0.0")
                    {
                        ui->DNS1LineEdit->setText("");
                        ui->DNS1LineEdit_2->setText("");
                        ui->DNS1LineEdit_3->setText("");
                        ui->DNS1LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D1Parts;
                        D1Parts = bestResults[bestResults.size() - 10].split(".", QString::KeepEmptyParts);

                        if(D1Parts.size() == 4)
                        {
                            ui->DNS1LineEdit->setText(D1Parts.at(0));
                            ui->DNS1LineEdit_2->setText(D1Parts.at(1));
                            ui->DNS1LineEdit_3->setText(D1Parts.at(2));
                            ui->DNS1LineEdit_4->setText(D1Parts.at(3));
                        }

                      //  ui->DNS1LineEdit->setText(bestResults[bestResults.size() - 3]);
                    }
                    if(bestResults[bestResults.size() - 9] == "64641964" || bestResults[bestResults.size() - 9] == "" || bestResults[bestResults.size() - 9] == "0.0.0.0")
                    {
                        ui->DNS2LineEdit->setText("");
                        ui->DNS2LineEdit_2->setText("");
                        ui->DNS2LineEdit_3->setText("");
                        ui->DNS2LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D2Parts;
                        D2Parts = bestResults[bestResults.size() - 9].split(".", QString::KeepEmptyParts);

                        if(D2Parts.size() == 4)
                        {
                            ui->DNS2LineEdit->setText(D2Parts.at(0));
                            ui->DNS2LineEdit_2->setText(D2Parts.at(1));
                            ui->DNS2LineEdit_3->setText(D2Parts.at(2));
                            ui->DNS2LineEdit_4->setText(D2Parts.at(3));
                        }
                        //ui->DNS2LineEdit->setText(bestResults[bestResults.size() - 2]);
                    }
                    if(bestResults[bestResults.size() - 8] == "64641964" || bestResults[bestResults.size() - 8] == "" || bestResults[bestResults.size() - 8] == "0.0.0.0")
                    {
                        ui->DNS3LineEdit->setText("");
                        ui->DNS3LineEdit_2->setText("");
                        ui->DNS3LineEdit_3->setText("");
                        ui->DNS3LineEdit_4->setText("");
                    }
                    else
                    {
                        QStringList D3Parts;
                        D3Parts = bestResults[bestResults.size() - 8].split(".", QString::KeepEmptyParts);

                        if(D3Parts.size() == 4)
                        {
                            ui->DNS3LineEdit->setText(D3Parts.at(0));
                            ui->DNS3LineEdit_2->setText(D3Parts.at(1));
                            ui->DNS3LineEdit_3->setText(D3Parts.at(2));
                            ui->DNS3LineEdit_4->setText(D3Parts.at(3));
                        }
                        //ui->DNS3LineEdit->setText(bestResults[bestResults.size() - 1]);
                    }
                    if(bestResults[bestResults.size() - 7] == "64641964" || bestResults[bestResults.size() - 7] == "" || bestResults[bestResults.size() - 7] == "0.0.0.0")
                    {
                        ui->leDNSSuffix1->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix1->setText(bestResults[bestResults.size() -7]);
                    }
                    if(bestResults[bestResults.size() - 6] == "64641964" || bestResults[bestResults.size() - 6] == "" || bestResults[bestResults.size() - 6] == "0.0.0.0")
                    {
                        ui->leDNSSuffix2->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix2->setText(bestResults[bestResults.size() - 6]);
                    }
                    if(bestResults[bestResults.size() - 5] == "64641964" || bestResults[bestResults.size() - 5] == "" || bestResults[bestResults.size() - 5] == "0.0.0.0")
                    {
                        ui->leDNSSuffix3->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix3->setText(bestResults[bestResults.size() - 5]);
                    }
                    if(bestResults[bestResults.size() - 4] == "64641964" || bestResults[bestResults.size() - 4] == "" || bestResults[bestResults.size() - 4] == "0.0.0.0")
                    {
                        ui->leDNSSuffix4->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix4->setText(bestResults[bestResults.size() - 4]);
                    }
                    if(bestResults[bestResults.size() - 3] == "64641964" || bestResults[bestResults.size() - 3] == "" || bestResults[bestResults.size() - 3] == "0.0.0.0")
                    {
                        ui->leDNSSuffix5->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix5->setText(bestResults[bestResults.size() - 3]);
                    }
                    if(bestResults[bestResults.size() - 2] == "64641964" || bestResults[bestResults.size() - 2] == "" || bestResults[bestResults.size() - 2] == "0.0.0.0")
                    {
                        ui->leDNSSuffix6->setText("");
                    }
                    else
                    {
                        ui->leDNSSuffix6->setText(bestResults[bestResults.size() - 2]);
                    }

                    if(bestResults[bestResults.size() - 1] == "64641964" || bestResults[bestResults.size() - 1] == "" || bestResults[bestResults.size() - 1] == "1")
                    {
                        //LOG_DEBUG( "importNetworkBut_clicked() DHCPHostnameCheckbox->setChecked(true) 3", NULL );
                        ui->DHCPHostnameCheckbox->setChecked(true);
                    }
                    else
                    {
                        //LOG_DEBUG( "importNetworkBut_clicked() DHCPHostnameCheckbox->setChecked(false) 4", NULL );
                        ui->DHCPHostnameCheckbox->setChecked(false);
                    }
                }
            }
        }
        else
        {
            // It is wired
            ui->availableConnectionsTableView->selectRow(0);
        }
        int numAdvancedNetworkSettings = 0;

        if(ui->cbTransferRate->currentIndex() > 0)
        {
            numAdvancedNetworkSettings = numAdvancedNetworkSettings + 1;
        }
        if(!ui->chkboxTransferRateAuto->isChecked())
        {
            numAdvancedNetworkSettings = numAdvancedNetworkSettings + 1;
        }
        if(ui->cbChannel->currentIndex() > 0)
        {
            numAdvancedNetworkSettings = numAdvancedNetworkSettings + 1;
        }

        if(numAdvancedNetworkSettings == 0)
        {
            ui->networkAdvancedBut->setIcon(QIcon());
            ui->networkAdvancedBut->setStyleSheet("");
        }
        else
        {
            ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(numAdvancedNetworkSettings) + ".png"));
        }

        this->ui->alertsLineEdit->setText(tr("Network successfully imported."));
        this->ui->alertsLineEdit->setStyleSheet("color:green;");
    }
    //processingAButton = false;

    //LOG_DEBUG( "importNetworkBut_click() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     E X P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void setupUtility::exportNetworkBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "exportNetworkBut_click" );
    LOG_DEBUG( "exportNetworkBut_click() START", NULL );

    bool exportCurrentNetworkSettings( false );
    bool exportResult( false );

    exportNetworkSettings = true;
    exportHostnameSettings = false;
    exportNTPSettings = false;
    exportNTPRemoveSettings = false;

    // prompt where export settings should save to
    const QStringList myOptions( buildExportConfigurationOptions() );

    //If true, the user selected cancel when opening a file to export to
    if ( myOptions.isEmpty() )
    {
        LOG_WARN( "user did not choose location to save settings", NULL );
        return;
    }

    // for now serving, there are two export options:
    // 1) export the current working scale settings (saved when they hit save/connect)
    // 2) export the settings in the user interface (even if they don't work)

    // SOF-484 No longer require the "SCALE_NOW_SERVING" license to access the
    //         Export dialog ... Always show it
    //if ( session_->uiUtilities->isFeatureLicensed( SCALE_NOW_SERVING, false ) )
    if ( true )
    {
        ExportNetworkSettingsDialog *dialog( new ExportNetworkSettingsDialog( mainWindow_ ) );

        LOG_WARN( "SCALE_NOW_SERVING ENABLED", NULL );

        // prompt!
        if ( QDialog::Accepted != dialog->exec() )
        {
            LOG_WARN( "user cancelled out dialog", NULL );
            return;
        }

        // export the current working scale settings
        else if ( ExportNetworkSettingsDialog::LastKnownWorkingNetworkSettings == dialog->exportType() )
        {
            // nothing to do, drop into method
            LOG_INFO( "export last known...", NULL );
        }

        // export the settings in the user interface
        else if ( ExportNetworkSettingsDialog::CurrentNetworkSettings == dialog->exportType() )
        {
            exportCurrentNetworkSettings = true;
            LOG_INFO( "export current...", NULL );
        }

        exportNetworkSettings = (ExportNetworkSettingsDialog::networkYes == dialog->exportNetwork());
        exportHostnameSettings = (ExportNetworkSettingsDialog::hostnameYes == dialog->exportHostname());
        exportNTPSettings = (ExportNetworkSettingsDialog::ntpYes == dialog->exportNTP());
        exportNTPRemoveSettings = (ExportNetworkSettingsDialog::ntpRemoveYes == dialog->exportNTPRemove());

        LOG_DEBUG( "net %d hostname %d addntp %d removentp %d", exportNetworkSettings, exportHostnameSettings, exportNTPSettings, exportNTPRemoveSettings );
    }

    // EXPORT UI ENTERED VALUES!
    if ( exportCurrentNetworkSettings )
    {
        Network net;

        // get net configuration
        python pythonProcess;
        QStringList config( buildSaveConfigurationOptions( &net ) );
        config.append( "-L" );
        config.append( myOptions.at(1) );

        if ( exportNetworkSettings )
        {
            config.append( "--addNetwork" );
            config.append( "1" );
        }

        // save
        LOG_DEBUG( "click Export call saveConfigs.py", NULL );
        if ( pythonProcess.execCommand( "saveConfigs", config ) )
            exportResult = (0 == pythonProcess.getExitCode());
        LOG_DEBUG( "done with saveConfigs %d", exportResult );

    }
    else //EXPORT LAST KNOWN SAVED SETTINGS WHEN USER PRESSED SAVE/CONNECT
    {
        python pythonProcess;
        // save
        LOG_DEBUG( "click Export call exportConfigs.py", NULL );
        if ( pythonProcess.execCommand( "exportConfigs", myOptions ) )
            exportResult = (0 == pythonProcess.getExitCode());
        LOG_DEBUG( "done with exportConfigs %d", exportResult );
    }

    // EXPORT CERTS!
    if (( ui->wpaEAPTLSPassPhraseFrame->isVisible() ) && ( exportResult ))
    {
        // have to export the certs also
        QStringList certOptions;
        certOptions.append( "-S" );
        certOptions.append( ui->trustedRootStoreCombo->itemText( ui->trustedRootStoreCombo->currentIndex() ) );
        certOptions.append( "-C" );
        certOptions.append( ui->clientCertStoreCombo->itemText( ui->clientCertStoreCombo->currentIndex() ) );
        certOptions.append( "-D" );

        // get the destination path dir for the flash drive
        QString destinationDir;

        if ( 2 == myOptions.size() )
        {
            //filename is at the end
            destinationDir = myOptions.at(1);
            QStringList driveDir;
            driveDir = destinationDir.split("/",QString::KeepEmptyParts);
            destinationDir = "";
            for (int i = 0; i < (driveDir.size()-1) ;i++)
            {
                destinationDir += "/" + driveDir.at(i);
            }
        }

        certOptions.append(destinationDir + "/");

        LOG_DEBUG( "click Export call exportEAPTLSCerts.py", NULL );
        LOG_DEBUG( "destdir '%s'", qPrintable( destinationDir ) );

        // go!
        exportResult = false;

        python pythonProcess;
        if ( pythonProcess.execCommand( "exportEAPTLSCerts", certOptions ) )
            exportResult = (0 == pythonProcess.getExitCode());

        LOG_DEBUG( "done with exportEAPTLSCerts %d", exportResult );
    }

    // update state
    if ( exportResult )
        ui->alertsLineEdit->setText( tr("Network configuration exported.") );
    else
        ui->alertsLineEdit->setText( tr("Network configuration not exported.") );

    LOG_DEBUG( "exportNetworkBut_click() END %d", exportResult );
}

bool setupUtility::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}
///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   E X P O R T   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList setupUtility::buildExportConfigurationOptions() {
   // processingAButton = true;
    QStringList options;

    //QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"/CHOOSE/THIS/DIRECTORY");
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName = "";
    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    QStringList folders;

#if defined( Q_OS_LINUX )
    folders.push_back( "/media/thumb" );
    folders.push_back( "/media" );
#elif defined( Q_OS_WIN )
    folders.push_back( "D:\\" );
    folders.push_back( "E:\\" );
    folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    dialog.setAcceptMode( QFileDialog::AcceptSave );
    dialog.setWindowTitle( tr( "Backup File" ) );
    dialog.setDirectory( startDirStr );
    dialog.setFilter( formatStr );
    dialog.setOptions( QFileDialog::DontConfirmOverwrite );
    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            fileName = selected[0] ;
    }

    if(fileName.isEmpty()) {//user clicked cancel
        return options;
    }

    options.append("-F");
    options.append(fileName);


    return options;
}

 //////////////////////////////////
//
//     B U I L D   N E T W O R K   C O M M A N D   O P T I O N S
//
///////////////////////////////////
QStringList setupUtility::buildNetworkCommandOptions(Network *locNetwork)
{
    LOG_CLASS_INFO( "setupUtility", "buildNetworkCommandOptions" );
    LOG_DEBUG( "buildNetworkCommandOptions() START", NULL );

    processingAButton = true;

    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    QStringList options;

    options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));

    options.append("-c"); // isconnected

    if(locNetwork->isConnected())
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isConnected is TRUE", NULL );
        options.append("1");
    }
    else
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isConnected is FALSE", NULL );
        options.append("0");
    }
    options.append("-I"); // Ip Address

    options.append(locNetwork->IPAddress());

    options.append("-s"); // isStaticIP

    if(locNetwork->isStatic())
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isStatic is TRUE", NULL );
        options.append("1");
    }
    else
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isStatic is FALSE", NULL );
        options.append("0");
    }

    options.append("-D"); // isAutoDNS
    if(locNetwork->isAutoDNS())
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isAutoDNS is TRUE", NULL );
        options.append("1");
    }
    else
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isAutoDNS is FALSE", NULL );
        options.append("0");
    }

    options.append("-B"); // subnet mask addr
    options.append(locNetwork->SubnetMask());

    options.append("-G"); // gateway addr
    options.append(locNetwork->Gateway());

    options.append("-1"); // primary dns server
    options.append(locNetwork->DNS1());

    options.append("-2"); // secondary dns server
    options.append(locNetwork->DNS2());

    options.append("-3"); // tertiary dns server
    options.append(locNetwork->DNS3());

    options.append("-W"); // isWireless
    if(locNetwork->isWireless())
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isWireless is TRUE", NULL );
        options.append("1");
        isWiredConnection_Fix = false;
    }
    else
    {
        //LOG_DEBUG( "buildNetworkCommandOptions() isWireless is FALSE", NULL );
        options.append("0");
        isWiredConnection_Fix = true;
    }

    options.append("-A");
    options.append(locNetwork->WPAPassPhrase());

    options.append("-S"); // SSID name
    options.append(locNetwork->SSID());

    options.append("-M"); // security method
    options.append(locNetwork->SecurityMethod());


    options.append("-E"); // encryption algorithm
    options.append(locNetwork->Encryption());

    options.append("-V"); // VPN Username
    options.append(locNetwork->VPNUsername());

    options.append("-P"); // VPN Password
    options.append(locNetwork->VPNPassword());

    options.append("-v");
    if(locNetwork->isVPN())
    {
        options.append("1");
    }
    else
    {
        options.append("0");
    }

    options.append("-w");

    if(ui->wepKey1LineEdit->text() != "")
    {
        options.append(ui->wepKey1LineEdit->text());
    }
    else
    {
        options.append("0");
    }

    options.append("-x");
    if(ui->wepKey2LineEdit->text() != "")
    {
        options.append(ui->wepKey2LineEdit->text());
    }
    else
    {
        options.append("0");
    }
    options.append("-y");
    if(ui->wepKey3LineEdit->text() != "")
    {
        options.append(ui->wepKey3LineEdit->text());
    }
    else
    {
        options.append("0");
    }
    options.append("-z");
    if(ui->wepKey4LineEdit->text() != "")
    {
        options.append(ui->wepKey4LineEdit->text());
    }
    else
    {
        options.append("0");
    }
    options.append("-N");
    options.append(locNetwork->EAPUsername());

    options.append("-n");
    options.append(locNetwork->EAPPassword());

    options.append("-K"); //default key
    if(this->ui->wirelessAuthenticationDefaultKey1Radio->isChecked())
    {
        options.append("1");
    }
    else if(this->ui->wirelessAuthenticationDefaultKey2Radio->isChecked())
    {
        options.append("2");
    }
    else if(this->ui->wirelessAuthenticationDefaultKey3Radio->isChecked())
    {
        options.append("3");
    }
    else if(this->ui->wirelessAuthenticationdefaultKey4Radio->isChecked())
    {
        options.append("4");
    }

    bool clearCertificateFromScaleSettings = false;
    //LYZ EAP-TLS cert filenames
    if(locNetwork->serverCertFilename() != "")
    {
        //copyCertsFiles();
        //changeCertFilesToUse();
        options.append("-L");
        options.append(locNetwork->serverCertFilename()); // this is trusted root
        dbServerCertFilename = locNetwork->serverCertFilename();
    }
    else
    {
        clearCertificateFromScaleSettings = true;
        dbServerCertFilename = "";
    }
    if(locNetwork->chainFilename() != "")
    {
        options.append("-Y");
        options.append(locNetwork->chainFilename());
        dbChainFilename = locNetwork->chainFilename();
    }
    else
    {
        clearCertificateFromScaleSettings = true;
        dbChainFilename = "";
    }
    if(locNetwork->clientCertFilename() != "")
    {
        options.append("-Z");
       // options.append(locNetwork->clientCertFilename());

        //enabled for 2.4.0 bug below
        options.append(getSerialNumber());


        //dbClientCertFilename = locNetwork->clientCertFilename();
        //dbClientCertFilename = getSerialNumber();

        //diabled for 2.40 bug below
        //options.append(this->ui->clientCertStoreCombo->itemData(this->ui->clientCertStoreCombo->currentIndex()).toString());
        dbClientCertFilename = this->ui->clientCertStoreCombo->itemData(this->ui->clientCertStoreCombo->currentIndex()).toString();
    }
    else
    {
        clearCertificateFromScaleSettings = true;
        dbClientCertFilename = "";
    }

    if(clearCertificateFromScaleSettings || ( ui->securityCombo->currentIndex() != 2 && ui->securityCombo->currentIndex() != 6))
    {
        clearCertificateInfoFromScaleSettings();
    }

    // Do not set network, just requesting Access Points
    options.append("-X");
    options.append("0");
    options.append("-a");
    options.append(ui->cbTransferRate->itemData(ui->cbTransferRate->currentIndex()).toString());
    options.append("-b");
    if(ui->chkboxTransferRateAuto->isChecked())
    {
        options.append("1");
    }
    else
    {
        options.append("0");
    }

    options.append("-d");
    if(ui->cbChannel->currentIndex() < 0)
    {
        options.append("auto");
    }
    else
    {
        options.append(ui->cbChannel->itemData(ui->cbChannel->currentIndex()).toString());
    }

    ///////////////////////////////////////////////
    ///  DNS SUFFIXES
    ///

    if(ui->leDNSSuffix1->text().length() > 0)
    {
        options.append("-5");
        options.append(ui->leDNSSuffix1->text());
    }
    else
    {
        options.append("-5");
        options.append("");
    }
    if(ui->leDNSSuffix2->text().length() > 0)
    {
        options.append("-6");
        options.append(ui->leDNSSuffix2->text());
    }
    else
    {
        options.append("-6");
        options.append("");
    }
    if(ui->leDNSSuffix3->text().length() > 0)
    {
        options.append("-7");
        options.append(ui->leDNSSuffix3->text());
    }
    else
    {
        options.append("-7");
        options.append("");
    }
    if(ui->leDNSSuffix4->text().length() > 0)
    {
        options.append("-8");
        options.append(ui->leDNSSuffix4->text());
    }
    else
    {
        options.append("-8");
        options.append("");
    }
    if(ui->leDNSSuffix5->text().length() > 0)
    {
        options.append("-9");
        options.append(ui->leDNSSuffix5->text());
    }
    else
    {
        options.append("-9");
        options.append("");
    }
    if(ui->leDNSSuffix6->text().length() > 0)
    {
        options.append("-4");
        options.append(ui->leDNSSuffix6->text());
    }
    else
    {
        options.append("-4");
        options.append("");
    }

    QString networkCommandsFileName = "settingswizard/networkCommandsAtBoot.txt";

    if( TESTING )
        networkCommandsFileName = "C:/hobart/settingswizard/networkCommandsAtBoot.txt";

    QString networkCommandsAtBootFileLocation = filesystem::absoluteFilePath(networkCommandsFileName, folderType::DBs);
    QFile file(networkCommandsAtBootFileLocation);

    QFile fOut(networkCommandsAtBootFileLocation);
       if (fOut.open(QFile::WriteOnly | QFile::Text))
       {
            QTextStream s(&fOut);
            for (int i = 0; i < options.size(); ++i)
                s << '"' + options.at(i) + '"' << ' ';
            fOut.close();

       }
       else
       {
            ;
       }

   for (QStringList::iterator it = options.begin();
        it != options.end(); ++it)
   {
       QString current = *it;
       qDebug() << "[[" << current << "]]";
       LOG_DEBUG( "options() %s", current.toLatin1().constData() );
   }

    LOG_DEBUG( "buildNetworkCommandOptions() END", NULL );

    return options;
}
//
//////////////////////////////////
//
//     B U I L D   N E T W O R K   C O M M A N D   O P T I O N S
//
///////////////////////////////////
QStringList setupUtility::buildNetworkCommandOptionsNew(Network *locNetwork)
{
   LOG_CLASS_INFO( "setupUtility", "buildNetworkCommandOptionsNew" );
   LOG_DEBUG( "buildNetworkCommandOptionsNew() START", NULL );

   processingAButton = true;

   // Load the commonNetwork to the App first
   this->setCommonNetwork(locNetwork);

   QStringList options;

    options.append("-c"); // isconnected
   if(locNetwork->isConnected())
   {
       options.append("1");
   }
   else
   {

       options.append("0");
   }
   options.append("-I"); // Ip Address

   options.append(locNetwork->IPAddress());

   options.append("-s"); // isStaticIP

   if(locNetwork->isStatic())
   {
       options.append("1");
   }
   else
   {
       options.append("0");
   }

   options.append("-D"); // isAutoDNS
   if(locNetwork->isAutoDNS())
   {
       options.append("1");
   }
   else
   {
       options.append("0");
   }

   options.append("-B"); // subnet mask addr
   options.append(locNetwork->SubnetMask());

   options.append("-G"); // gateway addr
   options.append(locNetwork->Gateway());

   options.append("-1"); // primary dns server
   options.append(locNetwork->DNS1());

   options.append("-2"); // secondary dns server
   options.append(locNetwork->DNS2());

   options.append("-3"); // tertiary dns server
   options.append(locNetwork->DNS3());

   options.append("-W"); // isWireless
   if(locNetwork->isWireless())
   {
       options.append("1");
   }
   else
   {
       options.append("0");
   }

   options.append("-A");
   options.append(locNetwork->WPAPassPhrase());

   options.append("-S"); // SSID name
   options.append(locNetwork->SSID());

   options.append("-M"); // security method
   options.append(locNetwork->SecurityMethod());


   options.append("-E"); // encryption algorithm
   options.append(locNetwork->Encryption());

   options.append("-V"); // VPN Username
   options.append(locNetwork->VPNUsername());

   options.append("-P"); // VPN Password
   options.append(locNetwork->VPNPassword());

   options.append("-v");
   if(locNetwork->isVPN())
   {
       options.append("1");
   }
   else
   {
       options.append("0");
   }

   options.append("-w");

   if(ui->wepKey1LineEdit->text() != "")
   {
       options.append(ui->wepKey1LineEdit->text());
   }
   else
   {
       options.append("0");
   }

   options.append("-x");
   if(ui->wepKey2LineEdit->text() != "")
   {
       options.append(ui->wepKey2LineEdit->text());
   }
   else
   {
       options.append("0");
   }
   options.append("-y");
   if(ui->wepKey3LineEdit->text() != "")
   {
       options.append(ui->wepKey3LineEdit->text());
   }
   else
   {
       options.append("0");
   }
   options.append("-z");
   if(ui->wepKey4LineEdit->text() != "")
   {
       options.append(ui->wepKey4LineEdit->text());
   }
   else
   {
       options.append("0");
   }
   options.append("-N");
   options.append(locNetwork->EAPUsername());

   options.append("-n");
   options.append(locNetwork->EAPPassword());

   options.append("-K"); //default key
   if(this->ui->wirelessAuthenticationDefaultKey1Radio->isChecked())
   {
       options.append("1");
   }
   else if(this->ui->wirelessAuthenticationDefaultKey2Radio->isChecked())
   {
       options.append("2");
   }
   else if(this->ui->wirelessAuthenticationDefaultKey3Radio->isChecked())
   {
       options.append("3");
   }
   else if(this->ui->wirelessAuthenticationdefaultKey4Radio->isChecked())
   {
       options.append("4");
   }

   bool clearCertificateFromScaleSettings = false;
   //LYZ EAP-TLS cert filenames
   if(locNetwork->serverCertFilename() != "")
   {
       //copyCertsFiles();
       //changeCertFilesToUse();
       options.append("-L");
       options.append(locNetwork->serverCertFilename()); // this is trusted root
       dbServerCertFilename = locNetwork->serverCertFilename();
   }
   else
   {
       clearCertificateFromScaleSettings = true;
       dbServerCertFilename = "";
   }
   if(locNetwork->chainFilename() != "")
   {
       options.append("-Y");
       options.append(locNetwork->chainFilename());
       dbChainFilename = locNetwork->chainFilename();
   }
   else
   {
       clearCertificateFromScaleSettings = true;
       dbChainFilename = "";
   }
   if(locNetwork->clientCertFilename() != "")
   {
       options.append("-Z");
      // options.append(locNetwork->clientCertFilename());
       options.append(getSerialNumber());
       //dbClientCertFilename = locNetwork->clientCertFilename();
       dbClientCertFilename = getSerialNumber();
   }
   else
   {
       clearCertificateFromScaleSettings = true;
       dbClientCertFilename = "";
   }

   if(clearCertificateFromScaleSettings || ( ui->securityCombo->currentIndex() != 2 && ui->securityCombo->currentIndex() != 6))
   {
       clearCertificateInfoFromScaleSettings();
   }

   // Do not set network, just requesting Access Points
   options.append("-X");
   options.append("0");

   for (QStringList::iterator it = options.begin();
        it != options.end(); ++it)
   {
       QString current = *it;
       qDebug() << "[[" << current << "]]";
       LOG_DEBUG( "options() %s", current.toLatin1().constData() );
   }

   LOG_DEBUG( "buildNetworkCommandOptionsNew() END", NULL );

   return options;
}

void setupUtility::changeCertFilesToUse()
{


    //if(isLinux())
    {
       python pythonProcess;
        QStringList options;
        options.clear();

        if (pythonProcess.execCommand("deleteOldTrustedRootCert", options))
           {
               if (pythonProcess.getExitCode() == 0)
               {
                    ;                   //TBD - updateCustomerVolume();
               }
               else
               {
                   ;// TODO:
                   // what do we do here?
                   //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
               }
           }
           else
           {
               ;
           }
        python certProcess;

        options.clear();
        options.append("-S");
        QString rootFile = "settingswizard/TRUSTEDROOTSTORE/" + ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex());
        if( TESTING )
            rootFile = "C:/hobart/settingswizard/TRUSTEDROOTSTORE/" + ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex());

        QString trustedRootPath = filesystem::absoluteFilePath(rootFile, folderType::DBs);
        options.append(trustedRootPath);
        options.append("-C");
        QString clientFile = "settingswizard/CLIENTCERTSTORE/" + ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex());
        if( TESTING )
            clientFile = "C:/hobart/settingswizard/CLIENTCERTSTORE/" + ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex());

        QString clientCertPath = filesystem::absoluteFilePath(clientFile, folderType::DBs);
        options.append(clientCertPath);
        options.append("-D");
        options.append(getSerialNumber());
        options.append("-P");
        options.append(ui->leClientKeyPassword->text());

        if (certProcess.execCommand("changeEAPTLSCerts", options))
        {
            if (certProcess.getExitCode() == 0)
            {
                updateDB("clientCertUsersFilename", ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
                updateDB("serverCertFilename", ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
            }
            else
            {
                ;// TODO:
            }
        }
        else
        {
            ;// TODO:
        }
        certProcess.cleanup();
    }
}

void setupUtility::clearCertificateInfoFromScaleSettings()
{
    updateDB("serverCertFilename","");
    updateDB("chainFilename","");
    updateDB("clientCertFilename","");
    updateDB("clientCertUsersFilename","");
}

///////////////////////////////////////
//
//       A U T O   D N S  R A D I O
//
///////////////////////////////////

QString setupUtility::modelName()
{
    /* OLD CODE */

    return "Hobart HTi";
}

///////////////////////////////////////
//
//       A U T O   D N S  R A D I O
//
///////////////////////////////////

void setupUtility::autoDNSRadio_click()
{
    LOG_CLASS_INFO( "setupUtility", "autoDNSRadio_click" );
    LOG_DEBUG( "autoDNSRadio_click() START", NULL );

    if(this->ui->autoDNSRadio->isChecked())
    {
        this->ui->DNS1LineEdit->setEnabled(false);
        this->ui->DNS1LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_2->setEnabled(false);
        this->ui->DNS1LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_3->setEnabled(false);
        this->ui->DNS1LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_4->setEnabled(false);
        this->ui->DNS1LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS2LineEdit->setEnabled(false);
        this->ui->DNS2LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_2->setEnabled(false);
        this->ui->DNS2LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_3->setEnabled(false);
        this->ui->DNS2LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_4->setEnabled(false);
        this->ui->DNS2LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS3LineEdit->setEnabled(false);
        this->ui->DNS3LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_2->setEnabled(false);
        this->ui->DNS3LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_3->setEnabled(false);
        this->ui->DNS3LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_4->setEnabled(false);
        this->ui->DNS3LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
    }
    else
    {
        this->ui->DNS1LineEdit->setEnabled(true);
        this->ui->DNS1LineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_2->setEnabled(true);
        this->ui->DNS1LineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_3->setEnabled(true);
        this->ui->DNS1LineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_4->setEnabled(true);
        this->ui->DNS1LineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS2LineEdit->setEnabled(true);
        this->ui->DNS2LineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_2->setEnabled(true);
        this->ui->DNS2LineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_3->setEnabled(true);
        this->ui->DNS2LineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_4->setEnabled(true);
        this->ui->DNS2LineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS3LineEdit->setEnabled(true);
        this->ui->DNS3LineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_2->setEnabled(true);
        this->ui->DNS3LineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_3->setEnabled(true);
        this->ui->DNS3LineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_4->setEnabled(true);
        this->ui->DNS3LineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
    }
    //LOG_DEBUG( "autoDNSRadio_click() END", NULL );
}

///////////////////////////////////////
//
//       M A N U A L   D N S   R A D I O
//
///////////////////////////////////

void setupUtility::manualDNSRadio_click()
{
    LOG_CLASS_INFO( "setupUtility", "manualDNSRadio_click" );
    LOG_DEBUG( "manualDNSRadio_click() START", NULL );

    if(this->ui->manualDNSRadio->isChecked())
    {
        this->ui->DNS1LineEdit->setEnabled(true);
        this->ui->DNS1LineEdit->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_2->setEnabled(true);
        this->ui->DNS1LineEdit_2->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_3->setEnabled(true);
        this->ui->DNS1LineEdit_3->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_4->setEnabled(true);
        this->ui->DNS1LineEdit_4->setStyleSheet("color:black; background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS2LineEdit->setEnabled(true);
        this->ui->DNS2LineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_2->setEnabled(true);
        this->ui->DNS2LineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_3->setEnabled(true);
        this->ui->DNS2LineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_4->setEnabled(true);
        this->ui->DNS2LineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS3LineEdit->setEnabled(true);
        this->ui->DNS3LineEdit->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_2->setEnabled(true);
        this->ui->DNS3LineEdit_2->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_3->setEnabled(true);
        this->ui->DNS3LineEdit_3->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_4->setEnabled(true);
        this->ui->DNS3LineEdit_4->setStyleSheet("color:black;background : rgb(255, 255, 255);border-radius: 10px; border: 2px solid gray;");
    }
    else
    {
        this->ui->DNS1LineEdit->setEnabled(false);
        this->ui->DNS1LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_2->setEnabled(false);
        this->ui->DNS1LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_3->setEnabled(false);
        this->ui->DNS1LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS1LineEdit_4->setEnabled(false);
        this->ui->DNS1LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS2LineEdit->setEnabled(false);
        this->ui->DNS2LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_2->setEnabled(false);
        this->ui->DNS2LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_3->setEnabled(false);
        this->ui->DNS2LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS2LineEdit_4->setEnabled(false);
        this->ui->DNS2LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");

        this->ui->DNS3LineEdit->setEnabled(false);
        this->ui->DNS3LineEdit->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_2->setEnabled(false);
        this->ui->DNS3LineEdit_2->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_3->setEnabled(false);
        this->ui->DNS3LineEdit_3->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
        this->ui->DNS3LineEdit_4->setEnabled(false);
        this->ui->DNS3LineEdit_4->setStyleSheet("color:rgb(0, 0, 0);background : rgb(240, 240, 240);border-radius: 10px; border: 2px solid gray;");
    }
    //LOG_DEBUG( "manualDNSRadio_click() END", NULL );
}

void setupUtility::setTimeTab()
{
    ui->wirelessAdapterGroupBox_2->hide();
    this->ui->timeEdit->setTime(QTime::currentTime());

    if(isAstronomical == "0")
    {
        this->ui->astronomicalCheckBox->setChecked(false); // this means to use a 12 hour clock
    }
    else
    {
        this->ui->astronomicalCheckBox->setChecked(true);  // this means to use a 24 hour clock
    }
    this->astronomicalCheckboxValueChanged(isAstronomical.toInt());


    ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(0)));
    ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(1)));
    ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(2)));
    ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(3)));
    this->ui->dateEdit->setSelectedDate(QDate::currentDate());

    ui->dateEdit->showSelectedDate();

    //ui->alertsLineEdit->setText(QString::number(debugMonth) + "/" + QString::number(debugDay) + "/" + QString::number(debugYear));

    //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
    //ui->alertsLineEdit->setText(ui->dateEdit->);
    this->setDateTimeText();
    //  this->ui->ntpCurrentDateLabel->setText(QDate::currentDate().toString(""));
    //ui->ntpCurrentDateLabel

    this->setTimezone();

    ui->ntpServer->setText(dbNTPServer);
    ui->ntpFrequencyCombo->setCurrentIndex(dbNTPFrequency.toInt());
    m_originalTimeEditStyleSheet = "color:black; background: rgb(238, 255, 211);";

    for(int i = 0; i < ui->ntpDayOfWeekCombo->count(); i++)
        ui->ntpDayOfWeekCombo->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    //ui->ntpDayOfWeekCombo->setEditable(true);
    //ui->ntpDayOfWeekCombo->lineEdit()->setReadOnly(true);
    //ui->ntpDayOfWeekCombo->lineEdit()->setAlignment(Qt::AlignCenter);
    //ui->ntpDayOfWeekCombo->lineEdit()->setFont(ui->ntpDayOfWeekCombo->font());
    for(int i = 0; i < ui->ntpFrequencyCombo->count(); i++)
        ui->ntpFrequencyCombo->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);

    if(ui->ntpFrequencyCombo->currentIndex() == 2 || ui->ntpFrequencyCombo->currentIndex() == 1)
    {
        //ui->alertsLineEdit->setText("grater than 1");
        ui->ntpDayOfWeekCombo->setEnabled(false);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        //ui->alertsLineEdit->setText("frequency is weekly");
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
    //ui->ntpFrequencyCombo->setEditable(true);
    //ui->ntpFrequencyCombo->lineEdit()->setReadOnly(true);
    //ui->ntpFrequencyCombo->lineEdit()->setAlignment(Qt::AlignCenter);
    //ui->ntpFrequencyCombo->lineEdit()->setFont(ui->ntpFrequencyCombo->font());
    //ui->ntpFrequencyCombo->lineEdit()->setStyleSheet("color: black; background: rgb(238,255,211);");
    ui->ntpTimeZoneCombo->view()->setStyleSheet("QComboBox#ntpTimeZoneCombo{ color: black;border-radius: 5px; border: 1px solid black; background: rgb(238, 255, 211); text-align: center;} QComboBox#ntpTimeZoneCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }  QComboBox#ntpTimeZoneCombo::!editable{  color: black;  border-radius: 5px; border: 1px solid black;  background: rgb(238, 255, 211);  text-align: center;   }  QComboBox#ntpTimeZoneCombo::editable{ color: black; border-radius: 5px;  border: 1px solid black;  background: rgb(238, 255, 211); text-align: center; }");
    ui->ntpServer->setText(dbNTPServer);

    if (dbNTPFrequency.toInt() > 0)
    {
        ui->ntpDayOfWeekCombo->setDisabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {  border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        ui->ntpDayOfWeekCombo->setCurrentIndex(-1);
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setCurrentIndex(dbNTPDayOfWeek.toInt());
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down {border: 1px solid gray; border-radius: 4px; border-image: url(:/images/screens/DropDownButton.png) 0 0 0 0 stretch stretch; background-color: transparent; }");
        fixComboBoxStyleSheet(ui->ntpDayOfWeekCombo);
    }
 //   ui->alertsLineEdit->setText(dbNTPFrequency + ":" + dbNTPDayOfWeek + ":" + dbNTPTimeOfDay);
    //ui->ntpDayOfWeekCombo->setCurrentIndex(dbNTPDayOfWeek.toInt());
    if(dbNTPTimeZone == "America/Detroit") dbNTPTimeZone = "America/New_York";
    ui->ntpTimeZoneCombo->setCurrentIndex(ui->ntpTimeZoneCombo->findData(dbNTPTimeZone));

    if(dbTimeKeeper != "Manual") ui->timeTabWidget->setCurrentIndex(2);
    else ui->timeTabWidget->setCurrentIndex(0);

    QStringList timeParts = dbNTPTimeOfDay.split(":", QString::KeepEmptyParts);

    dbNTPTimeOfDay = dbNTPTimeOfDay + " " + dbNTPAntiMeridian;
    QTime ntpTime = QTime::fromString(dbNTPTimeOfDay, "hh:mm:ss A");
    ui->ntpTimeEdit->setTime(ntpTime);
    ui->ntpHourBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(0)));
    ui->ntpMinuteBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(1)));
    ui->ntpSecondBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(2)));
    ui->ntpAmpmBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(3)));
    ui->timeTabWidget->setTabEnabled(1, false);
    //ui->alertsLineEdit->setText(QString::number(debugMonth) + "/" + QString::number(debugDay) + "/" + QString::number(debugYear));

}

void setupUtility::setHostnameBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "setHostnameBut_click" );
    LOG_DEBUG( "setHostnameBut_click() START", NULL );

    processingAButton = true;
    QString m_correctText;
    QRegExp regExp;
    regExp.setPattern("^([a-zA-Z]+[a-zA-Z0-9\\-\\.]*[a-zA-Z0-9]+)|([a-zA-Z])$");

    if(ui->hostnameLineEdit->text() != "")
    {
        if(regExp.exactMatch(ui->hostnameLineEdit->text()))
        {
            //m_correctText = text;    // Correct text so far..
            //QToolTip::hideText();
            LOG_DEBUG( "hostname true", NULL );
            ui->setHostnameBut->setEnabled(true);
            updateDB("hostTab", "1");
            updateDB("hostname",ui->hostnameLineEdit->text());
            ui->mainTab->setTabIcon(4, passIcon);

            python pythonProcess;
            QStringList options;
            options.append(this->ui->hostnameLineEdit->text());
            if (pythonProcess.execCommand("setHostname", options))
            {
                if (pythonProcess.getExitCode() == 0)
                {
                    ui->alertsLineEdit->setText(tr("Hostname set. System restart is required."));
                    changedHostname = true;
                }
                else
                {
                    // TODO:
                }
            }
            else
            {
                // TODO:
                // What do we do here?
            }
        }
        else
        {
            //QPoint point = QPoint(ui->hostnameLineEdit->geometry().left(),
            //                  geometry().top() + ui->hostnameLineEdit->geometry().bottom());

            //ui->hostnameLineEdit->setText(m_correctText);   // Reset previous text..
            //QToolTip::showText(point,tr("Invalid hostname..."));
            ui->setHostnameBut->setEnabled(false);

            ui->hostnameLabel->setAlignment(Qt::AlignHCenter);
            ui->hostnameLabel->setText(hostnamePromptText);
        }
    }
    processingAButton = false;
    //LOG_DEBUG( "setHostnameBut_click() END", NULL );
}

void setupUtility::syncHostname()
{
    LOG_CLASS_INFO( "setupUtility", "syncHostname" );
    LOG_DEBUG( "syncHostname() START", NULL );

    //m_correctText = text;    // Correct text so far..
    //QToolTip::hideText();
    LOG_DEBUG( "setHostnameBut_click() START", NULL );
    ui->setHostnameBut->setEnabled(true);
    updateDB("hostTab", "1");
    ui->mainTab->setTabIcon(4, passIcon);

    python pythonProcess;
    QStringList options;

    if (pythonProcess.execCommand("syncHostname", options))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            //ui->alertsLineEdit->setText(tr("Hostname synced. System restart is required."));
            //changedHostname = true;
        }
        else
        {
            // TODO:
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }


    //TODO set line edit hostname now

    updateDB("hostname",ui->hostnameLineEdit->text());

    //LOG_DEBUG( "syncHostname() END", NULL );
}


void setupUtility::setScalePasswordBut_click()
{
    QString password( ui->scalePasswordLineEdit->text() );

    if ( password.isEmpty() )
        return;

    processingAButton = true;

    // change password of user hobart
    python pythonProcess;
    QStringList options;
    options.append( "hobart" );
    options.append( password );

    if ( pythonProcess.execCommand( "setPassword", options ) )
        if ( 0 == pythonProcess.getExitCode() )
        {
            ui->alertsLineEdit->setText( tr( "Scale password has been set." ) );

            ui->setScalePasswordBut->setEnabled( false );
        }

    processingAButton = false;
}

void setupUtility::rebootScale()
{
    //processingAButton = false;
    python pythonProcess;

    if (pythonProcess.execCommand("reboot"))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            //ui->alertsLineEdit->setText("Device will now reboot");
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcessode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }
}

void setupUtility::getUIHostname()
{
    LOG_CLASS_INFO( "setupUtility", "getUIHostname" );
    LOG_DEBUG( "getUIHostname() START", NULL );

    //processingAButton = true;
    QString hostnameFileName;
    if(PLATFORM != platformType::Windows )
    {
        hostnameFileName = "/etc/hostname";
        //    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, filesystem::DBs);

        QFile file(hostnameFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         {
           // processingAButton = false;

            return;
        }
        QStringList theLine;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            this->ui->hostnameLineEdit->setText(line);
            m_correctText = line;
            this->initialHostname = line;

        }
        file.close();
    }
   // processingAButton = false;

    /*
    */
    /*
    QProcess getHostnameProcess;
    QStringList options;
    options.append("python/qmSettings/system/qmSettingsGetHostname.py");

    getHostnameProcess.start("python", options);

    if(!getHostnameProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        // Start configuration error.
        //return;
    }

    if(!getHostnameProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
        //return "";
        // Finish configuration error.
    }
*/
    else
    {
        // it is windows
        QStringList args;
        python pythonProcess;

        if ( pythonProcess.execCommand("getHostname", args, false, false, ( 30 * 1000 ) ) )
        {
            if ( pythonProcess.getExitCode() == 0 )
            {
                QString hostname;
                hostname = pythonProcess.readStdOut();
                this->ui->hostnameLineEdit->setText(hostname);
                m_correctText = hostname;
                this->initialHostname = hostname;
            }
        }

    }

    //LOG_DEBUG( "getUIHostname() END", NULL );
}

/*
void setupUtility::getConnectionStatus()
{
    QProcess getConnectionStatusProcess;
    QStringList options;
    options.append(filesystem::absoluteFilePath("getConnectionStatus.py",folderType::Python));
    getConnectionStatusProcess.start("python", options);

    if(!getConnectionStatusProcess.waitForStarted())
    {
        ;//Start configuration error.
    }

    if(!getConnectionStatusProcess.waitForFinished())
    {
        ;
    }

    QStringList connectionStatuses;
    QByteArray result = getConnectionStatusProcess.readAll();
    QString connectionStatus(result);
    connectionStatuses = connectionStatuses.mid(0,connectionStatuses.length()-1);
    connectionStatuses = connectionStatus.split(",", QString::SkipEmptyParts);

    // your local vars from output
    QString isConnected = connectionStatuses[0];
    QString strength = connectionStatuses[1];
    QString deviceType = connectionStatuses[2];

    ////qDebug() << isConnected;
    ////qDebug() << strength;
    ////qDebug() << deviceType;
}
*/

void setupUtility::getUIMACAddress()
{
    LOG_CLASS_INFO( "setupUtility", "getUIMACAddress" );
    LOG_DEBUG( "getUIMACAddress() START", NULL );

    //processingAButton = true;

    QProcess macAddressProcess;
    QStringList options;
    options.append(filesystem::absoluteFilePath("getMac.py",folderType::Python));

    macAddressProcess.start("python", options);

    if(!macAddressProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
    }

    if(!macAddressProcess.waitForFinished())
    {
        ;
    }

    QStringList macAddresses;
    QByteArray result = macAddressProcess.readAll();
    QString macAddress(result);
    macAddress = macAddress.mid(0,macAddress.length()-1);

    //qDebug() << "total string is:" << macAddress;
    // added in a string length check
    if(macAddress.size() > 0)
    {
        macAddresses = macAddress.split("|", QString::SkipEmptyParts);
        this->ui->ethernetMACAddressLabel->setText((macAddresses[0]).toUpper());
        this->ui->wirelessMACAddressLabel->setText((macAddresses[1]).toUpper());
        this->ui->wireMAClbl->setText((macAddresses[0]).toUpper());
        this->ui->wifiMAClbl->setText((macAddresses[1]).toUpper());
    }

    //processingAButton = false;
    //LOG_DEBUG( "getUIMACAddress() END", NULL );
}

void setupUtility::setTimezone()
{
    /*
    QProcess getTimezoneProcess;
    QStringList options;
    //options.append("getZone.py");
    options.append(filesystem::absoluteFilePath("getZone.py",folderType::Python));

    getTimezoneProcess.start("python", options);

    if(!getTimezoneProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        // Start configuration error.
        //return;
    }

    if(!getTimezoneProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
        //return "";
        // Finish configuration error.
    }

    QByteArray result = getTimezoneProcess.readAll();
    QString timezone(result);
    timezone = timezone.mid(0,timezone.length()-1);
    this->ui->alertsLineEdit->setText(timezone);

    //now the mapping
    if(timezone == "GMT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(0);
    }
    else if(timezone == "CET")
    {
        this->ui->timeZoneCombo->setCurrentIndex(1);
    }
    else if(timezone == "EET")
    {
        this->ui->timeZoneCombo->setCurrentIndex(2);
    }
    else if(timezone == "MSK")
    {
        this->ui->timeZoneCombo->setCurrentIndex(3);
    }
    else if(timezone == "GST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(4);
    }
    else if(timezone == "PKT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(5);
    }
    else if(timezone == "BDT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(6);
    }
    else if(timezone == "WIT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(7);
    }
    else if(timezone == "CST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(8);
    }
    else if(timezone == "JST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(9);
    }
    else if(timezone == "EST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(10);
    }
    else if(timezone == "NCT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(11);
    }
    else if(timezone == "NZDT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(12);
    }
    else if(timezone == "CVT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(13);
    }
    else if(timezone == "UTC")
    {
        this->ui->timeZoneCombo->setCurrentIndex(14);
    }
    else if(timezone == "WGT")
    {
        this->ui->timeZoneCombo->setCurrentIndex(15);
    }
    else if(timezone == "AST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(16);
    }
    else if(timezone == "EST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(17);
    }
    else if(timezone == "CST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(18);
    }
    else if(timezone == "MST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(20);
    }
    else if(timezone == "PST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(21);
    }
    else if(timezone == "AKST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(22);
    }
    else if(timezone == "HST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(23);
    }
    else if(timezone == "SST")
    {
        this->ui->timeZoneCombo->setCurrentIndex(24);
    }
    */

    if(dbTimezone == "America/Detroit") dbTimezone = "America/New_York";
    ui->timeZoneCombo->setCurrentIndex(ui->timeZoneCombo->findData(dbTimezone));
    initialTimeZone = this->ui->timeZoneCombo->currentText();
    ui->ntpTimeZoneCombo->setCurrentIndex(ui->ntpTimeZoneCombo->findData(dbNTPTimeZone));
}

/*
bool MainWindow::getDBAstronomicalSetting()
{
    // default value to using 12 hour clock; most common use
    bool isNonAstronomical = true;

    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");
    if(!database->open())
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
    }
    else
    {
        // Get volume from qmsettingsvolume table and load
        QSqlQuery query;

        // Update the single row in the table
        query.exec("select isnonastronomical from qmsettingsdate");
        while (query.next())
        {
            if(query.value(0).toInt() != 0)
            {
                isNonAstronomical = true; // DB 24 hour
            }
            else
            {
                isNonAstronomical = false; // DB 12 hour
            }
        }
    }
    database->close();

    return isNonAstronomical;

}
*/
//////////////////////////////////////////////////////
//
//     S E T   V O L U M E   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::setVolumeBut_click()
{
    processingAButton = true;
    bool saveit = false;
    dbValue = ui->volumeLabel->text();
    if (this->ui->operatorSoundBut->isChecked())
    {
        updateDB("operatorSoundTab", "1");
        setTheOperatorSound = true;
        soundDevice = "operator";
        dbKey = "oVolume";
        // Use setBeeper.py
        python pythonProcess;

        // Set backlight of both displays to max
        QStringList args;

        //args.append(QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55)));

        if(ui->volumeSpinBox->value() == 1)
        {
            args.append("128");
        }
        else
        {
            args.append("0");
        }
        //////qDebug() << " operator volume is: " << QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55));
        if (pythonProcess.execCommand("sampleBeep", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ui->alertsLineEdit->setText(tr("Operator volume successfully set."));
                saveit = true;
                    //TBD - updateCustomerVolume();
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }


    }
    else if(this->ui->customerSoundBut->isChecked())
    {
        updateDB("customerSoundTab", "1");
        setTheCustomerSound = true;
        soundDevice = "customer";
        dbKey = "cVolume";
        // use setVolume.py
        python pythonProcess;

        // Set backlight of both displays to max
        QStringList args;

        args.append(ui->volumeLabel->text());
        //        ////qDebug() << "customer volume is: " << ui->volumeLabel->text();

        if (pythonProcess.execCommand("setVolume", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                ui->alertsLineEdit->setText(tr("Customer volume successfully set."));
                saveit = true;
                //TBD - updateCustomerVolume();

                // FT-1501: save off customer volume into database
                session_->send( ConfigurationDev, CfgGCDSave, ",,," + ui->volumeLabel->text() );
            }
            else
            {
                // TODO:
                // what do we do here?
                //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            // TODO:
            // What do we do here?
        }
    }
    if(saveit) updateDB(dbKey, dbValue);

    if(setTheCustomerSound && setTheOperatorSound) ui->mainTab->setTabIcon(1, passIcon);
    processingAButton = false;
}

//////////////////////////////////////////////////////
//
//       P R I N T   P I N G   O U T P U T
//
//////////////////////////////////////////////////////

void setupUtility::printPingOutput()
{
    QString output = this->pingProcess->readAllStandardOutput();
    //////qDebug() << "standard output is:" << output;
    this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
}

void setupUtility::readyReadStandardError()
{
     QString output = this->pingProcess->readAllStandardError();
     this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
     pingProcess->kill();
     this->ui->startPingBut->setEnabled(true);
}

void setupUtility::finishedProcess(int value, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    pingProcess->kill();
    this->ui->startPingBut->setEnabled(true);
    processingAButton = false;

}

//////////////////////////////////////////////////////
//
//     S T A R T   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::startPingBut_click()
{
    LOG_CLASS_INFO( "setupUtility", "startPingBut_click" );
    LOG_DEBUG( "startPingBut_click() START", NULL );

    processingAButton = true;
    this->ui->startPingBut->setEnabled(false);
    this->ui->pingResultsList->clear();
    cancelPing = false;
    QStringList options;
    options.append(filesystem::absoluteFilePath("outputPing.py",folderType::Python));
    options.append(this->ui->pingIPLineEdit->text());
    pingProcess->waitForStarted(100);
    pingProcess->start("python", options);

    //LOG_DEBUG( "startPingBut_click() END", NULL );
}

//////////////////////////////////////////////////////
//
//     S T O P   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::stopPingBut_click()
{
      this->pingProcess->kill();
    processingAButton = false;

}

bool setupUtility::updateDB(QString key, QString value)
{
    LOG_CLASS_INFO( "setupUtility", "updateDB" );
    LOG_DEBUG( "updateDB() START with key: %s AND value: %s", key.toLatin1().constData(), value.toLatin1().constData() );

    //processingAButton = true;
    QString databaseFileName = "settingswizard/scalesettings";
    QString databaseTempFileName = "settingswizard/scalesettingstemp";
    QString databaseBackupFileName = "settingswizard/scalesettingsBACKUP";

    if( TESTING )
    {
        databaseFileName = "C:/hobart/settingswizard/scalesettings";
        databaseTempFileName = "C:/hobart/settingswizard/scalesettingstemp";
        databaseBackupFileName = "C:/hobart/settingswizard/scalesettingsBACKUP";
    }

    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
    QString databaseTempFileLocation = filesystem::absoluteFilePath(databaseTempFileName, folderType::DBs);
    QString databaseBackupFileLocation = filesystem::absoluteFilePath(databaseBackupFileName, folderType::DBs);

    QFile file(databaseFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //  processingAButton = false;
        LOG_DEBUG( "updateDB() return !file.open FALSE 1", NULL );
        return false;
    }

    QStringList theLine;
    QStringList fileContents;
    QTextStream in(&file);
    QString line;
    fileContents.clear();

    while (!in.atEnd())
    {
        line = in.readLine();
        theLine = line.split("=", QString::KeepEmptyParts);
        if(theLine.at(0) == key)
        {
            line = key + "=" + value;
        }
        fileContents.append(line);
    }

    file.close();
    QFile fileTemp(databaseTempFileLocation);
    if (!fileTemp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        //  processingAButton = false;
        //LOG_DEBUG( "updateDB() return !file.open FALSE 2", NULL );
        return false;
    }
    QTextStream outStream(&fileTemp);

    for(int i = 0; i < fileContents.count(); i++)
    {
        outStream << fileContents.at(i) + "\n";
    }

    fileTemp.close();
    QDir dir;
    dir.remove(databaseFileLocation);
    if (QFile::exists(databaseBackupFileLocation))
    {
        dir.remove(databaseBackupFileLocation);
    }
    QFile::copy(databaseTempFileLocation,databaseBackupFileLocation);
    dir.rename(databaseTempFileLocation,databaseFileLocation);

    python pythonProcess;

    // Set backlight of both displays to max
    QStringList args;

    //args.append(QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55)));

    //////qDebug() << " operator volume is: " << QString::number(static_cast<int>(this->ui->volumeLabel->text().toDouble() * 2.55));
    if (pythonProcess.execCommand("sync", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
             ;
        }
        else
        {
            ;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }
    // processingAButton = false;

    //LOG_DEBUG( "updateDB() END with key: %s AND value: %s", key.toLatin1().constData(), value.toLatin1().constData() );

    return true;
}

///////////////////////////////////////////////////////////
//
//     S E T   D A T E / T I M E   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////

void setupUtility::setNtpDateTimeBut_click()
{
    //settingTime = true;
    processingAButton = true;
    //ui->setNtpBut->hide();
    ui->setNtpBut->setEnabled(false);
    saveNTP2DB();
    updateDB("timeTab", "1");
    updateDB("timeKeeper", "NTP");
    QString msg = "120080,3";
    shouldChangeNTP = true;
    if(originalNTPTimeZoneComboBoxIndex != ui->ntpTimeZoneCombo->currentIndex())
    {
        QString msg = "120080,3";
        session_->send(Error, ErrVersionManagerDevice, msg);
    }
    else
    {
        changeTimeAfterServiceKeyRemoved();
    }
    //changeTimeAfterServiceKeyRemoved();
    ui->setDateTimeBut->show();
   // qApp->processEvents();

    //ui->hourBut->setFocus();

}

void setupUtility::setDateTimeBut_click()
{
    //ui->setDateTimeBut->hide();
    // Build command
//    QString command = "python";
    ui->setDateTimeBut->setEnabled(false);
    //ui->setDateTimeBut->repaint();

    // Let's reboot the scale if this button is clicked
    // first check for service key
    //settingTime = true;

    processingAButton = true;
    updateDB("timeTab", "1");
    updateDB("timeKeeper","Manual");
    shouldChangeTime = true;


    if(originalTimeZoneComboBoxIndex != ui->timeZoneCombo->currentIndex())
    {
        QString msg = "120080,3";
        session_->send(Error, ErrVersionManagerDevice, msg);
    }
    else
    {
        changeTimeAfterServiceKeyRemoved();
    }
    ui->setNtpBut->show();
   // qApp->processEvents();
    //ui->hourBut->setFocus();
}

void setupUtility::changeTimeAfterServiceKeyRemoved()
{
    processingAButton = true;
    //   ui->setDateTimeBut->hide();
    //   ui->setNtpBut->hide();
    bool need2Reboot = false;
    bool errorOccured = false;
    //qDebug() << "trying to set time";
    LOG_CLASS_INFO( "setupUtility", "setNTPSettings" );

    if(ui->timeTabWidget->currentIndex() == 0)
    {
        // First get rid of any cronjobs for NTP


        python pythonProcess;
        QStringList args;
        args.clear();
        args.append("-R");
        args.append("1");
        args.append("-S");
        args.append("/usr/local/hobart/python/updateTimeNTP.py");
        if(pythonProcess.execCommand("updateCrontab",args))
        {
            if(pythonProcess.getExitCode() == 0)
            {
                ;
            }
        }

       // now set the time
        args.clear();
        args.append("-t");
        args.append(ui->timeZoneCombo->itemData(ui->timeZoneCombo->currentIndex()).toString());

        // Erase ntp settings
        updateDB("ntpServer","0.0.0.0");
        updateDB("ntpFrequency","0");
        updateDB("ntpDayOfWeek","0");
        updateDB("ntpTimeOfDay","03:30:00");
        updateDB("ntpTimeZone","America/New_York");
        updateDB("ntpAntiMeridian","AM");

        if (pythonProcess.execCommand("setTimezone",args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                need2Reboot = true;
            }
            else
            {
                need2Reboot = false;
                errorOccured = true;
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
                this->ui->alertsLineEdit->repaint();
            }
        }

        args.clear();

        QStringList options;

        QString timeOffset = "0";
        QString timeOffsetSign = "-";
        this->updateDB("timezone", this->ui->timeZoneCombo->itemData(this->ui->timeZoneCombo->currentIndex()).toString());

        options.append("-Y"); //year
        options.append(QString::number(this->ui->dateEdit->selectedDate().year()));
        options.append("-M"); // month
        options.append(QString("%1").arg(this->ui->dateEdit->selectedDate().month() ,2,10, QChar('0')).toUpper());
        options.append("-D"); //day
        options.append(QString("%1").arg(this->ui->dateEdit->selectedDate().day() ,2,10, QChar('0')).toUpper());
        options.append("-H"); // hour
        options.append(QString("%1").arg(this->ui->timeEdit->time().hour(),2,10, QChar('0')).toUpper());
        options.append("-m"); // minute
        options.append(QString("%1").arg(this->ui->timeEdit->time().minute(),2,10, QChar('0')).toUpper());
        options.append("-S"); // second
        options.append(QString("%1").arg(this->ui->timeEdit->time().second(),2,10, QChar('0')).toUpper());
        options.append("-t"); // text of timezone e.g., America/New_York
        options.append(QString("%1").arg(this->ui->timeZoneCombo->itemData(this->ui->timeZoneCombo->currentIndex()).toString()));
        options.append("-A");
        options.append("0");

        //qApp->processEvents();

        python NTPTimezoneProcess;

        //qApp->flush();

        //ui->keepAliveManual->click();
        if (NTPTimezoneProcess.execCommand("setTime",options))
        {
            if (NTPTimezoneProcess.getExitCode() == 0)
            {
                //QTime::restart();
                //qApp->flush();
                need2Reboot = true;
                this->ui->ntpTimeZoneCombo->setCurrentIndex(ui->timeZoneCombo->currentIndex());
                updateDB("timezone", (ui->timeZoneCombo->itemData(ui->timeZoneCombo->currentIndex()).toString()));
                updateDB("ntpTimeZone", (ui->timeZoneCombo->itemData(ui->timeZoneCombo->currentIndex()).toString()));
                //qApp->processEvents();
                ui->alertsLineEdit->setText(tr("Time, Date and Timezone set."));
                ui->alertsLineEdit->repaint();

            }
            else
            {
                need2Reboot = false;
                errorOccured = true;
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
                this->ui->alertsLineEdit->repaint();
            }
        }

    }
    else if(ui->timeTabWidget->currentIndex() == 2)
    {
        //ui->keepAliveBut->click();
        // Keep the alternate time type initialized.
        updateDB("isAstronomical","0");
        updateDB("timezone","America/New_York");
        updateDB("isAstronomical","0");
        updateDB("timezone","America/New_York");

        python pythonProcess;
        QStringList options;
        QString timeOffset = "0";
        QString timeOffsetSign = "-";
        options.append("-S"); //ntp Server (text of server ip or domainname)
        options.append(ui->ntpServer->text());
        options.append("-F"); // ntp frequency 0=weekly 1=monthly 2=daylightSaving and Standard Time
        options.append(QString::number(ui->ntpFrequencyCombo->currentIndex()));
        options.append("-D"); //ntp dayOfWeek 0=Sunday 1=Monday 2=Tuesday 3=Wednesday 4=Thursday 5=Friday 6=Saturday
        options.append(QString::number(ui->ntpDayOfWeekCombo->currentIndex()));

        options.append("-H"); // ntp timeOfDay hour
        options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().hour(),2,10, QChar('0')).toUpper());
        options.append("-M"); // ntp timeOfDay minute
        options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().minute(),2,10, QChar('0')).toUpper());
        options.append("-S"); // ntp timeOfDay second
        options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().second(),2,10, QChar('0')).toUpper());

        options.append("-T"); // NTP text of timezone e.g., America/New_York
        options.append(QString("%1").arg(this->ui->ntpTimeZoneCombo->itemData(this->ui->ntpTimeZoneCombo->currentIndex()).toString()));

        options.clear();
        options.append("-M"); // ntp timeOfDay minute
        options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().minute(),2,10, QChar('0')).toUpper());
        options.append("-F");
        options.append(QString::number(ui->ntpFrequencyCombo->currentIndex()));
        options.append("-D"); //ntp dayOfWeek 0=Sunday 1=Monday 2=Tuesday 3=Wednesday 4=Thursday 5=Friday 6=Saturday
        options.append(QString::number(ui->ntpDayOfWeekCombo->currentIndex()));
        options.append("-H"); // ntp timeOfDay hour
        options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().hour(),2,10, QChar('0')).toUpper());
        options.append("-T"); // NTP text of timezone e.g., America/New_York
        options.append(QString("%1").arg(this->ui->ntpTimeZoneCombo->itemData(this->ui->ntpTimeZoneCombo->currentIndex()).toString()));

        if (pythonProcess.execCommand("setNTP",options))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                need2Reboot = true;
            }
            else
            {
                need2Reboot = false;
                errorOccured = true;
                ui->ntpCurrentTimeLabel->setText("setntp failed");
            }
        }
        //Set that timezone first
        QStringList args;
        args.clear();

        args.append("-t");
        args.append(ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString());

        if (pythonProcess.execCommand("setTimezone",args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                need2Reboot = true;
            }
            else
            {
                need2Reboot = false;
                errorOccured = true;
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
                this->ui->alertsLineEdit->repaint();
                ui->ntpCurrentTimeLabel->setText("timezone failed");
            }
        }

        // Costco does not allow third-party devices -- which includes our scales -- to send
        // ICMP echoes across their network, so this is one solution that is customer-/build-agnostic.
        if (ui->ntpPingFirstCheckbox->isChecked())
        {
            LOG_DEBUG("Ping-first checkbox is checked; attempting to ping NTP server...", NULL );
            ui->alertsLineEdit->setText(tr("Pinging NTP server; please wait..."));
            ui->alertsLineEdit->repaint();
            args.clear();
            args.append("-c");
            args.append("1");
            args.append(ui->ntpServer->text());

            if (pythonProcess.execCommand("ping",args,false,false, 20 * 1000))
            {
                    if (pythonProcess.getExitCode() != 0)
                    {
                        ui->alertsLineEdit->setText(tr("Cannot get time from NTP server; ping failed"));
                        ui->alertsLineEdit->repaint();

                        ui->setDateTimeBut->setEnabled(true); // Enable the buttons so the user can try again
                        ui->setNtpBut->setEnabled(true);

                        settingTime = false;
                        processingAButton = false;
                        shouldChangeTime = false;
                        shouldChangeNTP = false;
                        ui->setDateTimeBut->show();
                        ui->setNtpBut->show();
                        return; // Bailing out of the method; no point in proceeding if we need an ICMP echo first
                    }
            }
        }
        ui->alertsLineEdit->setText(tr("Sending packet to NTP server; please wait..."));
        ui->alertsLineEdit->repaint();

        args.clear();
        args.append(ui->ntpServer->text());

                python NTPTimezoneProcess;

        if (NTPTimezoneProcess.execCommand("ntp",args, false, false, 10 * 1000))
        {
            if (NTPTimezoneProcess.getExitCode() == 0)
            {
                QString ntpTime = NTPTimezoneProcess.readStdOut();
                QStringList ntpTimeValues = ntpTime.split(QChar(' '));

                if(ntpTimeValues.count() > 5)
                {
                    QString newHour = "0";

                    // Calculate hour offset that
                    //QString ntpHourOffset = "0";
                    options.clear();
                    args.clear();

                    args.append("-Y");
                    args.append(ntpTimeValues.at(2));
                    args.append("-M");
                    args.append(ntpTimeValues.at(0));
                    args.append("-D");
                    args.append(ntpTimeValues.at(1));
                    args.append("-H");
                    args.append(ntpTimeValues.at(3));
                    args.append("-m");
                    args.append(ntpTimeValues.at(4));
                    args.append("-S");
                    args.append(ntpTimeValues.at(5));
                    args.append("-t");
                    args.append(ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString());
                    args.append("-A");
                    args.append("0");

                    if (NTPTimezoneProcess.execCommand("setTime",args))
                    {
                        if (NTPTimezoneProcess.getExitCode() == 0)
                        {
                            need2Reboot = true;
                            this->ui->timeZoneCombo->setCurrentIndex(ui->ntpTimeZoneCombo->currentIndex());
                            this->ui->timeEdit->setTime(QTime::currentTime());
                            this->ui->dateEdit->setSelectedDate(QDate::currentDate());
                            ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(0)));
                            ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(1)));
                            ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(2)));
                            ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(3)));

                            this->ui->alertsLineEdit->setText(tr("Time set."));
                            this->ui->alertsLineEdit->repaint();
                            ui->ntpCurrentTimeLabel->setText("Time set");

                            updateDB("timezone", (ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString()));
                            updateDB("ntpTimeZone", (ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString()));
                        }
                        else
                        {
                            need2Reboot = false;
                            errorOccured = true;
                            this->ui->alertsLineEdit->setText(tr("Cannot set time."));
                            this->ui->alertsLineEdit->repaint();
                            ui->ntpCurrentTimeLabel->setText("set time failed");
                        }
                    }
                }
                else
                {
                    need2Reboot = false;
                }
            }
            else
            {
                //cannot get time from ntp server
                need2Reboot = false;
                errorOccured = true;
                this->ui->alertsLineEdit->setText(tr("Cannot get time from NTP server."));
                this->ui->alertsLineEdit->repaint();
                ui->ntpCurrentTimeLabel->setText("ntp failed");
            }
        }
        else
        {
            need2Reboot = false;
            errorOccured = true;
            this->ui->alertsLineEdit->setText(tr("Cannot get time from NTP server."));
            this->ui->alertsLineEdit->repaint();
        }
    }

    needToReboot = need2Reboot;
    if(need2Reboot)
    {
        if ( !errorOccured )
        {
            ui->alertsLineEdit->setText(tr("Time set."));
            ui->alertsLineEdit->repaint();
        }
        processingAButton = false;
        shouldChangeTime = false;
        shouldChangeNTP = false;
        ui->setDateTimeBut->show();
        ui->setNtpBut->show();
        if(((originalTimeZoneComboBoxIndex != ui->timeZoneCombo->currentIndex()) && (ui->timeTabWidget->currentIndex() == 0)) || ((ui->timeTabWidget->currentIndex() != 0) && (ui->ntpTimeZoneCombo->currentIndex()) != originalNTPTimeZoneComboBoxIndex))
        {
            this->reboot();
        }
    }
    else
    {
        {
            ui->alertsLineEdit->setText(tr("Time set."));
            ui->alertsLineEdit->repaint();
        }
        ui->setDateTimeBut->show();
        ui->setNtpBut->show();
        ui->USBGroupbox->hide();
        session_->send(Error,ErrRemoveAnyModalDialogDevice,"120080,3");   // remove dialogbox
    }

    currentUserDate = ui->dateEdit->selectedDate();
    tempDayOfMonth = currentUserDate.day();
    ui->setDateTimeBut->setEnabled(true);
    ui->setNtpBut->setEnabled(true);

    settingTime = false;
    processingAButton = false;
    shouldChangeTime = false;
    shouldChangeNTP = false;
    ui->setDateTimeBut->show();
    ui->setNtpBut->show();
    //qApp->processEvents();
}

//////////////////////////////////////////////////////
//
//        F I L L    T I M E Z O N E S
//
//////////////////////////////////////////////////////
// get the reference to QComboBox for filling
void setupUtility::fillTimeZones()
{
    // TODO:
    // We should put mapping of Province,Country to timezone file path in the path script
    // This would be more generic throughout the system (manufacturing test and settings wizard)

    this->ui->timeZoneCombo->clear();
     this->ui->timeZoneCombo->addItem("(UTC+00:00) Dublin, Edinburgh, Lisbon, London",QString("Europe/London"));
     this->ui->timeZoneCombo->addItem("(UTC+01:00) Brussels, Copenhagen, Madrid, Paris",QString("Europe/Paris"));
     this->ui->timeZoneCombo->addItem("(UTC+02:00) Cairo",QString("Africa/Cairo"));
     this->ui->timeZoneCombo->addItem("(UTC+03:00) Moscow, St. Petersburg, Volgograd",QString("Europe/Moscow"));
     this->ui->timeZoneCombo->addItem("(UTC+04:00) Abu Dhabi, Muscat",QString("Asia/Muscat"));
     this->ui->timeZoneCombo->addItem("(UTC+05:00) Islamabad, Karachi",QString("Asia/Karachi"));
     this->ui->timeZoneCombo->addItem("(UTC+06:00) Dhaka",QString("Asia/Dhaka"));
     this->ui->timeZoneCombo->addItem("(UTC+07:00) Bangkok, Hanoi, Jakarta",QString("Asia/Jakarta"));
     this->ui->timeZoneCombo->addItem("(UTC+08:00) Taipei",QString("Asia/Taipei"));
     this->ui->timeZoneCombo->addItem("(UTC+09:00) Osaka, Sapporo, Tokyo",QString("Asia/Tokyo"));
     this->ui->timeZoneCombo->addItem("(UTC+10:00) Brisbane",QString("Australia/Brisbane"));
     this->ui->timeZoneCombo->addItem("(UTC+11:00) Solomon Is., New Caledonia",QString("Pacific/Noumea"));
     this->ui->timeZoneCombo->addItem("(UTC+12:00) Auckland, Wellington",QString("Pacific/Auckland"));
     this->ui->timeZoneCombo->addItem("(UTC-01:00) Cape Verde Is.",QString("Atlantic/Cape_Verde"));
     this->ui->timeZoneCombo->addItem("(UTC-02:00) Mid-Atlantic",QString("Atlantic/Noronha"));
     this->ui->timeZoneCombo->addItem("(UTC-02:00) Brazil DeNoronha",QString("Brazil/DeNoronha"));
     this->ui->timeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
     this->ui->timeZoneCombo->addItem("(UTC-03:00) Buenos Aires",QString("America/Buenos_Aires"));
     this->ui->timeZoneCombo->addItem("(UTC-03:00) Brazil East",QString("Brazil/East"));
	 this->ui->timeZoneCombo->addItem("(UTC-03:00) Santiago",QString("America/Santiago"));
     this->ui->timeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
     this->ui->timeZoneCombo->addItem("(UTC-04:00) Brazil West",QString("Brazil/West"));
     this->ui->timeZoneCombo->addItem("(UTC-04:30) Caracas",QString("America/Caracas"));
     this->ui->timeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/New_York"));
     this->ui->timeZoneCombo->addItem("(UTC-05:00) Bogota",QString("America/Bogota"));
     this->ui->timeZoneCombo->addItem("(UTC-05:00) Lima",QString("America/Lima"));
     this->ui->timeZoneCombo->addItem("(UTC-05:00) Panama",QString("America/Panama"));
     this->ui->timeZoneCombo->addItem("(UTC-05:00) Brazil Acre",QString("Brazil/Acre"));
     this->ui->timeZoneCombo->addItem("(UTC-06:00) Central Time (US & Canada)",QString("America/Chicago"));
     this->ui->timeZoneCombo->addItem("(UTC-06:00) Costa Rica",QString("America/Costa_Rica"));
     this->ui->timeZoneCombo->addItem("(UTC-06:00) Guatemala",QString("America/Guatemala"));
     this->ui->timeZoneCombo->addItem("(UTC-06:00) Mexico General",QString("Mexico/General"));
     this->ui->timeZoneCombo->addItem("(UTC-07:00) Arizona",QString("America/Phoenix"));
     this->ui->timeZoneCombo->addItem("(UTC-07:00) Mountain Time (US & Canada)",QString("America/Denver"));
     this->ui->timeZoneCombo->addItem("(UTC-07:00) Mexico BajaSur",QString("Mexico/BajaSur"));
     this->ui->timeZoneCombo->addItem("(UTC-08:00) Pacific Time (US & Canada)",QString("America/Los_Angeles"));
     this->ui->timeZoneCombo->addItem("(UTC-08:00) Mexico BajaNorte",QString("Mexico/BajaNorte"));
     this->ui->timeZoneCombo->addItem("(UTC-09:00) Alaska",QString("America/Anchorage"));
     this->ui->timeZoneCombo->addItem("(UTC-10:00) Hawaii",QString("Pacific/Honolulu"));
     this->ui->timeZoneCombo->addItem("(UTC-11:00) Coordinated Universal Time-11",QString("Pacific/Midway"));
     // "Habitations in this time zone use neighboring timezone based on political owner or region." http://en.wikipedia.org/wiki/UTC%E2%88%9212:00
     // so Linux does not have an entry for it, reassign to Pacific/Midway
     this->ui->timeZoneCombo->addItem("(UTC-12:00) International Date Line West",QString("Pacific/Midway"));

     // NTP
     this->ui->ntpTimeZoneCombo->clear();
     this->ui->ntpTimeZoneCombo->addItem("(UTC+00:00) Dublin, Edinburgh, Lisbon, London",QString("Europe/London"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+01:00) Brussels, Copenhagen, Madrid, Paris",QString("Europe/Paris"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+02:00) Cairo",QString("Africa/Cairo"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+03:00) Moscow, St. Petersburg, Volgograd",QString("Europe/Moscow"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+04:00) Abu Dhabi, Muscat",QString("Asia/Muscat"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+05:00) Islamabad, Karachi",QString("Asia/Karachi"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+06:00) Dhaka",QString("Asia/Dhaka"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+07:00) Bangkok, Hanoi, Jakarta",QString("Asia/Jakarta"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+08:00) Taipei",QString("Asia/Taipei"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+09:00) Osaka, Sapporo, Tokyo",QString("Asia/Tokyo"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+10:00) Brisbane",QString("Australia/Brisbane"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+11:00) Solomon Is., New Caledonia",QString("Pacific/Noumea"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+12:00) Auckland, Wellington",QString("Pacific/Auckland"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-01:00) Cape Verde Is.",QString("Atlantic/Cape_Verde"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-02:00) Mid-Atlantic",QString("Atlantic/Noronha"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-02:00) Brazil DeNoronha",QString("Brazil/DeNoronha"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Buenos Aires",QString("America/Buenos_Aires"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Brazil East",QString("Brazil/East"));
	 this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Santiago",QString("America/Santiago"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:00) Brazil West",QString("Brazil/West"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:30) Caracas",QString("America/Caracas"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/New_York"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Bogota",QString("America/Bogota"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Lima",QString("America/Lima"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Panama",QString("America/Panama"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Brazil Acre",QString("Brazil/Acre"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Central Time (US & Canada)",QString("America/Chicago"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Costa Rica",QString("America/Costa_Rica"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Guatemala",QString("America/Guatemala"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Mexico General",QString("Mexico/General"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Arizona",QString("America/Phoenix"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Mountain Time (US & Canada)",QString("America/Denver"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Mexico BajaSur",QString("Mexico/BajaSur"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-08:00) Pacific Time (US & Canada)",QString("America/Los_Angeles"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-08:00) Mexico BajaNorte",QString("Mexico/BajaNorte"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-09:00) Alaska",QString("America/Anchorage"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-10:00) Hawaii",QString("Pacific/Honolulu"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-11:00) Coordinated Universal Time-11",QString("Pacific/Midway"));
     // "Habitations in this time zone use neighboring timezone based on political owner or region." http://en.wikipedia.org/wiki/UTC%E2%88%9212:00
     // so Linux does not have an entry for it, reassign to Pacific/Midway
     this->ui->ntpTimeZoneCombo->addItem("(UTC-12:00) International Date Line West",QString("Pacific/Midway"));


    //////////XXXXXXXXXXXXXXX
   /* this->ui->timeZoneCombo->clear();
    this->ui->timeZoneCombo->addItem("(UTC+00:00) Dublin, Edinburgh, Lisbon, London",QString("Europe/London"));
    this->ui->timeZoneCombo->addItem("(UTC+01:00) Brussels, Copenhagen, Madrid, Paris",QString("Europe/Paris"));
    this->ui->timeZoneCombo->addItem("(UTC+02:00) Cairo",QString("Africa/Cairo"));
    this->ui->timeZoneCombo->addItem("(UTC+03:00) Moscow, St. Petersburg, Volgograd",QString("Europe/Moscow"));
    this->ui->timeZoneCombo->addItem("(UTC+04:00) Abu Dhabi, Muscat",QString("Asia/Muscat"));
    this->ui->timeZoneCombo->addItem("(UTC+05:00) Islamabad, Karachi",QString("Asia/Karachi"));
    this->ui->timeZoneCombo->addItem("(UTC+06:00) Dhaka",QString("Asia/Dhaka"));
    this->ui->timeZoneCombo->addItem("(UTC+07:00) Bangkok, Hanoi, Jakarta",QString("Asia/Jakarta"));
    this->ui->timeZoneCombo->addItem("(UTC+08:00) Taipei",QString("Asia/Taipei"));
    this->ui->timeZoneCombo->addItem("(UTC+09:00) Osaka, Sapporo, Tokyo",QString("Asia/Tokyo"));
    this->ui->timeZoneCombo->addItem("(UTC+10:00) Brisbane",QString("Australia/Brisbane"));
    this->ui->timeZoneCombo->addItem("(UTC+11:00) Solomon Is., New Caledonia",QString("Pacific/Noumea"));
    this->ui->timeZoneCombo->addItem("(UTC+12:00) Auckland, Wellington",QString("Pacific/Auckland"));
    this->ui->timeZoneCombo->addItem("(UTC-01:00) Cape Verde Is.",QString("Atlantic/Cape_Verde"));
    this->ui->timeZoneCombo->addItem("(UTC-02:00) Mid-Atlantic",QString("Atlantic/Noronha"));
    this->ui->timeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
    this->ui->timeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
    this->ui->timeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/New_York"));
    this->ui->timeZoneCombo->addItem("(UTC-06:00) Central Time (US & Canada)",QString("America/Chicago"));
    this->ui->timeZoneCombo->addItem("(UTC-07:00) Arizona",QString("America/Phoenix"));
    this->ui->timeZoneCombo->addItem("(UTC-07:00) Mountain Time (US & Canada)",QString("America/Denver"));
    this->ui->timeZoneCombo->addItem("(UTC-08:00) Pacific Time (US & Canada)",QString("America/Los_Angeles"));
    this->ui->timeZoneCombo->addItem("(UTC-09:00) Alaska",QString("America/Anchorage"));
    this->ui->timeZoneCombo->addItem("(UTC-10:00) Hawaii",QString("Pacific/Honolulu"));
    this->ui->timeZoneCombo->addItem("(UTC-11:00) Coordinated Universal Time-11",QString("Pacific/Midway"));
    // "Habitations in this time zone use neighboring timezone based on political owner or region." http://en.wikipedia.org/wiki/UTC%E2%88%9212:00
    // so Linux does not have an entry for it, reassign to Pacific/Midway
    this->ui->timeZoneCombo->addItem("(UTC-12:00) International Date Line West",QString("Pacific/Midway"));

     // NTP
     this->ui->ntpTimeZoneCombo->clear();
     this->ui->ntpTimeZoneCombo->addItem("(UTC+00:00) Dublin, Edinburgh, Lisbon, London",QString("Europe/London"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+01:00) Brussels, Copenhagen, Madrid, Paris",QString("Europe/Paris"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+02:00) Cairo",QString("Africa/Cairo"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+03:00) Moscow, St. Petersburg, Volgograd",QString("Europe/Moscow"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+04:00) Abu Dhabi, Muscat",QString("Asia/Muscat"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+05:00) Islamabad, Karachi",QString("Asia/Karachi"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+06:00) Dhaka",QString("Asia/Dhaka"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+07:00) Bangkok, Hanoi, Jakarta",QString("Asia/Jakarta"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+08:00) Taipei",QString("Asia/Taipei"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+09:00) Osaka, Sapporo, Tokyo",QString("Asia/Tokyo"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+10:00) Brisbane",QString("Australia/Brisbane"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+11:00) Solomon Is., New Caledonia",QString("Pacific/Noumea"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC+12:00) Auckland, Wellington",QString("Pacific/Auckland"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-01:00) Cape Verde Is.",QString("Atlantic/Cape_Verde"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-02:00) Mid-Atlantic",QString("Atlantic/Noronha"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-02:00) Brazil DeNoronha",QString("Brazil/DeNoronha"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Buenos Aires",QString("America/Buenos_Aires"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Brazil East",QString("Brazil/East"));
	 this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Santiago",QString("America/Santiago"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:00) Brazil West",QString("Brazil/West"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-04:30) Caracas",QString("America/Caracas"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/New_York"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Bogota",QString("America/Bogota"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Lima",QString("America/Lima"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Panama",QString("America/Panama"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Brazil Acre",QString("Brazil/Acre"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Central Time (US & Canada)",QString("America/Chicago"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Costa Rica",QString("America/Costa_Rica"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Guatemala",QString("America/Guatemala"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Mexico General",QString("Mexico/General"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Arizona",QString("America/Phoenix"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Mountain Time (US & Canada)",QString("America/Denver"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Mexico BajaSur",QString("Mexico/BajaSur"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-08:00) Pacific Time (US & Canada)",QString("America/Los_Angeles"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-08:00) Mexico BajaNorte",QString("Mexico/BajaNorte"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-09:00) Alaska",QString("America/Anchorage"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-10:00) Hawaii",QString("Pacific/Honolulu"));
     this->ui->ntpTimeZoneCombo->addItem("(UTC-11:00) Coordinated Universal Time-11",QString("Pacific/Midway"));
     // "Habitations in this time zone use neighboring timezone based on political owner or region." http://en.wikipedia.org/wiki/UTC%E2%88%9212:00
     // so Linux does not have an entry for it, reassign to Pacific/Midway
     this->ui->ntpTimeZoneCombo->addItem("(UTC-12:00) International Date Line West",QString("Pacific/Midway"));
    */
}

//////////////////////////////////////////////////////
//
//       E N C R Y P T
//
//////////////////////////////////////////////////////
/*
QString MainWindow::encrypt(QString password)
{
    // using Sha1 method...this can be changed here.
     QCryptographicHash hash(QCryptographicHash::Sha1);
     hash.addData(password.toUtf8());
     return hash.result();
}
*/
//////////////////////////////////////////////////////
//
//      A L E R T S   C U R R E N T   I N D E X
//
//            C H A N G E D
//
//////////////////////////////////////////////////////

void setupUtility::currentChangedSlot(int index)
{
    LOG_CLASS_INFO( "setupUtility", "currentChangedSlot START" );

    if( index != 3)
    {
        if(mustTurnOnDialogs)
        {
            session_->send(SessionDev , SnRestartDialogs);
            mustTurnOnDialogs = false;
        }
    }
    QDate highlightedDate;
    highlightedDate = ui->dateEdit->selectedDate();


    switch (index)
    {
        case 0:
            this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
            this->ui  ->setBrightnessBut->setFocus();
            break;
        case 1:
            this->ui->alertsLineEdit->setText(tr("Change Sound Settings"));
            this->ui->setVolumeBut->setFocus();
            //this->ui->alertsLineEdit->setText(QString::number(ui->volumeSpinBox->value()));
            if(isSoundTabFirstTime)
            {
                isSoundTabFirstTime = false;
                // load the correct operator volume
                QString operatorVolume = getDBValue("oVolume");
                if(operatorVolume == "on")
                {
                    this->ui->volumeSpinBox->setValue(1);
                    this->ui->volumeLabel->setText(tr("on"));
                }
                else
                {
                    this->ui->volumeSpinBox->setValue(0);
                    this->ui->volumeLabel->setText(tr("off"));
                }

            }
            break;
            //this->ui->alertsLineEdit->setText(QString::number(ui->volumeSpinBox->value()) + " " + operatorVolume);
        case 2:
            this->ui->alertsLineEdit->setText(tr("Change Time Settings"));


            //ui->alertsLineEdit->setText(QString::number(ui->dateEdit->selectedDate().month()) + "/" + QString::number(ui->dateEdit->selectedDate().day()) + "/" + QString::number(ui->dateEdit->selectedDate().year()));
            //this->ui->setDateTimeBut->setFocus();
            if(isTimeTabFirstTime)
            {
                isTimeTabFirstTime = false;
                ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->HourSection));
                ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->MinuteSection));

                ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->SecondSection));
                ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->AmPmSection));
                this->update();
            }
            ui->dateEdit->setSelectedDate(highlightedDate);
            break;
        case 3:
            //mustTurnOnDialogs = true;
            //session_->send(SessionDev , SnStopDialogs);
            if(!networkTabAlreadyLoaded)
            {
                //LOG_CLASS_INFO( "setupUtility", "case 3 call loadNetworkTab()" );
                loadNetworkTab();
                networkTabAlreadyLoaded = true;
            }
            this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
            //this->ui->alertsLineEdit->setText(dbClientCertUsersFilename);
            this->ui->networkSaveBut->setFocus();

            if ( mainWindow_->isSupervisorMode() )
            {
                // this is the printer tab in Supervisor, or it could be
                if ( !printerTabAlreadyLoaded )
                    setupPrinterTab();                                  // only need to do this once

                continueSetupPrinterTab();                              // do this every time

                printerTabAlreadyLoaded = true;
                printerDriverChanged = false;
                internalConfigData_ = &session_->scaleConfig->internalConfig;
                internalConfigData_->hasExternalPrinterSettingsChanged = false;

                // 7,113 reguest Driver Names
                session_->send(Printer, PrReadAllNetworkPrinterDriverNames);    // Request Printer Driver Names
            }
            this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
            break;
        case 4:    
            this->ui->alertsLineEdit->setText(tr("Change Host Settings"));
            this->ui->setHostnameBut->setFocus();
            break;
        case 5:  
            if ( !printerTabAlreadyLoaded )
                setupPrinterTab();                                  // only need to do this once

            continueSetupPrinterTab();                              // do this every time

            printerTabAlreadyLoaded = true;
            printerDriverChanged = false;
            internalConfigData_ = &session_->scaleConfig->internalConfig;
            internalConfigData_->hasExternalPrinterSettingsChanged = false;

            // 7,111 reguest USB Driver Names
            session_->send(Printer, PrReadAllUsbDriverNames);
            break;
        case 6:
            this->ui->alertsLineEdit->setText(tr("Change SNMP Settings"));
    }

    //LOG_DEBUG( "currentChangedSlot() END", NULL );
}

//////////////////////////////////////////////////////
//
//       B R I G H T N E S S   S P I N B O X
//
//        V A L U E   C H A N G E D
//
//////////////////////////////////////////////////////

void setupUtility::brightnessSpinBoxValueChanged(int value)
{

    this->ui->brightnessLabel->setText(QString::number(value));


	this->setBrightnessIndicators(value);
}

//////////////////////////////////////////////////////
//
//        S E T    B R I G H T N E S S    C L I C K
//
//////////////////////////////////////////////////////
void setupUtility::setBrightnessBut_click()
{
    //    Call brightness with:
    //    echo -n 255 > /sys/class/backlight/pwm-backlight.[0|1]/brightness
    processingAButton = true;
    if (this->ui->operatorScreenBut->isChecked())
    {
        displayDevice = "operator";
        operatorBrightness = ui->brightnessLabel->text();
        dbKey = "oBrightness";
    }
    else if(this->ui->customerScreenBut->isChecked())
    {
        displayDevice = "customer";
        customerBrightness = ui->brightnessLabel->text();
        dbKey = "cBrightness";
    }
    else
    {
        processingAButton = false;

        return;
    }

    python pythonProcess;

    QStringList args;

    args.append(displayDevice);

    /*if(static_cast<int>(this->ui->brightnessLabel->text().toDouble() * 2.55) < 3)
    {
        args.append("3");
    }
    else
    {
        if(this->ui->brightnessLabel->text() == "100")
        {
            args.append("255");
        }
        else
        {
            args.append(QString::number(static_cast<int>(this->ui->brightnessLabel->text().toDouble() * 2.55)) );
        }
    }
    */
    args.append(QString::number(this->ui->brightnessLabel->text().toInt() + 104));
    if (pythonProcess.execCommand("setBrightness", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
           // ui->alertsLineEdit->setText(alertsText);
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }

    if(this->updateDB(dbKey, this->ui->brightnessLabel->text()))
    {
        if(displayDevice == "customer")
        {
            alertsText = tr("Customer");
        }
        else
        {
            alertsText = tr("Operator");
        }
        this->ui->alertsLineEdit->setText(alertsText + tr(" display brightness set."));
    }
    else
    {
        // failed to save
        this->ui->alertsLineEdit->setText(tr("Display brightness not set."));
    }
    processingAButton = false;
}

void setupUtility::turnOffDisplays()
{
    //    Call brightness with:
    //    echo -n 255 > /sys/class/backlight/pwm-backlight.[0|1]/brightness

    //processingAButton = true;
    displayDevice = "both";
    python pythonProcess;

    QStringList args;

    args.append(displayDevice);
    args.append("0");


    if (pythonProcess.execCommand("turnOffDisplays", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
           // ui->alertsLineEdit->setText(alertsText);
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }
    //processingAButton = false;

    this->ui->alertsLineEdit->setText(tr("System is restarting..."));
}

//////////////////////////////////////////////////////
//
//       V O L U M E   I N D I C A T O R S    C L I C K
//
//////////////////////////////////////////////////////

void setupUtility::volumeIndicator0_click()
{
    doPreset(0);
}

void setupUtility::volumeIndicator1_click()
{
    doPreset(1);
}

void setupUtility::volumeIndicator2_click()
{
    doPreset(2);
}

void setupUtility::volumeIndicator3_click()
{
    doPreset(3);
}

void setupUtility::volumeIndicator4_click()
{
    doPreset(4);
}

void setupUtility::volumeIndicator5_click()
{
    doPreset(5);
}

void setupUtility::volumeIndicator6_click()
{
    doPreset(6);
}

void setupUtility::volumeIndicator7_click()
{
    doPreset(7);
}

void setupUtility::volumeIndicator8_click()
{
    doPreset(8);
}

void setupUtility::volumeIndicator9_click()
{
    doPreset(9);
}

void setupUtility::volumeIndicator10_click()
{
    doPreset(10);
}

//////////////////////////////////////////////////////
//
//       V O L U M E    D O    P R E S E T
//
//////////////////////////////////////////////////////

void setupUtility::doPreset(int value)
{
    value *= 10;
    if(value != 0)
    {
        this->ui->muteBut->setChecked(true);
        this->ui->volumeSpinBox->setValue(value);
        this->ui->volumeLabel->setText(QString::number(value));
        this->setVolumeIndicators(value);
    }
    else
    {
        this->ui->volumeSpinBox->setValue(1);
        this->ui->volumeLabel->setText(QString::number(1));
        this->setVolumeIndicators(1);
        this->ui->muteBut->setChecked(false);
    }
}

void setupUtility::manualPause()
{
    state = Paused;
    QTimer::singleShot(1000, this, SLOT(setRunState()));
    while(1)
    {
        qApp->processEvents();
        if(state == Run) break;
    }
}

void setupUtility::setRunState()
{
    state = Run;
}

void setupUtility::on_saveSNMPManagerBut_clicked()
{
    LOG_CLASS_INFO( "on_saveSNMPManagerBut_clicked", "on_saveSNMPManagerBut_clicked" );

    //save to DB
    updateDB("snmpTrapsIP", ui->snmpManagerTrapsIP->text());
    updateDB("snmpTrapsPort", ui->snmpManagerTrapsPort->text());
    updateDB("snmpVariablesPort", ui->snmpManagerVariablesPort->text());
    updateDB("snmpEnabled", "1");

    ui->snmpStatusLabel->setAutoFillBackground(true);
    ui->snmpStatusLabel->setStyleSheet("color: rgb(69, 134, 26);background-color: rgba(255, 255, 255, 0);");
    ui->snmpStatusLabel->setText(tr("SNMP Agent is enabled."));

    // restart the snmp agent
    QProcess snmpProcess;
    QStringList options;
    options.append(filesystem::absoluteFilePath("snmpReload.py",folderType::Python));

    snmpProcess.start("python", options);

    if ( !snmpProcess.waitForStarted() )
        this->ui->alertsLineEdit->setText(tr("Start snmp reload error."));
    else
    {
        snmpProcess.waitForFinished();
        ui->alertsLineEdit->setText(tr("SNMP Agent started with new settings."));
    }
}

void setupUtility::on_snmpStopBut_clicked()
{
    //save to DB
    LOG_CLASS_INFO( "setupUtility", "on_snmpStopBut_clicked" );

    //LOG_DEBUG( "on_snmpStopBut_clicked() START", NULL );

   // Qprocess p;

   // p.start("pkill /usr/bin/python /usr/local/hobart/python/snmpHobart.py");

    // add root permission

    ui->junkTextEdit->hide();

    python pythonProcess;
    QStringList options;

    if (pythonProcess.execCommand("snmpKill", options))
   {
       if (pythonProcess.getExitCode() == 0)
       {
           LOG_DEBUG( "it worked : %d" , pythonProcess.getExitCode() );

           updateDB("snmpTrapsIP", ui->snmpManagerTrapsIP->text());
           updateDB("snmpTrapsPort", ui->snmpManagerTrapsPort->text());
           updateDB("snmpVariablesPort", ui->snmpManagerVariablesPort->text());
           updateDB("snmpEnabled", "0");

           // restart the snmp agent
           //system("pkill -f '/usr/bin/python /usr/local/hobart/python/s.py'");

           ui->alertsLineEdit->setText(tr(""));
           ui->snmpStatusLabel->setText(tr("SNMP Agent is disabled."));
           ui->snmpStatusLabel->setStyleSheet("color: rgb(179, 6, 6);background-color: rgba(255, 255, 255, 0);");
       }
       else
       {
           LOG_DEBUG( "it didn't work : %d", pythonProcess.getExitCode() );

           // TODO:
           ui->alertsLineEdit->setText(tr("SNMP unchanged."));
           ui->snmpStatusLabel->setText(tr("SNMP Agent is disabled."));
           ui->snmpStatusLabel->setStyleSheet("color: rgb(179, 6, 6);background-color: rgba(255, 255, 255, 0);");
       }
   }
   else
   {
       // TODO:
       // What do we do here?
   }

    //LOG_DEBUG( "on_snmpStopBut_clicked() END", NULL );
}

void setupUtility::manageInstallationDate()
{
    ui->installationDateFrame->hide();
    /*
    if(!isInstallationDateSet())
    {
        QString text( session_->uiUtilities->updateDateTime() );

        // convert case if required
        if ( convertUppercase_ )
            text = text.toUpper();

        ui->installationDateCurrentDateLabel->setText(text);
        ui->installationDateFrame->show();
        ui->installationDateFrameNo->hide();
        ui->installationDateFrameStart->show();
        ui->installationDateCalendar->hide();
    }
    else
    {
        ui->installationDateFrame->hide();
    }
    */
}

bool setupUtility::isInstallationDateSet()
{
    return false;
}

void setupUtility::on_pushButton_3_clicked()
{
    ui->installationDateFrame->hide();
}

void setupUtility::on_installationDateNoBut_clicked()
{
    //ui->installationDateCurrentDateLabel->setText(tr("Choose installation date below then press Set Installation Date button."));
    ui->installationDateFrameNo->show();
    ui->installationDateFrameStart->hide();
}

void setupUtility::on_installationDateYesBut_clicked()
{
    // set the date and hide all
    ui->installationDateFrame->hide();
}
