/**
 * @file gcdlanguage.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "gcdlanguage.h"

#include <QApplication>

#include <QLocale>
#include <QTextCodec>

QTranslator *gcdLanguage::translator_ = NULL;

QMap<gcdCurrencyType::type, QString> gcdCurrencyType::symbols_;
QMap<gcdCurrencyType::type, QString> gcdCurrencyType::unitSeparators_;

///////////////////////////////////////////////////////////////////////////////////////////////////
gcdLanguageType::type gcdLanguage::language()
{
    QLocale locale;

    if ( "French" == QLocale::languageToString( locale.language() ) )
        return gcdLanguageType::French;
    else if ( "Spanish" == QLocale::languageToString( locale.language() ) )
        return gcdLanguageType::Spanish;

    return gcdLanguageType::English;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLanguage::setLanguage( gcdLanguageType::type type )
{
    LOG_CLASS_INFO( "gcdLanguage", "setLanguage" );

    // remove existing translator
    if ( translator_ )
    {
        QApplication::removeTranslator( translator_ );

        delete translator_;
        translator_ = NULL;
    }

    // update locale and install translator
    switch ( type )
    {
    case gcdLanguageType::English:
        QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );

// Note: This has been depreciated in Qt5... do we need this in Qt4?
#if ( QT_VERSION < 0x050000 )
        QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
#endif

        break;

    case gcdLanguageType::French:
        QLocale::setDefault( QLocale( QLocale::French, QLocale::Canada ) );
        installTranslator( ":/languages/gcd_lang_fr" );

// Note: This has been depreciated in Qt5... do we need this in Qt4?
#if ( QT_VERSION < 0x050000 )
        QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
#endif

        break;

    case gcdLanguageType::Spanish:
        QLocale::setDefault( QLocale( QLocale::Spanish, QLocale::Mexico ) );
        installTranslator( ":/languages/gcd_lang_es" );

// Note: This has been depreciated in Qt5... do we need this in Qt4?
#if ( QT_VERSION < 0x050000 )
        QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
#endif

        break;

    default:
        LOG_WARN( "unknown language type %d", type );
        break;
    }

    QLocale locale;

    LOG_INFO( "current locale is '%s'", qPrintable( QLocale::languageToString( locale.language() ) ) );

// Note: This has been depreciated in Qt5... do we need this in Qt4?
#if ( QT_VERSION < 0x050000 )
    QTextCodec *textCodec = QTextCodec::codecForTr();

    if ( !textCodec )
        LOG_WARN( "NULL translation codec installed!", NULL );
    else
    {
        LOG_INFO( "current codec is '%s'", qPrintable( textCodec->name() ) );
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gcdLanguage::get( type t, gcdWeighModeType::type weighMode, bool weigherAttached )
{
    LOG_CLASS_INFO( "gcdLanguage", "get" );

    switch ( t )
    {
    //-------------------------------------------------------------------------
    case PricePerWeight:
        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Price /kg" );

        return QObject::tr( "Price /lb" );

    //-------------------------------------------------------------------------
    case PricePerWeightFor:
        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( " kg for" );

        return QObject::tr( " lb for" );

    //-------------------------------------------------------------------------
    case PricePerEach:
        // FT-851: W&M does not want a "price /each" on the display
        //return QObject::tr( "Price /Each" );
        return "";

    //-------------------------------------------------------------------------
    case PricePerFractional:
        return QObject::tr( "Price /" );

    //-------------------------------------------------------------------------
    case PricePerUnit:
        return QObject::tr( "Unit Price" );

    //-------------------------------------------------------------------------
    case Tare:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Tare kg" );

        return QObject::tr( "Tare lb" );

    //-------------------------------------------------------------------------
    case TotalPrice:
        return QObject::tr( "Total Price" );

    //-------------------------------------------------------------------------
    case Weight:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Weight kg" );

        return QObject::tr( "Weight lb" );

    //-------------------------------------------------------------------------
    case ManualWeight:
        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Manual Weight kg" );

        return QObject::tr( "Manual Weight lb" );

    //-------------------------------------------------------------------------
    case NetWeight:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Net Weight kg" );

        return QObject::tr( "Net Weight lb" );

    //-------------------------------------------------------------------------
    case NowServingCustomer:
        return QObject::tr( "Now Serving" );

    //-------------------------------------------------------------------------
    case Quantity:
        return QObject::tr( "Quantity" );

    //-------------------------------------------------------------------------
    case Capacity:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Capacity 15 x.005 kg" );

        return QObject::tr( "Capacity 30 x.01 lb" );

    //-------------------------------------------------------------------------
    case CapacityDualRangeUp:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Capacity 15 x.005 kg, *6 x .002 kg*" );

        return QObject::tr( "Capacity 30 x.01 lb, *12 x .005 lb*" );

    //-------------------------------------------------------------------------
    case CapacityDualRangeDown:
        if ( !weigherAttached )
            return QString();

        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Capacity *15 x.005 kg,* 6 x .002 kg" );

        return QObject::tr( "Capacity *30 x.01 lb,* 12 x .005 lb" );

    //-------------------------------------------------------------------------
    case CapacityEmpty:
        return QString("");

    //-------------------------------------------------------------------------
    case AnnunZero:
        return QObject::tr( "ZERO" );

    //-------------------------------------------------------------------------
    case AnnunPrePack:
        return QObject::tr( "PrePack" );

    //-------------------------------------------------------------------------
    case FSPricePerWeight:
        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( "Price /kg" );

        return QObject::tr( "Price /lb" );

    //-------------------------------------------------------------------------
    case FSPricePerWeightFor:
        if ( gcdWeighModeType::Metric == weighMode )
            return QObject::tr( " kg for" );

        return QObject::tr( " lb for" );

    //-------------------------------------------------------------------------
    case FSPricePerEach:
        // FT-851: W&M does not want a "price /each" on the display
        //return QObject::tr( "Price /Each" );
        return "";

    //-------------------------------------------------------------------------
    case FSPricePerFractional:
        return QObject::tr( "Price /" );

    //-------------------------------------------------------------------------
    case FSPricePerUnit:
        return QObject::tr( "Unit Price" );

    //-------------------------------------------------------------------------
    case FSTotalPrice:
        return QObject::tr( "Total" );

    //-------------------------------------------------------------------------
    case FSSavings:
        return QObject::tr( "Savings" );

    //-------------------------------------------------------------------------
    default:
        break;
    }

    LOG_WARN( "no string %d exists", t );
    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gcdCurrencyType::getSymbol( type t )
{
    if ( symbols_.contains( t ) )
        return symbols_[t];

    return "$";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCurrencyType::setSymbol( type t, const QString& value )
{
    symbols_[t] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gcdCurrencyType::getUnitSeparator( type t )
{
    if ( unitSeparators_.contains( t ) )
        return unitSeparators_[t];

    return ".";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdCurrencyType::setUnitSeparator( type t, const QString& value )
{
    unitSeparators_[t] = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLanguage::installTranslator( const QString& file )
{
    LOG_CLASS_INFO( "gcdLanguage", "installTranslator" );

    // allocate new translator
    translator_ = new QTranslator();

    // load it from file
    if ( translator_->load( file ) )
        QApplication::installTranslator( translator_ );
    else
    {
        LOG_ERROR( "failed to load translation '%s'!", qPrintable( file ) );

        delete translator_;
        translator_ = NULL;
    }
}
