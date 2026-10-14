/**
 * @file panelwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "panelwidget.h"

#include "../sessionmainwindow.h"
#include "../sessiondialog.h"

#include <backgroundwidgettypes.h>
#include <palette.h>
#include <pixmap.h>

#include <QApplication>

const int PanelWidget::fontPointSize_ = 10;

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget::PanelWidget( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( parent ),
    mainWindow_( mainWindow )
{
    // set parent dialog
    parentDialog_ = parent;

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    // set palette
    setForeground( this );

    // set font
    QFont f( font() );
    f.setPointSize( (mainWindow_->height() * fontPointSize_) / SessionMainWindow::scalingDividerY_ );
    setFont( f );

    // retrieve mode and session
    mode_ = mainWindow_->mode();
    session_ = mainWindow_->session();

    // connect into session
    connect( session_, SIGNAL( receive(SessionMessage) ), this, SLOT( onSessionMessageReceive(SessionMessage) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget::~PanelWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::accept()
{
    if ( parentDialog_ )
        parentDialog_->accept();
    else
        close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::done( int r )
{
    if ( parentDialog_ )
        parentDialog_->done( r );
    else
        close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::reject()
{
    if ( parentDialog_ )
        parentDialog_->reject();
    else
        close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    QPainter p( this );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.drawPixmap( 0, 0, b_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::resizeEvent( QResizeEvent *e )
{
    const hobart::backgroundWidgetRenderer renderer( rendererTraits(), rendererAppearance() );

    // render background
    b_ = renderer.drawControl( size(), (hobart::rendererState) rendererStyle() );

    // reverse background image when docked on right
    if ( RightDialogLocation == parentDialog_->location() )
    {
        flipPixmap flip( true, false );

        b_ = flip( b_ );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *PanelWidget::rendererAppearance() const
{
    return new hobart::transparentBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *PanelWidget::rendererTraits() const
{
    return new hobart::rectangleBackgroundWidget();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PanelWidget::rendererStyle() const
{
    return hobart::Normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::onSessionMessage( const SessionMessage &msg )
{
    Q_UNUSED( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PanelWidget::onSessionMessageReceive( const SessionMessage &msg )
{
    // call derived class method
    onSessionMessage( msg );
}
