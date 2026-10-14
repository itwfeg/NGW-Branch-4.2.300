/**
 * @file test.h
 * Tests front end interface for DBUS video player.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TEST_H
#define TEST_H

#include <abstractmediaplayer.h>

#include <QObject>
#include <QString>
#include <QTimer>

class dbusMediaPlayerClient;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Tests front end interface for DBUS video player.
class test : public QObject
{
    Q_OBJECT

    typedef test _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] path  object path
     * @param[in] filename  media filename
     * @param[in] type  media type
     * @param[in] parent  parent object
     */
    test( const QString &path, const QString &filename, abstractMediaPlayer::MediaType type, QObject *parent = NULL );

    /// Destructor.
    virtual ~test();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve test result.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Validate interface.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool validate();

private Q_SLOTS:

    /// Slot for checking playback.
    void onCheckPlayback();

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

private:

    bool valid_;

    dbusMediaPlayerClient *player_;

    QString filename_;
    abstractMediaPlayer::MediaType type_;

    QTimer checkPlaybackTimer_;
    QTimer validateTimer_;

    // not implemented
    _Myt& operator = ( const _Myt &value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TEST_H

