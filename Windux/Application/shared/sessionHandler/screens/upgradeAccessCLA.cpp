/* @file upgradeAccessCLA.cpp
 * This page is for checking the progress of the CLA upgrade.
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 */

#include "upgradeAccessCLA.h"
#include "ui_upgradeAccessCLA.h"

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

upgradeAccessCLA::upgradeAccessCLA(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::upgradeAccessCLA)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
upgradeAccessCLA::~upgradeAccessCLA()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::initialize()
{
    //Set up signals and slots
    // Buttons
    connect(ui->btnDone, SIGNAL(clicked()), this, SLOT(finish()));

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ));

    disableDoneButton();

    //Initialize the state of the upgrade labels
    appStatus   = -1;
    dataStatus  = -1;
    bootStatus  = -1;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::translate()
{
    ui->retranslateUi(this);
    refreshUpgradeStates();
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::enableDoneButton()
{
    ui->btnDone->setEnabled(true);
    upgradeInProgress = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::disableDoneButton()
{
    ui->btnDone->setEnabled(false);
    upgradeInProgress = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::updateBytesSent(int bytes)
{
    ui->lblBytesSent->setText(QString::number(bytes));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::updateTotalBytes(int bytes)
{
    ui->lblTotalBytes->setText(QString::number(bytes));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::returnToHome()
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
void upgradeAccessCLA::finish()
{
    returnToHome();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::onScreenChanged()
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
bool upgradeAccessCLA::onScreenChanging()
{
    //Disable the ability to leave the screen while upgrading
    if( upgradeInProgress )
    {
        return false;
    }
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::onSessionMessage( const SessionMessage& msg )
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
void upgradeAccessCLA::updateNodeState(QLabel* label, int status)
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
void upgradeAccessCLA::updateNodeState(int node, int status)
{
    switch( node )
    {
    case CLAAppSW:
        updateNodeState(ui->lblApplication2, status);
        appStatus = status;
        break;
    case CLADataSW:
        updateNodeState(ui->lblData2, status);
        dataStatus = status;
        break;
    case CLABootSW:
        updateNodeState(ui->lblBoot2, status);
        bootStatus = status;
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void upgradeAccessCLA::refreshUpgradeStates()
{
    updateNodeState(ui->lblApplication2, appStatus);
    updateNodeState(ui->lblData2, dataStatus);
    updateNodeState(ui->lblBoot2, bootStatus);
}
