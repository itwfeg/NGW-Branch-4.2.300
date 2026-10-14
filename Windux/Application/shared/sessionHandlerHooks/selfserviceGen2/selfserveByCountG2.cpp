/**
 * @file selfserveByCountG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * @swipedfrom Doug Antonides
 */

#include "selfserveByCountG2.h"
#include "ui_selfserveByCountG2.h"

#include "sessionhook.h"

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
#include <ScaleClasses.h>
#include <QMessageBox>
#include <QMutexLocker>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QGraphicsColorizeEffect>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCountG2::selfserveByCountG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveByCountG2),
    s(NULL),
    hook_( h ),
    scaleConfig_( NULL ),
    byCountQtyPopup_( NULL ),
    //activeKey_( NULL ),
    //autoLookupProduct_( false ),
    //autoLookupNumOfDigits_( 0 ),
    //autoLookupOverrideDigit_( 0 ),
    m_( QMutex::Recursive ),
    //validator_( NULL ),
    isByCountPopup_( false )
{
    ui->setupUi(this);

    initialize();
    translate();

    connect( hook_,       SIGNAL( reloadImages() ),              SLOT( onReloadImages() ) );
    //connect( quantity_,   SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );
    //connect( quantity_,   SIGNAL( lookup() ),                    SIGNAL( lookup() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCountG2::~selfserveByCountG2()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    if( byCountQtyPopup_ != NULL)
    {
        //delete byCountQtyPopup_;
        byCountQtyPopup_->deleteLater();
        byCountQtyPopup_ = NULL;
    }

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::refresh()
{
    if( !scaleConfig_ )
    {
        s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    if( scaleConfig_->internalConfig.activate_selfservice )
        return;

    if( scaleConfig_->currentPLU.productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT )
        return; // if we are updating a random weight, why update the images and FS data for a BC?

    ui->expandedTextArea->refresh();
    refreshPriceAndCount();

    // update item code/description
    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    QString itemDescription( hook_->fieldValue( sessionHooks::itemDescription ) );

    plu_->setText( itemCode );
    desc_->setText( itemDescription.toUpper() );

    update();
}

void selfserveByCountG2::refreshPriceAndCount()
{
    // no need to populate by count when no total price... there would be no by count in that scenario!

    QString bc = hook_->fieldValue( sessionHooks::itemByCount );
    QString tp = hook_->fieldValue( sessionHooks::itemTotalPrice );

    if ( hook_->fieldValue( sessionHooks::itemTotalPrice ).isEmpty() )
        byCount_->clear();
    else
        byCount_->setText( bc );

    QString unitPrice( hook_->fieldValue( sessionHooks::itemUnitPrice ) );
    QString unitPriceCountFor( hook_->fieldValue( sessionHooks::itemUnitPriceCountFor ) );
    bool itemHasExceptionPricing( "1" == hook_->fieldValue( sessionHooks::itemHasExceptionPricing ) );

    // strip lbs or kgs pricing method
    unitPrice.replace( " /lb", "" );
    unitPrice.replace( " /kg", "" );

    if (( itemHasExceptionPricing ) && ( hook_->fieldValue( sessionHooks::itemByCount ).toInt() < unitPriceCountFor.toInt() ))
        unitPrice = hook_->fieldValue( sessionHooks::itemExceptionPrice );
    else if ( 1 < unitPriceCountFor.toInt() )
    {
        if( 1 == scaleConfig_->printSpecialFields.unitPriceFormat().toInt() )
        {
            QString prefix( "%1 %2 " );

            unitPrice.prepend( prefix.arg( unitPriceCountFor ).arg( tr( "for" ) ) );

            //sizeUnitPriceToFit = true;
        }
    }

    //if( unitPrice.contains(tr(" For ")) || checkScreenSize() == 10 )
    //    sizeUnitPriceToFit = true;

    // I think the message is lying to me, get it direct from the PLU

    DomPrintSpecialFields *labelconfig = &scaleConfig_->printSpecialFields;
    DomCurrentPLU *currentPLUData = &scaleConfig_->currentPLU;
    QString unitPr = currentPLUData->unitPrice();
    if( ( 3 == currentPLUData->productType() ) && ( unitPr.toFloat() > 0 ) && ( "" == tp ) )
    {
        QString unitPf = labelconfig->unitPriceFormat(); // hey Mr. Crew - make sure your code builds ;)
        if( 3 == labelconfig->unitPriceFormat().toInt() )
            unitPrice = QString::number( currentPLUData->byCount() ) + " / $" + unitPr;        // 1 / $2.26
        else if( 4 == labelconfig->unitPriceFormat().toInt() )
            unitPrice = QString::number( currentPLUData->byCount() ) + " For $" + unitPr;        // 1 / $2.26

    }

    unitPrice_->setText(unitPrice );
    totalPrice_->setText( tp );
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::translate()
{
    ui->retranslateUi(this);

    if( byCountQtyPopup_ )
        byCountQtyPopup_->translate();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::resetKeypad()
{
    //ui->stackByCount->setCurrentIndex( 1 );    // 0 = keypad, 1=Print&Scan
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::resetKeypadDisplay()
{
    //quantity_->clear();
}

void selfserveByCountG2::resetEntry()
{
    //quantity_->clear();
    //ui->barcodeOnly->setVisible( true );
    ui->barcodeOnly->show();

    byCountQtyPopup_->show();
    isByCountPopup_ = true;
    scaleConfig_->internalConfig.selfServicePreventClicks = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::resizeEvent( QResizeEvent *e )
{
    renderLabelImage();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::showEvent( QShowEvent *e )
{
    bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );

    QFont ssfont;
    ssfont.setFamily( "Poppins" );
    ssfont.setBold( true );

    if( metricScale )
    {
        ssfont.setPointSize( 36 );
        byCount_->setFont( ssfont );
        unitPrice_->setFont( ssfont );
        totalPrice_->setFont( ssfont );

        byCount_->move( 20,33 );
        unitPrice_->move( 10,33 );
        totalPrice_->move( 20,33 );
    }
    else
    {
        ssfont.setPointSize( 42 );
        byCount_->setFont( ssfont );
        unitPrice_->setFont( ssfont );
        totalPrice_->setFont( ssfont );

        byCount_->move( 20,36 );
        unitPrice_->move( 10,36 );
        totalPrice_->move( 20,36 );
    }

    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    plu_->setText( itemCode );

    //quantity_->clear();
    //ui->barcodeOnly->setVisible( false );
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::white );
    p.setColor( foregroundRole(), Qt::white );
    setPalette( p );
    ui->barcodeOnly->setPalette( p );
    ui->barcodeOnly->update();

    ui->barcodeOnly->hide();

    resetScreen();

    popNumberPad( true );

    _Mybase::showEvent( e );
}

void selfserveByCountG2::hideEvent( QHideEvent *e)
{
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::white );
    p.setColor( foregroundRole(), Qt::white );
    setPalette( p );
    ui->barcodeOnly->setPalette( p );
    ui->barcodeOnly->update();

    desc_->setText( "" );
    plu_->setText( "" );
    totalPrice_->setText( "" );
    unitPrice_->setText( "" );
    byCount_->setText( "" );
    ui->barcodeOnly->hide();

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::mousePressEvent( QMouseEvent *e )
{
    if ( Qt::LeftButton == e->button() )
    {

        if( isByCountPopup_ )
        {
            // Bycount popup went away becasue it lost focus - pop it back up
            byCountQtyPopup_->show();
            isByCountPopup_ = true;
            scaleConfig_->internalConfig.selfServicePreventClicks = false;
        }
        else
        {
            //hook_->emitPrint();

            int clickX = e->globalX();
            int clickY = e->globalY();
            int screen_h = getScreenHeight();

            // See if the user clicked the quantity field
            int bc_w = ui->wgtQuantityBox->geometry().width();
            int bc_h = ui->wgtQuantityBox->geometry().height();

            if( ( clickX <= bc_w ) && ( clickY >= ( screen_h - bc_h ) ) )
                popNumberPad( true );
        }
    }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onNumberPadTextChanged( const QString& text )
{
    QString byCount( text );

    if ( byCount.isEmpty() )
        byCount = "0";

    // update by count
    hook_->emitChangeFieldValue( sessionHooks::itemByCount, byCount );

    emit hook_->resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onShowBackButton()
{
    hook_->resetBackButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onReloadImages()
{
    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfserveByCountG2", "renderBarcodeOnlyLabel" );

    LOG_DEBUG( "renderBarcodeOnlyLabel() call renderLabelImage() in numpad to refresh barcode label on bycount screen with file: %s", qPrintable(hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile )) );

    renderLabelImage();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::renderLabelImage()
{
    LOG_CLASS_INFO( "selfserveByCountG2", "renderLabelImage" );

    QMutexLocker lock( &m_ );

    int bgSize = 308;
    int printHeight( 240 );

    ui->barcodeOnly->setAutoFillBackground( true );
    ui->barcodeOnly->setFixedSize( bgSize - 2, printHeight - 2 );       // (306,238)

    QString label( hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile ) ); // from 101,4 message

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

    if( labelValid )
    {
        // set new palette
        ui->barcodeOnly->setPalette( p );

        ui->barcodeOnly->hide();
        ui->barcodeOnly->show();
    }
    else
    {
        QPalette p( palette() );
        p.setColor( backgroundRole(), Qt::white );
        p.setColor( foregroundRole(), Qt::white );
        setPalette( p );
        ui->barcodeOnly->setPalette( p );
        ui->barcodeOnly->hide();
    }

    ui->barcodeOnly->update();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfserveByCountG2", "onButtonClicked" );

    // handle button
    if ( printLabel_ == w )
    {
        if( "1" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog ) )
        {
            LOG_DEBUG( "BC onPrint() selfServiceRestrictedDialog = 1 so BAIL", NULL );
            return;
        }

        if ( scaleConfig_->internalConfig.selfServicePreventClicks )
            return;

        scaleConfig_->internalConfig.selfServicePreventClicks = true;

        QString bc = byCount_->text();
        if( ( 0 == bc.length() ) || ( 0 == bc.toInt() ))
            onPrintError( ProductDevError::ProductErrPrintBadByCount );
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
void selfserveByCountG2::onKeypadTimeout()
{
    //if( activeKey_ )
    //{
    //    activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
    //    activeKey_ = NULL;
    //}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onByCountPopupEnterClicked( const QString &quantity )
{
    hook_->emitChangeFieldValue( sessionHooks::itemByCount, quantity );

    if( ( quantity.toInt() > 0 ) && ( quantity.length() > 0 ) )
    {
        ui->barcodeOnly->show();
    }
    else
    {
        ui->barcodeOnly->hide();
        byCountQtyPopup_->hide();                   // I'm gonna hide this, but bring it back after the user clicks ok on dialog
        isByCountPopup_ = false;
        return;                                     //dont do anything else if no valid value
    }

    // Hide Prompt for Quantity
    byCountQtyPopup_->hide();
    isByCountPopup_ = false;

    resetScreenOpacity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onCancelClicked()
{
    // dump current item (should not have one here)
    hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
    hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onPrintError( int errorNum )
{
    hook_->session()->send( Error, ErrProductDevice, QString::number( errorNum ) + ",5" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::onResetIdleInactive()
{
    hook_->resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::setAutoLookupProduct( bool value )
{
    Q_UNUSED ( value );
    //autoLookupProduct_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::setAutoLookupNumOfDigits( int value )
{
    Q_UNUSED ( value );
    //autoLookupNumOfDigits_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::setAutoLookupOverrideDigit( int value )
{
    Q_UNUSED ( value );
    //autoLookupOverrideDigit_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::resetScreen()
{
    //quantity_->clear();
    //activeKey_ = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    screen_->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    screen_->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
    else
        screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(255,0,0); }" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::popNumberPad( bool popit)
{
    Q_UNUSED ( popit );

    dimScreenOpacity();

    //QGraphicsColorizeEffect* colorizeEffect = new QGraphicsColorizeEffect( this );
    //colorizeEffect->setColor( Qt::black );
    ////colorizeEffect->setStrength( .5 );
    //ui->screenContainer->setGraphicsEffect( colorizeEffect );

    //ui->barcodeOnly->setVisible( false );
    ui->barcodeOnly->hide();

    isByCountPopup_ = true;
    scaleConfig_->internalConfig.selfServicePreventClicks = false;

    if( byCountQtyPopup_ == NULL )
    {
        // Pop Prompt for Quantity
        byCountQtyPopup_ = new selfserveByCountQtyG2( "", this );
        int loc_x = ( getScreenWidth() / 2 ) - ( ui->wgtScanAndPrint->width() / 2 );
        byCountQtyPopup_->move( loc_x, 50 );
        byCountQtyPopup_->show();

        // listening for SIGNALS from the bycount keypad editor
        connect( byCountQtyPopup_, SIGNAL( enterClicked( QString ) ), SLOT( onByCountPopupEnterClicked( QString )) );
        connect( byCountQtyPopup_, SIGNAL( cancelClicked( ) ),         SLOT( onCancelClicked( ) ) );
        connect( byCountQtyPopup_, SIGNAL( printError( int ) ),         SLOT( onPrintError( int ) ) );                      // due to adding Print/Enter key to numPad
    }
    else
    {
        if( byCountQtyPopup_ )
        {
            // pop keypad
            byCountQtyPopup_->show();
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCountG2::initialize()
{
    s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // load images
    onReloadImages();

    plu_                = ui->lblPluNum;
    desc_               = ui->lblDescription;
    byCount_            = ui->lblByCount;
    totalPrice_         = ui->lblTotalPrice;
    unitPrice_          = ui->lblUnitPrice;

    printLabel_         = ui->butPrint;
    startOver_          = ui->butStartOver;
    back_               = ui->butBack;

    barcode_            = ui->barcodeOnly;
    scanAndPrint_       = ui->wgtScanAndPrint;

    screen_             = ui->screenContainer;

    // set up plu image area
    ui->expandedTextArea->setSelfServiceHook( hook_ );

    // load images
    onReloadImages();

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( printLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printLabel_, printLabel_ );

    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    connect( back_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( back_, back_ );

    connect(byCountQtyPopup_, SIGNAL( resetIdleInactive( QString ) ), SLOT( onResetIdleInactive()) );

    startOver_->setIcon( QIcon(":/images/screens/startover-icon.png" ) );
    startOver_->setIconSize(QSize(27,27));

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveByCountG2::getScreenWidth()
{
    int screen_width = 1366;        // 14 inch since thats what we are starting with

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        screen_width = resolution_.width();
    }

#ifdef QT_DEBUG
    screen_width = 1366;
#endif

    //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

    return screen_width;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveByCountG2::getScreenHeight()
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
} // hobart namespace
