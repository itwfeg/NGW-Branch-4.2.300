/**
 * @file hooktiliter.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apitiliter.h"
#include "common.h"
#include "hooktiliter.h"
#include "networkaccess.h"
#include "networkcheckingthread.h"
#include "scaledb.h"

#include <QDateTime>
#include <QSettings>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/tiliter-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

//#define USE_DEBUG_VALUES

const char *tiliterHook::version_ = "0.1.0";

QMutex tiliterHook::instanceMutex_;
tiliterHook *tiliterHook::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
tiliterHook::tiliterHook( QObject *parent ) :
    _Mybase( parent ),
    db_( nullptr ),
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
tiliterHook::~tiliterHook()
{
    LOG_DEBUG << "close database";
    closeDatabase();

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tiliterHook *tiliterHook::instance()
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
void tiliterHook::destroy()
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
bool tiliterHook::initialize()
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "==================================================";
    LOG_INFO << "tiliter hook";
    LOG_INFO << "version: " << version_;
    LOG_INFO << "build date: " << __DATE__ << " " << __TIME__;
    LOG_INFO << "==================================================";

    // start net
    net_->start( QThread::IdlePriority );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::finalize()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::configSaved()
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "config saved";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::beforeOpenDatabase( const QString& dbname )
{
    QMutexLocker guard( &m_ );
    dbname_ = dbname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::openDatabase()
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
void tiliterHook::closeDatabase()
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
bool tiliterHook::beforePluRead( const QString& plu, const QString& deptNum )
{
    Q_UNUSED( plu );
    Q_UNUSED( deptNum );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tiliterHook::beforePluTextRead( const QString& number, const QString& deptNum, int type )
{
    Q_UNUSED( number );
    Q_UNUSED( deptNum );
    Q_UNUSED( type );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tiliterHook::beforePrint( const QString& plu, const QString& deptNum, bool isRemotePrint )
{
    Q_UNUSED( plu );
    Q_UNUSED( deptNum );
    Q_UNUSED( isRemotePrint );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    Q_UNUSED( success );
    Q_UNUSED( numLabelsToPrint );
    Q_UNUSED( numLabelsPrinted );
    Q_UNUSED( templateLabel );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::customAction( const QString& commandString, int &result, QString& resultExtended )
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
            // tiliter camera result
            if ( resultCommand_ == action )
            {
                LOG_INFO << "requesting tiliter camera result...";
                predict();

                result = 0;
                resultExtended = "command queued";
            }

            // last label result
            else if ( labelLastResultCommand_ == action )
            {
                LOG_INFO << "sending label last result...";

                // TODO
                result = -1;
                resultExtended = "not implemented";
            }
        }
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer )
{
    Q_UNUSED( device );
    Q_UNUSED( deviceEx );
    Q_UNUSED( kind );
    Q_UNUSED( swVer );
    Q_UNUSED( hwVer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::updateScaleState( int type, const QString& value )
{
    LOG_INFO << "scale state " << type << " " << qPrintable( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tiliterHook::isReady() const
{
    QMutexLocker guard( &m_ );
    return (( db_ ) && ( db_->isValid() ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::sendPredictResults( PredictResult result, const QStringList& items )
{
    QString s;
    bool first;

    // build string and send
    s.append( "PredictResults" );
    s.append( "," );

    s.append( QString::number( result ) );
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

    LOG_DEBUG << "sending hook request " << qPrintable( s );
    hook_request( s, false, HOOK_REQUEST_DELAY_TIME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::readConfiguration()
{
    static const int CONVERT_SECS( 1000 );

    QMutexLocker guard( &m_ );

    QSettings internalConfigs( INTERNAL_CONFIG, QSettings::IniFormat );

    // ---- //

    hostAddress_ = internalConfigs.value( "HostAddress" ).toString();
    hostPort_ = internalConfigs.value( "HostPort" ).toInt();

    LOG_INFO << "tiliter host address " << qPrintable( hostAddress_ );
    LOG_INFO << "tiliter host port " << hostPort_;

    // ---- //

    resultUrl_ = internalConfigs.value( "ResultUrl" ).toString();

    if ( resultUrl_.isEmpty() )
        LOG_WARN << "empty result url!";

    getWeightUrl_ = internalConfigs.value( "GetWeightUrl" ).toString();

    if ( getWeightUrl_.isEmpty() )
        LOG_WARN << "empty get weight url!";

    labelLastResultUrl_ = internalConfigs.value( "LabelLastResultUrl" ).toString();

    if ( labelLastResultUrl_.isEmpty() )
        LOG_WARN << "empty label last result url!";

    requestTimeout_ = internalConfigs.value( "RequestTimeout" ).toInt();
    requestNumRetries_ = internalConfigs.value( "RequestNumRetries" ).toInt();

    LOG_INFO << "result url " << qPrintable( resultUrl_ );
    LOG_INFO << "get weight url " << qPrintable( getWeightUrl_ );
    LOG_INFO << "label last result url " << qPrintable( labelLastResultUrl_ );

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
    getWeightCommand_= internalConfigs.value( "GetWeightAction" ).toString();
    labelLastResultCommand_= internalConfigs.value( "LabelLastResultAction" ).toString();

    if ( resultCommand_.isEmpty() )
        resultCommand_ = "Result";

    if ( getWeightCommand_.isEmpty() )
        getWeightCommand_ = "GetWeight";

    if ( labelLastResultCommand_.isEmpty() )
        labelLastResultCommand_ = "LabelLastResult";

    LOG_INFO << "action " << qPrintable( resultCommand_ );
    LOG_INFO << "action " << qPrintable( getWeightCommand_ );
    LOG_INFO << "action " << qPrintable( labelLastResultCommand_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::updateLastOnlineTime()
{
    QMutexLocker guard( &m_ );
    net_->setLastOnlineTime( QDateTime::currentDateTime() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHook::onOnlineStatusChanged( bool online, int offlineAmount )
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

