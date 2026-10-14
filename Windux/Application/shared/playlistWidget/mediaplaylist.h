/**
 * @file mediaplaylist.h
 * Media and media playlist class object definitions.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef MEDIAPLAYLIST_H
#define MEDIAPLAYLIST_H

#include <platform.h>

#include <QColor>
#include <QFont>
#include <QList>
#include <QMap>
#include <QPixmap>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Object that stores text and font information for text overlay on an image.
class textOverlay
{
    typedef textOverlay _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    textOverlay() :
        text_( "" ),
        font_( "Arial", 8, QFont::Normal ),
        color_( Qt::black ),
        alignment_( Qt::AlignCenter ) {}

    /// Constructor.
    /**
     * @param[in] rhs  overlay to copy
     */
    textOverlay( const _Myt& rhs ) {copy( rhs );}

    /// Destructor.
    virtual ~textOverlay() {}

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

    /// Overlay text.
    /**
     * @return  text to display on the overlay
     */
    virtual QString text() const {return text_;}

    /// Set overlay text.
    /**
     * @param[in] value  overlay text
     */
    virtual void setText( const QString& value ) {text_ = value;}

    /// Overlay font.
    /**
     * @return  font to use for overlay text
     */
    virtual QFont font() const {return font_;}

    /// Set overlay font.
    /**
     * @param[in] value  overlay font
     */
    virtual void setFont( const QFont& value ) {font_ = value;}

    /// Overlay color.
    /**
     * @return  color to use when drawing overlay text
     */
    virtual QColor color() const {return color_;}

    /// Set overlay color.
    /**
     * @param[in] value  overlay color
     */
    virtual void setColor( const QColor& value ) {color_ = value;}

    /// Overlay alignment.
    /**
     * @return  overlay text alignment (location)
     */
    virtual Qt::Alignment alignment() const {return alignment_;}

    /// Set overlay alignment.
    /**
     * @param[in] value  overlay alignment
     */
    virtual void setAlignment( Qt::Alignment value ) {alignment_ = value;}

private:

    QString text_;

    QFont font_;
    QColor color_;
    Qt::Alignment alignment_;

    /// Copy overlay.
    void copy( const _Myt& rhs ) {
        text_ = rhs.text_;
        font_ = rhs.font_;
        color_ = rhs.color_;
        alignment_ = rhs.alignment_;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Media Types.
/**
 * Enumeration class of media types.
 */
class mediaType
{
public:

    /// Media type.
    enum type
    {
        Unknown,                            ///< unknown/invalid media
        Image,                              ///< images (i.e. bmp, jpeg, gif)
        Video,                              ///< videos (i.e. avi, mov)
        Web                                 ///< website url (i.e. http)
    };

    /// Get media type from filename.
    /**
     * Determines media type based upon filename extension.
     *
     * @param[in] filename  filename of media
     * @return  media type
     */
    static type get( const QString& filename );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Animation Types.
/**
 * Enumeration class of animation types.
 */
class animationType
{
public:

    /// Animation type.
    enum type
    {
        None = 0,                           ///< no animation
        ClockwiseSlide,                     ///< slide in from corner, rotating clockwise
        CounterClockwiseSlide,              ///< slide in from corner, rotating counter-clockwise

        // insert new types above this one
        MAX_TYPES
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that stores media name and media properties used during playback/slideshow.
class media
{
    typedef media _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    media() :
        type_( mediaType::Unknown ),
        duration_( 0 ),
        animation_( animationType::None  ),
        aspectRatio_( (Qt::AspectRatioMode) DEFAULT_ASPECT_RATIO ) {}

    /// Constructor.
    /**
     * @param[in] filename  media filename
     * @param[in] duration  duration of image media in milliseconds
     */
    media( QString filename, unsigned int duration ) {
        setFilename( filename, duration );
    }

    /// Constructor.
    /**
     * @param[in] rhs  media to copy
     */
    media( const _Myt& rhs ) {copy( rhs );}

    /// Destructor.
    virtual ~media() {}

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

    /// Media filename.
    /**
     * @return  filename of media
     */
    virtual QString filename() const {return filename_;}

    /// Set media filename.
    /**
     * @param[in] filename  media filename
     * @param[in] duration  duration of image media in milliseconds
     */
    virtual void setFilename( const QString& filename, unsigned int duration );

    /// Media type.
    /**
     * @return  type of media
     */
    virtual mediaType::type type() const {return type_;}

    /// Media duration.
    /**
     * How long media should be shown on screen (i.e. for images)
     *
     * @return  duration of media in milliseconds
     */
    virtual unsigned int duration() const {return duration_;}

    /// Text overlay for static image.
    /**
     * @return  text to overlay on image
     */
    virtual textOverlay overlay() const {return overlay_;}

    /// Set overlay for static image.
    /**
     * @param[in] value  text overlay
     */
    virtual void setOverlay( const textOverlay& value ) {overlay_ = value;}

    /// Sound to play in background for image.
    /**
     * @return  filename of audio to play in background
     */
    virtual QString soundFilename() const {return soundFilename_;}

    /// Set background sound.
    /**
     * @param[in] value  audio filename
     */
    virtual void setSoundFilename( const QString& value ) {soundFilename_ = value;}

    /// Aspect ratio.
    /**
     * @return  aspect ratio for scaling
     */
    virtual Qt::AspectRatioMode aspectRatio() const {return aspectRatio_;}

    /// Set aspect ratio.
    /**
     * @param[in] value  aspect ratio for scaling
     */
    virtual void setAspectRatio( Qt::AspectRatioMode value ) {aspectRatio_ = value;}

    /// Get animation.
    /**
     * @return  animation when transitioning images
     */
    virtual animationType::type animation() const {return animation_;}

    /// Set animation.
    /**
     * @param[in] value  animation when transitioning images
     */
    virtual void setAnimation( animationType::type value );

    /// Get rendering.
    /**
     * @return  pixmap rendering
     */
    virtual QPixmap rendering() const {return rendering_;}

    /// Get rendering.
    /**
     * @param[in] value  pixmap rendering
     */
    virtual void setRendering( const QPixmap& value ) {rendering_ = value;}

private:

    enum {MIN_DURATION = 2000};             // 2 seconds
    enum {MAX_DURATION = 1800000};          // 30 minutes

    enum {DEFAULT_ASPECT_RATIO = Qt::KeepAspectRatio};

    QString filename_;
    mediaType::type type_;
    unsigned int duration_;

    textOverlay overlay_;
    QString soundFilename_;

    animationType::type animation_;

    Qt::AspectRatioMode aspectRatio_;

    QPixmap rendering_;

    /// Copy media.
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// List of @c media objects.
typedef QList<media> mediaPlaylist;

/** @} */

#endif // MEDIAPLAYLIST_H
