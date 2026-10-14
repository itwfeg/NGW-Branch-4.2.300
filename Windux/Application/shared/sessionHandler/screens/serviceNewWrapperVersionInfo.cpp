/* @file serviceNwWrapperVersionInfo.cpp
 * serviceNewWrapperVersionInfo
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Shows Wrapper Version info
 */

#include "serviceNewWrapperVersionInfo.h"
#include "ui_serviceNewWrapperVersionInfo.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

serviceNewWrapperVersionInfo::serviceNewWrapperVersionInfo(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperVersionInfo)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();
}

bool serviceNewWrapperVersionInfo::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

serviceNewWrapperVersionInfo::~serviceNewWrapperVersionInfo()
{
    delete ui;
}

int serviceNewWrapperVersionInfo::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceNewWrapperVersionInfo::initialize()
{
    hasChanged = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    scaleModel = scaleConfigData->scaleModel();

    node.append( ui->lblNode1Node );
    node.append( ui->lblNode2Node );
    node.append( ui->lblNode3Node );
    node.append( ui->lblNode4Node );
    node.append( ui->lblNode5Node );
    node.append( ui->lblNode6Node );

    application.append( ui->lblNode1Application );
    application.append( ui->lblNode2Application );
    application.append( ui->lblNode3Application );
    application.append( ui->lblNode4Application );
    application.append( ui->lblNode5Application );
    application.append( ui->lblNode6Application );

    boot.append( ui->lblNode1Boot );
    boot.append( ui->lblNode2Boot );
    boot.append( ui->lblNode3Boot );
    boot.append( ui->lblNode4Boot );
    boot.append( ui->lblNode5Boot );
    boot.append( ui->lblNode6Boot );

    checksum.append( ui->lblNode1Checksum );
    checksum.append( ui->lblNode2Checksum );
    checksum.append( ui->lblNode3Checksum );
    checksum.append( ui->lblNode4Checksum );
    checksum.append( ui->lblNode5Checksum );
    checksum.append( ui->lblNode6Checksum );

    session_->send(WrapperSystem, WrSysReqStatus);
    session_->send(Wrapper, WrReqEvolSystemInfo);               // real time cycle counts
    //session_->send(MfgTestDev,RequestWrapperCycleCount);      // flash cycle counts

    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        session_->send(Wrapper, WrReqCalibration, QString::number(2)); // request TOF sensor version
    }
    else
    {
        // SOF-4995: Hide fields for TOF versions

        ui->label_9->setVisible( false );
        ui->label_10->setVisible( false );
        ui->lblTofSoftwareVersion->setVisible( false );
        ui->lblTofHardwareVersion->setVisible( false );
    }

    update();
}

void serviceNewWrapperVersionInfo::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperVersionInfo::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperVersionInfo::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperVersionInfo::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // 10,21
    if (( WrapperSystem == device ) && ( SysEvolWrSysStatus == event ))
    {
        // EXAMPLE MESSAGE:
        // Update: 10,21,0000,5,"2,1,3,4,5",5,"9.4,9.4,9.4,9.4,9.4",5,"1.50,1.50,1.50,1.50,1.50",5,"1.9,1.9,1.9,1.9,1.9",5,"0010,0010,0010,0010,0010",5,"0000,0000,0000,0000,0000",5,"0000,0000,0000,0000,0000",5,"12C132,12C132,12C132,12C132,12C132"
		// With Camera Added
        //               err  #  nodes        # appVer                         #  dataVer                    #  bootVer                  # bitErr                          #  devInitFlags                   #  errorFlags                     #  wrapBoxChecksum
		// Update: 10,21,0000,6,"2,1,0,3,4,5",6,"8.47,8.47,9.1,8.47,8.47,8.47",6,"1.50,0.0,0.0,0.0,1.50,0.0",6,"3.0,3.0,1.0,3.0,3.0,3.0",6,"001B,001B,0000,001B,001B,001B",6,"000F,000F,0000,000F,000F,000F",6,"0000,0000,0000,0000,0000,0000",6,"8E4D,8E4D,0000,8E4D,8E4D,8E4D"

        DomEvolWrapperSystemStatus m( msg );

        // We have enough data so setup the lists
        const int numOfItems( m.numOfItems().toInt() );

        const QStringList nodeList( m.nodeList().split(",") );
        const QStringList appVersionList( m.appVersionList().split(",") );
        const QStringList bootVersionList( m.bootVersionList().split(",") );
        const QStringList wrapBoxChecksumList( m.wrapBoxChecksumList().split(",") );

        ui->lblReportSystemErrorValue->setText( m.reportSysError() );

        if (( 0 < numOfItems ) &&
            ( numOfItems <= 6 ) &&
            ( numOfItems == nodeList.size() ) &&
            ( numOfItems == appVersionList.size() ) &&
            ( numOfItems == bootVersionList.size() ) &&
            ( numOfItems == wrapBoxChecksumList.size() ))
        {
            QMap<int,int> nodeMapping; // node id, array index

            // map node id's to array index
            for ( int i( numOfItems ); i--; )
                nodeMapping.insert( nodeList[i].toInt(), i );

            // display nodes in order of node id
            int n( 0 );

            foreach ( int i, nodeMapping.values() )
            {
                // for camera board (node 0) we want it to say camera not a number,
                // and now Im being told its node 6, not 0. Im so confused
                if ( 0 == n )
                    node[n]->setText(tr("Camera"));
                else
                    node[n]->setText( nodeList[i] );

                application[n]->setText( appVersionList[i] );
                boot[n]->setText( bootVersionList[i] );

                // for camera board (node 0) the checksum for wrap boxes is n/a
                if ( 0 == n )
                    checksum[n]->setText( tr( "n/a" ) );
                else
                    checksum[n]->setText( wrapBoxChecksumList[i] );

                ++n;
            }
        }
    }

    // (164,27)
    // flash cycle counts - static
    //else if (( MfgTestDev == device ) && ( WrapperCycleCount == event ))
    //{
    //    DomWrapperCycleCount m( msg );
    //
    //    ui->lblWrapperCycleCount->setText( QString::number( m.count() ) );
    //}
    //
    // (9,143)
    // real time cycle counts
    else if (Wrapper == device && WrEvolSystemInfo == event)
    {
        DomEvolWrapperSystemInformation m(msg);

        ui->lblWrapperCycleCount->setText(m.awsWrapperCycleCount());
    }
    else if (Wrapper == device && WrPPPSensorVersion == event)
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            QString softwareVersion = msg[3];
            QString hardwareVersion = msg[4];

            ui->lblTofSoftwareVersion->setText( softwareVersion );
            ui->lblTofHardwareVersion->setText( hardwareVersion );
        }
    }
}

bool serviceNewWrapperVersionInfo::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}

