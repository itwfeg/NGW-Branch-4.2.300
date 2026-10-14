/**
 * @file settingsrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "scalesettings.h"
#include "settingsrequest.h"
#include "stringsjson.h"

#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////
settingsRequest::settingsRequest( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
settingsRequest::settingsRequest( const QString& requestUuid, QObject *parent ) :
    _Mybase( requestUuid, parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
settingsRequest::~settingsRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument settingsRequest::toJsonDoc() const
{
    const scaleSettings settings;

    QJsonObject obj( request_ );
    obj[JSON_CURRENT_SETTINGS] = settings.toJsonObject();

    return QJsonDocument( obj );
}
