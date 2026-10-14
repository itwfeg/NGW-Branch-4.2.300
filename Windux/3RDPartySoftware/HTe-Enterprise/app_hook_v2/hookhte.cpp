/**
 * @file hookhte.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apihte.h"
#include "common.h"
#include "errordialogmgr.h"
#include "hookhte.h"
#include "networkaccess.h"
#include "networkcheckingthread.h"
#include "scaleconfig.h"
#include "scaledb.h"
#include "htedb.h"
#include "scalefirmware.h"
#include "scaleperipherals.h"
#include "scalesettings.h"
#include "scaleusagetoday.h"
#include "stringsconfig.h"

#include <QDateTime>
#include <QSettings>
#include <QFile>

//#define USE_DEBUG_VALUES

const char *hteHook::version_ = "0.2.1";

QMutex hteHook::instanceMutex_;
hteHook *hteHook::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
hteHook::hteHook( QObject *parent ) :
    _Mybase( parent ),
    lockout_( false ),
    shutdownMessageDialogTime_( 0 ),
    shutdownMessageMaxWaitTime_( 0 ),
    net_( nullptr )
{
    LOG_DEBUG << "ctor";

    // setup net
    net_ = new networkCheckingThread( this );

    connect( net_, SIGNAL(onlineStatusChanged(bool,int)), SLOT(onOnlineStatusChanged(bool,int)) );

    // read config
    readConfiguration();

    // init dependants
    scalePeripherals::instance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteHook::~hteHook()
{
    LOG_DEBUG << "close database";
    closeDatabase();

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteHook *hteHook::instance()
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
void hteHook::destroy()
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
bool hteHook::initialize()
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "==================================================";
    LOG_INFO << "HTe customer hook";
    LOG_INFO << "version: " << version_;
    LOG_INFO << "build date: " << __DATE__ << " " << __TIME__;
    LOG_INFO << "==================================================";

    // start net
    net_->start( QThread::IdlePriority );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::finalize()
{
    LOG_DEBUG << "send shutdown";
    performHealthCheck( "SCALE_POWER_DOWN" );

    LOG_TRACE << "wait for shutdown to complete...";

    const QDateTime now( QDateTime::currentDateTime() );
    void *d( nullptr );

    do
    {
        const int elapsed( now.msecsTo( QDateTime::currentDateTime() ) );

        LOG_TRACE << "elapsed " << elapsed;

        if (( shutdownMessageDialogTime_ ) && ( shutdownMessageDialogTime_ < elapsed ) && ( !d ))
        {
            LOG_INFO << "showing shutdown please wait dialog";
            errorDialogManager::instance()->showShutdownInProgress( &d );
        }

        if ( shutdownMessageMaxWaitTime_ < elapsed )
        {
            LOG_WARN << "shutdown processing took too long... aborting...";
            break;
        }

        LOG_TRACE << "waiting...";;
        QThread::msleep( 500 );

    } while ( pendingRequests() );

    if ( d )
    {
        LOG_TRACE << "terminate dialog";
        errorDialogManager::instance()->terminate( d );
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::beforeOpenDatabase( const QString& dbname )
{
    QMutexLocker guard( &m_ );
    dbname_ = dbname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::openDatabase()
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


        htedb_ = new hteDatabase( this );

        if ( !htedb_->isValid() )
            LOG_ERROR << "error opening HTe-scale database ";
        else if ( !htedb_->parent() )
        {
            // when database opened outside of parent thread an error will be thrown about
            // parenting and threads... here we try to fix the issue by moving database object
            // back to proper thread
            htedb_->moveToThread( thread() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteHook::isReady() const
{
    QMutexLocker guard( &m_ );
    return (( db_ ) && ( db_->isValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::closeDatabase()
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
void hteHook::customAction( const QString& commandString, int &result, QString& resultExtended )
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
            // single plu record load w/ delete (restore)
            // single plu record load (merge)
            if (( singlePluLoadCommand_ == action ) || ( singlePluLoadWithDeleteCommand_ == action ))
            {
                QString pluNum;
                QString deptNum;

                if ( 3 <= actionParams.size() )
                    deptNum = actionParams[2];
                if ( deptNum.isEmpty() )
                    deptNum = "0";

                if ( 2 <= actionParams.size() )
                    pluNum = actionParams[1];

                LOG_DEBUG << "plu " << qPrintable( pluNum ) << " deptNum " << qPrintable( deptNum );

                if ( pluNum.length() )
                {
                    // single plu retrieval was deprecated in P2/P3 specification
                    result = -1;
                    resultExtended = "deprecated";
                }
            }

            // health check
            else if ( healthCheckCommand_ == action )
            {
                // parse param
                QString param( "MANUAL" );

                if ( 2 <= actionParams.size() )
                    param = actionParams[1];

                LOG_DEBUG << "param " << qPrintable( param );

                // process
                performHealthCheck( param );

                result = 0;
                resultExtended = "command queued";
            }
        }
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer )
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "version info " << device << " (" << deviceEx << ")" <<
        " kind " << kind <<
        " sw " << qPrintable( swVer ) <<
        " hw " << qPrintable( hwVer );

    scalePeripherals::instance()->updatePeripheralList( device, deviceEx, kind, swVer, hwVer );

    if ( CUSTOMER_DISPLAY_DEVICE == device )
        scaleSettings::setCustomerDisplayExists( true );
    else if ( WEIGHER_DEVICE == device )
        scaleSettings::setWeigherExists( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::updateScaleState( int type, const QString& value )
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "scale state " << type << " " << qPrintable( value );

    if ( SCALE_BUILD_NUMBER == type )
        scaleFirmware::setBuildNumber( value );
    else if ( SCALE_MODE == type )
        scaleSettings::setScaleMode( value );
    else if ( SCALE_ITEM_ENTRY_TYPE == type )
        scaleSettings::setScaleItemEntryType( value );
    else if ( SCALE_LABEL_STOCK_1 == type )
        scaleSettings::setPrinterLabelStock( value );
    else if ( SCALE_WEIGHER_PRECISION == type )
        scaleSettings::setWeigherPrecision( value );
    else if ( SCALE_WEIGHER_CALIB_STAMP == type )
        scaleSettings::setWeigherCalibTimestamp( value );
    else if ( SCALE_WEIGHER_CONF_STAMP == type )
        scaleSettings::setWeigherConfigTimestamp( value );
    else if ( SCALE_ALL_FILES_DELETED == type )
        if ( htedb_ && htedb_->isValid() )
        {
            htedb()->markAllDownloadsReceivedFlag(false);
            htedb()->deleteAllDownloadsNotReceived();
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::configSaved()
{
    QMutexLocker guard( &m_ );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
hteHook::labelVersionMap hteHook::labelVersionInfo() const
{
    QMutexLocker guard( &m_ );

    labelVersionMap result;

    if (( db_ ) && ( db_->isValid() ))
    {
        int dept( 0 );

        // TODO
        // what department?

        result = db_->labelVersionInfo( dept );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime hteHook::lastComm() const
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->lastComm();

    return QDateTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime hteHook::lastItemChange() const
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->lastItemChange();

    return QDateTime();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int hteHook::productCount() const
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->activeProductCount();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::setProcessingProducts( bool value )
{
    if ( value )
    {
        int offlineTimerMax;
        int messageFadeTime;

        dialogStartStopTime dialog0;
        dialogStartStopTime dialog1;

        if ( !lookupOfflineDialogTimes( offlineTimerMax, dialog0, dialog1, messageFadeTime ) )
            LOG_WARN << "invalid or missing dialog times";

        else
        {
            LOG_TRACE << "showing load database dialog";
            errorDialogManager::instance()->showLoadingDatabase( messageFadeTime );
        }
    }
}

//#include <QFileInfo>
//#include <QDir>
///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::readConfiguration()
{
    static const int CONVERT_SECS( 1000 );

    QMutexLocker guard( &m_ );

    /*
    {
    // using where a relative filename will end up
    QFileInfo fi("temp");
    LOG_DEBUG << "QFileInfo fi('temp')  " << qPrintable( fi.absolutePath() );

    // explicitly using the relative name of the current working directory
    QDir dir(".");
    LOG_DEBUG << "QDir dir('.').absolutePath()  " << qPrintable( dir.absolutePath() );

    LOG_DEBUG << "QDir::currentPath()  " << qPrintable( QDir::currentPath() );
    }

    QDir::setCurrent( HOBART_LOCAL_DIR );


    // using where a relative filename will end up
    {
    QFileInfo fi("temp");
    LOG_DEBUG << "QFileInfo fi2('temp')  " << qPrintable( fi.absolutePath() );

    // explicitly using the relative name of the current working directory
    QDir dir(".");
    LOG_DEBUG << "QDir dir('.').absolutePath()  " << qPrintable( dir.absolutePath() );

    LOG_DEBUG << "QDir::currentPath()  " << qPrintable( QDir::currentPath() );
    }
    */


    LOG_DEBUG << "INTERNAL_CONFIG  " << qPrintable( INTERNAL_CONFIG );
    if ( !QFile::exists( INTERNAL_CONFIG ) )
        LOG_DEBUG << "INTERNAL_CONFIG file " << qPrintable( INTERNAL_CONFIG ) << " is missing...";


    QSettings internalConfigs( INTERNAL_CONFIG, QSettings::IniFormat );

    // ---- //

    initUrl_ = internalConfigs.value( "InitUrl" ).toString();
    LOG_DEBUG << "initUrl_  " << qPrintable( initUrl_ );


    if ( initUrl_.isEmpty() )
        LOG_WARN << "empty init url!";

    initScaleVendor_ = internalConfigs.value( "InitScaleVendor" ).toString();

    if ( initScaleVendor_.isEmpty() )
    {
        LOG_WARN << "empty init scale vendor!";
        initScaleVendor_ = "HOBART";
    }

    initScaleModel_ = internalConfigs.value( "InitScaleModel" ).toString();

    if ( initScaleModel_.isEmpty() )
    {
        LOG_WARN << "empty init scale model!";
        initScaleModel_ = "HTI";
    }

    requestTimeout_ = internalConfigs.value( "RequestTimeout" ).toInt();
    requestNumRetries_ = internalConfigs.value( "RequestNumRetries" ).toInt();

    const QString shutdownMessageDialogTime = internalConfigs.value( "ShutdownEventDialogTime" ).toString();
    const QString shutdownMessageMaxWaitTime = internalConfigs.value( "ShutdownEventMaxWaitTime" ).toString();

    if ( !shutdownMessageDialogTime.isEmpty() )
        shutdownMessageDialogTime_ = shutdownMessageDialogTime.toInt() * CONVERT_SECS;
    else
    {
        LOG_WARN << "empty shutdown dialog time!";
        shutdownMessageDialogTime_ = 0;
    }

    if ( !shutdownMessageMaxWaitTime.isEmpty() )
        shutdownMessageMaxWaitTime_ = shutdownMessageMaxWaitTime.toInt() * CONVERT_SECS;
    else
    {
        LOG_WARN << "empty shutdown max wait time!";
        shutdownMessageMaxWaitTime_ = DEFAULT_SHUTDOWN_MSG_MAX_WAIT;
    }

    LOG_INFO << "init url " << qPrintable( initUrl_ );

    LOG_INFO << "request timeout " << requestTimeout_;
    LOG_INFO << "request num retries " << requestNumRetries_;

    LOG_INFO << "shutdown message dialog time " << shutdownMessageDialogTime_;
    LOG_INFO << "shutdown message max wait time " << shutdownMessageMaxWaitTime_;

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
            const int value( connectionCheckInterval.toInt() * 1000 );

            LOG_INFO << "connection check interval " << value;
            net_->setCheckingInterval( value );
        }
    }

    // ---- //

    singlePluLoadCommand_ = internalConfigs.value( "SinglePluLoadCommand" ).toString();
    singlePluLoadWithDeleteCommand_ = internalConfigs.value( "SinglePluLoadWithDeleteCommand" ).toString();
    healthCheckCommand_ = internalConfigs.value( "HealthCheck" ).toString();

    if ( singlePluLoadCommand_.isEmpty() )
        singlePluLoadCommand_ = "MergeProductLoad";

    if ( singlePluLoadWithDeleteCommand_.isEmpty() )
        singlePluLoadWithDeleteCommand_ = "FullProductLoad";

    if ( healthCheckCommand_.isEmpty() )
        healthCheckCommand_ = "HealthCheck";

    LOG_INFO << "action " << qPrintable( singlePluLoadCommand_ );
    LOG_INFO << "action " << qPrintable( singlePluLoadWithDeleteCommand_ );
    LOG_INFO << "action " << qPrintable( healthCheckCommand_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::updateLastOnlineTime()
{
    QMutexLocker guard( &m_ );
    net_->setLastOnlineTime( QDateTime::currentDateTime() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteHook::onOnlineStatusChanged( bool online, int offlineAmount )
{
    QMutexLocker guard( &m_ );

    bool valid;

    // nothing to do
    if ( !online )
    {
        LOG_DEBUG << "OFFLINE";
        return;
    }

    // we are not in lockout when online
    lockout_ = false;

    // ensure configuration exists
    if ( !scaleConfig::instance()->isInitialized() )
    {
        LOG_WARN << "configuration missing";
        return;
    }

    const int reconnectOffset( scaleConfig::instance()->value<int>( CONF_HEARTBEAT_RECONNECT_OFFSET, &valid ) );

    if ( !valid )
    {
        LOG_WARN << "bad or missing reconnect offset";
        return;
    }

    LOG_TRACE << "reconnect offset " << reconnectOffset;

    if (( reconnectOffset ) && ( offlineAmount < reconnectOffset ))
    {
        LOG_DEBUG << "offline amount " << offlineAmount << " below offset";
        return;
    }

    performHealthCheck( "WAS_OFFLINE" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteHook::lookupOfflineDialogTimes( int& max, dialogStartStopTime& d0, dialogStartStopTime& d1, int& fade )
{
    static const int CONVERT_HOURS( 60 * 60 * 1000 );
    static const int CONVERT_SECS( 1000 );

#ifdef USE_DEBUG_VALUES
    // offline timer maximum
    max = 144 * CONVERT_HOURS;              // 144h

    // dialog values
    d0.first = 144 * CONVERT_HOURS;         // 144h
    d0.second = 120 * CONVERT_HOURS;        // 120h

    d1.first = 24 * CONVERT_HOURS;          // 24h
    d1.second = 0 * CONVERT_HOURS;          // 0h

    // message fade time
    fade = 5 * CONVERT_SECS;                // 5s

#else
    // list of each path
    QStringList paths;
    paths.append( CONF_OFFLINE_TIMER_MAX );
    paths.append( CONF_FIRST_PRINTING_MSG_START );
    paths.append( CONF_FIRST_PRINTING_MSG_STOP );
    paths.append( CONF_SECOND_PRINTING_MSG_START );
    paths.append( CONF_SECOND_PRINTING_MSG_STOP );
    paths.append( CONF_MESSAGE_FADE );

    // lookup each value
    bool valid;

    foreach ( const QString& path, paths )
    {
        const int value( scaleConfig::instance()->value<int>( path, &valid ) );

        if ( !valid )
        {
            LOG_WARN << "bad or missing value for path " << qPrintable( path );
            return false;
        }

        if ( CONF_OFFLINE_TIMER_MAX == path )
            max = value * CONVERT_HOURS;
        else if ( CONF_FIRST_PRINTING_MSG_START == path )
            d0.first = value * CONVERT_HOURS;
        else if ( CONF_FIRST_PRINTING_MSG_STOP == path )
            d0.second = value * CONVERT_HOURS;
        else if ( CONF_SECOND_PRINTING_MSG_START == path )
            d1.first = value * CONVERT_HOURS;
        else if ( CONF_SECOND_PRINTING_MSG_STOP == path )
            d1.second = value * CONVERT_HOURS;
        else if ( CONF_MESSAGE_FADE == path )
            fade = value * CONVERT_SECS;
    }

#endif

    return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::beforePluRead
/// \param plu
/// \param deptNum
/// \return
/// From WalmartAppHook
bool hteHook::beforePluRead( const QString& plu, const QString& deptNum )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "request plu " << qPrintable( plu ) << " deptNum " << qPrintable( deptNum );

    //
    // offline lockout dialog
    //

    const QDateTime lastSuccess( net_->lastOnlineTime() );
    const QDateTime now( QDateTime::currentDateTime() );

    int offlineTimerMax;
    int messageFadeTime;

    dialogStartStopTime dialog0;
    dialogStartStopTime dialog1;

    if ( !lookupOfflineDialogTimes( offlineTimerMax, dialog0, dialog1, messageFadeTime ) )
        LOG_WARN << "invalid or missing dialog times";
    else if ( net_->isOnline() )
        LOG_TRACE << "online";
    else if (( 0 < offlineTimerMax ) && ( lastSuccess.isValid() ) && ( lastSuccess <= now ))
    {
        // compute offline countdown timer
        const int offlineAmount( lastSuccess.msecsTo( now ) );

        if ( 0 < offlineAmount )
        {
            const int countdownTimer( offlineTimerMax - offlineAmount );
            const int countdownTimerHours( std::ceil( (double)(countdownTimer) / (double)(60 * 60 * 1000) ) );

            LOG_DEBUG << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";

            // AF-5.c
            // countdown at 0 hours
            if ( countdownTimer <= 0 )
            {
                if ( lockout_ )
                    LOG_DEBUG << "lockout";
                else
                {
                    LOG_WARN << "!!!! LOCKOUT !!!!";
                    LOG_WARN << "CONTACT FIELD SERVICES TO RAISE AN INCIDENT";
                    LOG_WARN << "scale offline since " << qPrintable( lastSuccess.toString() );
                    LOG_WARN << "scale offline for " << offlineAmount;
                    LOG_WARN << "initial countdown timer was " << offlineTimerMax;
                    LOG_WARN << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";
                    LOG_WARN << "REBOOT IN ORDER TO USE SCALE AGAIN";
                    LOG_WARN << "!!!! LOCKOUT !!!!";

                    errorDialogManager::instance()->showNotAvailable( lastSuccess );
                    lockout_ = true;
                }

                return false;
            }
        }
    }

    LOG_TRACE << "done";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::beforePluTextRead
/// \param number
/// \param deptNum
/// \param type
/// \return
/// From WalmartAppHook
bool hteHook::beforePluTextRead( const QString& number, const QString& deptNum, int type )
{
    Q_UNUSED( number );
    Q_UNUSED( deptNum );
    Q_UNUSED( type );

    QMutexLocker guard( &m_ );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::beforePrint
/// \param plu
/// \param deptNum
/// \param isRemotePrint
/// \return
/// From WalmartAppHook
bool hteHook::beforePrint( const QString& plu, const QString& deptNum, bool isRemotePrint )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "before print " << qPrintable( plu ) << " " << qPrintable( deptNum ) << " " << isRemotePrint;

    //
    // offline verify dialogs
    //

    const QDateTime lastSuccess( net_->lastOnlineTime() );
    const QDateTime now( QDateTime::currentDateTime() );

    int offlineTimerMax;
    int messageFadeTime;

    dialogStartStopTime dialog0;
    dialogStartStopTime dialog1;

    if ( !lookupOfflineDialogTimes( offlineTimerMax, dialog0, dialog1, messageFadeTime ) )
        LOG_WARN << "invalid or missing dialog times";
    else if ( net_->isOnline() )
        LOG_TRACE << "online";
    else if (( 0 < offlineTimerMax ) && ( lastSuccess.isValid() ) && ( lastSuccess <= now ))
    {
        // compute offline countdown timer
        const int offlineAmount( lastSuccess.msecsTo( now ) );

        if ( 0 < offlineAmount )
        {
            const int countdownTimer( offlineTimerMax - offlineAmount );
            const int countdownTimerHours( std::ceil( (double)(countdownTimer) / (double)(60 * 60 * 1000) ) );

            LOG_DEBUG << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";

            bool forceCheckOnline( false );

            // AF-5.c
            // countdown at 0 hours
            if ( countdownTimer <= 0 )
            {
                if ( lockout_ )
                    LOG_DEBUG << "lockout";
                else
                {
                    LOG_WARN << "!!!! LOCKOUT !!!!";
                    LOG_WARN << "CONTACT FIELD SERVICES TO RAISE AN INCIDENT";
                    LOG_WARN << "scale offline since " << qPrintable( lastSuccess.toString() );
                    LOG_WARN << "scale offline for " << offlineAmount;
                    LOG_WARN << "initial countdown timer was " << offlineTimerMax;
                    LOG_WARN << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";
                    LOG_WARN << "REBOOT IN ORDER TO USE SCALE AGAIN";
                    LOG_WARN << "!!!! LOCKOUT !!!!";

                    errorDialogManager::instance()->showNotAvailable( lastSuccess );
                    lockout_ = true;
                }

                LOG_DEBUG << "cancel printing locked out";
                return false;
            }

            // AF-5.a
            // coundown in first range (between 144 and 120 hours)
            else if (( dialog0.second < dialog0.first ) && ( 0 < dialog0.first ) &&
                     ( dialog0.second < countdownTimer ) && ( countdownTimer <= dialog0.first ))
            {
                LOG_INFO << "showing comm error dialog (144-120)";
                forceCheckOnline = errorDialogManager::instance()->showCommError( lastSuccess, countdownTimerHours );

                // operator chose cancel
                if ( !forceCheckOnline )
                {
                    LOG_DEBUG << "cancel printing (144-120)";
                    return false;
                }
            }

            // AF-5.b
            // coundown in second range (between 24 and 0 hours)
            else if (( dialog1.second < dialog1.first ) && ( 0 < dialog1.first ) &&
                     ( dialog1.second < countdownTimer ) && ( countdownTimer <= dialog1.first ))
            {
                LOG_INFO << "showing comm error dialog (24-0)";
                forceCheckOnline = errorDialogManager::instance()->showCommError( lastSuccess, countdownTimerHours );

                // operator chose cancel
                if ( !forceCheckOnline )
                {
                    LOG_DEBUG << "cancel printing (24-0)";
                    return false;
                }
            }

            // Basic Flow 12
            if (( forceCheckOnline ) && ( !net_->forceCheckOnlineStatus() ))
            {
                if ( messageFadeTime <= 0 )
                    LOG_WARN << "bad message fade time " << messageFadeTime;
                else
                {
                    LOG_INFO << "showing error still detected";
                    errorDialogManager::instance()->showErrorStillDetected( messageFadeTime );
                }
            }
        }

        LOG_TRACE << "printing...";
    }

    // keep track of items printed
    printingPlu_ = plu;
    printingDeptNum_ = deptNum;
    printingIsRemotePrint_ = isRemotePrint;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::afterPrint
/// \param success
/// \param numLabelsToPrint
/// \param numLabelsPrinted
/// \param templateLabel
/// From WalmartAppHook
void hteHook::afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "after print " << success << " " << numLabelsToPrint << " " << numLabelsPrinted << " " << templateLabel;

    // sanity check
    if ( printingPlu_.isEmpty() )
        return;

    if ( !success )
        LOG_DEBUG << "after print FAILURE";
    else
    {
        scaleUsageToday::PrintType type( scaleUsageToday::ManualPrint );

        if ( printingIsRemotePrint_ )
            type = scaleUsageToday::RemotePrint;

        // track items printed
        scaleUsageToday::instance()->updateUsage( printingPlu_.toInt(), numLabelsPrinted, type, templateLabel );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::deleteAllProducts
/// \return
/// From WalmartAppHook
bool hteHook::deleteAllProducts()
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
    {
        db_->deleteAllProductData();
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::deleteProduct
/// \param productNum
/// \return
/// From WalmartAppHook
bool hteHook::deleteProduct( int productNum )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->deleteProduct( productNum );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateCoolCategory
/// \param category
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateCoolCategory( const QJsonObject& category )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateCoolCategory( category );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateCoolCountry
/// \param country
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateCoolCountry( const QJsonObject& country )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateCoolCountry( country );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateCoolPreText
/// \param preText
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateCoolPreText( const QJsonObject& preText )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateCoolPreText( preText );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateCoolProdStep
/// \param prodStep
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateCoolProdStep( const QJsonObject& prodStep )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateCoolProdStep( prodStep );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateCoolText
/// \param text
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateCoolText( const QJsonObject& text )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateCoolText( text );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief hteHook::insertUpdateProduct
/// \param product
/// \return
/// From WalmartAppHook
bool hteHook::insertUpdateProduct( const QJsonObject& product )
{
    QMutexLocker guard( &m_ );

    if (( db_ ) && ( db_->isValid() ))
        return db_->insertUpdateProduct( product );

    return false;
}

