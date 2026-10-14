/**
 * @file abstractmediaplayer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "abstractmediaplayer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractMediaPlayer::abstractMediaPlayer( QObject *parent ) :
    _Mybase( parent )
{
    // register meta types for singals and slots
    qRegisterMetaType<abstractMediaPlayer::State>( "abstractMediaPlayer::State" );
    qRegisterMetaType<abstractMediaPlayer::MediaStatus>( "abstractMediaPlayer::MediaStatus" );
    qRegisterMetaType<abstractMediaPlayer::MediaType>( "abstractMediaPlayer::MediaType" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractMediaPlayer::~abstractMediaPlayer()
{
}
