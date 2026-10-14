/**
 * @file dbusmediaplayerclient.h
 * Front end interface for DBUS media player.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBUSMEDIAPLAYERCLIENT_H
#define DBUSMEDIAPLAYERCLIENT_H

#include "dbusmediaplayer.h"

#include <QVariant>

class QDBusInterface;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Front end interface for DBUS media player.
/**
 * This class allows interfacing DBUS @c abstractMediaPlayer objects.
 */
class dbusMediaPlayerClient : public dbusMediaPlayer
{
    Q_OBJECT

    Q_PROPERTY( bool valid READ isValid )

    typedef dbusMediaPlayerClient _Myt;
    typedef dbusMediaPlayer _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] service  dbus service name
     * @param[in] parent  parent object
     */
    dbusMediaPlayerClient( const QString &service, QObject *parent = NULL );

    /// Destructor.
    virtual ~dbusMediaPlayerClient();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve DBUS service name.
    /**
     * @return  service name
     */
    virtual QString dbusServiceName() const {return serviceName_;}

    /// Retrieve media duration.
    /**
     * @return  duration
     */
    virtual qint64 duration() const;

    /// Retrieve video window geometry.
    /**
     * @return  window geometry
     */
    virtual QRect geometry() const;

    /// Retrieve audio available.
    /**
     * @return  @c true if audio available, @c false otherwise
     */
    virtual bool isAudioAvailable() const;

    /// Retrieve audio muted.
    /**
     * @return  @c true if audio muted, @c false otherwise
     */
    virtual bool isMuted() const;

    /// Retrieve media seekable.
    /**
     * @return  @c true if media seekable, @c false otherwise
     */
    virtual bool isSeekable() const;

    /// Validate object.
    /**
     * @return  @c true if object valid, @c false otherwise
     */
    virtual bool isValid() const;

    /// Retrieve video available.
    /**
     * @return  @c true if video available, @c false otherwise
     */
    virtual bool isVideoAvailable() const;

    /// Retrieve player visible.
    /**
     * @return  @c true if player visible, @c false otherwise
     */
    virtual bool isVisible() const;

    /// Retrieve media status.
    /**
     * @return  status
     */
    virtual MediaStatus mediaStatus() const;

    /// Retrieve media type.
    /**
     * @return  type
     */
    virtual MediaType mediaType() const;

    /// Retrieve media position.
    /**
     * @return  position
     */
    virtual qint64 position() const;

    /// Retrieve media state.
    /**
     * @return  state
     */
    virtual State state() const;

    /// Retrieve media volume.
    /**
     * @return  volume
     */
    virtual int volume() const;

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set video window geometry.
    /**
     * @param[in] rect  window geometry
     */
    virtual void setGeometry( const QRect &rect );

    /// Set audio muted.
    /**
     * @param[in] muted  @c true to mute audio, @c false otherwise
     */
    virtual void setMuted( bool muted );

    /// Set media position.
    /**
     * @param[in]  position
     */
    virtual void setPosition( qint64 position );

    /// Set player visible.
    /**
     * @param[in] visible  @c true to show, @c false to hide
     */
    virtual void setVisible( bool visible );

    /// Set media volume.
    /**
     * @return  volume
     */
    virtual void setVolume( int volume );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Hide player.
    virtual void hide();

    /// Load media.
    /**
     * @param[in] filename  media filename
     * @param[in] type  media type
     */
    virtual void load( const QString &filename, abstractMediaPlayer::MediaType type );

    /// Pause media.
    virtual void pause();

    /// Play media.
    virtual void play();

    /// Show player.
    virtual void show();

    /// Stop media.
    virtual void stop();

Q_SIGNALS:

    /// Emitted upon DBUS error.
    /**
     * @param[in] error  error description
     */
    void busError( const QString &error );

protected:

    QString serviceName_;                           ///< DBUS service name.

    QDBusInterface *props_;                         ///< DBUS properties interface.
    QDBusInterface *object_;                        ///< DBUS object.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Invoke DBUS method.
    /**
     * @param[in] method  method name
     * @param[in] arg1  argument
     * @param[in] arg2  argument
     */
    virtual void call( const QString &method, const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant() );

    /// Retrieve DBUS property.
    /**
     * @param[in] prop  property name
     * @return  value
     */
    template <typename T>
    T getPropValue( const QString &prop );

    /// Set DBUS property.
    /**
     * @param[in] prop  property name
     * @param[in] value  value
     * @return  value
     */
    template <typename T>
    void setPropValue( const QString &prop, const T &value );

private Q_SLOTS:

    /// Slot for audio available changed.
    void onAudioAvailableChanged( bool available );

    /// Slot for duration changed.
    void onDurationChanged( qint64 duration );

    /// Slot for error.
    void onError( const QString &error );

    /// Slot for finished.
    void onFinished();

    /// Slot for geometry changed.
    void onGeometryChanged( const QRect &duration );

    /// Slot for media status changed.
    void onMediaStatusChanged( abstractMediaPlayer::MediaStatus status );

    /// Slot for muted changed.
    void onMutedChanged( bool muted );

    /// Slot for position changed.
    void onPositionChanged( qint64 position );

    /// Slot for seekable changed.
    void onSeekableChanged( bool muted );

    /// Slot for state changed.
    void onStateChanged( abstractMediaPlayer::State newState );

    /// Slot for visible changed.
    void onVisibleChanged( bool visible );

    /// Slot for volume changed.
    void onVolumeChanged( int volume );

    /// Slot for video available changed.
    void onVideoAvailableChanged( bool videoAvailable );

private:

    // not implemented
    _Myt& operator = ( const _Myt &value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBUSMEDIAPLAYERCLIENT_H
