
#include <QtDebug>
#include <QtGlobal>

// linux only
#ifdef Q_OS_LINUX

#include "afu050camera.h"
#include "usbutil.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

extern "C" {
#include <afu050.h>
}

#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Sublass of QThread to expose sleep in Qt4.
class this_thread : public QThread {
    typedef this_thread _Myt;
    typedef QThread _Mybase;
public:
    static void sleep( unsigned long value ) {_Mybase::sleep( value );}
    static void msleep( unsigned long value ) {_Mybase::msleep( value );}
    static void usleep( unsigned long value ) {_Mybase::usleep( value );}
private:
    this_thread();
    _Myt& operator = ( const _Myt& );
};

///////////////////////////////////////////////////////////////////////////////////////////////////
int afu050_new_frame_cb(
    afu050_handle_t handle,
    unsigned char *jpegbuf,
    size_t jpegsize,
    void *data )
{
    Q_UNUSED( handle );

    // process frame
    afu050Camera::processFrame( jpegbuf, jpegsize );

    // save data to buffer
    GstBuffer *buf( gst_buffer_new() );
    GST_BUFFER_MALLOCDATA(buf) = jpegbuf;
    GST_BUFFER_SIZE(buf) = jpegsize;
    GST_BUFFER_DATA(buf) = GST_BUFFER_MALLOCDATA(buf);

    // push to pipeline
    GstElement *appsrc( GST_ELEMENT(data) );

    if ( GST_FLOW_OK != gst_app_src_push_buffer( GST_APP_SRC(appsrc), buf ) )
        gst_buffer_unref( buf );

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
afu050Camera::afu050Camera( QWidget *parent ) :
    _Mybase( parent ),
    handle_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
afu050Camera::~afu050Camera()
{
    qDebug() << "finalizing...";

    // free camera
    if ( handle_ )
    {
        // stop image capture process
        afu050_capture_stop( handle_ );
        qDebug() << "afu050_capture_stop";

        // give thread time to stop
        this_thread::msleep( 500 );

        // free resources allocated for the camera
        afu050_close( handle_ );
        handle_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool afu050Camera::isAttachedToSystem()
{
    return usbUtil::isAttachedToSystem( 0x199e, 0x8209 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void afu050Camera::autoFocus()
{
    if ( !handle_ )
        return;

    afu050_auto_focus( handle_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool afu050Camera::isOverlayEnabled() const
{
    GstElementFactory *factory;

    // special processing for ximagesink
    if (( sink_ ) && ( (factory = gst_element_get_factory( sink_ )) ))
    {
        static const QString imxv4l2sink( "imxv4l2sink" );
        static const QString ximagesink( "ximagesink" );

        if ( imxv4l2sink == GST_PLUGIN_FEATURE_NAME( factory ) )
            return true;
        else if ( ximagesink == GST_PLUGIN_FEATURE_NAME( factory ) )
            return true;
    }

    return _Mybase::isOverlayEnabled();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void afu050Camera::resizeSinkGeometry( const QRect& sinkGeometry )
{
    GstElementFactory *factory;

    // special processing for overlaysink
    if (( sink_ ) && ( (factory = gst_element_get_factory( sink_ )) ))
    {
        static const QString overlaysink( "overlaysink" );

        if ( overlaysink == GST_PLUGIN_FEATURE_NAME( factory ) )
        {
            // set geometry
            g_object_set( G_OBJECT( sink_ ), "overlay-left", sinkGeometry.left(), NULL );
            g_object_set( G_OBJECT( sink_ ), "overlay-top", sinkGeometry.top(), NULL );
            g_object_set( G_OBJECT( sink_ ), "overlay-width", sinkGeometry.width(), NULL );
            g_object_set( G_OBJECT( sink_ ), "overlay-height", sinkGeometry.height(), NULL );
            g_object_set( G_OBJECT( sink_ ), "reconfig", 1, NULL );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void afu050Camera::initialize()
{
    connect( this, SIGNAL( sinkGeometryChanged(QRect) ), SLOT( resizeSinkGeometry(QRect) ) );

    // ---- //

    QString pipe;
    GstCaps *caps;

    // create pipeline
    if ( !generatePipeline( pipe ) )
    {
        qWarning() << "could not generate pipe!";
        return;
    }

    // set pipeline
    else if ( !setPipeline( pipe ) )
    {
        qWarning() << "invalid pipeline!";
        return;
    }

    // setup source
    g_object_set( source_, "block", FALSE, NULL );

    caps = gst_caps_from_string( "image/jpeg" );
    gst_app_src_set_caps( GST_APP_SRC( source_ ), caps );
    gst_caps_unref( caps );

    // ---- //

    // acquire a handle to the camera
    if ( !(handle_ = afu050_open() ) )
        qWarning() << "failed to open camera";
    else
    {
        qDebug() << "afu050_open: OK";

        // set desired video format
        afu050_set_video_format( handle_, AFU050_FMT_2592X1944 );

        // enable all auto functions
        afu050_set_exposure_auto( handle_, 1 );
        afu050_set_gain_auto( handle_, 1 );
        afu050_set_white_balance_auto( handle_, 1 );

        // start capturing image data
        if ( afu050_capture_start( handle_, afu050_new_frame_cb, source_ ) < 0 )
            qWarning() << "failed to start capture";
        else
        {
            qDebug() << "afu050_capture_start: OK";
        }
    }

    // ---- //

    // start pipline (set to pause() first for fsl plugins... they require this for some reason)
    pause();
    play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool afu050Camera::generatePipeline( QString& pipe )
{
    QStringList pipeline;

    QStringList plugins( installedPlugins() );
    QStringList pluginElements;

    // generate list of plugin elements
    foreach ( const QString& plugin, plugins )
        pluginElements << installedPluginElements( plugin );

    // ---- //

    // freescale pipeline
    if (( pluginElements.contains( "appsrc" ) ) &&
        ( pluginElements.contains( "queue" ) ) &&
        ( pluginElements.contains( "vpudec" ) ) &&
        ( pluginElements.contains( "overlaysink" ) ))
    {
        pipeline.append( "appsrc name=source block=false" );
        pipeline.append( "queue" );
        pipeline.append( "vpudec low-latency=true" );
        pipeline.append( "queue" );
        pipeline.append( "overlaysink name=sink sync=false" );

        // overlaysink requires VSALPHA environment variable
        setenv( "VSALPHA", "1", 1 );
    }

    // linux desktop pipeline
    else if (( pluginElements.contains( "appsrc" ) ) &&
        ( pluginElements.contains( "queue" ) ) &&
        ( pluginElements.contains( "jpegdec" ) ) &&
        ( pluginElements.contains( "ffmpegcolorspace" ) || pluginElements.contains( "cogcolorspace" ) ) &&
        ( pluginElements.contains( "videoscale" ) ) &&
        ( pluginElements.contains( "ximagesink" ) ))
    {
        pipeline.append( "appsrc name=source block=false" );
        pipeline.append( "queue" );
        pipeline.append( "jpegdec" );

        if ( pluginElements.contains( "ffmpegcolorspace" ) )
            pipeline.append( "ffmpegcolorspace" );
        else if ( pluginElements.contains( "cogcolorspace" ) )
            pipeline.append( "cogcolorspace" );

        pipeline.append( "videoscale" );
        pipeline.append( "ximagesink name=sink sync=false force-aspect-ratio=true" );
    }

    // unsupported
    else
    {
        return false;
    }

    // ---- //

    // build pipeline
    foreach ( const QString& p, pipeline )
    {
        if ( pipe.length() )
            pipe.append( " ! " );

        pipe.append( p );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // Q_OS_LINUX
