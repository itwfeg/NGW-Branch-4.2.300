/**
 * @file labeleddoublelineeditarray.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeleddoublelineeditarray.h"

#include <QSignalMapper>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleLineEditArray::labeledDoubleLineEditArray( int count, QWidget *parent, const QPixmap& image ) :
    _Mybase( count, parent, image )
{
    setObjectName( "hobartLabeledDoubleLineEditArray" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleLineEditArray::~labeledDoubleLineEditArray()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setAlignment( Qt::Alignment value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setAlignment( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setAlignmentSecond( Qt::Alignment value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setAlignmentSecond( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setEnabledSecond( bool value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setEnabledSecond( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setFont( const QFont& value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setFont( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setFontSecond( const QFont& value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setFontSecond( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setValidator( const QValidator *value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setValidator( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::setValidatorSecond( const QValidator *value )
{
    int index = arraySize_;

    while ( index-- )
        child_->widgetAt( index )->setValidatorSecond( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::clear()
{
    int index = arraySize_;

    while ( index-- )
        clear( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::clearSecond()
{
    int index = arraySize_;

    while ( index-- )
        clearSecond( index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleLineEditArray::initialize()
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
        connect( widget, SIGNAL( cursorPositionChangedSecond(int,int) ), SLOT( onCursorPositionChangedSecond(int,int) ) );
        connect( widget, SIGNAL( editingFinished() ), SLOT( onEditingFinished() ) );
        connect( widget, SIGNAL( editingFinishedSecond() ), SLOT( onEditingFinishedSecond() ) );
        connect( widget, SIGNAL( focused(bool,QRect) ), SLOT( onFocused(bool,QRect) ) );
        connect( widget, SIGNAL( focusedSecond(bool,QRect) ), SLOT( onFocusedSecond(bool,QRect) ) );
        connect( widget, SIGNAL( returnPressed() ), SLOT( onReturnPressed() ) );
        connect( widget, SIGNAL( returnPressedSecond() ), SLOT( onReturnPressedSecond() ) );
        connect( widget, SIGNAL( selectionChanged() ), SLOT( onSelectionChanged() ) );
        connect( widget, SIGNAL( selectionChangedSecond() ), SLOT( onSelectionChangedSecond() ) );
        connect( widget, SIGNAL( textChanged(QString) ), SLOT( onTextChanged(QString) ) );
        connect( widget, SIGNAL( textChangedSecond(QString) ), SLOT( onTextChangedSecond(QString) ) );
        connect( widget, SIGNAL( textEdited(QString) ), SLOT( onTextEdited(QString) ) );
        connect( widget, SIGNAL( textEditedSecond(QString) ), SLOT( onTextEditedSecond(QString) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


