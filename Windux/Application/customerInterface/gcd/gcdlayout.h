/**
 * @file gcdlayout.h
 * Storage of layout information pertaining to the GCD UI.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCDLAYOUT_H
#define GCDLAYOUT_H

#include "ui_gcd.h"

#include <QString>
#include <QPalette>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD user interface layout and palette information.
class gcdLayout
{
    typedef gcdLayout _Myt;

public:

    /// Layout palette types.
    /**
     * This class stores a number of different background and image palettes. Here is a listing
     * of each palette stored.
     */
    enum LayoutPalette
    {
        MetrologyBackground,                            ///< background for metrology page
        MetrologyFSBackground,                          ///< background for metrology page when frequent shopper item
        MetrologyBackgroundWithAdverts,                 ///< background for metrology page with advertising
        MetrologyFSBackgroundWithAdverts,               ///< background for metrology page when frequent shopper item with advertising
        OutOfService,                                   ///< out of service background image
        TestImage1,                                     ///< test image
        TestImage2,                                     ///< test image
        Slideshow,                                      ///< slideshow background
        StoreLogo,                                      ///< store logo
        Video                                           ///< video background
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    gcdLayout( Ui::gcd *ui ) : ui_( ui ), initialized_( false ) {}

    /// Constructor.
    /**
     * @param rhs  object to copy
     */
    gcdLayout( const _Myt& rhs ) {copy( rhs );}

    /// Destructor.
    virtual ~gcdLayout() {}

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

    /// Get layout filename.
    /**
     * @return  filename
     */
    virtual QString filename() const {return filename_;}

    /// Get pointer to gcd ui.
    /**
     * @return  pointer to ui
     */
    virtual Ui::gcd *ui() const {return ui_;}

    /// Check if adverts are embedded or not.
    /**
     * Adverts may be fullscreen (default) or embedded within the page.
     *
     * @return  @c true for embedded adverts, @c false otherwise
     */
    virtual bool isEmbeddedAdverts() const {return !fullscreenAdverts_;}

    /// Check if adverts are fullscreen or not.
    /**
     * Adverts may be fullscreen (default) or embedded within the page.
     *
     * @return  @c true for fullscreen adverts, @c false otherwise
     */
    virtual bool isFullscreenAdverts() const {return fullscreenAdverts_;}

    /// Retrieve now serving location.
    /**
     * @return  rect for now serving popup
     */
    virtual QRect nowServingLocation() const;

    /// Set now serving location.
    /**
     * @param[in] value  rect for now serving popup
     */
    virtual void setNowServingLocation( const QRect& value ) {nowServingLocation_ = value;}

    /// Retrieve palette of particular type.
    /**
     * @return  palette
     *
     * @tparam _Palette  which palette to retrieve
     */
    template <LayoutPalette _Palette>
    QPalette palette() const {
        return paletteRef<_Palette>();
    }

    /// Set palette from image file.
    /**
     * @param[in] filename  filename of background image
     * @return  @c true if image was loaded successfully, @c false otherwise
     *
     * @tparam _Palette  which palette to set
     */
    template <LayoutPalette _Palette>
    bool setPalette( const QString& filename ) {
        return setPaletteBackgroundRole( *ui_->videoPage, paletteRef<_Palette>(), filename );
    }

    /// Set palette from color.
    /**
     * @param[in] color  color to make palette
     * @return  @c true if image was loaded successfully, @c false otherwise
     *
     * @tparam _Palette  which palette to set
     */
    template <LayoutPalette _Palette>
    bool setPalette( const QColor& color ) {
        return setPaletteBackgroundRole( *ui_->videoPage, paletteRef<_Palette>(), color );
    }

    /// Set palette from image file overlay.
    /**
     * @param[in] filename  filename of background image overlay
     * @return  @c true if image was loaded successfully, @c false otherwise
     *
     * @tparam _Palette  which palette to set
     */
    template <LayoutPalette _Palette>
    bool setPaletteOverlay( const QString& filename );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Check if layout has changed.
    /**
     * @return  @c true if different, @c false otherwise
     */
    virtual bool changed() const;

    /// Load user interface settings from file.
    /**
     * @param[in] filename  file to load settings from
     * @return  @c true on success, @c false otherwise
     */
    virtual bool load( const QString& filename );

protected:

    /// Add images to layout.
    /**
     * @param[in] filename  image filename
     */
    virtual void addLayoutImage( const QString& filename );

    /// Clear all images from layout.
    virtual void clearLayoutImages();

private:

    Ui::gcd *ui_;

    QString filename_;

    QString metrologyFSBackgroundName_;
    QString metrologyAdvertBackgroundName_;

    QPalette metrologyBackground_;
    QPalette metrologyFSBackground_;
    QPalette metrologyBackgroundWithAdverts_;
    QPalette metrologyFSBackgroundWithAdverts_;
    QPalette outOfService_;
    QPalette slideshowPalette_;
    QPalette storeLogo_;
    QPalette testImage1_;
    QPalette testImage2_;
    QPalette videoPalette_;

    QRect nowServingLocation_;

    bool fullscreenAdverts_;

    typedef QMap<QString, QByteArray> fileHashMap;
    fileHashMap fileHashTable_;

    bool initialized_;

    /// Initialize layout.
    void initialize();

    /// Copy layout.
    void copy( const _Myt& rhs );

    /// Create palette from file.
    bool setPaletteBackgroundRole( const QWidget& widget, QPalette& palette, const QString& filename );

    /// Create palette from color.
    bool setPaletteBackgroundRole( const QWidget& widget, QPalette& palette, const QColor& color );

    /// Create palette from overlay file.
    bool setPaletteBackgroundRoleOverlay( const QWidget& widget, QPalette& palette, const QPalette& base, const QString& filename );

    /// Get reference to palette.
    template <LayoutPalette _Palette>
    QPalette& paletteRef();

    /// Get const reference to palette.
    template <LayoutPalette _Palette>
    const QPalette& paletteRef() const {
        return const_cast<gcdLayout*>( this )->paletteRef<_Palette>();
    }

    /// Generate hash of file.
    static QByteArray generateFileHash( const QString& filename );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::MetrologyBackground>() {return metrologyBackground_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::MetrologyFSBackground>() {return metrologyFSBackground_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::MetrologyBackgroundWithAdverts>() {return metrologyBackgroundWithAdverts_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::MetrologyFSBackgroundWithAdverts>() {return metrologyFSBackgroundWithAdverts_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::OutOfService>() {return outOfService_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::Slideshow>() {return slideshowPalette_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::StoreLogo>() {return storeLogo_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::TestImage1>() {return testImage1_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::TestImage2>() {return testImage2_;}
template <> inline QPalette& gcdLayout::paletteRef<gcdLayout::Video>() {return videoPalette_;}

// ================================================================================================
//
// Palette retrieval.
//
// These methods retrieve palette information. Below are specializations requiring extra logic.
//
// ================================================================================================

template <>
inline QPalette gcdLayout::palette<gcdLayout::MetrologyFSBackground>() const
{
    // check for valid frequent shopper background
    if ( metrologyFSBackgroundName_.length() )
        return paletteRef<MetrologyFSBackground>();
    return paletteRef<MetrologyBackground>();
}

template <>
inline QPalette gcdLayout::palette<gcdLayout::MetrologyBackgroundWithAdverts>() const
{
    // check for valid advertisment background
    if ( metrologyAdvertBackgroundName_.length() )
        return paletteRef<MetrologyBackgroundWithAdverts>();
    return paletteRef<MetrologyBackground>();
}

template <>
inline QPalette gcdLayout::palette<gcdLayout::MetrologyFSBackgroundWithAdverts>() const
{
    // check for valid advertisment background
    if ( metrologyAdvertBackgroundName_.length() )
        return paletteRef<MetrologyFSBackgroundWithAdverts>();
    return palette<MetrologyFSBackground>();
}

// ================================================================================================
//
// Palette creation from raw image.
//
// These methods simply overwrite palette with the supplied image. There is a default
// implementation above, below is specializations.
//
// ================================================================================================

template <>
inline bool gcdLayout::setPalette<gcdLayout::MetrologyFSBackground>( const QString& filename )
{
    if ( !setPaletteBackgroundRole( *ui_->videoPage, paletteRef<MetrologyFSBackground>(), filename ) )
        return false;

    metrologyFSBackgroundName_ = filename;
    return true;
}

// ================================================================================================
//
// Palette creation from overlays.
//
// These methods will create a palette using existing palette and then overlaying the supplied
// image on top of it.
//
// We support two types of overlays:
// 1) Frequent shopper overlay
//    This will create metrology background with box for frequent shopper info.
// 2) Advert overlay
//    This will create two backgrounds, one without frequent shopper info and one with also
//
// ================================================================================================

template <>
inline bool gcdLayout::setPaletteOverlay<gcdLayout::MetrologyBackgroundWithAdverts>( const QString& filename )
{
    if (( setPaletteBackgroundRoleOverlay(
              *ui_->videoPage,
              paletteRef<MetrologyBackgroundWithAdverts>(),
              palette<MetrologyBackground>(),
              filename ) ) &&
        ( setPaletteBackgroundRoleOverlay(
              *ui_->videoPage,
              paletteRef<MetrologyFSBackgroundWithAdverts>(),
              palette<MetrologyFSBackground>(),
              filename ) ))
    {
        metrologyAdvertBackgroundName_ = filename;
        return true;
    }

    return false;
}

template <>
inline bool gcdLayout::setPaletteOverlay<gcdLayout::MetrologyFSBackground>( const QString& filename )
{
    if ( setPaletteBackgroundRoleOverlay(
             *ui_->videoPage,
             paletteRef<MetrologyFSBackground>(),
             palette<MetrologyBackground>(),
             filename ) )
    {
        metrologyFSBackgroundName_ = filename;

        // if we have already set an advert background we need to render it again...
        if ( metrologyAdvertBackgroundName_.length() )
            setPaletteOverlay<gcdLayout::MetrologyBackgroundWithAdverts>( metrologyAdvertBackgroundName_ );

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDLAYOUT_H
