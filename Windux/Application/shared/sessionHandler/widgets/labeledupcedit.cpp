/**
 * @file labeledupcedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledupcedit.h"

#include <font.h>

#include <QComboBox>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledUpcEdit::labeledUpcEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image )
{
    setObjectName( "hobartLabeledUpcEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledUpcEdit::~labeledUpcEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledUpcEdit::isPrefixVisible() const
{
    return prefixCombo_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int labeledUpcEdit::prefix() const
{
    return prefixCombo_->currentIndex();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUpcEdit::clear()
{
    setPrefixVisible( false );
    _Mybase::clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUpcEdit::setPrefix( int value )
{
    prefixCombo_->setCurrentIndex( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUpcEdit::setPrefixVisible( bool visible )
{
    prefixCombo_->setVisible( visible );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledUpcEdit::initialize()
{
    modifyFontPointSize setFontPointSize18( (window()->height() * 18) / 600 );

    // prefix combo
    prefixCombo_ = new QComboBox( this );
    prefixCombo_->setFont( child_->font() );
    prefixCombo_->setObjectName( "prefixCombo" );
    prefixCombo_->setVisible( false );
    setFontPointSize18( prefixCombo_ );

    prefixCombo_->addItem( "0" );
    prefixCombo_->addItem( "1" );
    prefixCombo_->addItem( "2" );
    prefixCombo_->addItem( "3" );
    prefixCombo_->addItem( "4" );
    prefixCombo_->addItem( "5" );
    prefixCombo_->addItem( "6" );
    prefixCombo_->addItem( "7" );
    prefixCombo_->addItem( "8" );
    prefixCombo_->addItem( "9" );

    connect( prefixCombo_, SIGNAL( currentIndexChanged(int) ), SIGNAL( prefixChanged(int) ) );

    // adjust layout
    bottomLayout_->insertSpacing( 0, 8 );
    bottomLayout_->insertWidget( 0, prefixCombo_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


