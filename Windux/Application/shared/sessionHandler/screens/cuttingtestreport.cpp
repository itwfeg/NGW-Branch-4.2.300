/**
 * @file cuttingtestreport.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "cuttingtestreport.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
CuttingTestReport::CuttingTestReport( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "CuttingTestReport" );

    // check if in operate mode
    if ( (operateMode_ = dynamic_cast<OperateMode*>( parent )) )
    {
        // connect slots
        connect( operateMode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
    }

    // initialize
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CuttingTestReport::~CuttingTestReport()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::setReportData( const DomCuttingTestTotals& value )
{
    LOG_CLASS_INFO( "CuttingTestReport", "setReportData" );

    DomCuttingTestTotals *totals( const_cast<DomCuttingTestTotals*>( &value ) );

    // header
    testNumber_->setText( totals->testNum() );
    productNumber_->setText( totals->itemNum() );
    vendorNumber_->setText( totals->vendorNum() );
    operatorNumber_->setText( totals->operatorID() );
    startTime_->setText( totals->startDate() );
    endTime_->setText( totals->endDate() );

    // body
    if ( totals->numOfItems() )
    {
        QStringList itemsList( totals->field<QStringList>( totals->itemsList() ) );
        QStringList descList( totals->field<QStringList>( totals->descList() ) );
        QStringList weightList( totals->field<QStringList>( totals->weightList() ) );
        QStringList yieldList( totals->field<QStringList>( totals->yieldList() ) );
        QStringList unitPriceList( totals->field<QStringList>( totals->unitPriceList() ) );
        QStringList totalPriceList( totals->field<QStringList>( totals->totalPriceList() ) );

        if ( totals->numOfItems() != itemsList.size() )
            LOG_WARN( "mismatch in items list %d %d", totals->numOfItems(), itemsList.size() );

        if ( totals->num2() != descList.size() )
            LOG_WARN( "mismatch in desc list %d %d", totals->num2(), descList.size() );

        if ( totals->num3() != weightList.size() )
            LOG_WARN( "mismatch in weight list %d %d", totals->num3(), weightList.size() );

        if ( totals->num4() != yieldList.size() )
            LOG_WARN( "mismatch in yield list %d %d", totals->num4(), yieldList.size() );

        if ( totals->num5() != unitPriceList.size() )
            LOG_WARN( "mismatch in unit price list %d %d", totals->num5(), unitPriceList.size() );

        if ( totals->num6() != totalPriceList.size() )
            LOG_WARN( "mismatch in total price list %d %d", totals->num6(), totalPriceList.size() );

        int count( totals->numOfItems() );
        count = qMin( count, totals->num2() );
        count = qMin( count, totals->num3() );
        count = qMin( count, totals->num4() );
        count = qMin( count, totals->num5() );
        count = qMin( count, totals->num6() );

        // process items
        for ( int i( 0 ); i < count; ++i )
        {
            QTreeWidgetItem *item;
            QStringList rows;

            // item data
            rows.push_back( itemsList[i] );
            rows.push_back( descList[i] );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( weightList[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( yieldList[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( unitPriceList[i] ) );
            rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( totalPriceList[i] ) );

            // create widget item
            item = new QTreeWidgetItem( rows );
            item->setSizeHint( 0, QSize( 32, 32 ) );

            LOG_DEBUG( "adding item with key '%s' to table", qPrintable( itemsList[i] ) );
            items_->addTopLevelItem( item );
        }
    }

    // footer
    sellItemsWeight_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->sellWeight() ) );
    sellItemsYield_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->sellYield() ) );
    sellItemsPrice_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->sellPrice() ) );

    shrinkWeight_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->shrinkWeight() ) );
    shrinkYield_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->shrinkYield() ) );

    buyItemWeight_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->itemWeight() ) );
    buyItemYield_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->itemYield() ) );
    buyItemCost_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->itemCost() ) );
    buyItemAvgPrice_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->avgPrice() ) );

    imu_->setText( session_->uiUtilities->youGottaSetTheSeparator( totals->imu() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::translate()
{
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

    const QString unit( scaleConfig->isScaleMetric() ? "kg" : "lb" );
    const QString currSymbol( currencySettings->priCurrSymbol() );

    // header
    testNumberLabel_->setText( tr( "Cutting Test Number:" ) );
    productNumberLabel_->setText( tr( "Buy Item:" ) );
    vendorNumberLabel_->setText( tr( "Vendor:" ) );
    operatorNumberLabel_->setText( tr( "Operator:" ) );
    startTimeLabel_->setText( tr( "Start Date/Time:" ) );
    endTimeLabel_->setText( tr( "End Date/Time:" ) );

    // body
    QStringList tableHeaders;
    tableHeaders.push_back( QString( "" ) );
    tableHeaders.push_back( tr( "Sell Item" ) );
    tableHeaders.push_back( tr( "Weight" ) );
    tableHeaders.push_back( tr( "% Yield" ) );
    tableHeaders.push_back( QString( "%1 / %2" ).arg( currSymbol ).arg( unit ) );
    tableHeaders.push_back( QString( "%1 %2" ).arg( tr( "Total" ) ).arg( currSymbol ) );

    items_->setHeaderLabels( tableHeaders );

    items_->setColumnCount( tableHeaders.size() );
    calcItemColumnWidths();

    // footer
    totalSellItemsLabel_->setText( tr( "Total for Sell Items:" ) );
    totalShrinkLabel_->setText( tr( "Total Shrinkage:" ) );
    totalBuyItemLabel_->setText( tr( "Total for Buy Item:" ) );
    buyItemCostLabel_->setText( tr( "Cost" ) );
    imuLabel_->setText( tr( "IMU %" ) );

    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // hide any weight bar
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::initialize()
{
    // use default palette
    setAutoFillBackground( true );
    setPalette( QApplication::palette() );

    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );
    setFontPointSize14( this );

    //
    // header
    //

    // test number
    testNumberLabel_ = new QLabel( this );
    testNumberLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    testNumberLabel_->setObjectName( "testNumberLabel" );

    testNumber_ = new QLabel( this );
    testNumber_->setObjectName( "testNumber" );

    // product number
    productNumberLabel_ = new QLabel( this );
    productNumberLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    productNumberLabel_->setObjectName( "productNumberLabel" );

    productNumber_ = new QLabel( this );
    productNumber_->setObjectName( "productNumber" );

    // vendor number
    vendorNumberLabel_ = new QLabel( this );
    vendorNumberLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    vendorNumberLabel_->setObjectName( "vendorNumberLabel" );

    vendorNumber_ = new QLabel( this );
    vendorNumber_->setObjectName( "vendorNumber" );

    // operator number
    operatorNumberLabel_ = new QLabel( this );
    operatorNumberLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    operatorNumberLabel_->setObjectName( "operatorNumberLabel" );

    operatorNumber_ = new QLabel( this );
    operatorNumber_->setObjectName( "operatorNumber" );

    // start time
    startTimeLabel_ = new QLabel( this );
    startTimeLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    startTimeLabel_->setObjectName( "startTimeLabel" );

    startTime_ = new QLabel( this );
    startTime_->setObjectName( "startTime" );

    // end time
    endTimeLabel_ = new QLabel( this );
    endTimeLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    endTimeLabel_->setObjectName( "endTimeLabel" );

    endTime_ = new QLabel( this );
    endTime_->setObjectName( "endTime" );

    //
    // body
    //

    items_ = new QTreeWidget( this );
    items_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    items_->setObjectName( "items" );
    items_->setRootIsDecorated( false );
    items_->setSelectionMode( QAbstractItemView::NoSelection );
    items_->setUniformRowHeights( true );

    //
    // footer
    //

    spacer_ = new QLabel( this );
    spacer_->setObjectName( "doYouThinkQAReallyLooksAtTheNameOfTheseObjectsBecauseIAmNotSoSure" );

    totalSellItemsLabel_ = new QLabel( this );
    totalSellItemsLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    totalSellItemsLabel_->setObjectName( "totalSellItemsLabel" );

    totalShrinkLabel_ = new QLabel( this );
    totalShrinkLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    totalShrinkLabel_->setObjectName( "totalShrinkLabel" );

    totalBuyItemLabel_ = new QLabel( this );
    totalBuyItemLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    totalBuyItemLabel_->setObjectName( "totalBuyItemLabel" );

    sellItemsWeight_ = new QLabel( this );
    sellItemsWeight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    sellItemsWeight_->setObjectName( "sellItemsWeight" );

    sellItemsYield_ = new QLabel( this );
    sellItemsYield_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    sellItemsYield_->setObjectName( "sellItemsYield" );

    sellItemsPrice_ = new QLabel( this );
    sellItemsPrice_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    sellItemsPrice_->setObjectName( "sellItemsPrice" );

    shrinkWeight_ = new QLabel( this );
    shrinkWeight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    shrinkWeight_->setObjectName( "shrinkWeight" );

    shrinkYield_ = new QLabel( this );
    shrinkYield_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    shrinkYield_->setObjectName( "shrinkYield" );

    buyItemWeight_ = new QLabel( this );
    buyItemWeight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    buyItemWeight_->setObjectName( "buyItemWeight" );

    buyItemYield_ = new QLabel( this );
    buyItemYield_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    buyItemYield_->setObjectName( "buyItemYield" );

    buyItemAvgPrice_ = new QLabel( this );
    buyItemAvgPrice_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    buyItemAvgPrice_->setObjectName( "buyItemAvgPrice" );

    buyItemCostLabel_ = new QLabel( this );
    buyItemCostLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    buyItemCostLabel_->setObjectName( "buyItemCostLabel" );

    buyItemCost_ = new QLabel( this );
    buyItemCost_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    buyItemCost_->setObjectName( "buyItemCost" );

    imuLabel_ = new QLabel( this );
    imuLabel_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    imuLabel_->setObjectName( "imuLabel" );

    imu_ = new QLabel( this );
    imu_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    imu_->setObjectName( "imu" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::createLayout()
{
    static const QMargins margins( 50, 16, 50, 8 );

    // delete previous layout
    if ( layout() )
        delete layout();

    QFormLayout *leftHeader( new QFormLayout );
    leftHeader->addRow( testNumberLabel_, testNumber_ );
    leftHeader->addRow( productNumberLabel_, productNumber_ );
    leftHeader->addRow( vendorNumberLabel_, vendorNumber_ );

    QFormLayout *rightHeader( new QFormLayout );
    rightHeader->addRow( operatorNumberLabel_, operatorNumber_ );
    rightHeader->addRow( startTimeLabel_, startTime_ );
    rightHeader->addRow( endTimeLabel_, endTime_ );

    QHBoxLayout *header( new QHBoxLayout );
    header->addStretch();
    header->addLayout( leftHeader );
    header->addStretch();
    header->addLayout( rightHeader );
    header->addStretch();

    QVBoxLayout *labelFooter( new QVBoxLayout );
    labelFooter->addWidget( spacer_ );
    labelFooter->addWidget( totalSellItemsLabel_ );
    labelFooter->addWidget( totalShrinkLabel_ );
    labelFooter->addWidget( totalBuyItemLabel_ );

    QVBoxLayout *weightFooter( new QVBoxLayout );
    weightFooter->addStretch();
    weightFooter->addWidget( sellItemsWeight_ );
    weightFooter->addWidget( shrinkWeight_ );
    weightFooter->addWidget( buyItemWeight_ );

    QVBoxLayout *yieldFooter( new QVBoxLayout );
    yieldFooter->addStretch();
    yieldFooter->addWidget( sellItemsYield_ );
    yieldFooter->addWidget( shrinkYield_ );
    yieldFooter->addWidget( buyItemYield_ );

    QVBoxLayout *costFooter( new QVBoxLayout );
    costFooter->addWidget( buyItemCostLabel_ );
    costFooter->addStretch();
    costFooter->addWidget( buyItemAvgPrice_ );

    QVBoxLayout *priceFooter( new QVBoxLayout );
    priceFooter->addStretch();
    priceFooter->addWidget( sellItemsPrice_ );
    priceFooter->addStretch();
    priceFooter->addWidget( buyItemCost_ );

    QVBoxLayout *imuFooter( new QVBoxLayout );
    imuFooter->addWidget( imuLabel_ );
    imuFooter->addWidget( imu_ );
    imuFooter->addStretch();

    QHBoxLayout *footer( new QHBoxLayout );
    footer->addStretch();
    footer->addLayout( imuFooter );
    footer->addStretch();
    footer->addLayout( labelFooter );
    footer->addLayout( weightFooter );
    footer->addLayout( yieldFooter );
    footer->addLayout( costFooter );
    footer->addLayout( priceFooter );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( margins );

    screenLayout->addLayout( header );
    screenLayout->addWidget( items_, 1 );
    screenLayout->addLayout( footer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestReport::calcItemColumnWidths()
{
    int tableWidth( items_->contentsRect().width() );

    items_->setColumnWidth( 0, (tableWidth * 10) / 100 ); // item number
    items_->setColumnWidth( 1, (tableWidth * 44) / 100 ); // item description
    items_->setColumnWidth( 2, (tableWidth * 12) / 100 ); // weight
    items_->setColumnWidth( 3, (tableWidth * 10) / 100 ); // yield
    items_->setColumnWidth( 4, (tableWidth * 12) / 100 ); // unit price
    items_->setColumnWidth( 5, (tableWidth * 12) / 100 ); // total

    // weight
    sellItemsWeight_->setFixedWidth( (tableWidth * 12) / 100 );
    shrinkWeight_->setFixedWidth( (tableWidth * 12) / 100 );
    buyItemWeight_->setFixedWidth( (tableWidth * 12) / 100 );

    // yield
    sellItemsYield_->setFixedWidth( (tableWidth * 10) / 100 );
    shrinkYield_->setFixedWidth( (tableWidth * 10) / 100 );
    buyItemYield_->setFixedWidth( (tableWidth * 10) / 100 );

    // cost
    buyItemCostLabel_->setFixedWidth( (tableWidth * 12) / 100 );
    buyItemCost_->setFixedWidth( (tableWidth * 12) / 100 );

    // price
    sellItemsPrice_->setFixedWidth( (tableWidth * 12) / 100 );
    buyItemAvgPrice_->setFixedWidth( (tableWidth * 12) / 100 );
}
