/**
 * @file refusebin.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include "sellbysoftkeybutton.h"
#include <QString>


/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
sellBySoftkeyButton::sellBySoftkeyButton() :
    toggleExtraDayON_( false ),
    toggleExtraDayRow_( 0 ),
    toggleExtraDayCol_( 0 ),
    toggledExtraDayImage_( "" )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sellBySoftkeyButton::~sellBySoftkeyButton()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void sellBySoftkeyButton::setToggledExtraDayImage( const QString& value )
{
    if ( value == toggledExtraDayImage_ )
        return;

    toggledExtraDayImage_ = value;
    // should I set a dirty flag here?  not refreshing it in this class so no for now
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sellBySoftkeyButton::resetToggledExtraDayImage()
{
    toggledExtraDayImage_.clear();
    // should I set a dirty flag here?  not refreshing it in this class so no for now
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace



