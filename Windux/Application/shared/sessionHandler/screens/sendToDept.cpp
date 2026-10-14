/* @file sendToDept.cpp
 * sendToDept
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd Davis
 */

#include <logmanager.h>

#include "sendToDept.h"
#include "ui_sendToDept.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <ScaleConfiguration.h>
#include "python.h"

#include <QtGui>
#include <QtCore>
#include <QEvent>
#include <QIntValidator>
#include <Qt>
#include <QTimer>
#include <QProcess>


static const int NOT_CHECKED = 0;
static const int CHECKED = 2;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

sendToDept::sendToDept(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::sendToDept)
{
    ui->setupUi(this);
    ui->framesendToDept->show();
    ui->frameSplashWait->show();
    primaryScaleHostname = "";
    primaryScaleIP = "";
    session_->send( SessionDev, SnVersionInfo );
    keepModal = true;
    doneADownload = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    /////////
    /////////
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

             QString updateCmd = "sendToDept|";

            updateCmd += scaleConfig->FTPIpOrHostname() + "|";
            updateCmd += scaleConfig->FTPdirectory() + "|";

             //bool anyFlags = false;

   if(scaleConfig->FTPPLUDataSendToDept() == "0" &&
                     scaleConfig->FTPLabelDataSendToDept() == "0" &&
                     scaleConfig->FTPFlashkeyDataSendToDept() == "0" &&
                     scaleConfig->FTPOperatorDataSendToDept() == "0" &&
                     scaleConfig->FTPPlaylistsSendToDept() == "0" &&
                     scaleConfig->FTPAlertsSendToDept() == "0" &&
                     scaleConfig->FTPScaleConfigurationSendToDept() == "0" &&
                     scaleConfig->FTPSynchronizationConfigurationSendToDept() == "0" &&
                     scaleConfig->FTPStoreNameNumberSendToDept() == "0" &&
                     scaleConfig->FTPVideosSendToDept() == "0" &&
                     scaleConfig->FTPImagesSendToDept() == "0" &&
                     scaleConfig->FTPAudioSendToDept() == "0" &&
                     scaleConfig->FTPFirmwareSendToDept() == "0")
   {
        ui->lblDataXfer->setText("");
        ui->framesendToDeptConfirmation->hide();
        ui->framesendToDept->show();
        ui->lblsendToDeptStatus->setText("There is no information selected to update.");
        ui->pBarDataXfer->hide();
        ui->butClosesendToDept->setEnabled(true);
        ui->butClosesendToDept->show();
        connect(ui->butClosesendToDept, SIGNAL(clicked()),this,SLOT(closesendToDeptFrame()));
   }
   else
       {
            watcher = new QFileSystemWatcher(this);
            connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(fileChanged(const QString &)));
            connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(directoryChanged(const QString &)));
            watcher->addPath("/usr/local/hobart/pending");
            watcher->addPath("/usr/local/hobart/download");

            connect(ui->butConfirmNO, SIGNAL(clicked()),this,SLOT(nosendToDeptClicked()));
            connect(ui->butConfirmYES, SIGNAL(clicked()),this,SLOT(yessendToDeptClicked()));
            connect(ui->butClosesendToDept, SIGNAL(clicked()),this,SLOT(closesendToDeptFrame()));
            ui->framesendToDeptConfirmation->show();
            ui->framesendToDept->hide();
            translate();
   }
}

void sendToDept::closesendToDeptFrame()
{
    ui->framesendToDept->hide();
    keepModal = false;
   // delete ui;
    mainWindow_->navigateHome();
}


void sendToDept::nosendToDeptClicked()
{
    keepModal = false;
    mainWindow_->navigateHome();

    //delete ui;
    //ui->framesendToDept->hide();
}

void sendToDept::yessendToDeptClicked()
{
   doneADownload = false;
    ui->framesendToDeptConfirmation->hide();
    ui->framesendToDept->show();
    ui->pBarDataXfer->show();
    ui->butClosesendToDept->hide();
    //ui->lblDataXfer->setStyleSheet("color: rgb(255, 255, 255);");
    ui->lblDataXfer->setText(tr("Sending instructions to department scales..."));
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

             QString updateCmd = "updateNow|";

             // get IP

            updateCmd += primaryScaleIP + "|";
            //updateCmd += scaleConfig->FTPIpOrHostname() + "|";
            updateCmd += "/usr/local/hobart|";

             bool anyFlags = false;

             if(scaleConfig->FTPPLUDataSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "plu_data";
                 anyFlags = true;
             }
             if(scaleConfig->FTPLabelDataSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "label_data";
                 anyFlags = true;
             }
             if(scaleConfig->FTPFlashkeyDataSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "flashkey_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPOperatorDataSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "operator_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPScaleConfigurationSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "configuration_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPSynchronizationConfigurationSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "synchronization_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPStoreNameNumberSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "store_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPPlaylistsSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "playlists";
                 anyFlags = true;

             }
             if(scaleConfig->FTPAlertsSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "alerts";
                 anyFlags = true;

             }
             if(scaleConfig->FTPVideosSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "videos";
                 anyFlags = true;

             }
             if(scaleConfig->FTPImagesSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "images";
                 anyFlags = true;

             }
             if(scaleConfig->FTPAudioSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "audio";
                 anyFlags = true;

             }
             /* No firmware updates for operator
              * if(scaleConfig->FTPFirmwareSendToDept() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "firmware";
                 anyFlags = true;
             }
             */
             updateCmd += "|";
             QString userTemp = "";
             QString userPass = "";

             userTemp = "root";
             userPass = "ho1bart2";

             updateCmd += userTemp + "|" + userPass + "|||1\n";
             if(!anyFlags)
             {
                 ui->pBarDataXfer->hide();
                 ui->butClosesendToDept->setEnabled(true);
                 ui->butClosesendToDept->show();
                 ui->lblDataXfer->setText("");
                 ui->lblsendToDeptStatus->setText(tr("No data selected to send to department scales."));
             }

             else
             {
                 // send to each department scale add the installUpgrades flag too
                session_->send(VersionManagerDev, SendToDept2016, "\"" + updateCmd + "\"\n");
                ui->pBarDataXfer->hide();
                ui->butClosesendToDept->setEnabled(true);
                ui->butClosesendToDept->show();
                ui->lblDataXfer->setText("");
                ui->lblsendToDeptStatus->setText(tr("Instructions sent to department scales."));/* QString  failedSecondaryList = "";
                 QString msg;
                 python pythonProcess;
                 QStringList args;
                 QString portNumber = "5076";
                 QString ipAddressToPing;

                 args.append( ipAddressToPing );
                 args.append( portNumber );
                 args.append( updateCmd );

                 //LOG_DEBUG( "IP=%s, Port=%s", ipAddressToPing.toLatin1().constData(), portNumber.toLatin1().constData() );
                 //LOG_DEBUG( "msg=%s", msg.toLatin1().constData() );

                 if ( pythonProcess.execCommand("sendDeviceEventMessage", args, false, false, ( 60 * 1000 ) ) )
                 {
                     if ( pythonProcess.getExitCode() == 0 )
                     {
                         //LOG_DEBUG( "sendDeviceEventMessage SUCCESS", NULL );
                     }
                     else
                     {
                         failedSecondaryList += ipAddressToPing + "\n";
                         //LOG_DEBUG( "sendDeviceEventMessage FAILED", NULL );
                     }
                 }
                 //session_->send(VersionManagerDev, UpdateNow2016, "\"" + updateCmd + "\"\n");
                 */
             }


    ///////////////////////////////

}


sendToDept::~sendToDept()
{
    delete ui;
}

void sendToDept::directoryChanged(const QString & path)
{
    Q_UNUSED( path );

     //ui->lblDataXfer->setText(path);
/*     QString downloadPath = "/usr/local/hobart/download";
     QString pendingPath = "/usr/local/hobart/pending";

     QDir downloadDir( downloadPath );
     QDir pendingDir( pendingPath );
     pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int pendingTotalFiles = pendingDir.count();

     downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int downloadTotalFiles = downloadDir.count();

     //ui->butClosesendToDept->setText(QString::number(downloadTotalFiles+pendingTotalFiles) + " files");
     if((downloadTotalFiles + pendingTotalFiles) > 0)
     {
         ui->butClosesendToDept->hide();
         if(downloadTotalFiles+pendingTotalFiles == 1)
         {
             if(doneADownload) ui->lblDataXfer->setText("There is 1 update remaining to install...");
        }
         else
        {
                 if(doneADownload) ui->lblDataXfer->setText("There are " + QString::number(downloadTotalFiles+pendingTotalFiles) + " updates remaining to install...");
           }
         }
     else if(downloadTotalFiles + pendingTotalFiles == 0)
     {
         //dirs are cleaned out so tell the User
      if(doneADownload)
      {
         ui->lblDataXfer->setText("");
         ui->pBarDataXfer->hide();
         ui->butClosesendToDept->setEnabled(true);
         ui->butClosesendToDept->show();
         ui->lblsendToDeptStatus->setText("Update is complete.");
      }
     }
     */
    ;
}

void sendToDept::fileChanged(const QString & path)
{
     watcher->addPath(path);
     //ui->lblDataXfer->setText(path);

    //qDebug() << path;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void sendToDept::translate()
{
    ui->retranslateUi(this);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
bool sendToDept::onScreenChanging()
{
    if (keepModal) return false;
    else return _Mybase::onScreenChanging();
}

void sendToDept::continueSendToDept()
{
    if(primaryScaleHostname != "" || primaryScaleIP != "")
    {
        ui->frameSplashWait->hide();
       // ui->gbNewProduct->show();
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void sendToDept::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "sendToDept", "onSessionMessage" );

    if (( SessionDev == msg[0].toInt() ) && ( SnVersionInfo == msg[1].toInt() ))
    {
        DomCurrentScaleSetup m( msg );
        primaryScaleIP = m.scaleIpAddress();
        primaryScaleHostname = m.scaleHostName();
        if(primaryScaleHostname != "" || primaryScaleIP != "") continueSendToDept();
    }
    else if(ConfigurationDev == msg[0].toInt() &&  CfgFTPResponseWrite == msg[1].toInt())
    {
            // handle the response from an update now
            bool keepChecking = true;
            doneADownload = true;
            MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

            QString SFTPresponse = msg[2];
            QStringList responseParts = SFTPresponse.split("|",QString::KeepEmptyParts);

            if(responseParts[0].contains("done"))
            {
                    ui->pBarDataXfer->hide();
                    ui->butClosesendToDept->setEnabled(true);
                    ui->butClosesendToDept->show();
                    ui->lblDataXfer->setText("");
                    ui->lblsendToDeptStatus->setText(tr("Instructions sent to department scales."));
                    keepChecking = false;
             }
             else if(scaleConfig->FTPPLUDataSendToDept() != "1" && scaleConfig->FTPFlashkeyDataSendToDept() != "1" && scaleConfig->FTPLabelDataSendToDept() != "1" && scaleConfig->FTPOperatorDataSendToDept() != "1" && scaleConfig->FTPScaleConfigurationSendToDept() != "1" && scaleConfig->FTPSynchronizationConfigurationSendToDept() != "1" && scaleConfig->FTPStoreNameNumberSendToDept() != "1")
            {
                    ui->pBarDataXfer->hide();
                    ui->lblDataXfer->hide();
                    ui->butClosesendToDept->setEnabled(true);
                    ui->butClosesendToDept->show();
                    ui->lblsendToDeptStatus->setText(tr("No data selected to send to department scales."));
                    keepChecking = false;
            }
            else
            {
                ui->pBarDataXfer->hide();
                ui->lblDataXfer->hide();
                ui->butClosesendToDept->setEnabled(true);
                ui->butClosesendToDept->show();
                    ui->lblDataXfer->setText("Instructions sent to department scales.");
                    keepChecking = false;
            }


            if(keepChecking)
            {
                //int totalFiles = 1;
                //bool filesExist = true;
                //future watcher;

                //loop watching the pending and other directories until all are empty
                /*while (totalFiles > 0)
                {
                    totalFiles = pendingDir.count();
                    if(totalFiles == 0)
                    {
                        totalFiles = downloadDir.count();
                    }
                }*/
            }
        }
}


