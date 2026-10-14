

/* @file serviceNewWrapperTestUpperPusher.cpp
 * serviceNewWrapperTestUpperPusher
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper's upper pusher
 */

#include "serviceNewWrapperTestUpperPusher.h"

#include "ui_serviceNewWrapperTestUpperPusher.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestUpperPusher::serviceNewWrapperTestUpperPusher(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestUpperPusher)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    connect(ui->btnCycleUpperPusher, SIGNAL( clicked()), this, SLOT(cycleUpperPusher()));

    //Update and Initialize the Page
    initialize();
    translate();

    //Request a wrapper sensors msg
    session_->send(Wrapper, WrReqSensors);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUpperPusher::initialize()
{
    //Clear out all the sensor states
    setSensorState(ui->lblHomeSensor, false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestUpperPusher::~serviceNewWrapperTestUpperPusher()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUpperPusher::setSensorState(QLabel* label, bool open)
{
    static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
    static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

    label->setStyleSheet( open ? greenStyle : redStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestUpperPusher::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUpperPusher::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestUpperPusher::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUpperPusher::cycleUpperPusher()
{
    deviceTest->sendWrapperTest(session_, ui->btnCycleUpperPusher, WrPusherTest, -1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUpperPusher::onSessionMessage( const SessionMessage& msg )
{
    if(Wrapper == msg[0].toInt() && WrEvolSensors == msg[1].toInt())
    {
        // we just asked for and received the wrapper's sensor message so update the GUI
        DomEvolWrapperSensors m(msg);

        // Set Sensor States
        setSensorState(ui->lblHomeSensor, m.pusherHome());
    }
    else if(Wrapper == msg[0].toInt() && WrEvolTestComplete == msg[1].toInt())
    {
        DomEvolWrapperTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestUpperPusher::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
