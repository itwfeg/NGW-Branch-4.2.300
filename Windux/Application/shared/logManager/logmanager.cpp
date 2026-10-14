/**
 * @file logmanager.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "logmanager.h"

#include "filelog.h"
#include "qtlog.h"
#include "consolelog.h"

#include <QXmlStreamReader>

QMutex logManager::mutex_;
QScopedPointer<logManager> logManager::instance_;

static const QString DEFAULT_LOG_NAME( "default" );

///////////////////////////////////////////////////////////////////////////////////////////////////
logManager::logManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logManager::logManager( const QString& filename, create_from_config_t )
{
    QFile file( filename );

    if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QXmlStreamReader xml( &file );

        // look for our starting element
        while (( !xml.atEnd() ) && ( !xml.hasError() ))
        {
            xml.readNext();

            if ( xml.isStartElement() )
            {
                QString name = xml.name().toString();

                // found it... parse!
                if ( "logconfig" == name )
                {
                    parseLogConfig( xml );
                    break;
                }
            }
        }

        file.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logManager::logManager( const QString& filename, create_root_file_appender_t )
{
    logs_.insert( filename, logMap::mapped_type( fileLog::create( filename ) ) );
    root_.push_back( logs_[ filename ] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logManager::~logManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::initialize( const QString& filename, create_from_config_t )
{
    if ( !instance_ )
    {
        QMutexLocker lock( &mutex_ );

        if ( !instance_ )
            instance_.reset( new logManager( filename, create_from_config ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::initialize( const QString& filename, create_root_file_appender_t )
{
    if ( !instance_ )
    {
        QMutexLocker lock( &mutex_ );

        if ( !instance_ )
            instance_.reset( new logManager( filename, create_root_file_appender ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::finalize()
{
    QMutexLocker lock( &mutex_ );

    if ( instance_ )
        instance_.reset( NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logAppender& logManager::appender( const QString& name )
{
    // check if we know about this appender, if so return it
    if ( appenders_.contains( name ) )
        return appenders_[name];

    // unknown appender so return root
    return root_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logManager *logManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker lock( &mutex_ );

        if ( !instance_ )
            instance_.reset( new logManager );
    }

    return instance_.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseLogConfig( QXmlStreamReader& xml )
{
    int depth = 0;

    while ( !xml.hasError() )
    {
        xml.readNext();

        if (( xml.isEndElement() ) && ( 0 > --depth ))
            break;
        else if (( xml.isStartElement() ) && ( 0 == depth ))
        {
            QString node = xml.name().toString();

            if ( "logs" == node )
                parseLogs( xml );
            else if ( "root" == node )
                parseRootNode( xml );
            else if ( "appenders" == node )
                parseAppenders( xml );
            else
                ++depth;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseLogs( QXmlStreamReader& xml )
{
    int depth = 0;

    while ( !xml.hasError() )
    {
        xml.readNext();

        if (( xml.isEndElement() ) && ( 0 > --depth ))
            break;
        else if (( xml.isStartElement() ) && ( 0 == depth ))
        {
            QString node = xml.name().toString();

            if ( "log" == node )
            {
                parseLog( xml,
                    xml.attributes().value( "name" ).toString(),
                    xml.attributes().value( "type" ).toString() );
            }
            else
                ++depth;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseLog( QXmlStreamReader& xml, const QString& name, const QString& type )
{
    if (( !name.length() ) || ( !type.length() ))
        return;

    // create log object of appropriate type
    logger *p = NULL;

    if ( "file" == type )
        p = fileLog::create( xml );
    else if ( "qt" == type )
        p = qtLog::create( xml );
    else if ( "console" == type )
        p = consoleLog::create( xml );

    // unknown type so transverse xml node
    else
    {
        int depth = 0;

        while ( !xml.hasError() )
        {
            xml.readNext();

            if (( xml.isEndElement() ) && ( 0 > --depth ))
                break;
            else if ( xml.isStartElement() )
                ++depth;
        }
    }

    // add log to list
    if ( p )
        logs_.insert( name, logMap::mapped_type( p ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseRootNode( QXmlStreamReader& xml )
{
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
            if ( "level" == node )
                root_.setLevel( logLevel::fromString( xml.text().toString() ) );
            else if ( "log-ref" == node )
            {
                QString logName = xml.text().toString();

                if ( logs_.contains( logName ) )
                    root_.push_back( logs_[ logName ] );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseAppenders( QXmlStreamReader& xml )
{
    int depth = 0;

    while ( !xml.hasError() )
    {
        xml.readNext();

        if (( xml.isEndElement() ) && ( 0 > --depth ))
            break;
        else if (( xml.isStartElement() ) && ( 0 == depth ))
        {
            QString node = xml.name().toString();

            if ( "appender" == node )
            {
                parseAppender( xml,
                    xml.attributes().value( "name" ).toString() );
            }
            else
                ++depth;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logManager::parseAppender( QXmlStreamReader& xml, const QString& name )
{
    if ( !name.length() )
        return;

    logAppender appender( name );

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
            if ( "level" == node )
                appender.setLevel( logLevel::fromString( xml.text().toString() ) );
            else if ( "log-ref" == node )
            {
                QString logName = xml.text().toString();

                if ( logs_.contains( logName ) )
                    appender.push_back( logs_[ logName ] );
            }
        }
    }

    appenders_.insert( name, appender );
}
