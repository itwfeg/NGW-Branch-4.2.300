#ifndef TISMULTIMEDIACAMERA_H
#define TISMULTIMEDIACAMERA_H

#include "multimediacamera.h"

typedef struct HGRABBER_t__ *HGRABBER;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// The Imaging Source camera based on QtMultimedia.
class tisMultimediaCamera : public multimediaCamera
{
    Q_OBJECT

    typedef tisMultimediaCamera _Myt;
    typedef multimediaCamera _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] camera  camera to use
     * @param[in] parent  parent widget
     */
    tisMultimediaCamera( const QCameraInfo& camera, QWidget *parent = 0 );

    /// Destructor.
    virtual ~tisMultimediaCamera();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if auto focus is supported.
    /**
     * @return  @c true if supported, @c false otherwise
     */
    virtual bool isAutoFocusSupported() const {return true;}

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Auto focus camera image.
    virtual void autoFocus();

private:

    HGRABBER handle_;

    bool init_;
    bool attached_;

    /// Initialize.
    void initialize( const QCameraInfo& camera );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TISMULTIMEDIACAMERA_H
