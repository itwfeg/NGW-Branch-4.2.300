/**
 * @file scaleauditdialog.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "scaleauditdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <toolbutton.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ScaleAuditDialog::ScaleAuditDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    setObjectName( "ScaleAuditDialog" );

    // initialize
    initialize();
    createLayout();

    // setup tree
    reasonTree_.insert( 100, tr( "Printing" ) );
        reasonTree_.insert( 110, tr( "Print Position" ) );
        reasonTree_.insert( 120, tr( "Cassette" ) );
            reasonTree_.insert( 121, tr( "Damaged" ) );
            reasonTree_.insert( 122, tr( "Latch Adjustment" ) );
            reasonTree_.insert( 123, tr( "Loose Hardware" ) );
            reasonTree_.insert( 124, tr( "Print Roller" ) );
            reasonTree_.insert( 125, tr( "Take Up" ) );
            reasonTree_.insert( 126, tr( "Light Print" ) );
        reasonTree_.insert( 130, tr( "Printhead" ) );
        reasonTree_.insert( 140, tr( "Media Sensor" ) );
            reasonTree_.insert( 141, tr( "Calibrate" ) );
            reasonTree_.insert( 142, tr( "Replace" ) );
        reasonTree_.insert( 150, tr( "Motor" ) );
        reasonTree_.insert( 160, tr( "Belt" ) );
        reasonTree_.insert( 170, tr( "Pulley / Drive Gear" ) );
        reasonTree_.insert( 180, tr( "Other" ) );
    reasonTree_.insert( 200, tr( "Weighing" ) );
        reasonTree_.insert( 210, tr( "Calibrate" ) );
        reasonTree_.insert( 220, tr( "Scale Not Zero" ) );
        reasonTree_.insert( 230, tr( "Outside Maintenance" ) );
        reasonTree_.insert( 240, tr( "Replace Load Cell" ) );
        reasonTree_.insert( 250, tr( "Missing Peripheral" ) );
        reasonTree_.insert( 260, tr( "Other" ) );
    reasonTree_.insert( 300, tr( "Electrical" ) );
        reasonTree_.insert( 310, tr( "Main Board" ) );
        reasonTree_.insert( 320, tr( "Power Supply" ) );
        reasonTree_.insert( 330, tr( "Fan Assembly" ) );
        reasonTree_.insert( 340, tr( "Communication" ) );
            reasonTree_.insert( 341, tr( "Wireless Card" ) );
            reasonTree_.insert( 342, tr( "Ethernet" ) );
            reasonTree_.insert( 343, tr( "Other" ) );
        reasonTree_.insert( 350, tr( "I/O Board" ) );
        reasonTree_.insert( 360, tr( "Other" ) );
    reasonTree_.insert( 400, tr( "Display / Touchscreen" ) );
        reasonTree_.insert( 410, tr( "Display" ) );
            reasonTree_.insert( 411, tr( "Customer" ) );
            reasonTree_.insert( 412, tr( "Operator" ) );
        reasonTree_.insert( 420, tr( "Display Cable" ) );
            reasonTree_.insert( 421, tr( "Customer" ) );
            reasonTree_.insert( 422, tr( "Operator" ) );
        reasonTree_.insert( 430, tr( "Touchscreen Calibrate" ) );
        reasonTree_.insert( 440, tr( "Touchscreen Replaced" ) );
        reasonTree_.insert( 450, tr( "Other" ) );
    reasonTree_.insert( 500, tr( "Software" ) );
        reasonTree_.insert( 510, tr( "Micro SD Reseated" ) );
        reasonTree_.insert( 520, tr( "Micro SD Replaced" ) );
        reasonTree_.insert( 530, tr( "Unknown Micro SD" ) );
        reasonTree_.insert( 540, tr( "Locked Up / Frozen" ) );
        reasonTree_.insert( 550, tr( "Date/Time incorrect" ) );
        reasonTree_.insert( 560, tr( "Incorrect Configuration (2nd Check digit, Min Weight to Print, etc)" ) );
        reasonTree_.insert( 570, tr( "Missing Files" ) );
        reasonTree_.insert( 580, tr( "Other" ) );
    reasonTree_.insert( 600, tr( "Other" ) );

    // populate
    populateReason( 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScaleAuditDialog::~ScaleAuditDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ScaleAuditDialog::report() const
{
    QString details;

    details.append( "REASON FOR VISIT:\n" );
    details.append( reason1_->currentText() + "\n" );

    if ( reason2_->count() )
        details.append( reason2_->currentText() + "\n" );

    if ( reason3_->count() )
        details.append( reason3_->currentText() + "\n" );

    details.append( "\n" );

    details.append( "COMMENTS:\n" );
    details.append( comments_->toPlainText() );

    return details;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ScaleAuditDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 900) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 550) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *ScaleAuditDialog::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ScaleAuditDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::checkOkayEnabled()
{
    bool enabled( true );

    if (( tr( "Other" ) == reason1_->currentText() ) ||
        ( tr( "Other" ) == reason2_->currentText() ) ||
        ( tr( "Other" ) == reason3_->currentText() ))
    {
        if ( comments_->toPlainText().isEmpty() )
        {
            error_->setText( tr( "Please enter comments" ) );
            enabled = false;
        }
    }

    okay_->setEnabled( enabled );

    if ( enabled )
        error_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::onCurrentIndexChanged( int index )
{
    Q_UNUSED( index )

    if ( reason1_ == sender() )
        populateReason( 2 );
    else if ( reason2_ == sender() )
        populateReason( 3 );

    checkOkayEnabled();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::onPushButtonClicked()
{
    if ( okay_ == sender() )
    {
/*
        bool needComments( false );

        if ( comments_->toPlainText().isEmpty() )
            needComments = true;

        if ( needComments )
        {
            error_->setText( tr( "Please fill out comments section!" ) );
            return;
        }
*/
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::initialize()
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

    reasonForVisitLabel_ = new QLabel( tr( "Reason For Visit:" ), this );
    reasonForVisitLabel_->setAlignment( Qt::AlignLeft );
    reasonForVisitLabel_->setObjectName( "reasonForVisitLabel" );

    reason1_ = new QComboBox( this );
    reason1_->setObjectName( "reason1" );

    connect( reason1_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

    reason2_ = new QComboBox( this );
    reason2_->setObjectName( "reason2" );
    reason2_->setVisible( false );

    connect( reason2_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

    reason3_ = new QComboBox( this );
    reason3_->setObjectName( "reason3" );
    reason3_->setVisible( false );

    connect( reason3_, SIGNAL(currentIndexChanged(int)), SLOT(onCurrentIndexChanged(int)) );

    // ---- //

    commentsLabel_ = new QLabel( tr( "Comments:" ), this );
    commentsLabel_->setAlignment( Qt::AlignLeft );
    commentsLabel_->setObjectName( "commentsLabel" );

    comments_ = new QPlainTextEdit( this );
    comments_->setObjectName( "comments" );

    connect( comments_, SIGNAL(textChanged()), SLOT(checkOkayEnabled()) );

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::createLayout()
{
    QVBoxLayout *reasonLayout( new QVBoxLayout() );
    reasonLayout->addWidget( reasonForVisitLabel_ );
    reasonLayout->addWidget( reason1_ );
    reasonLayout->addWidget( reason2_ );
    reasonLayout->addWidget( reason3_ );

    QVBoxLayout *commentsLayout( new QVBoxLayout() );
    commentsLayout->addWidget( commentsLabel_ );
    commentsLayout->addWidget( comments_, 1 );

    QHBoxLayout *buttons( new QHBoxLayout() );
    buttons->addWidget( error_ );
    buttons->addStretch();
    buttons->addWidget( okay_ );
    buttons->addWidget( cancel_ );

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addLayout( reasonLayout );
    screenLayout->addItem( new QSpacerItem( 16, 16 ) );
    screenLayout->addLayout( commentsLayout, 1 );
    screenLayout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScaleAuditDialog::populateReason( int which )
{
    QComboBox *reason;

    int index;
    int step;

    switch ( which )
    {
    case 1:
        reason = reason1_;
        index = 0;
        step = 100;
        break;
    case 2:
        reason = reason2_;
        index = reason1_->currentData().toInt();
        step = 10;
        break;
    case 3:
        reason = reason3_;
        index = reason2_->currentData().toInt();
        step = 1;
        break;
    default:
        return;
    }

    reason->blockSignals( true );
    reason->clear();

    for ( int i( index + step ); reasonTree_.contains( i ); i += step )
        reason->addItem( reasonTree_[i], QVariant( i ) );

    reason->setVisible( 0 < reason->count() );
    reason->blockSignals( false );

    populateReason( which + 1 );
}
