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
#include "scalefirmware.h"
#include "scaleperipherals.h"
#include "scalesettings.h"
#include "scaleusagetoday.h"
#include "stringsconfig.h"
#include "stringsjson.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStorageInfo>

#if defined(Q_OS_LINUX)
#include <sys/sysinfo.h>
#elif defined(Q_OS_WIN32)
#include <Windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
eventControllerRequest::eventControllerRequest( RequestType type, QObject *parent ) :
    _Mybase( parent )
{
    const QDateTime now( QDateTime::currentDateTime() );
    const QDateTime lastBoot( now.addMSecs( -uptime() ) );

    healthCheck_[JSON_TRIGGER_TYPE] = requestTypeToString( type );

    healthCheck_[JSON_CONFIG_UUID] = scaleConfig::instance()->value<QString>( CONF_UUID );
    healthCheck_[JSON_LAST_REBOOT_STAMP] = scaleConfig::instance()->dateTimeToString( lastBoot );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
eventControllerRequest::~eventControllerRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument eventControllerRequest::toJsonDoc() const
{
    QJsonObject obj( request_ );
    obj[JSON_HEALTH_CHECK] = healthCheck();

    return QJsonDocument( obj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject eventControllerRequest::healthCheck() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    const scaleSettings settings;
    const scaleFirmware firmware;
    const QJsonArray peripherals( scalePeripherals::instance()->toJsonArray() );

    QJsonObject obj( healthCheck_ );

    if ( lastComm_.isValid() )
        obj[JSON_LAST_COMM_STAMP] = scaleConfig::instance()->dateTimeToString( lastComm_ );
    else
        obj[JSON_LAST_COMM_STAMP] = nullVal;

    if ( lastItemReceived_.isValid() )
        obj[JSON_LAST_ITEM_CHANGE_STAMP] = scaleConfig::instance()->dateTimeToString( lastItemReceived_ );
    else
        obj[JSON_LAST_ITEM_CHANGE_STAMP] = nullVal;

    obj[JSON_PLU] = plu();
    obj[JSON_SETTINGS] = settings.toJsonObject();
    obj[JSON_FIRMWARE] = firmware.toJsonObject();

    if ( 0 < peripherals.size() )
        obj[JSON_PERIPHERALS] = peripherals;
    else
        obj[JSON_PERIPHERALS] = nullVal;

    if ( 0 < labels_.size() )
        obj[JSON_LABELS] = labels_;
    else
        obj[JSON_LABELS] = nullVal;

    obj[JSON_USAGE_TODAY] = scaleUsageToday::instance()->toJsonObject();
    obj[JSON_DISK] = systemDiskInfo();

    return obj;
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
