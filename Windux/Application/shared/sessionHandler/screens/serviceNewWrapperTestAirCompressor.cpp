

/* @file serviceNewWrapperTestAirCompressor.cpp
 * serviceNewWrapperTestAirCompressor
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper's air compressor
 */

#include "serviceNewWrapperTestAirCompressor.h"

#include "ui_serviceNewWrapperTestAirCompressor.h"

#include "../sessionmainwindow.h"
#include "../modes/servicemode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtCore>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestAirCompressor::serviceNewWrapperTestAirCompressor(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestAirCompressor),
    prevPsi_( 0 ),
    autoAirTestOn_( false ),
    regulatedValveOpen_( false ),
    dumpValveOpen_( false ),
    tankDrainValveOpen_( false ),
    beenThereDoneThat_( false )
{
    ui->setupUi(this);
    scaleScreen();

    //Setup Signals and Slots
    connect(ui->btnAirCompressorOn,      SIGNAL( clicked()), this, SLOT(airCompressorOn()));
    connect(ui->btnAirCompressorOff,     SIGNAL( clicked()), this, SLOT(airCompressorOff()));
    connect(ui->butOpenRegulatedValve,   SIGNAL( clicked()), this, SLOT(regulatedValveOpen()));
    connect(ui->butCloseRegulatedValve,  SIGNAL( clicked()), this, SLOT(regulatedValveClose()));
    connect(ui->butOpenDumpValve,        SIGNAL( clicked()), this, SLOT(dumpValveOpen()));
    connect(ui->butCloseDumpValve,       SIGNAL( clicked()), this, SLOT(dumpValveClose()));
    connect(ui->butOpenTankDrainValve,   SIGNAL( clicked()), this, SLOT(tankDrainValveOpen()));
    connect(ui->butCloseTankDrainValve,  SIGNAL( clicked()), this, SLOT(tankDrainValveClose()));

    //Translate the Page
    translate();

    ui->lblTankPressure->setText( "" );
    ui->lblRegulatedPressure->setText( "" );

    ui->lblNotes->setText( tr("Test status....waiting.....") );

    // request wrapper status
    session_->send( Wrapper, WrReqStatus );     // use this to intitially test high power when first coming to screen
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestAirCompressor::~serviceNewWrapperTestAirCompressor()
{
    // Do this in case it wasn't previously stopped
    if( autoAirTestOn_ )
    {
        QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(COMPRESSOR_OFF);
        session_->send(Wrapper, WrEvolTest, msg);
    }

    if( regulatedValveOpen_ )
    {
        QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(REGULATED_VALVE_CLOSE) + ",-1)";
        session_->send(Wrapper, WrEvolTest, msg);
    }

    if( dumpValveOpen_ )
    {
        QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(DUMP_VALVE_CLOSE) + ",-1)";
        session_->send(Wrapper, WrEvolTest, msg);
    }

    if( tankDrainValveOpen_ )
    {
        QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(TANK_DRAIN_VALVE_CLOSE) + ",-1)";
        session_->send(Wrapper, WrEvolTest, msg);
    }

    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::airCompressorOn()
{
    if( autoAirTestOn_ )
        return;

    autoAirTestOn_ = true;

    // ENUM is NOW CORRECT - Was previously only accepting a 1, not ENUM 0 like it should of been
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(COMPRESSOR_ON);
    session_->send(Wrapper, WrEvolTest, msg);

    highlightButtonGreen( ui->btnAirCompressorOn );
    highlightButtonGreen( ui->btnAirCompressorOff );

    ui->lblAndGo->setText( tr("*  Test started. Click Off to end test.") );

    // a. When "Auto Air Compressor on" is pressed "Regulated Pressure Valve open" and "Dump Valve Closed" are pressed.
    // b. When "Auto Air Compressor off" is pressed "Regulated Pressure Valve closed" is pressed.

    // turning on triggers some other buttons but only vi8sually. The action is taken care of with firmware
    regulatedValveOpen_ = true;
    highlightButtonGreen( ui->butOpenRegulatedValve );
    unhighlightButton( ui->butCloseRegulatedValve );

    dumpValveOpen_ = false;
    highlightButtonRed( ui->butCloseDumpValve );
    unhighlightButton( ui->butOpenDumpValve );

    prevPsi_ = 0;

    ui->lblNotes->setText( tr("The Auto Air Compressor Test was turned ON") );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::airCompressorOff()
{
    autoAirTestOn_ = false;
    prevPsi_ = 0;

    // ENUM is CORRECT - But Application or Firmware wants a 0. I should be sending 1=COMPRESSOR_OFF.
    ui->lblAndGo->setText( tr("*  Test ended. Click On to begin test.") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(COMPRESSOR_OFF);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonRed( ui->btnAirCompressorOff );
    unhighlightButton( ui->btnAirCompressorOn );

    unhighlightButton( ui->butOpenRegulatedValve );
    highlightButtonRed( ui->butCloseRegulatedValve );
    regulatedValveOpen_ = false;

    ui->lblNotes->setText( tr("The Auto Air Compressor Test was turned OFF") );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::regulatedValveOpen()
{
    if( regulatedValveOpen_ )
        return;

    regulatedValveOpen_ = true;

    ui->lblNotes->setText( tr("The Regulated Pressure Valve was Opened") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(REGULATED_VALVE_OPEN);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonGreen( ui->butOpenRegulatedValve );
    unhighlightButton( ui->butCloseRegulatedValve );

    ui->butOpenRegulatedValve->setDisabled( true );
    ui->butCloseRegulatedValve->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::regulatedValveClose()
{
    if( !regulatedValveOpen_ )
        return;

    regulatedValveOpen_ = false;

    ui->lblNotes->setText( tr("The Regulated Pressure Valve was Closed") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(REGULATED_VALVE_CLOSE);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonRed( ui->butCloseRegulatedValve );
    unhighlightButton( ui->butOpenRegulatedValve );

    ui->butOpenRegulatedValve->setDisabled( true );
    ui->butCloseRegulatedValve->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::dumpValveOpen()
{
    if( dumpValveOpen_ )
        return;

    dumpValveOpen_ = true;

    ui->lblNotes->setText( tr("The Dump Valve was Opened") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(DUMP_VALVE_OPEN);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonGreen( ui->butOpenDumpValve );
    unhighlightButton( ui->butCloseDumpValve );

    ui->butCloseDumpValve->setDisabled( true );
    ui->butOpenDumpValve->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::dumpValveClose()
{
    dumpValveOpen_ = false;

    ui->lblNotes->setText( tr("The Dump Valve was Closed") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(DUMP_VALVE_CLOSE);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonRed( ui->butCloseDumpValve );
    unhighlightButton( ui->butOpenDumpValve );

    ui->butOpenDumpValve->setDisabled( true );
    ui->butCloseDumpValve->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::tankDrainValveOpen()
{ 
    if( tankDrainValveOpen_ )
        return;

    tankDrainValveOpen_ = true;

    ui->lblNotes->setText( tr("The Tank Drain Valve was Opened") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(TANK_DRAIN_VALVE_OPEN);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonGreen( ui->butOpenTankDrainValve );
    unhighlightButton( ui->butCloseTankDrainValve );

    ui->butOpenTankDrainValve->setDisabled( true );
    ui->butCloseTankDrainValve->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::tankDrainValveClose()
{
    if( !tankDrainValveOpen_ )
        return;

    tankDrainValveOpen_ = false;

    ui->lblNotes->setText( tr("The Tank Drain Valve was Closed") );
    QString msg = QString::number(WrAirCompressorTest) + "," + QString::number(TANK_DRAIN_VALVE_CLOSE);
    session_->send(Wrapper, WrEvolTest, msg);
    highlightButtonRed( ui->butCloseTankDrainValve );
    unhighlightButton( ui->butOpenTankDrainValve );

    ui->butOpenTankDrainValve->setDisabled( true );
    ui->butCloseTankDrainValve->setDisabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::highlightButtonRed(QPushButton *actionBtn)
{
    actionBtn->setStyleSheet("background-color: red");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::highlightButtonGreen(QPushButton *actionBtn)
{
    actionBtn->setStyleSheet("background-color: green");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::unhighlightButton(QPushButton* actionBtn)
{
    actionBtn->setStyleSheet("");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestAirCompressor::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if (( Wrapper == device ) && ( WrEvolTestComplete == event ))
    {
        DomEvolWrapperTestComplete m(msg);

        int valve_tested = m.subTestNum().toInt();     // this is the test just completed

        switch( valve_tested )
        {
            case COMPRESSOR_ON:
                unhighlightButton( ui->btnAirCompressorOff );
                ui->btnAirCompressorOff->setDisabled( false );
                break;

            case COMPRESSOR_OFF:
                ui->btnAirCompressorOn->setDisabled( false );

                if( autoAirTestOn_ )
                    airCompressorOff();

                autoAirTestOn_ = false;

                break;

            case COMPRESSOR_TANK_DRAIN_PULSE:
                break;          // NOT USED

            case REGULATED_VALVE_OPEN:
                ui->butOpenRegulatedValve->setDisabled( false );
                ui->butCloseRegulatedValve->setDisabled( false );
                break;

            case REGULATED_VALVE_CLOSE:
                ui->butOpenRegulatedValve->setDisabled( false );
                ui->butCloseRegulatedValve->setDisabled( false );
                break;

            case DUMP_VALVE_OPEN:
                ui->butOpenDumpValve->setDisabled( false );
                ui->butCloseDumpValve->setDisabled( false );
                break;

            case DUMP_VALVE_CLOSE:
                ui->butOpenDumpValve->setDisabled( false );
                ui->butCloseDumpValve->setDisabled( false );
                break;

            case TANK_DRAIN_VALVE_OPEN:
                ui->butOpenTankDrainValve->setDisabled( false );
                ui->butCloseTankDrainValve->setDisabled( false );
                break;

            case TANK_DRAIN_VALVE_CLOSE:
                ui->butOpenTankDrainValve->setDisabled( false );
                ui->butCloseTankDrainValve->setDisabled( false );
                break;
        }
    }
    else if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        DomEvolWrapperEventCapture m( msg );

        if ( DomEvolWrapperEventCapture::TankPressureReadingEvent == m.captureType() )
        {
            int tankPsi = m.value();

            ui->lblTankPressure->setText( QString::number( tankPsi ) );


            if( autoAirTestOn_ )
            {
                if( tankPsi >= 120)
                {
                    if( 120 == prevPsi_ )
                        return;

                    prevPsi_ = 120;
                    ui->btnAirCompressorOn->setEnabled( false );
                    ui->lblNotes->setText( tr("When the psi for the tank reaches 120 psi the compressor automatically shuts off.") );
                }
                else if( tankPsi >= 90 )
                {
                    if( 90 == prevPsi_ )
                        return;

                    prevPsi_ = 90;
                    ui->btnAirCompressorOn->setEnabled( false );
                    ui->lblNotes->setText( tr("When the psi for the tank exceeds 90 psi the On button is automatically disabled.") );
                }
                else
                {
                    if( 89 == prevPsi_ )
                        return;

                    prevPsi_ = 89;
                    ui->btnAirCompressorOn->setEnabled( true );
                    ui->lblNotes->setText( tr("When the psi is less than 90 psi the On button is available.") );
                }
            }
            else
            {
                if( tankPsi <= 90 )
                {
                    ui->lblNotes->setText( tr("When the psi is less than 90 psi the On button is available.") );
                    ui->btnAirCompressorOn->setEnabled( true );
                }
            }
        }
        else if ( DomEvolWrapperEventCapture::RegulatedPressureReadingEvent == m.captureType() )
        {
            ui->lblRegulatedPressure->setText( QString::number( m.value() ) );
        }
    }
    else if(Wrapper == msg[0].toInt() && WrEvolStatus == msg[1].toInt())
    {

        if( beenThereDoneThat_ )
            return;

        beenThereDoneThat_ = true;

        evolWrapperStatus_ = msg;

        if ( evolWrapperStatus_.mainPower() )
        {
            // if main power is on when first initially entering the screen, so its like pressing START/OPEN.
            // Auto air compressor: On(Green) && Regulated Pressure: Open(Green)
            highlightButtonGreen( ui->btnAirCompressorOn );
            highlightButtonGreen( ui->butOpenRegulatedValve );
            highlightButtonRed( ui->butCloseDumpValve );
            highlightButtonRed( ui->butCloseTankDrainValve );

            ui->lblAndGo->setText( tr("*  High Power ON. Click OFF to end test.") );
            ui->lblNotes->setText( tr("The Auto Air Compressor Test is RUNNING") );

            dumpValveOpen_ = false;
            autoAirTestOn_ = true;
            regulatedValveOpen_ = true;
            prevPsi_ = 0;
        }
        else
        {
            // main power is not off:
            // Auto air compressor: Off(Red) && Regulated Pressure: Closed(Red)
            highlightButtonRed( ui->btnAirCompressorOff );
            highlightButtonRed( ui->butCloseRegulatedValve );
            highlightButtonRed( ui->butCloseDumpValve );
            highlightButtonRed( ui->butCloseTankDrainValve );

            autoAirTestOn_ = false;
            prevPsi_ = 0;
            regulatedValveOpen_ = false;
        }
    }
    else if( ( Error == device ) && ( Wrapper == event ) )
    {
        // Wrapper - if users get an error while buttons might be disabled
        ui->butOpenRegulatedValve->setDisabled( false );
        ui->butCloseRegulatedValve->setDisabled( false );
        ui->butOpenDumpValve->setDisabled( false );
        ui->butCloseDumpValve->setDisabled( false );
        ui->butOpenTankDrainValve->setDisabled( false );
        ui->butCloseTankDrainValve->setDisabled( false );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestAirCompressor::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
