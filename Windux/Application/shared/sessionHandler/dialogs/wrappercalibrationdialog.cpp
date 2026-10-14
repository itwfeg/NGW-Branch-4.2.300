/**
 * @file wrappercalibrationdialog.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrappercalibrationdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperCalibrationDialog::WrapperCalibrationDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    retainMode_( false ),
    active_( false ),
    testFailed_( false )
{
    // set object name
    setObjectName( "WrapperCalibrationDialog" );

    // init
    initialize();
    createLayout();

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    // request wrapper status
    session_->send( Wrapper, WrReqStatus );

    // enter wrapper service mode
    session_->send( "9,7,1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperCalibrationDialog::WrapperCalibrationDialog( bool retainMode, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    retainMode_( retainMode ),
    active_( false ),
    testFailed_( false )
{
    // set object name
    setObjectName( "WrapperCalibrationDialog" );

    // init
    initialize();
    createLayout();

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    // request wrapper status
    session_->send( Wrapper, WrReqStatus );

    // enter wrapper service mode
    session_->send( "9,7,1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperCalibrationDialog::~WrapperCalibrationDialog()
{
    // exit wrapper service mode
    // when not in service
    // when not retaining mode
    if (( !mainWindow_->isServiceMode() ) && ( !retainMode_ ))
        session_->send( "9,7,0" );

    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperCalibrationDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 800) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 500) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::addMechanism( int number, const QString &name )
{
    QPushButton *button( new QPushButton( this ) );
    button->setText( name );
    button->setCheckable( true );

    connect( button, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );

    buttonsLayout_->insertWidget( 0, button );

    mechanisms_[button] = number;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "WrapperCalibrationDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // wrapper status
    if (( Wrapper == device ) && ( WrEvolStatus == event ))
    {
        evolWrapperStatus_ = msg;
        processWrapperStatus();
    }

    // wrapper test complete
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ) && ( active_ ))
    {
        LOG_INFO( "calibration complete!", NULL );

        active_ = false;
        testFailed_ = false;
        processWrapperStatus();

        // cancel timeout
        timeout_->stop();
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::onButtonClicked()
{
    LOG_CLASS_INFO( "WrapperCalibrationDialog", "onButtonClicked" );

    QPushButton *button( qobject_cast<QPushButton*>( sender() ) );

    // calibrate!
    if ( calibrate_ == button )
    {
        LOG_INFO( "starting calibration...", NULL );

        int timeoutMin( 5 ); // 5 min

        // look for specific mechanism
        int mechanism( 127 );

        foreach ( QPushButton *b, mechanisms_.keys() )
            if ( b->isChecked() )
            {
                timeoutMin = 1; // 1 min

                mechanism = mechanisms_[b];
                break;
            }

        const QString msg(
            "26," +                                 // CalibrateWrapper
            QString::number( mechanism ) + "," +    // All Mechanisms or specific mechanism
            "0" );                                  // TrayLength

        // start the test!
        session_->send( "9,7,1" );
        session_->send( Wrapper, WrEvolTest, msg );

        active_ = true;
        testFailed_ = false;
        processWrapperStatus();

        // setup timeout
        timeout_->setSingleShot( true );
        timeout_->start( timeoutMin * 60 * 1000 );
    }

    // mechanism
    else if ( mechanisms_.contains( button ) )
    {
        // uncheck other mechanisms
        foreach ( QPushButton *b, mechanisms_.keys() )
        {
            bool checked(( button == b ) && ( b->isChecked() ));

            // update palette
            QPalette p( defaultPalette_ );

            if ( checked )
                p.setBrush( QPalette::Button, hobart::colorPalette::green );

            b->setChecked( checked );
            b->setPalette( p );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::onTimerTimeout()
{
    LOG_CLASS_INFO( "WrapperCalibrationDialog", "onTimerTimeout" );

    // calibration timeout
    if ( timeout_ == sender() )
    {
        LOG_WARN( "calibration timed out!", NULL );

        testFailed_ = true;
        processWrapperStatus();
    }

    // blink timeout
    else if ( blink_ == sender() )
    {
        statusLabel_->setVisible( !statusLabel_->isVisible() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::initialize()
{
    modifyFontPointSize setFontPointSize36( (mainWindow_->height() * 36) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteForeground setForegroundRed( modifyPaletteForeground::createPalette( hobart::colorPalette::red ) );

    setFontPointSize18( this );

    // create widgets
    titleLabel_ = new QLabel( tr( "Calibrate Wrapper" ), this );
    titleLabel_->setAlignment( Qt::AlignCenter );
    titleLabel_->setObjectName( "titleLabel" );
    setFontPointSize36( titleLabel_ );

    instructionsLabel_ = new QLabel( tr( "This action will calibrate all wrapper mechanisms. This process takes 3-5 minutes to complete. You will not be able to wrap packages while calibration is in progress.\n\nRemove any packages, labels, debris in the machine." ), this );
    instructionsLabel_->setObjectName( "instructionsLabel" );
    instructionsLabel_->setWordWrap( true );

    warningLabel_ = new QLabel( tr( "WARNING: DO NOT POWER OFF THE WRAPPER WHILE CALIBRATION IS ACTIVE!" ), this );
    warningLabel_->setObjectName( "warningLabel" );
    warningLabel_->setWordWrap( true );
    setForegroundRed( warningLabel_ );

    statusLabel_ = new QLabel( this );
    statusLabel_->setAlignment( Qt::AlignCenter );
    statusLabel_->setObjectName( "statusLabel" );
    statusLabel_->setWordWrap( true );
    //setForegroundRed( statusLabel_ );

    calibrate_ = new QPushButton( tr( "Begin Calibration" ), this );
    calibrate_->setEnabled( false );
    calibrate_->setObjectName( "calibrate" );
    connect( calibrate_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    exit_ = new QPushButton( tr( "Exit" ), this );
    exit_->setObjectName( "exit" );
    connect( exit_, SIGNAL(clicked()), SLOT(accept()) );

    timeout_ = new QTimer( this );
    connect( timeout_, SIGNAL(timeout()), SLOT(onTimerTimeout()) );

    blink_ = new QTimer( this );
    blink_->setSingleShot( false );
    blink_->start( 1000 );
    connect( blink_, SIGNAL(timeout()), SLOT(onTimerTimeout()) );

    // save off palette
    defaultPalette_ = calibrate_->palette();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::createLayout()
{
    if ( layout() )
        delete layout();

    // create layouts
    buttonsLayout_ = new QHBoxLayout();
    buttonsLayout_->addStretch();
    buttonsLayout_->addWidget( calibrate_ );
    buttonsLayout_->addWidget( exit_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addWidget( titleLabel_ );
    screenLayout->addSpacing( 32 );
    screenLayout->addWidget( instructionsLabel_ );
    screenLayout->addSpacing( 16 );
    screenLayout->addWidget( warningLabel_ );
    screenLayout->addStretch();
    screenLayout->addWidget( statusLabel_ );
    screenLayout->addStretch();
    screenLayout->addLayout( buttonsLayout_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperCalibrationDialog::processWrapperStatus()
{
    // update enabled
    if ( !evolWrapperStatus_.mainPower() )
    {
        statusLabel_->setText( tr( "Press the START Button" ) );

        calibrate_->setEnabled( false );
    }
    else if ( evolWrapperStatus_.buildingPressure() )
    {
        statusLabel_->setText( tr( "Building Pressure..." ) );

        calibrate_->setEnabled( false );
    }
    else
    {
        if ( testFailed_ )
            statusLabel_->setText( tr( "Error Occurred! Please exit and try again." ) );
        else if ( active_ )
            statusLabel_->setText( tr( "Calibrating... Please Wait..." ) );
        else
            statusLabel_->clear();

        calibrate_->setEnabled( !active_ );

        foreach ( QPushButton *b, mechanisms_.keys() )
            b->setEnabled( !active_ );
    }

    statusLabel_->repaint();

    // update palette
    QPalette p( defaultPalette_ );

    if (( active_ ) && ( !testFailed_ ))
        p.setBrush( QPalette::Button, hobart::colorPalette::red );

    calibrate_->setPalette( p );

    // update exit
    exit_->setEnabled( !active_ || testFailed_ );
}

