/* HobSocket.cpp
 * HobSocket sets up the socket, reads from the socket, and writes to the socket.
 * Upon receiving a message HobSocket will pass the message on to Session to be
 * parsed which consequently pass the message on for further processing.
 * Author: Joe E. Crew/Randy Blankley
 */

#include <logmanager.h>

#include <QtNetwork>
#include "HobSocket.h"
#include "Session.h"

#define HEADER_SIZE 6

const quint16 HobSocket::hobPort = 6000;
const QString HobSocket::hobLocalHost( QHostAddress( QHostAddress::LocalHost ).toString() );

/** HobSocket()
 *
 *   The HobSocket() constructor, initialize and set up the network configuration manager
 *   params:  none
 *   returns: void
 *   Bugs\Issues: None
 */
HobSocket::HobSocket() : _Mybase(), networkSession(0)
{
    init( hobPort, hobLocalHost );
        }

/** HobSocket()
 *
 *   The HobSocket() constructor, initialize and set up the network configuration manager
 *   address: The host, typically 127.0.0.1
 *   returns: void
 *   Bugs\Issues: None
 */
HobSocket::HobSocket( const QString& address ) : _Mybase(), networkSession(0)
{
    init( hobPort, address );
     }

/** HobSocket()
 *
 *   The HobSocket() constructor, initialize and set up the network configuration manager
 *   address: The host, typically 127.0.0.1
 *   port:    The port the Hobart Appication communicates on, typically 6000
 *   returns: void
 *   Bugs\Issues: None
 */
HobSocket::HobSocket( const QString& address, quint16 port ) : _Mybase(), networkSession(0)
{
    init( port, address );
}

/** ~HobSocket()
 *
 *   ~HobSocket() is the class destructor cleaning stuff up
 *   params: None
 *   returns: Nothing
 *   Bugs\Issues: None known - need to make sure this is called to disconnect the socket
 */
HobSocket::~HobSocket()
{
    socketDisconnect();

    delete networkSession;
}

/** isConnected()
 *
 *   isConnected() checks to see if socket is connected
 *   params:  None
 *   returns: if socket is connected
 *   Bugs\Issues: None known
 */
bool HobSocket::isConnected() const
{
    return haveConnection;
}

/** init()
 *
 *  init() initializes everything and starts the connection process with the scale application
 *  port:    The port the Hobart Appication communicates on, typically 6000
 *  address:  The address the Hobart Appication communicates on, typically 127.0.0.1
 *  Bugs\Issues: None known - need to make sure this is called to disconnect the socket
 */
void HobSocket::init(quint16 port, const QString& address)
{
    QNetworkConfigurationManager manager;                           // Get saved network configuration

    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        QSettings settings(QSettings::UserScope, QLatin1String("Hobart"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();


        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);

        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        networkSession->open();
     }

    haveConnection = socketConnect(port, address);

    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

}

/** socketConnect()
 *
 *   The socketConnect() method is called by the HobSocket.init() method.
 *   address: The local host, typically 127.0.0.1
 *   port:    The port the Hobart Appication communicates on, typically 6000
 *   returns: nothing
 *   Bugs\Issues: Currently these values passed in are address 127.0.0.1 and
 *   port 6000. The port is okay, but for remote access we will need some other
 *   way to pass in some other address besides 127.0.0.1
 */
bool HobSocket::socketConnect(quint16 port, const QString& address)
{
    LOG_CLASS_INFO( "HobSocket", "socketConnect" );

    abort();                                            // clear out any previous sockets
    connectToHost( address, port );                     // connect to the Application using port and address

    if ( !waitForConnected() )
    {
        QString errorStr = errorString();               // get any errors

        LOG_ERROR( "Socket Error %s", errorStr.toLatin1().constData()  );
        return false;
    }

    LOG_INFO( "connected!", NULL );
    return true;
}

/** socketDisconnect()
 *
 *   The socketDisconnect() disconnects from the host, which is the Hobart application.
 *   params: None
 *   returns: void
 *   Bugs\Issues: None known
 */
void HobSocket::socketDisconnect()
{
    LOG_CLASS_INFO( "HobSocket", "socketDisconnect" );

    LOG_WARN( "Is this being called? Yes if I see this.", NULL );

    disconnectFromHost();
}

/** displayError()
 *
 *   The displayError() displays any error that is sent to it as a socketError. Errors are
 *   triggered tcpSocket SIGNAL which in turn calls the displayError SLOT.
 *   socketError: Any error triggered by the tcpSocket
 *   returns:     void
 *   Bugs\Issues: Needs to be fully implemented.
 */
void HobSocket::displayError(QAbstractSocket::SocketError socketError)
{
    LOG_CLASS_INFO( "HobSocket", "displayError" );

    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        LOG_ERROR( "socket error RemoteHostClosedError %d '%s'", socketError, errorString().toLatin1().constData() );
        break;
    case QAbstractSocket::HostNotFoundError:
        LOG_ERROR( "socket error HostNotFoundError %d '%s'", socketError, errorString().toLatin1().constData() );
        break;
    case QAbstractSocket::ConnectionRefusedError:
        LOG_ERROR( "socket error ConnectionRefusedError %d '%s'", socketError, errorString().toLatin1().constData() );
        break;
    default:
        LOG_ERROR( "socket error default %d '%s'", socketError, errorString().toLatin1().constData() );
        break;
    }
}

/** sessionOpened()
 *
 *  The sessionOpened() is triggered by an attempted network connect SIGNAL which in turn
 *  calls the sessionOpened() SLOT. Saves the current user network configuration.
 *  params:  none
 *  returns: void
 *  Bugs\Issues: I think we can get rid of this in the future and we don't need to use it.
 */
void HobSocket::sessionOpened()
{
    QNetworkConfiguration config = networkSession->configuration();
    QString id;

    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

/* should I do something like this??? */
/*
void Synapse::run(){

    connect(this,SIGNAL(get_next_packet()),this,SLOT(send_next_packet()));
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(receive_packet()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(close()));
    connect(parent,SIGNAL(destroyed()),this,SLOT(close()));
}
*/
