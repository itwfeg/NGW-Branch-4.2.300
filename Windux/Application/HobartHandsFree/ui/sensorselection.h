/**
 * @file sensorselection.h
 * Widget for sensor selection.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SENSORSELECTION
#define SENSORSELECTION

#include <QList>
#include <QString>
#include <QWidget>

class commClient;
class deviceList;
class message;

class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for sensor selection.
class sensorSelectionWidget : public QWidget
{
    Q_OBJECT

    typedef sensorSelectionWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when item double clicked.
    /**
     * @param[in] id  sensor id
     */
    void itemDoubleClicked( const QString &id );

    /// Emitted when selection changes.
    void itemSelectionChanged();

    /// Emitted when refresh pending.
    /**
     * @param[in] pending  @c true if pending, @c false otherwise
     */
    void updateRefreshPending( bool pending );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    sensorSelectionWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~sensorSelectionWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve selected items.
    /**
     * @return  list of selected items
     */
    virtual QList<QString> selectedItems() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

public Q_SLOTS:

    /// Clear selection.
    virtual void clearSelection();

    /// Refresh sensors.
    virtual void refresh();

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

    /// Slot for receive message.
    void onRecvMessage( const message *msg );

    /// Slot for item double clicked.
    void onItemDoubleClicked( QTreeWidgetItem *item, int column );

    /// Slot for timer timeout.
    void onTimeout();

private:

    enum {REFRESH_TIMER = 10000};

    enum {BATTERY_WARNING_FROM = 10};
    enum {BATTERY_NORMAL_FROM = 25};

    enum {RSSI_EXCELLENT_FROM = -50};
    enum {RSSI_GOOD_FROM = -60};
    enum {RSSI_FAIR_FROM = -70};

    enum {COLUMN_ID = 0};
    enum {COLUMN_OPER_NAME = 1};
    enum {COLUMN_FIRMWARE_VERSION = 2};
    enum {COLUMN_BATTERY_LEVEL = 3};
    enum {COLUMN_RSSI = 4};

    commClient *client_;

    bool refreshPending_;

    QTreeWidget *table_;

    QTimer *refreshTimer_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Process message.
    void processMessage( const deviceList *msg );

    /// Set table headers.
    void setTableHeaders();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SENSORSELECTION

