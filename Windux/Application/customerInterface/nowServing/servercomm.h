/**
 * @file servercomm.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SERVERCOMM_H
#define SERVERCOMM_H

#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>

class QTcpServer;
class QSignalMapper;
class QUdpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Now Serving server communications.
class serverComm : public QObject
{
    Q_OBJECT

    typedef serverComm _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// Signal for when currently serving changes.
    /**
     * @param[in] currentlyServing  who is currently being served
     * @param[in] queueSize  queue size
     */
    void updateCurrentlyServing( int currentlyServing, int queueSize );

public:

    /// Server mode types.
    enum modeType
    {
        tcpServerMode,                              ///< TCP Socket Server.
        udpServerMode,                              ///< UDP Socket Server.
    };

    /// Error codes.
    enum errorCodes
    {
        // internal error codes
        ERROR_SENDING_REQUEST = -5,
        TIMEOUT_WAITING_FOR_CONNECTION = -4,
        TIMEOUT_WAITING_FOR_RESPONSE = -3,
        INVALID_RESPONSE_FROM_SERVER = -2,
        INVALID_RESPONSE_FORMAT = -1,

        // server error codes
        SUCCESS = 0,
        INVALID_REQUEST = 1,
        LICENSE_NOT_INSTALLED = 2,
        INVALID_QUEUE_SETUP = 3,
        NO_CUSTOMERS_QUEUED = 4,
        INVALID_PRINTER_TEMPLATE = 5,
        NO_PRINTER_ATTACHED = 6,
        INVALID_NUMBER = 7
    };

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve last error.
    /**
     * @return  error number
     */
    virtual int error() const {return lastError_;}

    /// Retrieve text string of last error.
    /**
     * @return  error test string
     */
    virtual QString errorString() const;

    /// Retrieve port.
    /**
     * @return  port
     */
    virtual int port() const {return port_;}

    /// Retrieve server
    /**
     * @return  server
     */
    virtual QString server() const {return server_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Increment serving queue.
    /**
     * @param[out] customer  customer number
     * @return  @c true on success, @c false otherwise
     */
    virtual bool incrementServingQueue( int& customer );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Allocate server instance.
    /**
     * @param[in] server  server
     * @param[in] port  port
     * @param[in] queue  queue name
     * @param[in] updatePort  port to listen for updates on
     * @param[in] mode  server mode
     * @param[in] parent  parent object
     */
    static void alloc( const QString& server, int port, const QString& queue, int updatePort, modeType mode = tcpServerMode, QObject *parent = 0 );

    /// Retrieve server instance.
    /**
     * @warning
     * You must allocate an instance before invoking this method. Otherwise you may get a NULL
     * pointer returned.
     *
     * @return  pointer to instance
     */
    static _Myt *instance() {return instance_;}

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send and receive comm.
    /**
    * @param[out] resp  server response
    * @param[in] cmd  server request
    * @param[in] param  server request parameter(s)
    * @return  @c true on success, @c false otherwise
    */
    virtual bool sendRecv( QStringList& resp, const QString& cmd, const QStringList& param = QStringList() );

private Q_SLOTS:

    /// Slot to handle disconnects.
    void onDisconnect( QObject *sock );

    /// Slot to handle incoming connection.
    void onNewConnection();

    /// Slot to handle incoming datagram message.
    void onReadDatagram();

    /// Slot to handle socket reads.
    void onReadyRead( QObject *sock );

private:

    QSignalMapper *disconnectSignal_;
    QSignalMapper *readyReadSignal_;

    QString server_;
    int port_;
    QString queue_;
    int updatePort_;
    modeType mode_;

    QByteArray buffer_;

    QTcpServer *tcpUpdates_;
    QUdpSocket *udpUpdates_;

    int lastError_;

    static QMutex instanceMutex_;
    static serverComm *instance_;

    /// Constructor.
    serverComm( const QString& server, int port, const QString& queue, int updatePort, modeType mode = tcpServerMode, QObject *parent = 0 );

    /// Destructor.
    ~serverComm();

    /// Initialize.
    void initialize();

    /// Process buffer.
    void processBuffer();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERVERCOMM_H
