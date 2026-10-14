/**
 * @file qtlog.cpp
 * Qt based logger.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "qtlog.h"

#include <QDebug>
#include <QXmlStreamReader>

///////////////////////////////////////////////////////////////////////////////////////////////////
void qtLog::write( const logLine& line )
{
    if ( logLevel::Error == line.level() )
        qCritical() << formatted( line );
    else if ( logLevel::Warning == line.level() )
        qWarning() << formatted( line );
    else
        qDebug() << formatted( line );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qtLog::_Myt *qtLog::create( QXmlStreamReader& xml )
{
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
            if ( "format" == node )
                format = xml.text().toString();
        }
    }

    qtLog *p = new qtLog;

    if ( format.length() )
        p->setLineFormat( format );

    return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void qtLog::writeString( const QString& data )
{
    qDebug() << data;
}
