/**
 * @file smtpserverwidget.cpp
 * SMTP server widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "smtpserverwidget.h"

#include <QtDebug>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>

///////////////////////////////////////////////////////////////////////////////////////////////////
smtpServerWidget::smtpServerWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
smtpServerWidget::~smtpServerWidget()
{
    closeMailServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::closeMailServer()
{
    if ( server_.isListening() )
        server_.close();

    close_->setEnabled( false );
    open_->setEnabled( true );
    port_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::openMailServer()
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::processClientBuffer( QTcpSocket *client, QByteArray &buffer )
{
    static const QByteArray crlf( "\r\n" );

    // retrieve lines
    int offset( 0 );
    int processed( 0 );

    while ( -1 != (offset = buffer.indexOf( crlf, processed )) )
    {
        const QString s( buffer.mid( processed, offset - processed ) );

        qDebug() << s;

        processed = offset + crlf.length();

        // process
        Q_EMIT processRequest( client, s );
    }

    // pull entire message off buffer
    buffer = buffer.mid( processed );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::onClientDestroyed()
{
    QTcpSocket *client( reinterpret_cast<QTcpSocket*>( sender() ) );

    if (( client ) && ( clientBuffers_.contains( client ) ))
    {
        qDebug() << "client removed" << client;
        clientBuffers_.remove( client );
    }

    Q_EMIT processDisconnect( client );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::onClientReadyRead()
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
void smtpServerWidget::onPushButtonClicked()
{
    // close
    if ( close_ == sender() )
        closeMailServer();

    // open
    else if ( open_ == sender() )
        openMailServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::onServerAcceptError( QAbstractSocket::SocketError error )
{
    qDebug() << "server accept error" << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::onServerNewConnection()
{
    // accept all clients
    while ( server_.hasPendingConnections() )
    {
        QTcpSocket *client( server_.nextPendingConnection() );

        connect( client, SIGNAL(destroyed()), SLOT(onClientDestroyed()) );
        connect( client, SIGNAL(disconnected()), client, SLOT(deleteLater()) );
        connect( client, SIGNAL(readyRead()), SLOT(onClientReadyRead()) );

        qDebug() << "client added" << client;

        // process
        Q_EMIT processNewConnection( client );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::initialize()
{
    connect( &server_, SIGNAL(acceptError(QAbstractSocket::SocketError)), SLOT(onServerAcceptError(QAbstractSocket::SocketError)) );
    connect( &server_, SIGNAL(newConnection()), SLOT(onServerNewConnection()) );

    portLabel_ = new QLabel( tr( "Port:" ), this );

    port_ = new QLineEdit( this );
    port_->setText( "25" );
    port_->setValidator( new QIntValidator( 1, 65535, this ) );

    open_ = new QPushButton( tr( "Open" ), this );
    connect( open_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    close_ = new QPushButton( tr( "Close" ), this );
    close_->setEnabled( false );
    connect( close_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void smtpServerWidget::createLayout()
{
    QHBoxLayout *server( new QHBoxLayout() );
    server->addWidget( portLabel_ );
    server->addWidget( port_ );
    server->addWidget( open_ );
    server->addWidget( close_ );
    server->addStretch();

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addLayout( server );
}

