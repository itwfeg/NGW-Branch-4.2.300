/**
 * @file abstractmediaplayer.h
 * Abstract media player interface.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ABSTRACTMEDIAPLAYER_H
#define ABSTRACTMEDIAPLAYER_H

#include <QObject>
#include <QRect>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract media player interface.
class abstractMediaPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool audioAvailable READ isAudioAvailable NOTIFY audioAvailableChanged )
    Q_PROPERTY( qint64 duration READ duration NOTIFY durationChanged )
    Q_PROPERTY( QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged )
    Q_PROPERTY( MediaStatus mediaStatus READ mediaStatus NOTIFY mediaStatusChanged )
    Q_PROPERTY( MediaType mediaType READ mediaType )
    Q_PROPERTY( bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged )
    Q_PROPERTY( qint64 position READ position WRITE setPosition NOTIFY positionChanged )
    Q_PROPERTY( bool seekable READ isSeekable NOTIFY seekableChanged )
    Q_PROPERTY( State state READ state NOTIFY stateChanged )
    Q_PROPERTY( bool videoAvailable READ isVideoAvailable NOTIFY videoAvailableChanged )
    Q_PROPERTY( bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged )
    Q_PROPERTY( int volume READ volume WRITE setVolume NOTIFY volumeChanged )

    Q_ENUMS( State )
    Q_ENUMS( MediaStatus )
    Q_ENUMS( MediaType )

    typedef abstractMediaPlayer _Myt;
    typedef QObject _Mybase;

public:

    /// Player states.
    enum State
    {
        StoppedState,
        PlayingState,
        PausedState
    };

    /// Media status.
    enum MediaStatus
    {
        UnknownMediaStatus,
        NoMedia,
        LoadingMedia,
        LoadedMedia,
        StalledMedia,
        BufferingMedia,
        BufferedMedia,
        EndOfMedia,
        InvalidMedia
    };

    /// Media type.
    enum MediaType
    {
        UnknownMediaType,
        Audio,
        Video
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~abstractMediaPlayer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve media duration.
    /**
     * @return  duration
     */
    virtual qint64 duration() const = 0;

    /// Retrieve video window geometry.
    /**
     * @return  window geometry
     */
    virtual QRect geometry() const = 0;

    /// Retrieve audio available.
    /**
     * @return  @c true if audio available, @c false otherwise
     */
    virtual bool isAudioAvailable() const = 0;

    /// Retrieve audio muted.
    /**
     * @return  @c true if audio muted, @c false otherwise
     */
    virtual bool isMuted() const = 0;

    /// Retrieve media seekable.
    /**
     * @return  @c true if media seekable, @c false otherwise
     */
    virtual bool isSeekable() const = 0;

    /// Retrieve video available.
    /**
     * @return  @c true if video available, @c false otherwise
     */
    virtual bool isVideoAvailable() const = 0;

    /// Retrieve player visible.
    /**
     * @return  @c true if player visible, @c false otherwise
     */
    virtual bool isVisible() const = 0;

    /// Retrieve media status.
    /**
     * @return  status
     */
    virtual MediaStatus mediaStatus() const = 0;

    /// Retrieve media type.
    /**
     * @return  type
     */
    virtual MediaType mediaType() const = 0;

    /// Retrieve media position.
    /**
     * @return  position
     */
    virtual qint64 position() const = 0;

    /// Retrieve media state.
    /**
     * @return  state
     */
    virtual State state() const = 0;

    /// Retrieve media volume.
    /**
     * @return  volume
     */
    virtual int volume() const = 0;

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set video window geometry.
    /**
     * @param[in] rect  window geometry
     */
    virtual void setGeometry( const QRect &rect ) = 0;

    /// Set audio muted.
    /**
     * @param[in] muted  @c true to mute audio, @c false otherwise
     */
    virtual void setMuted( bool muted ) = 0;

    /// Set media position.
    /**
     * @param[in] position  new position
     */
    virtual void setPosition( qint64 position ) = 0;

    /// Set player visible.
    /**
     * @param[in] visible  @c true to show, @c false to hide
     */
    virtual void setVisible( bool visible ) = 0;

    /// Set media volume.
    /**
     * @param[in] volume  volume
     */
    virtual void setVolume( int volume ) = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Hide player.
    virtual void hide() = 0;

    /// Load media.
    /**
     * @param[in] filename  media filename
     * @param[in] type  media type
     */
    virtual void load( const QString &filename, abstractMediaPlayer::MediaType type ) = 0;

    /// Pause media.
    virtual void pause() = 0;

    /// Play media.
    virtual void play() = 0;

    /// Show player.
    virtual void show() = 0;

    /// Stop media.
    virtual void stop() = 0;

Q_SIGNALS:

    /// Emitted when audio available changes.
    /**
     * @param[in] available  @c true if audio available, @c false otherwise
     */
    void audioAvailableChanged( bool available );

    /// Emitted when duration changes.
    /**
     * @param[in] duration  duration
     */
    void durationChanged( qint64 duration );

    /// Emitted upon error.
    /**
     * @param[in] error  error description
     */
    void error( const QString &error );

    /// Emitted when media finishes.
    void finished();

    /// Emitted when video window geometry changes.
    /**
     * @param[in] rect  window geometry
     */
    void geometryChanged( const QRect &rect );

    /// Emitted when media status changes.
    /**
     * @param[in] status  status
     */
    void mediaStatusChanged( abstractMediaPlayer::MediaStatus status );

    /// Emitted when audio muted changes.
    /**
     * @param[in] muted  @c true when muted, @c false otherwise
     */
    void mutedChanged( bool muted );

    /// Emitted when position changes.
    /**
     * @param[in] position  position
     */
    void positionChanged( qint64 position );

    /// Emitted when media seekable changes.
    /**
     * @param[in] value  @c true if media is seekable, @c false otherwise
     */
    void seekableChanged( bool value );

    /// Emitted when state changes.
    /**
     * @param[in] newState  new state
     */
    void stateChanged( abstractMediaPlayer::State newState );

    /// Emitted when video available changes.
    /**
     * @param[in] videoAvailable  @c true if video available, @c false otherwise
     */
    void videoAvailableChanged( bool videoAvailable );

    /// Emitted when visibility changes.
    /**
     * @param[in] visible  @c true if visible, @c false otherwise
     */
    void visibleChanged( bool visible );

    /// Emitted when volume changes.
    /**
     * @param[in] volume  volume
     */
    void volumeChanged( int volume );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    abstractMediaPlayer( QObject *parent = NULL );

private:

    // not implemented
    _Myt& operator = ( const _Myt &value );

};

Q_DECLARE_METATYPE( abstractMediaPlayer::State )
Q_DECLARE_METATYPE( abstractMediaPlayer::MediaStatus )
Q_DECLARE_METATYPE( abstractMediaPlayer::MediaType )

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTMEDIAPLAYER_H
