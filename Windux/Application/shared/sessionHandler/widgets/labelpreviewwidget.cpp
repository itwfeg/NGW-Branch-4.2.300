/**
 * @file labelpreviewwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelpreviewwidget.h"

#include <font.h>
#include <labelrenderer.h>
#include <logmanager.h>

#include <QPainter>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labelPreviewWidget::labelPreviewWidget( QWidget *parent ) :
    _Mybase( parent ),
    renderer_( NULL )
{
    setObjectName( "hobartLabelPreviewWidget" );

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelPreviewWidget::labelPreviewWidget( LabelRenderer *renderer, QWidget *parent ) :
    _Mybase( parent ),
    renderer_( renderer )
{
    setObjectName( "hobartLabelPreviewWidget" );

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelPreviewWidget::~labelPreviewWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelPreviewWidget::setReadOnly( bool value )
{
    Q_UNUSED( value );

    // nothing to do here, we are always read only
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelPreviewWidget::setRenderer( LabelRenderer *value )
{
    if ( (renderer_ = value) )
    {
        modifyFontPointSize setFontPointSize10( (window()->height() * 10) / 600 );
        setFontPointSize10( renderer_ );

        renderer_->setPreviewMode( true );
    }

    render();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelPreviewWidget::render()
{
    if ( renderer_ )
        label_ = renderer_->toImage();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelPreviewWidget::mouseReleaseEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "labelPreviewWidget", "mouseReleaseEvent" );

    if ( !label_.isNull() )
    {
        // emit clicked signal
        Q_EMIT clicked();
    }

    _Mybase::mouseReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelPreviewWidget::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    QPainter painter( this );
    painter.fillRect( rect(), Qt::white );

    if ( label_.isNull() )
        return;

    QSize labelSize( label_.size() );
    labelSize.scale( size(), Qt::KeepAspectRatio );

    QRect rect(
        (width() - labelSize.width()) / 2,
        (height() - labelSize.height()) / 2,
        labelSize.width(),
        labelSize.height() );

    // set antialiasing
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawImage( rect, label_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


