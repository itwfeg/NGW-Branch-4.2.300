

/* @file serviceNewWrapperTestFilmSupply.cpp
 * serviceNewWrapperTestFilmSupply
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper's film supply
 */

#include "serviceNewWrapperTestFilmSupply.h"

#include "ui_serviceNewWrapperTestFilmSupply.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestFilmSupply::serviceNewWrapperTestFilmSupply(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestFilmSupply)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    connect(ui->btnSelectorPositionUpper, SIGNAL( clicked()), this, SLOT(selectorPositionUpper()));
    connect(ui->btnSelectorPositionLower, SIGNAL( clicked()), this, SLOT(selectorPositionLower()));
    connect(ui->btnUpperBrakeOn, SIGNAL( clicked()), this, SLOT(upperBrakeOn()));
    connect(ui->btnUpperBrakeOff, SIGNAL( clicked()), this, SLOT(upperBrakeOff()));
    connect(ui->btnLowerBrakeOn, SIGNAL( clicked()), this, SLOT(lowerBrakeOn()));
    connect(ui->btnLowerBrakeOff, SIGNAL( clicked()), this, SLOT(lowerBrakeOff()));
    connect(ui->btnFilmSensorTestOn, SIGNAL( clicked()), this, SLOT(filmSensorTestOn()));
    connect(ui->btnFilmSensorTestOff, SIGNAL( clicked()), this, SLOT(filmSensorTestOff()));

    //Update and Initialize the Page
    initialize();
    translate();

    //Send requests for wrapper sys info and sensors msgs
    session_->send(Wrapper, WrReqEvolSystemInfo);
    session_->send(Wrapper, WrReqSensors);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::initialize()
{
    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        disableSelectorPosition();
        disableLowerBrakes();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestFilmSupply::~serviceNewWrapperTestFilmSupply()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::setSensorState(QLabel* label, bool open)
{
    static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
    static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

    label->setStyleSheet( open ? greenStyle : redStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestFilmSupply::onScreenChanging()
{
    //If a test is running, cancel it
    deviceTest->cancelCurrentTest();

    //Turn off filmSensorTest if it was running
    if(sensorTestIsRunning)
    {
        filmSensorTestOff();
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::disableLowerBrakes()
{
    ui->btnLowerBrakeOn->setEnabled(false);
    ui->btnLowerBrakeOff->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::enableLowerBrakes()
{
    // PJR-145:  Wrap-only
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        ui->btnLowerBrakeOn->setEnabled(true);
        ui->btnLowerBrakeOff->setEnabled(true);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::disableSelectorPosition()
{
    ui->btnSelectorPositionUpper->setEnabled(false);
    ui->btnSelectorPositionLower->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::enableSelectorPosition()
{
    // PJR-145:  Wrap-only
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        ui->btnSelectorPositionUpper->setEnabled(true);
        ui->btnSelectorPositionLower->setEnabled(true);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::selectorPositionUpper()
{
    deviceTest->sendWrapperTest(session_, ui->btnSelectorPositionUpper, WrFilmSelectTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::selectorPositionLower()
{
    deviceTest->sendWrapperTest(session_, ui->btnSelectorPositionLower, WrFilmSelectTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::upperBrakeOn()
{
    deviceTest->sendWrapperTest(session_, ui->btnUpperBrakeOn, WrUpperFilmBrakeTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::upperBrakeOff()
{
    deviceTest->sendWrapperTest(session_, ui->btnUpperBrakeOff, WrUpperFilmBrakeTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::lowerBrakeOn()
{
    deviceTest->sendWrapperTest(session_, ui->btnLowerBrakeOn, WrLowerFilmBrakeTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::lowerBrakeOff()
{
    deviceTest->sendWrapperTest(session_, ui->btnLowerBrakeOff, WrLowerFilmBrakeTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::filmSensorTestOn()
{
    sensorTestIsRunning = true;
    deviceTest->sendWrapperTest(session_, ui->btnFilmSensorTestOn, WrFilmSensorTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::filmSensorTestOff()
{
    deviceTest->sendWrapperTest(session_, ui->btnFilmSensorTestOff, WrFilmSensorTest, 0, -1);
    sensorTestIsRunning = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestFilmSupply::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    if (( Wrapper == device ) && ( WrEvolSensors == event ))
    {
        // we just asked for and received the wrapper's sensor message so update the GUI
        DomEvolWrapperSensors m( msg );

        // Set Sensor States
        setSensorState( ui->lblFilmSensor, m.filmRoll() );
        setSensorState( ui->lblFilmSelectorUp, m.filmSelectorUp() );
        setSensorState( ui->lblFilmSelectorDown, m.filmSelectorDown() );
        setSensorState( ui->lblFilmSelectorLever, m.filmSelectorLever() );
    }
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        DomEvolWrapperSystemInformation m( msg );

        //Disable the Lower Brakes and SelectorPosition buttons if we have an EWS Wrapper
        if ( m.isSingleRoll() )
        {
            disableLowerBrakes();
            disableSelectorPosition();
        }
        else
        {
            enableLowerBrakes();
            enableSelectorPosition();
        }
    }
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ))
    {
        DomEvolWrapperTestComplete m( msg );

        deviceTest->testComplete(m.testNum().toInt());
    }
    else if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        DomEvolWrapperEventCapture m( msg );

        if ( DomEvolWrapperEventCapture::RegulatedPressureReadingEvent == m.captureType() )
            ui->lblRegulatedPressure->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::FilmSelectorUpEvent == m.captureType() )
            ui->lblSelectorUpTime->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::FilmSelectorDownEvent == m.captureType() )
            ui->lblSelectorDownTime->setText( QString::number( m.value() ) );
    }
}
