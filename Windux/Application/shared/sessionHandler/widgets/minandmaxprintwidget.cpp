/**
 * @file minandmaxprintwidget.cpp
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe B. Crew
 */

#include "sessioninfo.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../modes/operatemode.h"

#include <logmanager.h>
#include <filesystem.h>
#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>
#include <QDesktopWidget>

#include "minandmaxprintwidget.h"
#include "ui_minandmaxprintwidget.h"

// Hobart namespace
namespace hobart {

MinAndMaxPrintWidget::MinAndMaxPrintWidget(QWidget *parent, SessionMainWindow *mainWindow) :
    _Mybase(parent),
    ui(new Ui::MinAndMaxPrintWidget),
    session_( NULL ),
    config_( NULL ),
    operateMode_( NULL ),
    scaleConfigData( NULL ),
    currentPLU_( NULL ),
    valid_( true ),
    stablePrice_( false ),
    stableWeight_( false ),
    code_( "0" ),
    minPr_( 0.00 ),
    maxPr_( 0.00 ),
    minWt_( 0.00 ),
    maxWt_( 0.00 ),
    showPrice_( true ),
    showWeight_( true ),
    range_weight_(""),
    range_price_(""),
    minWeight_( "0" ),
    maxWeight_( "0" ),
    minPrice_( "0" ),
    maxPrice_( "0" ),
    use10Inch_( false )
{
    LOG_CLASS_INFO( "MinAndMaxPrintWidget", "MinAndMaxPrintWidget" );

    ui->setupUi(this);

    mainWindow_     = mainWindow;
    session_        = mainWindow_->session();
    config_         = session_->scaleConfig;
    scaleConfigData = &config_->scaleConfig;

    if ( session_ )
    {
        // connect signals and hook into session
        connect( session_,          SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceiveInternal(SessionMessage) ) );
        connect(ui->butOK,          SIGNAL( clicked() ),      this,    SLOT( onOKButtonClicked() ) );
        connect(ui->butCancel,      SIGNAL( clicked() ),      this,    SLOT( onCancelButtonClicked() ) );
    }
    else
    {
        LOG_WARN( "parent is not main window", NULL );
        valid_ = false;
    }

    initialize();

    translate();

    if( use10Inch_ )
    {
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the Min-Max Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the Min-Max Range") );
    }
    else
    {
        // assuming 14 inch here. May have to revisit if we go all 21 inch Zenith
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the minimum and maximum Weight Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the minimum and maximum Price Range") );
    }

    // while this widget is open do not show system dialogs
    //session_->send( SessionDev, SnStopDialogs );
}

MinAndMaxPrintWidget::~MinAndMaxPrintWidget()
{
    // resume dialogs
    session_->send( Error, ErrRemoveAnyModalDialogDevice, code_ + ",5,1" );
    //session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::translate()
{

    if( use10Inch_ )
    {
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the Min-Max Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the Min-Max Range") );
    }
    else
    {
        // assuming 14 inch here. May have to revisit if we go all 21 inch Zenith
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the minimum and maximum Weight Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the minimum and maximum Price Range") );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::initialize()
{
    LOG_CLASS_INFO( "MinAndMaxPrintWidget", "initialize" );

    // check desktop dimensions
    int mywidth = 1024;     // 14  is 1366
    int myheight = 600;     // 14 is 768

    use10Inch_ = true;

    QDesktopWidget *desktop( QApplication::desktop() );
    if ( desktop )
    {
        const QRect desktopRect( desktop->screenGeometry( 0 ) );
        if ( ( desktopRect.width() > mywidth ) || ( desktopRect.height() > myheight  ) )
            use10Inch_ = false;      // use 14 inch instead

        LOG_INFO( "primary screen size is (%d, %d)", desktopRect.width(), desktopRect.height() );
    }

    if( use10Inch_ )
    {
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the Min-Max Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the Min-Max Range") );
        this->resize(591, 391);
    }
    else
    {
        // assuming 14 inch here. May have to revisit if we go all 21 inch Zenith
        ui->lblWeightNotes->setText( tr("The Current Weight needs to fit within the minimum and maximum Weight Range") );
        ui->lblPriceNotes->setText( tr("The Current Price needs to fit within the minimum and maximum Price Range") );
        this->resize(761, 421);
    }

    if ( !operateMode_ )
    {
        sessionInfo info( this );
        SessionMainWindow *mainWindow( info() );

        operateMode_ = mainWindow->retrieveMode<OperateMode>();
    } 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::displayOutOfRangeData(QString errorCode, QString minWeight, QString maxWeight, QString minPrice, QString maxPrice)
{
    resetValues();

    MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

    const QString weight( productTotals->totalNetWeight() );
    const QString price( productTotals->totalPrice() );
    const QString unit( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    const QString currSymbol( currencySettings->priCurrSymbol() );

    code_           = errorCode;
    minWeight_      = minWeight;                // current PLU limits
    maxWeight_      = maxWeight;                // current PLU limits
    minPrice_       = minPrice;                 // current PLU limits
    maxPrice_       = maxPrice;                 // current PLU limits
    minWt_          = minWeight.toDouble();     // current PLU limits
    maxWt_          = maxWeight.toDouble();     // current PLU limits
    minPr_          = minPrice.toDouble();      // current PLU limits
    maxPr_          = maxPrice.toDouble();      // current PLU limits

    // retrieve new totals
    session_->send( ProductDev, ProductReadCurrentTotalValues );

    if( ( "0.000" == minWeight_ && "0.000" == maxWeight_ ) || ( "0.00" == minWeight_ && "0.00" == maxWeight_ ) )
        showWeight_ = false;

    if( ( "0.00" == minPrice_ && "0.00" == maxPrice_ ) )
        showPrice_ = false;

    if( (showPrice_ && !showWeight_) || ( !showPrice_ && showWeight_ ) )
    {
        int yfactor = 351;
        ui->lblTitle->setMinimumHeight(81);
        if( use10Inch_ )
            this->resize(591, yfactor);
        else
            this->resize(761, yfactor);
    }
    else if( showPrice_ && showWeight_ )
    {
        ui->lblTitle->setMinimumHeight(81);
        if( use10Inch_ )
            this->resize(591, 391);
        else
            this->resize(761, 421);
    }

    enablePriceFields( showPrice_ );
    enableWeightFields( showWeight_ );

    range_weight_ = minWeight_ + unit + "- " + maxWeight_ + unit;
    range_price_ = currSymbol + minPrice_ + " - " + currSymbol + maxPrice_;

    ui->lblMinMaxWeight->setText( range_weight_ );
    ui->lblCurrentWeight->setText( weight + unit );

    ui->lblMinMaxPrice->setText( range_price_ );
    ui->lblCurrentPrice->setText( currSymbol + price );

    // even with this, screen not updating. Is it because we are not visible yet?
    ui->lblMinMaxWeight->repaint();
    ui->lblMinMaxPrice->repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::printOnScreenPrintKey()
{
    onOKButtonClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::onSessionMessageReceiveInternal( const SessionMessage& msg )
{
    onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::onOKButtonClicked()
{
    // resume dialogs
    session_->send( Error, ErrRemoveAnyModalDialogDevice, code_ + ",5,1" );
    session_->send( SessionDev, SnRestartDialogs );
    session_->send( SessionDev, SnPrintKeyPressed );

    //close();
    hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::onCancelButtonClicked()
{
    // resume dialogs
    session_->send( Error, ErrRemoveAnyModalDialogDevice, code_ + ",5,0" );
    session_->send( SessionDev, SnRestartDialogs );
    session_->send( ProductDev, ProductReadCurrentTotalValues );    // retrieve new totals and update screen

    //close();
    hide();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *MinAndMaxPrintWidget::currentProduct() const
{
    DomCurrentPLU *product = (operateMode_ ? operateMode_->currentProduct() : NULL);

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MadpUpdateCurrentPLUTotals *MinAndMaxPrintWidget::currentProductTotals() const
{
    if ( !currentProduct() )
        return NULL;

    return (operateMode_ ? operateMode_->currentProductTotals() : NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::enablePriceFields( bool isEnabled )
{
    ui->lblMinMaxPriceTitle->setVisible( isEnabled );
    ui->lblCurrentPriceTitle->setVisible( isEnabled );
    ui->lblMinMaxPrice->setVisible( isEnabled );
    ui->lblCurrentPrice->setVisible( isEnabled );
    ui->lblPriceNotes->setVisible( isEnabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::enableWeightFields( bool isEnabled )
{
    ui->lblMinMaxWeightTitle->setVisible( isEnabled );
    ui->lblCurrentWeightTitle->setVisible( isEnabled );
    ui->lblMinMaxWeight->setVisible( isEnabled );
    ui->lblCurrentWeight->setVisible( isEnabled );
    ui->lblWeightNotes->setVisible( isEnabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::resetValues()
{
    code_         = "0";
    minPr_        = 0.00;
    maxPr_        = 0.00;
    minWt_        = 0.00;
    maxWt_        = 0.00;
    showPrice_    = true;
    showWeight_   = true;
    minWeight_    = "0";
    maxWeight_    = "0";
    minPrice_     = "0";
    maxPrice_     = "0";
    range_price_  = "";
    range_weight_ = "";
}

void MinAndMaxPrintWidget::resizeEvent( QResizeEvent *e )
{
    LOG_CLASS_INFO( "MinAndMaxPrintWidget", "resizeEvent" );

    _Mybase::resizeEvent( e );

    LOG_DEBUG( "height %d width %d", height(), width() );

    ui->lblMinMaxWeight->setText( range_weight_ );
    ui->lblMinMaxPrice->setText( range_price_ );

    setAutoFillBackground( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::highlightGreen(QLabel* label)
{
    //label->setStyleSheet("background-color: rgb(32, 178, 170); color: black;");
    label->setStyleSheet("background-color: rgb(0, 255, 127); color: black;");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::highlightRed(QLabel* label)
{
    label->setStyleSheet("background-color: rgb(255, 10, 63); color: black;");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::highlightCamo(QLabel* label)
{
    label->setStyleSheet("background-color: rgb(63,100,127);");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::highlightWhite(QLabel* label)
{
    label->setStyleSheet( "background-color: white;" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MinAndMaxPrintWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "MinAndMaxPrintWidget", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if( !this->isVisible() )
        return;

    if ((ProductDev == device ) && ( ProductWriteCurrentTotalValues == event ))
    {
        MadpUpdateCurrentPLUTotals *productTotals( currentProductTotals() );

        if( !productTotals )
            return;

        ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
        MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
        MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

        const QString unit( scaleConfig->isScaleMetric() ? " kg" : " lb" );
        const QString currSymbol( currencySettings->priCurrSymbol() );

        const QString weight( productTotals->totalNetWeight() );
        const QString price( productTotals->totalPrice() );
        double actualPr = price.toDouble();
        double actualWt = weight.toDouble();

        // itemNumber, tare, propTare, fsTotalPrice(), fsSavings(), fsUnitPrice()

        if( 0 == weight.length() && 0 == price.length() )
            return; // bc this is just adding or removing weight, and I don't want to blink on it.

        // update the onscreen values
        if( showPrice_ )
        {
            enablePriceFields( true );

            ui->lblMinMaxPrice->setText( range_price_ );
            ui->lblCurrentPrice->setText( currSymbol + price );

            // now update the field if it is in range or not
            if( minPrice_ == "" && maxPrice_ == "" && 0 == price.length() )
            {
                LOG_DEBUG( "we have no price or maxs or mins", NULL);
            }
            else
            {
                if( ( minPr_ >= 0 && maxPr_ > 0 ) && ( minPr_ <= actualPr && maxPr_ >= actualPr) )
                    highlightGreen( ui->lblCurrentPrice );
                else
                    highlightRed( ui->lblCurrentPrice );
            }
        }
        else
        {
            enablePriceFields( false );
        }

        if( showWeight_ )
        {
            enableWeightFields( true );

            ui->lblMinMaxWeight->setText( range_weight_ );
            ui->lblCurrentWeight->setText( weight + unit );

            // now update the field if it is in range or not
            if( minWeight_ == "" && maxWeight_ == "" && 0 == weight.length() )
            {
                LOG_DEBUG( "we have no weights or maxs or mins", NULL);
            }
            else
            {
                if( ( minWt_ >= 0 && maxWt_ > 0 ) && ( minWt_ <= actualWt && actualWt <= maxWt_ ) )
                    highlightGreen( ui->lblCurrentWeight );
                else
                    highlightRed( ui->lblCurrentWeight );
            }
        }
        else
        {
            enableWeightFields( false );
        }
    }
}

} // hobart namespace
