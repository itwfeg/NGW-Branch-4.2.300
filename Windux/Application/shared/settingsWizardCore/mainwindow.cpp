/*
PROLOGUE: QmSettings (aka Setup Wizard)
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

#include <QWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "network.h"
#include <qmessagebox.h>
#include <QSqlDatabase>
#include <qsqldatabase.h>
#include <QtSql>
#include <QCryptographicHash>

#include <QtGui>
#include <QPainter>
#include <QProcess>
#include "../platform/python.h"
#include <QDir>
#include <QFileInfo>
#include "../platform/filesystem.h"



#ifdef Q_OS_WIN32
#include <windows.h>
#endif

const QString MainWindow::ServicePassword = "hobart45374";
const QString MainWindow::PASSPHRASE = "passphrase";
//extern bool isDemoMode;


//////////////////////////////////////////////////////
//
//   true  = Service tech has a backdoor in.
//   false = Service tech has no backdoor in.
//
const bool MainWindow::USE_BACKDOOR = true;
//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//
//   Remove for production
//
const QString MainWindow::USERNAME = "service";
const QString MainWindow::PASSWORD = "hobart45374";
//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//
//        C O N S T R U C T O R
//
//////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :

    QWidget(parent),
    ui(new Ui::MainWindow)
{
    // make sure our images are ready
    Q_INIT_RESOURCE( settingsWizardCore );

    QStyle *demoStyle = new QCleanlooksStyle();
    visDemoMode = true;
    QString databaseFileName = "settingswizard/scalesettings.sql";
    databaseFileLocation = filesystem::absoluteFilePath(databaseFileName, folderType::DBs);

    qDebug() << " db location is:" << databaseFileLocation;
    //QPixmap pixmap(fullFilename);

    connectDB();
    ui->setupUi(this);
    firstTimeCustomerBrightnessDisplay = true;
    this->loadDisplayBrightnessValues("o");

    firstTimeCustomerVolume = true;
    this->loadSoundVolumeValues("o");

    pythonProcess = new python();
  //  demoStyle = n
    this->setStyle(demoStyle);
   // this->ui->muteBut->hide();


    if(!visDemoMode)
    {
        this->ui->quitBut->hide();
    }
    this->changedAstronomical = false;
    this->changedTimezone = false;
    this->changedHostname = false;
    this->networkCount = 2;
    this->machineID = "34A5DE30455";

    cancelPing = true;
    this->ui->alertsLineEdit->setAlignment(Qt::AlignCenter);
  //  this->ui->alertsLineEdit->setText(tr("Login to Configure System Settings"));


    /////////
    translator1 = new QTranslator(this);
    translator2 = new QTranslator(this);

   // this->setMask((new QPixmap("HobartLogo2Operator.png"))->mask());
   // QPalette* palette = new QPalette();
   // palette->setBrush(QPalette::Background,*(new QBrush(*(new QPixmap("HobartLogo2Operator.png")))));
   // setPalette(*palette);
   // setWindowFlags(Qt::FramelessWindowHint);

    // variable below is used for setting the mode of the attempted user
    // either set to 'guest' which is effectively logging them out,
    // or set it to 'user' which logs them in. This variable
    // is used with the changeMode() function to toggle
    // authentication accordingly.

    mode = "user";
  //  this->changeMode();

    //this->ui->usernameBox->setText(USERNAME);
   // this->ui->passwordBox->setText(PASSWORD);

    // Set Time
    this->ui->timeEdit->setTime(QTime::currentTime());

    // Set Volume

    loadSoundVolumeValues("o");
    volume = 50;

    this->ui->volumeSpinBox->setValue(volume);

    this->ui->volumeLabel->setText(QString::number(volume));

    if(volume > 0)
    {
        this->ui->muteBut->setChecked(true);
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px;border: 1px solid gray;background:url(:/images/volumeOnSmall.png);");
    }
    else
    {
        this->ui->muteBut->setChecked(false);
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px;border: 1px solid gray;background:url(:/images/volumeOffSmall.png);");
    }

    this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    // Set Brightness
    // int brightnessValue = 50;//getDBBrightness();

    //this->ui->brightnessSpinBox->setValue(brightnessValue);
    //this->ui->brightnessLabel->setText(QString::number(brightnessValue));

    //this->setBrightnessIndicators(brightnessValue);
    QString Octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    this->ui->ipAddressLineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));
    this->ui->subnetMaskLineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));
    this->ui->gatewayLineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));
    this->ui->DNS1LineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));
    this->ui->DNS2LineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));
    this->ui->DNS3LineEdit->setValidator(new QRegExpValidator(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));

    // Load login language combobox
    //this->ui->loginLanguageCombo->addItem("English");
   // this->ui->loginLanguageCombo->addItem("Espanol");
   // this->ui->loginLanguageCombo->addItem("Francais");

    QStandardItemModel *userModel = new QStandardItemModel(2,3,this); //2 Rows and 3 Columns
    userModel->setHorizontalHeaderItem(0, new QStandardItem(QString(tr("Username"))));
    userModel->setHorizontalHeaderItem(1, new QStandardItem(QString(tr("Password"))));
    userModel->setHorizontalHeaderItem(2, new QStandardItem(QString(tr("Admin"))));

    this->ui->operatorScreenBut->setIcon(QIcon(":/images/operatorDisplay.png"));
    this->ui->operatorScreenBut->setIconSize(QSize(91,54));

    this->ui->customerScreenBut->setIcon(QIcon(":/images/customer.png"));
    this->ui->customerScreenBut->setIconSize(QSize(86,54));
                                                                                                              //background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(170, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));
    this->ui->operatorScreenBut->setStyleSheet("QPushButton#operatorScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(170, 255, 0, 255), stop:0.28436 rgba(170, 255, 0, 255), stop:0.50237 rgba(170, 255, 0, 255), stop:0.772512 rgba(170, 255, 0, 255)); border: 2px solid rgba(170, 255, 0, 255);}");
    this->ui->customerScreenBut->setStyleSheet("QPushButton#customerScreenBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(0, 85, 127, 69), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(0, 85, 127, 208), stop:0.772512 rgba(0, 85, 127, 145)); border: 2px solid rgba(100, 255, 255, 255);}");
    this->ui->operatorSoundBut->setStyleSheet("QPushButton#operatorSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(0, 85, 127, 69), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(0, 85, 127, 208), stop:0.772512 rgba(0, 85, 127, 145)); border: 2px solid rgba(100, 255, 255, 255);}");
    this->ui->customerSoundBut->setStyleSheet("QPushButton#customerSoundBut:checked {color: rgb(139, 209, 0); background-color: qlineargradient(spread:repeat, cx:0.75, cy:0.5,  fx:0.75, fy:0.75, stop:0 rgba(0, 85, 127, 69), stop:0.28436 rgba(0, 85, 127, 69), stop:0.50237 rgba(0, 85, 127, 208), stop:0.772512 rgba(0, 85, 127, 145)); border: 2px solid rgba(100, 255, 255, 255);}");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#000000;background:url(:/images/brightnessLEDoff.png);");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLEDoff.png);");

    this->ui->brightnessSpinBox->setStyleSheet("QSpinBox#brightnessSpinBox{border-top-right-radius: 10px;border-bottom-right-radius: 10px;border: 2px solid black;height:252px;width: 98px;  background:black; margin: 0 0 0 0; } QSpinBox#brightnessSpinBox::up-button { width: 98px; height:126px; background:black;background: url(:/images/SpinBoxButtonsOffTop.png); } QSpinBox#brightnessSpinBox::down-button {width: 98px; height:126px; background:url(:/images/SpinBoxButtonsOffBottom.png); }");

    this->ui->volumeSpinBox->setFocus();

    //this->ui->adminTableView->setModel(userModel);

    //this->ui->setBrightnessBut->setEnabled(false);

    //this->ui->daylightSavingsLabel->setText("");

    this->ui->lastSettingLabel->setText("");

    //this->ui->logoutBut->hide();

    pickedDate = new QDate();

    this->fillTimeZones();

    this->setTimeTab();

    //this->getWirelessAPs();

    //this->ui->VPNUserLineEdit->setEnabled(false);

    this->adapterAlreadyConnected = false;

    this->loadNetworkConnections();

    //this->revealNetworkGroupBox(1, -1); // show adapter "Wired or Wireless" groupbox
    if(visDemoMode)
    {
        this->ui->availableConnectionsTableView->selectRow(1);
    }
    else
    {
        this->ui->availableConnectionsTableView->selectRow(1);
    }
    //this->

    // connections for login/out buttons
    commonNetwork = new Network();

    //this->loadSavedNetwork(commonNetwork);

    this->ui->securityCombo->addItem("WPA2-Enterprise (LEAP)",QString("WPA2-Enterprise"));

    this->loadWirelessSecurityMethodsCombo();

    // TBD SET MACHINE ID;

    QPixmap pixmap(":/images/refresh.png");
    QIcon refreshButtonIcon(pixmap);
    this->ui->refreshNetworksBut->setIcon(refreshButtonIcon);
//    this->ui->refreshNetworkBut->setIconSize(pixmap.rect().size());

    // get ModelName from API to SPI memory when implemented
    //this->ui->modelNameLabel->setText(this->modelName());

    this->getUIMACAddress();
    this->getUIHostname();

    this->ui->operatorScreenBut->setChecked(false);
    this->ui->customerScreenBut->setChecked(false);
    this->ui->autoDNSRadio->setChecked(true);
    this->ui->dhcpRadio->setChecked(true);

    this->ui->availableConnectionsTableView->selectRow(0);
    this->revealNetworkGroupBox(1,0);
    //this->connect(this->ui->loginBut, SIGNAL(clicked()), this, SLOT(loginBut_click()));
    //this->connect(this->ui->logoutBut, SIGNAL(clicked()), this, SLOT(logoutBut_click()));
    this->connect(this->ui->refreshNetworksBut, SIGNAL(clicked()), this, SLOT(refreshNetworksButton_click()));
    // connection for addUser button
    //this->connect(this->ui->addUserBut, SIGNAL(clicked()), this, SLOT(addUserBut_click()));
    // connection for deleteUser button
    //this->connect(this->ui->deleteUserBut, SIGNAL(clicked()), this, SLOT(deleteUserBut_click()));
    // connection for quit application button
    this->connect(this->ui->quitBut, SIGNAL(clicked()), qApp, SLOT(quit()));
    // connection for Set Volume button
    this->connect(this->ui->setVolumeBut, SIGNAL(clicked()), this, SLOT(setVolumeBut_click()));

   // this->connect(this->ui->networkImportBut, SIGNAL(clicked()), this, SLOT(importNetworkBut_click())); //Alex's Code
   // this->connect(this->ui->networkExportBut, SIGNAL(clicked()), this, SLOT(exportNetworkBut_click())); //Alex's Code
    this->connect(this->ui->exportBut, SIGNAL(clicked()), this, SLOT(exportNetworkBut_click()));
    this->connect(this->ui->importBut, SIGNAL(clicked()), this, SLOT(importNetworkBut_click()));
    this->connect(this->ui->testVolumeBut, SIGNAL(clicked()), this, SLOT(testVolumeBut_click()));

    this->connect(this->ui->setHostnameBut, SIGNAL(clicked()), this, SLOT(setHostnameBut_click()));

    // connection for Set Date/Time button
    this->connect(this->ui->setDateTimeBut, SIGNAL(clicked()), this, SLOT(setDateTimeBut_click()));
    // connection for Calibrate Screen button
    this->connect(this->ui->calibrateScreenBut, SIGNAL(clicked()), this, SLOT(calibrateScreenBut_click()));
    // connection for Network-Apply button
    this->connect(this->ui->addNetworkBut, SIGNAL(clicked()), this, SLOT(addNetworkBut_click()));
    this->connect(this->ui->networkSaveBut, SIGNAL(clicked()), this, SLOT(saveNetworkBut_click()));
    //this->connect(this->ui->networkCancelBut, SIGNAL(clicked()), this, SLOT(cancelNetworkBut_click()));
    // connection for Calibrate Screen button
    this->connect(this->ui->setBrightnessBut, SIGNAL(clicked()), this, SLOT(setBrightnessBut_click()));
    // connection for Start Ping button
    this->connect(this->ui->startPingBut, SIGNAL(clicked()), this, SLOT(startPingBut_click()));
    // connection for Stop Ping button
    this->connect(this->ui->stopPingBut, SIGNAL(clicked()), this, SLOT(stopPingBut_click()));
    // connection for volumeIndicator0
    this->connect(this->ui->volumeIndicator0, SIGNAL(clicked()), this, SLOT(volumeIndicator0_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator1, SIGNAL(clicked()), this, SLOT(volumeIndicator1_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator2, SIGNAL(clicked()), this, SLOT(volumeIndicator2_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator3, SIGNAL(clicked()), this, SLOT(volumeIndicator3_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator4, SIGNAL(clicked()), this, SLOT(volumeIndicator4_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator5, SIGNAL(clicked()), this, SLOT(volumeIndicator5_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator6, SIGNAL(clicked()), this, SLOT(volumeIndicator6_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator7, SIGNAL(clicked()), this, SLOT(volumeIndicator7_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator8, SIGNAL(clicked()), this, SLOT(volumeIndicator8_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator9, SIGNAL(clicked()), this, SLOT(volumeIndicator9_click()));
    // connection for volumeIndicator3
    this->connect(this->ui->volumeIndicator10, SIGNAL(clicked()), this, SLOT(volumeIndicator10_click()));
    // connection for dynamic language selection at runtime
    //this->connect(this->ui->loginLanguageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loginLanguageComboValueChange(int)));

    this->connect(this->ui->securityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setWirelessNetworkEncryptionCombo(int)));
    // connection for applying network configuration settings
    // this->connect(this->ui->configureNetworkBut, SIGNAL(clicked()),this, SLOT(configureNetworkBut_click()));
    // connection for launching the Screen Calibration Tool
    this->connect(this->ui->volumeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(volumeSpinBoxValueChanged(int)));
    this->connect(this->ui->brightnessSpinBox,SIGNAL(valueChanged(int)), this, SLOT(brightnessSpinBoxValueChanged(int)));
    // connection for Set Alert if tab selection changes
    this->connect(this->ui->mainTab,SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));
    //  connection for displaying ping output from Python script

    // Start Ping process and pipe output to textbox
    //this->connect(this->pingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printPingOutput()));
    this->connect(this->ui->operatorSoundBut, SIGNAL(clicked()),this, SLOT(changedOperatorSoundButtons()));
    this->connect(this->ui->customerSoundBut, SIGNAL(clicked()),this, SLOT(changedCustomerSoundButtons()));

    this->connect(this->ui->operatorScreenBut, SIGNAL(clicked()),this, SLOT(changedOperatorDisplayBrightnessButtons()));
    this->connect(this->ui->customerScreenBut, SIGNAL(clicked()),this, SLOT(changedCustomerDisplayBrightnessButtons()));
    this->connect(this->ui->muteBut, SIGNAL(toggled(bool)),this, SLOT(muteButChanged(bool)));
    this->connect(this->ui->dateEdit, SIGNAL(clicked(QDate)), this, SLOT(datePicked(QDate)));
    this->connect(this->ui->astronomicalCheckBox, SIGNAL(stateChanged(int)), this, SLOT(astronomicalCheckboxValueChanged(int)));



    this->connect(this->ui->dhcpRadio, SIGNAL(clicked()), this, SLOT(dhcpRadio_disableIPs()));
    this->connect(this->ui->dhcpRadio, SIGNAL(toggled(bool)), this, SLOT(dhcpRadio_disableIPs()));
    this->connect(this->ui->staticIPRadio, SIGNAL(clicked()), this, SLOT(staticIPRadio_enableIPs()));
    this->connect(this->ui->autoDNSRadio, SIGNAL(clicked()), this, SLOT(autoDNSRadio_click()));
    this->connect(this->ui->autoDNSRadio, SIGNAL(toggled(bool)), this, SLOT(autoDNSRadio_click()));
    this->connect(this->ui->manualDNSRadio, SIGNAL(clicked()), this, SLOT(manualDNSRadio_click()));

    this->connect(this->ui->availableConnectionsTableView, SIGNAL(cellPressed(int,int)), this, SLOT(networkTableWidgetSelectionChanged(int)));
    if(visDemoMode)
    {
        this->ui->availableConnectionsTableView->selectRow(0);
        this->revealNetworkGroupBox(3,0);
    }
}

//////////////////////////////////////////////////////
//
//        D E S T R U C T O R
//
//////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    bool doMSGBox = false;

    QString message = "";
    if(changedTimezone)
    {
        message = "TimeZone\n";
        doMSGBox = true;
    }
    if(changedHostname)
    {
        message += "Hostname\n";
        doMSGBox = true;
    }
    if(changedAstronomical)
    {
        message += "12/24 hour clock";
        doMSGBox = true;
    }

    if(doMSGBox)
    {
        QMessageBox msgBox;
        msgBox.setText("The following changes require a reboot:");
                //Set predefined icon, icon is show on left side of text.
            msgBox.setIcon(QMessageBox::Information);
                //set informative text
            msgBox.setInformativeText(message);
                //Add ok and cancel button.
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                //Set focus of ok button
            msgBox.setDefaultButton(QMessageBox::Ok);

                //execute message box. method exec() return the button value of cliecke button
            int ret = msgBox.exec();

                //User get input from returned value (ret). you can handle it here.
             switch (ret) {
               case QMessageBox::Cancel:
                   // Cancel was clicked do nothing
                   break;
               case QMessageBox::Ok:
                   // Ok was clicked
                   delete ui;
                   // do reboot
                   QProcess rebootProcess;

                   QStringList options;
                   options.append("python/qmSettings/system/qmSettingsReboot.py");
                   rebootProcess.start("python", options);
                   if(!rebootProcess.waitForStarted())
                   {
                       this->ui->alertsLineEdit->setText(tr("Python did not start"));
                       // python did not start
                       //return;
                   }

                   if(!rebootProcess.waitForFinished())
                   {
                       // do progressBar here
                       this->ui->alertsLineEdit->setText(tr("Python did not finish"));
                       //return "";
                       // python did not finish
                   }

                  break;
             }
    }
    else delete ui;
    pythonProcess->cleanup();
    disconnectDB();

}

//////////////////////////////////////////////////////
//
//       G E T   t h e   O S
//
//////////////////////////////////////////////////////

bool MainWindow::isLinux()
{
    bool isLinux = true;
    #ifdef Q_OS_WIN32
    isLinux = false;
    #endif
    return isLinux;
}

//////////////////////////////////////////////////////
//
//     C H A N G E   U S E R   M O D E
//
//////////////////////////////////////////////////////

/*void MainWindow::changeMode()
{
    if(mode == "user") // user = User is logged in (failsafe check)
    {
        this->ui->loginGroupBox->setEnabled(false);
        this->ui->loginGroupBox->lower();
        this->ui->loginGroupBox->hide();
        this->ui->mainTab->setEnabled(true);
        this->ui->mainTab->raise();
        this->ui->mainTab->show();
        this->ui->logoutBut->setEnabled(true);
        this->ui->loginBut->setEnabled(false);
        this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
        this->ui->mainTab->setCurrentIndex(0);
    }
    else               // guest = User is not allowed in, so force login
    {
        this->ui->mainTab->setEnabled(false);
        this->ui->mainTab->lower();
        this->ui->mainTab->hide();
        this->ui->loginGroupBox->setEnabled(true);
        this->ui->loginGroupBox->setHidden(false);
        this->ui->loginGroupBox->setVisible(true);
        this->ui->loginGroupBox->raise();
        this->ui->loginGroupBox->show();
        this->ui->usernameBox->setFocus();
        this->ui->usernameBox->setText("");
        this->ui->passwordBox->setText("");
        this->ui->daylightSavingsLabel->setText("");
        this->ui->timezoneLabel->setText("");
        this->ui->operatorIDLabel->setText("");
        this->ui->lastSettingLabel->setText("");
        this->ui->loginBut->setDefault(true);
        this->ui->logoutBut->setEnabled(false);
        this->ui->loginBut->setEnabled(true);
        this->ui->alertsLineEdit->setText(tr("Login to Configure System Settings"));
    }
}
*/
//////////////////////////////////////////////////////
//
//       L O G I N    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////
/*
void MainWindow::loginBut_click()
{
    if(userPassAreCorrect(this->ui->usernameBox->text(), this->ui->passwordBox->text()))
    {
        allowUserAccess(); // determine User entry
        this->ui->logoutBut->show();
    }
    else this->ui->alertsLineEdit->setText(tr("No Access. Try Again."));  //
}

//////////////////////////////////////////////////////
//
//     L O G O U T   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::logoutBut_click()
{
    mode = "guest";
    changeMode();
    this->ui->logoutBut->hide();
}
*/
//////////////////////////////////////////////////////
//
//     D H C P   R A D I O   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::dhcpRadio_disableIPs()
{

    if(this->ui->dhcpRadio->isChecked())
    {
        this->ui->subnetMaskLineEdit->setEnabled(false);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->ipAddressLineEdit->setEnabled(false);
        this->ui->ipAddressLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->gatewayLineEdit->setEnabled(false);
        this->ui->gatewayLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->autoDNSRadio->setEnabled(true);
    }
    else // brute force enable to be sure
    {
        this->ui->subnetMaskLineEdit->setEnabled(true);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:black;");
        this->ui->ipAddressLineEdit->setEnabled(true);
        this->ui->ipAddressLineEdit->setStyleSheet("color:black;");
        this->ui->gatewayLineEdit->setEnabled(true);
        this->ui->gatewayLineEdit->setStyleSheet("color:black;");
    }
}

//////////////////////////////////////////////////////
//
//     S T A T I C   I P   R A D I O   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::staticIPRadio_enableIPs()
{
    if(this->ui->staticIPRadio->isChecked())
    {
        this->ui->subnetMaskLineEdit->setEnabled(true);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:black;");
        this->ui->ipAddressLineEdit->setEnabled(true);
        this->ui->ipAddressLineEdit->setStyleSheet("color:black;");
        this->ui->gatewayLineEdit->setEnabled(true);
        this->ui->gatewayLineEdit->setStyleSheet("color:black;");

        this->ui->manualDNSRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(false);
        this->ui->autoDNSRadio->setEnabled(false);
        this->ui->autoDNSRadio->isHidden();
    }
    else // brute force disable to be sure
    {
        this->ui->subnetMaskLineEdit->setEnabled(false);
        this->ui->subnetMaskLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->ipAddressLineEdit->setEnabled(false);
        this->ui->ipAddressLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->gatewayLineEdit->setEnabled(false);
        this->ui->gatewayLineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->autoDNSRadio->isVisible();
    }

}

//////////////////////////////////////////////////////
//
//      N E T W O R K    T A B L E W I D G E T    S E L E C T I O N    C H A N G E D   S L O T
//
//////////////////////////////////////////////////////

void MainWindow::networkTableWidgetSelectionChanged(int row)
{
    if(row == 0) // WIRED - Has to be wired wired is always provided and at row 0
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

    // Trial to set availableConnectionsTableView->
    // this->ui->availableConnectionsTableView->itemAt(row, 0)->setFocus();
    this->ui->availableConnectionsTableView->selectRow(row);
    // TBD remove dotted line around selection of table cell.
    //  this->ui->availableConnectionsTableView->
}


//////////////////////////////////////////////////////
//
//      S E T   C O M M O N    N E T W O R K
//
//////////////////////////////////////////////////////
void MainWindow::setCommonNetwork(Network *locNetwork)
{
    // Set global network settings.

    locNetwork->setOperatorID(0);

    locNetwork->setMachineID(0);

    locNetwork->setIPAddress(this->ui->ipAddressLineEdit->text());

    locNetwork->setIsStatic(this->ui->staticIPRadio->isChecked());

    locNetwork->setIsAutoDNS(this->ui->autoDNSRadio->isChecked());

    locNetwork->setSubnetMask(this->ui->subnetMaskLineEdit->text());

    locNetwork->setGateway(this->ui->gatewayLineEdit->text());

    locNetwork->setPrimaryDNS(this->ui->DNS1LineEdit->text());

    locNetwork->setSecondaryDNS(this->ui->DNS2LineEdit->text());

    locNetwork->setTertiaryDNS(this->ui->DNS3LineEdit->text());

    locNetwork->setIsWireless(this->ui->wirelessAdapterGroupBox->isVisible());

    locNetwork->setSSID(this->ui->SSIDLineEdit->text());

    locNetwork->setSecurityMethod(this->ui->securityCombo->currentText());

    locNetwork->setEncryption(this->ui->encryptionCombo->currentText());

    locNetwork->setVPNUsername("");

    locNetwork->setVPNUsername("");

    locNetwork->setIsVPN(false /*this->ui->vpnGroupBox->isVisible()*/);

    locNetwork->setWEPKey1(this->ui->wepKey1LineEdit->text());

    locNetwork->setWEPKey2(this->ui->wepKey2LineEdit->text());

    locNetwork->setWEPKey3(this->ui->wepKey3LineEdit->text());

    locNetwork->setWEPKey4(this->ui->wepKey4LineEdit->text());

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

    locNetwork->setWPAPassPhrase(this->ui->wpaPassphraseLineEdit->text());

    locNetwork->setEAPUsername(this->ui->eapUsernameLineEdit->text());

    locNetwork->setEAPPassword(this->ui->eapPasswordLineEdit->text());
}


void MainWindow::addNetworkBut_click()
{
    this->ui->SSIDLineEdit->setText("");
    this->ui->securityCombo->setCurrentIndex(0);
    this->ui->encryptionCombo->setCurrentIndex(0);
    this->ui->wepKey1LineEdit->setText("");
    this->ui->wepKey2LineEdit->setText("");
    this->ui->wepKey3LineEdit->setText("");
    this->ui->wepKey4LineEdit->setText("");
    this->ui->DNS1LineEdit->setText("");
    this->ui->DNS2LineEdit->setText("");
    this->ui->DNS3LineEdit->setText("");
    this->ui->autoDNSRadio->setChecked(true);
    this->ui->dhcpRadio->setChecked(true);
    this->ui->eapPasswordLineEdit->setText("");
    this->ui->eapUsernameLineEdit->setText("");
    this->ui->gatewayLineEdit->setText("");
    this->ui->ipAddressLineEdit->setText("");
    this->ui->subnetMaskLineEdit->setText("");
    this->ui->wpaPassphraseLineEdit->setText("");
    this->ui->wirelessAuthenticationDefaultKey1Radio->setChecked(true);
    this->ui->availableConnectionsTableView->clearSelection();
}

//////////////////////////////////////////////////////
//
//      R E V E A L   N E T W O R K   G R O U P B O X
//
//////////////////////////////////////////////////////

void MainWindow::revealNetworkGroupBox(int box, int row)
{
    switch (box)
    {
    case 1: //WIRED
        //this->ui->dnsGroupBox->hide();
        //this->ui->vpnGroupBox->hide();
//        this->ui->wiredAdapterGroupBox->setGeometry(320,7,331,161);
        this->ui->dnsGroupBox->setGeometry(486,158,256,161);
        this->ui->wiredAdapterGroupBox->setGeometry(486,4,256,150);


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
        this->ui->wiredAdapterGroupBox->setGeometry(312,4,256,150);
        this->ui->dnsGroupBox->setGeometry(312,158,256,161);

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
        this->ui->dhcpRadio->setChecked(true);
        this->ui->autoDNSRadio->setChecked(true);
        break;
    }
}

//////////////////////////////////////////////////////
//
//      L O A D   W I R E L E S S   S E C U R I T Y   M E T H O D S   C O M B O
//
//////////////////////////////////////////////////////

void MainWindow::loadWirelessSecurityMethodsCombo()
{
    this->ui->securityCombo->clear();
    this->ui->securityCombo->addItem("OPEN", QString("Open"));
    this->ui->securityCombo->addItem("WPA-Personal", QString("WPA-Pesonal"));
    this->ui->securityCombo->addItem("WPA-Enterprise (PEAP)", QString("WPA-Enterprise (PEAP)"));
    this->ui->securityCombo->addItem("WPA-Enterprise (LEAP)", QString("WPA-Enterprise (LEAP)"));
    this->ui->securityCombo->addItem("WPA2-Personal", QString("WPA2-Personal"));
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

void MainWindow::datePicked(QDate selectionDate)
{
    pickedDate->setDate(selectionDate.year(),selectionDate.month(),selectionDate.day());  // ->setDate(selectionDate.currentDate()); //->setDate(selectionDate);
}

//////////////////////////////////////////////////////
//
//      S E T   W I R E L E S S   N E T W O R K   E N C R Y P T I O N   C O M B O
//
//////////////////////////////////////////////////////

void MainWindow::setWirelessNetworkEncryptionCombo(int securityMethodItemNum)
{
    switch (securityMethodItemNum)
    {
    case 0:
    {
        // Open
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->hide();
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
        this->ui->wpaPassPhraseFrame->show();
    }
        break;
    case 2:
    {
        // WPA-Enterprise (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;
    case 3:
    {
        // WPA-Enterprise (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->show();
    }
        break;
    case 4:
    {
        // WPA2-Personal
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->eapUserPassFrame->hide();
        this->ui->wpaPassPhraseFrame->show();
    }
        break;
    case 5:
    {
        // WPA2-Enterprise (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->show();

    }
        break;
    case 6:
    {
        // WPA2-Enterprise (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("AES", QString("AES"));
        this->ui->encryptionCombo->addItem("TKIP", QString("TKIP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->show();

    }
        break;
    case 7:
    {
        // 802.1X (PEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
        this->ui->eapUserPassFrame->show();

    }
        break;
    case 8:
    {
        // 802.1X (LEAP)
        this->ui->encryptionCombo->clear();
        this->ui->encryptionCombo->addItem("WEP", QString("WEP"));
        this->ui->wepKeyFrame->hide();
        this->ui->wpaPassPhraseFrame->hide();
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

void MainWindow::astronomicalCheckboxValueChanged(int value)
{
    if(value == 0) // Unchecked
    {
        this->ui->timeEdit->setDisplayFormat("hh:mm:ss AP");
    }
    else // checked
    {
        this->ui->timeEdit->setDisplayFormat("HH:mm:ss");
    }
   // this->ui->dateEdit->setFocus();
}

//////////////////////////////////////////////////////
//
//       C A L I B R A T E   S C R E E N    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::calibrateScreenBut_click()
{
   //this->ui->brightnessSpinBox->setFocus();

    QProcess calibrateProcess;
    QStringList options;
    options.append("python/qmSettings/display/qmSettingsCalibrateTouchscreen.py");
    calibrateProcess.start("python", options);
    if(!calibrateProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!calibrateProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

}

//////////////////////////////////////////////////////
//
//      T E S T    V O L U M E    B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::testVolumeBut_click()
{
  // this->ui->brightnessSpinBox->setFocus();

/*  Use this block when operator sound device is accessible
  QProcess testVolumeProcess;
    QStringList options;
    options.append("python/qmSettings/sound/qmSettingsPlaySound.py");
    testVolumeProcess.start("python", options);

    if(!testVolumeProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!testVolumeProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

  */
    this->ui->alertsLineEdit->setText(tr("Testing sound volume."));
    QStringList options;
    options.append("python/qmSettings/sound/qmSettingsSetVolume.py");
    options.append("--level");
    options.append(this->ui->volumeLabel->text());
    options.append("-D");

    if(this->ui->operatorSoundBut->isChecked())
    {
        options.append("1");
    }
    else if (this->ui->customerSoundBut->isChecked())
    {
        options.append("2");
    }

    QProcess applySoundVolumeConfiguration;
//    applySoundVolumeConfiguration.waitForFinished();

    applySoundVolumeConfiguration.start("python", options);
    if(!applySoundVolumeConfiguration.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!applySoundVolumeConfiguration.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

    QProcess playSoundProcess;

    QStringList optionsplay;
    optionsplay.append("python/qmSettings/sound/qmSettingsPlaySound.py");

   // playSoundProcess.waitForFinished();

    playSoundProcess.start("python", optionsplay);
    if(!playSoundProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!playSoundProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }


    // NOTE: Add configuration for GCD customer speakers as well.
    // REQ was added 1/2/2013 by Debra Dyer
    // There is no operator hardware sound device as of 01/11/2013

    /*if(updateVolume(this->ui->volumeLabel->text().toInt(), this->ui->operatorIDLabel->text().toInt()))

    {
        this->ui->volumeResultsLabel->setText(tr("Volume successfully set."));
        this->ui->alertsLineEdit->setText(tr("Volume successfully set."));
    }
    else
    {
        this->ui->volumeResultsLabel->setText(tr("Volume not set. Try again."));
        this->ui->alertsLineEdit->setText(tr("Volume successfully set."));
    }
*/
    //set volume on the fly
    //this->ui->volumeSpinBox->setValue(this->ui->volumeLabel->text().toInt());
//    this->ui->alertsLineEdit->setText("Sound volume set.");


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
    // this is for customer speakers - no hardware 01/11/2013
    // TBD


}

//////////////////////////////////////////////////////
//
//       S E T   B R I G H T N E S S   I N D I C A T O R S
//
//////////////////////////////////////////////////////

void MainWindow::setBrightnessIndicators(int value)
{
    QString objectName;
    for(int i=0; i <=10; i++)
    {
        objectName = "brightnessIndicator" + QString::number(i);
        if(i <=value/10 && value != 0)
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(images/brightnessLED.png);");
        }
        else
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(images/brightnessLEDoff.png);");
        }
    }
}

//////////////////////////////////////////////////////
//
//       S E T   V O L U M E   I N D I C A T O R S
//
//////////////////////////////////////////////////////

void MainWindow::setVolumeIndicators(int value)
{
    QString objectName;
    for(int i = 0; i <= 10; i++)
    {
        objectName = "volumeIndicator" + QString::number(i);

        if (i <= value/10 && value != 0) //turn on
        {
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(140, 140, 140);background:url(images/brightnessLED.png);");
        //    this->ui->volumeResultsLabel->setText(tr("Volume is on"));

        }
        else // turn off
        {
            if(value == 0)
            {
               this->ui->volumeResultsLabel->setText(tr("Muted"));
            }
            this->findChild<QPushButton*>(objectName)->setStyleSheet("margin:16 16 16 16; color: white;border-style: outset;border-radius: 5px; border: 1px solid rgb(130, 130, 130);background:url(images/brightnessLEDoff.png);");

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

void MainWindow::muteButChanged(bool checked)
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    volume = this->ui->volumeSpinBox->value();
    if(checked)
    {
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:20px;border: 1px solid rgb(0,120,120);background:url(images/volumeOnSmall.png);");
        this->ui->mutedResultsLabel->setText("");

        if(volume == 0)
        {
            this->ui->volumeSpinBox->setValue(volume+1);
            this->ui->volumeLabel->setText(QString::number(volume+1));
        }
        this->ui->volumeLabel->setText(QString::number(this->ui->volumeSpinBox->value()));
        this->setVolumeIndicators(this->ui->volumeSpinBox->value());
    }
    else
    {
        this->ui->muteBut->setStyleSheet("border-style:outset;border-radius:19px; border: 1px solid gray;background:url(images/volumeOffSmall.png);");
        this->ui->mutedResultsLabel->setText(tr("Muted"));
     //   this->ui->volumeSpinBox->setValue(0);
        this->ui->volumeLabel->setText("0");
        this->setVolumeIndicators(0);

    }
    this->ui->volumeSpinBox->setFocus();


}

//////////////////////////////////////////////////////
//
//       C H A N G E   O P E R A T O R   D I S P L A Y
//
//         B R I G H T N E S S   B U T T O N S
//
//////////////////////////////////////////////////////

void MainWindow::changedOperatorSoundButtons()
{
    this->ui->operatorSoundBut->setChecked(true);
    this->ui->operatorSoundButText->setStyleSheet("color: white;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLED.png);");
    this->ui->customerSoundButText->setStyleSheet("color: black;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLEDoff.png);");
    this->ui->customerSoundBut->setChecked(false);
}

void MainWindow::changedCustomerSoundButtons()
{
    this->ui->customerSoundBut->setChecked(true);
    this->ui->customerSoundButText->setStyleSheet("color: white;");
    this->ui->customerSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLED.png);");
    this->ui->operatorSoundButText->setStyleSheet("color: black;");
    this->ui->operatorSoundIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLEDoff.png);");
    this->ui->operatorSoundBut->setChecked(false);
}

void MainWindow::changedOperatorDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->operatorScreenBut->setChecked(true);
    this->ui->operatorDisplayButText->setStyleSheet("color: white;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLED.png);");
    this->ui->customerDisplayButText->setStyleSheet("color: black;");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(:/images/brightnessLEDoff.png);");
    this->ui->customerScreenBut->setChecked(false);
    //loadDisplayBrightnessValues("o");

}

//////////////////////////////////////////////////////
//
//       C H A N G E   C U S T O M E R   D I S P L A Y
//
//         B R I G H T N E S S   B U T T O N S
//
//////////////////////////////////////////////////////

void MainWindow::changedCustomerDisplayBrightnessButtons()
{
// do your checks here
// and if necessary
// emit (stateChanged(checked));
    this->ui->customerScreenBut->setChecked(true);
    this->ui->customerDisplayButText->setStyleSheet("color: white;");
    this->ui->customerDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(images/brightnessLED.png);");
    this->ui->operatorDisplayButText->setStyleSheet("color: black;");
    this->ui->operatorDisplayIndicator->setStyleSheet("margin:0 0 0 0;color:white;border-style:inset;border-radius:5px;border-width:1px;border-color:#0c457e;background:url(images/brightnessLEDoff.png);");
    this->ui->operatorScreenBut->setChecked(false);
    if(firstTimeCustomerBrightnessDisplay)
    {
        loadDisplayBrightnessValues("c");
        firstTimeCustomerBrightnessDisplay = false;
    }
}

//////////////////////////////////////////////////////
//
//       V O L U M E   S P I N B O X    V A L U E
//
//                 C H A N G E D
//
//////////////////////////////////////////////////////

void MainWindow::volumeSpinBoxValueChanged(int percentage)
{
  //  this->ui->setVolumeBut->setText(tr("Set volume to ") + QString::number(percentage) + tr("%"));
    // It's always a number so does not need translated
    this->ui->volumeLabel->setText(QString::number(percentage));
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

void MainWindow::retranslateUI()
{
    // Login
    //this->ui->label_2->setText(tr("Username:"));
   // this->ui->label_3->setText(tr("Password:"));
   // this->ui->label_13->setText(tr("Language:"));
   // this->ui->loginBut->setText(tr("Login"));
   // this->ui->loginGroupBox->setTitle(tr("L O G I N"));


    // Sound Volume
    this->ui->setVolumeBut->setText(tr("Set Volume"));

    // logout
  //  this->ui->logoutBut->setText(tr("Logout"));
    this->ui->quitBut->setText(tr("X"));

    // Date/Time
    this->ui->setDateTimeBut->setText(tr("Set Date/Time"));

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

void MainWindow::readStdOutput()
{
    //   QString chunk = readAllStandardOutput();
    //  this->ui->networkResultsLabel->setText(chunk);
}

void MainWindow::connectDB()
{
   // database = new QSqlDatabase();
    database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings

    database.setDatabaseName(databaseFileLocation);
    if(!database.open())
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
    }
    else
    {
        query = new QSqlQuery(database);
    }

}

void MainWindow::disconnectDB()
{
    database.close();
}

void MainWindow::loadSoundVolumeValues(QString device)
{
    // Get DB sound volume values

    // Get all from sound table and load

    query->exec("select id, operatorvolume, customervolume from sound");
    while (query->next())
    {
        operatorVolume = query->value(1).toInt();
        customerVolume = query->value(2).toInt();

        if(device == "o")
        {
            ui->volumeLabel->setText(QString::number(operatorVolume));
            ui->volumeSpinBox->setValue(operatorVolume);
            ui->operatorSoundBut->setChecked(true);
        }
        else
        {
            ui->volumeLabel->setText(QString::number(customerVolume));
            ui->volumeSpinBox->setValue(customerVolume);
            ui->customerSoundBut->setChecked(true);
        }
    }
}

void MainWindow::loadDisplayBrightnessValues(QString display)
{
    // Get DB display brightness values

    query->exec("select id, operatorbrightness, customerbrightness from display");
    while (query->next())
    {
        operatorBrightness = query->value(1).toString();
        qDebug() << "operatorBrightness" << operatorBrightness;
        customerBrightness = query->value(2).toString();
        qDebug() << "customerBrightness" << customerBrightness;

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
bool MainWindow::isWiredConnected()
{
    // TBD
    bool result = false;

    // result = true;
    return result;
}


void MainWindow::refreshNetworksButton_click()
{
    this->loadNetworkConnections();
}


////////////////////////////////
//
//  L O A D    N E T W O R K    C O N N E C T I O N S
//
/////////////////////////////////
void MainWindow::loadNetworkConnections()
{
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
    this->ui->availableConnectionsTableView->setStyleSheet("QTableWidget{background-color: white;border: 1px solid gray;} QTableWidget::item{ background-color: white;border: 1px solid white;} QTableWidget::item:selected{background: rgb(210, 219, 210); color: rgb(0, 58, 85); border: 1px solid rgb(204, 230, 239);} QTableWidget::item:focus {border: 0px; }");

    QStringList APs = this->getWirelessAPs();

    // first load the wired connection
    QString lightFileName = "images/disconnectedLightFinal.png";
    QString adapterIconFileName = "images/wiredIconFinal.png";
    QString strengthIconFileName = "images/strength5.png";

    this->ui->availableConnectionsTableView->setRowCount(APs.count()/2 + networkCount); // account for wired and vpn

    QTableWidgetItem *connectionNameNStatusItem = new QTableWidgetItem;
    QString nameNstatus = "Wired Ethernet";
    int signalStrength = 1000; // TBD get ethernet connection speed
    // Show connected icon and wording
    if(isWiredConnected())
    {
        this->adapterAlreadyConnected = true;
        lightFileName = "images/connectedLightFinal.png";
    }
    else
    {
        this->adapterAlreadyConnected = false;
    }

    connectionNameNStatusItem->setText(nameNstatus);

    QPixmap lightPic(lightFileName);
    QIcon lightIcon;

    lightIcon.actualSize(QSize(35,69));
    lightIcon.addPixmap(lightPic);

    QPixmap adapterIconPic(adapterIconFileName);
    QIcon adapterIcon;

    adapterIcon.addPixmap(adapterIconPic);
    QTableWidgetItem *connectionIconItem = new QTableWidgetItem;
    connectionIconItem->setSizeHint(QSize(35,69));
    connectionIconItem->setIcon(lightIcon);

    QTableWidgetItem *adapterIconItem = new QTableWidgetItem;
    adapterIconItem->setIcon(adapterIcon);

    // Ethernet Speed
    // TBD get live signal strength percentage here
    QTableWidgetItem *strengthIconItem = new QTableWidgetItem;

    strengthIconFileName = "images/" + QString::number(signalStrength) +  "Mbps.png";

    QPixmap strengthIconPic(strengthIconFileName);
    QIcon strengthIcon;
    strengthIcon.addPixmap(strengthIconPic);
    strengthIconItem->setIcon(strengthIcon);
    strengthIconItem->setTextAlignment(Qt::AlignLeft);

    QTableWidgetItem *wiredWirelessGroupBoxTypeItem = new QTableWidgetItem;
    wiredWirelessGroupBoxTypeItem->setText("0");

    // Set column widths to allowable table width of network connections. Theming will govern this real estate

    this->ui->availableConnectionsTableView->setItem(0,0, connectionIconItem);
    this->ui->availableConnectionsTableView->setItem(0,1, connectionNameNStatusItem);
    this->ui->availableConnectionsTableView->setItem(0,2, adapterIconItem);
    this->ui->availableConnectionsTableView->setItem(0,3, strengthIconItem);
    this->ui->availableConnectionsTableView->setItem(0,4, wiredWirelessGroupBoxTypeItem);
    this->ui->availableConnectionsTableView->setRowHeight(0, 69);
    this->ui->availableConnectionsTableView->setColumnWidth(0, 35);
    this->ui->availableConnectionsTableView->setColumnWidth(1, 139);
    this->ui->availableConnectionsTableView->setColumnWidth(2, 44);
    this->ui->availableConnectionsTableView->setColumnWidth(3, 44);
    this->ui->availableConnectionsTableView->setColumnWidth(4, 0);

//////////////////////////////////////////
    // TBD add VPN connection

    if(false  /*this->useVPNConnection()*/)
    {
        QTableWidgetItem *vpnAdapterIconItem = new QTableWidgetItem;
        QTableWidgetItem *vpnConnectionNameNStatusItem = new QTableWidgetItem;
        QTableWidgetItem *vpnLightIconItem = new QTableWidgetItem;
        QString vpnAdapterIconFileName = "images/VPNIconFinal.png";

        QPixmap vpnAdapterIconPic(vpnAdapterIconFileName);

        QIcon vpnAdapterIcon;

        vpnAdapterIcon.actualSize(QSize(35,69));
        vpnAdapterIcon.addPixmap(vpnAdapterIconPic);
        vpnAdapterIconItem->setIcon(vpnAdapterIcon);

        // TBD get connected or not;
        if(this->adapterAlreadyConnected)
        {
            lightFileName = "images/disconnectedLightFinal.png";
        }
        else
        {
            lightFileName = "images/connectedLightFinal.png";
            this->adapterAlreadyConnected = true;
        }

        QPixmap lightIconPic(lightFileName);
        QIcon lightIcon;

        lightIcon.actualSize(QSize(35,69));
        lightIcon.addPixmap(lightIconPic);
        vpnLightIconItem->setIcon(lightIcon);
        nameNstatus = "VPN";

        vpnConnectionNameNStatusItem->setText(nameNstatus);

        // Ethernet Speed
        // TBD get live signal strength percentage here
        QTableWidgetItem *vpnStrengthIconItem = new QTableWidgetItem;

        //TBD get VPN speed
        signalStrength = 100;
        strengthIconFileName = "images/" + QString::number(signalStrength) +  "Mbps.png";

        QPixmap vpnStrengthIconPic(strengthIconFileName);
        QIcon strengthIcon;
        strengthIcon.addPixmap(vpnStrengthIconPic);
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

    /////////////////////////////////////////////////////////////
    // add wireless networks
    // if wirless networks are not shown or none are reachable, show just the wired and possibly VPN

    adapterIconFileName = "images/wirelessIconFinal.png";

    for(int i = 0; i < APs.count()/2; i++)
    {
        QTableWidgetItem *adapterIconItem = new QTableWidgetItem;
        QTableWidgetItem *connectionNameNStatusItem = new QTableWidgetItem;
        QTableWidgetItem *lightIconItem = new QTableWidgetItem;
        QTableWidgetItem *strengthIconItem = new QTableWidgetItem;

        QPixmap adapterIconPic(adapterIconFileName);
        QIcon adapterIcon;
        adapterIcon.addPixmap(adapterIconPic);
        adapterIconItem->setIcon(adapterIcon);


        nameNstatus = APs.at(i*2+1);

        strengthIconFileName = "images/strength" + APs.at(i*2) + ".png";

        // TBD get connected or not;
        if(this->adapterAlreadyConnected)
        {
            lightFileName = "images/disconnectedLightFinal.png";
        }
        else
        {
            lightFileName = "images/connectedLightFinal.png";
            this->adapterAlreadyConnected = true;
        }

        QPixmap lightIconPic(lightFileName);
        QIcon lightIcon;

        // Show only the proper light for the signal strength reported by the local networks within range.

        //QPixmap lightIconItem(lightFileName);

        lightIcon.actualSize(QSize(35,69));
        lightIcon.addPixmap(lightPic);

        lightIconItem->setIcon(lightIcon);
        connectionNameNStatusItem->setText(nameNstatus);
        if(this->adapterAlreadyConnected)
        {
            this->adapterAlreadyConnected = true;
            lightFileName = "images/disconnectedLightFinal.png";
        }
        else
        {
            this->adapterAlreadyConnected = false;
        }

        QPixmap strengthIconPic(strengthIconFileName);
        QIcon strengthIcon;
        strengthIcon.addPixmap(strengthIconPic);
        strengthIconItem->setIcon(strengthIcon);
        strengthIconItem->setTextAlignment(Qt::AlignLeft);

        QTableWidgetItem *wirelessGroupBoxTypeItem = new QTableWidgetItem;
        wirelessGroupBoxTypeItem->setText("2");

        this->ui->availableConnectionsTableView->setRowHeight(i+this->networkCount, 69);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,0, lightIconItem); //TBD
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,1, connectionNameNStatusItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,2, adapterIconItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,3, strengthIconItem);
        this->ui->availableConnectionsTableView->setItem(i+this->networkCount,4, wirelessGroupBoxTypeItem);
    }
    this->ui->alertsLineEdit->setText(QString::number(this->ui->availableConnectionsTableView->rowCount()) + " networks");
    if(visDemoMode) this->ui->availableConnectionsTableView->selectRow(1);
}

////////////////////////////////
//
//  G E T  W I R E L E S S   A P s
//
/////////////////////////////////
QStringList MainWindow::getWirelessAPs()
{
    this->ui->alertsLineEdit->setText(tr("Trying Connection..."));

    QProcess saveNetworkConfiguration;
    QStringList APs;
    QString worked;
    QStringList options;
    options.append("python/qmSettings/network/qmSettingsNetwork.py");
    options.append("-X");
    options.append("1");

    if(!visDemoMode)
    {
        saveNetworkConfiguration.start("python", options);

        if(!saveNetworkConfiguration.waitForStarted())
        {
            this->ui->alertsLineEdit->setText(tr("Python did not start"));
            // python did not start
            //return;
        }

        if(!saveNetworkConfiguration.waitForFinished())
        {
            // do progressBar here
            this->ui->alertsLineEdit->setText(tr("Python did not finish"));
            // return;
            // python did not finish
        }

        QByteArray result = saveNetworkConfiguration.readAll();
        QString insideWorked(result);
        worked = insideWorked;
    }
    else //marketing Tablet
    {
        worked = "3|foodRouter1|4|WAP-deli|1|Bakery-Router|5|Hobart|5|Manager-AP|3|ProduceRouter|2|Marketing-AP|";
    }

    APs = worked.split("|", QString::SkipEmptyParts);

//    in case wireless device reports an SSID with a blank name, Hobart does not want to list it
    for (int i = 0; i < APs.count(); i++)
    {
        if(APs.at(i).trimmed().isEmpty() || APs.at(i).trimmed().isNull())
        {
            APs.removeAt(i);
        }
    }

    // the save was successful
    return APs;

}

///////////////////////////////////////////////////////////////////////////////////////
//
//     U S E   V P N
//
///////////////////////////////////////////////////////////////////////////////////////
/*bool MainWindow::useVPNConnection()
{
    bool result = false; // failsafe
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
        //QSqlQuery query;

        // Update the single row in the table
        query.exec("select isvpn from qmsettingsnetwork");
        while (query.next())
        {
            dbSetting = query.value(0).toInt();
        }
    }
    database->close();

    // Set the widgets Look and Feel
    if(dbSetting != 0)
    {
        // The VPN Connection is used
        result = true;
        this->ui->VPNUserLineEdit->setEnabled(true);
        this->ui->VPNUserLineEdit->setStyleSheet("color:rgb(0,0,0);");
        this->ui->VPNUserLineEdit->setPlaceholderText("Enter Username");
        this->ui->VPNPasswordLineEdit->setEnabled(true);
        this->ui->VPNPasswordLineEdit->setStyleSheet("color:rgb(0, 0, 0);");
        this->ui->VPNPasswordLineEdit->setPlaceholderText("Enter Password");
    }
    else
    {
        this->ui->VPNUserLineEdit->setDisabled(true);
        this->ui->VPNUserLineEdit->setStyleSheet("color:rgb(55, 0, 0);");
        this->ui->VPNUserLineEdit->setPlaceholderText("Disabled");
        this->ui->VPNPasswordLineEdit->setDisabled(true);
        this->ui->VPNPasswordLineEdit->setStyleSheet("color:rgb(55, 0, 0);");
        this->ui->VPNPasswordLineEdit->setPlaceholderText(("Disabled"));
    }

    // return if VPN is being used
    return result;
}
*/
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
///////////////////////////////////////////////////////////////////////////////////////
//
//     S A V E   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::saveNetworkBut_click()
{

    // Call Python script here, must be located in network folder on target OS
    // TBD define network folder


    this->ui->alertsLineEdit->setText(tr("Trying Connection..."));

    QProcess saveNetworkConfiguration;

    saveNetworkConfiguration.start("python", this->buildNetworkCommandOptions(commonNetwork));

    if(!saveNetworkConfiguration.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        // return;
    }

    if(!saveNetworkConfiguration.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

    QByteArray result = saveNetworkConfiguration.readAll();
    QString worked(result);

    worked = worked.mid(0, worked.length()-1);

    // was the save successful

    if(worked == "YES")
    {
        this->ui->alertsLineEdit->setText(tr("Network is Connected"));
        this->ui->alertsLineEdit->setStyleSheet("color:green;");
    }
    else
    {
        this->ui->alertsLineEdit->setText(tr("Network is NOT Connected"));
        this->ui->alertsLineEdit->setStyleSheet("color:red;");
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    //
    //     BEGINNING OF ALEX'S CODE
    //
    ///////////////////////////////////////////////////////////////////////////////////////
    //Start saving the configurations
    QProcess saveConfigs;
    saveConfigs.start("python", this->buildSaveConfigurationOptions(commonNetwork));

    if(!saveConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        // return;
    }

    if(!saveConfigs.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

    //this->ui->alertsLineEdit->setText(tr("Connection Saved"));
    //this->ui->alertsLineEdit->setStyleSheet("color:green;");
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   S A V E   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList MainWindow::buildSaveConfigurationOptions(Network *locNetwork) {
    QStringList options;
    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    options.append("python/qmSettings/network/saveConfigs.py");

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
    else {
        options.append("-N");
        options.append(locNetwork->EAPUsername());

        options.append("-n");
        options.append(locNetwork->EAPPassword());
    }

    options.append("-Z");

    //PASSPHRASE DEFINED HERE
    options.append(this->PASSPHRASE);
    return options;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     I M P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::importNetworkBut_click() {
    QProcess importConfigs;
    QStringList myOptions = this->buildImportConfigurationOptions(commonNetwork);
    if(myOptions.isEmpty()) {
        return;
    }
    importConfigs.start("python", myOptions);

    //Wait for import to finish
    if(!importConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!importConfigs.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

    //Get stdout from importConfigs
    QByteArray result = importConfigs.readAll();
    QString configs(result);
    QStringList res = configs.split(QRegExp("[\r\n]"),configs.SkipEmptyParts);

    //Use regex to split results
    //QRegExp configurationFormat("[\\w\.\(\)\-]+ : (\\w*)");
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
    }*/

    //Fill in the fields using the set functions
    if(bestResults.isEmpty()) {
        //if it gets here, then regex is wrong
        qDebug() << "Not setting fields correctly";
        return;
    }

    else if(bestResults.size() == 4) { //wpa
        commonNetwork->setSSID(bestResults[0]);
        commonNetwork->setSecurityMethod(bestResults[1]);
        commonNetwork->setEncryption(bestResults[2]);
        commonNetwork->setWPAPassPhrase(bestResults[3]);

        //Update SSID Info
        this->ui->SSIDLineEdit->setText(bestResults[0]);
        setWirelessNetworkEncryptionCombo(1);
        //Update Combo Boxes
        int pos = this->ui->securityCombo->findData(bestResults[1]);
        if(pos == -1) {
            qDebug() << bestResults[1];
            qDebug() << "couldnt find security Combo here 0";
            return;
        }
        else {
            this->ui->securityCombo->setCurrentIndex(pos);
        }

        pos = this->ui->encryptionCombo->findData(bestResults[2]);
        if(pos == -1) {
            qDebug() << bestResults[2];
            qDebug() << "couldnt find encryption Combo here 1";
            return;
        }
        else {
            this->ui->encryptionCombo->setCurrentIndex(pos);
        }

        this->ui->wpaPassphraseLineEdit->setText(bestResults[3]);
    }
    else if(bestResults.size() == 5) {//leapPeap
        commonNetwork->setSSID(bestResults[0]);
        commonNetwork->setSecurityMethod(bestResults[1]);
        commonNetwork->setEncryption(bestResults[2]);
        commonNetwork->setEAPUsername(bestResults[3]);
        commonNetwork->setEAPPassword(bestResults[4]);

        //Update SSID Info
        this->ui->SSIDLineEdit->setText(bestResults[0]);

        //Update Combo Boxes
        loadWirelessSecurityMethodsCombo();
        int pos = this->ui->securityCombo->findData(bestResults[1]);
        if(pos == -1) {
            qDebug() << bestResults[1];
            qDebug() << "couldnt find security Combo here 3";
            return;
        }
        else {
            this->ui->securityCombo->setCurrentIndex(pos);
        }

        if(bestResults[1].contains("8")) {
            setWirelessNetworkEncryptionCombo(7);
        }
        else {
            setWirelessNetworkEncryptionCombo(2);
            pos = this->ui->encryptionCombo->findData(bestResults[2]);
            if(pos == -1) {
                qDebug() << bestResults[2];
                qDebug() << "couldnt find encryption Combo here 4";
                return;
            }
            else {
                this->ui->encryptionCombo->setCurrentIndex(pos);
            }
        }




        this->ui->eapUsernameLineEdit->setText(bestResults[3]);
        this->ui->eapPasswordLineEdit->setText(bestResults[4]);
    }
    else {//wep
        commonNetwork->setSSID(bestResults[0]);
        commonNetwork->setSecurityMethod(bestResults[1]);
        commonNetwork->setEncryption(bestResults[2]);
        commonNetwork->setDefaultKey(bestResults[3]);
        commonNetwork->setWEPKey1(bestResults[4]);
        commonNetwork->setWEPKey2(bestResults[5]);
        commonNetwork->setWEPKey3(bestResults[6]);
        commonNetwork->setWEPKey4(bestResults[7]);

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
        this->ui->wepKey1LineEdit->setText(this->commonNetwork->WEPKey1());
        this->ui->wepKey2LineEdit->setText(this->commonNetwork->WEPKey2());
        this->ui->wepKey3LineEdit->setText(this->commonNetwork->WEPKey3());
        this->ui->wepKey4LineEdit->setText(this->commonNetwork->WEPKey4());
    }

    this->ui->alertsLineEdit->setText(tr("Network Successfully Imported"));
    this->ui->alertsLineEdit->setStyleSheet("color:green;");

}

///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   I M P O R T   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList MainWindow::buildImportConfigurationOptions(Network * locNetwork) {

    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    QStringList options;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/CHOOSE/THIS/DIRECTORY");
    if(fileName.isNull()) {//user clicked cancel
        return options;
    }

    options.append("python/qmSettings/network/importConfigs.py");

    options.append("-F");
    options.append(fileName);

    options.append("-Z");

    //PASSPHRASE DEFINED HERE
    options.append(PASSPHRASE);

    return options;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//     E X P O R T   N E T W O R K  C O N F I G U R A T I O N   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::exportNetworkBut_click() {
    QProcess exportConfigs;
    QStringList myOptions = this->buildExportConfigurationOptions();
    if(myOptions.isEmpty()) { //If true, the user selected cancel when opening a file to export to
        return;
    }
    exportConfigs.start("python", myOptions);

    if(!exportConfigs.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        // return;
    }

    if(!exportConfigs.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

    this->ui->alertsLineEdit->setText(tr("Network Successfully Exported"));
    this->ui->alertsLineEdit->setStyleSheet("color:green;");
}
///////////////////////////////////////////////////////////////////////////////////////
//
//     B U I L D   E X P O R T   C O N F I G U R A T I O N  C O M M A N D
//
///////////////////////////////////////////////////////////////////////////////////////
QStringList MainWindow::buildExportConfigurationOptions() {
    QStringList options;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"/CHOOSE/THIS/DIRECTORY");
    if(fileName.isNull()) {//user clicked cancel
        return options;
    }

    options.append("python/qmSettings/network/exportConfigs.py");

    options.append("-F");
    options.append(fileName);

    return options;
}

 //////////////////////////////////
//
//     B U I L D   N E T W O R K   C O M M A N D   O P T I O N S
//
///////////////////////////////////

QStringList MainWindow::buildNetworkCommandOptions(Network *locNetwork)
{

    // Load the commonNetwork to the App first
    this->setCommonNetwork(locNetwork);

    QStringList options;

    options.append("python/qmSettings/network/qmSettingsNetwork.py");
    options.append("-U");
    options.append(QString::number(locNetwork->OperatorID()));

    options.append("-m");
    options.append(locNetwork->MachineID());

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
    options.append(this->ui->wpaPassphraseLineEdit->text());

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
    options.append(locNetwork->WEPKey1());

    options.append("-x");
    options.append(locNetwork->WEPKey2());

    options.append("-y");
    options.append(locNetwork->WEPKey3());

    options.append("-z");
    options.append(locNetwork->WEPKey4());

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

    // Do not set network, just requesting Access Points
    options.append("-X");
    options.append("0");



    return options;
}

///////////////////////////////////////
//
//       A U T O   D N S  R A D I O
//
///////////////////////////////////

QString MainWindow::modelName()
{

    QProcess modelNameProcess;

    QStringList options;
    options.append("python/qmSettings/system/model.py");
    modelNameProcess.start("python", options);

    if(!modelNameProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    //   saveNetworkConfiguration.write(" TBD ");
    //   saveNetworkConfiguration.closeWriteChannel();

    if(!modelNameProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // return;
        // python did not finish
    }

    QByteArray result = modelNameProcess.readAll();
    QString worked(result);

    return worked.mid(0,worked.length()-1);

}

///////////////////////////////////////
//
//       A U T O   D N S  R A D I O
//
///////////////////////////////////

void MainWindow::autoDNSRadio_click()
{
    if(this->ui->autoDNSRadio->isChecked())
    {
        this->ui->DNS1LineEdit->setEnabled(false);
        this->ui->DNS1LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->DNS2LineEdit->setEnabled(false);
        this->ui->DNS2LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->DNS3LineEdit->setEnabled(false);
        this->ui->DNS3LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
    }
    else
    {
        this->ui->DNS1LineEdit->setEnabled(true);
        this->ui->DNS1LineEdit->setStyleSheet("color:black;");
        this->ui->DNS2LineEdit->setEnabled(true);
        this->ui->DNS2LineEdit->setStyleSheet("color:black;");
        this->ui->DNS3LineEdit->setEnabled(true);
        this->ui->DNS3LineEdit->setStyleSheet("color:black;");
    }
}

///////////////////////////////////////
//
//       M A N U A L   D N S   R A D I O
//
///////////////////////////////////

void MainWindow::manualDNSRadio_click()
{
    if(this->ui->manualDNSRadio->isChecked())
    {
        this->ui->DNS1LineEdit->setEnabled(true);
        this->ui->DNS1LineEdit->setStyleSheet("color:black;");
        this->ui->DNS2LineEdit->setEnabled(true);
        this->ui->DNS2LineEdit->setStyleSheet("color:black;");
        this->ui->DNS3LineEdit->setEnabled(true);
        this->ui->DNS3LineEdit->setStyleSheet("color:black;");
    }
    else
    {
        this->ui->DNS1LineEdit->setEnabled(false);
        this->ui->DNS1LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->DNS2LineEdit->setEnabled(false);
        this->ui->DNS2LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
        this->ui->DNS3LineEdit->setEnabled(false);
        this->ui->DNS3LineEdit->setStyleSheet("color:rgb(180, 180, 180);");
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
        // python did not start
    }


    cancelNetworkConfiguration.write(" TBD ");
    cancelNetworkConfiguration.closeWriteChannel();

    if(!cancelNetworkConfiguration.waitForFinished())
    {
        ;
        // python did not finish
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
void MainWindow::setTimeTab()
{
    this->ui->timeEdit->setTime(QTime::currentTime());
    this->ui->dateEdit->setSelectedDate(QDate::currentDate());
    this->ui->astronomicalCheckBox->setChecked(false /*!getDBAstronomicalSetting()*/);
    this->setTimezone();
    //this->ui->timeZoneCombo->setCurrentIndex(getDBTimezone());

    // Reload the Internet connections
 //   this->loadNetworkConnections();
    /*

    if(getDBAstronomicalSetting()) //Check last setting
    {
        this->ui->astronomicalCheckBox->setChecked(true);
    }
    else
    {
        this->ui->astronomicalCheckBox->setChecked(false);
    }
    */
}

/*int MainWindow::getDBTimezone()
{
    // default value to a 12 hour clock; most common use
    int timeZoneIndex = 0;

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
        query.exec("select timezone from qmsettingsdate");
        while (query.next())
        {
            timeZoneIndex = query.value(0).toInt();
        }
    }
    database->close();

    return timeZoneIndex;
}
*/
void MainWindow::setHostnameBut_click()
{
    QProcess hostnameProcess;
    QStringList options;
    options.append("python/qmSettings/system/qmSettingsSetHostname.py");
    options.append("-H");
    options.append(this->ui->hostnameLineEdit->text());

    hostnameProcess.start("python", options);

    bool success = true;

    if(!hostnameProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        success = false;
        // python did not start
    }

    if(!hostnameProcess.waitForFinished())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        success = false;
        // python did not finish
    }

    if(success)
    {
        this->ui->alertsLineEdit->setText("Hostname set to " + this->ui->hostnameLineEdit->text());
    }
    else
    {
        this->ui->alertsLineEdit->setText("Hostname not set");
    }
}

void MainWindow::rebootScale()
{
    QProcess getHostnameProcess;
    QStringList options;
    options.append("python/qmSettings/system/qmSettingsReboot.py");

    getHostnameProcess.start("python", options);

    if(!getHostnameProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
    }

    if(!getHostnameProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        // python did not finish
    }
}

void MainWindow::getUIHostname()
{
    QProcess getHostnameProcess;
    QStringList options;
    options.append("python/qmSettings/system/qmSettingsGetHostname.py");

    getHostnameProcess.start("python", options);

    if(!getHostnameProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!getHostnameProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

    QByteArray result = getHostnameProcess.readAll();
    QString hostname(result);
   // line below could be changed to -1. I noticed the space after the hostname plus the \n in the /etc/hostname file
    this->ui->hostnameLineEdit->setText(hostname.mid(0,hostname.length()-1));
    this->initialHostname = hostname.mid(0,hostname.length()-1);

}

void MainWindow::getUIMACAddress()
{

   if (!visDemoMode)
   { QProcess macAddressProcess;
    QStringList options;
    options.append("python/qmSettings/system/qmSettingsGetMACAddress.py");

    macAddressProcess.start("python", options);

    if(!macAddressProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!macAddressProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

    QStringList macAddresses;
    QByteArray result = macAddressProcess.readAll();
    QString macAddress(result);
    macAddress = macAddress.mid(0,macAddress.length()-1);
    macAddresses = macAddress.split("|", QString::SkipEmptyParts);

    this->ui->ethernetMACAddressLabel->setText(macAddresses[0]);
    this->ui->wirelessMACAddressLabel->setText(macAddresses[1]);
   }
   else
   {
       this->ui->ethernetMACAddressLabel->setText("23:6S:CD:98:00:43");
       this->ui->wirelessMACAddressLabel->setText("56:2C:4B:02:18:34");
   }



    //return macAddress.mid(0,macAddress.length()-1);

}

void MainWindow::setTimezone()
{
    QProcess getTimezoneProcess;
    QStringList options;
    options.append("python/qmSettings/time/qmSettingsGetTimezone.py");

    getTimezoneProcess.start("python", options);

    if(!getTimezoneProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!getTimezoneProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
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

    initialTimeZone = this->ui->timeZoneCombo->currentText();
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

void MainWindow::setVolumeBut_click()
{
    QStringList options;
    options.append("python/qmSettings/sound/qmSettingsSetVolume.py");
    options.append("--level");
    options.append(this->ui->volumeLabel->text());
    options.append("-D");

    if(this->ui->operatorSoundBut->isChecked())
    {
        options.append("1");
    }
    else if (this->ui->customerSoundBut->isChecked())
    {
        options.append("2");
    }

    QProcess applySoundVolumeConfiguration;
//    applySoundVolumeConfiguration.waitForFinished();

    applySoundVolumeConfiguration.start("python", options);
    if(!applySoundVolumeConfiguration.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!applySoundVolumeConfiguration.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

    QProcess playSoundProcess;

    QStringList optionsplay;
    optionsplay.append("python/qmSettings/sound/qmSettingsPlaySound.py");

   // playSoundProcess.waitForFinished();

    playSoundProcess.start("python", optionsplay);
    if(!playSoundProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!playSoundProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }


    // NOTE: Add configuration for GCD customer speakers as well.
    // REQ was added 1/2/2013 by Debra Dyer
    // There is no operator hardware sound device as of 01/11/2013

    /*if(updateVolume(this->ui->volumeLabel->text().toInt(), this->ui->operatorIDLabel->text().toInt()))

    {
        this->ui->volumeResultsLabel->setText(tr("Volume successfully set."));
        this->ui->alertsLineEdit->setText(tr("Volume successfully set."));
    }
    else
    {
        this->ui->volumeResultsLabel->setText(tr("Volume not set. Try again."));
        this->ui->alertsLineEdit->setText(tr("Volume successfully set."));
    }
*/
    //set volume on the fly
    //this->ui->volumeSpinBox->setValue(this->ui->volumeLabel->text().toInt());
    this->ui->alertsLineEdit->setText("Sound volume set.");


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
    // this is for customer speakers - no hardware 01/11/2013
    // TBD
}

//////////////////////////////////////////////////////
//
//       P R I N T   P I N G   O U T P U T
//
//////////////////////////////////////////////////////

void MainWindow::printPingOutput()
{
    QString output = this->pingProcess->readAllStandardOutput();
    this->ui->pingResultsList->addItem(output.mid(0,output.length()-1));
}

//////////////////////////////////////////////////////
//
//     S T A R T   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::startPingBut_click()
{
    cancelPing = false;
    QStringList options;
    options.append("python/qmSettings/network/ping.py");
    options.append("-c");
    options.append("10");
    options.append("-d");
    options.append(this->ui->pingIPLineEdit->text());
    pingProcess = new QProcess(this);
    connect(this->pingProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printPingOutput()));
    pingProcess->start("python", options);
}

//////////////////////////////////////////////////////
//
//     S T O P   P I N G   B U T T O N   C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::stopPingBut_click()
{
      this->pingProcess->kill();
}

///////////////////////////////////////////////////////////
//
//           U P D A T E    V O L U M E
//
///////////////////////////////////////////////////////////

/*bool MainWindow::updateVolume(int percentage, int operatorID)
{
    // Fail-safe to not allow login if accident in code
    bool updated = false;


    ////////////////////////////////////////
    // PUT SETTING IN THE DB
    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");
    if(!database->open())
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."),QMessageBox::Cancel);
    }
    else
    {
        // Set volume in the system using Python script


        // Set volume inthe Database
        // Get Settings from credentials table and load
        QSqlQuery query;


        // Update the single row in the table
        query.exec("update qmsettingsvolume set volume='" + QString::number(percentage) + "', operatorid='" + QString::number(operatorID) + "', occurrence=CURRENT_TIMESTAMP where id='0'");
        updated = true;
    }
    database->close();

    // END PUT SETTING IN THE DB
    ////////////////////////////////////

    ////////////////////////////////////
    // CHANGE SYSTEM VOLUME SETTING

    // Call to set volume from Python script.


    // END CHANGE SYSTEM VOLUME SETTING
    ////////////////////////////////////
    return updated;
}
*/
///////////////////////////////////////////////////////////
//
//           G E T    D B   V O L U M E
//
///////////////////////////////////////////////////////////
/*
int MainWindow::getDBVolume()
{
    // Fail-safe to not allow login if accident in code
    int percentage = 50;

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
        query.exec("select volume from qmsettingsvolume where id='0'");
        while (query.next())
        {
            percentage = query.value(0).toInt(); // DB volume
        }
    }
    database->close();

    return percentage;
}
*/
///////////////////////////////////////////////////////////
//
//           U P D A T E    B R I G H T N E S S
//
///////////////////////////////////////////////////////////

bool MainWindow::updateBrightness(int percentage, QString device)
{
    // Fail-safe to not allow login if accident in code
    bool updated = false;

    ////////////////////////////////////////
    // PUT SETTING IN THE DB
    //QSqlDatabase *database = new QSqlDatabase();
    //*database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    //database->setDatabaseName("qmsettings.sqlite");
    if(!database.open())
    {
        // no DB connection return false
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."),QMessageBox::Cancel);
    }
    else
    {
        // Get Settings from credentials table and load

        // Update the single row in the table

        if(device == "operator")
        {
            updated = query->exec("update display set operatorbrightness=" + QString::number(percentage));
        }
        else
        {
            updated = query->exec("update display set customerbrightness=" + QString::number(percentage));
        }
    }

    // END CHANGE SYSTEM VOLUME SETTING
    ////////////////////////////////////
    return updated;
}

///////////////////////////////////////////////////////////
//
//           G E T     D B    B R I G H T N E S S
//
///////////////////////////////////////////////////////////
/*
int MainWindow::getDBBrightness()
{
    // Fail-safe to not allow login if accident in code
    int percentage = 50;

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
        query.exec("select brightness from qmsettingsbrightness where id='0'");
        while (query.next())
        {
            percentage = query.value(0).toInt(); // DB brightness
        }
    }
    database->close();

    return percentage;
}

///////////////////////////////////////////////////////////
//
//     S E T   D A T E / T I M E   B U T T O N   C L I C K
//
///////////////////////////////////////////////////////////
*/
void MainWindow::setDateTimeBut_click()
{
    // Build command
//    QString command = "python";
    QStringList options;

    options.append("python/qmSettings/time/qmSettingsTime.py");
    options.append("-U");
    options.append("0");
    options.append("-Y"); //year
    options.append(QString::number(this->pickedDate->year()));
    options.append("-M"); // month
    options.append(QString("%1").arg(this->pickedDate->month() ,2,10, QChar('0')).toUpper());
    options.append("-D"); //day
    options.append(QString("%1").arg(this->pickedDate->day(),2,10, QChar('0')).toUpper());
    options.append("-H"); // hour
    options.append(QString("%1").arg(this->ui->timeEdit->time().hour(),2,10, QChar('0')).toUpper());
    options.append("-m"); // minute
    options.append(QString("%1").arg(this->ui->timeEdit->time().minute(),2,10, QChar('0')).toUpper());
    options.append("-S"); // second
    options.append(QString("%1").arg(this->ui->timeEdit->time().second(),2,10, QChar('0')).toUpper());
    options.append("-T"); // +- 2 digit hour
    options.append(this->ui->timeZoneCombo->currentText().mid(4,3));
    options.append("-t"); // timezonetext; the setting used by Hobart products
    options.append(QString("%1").arg(this->ui->timeZoneCombo->itemData(this->ui->timeZoneCombo->currentIndex()).toString()));
    options.append("-A"); // astronomical clock (12 or 24 hour)

    if(this->ui->astronomicalCheckBox->isChecked())
    {
        options.append("0");
    }
    else
    {
        options.append("1");
    }

    // Using set timezones

    // Call python to set time date
    QProcess timeProcess;
    timeProcess.start("python", options);
    if(!timeProcess.waitForStarted())
    {
        this->ui->alertsLineEdit->setText(tr("Python did not start"));
        // python did not start
        //return;
    }

    if(!timeProcess.waitForFinished())
    {
        // do progressBar here
        this->ui->alertsLineEdit->setText(tr("Python did not finish"));
        //return "";
        // python did not finish
    }

    if(this->ui->timeZoneCombo->currentText() != initialTimeZone)
    {
        changedTimezone = true;

    }
    else changedTimezone = false;

  //  timeProcess->waitForFinished();

    // Store in db
    //this->updateDBTimeDate();

  // Puts the focus at the calendar to eliminate highlighting
   // this->ui->dateEdit->setFocus();
}

/*
void MainWindow::updateDBTimeDate()
{
    // Default to using 12 hour clock
    int useNonAstronomical = 1;

    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");
    if(this->ui->astronomicalCheckBox->isChecked() )
    {
        useNonAstronomical = 0;
    }
    else
    {
        useNonAstronomical = 1;
    }

    QString userTimeDateSetting = "";

    userTimeDateSetting.append(QString("%1").arg(this->pickedDate->year() ,4,10, QChar('0')).toUpper());
    userTimeDateSetting.append("-");
    userTimeDateSetting.append(QString("%1").arg(this->pickedDate->month() ,2,10, QChar('0')).toUpper());
    userTimeDateSetting.append("-");
    userTimeDateSetting.append(QString("%1").arg(this->pickedDate->day() ,2,10, QChar('0')).toUpper());

    userTimeDateSetting.append(" ");
    userTimeDateSetting.append(QString("%1").arg(this->ui->timeEdit->time().hour() ,2,10, QChar('0')).toUpper());
    userTimeDateSetting.append(":");
    userTimeDateSetting.append(QString("%1").arg(this->ui->timeEdit->time().minute() ,2,10, QChar('0')).toUpper());
    userTimeDateSetting.append(":");
    userTimeDateSetting.append(QString("%1").arg(this->ui->timeEdit->time().second() ,2,10, QChar('0')).toUpper());

    QString command = "update qmsettingsdate set timezone=" + QString::number(this->ui->timeZoneCombo->currentIndex()) + ", isdaylightsavings=" + "1" + ", occurrence=CURRENT_TIMESTAMP, operatorid=0, machineid=0, isnonastronomical=" + QString::number(useNonAstronomical) + ", time='" + userTimeDateSetting + "'";

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");
    if(!database->open())
    {
        // no DB connection return false
        QMessageBox::critical(0, tr("Cannot open database"),
                              tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
    }
    else
    {
        QSqlQuery query;
        query.exec(command);
    }
    database->close();

    this->ui->alertsLineEdit->setText(tr("Time and Date successfully saved."));

}
*/
//////////////////////////////////////////////////////
//
//        G E T   T I M E Z O N E S
//
//////////////////////////////////////////////////////
// get the refence to QComboBox for filling
void MainWindow::fillTimeZones()
{
    // TODO:
    // We should put mapping of Province,Country to timezone file path in the path script
    // This would be more generic throughout the system (manufacturing test and settings wizard)
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
}

//////////////////////////////////////////////////////
//
//        A D D    U S E R    B U T T O N
//
//////////////////////////////////////////////////////

/*void MainWindow::addUserBut_click()
{
   // Stub

    if(this->ui->addUsernameBox->text().toLower() == "service")
    {
       int isAdmin = 0;
       //if(this->ui->isAdminCheckBox->isChecked()) isAdmin = 1;
       if(this->ui->isAdminScrollBar->value() == 0) isAdmin = 1;
       insertUser(this->ui->addUsernameBox->text(), this->ui->addPasswordBox->text(), isAdmin);
    }
    else
    {
        this->ui->addUserResultsLabel->setText(tr("Username ") + this->ui->addUsernameBox->text() + tr(" is not permitted."));
    }
}
*/
//////////////////////////////////////////////////////
//
//        D E L E T E   U S E R   B U T T O N
//
//////////////////////////////////////////////////////
/*
void MainWindow::deleteUserBut_click()
{
    if(this->ui->addUsernameBox->text().toLower() != "service")
    {
        deleteUser(this->ui->addUsernameBox->text());
    }
    else
    {
        this->ui->addUserResultsLabel->setText(tr("Variations of the Username \"Service\" are not permitted."));
    }
}
*/
//////////////////////////////////////////////////////
//
//   U S E R    P A S S    A R E   C O R R E C T
//
//////////////////////////////////////////////////////
/*
bool MainWindow::userPassAreCorrect(QString username, QString password)
{
    // Fail-safe to not allow login if accident in code
    bool authenticated = false;

    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    // The main DB for QMSettings
    database->setDatabaseName("qmsettings.sqlite");

    if(username == "service" && password == ServicePassword && USE_BACKDOOR) //backdoor for service
    {
        authenticated = true;
    }
    else
    {
        if(!database->open())
        {
            // no DB connection return false
            QMessageBox::critical(0,tr("Cannot open database"),
                                  tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
        }
        else
        {
            // Get Settings from credentials table and load
            QSqlQuery query;
            query.exec("select username, password, isAdmin from credentials where username='" + username + "'");

            // compare encrypted User entry with DB
            while (query.next())
            {
                if(encrypt(password)== query.value(1).toString())
                {
                    authenticated = true;
                    this->ui->adminTab->hide();
                    this->ui->adminTab->setEnabled(query.value(2).toBool());
                }
            }

            if(authenticated)
            {
                //User is allowed
                this->ui->usernameBox->setText("");
                this->ui->usernameBox->setFocus();
            }
            else
            {
                this->ui->passwordBox->setFocus();
            }
        }
        database->close();
        this->ui->passwordBox->setText("");
    }
    // return true;
    return authenticated;
}
*/
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

//////////////////////////////////////////////////////
//
//   A L L O W    U S E R   A C C E S S
//
//////////////////////////////////////////////////////

void MainWindow::allowUserAccess()
{
    mode = "user";
    changeMode();

    // blank that login results label
    this->ui->loginResultsLabel->setText("");

    // DB connection
    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE");

    database->setDatabaseName("qmsettings.sqlite");

    if(!database->open()) // No DB connection
    {
        QMessageBox::critical(0,tr("Cannot open database"),
                              tr("Unable to establish a database connection\n"),QMessageBox::Cancel);
    }
    else                  // DB is connected
    {
        //////////////////////////////////////////////////////
        //////////////// FIRST Load Last DB Settings /////////
        //////////////////////////////////////////////////////

        // Get Last DateTime Settings from DB and load
        QSqlQuery query;
        if(!query.exec("select timezone, isdaylightsavings, occurrence, operatorid from qmsettingsdate"))
        {
;
        }
    else
        {
        // DATE/TIME
        QString timezone = "";
      //  bool isDaylightSavings = true;
        QDateTime occurrence;
        qint32 operatorID;

        while (query.next())
        {
            timezone = query.value(0).toString();         // Set timezone
          //  isDaylightSavings = query.value(1).toBool();  // Set whether is daylight savings time or not
            occurrence = query.value(2).toDateTime();     // Set when setting took place (Diagnostics)
            operatorID = query.value(3).toInt();          // Set Operator ID
        }

        this->ui->operatorIDLabel->setText(QString::number(operatorID));
     //   this->ui->daylightSavingsLabel->setText(isDaylightSavings ? "true" : "false" );
     //   this->ui->lastSettingLabel->setText(occurrence.toString());
        this->ui->timezoneLabel->setText(timezone);

        // XXX Volume
        // XXX TBD


        // XXX Network
        // XXX

        // XXX Touch Screen
        // XXX
        }
    }

    QTableView queryView;
       QSqlQueryModel queryModel;
         queryModel.setQuery("SELECT username,password,isadmin FROM credentials");

         if (queryModel.lastError().isValid())
             qDebug() << queryModel.lastError();

     // not work nothing is display
         queryView.setModel(&queryModel);
         queryView.show();


         this->ui->adminTableView->setModel(&queryModel);

    //     this->ui->mainTab->setStyleSheet("QTabBar::tab { height: 50px; width: 187px; }");


     // works, display correct value
     // dziala i wyswietla poprawna komrke z tabelohioi
    database->close();
}

//////////////////////////////////////////////////////
//
//        D E L E T E   U S E R
//
//////////////////////////////////////////////////////

bool MainWindow::deleteUser(QString username)
{
    // Fail-safe to not allow login if accident in code
    bool deleted = false;

    if(username.toLower() != "service") //backdoor for service
    {
        QSqlDatabase *database = new QSqlDatabase();
        *database = QSqlDatabase::addDatabase("QSQLITE");

        // The main DB for QMSettings
        database->setDatabaseName("qmsettings.sqlite");
        if(!database->open())
        {
            // no DB connection return false
            QMessageBox::critical(0,tr("Cannot open database"),
                                  tr("Unable to establish a database connection.\nTry again."), QMessageBox::Cancel);
        }
        else
        {
            // Get Settings from credentials table and load
            QSqlQuery query;

            // looking for unique additions only
            query.exec("select COUNT(*) from credentials where username='" + username + "'");

            // See if username is a duplicate
            while (query.next()) // count returns only one row
            {
                if(query.value(0).toInt()) // safe to delete
                {
                    if(query.exec("delete from credentials where username='" + username + "'"))
                    {
                        this->ui->addUserResultsLabel->setText("User " + username + " deleted.");
                        deleted = true;
                    }
                    else
                    {
                        this->ui->addUserResultsLabel->setText("User" + username + " not deleted.");
                    }
                }
            }
        }
        database->close();
        this->ui->addUsernameBox->setText(""); //clear the User's entries
        this->ui->addPasswordBox->setText("");
        this->ui->isAdminScrollBar->setValue(1);
    }
    return deleted;
}

//////////////////////////////////////////////////////
//
//        I N S E R T   U S E R
//
//////////////////////////////////////////////////////

bool MainWindow::insertUser(QString username, QString password, int isAdmin)
{
    // Fail-safe to not allow login if accident in code
    bool inserted = false;

    if(username.toLower() == "service") //backdoor for service
    {
        QSqlDatabase *database = new QSqlDatabase();
        *database = QSqlDatabase::addDatabase("QSQLITE");

        // The main DB for QMSettings
        database->setDatabaseName("qmsettings.sqlite");
        if(!database->open())
        {
            // no DB connection return false
            QMessageBox::critical(0, tr("Cannot open database"),
                                  tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
        }
        else
        {
            // Get Settings from credentials table and load
            QSqlQuery query;

            // looking for unique additions only
            query.exec("select COUNT(*) from credentials where username='" + username + "'");

            // See if username is a duplicate
            while (query.next()) // count returns only one row
            {
                if(!query.value(0).toInt()) // safe to insert
                {
                    inserted = query.exec("insert into credentials (username, password, isadmin) values ('"+ username + "','" + encrypt(password) + "','" + QString::number(isAdmin) + "')");
                    this->ui->addUserResultsLabel->setText(tr("User ") + username + tr(" added."));
                    inserted = true;
                }
                else
                {
                    this->ui->addUserResultsLabel->setText(tr("User ") + username + tr(" not added"));
                }
            }
        }
        database->close();
        this->ui->addUsernameBox->setText("");
        this->ui->addPasswordBox->setText(""); //clear the User's entries
        this->ui->isAdminScrollBar->setValue(1);
    }
    return inserted;
}

//////////////////////////////////////////////////////
//
//        L O A D   U S E R S
//
//////////////////////////////////////////////////////

bool MainWindow::loadUsers()
{
    // Fail-safe to not allow login if accident in code
    bool loaded = false;


        QSqlDatabase *database = new QSqlDatabase();
        *database = QSqlDatabase::addDatabase("QSQLITE");

        // The main DB for QMSettings
        database->setDatabaseName("qmsettings.sqlite");
        if(!database->open())
        {
            // no DB connection return false
            QMessageBox::critical(0, tr("Cannot open database"),
                                  tr("Unable to establish a database connection\nApplication will now close."), QMessageBox::Cancel);
        }
        else
        {
            QSqlQuery query;

            // looking forall users
            query.exec("select  from credentials");

            while (query.next())
            {
               // timezone = query.value(0).toString();         // Set timezone
               // isDaylightSavings = query.value(1).toBool();  // Set whether is daylight savings time or not
               // occurrence = query.value(2).toDateTime();     // Set when setting took place (Diagnostics)
               // operatorID = query.value(3).toInt();          // Set Operator ID
            }
            loaded = true;
        }
        database->close();
        this->ui->addUsernameBox->setText("");
        this->ui->addPasswordBox->setText(""); //clear the User's entries
        this->ui->isAdminScrollBar->setValue(1);
//        this->ui->isAdminCheckBox->setChecked(false);

    return loaded;
}
*/
//////////////////////////////////////////////////////
//
//      A L E R T S   C U R R E N T   I N D E X
//
//            C H A N G E D
//
//////////////////////////////////////////////////////

void MainWindow::currentChangedSlot(int index)
{
    switch (index)
    {
        case 0:
            this->ui->alertsLineEdit->setText(tr("Change Display Settings"));
            this->ui->setBrightnessBut->setFocus();
        //this->ui->brightnessSpinBox->setFocus();
            break;
        case 1:
            this->ui->alertsLineEdit->setText(tr("Change Sound Settings"));
            this->ui->setVolumeBut->setFocus();
        //this->ui->volumeSpinBox->setFocus();
            break;
        case 2:
            this->ui->alertsLineEdit->setText(tr("Change Time Settings"));
            this->ui->setDateTimeBut->setFocus();
        //this->ui->dateEdit->setFocus();
            break;
        case 3:
            this->ui->alertsLineEdit->setText(tr("Change Network Settings"));
            this->ui->networkSaveBut->setFocus();
            break;
        case 4:
            this->ui->alertsLineEdit->setText(tr("Change Host Settings"));
            this->ui->setHostnameBut->setFocus();
        //this->ui->pingIPLineEdit->setFocus();
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

void MainWindow::brightnessSpinBoxValueChanged(int value)
{

    this->ui->brightnessLabel->setText(QString::number(value));


	this->setBrightnessIndicators(value);
}

//////////////////////////////////////////////////////
//
//        S E T    B R I G H T N E S S    C L I C K
//
//////////////////////////////////////////////////////
void MainWindow::setBrightnessBut_click()
{
    //Call brightness with
//    echo -n 255 > /sys/class/backlight/pwm-backlight.3/brightness
 //TBD Save to DB and set brightness
    if (this->ui->operatorScreenBut->isChecked())
    {
        displayDevice = "operator";
    }
    else if(this->ui->customerScreenBut->isChecked())
    {
        displayDevice = "customer";
    }
    else
    {
        return;
    }

    pythonProcess = new python();

    // Set backlight of both displays to max
    QStringList args;

    args.append(displayDevice);

    if(static_cast<int>(this->ui->brightnessLabel->text().toDouble() * 2.55) < 3)
    {
        args.append("3");
    }
    else
    {
        args.append(QString::number(static_cast<int>(this->ui->brightnessLabel->text().toDouble() * 2.55)) );
    }

    if (pythonProcess->execCommand("setBrightness", args))
    {
        if (pythonProcess->getExitCode() == 0)
        {
            ui->alertsLineEdit->setText(alertsText);
        }
        else
        {
            // TODO:
            // what do we do here?
            //LOG_DEBUG("exit code = %d", pythonProcess->getExitCode());
        }
    }
    else
    {
        // TODO:
        // What do we do here?
    }

    //XXXXXXXXXXXXXXXXXXXXXXXXXXX


    if(this->updateBrightness(ui->brightnessLabel->text().toInt(),displayDevice))
    {
        if(displayDevice == "customer")
        {
            alertsText = "Customer";
        }
        else
        {
            alertsText = "Operator";
        }
        this->ui->alertsLineEdit->setText(alertsText + " display brightness set.");
    }
    else
    {
        // failed to save
        this->ui->alertsLineEdit->setText("Display brightness not set.");
    }
}

//////////////////////////////////////////////////////
//
//       V O L U M E   I N D I C A T O R S    C L I C K
//
//////////////////////////////////////////////////////

void MainWindow::volumeIndicator0_click()
{
    doPreset(0);
}

void MainWindow::volumeIndicator1_click()
{
    doPreset(1);
}

void MainWindow::volumeIndicator2_click()
{
    doPreset(2);
}

void MainWindow::volumeIndicator3_click()
{
    doPreset(3);
}

void MainWindow::volumeIndicator4_click()
{
    doPreset(4);
}

void MainWindow::volumeIndicator5_click()
{
    doPreset(5);
}

void MainWindow::volumeIndicator6_click()
{
    doPreset(6);
}

void MainWindow::volumeIndicator7_click()
{
    doPreset(7);
}

void MainWindow::volumeIndicator8_click()
{
    doPreset(8);
}

void MainWindow::volumeIndicator9_click()
{
    doPreset(9);
}

void MainWindow::volumeIndicator10_click()
{
    doPreset(10);
}

//////////////////////////////////////////////////////
//
//       V O L U M E    D O    P R E S E T
//
//////////////////////////////////////////////////////

void MainWindow::doPreset(int value)
{
    value *= 10;
    if(value !=0)
    {
        this->ui->volumeSpinBox->setValue(value);
        this->ui->volumeLabel->setText(QString::number(value));
        this->setVolumeIndicators(value);
    }
    else
    {
        this->ui->volumeSpinBox->setValue(1);
        this->ui->volumeLabel->setText(QString::number(1));
        this->setVolumeIndicators(1);
    }
    this->ui->muteBut->setChecked(true);
    this->ui->volumeSpinBox->setFocus();
}
