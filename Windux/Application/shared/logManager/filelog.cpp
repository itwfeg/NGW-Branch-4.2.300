/**
 * @file filelog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "filelog.h"

#include <QTextStream>
#include <QXmlStreamReader>

///////////////////////////////////////////////////////////////////////////////////////////////////
fileLog::fileLog( const QString& filename ) :
    file_( filename ),
    rolloverSize_( DEFAULT_ROLLOVER_SIZE )
{
    // rename any existing log
    rollover( true );

    // open log file
    open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileLog::~fileLog()
{
    // close log file
    file_.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileLog::_Myt *fileLog::create( QXmlStreamReader& xml )
{
    QString filename;
    unsigned int rolloverSizeMegs = 0;
    QString format;

    int depth = 0;
    QString node;

    while ( !xml.hasError() )
    {
        xml.readNext();

        if (( xml.isEndElement() ) && ( 0 > --depth ))
            break;
        else if (( xml.isStartElement() ) && ( 0 == depth++ ))
            node = xml.name().toString();
        else if (( xml.isCharacters() ) && ( 1 == depth ))
        {
            if ( "filename" == node )
                filename = xml.text().toString();
            else if ( "rolloversize" == node )
                rolloverSizeMegs = xml.text().toString().toInt();
            else if ( "format" == node )
                format = xml.text().toString();
        }
    }

    if ( !filename.length() )
        return NULL;

    fileLog *p = new fileLog( filename );

    if ( rolloverSizeMegs )
        p->setRolloverSize( rolloverSizeMegs * 1024 * 1024 );

    if ( format.length() )
        p->setLineFormat( format );

    return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileLog::_Myt *fileLog::create( const QString &filename )
{
    return new fileLog( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fileLog::writeString( const QString& line )
{
    // check for rollover
    rollover();

    // get output stream
    QTextStream os( &file_ );

    // write the line
    os << line;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool fileLog::open()
{
    return file_.open( QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fileLog::rollover( bool force )
{
    if (( force ) || ( rolloverSize_ <= file_.size() ))
    {
        bool isOpen = file_.isOpen();

        QString filename = file_.fileName();
        QString filenameOld = filename + ".old";

        // close current log
        if ( isOpen )
            file_.close();

        // remove old backup log
        if ( QFile::exists( filenameOld ) )
            QFile::remove( filenameOld );

        // backup existing log
        if ( QFile::exists( filename ) )
            QFile::rename( filename, filenameOld );

        // open log up again
        if ( isOpen )
            open();
    }
}
