/**
 * @file appinit.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "appinit.h"
#include "logmanager.h"
#include "platform.h"
#include "filesystem.h"

#include <QtGlobal>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStringList>

#define LOG_FILESYSTEM_PATH( desc, sys, type ) \
    LOG_INFO( "%s [%s]: '%s'", desc, (sys ? "system" : "user"), qPrintable( filesystem::folder( type, sys ) ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
void appInitialize( const QString& appName )
{
    LOG_CLASS_INFO( "", "appInitialize" );

    // set application name for Phonon DBUS Interface
    qApp->setApplicationName( appName );

    QStringList appArguments( qApp->arguments() );

    // ----------------------------------------------------
    // initialize logging
    // ----------------------------------------------------

    QString configFile( filesystem::absoluteFilePath( appName + ".config", folderType::RootFolder ) );

    // check for config file existance
    if ( QFile::exists( configFile ) )
        logManager::initialize( configFile, create_from_config );

    // create simple logger
    else
    {
        QString file( appName + ".txt" );
        logLevel::type level( logLevel::Warning );

        if ( appArguments.contains( "-nologging" ) )
            level = logLevel::Disabled;
        else if ( appArguments.contains( "-error" ) )
            level = logLevel::Error;
        else if ( appArguments.contains( "-warn" ) )
            level = logLevel::Warning;
        else if ( appArguments.contains( "-info" ) )
            level = logLevel::Info;
        else if ( appArguments.contains( "-debug" ) )
            level = logLevel::Debug;
        else if ( appArguments.contains( "-trace" ) )
            level = logLevel::Trace;

        logManager::initialize( file, create_root_file_appender );
        logManager::instance()->rootAppender().setLevel( level );
    }

    // ----------------------------------------------------
    // log paths
    // ----------------------------------------------------

    for ( int i = 0; i < 2; ++i )
    {
        bool sys( 0 < i );

        LOG_FILESYSTEM_PATH( "root", sys, folderType::RootFolder );

        LOG_FILESYSTEM_PATH( "audio path", sys, folderType::Audio );
        LOG_FILESYSTEM_PATH( "documents path", sys, folderType::Documents );
        LOG_FILESYSTEM_PATH( "fonts path", sys, folderType::Fonts );
        LOG_FILESYSTEM_PATH( "html path", sys, folderType::Html );
        LOG_FILESYSTEM_PATH( "images path", sys, folderType::Images );
        LOG_FILESYSTEM_PATH( "python path", sys, folderType::Python );
        LOG_FILESYSTEM_PATH( "videos path", sys, folderType::Videos );

        LOG_FILESYSTEM_PATH( "categories images", sys, folderType::CategoriesImages );
        LOG_FILESYSTEM_PATH( "flashkeys images", sys, folderType::FlashkeyImages );
        LOG_FILESYSTEM_PATH( "graphics images", sys, folderType::LabelImages );
        LOG_FILESYSTEM_PATH( "self serve images", sys, folderType::SelfServeImages );
        LOG_FILESYSTEM_PATH( "slideshow images", sys, folderType::SlideshowImages );
        LOG_FILESYSTEM_PATH( "specials images", sys, folderType::SpecialsImages );
    }

    // ----------------------------------------------------
    // log out app argument list
    // ----------------------------------------------------

    LOG_INFO( "application arguments (%d):", appArguments.size() );

    foreach ( const QString& value, appArguments )
        LOG_INFO( "['%s']", qPrintable( value ) );

    // ----------------------------------------------------
    // library paths
    // ----------------------------------------------------

#ifdef Q_OS_WIN
    // append qt plugin library path
    QDir plugins( filesystem::folder( folderType::RootFolder ) );
    plugins.cd( "qt_plugins" );

    qApp->addLibraryPath( plugins.absolutePath() );
#endif

    QStringList libraryPaths( qApp->libraryPaths() );

    foreach ( const QString& p, libraryPaths )
        LOG_INFO( "library path '%s'", qPrintable( p ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void appFinalize()
{
    // ----------------------------------------------------
    // finalize logging
    // ----------------------------------------------------

    logManager::finalize();
}
