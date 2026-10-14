/**
 * @file downloadfileurlrequest.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "downloadfileurlrequest.h"
#include "stringsjson.h"

#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////
downloadFileUrlRequest::downloadFileUrlRequest( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
downloadFileUrlRequest::downloadFileUrlRequest( const QString& requestUuid, QObject *parent ) :
    _Mybase( requestUuid, parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
downloadFileUrlRequest::~downloadFileUrlRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument downloadFileUrlRequest::toJsonDoc() const
{
    return QJsonDocument( request_ );
}
