/* @file superEmailSetup.cpp
 * superEmailSetup 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Sets email notification settings for system alerts
 */

#include "superEmailSetup.h"
#include "ui_superEmailSetup.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

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

superEmailSetup::superEmailSetup(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superEmailSetup)
{
    ui->setupUi(this);
    scaleScreen();

    loading = true;
    ignoreSignals_ = false;

    this->isIPSelected = false;

    ui->gbHostname->show();
    ui->gbIP->setGeometry( ui->gbHostname->geometry() );
    ui->gbIP->hide();

    // Check if Hands-Free is licensed
    if( !( session_->uiUtilities->isFeatureLicensed( SCALE_HANDSFREEDEVICE ) ) )
        ui->mainTabs->removeTab( ui->mainTabs->indexOf( ui->tabHandsFreeAlert ) );

    connect(ui->leHostname, SIGNAL(textChanged(const QString)), this, SLOT(preventSpacesInHostnames(const QString&)));
    connect(ui->butSwitchIPHostname, SIGNAL(clicked()) , this, SLOT(switchIPHostnameButClicked()));
    connect(ui->tvAlerts, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(alertCheckboxChanged(QTableWidgetItem*)));
    connect(ui->tvAlertsHandsFree, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(alertCheckboxChanged(QTableWidgetItem*)));


    ui->leIP1->setValidator(ipFirstOctet_ = new QIntValidator(0, INT_MAX, this));
    ui->leIP2->setValidator(ipNonFirstOctet_ = new QIntValidator(0, INT_MAX, this));
    ui->leIP3->setValidator(ipNonFirstOctet_);
    ui->leIP4->setValidator(ipNonFirstOctet_);

    ui->leIP1->setDecimalPlaces(0);
    ui->leIP1->setInputWrapping(true);
    ipFirstOctet_->setTop(255);
    // no definitive answer to bottom, so leaving it as 0.
    ipFirstOctet_->setBottom(0);

    ui->leIP2->setDecimalPlaces(0);
    ui->leIP2->setInputWrapping(true);
    ui->leIP3->setDecimalPlaces(0);
    ui->leIP3->setInputWrapping(true);
    ui->leIP4->setDecimalPlaces(0);
    ui->leIP4->setInputWrapping(true);
    ipNonFirstOctet_->setTop(255);
    ipNonFirstOctet_->setBottom(0);

    session_->send(ConfigurationDev,CfgEmailRead); // 112,130 - request email config to from ip or dns

    translate();
}

superEmailSetup::~superEmailSetup()
{
    delete ui;
}

void superEmailSetup::preventSpacesInHostnames(const QString&)
{
    ui->leHostname->setText(ui->leHostname->text().replace(" ",""));
}

void superEmailSetup::switchIPHostnameButClicked()
{
    if(ui->gbIP->isVisible())
    {
        ui->gbIP->hide();
        ui->gbHostname->show();
        ui->butSwitchIPHostname->setText(tr("Switch to IP address"));
        isIPSelected = false;
    }
    else
    {
        ui->gbHostname->hide();
        ui->gbIP->show();
        ui->butSwitchIPHostname->setText(tr("Switch to Host name"));
        isIPSelected = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superEmailSetup::translate()
{
    ui->retranslateUi(this);
    ignoreSignals_ = true;

    // Need to update the table values like we update the combo boxes

    if( !loading )           // can reload this if completely loaded the first time
    {
        alertList.clear();
        session_->send(EmailAlertsDev,emailAlertDtReadAll); // 134,1 - request alerts
    }

    update();
    ignoreSignals_ = false;
}

void superEmailSetup::saveFromToIPOrDNS()
{
    //Sample message: 112,132,"APPLE SMPLE, SAM","DAVIS@WAVEFISH.COM",192.168.2.3

    QString msg;

    msg = "\"" + ui->leFrom->text() + "\",\"" + ui->leTo->text() + "\",";

    if(isIPSelected)
    {
        msg += ui->leIP1->text() + "." + ui->leIP2->text() + "." + ui->leIP3->text() + "." + ui->leIP4->text();
    }
    else
    {
        msg += ui->leHostname->text();
    }

    session_->send(ConfigurationDev,CfgEmailSave, msg); // 112,132 - request email config to from ip or dns
}

void superEmailSetup::loadFromToServerIsDNS()
{
    //send 134,1
    session_->send(EmailAlertsDev,emailAlertDtReadAll); // 134,1 - request email config to from ip or dns
}

void superEmailSetup::alertCheckboxChanged(QTableWidgetItem* item)
{
    if( ignoreSignals_ )
        return;

    if(!loading)
    {
        int col = item->column();
        int row = item->row();

        if( ( col == 0 || col == 2 || col == 5) && !loading )
        {
            if(alertList.at(row) != tr("Scale Offline Notice"))
            {
                alertMsg = alertTypeList.at(row) + "," +                                                // Row number (ID)
                           alertList.at(row) + "," +                                                    // Text description
                           (ui->tvAlerts->item(row,0)->checkState() == 0 ? "0" : "1" ) + "," +          // Email: Notify when failure
                           (ui->tvAlerts->item(row,2)->checkState() == 0 ? "0" : "1" ) + "," +          // Email: Notify when resolved
                           (ui->tvAlertsHandsFree->item(row,0)->checkState() == 0 ? "0" : "1" ) + "," + // HFPD: Notify when failure
                           (ui->tvAlertsHandsFree->item(row,2)->checkState() == 0 ? "0" : "1" ) + "," + // HFPD: Notify when resolved
                           ui->tvAlerts->item(row,5)->text();                                           // Email & HFPD: Delay length
            }
            else
            {
                alertMsg = alertTypeList.at(row) + "," +                                                // Row number (ID)
                           alertList.at(row) + "," +                                                    // Text description
                           (ui->tvAlerts->item(row,2)->checkState() == 0 ? "0" : "1" ) + "," +          // Email: Notify when failure
                           (ui->tvAlerts->item(row,2)->checkState() == 0 ? "0" : "1" ) + "," +          // Email: Notify when resolved
                           (ui->tvAlertsHandsFree->item(row,0)->checkState() == 0 ? "0" : "1" ) + "," + // HFPD: Notify when failure
                           (ui->tvAlertsHandsFree->item(row,2)->checkState() == 0 ? "0" : "1" ) + "," + // HFPD: Notify when resolved
                           ui->tvAlerts->item(row,5)->text();                                           // Email & HFPD: Delay length
            }
        }
        session_->send(EmailAlertsDev,emailAlertDtRestore, alertMsg);
// Why do we need to send this twice???        session_->send(EmailAlertsDev,emailAlertDtRestore, alertMsg);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superEmailSetup::onScreenChanging()
{
    //alerts are saved during a change so just save from to and server
    saveFromToIPOrDNS();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superEmailSetup::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if( ConfigurationDev == device && CfgEmailWrite == event ) //112,131
    {
        DomEmailConfig m( msg );

        ui->leFrom->setText( m.from() );
        ui->leTo->setText( m.sendTo() );
        QStringList mailServer;

        mailServer = m.mailServer().split( ".", QString::KeepEmptyParts );

        isIPSelected = false;

        if(mailServer.count() == 4)
        {
            // Could be ip
            bool ok;
            mailServer.at(0).toInt(&ok);

            if(ok)
            {
                if(mailServer.at(0).toInt() < 256)
                {
                    mailServer.at(1).toInt(&ok);
                    if(ok)
                    {
                        if(mailServer.at(1).toInt() < 256)
                        {
                            mailServer.at(2).toInt(&ok);
                            if(ok)
                            {
                                if(mailServer.at(2).toInt() < 256)
                                {
                                    mailServer.at(3).toInt(&ok);
                                    if(ok)
                                    {
                                        if(mailServer.at(3).toInt() < 256)
                                        {
                                           // must be IP
                                            ui->gbHostname->hide();
                                            ui->gbIP->show();
                                            ui->leIP1->setText(mailServer.at(0));
                                            ui->leIP2->setText(mailServer.at(1));
                                            ui->leIP3->setText(mailServer.at(2));
                                            ui->leIP4->setText(mailServer.at(3));
                                            ui->butSwitchIPHostname->setText( tr( "Switch to Host name" ) );
                                            isIPSelected = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if( !isIPSelected )
        {
            // must be hostname
            ui->gbIP->hide();
            ui->gbHostname->show();
            ui->leHostname->setText( m.mailServer() );
            ui->butSwitchIPHostname->setText( tr( "Switch to IP address" ) );
            isIPSelected = false;
        }

        loadFromToServerIsDNS();
    }
    else if( EmailAlertsDev == device && emailAlertDtWriteAll == event ) //134,3
    {
        DomEmailNotifyEvents m( msg );

        QFont fnt;
        fnt.setPointSize(20);

        ignoreSignals_ = true;

        int alertCount = m.evtSelItems().toInt();

        // TODO: Come back to this is needed when...
        // a. Needs fixed in HobEmailAlert.cpp. Does not work correct.
        // b. Right now users cant add/remove alerts/change their names
        // c. No interface to allow more added, so hard code them for now.

        QStringList alertCBText = m.evtTextList().split(",", QString::KeepEmptyParts);

        alertTypeList.clear();
        alertList.clear();
        alertCBList.clear();
        notifyCBList.clear();
        delayList.clear();

        alertTypeList = m.recordList().split(",", QString::KeepEmptyParts);
        alertCBList = m.evtSelList().split(",", QString::KeepEmptyParts);
        notifyCBList = m.evtNotifyList().split(",", QString::KeepEmptyParts);
        delayList = m.evtDelayList().split(",", QString::KeepEmptyParts);

        ui->tvAlerts->setRowCount(alertCount);
        ui->tvAlerts->setColumnCount(7);

        //-------------
        // Email Alerts
        //-------------
        for(int i = 0; i < alertCount; i++)
        {
            //  HTA-113: Only the applicable email alerts will be sent, based on the scaleModel.
            //           We can't depend on alertCBText to know what email alert types are in the list.
            //           We have to use the recordList in the message to know what alert types are being sent.
            alertList.append( alertCBText.at( i ) );
            ui->tvAlerts->setRowHeight( i, scaleHeightForScreen( 50 ) );
            QTableWidgetItem *alertCheckboxItem = new QTableWidgetItem;
            QTableWidgetItem *alertTextItem = new QTableWidgetItem;
            QTableWidgetItem *notifyCheckboxItem = new QTableWidgetItem;
            QTableWidgetItem *notifyTextItem = new QTableWidgetItem;
            QTableWidgetItem *delayTextItem = new QTableWidgetItem;
            QTableWidgetItem *delayEditItem = new QTableWidgetItem;
            QTableWidgetItem *delayTimeTextItem = new QTableWidgetItem;

            alertTextItem->setText( alertList.at(i) );
            alertCheckboxItem->setCheckState( alertCBList.at(i) == "0" ? Qt::Unchecked : Qt::Checked );
            notifyTextItem->setText( tr( "Notify when resolved" ) );
            notifyCheckboxItem->setCheckState( notifyCBList.at(i) == "0" ? Qt::Unchecked : Qt::Checked );
            delayTextItem->setText( tr( "Delay" ) );
            delayEditItem->setText( delayList.at(i) );
            delayEditItem->setFont( fnt );
            delayTimeTextItem->setText( tr( "min" ) );

            QString alert_item = alertList.at(i);

            // This is because we always want Scale Offline Notice to be selected.
            if(alertTypeList.at(i).toInt() == EMAILALERT_SCALE_OFFLINE)     //  HTA-113
            {
                QTableWidgetItem *scaleOfflineTextItem = new QTableWidgetItem;
                scaleOfflineTextItem->setText(" X");
                scaleOfflineTextItem->setFont(fnt);
                ui->tvAlerts->setItem(i,0,scaleOfflineTextItem);
            }
            else
            {
                ui->tvAlerts->setItem(i,0,alertCheckboxItem);
                ui->tvAlerts->item(i,0)->setTextAlignment(Qt::AlignRight);
            }

            ui->tvAlerts->setItem(i,1,alertTextItem);
            ui->tvAlerts->setItem(i,2,notifyCheckboxItem);
            ui->tvAlerts->setItem(i,3,notifyTextItem);
            ui->tvAlerts->setItem(i,4,delayTextItem);
            ui->tvAlerts->setItem(i,5,delayEditItem);
            ui->tvAlerts->setItem(i,6,delayTimeTextItem);


            this->ui->tvAlerts->setColumnWidth( 0, scaleWidthForScreen( 40 ) );  // alerts checkbox
            this->ui->tvAlerts->setColumnWidth( 1, scaleWidthForScreen( 305 ) ); // alerts text
            this->ui->tvAlerts->setColumnWidth( 2, scaleWidthForScreen( 40 ) );  // notify checkbox
            this->ui->tvAlerts->setColumnWidth( 3, scaleWidthForScreen( 290 ) ); // notify text
            this->ui->tvAlerts->setColumnWidth( 4, scaleWidthForScreen( 85 ) );  // delay text
            this->ui->tvAlerts->setColumnWidth( 5, scaleWidthForScreen( 60 ) );  // delay line edit
            this->ui->tvAlerts->setColumnWidth( 6, scaleWidthForScreen( 40 ) );  // delay time


            ui->tvAlerts->item(i,1)->setTextAlignment(Qt::AlignVCenter);
            ui->tvAlerts->item(i,2)->setTextAlignment(Qt::AlignRight);
            ui->tvAlerts->item(i,3)->setTextAlignment(Qt::AlignVCenter);
            ui->tvAlerts->item(i,4)->setTextAlignment(Qt::AlignVCenter);
            ui->tvAlerts->item(i,5)->setTextAlignment(Qt::AlignCenter);
            ui->tvAlerts->item(i,6)->setTextAlignment(Qt::AlignVCenter);
        }

        //----------------------------
        // Hands-Free Alerts: SOF-2455
        //----------------------------

        //-----------------------------------------------------------------------------------------------
        // NOTE: Need to process and update the Hands-Free tab even if Hands-Free is not licensed.
        //       The table on the Hands-Free tab is needed when saving the data in alertCheckboxChanged()
        //-----------------------------------------------------------------------------------------------

        alertTypeList.clear();
        alertList.clear();
        alertCBList.clear();
        notifyCBList.clear();

        alertTypeList = m.recordList().split(",", QString::KeepEmptyParts);
        alertCBList = m.evtHFNotifyList().split(",", QString::KeepEmptyParts);
        notifyCBList = m.evtHFResolutionList().split(",", QString::KeepEmptyParts);

        ui->tvAlertsHandsFree->setRowCount( alertCount );
        ui->tvAlertsHandsFree->setColumnCount( 4 );

        for( int i = 0; i < alertCount; i++ )
        {
            //  HTA-113: Only the applicable email alerts will be sent, based on the scaleModel.
            //           We can't depend on alertCBText to know what email alert types are in the list.
            //           We have to use the recordList in the message to know what alert types are being sent.
            alertList.append( alertCBText.at( i ) );
            ui->tvAlertsHandsFree->setRowHeight( i, scaleHeightForScreen( 50 ) );
            QTableWidgetItem *alertCheckboxItem = new QTableWidgetItem;
            QTableWidgetItem *alertTextItem = new QTableWidgetItem;
            QTableWidgetItem *notifyCheckboxItem = new QTableWidgetItem;
            QTableWidgetItem *notifyTextItem = new QTableWidgetItem;

            alertTextItem->setText( alertList.at(i) );
            alertCheckboxItem->setCheckState( alertCBList.at(i) == "0" ? Qt::Unchecked : Qt::Checked );
            notifyTextItem->setText( tr( "Notify when resolved" ) );
            notifyCheckboxItem->setCheckState( notifyCBList.at(i) == "0" ? Qt::Unchecked : Qt::Checked );

            QString alert_item = alertList.at(i);

            // This is because we always want Scale Offline Notice to be selected.
            if(alertTypeList.at(i).toInt() == EMAILALERT_SCALE_OFFLINE)     //  HTA-113
            {
                QTableWidgetItem *scaleOfflineTextItem = new QTableWidgetItem;
                scaleOfflineTextItem->setText(" X");
                scaleOfflineTextItem->setFont(fnt);
                ui->tvAlertsHandsFree->setItem(i,0,scaleOfflineTextItem);
            }
            else
            {
                ui->tvAlertsHandsFree->setItem(i,0,alertCheckboxItem);
                ui->tvAlertsHandsFree->item(i,0)->setTextAlignment(Qt::AlignRight);
            }

            ui->tvAlertsHandsFree->setItem(i,1,alertTextItem);
            ui->tvAlertsHandsFree->setItem(i,2,notifyCheckboxItem);
            ui->tvAlertsHandsFree->setItem(i,3,notifyTextItem);

            this->ui->tvAlertsHandsFree->setColumnWidth( 0, scaleWidthForScreen( 40 ) );  // alerts checkbox
            this->ui->tvAlertsHandsFree->setColumnWidth( 1, scaleWidthForScreen( 305 ) ); // alerts text
            this->ui->tvAlertsHandsFree->setColumnWidth( 2, scaleWidthForScreen( 40 ) );  // notify checkbox
            this->ui->tvAlertsHandsFree->setColumnWidth( 3, scaleWidthForScreen( 290 ) ); // notify text

            ui->tvAlertsHandsFree->item(i,1)->setTextAlignment(Qt::AlignVCenter);
            ui->tvAlertsHandsFree->item(i,2)->setTextAlignment(Qt::AlignRight);
            ui->tvAlertsHandsFree->item(i,3)->setTextAlignment(Qt::AlignVCenter);
        }

        // hide the Scale Offline Notice first checkbox

        loading = false;
        ignoreSignals_ = false;
    }
}
