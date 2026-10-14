/**
 * @file abstractapi.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "abstractapi.h"
#include "common.h"
#include "networkaccess.h"

#include <QEventLoop>
#include <QFile>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#define DEBUG_JSON
//#define DEBUG_JSON_SAVE

//#define DEBUG_HEADER

static const QString APPLICATION_JSON( "application/json" );

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractapi::abstractapi( QObject *parent ) :
    _Mybase( parent ),
    networkAccess_( nullptr )
{
    // create network access
    networkAccess_ = new networkAccess( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractapi::~abstractapi()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void abstractapi::onDownloadProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    LOG_DEBUG << "download progress " << bytesReceived << " of " << bytesTotal;

    // get reply
    QNetworkReply *reply( qobject_cast<QNetworkReply*>( sender() ) );

    // nothing to do
    if ( !reply )
    {
        LOG_WARN << "bad reply";
        return;
    }

    // read control block
    const requestControl rc( readRequestControl( reply ) );

    // we are getting data back so reset any active timeout
    if ( rc.timeout )
        rc.timeout->start();

    // emit!
    emit replyDownloadProgress( reply, bytesReceived, bytesTotal, rc.start.msecsTo( QDateTime::currentDateTime() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void abstractapi::onFinished()
{
    bool valid;

    LOG_DEBUG << "request finished";

    // get reply
    QNetworkReply *reply( qobject_cast<QNetworkReply*>( sender() ) );

    // nothing to do
    if ( !reply )
    {
        LOG_WARN << "bad reply";
        return;
    }

    // process reply    
    parseNetworkReply( reply, valid );

    reply->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void abstractapi::createRequestControl( QNetworkReply *reply, const QJsonDocument& doc, unsigned int timeout )
{
    requestControl rc;
    rc.start = QDateTime::currentDateTime();
    rc.doc = doc;

    if ( !timeout )
        rc.timeout = nullptr;
    else
    {
        QTimer *t( new QTimer( this ) );
        t->setSingleShot( true );
        t->setInterval( timeout );
        t->start();

        connect( t, &QTimer::timeout, reply, &QNetworkReply::abort );

        LOG_DEBUG << "timeout of " << timeout << " ms";
        rc.timeout = t;
    }

    QMutexLocker guard( &m_ );
    pending_[reply] = rc;

    emit requestsPendingChanged( pending_.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractapi::requestControl abstractapi::readRequestControl( QNetworkReply *reply ) const
{
    requestControl rc;

    QMutexLocker guard( &m_ );

    if ( !pending_.contains( reply ) )
        LOG_WARN << "reply not found in pending!";
    else
    {
        rc = pending_[reply];
    }

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractapi::requestControl abstractapi::destroyRequestControl( QNetworkReply *reply )
{
    requestControl rc;

    QMutexLocker guard( &m_ );

    if ( !pending_.contains( reply ) )
        LOG_WARN << "reply not found in pending!";
    else
    {
        rc = pending_[reply];
        rc.stop = QDateTime::currentDateTime();

        if ( rc.timeout )
            rc.timeout->deleteLater();

        pending_.remove( reply );
        emit requestsPendingChanged( pending_.size() );
    }

    LOG_DEBUG << "requests pending " << pending_.size();
    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *abstractapi::handleRequest( method m, const QUrl &url, bool blocking, unsigned int timeout, const QJsonDocument &doc )
{
    // create request
    QNetworkRequest request( url );

    const QString urlString( url.toString() );
    LOG_INFO << "request type " << m << " url " << qPrintable( urlString );

    if ( !doc.isNull() )
    {
        request.setHeader( QNetworkRequest::ContentTypeHeader, APPLICATION_JSON );      // application/json

#ifdef DEBUG_JSON
        saveDocument( doc, "request.json" );
#endif
    }

    // set headers
    for ( headersMap::const_iterator i( headers_.begin() ); i != headers_.end(); ++i )
    {
        LOG_TRACE << "header info " << qPrintable( i.key() ) << " " << qPrintable( i.value() );
        request.setRawHeader( i.key(), i.value() );
    }

    // post!
    QNetworkReply *reply( nullptr );

    if ( DELETE_RESOURCE == m )
        reply = networkAccess_->deleteResource( request );
    else if ( GET == m )
        reply = networkAccess_->get( request );
    else if ( POST == m )
        reply = networkAccess_->post( request, doc.toJson() );
    else if ( PUT == m )
        reply = networkAccess_->put( request, doc.toJson() );

    if ( !reply )
        LOG_WARN << "bad reply";
    else
    {
        if ( !timeout )
        {
            LOG_DEBUG << "using default timeout";
            timeout = DEFAULT_TIMEOUT;
        }

        // create request
        createRequestControl( reply, doc, timeout );

        // wait for response if blocking
        if ( !blocking )
        {
            LOG_DEBUG << "non-blocking request";

            connect( reply, &QNetworkReply::downloadProgress, this, &_Myt::onDownloadProgress );
            connect( reply, &QNetworkReply::finished, this, &_Myt::onFinished );
        }
        else
        {
            bool valid;

            LOG_DEBUG << "wait for response " << timeout << " ms timeout...";

            QEventLoop eventLoop;
            connect( reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit );

            // wait...

            LOG_TRACE << "exec...";
            eventLoop.exec();

            LOG_TRACE << "exec... complete";

            parseNetworkReply( reply, valid );
        }
    }

    LOG_TRACE << "done";
    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void abstractapi::parseNetworkReply( QNetworkReply *reply, bool &valid )
{
    const requestControl rc( destroyRequestControl( reply ) );

    const unsigned int elapsed( rc.start.msecsTo( rc.stop ) );

    int statusCode( 0 );

    // process reply
    if ( QNetworkReply::NoError != reply->error() )
    {
        LOG_WARN << "network reply error " << reply->error() << " " << qPrintable( reply->errorString() );
        valid = false;
    }
    else
    {
        // check status code
        const QVariant statusCodeAttr( reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ) );

        statusCode = statusCodeAttr.toInt();
        valid = true;
    }

    // check content
    const QString contentType( reply->header( QNetworkRequest::ContentTypeHeader ).toString() );

    LOG_DEBUG << "content type " << qPrintable( contentType );

#if defined( DEBUG_HEADER )
    foreach ( const QByteArray& h, reply->rawHeaderList() )
        LOG_DEBUG << "header " << qPrintable( h ) << ": " << qPrintable( reply->rawHeader( h ) );
#endif

    const int replySize( reply->size() );

    QJsonDocument doc;

    if ( contentType.contains( APPLICATION_JSON ) )
    {
        QJsonParseError err;

        // parse!
        doc = QJsonDocument::fromJson( reply->readAll(), &err );

        if ( QJsonParseError::NoError != err.error )
        {
            LOG_WARN << "error parsing network request JSON document " << err.error << " " << qPrintable( err.errorString() );
            valid = false;
        }

#ifdef DEBUG_JSON
        saveDocument( doc, "response.json" );
#endif
    }

    // emit!

    LOG_TRACE << "reply received...";
    emit replyReceived( reply, replySize, valid, statusCode, rc.doc, doc, elapsed );

    LOG_TRACE << "reply received... done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void abstractapi::saveDocument( const QJsonDocument& doc, const QString& filename )
{
#if !defined( DEBUG_JSON )
    Q_UNUSED( doc );
    Q_UNUSED( filename );
#elif !defined( DEBUG_JSON_SAVE )
    Q_UNUSED( filename );
#endif

#ifdef DEBUG_JSON
    const QByteArray a( doc.toJson( QJsonDocument::Indented ) );

#if defined(Q_OS_LINUX)
    LOG_TRACE << a.constData();
#elif defined(Q_OS_WIN32)
    LOG_TRACE << a;
#endif

#ifdef DEBUG_JSON_SAVE
    QFile f( filename );

    if ( f.open( QFile::WriteOnly ) )
    {
        f.write( a );
        f.close();
    }
#endif
#endif
}
