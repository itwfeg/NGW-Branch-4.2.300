// Label applier configuration on two printer systems

#include "servicePPPDistance.h"
#include "ui_servicePPPDistance.h"

#include "logmanager.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

servicePPPDistance::servicePPPDistance(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui( new Ui::servicePPPDistance )
{
    ui->setupUi(this);
    scaleScreen();

    this->initialize();
    translate();

    session_->send( Wrapper, WrReqCalibration, QString::number( 13 ) );
}

servicePPPDistance::~servicePPPDistance()
{

}

void servicePPPDistance::initialize()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void servicePPPDistance::translate()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool servicePPPDistance::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePPPDistance::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO("serviceLabelApplierConfig", "onSessionMessage()");

    if(Wrapper == msg[0].toInt() && WrPPPDist == msg[1].toInt())
    {
        LOG_INFO("Received Distance from home", NULL);

        if(msg[2].toInt() == 0) // Unknown position
        {
            LOG_INFO("Unknown Position Sent", NULL);
        }
        else if(msg[2].toInt() == 1) // Primary
        {
            float distanceInches = msg[3].toInt() * 0.039370f;

            LOG_INFO("Primary Position Sent", NULL);
            ui->lblDistPlate1->setText(QString::number(msg[3].toInt()) + " mm\n" + QString::number(distanceInches, 'f', 2) + " in");
        }
        else if(msg[2].toInt() == 2) // Secondary
        {
            float distanceInches = msg[3].toInt() * 0.039370f;

            LOG_INFO("Secondary Position Sent", NULL);
            ui->lblDistPlate2->setText(QString::number(msg[3].toInt()) + " mm\n" + QString::number(distanceInches, 'f', 2) + " in");
        }
        else
        {
            LOG_INFO("Something went wrong", NULL);
            ui->lblDistPlate1->setText("unknown");
            ui->lblDistPlate2->setText("unknown");
        }
    }
}
