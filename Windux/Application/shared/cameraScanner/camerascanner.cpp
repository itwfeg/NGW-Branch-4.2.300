
#include <QtConcurrentRun>
#include <QtDebug>

#include "camera.h"
#include "camerascanner.h"
#include "softekdecoderlinux.h"
#include "softekdecoderwin.h"

#include <QEvent>
#include <QFile>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
cameraScannerWidget::cameraScannerWidget( camera *c, QWidget *parent ) :
    _Mybase( parent ),
    camera_( c )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cameraScannerWidget::~cameraScannerWidget()
{
    QMutexLocker guard( &processing_ );

    // no more signals
    blockSignals( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool cameraScannerWidget::event( QEvent *e )
{
    if ( QEvent::MouseButtonRelease == e->type() )
        if ( camera_->isAutoFocusSupported() )
            camera_->autoFocus();

    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void cameraScannerWidget::capture()
{
    int id( camera_->capture() );

    if ( id < 0 )
        qWarning() << "error during capture!";
    else
    {
        qDebug() << "capture requested:" << id;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void cameraScannerWidget::captureImageSaved( int id, const QString& fileName )
{
    qDebug() << "processing:" << id << "[" << fileName << "]";

    // check for processing
    if ( !processing_.tryLock() )
    {
        qWarning() << "still processing previous image, dropping frame!";

        // remove file
        QFile::remove( fileName );
    }
    else
    {
        // process image in background
        QtConcurrent::run( this, &cameraScannerWidget::processImage, fileName );

        processing_.unlock();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void cameraScannerWidget::initialize()
{
    // create decoder
#if defined( Q_OS_WIN )
    decoder_ = new softekDecoderWin( this );
#elif defined( Q_OS_LINUX )
    decoder_ = new softekDecoderLinux( this );
#endif

    connect( decoder_, SIGNAL( decodedBarcodes(barcodeList) ), SIGNAL( decodedBarcodes(barcodeList) ) );

    // take ownership of camera widget
    camera_->setParent( this );

    connect( camera_, SIGNAL( captureImageSaved(int,QString) ), SLOT( captureImageSaved(int,QString) ) );

    // auto focus timer
    autoFocusCameraTimer_ = new QTimer( this );
    connect( autoFocusCameraTimer_, SIGNAL( timeout() ), camera_, SLOT( autoFocus() ) );

    if ( camera_->isAutoFocusSupported() )
        autoFocusCameraTimer_->start( 3000 );

    // snapshot timer
    snapshotTimer_ = new QTimer( this );
    connect( snapshotTimer_, SIGNAL( timeout() ), SLOT( capture() ) );

    snapshotTimer_->start( 250 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void cameraScannerWidget::createLayout()
{
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );
    screenLayout->addWidget( camera_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void cameraScannerWidget::processImage( const QString &fileName )
{
    QMutexLocker guard( &processing_ );

    // decode!
    decoder_->decode( fileName );

    // remove file
    QFile::remove( fileName );
}
