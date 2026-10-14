/**
 * @file test.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "test.h"

#include <dbusmediaplayerclient.h>
#include <logmanager.h>

#include <QApplication>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////
test::test( const QString &path, const QString &filename, abstractMediaPlayer::MediaType type, QObject *parent ) :
    _Mybase( parent ),
    valid_( false ),
    player_( new dbusMediaPlayerClient( path, this ) ),
    filename_( filename ),
    type_( type ),
    checkPlaybackTimer_( this ),
    validateTimer_( this )
{
    // setup playback timer
    checkPlaybackTimer_.setInterval( 250 );
    checkPlaybackTimer_.setSingleShot( true );

    connect( &checkPlaybackTimer_, SIGNAL(timeout()), SLOT(onCheckPlayback()) );

    // setup validation timer
    validateTimer_.setInterval( 0 );
    validateTimer_.setSingleShot( true );

    connect( &validateTimer_, SIGNAL(timeout()), SLOT(validate()) );

    validateTimer_.start();

    // connect interface signals
    connect( player_, SIGNAL(durationChanged(qint64)), SLOT(onDurationChanged(qint64)) );
    connect( player_, SIGNAL(error(QString)), SLOT(onError(QString)) );
    connect( player_, SIGNAL(finished()), SLOT(onFinished()) );
    connect( player_, SIGNAL(geometryChanged(QRect)), SLOT(onGeometryChanged(QRect)) );
    connect( player_, SIGNAL(mediaStatusChanged(abstractMediaPlayer::MediaStatus)), SLOT(onMediaStatusChanged(abstractMediaPlayer::MediaStatus)) );
    connect( player_, SIGNAL(mutedChanged(bool)), SLOT(onMutedChanged(bool)) );
    connect( player_, SIGNAL(positionChanged(qint64)), SLOT(onPositionChanged(qint64)) );
    connect( player_, SIGNAL(seekableChanged(bool)), SLOT(onSeekableChanged(bool)) );
    connect( player_, SIGNAL(stateChanged(abstractMediaPlayer::State)), SLOT(onStateChanged(abstractMediaPlayer::State)) );
    connect( player_, SIGNAL(visibleChanged(bool)), SLOT(onVisibleChanged(bool)) );
    connect( player_, SIGNAL(volumeChanged(int)), SLOT(onVolumeChanged(int)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
test::~test()
{
    if ( player_ )
        delete player_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool test::validate()
{
    if ( !player_->isValid() )
        return false;

    player_->setGeometry( QRect( 0, 0, 100, 100 ) );

    player_->load( filename_, type_ );
    QThread::msleep( 2000 );

    player_->play();
    QThread::msleep( 5000 );

    player_->pause();
    QThread::msleep( 2000 );

    player_->play();
    QThread::msleep( 5000 );

    player_->stop();
    QThread::msleep( 2000 );

    player_->play();
    QThread::msleep( 1000 );

    player_->setMuted( true );
    QThread::msleep( 1000 );
    player_->setMuted( false );
    QThread::msleep( 1000 );

    player_->setGeometry( QRect( 100, 100, 300, 300 ) );
    QThread::msleep( 1000 );

    player_->setVolume( 75 );
    QThread::msleep( 1000 );
    player_->setVolume( 50 );
    QThread::msleep( 1000 );
    player_->setVolume( 25 );
    QThread::msleep( 1000 );
    player_->setVolume( 0 );
    QThread::msleep( 1000 );
    player_->setVolume( 100 );
    QThread::msleep( 1000 );

    player_->hide();
    QThread::msleep( 5000 );

    player_->show();
    QThread::msleep( 1000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->setPosition( 10000 );
    QThread::msleep( 2000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->setPosition( 0 );
    QThread::msleep( 2000 );

    player_->pause();
    QThread::msleep( 2000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->setPosition( 10000 );
    QThread::msleep( 2000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->setPosition( 0 );
    QThread::msleep( 2000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->stop();
    QThread::msleep( 2000 );

    player_->setPosition( 60000 );
    QThread::msleep( 2000 );

    player_->setPosition( 0 );
    QThread::msleep( 2000 );

    player_->setPosition( 10000 );
    QThread::msleep( 2000 );

    return (valid_ = true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onCheckPlayback()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onDurationChanged( qint64 duration )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onError( const QString &error )
{
    LOG_CLASS_INFO( "test", "onError" );

    LOG_ERROR( "error: %s", qPrintable( error ) );
    QCoreApplication::exit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onFinished()
{
    QCoreApplication::exit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onGeometryChanged( const QRect &rect )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onMediaStatusChanged( abstractMediaPlayer::MediaStatus status )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onMutedChanged( bool muted )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onPositionChanged( qint64 position )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onSeekableChanged( bool seekable )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onStateChanged( abstractMediaPlayer::State newState )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onVisibleChanged( bool visible )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void test::onVolumeChanged( int volume )
{
}
