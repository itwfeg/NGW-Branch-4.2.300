/**
 * @file serviceNewWrapperBspCamera.h
 * @brief Service screen for viewing and adjusting BSP camera threashold.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERBSPCAMERA_H
#define SERVICENEWWRAPPERBSPCAMERA_H

#include "screenwidget.h"

#include <QMutex>
#include <QPixmap>
#include <QVector>

class QLabel;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Intake camera graph widget.
class intakeCameraGraphWidget : public QWidget
{
    Q_OBJECT

    typedef intakeCameraGraphWidget _Myt;
    typedef QWidget _Mybase;

public:

    /// Camera type.
    enum camera
    {
        leftCamera = 0,                             ///< Left camera.
        rightCamera = 1,                            ///< Right camera.

        NUM_CAMERAS = 2                             ///< Number of cameras.
    };

    /// Camera image list type.
    typedef QVector<int> cameraImageList;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    intakeCameraGraphWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~intakeCameraGraphWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set camera data.
    /**
     * @param[in] c  camera to set
     * @param[in] value  camera values
     */
    virtual void setCameraData( camera c, const cameraImageList &value );

    /// Set threshold.
    /**
     * @param[in] value  threshold
     */
    virtual void setThreshold( int value );

    /// Retrieve size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const {return QSize( 500, 500 );}

protected:

    enum {MIN_THRESHOLD = 80};
    enum {MAX_THRESHOLD = 192};

    enum {CAMERA_POINTS = 128};
    enum {CAMERA_OFFSET = 8};

    cameraImageList cameraLeftNominal_;
    cameraImageList cameraRightNominal_;

    cameraImageList cameraLeft_;
    cameraImageList cameraRight_;

    int threshold_;

    // ========================================================================
    // Events
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Render nominal data.
    void renderNominalData();

    /// Render data.
    void renderData();

private:

    QMutex m_;

    QPixmap pixmapNominal_;
    QPixmap pixmap_;

    /// Draw camera data.
    static void drawCameraData( QPainter &painter, const cameraImageList &left, const cameraImageList &right );

    /// Draw camera data.
    static void drawCameraDataEx( QPainter &painter, const cameraImageList &data, int offset );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for viewing and adjusting BSP camera threashold.
class serviceNewWrapperBspCamera : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperBspCamera _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceNewWrapperBspCamera( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperBspCamera();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Save threshold.
    virtual void saveCurrentThreshold();

private Q_SLOTS:

    /// Slot for handling push button pressed.
    void onPushButtonPressed();

    /// Slot to request camera tracking.
    void requestCameraTracking();

    /// SOF-1205 Slot for timer timeout
    void startCameraTest();

private:

    enum {START_TEST_TIMER = 3000};
    enum {CAMERA_TRACKING_TIMER = 5000};

    enum {MIN_THRESHOLD = 80};
    enum {MAX_THRESHOLD = 192};

    int origThreshold_;
    int threshold_;

    QTimer *startCameraTestTimer_;
    QTimer *requestCameraTracking_;

    intakeCameraGraphWidget *graph_;

    QPushButton *inc_;
    QPushButton *dec_;
    QPushButton *save_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERBSPCAMERA_H
