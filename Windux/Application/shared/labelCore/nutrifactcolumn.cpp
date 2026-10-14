/**
 * @file nutrifactcolumn.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "nutrifactcolumn.h"

#include <QDataStream>

const qint8 nutrifactColumn::SUB = 0x1A;

///////////////////////////////////////////////////////////////////////////
nutrifactColumn::nutrifactColumn() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////
nutrifactColumn::nutrifactColumn( const QString& data ) :
    _Mybase()
{
    setData( data );
}

///////////////////////////////////////////////////////////////////////////
nutrifactColumn::nutrifactColumn( const _Myt& rhs ) :
    _Mybase( rhs )
{
}

///////////////////////////////////////////////////////////////////////////
nutrifactColumn::~nutrifactColumn()
{
}

///////////////////////////////////////////////////////////////////////////
QString nutrifactColumn::data() const
{
    LOG_CLASS_INFO( "nutrifactColumn", "data" );

    QByteArray a;
    QDataStream os( &a, QIODevice::WriteOnly );

    bool first = true;

    LOG_TRACE( "getting nutrifact column data...", NULL );

    // write each item into the stream
    for ( const_iterator i = begin(); i != end(); ++i )
    {
        if ( first )
            first = false;
        else
            os << SUB;

        LOG_DEBUG( "adding element ['%s', '%s']",
            i.key().toLatin1().constData(),
            i.value().toLatin1().constData() );

        os.writeRawData( i.key().toLatin1().constData(), i.key().length() );
        os.writeRawData( i.value().toLatin1().constData(), i.value().length() );
    }

    LOG_DEBUG( "have column data '%s'", a.constData() );
    return a;
}

///////////////////////////////////////////////////////////////////////////
void nutrifactColumn::setData( const QString& value )
{
    LOG_CLASS_INFO( "nutrifactColumn", "setData" );

    QByteArray a( value.toLatin1() );
    QDataStream is( a );

    LOG_TRACE( "parsing nutrifacts column...", NULL );

    // clear all current data
    clear();

    QString data;

    while ( !is.atEnd() )
    {
        qint8 c;
        is >> c;

        bool done = is.atEnd();
        bool process = (( SUB == c ) || ( done ));

        // at end of stream, but no seperator
        if (( done ) && ( SUB != c ))
            data += c;

        // process this data
        if ( !process )
            data += c;
        else
        {
            if ( 4 > data.length() )
                LOG_WARN( "bad nutrifact data '%s'", data.toLatin1().constData() );
            else
            {
                QString tag = data.left( 4 );
                QString element = data.right( data.length() - 4 );

                LOG_DEBUG( "inserting element ['%s', '%s']", tag.toLatin1().constData(), element.toLatin1().constData() );
                insert( tag, element );
            }

            data.clear();
        }

        // we are done
        if ( done )
            break;
    }

    LOG_DEBUG( "have %d nutrifacts", size() );
}
