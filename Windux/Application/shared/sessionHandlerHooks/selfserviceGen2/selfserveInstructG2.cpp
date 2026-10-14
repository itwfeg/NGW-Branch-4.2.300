/**
 * @file selfserveInstructG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe Crew
 * @borrowedfrom Randy Blankley
 */

#include "selfserveInstructG2.h"
#include "ui_selfserveInstructG2.h"

#include <filesystem.h>
#include <logmanager.h>
#include <pixmap.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsOpacityEffect>

#include <QFile>
#include <QTime>
#include <QFont>

// Hobart namespace
namespace hobart {

static const int image_width = 1366;
static const int image_height = 766;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveInstructG2::selfserveInstructG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveInstructG2),
    hook_( h ),
    scaleConfig_( NULL ),
    curSeparator_( "." )
{
    ui->setupUi(this);

    // This is the Weight Required Screen - 3 rotating images
    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveInstructG2::~selfserveInstructG2()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::refresh()
{
    // update image
    onUpdateImage();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::refreshWeight()
{
    // update all text and weight

    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );   // valid weight

    bool isMetric( "1" == hook_->fieldValue( sessionHooks::metricScale ) );
    int factor = weigherConfig_->weigherScaleFactor();

    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );

    // set and highlight the capacity range
    if( "1" == hook_->fieldValue( sessionHooks::capacityIndicator ) )
    {
        capacityHighRange_->setVisible( true );
        capacityHighRange_->setStyleSheet("background: rgb(55,60,75); border-radius: 9px; color: rgb(255,255,255);" );   // show capacity
    }
    else
    {
        //capacityHighRange_->setVisible( false );
        capacityHighRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255);");                      // hide capacity
    }

    if( "1" == hook_->fieldValue( sessionHooks::dualCapacityIndicator ) )
    {
       capacityLowRange_->setStyleSheet("background: rgb(55,60,75); border-radius: 9px; color: rgb(255,255,255);" );    // show capacity
       capacityLowRange_->setVisible( true );
    }
    else
    {
        if( !isMetric && ( 1 == factor ) )
            capacityLowRange_->setVisible( false );
        else
            capacityLowRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255); ");                 // hide capacity
    }

    // hide/show zero
    zero_->setVisible( "1" == hook_->fieldValue( sessionHooks::zeroIndicator ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250,0);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0,128);" ) );

    update();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::resizeEvent( QResizeEvent *e )
{
    onUpdateImage( true );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::showEvent( QShowEvent *e )
{
    bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );

    QFont ssfont;
    ssfont.setFamily( "Poppins" );
    ssfont.setBold( true );
    if( metricScale )
    {
        ssfont.setPointSize( 36 );
        weight_->setFont( ssfont );
        weight_->move( 20,33 );
    }
    else
    {
        ssfont.setPointSize( 42 );
        weight_->setFont( ssfont );
        weight_->move( 20,36 );
    }

    refreshWeight();

    onUpdateImage( true );

    if ( hook_->currentLanguage() != hook_->fieldValue( sessionHooks::primaryLanguage ) )
        hook_->emitChangeLanguage();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::onReloadImages()
{
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

    QString filename( filesystem::absoluteFilePath( ( "attract1" + image_lang + "_G2.png" ), folderType::SelfServeImages ) );
    if ( QFile::exists( filename ) )
    {
        images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( ( "attract2" + image_lang + "_G2.png" ), folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( ( "attract3" + image_lang + "_G2.png" ), folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );
    }
    else
    {
        filename = filesystem::absoluteFilePath( "attract1_G2.png", folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( "attract2_G2.png", folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );

        filename = filesystem::absoluteFilePath( "attract3_G2.png", folderType::SelfServeImages );
        if ( QFile::exists( filename ) )
            images_.append( QPixmap( filename ) );
    }

    onUpdateImage( true );

    // refresh ui
    update();

    refreshWeight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::onUpdateImage( bool force )
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
    //QPixmap pixmap( ui->screenContainer->size() );
    QPixmap pixmap( image_width, image_height );

    pixmap.fill( Qt::transparent );

    //QPixmap scaledImage( images_[index].scaled( pixmap.size(), Qt::KeepAspectRatio ) );
    QPixmap scaledImage( images_[index].scaled( image_width, image_height, Qt::KeepAspectRatio ) );
    QPoint p( 0, 0 );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawPixmap( p, scaledImage );

    // set image
    QPalette palette( ui->screenContainer->palette() );
    palette.setBrush( ui->screenContainer->backgroundRole(), pixmap );
    ui->screenContainer->setPalette( palette );

    // show!
    ui->screenContainer->setAutoFillBackground( true );
    ui->screenContainer->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::mouseReleaseEvent( QMouseEvent *e )
{
    _Mybase::mouseReleaseEvent( e );

    int clickX = e->globalX();
    int clickY = e->globalY();
    int screen_h = getScreenHeight();

    // See if the user clicked capacity and allow a leeway
    int wb_w = ui->wgtWeightBox->geometry().width();

#ifdef QT_DEBUG
    screen_h = 768;
#endif

    if( ( clickX <= wb_w ) && ( clickY < screen_h ) && ( clickY >= ( screen_h - 40 ) ) )
    {
        return;             // dont want go to main flashkey screen if we are firing off weights and measures
    }

    // the screen where you can click a flashkey and/or use keypad depending on configuration
    hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "selfserveInstructG2", "mousePressEvent" );
    LOG_DEBUG( "Got a mousePressEvent on selfserveInstructG2 Screen", NULL );

    // this is just for the weight field, the capacity string is handled over in the selfserviceweightbar area

    int clickX = e->globalX();
    int clickY = e->globalY();
    int screen_h = getScreenHeight();

    // See if the user clicked capacity and allow a leeway
    int wb_w = ui->wgtWeightBox->geometry().width();

    LOG_DEBUG( "Click location: (%d, %d)", clickX, clickY );
    LOG_DEBUG( "Screen height: %d", screen_h);
    LOG_DEBUG( "Weight box width: %d", wb_w );

#ifdef QT_DEBUG
    screen_h = 768;
    LOG_DEBUG( "QT_DEBUG screen_h NOW = 768", NULL );
#endif

    LOG_DEBUG( "Is clickX (%d) <= wb_w (%d)?", clickX, wb_w );
    LOG_DEBUG( "Is clickY (%d) < screen_h (%d)?", clickY, screen_h );
    LOG_DEBUG( "Is clickY (%d) >= screen_h - 40 (%d)?", clickY, screen_h - 40 );

    if( ( clickX <= wb_w ) && ( clickY < screen_h ) && ( clickY >= ( screen_h - 40 ) ) )
    {
        LOG_DEBUG( "Met all the rules - POP Weights and Measures", NULL );

        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );
    }
    else
        LOG_DEBUG( "Messed something up not popping W&M", NULL );

    LOG_DEBUG( "END mousePressedEvent()", NULL );

    _Mybase::mousePressEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveInstructG2::getScreenHeight()
{
    int screen_height = 768;        // 14 inch since thats what we are starting with

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        screen_height = resolution_.height();
    }


    // No idea why this is not working but it is not and it returns 1366
    // in the the width from the function. Come back to it later but now, this is 768 period

#ifdef QT_DEBUG
    screen_height = 768;
#endif

    return screen_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveInstructG2::initialize()
{
    if( !scaleConfig_ )
    {
        Session *s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    weigherConfig_  = &scaleConfig_->weigherConfigRead;

    MadpCurrencySettings *currencySettings( &scaleConfig_->currencySettings );
    if( currencySettings->unitSeparator().length() > 0 )
        curSeparator_ = currencySettings->unitSeparator();

    weight_            = ui->lblWeight;
    capacityLabel_     = ui->lblCapacity;
    zero_              = ui->lblZero;
    capacityLowRange_  = ui->lblLowRange;
    capacityHighRange_ = ui->lblHighRange;

    screen_             = ui->screenContainer;
    // load images
    onReloadImages();

    // setup timer
    connect( &timer_, SIGNAL(timeout()), SLOT(onUpdateImage()) );

    timer_.setSingleShot( false );
    timer_.setInterval( 1000 );
    timer_.start();

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
