/**
 * @file eventcontrollerrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "eventcontrollerrequest.h"
#include "scaleblockedplus.h"
#include "scaleconfig.h"
#include "scaleinfo.h"
#include "scalefirmware.h"
#include "scaleperipherals.h"
#include "scalesettings.h"
#include "networksettings.h"
#include "scaleusagetoday.h"
#include "stringsconfig.h"
#include "stringsjson.h"
#include "scaledb.h"
#include "htedb.h"
#include "hookhte.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStorageInfo>
#include <QHostInfo>

#if defined(Q_OS_LINUX)
#include <sys/sysinfo.h>
#elif defined(Q_OS_WIN32)
#include <Windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
eventControllerRequest::eventControllerRequest( RequestType type, QObject *parent ) :
    _Mybase( parent ),
    type_( type)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eventControllerRequest::~eventControllerRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString eventControllerRequest::requestTypeToString( RequestType type )
{
    switch ( type )
    {
    case WasOffline:
        return "WAS_OFFLINE";
    case ScalePowerUp:
        return "SCALE_POWER_UP";
    case ScalePowerDown:
        return "SCALE_POWER_DOWN";
    case Interval:
        return "INTERVAL";
    case Manual:
        return "MANUAL";
    default:
        break;
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qint64 eventControllerRequest::uptime()
{
#if defined(Q_OS_LINUX)
    struct sysinfo s_info;

    if ( 0 != sysinfo( &s_info ) )
    {
        LOG_WARN << "error getting sysinfo";
        return 0;
    }

    return (1000 * s_info.uptime);

#elif defined(Q_OS_WIN32)
    return GetTickCount64();

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonArray eventControllerRequest::systemDiskInfo()
{
    const QList<QStorageInfo> volumes( QStorageInfo::mountedVolumes() );

    QJsonArray a;

    foreach ( const QStorageInfo& storage, volumes )
    {
        const qint64 size( storage.bytesTotal() );
        const qint64 free( storage.bytesFree() );

        QJsonObject obj;
        obj[JSON_DEVICE] = QString::fromLatin1( storage.device() );
        obj[JSON_DISPLAY_NAME] = storage.displayName();
        obj[JSON_FILE_SYSTEM_TYPE] = QString::fromLatin1( storage.fileSystemType() );

        obj[JSON_SIZE] = size;
        obj[JSON_FREE] = free;

        if ( size < free )
            LOG_WARN << "invalid storage info";
        else
            obj[JSON_USED] = size - free;

        a.append( obj );
    }

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument eventControllerRequest::toJsonDoc() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    bool sendingFullHeartbeat( type_ != Interval );

    QJsonObject obj( request_ );

    obj[JSON_TRIGGER_TYPE] = requestTypeToString( type_ );

    hteDatabase *htedb ( hteHook::instance()->htedb() );
    if (( htedb ) && ( htedb->isValid() ))
        obj[JSON_DEVICEUUID] = htedb->readConfig( hteDatabase::CFG_UUID );

    // retrieve ip address
    QString ipAddress4;
    QString ipAddress6;

    networkSettings::ipAddress( ipAddress4, ipAddress6 );

    if ( ipAddress4.length() )
        obj[JSON_IPADDRESS] = ipAddress4;
    else if ( ipAddress6.length() )
        obj[JSON_IPADDRESS] = ipAddress6;
    else
        obj[JSON_IPADDRESS] = nullVal;

    obj[JSON_HOSTNAME] = QHostInfo::localHostName();


    QJsonObject healthCheckObj;

    const QDateTime now( QDateTime::currentDateTime() );
    const QDateTime lastBoot( now.addMSecs( -uptime() ) );
    healthCheckObj[JSON_LASTREBOOTTIMESTAMPUTC] = scaleConfig::instance()->dateTimeToString( lastBoot );

    const scaleDatabase *db ( hteHook::instance()->db() );
    if (( db ) && ( db->isValid() ))
    {
        QDateTime dt( db->lastComm() );
        healthCheckObj[JSON_LASTCOMMUNICATIONTIMESTAMPUTC] = ( dt.isValid() ) ? scaleConfig::instance()->dateTimeToString( dt ) : nullVal;

        dt = db->lastItemChange();
        healthCheckObj[JSON_LASTITEMCHANGERECEIVEDTIMESTAMPUTC] = ( dt.isValid() ) ? scaleConfig::instance()->dateTimeToString( dt ) : nullVal;

        healthCheckObj[JSON_PLU_RECORD_COUNT] = db->activeProductCount();

        if (sendingFullHeartbeat)
        {
            // format label version info
            QJsonArray labels;
            int dept( 0 );        // TODO - what department?

            const scaleDatabase::labelVersionMap labelInfo( db->labelVersionInfo( dept ) );
            const QList<int> labelNums( labelInfo.keys() );

            foreach ( int labelNum, labelNums )
            {
                const scaleDatabase::labelNameVersionPair nameVersion( labelInfo[labelNum] );

                LOG_DEBUG << "heartbeat - label: " << labelNum << " (" <<
                    qPrintable( nameVersion.first ) << "): " <<
                    qPrintable( nameVersion.second );

                QJsonObject labelObj;
                labelObj[JSON_LABEL_NAME] = nameVersion.first;
                labelObj[JSON_LABEL_NUMBER] = labelNum;
                labelObj[JSON_VERSION] = nameVersion.second;

                labels.append( labelObj );
            }
            obj[JSON_LABELS] = ( 0 < labels.size() ) ? labels : nullVal;
        }
    }
    else
    {
        healthCheckObj[JSON_LASTCOMMUNICATIONTIMESTAMPUTC] = nullVal;
        healthCheckObj[JSON_LASTITEMCHANGERECEIVEDTIMESTAMPUTC] = nullVal;
        healthCheckObj[JSON_PLU_RECORD_COUNT] = nullVal;
        if (sendingFullHeartbeat)
            obj[JSON_LABELS] = nullVal;
    }

    qlonglong printed = scaleDatabase::getLabelsPrinted();
    LOG_DEBUG << "heartbeat - getLabelsPrinted: " << printed;
    healthCheckObj[JSON_LABELS_PRINTED] = ( printed == -1 ) ? nullVal : printed;


    LOG_DEBUG << "heartbeat - healthCheck: " << healthCheckObj.size();
    obj[JSON_HEALTH_CHECK] = healthCheckObj;

    const scaleInfo info;
    obj[JSON_SCALEINFO] = info.toJsonObject();

    const scaleSettings settings;
    obj[JSON_SCALESETTINGS] = settings.toJsonObject();

    /*
    if ( !QFile::exists( CHECKSUMS_FILE ) )
    {
        LOG_WARN << "checksums file " << qPrintable(CHECKSUMS_FILE) << " does not exist";
        obj[JSON_LOCALBACKUPFILEINFO] = nullVal;
    }
    else
    {
        QFile f( CHECKSUMS_FILE );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "could not open checksums file " << qPrintable(CHECKSUMS_FILE);
        else
        {
            QJsonArray jsonFilesList;

            QTextStream s( &f );

            // parse all lines
            while ( !s.atEnd() )
            {
                const QString line( s.readLine() );

                LOG_TRACE << qPrintable( line );

                if ( !line.contains( "=" ) || !line.startsWith("download") )
                    continue;

                // split line into key/value pair
                const QStringList values( line.split( "=", KEEP_EMPTY_PARTS ) );
                if ( values.size() < 2 )
                    continue;

                QString fullfname( values[0] );
                QString checksum( values[1] );

                const QStringList fnameParts( fullfname.split( "/", KEEP_EMPTY_PARTS ) );
                if ( fnameParts.size() < 2 )
                    continue;

                QString fname( fnameParts[1] );

                fname = fname.trimmed();
                QString fid = "";
                checksum = checksum.trimmed();
                qint64 fsize = 0;
                if ( !QFile::exists( BACKUP_FILES_DIR + fname ) )
                {
                    LOG_WARN << "backup file " << qPrintable(BACKUP_FILES_DIR) << qPrintable(fname) << " does not exist";
                    continue;
                }

                QFile datafile( BACKUP_FILES_DIR + fname );
                fsize = datafile.size();

                LOG_TRACE << "File: " << qPrintable(fname) << "  checksum: " << qPrintable(checksum) << "  filesize: " << qPrintable(QString::number( fsize ));

                QJsonObject fileObj;
                fileObj["fileName"] = fname;
                fileObj["fileId"] = fid;
                fileObj["fileChecksum"] = checksum;
                fileObj["fileSize"] = fsize;
                fileObj["fileVersion"] = QString("1");

                jsonFilesList.append( fileObj );
            }

            f.close();
        }
    }
    */
    if (( htedb ) && ( htedb->isValid() ))
    {
        const QJsonArray jsonFilesList( htedb->getDownloadFileList() );

        obj[JSON_LOCALBACKUPFILEINFO] = jsonFilesList;
    }
    else
        obj[JSON_LOCALBACKUPFILEINFO] = nullVal;


    if (sendingFullHeartbeat)
    {
        const QJsonArray peripherals( scalePeripherals::instance()->toJsonArray() );
        obj[JSON_PERIPHERALS] = ( 0 < peripherals.size() ) ? peripherals : nullVal;

        const networkSettings netSettings;
        obj[JSON_NETWORKSETTINGS] = netSettings.toJsonObject();

        const scaleFirmware firmware;
        obj[JSON_FIRMWARE] = firmware.toJsonObject();

        obj[JSON_DISK] = systemDiskInfo();
    }


    // For Walmart, not really for HTe appHook
    //obj[JSON_PLU] = plu();
    //obj[JSON_USAGE_TODAY] = scaleUsageToday::instance()->toJsonObject();


    //QJsonDocument theObj(obj);
    //LOG_DEBUG << "heartbeat - " << qPrintable( theObj.toJson() );

    return QJsonDocument( obj );
}


