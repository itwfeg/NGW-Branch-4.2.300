/**
 * @file hookedgify.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiedgify.h"
#include "common.h"
#include "hookedgify.h"
#include "networkaccess.h"
#include "networkcheckingthread.h"
#include "scaledb.h"

#include <QDateTime>
#include <QSettings>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/edgify-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

//#define USE_DEBUG_VALUES

const char *edgifyHook::version_ = "0.1.0";

QMutex edgifyHook::instanceMutex_;
edgifyHook *edgifyHook::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyHook::edgifyHook( QObject *parent ) :
    _Mybase( parent ),
    db_( nullptr ),
    serverAddress_( "127.0.0.1" ),
    serverPort_( 6000 ),
    net_( nullptr )
{
    LOG_DEBUG << "ctor";

    // setup net
    net_ = new networkCheckingThread( this );

    connect( net_, &networkCheckingThread::onlineStatusChanged, this, &_Myt::onOnlineStatusChanged );

    // read config
    readConfiguration();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyHook::~edgifyHook()
{
    LOG_DEBUG << "close database";
    closeDatabase();

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyHook *edgifyHook::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyHook::initialize()
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "==================================================";
    LOG_INFO << "edgify hook";
    LOG_INFO << "version: " << version_;
    LOG_INFO << "build date: " << __DATE__ << " " << __TIME__;
    LOG_INFO << "==================================================";

    // start net
    net_->start( QThread::IdlePriority );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::finalize()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::configSaved()
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "config saved";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::beforeOpenDatabase( const QString& dbname )
{
    QMutexLocker guard( &m_ );
    dbname_ = dbname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::openDatabase()
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "open database " << qPrintable( dbname_ );

    if ( dbname_.isEmpty() )
        LOG_WARN << "empty db name!";
    else
    {
        db_ = new scaleDatabase( dbname_, this );

        if ( !db_->isValid() )
            LOG_ERROR << "error opening database " << qPrintable( dbname_ );
        else if ( !db_->parent() )
        {
            // when database opened outside of parent thread an error will be thrown about
            // parenting and threads... here we try to fix the issue by moving database object
            // back to proper thread
            db_->moveToThread( thread() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::closeDatabase()
{
    QMutexLocker guard( &m_ );

    if ( db_ )
    {
        LOG_DEBUG << "close database...";

        delete db_;
        db_ = nullptr;
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyHook::beforePluRead( const QString& plu, const QString& deptNum )
{
    Q_UNUSED( plu );
    Q_UNUSED( deptNum );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyHook::beforePluTextRead( const QString& number, const QString& deptNum, int type )
{
    Q_UNUSED( number );
    Q_UNUSED( deptNum );
    Q_UNUSED( type );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyHook::beforePrint( const QString& plu, const QString& deptNum, bool isRemotePrint )
{
    // for remote printing, clear out curent plu and dept
    if ( isRemotePrint )
    {
        currentPlu_.clear();
        currentDeptNum_.clear();
    }

    // save plu and dept
    else
    {
        currentPlu_ = plu;
        currentDeptNum_ = deptNum;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    Q_UNUSED( numLabelsToPrint );
    Q_UNUSED( numLabelsPrinted );

    // only handle "real" product
    if ( !success )
        return;
    else if ( templateLabel )
        return;

    // update truth
    updateTruth( currentPlu_, "PrintedLabel" );

    // clear out curent plu and dept
    currentPlu_.clear();
    currentDeptNum_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::customAction( const QString& commandString, int &result, QString& resultExtended )
{
    QMutexLocker guard( &m_ );

    LOG_DEBUG << "perform custom action " << qPrintable( commandString );
    result = -1;

    // parse out command string parameters
    const QStringList actionParams( commandString.split( "|", KEEP_EMPTY_PARTS ) );

    if ( actionParams.size() <= 0 )
        LOG_WARN << "empty action";
    else
    {
        const QString action( actionParams[0] );

        LOG_DEBUG << "action " << qPrintable( action );

        if ( action.isEmpty() )
            LOG_WARN << "empty action";
        else
        {
            // edgify camera result
            if ( resultCommand_ == action )
            {
                LOG_INFO << "requesting edgify camera result...";
                predict();

                result = 0;
                resultExtended = "command queued";
            }

            // reset result
            else if ( resetResultCommand_ == action )
            {
                LOG_INFO << "resetting result";
                resetResult();

                result = 0;
            }

            // last label result
            else if ( labelLastResultCommand_ == action )
            {
                if ( actionParams.size() < 3 )
                    LOG_WARN << "bad number of params for update truth";
                else
                {
                    const QString label( actionParams[1] );
                    const QString source( actionParams[2] );

                    LOG_INFO << "update edgify truth label " << qPrintable( label ) << " source " << qPrintable( source );
                    updateTruth( label, source );

                    result = 0;
                    resultExtended = "command queued";
                }
            }

            // server settings
            else if ( serverSettingsCommand_ == action )
            {
                if ( actionParams.size() < 3 )
                    LOG_WARN << "bad number of params for server settings";
                else
                {
                    const QString server( actionParams[1] );
                    const QString port( actionParams[2] );

                    LOG_INFO << "update server " << qPrintable( server ) << " port " << qPrintable( port );
                    serverAddress_ = server;
                    serverPort_ = port.toUShort();

                    result = 0;
                }
            }
        }
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer )
{
    Q_UNUSED( device );
    Q_UNUSED( deviceEx );
    Q_UNUSED( kind );
    Q_UNUSED( swVer );
    Q_UNUSED( hwVer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::updateScaleState( int type, const QString& value )
{
    LOG_INFO << "scale state " << type << " " << qPrintable( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyHook::isReady() const
{
    QMutexLocker guard( &m_ );
    return (( db_ ) && ( db_->isValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::sendPredictResults( PredictResult result, const QString& image, const QStringList& items, const QStringList& probs )
{
    QString s;
    bool first;

    // build string and send
    s.append( "PredictResults" );
    s.append( "," );

    s.append( QString::number( result ) );
    s.append( "," );

    s.append( image );
    s.append( "," );

    s.append( QString::number( items.size() ) );
    s.append( "," );

    first = true;

    foreach ( const QString& item, items )
    {
        if ( !first )
            s.append( "|" );

        s.append( item );
        first = false;
    }

    s.append( "," );

    s.append( QString::number( probs.size() ) );
    s.append( "," );

    first = true;

    foreach ( const QString& prob, probs )
    {
        if ( !first )
            s.append( "|" );

        s.append( prob );
        first = false;
    }

    LOG_DEBUG << "sending hook request " << qPrintable( s );
    hook_request_to_custom_endpoint( serverAddress_, serverPort_, s, false, HOOK_REQUEST_DELAY_TIME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::readConfiguration()
{
    static const int CONVERT_SECS( 1000 );

    QMutexLocker guard( &m_ );

    QSettings internalConfigs( INTERNAL_CONFIG, QSettings::IniFormat );

    // ---- //

    hostAddress_ = internalConfigs.value( "HostAddress" ).toString();
    hostPort_ = internalConfigs.value( "HostPort" ).toInt();

    LOG_INFO << "edgify host address " << qPrintable( hostAddress_ );
    LOG_INFO << "edgify host port " << hostPort_;

    // ---- //

    predictionUrl_ = internalConfigs.value( "PredictionUrl" ).toString();

    if ( predictionUrl_.isEmpty() )
        LOG_WARN << "empty prediction url!";

    setGroundTruthUrl_ = internalConfigs.value( "SetGroundTruthUrl" ).toString();

    if ( setGroundTruthUrl_.isEmpty() )
        LOG_WARN << "empty set ground truth url!";

    requestTimeout_ = internalConfigs.value( "RequestTimeout" ).toInt();
    requestNumRetries_ = internalConfigs.value( "RequestNumRetries" ).toInt();

    LOG_INFO << "prediction url " << qPrintable( predictionUrl_ );
    LOG_INFO << "set ground truth url " << qPrintable( setGroundTruthUrl_ );

    LOG_INFO << "request timeout " << requestTimeout_;
    LOG_INFO << "request num retries " << requestNumRetries_;

    // ---- //

    const QString ignoreAllSslErrors = internalConfigs.value( "IgnoreAllSslErrors" ).toString();
    const QString ignoreSslHostNameMismatch = internalConfigs.value( "IgnoreSslHostNameMismatch" ).toString();
    const QString ignoreSslSelfSignedCert = internalConfigs.value( "IgnoreSslSelfSignedCert" ).toString();

    const QString numberOfRequestsSaved = internalConfigs.value( "NumberOfRequestsSaved" ).toString();

    if ( api_ )
    {
        networkAccess *access( qobject_cast<networkAccess*>( api_->networkAccessManager() ) );

        if ( "1" == ignoreAllSslErrors )
        {
            LOG_INFO << "ignore all ssl errors";
            access->setIgnoreAllSslErrors( true );
        }

        QList<QSslError> errors;

        if ( "1" == ignoreSslHostNameMismatch )
        {
            LOG_INFO << "ignore ssl host name mismatch";
            errors.append( QSslError::HostNameMismatch );
        }

        if ( "1" == ignoreSslSelfSignedCert )
        {
            LOG_INFO << "ignore ssl self signed certs";
            errors.append( QSslError::SelfSignedCertificate );
        }

        if ( errors.size() )
            access->setIgnoreSslErrors( errors );

        if ( numberOfRequestsSaved.length() )
        {
            LOG_INFO << "number of saved requests " << numberOfRequestsSaved.toUInt();
            api_->setSavedRequests( numberOfRequestsSaved.toUInt() );
        }
    }

    // ---- //

    const QString uartEnabled( internalConfigs.value( "CommEnabled" ).toString() );

    if ( "1" == uartEnabled )
    {
        uartFile_ = internalConfigs.value( "CommDevice" ).toString();
        uartBaudRate_ = internalConfigs.value( "CommBaudRate" ).toInt();

        LOG_INFO << "edgify uart enabled!";
        LOG_INFO << "uart device " << qPrintable( uartFile_ );
        LOG_INFO << "uart baud rate " << uartBaudRate_;

        // setup uart
        enableSerialComms();
    }

    // ---- //

    const QString connectionCheckInterval( internalConfigs.value( "ConnectionCheckInterval" ).toString() );

    if ( net_ )
    {
        if ( connectionCheckInterval.length() )
        {
            const int value( connectionCheckInterval.toInt() * CONVERT_SECS );

            LOG_INFO << "connection check interval " << value;
            net_->setCheckingInterval( value );
        }
    }

    // ---- //

    resultCommand_= internalConfigs.value( "ResultAction" ).toString();
    resetResultCommand_ = internalConfigs.value( "ResetResultAction" ).toString();
    labelLastResultCommand_= internalConfigs.value( "LabelLastResultAction" ).toString();
    serverSettingsCommand_= internalConfigs.value( "ServerSettingsAction" ).toString();

    if ( resultCommand_.isEmpty() )
        resultCommand_ = "Result";

    if ( resetResultCommand_.isEmpty() )
        resetResultCommand_ = "ResetResult";

    if ( labelLastResultCommand_.isEmpty() )
        labelLastResultCommand_ = "LabelLastResult";

    if ( serverSettingsCommand_.isEmpty() )
        serverSettingsCommand_ = "ServerSettings";

    LOG_INFO << "action " << qPrintable( resultCommand_ );
    LOG_INFO << "action " << qPrintable( resetResultCommand_ );
    LOG_INFO << "action " << qPrintable( labelLastResultCommand_ );
    LOG_INFO << "action " << qPrintable( serverSettingsCommand_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::updateLastOnlineTime()
{
    QMutexLocker guard( &m_ );
    net_->setLastOnlineTime( QDateTime::currentDateTime() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHook::onOnlineStatusChanged( bool online, int offlineAmount )
{
    Q_UNUSED( offlineAmount );

    QMutexLocker guard( &m_ );

    // nothing to do
    if ( !online )
    {
        LOG_DEBUG << "OFFLINE";
        return;
    }
}

