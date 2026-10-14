/**
 * @file devicediscovery.h
 * BLE Device Listing
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QMap>
#include <QWidget>

class scannerAgent;

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluetooth device discovery widget.
class deviceDiscovery : public QWidget
{
    Q_OBJECT

    typedef deviceDiscovery _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent
     */
    deviceDiscovery( QWidget *parent = 0 );

    /// Destructor.
    virtual ~deviceDiscovery();

signals:

    /// Exit signal.
    void exit();

private slots:

    /// Slot for list item double clicked.
    void onListItemDoubleClicked( QListWidgetItem *item );

    /// Slot for agent device discovered.
    void onAgentDeviceDiscovered( const QBluetoothDeviceInfo &info );

private:

    scannerAgent *agent_;

    QMap<QBluetoothAddress, QString> devices_;
    QMap<QBluetoothAddress, QBluetoothDeviceInfo> tags_;

    // ---- //

    QLabel *header_;

    QListWidget *list_;

    QPushButton *exit_;

    /// Initialize.
    void initialize();

    /// Create Layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DEVICEDISCOVERY_H
