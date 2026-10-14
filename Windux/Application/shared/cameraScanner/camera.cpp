
#include "camera.h"

#include <QDir>
#include <QString>

/// Unique id for each camera.
int camera::cameraId_( 0 );

///////////////////////////////////////////////////////////////////////////////////////////////////
camera::camera( QWidget *parent ) :
    _Mybase( parent ),
    id_( ++cameraId_ ),
    serial_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
camera::~camera()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void camera::autoFocus()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int camera::capture( const QString& fileName )
{
    QString imageFileName( fileName );

    // generate unique filename when none passed in
    if ( imageFileName.isEmpty() )
    {
        QDir tempPath( QDir::tempPath() );

        imageFileName = tempPath.absoluteFilePath( QString( "IMG_%1_%2" ).arg( id_ ).arg( (++serial_ % 10000), 4, 10, QChar( '0' ) ) );
    }

    // capture an image!
    return captureImage( imageFileName );
}
