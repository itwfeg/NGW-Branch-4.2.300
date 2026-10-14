/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include <QTemporaryFile>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    QTextStream os( stdout );

    QString ip( "127.0.0.1" );
    quint16 port( 6000 );

    QStringList files;
    QString output;

    QTemporaryFile backup;

    // iterate arguments
    for ( int arg( 1 ); arg < argc; ++arg )
    {
        const QString param( argv[arg] );

        // ip address
        if (( "-ip" == param ) && ( ++arg < argc ))
        {
            ip = argv[arg];
        }

        // port
        else if (( "-port" == param ) && ( ++arg < argc ))
        {
            const QString portStr( argv[arg] );

            port = portStr.toUShort();
        }

        // output file
        else if (( "-out" == param ) && ( ++arg < argc ))
        {
            output = argv[arg];;
        }

        else if ( "-backup" == param )
        {
            if ( backup.open() )
            {
                backup.write( "RT75\x1e" ); // EVOL_TAG_READ_DEPT_NAME

                backup.write( "RT53\x1e" ); // EVOL_TAG_READ_PRODUCT
                backup.write( "RT64\x1e" ); // EVOL_TAG_READ_EXP_TEXT1
                backup.write( "RT5U\x1e" ); // EVOL_TAG_READ_EXP_TEXT2
                backup.write( "RT66\x1e" ); // EVOL_TAG_READ_SPECIAL_MSGS
                backup.write( "RT6K\x1e" ); // EVOL_TAG_READ_PRODUCT_NOTES
                backup.write( "RT59\x1e" ); // EVOL_TAG_READ_MARQUEES
                backup.write( "RT5S\x1e" ); // EVOL_TAG_READ_COOKING_INST
                backup.write( "RT5T\x1e" ); // EVOL_TAG_READ_RECIPE
                backup.write( "RT5V\x1e" ); // EVOL_TAG_READ_SUGGEST_SELL
                backup.write( "RT5W\x1e" ); // EVOL_TAG_READ_ALLERGENS
                backup.write( "RT59\x1e" ); // EVOL_TAG_READ_MARQUEES
                backup.write( "RT59\x1e" ); // EVOL_TAG_READ_MARQUEES

                backup.write( "RT62\x1e" ); // EVOL_TAG_READ_OPERATOR
                backup.write( "RT6L\x1e" ); // EVOL_TAG_READ_OPERATOR_NOTES

                backup.write( "RT6M\x1e" ); // EVOL_TAG_READ_CATEGORY

                backup.write( "RT6R\x1e" ); // EVOL_TAG_READ_ROM_GRAPHICS
                backup.write( "RT6S\x1e" ); // EVOL_TAG_READ_PRODUCT_GRAPHICS
                backup.write( "RT6T\x1e" ); // EVOL_TAG_READ_CATEGORY_GRAPHICS
                backup.write( "RT6U\x1e" ); // EVOL_TAG_READ_SPECIALS_GRAPHICS
                backup.write( "RT6V\x1e" ); // EVOL_TAG_READ_STREAMING_GRAPHICS
                backup.write( "RT6W\x1e" ); // EVOL_TAG_READ_USER_GRAPHICS
                backup.write( "RT6Z\x1e" ); // EVOL_TAG_READ_SOFTKEY_GRAPHICS
                backup.write( "RT6H\x1e" ); // EVOL_TAG_READ_PLANOGRAM_GRAPHICS

                backup.close();

                files.append( backup.fileName() );
            }
        }

        // file
        else
        {
            files.append( param );
        }
    }

    if ( files.isEmpty() )
    {
        os << "Please specify one (or more) files to process.";
        return 0;
    }

    foreach ( const QString& f, files )
        if ( !QFile::exists( f ) )
        {
            os << "One or more file(s) do not exist " << f;
            return 0;
        }

    // ---- //

    QTcpSocket s;

    // connect!
    s.connectToHost( ip, port );

    if ( !s.waitForConnected() )
    {
        os << "Failed to connect! " << ip << " " << port;
        return 0;
    }

    // send each file
    foreach ( const QString& f, files )
    {
        QFile is( f );

        if ( !is.open( QFile::ReadOnly ) )
        {
            os << "Failed to open file for reading! " << f;
            return 0;
        }

        // break file into chunks and send
        for ( ;; )
        {
            static const qint64 BUFFER_SIZE = 32 * 1024;

            const QByteArray a( is.read( BUFFER_SIZE ) );

            if ( !a.size() )
                break;

            s.write( a );

            if ( a.size() < BUFFER_SIZE )
                break;
        }

        is.close();

        s.flush();
    }

    // print what we receive
    while ( s.waitForReadyRead() )
    {
        const QByteArray a( s.readAll() );

        if ( output.length() )
        {
            QFile f( output );

            if ( f.open( QFile::WriteOnly | QFile::Append ) )
            {
                f.write( a );
                f.close();
            }
        }

        os << a;
    }

    // disconnect
    s.disconnectFromHost();

    return 0;
}
