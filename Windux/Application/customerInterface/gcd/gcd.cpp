/**
 * @file gcd.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "gcd.h"
#include "ui_gcd.h"

#include "gcdlayoutfileparser.h"
#include "nowservingoverlay.h"

#include <filesystem.h>
#include <logmanager.h>
#include <playlistwidget.h>

#include <QDir>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////
gcd::gcd( QWidget *parent ) :
    QMainWindow( parent, Qt::FramelessWindowHint ),
    ui_( new Ui::gcd ),
    layout_( ui_ ),
    layoutTimer_( NULL ),
    timer_( NULL ),
    idleTimer_( 0 ),
    idleTimerTimeout_( IDLE_TIMEOUT ),
    idleTimerBlockedItem_( false ),
    idleTimerBlockedWeight_( false ),
    advertType_( showPlaylist ),
    productAdvertType_( productShowNone ),
    overlay_( NULL ),
    outOfService_( false ),
    outOfServiceForced_( false ),
    testImage_( NoTestImage ),
    weighMode_( gcdWeighModeType::Avoir ),
    currency_( gcdCurrencyType::AmericanDollar ),
    capacityDualRangeMode_( gcdLanguage::CapacityEmpty ),
    weigherAttached_( false )
{
    LOG_CLASS_INFO( "gcd", "gcd" );

    setObjectName( "gcd" );

    // Manifest UI
    ui_->setupUi( this );

    // HT-419 hide mouse
    QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );

    connect( ui_->idleVideoFeed, SIGNAL( playlistError(QString,QString) ), SIGNAL( playlistError(QString,QString) ) );
    connect( ui_->itemVideoFeed, SIGNAL( playlistError(QString,QString) ), SIGNAL( playlistError(QString,QString) ) );
    connect( ui_->videoFeed, SIGNAL( playlistError(QString,QString) ), SIGNAL( playlistError(QString,QString) ) );

    //-------------------------------------------------------------------------
    // plu description
    //-------------------------------------------------------------------------

    ui_->productDescriptionLabel->setTextFormat( Qt::RichText );
    ui_->productDescriptionLabel->setScrollType( MarqueeLabelWidget::OnlyWhenLarger );

    //-------------------------------------------------------------------------
    // marquee
    //-------------------------------------------------------------------------

    ui_->marqueeLabel->setTextFormat( Qt::RichText );

    //-------------------------------------------------------------------------
    // timers
    //-------------------------------------------------------------------------

    timer_ = new QTimer( this );
    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    testImageTimer_ = new QTimer( this );
    connect( testImageTimer_, SIGNAL( timeout() ), SLOT( onTestImageTimeout() ) );

    layoutTimer_ = new QTimer( this );
    connect( layoutTimer_, SIGNAL( timeout() ), SLOT( onCheckForLayoutChanged() ) );

    // start timers
    timer_->start( TIMER_RESOLUTION );
    layoutTimer_->start( LAYOUT_TIMER_RESOLUTION );

    //-------------------------------------------------------------------------
    // user interface
    //-------------------------------------------------------------------------

    // load default layout
    setupUi( filesystem::absoluteFilePath( "defaultLayout.xml", folderType::Layouts ) );

    // hide labels we are not using
    ui_->zeroAnnunLabel->hide();
    ui_->prepackAnnunLabel->hide();

    // generate idle filename
    QDir idleFilePath( QDir::temp() );
    idleFile_ = idleFilePath.absoluteFilePath( "gcdidle.lock" );

    if (( idleFile_.length() ) && ( QFile::exists( idleFile_ ) ))
        QFile::remove( idleFile_ );

    LOG_INFO( "idle file lock is '%s'", qPrintable( idleFile_ ) );

    hideFrequentShopper();

    // setup language
    retranslateUi();

    // initial state is out of service
    setOutOfService( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gcd::~gcd()
{
    if ( ui_ )
        delete ui_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::setTestImage( TestImageMode mode, const QString &filename )
{
    LOG_CLASS_INFO( "gcd", "setTestImage" );

    // cannot do this when out of service
    if ( outOfService() )
    {
        LOG_WARN( "cannot show test image, scale is out of service", NULL );
        return;
    }
    // nothing to do...
    else if ( mode == testImage_ )
    {
        return;
    }
    else if ( BlankScreen == testImage_ && mode != BlankScreenOff )
    {
       	LOG_INFO( "ignoring setting test image, gcd is blank", NULL );
        return;
    }
    else if ( BlankScreenOff == mode )
    {
       	LOG_INFO( "turning off blanking of gcd", NULL );
    	mode = NoTestImage;
    }

    // show test image
    if ( (testImage_ = mode) )
    {
        LOG_INFO( "showing test image", NULL );

        bool allowTimeout( true );

        if ( ColorBarPattern == mode )
            ui_->servicePage->setPalette( layout_.palette<gcdLayout::TestImage1>() );
        else if ( WhiteScreen == mode )
            ui_->servicePage->setPalette( Qt::white );
        else if ( GrayBackground == mode )
            ui_->servicePage->setPalette( layout_.palette<gcdLayout::TestImage2>() );
        else if ( BlackScreen == mode )
            ui_->servicePage->setPalette( Qt::black );
        else if ( BlankScreen == mode )
        {
        	LOG_INFO( "blanking out gcd", NULL );
            ui_->servicePage->setPalette( Qt::black );
            allowTimeout = false;
        }
        else if ( CustomImage == mode )
        {
            const QPixmap image( filename );

            QPixmap background( size() );
            background.fill( Qt::black );

            // center image on background
            QPainter painter( &background );

            int offset( 0 );

            if ( image.height() < background.height() )
                offset = (background.height() - image.height()) / 2;

            painter.drawPixmap( (background.width() - image.width()) / 2, offset, image.width(), image.height(), image );

            QPalette p;
            p.setBrush( QPalette::Window, QPixmap( background ) );

            ui_->servicePage->setPalette( p );

            // no timeout for this image
            allowTimeout = false;
        }

        ui_->servicePage->setAutoFillBackground( true );

        // FT-1209: new functionality is to cycle through test images at a 3 second interval
        if ( allowTimeout )
        {
            testImageTimer_->setSingleShot( true );
            testImageTimer_->start( 3000 );
        }

        resetIdle();
        setState( stateTestImage );
    }

    // hide test image
    else
    {
        LOG_INFO( "hiding test image", NULL );
        setState( stateMetrology );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::setOutOfService( bool mode )
{
    LOG_CLASS_INFO( "gcd", "setOutOfService" );

    // nothing to do...
    if ( mode == outOfService_ )
        return;

    // if we are out of service we cannot show test image
    testImage_ = NoTestImage;

    // show out of service image
    if ( (outOfService_ = mode) )
    {
        LOG_INFO( "out of service!", NULL );

        ui_->servicePage->setPalette( layout_.palette<gcdLayout::OutOfService>() );
        ui_->servicePage->setAutoFillBackground( true );

        resetIdle();
        setState( stateOutOfService );
    }

    // hide out of service image
    else
    {
        LOG_INFO( "exiting out of service mode...", NULL );

        // emit status changed
        Q_EMIT updateStatus(
            VERSION_MAJOR,
            VERSION_MINOR,
            gcdLanguageType::toString( gcdLanguage::language() ),
            gcdWeighModeType::toString( weighMode_ ) );

        resetMetrologyPage();
        setState( stateMetrology );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcd::idle() const
{
    if ( !idleVideoFeed()->isCurrentPlaylistValid() )
        return false;

    return (( idleTimerTimeout_ ) && ( idleTimer_ >= idleTimerTimeout_ ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::onTimeout()
{
    LOG_CLASS_INFO( "gcd", "onTimeout" );

    // do nothing for these states
    if (( outOfService() ) || ( testImage() ))
    {
        QMutexLocker guard( &overlayMutex_ );

        // destroy existing overlay
        destroyOverlay();

        return;
    }

    // for fullscreen there is nothing to do when blocked
    if ( overlay_ )
    {
        QMutexLocker guard( &overlayMutex_ );

        // destroy inactive overlay
        if (( overlay_ ) && ( overlay::Active != overlay_->overlayState() ))
            destroyOverlay();
    }
    else if (( idleTimerBlockedItem_ ) || ( idleTimerBlockedWeight_ ))
    {
        if ( layout_.isFullscreenAdverts() )
            return;
    }
    else
    {
        idleTimer_ += TIMER_RESOLUTION;
    }

    // check for idle state
    if (( idleTimerTimeout_ ) && ( idle() ))
    {
        // check if we are changing states into idle
        if (( idleTimerTimeout_ <= idleTimer_ ) && ( idleTimer_ < (idleTimerTimeout_ + TIMER_RESOLUTION) ))
        {
            LOG_INFO( "entering idle state...", NULL );

            if ( layout_.isFullscreenAdverts() )
                setState( stateFullscreenAdverts );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::onTestImageTimeout()
{
    LOG_CLASS_INFO( "gcd", "onTestImageTimeout" );

    // nothing to do
    if ( !testImage_ )
        return;

    TestImageMode nextMode( NoTestImage );

    if ( ColorBarPattern == testImage_ )
        nextMode = GrayBackground;
    else if ( WhiteScreen == testImage_ )
        nextMode = BlackScreen;

    LOG_TRACE( "switching modes from %d to %d", testImage_, nextMode );
    setTestImage( nextMode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::onCheckForLayoutChanged()
{
    LOG_CLASS_INFO( "gcd", "onCheckForLayoutChanged" );

    LOG_TRACE( "checking layout for changes...", NULL );

    // layout changed!
    if ( layout_.changed() )
    {
        QString filename( layout_.filename() );

        LOG_INFO( "layout contents have changed!", NULL );
        setupUi( filename );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::onUpdateUserInterface( gcdUpdateUserInterfaceType::type type, const QString& value )
{
    LOG_CLASS_INFO( "gcd", "onUpdateUserInterface" );

    // Note:
    // James Meyer wants a QR barcode to be displayed on the GCD for certain items. This barcode is
    // to be displayed on top of the store logo area and remain for the duration of the PLU.
    static const bool enableDelhaizeDemoHack( false );

    // we are no longer out of service
    setOutOfService( false );

    // handle update
    switch ( type )
    {
    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::Tare:
        ui_->tareValueLabel->setText( updateUnitSeparator( value ) );
        ui_->tareValueLabel->setVisible( weigherAttached_ );

		ui_->tareLabel->setText( gcdLanguage::get( gcdLanguage::Tare, weighMode_, weigherAttached_ ) );
        ui_->tareLabel->setVisible( weigherAttached_ );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::Weight:
        ui_->weightValueLabel->setText( updateUnitSeparator( value ) );

        LOG_TRACE( "weight %s", qPrintable( value ) );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::UnitPrice:
        if ( value.isEmpty() )
             ui_->pricePerLbValueLabel->setText( value );
        else
        {
            const QString currency( gcdCurrencyType::getSymbol( currency_ ) );

            if (( value.contains( currency ) ) || ( value.trimmed().contains( " " ) ))
                ui_->pricePerLbValueLabel->setText( updateUnitSeparator( value ) );
            else
                ui_->pricePerLbValueLabel->setText( gcdCurrencyType::getSymbol( currency_ ) + updateUnitSeparator( value ) );
        }

        adjustLabelFontPointSize( ui_->pricePerLbValueLabel, layout_pricePerLbValueLabel_font_ );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::TotalPrice:
        if ( value.length() )
            ui_->totalPriceValueLabel->setText( gcdCurrencyType::getSymbol( currency_ ) + updateUnitSeparator( value ) );
        else
            ui_->totalPriceValueLabel->setText( value );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::ZeroAnnuc:

        if ( !value.isEmpty() && weigherAttached_ )
        {
            ui_->zeroAnnunLabel->setVisible( true );
            idleTimerBlockedWeight_ = false;
        }
        else
        {
            // FT-602: Only interrupt idle mode (screen saver) when weigher attached and weigher is
            // non-zero. This means tapping the weigher should break idle mode (per W&M). Code used to
            // check for value.length(), but this has been removed.

            // when we have a weigher and a non-zero weight we need to exit idle mode

            // SOF-2049 - Zero Indicator should be shown based on the zero indicator flag,
            // not that the weight value was 0.000, 0.00, or 0 (which is what it was based on before)
            // do not allow idle mode while non-zero stable weight

            ui_->zeroAnnunLabel->setVisible( false );
            idleTimerBlockedWeight_ = true;
            resetIdle();
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::PrePackAnnuc:
        ui_->prepackAnnunLabel->setVisible( !value.isEmpty() );
        break;        

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::DualRangeIndicator:

        {
            gcdLanguage::type dualRangeMode( gcdLanguage::Capacity );

            if ( !weigherAttached_ )
                dualRangeMode = gcdLanguage::CapacityEmpty;
            else if ( "0" == value )
                dualRangeMode = gcdLanguage::CapacityDualRangeUp;
            else if ( "1" == value )
                dualRangeMode = gcdLanguage::CapacityDualRangeDown;

            if ( capacityDualRangeMode_ != dualRangeMode )
                setCapacityDualRangeMode( dualRangeMode, gcdLanguage::get( dualRangeMode, weighMode_, weigherAttached_ ) );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::WeigherAttached:
        ui_->capacityLabel->setVisible( (weigherAttached_ = !value.isEmpty()) );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::WeightType:

        {
            gcdLanguage::type weightLabelType = gcdLanguage::Weight;

            if ( "Weight" == value )
                weightLabelType = gcdLanguage::Weight;
            else if ( "ManualWeight" == value )
                weightLabelType = gcdLanguage::ManualWeight;
            else if ( "NetWeight" == value )
                weightLabelType = gcdLanguage::NetWeight;
            else if ( "Quantity" == value )
                weightLabelType = gcdLanguage::Quantity;

            ui_->weightLabel->setText( gcdLanguage::get( weightLabelType, weighMode_, weigherAttached_ ) );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::UnitPriceType:

        {
            QString prefix;
            QString suffix;

            if ( 2 < value.length() )
            {
                if ( "_p" == value.left( 2 ) )
                    prefix = updateUnitSeparator( value.mid( 2 ) );
                else if ( "_s" == value.left( 2 ) )
                    suffix = updateUnitSeparator( value.mid( 2 ) );
            }

            gcdLanguage::type pricePerLbLabelType = gcdLanguage::PricePerWeight;
            gcdLanguage::type fsPricePerLbLabelType = gcdLanguage::FSPricePerWeight;

            if ( "Weight" == value )
            {
                pricePerLbLabelType = gcdLanguage::PricePerWeight;
                fsPricePerLbLabelType = gcdLanguage::FSPricePerWeight;
            }
            else if ( "UnitPrice" == value )
            {
                pricePerLbLabelType = gcdLanguage::PricePerUnit;
                fsPricePerLbLabelType = gcdLanguage::FSPricePerUnit;
            }
            else if ( "Each" == value )
            {
                pricePerLbLabelType = gcdLanguage::PricePerEach;
                fsPricePerLbLabelType = gcdLanguage::FSPricePerEach;
            }
            else if ( prefix.length() )
            {
                pricePerLbLabelType = gcdLanguage::PricePerWeightFor;
                fsPricePerLbLabelType = gcdLanguage::FSPricePerWeightFor;
            }
            else if ( suffix.length() )
            {
                pricePerLbLabelType = gcdLanguage::PricePerFractional;
                fsPricePerLbLabelType = gcdLanguage::FSPricePerFractional;
            }

            ui_->pricePerLbLabel->setText( prefix + gcdLanguage::get( pricePerLbLabelType, weighMode_, weigherAttached_ ) + suffix );
            ui_->fsPricePerLbLabel->setText( prefix + gcdLanguage::get( fsPricePerLbLabelType, weighMode_, weigherAttached_ ) + suffix );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::FreqShopUnitPrice:
        if ( value.isEmpty() )
             ui_->fsPricePerLbValueLabel->setText( value );
        else
        {
            const QString currency( gcdCurrencyType::getSymbol( currency_ ) );

            if (( value.contains( currency ) ) || ( value.trimmed().contains( " " ) ))
                ui_->fsPricePerLbValueLabel->setText( updateUnitSeparator( value ) );
            else
                ui_->fsPricePerLbValueLabel->setText( gcdCurrencyType::getSymbol( currency_ ) + updateUnitSeparator( value ) );
        }

        adjustLabelFontPointSize( ui_->fsPricePerLbValueLabel, layout_fsPricePerLbValueLabel_font_ );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::FreqShopTotalPrice:
        if ( value.length() )
            ui_->fsTotalPriceValueLabel->setText( gcdCurrencyType::getSymbol( currency_ ) + updateUnitSeparator( value ) );
        else
            ui_->fsTotalPriceValueLabel->setText( value );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::FreqShopSavings:

        // when we have a valid non-zero weight we need to exit idle mode
        if (( value.length() ) && ( "0.00" != value ))
        {
            if ( value.length() )
                ui_->fsSavingsValueLabel->setText( gcdCurrencyType::getSymbol( currency_ ) + updateUnitSeparator( value ) );
            else
                ui_->fsSavingsValueLabel->setText( value );

            showFrequentShopper();
        }
        else
        {
            hideFrequentShopper();
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::PluNumber:

        LOG_TRACE( "plu '%s'", qPrintable( value ) );

        // when we have a valid plu we need to exit idle mode
        if (( value.length() ) && ( "none" != value ) && ( "0" != value ))
        {
            // do not allow idle mode while plu in system
            idleTimerBlockedItem_ = true;
            resetIdle();

            // check for QR code
            if ( enableDelhaizeDemoHack )
            {
                QString demoImageName( filesystem::absoluteFilePath( "qrcode_" + value + ".png", folderType::LabelImages ) );

                // check file exists
                if ( QFile::exists( demoImageName ) )
                {
                    QPixmap demoImage( demoImageName );

                    if ( !demoImage.isNull() )
                    {
                        QPixmap pixmap( ui_->storeLogoLabel->size() );
                        pixmap.fill( Qt::transparent );

                        // scale image to widget size
                        QPixmap scaledDemoImage( demoImage.scaled( pixmap.size(), Qt::KeepAspectRatio ) );

                        QPoint center( 0, 0 );
                        center.setX( (pixmap.width() - scaledDemoImage.width()) / 2 );
                        center.setY( (pixmap.height() - scaledDemoImage.height()) / 2 );

                        // copy image to pixmap
                        QPainter painter( &pixmap );
                        painter.setRenderHint( QPainter::Antialiasing, true );
                        painter.drawPixmap( center, scaledDemoImage );

                        // set pixmap to ui for our customers (and their customers) to adore and cherish
                        QPalette p( ui_->storeLogoLabel->palette() );
                        p.setBrush( ui_->storeLogoLabel->backgroundRole(), QBrush( pixmap ) );
                        ui_->storeLogoLabel->setPalette( p );
                    }
                }
            }
        }
        else
        {
            idleTimerBlockedItem_ = false;

            // remove QR code
            if ( enableDelhaizeDemoHack )
                ui_->storeLogoLabel->setPalette( layout_.palette<gcdLayout::StoreLogo>() );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::Description:

        {
            const QString trimmedValue( value.trimmed() );

            // plu description needs to hold the text at beginning end end
            if ( trimmedValue.isEmpty() )
                ui_->productDescriptionLabel->setText( "" );
            else
                ui_->productDescriptionLabel->setText( "<HOLD>" + value + "<ROLL><HOLD>" );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::Marquee:
        ui_->marqueeLabel->setText( value );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::ProductAdvertNone:
        createProductAdvert( productShowNone );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::ProductAdvertImage:
        createProductAdvert( productShowImage, value );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::ProductAdvertText:
        createProductAdvert( productShowText, value );
        break;

    //-------------------------------------------------------------------------
    case gcdUpdateUserInterfaceType::ProductAdvertPlaylist:
        createProductAdvert( productShowPlaylist, value );
        break;

    //-------------------------------------------------------------------------
    default:
        break;
    }

    // refresh the screen
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::onControlMessage( gcdControlMessageType::type type, const QString& value )
{
    LOG_CLASS_INFO( "gcd", "controlMessage" );

    // we are no longer out of service
    setOutOfService( false );

    // handle message
    switch ( type )
    {
    //-------------------------------------------------------------------------
    case gcdControlMessageType::HideTestColor:

        setTestImage( NoTestImage );
        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::OutOfService:

        setOutOfService( true );
        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetLanguage:

        {
            LOG_DEBUG( "setting language '%s'", qPrintable( value ) );

            // set language
            gcdLanguage::setLanguage( gcdLanguageType::fromString( value ) );

            // translate ui to new weigh mode
            retranslateUi();

            // emit status changed
            Q_EMIT updateStatus(
                VERSION_MAJOR,
                VERSION_MINOR,
                gcdLanguageType::toString( gcdLanguage::language() ),
                gcdWeighModeType::toString( weighMode_ ) );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetLayout:

        setupUi( value );
        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetPlaybackWaitTime:

        if ( idleTimerTimeout_ != value.toUInt() )
        {
            unsigned int old( idleTimerTimeout_ );

            LOG_TRACE( "exiting idle state...", NULL );
            resetIdle();

            idleTimerTimeout_ = value.toUInt();
            LOG_INFO( "new idle timeout %d ms (old %d ms)", idleTimerTimeout_, old );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetPlaylist:

        if ( currentPlaylist_ == value )
            LOG_DEBUG( "playlist '%s' already set", qPrintable( value ) );
        else
        {
            LOG_INFO( "new playlist '%s'", qPrintable( value ) );
            currentPlaylist_ = value;

	        LOG_INFO( "Return to Metrology in case of an empty playlist.", NULL );
	        setState( stateMetrology );			//	FT-1643
		    idleTimer_ = 0;

            if ( showPlaylist == advertType_ )
                updateIdleAdverts();
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetVolume:

        {
            float volume( value.toFloat() );
            LOG_INFO( "setting volume %.2f", volume );

            ui_->idleVideoFeed->setVolume( volume );
            ui_->itemVideoFeed->setVolume( volume );
            ui_->videoFeed->setVolume( volume );
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetWeighMode:

        {
            gcdWeighModeType::type oldMode = weighMode_;
            gcdWeighModeType::type newMode = gcdWeighModeType::fromString( value );

            // set weight mode
            if ( oldMode != newMode )
            {
                weighMode_ = newMode;

                LOG_INFO( "new weigh mode %d (old %d)", newMode, oldMode );

                // translate ui to new weigh mode
                retranslateUi();

                // emit status changed
                Q_EMIT updateStatus(
                    VERSION_MAJOR,
                    VERSION_MINOR,
                    gcdLanguageType::toString( gcdLanguage::language() ),
                    gcdWeighModeType::toString( weighMode_ ) );
            }
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::ShowTestColor:

        if ( value.length() <= 1 )
            setTestImage( (TestImageMode) value.toInt() );
        else
            setTestImage( CustomImage, value );

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetUnitSeparator:

        gcdCurrencyType::setUnitSeparator( currency_, value );
        retranslateUi();

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetCurrencySymbol:

        gcdCurrencyType::setSymbol( currency_, value );
        retranslateUi();

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::NowServingCustomerNumber:

        {
            QMutexLocker guard( &overlayMutex_ );
            nowServingOverlay *o;

            // destroy existing overlay
            destroyOverlay();

            // generate now serving overlay
            if (( overlay_ = (o = new nowServingOverlay( value, this )) ) &&
                ( overlay_->initializeOverlay() ))
            {
                LOG_TRACE( "exiting idle state...", NULL );
                resetIdle();

                // FT-1502: stop any video playing
                idleVideoFeed()->setEnabled( false );
                itemVideoFeed()->setEnabled( false );

                // set overlay location
                o->setLocation( layout_.nowServingLocation() );

                LOG_INFO( "now serving customer %s", qPrintable( value ) );
                overlay_->showOverlay();
            }
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::DismissOverlay:

        {
            QMutexLocker guard( &overlayMutex_ );

            // destroy existing overlay
            destroyOverlay();
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::SetLayoutAdvertSetting:

        {
            advertismentMode mode( showPlaylist );

            // advert settings
            // 0 = use playlist
            // 1 = use website
            if ( "0" == value )
                mode = showPlaylist;
            else if ( "1" == value )
                mode = showWebsite;

            if ( advertType_ != mode )
            {
                advertType_ = mode;
                updateIdleAdverts();
            }
        }

        break;

    //-------------------------------------------------------------------------
    case gcdControlMessageType::Website:

        // set website
        if ( currentWebsite_ != value )
        {
            currentWebsite_ = value;

            if ( showWebsite == advertType_ )
                updateIdleAdverts();
        }

        break;

    //-------------------------------------------------------------------------
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::resetIdle()
{
    LOG_CLASS_INFO( "gcd", "resetIdle" );

    // get out of idle mode
    if ( idle() )
    {
        LOG_INFO( "exiting idle state", NULL );
        setState( stateMetrology );
    }

    // reset timer since we have weight
    idleTimer_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::setCapacityDualRangeMode( gcdLanguage::type newMode, const QString& text )
{
    QPalette p( ui_->capacityLabel->palette() );

    // set mode
    capacityDualRangeMode_ = newMode;

    // set text
    QString labelText( updateUnitSeparator( text ) );

    ui_->capacityLabel->setText( labelText.replace( "*", "" ) );

    // dual range background
    QPixmap background( ui_->capacityLabel->size() );
    background.fill( Qt::transparent );

    // look for dual range indicator
    int begin = text.indexOf( "*" );
    int end = -1;

    if ( 0 <= begin )
        end = text.indexOf( "*", begin+1 );

    // dual range indicator exists
    if ( begin < end )
    {
        int height = ui_->capacityLabel->height();

        // compute location of range highlight
        QFontMetrics fontMetrics( ui_->capacityLabel->font() );

        int left( fontMetrics.width( labelText, begin ) );
        int right( fontMetrics.width( labelText, end-1 ) );

        // check for right aligned text, offset points if required
        if ( Qt::AlignRight == (Qt::AlignRight & ui_->capacityLabel->alignment()) )
        {
            int offset( ui_->capacityLabel->width() - fontMetrics.width( labelText ) );

            left += offset;
            right += offset;
        }

        // draw underscore for current range
        QPainter painter( &background );
        painter.setPen( QPen( p.color( ui_->capacityLabel->foregroundRole() ), 2 ) );
        painter.setRenderHint( QPainter::Antialiasing, true );

        painter.drawLine( QPoint( left, height-2 ), QPoint( right, height-2 ) );
    }

    // set the background
    p.setBrush( ui_->capacityLabel->backgroundRole(), background );

    ui_->capacityLabel->setAutoFillBackground( true );
    ui_->capacityLabel->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::setState( state s )
{
    LOG_CLASS_INFO( "gcd", "setState" );

    switch ( s )
    {
    case stateOutOfService:
    case stateTestImage:

        // show page
        ui_->gcdStackedWidget->setCurrentWidget( ui_->servicePage );
        break;

    case stateMetrology:

        // remove idle file
        if (( idleFile_.length() ) && ( QFile::exists( idleFile_ ) ))
            QFile::remove( idleFile_ );

        // show page
        ui_->gcdStackedWidget->setCurrentWidget( ui_->metrologyPage );
        break;

    case stateFullscreenAdverts:

        // create idle file
        if ( idleFile_.length() )
        {
            QFile f( idleFile_ );

            if ( !f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
                LOG_WARN( "failed to create idle file '%s'", qPrintable( idleFile_ ) );
            else
            {
                f.write( "42" );
                f.close();
            }
        }

        // show page
        ui_->gcdStackedWidget->setCurrentWidget( ui_->videoPage );
        break;

    default:
        break;
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::showFrequentShopper()
{
    QPalette p;

    // choose proper background
    if ( isAdvertPaneVisible() )
        p = layout_.palette<gcdLayout::MetrologyFSBackgroundWithAdverts>();
    else
        p = layout_.palette<gcdLayout::MetrologyFSBackground>();

    // show labels
    ui_->fsSavingsLabel->show();
    ui_->fsPricePerLbLabel->show();
    ui_->fsTotalPriceLabel->show();

    ui_->fsSavingsValueLabel->show();
    ui_->fsPricePerLbValueLabel->show();
    ui_->fsTotalPriceValueLabel->show();

    // use fs background
    ui_->metrologyPage->setPalette( p );
    ui_->metrologyPage->setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::hideFrequentShopper()
{
    QPalette p;

    // choose proper background
    if ( isAdvertPaneVisible() )
        p = layout_.palette<gcdLayout::MetrologyBackgroundWithAdverts>();
    else
        p = layout_.palette<gcdLayout::MetrologyBackground>();

    // hide labels
    ui_->fsSavingsLabel->hide();
    ui_->fsPricePerLbLabel->hide();
    ui_->fsTotalPriceLabel->hide();

    ui_->fsSavingsValueLabel->hide();
    ui_->fsPricePerLbValueLabel->hide();
    ui_->fsTotalPriceValueLabel->hide();

    // use non-fs background
    ui_->metrologyPage->setPalette( p );
    ui_->metrologyPage->setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::resetMetrologyPage()
{
    LOG_CLASS_INFO( "gcd", "resetMetrologyPage" );

    LOG_TRACE( "resetting text fields...", NULL );

    ui_->productDescriptionLabel->setText( "" );
    ui_->tareValueLabel->setText( "" );
    ui_->weightValueLabel->setText( "" );
    ui_->pricePerLbValueLabel->setText( "" );
    ui_->pricePerLbValueLabel->setFont( layout_pricePerLbValueLabel_font_ );
    ui_->totalPriceValueLabel->setText( "" );
    ui_->fsSavingsValueLabel->setText( "" );
    ui_->fsPricePerLbValueLabel->setText( "" );
    ui_->fsPricePerLbValueLabel->setFont( layout_fsPricePerLbValueLabel_font_ );
    ui_->fsTotalPriceValueLabel->setText( "" );
    ui_->marqueeLabel->setText( "" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::updateIdleAdverts()
{
    LOG_CLASS_INFO( "gcd", "updateIdleAdverts" );

    QString playlist( currentPlaylist_ );

    // for website we need to generate a playlist
    if (( showWebsite == advertType_ ) && ( !currentWebsite_.isEmpty() ))
    {
        QDir tmp( QDir::tempPath() );
        QFile f( playlist = tmp.absoluteFilePath( "idlePlaylist.txt" ) );

        // generate playlist
        if ( !f.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
        {
            LOG_WARN( "error creating playlist '%s'", qPrintable( playlist ) );
            return;
        }
        else
        {
            QTextStream s( &f );
            s << "[slideshow1]" << endl;
            s << "delay=0" << endl;
            s << currentWebsite_ << endl;
            s << "[playlist]" << endl;
            s << "slideshow1" << endl;
        }
    }

    LOG_INFO( "loading playlist '%s'", qPrintable( playlist ) );
    idleVideoFeed()->setCurrentPlaylist( playlist );

    // update background image
    updateBackground();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::updateBackground()
{
    // repaint background
    if ( ui_->fsSavingsLabel->isHidden() )
        hideFrequentShopper();
    else
        showFrequentShopper();

    // set advert visibility
    ui_->embeddedAdverts->setVisible( isAdvertPaneVisible() );

    // update the screen
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::setupUi( const QString& filename )
{
    LOG_CLASS_INFO( "gcd", "setupUi" );

    LOG_INFO( "layout ui '%s'...", qPrintable( filename ) );

    // apply the layout
    if ( !QFile::exists( filename ) )
        LOG_WARN( "missing layout '%s'!", qPrintable( filename ) );
    else
    {
        playlistWidget *prevIdleVideoFeed( idleVideoFeed() );

        LOG_TRACE( "exiting idle state...", NULL );
        resetIdle();

        // stop any playlist
        idleVideoFeed()->setEnabled( false );

        // load the layout
        if ( !layout_.load( filename ) )
            LOG_WARN( "could not apply layout '%s'!", qPrintable( filename ) );
        else
        {
            // reset video feed
            prevIdleVideoFeed->resetCurrentPlaylist();
            updateIdleAdverts();

            // repaint background
            updateBackground();

            // save off point sizes of variable fields
            layout_pricePerLbValueLabel_font_ = ui_->pricePerLbValueLabel->font();
            layout_fsPricePerLbValueLabel_font_ = ui_->fsPricePerLbValueLabel->font();
        }

        // resume any playlist
        idleVideoFeed()->setEnabled( true );
    }

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::retranslateUi()
{
    LOG_CLASS_INFO( "gcd", "retranslateUi" );

    LOG_TRACE( "translating ui...", NULL );

    ui_->tareLabel->setText( gcdLanguage::get( gcdLanguage::Tare, weighMode_, weigherAttached_ ) );
    ui_->weightLabel->setText( gcdLanguage::get( gcdLanguage::Weight, weighMode_, weigherAttached_ ) );
    ui_->pricePerLbLabel->setText( gcdLanguage::get( gcdLanguage::PricePerWeight, weighMode_, weigherAttached_ ) );
    ui_->totalPriceLabel->setText( gcdLanguage::get( gcdLanguage::TotalPrice, weighMode_, weigherAttached_ ) );

    ui_->fsPricePerLbLabel->setText( gcdLanguage::get( gcdLanguage::FSPricePerWeight, weighMode_, weigherAttached_ ) );
    ui_->fsTotalPriceLabel->setText( gcdLanguage::get( gcdLanguage::FSTotalPrice, weighMode_, weigherAttached_ ) );
    ui_->fsSavingsLabel->setText( gcdLanguage::get( gcdLanguage::FSSavings, weighMode_, weigherAttached_ ) );

    setCapacityDualRangeMode( capacityDualRangeMode_, gcdLanguage::get( capacityDualRangeMode_, weighMode_, weigherAttached_ ) );
    ui_->zeroAnnunLabel->setText( gcdLanguage::get( gcdLanguage::AnnunZero, weighMode_, weigherAttached_ ) );
    ui_->prepackAnnunLabel->setText( gcdLanguage::get( gcdLanguage::AnnunPrePack, weighMode_, weigherAttached_ ) );

    update();

    LOG_TRACE( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString gcd::updateUnitSeparator( const QString& value )
{
    QString sep( gcdCurrencyType::getUnitSeparator( currency_ ) );

    // nothing to do
    if ( "." == sep )
        return value;

    QString newValue( value );
    newValue.replace( ".", sep );

    return newValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::destroyOverlay()
{
    if ( !overlay_ )
        return;

    overlay_->hideOverlay();
    overlay_->finalizeOverlay();

    delete overlay_;
    overlay_ = NULL;

    // FT-1502: resume any video playing
    idleVideoFeed()->setEnabled( true );
    itemVideoFeed()->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::createProductAdvert( productAdvertismentMode type, const QString& param )
{
    LOG_CLASS_INFO( "gcd", "createProductAdvert" );

    // nothing to do
    if (( productAdvertType_ == type ) && ( productAdvertParam_ == param ))
        return;

    // this mode does not apply to fullscreen adverts!
    if ( layout_.isFullscreenAdverts() )
        return;

    // ------------------------------------
    // create playlist with desired content
    // ------------------------------------

    QString playlist;

    switch ( type )
    {

    // remove product advert
    case productShowNone:
        break;

    // display an image on the screen
    case productShowImage:

        // validate param
        if ( !QFile::exists( param ) )
        {
            LOG_WARN( "image '%s' does not exist", qPrintable( param ) );
            return;
        }
        else
        {
            QDir tmp( QDir::tempPath() );
            QFile f( playlist = tmp.absoluteFilePath( "imagePlaylist.txt" ) );

            // generate playlist
            if ( !f.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
            {
                LOG_WARN( "error creating playlist '%s'", qPrintable( playlist ) );
                return;
            }
            else
            {
                QTextStream s( &f );
                s << "[slideshow1]" << endl;
                s << param << endl;
                s << "[playlist]" << endl;
                s << "slideshow1" << endl;
            }
        }

        break;

    case productShowText:
        LOG_WARN( "unsupported mode type %d", type );
        return;

    case productShowPlaylist:
	{
        // validate param
		QString filename( filesystem::absoluteFilePath( param, folderType::Playlists ) );
        if ( !QFile::exists( filename ) )
        {
            LOG_WARN( "playlist '%s' does not exist", qPrintable( filename ) );
            return;
        }
        else
        {
            // use this playlist
            playlist = filename;
        }

        break;
	}

    default:
        LOG_WARN( "invalid mode type %d", type );
        return;
    }

    // set the playlist
    itemVideoFeed()->setCurrentPlaylist( playlist );

    // update to proper advert
    ui_->embeddedAdverts->setCurrentWidget( (productShowNone == type) ? ui_->idleAdverts : ui_->itemAdverts );

    // save setting
    productAdvertType_ = type;
    productAdvertParam_ = param;

    updateBackground();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool gcd::isAdvertPaneVisible() const
{
    if ( layout_.isFullscreenAdverts() )
        return false;

    return (( idleVideoFeed()->isCurrentPlaylistValid() ) || ( itemVideoFeed()->isCurrentPlaylistValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
playlistWidget *gcd::idleVideoFeed() const
{
    return (layout_.isFullscreenAdverts() ? ui_->videoFeed : ui_->idleVideoFeed);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
playlistWidget *gcd::itemVideoFeed() const
{
    return ui_->itemVideoFeed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gcd::adjustLabelFontPointSize( QLabel *label, const QFont &originalFont )
{
    QFont f( originalFont );

    // go from large font to smaller font
    for ( int ps( originalFont.pointSize() ); ps >= 9; --ps )
    {
        // set font point size
        f.setPointSize( ps );
        label->setFont( f );

        // compute bounding rect
        QRect rect( label->rect() );
        rect.setHeight( INT_MAX );
        rect.setWidth( INT_MAX );

        const QFontMetrics metrics( f );
        QRect boundingRect( metrics.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, label->text() ) );

        // check for fit, break when okay
        if ( boundingRect.width() <= label->width() )
            break;
    }
}

