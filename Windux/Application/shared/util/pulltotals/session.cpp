
#include "session.h"

#include <QDebug>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

const char session::RS( 0x1E );
const char session::US( 0x1F );

///////////////////////////////////////////////////////////////////////////////////////////////////
session::session( sessionType t, QObject *parent ) :
    _Mybase( parent ),
    t_( t )
{
    socket_ = new QTcpSocket( this );
    connect( socket_, &QTcpSocket::connected, this, &_Myt::onSocketConnected );
    connect( socket_, &QTcpSocket::disconnected, this, &_Myt::onSocketDisconnected );
    connect( socket_, &QTcpSocket::readyRead, this, &_Myt::onSocketReadyRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
session::~session()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool session::connectToHost( const QHostAddress& address, quint16 port )
{
    // connect to host
    socket_->connectToHost( address, port );

    if ( socket_->waitForConnected() )
    {
        // setup timer
        QTimer::singleShot( SESSION_TIMEOUT, this, &_Myt::onTimeout );

        return true;
    }

    socket_->disconnectFromHost();
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::onSocketConnected()
{
    results_.clear();
    a_.clear();

    // register session
    registerSession();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::onSocketDisconnected()
{
    emit complete();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::onSocketReadyRead()
{
    a_.append( socket_->readAll() );
    processBuffer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::onTimeout()
{
    socket_->disconnectFromHost();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::processBuffer()
{
process_loop:

    for ( int i( 0 ); i < a_.length(); ++i )
        if ( RS == a_.at( i ) )
        {
            processResponse( a_.left( i ) );

            // truncate buffer
            a_ = a_.mid( i+1 );
            goto process_loop;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::processResponse( const QByteArray& response )
{
    static const int MIN_LENGTH( 4 );

    // bad response
    if ( response.length() < MIN_LENGTH )
    {
        qWarning() << "bad response" << response;
        return;
    }

    // ---- //

    // register session
    if ( qPrintable( "RT01" ) == response.left( MIN_LENGTH ) )
    {
        if ( PULL_AND_CLEAR_TOTALS == t_ )
            pullAndClearTotals();
        else if ( PULL_EXPIRATION_LABELS == t_ )
            pullExpirationLabels();
        else
        {
            qWarning() << "unknown session type" << t_;
        }
    }

    // terminate session
    else if ( qPrintable( "RTFF" ) == response.left( MIN_LENGTH ) )
    {
        socket_->disconnectFromHost();
    }

    // session error
    else if ( qPrintable( "RTER" ) == response.left( MIN_LENGTH ) )
    {
        qWarning() << "session error" << response;
        socket_->disconnectFromHost();
    }

    // expiration labels
    // totals
    else if (( qPrintable( "RTAR" ) == response.left( MIN_LENGTH ) ) ||
        ( qPrintable( "RTBA" ) == response.left( MIN_LENGTH ) ))
    {
        const tagMap tags( splitTags( response ) );

        // save totals
        results_.append( tags );

        qDebug() << tags;
    }

    else
    {
        qWarning() << "unhandled response type" << response;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::registerSession() const
{
    QByteArray request;
    request.append( qPrintable( "RT01" ) );
    request.append( RS );

    socket_->write( request );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::terminateSession() const
{
    QByteArray request;
    request.append( qPrintable( "RTFF" ) );
    request.append( RS );

    socket_->write( request );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::pullAndClearTotals() const
{
    QByteArray request;
    request.append( qPrintable( "RT77" ) );
    request.append( RS );

    socket_->write( request );

    // terminate session after receive totals
    terminateSession();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void session::pullExpirationLabels() const
{
    QByteArray request;
    request.append( qPrintable( "RT5R" ) );
    request.append( RS );

    socket_->write( request );

    // terminate session after receive totals
    terminateSession();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
session::tagMap session::splitTags( const QByteArray& response )
{
    static const int MIN_LENGTH( 2 );

    const QList<QByteArray> tags( response.split( US ) );

    tagMap result;

    foreach ( const QByteArray& tag, tags )
        if ( MIN_LENGTH <= tag.length() )
            result[tag.left( MIN_LENGTH )] = tag.mid( MIN_LENGTH );

    return result;
}
