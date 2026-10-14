/**
 * @file gcdlanguage.h
 * This file contains types and classes to manage language and weigh modes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCDLANGUAGE_H
#define GCDLANGUAGE_H

#include <QMap>
#include <QString>
#include <QTranslator>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Weigh mode types.
/**
 * Enumeration class of weigh modes.
 */
class gcdWeighModeType
{
public:

    /// Weigh mode type.
    enum type
    {
        Avoir,                                  ///< american pounds (i.e. "Lb")
        Metric                                  ///< metric (i.e. "Kg")
    };

    /// Convert from string.
    /**
     * @param[in] weighMode  weigh mode in string from (see above)
     * @return  weigh mode type
     */
    static type fromString( const QString& weighMode ) {
        if ( toString( Metric ) == weighMode )
            return Metric;
        return Avoir;
    }

    /// Convert to string.
    /**
     * @param[in] t  weigh mode type
     * @return  weigh mode in string from (see above)
     */
    static QString toString( type t ) {
        if ( Metric == t )
            return "Kg";
        return "Lb";
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Language types.
/**
 * Enumeration class of languages.
 */
class gcdLanguageType
{
public:

    /// Language type.
    enum type
    {
        English,                                ///< english (i.e. "En")
        French,                                 ///< french (i.e. "Fr")
        Spanish                                 ///< spanish (i.e. "Sp")
    };

    /// Convert from string.
    /**
     * @param[in] language  language in string form (see above)
     * @return  language type
     */
    static type fromString( const QString& language ) {
        if ( toString( French ) == language )
            return French;
        else if ( toString( Spanish ) == language )
            return Spanish;
        return English;
    }

    /// Convert to string.
    /**
     * @param[in] t  language type
     * @return  language in string form (see above)
     */
    static QString toString( type t ) {
        if ( French == t )
            return "Fr";
        else if ( Spanish == t )
            return "Sp";
        return "En";
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Language types and translation class.
/**
 * This class will manage language settings and translate strings as per the current language
 * setting.
 *
 * For example, to change language to spanish and get a string you would:
 * @code
 *   // install spanish codec
 *   gcdLanguage::setLanguage( gcdLanguageType::Spanish );
 *   // get translated string
 *   QString string = gcdLanguage::get( gcdLanguage::TotalPrice, gcdWeighModeType::Metric );
 * @endcode
 *
 * This class uses the @c QTranslator system to perform conversion.  The French and Spanish
 * translation files are part of this project and embedded within the resource system.
 */
class gcdLanguage
{
    typedef gcdLanguage _Myt;

public:

    /// String types.
    enum type
    {
        PricePerWeight,
        PricePerWeightFor,
        PricePerEach,
        PricePerFractional,
        PricePerUnit,
        Tare,
        TotalPrice,
        Weight,
        ManualWeight,
        NetWeight,
        NowServingCustomer,
        Quantity,
        Capacity,
        CapacityDualRangeUp,
        CapacityDualRangeDown,
        CapacityEmpty,
        AnnunZero,
        AnnunPrePack,
        FSPricePerWeight,
        FSPricePerWeightFor,
        FSPricePerEach,
        FSPricePerFractional,
        FSPricePerUnit,
        FSTotalPrice,
        FSSavings
    };

    /// Get language.
    /**
     * @return  current language installed
     */
    static gcdLanguageType::type language();

    /// Set language.
    /**
     * @param[in] type  language to install
     */
    static void setLanguage( gcdLanguageType::type type );

    /// Get string translation.
    /**
     * Uses current installed language codec to convert a string.
     *
     * @param[in] t  string to convert
     * @param[in] weighMode  weigh mode for conversion
     * @param[in] weigherAttached  @c true if weigher attached, @c false otherwise
     * @return  string converted to current language
     */
    static QString get( type t, gcdWeighModeType::type weighMode, bool weigherAttached );

private:

    static QTranslator *translator_;

    // not implemented
    gcdLanguage();

    // not implemented
    gcdLanguage( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Install translator.
    static void installTranslator( const QString& file );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Currency types.
/**
 * Enumeration class of currencies.
 */
class gcdCurrencyType
{
    typedef gcdCurrencyType _Myt;

public:

    /// Currency type.
    enum type
    {
        AmericanDollar,
        CanadianDollar,
        Peso
    };

    /// Get currency symbol.
    /**
     * @param[in] t  currency type
     * @return  currency symbol in string format
     */
    static QString getSymbol( type t );

    /// Set currency symbol.
    /**
     * @param[in] t  currency type
     * @param[in] value  currency symbol in string format
     */
    static void setSymbol( type t, const QString& value );

    /// Get currency unit separator.
    /**
     * @param[in] t  currency type
     * @return  currency unit separator in string format
     */
    static QString getUnitSeparator( type t );

    /// Set currency unit separator.
    /**
     * @param[in] t  currency type
     * @param[in] value  currency unit separator in string format
     */
    static void setUnitSeparator( type t, const QString& value );

private:

    static QMap<type, QString> symbols_;
    static QMap<type, QString> unitSeparators_;

    // not implemented
    gcdCurrencyType();

    // not implemented
    gcdCurrencyType( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDLANGUAGE_H
