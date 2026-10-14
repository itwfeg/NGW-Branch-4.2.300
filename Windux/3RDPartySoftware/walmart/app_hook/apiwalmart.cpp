/**
 * @file apiwalmart.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiwalmart.h"
#include "networkaccess.h"

#include <QDataStream>
#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVector>

#include <clio.h>

#define DEBUG_JSON
#define DEBUG_JSON_SAVE

static const QString APPLICATION_JSON( "application/json" );
static const QString LAST_USAGE_FILE( "/usr/share/walmart-apphook/lastapiusage" );

QMutex walmartapi::lastUsage_;

int walmartapi::uuidCtr_( 0 );

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi::walmartapi( QObject *parent ) :
    _Mybase( parent ),
    networkAccess_( nullptr ),
    ssl_( true ),
    host_( "" ),            // read from file but orig value = host_( "scales-legacy-api.dev.walmart.com" ),
    useCacheWhenOffline_( false ),
    pending_( 0 ),
    howManyRequests_( 3 ),
    requestTimeout_( 2000 ),
    allItemsRequestTimeout_( 30000 ),
    savedRequests_( 20 ),
    deptOverride_( -1 ),
    noQueryParams_( false )
{
    networkAccess_ = new networkAccess( this );

    timer_ = new QTimer( this );
    connect( timer_, SIGNAL(timeout()), SLOT(onTimeoutHit()) );

    headers_.append( "Correlation_ID" );
    headers_.append( "SCALE_TYPE" );
    headers_.append( "SCALE_ID" );
    headers_.append( "SCALE_NUMBER" );
    headers_.append( "IP_ADDRESS" );
    headers_.append( "WM_CONSUMER.ID" );
    headers_.append( "WM_SVC.NAME" );
    headers_.append( "WM_SVC.VERSION" );
    headers_.append( "WM_SVC.ENV" );

    // setup endpoing key values
    pathKeys_["GetOneItem"] = API_GET_ONE_ITEM;
    pathKeys_["GetAllItems"] = API_GET_ALL_ITEMS;
    pathKeys_["HealthCheck"] = API_HEALTH_CHECK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi::~walmartapi()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi::RequestStatus walmartapi::getOneItem( const QString& plu, const QString& country, const QString& store, const QString& scaleDept, QJsonDocument &response )
{
    LOG_DEBUG << "Start Server Request with plu = " << plu.toStdString();

    RequestStatus retStatus( SUCCESS );
    int retCode( 200 );
    int replylen( 0 );
    int j;

    QString printerType( httpHeadersMap( "SCALE_TYPE" ) );

    if ( printerType.isEmpty() )
        printerType = "HOBART";

    QString dept( scaleDept );

    if ( 0 <= deptOverride_ )
        dept = QString::number( deptOverride_ );

    QString query( "plu=" + plu + "&countryCode=" + country + "&printerType=" + printerType + "&storeNbr=" + store );

    if ( noQueryParams_ )
        query.clear();

    urlParams_.clear();
    urlParams_["{countryCode}"] = country;
    urlParams_["{bannerCode}"] = bannerCode_;
    urlParams_["{storeNumber}"] = store;
    urlParams_["{departmentId}"] = dept;
    urlParams_["{itemNumber}"] = plu;

    const QDateTime now( QDateTime::currentDateTime() );
    const QString correlationId( generateUniqueId( store.toInt(), now.toMSecsSinceEpoch() / 1000 ) );

    setHttpHeadersMap( "Correlation_ID", correlationId );

    reqResults_.clear();
    reqResults_[TIMESTAMP] = now.toString("hh:mm:ss:zzz AP MM/dd/yyyy");                                // timestamp
    reqResults_[PLU] = plu;                                                                             // plu
    reqResults_[UUID] = correlationId;                                                                  // uuid

    for( j = 0; j < howManyRequests_; j++ )
    {
        // post!
        QNetworkReply *reply( get( API_GET_ONE_ITEM, query, true ) );
        reply->deleteLater();

        // save off last usage info
        if ( QNetworkReply::NoError == reply->error() )
            exportLastUsage( paths_[API_GET_ONE_ITEM] );

        // parse response - response returned to caller
        response = parseNetworkReply( reply, retStatus, replylen, retCode );

        LOG_TRACE << "loop " << j+1 << " of " << howManyRequests_;
        LOG_TRACE << "retStatus: " << retStatus << "     retCode code: " << retCode;

        if (( 0 < replylen ) || ( exitRetries( retCode ) ))
        {
            LOG_DEBUG << "we got a returned JSON doc or a good code so BREAK OUT and Process it";
            break;
        }
    }

    if (( response.isEmpty() ) && ( j == howManyRequests_ ))
    {
        LOG_WARN << "Exceeded our number of retries and  the response is empty";
        retCode = 601;                                                                                  // internal Hobart return code, exceeded loop retries
    }

    // Returned data and how long it took
    reqResults_[RETURNCODE] = QString::number(retCode);                                                 // return code
    reqResults_[RETRIES] = QString::number(j+1);                                                        // we are counting these as TRIES, not RETRIES
    reqResults_[RETURNBYTES] = QString::number( replylen );                                             // number of bytes returned

    // Time Spent
    reqResults_[TIMESPENT] = QString::number( now.msecsTo( QDateTime::currentDateTime() ) ) + "ms";                                        // total time spent on transaction
    reqResults_[COMMENTS] = statusCodeLookup(retCode);

    logQueryToFile("WebServiceRequests.log");                                                           // log the results of the query

#ifdef DEBUG_JSON
    saveDocument( response, "response.json" );
#endif

    LOG_INFO << "finished server request with a return status: " << retStatus << "\n\n";
    return retStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi::RequestStatus walmartapi::getAllItems( const QString& country, const QString& store, const QString& scaleDept, QJsonDocument &response )
{
    LOG_DEBUG << "Start Server Request for all plu's";

    RequestStatus retStatus( SUCCESS );
    int retCode( 200 );
    int replylen( 0 );
    int j;

    QString dept( scaleDept );

    if ( 0 <= deptOverride_ )
        dept = QString::number( deptOverride_ );

    QString query( "countryCode=" + country + "&storeNbr=" + store );

    if ( noQueryParams_ )
        query.clear();

    urlParams_.clear();
    urlParams_["{countryCode}"] = country;
    urlParams_["{bannerCode}"] = bannerCode_;
    urlParams_["{storeNumber}"] = store;
    urlParams_["{departmentId}"] = dept;

    const QDateTime now( QDateTime::currentDateTime() );
    const QString correlationId( generateUniqueId( store.toInt(), now.toMSecsSinceEpoch() / 1000 ) );

    setHttpHeadersMap( "Correlation_ID", correlationId );

    reqResults_.clear();
    reqResults_[TIMESTAMP] = now.toString("hh:mm:ss:zzz AP MM/dd/yyyy");                                // timestamp
    reqResults_[PLU] = "N/A";                                                                           // plu
    reqResults_[UUID] = correlationId;                                                                  // uuid

    for( j = 0; j < howManyRequests_; j++ )
    {
        // post!
        QNetworkReply *reply( get( API_GET_ALL_ITEMS, query, true ) );
        reply->deleteLater();

        // save off last usage info
        if ( QNetworkReply::NoError == reply->error() )
            exportLastUsage( paths_[API_GET_ALL_ITEMS] );

        // parse response - response returned to caller
        response = parseNetworkReply( reply, retStatus, replylen, retCode );

        LOG_TRACE << "loop " << j+1 << " of " << howManyRequests_;
        LOG_TRACE << "retStatus: " << retStatus << "     retCode code: " << retCode;

        if (( 0 < replylen ) || ( exitRetries( retCode ) ))
        {
            LOG_DEBUG << "we got a returned JSON doc or a good code so BREAK OUT and Process it";
            break;
        }
    }

    if (( response.isEmpty() ) && ( j == howManyRequests_ ))
    {
        LOG_WARN << "Exceeded our number of retries and  the response is empty";
        retCode = 601;                                                                                  // internal Hobart return code, exceeded loop retries
    }

    // Returned data and how long it took
    reqResults_[RETURNCODE] = QString::number(retCode);                                                 // return code
    reqResults_[RETRIES] = QString::number(j+1);                                                        // we are counting these as TRIES, not RETRIES
    reqResults_[RETURNBYTES] = QString::number( replylen );                                             // number of bytes returned

    // Time Spent
    reqResults_[TIMESPENT] = QString::number( now.msecsTo( QDateTime::currentDateTime() ) ) + "ms";                                        // total time spent on transaction
    reqResults_[COMMENTS] = statusCodeLookup(retCode);

    logQueryToFile("WebServiceRequests.log");                                                           // log the results of the query

#ifdef DEBUG_JSON
    saveDocument( response, "response.json" );
#endif

    LOG_INFO << "finished server request with a return status: " << retStatus << "\n\n";
    return retStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi::RequestStatus walmartapi::healthCheck( const QString& country, const QString& store, const QString& scaleDept, const QJsonDocument &info, QJsonDocument &response )
{
    LOG_DEBUG << "Start Health Check";

    RequestStatus retStatus( SUCCESS );
    int retCode( 200 );
    int replylen( 0 );
    int j;

    QString dept( scaleDept );

    if ( 0 <= deptOverride_ )
        dept = QString::number( deptOverride_ );

    urlParams_.clear();
    urlParams_["{countryCode}"] = country;
    urlParams_["{bannerCode}"] = bannerCode_;
    urlParams_["{storeNumber}"] = store;
    urlParams_["{departmentId}"] = dept;

    const QDateTime now( QDateTime::currentDateTime() );
    const QString correlationId( generateUniqueId( store.toInt(), now.toMSecsSinceEpoch() / 1000 ) );

    setHttpHeadersMap( "Correlation_ID", correlationId );

    reqResults_.clear();
    reqResults_[TIMESTAMP] = now.toString("hh:mm:ss:zzz AP MM/dd/yyyy");                                // timestamp
    reqResults_[PLU] = "N/A";                                                                           // plu
    reqResults_[UUID] = correlationId;                                                                  // uuid

    for( j = 0; j < howManyRequests_; j++ )
    {
        // post!
        QNetworkReply *reply( post( API_HEALTH_CHECK, info, true ) );
        reply->deleteLater();

        // save off last usage info
        if ( QNetworkReply::NoError == reply->error() )
            exportLastUsage( paths_[API_HEALTH_CHECK] );

        // parse response - response returned to caller
        response = parseNetworkReply( reply, retStatus, replylen, retCode );

        LOG_TRACE << "loop " << j+1 << " of " << howManyRequests_;
        LOG_TRACE << "retStatus: " << retStatus << "     retCode code: " << retCode;

        if (( 0 < replylen ) || ( exitRetries( retCode ) ))
        {
            LOG_DEBUG << "we got a returned JSON doc or a good code so BREAK OUT and Process it";
            break;
        }
    }

    if (( response.isEmpty() ) && ( j == howManyRequests_ ))
    {
        LOG_WARN << "Exceeded our number of retries and  the response is empty";
        retCode = 601;                                                                                  // internal Hobart return code, exceeded loop retries
    }

    // Returned data and how long it took
    reqResults_[RETURNCODE] = QString::number(retCode);                                                 // return code
    reqResults_[RETRIES] = QString::number(j+1);                                                        // we are counting these as TRIES, not RETRIES
    reqResults_[RETURNBYTES] = QString::number( replylen );                                             // number of bytes returned

    // Time Spent
    reqResults_[TIMESPENT] = QString::number( now.msecsTo( QDateTime::currentDateTime() ) ) + "ms";                                        // total time spent on transaction
    reqResults_[COMMENTS] = statusCodeLookup(retCode);

    logQueryToFile("WebServiceRequests.log");                                                           // log the results of the query

#ifdef DEBUG_JSON
    saveDocument( response, "response.json" );
#endif

    LOG_INFO << "finished server request with a return status: " << retStatus << "\n\n";
    return retStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartapi::onFinished()
{
    LOG_DEBUG << "The request is finished ";

    RequestStatus retStatus;
    int retCode;
    int replylen;

    // decrement pending requests
    emit requestsPendingChanged( --pending_ );

    // get reply
    QNetworkReply *reply( qobject_cast<QNetworkReply*>( sender() ) );

    // nothing to do
    if ( !reply )
        return;

    reply->deleteLater();

    // process reply
    const QJsonDocument doc( parseNetworkReply( reply, retStatus, replylen, retCode ) );

    if (( retStatus < 0 ) || ( doc.isEmpty() ))
    {
        LOG_WARN << "invalid response or empty document";
        return;
    }

#ifdef DEBUG_JSON
    saveDocument( doc, "response.json" );
#endif

/*
    const QNetworkRequest request( reply->request() );
    const QUrl url( request.url() );

    // sheets
    if ( isUrlMatch( url, API_SHEETS ) )
    {
        const QUrl baseUrl( createUrlPath( API_SHEETS, QString() ) );
        const QString baseUrlString( baseUrl.toString() );

        const QString urlString( url.toString() );

        // check for sheet columns
        const int columns( urlString.indexOf( "/columns" ) );

        // strip out sheet id (if possible)
        QString sheetId;

        if ( 0 == urlString.indexOf( baseUrlString ) )
        {
            int trimPos;

            sheetId = urlString.mid( baseUrlString.length() );

            if (( sheetId.length() ) && ( '/' == sheetId[0] ))
                sheetId = sheetId.mid( 1 );

            if ( 0 <= (trimPos = sheetId.indexOf( '?' )) )
                sheetId = sheetId.left( trimPos );

            if ( 0 <= (trimPos = sheetId.indexOf( '/' )) )
                sheetId = sheetId.left( trimPos );
        }

        // ---- //

        // columns
        if (( 0 < columns ) && ( sheetId.length() ))
        {
            const smartsheetColumnMap objmap( doc );

            // emit!
            emit columnsChanged( sheetId.toLongLong(), objmap );
        }

        // sheet
        else if ( sheetId.length() )
        {
            if ( doc.isObject() )
            {
                const smartsheetSheet obj( doc.object() );

                // emit!
                if ( sheetId.toLongLong() == obj.id )
                {
                    smartsheetSheetMap objmap;
                    objmap[obj.id] = obj;

                    emit sheetsChanged( objmap );
                }
            }
        }

        // sheets
        else
        {
            const smartsheetSheetMap objmap( doc );

            // check for pagination
            if ( doc.isObject() )
            {
                static const QString pageNumber( "pageNumber" );
                static const QString totalPages( "totalPages" );

                QJsonObject pobj( doc.object() );

                if (( pobj.contains( pageNumber ) ) &&
                    ( pobj.contains( totalPages ) ))
                {
                    const QJsonValue pobjPageNumber( pobj.value( pageNumber ) );
                    const QJsonValue pobjTotalPages( pobj.value( totalPages ) );

                    // request next page (since this is not last page)
                    if (( pobjPageNumber.isDouble() ) &&
                        ( pobjTotalPages.isDouble() ) &&
                        ( pobjPageNumber.toInt() < pobjTotalPages.toInt() ))
                    {
                        sheets( pobjPageNumber.toInt() + 1 );
                    }
                }
            }

            // emit!
            emit sheetsChanged( objmap );
        }
    }
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QUrl walmartapi::createUrl( endpoint e, const QString &query ) const
{
    return createUrlPath( e, QString(), query );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QUrl walmartapi::createUrlPath( endpoint e, const QString &path, const QString &query ) const
{
    QString endpoint( paths_[e] );

    foreach ( const QString& key, urlParams_.keys() )
        endpoint.replace( key, urlParams_[key] );

    const QString prefix( ssl_ ? "https" : "http" );
    const QString urlString( prefix + "://" + host_ + "/" + endpoint + path );

    // convert string to url
    QUrl url( QUrl::fromUserInput( urlString ) );

    // attach query
    if ( query.length() )
        url.setQuery( query );

    return url;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartapi::isUrlMatch( const QUrl &url, endpoint e ) const
{
    const QString urlString( url.toString() );

    return urlString.startsWith( createUrl( e ).toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *walmartapi::handleRequest( method m, const QUrl &url, bool blocking, const QJsonDocument &doc )
{
    // create request
    QNetworkRequest request( url );

    const QString urlString( url.toString() );
    LOG_INFO << "request type " << m << " url " << urlString.toStdString();

    if ( !doc.isNull() )
    {
        request.setHeader( QNetworkRequest::ContentTypeHeader, APPLICATION_JSON );      // application/json

#ifdef DEBUG_JSON
        saveDocument( doc, "request.json" );
#endif
    }

    // set headers
    foreach ( const QString& key, headers_ )
    {
        const QByteArray a( httpHeadersMap( key ) );

        LOG_TRACE << "header info " << key.toStdString() << " " << a.toStdString();
        request.setRawHeader( key.toUtf8(), a );
    }

    // set headers
    foreach ( const QString& key, customHeaders_ )
    {
        const QByteArray a( httpHeadersMap( key ) );

        LOG_TRACE << "header info " << key.toStdString() << " " << a.toStdString();
        request.setRawHeader( key.toUtf8(), a );
    }

    // post!
    QNetworkReply *reply( NULL );

    if ( DELETE == m )
        reply = networkAccess_->deleteResource( request );
    else if ( GET == m )
        reply = networkAccess_->get( request );
    else if ( POST == m )
        reply = networkAccess_->post( request, doc.toJson() );
    else if ( PUT == m )
        reply = networkAccess_->put( request, doc.toJson() );

    // wait for response if blocking
    if (( reply ) && ( blocking ))
    {
        int requestTimeout( requestTimeout_ );

        if ( urlString.contains( paths_[API_GET_ALL_ITEMS] ) )
            requestTimeout = allItemsRequestTimeout_;

        LOG_DEBUG << "wait for response " << requestTimeout << " ms timeout...";

        // setup an event loop
        QEventLoop eventLoop;
        connect( reply, SIGNAL(finished()), &eventLoop, SLOT(quit()) );
        connect( timer_, SIGNAL(timeout()), &eventLoop, SLOT(quit()) );

        // start timer
        timer_->setSingleShot( true );
        timer_->start( requestTimeout );

        // wait...

        LOG_TRACE << "exec...";
        eventLoop.exec();

        LOG_TRACE << "exec... complete";
        timer_->stop();

        disconnect( reply, 0, 0, 0 );
        disconnect( timer_, 0, 0, 0 );
    }

    LOG_TRACE << "done";
    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument walmartapi::parseNetworkReply( QNetworkReply *reply, RequestStatus &retStatus, int &replyLen, int &retCode )
{
    // check status code
    const QVariant statusCode( reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ) );
    retCode = statusCode.toInt();

    // process reply
    if ( QNetworkReply::NoError != reply->error() )
    {
        const QString errorString( reply->errorString() );

        LOG_WARN << "network reply error " << reply->error() << " " << errorString.toStdString();

        switch( retCode )
        {
            case 0:     // if we are offline
            case 404:
            case 417:
                LOG_INFO << "CACHE DATA returned http status code " << retCode;
                retStatus = CACHEDATA;
                break;

            default:                            //  mostly wanted to catch unknown codes so I split these out of known/unknown
                LOG_INFO << "default NODATA UNKNOWN CODE  " << retCode;
                retStatus = NODATA;
                break;
        }
    }
    else
    {
        switch( retCode )
        {
            case 200:
                // scale on file, walmart on file
                LOG_INFO << "SUCCESS returned http status code " << retCode;
                retStatus = SUCCESS;
                break;

            case 0:
                // SOF-4634: use cache if server offline
                if ( useCacheWhenOffline() )
                {
                    LOG_INFO << "CACHE DATA, use it if we have it, returned http status code " << retCode;
                    retStatus = CACHEDATA;
                    break;
                }

                // fall thru...

            case 400:
                LOG_INFO << "NO DATA returned http status code " << retCode;
                retStatus = NODATA;
                break;

            default:
                LOG_INFO << "CACHE DATA, use it if we have it, returned http status code " << retCode;
                retStatus = CACHEDATA;
                break;
        }
    }

    // check content
    const QString contentType( reply->header( QNetworkRequest::ContentTypeHeader ).toString() );

    QJsonDocument doc;

    if ( !contentType.contains( APPLICATION_JSON ) )
        LOG_WARN << "bad network request content type" << contentType.toStdString();
    else
    {
        LOG_DEBUG << "good network request so process it";
        QJsonParseError err;

        // parse!
        const QByteArray a( reply->readAll() );
        replyLen = a.length();

        doc = QJsonDocument::fromJson( a, &err );

        if ( QJsonParseError::NoError != err.error )
        {
            const QString errorString( err.errorString() );

            LOG_WARN << "error parsing network request JSON document " << err.error << " " << err.offset << " " << errorString.toStdString();
        }
    }

    return doc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartapi::saveDocument( const QJsonDocument &doc, const QString &filename )
{
#ifdef DEBUG_JSON
    const QByteArray a( doc.toJson() );

    LOG_TRACE << HEX_DUMP( a.constData(), a.length() );

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

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartapi::setHttpHeadersMap( const QString& key, const QString& value, bool custom )
{
    // config
    if ( "NumberOfRequestsSaved" == key )
        savedRequests_ = value.toInt() + 1;
    else if ( "TimeoutLength" == key )
        requestTimeout_ = value.toInt();
    else if ( "AllItemsTimeoutLength" == key )
        allItemsRequestTimeout_ = value.toInt();
    else if ( "TimeoutRetryInterval" == key )
        howManyRequests_ = value.toInt();

    else if ( "BannerCode" == key )
        bannerCode_ = value;
    else if ( "Department" == key )
        deptOverride_ = value.toInt();
    else if ( "NoQueryParams" == key )
        noQueryParams_ = (0 != value.toInt());

    // headers
    else
    {
        webheaders_[key] = value.toUtf8();

        if ( custom )
            customHeaders_.append( key );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartapi::endpointPath( const QString& key ) const
{
    return paths_[pathKeys_[key]];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartapi::setEndpointPath( const QString& key, const QString& value )
{
    paths_[pathKeys_[key]] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray walmartapi::httpHeadersMap( const QString& key ) const
{
    return webheaders_[key];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartapi::generateUniqueId( int storeNo, uint64_t epoch_secs )
{
    QString timeLow( QString::number( epoch_secs & 0x00000000ffffffff, 16 ) );

    while ( timeLow.length() < 8 )
        timeLow.prepend( '0' );

    // ---- //

    const QString addr( httpHeadersMap( "IP_ADDRESS" ) );
    const QStringList addrList( addr.split( '.' ) );

    const int addr0 = (0 < addrList.length()) ? addrList[0].toInt() : 0;
    const int addr1 = (1 < addrList.length()) ? addrList[1].toInt() : 0;
    const int addr2 = (2 < addrList.length()) ? addrList[2].toInt() : 0;
    const int addr3 = (3 < addrList.length()) ? addrList[3].toInt() : 0;

    QString addrHex( QString::number( (addr0 << 24) + (addr1 << 16) + (addr2 << 8) + addr3, 16 ) );

    while ( addrHex.length() < 8 )
        addrHex.prepend( '0' );

    // ---- //

    const int index = ((++uuidCtr_) & 0x000f);
    const int store = (storeNo & 0x0fff);

    QString combo( QString::number( (index << 12) + store, 16 ) );

    while ( combo.length() < 4 )
        combo.prepend( '0' );

    // ---- //

    // format:
    // EEEEEEEE-NNNNNNNN-CSSS
    //
    // where
    // EEEEEEEE = low 32 bits of epoch time in seconds
    // NNNNNNNN = network ip address
    // C = low 4 bits of uuid counter
    // SSS = low 12 bits of store number

    return timeLow + "-" + addrHex + "-" + combo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartapi::exitRetries(int code)
{
    bool exitRetryLoop( true );

    switch( code )
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

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartapi::statusCodeLookup(int code)
{
    QString status( "" );

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
void walmartapi::logQueryToFile(const QString &filename)
{
    QString path( "/var/log/" );
    path.append( filename );
    QFile f( path );

    // create file if not existing
    if (( !QFile::exists( path ) ) && ( f.open( QIODevice::WriteOnly | QIODevice::Text ) ))
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
        QString returnBytes( reqResults_[RETURNBYTES] );

        if ( 8 <= returnBytes.length() )
            returnBytes = QString::number( returnBytes.toInt() / (1024*1024) ) + "M";
        else if ( 5 <= returnBytes.length() )
            returnBytes = QString::number( returnBytes.toInt() / 1024 ) + "K";

        // trim down time when too large
        QString timeSpent( reqResults_[TIMESPENT] );

        if ( 6 <= timeSpent.length() )
        {
            timeSpent.replace( "ms", "" );

            timeSpent = QString::number( timeSpent.toInt() / 1000 );
            timeSpent += "s";
        }

        // build new request
        QString newrequest( reqResults_[TIMESTAMP] + "," );
        newrequest.append( reqResults_[PLU] + "," );
        newrequest.append( reqResults_[UUID] + "," );
        newrequest.append( reqResults_[RETURNCODE] + "," );
        newrequest.append( returnBytes + "," );
        newrequest.append( reqResults_[RETRIES] + "," );
        newrequest.append( timeSpent + ",");
        newrequest.append( reqResults_[COMMENTS]);

        if( savedRequests_ == requests.count() )            // the +1 above when read accounts for the header
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
void walmartapi::onTimeoutHit()
{
    LOG_DEBUG << "TIMED OUT SO BREAK OUT";
    reqResults_[COMMENTS] = statusCodeLookup(602);          // expired?
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartapi::exportLastUsage( const QString &api )
{
    QFile f( LAST_USAGE_FILE );

    if ( !f.open( QIODevice::WriteOnly ) )
    {
        LOG_WARN << "could not open last usage file " << LAST_USAGE_FILE.toStdString();
        return;
    }

    const QDateTime now( QDateTime::currentDateTime() );

    QDataStream s( &f );
    s << now;
    s << api;

    f.close();

    const QString str( now.toString() );
    LOG_DEBUG << "wrote last usage as " << str.toStdString() << " " << api.toStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartapi::importLastUsage( QDateTime& stamp, QString& api )
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
        LOG_WARN << "could not open last usage file " << LAST_USAGE_FILE.toStdString();
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
    LOG_DEBUG << "read last usage as " << str.toStdString() << " " << importAPI.toStdString();

    stamp = importStamp;
    api = importAPI;

    return true;
}
