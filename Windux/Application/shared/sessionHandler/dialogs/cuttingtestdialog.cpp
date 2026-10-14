/**
 * @file cuttingtestdialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <softkeyswidget.h>
#include "cuttingtestdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <numericlineedit.h>
#include <palette.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
CuttingTestDialog::CuttingTestDialog( int num, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    num_( num )
{
    setObjectName( "CuttingTestDialog" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CuttingTestDialog::~CuttingTestDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize CuttingTestDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 600) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 400) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *CuttingTestDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CuttingTestDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDialog::showEvent( QShowEvent *event )
{
    _Mybase::showEvent( event );

    // operator starts in product number field
    productNumber_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestDialog::eventFilter( QObject *obj, QEvent *event )
{
    if ( QEvent::KeyRelease == event->type() )
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>( event );

        // check for enter key
        if ( keyEvent )
            if (( Qt::Key_Return == keyEvent->key() ) || (  Qt::Key_Enter == keyEvent->key() ))
            {
                QWidgetList widgets;
                widgets.append( productNumber_ );
                widgets.append( vendorNumber_ );
                widgets.append( operatorNumber_ );
                widgets.append( productCost_ );
                widgets.append( productWeight_ );

                foreach ( QWidget *w, widgets )
                    if ( w == obj )
                    {
                        // lets find something that needs filled out...
                        timer_->setSingleShot( true );
                        timer_->start( 0 );

                        break;
                    }
            }
    }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDialog::onStartButtonClicked()
{
    // request and refresh the cut test flashkeys
    session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::RunScreenCutTestOn ) );

    // start test
    session_->send( CutTestDev, BeginCuttingTest,
        QString::number( num_ ) + "," +
        operatorNumber_->text() + "," +
        vendorNumber_->text() + "," +
        productNumber_->text() + "," +
        productWeight_->toSessionField() + "," +
        productCost_->toSessionField() );

    // done!
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDialog::onTimerTimeout()
{
    QList<hobart::numericLineEdit*> widgets;
    widgets.append( productNumber_ );
    widgets.append( vendorNumber_ );
    widgets.append( operatorNumber_ );
    widgets.append( productCost_ );
    widgets.append( productWeight_ );

    // check for empty fields
    foreach ( hobart::numericLineEdit *w, widgets )
        if ( w->text().isEmpty() )
        {
            w->setFocus();
            return;
        }

    // no empty fields!
    // allow them to start test
    startButton_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDialog::initialize()
{
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

    modifyFontPointSize setFontPointSize14( (mainWindow_->height() * 14) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize14( this );

    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimerTimeout() ) );

    // ---- //

    // test number
    testNumber_ = new QLabel( tr( "Cutting Test Number" ) + " " + QString::number( num_ ), this );
    testNumber_->setAlignment( Qt::AlignCenter );
    testNumber_->setObjectName( "testNumber" );

    setFontPointSize18( testNumber_ );

    // product number
    productNumberLabel_ = new QLabel( tr( "Product number" ), this );
    productNumberLabel_->setObjectName( "productNumberLabel" );

    productNumber_ = new hobart::numericLineEdit( this );
    productNumber_->setInputTruncation( true );
    productNumber_->setObjectName( "productNumber" );
    productNumber_->setValidator( new QIntValidator( 0, 999999, this ) );

    productNumber_->installEventFilter( this );

    // vendor number
    vendorNumberLabel_ = new QLabel( tr( "Vendor number" ), this );
    vendorNumberLabel_->setObjectName( "vendorNumberLabel" );

    vendorNumber_ = new hobart::numericLineEdit( this );
    vendorNumber_->setInputTruncation( true );
    vendorNumber_->setObjectName( "vendorNumber" );
    vendorNumber_->setValidator( new QIntValidator( 0, 9999999, this ) );	//	HA-55

    vendorNumber_->installEventFilter( this );

    // operator number
    operatorNumberLabel_ = new QLabel( tr( "Operator number" ), this );
    operatorNumberLabel_->setObjectName( "operatorNumberLabel" );

    operatorNumber_ = new hobart::numericLineEdit( this );
    operatorNumber_->setInputTruncation( true );
    operatorNumber_->setObjectName( "operatorNumber" );
    operatorNumber_->setValidator( new QIntValidator( 0, 9999, this ) );

    operatorNumber_->installEventFilter( this );

    // product cost
    productCostLabel_ = new QLabel( tr( "Product cost" ), this );
    productCostLabel_->setObjectName( "productCostLabel" );

    productCost_ = new hobart::numericLineEdit( this );
    productCost_->setDecimal( currencySettings->unitSeparator() );
    productCost_->setDecimalPlaces( currencySettings->primaryPrecision() );
    productCost_->setInputTruncation( true );
    productCost_->setObjectName( "productCost" );
    productCost_->setPrefix( currencySettings->priCurrSymbol() );
    productCost_->setValidator( new QIntValidator( 0, 999999, this ) );

    productCost_->installEventFilter( this );

    // product weight
    productWeightLabel_ = new QLabel( tr( "Product weight" ), this );
    productWeightLabel_->setObjectName( "productWeightLabel" );

    productWeight_ = new hobart::numericLineEdit( this );
    productWeight_->setDecimal( currencySettings->unitSeparator() );
    productWeight_->setDecimalPlaces( scaleConfig->isScaleMetric() ? 3 : 2 );
    productWeight_->setInputTruncation( true );
    productWeight_->setObjectName( "productWeight" );
    productWeight_->setSuffix( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    productWeight_->setValidator( new QIntValidator( 0, 999999, this ) );	//  HTA-105:  999.999 lb or 999.999 kg

    productWeight_->installEventFilter( this );

    // start
    startButton_ = new QPushButton( tr( "Start Test" ), this );
    startButton_->setEnabled( false );
    startButton_->setObjectName( "startButton" );

    connect( startButton_, SIGNAL( clicked() ), SLOT( onStartButtonClicked() ) );

    // ---- //

    QWidgetList widgets;
    widgets.append( productNumber_ );
    widgets.append( vendorNumber_ );
    widgets.append( operatorNumber_ );
    widgets.append( productCost_ );
    widgets.append( productWeight_ );
    widgets.append( startButton_ );

    // modify foreground colors
    foreach ( QWidget *w, widgets )
    {
        QPalette palette( w->palette() );
        palette.setColor( QPalette::Active, w->foregroundRole(), hobart::colorPalette::black );
        palette.setColor( QPalette::Disabled, w->foregroundRole(), hobart::colorPalette::lightGray );

        w->setPalette( palette );
    }

    // SOF-3837: Set Label colors
    QWidgetList labelWidgets;
    labelWidgets.append( testNumber_ );
    labelWidgets.append( productNumberLabel_ );
    labelWidgets.append( vendorNumberLabel_ );
    labelWidgets.append( operatorNumberLabel_ );
    labelWidgets.append( productCostLabel_ );
    labelWidgets.append( productWeightLabel_ );

    foreach ( QWidget *w, labelWidgets )
    {
        QPalette palette( w->palette() );
        palette.setColor( QPalette::WindowText, hobart::colorPalette::white );

        w->setPalette( palette );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDialog::createLayout()
{
    QFormLayout *fields( new QFormLayout() );
    fields->addRow( productNumberLabel_, productNumber_ );
    fields->addRow( vendorNumberLabel_, vendorNumber_ );
    fields->addRow( operatorNumberLabel_, operatorNumber_ );
    fields->addRow( productCostLabel_, productCost_ );
    fields->addRow( productWeightLabel_, productWeight_ );

    QHBoxLayout *bottom( new QHBoxLayout() );
    bottom->addStretch();
    bottom->addWidget( startButton_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addWidget( testNumber_ );
    screenLayout->addStretch();
    screenLayout->addLayout( fields );
    screenLayout->addStretch();
    screenLayout->addLayout( bottom );
}
