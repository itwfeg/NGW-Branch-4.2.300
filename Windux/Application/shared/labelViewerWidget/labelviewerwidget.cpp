/**
 * @file labelviewerwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labelviewerwidget.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelViewerWidget::LabelViewerWidget( bool readOnly, QWidget *parent ) :
    _Mybase( parent ),
    renderer_( readOnly, this )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    setBackgroundRole( QPalette::Dark );
    setWidget( &renderer_ );

    connect( &renderer_, SIGNAL( labelChanged() ), this, SLOT( onLabelChanged() ) );
    connect( &renderer_, SIGNAL( labelFormSelected() ), this, SLOT( onLabelFormSelected() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelViewerWidget::~LabelViewerWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelViewerWidget::onLabelChanged()
{
    Q_EMIT labelChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelViewerWidget::onLabelFormSelected()
{
    Q_EMIT labelFormSelected();
}
