/**
 * @file common.h
 * Common (shared) includes.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef COMMON_H
#define COMMON_H

#include <QtGlobal>

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QThread>

#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>

#if defined(Q_OS_LINUX)
#include <clio.h>
#elif defined(Q_OS_WIN32)
#include <QDebug>
#define LOG_ERROR qCritical()
#define LOG_WARN qWarning()
#define LOG_INFO qInfo()
#define LOG_DEBUG qDebug()
#define LOG_TRACE qDebug()
#endif

#if defined(Q_OS_LINUX)
#define CLIO_LOGGING_CONFIG "/usr/share/hte-apphook/logging.config"
#elif defined(Q_OS_WIN32)
#define CLIO_LOGGING_CONFIG "\\tmp\\logging.config"
#endif

#if defined(Q_OS_LINUX)
static const QString APPHOOK_DIR        ( "/usr/share/hte-apphook/" );
static const QString LOG_DIR            ( "/var/log/" );
static const QString HOBART_LOCAL_DIR   ( "/usr/local/hobart/" );

static const QString INTERNAL_CONFIG            ( APPHOOK_DIR + "hobart_internal.config" );
static const QString WEB_HEADERS_FILE           ( APPHOOK_DIR + "web_headers.config" );
static const QString CONFIG_FILE                ( APPHOOK_DIR + "hte.config" );
static const QString CONFIG_FILE_OLD            ( APPHOOK_DIR + "hte.config.old" );
static const QString LOG_FILE                   ( LOG_DIR + "WebServiceRequests.log" );
static const QString APP_LOG                    ( LOG_DIR + "hte-applog.txt" );
static const QString DIALOGMGR_LOG              ( LOG_DIR + "hte-dialogmgr.txt" );
static const QString COMM_LOG                   ( LOG_DIR + "hte-commlog.txt" );
static const QString USAGE_FILE                 ( APPHOOK_DIR + "productusage" );
static const QString LAST_USAGE_FILE            ( APPHOOK_DIR + "lastapiusage" );
static const QString ONLINE_TIME_FILE           ( APPHOOK_DIR + "lastonline" );
static const QString TEMP_FILE_CACHE_DIR        ( APPHOOK_DIR + "cache" );
static const QString TEMP_FILE                  ( TEMP_FILE_CACHE_DIR + "/download-XXXXXX.tmp" );
static const QString EVENTS_DIR                 ( APPHOOK_DIR + "events" );
static const QString FETCH_PLU_OFFSET_TIMESTAMP ( APPHOOK_DIR + "fetchpluoffsettimestamp" );
static const QString SCALE_DOWNLOAD_DIR         ( HOBART_LOCAL_DIR + "download/" );
static const QString SCALE_STAGING_DIR          ( HOBART_LOCAL_DIR + "staging/" );
static const QString SCALE_UPGRADE_DIR          ( HOBART_LOCAL_DIR + "upgrade/" );
static const QString CHECKSUMS_FILE             ( HOBART_LOCAL_DIR + "checksums.txt" );
static const QString BACKUP_FILES_DIR           ( HOBART_LOCAL_DIR + "backup/" );
static const QString SCALESETTINGS_FILE         ( HOBART_LOCAL_DIR + "db/settingswizard/scalesettings" );
static const QString DIAGDBNAME                 ( HOBART_LOCAL_DIR + "db/diagnostics/diag.db" );


#elif defined(Q_OS_WIN32)
static const QString APPHOOK_DIR        ( "\\tmp\\" );
static const QString LOG_DIR            ( "\\tmp\\" );
static const QString HOBART_LOCAL_DIR   ( "C:\\Program Files\\Hobart\\" );

static const QString INTERNAL_CONFIG            ( HOBART_LOCAL_DIR + "hobart_internal.config" );
static const QString WEB_HEADERS_FILE           ( HOBART_LOCAL_DIR + "web_headers.config" );
static const QString CONFIG_FILE                ( APPHOOK_DIR + "hte.config" );
static const QString CONFIG_FILE_OLD            ( APPHOOK_DIR + "hte.config.old" );
static const QString LOG_FILE                   ( LOG_DIR + "WebServiceRequests.log" );
static const QString USAGE_FILE                 ( APPHOOK_DIR + "productusage" );
static const QString LAST_USAGE_FILE            ( APPHOOK_DIR + "lastapiusage" );
static const QString ONLINE_TIME_FILE           ( APPHOOK_DIR + "lastonline" );
static const QString TEMP_FILE_CACHE_DIR        ( APPHOOK_DIR + "cache" );
static const QString TEMP_FILE                  ( TEMP_FILE_CACHE_DIR + "\\download-XXXXXX.tmp" );
static const QString EVENTS_DIR                 ( APPHOOK_DIR + "events" );
static const QString FETCH_PLU_OFFSET_TIMESTAMP ( "fetchpluoffsettimestamp" );
static const QString SCALE_DOWNLOAD_DIR         ( HOBART_LOCAL_DIR + "download\\" );
static const QString SCALE_STAGING_DIR          ( HOBART_LOCAL_DIR + "staging\\" );
static const QString SCALE_UPGRADE_DIR          ( HOBART_LOCAL_DIR + "upgrade\\" );
static const QString CHECKSUMS_FILE             ( APPHOOK_DIR + "checksums.txt" );
static const QString BACKUP_FILES_DIR           ( APPHOOK_DIR );
static const QString SCALESETTINGS_FILE         ( HOBART_LOCAL_DIR + "db\\settingswizard\\scalesettings" );
static const QString DIAGDBNAME                 ( HOBART_LOCAL_DIR + "db\\diagnostics\\diag.db" );
#endif

static const QString TEMP_FILE_FILTER( "download-*.tmp" );

#if QT_VERSION >= 0x050e00
#define KEEP_EMPTY_PARTS Qt::KeepEmptyParts
#else
#define KEEP_EMPTY_PARTS QString::KeepEmptyParts
#endif

// backward compatible support for streaming of QJsonObject
#if QT_VERSION < 0x050d00

#include <QByteArray>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

inline QDataStream& operator<< ( QDataStream& stream, const QJsonDocument& doc )
{
    stream << doc.toJson(QJsonDocument::Compact);
    return stream;
}

inline QDataStream& operator>> ( QDataStream& stream, QJsonDocument& doc )
{
    QByteArray buffer;
    stream >> buffer;
    QJsonParseError parseError;
    doc = QJsonDocument::fromJson( buffer, &parseError );
    if (( parseError.error ) && ( !buffer.isEmpty() ))
        stream.setStatus( QDataStream::ReadCorruptData );
    return stream;
}

inline QDataStream& operator<< ( QDataStream& stream, const QJsonObject& object )
{
    QJsonDocument doc{object};
    stream << doc.toJson( QJsonDocument::Compact );
    return stream;
}

inline QDataStream& operator>> ( QDataStream& stream, QJsonObject& object )
{
    QJsonDocument doc;
    stream >> doc;
    object = doc.object();
    return stream;
}
#endif

template <typename T>
inline QString int_to_hex( T val, size_t width = sizeof(T)*2 )
{
    static std::locale l( "" );

    std::stringstream ss;
    ss.imbue( l ); // remove commas from formatting

    ss << std::setfill('0') << std::setw(width) << std::hex << (val|0);

    return QString::fromStdString( ss.str() );
}

inline bool hook_request( const QString& request, bool waitForRead = false, int delay = 0 )
{
    const QString server( "127.0.0.1" );
    const quint16 port( 6000 );

    // setup buffers
    QByteArray buffer;
    buffer.append( "RTDO\x1f" );
    buffer.append( request.toLatin1() + "\x1e" );

    // connect to server
    QTcpSocket s;
    s.connectToHost( QHostAddress( server ), port );

    if ( !s.waitForConnected( 2000 ) )
        return false;
    else if ( !s.write( buffer ) )
        return false;

    if ( waitForRead )
        s.waitForReadyRead( 5000 );

    if ( delay )
        QThread::msleep( delay );

    s.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMON_H
