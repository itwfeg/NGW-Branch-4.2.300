/**
 * @file healthstatusthread.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiwalmart.h"
#include "healthstatusthread.h"
#include "customerhook.h"

#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegExp>
#include <QStorageInfo>
#include <QTextStream>
#include <QTimeZone>

#include <clio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
healthStatusThread::healthStatusThread( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    checkingInterval_( DEFAULT_CHECKING_INTERVAL ),
    numBadAcks_( 0 ),
    format_( "US" )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
healthStatusThread::~healthStatusThread()
{
    // stop thread
    if ( isRunning() )
    {
        LOG_DEBUG << "request thread termination";
        requestInterruption();

        LOG_DEBUG << "wait for termination...";

        do
        {
            QThread::msleep( 50 );

        } while ( isRunning() );

        LOG_TRACE << "wait for termination... complete";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int healthStatusThread::checkingInterval() const
{
    QMutexLocker guard( &m_ );
    return checkingInterval_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthStatusThread::format() const
{
    QMutexLocker guard( &m_ );
    return format_;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void healthStatusThread::setCheckingInterval( int value )
{
    QMutexLocker guard( &m_ );
    checkingInterval_ = value;

    LOG_INFO << "health status interval is " << checkingInterval_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthStatusThread::setFormat( const QString& value )
{
    QMutexLocker guard( &m_ );
    format_ = value;

    LOG_INFO << "health status format is " << format_.toStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthStatusThread::run()
{
    LOG_DEBUG << "health status thread running!";

    bool exit( false );

    // system needs time to retrieve database, ip address, etc...
    LOG_TRACE << "startup delay...";
    msleep( STARTUP_DELAY );

    LOG_TRACE << "startup delay... complete";

    do
    {
        LOG_INFO << "performing health status check";

        int nextCheckingTimeOverride = 0;

        {
            QMutexLocker guard( &m_ );

            LOG_INFO << "health status format is " << format_.toStdString();

            QJsonDocument info;
            QJsonDocument response;

            // TODO
            // fill out JSON document with requested information

            // SOF-3801
            const QString scaleMakeModel( customerHook::instance()->model() );

            LOG_INFO << "scale model " << scaleMakeModel.toStdString();

            // SOF-3802
            const QString ipAddr( customerHook::instance()->ipAddress() );
            const QString hardwareAddr( customerHook::instance()->macAddress() );
            const QString serialNo( customerHook::instance()->serialNum() );

            LOG_INFO << "ip address " << ipAddr.toStdString();
            LOG_INFO << "mac address " << hardwareAddr.toStdString();
            LOG_INFO << "serial no " << serialNo.toStdString();

            // SOF-3803
            const QString fimwareVersion( firmwareVersionInfo() );

            LOG_INFO << "firmware version " << fimwareVersion.toStdString();

            // SOF-3804
            const QString countryCode( customerHook::instance()->country() );
            const QString storeNo( customerHook::instance()->storeNum() );

            LOG_INFO << "country " << countryCode.toStdString();
            LOG_INFO << "store no " << storeNo.toStdString();

            // SOF-3805
            const int activeProducts( customerHook::instance()->activePluCount() );
            const int blockedProducts( whitelistPluCount() );

            // Note:
            // Originally Walmart wanted a whitelist for remote print items which is why the method and file referenced
            // is whitelist.  Later they decided really its a blacklist... allow all items to be remote printed unless
            // the PLU is contained in the file.  So its a blacklist and is what they mean by "blocked items".
            LOG_INFO << "active product count " << activeProducts;
            LOG_INFO << "blocked product count " << blockedProducts;

            // SOF-3806
            const customerHook::labelVersionMap labelInfo( customerHook::instance()->labelVersionInfo() );
            const QList<int> labels( labelInfo.keys() );

            foreach ( int labelNum, labels )
            {
                const customerHook::labelNameVersionPair nameVersion( labelInfo[labelNum] );

                LOG_INFO << "label " << labelNum << " (" <<
                    nameVersion.first.toStdString() << "): " <<
                    nameVersion.second.toStdString();
            }

            // SOF-3807
            uint64_t diskSize( 0 );
            uint64_t diskUsed( 0 );
            uint64_t diskFree( 0 );

            systemDiskInfo( diskSize, diskUsed, diskFree );

            LOG_INFO << "disk size " << diskSize;
            LOG_INFO << "disk used " << diskUsed;
            LOG_INFO << "disk free " << diskFree;

            // SOF-3808
            QString lastAPI;
            QString lastAPITime;

            lastUsageInfo( lastAPI, lastAPITime );

            LOG_INFO << "last api usage " << lastAPI.toStdString();
            LOG_INFO << "last api timestamp " << lastAPITime.toStdString();

            // SOF-3809
            // TODO

            // SOF-3810
            const QString currentDateTime( systemUTC() );
            const QString timezone( systemTimezone() );

            LOG_INFO << "utc time " << currentDateTime.toStdString();
            LOG_INFO << "timezone " << timezone.toStdString();


            // ---- //

            if ( "US" == format_ )
                ; // TODO
            else if ( "CA" == format_ )
            {
                QJsonObject obj;
                obj["bannerCode"] = customerHook::instance()->walmartCanadaBanner();

                if ( ipAddr.isEmpty() )
                    obj["ipAddress"] = ipAddress();
                else
                    obj["ipAddress"] = ipAddr;

                obj["itemCount"] = activeProducts;
                obj["scaleFirmware"] = fimwareVersion;
                obj["scaleMaker"] = customerHook::instance()->walmartCanadaMaker();
                obj["scaleModel"] = scaleMakeModel;
                obj["serialNumber"] = serialNo;

                info.setObject( obj );
            }

            // ---- //

            // send!
            if ( !customerHook::instance()->healthCheck( info, response ) )
                LOG_WARN << "health check failed";
            else
            {
                // TODO
                // process response and perform actions

                if ( "US" == format_ )
                    ; // TODO
                else if ( "CA" == format_ )
                {
                    if ( !response.isObject() )
                        LOG_WARN << "response not an object";
                    else
                    {
                        static const QString acknowledge( "acknowledge" );
                        static const QString firmwareUpdate( "flagFirmwareUpdate" );

                        const QJsonObject obj( response.object() );

                        bool retry = false;
                        bool firmware = false;

                        // check acknowledged
                        if ( !obj.contains( acknowledge ) )
                            LOG_WARN << "acknowledge missing";
                        else if ( !obj[acknowledge].isBool() )
                            LOG_WARN << "acknowledge wrong format";
                        else if ( obj[acknowledge].toBool() )
                        {
                            LOG_DEBUG << "good ack";
                            numBadAcks_ = 0;
                        }
                        else
                        {
                            LOG_INFO << "need to retry";
                            ++numBadAcks_;

                            if ( numBadAcks_ <= 3 )
                            {
                                retry = true;
                                nextCheckingTimeOverride = WM_CA_BAD_ACK_CHECKING_INTERVAL;

                                LOG_WARN << "bad ack " << numBadAcks_ << " retry in " << nextCheckingTimeOverride;
                            }
                            else
                            {
                                LOG_WARN << "bad ack count exceeded!";
                                numBadAcks_ = 0;
                            }
                        }

                        // check firmware
                        if ( !obj.contains( firmwareUpdate ) )
                            LOG_WARN << "firmware missing";
                        else if ( !obj[firmwareUpdate].isBool() )
                            LOG_WARN << "firmware wrong format";
                        else if ( obj[firmwareUpdate].toBool() )
                        {
                            LOG_INFO << "need firmware";
                            firmware = true;
                        }

                        LOG_DEBUG << "retry " << retry << " firmware " << firmware;
                    }
                } // walmart ca

            }
        }

        // ---- //

        LOG_TRACE << "sleeping until next checking time...";

        int nextCheckingTime( checkingInterval() );

        // check sooner if needed
        if (( nextCheckingTimeOverride ) && ( nextCheckingTimeOverride < nextCheckingTime ))
        {
            LOG_DEBUG << "using override time for next check " << nextCheckingTimeOverride;
            nextCheckingTime = nextCheckingTimeOverride;
        }

        do
        {
            if ( currentThread()->isInterruptionRequested() )
            {
                exit = true;
                break;
            }

            msleep( IDLE_TIME );

        } while ( 0 < (nextCheckingTime -= IDLE_TIME) );

    } while ( !exit );

    LOG_DEBUG << "health status thread exiting";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthStatusThread::systemUTC()
{
    const QDateTime now( QDateTime::currentDateTimeUtc() );
    return now.toString( Qt::ISODate );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthStatusThread::systemTimezone()
{
    const QTimeZone z( QTimeZone::systemTimeZone() );
    return z.displayName( QTimeZone::GenericTime );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthStatusThread::ipAddress()
{
    QString program( "python" );

    QStringList args;
    args.append( "getIpAddress.py" );

    QProcess p;
    p.setWorkingDirectory( "/usr/local/hobart/python" );

    LOG_TRACE << "getting ip address...";
    p.start( program, args );

    if ( !p.waitForFinished() )
        LOG_WARN << "get ip address process did not complete!";
    else
    {
        const QByteArray a( p.readAllStandardOutput() );

        const QString result( a );

        return result.trimmed();
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthStatusThread::systemDiskInfo( uint64_t& size, uint64_t& used, uint64_t& free )
{
    const QStorageInfo storage( QStorageInfo::root() );

    size = storage.bytesTotal();
    free = storage.bytesFree();

    if ( size < free )
        LOG_WARN << "invalid storage info";
    else
        used = size - free;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthStatusThread::lastUsageInfo( QString& api, QString& utcTime )
{
    QDateTime stamp;

    if ( !walmartapi::importLastUsage( stamp, api ) )
        LOG_WARN << "could not import last api";
    else
    {
        const QDateTime stampUTC( stamp.toUTC() );
        utcTime = stampUTC.toString( Qt::ISODate );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString healthStatusThread::firmwareVersionInfo()
{
    QString version;

#ifdef Q_OS_LINUX
    const QString svi( "/SOFTWARE_VERSION_INFO" );
#else
    const QString svi( "C:\\SOFTWARE_VERSION_INFO" );
#endif

    if ( !QFile::exists( svi ) )
        LOG_WARN << "could not find software version info";
    else
    {
        const QString RELEASE_ID( "Release ID: " );

        QFile f( svi );

        if ( !f.open( QIODevice::ReadWrite | QIODevice::Text ) )
            LOG_WARN << "error opening software version info";
        else
        {
            QTextStream ts( &f );

            while ( !ts.atEnd() )
            {
                const QString line( ts.readLine() );

                if ( line.startsWith( RELEASE_ID ) )
                {
                    version = line.mid( RELEASE_ID.length() );
                    break;
                }
            }

            if ( version.isEmpty() )
                LOG_WARN << "could not find release id";

            f.close();
        }
    }

    return version;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int healthStatusThread::whitelistPluCount()
{
    int result( 0 );

#ifdef Q_OS_LINUX
    const QString conf( "/usr/local/hobart/apps/remote_print/plu_whitelist.csv" );
#else
    const QString conf( "C:\\plu_whitelist.csv" );
#endif

    if ( !QFile::exists( conf ) )
        LOG_WARN << "could not find plu whitelist";
    else
    {
        QFile f( conf );

        if ( !f.open( QIODevice::ReadWrite | QIODevice::Text ) )
            LOG_WARN << "error opening whitelist";
        else
        {
            QTextStream ts( &f );

            while ( !ts.atEnd() )
            {
                const QString line( ts.readLine() );

                if ( line.isEmpty() )
                    continue;
                else if (( line.startsWith( "#" ) ) || ( line.startsWith( "/" ) ))
                    continue;
                else
                {
                    QRegExp re( "\\d*" );           // a digit (\d), zero or more times (*)

                    if ( re.exactMatch( line ) )
                        ++result;
                    else
                    {
                        LOG_TRACE << "bad data " << line.toStdString();
                    }
                }
            }

            f.close();
        }
    }

    return result;
}
