/**
 * @file operatemodeexpanelex.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "operatemodepanelex.h"

#include "../sessiondialog.h"
#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

/// Renderer traits type.
typedef hobart::operateModeExPanelBackgroundWidget rendererTraitsType;

const bool OperateModePanelEx::convertUppercase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModePanelEx::OperateModePanelEx( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    setFixedSize( sizeHint() );

    // initialize interface
    initialize();
    createLayout();

    // while an exended operate panel is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModePanelEx::~OperateModePanelEx()
{
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanelEx::setTitle( const QString& value )
{
    title_->setText( convertUppercase_ ? value.toUpper() : value );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize OperateModePanelEx::sizeHint() const
{
    QSize s;

    // compute size
    s.setWidth( (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_ );
    s.setHeight( (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );

    // adjust width by dialog offset to take up entire screen
    const int offset( parentDialog_->dialogOffset() );

    s.rwidth() += ((mainWindow_->width() * offset) / SessionMainWindow::scalingDividerX_) - offset;

    // give result to caller
    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString OperateModePanelEx::title() const
{
    return title_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *OperateModePanelEx::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *OperateModePanelEx::rendererTraits() const
{
    return new rendererTraitsType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int OperateModePanelEx::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanelEx::initialize()
{
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );

    // set palette
    setForeground( this );

    modifyFontPointSize setFontPointSize14( (mainWindow_->height() * 14) / SessionMainWindow::scalingDividerY_ );

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );

    setFontPointSize14( title_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanelEx::createLayout()
{
    // create layouts
    screenLayout_ = new QVBoxLayout( this );
    screenLayout_->setContentsMargins( QMargins( 7, 15, 15, 15 ) );
    screenLayout_->setSpacing( 0 );

    screenLayoutInner_ = new QHBoxLayout();
    screenLayoutInner_->setContentsMargins( QMargins() );
    screenLayoutInner_->setSpacing( 0 );

    widgetsLayout_ = new QVBoxLayout();
    widgetsLayout_->setContentsMargins( QMargins() );
    widgetsLayout_->setSpacing( 0 );

    topLayout_ = new QHBoxLayout();
    topLayout_->setContentsMargins( QMargins() );
    topLayout_->setSpacing( 0 );

    topLayoutInner_ = new QVBoxLayout();
    topLayoutInner_->setContentsMargins( QMargins() );
    topLayoutInner_->setSpacing( 0 );

    bottomLayout_ = new QHBoxLayout();
    bottomLayout_->setContentsMargins( QMargins() );
    bottomLayout_->setSpacing( 0 );

    bottomLayoutInner_ = new QVBoxLayout();
    bottomLayoutInner_->setContentsMargins( QMargins() );
    bottomLayoutInner_->setSpacing( 0 );

    // nest layouts
    screenLayout_->addLayout( screenLayoutInner_ );
    screenLayoutInner_->addLayout( widgetsLayout_ );
    widgetsLayout_->addLayout( topLayout_ );
    widgetsLayout_->addStretch();
    widgetsLayout_->addLayout( bottomLayout_ );
    topLayout_->addLayout( topLayoutInner_ );
    topLayout_->addStretch();
    bottomLayout_->addLayout( bottomLayoutInner_ );

    // create widgets
    topLayoutInner_->addWidget( title_ );
}
