/**
 * @file webserverwidget.h
 * Web server widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBSERVERWIDGET_H
#define WEBSERVERWIDGET_H

#include "webserverstate.h"

#include <QByteArray>
#include <QMap>
#include <QTcpServer>
#include <QWidget>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTcpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Web server widget.
class webServerWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( webServerState::type state READ state WRITE setState NOTIFY stateChanged )

    typedef webServerWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    webServerWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~webServerWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve web server state.
    /**
     * @return  state
     */
    virtual webServerState::type state() const {return state_;}

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve web server state.
    /**
     * @return  state
     */
    virtual void setState( webServerState::type value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close web server.
    virtual void closeWebServer();

    /// Open web server.
    virtual void openWebServer();

Q_SIGNALS:

    /// Support legacy methods.
    /**
     * @param[in] enabled  @c true to support legacy methods
     */
    void legacySupportEnabled( bool enabled );

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] header  message header
     * @param[in] body  message body
     */
    void processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body );

    /// Emitted when web server state changes.
    /**
     * @param[in] newState  new state
     */
    void stateChanged( webServerState::type newState );

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

    /// Slot for radio button clicked.
    void onRadioButtonClicked();

    /// Slot for server accept error.
    void onServerAcceptError( QAbstractSocket::SocketError error );

    /// Slot for server new connection.
    void onServerNewConnection();

private:

    typedef QMap<QTcpSocket*, QByteArray> bufferMap;

    QTcpServer server_;
    bufferMap clientBuffers_;

    webServerState::type state_;

    QLabel *portLabel_;
    QLineEdit *port_;
    QPushButton *open_;
    QPushButton *close_;

    QPushButton *allowLegacy_;

    QGroupBox *options_;
    QRadioButton *okay_;
    QRadioButton *okayNoContent_;
    QRadioButton *incompleteHeader_;
    QRadioButton *malformedHeader_;
    QRadioButton *noResponse_;
    QRadioButton *closeSocket_;
    QRadioButton *moved_;
    QRadioButton *unauthorized_;
    QRadioButton *notFound_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WEBSERVERWIDGET_H
