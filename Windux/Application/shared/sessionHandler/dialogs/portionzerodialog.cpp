/**
 * @file portionzerodialog.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "portionzerodialog.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////
PortionZeroDialog::PortionZeroDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "PortionZeroDialog" );

    // init
    initialize();
    createLayout();

    // update
    updateWeight();

    // request weight
    session_->send( Weigher, WgReqStatus );

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PortionZeroDialog::~PortionZeroDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PortionZeroDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // weight
    if (( Weigher == device ) && ( WgStatus == event ))
        updateWeight();

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void PortionZeroDialog::onNewPortionClicked()
{
    // clear total tare
    totalTare_.clear();

    // update
    updateWeight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PortionZeroDialog::initialize()
{
    modifyFontPointSize setFontPointSize36( (mainWindow_->height() * 36) / SessionMainWindow::scalingDividerY_ );
    modifyPaletteForeground setForegroundRed( modifyPaletteForeground::createPalette( hobart::colorPalette::red ) );

    // create widgets
    warningLabel_ = new QLabel( tr( "Weight data is for informational purposes only and not valid for sale." ), this );
    warningLabel_->setAlignment( Qt::AlignCenter );
    warningLabel_->setObjectName( "warningLabel" );
    setForegroundRed( warningLabel_ );

    portionWeightLabel_ = new QLabel( tr( "Portion Weight:" ), this );
    portionWeightLabel_->setObjectName( "portionWeightLabel" );
    setFontPointSize36( portionWeightLabel_ );

    portionWeight_ = new QLabel( this );
    portionWeight_->setObjectName( "portionWeight" );
    setFontPointSize36( portionWeight_ );

    totalTareWeightLabel_ = new QLabel( tr( "Total Tare Weight:" ), this );
    totalTareWeightLabel_->setObjectName( "totalTareWeightLabel" );
    setFontPointSize36( totalTareWeightLabel_ );

    totalTareWeight_ = new QLabel( this );
    totalTareWeight_->setObjectName( "totalTareWeight" );
    setFontPointSize36( totalTareWeight_ );

    newPortion_ = new QPushButton( tr( "New Portion" ), this );
    newPortion_->setObjectName( "newPortion" );
    connect( newPortion_, SIGNAL(clicked()), SLOT(onNewPortionClicked()) );

    done_ = new QPushButton( tr( "Done" ), this );
    done_->setObjectName( "done" );
    connect( done_, SIGNAL(clicked()), SLOT(accept()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PortionZeroDialog::createLayout()
{
    if ( layout() )
        delete layout();

    // create layouts
    QFormLayout *topInnerLayout( new QFormLayout() );
    topInnerLayout->setHorizontalSpacing( 32 );
    topInnerLayout->setLabelAlignment( Qt::AlignRight );
    topInnerLayout->addRow( portionWeightLabel_, portionWeight_ );
    topInnerLayout->addItem( new QSpacerItem( 16, 16, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    topInnerLayout->addRow( totalTareWeightLabel_, totalTareWeight_ );

    QHBoxLayout *topLayout( new QHBoxLayout() );
    topLayout->addStretch();
    topLayout->addLayout( topInnerLayout );
    topLayout->addStretch();

    QHBoxLayout *buttonsLayout( new QHBoxLayout() );
    buttonsLayout->addStretch();
    buttonsLayout->addWidget( newPortion_ );
    buttonsLayout->addStretch();
    buttonsLayout->addWidget( done_ );
    buttonsLayout->addStretch();

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addStretch();
    screenLayout->addLayout( topLayout );
    screenLayout->addStretch();
    screenLayout->addWidget( warningLabel_ );
    screenLayout->addSpacing( 16 );
    screenLayout->addLayout( buttonsLayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PortionZeroDialog::updateWeight()
{
    MadpWeightData *weightData( &session_->scaleConfig->weightData );

    QString weightStr( weightData->netWeight() );

    // check for under zero
    if ( "----" == weightStr )
        weightStr.clear();

    // empty portion
    if ( weightStr.isEmpty() )
    {
        portionWeight_->clear();
        return;
    }

    // set total tare
    if ( totalTare_.isEmpty() )
        totalTare_ = weightStr;

    int tareWeightInt( weightStrToInt( totalTare_ ) );

    totalTareWeight_->setText( weightIntToStr( tareWeightInt ) );

    // set portion weight
    int weightInt( weightStrToInt( weightStr ) );

    if ( weightInt < tareWeightInt )
        portionWeight_->clear();
    else
    {
        portionWeight_->setText( weightIntToStr( weightInt - tareWeightInt ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PortionZeroDialog::weightStrToInt( const QString &weight )
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
QString PortionZeroDialog::weightIntToStr( int weight )
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
