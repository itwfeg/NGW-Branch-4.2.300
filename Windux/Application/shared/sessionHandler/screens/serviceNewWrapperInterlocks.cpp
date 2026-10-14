
/* @file serviceNewWrapperInterlocks.cpp
 * serviceNewWrapperInterlocks
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for updating the ui with the state of the wrapper interlocks
 */

#include "serviceNewWrapperInterlocks.h"

#include "ui_serviceNewWrapperInterlocks.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperInterlocks::serviceNewWrapperInterlocks(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperInterlocks)
{
    ui->setupUi(this);
    scaleScreen();

    //Update and Initialize the Page
    initialize();
    translate();

    //Send a request for a wrapper sensors msg
    session_->send(Wrapper, WrReqSensors);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperInterlocks::initialize()
{
    wrapperSensors = new DomEvolWrapperSensors();
    wrapperSensorsMsgReceived = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperInterlocks::~serviceNewWrapperInterlocks()
{
    delete wrapperSensors;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperInterlocks::setSensorState(QLabel* label, bool open)
{
    if ( open )
    {
        label->setStyleSheet("background-color:rgb(255,0,0);color: rgb(0,0,0);");
        label->setText(tr("Open"));
    }
    else
    {
        label->setStyleSheet("background-color:rgb(0,255,0);color: rgb(0,0,0);");
        label->setText(tr("Closed"));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperInterlocks::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperInterlocks::translate()
{
    ui->retranslateUi(this);
    if(wrapperSensorsMsgReceived)
        updateWrapperSensors();
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperInterlocks::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperInterlocks::onSessionMessage( const SessionMessage& msg )
{
    if(Wrapper == msg[0].toInt() && WrEvolSensors == msg[1].toInt())
    {
        // we just asked for and received the wrapper's sensor message so update the GUI
        DomEvolWrapperSensors m(msg);

        //Save the current state of the sensors
        *wrapperSensors = m;
        wrapperSensorsMsgReceived = true;
        updateWrapperSensors();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperInterlocks::updateWrapperSensors()
{
    // Film Supply Door Sensor
    setSensorState(ui->lblFilmSupplyDoor, wrapperSensors->interlockFilmSupplyDoor());

    // Right Side Cover Sensor
    setSensorState(ui->lblRightSideCover, wrapperSensors->interlockRightSideCover());

    // Right Canopy Sensor
    setSensorState(ui->lblRightCanopy, wrapperSensors->interlockRightCanopy());

    // Left Side Cover Sensor
    setSensorState(ui->lblLeftSideCover, wrapperSensors->interlockLeftSideCover());

    // Intake Clean Out Sensor
    setSensorState(ui->lblIntakeCleanOut, wrapperSensors->interlockIntakeCleanout());

    // Stop Switch
    setSensorState(ui->lblStopSwitch, wrapperSensors->interlockStop());

    // Start Switch
    setSensorState(ui->lblStartSwitch, wrapperSensors->start());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperInterlocks::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}

