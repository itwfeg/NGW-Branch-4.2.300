/**
 * @file proxyserverwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "proxyserverwidget.h"

#include <QtDebug>

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
proxyServerWidget::proxyServerWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
proxyServerWidget::~proxyServerWidget()
{
    closeProxyServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::closeProxyServer()
{
    if ( server_.isListening() )
        server_.close();

    close_->setEnabled( false );
    open_->setEnabled( true );
    port_->setEnabled( true );
    allowed_->setEnabled( true );
    options_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::openProxyServer()
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
    allowed_->setEnabled( false );
    options_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::processClientBuffer( QTcpSocket *client, QByteArray &buffer )
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

    // check if client is allowed
    clientAllowedResult rc( isClientAllowed( client, header ) );

    if ( ClientAllowed == rc )
    {
        // stop handling client, no longer care...
        client->disconnect( this, SLOT(onClientReadyRead()) );
    }
    else if ( ClientBadRequest == rc )
    {
        static const QString body( "<html><body>Say what?!?<br> </body></html>" );

        QByteArray response;
        response.append( "HTTP/1.0 400 Bad Request\r\n" );
        response.append( "Connection: close\r\n" );
        response.append( "Content-Type: text/html\r\n" );
        response.append( QString( "Content-Length: %0\r\n" ).arg( QString::number( body.length() ) ) );
        response.append( "\r\n" );
        response.append( body );

        client->write( response );
        client->close();

        return;
    }
    else if ( ClientForbidden == rc )
    {
        static const QString body( "<html><body>You are not allowed to access the internets!<br> </body></html>" );

        QByteArray response;
        response.append( "HTTP/1.0 403 Forbidden\r\n" );
        response.append( "Connection: close\r\n" );
        response.append( "Content-Type: text/html\r\n" );
        response.append( QString( "Content-Length: %0\r\n" ).arg( QString::number( body.length() ) ) );
        response.append( "\r\n" );
        response.append( body );

        client->write( response );
        client->close();

        return;
    }
    else if ( ClientAuthenticationRequired == rc )
    {
        static const QString body( "<html><body>This proxy requires a login.<br> </body></html>" );

        QByteArray response;
        response.append( "HTTP/1.0 407 Proxy Authentication Required\r\n" );
        response.append( QString( "Proxy-Authenticate: Basic realm=\"PROXY %0 REQUIRES VALID LOGIN\"\r\n" ).arg( client->localAddress().toString() ) );
        response.append( "Content-Type: text/html\r\n" );
        response.append( QString( "Content-Length: %0\r\n" ).arg( QString::number( body.length() ) ) );
        response.append( "\r\n" );
        response.append( body );

        client->write( response );
        return;
    }
    else if ( ClientUnauthorized == rc )
    {
        static const QString body( "<html><body>You are not authorized to use this proxy, too bad so sad... :(<br> </body></html>" );

        QByteArray response;
        response.append( "HTTP/1.0 401 Unauthorized\r\n" );
        response.append( "Connection: close\r\n" );
        response.append( "Content-Type: text/html\r\n" );
        response.append( QString( "Content-Length: %0\r\n" ).arg( QString::number( body.length() ) ) );
        response.append( "\r\n" );
        response.append( body );

        client->write( response );
        client->close();

        return;
    }

    // process
    Q_EMIT processRequest( client, header, body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::onClientDestroyed()
{
    QTcpSocket *client( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( client ) && ( clientBuffers_.contains( client ) ))
    {
        qDebug() << "client removed" << client;
        clientBuffers_.remove( client );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::onClientReadyRead()
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
void proxyServerWidget::onPushButtonClicked()
{
    // close
    if ( close_ == sender() )
        closeProxyServer();

    // open
    else if ( open_ == sender() )
        openProxyServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::onServerAcceptError( QAbstractSocket::SocketError error )
{
    qDebug() << "server accept error" << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::onServerNewConnection()
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
void proxyServerWidget::initialize()
{
    connect( &server_, SIGNAL(acceptError(QAbstractSocket::SocketError)), SLOT(onServerAcceptError(QAbstractSocket::SocketError)) );
    connect( &server_, SIGNAL(newConnection()), SLOT(onServerNewConnection()) );

    portLabel_ = new QLabel( tr( "Port:" ), this );

    port_ = new QLineEdit( this );
    port_->setText( "8080" );
    port_->setValidator( new QIntValidator( 1, 65535, this ) );

    allowedLabel_ = new QLabel( tr( "Allowed Clients:" ), this );

    allowed_ = new QLineEdit( this );
    allowed_->setText( "*" );

    open_ = new QPushButton( tr( "Open" ), this );
    connect( open_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    close_ = new QPushButton( tr( "Close" ), this );
    close_->setEnabled( false );
    connect( close_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    options_ = new QGroupBox( tr( "Server Options" ), this );
    options_->setEnabled( false );

    denyEverything_ = new QCheckBox( tr( "Nothing Allowed" ), options_ );

    denyHobart_ = new QCheckBox( tr( "Deny hobartcorp.com" ), options_ );

    denyGoogle_ = new QCheckBox( tr( "Deny google.com" ), options_ );

    denyYahoo_ = new QCheckBox( tr( "Deny yahoo.com" ), options_ );

    denyMSN_ = new QCheckBox( tr( "Deny msn.com" ), options_ );

    requireLogin_ = new QCheckBox( tr( "Require Login" ), options_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void proxyServerWidget::createLayout()
{
    QHBoxLayout *server( new QHBoxLayout() );
    server->addWidget( portLabel_ );
    server->addWidget( port_ );
    server->addWidget( allowedLabel_ );
    server->addWidget( allowed_ );
    server->addWidget( open_ );
    server->addWidget( close_ );
    server->addStretch();

    QHBoxLayout *options( new QHBoxLayout( options_ ) );
    options->addWidget( denyEverything_ );
    options->addWidget( denyHobart_ );
    options->addWidget( denyGoogle_ );
    options->addWidget( denyYahoo_ );
    options->addWidget( denyMSN_ );
    options->addWidget( requireLogin_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addLayout( server );
    form->addWidget( options_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
proxyServerWidget::clientAllowedResult proxyServerWidget::isClientAllowed( QTcpSocket *client, const QStringList &header ) const
{
    const QString clients( allowed_->text() );

    // check target host
    if ( header.size() < 1 )
        return ClientBadRequest;

    // check allowed clients
    if (( "*" != clients ) && ( !clients.contains( client->peerAddress().toString() ) ))
        return ClientUnauthorized;

    // check login/password
    if ( requireLogin_->isChecked() )
    {
        static const QString validation( "Proxy-Authorization: Basic " );

        QByteArray creds;

        // locate login information
        foreach ( const QString &h, header )
            if ( h.contains( validation ) )
            {
                creds = QByteArray::fromBase64( h.mid( validation.length() ).toLatin1() );
                break;
            }

        if ( creds.isEmpty() )
            return ClientAuthenticationRequired;

        qDebug() << "user authorization:" << creds;

        if ( "randilus:maximus" != creds )
            return ClientUnauthorized;
    }

    if ( denyEverything_->isChecked() )
        return ClientForbidden;
    if (( denyHobart_->isChecked() ) && ( header[0].toLower().contains( "hobartcorp" ) ))
        return ClientForbidden;
    if (( denyGoogle_->isChecked() ) && ( header[0].toLower().contains( "google" ) ))
        return ClientForbidden;
    if (( denyYahoo_->isChecked() ) && ( header[0].toLower().contains( "yahoo" ) ))
        return ClientForbidden;
    if (( denyMSN_->isChecked() ) && ( header[0].toLower().contains( "msn" ) ))
        return ClientForbidden;

    return ClientAllowed;
}
