/**
 * @file labelformrenderer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labelformrenderer.h"
#include "labelformrendererimpl.h"

QColor labelFormRenderer::formColor_( Qt::black );
QColor labelFormRenderer::selectedFormColor_( Qt::red );
QColor labelFormRenderer::previewFormColor_( Qt::black );

QColor labelFormRenderer::backgroundColor_( Qt::white );
QColor labelFormRenderer::previewBackgroundColor_( Qt::transparent );

///////////////////////////////////////////////////////////////////////////////////////////////////
labelFormRenderer::labelFormRenderer( const LabelRenderer *labelRenderer, const labelForm *form ) :
    labelRenderer_( labelRenderer ),
    form_( form ),
    mode_( NORMAL ),
    rectPenStyle_( Qt::SolidLine ),
    text_( form->typeText() ),
    textFlags_( Qt::AlignTop | form_->alignment() )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelFormRenderer::~labelFormRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelFormRenderer::paint( QPainter *painter, bool selected )
{
    LOG_CLASS_INFO( "labelFormRenderer", "paint" );

    int keyInt;
    QString keyString;

    int column = 0;

    // save painter
    painter->save();

    // setup defaults
    painter->setPen( defaultPainterPen( mode(), selected ) );
    painter->setBrush( defaultPainterBrush( mode() ) );

    // for preview mode, use font setting from label form
    if ( PREVIEW == mode() )
    {
        QFont f( painter->font() );

        // set family name
        if ( form_->fontFamilyName().length() )
            f.setFamily( form_->fontFamilyName() );

        // set pixel size
        if ( 0 < form_->fontPointSize() )
        {
            // 72 points to an inch
            // 203 dots per inch (HLX is 8 dots/mm)
            f.setPixelSize( ((form_->fontPointSize() * 203) / 72) );
        }

        painter->setFont( f );
    }

    LOG_TRACE( "painting form of type %d mode %d", form_->type(), mode() );

    // call paint handler
    switch ( form_->type() )
    {
    // ------------------------------------------------------------------------
    case labelFormType::USER_LINE:
    case labelFormType::DIVIDING_LINE:

        paintImpl<LINE>( painter );
        break;

    // ------------------------------------------------------------------------
    case labelFormType::GENERIC_TEXT_1:
    case labelFormType::GENERIC_TEXT_2:
    case labelFormType::GENERIC_TEXT_3:
    case labelFormType::GENERIC_TEXT_4:
    case labelFormType::GENERIC_TEXT_5:
    case labelFormType::GENERIC_TEXT_6:
    case labelFormType::GENERIC_TEXT_7:
    case labelFormType::GENERIC_TEXT_8:
    case labelFormType::GENERIC_TEXT_9:
    case labelFormType::GENERIC_TEXT_10:
    case labelFormType::GENERIC_TEXT_11:
    case labelFormType::GENERIC_TEXT_12:
    case labelFormType::GENERIC_TEXT_13:
    case labelFormType::GENERIC_TEXT_14:
    case labelFormType::GENERIC_TEXT_15:
    case labelFormType::GENERIC_TEXT_16:
    case labelFormType::GENERIC_TEXT_17:
    case labelFormType::GENERIC_TEXT_18:
    case labelFormType::GENERIC_TEXT_19:
    case labelFormType::GENERIC_TEXT_20:

        keyInt = 0;

        if (( labelFormType::GENERIC_TEXT_1 <= form_->type() ) && ( form_->type() <= labelFormType::GENERIC_TEXT_10 ))
            keyInt = form_->type() - labelFormType::GENERIC_TEXT_1 + 1;
        else if (( labelFormType::GENERIC_TEXT_11 <= form_->type() ) && ( form_->type() <= labelFormType::GENERIC_TEXT_20 ))
            keyInt = form_->type() - labelFormType::GENERIC_TEXT_11 + 1;

        // get text
        if ( labelRenderer_->store() )
        {
            const dataObjectContainer<systemString> *c = labelRenderer_->store()->store<systemString>();

            if ( c->contains( keyInt ) )
            {
                text_ = c->value( keyInt ).text();
                rectPenStyle_ = Qt::DashLine;
            }
        }

        paintImpl<TEXT>( painter );
        break;

    // ------------------------------------------------------------------------
    case labelFormType::INDIRECT_LABEL_REFERENCE:

        // label number in vertical justification field
        keyInt = form_->verticalJustification();

        if ( labelRenderer_->store() )
        {
            const dataObjectContainer<label> *c = labelRenderer_->store()->store<label>();

            if ( c->contains( keyInt ) )
            {
                LabelRenderer renderer;

                renderer.setLabel( c->value( keyInt ) );
                renderer.setStore( labelRenderer_->store() );
                renderer.setNutrifactsId( labelRenderer_->nutrifactsId() );

                if ( PREVIEW == mode() )
                    renderer.setPreviewMode( true );

                image_ = renderer.toImage();
            }
        }

        // valid image
        if ( !image_.isNull() )
            paintImpl<GRAPHIC>( painter );

        // bad image, draw text instead
        else if ( NORMAL == mode() )
        {
            text_ = QString( "Label: %1" ).arg( form_->verticalJustification() );
            textFlags_ = Qt::AlignCenter;

            rectPenStyle_ = Qt::DashLine;

            paintImpl<TEXT>( painter );
        }

        break;

    // ------------------------------------------------------------------------
    case labelFormType::INDIRECT_GRAPHIC_REFERENCE:

        // graphic name stored in font family name field
        keyString = form_->fontFamilyName();

        if ( labelRenderer_->store() )
        {
            const dataObjectContainer<graphic> *c = labelRenderer_->store()->store<graphic>();

            if ( c->contains( keyString ) )
                image_ = c->value( keyString ).image();
        }

        // valid image
        if ( !image_.isNull() )
            paintImpl<GRAPHIC>( painter );

        // bad image, draw text instead
        else if ( NORMAL == mode() )
        {
            text_ = "Embedded Graphic: " + form_->fontFamilyName();
            textFlags_ = Qt::AlignCenter;

            rectPenStyle_ = Qt::DashLine;

            paintImpl<TEXT>( painter );
        }

        break;

    // ------------------------------------------------------------------------
    case labelFormType::NUTRIFACT_TEXT_1:
    case labelFormType::NUTRIFACT_TEXT_2:
    case labelFormType::CONSTANT_TEXT:

        // get text
        if ( form_->data().length() )
        {
            text_ = form_->data();
            rectPenStyle_ = Qt::DashLine;
        }

        paintImpl<TEXT>( painter );
        break;

    // ------------------------------------------------------------------------
    case labelFormType::NUTRI_COLUMN_1:
    case labelFormType::NUTRI_COLUMN_2:
    case labelFormType::NUTRI_COLUMN_3:
    case labelFormType::NUTRI_COLUMN_4:

        column = form_->type() - labelFormType::NUTRI_COLUMN_1;

        // drop down (do not break)!!

    // ------------------------------------------------------------------------
    case labelFormType::NUTRIFACT_VALUE_1:
    case labelFormType::NUTRIFACT_VALUE_2:
    case labelFormType::NUTRIFACT_VALUE_3:

        // Note:
        // Unless we came from above, the column parameter should be zero. These form types are
        // hard coded to column 1.

        // not in normal mode, blank text unless set explicitly
        if ( NORMAL != mode() )
            text_.clear();

        // retrieve nutrifacts object
        keyInt = labelRenderer_->nutrifactsId();

        if ( labelRenderer_->store() )
        {
            const dataObjectContainer<nutrifact> *c = labelRenderer_->store()->store<nutrifact>();

            if ( c->contains( keyInt ) )
            {
                nutrifact n( c->value( keyInt ) );

                // lookup nutrifact value
                if ( n.columns().contains( column ) )
                {
                    keyString = form_->tag();

                    if ( n.column( column ).contains( keyString ) )
                        text_ = n.column( column ).value( keyString );
                }
            }
        }

        paintImpl<TEXT>( painter );
        break;

    // ------------------------------------------------------------------------
    default:

        paintImpl<TEXT>( painter );
        break;
    }

    // restore painter
    painter->restore();
}
