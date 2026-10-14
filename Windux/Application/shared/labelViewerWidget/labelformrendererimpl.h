/**
 * @file labelformrendererimpl.h
 * Renderer implementation.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELFORMRENDERERIMPL_H
#define LABELFORMRENDERERIMPL_H

#include "labelformrenderer.h"

#include "labelrenderer.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This is the default renderer. It paints a hatch pattern.
 */
template <>
inline void labelFormRenderer::paintImpl<labelFormRenderer::DEFAULT>( QPainter *painter ) const
{
    // not in normal mode, do nothing
    if ( NORMAL != mode() )
        return;

    // draw hatch pattern
    painter->fillRect( form_->rect(), Qt::BDiagPattern );

    // draw rectangle around pattern
    painter->setBrush( QBrush( Qt::transparent ) );
    painter->drawRect( form_->rect() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Renderer for text.
 */
template <>
inline void labelFormRenderer::paintImpl<labelFormRenderer::TEXT>( QPainter *painter ) const
{
    QPen pen( painter->pen() );

    // draw rectangle
    if ( NORMAL == mode() )
    {
        pen.setStyle( rectPenStyle_ );
        painter->setPen( pen );
        painter->drawRect( form_->rect() );
    }
    else if (( PREVIEW == mode() ) && ( labelFormFlags::RECT_BORDER & form_->flags() ))
    {
        painter->setPen( Qt::SolidLine );
        painter->drawRect( form_->rect() );
    }

    // HT-497
    if ( labelFormFlags::INVERTED & form_->flags() )
    {
        //-------------------------------------
        // Print white text on black background
        //-------------------------------------
        painter->fillRect( form_->rect(), Qt::black);
        pen.setColor( Qt::white );
    }

    pen.setStyle( Qt::SolidLine );
    painter->setPen( pen );

    // draw text
    painter->drawText( form_->rect(), textFlags_, text_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Renderer for line objects.
 */
template <>
inline void labelFormRenderer::paintImpl<labelFormRenderer::LINE>( QPainter *painter ) const
{
    QRect rect = form_->rect();

    // line width is stored in sub type... so grab it and adjust the rect size
    int width = form_->subType();

    // horizontal line
    if ( 0 == rect.height() )
        rect.setHeight( width );
    // vertical line
    else if ( 0 == rect.width() )
        rect.setWidth( width );

    painter->fillRect( rect, Qt::SolidPattern );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Renderer for graphic images.
 */
template <>
inline void labelFormRenderer::paintImpl<labelFormRenderer::GRAPHIC>( QPainter *painter ) const
{
    QPen pen( painter->pen() );

    // draw image
    painter->drawImage( form_->rect(), image_ );

    // draw rectangle around image
    //  - for normal mode, only if selected
    //  - for preview mode, only if border flag set
    if ((( NORMAL == mode() ) && ( selectedFormColor_ == pen.color() )) ||
        (( PREVIEW == mode() ) && ( labelFormFlags::RECT_BORDER & form_->flags() )))
    {
        painter->setBrush( QBrush( Qt::transparent ) );
        painter->drawRect( form_->rect() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELFORMRENDERERIMPL_H
