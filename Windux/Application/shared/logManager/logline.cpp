/**
 * @file logline.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "logline.h"

#include <QTextStream>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////
logLine::logLine( logLevel::type level, const QString& file, unsigned int line, const QString& name, const QString& function ) :
    level_( level ),
    sourceFilename_( file ),
    sourceLine_( line ),
    className_( name ),
    classFunction_( function ),
    stamp_( QDateTime::currentDateTime() ),
    threadId_( QThread::currentThreadId() )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logLine::setText( const char *format, ... )
{
    va_list params;
    va_start( params, format );

    text_.vsprintf( format, params );

    va_end( params );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logLine::setTextHex( const void *buffer, unsigned int bufferLen, unsigned int width )
{
    text_ = "";

    // log buffer in hex format
    if (( buffer ) && ( bufferLen ))
    {
        const unsigned char *bufferPtr = (const unsigned char*) buffer;

        QTextStream os( &text_ );

        QString readable;
        QString temp;

        if ( width < MIN_WIDTH )
            width = MIN_WIDTH;
        else if ( width > MAX_WIDTH )
            width = MAX_WIDTH;

        os << "\n";

        for ( unsigned int i = 0; i < bufferLen; ++i )
        {
            if ( 0 == (i%width) )
            {
                if ( 0 != i )
                {
                    os << readable << "\n";
                    readable.clear();
                }

                os << temp.sprintf( "    %08X ", i );
            }

            os << temp.sprintf( "%02X ", bufferPtr[i] );

            readable += temp.sprintf( "%c", (' ' <= bufferPtr[i] && bufferPtr[i] <= '~') ? bufferPtr[i] : '.' );

            if ( i == (bufferLen-1) )
            {
                while ( 0 != (++i%width) )
                    os << "   ";

                os << readable;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logLine::copy( const _Myt& rhs )
{
    level_ = rhs.level_;

    sourceFilename_ = rhs.sourceFilename_;
    sourceLine_ = rhs.sourceLine_;

    className_ = rhs.className_;
    classFunction_ = rhs.classFunction_;

    text_ = rhs.text_;

    stamp_ = rhs.stamp_;
    threadId_ = rhs.threadId_;
}
