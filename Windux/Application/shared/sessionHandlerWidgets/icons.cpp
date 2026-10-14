/**
 * @file icons.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "icons.h"

#include <QPainter>
#include <QTimer>
#include <QTime>

/// Hobart namespace.
namespace hobart {

QTimer *alertableIcon::alertTimer_ = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// icon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
icon::icon( QWidget *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
icon::~icon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize icon::sizeHint() const
{
    return icon_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void icon::setPixmap( const QPixmap& value )
{
    icon_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void icon::mouseReleaseEvent( QMouseEvent *event )
{
    Q_EMIT clicked();

    _Mybase::mouseReleaseEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void icon::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    // validate icon
    if ( icon_.isNull() )
        return;

    QPainter painter( this );
    QSize size( sizeHint() );

    painter.drawPixmap(
        (size.width() - icon_.width()) / 2,
        (size.height() - icon_.height()) / 2,
        icon_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap icon::scalePixmap( const QPixmap &pixmap ) const
{
    QSize pixmapSize( pixmap.size() );
    pixmapSize.scale( size(), Qt::KeepAspectRatio );

    QPixmap scaledPixmap( size() );
    scaledPixmap.fill( Qt::transparent );

    QPainter painter( &scaledPixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawPixmap(
        (width() - pixmapSize.width()) / 2,
        (height() - pixmapSize.height()) / 2,
        pixmapSize.width(),
        pixmapSize.height(),
        pixmap );

    return scaledPixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// alertableIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
alertableIcon::alertableIcon( QWidget *parent ) :
    _Mybase( parent ),
    alertState_( false ),
    blinking_( false )
{
    setObjectName( "hobartAlertableIcon" );

    // create timer
    if ( !alertTimer_ )
    {
        alertTimer_ = new QTimer();

        alertTimer_->setInterval( 1000 );
        alertTimer_->setSingleShot( false );
        alertTimer_->start();
    }

    // set visibility
    setVisible( alwaysVisible() );

    // connect slots
    connect( alertTimer_, SIGNAL( timeout() ), SLOT( onAlertTimerTimeout() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alertableIcon::~alertableIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void alertableIcon::setAlertState( bool value )
{
    if ( value == alertState_ )
        return;

    alertState_ = value;

    if ( alertState_ )
        show();
    else if ( !alwaysVisible() )
        hide();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void alertableIcon::setAlertPixmap( const QPixmap& value )
{
    alertIcon_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void alertableIcon::paintEvent( QPaintEvent *event )
{
    // use normal icon when blinking or not in alert state
    if (( !alertState_ ) || ( blinking_ ))
        _Mybase::paintEvent( event );
    else
    {
        // validate icon
        if ( alertIcon_.isNull() )
            return;

        QPainter painter( this );
        QSize size( sizeHint() );

        painter.drawPixmap(
            (size.width() - alertIcon_.width()) / 2,
            (size.height() - alertIcon_.height()) / 2,
            alertIcon_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void alertableIcon::onAlertTimerTimeout()
{
    QTime now( QTime::currentTime() );

    blinking_ = (0 == (now.second() % 2));

    if ( alertState_ )
        update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
