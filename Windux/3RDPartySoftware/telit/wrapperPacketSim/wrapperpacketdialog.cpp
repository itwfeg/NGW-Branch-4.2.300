
#include "edgedevicecommthread.h"
#include "wrapperpacketdialog.h"

#include "DiagnosticsEdgeDevice.h"

#ifdef min
#undef min
#endif

#include <QCheckBox>
#include <QDateTime>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

std::string CfgDiagGatewayDeviceAddress;
std::string CfgSerial;
std::string CfgModel;
std::string CfgAddress;
std::string CfgHostname;

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperPacketDialog::wrapperPacketDialog( QWidget *parent ) :
    _Mybase( parent ),
    edgeDevice_( NULL ),
    edgeDeviceComms_( NULL )
{
    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperPacketDialog::~wrapperPacketDialog()
{
    shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::shutdown()
{
    if ( edgeDeviceComms_ )
    {
        edgeDeviceComms_->quit();

        while ( edgeDeviceComms_->isRunning() )
            QThread::msleep( 250 );

        delete edgeDeviceComms_;
        edgeDeviceComms_ = NULL;
    }

    if ( edgeDevice_ )
    {
        delete edgeDevice_;
        edgeDevice_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::startup()
{
    CfgDiagGatewayDeviceAddress = qPrintable( serverAddress_->text() );
    CfgSerial = qPrintable( serial_->text() );
    CfgModel = qPrintable( model_->text() );
    CfgAddress = qPrintable( address_->text() );
    CfgHostname = qPrintable( hostname_->text() );

    edgeDevice_ = new DiagnosticsEdgeDevice();

    if ( wrapper_->isChecked() )
    {
        std::vector<std::string> nodeApp;
        std::vector<std::string> nodeData;
        std::vector<std::string> nodeBoot;
        std::vector<std::string> nodeChecksum;

        for ( unsigned int node( 6 ); node--; )
        {
            nodeApp.push_back( "10.20" );
            nodeData.push_back( "1.5" );
            nodeBoot.push_back( "1.9" );
            nodeChecksum.push_back( "74094" );
        }

        edgeDevice_->setWrapperBeltState( true, 190, 177 );
        edgeDevice_->setWrapperCompressorState( 122, 51, 277 );
        edgeDevice_->setWrapperFilmState( 21, 33, 18, 67 );
        edgeDevice_->setWrapperInfo( "AWS-2RL", "12-3456-789" );
        edgeDevice_->setWrapperNodeVersions( nodeApp, nodeData, nodeBoot, nodeChecksum );
        edgeDevice_->setWrapperVacuumState( 30194, 429 );
    }

    if ( weigher_->isChecked() )
    {
        edgeDevice_->setWeigherChecksum( "02FC" );
        edgeDevice_->setWeigherHardwareVersion( "0.0" );
        edgeDevice_->setWeigherSoftwareVersion( "1.4" );
    }

    if ( printer1_->isChecked() )
    {
        edgeDevice_->setPrinterChecksum( 0, "815A" );
        edgeDevice_->setPrinterHardwareVersion( 0, "0.0" );
        edgeDevice_->setPrinterSoftwareVersion( 0, "2.4" );
    }

    if ( printer2_->isChecked() )
    {
        edgeDevice_->setPrinterChecksum( 1, "815A" );
        edgeDevice_->setPrinterHardwareVersion( 1, "0.0" );
        edgeDevice_->setPrinterSoftwareVersion( 1, "2.4" );
    }

    if ( printer3_->isChecked() )
    {
        edgeDevice_->setPrinterChecksum( 2, "815A" );
        edgeDevice_->setPrinterHardwareVersion( 2, "0.0" );
        edgeDevice_->setPrinterSoftwareVersion( 2, "2.4" );
    }

    if ( errors_->isChecked() )
    {
        util::Dictionary err1;
        err1.append( "errorCode", "9520" );
        err1.append( "timestamp", "1472727897000" );
        err1.append( "count", "1" );
        err1.append( "latestOccurrence", "1472727897000" );
        err1.append( "severity", "4" );

        util::Dictionary err2;
        err2.append( "errorCode", "9655" );
        err2.append( "timestamp", "1472639277000" );
        err2.append( "count", "5" );
        err2.append( "latestOccurrence", "1472689277000" );
        err2.append( "severity", "3" );

        util::Dictionary err3;
        err3.append( "errorCode", "9657" );
        err3.append( "timestamp", "1472719277000" );
        err3.append( "count", "2" );
        err3.append( "latestOccurrence", "1472729277000" );
        err3.append( "severity", "3" );

        healthEventMap_[9520] = err1;
        healthEventMap_[9655] = err2;
        healthEventMap_[9657] = err3;

        sendHealthEvents();
    }

    edgeDeviceComms_ = new edgeDeviceCommThread( edgeDevice_, this );
    edgeDeviceComms_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::onPushButtonClicked()
{
    if ( startStopClient_ == sender() )
    {
        if ( edgeDeviceComms_ )
        {
            shutdown();

            startStopClient_->setText( "Start" );
            serverInfo_->setEnabled( true );
            statusEvent_->setEnabled( false );
            healthEvent_->setEnabled( false );
            diagEvent_->setEnabled( false );
        }
        else
        {
            startup();

            startStopClient_->setText( "Stop" );
            serverInfo_->setEnabled( false );
            statusEvent_->setEnabled( true );
            healthEvent_->setEnabled( true );
            diagEvent_->setEnabled( true );
        }
    }
    else if ( sendStatusEvent_ == sender() )
    {
        edgeDevice_->sendEvent(
            statusEventErrorCode_->text().toUInt(),
            statusEventSeverity_->text().toUInt(),
            diagOffered_->isChecked() ,
            diagForced_->isChecked() );
    }
    else if ( sendHealthEvent_ == sender() )
    {
        const int errorCode( healthEventErrorCode_->text().toInt() );
        const QString now( QString::number( QDateTime::currentMSecsSinceEpoch() / 1000 ) );

        util::Dictionary err;
        err.append( "errorCode", qPrintable( healthEventErrorCode_->text() ) );
        err.append( "timestamp", qPrintable( now ) );
        err.append( "count", "1" );
        err.append( "latestOccurrence", qPrintable( now ) );
        err.append( "latestDiag", qPrintable( generatedFromDiag_->isChecked() ? now : QString() ) );
        err.append( "severity", qPrintable( healthEventSeverity_->text() ) );

        err.append( "serviceMethod1", qPrintable( healthEventMethod1_->text() ) );
        err.append( "servicePart1", qPrintable( healthEventPart1_->text() ) );
        err.append( "serviceMethod2", qPrintable( healthEventMethod2_->text() ) );
        err.append( "servicePart2", qPrintable( healthEventPart2_->text() ) );
        err.append( "serviceMethod3", qPrintable( healthEventMethod3_->text() ) );
        err.append( "servicePart3", qPrintable( healthEventPart3_->text() ) );
        err.append( "serviceMethod4", qPrintable( healthEventMethod4_->text() ) );
        err.append( "servicePart4", qPrintable( healthEventPart4_->text() ) );

        if ( healthEventNotes_->text().length() )
            err.append( "serviceNotes", qPrintable( healthEventNotes_->text() ) );

        healthEventMap_[errorCode] = err;
        sendHealthEvents();

        if ( resolved_->isChecked() )
        {
            healthEventMap_.remove( errorCode );
            sendHealthEvents();
        }
    }
    else if ( resolved_ == sender() )
    {
        foreach ( QWidget *w, resolutionWidgets_ )
            w->setEnabled( resolved_->isChecked() );
    }
    else if ( sendDiagEvent_ == sender() )
    {
        DiagnosticsEdgeDevice::diagResult result;

        if ( timeout_->isChecked() )
            result = DiagnosticsEdgeDevice::FailTimeout;
        else if ( failed_->isChecked() )
            result = DiagnosticsEdgeDevice::Fail;
        else
            result = DiagnosticsEdgeDevice::Pass;

        edgeDevice_->sendDiagnostics(
            diagEventErrorCode_->text().toInt(),
            result,
            diagEventResultCode_->text().toUInt(),
            diagEventRoutine_->text().toInt(),
            diagEventExpected_->text().toInt(),
            diagEventActual_->text().toInt(),
            byOperator_->isChecked(),
            byService_->isChecked() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::initialize()
{
    serverInfo_ = new QGroupBox( "Server Info", this );

    serverAddressLabel_ = new QLabel( "Server Address (port 5080)", serverInfo_ );
    serverAddress_ = new QLineEdit( "10.3.176.61", serverInfo_ );

    serialLabel_ = new QLabel( "Serial Number", serverInfo_ );
    serial_ = new QLineEdit( "451128834", serverInfo_ );

    modelLabel_ = new QLabel( "Model Name", serverInfo_ );
    model_ = new QLineEdit( "EPCP-5i", serverInfo_ );

    addressLabel_ = new QLabel( "IP Address", serverInfo_ );
    address_ = new QLineEdit( "127.0.0.1", serverInfo_ );

    hostnameLabel_ = new QLabel( "Hostname", serverInfo_ );
    hostname_ = new QLineEdit( "ht00026b146f26", serverInfo_ );

    wrapper_ = new QCheckBox( "Wrapper", serverInfo_ );
    weigher_ = new QCheckBox( "Weigher", serverInfo_ );
    printer1_ = new QCheckBox( "Printer 1", serverInfo_ );
    printer2_ = new QCheckBox( "Printer 2", serverInfo_ );
    printer3_ = new QCheckBox( "Printer 3", serverInfo_ );
    errors_ = new QCheckBox( "Previous Errors", serverInfo_ );

    wrapper_->setChecked( true );
    weigher_->setChecked( true );
    printer1_->setChecked( true );

    startStopClient_ = new QPushButton( "Start", serverInfo_ );

    connect( startStopClient_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // ---- //

    statusEvent_ = new QGroupBox( "Status Event", this );
    statusEvent_->setEnabled( false );

    statusEventErrorCodeLabel_ = new QLabel( "Error code", serverInfo_ );
    statusEventErrorCode_ = new QLineEdit( "9622", serverInfo_ );

    statusEventSeverityLabel_ = new QLabel( "Severity (1-4)", serverInfo_ );
    statusEventSeverity_ = new QLineEdit( "2", serverInfo_ );

    diagOffered_ = new QCheckBox( "Diag Offered", serverInfo_ );
    diagForced_ = new QCheckBox( "Diag Forced", serverInfo_ );

    sendStatusEvent_ = new QPushButton( "Send", serverInfo_ );

    connect( sendStatusEvent_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // ---- //

    healthEvent_ = new QGroupBox( "Health Event", this );
    healthEvent_->setEnabled( false );

    healthEventErrorCodeLabel_ = new QLabel( "Error code", healthEvent_ );
    healthEventErrorCode_ = new QLineEdit( "9633", healthEvent_ );

    healthEventSeverityLabel_ = new QLabel( "Severity (1-4)", healthEvent_ );
    healthEventSeverity_ = new QLineEdit( "3", healthEvent_ );

    generatedFromDiag_ = new QCheckBox( "Generated from Diag", healthEvent_ );
    resolved_ = new QCheckBox( "Resolved", healthEvent_ );

    healthEventMethodsLabel_ = new QLabel( "Service Methods", healthEvent_ );
    healthEventMethod1_ = new QLineEdit( "10", healthEvent_ );
    healthEventMethod2_ = new QLineEdit( "", healthEvent_ );
    healthEventMethod3_ = new QLineEdit( "", healthEvent_ );
    healthEventMethod4_ = new QLineEdit( "", healthEvent_ );

    healthEventPartsLabel_ = new QLabel( "Service Parts", healthEvent_ );
    healthEventPart1_ = new QLineEdit( "12", healthEvent_ );
    healthEventPart2_ = new QLineEdit( "", healthEvent_ );
    healthEventPart3_ = new QLineEdit( "", healthEvent_ );
    healthEventPart4_ = new QLineEdit( "", healthEvent_ );

    healthEventNotesLabel_ = new QLabel( "Notes", healthEvent_ );
    healthEventNotes_ = new QLineEdit( "", healthEvent_ );

    sendHealthEvent_ = new QPushButton( "Send", healthEvent_ );

    connect( resolved_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
    connect( sendHealthEvent_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    resolutionWidgets_.append( healthEventMethodsLabel_ );
    resolutionWidgets_.append( healthEventMethod1_ );
    resolutionWidgets_.append( healthEventMethod2_ );
    resolutionWidgets_.append( healthEventMethod3_ );
    resolutionWidgets_.append( healthEventMethod4_ );
    resolutionWidgets_.append( healthEventPartsLabel_ );
    resolutionWidgets_.append( healthEventPart1_ );
    resolutionWidgets_.append( healthEventPart2_ );
    resolutionWidgets_.append( healthEventPart3_ );
    resolutionWidgets_.append( healthEventPart4_ );
    resolutionWidgets_.append( healthEventNotesLabel_ );
    resolutionWidgets_.append( healthEventNotes_ );

    foreach ( QWidget *w, resolutionWidgets_ )
        w->setEnabled( false );

    // ---- //

    diagEvent_ = new QGroupBox( "Diagnostics Event", this );
    diagEvent_->setEnabled( false );

    diagEventErrorCodeLabel_ = new QLabel( "Error code", diagEvent_ );
    diagEventErrorCode_ = new QLineEdit( "9622", diagEvent_ );

    diagEventResultCodeLabel_ = new QLabel( "Diagnostic result", diagEvent_ );
    diagEventResultCode_ = new QLineEdit( "9633", diagEvent_ );

    diagEventRoutineLabel_ = new QLabel( "Test routine number", diagEvent_ );
    diagEventRoutine_ = new QLineEdit( "17", diagEvent_ );

    diagEventExpectedLabel_ = new QLabel( "Test expected", diagEvent_ );
    diagEventExpected_ = new QLineEdit( "1234", diagEvent_ );

    diagEventActualLabel_ = new QLabel( "Test actual", diagEvent_ );
    diagEventActual_ = new QLineEdit( "4321", diagEvent_ );

    passed_ = new QCheckBox( "Passed", serverInfo_ );
    failed_ = new QCheckBox( "Failed", serverInfo_ );
    timeout_ = new QCheckBox( "Timeout", serverInfo_ );
    byOperator_ = new QCheckBox( "Operator", serverInfo_ );
    byService_ = new QCheckBox( "Service Tech", serverInfo_ );

    sendDiagEvent_ = new QPushButton( "Send", serverInfo_ );

    connect( sendDiagEvent_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::createLayout()
{
    QFormLayout *serverInfoForm( new QFormLayout );
    serverInfoForm->addRow( serverAddressLabel_, serverAddress_ );
    serverInfoForm->addRow( serialLabel_, serial_ );
    serverInfoForm->addRow( modelLabel_, model_ );
    serverInfoForm->addRow( addressLabel_, address_ );
    serverInfoForm->addRow( hostnameLabel_, hostname_ );

    QHBoxLayout *serverInfoPeripherals( new QHBoxLayout );
    serverInfoPeripherals->addWidget( wrapper_ );
    serverInfoPeripherals->addWidget( weigher_ );
    serverInfoPeripherals->addWidget( printer1_ );
    serverInfoPeripherals->addWidget( printer2_ );
    serverInfoPeripherals->addWidget( printer3_ );
    serverInfoPeripherals->addWidget( errors_ );

    QVBoxLayout *serverInfo( new QVBoxLayout( serverInfo_ ) );
    serverInfo->addLayout( serverInfoForm );
    serverInfo->addLayout( serverInfoPeripherals );

    // ---- //

    QFormLayout *statusEventForm( new QFormLayout );
    statusEventForm->addRow( statusEventErrorCodeLabel_, statusEventErrorCode_ );
    statusEventForm->addRow( statusEventSeverityLabel_, statusEventSeverity_ );

    QHBoxLayout *statusEventOptions( new QHBoxLayout );
    statusEventOptions->addWidget( diagOffered_ );
    statusEventOptions->addWidget( diagForced_ );

    QVBoxLayout *statusEvent( new QVBoxLayout( statusEvent_ ) );
    statusEvent->addLayout( statusEventForm );
    statusEvent->addLayout( statusEventOptions );
    statusEvent->addWidget( sendStatusEvent_ );

    // ---- //

    QFormLayout *healthEventForm( new QFormLayout );
    healthEventForm->addRow( healthEventErrorCodeLabel_, healthEventErrorCode_ );
    healthEventForm->addRow( healthEventSeverityLabel_, healthEventSeverity_ );

    QHBoxLayout *healthEventOptions( new QHBoxLayout );
    healthEventOptions->addWidget( generatedFromDiag_ );
    healthEventOptions->addWidget( resolved_ );

    QHBoxLayout *healthEventServiceMethods( new QHBoxLayout );
    healthEventServiceMethods->addWidget( healthEventMethodsLabel_, 1 );
    healthEventServiceMethods->addWidget( healthEventMethod1_, 1 );
    healthEventServiceMethods->addWidget( healthEventMethod2_, 1 );
    healthEventServiceMethods->addWidget( healthEventMethod3_, 1 );
    healthEventServiceMethods->addWidget( healthEventMethod4_, 1 );

    QHBoxLayout *healthEventServiceParts( new QHBoxLayout );
    healthEventServiceParts->addWidget( healthEventPartsLabel_, 1 );
    healthEventServiceParts->addWidget( healthEventPart1_, 1 );
    healthEventServiceParts->addWidget( healthEventPart2_, 1 );
    healthEventServiceParts->addWidget( healthEventPart3_, 1 );
    healthEventServiceParts->addWidget( healthEventPart4_, 1 );

    QHBoxLayout *healthEventServiceNotes( new QHBoxLayout );
    healthEventServiceNotes->addWidget( healthEventNotesLabel_ );
    healthEventServiceNotes->addWidget( healthEventNotes_ );

    QVBoxLayout *healthEvent( new QVBoxLayout( healthEvent_ ) );
    healthEvent->addLayout( healthEventForm );
    healthEvent->addLayout( healthEventOptions );
    healthEvent->addLayout( healthEventServiceMethods );
    healthEvent->addLayout( healthEventServiceParts );
    healthEvent->addLayout( healthEventServiceNotes );
    healthEvent->addWidget( sendHealthEvent_ );

    // ---- //

    QFormLayout *diagEventForm( new QFormLayout );
    diagEventForm->addRow( diagEventErrorCodeLabel_, diagEventErrorCode_ );
    diagEventForm->addRow( diagEventResultCodeLabel_, diagEventResultCode_ );
    diagEventForm->addRow( diagEventRoutineLabel_, diagEventRoutine_ );
    diagEventForm->addRow( diagEventExpectedLabel_, diagEventExpected_ );
    diagEventForm->addRow( diagEventActualLabel_, diagEventActual_ );

    QHBoxLayout *diagEventOptions( new QHBoxLayout );
    diagEventOptions->addWidget( passed_ );
    diagEventOptions->addWidget( failed_ );
    diagEventOptions->addWidget( timeout_ );
    diagEventOptions->addWidget( byOperator_ );
    diagEventOptions->addWidget( byService_ );

    QVBoxLayout *diagEvent( new QVBoxLayout( diagEvent_ ) );
    diagEvent->addLayout( diagEventForm );
    diagEvent->addLayout( diagEventOptions );
    diagEvent->addWidget( sendDiagEvent_ );

    // ---- //

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addWidget( startStopClient_ );
    layout->addWidget( serverInfo_ );
    layout->addWidget( statusEvent_ );
    layout->addWidget( diagEvent_ );
    layout->addWidget( healthEvent_ );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperPacketDialog::sendHealthEvents()
{
    util::DictionaryList healthInfo;

    foreach ( const util::Dictionary &e, healthEventMap_ )
        healthInfo.append( e );

    edgeDevice_->setHealthInfo( healthInfo );
}
