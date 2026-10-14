/**
 * @file videoplayer.cpp
 * Video player base class.
 *
 * The @c videoPlayer class is the base object abstraction layer for all video players.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "videoplayer.h"
#include "videoplayerconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayer::videoPlayer( QWidget *parent ) :
    _Mybase( parent ),
    device_( videoPlayerDevice::Default )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayer::videoPlayer( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( parent ),
    device_( deviceType )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayer::~videoPlayer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayer *videoPlayer::create( videoPlayerDevice::type deviceType, QWidget *parent )
{
    return videoPlayerConfig<PLATFORM>::createVideoPlayer( deviceType, parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayer::initialize()
{
    QPalette palette( _Mybase::palette() );
    palette.setBrush( _Mybase::backgroundRole(), QBrush( Qt::black ) );

    _Mybase::setAutoFillBackground( true );
    _Mybase::setPalette( palette );
    _Mybase::setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
