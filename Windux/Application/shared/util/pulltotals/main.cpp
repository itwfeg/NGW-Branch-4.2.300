
#include "session.h"

#include <QApplication>
#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QHostAddress>
#include <QInputDialog>

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList convertTagToCommaSeperated( const session::tagMapList& results )
{
    bool first;

    // generate list of unique keys
    QStringList keys;

    foreach ( const session::tagMap& result, results )
        keys.append( result.keys() );

    keys.removeDuplicates();
    keys.sort();

    // generate header
    QString header;

    first = true;

    foreach ( const QString& key, keys )
    {
        if ( !first )
            header.append( "," );

        header.append( key );
        first = false;
    }

    // generate csv lines
    QStringList csv;
    csv.append( header );

    foreach ( const session::tagMap& result, results )
    {
        QString record;

        first = true;

        foreach ( const QString& key, keys )
        {
            if ( !first )
                record.append( "," );

            if ( result.contains( key ) )
            {
                const QString value( result[key] );
                const bool hasComma( value.contains( "," ) );

                if ( hasComma )
                    record.append( "\"" );

                record.append( value );

                if ( hasComma )
                    record.append( "\"" );
            }

            first = false;
        }

        csv.append( record );
    }

    return csv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    // prompt for connection details
    bool ok;

    const QString host =
        QInputDialog::getText(
            nullptr,
            QObject::tr( "Enter Host Address" ),
            QObject::tr( "Please enter the hostname or ip address of the machine you wish to connect to." ),
            QLineEdit::Normal,
            QString(),
            &ok );

    if (( !ok ) || ( host.isEmpty() ))
        return -1;

    // connect
    const session::sessionType t( session::PULL_AND_CLEAR_TOTALS );
//    const session::sessionType t( session::PULL_EXPIRATION_LABELS );

    session s( t );

    if ( !s.connectToHost( QHostAddress( host ), 6000 ) )
        return -1;

    // wait for completion
    QEventLoop loop;
    QObject::connect( &s, &session::complete, &loop, &QEventLoop::quit );

    loop.exec();

    // generate csv
    const QStringList formatted( convertTagToCommaSeperated( s.results() ) );

    // write to file
    const QDateTime now( QDateTime::currentDateTime() );

    QFile f( now.toString( "yyyyMMdd_HHmmss" ) + ".csv" );

    if ( f.open( QFile::ReadWrite | QFile::Text ) )
    {
        foreach ( const QString& csv, formatted )
        {
            f.write( qPrintable( csv ) );
            f.write( "\n" );
        }

        f.close();
    }

    return 0;
}
