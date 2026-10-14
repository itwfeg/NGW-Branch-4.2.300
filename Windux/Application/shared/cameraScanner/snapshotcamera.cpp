
#include <QtDebug>

#include "snapshotcamera.h"

#include <QFile>
#include <QTimer>

snapshotCamera *snapshotCamera::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
snapshotCamera::snapshotCamera( QWidget *parent ) :
    _Mybase( parent ),
    id_( 0 )
{
    // set instance
    instance_ = this;

    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
snapshotCamera::~snapshotCamera()
{
    // reset instance
    instance_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int snapshotCamera::captureImage( const QString& fileName )
{
    QString imageFileName( fileName );

    // check for jpg extension
    if (( 0 != imageFileName.right( 3 ).compare( "jpg", Qt::CaseInsensitive ) ) &&
        ( 0 != imageFileName.right( 4 ).compare( "jpeg", Qt::CaseInsensitive ) ))
    {
        imageFileName.append( ".jpg" );
    }

    QMutexLocker guard( &m_ );

    // next id
    int id( ++id_ );

    // too many queued, dont bother something is wrong
    if ( CAPTURE_QUEUE_SIZE <= pending_.size() )
    {
        qWarning() << "aborting capture, queue too large";
        return -1;
    }

    qDebug() << "[" << id << "]" << imageFileName;
    pending_.append( captureImageInfo( id, imageFileName ) );

    return id;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void snapshotCamera::processFrame( const unsigned char *buf, size_t s )
{
    if ( !instance_ )
        return;

    instance_->processFrameInternal( buf, s );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void snapshotCamera::checkCompletedCaptures()
{
    captureImageInfo ci;

    {
        QMutexLocker guard( &m_ );

        // nothing to do
        if ( complete_.isEmpty() )
            return;

        // pull capture image info
        ci = complete_.takeFirst();
    }

    // emit signal!
    Q_EMIT captureImageSaved( ci.first, ci.second );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void snapshotCamera::initialize()
{
    checkCompletedCapturesTimer_ = new QTimer( this );
    connect( checkCompletedCapturesTimer_, SIGNAL( timeout() ), SLOT( checkCompletedCaptures() ) );

    checkCompletedCapturesTimer_->start( 250 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void snapshotCamera::processFrameInternal( const unsigned char *buf, size_t s )
{
    QMutexLocker guard( &m_ );

    // invoke derived class functionality
    frame( buf, s );

    // nothing to do
    if ( pending_.isEmpty() )
        return;

    // pull capture image info
    captureImageInfo ci( pending_.takeFirst() );

    // write file
    QFile f( ci.second );

    if ( f.open( QFile::WriteOnly ) )
    {
        f.write( (const char*) buf, s );
        f.close();

        complete_.append( ci );
    }
}
