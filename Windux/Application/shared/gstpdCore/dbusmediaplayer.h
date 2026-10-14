/**
 * @file dbusmediaplayer.h
 * Media player backend interface.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBUSMEDIAPLAYER_H
#define DBUSMEDIAPLAYER_H

#include "abstractmediaplayer.h"

class QDBusArgument;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Create @c dbusMediaPlayer as a DBUS client.
struct dbus_client_t {};

/// Used to construct @c dbusMediaPlayer as a DBUS client.
static const dbus_client_t dbus_client = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Media player backend interface.
/**
 * This class wraps the @c abstractMediaPlayer and provides an interface over DBUS.
 */
class dbusMediaPlayer : public abstractMediaPlayer
{
    Q_OBJECT

    Q_PROPERTY( QString dbusObjectInterface READ dbusObjectInterface )
    Q_PROPERTY( QString dbusObjectPath READ dbusObjectPath )
    Q_PROPERTY( QString dbusServiceName READ dbusServiceName )

    typedef dbusMediaPlayer _Myt;
    typedef abstractMediaPlayer _Mybase;

public:

    /// DBUS object interface.
    static const QString DBUS_OBJECT_INTERFACE;

    /// DBUS object path.
    static const QString DBUS_OBJECT_PATH;

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~dbusMediaPlayer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve DBUS service name.
    /**
     * @return  service name
     */
    virtual QString dbusServiceName() const = 0;

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Retrieve DBUS object interface.
    /**
     * @return  interface
     */
    static QString dbusObjectInterface() {return DBUS_OBJECT_INTERFACE;}

    /// Retrieve DBUS object path.
    /**
     * @return  path name
     */
    static QString dbusObjectPath()  {return DBUS_OBJECT_PATH;}

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    dbusMediaPlayer( QObject *parent = NULL );

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    dbusMediaPlayer( dbus_client_t, QObject *parent = NULL );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Emit DBUS signal.
    /**
     * @param[in] name  signal name
     */
    virtual void emitSignal( const QString &name );

    /// Emit DBUS signal.
    /**
     * @param[in] name  signal name
     * @param[in] value  value
     */
    template <typename T>
    void emitSignal( const QString &name, const T &value );

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

    /// Slot for video available changed.
    void onVideoAvailableChanged( bool videoAvailable );

    /// Slot for visible changed.
    void onVisibleChanged( bool visible );

    /// Slot for volume changed.
    void onVolumeChanged( int volume );

private:

    // not implemented
    _Myt& operator = ( const _Myt &value );

    /// Initialize.
    void initialize( bool isClient );

};

QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::State &rhs );
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::State &rhs );

QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::MediaStatus &rhs );
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::MediaStatus &rhs );

QDBusArgument& operator << ( QDBusArgument &lhs, const abstractMediaPlayer::MediaType &rhs );
const QDBusArgument& operator >> ( const QDBusArgument &lhs, abstractMediaPlayer::MediaType &rhs );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBUSMEDIAPLAYER_H
