/**
 * @file mediaplaylist.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "mediaplaylist.h"

#include <QFile>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////
mediaType::type mediaType::get( const QString& filename )
{
    mediaType::type result( mediaType::Video );
    int idx;

    // check for image extensions
    if ( 0 <= (idx = filename.lastIndexOf( '.' )) )
    {
        QString ext( filename.right( filename.length() - idx - 1 ) );
        ext = ext.trimmed();
        ext = ext.toLower();

        if (( "bmp" == ext ) || ( "gif" == ext ) || ( "jpg" == ext ) ||
            ( "jpeg" == ext ) || ( "tif" == ext ) || ( "tiff" == ext ) || ( "png" == ext ))
        {
            result = mediaType::Image;
        }
    }

    // check for web url
    if ( 0 < (idx = filename.indexOf( ':' )) )
    {
        QString prefix( filename.left( idx ) );
        prefix = prefix.trimmed();
        prefix = prefix.toLower();

        if (( "http" == prefix ) || ( "https" == prefix ) || ( "file" == prefix ))
        {
            result = mediaType::Web;
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void media::setFilename(
    const QString& filename,
    unsigned int duration )
{
    filename_ = filename;
    type_ = mediaType::get( filename );

    duration_ = duration;

    // Note:
    // The aspect ratio setting will scale the image as per Qt::AspectRatioMode settings.  The
    // Qt::IgnoreAspectRatio will center an unaltered image.
    aspectRatio_ = (Qt::AspectRatioMode) DEFAULT_ASPECT_RATIO;

    if (( 0 == duration_ ) && ( mediaType::Web == type_ ))
        ; // do nothing; zero (unlimited) duration allowed for web
    else if ( MIN_DURATION > duration_ )
        duration_ = MIN_DURATION;
    else if ( MAX_DURATION < duration_ )
        duration_ = MAX_DURATION;

    // for video just limit upper bound of playback
    if ( mediaType::Video == type_ )
        duration_ = MAX_DURATION;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void media::setAnimation( animationType::type value )
{
    if ( value < animationType::MAX_TYPES )
        animation_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void media::copy( const _Myt& rhs )
{
    filename_ = rhs.filename_;
    type_ = rhs.type_;
    duration_ = rhs.duration_;

    overlay_ = rhs.overlay_;
    soundFilename_ = rhs.soundFilename_;

    animation_ = rhs.animation_;

    aspectRatio_ = rhs.aspectRatio_;
}
