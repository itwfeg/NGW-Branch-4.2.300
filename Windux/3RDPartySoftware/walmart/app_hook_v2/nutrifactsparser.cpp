/**
 * @file nutrifactsparser.h
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "nutrifactsparser.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QSettings>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/walmart-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

QMutex nutritionFactsParser::fieldMappingMutex_( QMutex::Recursive );
nutritionFactsParser::stringMap nutritionFactsParser::fieldMapping_;

///////////////////////////////////////////////////////////////////////////////////////////////////
nutritionFactsParser::nutritionFactsParser( const QJsonObject& obj )
{
    QMutexLocker guard( &fieldMappingMutex_ );

    if ( !fieldMapping_.size() )
        loadMapping( INTERNAL_CONFIG, "nutrifield", fieldMapping_ );

    parseData( obj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutritionFactsParser::~nutritionFactsParser()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nutritionFactsParser::parseData( const QJsonObject& obj )
{
    static const char SUB = 0x1A;

    // clear existing
    nutrifacts_.clear();

    // iterate keys
    const QStringList fields( obj.keys() );

    foreach ( const QString& field, fields )
    {
        if ( !fieldMapping_.contains( field ) )
            LOG_DEBUG << "unhandled nutrifact field received " << qPrintable( field );
        else
        {
            const QString key( fieldMapping_[field] );

            if ( 4 != key.length() )
                LOG_DEBUG << "unmapped nutrifact field received " << qPrintable( field );
            else
            {
                const QJsonValue valueVal( obj[field] );

                QString value;

                if ( valueVal.isNull() )
                    LOG_TRACE << "skipping null field " << qPrintable( field );
                else if ( valueVal.isString() )
                    value = valueVal.toString();
                else if ( valueVal.isBool() )
                    value = valueVal.toBool() ? "1" : "0";
                else if ( valueVal.isDouble() )
                {
                    double decimalValue( valueVal.toDouble() );
                    int64_t integerValue( valueVal.toVariant().toLongLong() );

                    const double diff( decimalValue - integerValue );

                    // assume minimum 1 decimal place
                    if ( 0.0 == diff )
                        value = QString::number( integerValue ) + ".0";
                    else
                    {
                        value = QString::number( decimalValue, 'f', 4 );

                        int index( value.indexOf( '.' ) );

                        if ( 0 < index )
                        {
                            index += 2;

                            for ( int i( value.length() ); index < i--; )
                            {
                                if ( '0' != value[i] )
                                    break;

                                value.truncate( i );
                            }
                        }
                    }
                }
                else
                {
                    LOG_WARN << "unhandled data type field " << qPrintable( field );
                    continue;
                }

                if ( nutrifacts_.size() )
                    nutrifacts_.append( SUB );

                nutrifacts_.append( key );
                nutrifacts_.append( value );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nutritionFactsParser::loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping )
{
    QSettings configs( filename, QSettings::IniFormat );

    // clear existing mapping
    mapping.clear();

    // load in mappings
    for ( int i( 0 ); i < 1000; ++i )
    {
        const QString key( keyBase + QString( "%0" ).arg( i, 3, 10, QChar( '0' ) ) );
        const QString value( configs.value( key ).toString() );

        if ( value.isEmpty() )
            break;

        // split string into key:value pair
        else if ( 2 <= value.length() )
        {
            const int split( value.indexOf( ':' ) );

            if ( 0 < split )
                mapping[value.left( split )] = value.mid( split+1 );
        }
    }
}
