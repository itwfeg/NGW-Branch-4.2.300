/**
 * @file selfserveFinish.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveFinish.h"
#include "ui_selfserveFinish.h"

#include <filesystem.h>
#include <logmanager.h>
#include <pixmap.h>
#include <selfservicehook.h>
#include <softkeyswidget.h>

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <videoplayer/videoplayer.h>

#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QMovie>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveFinish::selfserveFinish( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveFinish),
    hook_( h ),
    scaleConfig_( NULL ),
    active_( false ),
    waitingForStart_( false ),
    playbacks_( 0 )
{
    ui->setupUi(this);

    // get media filename
    mediaFile_ = filesystem::absoluteFilePath( "takeLabel.gif", folderType::SelfServeImages );

    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveFinish::~selfserveFinish()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::refresh()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::hideEvent( QHideEvent *e )
{
    LOG_CLASS_INFO( "selfserveFinish", "hideEvent" );

    // pjr383 - use original test
    // X - pjr383
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() > SelfServicePrintOption::SelfServOption_PrintToLabel )
    {
        LOG_DEBUG( "hideEvent() beginPrintOrScanTimer_ STOP Timer", NULL );

        // if image
        beginPrintOrScanTimer_->stop();
    }

    LOG_DEBUG( "hideEvent() destroy player...", NULL );

    if ( active_ )
    {
        LOG_DEBUG( "hideEvent() actually destroy it...", NULL );
        gifPlayer_->stop();
        gifViewer_->setVisible( false );
        active_ = false;
    }

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "selfserveFinish", "resizeEvent" );

    // pjr383 - why is this if only if we are displaying Amazon Go label to the screen?
    // if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    // test1 FAIL: if ( ( 1 == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) || ( 2 == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    // test2: ongoing
    // test3 if( 1 == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
    if (  1 == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors()  && !scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
    {
        LOG_DEBUG( "resizeEvent() rendering label image to scan the label...", NULL );

        renderLabelImage();
    }
    else
    {
        // hide label image

    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "selfserveFinish", "showEvent" );


    LOG_DEBUG( "showEvent() START", NULL );

    //
    // display text on screen
    //

    static const QString thankYou_en_US( QString::fromUtf8( "Retrieve and Apply Label" ) );
    static const QString thankYou_es_MX( QString::fromUtf8( "Recuperar y aplicar la etiqueta" ) );
    static const QString thankYou_fr_CA( QString::fromUtf8( "Récupérer et appliquer l'étiquette" ) );

    QString scanLabel_en_US;
    QString scanLabel_es_MX;
    QString scanLabel_fr_CA;

    if ( ( SelfServicePrintOption::SelfServOption_PrintToScreen == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    {
        // Amazon GO - print entire label on 10071 to the screen for scan
        scanLabel_en_US = QString::fromUtf8( "SCAN label and Click DONE when finished." );
        scanLabel_es_MX = QString::fromUtf8( "ENCANEE la etiqueta y oprima FINALIZAR." );
        scanLabel_fr_CA = QString::fromUtf8( "SCANNEZ l'étiquette et cliquez sur TERMINÉ." );
    }
    else if( ( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    {
        // print entire label on 10071 to the screen and add a print button to print
        scanLabel_en_US = QString::fromUtf8( "Please SCAN or PRINT the label." );
        scanLabel_es_MX = QString::fromUtf8( "Por favor escanee o imprima la etiqueta." );
        scanLabel_fr_CA = QString::fromUtf8( "Veuillez scanner ou imprimer l'étiquette." );
    }

    QString primaryLang( hook_->fieldValue( sessionHooks::primaryLanguage ) );
    QString secondaryLang( hook_->fieldValue( sessionHooks::secondaryLanguage ) );

    if (( scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() || scaleConfig_->scaleConfig.selfServeEnablePlaylists() ) && ( "ES.MX" == primaryLang ))
    {
        // For Self Serv Chile we only want to see Spanish Text, period
        ui->lblThankYouPrimary->setText( thankYou_es_MX );
        ui->lblThankYouSecondary->setText( "" );
    }
    else
    {
        // check if current lang secondary
        if ( hook_->currentLanguage() == secondaryLang )
            primaryLang.swap( secondaryLang );

        if( scaleConfig_->normalScaleOperation.selfServeDisplayBarcodeLabel() || ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() >= SelfServicePrintOption::SelfServOption_PrintToLabel ) )
        {

            if( scaleConfig_->normalScaleOperation.selfServeDisplayBarcodeLabel() || scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen || ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == SelfServicePrintOption::SelfServOption_PrintToLabel ) )
            {
                // SOF-4431 Self-Serve: Incorrect message after printing label
                // Always need to be aware that this screen is used twice.  Once to display the label that can be scanned, and then again if you press print.
                // The IF here being true, means we are printing

                // set primary text
                if ( "ES.MX" == primaryLang )
                    ui->lblThankYouPrimary->setText( thankYou_es_MX );
                else if ( "FR.CA" == primaryLang )
                    ui->lblThankYouPrimary->setText( thankYou_fr_CA );
                else
                    ui->lblThankYouPrimary->setText( thankYou_en_US );

                // set secondary text
                if ( primaryLang == secondaryLang )
                    ui->lblThankYouSecondary->setText( "" );
                else if ( "ES.MX" == secondaryLang )
                    ui->lblThankYouSecondary->setText( thankYou_es_MX );
                else if ( "FR.CA" == secondaryLang )
                     ui->lblThankYouSecondary->setText( thankYou_fr_CA );
                else
                     ui->lblThankYouSecondary->setText( thankYou_en_US );
            }
            else
            {
                // The ELSE here being true, means we are scannin or printing

                // set primary text
                if ( "ES.MX" == primaryLang )
                    ui->lblThankYouPrimary->setText( scanLabel_es_MX );
                else if ( "FR.CA" == primaryLang )
                    ui->lblThankYouPrimary->setText( scanLabel_fr_CA );
                else
                    ui->lblThankYouPrimary->setText( scanLabel_en_US );

                // set secondary text
                if ( primaryLang == secondaryLang )
                    ui->lblThankYouSecondary->setText( "" );
                else if ( "ES.MX" == secondaryLang )
                    ui->lblThankYouSecondary->setText( scanLabel_es_MX );
                else if ( "FR.CA" == secondaryLang )
                     ui->lblThankYouSecondary->setText( scanLabel_fr_CA );
                else
                     ui->lblThankYouSecondary->setText( scanLabel_en_US );
            }
        }
    }

    //
    // display graphic on screen
    //

    if( scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
    {
        LOG_DEBUG( "showEvent() IF clickedPrintLabelFromFinishScreen true set up video", NULL );

        // need to reinitialize some stuff if we are going to display the video
        ui->takeLabelArea->setFixedSize( 640, 360 );

        QPalette labelAreaPalette( ui->takeLabelArea->palette() );
        labelAreaPalette.setColor( ui->takeLabelArea->backgroundRole(), Qt::black );
        labelAreaPalette.setColor( ui->takeLabelArea->foregroundRole(), Qt::black );
        ui->takeLabelArea->setPalette( labelAreaPalette );
    }

    // pjr383
    if (  ( ( SelfServicePrintOption::SelfServOption_PrintToScreen == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) || ( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )  && !scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
    {
        LOG_DEBUG( "showEvent() IF 1 rendering label image to scan the label......", NULL );

        renderLabelImage();
    }
    // pjr383 - drop weight required and playlist test options
    else if (  SelfServicePrintOption::SelfServOption_AfterPrintImage == scaleConfig_->scaleConfig.selfServeAfterPrintScreenState() )
    {
        LOG_DEBUG( "showEvent() IF ELSE 2 render pull - show the static image (weight req and playlists) ...", NULL );

        renderPull();
    }
    else
    {
        LOG_DEBUG( "showEvent() ELSE 3 create gifViewer_ player..with x,y: %dx%d", ui->takeLabelArea->x(),ui->takeLabelArea->y() );

        // for some reason, the ui->takeLabelArea->x(), ui->takeLabelArea->y() values are off on 14 inch when scanning label and printing is turned on. No idea why.

        int screensize = checkScreenSize();

        // activate video
        active_ = true;

        // Got these values from debut that was working correctly

        if( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        {

            if ( screensize > 10 )
                gifViewer_->setGeometry( 363, 208, 640, 360 );
            else
                gifViewer_->setGeometry( 192, 124 - 20, 640, 360 );       // original wrong printing(no idea where from): 102, 88
        }
        else
        {
            // Else with barcode/scan ON: create player..with x,y: 192x124

            gifViewer_->setGeometry( ui->takeLabelArea->x(), ui->takeLabelArea->y() - 20, 640, 360 );
        }

        gifViewer_->setVisible( true );
        gifPlayer_->start();

        // only want to clear GCD if we are playing a video
        //hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );     //hook_->session()->send( ProductDev, ProductRead, "0" );
    }

    // pjr383 - use original test
    // reset to false
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() > SelfServicePrintOption::SelfServOption_PrintToLabel ) )
    {
        LOG_DEBUG( "showEvent() IF 1 resetStartOverTimer()", NULL );

        scaleConfig_->internalConfig.selfServicePreventClicks = false;

        // SOF-4007 Big Y Self-Serve: Add timeout from the Print to Screen page
        resetStartOverTimer();

        // pjr383 - keep test as is
        // The default action is to display the default 10070 label type to the screen. This action will allow printing
        // a label assigned to a PLU after displaying the default 10070 label. Only do this if NOT printing the PLU label
        if( !scaleConfig_->scaleConfig.selfServeDisplayPluLabel() && ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel ) )
        {
            LOG_DEBUG( "showEvent() IF 2 resetStartOverTimer()", NULL );

            LOG_DEBUG( "Display default label - print PLU label", NULL );
            if( !scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg)
            {
                LOG_DEBUG( "showEvent() IF 3 resetStartOverTimer()", NULL );
                scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg = true;            // because it tries to send it twice because we reload the screen twice
                hook_->session()->send( SessionDev, SnSelfServReimageLabels );                  // now reimage labels so we can print them, in case we need to print them
            }
            else
            {
                 LOG_DEBUG( "showEvent() ELSE 1 resetStartOverTimer()", NULL );
                scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg = false;
            }
        }
    }

    _Mybase::showEvent( e );
}

/* PJR-383: Removed because using gif image instead of mp4 video
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::beginVideo()
{
    LOG_CLASS_INFO( "selfserveFinish", "beginVideo" );

    // sanity check
    if ( !active_ )
        return;
    else if ( !player_ )
        return;
    else if ( !QFile::exists( mediaFile_ ) )
        return;

    // play!
    LOG_DEBUG( "playing video...", NULL );

    waitingForStart_ = true;
    player_->load( mediaFile_ );
    //gifViewer_->setMovie(gifPlayer_);
    waitingForStart_ = false;

    if ( needToDestroyPlayer_ )
        destroyVideoPlayer();

    LOG_DEBUG( "done", NULL );
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::onReloadImages()
{
    QPixmap logo( hook_->storeLogo() );

    // set store logo
    if ( logo.isNull() )
        storeLogo_->hide();
    else
    {
        // set size
        storeLogo_->setFixedSize( logo.size() );

        // set image
        QPalette palette( storeLogo_->palette() );
        palette.setBrush( storeLogo_->backgroundRole(), logo );
        storeLogo_->setPalette( palette );

        // show!
        storeLogo_->setAutoFillBackground( true );
        storeLogo_->show();
    }

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::playbackError( const QString& desc )
{
    LOG_CLASS_INFO( "selfserveFinish", "playbackError" );

    LOG_WARN( "have playback error '%s'", qPrintable( desc ) );
}

/* PJR-383: Removed because using gif image instead of mp4 video
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::queueBeginVideo()
{
    beginVideoTimer_->setSingleShot( true );
    beginVideoTimer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::queueRestartVideo()
{
    restartVideoTimer_->setSingleShot( true );
    restartVideoTimer_->start( 0 );
}
*/

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::restartVideo()
{
    LOG_CLASS_INFO( "selfserveFinish", "restartVideo" );

    static const int maxPlaybacks( 32 );

    // when we reach max playbacks we destroy the player and create a new one
    if ( maxPlaybacks <= ++playbacks_ )
    {
        LOG_DEBUG( "reallocate player (%d)...", playbacks_ );

        destroyVideoPlayer();
        createVideoPlayer();

        queueBeginVideo();
    }

    // otherwise just restart current video
    else if ( player_ )
    {
        LOG_DEBUG( "restarting video (%d)...", playbacks_ );

        player_->stop();
        player_->play();
    }

    LOG_DEBUG( "done", NULL );
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::startOver()
{
    LOG_CLASS_INFO( "selfserveFinish", "startOver" );

    LOG_DEBUG( "startOver() beginPrintOrScanTimer_ Timer", NULL );

    // Called when we timeout from the FINISH screen.
    // pjr383 - use original test
    if( ( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )  || ( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == SelfServicePrintOption::SelfServOption_PrintToLabel ) )
    {
        // This is not a timer option with just print label
        LOG_DEBUG( "startOver() beginPrintOrScanTimer_ return because not a valid option", NULL );
        return;
    }

    // SOF-4007 - Big Y Self-Serve: Add timeout from the Print to Screen page
    // Return to Default Flashkeys Screen IF we have not pressed the PRINT key.  The Print key has its own timer and it
    // may have been pressed 15 seconds in, so we dont want to timeout in 15 more seconds. Allow its own timer to work
    if( !scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
    {
        LOG_DEBUG( "startOver() IF return to Flashkeys for beginPrintOrScanTimer_", NULL );

        if( scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg )
            scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg = false;

        // do nothing, just return to default location.
        hook_->session()->send( ProductDev, ProductAcceptSelfServiceBarcode, "1" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::resetStartOverTimer()
{
    LOG_CLASS_INFO( "selfserveFinish", "resetStartOverTimer" );

    LOG_DEBUG( "resetStartOverTimer() beginPrintOrScanTimer_ Timer", NULL );

    // pjr383 - use original test
    if( ( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() ) || ( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == SelfServicePrintOption::SelfServOption_PrintToLabel ) )
    {
        // This is not a timer option with just print label
        LOG_DEBUG( "resetStartOverTimer() beginPrintOrScanTimer_ return because not a valid option", NULL );
        return;
    }

    beginPrintOrScanTimer_->setSingleShot( true );
    beginPrintOrScanTimer_->start( 30000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::initialize()
{
    LOG_CLASS_INFO( "selfserveFinish", "initialize" );

    gifPlayer_ = new QMovie( mediaFile_ );
    gifViewer_ = new QLabel( this );
    gifViewer_->setMovie( gifPlayer_ );

    // Set up the hook to access ScaleConfiguration
    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // create store logo
    storeLogo_ = new QLabel( this );
    storeLogo_->lower();
    storeLogo_->move( 0, 0 );

    // set video playback area to fixed size (16:9 aspect ratio)
    ui->takeLabelArea->setAutoFillBackground( true );

    // pjr383 - use original test
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() > SelfServicePrintOption::SelfServOption_PrintToLabel )
    {
        // SOF-4007 - Big Y Self-Serve: There is no timeout from the Print to Screen page
        // When we come to this screen for option 3 we have to set up a timer
        LOG_DEBUG( "initialize() 1 set up new beginPrintOrScanTimer_ QTimer", NULL );

        beginPrintOrScanTimer_ = new QTimer( this );
        connect( beginPrintOrScanTimer_, SIGNAL( timeout() ), SLOT( startOver() ) );
    }
    else
         LOG_DEBUG( "initialize() 2 DID NOT set up new beginPrintOrScanTimer_ QTimer", NULL );

    if( "1" == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) )
    {
        LOG_DEBUG( "initialize() 3 selfserviceDisplayBarcodeLabel", NULL );
        // this is odd to me, the IF is testing if we are printing the BARCODE to the screen.  And when barcode is turned on, it does not go into here and do this. This shoud be true
        ui->takeLabelArea->setFixedSize( 640, 360 );
    }
    else if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    {
        LOG_DEBUG( "initialize() 4 else if Amazon GO", NULL );
        // pjr383 - use original test
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        {
            LOG_DEBUG( "initialize() 5 selfserviceDisplayBarcodeLabel", NULL );
            // When barcode is turned on, we enable "Print to Screen" for the Print Method so we enter here.  I thought originally I had this set to Scan and Print, so should make sure that works too
            // Option 3 Display Scan Label and allow Printing (ignore errors)
            ui->takeLabelArea->setFixedSize( 640, 360 );
        }
        else
        {
            int labelHeight( (height() * 80) / 100 );
            int labelWidth( ((width() * 80) / 100) );
            LOG_DEBUG( "initialize() 6 selfserviceDisplayBarcodeLabel labelwidth: %dx%d", labelWidth, labelHeight );
            // Scan and print comes in here, 819x480. Seems large.
            ui->takeLabelArea->setFixedSize( labelWidth, labelHeight );
        }
    }
    else
    {
        LOG_DEBUG( "initialize() 7 selfserviceDisplayBarcodeLabel", NULL );
        ui->takeLabelArea->setFixedSize( 640, 360 );
    }

    // dont do this while we are initially coming into self service
    if( scaleConfig_->internalConfig.ssactive_lock )
    {
        if( ( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ))
        {
            s->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) );           // Search on Fixed Flashkeys
            s->scaleConfig->internalConfig.updateSelfServiceFlashkeys = false;
        }
        else if (scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            s->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservProductGrid ) );
            s->scaleConfig->internalConfig.updateSelfServiceFlashkeys = false;
        }
    }

    // pjr383 - keep original IF
    if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    {
        LOG_DEBUG( "iinitialize() 8 selfServeDisplayLabelOnScreenGO", NULL );

        // pjr383 - use original test
        if( "1" == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) || (scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ))
        {
            LOG_DEBUG( "initialize() 9 selfServeDisplayLabelOnScreenGO", NULL );

            // Option 1 - Print the Label and play the take label video
            QPalette labelAreaPalette( ui->takeLabelArea->palette() );
            labelAreaPalette.setColor( ui->takeLabelArea->backgroundRole(), Qt::black );
            labelAreaPalette.setColor( ui->takeLabelArea->foregroundRole(), Qt::black );
            ui->takeLabelArea->setPalette( labelAreaPalette );
        }
        else
        {
            LOG_DEBUG( "initialize() 10 selfServeDisplayLabelOnScreenGO", NULL );

            QPalette labelAreaPalette( ui->takeLabelArea->palette() );
            labelAreaPalette.setColor( ui->takeLabelArea->backgroundRole(), Qt::white );
            labelAreaPalette.setColor( ui->takeLabelArea->foregroundRole(), Qt::white );
            ui->takeLabelArea->setPalette( labelAreaPalette );

            ui->lblThankYouSecondary->hide();
        }
    }
    // pjr383 - drop weight required option
    //else if ( ( 0 == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) )  && scaleConfig_->scaleConfig.SelfServeWeightRequiredToSelectItem() )
    else if (  SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
    {
        LOG_DEBUG( "initialize() 11 PrintToLabel", NULL );

        // Right now this is not working on a 10 inch.  I'm going to assume it's
        // working in a 14 and that why we did this, this way.

        // pjr383 - drop wt req and playlists checks
        //if( scaleConfig_->scaleConfig.SelfServeEnablePlaylists() )
        if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() < SelfServicePrintOption::SelfServOption_LookupKeypadOnly )
        {
            LOG_DEBUG( "initialize() 12 < keypad", NULL );

            // We have arrived on the print screen.  Check to see if we need to update flashkeys upon our return
            if ( scaleConfig_->internalConfig.updateSelfServiceFlashkeys )
            {
                s->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservProductGrid ) );
                s->scaleConfig->internalConfig.updateSelfServiceFlashkeys = false;
            }
        }

        QPalette labelAreaPalette( ui->takeLabelArea->palette() );
        labelAreaPalette.setColor( ui->takeLabelArea->backgroundRole(), Qt::transparent );
        labelAreaPalette.setColor( ui->takeLabelArea->foregroundRole(), Qt::transparent );
        ui->takeLabelArea->setPalette( labelAreaPalette );
    }
    else
    {
        LOG_DEBUG( "initialize() 12 final else", NULL );
        QPalette labelAreaPalette( ui->takeLabelArea->palette() );
        labelAreaPalette.setColor( ui->takeLabelArea->backgroundRole(), Qt::black );
        labelAreaPalette.setColor( ui->takeLabelArea->foregroundRole(), Qt::black );
        ui->takeLabelArea->setPalette( labelAreaPalette );
    }

    // load images
    onReloadImages();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveFinish::checkScreenSize()
{
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

/* PJR-383: Removed because using gif image instead of mp4 video
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::createVideoPlayer()
{
    LOG_CLASS_INFO( "selfserveFinish", "createVideoPlayer" );

    // nothing to do
    if ( !active_ )
        return;

    QWidget *w( ui->takeLabelArea );

    if ( w->layout() )
        delete w->layout();

    //
    // setup video player widget
    //

    LOG_DEBUG( "allocate new player...", NULL );

    //player_ = videoPlayer::create( videoPlayerDevice::Operator, w );

    //connect( player_, SIGNAL( finished() ), SLOT( queueRestartVideo() ) );
    //connect( player_, SIGNAL( playbackError(QString) ), SLOT( playbackError(QString) ) );

    //playbacks_ = 0;

    //
    // setup layout
    //

    QVBoxLayout *l( new QVBoxLayout( w ) );
    l->setContentsMargins( QMargins() );

    l->addWidget( player_ );

    LOG_DEBUG( "done", NULL );
}
*/

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::destroyVideoPlayer()
{
    LOG_CLASS_INFO( "selfserveFinish", "destroyVideoPlayer" );

    // do not destroy player out from under our feet if we are using it
    if ( waitingForStart_ )
    {
        needToDestroyPlayer_ = true;
        return;
    }

    if ( player_ )
    {
        videoPlayer *doomed( player_ );
        player_ = NULL;

        LOG_DEBUG( "destroy player...", NULL );

        doomed->stop();
        delete doomed;
    }

    needToDestroyPlayer_ = false;

    LOG_DEBUG( "done", NULL );
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::renderLabelImage()
{
    LOG_CLASS_INFO( "selfserveFinish", "renderLabelImage" );

    LOG_DEBUG( "renderLabelImage() START", NULL );

    int labelHeight( (height() * 80) / 100 );
    int labelWidth( (width() * 80) / 100 );

    ui->takeLabelArea->setAutoFillBackground( true );
    ui->takeLabelArea->setFixedSize( labelWidth, labelHeight );

    // ---- //

    const QString label( hook_->fieldValue( sessionHooks::selfServiceBarcodeFile ) );
    bool labelValid( false );

    QPalette p( ui->takeLabelArea->palette() );

    if ( !label.isEmpty() )
    {
        QPixmap image( label );

        if ( !image.isNull() )
        {
            // create pixmap
            QPixmap dest( labelWidth, labelHeight );
            dest.fill( Qt::transparent );

            // create painter
            QPainter painter( &dest );
            painter.setRenderHint( QPainter::Antialiasing, true );

            painter.setBackgroundMode( Qt::TransparentMode );

            // draw pixmap
            painter.drawPixmap( ( dest.width() - image.width() ) / 2, 0, image );

            p.setBrush( backgroundRole(), dest );
            labelValid = true;
        }
    }

    // no nutrifacts
    if ( !labelValid )
        p.setColor( backgroundRole(), Qt::white );

    // set new palette
    ui->takeLabelArea->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveFinish::renderPull()
{
    LOG_CLASS_INFO( "selfserveFinish", "renderPull" );

    LOG_DEBUG( "renderPull() START", NULL );

    QPixmap pullit( hook_->pullLabel() );

    QPalette p( ui->takeLabelArea->palette() );

    // set pull label image
    if ( pullit.isNull() )
    {
        p.setColor( ui->takeLabelArea->backgroundRole(), Qt::transparent );
        p.setColor( ui->takeLabelArea->foregroundRole(), Qt::transparent );
    }
    else
    {
        // set size
        ui->takeLabelArea->setFixedSize( pullit.size() );

        // set image
        p.setBrush( ui->takeLabelArea->backgroundRole(), pullit );
    }

    ui->takeLabelArea->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
