/**
 * @file webviewernetwork.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "webviewernetwork.h"
#include "webviewernetworkproxyfactory.h"

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
    LOG_CLASS_INFO( "requestTimeout", "onReplyDownloadProgress" );

    LOG_TRACE( "reply 0x%08x progress %lld / %lld", reply_, (long long) bytesReceived, (long long) bytesTotal );

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
    LOG_CLASS_INFO( "requestTimeout", "onReplyUploadProgress" );

    LOG_TRACE( "reply 0x%08x progress %lld / %lld", reply_, (long long) bytesSent, (long long) bytesTotal );

    if ( bytesSent <= bytesSent_ )
        return;

    // reset timer
    timer_->start( REPLY_TIMEOUT );

    bytesSent_ = bytesSent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void requestTimeout::onTimeout()
{
    LOG_CLASS_INFO( "requestTimeout", "onTimeout" );

    LOG_WARN( "reply 0x%08x has timed out, aborting...", reply_ );
    reply_->abort();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewerNetworkAccessManager::webViewerNetworkAccessManager( QObject *parent ) :
    _Mybase( parent )
{
    connect( this, SIGNAL( authenticationRequired(QNetworkReply*,QAuthenticator*) ), SLOT( onAuthenticationRequired(QNetworkReply*,QAuthenticator*) ) );
    connect( this, SIGNAL( proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*) ), SLOT( onProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*) ) );
    connect( this, SIGNAL( sslErrors(QNetworkReply*,QList<QSslError>) ), SLOT( onSslErrors(QNetworkReply*,QList<QSslError>) ) );

    // setup proxy
    webViewerNetworkProxyFactory *factory =
         new webViewerNetworkProxyFactory( filesystem::absoluteFilePath( "proxySettings", folderType::RootFolder ), this );

    setProxyFactory( factory );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewerNetworkAccessManager::~webViewerNetworkAccessManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *webViewerNetworkAccessManager::createRequest( Operation op, const QNetworkRequest& request, QIODevice *outgoingData )
{
    LOG_CLASS_INFO( "webViewerNetworkAccessManager", "createRequest" );

    QString url( request.url().toString() );
    LOG_TRACE( "requesting file '%s'", qPrintable( url ) );

    QNetworkReply *reply( NULL );

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

            LOG_DEBUG( "redirecting '%s' to filesystem file '%s'", qPrintable( filename ), qPrintable( remote ) );
            redirect.setUrl( QUrl( prefix + filesystem::absoluteFilePath( remote, folderType::Html ) + suffix ) );

            reply = _Mybase::createRequest( op, redirect, outgoingData );
        }
    }

    // invoke base
    if ( !reply )
        reply = _Mybase::createRequest( op, request, outgoingData );

    // ---- //

    // hook into reply
    if ( reply )
    {
        // this wrapper object will abort transfer when reply stops sending data
        requestTimeout *wrapper( new requestTimeout( reply, this ) );

        // Note:
        // This variable is used by Qt even though it is not referenced in this method. This macro will
        // get rid of the compiler warning.
        Q_UNUSED( wrapper );

        LOG_TRACE( "reply 0x%08x created", reply );
        connect( reply, SIGNAL( finished() ), SLOT( onReplyFinished() ) );
    }

    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewerNetworkAccessManager::onAuthenticationRequired( QNetworkReply* /*reply*/, QAuthenticator* /*auth*/ )
{
    LOG_CLASS_INFO( "webViewerNetworkAccessManager", "onAuthenticationRequired" );

    LOG_WARN( "authentication required!", NULL );
}

/////////////////////////////////////////////////////////////////////////////////////////////////// 
void webViewerNetworkAccessManager::onProxyAuthenticationRequired( const QNetworkProxy& /*proxy*/, QAuthenticator* /*auth*/ )
{
    LOG_CLASS_INFO( "webViewerNetworkAccessManager", "onProxyAuthenticationRequired" );

    LOG_WARN( "proxy authentication required!", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewerNetworkAccessManager::onReplyFinished()
{
    LOG_CLASS_INFO( "webViewerNetworkAccessManager", "onReplyFinished" );

    QNetworkReply *reply( dynamic_cast<QNetworkReply*>( sender() ) );

    if ( !reply )
    {
        LOG_WARN( "bad reply!", NULL );
    }
    else if ( QNetworkReply::NoError == reply->error() )
    {
        LOG_TRACE( "reply 0x%08x finished", reply );
    }
    else
    {
        LOG_WARN( "reply 0x%08x error %d", reply, reply->error() );
        LOG_WARN( "reply 0x%08x error string %s", reply, reply->errorString().toStdString().c_str());
        LOG_WARN( "reply URL: %s", reply->url().toString().toStdString().c_str());
        
        if (reply->error() == QNetworkReply::UnknownNetworkError)
        {
          LOG_WARN("got error 99 UnknownNetworkError, attempting to force online status", NULL);
          setNetworkAccessible(QNetworkAccessManager::Accessible);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewerNetworkAccessManager::onSslErrors( QNetworkReply *reply, const QList<QSslError>& errors )
{
    LOG_CLASS_INFO( "webViewerNetworkAccessManager", "onSslErrors" );

    LOG_INFO( "have %d ssl errors...", errors.size() );
    bool ignoreErrors = true;

    // check errors
    foreach ( QSslError e, errors )
    {
        LOG_WARN( "error %d: '%s'", e.error(), qPrintable( e.errorString() ) );

        // TODO
        // check error and set ignoreErrors to false if bad enough
    }

    // indicate to ignore errors in the reply
    if ( ignoreErrors )
    {
        LOG_INFO( "ignoring ssl errors", NULL );
        reply->ignoreSslErrors();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


