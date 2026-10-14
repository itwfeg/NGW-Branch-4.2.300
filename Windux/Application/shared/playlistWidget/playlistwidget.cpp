/**
 * @file playlistwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "mediaplaylistfileparser.h"
#include "playlistwidget.h"

#include <filesystem.h>
#include <logmanager.h>

#ifndef DISABLE_VIDEO
#include <videoplayer/videoplayer.h>
#endif

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QNetworkReply>
#include <QPainter>
#include <QSignalMapper>
#include <QTimer>

#ifndef DISABLE_WEB
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
playlistWidget::playlistWidget( QWidget *parent ) :
    _Mybase( parent ),
    imageBackground_( Qt::black ),
    videoBackground_( Qt::black ),
    volume_( 1.0 ),
    display_( Customer ),
    playbackTimer_( 0 ),
    playbackError_( false ),
    imagePlaybackCounter_( 0 ),
    videoPlaybackCounter_( 0 ),
    customerVideo_( NULL ),
    browser_( NULL ),
    animationCounter_( 0 ),
    imageSlideLocation_( 0 )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
playlistWidget::~playlistWidget()
{
    destroyVideoPlayer();
    destroyBrowser();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool playlistWidget::active() const
{
    // we are active when visible, enabled, and have a valid playlist
    return (( isVisible() ) && ( isEnabled() ) && ( isCurrentPlaylistValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::translate()
{
    // nothing to do
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::setCurrentPlaylist( const QString& value )
{
    // null/empty playlist
    if ( value.isEmpty() )
        loadPlaylist( value );
    else
    {
        QString filename( filesystem::absoluteFilePath( value, folderType::Playlists ) );

        // same playlist as current... lets check to see if something changed...
        if ( currentPlaylist_ == filename )
        {
            onCheckForPlaylistChanged();
            return;
        }

        loadPlaylist( filename );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::setDisabled( bool value )
{
#ifdef DISABLE_VIDEO
    _Mybase::setDisabled( value );
#else
    bool wasActive( active() );

    // invoke base method
    _Mybase::setDisabled( value );

    // update state: active -> inactive
    if (( wasActive ) && ( !active() ))
    {
        if ( customerVideo_ )
        {
            customerVideo_->pause();

            if ( customerVideo_->hasVideo() )
                customerVideo_->hide();
        }
    }
    // update state: inactive -> active
    else if (( !wasActive ) && ( active() ))
    {
        if ( customerVideo_ )
        {
            if ( customerVideo_->hasVideo() )
                customerVideo_->show();

            customerVideo_->play();
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::setEnabled( bool value )
{
#ifdef DISABLE_VIDEO
    _Mybase::setEnabled( value );
#else
    bool wasActive( active() );

    // invoke base method
    _Mybase::setEnabled( value );

    // update state: active -> inactive
    if (( wasActive ) && ( !active() ))
    {
        if ( customerVideo_ )
        {
            customerVideo_->pause();

            if ( customerVideo_->hasVideo() )
                customerVideo_->hide();
        }
    }
    // update state: inactive -> active
    else if (( !wasActive ) && ( active() ))
    {
        if ( customerVideo_ )
        {
            if ( customerVideo_->hasVideo() )
                customerVideo_->show();

            customerVideo_->play();
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );

    // update state
    destroyVideoPlayer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "playlistWidget", "resizeEvent" );

    _Mybase::resizeEvent( e );

    // update state
    if ( active() )
    {
        // Note:
        // When resize occurs our media needs to be resized as well... this does not happen
        // automatically in real time. So we must force a media change in order to resize the new
        // media to new dimensions.

        LOG_DEBUG( "forcing update media state", NULL );
        updateMediaState( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "playlistWidget", "showEvent" );

    _Mybase::showEvent( e );

    // update state
    if ( active() )
    {
        LOG_DEBUG( "update media state", NULL );
        updateMediaState(true);			//	FT-1642 (Blue screen on GCD)
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void playlistWidget::playMediaT<mediaType::Video>( const media& m )
{
    LOG_CLASS_INFO( "playlistWidget", "playMediaT" );

    QString video( m.filename() );

    LOG_INFO( "playing customer video '%s'", qPrintable( video ) );

    destroyBrowser();

    // reset background to black
    QPalette p( palette() );
    p.setBrush( backgroundRole(), videoBackground_ );
    setPalette( p );

    // reset playback error flag
    playbackError_ = false;

    // play the video!
    if ( !createVideoPlayer( video ) )
        Q_EMIT playlistError( video, "Error loading video '" + video + "'" );

    ++videoPlaybackCounter_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void playlistWidget::playMediaT<mediaType::Image>( const media& m )
{
    LOG_CLASS_INFO( "playlistWidget", "playMediaT" );

    QString imageFilename( m.filename() );
    textOverlay overlay( m.overlay() );

    QString sound( m.soundFilename() );

    LOG_INFO( "showing customer image '%s' with background sound '%s' duration %d ms",
        qPrintable( imageFilename ),
        qPrintable( sound ),
        m.duration() );

    destroyBrowser();

    // FT-1164
    // instead of handling missing images when we create the playlist (by inserting placeholder) we
    // are now handling it as we process the playlist
    if ( !QFile::exists( imageFilename ) )
        imageFilename = mediaPlaylistFileParser::missingMediaPlaceholderFilename();

    // load image from file
    QPixmap image( imageFilename );

    // FT-761
    if ( image.isNull() )
        Q_EMIT playlistError( imageFilename, "Error loading slideshow image '" + imageFilename + "'" );

    // check dimensions
    if ( image.size() != size() )
    {
        LOG_DEBUG( "dimensions (%d, %d) of image '%s' do not match display size (%d, %d)",
            image.width(),
            image.height(),
            qPrintable( imageFilename ),
            width(),
            height() );
    }

    // draw image
    QPixmap canvas( size() );
    canvas.fill( Qt::transparent );

    QPainter painter( &canvas );

    // fill with background color
    painter.fillRect( canvas.rect(), imageBackground_ );

    // scale image as per aspect ratio
    if ( Qt::IgnoreAspectRatio != m.aspectRatio() )
        image = image.scaled( canvas.size(), m.aspectRatio() );

    // center image
    QRect target( QPoint( 0, 0 ), image.size() );

    if ( image.width() < canvas.width() )
        target.moveLeft( (canvas.width() - image.width()) / 2 );

    if ( image.height() < canvas.height() )
        target.moveTop( (canvas.height() - image.height()) / 2 );

    painter.drawPixmap( target, image, image.rect() );

    // show the text overlay
    if ( overlay.text().length() )
    {
        // Note:
        // The "Qt::TextWordWrap" flag will tell the drawText() function to find newline characters
        // and split the text into multiple lines.
        int flags( overlay.alignment() | Qt::TextWordWrap );

        painter.setFont( overlay.font() );
        painter.setPen( overlay.color() );

        painter.drawText( QRect( QPoint( 0, 0 ), canvas.size() ), flags, overlay.text() );
    }

    // save the rendering
    nowPlaying_.setRendering( canvas );

    // animate image!
    animationTimer_->setSingleShot( false );
    animationTimer_->start( ANIMATION_TIMER_RESOLUTION );
    animationCounter_ = 0;

    // stop sound when nothing should be playing
    if ( !sound.length() )
        destroyVideoPlayer();

    // play the sound (if different from previous)
    else if ( sound != customerVideoFilename_ )
    {
        // reset playback error flag
        playbackError_ = false;

        // play the audio!
        if ( !createVideoPlayer( sound, false ) )
            Q_EMIT playlistError( sound, "Error loading audio file '" + sound + "'" );
    }

    ++imagePlaybackCounter_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void playlistWidget::playMediaT<mediaType::Web>( const media& m )
{
    LOG_CLASS_INFO( "playlistWidget", "playMediaT" );

    QString url( m.filename() );

    LOG_INFO( "browsing to url '%s'", qPrintable( url ) );

    destroyVideoPlayer();

    // reset background to black
    QPalette p( palette() );
    p.setBrush( backgroundRole(), videoBackground_ );
    setPalette( p );

    // browse
    if ( !createBrowser( url ) )
        Q_EMIT playlistError( url, "Error browsing to url '" + url + "'" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::playMedia( const media& m )
{
    LOG_CLASS_INFO( "playlistWidget", "playMedia" );

    // we have a new media
    nowPlaying_ = m;
    LOG_TRACE( "We have new media.", NULL );

    // display the media (handle by type)
    switch ( m.type() )
    {
    //-------------------------------------------------------------------------
    case mediaType::Video:
    	LOG_TRACE( "mediaType::Video", NULL );
        playMediaT<mediaType::Video>( m );
        break;

    //-------------------------------------------------------------------------
    case mediaType::Image:
    	LOG_TRACE( "mediaType::Image", NULL );
        playMediaT<mediaType::Image>( m );
        break;

    //-------------------------------------------------------------------------
    case mediaType::Web:
    	LOG_TRACE( "mediaType::Web", NULL );
        playMediaT<mediaType::Web>( m );
        break;

    //-------------------------------------------------------------------------
    default:
        LOG_WARN( "unknown media type %d", m.type() );
        break;
    }

    // log out how many playbacks
    LOG_INFO( "image playbacks %d, video playbacks %d", imagePlaybackCounter_, videoPlaybackCounter_ );

    // reset timer since new media
    playbackTimer_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onAnimateSlideshowImage()
{
    QPalette p( palette() );
    QPixmap image( nowPlaying_.rendering() );

    // tick our counter
    ++animationCounter_;

    // slide in from corner
    if ( 1 == nowPlaying_.animation() )
    {
        bool doneAnimating( 10 == animationCounter_ );

        QPixmap canvas( image.size() );
        canvas.fill( Qt::transparent );

        QPainter painter( &canvas );

        // render previous image
        if ( !doneAnimating )
            painter.fillRect( image.rect(), p.brush( backgroundRole() ) );

        // calculate offset for new image
        QPoint pt( 0, 0 );

        int xOff( (animationCounter_ * image.width()) / 10 );
        int yOff( (animationCounter_ * image.height()) / 10 );

        if ( 0 == (imageSlideLocation_ % 4) )
        {
            pt.setX( -image.width() + xOff );
            pt.setY( -image.height() + yOff );
        }
        else if ( 1 == (imageSlideLocation_ % 4) )
        {
            pt.setX( image.width() - xOff );
            pt.setY( -image.height() + yOff );
        }
        else if ( 2 == (imageSlideLocation_ % 4) )
        {
            pt.setX( -image.width() + xOff );
            pt.setY( image.height() - yOff );
        }
        else if ( 3 == (imageSlideLocation_ % 4) )
        {
            pt.setX( image.width() - xOff );
            pt.setY( image.height() - yOff );
        }

        // render new image
        painter.drawPixmap( pt.x(), pt.y(), image );

        // show the image
        p.setBrush( backgroundRole(), canvas );

        // done animating
        if ( 10 == animationCounter_ )
        {
            animationTimer_->stop();
            ++imageSlideLocation_;
        }
    }

    // no animation
    else
    {
        // show the image
        p.setBrush( backgroundRole(), image );

        // stop animating
        animationTimer_->stop();
    }

    // display
    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onBrowserLoadFinished( bool result )
{
    LOG_CLASS_INFO( "playlistWidget", "onBrowserLoadFinished" );

    if ( result )
        LOG_INFO( "browser load success", NULL );
    else if ( browser_ )
    {
        LOG_WARN( "browser load failed: '%s'", qPrintable( nowPlaying_.filename() ) );

        Q_EMIT playlistError( nowPlaying_.filename(), "page failed to load" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onBrowserLoadProgress( int progress )
{
    LOG_CLASS_INFO( "playlistWidget", "onBrowserLoadProgress" );

    LOG_DEBUG( "browser load progress %d", progress );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onBrowserLoadStarted()
{
    LOG_CLASS_INFO( "playlistWidget", "onBrowserLoadStarted" );

    LOG_DEBUG( "browser load started", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onBrowserNetworkAccessManagerFinished( QNetworkReply *reply )
{
    LOG_CLASS_INFO( "playlistWidget", "onBrowserNetworkAccessManagerFinished" );

    if (( !reply ) || ( QNetworkReply::NoError == reply->error() ))
        return;

    LOG_WARN( "error loading page: '%s'", qPrintable( reply->errorString() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onBrowserNetworkAccessManagerSslErrors( QNetworkReply *reply, const QList<QSslError> &errors )
{
    LOG_CLASS_INFO( "playlistWidget", "onBrowserNetworkAccessManagerSslErrors" );

    if (( !reply ) || ( QNetworkReply::NoError == reply->error() ))
        return;

    LOG_WARN( "error loading page: '%s'", qPrintable( reply->errorString() ) );

    foreach ( const QSslError& error, errors )
        LOG_WARN( "  ssl error '%s'", qPrintable( error.errorString() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onCheckForPlaylistChanged()
{
    LOG_CLASS_INFO( "playlistWidget", "onCheckForPlaylistChanged" );

    // no playlist, nothing to do
    if ( currentPlaylist_.isEmpty() )
        return;

    QFile playlist( currentPlaylist_ );
    bool different( false );

    LOG_TRACE( "checking playlist for changes...", NULL );

    // check playlist
    if ( playlist.open( QIODevice::ReadOnly ) )
    {
        different = (currentPlaylistHash_ != QCryptographicHash::hash( playlist.readAll(), QCryptographicHash::Md5 ));
        playlist.close();
    }

    // playlist has changed!
    if ( different )
    {
        LOG_INFO( "playlist contents have changed!", NULL );
        loadPlaylist( currentPlaylist_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onHasVideoChanged( bool videoAvailable )
{
#ifndef DISABLE_VIDEO
    // when video comes available make sure widget is visible
    if ( videoAvailable )
        if ( customerVideo_ )
            customerVideo_->setVisible( true );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onPlaybackError( const QString& desc )
{
    LOG_CLASS_INFO( "playlistWidget", "onPlaybackError" );

    LOG_WARN( "error during playback '%s'", qPrintable( desc ) );
    playbackError_ = true;

    Q_EMIT playlistError( nowPlaying_.filename(), desc );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::onTimeout()
{
    LOG_CLASS_INFO( "playlistWidget", "onTimeout" );

    // nothing to do
    if ( !active() )
        return;

    // increment timer
    playbackTimer_ += TIMER_RESOLUTION;

    // update media state
    LOG_TRACE( "update media state", NULL );
    updateMediaState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::initialize()
{
    setAutoFillBackground( true );

    timer_ = new QTimer( this );
    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    playlistTimer_ = new QTimer( this );
    connect( playlistTimer_, SIGNAL( timeout() ), SLOT( onCheckForPlaylistChanged() ) );

    animationTimer_ = new QTimer( this );
    animationTimer_->setInterval( ANIMATION_TIMER_RESOLUTION );
    connect( animationTimer_, SIGNAL( timeout() ), SLOT( onAnimateSlideshowImage() ) );

    // start timers
    timer_->start( TIMER_RESOLUTION );
    playlistTimer_->start( PLAYLIST_TIMER_RESOLUTION );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::createLayout()
{
    layout_ = new QHBoxLayout( this );
    layout_->setContentsMargins( QMargins() );
    layout_->setSpacing( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::createVideoPlayer( bool visible )
{
#ifdef DISABLE_VIDEO
    customerVideo_ = NULL;
#else
    LOG_CLASS_INFO( "playlistWidget", "createVideoPlayer" );

    // dump previous video player
    destroyVideoPlayer();

    LOG_TRACE( "allocate video player...", NULL );

    if ( Operator == display() )
        customerVideo_ = videoPlayer::create( videoPlayerDevice::Operator, this );
    else
        customerVideo_ = videoPlayer::create( videoPlayerDevice::Customer, this );

    customerVideo_->setVisible( visible );
    customerVideo_->setVolume( volume_ );

    // add widget to layout
    layout_->addWidget( customerVideo_ );

    connect( customerVideo_, SIGNAL( hasVideoChanged(bool) ), SLOT( onHasVideoChanged(bool) ) );
    connect( customerVideo_, SIGNAL( playbackError( QString ) ), SLOT( onPlaybackError( QString ) ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool playlistWidget::createVideoPlayer( const QString& video, bool visible )
{
#ifdef DISABLE_VIDEO
    return false;
#else
    createVideoPlayer( visible );

    // play the video!
    return (( customerVideo_ ) && ( customerVideo_->load( customerVideoFilename_ = video ) ));
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::destroyVideoPlayer()
{
#ifndef DISABLE_VIDEO
    LOG_CLASS_INFO( "playlistWidget", "destroyVideoPlayer" );

    if ( !customerVideo_ )
        return;

    customerVideo_->close();
    customerVideoFilename_.clear();

    LOG_TRACE( "delete video player...", NULL );
    delete customerVideo_;
#endif

    customerVideo_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::createBrowser()
{
#ifdef DISABLE_WEB
    browser_ = NULL;
#else
    LOG_CLASS_INFO( "playlistWidget", "createBrowser" );

    // dump previous browser
    destroyBrowser();

    LOG_TRACE( "allocate browser...", NULL );
    browser_ = new QWebView( this );
    browser_->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
    browser_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
    browser_->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
    browser_->settings()->setAttribute( QWebSettings::JavaEnabled, true );
    browser_->settings()->setAttribute( QWebSettings::AutoLoadImages, true );

    // add widget to layout
    layout_->addWidget( browser_ );

    connect( browser_, SIGNAL( loadFinished(bool) ), SLOT( onBrowserLoadFinished(bool) ) );
    connect( browser_, SIGNAL( loadProgress(int) ), SLOT( onBrowserLoadProgress(int) ) );
    connect( browser_, SIGNAL( loadStarted() ), SLOT( onBrowserLoadStarted() ) );

    connect( browser_->page()->networkAccessManager(), SIGNAL( finished(QNetworkReply*) ), SLOT( onBrowserNetworkAccessManagerFinished(QNetworkReply*) ) );
    connect( browser_->page()->networkAccessManager(), SIGNAL( sslErrors(QNetworkReply*,QList<QSslError>) ), SLOT( onBrowserNetworkAccessManagerSslErrors(QNetworkReply*,QList<QSslError>) ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool playlistWidget::createBrowser( const QString& url )
{
#ifdef DISABLE_WEB
    return false;
#else
    createBrowser();

    if ( !browser_ )
        return false;

    // browse to url!
    browser_->load( QUrl( url ) );

    browser_->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    browser_->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );

    return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::destroyBrowser()
{
#ifndef DISABLE_WEB
    LOG_CLASS_INFO( "playlistWidget", "destroyBrowser" );

    if ( !browser_ )
        return;

    LOG_TRACE( "delete browser...", NULL );
    delete browser_;
#endif

    browser_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::loadPlaylist( const QString& playlistFile )
{
    LOG_CLASS_INFO( "playlistWidget", "loadPlaylist" );

    LOG_INFO( "loading playlist '%s'", qPrintable( playlistFile ) );
    QStringList missing;

    // empty (null) playlist
    if ( playlistFile.isEmpty() )
    {
        destroyVideoPlayer();
        destroyBrowser();

        currentPlaylist_.clear();
        currentPlaylistHash_.clear();

        playbackList_.clear();
        playbackListIndex_ = 0;
    }

    // parse list
    else if ( !mediaPlaylistFileParser::parse( playlistFile, playbackList_, missing ) )
    {
        LOG_WARN( "error loading playlist '%s'!", qPrintable( playlistFile ) );

        Q_EMIT playlistError( playlistFile, "Error loading playlist file '" + playlistFile + "'" );
    }

    else
    {
        QString readable;

        QFile playlist( playlistFile );
        playlist.open( QIODevice::ReadOnly );

        // save off playlist filename and hash
        currentPlaylist_ = playlistFile;
        currentPlaylistHash_ = QCryptographicHash::hash( playlist.readAll(), QCryptographicHash::Md5 );

        foreach ( char c, currentPlaylistHash_ )
            readable.append( QString::number( (unsigned char) c, 16 ) );

        LOG_DEBUG( "playlist hash is %s", qPrintable( readable ) );

        // start at beginning of new playlist
        playbackListIndex_ = 0;

        // handle missing media
        if ( missing.size() )
        {
            missing.removeDuplicates();

            // format up list of missing files
            QString description( "The following media files are missing from the system:\n" );

            foreach ( const QString& filename, missing )
                description += filename + "\n";

            Q_EMIT playlistError( playlistFile, description );
        }
    }

    // update background image
    if ( isCurrentPlaylistValid() )
    {
        if ( active() )
        {
            LOG_DEBUG( "forcing update media state", NULL );
            updateMediaState( true );
        }
    }
    else
    {
        QPalette p( palette() );
        p.setBrush( backgroundRole(), Qt::transparent );
        setPalette( p );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void playlistWidget::updateMediaState( bool forceMediaChange )
{
    LOG_CLASS_INFO( "playlistWidget", "updateMediaState" );

    // This method performs two actions:
    // 1) Checks currently active media, determines if media change is required.
    // 2) If media change is required, retrieves next media and makes it active.

    // upon playback error we need to force a media change
    if ( playbackError_ )
    {
        LOG_DEBUG( "forcing media change due to playback error", NULL );
        forceMediaChange = true;

        playbackError_ = false;
    }

    // not changing states, so check timers
    if ( mediaType::Video == nowPlaying_.type() )
    {
#ifndef DISABLE_VIDEO
        // check for customer video active
        if (( customerVideo_ ) && ( customerVideo_->active() ))
        {
            // check media to make sure it has not been playing longer than the upper limit
            if (( forceMediaChange ) || ( playbackTimer_ >= nowPlaying_.duration() ))
            {
                int attempts( 0 );

                // try to gracefully stop the video

                do
                {
                    LOG_WARN( "media has played longer than allowed (force %d), stopping (%d)...", forceMediaChange, attempts );
                    customerVideo_->stop();

                } while (( customerVideo_->active() ) && ( ++attempts < 3 ));

                // unresponsive video player... forcefully stop the video
                if ( customerVideo_->active() )
                {
                    LOG_ERROR( "media not stopping gracefully, killing player...", NULL );
                    destroyVideoPlayer();
                }
            }

            // video is active, nothing to do
            else
            {
                LOG_TRACE( "video is active, nothing to do", NULL );
                return;
            }
        }
#endif
    }
    else if ( mediaType::Image == nowPlaying_.type() )
    {
        // check media see if we should loop (i.e. replay) it
        if (( !forceMediaChange ) && ( playbackTimer_ < nowPlaying_.duration() ))
        {
#ifndef DISABLE_VIDEO
            // loop background music that has ended
            if (( customerVideo_ ) && ( !customerVideo_->active() ))
            {
                QString sound( nowPlaying_.soundFilename() );

                if ( sound.length() )
                {
                    LOG_INFO( "looping current media", NULL );

                    if ( !createVideoPlayer( sound, false ) )
                        Q_EMIT playlistError( sound, "Error loading audio file '" + sound + "'" );
                }
            }
#endif
            LOG_TRACE( "image is active, nothing to do", NULL );
            return;
        }
    }
    else if ( mediaType::Web == nowPlaying_.type() )
    {
        // check playback timer for media change
        if (( !forceMediaChange ) &&
            (( 0 == nowPlaying_.duration() ) || ( playbackTimer_ < nowPlaying_.duration() )))
        {
            LOG_TRACE( "web is active, nothing to do", NULL );
            return;
        }
    }

    // grab next media
    LOG_DEBUG( "current media is index %d", playbackListIndex_ );
    media next( playbackList_.at( playbackListIndex_++ ) );

    // move index back to zero if exceeded list size
    if ( playbackListIndex_ >= (unsigned int) playbackList_.size() )
        playbackListIndex_ = 0;

    // play the media
    LOG_DEBUG( "playing new media...", NULL );
    playMedia( next );
}
