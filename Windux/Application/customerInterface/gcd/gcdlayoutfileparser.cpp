/**
 * @file gcdlayoutfileparser.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "gcdlayoutfileparser.h"
#include "gcdlayout.h"

#include <filesystem.h>

#include <QDir>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::parse(
    const QString& filename,
    gcdLayout &layout )
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "parse" );

    // check for existance
    if ( !QFile::exists( filename ) )
    {
        LOG_ERROR( "file '%s' does not exist!", qPrintable( filename ) );
        return false;
    }

    // handler
    gcdLayoutFileParser handler( layout );

    // reader object
    QXmlSimpleReader reader;

    reader.setContentHandler( &handler );
    reader.setErrorHandler( &handler );

    // input source
    QFile file( filename );
    QXmlInputSource source( &file );

    LOG_INFO( "parsing file '%s'", qPrintable( filename ) );

    // parse!
    return reader.parse( source );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::error( const QXmlParseException& ex )
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "error" );

    LOG_ERROR( "fatal error on line %d column %d '%s'",
        ex.lineNumber(),
        ex.columnNumber(),
        qPrintable( ex.message() ) );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::fatalError( const QXmlParseException& ex )
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "fatalError" );

    LOG_ERROR( "error on line %d column %d '%s'",
        ex.lineNumber(),
        ex.columnNumber(),
        qPrintable( ex.message() ) );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::warning( const QXmlParseException& ex )
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "warning" );

    LOG_WARN( "warning on line %d column %d '%s'",
        ex.lineNumber(),
        ex.columnNumber(),
        qPrintable( ex.message() ) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::startDocument()
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "startDocument" );

    LOG_DEBUG( "parsing...", NULL );

    current_ = NULL;
    depth_ = 0;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::startElement(
    const QString& namespaceURI,
    const QString& localName,
    const QString& qName,
    const QXmlAttributes& atts )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "startElement" );

    bool result = true;

    LOG_TRACE( "element '%s' depth %d", qPrintable( qName ), depth_ );

    // root level objects
    if ( !depth_ )
    {
        if ( "Objects" != qName )
        {
            LOG_ERROR( "unknown root element '%s'", qPrintable( qName ) );
            result = false;
        }
    }
    // objects
    else if ( 1 == depth_ )
    {
        if ( "DisplayObjects" == qName )
            current_ = &displayObject_;
        else if ( "OverlayObjects" == qName )
            current_ = &overlayObject_;
        else
        {
            LOG_ERROR( "unknown root element '%s'", qPrintable( qName ) );
            result = false;
        }
    }
    // display objects
    else if ( &displayObject_ == current_ )
    {
        result = displayObjectStartElement( qName, atts );
    }
    // overlay objects
    else if ( &overlayObject_ == current_ )
    {
        result = overlayObjectStartElement( qName, atts );
    }
    else
    {
        LOG_ERROR( "bad current pointer!", NULL );
        result = false;
    }

    ++depth_;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::characters( const QString& ch )
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "characters" );

    bool result = true;

    QString trimmed = ch.trimmed();

    // process only valid strings
    if ( trimmed.length() )
    {
        LOG_TRACE( "data '%s'", qPrintable( trimmed ) );

        if ( current_ )
            result = objectCharacters( *current_, ch );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::endElement(
    const QString& namespaceURI,
    const QString& localName,
    const QString& qName )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "endElement" );

    bool result = true;

    --depth_;

    LOG_TRACE( "end element '%s' depth %d", qPrintable( qName ), depth_ );

    // root level objects
    if ( depth_ <= 1 )
        current_ = NULL;

    // display objects
    else if ( &displayObject_ == current_ )
        result = displayObjectEndElement( qName );

    // overlay objects
    else if ( &overlayObject_ == current_ )
        result = overlayObjectEndElement( qName );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::endDocument()
{
    LOG_CLASS_INFO( "gcdLayoutFileParser", "endDocument" );

    LOG_DEBUG( "parsing done!", NULL );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::objectCharacters( stringMapType& obj, const QString& ch )
{
    LOG_CLASS_INFO( "objectCharacters", "displayObjectCharacters" );

    if ( 4 != depth_ )
    {
        LOG_ERROR( "bad depth %d!", depth_ );
        return false;
    }

    LOG_DEBUG( "['%s', '%s']", qPrintable( elementName_ ), qPrintable( ch ) );

    obj[ elementName_ ] = ch;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::displayObjectStartElement(
    const QString& qName,
    const QXmlAttributes& atts )
{
    Q_UNUSED( atts );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "displayObjectStartElement" );

    if ( 2 == depth_ )
    {
        // start a new object
        if ( "DisplayObject" == qName )
        {
            LOG_DEBUG( "display object:", NULL );
            displayObject_.clear();
        }
        else
        {
            LOG_ERROR( "unknown element '%s'", qPrintable( qName ) );
            return false;
        }
    }
    else if ( 3 == depth_ )
    {
        // save off element name (for later insertion into map)
        elementName_ = qName;
    }
    else
    {
        LOG_ERROR( "bad depth %d!", depth_ );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::displayObjectEndElement(
    const QString& qName )
{
    Q_UNUSED( qName );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "displayObjectEndElement" );

    // layout of widget
    if ( 2 == depth_ )
    {
        QString name = displayObject_[ "Name" ];
        QWidget *widget = layout_.ui()->mainFrame->findChild<QWidget*>( name );

        // for main frame we need to get manually!
        if ( "mainFrame" == name )
            widget = layout_.ui()->mainFrame;
        // for main widget we need to get manually!
        else if ( "mainWidget" == name )
            widget = layout_.ui()->mainWidget;
        // for scrolling marquee we need to get manually!
        else if ( "marqueeLabel" == name )
            widget = layout_.ui()->marqueeLabel;

        // make sure we have valid widget
        if ( !widget )
        {
            LOG_ERROR( "user interface does not contain widget '%s'!", qPrintable( name ) );
            return false;
        }

        LOG_DEBUG( "processing properties for widget '%s'...", qPrintable( widget->objectName() ) );

        // iterate through properties
        for ( stringMapType::const_iterator i = displayObject_.begin(); i != displayObject_.end(); i++ )
        {
            if ( "Left" == i.key() )
            {
                QRect geometry = widget->geometry();
                geometry.moveLeft( i.value().toInt() );
                widget->setGeometry( geometry );
            }
            else if ( "Top" == i.key() )
            {
                QRect geometry = widget->geometry();
                geometry.moveTop( i.value().toInt() );
                widget->setGeometry( geometry );
            }
            else if ( "Width" == i.key() )
            {
                QRect geometry = widget->geometry();
                geometry.setWidth( i.value().toInt() );
                widget->setGeometry( geometry );
            }
            else if ( "Height" == i.key() )
            {
                QRect geometry = widget->geometry();
                geometry.setHeight( i.value().toInt() );
                widget->setGeometry( geometry );
            }
            else if ( "Visible" == i.key() )
            {
                widget->setVisible( 0 < i.value().toInt() ? true : false );
            }
            else if ( "BGColorRed" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.background().color();
                color.setRed( i.value().toInt() );
                palette.setColor( widget->backgroundRole(), color );
                widget->setPalette( palette );
                widget->setAutoFillBackground( true );

                // for slideshow page save off data
                if ( "videoPage" == name )
                    layout_.setPalette<gcdLayout::Slideshow>( color );
            }
            else if ( "BGColorGreen" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.background().color();
                color.setGreen( i.value().toInt() );
                palette.setColor( widget->backgroundRole(), color );
                widget->setPalette( palette );
                widget->setAutoFillBackground( true );

                // for slideshow page save off data
                if ( "videoPage" == name )
                    layout_.setPalette<gcdLayout::Slideshow>( color );
            }
            else if ( "BGColorBlue" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.background().color();
                color.setBlue( i.value().toInt() );
                palette.setColor( widget->backgroundRole(), color );
                widget->setPalette( palette );
                widget->setAutoFillBackground( true );

                // for slideshow page save off data
                if ( "videoPage" == name )
                    layout_.setPalette<gcdLayout::Slideshow>( color );
            }
            else if ( "BGBmp" == i.key() )
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "background bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    QPalette palette = widget->palette();
                    QPixmap image( filename );
                    palette.setBrush( widget->backgroundRole(), QBrush( image ) );
                    widget->setPalette( palette );
                    widget->setAutoFillBackground( true );

                    // for metrology page save off data
                    if ( "metrologyPage" == name )
                        layout_.setPalette<gcdLayout::MetrologyBackground>( filename );
                    // for slideshow page save off data
                    else if ( "videoPage" == name )
                        layout_.setPalette<gcdLayout::Slideshow>( filename );
                    // for store logo page save off data
                    else if ( "storeLogoLabel" == name )
                    {
                    	QSize size( widget->size() );

                    	if ( displayObject_.contains( "Width" ) )
                    		size.setWidth( displayObject_["Width"].toInt() );
                    	if ( displayObject_.contains( "Height" ) )
                    		size.setHeight( displayObject_["Height"].toInt() );

                        QPixmap pixmap( size );
                        pixmap.fill( Qt::transparent );

                        // scale image to widget size
                        QPixmap scaledImage( image.scaled( pixmap.size(), Qt::KeepAspectRatio ) );

                        QPoint center( 0, 0 );
                        center.setX( (pixmap.width() - scaledImage.width()) / 2 );
                        center.setY( (pixmap.height() - scaledImage.height()) / 2 );

                        // copy image to pixmap
                        QPainter painter( &pixmap );
                        painter.setRenderHint( QPainter::Antialiasing, true );
                        painter.drawPixmap( center, scaledImage );

                        // set pixmap to ui for our customers (and their customers) to adore and cherish
                        palette.setBrush( widget->backgroundRole(), QBrush( pixmap ) );
                        widget->setPalette( palette );

                        // save off the scaled pixmap
                        QDir tempFolder( QDir::tempPath() );
                        QString storeLogo( tempFolder.absoluteFilePath( "gcdStoreLogo.png" ) );

                        pixmap.save( storeLogo );

                        // set palette info
                        layout_.setPalette<gcdLayout::StoreLogo>( storeLogo );
                    }
                }
            }
            else if (( "BGBmpFS" == i.key() ) && ( "metrologyPage" == name ))
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "fs background bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    LOG_TRACE( "setting fs background to '%s'...", qPrintable( filename ) );

                    // for metrology page save off data
                    layout_.setPalette<gcdLayout::MetrologyFSBackground>( filename );
                }
            }
            else if (( "BGBmpFSOverlay" == i.key() ) && ( "metrologyPage" == name ))
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "fs background overlay bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    LOG_TRACE( "setting fs overlay to '%s'...", qPrintable( filename ) );

                    // for metrology page save off data
                    layout_.setPaletteOverlay<gcdLayout::MetrologyFSBackground>( filename );
                }
            }
            else if (( "BGBmpAdvertOverlay" == i.key() ) && ( "metrologyPage" == name ))
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "advert background overlay bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    LOG_TRACE( "setting advert overlay to '%s'...", qPrintable( filename ) );

                    // for metrology page save off data
                    layout_.setPaletteOverlay<gcdLayout::MetrologyBackgroundWithAdverts>( filename );
                }
            }
            else if (( "BGBmpOutOfService" == i.key() ) && ( "metrologyPage" == name ))
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "out of service bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    // for metrology page save off data
                    layout_.setPalette<gcdLayout::OutOfService>( filename );
                }
            }
            else if (( "BGBmpTestImage" == i.key() ) && ( "metrologyPage" == name ))
            {
                QString filename( filesystem::absoluteFilePath( i.value(), folderType::Layouts ) );

                if ( !QFile::exists( filename ) )
                    LOG_WARN( "test image bitmap '%s' does not exist!", qPrintable( filename ) );
                else
                {
                    // for metrology page save off data
                    layout_.setPalette<gcdLayout::TestImage1>( filename );
                }
            }
            else if ( "FontColorRed" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.foreground().color();
                color.setRed( i.value().toInt() );
                palette.setColor( widget->foregroundRole(), color );
                widget->setPalette( palette );
            }
            else if ( "FontColorGreen" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.foreground().color();
                color.setGreen( i.value().toInt() );
                palette.setColor( widget->foregroundRole(), color );
                widget->setPalette( palette );
            }
            else if ( "FontColorBlue" == i.key() )
            {
                QPalette palette = widget->palette();
                QColor color = palette.foreground().color();
                color.setBlue( i.value().toInt() );
                palette.setColor( widget->foregroundRole(), color );
                widget->setPalette( palette );
            }
            else if ( "Font" == i.key() )
            {
                QFont font = widget->font();
                font.setFamily( i.value() );
                // reset font styles (can explicitly set with FontStyle)
                font.setBold( false );
                font.setItalic( false );
                font.setOverline( false );
                font.setStrikeOut( false );
                font.setUnderline( false );
                widget->setFont( font );
            }
            else if ( "FontStyle" == i.key().left( 9 ) )
            {
                QFont font = widget->font();
                if ( "Bold" == i.value() )
                    font.setBold( true );
                else if ( "Italic" == i.value() )
                    font.setItalic( true );
                else if ( "Overline" == i.value() )
                    font.setOverline( true );
                else if ( "StrikeOut" == i.value() )
                    font.setStrikeOut( true );
                else if ( "Underline" == i.value() )
                    font.setUnderline( true );
                else
                    LOG_WARN( "unhandled font style '%s'!", qPrintable( i.value() ) );
                widget->setFont( font );
            }
            else if ( "FontPointSize" == i.key() )
            {
                QFont font = widget->font();
                font.setPointSize( i.value().toInt() );
                widget->setFont( font );
            }
            else if ( "FontPixelSize" == i.key() )
            {
                QFont font = widget->font();
                font.setPixelSize( i.value().toInt() );
                widget->setFont( font );
            }
            else if ( "HAlign" == i.key() )
            {
                QLabel *label = qobject_cast<QLabel*>( widget );
                if ( !label )
                    LOG_WARN( "widget '%s' does not appear to be a label!", qPrintable( name ) );
                else
                {
                    Qt::Alignment alignment = label->alignment();
                    if ( "Left" == i.value() )
                    {
                        alignment &= Qt::AlignVertical_Mask;
                        alignment |= Qt::AlignLeft;
                    }
                    else if ( "Center" == i.value() )
                    {
                        alignment &= Qt::AlignVertical_Mask;
                        alignment |= Qt::AlignHCenter;
                    }
                    else if ( "Right" == i.value() )
                    {
                        alignment &= Qt::AlignVertical_Mask;
                        alignment |= Qt::AlignRight;
                    }
                    else
                        LOG_WARN( "unhandled horizontal align '%s'!", qPrintable( i.value() ) );
                    label->setAlignment( alignment );
                }
            }
            else if ( "VAlign" == i.key() )
            {
                QLabel *label = qobject_cast<QLabel*>( widget );
                if ( !label )
                    LOG_WARN( "widget '%s' does not appear to be a label!", qPrintable( name ) );
                else
                {
                    Qt::Alignment alignment = label->alignment();
                    if ( "Top" == i.value() )
                    {
                        alignment &= Qt::AlignHorizontal_Mask;
                        alignment |= Qt::AlignTop;
                    }
                    else if ( "Center" == i.value() )
                    {
                        alignment &= Qt::AlignHorizontal_Mask;
                        alignment |= Qt::AlignVCenter;
                    }
                    else if ( "Bottom" == i.value() )
                    {
                        alignment &= Qt::AlignHorizontal_Mask;
                        alignment |= Qt::AlignBottom;
                    }
                    else
                        LOG_WARN( "unhandled vertical align '%s'!", qPrintable( i.value() ) );
                    label->setAlignment( alignment );
                }
            }
            else if ( "Text" == i.key() )
            {
                QLabel *label = qobject_cast<QLabel*>( widget );
                if ( !label )
                    LOG_WARN( "widget '%s' does not appear to be a label!", qPrintable( name ) );
                else
                {
                    label->setText( i.value() );
                }
            }
            else
            {
                if ( "Name" != i.key() )
                    LOG_WARN( "unhandled property '%s'!", qPrintable( i.key() ) );
            }

        } // for each property
    } // depth 1

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::overlayObjectStartElement(
    const QString& qName,
    const QXmlAttributes& atts )
{
    Q_UNUSED( atts );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "overlayObjectStartElement" );

    if ( 2 == depth_ )
    {
        // start a new object
        if ( "OverlayObject" == qName )
        {
            LOG_DEBUG( "display object:", NULL );
            overlayObject_.clear();
        }
        else
        {
            LOG_ERROR( "unknown element '%s'", qPrintable( qName ) );
            return false;
        }
    }
    else if ( 3 == depth_ )
    {
        // save off element name (for later insertion into map)
        elementName_ = qName;
    }
    else
    {
        LOG_ERROR( "bad depth %d!", depth_ );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcdLayoutFileParser::overlayObjectEndElement(
    const QString& qName )
{
    Q_UNUSED( qName );

    LOG_CLASS_INFO( "gcdLayoutFileParser", "overlayObjectEndElement" );

    // layout of widget
    if ( 2 == depth_ )
    {
        QString name = overlayObject_[ "Name" ];
        QRect rect;

        // retrieve rect
        if ( "nowServing" == name )
            rect = layout_.nowServingLocation();
        else
        {
            LOG_ERROR( "user interface does not contain overlay '%s'!", qPrintable( name ) );
            return false;
        }

        LOG_DEBUG( "processing properties for overlay '%s'...", qPrintable( name ) );

        // iterate through properties
        for ( stringMapType::const_iterator i = overlayObject_.begin(); i != overlayObject_.end(); i++ )
        {
            if ( "Left" == i.key() )
                rect.moveLeft( i.value().toInt() );
            else if ( "Top" == i.key() )
                rect.moveTop( i.value().toInt() );
            else if ( "Width" == i.key() )
                rect.setWidth( i.value().toInt() );
            else if ( "Height" == i.key() )
                rect.setHeight( i.value().toInt() );
            else
            {
                if ( "Name" != i.key() )
                    LOG_WARN( "unhandled property '%s'!", qPrintable( i.key() ) );
            }

        } // for each property

        // set rect
        if ( "nowServing" == name )
            layout_.setNowServingLocation( rect );

    } // depth 1

    return true;
}

