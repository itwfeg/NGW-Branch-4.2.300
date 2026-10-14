/**
 * @file voidtransactionpanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "sizelabelspanel.h"

#include "../sessiondialog.h"
#include "../sessionmainwindow.h"

#include <font.h>
#include <backgroundwidgettypes.h>
#include <palette.h>
#include <pushbutton.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

static const int PRINTER_1 = 0;
static const int PRINTER_2 = 1;
static const int PRINTER_3 = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
SizeLabelsPanel::SizeLabelsPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "SizeLabelsPanel" );

    labelSizes_.resize( 3 );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SizeLabelsPanel::~SizeLabelsPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SizeLabelsPanel", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if (( Printer == device ) && ( PrStockMetrics == event ))
    {
        DomPrinterStockMetrics m( msg );

        QString label_deets = m.stockWidth().append(" x ").append(m.stockLength() );

        if( m.printerType() == PRINTER_1)
        {
            printer1Size_->setText( label_deets );                        // 2.250 x 3.000
        }
        else if( m.printerType() == PRINTER_2 )
        {
            printer2Size_->setText( label_deets );                        // 2.250 x 6.500
            printer2Label_->setVisible( true );
            printer2Size_->setVisible( true );
            resizePrinter2_->setVisible( true );
        }
        else if( m.printerType() == PRINTER_3 )
        {
            printer3Size_->setText( label_deets );                        // 2.250 x 8.000

            printer3Label_->setVisible( true );
            printer3Size_->setVisible( true );
            resizePrinter3_->setVisible( true );
        }
    }
    else if (( Printer == device ) && ( PrAreWeSizingLabels == event ))
    {
        DomPrAreWeSizingLabels m( msg );

        if( m.sizingLabels() == 1)
            areWeSizingLabels_->setText( tr("Sizing all labels.\n") );
        else
            areWeSizingLabels_->setText( tr("Continuous label stock selected.\nLabel sizing not being performed.") );
    }
    else if (( Printer == device ) && ( PrCutterStatus == event ))
    {
        // SOF-4958: Don't remove "Use Continuous" button for Free Standing scale with Cutter
        MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
        const QString scaleModel( settings->scaleName() );
        bool isFreeStandandingModel_ = false;

        if( scaleModel.length() >= 2 && ( scaleModel.toUpper().startsWith( "FS" ) ) )
        {
            isFreeStandandingModel_ = true;
        }

        DomPrinterCutterStatus m( msg );
        if( ( m.installed() == 1 ) && !( isFreeStandandingModel_ ) )
        {
            useContinuous1_->setVisible( false );    
        }   
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::onResizePrinter1ButtonClicked()
{
    LOG_CLASS_INFO( "SizeLabelsPanel", "onResizePrinter1ButtonClicked" );

    // send message to size the labels for Printer 1
    session_->send( Printer, PrReqLabelSizing, "0" );
    session_->send( Printer, PrReqAreWeSizingLabels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::onResizePrinter2ButtonClicked()
{
    LOG_CLASS_INFO( "SizeLabelsPanel", "onResizePrinter2ButtonClicked" );

    // send message to size the labels for Printer 2
    session_->send( Printer, PrReqLabelSizing, "1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::onResizePrinter3ButtonClicked()
{
    LOG_CLASS_INFO( "SizeLabelsPanel", "onResizePrinter3ButtonClicked" );

    // send message to size the labels for Printer 3
    session_->send( Printer, PrReqLabelSizing, "2" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::onUseContinuous1ButtonClicked()
{
    LOG_CLASS_INFO( "SizeLabelsPanel", "onUseContinuous1ButtonClicked" );

    // send message to use continuous stock for Printer 1
    session_->send( Printer, PrUseContinuousStock, "0" );
    session_->send( Printer, PrReqAreWeSizingLabels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SizeLabelsPanel::initialize()
{
    // send message to get label sizes for all Printers
    session_->send( SessionDev, SnPrinterMetrics );
    session_->send( Printer, PrReqAreWeSizingLabels );

    static const int spacing( 7 );
    static const int hspacing( 7 );
    static const int vspacing( 50 );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    widgetsLayout_->setSpacing( spacing );

    // The next line really messes up the layout but other forms have it without the same result
    // So Im leaving it here in case it needs to be added back in tho I dont know what it is today
    //QLayoutItem *doomed = widgetsLayout_->takeAt( 1 );

    bottomLayout_->addStretch();

    // See previous comment
    //if (( doomed ) && ( doomed->spacerItem() ) )
    //    delete doomed->spacerItem();

    // set panel title
    setTitle( tr( "Size Labels" ) );

    labelSizes_[0] = "";
    labelSizes_[1] = "";
    labelSizes_[2] = "";

    const QSize buttonSize(
        (mainWindow_->width() * 139) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 49) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::Normal ) );


    // setup layouts

    //theGrid_
    theGrid_ = new QGridLayout();
    theGrid_->setHorizontalSpacing( hspacing );
    theGrid_->setVerticalSpacing( vspacing );
    widgetsLayout_->addLayout(theGrid_);

    QFont f( font() );
    f.setPointSize(48);
    setFont( f );

    spacer_ = new QLabel( this );
    spacer_->setObjectName( "doYouThinkQAReallyLooksAtTheNameOfTheseObjectsBecauseImPrettySureTheCouldntCareLess" );
    spacer_->setFont(f);
    spacer_->setText("   ");
    widgetsLayout_->addWidget( spacer_ );

    QFont f14( font() );
    f14.setPointSize(14);
    setFont( f14 );

    //printer1Label_->set
    printer1Label_ = new QLabel( tr("Printer") + " 1", this );
    printer2Label_ = new QLabel( tr("Printer") + " 2", this );
    printer3Label_ = new QLabel( tr("Printer") + " 3", this );
    printer1Label_->setFont(f14);
    printer2Label_->setFont(f14);
    printer3Label_->setFont(f14);

    printer1Size_ = new QLabel( " ", this );                        // 2.250 x 3.000
    printer2Size_ = new QLabel( " ", this );                        // 2.250 x 6.500
    printer3Size_ = new QLabel( " ", this );                        // 2.250 x 8.000

    theGrid_->addWidget( printer1Label_, 0, 0 );
//    theGrid_->addWidget( printer2Label_, 0, 1 );
//    theGrid_->addWidget( printer3Label_, 0, 2 );

    printer2Label_->setVisible( false );
    printer3Label_->setVisible( false );
    printer2Size_->setVisible( false );
    printer3Size_->setVisible( false );

    theGrid_->addWidget( printer1Size_, 1, 0 );
//    theGrid_->addWidget( printer2Size_, 1, 1 );
//    theGrid_->addWidget( printer3Size_, 1, 2 );

    printer2Label_->setEnabled( false );
    printer3Label_->setEnabled( false );
    printer2Size_->setEnabled( false );
    printer3Size_->setEnabled( false );

    // resizePrinter1_
    resizePrinter1_ = new hobart::pushButton( tr( "Size Labels" ), this, buttonPixmap );
    resizePrinter1_->setEnabled( true );
    resizePrinter1_->setFocusPolicy( Qt::NoFocus );
    resizePrinter1_->setObjectName( "resizePrinter1Button" );
    setForeground( resizePrinter1_ );

    // resizePrinter2_
    resizePrinter2_ = new hobart::pushButton( tr( "Size Labels" ), this, buttonPixmap );
    resizePrinter2_->setEnabled( false );
    resizePrinter2_->setFocusPolicy( Qt::NoFocus );
    resizePrinter2_->setObjectName( "resizePrinter2Button" );
    setForeground( resizePrinter2_ );

    // resizePrinter3_
    resizePrinter3_ = new hobart::pushButton( tr( "Size Labels" ), this, buttonPixmap );
    resizePrinter3_->setEnabled( false );
    resizePrinter3_->setFocusPolicy( Qt::NoFocus );
    resizePrinter3_->setObjectName( "resizePrinter3Button" );
    setForeground( resizePrinter3_ );

    // useContinuous1_
    useContinuous1_ = new hobart::pushButton( tr( "Use Continuous" ), this, buttonPixmap );
    useContinuous1_->setEnabled( true );
    useContinuous1_->setFocusPolicy( Qt::NoFocus );
    useContinuous1_->setObjectName( "useContinuous1Button" );
    setForeground( useContinuous1_ );

    resizePrinter1_->setVisible( true );
    resizePrinter2_->setVisible( false );
    resizePrinter3_->setVisible( false );
    useContinuous1_->setVisible( true );

    connect( resizePrinter1_, SIGNAL( clicked() ), SLOT( onResizePrinter1ButtonClicked() ) );
    connect( resizePrinter2_, SIGNAL( clicked() ), SLOT( onResizePrinter2ButtonClicked() ) );
    connect( resizePrinter3_, SIGNAL( clicked() ), SLOT( onResizePrinter3ButtonClicked() ) );
    connect( useContinuous1_, SIGNAL( clicked() ), SLOT( onUseContinuous1ButtonClicked() ) );

    theGrid_->addWidget( resizePrinter1_, 2, 0 );
//    theGrid_->addWidget( resizePrinter2_, 2, 1 );
//    theGrid_->addWidget( resizePrinter3_, 2, 2 );

    theGrid_->addWidget( useContinuous1_, 3, 0 );
    QFont f12( font() );
    f12.setPointSize(12);
    setFont( f12 );

    areWeSizingLabels_ = new QLabel( " ", this );
    areWeSizingLabels_->setVisible( true );
    areWeSizingLabels_->setFixedWidth( buttonSize.width()*2 );
    theGrid_->addWidget( areWeSizingLabels_, 4, 0 );

    session_->send( Printer, PrReqCutterStatus);
}
