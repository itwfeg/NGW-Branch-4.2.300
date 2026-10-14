
#include <QtDebug>
#include <QtGlobal>

// requires Qt5
#if (0x050000 <= QT_VERSION)

#include "multimediacamera.h"

#include <QCamera>
#include <QCameraExposure>
#include <QCameraFocus>
#include <QCameraImageCapture>
#include <QCameraImageProcessing>
#include <QCameraViewfinder>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaCamera::multimediaCamera( QWidget *parent ) :
    _Mybase( parent ),
    cameraViewfinder_( NULL ),
    camera_( NULL ),
    cameraImage_( NULL )
{
    // iterate cameras
    QList<QCameraInfo> cameras( QCameraInfo::availableCameras() );

    QCameraInfo camera;
    bool foundCamera( false );

    foreach ( const QCameraInfo &c, cameras )
    {
        qDebug() << "CAMERA:" << c.deviceName() << "[" << c.description() << "]";

        // attach to default camera
        if (( !foundCamera ) || ( QCameraInfo::defaultCamera() == c ))
        {
            camera = c;
            foundCamera = true;
        }
    }

    // init
    initialize( camera );
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaCamera::multimediaCamera( const QCameraInfo& camera, QWidget *parent ) :
    _Mybase( parent ),
    cameraViewfinder_( NULL ),
    camera_( NULL ),
    cameraImage_( NULL )
{
    // init
    initialize( camera );
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
multimediaCamera::~multimediaCamera()
{
    // delete camera
    if ( camera_ )
    {
        camera_->stop();

        delete camera_;
        camera_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int multimediaCamera::captureImage( const QString& fileName )
{
    // sanity check
    if (( !camera_ ) || ( !cameraImage_ ))
        return -1;
    else if ( !cameraImage_->isReadyForCapture() )
        return -1;

    // capture an image!
    camera_->searchAndLock();
    int id( cameraImage_->capture( fileName ) );
    camera_->unlock();

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaCamera::initialize( const QCameraInfo& camera )
{
    // camera viewfinder
    cameraViewfinder_ = new QCameraViewfinder( this );

    // camera
    if ( camera.isNull() )
        qCritical() << "invalid camera!";
    else
    {
        QCameraExposure *cameraExposure;
        QCameraFocus *cameraFocus;
        QCameraImageProcessing *cameraImageProcessing;

        // setup camera
        camera_ = new QCamera( camera );
        camera_->setViewfinder( cameraViewfinder_ );

        // setup camera exposure
        if ( (cameraExposure = camera_->exposure()) )
        {
            if ( !cameraExposure->isAvailable() )
                qWarning() << "exposure not available";
            else
            {
                QCameraExposure::ExposureMode em( QCameraExposure::ExposureAuto );

                cameraExposure->setAutoAperture();
                cameraExposure->setAutoIsoSensitivity();
                cameraExposure->setAutoShutterSpeed();

                if ( !cameraExposure->isExposureModeSupported( em ) )
                    qWarning() << "exposure mode not supported";
                else
                {
                    cameraExposure->setExposureMode( em );
                }
            }
        }

        // setup camera focus
        if ( (cameraFocus = camera_->focus()) )
        {
            if ( !cameraFocus->isAvailable() )
                qWarning() << "focus not available";
            else
            {
                QList<QCameraFocus::FocusMode> focusModes;
                focusModes.append( QCameraFocus::ContinuousFocus );
                focusModes.append( QCameraFocus::AutoFocus );


                // focus mode
                foreach ( QCameraFocus::FocusMode fm, focusModes )
                    if ( cameraFocus->isFocusModeSupported( fm ) )
                    {
                        cameraFocus->setFocusMode( fm );
                        break;
                    }

                QList<QCameraFocus::FocusPointMode> focusPointModes;
                focusPointModes.append( QCameraFocus::FocusPointAuto );
                focusPointModes.append( QCameraFocus::FocusPointCenter );

                // focus point mode
                foreach ( QCameraFocus::FocusPointMode fpm, focusPointModes )
                    if ( cameraFocus->isFocusPointModeSupported( fpm ) )
                    {
                        cameraFocus->setFocusPointMode( fpm );
                        break;
                    }
            }
        }

        // setup camera image processing
        if ( (cameraImageProcessing = camera_->imageProcessing()) )
        {
            QCameraImageProcessing::WhiteBalanceMode wbm( QCameraImageProcessing::WhiteBalanceAuto );

            if ( !cameraImageProcessing->isAvailable() )
                qWarning() << "image processing not available";
            else if ( !cameraImageProcessing->isWhiteBalanceModeSupported( wbm ) )
                qWarning() << "white balance mode not supported";
            else
            {
                cameraImageProcessing->setWhiteBalanceMode( wbm );
            }
        }

        // setup camera image
        cameraImage_ = new QCameraImageCapture( camera_, this );

        connect( cameraImage_, SIGNAL( imageSaved(int,QString) ), SIGNAL( captureImageSaved(int,QString) ) );

        // go!
        camera_->setCaptureMode( QCamera::CaptureStillImage );
        camera_->start();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void multimediaCamera::createLayout()
{
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );
    screenLayout->addWidget( cameraViewfinder_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QT_VERSION
