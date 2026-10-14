/**
 * @file hookhtebase.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "hookhte.h"
#include "hookhtebase.h"
#include "apihte.h"
#include "callbackrequest.h"
#include "common.h"
#include "configrequest.h"
#include "downloadfilerequest.h"
#include "eventcontrollerrequest.h"
#include "fetchallproductsrequest.h"
#include "scaleconfig.h"
#include "scalesettings.h"
#include "settingsrequest.h"
#include "stringsconfig.h"
#include "stringsjson.h"
#include "htedb.h"

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

static const QString REQUEST_JSON_DOC( "request.json" );
static const QString RESPONSE_JSON_DOC( "response.json" );
static const QString EVENT_JSON_DOC( "event.json" );

static const QString PARENT_UUID( "parent" );

static const QString PENDING( "pending" );
static const QString COMPLETE( "complete" );
static const QString FAILED( "failed" );
static const QString PROCESSED( "processed" );

static const QString UPDATE_CONFIG( "SYNC_CONFIG" );
static const QString DOWNLOAD_FILE( "DOWNLOAD_FILE" );  // used internally
static const QString APPLY_SETTINGS( "APPLY_SETTINGS" );
static const QString FETCH_PLU( "FETCH_PLU" );
static const QString UPGRADE_FIRMWARE( "UPGRADE_FIRMWARE" );
static const QString INSTALL_FEATURE( "INSTALL_FEATURE" );
static const QString LOAD_LABELS( "LOAD_LABELS" );

///////////////////////////////////////////////////////////////////////////////////////////////////
hteHookBase::hteHookBase( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    api_( nullptr ),
    db_( nullptr ),
    htedb_( nullptr ),
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
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::WasOffline, CONF_HEARTBEAT_RECONNECT_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::ScalePowerUp, CONF_HEARTBEAT_STARTUP_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::ScalePowerDown, CONF_HEARTBEAT_SHUTDOWN_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::Interval, CONF_HEARTBEAT_INTERVAL_ENABLED ) );
    supportedTriggers.append( triggerTypeInfo( eventControllerRequest::Manual, QString() ) );

    foreach ( const triggerTypeInfo& trigger, supportedTriggers )
        triggerTypes_[eventControllerRequest::requestTypeToString( (eventControllerRequest::RequestType) trigger.first )] = trigger;

    // setup enabled events
    enabledEvents_[UPDATE_CONFIG] = CONF_UPDATE_CONFIG_ENABLED;
    enabledEvents_[DOWNLOAD_FILE] = CONF_DOWNLOAD_FILE_ENABLED;
    enabledEvents_[APPLY_SETTINGS] = CONF_APPLY_SETTINGS_ENABLED;
    enabledEvents_[FETCH_PLU] = CONF_FETCH_PLU_ENABLED;
    enabledEvents_[UPGRADE_FIRMWARE] = CONF_UPGRADE_FIRMWARE_ENABLED;
    enabledEvents_[INSTALL_FEATURE] = CONF_INSTALL_FEATURE_ENABLED;
    enabledEvents_[LOAD_LABELS] = CONF_LOAD_LABELS_ENABLED;

    // setup api
    api_ = new hteapi( this );

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
hteHookBase::~hteHookBase()
{
    LOG_DEBUG << "dtor";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHookBase::performHealthCheck( const QString& triggerType, bool force )
{
    QMutexLocker guard( &m_ );

    // retrieve url
    QString u( htedb_->isValid() ? htedb_->readConfig(hteDatabase::CFG_HEARTBEATURL) : "" );
    QString bu( htedb_->isValid() ? htedb_->readConfig(hteDatabase::CFG_HEARTBEATBACKUPURL) : "" );
    QString ep( htedb_->isValid() ? htedb_->readConfig(hteDatabase::CFG_HEARTBEATENDPOINT) : "" );
    LOG_DEBUG << "hteHookBase::performHealthCheck() url: " << qPrintable(u) << " backup: " << qPrintable(bu) << " endpoint: " << qPrintable(ep);

    // validate url and trigger type
    if ( u.isEmpty() || ep.isEmpty() )
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

    const QUrl url( u + ep );
    LOG_DEBUG << "hteHookBase::performHealthCheck() url: " << qPrintable(url.url());
    const QUrl backupUrl( (bu.isEmpty()) ? "" : (bu + ep) );
    LOG_DEBUG << "hteHookBase::performHealthCheck() backupUrl: " << qPrintable(backupUrl.url());

    // which request type
    LOG_DEBUG << "performHealthCheck() - triggerType: " << qPrintable( triggerType );
    const triggerTypeInfo info( triggerTypes_[triggerType] );
    const eventControllerRequest::RequestType rt( (eventControllerRequest::RequestType) info.first );
    //LOG_DEBUG << "performHealthCheck() - RequestType rt: " << qPrintable( rt );

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
        bool valid;
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

    // request object
    eventControllerRequest request( rt, this );
    request.setLastFetchProductStamp( lastProductFetch() );  // For Walmart, no affect to HTe

    //LOG_DEBUG << "queue health check: " <<  request.toJsonDoc().toVariant();
    //QJsonDocument theObj(request.toJsonDoc());
    //LOG_DEBUG << "hteHookBase::performHealthCheck(): " << qPrintable( theObj.toJson() );

    queueRequest( url, request.toJsonDoc(), QString(), backupUrl );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHookBase::onInitialize()
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
            LOG_DEBUG << "The initial configuration request is from the Walmart apphook.  We are not currently doing this for the HTe apphook.";
            //queueRequest( initUrl_, request.toJsonDoc() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHookBase::onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response )
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
        if ( hteapi::SUCCESS != status )
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
            // if we are getting responses from HTe api then we are online
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
void hteHookBase::onProcessFile( const QJsonDocument& request, int status, const QString& filenameCached, const QString& uuid, const QString& filename )
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
        if ( hteapi::SUCCESS != status )
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

            // if we are getting responses from HTe api then we are online
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
void hteHookBase::onProcessStateMachine()
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

        QString fname(eventDir + QDir::separator() + EVENT_JSON_DOC);
        LOG_TRACE << "event json filename: " << qPrintable( fname );
        fname = eventDir + QDir::separator() + REQUEST_JSON_DOC;
        LOG_TRACE << "request json filename: " << qPrintable( fname );

        if ( QFile::exists( eventDir + QDir::separator() + EVENT_JSON_DOC ) )
        {
            LOG_TRACE << "event json file exists, calling handleEventState() ";
            handleEventState( uuid );
        }
        else if ( QFile::exists( eventDir + QDir::separator() + REQUEST_JSON_DOC ) )
        {
            LOG_TRACE << "request json file exists, calling handleRequestState() ";
            handleRequestState( uuid );
        }
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
void hteHookBase::loadWebHeaders()
{
    QSettings s( WEB_HEADERS_FILE, QSettings::IniFormat );

    hteapi::headersMap headers;

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
void hteHookBase::handleEventState( const QString& uuid )
{
    const EventState state( requestState( uuid ) );

    LOG_DEBUG << "hteHookBase::handleEventState() - passed in uuid: " << qPrintable( uuid ) << "  state: " << state;

    // waiting for reply
    if ( STATE_PENDING == state )
        LOG_TRACE << qPrintable( uuid ) << " is pending";

    else
    {
        QUrl url, backupUrl;
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
            is >> backupUrl;
            is >> request;

            LOG_DEBUG << "hteHookBase::handleEventState() url: " << qPrintable(url.url());
            LOG_DEBUG << "hteHookBase::handleEventState() backupUrl: " << qPrintable(backupUrl.url());

            f.close();
        }

        if ( !request.isObject() )
            LOG_WARN << "not an object";
        else
        {
            const QJsonObject obj( request.object() );
            LOG_DEBUG << "hteHookBase::handleEventState - request: " << qPrintable( request.toJson() );

            const QString type( obj[JSON_EVENTTYPE].toString() );
            LOG_DEBUG << "hteHookBase::handleEventState - request object's type: " << qPrintable( type );

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
                        /* For HTe, don't queue another request as was done for Walmart
                        configRequest linkedRequest;

                        LOG_DEBUG << "queue config request";
                        success = queueRequest( url, linkedRequest.toJsonDoc(), uuid );
                        */

                        LOG_TRACE << "process files to download...";
                        const EventState downloadState( processHTeDownload( uuid, obj ) );

                        LOG_DEBUG << qPrintable( uuid ) << " download state " << downloadState;

                        // we should be pending a download at this point
                        success = (STATE_PENDING == downloadState);

                        if ( !success )
                        {
                            QString errorText;

                            if ( STATE_NONE == downloadState )
                                errorText = "No download files to process.";
                            else if ( STATE_FAILED == downloadState )
                                errorText = "Failed to process download.";
                            else
                                errorText = "Unknown download error occurred.";

                            if ( !setRequestState( uuid, STATE_FAILED, errorText ) )
                                LOG_WARN << "error setting state to failed for " << qPrintable( uuid );
                        }

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
                        const QDateTime last( lastProductFetch() );

                        // compute string value
                        QString offsetTimestamp( "null" );

                        if ( last.isValid() )
                            offsetTimestamp = scaleConfig::dateTimeToString( last );

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

                        // setup request
                        fetchAllProductsRequest linkedRequest;
                        linkedRequest.setLastFetchProductStamp( last );
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

                    EventState metaState( STATE_NONE );

                    if ( UPDATE_CONFIG == type )
                    {
                        LOG_TRACE << "check progress of files to download...";
                        metaState = processHTeDownload( uuid, obj, true );
                    }
                    else
                    {
                        // check metadata state:
                        // NONE = there is no metadata attached to this state
                        // PENDING = we are pending a download for this state
                        // COMPLETE = we have downloaded all files attached to this metadata
                        // FAILED = problem with one (or more) files
                        metaState = processMetadata( uuid, obj, true );
                    }

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
void hteHookBase::handleRequestState( const QString& uuid )
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
            QUrl url, backupUrl;
            QJsonDocument request;

            QDataStream is( &f );
            is >> url;
            is >> backupUrl;
            is >> request;

            LOG_DEBUG << "hteHookBase::handleRequestState() url: " << qPrintable(url.url());
            LOG_DEBUG << "hteHookBase::handleRequestState() backupUrl: " << qPrintable(backupUrl.url());

            // send!
            if ( QDataStream::Ok == is.status() )
            {
                api_->send( url, request, timeout, maxAttempts, backupUrl );

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


                QString uuidInResponse( obj[JSON_DEVICEUUID].toString() );
                if ( !uuidInResponse.isEmpty() )
                {
                    // If the uuid in the configuration (previously in /usr/share/hte-apphook/hte.config,
                    // but now in /usr/local/hobart/db/het/hteapphook*.db) is null or empty, then it's never been set before.
                    // The HTe server will assign one to us on the first heartbeat, so use that one from now on.
                    // Do not save it if it comes back from the server as a different value, this should be a warning condition.
                    //
                    hteDatabase *htedb ( hteHook::instance()->htedb() );
                    if (( htedb ) && ( htedb->isValid() ))
                    {
                        QString configUuid( htedb->readConfig( hteDatabase::CFG_UUID ) );
                        if ( configUuid.length() == 0 )
                            htedb->saveConfig( hteDatabase::CFG_UUID, uuidInResponse );
                        else if ( configUuid != uuidInResponse )
                            LOG_WARN << "uuid in response: " << qPrintable(uuidInResponse) << " does not match the current value in configuration: " << qPrintable(configUuid);
                    }
                }


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

                        const QJsonObject oneEvent( event.toObject() );
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
void hteHookBase::processEvent( const QJsonObject& obj )
{
    const QString type( obj[JSON_EVENTTYPE].toString() );
    const QString uuid( obj[JSON_UUID].toString() );

    QJsonObject myObj(obj);
    //const QString dlUrl( obj[JSON_DOWNLOAD_URL].toString() );

    QString dlUrl( htedb_->isValid() ? htedb_->readConfig(hteDatabase::CFG_HEARTBEATDOWNLOADURL) : "" );
    LOG_DEBUG << "hteHookBase::processEvent: looked up download URL (temporary): " << qPrintable( dlUrl );
    myObj[JSON_DOWNLOAD_URL] = dlUrl;

    LOG_INFO << qPrintable( type ) << " " << qPrintable( uuid );

    if (( type.isEmpty() ) || ( uuid.isEmpty() ) || ( dlUrl.isEmpty() ))
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
    queueRequest( dlUrl, QJsonDocument( myObj ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
hteHookBase::EventState hteHookBase::processHTeDownload( const QString& uuid, const QJsonObject& obj, bool updateState )
{
    bool valid( false );

    QString downloadUrlStr( htedb_->isValid() ? htedb_->readConfig(hteDatabase::CFG_HEARTBEATDOWNLOADURL) : "" );
    // validate url
    if ( downloadUrlStr.isEmpty() )
    {
        LOG_WARN << "configuration missing " << qPrintable( hteDatabase::CFG_HEARTBEATDOWNLOADURL );
        return STATE_NONE;
    }
    QUrl downloadUrl( downloadUrlStr );

    LOG_INFO << "hteHookBase::processHTeDownload() - uuid: " << qPrintable( uuid ) << "  download url: " << qPrintable( downloadUrlStr );
    QJsonDocument theObj(obj);
    LOG_DEBUG << "hteHookBase::processHTeDownload - file: " << qPrintable( theObj.toJson() );

    // check for files
    if ( !obj.contains( JSON_FILES ) )
    {
        LOG_DEBUG << qPrintable( uuid ) << " event contains no files";
        return STATE_NONE;
    }
    else if ( !obj[JSON_FILES].isArray() )
    {
        LOG_WARN << qPrintable( uuid ) << " event contains bad files";

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    const QJsonArray files( obj[JSON_FILES].toArray() );

    if ( !files.size() )
    {
        LOG_WARN << qPrintable( uuid ) << " event contains empty files";

        if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
            LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

        return STATE_FAILED;
    }

    /*
    // parse file data
    QMap<int, QString> filenames;
    QMap<int, QString> fileids;
    QMap<int, qint64> sizes;
    QMap<int, QString> checksums;
    QMap<int, QString> fileversions;

    int sequence( 0 );
    */

    QVector<QString> downloadFilenames[100];

    foreach ( const QJsonValue& fileVal, files )
    {
        if ( !fileVal.isObject() )
        {
            LOG_WARN << qPrintable( uuid ) << " event contains bad files";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata." ) ))
                LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

            return STATE_FAILED;
        }

        const QJsonObject file( fileVal.toObject() );

        QJsonDocument fdoc(file);
        LOG_DEBUG << "hteHookBase::processHTeDownload - file: " << qPrintable( fdoc.toJson() );

        if (( !file.contains( JSON_FILENAME ) ) || ( !file[JSON_FILENAME].isString() ) ||
            ( !file.contains( JSON_FILE_ID ) ) || ( !file[JSON_FILE_ID].isString() ) ||
            ( !file.contains( JSON_FILE_SIZE ) ) || ( !file[JSON_FILE_SIZE].isDouble() ) ||
            ( !file.contains( JSON_FILE_CHECKSUM ) ) || ( !file[JSON_FILE_CHECKSUM].isString() )
           )
        {
            LOG_WARN << qPrintable( uuid ) << " bad event file object";

            if (( updateState ) && ( !setRequestState( uuid, STATE_FAILED, "Malformed metadata file." ) ))
                LOG_WARN << "could not update state to failure for " << qPrintable( uuid );

            return STATE_FAILED;
        }

        /*
        if ( file.contains( JSON_FILE_SEQ_NO ) )
            sequence = file[JSON_FILE_SEQ_NO].toInt();
        else
	        ++sequence;

        filenames[sequence] = file[JSON_FILENAME].toString();
        fileids[sequence] = file[JSON_FILE_ID].toString();
        sizes[sequence] = file[JSON_FILE_SIZE].toInt();
        checksums[sequence] = file[JSON_FILE_CHECKSUM].toString();
        fileversions[sequence] = ( file.contains( JSON_FILE_VERSION ) ) ? file[JSON_FILE_VERSION].toString() : "";
        */


        // Initially assume we need to download this file.
        bool needToDownload = true;
        if ( htedb_ )
            needToDownload = htedb_->shouldDownloadFile( file );

        const QString filenameBase( file[JSON_FILENAME].toString() );
        const QString filename( EVENTS_DIR + QDir::separator() + uuid + QDir::separator() + filenameBase );

        if ( needToDownload )
            // Add file name to a list, to have a list of all files that are supposed to be downloaded.
            // We'll use this later to see if we actually received this file.
            downloadFilenames->append(filenameBase);

        if ( needToDownload && !QFile::exists( filename ) )
        {
            LOG_DEBUG << qPrintable( uuid ) << " file " << qPrintable( filenameBase ) << " is missing, will download it ...";

            // set file id query item
            static const QString fileNameQuery( "id" );

            QUrlQuery urlQuery( downloadUrl.query() );

            urlQuery.clear();

            urlQuery.addQueryItem( fileNameQuery, file[JSON_FILE_ID].toString() );

            downloadUrl.setQuery( urlQuery );

            // download!
            // example:  http://10.3.128.103:8080/hte-enterprise/filemgr/download?id=dacfe5bf-ce8d-4621-94e1-f49740530873
            const downloadFileRequest request( filenameBase );

            api_->downloadFile( downloadUrl, uuid, filenameBase, request.toJsonDoc() );

            if ( ( updateState ) && ( !setRequestState( uuid, STATE_PENDING ) ))
                LOG_WARN << "could not update state to pending for " << qPrintable( uuid );

            return STATE_PENDING;
        }
    }


    // check for combined package
    QString combineFilename;
    qint64 combineSize( -1 );
    QString combineChecksum;

    bool combine( false );
/*
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
*/


    //
    // Validate each file
    //
    foreach ( const QJsonValue& fileVal, files )
    {
        const QJsonObject file( fileVal.toObject() );

        const QString filenameBase( file[JSON_FILENAME].toString() );

        // Only check for the file if it is in the list of all files that are supposed to be downloaded
        if ( !downloadFilenames->contains(filenameBase) )
            continue;

        const QString filename( EVENTS_DIR + QDir::separator() + uuid + QDir::separator() + filenameBase );

        QFile f( filename );
        QString errorText;


        if ( !f.open( QFile::ReadOnly ) )
        {
            LOG_WARN << qPrintable( uuid ) << " error opening part file " << qPrintable( filenameBase );
            errorText = "File '" + filenameBase + "' does not exist.";
        }

        // size
        else if ( f.size() != file[JSON_FILE_SIZE].toInt() )
        {
            LOG_WARN << qPrintable( uuid ) << " size validation failed part file " << f.size() << "  " << qPrintable( filenameBase ) << file[JSON_FILE_SIZE].toInt();
            //errorText = "File '" + filenameBase + "' size validation failed.";
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

                /*
                if ( combine )
                {
                    temp.write( buffer );
                    tempChecksum.addData( buffer );
                }
                */
            }

            const QByteArray checksumResult( checksum.result() );
            LOG_DEBUG << "downloaded file's calculated checksum: " << qPrintable( checksumResult.toHex() );
            LOG_DEBUG << "downloaded file's checksum from server: " << qPrintable( file[JSON_FILE_CHECKSUM].toString() );

            if ( QString( checksumResult.toHex() ) != file[JSON_FILE_CHECKSUM].toString() )
            {
                LOG_WARN << qPrintable( uuid ) << " checksum validation failed part file " << qPrintable( filenameBase );
                //errorText = "File '" + filenameBase + "' checksum validation failed.";
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

/*
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
*/


    // TODO (phase 4)
    // install firmware
    // install feature
    // update request state

    const QString type( obj[JSON_EVENTTYPE].toString() );
    LOG_DEBUG << "download finished - type: " << qPrintable( type );

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
    // Move downloaded files to the 'download' directory so chobart_d will process them
    //
    else if (( UPDATE_CONFIG == type ) && ( !combine ))
    {
        int success( 0 );
        int totalFiles( 0 );

        // move each file to scale download folder
        foreach ( const QJsonValue& fileVal, files )
        {
            const QJsonObject file( fileVal.toObject() );

            const QString filenameBase( file[JSON_FILENAME].toString() );
            // Only check for, and move, the file if it is in the list of all files that are supposed to be downloaded
            if ( !downloadFilenames->contains(filenameBase) )
                continue;

            const QString filename( EVENTS_DIR + QDir::separator() + uuid + QDir::separator() + filenameBase );
            bool isFeatureFile( filename.contains("tgz") );
            QString newFilename( (isFeatureFile) ? SCALE_UPGRADE_DIR : SCALE_DOWNLOAD_DIR );
            newFilename.append(filenameBase);

            LOG_DEBUG << "moving downloaded file from: " << qPrintable( filename );
            LOG_DEBUG << "moving downloaded file to: " << qPrintable( newFilename );

            if ( isFeatureFile && QFile::exists(newFilename) )
            {
                LOG_DEBUG << "feature file already exists in " << qPrintable( newFilename ) << " directory; removing the existing one first";
                QFile::remove(newFilename);
            }

            if ( QFile::rename( filename, newFilename ) )
            {
                // Update the record (or create it if it's not there), with the data passed in the JSON object.
                // Also set the ListedInCurrentHeartbeat = 1, to indicate we received this filename in the heartbeat's response.
                if ( htedb_ )
                {
                    // The second parameter is true so we also update the 'mostrecentdownloadtime' and
                    // 'ListedInCurrentHeartbeat' in the record of the Downloads table.
                    htedb_->insertUpdateDownloadFile( file, true );
                }
                ++success;
            }
            else
            {
                LOG_WARN << "failed to move file " << qPrintable( filename ) << " to " << qPrintable( newFilename );
            }

            ++totalFiles;
        }

        if (( !success ) && ( totalFiles ))
            errorText = QString( "Error applying %0 downloaded files." ).arg( totalFiles );
        else
        {
            stateNext = STATE_COMPLETE;

            errorText = "Success. " + QString( "Applied %0 of %1 downloaded files." ).arg( success ).arg( totalFiles );
        }


        // After processing all files in the heatbeat's response filelist, whether downloaded or not,
        // remove any records for filenames that we didn't receive in the list.
        // Then set all records to be "not received", as a precursor for the next heartbeat
        // (and thus the filelist in it's response)
        if ( htedb_ )
        {
            htedb_->deleteAllDownloadsNotReceived();
            htedb_->markAllDownloadsReceivedFlag(false);
        }
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
hteHookBase::EventState hteHookBase::processMetadata( const QString& uuid, const QJsonObject& obj, bool updateState )
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
    QMap<int, QString> fileids;
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
            ( !file.contains( JSON_FILE_ID ) ) || ( !file[JSON_FILE_ID].isString() ) ||
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
        fileids[sequence] = file[JSON_FILE_ID].toString();
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

#else
//#elif defined(Q_OS_WIN32)
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
bool hteHookBase::queueRequest( const QUrl& url, const QJsonDocument& request, const QString& uuidParent, const QUrl& backupUrl )
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

#else
//#elif defined(Q_OS_WIN32)
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

            LOG_DEBUG << "queueRequest() - eventDir: " << qPrintable( eventDir );
            LOG_DEBUG << "queueRequest() - filename: " << qPrintable( filename );
            LOG_DEBUG << "queueRequest() - url: " << qPrintable( url.toString() );
            LOG_DEBUG << "queueRequest() - backupUrl: " << qPrintable( backupUrl.toString() );
            LOG_DEBUG << "queueRequest() - request: " << qPrintable( request.toJson() );

            LOG_DEBUG << "queueRequest() - events filename: " << qPrintable( (eventDir + QDir::separator() + filename) );
            // write request
            
            QFile f( eventDir + QDir::separator() + filename );

            if ( !f.open( QFile::WriteOnly ) )
                LOG_WARN << "error creating request " << qPrintable( eventDir );
            else
            {
                QDataStream os( &f );
                os << url;
                os << backupUrl;
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
hteHookBase::EventState hteHookBase::requestState( const QString& uuid )
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
bool hteHookBase::setRequestState( const QString& uuid, EventState state, const QString& errorText, BackoutResult br )
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

#else
//#elif defined(Q_OS_WIN32)
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
void hteHookBase::cleanupRequests( bool startup )
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
            (( startup ) && (( STATE_NONE == state ) || ( STATE_PENDING == state ))))
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
QDateTime hteHookBase::lastProductFetch()
{
    QDateTime result;

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
                LOG_DEBUG << "offset timestamp " << qPrintable( stamp.toString( Qt::ISODate ) );
                result = stamp;
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHookBase::setLastProductFetch( const QJsonValue& value, const QJsonValue& responseStamp )
{
    // determine timestamp of this success
    QDateTime stamp( QDateTime::currentDateTime() );

    if ( responseStamp.isString() )
    {
        const QDateTime dt( QDateTime::fromString( responseStamp.toString(), Qt::ISODate ) );

        if ( !dt.isValid() )
            LOG_WARN << "response timestamp could not be parsed";
        else
        {
            stamp = dt;
        }
    }

    if ( value.isString() )
    {
        const QDateTime dt( QDateTime::fromString( value.toString(), Qt::ISODate ) );

        if ( !dt.isValid() )
            LOG_WARN << "value could not be parsed";
        else
        {
            stamp = dt;
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
unsigned int hteHookBase::pendingRequests()
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
