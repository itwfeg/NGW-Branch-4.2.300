/**
 * @file gcdsignal.h
 * This file contains types used by the GCD frontend for signals/slots.
 *
 * The gcd user interface has two slots exposed to handle commands:
 * @code
 *   void onUpdateUserInterface( gcdUpdateUserInterfaceType::type type, const QString& value );
 *   void onControlMessage( gcdControlMessageType::type type, const QString& value );
 * @endcode
 *
 * User interface updates will update fields within the ui.  Control messages set parameters
 * and perform specific functionality.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCDSIGNAL_H
#define GCDSIGNAL_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// User interface field types.
/**
 * Enumeration class of GCD user interface fields.
 */
class gcdUpdateUserInterfaceType
{
public:

    /// GCD User interface field types.
    enum type
    {
        Tare,
        Weight,
        UnitPrice,
        TotalPrice,
        ZeroAnnuc,
        PrePackAnnuc,
        WeightType,
        UnitPriceType,
        FreqShopUnitPrice,
        FreqShopTotalPrice,
        FreqShopSavings,
        PluNumber,
        Description,
        Marquee,
        DualRangeIndicator,
        WeigherAttached,
        ProductAdvertNone,
        ProductAdvertImage,
        ProductAdvertText,
        ProductAdvertPlaylist,
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Control message types.
/**
 * Enumeration class of GCD control messages.
 */
class gcdControlMessageType
{
public:

    /// Control message types.
    enum type
    {
        HideTestColor,
        OutOfService,
        SetLanguage,
        SetLayout,
        SetPlaybackWaitTime,
        SetPlaylist,
        SetVolume,
        SetWeighMode,
        ShowTestColor,
        SetUnitSeparator,
        SetCurrencySymbol,
        NowServingCustomerNumber,
        DismissOverlay,
        SetLayoutAdvertSetting,
        Website
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDSIGNAL_H
