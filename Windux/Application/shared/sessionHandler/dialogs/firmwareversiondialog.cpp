/**
 * @file firmwareversiondialog.cpp
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe Crew
 */

#include "firmwareversiondialog.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <filesystem.h>
#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>
#include <logmanager.h>

#include "ui_firmwareversiondialog.h"

const static int MAX_MESSAGES = 8;

FirmwareVersionDialog::FirmwareVersionDialog(QWidget *parent, Qt::WindowFlags f) :
    _Mybase( CenterDialogLocation, parent, f ),
    ui(new Ui::FirmwareVersionDialog),
    weigherVer_( "0.0.0" ),
    applicationVer_( "0.0.0" ),
    tofVer_( "0.0.0" ),
    tofBootVer_( "0.0.0" ),
    receivedBoot_( false ),
    cycleCounts_( "0" ),
    printerNumber_( 0 ),
    printerOneLabelCounts_( "0" ),
    printerTwoLabelCounts_( "0" ),
    printerThreeLabelCounts_( "0" ),
    callTheseJustOneTimeEver_( true )
{
    ui->setupUi(this);

    translate();

    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FirmwareVersionDialog::~FirmwareVersionDialog()
{
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::translate()
{
    // FT-656
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::updateModelName(int modelNumber)
{
    switch( modelNumber )
    {
        case 1 : modelName_ = "AWS-1RL";   break;
        case 2 : modelName_ = "AWS-1LR";   break;
        case 3 : modelName_ = "AWS-MLR";   break;
        case 6 : modelName_ = "AWS-1BRL";  break;
        case 7 : modelName_ = "AWS-1BLR";  break;
        case 8 : modelName_ = "AWS-2RL";   break;
        case 9 : modelName_ = "AWS-2LR";   break;
        case 10: modelName_ = "AWS-3RL";   break;
        case 11: modelName_ = "AWS-3LR";   break;
        case 12: modelName_ = "EWS-3RL";   break;
        case 13: modelName_ = "EWS-3LR";   break;
        case 14: modelName_ = "NGW2-RL";   break;
        case 15: modelName_ = "NGW2-LR";   break;
        case 16: modelName_ = "NGW1-RL";   break;
        case 17: modelName_ = "NGW1-LR";   break;
        case 20: modelName_ = "NGW2-S2RL"; break;
        case 21: modelName_ = "NGW2-S2LR"; break;
        case 22: modelName_ = "NGW1-S2RL"; break;
        case 23: modelName_ = "NGW1-S2LR"; break;
        default: modelName_ = tr("Unknown");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::initialize()
{   
    // set up vars
    node.append( ui->lblWrapperNode0Title );
    node.append( ui->lblWrapperNode1Title );
    node.append( ui->lblWrapperNode2Title );
    node.append( ui->lblWrapperNode3Title );
    node.append( ui->lblWrapperNode4Title );
    node.append( ui->lblWrapperNode5Title );

    application.append( ui->lblWrapperNode0 );
    application.append( ui->lblWrapperNode1 );
    application.append( ui->lblWrapperNode2 );
    application.append( ui->lblWrapperNode3 );
    application.append( ui->lblWrapperNode4 );
    application.append( ui->lblWrapperNode5 );

    bootloader.append( ui->lblBootloader0 );
    bootloader.append( ui->lblBootloader1 );
    bootloader.append( ui->lblBootloader2 );
    bootloader.append( ui->lblBootloader3 );
    bootloader.append( ui->lblBootloader4 );
    bootloader.append( ui->lblBootloader5 );

    ui->butPrint->setEnabled( false );

    connect(ui->butPrint,          SIGNAL( clicked() ),      this,   SLOT( onPrintButtonClicked() ) );

    internalConfig_ = &session_->scaleConfig->internalConfig;

    // initialize wrapper model name and serial number to unknown
    modelName_ = "Unknown";
    serialNumber_ = "Unknown";

    /// send messages

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    updateVersionInfo( SysUsbWgStatus );                        // #1 2,20:  if it changes, then we will use it
    updateVersionInfo( SnVersionInfo );                         // #2 100,2: use cached data immediately

    ScaleConfiguration *config = session_->scaleConfig;
    ui->lblPrinterOne->setText( config->printerOneVer );
    ui->lblPrinterTwo->setText( config->printerTwoVer );
    ui->lblPrinterThree->setText( config->printerTreVer );

    ui->lblPrinter0Counts->setText(  internalConfig_->printerCycleCounts.at( 0 )  );
    ui->lblPrinter1Counts->setText( internalConfig_->printerCycleCounts.at( 1 )  );
    ui->lblPrinter2Counts->setText( internalConfig_->printerCycleCounts.at( 2 )  );

    // moved the messaging that contacts flash downt to onSessionMessaging

    // request wrapper info
    session_->send(WrapperSystem, WrSysReqStatus);              // #6 REQ 10,13 GET 10,21 - Fast as is

    // For real time Cycle Counter updates
    session_->send(Wrapper, WrReqEvolSystemInfo);               // #8 REQ 9,143 GET 9,144

    session_->send(Wrapper, WrReqCalibration, QString::number(2)); // REQ 9,93 2nd applier app info- GETS NO REPLY

    // get weigher version
    session_->send( WeigherSystem, SysReqStatus );              // #1 REQ 2,13 GET 2,20 (WeigherSystem,SysUsbWgStatus)

    // Lbl Cts for Printer One to begin this little dance
    session_->send( MfgTestDev, RequestPrinterLabelCount, "0" ); // #7 164,23 GET 164,24 - and reply triggers REQs for printers 2 and 3

    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        ui->lblTOFBootLoader->setVisible( true );
        ui->lblTofSW->setVisible( true );
        ui->lblTOF->setVisible( true );
        ui->lblCtsTOF->setVisible( true );
    }
    else
    {
        ui->lblTOFBootLoader->setVisible( false );
        ui->lblTofSW->setVisible( false );
        ui->lblTOF->setVisible( false );
        ui->lblCtsTOF->setVisible( false );
    }   
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO("FirmwareVersionDialog", "onSessionMessageReceive");

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if (Wrapper == device && WrEvolSystemInfo == event)
    {
        // Real time Cycle Counts
        DomEvolWrapperSystemInformation m(msg);

        cycleCounts_ = m.awsWrapperCycleCount();
        ui->lblCycleCounter->setText( m.awsWrapperCycleCount() );
        serialNumber_ = m.awsSerialNumber();
        updateModelName( m.awsModelNumber() );

        ui->lblSerialNumber->setText( serialNumber_ );
        ui->lblModelName->setText( modelName_ );
    }
    else if ((WeigherSystem == device ) && ( SysUsbWgStatus == event ))
    {
        updateVersionInfo( SysUsbWgStatus );            // if it changes, then we will use it
        ui->butPrint->setEnabled( true );               // all msgs received since this is the last one
    }

    else if ((PrinterSystem == device ) && ( PrSysStatus == event ))
    {
        updateVersionInfo( PrSysStatus );
    }

    else if ( ( WrapperSystem == device ) && ( SysEvolWrSysStatus == event ) )
    {
        // EXAMPLE MESSAGE:
        // Update: 10,21,0000,5,"2,1,3,4,5",5,"9.4,9.4,9.4,9.4,9.4",5,"1.50,1.50,1.50,1.50,1.50",5,"1.9,1.9,1.9,1.9,1.9",5,"0010,0010,0010,0010,0010",5,"0000,0000,0000,0000,0000",5,"0000,0000,0000,0000,0000",5,"12C132,12C132,12C132,12C132,12C132"
        // With Camera Added
        // Update: 10,21,0000,6,"2,1,0,3,4,5",6,"8.47,8.47,9.1,8.47,8.47,8.47",6,"1.50,0.0,0.0,0.0,1.50,0.0",6,"3.0,3.0,1.0,3.0,3.0,3.0",6,"001B,001B,0000,001B,001B,001B",6,"000F,000F,0000,000F,000F,000F",6,"0000,0000,0000,0000,0000,0000",6,"8E4D,8E4D,0000,8E4D,8E4D,8E4D"

        DomEvolWrapperSystemStatus m( msg );

        // We have enough data so setup the lists
        const int numOfItems( m.numOfItems().toInt() );

        const QStringList nodeList( m.nodeList().split(",") );

        QString tmpVerList = m.appVersionList();
        appVersionList = tmpVerList.split(",");
        appVersionListSort = appVersionList;

        QString tmpBootList = m.bootVersionList();
        bootVersionList = tmpBootList.split(",");
        bootVersionListSort = bootVersionList;

        if( (0 < numOfItems) && (numOfItems <= 6 ) &&
            ( numOfItems == appVersionList.size() ) && ( numOfItems == nodeList.size() ) )

        {
            QMap<int,int> nodeMapping; // node id, array index

            // display nodes in order of node id
            int n( 0 );

            // map node id's to array index
            for ( int i( numOfItems ); i--; )
                nodeMapping.insert( nodeList[i].toInt(), i );

            foreach ( int i, nodeMapping.values() )
            {
                application[n]->setText( appVersionList[i] );
                bootloader[n]->setText( bootVersionList[i]);        // just boot strapping onto app version

                // Used to print the Firmware version report
                appVersionListSort[n] = appVersionList[i];
                bootVersionListSort[n] = bootVersionList[i];

                ++n;
            }
        }
    }

    else if (( SessionDev == device ) && ( SnVersionInfo == event ))
    {
        updateVersionInfo( SnVersionInfo );                 // use cached data immediately
    }

    else if ((MfgTestDev == msg[0].toInt() ) && ( PrinterLabelCount == msg[1].toInt() ))
    {
        DomPrinterLabelCount m( msg );

        if( 0 == printerNumber_ )
        {
            printerOneLabelCounts_ = QString::number( m.count() );     // for printing
            ui->lblPrinter0Counts->setText(  printerOneLabelCounts_  );
            internalConfig_->printerCycleCounts[ printerNumber_ ] = printerOneLabelCounts_;
            printerNumber_++;
            session_->send( MfgTestDev, RequestPrinterLabelCount, QString::number( printerNumber_ ) ); // 164,23
        }
        else if( 1 == printerNumber_ )
        {
            printerTwoLabelCounts_ = QString::number( m.count() );     // for printing
            ui->lblPrinter1Counts->setText( printerTwoLabelCounts_  );
            internalConfig_->printerCycleCounts[ printerNumber_ ] = printerTwoLabelCounts_;
            printerNumber_++;
            session_->send( MfgTestDev, RequestPrinterLabelCount, QString::number( printerNumber_ ) ); // 164,23
        }
        else if( 2 == printerNumber_ )
        {
            printerThreeLabelCounts_ = QString::number( m.count() );     // for printing
            internalConfig_->printerCycleCounts[ printerNumber_ ] = printerThreeLabelCounts_;
            printerNumber_++;
            ui->lblPrinter2Counts->setText( printerThreeLabelCounts_  );
        }

        // Last message has been processed, so now no more blocking so ask for these
        if( callTheseJustOneTimeEver_ )
        {
            callTheseJustOneTimeEver_ = false;

            // get the information for the first printer
            session_->send( PrinterSystem, PrSysReqStatus, "0");        // #3 REQ 8,13 GET 8,4 - Fast as is

            // get the information for the second printer
            session_->send( PrinterSystem, PrSysReqStatus, "1");        // #4 REQ 8,13 GET 8,4 - Fast as is

            // get the information for the third printer
            session_->send( PrinterSystem, PrSysReqStatus, "2");        // #5 REQ 8,13 GET 8,4 - Fast as is

            // request version info - moved to bottom - REALLLLLLY SLOW to return and blocks - put it last
            session_->send( SessionDev, SnVersionInfo );                // #2 REQ 100,2 - GET 100,2 (SessionDev, SnVersionInfo)
        }

    }
    else if(msg[0].toInt() == Wrapper && msg[1].toInt() == WrPPPSensorVersion)
    {
        QString sof_ver( "0.0.0" );
        QString boot_ver( "0.0.0" );

        int len = msg.length();
        if( ( msg[2].toInt() > 0 ) && ( len >= 4 ) )
        {
            if(  ( msg[3] != sof_ver ) )
        {
                sof_ver = msg[3];
            }
            ui->lblTofSW->setText( sof_ver );
        }

        if(  ( 0 == msg[2].toInt() ) && ( len >= 6 ) )
        {
            if(  msg[5] != boot_ver )
            {
                boot_ver = msg[5];
        }
            ui->lblTOFBootLoader->setText( boot_ver );
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::updateVersionInfo( int updateInfo )
{
    QString tmpStr( "" );


    if( SysUsbWgStatus == updateInfo )      // 2,20
    {
        DomUsbWeigherPowerupTests *weigherPowerupTests = &session_->scaleConfig->weigherPowerupTests;

        // Weigher Software Version
        if( ui->lblWeigher->text() != weigherPowerupTests->weigherSoftwareVersion() )
        {
            ui->lblWeigher->setText( weigherPowerupTests->weigherSoftwareVersion() );
            weigherVer_ = weigherPowerupTests->weigherSoftwareVersion();
        }

        msg_counter_+=1;

        // USB Weigher - WeigherKind = 2, 3, 4
        MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
        QString weigherKind = QString::number( periphSettings->firstWeigher() );
        if(weigherKind != "1" && weigherKind != "2" && weigherKind != "3" && weigherKind != "4")
        {
            tmpStr = tr( "No Weigher Attached" );
            if( ui->lblWeigher->text() != tmpStr )
                ui->lblWeigher->setText( tmpStr );
        }

    }
    else if( SnVersionInfo == updateInfo )
    {
        DomCurrentScaleSetup *versioninfo_ = &session_->scaleConfig->currentScaleSetup;

        if( ui->lblApplication->text() != versioninfo_->version() )
            ui->lblApplication->setText( versioninfo_->version() );
    }
    else if( PrSysStatus == updateInfo )
    {
        DomPrinterPowerupTests *versioninfo_ = &session_->scaleConfig->printerPowerupTests;
        QString printerType = versioninfo_->printerType();
        ScaleConfiguration *config = session_->scaleConfig;
        // There could be up to 3 printers ...
        if( printerType == "0" )
        {
            QString test_cached = config->printerOneVer;
            QString test_mesged = versioninfo_->softwareVersion();
            QString test_screen = ui->lblPrinterOne->text();

            config->printerOneVer = versioninfo_->softwareVersion();
            if( ui->lblPrinterOne->text() != config->printerOneVer )
                ui->lblPrinterOne->setText( config->printerOneVer );
        }
        else if( printerType == "1" )
        {
            config->printerTwoVer = versioninfo_->softwareVersion();
            if( ui->lblPrinterTwo->text() != config->printerTwoVer)
                ui->lblPrinterTwo->setText( config->printerTwoVer );

        }
        else if(printerType == "2" )
        {
            config->printerTreVer = versioninfo_->softwareVersion();
            if( ui->lblPrinterThree->text() != config->printerTreVer )
                ui->lblPrinterThree->setText( config->printerTreVer );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FirmwareVersionDialog::onPrintButtonClicked()
{
    //------------------------------------------------
    // Send the command to print the Firmware Versions
    //------------------------------------------------

    ScaleConfiguration *config = session_->scaleConfig;

    QString title( "FIRMWARE VERSIONS" );

    QString msg = "\n";

    msg +=  "Date:  ";
    msg += QDate::currentDate().toString("MM/dd/yyyy  ");
    msg += QTime::currentTime().toString("hh:mm:ss AP");
    msg += "\n";

    msg += "Wrapper Model:  ";
    msg += modelName_;
    msg += "\n";

    msg += "Wrapper Serial:  ";
    msg += serialNumber_;
    msg += "\n";

    msg += "Weigher:  ";
    msg += weigherVer_;
    msg += "\n";

    msg += "Application:  ";
    msg += applicationVer_;
    msg += "\n";

    msg += "Primary Printer:  ";
    msg += config->printerOneVer;
    msg += "\n";

    msg += "Second Printer:  ";
    msg += config->printerTwoVer;
    msg += "\n";

    msg += "Third Printer:  ";
    msg += config->printerTreVer;
    msg += "\n";

    msg += "Camera Node/Boot:   ";
    if( appVersionListSort.size() >= 1 )
        msg += appVersionListSort[0] + " / " + bootVersionListSort[0];
    else
        msg += "0.0.0 / 0.0.0";

    msg += "\n";

    msg += "Wrapper Node/Boot 1:  ";
    if( appVersionListSort.size() >= 2 )
        msg += appVersionListSort[1] + " / " + bootVersionListSort[1];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += "Wrapper Node/Boot 2:  ";
    if( appVersionListSort.size() >= 3 )
        msg += appVersionListSort[2] + " / " + bootVersionListSort[2];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += "Wrapper Node/Boot 3:  ";
    if( appVersionListSort.size() >= 4 )
        msg += appVersionListSort[3] + " / " + bootVersionListSort[3];
    else
        msg += "0.0.0 / 0.0.0";;
    msg += "\n";

    msg += "Wrapper Node/Boot 4:  ";
    if( appVersionListSort.size() >= 5 )
        msg += appVersionListSort[4] + " / " + bootVersionListSort[4];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += "Wrapper Node/Boot 5:  ";
    if( appVersionListSort.size() >= 6 )
        msg += appVersionListSort[5] + " / " + bootVersionListSort[5];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += "Cycle Counts:  ";
    msg += cycleCounts_;
    msg += "\n";

    msg += "Printer One Labels:  ";
    msg += printerOneLabelCounts_;
    msg += "\n";

    msg += "Printer Two Labels:  ";
    msg += printerTwoLabelCounts_;
    msg += "\n";

    msg += "Printer Three Labels:  ";
    msg += printerThreeLabelCounts_;
    msg += "\n";

    msg += "Second Applier App/Boot: ";
    msg += tofVer_ + " / " + tofBootVer_;
    msg += "\n";

    session_->send( ReportManagerDev, StartReport,"2,0,0,0,0,\"" + title + "\",\"" + msg + "\""  );
}
