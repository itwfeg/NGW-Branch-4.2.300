#ifndef ECU010CAMERA_H
#define ECU010CAMERA_H

#include "v4l2camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Imaging Source ECU010 camera.
/**
 * This is a GST based camera with v4l2src feeding the pipeline and driving the snapshots.
 */
class ecu010Camera : public v4l2Camera
{
    Q_OBJECT

    typedef ecu010Camera _Myt;
    typedef v4l2Camera _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    ecu010Camera( QWidget *parent = 0 );

    /// Destructor.
    virtual ~ecu010Camera();

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Check for camera attached to system.
    /**
     * @return  @c true if attached, @c false otherwise
     */
    static bool isAttachedToSystem();

protected:

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Retrieve camera device name.
    /**
     * @return  camera device name
     */
    static QString deviceName();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ECU010CAMERA_H
