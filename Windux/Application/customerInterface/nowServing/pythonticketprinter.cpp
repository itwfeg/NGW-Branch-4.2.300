/**
 * @file pythonticketprinter.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "pythonticketprinter.h"

#include <filesystem.h>
#include <python.h>

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////
pythonTicketPrinter::pythonTicketPrinter( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pythonTicketPrinter::~pythonTicketPrinter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString pythonTicketPrinter::errorString() const
{
    if ( 0 == error() )
        return tr( "No error." );

    return tr( "An unknown error occured while trying to print." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool pythonTicketPrinter::isReadyPrint() const
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool pythonTicketPrinter::isInstalled()
{
    const QString scriptName( filesystem::absoluteFilePath( "nowServing.py", folderType::Python ) );

    return QFile::exists( scriptName );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool pythonTicketPrinter::print( const QString& logoFilename, int customerNumber )
{
    bool result( false );

    python proc;

    // build param list
    QStringList params;
    params.append( QString::number( customerNumber ) );
    params.append( logoFilename );

    // execute script
    proc.execCommand( "nowServing", params );

    if ( 0 == proc.getExitCode() )
        result = true;
    else
    {
        // TODO: what error codes?
        setError( -1 );
    }

    proc.cleanup();

    return result;
}
