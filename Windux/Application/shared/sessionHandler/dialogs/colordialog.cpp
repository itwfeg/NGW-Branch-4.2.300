/**
 * @file colordialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "colordialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ColorDialog::ColorDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "ColorDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ColorDialog::~ColorDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ColorDialog::setOptions( QColorDialog::ColorDialogOptions value )
{
    d_->setOptions( value | QColorDialog::DontUseNativeDialog );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ColorDialog::hideEvent( QHideEvent *e )
{
    // hide input widgets
    mainWindow_->hideInputWidgets();

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ColorDialog::initialize()
{
    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize12( this );

    // create widgets
    d_ = new QColorDialog( this );
    d_->setObjectName( "d" );
    d_->setOptions( QColorDialog::DontUseNativeDialog );

    Qt::WindowFlags f( d_->windowFlags() );
    f ^= (f & Qt::WindowType_Mask);
    d_->setWindowFlags( f );

    connect( d_, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( d_, SIGNAL( finished(int) ), SLOT( done(int) ) );
    connect( d_, SIGNAL( rejected() ), SLOT( reject() ) );

    // create layouts    
    QHBoxLayout *hlayout = new QHBoxLayout( this );
    hlayout->addStretch();
    hlayout->addWidget( d_, 1 );
    hlayout->addStretch();

    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addStretch();
    screenLayout->addLayout( hlayout, 1 );
    screenLayout->addStretch();
}
