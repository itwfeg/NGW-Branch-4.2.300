#ifndef GSTSNAPSHOTCAMERA_H
#define GSTSNAPSHOTCAMERA_H

#include "snapshotcamera.h"

typedef struct _GstElement GstElement;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GST based camera.
class gstSnapshotCamera : public snapshotCamera
{
    Q_OBJECT

    typedef gstSnapshotCamera _Myt;
    typedef snapshotCamera _Mybase;

Q_SIGNALS:

    /// Invoked when sink geometry changes.
    /**
     * @param[in] sinkGeometry  geometry
     */
    void sinkGeometryChanged( const QRect& sinkGeometry );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    gstSnapshotCamera( QWidget *parent = 0 );

    /// Destructor.
    virtual ~gstSnapshotCamera();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve media size.
    /**
     * @return  size
     */
    virtual QSize mediaSize() const {return mediaSize_;}

    /// Set media size.
    /**
     * @param[in] value  size
     */
    virtual void setMediaSize( const QSize& value );

    /// Set pipeline.
    /**
     * @param[in] value  pipeline
     * @return  @c true on success, @c false otherwise
     */
    virtual bool setPipeline( const QString& value );

    /// Retrieve size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const {return QSize( 640, 480 );}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Pause pipeline.
    virtual void pause();

    /// Play pipeline.
    virtual void play();

    /// Stop pipeline.
    virtual void stop();

protected:

    GstElement *pipeline_;                          ///< Pipeline.

    GstElement *source_;                            ///< Source object.
    GstElement *sink_;                              ///< Sink object.

    // ========================================================================
    // Events
    // ========================================================================

    /// Move event.
    /**
     * @param[in,out] e  event
     */
    virtual void moveEvent( QMoveEvent *e );

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

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if overlay enabled.
    /**
     * @return  @c true if enabled, @c false otherwise
     */
    virtual bool isOverlayEnabled() const {return false;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  object being filtered
     * @param[in] e  event
     * @return  @c true to filter, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Process frame data.
    /**
     * @param[in] buf  buffer
     * @param[in] s  buffer size
     */
    virtual void frame( const unsigned char*, size_t );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve list of installed plugin elements.
    /**
     * @param[in] plugin  plugin name
     * @return  list of elements
     */
    static QStringList installedPluginElements( const QString& plugin );

    /// Retrieve list of installed plugins.
    /**
     * @return  list of installed plugins
     */
    static QStringList installedPlugins();

private:

    bool overlayEnabled_;

    QSize mediaSize_;

    QRect sinkGeometry_;

    /// Initialize.
    void initialize();

    /// Calculate sink geometry.
    void calcSinkGeometry();

    /// Setup overlay.
    void setOverlay();

    /// Expose overlay.
    void exposeOverlay();

    /// Sync X11.
    static void syncX();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GSTSNAPSHOTCAMERA_H
