/**
 * @file apiedgify.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiedgify.h"
#include "common.h"
#include "stringsjson.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QTextStream>
#include <QUrl>
#include <QVector>

#if defined(Q_OS_LINUX)
#elif defined(Q_OS_WIN32)
#endif

#if defined(Q_OS_LINUX)
static const QString LOG_FILE( "/var/log/EdgifyRequests.log" );

static const QString TEMP_FILE_CACHE_DIR( "/usr/share/edgify-apphook/cache" );
static const QString TEMP_FILE( TEMP_FILE_CACHE_DIR + "/download-XXXXXX.tmp" );
#elif defined(Q_OS_WIN32)
static const QString LOG_FILE( "EdgifyRequests.log" );

static const QString TEMP_FILE_CACHE_DIR( "cache" );
static const QString TEMP_FILE( TEMP_FILE_CACHE_DIR + "\\download-XXXXXX.tmp" );
#endif

static const QString TEMP_FILE_FILTER( "download-*.tmp" );

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyapi::edgifyapi( QObject *parent ) :
    _Mybase( parent ),
    savedRequests_( 20 ),
    blocking_( false )
{
    // connect signals
    connect( this, &_Myt::replyDownloadProgress, this, &_Myt::onReplyDownloadProgress );
    connect( this, &_Myt::replyReceived, this, &_Myt::onReplyReceived );

    // delete all stale downloads
    QDir d;

    if ( !d.exists( TEMP_FILE_CACHE_DIR ) )
    {
        if ( !d.mkdir( TEMP_FILE_CACHE_DIR ) )
            LOG_ERROR << "error creating cache dir " << qPrintable( TEMP_FILE_CACHE_DIR );
    }
    else if ( !d.cd( TEMP_FILE_CACHE_DIR ) )
        LOG_WARN << "could not cd to " << qPrintable( TEMP_FILE_CACHE_DIR );
    else
    {
        QStringList names;
        names.append( TEMP_FILE_FILTER );

        // retrieve list of files to remove
        const QStringList files( d.entryList( names, QDir::Files ) );

        foreach ( const QString& file, files )
        {
            LOG_DEBUG << "removing cache file " << qPrintable( file );

            if ( !QFile::remove( d.absoluteFilePath( file ) ) )
                LOG_WARN << "could not remove cache file " << qPrintable( file );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyapi::~edgifyapi()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::send( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts )
{
    LOG_DEBUG << "send " << qPrintable( url.toString() );
    processRequestControl( createDocumentRequestControl( url, request, timeout, maxAttempts ) );

    if ( isBlocking() )
        waitForPending();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::onReplyDownloadProgress( QNetworkReply *reply, qint64 bytesReceived, qint64 bytesTotal, unsigned int elapsed )
{
    Q_UNUSED( bytesReceived );
    Q_UNUSED( bytesTotal );
    Q_UNUSED( elapsed );

    // nothing to do
    if ( !bytesReceived )
        return;

    // read control block
    bool exists;

    const requestControl rc( readRequestControl( reply, exists ) );

    if ( !exists )
        LOG_WARN << "bad control block";
    else if ( rc.file.isEmpty() )
        LOG_TRACE << "not a file";
    else
    {
        QFile f( rc.location );

        // append downloaded data to file
        if ( !f.open( QFile::WriteOnly | QFile::Append ) )
            LOG_WARN << "error opening file " << qPrintable( rc.location );
        else
        {
            f.write( reply->readAll() );
            f.close();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::onReplyReceived( QNetworkReply *reply, int replySize, bool valid, int statusCode, const QJsonDocument& request, const QJsonDocument& response, unsigned int elapsed )
{
    Q_UNUSED( request );
    Q_UNUSED( elapsed );

    // read control block
    bool exists;

    requestControl rc( readRequestControl( reply, exists ) );

    // check valid control block
    if ( !exists )
    {
        LOG_WARN << "bad control block";
        return;;
    }

    // increment attempts
    ++rc.attempts;

    LOG_INFO << "processing response...";

    bool done( false );
    bool retStatus( false );

    // validate response structure
    if ( response.isEmpty() )
    {
        LOG_WARN << "response mismatch or invalid!";
        valid = false;
    }

    // check response for basic information
    else if ( !response.isObject() )
    {
        LOG_WARN << "not an object";
        valid = false;
    }

    // process status code
    if ( valid )
    {
        switch ( statusCode )
        {
        case 200:
            LOG_INFO << "SUCCESS - http status code " << statusCode;
            retStatus = true;
            break;

        default:
            LOG_WARN << "FAILURE - http status code " << statusCode;
            retStatus = false;
            break;
        }

        // update control block
        rc.stop = QDateTime::currentDateTime();

        rc.status = statusCode;
        rc.response = response;

        // control block done
        done = true;
    }
    else
    {
        LOG_WARN << "FAILURE - http status code " << statusCode;
        retStatus = false;

        // special case
        // item not on file but we need to use cache
        if (( rc.file.isEmpty() ) && ( exitRetries( statusCode ) ))
            done = true;

        // too many attempts
        else if ( rc.maxAttempts <= rc.attempts )
        {
            LOG_WARN << "request failed ";
            done = true;
        }

        // request failure but we are done
        if ( done )
        {
            // update control block
            rc.stop = QDateTime::currentDateTime();

            rc.status = statusCode;
        }
    }

    LOG_DEBUG << "request status " << rc.status << " " << done;

    if ( done )
    {
        const qint64 elapsedTotal( rc.start.msecsTo( rc.stop ) );

        LOG_TRACE << "saving query result...";

        // log result to file
        queryResult result;
        result.append( rc.start.toString("hh:mm:ss:zzz AP MM/dd/yyyy") );
        result.append( QString::number( statusCode ) );
        result.append( QString::number( replySize ) );
        result.append( QString::number( rc.attempts - 1 ) );
        result.append( QString::number( elapsedTotal ) + "ms" );
        result.append( QString::number( retStatus ) );

        logQueryToFile( result, LOG_FILE );

        // log result
        LOG_INFO << "request took " << elapsedTotal << "ms (" << rc.status << ")";

        // emit!
        if ( rc.file.isEmpty() )
            emit processDocument( rc.request, rc.status, rc.response );
        else
        {
            emit processFile( rc.request, rc.status, rc.location, rc.file );

            // remove any cache file
            if (( rc.location.length() ) && ( QFile::exists( rc.location ) ))
            {
                LOG_DEBUG << "removing cache file " << qPrintable( rc.location );

                if ( !QFile::remove( rc.location ) )
                    LOG_WARN << "could not remove cache file " << qPrintable( rc.location );
            }
        }
    }
    else
    {
        LOG_DEBUG << "attempting request again " << rc.attempts << " " << rc.maxAttempts;
        processRequestControl( rc );
    }

    // destroy control block
    destroyRequestControl( reply );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *edgifyapi::processRequestControl( const requestControl& rc )
{
    QNetworkReply *reply( nullptr );

    if ( rc.request.isEmpty() )
        reply = get( rc.url, false, rc.timeout );
    else
        reply = post( rc.url, rc.request, false, rc.timeout );

    if ( reply )
        writeRequestControl( reply, rc );
    else
    {
        LOG_WARN << "process request failed!";
    }

    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyapi::requestControl edgifyapi::createDocumentRequestControl( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts )
{
    // create request control
    requestControl rc;
    rc.start = QDateTime::currentDateTime();
    rc.timeout = timeout;
    rc.maxAttempts = maxAttempts;
    rc.attempts = 0;
    rc.url = url;
    rc.request = request;

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyapi::requestControl edgifyapi::createFileRequestControl( const QUrl& url, const QString& filename, const QJsonDocument& request )
{
    QTemporaryFile f( TEMP_FILE );
    QString location;

    // generate a temporary file
    if ( !f.open() )
        LOG_WARN << "error creating temporary file!";
    else
    {
        location = f.fileName();
        f.close();

        LOG_INFO << "temp file for " << qPrintable( filename ) << " created " << qPrintable( location );
    }

    // create request control
    requestControl rc;
    rc.start = QDateTime::currentDateTime();
    rc.timeout = 0;
    rc.maxAttempts = 1;
    rc.attempts = 0;
    rc.url = url;
    rc.request = request;
    rc.file = filename;
    rc.location = location;

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyapi::requestControl edgifyapi::readRequestControl( QNetworkReply *reply, bool& exists ) const
{
    requestControl rc;

    QMutexLocker guard( &m_ );

    if ( !(exists = pending_.contains( reply )) )
        LOG_WARN << "uuid not found in pending!";
    else
    {
        rc = pending_[reply];
    }

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::writeRequestControl( QNetworkReply *reply, const requestControl& rc )
{
    QMutexLocker guard( &m_ );
    pending_[reply] = rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::destroyRequestControl( QNetworkReply *reply )
{
    QMutexLocker guard( &m_ );

    if ( !pending_.contains( reply ) )
        LOG_WARN << "uuid not found in pending!";
    else
    {
        pending_.remove( reply );
    }

    LOG_DEBUG << "requests pending " << pending_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int edgifyapi::numPendingRequests() const
{
    QMutexLocker guard( &m_ );
    return pending_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::logQueryToFile( const queryResult& query, const QString& filename ) const
{
    QFile f( filename );

    // create file if not existing
    if (( !QFile::exists( filename ) ) && ( f.open( QIODevice::WriteOnly | QIODevice::Text ) ))
    {
        f.write( "TIMESTAMP,RETURNCODE,RETURNBYTES,RETRIES,TIMESPENT,STATUS\n" );
        f.close();
    }

    if ( !f.open( QIODevice::ReadWrite | QIODevice::Text ) )
        LOG_WARN << "Log file DID NOT EXIST";
    else
    {
        QString querylines( "" );
        QTextStream ts(&f);
        QVector<QString> requests;

        while( !ts.atEnd() )
        {
            QString line = ts.readLine();
            requests.append( line );                        // I now have a vector of requests.
        }

        // trim down bytes when too large
        QString returnBytes( query[RETURNBYTES] );

        if ( 8 <= returnBytes.length() )
            returnBytes = QString::number( returnBytes.toInt() / (1024*1024) ) + "M";
        else if ( 5 <= returnBytes.length() )
            returnBytes = QString::number( returnBytes.toInt() / 1024 ) + "K";

        // trim down time when too large
        QString timeSpent( query[TIMESPENT] );

        if ( 6 <= timeSpent.length() )
        {
            timeSpent.replace( "ms", "" );

            timeSpent = QString::number( timeSpent.toInt() / 1000 );
            timeSpent += "s";
        }

        // build new request
        QString newrequest( query[TIMESTAMP] + "," );
        newrequest.append( query[RETURNCODE] + "," );
        newrequest.append( returnBytes + "," );
        newrequest.append( query[RETRIES] + "," );
        newrequest.append( timeSpent + ",");
        newrequest.append( query[STATUS]);

        if( savedRequests_ == (unsigned int) requests.count() )            // the +1 above when read accounts for the header
            requests.remove(1);                             // if there are max items (max+header) remove element one[1]

        requests.append( newrequest );                      //  always append new one to the end

        QString str;
        foreach( str, requests )
            querylines.append(str + "\n");                  // build new file data

        f.resize(0);                                        // clear the file
        ts << querylines;                                   // write the file
        f.flush();                                          // flush the file
        f.close();                                          // close the file
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyapi::waitForPending() const
{
    static const int PROCESS_EVENTS_TIME( 32 );
    static const int SLEEP_TIME( 16 );

    LOG_DEBUG << "waiting for pending requests to complete...";

    do
    {
        // process pending events
        QCoreApplication::processEvents( QEventLoop::AllEvents, PROCESS_EVENTS_TIME );

        // do not consume 100% cpu resources
        QThread::msleep( SLEEP_TIME );

    } while ( numPendingRequests() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyapi::exitRetries( int code )
{
    bool exitRetryLoop( true );

    switch ( code )
    {
    case 0:                                 // happens on contact and network issues, no network, so retry
    case 404:                               // Service not available so try again
    case 408:                               // Client timed out so try again
    case 504:                               // Gateway timeout so try again";
    case 525:                               // SSL connection failed so see if we can do it again
        exitRetryLoop = false;
    break;

    case 200:                                // we got a plu we got data so exit
    case 204:                                // PLU does not exist so exit even with no data
    case 400:                                // PLU does not exist so exit even with no data
    case 417:                                // Invalid request so exit
    case 500:                                // Server error so exit out
        break;

    default:
        exitRetryLoop = false;               // we dont know this code so try it again
        break;
    }

    return exitRetryLoop;
}
