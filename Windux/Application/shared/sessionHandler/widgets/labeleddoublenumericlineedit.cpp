/**
 * @file labeleddoublenumericlineedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeleddoublenumericlineedit.h"

#include <QTimer>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleNumericLineEdit::labeledDoubleNumericLineEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image )
{
    setObjectName( "labeledDoubleNumericLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleNumericLineEdit::labeledDoubleNumericLineEdit( const QString& text, QWidget *parent, const QPixmap& image ) :
    _Mybase( text, parent, image )
{
    setObjectName( "labeledDoubleNumericLineEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledDoubleNumericLineEdit::~labeledDoubleNumericLineEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleNumericLineEdit::setReadOnly( bool value )
{
    childSecond_->setDisabled( value );
    childSecond_->setReadOnly( value );

    _Mybase::setReadOnly( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledDoubleNumericLineEdit::eventFilter( QObject *obj, QEvent *event )
{
    // do not allow childSecond_ to get focus, channel the focus to child_
    if ( childSecond_ == obj )
        if ( QEvent::FocusIn == event->type() )
        {
            // force focus to childSecond_
            timer_->setSingleShot( true );
            timer_->start( 0 );

            // eat the event
            return true;
        }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleNumericLineEdit::onTimeout()
{
    child_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledDoubleNumericLineEdit::initialize()
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

    // create timer
    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    // install event filters
    childSecond_->installEventFilter( this );

    // add widget to layout
    bottomLayoutInner_->addStretch();
    bottomLayoutInner_->addWidget( childSecond_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

