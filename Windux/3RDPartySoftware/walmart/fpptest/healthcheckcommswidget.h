/**
 * @file healthcheckcommswidget.h
 * Health Check communication list widget.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HEALTHCHECKCOMMSWIDGET_H
#define HEALTHCHECKCOMMSWIDGET_H

#include "webserverstate.h"

#include <QByteArray>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QWidget>

class QJsonObject;
class QLabel;
class QPushButton;
class QTcpSocket;
class QTreeWidget;
class QTreeWidgetItem;
class QUuid;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Health Check communication list widget.
class healthCheckCommsWidget : public QWidget
{
    Q_OBJECT

    typedef healthCheckCommsWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    healthCheckCommsWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~healthCheckCommsWidget();

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set event.
    /**
     * @param[in] event  event name
     * @param[in] enabled  @c true to enable, @c false otherwise
     * @param[in] url  event url
     * @param[in] forced  @c true to force callback event, @c false otherwise
     * @param[in] urlCallback  callback url
     * @param[in] file  filename
     * @param[in] parts  number of parts
     */
    virtual void setEvent( const QString& event, bool enabled, const QString& url, bool forced, const QString& urlCallback, const QString& file, int parts );

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
    virtual void sendResponseJSON( QTcpSocket *client, const QString &responseCode, const QByteArray &body );

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
     * @param[in] filename  filename
     */
    virtual void sendResponseFILE( QTcpSocket *client, const QString &responseCode, const QString &filename );

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

    clientCommDataMap requestComms_;
    clientCommDataMap responseComms_;

    typedef QMap<QString, bool> eventEnabledMap;
    typedef QMap<QString, int> eventPartsMap;
    typedef QMap<QString, QString> eventUrlMap;

    eventEnabledMap events_;
    eventUrlMap eventUrl_;
    eventEnabledMap eventForced_;
    eventUrlMap eventUrlCallback_;
    eventUrlMap eventFile_;
    eventPartsMap eventParts_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set tree headers.
    void setTreeHeaders();

    /// Send response.
    void sendResponse( QTcpSocket *client, const QString &responseCode, const QString &contentType, const QByteArray &body, const QString &attachmentName = QString() );

    /// Create new configuration.
    QJsonObject newConfig( const QUuid& uuid, const QJsonObject& request ) const;

    /// Update response to include file download information.
    void createFileDownload( QJsonObject& response, const QString& e ) const;

    /// Break up firmware into multiple parts.
    static void createMetadataForFirmware( QJsonObject& metadata, const QString& filename, int parts );

    /// Give all tag files to caller.
    static void createMetadataForLabels( QJsonObject& metadata, const QString& folder );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HEALTHCHECKCOMMSWIDGET_H
