/**
 * @file sensorselection.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "commclient.h"
#include "sensorselection.h"

#include <devicelist.h>
#include <devicelistreq.h>

#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorSelectionWidget::sensorSelectionWidget( QWidget *parent ) :
    _Mybase( parent ),
    client_( commClient::instance() ),
    refreshPending_( false ),
    refreshTimer_( new QTimer( this ) )
{
    // init
    initialize();
    createLayout();
    translate();

    // connect client
    connect( client_, SIGNAL(recvMessage(const message*)), SLOT(onRecvMessage(const message*)) );

    // refresh timer
    refresh();

    connect( refreshTimer_, SIGNAL(timeout()), SLOT(onTimeout()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorSelectionWidget::~sensorSelectionWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QString> sensorSelectionWidget::selectedItems() const
{
    QList<QString> results;

    // grab selected items
    QList<QTreeWidgetItem*> items( table_->selectedItems() );

    // return list of sensor id to caller
    foreach ( const QTreeWidgetItem *item, items )
        results.append( item->data( 0, Qt::DisplayRole ).toString() );

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::translate()
{
    setTableHeaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::clearSelection()
{
    table_->clearSelection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::refresh()
{
    // request device list
    // high tx power advertisements from last 30s
    deviceListRequest request( deviceListRequest::HFPD_HIGH_POWER_ADVERT, 30 );

    client_->sendMessage( &request );

    // reset timer
    refreshTimer_->start( REFRESH_TIMER );

    Q_EMIT updateRefreshPending( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::resizeEvent( QResizeEvent *e )
{
    setTableHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::onRecvMessage( const message *msg )
{
    // device list
    if ( message::HFPD_DEV_LIST == msg->type_ )
    {
        const deviceList *request( dynamic_cast<const deviceList*>( msg ) );

        if ( !request )
            return;

        processMessage( request );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::onItemDoubleClicked( QTreeWidgetItem *item, int column )
{
    Q_UNUSED( column );

    if ( !item )
        return;

    Q_EMIT itemDoubleClicked( item->data( COLUMN_ID, Qt::DisplayRole ).toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::onTimeout()
{
    if ( refreshTimer_ == sender() )
        refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::initialize()
{
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionBehavior( QAbstractItemView::SelectRows );
    table_->setSelectionMode( QAbstractItemView::SingleSelection );
    table_->setUniformRowHeights( true );
    table_->setFocusPolicy( Qt::NoFocus );

    connect( table_, SIGNAL(itemSelectionChanged()), SIGNAL(itemSelectionChanged()) );
    connect( table_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addWidget( table_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::processMessage( const deviceList *msg )
{
    // make list of top level items
    QList<QTreeWidgetItem*> doomed;

    for ( int i( table_->topLevelItemCount() ); i--;  )
        doomed.append( table_->topLevelItem( i ) );

    // update devices
    for ( size_t i( 0 ); i < msg->size(); ++i )
    {
        const QString id( QString::fromStdString( msg->address_[i] ) );

        QString operatorName( QString::fromStdString( msg->operatorName_[i] ) );
        QString firmware;
        QString battery;
        QString rssi;

        QColor batteryColor( GREEN );
        QColor rssiColor( GREEN );

        // firmware version
        firmware = QString::number( (msg->firmwareVersion_[i] >> 8) & 0xFF ) + "." +
            QString::number( msg->firmwareVersion_[i] & 0xFF );

        // battery level (percent)
        if ( !msg->batteryLevel_[i] )
            battery = tr( "Unknown" );
        else
        {
            if ( (2*BATTERY_NORMAL_FROM) < msg->batteryLevel_[i] )
                battery = tr( "Excellent" );
            else if ( BATTERY_NORMAL_FROM < msg->batteryLevel_[i] )
                battery = tr( "Good" );
            else if ( BATTERY_WARNING_FROM < msg->batteryLevel_[i] )
            {
                battery = tr( "Okay" );
                batteryColor = YELLOW;
            }
            else
            {
                battery = tr( "Low" );
                batteryColor = RED;
            }

#ifdef QT_DEBUG
            battery += " (" + QString::number( msg->batteryLevel_[i] ) + "%)";
#endif
        }

        // rssi
        if ( !msg->rssi_[i] )
            rssi = tr( "Unknown" );
        else
        {
            if ( RSSI_EXCELLENT_FROM < msg->rssi_[i] )
                rssi = tr( "Excellent" );
            else if ( RSSI_GOOD_FROM < msg->rssi_[i] )
                rssi = tr( "Good" );
            else if ( RSSI_FAIR_FROM < msg->rssi_[i] )
            {
                rssi = tr( "Fair" );
                rssiColor = YELLOW;
            }
            else
            {
                rssi = tr( "Weak" );
                rssiColor = RED;
            }

#ifdef QT_DEBUG
            rssi += " (" + QString::number( msg->rssi_[i] ) + ")";
#endif
        }

        // ---- //

        // locate existing item
        QTreeWidgetItem *item( NULL );

        foreach ( QTreeWidgetItem *tempItem, doomed )
        {
            const QString tempItemId( tempItem->data( COLUMN_ID, Qt::DisplayRole ).toString() );

            if ( id == tempItemId )
            {
                doomed.removeOne( (item = tempItem) );
                break;
            }
        }

        // ---- //

        // existing device
        if ( item )
        {
            item->setText( COLUMN_OPER_NAME, operatorName );
            item->setText( COLUMN_FIRMWARE_VERSION, firmware );
            item->setText( COLUMN_BATTERY_LEVEL, battery );
            item->setText( COLUMN_RSSI, rssi );
        }
        else
        {
            QStringList rows;

            // tag data
            rows.append( id );
            rows.append( operatorName );
            rows.append( firmware );
            rows.append( battery );
            rows.append( rssi );

            // create widget item
            item = new QTreeWidgetItem( rows );

            // insert item
            bool inserted( false );

            for ( int loc( 0 ); loc < table_->topLevelItemCount(); ++loc )
            {
                const QTreeWidgetItem *tempItem( table_->topLevelItem( loc ) );
                const QString tempItemId( tempItem->data( COLUMN_ID, Qt::DisplayRole ).toString() );

                if ( id < tempItemId )
                {
                    table_->insertTopLevelItem( loc, item );
                    inserted = true;
                    break;
                }
            }

            if ( !inserted )
                table_->addTopLevelItem( item );
        }

        // background color
        item->setBackgroundColor( COLUMN_BATTERY_LEVEL, batteryColor );
        item->setBackgroundColor( COLUMN_RSSI, rssiColor );
    }

    // remove old tags
    foreach ( QTreeWidgetItem *tempItem, doomed )
        delete tempItem;

    Q_EMIT updateRefreshPending( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::setTableHeaders()
{
    const int tableWidth( table_->width() - 32 ); // leave room for scroll bar

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Tag ID" ) );
    tableHeaders.push_back( tr( "Operator Name" ) );
    tableHeaders.push_back( tr( "Version" ) );
    tableHeaders.push_back( tr( "Battery" ) );
    tableHeaders.push_back( tr( "Signal" ) );

    // table
    table_->setColumnCount( tableHeaders.size() );
    table_->setHeaderLabels( tableHeaders );

    table_->setColumnWidth( COLUMN_ID, (20 * tableWidth) / 100 );
    table_->setColumnWidth( COLUMN_OPER_NAME, (46 * tableWidth) / 100 );
    table_->setColumnWidth( COLUMN_FIRMWARE_VERSION, (10 * tableWidth) / 100 );
    table_->setColumnWidth( COLUMN_BATTERY_LEVEL, (12 * tableWidth) / 100 );
    table_->setColumnWidth( COLUMN_RSSI, (12 * tableWidth) / 100 );
}
