/* @file superUpgradeFirmwareViaUSB.cpp
 * superUpgradeFirmwareViaUSB
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Sets the confguration for Uprgrading firmware via USB
   Java class for diretory picker loacted at:\SVN\fastrack\trunk\Windux\Application\source\mom\com\hobart\scale\mom
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../dialogs/filedialog.h"

#include "../modes/supervisormode.h"
#include "python.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "superUpgradeFirmwareViaUSB.h"
#include "ui_superUpgradeFirmwareViaUSB.h"

#include "../dialogs/messageboxdialog.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

superUpgradeFirmwareViaUSB::superUpgradeFirmwareViaUSB(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superUpgradeFirmwareViaUSB)
{
    ui->setupUi(this);
    scaleScreen();

    ui->gbNotice->hide();
   // ui->butDelete->setEnabled(false);

    loading = true;

    //  loadColorBoxes();

    scaleConfigData    = &session_->scaleConfig->scaleConfig;
    internalConfigData = &session_->scaleConfig->internalConfig;

    /*if( session_->scaleConfig->getScaleModel() == 2 )//HTi-Access
    {
        connect(ui->butChoose, SIGNAL(clicked()), this, SLOT(openDir()));
    }
    else
    {*/
        connect(ui->butChoose, SIGNAL(clicked()), this, SLOT(openFile()));
    //}
    connect(ui->butUpgrade, SIGNAL(clicked()), this, SLOT(upgrade()));
    connect(ui->butOK, SIGNAL(clicked()), this, SLOT(notice()));
    translate();
}

superUpgradeFirmwareViaUSB::~superUpgradeFirmwareViaUSB()
{
    delete ui;
}

void superUpgradeFirmwareViaUSB::notice()
{
    ui->gbNotice->hide();
    ui->butUpgrade->setEnabled(false);
}

void superUpgradeFirmwareViaUSB::upgrade()
{
    LOG_CLASS_INFO( "superUpgradeFirmwareViaUSB", "upgrade" );

    //TBD
    //120,1," + actionFile);		//	VersionManagerDev, UpgradeManual


    /*
    if(firmwareFilename != "")
    {
        session_->send(VersionManagerDev, UpgradeManual, firmwareDirName.replace("/", "\\"));
    }
    */

    LOG_DEBUG( "file is: %s", firmwareFilename.toLatin1().constData() );

    //--------------------------------------------------------------------------------------
    // SOF-4509: Validate the wrapper firmware selected is compatible with the wrapper model
    //--------------------------------------------------------------------------------------
    if( session_->isPrepack() )
    {
        bool ngwCompatError = false;
        QString strModelName = session_->scaleConfig->getScaleModelName();

        if( strModelName.toUpper().startsWith( "NGW" ) )
        {
            //--------------------------------------------------------------------------------------------------------------------
            // 2nd Label Applier wrappers will contain "2P" or "CP" at the end (e.g. NGW2-LS2RL2P or NGW2-LS2RLCP)
            // The firmware upgrade for a 2nd Label Applier wrapper will be named "htseries-any-ngw2p-XX.XX-0-release.package.tgz"
            //--------------------------------------------------------------------------------------------------------------------

            if( ( session_->scaleConfig->isWrapperTwoPrinter() ) && ( firmwareFilename.toLower().contains( "htseries-any-ngw-" ) ) )
                ngwCompatError = true;

            if( ( !( session_->scaleConfig->isWrapperTwoPrinter() ) ) && ( firmwareFilename.toLower().contains( "htseries-any-ngw2p-" ) ) )
                ngwCompatError = true;

            if( ngwCompatError )
            {
                MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                dialog.setAllowOverlayClickDismiss( false );

                QString msgStr = tr( "You have selected a wrapper firmware upgrade file that is not compatible with the wrapper model (") + strModelName.toLatin1().constData() + tr( ").\n\nPlease select a different firmware upgrade file.");
                dialog.setTitle( tr( "INCOMPATIBLE UPGRADE FILE" ) );
                dialog.setText( msgStr );
                dialog.setStandardButtons( QDialogButtonBox::Ok );
                dialog.exec();

                return;
            }
        }
    }

    QStringList args;
    args.append(firmwareFilename);

    /*if( session_->scaleConfig->getScaleModel() == 2 )//HTi-Access
    {
        session_->send(VersionManagerDev, UpgradeManual,firmwareFilename);
    }
    else
    {*/
        python pythonProcess;
        if (pythonProcess.execCommand("installUpgrade", args))
        {
            if (pythonProcess.getExitCode() == 0)
            {
                LOG_DEBUG("installUpgrade called", NULL);
            }
            else
            {
                LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }
        }
        else
        {
            LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
        }
    //}
}

void superUpgradeFirmwareViaUSB::openDir()
{
    FileDialog dialog( mainWindow_ );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    // TODO:
    // Need to set directory of upgrade files location to path to windows thumb drive
    dialog.setDirectory( "/media/thumb" );
    //dialog.setFileMode( QFileDialog::DirectoryOnly );
    dialog.setFileMode( QFileDialog::Directory );
    dialog.setOptions( QFileDialog::ShowDirsOnly );
    dialog.setWindowTitle( tr( "Choose Upgrade Directory" ) );

    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            firmwareFilename = selected[0];

        QListWidgetItem *fileChoice = new QListWidgetItem;
        ui->lwFileChoice->clear();

        if ( !firmwareFilename.isEmpty() )
        {
           ui->butUpgrade->setEnabled(true);

           fileChoice->setText(firmwareFilename);
           //ui->lvFileChoice->
        }
        else
        {
            ui->butUpgrade->setEnabled(false);
            fileChoice->setText(tr("No Upgrade File Selected"));
            firmwareFilename = "";
        }

        ui->lwFileChoice->addItem(fileChoice);
    }
}

void superUpgradeFirmwareViaUSB::openFile()
{
    QString startDirStr;

#if defined( Q_OS_LINUX )
    startDirStr = "/media/thumb";
    if ( !QDir( startDirStr ).exists() )
        startDirStr = "/media";
#elif defined( Q_OS_WIN )
    startDirStr = "D:\\";
    if ( !QDir( startDirStr ).exists() )
        startDirStr = "C:\\";
#endif

    FileDialog dialog( mainWindow_ );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( startDirStr );
    dialog.setFileMode( QFileDialog::ExistingFile );
    dialog.setNameFilter( tr( "Upgrade File (*.tgz);;License File (*.lic)" ) );
    dialog.setWindowTitle( tr( "Choose Upgrade File" ) );

    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );

        if ( 1 == selected.count() )
            firmwareFilename = selected[0];

        QListWidgetItem *fileChoice = new QListWidgetItem;
        ui->lwFileChoice->clear();

        if ( !firmwareFilename.isEmpty() )
        {
           ui->butUpgrade->setEnabled(true);

           fileChoice->setText(firmwareFilename);
           //ui->lvFileChoice->
        }
        else
        {
            ui->butUpgrade->setEnabled(false);
            fileChoice->setText(tr("No Upgrade File Selected"));
            firmwareFilename = "";
        }

        ui->lwFileChoice->addItem(fileChoice);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superUpgradeFirmwareViaUSB::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superUpgradeFirmwareViaUSB::onScreenChanging()
{
    //alerts are saved during a change so just save from to and server
    //saveFromToIPOrDNS();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superUpgradeFirmwareViaUSB::onSessionMessage( const SessionMessage& msg )
{
    if(Error == msg[0].toInt() && ErrVersionManagerDevice == msg[1].toInt())//103,120 Version was incorrect
    {
        ui->gbNotice->show();
    }
}
