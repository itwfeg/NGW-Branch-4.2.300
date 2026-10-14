/* @file serviceLabelApplierInfo.cpp
 * serviceLabelApplierInfo
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Shows Wrapper system info
 */

#include "serviceLabelApplierInfo.h"
#include "ui_serviceLabelApplierInfo.h"

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

serviceLabelApplierInfo::serviceLabelApplierInfo(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceLabelApplierInfo)
{
    ui->setupUi(this);
    scaleScreen();

    wasErased = false;

    //ui->butErase->hide();
    ui->butSave->hide();
    internalConfigData = &session_->scaleConfig->internalConfig;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    connect(ui->butEasterEgg, SIGNAL(clicked()), this, SLOT(showEraseButton()));
    connect(ui->butErase, SIGNAL(clicked()), this, SLOT(eraseButClicked()));
    connect(ui->butSave, SIGNAL(clicked()), this, SLOT(saveButClicked()));
    connect(ui->leSerialNumber, SIGNAL(textChanged(QString)), this, SLOT(truncatingSerialNumber(QString)));
    connect(ui->leDateOfInstallation, SIGNAL(textChanged(QString)), this, SLOT(dateOfInstallationChanged(QString)));
    connect(ui->leDateOfManufacture, SIGNAL(textChanged(QString)), this, SLOT(dateOfManufactureChanged(QString)));

    ui->cboxModelNumber->clear();
    ui->cboxModelNumber->addItem(tr("CLAS"),"2");
    ui->cboxModelNumber->addItem(tr("CLAS-B"), "3");
    ui->cboxModelNumber->setEnabled(false);

    //	HTA-83:  Disable these fields until they are editable.
    ui->leSerialNumber->setEnabled(false);
    ui->leDateOfManufacture->setEnabled(false);
    ui->leDateOfInstallation->setEnabled(false);
    ui->leCycleCount->setEnabled(false);

    update();

    // Set Label Applier to service mode
    session_->send(LabelApplier, LaMode, "5"); //5,7,5

    // Get the version info
    session_->send(LabelApplierSystem, LaSysReqStatus); // 6,13

    // Get the board info
    session_->send(LabelApplier, LaReqSysInfo); // 5,29
    ui->butErase->hide();
    this->initialize();
    translate();
}

QString serviceLabelApplierInfo::prepareDateString(bool forDB, QString incomingDate)
{
    //incomingDate is guaranteed to have 00/00/00 format
    if(forDB)
    {
        //remove slashes
        return incomingDate.replace("/","");
    }
    else
    {
        //add slashes
        return incomingDate.mid(0,2) + "/" + incomingDate.mid(2,2) + "/" + incomingDate.mid(4,2);
    }
}

QString serviceLabelApplierInfo::prepareSerialNumber(bool forDB, QString incomingSerial)
{
    if(forDB)
    {
        //remove hyphens
        return incomingSerial.replace("-", "");
    }
    else
    {
        //add hyphens
        return incomingSerial.mid(0,2) + "-" + incomingSerial.mid(2,4) + "-" + incomingSerial.mid(6);
    }
}



void serviceLabelApplierInfo::hyphenateSerialNumber(QString newSerial)
{
    Q_UNUSED( newSerial );
}

void serviceLabelApplierInfo::dateOfManufactureChanged(QString dateManu)
{
    ui->leDateOfManufacture->setText(truncatingDate(dateManu));
    ui->leDateOfManufacture->setCursorPosition(ui->leDateOfManufacture->text().length());
}

void serviceLabelApplierInfo::dateOfInstallationChanged(QString dateInstall)
{
    ui->leDateOfInstallation->setText(truncatingDate(dateInstall));
    ui->leDateOfInstallation->setCursorPosition(ui->leDateOfInstallation->text().length());
}

QString serviceLabelApplierInfo::truncatingDate(QString incomingDate)
{
    QString finalDate = "";
    finalDate = incomingDate.replace("/", "");
    bool ok;

    finalDate.mid(finalDate.length()-1).toInt(&ok);

    if(!ok)
    {
        //last character typed was not a number
        finalDate = incomingDate.mid(0,incomingDate.length()-1);
    }
    else
    {
        if(finalDate.length() > 6)
        {
            finalDate = finalDate.mid(1);
        }
        else
        {
            //pad with zeroes on the left
            QString tempDate = "";
            for(int i=1; i < (6 - finalDate.length()); i++)
            {
                tempDate += "0";
            }
            finalDate = tempDate + finalDate;
        }
        // Add slashes now
    }
    finalDate = finalDate.mid(0,2) + "/" + finalDate.mid(2,2) + "/" + finalDate.mid(4);
    return finalDate;

}

void serviceLabelApplierInfo::truncatingSerialNumber(QString incomingSerial)
{
    //qDebug() << "newword is : " << incomingSerial;
    QString finalSerial = "";
    finalSerial = incomingSerial.replace("-", "");
    bool ok;

    finalSerial.mid(finalSerial.length()-1).toInt(&ok);

    if(!ok)
    {
        ui->leSerialNumber->setText(incomingSerial.mid(0,incomingSerial.length()-1));
    }
    else
    {
        if(finalSerial.length() > 9)
        {
            //lop off left character
            finalSerial = finalSerial.mid(1);
        }
        else
        {
            //length is less than max of nine so pad with zeroes on the left
            QString tempSerial = "";
            for(int i = 1; i < (9 - finalSerial.length()); i++ )
            {
                tempSerial += "0";
            }
            finalSerial = tempSerial + finalSerial;
        }
        // Add hyphens now
        finalSerial = finalSerial.mid(0,2) + "-" + finalSerial.mid(2,4) + "-" + finalSerial.mid(6);
        ui->leSerialNumber->setText(finalSerial);
    }
    ui->leSerialNumber->setCursorPosition(ui->leSerialNumber->text().length());

}

void serviceLabelApplierInfo::saveButClicked()
{
    if(ui->butSave->isEnabled())
    {
        QString msg = "";
        msg += ui->leSerialNumber->text();

        //prepare manufacturer date as a 6 character string
        msg += "," + prepareDateString(true, ui->leDateOfManufacture->text());

        //prepare installation date as a 6 character string
        msg += "," + prepareDateString(true, ui->leDateOfInstallation->text());

        msg += "," + ui->cboxModelNumber->itemData(ui->cboxModelNumber->currentIndex()).toString();

        msg += "," + ui->leCycleCount->text();

        session_->send(LabelApplier, LaSysInfoWrite, msg); //5,26
        ui->butSave->hide();
        ui->butSave->setDisabled(true);

        //  Disable these fields since they are no longer editable.
        ui->cboxModelNumber->setEnabled(false);
        ui->leSerialNumber->setEnabled(false);
        ui->leDateOfManufacture->setEnabled(false);
        ui->leDateOfInstallation->setEnabled(false);
        ui->leCycleCount->setEnabled(false);
    }
}

void serviceLabelApplierInfo::eraseButClicked()
{
    session_->send(LabelApplier, LaSysInfoErase); //5,27
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    wasErased = true;
}

void serviceLabelApplierInfo::showEraseButton()
{
    ui->butErase->show();
    ui->butErase->setEnabled(true);

    ui->butEasterEgg->setEnabled(false);    //  Now disable it
}

bool serviceLabelApplierInfo::eventFilter( QObject *obj, QEvent *event )
{
    QString msg;

    /* Sample event below
    if(ui->leClass == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            this->doSave();
        }
    }
    */
    return _Mybase::eventFilter( obj, event );
}

serviceLabelApplierInfo::~serviceLabelApplierInfo()
{
    delete ui;
}

int serviceLabelApplierInfo::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceLabelApplierInfo::initialize()
{

    hasChanged = false;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->scaleModel = scaleConfigData->scaleModel();

    update();
}

void serviceLabelApplierInfo::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierInfo::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierInfo::onScreenChanging()
{

    if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
    {
        ;//session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    return _Mybase::onScreenChanging();
}

void serviceLabelApplierInfo::updateCLASystemInfo(DomLabelApplierSystemInformation msg)
{
    ui->cboxModelNumber->setCurrentIndex(ui->cboxModelNumber->findData(msg.awsModelNumberRaw()));
    ui->leSerialNumber->setText(msg.awsSerialNumber());
    ui->leCycleCount->setText(msg.awsLabelApplierCycleCount());
    ui->leDateOfInstallation->setText(truncatingDate(msg.awsInstallDate()));
    ui->leDateOfManufacture->setText(truncatingDate(msg.awsManufactureDate()));
    ui->lblPartNumberValue->setText(msg.boardPartNumber());
    ui->lblRevisionValue->setText(msg.boardRev());
    ui->lblDateOfManufactureValue->setText(msg.boardManufactureDate());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierInfo::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        ;//popupActive = true;
        //popupCreateNewPLUDialog();
    }
    else if(LabelApplier == msg[0].toInt() && LaSysInfo == msg[1].toInt())
    {
        DomLabelApplierSystemInformation m(msg);


        updateCLASystemInfo(m);
        if (wasErased)		//	HTA-83:  Enable these fields only if they have been erased.
        {
            ui->cboxModelNumber->setEnabled(true);
            ui->leSerialNumber->setEnabled(true);
            ui->leDateOfManufacture->setEnabled(true);
            ui->leDateOfInstallation->setEnabled(true);
            ui->leCycleCount->setEnabled(true);
            ui->butSave->show();
            ui->butSave->setEnabled(true);
            wasErased = false;
        }
//        if(m.awsInstallDate() == "000000")
//        {
//            ui->leDateOfInstallation->setEnabled(true);
//            ui->butSave->show();
//            ui->butSave->setEnabled(true);
//        }
    }
    else if(LabelApplier == msg[0].toInt() && LaCLASysStatus == msg[1].toInt())
    {
        //5,21
        DomLabelApplierSystemStatus m(msg);

        ui->lblChecksumValue->setText(m.configChecksum());
        ui->lblBootValue->setText(m.bootVersion());
        ui->lblApplicationValue->setText(m.appVersion());
        ui->lblDataValue->setText(m.dataVersion());
    }
}

bool serviceLabelApplierInfo::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}


