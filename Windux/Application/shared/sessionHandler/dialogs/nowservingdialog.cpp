/**
 * @file nowservingdialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "nowservingdialog.h"
#include "numericinputboxdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingDialog::NowServingDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    error_( false ),
    waitingForResponse_( 0 )
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    if ( 1 == scaleConfig->nowServingSysType() )
        numberIssuer_ = Electronic;
    else
        numberIssuer_ = PrePrintedPaper;



    // set object name
    setObjectName( "NowServingDialog" );

    // initialize
    initialize();
    createLayout();

    // get queue status
    requestQueueStatus( true, true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NowServingDialog::~NowServingDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize NowServingDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 640) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 450) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::hideEvent( QHideEvent *e )
{
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::showEvent( QShowEvent *e )
{
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // version info
    if (( SessionDev == device ) && ( SnNowServingQueueManagement == event ))
    {
        NowServingQueueManagement m( msg );

        bool isResponseMessage( waitingForResponse_ == m.action() );

        if ( isResponseMessage )
            waitingForResponse_ = 0;

        // validate message received
        if ( validateMsg( m ) )
        {
            switch ( m.action() )
            {
            case NowServingQueueManagement::ServingNextCustomer:
                // prompting of next customer number is done elsewhere
                if ( isResponseMessage )
                    accept();
                break;

            case NowServingQueueManagement::GetNextCustomerInLine:
                custQueueNext_->setText( m.value1() );
                custQueueNextLabel_->setVisible( 0 <= m.value1().toInt() );
                custQueueNext_->setVisible( 0 <= m.value1().toInt() );
                break;

            case NowServingQueueManagement::DeleteAllCustomersInLine:
                if ( isResponseMessage )
                    requestQueueStatus( true, false );
                break;

            case NowServingQueueManagement::GetCustomerQueueSize:
                custQueueSize_->setText( m.value1() );
                custQueueSizeLabel_->show();
                custQueueSize_->show();
                break;

            case NowServingQueueManagement::ServeSpecificCustomer:
                // prompting of next customer number is done elsewhere
                if ( isResponseMessage )
                    accept();
                break;

            case NowServingQueueManagement::PrintCustomerNumber:
                if ( isResponseMessage )
                    accept();
                break;

            case NowServingQueueManagement::SetNextCustomerNumber:
                if ( isResponseMessage )
                    requestQueueStatus( false, true );
                break;

            case NowServingQueueManagement::GetNextCustomerNumber:

                // for paper issuer this is our next customer in line
                if ( PrePrintedPaper == numberIssuer_ )
                {
                    custQueueNext_->setText( m.value1() );
                    custQueueNextLabel_->setVisible( 0 <= m.value1().toInt() );
                    custQueueNext_->setVisible( 0 <= m.value1().toInt() );
                }
                else
                {
                    printerQueueNext_->setText( m.value1() );
                    printerQueueNextLabel_->show();
                    printerQueueNext_->show();
                }
                break;

            case NowServingQueueManagement::IncrementCustomerNumber:
                if ( isResponseMessage )
                    accept();

            case NowServingQueueManagement::ResetQueue:
                if ( isResponseMessage )
                    requestQueueStatus( true, true );
                break;

            default:
                break;
            }
        }
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::onButtonClicked( QWidget *widget )
{
    // sanity check
    if ( !widget )
        return;

    NowServingQueueManagement::actionType action( NowServingQueueManagement::Unknown );
    QString actionParam;

    // display on GCD now serving customer, pop customer off the queue
    if ( serveNextCustomer_ == widget )
    {
        if ( PrePrintedPaper == numberIssuer_ )
            action = NowServingQueueManagement::IncrementCustomerNumber;
        else
            action = NowServingQueueManagement::ServingNextCustomer;
    }

    // serve customer X
    else if ( serveSpecificCustomer_ == widget )
    {
        QIntValidator *v( new QIntValidator( 1, 999, this ) );

        // close this dialog
        accept();

        NumericInputBoxDialog dialog( mainWindow_ );
        dialog.setText( tr( "Please enter customer number:" ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
        dialog.setValidator( v );
        dialog.setInputWrapping( true );
        dialog.setInputTruncation( true );

        dialog.setInputValue( 1 );

        // prompt
        if ( QDialog::Accepted != dialog.exec() )
            return;

       actionParam = dialog.inputText();

        // validate
        if (( actionParam.isEmpty() ) || ( "0" == actionParam ))
            return;

        action = NowServingQueueManagement::ServeSpecificCustomer;
    }

    // delete all queued customers
    else if ( deleteCustomerQueue_ == widget )
        action = NowServingQueueManagement::DeleteAllCustomersInLine;

    // PRINT next customer number, increment number
    else if ( printCustomerLabel_ == widget )
        action = NowServingQueueManagement::PrintCustomerNumber;

    // set next printed customer number to X
    // reset next printed customer to X, delete all queued customers
    else if (( setNextCustomerNumber_ == widget ) || ( resetEverything_ == widget ))
    {
        QIntValidator *v( new QIntValidator( 1, 999, this ) );

        // close this dialog
        accept();

        NumericInputBoxDialog dialog( mainWindow_ );
        dialog.setText( tr( "Please enter starting number:" ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
        dialog.setValidator( v );
        dialog.setInputWrapping( true );
        dialog.setInputTruncation( true );

        dialog.setInputValue( 1 );

        // prompt
        if ( QDialog::Accepted != dialog.exec() )
            return;

        actionParam = dialog.inputText();

        // validate
        if (( actionParam.isEmpty() ) || ( "0" == actionParam ))
            actionParam = "1";

        if ( setNextCustomerNumber_ == widget )
            action = NowServingQueueManagement::SetNextCustomerNumber;
        else if ( resetEverything_ == widget )
            action = NowServingQueueManagement::ResetQueue;
    }

    session_->send(
        SessionDev,
        SnNowServingQueueManagement,
        QString::number( waitingForResponse_ = action ) +
            (actionParam.isEmpty() ? QString() : (QString( ",," ) + actionParam)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::initialize()
{
    modifyFontPointSize setFontPointSize14( (mainWindow_->height() * 14) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize14( this );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // title
    titleLabel_ = new QLabel( tr( "Now Serving Queue Management" ), this );
    titleLabel_->setObjectName( "titleLabel" );
    setFontPointSize18( titleLabel_ );

    // next customer
    custQueueNextLabel_ = new QLabel( tr( "Next Customer In Line:" ), this );
    custQueueNextLabel_->setObjectName( "custQueueNextLabel" );
    custQueueNextLabel_->setVisible( false );

    custQueueNext_ = new QLabel( this );
    custQueueNext_->setObjectName( "custQueueNext" );
    custQueueNext_->setVisible( false );

    // queue size
    custQueueSizeLabel_ = new QLabel( tr( "Queue Size:" ), this );
    custQueueSizeLabel_->setObjectName( "custQueueSizeLabel" );
    custQueueSizeLabel_->setVisible( false );

    custQueueSize_ = new QLabel( this );
    custQueueSize_->setObjectName( "custQueueSize" );
    custQueueSize_->setVisible( false );

    // serve next customer
    serveNextCustomer_ = new QPushButton( tr( "Serve Next\nCustomer" ), this );
    serveNextCustomer_->setObjectName( "serveNextCustomer" );
    serveNextCustomer_->setEnabled( mainWindow_->isPrimarySessionType() );

    buttonSignalMapper_->setMapping( serveNextCustomer_, serveNextCustomer_ );
    connect( serveNextCustomer_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // serve specific customer
    serveSpecificCustomer_ = new QPushButton( tr( "Serve Specific\nCustomer" ), this );
    serveSpecificCustomer_->setObjectName( "serveSpecificCustomer" );
    serveSpecificCustomer_->setEnabled( mainWindow_->isPrimarySessionType() );

    buttonSignalMapper_->setMapping( serveSpecificCustomer_, serveSpecificCustomer_ );
    connect( serveSpecificCustomer_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // delete queued customers
    deleteCustomerQueue_ = new QPushButton( tr( "Delete All\nCustomers in Queue" ), this );
    deleteCustomerQueue_->setObjectName( "deleteCustomerQueue" );

    buttonSignalMapper_->setMapping( deleteCustomerQueue_, deleteCustomerQueue_ );
    connect( deleteCustomerQueue_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // printer number
    printerQueueNextLabel_ = new QLabel( tr( "Next Printed Number:" ), this );
    printerQueueNextLabel_->setObjectName( "printerQueueNextLabel" );
    printerQueueNextLabel_->setVisible( false );

    printerQueueNext_ = new QLabel( this );
    printerQueueNext_->setObjectName( "printerQueueNext" );
    printerQueueNext_->setVisible( false );

    // print customer label
    //printCustomerLabel_ = new QPushButton( tr( "Print Label" ), this );
    //printCustomerLabel_->setObjectName( "printLabel_" );
    //printCustomerLabel_->setEnabled( mainWindow_->isPrimarySessionType() );


    //buttonSignalMapper_->setMapping( printCustomerLabel_, printCustomerLabel_ );
    //connect( printCustomerLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // set printer customer number
    setNextCustomerNumber_ = new QPushButton( tr( "Set Next Customer" ), this );
    setNextCustomerNumber_->setObjectName( "setNextPrinterNumber" );

    buttonSignalMapper_->setMapping( setNextCustomerNumber_, setNextCustomerNumber_ );
    connect( setNextCustomerNumber_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // reset
    resetEverything_ = new QPushButton( tr( "Reset System" ), this );
    resetEverything_->setObjectName( "resetEverything" );

    buttonSignalMapper_->setMapping( resetEverything_, resetEverything_ );
    connect( resetEverything_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

    // ---- //

    QList<QPushButton*> redButtons;
    redButtons.push_back( deleteCustomerQueue_ );
    redButtons.push_back( setNextCustomerNumber_ );
    redButtons.push_back( resetEverything_ );

    foreach ( QPushButton *b, redButtons )
    {
        QPalette p( b->palette() );
        p.setColor( QPalette::Active, QPalette::ButtonText, hobart::colorPalette::red );
        b->setPalette( p );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::createLayout()
{
    QHBoxLayout *title = new QHBoxLayout();
    title->addStretch();
    title->addWidget( titleLabel_ );
    title->addStretch();

    QFormLayout *custQueueInfoInner = new QFormLayout();
    custQueueInfoInner->addRow( custQueueSizeLabel_, custQueueSize_ );
    custQueueInfoInner->addRow( custQueueNextLabel_, custQueueNext_ );

    QHBoxLayout *custQueueInfo = new QHBoxLayout();
    custQueueInfo->addStretch();
    custQueueInfo->addLayout( custQueueInfoInner );
    custQueueInfo->addStretch();

    QHBoxLayout *custActions = new QHBoxLayout();
    custActions->addStretch();
    custActions->addWidget( serveNextCustomer_ );
    custActions->addWidget( serveSpecificCustomer_ );
    custActions->addWidget( deleteCustomerQueue_ );
    custActions->addStretch();

    QFormLayout *printerQueueInfoInner = new QFormLayout();
    printerQueueInfoInner->addRow( printerQueueNextLabel_, printerQueueNext_ );

    QHBoxLayout *printerQueueInfo = new QHBoxLayout();
    printerQueueInfo->addStretch();
    printerQueueInfo->addLayout( printerQueueInfoInner );
    printerQueueInfo->addStretch();

    QHBoxLayout *printerActions = new QHBoxLayout();
    printerActions->addStretch();
    //printerActions->addWidget( printCustomerLabel_ );
    printerActions->addWidget( setNextCustomerNumber_ );
    printerActions->addStretch();

    QHBoxLayout *systemActions = new QHBoxLayout();
    systemActions->addWidget( resetEverything_ );

    QVBoxLayout *formLayout = new QVBoxLayout( this );
    formLayout->addLayout( title );
    formLayout->addStretch();
    formLayout->addLayout( custQueueInfo );
    formLayout->addLayout( custActions );
    formLayout->addStretch();
    formLayout->addLayout( printerQueueInfo );
    formLayout->addLayout( printerActions );
    formLayout->addStretch();
    formLayout->addLayout( systemActions );

    // ---- //

    // for paper issuer only some of the actions are relevant
    if ( PrePrintedPaper == numberIssuer_ )
    {
        serveSpecificCustomer_->hide();
        deleteCustomerQueue_->hide();
        //printCustomerLabel_->hide();
        resetEverything_->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NowServingDialog::requestQueueStatus( bool cust, bool printer )
{
    if (( cust ) && ( Electronic == numberIssuer_ ))
    {
        // request customer queue size
        session_->send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::GetCustomerQueueSize ) );

        // request next customer in line
        session_->send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::GetNextCustomerInLine ) );
    }

    if ( printer )
    {
        // request current printer number
        session_->send( SessionDev, SnNowServingQueueManagement, QString::number( NowServingQueueManagement::GetNextCustomerNumber ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool NowServingDialog::validateMsg( const NowServingQueueManagement& m )
{
    return (0 == m.resultCode());
}
