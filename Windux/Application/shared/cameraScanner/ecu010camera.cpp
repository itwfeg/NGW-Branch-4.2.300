
#include <QtDebug>
#include <QtGlobal>

// linux only
#ifdef Q_OS_LINUX

#include "ecu010camera.h"
#include "usbutil.h"

#include <QDir>
#include <QFileInfo>

///////////////////////////////////////////////////////////////////////////////////////////////////
ecu010Camera::ecu010Camera( QWidget *parent ) :
    _Mybase( deviceName(), parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ecu010Camera::~ecu010Camera()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ecu010Camera::isAttachedToSystem()
{
    return usbUtil::isAttachedToSystem( 0x199e, 0x0109 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ecu010Camera::deviceName()
{
    QString path( "/dev/v4l/by-id" );

    QDir dir( path );
    dir.setFilter( QDir::Files );

    // perform listing
    QFileInfoList list( dir.entryInfoList() );

    // look for our device
    foreach ( const QFileInfo& fi, list )
    {
        QString fileName( fi.fileName() );

        if ( fileName.contains( "DFK_ECU010" ))
            return fi.filePath();
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // Q_OS_LINUX
