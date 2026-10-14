/**
 * @file apihte.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apihte.h"
#include "common.h"
#include "scaleconfig.h"
#include "stringsjson.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QTextStream>
#include <QUrl>
#include <QVector>


///////////////////////////////////////////////////////////////////////////////////////////////////
hteapi::hteapi( QObject *parent ) :
    _Mybase( parent ),
    savedRequests_( 20 )
{
    // connect signals
    connect( this, SIGNAL(replyDownloadProgress(QNetworkReply*,qint64,qint64,unsigned int)), SLOT(onReplyDownloadProgress(QNetworkReply*,qint64,qint64,unsigned int)) );
    connect( this, SIGNAL(replyReceived(QNetworkReply*,bool,int,const QJsonDocument&,const QJsonDocument&,unsigned int)), SLOT(onReplyReceived(QNetworkReply*,bool,int,const QJsonDocument&,const QJsonDocument&,unsigned int)) );

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
hteapi::~hteapi()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::downloadFile( const QUrl& url, const QString& uuid, const QString& filename, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts )
{
    LOG_DEBUG << "hteapi::downloadFile() - uuid: " << qPrintable( uuid ) << "  url: " << qPrintable( url.toString() ) << "  filename: " << qPrintable( filename );
    LOG_DEBUG << "hteapi::downloadFile() - request: " << qPrintable( request.toJson() );

    QJsonObject obj( request.object() );

    QJsonDocument dontNeedAJsonDocumentForThis;
    processRequestControl( createFileRequestControl( url, uuid, filename, dontNeedAJsonDocumentForThis, timeout, maxAttempts ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::send( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts, const QUrl& backupUrl )
{
    if ( !request.isObject() )
        LOG_WARN << "not an object";
    else
    {
        QJsonObject obj( request.object() );

        if (( !obj.contains( JSON_REQUEST_UUID ) ) || ( !obj[JSON_REQUEST_UUID].isString() ))
        {
            LOG_WARN << "request missing required information";
        }
        else
        {
            const QString uuid( obj[JSON_REQUEST_UUID].toString() );

            LOG_DEBUG << "hteapi::send() - uuid: " << qPrintable( uuid ) << "  url: " << qPrintable( url.toString() );
            LOG_DEBUG << "hteapi::send() - request: " << qPrintable( request.toJson() );
            processRequestControl( createDocumentRequestControl( url, uuid, request, timeout, maxAttempts, backupUrl ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::exportLastUsage( const QString &api ) const
{
    QFile f( LAST_USAGE_FILE );

    if ( !f.open( QIODevice::WriteOnly ) )
    {
        LOG_WARN << "could not open last usage file " << qPrintable( LAST_USAGE_FILE );
        return;
    }

    const QDateTime now( QDateTime::currentDateTime() );

    QDataStream s( &f );
    s << now;
    s << api;

    f.close();

    const QString str( now.toString() );
    LOG_DEBUG << "wrote last usage as " << qPrintable( str ) << " " << qPrintable( api );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteapi::importLastUsage( QDateTime& stamp, QString& api )
{
    QDateTime importStamp;
    QString importAPI;

    if ( !QFile::exists( LAST_USAGE_FILE ) )
    {
        LOG_WARN << "last usage file does not exist";
        return false;
    }

    QFile f( LAST_USAGE_FILE );

    if ( !f.open( QIODevice::ReadOnly ) )
    {
        LOG_WARN << "could not open last usage file " << qPrintable( LAST_USAGE_FILE );
        return false;
    }

    QDataStream s( &f );
    s >> importStamp;
    s >> importAPI;

    f.close();

    if (( importStamp.isNull() ) || ( !importStamp.isValid() ))
    {
        LOG_WARN << "last usage stamp is invalid";
        return false;
    }
    else if (( importAPI.isNull() ) || ( importAPI.isEmpty() ))
    {
        LOG_WARN << "last usage api is invalid";
        return false;
    }

    const QString str( importStamp.toString() );
    LOG_DEBUG << "read last usage as " << qPrintable( str ) << " " << qPrintable( importAPI );

    stamp = importStamp;
    api = importAPI;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::onReplyDownloadProgress( QNetworkReply *reply, qint64 bytesReceived, qint64 bytesTotal, unsigned int elapsed )
{
    Q_UNUSED( bytesReceived );
    Q_UNUSED( bytesTotal );
    Q_UNUSED( elapsed );

    // nothing to do
    if ( !bytesReceived )
        return;

    // read control block
    const requestControl rc( readRequestControl( reply ) );

    if ( rc.uuid.isEmpty() )
        LOG_WARN << "invalid control block!";
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
void hteapi::onReplyReceived( QNetworkReply *reply, bool valid, int statusCode, const QJsonDocument& request, const QJsonDocument& response, unsigned int elapsed )
{
    Q_UNUSED( request );
    Q_UNUSED( elapsed );

    // read control block
    requestControl rc( readRequestControl( reply ) );

    if ( rc.uuid.isEmpty() )
    {
        LOG_WARN << "invalid control block!";
        return;
    }

    // increment attempts
    ++rc.attempts;

    LOG_INFO << "processing response for request " << qPrintable( rc.uuid );

    RequestStatus retStatus( SUCCESS );

    QString plu( "N/A" );

    bool done( false );

    LOG_DEBUG << "hteapi::onReplyReceived() - request: " << qPrintable( request.toJson() );
    LOG_DEBUG << "hteapi::onReplyReceived() - response: " << qPrintable( response.toJson() );
    LOG_DEBUG << "hteapi::onReplyReceived() - statusCode: " <<  statusCode;

    // validate response structure
    if (( !response.isEmpty() ) && ( !scaleConfig::instance()->validateRequestInfo( response, rc.uuid ) ))
    {
        LOG_WARN << "response mismatch or invalid!";
        valid = false;
    }

    // process status code
    if ( valid )
    {
        switch ( statusCode )
        {
        case 200:
            // scale on file, walmart on file
            LOG_INFO << "SUCCESS returned http status code " << statusCode;
            retStatus = SUCCESS;
            break;

        case 0:
        case 400:
            LOG_INFO << "NO DATA returned http status code " << statusCode;
            retStatus = NODATA;
            break;

        default:
            LOG_INFO << "CACHE DATA, use it if we have it, returned http status code " << statusCode;
            retStatus = CACHEDATA;
            break;
        }

        // update control block
        rc.stop = QDateTime::currentDateTime();

        rc.status = retStatus;
        rc.response = response;

        // record last success
        const QUrl url( reply->url() );

        exportLastUsage( url.fileName() );

        // control block done
        done = true;
    }
    else
    {
        switch ( statusCode )
        {
        case 0:     // if we are offline
        case 404:
        case 417:
            LOG_INFO << "CACHE DATA returned http status code " << statusCode;
            retStatus = CACHEDATA;
            break;

        default:                            //  mostly wanted to catch unknown codes so I split these out of known/unknown
            LOG_INFO << "default NODATA UNKNOWN CODE  " << statusCode;
            retStatus = NODATA;
            break;
        }

        // special case
        // item not on file but we need to use cache
        if (( rc.file.isEmpty() ) && ( exitRetries( statusCode ) ))
            done = true;

        // too many attempts
        else if ( rc.maxAttempts <= rc.attempts )
        {
            LOG_WARN << "request " << qPrintable( rc.uuid ) << " failed ";

            if ( !rc.usingBackupUrl && !rc.backupUrl.url().isEmpty() )
            {
                rc.attempts = 0;
                rc.usingBackupUrl = true;
            }
            else
                done = true;
        }

        // request failure but we are done
        if ( done )
        {
            // update control block
            rc.stop = QDateTime::currentDateTime();

            rc.status = retStatus;
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
        result.append( plu );
        result.append( rc.uuid );
        result.append( QString::number( statusCode ) );
        result.append( QString::number( reply->size() ) );
        result.append( QString::number( rc.attempts - 1 ) );
        result.append( QString::number( elapsedTotal ) + "ms" );
        result.append( statusCodeLookup( statusCode ) );

        logQueryToFile( result, LOG_FILE );

        // log result
        LOG_INFO << "request " << qPrintable( rc.uuid ) << " took " << elapsedTotal << "ms (" << rc.status << ")";

        // check for exception message
        if (( SUCCESS != retStatus ) && ( response.isObject() ))
        {
            const QJsonObject exception( response.object() );

            if ( exception.contains( JSON_EXCEPTION ) )
            {
                QString ex( "\n" );

                if ( exception.contains( JSON_TIMESTAMP ) )
                    ex += "    Stamp: " + exception[JSON_TIMESTAMP].toString() + "\n";

                if ( exception.contains( JSON_ERROR ) )
                {
                    ex += "    Error: " + exception[JSON_ERROR].toString();

                    if ( exception.contains( JSON_STATUS ) )
                        ex += " (" + QString::number( exception[JSON_STATUS].toInt() ) + ")";

                    ex += "\n";
                }

                ex += "    Excep: " + exception[JSON_EXCEPTION].toString() + "\n";

                if ( exception.contains( JSON_MESSAGE ) )
                    ex += "\n    Message:\n    " + exception[JSON_MESSAGE].toString() + "\n";

                if ( exception.contains( JSON_PATH ) )
                    ex += "\n    Path:\n    " + exception[JSON_PATH].toString() + "\n";

                LOG_WARN << "received exception:" << qPrintable( ex );
            }
        }

        // emit!
        if ( rc.file.isEmpty() )
            emit processDocument( rc.request, rc.status, rc.response );
        else
        {
            emit processFile( rc.request, rc.status, rc.location, rc.uuid, rc.file );

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
        LOG_DEBUG << "attempting request " << qPrintable( rc.uuid ) << " again " << rc.attempts << " " << rc.maxAttempts;
        processRequestControl( rc );
    }

    // destroy control block
    destroyRequestControl( reply );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *hteapi::processRequestControl( const requestControl& rc )
{
    QNetworkReply *reply( nullptr );

    LOG_DEBUG << "hteapi::processRequestControl() - uuid: " << qPrintable( rc.uuid ) << "  filename: " << qPrintable( rc.file );
    LOG_DEBUG << "hteapi::processRequestControl() - request: " << qPrintable( rc.request.toJson() );

    QUrl activeUrl( ( rc.usingBackupUrl ) ? rc.backupUrl : rc.url );
    LOG_DEBUG << "hteapi::processRequestControl() - active url: " << qPrintable( activeUrl.toString() );

    if ( rc.request.isEmpty() )
    {
        reply = get( activeUrl, false, rc.timeout );

        QUrl url = reply->url();
        if (reply->error()) {
            LOG_WARN << "Download of " << url.toEncoded().constData() << " failed:" << qPrintable(reply->errorString());
        } else {
            QString filename = rc.location;
            QString path = url.path();
            QString basename = QFileInfo(path).fileName();

            LOG_DEBUG << "get() reply filename " << qPrintable(filename) << "  path:" << qPrintable(path) << "  basename:" << qPrintable(basename);

            {
                QFile file(filename);
                if (!file.open(QIODevice::WriteOnly)) {
                    LOG_WARN << "Could not open file " << qPrintable(filename) << "for writing, error:" << qPrintable(file.errorString());
                }
                else
                {
                    file.write(reply->readAll());
                    file.close();
                    LOG_DEBUG << "Download of file " << url.toEncoded().constData() << " succeeded, saved to " << qPrintable(filename);
                }
            }
        }

    }
    else
        reply = post( activeUrl, rc.request, false, rc.timeout );

    if ( reply )
        writeRequestControl( reply, rc );
    else
    {
        LOG_WARN << "process request " << qPrintable( rc.uuid ) << " failed!";
    }

    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteapi::requestControl hteapi::createDocumentRequestControl( const QUrl& url, const QString& uuid, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts, const QUrl& backupUrl )
{
    // create request control
    requestControl rc;
    rc.start = QDateTime::currentDateTime();
    rc.timeout = timeout;
    rc.maxAttempts = maxAttempts;
    rc.attempts = 0;
    rc.url = url;
    rc.uuid = uuid;
    rc.request = request;
    rc.backupUrl = backupUrl;
    rc.usingBackupUrl = false;

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteapi::requestControl hteapi::createFileRequestControl( const QUrl& url, const QString& uuid, const QString& filename, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts )
{
    QTemporaryFile f( TEMP_FILE );
    QString location;

    // generate a temporary file
    if ( !f.open() )
    {
        LOG_WARN << "error creating temporary file!";
        location = TEMP_FILE;
    }
    else
    {
        location = f.fileName();
        f.close();

        LOG_INFO << "temp file for " << qPrintable( uuid ) << " created " << qPrintable( location );
    }

    LOG_DEBUG << "hteapi::createFileRequestControl() - uuid: " << qPrintable( uuid ) << "  download url: " << qPrintable( url.toString() ) << "  filename: " << qPrintable( filename );

    // create request control
    requestControl rc;
    rc.start = QDateTime::currentDateTime();
    rc.timeout = timeout;
    rc.maxAttempts = maxAttempts;
    rc.attempts = 0;
    rc.url = url;
    rc.uuid = uuid;
    rc.request = request;
    rc.file = filename;
    rc.location = location;
    rc.backupUrl = QUrl();
    rc.usingBackupUrl = false;

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteapi::requestControl hteapi::readRequestControl( QNetworkReply *reply ) const
{
    requestControl rc;

    QMutexLocker guard( &m_ );

    if ( !pending_.contains( reply ) )
        LOG_WARN << "uuid not found in pending!";
    else
    {
        rc = pending_[reply];
    }

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::writeRequestControl( QNetworkReply *reply, const requestControl& rc )
{
    QMutexLocker guard( &m_ );
    pending_[reply] = rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteapi::destroyRequestControl( QNetworkReply *reply )
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
void hteapi::logQueryToFile( const queryResult& query, const QString& filename ) const
{
    LOG_DEBUG << "hteapi::logQueryToFile() filename: " << qPrintable(filename);

    QFile f( filename );

    // create file if not existing
    if (( !QFile::exists( filename ) ) && ( f.open( QIODevice::WriteOnly | QIODevice::Text ) ))
    {
        f.write( "TIMESTAMP,PLU,UUID,RETURNCODE,RETURNBYTES,RETRIES,TIMESPENT,COMMENTS\n" );
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
        newrequest.append( query[PLU] + "," );
        newrequest.append( query[UUID] + "," );
        newrequest.append( query[RETURNCODE] + "," );
        newrequest.append( returnBytes + "," );
        newrequest.append( query[RETRIES] + "," );
        newrequest.append( timeSpent + ",");
        newrequest.append( query[COMMENTS]);

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
QString hteapi::statusCodeLookup( int code )
{
    QString status;

    // In the case we cannot reach the web service, the web service times out, or we
    // receive a return code not listed above we will use cached PLU data as per caching policy

    switch( code )
    {
    case 200:
        status = "200 - SUCCESS";                // scale on file, walmart on file
        break;
    case 204:
        status = "204 - NO DATA - USE CACHE POLICY";
        break;
    case 0:
    // case 204:                                // this is cache data now according to Walmart 08.20.2020
    case 400:
        // BAD REQUEST - Bad syntax, Server cannot or will not reply to request.  The server cannot or will not process the request due
        // to an apparent client error (e.g., malformed request syntax, size too large, invalid request message framing, or deceptive request routing
        status = QString::number(code) + " NO DATA, PLU NOT ON FILE - DO NOT USE CACHE";
        break;
    case 404:
        status = "404 SERVICE NOT AVAILABLE - USE CACHE POLICY";
        break;
    case 408:
        status = "408 CLIENT REQUEST TIMEOUT ERROR - USE CACHE POLICY";
        break;
    case 417:
        status = "417 INVALID REQUST ERROR - USE CACHE POLICY";
        break;
    case 500:
        status = "500 INTERNAL SERVER ERROR - USE CACHE POLICY";
        break;
    case 504:
        status = "504 GATEWAY TIMEROUT ERROR - USE CACHE POLICY";
        break;
    case 525:
        status = "525 SSL HANDSHAKE FAILED - USE CACHE POLICY";
        break;
    case 601:
        status = "601 EXCEEDED THE CONFIGURED NUMBER OF RETRIES";
        break;
    case 602:
        status = "602 EXCEEDED THE CONFIGURED TIMEOUT";
        break;
    default:
        status = "UNKNOWN CODE - USE CACHE POLICY, CODE " + QString::number(code);
        break;
    }

    return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteapi::exitRetries( int code )
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
    case 204:                                // PLU does not exist so exit even with no data - this is cache data now according to Walmart 08.20.2020
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
