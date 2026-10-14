/* @file superUpgradeFirmwareViaSFTP.cpp
 * superUpgradeFirmwareViaSFTP
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Defines the information to upgrade scale software via SFTP
 */

#include <logmanager.h>

#include "superUpgradeFirmwareViaSFTP.h"
#include "ui_superUpgradeFirmwareViaSFTP.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <filesystem.h>
#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QProcess>

static const int STANDALONE_SCALE = 0;
static const int PRIMARY_SCALE = 1;
static const int SECONDARY_SCALE = 2;
/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

superUpgradeFirmwareViaSFTP::superUpgradeFirmwareViaSFTP(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superUpgradeFirmwareViaSFTP)
{
    // set this to false before final testing.
    prelimTesting = false;
    updateUIProgressMessages = true;
    doneADownload = false;
    clickedInstallUpdates = false;
    waitForEmptyInstallOnly = false;
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

    hadLostConnectionError = false;
    keepModal = false;

    ui->setupUi(this);
    //ui->infiniteProgressBar->hide();
    ui->downloadInstallAlert->hide();
    ui->frameUpdateNow->setGeometry(0,1000,991,532);
    ui->frameUpdateNow->hide();
    ui->butCloseUpdateNow->hide();
    watcher = new QFileSystemWatcher(this);
      connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(fileChanged(const QString &)));
      connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(directoryChanged(const QString &)));
      watcher->addPath("/usr/local/hobart/pending");
      watcher->addPath("/usr/local/hobart/download");

    processingAnUpgrade = false;

    loading = true;

    this->isIPSelected = false;
    this->isAMSelected = true;

    ui->gbHostname->show();
    ui->gbIP->setGeometry( ui->gbHostname->geometry() );
    ui->gbIP->hide();
    ui->frameFirmwareAlert->hide();
    ui->frameFirmwareAlertSendToDept->hide();

    session_->send(ConfigurationDev, CfgFTPRead); //112,170
    //session_->send(ConfigurationDev,CfgScaleUpgradeRead); // 112,120 - request email config to from ip or dns
    session_->send(SessionDev, SnVersionInfo); //100,2

    connect(ui->leHostname, SIGNAL(textChanged(const QString)), this, SLOT(preventSpacesInHostnames(const QString&)));
    connect(ui->butSwitchIPHostname, SIGNAL(clicked()) , this, SLOT(switchIPHostnameButClicked()));
    connect(ui->butAmPm, SIGNAL(clicked()), this, SLOT(switchAMPMButClicked()));
    //connect(ui->butDownloadUpdates, SIGNAL(clicked()), this, SLOT(upgradeNow()));
    connect(ui->butDownloadUpdates, SIGNAL(clicked()), this, SLOT(downloadNowButClicked()));
    connect(ui->checkBoxFirmware, SIGNAL(toggled(bool)),this, SLOT(manageFirmwareAlert(bool)));
    connect(ui->checkBoxFirmwareSendToDept, SIGNAL(toggled(bool)), this,SLOT(manageFirmwareSendToDeptAlert(bool)));
    connect(ui->frequencyUnitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(manageFrequencyControls()));
    connect(ui->viewUpgradesBut,SIGNAL(clicked()),this,SLOT(viewUpgradesButClicked()));
    connect(ui->closeUpgradeViewBut,SIGNAL(clicked()),this,SLOT(closeUpgradeViewButClicked()));
    connect(ui->butInstallUpdates,SIGNAL(clicked()),this,SLOT(installUpgradesNow()));
    connect(ui->butUpdateNow,SIGNAL(clicked()),this,SLOT(butUpdateNowClicked()));
    connect(ui->butConfirmNO, SIGNAL(clicked()),this,SLOT(noUpdateNowClicked()));
    connect(ui->butConfirmYES, SIGNAL(clicked()),this,SLOT(yesUpdateNowClicked()));
    connect(ui->closeUpgradeViewBut, SIGNAL(clicked()),this,SLOT(closeUpdateNowFrame()));
    connect(ui->butCloseUpdateNow, SIGNAL(clicked()),this,SLOT(closeUpdateNowFrame()));
    connect(ui->chkDaily, SIGNAL(toggled(bool)), this,SLOT(manageStartTimeBox(bool)));
    connect(ui->chkHourly, SIGNAL(toggled(bool)), this,SLOT(manageNoStartTimeBox(bool)));

    connect(ui->radioDownloadOnly, SIGNAL(clicked()), this, SLOT(manageUpdateNowDownloadOnly()));
    connect(ui->radioDownloadAndInstall, SIGNAL(clicked()), this, SLOT(manageUpdateNowDownloadAndInstall()));


    ui->leIP1->setValidator(ipFirstOctet_ = new QIntValidator(0, INT_MAX, this));
    ui->leIP2->setValidator(ipNonFirstOctet_ = new QIntValidator(0, INT_MAX, this));
    ui->leIP3->setValidator(ipNonFirstOctet_);
    ui->leIP4->setValidator(ipNonFirstOctet_);
    ui->leHour->setValidator(hoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leMinutes->setValidator(minutesValidator_ = new QIntValidator(0, INT_MAX, this));

    ui->leIP1->setDecimalPlaces(0);
    ui->leIP1->setInputWrapping(true);
    ipFirstOctet_->setTop(255);
    ipFirstOctet_->setBottom(1);

    ui->leIP2->setDecimalPlaces(0);
    ui->leIP2->setInputWrapping(true);
    ui->leIP3->setDecimalPlaces(0);
    ui->leIP3->setInputWrapping(true);
    ui->leIP4->setDecimalPlaces(0);
    ui->leIP4->setInputWrapping(true);

    ipNonFirstOctet_->setTop(255);
    ipNonFirstOctet_->setBottom(0);

    ui->leHour->setInputWrapping(true);
    ui->leHour->setInputLeadingZeros(true);
    hoursValidator_->setTop(12);
    hoursValidator_->setBottom(1);

    ui->leMinutes->setInputLeadingZeros(true);
    ui->leMinutes->setInputWrapping(true);
    minutesValidator_->setTop(59);
    minutesValidator_->setBottom(0);

    ui->TabWidget->setCurrentIndex(0);
    ui->leMinutes->hide();
    ui->leHour->hide();
    //ui->label_11->hide();
    ui->butAmPm->hide();
    ui->frequencyUnitsComboBox->clear();
    ui->frequencyUnitsComboBox->addItem(tr("minutes"),QString("minutes")); // index 0
    ui->frequencyUnitsComboBox->addItem(tr("hours"), QString("hours"));    // index 1
    ui->frequencyUnitsComboBox->addItem(tr("days"), QString("days"));      // index 2

    manageInstallUpdatesButton();
    ui->viewUpgradesFrame->hide();
    ui->viewUpgradesFrame->setGeometry(70,40,861,381);
    ui->pBarDataXfer->setEnabled(false);
    ui->pBarDataXfer->setVisible(false);
    ui->frequencyQtySpinbox->setVisible(false);
    ui->frequencyUnitsComboBox->setVisible(false);
    ui->butAmPm->setVisible(false);
    ui->leHour->setVisible(false);
    ui->leMinutes->setVisible(false);
    ui->gbSearchTime->setVisible(false);
    ui->gbAutoSearch->setVisible(false);
    ui->gbAutoSearch->hide();
    ui->lblDataXfer->hide();
    //ui->TabWidget->removeTab(3);

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        ui->TabWidget->removeTab( 5 );  // Auto Backup
        ui->TabWidget->removeTab( 4 );  // Operator Access

        // Update Data tab
        // ---------------
        ui->checkBoxPLUData->hide();
        ui->checkBoxLabelData->hide();
        ui->checkBoxStoreNameNumber->hide();
        ui->checkBoxFlashkeyData->hide();
        ui->checkBoxVideos->hide();
        ui->checkBoxOperatorData->hide();
        ui->checkBoxImages->hide();
        ui->checkBoxPlaylists->hide();
        ui->checkBoxAudio->hide();

    }

    if(scaleConfig->AutomaticUpdateScaleType() == "0")
    {
        ui->TabWidget->removeTab(3);
        ui->chkAllowOperatorSendToDept->setDisabled(true);
        ui->chkAllowOperatorSendToDept->setChecked(false);
        ui->labelDisabledSendToDeptConfigAlert->setText(tr("The configuration above is disabled on a Stand-Alone scale."));
        ui->labelDisabledSendToDeptConfigAlert->show();
    }
    else if(scaleConfig->AutomaticUpdateScaleType() == "2")
    {
        ui->TabWidget->removeTab(3);
        ui->chkAllowOperatorSendToDept->setDisabled(true);
        ui->chkAllowOperatorSendToDept->setChecked(false);
        ui->labelDisabledSendToDeptConfigAlert->setText(tr("The configuration above is disabled on a Secondary scale."));
        ui->labelDisabledSendToDeptConfigAlert->show();
    }
    else if(scaleConfig->AutomaticUpdateScaleType() == "1")
    {
        ui->chkAllowOperatorSendToDept->setEnabled(true);
        ui->labelDisabledSendToDeptConfigAlert->hide();
    }

    ui->checkBoxFirmwareSendToDept->setChecked(false);
    ui->groupBoxSendToDeptFirmware->hide();

    // HT-360
    ui->cboAutoBackupFrequency->addItem( tr( "None" ), "0" );
    ui->cboAutoBackupFrequency->addItem( tr( "Every 15 minutes" ), "96" );
    ui->cboAutoBackupFrequency->addItem( tr( "Every hour and half-hour" ), "48" );
    ui->cboAutoBackupFrequency->addItem( tr( "Every hour (on the hour)" ), "24" );
    ui->cboAutoBackupFrequency->addItem( tr( "Twice per day (noon and midnight)" ), "2" );
    ui->cboAutoBackupFrequency->addItem( tr( "Once at midnight" ), "1" );
    // At this time, it was decided not to display the following options:
    //ui->cboAutoBackupFrequency->addItem( tr( "Every minute" ), "1440" );
    //ui->cboAutoBackupFrequency->addItem( tr( "Every 5 minutes" ), "288" );
    //ui->cboAutoBackupFrequency->addItem( tr( "Every 10 minutes" ), "144" );

    onlyInstallingFirmware = false;

    scaleScreen();
    expandTabs(ui->TabWidget);
    qDebug() << ui->TabWidget->styleSheet();

    translate();
}

void superUpgradeFirmwareViaSFTP::manageStartTimeBox(bool isChecked)
{
    ui->leStartTime->setEnabled(isChecked);
}

void superUpgradeFirmwareViaSFTP::manageNoStartTimeBox(bool isChecked)
{
    ui->leStartTime->setEnabled(!isChecked);
}

void superUpgradeFirmwareViaSFTP::manageUpdateNowDownloadOnly()
{
    if(ui->radioDownloadOnly->isChecked())
    {
        ui->radioDownloadAndInstall->setChecked(false);
    }
}

void superUpgradeFirmwareViaSFTP::manageUpdateNowDownloadAndInstall()
{
    if(ui->radioDownloadAndInstall->isChecked())
    {
        ui->radioDownloadOnly->setChecked(false);
    }

}


void superUpgradeFirmwareViaSFTP::toggleUseAutomatic(bool isChecked)
{
   if(isChecked)
   {
       ui->chkHourly->setEnabled(true);
       ui->radioDownloadAndInstall->setEnabled(true);
       ui->radioDownloadOnly->setEnabled(true);
       ui->chkMonday->setEnabled(true);
       ui->chkTuesday->setEnabled(true);
       ui->chkWednesday->setEnabled(true);
       ui->chkThursday->setEnabled(true);
       ui->chkFriday->setEnabled(true);
       ui->chkSaturday->setEnabled(true);
       ui->chkSunday->setEnabled(true);
       ui->leStartTime->setEnabled(true);
       ui->gbStartTime->setEnabled(true);
       ui->gbFirmware->setEnabled(true);
   }
   else
   {
       ui->chkHourly->setEnabled(false);
       ui->radioDownloadAndInstall->setEnabled(false);
       ui->radioDownloadOnly->setEnabled(false);
       ui->chkMonday->setEnabled(false);
       ui->chkTuesday->setEnabled(false);
       ui->chkWednesday->setEnabled(false);
       ui->chkThursday->setEnabled(false);
       ui->chkFriday->setEnabled(false);
       ui->chkSaturday->setEnabled(false);
       ui->chkSunday->setEnabled(false);
       ui->leStartTime->setEnabled(false);
       ui->gbStartTime->setEnabled(false);
       ui->gbFirmware->setEnabled(false);
   }
}

void superUpgradeFirmwareViaSFTP::viewUpgradesButClicked()
{
    QString downloadHoldingDirName = "";
    downloadHoldingDirName = filesystem::folder(folderType::DownloadHolding, false);

    QDir downloadHoldingDirectory;
    downloadHoldingDirectory.setFilter(QDir::Files | QDir::NoSymLinks);
    downloadHoldingDirectory.setSorting(QDir::Size | QDir::Reversed);
    downloadHoldingDirectory.setCurrent(downloadHoldingDirName);

    QStringList downloadHoldinglist = downloadHoldingDirectory.entryList();


    QString upgradeDirName = "";
    upgradeDirName = filesystem::folder(folderType::Upgrades, false);

    QDir upgradeDirectory;
    upgradeDirectory.setFilter(QDir::Files | QDir::NoSymLinks);
    upgradeDirectory.setSorting(QDir::Size | QDir::Reversed);
    upgradeDirectory.setCurrent(upgradeDirName);

    QStringList list = upgradeDirectory.entryList();

    ui->upgradeViewTextEdit->clear();
    for(int i = 0; i < list.count(); i++)
    {
        ui->upgradeViewTextEdit->insertPlainText(list.at(i) + "\n");
    }
    for(int i = 0; i < downloadHoldinglist.count(); i++)
    {
        ui->upgradeViewTextEdit->insertPlainText(downloadHoldinglist.at(i) + "\n");
    }

    ui->viewUpgradesFrame->show();

}

void superUpgradeFirmwareViaSFTP::closeUpgradeViewButClicked()
{
    ui->viewUpgradesFrame->hide();
}

superUpgradeFirmwareViaSFTP::~superUpgradeFirmwareViaSFTP()
{
    delete ui;
}

void superUpgradeFirmwareViaSFTP::updateNowComplete()
{
    ;
}

void superUpgradeFirmwareViaSFTP::closeUpdateNowFrame()
{
    ui->lblDataXfer->setText("Donwloading updates...please wait...");
    ui->frameUpdateNow->hide();
    updateUIProgressMessages = true;
    keepModal = false;
}

void superUpgradeFirmwareViaSFTP::preventSpacesInHostnames(const QString&)
{
    ui->leHostname->setText(ui->leHostname->text().replace(" ",""));
}

void superUpgradeFirmwareViaSFTP::switchAMPMButClicked()
{
    if(isAMSelected)
    {
        ui->butAmPm->setText("PM");
        isAMSelected = false;
    }
    else
    {
        ui->butAmPm->setText("AM");
        isAMSelected = true;
    }
}

void superUpgradeFirmwareViaSFTP::switchIPHostnameButClicked()
{
    if(ui->gbIP->isVisible())
    {
        ui->gbIP->hide();
        ui->gbHostname->show();
        ui->butSwitchIPHostname->setText(tr("Switch to IP address"));
        isIPSelected = false;
    }
    else
    {
        ui->gbHostname->hide();
        ui->gbIP->show();
        ui->butSwitchIPHostname->setText(tr("Switch to Host name"));
        isIPSelected = true;
    }
}

QString superUpgradeFirmwareViaSFTP::generateScheduleMessagePart()
{
    QString msg = "";

    //if(this->ui->chkDownloadFirmwareOnly->isChecked()) msg += ",1"; // add to msg
    //else msg += ",0";

    if(this->ui->radioDownloadOnly->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkMonday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkTuesday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkWednesday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkThursday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkFriday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkSaturday->isChecked())
        msg += ",1";  // add to msg
    else
        msg += ",0";

    if(this->ui->chkSunday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //if(this->ui->chkDownloadFirmwareOnly->isChecked()) msg += ",1"; // add to msg
    // else msg += ",0";

    if(this->ui->radioDownloadOnly->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    // if(this->ui->chkDownloadInstallFirmware->isChecked()) msg += ",1"; // add to msg
    // else msg += ",0";

    if(this->ui->radioDownloadAndInstall->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkHourly->isChecked())
        msg += ",1"; //add to msg
    else
        msg += ",0";

    msg += ",0"; // Every 15 minutes has been removed ...

    return msg;
}

void superUpgradeFirmwareViaSFTP::butUpdateNowClicked()
{
    // HT-361: Check if Automated DB Backup is turned on
    // ... Need to check what the value currently is on the screen, not the saved value
    QString backupFrequency = ui->cboAutoBackupFrequency->itemData( ui->cboAutoBackupFrequency->currentIndex() ).toString();
    if( !( mainWindow_->checkForAutomaticBackup( backupFrequency ) ) )
        return;

    ui->butCloseUpdateNow->hide();
    ui->butCloseUpdateNow->setEnabled(false);
    ui->frameUpdateNow->setGeometry(0,0,scaleWidthForScreen(991), scaleHeightForScreen(532));
    ui->frameUpdateNow->show();
    ui->frameUpdateNowConfirmation->show();
}

void superUpgradeFirmwareViaSFTP::noUpdateNowClicked()
{
    keepModal = false;
    ui->frameUpdateNow->hide();
}

void superUpgradeFirmwareViaSFTP::yesUpdateNowClicked()
{
    // scheduling updatenow but clicked
    buttonHit = "UpdateNow";
    keepModal = true;
    onlyInstallingFirmware = false;
    updateUIProgressMessages = true;
    ui->frameUpdateNowConfirmation->hide();
    // build the message and send the message
    //tbdb


    ui->pBarDataXfer->show();
    ui->lblDataXfer->setText(tr("Downloading updates ..."));
    ui->lblDataXfer->show();
    ui->lblUpdateNowStatus->setText("This scale is currently updating.\nPlease wait...");

    if(!ui->checkBoxAlerts->isChecked() && !ui->checkBoxAudio->isChecked() && !ui->checkBoxFirmware->isChecked() &&
       !ui->checkBoxFlashkeyData->isChecked() && !ui->checkBoxImages->isChecked() &&
       !ui->checkBoxLabelData->isChecked() && !ui->checkBoxOperatorData->isChecked() &&
       !ui->checkBoxPlaylists->isChecked() && !ui->checkBoxPLUData->isChecked() &&
       !ui->checkBoxScaleConfiguration->isChecked() && !ui->checkBoxStoreNameNumber->isChecked() &&
       !ui->checkBoxSynchronizationConfiguration ->isChecked() && !ui->checkBoxVideos->isChecked())
    {
        // Well there is nothing selected to update
        ui->pBarDataXfer->hide();
        ui->lblUpdateNowStatus->setText("");
        ui->lblDataXfer->setText(tr("No data selected to update..."));
        ui->lblDataXfer->setStyleSheet("color: rgb(170, 0, 0);");
        ui->butCloseUpdateNow->setEnabled(true);
        ui->butCloseUpdateNow->show();
    }

    QString updateCmd = "updateNow|";

   if(ui->leHostname->isVisible() && ui->leHostname->text() != "")
   {
       updateCmd += ui->leHostname->text() + "|";
   }
   else
   {
     updateCmd += ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text() + "|";
   }

   updateCmd += ui->leDirectory->text() + "|";

    bool anyFlags = false;

    if(ui->checkBoxPLUData->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "plu_data";
        anyFlags = true;
    }
    if(ui->checkBoxLabelData->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "label_data";
        anyFlags = true;
    }
    if(ui->checkBoxFlashkeyData->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "flashkey_data";
        anyFlags = true;
    }
    if(ui->checkBoxOperatorData->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "operator_data";
        anyFlags = true;
    }
    if(ui->checkBoxPlaylists->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "playlists";
        anyFlags = true;
    }
    if(ui->checkBoxAlerts->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "alerts";
        anyFlags = true;
    }
    if(ui->checkBoxScaleConfiguration->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "configuration_data";
        anyFlags = true;
    }
    if(ui->checkBoxSynchronizationConfiguration->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "synchronization_data";
        anyFlags = true;
    }
    if(ui->checkBoxStoreNameNumber->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "store_data";
        anyFlags = true;
    }
    if(ui->checkBoxVideos->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "videos";
        anyFlags = true;
    }
    if(ui->checkBoxImages->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "images";
        anyFlags = true;
    }
    if(ui->checkBoxAudio->isChecked())
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "audio";
        anyFlags = true;

    }
    if(ui->checkBoxFirmware->isChecked() && (ui->radioDownloadOnly->isChecked() || ui->radioDownloadAndInstall->isChecked()))
    {
        if(anyFlags)
            updateCmd += ",";
        updateCmd += "upgrade";
        anyFlags = true;
    }

    updateCmd += "|";
    QString userTemp = "";
    QString userPass = "";

    if(ui->leUser->text() == "" && ui->lePassword->text() == "")
    {
        userTemp = "root";
        userPass = "ho1bart2";
    }
    else
    {
     userTemp = ui->leUser->text();
     userPass = ui->lePassword->text();
    }

    updateCmd += userTemp + "|" + userPass + "||";

    //check the Scheduling tab setting also
    if(ui->checkBoxFirmware->isChecked() && ui->radioDownloadAndInstall->isChecked())
    {
        updateCmd += "installFirmwarePlease";
    }

    updateCmd += "|1\n";

    if(!anyFlags)
    {
        ui->pBarDataXfer->hide();
        ui->butCloseUpdateNow->setEnabled(true);
        ui->butCloseUpdateNow->show();
        ui->lblDataXfer->setText("");
        ui->lblUpdateNowStatus->setText(tr("No data selected to update."));
    }
    else
    {
        session_->send(VersionManagerDev, UpdateNow2016, "\"" + updateCmd + "\"\n");
    }
}

void superUpgradeFirmwareViaSFTP::manageFirmwareAlert(bool isChecked)
{
    if(isChecked)
    {
        ui->frameFirmwareAlert->show();
    }
    else
    {
        ui->frameFirmwareAlert->hide();
    }
}

void superUpgradeFirmwareViaSFTP::manageFirmwareSendToDeptAlert(bool isChecked)
{
    if(isChecked)
    {
        ui->frameFirmwareAlertSendToDept->show();
    }
    else
    {
        ui->frameFirmwareAlertSendToDept->hide();
    }
}

void superUpgradeFirmwareViaSFTP::manageFrequencyControls()
{
    // frequency must be in the range  5minutes <= freq <= 7 days
    if(ui->frequencyUnitsComboBox->currentIndex() == 2)
    {
        // days
        ui->frequencyQtySpinbox->setMaximum(7);
        ui->frequencyQtySpinbox->setMinimum(1);
        if(ui->frequencyQtySpinbox->value() > 7)
        {
            // greater than a week so reduce it to a week max
            ui->frequencyQtySpinbox->setValue(7);
        }
        ui->frequencyQtySpinbox->setMaximum(7);
        ui->frequencyQtySpinbox->setMinimum(1);
    }
    else if (ui->frequencyUnitsComboBox->currentIndex() == 1)
    {
        // hours
        ui->frequencyQtySpinbox->setMaximum(168);
        ui->frequencyQtySpinbox->setMinimum(1);
        if(ui->frequencyQtySpinbox->value() > 168)
        {
            // greater than a week so reduce it to a week max
            ui->frequencyQtySpinbox->setValue(168);
        }
        ui->frequencyQtySpinbox->setMaximum(168);
        ui->frequencyQtySpinbox->setMinimum(1);
    }
    else if(ui->frequencyUnitsComboBox->currentIndex() == 0)
    {
        // minutes
        ui->frequencyQtySpinbox->setMinimum(5);
        ui->frequencyQtySpinbox->setMaximum(10080);
        if( ui->frequencyQtySpinbox->value() > 10080)
        {
            // greater than a week so reduce it to a week max
            ui->frequencyQtySpinbox->setValue(10080);
        }
        else if (ui->frequencyQtySpinbox->value() < 5)
        {
            // must be at least 5 minutes
            ui->frequencyQtySpinbox->setValue(5);
        }
        ui->frequencyQtySpinbox->setMinimum(5);
        ui->frequencyQtySpinbox->setMaximum(10080);
    }
}

void superUpgradeFirmwareViaSFTP::manageInstallUpdatesButton()
{
    QString upgradeDirName = "";
    upgradeDirName = filesystem::folder(folderType::Upgrades, false);

    QDir upgradeDirectory;
    upgradeDirectory.setFilter(QDir::Files | QDir::NoSymLinks);
    upgradeDirectory.setSorting(QDir::Size | QDir::Reversed);
    upgradeDirectory.setCurrent(upgradeDirName);
    QStringList upgradeList = upgradeDirectory.entryList();

    QString downloadHoldingDirName = "";
    downloadHoldingDirName = filesystem::folder(folderType::DownloadHolding, false);

    QDir downloadHoldingDirectory;
    downloadHoldingDirectory.setFilter(QDir::Files | QDir::NoSymLinks);
    downloadHoldingDirectory.setSorting(QDir::Size | QDir::Reversed);
    downloadHoldingDirectory.setCurrent(downloadHoldingDirName);
    QStringList downloadHoldingList = downloadHoldingDirectory.entryList();

    if(upgradeList.size() > 0 || downloadHoldingList.size() > 0)
    {
        ui->butInstallUpdates->setEnabled(true);
        ui->butInstallUpdates->setText(tr("Install Updates"));
    }
    else
    {
        ui->butInstallUpdates->setDisabled(true);
        if(buttonHit == "InstallOnly")
        {
            //----------------
            // Update Data tab
            //----------------

            ui->butInstallUpdates->setText(tr("No updates to install"));
        }
    }
}

void superUpgradeFirmwareViaSFTP::upgradeNow()
{

    // this is really download files
    processingAnUpgrade = true;
    QString msg;
    QString quotedString;

    quotedString = "\"";

    if(isIPSelected)
    {
        msg = quotedString + ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text() + quotedString + ",";
    }
    else
    {
        msg = quotedString + ui->leHostname->text() + quotedString + ",";
    }

    msg += quotedString + ui->leUser->text() + quotedString + "," + quotedString + ui->lePassword->text() + quotedString + "," + quotedString + ui->leDirectory->text() + quotedString;
}

void superUpgradeFirmwareViaSFTP::downloadNowButClicked()
{
  LOG_CLASS_INFO( "superUpgradeFirmwareViaSFTP", "downloadNowButClicked" );
  /*
    processingAnUpgrade = false;
    QString word = "testing";
    QString command;
    LOG_DEBUG("before prepareDownloadRoutineCallString() call %s", qPrintable(word));

    //command = prepareDownloadRoutineCallString();
    LOG_DEBUG("after prepareDownloadRoutineCallString() call %s", qPrintable(word));
    // Use the command
    session_->send(VersionManagerDev, 0 , command);
    LOG_DEBUG("after line 339 call %s", qPrintable(word));
*/

    buttonHit = "DownloadOnly";
    if(!ui->checkBoxAlerts->isChecked() && !ui->checkBoxAudio->isChecked() && !ui->checkBoxFirmware->isChecked()
            && !ui->checkBoxFlashkeyData->isChecked() && !ui->checkBoxImages->isChecked() &&
            !ui->checkBoxLabelData->isChecked() && !ui->checkBoxOperatorData->isChecked() &&
            !ui->checkBoxPlaylists->isChecked() && !ui->checkBoxPLUData->isChecked() &&
            !ui->checkBoxScaleConfiguration->isChecked() && !ui->checkBoxStoreNameNumber->isChecked() &&
            !ui->checkBoxSynchronizationConfiguration ->isChecked() && !ui->checkBoxVideos->isChecked())
    {
        // Well there is nothing selected to update
        ui->downloadInstallAlert->setText(tr("No data selected to update..."));
        LOG_DEBUG( "downloadInstallAlert->setText-1", NULL );
        ui->downloadInstallAlert->show();
    }
    else
    {
        QString downloadHoldingDirName = "";
        downloadHoldingDirName = filesystem::folder(folderType::DownloadHolding, false);
        QDir downloadHoldingDirectory(downloadHoldingDirName);
        bool answer;
        if(!downloadHoldingDirectory.exists())
        {
            answer = downloadHoldingDirectory.mkpath(".");
        }

        QString updateCmd = "updateNow|";

           if(ui->leHostname->isVisible() && ui->leHostname->text() != "")
           {
               updateCmd += ui->leHostname->text() + "|";
           }
           else
           {
             updateCmd += ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text() + "|";
           }

           updateCmd += ui->leDirectory->text() + "|";

            bool anyFlags = false;

            if(ui->checkBoxPLUData->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "plu_data";
                anyFlags = true;
            }
            if(ui->checkBoxLabelData->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "label_data";
                anyFlags = true;
            }
            if(ui->checkBoxFlashkeyData->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "flashkey_data";
                anyFlags = true;

            }
            if(ui->checkBoxOperatorData->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "operator_data";
                anyFlags = true;

            }
            if(ui->checkBoxScaleConfiguration->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "configuration_data";
                anyFlags = true;

            }
            if(ui->checkBoxSynchronizationConfiguration->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "synchronization_data";
                anyFlags = true;

            }
            if(ui->checkBoxStoreNameNumber->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "store_data";
                anyFlags = true;

            }
            if(ui->checkBoxPlaylists->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "playlists";
                anyFlags = true;

            }
            if(ui->checkBoxAlerts->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "alerts";
                anyFlags = true;

            }
            if(ui->checkBoxVideos->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "videos";
                anyFlags = true;

            }
            if(ui->checkBoxImages->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "images";
                anyFlags = true;

            }
            if(ui->checkBoxAudio->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "audio";
                anyFlags = true;
            }
            if(ui->checkBoxFirmware->isChecked())
            {
                if(anyFlags)
                    updateCmd += ",";
                updateCmd += "upgrade";
                anyFlags = true;
            }

            updateCmd += "|";
            QString userTemp = "";
            QString userPass = "";

            if(ui->leUser->text() == "" && ui->lePassword->text() == "")
            {
                userTemp = "root";
                userPass = "ho1bart2";
            }
            else
            {
             userTemp = ui->leUser->text();
             userPass = ui->lePassword->text();
            }

            updateCmd += userTemp + "|" + userPass + "|downloadHolding||1";
            if(!anyFlags)
                return;
            else
            {
                //loop through secondaries
                session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"" + updateCmd + "\"\n");
            }
            ui->downloadInstallAlert->setText(tr("This HTSeries device is downloading updates..."));
            LOG_DEBUG( "downloadInstallAlert->setText-2", NULL );
            ui->downloadInstallAlert->show();
            manageInstallUpdatesButton();

    }
}

void superUpgradeFirmwareViaSFTP::installUpgradesNow()
{
    LOG_CLASS_INFO( "superUpgradeFirmwareViaSFTP", "installUpgradesNow" );

    onlyInstallingFirmware = false;

    buttonHit = "InstallOnly";
    processingAnUpgrade = true;
    keepModal = true;

    doneADownload = false;
    clickedInstallUpdates = true;

    //---------------------------------------------------------------------------------
    // SOF-2779: Need to check if there are any files in download or pending directory.
    //           The logic in directoryChanged() to kick off the install will only be
    //           Executed if the contents of the download or pending directories
    //           changes.
    //---------------------------------------------------------------------------------
    QString downloadPath = "/usr/local/hobart/download";
    QString pendingPath = "/usr/local/hobart/pending";

    QDir dir;
    dir.setPath( downloadPath );
    dir.setFilter( QDir::AllDirs | QDir::NoDotAndDotDot );
    int downloadDirCount = dir.count();

    dir.setPath( pendingPath );
    dir.setFilter( QDir::AllDirs | QDir::NoDotAndDotDot );
    int pendingDirCount = dir.count();


    ui->downloadInstallAlert->setText(tr("This HTSeries device is installing updates..."));
    LOG_DEBUG( "downloadInstallAlert->setText-3", NULL );
    ui->downloadInstallAlert->show();

    //////////////////
    // just cp downloadHolding files to download dir

    QString downloadHoldingDirName = "";
    downloadHoldingDirName = filesystem::folder(folderType::DownloadHolding, false);
    QDir downloadHoldingDirectory;

    if(!downloadHoldingDirectory.exists())
    {
        QDir().mkdir(downloadHoldingDirName);
    }

    QString downloadDirName = "";
    downloadDirName = filesystem::folder(folderType::Download, false);
    QDir downloadDirectory;

    downloadHoldingDirectory.setFilter(QDir::Files | QDir::NoSymLinks);
    downloadHoldingDirectory.setSorting(QDir::Size | QDir::Reversed);
    downloadHoldingDirectory.setCurrent(downloadHoldingDirName);

    QStringList downloadHoldingList = downloadHoldingDirectory.entryList();

    downloadHoldingDirectory.rename(downloadHoldingDirectory.path(),downloadDirectory.path());

    for(int i = 0; i < downloadHoldingList.size(); i++)
    {
        // rename file dir
        downloadHoldingDirectory.setCurrent(downloadHoldingDirName);
        QFileInfo fInfo = downloadHoldingList.at(i);
        QString fPath = fInfo.absoluteFilePath();
        QFile file (fPath);

       // QFileInfo fileInfo(f.fileName());
        QString ORIGFilename(fInfo.fileName());

        QString fPath_ren;

        downloadDirectory.setCurrent(downloadDirName);
        fPath_ren = "/usr/local/hobart/download/" + ORIGFilename;
        ui->downloadInstallAlert->setText("ORIG:" + fPath + "  NEW:" + fPath_ren);
        LOG_DEBUG( "downloadInstallAlert->setText-4", NULL );
        downloadDirectory.rename(fPath, fPath_ren);
    }

    LOG_DEBUG( "installUpgradesNow(): downloadHoldingList.size()=%d", downloadHoldingList.size() );
    LOG_DEBUG( "installUpgradesNow(): downloadDirCount=%d", downloadDirCount );
    LOG_DEBUG( "installUpgradesNow(): pendingDirCount=%d", pendingDirCount );

    //------------------------------------------------------------------
    // SOF-2779: If there was nothing to be processed by chobart_d, then
    //           go ahead and issue the message to install any files
    //           in the upgrade directory.
    //
    //           The only items to install are firmware.
    //------------------------------------------------------------------
    if( downloadHoldingList.size() == 0 &&
        downloadDirCount == 0 &&
        pendingDirCount == 0 )
    {
        onlyInstallingFirmware = true;

        ui->downloadInstallAlert->setText(tr("Checking firmware..."));
        LOG_DEBUG( "downloadInstallAlert->setText-15", NULL );

        QString cmd = "";
        cmd = "installUpgrades|1";
        session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"" + cmd + "\"\n");
    }

    waitForEmptyInstallOnly = true;
}

void superUpgradeFirmwareViaSFTP::saveAllConfigsForUpdateNow()
{
    QString orderedMsg = "";
    QString quotedString = "\"";
    QString msg = "";

    //TAB : Location/Login
    if(isIPSelected)
    {
    //if(testing) orderedMsg += "IPorDomain,";
        msg = quotedString + ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text() + quotedString + ",";
    }
    else
    {
    //if(testing) orderedMsg += "IPorDomain,";
        msg = quotedString + ui->leHostname->text() + quotedString + ",";
    }
    //if(testing) orderedMsg += "username,password,directory,";

    msg += quotedString + ui->leUser->text() + quotedString + "," + quotedString + ui->lePassword->text() + quotedString + "," + quotedString + ui->leDirectory->text() + quotedString;

    //TAB : Scheduling
    //if(testing) orderedMsg += "ScheduleStartTime,";

    msg += "," + ui->leStartTime->text();

    // if(testing) orderedMsg += "UseAutomatic,";

    if(this->ui->cbAutoSearch->isChecked())
        msg += ",1";
    else
        msg += ",0";

    // if(testing) orderedMsg += "Monday,";

    if(this->ui->chkMonday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //   if(testing) orderedMsg += "Tuesday,";
    if(this->ui->chkTuesday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //  if(testing) orderedMsg += "Wednesday,";

    if(this->ui->chkWednesday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //  if(testing) orderedMsg += "Thursday,";

    if(this->ui->chkThursday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //  if(testing) orderedMsg += "Friday,";

    if(this->ui->chkFriday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    //   if(testing) orderedMsg += "Saturday,";

    if(this->ui->chkSaturday->isChecked())
        msg += ",1";  // add to msg
    else
        msg += ",0";

    //  if(testing) orderedMsg += "Sunday,";

    if(this->ui->chkSunday->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->radioDownloadOnly->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->radioDownloadAndInstall->isChecked())
        msg += ",1"; // add to msg
    else
        msg += ",0";

    if(this->ui->chkDaily->isChecked())
        msg += ",1";
    else
        msg += ",0";

    // if(testing) orderedMsg += "hourlyUpdates,";

    if(this->ui->chkHourly->isChecked())
        msg += ",1"; //add to msg
    else
        msg += ",0";

    // if(testing) orderedMsg += "15MinuteUpdates,";

    msg += ",0";  // Every 15 minutes has been removed ...

    // TAB : Update Now
    // if(testing) orderedMsg += "Update:PLUData,";

    if(ui->checkBoxPLUData->isChecked())
    {
        msg += ",1,";
    }
    else
    {
        msg += ",0,";
    }

    //if(testing) orderedMsg += "Update:LabelData,";

    if(ui->checkBoxLabelData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:FlashkeyData,";

    if(ui->checkBoxFlashkeyData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:OperatorData,";

    if(ui->checkBoxOperatorData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:Playlists,";

    if(ui->checkBoxPlaylists->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:Alerts,";

    if(ui->checkBoxAlerts->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:ScaleConfiguration,";

    if(ui->checkBoxScaleConfiguration->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //  if(testing) orderedMsg += "Update:SyncronizationConfiguration,";

    if(ui->checkBoxSynchronizationConfiguration->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:StoreNameNumber,";

    if(ui->checkBoxStoreNameNumber->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:Videos,";

    if(ui->checkBoxVideos->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //   if(testing) orderedMsg += "Update:Images,";

    if(ui->checkBoxImages->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:Audio,";

    if(ui->checkBoxAudio->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Update:Firmware,";

    if(ui->checkBoxFirmware->isChecked())
    {
        msg += "1";
    }
    else
    {
        msg += "0";
    }

    // TAB : Send to Department
    //if(testing) orderedMsg += "Send:PLUData,";

    if(ui->checkBoxPLUDataSendToDept->isChecked())
    {
        msg += ",1,";
    }
    else
    {
        msg += ",0,";
    }

    //if(testing) orderedMsg += "Send:LabelData,";

    if(ui->checkBoxLabelDataSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:FlashkeyData,";

    if(ui->checkBoxFlashkeyDataSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:OperatorData,";

    if(ui->checkBoxOperatorDataSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Playlists,";

    if(ui->checkBoxPlaylistsSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Alerts,";

    if(ui->checkBoxAlertsSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:ScaleConfiguration,";

    if(ui->checkBoxScaleConfigurationSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Synchronization,";

    if(ui->checkBoxSynchronizationConfigurationSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:StoreNameNumber,";

    if(ui->checkBoxStoreNameNumberSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Videos,";

    if(ui->checkBoxVideosSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Images,";

    if(ui->checkBoxImagesSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Audio,";

    if(ui->checkBoxAudioSendToDept->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }

    //if(testing) orderedMsg += "Send:Firmware,";

    msg += "0";

    /*    if(ui->checkBoxFirmwareSendToDept->isChecked())
    {
        msg += "1";
    }
    else
    {
        msg += "0";
    }
    */

    // TAB : Operator Access
    //if(testing) orderedMsg += "AllowOperatorSendToDept,";

    if(ui->chkAllowOperatorSendToDept->isChecked())
        msg += ",1";
    else
        msg += ",0";

    //if(testing) orderedMsg += "AllowOperatorUpdateNow -------,";

    if(ui->chkAllowOperatorUpdateNow->isChecked())
        msg += ",1";
    else
        msg += ",0";

    // HT-360
    msg += "," + ( ui->cboAutoBackupFrequency->itemData( ui->cboAutoBackupFrequency->currentIndex() ) ).toString();

    session_->send(ConfigurationDev, CfgFTPSave, msg); // 112,172,
}

QString superUpgradeFirmwareViaSFTP::generateUserConfigMessagePart()
{
    // Generates an ordered, comma delimited string for the backend

    // FORMAT: PLU Data,Label Data,Flashkey Data,Operator Data,Playlists,
    //         Alerts,ScaleConfiguration,Synchronization Configuration,
    //         Store Name/Number,Videos,Images,Audio,Firmware

    QString msg;
    msg = "";

    // PLU Data
    if(ui->checkBoxPLUData->isChecked())
    {
        msg += ",1,";
    }
    else
    {
        msg += ",0,";
    }
    // Label Data
    if(ui->checkBoxLabelData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Flashkey Data
    if(ui->checkBoxFlashkeyData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Operator Data
    if(ui->checkBoxOperatorData->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Playlists
    if(ui->checkBoxPlaylists->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Alerts
    if(ui->checkBoxAlerts->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Scale Configuration
    if(ui->checkBoxScaleConfiguration->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Synchronization Configuration
    if(ui->checkBoxSynchronizationConfiguration->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Store Name Number
    if(ui->checkBoxStoreNameNumber->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Videos
    if(ui->checkBoxVideos->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Images
    if(ui->checkBoxImages->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Audio
    if(ui->checkBoxAudio->isChecked())
    {
        msg += "1,";
    }
    else
    {
        msg += "0,";
    }
    // Firmware
    if(ui->checkBoxFirmware->isChecked())
    {
        msg += "1";
    }
    else
    {
        msg += "0";
    }

    return msg;
}

void superUpgradeFirmwareViaSFTP::manageDownloadUpdatesButton()
{
    bool oneIsChecked;
    oneIsChecked = false;
    if(!ui->checkBoxAlerts->isChecked() && !ui->checkBoxAudio->isChecked() && !ui->checkBoxFirmware->isChecked()
            && !ui->checkBoxFlashkeyData->isChecked() && !ui->checkBoxImages->isChecked() && !ui->checkBoxLabelData->isChecked()
            && !ui->checkBoxOperatorData->isChecked() && !ui->checkBoxPlaylists->isChecked() && !ui->checkBoxPLUData->isChecked()
            && !ui->checkBoxScaleConfiguration->isChecked() && !ui->checkBoxStoreNameNumber->isChecked()
            && !ui->checkBoxSynchronizationConfiguration->isChecked() && !ui->checkBoxVideos->isChecked())
    {
        oneIsChecked = false;
    }
    else
    {
        oneIsChecked = true;
    }

    if(oneIsChecked)
        ui->butDownloadUpdates->setEnabled(true);
    else
        ui->butDownloadUpdates->setEnabled(false);
}

QString superUpgradeFirmwareViaSFTP::prepareDownloadRoutineCallString()
{
    QString msg;
    // string should be: updateNow|IP address or hostname|remote directory|config flags|id

    // Add the command name
    msg = "updateNow|";

    // Add the IP/Hostname
    if(ui->gbHostname->isVisible())
    {
        // it is a hostname
        msg += ui->leHostname->text() + "|";
    }
    else if(ui->gbIP->isVisible())
    {
        // it is an IP address
        msg += ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text() + "|";
    }

    // Add the remote directory
    msg += ui->leDirectory->text() + "|";

    // add the configuration flags
    bool previousFlag;
    previousFlag = false;

    if(ui->checkBoxVideos->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "videos";
        previousFlag = true;
    }
    if(ui->checkBoxImages->isChecked())
    {
        if (previousFlag)
            msg += "-";
        msg += "images";
        previousFlag = true;
    }
    if(ui->checkBoxAudio->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "audio";
        previousFlag = true;
    }
    if(ui->checkBoxPlaylists->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "playlists";
        previousFlag = true;
    }
    if(ui->checkBoxPLUData->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "plu_data";
        previousFlag = true;
    }
    if(ui->checkBoxLabelData->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "label_data";
        previousFlag = true;
    }
    if(ui->checkBoxFlashkeyData->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "flashkey_data";
        previousFlag = true;
    }
    if(ui->checkBoxOperatorData->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "operator_data";
        previousFlag = true;
    }
    if(ui->checkBoxStoreNameNumber->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "store_data";
        previousFlag = true;
    }
    if(ui->checkBoxSynchronizationConfiguration->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "synchronization_data";
        previousFlag = true;
    }
    if(ui->checkBoxScaleConfiguration->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "scale_data";
        previousFlag = true;
    }

    if(ui->checkBoxFirmware->isChecked())
    {
        if(previousFlag)
            msg += "-";
        msg += "upgrade";
        previousFlag = true;
    }

    // Username
    msg += "|" + ui->leUser->text();

    // Password
    msg += "|" + ui->lePassword->text();

    // ID
    msg += "|1";

    return msg;
}

QString superUpgradeFirmwareViaSFTP::prepareInstallRoutineCallString()
{
    // Made this routine in case it ever changes or becomes more complex.
    // It all happens locally on machine so we don't need username or passwords
    QString msg;
    msg = "installUpgrades|1";
    return msg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superUpgradeFirmwareViaSFTP::translate()
{
    ui->retranslateUi(this);

    // HT-360
    ui->cboAutoBackupFrequency->setItemText( 0, tr( "None" ) );
    ui->cboAutoBackupFrequency->setItemText( 1, tr( "Every 15 minutes" ) );
    ui->cboAutoBackupFrequency->setItemText( 2, tr( "Every hour and half-hour" ) );
    ui->cboAutoBackupFrequency->setItemText( 3, tr( "Every hour (on the hour)" ) );
    ui->cboAutoBackupFrequency->setItemText( 4, tr( "Twice per day (noon and midnight)" ) );
    ui->cboAutoBackupFrequency->setItemText( 5, tr( "Once at midnight" ) );
    // At this time, it was decided not to display the following options:
    //ui->cboAutoBackupFrequency->setItemText( x, tr( "Every minute" ) );
    //ui->cboAutoBackupFrequency->setItemText( x,  tr( "Every 5 minutes" ) );
    //ui->cboAutoBackupFrequency->setItemText( x,  tr( "Every 10 minutes" ) );


    update();
}

void superUpgradeFirmwareViaSFTP::updateScaleSynchronizationType(const SessionMessage &msg)
{
    // update the scale synchronization as primary, secondary, or standalone. If we have a secondary
    // scale, the primary scale address or host name the secondary is pointing to is also updated.
    // Received a 112,181

    DomAutomaticUpdate m( msg );

    if ( automaticUpdate_ )
        *automaticUpdate_ = m;

    //ignoreSignals_ = true;

    localScaleType_ = automaticUpdate_->scaleType();
    QString localHostName = automaticUpdate_->hostName();

    if ( localScaleType_ == SECONDARY_SCALE )
        ui->SendToDeptTab->hide();
    else if( localScaleType_ == PRIMARY_SCALE )
        ui->SendToDeptTab->show();
    else  ui->SendToDeptTab->hide();                                  // should always be 0 here, but just in case set it standalone by default

    //ignoreSignals_  = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superUpgradeFirmwareViaSFTP::onScreenChanging()
{
    saveAllConfigsForUpdateNow();

    if (keepModal)
        return false;
    else
        return _Mybase::onScreenChanging();
}

void superUpgradeFirmwareViaSFTP::directoryChanged(const QString & path)
{
    LOG_CLASS_INFO( "superUpgradeFirmwareViaSFTP", "directoryChanged" );

    LOG_DEBUG("TOP of directoryChanged()", NULL );

    //---------------------------------------------------------------------------------
    // SOF-2779: If there is only firmware to install, because there was nothing in the
    //           downloadHolding, download or pending directories, then there is
    //           no need to react to the directory contents changing.
    //
    //           For some firmware upgrades, files are placed in the download
    //           directory.  Do not want to trigger extra processing or
    //           user messages.
    //---------------------------------------------------------------------------------
    if( onlyInstallingFirmware )
        return;

     Q_UNUSED( path );
     QString downloadPath = "/usr/local/hobart/download";
     QString pendingPath = "/usr/local/hobart/pending";

     QDir downloadDir( downloadPath );
     QDir pendingDir( pendingPath );
     pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int pendingTotalFiles = pendingDir.count();

     downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int downloadTotalFiles = downloadDir.count();

     if((downloadTotalFiles + pendingTotalFiles) > 0)
     {
         LOG_DEBUG("directoryChanged(): Total file > 0", NULL );

         ui->lblUpdateNowStatus->setText("Please Wait...");
         if(waitForEmptyInstallOnly)
         {
            ui->downloadInstallAlert->setText(tr("Installing Updates..."));
            LOG_DEBUG( "downloadInstallAlert->setText-5", NULL );
         }

         ui->butCloseUpdateNow->hide();
         if(updateUIProgressMessages)
         {
            ui->lblUpdateNowStatus->setText("Please Wait...");
            if(downloadTotalFiles+pendingTotalFiles == 1)
            {
                ui->lblUpdateNowStatus->setText("Please Wait...");
                if(doneADownload)
                    ui->lblDataXfer->setText("There is 1 update remaining to install...");
            }
            else
            {
                ui->lblUpdateNowStatus->setText("Please Wait...");
                if(doneADownload)
                    ui->lblDataXfer->setText("There are " + QString::number(downloadTotalFiles+pendingTotalFiles) + " updates remaining to install...");
            }
        }
     }
     else if(downloadTotalFiles + pendingTotalFiles == 0)
     {
         LOG_DEBUG("directoryChanged(): Total file == 0", NULL );

         if(waitForEmptyInstallOnly && buttonHit == "InstallOnly")
         {
             //----------------
             // Update Data tab
             //----------------

             ui->downloadInstallAlert->setText(tr("Checking firmware..."));
             LOG_DEBUG( "downloadInstallAlert->setText-6", NULL );

             waitForEmptyInstallOnly = false;
             QString cmd = "";
             cmd = "installUpgrades|1";
             ui->lblUpdateNowStatus->setText("Please Wait...");
             ui->lblDataXfer->setText(tr("Installing firmware..."));
             session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"" + cmd + "\"\n");
         }

         //dirs are cleaned out so tell the User
         if(doneADownload)
         {
             if(ui->checkBoxFirmware->isChecked())
             {

             }
             else
             {
                 ui->lblDataXfer->setText("");
                 ui->pBarDataXfer->hide();
                 ui->butCloseUpdateNow->setEnabled(true);
                 ui->butCloseUpdateNow->show();
                 ui->lblUpdateNowStatus->setText("Update is complete.");
             }
         }

         keepModal = false;
     }

     manageInstallUpdatesButton();
}

void superUpgradeFirmwareViaSFTP::fileChanged(const QString & path)
{
     watcher->addPath(path);

    //qDebug() << path;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superUpgradeFirmwareViaSFTP::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superUpgradeFirmwareViaSFTP", "onSessionMessage" );

    if(ConfigurationDev == msg[0].toInt() && CfgFTPWrite == msg[1].toInt()) //112,171
    {
        DomFtpConfig m(msg);

        ui->leUser->setText(m.userName());
        ui->lePassword->setText(m.password());
        ui->leDirectory->setText(m.subDirectory());
        ui->leStartTime->setText(m.scheduleStartTime());

      //  ui->cbAutoSearch->setText(m.useAutomaticUpdates());
        if(m.useAutomaticUpdates() == "1")
        {
            ui->cbAutoSearch->setChecked(true);
        }
        else
        {
            ui->cbAutoSearch->setChecked(false);
        }

        if(m.useAutomaticUpdates() == "1")
            toggleUseAutomatic(true);
        else
            toggleUseAutomatic(false);

        QStringList SFTPServer;

        SFTPServer = m.ftpServer().split(".", QString::KeepEmptyParts);

        isIPSelected = false;
        isIPSelected = false;

        if(SFTPServer.count() == 4)
        {
            // Could be ip
            bool ok;
            SFTPServer.at(0).toInt(&ok);
            if(ok)
            {
                if(SFTPServer.at(0).toInt() <256)
                {
                    SFTPServer.at(1).toInt(&ok);
                    if(ok)
                    {
                        if(SFTPServer.at(1).toInt() <256)
                        {
                            SFTPServer.at(2).toInt(&ok);
                            if(ok)
                            {
                                if(SFTPServer.at(2).toInt() <256)
                                {
                                    SFTPServer.at(3).toInt(&ok);
                                    if(ok)
                                    {
                                        if(SFTPServer.at(3).toInt() <256)
                                        {
                                           // must be IP
                                            ui->gbHostname->hide();
                                            ui->gbIP->show();
                                            ui->leIP1->setText(SFTPServer.at(0));
                                            ui->leIP2->setText(SFTPServer.at(1));
                                            ui->leIP3->setText(SFTPServer.at(2));
                                            ui->leIP4->setText(SFTPServer.at(3));
                                            ui->butSwitchIPHostname->setText(tr("Switch to Host name"));
                                            isIPSelected = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(!isIPSelected)
        {
            // must be hostname
            ui->gbIP->hide();
            ui->gbHostname->show();
            ui->leHostname->setText(msg[2]);
            ui->butSwitchIPHostname->setText(tr("Switch to IP address"));
            isIPSelected = false;
        }

        ui->chkMonday->setChecked(m.monday() == "1" ? true : false);
        ui->chkTuesday->setChecked(m.tuesday() == "1" ? true : false);
        ui->chkWednesday->setChecked(m.wednesday() == "1" ? true : false);
        ui->chkThursday->setChecked(m.thursday() == "1" ? true : false);
        ui->chkFriday->setChecked(m.friday() == "1" ? true : false);
        ui->chkSaturday->setChecked(m.saturday() == "1" ? true : false);
        ui->chkSunday->setChecked(m.sunday() == "1" ? true : false);
        ui->radioDownloadOnly->setChecked(m.downloadFirmwareOnly() == "1" ? true : false);
        ui->radioDownloadAndInstall->setChecked(m.downloadInstallFirmware() == "1" ? true : false);

        if(m.dailyUpdates() == "1")
        {
            ui->chkDaily->setChecked(true);
            ui->leStartTime->setEnabled(true);
        }
        else if(m.hourlyUpdates() == "1")
        {
            ui->chkHourly->setChecked(true);
            ui->leStartTime->setEnabled(false);
        }

        ui->checkBoxPLUData->setChecked(m.updatePLUData() == "1" ? true : false);
        ui->checkBoxLabelData->setChecked(m.updateLabelData() == "1" ? true : false);
        ui->checkBoxFlashkeyData->setChecked(m.updateFlashkeyData() == "1" ? true : false);
        ui->checkBoxOperatorData->setChecked(m.updateOperatorData() == "1" ? true : false);
        ui->checkBoxPlaylists->setChecked(m.updatePlaylists() == "1" ? true : false);
        ui->checkBoxAlerts->setChecked(m.updateAlerts() == "1" ? true : false);
        ui->checkBoxScaleConfiguration->setChecked(m.updateScaleConfiguration() == "1" ? true : false);
        ui->checkBoxSynchronizationConfiguration->setChecked(m.updateSynchronizationConfiguration() == "1" ? true : false);
        ui->checkBoxStoreNameNumber->setChecked(m.updateStoreNameNumber() == "1" ? true : false);
        ui->checkBoxVideos->setChecked(m.updateVideos() == "1" ? true : false);
        ui->checkBoxImages->setChecked(m.updateImages() == "1" ? true : false);
        ui->checkBoxAudio->setChecked(m.updateAudio() == "1" ? true : false);
        ui->checkBoxFirmware->setChecked(m.updateFirmware() == "1" ? true : false);

        ui->checkBoxPLUDataSendToDept->setChecked(m.sendPLUData() == "1" ? true : false);
        ui->checkBoxLabelDataSendToDept->setChecked(m.sendLabelData() == "1" ? true : false);
        ui->checkBoxFlashkeyDataSendToDept->setChecked(m.sendFlashkeyData() == "1" ? true : false);
        ui->checkBoxOperatorDataSendToDept->setChecked(m.sendOperatorData() == "1" ? true : false);
        ui->checkBoxPlaylistsSendToDept->setChecked(m.sendPlaylists() == "1" ? true : false);
        ui->checkBoxAlertsSendToDept->setChecked(m.sendAlerts() == "1" ? true : false);
        ui->checkBoxScaleConfigurationSendToDept->setChecked(m.sendScaleConfiguration() == "1" ? true : false);
        ui->checkBoxSynchronizationConfigurationSendToDept->setChecked(m.sendSynchronizationConfiguration() == "1" ? true : false);
        ui->checkBoxStoreNameNumberSendToDept->setChecked(m.sendStoreNameNumber() == "1" ? true : false);
        ui->checkBoxVideosSendToDept->setChecked(m.sendVideos() == "1" ? true : false);
        ui->checkBoxImagesSendToDept->setChecked(m.sendImages() == "1" ? true : false);
        ui->checkBoxAudioSendToDept->setChecked(m.sendAudio() == "1" ? true : false);
        ui->checkBoxFirmwareSendToDept->setChecked(m.sendFirmware() == "1" ? true : false);

        ui->chkAllowOperatorSendToDept->setChecked(m.allowOperatorSendToDept() == "1" ? true : false);
        ui->chkAllowOperatorUpdateNow->setChecked(m.allowOperatorUpdateNow() == "1" ? true : false);

        if(m.useAutomaticUpdates() == "1")
        {
            ui->cbAutoSearch->setChecked(true);
        }
        else
        {
            ui->cbAutoSearch->setChecked(false);

        }

        if(!prelimTesting)
        {
            if(m.useAutomaticUpdates() == "1")
                toggleUseAutomatic(true);
            else
                toggleUseAutomatic(false);

            connect(ui->cbAutoSearch, SIGNAL(toggled(bool)), this, SLOT(toggleUseAutomatic(bool)));
        }
        else
        {
            toggleUseAutomatic(false);
            ui->cbAutoSearch->setEnabled(false);
            ui->checkBoxFirmware->setEnabled(false);
            ui->butDownloadUpdates->setEnabled(false);
            ui->TabWidget->setTabEnabled(3,false);
            ui->TabWidget->removeTab(3);
            ui->chkAllowOperatorSendToDept->setEnabled(false);
            ui->chkAllowOperatorSendToDept->hide();
        }

        // HT-360
        ui->cboAutoBackupFrequency->setCurrentIndex( ui->cboAutoBackupFrequency->findData( m.autoBackupDBFrequency() ) );
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgScaleUpgradeWrite == msg[1].toInt()) //112,121
    {
        if( msg.size() == 5 )
        {
            QStringList time1;
            if( msg[4].contains(":") )
            {
                time1 = msg[4].split(":", QString::KeepEmptyParts);
                ui->leHour->setText(time1.at(0));
            }

            QString time2 = time1.at(1).trimmed();
            if( time2.contains(" ") )
            {
                // Do it this way if we have a time that contains an AM/PM indicator
                ui->leMinutes->setText(time1.at(1).split(" ",QString::KeepEmptyParts).at(0));
                ui->butAmPm->setText(time1.at(1).split(" ",QString::KeepEmptyParts).at(1).toUpper());
            }
            else if( time1.at(1).length() > 0)
            {
                // then the time string contains minutes, but not the AM/PM indicator so default it to AM
                ui->leMinutes->setText(time1.at(1).split(" ",QString::KeepEmptyParts).at(0));
                ui->butAmPm->setText("AM");
            }
            else
            {
                // then there is no minute string at all, so default both
                ui->leMinutes->setText("00");
                ui->butAmPm->setText("AM");
            }
        }
        else
        {
            // then we didnt have a time string at all so default it.
            ui->leHour->setText("5");
            ui->leMinutes->setText("00");
            ui->butAmPm->setText("AM");
        }

        if(ui->butAmPm->text() == "AM")
        {
            isAMSelected = true;
        }
        else
        {
            isAMSelected = false;
        }
    }
    else if(SessionDev == msg[0].toInt() && SnVersionInfo == msg[1].toInt()) //100,2
    {
        ui->lblVersion->setText(tr("Current software version: ") + msg[3]);
    }
    else if(SessionDev == msg[0].toInt() && SnUpgradeFirmwareStatus == msg[1].toInt()) //100,147
    {
        processingAnUpgrade = false;
        if(msg[2] == "Finished")
        {
            // the updates are finished so change the label.
            ui->lblVersion->setText(tr("Upgrade Completed."));
            ui->butDownloadUpdates->setEnabled(true);
            session_->send(SessionDev, SnVersionInfo); //100,2

        }
    }
    else if(SessionDev == msg[0].toInt() && SnUpgradeSFTPBadAuthentication == msg[1].toInt()) //100,147
    {
        processingAnUpgrade = false;
        if(msg[2] == "BadAuthentication")
        {
            // the updates are finished so change the label.
            ui->lblVersion->setText(tr("No access to upgrades on SFTP server."));
            ui->butDownloadUpdates->setEnabled(true);
            session_->send(SessionDev, SnVersionInfo); //100,2
        }
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgFTPOptionsWrite == msg[1].toInt()) // 112,174
    {
        // Populate the "Select Updates" tab
        if(msg[2] == "1")
            ui->checkBoxPLUData->setChecked(true);
        else
            ui->checkBoxPLUData->setChecked(false);

        if(msg[3] == "1")
            ui->checkBoxLabelData->setChecked(true);
        else
            ui->checkBoxLabelData->setChecked(false);

        if(msg[4] == "1")
            ui->checkBoxFlashkeyData->setChecked(true);
        else
            ui->checkBoxFlashkeyData->setChecked(false);

        if(msg[5] == "1")
            ui->checkBoxOperatorData->setChecked(true);
        else
            ui->checkBoxOperatorData->setChecked(false);

        if(msg[6] == "1")
            ui->checkBoxPlaylists->setChecked(true);
        else
            ui->checkBoxPlaylists->setChecked(false);

        if(msg[7] == "1")
            ui->checkBoxAlerts->setChecked(true);
        else
            ui->checkBoxAlerts->setChecked(false);

        if(msg[8] == "1")
            ui->checkBoxScaleConfiguration->setChecked(true);
        else
            ui->checkBoxScaleConfiguration->setChecked(false);

        if(msg[9] == "1")
            ui->checkBoxSynchronizationConfiguration->setChecked(true);
        else
            ui->checkBoxSynchronizationConfiguration->setChecked(false);

        if(msg[10] == "1")
            ui->checkBoxStoreNameNumber->setChecked(true);
        else
            ui->checkBoxStoreNameNumber->setChecked(false);

        if(msg[11] == "1")
            ui->checkBoxVideos->setChecked(true);
        else
            ui->checkBoxVideos->setChecked(false);

        if(msg[12] == "1")
            ui->checkBoxImages->setChecked(true);
        else
            ui->checkBoxImages->setChecked(false);

        if(msg[13] == "1")
            ui->checkBoxAudio->setChecked(true);
        else
            ui->checkBoxAudio->setChecked(false);

        if(msg[14] == "1")
            ui->checkBoxFirmware->setChecked(true);
        else
            ui->checkBoxFirmware->setChecked(false);
    }
    else if(ConfigurationDev == msg[0].toInt() &&  CfgFTPResponseWrite == msg[1].toInt())
    {
        // handle the response from an update now
        bool keepChecking = true;
        doneADownload = true;
        QString SFTPresponse = msg[2];
        QStringList responseParts = SFTPresponse.split("|",QString::KeepEmptyParts);

        if(responseParts[0].contains("updateNow"))
        {
            if(buttonHit == "DownloadOnly")
            {
                // we are finished
                ui->downloadInstallAlert->setText(tr("Download complete."));
                LOG_DEBUG( "downloadInstallAlert->setText-7", NULL );
                ui->downloadInstallAlert->show();
                manageInstallUpdatesButton();

            }
            else if(buttonHit == "UpdateNow")
            {
                ui->downloadInstallAlert->hide();
                QString downloadPath = "/usr/local/hobart/download";
                QString pendingPath = "/usr/local/hobart/pending";

                QDir downloadDir( downloadPath );
                QDir pendingDir( pendingPath );
                pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int pendingTotalFiles = pendingDir.count();

                downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int downloadTotalFiles = downloadDir.count();

                if(pendingTotalFiles == 0 && downloadTotalFiles == 0 )
                {
                    ui->pBarDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblDataXfer->setText("");
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                    keepModal = false;
                }
                else if(!ui->checkBoxFirmware->isChecked() &&
                     !ui->checkBoxFlashkeyData->isChecked() &&
                     !ui->checkBoxLabelData->isChecked() &&
                     !ui->checkBoxOperatorData->isChecked() &&
                     !ui->checkBoxPLUData->isChecked() &&
                     !ui->checkBoxScaleConfiguration->isChecked() &&
                     !ui->checkBoxStoreNameNumber->isChecked() &&
                     !ui->checkBoxSynchronizationConfiguration->isChecked())
                {
                    ui->pBarDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblDataXfer->setText("");
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                    keepModal = false;
                }
                else
                {
                    ui->lblDataXfer->setText("Download complete....installing now....");
                    QString downloadPath = "/usr/local/hobart/download";
                    QString pendingPath = "/usr/local/hobart/pending";

                    QDir downloadDir( downloadPath );
                    QDir pendingDir( pendingPath );
                    pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                    int pendingTotalFiles = pendingDir.count();

                    downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                    int downloadTotalFiles = downloadDir.count();

                    if((downloadTotalFiles + pendingTotalFiles) == 0)
                    {
                        if(waitForEmptyInstallOnly && buttonHit == "InstallOnly")
                        {
                            //----------------
                            // Update Data tab
                            //----------------

                            ui->downloadInstallAlert->setText(tr("Checking firmware..."));
                            LOG_DEBUG( "downloadInstallAlert->setText-8", NULL );

                            waitForEmptyInstallOnly = false;
                            session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"installUpgrades|1\"\n");
                        }

                        //dirs are cleaned out so tell the User
                        if(doneADownload)
                        {

                            if(ui->checkBoxFirmware->isChecked())
                           {
                               QString cmd = "";
                               cmd = "installUpgrades|1";
                               ui->lblDataXfer->setText(tr("Installing firmware..."));
                               session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"" + cmd + "\"\n");
                           }
                            else
                            {
                                ui->lblDataXfer->setText("");
                                ui->pBarDataXfer->hide();
                                ui->butCloseUpdateNow->setEnabled(true);
                                ui->butCloseUpdateNow->show();
                                ui->lblUpdateNowStatus->setText("Update is complete.");
                            }
                        }
                        keepModal = false;

                        keepChecking = false;
                    }
                    else
                    {
                        keepChecking = true;
                    }
                }
            }
        }
        // SOF-2779
        else if( ( responseParts[0].contains( "installUpgrades" ) ) &&
               ( ( responseParts.size() >= 3 ) && ( responseParts[2].contains("done") ) ) )
        {
            if( buttonHit == "InstallOnly" )
            {
                //------------------------------------
                // Update Data tab --> Install Updates
                //------------------------------------
                ui->downloadInstallAlert->setText(tr("Update is complete.") );
                LOG_DEBUG( "downloadInstallAlert->setText-14", NULL );
                ui->downloadInstallAlert->show();

                keepModal = false;
            }
        }
        else if(responseParts.size() > 2 && (responseParts[1].contains("installUpgrades") && responseParts[0].contains("error") && responseParts[2].contains("no upgrades to install")))
        {
            ui->downloadInstallAlert->setText(tr("Installation complete."));
            LOG_DEBUG( "downloadInstallAlert->setText-9", NULL );
            if(buttonHit == "UpdateNow")
            {
                ui->downloadInstallAlert->hide();
                QString downloadPath = "/usr/local/hobart/download";
                QString pendingPath = "/usr/local/hobart/pending";

                QDir downloadDir( downloadPath );
                QDir pendingDir( pendingPath );
                pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int pendingTotalFiles = pendingDir.count();

                downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int downloadTotalFiles = downloadDir.count();

                if(pendingTotalFiles == 0 && downloadTotalFiles == 0 )
                {
                    ui->pBarDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblDataXfer->setText("");
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                }
                else if(!ui->checkBoxFirmware->isChecked() &&
                        !ui->checkBoxFlashkeyData->isChecked() &&
                        !ui->checkBoxLabelData->isChecked() &&
                        !ui->checkBoxOperatorData->isChecked() &&
                        !ui->checkBoxPLUData->isChecked() &&
                        !ui->checkBoxScaleConfiguration->isChecked() &&
                        !ui->checkBoxStoreNameNumber->isChecked() &&
                        !ui->checkBoxSynchronizationConfiguration->isChecked())
                {
                    ui->pBarDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblDataXfer->setText("");
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                }
            }

            waitForEmptyInstallOnly = false;
            keepModal = false;
        }
        else if(responseParts.size() > 2 && responseParts[0].contains("error"))
        {
            if(buttonHit == "InstallOnly")
            {
                //----------------
                // Update Data tab
                //----------------

                if(responseParts[1].contains("installUpgrades") && responseParts[2].contains("no upgrades to install"))
                {
                    ui->downloadInstallAlert->setText(tr("Installation complete."));
                    LOG_DEBUG( "downloadInstallAlert->setText-10", NULL );
                }
                else if(responseParts[1].contains("installUpgrades"))
                {
                    ui->downloadInstallAlert->setText(tr("Installation complete."));
                    LOG_DEBUG( "downloadInstallAlert->setText-11", NULL );
                }
                keepModal = false;
                waitForEmptyInstallOnly = false;
            }
            else if(buttonHit == "DownloadOnly")
            {
                // we are finished
                ui->downloadInstallAlert->setText(tr("Download complete."));
                LOG_DEBUG( "downloadInstallAlert->setText-12", NULL );
                ui->downloadInstallAlert->show();
                manageInstallUpdatesButton();


            }
            else if(buttonHit == "UpdateNow")
            {
                ui->downloadInstallAlert->hide();
                if(responseParts[2].contains("Undetermined error",Qt::CaseInsensitive))
                {
                     ui->lblDataXfer->setText("Update stopped...\nCorrect the Location/Login tab\nentries and try again.");
                }
                else if(responseParts[2].contains("no upgrades to install"))
                {
                    ui->lblDataXfer->setText("");
                }
                else
                {
                    bool keeplooking = true;
                    foreach (const QString &str, responseParts)
                    {
                        if (str.contains("arguments"))
                           {
                            ui->lblUpdateNowStatus->setText(tr("Update Now done."));
                            ui->downloadInstallAlert->hide();
                            ui->lblDataXfer->setText(tr("Correct the information you\nprovided on the Location/Login tab."));
                            ui->pBarDataXfer->hide();
                            ui->butCloseUpdateNow->setEnabled(true);
                            ui->butCloseUpdateNow->show();
                            hadLostConnectionError = true;
                            keeplooking = false;
                        }
                    }
                    if(keeplooking)
                    {
                        ui->lblDataXfer->setText(tr(""));
                        ui->lblUpdateNowStatus->setText(tr("Update Now done."));
                        ui->downloadInstallAlert->hide();
                        ui->pBarDataXfer->hide();
                        ui->butCloseUpdateNow->setEnabled(true);
                        ui->butCloseUpdateNow->show();
                        hadLostConnectionError = true;
                    }
                }

                ui->pBarDataXfer->hide();
                ui->butCloseUpdateNow->setEnabled(true);
                ui->butCloseUpdateNow->show();
                keepChecking = false;
            }
        }
        else
        {
            if(buttonHit == "DownloadOnly")
            {
                // we are finished
                ui->downloadInstallAlert->setText(tr("Download complete."));
                LOG_DEBUG( "downloadInstallAlert->setText-13", NULL );
                ui->downloadInstallAlert->show();

                manageInstallUpdatesButton();
            }
            else if(buttonHit == "UpdateNow")
            {
                bool foundABranch = false;
                foreach (const QString &str, responseParts)
                {
                    if (str.contains("Lost connection"))
                    {
                        ui->downloadInstallAlert->hide();
                        ui->lblDataXfer->setText(tr("Lost Connection or no route to server."));
                        ui->pBarDataXfer->hide();
                        ui->butCloseUpdateNow->setEnabled(true);
                        ui->butCloseUpdateNow->show();
                        hadLostConnectionError = true;
                        keepChecking = false;
                        foundABranch = true;
                    }
                }
                if(!foundABranch)
                {
                    if(responseParts.size() > 2 && (responseParts[0].contains("error") && responseParts[2].contains("Lost connection")))
                    {
                        ui->downloadInstallAlert->hide();
                        ui->lblDataXfer->setText(tr("Lost Connection or no route to server."));
                        ui->pBarDataXfer->hide();
                        ui->butCloseUpdateNow->setEnabled(true);
                        ui->butCloseUpdateNow->show();
                        hadLostConnectionError = true;
                        keepChecking = false;
                    }
                    else
                    {
                        ui->downloadInstallAlert->hide();
                        if(!hadLostConnectionError && responseParts[0].contains("error"))
                        {
                            ui->lblDataXfer->setText(tr("Update stopped...\nan undetermined error occurred."));
                            ui->pBarDataXfer->hide();
                            ui->butCloseUpdateNow->setEnabled(true);
                            ui->butCloseUpdateNow->show();
                            keepChecking = false;
                        }
                    }
                }
            }
        }
    }

    if (( ConfigurationDev == msg[0].toInt() ) && ( CfgSynchronizationWrite == msg[1].toInt() ))
    {
        // 112,181 - update sync scale type
        updateScaleSynchronizationType(msg);
    }

    manageInstallUpdatesButton();
}
