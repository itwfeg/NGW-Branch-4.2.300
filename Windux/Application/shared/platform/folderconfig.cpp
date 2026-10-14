/**
 * @file folderconfig.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "filesystemconfig.h"
#include "folderconfig.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////
folderConfig::folderConfig() :
    _Mybase(),
    platform_( PLATFORM )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
folderConfig::~folderConfig()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString folderConfig::folder( folderType::type type, bool system ) const
{
    // root folder path
    QString rootFolderPath( parameters_[folderType::RootFolder + SYSTEM_PARAMETER_OFFSET] );

    if ( rootFolderPath.isEmpty() )
        rootFolderPath = rootFolder();

    int key = type;
    QDir d;

    // parameter number
    if ( system )
        key += SYSTEM_PARAMETER_OFFSET;

    // parameter
    QString value( parameters_[key] );

    // check for explicit path
    if ( value.length() )
    {
        // absolute path
        if ( isQualifiedFilename( value ) )
            d.setPath( value );

        // relative path
        else
            d.setPath( rootFolderPath  + "/" + value );

        return d.absolutePath();
    }

    // use platform configuration
    d.setPath( rootFolderPath );
    folderType::appendFolderName( d, type, system );

    return d.absolutePath();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void folderConfig::setFolder( const QString& value, folderType::type type, bool system )
{
    int key = type;

    // parameter number
    if ( system )
        key += SYSTEM_PARAMETER_OFFSET;

    // empty value
    if ( !value.length() )
    {
        parameters_[key] = value;
        return;
    }

    QDir d;

    // absolute path
    if ( isQualifiedFilename( value ) )
        d.setPath( value );

    // relative path
    else
        d.setPath( folder( folderType::RootFolder, system ) + "/" + value );

    parameters_[key] = d.absolutePath();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void folderConfig::loadFromEnvironment()
{
    // check environment variables
    QByteArray py = qgetenv( "PYTHON_SCRIPT_PATH" );

    if ( py.length() )
        setFolder( py, folderType::Python );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void folderConfig::loadFromFile( const QString& filename )
{
    QFile f( filename );

    // open file
    if ( f.open( QIODevice::ReadOnly ) )
    {
        QTextStream fs( &f );

        // process all lines in file
        while ( !fs.atEnd() )
        {
            QString line( fs.readLine().trimmed() );

            // validate line
            if (( line.length() ) && ( "#" != line.left( 1 ) ))
                if ( 1 == line.count( "=" ) )
                {
                    QStringList c( line.split( "=" ) );

                    // process parameter name and value
                    if ( 2 == c.size() )
                    {
                        QString name( c.at( 0 ).trimmed() );
                        QString value( c.at( 1 ).trimmed() );

                        // process!
                        checkParameter( name, value );
                    }
                }
        }

        // close file
        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool folderConfig::isQualifiedFilename( const QString& filename )
{
    if ( filename.contains( ":" ) )
        return true;

    if ( filename.length() )
        if (( '\\' == filename[0] ) || ( '/' == filename[0] ))
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void folderConfig::checkParameter( const QString& name, const QString& value )
{
    // Root
    if ( "systemUserRootDir" == name )
        setFolder( value, folderType::RootFolder );

    // Fonts
    else if ( "userFontsDir" == name )
        setFolder( value, folderType::Fonts );
    else if ( "systemFontsDir" == name )
        setFolder( value, folderType::Fonts, true );

    // Databases
    else if ( "db" == name )
        setFolder( value, folderType::DBs );

    // Videos
    else if ( "videosDir" == name )
        setFolder( value, folderType::Videos );

    // Upgrades
    else if ( "updateDir" == name)
        setFolder( value, folderType::Upgrades);

    // CategoriesImages
    else if ( "categoryGraphicsDir" == name )
        setFolder( value, folderType::CategoriesImages );
    else if ( "systemCategoryGraphicsDir" == name )
        setFolder( value, folderType::CategoriesImages, true );

    // FlashkeyImages
    else if ( "softkeyGraphicsDir" == name )
        setFolder( value, folderType::FlashkeyImages );
    else if ( "reservedSoftkeyGraphicsDir" == name )
        setFolder( value, folderType::FlashkeyImages, true );

    // LabelImages
    else if ( "systemGraphicsLocation" == name )
        setFolder( value, folderType::LabelImages, true );
    else if ( "userGraphicsLocation" == name )
        setFolder( value, folderType::LabelImages );

    // PlanogramImages
    else if ( "planogramGraphicsDir" == name )
        setFolder( value, folderType::PlanogramImages );

    // ProductsImages
    else if ( "systemProductGraphicsDir" == name )
        setFolder( value, folderType::ProductsImages, true );
    else if ( "productsGraphicsDir" == name )
        setFolder( value, folderType::ProductsImages );

    // SelfServeImages
    else if ( "systemSelfServeGraphicsDir" == name )
        setFolder( value, folderType::SelfServeImages, true );
    else if ( "selfServeGraphicsDir" == name )
        setFolder( value, folderType::SelfServeImages );

    // SlideshowImages
    else if ( "streamingGraphicsDir" == name )
        setFolder( value, folderType::SlideshowImages );

    // SpecialsImages
    else if ( "specialsGraphicsDir" == name )
        setFolder( value, folderType::SpecialsImages );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString folderConfig::configFileName() const
{
    switch ( platform() )
    {
    case platformType::Windows:
        return filesystemConfig<platformType::Windows>::configFileName();
    case platformType::Linux:
        return filesystemConfig<platformType::Linux>::configFileName();
    case platformType::EmbeddedLinux:
        return filesystemConfig<platformType::EmbeddedLinux>::configFileName();
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString folderConfig::rootFolder() const
{
    switch ( platform() )
    {
    case platformType::Windows:
        return filesystemConfig<platformType::Windows>::rootFolder();
    case platformType::Linux:
        return filesystemConfig<platformType::Linux>::rootFolder();
    case platformType::EmbeddedLinux:
        return filesystemConfig<platformType::EmbeddedLinux>::rootFolder();
    }

    return QString();
}
