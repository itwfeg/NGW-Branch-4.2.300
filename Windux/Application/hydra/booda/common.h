/**
 * @file common.h
 * Common (shared) includes.
 *
 * @author Randy Blankley
 */

#ifndef COMMON_H
#define COMMON_H

#include <QtGlobal>

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

// ============================================================================
// Logging
// ============================================================================

#if defined(HAVE_CLIO_H)
#include <clio.h>

#else
#include <QDebug>
#define LOG_ERROR qCritical()
#define LOG_WARN qWarning()
#define LOG_INFO qInfo()
#define LOG_DEBUG qDebug()
#define LOG_TRACE qDebug()

#endif

// ============================================================================
// Qt Backward Compat
// ============================================================================

#if QT_VERSION >= 0x050e00
#define KEEP_EMPTY_PARTS Qt::KeepEmptyParts
#define SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define KEEP_EMPTY_PARTS QString::KeepEmptyParts
#define SKIP_EMPTY_PARTS QString::SkipEmptyParts
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

// ============================================================================
// Helper Methods
// ============================================================================

#include <QString>

#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>

template <typename T>
inline QString int_to_hex( T val, size_t width = sizeof(T)*2 )
{
    static std::locale l( "" );

    std::stringstream ss;
    ss.imbue( l ); // remove commas from formatting

    ss << std::setfill('0') << std::setw(width) << std::hex << (val|0);

    return QString::fromStdString( ss.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMON_H
