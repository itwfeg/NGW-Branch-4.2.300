#ifndef SNAPSHOTCAMERA_H
#define SNAPSHOTCAMERA_H

#include "camera.h"

#include <QMutex>

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Snapshot camera.
class snapshotCamera : public camera
{
    Q_OBJECT

    typedef snapshotCamera _Myt;
    typedef camera _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    snapshotCamera( QWidget *parent = 0 );

    /// Destructor.
    virtual ~snapshotCamera();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Process frame data.
    /**
     * The snapshot camera must invoke this method with raw JPEG image data.
     *
     * @param[in] buf  buffer
     * @param[in] s  buffer size
     */
    static void processFrame( const unsigned char *buf, size_t s );

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

    /// Process frame data.
    /**
     * @param[in] buf  buffer
     * @param[in] s  buffer size
     */
    virtual void frame( const unsigned char*, size_t ) {}

private Q_SLOTS:

    /// Check for completed image captures.
    void checkCompletedCaptures();

private:

    enum {CAPTURE_QUEUE_SIZE = 1};

    QMutex m_;

    int id_;

    typedef QPair<int, QString> captureImageInfo;
    QList<captureImageInfo> pending_;
    QList<captureImageInfo> complete_;

    QTimer *checkCompletedCapturesTimer_;

    static _Myt *instance_;

    /// Initialize.
    void initialize();

    /// Process frame data.
    void processFrameInternal( const unsigned char *buf, size_t s );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SNAPSHOTCAMERA_H
