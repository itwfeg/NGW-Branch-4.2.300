#ifndef V4L2CAMERA_H
#define V4L2CAMERA_H

#include "gstsnapshotcamera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Generic V4L2 camera.
class v4l2Camera : public gstSnapshotCamera
{
    Q_OBJECT

    typedef v4l2Camera _Myt;
    typedef gstSnapshotCamera _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] device  device name (i.e. /dev/video0)
     * @param[in] parent  parent widget
     */
    v4l2Camera( const QString& device, QWidget *parent = 0 );

    /// Destructor.
    virtual ~v4l2Camera();

protected:

    GstElement *appsink_;                           ///< Application sink object.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device name.
    /**
     * @return  device name
     */
    virtual QString device() const {return device_;}

    /// Check if overlay enabled.
    /**
     * @return  @c true if enabled, @c false otherwise
     */
    virtual bool isOverlayEnabled() const;

private Q_SLOTS:

    /// Resize sink geometry.
    void resizeSinkGeometry( const QRect& sinkGeometry );

private:

    QString device_;

    /// Initialize.
    void initialize();

    /// Generate pipeline.
    static bool generatePipeline( QString& pipe, const QString& device );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // V4L2CAMERA_H
