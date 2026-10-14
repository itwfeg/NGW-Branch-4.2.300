/**
 * @file callbackrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "callbackrequest.h"
#include "common.h"
#include "stringsjson.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

///////////////////////////////////////////////////////////////////////////////////////////////////
callbackRequest::callbackRequest( const QString& uuid, const QString& eventType, EventResult result, const QString& errorText, QObject *parent ) :
    _Mybase( parent ),
    uuid_( uuid ),
    eventType_( eventType ),
    success_( SUCCESS == result ),
    backoutAttempted_(( FAILURE_BACKOUT_SUCCESS == result ) || ( FAILURE_BACKOUT_FAILED == result )),
    backout_( FAILURE_BACKOUT_SUCCESS == result ),
    errors_( errorText )
{
    // as per Walmart, do not send logs when success
    if ( !success_ )
        collectLogs();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
callbackRequest::~callbackRequest()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument callbackRequest::toJsonDoc() const
{
    QJsonObject obj( request_ );
    obj[JSON_CALLBACK] = callback();

    return QJsonDocument( obj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject callbackRequest::callback() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    QJsonObject output;

    if ( logs_.isEmpty() )
        output[JSON_LOG_TEXT] = nullVal;
    else
        output[JSON_LOG_TEXT] = logs_;

    if ( errors_.isEmpty() )
        output[JSON_ERROR_TEXT] = nullVal;
    else
        output[JSON_ERROR_TEXT] = errors_;

    QJsonObject obj;
    obj[JSON_CALLBACK_UUID] = uuid_;
    obj[JSON_CALLBACK_EVENT_TYPE] = eventType_;
    obj[JSON_IS_EVENT_SUCCESS] = success_;

    if ( backoutAttempted_ )
        obj[JSON_IS_BACKOUT_SUCCESS] = backout_;
    else
        obj[JSON_IS_BACKOUT_SUCCESS] = nullVal;

    obj[JSON_OUTPUT] = output;

    return obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void callbackRequest::collectLogs()
{
#if defined(Q_OS_LINUX)

    QStringList logs;
    logs.append( APP_LOG );
    logs.append( DIALOGMGR_LOG );
    logs.append( COMM_LOG );

    foreach ( const QString& log, logs )
    {
        QFile f( log );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_DEBUG << "error opening file " << qPrintable( f.fileName() );
        else
        {
            logs_.append( f.fileName() + ":\n" );

            // seek end of file
            f.seek( f.size() - 1 );

            int count( 0 );

            // seek backward to num lines
            while (( count < NUM_LOG_LINES ) && ( 0 < f.pos() ))
            {
                const QString ch( f.read( 1 ) );

                f.seek( f.pos() - 2 );

                if ( "\n" == ch )
                    ++count;
            }

            // append log
            logs_.append( f.readAll() );
            logs_.append( "\n" );

            f.close();
        }
    }

#elif defined(Q_OS_WIN32)
    // not supported

#endif
}
