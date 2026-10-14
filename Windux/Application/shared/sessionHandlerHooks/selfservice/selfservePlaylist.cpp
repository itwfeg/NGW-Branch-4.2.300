/**
 * @file selfservePlaylist.cpp
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Job. E Crew
 */

#include "selfservePlaylist.h"
#include "ui_selfservePlaylist.h"

#include <filesystem.h>
#include <logmanager.h>
#include <pixmap.h>
#include <selfservicehook.h>


#include <../playlistWidget/playlistwidget.h>

#include <ScaleConfiguration.h>
#include <QDir>
#include <QFile>
#include <QTime>

#include <QHBoxLayout>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfservePlaylist::selfservePlaylist( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfservePlaylist),
    hook_( h ),
    scaleConfig_( NULL )
{
    ui->setupUi(this);

    initialize();

    translate();

    // in the future maybe someone else needs to do this, like call it when printing is finished.
    // Because right now, this is happening at boot and if the playlist changes then thats a reboot

    Session *s = hook_->session();
    MadpScaleConfig *settings = &s->scaleConfig->scaleConfig;
    setPlaylist( settings->selfServePlaylistFile() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfservePlaylist::~selfservePlaylist()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::refresh()
{
    // update welcome message
    // future considerations incase they want title message and logo
    //const QString attractMessage( tr("Welcome Shoppers") );
    //ui->lblWelcomeMessage->setText( attractMessage );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::translate()
{
    ui->retranslateUi(this);

    // future considerations incase they want title message and logo
    //const QString attractMessage( tr("Welcome Shoppers") );
    //ui->lblWelcomeMessage->setText( attractMessage );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::mouseReleaseEvent( QMouseEvent *e )
{
    _Mybase::mouseReleaseEvent( e );

    // any click exits start screen
    if ( "0" == hook_->fieldValue( sessionHooks::attractRequiresStableWeight ) )
        hook_->setCurrentState( selfServiceHook::startScreen );
    // pjr383 - added
    else
        hook_->setCurrentState( selfServiceHook::itemEntryKeyed );     // enter PLU or click flashkeys
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::initialize()
{
    // future considerations incase they want title message and logo
    // load logo
    // loadLogo();

    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    // playlist area

    // adverts
    QPalette p;
    setAutoFillBackground( true );

    adverts_ = new playlistWidget( this );

    adverts_->setAutoFillBackground( true );
    adverts_->setDisplay( playlistWidget::Operator );
    adverts_->setImageBackground( Qt::transparent );
    adverts_->setVideoBackground( Qt::black );

    p = adverts_->palette();
    p.setBrush( adverts_->backgroundRole(), Qt::transparent );
    adverts_->setPalette( p );

    // now add the advert to the layout
    ui->layPlaylist->setContentsMargins( QMargins() );
    ui->layPlaylist->setSpacing( 0 );
    ui->layPlaylist->addWidget( adverts_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::loadLogo()
{
    // future considerations incase they want title message and logo
    /*
    QPixmap logo( hook_->storeLogo() );

    // set store logo
    if ( logo.isNull() )
    {
        ui->lblStoreLogo->hide();
    }
    else
    {
        // set size
        ui->lblStoreLogo->setFixedSize( logo.size() );

        // set image
        QPalette palette( ui->lblStoreLogo->palette() );
        palette.setBrush( ui->lblStoreLogo->backgroundRole(), logo );
        ui->lblStoreLogo->setPalette( palette );

        // show!
        ui->lblStoreLogo->setAutoFillBackground( true );
        ui->lblStoreLogo->show();
    }
    */

    // refresh ui
    update();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfservePlaylist::playlist() const
{
    return adverts_->currentPlaylist();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfservePlaylist::setPlaylist( const QString& value )
{
    adverts_->setCurrentPlaylist( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
