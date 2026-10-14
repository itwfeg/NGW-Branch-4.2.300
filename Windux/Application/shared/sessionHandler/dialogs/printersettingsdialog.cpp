/**
 * @file printersettingsdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "printersettingsdialog.h"
#include "ScaleClasses.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

static const int NoPrinter    = 0;          // Not sure if this is even possible, unless we have major issue and cant see it
static const int EPPPrinter   = 1;          // Standalone EPP Printer, but we are running out of parts for it, moving to EPP2.
static const int ApcPrinter   = 4;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int EPP2Printer  = 5;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int KpcPrinter	  = 9;			// New service scale printer for HT scales on ARK system controller board.
static const int SSKPrinter   = 12;         // New free standing self service scale printer
static const QString SelfService    = "1";  // Newest Atmel EPP Printer which is becoming the newer standard.
static const QString AccessWrapper  = "2";  // Access Wrapper
static const QString CounterScale   = "3";  // Newest Atmel EPP Printer which is becoming the newer standard.

const bool PrinterSettingsDialog::convertUpperCase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterSettingsDialog::PrinterSettingsDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    dirty_( false ),
    ignoreSignals_( false ),
    activePrinter_( -1 )
{    
    setObjectName( "PrinterSettingsDialog" );

    // reserve up to three printers
    printerConfig_.resize( 3 );
    printerStockMetrics_.resize( 3 );

    // I may not even need this
    connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceive(SessionMessage) ) );

    // initialize
    initialize();
    createLayout();

    // request printer settings
    if ( 0 <= (activePrinter_ = selectedPrinter()) )
        session_->send( Printer, PrReqInquire, QString::number( activePrinter_ ) );
        session_->send( Printer, PrReqCutterStatus);

    session_->send( Printer, PrReqStockMetrics, QString::number( activePrinter_ ) );

    // FT-415
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterSettingsDialog::~PrinterSettingsDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );

    savePrinterConfig();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "PrinterSettingsDialog", "onButtonClicked" );

    if ( applyAndPrint_ == button )
    {
        LOG_INFO( "apply and print button clicked", NULL );

        // save config
        savePrinterConfig();

        // print!
        if (( 0 <= activePrinter_ ) && ( activePrinter_ < 3 ))
            session_->send( Printer, PrTestLabel, QString::number( activePrinter_ ) + ",2,0" ); // Printer, PrTestLabel, printerNum, PrCheckerboard, ?

        return;
    }

    if ( ejectAndCut_ == button )
    {
        LOG_INFO( "eject and cut button clicked", NULL );

         if (( 0 <= activePrinter_ ) && ( activePrinter_ < 3 ))
            session_->send( Printer, PrEjectCut);

        return;
    }

    DomPrinterConfigRead& m = printerConfig_[activePrinter_];
    int pos = -1;

    if ( down_ == button )
    {
        LOG_INFO( "down button clicked", NULL );
        pos = m.vertPrintPosition() - 1;
    }
    else if ( left_ == button )
    {
        LOG_INFO( "left button clicked", NULL );
        pos = m.horizPrintPosition() - 1;
    }
    else if ( right_ == button )
    {
        LOG_INFO( "right button clicked", NULL );
        pos = m.horizPrintPosition() + 1;
    }
    else if ( up_ == button )
    {
        LOG_INFO( "up button clicked", NULL );
        pos = m.vertPrintPosition() + 1;
    }

    if (( 0 <= pos ) && ( pos < 100 ))
    {
        if (( down_ == button ) || ( up_ == button ))
        {
            m.setVertPrintPosition( pos );

            ignoreSignals_ = true;
            verticalPrintPosition_->setText( QString::number( pos ) );
            ignoreSignals_ = false;
        }
        else if (( left_ == button ) || ( right_ == button ))
        {
            m.setHorizPrintPosition( pos );

            ignoreSignals_ = true;
            horizontalPrintPosition_->setText( QString::number( pos ) );
            ignoreSignals_ = false;
        }

        // update preview widget
        preview_->setPrinterConfig( m );

        onFieldChanged();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::onCurrentPrinterChanged( int index )
{
    Q_UNUSED( index );

    // save
    savePrinterConfig();

    // disable everything until we get settings
    disableInterface();

    // request printer settings
    session_->send( Printer, PrReqInquire, QString::number( activePrinter_ = selectedPrinter() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::onFieldChanged()
{
    if ( ignoreSignals_ )
        return;

    dirty_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PrinterSettingsDialog::selectedPrinter() const
{
    if ( 0 == currentPrinter_->count() )
        return -1;

    return currentPrinter_->itemData( currentPrinter_->currentIndex() ).toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterSettingsDialog::isSupportedPrinterType( int type )
{
    return ( ( MadpPeripheralList::ptEPP == type ) ||
             ( MadpPeripheralList::ptAPC == type ) ||
             ( MadpPeripheralList::ptEPP2 == type ) ||
             ( MadpPeripheralList::ptEPP4 == type ) ||
             ( MadpPeripheralList::ptKPC == type ) ||
             ( MadpPeripheralList::ptFSSS == type ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::disableInterface()
{
    contrast_->setEnabled( false );
    labelWidth_->setEnabled( false );
    expelPosition_->setEnabled( false );

    horizontalPrintPosition_->setEnabled( false );
    verticalPrintPosition_->setEnabled( false );

    down_->setEnabled( false );
    left_->setEnabled( false );
    right_->setEnabled( false );
    up_->setEnabled( false );

    applyAndPrint_->setEnabled( false );
    ejectAndCut_->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "PrinterSettingsDialog", "onSessionMessageReceive" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // printer inquire
    if (( Printer == device ) && ( PrInquire == event ))
    {
        DomPrinterConfigRead m( msg );
        int type( m.printerType() );

        if (( 0 <= type ) && ( type < printerConfig_.size() ))
        {
            LOG_DEBUG( "updating config...", NULL );

            printerConfig_[type] = m;

            ignoreSignals_ = true;
            updatePrinterConfig();
            ignoreSignals_ = false;
        }
    }

    // printer stock metrics
    else if (( Printer == device ) && ( PrStockMetrics == event ))
    {
        DomPrinterStockMetrics m( msg );
        int type( m.printerType() );

        if (( 0 <= type ) && ( type < printerStockMetrics_.size() ))
        {
            LOG_DEBUG( "updating stock metrics...", NULL );

            printerStockMetrics_[type] = m;

            ignoreSignals_ = true;
            updatePrinterStockMetrics();
            ignoreSignals_ = false;
        }
    }

    // printer cutter status
    else if (( Printer == device ) && ( PrCutterStatus == event ))
    {
        DomPrinterCutterStatus m( msg );
        LOG_DEBUG( "updating cutter status...", NULL );

        printerCutterStatus_ = m;

        ignoreSignals_ = true;
        updatePrinterCutterStatus();
        ignoreSignals_ = false;
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::initialize()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    static const QSize BUTTON_SIZE( 128, 64 );
    static const QSize LABEL_SIZE( 200, 32 );

    MadpPeripheralList *peripheralList( &session_->scaleConfig->peripheralList );
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    // signal mapper
    buttons_ = new QSignalMapper( this );

    connect( buttons_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // ---- //

    QString comboBoxStyle("font: 20pt;");

    // preview widget
    preview_ = new PrinterSettingsPreviewWidget( this );
    preview_->setFixedWidth( 400 );
    preview_->setObjectName( "preview" );

    // current printer
    currentPrinterLabel_ = new QLabel( tr( "Printer" ), this );
    currentPrinterLabel_->setObjectName( "currentPrinterLabel" );
    currentPrinterLabel_->setFixedSize( LABEL_SIZE );

    currentPrinter_ = new QComboBox( this );
    currentPrinter_->setFocusPolicy( Qt::NoFocus );
    currentPrinter_->setObjectName( "currentPrinter" );
    currentPrinter_->setStyleSheet(comboBoxStyle);

    int printer1 = peripheralList->firstPrinter();
    int printer2 = peripheralList->secondPrinter();
    int printer3 = peripheralList->thirdPrinter();

    // Original: If the first printer is an EPP or EPP2 printer, this is for Access/NGW. For HT, this is APC printer as firstPrinter
    if ( ( PrinterKind::fieldOffsetEppPrinter == printer1 ) || ( PrinterKind::fieldOffsetEpp2Printer == printer1 ) ||
         ( PrinterKind::fieldOffsetSTEPPPrinter == printer1) )
        currentPrinter_->addItem( "Primary", QVariant( 0 ) );
    //else as a standalone APC HT printer, with no EPPs, etc, the printer is not even shown which is why there is no code here

    // NEW: Only add Primary as a printer on HT scale if and only if we have extra EPPs attached in one of the slots
    if ( (   ( ApcPrinter == printer1 )  || ( KpcPrinter  == printer1 ) || ( SSKPrinter == printer1 ) ) &&
         ( ( ( EPPPrinter == printer2 )  || ( EPP2Printer == printer2 ) || ( PrinterKind::fieldOffsetSTEPPPrinter == printer2 ) ) ||
           ( ( EPPPrinter == printer3 )  || ( EPP2Printer == printer3 ) || ( PrinterKind::fieldOffsetSTEPPPrinter == printer3 ) )  ) )
        currentPrinter_->addItem( "Primary", QVariant( 0 ) );

    if( ( EPPPrinter == printer2 ) || ( EPP2Printer == printer2 ) || ( PrinterKind::fieldOffsetSTEPPPrinter == printer2 ) )
    {
        if ( CounterScale == scaleConfig->scaleModel() )
            currentPrinter_->addItem( "EPP", QVariant( 1 ) );           // 2nd Printer: Only add if we have an HT and EPP/EPP2/STEPP as 2nd printer
        else
            currentPrinter_->addItem( "Second", QVariant( 1 ) );        // Original - add this if EPP printer on Access, but for HT we add EPP. Could this be the same as both?
    }

    if ( ( EPPPrinter == printer3 ) || ( EPP2Printer == printer3 ) || ( PrinterKind::fieldOffsetSTEPPPrinter == printer3 ) )
    {
        if ( CounterScale == scaleConfig->scaleModel() )
            currentPrinter_->addItem( "EPP", QVariant( 2 ) );           // 2nd Pritner: Only add if we have an HT and EPP/EPP2 as 2nd printer
        else
            currentPrinter_->addItem( "Third", QVariant( 2 ) );         // Original - add this if EPP printer on Access, but for HT we add EPP. Could this be the same as both?
    }

    // OK - last resort to add a primary printer
    if ( currentPrinter_->count() == 0 && ( ApcPrinter == peripheralList->firstPrinter() || KpcPrinter == peripheralList->firstPrinter() || 
         SSKPrinter == peripheralList->firstPrinter() ))

        currentPrinter_->addItem( "Primary", QVariant( 0 ) );

    if ( currentPrinter_->count() <= 1 )
    {
        currentPrinterLabel_->hide();
        currentPrinter_->hide();
    }
    else if (session_->isPrepack() && (currentPrinter_->count() == 1))
    {
        currentPrinter_->setEnabled(false);
	}

    connect( currentPrinter_, SIGNAL( currentIndexChanged(int) ), SLOT( onCurrentPrinterChanged(int) ) );

    // expel position
    expelPositionLabel_ = new QLabel( tr( "Expel Position" ), this );
    expelPositionLabel_->setObjectName( "expelPositionLabel" );
    expelPositionLabel_->setFixedSize( LABEL_SIZE );

    expelPosition_ = new QLineEdit( this );
    expelPosition_->setEnabled( false );
    expelPosition_->setObjectName( "expelPosition" );
    expelPosition_->setReadOnly( true );
    expelPosition_->setValidator( new QIntValidator( 0, 999, this ) );

    // expel position does not apply for wrapper
    if ( session_->isPrepack() )
    {
        expelPositionLabel_->hide();
        expelPosition_->hide();
    }

    connect( expelPosition_, SIGNAL( textChanged(QString) ), SLOT( onFieldChanged() ) );

    // print contrast
    contrastLabel_ = new QLabel( tr( "Print Contrast" ), this );
    contrastLabel_->setObjectName( "contrastLabel" );
    contrastLabel_->setFixedSize( LABEL_SIZE );

    contrast_ = new QComboBox( this );
    contrast_->setEnabled( false );
    contrast_->setFocusPolicy( Qt::NoFocus );
    contrast_->setObjectName( "contrast" );
    contrast_->setStyleSheet(comboBoxStyle);

    contrast_->addItem( "0", 0 );
    contrast_->addItem( "1", 1 );
    contrast_->addItem( "2", 2 );
    contrast_->addItem( "3", 3 );
    contrast_->addItem( "4", 4 );
    contrast_->addItem( "5", 5 );
    contrast_->addItem( "6", 6 );
    contrast_->addItem( "7", 7 );

    connect( contrast_, SIGNAL( currentIndexChanged(int) ), SLOT( onFieldChanged() ) );

    // label width
    labelWidthLabel_ = new QLabel( tr( "Label Width" ), this );
    labelWidthLabel_->setObjectName( "labelWidthLabel" );
    labelWidthLabel_->setFixedSize( LABEL_SIZE );

    labelWidth_ = new QComboBox( this );
    labelWidth_->setEnabled( false );
    labelWidth_->setFocusPolicy( Qt::NoFocus );
    labelWidth_->setObjectName( "labelWidth" );
    labelWidth_->setStyleSheet(comboBoxStyle);

    QString width_150( (settings->isScaleMetric()) ? "38.10" : "1.50" );
    QString width_225( (settings->isScaleMetric()) ? "57.15" : "2.25" );
    QString width_300( (settings->isScaleMetric()) ? "76.20" : "3.00" );

    session_->uiUtilities->youGottaSetTheSeparator( width_150 );
    session_->uiUtilities->youGottaSetTheSeparator( width_225 );
    session_->uiUtilities->youGottaSetTheSeparator( width_300 );

    labelWidth_->addItem( width_150, QString( "1" ) );
    labelWidth_->addItem( width_225, QString( "0" ) );
    labelWidth_->addItem( width_300, QString( "2" ) );

    connect( labelWidth_, SIGNAL( currentIndexChanged(int) ), SLOT( onFieldChanged() ) );

    // horizontal print position
    horizontalPrintPositionLabel_ = new QLabel( tr( "Horiz. Print Position" ), this );
    horizontalPrintPositionLabel_->setObjectName( "horizontalPrintPositionLabel" );
    horizontalPrintPositionLabel_->setFixedSize( LABEL_SIZE );

    horizontalPrintPosition_ = new QLineEdit( this );
    horizontalPrintPosition_->setEnabled( false );
    horizontalPrintPosition_->setObjectName( "horizontalPrintPosition" );
    horizontalPrintPosition_->setReadOnly( true );

    // vertical print position
    verticalPrintPositionLabel_ = new QLabel( tr( "Vertical Print Position" ), this );
    verticalPrintPositionLabel_->setObjectName( "verticalPrintPositionLabel" );
    verticalPrintPositionLabel_->setFixedSize( LABEL_SIZE );

    verticalPrintPosition_ = new QLineEdit( this );
    verticalPrintPosition_->setEnabled( false );
    verticalPrintPosition_->setObjectName( "verticalPrintPosition" );
    verticalPrintPosition_->setReadOnly( true );

    // up button
    up_ = new QPushButton( tr( "Up" ), this );
    up_->setEnabled( false );
    up_->setFixedSize( BUTTON_SIZE );
    up_->setFocusPolicy( Qt::NoFocus );
    up_->setObjectName( "upButton" );

    connect( up_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( up_, up_ );

    // left button
    left_ = new QPushButton( tr( "Left" ), this );
    left_->setEnabled( false );
    left_->setFixedSize( BUTTON_SIZE );
    left_->setFocusPolicy( Qt::NoFocus );
    left_->setObjectName( "leftButton" );

    connect( left_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( left_, left_ );

    // right button
    right_ = new QPushButton( tr( "Right" ), this );
    right_->setEnabled( false );
    right_->setFixedSize( BUTTON_SIZE );
    right_->setFocusPolicy( Qt::NoFocus );
    right_->setObjectName( "rightButton" );

    connect( right_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( right_, right_ );

    // down button
    down_ = new QPushButton( tr( "Down" ), this );
    down_->setEnabled( false );
    down_->setFixedSize( BUTTON_SIZE );
    down_->setFocusPolicy( Qt::NoFocus );
    down_->setObjectName( "downButton" );

    connect( down_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( down_, down_ );

    // apply and print button
    applyAndPrint_ = new QPushButton( tr( "Apply Changes && Print Label" ), this );
    applyAndPrint_->setEnabled( false );
    applyAndPrint_->setFocusPolicy( Qt::NoFocus );
    applyAndPrint_->setObjectName( "applyAndPrint" );

    
    connect( applyAndPrint_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( applyAndPrint_, applyAndPrint_ );

    ScaleConfiguration *config( session_->scaleConfig );
    cutterEnabled_ = config->scaleConfig.isCutterEnabled();

    LOG_CLASS_INFO( "PrinterSettingsDialog", "initialize" );
    if(cutterEnabled_ )
    {
        LOG_INFO("cutterEnabled", NULL);
    }
    else
    {
        LOG_INFO("cutterDisabled", NULL);
    }
    
    // eject and cut button
    ejectAndCut_ = new QPushButton( tr( "Eject && Cut Label" ), this );
    ejectAndCut_->setEnabled( false );
    ejectAndCut_->setHidden( true );
    ejectAndCut_->setFocusPolicy( Qt::NoFocus );
    ejectAndCut_->setObjectName( "ejectAndCut" );

    connect( ejectAndCut_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );
    buttons_->setMapping( ejectAndCut_, ejectAndCut_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::createLayout()
{
    // setup controls
    QVBoxLayout *controls = new QVBoxLayout();
    QHBoxLayout *hlayout;

    hlayout = new QHBoxLayout();
    hlayout->addWidget( currentPrinterLabel_ );
    hlayout->addWidget( currentPrinter_ );
    controls->addLayout( hlayout );

    controls->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addWidget( expelPositionLabel_ );
    hlayout->addWidget( expelPosition_ );
    controls->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( contrastLabel_ );
    hlayout->addWidget( contrast_ );
    controls->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( labelWidthLabel_ );
    hlayout->addWidget( labelWidth_ );
    controls->addLayout( hlayout );

    controls->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addWidget( horizontalPrintPositionLabel_ );
    hlayout->addWidget( horizontalPrintPosition_ );
    controls->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( verticalPrintPositionLabel_ );
    hlayout->addWidget( verticalPrintPosition_ );
    controls->addLayout( hlayout );

    controls->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( up_ );
    hlayout->addStretch();
    controls->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( left_ );
    hlayout->addWidget( right_ );
    hlayout->addStretch();
    controls->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( down_ );
    hlayout->addStretch();
    controls->addLayout( hlayout );

    controls->addStretch();
    controls->addWidget( ejectAndCut_ );
    controls->addWidget( applyAndPrint_ );

    // setup screen
    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    screenLayout->addWidget( preview_ );
    screenLayout->addLayout( controls );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool PrinterSettingsDialog::isEPP() const
{
    MadpPeripheralList *peripheralList( &session_->scaleConfig->peripheralList );

    int type( MadpPeripheralList::ptNone );

    // enable label width for epp printer
    if ( 0 == activePrinter_ )
        type = peripheralList->firstPrinter();
    else if ( 1 == activePrinter_ )
        type = peripheralList->secondPrinter();
    else if ( 2 == activePrinter_ )
        type = peripheralList->thirdPrinter();

    return (( MadpPeripheralList::ptEPP == type ) ||
        ( MadpPeripheralList::ptEPP2 == type ) ||
        ( MadpPeripheralList::ptEPP4 == type ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::updatePrinterConfig()
{
    // sanity check
    if (( activePrinter_ < 0 ) || ( printerConfig_.size() < activePrinter_ ))
        return;

    DomPrinterConfigRead m( printerConfig_[activePrinter_] );

    // update preview widget
    preview_->setPrinterConfig( m );

    // set contrast
    contrast_->setCurrentIndex( m.printContrast() );
    contrast_->setEnabled( true );

    // set label width
    labelWidth_->setCurrentIndex( labelWidth_->findData( m.labelWidth() ) );
    // enable label width for epp printers
    labelWidth_->setEnabled( isEPP() );

    // set vertical offset
    verticalPrintPosition_->setText( QString::number( m.vertPrintPosition() ) );

    // set horizontal offset
    horizontalPrintPosition_->setText( QString::number( m.horizPrintPosition() ) );

    // set expel position
    expelPosition_->setText( QString::number( m.expelPosition() ) );

    // enable buttons
    applyAndPrint_->setEnabled( true );
    ejectAndCut_->setEnabled(true);
    down_->setEnabled( true );
    left_->setEnabled( true );
    right_->setEnabled( true );
    up_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::updatePrinterStockMetrics()
{
    // sanity check
    if (( activePrinter_ < 0 ) || ( printerConfig_.size() < activePrinter_ ))
        return;

    DomPrinterStockMetrics m( printerStockMetrics_[activePrinter_] );

    // update preview widget
    preview_->setPrinterStockMetrics( m );

    // enable expel for continuous labels
    if ( "continuous" == m.stockLength().toLower() )
    {
        expelPosition_->setEnabled( true );
        expelPosition_->setReadOnly( false );
    }
    else
    {
        expelPosition_->setEnabled( false );
        expelPosition_->setReadOnly( true );
    }
}

void PrinterSettingsDialog::updatePrinterCutterStatus()
{
    // sanity check
    if (( activePrinter_ < 0 ) )
        return;

    DomPrinterCutterStatus m( printerCutterStatus_ );

    preview_->setPrinterCutterStatus( m );

    if (m.installed() && cutterEnabled_)
    {
        ejectAndCut_->setEnabled( true );
        ejectAndCut_->setHidden( false );
    }
    else
    {
        ejectAndCut_->setEnabled( false );
        ejectAndCut_->setHidden( true );
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsDialog::savePrinterConfig()
{
    // sanity check
    if (( activePrinter_ < 0 ) || ( printerConfig_.size() < activePrinter_ ))
        return;

    // nothing to do
    if ( !dirty_ )
        return;

    DomPrinterConfigRead m( printerConfig_[activePrinter_] );

    m.event() = QString::number( PrConfig );

    m.setExpelPosition( expelPosition_->text().toInt() );
    m.setPrintContrast( contrast_->currentIndex() );
    m.labelWidth() = "\"" + labelWidth_->itemData( labelWidth_->currentIndex() ).toString() + "\"";

    session_->send( m );

    dirty_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterSettingsPreviewWidget::PrinterSettingsPreviewWidget( QWidget *parent ) :
    _Mybase( parent ),
    havePrinterConfig_( false ),
    havePrinterStockMetrics_( false ),
    havePrinterCutterStatus_( false )
{
    setFrameShadow( QFrame::Sunken );
    setFrameShape( QFrame::Panel );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterSettingsPreviewWidget::~PrinterSettingsPreviewWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsPreviewWidget::setPrinterConfig( const DomPrinterConfigRead& m )
{
    printerConfig_ = m;
    havePrinterConfig_ = true;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsPreviewWidget::setPrinterStockMetrics( const DomPrinterStockMetrics& m )
{
    printerStockMetrics_ = m;
    havePrinterStockMetrics_ = true;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsPreviewWidget::setPrinterCutterStatus( const DomPrinterCutterStatus& m )
{
    printerCutterStatus_ = m;
    havePrinterCutterStatus_ = true;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterSettingsPreviewWidget::paintEvent( QPaintEvent* e )
{
    // draw base class
    _Mybase::paintEvent( e );

    QRect clientArea(
        rect().adjusted( frameWidth(), frameWidth(), -frameWidth(), -frameWidth() ) );

    // setup painter
    QPainter painter( this );
    painter.setPen( Qt::black );

    // fill client area
    painter.fillRect( clientArea, Qt::darkGray );

    // nothing to do when no settings
    if (( !havePrinterConfig_ ) || ( !havePrinterStockMetrics_ ) )
        return;

    int minWidth = clientArea.width() / 2;
    int maxWidth = (clientArea.width() * 7) / 8;

    // retrieve label width
    int w = printerStockMetrics_.stockWidthInDots();

    int horzOffset = printerConfig_.horizPrintPosition();
    int vertOffset = printerConfig_.vertPrintPosition();

    // make sure within valid range
    if ( w < minWidth )
        w = minWidth;
    else if ( maxWidth < w )
        w = maxWidth;

    // setup label rect
    QRect labelRect( clientArea );
    labelRect.adjust( ((clientArea.width() - w) / 2), 15, 0, -15 );
    labelRect.setWidth( w );

    // setup label text rect
    QRect labelTextRect( labelRect );
    labelTextRect.adjust( horzOffset, 0, 0, -vertOffset );

    // draw label
    painter.setBrush( Qt::white );
    painter.setPen( Qt::transparent );
    painter.drawRoundedRect( labelRect, 5.0, 5.0 );
    painter.setBrush( Qt::transparent );
    painter.setPen( Qt::black );

    // draw text rect
    painter.drawLine( labelTextRect.topLeft(), labelTextRect.bottomLeft() );
    painter.drawLine( labelTextRect.bottomLeft(), labelTextRect.bottomRight() );

    // draw text
    QString textLine( tr( "Print Line Print Line Print Line" ) );
    QString text( textLine + "\n" + textLine + "\n" + textLine );

    painter.drawText( labelTextRect, Qt::AlignLeft | Qt::AlignBottom, text );
}
