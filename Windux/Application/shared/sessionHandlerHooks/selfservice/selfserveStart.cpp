
/**
 * @file selfserveStart.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveStart.h"
#include "ui_selfserveStart.h"

#include <logmanager.h>
#include <pixmap.h>
#include <filesystem.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>

#include <QFile>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveStart::selfserveStart( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveStart),
    hook_( h ),
    scaleConfig_( NULL )
{
    ui->setupUi(this);

    // This is the big green button screen.  If you want the Weight Required Screen, go to the selfserveInstruct Screen
    // If you want the login screen, go to the SelfServeLogin screen.

    initialize();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveStart::~selfserveStart()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::refresh()
{
    QString yourmom = "a great person";
    if( yourmom == "really?")
        yourmom = "yep";

    // Thanks
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::mouseReleaseEvent( QMouseEvent *e )
{
    _Mybase::mouseReleaseEvent( e );

    // any click exits start screen
    // jira383
    //if ( ( "0" == hook_->fieldValue( sessionHooks::attractRequiresStableWeight ) ) || ( "1" == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) ) )
    if ( ( "0" == hook_->fieldValue( sessionHooks::attractRequiresStableWeight ) ) || ( scaleConfig_->scaleConfig.selfServeStartScreenClickable() ) )
        hook_->setCurrentState( selfServiceHook::startScreen );         // red apple screen I think
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // reload images to new size
    onReloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "selfserveStart", "showEvent" );

    // switch to primary language
    if ( hook_->currentLanguage() != hook_->fieldValue( sessionHooks::primaryLanguage ) )
    {
        LOG_INFO( "toggle language current %s desired %s",
            qPrintable( hook_->currentLanguage() ),
            qPrintable( hook_->fieldValue( sessionHooks::primaryLanguage ) ) );

        hook_->emitChangeLanguage();
    }

    onReloadImages();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::onReloadImages()
{
    LOG_CLASS_INFO( "selfserveStart", "onReloadImages" );

    // set some generic text (for now)
    ui->lblAttract->setText( tr( "Touch here to begin" ) );
    ui->lblAttract->setAutoFillBackground( false );

    QString primaryLang( hook_->fieldValue( sessionHooks::primaryLanguage ) );
    QString secondaryLang( hook_->fieldValue( sessionHooks::secondaryLanguage ) );
    QString filename = "";

    if ( "EN.US" == primaryLang && "ES.MX" == secondaryLang )
        filename = "start_en_es.png";
    else if ( "EN.US" == primaryLang && "FR.CA" == secondaryLang )
        filename = "start_en_fr.png";
    else if ( "ES.MX" == primaryLang && "EN.US" == secondaryLang)
        filename = "start_es_en.png";
    else if ( "ES.MX" == primaryLang && "FR.CA" == secondaryLang)
        filename = "start_es_fr.png";
    else if ( "FR.CA" == primaryLang && "EN.US" == secondaryLang)
        filename = "start_fr_en.png";
    else if ( "FR.CA" == primaryLang && "ES.MX" == secondaryLang)
        filename = "start_fr_es.png";
    else
        filename = "attract.png";

    // get image to display
    //QString imageName( filesystem::absoluteFilePath( "attract.png", folderType::SelfServeImages ) );
    QString imageName( filesystem::absoluteFilePath( filename, folderType::SelfServeImages ) );

    // attract screen image does not exist!!
    if ( !QFile::exists( imageName ) )
    {
        LOG_WARN( "attract screen image '%s' missing!", qPrintable( imageName ) );
        return;
    }

    QPixmap image( imageName );

    // attract screen image missing or corrupt
    if ( image.isNull() )
    {
        LOG_WARN( "attract screen image '%s' corrupt!", qPrintable( imageName ) );
        return;
    }

    // set background
    sizeToFitPixmap stretcher( ui->lblAttract->size() );

    QPalette p( ui->lblAttract->palette() );
    p.setBrush( ui->lblAttract->backgroundRole(), stretcher( image ) );
    ui->lblAttract->setPalette( p );

    ui->lblAttract->clear();
    ui->lblAttract->setAutoFillBackground( true );

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStart::initialize()
{
    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    // load images
    onReloadImages();

    // set background
    QPalette p( palette() );
    p.setBrush( backgroundRole(), Qt::transparent );
    setPalette( p );

    setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
