/**
 * @file networkinfodialog.h
 * Network information dialog.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef NETWORKINFODIALOG_H
#define NETWORKINFODIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPushButton;
class QStackedWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Network information dialog.
class NetworkInfoDialog : public SessionDialog
{
    Q_OBJECT

    typedef NetworkInfoDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit NetworkInfoDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~NetworkInfoDialog();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Refresh information
    void onRefreshButtonPressed();

private:

    QStackedWidget *pages_;
    QWidget *page_[2];

    //
    // page 0
    //

    QLabel *pleaseWaitMessage_;

    //
    // page 1
    //

    QLabel *hostnameLabel_;
    QLabel *hostname_;

    QLabel *macAddressLabel_;
    QLabel *macAddress_;

    QLabel *ipAddressLabel_;
    QLabel *ipAddress_;

    QLabel *dhcpLabel_;
    QLabel *dhcp_;

    QLabel *gatewayLabel_;
    QLabel *gateway_;

    QLabel *connectSpeedLabel_;
    QLabel *connectSpeed_;

    QLabel *connectTimeLabel_;
    QLabel *connectTime_;

    QLabel *ssidLabel_;
    QLabel *ssid_;

    QLabel *channelLabel_;
    QLabel *channel_;

    QLabel *signalToNoiseLabel_;
    QLabel *signalToNoise_;

    QPushButton *refresh_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NETWORKINFODIALOG_H
