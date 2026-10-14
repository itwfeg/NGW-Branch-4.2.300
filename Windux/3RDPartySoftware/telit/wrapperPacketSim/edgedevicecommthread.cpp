
#include "edgedevicecommthread.h"

#include "DiagnosticsEdgeDevice.h"

#ifdef min
#undef min
#endif

#include <QDateTime>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
edgeDeviceCommThread::edgeDeviceCommThread( DiagnosticsEdgeDevice *edgeDevice, QObject *parent ) :
    _Mybase( parent ),
    edgeDevice_( edgeDevice )
{
    cycles_ = (QDateTime::currentMSecsSinceEpoch() / (1024 * 1024));

    updateCycles_ = new QTimer( this );
    updateCycles_->setInterval( 60000 ); // 1m
    updateCycles_->setSingleShot( false );

    sendStatusCheck_ = new QTimer( this );
    sendStatusCheck_->setInterval( 3600000 ); // 1h
    sendStatusCheck_->setSingleShot( false );

    connect( updateCycles_, SIGNAL(timeout()), SLOT(onTimeout()) );
    connect( sendStatusCheck_, SIGNAL(timeout()), SLOT(onTimeout()) );

    updateCycles_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgeDeviceCommThread::~edgeDeviceCommThread()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgeDeviceCommThread::exit( int returnCode )
{
    sendStatusCheck_->stop();
    exitThread_ = true;

    _Mybase::exit( returnCode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgeDeviceCommThread::quit()
{
    sendStatusCheck_->stop();
    exitThread_ = true;

    _Mybase::quit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgeDeviceCommThread::start( Priority priority )
{
    sendStatusCheck_->start();
    exitThread_ = false;

    _Mybase::start( priority );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgeDeviceCommThread::run()
{
    edgeDevice_->setWrapperCycleCount( cycles_ );

    edgeDevice_->setEnabled( true );
    edgeDevice_->sendPowerUp();

    QDateTime checkOnlineTime( QDateTime::fromMSecsSinceEpoch( 0 ) );

    do
    {
        edgeDevice_->checkComms();

        // check online every 5min
        const QDateTime now( QDateTime::currentDateTime() );

        if ( 300 <= checkOnlineTime.secsTo( now ) )
        {
            edgeDevice_->checkOnline();
            checkOnlineTime = now;
        }

    } while ( !exitThread_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgeDeviceCommThread::onTimeout()
{
    if ( updateCycles_ == sender() )
    {
        cycles_ += 13;
        edgeDevice_->setWrapperCycleCount( cycles_ );
    }
    else if ( sendStatusCheck_ == sender() )
    {
        edgeDevice_->sendPowerUp();
    }
}

