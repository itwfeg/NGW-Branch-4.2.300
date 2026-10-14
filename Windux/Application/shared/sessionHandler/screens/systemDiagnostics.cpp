/*
PROLOGUE: System Diagnostics
Date: Q4 2014
AUTHOR: Todd Davis
COMPANY: Hobart

PURPOSE: This application performs system diagnostics for:

         - Time & Date
         - Sound Volume
         - Network Configuration
         - Touchscreen Calibration
*/

#include <QtGui>
#include <QtCore>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <UiUtilities.h>
#include <filesystem.h>
#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

//#include <QWidget>
#include "systemDiagnostics.h"
#include "ui_systemDiagnostics.h"
#include "Devices.h"
#include "Events.h"
#include "Messages.h"
#include "../dialogs/filedialog.h"
#include "../dialogs/messageboxdialog.h"

#include <QCryptographicHash>
#include <QScrollBar>

#include "../platform/platform.h"
#include "../platform/network.h"
#include "../platform/python.h"

#include <logmanager.h>
#include <ScaleConfiguration.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

const QString systemDiagnostics::ServicePassword = "hobart45374";
const QString systemDiagnostics::PASSPHRASE = "woozal";
const bool convertUppercase_ = true;
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
#include <qthread.h>

class internalSleeper : public QThread
{
    public:
        static void sleep(unsigned long secs) {
        QThread::sleep(secs);
        }
        static void msleep(unsigned long msecs) {
        QThread::msleep(msecs);
        }
        static void usleep(unsigned long usecs) {
        QThread::usleep(usecs);
        }
};

systemDiagnostics::systemDiagnostics(SessionMainWindow *mainWindow, ModeWidget *parent) :
  _Mybase( mainWindow, parent ),
  ui(new Ui::systemDiagnostics),
  player( NULL )
{
     setObjectName( "systemDiagnostics" );
     processingAButton = true;
    ///////////////////////////
    // SOME GENERAL INITIALIZING
    ///////////////////////////
    failIcon = QIcon(":/images/screens/fail.png");
    passIcon = QIcon(":/images/screens/pass.png");
    ui->setupUi(this);


    firstLoad = true;
    ui->label_82->raise();
    ui->label_82->show();
    ui->gbDebug->setGeometry( scaleRectForScreen( QRect( 3, 3, 954, 517 ) ) );
    ui->gbDebug->hide();
    this->update();
    qApp->processEvents();
    ui->USBGroupbox->hide();
    mustTurnOnDialogs = false;
    shouldChangeTime = false;
    tryingAHiddenSSID = false;
    networkTabAlreadyLoaded = false;
    weHaveServiceMode = false;
    connectedSuccessfully = false;
    connectionTimeout = false;
    dbDisplayTab = false;
    dbCustomerSoundTab = false;
    dbOperatorSoundTab = false;
    dbTimeTab = false;
    dbNetworkTab = false;
    dbHostTab = false;
    setTheCustomerSound = false;
    setTheOperatorSound = false;
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
    ui->debugBut->hide();
    ui->debugLabel->hide();
    ui->gbCertInfo->hide();
    ui->frameDeleteServerCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
    ui->advNetworkingFrame->hide();
    //ui->frmAdvWirelessWait->hide();
    ui->frmToolsWait->hide();
    ui->frmWirelessWait->hide();

    linkRateIndex = 0;
    automaticLinkRate = true;
    channelIndex = 0;
    etherLinkRateIndex = 0;
    etherAutomaticLinkRate = true;

    statusAnchorNumber = 0;
    toolsAnchorNumber = 0;
    memoryAnchorNumber = 0;

    //ui->gbDeleteClientCertPrompt->hide();
    //ui->gbDeleteServerCertPrompt->hide();

    this->setWirelessNetworkEncryptionCombo(0);
    this->ui->networkLoadingStatusLabel->hide();
    this->ui->gbNetworkLoading->hide();
    this->ui->gbAdvancedNetworking->hide();
    this->ui->advReportsNetworkConfigUSBBut->hide();
    this->ui->advReportsNetworkConfigUSBBut->setDisabled(true);
    this->ui->advNetworkingTabWidget->setCurrentIndex(0);
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
    advcancelPing = true;
    this->ui->alertsLineEdit->setAlignment(Qt::AlignCenter);
    this->connect(this->ui->mainTab,SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

    ///////////////////////////
    // GET DB SETTINGS
    ///////////////////////////

    connectDB();
    bool redo = false;

    if(mergeNewDBEntries("timeKeeper","Manual")) redo = true;
    if(mergeNewDBEntries("ntpServer", "0.0.0.0")) redo = true;
    if(mergeNewDBEntries("ntpFrequency","0")) redo = true;
    if(mergeNewDBEntries("ntpDayOfWeek","0")) redo = true;
    if(mergeNewDBEntries("ntpTimeOfDay","03:30:00")) redo = true;
    if(mergeNewDBEntries("ntpTimeZone","America/Detroit")) redo = true;
    if(mergeNewDBEntries("ntpAntiMeridian","AM")) redo = true;
    if(mergeNewDBEntries("wpaPassphrase","")) redo = true;
    if(mergeNewDBEntries("eapUsername","")) redo = true;
    if(mergeNewDBEntries("eapPassword","")) redo = true;
    if(mergeNewDBEntries("speakerOutput", "0")) redo = true;
    if(mergeNewDBEntries("serverCertFilename", "")) redo = true;
    if(mergeNewDBEntries("chainFilename", "")) redo = true;
    if(mergeNewDBEntries("clientCertFilename", "")) redo = true;
    if(mergeNewDBEntries("csrFilename", "")) redo = true;
    if(mergeNewDBEntries("csrKeyFilename", "")) redo = true;
    if(mergeNewDBEntries("clientKeyFilename", "")) redo = true;
    if(mergeNewDBEntries("clientCertUsersFilename", "")) redo = true;
    if(mergeNewDBEntries("clientCertPassword", "")) redo = true;
    if(mergeNewDBEntries("linkRate", "auto")) redo = true;
    if(mergeNewDBEntries("automaticLinkRate", "1")) redo = true;
    if(mergeNewDBEntries("channel","auto")) redo = true;
    if(mergeNewDBEntries("wirelessLogging", "0")) redo = true;

    if(redo) connectDB();

    if (dbDisplayTab) ui->mainTab->setTabIcon(0, passIcon);
    if( dbOperatorSoundTab && dbCustomerSoundTab) ui->mainTab->setTabIcon(1, passIcon);
    if(dbTimeTab) ui->mainTab->setTabIcon(2, passIcon);
    if(dbNetworkTab) ui->mainTab->setTabIcon(3, passIcon);
    if(dbHostTab) ui->mainTab->setTabIcon(4, passIcon);

    // now set icons of completed tabs
    //if()

    dbConnectionType == 0 ? finalIsWireless = false : finalIsWireless = true;

    //////////////////////////
    // GET PYTHON & PING PROCESSES READY
    ///////////////////////////

    pingProcess = new QProcess(this);
    advpingProcess = new QProcess(this);
    advTracerouteProcess = new QProcess(this);
    memoryProcess = new QProcess(this);
    saveNetworkConfigurationProcess = new QProcess(this);
    networkProcessConnected = false;

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

    this->ui->customerScreenBut->setIcon(QIcon(":/images/screens/customer.png"));
    this->ui->customerScreenBut->setIconSize(QSize(86,54));
                                                                                                              //background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(170, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));
    this->ui->operatorScreenBut->setStyleSheet("QPushButton#operatorScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(170, 255, 0, 255), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    this->ui->customerScreenBut->setStyleSheet("QPushButton#customerScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(170, 255, 0, 255), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");

    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLEDoff.png);");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/screens/brightnessLEDoff.png);");
    this->ui->gcdSlider->setValue(dbGCD.toInt());
    this->gcdSliderChanged(dbGCD.toInt());

    this->connect(this->ui->gcdSlider, SIGNAL(valueChanged(int)), this, SLOT(gcdSliderChanged(int)));
    this->connect(this->ui->operatorScreenBut, SIGNAL(clicked()),this, SLOT(changedOperatorDisplayBrightnessButtons()));
    this->connect(this->ui->customerScreenBut, SIGNAL(clicked()),this, SLOT(changedCustomerDisplayBrightnessButtons()));
    this->connect(this->ui->calibrateScreenBut, SIGNAL(clicked()), this, SLOT(calibrateScreenBut_click()));
    this->connect(this->ui->setBrightnessBut, SIGNAL(clicked()), this, SLOT(setBrightnessBut_click()));
    this->connect(this->ui->brightnessSpinBox,SIGNAL(valueChanged(int)), this, SLOT(brightnessSpinBoxValueChanged(int)));

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

    this->loadSoundVolumeValues("o");
    this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    this->ui->operatorSoundBut->setStyleSheet("QPushButton#operatorSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    this->ui->customerSoundBut->setStyleSheet("QPushButton#customerSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    // QT5 PORT    
    //NO LONGER WORKS - audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    //NO LONGER WORKS - mediaObject = new Phonon::MediaObject(this);
    // Used examples from:
    // 1: http://doc.qt.io/qt-5/qaudiooutput.html#details AND
    // 2: http://www.codeproject.com/Articles/421287/Cross-Platform-Microphone-Audio-Processing-Utility

    // QT5 PORT - 1st option - that I think I am not even going to try
    //QAudioFormat format;
    //format.setSampleRate(8000);     // set frequency. Is this correct? // replaced - format.setFrequency(8000);
    //format.setChannelCount(2);      // stero? was orginal (1) for mono
    //format.setSampleSize(16);       // was originally 8
    //format.setCodec("audio/pcm");
    //format.setByteOrder( QAudioFormat::LittleEndian );
    //format.setSampleType( QAudioFormat::UnSignedInt );

    //audioOutput = new QAudioOutput( format, this);
    //mediaObject = new QMediaPlayer;

    // QT5 PORT - 2nd option which may be the right way
    player = videoPlayer::create( videoPlayerDevice::Operator, this );
    player->setVisible(false);

    this->connect(this->ui->operatorSoundBut, SIGNAL(clicked()), this, SLOT(changedOperatorSoundButtons()));
    this->connect(this->ui->customerSoundBut, SIGNAL(clicked()), this, SLOT(changedCustomerSoundButtons()));
    this->connect(this->ui->setVolumeBut,     SIGNAL(clicked()), this, SLOT(setVolumeBut_click()));
    this->connect(this->ui->testVolumeBut,    SIGNAL(clicked()), this, SLOT(testVolumeBut_click()));
    this->connect(this->ui->muteBut,          SIGNAL(toggled(bool)),this, SLOT(muteButChanged(bool)));
    connect(player,                           SIGNAL( finished() ),    SLOT( playAudioFile() ) );
    //this->connect(this->mediaObject,SIGNAL(finished()),this,SLOT(playAudioFile()));
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

    ////////////////////////////
    // SET TIME TAB
    ////////////////////////////

    pickedDate = new QDate();
    this->changedAstronomical = false;
    this->changedTimezone = false;
    //qApp->processEvents();
    this->fillTimeZones();
    this->ui->timeZoneCombo->setCurrentIndex(ui->timeZoneCombo->findData(dbTimezone));
    this->setTimeTab();

    //connect(ui->keepAliveBut, SIGNAL(clicked), this, SLOT(keepAppAlive()));
    //connect(ui->keepAliveManual, SIGNAL(clicked), this, SLOT(keepAppAlive()));

    this->connect(this->ui->setDateTimeBut, SIGNAL(clicked()), this, SLOT(setDateTimeBut_click()));
    this->connect(this->ui->dateEdit, SIGNAL(clicked(QDate)), this, SLOT(datePicked(QDate)));
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

    m_originalTimeEditStyleSheet = "color:black; background: rgb(238, 255, 211);";

    for(int i = 0; i < ui->ntpDayOfWeekCombo->count(); i++)
        ui->ntpDayOfWeekCombo->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    //ui->ntpDayOfWeekCombo->setEditable(true);
    //ui->ntpDayOfWeekCombo->lineEdit()->setReadOnly(true);
    //ui->ntpDayOfWeekCombo->lineEdit()->setAlignment(Qt::AlignCenter);
    //ui->ntpDayOfWeekCombo->lineEdit()->setFont(ui->ntpDayOfWeekCombo->font());
    if(ui->ntpFrequencyCombo->currentIndex() == 2 || ui->ntpFrequencyCombo->currentIndex() == 1)
    {
        ui->ntpDayOfWeekCombo->setEnabled(false);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down { width: 48px; height: 48px; border: 1px solid gray; border-radius: 4px; background: url(:/images/screens/SpinBoxButtonsOffBottomTime.png); /*background: url(:/images/screens/SpinBoxButtonsOffCalendarBottom.png);*/ color: black; }");
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down { width: 48px; height: 48px; border: 1px solid gray; border-radius: 4px; background: url(:/images/screens/SpinBoxButtonsOffBottomTime.png); /*background: url(:/images/screens/SpinBoxButtonsOffCalendarBottom.png);*/ color: black; }");
    }
    for(int i = 0; i < ui->ntpFrequencyCombo->count(); i++)
        ui->ntpFrequencyCombo->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    //ui->ntpFrequencyCombo->setEditable(true);
    //ui->ntpFrequencyCombo->lineEdit()->setReadOnly(true);
    //ui->ntpFrequencyCombo->lineEdit()->setAlignment(Qt::AlignCenter);
    //ui->ntpFrequencyCombo->lineEdit()->setFont(ui->ntpFrequencyCombo->font());
    //ui->ntpFrequencyCombo->lineEdit()->setStyleSheet("color: black; background: rgb(238,255,211);");
    ui->ntpTimeZoneCombo->view()->setStyleSheet("QComboBox#ntpTimeZoneCombo{ color: black;border-radius: 5px; border: 1px solid black; background: rgb(238, 255, 211); text-align: center;} QComboBox#ntpTimeZoneCombo::drop-down { width: 48px; height: 48px; border: 1px solid gray; border-radius: 4px; background: url(:/images/screens/SpinBoxButtonsOffBottomTime.png); color: black; }  QComboBox#ntpTimeZoneCombo::!editable{  color: black;  border-radius: 5px; border: 1px solid black;  background: rgb(238, 255, 211);  text-align: center;   }  QComboBox#ntpTimeZoneCombo::editable{ color: black; border-radius: 5px;  border: 1px solid black;  background: rgb(238, 255, 211); text-align: center; }");
    ui->ntpServer->setText(dbNTPServer);

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
    //ui->timeTabWidget->removeTab(1);

    ///////////////////////////
    // SET NETWORK TAB
    ///////////////////////////
    if(mainWindow->isServiceMode()) weHaveServiceMode = true;
    else weHaveServiceMode = false;

    if(mainWindow->isSupervisorMode())
    {
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

        if ( !settings->rebootUnprotectedAllowed() )
        {
            ui->mainTab->removeTab(4);
            ui->mainTab->removeTab(3);
            ui->timeTabWidget->setCurrentIndex(0);
            ui->timeTabWidget->removeTab(2);
            //set a stylesheet to increase width
            ui->mainTab->setStyleSheet("QTabWidget#mainTab{background: transparent;} QTabWidget::pane{ border-top: 1px solid rgb(180,180,180); border: 1px solid rgb(139,209,0); background:  url(images/carbonFiberBackground.png);} QTabWidget::tab-bar{ /*left: 10px;*/ } QTabBar::tab{ border: 2px solid #C4C4C3; border-color: rgb(139, 209, 0); border-bottom-color: rgb(139,209,0); height: 50px;  min-width: 283px; background: rgb(227, 227, 227); border-top-left-radius: 8px; border-top-right-radius: 8px; padding-left: 20px;} QTabBar::tab:selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); border-color: rgb(139,209,0); color:rgb(139,209,0); margin-top: 2px; height: 48px;} QTabBar::tab:!selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));  border-color: rgb(139,209,0);  color:rgb(255,255,255); margin-top: 2px;  height: 48px; } QTabBar::tab:selected{  margin-left: 0px;  margin-right: 0px;  border-bottom-color: rgb(139,209,0); }  QTabBar::tab:first:selected{  margin-left: 0px; } QTabBar::tab:last:selected{ margin-right: 0px; }");

            //ui->mainTab->setStyleSheet("QTabWidget#mainTab{background: transparent;} QTabWidget#mainTab::pane{ border-top: 1px solid rgb(180,180,180); border: 1px solid rgb(139,209,0); background:  url(images/carbonFiberBackground.png);} QTabWidget#mainTab::tab-bar{ /*left: 10px;*/ } QTabWidget#mainTab::tab-bar::tab{ border: 2px solid #C4C4C3; border-color: rgb(139, 209, 0); border-bottom-color: rgb(139,209,0); height: 50px;  min-width: 283px; background: rgb(227, 227, 227); border-top-left-radius: 8px; border-top-right-radius: 8px; padding-left: 20px;} QTabWidget#mainTab::tab-bar::tab:selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); border-color: rgb(139,209,0); color:rgb(139,209,0); margin-top: 2px; height: 48px;} QTabWidget:tab-bar::tab:!selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));  border-color: rgb(139,209,0);  color:rgb(255,255,255); margin-top: 2px;  height: 48px; } QTabWidget::tab-bar::tab:selected{  margin-left: 0px;  margin-right: 0px;  border-bottom-color: rgb(139,209,0); }  QTabWidget::tab-bar::tab:first:selected{  margin-left: 0px; } QTabWidget::tab-bar::tab:last:selected{ margin-right: 0px; }");
            ui->timeTabWidget->setStyleSheet("QTabWidget#timeTabWidget{ background:  transparent; }  QTabWidget#timeTabWidget::pane{ border-top: 1px solid rgb(180,180,180);  border: 1px solid rgb(139,209,0);  background:  url(images/carbonFiberBackground.png); margin-bottom:0px;  margin-top: -11px; }  QTabWidget#timeTabWidget::tab-bar{ } QTabBar::tab{  border-color: rgb(139,209,0); border-bottom-color: rgb(139,209,0); height: 41px; min-width: 170px; margin-top: 2px; background: rgb(227, 227, 227); padding-left: 10px; } QTabWidget#timeTabWidget::tab-bar::tab:selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); color:rgb(139,209,0); margin-bottom:0px; margin-top: 2px; border-bottom-color: rgb(139,209,0); height:41px; } QTabWidget#timeTabWidget::tab-bar::tab:!selected{ background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255)); color:rgb(255,255,255); height: 41px; margin-top: 2px;  margin-bottom:0px; } QTabBar::tab:first:selected{ min-width: 353px; border-top-left-radius: 8px; border-top-right-radius: 8px; margin-left: 0px; height: 41px; margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));} QTabBar::tab:first:!selected{  min-width: 353px;  border-top-left-radius: 8px; border-top-right-radius: 8px;  margin-left: 0px;  height: 41px; margin-top: 2px; margin-bottom:0px;  border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));}  QTabBar::tab:last:selected{ margin-right: 0px; min-width: 170px; border-top-left-radius: 8px;  border-top-right-radius: 8px; height: 41px;  margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));} QTabBar::tab:last:!selected{  margin-right: 0px; min-width: 170px; border-top-left-radius: 8px; border-top-right-radius: 8px;  height: 41px; margin-top: 2px; margin-bottom:0px; border-color: rgb(139,209,0); background:qlineargradient(spread:reflect, x1:0.005, y1:0, x2:1, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(66, 66, 66, 255));}");

            ui->gcdLabel->hide();
            ui->gcdSlider->setDisabled(true);
            ui->gcdSlider->setEnabled(false);
            ui->gcdSlider->hide();
            ui->gcdNoLabel->hide();
            ui->gcdYesLabel->hide();
        }
        else
        {
            this->loadHostsTab();
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

    isReadyToTranslate = true;
    this->ui->mainTab->setCurrentIndex(0);
    //dialog = new MessageBoxDialog(this );

    //dialoginternal.setText( tr( "Please remove the USB bulk storage device." ) );
    //dialog.setStandardButtons( QDialogButtonBox::Ok );
    //dialog = dialoginternal;
    changedSpeakers = false;
    this->loadNetworkTab();

    this->ui->label_82->hide();
    this->ui->advNetworkingFrame->raise();
    this->ui->advNetworkingFrame->show();
    processingAButton = false;
    //ui->alertsLineEdit->setText(dbClientCertUsersFilename);

    scaleScreen();
    expandTabs(ui->mainTab);
    expandTabs(ui->advNetworkingTabWidget);
    hideAll(ui->mainTab);

}

void systemDiagnostics::internalSpeakerClicked(bool)
{
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

void systemDiagnostics::externalSpeakerClicked(bool)
{
  //  ui->butInternalSpeaker->setChecked(false);
  //  this->ui->butInternalSpeaker->setStyleSheet("color: rgb(0,0,0); background-color: rgb(241,241,241);");
  //  this->ui->butExternalUSBSpeaker->setStyleSheet("color: rgb(0, 0, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);");
    // immediately change the speaker output source to external through python ;
    // call python directly. maybe in the future we can permit setup utility to execute remotely
    //QProcess
    //processingAButton = true;
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

void systemDiagnostics::keepAppAlive()
{
    this->update();
}

void systemDiagnostics::saveNTP2DB()
{
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


void systemDiagnostics::ntpFrequencyChanged(int index)
{
    if(index > 0)
    {
        ui->ntpDayOfWeekCombo->setDisabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: rgb(200,200,200); border-radius: 5px; border: 1px solid black; background: rgb(200,200,200); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down { width: 48px; height: 48px; border: 1px solid gray; border-radius: 4px; background: url(:/images/screens/SpinBoxButtonsOffBottomTime.png); /*background: url(:/images/screens/SpinBoxButtonsOffCalendarBottom.png);*/ color: black; }");

        //ui->ntpDayOfWeekCombo->lineEdit()->setStyleSheet("color: black; background: rgb(200,200,200);");
    }
    else
    {
        ui->ntpDayOfWeekCombo->setEnabled(true);
        ui->ntpDayOfWeekCombo->setStyleSheet("QComboBox#ntpDayOfWeekCombo{ color: black; border-radius: 5px; border: 1px solid black; background: rgb(238,255,211); text-align: center;} QComboBox#ntpDayOfWeekCombo::drop-down { width: 48px; height: 48px; border: 1px solid gray; border-radius: 4px; background: url(:/images/screens/SpinBoxButtonsOffBottomTime.png); /*background: url(:/images/screens/SpinBoxButtonsOffCalendarBottom.png);*/ color: black; }");
        //ui->ntpDayOfWeekCombo->lineEdit()->setStyleSheet("color: black; background: rgb(238,255,211);");
    }
}

bool systemDiagnostics::eventFilter( QObject *obj, QEvent *event )
{
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

void systemDiagnostics::loadHostsTab()
{
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
    //this->connect(this->ui->advstartPingBut, SIGNAL(clicked()), this, SLOT(advstartPingBut_click()));
    //this->connect(this->ui->advstopPingBut, SIGNAL(clicked()), this, SLOT(advstopPingBut_click()));
    //this->connect(this->ui->toolsPingTestBut, SIGNAL(clicked(bool)), this, SLOT(advstartPingBut_click(bool)));
    this->connect(pingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printPingOutput()));
    this->connect(pingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int,QProcess::ExitStatus)));
    this->connect(pingProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
    //this->connect(advpingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(advprintPingOutput()));
    //this->connect(advpingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(advfinishedProcess(int,QProcess::ExitStatus)));
    //this->connect(advpingProcess,SIGNAL(readyReadStandardError()),this,SLOT(advreadyReadStandardError()));
    this->connect(this->ui->scalePasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAgainstEmptyPassword(QString)));
    this->connect(this->ui->setScalePasswordBut, SIGNAL(clicked()), this, SLOT(setScalePasswordBut_click()));
    //processingAButton = false;

}

void systemDiagnostics::hourBut_clicked()
{
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(0);
}

void systemDiagnostics::minuteBut_clicked()
{
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->timeEdit->setCurrentSectionIndex(1);
}

void systemDiagnostics::secondBut_clicked()
{
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(2);
}

void systemDiagnostics::ampmBut_clicked()
{
    ui->ampmBut->setStyleSheet("QPushButton#ampmBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->timeEdit->setCurrentSectionIndex(3);
}

void systemDiagnostics::timeEditTimeChanged(QTime)
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

void systemDiagnostics::ntpHourBut_clicked()
{
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(0);
}

void systemDiagnostics::ntpMinuteBut_clicked()
{
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpTimeEdit->setCurrentSectionIndex(1);
}

void systemDiagnostics::ntpSecondBut_clicked()
{
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(2);
}

void systemDiagnostics::ntpAmpmBut_clicked()
{
    ui->ntpAmpmBut->setStyleSheet("QPushButton#ntpAmpmBut{ color: white; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(0, 0, 255);}");
    ui->ntpMinuteBut->setStyleSheet("QPushButton#ntpMinuteBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpSecondBut->setStyleSheet("QPushButton#ntpSecondBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");
    ui->ntpHourBut->setStyleSheet("QPushButton#ntpHourBut{ color: black; border-radius: 5px; border: 1px rgb(238, 255, 211); background: rgb(238, 255, 211);}");

    ui->ntpTimeEdit->setCurrentSectionIndex(3);
}

void systemDiagnostics::ntpTimeEditTimeChanged(QTime)
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

systemDiagnostics::~systemDiagnostics()
{
    //takeDownNICs();
    if(mustTurnOnDialogs)
    {
       session_->send(SessionDev , SnRestartDialogs);
      // session_->send( NetworkConnectivityDev, RequestCurrentStatus );
       mustTurnOnDialogs = false;
    }

    if(changedHostname /*|| changedSpeakers*/)
    {
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
        // QT5 PORT
        //mediaObject->deleteLater();
        //audioOutput->deleteLater();
        player->deleteLater();
    }
}

void systemDiagnostics::updateCharacterCount(QString incomingString)
{
    ui->wirelessAuthenticationGroupBox->setTitle(tr("Wireless Authentication: ") + QString::number(incomingString.count()) + tr(" char."));
}

void systemDiagnostics::loadNetworkTab()
{
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
       // ui->advEthernetTab->hide();
        ui->advNetworkingCloseBut->hide();
        this->powerDevices("11");
        commonNetwork = new Network();
        //QPixmap pixmap(":/images/screens/refresh.png");
        //QIcon refreshButtonIcon(pixmap);
        //this->ui->refreshNetworksBut->setIcon(refreshButtonIcon);
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

        if(dbWirelessLogging == "1")
        {
            ui->toolsWirelessLoggingBut->setText(tr("STOP\nWireless Logging"));
            ui->toolsWirelessLoggingBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            //processingAButton = true;
            this->ui->toolsWirelessLoggingBut->setChecked(true);// startPingBut->setEnabled(false);
            //updateDB("wirelessLogging", "1");
        }
        else
        {
            ui->toolsWirelessLoggingBut->setText(tr("START\nWireless Logging"));
            ui->toolsWirelessLoggingBut->setStyleSheet("");
            ui->toolsWirelessLoggingBut->setChecked(false);
            ui->toolsWirelessLoggingBut->setDown(false);
            ;//updateDB("wirelessLogging", "0");
        }


        this->connect(this->ui->noGatewayBut, SIGNAL(clicked()), this, SLOT(gatewayNoSlot()));
        this->connect(this->ui->yesGatewayBut, SIGNAL(clicked()), this, SLOT(gatewayYesSlot()));
        this->connect(saveNetworkConfigurationProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printNetworkOutput()));
        this->connect(saveNetworkConfigurationProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedNetworkProcess(int,QProcess::ExitStatus)));
        this->connect(saveNetworkConfigurationProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadNetworkStandardError()));
        this->connect(this->ui->refreshNetworksBut, SIGNAL(clicked()), this, SLOT(refreshNetworksButton_click()));
        this->connect(this->ui->quitBut, SIGNAL(clicked()), qApp, SLOT(quit()));
        this->connect(this->ui->exportBut, SIGNAL(clicked()), this, SLOT(exportNetworkBut_click()));
        this->connect(this->ui->importBut, SIGNAL(clicked()), this, SLOT(importNetworkBut_click()));
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
        this->connect(this->ui->wirelesstw, SIGNAL(cellPressed(int,int)), this, SLOT(wirelessTWCellPressed(int,int)));
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
        //this->connect(this->ui->advancedNetworkingApplyBut_2, SIGNAL(clicked()), this, SLOT(advancedNetworkingApplyBut_clicked()));
        //this->connect(this->ui->advancedNetworkingDefaultsBut_2, SIGNAL(clicked()), this, SLOT(advancedNetworkingDefaultsBut_clicked()));
        //this->connect(this->ui->advancedNetworkingCancelBut_2, SIGNAL(clicked()), this, SLOT(advancedNetworkingCancelBut_clicked()));
        //this->connect(this->ui->cbAdvWifiTransferRate, SIGNAL(currentIndexChanged(int)), this, SLOT(advancedNetworkingLinkRateCombo_changed(int)));
        this->connect(this->ui->advNetworkingCloseBut, SIGNAL(clicked()), this, SLOT(advNetworkingCloseBut_clicked()));
        //this->connect(this->ui->advEtherApplyBut, SIGNAL(clicked()), this, SLOT(advEtherApplyBut_clicked()));
        //this->connect(this->ui->advEtherDefaultsBut, SIGNAL(clicked()), this, SLOT(advEtherDefaultsBut_clicked()));
        //this->connect(this->ui->advEtherCancelBut, SIGNAL(clicked()), this, SLOT(advEtherCancelBut_clicked()));
        //this->connect(this->ui->cbEtherLinkRate, SIGNAL(currentIndexChanged(int)), this, SLOT(advEtherLinkRateCombo_changed(int)));
        //this->connect(this->ui->advWifiStationInfoBut, SIGNAL(clicked()), this, SLOT(advWifiStationInfoBut_clicked()));
        //this->connect(this->ui->advWifiScanAPsBut , SIGNAL(clicked()), this, SLOT(advWifiScanAPsBut_clicked()));
        //this->connect(this->ui->advWifiScanAPsBut, SIGNAL(pressed()), this, SLOT(advWifiScanAPsBut_pressed()));
        this->connect(this->ui->advReportsNetworkConfigUSBBut, SIGNAL(clicked()), this, SLOT(advReportsNetworkConfigUSBBut_clicked()));
        this->connect(this->ui->advReportsNetworkScreenshotUSBBut, SIGNAL(clicked()), this, SLOT(advReportsNetworkScreenshotUSBBut_clicked()));
        this->connect(this->ui->advReportsNetworkAPInfoUSBBut, SIGNAL(clicked()), this, SLOT(advReportsNetworkAPInfoUSBBut_clicked()));
        this->connect(this->ui->toolsPingTestBut, SIGNAL(toggled(bool)), this, SLOT(advstartPingBut_click(bool)));
        this->connect(advpingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(advprintPingOutput()));
        this->connect(advpingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(advfinishedProcess(int,QProcess::ExitStatus)));
        this->connect(advpingProcess,SIGNAL(readyReadStandardError()),this,SLOT(advreadyReadStandardError()));
        this->connect(memoryProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(advprintMemoryOutput()));
        this->connect(memoryProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(advfinishedMemoryProcess(int,QProcess::ExitStatus)));
        this->connect(memoryProcess,SIGNAL(readyReadStandardError()),this,SLOT(advMemoryReadyReadStandardError()));
        this->connect(advTracerouteProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(advprintTracerouteOutput()));
        this->connect(advTracerouteProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(advfinishedProcessTraceroute(int,QProcess::ExitStatus)));
        this->connect(advTracerouteProcess,SIGNAL(readyReadStandardError()),this,SLOT(advreadyReadStandardErrorTraceroute()));
        this->connect(this->ui->toolsTracerouteTest, SIGNAL(clicked()), this, SLOT(toolsTracerouteBut_clicked()));
        this->connect(this->ui->ToolsClearBut, SIGNAL(clicked()), this, SLOT(toolsClearBut_clicked()));
        this->connect(this->ui->toolsScanWirelessAPsBut, SIGNAL(clicked()), this, SLOT(toolsScanWirelessAPsBut_clicked()));
        this->connect(this->ui->toolsScanWirelessAPsBut, SIGNAL(pressed()), this, SLOT(toolsScanWirelessAPsBut_pressed()));
        this->connect(this->ui->toolsWirelessLoggingBut, SIGNAL(toggled(bool)), this, SLOT(toolsWirelessLoggingBut_toggled(bool)));
        this->connect(this->ui->statusEthernetConnectionDetailsBut, SIGNAL(clicked()), this, SLOT(statusEthernetConnectionDetailsBut_clicked()));
        this->connect(this->ui->statusLinkStatesBut, SIGNAL(clicked()), this, SLOT(statusLinkStatesBut_clicked()));
        this->connect(this->ui->statusNICConfigurationsBut, SIGNAL(clicked()), this, SLOT(statusNICConfigurationsBut_clicked()));
        this->connect(this->ui->statusWirelessConnectionDetailsBut, SIGNAL(clicked()), this, SLOT(statusWirelessConnectionDetailsBut_clicked()));
        this->connect(this->ui->StatusClearBut, SIGNAL(clicked()), this, SLOT(statusClearBut_clicked()));
        this->connect(this->ui->advReportsSaveAllBut, SIGNAL(clicked()), this, SLOT(advReportsSaveAllBut_clicked()));
        this->connect(this->ui->memoryStatesBut, SIGNAL(clicked()), this, SLOT(memoryStatesBut_click()));
        this->connect(this->ui->memoryClearBut, SIGNAL(clicked()), this, SLOT(memoryClearBut_click()));
        this->connect(this->ui->reportsSaveDumpStatesToUSBBut, SIGNAL(clicked()), this, SLOT(reportsSaveMemoryStatesBut_click()));
        this->connect(this->ui->advWirelessTabRefreshBut, SIGNAL(clicked()),this, SLOT(wirelessRefreshBut_clicked()));
        this->connect(this->ui->advWirelessTabRefreshBut, SIGNAL(pressed()), this, SLOT(wirelessRefreshBut_pressed()));
        this->connect(this->ui->debugBut, SIGNAL(clicked()), this, SLOT(debugBut_clicked()));
        this->connect(this->ui->closeDebugBut, SIGNAL(clicked()), this, SLOT(closeDebugBut_clicked()));
        this->connect(this->ui->wirelesstw->horizontalHeader(),SIGNAL(sectionClicked(int)), this, SLOT(wirelessHeaderClicked(int)));

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
        ui->cbTransferRate->addItem(" 36 Mbsp",QString("36M"));
        ui->cbTransferRate->addItem(" 48 Mbsp",QString("48M"));
        ui->cbTransferRate->addItem(" 54 Mbsp  - Mode G ",QString("54M"));
        ui->cbTransferRate->addItem(" 150 Mbsp - Mode N", QString("149M"));

        ui->cbChannel->clear();
        ui->cbChannel->addItem(tr("Auto : Roaming"), "auto");
        ui->cbChannel->addItem(" Channel 1 : 2.412 GHz","2412");
        ui->cbChannel->addItem(" Channel 2 : 2.417 GHz","2417");
        ui->cbChannel->addItem(" Channel 3 : 2.422 GHz","2422");
        ui->cbChannel->addItem(" Channel 4 : 2.427 GHz","2427");
        ui->cbChannel->addItem(" Channel 5 : 2.432 GHz","2432");
        ui->cbChannel->addItem(" Channel 6 : 2.437 GHz","2437");
        ui->cbChannel->addItem(" Channel 7 : 2.442 GHz","2442");
        ui->cbChannel->addItem(" Channel 8 : 2.447 GHz","2447");
        ui->cbChannel->addItem(" Channel 9 : 2.452 GHz","2452");
        ui->cbChannel->addItem(" Channel 10 : 2.457 GHz","2457");
        ui->cbChannel->addItem(" Channel 11 : 2.462 GHz","2462");
        ui->cbChannel->addItem(" Channel 149 : 5.745 GHz","5745");
        ui->cbChannel->addItem(" Channel 153 : 5.765 GHz","5765");
        ui->cbChannel->addItem(" Channel 157 : 5.785 GHz","5785");
        ui->cbChannel->addItem(" Channel 161 : 5.805 GHz","5805");
        ui->cbChannel->addItem(" Channel 165 : 5.825 GHz","5825");

        //ui->cbTransferRate->clear();


        /*ui->cbAdvWifiTransferRate->clear();
        ui->cbAdvWifiTransferRate->addItem(" Automatic",QString("auto"));
        ui->cbAdvWifiTransferRate->addItem(" 1 Mbps",QString("1M"));
        ui->cbAdvWifiTransferRate->addItem(" 11 Mbps  - Mode B",QString("11M"));
        ui->cbAdvWifiTransferRate->addItem(" 24 Mbps",QString("24M"));
        ui->cbAdvWifiTransferRate->addItem(" 36 Mbsp",QString("36M"));
        ui->cbAdvWifiTransferRate->addItem(" 48 Mbsp",QString("48M"));
        ui->cbAdvWifiTransferRate->addItem(" 54 Mbsp  - Mode G ",QString("54M"));
        ui->cbAdvWifiTransferRate->addItem(" 150 Mbsp - Mode N", QString("149M"));

        ui->cbEtherLinkRate->clear();
        ui->cbEtherLinkRate->addItem(" Automatic",QString("auto"));
        ui->cbEtherLinkRate->addItem(" 10 Mbps",QString("10"));
        ui->cbEtherLinkRate->addItem(" 100 Mbps",QString("100"));
        ui->cbEtherLinkRate->addItem(" 1000 Mbps - Gigabit",QString("1000"));

        ui->cbAdvWifiChannel->clear();
        ui->cbAdvWifiChannel->addItem(tr("Auto : Roaming"), "auto");
        ui->cbAdvWifiChannel->addItem(" Channel 1 : 2.412 GHz","2412");
        ui->cbAdvWifiChannel->addItem(" Channel 2 : 2.417 GHz","2417");
        ui->cbAdvWifiChannel->addItem(" Channel 3 : 2.422 GHz","2422");
        ui->cbAdvWifiChannel->addItem(" Channel 4 : 2.427 GHz","2427");
        ui->cbAdvWifiChannel->addItem(" Channel 5 : 2.432 GHz","2432");
        ui->cbAdvWifiChannel->addItem(" Channel 6 : 2.437 GHz","2437");
        ui->cbAdvWifiChannel->addItem(" Channel 7 : 2.442 GHz","2442");
        ui->cbAdvWifiChannel->addItem(" Channel 8 : 2.447 GHz","2447");
        ui->cbAdvWifiChannel->addItem(" Channel 9 : 2.452 GHz","2452");
        ui->cbAdvWifiChannel->addItem(" Channel 10 : 2.457 GHz","2457");
        ui->cbAdvWifiChannel->addItem(" Channel 11 : 2.462 GHz","2462");
        ui->cbAdvWifiChannel->addItem(" Channel 149 : 5.745 GHz","5745");
        ui->cbAdvWifiChannel->addItem(" Channel 153 : 5.765 GHz","5765");
        ui->cbAdvWifiChannel->addItem(" Channel 157 : 5.785 GHz","5785");
        ui->cbAdvWifiChannel->addItem(" Channel 161 : 5.805 GHz","5805");
        ui->cbAdvWifiChannel->addItem(" Channel 165 : 5.825 GHz","5825");
*/
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

        //ui->ipAddressLineEdit->setText(dbipAddress);
      //  ui->subnetMaskLineEdit->setText(dbnetmask);
      //  ui->gatewayLineEdit->setText(dbgateway);

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

        if(dbfirstTimeExport != "1")
        {
            this->ui->exportBut->setEnabled(false);
            this->ui->exportBut->setDisabled(true);
        }
        else
        {
            this->ui->exportBut->setEnabled(true);
            this->ui->exportBut->setDisabled(false);
        }

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
        if(dbConnectionType == "1")
        {
           // this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
           // this->ui->alertsLineEdit->setText("LinkRate index:" + QString::number(ui->cbTransferRate->findData(dbLinkRate)) + " Channelindex:" + QString::number(ui->cbChannel->findData(dbChannel)));


            ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(dbChannel));
  //          ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(dbChannel));
            //////////////
           // channelIndex = ui->cbChannel->currentIndex();
    //        channelIndex = ui->cbAdvWifiChannel->currentIndex();

            ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(dbLinkRate));
      //      ui->cbAdvWifiTransferRate->setCurrentIndex(ui->cbAdvWifiTransferRate->findData(dbLinkRate));

          //   linkRateIndex = ui->cbTransferRate->currentIndex();
        //    linkRateIndex = ui->cbAdvWifiTransferRate->currentIndex();

                    if (dbAutomaticLinkRate == "0")
                    {
                        ui->chkboxTransferRateAuto->setChecked(false);
          //              ui->chkboxTransferRateAuto_2->setChecked(false);
                        automaticLinkRate = false;
                    }
                    else
                    {
                        ui->chkboxTransferRateAuto->setChecked(true);
            //            ui->chkboxTransferRateAuto_2->setChecked(true);
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
/*            if(ui->cbAdvWifiTransferRate->currentIndex() == 0 && ui->cbAdvWifiChannel->currentIndex() == 0)
            {
                // default settings so no advanced config
                ui->chkboxTransferRateAuto->setChecked(true);
                ui->chkboxTransferRateAuto_2->setChecked(true);
                ui->networkAdvancedBut->setStyleSheet("");
                ui->networkAdvancedBut->setIcon(QIcon());
            }
            else
            {
                //advanced config has been set
                howMany = 0;
                if(ui->cbAdvWifiTransferRate->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(!ui->chkboxTransferRateAuto_2->isChecked())
                {
                    howMany = howMany + 1;
                }
                if(ui->cbAdvWifiChannel->currentIndex() > 0)
                {
                    howMany = howMany + 1;
                }
                if(howMany > 0)
                {
                    ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
                    ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
                }
            }
  */      }
        else
        {
            ui->cbChannel->setCurrentIndex(0);
    //        ui->cbAdvWifiChannel->setCurrentIndex(0);
            ui->cbTransferRate->setCurrentIndex(0);
      //      ui->cbAdvWifiTransferRate->setCurrentIndex(0);
            ui->chkboxTransferRateAuto->setChecked(true);
        //    ui->chkboxTransferRateAuto_2->setChecked(true);
            ui->networkAdvancedBut->setStyleSheet("");
            ui->networkAdvancedBut->setIcon(QIcon());

        }

    }
   /* howMany = 0;
 //   if(ui->cbAdvWifiTransferRate->currentIndex() > 0)
    {
        howMany = howMany + 1;
    }
 //   if(!ui->chkboxTransferRateAuto_2->isChecked())
    {
        howMany = howMany + 1;
    }
   // if(ui->cbAdvWifiChannel->currentIndex() > 0)
    {
        howMany = howMany + 1;
    }
    if(howMany > 0)
    {
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
    }
    else
    {
       ui->networkAdvancedBut->setStyleSheet("");
       ui->networkAdvancedBut->setIcon(QIcon());
    }
    networkTabAlreadyLoaded = true;
    ui->advNetworkingTabWidget->removeTab(1);
    ui->advNetworkingTabWidget->removeTab(0);
*/
    networkTabAlreadyLoaded = true;
    ui->mainTab->hide();
    ui->advNetworkingFrame->raise();
    ui->advNetworkingFrame->show();

    //QString styleSheet = "HeaderView::section {color: white; background-color: rgb(40,70,40); border: 1px solid green; margin: 1px; text-align: center;}";
    //ui->wirelesstw->horizontalHeader()->setStyleSheet(styleSheet);
    //ui->wirelesstw->verticalHeader()->setStyleSheet(styleSheet);
    processingAButton = false;
}
void systemDiagnostics::closeCertInfoButClicked()
{
    ui->teCertInfo->setText("");
    ui->lblCertInfoSourceFilename->setText("");
    ui->teCertInfo->setText("");
    ui->gbCertInfo->hide();
}

void systemDiagnostics::closeDebugBut_clicked()
{
    ui->gbDebug->hide();
}

void systemDiagnostics::debugBut_clicked()
{
    ui->gbDebug->show();
}

void systemDiagnostics::advNetworkingCloseBut_clicked()
{
    howMany = 0;
//    if(ui->cbAdvWifiChannel->currentIndex() > 0) howMany = howMany + 1;
 //   if(!ui->chkboxAdvEtherRateAuto->isChecked()) howMany = howMany + 1;
  //  if(ui->cbAdvWifiTransferRate->currentIndex() > 0) howMany = howMany + 1;
   // if(ui->cbEtherLinkRate->currentIndex() > 0 ) howMany = howMany + 1;
    //if(!ui->chkboxTransferRateAuto_2 > 0) howMany = howMany + 1;

    if(howMany > 0)
    {
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
    }
    else
    {
        ui->networkAdvancedBut->setStyleSheet("");
        ui->networkAdvancedBut->setIcon(QIcon());
    }
    ui->leToolsPingTraceroute->setText("");
    ui->teStatus->clear();
    ui->teTools->clear();
    statusAnchorNumber = 0;
    toolsAnchorNumber = 0;
    ui->advNetworkingFrame->hide();
}

void systemDiagnostics::statusClearBut_clicked()
{
    ui->teStatus->clear();
    statusAnchorNumber = 0;
}

void systemDiagnostics::advancedNetworkingBut_clicked()
{
    //ui->gbAdvancedNetworking->show();
    ui->advNetworkingTabWidget->setCurrentIndex(0);
    ui->advNetworkingFrame->show();
}

void systemDiagnostics::advancedNetworkingCancelBut_clicked()
{
  //  ui->cbAdvWifiTransferRate->setCurrentIndex(linkRateIndex);
  //  ui->chkboxTransferRateAuto_2->setChecked(automaticLinkRate);
  //  ui->cbAdvWifiChannel->setCurrentIndex(channelIndex);
/*
    if(ui->cbAdvWifiTransferRate->currentIndex() == 0 && ui->cbAdvWifiChannel->currentIndex() == 0)
    {
        // default settings so no advanced config
        ui->chkboxTransferRateAuto_2->setChecked(true);
        ui->networkAdvancedBut->setStyleSheet("");
        ui->networkAdvancedBut->setIcon(QIcon());
    }
    else
    {
        //advanced config has been set
        ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
        howMany = 0;
        if(ui->cbAdvWifiTransferRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxTransferRateAuto_2->isChecked())
        {
            howMany = howMany + 1;
        }
        if(ui->cbAdvWifiChannel->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        // temporary
        if(howMany > 0)
        {
            ui->networkAdvancedBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
        }
        else
        {
            ui->networkAdvancedBut->setIcon(QIcon());
            ui->networkAdvancedBut->setStyleSheet("");
        }
        //ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
    }
*/
    //ui->gbAdvancedNetworking->hide();
    //ui->advNetworkingTabWidget->hide();
}

void systemDiagnostics::advancedNetworkingLinkRateCombo_changed(int /*index*/)
{
  ;/*  if(index == 0)
    {
        ui->chkboxTransferRateAuto_2->setChecked(true);
        ui->chkboxTransferRateAuto_2->setEnabled(false);
    }
    else
    {
        ui->chkboxTransferRateAuto_2->setEnabled(true);
    }
    */
}

void systemDiagnostics::advancedNetworkingApplyBut_clicked()
{
//    linkRateIndex = ui->cbTransferRate->currentIndex();
   /* linkRateIndex = ui->cbAdvWifiTransferRate->currentIndex();
//    automaticLinkRate = ui->chkboxTransferRateAuto->isChecked();
//    channelIndex = ui->cbChannel->currentIndex();
    automaticLinkRate = ui->chkboxTransferRateAuto_2->isChecked();
    channelIndex = ui->cbAdvWifiChannel->currentIndex();
    if(ui->cbAdvWifiTransferRate->currentIndex() == 0 && ui->cbAdvWifiChannel->currentIndex() == 0)
    {
        // default settings so no advanced config
        ui->chkboxTransferRateAuto_2->setChecked(true);
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
        if(ui->cbAdvWifiTransferRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxTransferRateAuto_2->isChecked())
        {
            howMany = howMany + 1;
        }
        if(ui->cbAdvWifiChannel->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        // temporary
        if(howMany > 0)
        {
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
        }
        else
        {
            ui->networkAdvancedBut->setIcon(QIcon());
        }
        */
   // }
    //ui->gbAdvancedNetworking->hide();
}

void systemDiagnostics::advancedNetworkingDefaultsBut_clicked()
{
    ui->chkboxTransferRateAuto->setChecked(true);
    ui->cbTransferRate->setCurrentIndex(0);
    ui->cbChannel->setCurrentIndex(0);
//    ui->cbAdvWifiChannel->setCurrentIndex(0);
//    ui->cbAdvWifiTransferRate->setCurrentIndex(0);
//    ui->chkboxTransferRateAuto_2->setChecked(true);
}

////////////
void systemDiagnostics::advEtherCancelBut_clicked()
{
  //  ui->cbEtherLinkRate->setCurrentIndex(etherLinkRateIndex);
    //ui->chkboxAdvEtherRateAuto->setChecked(etherAutomaticLinkRate);

   /* if(ui->cbEtherLinkRate->currentIndex() == 0)
    {
        // default settings so no advanced config
        ui->chkboxAdvEtherRateAuto->setChecked(true);
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
     if(ui->cbEtherLinkRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxAdvEtherRateAuto->isChecked())
        {
            howMany = howMany + 1;
        }
        // temporary
        if(howMany > 0)
        {
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
        }
        else
        {
            ui->networkAdvancedBut->setIcon(QIcon());
        }
        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
//        ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge" + QString::number(howMany) + ".png"));
    }
*/
    //ui->gbAdvancedNetworking->hide();
    //ui->advNetworkingTabWidget->hide();
}

void systemDiagnostics::advEtherLinkRateCombo_changed(int /*index*/)
{
/*    if(index == 0)
    {
        ui->chkboxAdvEtherRateAuto->setChecked(true);
        ui->chkboxAdvEtherRateAuto->setEnabled(false);
    }
    else
    {
        ui->chkboxAdvEtherRateAuto->setEnabled(true);
    }
*/
}

void systemDiagnostics::advWifiScanAPsBut_pressed()
{
/*    ui->frmAdvWirelessWait->setGeometry(3,3,954,517);
    ui->frmAdvWirelessWait->raise();
    ui->frmAdvWirelessWait->show();
*/
}

void systemDiagnostics::advWifiScanAPsBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
//    ui->teWifiTab->clear();
    QString result;
    result = " ";
    if (pythonProcess.execCommand("getFullAPInfo", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
        }
        else
        {
            result = tr("\n\n\n    No wireless station info.");
  //          ui->teWifiTab->setText(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No wireless station info.");
    //    ui->teWifiTab->setText(result);
        return;
        // TODO:
        // What do we do here?
    }
        ////qDebug() << "result APs is : " << result;
        ///
        ///
        /// Process result to remove the MAC address from the SSID showing up
        ///
    QStringList resultArray;
    QStringList tempLineArray;
    //QStringList ssidRemoved;
    resultArray.clear();
    tempLineArray.clear();
    resultArray = result.split("\n",QString::KeepEmptyParts);

    if(resultArray.size() > 8)
    {
        int j = 1;

        for(int i = 0; i < resultArray.size(); i++)
        {
            if(resultArray[i].contains("BSS",Qt::CaseInsensitive) && resultArray[i].contains("(on wlan0)"))
            {

      //          if(i > 0) ui->teWifiTab->append("<BR>");
                //Customers do not want the MAC of the AP shown
                //ssidRemoved.append("Access Point : " + QString::number(i));
                tempLineArray.clear();
                if(resultArray.size() > (i+8))
                {
                    tempLineArray = resultArray[i+8].split(":", QString::KeepEmptyParts);

                    if(!tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
                        tempLineArray.clear();
                        tempLineArray = resultArray[i+7].split(":", QString::KeepEmptyParts);
                        if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                        {
                            if(tempLineArray.size() > 1)
                            {
        //                        ui->teWifiTab->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");

                            }
                        }
                    }
                    else
                    {
                        if(tempLineArray.size() > 1)
                        {
          //                  ui->teWifiTab->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");
                        }
                    }
                    //ui->teWifiTab->append("<HR />");
                }
                else if(resultArray.size() > i+7)
                {
                    tempLineArray = resultArray[i+7].split(":", QString::KeepEmptyParts);
                    if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
                        if(tempLineArray.size() > 1)
                        {
            //                ui->teWifiTab->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");

                        }
                    }
                }
                j = j + 1;
            }
            else
            {
                tempLineArray.clear();
                tempLineArray = resultArray[i].split(":",QString::KeepEmptyParts);
                if(tempLineArray.size() > 1)
                {
                    if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
              //          ui->teWifiTab->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else if(tempLineArray[0].contains("signal", Qt::CaseSensitive))
                    {
                //        ui->teWifiTab->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else if(tempLineArray[0].contains("DS Parameter set", Qt::CaseSensitive))
                    {
                  //      ui->teWifiTab->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else
                    {

                    //    ui->teWifiTab->append("&nbsp;&nbsp;&nbsp;<B>" + tempLineArray[0] + ":</B>" + tempLineArray[1]);
                    }
                }
            }

        }
    }
    else
    {
//        ui->teWifiTab->setText(tr("\n\n\n   No Wireless networks detected."));
    }
  //  QScrollBar *vScrollBar = ui->teWifiTab->verticalScrollBar();
  //  vScrollBar->triggerAction(QScrollBar::SliderToMinimum);

    //ui->frmAdvWirelessWait->hide();
    update();
    qApp->processEvents();

}

void systemDiagnostics::statusEthernetConnectionDetailsBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
    args.append("-M");
    args.append("1");
    args.append("-E");
    args.append("1");


    QString result;
    result = " ";
    ui->teStatus->append("<BR />");
    QString anchorName = QString::number(statusAnchorNumber);
    statusAnchorNumber = statusAnchorNumber + 1;
    ui->teStatus->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
    ui->teStatus->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>ETHERNET CONNECTION DETAILS</B></FONT></CENTER></TD></TR></TABLE>");

    if (pythonProcess.execCommand("ethernetConnectionDetails", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
            ui->teStatus->append(result);
        }
        else
        {
            result = tr("\n\n\n    No details available.");
            ui->teStatus->append(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No details available.");
        ui->teStatus->append(result);
        return;
        // TODO:
        // What do we do here?
    }
    ;
    ui->teStatus->scrollToAnchor(anchorName);
}

void systemDiagnostics::statusLinkStatesBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
    args.append("-M");
    args.append("1");
    args.append("-E");
    args.append("1");
    args.append("-s");
    args.append("1");
    args.append("-L");
    args.append("1");


    QString result;
    result = " ";
    ui->teStatus->append("<BR />");
    QString anchorName = QString::number(statusAnchorNumber);
    statusAnchorNumber = statusAnchorNumber + 1;
    ui->teStatus->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
    ui->teStatus->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>LINK STATES</B></FONT></CENTER></TD></TR></TABLE>");

    if (pythonProcess.execCommand("statusLinkDetails", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
            ui->teStatus->append(result);
        }
        else
        {
            result = tr("\n\n\n    No details available.");
            ui->teStatus->append(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No details available.");
        ui->teStatus->append(result);
        return;
        // TODO:
        // What do we do here?
    }
    ;;
    ui->teStatus->scrollToAnchor(anchorName);
}

void systemDiagnostics::statusNICConfigurationsBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
    args.append("-f");
    args.append("1");

    QString result;
    result = " ";
    ui->teStatus->append("<BR />");
    QString anchorName = QString::number(statusAnchorNumber);
    statusAnchorNumber = statusAnchorNumber + 1;
    ui->teStatus->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");

    ui->teStatus->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>NIC CONFIGURATIONS</B></FONT></CENTER></TD></TR></TABLE>");
    if (pythonProcess.execCommand("statusNICConfigurations", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
            ui->teStatus->append(result);
        }
        else
        {
            result = tr("\n\n\n    No details available.");
            ui->teStatus->append(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No details available.");
        ui->teStatus->append(result);
        return;
        // TODO:
        // What do we do here?
    }
    ;;
    ui->teStatus->scrollToAnchor(anchorName);
}

void systemDiagnostics::statusWirelessConnectionDetailsBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
    args.append("-s");
    args.append("1");
    args.append("-w");
    args.append("1");
    args.append("-L");
    args.append("1");


    QString result;
    result = " ";
    ui->teStatus->append("<BR />");
    QString anchorName = QString::number(statusAnchorNumber);
    statusAnchorNumber = statusAnchorNumber + 1;
    ui->teStatus->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");

    ui->teStatus->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>WIRELESS CONNECTION DETAILS</B></FONT></CENTER></TD></TR></TABLE>");
    //QScrollBar *vScrollBar = ui->teStatus->verticalScrollBar();
    //vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
    //ScrollBar->triggerAction(QScrollBar::SliderToMaximum);
    if (pythonProcess.execCommand("wirelessConnectionDetails", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
            ui->teStatus->append(result);
        }
        else
        {
            result = tr("\n\n\n    No details available.");
            ui->teStatus->append(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No details available.");
        ui->teStatus->append(result);
        return;
        // TODO:
        // What do we do here?
    }
    ;
    ui->teStatus->scrollToAnchor(anchorName);
}

void systemDiagnostics::toolsScanWirelessAPsBut_pressed()
{
    ui->frmToolsWait->setGeometry( scaleRectForScreen( QRect( 3, 3, 954, 517 ) ) );
    ui->frmToolsWait->raise();
    ui->frmToolsWait->show();

}

void systemDiagnostics::wirelessRefreshBut_pressed()
{
    ui->frmWirelessWait->setGeometry( scaleRectForScreen( QRect( 2, 2, 954, 517 ) ) );
    ui->frmWirelessWait->raise();
    ui->frmWirelessWait->show();
}

void systemDiagnostics::toolsScanWirelessAPsBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
//    ui->teTools->clear();
    QString result;
    result = " ";
    ui->teTools->append("<BR />");
    QString anchorName = QString::number(toolsAnchorNumber);
    toolsAnchorNumber = toolsAnchorNumber + 1;
    ui->teTools->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");

    ui->teTools->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>SCAN WIRELESS ACCESS POINTS</B></FONT></CENTER></TD></TR></TABLE>");

    if (pythonProcess.execCommand("getFullAPInfo", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
        }
        else
        {
            result = tr("\n\n\n    No wireless station info.");
            ui->teTools->append(result);
            return;
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
         }
     }
    else
    {
        result = tr("\n\n\n    No wireless station info.");
        ui->teTools->append(result);
        return;
        // TODO:
        // What do we do here?
    }
        ////qDebug() << "result APs is : " << result;
        ///
        ///
        /// Process result to remove the MAC address from the SSID showing up
        ///
    QStringList resultArray;
    QStringList tempLineArray;
    //QStringList ssidRemoved;
    resultArray.clear();
    tempLineArray.clear();
    resultArray = result.split("\n",QString::KeepEmptyParts);

    if(resultArray.size() > 8)
    {
        int j = 1;

        for(int i = 0; i < resultArray.size(); i++)
        {
            if(resultArray[i].contains("BSS",Qt::CaseInsensitive) && resultArray[i].contains("(on wlan0)"))
            {

                if(i > 0) ui->teTools->append("<BR>");
                //Customers do not want the MAC of the AP shown
                //ssidRemoved.append("Access Point : " + QString::number(i));
                tempLineArray.clear();
                if(resultArray.size() > (i+8))
                {
                    tempLineArray = resultArray[i+8].split(":", QString::KeepEmptyParts);

                    if(!tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
                        tempLineArray.clear();
                        tempLineArray = resultArray[i+7].split(":", QString::KeepEmptyParts);
                        if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                        {
                            if(tempLineArray.size() > 1)
                            {
                                ui->teTools->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");

                            }
                        }
                    }
                    else
                    {
                        if(tempLineArray.size() > 1)
                        {
                            ui->teTools->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");
                        }
                    }
                    //ui->teWifiTab->append("<HR />");
                }
                else if(resultArray.size() > i+7)
                {
                    tempLineArray = resultArray[i+7].split(":", QString::KeepEmptyParts);
                    if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
                        if(tempLineArray.size() > 1)
                        {
                            ui->teTools->append("<table width=\"100%\" bgcolor=\"#005500\"><tr><td><B><FONT COLOR=#FFFFFF SIZE=+1>Access Point " + QString::number(j) + ":" + tempLineArray[1] +"</FONT></B></td></tr></table>");

                        }
                    }
                }
                j = j + 1;
            }
            else
            {
                tempLineArray.clear();
                tempLineArray = resultArray[i].split(":",QString::KeepEmptyParts);
                if(tempLineArray.size() > 1)
                {
                    if(tempLineArray[0].contains("SSID", Qt::CaseSensitive))
                    {
                        ui->teTools->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else if(tempLineArray[0].contains("signal", Qt::CaseSensitive))
                    {
                        ui->teTools->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else if(tempLineArray[0].contains("DS Parameter set", Qt::CaseSensitive))
                    {
                        ui->teTools->append("&nbsp;&nbsp;&nbsp;<B><FONT COLOR=#ee0000>" + tempLineArray[0] + ":" + tempLineArray[1] + "</FONT></B>");
                    }
                    else
                    {

                        ui->teTools->append("&nbsp;&nbsp;&nbsp;<B>" + tempLineArray[0] + ":</B>" + tempLineArray[1]);
                    }
                }
            }

        }
    }
    else
    {
        ui->teTools->append(tr("\n\n\n   No Wireless networks detected."));
    }
    ui->frmToolsWait->lower();
    ui->frmToolsWait->hide();

    ui->teTools->scrollToAnchor(anchorName);

}

void systemDiagnostics::advWifiStationInfoBut_clicked()
{
    python pythonProcess;
    QStringList args;
    args.clear();
    QString result;
    if (pythonProcess.execCommand("getWifiStationInfo", args))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            result = pythonProcess.readStdOut();
        }
        else
        {
            result = tr("\n\n\n    No wireless station info.");
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
  //  ui->teWifiTab->setText(result);
}

void systemDiagnostics::advEtherApplyBut_clicked()
{
//    linkRateIndex = ui->cbTransferRate->currentIndex();
    //etherLinkRateIndex = ui->cbEtherLinkRate->currentIndex();
   // etherAutomaticLinkRate = ui->chkboxAdvEtherRateAuto->isChecked();
   /* if(ui->cbAdvWifiTransferRate->currentIndex() == 0)
    {
        // default settings so no advanced config
        ui->chkboxAdvEtherRateAuto->setChecked(true);
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
        if(ui->cbEtherLinkRate->currentIndex() > 0)
        {
            howMany = howMany + 1;
        }
        if(!ui->chkboxAdvEtherRateAuto->isChecked())
        {
            howMany = howMany + 1;
        }
        // temporary
        if(howMany > 0)
        {
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
        }
        else
        {
            ui->networkAdvancedBut->setIcon(QIcon());
        }

    }
    //ui->gbAdvancedNetworking->hide();
*/
}

void systemDiagnostics::advEtherDefaultsBut_clicked()
{
  //  ui->chkboxAdvEtherRateAuto->setChecked(true);
    //ui->cbEtherLinkRate->setCurrentIndex(0);
}

////////////
void systemDiagnostics::searchTrustedRootCertButClicked()
{

    ///////xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    QString result;
    QString source = ui->trustedRootStoreCombo->itemData(ui->trustedRootStoreCombo->currentIndex()).toString();
    python pythonProcess;
    QStringList args;
    QString certFile = "settingswizard/TRUSTEDROOTSTORE/" + source;
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

void systemDiagnostics::clientCertDeleteButClicked()
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
        ui->frameDeleteClientCertPrompt->show();
    }
}

void systemDiagnostics::clearWpaSupplicantConfAndDisconnect()
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

void systemDiagnostics::deleteClientCertYesBut_clicked()
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
            clearWpaSupplicantConfAndDisconnect();
        }

        deleteCert(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()), "client");

        loadClientCertsCombo();
        ui->clientCertStoreCombo->setCurrentIndex(0);
    }
    //ui->gbDeleteClientCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
}

void systemDiagnostics::deleteClientCertCancelBut_clicked()
{
    //ui->gbDeleteClientCertPrompt->hide();
    ui->frameDeleteClientCertPrompt->hide();
}

void systemDiagnostics::serverCertDeleteButClicked()
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

        ui->frameDeleteServerCertPrompt->show();
    }
}

void systemDiagnostics::deleteServerCertYesBut_clicked()
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
            clearWpaSupplicantConfAndDisconnect();
        }
        deleteCert(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()), "ca");
        loadTrustedRootCertsCombo();
        ui->trustedRootStoreCombo->setCurrentIndex(0);
    }
    //ui->gbDeleteServerCertPrompt->hide();
    ui->frameDeleteServerCertPrompt->hide();
}

void systemDiagnostics::deleteServerCertCancelBut_clicked()
{
    //ui->gbDeleteServerCertPrompt->hide();
    ui->frameDeleteServerCertPrompt->hide();
}


void systemDiagnostics::deleteCert(QString certName, QString certType)
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

void systemDiagnostics::deleteActiveCert(QString certName)
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


bool systemDiagnostics::checkIfActiveCert(QString certName, QString certType)
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

void systemDiagnostics::searchClientCertButClicked()
{

    ///////xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    QString result;
    QString source = ui->clientCertStoreCombo->itemData(ui->clientCertStoreCombo->currentIndex()).toString();
    python pythonProcess;
    QStringList args;
    QString certFile = "settingswizard/CLIENTCERTSTORE/" + source;
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

void systemDiagnostics::loadTrustedRootCertsCombo()
{
    QStringList rootCertsList = getStoredCerts("TRUSTEDROOTSTORE");
    ui->trustedRootStoreCombo->clear();
    ui->trustedRootStoreCombo->addItem(tr("Choose a certificate"), "");
    for (int i = 0; i < rootCertsList.size(); i++)
    {
        ui->trustedRootStoreCombo->addItem(rootCertsList.at(i),rootCertsList.at(i));
    }

}

void systemDiagnostics::updateServerCertFilename(int userSelection)
{
    dbServerCertFilename = ui->trustedRootStoreCombo->itemData(userSelection).toString();
}

void systemDiagnostics::updateClientCertFilename(int userSelection)
{
    dbClientCertUsersFilename = ui->clientCertStoreCombo->itemData(userSelection).toString();
}

void systemDiagnostics::loadClientCertsCombo()
{
    QStringList clientCertsList = getStoredCerts("CLIENTCERTSTORE");
    ui->clientCertStoreCombo->clear();
    ui->clientCertStoreCombo->addItem(tr("Choose a certificate"), "");
    for (int i = 0; i < clientCertsList.size(); i++)
    {
        ui->clientCertStoreCombo->addItem(clientCertsList.at(i),clientCertsList.at(i));
    }
}

void systemDiagnostics::setWirelessFrames(int)
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

void systemDiagnostics::signingRequestButClicked()
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

void systemDiagnostics::importCertFiles(QString certToCopy, QString certType)
{
    QStringList args;
    QString certDir = "";

    QString databaseFileName = "settingswizard/";
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

        python pythonProcess;
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

        python pythonProcess;
        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                //updateDB("clientCertFilename", dbClientCertFilename);//
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
        //this->ui->alertsLineEdit->setText(tr("Problem installing certificates."));
       // this->ui->signingRequestLabel->setText(tr("Error."));
    }
    else
    {
        //this->ui->alertsLineEdit->setText(tr("Certificates installed successfully."));
       // this->ui->signingRequestLabel->setText(tr("Completed."));
    }
}

void systemDiagnostics::copyCertsFiles(QString certToCopy)
{
    QStringList args;
    QString databaseFileName = "settingswizard/" + dbServerCertFilename;
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

        python pythonProcess;
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
    else if (certToCopy == "Client Cert")
    {
        // Client cert and key in a p12 format only for Delhaize

        args.clear();
        args.append("-S");
        args.append(clientCertFilename);
        args.append("-D");
        databaseFileName = "settingswizard/" + dbClientCertFilename;
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

        python pythonProcess;
        if (pythonProcess.execCommand("installCert", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                updateDB("clientCertFilename", dbClientCertFilename);//
                updateDB("clientCertUsersFilename", dbClientCertUsersFilename);
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

        python pythonProcess;
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
        updateDB("clientKeyFilename",dbClientKeyFilename);
        this->ui->alertsLineEdit->setText(tr("Problem installing certificates."));
       // this->ui->signingRequestLabel->setText(tr("Error."));
    }
    else
    {
        this->ui->alertsLineEdit->setText(tr("Certificates installed successfully."));
       // this->ui->signingRequestLabel->setText(tr("Completed."));
    }
}

QString systemDiagnostics::getFileSelected(QString dialogBanner)
{
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName;
    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    if ( isLinux() )                                            // Will have to modify if IMPORTING, but we use this 95% of the time
        startDirStr = "/media/thumb";                           // has to exist
    else
        startDirStr = "D:/backup/";                             // If windows

    if ( !QDir(startDirStr).exists() )
    {
        if ( isLinux() )                                        // Will have to modify if IMPORTING, but we use this 95% of the time
            startDirStr = "/media";                             // has to exist
        else
            startDirStr = "E:/backup/";                         // If windows - other mounted drive
    }

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

QString systemDiagnostics::getDirSelected(QString dialogBanner)
{
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName;
    FileDialog dialog( mainWindow_ );
    //dialog.setFilter( formatStr );
    // Only a directory can be chosen.

    if ( isLinux() )                                            // Will have to modify if IMPORTING, but we use this 95% of the time
        startDirStr = "/media/thumb";                           // has to exist
    else
        startDirStr = "D:/backup/";                             // If windows

    if ( !QDir(startDirStr).exists() )
    {
        if ( isLinux() )                                        // Will have to modify if IMPORTING, but we use this 95% of the time
            startDirStr = "/media";                             // has to exist
        else
            startDirStr = "E:/backup/";                         // If windows - other mounted drive
    }

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

void systemDiagnostics::serverCertButClicked()
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

void systemDiagnostics::chainFileButClicked()
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

void systemDiagnostics::clientCertButClicked()
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

QStringList systemDiagnostics::populateWEPKeys()
{
    processingAButton = true;
    //open etcwpa_supplicant.conf file and parse wepkeys
    QString wpaSupplicantFilename = "/etc/wpa_supplicant.conf";

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

void systemDiagnostics::checkAgainstPoorServiceTraining(QString text)
{
    //^([a-zA-Z]+[a-zA-Z0-9\-]*[a-zA-Z0-9]+)$
    //ui->hostnameLabel->setAlignment(Qt::AlignHCenter);
    ui->hostnameLabel->setText(tr("Hostname"));
    if(ui->hostnameLineEdit->text() == "" || text == "")
    {
        ui->setHostnameBut->setEnabled(false);
    }
    else
    {
        ui->setHostnameBut->setEnabled(true);
    }
}

void systemDiagnostics::checkAgainstEmptyPassword(QString text)
{
    ui->setScalePasswordBut->setEnabled( text.isEmpty() ? false : true );
}

void systemDiagnostics::shutdown() // antiquated
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

void systemDiagnostics::setDateTimeText()
{
    QString text( session_->uiUtilities->updateDateTime() );

    // convert case if required
    if ( convertUppercase_ )
        text = text.toUpper();

    ui->CurrentDateTimeTimeLabel->setText(text);
    //ui->ntpCurrentTimeLabel->setText( text );
    //ui->timeTabWidget->setTabText(1, text);
}

void systemDiagnostics::onSessionMessage( const SessionMessage& msg )
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
        else if(changedHostname || changedSpeakers)
        {
            this->shutdown();
        }
    }
    // date and time
    else if (SessionDev == msg[0].toInt()  && SnWriteDateTime == msg[1].toInt(0))
    {
        this->setDateTimeText();
    }
}

bool systemDiagnostics::isErrorMessageHandler( const SessionMessage& errorMsg ) const
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

void systemDiagnostics::reboot()
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

void systemDiagnostics::gcdSliderChanged(int value)
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
    /{
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
QStringList systemDiagnostics::buildImportConfigurationOptions(Network * locNetwork) {

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

    if ( isLinux() )                                            // Will have to modify if IMPORTING, but we use this 95% of the time
        startDirStr = "/media/thumb";                           // has to exist
    else
        startDirStr = "D:/backup/";                             // If windows

    if ( !QDir(startDirStr).exists() )
    {
        if ( isLinux() )                                        // Will have to modify if IMPORTING, but we use this 95% of the time
            startDirStr = "/media";                             // has to exist
        else
            startDirStr = "E:/backup/";                         // If windows - other mounted drive
    }

//    dialog.setAcceptMode( QFileDialog::AcceptSave );

    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( startDirStr );
  //  dialog.setFileMode( QFileDialog::DirectoryOnly );
  //  dialog.setOptions( QFileDialog::ShowDirsOnly );


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


    if(fileName.isNull()) {//user clicked cancel
        processingAButton = false;

        return options;
    }

    //options.append("python/qmSettings/network/importConfigs.py");
    options.append(filesystem::absoluteFilePath("importConfigs.py",folderType::Python));
    options.append("-F");
    options.append(fileName);

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

void systemDiagnostics::dhcpRadio_disableIPs()
{

    if(this->ui->dhcpRadio->isChecked())
    {
        //this->ui->subnetMaskLineEdit->setText("");
        //this->ui->ipAddressLineEdit->setText("");
        //this->ui->gatewayLineEdit->setText("");
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
}

void systemDiagnostics::controlNICs(QString device, QString direction)
{
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

void systemDiagnostics::staticIPRadio_enableIPs()
{
    if(this->ui->staticIPRadio->isChecked())
    {
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
        //this->ui->subnetMaskLineEdit->setText("");
        //this->ui->ipAddressLineEdit->setText("");
        //this->ui->gatewayLineEdit->setText("");
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

void systemDiagnostics::setTheStoredNetworkFields()
{
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

    //ui->ipAddressLineEdit->setText(dbipAddress);
  //  ui->subnetMaskLineEdit->setText(dbnetmask);
  //  ui->gatewayLineEdit->setText(dbgateway);

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
}

//////////////////////////////////////////////////////
//
//      N E T W O R K    T A B L E W I D G E T    S E L E C T I O N    C H A N G E D   S L O T
//
//////////////////////////////////////////////////////

void systemDiagnostics::networkTableWidgetSelectionChanged(int row)
{
    //processingAButton = true;

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
            if(howMany > 0)
            {
                ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
            }
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


//////////////////////////////////////////////////////
//
//      S E T   C O M M O N    N E T W O R K
//
//////////////////////////////////////////////////////
void systemDiagnostics::setCommonNetwork(Network *locNetwork)
{
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

}

void systemDiagnostics::clearNetworkData()
{
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
    this->ui->dhcpRadio->setChecked(false);
    // Per Darryl Needler on 8/12/2013 set default static and manual DNS
    this->ui->staticIPRadio->setChecked(true);
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
}

void systemDiagnostics::addNetworkBut_click()
{
    ui->networkLoadingLabel->setText("Select\nDevice");

    ui->yesGatewayBut->hide();
    ui->noGatewayBut->hide();
    ui->newWiredNetworkBut->raise();
    ui->newWirelessNetworktBut->raise();
    ui->newWiredNetworkBut->show();
    ui->newWirelessNetworktBut->show();
    ui->gbNetworkLoading->raise();
    ui->gbNetworkLoading->show();
}

void systemDiagnostics::newWirelessNetworkBut_click()
{
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
}

void systemDiagnostics::newWiredNetworkBut_click()
{
    newConnectionFlag = "Wire";
    this->clearNetworkData();
    tryingAHiddenSSID = false;
    this->ui->availableConnectionsTableView->clearSelection();
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
}

//////////////////////////////////////////////////////
//
//      R E V E A L   N E T W O R K   G R O U P B O X
//
//////////////////////////////////////////////////////

void systemDiagnostics::revealNetworkGroupBox(int box, int row)
{
    this->clearNetworkData();
    switch (box)
    {
    case 1: //WIRED
        //this->ui->dnsGroupBox->hide();
        //this->ui->vpnGroupBox->hide();
//        this->ui->wiredAdapterGroupBox->setGeometry(320,7,331,161);
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 196, 256, 201 ) ) );
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 13, 25, 171 ) ) );

       // this->ui->macAddressFixedLabel->setGeometry(494, 326, 101, 20);
       // this->ui->macAddressLabel->setGeometry(595, 327, 138,20);

        this->ui->wirelessAdapterGroupBox->hide();
        this->ui->wirelessAuthenticationGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
    //    this->ui->macAddressFixedLabel->setGeometry(340, 179, 121, 20);
   //     this->ui->macAddressLabel->setGeometry(468, 180, 171, 20);
        this->ui->staticIPRadio->setChecked(true);
        this->ui->manualDNSRadio->setChecked(true);
        this->ui->autoDNSRadio->setEnabled(false);
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
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 13, 256, 171 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 310, 196, 256, 201 ) ) );

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
                if(howMany > 0)
                {
                    ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
                }
            }
        }
        this->ui->dhcpRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(true);
        break;
    case 4: // HIDDEN OR NEW WIRELESS
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 13, 256, 171 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 310, 196, 256, 201 ) ) );

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
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 13, 256, 171 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 310, 196, 256, 201 ) ) );

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
}

void systemDiagnostics::revealNetworkGroupBoxPrefilled(int isWireless)
{
    //this->clearNetworkData();
    switch (isWireless)
    {
    case 1: //WIRED
        //this->ui->dnsGroupBox->hide();
        //this->ui->vpnGroupBox->hide();
//        this->ui->wiredAdapterGroupBox->setGeometry(320,7,331,161);
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 196, 256, 201 ) ) );
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 486, 13, 256, 171 ) ) );

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
        this->ui->wiredAdapterGroupBox->setGeometry( scaleRectForScreen( QRect( 312, 13, 256, 171 ) ) );
        this->ui->dnsGroupBox->setGeometry( scaleRectForScreen( QRect( 310, 196, 256, 201 ) ) );

        //this->ui->vpnGroupBox->hide();
        this->ui->wiredAdapterGroupBox->show();
        this->ui->wirelessAdapterGroupBox->show();
        this->ui->dnsGroupBox->show();
        this->ui->wirelessAuthenticationGroupBox->show();
        break;
    }
}

//////////////////////////////////////////////////////
//
//      L O A D   W I R E L E S S   S E C U R I T Y   M E T H O D S   C O M B O
//
//////////////////////////////////////////////////////

void systemDiagnostics::loadWirelessSecurityMethodsCombo()
{
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
}

//////////////////////////////////////////////////////
//
//      D A T E   S E L E C T I O N   S L O T
//
//////////////////////////////////////////////////////

void systemDiagnostics::datePicked(QDate selectionDate)
{
    pickedDate->setDate(selectionDate.year(),selectionDate.month(),selectionDate.day());  // ->setDate(selectionDate.currentDate()); //->setDate(selectionDate);
}

//////////////////////////////////////////////////////
//
//      S E T   W I R E L E S S   N E T W O R K   E N C R Y P T I O N   C O M B O
//
//////////////////////////////////////////////////////

void systemDiagnostics::setWirelessNetworkEncryptionCombo(int securityMethodItemNum)
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

void systemDiagnostics::astronomicalCheckboxValueChanged(int value)
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

void systemDiagnostics::calibrateScreenBut_click()
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
}

//////////////////////////////////////////////////////
//
//      T E S T    V O L U M E    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void systemDiagnostics::testVolumeBut_click()
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
                // QT5 PORT - This doesn't exist in QT5
                //Phonon::createPath(mediaObject, audioOutput);
                //createPath(mediaObject, audioOutput);
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

void systemDiagnostics::playAudioFile()
{
    //processingAButton = true;

    if (currentPlaybackIndex < stereoTestFiles.length())
    {
        // QT5 PORT - original version
        //mediaObject->setCurrentSource(filesystem::absoluteFilePath(stereoTestFiles.at(currentPlaybackIndex),folderType::Audio));
        // QT5 PORT = version 2.0
        //QString aPath = filesystem::absoluteFilePath(stereoTestFiles.at(currentPlaybackIndex),folderType::Audio);
        //mediaObject->setMedia(QUrl::fromLocalFile(aPath));
        //audioOutput->setVolume(1.0);
        //mediaObject->play();
        //currentPlaybackIndex++;

        // QTPORT VERSION 3.0
        QString mediaPath( filesystem::absoluteFilePath(stereoTestFiles.at(currentPlaybackIndex),folderType::Audio) );
        player->load( mediaPath, false );
        player->setVolume( 1.0 );
        player->play();
        currentPlaybackIndex++;
    }
    else
    {
        // QT5 PORT
        // mediaObject->stop();
        player->stop();
    }
    //processingAButton = false;
}

//////////////////////////////////////////////////////
//
//       S E T   B R I G H T N E S S   I N D I C A T O R S
//
//////////////////////////////////////////////////////

void systemDiagnostics::setBrightnessIndicators(int value)
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

void systemDiagnostics::setVolumeIndicators(int value)
{
    QString objectName;
    for(int i = 0; i <= 10; i++)
    {
        objectName = "volumeIndicator" + QString::number(i);

        if (i <= value/10 && value != 0) //turn on
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(:/images/screens/brightnessLED.png);");
        //    this->ui->volumeResultsLabel->setText(tr("Volume is on"));

        }
        else // turn off
        {
            if(value == 0)
            {
               this->ui->volumeResultsLabel->setText(tr("Muted"));
            }
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(130, 130, 130);background:url(:/images/screens/brightnessLEDoff.png);");

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

void systemDiagnostics::muteButChanged(bool checked)
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    //volume = this->ui->volumeSpinBox->value();
    if(checked)
    {
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:20px;border: 1px solid rgb(0,120,120);background:url(:/images/screens/volumeOnSmall.png);");
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
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px; border: 1px solid gray;background:url(:/images/screens/volumeOffSmall.png);");
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

void systemDiagnostics::changedOperatorSoundButtons()
{
    this->ui->operatorSoundBut->setChecked(true);
    this->ui->operatorSoundButText->setStyleSheet("color: black;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLED.png);");
    this->ui->customerSoundButText->setStyleSheet("color: black;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLEDoff.png);");
    this->ui->customerSoundBut->setChecked(false);
    loadSoundVolumeValues("o");
    this->ui->volumeSpinBox->setMinimum(0);
    this->ui->volumeSpinBox->setMaximum(1);
    this->ui->testVolumeBut->hide();
}

void systemDiagnostics::changedCustomerSoundButtons()
{
    ui->volumeSpinBox->setMinimum(0);
    ui->volumeSpinBox->setMaximum(100);
    this->ui->customerSoundBut->setChecked(true);
    this->ui->customerSoundButText->setStyleSheet("color: black;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLED.png);");
    this->ui->operatorSoundButText->setStyleSheet("color: black;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLEDoff.png);");
    this->ui->operatorSoundBut->setChecked(false);
    loadSoundVolumeValues("c");
    firstTimeCustomerVolume = false;
    this->ui->testVolumeBut->show();
}

void systemDiagnostics::changedOperatorDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->operatorScreenBut->setChecked(true);
    this->ui->operatorDisplayButText->setStyleSheet("color: black;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLED.png);");
    this->ui->customerDisplayButText->setStyleSheet("color: black;");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLEDoff.png);");
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

void systemDiagnostics::changedCustomerDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->customerScreenBut->setChecked(true);
    this->ui->customerDisplayButText->setStyleSheet("color: white;");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLED.png);");
    this->ui->operatorDisplayButText->setStyleSheet("color: black;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/screens/brightnessLEDoff.png);");
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

void systemDiagnostics::volumeSpinBoxValueChanged(int percentage)
{
  //  this->ui->setVolumeBut->setText(tr("Set volume to ") + QString::number(percentage) + tr("%"));
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

void systemDiagnostics::resizeControls()
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

void systemDiagnostics::translate()
{

        ui->retranslateUi(this);
        this->resizeControls();

        // update connections to update translations in table view
        if(isReadyToTranslate)
        {
            this->ui->availableConnectionsTableView->clear();
            loadNetworkConnections();
           // bool isChecked = ui->operatorSoundBut->isChecked();
            //muteButChanged(isChecked);
            //muteButChanged()
            this->ui->alertsLineEdit->setText(tr("Change Display Settings"));       // set to default text
            // The font size needs to update as well and immediately for the ON/OFF fields when
            // the language option is pressed. There is a possibility that the word ON may overflow its bounds
            int curPercent = this->ui->volumeLabel->text().toInt();
            volumeSpinBoxValueChanged(curPercent);
        }

        update();

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

void systemDiagnostics::readStdOutput()
{
    //   QString chunk = readAllStandardOutput();
    //  this->ui->networkResultsLabel->setText(chunk);
}

bool systemDiagnostics::mergeNewDBEntries(QString key, QString value)
{
    //processingAButton = true;

    QString databaseFileName = "settingswizard/scalesettings";
    QString answer = "";
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

QString systemDiagnostics::getDBValue(QString key)
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

QStringList systemDiagnostics::getStoredCerts(QString type)
{
    QStringList certsList;
    if(isLinux())
    {
        //get Directory Listing
        QString certsDirName = "settingswizard/" + type;
        QString certsLocation = filesystem::absoluteFilePath(certsDirName, folderType::DBs);

        QDir certsDir(certsLocation);
        certsDir.setFilter(QDir::Files);
        certsDir.setSorting(QDir::Name | QDir::IgnoreCase);
        certsList = certsDir.entryList();
    }
    return certsList;
}

void systemDiagnostics::connectDB()
{
    //processingAButton = true;

   QString databaseFileName = "settingswizard/scalesettings";
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
           else if(theLine.at(0) == "wirelessLogging") dbWirelessLogging = theLine.at(1);

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
           else if(theLine.at(0) == "timezone") dbTimezone = "America/Detroit";
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
           else if(theLine.at(0) == "ntpServer") dbNTPServer = "0.0.0.0";
           else if(theLine.at(0) == "ntpFrequency") dbNTPFrequency = "0";
           else if(theLine.at(0) == "ntpDayOfWeek") dbNTPDayOfWeek = "0";
           else if(theLine.at(0) == "ntpTimeOfDay") dbNTPTimeOfDay = "03:30:00";
           else if(theLine.at(0) == "ntpTimeZone") dbNTPTimeZone = "America/Detroit";
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
           else if(theLine.at(0) == "wirelessLogging") dbWirelessLogging = "0";

       }
   }
   file.close();

   //processingAButton = false;

}

void systemDiagnostics::disconnectDB()
{
    //database.close();
}

void systemDiagnostics::loadSoundVolumeValues(QString device)
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
    else
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
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px;border: 1px solid gray;background:url(:/images/screens/volumeOnSmall.png);");
    }
    else
    {
        this->ui->muteBut->setChecked(false);
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px;border: 1px solid gray;background:url(:/images/screens/volumeOffSmall.png);");
    }
}

void systemDiagnostics::loadDisplayBrightnessValues(QString display)
{
    // Get DB display brightness values
    if(display == "o")
    {

        this->ui->brightnessLabel->setText(operatorBrightness);
        this->ui->brightnessSpinBox->setValue(operatorBrightness.toInt());
        this->ui->operatorScreenBut->setChecked(true);
    }
    else
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
int systemDiagnostics::wiredSpeed()
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
            ;// TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
     }
     else
     {
        ;// TODO:
        // What do we do here?
     }

    // result = true;
    //processingAButton = false;
    return speed;
}

void systemDiagnostics::wirelessTWCellPressed(int row, int column)
{
    //processingAButton = true;

    QTableWidgetItem *wirelessItem = new QTableWidgetItem;
    wirelessItem = this->ui->wirelesstw->item(row,column);

    // HT-156
    if( wirelessItem )
    {
        QString ssidContents = wirelessItem->text();
        if(ssidContents != "")
        {
            //ui->teDebug->append("Number of SSIDs on this channel: " + QString::number(numSSIDs));
            QStringList qtyssids = ssidContents.split("\n",QString::KeepEmptyParts);
            ui->debugNumSSIDs->setText(tr("Number of SSID's: ") + QString::number(qtyssids.size()));
            if(column == 11)
            {
                ui->debugChannelLabel->setText(tr("Mixed Channels"));
              //  ui->frame_4->setGeometry(210,40,551,451);
              //  ui->teDebug->setGeometry(24,80,501,350);
            }
            else if (column < 11)
            {
                ui->debugChannelLabel->setText(tr("Channel ") + QString::number(column+1));
              //  ui->frame_4->setGeometry(310,40,351,451);
              //  ui->teDebug->setGeometry(24,80,301,350);
            }
            ui->teDebug->clear();
            ui->teDebug->append(ssidContents);
            QScrollBar *vScrollBar = ui->teDebug->verticalScrollBar();
            vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
            ui->gbDebug->show();
        }
    }
}

void systemDiagnostics::wirelessHeaderClicked(int column)
{
    //get data for the whole column

    QString finalString = "";

    for (int i = 0; i < 5; i++)
    {
        QTableWidgetItem *iterItem = new QTableWidgetItem;
        iterItem = ui->wirelesstw->item(i,column);
        if(iterItem->text() != "")
        {
            finalString += iterItem->text() + "\n";
        }
    }
    if(finalString != "")
    {
        ui->teDebug->clear();
        int numSSIDs = 0;
        switch(column)
        {

        case 0:
            numSSIDs = col1Count;
            break;
        case 1:
            numSSIDs = col2Count;
            break;
        case 2:
            numSSIDs = col3Count;
            break;
        case 3:
            numSSIDs = col4Count;
            break;
        case 4:
            numSSIDs = col5Count;
            break;
        case 5:
            numSSIDs = col6Count;
            break;
        case 6:
            numSSIDs = col7Count;
            break;
        case 7:
            numSSIDs = col8Count;
            break;
        case 8:
            numSSIDs = col9Count;
            break;
        case 9:
            numSSIDs = col10Count;
            break;
        case 10:
            numSSIDs = col11Count;
            break;
        case 11:
            numSSIDs = col12Count;
            break;
        }

        //ui->teDebug->append("Number of SSIDs on this channel: " + QString::number(numSSIDs));
        ui->debugNumSSIDs->setText(tr("Number of SSID's: ") + QString::number(numSSIDs));
        if(column == 11)
        {
            ui->debugChannelLabel->setText(tr("Mixed Channels"));
           // ui->frame_4->setGeometry(210,40,551,451);
           // ui->teDebug->setGeometry(24,80,501,350);
        }
        else if (column < 11)
        {
            ui->debugChannelLabel->setText(tr("Channel ") + QString::number(column+1));
          //  ui->frame_4->setGeometry(310,40,351,451);
          //  ui->teDebug->setGeometry(24,80,301,350);
        }
        ui->teDebug->append(finalString);
        QScrollBar *vScrollBar = ui->teDebug->verticalScrollBar();
        vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
        ui->gbDebug->show();
    }
}

QStringList systemDiagnostics::getWirelessAPChartInfo()
{
    // QProcess workConfiguration;
    QStringList APs;
    QString result;
    python pythonProcess;
    QStringList options;
    //options.append("python/qmSettings/network/qmSettingsNetwork.py");
    //ui->alertsLineEdit->setText(alertsText);

    //options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));
    options.append("-X");
    options.append("1");

    if(!isDemoMode)
    {
        if (pythonProcess.execCommand("getWirelessAPInfo", options))
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
        ;//result = "4|foodRouter1|3|WAP-deli|1|Bakery-Router|5|Hobart|5|Manager-AP|3|ProduceRouter|2|Marketing-AP|2|";
    }

    ////qDebug() << "result APs is : " << result;
    APs = result.split("|", QString::KeepEmptyParts);
    // Format is : Channel Frequency SignalBars SSID   (4 parts)
    return APs;
}

void systemDiagnostics::wirelessRefreshBut_clicked()
{

    //this->loadWirelessConnectionsTable();
    //processingAButton = true;
    isReadyToTranslate = false;

    col1Count = 0;
    col2Count = 0;
    col3Count = 0;
    col4Count = 0;
    col5Count = 0;
    col6Count = 0;
    col7Count = 0;
    col8Count = 0;
    col9Count = 0;
    col10Count = 0;
    col11Count = 0;
    col12Count = 0;

    foundSelectedConnection = false;

    this->ui->wirelesstw->clearContents();
    this->ui->wirelesstw->clear();

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 12; j++)
        {
            QTableWidgetItem *aConnectionItem = new QTableWidgetItem;
            aConnectionItem->setText("");
            aConnectionItem->setBackground(QColor(10,10,10));
            aConnectionItem->setForeground(QColor(255,255,255));
            ui->wirelesstw->setItem(i,j,aConnectionItem);
        }
    }

    //this->ui->availableConnectionsTableView->setStyleSheet("QTableWidget{background-color: white;border: 1px solid gray;} QTableWidget::item{ background-color: white;border: 0px solid white;} QTableWidget::item:selected{background: rgb(255, 255, 255); color: rgb(91, 153, 25); border-top: 6px solid rgb(135, 202, 0); border-bottom: 6px solid rgb(135, 202, 0);} QTableWidget::item:focus {border: 0px; }");
    //this->ui->wirelesstw->setStyleSheet("QTableWidget{background-color: white;border: 1px solid gray;} QTableWidget::item{ background-color: white;border: 0px solid white;} QTableWidget::item:selected{background: rgb(255, 255, 255); color:rgb(0,0,0); border-top: 6px solid rgb(135, 202, 0); border-bottom: 6px solid rgb(135, 202, 0);} QTableWidget::item:focus {border: 0px; }");

    // Format is 4 parts: Channel Frequency SignalBars SSID
    APList = this->getWirelessAPChartInfo();

    ui->wirelesstw->setRowCount(5);
    ui->wirelesstw->setColumnCount(12);
    //72,90
    ui->wirelesstw->setRowHeight( 0, scaleWidthForScreen( 87 ) );
    ui->wirelesstw->setRowHeight( 1, scaleWidthForScreen( 87 ) );
    ui->wirelesstw->setRowHeight( 2, scaleWidthForScreen( 87 ) );
    ui->wirelesstw->setRowHeight( 3, scaleWidthForScreen( 87 ) );
    ui->wirelesstw->setRowHeight( 4, scaleWidthForScreen( 87 ) );
    QString channel = "";
    QString frequency = "";
    QString signalBars = "";
    QString ssid = "";
    int columnNumber;
    ui->teDebug->clear();

    ui->debugLabel->setText(QString::number(APList.size()));
    for(int i = 0; i < (APList.size() / 4); i++)
    {
        channel = APList.at(i * 4);
        switch(APList.at(i*4).toInt())
        {
        case 1:
            col1Count = col1Count + 1;
            break;
        case 2:
            col2Count = col2Count + 1;
            break;
        case 3:
            col3Count = col3Count + 1;
            break;
        case 4:
            col4Count = col4Count + 1;
            break;
        case 5:
            col5Count = col5Count + 1;
            break;
        case 6:
            col6Count = col6Count + 1;
            break;
        case 7:
            col7Count = col7Count + 1;
            break;
        case 8:
            col8Count = col8Count + 1;
            break;
        case 9:
            col9Count = col9Count + 1;
            break;
        case 10:
            col10Count = col10Count + 1;
            break;
        case 11:
            col11Count = col11Count + 1;
            break;
        case 12:
            col12Count = col12Count + 1;
            break;
        default:
            col12Count = col12Count + 1;
            break;
        }

        frequency = APList.at((i * 4)+1);
        signalBars = APList.at((i * 4)+2);
        if( APList.at((i * 4)+3).length() == 0)
        {
            ssid = tr("BLANK SSID NAME");
        }
        else
        {
            ssid = APList.at((i*4)+3);
            if(ssid == "N/A")
            {
                ssid = tr("BLANK SSID NAME");
            }
        }
        // Find the cell for channel and signal bars
        QTableWidgetItem *aConnectionItem = new QTableWidgetItem;
        QTableWidgetItem *wirelessItem = new QTableWidgetItem;
        QString newText = "";

        if(channel.toInt() > 11) columnNumber = 12;
        else columnNumber = channel.toInt();
        if(this->ui->wirelesstw->item(signalBars.toInt()-1,columnNumber-1) != 0)
        {
            wirelessItem = ui->wirelesstw->item(signalBars.toInt() - 1, columnNumber - 1);
            newText = wirelessItem->text();
        }

        if (newText.length() > 0)
        {
            if(columnNumber < 12)
            {
                newText = newText + "\n" + ssid;
            }
            else
            {
                newText = newText + "\n" + ssid + "  (Channel:" + channel + ") (Freq:" + frequency + ")";
            }
        }
        else
        {
            newText = ssid;
        }

        aConnectionItem->setText(newText);
        aConnectionItem->setBackgroundColor(QColor(0,30,0));
        aConnectionItem->setTextColor(QColor(255,255,255));
        aConnectionItem->setTextAlignment(Qt::AlignLeft);

        if(columnNumber < 12)
        {
            this->ui->wirelesstw->setItem(signalBars.toInt()-1,columnNumber-1, aConnectionItem);
        }
        else
        {
            this->ui->wirelesstw->setItem(signalBars.toInt()-1, 11, aConnectionItem);
            ;// channel is for column 12 because it is likely A band
        }
        /*for(i = 0; i < APs.size(); i++)
        {
            ui->teDebug->append(APs.at(i));
        }
        */
        ui->teDebug->append("Channel: " + channel);
        ui->teDebug->append("frequency: " + frequency);
        ui->teDebug->append("bars: " + signalBars);
        ui->teDebug->append("ssid: " + ssid);

       // ui->debugLabel->setText("ch:"+ APs.at(0) + "   fr:" + APs.at(1) + "   ba:" + APs.at(2) + "   ssid:" + APs.at(3));
    }

    ui->frmWirelessWait->hide();
}



void systemDiagnostics::refreshNetwork_click()
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

//    networkTableWidgetSelectionChanged(this->ui->availableConnectionsTableView->currentRow());

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



void systemDiagnostics::refreshNetworksPressed()
{
    ui->networkStatusLabel->show();
    ui->networkStatusLabel->setText(tr("Scanning networks...wait"));
    update();
}

void systemDiagnostics::refreshNetworksButton_click()
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
void systemDiagnostics::loadNetworkConnections()
{
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

    QFont fontStrengthRatio;
    fontStrengthRatio.setPointSize(9);
    fontStrengthRatio.setFamily("Sans Serif");
    QBrush goodSignalStrengthColor(Qt::black);
    QBrush badSignalStrengthColor(Qt::darkRed);

    for(int i = 0; i < APs.count()/4; i++)
    {

        QTableWidgetItem *adapterIconItem = new QTableWidgetItem;
        QTableWidgetItem *connectionNameNStatusItem = new QTableWidgetItem;
        QTableWidgetItem *lightIconItem = new QTableWidgetItem;
        QTableWidgetItem *strengthIconItem = new QTableWidgetItem;

        nameNstatus = APs.at(i*4 + 3);
        strengthRatio = APs.at(i*4 + 1) + "%";
        signalNoise = APs.at(i*4 + 2) + "dBm";
        signalNoiseValue = APs.at(i*4 + 2).toInt();
        if (signalNoiseValue < -69)
        {
            strengthIconItem->setForeground(badSignalStrengthColor);
            strengthIconItem->setTextColor(QColor(144, 0, 0));
        }
        else
        {
            strengthIconItem->setForeground(goodSignalStrengthColor);
            strengthIconItem->setForeground(QColor(0,0,0));
        }

        strengthIconFileName = ":/images/screens/wirelessIcon" + APs.at(i*4) + ".png";
        QPixmap adapterIconPic(strengthIconFileName);

        QIcon adapterIcon;
        adapterIcon.addPixmap(adapterIconPic,QIcon::Normal);
        adapterIcon.addPixmap(adapterIconPic,QIcon::Selected);

        adapterIconItem->setIcon(adapterIcon);

        if(dbConnectionName == nameNstatus && !foundSelectedConnection)
        {
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

        strengthIconItem->setFont(fontStrengthRatio);
        strengthIconItem->setText(strengthRatio + "\n" + signalNoise);
        strengthIconItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *wirelessGroupBoxTypeItem = new QTableWidgetItem;
        wirelessGroupBoxTypeItem->setText("2");

        this->ui->availableConnectionsTableView->setRowHeight(i+this->networkCount, 69);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,0, lightIconItem); //TBD
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,1, connectionNameNStatusItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,2, adapterIconItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,3, strengthIconItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,4, wirelessGroupBoxTypeItem);
        //////qDebug() << "selectedRow is: " << selectedConnectionRow;
    }
    if(isDemoMode) this->ui->availableConnectionsTableView->selectRow(1);
    else
    {
        if(foundSelectedConnection)
        {
                this->ui->availableConnectionsTableView->selectRow(selectedConnectionRow);

                if(selectedConnectionRow == 0)
                {
                    networkTableWidgetSelectionChanged(0);
                }
        }
        else
        {
            this->ui->availableConnectionsTableView->clearSelection();
        }
    }
    if(dbConnectionType == "1")
    {
        finalIsWireless = true;
        ui->SSIDLineEdit->setText(dbConnectionName);
    }
    else
    {
        finalIsWireless = false;
    }
    isReadyToTranslate = true;
}

void systemDiagnostics::powerDevices(QString states)
{
    // 10 = wire on  / wifi off
    // 01 = wire off / wifi on
    // 11 = wire on / wifi on
    // 00 = wire off / wifi off
    //processingAButton = true;
    python pythonProcess;
    QStringList options;
    options.append("-C");
    options.append(states);

    if(!isDemoMode)
    {
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
QStringList systemDiagnostics::getWirelessAPs()
{
    //this->ui->alertsLineEdit->setText(tr("Scanning wireless access points..."));

    //processingAButton = true;
    QProcess workConfiguration;
    QStringList APs;
    QString result;
    python pythonProcess;
    QStringList options;
    //options.append("python/qmSettings/network/qmSettingsNetwork.py");
      //ui->alertsLineEdit->setText(alertsText);

    //options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));
    options.append("-X");
    options.append("1");

   // saveNetworkConfigurationProcess->waitForStarted(100);
   // saveNetworkConfigurationProcess->start("python", this->buildNetworkCommandOptions(commonNetwork));
   //

    if(!isDemoMode)
    {
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

void systemDiagnostics::toolsClearBut_clicked()
{
    ui->teTools->clear();
    toolsAnchorNumber = 0;
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

//////////////////////////////////////////////////////
//
//       P R I N T   N E T W O R K   C O N N E C T I N G   O U T P U T
//
//////////////////////////////////////////////////////

void systemDiagnostics::printNetworkOutput()
{
    ////qDebug() << networkProcessConnected;
    ////qDebug() << "made it in network print process";
    int newRow = -1;
    bool changeLights = false;
    if(networkProcessConnected)
    {
        QString output = this->saveNetworkConfigurationProcess->readAllStandardOutput();
        //qDebug() << "just received is:" << output;
        output = output.mid(0,output.length()-1);

        if(connectionTimeout)
        {
            output = noConnectionPhrase;
        }
        ////qDebug() << " output isXXX: " << output;
        if(output == "1")
        {
            this->ui->alertsLineEdit->setText(tr("Removing DHCP leases..."));
            this->ui->connectingProgressBar->setValue(5);
        }
        else if(output == "2")
        {
            this->ui->alertsLineEdit->setText(tr("Stopping DHCP executable..."));
            this->ui->connectingProgressBar->setValue(10);
        }
        else if(output == "3")
        {
            if(ui->availableConnectionsTableView->currentRow() == 0)
            {
                this->ui->alertsLineEdit->setText(tr("Disabling the wired device..."));
            }
            else
            {
                this->ui->alertsLineEdit->setText(tr("Disabling the wireless device..."));
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
                this->ui->alertsLineEdit->setText(tr("Enabling the wired device..."));
            }
            else if (newConnectionFlag == "Wireless")
            {
                this->ui->alertsLineEdit->setText(tr("Enabling the wireless device..."));
            }
            else
            {
                if(ui->availableConnectionsTableView->currentRow() == 0)
                {
                    this->ui->alertsLineEdit->setText(tr("Enabling the wired device..."));
                }
                else
                {
                    this->ui->alertsLineEdit->setText(tr("Enabling the wireless device..."));
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
                this->ui->connectingProgressBar->setValue(40);
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
            //this->ui->alertsLineEdit->setText(tr("Connected!"));
            //this->refreshNetworksButton_click();

            //if(tryingAHiddenSSID)

            if(ui->wiredAdapterGroupBox->isVisible())
            {
                dbConnectionType = "1";
                dbConnectionName = ui->SSIDLineEdit->text();
                finalIsWireless = true;
            }
            else
            {
                dbConnectionType = "0";
                dbConnectionName = "WiredEthernet";
                finalIsWireless = false;
            }

            loadNetworkConnections();

            if(this->ui->availableConnectionsTableView->currentRow() > 0)
            {
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
                    //this->ui->availableConnectionsTableView->selectRow(-1);
                    newRow = -1;
                }
                else
                {
                    this->ui->availableConnectionsTableView->selectRow(0);
                    newRow = 0;
                }
                if(newRow == 0)
                {
                    // wired
                    this->updateDB("connectionType", "0");
                    this->updateDB("connectionName", "WiredEthernet");
                    this->dbConnectionName = "WiredEthernet";
                    dbConnectionType = "0";
                    finalIsWireless = false;
                }
                else if(newRow < 0 && ui->wiredAdapterGroupBox->isVisible())
                {
                    //it's hidden
                    this->updateDB("connectionType", "1");
                    this->updateDB("connectionName", ui->SSIDLineEdit->text());
                    dbConnectionName = ui->SSIDLineEdit->text();
                    dbConnectionType = "1";
                    finalIsWireless = true;
                }
            }
            this->ui->connectingProgressBar->hide();
            this->ui->connectingProgressBar->lower();
            this->ui->availableConnectionsTableView->setEnabled(true);
            this->ui->addNetworkBut->setEnabled(true);
            this->ui->exportBut->setEnabled(true);
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

        }
        else if(output == "NO")
        {
            connectionEstablished = false;
            connectedSuccessfully = false;
            changeLights = true;
            this->ui->alertsLineEdit->setText(tr("Not connected...Try again."));
            this->ui->exportBut->setEnabled(true);
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
            saveConfigsNow();
        }
        else if(output.mid(0,6) == "OUTPUT")
        {
            //endNetworkConnection();
            //saveNetworkConfigurationProcess->kill();
            // we have connection info like gateway, netmask, etc.
            //order is ip|gateway|metmask|dn1|dns2|dn3
            QStringList args;
            //qDebug() << "output is : " << output;
            args = output.split("|",QString::KeepEmptyParts);
            if(args.size() > 2 && connectionEstablished)
            {
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
                }
                ui->alertsLineEdit->setText(tr("Connected!") + tr(" Your IP is : ") + args[1]);
                this->update();
            }
            else
            {
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
            }
            else
            {
                this->updateDB("eapUsername", ui->eapUsernameLineEdit->text());
                this->updateDB("eapPassword", ui->eapPasswordLineEdit->text());
                this->updateDB("wpaPassphrase", "");
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
            this->ui->networkAdvancedBut->setEnabled(true);

        }
        else
        {
            ;//////qDebug() << "XX: " << output;
        }
    }
    //qApp->processEvents();

    if(changeLights)
    {
        this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);

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

void systemDiagnostics::saveConfigsNow()
{
    //processingAButton = true;
    python pythonProcess;

    if (pythonProcess.execCommand("saveConfigs", this->buildSaveConfigurationOptions(commonNetwork)))
       {
           if (pythonProcess.getExitCode() == 0)
           {
               updateDB("firstTimeExport","1");
               ui->exportBut->setEnabled(true);
               ui->exportBut->setDisabled(false);
               //TBD - updateCustomerVolume();
           }
           else
           {
               ui->exportBut->setEnabled(true);
               ui->exportBut->setDisabled(false);
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

void systemDiagnostics::changeConnectedIcons(int rowToConnect, bool connectionOKAY)
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


void systemDiagnostics::readyReadNetworkStandardError()
{
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
}

void systemDiagnostics::endNetworkConnection()
{
    //////qDebug() << "finished";

    //XXX COMMENT LINE BELOW AFTER TESTING
   // this->ui->alertsLineEdit->setText("Finished Process");

    this->ui->mainTab->setEnabled(true);
    if(mustTurnOnDialogs)
    {
       ;// session_->send(SessionDev , SnRestartDialogs);
    }
    if(networkProcessConnected)
    {
        saveNetworkConfigurationProcess->kill();
        this->ui->networkSaveBut->setEnabled(true);
        this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);
    }
    networkProcessConnected = false;
    if(!connectedSuccessfully)
    {
        this->ui->alertsLineEdit->setText(tr("Not connected... Try again."));
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

    //XXX ADDED THIS 10/14/2014
    //saveNetworkConfigurationProcess->kill();

}

void systemDiagnostics::finishedNetworkProcess(int value, QProcess::ExitStatus exitStatus)
{
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
        this->ui->exportBut->setEnabled(true);
        this->ui->importBut->setEnabled(true);
        this->ui->refreshNetworksBut->setEnabled(true);
        this->ui->availableConnectionsTableView->setEnabled(true);

    }
    networkProcessConnected = false;
    if(!connectedSuccessfully)
    {
        this->ui->alertsLineEdit->setText(tr("Not connected... Try again."));
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

}

bool systemDiagnostics::onScreenChanging()
{
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
}

void systemDiagnostics::beginTheNetworkConnection()
{
    if(ui->wpaEAPTLSPassPhraseFrame->isVisible() && (ui->trustedRootStoreCombo->currentIndex() == 0 || ui->clientCertStoreCombo->currentIndex() == 0))
    {
        ; // Do nothing because a cert was not chosen.
    }
    else
    {
        processingAButton = true;

        updateDB("networkTab", "1");
        ui->mainTab->setTabIcon(3, passIcon);

        session_->send(SessionDev, SnStopDialogs);
        mustTurnOnDialogs = true;

        networkProcessConnected = true;
        this->ui->networkSaveBut->setEnabled(false);
        this->ui->exportBut->setEnabled(false);
        this->ui->importBut->setEnabled(false);
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

        //this->ui->mainTab->setDisabled(true);

        this->ui->connectingProgressBar->setValue(0);
        this->ui->connectingProgressBar->raise();
        this->ui->connectingProgressBar->show();
        if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
        {
            this->ui->alertsLineEdit->setText(tr("Processing certificates..."));
            this->ui->connectingProgressBar->setValue(40);
            qApp->processEvents();
            changeCertFilesToUse();
        }
        else
        {
            this->ui->alertsLineEdit->setText(tr("Trying connection..."));
            qApp->processEvents();
        }

        finalIsWireless = ui->SSIDLineEdit->isVisible();

        //  OLD AND USABLE
        QStringList options = this->buildNetworkCommandOptions(commonNetwork);
        saveNetworkConfigurationProcess->waitForStarted(100);
        saveNetworkConfigurationProcess->start("python", options);

        if(!saveNetworkConfigurationProcess->waitForFinished(60000)) // was 60000
        {
            // tell the User the connection didn't succeed
            connectionTimeout = true;
            noConnectionPhrase = "NO";
            printNetworkOutput();
            noConnectionPhrase = "OUTPUT|None";
            printNetworkOutput();
            connectionTimeout = false;
            if(networkProcessConnected)
            {
                saveNetworkConfigurationProcess->kill();
                networkProcessConnected = false;
            }
        }



        // Start saving the configurations

        /* python pythonProcess;

        if (pythonProcess.execCommand("saveConfigs", this->buildSaveConfigurationOptions(commonNetwork)))
           {
               if (pythonProcess.getExitCode() == 0)
               {
                   updateDB("firstTimeExport","1");
                   ui->exportBut->setEnabled(true);
                   ui->exportBut->setDisabled(false);
                   //TBD - updateCustomerVolume();
               }
               else
               {
                   ui->exportBut->setEnabled(false);
                   ui->exportBut->setDisabled(true);
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

        if(ui->staticIPRadio->isChecked())
        {
            updateDB("isStatic","1");
        }
        else
        {
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
        }
        if(ui->availableConnectionsTableView->currentRow() > 0)
        {
            //updateDB("linkRate", ui->cbTransferRate->itemData(ui->cbTransferRate->currentIndex()).toString());
      //      updateDB("linkRate", ui->cbAdvWifiTransferRate->itemData(ui->cbAdvWifiTransferRate->currentIndex()).toString());
      //      if(ui->chkboxTransferRateAuto_2->isChecked()) updateDB("automaticLinkRate","1");
      //      else updateDB("automaticLinkRate","0");
      //      updateDB("channel",ui->cbAdvWifiChannel->itemData(ui->cbAdvWifiChannel->currentIndex()).toString());
        }

        //XXX October 20, 2014 Uncomment this if dialogs are required
        //session_->send( NetworkConnectivityDev, RequestCurrentStatus );
        loadNetworkConnections();
        processingAButton = false;
    //
    }
}

void systemDiagnostics::gatewayYesSlot()
{
    this->ui->gbNetworkLoading->hide();
    qApp->processEvents();
    this->update();
    beginTheNetworkConnection();
}

void systemDiagnostics::gatewayNoSlot()
{
    this->ui->gbNetworkLoading->hide();
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     S A V E   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void systemDiagnostics::saveNetworkBut_click()
{
    // we have to suppress the lost connection dialog
    if(ui->gatewayLineEdit->text() == "" && ui->gatewayLineEdit_2->text() == "" && ui->gatewayLineEdit_3->text() == "" && ui->gatewayLineEdit_4->text() == "" && ui->staticIPRadio->isChecked())
    {
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
        beginTheNetworkConnection();
    }

}

///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   S A V E   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList systemDiagnostics::buildSaveConfigurationOptions(Network *locNetwork) {
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

    options.append("-Z");

    //PASSPHRASE DEFINED HERE
    options.append(this->PASSPHRASE);


    return options;
}

void systemDiagnostics::toolsWirelessLoggingBut_toggled(bool checked)
{
    if(checked)
    {
        python pythonProcess;
        QStringList args;
        args.append("-S");
        args.append("1");
        if(pythonProcess.execCommand("wirelessLogging", args))
        {
            ui->toolsWirelessLoggingBut->setText(tr("STOP\nWireless Logging"));
            ui->toolsWirelessLoggingBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            //processingAButton = true;
            this->ui->toolsWirelessLoggingBut->setChecked(true);// startPingBut->setEnabled(false);
            updateDB("wirelessLogging", "1");
        }
        else
        {
            ui->toolsWirelessLoggingBut->setText(tr("START\nWireless Logging"));
            ui->toolsWirelessLoggingBut->setStyleSheet("");
            ui->toolsWirelessLoggingBut->setChecked(false);
            ui->toolsWirelessLoggingBut->setDown(false);
            ;//updateDB("wirelessLogging", "0");
        }
    }
    else
    {
        python pythonProcess;
        QStringList args;
        args.append("-S");
        args.append("0");
        if(pythonProcess.execCommand("wirelessLogging", args))
        {
            updateDB("wirelessLogging", "0");
            ui->toolsWirelessLoggingBut->setText(tr("START\nWireless Logging"));
            ui->toolsWirelessLoggingBut->setStyleSheet("");
            ui->toolsWirelessLoggingBut->setChecked(false);
            ui->toolsWirelessLoggingBut->setDown(false);
        }
        else
        {
           ;// updateDB("wirelessLogging", "1");
        }
    }
}


QString systemDiagnostics::getSerialNumber()
{
    QString serialNumberAnswer = "";
    LOG_CLASS_INFO( "systemDiagnostics", "getSerialNumber" );

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


///////////////////////////////////////////////////////////////////////////////////////
//
//     I M P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void systemDiagnostics::importNetworkBut_click() {
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
        int j = 0;

        //Go through the file format and extract the connection information
        while(j < res.size()) {
            configurationFormat.indexIn(res[j]);
            bestResults.append(configurationFormat.capturedTexts()[1]);
            j++;
        }
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
            if(bestResults[12] == "0")
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
                if(bestResults[13] == "0" || bestResults[13] == "64641964")
                {
                    ui->ipAddressLineEdit->setText("");
                    ui->ipAddressLineEdit_2->setText("");
                    ui->ipAddressLineEdit_3->setText("");
                    ui->ipAddressLineEdit_4->setText("");
                }
                else
                {
                    QStringList IPAddressParts;
                    IPAddressParts = bestResults[13].split(".", QString::KeepEmptyParts);

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
                if(bestResults[14] == "0" || bestResults[14] == "64641964")
                {
                    ui->subnetMaskLineEdit->setText("");
                    ui->subnetMaskLineEdit_2->setText("");
                    ui->subnetMaskLineEdit_3->setText("");
                    ui->subnetMaskLineEdit_4->setText("");
                }
                else
                {
                    QStringList SubnetParts;
                    SubnetParts = bestResults[14].split(".", QString::KeepEmptyParts);

                    if(SubnetParts.size() == 4)
                    {
                        ui->subnetMaskLineEdit->setText(SubnetParts.at(0));
                        ui->subnetMaskLineEdit_2->setText(SubnetParts.at(1));
                        ui->subnetMaskLineEdit_3->setText(SubnetParts.at(2));
                        ui->subnetMaskLineEdit_4->setText(SubnetParts.at(3));
                    }
                    //ui->subnetMaskLineEdit->setText(bestResults[10]);
                }
                if(bestResults[15] == "0" || bestResults[15] == "64641964")
                {
                    ui->gatewayLineEdit->setText("");
                    ui->gatewayLineEdit_2->setText("");
                    ui->gatewayLineEdit_3->setText("");
                    ui->gatewayLineEdit_4->setText("");
                }
                else
                {
                    QStringList GatewayParts;
                    GatewayParts = bestResults[15].split(".", QString::KeepEmptyParts);

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
        //        this->ui->cbAdvWifiTransferRate->setCurrentIndex(0);

                ui->chkboxTransferRateAuto->setChecked(true);
                //ui->chkboxTransferRateAuto_2->setChecked(true);
          //      this->ui->cbChannel->setCurrentIndex(0);
 //               this->ui->cbAdvWifiChannel->setCurrentIndex(0);
//
                revealNetworkGroupBoxPrefilled(3);

            }

            if(bestResults[16] == "1" || bestResults[16] == "" || bestResults[16] == "64641964")
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
                if(bestResults[17] == "0" || bestResults[17] == "64641964")
                {
                    ui->DNS1LineEdit->setText("");
                    ui->DNS1LineEdit_2->setText("");
                    ui->DNS1LineEdit_3->setText("");
                    ui->DNS1LineEdit_4->setText("");
                }
                else
                {
                    QStringList D1Parts;
                    D1Parts = bestResults[17].split(".", QString::KeepEmptyParts);

                    if(D1Parts.size() == 4)
                    {
                        ui->DNS1LineEdit->setText(D1Parts.at(0));
                        ui->DNS1LineEdit_2->setText(D1Parts.at(1));
                        ui->DNS1LineEdit_3->setText(D1Parts.at(2));
                        ui->DNS1LineEdit_4->setText(D1Parts.at(3));
                    }
                    //ui->DNS1LineEdit->setText(bestResults[13]);
                }

                if(bestResults[18] == "0" || bestResults[18] == "64641964")
                {
                    ui->DNS2LineEdit->setText("");
                    ui->DNS2LineEdit_2->setText("");
                    ui->DNS2LineEdit_3->setText("");
                    ui->DNS2LineEdit_4->setText("");
                }
                else
                {
                    QStringList D2Parts;
                    D2Parts = bestResults[18].split(".", QString::KeepEmptyParts);

                    if(D2Parts.size() == 4)
                    {
                        ui->DNS2LineEdit->setText(D2Parts.at(0));
                        ui->DNS2LineEdit_2->setText(D2Parts.at(1));
                        ui->DNS2LineEdit_3->setText(D2Parts.at(2));
                        ui->DNS2LineEdit_4->setText(D2Parts.at(3));
                    }
                    //ui->DNS2LineEdit->setText(bestResults[13]);
                }

                if(bestResults[19] == "0" || bestResults[19] == "64641964")
                {
                    ui->DNS3LineEdit->setText("");
                    ui->DNS3LineEdit_2->setText("");
                    ui->DNS3LineEdit_3->setText("");
                    ui->DNS3LineEdit_4->setText("");
                }
                else
                {
                    QStringList D3Parts;
                    D3Parts = bestResults[19].split(".", QString::KeepEmptyParts);

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

        else if(bestResults.size() == 15) { //wpa
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
            else
            {
                this->ui->securityCombo->setCurrentIndex(pos);
            }


            pos = this->ui->encryptionCombo->findData(bestResults[2]);
            if(pos == -1) {
                //////qDebug() << bestResults[2];
                //////qDebug() << "couldnt find encryption Combo here 1";
                //processingAButton = false;
                return;
            }
            else {
                this->ui->encryptionCombo->setCurrentIndex(pos);
            }

            this->ui->wpaPassphraseLineEdit->setText(bestResults[3]);
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[4]));
  //          this->ui->cbAdvWifiTransferRate->setCurrentIndex(ui->cbAdvWifiTransferRate->findData(bestResults[4]));
            if(bestResults[5] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
    //            ui->chkboxTransferRateAuto_2->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
      //          ui->chkboxTransferRateAuto_2->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[6]));
        //    this->ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(bestResults[6]));
            revealNetworkGroupBoxPrefilled(3);
        }
        else if(bestResults.size() == 16) {//leapPeap
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
                setWirelessNetworkEncryptionCombo(7);
            }
            else {
                setWirelessNetworkEncryptionCombo(2);
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

            ui->encryptionCombo->setCurrentIndex(ui->encryptionCombo->findData(bestResults[2]));
            this->ui->eapUsernameLineEdit->setText(bestResults[3]);
            this->ui->eapPasswordLineEdit->setText(bestResults[4]);
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[5]));
            if(bestResults[6] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
          //      ui->chkboxTransferRateAuto_2->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            //    ui->chkboxTransferRateAuto_2->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[7]));
         //   this->ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(bestResults[7]));
            revealNetworkGroupBoxPrefilled(3);
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
           //     ui->chkboxTransferRateAuto_2->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
             //   ui->chkboxTransferRateAuto_2->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[5]));
//            this->ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(bestResults[5]));
            revealNetworkGroupBoxPrefilled(3);


            this->ui->wpaEAPTLSPassPhraseFrame->hide();
            this->ui->wpaPassPhraseFrame->hide();
            this->ui->eapUserPassFrame->hide();
            this->ui->wepKeyFrame->hide();
            this->ui->openNoneFrame->show();
        }
        else if(bestResults.size() == 18)
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
  //          this->ui->cbAdvWifiTransferRate->setCurrentIndex(ui->cbAdvWifiTransferRate->findData(bestResults[7]));
            if(bestResults[8] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
    //            ui->chkboxTransferRateAuto_2->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
      //          ui->chkboxTransferRateAuto_2->setChecked(false);
            }

            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[9]));
        //    this->ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(bestResults[9]));
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
                this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
            }
            else if(bestResults[3] == "3")
            {
                this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
            }
            else if(bestResults[3] == "4")
            {
                this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
            }
            //this->ui->wepKey1LineEdit->setText(this->commonNetwork->WEPKey1());
            //this->ui->wepKey2LineEdit->setText(this->commonNetwork->WEPKey2());
            //this->ui->wepKey3LineEdit->setText(this->commonNetwork->WEPKey3());
            //this->ui->wepKey4LineEdit->setText(this->commonNetwork->WEPKey4());
            this->ui->cbTransferRate->setCurrentIndex(ui->cbTransferRate->findData(bestResults[8]));
            if(bestResults[9] == "1")
            {
                ui->chkboxTransferRateAuto->setChecked(true);
          //      ui->chkboxTransferRateAuto_2->setChecked(true);
            }
            else
            {
                ui->chkboxTransferRateAuto->setChecked(false);
            //    ui->chkboxTransferRateAuto_2->setChecked(false);
            }
            this->ui->cbChannel->setCurrentIndex(ui->cbChannel->findData(bestResults[10]));
       //     this->ui->cbAdvWifiChannel->setCurrentIndex(ui->cbAdvWifiChannel->findData(bestResults[10]));
            revealNetworkGroupBoxPrefilled(3);
        }

        if(doSecondAssignments)
        {
            if(bestResults[bestResults.size() - 8] == "1")
            {
                ui->dhcpRadio->setChecked(false);
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
                    D1Parts = bestResults[bestResults.size() - 3].split(".", QString::KeepEmptyParts);

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
        }
        else
        {
            // It is wired
            ui->availableConnectionsTableView->selectRow(0);
        }
        int numAdvancedNetworkSettings = 0;

//        if(ui->cbAdvWifiTransferRate->currentIndex() > 0)
        {
            numAdvancedNetworkSettings = numAdvancedNetworkSettings + 1;
        }
  //      if(!ui->chkboxTransferRateAuto_2->isChecked())
        {
            numAdvancedNetworkSettings = numAdvancedNetworkSettings + 1;
        }
    //    if(ui->cbAdvWifiChannel->currentIndex() > 0)
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
            ui->networkAdvancedBut->setIcon( QIcon(":/images/screens/badge.png"));
        }

        this->ui->alertsLineEdit->setText(tr("Network successfully imported."));
        this->ui->alertsLineEdit->setStyleSheet("color:green;");
    }
    //processingAButton = false;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     E X P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void systemDiagnostics::exportNetworkBut_click() {
    //processingAButton = true;
    QProcess exportConfigs;
    python pythonProcess;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }

    if (pythonProcess.execCommand("exportConfigs", myOptions))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            //this->manualPause();
            if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
            {
                // have to export the certs also
                //exportEAPTLSCerts.py
                QStringList certOptions;
                QString destinationDir = "";

                certOptions.append("-S");
                certOptions.append(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
                certOptions.append("-C");
                certOptions.append(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));
                certOptions.append("-D");

                // get the destination path dir for the flash drive
                if (myOptions.size() == 2)
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

                if(pythonProcess.execCommand("exportEAPTLSCerts", certOptions))
                {
                    if(pythonProcess.getExitCode() == 0)
                    {
                        ui->alertsLineEdit->setText(tr("Network configuration exported."));
                    }
                    else
                    {
                        ui->alertsLineEdit->setText(tr("Network configuration not exported."));
                    }
                }
                else
                {
                    ui->alertsLineEdit->setText(tr("Network configuration not exported."));
                }
            }
            //ui->alertsLineEdit->setText(tr("Network configuration exported."));
        }
        else
        {
            ui->alertsLineEdit->setText(tr("Network configuration not exported."));
        }
    }
    else
    {
        ;
    }

    /*
    //////qDebug() << "options are:" << myOptions;
    exportConfigs.start("python", myOptions);

    if(!exportConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Start configuration error."));
        // Start configuration error.
        // return;
    }

    if(!exportConfigs.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Finish configuration error."));
        // return;
        // Finish configuration error.
    }
    */

    //this->ui->alertsLineEdit->setText(tr("Network Successfully Exported"));
    //this->ui->alertsLineEdit->setStyleSheet("color:green;");
   // processingAButton = false;
}

void systemDiagnostics::advReportsNetworkConfigUSBBut_clicked()
{
    //processingAButton = true;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }
    else
    {

            this->ui->advNetworkingFrame->hide();
            this->ui->mainTab->show();
            this->ui->mainTab->setCurrentIndex(3);
            this->update();
            qApp->processEvents();

            //this->manualPause();
            if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
            {
                // have to export the certs also
                //exportEAPTLSCerts.py
                QStringList certOptions;
                QString destinationDir = "";

                myOptions.append("-S");
                myOptions.append(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
                myOptions.append("-C");
                myOptions.append(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));

                // get the destination path dir for the flash drive
            }
            myOptions.append("-I");
            myOptions.append("1");
            myOptions.append("-A");
            myOptions.append("1");
            myOptions.append("-c");
            myOptions.append("1");
            myOptions.append("-s");
            myOptions.append("1");
            myOptions.append("-w");
            myOptions.append("1");
            myOptions.append("-f");
            myOptions.append("1");
            myOptions.append("-L");
            myOptions.append("1");
            myOptions.append("-M");
            myOptions.append("1");
            myOptions.append("-E");
            myOptions.append("1");

            python pythonProcess;

            if(pythonProcess.execCommand("netConfig2USB", myOptions))
            {
                if(pythonProcess.getExitCode() == 0)
                {
                    ;//ui->alertsLineEdit->setText(tr("Network configuration exported."));
                }
                else
                {
                    ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
                }
            }
            else
            {
                ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
            }
            this->ui->mainTab->hide();
            this->ui->advNetworkingFrame->show();
            this->update();
            qApp->processEvents();
    }
}

void systemDiagnostics::advReportsSaveAllBut_clicked()
{
    //processingAButton = true;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }
    else
    {

        // Save other tabs now
         if(isLinux())
         {
             //Tools tab content
             QFile file(filesystem::absoluteFilePath("settingswizard/toolsTabCapture.txt", folderType::DBs));
             file.open(QIODevice::WriteOnly | QIODevice::Text);
             QTextStream out(&file);
             out << ui->teTools->toPlainText() << endl;
             file.close();

             //Status tab content
             QFile file2(filesystem::absoluteFilePath("settingswizard/statusTabCapture.txt", folderType::DBs));
             file2.open(QIODevice::WriteOnly | QIODevice::Text);
             QTextStream out2(&file2);
             out2 << ui->teStatus ->toPlainText() << endl;
             file2.close();

             //Memory tab content
             //QFile file3(filesystem::absoluteFilePath("settingswizard/memoryTabCapture.txt", folderType::DBs));
             //file3.open(QIODevice::WriteOnly | QIODevice::Text);
             //QTextStream out3(&file3);
             //out3 << ui->teStatus ->toPlainText() << endl;
             //file3.close();
         }
         else
         {
             ;//TBD for winblows
         }
        this->ui->advNetworkingFrame->hide();
        this->ui->mainTab->show();
        this->ui->mainTab->setCurrentIndex(3);
        this->update();
        qApp->processEvents();

        //this->manualPause();
        if(ui->wpaEAPTLSPassPhraseFrame->isVisible())
        {
            // have to export the certs also
            //exportEAPTLSCerts.py
            QStringList certOptions;
            QString destinationDir = "";

            myOptions.append("-S");
            myOptions.append(ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex()));
            myOptions.append("-C");
            myOptions.append(ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex()));

            // get the destination path dir for the flash drive
        }
        myOptions.append("-I");
        myOptions.append("1");
        myOptions.append("-A");
        myOptions.append("1");
        myOptions.append("-c");
        myOptions.append("1");
        myOptions.append("-s");
        myOptions.append("1");
        myOptions.append("-w");
        myOptions.append("1");
        myOptions.append("-f");
        myOptions.append("1");
        myOptions.append("-L");
        myOptions.append("1");
        myOptions.append("-M");
        myOptions.append("1");
        myOptions.append("-E");
        myOptions.append("1");
        myOptions.append("-Z");
        myOptions.append("1");
        myOptions.append("-Y");
        myOptions.append("1");
        myOptions.append("-P");
        myOptions.append(PASSPHRASE);
        myOptions.append("-T");
        myOptions.append("1");
        myOptions.append("-R");
        myOptions.append("1");

        python pythonProcess;
        if(pythonProcess.execCommand("netConfig2USB", myOptions))
        {
            if(pythonProcess.getExitCode() == 0)
            {
                ;//ui->alertsLineEdit->setText(tr("Network configuration exported."));
            }
            else
            {
                ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
            }
        }
        else
        {
            ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
        }
        this->ui->mainTab->hide();
        this->ui->advNetworkingFrame->show();
        this->update();
        qApp->processEvents();


    }
}

void systemDiagnostics::reportsSaveMemoryStatesBut_click()
{
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }

    myOptions.append("-S");
    myOptions.append("1");
    myOptions.append("-P");
    myOptions.append(this->PASSPHRASE);

    python pythonProcess;
    if(pythonProcess.execCommand("dumpState", myOptions))
    {
        if(pythonProcess.getExitCode() == 0)
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

void systemDiagnostics::advReportsNetworkAPInfoUSBBut_clicked()
{
    //processingAButton = true;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }

    myOptions.append("-A");
    myOptions.append("1");

    python pythonProcess;
    if(pythonProcess.execCommand("netConfig2USB", myOptions))
    {
        if(pythonProcess.getExitCode() == 0)
        {
            ;//ui->alertsLineEdit->setText(tr("Network configuration exported."));
        }
        else
        {
            ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
        }
    }
    else
    {
        ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
    }
}



void systemDiagnostics::advReportsNetworkScreenshotUSBBut_clicked()
{
    //processingAButton = true;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
       // processingAButton = false;
        return;
    }
    else
    {

        this->ui->advNetworkingFrame->hide();
        this->ui->mainTab->show();
        this->ui->mainTab->setCurrentIndex(3);
        this->update();
        qApp->processEvents();

        myOptions.append("-I");
        myOptions.append("1");

        python pythonProcess;
        if(pythonProcess.execCommand("netConfig2USB", myOptions))
        {
            if(pythonProcess.getExitCode() == 0)
            {
                ;//ui->alertsLineEdit->setText(tr("Network configuration exported."));
            }
            else
            {
                ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
            }
        }
        else
        {
            ;//ui->alertsLineEdit->setText(tr("Network configuration not exported."));
        }
        this->ui->mainTab->hide();
        this->ui->advNetworkingFrame->show();
        this->update();
        qApp->processEvents();
    }
}

bool systemDiagnostics::isLinux()
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
QStringList systemDiagnostics::buildExportConfigurationOptions() {
   // processingAButton = true;
    QStringList options;

    //QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"/CHOOSE/THIS/DIRECTORY");
    QString startDirStr;
    QString formatStr( tr( "*" ) );
    QString fileName = "";
    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    if ( isLinux() )                                            // Will have to modify if IMPORTING, but we use this 95% of the time
        startDirStr = "/media/thumb";                           // has to exist
    else
        startDirStr = "D:/backup/";                             // If windows

    if ( !QDir(startDirStr).exists() )
    {
        if ( isLinux() )                                        // Will have to modify if IMPORTING, but we use this 95% of the time
            startDirStr = "/media";                             // has to exist
        else
            startDirStr = "E:/backup/";                         // If windfows - other mounted drive
    }

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

    if(fileName.isNull() || fileName == "" || fileName.length() < 6) {//user clicked cancel
        return options;
    }
    else
    {
        options.append("-F");
        options.append(fileName);

        return options;
    }
}

 //////////////////////////////////
//
//     B U I L D   N E T W O R K   C O M M A N D   O P T I O N S
//
///////////////////////////////////
QStringList systemDiagnostics::buildNetworkCommandOptions(Network *locNetwork)
{
    processingAButton = true;

    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    QStringList options;

    options.append(filesystem::absoluteFilePath("setNetwork.py",folderType::Python));

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
   // processingAButton = false;
    /*QFile fOut("/usr/local/hobart/python/netA.txt");
      if (fOut.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream s(&fOut);
        for (int i = 0; i < options.size(); ++i)
          s << options.at(i) << '\n';
      } else {
        ;
        ;
      }
      fOut.close();
      */
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
    return options;
}

//////////////////////////////////
//
//     B U I L D   N E T W O R K   C O M M A N D   O P T I O N S
//
///////////////////////////////////
QStringList systemDiagnostics::buildNetworkCommandOptionsNew(Network *locNetwork)
{
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
  // processingAButton = false;
   /*QFile fOut("/usr/local/hobart/python/netA.txt");
     if (fOut.open(QFile::WriteOnly | QFile::Text)) {
       QTextStream s(&fOut);
       for (int i = 0; i < options.size(); ++i)
         s << options.at(i) << '\n';
     } else {
       ;
       ;
     }
     fOut.close();
     */
   return options;
}

void systemDiagnostics::changeCertFilesToUse()
{


    if(isLinux())
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
               // TODO:
               // What do we do here?
           }
        //processingAButton = false;
        python certProcess;

        options.clear();
        options.append("-S");
        QString rootFile = "settingswizard/TRUSTEDROOTSTORE/" + ui->trustedRootStoreCombo->itemText(ui->trustedRootStoreCombo->currentIndex());
        QString trustedRootPath = filesystem::absoluteFilePath(rootFile, folderType::DBs);
        options.append(trustedRootPath);
        options.append("-C");
        QString clientFile = "settingswizard/CLIENTCERTSTORE/" + ui->clientCertStoreCombo->itemText(ui->clientCertStoreCombo->currentIndex());
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
                ;//ui->alertsLineEdit->setText(tr(""));
                //changedHostname = true;
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
            // What do we do here?
        }
        certProcess.cleanup();
    }
}

void systemDiagnostics::clearCertificateInfoFromScaleSettings()
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

QString systemDiagnostics::modelName()
{
    /* OLD CODE */

    return "Hobart HTi";
}

///////////////////////////////////////
//
//       A U T O   D N S  R A D I O
//
///////////////////////////////////

void systemDiagnostics::autoDNSRadio_click()
{
    if(this->ui->autoDNSRadio->isChecked())
    {
        //this->ui->DNS1LineEdit->setText("");
        //this->ui->DNS2LineEdit->setText("");
        //this->ui->DNS3LineEdit->setText("");

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
}

///////////////////////////////////////
//
//       M A N U A L   D N S   R A D I O
//
///////////////////////////////////

void systemDiagnostics::manualDNSRadio_click()
{
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
        //this->ui->DNS1LineEdit->setText("");
        //this->ui->DNS2LineEdit->setText("");
        //this->ui->DNS3LineEdit->setText("");

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

}

///////////////////////////////////////////////////////////////////////////////////////
//
//     C A N C E L   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
/*void MainWindow::cancelNetworkBut_click()
{


    // Call script here,must be located in network folder on target OS
    // TBD define network folder
    ; // restore data entries
    QProcess cancelNetworkConfiguration;
    cancelNetworkConfiguration.start("applyNetworkConfiguration.pyc", QStringList() << "TBD");

    if(!cancelNetworkConfiguration.waitForStarted())
    {
        ;
        // Start configuration error.
    }


    cancelNetworkConfiguration.write(" TBD ");
    cancelNetworkConfiguration.closeWriteChannel();

    if(!cancelNetworkConfiguration.waitForFinished())
    {
        ;
        // Finish configuration error.
    }

    QByteArray result = cancelNetworkConfiguration.readAll();

   // QString program = "applyNetworkConfiguration.pyc";
   // QStringList args;
 //   args <<
   // int result = applyNetworkConfiguration->execute(program, args);

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
void systemDiagnostics::setTimeTab()
{
    ui->wirelessAdapterGroupBox_2->hide();
    this->ui->timeEdit->setTime(QTime::currentTime());
    //ui->ntpCurrentTimeLabel->setText(QTime::currentTime().toString("hh:mm AP"));

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
    //this->ui->hourBut->setText(QString("%1").arg(this->ui->timeEdit->time().hour(),2,10, QChar('0')).toUpper());
    //this->ui->minuteBut->setText(QString("%1").arg(this->ui->timeEdit->time().minute(),2,10, QChar('0')).toUpper());
    //this->ui->secondBut->setText(QString("%1").arg(this->ui->timeEdit->time().second(),2,10, QChar('0')).toUpper());
    this->ui->dateEdit->setSelectedDate(QDate::currentDate());
    this->setDateTimeText();
  //  this->ui->ntpCurrentDateLabel->setText(QDate::currentDate().toString(""));
    //ui->ntpCurrentDateLabel

    this->setTimezone();

    ui->ntpServer->setText(dbNTPServer);
    ui->ntpFrequencyCombo->setCurrentIndex(dbNTPFrequency.toInt());
    ui->ntpDayOfWeekCombo->setCurrentIndex(dbNTPDayOfWeek.toInt());
    ui->ntpTimeZoneCombo->setCurrentIndex(ui->ntpTimeZoneCombo->findData(dbNTPTimeZone));

    if(dbTimeKeeper != "Manual") ui->timeTabWidget->setCurrentIndex(2);
    else ui->timeTabWidget->setCurrentIndex(0);

    QStringList timeParts = dbNTPTimeOfDay.split(":", QString::KeepEmptyParts);

    /*
    if(dbNTPAntiMeridian == "PM")
    {
        QStringList ntpTimeSplitUp = dbNTPTimeOfDay.split(":", QString::KeepEmptyParts);
        if(ntpTimeSplitUp.at(0) == "12")
        dbNTPTimeOfDay = QString::number(ntpTimeSplitUp.at(0).toInt() + 12) + ":" + ntpTimeSplitUp.at(1) + ":" + ntpTimeSplitUp.at(2);
    }
    */

    dbNTPTimeOfDay = dbNTPTimeOfDay + " " + dbNTPAntiMeridian;
    QTime ntpTime = QTime::fromString(dbNTPTimeOfDay, "hh:mm:ss A");
    ui->ntpTimeEdit->setTime(ntpTime);
    ui->ntpHourBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(0)));
    ui->ntpMinuteBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(1)));
    ui->ntpSecondBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(2)));
    ui->ntpAmpmBut->setText(ui->ntpTimeEdit->sectionText(ui->ntpTimeEdit->sectionAt(3)));
    ui->timeTabWidget->setTabEnabled(1, false);

}

void systemDiagnostics::setHostnameBut_click()
{
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
            ui->setHostnameBut->setEnabled(true);
            updateDB("hostTab", "1");
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
}

void systemDiagnostics::setScalePasswordBut_click()
{
    QString password( ui->scalePasswordLineEdit->text() );

    if ( password.isEmpty() )
        return;

    processingAButton = true;

    // change password of user hobart
    QStringList options;
    options.append( "hobart" );
    options.append( password );

    python pythonProcess;
    if ( pythonProcess.execCommand( "setPassword", options ) )
        if ( 0 == pythonProcess.getExitCode() )
        {
            ui->alertsLineEdit->setText( tr( "Scale password has been set." ) );

            ui->setScalePasswordBut->setEnabled( false );
        }

    processingAButton = false;
}

void systemDiagnostics::rebootScale()
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

void systemDiagnostics::getUIHostname()
{
    //processingAButton = true;
    QString hostnameFileName;
    if(PLATFORM != platformType::Windows )
    {
        hostnameFileName = "/etc/hostname";
    }
    else
    {
        // it is windows
        //TBD
    }
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

}

/*
void systemDiagnostics::getConnectionStatus()
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

void systemDiagnostics::getUIMACAddress()
{
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
        this->ui->advEtherMAClbl->setText((macAddresses[0]).toUpper());
        this->ui->advWifiMAClbl->setText((macAddresses[1]).toUpper());
    }

    //processingAButton = false;

}

void systemDiagnostics::setTimezone()
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

void systemDiagnostics::setVolumeBut_click()
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

void systemDiagnostics::printPingOutput()
{
    QString output = this->pingProcess->readAllStandardOutput();
    this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
}

void systemDiagnostics::advprintPingOutput()
{
    QString output = this->advpingProcess->readAllStandardOutput();
    this->ui->teTools->append(output.mid(0,output.length()-1));
}

void systemDiagnostics::advprintMemoryOutput()
{
    QString output = this->memoryProcess->readAllStandardOutput();
    if(output.contains("<TABLE",Qt::CaseSensitive))
    {
        ui->teMemory->append(output);
    }
    else
    {
        //this->ui->teMemory->append(output.replace("\n","<BR />"));
        ui->teMemory->append(output);
    }
    //this->ui->teMemory->append(output.mid(0,output.length()-1));
}

void systemDiagnostics::advprintTracerouteOutput()
{
    QString output = this->advTracerouteProcess->readAllStandardOutput();
    this->ui->teTools->append(output.mid(0,output.length()-1));
}

void systemDiagnostics::readyReadStandardError()
{
     QString output = this->pingProcess->readAllStandardError();
     this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
     pingProcess->kill();
     this->ui->startPingBut->setEnabled(true);
}

void systemDiagnostics::finishedProcess(int value, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    pingProcess->kill();
    this->ui->startPingBut->setEnabled(true);
    processingAButton = false;

}

void systemDiagnostics::advreadyReadStandardError() // Ping
{
     QString output = this->advpingProcess->readAllStandardError();
     this->ui->teTools->append(output.mid(0,output.length()-1));
     advpingProcess->kill();
     this->ui->toolsPingTestBut->setChecked(false);
     this->ui->toolsPingTestBut->setDown(false);
     this->ui->toolsPingTestBut->setText(tr("Start\nPingTest"));
     this->ui->toolsPingTestBut->setStyleSheet("");
}

void systemDiagnostics::advMemoryReadyReadStandardError() // Memory
{
     QString output = this->memoryProcess->readAllStandardError();
     this->ui->teMemory->append(output.mid(0,output.length()-1));
     memoryProcess->kill();
}

void systemDiagnostics::advreadyReadStandardErrorTraceroute() // Traceroute
{
     QString output = this->advTracerouteProcess->readAllStandardError();
     this->ui->teTools->append(output.mid(0,output.length()-1));
     advTracerouteProcess->kill();
}

void systemDiagnostics::advfinishedProcess(int value, QProcess::ExitStatus exitStatus)
{                                              // Ping
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    advpingProcess->kill();
    this->ui->toolsPingTestBut->setChecked(false);
    this->ui->toolsPingTestBut->setText(tr("Start\nPingTest"));
    this->ui->toolsPingTestBut->setStyleSheet("");
    this->ui->toolsPingTestBut->setDown(false);
    processingAButton = false;
    ui->teTools->scrollToAnchor(QString::number(toolsAnchorNumber));
}

void systemDiagnostics::advfinishedMemoryProcess(int value, QProcess::ExitStatus exitStatus)
{                                              // Ping
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    memoryProcess->kill();
    processingAButton = false;
   // ui->teMemory->toHtml();
    ui->teMemory->scrollToAnchor(QString::number(memoryAnchorNumber));
}

void systemDiagnostics::advfinishedProcessTraceroute(int value, QProcess::ExitStatus exitStatus)
{                                              // Ping
    Q_UNUSED( value );
    Q_UNUSED( exitStatus );

    advTracerouteProcess->kill();
    processingAButton = false;
    ui->teTools->scrollToAnchor(QString::number(toolsAnchorNumber));
}

//////////////////////////////////////////////////////
//
//     S T A R T   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void systemDiagnostics::startPingBut_click()
{
    processingAButton = true;
    this->ui->startPingBut->setEnabled(false);
    this->ui->pingResultsList->clear();
    cancelPing = false;
    QStringList options;
    options.append(filesystem::absoluteFilePath("outputPing.py",folderType::Python));
    options.append(this->ui->pingIPLineEdit->text());
    pingProcess->waitForStarted(100);
    pingProcess->start("python", options);
}

void systemDiagnostics::toolsTracerouteBut_clicked()
{
    toolsAnchorNumber = toolsAnchorNumber + 1;

    QString anchorName = QString::number(toolsAnchorNumber);

    ui->teTools->scrollToAnchor(anchorName);
    ui->teTools->append("<BR />");
    ui->teTools->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
    ui->teTools->scrollToAnchor(anchorName);

    ui->teTools->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>TRACEROUTE TEST : " + ui->leToolsPingTraceroute->text() + "</B></FONT></CENTER></TD></TR></TABLE>");

    qApp->processEvents();
    if(ui->leToolsPingTraceroute->text() != "")
    {
        QString result;
        QStringList options;

        //options.append(filesystem::absoluteFilePath("customTraceroute.py",folderType::Python));
        options.append(filesystem::absoluteFilePath("traceroute.py",folderType::Python));
        options.append(ui->leToolsPingTraceroute->text());
        advTracerouteProcess->waitForStarted(100);
        advTracerouteProcess->start("python", options);
        //advTracerouteProcess->waitForFinished(20000);
    }
    else
    {
        ui->teTools->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
        this->ui->teTools->append(tr("Enter a location to trace a route to."));
        //this->advpingProcess->kill();
        processingAButton = false;
        ui->teTools->scrollToAnchor(anchorName);
    }
}

void systemDiagnostics::advstartPingBut_click(bool isChecked)
{
    if(isChecked)
    {
        toolsAnchorNumber = toolsAnchorNumber + 1;

        QString anchorName = QString::number(toolsAnchorNumber);
        ui->teTools->append("<BR />");
        ui->teTools->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
        ui->teTools->scrollToAnchor(anchorName);
        qApp->processEvents();

        ui->teTools->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>PING TEST : " + ui->leToolsPingTraceroute->text() + "</B></FONT></CENTER></TD></TR></TABLE>");
        //toolsAnchorNumber = toolsAnchorNumber + 1;

        if(ui->leToolsPingTraceroute->text() != "")
        {

            ui->toolsPingTestBut->setText(tr("Stop\nPing Test"));
            ui->toolsPingTestBut->setStyleSheet("background-color: rgb(170, 255, 127);");
            processingAButton = true;
            this->ui->toolsPingTestBut->setChecked(true);// startPingBut->setEnabled(false);
            //this->ui->teTools->clear();
            advcancelPing = false;
            QStringList options;
            options.append(filesystem::absoluteFilePath("outputPing.py",folderType::Python));
            options.append(this->ui->leToolsPingTraceroute->text());
            advpingProcess->waitForStarted(100);
            advpingProcess->start("python", options);
            //advpingProcess->waitForFinished(20000);
        }
        else
        {
            this->ui->teTools->append(tr("Enter a location to ping"));
            ui->toolsPingTestBut->setText(tr("Start\nPing Test"));
            ui->toolsPingTestBut->setStyleSheet("");
            ui->toolsPingTestBut->setChecked(false);
            ui->toolsPingTestBut->setDown(false);
            //this->advpingProcess->kill();
            processingAButton = false;
            //ui->teTools->scrollToAnchor(anchorName);
        }
    }
    else
    {
        ui->toolsPingTestBut->setText(tr("Start\nPing Test"));
        ui->toolsPingTestBut->setStyleSheet("");
        ui->toolsPingTestBut->setChecked(false);
        ui->toolsPingTestBut->setDown(false);
        this->advpingProcess->kill();
        processingAButton = false;
        ui->teTools->scrollToAnchor(QString::number(toolsAnchorNumber));
    }
}

void systemDiagnostics::memoryStatesBut_click()
{
        memoryAnchorNumber = 0;
        ui->teMemory->clear();
        memoryAnchorNumber = memoryAnchorNumber + 1;

        QString anchorName = QString::number(memoryAnchorNumber);
        ui->teMemory->append("<BR />");
        ui->teMemory->append("<A NAME=\"" + anchorName + "\" HREF=\"#word\">&nbsp;</A>");
        ui->teMemory->scrollToAnchor(anchorName);
        qApp->processEvents();

        //ui->teMemory->append("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>MEMORY TEST</B></FONT></CENTER></TD></TR></TABLE>");

        processingAButton = true;
        QStringList options;

        options.append(filesystem::absoluteFilePath("dumpState.py",folderType::Python));
        options.append("-S");
        options.append("1");
        options.append("-P");
        options.append(PASSPHRASE);
        memoryProcess->waitForStarted(100);
        memoryProcess->start("python", options);

}

void systemDiagnostics::memoryClearBut_click()
{
        memoryAnchorNumber = 0;
        ui->teMemory->clear();
}


//////////////////////////////////////////////////////
//
//     S T O P   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void systemDiagnostics::stopPingBut_click()
{
    this->pingProcess->kill();
    processingAButton = false;

}

bool systemDiagnostics::updateDB(QString key, QString value)
{
    //processingAButton = true;
    QString databaseFileName = "settingswizard/scalesettings";
    QString databaseTempFileName = "settingswizard/scalesettingstemp";
    QString databaseBackupFileName = "settingswizard/scalesettingsBACKUP";
    QString databasePath = filesystem::absoluteFilePath("settingswizard", folderType::DBs);
    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);
    QString databaseTempFileLocation = filesystem::absoluteFilePath(databaseTempFileName, folderType::DBs);
    QString databaseBackupFileLocation = filesystem::absoluteFilePath(databaseBackupFileName, folderType::DBs);
    QFile file(databaseFileLocation);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      //  processingAButton = false;

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

    return true;
}

///////////////////////////////////////////////////////////
//
//     S E T   D A T E / T I M E   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////

void systemDiagnostics::setNtpDateTimeBut_click()
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
    //session_->send(Error, ErrVersionManagerDevice, msg);
    //qApp->processEvents();
    changeTimeAfterServiceKeyRemoved();
    //ui->setDateTimeBut->show();
   // qApp->processEvents();

    //ui->hourBut->setFocus();

}

void systemDiagnostics::setDateTimeBut_click()
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


    //QString msg = "120080,3";
    //session_->send(Error, ErrVersionManagerDevice, msg);
    changeTimeAfterServiceKeyRemoved();
    //ui->setNtpBut->show();
   // qApp->processEvents();
    //ui->hourBut->setFocus();
}

void systemDiagnostics::changeTimeAfterServiceKeyRemoved()
{
    processingAButton = true;
    //   ui->setDateTimeBut->hide();
    //   ui->setNtpBut->hide();
    bool need2Reboot = false;
    //qDebug() << "trying to set time";

    if(ui->timeTabWidget->currentIndex() == 0)
    {
        QStringList args;
        args.clear();
        args.append("-t");
        args.append(ui->timeZoneCombo->itemData(ui->timeZoneCombo->currentIndex()).toString());

        python pythonProcess;
        if (pythonProcess.execCommand("setTimezone",args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                need2Reboot = true;
            }
            else
            {
                need2Reboot = false;
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
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
        options.append("-t"); // text of timezone e.g., America/Detroit
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

            }
            else
            {
                need2Reboot = false;
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
            }
        }

    }
    else if(ui->timeTabWidget->currentIndex() == 2)
    {
        python pythonProcess;
        //ui->keepAliveBut->click();
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

        options.append("-T"); // NTP text of timezone e.g., America/Detroit
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

        options.append("-T"); // NTP text of timezone e.g., America/Detroit
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
                this->ui->alertsLineEdit->setText(tr("Cannot set time."));
                ui->ntpCurrentTimeLabel->setText("timezone failed");
            }
        }

        args.clear();
        args.append("-c");
        args.append("1");
        args.append(ui->ntpServer->text());

        // ping for ntp server and after 20 seconds fail
        if (pythonProcess.execCommand("ping",args,false,false, 20 * 1000))
        {
            if (pythonProcess.getExitCode() == 0)
            {

                args.clear();
                args.append(ui->ntpServer->text());

                python NTPTimezoneProcess;

                if (NTPTimezoneProcess.execCommand("ntp",args, false, false, 10 * 1000))
                {
                    if (NTPTimezoneProcess.getExitCode() == 0)
                    {
                        QString ntpTime = NTPTimezoneProcess.readStdOut();


                        QStringList ntpTimeValues = ntpTime.split(' ');

                        if(ntpTimeValues.count() > 5)
                        {
                            QString newHour = "0";

                            // Calculate hour offset that
                            //QString ntpHourOffset = "0";
                            options.clear();
                            //if (NTPTimezoneProcess->execCommand("getTimezoneOffset",options, false, false, 10 * 1000))
                            //{
                            //    if (NTPTimezoneProcess->getExitCode() == 0)
                            //    {
                            //        ntpHourOffset = NTPTimezoneProcess->readStdOut();

                            //    }
                            //}
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

                            //QTime::restart();
                            //qApp->flush();
                            //ui->keepAliveBut->click();
                            if (NTPTimezoneProcess.execCommand("setTime",args))
                            {
                                if (NTPTimezoneProcess.getExitCode() == 0)
                                {
                                    //qApp->flush();
                                    //QTime::restart();
                                    need2Reboot = true;
                                    this->ui->timeZoneCombo->setCurrentIndex(ui->ntpTimeZoneCombo->currentIndex());
                                    this->ui->timeEdit->setTime(QTime::currentTime());
                                    this->ui->dateEdit->setSelectedDate(QDate::currentDate());
                                    ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(0)));
                                    ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(1)));
                                    ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(2)));
                                    ui->ampmBut->setText(ui->timeEdit->sectionText(ui->timeEdit->sectionAt(3)));

                                    updateDB("timezone", (ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString()));
                                    updateDB("ntpTimeZone", (ui->ntpTimeZoneCombo->itemData(ui->ntpTimeZoneCombo->currentIndex()).toString()));
                                }
                                else
                                {
                                    need2Reboot = false;
                                    this->ui->alertsLineEdit->setText(tr("Cannot set time."));
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
                        this->ui->alertsLineEdit->setText(tr("Cannot get time from NTP server."));
                        ui->ntpCurrentTimeLabel->setText("ntp failed");
                    }
                }
                else
                {
                    need2Reboot = false;
                    this->ui->alertsLineEdit->setText(tr("Cannot get time from NTP server."));
                }
            }
            else
            {
                need2Reboot = false;
                this->ui->alertsLineEdit->setText(tr("Cannot get time from NTP server."));
                ui->ntpCurrentTimeLabel->setText("ping failed");
            }
        }
        else
        {
            //Popup that the ntp server is not reachable so can't set time
            need2Reboot = false;

        }
    }
    needToReboot = need2Reboot;
    if(need2Reboot)
    {
        ui->alertsLineEdit->setText(tr("Time set."));
        processingAButton = false;
        shouldChangeTime = false;
        shouldChangeNTP = false;
        ui->setDateTimeBut->show();
        ui->setNtpBut->show();
        //this->reboot();
    }
    else
    {
        ui->alertsLineEdit->setText(tr("Time not set."));
        ui->setDateTimeBut->show();
        ui->setNtpBut->show();
        ui->USBGroupbox->hide();
        //session_->send(Error,ErrRemoveAnyModalDialogDevice,"120080,3");   // remove dialogbox
    }

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
void systemDiagnostics::fillTimeZones()
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
    this->ui->timeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
    this->ui->timeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
    this->ui->timeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/Detroit"));
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
    this->ui->ntpTimeZoneCombo->addItem("(UTC-03:00) Greenland",QString("America/Godthab"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-04:00) Atlantic Time (Canada)",QString("America/Halifax"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-05:00) Eastern Time (US & Canada)",QString("America/Detroit"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-06:00) Central Time (US & Canada)",QString("America/Chicago"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Arizona",QString("America/Phoenix"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-07:00) Mountain Time (US & Canada)",QString("America/Denver"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-08:00) Pacific Time (US & Canada)",QString("America/Los_Angeles"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-09:00) Alaska",QString("America/Anchorage"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-10:00) Hawaii",QString("Pacific/Honolulu"));
    this->ui->ntpTimeZoneCombo->addItem("(UTC-11:00) Coordinated Universal Time-11",QString("Pacific/Midway"));
    // "Habitations in this time zone use neighboring timezone based on political owner or region." http://en.wikipedia.org/wiki/UTC%E2%88%9212:00
    // so Linux does not have an entry for it, reassign to Pacific/Midway
    this->ui->ntpTimeZoneCombo->addItem("(UTC-12:00) International Date Line West",QString("Pacific/Midway"));

    // populate timezonehours array

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

void systemDiagnostics::currentChangedSlot(int index)
{
    if( index != 3)
    {
        if(mustTurnOnDialogs)
        {
            session_->send(SessionDev , SnRestartDialogs);
            mustTurnOnDialogs = false;
        }
    }
    switch (index)
    {
        case 0:
            this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
            this->ui->setBrightnessBut->setFocus();
            break;
        case 1:
            this->ui->alertsLineEdit->setText(tr("Change Sound Settings"));
            this->ui->setVolumeBut->setFocus();
            break;
        case 2:
            this->ui->alertsLineEdit->setText(tr("Change Time Settings"));
            //this->ui->setDateTimeBut->setFocus();
            break;
        case 3:
            //mustTurnOnDialogs = true;
            //session_->send(SessionDev , SnStopDialogs);
            if(!networkTabAlreadyLoaded)
            {
                loadNetworkTab();
                networkTabAlreadyLoaded = true;
            }
            this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
            //this->ui->alertsLineEdit->setText(dbClientCertUsersFilename);
            this->ui->networkSaveBut->setFocus();
            break;
        case 4:
            this->ui->alertsLineEdit->setText(tr("Change Host Settings"));
            this->ui->setHostnameBut->setFocus();
            break;
    }
}

//////////////////////////////////////////////////////
//
//       B R I G H T N E S S   S P I N B O X
//
//        V A L U E   C H A N G E D
//
//////////////////////////////////////////////////////

void systemDiagnostics::brightnessSpinBoxValueChanged(int value)
{

    this->ui->brightnessLabel->setText(QString::number(value));


    this->setBrightnessIndicators(value);
}

//////////////////////////////////////////////////////
//
//        S E T    B R I G H T N E S S    C L I C K
//
//////////////////////////////////////////////////////
void systemDiagnostics::setBrightnessBut_click()
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

void systemDiagnostics::turnOffDisplays()
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

void systemDiagnostics::volumeIndicator0_click()
{
    doPreset(0);
}

void systemDiagnostics::volumeIndicator1_click()
{
    doPreset(1);
}

void systemDiagnostics::volumeIndicator2_click()
{
    doPreset(2);
}

void systemDiagnostics::volumeIndicator3_click()
{
    doPreset(3);
}

void systemDiagnostics::volumeIndicator4_click()
{
    doPreset(4);
}

void systemDiagnostics::volumeIndicator5_click()
{
    doPreset(5);
}

void systemDiagnostics::volumeIndicator6_click()
{
    doPreset(6);
}

void systemDiagnostics::volumeIndicator7_click()
{
    doPreset(7);
}

void systemDiagnostics::volumeIndicator8_click()
{
    doPreset(8);
}

void systemDiagnostics::volumeIndicator9_click()
{
    doPreset(9);
}

void systemDiagnostics::volumeIndicator10_click()
{
    doPreset(10);
}

//////////////////////////////////////////////////////
//
//       V O L U M E    D O    P R E S E T
//
//////////////////////////////////////////////////////

void systemDiagnostics::doPreset(int value)
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

void systemDiagnostics::manualPause()
{
    state = Paused;
    QTimer::singleShot(1000, this, SLOT(setRunState()));
    while(1)
    {
        qApp->processEvents();
        if(state == Run) break;
    }
}

void systemDiagnostics::setRunState()
{
    state = Run;
}

