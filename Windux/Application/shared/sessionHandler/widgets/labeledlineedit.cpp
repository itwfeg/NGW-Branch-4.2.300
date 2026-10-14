/**
 * @file labeledlineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledlineedit.h"

#include <QMouseEvent>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLineEdit::labeledLineEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image )
{
    setObjectName( "hobartLabeledLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLineEdit::labeledLineEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image )
{
    setObjectName( "hobartLabeledLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledLineEdit::~labeledLineEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledLineEdit::eventFilter( QObject *obj, QEvent *event )
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
void labeledLineEdit::onToggleButtonClicked()
{
    // toggle widget available
    setWidgetAvailable( !widgetAvailable() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledLineEdit::initialize()
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

    // connect toggle button slot
    connect( toggleButton_, SIGNAL( clicked() ), SLOT( onToggleButtonClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
