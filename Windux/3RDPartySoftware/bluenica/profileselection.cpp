/**
 * @file profileselection.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dbtables.h"
#include "profileselection.h"

#include <QTreeWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
tempProfileSelectionWidget::tempProfileSelectionWidget( QWidget *parent ) :
    _Mybase( parent ),
    useCelsius_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tempProfileSelectionWidget::~tempProfileSelectionWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<unsigned int> tempProfileSelectionWidget::selectedItems() const
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
void tempProfileSelectionWidget::clearSelection()
{
    table_->clearSelection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::refresh()
{
    const unsigned int tempUnitOfMeasure( db_.queryConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE ).toUInt() );

    // set temp uom
    useCelsius_ = (TEMP_UOM_CELSIUS == tempUnitOfMeasure);

    // clear all items
    table_->clear();

    // query new set
    QList<profileInfo> profiles( db_.queryProfiles() );

    foreach ( const profileInfo &pi, profiles )
    {
        QTreeWidgetItem *item;
        QStringList rows;

        // transaction data
        rows.push_back( QString::number( pi.id ) );
        rows.push_back( pi.description );
        rows.push_back( formatDisplayTemp( pi.tempWarningFrom ) );
        rows.push_back( formatDisplayTemp( pi.tempNormalFrom ) );
        rows.push_back( formatDisplayTemp( pi.tempNormalTo ) );
        rows.push_back( formatDisplayTemp( pi.tempWarningTo ) );
        rows.push_back( formatDisplayTemp( pi.tempStandard ) );

        // create widget item
        item = new QTreeWidgetItem( rows );

        table_->addTopLevelItem( item );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::translate()
{
    setTableHeaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::resizeEvent( QResizeEvent *e )
{
    setTableHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::itemDoubleClicked( QTreeWidgetItem *item, int column )
{
    Q_UNUSED( column );

    if ( !item )
        return;

    Q_EMIT itemDoubleClicked( item->data( 0, Qt::DisplayRole ).toUInt() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::initialize()
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
void tempProfileSelectionWidget::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addWidget( table_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tempProfileSelectionWidget::setTableHeaders()
{
    const int tableWidth( table_->width() );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "" ) );
    tableHeaders.push_back( tr( "Description" ) );
    tableHeaders.push_back( tr( "Warning From" ) );
    tableHeaders.push_back( tr( "Normal From" ) );
    tableHeaders.push_back( tr( "Normal To" ) );
    tableHeaders.push_back( tr( "Warning To" ) );
    tableHeaders.push_back( tr( "Standard" ) );

    // table
    table_->setColumnCount( tableHeaders.size() );
    table_->setHeaderLabels( tableHeaders );

    table_->setColumnHidden( 0, true );
    table_->setColumnWidth( 1, (33 * tableWidth) / 100 );
    table_->setColumnWidth( 2, (13 * tableWidth) / 100 );
    table_->setColumnWidth( 3, (13 * tableWidth) / 100 );
    table_->setColumnWidth( 4, (13 * tableWidth) / 100 );
    table_->setColumnWidth( 5, (13 * tableWidth) / 100 );
    table_->setColumnWidth( 6, (15 * tableWidth) / 100 ); // add some extra in case of scroll bar
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal tempProfileSelectionWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString tempProfileSelectionWidget::formatDisplayTemp( qreal temp ) const
{
    static const QChar degree( 0260 ); // degree character in octal

    return QString( "%0 %1%2").arg( convertTemp( temp ), 0, 'f', 1 ).arg( degree ).arg( useCelsius_ ? "C" : "F" );
}
