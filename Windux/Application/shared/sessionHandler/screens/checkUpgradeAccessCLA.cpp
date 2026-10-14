/* @file checkUpgradeAccessCLA.cpp
 * This page is for checking whether you really want to do an upgrade.
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 */

#include "checkUpgradeAccessCLA.h"
#include "ui_checkUpgradeAccessCLA.h"

#include "upgradeAccessCLA.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

checkUpgradeAccessCLA::checkUpgradeAccessCLA(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::checkUpgradeAccessCLA)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
checkUpgradeAccessCLA::~checkUpgradeAccessCLA()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool checkUpgradeAccessCLA::onScreenChanging()
{
    //Don't want people to exit this page through the menu
    return switchScreen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    // make sure weight bar is hidden
    if ( operateMode )
        operateMode->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::initialize()
{
    switchScreen = false;

    //Set up signals and slots
    // Buttons
    connect(ui->btnBeginUpgrade, SIGNAL(clicked()), this, SLOT(beginUpgrade()));
    connect(ui->btnCancelUpgrade, SIGNAL(clicked()), this, SLOT(cancelUpgrade()));

    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ));

    //if we received the upgradeList before we were ready to upgrade
    if(session_->scaleConfig->receivedUpgradeList)
    {
        updateTable(session_->scaleConfig->upgradeList.componentList(), session_->scaleConfig->upgradeList.curVersionList(),
                    session_->scaleConfig->upgradeList.newVersionList(), session_->scaleConfig->upgradeList.isPendingList(),
                    session_->scaleConfig->upgradeList.numOfItems().toInt());

        //Reset the variable in case another device gets plugged in needing an upgrade
        session_->scaleConfig->receivedUpgradeList = false;
    }
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::loadUpgradePage()
{
    switchScreen = true;
    if( mainWindow_->isOperateMode() )
    {
        mainWindow_->switchToModeScreen<OperateMode, upgradeAccessCLA>();
    }
    else if ( mainWindow_->isSupervisorMode() )
    {
        mainWindow_->switchToModeScreen<SupervisorMode, upgradeAccessCLA>();
    }
    else if ( mainWindow_->isServiceMode() )
    {
        mainWindow_->switchToModeScreen<ServiceMode, upgradeAccessCLA>();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::beginUpgrade()
{
    // VersionManagerDev, BeginUpgrade, LabelApplier, CompactLabelApplier_EPSAC2
    QString msg = QString::number(LabelApplier) + "," + QString::number(5);
    session_->send(VersionManagerDev, BeginUpgrade, msg);

    loadUpgradePage();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::cancelUpgrade()
{
    // VersionManagerDev, CancelUpgrade, LabelApplier, CompactLabelApplier_EPSAC2
    QString msg = QString::number(LabelApplier) + "," + QString::number(5);
    session_->send(VersionManagerDev, CancelUpgrade, msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          END OF SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::onSessionMessage( const SessionMessage& msg )
{
    if (( VersionManagerDev == msg[0].toInt() ) && ( PeripheralUpgradeList == msg[1].toInt() ))
    {
        MadpUpgradeLists m(msg);
        updateTable(m.componentList(), m.curVersionList(), m.newVersionList(),
                    m.isPendingList(), m.numOfItems().toInt());
    }
    else if( Error == msg[0].toInt() && ErrVersionManagerDevice == msg[1].toInt() )
    {
        returnedErrorsVersionManagerDev m(msg);

        //Would you like to remove cla files
        if ( 120081 == m.errorCode() )
        {
            //User pressed Ok
            if ( returnedErrorsVersionManagerDev::ActionYes == m.errorData2() )
            {
                //Remove the files
                session_->send(VersionManagerDev, RemoveCLASWComponents);
            }

            //Leave page
            switchScreen = true;
            mainWindow_->navigateHome();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QStringList checkUpgradeAccessCLA::parseList(QString list)
{
    return list.split(",");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessCLA::updateTable(QString components, QString currentVersions, QString newVersions, QString pending, int numOfItems)
{
    QStringList componentList = parseList(components);
    QStringList currentVersionList = parseList(currentVersions);
    QStringList newVersionList = parseList(newVersions);
    QStringList pendingList = parseList(pending);

    for(int i = 0; i < numOfItems;++i)
    {
        switch( componentList[i].toInt() )
        {
        case upgradeAccessCLA::CLAAppSW:
        {
            ui->lblCurrentApplication2->setText(currentVersionList[i]);
            ui->lblNewApplication2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessCLA::CLADataSW:
        {
            ui->lblCurrentData2->setText(currentVersionList[i]);
            ui->lblNewData2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessCLA::CLABootSW:
        {
            ui->lblCurrentBoot2->setText(currentVersionList[i]);
            ui->lblNewBoot2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        default:
            break;
        }
    }
}
