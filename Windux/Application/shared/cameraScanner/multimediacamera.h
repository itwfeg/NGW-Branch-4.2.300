#ifndef MULTIMEDIACAMERA_H
#define MULTIMEDIACAMERA_H

#include "camera.h"

// requires Qt5
#if (0x050000 <= QT_VERSION)
#include <QCameraInfo>
#else
class QCameraInfo;
#endif

class QCamera;
class QCameraImageCapture;
class QCameraViewfinder;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// QtMultimedia camera.
class multimediaCamera : public camera
{
    Q_OBJECT

    typedef multimediaCamera _Myt;
    typedef camera _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    multimediaCamera( QWidget *parent = 0 );

    /// Constructor.
    /**
     * @param[in] camera  camera to use
     * @param[in] parent  parent widget
     */
    multimediaCamera( const QCameraInfo& camera, QWidget *parent = 0 );

    /// Destructor.
    virtual ~multimediaCamera();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Capture current camera image.
    /**
     * @param[in] fileName  capture filename
     * @return  image id
     */
    virtual int captureImage( const QString& fileName );

private:

    QCameraViewfinder *cameraViewfinder_;

    QCamera *camera_;
    QCameraImageCapture *cameraImage_;

    /// Initialize.
    void initialize( const QCameraInfo& camera );

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MULTIMEDIACAMERA_H
