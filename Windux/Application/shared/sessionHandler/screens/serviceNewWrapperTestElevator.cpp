
/* @file serviceNewWrapperTestElevator.cpp
 * serviceNewWrapperInterlocks
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper elevator
 */

#include "serviceNewWrapperTestElevator.h"

#include "ui_serviceNewWrapperTestElevator.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestElevator::serviceNewWrapperTestElevator(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestElevator)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    connect(ui->btnElevatorCycle, SIGNAL( clicked()), this, SLOT(cycleElevator()));

    //Update and Initialize the Page
    initialize();
    translate();

    //Send requests for wrapper sys info and sensors msgs
    session_->send(Wrapper, WrReqEvolSystemInfo);
    session_->send(Wrapper, WrReqSensors);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestElevator::initialize()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestElevator::~serviceNewWrapperTestElevator()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestElevator::setSensorState(QLabel* label, bool open)
{
    static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
    static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

    label->setStyleSheet( open ? greenStyle : redStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestElevator::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestElevator::cycleElevator()
{
    deviceTest->sendWrapperTest(session_, ui->btnElevatorCycle, WrElevatorTest, -1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestElevator::onSessionMessage( const SessionMessage& msg )
{
    if(Wrapper == msg[0].toInt() && WrEvolSensors == msg[1].toInt())
    {
        // we just asked for and received the wrapper's sensor message so update the GUI
        DomEvolWrapperSensors m(msg);

        // Set Sensor States
        setSensorState(ui->lblElevatorSensor, m.elevatorHome());
        setSensorState(ui->lblPackageSensor, m.elevatorPkg());
    }
    else if(Wrapper == msg[0].toInt() && WrEvolTestComplete == msg[1].toInt())
    {
        DomEvolWrapperTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestElevator::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
