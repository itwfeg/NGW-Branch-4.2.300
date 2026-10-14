/**
 * @file superCommInterface.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "superCommInterface.h"
#include "ui_superCommInterface.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

#include "../dialogs/filemandialog.h"
#include "filesystem.h"

superCommInterface::superCommInterface(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::superCommInterface)
{
    ui->setupUi(this);
    scaleScreen();
    expandTabs( ui->tabCommInterface );

    //----------------------------------------------------------------
    // Set the connections for when a checkbox is checked/un-checked.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //----------------------------------------------------------------
    connect( ui->ckClearOperatorTotals, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->ckClearOperatorChanges, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->ckClearHourlyTotals, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->ckClearProductAndClassTotals, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->ckClearCOOLSTotals, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkDontChangeCurrentCool, SIGNAL( clicked() ), this, SLOT( CoolSetupChanged() ) );
    connect( ui->ckIgnoreUltimaLabels, SIGNAL( clicked() ), this, SLOT( IgnoreUltimaLabelsChanged() ) );
    connect( ui->ckIncludePrefix, SIGNAL( clicked() ), this, SLOT( CoolSetupChanged() ) );

    connect( ui->chkAutoMerge, SIGNAL( toggled( bool ) ), this, SLOT( toggleUseAutomatic(bool) ) );
    connect( ui->chkAutoMerge, SIGNAL( toggled( bool ) ), this, SLOT( SetupChanged() ) );

    connect( ui->chkAutoMerge, SIGNAL( stateChanged(int) ), this, SLOT( checkboxChangedEnableTimeOfDay( int ) ) );

    connect( ui->chkMonday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkTuesday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkWednesday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkThursday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkFriday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkSaturday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect( ui->chkSunday, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );

    connect( ui->chkDaily, SIGNAL( toggled( bool ) ), this, SLOT( SetupChanged() ) );
    connect (ui->chkDaily, SIGNAL( toggled( bool ) ), this, SLOT( checkboxChangedEnableTimeOfDay( int ) ) );
    connect( ui->chkHourly, SIGNAL( toggled( bool ) ), this, SLOT( SetupChanged() ) );
    connect( ui->chkExecMergeAfterComm, SIGNAL( toggled( bool ) ), this, SLOT( SetupChanged() ) );

    connect(ui->chkDaily, SIGNAL( toggled( bool ) ), this, SLOT( manageStartTimeBox( bool ) ) );
    connect(ui->chkHourly, SIGNAL( toggled( bool  )), this, SLOT( manageNoStartTimeBox( bool ) ) );

    connect(ui->butScheduledStartAmPm, SIGNAL( clicked() ), this, SLOT( toggleScheduledStartAmPm() ) );
    isScheduledStartAmSelected = true;

    connect( ui->butMergeNow, SIGNAL( clicked() ), this, SLOT( mergeNowClicked() ) );
    connect( ui->butViewFiles, SIGNAL( clicked() ), this, SLOT( viewFilesButtonClicked() ) );

    //------------------------------------
    // Request the COMM Interface settings
    //------------------------------------
    session_->send( ConfigurationDev, CfgCommInterfaceRead );           // 112, 190
    session_->send( ConfigurationDev, CfgScaleManagementRead );         // 112, 200
    session_->send( ConfigurationDev, CfgCommIntfaceScheduledRead );    // 112, 205

    bSetupChanged = false;
    bCoolSetupChanged = false;
    bIgnoreUltimaLabels = false;

    setupValidators();

    translate();
}

superCommInterface::~superCommInterface()
{
    delete ui;
}

void superCommInterface::translate()
{
    ui->retranslateUi(this);

    update();
}

bool superCommInterface::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void superCommInterface::onSessionMessage( const SessionMessage& msg )
{
    // COMM Interface settings
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgCommInterfaceWrite == msg[1].toInt() ))
    {
        // 112,191
        DomCommInterfaceConfig m( msg );

        // Update UI checkboxes with the COMM Interface settings
        ui->ckClearOperatorTotals->setChecked((m.enableClearOperatorTotal().toInt() == 1 ? true : false));
        ui->ckClearOperatorChanges->setChecked((m.enableClearOperatorChange().toInt() == 1 ? true : false));
        ui->ckClearHourlyTotals->setChecked((m.enableClearHourlyTotal().toInt() == 1 ? true : false));
        ui->ckClearProductAndClassTotals->setChecked((m.enableClearProductClassTotals().toInt() == 1 ? true : false));
        ui->ckClearCOOLSTotals->setChecked((m.enableClearCoolTotal().toInt() == 1 ? true : false));

        bSetupChanged = false;
    }
    else if((ConfigurationDev == msg[0].toInt() ) && ( CfgScaleManagementWrite == msg[1].toInt() ))
    {
        // 112, 201
        DomScaleManagement m( msg );

        ui->chkDontChangeCurrentCool->setChecked((m.dontChangeCurrentCool().toInt() == 1 ? true : false));
        ui->ckIgnoreUltimaLabels->setChecked((m.ignoreUltimaLabels().toInt() == 1 ? true : false));
        ui->ckIncludePrefix->setChecked((m.enableUPTagPrefix().toInt() == 1 ? true : false));

        bCoolSetupChanged = false;
        bIgnoreUltimaLabels = false;
    }
    else if((ConfigurationDev == msg[0].toInt() ) && ( CfgCommIntfaceScheduledWrite == msg[1].toInt() ))
    {
        // SOF-4769: Scheduled Merge
        // 112,206
        DomCommInterfaceScheduledMergeConfig m( msg );

        if( m.useAutomaticMerges() == "1" )
        {
            ui->chkAutoMerge->setChecked( true );
            toggleUseAutomatic( true );
        }
        else
        {
            ui->chkAutoMerge->setChecked( false );
            toggleUseAutomatic( false );
        }

        ui->chkMonday->setChecked( m.monday() == "1" ? true : false );
        ui->chkTuesday->setChecked( m.tuesday() == "1" ? true : false );
        ui->chkWednesday->setChecked (m.wednesday() == "1" ? true : false );
        ui->chkThursday->setChecked (m.thursday() == "1" ? true : false );
        ui->chkFriday->setChecked( m.friday() == "1" ? true : false );
        ui->chkSaturday->setChecked( m.saturday() == "1" ? true : false );
        ui->chkSunday->setChecked (m.sunday() == "1" ? true : false );

        if( m.dailyUpdates() == "1" )
        {
            ui->chkDaily->setChecked(true);
            ui->leScheduledStartHour->setEnabled(true);
            ui->leScheduledStartMinutes->setEnabled(true);
        }
        else if( m.hourlyUpdates() == "1" )
        {
            ui->chkHourly->setChecked(true);
            ui->leScheduledStartHour->setEnabled(false);
            ui->leScheduledStartMinutes->setEnabled(false);
        }

        ui->chkExecMergeAfterComm->setChecked( m.execMergeAfterComm() == "1" ? true : false );

        // Parse Scheduled Start time and set hour/minutes/am-pm controls
        if( m.scheduleStartTime() == "00:00 AM" )
        {
            ui->leScheduledStartHour->setText( "00" );
            ui->leScheduledStartMinutes->setText( "00" );
            ui->butScheduledStartAmPm->setText( "AM" );

            ui->leScheduledStartHour->setEnabled( false );
            ui->leScheduledStartMinutes->setEnabled( false );
            ui->butScheduledStartAmPm->setEnabled( false );
        }
        else
        {
            QStringList tmpTime;
            if( m.scheduleStartTime().contains( ":" ) )
            {
                tmpTime = m.scheduleStartTime().split( ":", QString::KeepEmptyParts );
                ui->leScheduledStartHour->setText( tmpTime.at( 0 ) );

                QString tmpTime2 = tmpTime.at( 1 ).trimmed();
                if( tmpTime2.contains( " ") )
                {
                    // The time contains the AM/PM indicator
                    ui->leScheduledStartMinutes->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts ).at( 0 ) );
                    ui->butScheduledStartAmPm->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts).at( 1 ).toUpper() );
                }
                else if( tmpTime.at( 1 ).length() > 0 )
                {
                    // The time contains the minutes, but not the AM/PM indicator. Default to AM
                    ui->leScheduledStartMinutes->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts ).at( 0 ) );
                    ui->butScheduledStartAmPm->setText( "AM" );
                }
                else
                {
                    // There are no minutes.  Default to 00 minutes and AM
                    ui->leScheduledStartMinutes->setText( "00" );
                    ui->butScheduledStartAmPm->setText( "AM" );
                }

                ui->leScheduledStartHour->setEnabled( true );
                ui->leScheduledStartMinutes->setEnabled( true );
                ui->butScheduledStartAmPm->setEnabled( true );

                // Do not allow "00" to be entered
                scheduledStartHourValidator_->setBottom( 1 );
            }
            else
            {
                // Something wrong ... just set to default values and disable
                ui->leScheduledStartHour->setText( "00" );
                ui->leScheduledStartMinutes->setText( "00" );
                ui->butScheduledStartAmPm->setText( "AM" );

                ui->chkAutoMerge->setChecked( false );

                ui->leScheduledStartHour->setEnabled( false );
                ui->leScheduledStartMinutes->setEnabled( false );
                ui->butScheduledStartAmPm->setEnabled( false );
            }
        }

        ui->lblMergeHistoryData->setText( m.mergeHistoryDateTime() );

        isScheduledStartAmSelected = ( ui->butScheduledStartAmPm->text() == "AM" ? true : false );
    }
}

void superCommInterface::toggleUseAutomatic (bool isChecked )
{
   if(isChecked)
   {
       ui->chkHourly->setEnabled( true );
       ui->chkMonday->setEnabled( true );
       ui->chkTuesday->setEnabled( true );
       ui->chkWednesday->setEnabled( true );
       ui->chkThursday->setEnabled( true );
       ui->chkFriday->setEnabled( true );
       ui->chkSaturday->setEnabled( true );
       ui->chkSunday->setEnabled( true );
       ui->leScheduledStartHour->setEnabled( true );
       ui->leScheduledStartMinutes->setEnabled( true );
       ui->butScheduledStartAmPm->setEnabled( true );
       ui->gbStartTime->setEnabled( true );
   }
   else
   {
       ui->chkHourly->setEnabled( false );
       ui->chkMonday->setEnabled( false );
       ui->chkTuesday->setEnabled( false );
       ui->chkWednesday->setEnabled( false );
       ui->chkThursday->setEnabled( false );
       ui->chkFriday->setEnabled( false );
       ui->chkSaturday->setEnabled( false );
       ui->chkSunday->setEnabled( false );
       ui->leScheduledStartHour->setEnabled( false );
       ui->leScheduledStartMinutes->setEnabled( false );
       ui->butScheduledStartAmPm->setEnabled( false );
       ui->gbStartTime->setEnabled( false );
   }
}

void superCommInterface::manageStartTimeBox( bool isChecked )
{
    ui->leScheduledStartHour->setEnabled( isChecked );
    ui->leScheduledStartMinutes->setEnabled( isChecked );
    ui->butScheduledStartAmPm->setEnabled( isChecked );
}

void superCommInterface::manageNoStartTimeBox( bool isChecked )
{
    ui->leScheduledStartHour->setEnabled( !isChecked );
    ui->leScheduledStartMinutes->setEnabled( !isChecked );
    ui->butScheduledStartAmPm->setEnabled( !isChecked );
}

void superCommInterface::setupValidators()
{
    ui->leScheduledStartHour->setValidator(scheduledStartHourValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leScheduledStartHour->setInputWrapping(true);
    ui->leScheduledStartHour->setInputLeadingZeros(true);
    scheduledStartHourValidator_->setTop( 12 );
    scheduledStartHourValidator_->setBottom( 0 ); // Allow 00:00 to be entered to indicate disabled

    ui->leScheduledStartMinutes->setValidator(scheduledStartMinuteValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leScheduledStartMinutes->setInputLeadingZeros(true);
    ui->leScheduledStartMinutes->setInputWrapping(true);
    scheduledStartMinuteValidator_->setTop( 59 );
    scheduledStartMinuteValidator_->setBottom( 0 );
}

void superCommInterface::toggleScheduledStartAmPm()
{
    if( isScheduledStartAmSelected )
    {
        ui->butScheduledStartAmPm->setText( "PM" );
        isScheduledStartAmSelected = false;
    }
    else
    {
        ui->butScheduledStartAmPm->setText( "AM" );
        isScheduledStartAmSelected = true;
    }
}

void superCommInterface::checkboxChangedEnableTimeOfDay( int state )
{
    Q_UNUSED( state );

    bSetupChanged = true;

    if( (ui->chkAutoMerge->isChecked() && ui->chkDaily->isChecked() ) )
    {
        scheduledStartHourValidator_->setBottom( 1 );
        ui->leScheduledStartHour->setText( "12" );
        ui->leScheduledStartMinutes->setText( "00" );
        ui->butScheduledStartAmPm->setText( "AM" );

        ui->leScheduledStartHour->setEnabled( true );
        ui->leScheduledStartMinutes->setEnabled( true );
        ui->butScheduledStartAmPm->setEnabled( true );
    }
    else
    {
        scheduledStartHourValidator_->setBottom( 0 );

        // Set to default values when disabled
        ui->leScheduledStartHour->setText( "00" );
        ui->leScheduledStartMinutes->setText( "00" );
        ui->butScheduledStartAmPm->setText( "AM" );

        ui->leScheduledStartHour->setEnabled( false );
        ui->leScheduledStartMinutes->setEnabled( false );
        ui->butScheduledStartAmPm->setEnabled( false );
    }
}

void superCommInterface::mergeNowClicked()
{
    session_->send( SessionDev, snMergeNow );
}

void superCommInterface::viewFilesButtonClicked()
{
    //filemandialog *filemanDialog;
    //QString s1 = "merge";
    //QString s2 = "All files shown below will be merged:";
    //filemandialog *filemanDialog = new filemandialog( this, s1, s2 );
    //filemanDialog = new filemandialog( "merge", tr( "All files shown below will be merged:" ), mainWindow_ );
    //filemanDialog->exec();

    filemandialog *filemanDialog;
    filemanDialog = new filemandialog( filesystem::folder( folderType::Merge ), tr( "All files shown below will be merged:" ), mainWindow_ );
    filemanDialog->exec();
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superCommInterface::SetupChanged()
{
    bSetupChanged = true;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superCommInterface::CoolSetupChanged()
{
    bCoolSetupChanged = true;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superCommInterface::IgnoreUltimaLabelsChanged()
{
    bIgnoreUltimaLabels = true;
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void superCommInterface::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = QString(ui->ckClearProductAndClassTotals->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckClearHourlyTotals->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckClearOperatorTotals->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckClearCOOLSTotals->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckClearOperatorChanges->isChecked() ? "1" : "0");

        session_->send( ConfigurationDev, CfgCommInterfaceSave, param);         // 112,192


        // SOF-4769: Scheduled Merge
        if( this->ui->chkAutoMerge->isChecked() )
            param = "1";
        else
            param = "0";

        if( this->ui->chkMonday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if( this->ui->chkTuesday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if( this->ui->chkWednesday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if( this->ui->chkThursday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if(this->ui->chkFriday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if(this->ui->chkSaturday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if( this->ui->chkSunday->isChecked() )
            param += ",1";
        else
            param += ",0";

        if( this->ui->chkDaily->isChecked() )
            param += ",1";
        else
            param += ",0";

        param += ",";
        if( ui->chkDaily->isChecked() )
        {
            param += ui->leScheduledStartHour->text();
            param += ":";
            param += ui->leScheduledStartMinutes->text();
            param += " ";
            param += ui->butScheduledStartAmPm->text();
        }
        else
        {
            param +="00:00 AM";
        }

        if( this->ui->chkHourly->isChecked() )
            param += ",1";
        else
            param += ",0";

        if(this->ui->chkExecMergeAfterComm->isChecked() )
            param += ",1";
        else
            param += ",0";

        session_->send( ConfigurationDev, CfgCommIntfaceScheduledSave, param);  // 112,207

        bSetupChanged = false;
    }

    if (bCoolSetupChanged || bIgnoreUltimaLabels)
    {
        QString param = "";               // dont care about first param, something about ignoring Quantum\Ultima labels
        param += QString(ui->ckIgnoreUltimaLabels->isChecked() ? "1" : "0") + ","; // HTA-149 Yes we do care about Ultima Labels
        param += QString(ui->chkDontChangeCurrentCool->isChecked() ? "1" : "0");

        session_->send( ConfigurationDev, CfgScaleManagementSave, param);       // 112,202

        bCoolSetupChanged = false;
        bIgnoreUltimaLabels = false;
    }
}
