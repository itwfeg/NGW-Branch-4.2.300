/**
 * @file selfserviceicons.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfserviceicons.h"

#include <pixmap.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWirelessSignalIcon::selfServiceWirelessSignalIcon( const QSize& s, QWidget *parent ) :
    _Mybase( parent ),
    iconSize_( s )
{
    // invert the wired network icon from white to black
    invertWiredIcon();

    // set connection type to force icon update
    setConnectionType( WiredConnectionType );

    // resize pixmaps
    setPixmap( pixmap() );
    setAlertPixmap( alertPixmap() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceWirelessSignalIcon::~selfServiceWirelessSignalIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWirelessSignalIcon::setAlertPixmap( const QPixmap& value )
{
    // resize pixmap
    sizeToFitPixmap stretcher( iconSize_ );
    _Mybase::setAlertPixmap( stretcher( value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceWirelessSignalIcon::setPixmap( const QPixmap& value )
{
    // resize pixmap
    sizeToFitPixmap stretcher( iconSize_ );
    _Mybase::setPixmap( stretcher( value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize selfServiceWirelessSignalIcon::sizeHint() const
{
    return pixmap().size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
