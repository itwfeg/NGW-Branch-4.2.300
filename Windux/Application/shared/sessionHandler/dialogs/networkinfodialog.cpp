/**
 * @file networkinfodialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "networkinfodialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
NetworkInfoDialog::NetworkInfoDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "NetworkInfoDialog" );

    // initialize interface
    initialize();

    // create layout
    createLayout();

    // request current network status to quickly update the NIC type
    session_->send( NetworkConnectivityDev, RequestNetworkDeviceType );

    // request network info
    session_->send( SessionDev, SnReadNetworkInfo );

    // FT-415
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NetworkInfoDialog::~NetworkInfoDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkInfoDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "NetworkInfoDialog", "onSessionMessageReceive" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // network info
    if (( SessionDev == device ) && ( SnWriteNetworkInfo == event ))
    {
        MadpNetworkInfo m( msg );

        QStringList params;
        for ( int i = 2; i < m.size(); ++i )
            params.append( m[i] );

        QList<QLabel*> fields;
        fields.append( hostname_ );
        fields.append( macAddress_ );
        fields.append( ipAddress_ );
        fields.append( dhcp_ );
        fields.append( gateway_ );
        fields.append( NULL );
        fields.append( connectSpeed_ );
        fields.append( connectTime_ );
        fields.append( ssid_ );
        fields.append( channel_ );
        fields.append( signalToNoise_ );

        // process each parameter
        foreach ( const QString& p, params )
        {
            if ( fields.isEmpty() )
            {
                LOG_WARN( "have more parameters than fields!", NULL );
                break;
            }

            // display it
            QLabel *l( fields.takeFirst() );

            if ( !l )
                continue;

            l->setText( p );
        }

        // show results
        pages_->setCurrentWidget( page_[1] );
    }

    // network device information
    else if (( NetworkConnectivityDev == device ) && ( NetworkDeviceType == event ))
    {
        MadpNetworkDeviceType m( msg );

        // for non-wireless network we need to hide wifi specific info
        if ( "1" != m.deviceType() )
        {
            ssidLabel_->hide();
            ssid_->hide();

            channelLabel_->hide();
            channel_->hide();

            signalToNoiseLabel_->hide();
            signalToNoise_->hide();
        }
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkInfoDialog::onRefreshButtonPressed()
{
    // waiting...
    pages_->setCurrentWidget( page_[0] );

    // request network info
    session_->send( SessionDev, SnReadNetworkInfo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkInfoDialog::initialize()
{
    // create pages
    pages_ = new QStackedWidget( this );
    pages_->addWidget( page_[0] = new QWidget( pages_ ) );
    pages_->addWidget( page_[1] = new QWidget( pages_ ) );

    pages_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    page_[0]->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    page_[1]->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    QWidget *currentPage;

    //
    // page 0
    //

    currentPage = page_[0];

    // create widgets
    pleaseWaitMessage_ = new QLabel( tr( "Retrieving network information... please wait..." ), currentPage );
    pleaseWaitMessage_->setAlignment( Qt::AlignCenter );
    pleaseWaitMessage_->setObjectName( "pleaseWaitMessage" );

    //
    // page 1
    //

    currentPage = page_[1];

    // create widgets
    hostnameLabel_ = new QLabel( tr( "Scale Host Name:" ), currentPage );
    hostnameLabel_->setObjectName( "hostnameLabel" );

    hostname_ = new QLabel( currentPage );
    hostname_->setObjectName( "hostname" );

    macAddressLabel_ = new QLabel( tr( "Scale MAC Address:" ), currentPage );
    macAddressLabel_->setObjectName( "macAddressLabel" );

    macAddress_ = new QLabel( currentPage );
    macAddress_->setObjectName( "macAddress" );

    ipAddressLabel_ = new QLabel( tr( "Scale IP Address:" ), currentPage );
    ipAddressLabel_->setObjectName( "ipAddressLabel" );

    ipAddress_ = new QLabel( currentPage );
    ipAddress_->setObjectName( "ipAddress" );

    dhcpLabel_ = new QLabel( tr( "DHCP Server:" ), currentPage );
    dhcpLabel_->setObjectName( "dhcpLabel" );

    dhcp_ = new QLabel( currentPage );
    dhcp_->setObjectName( "dhcp" );

    gatewayLabel_ = new QLabel( tr( "Gateway:" ), currentPage );
    gatewayLabel_->setObjectName( "gatewayLabel" );

    gateway_ = new QLabel( currentPage );
    gateway_->setObjectName( "gateway" );

    connectSpeedLabel_ = new QLabel( tr( "Connection Speed:" ), currentPage );
    connectSpeedLabel_->setObjectName( "connectSpeedLabel" );

    connectSpeed_ = new QLabel( currentPage );
    connectSpeed_->setObjectName( "connectSpeed" );
    /*
     * SOF-1685: There is an issue with the ath10k driver that causes the
     * the connection speed to always be reported as 6mbps even though the
     * device connects at different rates. If we re-enable connectSpeed_ we
     * need to fix the ath10k driver. Fields are also commented out in
     * NetworkInfoDialog::createLayout()
    */
    connectSpeedLabel_->hide();
    connectSpeed_->hide();

    connectTimeLabel_ = new QLabel( tr( "Connected For:" ), currentPage );
    connectTimeLabel_->setObjectName( "connectTimeLabel" );

    connectTime_ = new QLabel( currentPage );
    connectTime_->setObjectName( "connectTime" );
    connectTimeLabel_->hide();
    connectTime_->hide();

    ssidLabel_ = new QLabel( tr( "SSID:" ), currentPage );
    ssidLabel_->setObjectName( "ssidLabel" );

    ssid_ = new QLabel( currentPage );
    ssid_->setObjectName( "ssid" );

    channelLabel_ = new QLabel( tr( "Channel:" ), currentPage );
    channelLabel_->setObjectName( "channelLabel" );

    channel_ = new QLabel( currentPage );
    channel_->setObjectName( "channel" );

    signalToNoiseLabel_ = new QLabel( tr( "Signal to Noise:" ), currentPage );
    signalToNoiseLabel_->setObjectName( "signalToNoiseLabel" );

    signalToNoise_ = new QLabel( currentPage );
    signalToNoise_->setObjectName( "signalToNoise" );

    refresh_ = new QPushButton( tr( "Refresh" ), currentPage );
    connect( refresh_, SIGNAL( pressed() ), SLOT( onRefreshButtonPressed() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkInfoDialog::createLayout()
{
    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addWidget( pages_ );

    QVBoxLayout *page0Layout = new QVBoxLayout( page_[0] );
    page0Layout->addStretch();
    page0Layout->addWidget( pleaseWaitMessage_ );
    page0Layout->addStretch();

    QFormLayout *page1LayoutTop = new QFormLayout();
    page1LayoutTop->addRow( hostnameLabel_, hostname_ );
    page1LayoutTop->addRow( macAddressLabel_, macAddress_ );
    page1LayoutTop->addRow( ipAddressLabel_, ipAddress_ );
    page1LayoutTop->addRow( dhcpLabel_, dhcp_ );
    page1LayoutTop->addRow( gatewayLabel_, gateway_ );
    /*
     * SOF-1685: There is an issue with the ath10k driver that causes the
     * the connection speed to always be reported as 6mbps even though the
     * device connects at different rates. If we re-enable connectSpeed_ we
     * need to fix the ath10k driver. Fields are also hidden in NetworkInfoDialog::initialize()
    */
    //page1LayoutTop->addRow( connectSpeedLabel_, connectSpeed_ );

    //page1LayoutTop->addRow( connectTimeLabel_, connectTime_ );
    page1LayoutTop->addRow( ssidLabel_, ssid_ );
    page1LayoutTop->addRow( channelLabel_, channel_ );
    page1LayoutTop->addRow( signalToNoiseLabel_, signalToNoise_ );

    QHBoxLayout *page1LayoutBottom = new QHBoxLayout();
    page1LayoutBottom->addStretch();
    page1LayoutBottom->addWidget( refresh_ );

    QVBoxLayout *page1Layout = new QVBoxLayout( page_[1] );
    page1Layout->addLayout( page1LayoutTop );
    page1Layout->addStretch();
    page1Layout->addLayout( page1LayoutBottom );

}
