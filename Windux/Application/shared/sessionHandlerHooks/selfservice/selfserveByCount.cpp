/**
 * @file selfserveByCount.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveByCount.h"
#include "ui_selfserveByCount.h"
#include "sessionhook.h"

#include "selfservicenumberpad.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <filesystem.h>
#include <pixmap.h>
#include <pushbutton.h>
#include <selfservicehook.h>
#include <ScaleConfiguration.h>
#include <pushbutton.h>
#include <QIntValidator>
#include <QMouseEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QSignalMapper>
#include <ScaleClasses.h>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCount::selfserveByCount( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveByCount),
    s(NULL),
    hook_( h ),
    scaleConfig_(NULL)
{
    ui->setupUi(this);

    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );

    connect( ui->numPad,           SIGNAL( textChanged(QString) ),        SLOT( onNumberPadTextChanged(QString) ) );
    connect( ui->numPad,           SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );
    connect( ui->numPad,           SIGNAL( print() ),                     SIGNAL( print() ) );                  // due to adding Print/Enter key to numPad
    connect( ui->numPad,           SIGNAL( printError(int) ),             SLOT( onPrintError(int) ) );          // due to adding Print/Enter key to numPad
    connect( ui->numPad,           SIGNAL( showBackButton() ),            SLOT( onShowBackButton() ) );         // due to adding Print/Enter key to numPad
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveByCount::~selfserveByCount()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::refresh()
{
    //LOG_CLASS_INFO( "selfserveByCount", "renderBarcodeOnlyLabel" );

    if( !scaleConfig_ )
    {
        s = hook_->session();
        scaleConfig_ = s->scaleConfig;
    }

    if( scaleConfig_->internalConfig.activate_selfservice )
        return;

    if( scaleConfig_->currentPLU.productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT )
        return; // if we are updating a random weight, why update the images and FS data for a BC?

    // update number pad style
    ui->numPad->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );

    // due to adding Print/Enter key to numPad. The key is an ENTER key(0) to do a PLU Lookup or a Print Key(1)
    ui->numPad->setEnterResponse(1);        // 1 = Print Action

    const QString enterText = ui->numPad->enter()->text();
    const QString prompt = tr( "Enter quantity, then press") + " " + tr( enterText.toStdString().c_str() ) + ".";
    sessionHooks::instance()->updateFieldValue(sessionHooks::selfServiceByCountPrompt, enterText);

    // Kinda reversed this so it only says Print when we are in Print Label Only mode
    if( ( SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) &&
        ( !scaleConfig_->scaleConfig.selfServeDisplayBarcodeLabel() ) )
    {

         ui->numPad->setEnterText(1);            // 1 = Print Text
         ui->numPad->setInstructions(prompt);
    }
    else
    {
        ui->numPad->setEnterText(0);            // 0 = Enter Text
        ui->numPad->setInstructions(prompt);
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

    // update images
    ui->nutrifactsArea->refresh();
    ui->expandedTextArea->refresh();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::translate()
{
    ui->retranslateUi(this);

    const QString enterText = ui->numPad->enter()->text();
    const QString prompt = tr( "Enter quantity, then press") + " " + tr( enterText.toStdString().c_str() ) + ".";

    ui->numPad->setInstructions( prompt ) ;
    ui->numPad->translate();

    ui->nutrifactsArea->translate();
    ui->expandedTextArea->translate();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::resetKeypad()
{
    if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        ui->numPad->displayStackedNumWidget(0);     // 0 = keypad, 1=Print&Scan
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::resetKeypadDisplay()
{
    ui->numPad->clear();         //ui->numPad->setText("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveByCount::getStackState()
{
    int stackstate = 0;

    if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        stackstate = ui->numPad->numberPadStackState();     // did this bc I was having trouble getting the right state so did this so I could debug it

    return stackstate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::resizeEvent( QResizeEvent *e )
{
    // FT-1183: do not allow widgets to resize too small
    int layoutWidth( ui->itemInfoLayout->geometry().width() );

    ui->nutrifactsArea->setMinimumWidth( (40 * layoutWidth) / 100 );
    ui->expandedTextArea->setMinimumWidth( (40 * layoutWidth) / 100 );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::showEvent( QShowEvent *e )
{
    ui->numPad->blockSignals( true );

    resetKeypadDisplay();

    ui->numPad->displayStackedWidget(0);

    if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
    {
        ui->numPad->displayStackedNumWidget(0);     // 0 = keypad, 1=Print&Scan
        ui->numPad->setDisplayBarcodeOnUserInterface( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() );
    }

    ui->numPad->blockSignals( false );

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::mousePressEvent( QMouseEvent *e )
{
    // due to adding Print/Enter key to numPad. The key is an ENTER key(0) to do a PLU Lookup or a Print Key(1)

    if ( Qt::LeftButton == e->button() )
    {
        // pjr383 - use original test
        // eliminate double clicks until done
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            //selfServiceHook::state teststate = hook_->currentState();
            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        hook_->emitPrint();
    }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::onNumberPadTextChanged( const QString& text )
{
    QString byCount( text );

    if ( byCount.isEmpty() )
        byCount = "0";

    // update by count
    hook_->emitChangeFieldValue( sessionHooks::itemByCount, byCount );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::onPrintError(int errorNum )
{
     hook_->session()->send( Error, ErrProductDevice, QString::number(errorNum) + ",5" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::onShowBackButton()
{
    hook_->resetBackButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::onReloadImages()
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
void selfserveByCount::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfserveByCount", "renderBarcodeOnlyLabel" );

    LOG_DEBUG( "renderBarcodeOnlyLabel() call renderLabelImage() in numpad to refresh barcode label on bycount screen with file: %s", qPrintable(hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile )) );

    ui->numPad->setBarcodeFileName(  hook_->fieldValue( sessionHooks::selfServiceBarcodeOnlyFile ) );

    ui->numPad->renderBarcodeOnlyLabel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveByCount::initialize()
{
    s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // load images
    onReloadImages();

    // set number pad validator
    ui->numPad->setValidator( new QIntValidator( 0, 99, this ) );

    // setup nutrifacts widget
    ui->nutrifactsArea->setSelfServiceHook( hook_ );
    ui->nutrifactsArea->setDisplayFreqShopper( true );

    // setup item expanded text widget
    ui->expandedTextArea->setSelfServiceHook( hook_ );
    ui->expandedTextArea->setDisplayFreqShopper( false );
}

int selfserveByCount::checkScreenSize()
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
