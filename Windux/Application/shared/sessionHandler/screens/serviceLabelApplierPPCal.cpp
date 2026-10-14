#include "serviceLabelApplierPPCal.h"
#include "Events.h"
#include "Devices.h"
#include "logmanager.h"
#include "ui_serviceLabelApplierPPCal.h"
#include "../sessionmainwindow.h"

#include <dialogs/messageboxdialog.h>

#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>

serviceLabelApplierPPCal::serviceLabelApplierPPCal(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui( new Ui::serviceLabelApplierPPCal ),
    receivedHomeComplete(false ),
    receivedReady( false ),
    calibrationStarted ( false )
{
    ui->setupUi( this );
    scaleScreen();

    startingCalibrationDialog = NULL;

    initialize();

    session_->send( Wrapper, WrReqCalibration, QString::number( 13 ) );
}

serviceLabelApplierPPCal::~serviceLabelApplierPPCal()
{
    if( startingCalibrationDialog )
    {
        delete startingCalibrationDialog;
    }
    delete ui;
}

void serviceLabelApplierPPCal::calibButClicked()
{
    // start the calibration
    //session_->send( Wrapper, WrReqCalibration, QString::number( 3 ) );

    // SOF-4601
    MessageBoxDialog dialog( mainWindow_ );
    dialog.setTitle( tr( "Move Plates" ) );
    dialog.setText( tr( "Please push both printer plates to the home position (first notch) and hit \"Next\"." ) );
    dialog.setStandardButtons( QDialogButtonBox::Ok );

    dialog.exec();

    ui->butNext->setEnabled( true );

    calibrationStarted = false;

    //startingCalibrationDialog->exec();

    //ui->butExit->setEnabled( true );
}

void serviceLabelApplierPPCal::exitButClicked()
{
    // calibration exit
    session_->send( Wrapper, WrReqCalibration, QString::number( 9 ) );
    mainWindow_->navigateHome();
}

void serviceLabelApplierPPCal::nextButClicked()
{
    if( calibrationStarted )
    {
        session_->send( Wrapper, WrReqCalibration, QString::number( 4 ) );
    }
    else
    {
        session_->send( Wrapper, WrReqCalibration, QString::number( 3 ) );
        startingCalibrationDialog->exec();
        ui->butExit->setEnabled( true );

        calibrationStarted = true;
    }
}

void serviceLabelApplierPPCal::onSessionMessage( const SessionMessage& msg )
{
    if(Wrapper == msg[0].toInt() && WrPPPCalibration == msg[1].toInt())
    {
        int calibStatus = msg[2].toInt();
        //int secondaryCalibStatus = msg[5].toInt();

        float primaryDistInches = msg[3].toInt() * 0.039370;
        float secondaryDistInches = msg[6].toInt() * 0.039370;

        handleCalibStatus( calibStatus, msg[3].toInt(), msg[6].toInt(), primaryDistInches, secondaryDistInches );

        if( calibStatus == _COMPLETE /*&& secondaryCalibStatus == _COMPLETE*/ )
        {
            ui->butExit->setEnabled( true );
        }
        else
        {
            ui->butExit->setEnabled( false );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierPPCal::onScreenChanging()
{
    // stop calibrating
    session_->send( Wrapper, WrReqCalibration, QString::number( 8 ) );

    return _Mybase::onScreenChanging();
}

void serviceLabelApplierPPCal::initialize()
{
    ui->butExit->setEnabled( false );
    ui->butNext->setEnabled( false );

    ui->homeHightlight->setStyleSheet( "QLabel { background-color: yellow; }" );
    ui->calHighlight->setStyleSheet( "QLabel { background-color: yellow; }" );

    connect( ui->butStartCalib, SIGNAL( clicked() ), this, SLOT( calibButClicked() ) );
    connect( ui->butExit, SIGNAL( clicked() ), this, SLOT( exitButClicked() ) );
    connect( ui->butNext, SIGNAL( clicked() ), this, SLOT( nextButClicked() ) );

    MessageBoxDialog movePlateHomeDialog( mainWindow_);
/*
    movePlateHomeDialog.setTitle( tr( "Move Plates" ) );
    movePlateHomeDialog.setText( tr( "Please push both printer plates to the home position (first notch)." ) );
    movePlateHomeDialog.setStandardButtons( QDialogButtonBox::Ok );
    movePlateHomeDialog.exec();
*/
    startingCalibrationDialog = new MessageBoxDialog( mainWindow_ );
    startingCalibrationDialog->setTitle( tr( "Calibration Starting" ) );
    startingCalibrationDialog->setText( tr( "Calibration is starting.\nPlease wait..." ) );
    startingCalibrationDialog->setStandardButtons( QDialogButtonBox::NoButton );
}

void serviceLabelApplierPPCal::handleCalibStatus( int status, int primaryDist, int secondDist, float primaryInches, float secondaryInches )
{
    LOG_CLASS_INFO( "serviceLabelApplierPPCal", "handlePrimaryCalibStatus" );

    calDistance0 = primaryDist;
    calDistance1 = secondDist;

    switch( status )
    {
        case _READY:
        {
            // ready
            LOG_INFO( "Ready received", NULL );

            if( !startingCalibrationDialog->close() )
            {
                LOG_INFO( "startingCalibrationDialog was not closed", NULL );
            }

            if( !receivedReady )
            {
                /* Commented out for SOF-4601
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setTitle( tr( "Move Plates" ) );
                dialog.setText( tr( "Please push both printer plates to the home position (first notch) and hit \"Next\"." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();
                */

                MessageBoxDialog dialog( mainWindow_ );
                dialog.setTitle( tr( "Move Plates" ) );
                dialog.setText( tr( "Ensure both printer plates are in the home position (first notch) and hit \"Next\"." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();

                receivedReady = true;
            }

            break;
        }
        case _SAMPLING:
        {
            // sampling
            LOG_INFO( "Sampling received", NULL );

            ui->butNext->setEnabled( true );

            receivedReady = false;

            break;
        }
        case _HOME_COMPLETE:
        {
            LOG_INFO( "_HOME_COMPLETE", NULL );

            if( !receivedHomeComplete )
            {
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setTitle( tr( "Move Plates" ) );
                dialog.setText( tr( "Please move both printer plates to 9.5\" and hit \"Next\"." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();

                receivedHomeComplete = true;
            }

            if( ( primaryInches <= 10 && primaryInches >= 9 ) && ( secondaryInches <= 10 && secondaryInches >= 9 ) )
            {
                ui->butNext->setEnabled( true);
            }
            else
            {
                ui->butNext->setEnabled( false );
            }

            break;
        }
        case _VERIFY_HOME:
        {
            LOG_INFO( "_VERIFY_HOME", NULL );

            if( ( primaryInches <= 10 && primaryInches >= 9 ) && ( secondaryInches <= 10 && secondaryInches >= 9 ) )
            {
                ui->butNext->setEnabled( true);
            }
            else
            {
                ui->butNext->setEnabled( false );
            }

            break;
        }
        case _SAMPLING_SLOPE:
        {
            LOG_INFO( "_SAMPLING_SLOPE", NULL );

            ui->butNext->setEnabled( true );

            receivedHomeComplete = false;

            break;
        }
        case _SLOPE_CALC:
        {
            LOG_INFO( "_SLOPE_CALC", NULL );

            ui->lblCalibrationData0->setText( "Slope" );
            ui->lblCalibrationData0->setStyleSheet( "QLabel { background-color : green }" );

            ui->lblCalibrationData1->setText( "Slope" );
            ui->lblCalibrationData1->setStyleSheet( "QLabel { background-color : green }" );

            break;
        }
        case _COMPLETE:
        {
            // complete
            LOG_INFO( "_COMPLETE received", NULL );
            ui->lblCalibrationData0->setText( "Complete" );
            ui->lblCalibrationData0->setStyleSheet( "QLabel { background-color : green }" );

            receivedReady = false;
            receivedHomeComplete = false;

            break;
        }
        case _WAIT_FOR_MODE_CHANGE:
        {
            // wait for mode change
            LOG_INFO( "Wait for mode change", NULL );

            break;
        }
        case _PLATE0_NOT_HOME:
        {
            LOG_INFO( "Plate 0 Not Home", NULL );

            // primary plate not home
            ui->lblCalibrationData0->setText( "Not Home" );
            ui->lblCalibrationData0->setStyleSheet( "QLabel { background-color : yellow }" );

            break;
        }
        case _PLATE0_TARGET_ALIGNMENT:
        {
            // primary plate target alignment
            ui->lblCalibrationData0->setText( "Alignment" );
            ui->lblCalibrationData0->setStyleSheet( "QLabel { background-color : red }" );

            break;
        }
        case _ERROR_SENSOR0:
        {
            // sensor 0 error
            ui->lblCalibrationData0->setText( "Sensor" );
            ui->lblCalibrationData0->setStyleSheet( "QLabel { background-color : red }" );

            break;
        }
        case _PLATE1_NOT_HOME:
        {
            LOG_INFO( "Plate 1 Not Home", NULL );

            // primary plate not home
            ui->lblCalibrationData1->setText( "Not Home" );
            ui->lblCalibrationData1->setStyleSheet( "QLabel { background-color : yellow }" );

            break;
        }
        case _PLATE1_TARGET_ALIGNMENT:
        {
            LOG_INFO( "_PLATE1_TARGET_ALIGNMENT sent", NULL );

            // primary plate target alignment
            ui->lblCalibrationData1->setText( "Alignment" );
            ui->lblCalibrationData1->setStyleSheet( "QLabel { background-color : red }" );

            break;
        }
        case _ERROR_SENSOR1:
        {
            LOG_INFO( "_ERROR_SENSOR1 sent", NULL );

            // sensor 0 error
            ui->lblCalibrationData1->setText( "Sensor" );
            ui->lblCalibrationData1->setStyleSheet( "QLabel { background-color : red }" );

            break;
        }
        case _ERROR_CONFIG:
        {
            // error config
            LOG_INFO( "ERROR_CONFIG sent", NULL );

            break;
        }
    }

    ui->lblDistPlate1->setText( QString::number( calDistance0 ) + " mm\n" + QString::number( primaryInches, 'f', 2 ) + " in" );
    ui->lblDistPlate2->setText( QString::number( calDistance1 ) + " mm\n" + QString::number( secondaryInches, 'f', 2 ) + " in" );
}
