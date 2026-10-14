/**
 * @file externalvideoplayer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "externalvideoplayer.h"

#include <logmanager.h>

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
externalVideoPlayer::externalVideoPlayer( appConfig *config, QWidget *parent ) :
    _Mybase( parent ),
    config_( config )
{
    setObjectName( "externalVideoPlayer" );

    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
externalVideoPlayer::externalVideoPlayer( appConfig *config, videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( deviceType, parent ),
    config_( config )
{
    setObjectName( "externalVideoPlayer" );

    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
externalVideoPlayer::~externalVideoPlayer()
{
    terminate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool externalVideoPlayer::active() const
{
    LOG_CLASS_INFO( "externalVideoPlayer", "active" );

    bool result( process_.active() );

    LOG_TRACE( "player active %d", result );
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool externalVideoPlayer::load( const QString &filename, bool startPlaying )
{
    LOG_CLASS_INFO( "externalVideoPlayer", "load" );

    LOG_INFO( "loading media file '%s'", qPrintable( filename ) );

    // make sure file exists
    if ( !QFile::exists( filename ) )
    {
        LOG_ERROR( "media file '%s' does not appear to exist", qPrintable( filename ) );
        return false;
    }

    // get rid of anything currently running
    LOG_INFO( "killing active process...", NULL );
    terminate();

    QRect rect( geometry() );

    LOG_INFO( "geometry left %d top %d width %d height %d",
        rect.left(),
        rect.top(),
        rect.width(),
        rect.height() );

    // save off our commands
    launchCommand_ = config_->launchCommand( filename, rect, device() );
    playCommand_ = "";
    pauseCommand_ = "";
    stopCommand_ = "";
    killCommand_ = "";

    // play!
    if ( startPlaying )
    {
        LOG_TRACE( "playing...", NULL );
        play();

        return active();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::play()
{
    LOG_CLASS_INFO( "externalVideoPlayer", "play" );

    LOG_INFO( "playing media", NULL );

    // load up new video
    if ( !active() )
    {
        LOG_INFO( "starting new process...", NULL );
        LOG_DEBUG( "launch command is '%s'", qPrintable( launchCommand_ ) );

        bool result = process_.start(
            launchCommand_,                         // command and arguments all in this parameters
            QStringList(),                          // no command arguments
            QString(),                              // working directory not set
            START_COMMAND_TIMEOUT_MS,
            QString(),
            config_->logfile( device() ) );

        if ( !result )
            LOG_WARN( "failed to play video!", NULL );
        else
        {
            LOG_INFO( "pid %d created", process_.pid() );

            playCommand_ = config_->playCommand( process_.pid() );
            pauseCommand_ = config_->pauseCommand( process_.pid() );
            stopCommand_ = config_->stopCommand( process_.pid() );
            killCommand_ = config_->killCommand( process_.pid() );
        }
    }
    else if ( playCommand_.length() )
    {
        LOG_DEBUG( "play command is '%s'", qPrintable( playCommand_ ) );

        QProcess play;
        play.execute( playCommand_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool externalVideoPlayer::canPause() const
{
    return (pauseCommand_.length() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::pause()
{
    LOG_CLASS_INFO( "externalVideoPlayer", "pause" );

    LOG_INFO( "pausing media", NULL );

    if ( !active() )
        LOG_TRACE( "player not active", NULL );
    else if ( !pauseCommand_.length() )
        LOG_TRACE( "no pause command available", NULL );
    else
    {
        LOG_DEBUG( "pause command is '%s'", qPrintable( pauseCommand_ ) );

        QProcess pause;
        pause.execute( pauseCommand_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::stop()
{
    LOG_CLASS_INFO( "externalVideoPlayer", "stop" );

    LOG_INFO( "stopping media", NULL );

    if ( !active() )
        LOG_TRACE( "player not active", NULL );
    else if ( !stopCommand_.length() )
    {
        LOG_TRACE( "no stop command available... terminating...", NULL );
        terminate();
    }
    else
    {
        LOG_DEBUG( "stop command is '%s'", qPrintable( stopCommand_ ) );

        QProcess stop;
        stop.execute( stopCommand_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::terminate()
{
    LOG_CLASS_INFO( "externalVideoPlayer", "terminate" );

    if ( !process_.active() )
    {
        LOG_TRACE( "player not active", NULL );
        return;
    }

    bool stopVideoPlayer( true );

    if ( killCommand_.length() )
    {
        unsigned int processId( process_.pid() );

        if ( !processId )
            LOG_DEBUG( "bad process id", NULL );
        else
        {
            LOG_DEBUG( "kill command is '%s'", qPrintable( killCommand_ ) );

            QProcess kill;
            kill.execute( killCommand_ );

            LOG_TRACE( "waiting for process to finish...", NULL );

            if ( process_.waitForFinished( STOP_COMMAND_TIMEOUT_MS ) )
                stopVideoPlayer = false;
            else
            {
                LOG_WARN( "process (pid %d) is not playing nicely!", processId );
            }
        }
    }

    if ( stopVideoPlayer )
    {
        LOG_INFO( "stopping process...", NULL );
        process_.stop( TERM_COMMAND_TIMEOUT_MS );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::onProcessFinished()
{
    LOG_CLASS_INFO( "externalVideoPlayer", "onProcessFinished" );

    LOG_DEBUG( "playback complete", NULL );
    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void externalVideoPlayer::initialize()
{
    // take ownership of config
    config_->setParent( this );

    // connect slots
    connect( &process_, SIGNAL( finished() ), SLOT( onProcessFinished() ) );
}
