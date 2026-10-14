/**
 * @file pushbutton.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "pushbutton.h"

#include <QStyleOption>
#include <QStylePainter>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
pushButton::pushButton( QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pushButton::pushButton( const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( text, parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pushButton::pushButton( const QIcon& icon, const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( icon, text, parent, image, imagePressed )
{
    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pushButton::~pushButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void pushButton::setDown( bool value )
{
    LOG_CLASS_INFO( "pushButton", "setDown" );

    LOG_DEBUG( "setting state to %d", value );
    _Mybase::setDown( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void pushButton::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    QStylePainter p( this );
    QStyleOptionButton option;

    // init style
    initStyleOption( &option );

    // draw control
    if ( background().isNull() )
        p.drawControl( QStyle::CE_PushButton, option );
    else
    {
        drawControlBackground( &p );

        p.drawControl( QStyle::CE_PushButtonLabel, option );
        p.drawPrimitive( QStyle::PE_FrameFocusRect, option );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void pushButton::initialize()
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "hobartPushButton" );

    // connect slots
    connect( this, SIGNAL( pressed() ), SLOT( onPressed() ) );
    connect( this, SIGNAL( released() ), SLOT( onReleased() ) );
    connect( this, SIGNAL( toggled(bool) ), SLOT( onToggled(bool) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
