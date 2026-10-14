/**
 * @file operatorlogindialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "operatorlogindialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QFormLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>

///////////////////////////////////////////////////////////////////////////////////////////////////
OperatorLoginDialog::OperatorLoginDialog( LoginType loginType, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    loginType_( loginType ),
    accepted_( false ),
    ignoreEvents_( true ),
    processAcceptLogin_( false )
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "OperatorLoginDialog" );

    LOG_INFO( "login type is %d", loginType_ );

    // set object name
    setObjectName( "OperatorLoginDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperatorLoginDialog::~OperatorLoginDialog()
{
    if ( !accepted_ )
        rejectLogin();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperatorLoginDialog::allowOverlayClickDismiss() const
{
    // do not allow dismiss when remote login
    if ( isRemoteLoginType() )
        return false;

    return _Mybase::allowOverlayClickDismiss();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperatorLoginDialog::isRemoteLoginType() const
{
    return (( RemoteOperatorIdRequired == loginType() ) || ( RemoteUsernamePasswordRequired == loginType() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int OperatorLoginDialog::priority() const
{
    // prompt real time priority when remote login
    if ( isRemoteLoginType() )
        return RealTime;

    return _Mybase::priority();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::keyPressEvent( QKeyEvent *e )
{
    // Note:
    // The QDialogButtonBox class will intercept the enter key and trigger an accept() on the
    // dialog. In order to prevent this lets intercept it ourselves and ignore it.
    if (( Qt::Key_Enter == e->key() ) || ( Qt::Key_Return == e->key() ))
    {
        e->accept();
        return;
    }

    _Mybase::keyPressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::keyReleaseEvent( QKeyEvent *e )
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "keyReleaseEvent" );

    // Note:
    // The QDialogButtonBox class will intercept the enter key and trigger an accept() on the
    // dialog. In order to prevent this lets intercept it ourselves and ignore it.
    if (( Qt::Key_Enter == e->key() ) || ( Qt::Key_Return == e->key() ))
    {
        e->accept();

        // accept login
        if ( processAcceptLogin_ )
        {
            LOG_DEBUG( "processing login...", NULL );

            acceptLogin( inputText() );
            accept();
        }

        // done
        return;
    }

    _Mybase::keyReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    // set focus to input widget
    if ( RemoteUsernamePasswordRequired == loginType() )
        userNameLineEdit_->setFocus();

    ignoreEvents_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onButtonClicked( QAbstractButton *button )
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "onButtonClicked" );

    if ( ignoreEvents_ )
        return;

    LOG_TRACE( "button clicked 0x%08x", button );

    if ( !button )
        return;

    // check for ok button pressed
    if ( QDialogButtonBox::AcceptRole == buttonRole( button ) )
    {
        acceptLogin( inputText() );
        accept();
    }

    // check for cancel button pressed
    else if ( QDialogButtonBox::RejectRole == buttonRole( button ) )
    {
        LOG_WARN( "login cancelled... terminating...", NULL );

        // terminate session
        session_->send( SessionDev, SnTerminate, "1" );
        accepted_ = true;

        reject();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onInputEditingFinished()
{
    // do nothing
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onInputReturnPressed()
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "onInputReturnPressed" );

    if ( ignoreEvents_ )
        return;

    LOG_TRACE( "return pressed", NULL );

    // set flag to complete form on key release
    processAcceptLogin_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    LOG_TRACE( "device %d event %d", device, event );

    // dismiss numberpad
    if (( SessionDev == device ) && ( SnDismissVirtualNumberpad == event ))
    {
        LOG_DEBUG( "rejecting dialog...", NULL );
        reject();
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onUserNameReturnKeyPressed()
{
    if ( ignoreEvents_ )
        return;

    // set focus to password
    input_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::onUserNameReturnKeyPressedInternal()
{
    onUserNameReturnKeyPressed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::initialize()
{
    QString text;

    // user name label
    userNameLabel_ = new QLabel( tr( "User Name:" ), this );
    userNameLabel_->setObjectName( "userNameLabel" );
    userNameLabel_->setVisible( false );

    // user name line edit
    userNameLineEdit_ = new QLineEdit( this );
    userNameLineEdit_->setObjectName( "userNameLineEdit" );
    userNameLineEdit_->setVisible( false );

    connect( userNameLineEdit_, SIGNAL( editingFinished() ), SLOT( onUserNameReturnKeyPressedInternal() ) );

    // password label
    passwordLabel_ = new QLabel( tr( "Password:" ), this );
    passwordLabel_->setObjectName( "passwordLabel" );
    passwordLabel_->setVisible( false );

    // create layout
    QFormLayout *inputLayout = new QFormLayout();
    inputLayout->addRow( userNameLabel_, userNameLineEdit_ );
    inputLayout->addRow( passwordLabel_, input_ );

    // modify existing layout
    messageAreaInner_->insertLayout( messageAreaInner_->indexOf( input_ ), inputLayout );
    messageAreaInner_->removeWidget( input_ );

    // Cutting Test has to know when we are loggin in
    if ( OperatorLoginRequiredForCutTest == loginType() )
        session_->scaleConfig->operatorLoggedInForCutTest = true;

    if ( OpLoginRequiredForCutTestHistory == loginType() )
        session_->scaleConfig->opLoggedInForCutTestHistory = true;

    // session requires operator id
    // remote operator id required
    if (( OperatorLoginRequired == loginType() ) ||
        ( OperatorLoginRequiredFieldAccess == loginType() ) ||
        ( OperatorLoginRequiredForCutTest == loginType()  ) ||
        ( OpLoginRequiredForCutTestHistory == loginType()  ) ||
        ( RemoteOperatorIdRequired == loginType() ))
    {
        MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;

        // access to field that requires operator login
        if ( OperatorLoginRequiredFieldAccess == loginType() )
            text += tr( "You must be logged in to edit this field.\n\n" );

        // check for RFID
        if ( config->isRFIDHWPresent() )
            text += tr( "Enter operator ID, or use ID tag" );
        else
            text += tr( "Enter operator ID" );

        // check for numeric keyboard operator login
        // 1 = qwerty
        // 2 = numeric keypad
        if ( 2 == config->operatorLoginKeyboardStyle() )
            setValidator( new QIntValidator( this ) );
    }

    // remote user name and password required
    else if ( RemoteUsernamePasswordRequired == loginType() )
    {
        text = tr( "Enter user name and password" );

        userNameLabel_->setVisible( true );
        userNameLineEdit_->setVisible( true );

        passwordLabel_->setVisible( true );

        // fix tab order
        setTabOrder( userNameLineEdit_, input_ );
    }

    // set message text
    setText( text );

    // set input
    setEchoMode( QLineEdit::Password );

    // add okay button to dialog
    addButton( QDialogButtonBox::Ok );

    // remote login dialog style
    if ( isRemoteLoginType() )
    {
        // remote login uses centered dialog
        setLocation( CenterDialogLocation );

        // remote login allows cancel
        addButton( QDialogButtonBox::Cancel );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::acceptLogin( const QString& id )
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "acceptLogin" );

    // session requires operator id
    if (( OperatorLoginRequired == loginType() ) ||
        ( OperatorLoginRequiredForCutTest == loginType()  ) ||
        ( OpLoginRequiredForCutTestHistory == loginType()  ) ||
        ( OperatorLoginRequiredFieldAccess == loginType() ))
    {
        if (( id.length() ) && ( !accepted_ ))
        {
            LOG_INFO( "sending operator login message '%s'", qPrintable( id ) );

            session_->send( Error, ErrRemoveAnyModalDialogDevice, QString::number( 102009 ) + ",2,1," + id );
            accepted_ = true;
        }
    }

    // remote operator id required
    else if ( RemoteOperatorIdRequired == loginType() )
    {
        if (( id.length() ) && ( !accepted_ ))
        {
            LOG_INFO( "sending operator login message id '%s'", qPrintable( id ) );

            session_->send( OperatorDev, OpRemoteAccessCredentials, "\"" + id + "\"" );
            accepted_ = true;
        }
    }

    // remote user name and password required
    else if ( RemoteUsernamePasswordRequired == loginType() )
    {
        QString userName( userNameLineEdit_->text() );

        if ((( userName.length() ) || ( id.length() )) && ( !accepted_ ))
        {
            LOG_INFO( "sending operator login message id '%s' pwd '*****'", qPrintable( userName ) );

            session_->send( OperatorDev, OpRemoteAccessCredentials, "\"" + userName + "\",\"" + id + "\"" );
            accepted_ = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperatorLoginDialog::rejectLogin()
{
    LOG_CLASS_INFO( "OperatorLoginDialog", "rejectLogin" );

    LOG_INFO( "operator rejected login", NULL );

    // session requires operator id
    if (( OperatorLoginRequired == loginType() ) ||
        ( OperatorLoginRequiredForCutTest == loginType()  ) ||
        ( OpLoginRequiredForCutTestHistory == loginType()  ) ||
        ( OperatorLoginRequiredFieldAccess == loginType() ))
    {
        LOG_DEBUG( "sending reject operator login message", NULL );
        session_->send( Error, ErrRemoveAnyModalDialogDevice, QString::number( 102009 ) + ",2,0" );
    }

    // remote operator id required
    else if ( RemoteOperatorIdRequired == loginType() )
    {
        LOG_DEBUG( "sending reject operator credentials", NULL );
        session_->send( OperatorDev, OpRemoteAccessCredentials, "\"\"" );
    }

    // remote user name and password required
    else if ( RemoteUsernamePasswordRequired == loginType() )
    {
        LOG_DEBUG( "sending reject operator credentials", NULL );
        session_->send( OperatorDev, OpRemoteAccessCredentials, "\"\",\"\"" );
    }
}
