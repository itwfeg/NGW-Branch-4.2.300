

/* @file serviceNewWrapperTestGripper.cpp
 * serviceNewWrapperTestGripper
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief This file contains all the functions necessary for testing the wrapper's film supply
 */

#include "serviceNewWrapperTestGripper.h"

#include "ui_serviceNewWrapperTestGripper.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestGripper::serviceNewWrapperTestGripper(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestGripper)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    //Gripper
    connect(ui->btnGripperIn, SIGNAL( clicked()), this, SLOT(gripperIn()));
    connect(ui->btnGripperHome, SIGNAL( clicked()), this, SLOT(gripperHome()));
    connect(ui->btnGripperOut, SIGNAL( clicked()), this, SLOT(gripperOut()));

    //Narrow Gripper
    connect(ui->btnNarrowGripperOpen, SIGNAL( clicked()), this, SLOT(narrowGripperOpen()));
    connect(ui->btnNarrowGripperClose, SIGNAL( clicked()), this, SLOT(narrowGripperClose()));

    //Wide Gripper
    connect(ui->btnWideGripperOpen, SIGNAL( clicked()), this, SLOT(wideGripperOpen()));
    connect(ui->btnWideGripperClose, SIGNAL( clicked()), this, SLOT(wideGripperClose()));

    //Film Select Tests
    connect(ui->btnFilmSelectTestUpper, SIGNAL( clicked()), this, SLOT(filmSelectTestUpper()));
    connect(ui->btnFilmSelectTestLower, SIGNAL( clicked()), this, SLOT(filmSelectTestLower()));

    //Cycle and Finish
    connect(ui->btnCycleGripper, SIGNAL( clicked()), this, SLOT(cycleGripper()));
    connect(ui->btnFinish, SIGNAL( clicked()), this, SLOT(finish()));


    //Send requests for wrapper sys info and sensors msgs
    session_->send(Wrapper, WrReqEvolSystemInfo);
    session_->send(Wrapper, WrReqSensors);

    //Update and Initialize the Page
    initialize();
    translate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::initialize()
{
    gripperIsCycling = false;

    //Clear out all the sensor states
    setSensorState(ui->lblHomeSensor, false);

    //Disable the finish button
    disableFinish();

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        ui->lblProductNumber_6->hide();
        ui->btnFilmSelectTestUpper->hide();
        ui->btnFilmSelectTestLower->hide();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestGripper::~serviceNewWrapperTestGripper()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::setSensorState(QLabel* label, bool open)
{
    static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
    static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

    label->setStyleSheet( open ? greenStyle : redStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestGripper::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestGripper::onScreenChanging()
{
    //If the gripper is cycling, tell it to finish before leaving
    if( gripperIsCycling )
        finish();

    //Disable finish
    disableFinish();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::gripperIn()
{
    deviceTest->sendWrapperTest(session_, ui->btnGripperIn, WrGripperTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::gripperHome()
{
    deviceTest->sendWrapperTest(session_, ui->btnGripperHome, WrGripperTest, 2, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::gripperOut()
{
    deviceTest->sendWrapperTest(session_, ui->btnGripperOut, WrGripperTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::narrowGripperOpen()
{
    deviceTest->sendWrapperTest(session_, ui->btnNarrowGripperOpen, WrGripperNarrowTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::narrowGripperClose()
{
    deviceTest->sendWrapperTest(session_, ui->btnNarrowGripperClose, WrGripperNarrowTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::wideGripperOpen()
{
    deviceTest->sendWrapperTest(session_, ui->btnWideGripperOpen, WrGripperWideTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::wideGripperClose()
{
    deviceTest->sendWrapperTest(session_, ui->btnWideGripperClose, WrGripperWideTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::filmSelectTestUpper()
{
    deviceTest->sendWrapperTest(session_, ui->btnFilmSelectTestUpper, WrFilmSelectTest, 0, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::filmSelectTestLower()
{
    deviceTest->sendWrapperTest(session_, ui->btnFilmSelectTestLower, WrFilmSelectTest, 1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::cycleGripper()
{
    deviceTest->sendWrapperTest(session_, ui->btnCycleGripper, WrGripperTest, 3, -1);
    gripperIsCycling = true;
    enableFinish();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::finish()
{
    deviceTest->sendWrapperTest(session_, ui->btnFinish, WrGripperTest, 4, -1);
    gripperIsCycling = false;
    disableFinish();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::disableLowerFilm()
{
    ui->btnFilmSelectTestLower->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::enableLowerFilm()
{
    ui->btnFilmSelectTestLower->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::disableFinish()
{
    ui->btnFinish->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::enableFinish()
{
    ui->btnFinish->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestGripper::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    if (( Wrapper == device ) && ( WrEvolSensors == event ))
    {
        // we just asked for and received the wrapper's sensor message so update the GUI

        //Save parsing of comma delimited message in case it ever changes
        DomEvolWrapperSensors m( msg );

        // Set Sensor States
        setSensorState( ui->lblHomeSensor, m.gripperHome() );
        setSensorState( ui->lblNarrowGripperSensor, m.gripperNarrow() );
        setSensorState( ui->lblRightGripperSensor, m.gripperRight() );
        setSensorState( ui->lblLeftGripperSensor, m.gripperLeft() );
    }
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        //Save parsing of comma delimited message in case it ever changes
        DomEvolWrapperSystemInformation m( msg );

        //Disable the Lower Brakes and SelectorPosition buttons if we have an EWS Wrapper
        if ( m.isSingleRoll() )
            disableLowerFilm();
        else
            enableLowerFilm();
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
            ui->lblRegulatedAirPressure->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperNarrowOpenEvent == m.captureType() )
            ui->lblNarrowOpen->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperNarrowCloseEvent == m.captureType() )
            ui->lblNarrowClose->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperWideRightOpenEvent == m.captureType() )
            ui->lblRightOpen->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperWideRightCloseEvent == m.captureType() )
            ui->lblRightClose->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperWideLeftOpenEvent == m.captureType() )
            ui->lblLeftOpen->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::GripperWideLeftCloseEvent == m.captureType() )
            ui->lblLeftClose->setText( QString::number( m.value() ) );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestGripper::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
