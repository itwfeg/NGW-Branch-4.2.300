/**
 * @file serviceRemoveLabels.cpp
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @author Job E. Crew
 */

#include "serviceRemoveLabels.h"
#include "ui_serviceRemoveLabels.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"
#include "../dialogs/messageboxdialog.h"

#include "../sessionmainwindow.h"

// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"

serviceRemoveLabels::serviceRemoveLabels(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceRemoveLabels)
{
    ui->setupUi(this);
    scaleScreen();

    isUpdating = true;

    session_->send(ConfigurationDev, CfgSupportedLabelsRead);              // 112,55 -> get supported labels

    translate();
}

serviceRemoveLabels::~serviceRemoveLabels()
{
    delete ui;
}

void serviceRemoveLabels::translate()
{
    ui->retranslateUi(this);

    update();
}

bool serviceRemoveLabels::onScreenChanging()
{
    if( isUpdating )
    {
        // SOF-3161: Need to wait until the checkboxes have been populated
        MessageBoxDialog dialog( mainWindow_ );

        dialog.setText( tr( "Please wait until the screen has been updated before exiting." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();

        return false;
    }

    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

void serviceRemoveLabels::updateCheckboxes(const SessionMessage &msg)
{
    MadpSupportedLabels m( msg );

    ui->chkLabelSize_1_000->setChecked( m.labelSize_1_000() );
    ui->chkLabelSize_1_500->setChecked( m.labelSize_1_500() );
    ui->chkLabelSize_1_750->setChecked( m.labelSize_1_750() );
    ui->chkLabelSize_2_000->setChecked( m.labelSize_2_000() );
    ui->chkLabelSize_2_375->setChecked( m.labelSize_2_375() );
    ui->chkLabelSize_2_500->setChecked( m.labelSize_2_500() );
    ui->chkLabelSize_3_000->setChecked( m.labelSize_3_000() );
    ui->chkLabelSize_3_500->setChecked( m.labelSize_3_500() );
    ui->chkLabelSize_4_000->setChecked( m.labelSize_4_000() );
    ui->chkLabelSize_4_500->setChecked( m.labelSize_4_500() );
    ui->chkLabelSize_5_000->setChecked( m.labelSize_5_000() );
    ui->chkLabelSize_5_500->setChecked( m.labelSize_5_500() );
    ui->chkLabelSize_6_000->setChecked( m.labelSize_6_000() );
    ui->chkLabelSize_6_500->setChecked( m.labelSize_6_500() );
    ui->chkLabelSize_7_000->setChecked( m.labelSize_7_000() );
    ui->chkLabelSize_7_500->setChecked( m.labelSize_7_500() );
    ui->chkLabelSize_8_000->setChecked( m.labelSize_8_000() );
    ui->chkLabelSize_8_500->setChecked( m.labelSize_8_500() );
    ui->chkLabelSize_9_000->setChecked( m.labelSize_9_000() );
    ui->chkLabelSize_9_500->setChecked( m.labelSize_9_500() );

    update();

    isUpdating = false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRemoveLabels::onSessionMessage( const SessionMessage& msg )
{
    if( ConfigurationDev == msg[0].toInt() && CfgSupportedLabelsWrite == msg[1].toInt() )
    {
        updateCheckboxes(msg);
    }
}

void serviceRemoveLabels::saveDataOnExit()
{
    QString msg = "";

    msg =  ui->chkLabelSize_1_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_1_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_1_750->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_2_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_2_375->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_2_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_3_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_3_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_4_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_4_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_5_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_5_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_6_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_6_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_7_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_7_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_8_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_8_500->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_9_000->isChecked() == true ? "1" : "0";
    msg += ",";
    msg += ui->chkLabelSize_9_500->isChecked() == true ? "1" : "0";

    session_->send( ConfigurationDev, CfgSupportedLabelsWrite, msg );       // 112,56
    session_->send( ConfigurationDev, CfgSupportedLabelsSave, msg );        // 112,57
}
