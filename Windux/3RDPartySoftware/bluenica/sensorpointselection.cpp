/**
 * @file sensorpointselection.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "dbtables.h"
#include "sensorpointselection.h"

#include <QDateTime>
#include <QTreeWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointSelectionWidget::sensorPointSelectionWidget( QWidget *parent ) :
    _Mybase( parent ),
    useCelsius_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointSelectionWidget::~sensorPointSelectionWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<unsigned int> sensorPointSelectionWidget::selectedItems() const
{
    QList<unsigned int> results;

    // grab selected items
    QList<QTreeWidgetItem*> items( table_->selectedItems() );

    // return list of sensor id to caller
    foreach ( const QTreeWidgetItem *item, items )
        results.append( item->data( 0, Qt::DisplayRole ).toUInt() );

    return results;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::clearSelection()
{
    table_->clearSelection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::refresh()
{
    const unsigned int batteryWarningFrom( db_.queryConfigValue( CONFIG_BATTERY_WARNING_FROM ).toUInt() );
    const unsigned int batteryNormalFrom( db_.queryConfigValue( CONFIG_BATTERY_NORMAL_FROM ).toUInt() );
    const unsigned int tempUnitOfMeasure( db_.queryConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE ).toUInt() );

    const qint64 now( QDateTime::currentMSecsSinceEpoch() );

    // set temp uom
    useCelsius_ = (TEMP_UOM_CELSIUS == tempUnitOfMeasure);

    // clear all items
    table_->clear();

    // query new set
    QList<sensorPointInfo> sensorPoints( db_.querySensorPoints() );

    foreach ( const sensorPointInfo &sensorPoint, sensorPoints )
    {
        QTreeWidgetItem *item;
        QStringList rows;

        // retrieve sensor point state
        sensorPointStateInfo state( db_.querySensorPointState( sensorPoint.id ) );

        // retrieve temp profile
        profileInfo tempProfile( db_.queryProfile( sensorPoint.profile ) );

        // sensor point info
        QString lastComm;
        QString temp;
        QString battery;

        if ( sensorPoint.sensorAddress.isEmpty() )
            ; // no sensor bound to this point
        else if ( state.stamp )
        {
            // compute sensor age
            QDateTime stamp;
            stamp.setMSecsSinceEpoch( (qint64) state.stamp * 1000 );

            qint64 diff( now - stamp.toMSecsSinceEpoch() );
            diff /= 1000;

            // set comm
            lastComm = sensorDataAge( diff );

            // when comm error, do not fill in other fields
            if ( !(STATE_COMM_ERROR & state.state) )
            {
                // set temp and temp background color
                temp = formatDisplayTemp( state.temp );

                // set battery
                if ( (2*batteryNormalFrom) <= state.batteryLevel )
                    battery = tr( "Excellent" );
                else if ( batteryNormalFrom <= state.batteryLevel )
                    battery = tr( "Good" );
                else if ( batteryWarningFrom <= state.batteryLevel )
                    battery = tr( "Low" );
                else
                    battery = tr( "Very Low" );
            }
        }

        // transaction data
        rows.push_back( QString::number( sensorPoint.id ) );
        rows.push_back( sensorPoint.description );
        rows.push_back( tempProfile.description );
        rows.push_back( lastComm );
        rows.push_back( temp );
        rows.push_back( battery );

        // create widget item
        item = new QTreeWidgetItem( rows );

        // set background colors
        if ( STATE_COMM_ERROR & state.state )
            item->setBackgroundColor( 3, RED );
        else if ( STATE_COMM_WARNING & state.state )
            item->setBackgroundColor( 3, YELLOW );
        else
            item->setBackgroundColor( 3, GREEN );

        if ( STATE_TEMP_ERROR & state.state )
            item->setBackgroundColor( 4, RED );
        else if ( STATE_TEMP_WARNING & state.state )
            item->setBackgroundColor( 4, YELLOW );
        else
            item->setBackgroundColor( 4, GREEN );

        if ( STATE_BATT_ERROR & state.state )
            item->setBackgroundColor( 5, RED );
        else if ( STATE_BATT_WARNING & state.state )
            item->setBackgroundColor( 5, YELLOW );
        else
            item->setBackgroundColor( 5, GREEN );

        table_->addTopLevelItem( item );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::translate()
{
    setTableHeaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::resizeEvent( QResizeEvent *e )
{
    setTableHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::itemDoubleClicked( QTreeWidgetItem *item, int column )
{
    Q_UNUSED( column );

    if ( !item )
        return;

    Q_EMIT itemDoubleClicked( item->data( 0, Qt::DisplayRole ).toUInt() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::initialize()
{
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::SingleSelection );
    table_->setUniformRowHeights( true );

    connect( table_, SIGNAL(itemSelectionChanged()), SIGNAL(itemSelectionChanged()) );
    connect( table_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(itemDoubleClicked(QTreeWidgetItem*,int)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addWidget( table_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointSelectionWidget::setTableHeaders()
{
    const int tableWidth( table_->width() );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "" ) );
    tableHeaders.push_back( tr( "Description" ) );
    tableHeaders.push_back( tr( "Profile" ) );
    tableHeaders.push_back( tr( "Data Age" ) );
    tableHeaders.push_back( tr( "Temp" ) );
    tableHeaders.push_back( tr( "Battery" ) );

    // table
    table_->setColumnCount( tableHeaders.size() );
    table_->setHeaderLabels( tableHeaders );

    table_->setColumnHidden( 0, true );
    table_->setColumnWidth( 1, (40 * tableWidth) / 100 );
    table_->setColumnWidth( 2, (24 * tableWidth) / 100 );
    table_->setColumnWidth( 3, (12 * tableWidth) / 100 );
    table_->setColumnWidth( 4, (12 * tableWidth) / 100 );
    table_->setColumnWidth( 5, (12 * tableWidth) / 100 ); // add some extra in case of scroll bar
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal sensorPointSelectionWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sensorPointSelectionWidget::formatDisplayTemp( qreal temp ) const
{
    static const QChar degree( 0260 ); // degree character in octal

    return QString( "%0 %1%2").arg( convertTemp( temp ), 0, 'f', 1 ).arg( degree ).arg( useCelsius_ ? "C" : "F" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sensorPointSelectionWidget::sensorDataAge( qint64 diff )
{
    QString dataAge;

    int days( diff / (24 * 60 * 60) );
    diff %= (24 * 60 * 60);

    int hours( diff / (60 * 60) );
    diff %= (60 * 60);

    int mins( diff / 60 );
    diff %= 60;

    int secs( diff );

    if ( days )
        dataAge = QString( "%0d %1h" ).arg( days ).arg( hours );
    else if ( hours )
        dataAge = QString( "%0h %1m" ).arg( hours ).arg( mins );
    else if ( mins )
        dataAge = QString( "%0m %1s" ).arg( mins ).arg( secs );
    else
        dataAge = QString( "%0s" ).arg( secs );

    return dataAge;
}
