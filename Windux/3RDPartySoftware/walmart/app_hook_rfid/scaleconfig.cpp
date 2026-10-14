/**
 * @file scaleconfig.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleconfig.h"
#include "stringsconfig.h"
#include "stringsjson.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QProcess>
#include <QString>
#include <QTextStream>

#if defined(Q_OS_WIN32)
#include <Windows.h>
#endif

#if defined(Q_OS_LINUX)
static const QString CONFIG_FILE( "/usr/share/walmart-apphook/walmart.config" );
static const QString CONFIG_FILE_OLD( "/usr/share/walmart-apphook/walmart.config.old" );
#elif defined(Q_OS_WIN32)
static const QString CONFIG_FILE( "walmart.config" );
static const QString CONFIG_FILE_OLD( "walmart.config.old" );
#endif

QMutex scaleConfig::instanceMutex_;
scaleConfig *scaleConfig::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleConfig::scaleConfig( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    storeNum_( 0 )
{
    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // read flash
    readFlashInfo();

    // read config
    readConfiguration();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleConfig::~scaleConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleConfig::countryCode() const
{
    QMutexLocker guard( &m_ );
    return countryCode_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleConfig::timestamp() const
{
    QMutexLocker guard( &m_ );
    return dateTimeToString( configStamp_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleConfig::toJsonObject() const
{
    QMutexLocker guard( &m_ );

    // populate object with config values
    QJsonObject obj;

    if ( isInitialized() )
        obj = config_;
    else
    {
        LOG_WARN << "scale configuration not initialized";
    }

    return obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::isInitialized() const
{
    QMutexLocker guard( &m_ );

    if ( config_.isEmpty() )
    {
        LOG_DEBUG << "missing config";
        return false;
    }
    else if (( configStamp_.isNull() ) || ( !configStamp_.isValid() ))
    {
        LOG_DEBUG << "invalid timestamp";
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::setCurrentConfig( const QJsonDocument& value )
{
    QMutexLocker guard( &m_ );

    bool result( false );

    if ( !validateRequestInfo( value ) )
        LOG_WARN << "document invalid";
    else
    {
        const QJsonObject obj( value.object() );

        if ( !obj.contains( JSON_NEW_CONFIGURATION ) )
            LOG_WARN << "new configuration is missing";
        else if ( !obj[JSON_NEW_CONFIGURATION].isObject() )
            LOG_WARN << "new configuration is not an object";
        else
        {
            const QJsonObject newConfig( obj[JSON_NEW_CONFIGURATION].toObject() );

            if ( newConfig != config_ )
                result = writeConfiguration( newConfig, QDateTime::currentDateTime(), true );
            else
            {
                LOG_DEBUG << "configuration is same";
                result = true;
            }
        }
    }

    LOG_INFO << "done " << result;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int scaleConfig::storeNum() const
{
    QMutexLocker guard( &m_ );
    return storeNum_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleConfig::vendor() const
{
    QMutexLocker guard( &m_ );
    return vendor_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleConfig::createRequestInfo() const
{
    QMutexLocker guard( &m_ );

    QJsonObject scale;
    scale[JSON_SCALE_MODEL_TEXT] = fullModel();
    scale[JSON_SERIAL_NUMBER] = serialNum();

    if ( isInitialized() )
    {
        scale[JSON_SCALE_VENDOR] = vendor();
        scale[JSON_SCALE_MODEL] = model();
        scale[JSON_STORE_NUMBER] = (double) storeNum();
        scale[JSON_COUNTRY_CODE] = countryCode();
    }
    else
    {
        static const QJsonValue nullVal( QJsonValue::Null );

        scale[JSON_SCALE_VENDOR] = nullVal;
        scale[JSON_SCALE_MODEL] = nullVal;
        scale[JSON_STORE_NUMBER] = nullVal;
        scale[JSON_COUNTRY_CODE] = nullVal;
    }

    QJsonObject obj;
    obj[JSON_REQUEST_UUID] = uuid();
    obj[JSON_REQUEST_TIMESTAMP] = dateTimeToString( QDateTime::currentDateTime() );
    obj[JSON_SCALE] = scale;

    return obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::validateRequestInfo( const QJsonDocument& doc, const QString& requestUuid ) const
{
    QMutexLocker guard( &m_ );

    if ( !doc.isObject() )
    {
        LOG_WARN << "not an object";
        return false;
    }

    const QJsonObject obj( doc.object() );

    if (( !obj.contains( JSON_RESPONSE_UUID ) ) ||
        ( !obj[JSON_RESPONSE_UUID].isString() ) ||
        (( requestUuid.length() ) && ( requestUuid != obj[JSON_RESPONSE_UUID].toString() )))
    {
        LOG_WARN << "bad response uuid";
        return false;
    }

    if (( !obj.contains( JSON_RESPONSE_TIMESTAMP ) ) ||
        ( !obj[JSON_RESPONSE_TIMESTAMP].isString() ))
    {
        LOG_WARN << "bad response timestamp";
        return false;
    }

    if (( !obj.contains( JSON_SCALE ) ) ||
        ( !obj[JSON_SCALE].isObject() ))
    {
        LOG_WARN << "bad response scale missing";
        return false;
    }

    const QJsonObject scale( obj[JSON_SCALE].toObject() );

    if (( !scale.contains( JSON_SERIAL_NUMBER ) ) ||
        ( !scale[JSON_SERIAL_NUMBER].isString() ) ||
        ( serialNum() != scale[JSON_SERIAL_NUMBER].toString() ))
    {
        LOG_WARN << "bad serial number";
        return false;
    }

    if (( !scale.contains( JSON_STORE_NUMBER ) ) ||
        ( !scale[JSON_STORE_NUMBER].isDouble() ) ||
        (( isInitialized() ) && ( storeNum() != (unsigned int) scale[JSON_STORE_NUMBER].toInt() )))
    {
        LOG_WARN << "bad store number";
        return false;
    }

    if (( !scale.contains( JSON_COUNTRY_CODE ) ) ||
        ( !scale[JSON_COUNTRY_CODE].isString() ) ||
        (( isInitialized() ) && ( countryCode() != scale[JSON_COUNTRY_CODE].toString() )))
    {
        LOG_WARN << "bad country code";
        return false;
    }

    if (( !scale.contains( JSON_SCALE_VENDOR ) ) ||
        ( !scale[JSON_SCALE_VENDOR].isString() ) ||
        (( isInitialized() ) && ( vendor() != scale[JSON_SCALE_VENDOR].toString() )))
    {
        LOG_WARN << "bad scale vendor";
        return false;
    }

    if (( !scale.contains( JSON_SCALE_MODEL ) ) ||
        ( !scale[JSON_SCALE_MODEL].isString() ) ||
        (( isInitialized() ) && ( model() != scale[JSON_SCALE_MODEL].toString() )))
    {
        LOG_WARN << "bad scale model";
        return false;
    }

    if (( !scale.contains( JSON_SCALE_MODEL_TEXT ) ) ||
        ( !scale[JSON_SCALE_MODEL_TEXT].isString() ) ||
        ( fullModel() != scale[JSON_SCALE_MODEL_TEXT].toString() ))
    {
        LOG_WARN << "bad scale model text";
        return false;
    }

    LOG_TRACE << "success";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleConfig::dateTimeToString( const QDateTime& value )
{
    QDateTime dt( value );
    dt.setOffsetFromUtc( value.offsetFromUtc() );

    // check for msecs
    if ( 0 < dt.time().msec() )
        return dt.toString( Qt::ISODateWithMs );

    return dt.toString( Qt::ISODate );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleConfig *scaleConfig::instance()
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
bool scaleConfig::parseConfigFile( const QString& file, const QString& delim, stringMap& config )
{
    bool result( false );

    if ( !QFile::exists( file ) )
        LOG_WARN << "config file " << qPrintable( file ) << " does not exist";
    else
    {
        QFile f( file );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "could not open config file " << qPrintable( file );
        else
        {
            QTextStream s( &f );

            // parse all lines
            while ( !s.atEnd() )
            {
                const QString line( s.readLine() );

                LOG_TRACE << qPrintable( line );

                if ( !line.contains( delim ) )
                    continue;

                // split line into key/value pair
                const QStringList values( line.split( delim, KEEP_EMPTY_PARTS ) );

                if ( values.size() < 2 )
                    continue;

                QString value( values[1] );

                // value may contain '='... handle that here
                for ( int i( 2 ); i < values.size(); ++i )
                {
                    value.append( delim );
                    value.append( values[i] );
                }

                // save value
                QString key( values[0] );
                key = key.trimmed();

                LOG_TRACE << qPrintable( key ) << " " << qPrintable( value );
                config[key] = value;
            }

            f.close();

            result = true;
        }
    }

    LOG_DEBUG << "done " << result;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::readConfiguration()
{
    bool result( false );

    if ( !QFile::exists( CONFIG_FILE ) )
        LOG_WARN << "config file does not exist";
    else
    {
        QFile f( CONFIG_FILE );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "could not open config file";
        else
        {
            QJsonParseError e;

            const QJsonDocument doc( QJsonDocument::fromJson( f.readAll(), &e ) );
            const QFileInfo info( CONFIG_FILE );

            if ( QJsonParseError::NoError != e.error )
            {
                const QString s( e.errorString() );
                LOG_WARN << "error parsing config " << e.error << " (" << e.offset << ") " << qPrintable( s );
            }
            else if ( !doc.isObject() )
                LOG_WARN << "configuration is not an object";
            else if ( !writeConfiguration( doc.object(), info.lastModified(), false ) )
                LOG_WARN << "could not update configuration";
            else
            {
                LOG_TRACE << "read configuration success";
                result = true;
            }

            f.close();
        }
    }

    LOG_DEBUG << "done " << result;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::writeConfiguration( const QJsonObject& config, const QDateTime& configStamp, bool save )
{
    if ( !validate( config ) )
    {
        LOG_WARN << "invalid configuration";
        return false;
    }

    QDateTime dt( configStamp );
    bool saveError( false );

    if ( save )
    {
        // backup old config
        if (( QFile::exists( CONFIG_FILE_OLD ) ) && ( !QFile::remove( CONFIG_FILE_OLD ) ))
        {
            LOG_WARN << "could not remove backup configuration";
            return false;
        }

        if (( QFile::exists( CONFIG_FILE ) ) && ( !QFile::rename( CONFIG_FILE, CONFIG_FILE_OLD ) ))
        {
            LOG_WARN << "could not backup configuration";
            return false;
        }

        // write new configuration
        QFile f( CONFIG_FILE );

        if ( !f.open( QFile::WriteOnly ) )
        {
            LOG_WARN << "could not open config file";
            saveError = true;
        }
        else
        {
            const QJsonDocument doc( config );
            const QByteArray a( doc.toJson( QJsonDocument::Indented ) );

            if ( a.size() != f.write( a ) )
            {
                LOG_WARN << "could not write config file";
                saveError = true;
            }

            f.close();
        }

        // restore old config backup
        if ( saveError )
        {
            if (( QFile::exists( CONFIG_FILE ) ) && ( !QFile::remove( CONFIG_FILE ) ))
                LOG_ERROR << "could not remove corrupt configuration";

            if (( QFile::exists( CONFIG_FILE_OLD ) ) && ( !QFile::rename( CONFIG_FILE_OLD, CONFIG_FILE ) ))
                LOG_ERROR << "could not restore configuration";

            return false;
        }

        // retrieve file modified time
        const QFileInfo info( CONFIG_FILE );

        dt = info.lastModified();
    }

    // update and emit signal
    config_ = config;
    configStamp_ = dt;

    const QJsonObject scaleConfig( config[JSON_SCALE_CONFIG].toObject() );
    vendor_ = scaleConfig[JSON_SCALE_VENDOR].toString();
    model_ = scaleConfig[JSON_SCALE_MODEL].toString();

    const QJsonObject site( config[JSON_SITE].toObject() );
    storeNum_ = (unsigned int) site[JSON_STORE_NUMBER].toInt();
    countryCode_ = site[JSON_COUNTRY_CODE].toString();

    LOG_TRACE << "update cron jobs...";
    updateCronJobs();

    LOG_DEBUG << "emit config changed...";
    emit configChanged();

    LOG_DEBUG << "success";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleConfig::validate( const QJsonObject& value )
{
    if (( !value.contains( JSON_CONFIG ) ) ||
        ( !value[JSON_CONFIG].isObject() ))
    {
        LOG_WARN << "bad config";
        return false;
    }
    else
    {
        QJsonObject obj( value[JSON_CONFIG].toObject() );

        if (( !obj.contains( JSON_UUID ) ) ||
            ( !obj[JSON_UUID].isString() ))
        {
            LOG_WARN << "bad uuid";
            return false;
        }
    }

    if (( !value.contains( JSON_SCALE_CONFIG ) ) ||
        ( !value[JSON_SCALE_CONFIG].isObject() ))
    {
        LOG_WARN << "bad scale config";
        return false;
    }
    else
    {
        QJsonObject obj( value[JSON_SCALE_CONFIG].toObject() );

        if (( !obj.contains( JSON_SERIAL_NUMBER ) ) ||
            ( !obj[JSON_SERIAL_NUMBER].isString() ) ||
            ( serialNum() != obj[JSON_SERIAL_NUMBER].toString() ))
        {
            LOG_WARN << "bad serial number";
            return false;
        }

        if (( !obj.contains( JSON_SCALE_VENDOR ) ) ||
            ( !obj[JSON_SCALE_VENDOR].isString() ))
        {
            LOG_WARN << "bad vendor";
            return false;
        }

        if (( !obj.contains( JSON_SCALE_MODEL ) ) ||
            ( !obj[JSON_SCALE_MODEL].isString() ))
        {
            LOG_WARN << "bad model";
            return false;
        }

        // as per Walmart, they may give back "HTi" when actual model is longer (i.e. "HTi-7LS")...
        // only validate the beginning characters
        const QString m( fullModel() );

        if (( !obj.contains( JSON_SCALE_MODEL_TEXT ) ) ||
            ( !obj[JSON_SCALE_MODEL_TEXT].isString() ) ||
            ( !m.startsWith( obj[JSON_SCALE_MODEL_TEXT].toString() ) ))
        {
            LOG_WARN << "bad model text";
            return false;
        }
    }

    if (( !value.contains( JSON_SITE ) ) ||
        ( !value[JSON_SITE].isObject() ))
    {
        LOG_WARN << "bad site";
        return false;
    }
    else
    {
        QJsonObject obj( value[JSON_SITE].toObject() );

        if (( !obj.contains( JSON_STORE_NUMBER ) ) ||
            ( !obj[JSON_STORE_NUMBER].isDouble() ))
        {
            LOG_WARN << "bad store number";
            return false;
        }

        if (( !obj.contains( JSON_COUNTRY_CODE ) ) ||
            ( !obj[JSON_COUNTRY_CODE].isString() ))
        {
            LOG_WARN << "bad country code";
            return false;
        }
    }

    LOG_TRACE << "success";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleConfig::readFlashInfo()
{
#if defined(Q_OS_LINUX)
    static const QString filename( "/dev/mtd3" );
#elif defined(Q_OS_WIN32)
    static const QString filename( "mtd3" );
#endif

    QByteArray buffer;

    // load the file
    if ( !QFile::exists( filename ) )
    {
        LOG_WARN << "file did not exist";
        return;
    }

    QFile f( filename );

    if ( f.open( QFile::ReadOnly ) )
    {
        LOG_DEBUG << "opened flash file";

        buffer = f.readAll();
        f.close();
    }

    if ( !buffer.length() )
    {
        LOG_WARN << "nothing in the buffer";
        return;
    }

    // parse the file
    static const QString modelfrontstr( "<SStyle>" );
    static const QString modelendstr( "</SStyle>" );
    static const QString serialfrontstr( "<Serial>" );                       // <Serial>1234567</Serial>
    static const QString serialendstr( "</Serial>" );                        // 130     138    145    154

    QString str_data( buffer );

    int start_serial = str_data.indexOf(serialfrontstr);
    int end_serial   = str_data.indexOf(serialendstr);
    int serial_begin = start_serial + serialfrontstr.length();
    int serial_len   = end_serial - serial_begin;

    QString tmpSerialNum( buffer.mid(serial_begin  , serial_len) );
    LOG_INFO << "buffer tmpSerialNum " << qPrintable( tmpSerialNum );

    tmpSerialNum.remove( '-' );
    tmpSerialNum.remove( '\'' );
    tmpSerialNum.remove( '\"' );

    int start_model = str_data.indexOf(modelfrontstr);
    int end_model   = str_data.indexOf(modelendstr);
    int model_begin = start_model + modelfrontstr.length();
    int model_len   = end_model - model_begin;

    QString tmpModel( buffer.mid(model_begin  , model_len) );
    LOG_INFO << "buffer tmpModel " << qPrintable( tmpModel );

    tmpModel.remove( '\'' );
    tmpModel.remove( '\"' );

    // set parameters
    serialNum_ = tmpSerialNum;
    fullModel_ = tmpModel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleConfig::updateCronJobs() const
{
    bool valid;

    const QString quartzInterval( value<QString>( CONF_HEALTH_CHECK_INTERVAL, &valid ) );

    if ( !valid )
    {
        LOG_WARN << "configuration missing";
        return;
    }

    LOG_INFO << "health check interval " << qPrintable( quartzInterval );

    // 6/13/2022
    //
    // Walmart is not able to support UNIX cron formatting. Instead they are using a Java package
    // called Quartz CRON. See http://www.quartz-scheduler.org/documentation/quartz-2.3.0/tutorials/crontrigger.html
    //
    // They have asked us to convert their hourly job time into an hourly unix cron job time. For
    // example, they are sending us "30 6 * * * ? *" which indicates they want the job to fire off
    // every hour at the 6th minute and 30th second.
    //
    // Walmart (Johnvinothkumar Johnpeterkrishnan and Varun Mannuru) has made assurances to us
    // that this is the most complex cron format we will ever see.
    //
    // So here we pick apart what they sent us and convert to UNIX cron.

    static const QString star( "*" );

    const QStringList parts( quartzInterval.trimmed().split( ' ' ) );

    QString interval;

    // if they sent us 5 parts lets assume this is UNIX CRON, in case they ever decide to fix things
    if ( 5 == parts.size() )
        interval = quartzInterval;
    else if (( 5 < parts.size() ) && ( "?" == parts[5] ))
    {
        LOG_TRACE << "found quartz format";

        interval =
            parts[1] + " " + // minute 0-59
            parts[2] + " " + // hour 0-23
            parts[3] + " " + // day of month 1-31
            star     + " " + // month 1-12
            star;            // day of week 0-7 where 0 is Sunday
    }
    else
    {
        LOG_WARN << "bad CRON format " << qPrintable( quartzInterval );
        return;
    }

    LOG_INFO << "cron expression" << qPrintable( interval );

#if defined(Q_OS_LINUX)
    static const QString program( "python" );

    QStringList args;
    args.append( "/usr/share/walmart-apphook/healthCheckAddCronJob.py" );
    args.append( "-t" );
    args.append( interval );

    // check for seconds
    // when existing we need to add a "delay" to the cron job
    if ( 0 < parts[0].length() )
        if (( "0" != parts[0] ) && ( "*" != parts[0] ))
        {
            args.append( "-d" );
            args.append( parts[0] );
        }

    QProcess p;
    p.start( program, args );

    if ( !p.waitForStarted() )
        LOG_WARN << "cron process did not start!";
    else if ( !p.waitForFinished() )
        LOG_WARN << "cron process did not complete!";

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue scaleConfig::lookupValue( const QString& path, bool *okay ) const
{
    QJsonValue result;

    QJsonObject obj( config_ );
    QStringList paths( path.split( "/" ) );

    while ( paths.size() )
    {
        if ( !obj.contains( paths.first() ) )
        {
            if ( okay )
                *okay = false;

            break;
        }

        if ( 1 == paths.size() )
        {
            if ( okay )
                *okay = true;

            result = obj[paths.first()];
        }
        else if ( !obj[paths.first()].isObject() )
        {
            if ( okay )
                *okay = false;

            break;
        }
        else
        {
            obj = obj[paths.first()].toObject();
        }

        paths.pop_front();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleConfig::uuid( bool random )
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "uuidgen" );

    QStringList args;
    args.append( random ? "-r" : "-t" );

    QProcess p;
    p.start( program, args );

    if ( p.waitForFinished() )
    {
        const QByteArray a( p.readAllStandardOutput() );
        const QString val( QString::fromLatin1( a ).trimmed() );

        if ( 36 == val.length() )
            if (( '-' == val[8] ) && ( '-' == val[13] ) && ( '-' == val[18] ) && ( '-' == val[23] ))
                result = val;
    }

#elif defined(Q_OS_WIN32)
    Q_UNUSED( random );

    GUID id;
    UuidCreate( &id );

    RPC_CSTR str;
    UuidToStringA( &id, &str );
    result = (char*) str;

    RpcStringFreeA( &str );

#endif

    return result;
}
