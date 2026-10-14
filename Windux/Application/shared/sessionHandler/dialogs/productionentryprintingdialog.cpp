/**
 * @file productionentryprintingdialog.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "productionentryprintingdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductionEntryPrintingDialog::ProductionEntryPrintingDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "ProductionEntryPrintingDialog" );

    // init
    initialize();
    createLayout();

    // update
    updateWeight();

    // request weight
    session_->send( Weigher, WgReqStatus );

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    //allowDismiss_ = false;
    //setModal(true);
    //show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ProductionEntryPrintingDialog::~ProductionEntryPrintingDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductionEntryPrintingDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // weight
    if (( Weigher == device ) && ( WgStatus == event ))
        updateWeight();

    // weight
    if (( OperationMode == device ) && ( OpModeLabelsPrintedCounts == event ))
    {
        DomOpModeLabelsPrintedCounts labelCounts( msg );
        int current = labelCounts.currentPrintedCount();
        int total = labelCounts.totalLabelsCount();

        // We'll set the current labels pinted count to greater than the total to indicate we are finished and delete this dialog
        if (current > total)
            accept();
        else
        {
            printProgress_->setValue(current);
            printProgress_->setMaximum(total);
            if (current == total)
                // Sleep for a second to let the user see the completion, since the message to clear the dialog comes right after receiving the last progress bar update.
                sleepFor( 1000 );
        }
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductionEntryPrintingDialog::onStopPrintingClicked()
{
    // clear total tare
    totalTare_.clear();

    // update
    updateWeight();
	
    if ( session_ )
        session_->send( ProductDev, ProductStopPrintAllLabels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductionEntryPrintingDialog::initialize()
{
    modifyFontPointSize setFontPointSize36( (mainWindow_->height() * 36) / SessionMainWindow::scalingDividerY_ );
    modifyPaletteForeground setForegroundRed( modifyPaletteForeground::createPalette( hobart::colorPalette::red ) );

    // create widgets
    warningLabel_ = new QLabel( tr( "Weight data is for informational purposes only and not valid for sale." ), this );
    warningLabel_->setAlignment( Qt::AlignCenter );
    warningLabel_->setObjectName( "warningLabel" );
    setForegroundRed( warningLabel_ );

    currentWeightLabel_ = new QLabel( tr( "Current Weight:" ), this );
    currentWeightLabel_->setObjectName( "currentWeightLabel" );
    currentWeightLabel_->setAlignment( Qt::AlignRight );
    setFontPointSize36( currentWeightLabel_ );

    currentWeight_ = new QLabel( this );
    currentWeight_->setObjectName( "currentWeight" );
    currentWeight_->setMinimumWidth(200);
    currentWeight_->setMaximumWidth(200);
    setFontPointSize36( currentWeight_ );

    printProgressLabel_ = new QLabel( tr( "Label printing progress" ), this );
    printProgressLabel_->setAlignment( Qt::AlignCenter );

    printProgress_ = new QProgressBar( this );
    printProgress_->setFormat( "%v of %m" );
    printProgress_->setMinimumSize(QSize(400, 50));

    stopPrinting_ = new QPushButton( tr( "Cancel Printing" ), this );
    stopPrinting_->setObjectName( "newPortion" );
    connect( stopPrinting_, SIGNAL(clicked()), SLOT(onStopPrintingClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductionEntryPrintingDialog::createLayout()
{
    if ( layout() )
        delete layout();

    // create layouts
    QHBoxLayout *weightLayout = new QHBoxLayout();
    weightLayout->setSpacing(40);
    weightLayout->addWidget(currentWeightLabel_);
    weightLayout->addWidget(currentWeight_);

    QHBoxLayout *weightRowLayout = new QHBoxLayout();
    weightRowLayout->addSpacing( 100 );
    weightRowLayout->addLayout(weightLayout);
    weightRowLayout->addSpacing( 100 );

    QHBoxLayout *buttonsLayout( new QHBoxLayout() );
    buttonsLayout->addStretch();
    buttonsLayout->addWidget( stopPrinting_ );
    buttonsLayout->addStretch();

    QVBoxLayout *progressLayout( new QVBoxLayout() );
    //progressLayout->addStretch();
    progressLayout->addWidget( printProgressLabel_ );
    progressLayout->addSpacing( 5 );
    progressLayout->addWidget( printProgress_ );
    progressLayout->addStretch();

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addStretch();
    screenLayout->addWidget( warningLabel_ );
    screenLayout->addSpacing( 15 );
    screenLayout->addLayout( weightRowLayout );
    screenLayout->addStretch();
    screenLayout->addLayout( progressLayout );
    screenLayout->addStretch();
    //screenLayout->addSpacing( 16 );
    screenLayout->addLayout( buttonsLayout );
    screenLayout->addSpacing( 10 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ProductionEntryPrintingDialog::updateWeight()
{
    MadpWeightData *weightData( &session_->scaleConfig->weightData );

    QString weightStr( weightData->netWeight() );

    if ( weightStr.isEmpty() )
        // empty portion
        currentWeight_->clear();
    else if ( weightStr.indexOf("--") >= 0)
        // check for under zero
        currentWeight_->setText( weightStr );
    else
    {
        // set portion weight
        int weightInt( weightStrToInt( weightStr ) );

        currentWeight_->setText( weightIntToStr( weightInt ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ProductionEntryPrintingDialog::weightStrToInt( const QString &weight )
{
    QStringList w( weight.split( "." ) );

    int value( w[0].toInt() );
    value *= 1000;

    if ( 2 == w.count() )
    {
        if ( 3 == w[1].length() )
            value += w[1].toInt();
        else if ( 2 == w[1].length() )
            value += (w[1].toInt() * 10);
        else if ( 1 == w[1].length() )
            value += (w[1].toInt() * 100);
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ProductionEntryPrintingDialog::weightIntToStr( int weight )
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    MadpCurrencySettings *currencySettings( &session_->scaleConfig->currencySettings );

    MadpWeightData *weightData( &session_->scaleConfig->weightData );

    // compute number of decimals
    int numDecimals( 0 );

    QStringList w( weightData->netWeight().split( "." ) );

    if ( 2 == w.count() )
        numDecimals = w[1].length();

    // round weight up
    if ( 2 == numDecimals )
    {
        if ( 5 <= weight % 10 )
            weight += 10;
    }
    else if ( 1 == numDecimals )
    {
        if ( 50 <= weight % 100 )
            weight += 100;
    }
    else if ( 0 == numDecimals )
    {
        if ( 500 <= weight % 1000 )
            weight += 1000;
    }

    // format string
    QString value;

    if ( weight < 0 )
    {
        value.append( "-" );
        weight *= -1;
    }

    value.append( QString::number( weight / 1000 ) );

    if ( numDecimals )
    {
        value.append( currencySettings->unitSeparator() );

        weight %= 1000;

        if ( 2 == numDecimals )
            weight /= 10;
        else if ( 1 == numDecimals )
            weight /= 100;

        value.append( QString( "%0" ).arg( weight, numDecimals, 10, QLatin1Char( '0' ) ) );
    }

    value.append( " " );
    value.append( scaleConfig->isScaleMetric() ? "kg" : "lb" );

    return value;
}

void ProductionEntryPrintingDialog::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

