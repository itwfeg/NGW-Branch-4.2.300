/**
 * @file labeledlineeditarray.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledlineeditarray.h"

#include <QSignalMapper>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLineEditArray::labeledLineEditArray( int count, QWidget *parent, const QPixmap& image ) :
    _Mybase( count, parent, image )
{
    setObjectName( "hobartLabeledLineEditArray" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLineEditArray::~labeledLineEditArray()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEditArray::setAlignment( Qt::Alignment value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setAlignment( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEditArray::setFont( const QFont& value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setFont( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEditArray::setValidator( const QValidator *value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setValidator( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEditArray::clear()
{
    int index = arraySize_;

    while ( index-- )
        clear( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEditArray::initialize()
{
    int index = arraySize_;

    // create signal mapper
    signalMapper_ = new QSignalMapper( this );
    connect( signalMapper_, SIGNAL( mapped(int) ), SLOT( onMapped(int) ) );

    // init array widgets
    while ( index-- )
    {
        labeledLineEdit *widget = child_->widgetAt( index );

        widget->setLabelVisible( false );

        // setup signal mapper
        signalMapper_->setMapping( widget, index );
        connect( widget, SIGNAL( focused(bool,QRect) ), signalMapper_, SLOT( map() ) );

        // connect slots
        connect( widget, SIGNAL( cursorPositionChanged(int,int) ), SLOT( onCursorPositionChanged(int,int) ) );
        connect( widget, SIGNAL( editingFinished() ), SLOT( onEditingFinished() ) );
        connect( widget, SIGNAL( focused(bool,QRect) ), SLOT( onFocused(bool,QRect) ) );
        connect( widget, SIGNAL( returnPressed() ), SLOT( onReturnPressed() ) );
        connect( widget, SIGNAL( selectionChanged() ), SLOT( onSelectionChanged() ) );
        connect( widget, SIGNAL( textChanged(QString) ), SLOT( onTextChanged(QString) ) );
        connect( widget, SIGNAL( textEdited(QString) ), SLOT( onTextEdited(QString) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

