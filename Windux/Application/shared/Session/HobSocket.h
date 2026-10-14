//! HobSocket.h
/*! HobSocket sets up the socket, reads from the socket, and writes to the socket.
 *  Upon receiving a message HobSocket will pass the message on to Session to be
 *  parsed which consequently pass the message on for further processing.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef HOBSOCKET_H
#define HOBSOCKET_H

#include <QTcpSocket>

class QNetworkSession;

class HobSocket : public QTcpSocket
{
    Q_OBJECT

    typedef HobSocket _Myt;
    typedef QTcpSocket _Mybase;

public:
    /**
      * Constructor.
      */
    HobSocket();
    HobSocket( const QString& address );
    HobSocket( const QString& address, quint16 port );
    /**
     * Destructor.
     */
    virtual ~HobSocket();

    /**
     * Method called to determine if the socket is connected
     *
     * @param[in] none
     * @returns  @c true socket is connected, @c false otherwise
     */
    virtual bool isConnected() const;

private slots:
    /**
     * SLOT method used to display any socket error
     *
     * @param[in] socketError  socketError is the error encountered by the socket
     * @returns none
     */
    void displayError(QAbstractSocket::SocketError socketError);

    /**
     * Method used to open a session socket connection with the scale application
     *
     * @param[in] none
     * @returns none
     */
    void sessionOpened();

private:
    QNetworkSession *networkSession;
    bool haveConnection;

    static const quint16 hobPort;
    static const QString hobLocalHost;

    /**
     * The method checks to determine if a valid file was entered. If the filename
     * is an empty string, or one of a number of illegal characters, a warning will
     * be displayed to the user indicating the problem.
     *
     * @param[in] host  host is the port being connected, where 6000 is the default.
     * @param[in] address  address is the scale address being connected. Typically 127.0.0.1
     * @returns none
     */
    void init(quint16 host, const QString& address);

    /**
     * The method connects to the scale application with a TCP/IP socket
     *
     * @param[in] host  host is the port being connected, where 6000 is the default.
     * @param[in] address  address is the scale address being connected. Typically 127.0.0.1
     * @returns none
     */
    bool socketConnect(quint16 port, const QString& address);

    /**
     * The method disconnects from the scale application
     *
     * @param[in] none
     * @returns none
     */
    void socketDisconnect();

//protected:
    //    void run();
};

#endif // HOBSOCKET_H
