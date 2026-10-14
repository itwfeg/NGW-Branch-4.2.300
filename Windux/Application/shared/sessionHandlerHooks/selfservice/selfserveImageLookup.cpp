/**
 * @file selfserveImageLookup.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveImageLookup.h"
#include "ui_selfserveImageLookup.h"

#include "selfservicesoftkeyswidget.h"

#include <filesystem.h>
#include <selfservicehook.h>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookup::selfserveImageLookup( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveImageLookup),
    hook_( h )
{
    ui->setupUi(this);

    initialize();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookup::~selfserveImageLookup()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    softkeys_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::refresh()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::translate()
{
    ui->retranslateUi(this);

    softkeys_->translate();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::hideEvent( QHideEvent *e )
{
    // reset softkeys state
    softkeys_->resetState();

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::showEvent( QShowEvent *e )
{
    // reset softkeys state
    softkeys_->resetState();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::onReloadImages()
{
    softkeys_->reloadImages();
    softkeys_->setStoreLogo( hook_->storeLogo() );

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookup::initialize()
{
    softkeys_ = new hobart::selfServiceSoftkeysWidget( hook_->session(), hook_, this );
    connect( softkeys_, SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    layout()->addWidget( softkeys_ );

    // load images
    onReloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
