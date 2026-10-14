/**
 * @file refusebin.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "refusebin.h"
#include "softkeybuttonlist.h"

#include "backgroundwidgettypes.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
refuseBinWidget::refuseBinWidget( QWidget *parent ) :
    _Mybase( parent )
{
    setObjectName( "refuseBinWidget" );

    // enable dropping
    setAcceptDrops( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
refuseBinWidget::~refuseBinWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::setIcon( const QPixmap& value )
{
    icon_ = value;
    updatePaletteBackground();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::dragEnterEvent( QDragEnterEvent *e )
{
    LOG_CLASS_INFO( "refuseBinWidget", "dragEnterEvent" );

    // sanity check
    if ( !e )
        return;

    LOG_TRACE( "object dragged into widget 0x%08x", e->source() );

    bool accept;
    bool reject;

    // check mime type
    accept = true;
    reject = false;

    if ( acceptedMimeTypes_.size() )
    {
        accept = false;

        foreach ( const QString& mimeType, acceptedMimeTypes_ )
            accept |= e->mimeData()->hasFormat( mimeType );
    }

    foreach ( const QString& mimeType, rejectedMimeTypes_ )
        reject |= e->mimeData()->hasFormat( mimeType );

    if (( !accept ) || ( reject ))
    {
        LOG_INFO( "bad mime type, rejecting", NULL );

        e->ignore();
        return;
    }

    // check source
    accept = true;
    reject = false;

    if ( acceptedSources_.size() )
    {
        accept = false;

        foreach ( const QObject *obj, acceptedSources_ )
        {
            QWidget *source( dynamic_cast<QWidget*>( e->source() ) );
            int parents = 2;

            while (( source ) && ( 0 <= parents-- ))
            {
                LOG_TRACE( "looking for accepted source '%s'", qPrintable( source->objectName() ) );

                accept |= (obj == source);
                source = source->parentWidget();
            }
        }
    }

    foreach ( const QObject *obj, rejectedSources_ )
    {
        QWidget *source( dynamic_cast<QWidget*>( e->source() ) );
        int parents = 2;

        while (( source ) && ( 0 <= parents-- ))
        {
            LOG_TRACE( "looking for rejected source '%s'", qPrintable( source->objectName() ) );

            reject |= (obj == source);
            source = source->parentWidget();
        }
    }

    if (( !accept ) || ( reject ))
    {
        LOG_INFO( "bad source widget, rejecting", NULL );

        e->ignore();
        return;
    }

    LOG_DEBUG( "valid object 0x%08x", e->source() );
    e->acceptProposedAction();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::dragMoveEvent( QDragMoveEvent *e )
{
    // sanity check
    if ( !e )
        return;

    // set the action and accept
    e->setDropAction( Qt::MoveAction );
    e->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::dropEvent( QDropEvent *e )
{
    LOG_CLASS_INFO( "refuseBinWidget", "dropEvent" );

    // sanity check
    if ( !e )
        return;

    LOG_INFO( "handle object drop 0x%08x", e->source() );

    // emit signal
    QWidget *source( dynamic_cast<QWidget*>( e->source() ) );

    if ( source )
    {
        Q_EMIT deleteWidget( source );
    }

    LOG_DEBUG( "done", NULL );

    // set the action and accept
    e->setDropAction( Qt::MoveAction );
    e->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    updatePaletteBackground();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void refuseBinWidget::updatePaletteBackground()
{
    if ( icon_.isNull() )
        setAutoFillBackground( false );
    else
    {
        QPixmap background( size() );
        background.fill( Qt::transparent );

        QPainter p( &background );
        p.setBrush( hobart::colorPalette::white );
        p.setPen( hobart::colorPalette::gray );
        p.setRenderHint( QPainter::Antialiasing, true );

        QSize scaledSize( icon_.size() );
        scaledSize.scale( size(), Qt::KeepAspectRatio );

        QPoint pos( (width() - scaledSize.width()) / 2, (height() - scaledSize.height()) / 2 );

        p.drawRoundedRect( rect(), 5.0, 5.0 );
        p.drawPixmap( QRect( pos, scaledSize ), icon_ );

        QPalette newPalette( palette() );
        newPalette.setBrush( backgroundRole(), background );

        setAutoFillBackground( true );
        setPalette( newPalette );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace



