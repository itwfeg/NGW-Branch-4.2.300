/**
 * @file serviceNewWrapperTestUnderfolders.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperTestUnderfolders.h"

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
serviceNewWrapperTestUnderfolders::serviceNewWrapperTestUnderfolders( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    testActive_( false )
{
    setObjectName( "serviceNewWrapperTestUnderfolders" );

    // initialize
    initialize();
    createLayout();
    translate();

    // request sensor status
    session_->send( Wrapper, WrReqSensors );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestUnderfolders::~serviceNewWrapperTestUnderfolders()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUnderfolders::translate()
{
    sensorsLabel_->setText( tr( "Underfolder Sensors" ) );
    sideHomeLabel_->setText( tr( "Side home" ) );
    frontHomeLabel_->setText( tr( "Front home" ) );
    rearHomeLabel_->setText( tr( "Rear home" ) );

    testLabel_->setText( tr( "Underfolder Tests" ) );
    cycleSide_->setText( tr( "Cycle Side" ) );
    cycleFront_->setText( tr( "Cycle Front" ) );
    cycleRear_->setText( tr( "Cycle Rear" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUnderfolders::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // update height of sensor labels to align chart
    sideHomeLabel_->setFixedHeight( cycleSide_->height() );
    sideHome_->setFixedHeight( cycleSide_->height() );
    frontHomeLabel_->setFixedHeight( cycleFront_->height() );
    frontHome_->setFixedHeight( cycleFront_->height() );
    rearHomeLabel_->setFixedHeight( cycleRear_->height() );
    rearHome_->setFixedHeight( cycleRear_->height() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestUnderfolders::onScreenChanging()
{
    // cannot leave this screen until test complete
    if ( testActive_ )
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUnderfolders::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // wrapper sensors
    if (( Wrapper == device ) && ( WrEvolSensors == event ))
    {
        DomEvolWrapperSensors m( msg );

        modifyPaletteBackground greenBackground( hobart::colorPalette::green );
        modifyPaletteBackground redBackground( hobart::colorPalette::red );

        // side folder sensor state
        if ( m.sideFolderHome() )
            greenBackground( sideHome_ );
        else
            redBackground( sideHome_ );

        // front folder sensor state
        if ( m.frontFolderHome() )
            greenBackground( frontHome_ );
        else
            redBackground( frontHome_ );

        // rear folder sensor state
        if ( m.rearFolderHome() )
            greenBackground( rearHome_ );
        else
            redBackground( rearHome_ );
    }

    // wrapper test complete
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ) && ( testActive_ ))
    {
        // enable buttons
        cycleSide_->setEnabled( true );
        cycleSide_->setPalette( pushButtonPalette_ );

        cycleFront_->setEnabled( true );
        cycleFront_->setPalette( pushButtonPalette_ );

        cycleRear_->setEnabled( true );
        cycleRear_->setPalette( pushButtonPalette_ );

        testActive_ = false;
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUnderfolders::onPushButtonClicked( QWidget *widget )
{
    QPushButton *pushButtonWrapperTest( NULL );

    if (( cycleSide_ == widget ) && ( !testActive_ ))
    {
        // knife up
        session_->send( Wrapper, WrEvolTest, "23" ); // SideFolderTest
        pushButtonWrapperTest = cycleSide_;
    }
    else if (( cycleFront_ == widget ) && ( !testActive_ ))
    {
        // knife down
        session_->send( Wrapper, WrEvolTest, "16" ); // FrontFolderTest
        pushButtonWrapperTest = cycleFront_;
    }
    else if (( cycleRear_ == widget ) && ( !testActive_ ))
    {
        // cycle wrapper
        session_->send( Wrapper, WrEvolTest, "15" ); // RearFolderTest
        pushButtonWrapperTest = cycleRear_;
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
void serviceNewWrapperTestUnderfolders::initialize()
{
    QFont titleFont( font() );
    titleFont.setPointSize( 26 );

    QFont bodyFont( font() );
    bodyFont.setPointSize( 22 );

    modifyPaletteBackground blackBackground( hobart::colorPalette::black );

    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    pushButtonClicked_ = new QSignalMapper( this );

    connect( pushButtonClicked_, SIGNAL( mapped(QWidget*) ), SLOT( onPushButtonClicked(QWidget*)) );

    // ---- //

    // sensors label
    sensorsLabel_ = new QLabel( this );
    sensorsLabel_->setAlignment( Qt::AlignCenter );
    sensorsLabel_->setAutoFillBackground( true );
    sensorsLabel_->setFont( titleFont );
    sensorsLabel_->setObjectName( "sensorsLabel" );

    blackBackground( sensorsLabel_ );
    whiteForeground( sensorsLabel_ );

    // side home label
    sideHomeLabel_ = new QLabel( this );
    sideHomeLabel_->setFont( bodyFont );
    sideHomeLabel_->setObjectName( "sideHomeLabel" );

    // side home
    sideHome_ = new QLabel( this );
    sideHome_->setAutoFillBackground( true );
    sideHome_->setFont( bodyFont );
    sideHome_->setObjectName( "sideHome" );

    // front home label
    frontHomeLabel_ = new QLabel( this );
    frontHomeLabel_->setFont( bodyFont );
    frontHomeLabel_->setObjectName( "frontHomeLabel" );

    // front home
    frontHome_ = new QLabel( this );
    frontHome_->setAutoFillBackground( true );
    frontHome_->setFont( bodyFont );
    frontHome_->setObjectName( "frontHome" );

    // rear home label
    rearHomeLabel_ = new QLabel( this );
    rearHomeLabel_->setFont( bodyFont );
    rearHomeLabel_->setObjectName( "rearHomeLabel" );

    // rear home
    rearHome_ = new QLabel( this );
    rearHome_->setAutoFillBackground( true );
    rearHome_->setFont( bodyFont );
    rearHome_->setObjectName( "rearHome" );

    // ---- //

    // test label
    testLabel_ = new QLabel( this );
    testLabel_->setAlignment( Qt::AlignCenter );
    testLabel_->setAutoFillBackground( true );
    testLabel_->setFont( titleFont );
    testLabel_->setObjectName( "testLabel" );

    blackBackground( testLabel_ );
    whiteForeground( testLabel_ );

    // cycle side
    cycleSide_ = new QPushButton( this );
    cycleSide_->setFont( bodyFont );
    cycleSide_->setObjectName( "cycleSide" );

    connect( cycleSide_, SIGNAL( clicked() ), pushButtonClicked_, SLOT( map() ) );
    pushButtonClicked_->setMapping( cycleSide_, cycleSide_ );

    // cycle front
    cycleFront_ = new QPushButton( this );
    cycleFront_->setFont( bodyFont );
    cycleFront_->setObjectName( "cycleFront" );

    connect( cycleFront_, SIGNAL( clicked() ), pushButtonClicked_, SLOT( map() ) );
    pushButtonClicked_->setMapping( cycleFront_, cycleFront_ );

    // cycle rear
    cycleRear_ = new QPushButton( this );
    cycleRear_->setFont( bodyFont );
    cycleRear_->setObjectName( "cycleRear" );

    connect( cycleRear_, SIGNAL( clicked() ), pushButtonClicked_, SLOT( map() ) );
    pushButtonClicked_->setMapping( cycleRear_, cycleRear_ );

    // ---- //

    // save off palette for future use
    pushButtonPalette_ = cycleRear_->palette();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestUnderfolders::createLayout()
{
    QHBoxLayout *sideHomeLayout( new QHBoxLayout() );
    sideHomeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    sideHomeLayout->addWidget( sideHomeLabel_, 3 );
    sideHomeLayout->addWidget( sideHome_, 1 );

    QHBoxLayout *frontHomeLayout( new QHBoxLayout() );
    frontHomeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    frontHomeLayout->addWidget( frontHomeLabel_, 3 );
    frontHomeLayout->addWidget( frontHome_, 1 );

    QHBoxLayout *rearHomeLayout( new QHBoxLayout() );
    rearHomeLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    rearHomeLayout->addWidget( rearHomeLabel_, 3 );
    rearHomeLayout->addWidget( rearHome_, 1 );

    QVBoxLayout *sensorsLayout( new QVBoxLayout() );

    sensorsLayout->addWidget( sensorsLabel_ );
    sensorsLayout->addLayout( sideHomeLayout );
    sensorsLayout->addLayout( frontHomeLayout );
    sensorsLayout->addLayout( rearHomeLayout );

    // ---- //

    QVBoxLayout *testsLayout( new QVBoxLayout() );

    testsLayout->addWidget( testLabel_ );
    testsLayout->addWidget( cycleSide_ );
    testsLayout->addWidget( cycleFront_ );
    testsLayout->addWidget( cycleRear_ );

    // ---- //

    QHBoxLayout *underfolderLayout( new QHBoxLayout() );

    underfolderLayout->addLayout( sensorsLayout, 1 );
    underfolderLayout->addLayout( testsLayout, 1 );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addLayout( underfolderLayout );
    screenLayout->addStretch();
}

