/* @file serviceNewWrapperTestIntake.cpp
 * serviceNewWrapperTestIntake
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Shows Wrapper test intake
 */

#include "serviceNewWrapperTestIntake.h"
#include "ui_serviceNewWrapperTestIntake.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <numericlineedit.h>

#include <backgroundwidgetstyles.h>

#include <QIntValidator>
#include <QTimer>

#include <logmanager.h>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

serviceNewWrapperTestIntake::serviceNewWrapperTestIntake(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestIntake)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    internalConfigData = &session_->scaleConfig->internalConfig;

    scaleConfigData = &session_->scaleConfig->scaleConfig;
    evolWrapperConfigData = &session_->scaleConfig->evolWrapperConfig;
    evolWrapperSystemInformation = &session_->scaleConfig->evolWrapperSystemInformation;

    update();
    initialize();
    translate();

    // SOF-1264
    requestCameraTracking_ = new QTimer( this );
    requestCameraTracking_->setSingleShot( true );
    requestCameraTracking_->setInterval( 5000 );

    connect( requestCameraTracking_, SIGNAL(timeout()), SLOT(requestCameraTracking()) );
}

bool serviceNewWrapperTestIntake::eventFilter( QObject *obj, QEvent *event )
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

serviceNewWrapperTestIntake::~serviceNewWrapperTestIntake()
{
    delete ui;
}

int serviceNewWrapperTestIntake::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceNewWrapperTestIntake::initialize()
{
    hasChanged = false;
    heightSensorsOn = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->scaleModel = scaleConfigData->scaleModel();

    connect(ui->butPrimaryMotorCycle, SIGNAL(clicked()), this, SLOT(butPrimaryMotorCycleClicked()));
    connect(ui->butSecondaryMotorCycle, SIGNAL(clicked()), this, SLOT(butSecondaryMotorCycleClicked()));
    connect(ui->butCycleIntake, SIGNAL(clicked()), this, SLOT(butCycleIntakeClicked()));
    connect(ui->butCycleWrapper, SIGNAL(clicked()), this, SLOT(butCycleWrapperClicked()));
    connect(ui->butHeightSensorsOff, SIGNAL(clicked()), this, SLOT(butHeightSensorsOffClicked()));
    connect(ui->butHeightSensorsOn,  SIGNAL(clicked()), this, SLOT(butHeightSensorsOnClicked()));
    connect(ui->butShifterHome,  SIGNAL(clicked()), this, SLOT(butShifterHomeClicked()));
    connect(ui->butShifterLeft,  SIGNAL(clicked()), this, SLOT(butShifterLeftClicked()));
    connect(ui->butShifterRight,  SIGNAL(clicked()), this, SLOT(butShifterRightClicked()));

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        ui->label_6->hide();
        ui->butShifterLeft->hide();
        ui->butShifterHome->hide();
        ui->butShifterRight->hide();

        ui->label_10->hide();
        ui->lblShiftHome->hide();

        ui->label_11->hide();
        ui->lblShiftLeft->hide();
    }

    normalBackground_ = ui->lblPrimaryHome->palette();

    greenBackground_ = normalBackground_;
    greenBackground_.setBrush( ui->lblPrimaryHome->backgroundRole(), hobart::colorPalette::green );

    redBackground_ = normalBackground_;
    redBackground_.setBrush( ui->lblPrimaryHome->backgroundRole(), hobart::colorPalette::red );

    ui->lblPrimaryHome->setAutoFillBackground( true );
    ui->lblShiftHome->setAutoFillBackground( true );
    ui->lblShiftLeft->setAutoFillBackground( true );
    ui->lblElevatorHome->setAutoFillBackground( true );

    ui->lblBit600->setAutoFillBackground( true );
    ui->lblBit500->setAutoFillBackground( true );
    ui->lblBit450->setAutoFillBackground( true );
    ui->lblBit400->setAutoFillBackground( true );
    ui->lblBit350->setAutoFillBackground( true );
    ui->lblBit300->setAutoFillBackground( true );
    ui->lblBit250->setAutoFillBackground( true );
    ui->lblBit225->setAutoFillBackground( true );
    ui->lblBit200->setAutoFillBackground( true );
    ui->lblBit175->setAutoFillBackground( true );
    ui->lblBit150->setAutoFillBackground( true );
    ui->lblBit125->setAutoFillBackground( true );
    ui->lblBit100->setAutoFillBackground( true );
    ui->lblBit075->setAutoFillBackground( true );
    ui->lblBit050->setAutoFillBackground( true );

    if(evolWrapperSystemInformation->awsManufactureDate() == "0" || evolWrapperSystemInformation->awsManufactureDate() == "000000" || evolWrapperSystemInformation->awsManufactureDate() == "00/00/00")
    {
        session_->send(Wrapper, WrReqEvolSystemInfo);
    }

    session_->send(Wrapper,WrReqSensors);

    update();
}

void serviceNewWrapperTestIntake::updateEvolWrapperSensors(const SessionMessage& msg)
{
    DomEvolWrapperSensors m(msg);

    if(m.primaryHome())
        greenTheLabel(ui->lblPrimaryHome);
    else
        redTheLabel(ui->lblPrimaryHome);

    if(m.shiftHome())
        greenTheLabel(ui->lblShiftHome);
    else
        redTheLabel(ui->lblShiftHome);

    if(m.shiftLeftLimit())
        greenTheLabel(ui->lblShiftLeft);
    else
        redTheLabel(ui->lblShiftLeft);

    if(m.elevatorHome())
        greenTheLabel(ui->lblElevatorHome);
    else
        redTheLabel(ui->lblElevatorHome);
}

void serviceNewWrapperTestIntake::updateEvolWrapperPkgDescription(const SessionMessage& msg)
{
    DomEvolWrapperPkgDesc m(msg);
    ui->lblTrayValue->setText(m.trayName());
    ui->lblLengthValue->setText(m.length());
    ui->lblWidthValue->setText(m.width());
    ui->lblHeightValue->setText(m.height());
    ui->lblLipValue->setText(m.lip());
    ui->lblFilmUsedValue->setText(m.filmWidthUsedToWrap());
}

void serviceNewWrapperTestIntake::greenTheLabel(QLabel *lbl)
{
    LOG_CLASS_INFO( "testIntake", "greenTheLabel" );

    if ( lbl )
    {
        LOG_DEBUG( "Turn %s Green.", qPrintable( lbl->text() ) );
        lbl->setPalette( greenBackground_ );
    }
    LOG_DEBUG( "Done", NULL );
}

void serviceNewWrapperTestIntake::greyTheLabel(QLabel *lbl)
{
    LOG_CLASS_INFO( "testIntake", "greyTheLabel" );

    if ( lbl )
    {
        LOG_DEBUG( "Turn %s back to normal.", qPrintable( lbl->text() ) );
        lbl->setPalette( normalBackground_ );
    }
    LOG_DEBUG( "Done", NULL );
}

void serviceNewWrapperTestIntake::displaySensorState(QLabel *lbl, const QString &state)
{
    LOG_CLASS_INFO( "testIntake", "displaySensorState" );
    if(lbl != NULL)
    {
        LOG_DEBUG( "QLabel: %s", lbl->text().toLatin1().constData() );
        if(state == "1")
        {
            greenTheLabel(lbl);
        }
        else
        {
            greyTheLabel(lbl);
        }
    }
    LOG_DEBUG( "Done", NULL );
}

void serviceNewWrapperTestIntake::updateCameraHeightScan(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "testIntake", "updateCameraHeightScan" );

    DomEvolWrapperCameraHeightScan m( msg );      //  9,140

    LOG_DEBUG( "Begin...", NULL );

    if ( m.bit600() != prevScan_.bit600() )
        displaySensorState( ui->lblBit600, m.bit600() );

    if ( m.bit500() != prevScan_.bit500() )
        displaySensorState( ui->lblBit500, m.bit500() );

    if ( m.bit450() != prevScan_.bit450() )
        displaySensorState( ui->lblBit450, m.bit450() );

    if ( m.bit400() != prevScan_.bit400() )
        displaySensorState( ui->lblBit400, m.bit400() );

    if ( m.bit350() != prevScan_.bit350() )
        displaySensorState( ui->lblBit350, m.bit350() );

    if ( m.bit300() != prevScan_.bit300() )
        displaySensorState( ui->lblBit300, m.bit300() );

    if ( m.bit250() != prevScan_.bit250() )
        displaySensorState( ui->lblBit250, m.bit250() );

    if ( m.bit225() != prevScan_.bit225() )
        displaySensorState( ui->lblBit225, m.bit225() );

    if ( m.bit200() != prevScan_.bit200() )
        displaySensorState( ui->lblBit200, m.bit200() );

    if ( m.bit175() != prevScan_.bit175() )
        displaySensorState( ui->lblBit175, m.bit175() );

    if ( m.bit150() != prevScan_.bit150() )
        displaySensorState( ui->lblBit150, m.bit150() );

    if ( m.bit125() != prevScan_.bit125() )
        displaySensorState( ui->lblBit125, m.bit125() );

    if ( m.bit100() != prevScan_.bit100() )
        displaySensorState( ui->lblBit100, m.bit100() );

    if ( m.bit075() != prevScan_.bit075() )
        displaySensorState( ui->lblBit075, m.bit075() );

    if ( m.bit050() != prevScan_.bit050() )
        displaySensorState( ui->lblBit050, m.bit050() );

    prevScan_ = m;

    LOG_DEBUG( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestIntake::redTheLabel(QLabel *lbl)
{
    if ( lbl )
        lbl->setPalette( redBackground_ );
}

void serviceNewWrapperTestIntake::butPrimaryMotorCycleClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butPrimaryMotorCycle, WrPrimaryIntakeTest, -1, -1);
}

void serviceNewWrapperTestIntake::butSecondaryMotorCycleClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butSecondaryMotorCycle, WrSecondaryIntakeTest, -1, -1);
}

void serviceNewWrapperTestIntake::butCycleIntakeClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butCycleIntake, WrCycleIntakeTest, -1, -1);
}

void serviceNewWrapperTestIntake::butCycleWrapperClicked()
{
    if (heightSensorsOn)
		butHeightSensorsOffClicked();
	deviceTest->sendWrapperTest(session_, ui->butCycleWrapper, WrCycleWrapper, -1, -1);
}

void serviceNewWrapperTestIntake::butHeightSensorsOffClicked()
{
    ui->butHeightSensorsOn->setStyleSheet("");
    ui->butHeightSensorsOff->setStyleSheet("");

    session_->send(Wrapper, WrEvolCameraHeightScanTracking,"0");
    requestCameraTracking_->stop();

    heightSensorsOn = false;
}

void serviceNewWrapperTestIntake::butHeightSensorsOnClicked()
{
    ui->butHeightSensorsOn->setStyleSheet("background-color: red");
    ui->butHeightSensorsOff->setStyleSheet("");

    requestCameraTracking();

    heightSensorsOn = true;
}

void serviceNewWrapperTestIntake::butShifterHomeClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butShifterHome, WrShifterTest, 2, -1);
}

void serviceNewWrapperTestIntake::butShifterLeftClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butShifterLeft, WrShifterTest, 0, -1);
}

void serviceNewWrapperTestIntake::butShifterRightClicked()
{
    deviceTest->sendWrapperTest(session_, ui->butShifterRight, WrShifterTest, 1, -1);
}

void serviceNewWrapperTestIntake::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

void serviceNewWrapperTestIntake::requestCameraTracking()
{
    session_->send(Wrapper, WrEvolCameraHeightScanTracking,"1");

    requestCameraTracking_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestIntake::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestIntake::onScreenChanging()
{

    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperTestIntake::saveDataOnExit()
{
    session_->send(Wrapper, WrEvolCameraHeightScanTracking,"0");
}

void serviceNewWrapperTestIntake::updateSystemInfo(const SessionMessage& msg)
{
    DomEvolWrapperSystemInformation m(msg);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestIntake::onSessionMessage( const SessionMessage& msg )
{
    if (Wrapper == msg[0].toInt() &&  WgReqStatus == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        ;//popupActive = true;
        //popupCreateNewPLUDialog();
    }
    else if(Wrapper == msg[0].toInt() && WrEvolSystemInfo == msg[1].toInt())  //9,144
    {
        ;
    }
    else if(Wrapper == msg[0].toInt() && WrEvolSensors == msg[1].toInt()) // 9,115
    {
        updateEvolWrapperSensors(msg);
    }
    else if(Wrapper == msg[0].toInt() && WrEvolPackageDesc == msg[1].toInt()) // 9,125
    {
        updateEvolWrapperPkgDescription(msg);
    }
    else if(Wrapper == msg[0].toInt() && WrEvolTestComplete == msg[1].toInt())
    {
        DomEvolWrapperTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }
    else if(Wrapper == msg[0].toInt() && WrCameraHeightScan == msg[1].toInt())
    {
        updateCameraHeightScan(msg);
        requestCameraTracking_->start();
    }
}

bool serviceNewWrapperTestIntake::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}


