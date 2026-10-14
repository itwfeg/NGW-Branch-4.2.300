
#include <QtDebug>

#include "afu050camera.h"
#include "camerascanner.h"
#include "ecu010camera.h"
#include "mainwindow.h"
#include "multimediacamera.h"
#include "tismultimediacamera.h"
#include "v4l2camera.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>

#if (0x050000 <= QT_VERSION)
#include <QSound>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::onDecodedBarcodes( const barcodeList& barcodes )
{
    Q_UNUSED( barcodes );

    // visual indication
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";

#if defined( Q_OS_LINUX ) && ( defined( QT_ARCH_ARM ) || defined( Q_PROCESSOR_ARM ) ) && ( 0x050000 <= QT_VERSION )
    // audio indication
    QSound::play( "ding.wav" );
#else
    // audio indication
    QApplication::beep();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::initialize()
{
    // setup widget for cameras
    central_ = new QWidget( this );

    centralLayout_ = new QHBoxLayout( central_ );
    centralLayout_->setContentsMargins( QMargins() );
    centralLayout_->setSpacing( 8 );

    setCentralWidget( central_ );

    // create cameras
    QList<camera*> cameras( createCameras() );

    // create camera scanner widgets
    foreach ( camera *cam, cameras )
    {
        // create widget
        cameraScannerWidget *scanner( new cameraScannerWidget( cam, central_ ) );
        connect( scanner, SIGNAL( decodedBarcodes(barcodeList) ), SLOT( onDecodedBarcodes(barcodeList) ) );

        centralLayout_->addWidget( scanner, 1 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<camera*> MainWindow::createCameras()
{
    QList<camera*> result;

// Qt5 Cameras
#if (0x050000 <= QT_VERSION)

    // iterate cameras
    QList<QCameraInfo> cameras( QCameraInfo::availableCameras() );

    foreach ( const QCameraInfo &c, cameras )
    {
        qDebug() << "CAMERA: " << c.deviceName() << " [" << c.description() << "]";

#if defined(Q_OS_WIN)
        // the imaging source cameras
        if (( c.description().contains( "DFK AFU050" ) ) ||
            ( c.description().contains( "DFK ECU010" ) ))
        {
            result.append( new tisMultimediaCamera( c ) );
        }
#else
        // the imaging source cameras
        if ( c.description().contains( "DFK ECU010" ) )
        {
            result.append( new v4l2Camera( c.deviceName() ) );
        }
#endif
        // generic camera
        else
        {
            result.append( new multimediaCamera( c ) );
        }
    }

#endif

// Linux Cameras
#ifdef Q_OS_LINUX

    // afu050 camera
    if ( afu050Camera::isAttachedToSystem() )
    {
        qDebug() << "CAMERA: DFK AFU050";

        // attach to camera
        result.append( new afu050Camera() );
    }
#endif

    return result;
}

