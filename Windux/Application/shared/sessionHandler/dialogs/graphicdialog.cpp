/**
 * @file graphicdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "graphicdialog.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Session.h>
#include <Messages.h>

#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicDialog::GraphicDialog( const QPixmap& pixmap, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    showCustomerDisplay_( false ),
    customerDisplayVisible_( false ),
    graphic_( NULL ),
    pixmap_( pixmap )
{
    // set object name
    setObjectName( "GraphicDialog" );

    // initialize interface
    initialize();

    // display pixmap
    updateGraphic( pixmap_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicDialog::GraphicDialog( const QString& filename, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    showCustomerDisplay_( false ),
    customerDisplayVisible_( false ),
    graphic_( NULL ),
    pixmap_( graphicFilename_ = filename )
{
    // set object name
    setObjectName( "GraphicDialog" );

    // initialize interface
    initialize();

    // display pixmap
    updateGraphic( pixmap_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
GraphicDialog::~GraphicDialog()
{
    // remove gcd image
    if ( customerDisplayVisible_ )
    {
        session_->send( GCDSessionDev, GcdSessionColorTestOff );
        customerDisplayVisible_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicDialog::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );

    // remove gcd image
    if ( customerDisplayVisible_ )
    {
        session_->send( GCDSessionDev, GcdSessionColorTestOff );
        customerDisplayVisible_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicDialog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // display pixmap
    updateGraphic( pixmap_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicDialog::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    // show gcd image
    if ( showCustomerDisplay_ )
        if (( graphicFilename_.length() ) && ( QFile::exists( graphicFilename_ ) ))
        {
            session_->send( GCDSessionDev, GcdSessionColorTestOn, graphicFilename_ );
            customerDisplayVisible_ = true;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicDialog::initialize()
{
    // create widgets
    graphic_ = new QWidget( this );
    graphic_->setAutoFillBackground( true );
    graphic_->setObjectName( "graphic" );
    graphic_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->addWidget( graphic_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicDialog::updateGraphic( const QPixmap& value )
{
    // sanity check
    if ( !graphic_ )
        return;

    // target size
    const QSize targetSize( graphic_->size() );

    // create pixmap
    QPixmap pixmap( targetSize );
    pixmap.fill( Qt::transparent );

    // handle source
    if ( !value.isNull() )
    {
        const QSize sourceSize( value.size() );

        // scale source
        QSize scaledSourceSize( sourceSize );
        scaledSourceSize.scale( targetSize, Qt::KeepAspectRatio );

        QRect rect(
            (targetSize.width() - scaledSourceSize.width()) / 2,
            (targetSize.height() - scaledSourceSize.height()) / 2,
            scaledSourceSize.width(),
            scaledSourceSize.height() );

        // create painter
        QPainter painter( &pixmap );
        painter.setRenderHint( QPainter::Antialiasing, true );

        // draw pixmap
        painter.drawPixmap( rect, value );
    }

    // set background
    modifyPaletteBackground setBackgroundPalette( pixmap );
    setBackgroundPalette( graphic_ );
}
