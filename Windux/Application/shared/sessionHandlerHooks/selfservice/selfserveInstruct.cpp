/**
 * @file selfserveInstruct.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "selfserveInstruct.h"
#include "ui_selfserveInstruct.h"

#include <filesystem.h>
#include <logmanager.h>
#include <pixmap.h>
#include <selfservicehook.h>

#include <ScaleConfiguration.h>

#include <QFile>
#include <QTime>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveInstruct::selfserveInstruct( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveInstruct),
    hook_( h ),
    scaleConfig_( NULL )
{
    ui->setupUi(this);

    // This is the Weight Required Screen - 3 rotating images
    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveInstruct::~selfserveInstruct()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::refresh()
{
    // update welcome message
    const QString attractMessage( hook_->fieldValue( sessionHooks::attractMessage ) );

    ui->lblWelcomeMessage->setText( attractMessage );

    // update image
    onUpdateImage();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::translate()
{
    ui->retranslateUi(this);

    // update welcome message
    const QString attractMessage( hook_->fieldValue( sessionHooks::attractMessage ) );

    ui->lblWelcomeMessage->setText( attractMessage );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::mouseReleaseEvent( QMouseEvent *e )
{
    _Mybase::mouseReleaseEvent( e );

    // pjr383
    if( scaleConfig_->scaleConfig.selfServeStartScreenClickable() )
    {
        // the screen where you can click a flashkey and/or use keypad depending on configuration
        hook_->setCurrentState( selfServiceHook::itemEntryKeyed );
    }
    else if ( "0" == hook_->fieldValue( sessionHooks::attractRequiresStableWeight ) )
    {
        // red apple screen I think
        hook_->setCurrentState( selfServiceHook::startScreen );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::resizeEvent( QResizeEvent *e )
{
    onUpdateImage( true );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::showEvent( QShowEvent *e )
{
    onUpdateImage( true );

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::onReloadImages()
{
    QPixmap logo( hook_->storeLogo() );

    // set store logo
    if ( logo.isNull() )
        ui->lblStoreLogo->hide();
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

    // load attract images
    images_.clear();

    Session *s = hook_->session();
    MadpScaleConfig *settings = &s->scaleConfig->scaleConfig;
    QString lang = settings->languageCode().toUpper() + "." + settings->countryCode().toUpper();

    QString image_lang;
    if ( "ES.MX" == lang )
        image_lang = "_es-mx";
    else if ( "FR.CA" == lang )
        image_lang = "_fr-ca";
    else if ( "EN.US" == lang )
        image_lang = "_en-us";

    QString filename( filesystem::absoluteFilePath( ( "attract1" + image_lang + ".png" ), folderType::SelfServeImages ) );
    if ( QFile::exists( filename ) )
    {
        images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( ( "attract2" + image_lang + ".png" ), folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( ( "attract3" + image_lang + ".png" ), folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );
    }
    else
    {
        filename = filesystem::absoluteFilePath( "attract.png", folderType::SelfServeImages );
        images_.append( QPixmap( filename ) );
    }

    onUpdateImage( true );

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::onUpdateImage( bool force )
{
    const QTime now( QTime::currentTime() );

    // sanity check
    if ( !images_.size() )
        return;

    // compute slideshow speed
    int speed( hook_->fieldValue( sessionHooks::attractSlideshowSpeed ).toInt() );

    if ( speed <= 0 )
        speed = 5;

    speed *= 1000;

    // what image should we show
    const int index( (now.msecsSinceStartOfDay() / speed) % images_.size() );

    if ( !force )
        if ( imageIndex_ == index )
            return;

    imageIndex_ = index;

    // generate image
    QPixmap pixmap( ui->instructionArea->size() );
    pixmap.fill( Qt::transparent );

    QPixmap scaledImage( images_[index].scaled( pixmap.size(), Qt::KeepAspectRatio ) );

    QPoint p( (pixmap.width() - scaledImage.width()) / 2, (pixmap.height() - scaledImage.height()) / 2 );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawPixmap( p, scaledImage );

    // set image
    QPalette palette( ui->instructionArea->palette() );
    palette.setBrush( ui->instructionArea->backgroundRole(), pixmap );
    ui->instructionArea->setPalette( palette );

    // show!
    ui->instructionArea->setAutoFillBackground( true );
    ui->instructionArea->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstruct::initialize()
{
    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    // load images
    onReloadImages();

    // setup timer
    connect( &timer_, SIGNAL(timeout()), SLOT(onUpdateImage()) );

    timer_.setSingleShot( false );
    timer_.setInterval( 1000 );
    timer_.start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
