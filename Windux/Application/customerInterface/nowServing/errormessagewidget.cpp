/**
 * @file errormessagewidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "errormessagewidget.h"
#include "fontutil.h"
#include "pixmaputil.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
errorMessageWidget::errorMessageWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
errorMessageWidget::~errorMessageWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString errorMessageWidget::errorText()
{
    return errorText_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool errorMessageWidget::isErrorTextVisible() const
{
    return errorText_->isVisible();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::setErrorText( const QString& value )
{
    errorText_->setText( value );
    adjustFontPointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::setErrorTextVisible( bool value )
{
    errorText_->setVisible( value );
    adjustFontPointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::setText( const QString& value )
{
    text_->setText( value );
    adjustFontPointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString errorMessageWidget::text() const
{
    return text_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::resizeEvent( QResizeEvent *e )
{
    // adjust icon size
    icon_->setFixedSize( height() / 3, height() / 3 );

    // adjust font sizes
    adjustFontPointSize();

    // adjust pixmap sizes
    QPalette p;

    p = icon_->palette();
    p.setBrush( icon_->backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/warningIcon.png" ), icon_->size(), Qt::KeepAspectRatio, Qt::transparent ) );
    icon_->setPalette( p );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::initialize()
{
    QPalette p;

    // logo
    icon_ = new QWidget( this );
    icon_->setAutoFillBackground( true );

    // text
    text_ = new QLabel( this );
    text_->setAlignment( Qt::AlignCenter );

    p = text_->palette();
    p.setBrush( text_->foregroundRole(), Qt::black );
    text_->setPalette( p );

    // error text
    errorText_ = new QLabel( this );
    errorText_->setAlignment( Qt::AlignCenter );

    p = errorText_->palette();
    p.setBrush( errorText_->foregroundRole(), Qt::black );
    errorText_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::createLayout()
{
    if ( layout() )
        delete layout();

    QVBoxLayout *iconLayout( new QVBoxLayout );
    iconLayout->addStretch();
    iconLayout->addWidget( icon_ );
    iconLayout->addStretch();

    QHBoxLayout *textLayout( new QHBoxLayout );
    textLayout->addStretch( 1 );
    textLayout->addLayout( iconLayout );
    textLayout->addWidget( text_, 2 );
    textLayout->addStretch( 1 );

    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->addStretch();
    formLayout->addLayout( textLayout );
    formLayout->addStretch();
    formLayout->addWidget( errorText_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorMessageWidget::adjustFontPointSize()
{
    QFont f( font() );

    // text
    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( text_->width(), height() / 3 ), text_->text() ) );
    text_->setFont( f );

    // error text
    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( errorText_->width(), height() / 16 ), errorText_->text() ) );
    errorText_->setFont( f );
}
