/**
 * @file expandedtextdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "expandedtextdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QApplication>
#include <QPlainTextEdit>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ExpandedTextDialog::ExpandedTextDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( TopDialogLocation, parent, f )
{
    setObjectName( "ExpandedTextDialog" );

    // initialize interface
    initialize();

    connect( this, SIGNAL( active(bool) ), SLOT( onActive(bool) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ExpandedTextDialog::~ExpandedTextDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ExpandedTextDialog::text() const
{
    return text_->toPlainText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QFont ExpandedTextDialog::textFont() const
{
    return text_->font();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpandedTextDialog::setText( const QString& value )
{
    text_->setPlainText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpandedTextDialog::setTextFont( const QFont& value )
{
    text_->setFont( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ExpandedTextDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 470) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 380) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *ExpandedTextDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ExpandedTextDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpandedTextDialog::onActive( bool value )
{
    // on activation hide input widgets
    if ( value )
        if ( mainWindow_ )
            mainWindow_->hideInputWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpandedTextDialog::initialize()
{
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    // text
    text_ = new QPlainTextEdit( this );
    text_->setObjectName( "text" );
    screenLayout->addWidget( text_ );

    text_->setBackgroundVisible( false );
    text_->setFocusPolicy( Qt::NoFocus );
    text_->setFrameShape( QFrame::NoFrame );
    text_->setReadOnly( true );

    modifyPaletteBase setBackground( Qt::transparent );
    setBackground( text_ );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
    setForeground( text_ );

    modifyFontPointSize setFontPointSize( (mainWindow_->height() * 16) / SessionMainWindow::scalingDividerY_ );
    setFontPointSize( text_ );
}
