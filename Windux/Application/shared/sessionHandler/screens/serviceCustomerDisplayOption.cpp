/**
 * @file serviceCustomerDisplayOption.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceCustomerDisplayOption.h"
#include "ui_serviceCustomerDisplayOption.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"


serviceCustomerDisplayOption::serviceCustomerDisplayOption(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceCustomerDisplayOption)
{
    ui->setupUi(this);
    scaleScreen();

    ignoreSignals_ = true;

    //----------------------------------------------------
    // Request info for the "Allow printing ... " checkbox
    //----------------------------------------------------
    session_->send( ConfigurationDev, CfgReadSingleField, "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) );

    //------------------------------------------------------
    // Set the connections for when the checkbox is changed.
    //------------------------------------------------------
    connect(ui->ckAllowPrintingWithoutGCD, SIGNAL( clicked() ), this, SLOT( AllowPrintingWithoutGcdChanged() ) );

    translate();

    ignoreSignals_ = false;
}

serviceCustomerDisplayOption::~serviceCustomerDisplayOption()
{
    delete ui;
}

void serviceCustomerDisplayOption::translate()
{
    ui->retranslateUi(this);

    update();
}

void serviceCustomerDisplayOption::onSessionMessage( const SessionMessage& msg )
{
    //------------------------------------------------------
    // Processes message for allowing printing without a GCD
    //------------------------------------------------------
    if (( ConfigurationDev == msg[0].toInt() ) && ( CfgWriteSingleField == msg[1].toInt() ))
    {
        DomConfigWriteSingleField m( msg );

        if( m.offsetIndex() == QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay ) )  // Make sure this is message that was requested
        {
//            originalValue = m.newValue();
//            ui->ckAllowPrintingWithoutGCD->setChecked( (originalValue == "1") ? true : false );

            // Do I need to parse this instead???
            QVector<QString> newValueList = TokenParser::splitStringTokens( m.newValue() );
            if(newValueList.count() == 1)
            {
                originalValue = newValueList.at( 0 );
                ui->ckAllowPrintingWithoutGCD->setChecked( (originalValue == "1") ? true : false );
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Slot for when "Allow Printing Without Customer Display" checkbox is changed.
// Need to send message to save change as soon as it occurs
//-----------------------------------------------------------------------------
void serviceCustomerDisplayOption::AllowPrintingWithoutGcdChanged()
{
    if ( !ignoreSignals_ )
    {
        QString param;

        param = "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay );
        param += ",\"" + QString(ui->ckAllowPrintingWithoutGCD->isChecked() ? "1" : "0") + "\"";
        param += "," + originalValue;

        session_->send( ConfigurationDev, CfgSaveSingleField, param);
    }
}
