/**
 * @file labelednumericlineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelednumericlineedit.h"

#include <QMouseEvent>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledNumericLineEdit::labeledNumericLineEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image )
{
    setObjectName( "hobartLabeledNumericEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledNumericLineEdit::labeledNumericLineEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledNumericLineEdit::~labeledNumericLineEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledNumericLineEdit::setForced( bool value )
{
    // FT-808

    // when forced do not zero field out
    child_->setInputFocusZeroing( !value );

    // invoke base
    _Mybase::setForced( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledNumericLineEdit::eventFilter( QObject *obj, QEvent *event )
{
    bool result = _Mybase::eventFilter( obj, event );

    if ( child_ == obj )
        if (( QEvent::FocusIn == event->type() ) || ( QEvent::FocusOut == event->type() ))
        {
            // check access level
            if ( QEvent::FocusIn == event->type() )
                if ( !checkOperatorAccessLevel() )
                {
                    Q_EMIT invalidAccessLevel();

                    // eat the event
                    return true;
                }

            updateBackgroundPalette();

            Q_EMIT focused( (QEvent::FocusIn == event->type()), geometry() );
        }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledNumericLineEdit::initialize()
{
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( child_ );

    child_->setFrame( false );

    // connect slots
    connect( child_, SIGNAL( cursorPositionChanged(int,int) ), SIGNAL( cursorPositionChanged(int,int) ) );
    connect( child_, SIGNAL( editingFinished() ), SIGNAL( editingFinished() ) );
    connect( child_, SIGNAL( returnPressed() ), SIGNAL( returnPressed() ) );
    connect( child_, SIGNAL( selectionChanged() ), SIGNAL( selectionChanged() ) );
    connect( child_, SIGNAL( textChanged(QString) ), SIGNAL( textChanged(QString) ) );
    connect( child_, SIGNAL( textEdited(QString) ), SIGNAL( textEdited(QString) ) );

    // install event filter
    child_->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
