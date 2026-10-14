/**
 * @file jsondocumentevent.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "jsondocumentevent.h"

#include <QDebug>
#include <QVector>

static const QVector<uint> JSON_DOCUMENT_EVENT = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonDocumentEvent::JsonDocumentEvent( const QJsonDocument& doc ) :
    _Mybase( JSON_DOCUMENT_EVENT )
{
    setDocument( doc );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonDocumentEvent::JsonDocumentEvent( DeviceType device, uint event, const QJsonDocument& doc ) :
    _Mybase( device, event, JSON_DOCUMENT_EVENT )
{
    setDocument( doc );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonDocumentEvent::JsonDocumentEvent( const QByteArray& a ) :
    _Mybase( a )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonDocumentEvent::~JsonDocumentEvent()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument JsonDocumentEvent::document() const
{
    QJsonParseError e;

    // parse JSON data element
    const QJsonDocument doc( QJsonDocument::fromJson( data_[0], &e ) );

    if ( QJsonParseError::NoError != e.error )
        qWarning() << "failed to parse json doc" << e.error << e.errorString() << e.offset;

    return doc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void JsonDocumentEvent::setDocument( const QJsonDocument& value )
{
    // convert to byte array
    data_[0] = value.toJson( QJsonDocument::Compact );
}
