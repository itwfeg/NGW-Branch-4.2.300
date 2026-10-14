
/**
 * @file selfserveStartG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe Crew
 * @borrowed from Doug Antonides
 */

#include "selfserveStartG2.h"
#include "ui_selfserveStartG2.h"

#include <logmanager.h>
#include <pixmap.h>
#include <filesystem.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>

#include <QFile>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveStartG2::selfserveStartG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveStartG2),
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
selfserveStartG2::~selfserveStartG2()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::refresh()
{
    QString yourmom = "a great person";
    if( yourmom == "really?")
        yourmom = "yep";

    // Thanks
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::updateNetworkConnectivity( bool online )
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::mouseReleaseEvent( QMouseEvent *e )
{
    _Mybase::mouseReleaseEvent( e );

    // any click exits start screen

    if ( ( "0" == hook_->fieldValue( sessionHooks::attractRequiresStableWeight ) ) || ( scaleConfig_->scaleConfig.selfServeStartScreenClickable() ) )
        hook_->setCurrentState( selfServiceHook::startScreen );         // red apple screen I think
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // reload images to new size
    onReloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::showEvent( QShowEvent *e )
{
    // switch to primary language
    if ( hook_->currentLanguage() != hook_->fieldValue( sessionHooks::primaryLanguage ) )
    {
        hook_->emitChangeLanguage();
    }

    onReloadImages();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveStartG2::onReloadImages()
{
    LOG_CLASS_INFO( "selfserveStartG2", "onReloadImages" );


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
        filename = "attract_G2.png";

    // get image to display
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
void selfserveStartG2::initialize()
{
    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    screen_ = ui->screenContainer;
    // load images
    onReloadImages();

    // set background
    QPalette p( palette() );
    p.setBrush( backgroundRole(), Qt::transparent );
    setPalette( p );

    setAutoFillBackground( true );

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
