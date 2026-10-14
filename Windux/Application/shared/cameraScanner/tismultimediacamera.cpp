
#include <QtDebug>
#include <QtGlobal>

// requires Qt5
// windows only
#if (0x050000 <= QT_VERSION) && defined(Q_OS_WIN)

#include "tismultimediacamera.h"

#include <tisgrabber.h>

#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
tisMultimediaCamera::tisMultimediaCamera( const QCameraInfo& camera, QWidget *parent ) :
    _Mybase( camera, parent ),
    handle_( NULL ),
    init_( false ),
    attached_( false )
{
    // init
    initialize( camera );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tisMultimediaCamera::~tisMultimediaCamera()
{
    if ( attached_ )
        IC_CloseVideoCaptureDevice( handle_ );

    if ( handle_ )
        IC_ReleaseGrabber( &handle_ );

    if ( init_ )
        IC_CloseLibrary();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tisMultimediaCamera::autoFocus()
{
    if ( attached_ )
        IC_FocusOnePush( handle_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tisMultimediaCamera::initialize( const QCameraInfo& camera )
{
    QString cameraName( camera.description() );

    // init lib
    if ( !(init_ = (IC_SUCCESS == IC_InitLibrary( 0 ))) )
        qWarning() << "failed to init IC library";

    // allocate handle to lib
    else if ( !(handle_ = IC_CreateGrabber()) )
        qWarning() << "failed to create IC handle";

    // attach to camera
    else if ( !(attached_ = (IC_SUCCESS == IC_OpenVideoCaptureDevice( handle_, cameraName.toUtf8().data() ))) )
        qWarning() << "failed to open camera:" << cameraName;

    else
    {
        IC_EnableAutoCameraProperty( handle_, PROP_CAM_EXPOSURE, 1 );
        IC_EnableAutoVideoProperty( handle_, PROP_VID_GAIN, 1 );
        IC_EnableAutoVideoProperty( handle_, PROP_VID_WHITEBALANCE, 1 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QT_VERSION
