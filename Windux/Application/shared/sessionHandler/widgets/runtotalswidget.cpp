/**
 * @file runtotalswidget.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "runtotalswidget.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <logmanager.h>
#include <sessioninfo.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
runTotalsWidget::runTotalsWidget( QWidget *parent ) :
    _Mybase( parent ),
    productRunTotals_( new DomProductRunTotal ),
    productType_( 0 )
{
    LOG_CLASS_INFO( "runTotalsWidget", "runTotalsWidget" );

    setAutoFillBackground( false );
    setObjectName( "runTotalsWidget" );

    // hook into session
    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();

    if ( !mainWindow )
        LOG_WARN( "could not retrieve main window!", NULL );
    else if ( !(session_ = mainWindow->session()) )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        // hook into session
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceive(SessionMessage) ) );
    }

    operateMode_ = mainWindow->retrieveMode<OperateMode>();
    config_ = session_->scaleConfig;

    if ( operateMode_ )//Try to set up the signal/slot
    {
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );
    }

    // initialize
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
runTotalsWidget::~runTotalsWidget()
{
    if ( productRunTotals_ )
    {
        delete productRunTotals_;
        productRunTotals_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::translate()
{
    header_->setText( tr( "RUN TOTALS" ) );
    packagesLabel_->setText( tr( "Packages" ) );
    itemsLabel_->setText( tr( "Items" ) );
    totalLabel_->setText( tr( "Total" ) );

    updateTotals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // create renderer
    const backgroundWidgetRenderer renderer(
        new panelButtonBackgroundWidget( 0 ),
        new darkOperatorBackgroundWidgetRendererAppearance() );

    // set palette
    QPalette p( palette() );
    p.setBrush( backgroundRole(), renderer.drawControl( size(), ReadOnly ) );
    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // currency configuration
    if (( ConfigurationDev == device ) && ( CfgCurrencyWrite == event ))
        updateTotals();

    // scale configuration
    else if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
        updateTotals();

    // run totals
    else if (( TotalsDev == device ) && ( productRunTotal02 == event ))
    {
        *productRunTotals_ = DomProductRunTotal( msg );

        updateTotals();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::onCurrentProductChanged( bool nof )
{
    DomCurrentPLU *product;
    productType_ = 0;

    if (( !nof ) && ( product = operateMode_->currentProduct() ))
        productType_ = product->productType();

    updateTotals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::initialize()
{
    header_ = new QLabel( this );
    header_->setObjectName( "header" );

    packagesLabel_ = new QLabel( this );
    packagesLabel_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    packagesLabel_->setObjectName( "packagesLabel" );

    packages_ = new QLabel( this );
    packages_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    packages_->setObjectName( "packages" );

    itemsLabel_ = new QLabel( this );
    itemsLabel_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    itemsLabel_->setObjectName( "itemsLabel" );

    items_ = new QLabel( this );
    items_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    items_->setObjectName( "items" );

    totalLabel_ = new QLabel( this );
    totalLabel_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    totalLabel_->setObjectName( "totalLabel" );

    total_ = new QLabel( this );
    total_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    total_->setObjectName( "total" );

    weightLabel_ = new QLabel( this );
    weightLabel_->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    weightLabel_->setObjectName( "weightLabel" );

    weight_ = new QLabel( this );
    weight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    weight_->setObjectName( "weight" );

    // ---- //

    // make list of labels on widget
    labels_.append( packagesLabel_ );
    labels_.append( packages_ );
    labels_.append( itemsLabel_ );
    labels_.append( items_ );
    labels_.append( totalLabel_ );
    labels_.append( total_ );
    labels_.append( weightLabel_ );
    labels_.append( weight_ );

    modifyFontPointSize setFontPointSize18( (window()->height() * 18) / 600 );

    foreach ( QLabel *l, labels_ )
        setFontPointSize18( l );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::createLayout()
{
    QHBoxLayout *headerLayout( new QHBoxLayout );
    headerLayout->addWidget( header_ );
    headerLayout->addStretch();

    QVBoxLayout *left( new QVBoxLayout );
    left->addWidget( packages_ );
    left->addWidget( items_ );
    left->addWidget( total_ );
    left->addWidget( weight_ );

    QVBoxLayout *right( new QVBoxLayout );
    right->addWidget( packagesLabel_ );
    right->addWidget( itemsLabel_ );
    right->addWidget( totalLabel_ );
    right->addWidget( weightLabel_ );

    QHBoxLayout *formsLayout( new QHBoxLayout );
    formsLayout->setSpacing( 16 );

    formsLayout->addLayout( left );
    formsLayout->addLayout( right );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addLayout( headerLayout );
    screenLayout->addLayout( formsLayout, 1 );
    screenLayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void runTotalsWidget::updateTotals()
{
    //Check if we can connect the slot now
    if( !operateMode_ )
    {
        sessionInfo info( this );
        SessionMainWindow *mainWindow = info();
        operateMode_ = mainWindow->retrieveMode<OperateMode>();
        if( operateMode_ )
            connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );
    }

    // no product
    if ( 0 == productType_ )
    {
        foreach ( QLabel *l, labels_ )
            l->hide();

        return;
    }

    MadpScaleConfig *scaleConfig( &config_->scaleConfig );
    MadpCurrencySettings *currencySettings( &config_->currencySettings );

    QString scaleUnits( scaleConfig->isScaleMetric() ? "kg" : "lb" );
    QString fixedWeightUnits( scaleConfig->isScaleMetric() ? "g" : "oz" );
    QString volumeUnits( "fl oz" );

    int currencyPrecision( currencySettings->primaryPrecision() );
    QString currencySeparator( currencySettings->unitSeparator() );
    QString currencyUnit( currencySettings->priCurrSymbol() );

    // ---- //

    QString totalPackages( productRunTotals_->totalPackages() );
    QString totalItems( productRunTotals_->totalItems() );
    QString totalPrice( productRunTotals_->totalPrice() );
    QString totalWeight( productRunTotals_->totalWeight() );

    if ( totalPackages.isEmpty() )
        totalPackages = "0";

    if ( totalItems.isEmpty() )
        totalItems = "0";

    if ( totalPrice.isEmpty() )
    {
        totalPrice = "0";

        if ( currencyPrecision )
        {
            totalPrice.append( "." );

            while ( currencyPrecision-- )
                totalPrice.append( "0" );
        }
    }

    if ( totalWeight.isEmpty() )
        totalWeight = "0.000";

    totalPrice.replace( ".", currencySeparator );
    totalWeight.replace( ".", currencySeparator );

    // ---- //

    // packages
    packages_->setText( totalPackages );

    packagesLabel_->show();
    packages_->show();

    // items
    items_->setText( totalItems );

    itemsLabel_->setVisible( 1 != productType_ );
    items_->setVisible( 1 != productType_ );

    // total
    total_->setText( currencyUnit + totalPrice );

    totalLabel_->show();
    total_->show();

    // weight
    weightLabel_->setText( scaleUnits );
    weight_->setText( totalWeight );

    weightLabel_->setVisible( 3 != productType_ );
    weight_->setVisible( 3 != productType_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
