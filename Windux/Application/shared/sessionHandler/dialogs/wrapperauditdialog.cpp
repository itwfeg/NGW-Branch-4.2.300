/**
 * @file wrappermodedialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrapperauditdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <toolbutton.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <QComboBox>

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperAuditDialog::WrapperAuditDialog( const QStringList &trays, bool doubleRoll, int upperRollWidth, int lowerRollWidth, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    doubleRoll_( doubleRoll )
{
    LOG_CLASS_INFO( "WrapperAuditDialog", "WrapperAuditDialog" );

    int i;

    setObjectName( "WrapperAuditDialog" );

    // initialize
    initialize();
    createLayout();

    populateTrays( trays );

    LOG_INFO( "two roll %d, upper %d, lower %d", doubleRoll, upperRollWidth, lowerRollWidth );

    if ( !doubleRoll_ )
    {
        upperRollLabel_->hide();
        upperRollManfLabel_->hide();
        upperRollManf_->hide();
        upperRollWidthLabel_->hide();
        upperRollWidth_->hide();
        upperRollGaugeLabel_->hide();
        upperRollGauge_->hide();
    }
    else
    {
        i = upperRollWidth_->findText( QString::number( upperRollWidth ) + "\"" );

        if ( 0 <= i )
            upperRollWidth_->setCurrentIndex( i );
    }

    i = lowerRollWidth_->findText( QString::number( lowerRollWidth ) + "\"" );

    if ( 0 <= i )
        lowerRollWidth_->setCurrentIndex( i );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperAuditDialog::~WrapperAuditDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString WrapperAuditDialog::report() const
{
    QString details;

    details.append( "TRAYS APPROVED:\n" );

    for ( int i( 0 ); i < trays_->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem *item( const_cast<QTreeWidget*>( trays_ )->topLevelItem( i ) );

        if (( 2 != item->columnCount() ) || ( item->isHidden() ))
            continue;

        QCheckBox *box( qobject_cast<QCheckBox*>( trays_->itemWidget( item, 1 ) ) );

        if ( !box )
            continue;

        QString tray;
        tray += "  [";
        tray += box->isChecked() ? "X" : " ";
        tray += "] ";
        tray += item->text( 0 );

        details.append( tray + "\n" );
    }

    if ( doubleRoll_ )
    {
        details.append( "\n" );
        details.append( "UPPER ROLL:\n" );
        details.append( "  Film Type: " + upperRollManf_->currentText() + "\n" );
        details.append( "  Film Width: " + upperRollWidth_->currentText() + "\n" );
        details.append( "  Film Gauge: " + upperRollGauge_->currentText() + "\n" );
    }

    details.append( "\n" );
    details.append( "LOWER ROLL:\n" );
    details.append( "  Film Type: " + lowerRollManf_->currentText() + "\n" );
    details.append( "  Film Width: " + lowerRollWidth_->currentText() + "\n" );
    details.append( "  Film Gauge: " + lowerRollGauge_->currentText() + "\n" );

    details.append( "\n" );
    details.append( "COMMENTS:\n" );
    details.append( comments_->toPlainText() );

    return details;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperAuditDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 900) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 550) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *WrapperAuditDialog::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WrapperAuditDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::onPushButtonClicked()
{
    if ( okay_ == sender() )
    {
        bool needAllFields( false );
        bool needComments( false );

        if ( doubleRoll_ )
        {
            if (( 0 == upperRollManf_->currentIndex() ) ||
                ( 0 == upperRollWidth_->currentIndex() ) ||
                ( 0 == upperRollGauge_->currentIndex() ))
            {
                needAllFields = true;
            }

            if (( upperRollManf_->count() == (upperRollManf_->currentIndex() + 1) ) ||
                ( upperRollGauge_->count() == (upperRollGauge_->currentIndex() + 1) ))
                if ( comments_->toPlainText().isEmpty() )
                    needComments = true;
        }

        if (( 0 == lowerRollManf_->currentIndex() ) ||
            ( 0 == lowerRollWidth_->currentIndex() ) ||
            ( 0 == lowerRollGauge_->currentIndex() ))
        {
            needAllFields = true;
        }

        if (( lowerRollManf_->count() == (lowerRollManf_->currentIndex() + 1) ) ||
            ( lowerRollGauge_->count() == (lowerRollGauge_->currentIndex() + 1) ))
            if ( comments_->toPlainText().isEmpty() )
                needComments = true;

        if ( needAllFields )
        {
            error_->setText( tr( "Please fill out all fields!" ) );
            return;
        }
        else if ( needComments )
        {
            error_->setText( tr( "Please fill out comments section!" ) );
            return;
        }

        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::initialize()
{
    const QSize buttonSize(
        (mainWindow_->width() * 120) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 60) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    modifyPaletteForeground setRedForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::red ) );

    // ---- //

    upperRollLabel_ = new QLabel( tr( "UPPER ROLL" ), this );
    upperRollLabel_->setAlignment( Qt::AlignCenter );
    upperRollLabel_->setObjectName( "upperRollLabel" );

    upperRollManfLabel_ = new QLabel( tr( "Film Type:" ), this );
    upperRollManfLabel_->setAlignment( Qt::AlignLeft );
    upperRollManfLabel_->setObjectName( "upperRollManfLabel" );

    upperRollManf_ = new QComboBox( this );
    upperRollManf_->setObjectName( "upperRollManf" );

    upperRollWidthLabel_ = new QLabel( tr( "Film Width:" ), this );
    upperRollWidthLabel_->setAlignment( Qt::AlignLeft );
    upperRollWidthLabel_->setObjectName( "upperRollWidthLabel" );

    upperRollWidth_ = new QComboBox( this );
    upperRollWidth_->setObjectName( "upperRollWidth" );

    upperRollGaugeLabel_ = new QLabel( tr( "Film Gauge:" ), this );
    upperRollGaugeLabel_->setAlignment( Qt::AlignLeft );
    upperRollGaugeLabel_->setObjectName( "upperRollGaugeLabel" );

    upperRollGauge_ = new QComboBox( this );
    upperRollGauge_->setObjectName( "upperRollGauge" );

    // ---- //

    lowerRollLabel_ = new QLabel( tr( "LOWER ROLL" ), this );
    lowerRollLabel_->setAlignment( Qt::AlignCenter );
    lowerRollLabel_->setObjectName( "lowerRollLabel" );

    lowerRollManfLabel_ = new QLabel( tr( "Film Type:" ), this );
    lowerRollManfLabel_->setAlignment( Qt::AlignLeft );
    lowerRollManfLabel_->setObjectName( "lowerRollManfLabel" );

    lowerRollManf_ = new QComboBox( this );
    lowerRollManf_->setObjectName( "lowerRollManf" );

    lowerRollWidthLabel_ = new QLabel( tr( "Film Width:" ), this );
    lowerRollWidthLabel_->setAlignment( Qt::AlignLeft );
    lowerRollWidthLabel_->setObjectName( "lowerRollWidthLabel" );

    lowerRollWidth_ = new QComboBox( this );
    lowerRollWidth_->setObjectName( "lowerRollWidth" );

    lowerRollGaugeLabel_ = new QLabel( tr( "Film Gauge:" ), this );
    lowerRollGaugeLabel_->setAlignment( Qt::AlignLeft );
    lowerRollGaugeLabel_->setObjectName( "lowerRollGaugeLabel" );

    lowerRollGauge_ = new QComboBox( this );
    lowerRollGauge_->setObjectName( "lowerRollGauge" );

    // ---- //

    traysLabel_ = new QLabel( tr( "TRAY TYPES" ), this );
    traysLabel_->setAlignment( Qt::AlignCenter );
    traysLabel_->setObjectName( "traysLabel" );

    trays_ = new QTreeWidget( this );
    trays_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    trays_->setObjectName( "trays" );
    trays_->setRootIsDecorated( false );
    trays_->setSelectionMode( QAbstractItemView::NoSelection );
    trays_->setUniformRowHeights( true );

    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Tray Name" ) );
    tableHeaders.push_back( tr( "Approved" ) );

    trays_->setHeaderLabels( tableHeaders );
    trays_->setColumnCount( tableHeaders.size() );
    calcItemColumnWidths();

    // ---- //

    commentsLabel_ = new QLabel( tr( "Comments:" ), this );
    commentsLabel_->setAlignment( Qt::AlignLeft );
    commentsLabel_->setObjectName( "commentsLabel" );

    comments_ = new QPlainTextEdit( this );
    comments_->setObjectName( "comments" );

    // ---- //

    error_ = new QLabel( this );
    error_->setAlignment( Qt::AlignLeft );
    error_->setObjectName( "error" );
    setRedForegroundPalette( error_ );

    // okay
    okay_ = new hobart::toolButton( tr( "Okay" ), this, buttonPixmap );
    okay_->setObjectName( "okay" );
    setFontPointSize12( okay_ );
    setForegroundPalette( okay_ );

    connect( okay_, SIGNAL(clicked(bool)), SLOT(onPushButtonClicked()) );

    // cancel
    cancel_ = new hobart::toolButton( tr( "Cancel" ), this, buttonPixmap );
    cancel_->setObjectName( "cancel" );
    setFontPointSize12( cancel_ );
    setForegroundPalette( cancel_ );

    connect( cancel_, SIGNAL(clicked(bool)), SLOT(reject()) );

    // ---- //

    QStringList rollManf;
    rollManf.append( tr( "CHOOSE TYPE" ) );
    rollManf.append( tr( "Berry Plastics: Revolution  (48 ga)" ) );
    rollManf.append( tr( "Berry Plastics: Omni  (50 ga)" ) );
    rollManf.append( tr( "Berry Plastics: Omni  (60 ga)" ) );
    rollManf.append( tr( "Berry Plastics: Omni  (70 ga)" ) );
    rollManf.append( tr( "AEP: Elite L  (50 ga DL)" ) );
    rollManf.append( tr( "AEP: First Wrap M  (60 ga)" ) );
    rollManf.append( tr( "AEP: RMF - 61HY  (60 ga)" ) );
    rollManf.append( tr( "Filmco: Premium L  (50 ga DL)" ) );
    rollManf.append( tr( "Filmco: Premium M  (55 ga DL)" ) );
    rollManf.append( tr( "Filmco: HI - Y - G5  (60 ga)" ) );
    rollManf.append( tr( "Other (Specify in Comments)" ) );

    foreach( const QString &s, rollManf )
    {
        upperRollManf_->addItem( s );
        lowerRollManf_->addItem( s );
    }

    QStringList rollWidth;
    rollWidth.append( tr( "CHOOSE WIDTH" ) );
    rollWidth.append( "13\"" );
    rollWidth.append( "14\"" );
    rollWidth.append( "15\"" );
    rollWidth.append( "16\"" );
    rollWidth.append( "17\"" );
    rollWidth.append( "18\"" );
    rollWidth.append( "19\"" );
    rollWidth.append( "20\"" );

    foreach( const QString &s, rollWidth )
    {
        upperRollWidth_->addItem( s );
        lowerRollWidth_->addItem( s );
    }

    QStringList rollGauge;
    rollGauge.append( tr( "CHOOSE GAUGE" ) );
    rollGauge.append( tr( "50" ) );
    rollGauge.append( tr( "55" ) );
    rollGauge.append( tr( "60" ) );
    rollGauge.append( tr( "65" ) );
    rollGauge.append( tr( "70" ) );
    rollGauge.append( tr( "75" ) );
    rollGauge.append( tr( "80" ) );
    rollGauge.append( tr( "85" ) );
    rollGauge.append( tr( "90" ) );
    rollGauge.append( tr( "95" ) );
    rollGauge.append( tr( "100" ) );
    rollGauge.append( tr( "Other (Specify in Comments)" ) );

    foreach( const QString &s, rollGauge )
    {
        upperRollGauge_->addItem( s );
        lowerRollGauge_->addItem( s );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::createLayout()
{
    QVBoxLayout *traysLayout( new QVBoxLayout() );
    traysLayout->addWidget( traysLabel_ );
    traysLayout->addWidget( trays_, 1 );

    QFormLayout *upperRollLayout( new QFormLayout() );
    upperRollLayout->addRow( upperRollManfLabel_, upperRollManf_ );
    upperRollLayout->addRow( upperRollWidthLabel_, upperRollWidth_ );
    upperRollLayout->addRow( upperRollGaugeLabel_, upperRollGauge_ );

    QFormLayout *lowerRollLayout( new QFormLayout() );
    lowerRollLayout->addRow( lowerRollManfLabel_, lowerRollManf_ );
    lowerRollLayout->addRow( lowerRollWidthLabel_, lowerRollWidth_ );
    lowerRollLayout->addRow( lowerRollGaugeLabel_, lowerRollGauge_ );

    QVBoxLayout *rollLayout( new QVBoxLayout() );
    rollLayout->addWidget( upperRollLabel_ );
    rollLayout->addLayout( upperRollLayout );
    rollLayout->addStretch();
    rollLayout->addWidget( lowerRollLabel_ );
    rollLayout->addLayout( lowerRollLayout );
    rollLayout->addStretch();

    QHBoxLayout *upperLayout( new QHBoxLayout() );
    upperLayout->addLayout( traysLayout, 1 );
    upperLayout->addLayout( rollLayout, 2 );

    QVBoxLayout *commentsLayout( new QVBoxLayout() );
    commentsLayout->addWidget( commentsLabel_ );
    commentsLayout->addWidget( comments_, 1 );

    QHBoxLayout *buttons( new QHBoxLayout() );
    buttons->addWidget( error_ );
    buttons->addStretch();
    buttons->addWidget( okay_ );
    buttons->addWidget( cancel_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addLayout( upperLayout, 2 );
    screenLayout->addLayout( commentsLayout, 1 );
    screenLayout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::calcItemColumnWidths()
{
    int tableWidth( trays_->contentsRect().width() - 32 );

    trays_->setColumnWidth( 0, (tableWidth * 60) / 100 ); // tray name
    trays_->setColumnWidth( 1, (tableWidth * 30) / 100 ); // tested
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperAuditDialog::populateTrays( const QStringList &trays )
{
    trays_->clear();

    // add each tray
    foreach ( const QString &tray, trays )
    {
        QTreeWidgetItem *item;
        QStringList rows;

        // item data
        rows.push_back( tray );
        rows.push_back( QString() );

        // create widget item
        item = new QTreeWidgetItem( rows );
        item->setSizeHint( 0, QSize( 32, 32 ) );

        trays_->addTopLevelItem( item );

        // add box to check
        QCheckBox *box( new QCheckBox( this ) );
        trays_->setItemWidget( item, 1, box );
    }
}
