/**
 * @file mediaplaylistfileparser.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "mediaplaylistfileparser.h"

#include <filesystem.h>
#include <python.h>

#include <QFile>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaPlaylistFileParserSection::addToPlaylist( mediaPlaylist& playlist, QStringList& missing ) const
{
    LOG_CLASS_INFO( "mediaPlaylistFileParserSection", "addToPlaylist" );

    // Note:
    // The SRD states that missing/corrupt images should use a "default image" placeholder in
    // their stead. If this requirement changes and you decide to just drop the missing images,
    // set this boolean to true.
    bool dropMissingFiles = false;

    // Note:
    // The SRD makes no mention on what to do for missing background audio files.  The current
    // behavior is to log out a warning and ignore the audio. If you wish to drop the offending
    // slide instead you may set this boolean to true.
    bool dropMissingBackgroundAudio = false;

    // handle each file in list
    foreach ( const QString& name, *this )
    {
        bool checkFileExists( true );

        // media filename
        QString filename( name );

        // path out media filename
        switch ( mediaType::get( name ) )
        {
        case mediaType::Image:
            filename = filesystem::absoluteFilePath( name, folderType::Images );
            break;

        case mediaType::Video:
            filename = filesystem::absoluteFilePath( name, folderType::Videos );

#ifdef DISABLE_VIDEO
            LOG_WARN( "media file '%s' not allowed (videos disabled)!", qPrintable( filename ) );
            continue;
#else
            // check if video allowed
            if ( filename.length() )
            {
                python p;

                // get scale name
                if (( p.execCommand( "getModelName" ) ) && ( 0 == p.getExitCode() ))
                {
                    QString result( p.readStdOut() );

                    // HTs
                    // this feature is disabled
                    if ( result.contains( "hts", Qt::CaseInsensitive ) )
                    {
                        LOG_WARN( "media file '%s' not allowed!", qPrintable( filename ) );
                        continue;
                    }
                }
            }
#endif
            break;

        case mediaType::Web:
#ifdef DISABLE_WEB
            LOG_WARN( "media file '%s' not allowed (web disabled)!", qPrintable( filename ) );
            continue;
#else
            checkFileExists = false;
#endif
            break;

        default:
            break;
        }

        // media object
        media mediaItem( filename, delay_ * 1000 );

        // check media filename
        if (( checkFileExists ) && ( !QFile::exists( filename ) ))
        {
            LOG_WARN( "media file '%s' does not exist!", qPrintable( filename ) );

            // FT-761
            missing.append( filename );

            // just drop this missing file from the playback list
            if ( dropMissingFiles )
                continue;

            // FT-1164 [Begin]
            // instead of handling missing images when we create the playlist (by inserting placeholder) we
            // are now handling it as we process the playlist... commenting this code...

            // setup a default playback image
            //mediaItem = media( missingMediaPlaceholderFilename(), DEFAULT_DELAY_MS );
            // FT-1164 [End]
        }

        // check audio filename
        if ( audio_.length() )
        {
            QString audioFilename( filesystem::absoluteFilePath( audio_, folderType::Audio ) );

            if ( QFile::exists( audioFilename ) )
                mediaItem.setSoundFilename( audioFilename );
            else
            {
                LOG_WARN( "audio file '%s' does not exist!", qPrintable( audioFilename ) );

                // FT-761
                missing.append( audioFilename );

                // just drop this media from the playlist
                if ( dropMissingBackgroundAudio )
                    continue;
            }
        }

        // set the overlay
        mediaItem.setOverlay( overlay_ );

        // set the animation
        mediaItem.setAnimation( (animationType::type) animation_ );

        LOG_INFO( "adding media file '%s' with delay %d ms to playlist (text '%s', audio '%s')",
            qPrintable( mediaItem.filename() ),
            mediaItem.duration(),
            qPrintable( mediaItem.overlay().text() ),
            qPrintable( mediaItem.soundFilename() ) );

        // add to playlist
        playlist.push_back( mediaItem );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaPlaylistFileParserSection::clear()
{
    _Mybase::clear();

    delay_ = DEFAULT_DELAY_MS;
    overlay_ = textOverlay();
    audio_ = "";
    animation_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaPlaylistFileParserSection::copy( const _Myt& rhs )
{
    // empty list
    // do _not_ call clear() here, results in segment fault! more mingw32 workarounds...
    while ( !empty() )
        pop_front();

    // copy list
    // do _not_ use a "foreach" loop here, results in segment fault! more mingw32 workarounds...
    for ( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
        push_back( *i );

    // copy variables
    delay_ = rhs.delay_;
    overlay_ = rhs.overlay_;
    audio_ = rhs.audio_;
    animation_ = rhs.animation_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaPlaylistFileParser::mediaPlaylistFileParser()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaPlaylistFileParser::parse( const QString& filename, mediaPlaylist& playlist, QStringList& missing  )
{
    LOG_CLASS_INFO( "mediaPlaylistFileParser", "parse" );

    QFile file( filename );

    LOG_INFO( "opening file '%s'", qPrintable( filename ) );

    // open playlist file
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        LOG_ERROR( "error opening file '%s'!", qPrintable( filename ) );
        return false;
    }

    mediaPlaylistFileParser parser;
    QTextStream is( &file );

    LOG_DEBUG( "parsing file...", NULL );

    // parse each line of playlist file
    while ( !is.atEnd() )
    {
        QString line = is.readLine().trimmed();

        // valid line to parse
        if ( line.length() )
        {
            LOG_TRACE( "parsing line '%s'...", qPrintable( line ) );
            parser.parseLine( line );
        }
    }

    // add current section to map
    parser.insertSection( "" );

    // finalize parser
    playlist = parser.finalize( missing );

    LOG_INFO( "playlist has %d items", playlist.size() );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaPlaylistFileParser::parseLine( const QString& line )
{
    LOG_CLASS_INFO( "mediaPlaylistFileParser", "parseLine" );

    // tag
    // add current section to map, create new section...
    if ( '[' == line[0] )
    {
        LOG_TRACE( "have tag", NULL );

        if (( line.length() >= 4 ) && ( "]" == line.right( 1 )))
            insertSection( line.mid( 1, line.length() - 2 ) );
    }

    // filename
    // push it it to list of lines
#ifdef Q_OS_LINUX
    else if (( line[0].isLetterOrNumber() ) || ( '/' == line[0] ))
#else
    else if ( line[0].isLetterOrNumber() )
#endif
    {
        QString value;

        LOG_TRACE( "have filename", NULL );

        if ( checkForParameter( line, "delay", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting delay to %d", temp );
            current_.setDelay( temp );
        }
        else if ( checkForParameter( line, "text", value ) )
        {
            LOG_DEBUG( "setting text to '%s'", qPrintable( value ) );

            textOverlay overlay = current_.overlay();
            overlay.setText( value.replace( "\\n", QChar( QChar::LineSeparator ) ) );

            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "audio", value ) )
        {
            LOG_DEBUG( "setting audio to '%s'", qPrintable( value ) );
            current_.setAudio( value );
        }
        else if ( checkForParameter( line, "texthalign", value ) )
        {
            LOG_DEBUG( "setting text halign to '%s'", qPrintable( value ) );

            textOverlay overlay = current_.overlay();
            Qt::Alignment overlayAlignment = overlay.alignment();

            if ( 0 == value.compare( "left", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignVertical_Mask;
                overlayAlignment |= Qt::AlignLeft;
            }
            else if ( 0 == value.compare( "center", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignVertical_Mask;
                overlayAlignment |= Qt::AlignHCenter;
            }
            else if ( 0 == value.compare( "right", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignVertical_Mask;
                overlayAlignment |= Qt::AlignRight;
            }
            else
                LOG_WARN( "unhandled horizontal align '%s'!", qPrintable( value ) );

            overlay.setAlignment( overlayAlignment );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textvalign", value ) )
        {
            LOG_DEBUG( "setting text valign to '%s'", qPrintable( value ) );

            textOverlay overlay = current_.overlay();
            Qt::Alignment overlayAlignment = overlay.alignment();

            if ( 0 == value.compare( "top", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignHorizontal_Mask;
                overlayAlignment |= Qt::AlignTop;
            }
            else if ( 0 == value.compare( "center", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignHorizontal_Mask;
                overlayAlignment |= Qt::AlignVCenter;
            }
            else if ( 0 == value.compare( "bottom", Qt::CaseInsensitive ) )
            {
                overlayAlignment &= Qt::AlignHorizontal_Mask;
                overlayAlignment |= Qt::AlignBottom;
            }
            else
                LOG_WARN( "unhandled vertical align '%s'!", qPrintable( value ) );

            overlay.setAlignment( overlayAlignment );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textcolorr", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting text red to %d", temp );

            textOverlay overlay = current_.overlay();
            QColor overlayColor = overlay.color();

            overlayColor.setRed( temp );

            overlay.setColor( overlayColor );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textcolorg", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting text green to %d", temp );

            textOverlay overlay = current_.overlay();
            QColor overlayColor = overlay.color();

            overlayColor.setGreen( temp );

            overlay.setColor( overlayColor );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textcolorb", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting text blue to %d", temp );

            textOverlay overlay = current_.overlay();
            QColor overlayColor = overlay.color();

            overlayColor.setBlue( temp );

            overlay.setColor( overlayColor );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textpointsize", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting text point size to %d", temp );

            textOverlay overlay = current_.overlay();
            QFont overlayFont = overlay.font();

            overlayFont.setPointSize( temp );

            overlay.setFont( overlayFont );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "textfont", value ) )
        {
            LOG_DEBUG( "setting text font to '%s'", qPrintable( value ) );

            textOverlay overlay = current_.overlay();
            QFont overlayFont = overlay.font();

            overlayFont.setFamily( value );

            overlay.setFont( overlayFont );
            current_.setOverlay( overlay );
        }
        else if ( checkForParameter( line, "animation", value ) )
        {
            unsigned int temp = value.toInt();

            LOG_DEBUG( "setting animation to %d", temp );

            current_.setAnimation( temp );
        }
        else
        {
            LOG_TRACE( "adding file '%s'", qPrintable( line ) );
            current_.push_back( line );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaPlaylist mediaPlaylistFileParser::finalize( QStringList& missing ) const
{
    LOG_CLASS_INFO( "mediaPlaylistFileParser", "finalize" );

    mediaPlaylist result;

    // find playlist section
    sectionMapType::const_iterator playlist = sectionMap_.find( "playlist" );

    if ( sectionMap_.end() == playlist )
        LOG_ERROR( "could not find 'playlist' section!", NULL );
    else
    {
        // loop through each playlist
        foreach ( const QString& name, playlist.value() )
        {
            // find playlist in map
            sectionMapType::const_iterator m = sectionMap_.find( name );

            // add it to list
            if ( sectionMap_.end() == m )
                LOG_ERROR( "could not find '%s' section!", qPrintable( name ) );
            else
            {
                m.value().addToPlaylist( result, missing );
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mediaPlaylistFileParser::insertSection( const QString &nextTag )
{
    LOG_CLASS_INFO( "mediaPlaylistFileParser", "insertSection" );

    if ( currentTag_.length() )
    {
        if ( !current_.size() )
            LOG_WARN( "empty playlist '%s'", qPrintable( currentTag_ ) );
        else
        {
            LOG_TRACE( "inserting section '%s'", qPrintable( currentTag_ ) );

            sectionMap_.insert( currentTag_, current_ );
        }
    }

    currentTag_ = nextTag;
    current_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mediaPlaylistFileParser::checkForParameter( const QString& text, const QString& param, QString& value )
{
    QString paramString = param + "=";
    int paramStringLen = paramString.length();

    if ( paramString == text.left( paramStringLen ) )
    {
        value = text.right( text.length() - paramStringLen );
        return true;
    }

    return false;
}
