/**
 * @file sessionmenupanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "sessionmenupanel.h"

#include <backgroundwidgettypes.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMenuPanel::SessionMenuPanel( DialogLocation location, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( location, parent, f ),
    layout_( NULL ),
    layoutAlignment_( Qt::AlignCenter )
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "SessionMenuPanel" );

    // install an event filter
    parent->installEventFilter( this );

    // setup layout
    switch ( location )
    {
    case LeftDialogLocation:
    case RightDialogLocation:
        layout_ = new QHBoxLayout( this );
        break;
    default:
        layout_ = new QVBoxLayout( this );
        break;
    }

    layout_->setContentsMargins( QMargins() );
    layout_->setSpacing( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMenuPanel::~SessionMenuPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize SessionMenuPanel::sizeHint() const
{
    return size_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMenuPanel::addWidget( QWidget *widget )
{
    // we now own this widget
    widget->setParent( this );

    // add widget to layout
    switch ( location() )
    {
    case BottomDialogLocation:
    case RightDialogLocation:
        layout_->insertWidget( 0, widget );
        break;
    default:
        layout_->addWidget( widget );
        break;
    }

    layout_->setAlignment( widget, layoutAlignment_ );

    // update size to reflect new widget
    size_.rwidth() += widget->width();
    size_.rheight() = qMax( size_.height(), widget->height() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *SessionMenuPanel::rendererAppearance() const
{
    return new hobart::transparentBackgroundWidgetRendererAppearance();
}

