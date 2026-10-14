/**
 * @file clientcommswidget.h
 * Client communication list widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CLIENTCOMMSWIDGET_H
#define CLIENTCOMMSWIDGET_H

#include "webserverstate.h"

#include <QByteArray>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QWidget>

class QLabel;
class QPushButton;
class QTcpSocket;
class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Client communication list widget.
class clientCommsWidget : public QWidget
{
    Q_OBJECT

    typedef clientCommsWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    clientCommsWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~clientCommsWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve allow legacy methods.
    /**
     * @return  @c true to allow legacy methods, @c false otherwise
     */
    virtual bool legacyUpdateAllowed() const  {return allowLegacy_;}

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Allow legacy methods.
    /**
     * @param[in] value  @c true to allow legacy methods, @c false otherwise
     */
    virtual void setLegacyUpdateAllowed( bool value ) {allowLegacy_ = value;}

    /// Set server state.
    /**
     * @param[in] value  new state
     */
    virtual void setServerState( webServerState::type value ) {state_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] header  message header
     * @param[in] body  message body
     */
    virtual void processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body );

protected:

    bool allowLegacy_;                              ///< Allow legacy methods.

    webServerState::type state_;                    ///< Server state.

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send response to client.
    /**
     * @param[in,out] client  client socket
     * @param[in] responseCode  response code
     */
    virtual void sendResponse( QTcpSocket *client, const QString &responseCode );

    /// Send response to client.
    /**
     * @param[in,out] client  client socket
     * @param[in] responseCode  response code
     * @param[in] body  message body
     */
    virtual void sendResponseCSS( QTcpSocket *client, const QString &responseCode, const QByteArray &body );

    /// Send response to client.
    /**
     * @param[in,out] client  client socket
     * @param[in] responseCode  response code
     * @param[in] body  message body
     */
    virtual void sendResponseHTML( QTcpSocket *client, const QString &responseCode, const QByteArray &body );

    /// Send response to client.
    /**
     * @param[in,out] client  client socket
     * @param[in] responseCode  response code
     * @param[in] body  message body
     */
    virtual void sendResponsePAC( QTcpSocket *client, const QString &responseCode, const QByteArray &body );

private Q_SLOTS:

    /// Slot for button clicked.
    void onPushButtonClicked();

    /// Slot for item double clicked.
    void onTreeItemDoubleClicked( QTreeWidgetItem *item, int column );

private:

    typedef QPair<QStringList, QByteArray> clientCommData;
    typedef QMap<int, clientCommData> clientCommDataMap;

    QTreeWidget *tree_;

    QLabel *setup_;
    QPushButton *clear_;

    clientCommDataMap comms_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set tree headers.
    void setTreeHeaders();

    /// Send response.
    void sendResponse( QTcpSocket *client, const QString &responseCode, const QString &contentType, const QByteArray &body );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CLIENTCOMMSWIDGET_H
