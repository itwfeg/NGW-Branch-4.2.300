/**
 * @file playlistwidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include "mediaplaylist.h"

#include <QSignalMapper>
#include <QSslError>
#include <QString>
#include <QWidget>

class videoPlayer;

class QLayout;
class QNetworkReply;
class QTimer;
class QWebView;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for displaying a playlist.
class playlistWidget : public QWidget
{
    Q_OBJECT

    Q_ENUMS( displayType )
    Q_PROPERTY( QString currentPlaylist READ currentPlaylist WRITE setCurrentPlaylist RESET resetCurrentPlaylist )
    Q_PROPERTY( displayType display READ display WRITE setDisplay )
    Q_PROPERTY( QBrush imageBackground READ imageBackground WRITE setImageBackground )
    Q_PROPERTY( unsigned int imagePlaybackCounter READ imagePlaybackCounter )
    Q_PROPERTY( QBrush videoBackground READ videoBackground WRITE setVideoBackground )
    Q_PROPERTY( unsigned int videoPlaybackCounter READ videoPlaybackCounter )
    Q_PROPERTY( float volume READ volume WRITE setVolume )

    typedef playlistWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Playlist error.
    /**
     * This signal is emitted when a playlist error occurs.
     *
     * @param[in] file  file in error
     * @param[in] errorDescription  description of error
     */
    void playlistError(
        const QString& file,
        const QString& errorDescription );

public:

    /// Display types (for video playback).
    enum displayType
    {
        Operator,                                   ///< Operator display.
        Customer                                    ///< Customer display.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    playlistWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~playlistWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if active.
    /**
     * @return  @c true if active, @c false otherwise
     */
    virtual bool active() const;

    /// Retrieve current playlist.
    /**
     * @return  current playlist
     */
    virtual QString currentPlaylist() const {return currentPlaylist_;}

    /// Retrieve display type.
    /**
     * @return  display type
     */
    virtual displayType display() const {return display_;}

    /// Get image background brush.
    /**
     * @return  brush
     */
    virtual QBrush imageBackground() const {return imageBackground_;}

    /// Get number of image playbacks.
    /**
     * @return  number of images played since application started.
     */
    virtual unsigned int imagePlaybackCounter() const {return imagePlaybackCounter_;}

    /// Check if current playlist valid.
    /**
     * @return  @c true if playlist valid, @c false otherwise
     */
    virtual bool isCurrentPlaylistValid() const {return (0 < playbackList_.size());}

    /// Get video background brush.
    /**
     * @return  brush
     */
    virtual QBrush videoBackground() const {return videoBackground_;}

    /// Get number of video playbacks.
    /**
     * @return  number of videos played since application started.
     */
    virtual unsigned int videoPlaybackCounter() const {return videoPlaybackCounter_;}

    /// Retrieve volume.
    /**
     * @return  volue
     */
    virtual float volume() const {return volume_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset current playlist.
    virtual void resetCurrentPlaylist() {setCurrentPlaylist( QString() );}

    /// Translate widgets.
    virtual void translate();

public Q_SLOTS:

    /// Set current playlist.
    /**
     * @param[in] value  current playlist
     */
    virtual void setCurrentPlaylist( const QString& value );

    /// Disable widget.
    /**
     * @param[in] value  @c true to disable, @c false to enable
     */
    virtual void setDisabled( bool value );

    /// Set display type.
    /**
     * @param[in]  display type
     */
    virtual void setDisplay( displayType value ) {display_ = value;}

    /// Enable widget.
    /**
     * @param[in] value  @c true to enable, @c false to disable
     */
    virtual void setEnabled( bool value );

    /// Set image background brush.
    /**
     * @param[in] value  brush
     */
    virtual void setImageBackground( const QBrush& value ) {imageBackground_ = value;}

    /// Set video background brush.
    /**
     * @param[in] value  brush
     */
    virtual void setVideoBackground( const QBrush& value ) {videoBackground_ = value;}

    /// Set volume.
    /**
     * @param[in] value  volume
     */
    virtual void setVolume( float value ) {volume_ = value;}

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Play or show media on the GCD.
    /**
     * @param[in] m  media to play or show
     */
    virtual void playMedia( const media& m );

private Q_SLOTS:

    /// Slot for slideshow animation.
    void onAnimateSlideshowImage();

    /// Slot for browser load finished.
    void onBrowserLoadFinished( bool result );

    /// Slot for browser load progress.
    void onBrowserLoadProgress( int progress );

    /// Slot for browser load started.
    void onBrowserLoadStarted();

    /// Slot for browser net access finished.
    void onBrowserNetworkAccessManagerFinished( QNetworkReply *reply );

    /// Slot for browser net access ssl errors.
    void onBrowserNetworkAccessManagerSslErrors( QNetworkReply *reply, const QList<QSslError> &errors );

    /// Slot for playlist checking timer.
    void onCheckForPlaylistChanged();

    /// Slot for video available changed.
    void onHasVideoChanged( bool videoAvailable );

    /// Slot for playback errors.
    void onPlaybackError( const QString& desc );

    /// Slot for timer timeout.
    void onTimeout();

private:

    enum {TIMER_RESOLUTION = 1000};

    enum {PLAYLIST_TIMER_RESOLUTION = 60000};

    enum {ANIMATION_TIMER_RESOLUTION = 100};

    QBrush imageBackground_;
    QBrush videoBackground_;

    QString currentPlaylist_;
    QByteArray currentPlaylistHash_;

    float volume_;

    QTimer *timer_;
    QTimer *playlistTimer_;

    displayType display_;

    mediaPlaylist playbackList_;
    unsigned int playbackListIndex_;
    media nowPlaying_;

    unsigned int playbackTimer_;
    bool playbackError_;

    unsigned int imagePlaybackCounter_;
    unsigned int videoPlaybackCounter_;

    videoPlayer *customerVideo_;
    QString customerVideoFilename_;

    QWebView *browser_;

    QTimer *animationTimer_;
    unsigned int animationCounter_;

    unsigned int imageSlideLocation_;

    QLayout *layout_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Destroy old video player and make a new one.
    void createVideoPlayer( bool visible = true );

    /// Destroy old video player and make a new one, play video.
    bool createVideoPlayer( const QString& video, bool visible = true );

    /// Destroy video player.
    void destroyVideoPlayer();

    /// Destroy old browser and make a new one.
    void createBrowser();

    /// Destroy old browser and make a new one, browse to url.
    bool createBrowser( const QString& url );

    /// Destroy browser.
    void destroyBrowser();

    /// Load playlist.
    void loadPlaylist( const QString& playlist );

    /// Check media state.
    void updateMediaState( bool forceMediaChange = false );

    /// Play or show media of a specific type.
    template <mediaType::type T>
    void playMediaT( const media& m );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PLAYLISTWIDGET_H
