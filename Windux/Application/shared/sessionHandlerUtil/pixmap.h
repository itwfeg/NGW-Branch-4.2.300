/**
 * @file pixmap.h
 * Methods for QPixmap manipulation.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PIXMAP_H
#define PIXMAP_H

#include <QtGlobal>

#include <QBitmap>
#include <QImage>
#include <QPainter>
#include <QPixmap>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct scale_pixmap_to_width_t
 * @brief Create scaled pixmap to specified width.
 */
struct scale_pixmap_to_width_t {};

/**
 * @struct scale_pixmap_to_height_t
 * @brief Create scaled pixmap to specified height.
 */
struct scale_pixmap_to_height_t {};

/**
 * Used to scale a pixmap to specific width.
 */
static const scale_pixmap_to_width_t scale_pixmap_to_width = {};

/**
 * Used to scale a pixmap to specific height
 */
static const scale_pixmap_to_height_t scale_pixmap_to_height = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class flipPixmap
 * @brief Functor for flipping a pixmap.
 *
 * This functor object will flip pixmaps across a parameterized axis.
 *
 * Example usage:
 * @code
 *   flipPixmap flipH( true );
 *   flipPixmap flipV( false, true );
 *   flipPixmap rotate( true, true );
 *   QPixmap image( "/path/to/image.png" );
 *   QPixmap mirroredH = flipH( image );
 *   QPixmap mirroredV = flipV( image );
 *   QPixmap rotated = rotate( image );
 * @endcode
 */
class flipPixmap : public std::unary_function<QPixmap, QPixmap>
{
public:

    /**
     * Constructor.
     *
     * @param[in] horizontal  @c true to flip over the y axis, @c false otherwise
     * @param[in] vertical  @c true to flip over the x axis, @c false otherwise
     */
    flipPixmap( bool horizontal = false, bool vertical = false ) :
        horizontal_( horizontal ), vertical_( vertical ) {}

    /// Destructor.
    virtual ~flipPixmap() {}

    /**
     * Function operator.
     *
     * @param[in] image  pixmap to flip
     * @returns  flipped pixmap
     */
    QPixmap operator () ( const QPixmap& image ) const
    {
        QImage temp( image.toImage() );
        return QPixmap::fromImage( temp.mirrored( horizontal_, vertical_ ) );
    }

private:

    bool horizontal_;
    bool vertical_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class invertPixmap
 * @brief Functor for inverting a pixmap.
 *
 * This functor object will invert pixmaps.
 *
 * Example usage:
 * @code
 *   invertPixmap inverter();
 *   QPixmap image( "/path/to/image.png" );
 *   QPixmap inverted = inverter( image );
 * @endcode
 */
class invertPixmap : public std::unary_function<QPixmap, QPixmap>
{
public:

    /// Constructor.
    invertPixmap() {}

    /// Destructor.
    virtual ~invertPixmap() {}

    /**
     * Function operator.
     *
     * @param[in] image  pixmap to invert
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image ) const
    {
        // invert!!
        QImage temp( image.toImage() );
        temp.invertPixels();

        QPixmap result( QPixmap::fromImage( temp ) );

// Note: This has been depreciated in Qt5, needs checked!!
#if ( QT_VERSION < 0x050000 )
        // fixup alpha channel
        if ( image.hasAlphaChannel() )
            result.setAlphaChannel( image.alphaChannel() );
#endif

        return result;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class scalePixmap
 * @brief Functor for scaling a pixmap.
 *
 * This functor object will scale pixmaps to specific sizes using parameterized modes.
 *
 * Example usage:
 * @code
 *   scalePixmap scaler( QSize( 32, 32 ) );
 *   QPixmap image1( "/path/to/image1.png" );
 *   QPixmap image2( "/path/to/image2.png" );
 *   QPixmap thumbnail1 = scaler( image1 );
 *   QPixmap thumbnail2 = scaler( image2 );
 * @endcode
 */
class scalePixmap : public std::unary_function<QPixmap, QPixmap>
{
public:

    /**
     * Constructor.
     *
     * @param[in] aspectRatioMode  aspect ratio mode
     * @param[in] transformMode  transformation mode
     */
    scalePixmap( Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) :
        aspectRatioMode_( aspectRatioMode ), transformMode_( transformMode ) {}

    /**
     * Constructor.
     *
     * @param[in] size  size to scale to
     * @param[in] aspectRatioMode  aspect ratio mode
     * @param[in] transformMode  transformation mode
     */
    scalePixmap( const QSize& size, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) :
        size_( size ), aspectRatioMode_( aspectRatioMode ), transformMode_( transformMode ) {}

    /// Destructor.
    virtual ~scalePixmap() {}

    /**
     * Function operator.
     *
     * @param[in] image  pixmap to scale
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image ) const
    {
        QImage temp( image.toImage() );
        return QPixmap::fromImage( temp.scaled( size_, aspectRatioMode_, transformMode_ ) );
    }

    /**
     * Function operator.
     *
     * This functor will scale an image to a specific size. For example:
     * @code
     *   scalePixmap scaler;
     *   QPixmap image( "/path/to/image.png" );
     *   QPixmap i200x50px = scaler( image, QSize( 200, 50 ) ); // make 200 pixels wide and 50 pixels tall
     *   QPixmap i60x300px = scaler( image, QSize( 60, 300 ) ); // make 60 pixels wide and 300 pixels tall
     * @endcode
     *
     * @param[in] image  pixmap to scale
     * @param[in] size  size to scale to
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image, const QSize& size ) const
    {
        QImage temp( image.toImage() );
        return QPixmap::fromImage( temp.scaled( size, aspectRatioMode_, transformMode_ ) );
    }

    /**
     * Function operator.
     *
     * This functor will scale an image by width. For example:
     * @code
     *   scalePixmap scaler;
     *   QPixmap image( "/path/to/image.png" );
     *   QPixmap i200px = scaler( image, 200, scale_pixmap_to_width ); // make 200 pixels wide
     *   QPixmap i60px = scaler( image, 60, scale_pixmap_to_width ); // make 60 pixels wide
     * @endcode
     *
     * @param[in] image  pixmap to scale
     * @param[in] width  width to scale to
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image, int width, scale_pixmap_to_width_t ) const
    {
        QImage temp( image.toImage() );
        return QPixmap::fromImage( temp.scaledToWidth( width, transformMode_ ) );
    }

    /**
     * Function operator.
     *
     * This functor will scale an image by height. For example:
     * @code
     *   scalePixmap scaler;
     *   QPixmap image( "/path/to/image.png" );
     *   QPixmap i200px = scaler( image, 200, scale_pixmap_to_height ); // make 200 pixels tall
     *   QPixmap i60px = scaler( image, 60, scale_pixmap_to_height ); // make 60 pixels tall
     * @endcode
     *
     * @param[in] image  pixmap to scale
     * @param[in] height  height to scale to
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image, int height, scale_pixmap_to_height_t ) const
    {
        QImage temp( image.toImage() );
        return QPixmap::fromImage( temp.scaledToHeight( height, transformMode_ ) );
    }

    /**
     * Function operator.
     *
     * This functor will scale an image by a percentage. For example:
     * @code
     *   scalePixmap scaler;
     *   QPixmap image( "/path/to/image.png" );
     *   QPixmap smaller = scaler( image, -0.75 ); // make 25% smaller
     *   QPixmap bigger = scaler( image, 1.3 ); // make 30% bigger
     *   QPixmap huge = scaler( image, 7.0 ); // make 700% bigger
     * @endcode
     *
     * @param[in] image  pixmap to scale
     * @param[in] percent  percentage to scale
     * @returns  inverted pixmap
     */
    QPixmap operator () ( const QPixmap& image, qreal percent ) const
    {
        QImage temp( image.toImage() );
        QSize size( image.width() * percent, image.height() * percent );
        return QPixmap::fromImage( temp.scaled( size, aspectRatioMode_, transformMode_ ) );
    }

private:

    QSize size_;
    Qt::AspectRatioMode aspectRatioMode_;
    Qt::TransformationMode transformMode_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class sizeToFitPixmap
 * @brief Functor for resizing a pixmap to a specific area while maintaining aspect ratio.
 *
 * This functor object will resize and center pixmaps.
 *
 * Example usage:
 * @code
 *   sizeToFitPixmap sizeToFit( QSize( 32, 32 ) );
 *   QPixmap image1( "/path/to/image1.png" );
 *   QPixmap image2( "/path/to/image2.png" );
 *   QPixmap thumbnail1 = sizeToFit( image1 );
 *   QPixmap thumbnail2 = sizeToFit( image2 );
 * @endcode
 */
class sizeToFitPixmap : public std::unary_function<QPixmap, QPixmap>
{
public:

    /**
     * Constructor.
     */
    sizeToFitPixmap() {}

    /**
     * Constructor.
     *
     * @param[in] size  size to scale to
     */
    sizeToFitPixmap( const QSize& size ) :
        size_( size ) {}

    /// Destructor.
    virtual ~sizeToFitPixmap() {}

    /**
     * Function operator.
     *
     * @param[in] image  pixmap to resize
     * @returns  resized pixmap
     */
    QPixmap operator () ( const QPixmap& image ) const
    {
        return sizeToFit( image, size_ );
    }

    /**
     * Function operator.
     *
     * This functor will resize an image to a specific area. For example:
     * @code
     *   sizeToFitPixmap sizeToFit;
     *   QPixmap image( "/path/to/image.png" );
     *   QPixmap i200x50px = sizeToFit( image, QSize( 200, 50 ) ); // make 200 pixels wide and 50 pixels tall
     *   QPixmap i60x300px = sizeToFit( image, QSize( 60, 300 ) ); // make 60 pixels wide and 300 pixels tall
     * @endcode
     *
     * @param[in] image  pixmap to resize
     * @param[in] size  new size
     * @returns  resized pixmap
     */
    QPixmap operator () ( const QPixmap& image, const QSize& size ) const
    {
        return sizeToFit( image, size );
    }

private:

    QSize size_;

    /**
     * Resize pixmap @p source while maintaining aspect ratio.
     *
     * @param[in] source  pixmap to resize
     * @param[in] resizedSize  new size
     * @returns  resized pixmap
     */
    static QPixmap sizeToFit( const QPixmap& source, const QSize& resizedSize )
    {
        QSize destSize( resizedSize );
        int w( destSize.width() );
        int h( destSize.height() );

        const QSize sourceSize( source.size() );

        // sanity check
        if (( source.isNull() ) ||
            ( sourceSize.width() <= 0 ) ||
            ( sourceSize.height() <= 0 ) ||
            (( w <= 0 ) && ( h <= 0 )))
        {
            return QPixmap();
        }

        // calculate bounds
        // we are bounding by width, height, or both... whatever is set...
        if (( w <= 0 ) && ( 0 < h ))
            destSize = QSize( INT_MAX, h );
        else if (( 0 < w ) && ( h <= 0 ))
            destSize = QSize( w, INT_MAX );

        // scale source image
        QSize scaledSourceSize( sourceSize );
        scaledSourceSize.scale( destSize, Qt::KeepAspectRatio );

        // update bounding size
        // use scaled width or height if we did not bound that dimension
        if ( w <= 0 )
            destSize.setWidth( scaledSourceSize.width() );
        else if ( h <= 0 )
            destSize.setHeight( scaledSourceSize.height() );

        // create pixmap
        QPixmap dest( destSize );
        dest.fill( Qt::transparent );

        QRect rect(
            (destSize.width() - scaledSourceSize.width()) / 2,
            (destSize.height() - scaledSourceSize.height()) / 2,
            scaledSourceSize.width(),
            scaledSourceSize.height() );

        // create painter
        QPainter painter( &dest );
        painter.setRenderHint( QPainter::Antialiasing, true );

        painter.setBackgroundMode( Qt::TransparentMode );

        // draw pixmap
        painter.drawPixmap( rect, source );

        return dest;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PIXMAP_H
