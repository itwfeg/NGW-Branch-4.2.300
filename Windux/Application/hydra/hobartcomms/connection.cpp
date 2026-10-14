/**
 * @file connection.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "connection.h"

#include <connection_adaptor.h>
#include <sessioneventtype.h>
#include <sessionmanager_interface.h>
#include <tageventadapter.h>

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>

static const QString DBUS_SERVICE( COMM_MANAGER_DBUS_SERVICE );
static const QString DBUS_OBJECT_PATH( CONNECTION_DBUS_OBJECT_PATH );

// define proper constant dependending on qt version
#if QT_VERSION >= 0x050e00
#define KEEP_EMPTY_PARTS Qt::KeepEmptyParts
#define SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define KEEP_EMPTY_PARTS QString::KeepEmptyParts
#define SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif

QMutex Connection::m_;
uint Connection::nextConnection_( 1 );

///////////////////////////////////////////////////////////////////////////////////////////////////
Connection::Connection( QTcpSocket *s, QObject *parent ) :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH.arg( nextConnectionId() ), parent ),
    sent_( 0 ),
    recv_( 0 ),
    peekTimer_( new QTimer( this ) ),
    idleTimer_( new QTimer( this ) ),
    s_( s ),
    sendErrors_( 0 ),
    recvErrors_( 0 ),
    adapter_( nullptr ),
    sessionType_( 0 ),
    sessionPersistent_( false )
{
    s_->setParent( this );

    qInfo() << "CTOR" << objectPath();

    connect( peekTimer_, &QTimer::timeout, this, &_Myt::onTimeout );

    peekTimer_->setSingleShot( true );
    peekTimer_->start( PEEK_TIME );

    connect( idleTimer_, &QTimer::timeout, this, &_Myt::onTimeout );

    idleTimer_->setSingleShot( true );
    idleTimer_->setInterval( IDLE_TIME );

    connect( s_, &QTcpSocket::disconnected, this, &_Myt::onDisconnected );
#if QT_VERSION >= 0x050e00
    connect( s_, &QTcpSocket::errorOccurred, this, &_Myt::onErrorOccurred );
#endif
    connect( s_, &QTcpSocket::readyRead, this, &_Myt::onReadyRead );

    // set object name
    setObjectName( objectPath() );

    // register all events, this class simply forwards over comm
    setRegisterAllEvents( true );

    // parse out session id
    QStringList l( objectName().split( "/", SKIP_EMPTY_PARTS ) );

    if ( 3 == l.size() )
        id_ = l[2];

    // create adaptor
    new ConnectionAdaptor( this );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerObject( objectPath(), this ) )
    {
        const QDBusError err( connection.lastError() );

        if ( QDBusError::NoError != err.type() )
            qWarning() << "failed to register object" << objectPath() << err.type() << err.name() << err.message();
    }

    // register for all events
    // we will filter for events we want in onProcessEvent()
    setRegisterAllEvents( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Connection::~Connection()
{
    qInfo() << "DTOR" << objectPath();

    // destroy attached session
    if ( sessionPath_.length() )
    {
        // unregister
        SessionUnregister e;

        send( SESSION_MANAGER_DBUS_SERVICE, sessionPath_, &e );

        // destroy
        if ( !isSessionPersistent() )
        {
            com::hobartcorp::SessionManager *sessionMan = new com::hobartcorp::SessionManager(
                SESSION_MANAGER_DBUS_SERVICE,
                SESSION_MANAGER_DBUS_OBJECT_PATH,
                QDBusConnection::sessionBus() );

            if ( !sessionMan )
                qWarning() << "failed to allocate session manager";
            else
            {
                if ( !sessionMan->isValid() )
                    qWarning() << "invalid session manager";
                else
                {
                    qDebug() << "destroy session...";
                    sessionMan->destroySession( sessionPath_ );
                }

                delete sessionMan;
            }
        }
    }

    // delete any adapter
    if ( adapter_ )
    {
        qDebug() << "delete adapter...";

        delete adapter_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Connection::id() const
{
    return id_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qulonglong Connection::bytesSent() const
{
    return sent_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qulonglong Connection::bytesReceived() const
{
    return recv_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Connection::isSessionPersistent() const
{
    return sessionPersistent_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::setSessionPersistent( bool value )
{
    sessionPersistent_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( destroy )

    const bool isFromSession(( SESSION_MANAGER_DBUS_SERVICE == e->senderService() ) && ( sessionPath_ == e->senderObjectPath() ));

    // only interested in events sent to our device type -or- events from our service
    if (( DeviceType::Connection != e->device() ) && ( !isFromSession ))
        return;
    else if ( !adapter_ )
        return;

    qDebug() << "process event" << (uint) e->device() << e->event();

    // translate event

    bool valid;
    const QByteArray a( adapter_->translateEvent( e, &valid ) );

    if ( valid )
        sendErrors_ = 0;
    else
    {
        qWarning() << "send error";
        ++sendErrors_;

        if ( MAX_ERRORS <= sendErrors_ )
        {
            qWarning() << "too many errors, terminating connection";

            deleteLater();
            return;
        }
    }

    // send data
    qDebug() << "write" << a.size() << "bytes...";
    s_->write( a );

    sent_ += a.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::onDisconnected()
{
    qInfo() << "disconnect" << objectPath();

    deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::onErrorOccurred( QAbstractSocket::SocketError socketError )
{
    qWarning() << "socket error" << socketError << objectPath();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::onReadyRead()
{
    qDebug() << "ready read" << objectPath();

    const qulonglong origBufferSize( buffer_.size() );

    // read all pending data
    buffer_.append( s_->readAll() );

    recv_ += (buffer_.size() - origBufferSize);

    // reset idle timer
    idleTimer_->start();

    // find an adapter based on what is in the buffer
    if ( !adapter_ )
        createAdapter();

    if ( !adapter_ )
    {
        // when we are not able to find an adapter within a reasonable amount of data then terminate
        if ( PEEK_SIZE < buffer_.size() )
        {
            qWarning() << "unable to determine adapter type, terminating connection";
            deleteLater();
        }

        return;
    }

    // find a session based on adapter in data in buffer
    if ( sessionPath_.isEmpty() )
        createSession();

    if ( sessionPath_.isEmpty() )
        return;

    // ---- //

    bool valid;

    // translate
    const QList<Event*> events( adapter_->translateComm( buffer_, &valid ) );

    if ( valid )
        recvErrors_ = 0;
    else
    {
        qWarning() << "recv error";
        ++recvErrors_;

        if ( MAX_ERRORS <= recvErrors_ )
        {
            adapter_->destroyEvents( events );

            qWarning() << "too many errors, terminating connection";

            deleteLater();
            return;
        }
    }

    // process events
    foreach ( const Event *e, events )
        send( SESSION_MANAGER_DBUS_SERVICE, sessionPath_, e );

    adapter_->destroyEvents( events );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::onTimeout()
{
    // check if we have valid client
    if ( peekTimer_ == sender() )
    {
        if ( !adapter_ )
        {
            qWarning() << "no adapter established, terminating connection";
            deleteLater();
        }
    }

    // check idle (dead) connection
    else if ( idleTimer_ == sender() )
    {
        qWarning() << "connection is idle (stale), terminating connection";
        deleteLater();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::createAdapter()
{
    // check for tag comms
    if ( 2 <= buffer_.size() )
    {
        const QString header( buffer_.left( 2 ).constData() );

        // check for "record type" or "data type" tags
        if (( "RT" == header ) || ( "DT" == header ))
        {
            qInfo() << "found tag comms";

            adapter_ = new TagEventAdapter;
            return;
        }
    }

    qDebug() << "could not identify comm type";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Connection::createSession()
{
    // TODO
    // determine session type

    com::hobartcorp::SessionManager *sessionMan = new com::hobartcorp::SessionManager(
        SESSION_MANAGER_DBUS_SERVICE,
        SESSION_MANAGER_DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus() );

    if ( !sessionMan )
    {
        qWarning() << "failed to allocate session manager";
        return;
    }

    if ( !sessionMan->isValid() )
        qWarning() << "invalid session manager";
    else
    {
        qDebug() << "create session" << sessionType_ << "...";
        sessionPath_ = sessionMan->createSession( sessionType_ );

        if ( sessionPath_.isEmpty() )
            qWarning() << "failed to create session!";
        else
        {
            // register as owner of this session
            SessionRegister e;

            send( SESSION_MANAGER_DBUS_SERVICE, sessionPath_, &e );
        }
    }

    delete sessionMan;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Connection::nextConnectionId()
{
    QMutexLocker guard( &m_ );

    // increment session counter
    QString id( QString::number( nextConnection_++, 16 ) );

    // return hex string of length 8 characters
    while ( id.length() < 8 )
        id.prepend( "0" );

    return id;
}
