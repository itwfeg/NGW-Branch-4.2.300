/**
 * @file webviewnetwork.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "webviewnetwork.h"
#include "webviewnetworkproxyfactory.h"

#include <QFile>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QTextStream>
#include <QTimer>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
requestTimeout::requestTimeout( QNetworkReply *reply, QObject *parent ) :
    QObject( parent ),
    reply_( reply ),
    timer_( new QTimer( this ) ),
    bytesReceived_( 0 ),
    bytesSent_( 0 )
{
    // hook into timer
    connect( timer_, SIGNAL(timeout()), SLOT(onTimeout()) );

    // hook into reply
    connect( reply_, SIGNAL(destroyed()), SLOT(onReplyDestroyed()) );
    connect( reply_, SIGNAL(downloadProgress(qint64,qint64)), SLOT(onReplyDownloadProgress(qint64,qint64)) );
    connect( reply_, SIGNAL(finished()), SLOT(onReplyFinished()) );
    connect( reply_, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onReplyUploadProgress(qint64,qint64)) );

    // go!
    timer_->setSingleShot( true );
    timer_->start( REPLY_TIMEOUT );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
requestTimeout::~requestTimeout()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onReplyDestroyed()
{
    deleteLater();
    reply_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onReplyDownloadProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    qDebug( "reply 0x%08x progress %lld / %lld", (qintptr) reply_, (long long) bytesReceived, (long long) bytesTotal );

    if ( bytesReceived <= bytesReceived_ )
        return;

    // reset timer
    timer_->start( REPLY_TIMEOUT );

    bytesReceived_ = bytesReceived;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onReplyFinished()
{
    timer_->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onReplyUploadProgress( qint64 bytesSent, qint64 bytesTotal )
{
    qDebug( "reply 0x%08x progress %lld / %lld", (qintptr) reply_, (long long) bytesSent, (long long) bytesTotal );

    if ( bytesSent <= bytesSent_ )
        return;

    // reset timer
    timer_->start( REPLY_TIMEOUT );

    bytesSent_ = bytesSent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onTimeout()
{
    qWarning( "reply 0x%08x has timed out, aborting...", (qintptr) reply_ );
    reply_->abort();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewNetworkAccessManager::webViewNetworkAccessManager( QObject *parent ) :
    _Mybase( parent )
{
    static const QString proxy( "proxySettings" );

    connect( this, SIGNAL( authenticationRequired(QNetworkReply*,QAuthenticator*) ), SLOT( onAuthenticationRequired(QNetworkReply*,QAuthenticator*) ) );
    connect( this, SIGNAL( proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*) ), SLOT( onProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*) ) );
    connect( this, SIGNAL( sslErrors(QNetworkReply*,QList<QSslError>) ), SLOT( onSslErrors(QNetworkReply*,QList<QSslError>) ) );

    // setup proxy
    if ( QFile::exists( proxy ) )
    {
        setProxyFactory( new webViewNetworkProxyFactory( proxy, this ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewNetworkAccessManager::~webViewNetworkAccessManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *webViewNetworkAccessManager::createRequest( Operation op, const QNetworkRequest& request, QIODevice *outgoingData )
{
    QString url( request.url().toString() );
    qDebug( "requesting file '%s'", qPrintable( url ) );

    QNetworkReply *reply( NULL );
/*
    // Note:
    // The intent here is when we access local files we need to pull them from the remote system
    // to local before QWebView can display them. We invoke filesystem to perform this action.

    // check for get of local file
    if (( GetOperation == op ) && ( "file://" == url.left( 7 ) ))
    {
        QString prefix( url.left( 7 ) );
        QString suffix;

        QString filename( url.right( url.length() - prefix.length() ) );
        int index = -1;

        // chop off prefix if exists
        if (( 3 <= filename.length() ) && ( '/' == filename[0] ))
            if (( '/' == filename[1] ) || ( ':' == filename[2] ))
            {
                prefix += "/";
                filename = filename.right( filename.length() - 1 );
            }

        // chop off suffix if exists
        index = qMax( index, filename.indexOf( "?" ) );
        index = qMax( index, filename.indexOf( "#" ) );

        if ( 0 < index )
        {
            suffix = filename.mid( index );
            filename = filename.left( filename.length() - suffix.length() );
        }

        // get remote filename
        QString remote( filesystem::fileName( filename, folderType::Html ) );

        // set new url to cached filename
        if ( filename != remote )
        {
            QNetworkRequest redirect( request );

            qDebug( "redirecting '%s' to filesystem file '%s'", qPrintable( filename ), qPrintable( remote ) );
            redirect.setUrl( QUrl( prefix + filesystem::absoluteFilePath( remote, folderType::Html ) + suffix ) );

            reply = _Mybase::createRequest( op, redirect, outgoingData );
        }
    }
*/
    // invoke base
    if ( !reply )
        reply = _Mybase::createRequest( op, request, outgoingData );

    // ---- //

    // hook into reply
    if ( reply )
    {
        // this wrapper object will abort transfer when reply stops sending data
        requestTimeout *wrapper( new requestTimeout( reply, this ) );

        qDebug( "reply 0x%08x created", (qintptr) reply );
        connect( reply, SIGNAL( finished() ), SLOT( onReplyFinished() ) );
    }

    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewNetworkAccessManager::onAuthenticationRequired( QNetworkReply* /*reply*/, QAuthenticator* /*auth*/ )
{
    qWarning( "authentication required!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewNetworkAccessManager::onProxyAuthenticationRequired( const QNetworkProxy& /*proxy*/, QAuthenticator* /*auth*/ )
{
    qWarning( "proxy authentication required!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewNetworkAccessManager::onReplyFinished()
{
    QNetworkReply *reply( dynamic_cast<QNetworkReply*>( sender() ) );

    if ( !reply )
        qWarning( "bad reply!" );
    else if ( QNetworkReply::NoError == reply->error() )
        qDebug( "reply 0x%08x finished", (qintptr) reply );
    else
    {
        qWarning( "reply 0x%08x error %d", (qintptr) reply, reply->error() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewNetworkAccessManager::onSslErrors( QNetworkReply *reply, const QList<QSslError>& errors )
{
    qDebug( "have %d ssl errors...", errors.size() );
    bool ignoreErrors = true;

    // check errors
    foreach ( QSslError e, errors )
    {
        qWarning( "error %d: '%s'", e.error(), qPrintable( e.errorString() ) );

        // TODO
        // check error and set ignoreErrors to false if bad enough
    }

    // indicate to ignore errors in the reply
    if ( ignoreErrors )
    {
        qDebug( "ignoring ssl errors" );
        reply->ignoreSslErrors();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


