/**
 * @file logappender.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "logappender.h"
#include "logline.h"

#include <QTextStream>
#include <QDateTime>
#include <QThread>

#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
void logAppender::write(
    logLevel::type level,
    const char *file,
    unsigned int line,
    const char *name,
    const char *function,
    const char *format,
    ... )
{
    if ( enabled( level ) )
    {
        logLine data( level, file, line, name, function );

        va_list params;
        va_start( params, format );

        QString buffer;
        buffer.vsprintf( format, params );

        data.setText( buffer );

        va_end( params );

        // append data to logs
        append( data );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logAppender::writeHex(
    logLevel::type level,
    const char *file,
    unsigned int line,
    const char *name,
    const char *function,
    const void *buffer,
    unsigned int bufferLen )
{
    if ( enabled( level ) )
    {
        logLine data( level, file, line, name, function );

        data.setTextHex( buffer, bufferLen );

        // append data to logs
        append( data );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool logAppender::enabled( logLevel::type value ) const
{
    return (( logLevel::Disabled != level() ) && ( value <= level() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logAppender::copy( const _Myt& rhs )
{
    // remove all elements
    while ( size() )
        pop_front();

    // copy all elements from rhs
    for ( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
        push_back( (*i) );

    // copy properties
    name_ = rhs.name_;
    level_ = rhs.level_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logAppender::append(
    const logLine& data )
{
    for ( iterator i = begin(); i != end(); ++i )
        (*i)->write( data );
}
