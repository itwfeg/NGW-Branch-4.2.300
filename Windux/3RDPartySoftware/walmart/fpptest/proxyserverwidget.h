/**
 * @file proxyserverwidget.h
 * Proxy server widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PROXYSERVERWIDGET_H
#define PROXYSERVERWIDGET_H

#include <QByteArray>
#include <QMap>
#include <QTcpServer>
#include <QWidget>

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Proxy server widget.
class proxyServerWidget : public QWidget
{
    Q_OBJECT

    typedef proxyServerWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    proxyServerWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~proxyServerWidget();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close proxy server.
    virtual void closeProxyServer();

    /// Open proxy server.
    virtual void openProxyServer();

Q_SIGNALS:

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] header  message header
     * @param[in] body  message body
     */
    void processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body );

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

    enum clientAllowedResult
    {
        ClientAllowed,
        ClientBadRequest,
        ClientAuthenticationRequired,
        ClientUnauthorized,
        ClientForbidden,
    };

    typedef QMap<QTcpSocket*, QByteArray> bufferMap;

    QTcpServer server_;
    bufferMap clientBuffers_;

    QLabel *portLabel_;
    QLineEdit *port_;
    QLabel *allowedLabel_;
    QLineEdit *allowed_;
    QPushButton *open_;
    QPushButton *close_;

    QGroupBox *options_;
    QCheckBox *denyEverything_;
    QCheckBox *denyHobart_;
    QCheckBox *denyGoogle_;
    QCheckBox *denyYahoo_;
    QCheckBox *denyMSN_;
    QCheckBox *requireLogin_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check if client is allowed.
    clientAllowedResult isClientAllowed( QTcpSocket *client, const QStringList &header ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PROXYSERVERWIDGET_H
