/**
 * @file serviceNewWrapperCalibration.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperCalibration.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>
#include <numericlineedit.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperCalibration::serviceNewWrapperCalibration( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    testActive_( false )
{
    setObjectName( "serviceNewWrapperCalibration" );

    // initialize
    initialize();
    createLayout();
    translate();

    // NGW-36
    // For AWS Enhanced (and greater) we don't need package length
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

    if ( DomEvolWrapperSystemInformation::reducedTimingModels <= evolWrapperSystemInformation->awsModelNumber() )
    {
        trayLengthLabel_->hide();
        trayLength_->hide();
        maxLength_->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperCalibration::~serviceNewWrapperCalibration()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::translate()
{
    title_->setText( tr( "Wrapper Calibration" ) );

    allMechanisms_->setText( tr( "All mechanisms" ) );
    primary_->setText( tr( "Primary" ) );
    shifter_->setText( tr( "Shifter" ) );
    gripper_->setText( tr( "Gripper" ) );
    sideClamps_->setText( tr( "Side Clamps" ) );
    pusher_->setText( tr( "Pusher" ) );
    frontUnderfolder_->setText( tr( "Front underfolder" ) );
    rearUnderfolder_->setText( tr( "Rear underfolder" ) );
    sideUnderfolders_->setText( tr( "Side underfolders" ) );
    actuators_->setText( tr( "Actuation Sensors" ) );

    trayLengthLabel_->setText( tr( "Tray length to test" ) );
    maxLength_->setText( tr( "(Max length = 10.0\")" ) );
    calibrate_->setText( tr( "Calibrate" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperCalibration::onScreenChanging()
{
    // cannot leave this screen until test complete
    if ( testActive_ )
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // wrapper test complete
    if (( Wrapper == device ) && ( WrEvolTestComplete == event ))
    {
        if ( testActive_ )
        {
            // enable button
            calibrate_->setEnabled( true );
            calibrate_->setPalette( calibratePalette_ );

            testActive_ = false;
        }
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::onCalibrateClicked()
{
    // nothing to do when test active
    if ( testActive_ )
        return;

    const QString msg(
        "26," +                                 // CalibrateWrapper
        mechValue_->text() + "," +              // Mechanism
        trayLength_->toSessionField() );        // TrayLength

    // start the test!
    session_->send( Wrapper, WrEvolTest, msg );

    // disable button temporarily
    QPalette p( calibratePalette_ );
    p.setBrush( QPalette::Button, hobart::colorPalette::red );

    calibrate_->setEnabled( false );
    calibrate_->setPalette( p );

    testActive_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::onRadioButtonClicked( QWidget *widget )
{
    if ( mechanisms_.contains( widget ) )
        mechValue_->setText( QString::number( mechanisms_[widget] ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::initialize()
{
    QFont titleFont( font() );
    titleFont.setPointSize( 26 );

    QFont bodyFont( font() );
    bodyFont.setPointSize( 22 );

    modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    modifyPaletteBackground whiteBackground( hobart::colorPalette::white );

    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    radioButtonClicked_ = new QSignalMapper( this );

    connect( radioButtonClicked_, SIGNAL( mapped(QWidget*) ), SLOT( onRadioButtonClicked(QWidget*)) );

    // ---- //

    // screen title
    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setAutoFillBackground( true );
    title_->setFont( titleFont );
    title_->setObjectName( "title" );

    blackBackground( title_ );
    whiteForeground( title_ );

    // ---- //

    // all mechanisms
    allMechanisms_ = new QRadioButton( this );
    allMechanisms_->setAutoFillBackground( true );
    allMechanisms_->setChecked( true );
    allMechanisms_->setFont( bodyFont );
    allMechanisms_->setObjectName( "allMechanisms" );

    whiteBackground( allMechanisms_ );

    connect( allMechanisms_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( allMechanisms_, allMechanisms_ );

    // mechanical value
    mechValue_ = new QLabel( this );
    mechValue_->setAlignment( Qt::AlignCenter );
    mechValue_->setFont( bodyFont );
    mechValue_->setObjectName( "mechValue" );
    mechValue_->setText( "127" );

    whiteBackground( allMechanisms_ );

    // primary
    primary_ = new QRadioButton( this );
    primary_->setAutoFillBackground( true );
    primary_->setFont( bodyFont );
    primary_->setObjectName( "primary" );

    whiteBackground( primary_ );

    connect( primary_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( primary_, primary_ );

    // shifter
    shifter_ = new QRadioButton( this );
    shifter_->setAutoFillBackground( true );
    shifter_->setFont( bodyFont );
    shifter_->setObjectName( "shifter" );

    whiteBackground( shifter_ );

    connect( shifter_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( shifter_, shifter_ );

    // gripper
    gripper_ = new QRadioButton( this );
    gripper_->setAutoFillBackground( true );
    gripper_->setFont( bodyFont );
    gripper_->setObjectName( "gripper" );

    whiteBackground( gripper_ );

    connect( gripper_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( gripper_, gripper_ );

    // side clamps
    sideClamps_ = new QRadioButton( this );
    sideClamps_->setAutoFillBackground( true );
    sideClamps_->setFont( bodyFont );
    sideClamps_->setObjectName( "sideClamps" );

    whiteBackground( sideClamps_ );

    connect( sideClamps_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( sideClamps_, sideClamps_ );

    // pusher
    pusher_ = new QRadioButton( this );
    pusher_->setAutoFillBackground( true );
    pusher_->setFont( bodyFont );
    pusher_->setObjectName( "pusher" );

    whiteBackground( pusher_ );

    connect( pusher_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( pusher_, pusher_ );

    // front underfolder
    frontUnderfolder_ = new QRadioButton( this );
    frontUnderfolder_->setAutoFillBackground( true );
    frontUnderfolder_->setFont( bodyFont );
    frontUnderfolder_->setObjectName( "frontUnderfolder" );

    whiteBackground( frontUnderfolder_ );

    connect( frontUnderfolder_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( frontUnderfolder_, frontUnderfolder_ );

    // rear underfolder
    rearUnderfolder_ = new QRadioButton( this );
    rearUnderfolder_->setAutoFillBackground( true );
    rearUnderfolder_->setFont( bodyFont );
    rearUnderfolder_->setObjectName( "rearUnderfolder" );

    whiteBackground( rearUnderfolder_ );

    connect( rearUnderfolder_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( rearUnderfolder_, rearUnderfolder_ );

    // side underfolders
    sideUnderfolders_ = new QRadioButton( this );
    sideUnderfolders_->setAutoFillBackground( true );
    sideUnderfolders_->setFont( bodyFont );
    sideUnderfolders_->setObjectName( "sideUnderfolders" );

    whiteBackground( sideUnderfolders_ );

    connect( sideUnderfolders_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( sideUnderfolders_, sideUnderfolders_ );

    // actuators
    actuators_ = new QRadioButton( this );
    actuators_->setAutoFillBackground( true );
    actuators_->setFont( bodyFont );
    actuators_->setObjectName( "actuators" );

    whiteBackground( actuators_ );

    connect( actuators_, SIGNAL( clicked() ), radioButtonClicked_, SLOT( map() ) );
    radioButtonClicked_->setMapping( actuators_, actuators_ );

    // ---- //

    // tray length label
    trayLengthLabel_ = new QLabel( this );
    trayLengthLabel_->setAlignment( Qt::AlignCenter );
    trayLengthLabel_->setFont( bodyFont );
    trayLengthLabel_->setObjectName( "trayLengthLabel" );

    // tray length
    trayLength_ = new hobart::numericLineEdit( this );
    trayLength_->setDecimalPlaces( 1 );
    trayLength_->setFixedWidth( 200 );
    trayLength_->setFont( bodyFont );
    trayLength_->setInputTruncation( true );
    trayLength_->setInputWrapping( true );
    trayLength_->setValidator( new QIntValidator( 0, 100, this ) );
    trayLength_->setObjectName( "trayLength" );

    trayLength_->fromInt( 0 );

    // max length
    maxLength_ = new QLabel( this );
    maxLength_->setAlignment( Qt::AlignCenter );
    maxLength_->setObjectName( "maxLength" );

    // calibrate
    calibrate_ = new QPushButton( this );
    calibrate_->setFont( bodyFont );
    calibrate_->setFixedHeight( 64 );
    calibrate_->setFixedWidth( 300 );
    calibrate_->setObjectName( "calibrate" );

    calibratePalette_ = calibrate_->palette();

    connect( calibrate_, SIGNAL( clicked() ), SLOT( onCalibrateClicked() ) );

    // ---- //

    // generate mapping of mechanism values
    mechanisms_[allMechanisms_] = 127;
    mechanisms_[primary_] = 0;
    mechanisms_[shifter_] = 2;
    mechanisms_[gripper_] = 3;
    mechanisms_[sideClamps_] = 4;
    mechanisms_[pusher_] = 5;
    mechanisms_[frontUnderfolder_] = 6;
    mechanisms_[rearUnderfolder_] = 9;
    mechanisms_[sideUnderfolders_] = 13;
    mechanisms_[actuators_] = 120;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperCalibration::createLayout()
{
    QVBoxLayout *leftSide( new QVBoxLayout() );
    leftSide->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    leftSide->addStretch();
    leftSide->addWidget( allMechanisms_ );
    leftSide->addWidget( mechValue_ );
    leftSide->addWidget( primary_ );
    leftSide->addWidget( shifter_ );
    leftSide->addWidget( gripper_ );
    leftSide->addWidget( sideClamps_ );
    leftSide->addWidget( pusher_ );
    leftSide->addWidget( frontUnderfolder_ );
    leftSide->addWidget( rearUnderfolder_ );
    leftSide->addWidget( sideUnderfolders_ );
    leftSide->addWidget( actuators_ );
    leftSide->addStretch();

    QHBoxLayout *trayLengthLayout( new QHBoxLayout() );
    trayLengthLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    trayLengthLayout->addStretch();
    trayLengthLayout->addWidget( trayLength_ );
    trayLengthLayout->addStretch();

    QHBoxLayout *calibrateLayout( new QHBoxLayout() );
    calibrateLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    calibrateLayout->addStretch();
    calibrateLayout->addWidget( calibrate_ );
    calibrateLayout->addStretch();

    QVBoxLayout *rightSide( new QVBoxLayout() );
    rightSide->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    rightSide->addStretch();
    rightSide->addWidget( trayLengthLabel_ );
    rightSide->addLayout( trayLengthLayout );
    rightSide->addWidget( maxLength_ );
    rightSide->addStretch();
    rightSide->addLayout( calibrateLayout );
    rightSide->addStretch();

    QHBoxLayout *body( new QHBoxLayout() );
    body->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    body->addLayout( leftSide, 1 );
    body->addLayout( rightSide, 1 );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addWidget( title_ );
    screenLayout->addLayout( body, 1 );

    if( session_->uiUtilities->isWrapOnly() )
    {
        shifter_->hide();
    }
}

