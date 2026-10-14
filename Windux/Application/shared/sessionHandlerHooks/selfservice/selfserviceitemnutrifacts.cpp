/**
 * @file selfserviceitemnutrifacts.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfserviceitemnutrifacts.h"

#include <filesystem.h>
#include <pixmap.h>
#include <selfservicehook.h>

#include <QFile>
#include <QLabel>
#include <QVBoxLayout>
#include <ScaleConfiguration.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceItemNutrifacts::selfServiceItemNutrifacts( QWidget *parent ) :
    _Mybase( parent ),
    scaleConfig_( NULL ),
    hook_( NULL ),
    fs_( false )
{
    setObjectName( "selfServiceItemNutrifacts" );

    // Initialize
    initialize();

    // Create layouts
    createLayout();

    // Translate
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceItemNutrifacts::~selfServiceItemNutrifacts()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::refresh()
{
    // sanity check
    if ( !hook_ )
        return;

    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    bool itemHasFreqShopperPricing;
    bool itemHasNutrifacts;

    if( scaleConfig_->scaleConfig.enableHelpPrompts() )
    {
        itemHasFreqShopperPricing = false;
        itemHasNutrifacts = false;
    }
    else
    {
        if( "1" == hook_->fieldValue( sessionHooks::itemHasFreqShopperPricing ) )
            itemHasFreqShopperPricing = true;
        else
            itemHasFreqShopperPricing = false;

    if ( !displayFreqShopper() )
        itemHasFreqShopperPricing = false;

        if( QFile::exists( hook_->fieldValue( sessionHooks::itemNutrifactsPanel ) ) )
            itemHasNutrifacts = true;
        else
            itemHasNutrifacts = false;
    }

    // when no nutrifacts and no frequent shopper info we should hide
    if (( itemHasNutrifacts ) || ( itemHasFreqShopperPricing ))
        show();
    else
    {
        hide();
        return;
    }

    // update images
    updateImages();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::translate()
{
    bool metricScale( false );

    if (( hook_ ) && ( "1" == hook_->fieldValue( sessionHooks::metricScale ) ))
        metricScale = true;

    fsItemSavingsLabel_->setText( tr( "Savings" ) );
    fsItemUnitPriceLabel_->setText( tr( "Discount Unit Price" ) + (metricScale ? " /kg" : " /lb") );
    fsItemTotalPriceLabel_->setText( tr( "Discount Total Price" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::resizeEvent( QResizeEvent *e )
{
    updateImages();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::initialize()
{
    // create widgets
    fsItemUnitPriceLabel_ = new QLabel( this );
    fsItemUnitPrice_ = new QLabel( this );

    fsItemSavingsLabel_ = new QLabel( this );
    fsItemSavings_ = new QLabel( this );

    fsItemTotalPriceLabel_ = new QLabel( this );
    fsItemTotalPrice_ = new QLabel( this );

    // set widget properties
    QList<QLabel*> labels;
    labels.append( fsItemUnitPriceLabel_ );
    labels.append( fsItemUnitPrice_ );
    labels.append( fsItemSavingsLabel_ );
    labels.append( fsItemSavings_ );
    labels.append( fsItemTotalPriceLabel_ );
    labels.append( fsItemTotalPrice_ );

    QList<QLabel*> smallFontLabels;
    smallFontLabels.append( fsItemUnitPriceLabel_ );
    smallFontLabels.append( fsItemSavingsLabel_ );
    smallFontLabels.append( fsItemTotalPriceLabel_ );

    foreach ( QLabel *l, labels )
    {
        QFont f( l->font() );

        if ( smallFontLabels.contains( l ) )
            f.setPointSize( 12 );
        else
            f.setPointSize( 45 );

        f.setBold( true );

        l->setAlignment( Qt::AlignCenter );
        l->setFont( f );
    }

    // prepare backgrounds
    setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::createLayout()
{
    // delete existing layout
    if ( this->layout() )
        delete this->layout();

    // generate new layout
    QVBoxLayout *layout( new QVBoxLayout( this ) );

    layout->addStretch();
    layout->addWidget( fsItemSavingsLabel_ );
    layout->addWidget( fsItemSavings_ );
    layout->addStretch();
    layout->addWidget( fsItemUnitPriceLabel_ );
    layout->addWidget( fsItemUnitPrice_ );
    layout->addStretch();
    layout->addWidget( fsItemTotalPriceLabel_ );
    layout->addWidget( fsItemTotalPrice_ );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemNutrifacts::updateImages()
{
    // sanity check
    if ( !hook_ )
        return;

    //
    // nutrifacts panel
    //

    QPalette p( palette() );

    bool itemHasFreqShopperPricing( "1" == hook_->fieldValue( sessionHooks::itemHasFreqShopperPricing ) );

    if ( !displayFreqShopper() )
        itemHasFreqShopperPricing = false;

    QString itemDiscountedSavings( hook_->fieldValue( sessionHooks::itemDiscountedSavings ) );
    QString itemDiscountedTotalPrice( hook_->fieldValue( sessionHooks::itemDiscountedTotalPrice ) );
    QString itemDiscountedUnitPrice( hook_->fieldValue( sessionHooks::itemDiscountedUnitPrice ) );

    // only has fs pricing when one or more fields populated
    if ( itemHasFreqShopperPricing )
    {
        itemDiscountedSavings = itemDiscountedSavings.trimmed();
        itemDiscountedTotalPrice = itemDiscountedTotalPrice.trimmed();
        itemDiscountedUnitPrice = itemDiscountedUnitPrice.trimmed();

        if (( itemDiscountedSavings.isEmpty() ) &&
            ( itemDiscountedTotalPrice.isEmpty() ) &&
            ( itemDiscountedUnitPrice.isEmpty() ))
        {
            itemHasFreqShopperPricing = false;
        }
    }

    QString itemNutrifacts( hook_->fieldValue( sessionHooks::itemNutrifactsPanel ) );
    bool itemNutrifactsValid( false );

    // check for nutrifacts image
    // when fs pricing present we do NOT show the image
    if (( !itemNutrifacts.isEmpty() ) && ( !itemHasFreqShopperPricing ))
    {
        QPixmap image( itemNutrifacts );

        if ( !image.isNull() )
        {
            sizeToFitPixmap stretcher( size() );
            QPixmap resizedImage( stretcher( image ) );

            if ( !resizedImage.isNull() )
            {
                p.setBrush( backgroundRole(), resizedImage );
                itemNutrifactsValid = true;
            }
        }
    }

    // no nutrifacts
    if ( !itemNutrifactsValid )
        p.setColor( backgroundRole(), Qt::transparent );

    // set new palette
    setPalette( p );

    //
    // frequent shopper pricing
    //

    // set widget visibility
    fsItemUnitPriceLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemUnitPrice_->setVisible( itemHasFreqShopperPricing );
    fsItemSavingsLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemSavings_->setVisible( itemHasFreqShopperPricing );
    fsItemTotalPriceLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemTotalPrice_->setVisible( itemHasFreqShopperPricing );

    // update frequent shopper amounts
    if ( itemHasFreqShopperPricing )
    {
        fsItemSavings_->setText( itemDiscountedSavings );
        fsItemTotalPrice_->setText( itemDiscountedTotalPrice );
        fsItemUnitPrice_->setText( itemDiscountedUnitPrice );

        if ( itemDiscountedSavings.isEmpty() )
        {
            fsItemSavingsLabel_->setVisible( false );
            fsItemSavings_->setVisible( false );
        }

        if ( itemDiscountedTotalPrice.isEmpty() )
        {
            fsItemTotalPriceLabel_->setVisible( false );
            fsItemTotalPrice_->setVisible( false );
        }

        if ( itemDiscountedUnitPrice.isEmpty() )
        {
            fsItemUnitPriceLabel_->setVisible( false );
            fsItemUnitPrice_->setVisible( false );
        }

        // generate pixmap for freq shopper area
        QPixmap pixmap( size() );
        pixmap.fill( Qt::transparent );

        QPainter painter( &pixmap );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.setBrush( QColor( 255, 255, 255, 127 ) );
        painter.setPen( QPen( Qt::black, 2 ) );

        // create button
        painter.drawRoundedRect( rect().adjusted( 1, 1, -1, -1 ), 30, 30 );

        // set palette
        p.setBrush( backgroundRole(), pixmap );
        setPalette( p );
    }
}
