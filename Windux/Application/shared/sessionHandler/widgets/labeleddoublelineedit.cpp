/**
 * @file labeleddoublelineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeleddoublelineedit.h"

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleLineEdit::labeledDoubleLineEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image )
{
    setObjectName( "hobartLabeledDoubleLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleLineEdit::labeledDoubleLineEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image )
{
    setObjectName( "hobartLabeledDoubleLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleLineEdit::~labeledDoubleLineEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEdit::setReadOnly( bool value )
{
    childSecond_->setDisabled( value );
    childSecond_->setReadOnly( value );

    _Mybase::setReadOnly( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEdit::initialize()
{
    childSecond_ = new QLineEdit( this );
    childSecond_->setObjectName( "childSecond" );

    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( childSecond_ );

    childSecond_->setFrame( false );

    // connect slots
    connect( childSecond_, SIGNAL( cursorPositionChanged(int,int) ), SIGNAL( cursorPositionChangedSecond(int,int) ) );
    connect( childSecond_, SIGNAL( editingFinished() ), SIGNAL( editingFinishedSecond() ) );
    connect( childSecond_, SIGNAL( returnPressed() ), SIGNAL( returnPressedSecond() ) );
    connect( childSecond_, SIGNAL( selectionChanged() ), SIGNAL( selectionChangedSecond() ) );
    connect( childSecond_, SIGNAL( textChanged(QString) ), SIGNAL( textChangedSecond(QString) ) );
    connect( childSecond_, SIGNAL( textEdited(QString) ), SIGNAL( textEditedSecond(QString) ) );

    // add widget to layout
    bottomLayoutInner_->addStretch();
    bottomLayoutInner_->addWidget( childSecond_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
