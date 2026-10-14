/**
 * @file mediaplaylistfileparser.h
 * Parser for media playlist files.
 *
 * This parser will parse a playlist text file into a @c mediaPlaylist.
 *
 * Here is an example playlist file:
 * @code
 *   [video1]
 *   cub1.wmv
 *
 *   [video2]
 *   cub2.wmv
 *
 *   [slideshow1]
 *   delay=5
 *   fruit_01.jpg
 *   fruit_02.jpg
 *   fruit_03.jpg
 *   fruit_04.jpg
 *
 *   [slideshow2]
 *   delay=15
 *   audio=music.mp3
 *   text=Overlay with\nmultiple text lines\nand sound!
 *   textfont=Arial
 *   textcolorr=255
 *   textcolorg=255
 *   textcolorb=0
 *   textpointsize=32
 *   textvalign=top
 *   texthalign=left
 *   hobart.png
 *
 *   [playlist]
 *   video1
 *   slideshow1
 *   video2
 *   slideshow2
 * @endcode
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef MEDIAPLAYLISTFILEPARSER_H
#define MEDIAPLAYLISTFILEPARSER_H

#include "mediaplaylist.h"

#include <QString>
#include <QList>
#include <QMap>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class used by @c mediaPlaylistFileParser to parse sections.
/**
 * This class is an internal helper class used by @c mediaPlaylistFileParser.
 */
class mediaPlaylistFileParserSection : public QList<QString>
{
    typedef mediaPlaylistFileParserSection _Myt;
    typedef QList<QString> _Mybase;

public:

    /// Default delay (in ms).
    enum {DEFAULT_DELAY_MS = 5000};

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    mediaPlaylistFileParserSection() : _Mybase(), delay_( DEFAULT_DELAY_MS ) {}

    /// Constructor.
    /**
     * @param[in] rhs  section to copy
     */
    mediaPlaylistFileParserSection( const _Myt& rhs ) : _Mybase() {copy( rhs );}

    /// Destructor.
    virtual ~mediaPlaylistFileParserSection() {}

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  value to assign from
     * @return  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Get delay.
    /**
     * @return  section delay in seconds
     */
    virtual unsigned int delay() const {return delay_;}

    /// Set delay.
    /**
     * @param[in] value  delay in seconds
     */
    virtual void setDelay( unsigned int value ) {delay_ = value;}

    /// Get overlay.
    /**
     * @return  section overlay
     */
    virtual textOverlay overlay() const {return overlay_;}

    /// Set overlay.
    /**
     * @param[in] value  overlay
     */
    virtual void setOverlay( const textOverlay& value ) {overlay_ = value;}

    /// Get audio filename.
    /**
     * @return  audio filename string
     */
    virtual QString audio() const {return audio_;}

    /// Set audio.
    /**
     * @param[in] value  audio filename string
     */
    virtual void setAudio( const QString& value ) {audio_ = value;}

    /// Get animation.
    /**
     * @return  animation when transitioning images
     */
    virtual unsigned int animation() const {return animation_;}

    /// Set animation.
    /**
     * @param[in] value  animation when transitioning images
     */
    virtual void setAnimation( unsigned int value ) {animation_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Adds this section to passed in playlist.
    /**
     * @param[out] playlist  playlist to add @c this into
     * @param[out] missing  media files that are missing from the filesystem
     */
    virtual void addToPlaylist( mediaPlaylist& playlist, QStringList& missing ) const;

    /// Clears the list and section settings.
    virtual void clear();

private:

    unsigned int delay_;
    textOverlay overlay_;
    QString audio_;
    unsigned int animation_;

    /// Copy passed in object.
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Parses a playlist into a @c playlist object.
/**
 * Example usage:
 * @code
 *   mediaPlaylist myPlaylist;
 *   mediaPlaylistFileParser::parse( "playlist.txt", myPlaylist );
 * @endcode
 */
class mediaPlaylistFileParser
{
    typedef mediaPlaylistFileParser _Myt;

public:

    /// Destructor.
    virtual ~mediaPlaylistFileParser() {}

    /// Create playlist from file.
    /**
     * Parses the playlist file @p filename and creates a playlist.  Playlist is given back
     * to caller.
     *
     * @param[in] filename  playlist filename
     * @param[out] playlist  playlist object to receive media
     * @param[out] missing  missing media files
     * @return  @c true if file parsed successfully, @c false otherwise
     */
    static bool parse( const QString& filename, mediaPlaylist& playlist, QStringList& missing  );

    /// Image to display for missing/corrupt media.
    /**
     * @return  image filename string
     */
    static QString missingMediaPlaceholderFilename() {return ":/images/RGB64Th.png";}

protected:

    /// Parse one line.
    /**
     * @param[in] line  line from playlist file
     */
    virtual void parseLine( const QString& line );

    /// Finalize parser.
    /**
     * Finalizes parser and returns the playlist.
     *
     * @param[out] missing  missing media files
     * @return  @c mediaPlaylist object
     */
    virtual mediaPlaylist finalize( QStringList& missing ) const;

private:

    QString currentTag_;
    mediaPlaylistFileParserSection current_;

    typedef QMap<QString, mediaPlaylistFileParserSection> sectionMapType;
    sectionMapType sectionMap_;

    /// Constructor.
    mediaPlaylistFileParser();

    /// Insert current section into mapping.
    void insertSection( const QString& nextTag );

    /// Check text against parameter and retrieve the value.
    static bool checkForParameter( const QString& text, const QString& param, QString& value );

    // not implemented
    mediaPlaylistFileParser( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MEDIAPLAYLISTFILEPARSER_H
