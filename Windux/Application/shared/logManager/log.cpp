/**
 * @file log.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "log.h"
#include "logline.h"

static const QString DEFAULT_LINE_FORMAT( "%message\\n" );

static const QString DEFAULT_DATE_FORMAT( "MM/dd/yyyy hh:mm:ss.zzz" );

static const QString DEFAULT_NUMBER_FORMAT( "%d" );
static const QString DEFAULT_STRING_FORMAT( "%s" );

///////////////////////////////////////////////////////////////////////////////////////////////////
logger::logger() : lineFormat_( DEFAULT_LINE_FORMAT )
{
    // create list of all formats
    formats_.push_back( "%date" );
    formats_.push_back( "%thread" );
    formats_.push_back( "%levelnum" );
    formats_.push_back( "%level" );
    formats_.push_back( "%class" );
    formats_.push_back( "%method" );
    formats_.push_back( "%message" );
    formats_.push_back( "%file" );
    formats_.push_back( "%line" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logger::write( const logLine& line )
{
    QString data( formatted( line ) );

    QMutexLocker guard( &m_ );
    writeString( data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString logger::formatted( const logLine& line ) const
{
    QString result( lineFormat() );

    QString name;
    QString format;

    // formats
    while ( findFormat( result, name, format ) )
    {
        QString value;

        // --------------------------------------------------------------------
        if ( name.contains( "%date" ) )
        {
            if ( !format.length() )
                format = DEFAULT_DATE_FORMAT;

            value = line.timeStamp().toString( format );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%thread" ) )
        {
            if ( !format.length() )
                format = DEFAULT_NUMBER_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.threadId() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%levelnum" ) )
        {
            if ( !format.length() )
                format = DEFAULT_NUMBER_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.level() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%level" ) )
        {
            if ( !format.length() )
                format = DEFAULT_STRING_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                logLevel::toString( line.level() ).toLatin1().constData() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%class" ) )
        {
            if ( !format.length() )
                format = DEFAULT_STRING_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.className().toLatin1().constData() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%method" ) )
        {
            if ( !format.length() )
                format = DEFAULT_STRING_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.classFunction().toLatin1().constData() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%message" ) )
        {
            if ( !format.length() )
                format = DEFAULT_STRING_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.text().toLatin1().constData() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%file" ) )
        {
            if ( !format.length() )
                format = DEFAULT_STRING_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.sourceFilename().toLatin1().constData() );
        }

        // --------------------------------------------------------------------
        else if ( name.contains( "%line" ) )
        {
            if ( !format.length() )
                format = DEFAULT_NUMBER_FORMAT;

            value.sprintf(
                format.toLatin1().constData(),
                line.sourceLine() );
        }

        result.replace( name, value );
    }

    // carriage return and line feeds
    result.replace( "\\r", "\r" );
    result.replace( "\\n", "\n" );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool logger::findFormat( const QString& line, QString& name, QString& format ) const
{
    int offset = -1;

    format = "";

    // loop through each format, find match
    for ( QStringList::const_iterator i = formats_.begin(); i != formats_.end(); ++i )
        if ( 0 <= (offset = line.indexOf( *i )) )
        {
            name = *i;
            break;
        }

    if ( offset < 0 )
        return false;

    // look for formatting override
    if ( "{" == line.mid( offset + name.length(), 1 ) )
    {
        int end = line.indexOf( "}", offset );

        // found!
        if ( end >= 0 )
        {
            int nameLength = name.length();

            name = line.mid( offset, end - offset + 1 );
            format = name.mid( nameLength + 1, name.length() - nameLength - 2 );
        }
    }

    return true;
}
