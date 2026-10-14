
#include <QtDebug>
#include <QtGlobal>

// linux only
#ifdef Q_OS_LINUX

#include "v4l2camera.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
void print_buffer( GstBuffer *buffer, const char *title )
{
#ifndef QT_DEBUG
    Q_UNUSED( buffer );
    Q_UNUSED( title );

#else
    GstCaps *caps( gst_buffer_get_caps( buffer ) );

    for ( uint j( 0 ); j < gst_caps_get_size( caps ); ++j )
    {
        GstStructure *structure( gst_caps_get_structure( caps, j ) );

        QString serialized( QString( "%1{%2}: " ).arg( title ).arg( gst_structure_get_name( structure ) ) );

        for ( int i( 0 ); i < gst_structure_n_fields( structure ); ++i )
        {
            const char *name( gst_structure_nth_field_name( structure, i ) );
            GType type( gst_structure_get_field_type( structure, name ) );

            if ( 0 < i )
                serialized.append( ", " );

            serialized.append( QString( "%1[%2]" ).arg( name ).arg( g_type_name( type ) ) );
        }

        qCritical() << serialized;
    }

#endif
}

GstFlowReturn new_buffer_list( GstAppSink *sink, gpointer user_data )
{
    Q_UNUSED(user_data);

    GstBufferList *list( gst_app_sink_pull_buffer_list( sink ) );

    // iterate buffer list
    GstBufferListIterator *it( gst_buffer_list_iterate( list ) );

    while ( gst_buffer_list_iterator_next_group( it ) )
    {
        GstBuffer *buffer;

        // grab buffer
        while ( (buffer = gst_buffer_list_iterator_next( it )) )
        {
            print_buffer( buffer, "new_buffer_list" );
            gst_buffer_unref( buffer );
        }
    }

    gst_buffer_list_iterator_free( it );
    gst_buffer_list_unref( list );

    return GST_FLOW_OK;
}

GstFlowReturn new_preroll( GstAppSink *sink, gpointer user_data )
{
    Q_UNUSED( user_data );

    // retreive buffer
    GstBuffer *buffer( gst_app_sink_pull_preroll( sink ) );

    if ( buffer )
    {
        print_buffer( buffer, "preroll" );
        gst_buffer_unref( buffer );
    }

    return GST_FLOW_OK;
}

GstFlowReturn new_buffer( GstAppSink *sink, gpointer user_data )
{
    Q_UNUSED( user_data );

    // retreive buffer
    GstBuffer *buffer( gst_app_sink_pull_buffer( sink ) );

    if ( buffer )
    {
        // process frame
        v4l2Camera::processFrame( GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer) );

        print_buffer( buffer, "buffer" );
        gst_buffer_unref( buffer );
    }

    return GST_FLOW_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
v4l2Camera::v4l2Camera( const QString& device, QWidget *parent ) :
    _Mybase( parent ),
    device_( device )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
v4l2Camera::~v4l2Camera()
{
    if ( appsink_ )
    {
        gst_object_unref( appsink_ );
        appsink_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool v4l2Camera::isOverlayEnabled() const
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
void v4l2Camera::resizeSinkGeometry( const QRect& sinkGeometry )
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
void v4l2Camera::initialize()
{
    connect( this, SIGNAL( sinkGeometryChanged(QRect) ), SLOT( resizeSinkGeometry(QRect) ) );

    // ---- //

    QString pipe;

    // create pipeline
    if ( !generatePipeline( pipe, device_ ) )
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

    // retreive elements
    appsink_ = gst_bin_get_by_name( GST_BIN( pipeline_ ), "appsink" );

    // setup source
    GstElement *capsFilter( gst_bin_get_by_name( GST_BIN( pipeline_ ), "filter" ) );

    if ( capsFilter )
    {
        GstCaps *caps( gst_caps_from_string( "image/jpeg" ) );
        g_object_set( capsFilter, "caps", caps, NULL );

        gst_caps_unref( caps );
    }

    // setup appsink
    g_object_set( appsink_, "emit-signals", TRUE, "sync", FALSE, NULL );

    GstAppSinkCallbacks callbacks = {NULL, new_preroll, new_buffer, new_buffer_list, { NULL }};
    gst_app_sink_set_callbacks( GST_APP_SINK(appsink_), &callbacks, NULL, NULL );

    // start pipline (set to pause() first for fsl plugins... they require this for some reason)
    pause();
    play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool v4l2Camera::generatePipeline( QString& pipe, const QString& device )
{
    QStringList pipeline;

    QStringList plugins( installedPlugins() );
    QStringList pluginElements;

    // generate list of plugin elements
    foreach ( const QString& plugin, plugins )
        pluginElements << installedPluginElements( plugin );

    // ---- //

    // freescale pipeline
    if (( pluginElements.contains( "v4l2src" ) ) &&
        ( pluginElements.contains( "capsfilter" ) ) &&
        ( pluginElements.contains( "tee" ) ) &&
        ( pluginElements.contains( "queue" ) ) &&
        ( pluginElements.contains( "vpudec" ) ) &&
        ( pluginElements.contains( "overlaysink" ) ) &&
        ( pluginElements.contains( "appsink" ) ))
    {
        pipeline.append( QString( "v4l2src name=source device=%1" ).arg( device ) );
        pipeline.append( "capsfilter name=filter" );
        pipeline.append( "tee name=t t." );

        pipeline.append( "queue" );
        pipeline.append( "vpudec" );
        pipeline.append( "overlaysink name=sink sync=false t." );

        pipeline.append( "queue" );
        pipeline.append( "appsink name=appsink" );

        // overlaysink requires VSALPHA environment variable
        setenv( "VSALPHA", "1", 1 );
    }

    // linux desktop pipeline
    else if (( pluginElements.contains( "v4l2src" ) ) &&
        ( pluginElements.contains( "tee" ) ) &&
        ( pluginElements.contains( "queue" ) ) &&
        ( pluginElements.contains( "jpegdec" ) ) &&
        ( pluginElements.contains( "ffmpegcolorspace" ) || pluginElements.contains( "cogcolorspace" ) ) &&
        ( pluginElements.contains( "videoscale" ) ) &&
        ( pluginElements.contains( "ximagesink" ) ) &&
        ( pluginElements.contains( "appsink" ) ))
    {
        pipeline.append( QString( "v4l2src name=source device=%1" ).arg( device ) );
        pipeline.append( "capsfilter name=filter" );
        pipeline.append( "tee name=t t." );

        pipeline.append( "queue" );
        pipeline.append( "jpegdec" );

        if ( pluginElements.contains( "ffmpegcolorspace" ) )
            pipeline.append( "ffmpegcolorspace" );
        else if ( pluginElements.contains( "cogcolorspace" ) )
            pipeline.append( "cogcolorspace" );

        pipeline.append( "videoscale" );
        pipeline.append( "ximagesink name=sink sync=false force-aspect-ratio=true t." );

        pipeline.append( "queue" );
        pipeline.append( "appsink name=appsink" );
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
