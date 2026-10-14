/**
 * @file toolbutton.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "toolbutton.h"

#include <QStyleOption>
#include <QStylePainter>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
toolButton::toolButton( QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
toolButton::toolButton( const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( text, parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
toolButton::toolButton( const QIcon& icon, const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( icon, text, parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
toolButton::~toolButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void toolButton::setDown( bool value )
{
    LOG_CLASS_INFO( "toolButton", "setDown" );

    LOG_DEBUG( "setting state to %d", value );
    _Mybase::setDown( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void toolButton::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    QStylePainter p( this );
    QStyleOptionToolButton option;

    // init style
    initStyleOption( &option );

    // draw control
    if ( background().isNull() )
        p.drawComplexControl( QStyle::CC_ToolButton, option );
    else
    {
        drawControlBackground( &p );

        p.drawControl( QStyle::CE_ToolButtonLabel, option );
        p.drawPrimitive( QStyle::PE_FrameFocusRect, option );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void toolButton::initialize()
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "hobartToolButton" );

    // connect slots
    connect( this, SIGNAL( pressed() ), SLOT( onPressed() ) );
    connect( this, SIGNAL( released() ), SLOT( onReleased() ) );
    connect( this, SIGNAL( toggled(bool) ), SLOT( onToggled(bool) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

