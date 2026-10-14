/**
 * @file loginpanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "loginpanel.h"

#include "../sessionmainwindow.h"
#include "../sessiondialog.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

/// Renderer traits type.
typedef hobart::modeLoginPanelBackgroundWidget rendererTraitsType;

///////////////////////////////////////////////////////////////////////////////////////////////////
LoginPanel::LoginPanel( ModeWidget *mode, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    loginMode_( mode )
{
    setFixedSize( sizeHint() );
    setObjectName( "LoginPanel" );

    // set font
    setFont( mainWindow_->font() );

    // initialize interface
    initialize();

    // connect into session
    connect( loginMode_, SIGNAL( operatorLoggedIn() ), SLOT( onOperatorLoggedIn() ) );
    connect( loginMode_, SIGNAL( usbKeyInserted() ), SLOT( onOperatorLoggedIn() ) );
    connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceive(SessionMessage) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LoginPanel::~LoginPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize LoginPanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::allowLoginOrServiceKey()
{
    title_->setText( tr( "Please login or insert your service key" ) );
    loginGroupBox_->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::allowServiceKeyOnly()
{
    title_->setText( tr( "Please insert your service key" ) );
    loginGroupBox_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::showEvent( QShowEvent *event )
{
    // handle base
    _Mybase::showEvent( event );

    // set focus to login widget
    timer_->setSingleShot( true );
    timer_->start( 0 );

    // request the version
    session_->send( SessionDev, SnVersionInfo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *LoginPanel::rendererTraits() const
{
    return new rendererTraitsType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::onLoginButtonClicked()
{
    // send login id
    loginMode_->login( loginId_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::onOperatorLoggedIn()
{
    // show unlocked mode panel
    if ( dynamic_cast<OperateMode*>( loginMode_ ) )
        mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode );
    else if ( dynamic_cast<SupervisorMode*>( loginMode_ ) )
        mainWindow_->showMenuPanel( SessionMainWindow::SupervisorMenuPanelMode );
    else if ( dynamic_cast<ServiceMode*>( loginMode_ ) )
        mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::onSessionMessageReceive( const SessionMessage& msg )
{
    // version message
    if (( SessionDev == msg[0].toInt() ) && ( SnVersionInfo == msg[1].toInt() ))
    {
        DomCurrentScaleSetup m( msg );

        if ( m.version().length() )
            scaleVersion_->setText( tr( "Version: " ) + m.version() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::onTimeout()
{
    if ( loginId_->isVisible() )
        loginId_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LoginPanel::initialize()
{
    MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;

    QHBoxLayout *horizontalLayout;

    modifyPaletteForeground setForegroundBlack( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );

    // create layout for widgets
    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    QVBoxLayout *layout = new QVBoxLayout();

    screenLayout->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    screenLayout->addStretch();

    // create timer
    timer_ = new QTimer( this );
    timer_->setObjectName( "timer" );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    ///////////////////////////////////////////////////////////////////////////
    // service mode buttons

    // add layout
    screenLayout->addLayout( layout );
    screenLayout->addStretch();

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );
    layout->addWidget( title_ );

    // login
    loginGroupBox_ = new QGroupBox( this );
    loginGroupBox_->setObjectName( "loginGroupBox" );
    QVBoxLayout *loginGroupBoxLayout = new QVBoxLayout( loginGroupBox_ );

    layout->addWidget( loginGroupBox_ );

    QLabel *loginLabel = new QLabel( tr( "Enter your login ID:" ), loginGroupBox_ );
    loginLabel->setObjectName( "loginLabel" );

    loginId_ = new QLineEdit( loginGroupBox_ );
    loginId_->setObjectName( "loginId" );

    // check for numeric keyboard operator login
    // 1 = qwerty
    // 2 = numeric keypad
    if ( dynamic_cast<ServiceMode*>( loginMode_ ) )
        ; // service mode always uses a qwerty keyboard
    else
    {
        if ( 2 == config->operatorLoginKeyboardStyle() )
            loginId_->setValidator( new QIntValidator( this ) );
    }

    loginId_->setEchoMode( QLineEdit::Password );
    setForegroundBlack( loginId_ );

    connect( loginId_, SIGNAL( returnPressed() ), SLOT( onLoginButtonClicked() ) );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget( loginLabel );
    horizontalLayout->addWidget( loginId_ );
    loginGroupBoxLayout->addLayout( horizontalLayout );

    QPushButton *loginButton = new QPushButton( tr( "Login" ), loginGroupBox_ );
    loginButton->setObjectName( "loginButton" );
    setForegroundBlack( loginButton );

    connect( loginButton, SIGNAL( clicked() ), SLOT( onLoginButtonClicked() ) );

    loginGroupBoxLayout->addWidget( loginButton );

    layout->addStretch();

    // application information
    QLabel *scaleDescription = new QLabel( session_->uiUtilities->scaleName(), this );
    scaleDescription->setObjectName( "scaleDescription" );

    scaleVersion_ = new QLabel( tr( "Version:" ), this );
    scaleVersion_->setObjectName( "scaleVersion" );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget( scaleDescription );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( scaleVersion_ );
    layout->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    if ( dynamic_cast<OperateMode*>( loginMode_ ) )
        layout = createMenuButtonLayout( OperateButtonSelected );
    else if ( dynamic_cast<SupervisorMode*>( loginMode_ ) )
        layout = createMenuButtonLayout( SupervisorButtonSelected );
    else if ( dynamic_cast<ServiceMode*>( loginMode_ ) )
        layout = createMenuButtonLayout( ServiceButtonSelected );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( layout );
    else
        screenLayout->insertLayout( 0, layout );
}
