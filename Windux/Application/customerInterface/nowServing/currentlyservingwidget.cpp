/**
 * @file currentlyservingwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "currentlyservingwidget.h"
#include "queuestatewidget.h"

#include <playlistwidget.h>

#include <QHBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
currentlyServingWidget::currentlyServingWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
currentlyServingWidget::~currentlyServingWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString currentlyServingWidget::playlist() const
{
    return adverts_->currentPlaylist();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::setNowServingText( const QString &value )
{
    queueState_->setNowServingText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::setPlaylist( const QString& value )
{
    adverts_->setCurrentPlaylist( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::setQueueSizeText( const QString &value )
{
    queueState_->setQueueSizeText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::setShowQueueSize( bool value )
{
    // hide/show queue size
    queueState_->setShowQueueSize( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool currentlyServingWidget::showQueueSize() const
{
    return queueState_->showQueueSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::translate()
{
    queueState_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::initialize()
{
    QPalette p;

    setAutoFillBackground( true );

    // queue state
    queueState_ = new queueStateWidget( this );

    // adverts
    adverts_ = new playlistWidget( this );
    adverts_->setAutoFillBackground( true );
    adverts_->setDisplay( playlistWidget::Operator );
    adverts_->setImageBackground( Qt::transparent );
    adverts_->setVideoBackground( Qt::black );

    p = adverts_->palette();
    p.setBrush( adverts_->backgroundRole(), Qt::transparent );
    adverts_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void currentlyServingWidget::createLayout()
{
    QHBoxLayout *formLayout( new QHBoxLayout( this ) );
    formLayout->setContentsMargins( QMargins() );
    formLayout->setSpacing( 0 );
    formLayout->addWidget( queueState_, 1 );
    formLayout->addWidget( adverts_, 2 );
}
