
#include <QtDebug>
#include <QtGlobal>

// linux only
#ifdef Q_OS_LINUX

#include "gstsnapshotcamera.h"

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include <QApplication>
#include <QEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////
gstSnapshotCamera::gstSnapshotCamera( QWidget *parent ) :
    _Mybase( parent ),
    pipeline_( NULL ),
    source_( NULL ),
    sink_( NULL ),
    overlayEnabled_( false )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gstSnapshotCamera::~gstSnapshotCamera()
{
    qDebug() << "finalizing...";

    // free pipeline
    if ( pipeline_ )
    {
        // stop pipeline
        stop();

        source_ = NULL;
        sink_ = NULL;

        // free
        gst_object_unref( pipeline_ );
        pipeline_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::setMediaSize( const QSize& value )
{
    if ( mediaSize_ == value )
        return;

    mediaSize_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstSnapshotCamera::setPipeline( const QString& value )
{
    GError *error( NULL );

    // init gstreamer
    gst_init( NULL, NULL );

    // create pipeline
    qDebug() << "pipe:" << "[" << value << "]";
    pipeline_ = gst_parse_launch( value.toUtf8().constData(), &error );

    if ( error )
    {
        qWarning() << "pipeline error:" << error->message;
        return false;
    }
    else if ( !pipeline_ )
    {
        qWarning() << "error parsing pipeline";
        return false;
    }

    // retreive elements
    source_ = gst_bin_get_by_name( GST_BIN( pipeline_ ), "source" );
    sink_ = gst_bin_get_by_name( GST_BIN( pipeline_ ), "sink" );

    // setup overlay
    overlayEnabled_ = isOverlayEnabled();
    setOverlay();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::pause()
{
    if ( pipeline_ )
        gst_element_set_state( pipeline_, GST_STATE_PAUSED );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::play()
{
    if ( pipeline_ )
        gst_element_set_state( pipeline_, GST_STATE_PLAYING );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::stop()
{
    if ( pipeline_ )
        gst_element_set_state( pipeline_, GST_STATE_NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::moveEvent( QMoveEvent *e )
{
    update();
    _Mybase::moveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::paintEvent( QPaintEvent *e )
{
    calcSinkGeometry();
    _Mybase::paintEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::resizeEvent( QResizeEvent *e )
{
    update();
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::showEvent( QShowEvent *e )
{
    exposeOverlay();
    update();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gstSnapshotCamera::eventFilter( QObject *obj, QEvent *e )
{
    Q_UNUSED( obj );

    // when application moves we need to update
    if ( QEvent::Move == e->type() )
        update();

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::frame( const unsigned char *buf, size_t s )
{
    // check media size
    QSize media( mediaSize() );

    if ( !media.isValid() )
    {
        QImage image( QImage::fromData( buf, s ) );

        if ( image.isNull() )
            qWarning() << "error loading image!";
        else
        {
            // set media size
            setMediaSize( image.size() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList gstSnapshotCamera::installedPluginElements( const QString& plugin )
{
    QStringList elements;
    GList *node;

    // get list of features
    GList *list( node = gst_registry_get_feature_list_by_plugin( gst_registry_get_default(), plugin.toUtf8().data() ) );

    // process list
    while ( node )
    {
        if ( node->data )
        {
            // currently pointed-to feature
            GstPluginFeature *feature( GST_PLUGIN_FEATURE( node->data ) );

            if ( GST_IS_ELEMENT_FACTORY( feature ) )
            {
                GstElementFactory *factory( GST_ELEMENT_FACTORY( gst_plugin_feature_load( feature ) ) );

                // append to result list
                elements.append( QString( GST_PLUGIN_FEATURE_NAME( factory ) ) );
            }
        }

        // get next node
        node = g_list_next( node );
    }

    // free list
    gst_plugin_feature_list_free( list );

    return elements;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList gstSnapshotCamera::installedPlugins()
{
    QStringList plugins;
    GList *node;

    // get list of plugins
    GList *list( node = gst_default_registry_get_plugin_list() );

    // process list
    while ( node )
    {
        GstPlugin *plugin( (GstPlugin*) node->data );

        // append plugin to result list
        plugins.append( QString( plugin->desc.name ) );

        // get next node
        node = g_list_next( node );
    }

    // free list
    gst_plugin_list_free( list );

    return plugins;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::initialize()
{
    QPalette p;
    p.setColor( QPalette::Background, Qt::black );

    setPalette( p );
    setAutoFillBackground( true );

    // install filter for move events
    qApp->installEventFilter( this );

    // ---- //

    // init gstreamer
    gst_init( NULL, NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::calcSinkGeometry()
{
    static const QPoint origin( 0, 0 );

    QPoint pos( mapToGlobal( origin ) );

    QSize s;
    QSize viewport( s = size() );

    // adjust viewport size if we know media size
    QSize media( mediaSize() );

    if ( media.isValid() )
    {
#if (0x050000 <= QT_VERSION)
        viewport = media.scaled( s, Qt::KeepAspectRatio );
#else
        viewport = media;
        viewport.scale( s, Qt::KeepAspectRatio );
#endif
    }

    // calc geometry
    int adj_x( (s.width() - viewport.width()) / 2 );
    int adj_y( (s.height() - viewport.height()) / 2 );

    QRect g( adj_x + pos.x(), adj_y + pos.y(), viewport.width(), viewport.height() );

    // notify if different
    if ( sinkGeometry_ != g )
    {
        // emit!
        Q_EMIT sinkGeometryChanged( sinkGeometry_ = g );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::setOverlay()
{
    if ( !overlayEnabled_ )
        return;
    else if (( sink_ ) && ( GST_IS_X_OVERLAY( sink_ ) ))
    {
        WId win_id( _Mybase::winId() );
        syncX();

        gst_x_overlay_set_xwindow_id( GST_X_OVERLAY( sink_ ), win_id );
    }

    exposeOverlay();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::exposeOverlay()
{
    if ( !overlayEnabled_ )
        return;

    syncX();

    if (( sink_ ) && ( GST_IS_X_OVERLAY( sink_ ) ))
        gst_x_overlay_expose( GST_X_OVERLAY( sink_ ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gstSnapshotCamera::syncX()
{
#if (QT_VERSION < 0x050000)
    QApplication::syncX();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // Q_OS_LINUX
