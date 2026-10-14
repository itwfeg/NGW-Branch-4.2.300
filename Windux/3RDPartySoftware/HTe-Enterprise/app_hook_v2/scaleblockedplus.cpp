/**
 * @file scaleblockedplus.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleblockedplus.h"
#include "stringsjson.h"

#include <QFile>
#include <QJsonArray>
#include <QRegExp>
#include <QTextStream>

#if defined(Q_OS_LINUX)
const QString WHITELIST_FILE( "/usr/local/hobart/apps/remote_print/plu_whitelist.csv" );
#elif defined(Q_OS_WIN32)
const QString WHITELIST_FILE( "plu_whitelist.csv" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleBlockedProducts::scaleBlockedProducts( QObject *parent ) :
    _Mybase( parent )
{
    blocked_ = blockedProducts();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleBlockedProducts::~scaleBlockedProducts()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleBlockedProducts::addBlockedProduct( int value )
{
    LOG_DEBUG << "adding " << value << " to blocked products";

    if ( !blocked_.contains( value ) )
        blocked_.append( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleBlockedProducts::removeBlockedProduct( int value )
{
    LOG_DEBUG << "remove " << value << " from blocked products";

    if ( blocked_.contains( value ) )
        blocked_.removeOne( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleBlockedProducts::saveFile() const
{
    QFile f( WHITELIST_FILE );

    if ( !f.open( QFile::WriteOnly | QFile::Text ) )
    {
        LOG_WARN << "error opening whitelist " << qPrintable( f.fileName() );
        return false;
    }

    QTextStream ts( &f );

    foreach ( const int& plu, blocked_ )
        ts << plu << "\n";

    f.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonArray scaleBlockedProducts::toJsonArray() const
{
    QJsonArray a;

    foreach ( const int& plu, blocked_ )
        a.append( plu );

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleBlockedProducts::integerList scaleBlockedProducts::blockedProducts()
{
    integerList result;

    if ( !QFile::exists( WHITELIST_FILE ) )
        LOG_WARN << "could not find plu whitelist " << qPrintable( WHITELIST_FILE );
    else
    {
        QFile f( WHITELIST_FILE );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "error opening whitelist " << qPrintable( f.fileName() );
        else
        {
            QTextStream ts( &f );

            while ( !ts.atEnd() )
            {
                const QString line( ts.readLine() );

                if ( line.isEmpty() )
                    continue;
                else if (( line.startsWith( "#" ) ) || ( line.startsWith( "/" ) ))
                    continue;
                else
                {
                    QRegExp re( "\\d*" );           // a digit (\d), zero or more times (*)

                    if ( re.exactMatch( line ) )
                        result.append( line.toInt() );
                    else
                    {
                        LOG_TRACE << "bad data " << qPrintable( line );
                    }
                }
            }

            f.close();
        }
    }

    LOG_DEBUG << "have " << result.size() << " blocked plu";
    return result;
}
