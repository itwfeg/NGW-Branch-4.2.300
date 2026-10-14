/**
 * @file videoplayerwidget.h
 * Widget for playing videos.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include "videoplayer/videoplayer.h"

#include <QWidget>

class seekSlider;
class volumeSlider;

class QLabel;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for playing multimedia videos.
class videoPlayerWidget : public QWidget
{
    Q_OBJECT

    typedef videoPlayerWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal for when media is finished.
    /**
     * @param[in] exitPressed  @c true when exit button was pressed, @c false otherwise
     */
    void finished( bool exitPressed );
    
    /// This signal is emitted when the media object detects audio.
    void hasAudioChanged( bool hasAudio );

    /// This signal is emitted when the media object detects video.
    void hasVideoChanged( bool hasVideo );

    /// This signal is emitted when playback error.
    /**
     * @param[in] desc  description of error
     */
    void playbackError( const QString& desc );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] deviceType  video player device type
     * @param[in] parent  parent widget
     */
    videoPlayerWidget( videoPlayerDevice::type deviceType, QWidget *parent = 0 );

    /// Constructor.
    /**
     * Creates player and signals video to play.
     *
     * @param[in] filename  filename of video to play
     * @param[in] deviceType  video player device type
     * @param[in] parent  parent widget
     */
    videoPlayerWidget( const QString& filename, videoPlayerDevice::type deviceType, QWidget *parent = 0 );

    /// Destructor.
    virtual ~videoPlayerWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if player active.
    /**
     * @return  @c true if player is active, @c false otherwise
     */
    virtual bool active() const;

    /// Check if user interface disabled.
    /**
     * @return  @c true if disabled, @c false otherwise
     */
    virtual bool isUserInterfaceDisabled() const {return disableUi_;}

    /// This property holds the recommended size for the widget.
    /**
     * If the value of this property is an invalid size, no size is recommended.
     *
     * @return  size
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Disable the user interface.
    virtual void disableUserInterface();

    /// Pause the video.
    virtual void pause();

    /// Play a video.
    /**
     * @param[in] filename filename of video to play
     * @return  @c true if video starts playing, @c false otherwise
     */
    virtual bool play( const QString& filename );

    /// Translate the widget fields.
    virtual void translate();

private Q_SLOTS:

    /// Slot for when the exit button is pressed.
    void onExitButtonPressed();

    /// Slot for when media finishes.
    void onFinished();

    /// Slot for when the play/pause button is pressed.
    void onPlayPauseButtonPressed();

    /// Slot for when the stop button is pressed.
    void onStopButtonPressed();

    /// Slot for when location changes.
    void onTick( qint64 time );

    /// Slot for when total time changes.
    void onTotalTimeChanged( qint64 newTotalTime );

private:

    enum PlayPauseButtonState
    {
        Toggle,
        ForcePlay,
        ForcePause
    };

    QPushButton *playPauseButton_;
    QPushButton *stopButton_;
    seekSlider *seekSlider_;
    QLabel *position_;
    QLabel *totalTime_;
    volumeSlider *volumeSlider_;
    QPushButton *exitButton_;

    videoPlayer *player_;

    bool disableUi_;

    // ========================================================================

    // not implemented
    videoPlayerWidget( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize
    void initialize( videoPlayerDevice::type deviceType = videoPlayerDevice::Operator );

    /// Check if the state of the play/pause button is play.
    bool isPlayButtonState() const;

    /// Check if the state of the play/pause button is pause.
    bool isPauseButtonState() const {return !isPlayButtonState();}

    /// Toggle the play/pause button.
    void togglePlayPauseButtonState( PlayPauseButtonState state = Toggle );

    /**
     * Delete video player object.
     */
    template <class T>
    static void deleteVideoObject( T **ptr )
    {
        if (( ptr ) && ( *ptr ))
        {
            T *doomed( *ptr );
            (*ptr) = NULL;

            deleteVideoObjectImpl( doomed );

            delete doomed;
        }
    }

    /**
     * Delete video player object.
     */
    template <class T>
    static void deleteVideoObjectImpl( T* /*ptr*/ ) {}

    /**
     * Convert milliseconds to time string.
     */
    static QString makeReadableTime( int msec );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VIDEOPLAYERWIDGET_H
