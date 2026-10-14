/**
 * @file gcdlayout.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "gcdlayout.h"
#include "gcdlayoutfileparser.h"

#include <QCryptographicHash>
#include <QFile>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect gcdLayout::nowServingLocation() const
{
    // for embedded adverts just use the widget geometry
    if ( nowServingLocation_.isNull() )
        if ( isEmbeddedAdverts() )
            return ui_->embeddedAdverts->geometry();

    return nowServingLocation_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayout::changed() const
{
    LOG_CLASS_INFO( "gcdLayout", "changed" );

    LOG_TRACE( "checking %d files...", fileHashTable_.size() );

    // check for file different
    for ( fileHashMap::const_iterator i = fileHashTable_.begin(); i != fileHashTable_.end(); ++i )
        if ( i.value() != generateFileHash( i.key() ) )
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayout::load( const QString& filename )
{
    LOG_CLASS_INFO( "gcdLayout", "load" );

    // init
    initialize();

    // clear existing images
    clearLayoutImages();

    // reset widgets
    ui_->embeddedAdverts->hide();
    fullscreenAdverts_ = true;

    metrologyFSBackgroundName_ = "";
    metrologyAdvertBackgroundName_ = "";

    nowServingLocation_ = QRect();

    LOG_INFO( "loading layout '%s'...", qPrintable( filename ) );
    if ( gcdLayoutFileParser::parse( filename, *this ) )
    {
        addLayoutImage( (filename_ = filename) );

        // save off fullscreen advert setting
        if ( ui_->embeddedAdverts->isVisible() )
        {
            ui_->embeddedAdverts->hide();
            fullscreenAdverts_ = false;
        }

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLayout::addLayoutImage( const QString& filename )
{
    LOG_CLASS_INFO( "gcdLayout", "addLayoutImage" );

    LOG_INFO( "adding image file '%s'...", qPrintable( filename ) );
    fileHashTable_[filename] = generateFileHash( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLayout::clearLayoutImages()
{
    fileHashTable_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLayout::initialize()
{
    if ( !initialized_ )
    {
        // set default palettes
        setPalette<OutOfService>( QString::fromLatin1( ":/images/HobartWallpaper.png" ) );
        setPalette<TestImage1>( QString::fromLatin1( ":/images/RGB64Th.png" ) );
        setPalette<TestImage2>( QString::fromLatin1( ":/images/GcdGrayBackground.png" ) );
        setPalette<Slideshow>( Qt::black );
        setPalette<Video>( Qt::black );

        // fullscreen adverts by default
        fullscreenAdverts_ = true;

        initialized_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcdLayout::copy( const _Myt& rhs )
{
    ui_ = rhs.ui_;

    filename_ = rhs.filename_;

    metrologyFSBackgroundName_ = rhs.metrologyFSBackgroundName_;
    metrologyAdvertBackgroundName_ = rhs.metrologyAdvertBackgroundName_;

    metrologyBackground_ = rhs.metrologyBackground_;
    metrologyFSBackground_ = rhs.metrologyFSBackground_;
    metrologyBackgroundWithAdverts_ = rhs.metrologyBackgroundWithAdverts_;
    metrologyFSBackgroundWithAdverts_ = rhs.metrologyFSBackgroundWithAdverts_;
    outOfService_ = rhs.outOfService_;
    slideshowPalette_ = rhs.slideshowPalette_;
    storeLogo_ = rhs.storeLogo_;
    testImage1_ = rhs.testImage1_;
    testImage2_ = rhs.testImage2_;
    videoPalette_ = rhs.videoPalette_;

    nowServingLocation_ = rhs.nowServingLocation_;

    fullscreenAdverts_ = rhs.fullscreenAdverts_;

    fileHashTable_ = rhs.fileHashTable_;

    initialized_ = rhs.initialized_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayout::setPaletteBackgroundRole( const QWidget& widget, QPalette& palette, const QString& filename )
{
    LOG_CLASS_INFO( "gcdLayout", "setPaletteBackgroundRole" );

    QPixmap image;

    if ( !QFile::exists( filename ) )
    {
        LOG_ERROR( "file '%s' does not exist!", qPrintable( filename ) );
        return false;
    }

    if ( !image.load( filename ) )
    {
        LOG_ERROR( "failed to load image '%s'!", qPrintable( filename ) );
        return false;
    }

    palette = widget.palette();
    palette.setBrush( widget.backgroundRole(), QBrush( image ) );

    // keep track of this file
    addLayoutImage( filename );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayout::setPaletteBackgroundRoleOverlay( const QWidget& widget, QPalette& palette, const QPalette& base, const QString& filename )
{
    LOG_CLASS_INFO( "gcdLayout", "setPaletteBackgroundRoleOverlay" );

    QPixmap overlay;

    if ( !QFile::exists( filename ) )
    {
        LOG_ERROR( "file '%s' does not exist!", qPrintable( filename ) );
        return false;
    }

    // load overlay
    if ( !overlay.load( filename ) )
    {
        LOG_ERROR( "failed to load image '%s'!", qPrintable( filename ) );
        return false;
    }

    const QSize renderedSize( overlay.size() );

    QPixmap rendered( renderedSize );
    rendered.fill();

    QPainter painter( &rendered );
    painter.setPen( Qt::transparent );
    painter.setRenderHint( QPainter::Antialiasing, true );

    // paint base
    painter.setBrush( base.brush( widget.backgroundRole() ) );
    painter.drawRect( QRect( QPoint( 0, 0 ), renderedSize ) );

    // paint overlay
    painter.setBrush( overlay );
    painter.drawRect( QRect( QPoint( 0, 0 ), renderedSize ) );

    // set palette
    palette = widget.palette();
    palette.setBrush( widget.backgroundRole(), QBrush( rendered ) );

    // keep track of this file
    addLayoutImage( filename );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayout::setPaletteBackgroundRole( const QWidget& widget, QPalette& palette, const QColor& color )
{
    palette = widget.palette();
    palette.setBrush( widget.backgroundRole(), QBrush( color ) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray gcdLayout::generateFileHash( const QString& filename )
{
    QFile f( filename );
    QByteArray hash;

    // open file and generate hash
    if ( f.open( QIODevice::ReadOnly ) )
    {
        // generate hash
        hash = QCryptographicHash::hash( f.readAll(), QCryptographicHash::Md5 );

        f.close();
    }

    return hash;
}
