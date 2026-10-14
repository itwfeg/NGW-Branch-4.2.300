/**
 * @file gstpdappconfig.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "gstpdappconfig.h"

#include <platform.h>

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdAppConfig::gstpdAppConfig( QObject *parent ) :
    _Mybase( parent )
{
    setObjectName( "gstpdAppConfig" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstpdAppConfig::~gstpdAppConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstpdAppConfig::launchCommand( const QString& /*filename*/, const QRect& /*geometry*/, videoPlayerDevice::type device ) const
{
    // freescale arm
    if ( platformType::EmbeddedLinux == PLATFORM )
    {
        if ( videoPlayerDevice::Operator == device )
            return QString( "DISPLAY=:0 gstpd -oper" );
        else if ( videoPlayerDevice::Customer == device )
            return QString( "DISPLAY=:0 gstpd -cust" );
    }

    return QString( "gstpd %1" ).arg( (videoPlayerDevice::Customer == device) ? "-cust" : "-oper" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstpdAppConfig::killCommand( unsigned int id ) const
{
    return QString( "kill -s INT %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstpdAppConfig::forceKillCommand( unsigned int id ) const
{
    return QString( "kill -9 %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstpdAppConfig::logfile( videoPlayerDevice::type device ) const
{
    if ( videoPlayerDevice::Operator == device )
        return "/tmp/gstpdoper.txt";
    else if ( videoPlayerDevice::Customer == device )
        return "/tmp/gstpdcust.txt";

    return _Mybase::logfile( device );
}
