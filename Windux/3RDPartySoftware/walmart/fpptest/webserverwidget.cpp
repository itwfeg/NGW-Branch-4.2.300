/**
 * @file webserverwidget.cpp
 * Web server widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webserverwidget.h"

#include <QtDebug>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTcpSocket>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
webServerWidget::webServerWidget( QWidget *parent ) :
    _Mybase( parent ),
    state_( webServerState::Okay )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webServerWidget::~webServerWidget()
{
    closeWebServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::setState( webServerState::type value )
{
    if ( value == state_ )
        return;

    Q_EMIT stateChanged( state_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::closeWebServer()
{
    if ( server_.isListening() )
        server_.close();

    close_->setEnabled( false );
    open_->setEnabled( true );
    port_->setEnabled( true );
    options_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::openWebServer()
{
    const QIntValidator *v( dynamic_cast<const QIntValidator*>( port_->validator() ) );

    // validate port
    QString portStr( port_->text() );
    int pos( 0 );

    if (( !v ) || ( QValidator::Acceptable != v->validate( portStr, pos ) ))
    {
        qDebug() << "invalid port" << portStr;
        return;
    }

    // start server
    if ( !server_.listen( QHostAddress::Any, portStr.toUShort() ) )
    {
        qDebug() << "failed to open server" << server_.errorString();
        return;
    }

    close_->setEnabled( true );
    open_->setEnabled( false );
    port_->setEnabled( false );
    options_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::processClientBuffer( QTcpSocket *client, QByteArray &buffer )
{
    static const QByteArray crlf( "\r\n" );

    // retrieve header
    QStringList header;

    int headerLen( 0 );
    int offset;

    while ( -1 != (offset = buffer.indexOf( crlf, headerLen )) )
    {
        const QString s( buffer.mid( headerLen, offset - headerLen ) );

        headerLen = offset + crlf.length();

        // we have whole header
        if ( !s.length() )
            break;

        header.append( s );
        qDebug() << s;
    }

    // we do not have the entire header yet
    if ( offset < 0 )
        return;

    // check for body
    QByteArray body;

    foreach ( const QString &s, header )
        if ( s.contains( "Content-Length: " ) )
        {
            int contentLength( s.mid( 15 ).toInt() );

            if ( contentLength )
            {
                // we do not have entire body yet
                if (  buffer.length() < (headerLen + contentLength) )
                    return;

                body = buffer.mid( headerLen, contentLength );
            }
        }

    // pull entire message off buffer
    buffer = buffer.mid( headerLen + body.length() );

    // process
    Q_EMIT processRequest( client, header, body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onClientDestroyed()
{
    QTcpSocket *client( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( client ) && ( clientBuffers_.contains( client ) ))
    {
        qDebug() << "client removed" << client;
        clientBuffers_.remove( client );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onClientReadyRead()
{
    QTcpSocket *client( dynamic_cast<QTcpSocket*>( sender() ) );

    if ( client )
    {
        QByteArray buffer;

        // retrieve existing buffer
        if ( clientBuffers_.contains( client ) )
            buffer = clientBuffers_[client];

        // read socket
        buffer.append( client->readAll() );

        // process
        processClientBuffer( client, buffer );

        // save state
        clientBuffers_[client] = buffer;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onPushButtonClicked()
{
    // close
    if ( close_ == sender() )
        closeWebServer();

    // open
    else if ( open_ == sender() )
        openWebServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onRadioButtonClicked()
{
    webServerState::type newState;

    // handle state
    if ( okay_ == sender() )
        newState = webServerState::Okay;
    else if ( okayNoContent_ == sender() )
        newState = webServerState::OkayNoContent;
    else if ( incompleteHeader_ == sender() )
        newState = webServerState::IncompleteHeader;
    else if ( malformedHeader_ == sender() )
        newState = webServerState::MalformedHeader;
    else if ( noResponse_ == sender() )
        newState = webServerState::NoResponse;
    else if ( closeSocket_ == sender() )
        newState = webServerState::CloseSocket;
    else if ( moved_ == sender() )
        newState = webServerState::Moved;
    else if ( unauthorized_ == sender() )
        newState = webServerState::Unauthorized;
    else if ( notFound_ == sender() )
        newState = webServerState::NotFound;
    else
    {
        return;
    }

    setState( newState );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onServerAcceptError( QAbstractSocket::SocketError error )
{
    qDebug() << "server accept error" << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::onServerNewConnection()
{
    // accept all clients
    while ( server_.hasPendingConnections() )
    {
        QTcpSocket *client( server_.nextPendingConnection() );

        connect( client, SIGNAL(destroyed()), SLOT(onClientDestroyed()) );
        connect( client, SIGNAL(disconnected()), client, SLOT(deleteLater()) );
        connect( client, SIGNAL(readyRead()), SLOT(onClientReadyRead()) );

        qDebug() << "client added" << client;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::initialize()
{
    connect( &server_, SIGNAL(acceptError(QAbstractSocket::SocketError)), SLOT(onServerAcceptError(QAbstractSocket::SocketError)) );
    connect( &server_, SIGNAL(newConnection()), SLOT(onServerNewConnection()) );

    portLabel_ = new QLabel( tr( "Port:" ), this );

    port_ = new QLineEdit( this );
    port_->setText( "80" );
    port_->setValidator( new QIntValidator( 1, 65535, this ) );

    open_ = new QPushButton( tr( "Open" ), this );
    connect( open_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    close_ = new QPushButton( tr( "Close" ), this );
    close_->setEnabled( false );
    connect( close_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    allowLegacy_ = new QPushButton( tr( "Legacy FPP" ), this );
    allowLegacy_->setCheckable( true );
    connect( allowLegacy_, SIGNAL(toggled(bool)), SIGNAL(legacySupportEnabled(bool)) );

    options_ = new QGroupBox( tr( "Server Options" ), this );
    options_->setEnabled( false );

    okay_ = new QRadioButton( tr( "Okay" ), options_ );
    okay_->setChecked( true );
    connect( okay_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    okayNoContent_ = new QRadioButton( tr( "Okay (204)" ), options_ );
    connect( okayNoContent_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    incompleteHeader_ = new QRadioButton( tr( "Incomplete Header" ), options_ );
    connect( incompleteHeader_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    malformedHeader_ = new QRadioButton( tr( "Bad Header" ), options_ );
    connect( malformedHeader_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    noResponse_ = new QRadioButton( tr( "Do not respond" ), options_ );
    connect( noResponse_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    closeSocket_ = new QRadioButton( tr( "Close socket" ), options_ );
    connect( closeSocket_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    moved_ = new QRadioButton( tr( "Moved (301)" ), options_ );
    connect( moved_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    unauthorized_ = new QRadioButton( tr( "Unauthorized (401)" ), options_ );
    connect( unauthorized_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));

    notFound_ = new QRadioButton( tr( "Not Found (404)" ), options_ );
    connect( notFound_, SIGNAL(clicked()), SLOT(onRadioButtonClicked()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webServerWidget::createLayout()
{
    QHBoxLayout *server( new QHBoxLayout() );
    server->addWidget( portLabel_ );
    server->addWidget( port_ );
    server->addWidget( open_ );
    server->addWidget( close_ );
    server->addStretch();
    server->addWidget( allowLegacy_ );

    QHBoxLayout *options( new QHBoxLayout( options_ ) );
    options->addWidget( okay_ );
    options->addWidget( okayNoContent_ );
    options->addWidget( incompleteHeader_ );
    options->addWidget( malformedHeader_ );
    options->addWidget( noResponse_ );
    options->addWidget( closeSocket_ );
    options->addWidget( moved_ );
    options->addWidget( unauthorized_ );
    options->addWidget( notFound_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addLayout( server );
    form->addWidget( options_ );
}
