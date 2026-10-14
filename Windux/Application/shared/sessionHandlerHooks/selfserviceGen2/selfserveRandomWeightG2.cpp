/**
 * @file selfserveRandomWeight.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveRandomWeightG2.h"
#include "ui_selfserveRandomWeightG2.h"
#include "selfservicesoftkeybuttonG2.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <pixmap.h>
#include <QMouseEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QSignalMapper>
#include <filesystem.h>
#include <softkeyswidget.h>
#include <pushbutton.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <QMessageBox>
#include <QMutexLocker>
#include <ScaleClasses.h>
#include <QGraphicsOpacityEffect>
#include <QFont>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveRandomWeightG2::selfserveRandomWeightG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveRandomWeightG2),
    s(NULL),
    hook_( h ),
    hideBigPrintKey( 0 ),
    scaleConfig_(NULL),
    m_( QMutex::Recursive )
{
    ui->setupUi(this);

    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveRandomWeightG2::~selfserveRandomWeightG2()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    Q_UNUSED ( m );
    return;         // only had code for tare flashkeys so I deleted it all
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::refreshWeight()
{
    // Update weight
    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );
    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );
    //QString lows = hook_->fieldValue( sessionHooks::dualCapacityIndicatorString );
    //QString high = hook_->fieldValue( sessionHooks::capacityIndicatorString );

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
        if( hook_->session()->uiUtilities->isDualRangeCapable() )
            capacityLowRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255); ");                      // hide capacity
        else
            capacityLowRange_->setVisible( false );
    }

    // Update hide/show zero
    zero_->setVisible( "1" == hook_->fieldValue( sessionHooks::zeroIndicator ) );

    // Update all unit prices and total prices
    QString unitPrice( hook_->fieldValue( sessionHooks::itemUnitPrice ) );
    QString unitPriceFractional( hook_->fieldValue( sessionHooks::itemUnitPriceFractional ) );
    QString unitPricePoundsFor( hook_->fieldValue( sessionHooks::itemUnitPricePoundsFor ) );

    unitPriceFractional = unitPriceFractional.trimmed();
    unitPricePoundsFor = unitPricePoundsFor.trimmed();

    if (( unitPriceFractional.length() ) && ( "1" != unitPriceFractional ))
    {
        unitPrice.append( " " );

        if ( "100" == unitPriceFractional )
            unitPrice.append( "100g" );
        else if ( "50" == unitPriceFractional )
            unitPrice.append( QString::fromUtf8( "½ lb" ) );
        else if ( "25" == unitPriceFractional )
            unitPrice.append( QString::fromUtf8( "¼ lb" ) );
    }
    else if (( unitPricePoundsFor.length() ) && ( "1" != unitPricePoundsFor ))
    {
        if( 1 == scaleConfig_->printSpecialFields.unitPriceFormat().toInt() )
        {
            bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );
            QString prefix( "%1 %2 %3 " );

            unitPrice.prepend( prefix.arg( unitPricePoundsFor ).arg( metricScale ? "kgs" : "lbs" ).arg( tr( "for" ) ) );
        }
    }

    unitPrice_->setText( unitPrice );
    totalPrice_->setText( hook_->fieldValue( sessionHooks::itemTotalPrice ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::refresh()
{
    if( scaleConfig_->internalConfig.activate_selfservice )
        return;

    if( scaleConfig_->currentPLU.productType() == ProductType::PRODUCTTYPE_BYCOUNT )
        return;                     // if we are updating a random weight, why update fields for a BC?

    // SOF-5010 always update the min weight to print
    minWeightToPrint_ = hook_->fieldValue( sessionHooks::minWeightToPrint ).toDouble();

    if( !scaleConfig_ )
    {
        s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    refreshWeight();

    // update item code/description
    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    QString itemDescription( hook_->fieldValue( sessionHooks::itemDescription ) );

    ui->expandedTextArea->refresh();

    plu_->setText( itemCode );
    desc_->setText( itemDescription.toUpper() );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::translate()
{
    ui->retranslateUi(this);

    if( hideBigPrintKey )
        return;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::resizeEvent( QResizeEvent *e )
{
    renderLabelImage();

    _Mybase::resizeEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::showEvent( QShowEvent *e )
{
    bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );

    QFont ssfont;
    ssfont.setFamily( "Poppins" );
    ssfont.setBold( true );
    if( metricScale )
    {
        ssfont.setPointSize( 36 );

        weight_->setFont( ssfont );
        unitPrice_->setFont( ssfont );
        totalPrice_->setFont( ssfont );

        weight_->move( 20,33 );
        unitPrice_->move( 10,33 );
        totalPrice_->move( 20,33 );
    }
    else
    {
        ssfont.setPointSize( 42 );

        weight_->setFont( ssfont );
        unitPrice_->setFont( ssfont );
        totalPrice_->setFont( ssfont );

        weight_->move( 20,36 );
        unitPrice_->move( 10,36 );
        totalPrice_->move( 20,36 );
    }

    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    plu_->setText( itemCode );

    resetScreen();

    translate();

    _Mybase::showEvent( e );
}

void selfserveRandomWeightG2::hideEvent( QHideEvent *e)
{
    desc_->setText( "" );
    plu_->setText( "" );
    totalPrice_->setText( "" );
    unitPrice_->setText( "" );
    weight_->setText( "" );

    _Mybase::hideEvent( e );
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "selfserveRandomWeightG2", "mousePressEvent" );

    if ( Qt::LeftButton == e->button() )
    {
        LOG_DEBUG( "Got a mousePressEvent on RandomWeight Screen", NULL );

        int clickX = e->globalX();
        int clickY = e->globalY();
        int screen_h = getScreenHeight();

        // See if the user clicked capacity and allow a little leeway
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
            // pop the weights and measures
            hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );
            scaleConfig_->internalConfig.selfServicePreventClicks = false;
        }
        //else
        //{
        //    // if we didnt click the capacity, then we must be printing
        //    hook_->emitPrint();
        //    LOG_DEBUG( "Do a PRINT", NULL );
        //}
    }

    LOG_DEBUG( "END mousePressedEvent()", NULL );

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::onButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfserveRandomWeightG2", "onButtonClicked" );

    if ( printLabel_ == w )
    {
        if( "1" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog ) )
        {
            LOG_DEBUG( "RW onPrint() selfServiceRestrictedDialog = 1 so BAIL", NULL );
            return;
        }

        if ( scaleConfig_->internalConfig.selfServicePreventClicks )
            return;

        scaleConfig_->internalConfig.selfServicePreventClicks = true;

        QString wgt = weight_->text();
        wgt.replace(" kg", "");      // get rid of it if its present
        wgt.replace(" lb", "");

        if( ( 0 == wgt.length() ) || ( 0 == wgt.toDouble() ))
            hook_->session()->send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrPrintNoWeight ) + ",5" );
        else if( wgt.contains("-") )
            hook_->session()->send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrPrintBelowMinWeight ) + ",5" );
        else
            hook_->emitPrint();
    }
    else if ( startOver_ == w )
    {
        // dump current item (should not have one here)
        hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
    }
    else if( back_ == w )
    {
        // dump current item (should not have one here)
        hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
    }

    emit hook_->resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::onReloadImages()
{
    refreshWeight();

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfserveRandomWeightG2", "renderBarcodeOnlyLabel" );

    LOG_DEBUG( "renderBarcodeOnlyLabel() call renderLabelImage()", NULL );
    renderLabelImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::renderLabelImage()
{
    LOG_CLASS_INFO( "selfserveRandomWeightG2", "renderLabelImage" );

    double minWtPrint = minWeightToPrint_;
    double incomingWt = 0.0;

    if( hook_->fieldValue( sessionHooks::netWeight ).length() > 0 )
    {
        QString testNetWt = hook_->fieldValue( sessionHooks::netWeight );
        if( testNetWt.contains( " " ) )
        {
            // need to get rid of the units lb/kg - everything past a space
            testNetWt = testNetWt.mid(0, testNetWt.indexOf(" "));
            incomingWt = testNetWt.toDouble();
        }

        if( incomingWt < minWtPrint )
        {
            ui->barcodeOnly->hide();
            return;
        }
        else
        {
            ui->barcodeOnly->show();
        }
    }

    hideBigPrintKey = ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == 1 && scaleConfig_->scaleConfig.selfServeDisplayBarcodeLabel() == 1);

    QMutexLocker lock( &m_ );

    int bgSize = 308;
    int printHeight( 240 );

    ui->barcodeOnly->setAutoFillBackground( true );
    ui->barcodeOnly->setFixedSize( bgSize - 2, printHeight - 2 );       // (306,176)

    QString label( hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile ) ); // from 101,4 message

    if( "" == label )
    {
        // this method is called by 101,4 (print) or in here, a 101,15(total) and the weight is a bad weight. Might be existing or not.
        ui->barcodeOnly->hide();
        LOG_DEBUG( "selfserveRandomWeight::renderLabelImage() Blank Label return END", NULL );
        return;
    }
    else
    {
        ui->barcodeOnly->show();
    }

    // New Scaling
    bool labelValid( false );

    QPalette p( ui->barcodeOnly->palette() );

    if ( !label.isEmpty() )
    {
        QPixmap image( label );
        if ( !image.isNull() )          // 448x335
        {

            // create pixmap
            QPixmap dest( (bgSize - 2), (printHeight - 2) );

            if( dest.isNull() )
                LOG_DEBUG( "renderLabelImage() dest.isNull() is TRUE so this update() WILL FAIL", NULL );

            dest.fill( Qt::transparent );
            {
                // create painter
                QPainter painter( &dest );
                painter.setRenderHint( QPainter::Antialiasing, true );

                // draw pixmap
                painter.begin( &dest );
                //painter.drawPixmap( 30, 0, dest.width(), dest.height(), image );
                int xpos = scaleConfig_->scaleConfig.selfServeFreshServeLabelOffset().toInt();
                painter.drawPixmap( xpos, 0, dest.width(), dest.height(), image );
                painter.end();
            }

            p.setBrush( backgroundRole(), dest );
            labelValid = true;
        }
        else
            LOG_DEBUG( "renderLabelImage() image.isNull = true so dont do anything", NULL );
    }
    else
        LOG_DEBUG( "renderLabelImage() label.isEmpty() label is empty so didn't display it", NULL );

    // set new palette
    ui->barcodeOnly->setPalette( p );

    ui->barcodeOnly->hide();
    ui->barcodeOnly->show();

    ui->barcodeOnly->update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveRandomWeightG2::getScreenHeight()
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
void selfserveRandomWeightG2::initialize()
{
    s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    minWeightToPrint_ = hook_->fieldValue( sessionHooks::minWeightToPrint ).toDouble();

    hideBigPrintKey = true;

    plu_                = ui->lblPluNum;
    desc_               = ui->lblDescription;
    totalPrice_         = ui->lblTotalPrice;
    unitPrice_          = ui->lblUnitPrice;
    weight_             = ui->lblWeight;
    capacityLabel_      = ui->lblCapacity;
    zero_               = ui->lblZero;
    capacityLowRange_   = ui->lblLowRange;
    capacityHighRange_  = ui->lblHighRange;

    printLabel_         = ui->butPrint;
    startOver_          = ui->butStartOver;
    back_               = ui->butBack;

    barcode_            = ui->barcodeOnly;
    screen_             = ui->screenContainer;
    scanAndPrint_       = ui->wgtScanAndPrint;

    // load images
    onReloadImages();

    // set up plu image area
    ui->expandedTextArea->setSelfServiceHook( hook_ );

    // This all comes from IF normalScaleOps.selfServeDisplayBarcodeLabel() is TRUE and it IS

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( printLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printLabel_, printLabel_ );

    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    connect( back_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( back_, back_ );

    startOver_->setIcon( QIcon(":/images/screens/startover-icon.png" ) );
    startOver_->setIconSize(QSize(27,27));

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    screen_->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    screen_->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
    else
        screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(255,0,0); }" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeightG2::resetScreen()
{
    return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
