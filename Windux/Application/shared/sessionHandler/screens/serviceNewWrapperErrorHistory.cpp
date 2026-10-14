/**
 * @file serviceNewWrapperErrorHistory.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperErrorHistory.h"

#include "../sessionmainwindow.h"

#include "../dialogs/filedialog.h"
#include "../dialogs/messageboxdialog.h"

#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFile>
#include <QHBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QSignalMapper>
#include <QStackedLayout>
#include <QString>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <QtGui>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperErrorHistory::serviceNewWrapperErrorHistory( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "serviceNewWrapperErrorHistory" );

    // initialize
    initialize();
    createLayout();
    translate();

    //fakeData();

    // retrieve error history
    session_->send( Wrapper, WrEvolReqErrorHistory );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperErrorHistory::~serviceNewWrapperErrorHistory()
{
    // delete error blocks
    while ( errors_.size() )
    {
        delete errors_.front();
        errors_.pop_front();
    }

    while ( errorsList_.size() )
    {
        delete errorsList_.front();
        errorsList_.pop_front();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::translate()
{
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Error" ) );
    tableHeaders.push_back( QString( "%1 /\n%2" ).arg( tr( "Tray" ) ).arg( tr( "Dimension" ) ) );
    tableHeaders.push_back( QString( "%1 /\n%2" ).arg( tr( "Best Fit" ) ).arg( tr( "% Stretch" ) ) );
    tableHeaders.push_back( QString( "%1 /\n%2" ).arg( tr( "Date" ) ).arg( tr( "Time" ) ) );
    tableHeaders.push_back( "" );

    table_->setHeaderLabels( tableHeaders );
    table_->setColumnCount( tableHeaders.size() );

    calcTableColumnWidths();
    calcTableFontSizes( table_->headerItem() );

    clearHistory_->setText( tr( "Clear History" ) );
    saveErrorHistory_->setText( tr( "Save to USB" ) );

    back_->setText( tr( "Back" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // resize columns
    calcTableColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::insertSchematics( QTextEdit *edit ) const
{
    // do we need to insert diagrams into this help?
    if ( !edit->find( "%SCHEMATICS%", QTextDocument::FindCaseSensitively ) )
        return;

    // make mapping of translation strings
    QMap<QString, QString> translations;
    translations["textComboBoards"] = tr( "Combo boards" );
    translations["textBLDCBoards"] = tr( "BLDC boards" );
    translations["textAboutComboFuses"] = tr( "Fuse F4 (1.5A) on all nodes monitor 100VDC to all BV terminals on that node." );
    translations["textAboutBLDCFuses"] = tr( "Fuses F2 (1.5A) and  F4 (1.5A) on all nodes monitor 100VDC to all BV terminals on that node." );
    translations["textBrakeAndAirValves"] = tr( "NOTE: Brake = about 564 ohms. Air valves = about 1200 ohms." );
    translations["textNode1"] = tr( "NODE 1" );
    translations["textNode2"] = tr( "NODE 2" );
    translations["textNode3"] = tr( "NODE 3" );
    translations["textNode4"] = tr( "NODE 4" );
    translations["textNode5"] = tr( "NODE 5" );
    translations["textMotor1"] = tr( "Motor 1" );
    translations["textMotor2"] = tr( "Motor 2" );
    translations["textPrimaryIntake"] = tr( "Primary intake" );
    translations["textGripper"] = tr( "Gripper" );
    translations["textFrontUnderfolder"] = tr( "Front underfolder" );
    translations["textRearUnderfolder"] = tr( "Rear underfolder" );
    translations["textTransferStation"] = tr( "Transfer station" );
    translations["text220W"] = tr( "220W" );
    translations["text440W"] = tr( "440W" );
    translations["text660W"] = tr( "660W" );
    translations["textFuse"] = tr( "Fuse" );
    translations["textF1_0_5A"] = tr( "F1, 0.5A" );
    translations["textF2_0_5A"] = tr( "F2, 0.5A" );
    translations["textF3_0_5A"] = tr( "F3, 0.5A" );
    translations["textF5_0_5A"] = tr( "F5, 0.5A" );
    translations["textF6_0_5A"] = tr( "F6, 0.5A" );
    translations["textF7_0_5A"] = tr( "F7, 0.5A" );
    translations["textF8_0_5A"] = tr( "F8, 0.5A" );
    translations["textF1_2A"] = tr( "F1, 2A" );
    translations["textF3_2A"] = tr( "F3, 2A" );
    translations["textF5_2A"] = tr( "F5, 2A" );
    translations["textF6_2A"] = tr( "F6, 2A" );
    translations["textF1_3_15A"] = tr( "F1, 3.15A" );
    translations["textF3_3_15A"] = tr( "F3, 3.15A" );
    translations["textF5_3_15A"] = tr( "F5, 3.15A" );
    translations["textF6_3_15A"] = tr( "F6, 3.15A" );
    translations["textF1_4A"] = tr( "F1, 4A" );
    translations["textF6_4A"] = tr( "F6, 4A" );
    translations["textHome"] = tr( "Home" );
    translations["textJ7"] = tr( "J7" );
    translations["textJ12"] = tr( "J12" );
    translations["textJ13"] = tr( "J13" );
    translations["textJ14"] = tr( "J14" );
    translations["textJ15"] = tr( "J15" );
    translations["textJ18"] = tr( "J18" );
    translations["textJ19"] = tr( "J19" );
    translations["textJ22"] = tr( "J22" );
    translations["textJ23"] = tr( "J23" );
    translations["textBV1"] = tr( "BV1" );
    translations["textBV2"] = tr( "BV2" );
    translations["textBV3"] = tr( "BV3" );
    translations["textBV4"] = tr( "BV4" );
    translations["textElevator"] = tr( "Elevator" );
    translations["textSideClamp"] = tr( "Side clamp" );
    translations["textUpperPusher"] = tr( "Upper pusher" );
    translations["textSecondaryIntake"] = tr( "Secondary intake" );
    translations["textSideUnderfolder"] = tr( "Side underfolder" );
    translations["textGripperNarrowValveRS"] = tr( "Gripper narrow valve, right side" );
    translations["textUpperFilmBrake"] = tr( "Upper film brake" );
    translations["textElevatorWideValveRS"] = tr( "Elevator wide valve, right side" );
    translations["textLabelApplierValve"] = tr( "Label applier valve" );
    translations["textKnifeValveLS"] = tr( "Knife valve, left side" );
    translations["textGripperWideValveRS"] = tr( "Gripper wide valve, right side" );
    translations["textLowerFilmBrake"] = tr( "Lower film brake" );
    translations["textLabelWandValve"] = tr( "Label wand valve" );
    translations["textSideClampValveRS"] = tr( "Side clamp valve, right side" );
    translations["textFilmRollSelectValveLS"] = tr( "Film roll select valve, left side" );
    translations["textLabelSuctionValve"] = tr( "Label suction valve" );
    translations["textPrimaryIntakeHS"] = tr( "Primary intake home sensor" );
    translations["textGripperHS"] = tr( "Gripper home sensor" );
    translations["textFrontUnderfolderHS"] = tr( "Front underfolder home sensor" );
    translations["textRearUnderfolderHS"] = tr( "Rear underfolder home sensor" );
    translations["textLabelRotationHS"] = tr( "Label rotation home sensor" );
    translations["textElevatorHS"] = tr( "Elevator home sensor" );
    translations["textSideClampHS"] = tr( "Side clamp home sensor" );
    translations["textPusherHS"] = tr( "Pusher home sensor" );
    translations["textShiftHS"] = tr( "Shift home sensor" );
    translations["textSideUnderfolderHS"] = tr( "Side underfolder home sensor" );
    translations["textStartSwitch"] = tr( "Start switch" );
    translations["textShiftLeftSensor"] = tr( "Shift left sensor" );
    translations["textKnifeHeightSensor"] = tr( "Knife height sensor" );
    translations["textFilmDetect"] = tr( "Film detect" );
    translations["textPackageOnElevatorSensor"] = tr( "Package on elevator sensor" );
    translations["textTransferStationSensor"] = tr( "Transfer station sensor" );
    translations["textInterlockSense1"] = tr( "Interlock sense 1" );
    translations["textInterlockSense2"] = tr( "Interlock sense 2" );
    translations["textLEDOnIndicatesClosed"] = tr( "LED on indicates closed." );
    translations["textD9"] = tr( "D9" );
    translations["textD12"] = tr( "D12" );
    translations["textD14"] = tr( "D14" );
    translations["textD16"] = tr( "D16" );
    translations["textD20"] = tr( "D20" );
    translations["textD21"] = tr( "D21" );
    translations["textD26"] = tr( "D26" );
    translations["textD27"] = tr( "D27" );
    translations["textD28"] = tr( "D28" );
    translations["textD29"] = tr( "D29" );
    translations["textD30"] = tr( "D30" );
    translations["textD31"] = tr( "D31" );
    translations["textForComboBoards"] = tr( "For boards after Serial Number 45 1066 373, view the" );
    translations["textForBLDCBoards"] = tr( "For boards prior to Serial Number 45 1066 374, view the " );
    translations["textBLDCBoardsChart"] = tr( "BLDC boards chart" );
    translations["textComboBoardsChart"] = tr( "Combo boards chart" );
    translations["textPower"] = tr( "Power" );
    translations["textEncoder"] = tr( "Encoder" );

    // insert the data
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

    if ( evolWrapperSystemInformation->isClosedLoop() )
        insertHtml( edit, ":/html/Prepack/driverBoardInfoChartNGW.html", translations );
    else
        insertHtml( edit, ":/html/Prepack/driverBoardInfoChart.html", translations );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::insertLimits( QTextEdit *edit ) const
{
    // do we need to insert limits into this help?
    if ( !edit->find( "%LIMITS%", QTextDocument::FindCaseSensitively ) )
        return;

    // make mapping of translation strings
    QMap<QString, QString> translations;
    translations["textMinimum"] = tr( "Minimum" );
    translations["textMaximum"] = tr( "Maximum" );
    translations["textLength"] = tr( "Length" );
    translations["textWidth"] = tr( "Width" );
    translations["textLip"] = tr( "Lip" );
    translations["textPercentNetStretch"] = tr( "Percent Net Stretch" );
    translations["textKnifeFireHeight"] = tr( "Knife Fire Height" );
    translations["textSideClampOpenHeight"] = tr( "Side Clamp Open Height" );
    translations["textDistance"] = tr( "Distance" );
    translations["textGripperPullAdjust"] = tr( "Gripper Pull Adjust" );
    translations["textInsideGripperRelease"] = tr( "Inside Gripper Release" );
    translations["textFrontFolderAdjust"] = tr( "Front Folder Adjust" );
    translations["textWrapSpeed"] = tr( "Wrap Speed" );
    translations["textDischargeSpeed"] = tr( "Discharge Speed" );
    translations["textSideUnderfolderAdjust"] = tr( "Side Underfolder Adjust" );
    translations["textRearUnderfolderTripDelay"] = tr( "Rear Underfolder Trip Delay" );
    translations["textBrake"] = tr( "Brake" );
    translations["textHeightRestriction"] = tr( "Height Restriction" );

    // insert the data
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

    if ( evolWrapperSystemInformation->isClosedLoop() )
        insertHtml( edit, ":/html/Prepack/driverBoardLimitsNGW.html", translations );
    else
        insertHtml( edit, ":/html/Prepack/driverBoardLimits.html", translations );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceNewWrapperErrorHistory", "onSessionMessage" );

    LOG_DEBUG("Entered onSessionMessage ", NULL);

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // error history
    if ( ( Wrapper == device ) && ( WrEvolErrorHistory == event ) )
    {
        LOG_DEBUG("device=Wrapper event=WrEvolErrorHistory", NULL);

        DomEvolWrapperErrors m( msg );

        // process errors
        if ( m.totalErrors() )
        {
            QStringList evolErrCode( m.field<QStringList>( m.evolErrCode() ) );
            QStringList evolErrDesc( m.field<QStringList>( m.evolErrDesc() ) );
            QStringList evolErrPkgLength( m.field<QStringList>( m.evolErrPkgLength() ) );
            QStringList evolErrPkgWidth( m.field<QStringList>( m.evolErrPkgWidth() ) );
            QStringList evolErrPkgHeight( m.field<QStringList>( m.evolErrPkgHeight() ) );
            QStringList evolErrFilmUsed( m.field<QStringList>( m.evolErrFilmUsed() ) );
            QStringList evolErrBestFit( m.field<QStringList>( m.evolErrBestFit() ) );
            QStringList evolErrPerNet( m.field<QStringList>( m.evolErrPerNet() ) );
            QStringList evolErrTrayName( m.field<QStringList>( m.evolErrTrayName() ) );
            QStringList evolErrDateTime( m.field<QStringList>( m.evolErrDateTime() ) );

            int count( m.totalErrors() );
            count = qMin( count, evolErrCode.size() );
            count = qMin( count, evolErrDesc.size() );
            count = qMin( count, evolErrPkgLength.size() );
            count = qMin( count, evolErrPkgWidth.size() );
            count = qMin( count, evolErrPkgHeight.size() );
            count = qMin( count, evolErrFilmUsed.size() );
            count = qMin( count, evolErrBestFit.size() );
            count = qMin( count, evolErrPerNet.size() );
            count = qMin( count, evolErrTrayName.size() );
            count = qMin( count, evolErrDateTime.size() );

            // do we have errors?
            if ( 0 < count )
            {
                // save block for later use
                errors_.push_back( new DomEvolWrapperErrors( m ) );

                // process items
                for ( int i( 0 ); i < count; ++i )
                {
                    const int index( table_->topLevelItemCount() );

                    QString errDateTime( evolErrDateTime[i] );
                    errDateTime.replace( ",", "\n" );

                    // item data
                    QStringList rows;
                    rows.push_back( evolErrDesc[i] );
                    rows.push_back( evolErrTrayName[i] + "\n" + evolErrPkgLength[i] + " " + evolErrPkgWidth[i] + " " + evolErrPkgHeight[i] );
                    rows.push_back( evolErrBestFit[i] + "\n" + evolErrPerNet[i] );
                    rows.push_back( errDateTime );
                    rows.push_back( "" );

                    // create widget item
                    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
                    item->setSizeHint( 0, QSize( 64, 64 ) );

                    calcTableFontSizes( item );

                    table_->addTopLevelItem( item );

                    // create help button
                    QPushButton *helpButton( new QPushButton( tr( "Help" ), this ) );
                    helpButton->setFont( tableFont_ );
                    helpButton->setObjectName( QString( "help_%1" ).arg( index ) );

                    connect( helpButton, SIGNAL( clicked() ), pushButtonSignalMapper_, SLOT( map() ) );
                    pushButtonSignalMapper_->setMapping( helpButton, helpButton );

                    table_->setItemWidget( item, 4, helpButton );

                    // save index off in case they press the help button
                    indexMap_[helpButton] = index;
                }

                // check if we have all the errors
                if ( m.totalErrors() <= table_->topLevelItemCount() )
                {
                    // okay, got them all, tell session to release resources
                    session_->send( Wrapper, WrEvolErrorHistoryListDone );

                    // enable clear history button
                    saveErrorHistory_->setEnabled( true );
                    clearHistory_->setEnabled( true );
                }
                else
                {
                    // we do not have all of them... request more...
                    session_->send( Wrapper, WrEvolErrorHistoryListNext );
                }

            } // 0 < count of errors
        } // 0 < total errors
    }
    // error history list
    else if ( ( Wrapper == device ) && ( WrEvolErrorHistoryList == event ) )
    {
        LOG_DEBUG("device=Wrapper event=WrEvolErrorHistoryList", NULL);

        DomEvolWrapperErrorsList m( msg );

        int totErrs = m.totalErrors();

        LOG_DEBUG("totErrs = %d", totErrs);

        // process errors
        if ( totErrs )
        {
            LOG_DEBUG("if ( totErrs )", NULL);

            QStringList evolErrCode( m.field<QStringList>( m.evolErrCode() ) );
            QStringList evolErrDesc( m.field<QStringList>( m.evolErrDesc() ) );
            QStringList evolErrPkgLength( m.field<QStringList>( m.evolErrPkgLength() ) );
            QStringList evolErrPkgWidth( m.field<QStringList>( m.evolErrPkgWidth() ) );
            QStringList evolErrPkgHeight( m.field<QStringList>( m.evolErrPkgHeight() ) );
            QStringList evolErrFilmUsed( m.field<QStringList>( m.evolErrFilmUsed() ) );
            QStringList evolErrBestFit( m.field<QStringList>( m.evolErrBestFit() ) );
            QStringList evolErrPerNet( m.field<QStringList>( m.evolErrPerNet() ) );
            QStringList evolErrTrayName( m.field<QStringList>( m.evolErrTrayName() ) );
            QStringList evolErrDateTime( m.field<QStringList>( m.evolErrDateTime() ) );

            int count( m.totalErrors() );
            count = qMin( count, evolErrCode.size() );
            count = qMin( count, evolErrDesc.size() );
            count = qMin( count, evolErrPkgLength.size() );
            count = qMin( count, evolErrPkgWidth.size() );
            count = qMin( count, evolErrPkgHeight.size() );
            count = qMin( count, evolErrFilmUsed.size() );
            count = qMin( count, evolErrBestFit.size() );
            count = qMin( count, evolErrPerNet.size() );
            count = qMin( count, evolErrTrayName.size() );
            count = qMin( count, evolErrDateTime.size() );

            // do we have errors?
            if ( 0 < count )
            {
                LOG_DEBUG("count = %d", count);

                // save block for later use
                errorsList_.push_back( new DomEvolWrapperErrorsList( m ) );

                // process items
                for ( int i( 0 ); i < count; ++i )
                {
                    const int index( table_->topLevelItemCount() );

                    QString errDateTime( evolErrDateTime[i] );
                    errDateTime.replace( ",", "\n" );

                    // item data
                    QStringList rows;
                    rows.push_back( evolErrDesc[i] );
                    rows.push_back( evolErrTrayName[i] + "\n" + evolErrPkgLength[i] + " " + evolErrPkgWidth[i] + " " + evolErrPkgHeight[i] );
                    rows.push_back( evolErrBestFit[i] + "\n" + evolErrPerNet[i] );
                    rows.push_back( errDateTime );
                    rows.push_back( "" );

                    // create widget item
                    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
                    item->setSizeHint( 0, QSize( 64, 64 ) );

                    calcTableFontSizes( item );

                    table_->addTopLevelItem( item );

                    // create help button
                    QPushButton *helpButton( new QPushButton( tr( "Help" ), this ) );
                    helpButton->setFont( tableFont_ );
                    helpButton->setObjectName( QString( "help_%1" ).arg( index ) );

                    connect( helpButton, SIGNAL( clicked() ), pushButtonSignalMapper_, SLOT( map() ) );
                    pushButtonSignalMapper_->setMapping( helpButton, helpButton );

                    table_->setItemWidget( item, 4, helpButton );

                    // save index off in case they press the help button
                    indexMap_[helpButton] = index;

                    // create exportable string
                    QString errorStr;
                    errorStr.append( evolErrCode[i] + "," );
                    errorStr.append( "\"" + evolErrDesc[i] + "\"," );
                    errorStr.append( evolErrPkgLength[i] + "," );
                    errorStr.append( evolErrPkgWidth[i] + "," );
                    errorStr.append( evolErrPkgHeight[i] + "," );
                    errorStr.append( evolErrFilmUsed[i] + "," );
                    errorStr.append( evolErrBestFit[i] + "," );
                    errorStr.append( evolErrPerNet[i] + "," );
                    errorStr.append( "\"" + evolErrTrayName[i] + "\"," );
                    errorStr.append( "\"" + evolErrDateTime[i] + "\"" );

                    errorStr_.push_back( errorStr );
                }

                // check if we have all the errors
                if ( m.totalErrors() <= table_->topLevelItemCount() )
                {
                    // okay, got them all, tell session to release resources
                    session_->send( Wrapper, WrEvolErrorHistoryListDone );

                    // enable clear history button
                    saveErrorHistory_->setEnabled( true );
                    clearHistory_->setEnabled( true );
                }
                else
                {
                    // we do not have all of them... request more...
                    session_->send( Wrapper, WrEvolErrorHistoryListNext );
                }

            } // 0 < count of errors
        } // 0 < total errors
        else
            LOG_DEBUG("if NOT ( totErrs )", NULL);
    }
    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::onPushButtonClicked( QWidget *widget )
{
    LOG_CLASS_INFO( "serviceNewWrapperErrorHistory", "onPushButtonClicked" );

    // clear history
    if ( clearHistory_ == widget )
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "CLEAR HISTORY" ) );
        dialog.setText( tr( "Are you sure you want to clear error history?" ) );
        dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

        int result = dialog.exec();

        LOG_DEBUG( "clear history result %d", result );

        if ( QDialog::Accepted == result )
        {
            table_->clear();
            saveErrorHistory_->setEnabled( false );
            clearHistory_->setEnabled( false );

            LOG_INFO( "clearing history...", NULL );

            session_->send( Wrapper, WrEvolErrorHistoryClear );
        }
    }

    // save history
    else if ( saveErrorHistory_ == widget )
    {
        exportErrorHistory();
    }

    // back
    else if ( back_ == widget )
    {
        pages_->setCurrentWidget( tablePage_ );
    }

    // help
    else if ( indexMap_.contains( widget ) )
    {
        static const int errorsPerBlock( 10 );

        // what index was help requested for?
        int index( indexMap_[widget] );
        int block( index / errorsPerBlock );

        // grab block containing error
        DomEvolWrapperErrors *e( (block < errors_.size()) ? errors_[block] : NULL );

        // process block
        if ( e )
        {
            // what error code?
            QStringList evolErrCode( e->field<QStringList>( e->evolErrCode() ) );
            int errCode( evolErrCode[index % errorsPerBlock].toInt() );

            LOG_INFO( "help requested for error code %d", errCode );
            QString errText( errorText( errCode ) );

            // show!
            helpText_->setText( errText );
            insertSchematics( helpText_ );
            insertLimits( helpText_ );

            pages_->setCurrentWidget( helpPage_ );
        }

        // grab block containing error - if previous block didnt contain anything.
        DomEvolWrapperErrorsList *ee( (block < errorsList_.size()) ? errorsList_[block] : NULL );

        // process block
        if ( ee )
        {
            // what error code?
            QStringList evolErrCode( ee->field<QStringList>( ee->evolErrCode() ) );
            int errCode( evolErrCode[index % errorsPerBlock].toInt() );

            LOG_INFO( "help requested for error code %d", errCode );
            QString errText( errorText( errCode ) );

            // show!
            helpText_->setText( errText );
            insertSchematics( helpText_ );
            insertLimits( helpText_ );

            pages_->setCurrentWidget( helpPage_ );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::initialize()
{
    tableFont_ = font();
    tableFont_.setPointSize( 14 );

    tableFontSplitRow_ = font();
    tableFontSplitRow_.setPointSize( 12 );

    // ---- //

    pushButtonSignalMapper_ = new QSignalMapper( this );

    connect( pushButtonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onPushButtonClicked(QWidget*) ) );

    pages_ = new QStackedWidget( this );
    pages_->addWidget( tablePage_ = new QWidget( pages_ ) );
    pages_->addWidget( helpPage_ = new QWidget( pages_ ) );

    // ---- //

    // error history table
    table_ = new QTreeWidget( tablePage_ );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::NoSelection );
    table_->setUniformRowHeights( true );

    // clear error history
    clearHistory_ = new QPushButton( tablePage_ );
    clearHistory_->setEnabled( false );
    clearHistory_->setObjectName( "clearHistory" );

    connect( clearHistory_, SIGNAL( clicked() ), pushButtonSignalMapper_, SLOT( map() ) );
    pushButtonSignalMapper_->setMapping( clearHistory_, clearHistory_ );

    // save error history
    saveErrorHistory_ = new QPushButton( tablePage_ );
    saveErrorHistory_->setEnabled( false );
    saveErrorHistory_->setObjectName( "saveHistory" );

    connect( saveErrorHistory_, SIGNAL( clicked() ), pushButtonSignalMapper_, SLOT( map() ) );
    pushButtonSignalMapper_->setMapping( saveErrorHistory_, saveErrorHistory_ );


    // ---- //

    // help text
    helpText_ = new QTextEdit( helpPage_ );
    helpText_->setObjectName( "helpText" );
    helpText_->setReadOnly( true );
    helpText_->setFocusPolicy( Qt::NoFocus );
    helpText_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // back button
    back_ = new QPushButton( helpPage_ );
    back_->setObjectName( "back" );

    connect( back_, SIGNAL( clicked() ), pushButtonSignalMapper_, SLOT( map() ) );
    pushButtonSignalMapper_->setMapping( back_, back_ );

    // ---- //

    // Note:
    // Below table was ported from serviceNewWrapperErrorHistoryScripts.asp, function sendToHelp()

    // create mapping of error codes to help strings
    // 500 does not exist as an error
    errorMap_[501] = 501;
    errorMap_[502] = 502;
    errorMap_[507] = 502;
    errorMap_[510] = 502;
    errorMap_[513] = 502;
    errorMap_[543] = 502;
    errorMap_[562] = 502;
    errorMap_[573] = 502;

    errorMap_[503] = 503;
    errorMap_[505] = 503;
    errorMap_[508] = 503;
    errorMap_[511] = 503;
    errorMap_[512] = 503;
    errorMap_[514] = 503;

    errorMap_[532] = 503;
    errorMap_[544] = 503;
    errorMap_[563] = 503;
    errorMap_[574] = 503;

    errorMap_[504] = 504;
    errorMap_[509] = 504;
    errorMap_[529] = 504;
    errorMap_[536] = 504;
    errorMap_[561] = 504;
    errorMap_[564] = 504;
    errorMap_[566] = 504;
    errorMap_[582] = 504;
    errorMap_[638] = 504;

    errorMap_[506] = 506;

    errorMap_[515] = 515;

    errorMap_[516] = 516;

    errorMap_[517] = 517;
    errorMap_[523] = 517;
    errorMap_[531] = 517;
    errorMap_[541] = 517;
    errorMap_[547] = 517;
    errorMap_[549] = 517;
    errorMap_[551] = 517;
    errorMap_[553] = 517;
    errorMap_[555] = 517;
    errorMap_[556] = 517;

    errorMap_[640] = 517;
    errorMap_[641] = 517;
    errorMap_[642] = 517;
    errorMap_[643] = 517;
    errorMap_[644] = 517;
    errorMap_[645] = 517;
    errorMap_[646] = 517;
    errorMap_[647] = 517;
    errorMap_[648] = 517;
    errorMap_[649] = 517;
    errorMap_[650] = 517;

    errorMap_[518] = 518;
    errorMap_[528] = 518;
    errorMap_[533] = 518;
    errorMap_[542] = 518;
    errorMap_[575] = 518;
    errorMap_[576] = 518;
    errorMap_[577] = 518;
    errorMap_[578] = 518;
    errorMap_[579] = 518;
    errorMap_[558] = 518;

    errorMap_[519] = 519;

    errorMap_[520] = 520;
    errorMap_[581] = 520;

    errorMap_[521] = 521;

    errorMap_[522] = 522;

    errorMap_[524] = 524;

    errorMap_[526] = 526;

    errorMap_[527] = 527;

    errorMap_[525] = 525;

    errorMap_[530] = 530;

    errorMap_[534] = 534;

    errorMap_[535] = 535;

    errorMap_[537] = 537;

    errorMap_[538] = 538;

    errorMap_[539] = 539;

    errorMap_[540] = 540;

    errorMap_[545] = 545;

    errorMap_[546] = 546;

    errorMap_[548] = 548;

    errorMap_[550] = 550;

    errorMap_[552] = 552;

    errorMap_[554] = 554;

    errorMap_[557] = 557;

    errorMap_[559] = 559;
    errorMap_[560] = 560;
    errorMap_[565] = 565;
    errorMap_[567] = 567;
    errorMap_[568] = 568;
    errorMap_[569] = 569;
    errorMap_[570] = 570;
    errorMap_[571] = 571;
    errorMap_[572] = 572;
    errorMap_[583] = 583;
    errorMap_[584] = 584;

    errorMap_[585] = 585;
    errorMap_[586] = 585;
    errorMap_[587] = 585;
    errorMap_[588] = 585;
    errorMap_[589] = 585;

    errorMap_[590] = 591;
    errorMap_[590] = 591;


    // original code had this case defined, but it is mapped above (in original code also)
    // so commenting out...
    //errorMap_[581] = 581;

    errorMap_[634] = 634;
    errorMap_[635] = 634;
    errorMap_[636] = 634;
    errorMap_[637] = 634;

    errorMap_[592] = 592;
    errorMap_[593] = 592;
    errorMap_[594] = 592;
    errorMap_[595] = 592;
    errorMap_[596] = 592;
    errorMap_[597] = 592;
    errorMap_[598] = 592;
    errorMap_[599] = 592;
    errorMap_[600] = 592;

    errorMap_[601] = 592;
    errorMap_[602] = 592;
    errorMap_[603] = 592;
    errorMap_[604] = 592;
    errorMap_[605] = 592;
    errorMap_[606] = 592;
    errorMap_[607] = 592;
    errorMap_[608] = 592;
    errorMap_[609] = 592;

    errorMap_[610] = 592;
    errorMap_[611] = 592;
    errorMap_[612] = 592;
    errorMap_[613] = 592;
    errorMap_[614] = 592;
    errorMap_[615] = 592;
    errorMap_[616] = 592;

    errorMap_[617] = 617;
    errorMap_[618] = 617;
    errorMap_[619] = 617;
    errorMap_[620] = 617;
    errorMap_[621] = 617;
    errorMap_[622] = 617;
    errorMap_[623] = 617;
    errorMap_[624] = 617;
    errorMap_[625] = 617;
    errorMap_[626] = 617;

    errorMap_[627] = 627;

    errorMap_[628] = 628;

    errorMap_[629] = 629;

    errorMap_[630] = 630;

    errorMap_[631] = 631;

    errorMap_[632] = 632;

    errorMap_[633] = 633;

    errorMap_[638] = 638;
    errorMap_[639] = 639;

    errorMap_[651] = 651;
    errorMap_[652] = 652;
    errorMap_[653] = 653;
    errorMap_[654] = 654;
    errorMap_[655] = 655;
    errorMap_[656] = 656;
    errorMap_[657] = 657;
    errorMap_[658] = 658;
    errorMap_[659] = 659;
    errorMap_[660] = 660;
    errorMap_[661] = 661;
    errorMap_[662] = 662;
    errorMap_[663] = 663;
    errorMap_[664] = 664;
    errorMap_[665] = 665;
    errorMap_[666] = 666;
    errorMap_[667] = 667;
    errorMap_[668] = 668;
    errorMap_[669] = 669;
    errorMap_[670] = 670;
    errorMap_[671] = 671;
    errorMap_[672] = 672;
    errorMap_[673] = 673;
    errorMap_[674] = 674;
    errorMap_[675] = 675;
    errorMap_[676] = 676;
    errorMap_[677] = 677;
    errorMap_[678] = 678;
    errorMap_[679] = 679;
    errorMap_[680] = 680;
    errorMap_[681] = 681;
    errorMap_[682] = 682;
    errorMap_[683] = 683;
    errorMap_[684] = 684;
    errorMap_[685] = 685;

    errorMap_[686] = 686;
    errorMap_[687] = 687;
    errorMap_[688] = 688;
    errorMap_[689] = 689;
    errorMap_[690] = 690;
    errorMap_[691] = 691;
    errorMap_[692] = 692;
    errorMap_[693] = 693;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::createLayout()
{
    QHBoxLayout *tablePageButtons( new QHBoxLayout() );

    tablePageButtons->addStretch();
    tablePageButtons->addWidget( clearHistory_ );
    tablePageButtons->addWidget( saveErrorHistory_ );

    QVBoxLayout *tablePageLayout( new QVBoxLayout( tablePage_ ) );
    tablePageLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    tablePageLayout->addWidget( table_, 1 );
    tablePageLayout->addLayout( tablePageButtons );

    // ---- //

    QHBoxLayout *helpPageButtons( new QHBoxLayout() );

    helpPageButtons->addStretch();
    helpPageButtons->addWidget( back_ );

    QVBoxLayout *helpPageLayout( new QVBoxLayout( helpPage_ ) );
    helpPageLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    helpPageLayout->addWidget( helpText_, 1 );
    helpPageLayout->addLayout( helpPageButtons );

    // ---- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addWidget( pages_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::calcTableColumnWidths()
{
    const int tableWidth( table_->contentsRect().width() );

    table_->setColumnWidth( 0, (tableWidth * 42) / 100 ); // error
    table_->setColumnWidth( 1, (tableWidth * 19) / 100 ); // tray / dimension
    table_->setColumnWidth( 2, (tableWidth * 12) / 100 ); // best fit / % stretch
    table_->setColumnWidth( 3, (tableWidth * 12) / 100 ); // date / time
    table_->setColumnWidth( 4, (tableWidth * 10) / 100 ); // help

    // Note:
    // 5% not allocated to handle a vertical scroll bar.
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::calcTableFontSizes( QTreeWidgetItem *item ) const
{
    item->setFont( 0, tableFont_ ); // error
    item->setFont( 1, tableFontSplitRow_ ); // tray / dimension
    item->setFont( 2, tableFontSplitRow_ ); // best fit / % stretch
    item->setFont( 3, tableFontSplitRow_ ); // date / time
    item->setFont( 4, tableFont_ ); // help
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString serviceNewWrapperErrorHistory::errorText( int errCode ) const
{
    QString helpText;

    // what text does this error map to?
    if ( errorMap_.contains( errCode ) )
        errCode = errorMap_[errCode];
    else
        errCode = -1;

    // get help text for error code
    switch ( errCode )
    {

    case 501:

        // serviceNewWrapperErrorHistHelp501
        helpText = tr( "Wrapper status\n\nThe wrapper stopped communicating.\nCheck to be sure the wrapper cable is plugged in.\n\nPress the STOP button, then press the START button to home the machine." );
        break;

    case 502:

        // serviceNewWrapperErrorHistHelp502
        helpText = tr(
            "Error Description: Sensor or Belt failure\n"
            "\n"
            "Affected:\n"
            "Primary Intake, Upper Pusher, Rear Underfolder, Front Underfolder, Side Underfolder, Gripper, Side Clamp\n"
            "\n"
            "Error Definition:\n"
            "Mechanism has completed a motion away from the home switch but switch still reports closed.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual chapters 1 & 2.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Visually check the mechanism for broken hardware (home switch sensor / belts / motor shaft / gears / missing gear key / missing magnet).\n"
            "\n"
            "2. If the mechanical hardware is good, manually move the mechanism away from its home switch. Enter Service Mode > Peripherals > Tests and select the offending mechanism.\n"
            "\n"
            "The Sensor (home switch) for this mechanism should be reporting open (0) and the sensor box should be red in color. If green in color, check the home switch for proper gap.\n"
            "\n"
            "The home switch air gap should be a minimum of .063\" to a maximum of .125\" adjust gap if needed and retest.\n"
            "\n"
            "If the air gap is correct and the sensor is still not reporting open, be sure that the sensor input wiring to the brushless board is not excessively tight and that the connector pins make good contact.\n"
            "\n"
            "If the sensor is still not reporting open, replace the sensor and adust the air gap. After replacing the sensor, verify that the sensor box is showing open.\n"
            "\n"
            "Cycle the mechanism by touching the Cycle button and verify that the sensor box changes states (green, red). If the sensor was replaced, calibrate the offending mechanism.\n" );

        break;

    case 503:

        // serviceNewWrapperErrorHistHelp503
        helpText = tr(
            "Error Description: Motor electrical failure\n"
            "\n"
            "Affected:\n"
            "Primary Intake, Upper Pusher, Rear Underfolder, Front Underfolder, Side Underfolder, Gripper, Side Clamp, Secondary Intake, Elevator.\nSealer belt for Combo boards.\n"
            "\n"
            "Error Definition:\n"
            "While executing a motion, no motor encoder pulses were found and low motor current was measured.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual chapters 1 & 2.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Press the wrapper stop button then visually check the motor power connector (see above: J8 or J12 for BLDC; J7 or J18 for Combo). Check that the wiring is not excessively tight and pins are seated correctly in the connector.\n"
            "\n"
            "If a pin to the connector does not look seated, you can give the wire a slight tug and the pin should not pull out of the connector. If it does, try to reseat the pin in the connector or replace the motor. Go to step 3.\n"
            "\n"
            "2. Visually check the motor encoder connector (see above J13 or J15 for BLDC; J22 or J23 for Combo). Check that the wiring is not excessively tight and all pins are seated correctly in the connector.\n"
            "\n"
            "3. If the motor connections are good, enter Service Mode, Peripherals > Wrapper > Tests and select the mechanism that is failing. Press the start button and try to cycle the mechanism by touching the Cycle button.\n"
            "\n"
            "If the mechanism does not move, Replace the motor and retest. If mechanism still does not move, replace the brushless drive board and retest.\n" );

        break;

    case 504:

        // serviceNewWrapperErrorHistHelp504
        helpText = tr(
            "Error Description: Slow Mechanism\n"
            "\n"
            "Affected:\n"
            "Primary Intake, Upper Pusher, Rear Underfolder, Front Underfolder, Side Underfolder, Gripper, Side Clamp, Secondary Intake, Elevator\n"
            "\n"
            "Error Definition:\n"
            "Timeout occurred while waiting for mechanism to complete movement.\n"
            "\n"
            "Electrical Locations:\n"
            "See service electrical manual chapters 1 and 2.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Manually check the mechanism range has free range of motion and that it does not stick or bind. Check that the drive belt tension is not too loose or too tight.\n"
            "\n"
            "To check the tension, apply moderate pressure at the belt mid point and adjust if needed. Long belts such as the Upper Pusher belts should only deflect a minimum of .25\" and a maximum of .5\". Shorter belts such as the Primary Conveyor should only deflect a minimum of .125\" and a maximum of .25\".\n" );

        break;

    case 506:

        // serviceNewWrapperErrorHistHelp506
        helpText = tr(
            "Error Description: Height sensors failed to auto calibrate\n"
            "\n"
            "Affected:\n"
            "Height sensors portion of the camera assembly\n"
            "\n"
            "Error Definition:\n"
            "Error is caused by the height sensor receivers not detecting enough light from the emitters during an auto calibration.\nFor Combo boards, the height sensors auto calibrate each time the wrapper circuit breaker is cycled.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual chapter 2 and 5.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Verify that both sides of the height sensors are free of debris and clean. Check the calibrated light level by entering Peripherals > Wrapper > Test > Intake.\n"
            "\n"
            "Touch the Height Sensors On button to view the Height sensors. An initial height value will light for 20 seconds. This is an indication of the power level used when the wrapper auto calibrated the height sensors. Height values of 2\",3\" or 4\" is a normal calibration indication.\n"
            "\n"
            "After 20 seconds, no height value should be green. If so, visually check the height sensors connectors at the camera board. All pins in the connector should be properly seated. If any pin does not look properly seated, give slight pull on the wire and if the pin comes out, replace the assembly and retest.\n"
            "\n"
            "If you view a calibration value of 5 or 6, replace the height sensor assembly and retest. If the calibration value is within normal range, manually block all height sensors to verify functionality. Turn height sensors off.\n" );

        break;

    case 515:

        // serviceNewWrapperErrorHistHelp515
        helpText = tr( "Wrapper status\n\nPrimary conveyor not at home.\n\nPress the START button to home the machine." );
        break;

    case 516:

        // serviceNewWrapperErrorHistHelp516
        helpText = tr( "Wrapper status\n\nCamera and height sensors blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine.\n\nCheck for package too far forward. Clean both cameras, light tube, and height sensors.\n\nPress the START button." );
        break;

    case 517:

        // serviceNewWrapperErrorHistHelp517
        helpText = tr(
            "Error Description: Mechanism jammed\n"
            "\n"
            "Affected:\n"
            "Primary Intake, Upper Pusher, Rear Underfolder, Front Underfolder, Side Underfolder, Gripper, Side Clamp, Secondary Intake, Elevator, Sealer Belt\n"
            "\n"
            "Error Definition:\n"
            "While executing a motion, no motor encoder pulses were present and very high motor current was measured. This error usually happens when a package is crushed or debris is in the mechanism.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual pages 2-2, 2-3 and 1-5.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Press the wrapper stop button and visually check mechanism for obstructions. Check the range of motion for the offending mechanism. Make sure there is no binding and the mechanism moves freely.\n"
            "\n"
            "2. Check the mechanisms parallel alignment. To check, position assembly to a mechanical stop or to a fixed surface on the machine. Both sides of the mechanism should be equally spaced.\n"
            "\n"
            "3. Check the air gap on the home switch of the offending mechanism. The air gap should be a minimum of .063\" to a maximum of .125\".\n"
            "\n"
            "4. Check that the home switch is functioning properly.\n"
            "\n"
            "5. Cycle the mechanism (Peripherals > Wrapper > Test) several times to make sure the mechanism does not jam. Go back to the Error History to see if this error is happening frequently and make note of the tray name for each error instance.\n"
            "\n"
            "If the history shows a high jam occurrence with the same tray pattern, this might indicate a wrap timing problem with this tray. Ask the operator if he / she has noticed that a particular tray has been crushing.\n"
            "\n"
            "If Primary Intake Jam, make sure that the primary paddles are aligned with each other.\n"
            "\n"
            "If Pusher Jam, check the mechanisms parallel alignment and calibrate just the Pusher, Peripherals > Wrapper > Calibration.\n"
            "\n"
            "If Front Folder or Rear Folder Jam, try and reduce the front to back tension for the particular tray.\n"
            "\n"
            "If Side Folder or Side Clamp Jam, try and reduce the side to side tension for the particular tray.\n"
            "\n"
            "If Gripper Jam, check the Gripper stops at the film selector are properly adjusted to as close to a .125\" gap between the end of the Gripper fingers and the Film Selector as possible.\n"
            "\n"
            "Check that the Gripper is not making contact with any of Film Selector combs. If Gripper is making contact, center the Film Selector (see service manual).\n"
            "\n"
            "If the Grippers home switch or stops are adjusted, be sure to re-calibrate the Gripper, Peripherals > Wrapper > Calibration. The Gripper could also collide with the knife causing a jam. Make sure that the Knife has free range of motion and that it drops freely under its own weight." );

        break;

    case 518:

        // serviceNewWrapperErrorHistHelp518
        helpText = tr(
            "Error Description: Mechanism did not find home\n"
            "\n"
            "Affected:\n"
            "Primary Intake, Upper Pusher, Rear Underfolder, Front Underfolder, Side Underfolder, Gripper, Side Clamp, Shifter, Elevator, Label Applier\n"
            "\n"
            "Error Definition:\n"
            "Mechanism travel through its full homing range and home switch state did not change to the closed state.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual pages 2-1, 2-2, 2-3 and 1-5.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Visually check the mechanism for broken hardware (home switch sensor / belts / motor shaft / gears / missing gear key).\n"
            "\n"
            "2. If the mechanical hardware is good, manually move the mechanism to its home switch. Enter Service Mode > Peripherals > Tests and select the offending mechanism.\n"
            "\n"
            "The Sensor (home switch) for this mechanism should be reporting closed (1) and the sensor box should be green in color. If red in color, check the home switch for proper gap.\n"
            "\n"
            "The home switch air gap should be a minimum of .063\" to a maximum of .125\" adjust gap if needed and retest.\n"
            "\n"
            "If the air gap is correct and the sensor is still not reporting closed, be sure that the sensor input wiring to the brushless board is not excessively tight and that the connector pins make good contact.\n"
            "\n"
            "If the sensor is still not reporting closed, replace the sensor and adust the air gap. After replacing the sensor, verify that the sensor box is showing closed. If sensor was replaced, be sure to recalibrate the mechanism.\n"
            "\n"
            "Cycle the mechanism by pressing the start button on the wrapper and touching the Cycle button, verify that the sensor box changes states (green, red).\n"
            "\n"
            "If the mechanism cycles and the home switch is functioning then press the wrapper stop button. Manually move the mechanism away from the home switch.\n"
            "\n"
            "Verify that the sensor box is showing the home switch open(red). Return to the Operate mode, press the wrapper start button and verify that the mechanism homed.\n"
            "\n"
            "3. If the mechanism does not move during the cycle test or has trouble moving then replace the mechanisms motor and retest. If the mechanism still has problems then replace the brushless driver board and retest.\n"
            "\n"
            "The motor should cycle freely at this point. The mechanism motor that was removed may not be bad, before putting it back in try to spin the motor shaft.\n"
            "\n"
            "If the shaft does not turn freely, do not re-install the motor. If the motor shaft does turn freely, replace and retest mechanism.\n" );

        break;

    case 519:

        // serviceNewWrapperErrorHistHelp519
        helpText = tr( "Wrapper status\n\nIntake blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 520:

        // serviceNewWrapperErrorHistHelp520
        helpText = tr(
            "Error Description: Secondary Intake / Elevator fall sync or Secondary Intake / Elevator rise sync\n"
            "\n"
            "Affected:\n"
            "Secondary Intake\n"
            "\n"
            "Error Definition:\n"
            "Error is caused by the secondary intake and elevator being out of sync from each other. The Secondary Intake stops the package short of the Elevator because the Elevator was too high when the secondary was to place the package onto the Elevator.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Adjust the intake speed or discharge speed for this packages wrap box. The technician should note the tray type and dimensions associated with this error and also the wrapper software version and report problem to product support.\n" );

        break;

    case 521:

        // serviceNewWrapperErrorHistHelp521
        helpText = tr(
            "Error Description: Package dimension error\n"
            "\n"
            "Affected:\n"
            "Cameras\n"
            "\n"
            "Error Definition:\n"
            "Package dimensions outside of cameras measurement range.\nFor Combo boards, package length measurement too long or too short.\n"
            "\n"
            "Electrical Locations:\n"
            "See AWS electrical service manual chapter 2 and AWS installation manual chapter 2.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Check that the Primary Intake paddles are all in alignment with each other. If not, adjust to bring paddles into alignment. Intake paddles that are out of alignment will cause camera measurement problems.\nVerify the operation of the primary intake home switch; Node 1, LED D9, or in Service > Wrapper > Tests > Intake, Primary conveyor Home Sensor.  The sensor should be green when the intake pusher is home and red when the pusher is off home.\n"
            "\n"
            "2. Check the cameras view of the intake by entering into service mode on the console and selecting Peripherals > Wrapper > Tests > Intake Camera View. Verify that the camera's waveforms closely conforms to the reference (light green) lines.\n"
            "\n"
            "Make sure that each camera view (black lines) in the middle of the screen do not trail off too soon. They should trail off very close to how the reference line trails off. If the camera's scan does not closely match the reference,\n"
            "\n"
            "Make sure that the light tube and camera's clear plastic cover is clean and that the center of the tube is not light pink in color.\nFor BLDC boards the light tube should be red in color from end to end. If pink in color, The light tube emitters need to be replaced.\n"
            "\n"
            "Make sure that the camera view for each side also closely matches the top of the reference line. If not, try to adjust the camera assembly.\nFor BLDC boards if the light tube is not bright enough, the emitters will need to be replaced.\n"
            "\n"
            "3. Calibrate the right and left camera and check the Height sensors. Enter Service mode, select Peripherals > Wrapper > Tests > Camera or Peripherals > Wrapper > Tests > Intake. Cut a 5\" x 8\" piece from a flat bottom tray and place tray piece over the right side of the light pipe with the edge of piece 7-1/2\" from left side of the intake.\n"
            "\n"
            "Touch the Cal left button, the button will turn red then returns to normal when left side calibration is finished. Move the tray piece to the left side of the light pipe, measure 7-1/2\" from right side and touch the Cal right button. After Calibration touch the On button for the Height Sensors.\n"
            "\n"
            "An initial height value will light for 20 seconds. This is an indication of the power level used when the wrapper auto calibrated the height sensors. Height values of 2\",3\" or 4\" is a normal calibration indication. Manually block all height sensors to verify functionality. Turn height sensors off.\n"
            "\n"
            "If the height sensors indicate a power level of 5\" or 6\", verify that both sides of the height sensors are free of debris and clean. If assembly is clean then replace height sensor assembly and retest.\n"
            "\n"
            "For Combo boards, calibrate the primary intake using a tray from the middle of the length range. Service > Wrapper > Calibration, Primary intake.  Enter the length of the tray in the tray length to test field.\n" );

        break;

    case 522:

        // serviceNewWrapperErrorHistHelp522
        helpText = tr(
            "Error Description: Package did not trip Elevator Intake sensor\n"
            "\n"
            "Affected:\n"
            "Package on Elevator sensor\n"
            "\n"
            "Error Definition:\n"
            "Secondary Intake did not see the package sensor detect the package.\n"
            "\n"
            "Electrical Locations:\n"
            "See electrical service manual chapters 2 and 5.\n"
            "\n"
            "Package sensor input is located at J15 on the Combo driver board for Node 4.\n"
            "Package sensor LED is located at D16 on the Combo driver board for Node 4.\n"
            "\n"
            "Package sensor input is located at J11 pin 8 on the brushless motor board for Node 4.\n"
            "Package sensor LED is located at D28 on the brushless motor board for Node 4.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Test the package sensor by placing a tray on the Secondary Intake and across the sensors.\n"
            "     Check the sensor two ways:\n"
            "        a) By visually looking at LED D28 [BLCD] or D16 [Combo] on the brushless motor driver board for Node 4. It should be off.\n"
            "        b) Check that the Package Sensor at Peripherals > Wrapper > Test > Elevator is red.\nIf it is off/red, remove and clean both sides of the sensor and retest.\n"
            "\n"
            "2. If sensor is still not working, replace sensors and retest. Make sure Secondary Intake drive belts are clean and that the drive coupler is properly engaged.\n"
            "\n"
            "For Combo boards, the sensor should be adjusted so that it remains on/green when a 0.250 gage is on the intake between the emmitter and detector, and off/red when a 0.500 gage is on the intake between the emmitter and receiver.\n" );

        break;

    case 524:

        // serviceNewWrapperErrorHistHelp524
        helpText = tr( "Wrapper status\n\nCentering conveyor not at home.\n\nPress the START button to home the machine." );
        break;

    case 525:

        // serviceNewWrapperErrorHistHelp525
        helpText = tr(
            "Error Description: Cameras need alignment\n"
            "\n"
            "Affected:\n"
            "Camera board\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual chapter 2 and installation manual chapter 2.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Check the cameras view of the intake by entering into service mode on the console and selecting Peripherals > Wrapper > Tests > Intake Camera View. Verify that the camera's waveforms closely conforms to the reference (light green) lines.\n"
            "\n"
            "Make sure that each camera view (black lines) in the middle of the screen do not trail off too soon. They should trail off very close to how the reference line trails off. If the camera's scan does not closely match the reference,\n"
            "\n"
            "Make sure that the light tube and camera's clear plastic cover is clean and that the center of the tube is not light pink in color. The light tube should be red in color from end to end. If pink in color, The light tube emitters need to be replaced.\n"
            "\n"
            "Make sure that the camera view for each side also closely matches the top of the reference line. If not, try to adjust the camera assembly or the light tube is not bright enough and the emitters will need to be replaced.\n"
            "\n"
            "2. Calibrate the right and left camera and check the Height sensors. Enter Service mode, select Peripherals > Wrapper > Tests > Camera or Peripherals > Wrapper > Tests > Intake. Cut a 5\" x 8\" piece from a flat bottom tray and place tray piece over the right side of the light pipe with the edge of piece 7-1/2\" from left side of the intake.\n"
            "\n"
            "Touch the Cal left button, the button will turn red then returns to normal when left side calibration is finished. Move the tray piece to the left side of the light pipe, measure 7-1/2\" from right side and touch the Cal right button. After Calibration touch the On button for the Height Sensors.\n"
            "\n"
            "An initial height value will light for 20 seconds. This is an indication of the power level used when the wrapper auto calibrated the height sensors. Height values of 2\",3\" or 4\" is a normal calibration indication. Manually block all height sensors to verify functionality. Turn height sensors off.\n"
            "\n"
            "If the height sensors indicate a power level of 5\" or 6\", verify that both sides of the height sensors are free of debris and clean. If assembly is clean then replace assembly and retest.\n" );

        break;

    case 526:

        // serviceNewWrapperErrorHistHelp526
        helpText = tr( "Wrapper status\n\nPackage is too far left.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 527:

        // serviceNewWrapperErrorHistHelp527
        helpText = tr( "Wrapper status\n\nPackage is too far right.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 530:

        // serviceNewWrapperErrorHistHelp530
        helpText = tr( "Wrapper status\n\nElevator not at home.\n\nPress the START button to home the machine." );
        break;

    case 534:

        // serviceNewWrapperErrorHistHelp534
        helpText = tr(
            "Error Description: Elevator sensor or cam failure\n"
            "\n"
            "Affected:\n"
            "Elevator\n"
            "\n"
            "Error Definition:\n"
            "Elevator has completed most of its motion and is still detecting its home switch.\n"
            "\n"
            "Electrical Locations:\n"
            "See AWS electrical service manual chapter 2 and AWS Installation manual chapter 4.\n"
            "\n"
            "Elevator home sensor input is located at J13 on the Combo driver board for Node 1.\n"
            "Elevator home sensor LED D12 on the Combo driver board for Node 1.\n"
            "\n"
            "Elevator home sensor input is located at J11 pin 4 on the brushless driver board for Node 1.\n"
            "Elevator home sensor LED D31 on the brushless driver board for Node 1.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Check the Elevator home switch air gap it should be a minimum of .063\" to a maximum of .125\". Enter Peripherals > Wrapper > Tests > Elevator. Press the start button and touch the cycle button.\n"
            "\n"
            "Make sure that the Elevator home sensor (switch) changes from red (off home) to green (home). If switch states do not change, replace home switch and retest.\n" );

        break;

    case 535:

        // serviceNewWrapperErrorHistHelp535
        helpText = tr(
            "Error Description: Elevator home switch failure\n"
            "\n"
            "Affected:\n"
            "Elevator\n"
            "\n"
            "Error Definition:\n"
            "Elevator home switch has failed\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual pages 1-5 Installation manual page 4-48.\n"
            "\n"
            "Elevator home sensor input is located at J13 on the Combo driver board for Node 1.\n"
            "Elevator home sensor LED is located at D12 on the Combo driver board for Node 1.\n"
            ".\n"
            "Elevator home sensor input is located at J11 pin 4 on the brushless driver board for Node 1.\n"
            "Elevator home sensor LED is located at D12 on the brushless driver board for Node 1.\n" );

        break;

    case 537:

        // serviceNewWrapperErrorHistHelp537
        helpText = tr(
            "Error Description: Could not adjust percent net stretch enough for package\n"
            "\n"
            "Affected:\n"
            "Wrap box timings\n"
            "\n"
            "Error Definition:\n"
            "Could not auto adjust percent net stretch enough to wrap package\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Check film width settings in Operate mode > Settings > Wrapper Settings matches with the film size installed. If Setting are correct see step 2.\n"
            "\n"
            "2. Go back to the history page and note the tray name and package height for this error. Enter wrapper timings by Peripherals > Wrapper > Wrap Timings and adjust this trays height wrap box Percent Net Stretch.\n"
            "\n"
            "If small tray profile, decrease Percent Net Stretch value and retry package (cycle several times) taking note that the Percent Net Stretch field does not turn yellow (indicates auto adj decrease of Percent Net Stretch).\n"
            "\n"
            "If large tray profile, increase Percent Net Stretch value and retry package taking note that the Percent Net Stretch field does not turn yellow (indicates auto adj increase of Percent Net Stretch).\n"
            "\n"
            "If Percent Net Stretch field turns yellow while cycling the package, readjust Percent Net Stretch value and retest until the field is normal while cycling the package.\n" );

        break;

    case 538:

        // serviceNewWrapperErrorHistHelp538
        helpText = tr( "Wrapper status\n\nFilm not pulled. Press the STOP button. Check to make sure film is properly loaded and the gripper and film selector are clear of debris. Press the START button." );
        break;

    case 539:

        // serviceNewWrapperErrorHistHelp539
        helpText = tr( "Wrapper status\n\nGripper not at home.\n\nPress the START button to home the machine." );
        break;

    case 540:

        // serviceNewWrapperErrorHistHelp540
        helpText = tr(
            "Error Description: Knife Sensor did not trip\n"
            "\n"
            "Affected:\n"
            "Knife\n"
            "\n"
            "Error Definition:\n"
            "Knife switch did not trip for package.\n"
            "\n"
            "Electrical Locations:\n"
            "See AWS installation manual chapter 4.\n"
            "\n"
            "Knife height sensor LED D16 on Node 1 Combo driver board.\n"
            "\n"
            "Knife height sensor LED D28 on Node 1 brushless motor board.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Lift Knife assembly and verify that the Knife assembly drops freely under its own weight. Check that the compressor is set to 50 PSI and that no air leaks are found. Check the Knife cylinders for water. If water is present, drain and retest.\n"
            "\n"
            "2. Check the Knife switch air gap - it should be a minimum of .063\" to a maximum of .125\". Check that the Knife switch is functioning properly by viewing the sensor indicator in Peripherals > Wrapper > Tests > Knife, while the mechanism is in the UP (green) / Down (red) positions.\n"
            "\n"
            "3. Enter Peripherals > Wrapper > Tests > Knife. The box at the bottom of the screen displays the time it takes the knife to fire and is labeled \"Knife Time\".\n"
            "\n"
            "Cycle a few packages, the Knife Time should be no greater / less than 10 difference from the knife time recorded in the \"Install Knife Time\" field. Check that the Knife Time does not exceed 150.\n"
            "\n"
            "If the Knife does not fire, ohms check across the Knife valve coil for proper reading and check each coil pin to ground for shorts. Check the valve wiring harness for shorts and manually test the valve to insure that it does not stick.\n"
            "\n"
            "If the Knife does fire and is greater than 150, try adding 5 drops of tool oil to the cylinders air line and retest. If the time is still too high, try replacing the cylinders and retest. If the Knife Time is still too high, change the valve for the Knife and retest.\n"
            "\n"
            "Be aware that the greater the difference between the \"Install Knife Time\" and \"Knife Time\" may cause wrap quality issues. Using all the customers trays, run the wrapper and check for wrap quality issues.\n"
            "\n"
            "If wrap quality issues are found, Enter Service Mode, Peripherals > Wrapper > Wrapper Timings and adjust the Knife Fire Height parameter for each tray that has a wrap quality issue and retest that tray.\n"
            "\n"
            "If the knife cylinder or valve was changed or water removed, go back to the Knife test page and touch the Clear Install Time button. This will clear the knife history information and the very next package cycled in the machine will record a new Install Time and date.\n" );

        break;

    case 545:

        // serviceNewWrapperErrorHistHelp545
        helpText = tr(
            "Error Description: Knife took too long to fire\n"
            "\n"
            "Affected:\n"
            "Knife\n"
            "\n"
            "Error Definition:\n"
            "Knife mechanism took too long a time to cut film. Knife Time value exceeded 300mS.\n"
            "\n"
            "Electrical Locations:\n"
            "See AWS installation manual chapter 4.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Lift Knife assembly and verify that the Knife assembly drops freely under its own weight. Check that the compressor is set to 50 PSI and that no air leaks are found. Check the Knife cylinders for water. If water is present, drain and retest.\n"
            "\n"
            "2. Enter Peripherals > Wrapper > Tests > Knife. The box at the bottom of the screen displays the time it takes the knife to fire and is labeled \"Knife Time\".\n"
            "\n"
            "Cycle a few packages, the Knife Time should be no greater / less than 10 difference from the knife time recorded in the \"Install Knife Time\" field. Check that the Knife Time does not exceed 150.\n"
            "\n"
            "If the Knife Time is greater than 150, try adding 5 drops of tool oil to the cylinders air line and retest. If the Knife Time is still too high, try replacing the cylinder and retest. If the Knife Time is still too high change the valve for the Knife and retest.\n"
            "\n"
            "Be aware that the greater the difference between the \"Install Knife Time\" and \"Knife Time\" may cause wrap quality issues. Using all the customers trays, run the wrapper and check for wrap quality issues.\n"
            "\n"
            "If wrap quality issues are found, Enter Service Mode, Peripherals > Wrapper > Wrapper Timings and adjust the Knife Fire Height parameter for each tray that has a wrap quality issue and retest that tray.\n"
            "\n"
            "If the knife cylinders was changed or water removed or valve changed, Enter back into the Knife test page and touch the Clear Install Time button. This will clear the knife history information and the very next package cycled in the machine will record a new Install Time and date.\n" );

        break;

    case 546:

        // serviceNewWrapperErrorHistHelp546
        helpText = tr("Wrapper status\n\nSide clamps not at home.\n\nPress the START button to home the machine." );
        break;

    case 548:

        // serviceNewWrapperErrorHistHelp548
        helpText = tr("Wrapper status\n\nSide folders not at home.\n\nPress the START button to home the machine." );
        break;

    case 550:

        // serviceNewWrapperErrorHistHelp550
        helpText = tr( "Wrapper status\n\nRear folder not at home.\n\nPress the START button to home the machine." );
        break;

    case 552:

        // serviceNewWrapperErrorHistHelp552
        helpText = tr( "Wrapper status\n\nFront folder not at home.\n\nPress the START button to home the machine." );
        break;

    case 554:

        // serviceNewWrapperErrorHistHelp554
        helpText = tr( "Wrapper status\n\nPusher not at home.\n\nPress the START button to home the machine." );
        break;

    case 557:

        // serviceNewWrapperErrorHistHelp557
        helpText = tr(
            "Error Description: Knife jammed or sensor failure\n"
            "\n"
            "Affected:\n"
            "Knife\n"
            "\n"
            "Error Definition:\n"
            "Knife jammed while cutting film.\n"
            "\n"
            "Electrical Locations:\n"
            "See AWS electrical service manual chapter 1 and AWS Installation manual chapter 4.\n"
            "\n"
            "Knife height sensor input is located at J15 on Combo driver board for Node 1.\n"
            "Knife height sensor LED D16 on Combo driver board for Node 1.\n"
            "Knife height sensor input is located at J11 pin 8 on brushless driver board for Node 1.\n"
            "Knife height sensor LED D28 on brushless driver board for Node 1.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Lift Knife assembly and verify that the Knife assembly does not bind and that it drops freely under its own weight.\n"
            "\n"
            "Check the Knife switch air gap - it should be a minimum of .063\" to a maximum of .125\". Check that the Knife switch is functioning properly by viewing the sensor indicator in Peripherals > Wrapper > Tests > Knife while the mechanism is in the UP (green) / Down (red) positions.\n"
            "\n"
            "Enter Peripherals > Wrapper > Tests > Knife. Cycle a few packages through the machine while watching for any binding or jamming of the Knife.\n" );

        break;

    case 559:

        // serviceNewWrapperErrorHistHelp559
        helpText = tr( "Wrapper status\n\nTemperature Error" );
        break;

    case 560:

        // serviceNewWrapperErrorHistHelp560
        helpText = tr( "Wrapper status\n\nHeater Error" );
        break;

    case 565:

        // serviceNewWrapperErrorHistHelp5565
        helpText = tr( "Wrapper status\n\nWrapper lost CAN communications.\n\nCycle the circuit breaker. Press the START button." );
        break;

    case 567:

        // serviceNewWrapperErrorHistHelp567
        helpText = tr( "Wrapper status\n\nCanopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 568:

        // serviceNewWrapperErrorHistHelp568
        helpText = tr( "Wrapper status\n\nRight-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 569:

        // serviceNewWrapperErrorHistHelp569
        helpText = tr( "Wrapper status\n\nLeft-side access door open.\n\nPlease close the door. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 570:

        // serviceNewWrapperErrorHistHelp570
        helpText = tr( "Wrapper status\n\nCanopy open.\n\nPlease close the canopy. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 571:

        // serviceNewWrapperErrorHistHelp571
        helpText = tr( "Wrapper status\n\nRear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 572:

        // serviceNewWrapperErrorHistHelp572
        helpText = tr( "Wrapper status\n\nRear film door open.\n\nPlease be sure both film doors are closed. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 581:

        // serviceNewWrapperErrorHistHelp581
        helpText = tr(
            "Error Description: Package stopped on Secondary Intake\n"
            "\n"
            "Affected:\n"
            "Secondary Intake\n"
            "\n"
            "Error Definition:\n"
            "Error is caused by the secondary intake and elevator being out of sync from each other. The Secondary Intake stops the package short of the Elevator because the Elevator was to high when the secondary was to place the package onto the Elevator.\n" );

        break;

    case 583:

        // serviceNewWrapperErrorHistHelp583
        helpText = tr("Press the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 584:

        // serviceNewWrapperErrorHistHelp584
        helpText = tr( "Package is too tall" );
        break;

    case 585:

        // serviceNewWrapperErrorHistHelp585
        helpText = tr(
            "Error Description: Motor fuse blown\n"
            "\n"
            "Affected:\n"
            "Node boards 1 - 5\n"
            "\n"
            "Error Definition:\n"
            "The node board motor fuse might be blown.\n"
            "\n"
            "Electrical Locations: High Voltage, Please Press Wrapper Stop Button Before Checking Fuses!\n"
            "See AWS electrical service manual chapter 1 and 2.\n"
            "\n"
            "%SCHEMATICS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Press the start switch and check that the LED on the power supply board is on (LED D15 on standard BLDC board machines, LED D7 on Combo board machines). If not, press the AWS stop switch to remove high power. Wait until D15 [D7 for Combo boards] is off then check power supply fuse F4 by removing fuse and checking with an ohm meter. A visual check of the fuse might be all that is needed. Bad fuses will sometimes show burn marks on the glass or the wire inside will sometimes show a visible break. If bad, replace with a good 20A, 250V fast blow fuse.\n"
            "\n"
            "If LED D15/D7 is on, press the stop button and check the Nodes high power LED is off (LED D23 on standard BLDC board machines, LED D18 on Combo board machines).\n"
            "\n"
            "all brushless board motor fuses (F1 & F3 on standard BLDC board machines [F5 & F6 on Combo board machines]).\n"
            "\n"
            "Check all brake valve fuses (F2, F4, F5 - F8 on standard BLDC board machines [F1, F2, F3 and F4 on Combo board machines]).\n"
            "\n"
            "If any fuse F2, F5 - F8 are blown, the Brushless board needs replaced. Check each brake or valve assembly attached to the offending node (see above). Valve: Ohms check across each valve coil for proper reading. Check each coil pin to ground (should not be shorted). Check valve wiring harness for shorts to ground. Any bad fuse, brake or valve should be replaced.\n"
            "\n"
            "If any fuse F4, F5 - F8 are blown, check each brake or valve assembly attached to the offending node (see above). Valve: Ohms check across each valve coil for proper reading. Check each coil pin to ground (should not be shorted). Check valve wiring harness for shorts to ground. Any bad fuse, brake or valve should be replaced.\n"
            "\n"
            "If F1 or F3 is blown, replace fuse, disconnect the motor from the drive board and visually check the motor wire harness for shorts to ground. Remove the mechanisms drive belt and manually turn the motor shaft. If the shaft does not spin freely or if it feels like it sticks in places, then replace the motor.\n"
            "\n"
            "Check the motor for proper operation by entering Peripherals > Wrapper > Tests select the mechanism. Press the start switch and touch the Cycle button and the mechanism should move freely. If not and the motor has been replaced, replace the brushless driver board.\n" );

        break;

    case 590:

        // serviceNewWrapperErrorHistHelp590
        helpText = tr("\nWarning. Low film on Top roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
        break;

    case 591:

        // serviceNewWrapperErrorHistHelp591
        helpText = tr("\nWarning. Low film on Bottom roll.\n\nPress the STOP button. Remove package from intake. Press the START button to resume wrapping." );
        break;

    case 592:

        // serviceNewWrapperErrorHistHelp592
        helpText = tr(
            "Error Description: Wrap box timings memory error\n"
            "\n"
            "Affected:\n"
            "Nodes 1-5\n"
            "\n"
            "Error Definition:\n"
            "Node ran out of memory while trying to create wrap box or timings sent from console was corrupted.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. If error is \"wrap box timings empty\" nodes 1-5:\n"
            "\n"
            "2. If error is \"wrap box timings corruption\" nodes 1-5: If loading timings from a thumb drive, reboot EPCP and try again. If you encounter the same failure, the timings on the thumb drive are corrupt and should not be used. Try a different set of timings.\n"
            "\n"
            "If changing timings from the console and this error was encountered, reboot EPCP and wrapper and try again. If encountered during normal operations, reboot the wrapper.\n"
            "\n"
            "3. If error is \"missing height box\" nodes 1-5: If loading timings from a thumb drive, reboot EPCP and try again. If you encounter the same failure, the timings on the thumb drive are corrupt and should not be used. Try a different set of timings.\n"
            "\n"
            "If changing timings from the console and this error was encountered, reboot EPCP and wrapper and try again.\n"
            "\n"
            "4. If error is \"too many wrap boxes\" nodes 1-5: If loading timings from a thumb drive, the timings on the thumb driver are corrupted and should not be used. Try a different set of timings.\n"
            "\n"
            "If changing timings from the console and this error was encountered, look at the list if wrap timing trays (Peripherals > Wrapper > Wrapper Timings). You can have a maximum of 32 trays. Delete Trays that are not needed from the list.\n"
            "\n"
            "5. If error is \"missing wrap box\" node 1-5: If loading timings from a thumb drive, the timings on the thumb driver are corrupted and should not be used. Try a different set of timings.\n"
            "\n"
            "If changing timings from the console and this error was encountered, reboot EPCP and wrapper and try again.\n"
            "\n"
            "Any of these errors should be reported to product support along with the wrapper software version.\n" );

        break;

    case 617:

        // serviceNewWrapperErrorHistHelp617
        helpText = tr(
            "Error Description: Wrap box timings flash memory error\n"
            "\n"
            "Affected:\n"
            "Nodes 1-5\n"
            "\n"
            "Error Definition:\n"
            "Node ran out of flash memory while trying to create wrap box or flash memory corruption.\n"
            "\n"
            "Electrical Locations:\n"
            "AWS installation manual chapter 4 AWS electrical manual chapter 1.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot EPCP and wrapper. If error is encountered again, replace the offending CPU (logic) board.\n"
            "\n"
            "2. This error should be reported to product support along with the wrapper version.\n" );

        break;

    case 627:

        // serviceNewWrapperErrorHistHelp627
        helpText = tr(
            "Error Description: Wrap Box Read Buffer Corruption\n"
            "\n"
            "Affected:\n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Console sent wrap timings with a timing value that is out of bounds. These timings were not saved to the Wrapper.\n"
            "\n"
            "Wrap Box Parameter Limits:\n"
            "Total Number of  Wrap Boxes: 34\n"
            "%LIMITS%"
            "\n"
            "Troubleshooting Tips:\n"
            "1. If loading timings from a thumb drive, reboot EPCP and try again. If you encounter the same failure, the timings on the thumb driver are corrupted and should not be used. Try a different set of timings.\n"
            "\n"
            "2. If changing timings from the console and this error was encountered, reboot EPCP and wrapper and try again. This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 628:

        // serviceNewWrapperErrorHistHelp628
        helpText = tr(
            "Error Description: Wrap Box Write Buffer Corruption\n"
            "\n"
            "Affected: \n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Wrapper tried to send wrap timings with a timing value that is out of bounds to the console.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot EPCP and wrapper and try again. If you encounter the same failure, restore factory default wrapper timings.\n"
            "\n"
            "This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 629:

        // serviceNewWrapperErrorHistHelp629
        helpText = tr(
            "Error Description: Too many wrap boxes in read buffer\n"
            "\n"
            "Affected: \n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Console sent wrap timings with too many wrap boxes in the read buffer.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot EPCP and wrapper and try again.\n"
            "\n"
            "This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 630:

        // serviceNewWrapperErrorHistHelp630
        helpText = tr(
            "Error Description: Too many wrap boxes in write buffer\n"
            "\n"
            "Affected: \n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Wrapper tried to send too many wrap boxes to the console.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot EPCP and wrapper and try again. If you encounter the same failure, restore factory default wrapper timings.\n"
            "\n"
            "This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 631:

        // serviceNewWrapperErrorHistHelp631
        helpText = tr(
            "Error Description: Package sensor across elevator is blocked\n"
            "\n"
            "Affected:\n"
            "Package intake\n"
            "\n"
            "Error Definition:\n"
            "The Elevator Package Position Sensor is indicating that it is in the blocked / off condition. See AWS mechanical service manual: Elevator Package Position Sensor.\n"
            "\n"
            "Electrical Locations:\n"
            "\n"
            "Elevator sensor input is located at J15, LVIN4, on Combo driver board for Node 4. \n"
            "Elevator sensor LED D16 on Combo driver board for Node 4.\n"
            "\n"
            "Elevator sensor input is located at J11 pins 7, 8 and 12 on brushless driver board for Node 4.\n"
            "Elevator sensor LED D28 on brushless driver board for Node 4.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Check sensor area of intake for debris that may be blocking the sensor such as film, product, tray, possibly a conveyor belt or gripper hose.\n"
            "\n"
            "2. In Service > Wrapper > Tests > Elevator, check the state of the sensor.  The sensor should display green when on and unblocked, and red when off or blocked.\n"
            "\n"
            "3. Check adjustment of sensor.  A single layer of Styrofoam tray, about 1/4 inch, should not trip the sensor and 2 layers of Styrofoam, about 1/2 inch, MUST trip the sensor.  For more information on the Elevator Package Position Sensor, refer to the Mechanical Service Manual.\n"
            "\n"
            "4. If sensor is not functional, replace the emitter assembly first, test.  Replace the receiver if the sensor is still not functioning.\n" );

        break;

    case 632:

        // serviceNewWrapperErrorHistHelp632
        helpText = tr(
            "Error Description: Wrapper timings failed to update.\n"
            "\n"
            "Affected:\n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Wrapper did not update wrapper timings before update timer expired.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot EPCP and wrapper and try again.\n"
            "\n"
            "This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 633:

        // serviceNewWrapperErrorHistHelp633
        helpText = tr(
            "Error Description: Wrapper timings update memory failure.\n"
            "\n"
            "Affected:\n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Wrapper ran out of memory while trying to update wrapper timings.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Reboot wrapper and try again.\n"
            "\n"
            "This error should be reported to product support along with the EPCP software version and wrapper software version.\n" );

        break;

    case 634:

        // serviceNewWrapperErrorHistHelp634
        helpText = tr(
            "Error Description: CAN Communications failure\n"
            "\n"
            "Affected:\n"
            "Node boards 1,3,4,5\n"
            "\n"
            "Error Definition:\n"
            "Node stopped talking on the CAN bus.\n"
            "\n"
            "Electrical Locations:\n"
            "See service manual chapter 6.\n"
            "\n"
            "CAN bus terminator between Nodes 2 and 4.\n"
            "\n"
            "Troubleshooting Tips:\n"
            "1. Verify that the offending Node board LED D2 [D1 on Combo board] is flashing. If not, verify step 3 then cycle the power to the wrapper and report this error to product support.\n"
            "\n"
            "2. Inspect the CAN socket and plug (J14 located on the brushless motor board [J21 on the Combo board])  that the connector terminals are making proper contact and are not loose or corroded.\n"
            "\n"
            "3. Check if the offending Node board is correctly seated into its brushless motor driver board connector. If board is not seated properly, turn off power to the wrapper and re-seat the board.\n"
            "\n"
            "4. Visually check the 15Vdc cable assembly for any cuts or defects that could cause a short to the frame.\n"
            "\n"
            "5. If no electrical problem is found, please report this error to product support along with the Node number and wrapper software version.\n" );

        break;

    case 638:

        // serviceNewWrapperErrorHistHelp638
        helpText = tr( "Pusher slow.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button.\n\nIf problem persists please call service." );
        break;

    case 639:

        // serviceNewWrapperErrorHistHelp639
        helpText = tr(
            "Error Description: Wrapper box timings corruption.\n"
            "\n"
            "Affected:\n"
            "Node 2\n"
            "\n"
            "Error Definition:\n"
            "Wrapper wrap box timings are corrupted.\n" );

        break;

    case 651:

        // serviceNewWrapperErrorHistHelp651
        helpText = tr( "Sealer belt motor move error." );
        break;

    case 652:

        // serviceNewWrapperErrorHistHelp652
        helpText = tr( "\nThe Primary label is still at the printer." );
        break;

    case 653:

        // serviceNewWrapperErrorHistHelp653
        helpText = tr( "\nThe Second label is still at the printer.");
        break;

    case 654:

        // serviceNewWrapperErrorHistHelp654
        helpText = tr( "The wrap timings are not compatible with the selected model. Check model selection in wrapper system info." );
        break;

    case 655:

        // serviceNewWrapperErrorHistHelp655
        helpText = tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 656:

        // serviceNewWrapperErrorHistHelp656
        helpText = tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 657:

        // serviceNewWrapperErrorHistHelp657
        helpText = tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 658:

        // serviceNewWrapperErrorHistHelp658
        helpText = tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 659:

        // serviceNewWrapperErrorHistHelp659
        helpText = tr( "Wrapper status\n\nGripper Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 660:

        // serviceNewWrapperErrorHistHelp660
        helpText = tr( "Wrapper status\n\nGripper Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 661:

        // serviceNewWrapperErrorHistHelp661
        helpText = tr( "Wrapper status\n\nSide Clamp Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 662:

        // serviceNewWrapperErrorHistHelp662
        helpText = tr( "Wrapper status\n\nSide Clamp Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 663:

        // serviceNewWrapperErrorHistHelp663
        helpText = tr( "Wrapper status\n\nSide Clamp Failed to Close.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 664:

        // serviceNewWrapperErrorHistHelp664
        helpText = tr( "Wrapper status\n\nSide Clamp Failed to Open.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 665:

        // serviceNewWrapperErrorHistHelp665
        helpText = tr( "Wrapper status\n\nUpper Film Roll Failed to Switch.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 666:

        // serviceNewWrapperErrorHistHelp666
        helpText = tr( "Wrapper status\n\nLower Film Roll Failed to Switch.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 667:

        // serviceNewWrapperErrorHistHelp667
        helpText = tr( "Wrapper status\n\nFilm Selector Lever is unlatched.\n\nPress the STOP button. Engage Film Selector Lever. Press the START button." );
        break;

    case 668:

        // serviceNewWrapperErrorHistHelp668
        helpText = tr( "Wrapper status\n\nPrimary Applier Wand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 669:

        // serviceNewWrapperErrorHistHelp669
        helpText = tr( "Wrapper status\n\nPrimary Applier Wand Failed to Move.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 670:

        // serviceNewWrapperErrorHistHelp670
        helpText = tr( "Wrapper status\n\nPrimary Applier Hand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 671:

        // serviceNewWrapperErrorHistHelp671
        helpText = tr( "Wrapper status\n\nPrimary Applier Wand Vacuum Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 672:

        // serviceNewWrapperErrorHistHelp672
        helpText = tr( "Wrapper status\n\nAir Compressor Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 673:

        // serviceNewWrapperErrorHistHelp673
        helpText = tr( "Wrapper status\n\nRegulated Air Pressure Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 674:

        // serviceNewWrapperErrorHistHelp674
        helpText = tr( "Wrapper status\n\nActuator Air Pressure Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 675:

        // serviceNewWrapperErrorHistHelp675
        helpText = tr( "Wrapper status\n\nUpper Film Pull not Detected.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 676:

        // serviceNewWrapperErrorHistHelp676
        helpText = tr( "Wrapper status\n\nLower Film Pull not Detected.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 677:

        // serviceNewWrapperErrorHistHelp677
        helpText = tr( "Wrapper status\n\n24V Power Supply Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 678:

        // serviceNewWrapperErrorHistHelp678
        helpText = tr( "Wrapper status\n\n170V Inrush Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 679:

        // serviceNewWrapperErrorHistHelp679
        helpText = tr( "Wrapper status\n\n170V Power Supply Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 680:

        // serviceNewWrapperErrorHistHelp680
        helpText = tr( "Wrapper status\n\nUpper Film Brake Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 681:

        // serviceNewWrapperErrorHistHelp681
        helpText = tr( "Wrapper status\n\nLower Film Brake Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 682:

        // serviceNewWrapperErrorHistHelp682
        helpText = tr( "Wrapper status\n\nUpper Film Roll is Loose.\n\nThe Upper Film Roll is loose on the film Arbor.\n\nPlease tighten the Film Arbor knob, turning clockwise.\n\nEnsure the core grip is tight to the film core." );
        break;

    case 683:

        // serviceNewWrapperErrorHistHelp683
        helpText = tr( "Wrapper status\n\nLower Film Roll is Loose.\n\nThe Lower Film Roll is loose on the film Arbor.\n\nPlease tighten the Film Arbor knob, turning clockwise.\n\nEnsure the core grip is tight to the film core." );
        break;

    case 684:

        // serviceNewWrapperErrorHistHelp684
        helpText = tr( "Wrapper status\n\nPrinter Dock Failure.\n\nPlease make sure printer is docked in proper position." );
        break;

    case 685:

        // serviceNewWrapperErrorHistHelp685
        helpText = tr( "Wrapper status\n\nPackage Sensor across Sealer Belt is blocked.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Clear any obstructions from the package sensor. Press the START button." );
        break;

    case 686:

        // serviceNewWrapperErrorHistHelp686
        helpText = tr( "Wrapper status\n\nPress Start Button and then place package on Intake." );
        break;

    case 687:

        // serviceNewWrapperErrorHistHelp687
        helpText = tr( "Wrapper status\n\nIntake Package Sensor is blocked. Please make sure sensor is clean and free of debris. \n\nIf problem persists please call service." );
        break;

    case 688:

        // serviceNewWrapperErrorHistHelp688
        helpText = tr( "Regulated air pressure is too high. Adjust air regulator to within the range 45-55 psi psi. " );
        break;

    case 689:

        helpText = tr( "Wrapper status\n\nSecondary Applier Wand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 690:

        helpText = tr( "Wrapper status\n\nSecondary Applier Wand Failed to Move.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 691:

        helpText = tr( "Wrapper status\n\nSecondary Applier Hand Failed to Home.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 692:

        helpText = tr( "Wrapper status\n\nSecondary Applier Wand Vacuum Failure.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    case 693:

        helpText = tr( "Wrapper status\n\nSlide Adjustment Timeout.\n\nPress the STOP button. Remove any packages, labels, debris in the machine. Press the START button." );
        break;

    default:

        // serviceNewWrapperErrorHistHelpU
        helpText = tr(
            "Error Description: Unknown Wrapper Error\n"
            "\n"
            "Affected:\n"
            "N/A\n"
            "\n"
            "Error Definition:\n"
            "Wrapper sent an error that was not recognized.\n" );

        break;
    }

    return helpText;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperErrorHistory::insertHtml( QTextEdit *edit, const QString& filename, const QMap<QString, QString>& translations ) const
{
    QFile f( filename );

    // load html from resource
    if ( !f.open( QFile::ReadOnly ) )
        return false;

    QString html( f.readAll() );

    // replace all embedded text strings
    for ( QMap<QString, QString>::const_iterator i( translations.begin() ); i != translations.end(); ++i )    //  HTA-5
    {
        const QString pattern( QString( "<% write(%1); %>" ).arg( i.key() ) );

        html.replace( pattern, i.value() );
    }

    bool isReadOnly( edit->isReadOnly() );

    // insert the html
    edit->setReadOnly( false );
    edit->insertHtml( html );

    // remove the keyword
    QTextCursor c = edit->textCursor();
    c.select( QTextCursor::WordUnderCursor );
    edit->setTextCursor( c );

    // restore state
    edit->setReadOnly( isReadOnly );
    edit->moveCursor( QTextCursor::Start );

    f.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperErrorHistory::checkFileName( QString& myfile, int fileOrFolder ) const
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            //else if(fileOrFolder == 2)
            //    ; //myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                //else if( myfile.indexOf("C:") > 0 )     // cannot contain C:
                //    errStr = tr("The file cannot be saved to the root file system");
                //else if( isLinux() && myfile.indexOf("media") < 0 )  // has to contain media (a path to a thumb)
                //    errStr = tr("The file cannot be saved to the root file system");
                else
                    validFile = true;
            }
        }
    }

    if ( !validFile )
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "ERROR SAVING FILE" ) );
        dialog.setText( errStr );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();
    }

    return validFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString serviceNewWrapperErrorHistory::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if( myfile.indexOf(".csv") > 0 )
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .csv
            strippedFileName = myfile;                              // then we are good to go
        }
        else if(myfile.indexOf(".") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path, just the file.
            // An extension was added to the file, but we don't recogonize it, but include it.
            int getDot = myfile.indexOf(".") + 1;
            if (myfile.length() == getDot + 3)
                strippedFileName = myfile;
        }
        else
        {
            // we don't have a file, so create one. Do not include the entire path of the file.
            // No extension was given for the file, so default to the file to an csv file ext.
            strippedFileName = myfile;
        }

        retFile = strippedFileName;
    }
    return retFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperErrorHistory::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::exportErrorHistory() const
{
    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    FileDialog filepicker( mainWindow_ );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save to USB" ) );

    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            QString header;
            header.append( "Code,\"Desc\",PkgLength,PkgWidth,PkgHeight,FilmUsed,BestFit,PerNet,\"TrayName\",\"DateTime\"");

            QFile f( fileName );

            if ( f.open( QFile::WriteOnly ) )
            {
                f.write( qPrintable( header ) );
                f.write( "\n" );

                foreach( const QString& err, errorStr_ )
                {
                    f.write( qPrintable( err ) );
                    f.write( "\n" );
                }

                f.close();

                if ( isLinux() )
                    system( "sync" );         // flush the file to disc
            }
            else
            {
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setTitle( tr( "ERROR EXPORTING HISTORY" ) );
                dialog.setText( tr( "Error exporting history to USB drive. Please check your drive and try again." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperErrorHistory::fakeData()
{
    DomEvolWrapperErrorsList m;

    m.setTotalErrors(3);
    m.firstErrorNum() = "0";
    m.setNumOfErrCode(3);
    m.evolErrCode() = "517,688,688";
    m.setNumOfErrDesc(2);
    m.evolErrDesc() = "\"Primary conveyor jammed\",\"Regulated Air Pressure too high\",\"Regulated Air Pressure too high\"";
    m.setNumOfErrPkgLength(3);
    m.evolErrPkgLength() = "0.0,0.0,0.0";
    m.setNumOfErrPkgWidth(3);
    m.evolErrPkgWidth() = "0.0,0.0,0.0";
    m.setNumOfErrPkgHeight(3);
    m.evolErrPkgHeight() = "0.0,0.0,0.0";
    m.setNumOfErrFilmUsed(3);
    m.evolErrFilmUsed() = "0,0,0";
    m.setNumOfErrBestFit(3);
    m.evolErrBestFit() = "0,0,0";
    m.setNumOfErrPerNet(3);
    m.evolErrPerNet() = "0,0,0";
    m.setNumOfErrTrayName(3);
    m.evolErrTrayName() = "\"\",\"\",\"\"";
    m.setNumOfErrDateTime(2);
    m.evolErrDateTime() = "\"05/07/18 ,16:18:27\",\"05/04/18 ,13:59:32\",\"05/03/18 ,16:35:15\"";

    onSessionMessage( m );

}
