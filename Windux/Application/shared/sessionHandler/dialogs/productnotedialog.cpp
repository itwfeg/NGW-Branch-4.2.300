/**
 * @file productnotedialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "productnotedialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

const bool ProductNoteDialog::convertUpperCase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductNoteDialog::ProductNoteDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    allowOverlayDismiss_( true )
{
    setObjectName( "ProductNoteDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductNoteDialog::~ProductNoteDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDialogButtonBox::ButtonRole ProductNoteDialog::buttonRole( QAbstractButton *button ) const
{
    return buttons_->buttonRole( button );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QAbstractButton*>	ProductNoteDialog::buttons() const
{
    return buttons_->buttons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ProductNoteDialog::text() const
{
    return text_->toPlainText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ProductNoteDialog::title() const
{
    return title_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNoteDialog::setText( const QString& value )
{
    text_->setPlainText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNoteDialog::setTitle( const QString& value )
{
    title_->setText( convertUpperCase_ ? value.toUpper() : value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *ProductNoteDialog::addButton( QDialogButtonBox::StandardButton button )
{
    QPushButton *b( buttons_->addButton( button ) );
    groomPushButton( b ); // FT-595

    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *ProductNoteDialog::addButton( const QString& text, QDialogButtonBox::ButtonRole role )
{
    QPushButton *b( buttons_->addButton( text, role ) );
    groomPushButton( b ); // FT-595

    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNoteDialog::onButtonClicked( QAbstractButton *button )
{
    Q_UNUSED( button );

    // Note:
    // This will need to do something eventually, especially for Yes/No buttons.
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNoteDialog::initialize()
{
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteBackground setBackground( hobart::colorPalette::white );

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );
    setFontPointSize18( title_ );

    // text
    text_ = new QPlainTextEdit( this );
    text_->setFocusPolicy( Qt::NoFocus ); // FT-821
    text_->setObjectName( "text" );
    text_->setReadOnly( true );
    text_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    setBackground( text_ );

    // buttons
    buttons_ = new QDialogButtonBox( this );
    buttons_->setObjectName( "buttons" );

    connect( buttons_, SIGNAL( clicked(QAbstractButton*) ), SLOT( onButtonClicked(QAbstractButton*) ) );

    // set layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    QHBoxLayout *hlayout;

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( title_ );
    hlayout->addStretch();

    screenLayout->addLayout( hlayout );
    screenLayout->addWidget( text_ );
    screenLayout->addWidget( buttons_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNoteDialog::groomPushButton( QPushButton *button )
{
    if ( !button )
        return;

    // FT-595 set button properties
    button->setAutoDefault( false );
    button->setDefault( false );
    button->setFocusPolicy( Qt::NoFocus );
}
