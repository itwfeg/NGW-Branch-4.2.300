/**
 * @file wrappermodewidget.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "wrappermodewidget.h"

#include <backgroundwidgettypes.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
#include <QPair>
#include <QVBoxLayout>

/// Hobart namespace.
namespace hobart {

// the actual widget extends a little too far and overlaps the border... this offset is used to
// correct that overlap.
const int wrapperModeWidget::offsetAmount_( 4 );

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperModeWidget::wrapperModeWidget( QWidget *parent ) :
    _Mybase( parent ),
    flags_( (indicatorFlags) -1 ),
    readOnly_( true )
{
    setAutoFillBackground( true );
    setObjectName( "wrapperModeWidget" );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // initialize
    initialize();
    createLayout();
    translate();

    setIndicators( None );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperModeWidget::~wrapperModeWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::setIndicators( indicatorFlags value )
{
    if ( value == flags_ )
        return;

    // update indicator states
    updateIndicator( apply_, (value & ApplyHidden), (value & Apply) );
    updateIndicator( label_, (value & LabelHidden), (value & Label) );
    updateIndicator( wrap_, (value & WrapHidden), (value & Wrap) );

    flags_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::setReadOnly( bool value )
{
    // nothing to do
    if ( value == readOnly_ )
        return;

    readOnly_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::translate()
{
    apply_->setText( " " + tr( "A" ) + " " );
    wrap_->setText( " " + tr( "W" ) + " " );
    label_->setText( " " + tr( "L" ) + " " );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::mousePressEvent( QMouseEvent *e )
{
    // accept event
    e->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::mouseReleaseEvent( QMouseEvent *e )
{
    // check for left button release, emit pressed event when set
    if ( Qt::LeftButton & e->button() )
        if ( !isReadOnly() )
            Q_EMIT pressed( flags_ );

    // accept event
    e->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::resizeEvent( QResizeEvent *e )
{
    QRect buttonRect( rect() );
    buttonRect.adjust( offsetAmount_, 0, 0, -offsetAmount_ );

    const backgroundWidgetRenderer renderer(
        new panelButtonBackgroundWidget( 5 ),
        new darkButtonBackgroundWidgetRendererAppearance );

    // draw widget background
    QPixmap pixmap( size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawPixmap( buttonRect, renderer.drawControl( buttonRect.size(), ReadOnly ) );

    // set widget palette
    QPalette p( palette() );
    p.setBrush( backgroundRole(), pixmap );
    setPalette( p );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::initialize()
{
    icon_ = new QLabel( this );
    icon_->setObjectName( "icon" );
    icon_->setPixmap( QPixmap( ":/images/panels/gearIcon.png" ) );

    apply_ = new QLabel( this );
    apply_->setObjectName( "apply" );

    wrap_ = new QLabel( this );
    wrap_->setObjectName( "wrap" );

    label_ = new QLabel( this );
    label_->setObjectName( "label" );

    // ---- //

    QList<QLabel*> labels;
    labels.append( apply_ );
    labels.append( wrap_ );
    labels.append( label_ );

    foreach ( QLabel *l, labels )
    {
        l->setAlignment( Qt::AlignCenter );
        l->setAutoFillBackground( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::createLayout()
{
    QHBoxLayout *icon( new QHBoxLayout() );
    icon->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    icon->setSpacing( 0 );

    icon->addStretch();
    icon->addWidget( icon_ );
    icon->addStretch();

    QVBoxLayout *labels( new QVBoxLayout() );
    labels->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    labels->setSpacing( 0 );

    labels->addWidget( apply_ );
    labels->addWidget( wrap_ );
    labels->addWidget( label_ );

    QHBoxLayout *screenLayout( new QHBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 3, 3, 3, 3 + offsetAmount_ ) );
    screenLayout->setSpacing( 0 );

    screenLayout->addLayout( icon );
    screenLayout->addLayout( labels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperModeWidget::updateIndicator( QLabel *l, bool isHidden, bool isEnabled )
{
    QPalette p( l->palette() );

    if ( isHidden )
    {
        p.setBrush( l->backgroundRole(), Qt::transparent );
        p.setBrush( l->foregroundRole(), Qt::transparent );
    }
    else
    {
        p.setBrush( l->backgroundRole(), isEnabled ? hobart::colorPalette::green : hobart::colorPalette::red );
        p.setBrush( l->foregroundRole(), Qt::black );
    }

    l->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace



