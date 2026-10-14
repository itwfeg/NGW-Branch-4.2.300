#ifndef AFU050CAMERA_H
#define AFU050CAMERA_H

#include "gstsnapshotcamera.h"

typedef struct afu050_handle *afu050_handle_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Imaging Source AFU050 camera.
/**
 * This is a GST based camera with appsrc feeding the pipeline and driving the snapshots.
 */
class afu050Camera : public gstSnapshotCamera
{
    Q_OBJECT

    typedef afu050Camera _Myt;
    typedef gstSnapshotCamera _Mybase;

    friend int afu050_new_frame_cb( afu050_handle_t handle, unsigned char *jpegbuf, size_t jpegsize, void *data );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    afu050Camera( QWidget *parent = 0 );

    /// Destructor.
    virtual ~afu050Camera();

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Check for camera attached to system.
    /**
     * @return  @c true if attached, @c false otherwise
     */
    static bool isAttachedToSystem();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Auto focus camera image.
    virtual void autoFocus();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if auto focus is supported.
    /**
     * @return  @c true if supported, @c false otherwise
     */
    virtual bool isAutoFocusSupported() const {return true;}

    /// Check if overlay enabled.
    /**
     * @return  @c true if enabled, @c false otherwise
     */
    virtual bool isOverlayEnabled() const;

private Q_SLOTS:

    /// Resize sink geometry.
    void resizeSinkGeometry( const QRect& sinkGeometry );

private:

    afu050_handle_t handle_;

    /// Initialize.
    void initialize();

    /// Generate pipeline.
    static bool generatePipeline( QString& pipe );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // AFU050CAMERA_H
