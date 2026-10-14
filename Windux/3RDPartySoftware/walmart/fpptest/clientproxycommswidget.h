/**
 * @file clientproxycommswidget.h
 * Client proxy communications list widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CLIENTPROXYCOMMSWIDGET_H
#define CLIENTPROXYCOMMSWIDGET_H

#include <QAbstractSocket>
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

/// Client proxy communications list widget.
class clientProxyCommsWidget : public QWidget
{
    Q_OBJECT

    typedef clientProxyCommsWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    clientProxyCommsWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~clientProxyCommsWidget();

public Q_SLOTS:

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

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for client connected.
    void onClientConnected();

    /// Slot for client destroyed.
    void onClientDestroyed();

    /// Slot for client error.
    void onClientError( QAbstractSocket::SocketError error );

    /// Slot for client ready read.
    void onClientReadyRead();

    /// Slot for button clicked.
    void onPushButtonClicked();

    /// Slot for item double clicked.
    void onTreeItemDoubleClicked( QTreeWidgetItem *item, int column );

private:

    enum clientState
    {
        Connecting,
        Active,
        Complete,
        Error
    };

    struct controlBlock
    {
        int index_;
        QString type_;
        QByteArray pending_;
        clientState state_;
        QTcpSocket *client_;
        QTcpSocket *server_;
        quint64 send_;
        quint64 recv_;
    };

    typedef QList<controlBlock*> controlBlockList;
    typedef QMap<QTcpSocket*, controlBlock*> clientCommDataMap;
    typedef QMap<QTreeWidgetItem*, controlBlock*> treeDataMap;

    QTreeWidget *tree_;

    QLabel *setup_;
    QPushButton *clear_;

    controlBlockList blocks_;
    clientCommDataMap comms_;
    treeDataMap items_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set tree headers.
    void setTreeHeaders();

    /// Update tree item.
    void updateTreeItem( int index );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CLIENTPROXYCOMMSWIDGET_H
