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
    buffer.append( "HR" + request.toLatin1() + "\x1e" );

#ifdef CLIO_H
    LOG_DEBUG << "sending request: " << HEX_DUMP( buffer.constData(), buffer.length() );
#endif

    // connect to server
    QTcpSocket s;
    s.connectToHost( QHostAddress( server ), port );

    if ( !s.waitForConnected( 2000 ) )
    {
        LOG_WARN << "failed to connect to host";
        return false;
    }
    else if ( !s.write( buffer ) )
    {
        LOG_WARN << "socket write failed";
        return false;
    }

    LOG_TRACE << "wait for write...";
    s.waitForBytesWritten( 5000 );

    if ( waitForRead )
    {
        LOG_DEBUG << "wait for read...";
        s.waitForReadyRead( 5000 );
    }

    if ( delay )
    {
        LOG_TRACE << "delay " << delay;
        QThread::msleep( delay );
    }

    s.close();

    LOG_TRACE << "done";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMON_H
