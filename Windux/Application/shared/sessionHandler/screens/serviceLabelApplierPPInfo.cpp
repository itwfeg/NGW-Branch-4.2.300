#include "serviceLabelApplierPPInfo.h"
#include "ui_serviceLabelApplierPPInfo.h"
#include "Events.h"

#include "Events.h"
#include "Devices.h"
#include "logmanager.h"
#include "../sessionmainwindow.h"

serviceLabelApplierPPInfo::serviceLabelApplierPPInfo(SessionMainWindow *mainWindow, ModeWidget *parent)
    : _Mybase( mainWindow, parent )
    , ui( new Ui::serviceLabelApplierPPInfo )
{
    ui->setupUi(this);
    scaleScreen();

    session_->send(Wrapper, WrReqCalibration, QString::number(2)); // request sensor version
    //session_->send(Wrapper, WrReqCalibration, QString::number(1)); // request configuration
}

serviceLabelApplierPPInfo::~serviceLabelApplierPPInfo()
{
    delete ui;
}

bool serviceLabelApplierPPInfo::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

void serviceLabelApplierPPInfo::onSessionMessage(const SessionMessage &msg)
{

    if(msg[0].toInt() == Wrapper && msg[1].toInt() == WrPPPSensorVersion)
    {
        if(msg[2] != "0")
        {
            productID_ = msg[2];
        }

        if(msg[3] != "0.0.0")
        {
            softwareVersion_ = msg[3];
        }

        if(msg[4] != "0.0")
        {
            hardwareVersion_ = msg[4];
        }

        ui->productId->setText(productID_);
        ui->sensorSoftwareVersion->setText(softwareVersion_);
        ui->sensorHardwareVersion->setText(hardwareVersion_);
    }
}
