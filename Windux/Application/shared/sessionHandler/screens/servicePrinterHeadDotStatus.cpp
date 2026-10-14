/**
 * @file servicePrinterHeadDotStatus.cpp
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author J Crew
 */

#include "servicePrinterHeadDotStatus.h"
#include "ui_servicePrinterHeadDotStatus.h"

#include <UiUtilities.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../../source/HFC/CfgBaseFieldOffsets.h"       // from chobart-d in /HFC

#include <QIntValidator>
#include <QTimer>

#include <dialogs/messageboxdialog.h>

static const int NoPrinter    = 0;          // Not sure if this is even possible, unless we have major issue and cant see it
static const int EPPPrinter   = 1;          // Standalone EPP Printer, but we are running out of parts for it, moving to EPP2.
static const int ApcPrinter   = 4;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int EPP2Printer  = 5;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int UsbPrinter   = 6;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int NetPrinter   = 7;          // Newest Atmel EPP Printer which is becoming the newer standard.

static const QString SelfService    = "1";  // Newest Atmel EPP Printer which is becoming the newer standard.
static const QString AccessWrapper  = "2";  // Access Wrapper
static const QString CounterScale   = "3";  // Newest Atmel EPP Printer which is becoming the newer standard.

static const bool USE_PRINTERS = false;
static bool runningTest = false;

servicePrinterHeadDotStatus::servicePrinterHeadDotStatus(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePrinterHeadDotStatus)
{
    ui->setupUi(this);

    if ( mainWindow_->isOperateMode() )
        connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    initialize();

    translate();

    if( codeCheckerEnabled_ )
    {
        startDotWearTest();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
servicePrinterHeadDotStatus::~servicePrinterHeadDotStatus()
{
    delete dotWearTimer;
    delete ui;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::translate()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    if( USE_PRINTERS )
    {
        ui->lblPrinterSelection->setText( tr("Printer") );

        // Printer Dialog
        ui->cbPrinterSelection->setItemText( 0, tr( "Primary" ) );

        if(scaleModelSetting == AccessWrapper)
        {
            ui->cbPrinterSelection->setItemText( 1, tr( "Second" ) );
            ui->cbPrinterSelection->setItemText( 2, tr( "Third" )  );
        }
        else if(scaleModelSetting == CounterScale)
        {
            // If we have an HT scale, see if we need to add some EPP printers to the combo
            if ( isEppPrinterAttached(2) )
                ui->cbPrinterSelection->setItemText( 1, "EPP" );                // We can only have one EPP printer attached if counter scale, so its the 2nd item
            else if( isUsbPrinterAttached(2) )
                ui->cbPrinterSelection->setItemText( 1, "USB" );

            if ( isEppPrinterAttached(3) )
                ui->cbPrinterSelection->setItemText( 2, "EPP" );
            else if( isUsbPrinterAttached(3) )
                ui->cbPrinterSelection->setItemText( 2, "USB" );
        }
    }

    ui->lblBadDots->setText(  tr("Bad\ndots: ") + QString::number(marginalDots_) );
    ui->lblMargDots->setText( tr("Marginal\ndots: ") + " " + QString::number(badDots_) );

    if( codeCheckerEnabled_ )
        ui->butEnableDotWear->setText( tr("Disable Code Checker") );
    else
        ui->butEnableDotWear->setText( tr("Enable Code Checker") ) ;

    update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool servicePrinterHeadDotStatus::onScreenChanging()
{
    // Copy this to when they click the button so I can update the screen
    if( codeCheckerEnabled_ != codeCheckerEnabledOriginal_ )
    {
       QString param;
       param += "," + QString::number( fieldOffsetCfgIsCodeCheckerEnabled);
       param += "," + QString::number( codeCheckerEnabled_ );
       param += "," + QString::number(codeCheckerEnabledOriginal_);

       session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }

    if( runningTest )
    {
        MessageBoxDialog dialog( mainWindow_ );

        dialog.setAllowOverlayClickDismiss( false );

        dialog.setText( tr( "Please wait for the current process to complete!" ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();

        return false;
    }

    return _Mybase::onScreenChanging();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::initPrinterValues(QString printerType)
{
    //----------------------------------------------------------------
    // Clears existing data and sends messages to request setting info
    //----------------------------------------------------------------
    currentPrinter = printerType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::initialize()
{
    //---------------------
    // Initialize dropdowns
    //---------------------
    ScaleConfiguration *scaleConfig( session_->scaleConfig );
    codeCheckerEnabled_ = scaleConfig->scaleConfig.isCodeCheckerEnabled();
    codeCheckerEnabledOriginal_ = codeCheckerEnabled_;             // Code Checker is the same as Dot Wear

    if( codeCheckerEnabled_ )
        ui->butEnableDotWear->setText( tr("Disable Code Checker") );
    else
        ui->butEnableDotWear->setText( tr("Enable Code Checker") ) ;

    badDots_ = 0;
    marginalDots_ = 0;

    dotWearTimer = new QTimer(this);
    dotWearTimer->setSingleShot(true);
    dotWearTimer->setInterval(30000); //30 sec timeout

    connect( dotWearTimer, SIGNAL(timeout()), this, SLOT(onDotWearTimerTimeout()));
    connect( ui->butEnableDotWear,      SIGNAL( clicked() ), this, SLOT( onEnableCodeCheckerClicked() ) );

    if( USE_PRINTERS )
    {
        // Printer selection
        ui->cbPrinterSelection->addItem( tr( "Primary" ), 0);

        if(session_->isPrepack())
        {
            ui->cbPrinterSelection->addItem( tr( "Second" ), 1);
            ui->cbPrinterSelection->addItem( tr( "Third" ),  2);
        }
        else
        {
            // If we have an HT scale, see if we need to add some EPP printers to the combo
            if ( isEppPrinterAttached(2) )
                ui->cbPrinterSelection->addItem( "EPP", ui->cbPrinterSelection->count() );
            else if( isUsbPrinterAttached(2) )
                ui->cbPrinterSelection->addItem( "USB", ui->cbPrinterSelection->count() );

            if ( isEppPrinterAttached(3) )
                ui->cbPrinterSelection->addItem( "EPP", ui->cbPrinterSelection->count() );
            else if( isUsbPrinterAttached(3) )
                ui->cbPrinterSelection->addItem( "USB", ui->cbPrinterSelection->count() );
        }

        //---------------------------------------------------------
        // Set the connection for when the Printer dropdown changes
        //---------------------------------------------------------
        connect(ui->cbPrinterSelection, SIGNAL( currentIndexChanged(int) ), this, SLOT( onPrinterChanged(int) ) );

        // Get the info for the Primary printer
        initPrinterValues("0");
    }
    else
    {
        ui->lblPrinterSelection->setEnabled( false );
        ui->cbPrinterSelection->setEnabled( false );
        ui->lblPrinterSelection->setVisible( false );
        ui->cbPrinterSelection->setVisible( false );
    }

    // Default values if everything is good - if I decide to do this (its kind of slow)

}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::onPrinterChanged(int index)
{
    Q_UNUSED( index );
    initPrinterValues((ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::onEnableCodeCheckerClicked()
{
    if( codeCheckerEnabled_ )
    {
        codeCheckerEnabled_ = false;
        ui->butEnableDotWear->setText( tr("Enable Code Checker") );
    }
    else
    {
        codeCheckerEnabled_ = true;
        ui->butEnableDotWear->setText( tr("Disable Code Checker") );
    }

    if( codeCheckerEnabled_ != codeCheckerEnabledOriginal_ )
    {
        badDots_ = 0;
        marginalDots_ = 0;

        codeCheckerEnabledOriginal_ = codeCheckerEnabled_;      // new original value

        ui->lblBadDotsResult->clear();
        ui->lblMarginalDotsResult->clear();

        ui->lblBadDots->setText(  tr("Bad\ndots: 0") );
        ui->lblMargDots->setText( tr("Marginal\ndots: 0") );

        // Reset dots before updating them.
        QList<QLabel *> labels = this->findChildren<QLabel*>();
        foreach ( QLabel *label, labels )
        {
            if( label->objectName().contains("lblDot"))
            {
                label->setText("O");
                label->setStyleSheet("color: green; background-color: white;");        // reset dots green
            }
        }

        QString param;
        param += "," + QString::number( fieldOffsetCfgIsCodeCheckerEnabled);
        param += "," + QString::number( codeCheckerEnabled_ );
        param += "," + QString::number(codeCheckerEnabledOriginal_);

        session_->send( ConfigurationDev, CfgSaveSingleField, param );

        // will this be too fast? Do I need to wait a second to let the config go thru first?
        if(codeCheckerEnabled_){
            startDotWearTest();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool servicePrinterHeadDotStatus::isUsbPrinterAttached(int printerLocation)
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    bool isAttached = false;

    if(scaleModelSetting == CounterScale)
    {
        if ( 2 == printerLocation )
        {
            if (periphSettings->secondPrinter()  == UsbPrinter )
                isAttached = true;
        }
        else if ( 3 == printerLocation )
        {
            if ( periphSettings->thirdPrinter() == UsbPrinter )
                isAttached = true;
        }
    }

    return isAttached;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool servicePrinterHeadDotStatus::isEppPrinterAttached(int printerLocation)
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    bool isAttached = false;

    if(scaleModelSetting == CounterScale)
    {
        if ( 2 == printerLocation )
        {
            if (periphSettings->secondPrinter()  == EPPPrinter  || periphSettings->secondPrinter()  == EPP2Printer )
                isAttached = true;
        }
        else if ( 3 == printerLocation )
        {
            if ( periphSettings->thirdPrinter() == EPPPrinter || periphSettings->thirdPrinter()  == EPP2Printer)
                isAttached = true;
        }
    }

    return isAttached;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void servicePrinterHeadDotStatus::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event  = msg[1].toInt();

    // Printer, DotStatus  7,179
    if (( Printer == device ) && ( PrHeadDotStatus   == event ))
    {
        DomPrHeadDotStatus m( msg );

        if( m.status() == 0 )
        {
            session_->send(Printer, PrReqHeadDotWear);
            ui->lblPercentDotsBad->setText( "0% " + tr("Bad dots") );// Request the Head Dot Wear
        }
        //dot wear failed
        else if( m.status() == 3 )
        {
            ui->lblPercentDotsBad->setText( tr( "Failed...") );

            MessageBoxDialog dialog( mainWindow_ );
            dialog.setAllowOverlayClickDismiss( false );

            dialog.setText( tr( "Printer Dot Wear failed to complete" ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );
            dialog.exec();
        }
        //dot wear aborted. Cassette is open.
        else if( m.status() == 4 )
        {
            ui->lblPercentDotsBad->setText( tr( "") );

            MessageBoxDialog dialog( mainWindow_ );
            dialog.setAllowOverlayClickDismiss( false );

            dialog.setText( tr( "Printer Cassette is open. Close cassette and try again." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );
            dialog.exec();
        }

        dotWearTestFinished();
    }

    // Printer, Dot wear 7,175
    else if (( Printer == device)  &&  ( PrHeadDotWear  == event ))
    {
        DomPrHeadDotWear m( msg );
        QString statusListStr = m.dotsStatusList();

        // DOT_GOOD = 0x00
        // DOT_BAD  = 0x01
        // DOT_SOSO = 0x02

        QString dotStr = "";
        QString marStr = "";

        badDots_ = 0;
        marginalDots_ = 0;

        QList<QLabel *> labels = this->findChildren<QLabel*>();

        foreach ( QLabel *label, labels )
        {
            if( label->objectName().contains("lblDot"))
            {
                QString idstr = label->objectName();
                int idNum = idstr.mid(6).toInt();

                if( statusListStr.at(idNum) == '2'  && mainWindow_->isServiceMode() )
                {
                    // if the dot is MARGINAL, use this and flip yellow.
                    //label->setStyleSheet("color:rgb(255,20,147); background-color: #E0E0E0;");  // pink
                    //label->setStyleSheet("color:rgb(0,0,0); background-color: #CCCC00;");  // yellow background black text
                    label->setStyleSheet("color:rgb(0,0,0); background-color: #fff67c;");  // (French's Mustard) yellow background black text
                    label->setText("X");
                    marStr.append(QString::number(idNum));
                    marStr.append(", ");
                    marginalDots_++;
                }
                else if( statusListStr.at(idNum) == '1' )
                {
                    // If the dot is BAD, use this and flip red
                    //label->setStyleSheet("color:rgb(255, 0, 51); background-color: #FFFFFF;");  // red
                    label->setStyleSheet("color:rgb(0, 0, 0); background-color: #FF0033;");  // red background black text
                    label->setText("X");
                    dotStr.append(QString::number(idNum));
                    dotStr.append(", ");
                    badDots_++;
                }
            }
        }

        marStr.chop(2);
        dotStr.chop(2);

        ui->lblBadDots->setText(  tr("Bad\ndots: ")  + QString::number(badDots_) );
        ui->lblMargDots->setText( tr("Marginal\ndots: ") + QString::number(marginalDots_) );

        ui->lblBadDotsResult->setPlainText(dotStr);
        ui->lblMarginalDotsResult->setPlainText(marStr);

        ui->lblPercentDotsBad->setText( QString::number((int)(((float)badDots_ / 448.0) * 100)) +"% " + tr("Bad dots") );
    }
}

void servicePrinterHeadDotStatus::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    // make sure weight bar is hidden
    if ( operateMode )
        operateMode->hideWeightBar();
}

void servicePrinterHeadDotStatus::onDotWearTimerTimeout(){
    MessageBoxDialog dialog( mainWindow_ );

    dialog.setAllowOverlayClickDismiss( false );

    ui->lblPercentDotsBad->setText( tr( "Failed...") );

    dialog.setText( tr( "Printer Dot Wear failed to complete" ) );
    dialog.setStandardButtons( QDialogButtonBox::Ok );

    dialog.exec();
    dotWearTestFinished();

}

void servicePrinterHeadDotStatus::startDotWearTest(){
    session_->send(Printer, PrReqHeadDotStatus);   // Request the New Head Dot Status
    ui->lblPercentDotsBad->setText( tr( "Running...") );
    runningTest = true;
    ui->butEnableDotWear->setEnabled(false);
    if(dotWearTimer){
        dotWearTimer->start();
    }
}

void servicePrinterHeadDotStatus::dotWearTestFinished(){
    runningTest = false;
    ui->butEnableDotWear->setEnabled(true);
    if(dotWearTimer != NULL && dotWearTimer->isActive()){
        dotWearTimer->stop();
    }
}


// label->setStyleSheet("color:rgb(255,20,147); border: 1px solid rgb(255,20,147); border-radius: 7px;");  // pink
// label->setStyleSheet("color:rgb(255, 0, 51); border: 2px solid rgb(255, 0, 51); border-radius: 8px;");  // red
// label->setStyleSheet("background: white; border: 1px solid rgb(0, 255, 0); border-radius: 8px;");  // green
// label->setStyleSheet("color:rgb(186, 74, 0); border: 1px solid rgb(255, 183, 77); border-radius: 4px;");  // orange - best canidate
// label->setStyleSheet("color:rgb(153, 0, 0); border: 1px solid rgb(255, 0, 51); border-radius: 3px;");  // kinda red
// label->setStyleSheet("border: 3px solid rgb(240, 98, 146); border-radius: 8px;");  // pink - eh
// label->setStyleSheet("border: 3px solid rgb(253, 216, 53); border-radius: 8px;");  // yellow-orange - eh
// label->setStyleSheet("background: white; border: 3px solid rgb(0, 0, 255); border-radius: 8px;");  // blue
// label->setStyleSheet("background: white; border: 1px solid rgb(0, 255, 127); border-radius: 8px;"); // green
// label->setStyleSheet("background: white; border: 3px solid rgb(232, 32, 32); border-radius: 8px;");  // red
// label->setStyleSheet("background: white; border: 2px solid rgb(191, 183, 15); border-radius: 8px;");  // yellow
