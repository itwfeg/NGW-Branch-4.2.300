/**
 * @file gstlaunchappconfig.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "gstlaunchappconfig.h"

// Linux only!
#ifndef Q_OS_WIN

#include <platform.h>

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
gstlaunchAppConfig::gstlaunchAppConfig( QObject *parent ) :
    _Mybase( parent )
{
    setObjectName( "gstreamerAppConfig" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstlaunchAppConfig::~gstlaunchAppConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::launchCommand( const QString& filename, const QRect& geometry, videoPlayerDevice::type device ) const
{
    // freescale arm
    if ( platformType::EmbeddedLinux == PLATFORM )
    {
        // when dedicated hardware decoding devices are present use them!
        if ( videoSinkDeviceName( device ).length() )
        {
            QString videoSinkString = _Myt::videoSink( geometry, device );
            QString audioSinkString = _Myt::audioSink( device );
            return QString( "gst-launch -v --gst-debug=2 playbin2 uri=file://%1 %2 %3" ).arg( filename, videoSinkString, audioSinkString );
        }

        // otherwise revert to software
        QString displayName;
        int width;
        int height;

        if ( videoPlayerDevice::Customer == device )
        {
            displayName = ":0.1";
            width = 800;
            height = 480;
        }
        else
        {
            displayName = ":0.0";
            width = 1024;
            height = 600;
        }

        if ( geometry.isValid() )
        {
            width = geometry.width();
            height = geometry.height();
        }

        const QString command(
            "gst-launch -v --gst-debug=2 filesrc location=%1 typefind=true ! aiurdemux name=demux"
            " demux. ! queue ! vpudec ! imxvideoconvert_ipu ! video/x-raw-rgb,width=%2,height=%3 ! ximagesink display=%4 window-width=%2 window-height=%3"
            " demux. ! queue ! beepdec ! alsasink" );

        return command.arg( filename, QString::number( width ), QString::number( height ), displayName );
    }

    // generic pipeline
    return QString( "gst-launch -v --gst-debug=2 playbin2 uri=file://%1" ).arg( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::playCommand( unsigned int id ) const
{
    return QString( "kill -s USR1 %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::stopCommand( unsigned int id ) const
{
    return QString( "kill -s USR2 %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::killCommand( unsigned int id ) const
{
    return QString( "kill -s INT %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::forceKillCommand( unsigned int id ) const
{
    return QString( "kill -9 %1" ).arg( id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::audioSink( videoPlayerDevice::type /*device*/ ) const
{
    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::videoSink( const QRect& geometry, videoPlayerDevice::type device ) const
{
    // freescale arm
    if ( platformType::EmbeddedLinux == PLATFORM )
    {
        QString geometryString;
        QString deviceString( QString( "device=%1" ).arg( videoSinkDeviceName( device ) ) );

        if ( geometry.isValid() )
            geometryString.sprintf( "axis-left=%d axis-top=%d disp-width=%d disp-height=%d", geometry.left(), geometry.top(), geometry.width(), geometry.height() );

        return QString( "video-sink=\"imxv4l2sink %1 %2\"" ).arg( geometryString, deviceString );
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::logfile( videoPlayerDevice::type device ) const
{
    if ( videoPlayerDevice::Operator == device )
        return "/tmp/operatorVideoPlayback.txt";
    else if ( videoPlayerDevice::Customer == device )
        return "/tmp/customerVideoPlayback.txt";

    return _Mybase::logfile( device );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gstlaunchAppConfig::videoSinkDeviceName( videoPlayerDevice::type device ) const
{
    if ( videoPlayerDevice::Operator == device )
        return "/dev/video17";
    else if (( videoPlayerDevice::Customer == device ) && ( QFile::exists( "/dev/video19" ) ))
        return "/dev/video19";

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // linux
