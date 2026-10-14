/**
 * @file selfserviceitemimage.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfserviceitemimage.h"

#include <filesystem.h>
#include <pixmap.h>
#include <selfservicehook.h>

#include <QLabel>
#include <QVBoxLayout>
#include <ScaleConfiguration.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceItemImage::selfServiceItemImage( QWidget *parent ) :
    _Mybase( parent ),
    hook_( NULL ),
    scaleConfig_( NULL ),
    fs_( false )
{
    setObjectName( "selfServiceItemImage" );

    // This is s widget loaded into the ByCount and Random Weight Screens.  It is used to display the Expanded Text Panel,
    // which in turn can display the Expanded Text, Product Images, or Nutrifacts information.

    // Initialize
    initialize();

    // Create layouts
    createLayout();

    // Translate
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceItemImage::~selfServiceItemImage()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::refresh()
{
    // update images
    updateImages();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::translate()
{
    bool metricScale( false );

    if (( hook_ ) && ( "1" == hook_->fieldValue( sessionHooks::metricScale ) ))
        metricScale = true;

    fsItemSavingsLabel_->setText( tr( "Savings" ) );
    fsItemUnitPriceLabel_->setText( tr( "Price" ) + (metricScale ? " /kg" : " /lb") );
    fsItemTotalPriceLabel_->setText( tr( "Total" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::resizeEvent( QResizeEvent *e )
{
    updateImages();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::initialize()
{
    // create widgets
    itemExpandedText_ = new QLabel( this );
    itemExpandedText_->setWordWrap( true );

    fsItemUnitPriceLabel_ = new QLabel( this );
    fsItemUnitPrice_ = new QLabel( this );

    fsItemSavingsLabel_ = new QLabel( this );
    fsItemSavings_ = new QLabel( this );

    fsItemTotalPriceLabel_ = new QLabel( this );
    fsItemTotalPrice_ = new QLabel( this );

    // set widget properties
    QList<QLabel*> labels;
    labels.append( itemExpandedText_ );
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

        if ( itemExpandedText_ == l )
            f.setPointSize( 18 );
        else if ( smallFontLabels.contains( l ) )
            f.setPointSize( 9 );
        else
            f.setPointSize( 24 );

        l->setAlignment( Qt::AlignCenter );
        l->setFont( f );
    }

    // prepare backgrounds
    setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::createLayout()
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
    layout->addWidget( itemExpandedText_ );
    layout->addWidget( fsItemUnitPrice_ );
    layout->addStretch();
    layout->addWidget( fsItemTotalPriceLabel_ );
    layout->addWidget( fsItemTotalPrice_ );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceItemImage::updateImages()
{
    // sanity check
    if ( !hook_ )
        return;

    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    //
    // expanded text panel
    //

    QPalette p( palette() );

    /// determines if we have frequent shopper information to display
    bool itemHasFreqShopperPricing;

    /// The expanded text
    QString itemExpandedText;

    if( scaleConfig_->scaleConfig.enableHelpPrompts() )
    {
        itemHasFreqShopperPricing = false;
        itemExpandedText = "";
    }
    else
    {
        if( "1" == hook_->fieldValue( sessionHooks::itemHasFreqShopperPricing ) )
            itemHasFreqShopperPricing = true;
        else
            itemHasFreqShopperPricing = false;

    if ( !displayFreqShopper() )
        itemHasFreqShopperPricing = false;

        itemExpandedText = hook_->fieldValue( sessionHooks::itemExpandedText );
        itemExpandedText = itemExpandedText.trimmed();
    }

    /// Product image information - OR, the onscreen help instructions if it's enabled.
    /// If onscreen help messages are enabled, they are loaded into the product image field (see plumainscreen.cpp)
    QString itemImage( hook_->fieldValue( sessionHooks::itemImage ) );

    if( scaleConfig_->scaleConfig.enableHelpPrompts() )
    {
        if ( !itemImage.isEmpty() )
        {
            QPixmap image( itemImage );

            if ( !image.isNull() )
            {
                sizeToFitPixmap stretcher( size() );
                QPixmap resizedImage( stretcher( image ) );

                if ( !resizedImage.isNull() )
                    p.setBrush( backgroundRole(), resizedImage );
            }
        }
    }
    else
    {
        bool itemImageValid( false );

    // for pending text do nothing... this prevents flicker when plu has image and expanded text
    if ( "<pending>" != itemExpandedText )
        itemExpandedText_->setText( itemExpandedText );

    // check for item image when:
    // 1) item has no expanded text
    // 2) item has frequent shopper pricing
    if (( itemExpandedText.isEmpty() ) || ( itemHasFreqShopperPricing ))
        {
        if ( !itemImage.isEmpty() )
        {
            QPixmap image( itemImage );

            if ( !image.isNull() )
            {
                sizeToFitPixmap stretcher( size() );
                QPixmap resizedImage( stretcher( image ) );

                if ( !resizedImage.isNull() )
                {
                    p.setBrush( backgroundRole(), resizedImage );
                    itemImageValid = true;
                }
            }
        }
        }

    if ( !itemImageValid )
        p.setColor( backgroundRole(), Qt::transparent );
    }

    // set new palette
    setPalette( p );

    //
    // frequent shopper pricing
    //

    // set widget visibility
    itemExpandedText_->setVisible( !itemHasFreqShopperPricing );

    fsItemUnitPriceLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemUnitPrice_->setVisible( itemHasFreqShopperPricing );
    fsItemSavingsLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemSavings_->setVisible( itemHasFreqShopperPricing );
    fsItemTotalPriceLabel_->setVisible( itemHasFreqShopperPricing );
    fsItemTotalPrice_->setVisible( itemHasFreqShopperPricing );

    // update frequent shopper amounts
    if ( itemHasFreqShopperPricing )
    {
        QString itemDiscountedSavings( hook_->fieldValue( sessionHooks::itemDiscountedSavings ) );
        QString itemDiscountedTotalPrice( hook_->fieldValue( sessionHooks::itemDiscountedTotalPrice ) );
        QString itemDiscountedUnitPrice( hook_->fieldValue( sessionHooks::itemDiscountedUnitPrice ) );

        itemDiscountedSavings = itemDiscountedSavings.trimmed();
        itemDiscountedTotalPrice = itemDiscountedTotalPrice.trimmed();
        itemDiscountedUnitPrice = itemDiscountedUnitPrice.trimmed();

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
    }
}

