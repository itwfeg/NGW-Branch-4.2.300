/**
 * @file superScaleManagement.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "superScaleManagement.h"
#include "ui_superScaleManagement.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

superScaleManagement::superScaleManagement(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::superScaleManagement)
{
    ui->setupUi(this);
    scaleScreen();

    //----------------------------------------------------------------
    // Set the connections for when a checkbox is checked/un-checked.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //----------------------------------------------------------------
    connect( ui->ckIgnoreUltimaLabels, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->ckDontChangeCurrCool, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );

    //------------------------------------
    // Request the COMM Interface settings
    //------------------------------------
    session_->send( ConfigurationDev, CfgCommInterfaceRead );

    bSetupChanged = false;

    translate();
}

superScaleManagement::~superScaleManagement()
{
    delete ui;
}

void superScaleManagement::translate()
{
    ui->retranslateUi(this);

    update();
}

bool superScaleManagement::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void superScaleManagement::onSessionMessage( const SessionMessage& msg )
{
    // COMM Interface settings
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgCommInterfaceWrite == msg[1].toInt() ))
    {
        DomScaleManagement m( msg );

        // Update UI checkboxes with the COMM Interface settings
        ui->ckIgnoreUltimaLabels->setChecked((m.ignoreUltimaLabels().toInt() == 1 ? true : false));
        ui->ckDontChangeCurrCool->setChecked((m.dontChangeCurrentCool().toInt() == 1 ? true : false));

        bSetupChanged = false;
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superScaleManagement::SetupChanged()
{
    bSetupChanged = true;
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void superScaleManagement::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = QString(ui->ckIgnoreUltimaLabels->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckDontChangeCurrCool->isChecked() ? "1" : "0");

        session_->send( ConfigurationDev, CfgCommInterfaceSave, param);

        bSetupChanged = false;
    }
}
