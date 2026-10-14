 /* @file serviceNewWrapperTestCamera.cpp
 * serviceNewWrapperTestCamera
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Shows Wrapper test Camera
 */

#include "serviceNewWrapperTestCamera.h"
#include "ui_serviceNewWrapperTestCamera.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

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

serviceNewWrapperTestCamera::serviceNewWrapperTestCamera(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestCamera)
{
    ui->setupUi(this);
    scaleScreen();

    isCalibrating = false;
    leftTestResult = "";
    rightTestResult = "";

    deviceTest = new deviceTests();

    internalConfigData = &session_->scaleConfig->internalConfig;

    scaleConfigData = &session_->scaleConfig->scaleConfig;
    update();
    connect(ui->butCalibrateLeftSide, SIGNAL(clicked()), this, SLOT(butCalibrateLeftSideClicked()));
    connect(ui->butCalibrateRightSide, SIGNAL(clicked()), this, SLOT(butCalibrateRightSideClicked()));



    initialize();
    translate();

}

void serviceNewWrapperTestCamera::butCalibrateLeftSideClicked()
{
    if(!isCalibrating)
    {
        isCalibrating = true;
        deviceTest->sendWrapperTest(session_, ui->butCalibrateLeftSide, WrCameraTest, 6, -1);
    }
}

void serviceNewWrapperTestCamera::butCalibrateRightSideClicked()
{
    if(!isCalibrating)
    {
        isCalibrating = true;
        deviceTest->sendWrapperTest(session_, ui->butCalibrateRightSide, WrCameraTest, 5, -1);
    }
}

void serviceNewWrapperTestCamera::updateCameraCalibration(const SessionMessage &msg)
{
    DomEvolWrapperCameraCalibration m(msg);

    if(m.camera() == "0")
    {
        //right camera
        if(m.result() == "0")
        {
            ui->lblTestResultRight->setText(tr("Failed"));
            rightTestResult = "Failed";
        }
        else
        {
            ui->lblTestResultRight->setText(tr("Passed"));
            rightTestResult = "Passed";
        }
        ui->lblScaleFactorRight->setText(m.scaleFactor());
        ui->lblFirstPixelRight->setText(m.firstPixel());
        ui->lblLastPixelRight->setText(m.lastPixel());


    }
    else
    {
        //left camera
        if(m.result() == "0")
        {
            ui->lblTestResultLeft->setText(tr("Failed"));
            leftTestResult = "Failed";
        }
        else
        {
            ui->lblTestResultLeft->setText(tr("Passed"));
            leftTestResult = "Passed";
        }
        ui->lblScaleFactorLeft->setText(m.scaleFactor());
        ui->lblFirstPixelLeft->setText(m.firstPixel());
        ui->lblLastPixelLeft->setText(m.lastPixel());
    }
    isCalibrating = false;
}

bool serviceNewWrapperTestCamera::eventFilter( QObject *obj, QEvent *event )
{
    QString msg;

    /* Sample event below
    if(ui->leClass == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            this->doSave();
        }
    }
    */
    return _Mybase::eventFilter( obj, event );
}

serviceNewWrapperTestCamera::~serviceNewWrapperTestCamera()
{
    if( deviceTest )
        delete deviceTest;
    delete ui;
}

int serviceNewWrapperTestCamera::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceNewWrapperTestCamera::initialize()
{

    hasChanged = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->scaleModel = scaleConfigData->scaleModel();

    update();
}

void serviceNewWrapperTestCamera::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestCamera::translate()
{
    ui->retranslateUi(this);
    if(leftTestResult == "Failed") ui->lblTestResultLeft->setText(tr("Failed"));
    else if(leftTestResult == "Passed") ui->lblTestResultLeft->setText(tr("Passed"));

    if(rightTestResult == "Failed") ui->lblTestResultRight->setText(tr("Failed"));
    else if(rightTestResult == "Passed") ui->lblTestResultRight->setText(tr("Passed"));

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestCamera::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestCamera::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        ;//popupActive = true;
        //popupCreateNewPLUDialog();
    }
    else if(Wrapper == msg[0].toInt() && WrCameraCalibration == msg[1].toInt())
    {
        updateCameraCalibration(msg);
    }
    else if(Wrapper == msg[0].toInt() && WrEvolTestComplete == msg[1].toInt())
    {
        DomEvolWrapperTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }

}

bool serviceNewWrapperTestCamera::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}


