/**
 * @file custgstmp.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <QtGlobal>

// Linux only!
#ifndef Q_OS_WIN

#include "custgstmp.h"

#include <QFile>

const QString custgstmp::DBUS_SERVICE_NAME( "com.itwweighwrap.mediaplayer.customer" );

///////////////////////////////////////////////////////////////////////////////////////////////////
custgstmp::custgstmp( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
custgstmp::~custgstmp()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString custgstmp::fb() const
{
#if defined( ARM_MXC_FREESCALE )

    // HTi frame buffer of customer display
    if ( QFile::exists( "/dev/video19" ) )
        return QString( "/dev/fb2" );

    // HTs frame buffer of customer display
    return QString( "/dev/fb3" );
#else

    return QString();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString custgstmp::v4l2dev() const
{
#if defined( ARM_MXC_FREESCALE )

    // HTi has hw acceleration for video playback, use it
    if ( QFile::exists( "/dev/video19" ) )
        return QString( "/dev/video19" );

    // HTs does not have hw acceleration, use appropriate device
    return QString( "/dev/video18" );

#endif

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux
