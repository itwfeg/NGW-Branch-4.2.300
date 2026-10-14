/**
 * @file devicediscovery.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "devicediscovery.h"
#include "deviceinfo.h"

#include "scanneragent/hciscanneragent.h"

#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
deviceDiscovery::deviceDiscovery( QWidget *parent ) :
    _Mybase( parent ),
    agent_( NULL )
{
    // init
    initialize();
    createLayout();

    // create agent
    agent_ = new hciScannerAgent( this );

    connect( agent_, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), SLOT(onAgentDeviceDiscovered(QBluetoothDeviceInfo)) );

    // scan!
    agent_->startScanning();

    if ( !agent_->isScanningActive() )
    {
        qWarning() << agent_->errorString();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
deviceDiscovery::~deviceDiscovery()
{
    // destroy agent
    if ( agent_ )
        delete agent_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceDiscovery::onListItemDoubleClicked( QListWidgetItem *item )
{
    if ( !item )
        return;

    const QBluetoothAddress address( item->data( Qt::UserRole ).toString() );

    if ( !tags_.contains( address ) )
        return;

    qInfo() << "opening tag" << address.toString() << "...";

    // show dialog!
    deviceInfo dialog( tags_[address], this );
    dialog.exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceDiscovery::onAgentDeviceDiscovered( const QBluetoothDeviceInfo &info )
{
    if ( !info.isValid() )
        return;

    // check against devices
    const QBluetoothAddress address( info.address() );

    if ( devices_.contains( address ) )
        return;

    const QString addressStr( address.toString() );
    const QString nameStr( info.name() );

    // add to devices
    devices_[address] = nameStr;

    // add to list
    QListWidgetItem *item( new QListWidgetItem() );
    item->setText( addressStr + " " + nameStr );
    item->setData( Qt::UserRole, addressStr );

    list_->addItem( item );

    // check for hobart tag
    if ( info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration )
        if ( nameStr.startsWith( "ITWH" ) )
        {
            tags_[address] = info;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceDiscovery::initialize()
{
    header_ = new QLabel( this );
    header_->setText( tr( "Double click item for more information" ) );

    list_ = new QListWidget( this );

    connect( list_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onListItemDoubleClicked(QListWidgetItem*)) );

    exit_ = new QPushButton( this );
    exit_->setText( tr( "Exit" ) );

    connect( exit_, SIGNAL(clicked(bool)), SIGNAL(exit()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceDiscovery::createLayout()
{
    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->addWidget( header_ );
    formLayout->addWidget( list_ );
    formLayout->addWidget( exit_ );
}
