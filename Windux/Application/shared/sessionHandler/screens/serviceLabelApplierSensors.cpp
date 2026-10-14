/* @file serviceLabelApplierSensors.cpp
 * serviceLabelApplierSensors
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for updating the ui with the state of the label applier sensors
 */

#include "serviceLabelApplierSensors.h"

#include "ui_serviceLabelApplierSensors.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceLabelApplierSensors::serviceLabelApplierSensors(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceLabelApplierSensors)
{
    ui->setupUi(this);
    scaleScreen();

    //Will display/hide the weigher sensor information
    updateLabelApplierConfig();
    labelApplierSensors = new DomLabelApplierSensors();
    labelApplierSensorMsgReceived = false;
    update();
    translate();

    session_->send(LabelApplier, LaMode, "5");              // Set label applier mode to service
    session_->send(LabelApplier, LaSensorsTracking, "1");   // Turn on sensor tracking
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceLabelApplierSensors::~serviceLabelApplierSensors()
{
    delete labelApplierSensors;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::hideWeigher()
{
    //Hide the weigher text
    ui->lblWeigherText->hide();

    //Hide the Wakeup Sensor Fields
    ui->lblWakeupSensor->hide();
    ui->lblWakeupSensorText->hide();

    //Hide the Entry Sensor Fields
    ui->lblEntrySensor->hide();
    ui->lblEntrySensorText->hide();

    //Move the Label Applier Fields Up
    //Move up the label applier sensors text
    ui->lblPrimaryLabelApplierText->setGeometry(scaleRectForScreen(QRect(30, 70, 521, 41)));
    ui->lblCLAEntrySensorText->setGeometry(scaleRectForScreen(QRect(140, 130, 431, 51)));
    ui->lblPkgEdgeDetectSensorText->setGeometry(scaleRectForScreen(QRect(140, 180, 431, 41)));
    ui->lblStartSwitchText->setGeometry(scaleRectForScreen(QRect(140, 230, 431, 41)));
    ui->lblStopSwitchText->setGeometry(scaleRectForScreen(QRect(140, 280, 431, 41)));
    ui->lblCLAGateSwitchText->setGeometry(scaleRectForScreen(QRect(140, 330, 431, 41)));
    ui->lblPrinterPostSwitchText->setGeometry(scaleRectForScreen(QRect(140, 380, 431, 41)));

    //Move up the label applier sensor states
    ui->lblCLAEntrySensor->setGeometry(scaleRectForScreen(QRect(630, 130, 181, 41)));
    ui->lblPkgEdgeDetectSensor->setGeometry(scaleRectForScreen(QRect(630, 180, 181, 41)));
    ui->lblStartSwitch->setGeometry(scaleRectForScreen(QRect(630, 230, 181, 41)));
    ui->lblStopSwitch->setGeometry(scaleRectForScreen(QRect(630, 280, 181, 41)));
    ui->lblCLAGateSwitch->setGeometry(scaleRectForScreen(QRect(630, 330, 181, 41)));
    ui->lblPrinterPostSwitch->setGeometry(scaleRectForScreen(QRect(630, 380, 181, 41)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::showWeigher()
{
    //Move the Label Applier Fields Down
    //Shift down the label applier sensors text
    ui->lblPrimaryLabelApplierText->setGeometry(scaleRectForScreen(QRect(30, 220, 521, 41)));
    ui->lblCLAEntrySensorText->setGeometry(scaleRectForScreen(QRect(140, 270, 431, 51)));
    ui->lblPkgEdgeDetectSensorText->setGeometry(scaleRectForScreen(QRect(140, 320, 431, 41)));
    ui->lblStartSwitchText->setGeometry(scaleRectForScreen(QRect(140, 370, 431, 41)));
    ui->lblStopSwitchText->setGeometry(scaleRectForScreen(QRect(140, 420, 431, 41)));
    ui->lblCLAGateSwitchText->setGeometry(scaleRectForScreen(QRect(140, 470, 431, 41)));
    ui->lblPrinterPostSwitchText->setGeometry(scaleRectForScreen(QRect(140, 520, 431, 41)));

    //Shift down the label applier sensor states
    ui->lblCLAEntrySensor->setGeometry(scaleRectForScreen(QRect(630, 270, 181, 41)));
    ui->lblPkgEdgeDetectSensor->setGeometry(scaleRectForScreen(QRect(630, 320, 181, 41)));
    ui->lblStartSwitch->setGeometry(scaleRectForScreen(QRect(630, 370, 181, 41)));
    ui->lblStopSwitch->setGeometry(scaleRectForScreen(QRect(630, 420, 181, 41)));
    ui->lblCLAGateSwitch->setGeometry(scaleRectForScreen(QRect(630, 470, 181, 41)));
    ui->lblPrinterPostSwitch->setGeometry(scaleRectForScreen(QRect(630, 520, 181, 41)));

    //Show the weigher text
    ui->lblWeigherText->show();

    //Show the Wakeup Sensor Fields
    ui->lblWakeupSensor->show();
    ui->lblWakeupSensorText->show();

    //Show the Entry Sensor Fields
    ui->lblEntrySensor->show();
    ui->lblEntrySensorText->show();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierSensors::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::updateLabelApplierConfig()
{
     //If a weigher is attached, display the weigher sensors
     if(session_->scaleConfig->peripheralList.firstWeigher())
     {
         showWeigher();
     }
     else
     {
         hideWeigher();
     }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::setBlockedUnblocked(QLabel* label, int blocked)
{
    switch( blocked )
    {
        case 0:
            label->setText(tr("Unblocked"));
            label->setStyleSheet("background-color:rgb(0,255,0);color: rgb(0,0,0);");
            break;
        case 1:
            label->setText(tr("Blocked"));
            label->setStyleSheet("background-color:rgb(255,0,0);color: rgb(0,0,0);");
            break;
        default:
            label->setText(tr("Unknown"));
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::setPressedReleased(QLabel* label, int pressed)
{
    switch( pressed )
    {
        case 0:
            label->setText(tr("Released"));
            label->setStyleSheet("background-color:rgb(0,255,0);color: rgb(0,0,0);");
            break;
        case 1:
            label->setText(tr("Pressed"));
            label->setStyleSheet("background-color:rgb(255,0,0);color: rgb(0,0,0);");
            break;
        default:
            label->setText(tr("Unknown"));
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::translate()
{
    ui->retranslateUi(this);
    if(labelApplierSensorMsgReceived)
        updateLabelApplierSensors();
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierSensors::onScreenChanging()
{
    //Turn off sensor tracking
    session_->send(LabelApplier, LaSensorsTracking, "0");

    //Set label Applier to conveyor mode
    session_->send(LabelApplier, LaMode, "3");

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::onSessionMessage( const SessionMessage& msg )
{
    if(LabelApplier == msg[0].toInt() && LaSensors == msg[1].toInt())
    {
        // we just asked for and received the applier's sensor message so update the GUI
        DomLabelApplierSensors m(msg);

        //Save the current state
        *labelApplierSensors = m;
        labelApplierSensorMsgReceived = true;
        updateLabelApplierSensors();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierSensors::updateLabelApplierSensors()
{
    // Wakeup Sensor
    setBlockedUnblocked(ui->lblWakeupSensor, labelApplierSensors->wakeupSensor().toInt());

    // Weigher Entry Sensor
    setBlockedUnblocked(ui->lblEntrySensor, labelApplierSensors->weigherEntrySensor().toInt());

    // CLA Entry Sensor
    setBlockedUnblocked(ui->lblCLAEntrySensor, labelApplierSensors->claEntrySensor().toInt());

    // Package Edge Detect Sensor
    setBlockedUnblocked(ui->lblPkgEdgeDetectSensor, labelApplierSensors->pkgEdgeDetectSensor().toInt());

    // Start Switch
    setPressedReleased(ui->lblStartSwitch, labelApplierSensors->startSwitch().toInt());

    // Stop Switch
    setPressedReleased(ui->lblStopSwitch, labelApplierSensors->stopSwitch().toInt());

    // CLA Gate Switch
    setBlockedUnblocked(ui->lblCLAGateSwitch, labelApplierSensors->claGateSwitch().toInt());

    // First Printer Post Switch
    setBlockedUnblocked(ui->lblPrinterPostSwitch, labelApplierSensors->firstPrinterPostSwitch().toInt());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierSensors::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}

