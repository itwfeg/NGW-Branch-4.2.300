/**
 * @file uartedgify.h
 * Edgify API UART Wrapper.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef UARTEDGIFY_H
#define UARTEDGIFY_H

#include <QByteArray>
#include <QMutex>
#include <QObject>
#include <QSerialPort>

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////
class edgifyuart : public QObject
{
    Q_OBJECT

    typedef edgifyuart _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for document processing.
    /**
     * @param[in] request  request
     * @param[in] status  request status
     * @param[in] response  response
     */
    void processDocument( const QJsonDocument& request, int status, const QJsonDocument& response );

    /// Send command.
    /**
     * @param[in] cmd  command
     * @param[in] a  payload
     */
    void sendCommand( const QString& cmd, const QByteArray& a = QByteArray() );

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] name  serial port name
     * @param[in] baud  serial port baud rate
     * @param[in,out] parent  parent object
     */
    edgifyuart( const QString& name, int baud = 115200, QObject *parent = nullptr );

    /// Destructor.
    virtual ~edgifyuart();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check if object is valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool valid() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Force reset of serial port.
    virtual void forceReset();

public slots:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Execute command.
    /**
     * @param[in] cmd  command
     */
    virtual void executeCommand( const QString& cmd );

    /// Request prediction.
    virtual void predict();

    /// Set current date.
    virtual void setDate();

    /// Set ground truth.
    /**
     * @param[in] doc  document
     */
    virtual void setGroundTruth( const QJsonDocument& doc );

private slots:

    /// Slot for serial port error.
    void onErrorOccurred( QSerialPort::SerialPortError error );

    /// Slot for ready read.
    void onReadyRead();

    /// Send command.
    void onSendCommand( const QString& cmd, const QByteArray& a = QByteArray() );

    /// Slot for timer timeout.
    void onTimeout();

private:

    enum {PING_TIMEOUT = 8000};
    enum {WRITE_TIMEOUT = 4000};

    enum {STATS_REQUEST = 32};

    mutable QMutex m_;

    QTimer *pingTimer_;

    QByteArray buffer_;

    bool processing_;

    QSerialPort *uart_;

    int errors_;
    int resets_;

    int stats_;

    /// Process command.
    void processCommand( const QString& cmd, const QByteArray& a = QByteArray() );

    /// Parse buffer.
    void parseBuffer();

    /// Reset serial port.
    void resetSerialPort();

    // not implemented
    edgifyuart( const _Myt& ) = delete;

    // not implemented
    _Myt &operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UARTEDGIFY_H
