/**
 * @file serviceNewWrapperTestKnife.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperTestKnife.h"

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
serviceNewWrapperTestKnife::serviceNewWrapperTestKnife( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    testActive_( false )
{
    setObjectName( "serviceNewWrapperTestKnife" );

    // initialize
    initialize();
    createLayout();
    translate();

    // request knife times
    session_->send( Wrapper, WrRequestKnifeTimeHistory );

    // request sensor status
    session_->send( Wrapper, WrReqSensors );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestKnife::~serviceNewWrapperTestKnife()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestKnife::translate()
{
    title_->setText( tr( "Knife Tests" ) );

    knifeUpDownLabel_->setText( tr( "Knife Up / Down" ) );
    knifeUp_->setText( tr( "Up" ) );
    knifeDown_->setText( tr( "Down" ) );

    minLabel_->setText( tr( "Min" ) );
    installLabel_->setText( tr( "Install" ) );
    maxLabel_->setText( tr( "Max" ) );
    currentLabel_->setText( tr( "Current" ) );
    knifeUpTimeLabel_->setText( tr( "Knife Up Time (ms)" ) );
    dateUpLabel_->setText( tr( "Date Recorded" ) );
    knifeDownTimeLabel_->setText( tr( "Knife Down Time (ms)" ) );
    dateDownLabel_->setText( tr( "Date Recorded" ) );

    sensorsTitle_->setText( tr( "Sensors" ) );
    knifeUpSensorLabel_->setText( tr( "Knife Up" ) );
    knifeDownSensorLabel_->setText( tr( "Knife Down" ) );
    regulatedPressureLabel_->setText( tr( "Regulated Air Pressure" ) );

    clearInstall_->setText( tr( "Clear Install Time" ) );
    cycleWrapperButton_->setText( tr( "Cycle Wrapper" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestKnife::onScreenChanging()
{
    // cannot leave this screen until test complete
    if ( testActive_ )
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestKnife::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // knife time
    if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        DomEvolWrapperEventCapture m( msg );

        if ( DomEvolWrapperEventCapture::RegulatedPressureReadingEvent == m.captureType() )
            regulatedPressure_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::KnifeUpEvent == m.captureType() )
            eventUpTime_->setText( QString::number( m.value() ) );
        else if ( DomEvolWrapperEventCapture::KnifeDownEvent == m.captureType() )
            eventDownTime_->setText( QString::number( m.value() ) );
    }

    // knife time history
    else if (( Wrapper == device ) && ( WrKnifeTimeHistory == event ))
    {
        DomEvolWrapperKnifeHist m( msg );

        knifeUpTimeMin_->setText( QString::number( m.knifeUpTimeMin() ) );
        knifeUpTimeSetup_->setText( QString::number( m.knifeUpTimeInstall() ) );
        knifeUpTimeMax_->setText( QString::number( m.knifeUpTimeMax() ) );

        dateUpMin_->setText( m.knifeUpTimeMinDate() );
        dateUpSetup_->setText( m.knifeUpTimeInstallDate() );
        dateUpMax_->setText( m.knifeUpTimeMaxDate() );

        knifeDownTimeMin_->setText( QString::number( m.knifeDownTimeMin() ) );
        knifeDownTimeSetup_->setText( QString::number( m.knifeDownTimeInstall() ) );
        knifeDownTimeMax_->setText( QString::number( m.knifeDownTimeMax() ) );

        dateDownMin_->setText( m.knifeDownTimeMinDate() );
        dateDownSetup_->setText( m.knifeDownTimeInstallDate() );
        dateDownMax_->setText( m.knifeDownTimeMaxDate() );
    }

    // wrapper sensors
    else if (( Wrapper == device ) && ( WrEvolSensors == event ))
    {
        modifyPaletteBackground greenBackground( hobart::colorPalette::green );
        modifyPaletteBackground redBackground( hobart::colorPalette::red );

        DomEvolWrapperSensors m( msg );

        // set sensor states
        if ( m.knifeUp() )
            greenBackground( knifeUpSensor_ );
        else
            redBackground( knifeUpSensor_ );

        if ( m.knifeDown() )
            greenBackground( knifeDownSensor_ );
        else
            redBackground( knifeDownSensor_ );
    }

    // wrapper test complete
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ) && ( testActive_ ))
    {
        // enable buttons
        knifeUp_->setEnabled( true );
        knifeUp_->setPalette( pushButtonPalette_ );

        knifeDown_->setEnabled( true );
        knifeDown_->setPalette( pushButtonPalette_ );

        cycleWrapperButton_->setEnabled( true );
        cycleWrapperButton_->setPalette( pushButtonPalette_ );

        testActive_ = false;
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestKnife::onPushButtonClicked()
{
    QPushButton *pushButtonWrapperTest( NULL );

    if (( knifeUp_ == sender() ) && ( !testActive_ ))
    {
        // knife up
        session_->send( Wrapper, WrEvolTest, "7,0" ); // KnifeTest, Up (0)
        pushButtonWrapperTest = knifeUp_;
    }
    else if (( knifeDown_ == sender() ) && ( !testActive_ ))
    {
        // knife down
        session_->send( Wrapper, WrEvolTest, "7,1" ); // KnifeTest, Down (1)
        pushButtonWrapperTest = knifeDown_;
    }
    else if ( clearInstall_ == sender() )
    {
        // clear install history
        session_->send( Wrapper, WrClearInstallKnifeTime );
    }
    else if (( cycleWrapperButton_ == sender() ) && ( !testActive_ ))
    {
        // cycle wrapper
        session_->send( Wrapper, WrEvolTest, "24" ); // WrapperCycle
        pushButtonWrapperTest = cycleWrapperButton_;
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
void serviceNewWrapperTestKnife::initialize()
{
    QFont titleFont( font() );
    titleFont.setPointSize( 26 );

    QFont bodyFont( font() );
    bodyFont.setPointSize( 22 );

    modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    modifyPaletteBackground greenBackground( hobart::colorPalette::green );
    modifyPaletteBackground whiteBackground( hobart::colorPalette::white );

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

    // ---- //

    // knife up / down
    knifeUpDownLabel_ = new QLabel( this );
    knifeUpDownLabel_->setFont( bodyFont );
    knifeUpDownLabel_->setObjectName( "knifeUpDownLabel" );

    // knife up
    knifeUp_ = new QPushButton( this );
    knifeUp_->setFont( bodyFont );
    knifeUp_->setObjectName( "knifeUp" );

    connect( knifeUp_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // knife down
    knifeDown_ = new QPushButton( this );
    knifeDown_->setFont( bodyFont );
    knifeDown_->setObjectName( "knifeDown" );

    connect( knifeDown_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // ---- //

    // min label
    minLabel_ = new QLabel( this );
    minLabel_->setAlignment( Qt::AlignCenter );
    minLabel_->setAutoFillBackground( true );
    minLabel_->setFont( bodyFont );
    minLabel_->setObjectName( "minLabel" );

    blackBackground( minLabel_ );
    whiteForeground( minLabel_ );

    // install label
    installLabel_ = new QLabel( this );
    installLabel_->setAlignment( Qt::AlignCenter );
    installLabel_->setAutoFillBackground( true );
    installLabel_->setFont( bodyFont );
    installLabel_->setObjectName( "installLabel" );

    blackBackground( installLabel_ );
    whiteForeground( installLabel_ );

    // max label
    maxLabel_ = new QLabel( this );
    maxLabel_->setAlignment( Qt::AlignCenter );
    maxLabel_->setAutoFillBackground( true );
    maxLabel_->setFont( bodyFont );
    maxLabel_->setObjectName( "maxLabel" );

    blackBackground( maxLabel_ );
    whiteForeground( maxLabel_ );

    // current label
    currentLabel_ = new QLabel( this );
    currentLabel_->setAlignment( Qt::AlignCenter );
    currentLabel_->setAutoFillBackground( true );
    currentLabel_->setFont( bodyFont );
    currentLabel_->setObjectName( "currentLabel" );

    greenBackground( currentLabel_ );
    whiteForeground( currentLabel_ );

    // ---- //

    // knife up time
    knifeUpTimeLabel_ = new QLabel( this );
    knifeUpTimeLabel_->setAlignment( Qt::AlignCenter );
    knifeUpTimeLabel_->setAutoFillBackground( true );
    knifeUpTimeLabel_->setFont( bodyFont );
    knifeUpTimeLabel_->setObjectName( "knifeUpTimeLabel" );

    blackBackground( knifeUpTimeLabel_ );
    whiteForeground( knifeUpTimeLabel_ );

    // knife up time min
    knifeUpTimeMin_ = new QLabel( this );
    knifeUpTimeMin_->setAlignment( Qt::AlignCenter );
    knifeUpTimeMin_->setFont( bodyFont );
    knifeUpTimeMin_->setObjectName( "knifeUpTimeMin" );

    // knife up time setup
    knifeUpTimeSetup_ = new QLabel( this );
    knifeUpTimeSetup_->setAlignment( Qt::AlignCenter );
    knifeUpTimeSetup_->setFont( bodyFont );
    knifeUpTimeSetup_->setObjectName( "knifeUpTimeSetup" );

    // knife up time max
    knifeUpTimeMax_ = new QLabel( this );
    knifeUpTimeMax_->setAlignment( Qt::AlignCenter );
    knifeUpTimeMax_->setFont( bodyFont );
    knifeUpTimeMax_->setObjectName( "knifeUpTimeMax" );

    // event up time
    eventUpTime_ = new QLabel( this );
    eventUpTime_->setAlignment( Qt::AlignCenter );
    eventUpTime_->setFont( bodyFont );
    eventUpTime_->setObjectName( "eventUpTime" );

    // ---- //

    // date
    dateUpLabel_ = new QLabel( this );
    dateUpLabel_->setAlignment( Qt::AlignCenter );
    dateUpLabel_->setAutoFillBackground( true );
    dateUpLabel_->setFont( bodyFont );
    dateUpLabel_->setObjectName( "dateUpLabel" );

    blackBackground( dateUpLabel_ );
    whiteForeground( dateUpLabel_ );

    // date min
    dateUpMin_ = new QLabel( this );
    dateUpMin_->setAlignment( Qt::AlignCenter );
    dateUpMin_->setFont( bodyFont );
    dateUpMin_->setObjectName( "dateUpMin" );

    // date setup
    dateUpSetup_ = new QLabel( this );
    dateUpSetup_->setAlignment( Qt::AlignCenter );
    dateUpSetup_->setFont( bodyFont );
    dateUpSetup_->setObjectName( "dateUpSetup" );

    // date max
    dateUpMax_ = new QLabel( this );
    dateUpMax_->setAlignment( Qt::AlignCenter );
    dateUpMax_->setFont( bodyFont );
    dateUpMax_->setObjectName( "dateUpMax" );

    // ---- //

    // knife down time
    knifeDownTimeLabel_ = new QLabel( this );
    knifeDownTimeLabel_->setAlignment( Qt::AlignCenter );
    knifeDownTimeLabel_->setAutoFillBackground( true );
    knifeDownTimeLabel_->setFont( bodyFont );
    knifeDownTimeLabel_->setObjectName( "knifeDownTimeLabel" );

    blackBackground( knifeDownTimeLabel_ );
    whiteForeground( knifeDownTimeLabel_ );

    // knife down time min
    knifeDownTimeMin_ = new QLabel( this );
    knifeDownTimeMin_->setAlignment( Qt::AlignCenter );
    knifeDownTimeMin_->setFont( bodyFont );
    knifeDownTimeMin_->setObjectName( "knifeDownTimeMin" );

    // knife down time setup
    knifeDownTimeSetup_ = new QLabel( this );
    knifeDownTimeSetup_->setAlignment( Qt::AlignCenter );
    knifeDownTimeSetup_->setFont( bodyFont );
    knifeDownTimeSetup_->setObjectName( "knifeDownTimeSetup" );

    // knife down time max
    knifeDownTimeMax_ = new QLabel( this );
    knifeDownTimeMax_->setAlignment( Qt::AlignCenter );
    knifeDownTimeMax_->setFont( bodyFont );
    knifeDownTimeMax_->setObjectName( "knifeDownTimeMax" );

    // event up time
    eventDownTime_ = new QLabel( this );
    eventDownTime_->setAlignment( Qt::AlignCenter );
    eventDownTime_->setFont( bodyFont );
    eventDownTime_->setObjectName( "eventDownTime" );

    // ---- //

    // date
    dateDownLabel_ = new QLabel( this );
    dateDownLabel_->setAlignment( Qt::AlignCenter );
    dateDownLabel_->setAutoFillBackground( true );
    dateDownLabel_->setFont( bodyFont );
    dateDownLabel_->setObjectName( "dateDownLabel" );

    blackBackground( dateDownLabel_ );
    whiteForeground( dateDownLabel_ );

    // date min
    dateDownMin_ = new QLabel( this );
    dateDownMin_->setAlignment( Qt::AlignCenter );
    dateDownMin_->setFont( bodyFont );
    dateDownMin_->setObjectName( "dateDownMin" );

    // date setup
    dateDownSetup_ = new QLabel( this );
    dateDownSetup_->setAlignment( Qt::AlignCenter );
    dateDownSetup_->setFont( bodyFont );
    dateDownSetup_->setObjectName( "dateDownSetup" );

    // date max
    dateDownMax_ = new QLabel( this );
    dateDownMax_->setAlignment( Qt::AlignCenter );
    dateDownMax_->setFont( bodyFont );
    dateDownMax_->setObjectName( "dateDownMax" );

    // ---- //

    // sensors title
    sensorsTitle_ = new QLabel( this );
    sensorsTitle_->setAlignment( Qt::AlignCenter );
    sensorsTitle_->setAutoFillBackground( true );
    sensorsTitle_->setFont( titleFont );
    sensorsTitle_->setObjectName( "sensorsTitle" );

    blackBackground( sensorsTitle_ );
    whiteForeground( sensorsTitle_ );

    // knife up sensor label
    knifeUpSensorLabel_ = new QLabel( this );
    knifeUpSensorLabel_->setFont( bodyFont );
    knifeUpSensorLabel_->setObjectName( "knifeUpSensorLabel" );

    // knife up sensor
    knifeUpSensor_ = new QLabel( this );
    knifeUpSensor_->setAutoFillBackground( true );
    knifeUpSensor_->setFont( bodyFont );
    knifeUpSensor_->setObjectName( "knifeUpSensor" );

    // knife down sensor label
    knifeDownSensorLabel_ = new QLabel( this );
    knifeDownSensorLabel_->setFont( bodyFont );
    knifeDownSensorLabel_->setObjectName( "knifeDownSensorLabel" );

    // knife down sensor
    knifeDownSensor_ = new QLabel( this );
    knifeDownSensor_->setAutoFillBackground( true );
    knifeDownSensor_->setFont( bodyFont );
    knifeDownSensor_->setObjectName( "knifeDownSensor" );

    // regulated pressure label
    regulatedPressureLabel_ = new QLabel( this );
    regulatedPressureLabel_->setFont( bodyFont );
    regulatedPressureLabel_->setObjectName( "regulatedPressureLabel" );

    // regulated pressure
    regulatedPressure_ = new QLabel( this );
    regulatedPressure_->setAlignment( Qt::AlignCenter );
    regulatedPressure_->setFont( bodyFont );
    regulatedPressure_->setObjectName( "regulatedPressure" );

    // ---- //

    // clear install
    clearInstall_ = new QPushButton( this );
    clearInstall_->setFont( bodyFont );
    clearInstall_->setObjectName( "clearInstall" );

    connect( clearInstall_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // cycle wrapper
    cycleWrapperButton_ = new QPushButton( this );
    cycleWrapperButton_->setFont( bodyFont );
    cycleWrapperButton_->setObjectName( "cycleWrapperButton" );

    connect( cycleWrapperButton_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // save off palette for future use
    pushButtonPalette_ = cycleWrapperButton_->palette();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestKnife::createLayout()
{
    QHBoxLayout *knifeButtonLayout( new QHBoxLayout() );

    knifeButtonLayout->addWidget( knifeUp_ );
    knifeButtonLayout->addWidget( knifeDown_ );

    QHBoxLayout *knifeLayout( new QHBoxLayout() );
    knifeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    knifeLayout->addWidget( knifeUpDownLabel_, 1 );
    knifeLayout->addLayout( knifeButtonLayout, 1 );

    // ---- //

    QVBoxLayout *chartLabelsLayout( new QVBoxLayout() );
    chartLabelsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    chartLabelsLayout->setSpacing( 1 );

    chartLabelsLayout->addStretch( 1 );
    chartLabelsLayout->addWidget( minLabel_, 1 );
    chartLabelsLayout->addWidget( installLabel_, 1 );
    chartLabelsLayout->addWidget( maxLabel_, 1 );
    chartLabelsLayout->addWidget( currentLabel_, 1 );

    QVBoxLayout *knifeUpTimeLayout( new QVBoxLayout() );
    knifeUpTimeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    knifeUpTimeLayout->setSpacing( 1 );

    knifeUpTimeLayout->addWidget( knifeUpTimeLabel_, 1 );
    knifeUpTimeLayout->addWidget( knifeUpTimeMin_, 1 );
    knifeUpTimeLayout->addWidget( knifeUpTimeSetup_, 1 );
    knifeUpTimeLayout->addWidget( knifeUpTimeMax_, 1 );
    knifeUpTimeLayout->addWidget( eventUpTime_, 1 );

    QVBoxLayout *dateUpLayout( new QVBoxLayout() );
    dateUpLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    dateUpLayout->setSpacing( 1 );

    dateUpLayout->addWidget( dateUpLabel_, 1 );
    dateUpLayout->addWidget( dateUpMin_, 1 );
    dateUpLayout->addWidget( dateUpSetup_, 1 );
    dateUpLayout->addWidget( dateUpMax_, 1 );
    dateUpLayout->addStretch( 1 );

    QVBoxLayout *knifeDownTimeLayout( new QVBoxLayout() );
    knifeDownTimeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    knifeDownTimeLayout->setSpacing( 1 );

    knifeDownTimeLayout->addWidget( knifeDownTimeLabel_, 1 );
    knifeDownTimeLayout->addWidget( knifeDownTimeMin_, 1 );
    knifeDownTimeLayout->addWidget( knifeDownTimeSetup_, 1 );
    knifeDownTimeLayout->addWidget( knifeDownTimeMax_, 1 );
    knifeDownTimeLayout->addWidget( eventDownTime_, 1 );

    QVBoxLayout *dateDownLayout( new QVBoxLayout() );
    dateDownLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    dateDownLayout->setSpacing( 1 );

    dateDownLayout->addWidget( dateDownLabel_, 1 );
    dateDownLayout->addWidget( dateDownMin_, 1 );
    dateDownLayout->addWidget( dateDownSetup_, 1 );
    dateDownLayout->addWidget( dateDownMax_, 1 );
    dateDownLayout->addStretch( 1 );

    QHBoxLayout *chartLayout( new QHBoxLayout() );
    chartLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    chartLayout->setSpacing( 1 );

    chartLayout->addStretch();
    chartLayout->addLayout( chartLabelsLayout, 2 );
    chartLayout->addLayout( knifeUpTimeLayout, 3 );
    chartLayout->addLayout( dateUpLayout, 3 );
    chartLayout->addLayout( knifeDownTimeLayout, 3 );
    chartLayout->addLayout( dateDownLayout, 3 );
    chartLayout->addStretch();

    // ---- //

    QHBoxLayout *knifeUpSensorLayout( new QHBoxLayout() );
    knifeUpSensorLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    knifeUpSensorLayout->addWidget( knifeUpSensorLabel_, 3 );
    knifeUpSensorLayout->addWidget( knifeUpSensor_, 1 );

    QHBoxLayout *knifeDownSensorLayout( new QHBoxLayout() );
    knifeDownSensorLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    knifeDownSensorLayout->addWidget( knifeDownSensorLabel_, 3 );
    knifeDownSensorLayout->addWidget( knifeDownSensor_, 1 );

    QHBoxLayout *regulatedPressureLayout( new QHBoxLayout() );
    regulatedPressureLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    regulatedPressureLayout->addWidget( regulatedPressureLabel_, 3 );
    regulatedPressureLayout->addWidget( regulatedPressure_, 1 );

    QVBoxLayout *sensorsLayoutActual( new QVBoxLayout() );
    sensorsLayoutActual->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sensorsLayoutActual->addWidget( sensorsTitle_ );
    sensorsLayoutActual->addLayout( knifeUpSensorLayout );
    sensorsLayoutActual->addLayout( knifeDownSensorLayout );
    sensorsLayoutActual->addSpacing( 20 );
    sensorsLayoutActual->addLayout( regulatedPressureLayout );

    QHBoxLayout *sensorsLayout( new QHBoxLayout() );
    sensorsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sensorsLayout->addStretch( 1 );
    sensorsLayout->addLayout( sensorsLayoutActual, 2 );
    sensorsLayout->addStretch( 1 );

    // ---- //

    QHBoxLayout *buttonLayout( new QHBoxLayout() );

    buttonLayout->addStretch();
    buttonLayout->addWidget( clearInstall_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( cycleWrapperButton_ );
    buttonLayout->addStretch();

    // ---- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addWidget( title_ );
    screenLayout->addLayout( knifeLayout );
    screenLayout->addStretch();
    screenLayout->addLayout( chartLayout );
    screenLayout->addStretch();
    screenLayout->addLayout( sensorsLayout );
    screenLayout->addStretch();
    screenLayout->addLayout( buttonLayout );
}

