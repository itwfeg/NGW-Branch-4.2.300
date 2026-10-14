/**
 * @file configrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "configrequest.h"
#include "scaleconfig.h"
#include "scalesettings.h"
#include "networksettings.h"
#include "stringsjson.h"

#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////
configRequest::configRequest( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
configRequest::~configRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configRequest::setInitScaleInfo( const QString& scaleVendor, const QString& scaleModel )
{
    QJsonObject scale( request_[JSON_SCALE].toObject() );
    scale[JSON_SCALE_VENDOR] = scaleVendor;
    scale[JSON_SCALE_MODEL] = scaleModel;

    request_[JSON_SCALE] = scale;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument configRequest::toJsonDoc() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    QJsonObject obj( request_ );

    if ( scaleConfig::instance()->isInitialized() )
    {
        obj[JSON_CURRENT_CONFIG] = scaleConfig::instance()->toJsonObject();
        obj[JSON_CONFIG_STAMP] = scaleConfig::instance()->timestamp();
    }
    else
    {
        obj[JSON_CURRENT_CONFIG] = nullVal;
        obj[JSON_CONFIG_STAMP] = nullVal;

        // retrieve ip address
        QString ipAddress4;
        QString ipAddress6;

        networkSettings::ipAddress( ipAddress4, ipAddress6 );

        if ( ipAddress4.length() )
            obj[JSON_IP_ADDRESS] = ipAddress4;
        else if ( ipAddress6.length() )
            obj[JSON_IP_ADDRESS] = ipAddress6;
        else
            obj[JSON_IP_ADDRESS] = nullVal;
    }

    return QJsonDocument( obj );
}
