/* @file checkUpgradeAccessWrapper.cpp
 * This page is for checking whether you really want to do an upgrade.
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 */

#include "checkUpgradeAccessWrapper.h"
#include "ui_checkUpgradeAccessWrapper.h"

#include "upgradeAccessWrapper.h"

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

checkUpgradeAccessWrapper::checkUpgradeAccessWrapper(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::checkUpgradeAccessWrapper)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
checkUpgradeAccessWrapper::~checkUpgradeAccessWrapper()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool checkUpgradeAccessWrapper::onScreenChanging()
{
    //Don't want people to exit this page through the menu
    return switchScreen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::initialize()
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
void checkUpgradeAccessWrapper::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::loadUpgradePage()
{
    switchScreen = true;
    if( mainWindow_->isOperateMode() )
    {
        mainWindow_->switchToModeScreen<OperateMode, upgradeAccessWrapper>();
    }
    else if ( mainWindow_->isSupervisorMode() )
    {
        mainWindow_->switchToModeScreen<SupervisorMode, upgradeAccessWrapper>();
    }
    else if ( mainWindow_->isServiceMode() )
    {
        mainWindow_->switchToModeScreen<ServiceMode, upgradeAccessWrapper>();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::beginUpgrade()
{
    //	VersionManagerDev, BeginUpgrade, Wrapper, WrEvolWrapper
    QString msg = QString::number(Wrapper) + "," + QString::number(2);
    session_->send(VersionManagerDev, BeginUpgrade, msg);

    loadUpgradePage();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::cancelUpgrade()
{
    //	VersionManagerDev, CancelUpgrade, Wrapper, WrEvolWrapper
    QString msg = QString::number(Wrapper) + "," + QString::number(2);
    session_->send(VersionManagerDev, CancelUpgrade, msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::onScreenChanged()
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
void checkUpgradeAccessWrapper::onSessionMessage( const SessionMessage& msg )
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

        //Would you like to remove wrapper files
        if ( 120024 == m.errorCode() )
        {
            //User pressed Ok
            if ( returnedErrorsVersionManagerDev::ActionYes == m.errorData2() )
            {
                //Remove the files
                session_->send(VersionManagerDev, RemoveWrapperSWComponents);
            }

            //Leave page
            switchScreen = true;
            mainWindow_->navigateHome();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QStringList checkUpgradeAccessWrapper::parseList(QString list)
{
    return list.split(",");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void checkUpgradeAccessWrapper::updateTable(QString components, QString currentVersions, QString newVersions, QString pending, int numOfItems)
{
    QStringList componentList = parseList(components);
    QStringList currentVersionList = parseList(currentVersions);
    QStringList newVersionList = parseList(newVersions);
    QStringList pendingList = parseList(pending);

    for(int i = 0; i < numOfItems;++i)
    {
        switch( componentList[i].toInt() )
        {
        case upgradeAccessWrapper::WRMasterNVDataSW:
        case upgradeAccessWrapper::WREMasterNVDataSW:
        {
            ui->lblCurrentData2->setText(currentVersionList[i]);
            ui->lblNewData2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRMasterBootSW:
        case upgradeAccessWrapper::WREMasterBootSW:
        {
            ui->lblCurrentBoot2->setText(currentVersionList[i]);
            ui->lblNewBoot2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRMasterAppSW:
        case upgradeAccessWrapper::WREMasterAppSW:
        {
            ui->lblCurrentApplication2->setText(currentVersionList[i]);
            ui->lblNewApplication2->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication2->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave1NVDataSW:
        case upgradeAccessWrapper::WRESlave1NVDataSW:
        {
            ui->lblCurrentData1->setText(currentVersionList[i]);
            ui->lblNewData1->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData1->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave1BootSW:
        case upgradeAccessWrapper::WRESlave1BootSW:
        {
            ui->lblCurrentBoot1->setText(currentVersionList[i]);
            ui->lblNewBoot1->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot1->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave1AppSW:
        case upgradeAccessWrapper::WRESlave1AppSW:
        {
            ui->lblCurrentApplication1->setText(currentVersionList[i]);
            ui->lblNewApplication1->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication1->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave3NVDataSW:
        case upgradeAccessWrapper::WRESlave3NVDataSW:
        {
            ui->lblCurrentData3->setText(currentVersionList[i]);
            ui->lblNewData3->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData3->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave3BootSW:
        case upgradeAccessWrapper::WRESlave3BootSW:
        {
            ui->lblCurrentBoot3->setText(currentVersionList[i]);
            ui->lblNewBoot3->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot3->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave3AppSW:
        case upgradeAccessWrapper::WRESlave3AppSW:
        {
            ui->lblCurrentApplication3->setText(currentVersionList[i]);
            ui->lblNewApplication3->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication3->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave4NVDataSW:
        case upgradeAccessWrapper::WRESlave4NVDataSW:
        {
            ui->lblCurrentData4->setText(currentVersionList[i]);
            ui->lblNewData4->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData4->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave4BootSW:
        case upgradeAccessWrapper::WRESlave4BootSW:
        {
            ui->lblCurrentBoot4->setText(currentVersionList[i]);
            ui->lblNewBoot4->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot4->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave4AppSW:
        case upgradeAccessWrapper::WRESlave4AppSW:
        {
            ui->lblCurrentApplication4->setText(currentVersionList[i]);
            ui->lblNewApplication4->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication4->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave5NVDataSW:
        case upgradeAccessWrapper::WRESlave5NVDataSW:
        {
            ui->lblCurrentData5->setText(currentVersionList[i]);
            ui->lblNewData5->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData5->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave5BootSW:
        case upgradeAccessWrapper::WRESlave5BootSW:
        {
            ui->lblCurrentBoot5->setText(currentVersionList[i]);
            ui->lblNewBoot5->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot5->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave5AppSW:
        case upgradeAccessWrapper::WRESlave5AppSW:
        {
            ui->lblCurrentApplication5->setText(currentVersionList[i]);
            ui->lblNewApplication5->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication5->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave6NVDataSW:
        {
            ui->lblCurrentData6->setText(currentVersionList[i]);
            ui->lblNewData6->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewData6->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave6BootSW:
        {
            ui->lblCurrentBoot6->setText(currentVersionList[i]);
            ui->lblNewBoot6->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewBoot6->setStyleSheet("background-color: #99CCFF");
            break;
        }
        case upgradeAccessWrapper::WRSlave6AppSW:
        {
            ui->lblCurrentApplication6->setText(currentVersionList[i]);
            ui->lblNewApplication6->setText(newVersionList[i]);
            if(pendingList[i] == "1")
                ui->lblNewApplication6->setStyleSheet("background-color: #99CCFF");
            break;
        }
        default:
            break;
        }
    }
}
