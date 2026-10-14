/**
 * @file labeledunitpriceedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledunitpriceedit.h"

#include <font.h>

#include <QComboBox>
#include <QLabel>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledUnitPriceEdit::labeledUnitPriceEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    fractional_( false ),
    weightFor_( false ),
    metric_( false )
{
    setObjectName( "labeledUnitPriceEdit" );

    // initialize
    initialize();

    // setup combo options
    setFractionalComboOptions();
    setWeightForComboOptions();

    // translate
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledUnitPriceEdit::~labeledUnitPriceEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int labeledUnitPriceEdit::fractional() const
{
    int value = fractionalCombo_->itemData( fractionalCombo_->currentIndex() ).toInt();

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setReadOnly( bool value )
{
    // enable/disable combo boxes
    weightForCombo_->setEnabled( !value );
    fractionalCombo_->setEnabled( !value );

    _Mybase::setReadOnly( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int labeledUnitPriceEdit::weightFor() const
{
    return (weightForCombo_->currentIndex()+1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::translate()
{
    updatePrefix();
    updateSuffix();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::clear()
{
    // default entry is first entry
    weightForCombo_->setCurrentIndex( 0 );
    fractionalCombo_->setCurrentIndex( fractionalCombo_->count()-1 );

    _Mybase::clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setFractional( int value )
{
    int index = fractionalCombo_->findData( QVariant( value ) );

    // update combo
    if ( 0 <= index )
        fractionalCombo_->setCurrentIndex( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setFractionalEnabled( bool value )
{
    if ( value == fractional_ )
        return;

    fractional_ = value;

    // fractional pricing and weight for pricing are mutually exclusive
    if (( isFractionalEnabled() ) && ( isWeightForEnabled() ))
        setWeightForEnabled( false );

    fractionalCombo_->setVisible( value );

    updatePrefix();
    updateSuffix();
    updateFontPointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setMetric( bool value )
{
    if ( value == metric_ )
        return;

    metric_ = value;

    setFractionalComboOptions();
    setWeightForComboOptions();

    updatePrefix();
    updateSuffix();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setWeightFor( int value )
{
    bool valid = (( 0 < value ) && ( value <= 9 ));

    // update combo
    if ( valid )
        weightForCombo_->setCurrentIndex( value-1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setWeightForEnabled( bool value )
{
    if ( value == weightFor_ )
        return;

    weightFor_ = value;

    // fractional pricing and weight for pricing are mutually exclusive
    if (( isFractionalEnabled() ) && ( isWeightForEnabled() ))
        setFractionalEnabled( false );

    weightForCombo_->setVisible( value );
    prefix_->setVisible( value );

    updatePrefix();
    updateSuffix();
    updateFontPointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::onFractionalIndexChanged( int index )
{
    if ( isFractionalEnabled() )
    {
        int value = fractionalCombo_->itemData( index ).toInt();

        Q_EMIT fractionalChanged( value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::onWeightForIndexChanged( int index )
{
    if ( isWeightForEnabled() )
        Q_EMIT weightForChanged( index+1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::initialize()
{
    modifyFontPointSize setFontPointSize12( (window()->height() * 12) / 600 );
    modifyFontPointSize setFontPointSize18( (window()->height() * 18) / 600 );

    // weight for combo
    weightForCombo_ = new QComboBox( this );
    weightForCombo_->setFocusPolicy( Qt::NoFocus ); // FT-988
    weightForCombo_->setFont( child_->font() );
    weightForCombo_->setObjectName( "weightForCombo" );
    weightForCombo_->setVisible( false );
    setFontPointSize18( weightForCombo_ );

    connect( weightForCombo_, SIGNAL( currentIndexChanged(int) ), SLOT( onWeightForIndexChanged(int) ) );

    // prefix
    prefix_ = new QLabel( this );
    prefix_->setAlignment( Qt::AlignCenter );
    prefix_->setFont( child_->font() );
    prefix_->setObjectName( "prefix" );
    prefix_->setVisible( false );
    setFontPointSize12( prefix_ );

    // fractional combo
    fractionalCombo_ = new QComboBox( this );
    fractionalCombo_->setFocusPolicy( Qt::NoFocus );
    fractionalCombo_->setFont( child_->font() );
    fractionalCombo_->setObjectName( "fractionalCombo" );
    fractionalCombo_->setVisible( false );
    setFontPointSize18( fractionalCombo_ );

    connect( fractionalCombo_, SIGNAL( currentIndexChanged(int) ), SLOT( onFractionalIndexChanged(int) ) );

    // adjust layout
    bottomLayout_->setSpacing( 7 );
    bottomLayout_->insertWidget( 0, prefix_ );
    bottomLayout_->insertWidget( 0, weightForCombo_ );
    bottomLayout_->addWidget( fractionalCombo_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setFractionalComboOptions()
{
    // update fractional price combo
    fractionalCombo_->clear();

    if ( isMetric() )
    {
        fractionalCombo_->addItem( "100g", QVariant( 100 ) );
        fractionalCombo_->addItem( "kg", QVariant( 1 ) );
    }
    else
    {
        fractionalCombo_->addItem( "1/4 lb", QVariant( 25 ) );
        fractionalCombo_->addItem( "1/2 lb", QVariant( 50 ) );
        fractionalCombo_->addItem( "lb", QVariant( 1 ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::setWeightForComboOptions()
{
    // update weight for price combo
    weightForCombo_->clear();

    weightForCombo_->addItem( "1" );
    weightForCombo_->addItem( "2" );
    weightForCombo_->addItem( "3" );
    weightForCombo_->addItem( "4" );
    weightForCombo_->addItem( "5" );
    weightForCombo_->addItem( "6" );
    weightForCombo_->addItem( "7" );
    weightForCombo_->addItem( "8" );
    weightForCombo_->addItem( "9" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::updatePrefix()
{
    QString text( tr( "lbs\nfor" ) );

    if ( isMetric() )
        text = tr( "kgs\nfor" );

    prefix_->setText( text );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::updateSuffix()
{
    QString text;

    if ( isFractionalEnabled() )
    {
        text = tr( " /" );
    }
    else if ( isWeightForEnabled() )
    {
        // empty suffix
    }
    else
    {
        if ( isMetric() )
            text = tr( " /kg" );
        else
            text = tr( " /lb" );
    }

    child_->setSuffix( text );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUnitPriceEdit::updateFontPointSize()
{
    modifyFontPointSize setFontPointSize18( (window()->height() * 18) / 600 );
    modifyFontPointSize setFontPointSize32( (window()->height() * 32) / 600 );

    if (( isFractionalEnabled() ) || ( isWeightForEnabled() ))
        setFontPointSize18( child_ );
    else
        setFontPointSize32( child_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

