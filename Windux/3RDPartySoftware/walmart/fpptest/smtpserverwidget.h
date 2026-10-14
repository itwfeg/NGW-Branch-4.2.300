/**
 * @file smtpserverwidget.h
 * SMTP server widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SMTPSERVERWIDGET_H
#define SMTPSERVERWIDGET_H

#include <QByteArray>
#include <QMap>
#include <QTcpServer>
#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// SMTP server widget.
class smtpServerWidget : public QWidget
{
    Q_OBJECT

    typedef smtpServerWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    smtpServerWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~smtpServerWidget();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close mail server.
    virtual void closeMailServer();

    /// Open mail server.
    virtual void openMailServer();

Q_SIGNALS:

    /// Process client disconnect.
    /**
     * @param[in,out] client  client socket
     */
    void processDisconnect( QTcpSocket *client );

    /// Process client new connection.
    /**
     * @param[in,out] client  client socket
     */
    void processNewConnection( QTcpSocket *client );

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] msg  message
     */
    void processRequest( QTcpSocket *client, const QString &msg );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process client buffer.
    /**
     * @param[in,out] client  client socket
     * @param[in,out] buffer  buffer
     */
    virtual void processClientBuffer( QTcpSocket *client, QByteArray &buffer );

private Q_SLOTS:

    /// Slot for client destroyed.
    void onClientDestroyed();

    /// Slot for client ready read.
    void onClientReadyRead();

    /// Slot for button clicked.
    void onPushButtonClicked();

    /// Slot for server accept error.
    void onServerAcceptError( QAbstractSocket::SocketError error );

    /// Slot for server new connection.
    void onServerNewConnection();

private:

    typedef QMap<QTcpSocket*, QByteArray> bufferMap;

    QTcpServer server_;
    bufferMap clientBuffers_;

    QLabel *portLabel_;
    QLineEdit *port_;
    QPushButton *open_;
    QPushButton *close_;

    QPushButton *allowLegacy_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SMTPSERVERWIDGET_H
