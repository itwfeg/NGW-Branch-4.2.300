/* @file updateNow.cpp
 * updateNow
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd Davis
 */

#include <logmanager.h>

#include "updateNow.h"
#include "ui_updateNow.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <ScaleConfiguration.h>


#include <QtGui>
#include <QtCore>
#include <QEvent>
#include <QIntValidator>
#include <Qt>
#include <QTimer>

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

updateNow::updateNow(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::updateNow)
{
    ui->setupUi(this);
    keepModal = true;
    doneADownload = false;
    doingFirmware = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;


    /////////
    /////////
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

             QString updateCmd = "updateNow|";

            updateCmd += scaleConfig->FTPIpOrHostname() + "|";
            updateCmd += scaleConfig->FTPdirectory() + "|";

             //bool anyFlags = false;

   if(scaleConfig->FTPPLUDataUpdateNow() == "0" &&
                     scaleConfig->FTPLabelDataUpdateNow() == "0" &&
                     scaleConfig->FTPFlashkeyDataUpdateNow() == "0" &&
                     scaleConfig->FTPOperatorDataUpdateNow() == "0" &&
                     scaleConfig->FTPPlaylistsUpdateNow() == "0" &&
                     scaleConfig->FTPAlertsUpdateNow() == "0" &&
                     scaleConfig->FTPScaleConfigurationUpdateNow() == "0" &&
                     scaleConfig->FTPSynchronizationConfigurationUpdateNow() == "0" &&
                     scaleConfig->FTPStoreNameNumberUpdateNow() == "0" &&
                     scaleConfig->FTPVideosUpdateNow() == "0" &&
                     scaleConfig->FTPImagesUpdateNow() == "0" &&
                     scaleConfig->FTPAudioUpdateNow() == "0" &&
                     scaleConfig->FTPFirmwareUpdateNow() == "0")
   {
        ui->lblDataXfer->setText("");
        ui->frameUpdateNowConfirmation->hide();
        ui->frameUpdateNow->show();
        ui->lblUpdateNowStatus->setText("There is no information selected to update.");
        ui->pBarDataXfer->hide();
        ui->butCloseUpdateNow->setEnabled(true);
        ui->butCloseUpdateNow->show();
        connect(ui->butCloseUpdateNow, SIGNAL(clicked()),this,SLOT(closeUpdateNowFrame()));
   }
   else
       {
            watcher = new QFileSystemWatcher(this);
            connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(fileChanged(const QString &)));
            connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(directoryChanged(const QString &)));
            watcher->addPath("/usr/local/hobart/pending");
            watcher->addPath("/usr/local/hobart/download");

            connect(ui->butConfirmNO, SIGNAL(clicked()),this,SLOT(noUpdateNowClicked()));
            connect(ui->butConfirmYES, SIGNAL(clicked()),this,SLOT(yesUpdateNowClicked()));
            connect(ui->butCloseUpdateNow, SIGNAL(clicked()),this,SLOT(closeUpdateNowFrame()));
            ui->frameUpdateNowConfirmation->show();
            ui->frameUpdateNow->hide();
            translate();
   }
}

void updateNow::closeUpdateNowFrame()
{
    ui->frameUpdateNow->hide();
    keepModal = false;
   // delete ui;
    mainWindow_->navigateHome();
}


void updateNow::noUpdateNowClicked()
{
    keepModal = false;
    mainWindow_->navigateHome();

    //delete ui;
    //ui->frameUpdateNow->hide();
}

void updateNow::yesUpdateNowClicked()
{
   doneADownload = false;
    ui->frameUpdateNowConfirmation->hide();
    ui->frameUpdateNow->show();
    ui->pBarDataXfer->show();
    ui->butCloseUpdateNow->hide();
    //ui->lblDataXfer->setStyleSheet("color: rgb(255, 255, 255);");
    ui->lblDataXfer->setText(tr("Downloading..."));
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

             QString updateCmd = "updateNow|";

            updateCmd += scaleConfig->FTPIpOrHostname() + "|";
            updateCmd += scaleConfig->FTPdirectory() + "|";

             bool anyFlags = false;

             if(scaleConfig->FTPPLUDataUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "plu_data";
                 anyFlags = true;
             }
             if(scaleConfig->FTPLabelDataUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "label_data";
                 anyFlags = true;
             }
             if(scaleConfig->FTPFlashkeyDataUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "flashkey_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPOperatorDataUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "operator_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPPlaylistsUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "playlists";
                 anyFlags = true;

             }
             if(scaleConfig->FTPAlertsUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "alerts";
                 anyFlags = true;

             }
             if(scaleConfig->FTPScaleConfigurationUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "configuration_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPSynchronizationConfigurationUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "synchronization_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPStoreNameNumberUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "store_data";
                 anyFlags = true;

             }
             if(scaleConfig->FTPVideosUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "videos";
                 anyFlags = true;

             }
             if(scaleConfig->FTPImagesUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "images";
                 anyFlags = true;

             }
             if(scaleConfig->FTPAudioUpdateNow() == "1")
             {
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "audio";
                 anyFlags = true;

             }

             // REQs change on 8/24/2016 to not allow operator to do anything with firmware
             /*if(scaleConfig->FTPFirmwareUpdateNow() == "1")
             {
                 doingFirmware = true;
                 if(anyFlags) updateCmd += ",";
                 updateCmd += "firmware";
                 anyFlags = true;

             }
             */
             updateCmd += "|";
             QString userTemp = "";
             QString userPass = "";

             if(scaleConfig->FTPpassword() == "" && scaleConfig->FTPusername() == "")
             {
                 userTemp = "root";
                 userPass = "ho1bart2";
             }
             else
             {
              userTemp = scaleConfig->FTPusername();
              userPass = scaleConfig->FTPpassword();
             }

             updateCmd += userTemp + "|" + userPass + "||";
             // REQs change on 8/24/2016 no firmware changes made by operator
            // if(scaleConfig->FTPFirmwareUpdateNow() == "1") updateCmd += "installFirmwarePlease";
             updateCmd += "|1\n";
             if(!anyFlags)
             {
                 ui->pBarDataXfer->hide();
                 ui->butCloseUpdateNow->setEnabled(true);
                 ui->butCloseUpdateNow->show();
                 ui->lblDataXfer->setText("");
                 ui->lblUpdateNowStatus->setText(tr("No data selected to update."));

             }
             else
             {
                 session_->send(VersionManagerDev, UpdateNow2016, "\"" + updateCmd + "\"\n");
             }


    ///////////////////////////////

}


updateNow::~updateNow()
{
    delete ui;
}

void updateNow::directoryChanged(const QString & path)
{
     Q_UNUSED( path );
     QString downloadPath = "/usr/local/hobart/download";
     QString pendingPath = "/usr/local/hobart/pending";

     QDir downloadDir( downloadPath );
     QDir pendingDir( pendingPath );
     pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int pendingTotalFiles = pendingDir.count();

     downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
     int downloadTotalFiles = downloadDir.count();

     //ui->butCloseUpdateNow->setText(QString::number(downloadTotalFiles+pendingTotalFiles) + " files");
     if((downloadTotalFiles + pendingTotalFiles) > 0)
     {
         ui->butCloseUpdateNow->hide();
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
          if(doingFirmware)
          {
              QString cmd = "";
              cmd = "installUpgrades|1";
              ui->lblDataXfer->setText(tr("Installing firmware..."));
              session_->send(VersionManagerDev, UpdateNow2016, " --timeout 7200 \"" + cmd + "\"\n");
          }
          else
          {
             ui->lblDataXfer->setText("");
             ui->pBarDataXfer->hide();
             ui->butCloseUpdateNow->setEnabled(true);
               ui->butCloseUpdateNow->show();
             ui->lblUpdateNowStatus->setText("Update is complete.");
          }
      }
     }
}

void updateNow::fileChanged(const QString & path)
{
     watcher->addPath(path);
     //ui->lblDataXfer->setText(path);

    //qDebug() << path;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void updateNow::translate()
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
bool updateNow::onScreenChanging()
{
    if (keepModal) return false;
    else return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void updateNow::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "updateNow", "onSessionMessage" );
    if(ConfigurationDev == msg[0].toInt() &&  CfgFTPResponseWrite == msg[1].toInt())
        {
            // handle the response from an update now
            LOG_DEBUG("Got response from update now, setting doneADownload to true!", NULL);
            bool keepChecking = true;
            doneADownload = true;
            MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

            QString SFTPresponse = msg[2];
            QStringList responseParts = SFTPresponse.split("|",QString::KeepEmptyParts);

            if(responseParts[0].contains("updateNow"))
            {
                QString downloadPath = "/usr/local/hobart/download";
                QString pendingPath = "/usr/local/hobart/pending";

                QDir downloadDir( downloadPath );
                QDir pendingDir( pendingPath );
                pendingDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int pendingTotalFiles = pendingDir.count();

                downloadDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
                int downloadTotalFiles = downloadDir.count();

                if(pendingTotalFiles == 0 && downloadTotalFiles == 0 )
                {
                    ui->pBarDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblDataXfer->setText("");
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                }
                 else if(scaleConfig->FTPPLUDataUpdateNow() != "1" && scaleConfig->FTPFlashkeyDataUpdateNow() != "1" && scaleConfig->FTPLabelDataUpdateNow() != "1" && scaleConfig->FTPOperatorDataUpdateNow() != "1" && scaleConfig->FTPScaleConfigurationUpdateNow() != "1" && scaleConfig->FTPSynchronizationConfigurationUpdateNow() != "1" && scaleConfig->FTPStoreNameNumberUpdateNow() != "1")
                {
                    ui->pBarDataXfer->hide();
                    ui->lblDataXfer->hide();
                    ui->butCloseUpdateNow->setEnabled(true);
                    ui->butCloseUpdateNow->show();
                    ui->lblUpdateNowStatus->setText("Update is complete.");
                    keepChecking = false;
                }
                else
                {
                    ui->lblDataXfer->setText("Download complete....installing now....");
                    keepChecking = true;
                }

            }
            else if(responseParts.size() > 2 && responseParts[0].contains("error"))
            {
                if(responseParts[2].contains("Undetermined error",Qt::CaseInsensitive))
                {
                     ui->lblDataXfer->setText("Update stopped...\nCorrect the Location/Login tab\nentries and try again.");
                }
                if(responseParts[1].contains("installUpgrades") && responseParts[2].contains("no upgrades to install"))
                {
                    ui->lblDataXfer->setText("");
                }
                else ui->lblDataXfer->setText("");
                ui->pBarDataXfer->hide();
                ui->lblUpdateNowStatus->setText(tr("Update Now done."));
                ui->butCloseUpdateNow->setEnabled(true);
                ui->butCloseUpdateNow->show();
               keepChecking = false;
            }
            else
            {
                LOG_WARN("Unexpected response from update now: %s", SFTPresponse.toStdString().c_str());
                ui->lblUpdateNowStatus->setText(tr("Update Now done."));
                ui->lblDataXfer->setText(tr("Update stopped...\nan undetermined error occurred."));
                ui->pBarDataXfer->hide();
                ui->butCloseUpdateNow->setEnabled(true);
                ui->butCloseUpdateNow->show();
                keepChecking = false;
            }
            if(keepChecking)
            {
                //int totalFiles = 1;
                //bool filesExist = true;
                //watcher;

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


