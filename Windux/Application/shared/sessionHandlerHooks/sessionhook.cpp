/**
 * @file sessionhook.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "sessionhook.h"

#include <Session.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHook::sessionHook( const QString& name, QObject *parent ) :
    _Mybase( parent ),
    session_( NULL )
{
    setObjectName( name );

    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHook::sessionHook( QObject *parent ) :
    _Mybase( parent )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sessionHook::~sessionHook()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitChangeFieldValue( int type, const QString &value )
{
    Q_EMIT changeFieldValue( type, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitChangeLanguage()
{
    Q_EMIT changeLanguage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitResetPrimaryLanguage()
{
    Q_EMIT resetPrimaryLanguage();
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitDialogButtonPress( int role )
{
    Q_EMIT dialogButtonPress( role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitPrint()
{
    Q_EMIT print();         // SLOT calls SessionMainWindow::onPrintKeyPressed()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    Q_EMIT softkeyClicked( w, b, action, param, feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitShutdownKeyboards()
{
    Q_EMIT shutdownKeyboards();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitZeroScale()
{
    Q_EMIT zeroScale();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::emitAuditLog()
{
    Q_EMIT auditLog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Session *sessionHook::session() const
{
    return session_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString sessionHook::fieldValue( fieldType type ) const
{
    return fields_[type];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::clearItemFieldValues()
{
    foreach ( fieldType i, itemFields_ )
        fields_[i] = QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::initialize()
{
    // init item fields
    itemFields_.append( sessionHooks::itemByCount );
    itemFields_.append( sessionHooks::itemCode );
    itemFields_.append( sessionHooks::itemDescription );
    itemFields_.append( sessionHooks::itemDiscountedSavings );
    itemFields_.append( sessionHooks::itemDiscountedTotalPrice );
    itemFields_.append( sessionHooks::itemDiscountedUnitPrice );
    itemFields_.append( sessionHooks::itemExceptionPrice );
    itemFields_.append( sessionHooks::itemExpandedText );
    itemFields_.append( sessionHooks::itemHasExceptionPricing );
    itemFields_.append( sessionHooks::itemHasFreqShopperPricing );
    itemFields_.append( sessionHooks::itemImage );
    itemFields_.append( sessionHooks::itemNutrifactsPanel );
    itemFields_.append( sessionHooks::itemTare );
    itemFields_.append( sessionHooks::itemTotalPrice );
    itemFields_.append( sessionHooks::itemUnitPrice );
    itemFields_.append( sessionHooks::itemUnitPriceCountFor );
    itemFields_.append( sessionHooks::itemUnitPriceFractional );
    itemFields_.append( sessionHooks::itemUnitPricePoundsFor );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::setCurrentItem( itemType type )
{
    // no item zeros out item fields!
    if ( sessionHooks::none == type )
        clearItemFieldValues();

    // invoke derived method
    onUpdateCurrentItem( type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::setFieldValue( fieldType type, const QString& value )
{
    // nothing to do
    if ( fields_[type] == value )
        return;

    // update mapping
    fields_[type] = value;

    // invoke derived method
    onUpdateFieldValue( type, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sessionHook::setSession( Session *value )
{
    // nothing to do
    if ( session_ == value )
        return;

    // disconnect from previous session
    if ( session_ )
    {
        disconnect( session_, 0, this, 0 );
    }

    // update value
    session_ = value;

    // invoke derived method
    onUpdateSession( value );
}
