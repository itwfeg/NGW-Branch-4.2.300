
/**
 * @file selfserveLogin.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe Crew
 * @inspired by Doug Antonides
 */

#include "selfserveLogin.h"
#include "ui_selfserveLogin.h"

#include <logmanager.h>
#include <pixmap.h>
#include <filesystem.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>
#include <QDesktopWidget>
#include <ScaleClasses.h>
#include <QFile>
#include <QIntValidator>
#include <QMessageBox>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveLogin::selfserveLogin( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveLogin),
    hook_( h ),
    scaleConfig_( NULL )
{
    ui->setupUi(this);

    // This is the User Login screen.  If you want the Weight Required Screen, go to the
    // selfserveInstruct Screen. The Big Green GO Button can be found in selfserveStart Screen.

    initialize();

    connect( hook_,            SIGNAL( reloadImages() ),                   SLOT( onReloadImages() ) );

    connect( ui->logPad,       SIGNAL( textChanged(QString) ),      hook_, SLOT( resetIdleTimer() ) );
    connect( ui->logPad,       SIGNAL( textChanged(QString) ),             SLOT( onNumberPadTextChanged(QString) ) );
    connect( ui->logPad,       SIGNAL( selfServiceLogin(QString) ),        SLOT( onSelfServiceLogin(QString) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveLogin::~selfserveLogin()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::refresh()
{
    // update number pad style
    ui->logPad->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );

    // The key is an ENTER key(0) to do a PLU Lookup or a Print Key(1)
    ui->logPad->setEnterResponse(0);

    ui->logPad->setEnterText(0);            // Enter

    // update item code (maybe user ID????)
    // QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );

    QFont font;

    int screenSize = checkScreenSize();
    if(screenSize == 10)
        font.setPointSize( 36 );
    else
        font.setPointSize( 48 );

    font.setBold( true );

    ui->lblLogin->setFont( font );
    ui->lblLogin->setFont( font );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::translate()
{
    ui->retranslateUi(this);

    //ui->logPad->setInstructions( tr( "Enter Operator ID and press ENTER." ) );

 /*  HOLD OFF FOR NOW

    QString primaryText = "Enter ID to begin";

    if ( "EN.US" == primaryLang )
        primaryText = "Enter ID to begin";
    else if( "EN.US" == secondaryLang )
        secondaryText = "Enter ID to begin";

    if ( "ES.MX" == primaryLang )
        primaryText = "Ingrese el ID para comenzar";
    else if( "ES.MX" == secondaryLang )
        secondaryText = "Ingrese el ID para comenzar";

    if ( "FR.CA" == primaryLang )
        primaryText = "Entrez l’ID pour commencer";
    else if( "FR.CA" == secondaryLang )
        secondaryText = "Entrez l’ID pour commencer";

    //QString primaryText = "Enter ID to begin\nor Scan your Code";

    if ( "ES.MX" == primaryLang )
        primaryText = "Ingrese el ID para comenzar\no escanee su código";
    else if( "ES.MX" == secondaryLang )
        secondaryText = "Ingrese el ID para comenzar\no escanee su código";

    if ( "FR.CA" == primaryLang )
        primaryText = "Entrez l’ID pour commencer\nou scannez votre code";
    else if( "FR.CA" == secondaryLang )
        secondaryText = "Entrez l’ID pour commencer\nou scannez votre code";
*/

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // reload images to new size
    onReloadImages();

    // refresh numpad
    onReloadScreen();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "selfserveLogin", "showEvent" );

    ui->logPad->blockSignals( true );

    ui->logPad->clear();
    ui->logPad->setInstructions( tr( "Enter Operator ID and press ENTER." ) );

    // switch back to the primary language
    if ( hook_->currentLanguage() != hook_->fieldValue( sessionHooks::primaryLanguage ) )
    {
        LOG_INFO( "toggle language current %s desired %s",
            qPrintable( hook_->currentLanguage() ),
            qPrintable( hook_->fieldValue( sessionHooks::primaryLanguage ) ) );

        hook_->emitChangeLanguage();
    }

    onReloadScreen();
    onReloadImages();

    ui->logPad->blockSignals( false );

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::onReloadImages()
{
    LOG_CLASS_INFO( "selfserveLogin", "onReloadImages" );

    // set some generic text (for now)
    ui->lblLogin->setText( tr( "Enter your ID to begin" ) );
    ui->lblLogin->setAutoFillBackground( false );

    QString primaryLang( hook_->fieldValue( sessionHooks::primaryLanguage ) );
    QString secondaryLang( hook_->fieldValue( sessionHooks::secondaryLanguage ) );
    QString filename = "";

    if ( "EN.US" == primaryLang && "ES.MX" == secondaryLang )
        filename = "login_en_es.png";
    else if ( "EN.US" == primaryLang && "FR.CA" == secondaryLang )
        filename = "login_en_fr.png";
    else if ( "ES.MX" == primaryLang && "EN.US" == secondaryLang)
        filename = "login_es_en.png";
    else if ( "ES.MX" == primaryLang && "FR.CA" == secondaryLang)
        filename = "login_es_fr.png";
    else if ( "FR.CA" == primaryLang && "EN.US" == secondaryLang)
        filename = "login_fr_en.png";
    else if ( "FR.CA" == primaryLang && "ES.MX" == secondaryLang)
        filename = "login_fr_es.png";
    else
        filename = "login.png";

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
    sizeToFitPixmap stretcher( ui->lblLogin->size() );

    QPalette p( ui->lblLogin->palette() );
    p.setBrush( ui->lblLogin->backgroundRole(), stretcher( image ) );
    ui->lblLogin->setPalette( p );

    ui->lblLogin->clear();
    ui->lblLogin->setAutoFillBackground( true );

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::onReloadScreen()
{
    LOG_CLASS_INFO( "selfserveLogin", "onReloadScreen" );

    int screensize = checkScreenSize();

    int keyWidth = 318;
    int keyHeight = 480;

    if( screensize == 10 )
    {
        LOG_INFO( "Login Screen Size: 10", NULL );
        keyWidth = 268;
        keyHeight = 416;
    }

    //ui->logPad->setFixedSize( keyWidth, keyHeight + 70 );
    //ui->logPad->setFixedSize( keyWidth, keyHeight + 100 );
    ui->logPad->setFixedSize( keyWidth, keyHeight + 156 );

    translate();

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::onNumberPadTextChanged( const QString& text )
{
    // might not need this
    QString oper( text );

    if ( oper.isEmpty() )
        oper = "0";

    ui->logPad->setOperatorID( text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::onSelfServiceLogin( const QString& operId )
{
    QString oper( operId );

    if ( oper.isEmpty() )
        oper = "0";

     hook_->emitChangeFieldValue( sessionHooks::selfServiceOperatorLogin, oper );
     ui->logPad->clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveLogin::initialize()
{
    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    // load text
    onReloadScreen();

    // refresh images
    onReloadImages();

    ui->logPad->displayStackedWidget(0);
    ui->logPad->setValidator( new QIntValidator( 0, 999999999, this ) );

    // Tell the Enter key to look up operators
    ui->logPad->setEnterResponse( SelfServicePrintOption::Action_LookupOperator );
}

int selfserveLogin::checkScreenSize()
{
    // laptop (1920x1080)
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
