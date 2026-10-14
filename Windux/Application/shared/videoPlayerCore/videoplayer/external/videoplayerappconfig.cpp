/**
 * @file videoplayerappconfig.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "videoplayerappconfig.h"

#include <platform.h>

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerAppConfig::videoPlayerAppConfig( QObject *parent ) :
    _Mybase( parent )
{
    setObjectName( "videoPlayerAppConfig" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerAppConfig::~videoPlayerAppConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString videoPlayerAppConfig::launchCommand( const QString &filename, const QRect& /*geometry*/, videoPlayerDevice::type /*device*/ ) const
{
#ifdef Q_OS_WIN

    return QString( "videoPlayer -2 -disableui \"%1\"" ).arg( filename );
#else

    return QString( "videoPlayer -disableui %1" ).arg( filename );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString videoPlayerAppConfig::killCommand( unsigned int id ) const
{
#ifdef Q_OS_WIN

    return _Mybase::killCommand( id );
#else

    return QString( "kill %1" ).arg( id );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString videoPlayerAppConfig::forceKillCommand( unsigned int id ) const
{
#ifdef Q_OS_WIN

    return _Mybase::killCommand( id );
#else

    return QString( "kill %1" ).arg( id );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString videoPlayerAppConfig::logfile( videoPlayerDevice::type device ) const
{
#ifdef Q_OS_WIN

    Q_UNUSED( device );

    return QString( "vplog.txt" );
#else

    if ( videoPlayerDevice::Customer == device )
        return "/tmp/vplogcust.txt";

    return "/tmp/vplogoper.txt";

#endif
}
