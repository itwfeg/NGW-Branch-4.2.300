/**
 * @file serviceHealthAgent.cpp
 *
 * @copyright Copyright (c) 2013-2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 * @author Randy Blankley
 */

#include "serviceHealthAgent.h"

#include <sessionmainwindow.h>

#include <dialogs/diagresolutiondialog.h>
#include <dialogs/filedialog.h>
#include <dialogs/messageboxdialog.h>
#include <dialogs/scaleauditdialog.h>
#include <dialogs/wrapperauditdialog.h>

#include <modes/servicemode.h>

#include <widgets/labeleddateedit.h>

#include <filesystem.h>
#include <font.h>
#include <logmanager.h>

#include <ScaleConfiguration.h>

#include <QByteArray>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceHealthAgent::serviceHealthAgent( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    valueMaxWeigher_( false ),
    configs_( false ),
    printing_( false ),
    printingDiagReport_( false ),
    printingAuditReport_( false )
{
    // check if we are in service mode
    // service mode will enable extra functionality
    serviceMode_ = (( dynamic_cast<ServiceMode*>( mode_ ) ) ? true : false );

    // check for value max weigher
    const DomWeigherConfigRead *weigherFields( &session_->scaleConfig->weigherConfigRead );

    if ( SessionMainWindow::HTM == weigherFields->weigherModel() )
        valueMaxWeigher_ = true;

    // initialize
    initialize();
    createLayout();
    translate();

    // request diagnostic log
    requestMaintLog();

    // request diagnostics configuration and status
    session_->send( Diagnostics, DiagConfigurationReq );

    requestDiagStatus();
    requestAuditLogs();

    // request service visit status
    session_->send( Diagnostics, DiagAuditLogServiceVisitStatus );

    requestServiceSettings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceHealthAgent::~serviceHealthAgent()
{
    updateDiagConfig();

    // cancel any active report
    if ( printing_ )
        cancelPrinting();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::translate()
{
    tab_->setTabText( diagMaintLogTab_, tr( "Diagnostics" ) );
    tab_->setTabText( diagConfigTab_, tr( "Diagnostics Config" ) );
    tab_->setTabText( healthLogTab_, tr( "Health Agent" ) );
    tab_->setTabText( auditLogTab_, tr( "Audit Log" ) );

    //
    // diag maintenance log
    //

    QStringList diagTableHeaders;
    diagTableHeaders.push_back( tr( "Id" ) );
    diagTableHeaders.push_back( tr( "Date" ) );
    diagTableHeaders.push_back( tr( "Diagnostic\nCode" ) );
    diagTableHeaders.push_back( tr( "Occurrences" ) );
    diagTableHeaders.push_back( tr( "First\nOccurrence" ) );
    diagTableHeaders.push_back( tr( "" ) );
    diagTableHeaders.push_back( tr( "" ) );

    diagTable_->setHeaderLabels( diagTableHeaders );
    diagTable_->setColumnCount( diagTableHeaders.size() );

    // update button text
    for ( int i( 0 ); i < diagTable_->topLevelItemCount(); ++i )
    {
        QPushButton *diagButton;
        QPushButton *clearButton;

        QTreeWidgetItem *item( diagTable_->topLevelItem( i ) );

        if ( (clearButton = reinterpret_cast<QPushButton*>( diagTable_->itemWidget( item, DiagClearButton ) )) )
            clearButton->setText( tr( "Resolve" ) );

        if ( (diagButton = reinterpret_cast<QPushButton*>( diagTable_->itemWidget( item, DiagDiagnoseButton ) )) )
            diagButton->setText( tr( "Diagnose" ) );
    }

    translatePrintMaintLogButton();
    exportDiagLog_->setText( tr( "Export\nLog to USB" ) );
    emailDiagLog_->setText( tr( "Send Log\nvia Email" ) );
    uploadDiagLog_->setText( tr( "Upload Log\nvia Gateway" ) );

    //
    // diag config
    //

    serviceContactLabel_->setText( tr( "Call For Service Contact" ) );
    gatewayAddressLabel_->setText( tr( "Gateway Device Address" ) );
    gatewayLogLevelLabel_->setText( tr( "Gateway Log Level" ) );

    gatewayLogLevel_->setItemText( 0, tr( "DISABLED" ) );
    gatewayLogLevel_->setItemText( 1, tr( "Info" ) );
    gatewayLogLevel_->setItemText( 2, tr( "Warning" ) );
    gatewayLogLevel_->setItemText( 3, tr( "Error" ) );
    gatewayLogLevel_->setItemText( 4, tr( "Critical" ) );

    disableValueMaxDiagCollection_->setText( tr( "Disable ValueMax Diagnostics Collection" ) );

    //
    // health log
    //

    heading_->setText( tr( "Health Agent Log" ) );
    clearLog_->setText( tr( "Clear Log" ) );
    exportLog_->setText( tr("Export Logs to USB") );

    if ( emptyLog_ )
    {
        log_->clear();
        log_->appendPlainText( tr( "No data to display." ) );
    }

    //
    // audit log
    //

    QStringList auditTableHeaders;
    auditTableHeaders.push_back( tr( "Id" ) );
    auditTableHeaders.push_back( tr( "Date" ) );
    auditTableHeaders.push_back( tr( "Severity" ) );
    auditTableHeaders.push_back( tr( "Source" ) );
    auditTableHeaders.push_back( tr( "Description" ) );

    auditTable_->setHeaderLabels( auditTableHeaders );
    auditTable_->setColumnCount( auditTableHeaders.size() );

    translatePrintAuditButton();
    prevAuditLog_->setText( tr( "Prev" ) );
    nextAuditLog_->setText( tr( "Next" ) );
    startServiceVisit_->setText( tr( "Start Service Visit" ) );
    stopServiceVisit_->setText( tr( "Stop Service Visit" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceHealthAgent::eventFilter( QObject *obj, QEvent *e )
{
    if ( QEvent::Resize == e->type() )
    {
        if ( diagTable_ == obj )
            calcDiagMaintTableColumnWidths();
        else if ( auditTable_ == obj  )
            calcAuditTableColumnWidths();
    }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::cancelPrinting()
{
    session_->send( ReportManagerDev, CancelReport );

    printing_ = printingDiagReport_ = printingAuditReport_ = false;
    translatePrintAuditButton();
    translatePrintMaintLogButton();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceHealthAgent", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // diagnostics configuration
    if (( Diagnostics == device ) && ( DiagConfiguration == event ))
    {
        DiagConfigurationMsg m( msg );

        serviceContact_->setText( m.serviceContactInfo() );
        gatewayAddress_->setText( m.gatewayDeviceAddress() );
        gatewayLogLevel_->setCurrentIndex( m.gatewayDeviceLogLevel() );
        disableValueMaxDiagCollection_->setChecked( m.disableValueMaxDiagCollection() );

        configs_ = true;
    }

    // diagnostics status
    else if (( Diagnostics == device ) && ( DiagStatus == event ))
    {
        DiagStatusMsg m( msg );

        if ( m.gatewayOnline() )
        {
            QString status;

            status += tr( "Connected to" ) + " " + m.gatewayAddress() + " ";

            if ( m.gatewayDiscovered() )
                status += "(" + tr( "Discovered" ) + ")";

            gatewayStatus_->setText( status );

            modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::green, hobart::colorPalette::green ) );
            setForegroundPalette( gatewayStatus_ );
        }
        else
        {
            gatewayStatus_->setText(
                tr( "Gateway Currently Offline!" ) +
                " (" + QString::number( m.offlineMessages() ) + " " + tr( "messages queued" ) + ")" );

            modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::red, hobart::colorPalette::red ) );
            setForegroundPalette( gatewayStatus_ );
        }
    }

    // service visit status
    else if (( Diagnostics == device ) && ( DiagAuditLogServiceVisitStatus == event ))
    {
        DiagAuditLogServiceVisitStatusMsg m( msg );

        startServiceVisit_->setEnabled( m.start() );
        stopServiceVisit_->setEnabled( m.stop() );
    }

    // error handler failed
    else if (( Diagnostics == device ) && ( DiagErrorHandlerFailed == event ))
    {
        session_->send( Error, ErrDiagnostics, "173000,5" );
    }

    // service maintenance log
    else if (( Diagnostics == device ) && ( DiagAuditLog == event ))
    {
        DiagAuditLogMsg m( msg );

        // retrieve records
        if ( !m.count() )
        {
            // clear existing items
            auditTable_->clear();
        }
        else
        {
            QVector<QString> records( TokenParser::splitStringTokens( m.auditRecords() ) );

            if ( records.count() != m.count() )
                LOG_WARN( "mismatch in record count %d %d", m.count(), records.count() );
            else
            {
                // clear existing items
                auditTable_->clear();

                // process records
                foreach ( const QString &record, records )
                {
                    QVector<QString> fields( TokenParser::splitStringTokens( record ) );

                    // record structure:
                    // 0  timestamp
                    // 1  timestamp string
                    // 2  severity
                    // 3  severity string
                    // 4  source
                    // 5  source string
                    // 6  source extended
                    // 7  source extended string

                    // validate structure
                    if ( 8 != fields.count() )
                        LOG_ERROR( "unknown record structure for audit log!", NULL );
                    else
                    {
                        // item data
                        QStringList rows;
                        rows.push_back( fields[0] );
                        rows.push_back( fields[1] );
                        rows.push_back( fields[3] );
                        rows.push_back( fields[5] );
                        rows.push_back( fields[7] );

                        // create widget item
                        QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
                        item->setSizeHint( 1, QSize( 40, 40 ) );

                        calcAuditTableFontSizes( item );

                        // add item!
                        auditTable_->addTopLevelItem( item );
                    }

                } // for each record
            } // records == parsed records
        } // records
    }

    // service maintenance log record
    else if (( Diagnostics == device ) && ( DiagAuditLogRecord == event ))
    {
        DiagAuditLogRecordMsg m( msg );

        printAuditLog_->setEnabled( true );
        auditLogDetails_->setPlainText( m.description() );
    }

    // service maintenance log
    else if (( Diagnostics == device ) && ( DiagServiceMaintLog == event ))
    {
        DiagServiceMaintLogMsg m( msg );

        // retrieve records
        if ( !m.count() )
        {
            // clear existing items
            diagTable_->clear();
        }
        else
        {
            QVector<QString> records( TokenParser::splitStringTokens( m.maintRecords() ) );

            if ( records.count() != m.count() )
                LOG_WARN( "mismatch in record count %d %d", m.count(), records.count() );
            else
            {
                // clear existing items
                diagTable_->clear();

                // process records
                foreach ( const QString &record, records )
                {
                    QVector<QString> fields( TokenParser::splitStringTokens( record ) );

                    // record structure:
                    // 0  timestamp
                    // 1  timestamp string
                    // 2  errorCode
                    // 3  state
                    // 4  count
                    // 5  latestOccurrence
                    // 6  latestOccurrence string
                    // 7  latestDiag
                    // 8  latestDiag string
                    // 9  resolved
                    // 10 resolved string
                    // 11 serviceMethod1
                    // 12 servicePart1
                    // 13 serviceMethod2
                    // 14 servicePart2
                    // 15 serviceMethod3
                    // 16 servicePart3
                    // 17 serviceMethod4
                    // 18 servicePart4
                    // 19 serviceNotes
                    // 20 severity

                    // validate structure
                    if ( 21 != fields.count() )
                        LOG_ERROR( "unknown record structure for maint log!", NULL );
                    else
                    {
                        // item data
                        QStringList rows;
                        rows.push_back( fields[0] );
                        rows.push_back( fields[6] );
                        rows.push_back( fields[2] );
                        rows.push_back( fields[4] );
                        rows.push_back( fields[1] );
                        rows.push_back( QString() );
                        rows.push_back( QString() );

                        // create widget item
                        QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
                        item->setSizeHint( 1, QSize( 64, 64 ) );

                        calcDiagMaintTableFontSizes( item );

                        // add item!
                        diagTable_->addTopLevelItem( item );

                        int index( diagTable_->indexOfTopLevelItem( item ) );

                        // create resolve button
                        QPushButton *clearButton( new QPushButton( tr( "Resolve" ), diagMaintLog_ ) );
                        clearButton->setEnabled( serviceMode_ && mainWindow_->isPrimarySessionType() );
                        clearButton->setFont( diagTableFont_ );
                        clearButton->setObjectName( QString( "clear_%1" ).arg( index ) );

                        connect( clearButton, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

                        diagTable_->setItemWidget( item, DiagClearButton, clearButton );

                        // create diagnose button
                        QPushButton *diagButton( new QPushButton( tr( "Diagnose" ), diagMaintLog_ ) );
                        diagButton->setEnabled( mainWindow_->isPrimarySessionType() );
                        diagButton->setFont( diagTableFont_ );
                        diagButton->setObjectName( QString( "diag_%1" ).arg( index ) );

                        connect( diagButton, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

                        diagTable_->setItemWidget( item, DiagDiagnoseButton, diagButton );
                    }

                } // for each record
            } // records == parsed records
        } // records

    }

    // export database complete
    else if (( Diagnostics == device ) && ( DiagExportDatabaseComplete == event ))
    {
        DiagExportDatabaseCompleteMsg m( msg );

        exportDiagLog_->setEnabled( true );

        if ( m.success() )
            session_->send( Error, ErrDiagnostics, "173005,5" );
        else
            session_->send( Error, ErrDiagnostics, "173006,5" );
    }

    // send database complete
    else if (( Diagnostics == device ) && ( DiagSendDatabaseComplete == event ))
    {
        DiagSendDatabaseCompleteMsg m( msg );

        emailDiagLog_->setEnabled( true );
        uploadDiagLog_->setEnabled( true );

        if ( m.success() )
            session_->send( Error, ErrDiagnostics, "173007,5" );
        else
            session_->send( Error, ErrDiagnostics, "173008,5" );
    }

    // wrapper settings
    else if (( Wrapper == device ) && ( WrEvolInquire == event ))
    {
        DomEvolWrapperConfig m( msg );

        upperRollWidth_ = m.upperRollWidth().toInt() / 10;
        lowerRollWidth_ = m.lowerRollWidth().toInt() / 10;
    }

    // wrapper system info
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        DomEvolWrapperSystemInformation m( msg );

        singleRoll_ = m.isSingleRoll();
        doubleRoll_ = m.isDoubleRoll();
    }

    // wrapper tray types
    else if (( Wrapper == device ) && ( WrEvolWriteAllTrayTypes == event ))
    {
        DomNewWrapperAllTrays m( msg );

        const QVector<QString> trayTypes( TokenParser::splitStringTokens( m.trayTypes() ) );
        const QVector<QString> pluExceptions( TokenParser::splitStringTokens( m.pluExceptions() ) );

        if (( trayTypes.size() == m.numOfTrays().toInt() ) &&
            ( pluExceptions.size() == m.numOfExceptions().toInt() ) &&
            ( trayTypes.size() == pluExceptions.size() ))
        {
            for ( int i( 0 ); i < trayTypes.size(); ++i )
            {
                const QString pluException( pluExceptions[i].trimmed() );

                if (( pluException.length() ) && ( "0" != pluException ))
                    trays_.append( trayTypes[i] + " [PLU EXCEPTION: " + pluException + "]" );
                else
                    trays_.append( trayTypes[i] );
            }
        }
    }

    // report complete
    else if (( ReportManagerDev == device ) && ( TerminateReport == event ))
    {
        printing_ = printingDiagReport_ = printingAuditReport_ = false;
        translatePrintAuditButton();
        translatePrintMaintLogButton();
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::requestAuditLogs()
{
    // request audit log (all records, last 365 days worth)
    session_->send( Diagnostics, DiagAuditLogReq, "-1,365" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::requestDiagStatus()
{
    session_->send( Diagnostics, DiagStatusReq );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::requestMaintLog()
{
    DiagServiceMaintLogReqMsg diagLogRequest;
    diagLogRequest.setFilter( DiagServiceMaintLogReqMsg::OpenRecordsOnly );
    diagLogRequest.setAge( 0 );

    session_->send( diagLogRequest );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::translatePrintAuditButton()
{
    printAuditLog_->setText( printingAuditReport_ ? tr( "CANCEL PRINTING" ) : tr( "Print" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::translatePrintMaintLogButton()
{
    printMaintLog_->setText( printingDiagReport_ ? tr( "CANCEL PRINTING" ) : tr( "Print" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::onPushButtonClicked()
{
    LOG_CLASS_INFO( "serviceHealthAgent", "onPushButtonClicked" );

    QPushButton *pushButton( reinterpret_cast<QPushButton*>( sender() ) );

    if ( !pushButton )
        return;

    // diagnostics
    if ( pushButton->objectName().startsWith( "diag_" ) )
    {
        int index( pushButton->objectName().mid( 5 ).toInt() );

        LOG_INFO( "diag button '%s' pressed", qPrintable( pushButton->objectName() ) );

        if (( 0 <= index ) && ( index < diagTable_->topLevelItemCount() ))
        {
            QTreeWidgetItem *item( diagTable_->topLevelItem( index ) );

            // diagnose
            if (( item ) && ( DiagId <= item->columnCount() ))
            {
                LOG_INFO( "running diagnostic wizard for maint record %s", qPrintable( item->text( DiagId ) ) );

                DiagStartErrorHandlerFromMaintRecordMsg msg;
                msg.setMaintRecord( item->text( DiagId ) );
                msg.setServiceMode( serviceMode_ );

                // run
                session_->send( msg );
            }
        }
    }

    // clear log
    else if ( pushButton->objectName().startsWith( "clear_" ) )
    {
        int index( pushButton->objectName().mid( 6 ).toInt() );

        LOG_INFO( "clear button '%s' pressed", qPrintable( pushButton->objectName() ) );

        if (( 0 <= index ) && ( index < diagTable_->topLevelItemCount() ))
        {
            QTreeWidgetItem *item( diagTable_->topLevelItem( index ) );

            // request record
            if (( item ) && ( DiagErrorCode <= item->columnCount() ))
            {
                int errorCode( item->text( DiagErrorCode ).toInt() );

                LOG_INFO( "prompting for resolution of maint record %s %d", qPrintable( item->text( DiagId ) ), errorCode );

                DiagnosticResolutionDialog d( errorCode, mainWindow_ );

                // pop dialog
                if ( QDialog::Accepted == d.exec() )
                {
                    DiagClearServiceMaintRecordMsg msg;
                    msg.setMaintRecord( item->text( DiagId ) );
                    msg.setReason( DiagClearServiceMaintRecordMsg::ResolvedByService );
                    msg.setServiceMethod1( d.serviceMethod( 0 ) );
                    msg.setServicePart1( d.servicePart( 0 ) );
                    msg.setServiceMethod2( d.serviceMethod( 1 ) );
                    msg.setServicePart2( d.servicePart( 1 ) );
                    msg.setServiceMethod3( d.serviceMethod( 2 ) );
                    msg.setServicePart3( d.servicePart( 2 ) );
                    msg.setServiceMethod4( d.serviceMethod( 3 ) );
                    msg.setServicePart4( d.servicePart( 3 ) );
                    msg.setServiceNotes( d.serviceNotes() );

                    // clear record
                    session_->send( msg );

                    // request diagnostic log
                    requestMaintLog();
                }
            }
        }
    }

    // print maint log
    else if ( printMaintLog_ == pushButton )
    {
        LOG_INFO( "print service maint records button pressed", NULL );

        if ( printingDiagReport_ )
            cancelPrinting();
        else if ( !printing_ )
        {
            DiagPrintServiceMaintLogMsg diagPrintLog;
            diagPrintLog.setFilter( DiagPrintServiceMaintLogMsg::OpenRecordsOnly );
            diagPrintLog.setAge( 0 );

            session_->send( diagPrintLog );

            printing_ = printingDiagReport_ = true;
            translatePrintMaintLogButton();
        }
    }

    // export log
    else if ( exportDiagLog_ == pushButton )
    {
        QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb/" );
        folders.push_back( "/media/thumb1/" );
        folders.push_back( "/media/thumb2/" );
        folders.push_back( "/media/thumb3/" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        QString startDirStr;

        foreach ( const QString& f, folders )
            if ( QDir( startDirStr = f ).exists() )
                break;

        QStringList filters;
        filters.append( "Database Files (*.db)" );
        filters.append( "All Files (*)" );

        FileDialog dialog( mainWindow_ );
        dialog.setAcceptMode( QFileDialog::AcceptSave );
        dialog.setDirectory( startDirStr );
        dialog.setFileMode( QFileDialog::AnyFile );
        dialog.setNameFilters( filters );
        dialog.setOptions( QFileDialog::DontConfirmOverwrite );
        dialog.setWindowTitle( tr( "Export" ) );

        LOG_INFO( "prompting for export location... starting folder '%s'", qPrintable( startDirStr ) );

        if ( QDialog::Accepted != dialog.exec() )
            LOG_INFO( "export cancelled", NULL );
        else
        {
            QStringList selected( dialog.selectedFiles() );

            if ( 1 != selected.count() )
                session_->send( Error, ErrDiagnostics, "173100,5" );
            else
            {
                QString file( selected[0] );

                LOG_INFO( "file '%s' was selected", qPrintable( file ) );

                if ( !file.length() )
                    session_->send( Error, ErrDiagnostics, "173101,5" );
                else if ( "/" == file.right( 1 ) )
                    session_->send( Error, ErrDiagnostics, "173102,5" );
                else if ( 0 <= file.indexOf( ";" ) )
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if ( 0 <= file.indexOf( "*" ) )
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if ( 0 <= file.indexOf( "?" ) )
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if (( 0 <= file.indexOf( "\"" ) ) || ( 0 <= file.indexOf( "\'" ) ))
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if (( 0 <= file.indexOf( "<" ) ) || ( 0 <= file.indexOf( ">" ) ))
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if (( 0 <= file.indexOf( "+" ) ) ||( 0 <= file.indexOf( "-" ) ))
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else if ( 0 <= file.indexOf( "|" ) )
                    session_->send( Error, ErrDiagnostics, "173103,5" );
                else
                {
#if defined( Q_OS_WIN )
                    // change the file separator slashes
                    file.replace( "/", "\\\\" );
#endif

                    LOG_INFO( "exporting database to '%s'", qPrintable( file ) );
                    session_->send( Diagnostics, DiagExportDatabase, file );

                    exportDiagLog_->setEnabled( false );
                }
            }
        }
    }

    // email log
    else if ( emailDiagLog_ == pushButton )
    {
        LOG_INFO( "email diag log button pressed", NULL );
        session_->send( Diagnostics, DiagSendDatabaseEmail );

        emailDiagLog_->setEnabled( false );
    }

    // upload log
    else if ( uploadDiagLog_ == pushButton )
    {
        LOG_INFO( "upload diag log button pressed", NULL );
        session_->send( Diagnostics, DiagSendDatabaseGatewayDevice );

        uploadDiagLog_->setEnabled( false );
    }

    // previous audit log
    else if ( prevAuditLog_ == pushButton )
    {
        QList<QTreeWidgetItem*> selected( auditTable_->selectedItems() );

        if ( 1 == selected.size() )
        {
            QTreeWidgetItem *item( auditTable_->itemAbove( selected[0] ) );

            if ( item )
            {
                selected[0]->setSelected( false );
                item->setSelected( true );
            }
        }
    }

    // previous audit log
    else if ( nextAuditLog_ == pushButton )
    {
        QList<QTreeWidgetItem*> selected( auditTable_->selectedItems() );

        if ( 1 == selected.size() )
        {
            QTreeWidgetItem *item( auditTable_->itemBelow( selected[0] ) );

            if ( item )
            {
                selected[0]->setSelected( false );
                item->setSelected( true );
            }
        }
    }

    // start service visit
    else if ( startServiceVisit_ == pushButton )
    {
        LOG_INFO( "starting service visit", NULL );
        session_->send( Diagnostics, DiagAuditLogServiceVisitStart, "\"\"" );

        startServiceVisit_->setEnabled( false );
        stopServiceVisit_->setEnabled( false );

        requestAuditLogs();

        // request service visit status
        session_->send( Diagnostics, DiagAuditLogServiceVisitStatus );
    }

    // stop service visit
    else if ( stopServiceVisit_ == pushButton )
    {
        QString details;

        if ( session_->isPrepack() )
        {
            // we do not have all the info we need
            if (( trays_.isEmpty() ) ||
                ( !singleRoll_ && !doubleRoll_ ) ||
                ( !upperRollWidth_ && !lowerRollWidth_ ))
            {
                MessageBoxDialog dialog( mainWindow_ );

                dialog.setText( tr( "Please turn on the wrapper and try again." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();

                requestServiceSettings();
                return;
            }

            // prompt audit dialog
            WrapperAuditDialog auditDialog( trays_, doubleRoll_, upperRollWidth_, lowerRollWidth_, mainWindow_ );

            if ( QDialog::Accepted != auditDialog.exec() )
                return;

            details = auditDialog.report();
        }
        else
        {
            // prompt audit dialog
            ScaleAuditDialog auditDialog( mainWindow_ );

            if ( QDialog::Accepted != auditDialog.exec() )
                return;

            details = auditDialog.report();
        }

        details.replace( "\"", "\"\"" );

        LOG_INFO( "stopping service visit", NULL );
        session_->send( Diagnostics, DiagAuditLogServiceVisitStop, "\"" + details + "\"" );

        startServiceVisit_->setEnabled( false );
        stopServiceVisit_->setEnabled( false );

        requestAuditLogs();

        // request service visit status
        session_->send( Diagnostics, DiagAuditLogServiceVisitStatus );
    }

    // print audit log
    else if ( printAuditLog_ == pushButton )
    {
        LOG_INFO( "print audit record button pressed", NULL );

        QList<QTreeWidgetItem*> selected( auditTable_->selectedItems() );

        if ( printingAuditReport_ )
            cancelPrinting();
        else if (( !printing_ ) && ( 1 == selected.size() ))
        {
            QString title( "AUDIT REPORT" );

            QString msg;

            msg += "Date:  ";
            msg += selected[0]->text( 1 );
            msg += "\n";

            msg += "Severity:  ";
            msg += selected[0]->text( 2 );
            msg += "\n";

            msg += "\n";

            msg += "Source:  ";
            msg += selected[0]->text( 3 );
            msg += "\n";

            msg += "Description:  ";
            msg += selected[0]->text( 4 );
            msg += "\n";

            msg += "\n";

            msg += auditLogDetails_->toPlainText();

            session_->send( ReportManagerDev, StartReport, "2,0,0,0,0,\"" + title + "\",\"" + msg + "\"" );

            printing_ = printingAuditReport_ = true;
            translatePrintAuditButton();
        }
    }

    // export health agent and system logs
    else if( exportLog_ == pushButton )
    {
        QStringList folders;
        QString folder;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/thumb1" );
        folders.push_back( "/media/thumb2" );
        folders.push_back( "/tmp/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        foreach ( const QString& f, folders )
            if ( QDir( f ).exists() )
            {
                folder = f;
                break;
            }

        if ( folder.isEmpty() )
        {
            MessageBoxDialog dialog( mainWindow_ );
            dialog.setTitle( tr( "Error" ) );
            dialog.setText( tr( "No thumb drive was found." ) );
            dialog.setStandardButtons( QDialogButtonBox::Ok );

            dialog.exec();

            return;
        }

        LOG_INFO("USB Found: %s", qPrintable(folder));

        // save logs to /usr/local/hobart and run log collector
        QString logPlainText = log_->toPlainText();

        QFile healthAgentLog( "/usr/local/hobart/healthAgent.log" );

        if ( healthAgentLog.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream os(&healthAgentLog);
            os << logPlainText;

            healthAgentLog.close();
        }
        else
        {
            LOG_INFO("Failed to create healthAgent.log", NULL);
        }

        QProcess p;
        QStringList params;
        QString pythonScript = "/usr/local/hobart/python/dumpState.py";

        params << pythonScript << "-F" << "/usr/local/hobart/dumpState.log";
        p.start("python", params);
        p.waitForFinished();
        params.clear();

        pythonScript = "/usr/local/hobart/python/logCollector.py";
        params << pythonScript << "-d" << folder;
        p.start("python", params);
        p.waitForFinished();

        params << "";
        p.start("sync", params);
        p.waitForFinished();

        // remove newly created files
        QFile dumpStateLog("/usr/local/hobart/dumpState.log");
        dumpStateLog.remove();
        healthAgentLog.remove();

        // show save complete message
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "Save Complete" ) );
        dialog.setText( tr( "All logs have been saved to thumb.\n\nPlease use the eject button before removing." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::onTimerTimeout()
{
    if ( requestDiagStatus_ == sender() )
        requestDiagStatus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::onTreeSelectionChanged()
{
    if ( auditTable_ == sender() )
    {
        QList<QTreeWidgetItem*> selected( auditTable_->selectedItems() );

        printAuditLog_->setEnabled( false );
        auditLogDetails_->clear();

        if ( 1 != selected.size() )
        {
            prevAuditLog_->setEnabled( false );
            nextAuditLog_->setEnabled( false );

            auditLogDetails_->setVisible( false );
        }
        else
        {
            const int index( auditTable_->indexOfTopLevelItem( selected[0] ) );

            prevAuditLog_->setEnabled( 0 < index );
            nextAuditLog_->setEnabled( index < (auditTable_->topLevelItemCount() - 1) );

            auditLogDetails_->setVisible( true );

            // request details
            session_->send( Diagnostics, DiagAuditLogRecordReq, selected[0]->text( 0 ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::clearLog()
{
    static const QString logFileName( "info.log" );

    // clear existing log
    log_->clear();
    emptyLog_ = true;

    // remove log
    QFile file( filesystem::absoluteFilePath( logFileName, folderType::RootFolder) );
    file.remove();

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::refreshLog()
{
    static const QString logFileName( "info.log" );

    // clear existing log
    log_->clear();
    emptyLog_ = true;

    // load log
    QFile file( filesystem::absoluteFilePath( logFileName, folderType::RootFolder) );

    if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        while ( !file.atEnd() )
        {
            QByteArray buffer( file.readLine() );

            // Need to strip off trailing newline characters since appendPlainText() adds a newline
            if ( buffer.endsWith( '\n' ) )
                buffer.chop( 1 );

            log_->appendPlainText( buffer );
            emptyLog_ = false;
        }

        file.close();
    }

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::initialize()
{
    modifyFontPointSize setFontPointSize10( 10 );
    modifyFontPointSize setFontPointSize16( 16 );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    // diagnostic status timer
    requestDiagStatus_ = new QTimer( this );
    requestDiagStatus_->setObjectName( "requestDiagStatus" );
    requestDiagStatus_->setSingleShot( false );

    connect( requestDiagStatus_, SIGNAL(timeout()), SLOT(onTimerTimeout()) );

    requestDiagStatus_->start( 10000 );

    //
    // diag maintenance log
    //

    diagMaintLog_ = new QWidget( this );
    diagMaintLog_->setObjectName( "diagMaintLog" );

    // diag table
    diagTable_ = new QTreeWidget( diagMaintLog_ );
    diagTable_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    diagTable_->setObjectName( "diagTable" );
    diagTable_->setRootIsDecorated( false );
    diagTable_->setSelectionMode( QAbstractItemView::NoSelection );
    diagTable_->setUniformRowHeights( true );

    diagTable_->installEventFilter( this );

    diagTableFont_ = font();
    diagTableFont_.setPointSize( 12 );

    // print
    printMaintLog_ = new QPushButton( diagMaintLog_ );
    printMaintLog_->setEnabled( mainWindow_->isPrimarySessionType() );
    printMaintLog_->setObjectName( "printMaintLog" );

    connect( printMaintLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // export diag
    exportDiagLog_ = new QPushButton( diagMaintLog_ );
    exportDiagLog_->setEnabled( mainWindow_->isPrimarySessionType() );
    exportDiagLog_->setObjectName( "exportDiagLog" );

    connect( exportDiagLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // email diag
    emailDiagLog_ = new QPushButton( diagMaintLog_ );
    emailDiagLog_->setObjectName( "emailDiagLog" );
    emailDiagLog_->setVisible( false );

    connect( emailDiagLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // upload diag
    uploadDiagLog_ = new QPushButton( diagMaintLog_ );
    uploadDiagLog_->setObjectName( "uploadDiagLog" );
    uploadDiagLog_->setVisible( false );

    connect( uploadDiagLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    //
    // diag config
    //

    diagConfig_ = new QWidget( this );
    diagConfig_->setObjectName( "diagConfig" );

    // service contact label
    serviceContactLabel_ = new QLabel( diagConfig_ );
    serviceContactLabel_->setEnabled( serviceMode_ );
    serviceContactLabel_->setObjectName( "serviceContactLabel" );

    // service contact label
    serviceContact_ = new QLineEdit( diagConfig_ );
    serviceContact_->setEnabled( serviceMode_ );
    serviceContact_->setObjectName( "serviceContact" );

    // gateway address label
    gatewayAddressLabel_ = new QLabel( diagConfig_ );
    gatewayAddressLabel_->setObjectName( "gatewayAddressLabel" );

    // gateway address
    gatewayAddress_ = new QLineEdit( diagConfig_ );
    gatewayAddress_->setObjectName( "gatewayAddress" );

    // gateway log level label
    gatewayLogLevelLabel_ = new QLabel( diagConfig_ );
    gatewayLogLevelLabel_->setObjectName( "gatewayLogLevelLabel" );

    // gateway log level
    gatewayLogLevel_ = new QComboBox( diagConfig_ );
    gatewayLogLevel_->setFocusPolicy( Qt::NoFocus );
    gatewayLogLevel_->setObjectName( "gatewayLogLevel" );

    gatewayLogLevel_->addItem( "", 0 );
    gatewayLogLevel_->addItem( "", 1 );
    gatewayLogLevel_->addItem( "", 2 );
    gatewayLogLevel_->addItem( "", 3 );
    gatewayLogLevel_->addItem( "", 4 );

    // disable valuemax diag collection
    disableValueMaxDiagCollection_ = new QCheckBox( diagConfig_ );
    disableValueMaxDiagCollection_->setEnabled( serviceMode_ );
    disableValueMaxDiagCollection_->setObjectName( "disableValueMaxDiagCollection" );
    disableValueMaxDiagCollection_->setVisible( valueMaxWeigher_ );

    // gateway status
    gatewayStatus_ = new QLabel( diagConfig_ );
    gatewayStatus_->setObjectName( "gatewayStatus" );

    //
    // health log
    //

    healthLog_ = new QWidget( this );
    healthLog_->setObjectName( "healthLog" );

    // heading
    heading_ = new QLabel( healthLog_ );
    heading_->setObjectName( "heading" );
    heading_->setAlignment( Qt::AlignCenter );

    setFontPointSize16( heading_ );

    // clear log
    clearLog_ = new QPushButton( healthLog_ );
    clearLog_->setEnabled( serviceMode_ && mainWindow_->isPrimarySessionType() );
    clearLog_->setObjectName( "clearLog" );

    setFontPointSize16( clearLog_ );

    connect( clearLog_, SIGNAL(clicked()), SLOT(clearLog()) );

    // export log
    exportLog_ = new QPushButton( healthLog_ );
    exportLog_->setEnabled( serviceMode_ && mainWindow_->isPrimarySessionType() );
    exportLog_->setObjectName( "exportLog" );

    setFontPointSize16( exportLog_ );

    connect( exportLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // log
    log_ = new QPlainTextEdit( healthLog_ );
    log_->setLineWrapMode( QPlainTextEdit::NoWrap );
    log_->setFocusPolicy( Qt::NoFocus ); // FT-791
    log_->setObjectName( "log" );
    log_->setReadOnly( true );

    setFontPointSize10( log_ );

    //
    // audit log
    //

    auditLog_ = new QWidget( this );
    auditLog_->setObjectName( "auditLog" );

    // audit log table
    auditTable_ = new QTreeWidget( auditLog_ );
    auditTable_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    auditTable_->setObjectName( "auditTable" );
    auditTable_->setRootIsDecorated( false );
    auditTable_->setSelectionMode( QAbstractItemView::SingleSelection );
    auditTable_->setUniformRowHeights( true );

    auditTable_->installEventFilter( this );

    setHighlight( auditTable_ );
    setHighlightText( auditTable_ );

    auditTableFont_ = font();
    auditTableFont_.setPointSize( 12 );

    connect( auditTable_, SIGNAL(itemSelectionChanged()), SLOT(onTreeSelectionChanged()) );

    // audit log details
    auditLogDetails_ = new QPlainTextEdit( auditLog_ );
    auditLogDetails_->setFocusPolicy( Qt::NoFocus );
    auditLogDetails_->setLineWrapMode( QPlainTextEdit::NoWrap );
    auditLogDetails_->setObjectName( "auditLogDetails" );
    auditLogDetails_->setReadOnly( true );
    auditLogDetails_->setVisible( false );

    // prev audit log
    prevAuditLog_ = new QPushButton( auditLog_ );
    prevAuditLog_->setEnabled( false );
    prevAuditLog_->setObjectName( "prevAuditLog" );

    connect( prevAuditLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // next audit log
    nextAuditLog_ = new QPushButton( auditLog_ );
    nextAuditLog_->setEnabled( false );
    nextAuditLog_->setObjectName( "nextAuditLog" );

    connect( nextAuditLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // start service visit
    startServiceVisit_ = new QPushButton( auditLog_ );
    startServiceVisit_->setEnabled( false );
    startServiceVisit_->setObjectName( "startServiceVisit" );

    connect( startServiceVisit_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // stop service visit
    stopServiceVisit_ = new QPushButton( auditLog_ );
    stopServiceVisit_->setEnabled( false );
    stopServiceVisit_->setObjectName( "stopServiceVisit" );

    connect( stopServiceVisit_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    // print audit log
    printAuditLog_ = new QPushButton( auditLog_ );
    printAuditLog_->setEnabled( false );
    printAuditLog_->setObjectName( "printAuditLog" );

    connect( printAuditLog_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    //
    // screen
    //

    tab_ = new QTabWidget( this );
    tab_->setObjectName( "tab" );

    diagMaintLogTab_ = tab_->addTab( diagMaintLog_, QString() );
    diagConfigTab_ = tab_->addTab( diagConfig_, QString() );
    healthLogTab_ = tab_->addTab( healthLog_, QString() );
    auditLogTab_ = tab_->addTab( auditLog_, QString() );

    // disable tab(s) when not service
    tab_->setTabEnabled( healthLogTab_, serviceMode_ );
    tab_->setTabEnabled( auditLogTab_, serviceMode_ );

    // refresh
    refreshLog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::createLayout()
{
    //
    // diag maintenance log
    //

    QHBoxLayout *maintLogButtons( new QHBoxLayout() );
    maintLogButtons->addStretch();
    maintLogButtons->addWidget( uploadDiagLog_ );
    maintLogButtons->addWidget( emailDiagLog_ );
    maintLogButtons->addWidget( exportDiagLog_ );
    maintLogButtons->addWidget( printMaintLog_ );

    QVBoxLayout *diagMaintLog( new QVBoxLayout( diagMaintLog_ ) );
    diagMaintLog->addWidget( diagTable_, 1 );
    diagMaintLog->addLayout( maintLogButtons );

    //
    // diag config
    //

    QFormLayout *diagConfigForm( new QFormLayout() );
    diagConfigForm->addRow( serviceContactLabel_, serviceContact_ );
    diagConfigForm->addItem( new QSpacerItem( 16, 16 ) );
    diagConfigForm->addRow( gatewayAddressLabel_, gatewayAddress_ );
    diagConfigForm->addRow( gatewayLogLevelLabel_, gatewayLogLevel_ );
    diagConfigForm->addItem( new QSpacerItem( 16, 16 ) );
    diagConfigForm->addRow( disableValueMaxDiagCollection_ );

    QVBoxLayout *diagConfig( new QVBoxLayout( diagConfig_ ) );
    diagConfig->addLayout( diagConfigForm );
    diagConfig->addStretch();
    diagConfig->addWidget( gatewayStatus_ );

    //
    // health log
    //

    QHBoxLayout *heading( new QHBoxLayout() );
    heading->addWidget( heading_, 1 );
    heading->addWidget( clearLog_ );
    heading->addWidget( exportLog_ );

    QVBoxLayout *healthLog( new QVBoxLayout( healthLog_ ) );
    healthLog->addLayout( heading );
    healthLog->addWidget( log_ );

    //
    // audit log
    //

    QHBoxLayout *auditLogButtons( new QHBoxLayout() );
    auditLogButtons->addWidget( prevAuditLog_ );
    auditLogButtons->addWidget( nextAuditLog_ );
    auditLogButtons->addStretch();
    auditLogButtons->addWidget( startServiceVisit_ );
    auditLogButtons->addWidget( stopServiceVisit_ );
    auditLogButtons->addStretch();
    auditLogButtons->addWidget( printAuditLog_ );

    QVBoxLayout *auditLog( new QVBoxLayout( auditLog_ ) );
    auditLog->addWidget( auditTable_, 1 );
    auditLog->addWidget( auditLogDetails_, 1 );
    auditLog->addLayout( auditLogButtons );

    //
    // screen
    //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 50, 8, 50, 8 ) );

    screenLayout->addWidget( tab_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::calcDiagMaintTableColumnWidths()
{
    const int tableWidth( diagTable_->width() - 34 );

    diagTable_->setColumnHidden( DiagId, true );
    diagTable_->setColumnWidth( DiagDate, (tableWidth * 20) / 100 ); // date
    diagTable_->setColumnWidth( DiagErrorCode, (tableWidth * 20) / 100 ); // diagnostic code
    diagTable_->setColumnWidth( DiagOccurrences, (tableWidth * 20) / 100 ); // occurrences
    diagTable_->setColumnWidth( DiagLastOccurrence, (tableWidth * 20) / 100 ); // last occurrence
    diagTable_->setColumnWidth( DiagClearButton, (tableWidth * 10) / 100 ); // clear
    diagTable_->setColumnWidth( DiagDiagnoseButton, (tableWidth * 10) / 100 ); // diagnose
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::calcDiagMaintTableFontSizes( QTreeWidgetItem *item ) const
{
    item->setFont( DiagId, diagTableFont_ ); // id
    item->setFont( DiagDate, diagTableFont_ ); // date
    item->setFont( DiagErrorCode, diagTableFont_ ); // diagnostic code
    item->setFont( DiagOccurrences, diagTableFont_ ); // occurrences
    item->setFont( DiagLastOccurrence, diagTableFont_ ); // last occurrence
    item->setFont( DiagClearButton, diagTableFont_ ); // clear
    item->setFont( DiagDiagnoseButton, diagTableFont_ ); // diagnose
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::updateDiagConfig()
{
    // nothing to do
    if ( !configs_ )
        return;

    DiagConfigurationMsg m;
    m.setServiceContactInfo( serviceContact_->text() );
    m.setGatewayDeviceAddress( gatewayAddress_->text() );
    m.setGatewayDeviceLogLevel( gatewayLogLevel_->currentIndex() );
    m.setDisableValueMaxDiagCollection( disableValueMaxDiagCollection_->isChecked() );

    session_->send( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::calcAuditTableColumnWidths()
{
    const int tableWidth( auditTable_->width() - 34 );

    auditTable_->setColumnHidden( AuditId, true );
    auditTable_->setColumnWidth( AuditDate, (tableWidth * 20) / 100 ); // date
    auditTable_->setColumnWidth( AuditSeverity, (tableWidth * 10) / 100 ); // severity
    auditTable_->setColumnWidth( AuditSource, (tableWidth * 20) / 100 ); // source
    auditTable_->setColumnWidth( AuditSourceExt, (tableWidth * 50) / 100 ); // source extended
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::calcAuditTableFontSizes( QTreeWidgetItem *item ) const
{
    item->setFont( AuditId, diagTableFont_ ); // id
    item->setFont( AuditDate, diagTableFont_ ); // date
    item->setFont( AuditSeverity, diagTableFont_ ); // severity
    item->setFont( AuditSource, diagTableFont_ ); // source
    item->setFont( AuditSourceExt, diagTableFont_ ); // source extended
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceHealthAgent::requestServiceSettings()
{
    if ( !serviceMode_ )
        return;

    // wrapper
    if ( session_->isPrepack() )
    {
        upperRollWidth_ = lowerRollWidth_ = 0;
        singleRoll_ = doubleRoll_ = false;

        // request settings
        session_->send( Wrapper, WrReqEvolSystemInfo );
        session_->send( Wrapper, WrReqInquire );

        // request timings
        session_->send( Wrapper, WrEvolLoadPackageTimings, "1" );
    }
}
