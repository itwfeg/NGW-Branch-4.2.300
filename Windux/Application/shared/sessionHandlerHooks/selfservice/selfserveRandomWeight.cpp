/**
 * @file selfserveRandomWeight.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveRandomWeight.h"
#include "ui_selfserveRandomWeight.h"
#include "selfservicesoftkeybutton.h"

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

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveRandomWeight::selfserveRandomWeight( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveRandomWeight),
    s(NULL),
    hook_( h ),
    scaleConfig_(NULL),
    hideBigPrintKey( 0 ),
    m_( QMutex::Recursive )
{
    ui->setupUi(this);

    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveRandomWeight::~selfserveRandomWeight()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
        ssTareFlashkeys_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::refresh()
{
    //LOG_CLASS_INFO( "selfserveRandomWeight", "refresh" );

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

    // update item code/description
    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    QString itemDescription( hook_->fieldValue( sessionHooks::itemDescription ) );

    // pjr-383: if there is too much text, it needs to be smaller to fit the screen
    // if really long, may need to fit the text below the item code as well....
    QFont font;

    int screenSize = checkScreenSize();
    if(screenSize == 10)
    {
        if(itemDescription.size() > 27)
        {
            font.setPointSize( 32 );
        }
        else
        {
            font.setPointSize( 38 );
        }
    }
    else
    {
        if(itemDescription.size() > 27)
        {
            font.setPointSize( 40 );
        }
        else
        {
            font.setPointSize( 48 );
        }
    }

    font.setBold( true );

    ui->lblItemCode->setWordWrap( true );
    ui->lblItemCode->setFont( font );

    if ( itemCode.isEmpty() )
        ui->lblItemCode->clear();
    else if ( itemDescription.isEmpty() )
        ui->lblItemCode->setText( /*"PLU #: " +*/"#" + itemCode );           // display the item code with the words Item Code
    else
        ui->lblItemCode->setText( /*"PLU #: " +*/"#" + itemCode + ' ' + itemDescription );           // display the item code with the words Item Code

    ui->nutrifactsArea->refresh();
    ui->expandedTextArea->refresh();

    if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
        ssTareFlashkeys_->sendResetMessage( true );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::translate()
{
    ui->retranslateUi(this);

    if( hideBigPrintKey )
        return;

    ui->nutrifactsArea->translate();
    ui->expandedTextArea->translate();

    if( 0 == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) )
        ui->lblWeighItem->setText( tr("Place\nproduct on\nscale,\nthen press\nPrint Label.") );

    if( SelfServicePrintOption::SelfServOption_PrintToScreen == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        ui->instructPrintKey->setText( tr("Scan Label\n" ) );
    else if( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        ui->instructPrintKey->setText( tr("Scan or Print Label\n" ) );
    else
        ui->instructPrintKey->setText( tr("Print Label\n" ) );

    if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
        ssTareFlashkeys_->sendResetMessage( true );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "selfserveRandomWeight", "resizeEvent" );

    // FT-1183: do not allow widgets to resize too small
    int layoutWidth( ui->itemInfoLayout->geometry().width() );
    int layoutHeight( ui->itemInfoLayout->geometry().height() );

    ui->nutrifactsArea->setMinimumWidth( (40 * layoutWidth) / 100 );
    ui->expandedTextArea->setMinimumWidth( (40 * layoutWidth) / 100 );

    int t3_width = ( ui->expandedTextArea->geometry().width() );
    int t3_hight = ( ui->expandedTextArea->geometry().height() );

    int screensize = checkScreenSize();
    if( "1" == hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel) )
    {
        int bgSize = 318;                       // 14 inch
        int npSize = 480;                       // 14 inch
        int labelHeight( 304 );
        int printHeight( 178 );

        if( 10 == screensize )
        {
            bgSize = 268;
            npSize = 416;
            labelHeight = 180;
        }

        ui->rwStackedWidget->setFixedSize( bgSize - 10, npSize + 70);                               // RESIZE = bgSize
        ui->barcodeOnly->setFixedSize( (bgSize - 12), printHeight - 2 );                            // lets hardcode it
        ui->bigPrintKey->setFixedSize( (bgSize - 12), printHeight - 3 );

        ui->lblScanBarcode->setFixedSize( bgSize - 12, 35 );
    }
    else
    {
        int instructWidth = 358;       // 318 in numpad    (1st) 289
        if( 10 == screensize )
            instructWidth = 239;

        ui->rwStackedWidget->setMinimumWidth( instructWidth );
        ui->rwStackedWidget->setMaximumWidth( instructWidth );
    }

    if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
    {
        ssTareFlashkeys_->resetSelfServTareFlashkey();

        if( scaleConfig_->tareFlashkeysDoExist )
        {
            ui->tareKeyArea->setMinimumWidth(  (40 * layoutWidth)  / 100 );
            ui->tareKeyArea->setMinimumHeight( (50 * layoutHeight) / 100 );
            ui->expandedTextArea->setMinimumHeight( (50 * layoutHeight) / 100 );
        }
        else
        {
            ui->expandedTextArea->setMinimumHeight( layoutHeight );
            ui->tareKeyArea->setMinimumWidth( 0 );
            ui->tareKeyArea->setMinimumHeight( 0 );
        }
    }
    else
    {
        ui->expandedTextArea->setMinimumHeight( layoutHeight );
        ui->tareKeyArea->setMinimumWidth( 0 );
        ui->tareKeyArea->setMinimumHeight( 0 );
    }

    t3_width = ( ui->expandedTextArea->geometry().width() );
    t3_hight = ( ui->expandedTextArea->geometry().height() );

    //LOG_DEBUG( "Resize() expandedTextArea: %dx%d", ui->expandedTextArea->width(), ui->expandedTextArea->height() );

    if( scaleConfig_->scaleConfig.enableHelpPrompts() )
    {
        // we dont need this at all if we are displaying help images
        ui->lblWeighItem->setMinimumWidth( 0 );
        ui->lblWeighItem->setMaximumWidth( 0 );
        ui->rwStackedWidget->setMinimumWidth( 0 );
        ui->rwStackedWidget->setMaximumWidth( 0 );
        ui->lblWeighItem->setText("");
    }
    else
    {
        if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        {
            ui->rwStackedWidget->setCurrentIndex( 1 );      // 1 = scan and print

            LOG_DEBUG( "CALL renderLabelImage(false) from resize()", NULL );
            renderLabelImage();
        }
        else
        {
            // FT-1268: restrict how small the weigh item message can be
            int bgWidth = 306;
            int printHeight( 208);

            if( 10 == checkScreenSize() )
            {
                bgWidth = 257;                        // 260->260  = -61

                QFont f( font() );
                f.setPointSize( 28);
                ui->lblWeighItem->setFont( f );
            }

            ui->rwStackedWidget->setCurrentIndex( 0 );                               // 0 = instructions

            ui->rwStackedWidget->setFixedSize( bgWidth + 4, 692);                     // ORIG = 316 x 692
            ui->lblWeighItem->setFixedSize( bgWidth, 280 );                           // 290->280    ORIG = 316 x 300

            ui->instructPrintKey->setFixedSize( bgWidth, printHeight );      // 20->30      ORIG: 316x178
        }
    }

    if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
        ssTareFlashkeys_->sendResetMessage( true );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::mousePressEvent( QMouseEvent *e )
{
    if ( Qt::LeftButton == e->button() )
    {
        // pjr383 - use original test
        // eliminate double clicks until done
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        // PJR413 (SOF-4772) - Prevent clicks if we are in Fixed PLU mode
        if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" ){
            return;
        }

        hook_->emitPrint();
    }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::onButtonClicked( QWidget *w )
{
    if ( printLabel_ == w )
    {
        // pjr383 - use original test
        // have to handle clicks and double clicks even if we are ready or not. But I dont want to handle them again
        // if I havent drawn the print screen in the first place. so only allow again once Ive displayed the print screen

        if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        {
            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        hook_->emitPrint();
    }
    else if( instructPrintLabel_ == w )
    {
        if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        {
            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        hook_->emitPrint();
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::onReloadImages()
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

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfserveRandomWeight", "renderBarcodeOnlyLabel" );

    LOG_DEBUG( "renderBarcodeOnlyLabel() call renderLabelImage()", NULL );
    renderLabelImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::renderLabelImage()
{
    LOG_CLASS_INFO( "selfserveRandomWeight", "renderLabelImage" );

    // SOF-5010 PJR-413 - QR Code shows weight in self-serve for Walmart Canada
    // hide QR Code when < min weight to print
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
            //LOG_DEBUG( "renderLabelImg() HIDE incomingWt %s < minWtPrint %s", qPrintable( QString::number(incomingWt) ), qPrintable( QString::number(minWtPrint) ) );
            ui->scanAndPrint->hide();
            return;
        }
        else
        {
            //LOG_DEBUG( "renderLabelImg() SHOW incomingWt %s >= minWtPrint %s", qPrintable( QString::number(incomingWt) ), qPrintable( QString::number(minWtPrint) ) );
            ui->scanAndPrint->show();
        }
    }

    //PJR413 (SOF-4772) - Didn't feel like typing this every time
    hideBigPrintKey = ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == 1 && scaleConfig_->scaleConfig.selfServeDisplayBarcodeLabel() == 1);


    QMutexLocker lock( &m_ );

    //LOG_DEBUG( "selfserveRandomWeight::renderLabelImage() START", NULL );

    int bgSize = 318;                       // 14 inch
    int npSize = 480;                       // 14 inch

    int screensize = checkScreenSize();
    int screenheight = checkScreenHeight();

    int labelHeight( 304 );
    int printHeight( 178 );
    int spacing( (screenheight - ((printHeight * 2) + 35) ) / 2 );

    if( 10 == screensize )
    {
        bgSize = 268;
        npSize = 416;

        labelHeight = 180;
        spacing = 20;
    }

    ui->rwStackedWidget->setFixedSize( bgSize - 10, npSize + 70 );                                  // RESIZE = bgSize
    ui->barcodeOnly->setAutoFillBackground( true );
    ui->barcodeOnly->setFixedSize( bgSize - 12, printHeight - 2 );

    // Show for debugging
    ui->scanAndPrint->show();

    // PJR413 (SOF-4772) - If we are only showing the QR code, hide the bigPrintKey
    if( hideBigPrintKey )
    {
        ui->lblScanBarcode->setText( tr("SCAN CODE") ); // QR Code not Barcode will be displayed
        ui->bigPrintKey->setFixedSize( 306, 1);         // adjust height if you want to push the barcode down
    }
    else
    {
        ui->lblScanBarcode->setText( tr("SCAN BARCODE") );
        ui->bigPrintKey->setFixedSize( bgSize - 12, printHeight - 3 );
    }

    ui->lblScanBarcode->setFixedSize( bgSize - 12, 35 );
    ui->topSpacer->setGeometry( QRect( 1, 1, 20, spacing) );
    ui->bottomSpacer->setGeometry( QRect( 1, 1, 20, spacing) );

    QString label( hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile ) ); // from 101,4 message

    //LOG_DEBUG( "renderLabelImage() current TotalPrice: %s", qPrintable(hook_->fieldValue( sessionHooks::itemTotalPrice )) );
    //LOG_DEBUG( "barcode label name: %s", qPrintable(label) );

    if( "" == label )
    {
        LOG_DEBUG( "renderLabelImage() weight/file is blank or ---- so hide it", NULL );

        // this method is called by 101,4 (print) or in here, a 101,15(total) and the weight is a bad weight. Might be existing or not.
        ui->scanAndPrint->hide();
        LOG_DEBUG( "selfserveRandomWeight::renderLabelImage() Blank Label return END", NULL );
        return;
    }
    else
    {
        //LOG_DEBUG( "renderLabelImage() value exists so display it if one exists so try and display it", NULL );
        if( !hideBigPrintKey )
        {
            ui->bigPrintKey->setStyleSheet(QLatin1String("color: white;\n" "border: 3px solid rgb(255,255,255);"));
            ui->bigPrintKey->setText( tr("Print Label") );
            printLabel_->setText( tr("Print Label") );
        }
        ui->scanAndPrint->show();
    }

    // New Scaling
    bool labelValid( false );

    QPalette p( ui->barcodeOnly->palette() );

    if ( !label.isEmpty() )
    {
        //LOG_DEBUG( "renderLabelImage() label.isEmpty() is false so check if label is null", NULL );

        QPixmap image( label );
        if ( !image.isNull() )
        {
            //LOG_DEBUG( "renderLabelImage() label is not null so render it", NULL );

            // create pixmap
            QPixmap dest( (bgSize - 17), (printHeight - 7) );

            if( dest.isNull() )
                LOG_DEBUG( "renderLabelImage() dest.isNull() is TRUE so this update() WILL FAIL", NULL );

            dest.fill( Qt::transparent );
            {
                // create painter
                QPainter painter( &dest );
                painter.setRenderHint( QPainter::Antialiasing, true );

                // Got rid of this
                // painter.setBackgroundMode( Qt::TransparentMode );

                // draw pixmap
                painter.begin( &dest );
                //LOG_DEBUG( "renderLabelImage() begin it: %d",  beginit);

                painter.drawPixmap( 0, 0, dest.width(), (printHeight - 7), image );     // painter.drawPixmap( 0, 0, ( bgSize + 50 ), (printHeight - 7), image );

                painter.end();
                //LOG_DEBUG( "renderLabelImage() end it: %d",  endit);
            }

            //LOG_DEBUG( "renderLabelImage() backgroundRole() ColorRole: %d",  backgroundRole());
            p.setBrush( backgroundRole(), dest );
            labelValid = true;
        }
        else
            LOG_DEBUG( "renderLabelImage() image.isNull = true so dont do anything", NULL );
    }
    else
        LOG_DEBUG( "renderLabelImage() label.isEmpty() label is empty so didn't display it", NULL );

    if ( !labelValid )
    {
        LOG_DEBUG( "!labelValid - Label is not valid so make the background white. Dont make it white anymore just return", NULL );
        //p.setColor( backgroundRole(), Qt::white );
        return;
    }

    // set new palette
    ui->barcodeOnly->setPalette( p );

    ui->barcodeOnly->hide();
    ui->barcodeOnly->show();

    ui->barcodeOnly->update();

    //LOG_DEBUG( "selfserveRandomWeight::renderLabelImage() END\n", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveRandomWeight::checkScreenSize()
{
    // laptop (1920x1080)
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveRandomWeight::checkScreenHeight()
{
    int screen_height = 600;        // 10 inch

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        // pjr383 TODO - ummmmm I need the width and Im returning the height?  What the heck? But it's working do I dare touch it LOL?
        screen_height = resolution_.width();
    }

    return screen_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveRandomWeight::initialize()
{
    s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // SOF-5010 initialize the min weight to print
    minWeightToPrint_ = hook_->fieldValue( sessionHooks::minWeightToPrint ).toDouble();

    //PJR413 (SOF-4772) - Didn't feel like typing this every time
    hideBigPrintKey = ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() == 1 && scaleConfig_->scaleConfig.selfServeDisplayBarcodeLabel() == 1);

    // load images
    onReloadImages();

    // Tare Flashkeys
    ssTareFlashkeys_ = new hobart::softkeysWidget( hook_->session(), ui->tareKeyArea );
    ssTareFlashkeys_->setAutoFillBackground( true );

    ssTareFlashkeys_->setCreateFilter( hobart::softkeysWidget::PresetTareGrid );

    ssTareFlashkeys_->setFactory( new selfServiceSoftkeyButtonFactory() );
    connect( ssTareFlashkeys_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );
    ui->tareKeyArea->layout()->addWidget( ssTareFlashkeys_ );

    // setup nutrifacts widget
    ui->nutrifactsArea->setSelfServiceHook( hook_ );
    ui->nutrifactsArea->setDisplayFreqShopper( true );

    // setup item expanded text widget
    ui->expandedTextArea->setSelfServiceHook( hook_ );
    ui->expandedTextArea->setDisplayFreqShopper( false );

    // What if Larrys feature and Tare Flashkeys turned on at the same time.
    // I think that the tare flashkeys want to fill this print area too
    // Do we need to make the 2 features mutually exclusive in Normal Scale Ops?
    // If they send it COMM then how can we prevent that?

    int bgSize = 318;
    int npSize = 480;
    int screensize = checkScreenSize();

    if( screensize == 10 )
    {
        bgSize = 268;   // 273
        npSize = 416;
    }

    int printHeight( 178 );

    const hobart::backgroundWidgetRenderer renderGreenButton( new hobart::panelButtonBackgroundWidget( 4 ), new hobart::greenButtonBackgroundWidgetRendererAppearance() );
    const QPixmap greenButtonPixmap( renderGreenButton.drawControl( QSize( bgSize - 12, printHeight ), hobart::ReadOnly ) );
    buttonSignalMapper_ = new QSignalMapper( this );

    // pjr383 - leave as is
    if( scaleConfig_->normalScaleOperation.selfServeDisplayBarcodeLabel() )
    {
        connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

        if( !hideBigPrintKey )
        {
            printLabel_ = new hobart::pushButton( ui->bigPrintKey, greenButtonPixmap );
            QFont font( printLabel_->font() );

            if( screensize == 10 )
                font.setPointSize( 20 );
            else
                font.setPointSize( 24 );

            printLabel_->setFont( font );
            connect( printLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
            buttonSignalMapper_->setMapping( printLabel_, printLabel_ );
            ui->bigPrintKey = printLabel_;
        }

        ui->rwStackedWidget->setFixedSize( bgSize, npSize + 70);
        ui->scanAndPrint->setFixedSize( bgSize, npSize + 70);

        ui->layPrintAndScan->setContentsMargins(2, 2, 2, 2);
        ui->barcodeOnly->setFixedSize( bgSize - 12, printHeight - 2 );

        if( !hideBigPrintKey )
        {
            ui->lblScanBarcode->setFixedSize( bgSize - 12, 35 );
            ui->bigPrintKey->setFixedSize( bgSize - 12, printHeight - 3 );
            printLabel_->setFixedSize(  bgSize - 2, printHeight - 2 );
        }

        ui->rwStackedWidget->setCurrentIndex( 1 );      // 1 = scan and print

        if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
            ui->lblItemCode->hide();
    }
    else if( scaleConfig_->scaleConfig.enableHelpPrompts() )
    {
        // This piece is actually tied to the fact if we are displaying Tare, the tare images will display in this area
        // we dont need this at all if we are displaying help images
        ui->rwStackedWidget->setCurrentIndex( 0 );      // 0 = instructions
        ui->lblWeighItem->setText("");
    }
    else
    {
        connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

        instructPrintLabel_ = new hobart::pushButton( ui->instructPrintKey, greenButtonPixmap );
        QFont font( instructPrintLabel_->font() );

        if( screensize == 10 )
            font.setPointSize( 20 );
        else
            font.setPointSize( 24 );

        instructPrintLabel_->setFont( font );
        connect( instructPrintLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
        buttonSignalMapper_->setMapping( instructPrintLabel_, instructPrintLabel_ );
        ui->instructPrintKey = instructPrintLabel_;

        if( SelfServicePrintOption::SelfServOption_PrintToScreen == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
            ui->instructPrintKey->setText( tr("Scan Label\n" ) );
        else if( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
            ui->instructPrintKey->setText( tr("Scan or Print Label\n" ) );
        else
            ui->instructPrintKey->setText( tr("Print Label\n" ) );

        ui->rwStackedWidget->setCurrentIndex( 0 );      // 0 = instructions
        // FT-1268: restrict how small the weigh item message can be
        ui->lblWeighItem->setText("Place\nproduct on\nscale,\nthen press\nPrint Label.");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
