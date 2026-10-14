/**
 * @file hookwalmartbase.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiwalmart.h"
#include "callbackrequest.h"
#include "common.h"
#include "configrequest.h"
#include "downloadfilerequest.h"
#include "eventcontrollerrequest.h"
#include "fetchallproductsrequest.h"
#include "hookwalmartbase.h"
#include "scaleconfig.h"
#include "scalesettings.h"
#include "settingsrequest.h"
#include "stringsconfig.h"
#include "stringsjson.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>
#include <QPair>
#include <QSettings>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

#if defined(Q_OS_LINUX)
static const QString EVENTS_DIR( "/usr/share/walmart-apphook/events" );

static const QString WEB_HEADERS_FILE( "/usr/share/walmart-apphook/web_headers.config" );

static const QString FETCH_PLU_OFFSET_TIMESTAMP( "/usr/share/walmart-apphook/fetchpluoffsettimestamp" );
static const QString FETCH_PLU_OFFSET_TIMESTAMP_RAW( "/usr/share/walmart-apphook/fetchpluoffsettimestampraw" );

static const QString SCALE_DOWNLOAD_DIR( "/usr/local/hobart/download" );
#elif defined(Q_OS_WIN32)
static const QString EVENTS_DIR( "events" );

static const QString WEB_HEADERS_FILE( "web_headers.config" );

static const QString FETCH_PLU_OFFSET_TIMESTAMP( "fetchpluoffsettimestamp" );
static const QString FETCH_PLU_OFFSET_TIMESTAMP_RAW( "fetchpluoffsettimestampraw" );
#endif

static const QString REQUEST_JSON_DOC( "request.json" );
static const QString RESPONSE_JSON_DOC( "response.json" );
static const QString EVENT_JSON_DOC( "event.json" );

static const QString PARENT_UUID( "parent" );

static const QString PENDING( "pending" );
static const QString COMPLETE( "complete" );
static const QString FAILED( "failed" );
static const QString PROCESSED( "processed" );

static const QString UPDATE_CONFIG( "UPDATE_CONFIGURATION" );
static const QString APPLY_SETTINGS( "APPLY_SETTINGS" );
static const QString FETCH_PLU( "FETCH_PLU" );
static const QString UPGRADE_FIRMWARE( "UPGRADE_FIRMWARE" );
static const QString INSTALL_FEATURE( "INSTALL_FEATURE" );
static const QString LOAD_LABELS( "LOAD_LABELS" );

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookBase::walmartHookBase( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    api_( nullptr ),
    requestTimeout_( 0 ),
    requestNumRetries_( 0 )
{
    QDir d;

    LOG_DEBUG << "ctor";

    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // create folder for events
    if ( d.exists( EVENTS_DIR ) )
        cleanupRequests( true );
    else if ( !d.mkdir( EVENTS_DIR ) )
        LOG_ERROR << "error creating events dir " << qPrintable( EVENTS_DIR );

    // setup trigger types
    QList<triggerTypeInfo> supportedTriggers;
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::WasOffline, CONF_HEALTH_CHECK_RECONNECT_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::ScalePowerUp, CONF_HEALTH_CHECK_STARTUP_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::ScalePowerDown, CONF_HEALTH_CHECK_SHUTDOWN_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::Interval, CONF_HEALTH_CHECK_INTERVAL_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::Manual, QString() ) );

    foreach ( const triggerTypeInfo& trigger, supportedTriggers )
        triggerTypes_[eventControllerRequest::requestTypeToString( (eventControllerRequest::RequestType) trigger.first )] = trigger;

    // setup enabled events
    enabledEvents_[UPDATE_CONFIG] = CONF_UPDATE_CONFIG_ENABLED;
    enabledEvents_[APPLY_SETTINGS] = CONF_APPLY_SETTINGS_ENABLED;
    enabledEvents_[FETCH_PLU] = CONF_FETCH_PLU_ENABLED;
    enabledEvents_[UPGRADE_FIRMWARE] = CONF_UPGRADE_FIRMWARE_ENABLED;
    enabledEvents_[INSTALL_FEATURE] = CONF_INSTALL_FEATURE_ENABLED;
    enabledEvents_[LOAD_LABELS] = CONF_LOAD_LABELS_ENABLED;

    // setup api
    api_ = new walmartapi( this );

    connect( api_, SIGNAL(processDocument(const QJsonDocument&,int,const QJsonDocument&)), SLOT(onProcessDocument(const QJsonDocument&,int,const QJsonDocument&)) );
    connect( api_, SIGNAL(processFile(const QJsonDocument&,int,const QString&,const QString&,const QString&)), SLOT(onProcessFile(const QJsonDocument&,int,const QString&,const QString&,const QString&)) );

    // setup api headers
    loadWebHeaders();

    // startup timer
    startup_ = new QTimer( this );
    startup_->setInterval( INITIALIZE_INTERVAL );
    startup_->setSingleShot( false );
    startup_->start();

    connect( startup_, SIGNAL(timeout()), SLOT(onInitialize()) );

    // state machine timer
    stateMachine_ = new QTimer( this );
    stateMachine_->setInterval( STATE_MACHINE_INTERVAL );
    stateMachine_->setSingleShot( false );
    stateMachine_->start();

    connect( stateMachine_, SIGNAL(timeout()), SLOT(onProcessStateMachine()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookBase::~walmartHookBase()
{
    LOG_DEBUG << "dtor";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::performHealthCheck( const QString& triggerType, bool force )
{
    QMutexLocker guard( &m_ );

    bool valid;

    // retrieve url
    const QUrl url( scaleConfig::instance()->value<QString>( CONF_HEALTH_CHECK_URL, &valid ) );

    // validate url and trigger type
    if ( !valid )
    {
        LOG_WARN << "configuration missing";
        return;
    }
    else if ( !triggerTypes_.contains( triggerType ) )
    {
        LOG_WARN << "unknown trigger type " << qPrintable( triggerType );
        return;
    }
    else if ( startup_->isActive() )
    {
        LOG_DEBUG << "waiting for power up...";
        return;
    }

    // which request type
    const triggerTypeInfo info( triggerTypes_[triggerType] );
    const eventControllerRequest::RequestType rt( (eventControllerRequest::RequestType) info.first );

    // ignore when we have not sent power up
    if (( eventControllerRequest::ScalePowerUp != rt ) && ( startup_->isActive() ))
    {
        LOG_INFO << "ignore event";
        return;
    }

    // check enabled
    if ( force )
        LOG_DEBUG << "event is forced";
    else if ( info.second.length() )
    {
        LOG_TRACE << "checking path " << qPrintable( info.second );
        const bool execute( scaleConfig::instance()->value<bool>( info.second, &valid ) );

        if ( !valid )
        {
            LOG_WARN << "trigger configuration missing " << qPrintable( info.second );
            return;
        }
        else if ( !execute )
        {
            LOG_DEBUG << "event is disabled";
            return;
        }
    }

    // format label version info
    QJsonArray labels;

    const labelVersionMap labelInfo( labelVersionInfo() );
    const QList<int> labelNums( labelInfo.keys() );

    foreach ( int labelNum, labelNums )
    {
        const labelNameVersionPair nameVersion( labelInfo[labelNum] );

        LOG_DEBUG << "label " << labelNum << " (" <<
            qPrintable( nameVersion.first ) << "): " <<
            qPrintable( nameVersion.second );

        QJsonObject obj;
        obj[JSON_LABEL_NAME] = nameVersion.first;
        obj[JSON_LABEL_NUMBER] = labelNum;
        obj[JSON_VERSION] = nameVersion.second;

        labels.append( obj );
    }

    // update store settings
    QString storeId;
    QString storeName;
    QString storeGraphic;

    if ( storeInformation( storeId, storeName, storeGraphic ) )
        scaleSettings::setStoreInformation( storeId, storeName, storeGraphic );

    // request object
    eventControllerRequest request( rt, this );
    request.setLabelVersionInfo( labels );
    request.setLastComm( lastComm() );
    request.setLastItemReceived( lastItemChange() );
    request.setLastFetchProductStamp( lastProductFetch() );
    request.setProductCount( productCount() );

    LOG_DEBUG << "queue health check";
    queueRequest( url, request.toJsonDoc() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::onInitialize()
{
    QMutexLocker guard( &m_ );

    // startup
    if ( startup_ == sender() )
    {
        // check ready
        if ( !isReady() )
        {
            LOG_DEBUG << "not ready";
            return;
        }

        // check config
        if ( scaleConfig::instance()->isInitialized() )
        {
            startup_->stop();

            LOG_DEBUG << "processing startup";
            performHealthCheck( "SCALE_POWER_UP" );
        }
        else if ( initUrl_.isEmpty() )
            LOG_WARN << "empty init url!";
        else if ( pendingRequests() )
            LOG_INFO << "initialization pending...";
        else
        {
            LOG_INFO << "processing initialization";

            // request object
            configRequest request( this );
            request.setInitScaleInfo( initScaleVendor_, initScaleModel_ );

            LOG_DEBUG << "queue init";
            queueRequest( initUrl_, request.toJsonDoc() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response )
{
    QMutexLocker guard( &m_ );

    const QJsonObject reqObj( request.object() );
    const QString uuid( reqObj[JSON_REQUEST_UUID].toString() );

    LOG_DEBUG << "process document " << qPrintable( uuid ) << " " << status;

    if ( uuid.isEmpty() )
        LOG_WARN << "invalid uuid";
    else
    {
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );

        QString errorText( "Unknown error occured." );
        bool success( false );

        // check status
        if ( walmartapi::SUCCESS != status )
        {
            LOG_WARN << qPrintable( uuid ) << " bad status " << status;
            errorText = "Request failed status " + QString::number( status ) + " (no response or bad return code).";
        }
        else if ( !response.isObject() )
        {
            LOG_WARN << qPrintable( uuid ) << " response not an object";
            errorText = "Response document corrupt or invalid.";
        }
        else
        {
            // if we are getting responses from walmart api then we are online
            updateLastOnlineTime();

            // write response
            QFile f( eventDir + QDir::separator() + RESPONSE_JSON_DOC );

            if ( !f.open( QFile::WriteOnly ) )
            {
                LOG_WARN << "error saving response";
                errorText = "Error saving response document.";
            }
            else
            {
                QDataStream os( &f );
                os << response;

                // indicate complete
                if ( QDataStream::Ok == os.status() )
                    success = setRequestState( uuid, STATE_COMPLETE );

                f.close();
            }
        }

        // indicate failure
        if (( !success ) && ( !setRequestState( uuid, STATE_FAILED, errorText ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::onProcessFile( const QJsonDocument& request, int status, const QString& filenameCached, const QString& uuid, const QString& filename )
{
    Q_UNUSED( request )

    QMutexLocker guard( &m_ );

    LOG_DEBUG << "process file " << qPrintable( uuid ) << " " << status;

    if ( uuid.isEmpty() )
        LOG_WARN << "invalid uuid";
    else if ( filename.isEmpty() )
        LOG_WARN << "invalid filename";
    else
    {
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );

        QString errorText( "Unknown error occured." );
        bool success( false );

        // check download success
        if ( walmartapi::SUCCESS != status )
        {
            LOG_WARN << qPrintable( uuid ) << " bad status " << status;
            errorText = "Failed to download '" + filename + "' status " + QString::number( status ) + " (no response or bad return code).";
        }
        else if ( !QFile::exists( filenameCached ) )
        {
            LOG_WARN << qPrintable( uuid ) << " file missing " << qPrintable( filename );
            errorText = "Downloaded file '" + filename + "' is missing.";
        }
        else
        {
            const QString newFilename( eventDir + QDir::separator() + filename );

            // if we are getting responses from walmart api then we are online
            updateLastOnlineTime();

            LOG_DEBUG << qPrintable( uuid ) << " processing file " << qPrintable( filename ) << "...";

            // copy file into queue
            if ( !QFile::rename( filenameCached, newFilename ) )
            {
                LOG_WARN << "failed to move file " <<  qPrintable( filenameCached ) << " " << qPrintable( newFilename );
                errorText = "Error moving file '" + filename + "' from cache location.";
            }
            else
            {
                // indicate complete
                success = setRequestState( uuid, STATE_COMPLETE );
            }
        }

        // indicate failure
        if (( !success ) && ( !setRequestState( uuid, STATE_FAILED, errorText ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

        LOG_TRACE << "done";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::onProcessStateMachine()
{
    QMutexLocker guard( &m_ );

    /*
     * How this state machine works:
     *
     * Call queueReqest() to add a JSON document to the queue. A directory will be created to
     * store all info and files related to that document. The directory will have the uuid of
     * the document you queue.
     *
     * a) either request.json or event.json will be created
     *
     * At periodic intervals onProcessStateMachine() is invoked. This method will iterate over
     * all queued documents. The document will be processed and the state updated. The possible
     * states are as follows:
     *
     * STATE_NONE - needs sent
     * STATE_PENDING - sent; waiting on a reply
     * STATE_FAILED - unable to send or bad reply receieved
     * STATE_COMPLETE - valid reply receieved
     * STATE_PROCESSED - reply has been processed
     *
     * The flow of these state is as such:
     *
     * none  ->  pending  ->  complete  ------------->  processed
     *              |            |            |
     *              --------------------->  failed
     *
     * b) new documents will be sent and marked pending
     *
     * Callback methods onProcessDocument() or onProcessFile() will be invoked. For events, if a
     * download was required the file callback will occur. Otherwise, the document callback will
     * occur.
     *
     * c) callback is invoked with success or failure for the request; for success a response.json
     *    will be created -or- for events the downloaded file will be copied into the directory
     * d) the document will be marked as complete or failure
     *
     * When onProcessStateMachine() is invoked again the complete or failed event will be
     * processed.
     *
     * Documents that are complete are handled with either handleEventState() method
     * or handleRequestState() method depending upon type.
     *
     * Depending on request type many things could occur. For example we could process products,
     * install a feature file, install label types, install new settings or configuration, etc.
     * After this completes the document is purged from the system. If a callback was required it
     * it queued.
     *
     * e) documents in complete state are handled and marked as processed; if a callback is
     *    required is will be queued
     *
     * e) failed events are marked as processed; if a callback is required is will be
     *    queued
     *
     * At the end of processing cleanupRequests() is invoked to purge events from the system
     *
     * g) all processed events are purged from the system
     *
     * Repeat this process.
     */

    // iterate all events
    QDirIterator iter( EVENTS_DIR, QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( iter.hasNext() )
    {
        const QString eventDir( iter.next() );
        const QString uuid( iter.fileName() );

        const EventState stateBefore( requestState( uuid ) );

        LOG_TRACE << "processing " << qPrintable( uuid ) << " (" << stateBefore << ")...";

        bool error( false );

        if ( QFile::exists( eventDir + QDir::separator() + EVENT_JSON_DOC ) )
            handleEventState( uuid );
        else if ( QFile::exists( eventDir + QDir::separator() + REQUEST_JSON_DOC ) )
            handleRequestState( uuid );
        else
        {
            LOG_WARN << "json missing from " << qPrintable( uuid );
            error = true;
        }

        // check new state, we need to be progressing otherwise something wrong
        const EventState stateAfter( requestState( uuid ) );

        if (( stateBefore == stateAfter ) && ( STATE_PENDING != stateBefore ))
        {
            LOG_WARN << qPrintable( uuid ) << " in bad state " << stateBefore << " " << stateAfter;
            error = true;
        }

        // handle error
        if ( error )
        {
            LOG_DEBUG << qPrintable( uuid ) << " in error state!";
            setRequestState( uuid, STATE_PROCESSED );
        }
    }

    // cleanup
    cleanupRequests();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::loadWebHeaders()
{
    QSettings s( WEB_HEADERS_FILE, QSettings::IniFormat );

    walmartapi::headersMap headers;

    // look for web headers
    for ( int i = 1; i <= 16; i++ )
    {
        const QString headerKey( "Header" + QString::number( i ) );
        const QString headerVal( "HeaderVal" + QString::number( i ) );

        const QString key( s.value( headerKey ).toString() );
        const QString val( s.value( headerVal ).toString() );

        if (( key.length() ) && ( val.length() ))
        {
            LOG_TRACE << "loaded header " << i << ": " << qPrintable( key ) << " " << qPrintable( val );
            headers[key.toLatin1()] = val.toLatin1();
        }
        else
        {
            break;
        }
    }

    // set web headers
    if ( headers.size() )
        api_->setHeaders( headers );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::handleEventState( const QString& uuid )
{
    const EventState state( requestState( uuid ) );

    // waiting for reply
    if ( STATE_PENDING == state )
        LOG_TRACE << qPrintable( uuid ) << " is pending";

    else
    {
        QUrl url;
        QJsonDocument request;

        // read request
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + EVENT_JSON_DOC );

        QFile f( filename );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "failed to open " << qPrintable( filename );
        else
        {
            QDataStream is( &f );
            is >> url;
            is >> request;

            f.close();
        }

        if ( !request.isObject() )
            LOG_WARN << "not an object";
        else
        {
            const QJsonObject obj( request.object() );

            const QString type( obj[JSON_EVENT].toString() );

            // send request
            if ( STATE_NONE == state )
            {
                if (( url.isEmpty() ) || ( !url.isValid() ))
                    LOG_WARN << "invalid url " << qPrintable( url.toString() );
                else if ( !enabledEvents_.contains( type ) )
                    LOG_WARN << "unsupported event type " << qPrintable( type );
                else
                {
                    bool success( true );

                    //
                    // CONFIG REQUEST
                    //
                    if ( UPDATE_CONFIG == type )
                    {
                        configRequest linkedRequest;

                        LOG_DEBUG << "queue config request";
                        success = queueRequest( url, linkedRequest.toJsonDoc(), uuid );
                    }

                    //
                    // SETTINGS REQUEST
                    //
                    else if ( APPLY_SETTINGS == type )
                    {
                        settingsRequest linkedRequest;

                        LOG_DEBUG << "queue settings request";
                        success = queueRequest( url, linkedRequest.toJsonDoc(), uuid );
                    }

                    //
                    // PRODUCTS REQUEST
                    //
                    else if ( FETCH_PLU == type )
                    {
                        // as per Walmart Varun Mannuru, do not append a null offset timestamp
                        const QString offsetTimestamp( lastProductFetch() );

                        if (( !offsetTimestamp.isEmpty() ) && ( !offsetTimestamp.isNull() ))
                        {
                            // set offset timestamp query item
                            static const QString offsetTimestampQuery( "offsetTimestamp" );

                            QUrlQuery urlQuery( url.query() );
                            bool found( false );

                            const QList<urlQueryParams> queryItems( urlQuery.queryItems() );

                            urlQuery.clear();

                            foreach ( const urlQueryParams& item, queryItems )
                            {
                                if ( offsetTimestampQuery != item.first )
                                    urlQuery.addQueryItem( item.first, item.second );
                                else
                                {
                                    urlQuery.addQueryItem( item.first, QUrl::toPercentEncoding( offsetTimestamp ) );
                                    found = true;
                                }
                            }

                            if ( !found )
                                urlQuery.addQueryItem( offsetTimestampQuery, QUrl::toPercentEncoding( offsetTimestamp ) );

                            url.setQuery( urlQuery );
                        }

                        // setup request
                        fetchAllProductsRequest linkedRequest;
                        linkedRequest.setBusinessUnitType( businessUnitType_.toInt() );
                        linkedRequest.setLastFetchProductStamp( offsetTimestamp );
                        linkedRequest.setProductCount( productCount() );

                        LOG_DEBUG << "queue fetch all products request";
                        success = queueRequest( url, linkedRequest.toJsonDoc(), uuid );
                    }

                    //
                    // FIRMWARE
                    // FEATURE
                    // LABELS
                    //
                    else if (( UPGRADE_FIRMWARE == type ) ||
                             ( INSTALL_FEATURE == type ) ||
                             ( LOAD_LABELS == type ))
                    {
                        LOG_TRACE << "process metadata files...";
                        const EventState metaState( processMetadata( uuid, obj ) );

                        LOG_DEBUG << qPrintable( uuid ) << " metadata state " << metaState;

                        // we should be pending a download at this point
                        success = (STATE_PENDING == metaState);

                        if ( !success )
                        {
                            QString errorText;

                            if ( STATE_NONE == metaState )
                                errorText = "No metadata to process.";
                            else if ( STATE_FAILED == metaState )
                                errorText = "Failed to process metadata.";
                            else
                                errorText = "Unknown error occured.";

                            if ( !setRequestState( uuid, STATE_FAILED, errorText ) )
                                LOG_WARN << "error setting state to failed for " << qPrintable( uuid );
                        }
                    }

                    // pending
                    if (( success ) && ( !setRequestState( uuid, STATE_PENDING ) ))
                        LOG_WARN << "error setting state to pending for " << qPrintable( uuid );
                }
            }

            else
            {
                bool success( false );

                // request failed
                if ( STATE_FAILED == state )
                    LOG_WARN << qPrintable( uuid ) << " FAILED!";

                // request complete
                else if ( STATE_COMPLETE == state )
                {
                    LOG_DEBUG << qPrintable( uuid ) << " is complete";

                    // check metadata state:
                    // NONE = there is no metadata attached to this state
                    // PENDING = we are pending a download for this state
                    // COMPLETE = we have downloaded all files attached to this metadata
                    // FAILED = problem with one (or more) files
                    const EventState metaState( processMetadata( uuid, obj, true ) );

                    if ( STATE_NONE == metaState )
                        success = true;
                    else
                    {
                        LOG_DEBUG << qPrintable( uuid ) << " metadata state " << metaState;

                        if ( STATE_COMPLETE == metaState )
                            success = true;
                        else
                        {
                            LOG_DEBUG << qPrintable( uuid ) << " not complete";
                            return;
                        }
                    }
                }

                else
                {
                    LOG_WARN << "unhandled state " << state;
                }

                // check callback url
                QUrl urlCallback;

                if ( !obj.contains( JSON_CALLBACK_URL ) )
                    LOG_WARN << "event callback url is missing";
                else if ( obj[JSON_CALLBACK_URL].isNull() )
                    LOG_DEBUG << "no callback required";
                else if ( !obj[JSON_CALLBACK_URL].isString() )
                    LOG_WARN << "event metadata callback url not a string";
                else
                {
                    urlCallback = obj[JSON_CALLBACK_URL].toString();
                }

                // perform callback
                if (( urlCallback.isEmpty() ) || ( !urlCallback.isValid() ))
                    LOG_DEBUG << "callback url empty or invalid";
                else
                {
                    QFile f( eventDir + QDir::separator() + (success ? COMPLETE : FAILED) );

                    if ( !f.open( QFile::ReadOnly ) )
                        LOG_WARN << "failed to open state " << qPrintable( f.fileName() );
                    else
                    {
                        QString errorText;
                        int br;

                        QDataStream is( &f );
                        is >> errorText;
                        is >> br;

                        if ( QDataStream::Ok != is.status() )
                            LOG_WARN << "failed to read state " << qPrintable( f.fileName() );
                        else if ( success )
                        {
                            callbackRequest request( uuid, type, callbackRequest::SUCCESS, errorText );

                            LOG_INFO << "queue success callback";
                            queueRequest( urlCallback, request.toJsonDoc() );
                        }
                        else if ( BACKOUT_NOT_ATTEMPTED == br )
                        {
                            callbackRequest request( uuid, type, callbackRequest::FAILURE_BACKOUT_NOT_ATTEMPTED, errorText );

                            LOG_INFO << "queue failure callback (backout not attempted)";
                            queueRequest( urlCallback, request.toJsonDoc() );
                        }
                        else
                        {
                            callbackRequest request( uuid, type, ((BACKOUT_SUCCESS == br) ? callbackRequest::FAILURE_BACKOUT_SUCCESS : callbackRequest::FAILURE_BACKOUT_FAILED), errorText );

                            LOG_INFO << "queue failure callback (backout " << br << ")";
                            queueRequest( urlCallback, request.toJsonDoc() );
                        }

                        f.close();
                    }
                }

                // done!
                if ( !setRequestState( uuid, STATE_PROCESSED ) )
                    LOG_WARN << "error setting state to processed for " << qPrintable( uuid );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::handleRequestState( const QString& uuid )
{
    const EventState state( requestState( uuid ) );

    // send request
    if ( STATE_NONE == state )
    {
        static const int CONVERT_SECS( 1000 );

        bool valid;

        unsigned int timeout;
        unsigned int maxAttempts;

        // timeout
        timeout = scaleConfig::instance()->value<int>( CONF_REQUEST_TIMEOUT, &valid );

        if ( valid )
            timeout *= CONVERT_SECS;
        else
        {
            LOG_DEBUG << "bad or missing timeout";
            timeout = qMax( 0, requestTimeout_ * CONVERT_SECS );
        }

        // max attempts
        maxAttempts = scaleConfig::instance()->value<int>( CONF_REQUEST_MAX_ATTEMPTS, &valid );

        if ( !valid )
        {
            LOG_DEBUG << "bad or missing max attempts";
            maxAttempts = qMax( 1, requestNumRetries_+1 );
        }

        // ---- //

        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + REQUEST_JSON_DOC );

        QFile f( filename );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "failed to open " << qPrintable( filename );
        else
        {
            QUrl url;
            QJsonDocument request;

            QDataStream is( &f );
            is >> url;
            is >> request;

            // send!
            if ( QDataStream::Ok == is.status() )
            {
                api_->send( url, request, timeout, maxAttempts );

                if ( !setRequestState( uuid, STATE_PENDING ) )
                    LOG_WARN << "error setting state to pending for " << qPrintable( uuid );
            }

            f.close();
        }
    }

    // waiting for reply
    else if ( STATE_PENDING == state )
        LOG_TRACE << qPrintable( uuid ) << " is pending";

    // request failed
    else if ( STATE_FAILED == state )
    {
        LOG_WARN << qPrintable( uuid ) << " FAILED!";

        // done!
        if ( !setRequestState( uuid, STATE_PROCESSED ) )
            LOG_WARN << "error setting state to processed for " << qPrintable( uuid );
    }

    // request complete
    else if ( STATE_COMPLETE == state )
    {
        QJsonDocument response;

        LOG_DEBUG << qPrintable( uuid ) << " is complete";

        // read response
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + RESPONSE_JSON_DOC );

        QFile f( filename );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "failed to open " << qPrintable( filename );
        else
        {
            QDataStream is( &f );
            is >> response;

            f.close();
        }

        // process response
        if ( !response.isObject() )
            LOG_WARN << "not an object";
        else
        {
            const QJsonObject obj( response.object() );
            const QString responseUuid( obj[JSON_RESPONSE_UUID].toString() );

            if ( responseUuid.isEmpty() )
                LOG_WARN << "invalid uuid";
            else if ( uuid != responseUuid )
                LOG_WARN << "uuid mismatch";
            else
            {
                LOG_INFO << "processing response " << qPrintable( responseUuid ) << "...";

                EventState stateNext( STATE_PROCESSED );

                BackoutResult br( BACKOUT_NOT_ATTEMPTED );
                QString errorText;

                //
                // EVENTS
                //
                if ( obj.contains( JSON_EVENTS ) )
                {
                    const QJsonArray events( obj[JSON_EVENTS].toArray() );
                    LOG_DEBUG << "processing " << events.size() << " events...";

                    foreach ( const QJsonValue& event, events )
                    {
                        if ( !event.isObject() )
                        {
                            LOG_WARN << "event not an object";
                            continue;
                        }

                        processEvent( event.toObject() );
                    }

                    LOG_TRACE << "done with events";
                }

                //
                // CONFIG
                //
                else if ( obj.contains( JSON_NEW_CONFIGURATION ) )
                {
                    LOG_INFO << "have new configuration...";
                    const bool valid( scaleConfig::instance()->setCurrentConfig( response ) );

                    LOG_INFO << "new config result " << valid;

                    if ( valid )
                        errorText = "Success.";
                    else
                    {
                        stateNext = STATE_FAILED;

                        // for this event backout is always successful
                        br = BACKOUT_SUCCESS;
                        errorText = "Failed to set configuration. See log for details.";
                    }
                }

                //
                // SETTINGS
                //
                else if ( obj.contains( JSON_NEW_SETTINGS ) )
                {
                    LOG_INFO << "have new settings...";

                    // TODO (phase 4)

                    // check reboot flag
                    if (( obj.contains( JSON_REBOOT_SCALE ) ) && ( obj[JSON_REBOOT_SCALE].isBool() ))
                    {
                        const bool needToReboot( obj[JSON_REBOOT_SCALE].toBool() );

                        LOG_INFO << "reboot scale " << needToReboot;

#if defined(Q_OS_LINUX)
                        if ( needToReboot )
                        {
                            // queue reboot for 5m from now
                            system( "shutdown -r +5" );
                        }
#endif
                    }

                    // unsupported for now
                    stateNext = STATE_FAILED;

                    br = BACKOUT_SUCCESS;
                    errorText = "Not implemented.";
                }

                //
                // PRODUCTS
                //
                else if (( obj.contains( JSON_PRODUCTS ) ) || ( obj.contains( JSON_COOL ) ) || ( obj.contains( JSON_DELETE_PRODUCTS )))
                {
                    const QJsonArray products( obj[JSON_PRODUCTS].toArray() );
                    const QJsonObject cool( obj[JSON_COOL].toObject() );
                    const QJsonArray deleteProducts( obj[JSON_DELETE_PRODUCTS].toArray() );

                    const QJsonArray coolPreText( cool[JSON_COOL_PRE_TEXT].toArray() );
                    const QJsonArray coolCountries( cool[JSON_COOL_COUNTRIES].toArray() );
                    const QJsonArray coolProductionSteps( cool[JSON_COOL_PROD_STEPS].toArray() );
                    const QJsonArray coolCategories( cool[JSON_COOL_CATS].toArray() );
                    const QJsonArray coolText( cool[JSON_COOL_TEXT].toArray() );

                    const bool shouldRemoveProducts( obj[JSON_DELETE_ALL_PRODUCTS].toBool() );

                    LOG_DEBUG << "processing " << deleteProducts.size() << " product deletions...";
                    LOG_DEBUG << "processing " << products.size() << " products (should remove " << shouldRemoveProducts << ")...";
                    LOG_DEBUG << "processing " << coolPreText.size() << " cool pre-text...";
                    LOG_DEBUG << "processing " << coolCountries.size() << " cool countries...";
                    LOG_DEBUG << "processing " << coolProductionSteps.size() << " cool production steps...";
                    LOG_DEBUG << "processing " << coolCategories.size() << " cool categories...";
                    LOG_DEBUG << "processing " << coolText.size() << " cool text...";

                    // update state
                    setProcessingProducts( true );

                    // delete all products
                    if ( shouldRemoveProducts )
                    {
                        LOG_TRACE << "delete all products...";
                        deleteAllProducts();
                    }

                    // delete specific products
                    const int totalProductsToDelete( deleteProducts.size() );
                    int productDeleteFailures( 0 );

                    if ( totalProductsToDelete )
                    {
                        LOG_TRACE << "delete specific products...";

                        foreach ( const QJsonValue& doomedProduct, deleteProducts )
                        {
                            if ( !doomedProduct.isDouble() )
                            {
                                LOG_WARN << "delete plu not an integer";
                                continue;
                            }

                            if ( !deleteProduct( doomedProduct.toInt() ) )
                                ++productDeleteFailures;
                        }
                    }

                    // ----
                    // COOL
                    // ----

                    const int totalCool( coolPreText.size() + coolCountries.size() + coolProductionSteps.size() + coolCategories.size() + coolText.size() );
                    int coolFailures( 0 );

                    // iterate and update each pre text
                    foreach ( const QJsonValue& preText, coolPreText )
                    {
                        if ( !preText.isObject() )
                        {
                            LOG_WARN << "pre-text not an object";
                            continue;
                        }

                        if ( !insertUpdateCoolPreText( preText.toObject() ) )
                            ++coolFailures;
                    }

                    // iterate and update each country
                    foreach ( const QJsonValue& country, coolCountries )
                    {
                        if ( !country.isObject() )
                        {
                            LOG_WARN << "country not an object";
                            continue;
                        }

                        if ( !insertUpdateCoolCountry( country.toObject() ) )
                            ++coolFailures;
                    }

                    // iterate and update each production step
                    foreach ( const QJsonValue& prodStep, coolProductionSteps )
                    {
                        if ( !prodStep.isObject() )
                        {
                            LOG_WARN << "prod step not an object";
                            continue;
                        }

                        if ( !insertUpdateCoolProdStep( prodStep.toObject() ) )
                            ++coolFailures;
                    }

                    // iterate and update each category
                    foreach ( const QJsonValue& category, coolCategories )
                    {
                        if ( !category.isObject() )
                        {
                            LOG_WARN << "category not an object";
                            continue;
                        }

                        if ( !insertUpdateCoolCategory( category.toObject() ) )
                            ++coolFailures;
                    }

                    // iterate and update each text
                    foreach ( const QJsonValue& text, coolText )
                    {
                        if ( !text.isObject() )
                        {
                            LOG_WARN << "text not an object";
                            continue;
                        }

                        if ( !insertUpdateCoolText( text.toObject() ) )
                            ++coolFailures;
                    }

                    // -------
                    // Product
                    // -------

                    const int totalProducts( products.size() );
                    int productFailures( 0 );

                    // iterate and update each product
                    foreach ( const QJsonValue& product, products )
                    {
                        if ( !product.isObject() )
                        {
                            LOG_WARN << "product not an object";
                            continue;
                        }

                        if ( !insertUpdateProduct( product.toObject() ) )
                            ++productFailures;
                    }

                    LOG_TRACE << "done with products";

                    // update state
                    if (( totalProducts ) && ( totalProducts <= productFailures ))
                    {
                        stateNext = STATE_FAILED;

                        br = BACKOUT_NOT_ATTEMPTED;
                        errorText = "Error applying products to database.";
                    }
                    else
                    {
                        errorText = "Success. " +
                                QString( "Deleted %0 of %1 products. " ).arg( totalProductsToDelete - productDeleteFailures ).arg( totalProductsToDelete ) +
                                QString( "Applied %0 of %1 cool updates. " ).arg( totalCool - coolFailures ).arg( totalCool ) +
                                QString( "Applied %0 of %1 product updates. " ).arg( totalProducts - productFailures ).arg( totalProducts );

                        // save off last success
                        setLastProductFetch( obj[JSON_NEW_FETCH_PLU_STAMP], obj[JSON_RESPONSE_TIMESTAMP] );
                    }

                    // update state
                    setProcessingProducts( false );
                }

                //
                // CALLBACK
                //
                else if ( obj.contains( JSON_ACKNOWLEDGEMENT ) )
                {
                    if ( !obj[JSON_ACKNOWLEDGEMENT].isBool() )
                        LOG_WARN << "callback ack bad type";
                    else
                    {
                        const bool ack( obj[JSON_ACKNOWLEDGEMENT].toBool() );

                        if ( ack )
                            LOG_INFO << "callback request " << qPrintable( responseUuid ) << " was acknowledged";
                        else
                        {
                            LOG_WARN << "callback request " << qPrintable( responseUuid ) << " was NOT acknowledged";
                        }
                    }
                }

                LOG_TRACE << "next state " << stateNext;
                LOG_TRACE << "error text " << qPrintable( errorText );
                LOG_TRACE << "backout result " << br;

                if ( !setRequestState( uuid, stateNext, errorText, br ) )
                    LOG_WARN << "error setting state for " << qPrintable( uuid );

                LOG_DEBUG << "processing response done";
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::processEvent( const QJsonObject& obj )
{
    const QString type( obj[JSON_EVENT].toString() );
    const QString uuid( obj[JSON_UUID].toString() );
    const QString url( obj[JSON_URL].toString() );

    LOG_INFO << qPrintable( type ) << " " << qPrintable( uuid );

    if (( type.isEmpty() ) || ( uuid.isEmpty() ) || ( url.isEmpty() ))
    {
        LOG_WARN << "bad event info";
        return;
    }
    else if ( !enabledEvents_.contains( type ) )
    {
        LOG_WARN << "unsupported event type " << qPrintable( type );
        return;
    }

    // check always enabled
    bool alwaysEnabled( false );

    if ( !obj.contains( JSON_ALWAYS_ENABLED ) )
        LOG_WARN << "event always enabled is missing";
    else if ( !obj[JSON_ALWAYS_ENABLED].isBool() )
        LOG_WARN << "event metadata always enabled not a bool";
    else
    {
        alwaysEnabled = obj[JSON_ALWAYS_ENABLED].toBool();
    }

    // check enabled
    if ( alwaysEnabled )
        LOG_DEBUG << "event always enabled";
    else
    {
        bool valid;

        const QString conf( enabledEvents_[type] );
        LOG_TRACE << "checking enablement " << qPrintable( conf );

        const bool enabled( scaleConfig::instance()->value<bool>( conf, &valid ) );

        if ( !valid )
            LOG_WARN << "missing event enablement " << qPrintable( conf );
        else if ( !enabled )
        {
            LOG_INFO << "event not enabled";
            return;
        }
    }

    LOG_DEBUG << "queue event";
    queueRequest( url, QJsonDocument( obj ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookBase::EventState walmartHookBase::processMetadata( const QString& uuid, const QJsonObject& obj, bool updateState )
{
    QUrl url( obj[JSON_URL].toString() );

    // check for metadata
    if (( !obj.contains( JSON_METADATA ) ) || ( !obj[JSON_METADATA].isObject() ))
    {
        LOG_DEBUG << qPrintable( uuid ) << " has no metadata";
        return STATE_NONE;
    }

    const QJsonObject metadata( obj[JSON_METADATA].toObject() );

    // check for files
    if ( !metadata.contains( JSON_FILES ) )
    {
        LOG_DEBUG << qPrintable( uuid ) << " metadata contains no files";
        return STATE_NONE;
    }
    else if ( !metadata[JSON_FILES].isArray() )
    {
        LOG_WARN << qPrintable( uuid ) << " metadata contains bad files";

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    const QJsonArray files( metadata[JSON_FILES].toArray() );

    if ( !files.size() )
    {
        LOG_WARN << qPrintable( uuid ) << " metadata contains empty files";

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    // parse file data
    QMap<int, QString> filenames;
    QMap<int, qint64> sizes;
    QMap<int, QString> checksums;

    int sequence( 0 );

    foreach ( const QJsonValue& fileVal, files )
    {
        if ( !fileVal.isObject() )
        {
            LOG_WARN << qPrintable( uuid ) << " metadata contains bad files";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
                LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

            return STATE_FAILED;
        }

        const QJsonObject file( fileVal.toObject() );

        if (( !file.contains( JSON_FILENAME ) ) || ( !file[JSON_FILENAME].isString() ) ||
            ( !file.contains( JSON_FILE_SIZE ) ) || ( !file[JSON_FILE_SIZE].isDouble() ) ||
            ( !file.contains( JSON_FILE_CHECKSUM ) ) || ( !file[JSON_FILE_CHECKSUM].isString() ) ||
            (( file.contains( JSON_FILE_SEQ_NO ) ) && ( !file[JSON_FILE_SEQ_NO].isDouble() )))
        {
            LOG_WARN << qPrintable( uuid ) << " bad metadata file object";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata file." ) ))
                LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

            return STATE_FAILED;
        }

        if ( file.contains( JSON_FILE_SEQ_NO ) )
            sequence = file[JSON_FILE_SEQ_NO].toInt();
        else
            ++sequence;

        filenames[sequence] = file[JSON_FILENAME].toString();
        sizes[sequence] = file[JSON_FILE_SIZE].toInt();
        checksums[sequence] = file[JSON_FILE_CHECKSUM].toString();
    }

    // check for bad sequence information
    for ( int i( 1 ); i <= filenames.size(); ++i )
        if ( !filenames.contains( i ) )
        {
            LOG_WARN << qPrintable( uuid ) << " bad sequencing";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata file sequence numbers." ) ))
                LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

            return STATE_FAILED;
        }

    // download missing files
    for ( int i( 1 ); i <= filenames.size(); ++i )
    {
        const QString filenameBase( filenames[i] );

        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + filenameBase );

        if ( !QFile::exists( filename ) )
        {
            LOG_DEBUG << qPrintable( uuid ) << " file " << qPrintable( filenameBase ) << " is missing...";

            // set file name query item
            static const QString fileNameQuery( "fileName" );

            QUrlQuery urlQuery( url.query() );
            bool found( false );

            const QList<urlQueryParams> queryItems( urlQuery.queryItems() );

            urlQuery.clear();

            foreach ( const urlQueryParams& item, queryItems )
            {
                if ( fileNameQuery != item.first )
                    urlQuery.addQueryItem( item.first, item.second );
                else
                {
                    urlQuery.addQueryItem( item.first, QUrl::toPercentEncoding( filenameBase ) );
                    found = true;
                }
            }

            if ( !found )
                urlQuery.addQueryItem( fileNameQuery, QUrl::toPercentEncoding( filenameBase ) );

            url.setQuery( urlQuery );

            // download!
            const downloadFileRequest request( filenameBase );

            api_->downloadFile( url, uuid, filenameBase, request.toJsonDoc() );

            if (( updateState ) && ( !setRequestState( uuid, STATE_PENDING ) ))
                LOG_WARN << "could not update state to pending for " << qPrintable( uuid );

            return STATE_PENDING;
        }
    }

    // check for combined package
    QString combineFilename;
    qint64 combineSize( -1 );
    QString combineChecksum;

    bool combine( false );

    if (( metadata.contains( JSON_PACKAGE_FILENAME ) ) ||
        ( metadata.contains( JSON_FIRMWARE_SIZE ) ) ||
        ( metadata.contains( JSON_CHECKSUM ) ))
    {
        if (( !metadata.contains( JSON_PACKAGE_FILENAME ) ) || ( !metadata[JSON_PACKAGE_FILENAME].isString() ) ||
            ( !metadata.contains( JSON_FIRMWARE_SIZE ) ) || ( !metadata[JSON_FIRMWARE_SIZE].isDouble() ) ||
            ( !metadata.contains( JSON_CHECKSUM ) ) || ( !metadata[JSON_CHECKSUM].isString() ))
        {
            LOG_WARN << qPrintable( uuid ) << " metadata is missing combined package file information";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
                LOG_WARN << "could not update state to failed for " << qPrintable( uuid );

            return STATE_FAILED;
        }

        combineFilename = metadata[JSON_PACKAGE_FILENAME].toString();
        combineSize = metadata[JSON_FIRMWARE_SIZE].toInt();
        combineChecksum = metadata[JSON_CHECKSUM].toString();

        LOG_INFO << "file parts downloaded... combined package is " << qPrintable( combineFilename );
        combine = true;
    }

    QTemporaryFile temp;
    QCryptographicHash tempChecksum( QCryptographicHash::Sha256 );

    if (( combine ) && ( !temp.open() ))
    {
        LOG_WARN << qPrintable( uuid ) << " error opening temp file";

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Error opening temporary file." ) ))
            LOG_WARN << "could not update state to failed for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    // validate each file
    for ( int i( 1 ); i <= filenames.size(); ++i )
    {
        const QString filenameBase( filenames[i] );

        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + filenameBase );

        QFile f( filename );
        QString errorText;

        if ( !f.open( QFile::ReadOnly ) )
        {
            LOG_WARN << qPrintable( uuid ) << " error opening part file " << qPrintable( filenameBase );
            errorText = "File '" + filenameBase + "' does not exist.";
        }

        // size
        else if ( f.size() != sizes[i] )
        {
            LOG_WARN << qPrintable( uuid ) << " size validation failed part file " << qPrintable( filenameBase );
            errorText = "File '" + filenameBase + "' size validation failed.";
            f.close();
        }

        // checksum
        else
        {
            static const qint64 bufferLength( 4 * 1024 * 1024 );

            QCryptographicHash checksum( QCryptographicHash::Sha256 );

            for ( qint64 progress( 0 ); progress < f.size(); progress += bufferLength )
            {
                const QByteArray buffer = f.read( bufferLength );

                checksum.addData( buffer );

                if ( combine )
                {
                    temp.write( buffer );
                    tempChecksum.addData( buffer );
                }
            }

            const QByteArray checksumResult( checksum.result() );

            if ( QString( checksumResult.toHex() ) != checksums[i] )
            {
                LOG_WARN << qPrintable( uuid ) << " checksum validation failed part file " << qPrintable( filenameBase );
                errorText = "File '" + filenameBase + "' checksum validation failed.";
            }

            f.close();
        }

        // remove file part
        if (( combine ) && ( !QFile::remove( filename ) ))
            LOG_WARN << qPrintable( uuid ) << " error removing part file " << qPrintable( filenameBase );

        if ( errorText.isEmpty() )
            continue;

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, errorText ) ))
            LOG_WARN << "could not update state to failed for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    // validate combined file
    if ( combine )
    {
        QString errorText;

        // size
        if ( combineSize != temp.size() )
        {
            LOG_WARN << qPrintable( uuid ) << " size validation failed combine file " << qPrintable( combineFilename );
            errorText = "File '" + combineFilename + "' size validation failed.";
        }

        // checksum
        else
        {
            const QByteArray tempChecksumResult( tempChecksum.result() );

            if ( QString( tempChecksumResult.toHex() ) != combineChecksum )
            {
                LOG_WARN << qPrintable( uuid ) << " checksum validation failed combine file " << qPrintable( combineFilename );
                errorText = "File '" + combineFilename + "' checksum validation failed.";
            }
        }

        temp.close();

        // move file
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
        const QString filename( eventDir + QDir::separator() + combineFilename );

        if ( temp.rename( filename ) )
            temp.setAutoRemove( false );
        else
        {
            LOG_WARN << qPrintable( uuid ) << " error renaming temp file " << qPrintable( temp.fileName() );
            errorText = "File '" + combineFilename + "' could not be saved.";
        }

        if ( errorText.length() )
        {
            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, errorText ) ))
                LOG_WARN << "could not update state to failed for " << qPrintable( uuid );

            return STATE_FAILED;
        }
    }

    // TODO (phase 4)
    // install firmware
    // install feature
    // update request state

    const QString type( obj[JSON_EVENT].toString() );

    EventState stateNext( STATE_FAILED );
    QString errorText( "Not implemented." );

    //
    // UPGRADE FIRMWARE
    //
    if ( UPGRADE_FIRMWARE == type )
    {
        // not supported
    }

    //
    // INSTALL FEATURE
    //
    else if ( INSTALL_FEATURE == type )
    {
        // not supported
    }

    //
    // LOAD LABELS
    //
    else if (( LOAD_LABELS == type ) && ( !combine ))
    {
#if defined(Q_OS_LINUX)
        int success( 0 );
        int totalFiles( 0 );

        // move each file to scale download folder
        for ( int i( 1 ); i <= filenames.size(); ++i )
        {
            const QString filenameBase( filenames[i] );

            const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );
            const QString filename( eventDir + QDir::separator() + filenameBase );

            const QString newFilename( SCALE_DOWNLOAD_DIR + QDir::separator() + filenameBase );

            if ( QFile::rename( filename, newFilename ) )
                ++success;
            else
            {
                LOG_WARN << "failed to move file " << qPrintable( filename ) << " " << qPrintable( newFilename );
            }

            ++totalFiles;
        }

        if (( !success ) && ( totalFiles ))
            errorText = QString( "Error applying %0 label files." ).arg( totalFiles );
        else
        {
            stateNext = STATE_COMPLETE;

            errorText = "Success. " +
                QString( "Applied %0 of %1 label files." ).arg( success ).arg( totalFiles );
        }

#elif defined(Q_OS_WIN32)
        stateNext = STATE_COMPLETE;
        errorText = "Success.";

#endif
    }

    else
    {
        LOG_WARN << "unknown event type " << qPrintable( type ) << " " << combine;
        errorText = "Unsupported event type.";
    }

    if (( updateState ) && ( !setRequestState( uuid, stateNext, errorText ) ))
        LOG_WARN << "could not update state to failed for " << qPrintable( uuid );

    return stateNext;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookBase::queueRequest( const QUrl& url, const QJsonDocument& request, const QString& uuidParent )
{
    const QJsonObject obj( request.object() );

    QString uuid;
    QString filename;

    if ( obj.contains( JSON_REQUEST_UUID ) )
    {
        uuid = obj[JSON_REQUEST_UUID].toString();
        filename = REQUEST_JSON_DOC;
    }
    else
    {
        uuid = obj[JSON_UUID].toString();
        filename = EVENT_JSON_DOC;
    }

    LOG_DEBUG << "creating request " << qPrintable( uuid );

    if ( uuid.isEmpty() )
        LOG_WARN << "invalid uuid";
    else
    {
        // save request into queue
        const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );

        QDir d;

        // create folder
        if ( d.exists( eventDir ) )
            LOG_WARN << "duplicate uuid " << qPrintable( uuid );
        else if ( !d.mkdir( eventDir ) )
            LOG_WARN << "error creating request dir " << qPrintable( eventDir );
        else
        {
            // write parent uuid information
            if ( uuidParent.length() )
            {
#if defined(Q_OS_LINUX)
                static const QString dotdot( ".." );

                if ( !QFile::link( dotdot + QDir::separator() + uuidParent, eventDir + QDir::separator() + PARENT_UUID ) )
                    LOG_WARN << "error creating parent " << qPrintable( eventDir );

#elif defined(Q_OS_WIN32)
                QFile f( eventDir + QDir::separator() + PARENT_UUID );

                if ( !f.open( QFile::WriteOnly ) )
                    LOG_WARN << "error creating parent " << qPrintable( eventDir );
                else
                {
                    QDataStream os( &f );
                    os << uuidParent;

                    if ( QDataStream::Ok != os.status() )
                        LOG_WARN << "error writing parent " << qPrintable( eventDir );

                    f.close();
                }
#endif
            }

            // write request
            QFile f( eventDir + QDir::separator() + filename );

            if ( !f.open( QFile::WriteOnly ) )
                LOG_WARN << "error creating request " << qPrintable( eventDir );
            else
            {
                QDataStream os( &f );
                os << url;
                os << request;

                const bool okay( QDataStream::Ok == os.status() );

                f.close();

                if ( !okay )
                    return false;
            }

            // success
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookBase::EventState walmartHookBase::requestState( const QString& uuid )
{
    const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );

    // the order of these if statements is important , more than one file could exist!!
    if ( QFile::exists( eventDir + QDir::separator() + PROCESSED ) )
        return STATE_PROCESSED;
    else if ( QFile::exists( eventDir + QDir::separator() + FAILED ) )
        return STATE_FAILED;
    else if ( QFile::exists( eventDir + QDir::separator() + COMPLETE ) )
        return STATE_COMPLETE;
    else if ( QFile::exists( eventDir + QDir::separator() + PENDING ) )
        return STATE_PENDING;

    return STATE_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookBase::setRequestState( const QString& uuid, EventState state, const QString& errorText, BackoutResult br )
{
    const QString eventDir( EVENTS_DIR + QDir::separator() + uuid );

    QString filename;

    if ( STATE_PENDING == state )
    {
        filename = PENDING;

        // when reverting from complete to pending, we need to remove this flag
        QFile::remove( eventDir + QDir::separator() + COMPLETE );
    }
    else if ( STATE_COMPLETE == state )
        filename = COMPLETE;
    else if ( STATE_FAILED == state )
        filename = FAILED;
    else if ( STATE_PROCESSED == state )
        filename = PROCESSED;
    else
    {
        LOG_WARN << "unknown state " << state;
        return false;
    }

    // update state
    QFile f( eventDir + QDir::separator() + filename );

    if ( !f.open( QFile::WriteOnly ) )
    {
        LOG_WARN << "failed to create state " << qPrintable( filename );
        return false;
    }

    bool result( true );

    if (( STATE_FAILED == state ) || ( STATE_PROCESSED == state ) || ( STATE_COMPLETE == state ))
    {
        // update state with results
        QDataStream os( &f );
        os << errorText;
        os << (int) br;

        if ( QDataStream::Ok != os.status() )
        {
            LOG_WARN << "failed to write state " << qPrintable( filename );
            result = false;
        }
    }

    f.close();

    // check for parent uuid
    const QString parentFilename( eventDir + QDir::separator() + PARENT_UUID );

    if (( STATE_PROCESSED == state ) && ( QFile::exists( parentFilename ) ))
    {
        // parent exists so we need to copy the result into the parent event
        // this will indicate the event is ready to be processed and it will use the result
        // from this request
        //
        // we will try to copy failed first... if that doesnt work (i.e. it doesnt exist) we
        // will copy the processed file to complete
        //
        // for linux machines parent is a link and we only need to copy using the link
        // for windows machines parent is a file we need to open and read the parent out of it

        QList<fileCopyInfo> filesToCopy;

#if defined(Q_OS_LINUX)
        filesToCopy.append( fileCopyInfo( eventDir + QDir::separator() + FAILED, parentFilename + QDir::separator() + FAILED ) );
        filesToCopy.append( fileCopyInfo( eventDir + QDir::separator() + PROCESSED, parentFilename + QDir::separator() + COMPLETE ) );

#elif defined(Q_OS_WIN32)
        QFile f( parentFilename );

        // read parent uuid
        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "unable to open linked uuid";
        else
        {
            QString uuidParent;

            QDataStream is( &f );
            is >> uuidParent;

            // set linked uuid state
            if (( QDataStream::Ok != is.status() ) || ( uuidParent.isEmpty() ))
                LOG_WARN << "failed to read parent uuid";
            else if ( uuid == uuidParent )
                LOG_WARN << "circular parent uuid detected";
            else
            {
                const QString parentEventDir( EVENTS_DIR + QDir::separator() + uuidParent );

                LOG_TRACE << "parent " << parentEventDir;

                filesToCopy.append( fileCopyInfo( eventDir + QDir::separator() + FAILED, parentEventDir + QDir::separator() + FAILED ) );
                filesToCopy.append( fileCopyInfo( eventDir + QDir::separator() + PROCESSED, parentEventDir + QDir::separator() + COMPLETE ) );
            }

            f.close();
        }

#endif
        bool success( false );

        foreach ( const fileCopyInfo& copyInfo, filesToCopy )
        {
            if ( !QFile::exists( copyInfo.first ) )
                LOG_TRACE << "file " << qPrintable( copyInfo.first ) << " does not exist";
            else if ( !QFile::copy( copyInfo.first, copyInfo.second ) )
                LOG_WARN << "failed to copy " << qPrintable( copyInfo.first ) << " to parent " << qPrintable( parentFilename );
            else
            {
                success = true;
                break;
            }
        }

        if ( !success )
            LOG_WARN << "one or more states failed to copy to parent " << qPrintable( parentFilename );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::cleanupRequests( bool startup )
{
    QStringList doomed;

    // iterate all events
    QDirIterator iter( EVENTS_DIR, QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( iter.hasNext() )
    {
        const QString eventDir( iter.next() );
        const QString uuid( iter.fileName() );

        const EventState state( requestState( uuid ) );

        if (( STATE_PROCESSED == state ) ||
            // remove all pending at startup, regardless of state
            // saw an issue with WM plu data that could crash after rebooting in middle of processing fetch plu event
            (( startup ) && ( true ))) /*(( STATE_NONE == state ) || ( STATE_PENDING == state ))))*/
        {
            LOG_DEBUG << qPrintable( uuid ) << " is doomed";
            doomed.append( eventDir );
        }
    }

    // cleanup
    foreach ( const QString& eventDir, doomed )
    {
        QDir d( eventDir );

        if ( d.removeRecursively() )
            LOG_DEBUG << "removed " << qPrintable( eventDir );
        else
        {
            LOG_WARN << "failed to remove " << qPrintable( eventDir );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartHookBase::lastProductFetch()
{
    // 7/1/2022
    // As per John at Walmart they just want us to give them back what they gave us originally.

    // check for raw data first
    if ( !QFile::exists( FETCH_PLU_OFFSET_TIMESTAMP_RAW ) )
        LOG_DEBUG << "raw offset timestamp does not exist";
    else
    {
        QFile f( FETCH_PLU_OFFSET_TIMESTAMP_RAW );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "failed to open raw offset timestamp";
        else
        {
            QString stamp;

            QDataStream is( &f );
            is >> stamp;

            f.close();

            if (( stamp.isEmpty() ) || ( stamp.isNull() ))
                LOG_WARN << "could not read raw offset timestamp";
            else
            {
                LOG_DEBUG << "raw offset timestamp " << qPrintable( stamp );
                return stamp;
            }
        }
    }

    // check for date time file
    if ( !QFile::exists( FETCH_PLU_OFFSET_TIMESTAMP ) )
        LOG_DEBUG << "offset timestamp does not exist";
    else
    {
        QFile f( FETCH_PLU_OFFSET_TIMESTAMP );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "failed to open offset timestamp";
        else
        {
            QDateTime stamp;

            QDataStream is( &f );
            is >> stamp;

            f.close();

            if ( !stamp.isValid() )
                LOG_WARN << "could not read offset timestamp";
            else
            {
                // compute string value
                const QString result( scaleConfig::dateTimeToString( stamp ) );

                LOG_DEBUG << "offset timestamp " << qPrintable( result );
                return result;
            }
        }
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookBase::setLastProductFetch( const QJsonValue& value, const QJsonValue& responseStamp )
{
    // determine timestamp of this success
    QDateTime stamp( QDateTime::currentDateTime() );

    // make list of dates to evaluate
    QStringList dates;

    if ( responseStamp.isString() )
        dates.push_back( responseStamp.toString() );

    if ( value.isString() )
        dates.push_back( value.toString() );

    // evaluate
    foreach ( const QString& valueStr, dates )
    {
        // 6/24/2022
        //
        // As per John Vinoth Kumar at Walmart, they are sending this in UTC format... so try and
        // convert to local time. For example "2022-06-24T14:09:33.079233Z[UTC]"

        // check for milliseconds
        Qt::DateFormat fmt( Qt::ISODate );

        if (( 19 < valueStr.length() ) && ( '.' == valueStr[19] ))
            fmt = Qt::ISODateWithMs;

        QDateTime dt( QDateTime::fromString( valueStr, fmt ) );

        if ( !dt.isValid() )
            LOG_WARN << "value could not be parsed";
        else
        {
            // check for UTC
            if (( valueStr.contains( "Z" ) ) || ( valueStr.contains( "UTC" ) ))
            {
                dt.setTimeSpec( Qt::UTC );

                dt = dt.toLocalTime();
            }

            stamp = dt;
        }
    }

    // save off raw value
    if ( value.isString() )
    {
        const QString valueStr( value.toString() );

        if ( valueStr.length() )
        {
            QFile f( FETCH_PLU_OFFSET_TIMESTAMP_RAW );

            if ( !f.open( QFile::WriteOnly ) )
                LOG_WARN << "failed to open offset timestamp";
            else
            {
                QDataStream os( &f );
                os << valueStr;

                LOG_DEBUG << "saved raw offset timestamp " << qPrintable( valueStr );

                f.close();
            }
        }
    }

    // save off last success
    QFile f( FETCH_PLU_OFFSET_TIMESTAMP );

    if ( !f.open( QFile::WriteOnly ) )
        LOG_WARN << "failed to open offset timestamp";
    else
    {
        QDataStream os( &f );
        os << stamp;

        LOG_DEBUG << "saved offset timestamp " << qPrintable( stamp.toString() );

        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int walmartHookBase::pendingRequests()
{
    unsigned int result( 0 );

    // iterate all events
    QDirIterator iter( EVENTS_DIR, QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( iter.hasNext() )
    {
        iter.next();
        ++result;
    }

    return result;
}
