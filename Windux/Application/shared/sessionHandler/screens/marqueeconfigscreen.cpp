/**
 * @file marqueeconfigscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueeconfigscreen.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/supervisormode.h"

#include "../screens/marqueeeditorscreen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeConfigScreen::MarqueeConfigScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    marqueeConfig_( NULL ),
    blockEvents_( false ),
    saveOnExit_( false )
{
    // initialize screen
    initialize();
    translate();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        marqueeConfig_ = &dynamic_cast<SupervisorMode*>( mode_ )->marqueeConfig_;

    // read marquee configuration
    session_->send( ConfigurationDev, CfgMarqueeRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeConfigScreen::~MarqueeConfigScreen()
{
    // if user clicked the marquee text area it is possible we popped the keyboard... lets hide it
    mainWindow_->hideKeyboard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeConfigScreen::translate()
{
    marqueeLabel_->setText( tr( "Display the Permanent Marquee Message" ) );
    marqueeEditButton_->setText( tr( "Edit" ) );
    marqueeClearButton_->setText( tr( "Clear" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *MarqueeConfigScreen::swipeScreenLeft() const
{
    if ( mainWindow_->isSupervisorMode() )
        return new MarqueeEditorScreen( mainWindow_, mode_ );

    return _Mybase::swipeScreenLeft();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeConfigScreen::eventFilter( QObject *obj, QEvent *event )
{
    // check for clicking the marquee text box
    if ( marqueeText_ == obj )
    {
        if ( QEvent::FocusIn == event->type() )
            onMarqueeEditClicked();
    }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeConfigScreen::onScreenChanging()
{
    // save settings
    if (( saveOnExit_ ) && ( marqueeConfig_ ))
    {
        // save unquoted text
        QString temp( marqueeConfig_->permanentMarqueeText() );

        // save the values
        marqueeConfig_->setField( marqueeConfig_->event(), CfgMarqueeSave );

        // make sure marquee text is quoted
        marqueeConfig_->setField(
            marqueeConfig_->permanentMarqueeText(),
            marqueeConfig_->permanentMarqueeText() );

        // send!!
        session_->send( (SessionMessage) *marqueeConfig_ );

        // restore unquoted text
        marqueeConfig_->permanentMarqueeText() = temp;
    }

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeConfigScreen::onSessionMessage( const SessionMessage& msg )
{
    // marquee config
    if (( ConfigurationDev == msg[0].toInt() ) && ( CfgMarqueeWrite == msg[1].toInt() ))
    {
        DomMarqueeConfig m( msg );

        // update user interface
        marqueeText_->setPlainText( m.permanentMarqueeTextStripped() );
        saveOnExit_ = false;

        // update config
        if ( marqueeConfig_ )
            *marqueeConfig_ = m;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeConfigScreen::onMarqueeEditClicked()
{
    if ( mainWindow_->isSupervisorMode() )
        mainWindow_->switchToModeScreen<SupervisorMode, MarqueeEditorScreen>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeConfigScreen::onMarqueeClearClicked()
{
    // update user interface
    marqueeText_->setPlainText( "" );

    // update config
    if ( marqueeConfig_ )
    {
        marqueeConfig_->permanentMarqueeText() = "";
        saveOnExit_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeConfigScreen::initialize()
{
    QHBoxLayout *horizontalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->setMargin( marginSpacing_ );

    ///////////////////////////////////////////////////////////////////////////
    // marquee

    marqueeLabel_ = new QLabel( this );
    marqueeLabel_->setObjectName( "marqueeLabel" );

    marqueeEditButton_ = new QPushButton( this );
    marqueeEditButton_->setObjectName( "marqueeEditButton" );

    marqueeClearButton_ = new QPushButton( this );
    marqueeClearButton_->setObjectName( "marqueeClearButton" );

    marqueeText_ = new QPlainTextEdit( this );
    marqueeText_->setObjectName( "marqueeText" );
    marqueeText_->setReadOnly( true );

    connect( marqueeEditButton_, SIGNAL( clicked() ), this, SLOT( onMarqueeEditClicked() ) );
    connect( marqueeClearButton_, SIGNAL( clicked() ), this, SLOT( onMarqueeClearClicked() ) );

    // we need to hook into the marquee text for when it is clicked
    marqueeText_->installEventFilter( this );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget( marqueeLabel_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( marqueeEditButton_ );
    horizontalLayout->addWidget( marqueeClearButton_ );

    screenLayout->addLayout( horizontalLayout );
    screenLayout->addWidget( marqueeText_ );

    screenLayout->addStretch();
}
