/**
 * @file clientalertswidget.h
 * Client alerts list widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CLIENTALERTSWIDGET_H
#define CLIENTALERTSWIDGET_H

#include <QByteArray>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QWidget>

class QPushButton;
class QTcpSocket;
class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Client alerts list widget.
class clientAlertsWidget : public QWidget
{
    Q_OBJECT

    typedef clientAlertsWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    clientAlertsWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~clientAlertsWidget();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process client disconnect.
    /**
     * @param[in,out] client  client socket
     */
    virtual void processDisconnect( QTcpSocket *client );

    /// Process client new connection.
    /**
     * @param[in,out] client  client socket
     */
    virtual void processNewConnection( QTcpSocket *client );

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] msg  message
     */
    virtual void processRequest( QTcpSocket *client, const QString &msg );

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

    /// Slot for button clicked.
    void onPushButtonClicked();

    /// Slot for item double clicked.
    void onTreeItemDoubleClicked( QTreeWidgetItem *item, int column );

private:

    enum clientState
    {
        Connected,
        Hello,
        Header,
        Data,
        Complete,
        Error
    };

    typedef QPair<clientState, QStringList> clientComm;
    typedef QPair<QTcpSocket*, clientComm> clientCommData;
    typedef QMap<int, clientCommData> clientCommDataMap;

    QTreeWidget *tree_;

    QPushButton *clear_;

    clientCommDataMap comms_;

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

#endif // CLIENTALERTSWIDGET_H
