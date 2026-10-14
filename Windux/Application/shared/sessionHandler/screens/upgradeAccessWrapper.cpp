/* @file upgradeAccessWrapper.cpp
 * This page is for checking the progress of the Wrapper upgrade.
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 */

#include "upgradeAccessWrapper.h"
#include "ui_upgradeAccessWrapper.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

upgradeAccessWrapper::upgradeAccessWrapper(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::upgradeAccessWrapper)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeAccessWrapper::~upgradeAccessWrapper()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::initialize()
{
    hideDialog();

    //Set up signals and slots
    // Buttons
    connect(ui->btnDone, SIGNAL(clicked()), this, SLOT(finish()));
    connect(ui->btnOk, SIGNAL( clicked()), this, SLOT(okPressed()));

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ));

    disableDoneButton();
    initializeWrapperUpgradeStatus();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::initializeWrapperUpgradeStatus()
{
    wrapperUpgradeStatus.masterAppStatus    = -1;
    wrapperUpgradeStatus.masterDataStatus   = -1;
    wrapperUpgradeStatus.masterBootStatus   = -1;

    wrapperUpgradeStatus.slave1AppStatus    = -1;
    wrapperUpgradeStatus.slave1DataStatus   = -1;
    wrapperUpgradeStatus.slave1BootStatus   = -1;

    wrapperUpgradeStatus.slave3AppStatus    = -1;
    wrapperUpgradeStatus.slave3DataStatus   = -1;
    wrapperUpgradeStatus.slave3BootStatus   = -1;

    wrapperUpgradeStatus.slave4AppStatus    = -1;
    wrapperUpgradeStatus.slave4DataStatus   = -1;
    wrapperUpgradeStatus.slave4BootStatus   = -1;

    wrapperUpgradeStatus.slave5AppStatus    = -1;
    wrapperUpgradeStatus.slave5DataStatus   = -1;
    wrapperUpgradeStatus.slave5BootStatus   = -1;

    wrapperUpgradeStatus.slave6AppStatus    = -1;
    wrapperUpgradeStatus.slave6DataStatus   = -1;
    wrapperUpgradeStatus.slave6BootStatus   = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::translate()
{
    ui->retranslateUi(this);
    refreshUpgradeStates();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::refreshUpgradeStates()
{
    updateNodeState(ui->lblApplication2, wrapperUpgradeStatus.masterAppStatus);
    updateNodeState(ui->lblData2, wrapperUpgradeStatus.masterDataStatus);
    updateNodeState(ui->lblBoot2, wrapperUpgradeStatus.masterBootStatus);

    updateNodeState(ui->lblApplication1, wrapperUpgradeStatus.slave1AppStatus);
    updateNodeState(ui->lblData1, wrapperUpgradeStatus.slave1DataStatus);
    updateNodeState(ui->lblBoot1, wrapperUpgradeStatus.slave1BootStatus);

    updateNodeState(ui->lblApplication3, wrapperUpgradeStatus.slave3AppStatus);
    updateNodeState(ui->lblData3, wrapperUpgradeStatus.slave3DataStatus);
    updateNodeState(ui->lblBoot3, wrapperUpgradeStatus.slave3BootStatus);

    updateNodeState(ui->lblApplication4, wrapperUpgradeStatus.slave4AppStatus);
    updateNodeState(ui->lblData4, wrapperUpgradeStatus.slave4DataStatus);
    updateNodeState(ui->lblBoot4, wrapperUpgradeStatus.slave4BootStatus);

    updateNodeState(ui->lblApplication5, wrapperUpgradeStatus.slave5AppStatus);
    updateNodeState(ui->lblData5, wrapperUpgradeStatus.slave5DataStatus);
    updateNodeState(ui->lblBoot5, wrapperUpgradeStatus.slave5BootStatus);

    updateNodeState(ui->lblApplication6, wrapperUpgradeStatus.slave6AppStatus);
    updateNodeState(ui->lblData6, wrapperUpgradeStatus.slave6DataStatus);
    updateNodeState(ui->lblBoot6, wrapperUpgradeStatus.slave6BootStatus);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::enableDoneButton()
{
    ui->btnDone->setEnabled(true);
    upgradeInProgress = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::disableDoneButton()
{
    ui->btnDone->setEnabled(false);
    upgradeInProgress = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::showDialog()
{
    ui->Dialog->setVisible(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::hideDialog()
{
    ui->Dialog->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::updateBytesSent(int bytes)
{
    ui->lblBytesSent->setText(QString::number(bytes));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::updateTotalBytes(int bytes)
{
    ui->lblTotalBytes->setText(QString::number(bytes));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::returnToHome()
{
    //Switch to the home screen when done upgrading
    mainWindow_->navigateHome();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::finish()
{
    showDialog();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::okPressed()
{
    hideDialog();
    returnToHome();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    // make sure weight bar is hidden
    if ( operateMode )
        operateMode->hideWeightBar();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          END OF SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool upgradeAccessWrapper::onScreenChanging()
{
    //Disable the ability to leave the screen while upgrading
    if( upgradeInProgress )
    {
        return false;
    }
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::onSessionMessage( const SessionMessage& msg )
{
    if (( VersionManagerDev == msg[0].toInt() ) && ( UsbWrapperUpgradeStatus == msg[1].toInt() ))
    {
        MadpWrUpgradeStatus m(msg);
        updateNodeState(m.node().toInt(), m.status().toInt());
    }
    else if (( VersionManagerDev == msg[0].toInt() ) && ( UpgradeXferStatus == msg[1].toInt() ))
    {
        MadpUpgradeTransferStatus m(msg);
        updateBytesSent(m.bytesSent().toInt());
        updateTotalBytes(m.totalBytes().toInt());
    }
    else if (( VersionManagerDev == msg[0].toInt() ) && ( UpgradeComplete == msg[1].toInt() ))
    {
        enableDoneButton();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::updateNodeState(QLabel* label, int status)
{
    switch( status )
    {
    case MT_START:
        label->setText(tr("In Progress"));
        label->setStyleSheet("background-color: yellow");
        break;
    case MT_FAILED:
        label->setText(tr("Failed"));
        label->setStyleSheet("background-color: red");
        enableDoneButton();
        break;
    case MT_SUCCESS:
        label->setText(tr("Done"));
        label->setStyleSheet("background-color: green");
        break;
    case MT_PENDING:
        label->setText(tr("Pending"));
        label->setStyleSheet("background-color: #99CCFF");
        break;
    case MT_ABORTED:
        label->setText(tr("Aborted"));
        label->setStyleSheet("background-color: red");
        enableDoneButton();
        break;
    default:
        break;
    }
}

//Helper method for updateNodeState
//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessWrapper::updateNodeState(int node, int status)
{
    switch( node )
    {
    case WRMasterNVDataSW:
    case WREMasterNVDataSW:
    {
        updateNodeState(ui->lblData2, status);
        wrapperUpgradeStatus.masterDataStatus = status;
        break;
    }
    case WRMasterBootSW:
    case WREMasterBootSW:
    {
        updateNodeState(ui->lblBoot2, status);
        wrapperUpgradeStatus.masterBootStatus = status;
        break;
    }
    case WRMasterAppSW:
    case WREMasterAppSW:
    {
        updateNodeState(ui->lblApplication2, status);
        wrapperUpgradeStatus.masterAppStatus = status;
        break;
    }
    case WRSlave1NVDataSW:
    case WRESlave1NVDataSW:
    {
        updateNodeState(ui->lblData1, status);
        wrapperUpgradeStatus.slave1DataStatus = status;
        break;
    }
    case WRSlave1BootSW:
    case WRESlave1BootSW:
    {
        updateNodeState(ui->lblBoot1, status);
        wrapperUpgradeStatus.slave1BootStatus = status;
        break;
    }
    case WRSlave1AppSW:
    case WRESlave1AppSW:
    {
        updateNodeState(ui->lblApplication1, status);
        wrapperUpgradeStatus.slave1AppStatus = status;
        break;
    }

    case WRSlave3NVDataSW:
    case WRESlave3NVDataSW:
    {
        updateNodeState(ui->lblData3, status);
        wrapperUpgradeStatus.slave3DataStatus = status;
        break;
    }
    case WRSlave3BootSW:
    case WRESlave3BootSW:
    {
        updateNodeState(ui->lblBoot3, status);
        wrapperUpgradeStatus.slave3BootStatus = status;
        break;
    }
    case WRSlave3AppSW:
    case WRESlave3AppSW:
    {
        updateNodeState(ui->lblApplication3, status);
        wrapperUpgradeStatus.slave3AppStatus = status;
        break;
    }

    case WRSlave4NVDataSW:
    case WRESlave4NVDataSW:
    {
        updateNodeState(ui->lblData4, status);
        wrapperUpgradeStatus.slave4DataStatus = status;
        break;
    }
    case WRSlave4BootSW:
    case WRESlave4BootSW:
    {
        updateNodeState(ui->lblBoot4, status);
        wrapperUpgradeStatus.slave4BootStatus = status;
        break;
    }
    case WRSlave4AppSW:
    case WRESlave4AppSW:
    {
        updateNodeState(ui->lblApplication4, status);
        wrapperUpgradeStatus.slave4AppStatus = status;
        break;
    }

    case WRSlave5NVDataSW:
    case WRESlave5NVDataSW:
    {
        updateNodeState(ui->lblData5, status);
        wrapperUpgradeStatus.slave5DataStatus = status;
        break;
    }
    case WRSlave5BootSW:
    case WRESlave5BootSW:
    {
        updateNodeState(ui->lblBoot5, status);
        wrapperUpgradeStatus.slave5BootStatus = status;
        break;
    }
    case WRSlave5AppSW:
    case WRESlave5AppSW:
    {
        updateNodeState(ui->lblApplication5, status);
        wrapperUpgradeStatus.slave5AppStatus = status;
        break;
    }

    case WRSlave6NVDataSW:
    {
        updateNodeState(ui->lblData6, status);
        wrapperUpgradeStatus.slave6DataStatus = status;
        break;
    }
    case WRSlave6BootSW:
    {
        updateNodeState(ui->lblBoot6, status);
        wrapperUpgradeStatus.slave6BootStatus = status;
        break;
    }
    case WRSlave6AppSW:
    {
        updateNodeState(ui->lblApplication6, status);
        wrapperUpgradeStatus.slave6AppStatus = status;
        break;
    }
    default:
        break;
    }
}
