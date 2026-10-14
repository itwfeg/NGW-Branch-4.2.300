/**
 * @file videoplayerwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "seekslider/seekslider.h"
#include "volumeslider/volumeslider.h"
#include "videoplayerwidget.h"

#include "seekslider/seekslider.h"
#include "volumeslider/volumeslider.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

static const char PLAY_BUTTON_TEXT[] = "&Play";
static const char PAUSE_BUTTON_TEXT[] = "&Pause";
static const char STOP_BUTTON_TEXT[] = "&Stop";
static const char EXIT_BUTTON_TEXT[] = "E&xit";

static const QSize BUTTON_ICON_SIZE( 32, 32 );
static const int BUTTON_HEIGHT = 40;
static const QSize WIDGET_SIZE( 400, 300 );

template <>
void videoPlayerWidget::deleteVideoObjectImpl( videoPlayer *ptr )
{
    ptr->disconnect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerWidget::videoPlayerWidget( videoPlayerDevice::type deviceType, QWidget *parent ) :
    _Mybase( parent ),
    player_( NULL ),
    disableUi_( false )
{
    setObjectName( "videoPlayerWidget" );

    // initialize
    initialize( deviceType );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerWidget::videoPlayerWidget( const QString& filename, videoPlayerDevice::type deviceType, QWidget *parent ) :
    QWidget( parent ),
    player_( NULL ),
    disableUi_( false )
{
    setObjectName( "videoPlayerWidget" );

    // initialize
    initialize( deviceType );

    // play!
    play( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerWidget::~videoPlayerWidget()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "~videoPlayerWidget" );

    LOG_TRACE( "disconnecting slots...", NULL );
    disconnect();

    LOG_TRACE( "delete player...", NULL );
    deleteVideoObject( &player_ );

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool videoPlayerWidget::active() const
{
    return (player_ ? player_->active() : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize videoPlayerWidget::sizeHint() const
{
    return WIDGET_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::disableUserInterface()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "disableUserInterface" );

    // When the user interface is hidden we are to only show the video. It is a passive mode that
    // will play a video until the video completes or the application is terminated. Currently
    // only the gcd application uses it (in a particular configuration) in order to get the video
    // playback out of its own process space (i.e. due to memory leaks, etc).

    LOG_INFO( "hiding user interface...", NULL );
    disableUi_ = true;

    playPauseButton_->hide();
    stopButton_->hide();
    seekSlider_->hide();
    position_->hide();
    totalTime_->hide();
    volumeSlider_->hide();
    exitButton_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::pause()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "pause" );

    if ( !player_ )
        LOG_WARN( "player is NULL", NULL );
    else if ( player_->canPause() )
    {
        LOG_INFO( "pause button was pressed", NULL );
        player_->pause();

        togglePlayPauseButtonState( ForcePlay );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool videoPlayerWidget::play( const QString& filename )
{
    LOG_CLASS_INFO( "videoPlayerWidget", "play" );

    // validate active player
    if ( !player_ )
    {
        LOG_WARN( "player is NULL", NULL );
        return false;
    }

    LOG_INFO( "playing file '%s'...", qPrintable( filename ) );

    // make sure filename is valid
    if ( !filename.length() )
    {
        LOG_WARN( "bad filename '%s'!", qPrintable( filename ) );
        return false;
    }

    // times are not visible by default
    position_->hide();
    totalTime_->hide();

    // reset controls
    seekSlider_->setMediaObject( NULL );
    seekSlider_->setEnabled( false );

    volumeSlider_->setAudioOutput( NULL );
    volumeSlider_->setEnabled( false );

    LOG_TRACE( "loading...", NULL );

    if ( !player_->load( filename, false ) )
    {
        LOG_ERROR( "file '%s' did not load!", qPrintable( filename ) );
        return false;
    }

    LOG_TRACE( "setting up controls...", NULL );

    seekSlider_->setMediaObject( player_->source() );
    seekSlider_->setEnabled( player_->source() ? true : false );

    volumeSlider_->setAudioOutput( player_->audioSink() );
    volumeSlider_->setEnabled( player_->audioSink() ? true : false );

    togglePlayPauseButtonState( ForcePause );

    LOG_TRACE( "playing...", NULL );
    player_->play();

    LOG_TRACE( "done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::translate()
{
    playPauseButton_->setText( tr( PLAY_BUTTON_TEXT ) );
    stopButton_->setText( tr( STOP_BUTTON_TEXT ) );
    exitButton_->setText( tr( EXIT_BUTTON_TEXT ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onPlayPauseButtonPressed()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "onPlayPauseButtonPressed" );

    if ( !player_ )
        LOG_WARN( "player is NULL", NULL );
    else
    {
        if ( isPlayButtonState() )
        {
            LOG_INFO( "play button was pressed", NULL );
            player_->play();
        }
        else
        {
            LOG_INFO( "pause button was pressed", NULL );
            player_->pause();
        }

        togglePlayPauseButtonState();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onStopButtonPressed()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "onStopButtonPressed" );

    if ( !player_ )
        LOG_WARN( "player is NULL", NULL );
    else
    {
        LOG_INFO( "stop button was pressed", NULL );
        player_->stop();

        togglePlayPauseButtonState( ForcePlay );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onExitButtonPressed()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "onExitButtonPressed" );

    if ( !player_ )
        LOG_WARN( "player is NULL", NULL );
    else
    {
        LOG_INFO( "exit button was pressed", NULL );
        player_->stop();
    }

    Q_EMIT finished( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onFinished()
{
    LOG_CLASS_INFO( "videoPlayerWidget", "onFinished" );

    if ( !player_ )
        LOG_WARN( "player is NULL", NULL );
    else
    {
        LOG_INFO( "media has completed", NULL );
        player_->stop();

        togglePlayPauseButtonState( ForcePlay );
    }

    Q_EMIT finished( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onTick( qint64 time )
{
    if ( disableUi_ )
        return;

    QString s( makeReadableTime( time ) );

    if ( totalTime_->isVisible() )
        s += " /";

    position_->setText( s );
    position_->show();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::onTotalTimeChanged( qint64 newTotalTime )
{
    if ( disableUi_ )
        return;

    totalTime_->setText( makeReadableTime( newTotalTime ) );
    totalTime_->show();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::initialize( videoPlayerDevice::type deviceType )
{
    LOG_CLASS_INFO( "videoPlayerWidget", "initialize" );

    QVBoxLayout *layout = new QVBoxLayout( this );

    layout->setMargin( 0 );

    // set widget size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    ///////////////////////////////////////////////////////////////////////////
    // drop dma caches

#if defined( Q_OS_LINUX )

    // only need to do this on embedded platforms
    if ( platformType::EmbeddedLinux == PLATFORM )
    {
        LOG_DEBUG( "dropping cache table...", NULL );
        int rc( system( "echo 3 > /proc/sys/vm/drop_caches" ) );

        LOG_TRACE( "dropping cache table... done! rc %d", rc );
    }

#endif

    ///////////////////////////////////////////////////////////////////////////
    // setup video player widget

    player_ = videoPlayer::create( deviceType, this );

    connect( player_, SIGNAL( finished() ), SLOT( onFinished() ) );
    connect( player_, SIGNAL( hasVideoChanged(bool) ), SIGNAL( hasVideoChanged(bool) ) );
    connect( player_, SIGNAL( playbackError(QString) ), SIGNAL( playbackError(QString) ) );
    connect( player_, SIGNAL( tick(qint64) ), SLOT( onTick(qint64) ) );
    connect( player_, SIGNAL( totalTimeChanged(qint64) ), SLOT( onTotalTimeChanged(qint64) ) );

    layout->addWidget( player_ );

    ///////////////////////////////////////////////////////////////////////////
    // setup video control widgets

    QHBoxLayout *controls = new QHBoxLayout();

    // create the widgets
    playPauseButton_ = new QPushButton( style()->standardIcon( QStyle::SP_MediaPlay ), tr( PLAY_BUTTON_TEXT ), this );
    stopButton_ = new QPushButton( style()->standardIcon( QStyle::SP_MediaStop ), tr( STOP_BUTTON_TEXT ), this );
    seekSlider_ = new seekSlider( this );
    position_ = new QLabel( this );
    totalTime_ = new QLabel( this );
    volumeSlider_ = new volumeSlider( this );
    exitButton_ = new QPushButton( style()->standardIcon( QStyle::SP_DialogCloseButton ), tr( EXIT_BUTTON_TEXT ), this );

    playPauseButton_->setFixedHeight( BUTTON_HEIGHT );
    stopButton_->setFixedHeight( BUTTON_HEIGHT );
    seekSlider_->setFixedHeight( BUTTON_HEIGHT );
    volumeSlider_->setFixedHeight( BUTTON_HEIGHT );
    exitButton_->setFixedHeight( BUTTON_HEIGHT );

    foreach ( QObject *child, volumeSlider_->children() )
    {
        QPushButton *pushButton = NULL;
        QToolButton *toolButton = NULL;

        if ( (pushButton = dynamic_cast<QPushButton*>( child )) )
            pushButton->setFixedHeight( BUTTON_HEIGHT );
        else if ( (toolButton = dynamic_cast<QToolButton*>( child )) )
            toolButton->setFixedHeight( BUTTON_HEIGHT );
    }

    playPauseButton_->setIconSize( BUTTON_ICON_SIZE );
    stopButton_->setIconSize( BUTTON_ICON_SIZE );
    volumeSlider_->setIconSize( BUTTON_ICON_SIZE );
    exitButton_->setIconSize( BUTTON_ICON_SIZE );

    // nothing gets focus
    playPauseButton_->setFocusPolicy( Qt::NoFocus );
    stopButton_->setFocusPolicy( Qt::NoFocus );
    seekSlider_->setFocusPolicy( Qt::NoFocus );
    volumeSlider_->setFocusPolicy( Qt::NoFocus );
    exitButton_->setFocusPolicy( Qt::NoFocus );

    // times are not visible by default
    position_->hide();
    totalTime_->hide();

    // add them to the layout
    controls->addWidget( playPauseButton_ );
    controls->addWidget( stopButton_ );
    controls->addWidget( seekSlider_, 1 );
    controls->addWidget( position_ );
    controls->addWidget( totalTime_ );
    controls->addWidget( volumeSlider_ );
    controls->addWidget( exitButton_ );

    // connect up signals and slots
    connect( playPauseButton_, SIGNAL( pressed() ), SLOT( onPlayPauseButtonPressed() ) );
    connect( stopButton_, SIGNAL( pressed() ), SLOT( onStopButtonPressed() ) );
    connect( exitButton_, SIGNAL( pressed() ), SLOT( onExitButtonPressed() ) );

    layout->addLayout( controls );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool videoPlayerWidget::isPlayButtonState() const
{
    return (tr( PLAY_BUTTON_TEXT ) == playPauseButton_->text());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerWidget::togglePlayPauseButtonState( PlayPauseButtonState state )
{
    LOG_CLASS_INFO( "videoPlayerDialog", "togglePlayPauseButtonState" );

    if (( ForcePlay == state ) || (( Toggle == state ) && ( isPauseButtonState() )))
    {
        LOG_TRACE( "showing play button", NULL );

        playPauseButton_->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
        playPauseButton_->setText( tr( PLAY_BUTTON_TEXT ) );
        playPauseButton_->setEnabled( true );
    }
    else if (( ForcePause == state ) || (( Toggle == state ) && ( isPlayButtonState() )))
    {
        LOG_TRACE( "showing pause button", NULL );

        playPauseButton_->setIcon( style()->standardIcon( QStyle::SP_MediaPause ) );
        playPauseButton_->setText( tr( PAUSE_BUTTON_TEXT ) );

        if (( player_ ) && ( player_->canPause() ))
            playPauseButton_->setEnabled( true );
        else
        {
            LOG_WARN( "media cannot be paused", NULL );
            playPauseButton_->setEnabled( false );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString videoPlayerWidget::makeReadableTime( int msec )
{
    msec /= 1000;

    // hours
    QString hours = QString::number( msec / 3600 );
    msec %= 3600;

    if ( "0" == hours )
        hours.clear();

    // minutes
    QString minutes = QString::number( msec / 60 );
    msec %= 60;

    if ( hours.length() )
        if ( 1 == minutes.length() )
            minutes = "0" + minutes;

    // seconds
    QString seconds = QString::number( msec );

    if ( 1 == seconds.length() )
        seconds = "0" + seconds;

    if ( hours.length() )
        return hours + ":" + minutes + ":" + seconds;

    return minutes + ":" + seconds;
}
