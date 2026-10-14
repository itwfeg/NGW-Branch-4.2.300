/**
 * @file overlaywidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "overlaywidget.h"

#include <palette.h>

#include <QEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////
OverlayWidget::OverlayWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "OverlayWidget" );

    // resize to parent size
    resize( parent->size() );

    // set palette
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( this );

    // we are a graphical overlay, we do not interact with the mouse
    setAttribute( Qt::WA_TransparentForMouseEvents );

    // install an event filter
    parent->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OverlayWidget::~OverlayWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OverlayWidget::eventFilter( QObject *obj, QEvent *event )
{
    // resize our overlay
    if (( obj ) && ( parentWidget() == obj ))
        if ( QEvent::Resize == event->type() )
            resize( parentWidget()->contentsRect().size() );

    return _Mybase::eventFilter( obj, event );
}
