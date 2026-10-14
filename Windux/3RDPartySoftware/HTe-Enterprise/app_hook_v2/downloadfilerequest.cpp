/**
 * @file downloadfilerequest.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "downloadfilerequest.h"
#include "stringsjson.h"

#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////
downloadFileRequest::downloadFileRequest( const QString& filename, QObject *parent ) :
    _Mybase( parent ),
    filename_( filename )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
downloadFileRequest::~downloadFileRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument downloadFileRequest::toJsonDoc() const
{
    QJsonObject download;
    download[JSON_FILENAME] = filename_;

    QJsonObject obj( event_ );
    obj[JSON_DOWNLOAD] = download;

    return QJsonDocument( obj );
}
