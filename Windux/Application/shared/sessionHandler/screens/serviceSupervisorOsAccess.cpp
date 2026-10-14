/**
 * @file serviceSupervisorOsAccess.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceSupervisorOsAccess.h"
#include "ui_serviceSupervisorOsAccess.h"

#include <Session.h>
#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"

serviceSupervisorOsAccess::serviceSupervisorOsAccess(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceSupervisorOsAccess)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    //-----------------------------------------------------
    // Set the connection for when the checkbox is changed.
    // Call a method save the change immediately.
    //-----------------------------------------------------
    connect( ui->ckSupervisorOsAccess, SIGNAL( clicked() ), this, SLOT( SaveCheckboxSetting() ) );
    connect(ui ->ckSupervisorDotWearAccess, SIGNAL( clicked() ), this, SLOT ( SaveCheckboxSetting() ) );
    connect(ui ->ckOperatorDotWearAccess, SIGNAL( clicked() ), this, SLOT ( SaveCheckboxSetting() ) );

    translate();
}

serviceSupervisorOsAccess::~serviceSupervisorOsAccess()
{
    delete ui;
}

void serviceSupervisorOsAccess::translate()
{
    ui->retranslateUi(this);

    update();
}

//------------------------------------------------------------------------------------
// Set the checkbox based on the value of rebootUnprotectedAllowed in MadpScaleConfig.
//------------------------------------------------------------------------------------
void serviceSupervisorOsAccess::initialize()
{
    //--------------------------------------------------------------
    // NOTE:    This is set from a global Scale Config variable
    //          and NOT as a result of a message requesting data and
    //          then processing of a return message.
    //--------------------------------------------------------------

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString rebootOsSetting = settings->rebootUnprotectedAllowedRaw();
    //qDebug() << "os config is " << settings->rebootUnprotectedAllowedRaw();

    ui->ckSupervisorOsAccess->setChecked((rebootOsSetting == "1" ? true : false));

    // SOF-3695: Only display Dot Wear selections for HTm scale model
    const QString scaleModel( settings->scaleName() );
    if( scaleModel.size() >= 3 && scaleModel.toUpper().startsWith( "HTM") )
    {
        ui->ckSupervisorDotWearAccess->setChecked((settings->allowSupervisorDotWearRaw() == "1" ? true : false));
        ui->ckOperatorDotWearAccess->setChecked((settings->allowOperatorDotWearRaw() == "1" ? true : false));
    }
    else
    {
        ui->ckSupervisorDotWearAccess->setVisible( false );
        ui->ckOperatorDotWearAccess->setVisible( false );
    }
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void serviceSupervisorOsAccess::SaveCheckboxSetting()
{
    QString param;

    param = "," + QString::number( fieldOffsetCfgRebootUnprotectedAllowed );
    param += "," + QString(ui->ckSupervisorOsAccess->isChecked() ? "1" : "0");
    session_->send( ConfigurationDev, CfgSaveSingleField, param );

    param = "," + QString::number( fieldOffsetCfgAllowSupervisorDotWear );
    param += "," + QString(ui->ckSupervisorDotWearAccess->isChecked() ? "1" : "0");
    session_->send( ConfigurationDev, CfgSaveSingleField, param );

    param = "," + QString::number( fieldOffsetCfgAllowOperatorDotWear );
    param += "," + QString(ui->ckOperatorDotWearAccess->isChecked() ? "1" : "0");
    session_->send( ConfigurationDev, CfgSaveSingleField, param );
}
