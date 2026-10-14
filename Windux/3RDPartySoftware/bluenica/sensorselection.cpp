/**
 * @file sensorselection.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "dbtables.h"
#include "sensorselection.h"

#include <QDateTime>
#include <QTreeWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorSelectionWidget::sensorSelectionWidget( QWidget *parent ) :
    _Mybase( parent ),
    useCelsius_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorSelectionWidget::~sensorSelectionWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<unsigned int> sensorSelectionWidget::selectedItems() const
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
void sensorSelectionWidget::clearSelection()
{
    table_->clearSelection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::refresh( unsigned int age )
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
    QList<sensorInfo> sensors( db_.querySensors( age ) );

    foreach ( const sensorInfo &sensor, sensors )
    {
        QTreeWidgetItem *item;
        QStringList rows;

        // compute sensor age
        QDateTime stamp;
        stamp.setMSecsSinceEpoch( (qint64) sensor.lastComm * 1000 );

        qint64 diff( now - stamp.toMSecsSinceEpoch() );
        diff /= 1000;

        // sensor type
        QString sensorType( tr( "Unknown" ) );

        if ( SENSOR_TYPE_TEMPERATURE & sensor.type )
            sensorType = tr( "Temperature Sensor" );

        // battery level
        QString battery;

        if ( (2*batteryNormalFrom) <= sensor.batteryLevel )
            battery = tr( "Excellent" );
        else if ( batteryNormalFrom <= sensor.batteryLevel )
            battery = tr( "Good" );
        else if ( batteryWarningFrom <= sensor.batteryLevel )
            battery = tr( "Okay" );
        else
            battery = tr( "Low" );

        // transaction data
        rows.push_back( QString::number( sensor.id ) );
        rows.push_back( sensor.address );
        rows.push_back( sensorType );
        rows.push_back( sensorDataAge( diff ) );
        rows.push_back( formatDisplayTemp( sensor.temperature ) );
        rows.push_back( battery );

        // create widget item
        item = new QTreeWidgetItem( rows );

        table_->addTopLevelItem( item );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::translate()
{
    setTableHeaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::resizeEvent( QResizeEvent *e )
{
    setTableHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::itemDoubleClicked( QTreeWidgetItem *item, int column )
{
    Q_UNUSED( column );

    if ( !item )
        return;

    Q_EMIT itemDoubleClicked( item->data( 0, Qt::DisplayRole ).toUInt() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::initialize()
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
void sensorSelectionWidget::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addWidget( table_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorSelectionWidget::setTableHeaders()
{
    const int tableWidth( table_->width() );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "" ) );
    tableHeaders.push_back( tr( "Sensor ID" ) );
    tableHeaders.push_back( tr( "Type" ) );
    tableHeaders.push_back( tr( "Data Age" ) );
    tableHeaders.push_back( tr( "Temp" ) );
    tableHeaders.push_back( tr( "Battery" ) );

    // table
    table_->setColumnCount( tableHeaders.size() );
    table_->setHeaderLabels( tableHeaders );

    table_->setColumnHidden( 0, true );
    table_->setColumnWidth( 1, (20 * tableWidth) / 100 );
    table_->setColumnWidth( 2, (44 * tableWidth) / 100 );
    table_->setColumnWidth( 3, (12 * tableWidth) / 100 );
    table_->setColumnWidth( 4, (12 * tableWidth) / 100 );
    table_->setColumnWidth( 5, (12 * tableWidth) / 100 ); // add some extra in case of scroll bar
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal sensorSelectionWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sensorSelectionWidget::formatDisplayTemp( qreal temp ) const
{
    static const QChar degree( 0260 ); // degree character in octal

    return QString( "%0 %1%2").arg( convertTemp( temp ), 0, 'f', 1 ).arg( degree ).arg( useCelsius_ ? "C" : "F" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sensorSelectionWidget::sensorDataAge( qint64 diff )
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
