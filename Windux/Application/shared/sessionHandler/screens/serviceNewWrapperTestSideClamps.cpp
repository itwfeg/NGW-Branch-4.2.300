/**
 * @file serviceNewWrapperTestSideClamps.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperTestSideClamps.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestSideClamps::serviceNewWrapperTestSideClamps( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    testActive_( false )
{
    setObjectName( "serviceNewWrapperTestSideClamps" );

    wrapperSystemInformation_ = &session_->scaleConfig->evolWrapperSystemInformation;

    // initialize
    initialize();
    createLayout();
    translate();

    // request system info
    session_->send( Wrapper, WrReqEvolSystemInfo );

    // request sensor status
    session_->send( Wrapper, WrReqSensors );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestSideClamps::~serviceNewWrapperTestSideClamps()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSideClamps::translate()
{
    title_->setText( tr( "Side Clamp Tests" ) );

    sideClampsLabel_->setText( tr( "Side clamps (N2)" ) );
    clampsOn_->setText( tr( "On" ) );
    clampsOff_->setText( tr( "Off" ) );

    cycleLowerFilmButton_->setText( tr( "Cycle lower film" ) );
    finishButton_->setText( tr( "Finish" ) );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
        cycleUpperFilmButton_->setText( tr( "Cycle film" ) );
    else
        cycleUpperFilmButton_->setText( tr( "Cycle upper film" ) );

    sensorsTitle_->setText( tr( "Sensors" ) );
    homeSensorLabel_->setText( tr( "Home" ) );
    rightClampSensorLabel_->setText( tr( "Right Clamp" ) );
    leftClampSensorLabel_->setText( tr( "Left Clamp" ) );
    regulatedPressureLabel_->setText( tr( "Regulated Air Pressure" ) );

    actuationTitle_->setText( tr( "Actuation Times" ) );
    actuationOpenTitle_->setText( tr( "Open" ) );
    actuationCloseTitle_->setText( tr( "Close" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestSideClamps::onScreenChanging()
{
    // cannot leave this screen until test complete
    if ( testActive_ )
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSideClamps::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // wrapper info
    if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        // lower film only enabled for AWS wrapper, it does not exist
        cycleLowerFilmButton_->setEnabled( wrapperSystemInformation_->isDoubleRoll() );
    }

    // wrapper sensors
    else if (( Wrapper == device ) && ( WrEvolSensors == event ))
    {
        static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
        static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

        DomEvolWrapperSensors m( msg );

        // set sensor states
        homeSensor_->setStyleSheet( m.sideClampHome() ? greenStyle : redStyle );
        rightClampSensor_->setStyleSheet( m.sideClampRight() ? greenStyle : redStyle );
        leftClampSensor_->setStyleSheet( m.sideClampLeft() ? greenStyle : redStyle );
    }

    // wrapper event capture
    else if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        DomEvolWrapperEventCapture m( msg );

        if ( DomEvolWrapperEventCapture::RegulatedPressureReadingEvent == m.captureType() )
            regulatedPressure_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::SideClampRightOpenEvent == m.captureType() )
            rightClampOpen_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::SideClampRightCloseEvent == m.captureType() )
            rightClampClose_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::SideClampLeftOpenEvent == m.captureType() )
            leftClampOpen_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::SideClampLeftCloseEvent == m.captureType() )
            leftClampClose_->setText( QString::number( m.value() ) );
    }

    // wrapper test complete
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ) && ( testActive_ ))
    {
        // enable buttons
        clampsOn_->setEnabled( true );
        clampsOn_->setPalette( pushButtonPalette_ );

        clampsOff_->setEnabled( true );
        clampsOff_->setPalette( pushButtonPalette_ );

        cycleLowerFilmButton_->setEnabled( wrapperSystemInformation_->isDoubleRoll() );
        cycleLowerFilmButton_->setPalette( pushButtonPalette_ );

        //finishButton_->setEnabled( true ); do not enable finish button, only enabled upon cycle lower/upper
        finishButton_->setPalette( pushButtonPalette_ );

        cycleUpperFilmButton_->setEnabled( true );
        cycleUpperFilmButton_->setPalette( pushButtonPalette_ );

        testActive_ = false;
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSideClamps::onPushButtonClicked()
{
    QPushButton *pushButtonWrapperTest( NULL );

    // dont process button when actively testing
    if ( testActive_ )
        return;

    if ( clampsOn_ == sender() )
    {
        // side clamps on
        session_->send( Wrapper, WrEvolTest, "14,1" ); // SideClampsTest, On (1)
        pushButtonWrapperTest = clampsOn_;
    }
    else if ( clampsOff_ == sender() )
    {
        // side clamps off
        session_->send( Wrapper, WrEvolTest, "14,0" ); // SideClampsTest, Off (0)
        pushButtonWrapperTest = clampsOff_;
    }
    else if ( cycleLowerFilmButton_ == sender() )
    {
        // cycle lower
        session_->send( Wrapper, WrEvolTest, "14,3" ); // SideClampsTest, CycleLower (3)
        pushButtonWrapperTest = cycleLowerFilmButton_;

        finishButton_->setEnabled( true );
    }
    else if ( finishButton_ == sender() )
    {
        // cycle finish
        session_->send( Wrapper, WrEvolTest, "14,4" ); // SideClampsTest, CycleFinish (4)
        pushButtonWrapperTest = finishButton_;
    }
    else if ( cycleUpperFilmButton_ == sender() )
    {
        // cycle upper
        session_->send( Wrapper, WrEvolTest, "14,2" ); // SideClampsTest, CycleUpper (2)
        pushButtonWrapperTest = cycleUpperFilmButton_;

        finishButton_->setEnabled( true );
    }

    // disable test button button temporarily...
    if ( pushButtonWrapperTest )
    {
        QPalette p( pushButtonPalette_ );
        p.setBrush( QPalette::Button, hobart::colorPalette::red );

        pushButtonWrapperTest->setEnabled( false );
        pushButtonWrapperTest->setPalette( p );

        testActive_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSideClamps::initialize()
{
    QFont titleFont( font() );
    titleFont.setPointSize( 26 );

    QFont bodyFont( font() );
    bodyFont.setPointSize( 22 );

    modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    // ---- //

    // screen title
    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setAutoFillBackground( true );
    title_->setFont( titleFont );
    title_->setObjectName( "title" );

    blackBackground( title_ );
    whiteForeground( title_ );

    // side clamps on / off
    sideClampsLabel_ = new QLabel( this );
    sideClampsLabel_->setFont( bodyFont );
    sideClampsLabel_->setObjectName( "sideClampsLabel" );

    // clamps on
    clampsOn_ = new QPushButton( this );
    clampsOn_->setFont( bodyFont );
    clampsOn_->setObjectName( "clampsOn" );

    connect( clampsOn_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // clamps off
    clampsOff_ = new QPushButton( this );
    clampsOff_->setFont( bodyFont );
    clampsOff_->setObjectName( "clampsOff" );

    connect( clampsOff_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // ---- //

    // cycle lower
    cycleLowerFilmButton_ = new QPushButton( this );
    cycleLowerFilmButton_->setEnabled( wrapperSystemInformation_->isDoubleRoll() );
    cycleLowerFilmButton_->setFont( bodyFont );
    cycleLowerFilmButton_->setObjectName( "cycleLowerFilmButton" );

    connect( cycleLowerFilmButton_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // finish
    finishButton_ = new QPushButton( this );
    finishButton_->setEnabled( false );
    finishButton_->setFont( bodyFont );
    finishButton_->setObjectName( "finishButton" );

    connect( finishButton_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // cycle upper
    cycleUpperFilmButton_ = new QPushButton( this );
    cycleUpperFilmButton_->setFont( bodyFont );
    cycleUpperFilmButton_->setObjectName( "cycleUpperFilmButton" );

    connect( cycleUpperFilmButton_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // ---- //

    // sensors title spacer
    sensorsTitleSpacer_ = new QLabel( this );
    sensorsTitleSpacer_->setAlignment( Qt::AlignCenter );
    sensorsTitleSpacer_->setAutoFillBackground( true );
    sensorsTitleSpacer_->setFont( titleFont );
    sensorsTitleSpacer_->setObjectName( "sensorsTitleSpacer" );
    sensorsTitleSpacer_->setText( " " );

    // sensors title
    sensorsTitle_ = new QLabel( this );
    sensorsTitle_->setAlignment( Qt::AlignCenter );
    sensorsTitle_->setAutoFillBackground( true );
    sensorsTitle_->setFont( titleFont );
    sensorsTitle_->setObjectName( "sensorsTitle" );

    blackBackground( sensorsTitle_ );
    whiteForeground( sensorsTitle_ );

    // home sensor label
    homeSensorLabel_ = new QLabel( this );
    homeSensorLabel_->setFont( bodyFont );
    homeSensorLabel_->setObjectName( "homeSensorLabel" );

    // home sensor
    homeSensor_ = new QLabel( this );
    homeSensor_->setAutoFillBackground( true );
    homeSensor_->setFont( bodyFont );
    homeSensor_->setObjectName( "homeSensor" );
    homeSensor_->setAlignment( Qt::AlignHCenter );

    // right clamp sensor label
    rightClampSensorLabel_ = new QLabel( this );
    rightClampSensorLabel_->setFont( bodyFont );
    rightClampSensorLabel_->setObjectName( "rightClampSensorLabel" );

    // right clamp sensor
    rightClampSensor_ = new QLabel( this );
    rightClampSensor_->setAutoFillBackground( true );
    rightClampSensor_->setFont( bodyFont );
    rightClampSensor_->setObjectName( "rightClampSensor" );
    rightClampSensor_->setAlignment( Qt::AlignHCenter );

    // left clamp sensor label
    leftClampSensorLabel_ = new QLabel( this );
    leftClampSensorLabel_->setFont( bodyFont );
    leftClampSensorLabel_->setObjectName( "leftClampSensorLabel" );

    // left clamp sensor
    leftClampSensor_ = new QLabel( this );
    leftClampSensor_->setAutoFillBackground( true );
    leftClampSensor_->setFont( bodyFont );
    leftClampSensor_->setObjectName( "leftClampSensor" );
    leftClampSensor_->setAlignment( Qt::AlignHCenter );

    // regulated pressure label
    regulatedPressureLabel_ = new QLabel( this );
    regulatedPressureLabel_->setFont( bodyFont );
    regulatedPressureLabel_->setObjectName( "regulatedPressureLabel" );

    // regulated pressure
    regulatedPressure_ = new QLabel( this );
    regulatedPressure_->setFont( bodyFont );
    regulatedPressure_->setObjectName( "regulatedPressure" );
    regulatedPressure_->setAlignment( Qt::AlignHCenter );

    // ---- //

    // actuation title
    actuationTitle_ = new QLabel( this );
    actuationTitle_->setAlignment( Qt::AlignCenter );
    actuationTitle_->setAutoFillBackground( true );
    actuationTitle_->setFont( titleFont );
    actuationTitle_->setObjectName( "actuationTitle" );

    blackBackground( actuationTitle_ );
    whiteForeground( actuationTitle_ );

    // actuation open title
    actuationOpenTitle_ = new QLabel( this );
    actuationOpenTitle_->setAlignment( Qt::AlignCenter );
    actuationOpenTitle_->setAutoFillBackground( true );
    actuationOpenTitle_->setFont( titleFont );
    actuationOpenTitle_->setObjectName( "actuationOpenTitle" );

    blackBackground( actuationOpenTitle_ );
    whiteForeground( actuationOpenTitle_ );

    // actuation close title
    actuationCloseTitle_ = new QLabel( this );
    actuationCloseTitle_->setAlignment( Qt::AlignCenter );
    actuationCloseTitle_->setAutoFillBackground( true );
    actuationCloseTitle_->setFont( titleFont );
    actuationCloseTitle_->setObjectName( "actuationCloseTitle" );

    blackBackground( actuationCloseTitle_ );
    whiteForeground( actuationCloseTitle_ );

    // clamp open spacer
    clampOpenSpacer_ = new QLabel( this );
    clampOpenSpacer_->setFont( bodyFont );
    clampOpenSpacer_->setObjectName( "clampOpenSpacer" );
    clampOpenSpacer_->setAlignment( Qt::AlignHCenter );
    clampOpenSpacer_->setText( " " );

    // clamp close spacer
    clampCloseSpacer_ = new QLabel( this );
    clampCloseSpacer_->setFont( bodyFont );
    clampCloseSpacer_->setObjectName( "clampCloseSpacer" );
    clampCloseSpacer_->setAlignment( Qt::AlignHCenter );
    clampCloseSpacer_->setText( " " );

    // right clamp open
    rightClampOpen_ = new QLabel( this );
    rightClampOpen_->setFont( bodyFont );
    rightClampOpen_->setObjectName( "rightClampOpen" );
    rightClampOpen_->setAlignment( Qt::AlignHCenter );
    rightClampOpen_->setText( " " );

    // right clamp close
    rightClampClose_ = new QLabel( this );
    rightClampClose_->setFont( bodyFont );
    rightClampClose_->setObjectName( "rightClampClose" );
    rightClampClose_->setAlignment( Qt::AlignHCenter );
    rightClampClose_->setText( " " );

    // left clamp open
    leftClampOpen_ = new QLabel( this );
    leftClampOpen_->setFont( bodyFont );
    leftClampOpen_->setObjectName( "leftClampOpen" );
    leftClampOpen_->setAlignment( Qt::AlignHCenter );
    leftClampOpen_->setText( " " );

    // left clamp close
    leftClampClose_ = new QLabel( this );
    leftClampClose_->setFont( bodyFont );
    leftClampClose_->setObjectName( "leftClampClose" );
    leftClampClose_->setAlignment( Qt::AlignHCenter );
    leftClampClose_->setText( " " );

    // save off palette for future use
    pushButtonPalette_ = finishButton_->palette();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSideClamps::createLayout()
{
    QHBoxLayout *sideClampsButtonLayout( new QHBoxLayout() );

    sideClampsButtonLayout->addWidget( clampsOn_ );
    sideClampsButtonLayout->addWidget( clampsOff_ );

    QHBoxLayout *sideClampsLayout( new QHBoxLayout() );
    sideClampsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sideClampsLayout->addWidget( sideClampsLabel_, 1 );
    sideClampsLayout->addLayout( sideClampsButtonLayout, 1 );

    // ---- //

    QHBoxLayout *buttonLayout( new QHBoxLayout() );

    buttonLayout->addWidget( cycleLowerFilmButton_ );
    buttonLayout->addWidget( finishButton_ );
    buttonLayout->addWidget( cycleUpperFilmButton_ );

    // ---- //

    QHBoxLayout *sideClampsSensorLayout( new QHBoxLayout() );
    sideClampsSensorLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sideClampsSensorLayout->addWidget( homeSensorLabel_, 3 );
    sideClampsSensorLayout->addWidget( homeSensor_, 1 );

    QHBoxLayout *rightClampsSensorLayout( new QHBoxLayout() );
    rightClampsSensorLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    rightClampsSensorLayout->addWidget( rightClampSensorLabel_, 3 );
    rightClampsSensorLayout->addWidget( rightClampSensor_, 1 );

    QHBoxLayout *leftClampsSensorLayout( new QHBoxLayout() );
    leftClampsSensorLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    leftClampsSensorLayout->addWidget( leftClampSensorLabel_, 3 );
    leftClampsSensorLayout->addWidget( leftClampSensor_, 1 );

    QHBoxLayout *regulatedPressureLayout( new QHBoxLayout() );
    regulatedPressureLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    regulatedPressureLayout->addWidget( regulatedPressureLabel_, 3 );
    regulatedPressureLayout->addWidget( regulatedPressure_, 1 );

    QVBoxLayout *sideClampsLayoutActual( new QVBoxLayout() );
    sideClampsLayoutActual->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sideClampsLayoutActual->addWidget( sensorsTitleSpacer_ );
    sideClampsLayoutActual->addWidget( sensorsTitle_ );
    sideClampsLayoutActual->addLayout( sideClampsSensorLayout );
    sideClampsLayoutActual->addLayout( rightClampsSensorLayout );
    sideClampsLayoutActual->addLayout( leftClampsSensorLayout );
    sideClampsLayoutActual->addSpacing( 20 );
    sideClampsLayoutActual->addLayout( regulatedPressureLayout );
    sideClampsLayoutActual->addStretch();

    // ---- //

    QHBoxLayout *actuationHeaderLayout( new QHBoxLayout() );
    actuationHeaderLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    actuationHeaderLayout->addWidget( actuationOpenTitle_, 1 );
    actuationHeaderLayout->addWidget( actuationCloseTitle_, 1 );

    QHBoxLayout *actuationSpacerLayout( new QHBoxLayout() );
    actuationSpacerLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    actuationSpacerLayout->addWidget( clampOpenSpacer_, 3 );
    actuationSpacerLayout->addWidget( clampCloseSpacer_, 1 );

    QHBoxLayout *actuationRightLayout( new QHBoxLayout() );
    actuationRightLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    actuationRightLayout->addWidget( rightClampOpen_, 3 );
    actuationRightLayout->addWidget( rightClampClose_, 1 );

    QHBoxLayout *actuationLeftLayout( new QHBoxLayout() );
    actuationLeftLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    actuationLeftLayout->addWidget( leftClampOpen_, 3 );
    actuationLeftLayout->addWidget( leftClampClose_, 1 );

    QVBoxLayout *actuationLayoutActual( new QVBoxLayout() );
    actuationLayoutActual->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    actuationLayoutActual->addWidget( actuationTitle_ );
    actuationLayoutActual->addLayout( actuationHeaderLayout );
    actuationLayoutActual->addLayout( actuationSpacerLayout );
    actuationLayoutActual->addLayout( actuationRightLayout );
    actuationLayoutActual->addLayout( actuationLeftLayout );
    actuationLayoutActual->addStretch();

    // ---- //

    QHBoxLayout *sensorsLayout( new QHBoxLayout() );
    sensorsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sensorsLayout->addStretch( 1 );
    sensorsLayout->addLayout( sideClampsLayoutActual, 4 );
    sensorsLayout->addLayout( actuationLayoutActual, 2 );
    sensorsLayout->addStretch( 1 );

    // ---- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addWidget( title_ );
    screenLayout->addLayout( sideClampsLayout );
    screenLayout->addStretch( 1 );
    screenLayout->addLayout( buttonLayout );
    screenLayout->addStretch( 1 );
    screenLayout->addLayout( sensorsLayout );
    screenLayout->addStretch( 5 );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
        cycleLowerFilmButton_->hide();
}
