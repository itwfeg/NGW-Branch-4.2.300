/* @file ServiceInstallationLog.cpp
 * ServiceInstallationLog 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "serviceinstallationlog.h"
#include "ui_serviceinstallationlog.h"
#include "TokenParser.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

ServiceInstallationLog::ServiceInstallationLog(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::ServiceInstallationLog),
    installationLogDeviceData_(NULL)
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    ui->txtInstallNote->setReadOnly( true );

    if ( mainWindow_->isServiceMode() )
        installationLogDeviceData_ = &dynamic_cast<ServiceMode*>( mode_ )->installationLogDeviceData_;

    initialize();
    translate();
}

ServiceInstallationLog::~ServiceInstallationLog()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::initialize()
{

    connect(ui->butAddNewNote,      SIGNAL(clicked()),                    this,   SLOT(addNote()));             // click the button
    connect(ui->cboDeviceId,        SIGNAL(currentIndexChanged(int)),     this,   SLOT(setupSubIdList(int)));
    connect(ui->cboDeviceSubId,     SIGNAL(currentIndexChanged(int)),     this,   SLOT(requestDeviceInfo(int)));
    connect(ui->txtEditDateId,      SIGNAL(textEdited(QString)),          this,   SLOT(performKeyMasking(QString)));

    // set up all of the device strings
    noneList << tr("None");
    twoList << tr("Primary") << tr("Secondary");
    printerList << tr("Primary");
    dateMask = "000000";

    // set the device list with the corresponding ID's
    ui->cboDeviceId->addItem( tr( "Select Device" ), 0 );

    if ( session_->isServiceCounter() )
        ui->cboDeviceId->addItem( tr( "Scale" ), 14 );
    else
    {
        ui->cboDeviceId->addItem( tr( "Console" ), 14 );
        ui->cboDeviceId->addItem( tr( "Printer" ), 7 );
        ui->cboDeviceId->addItem( tr( "Weigher" ), 1 );
    }
    ui->cboDeviceId->addItem( tr( "Wireless Card" ), 99 );

    // set the subdevice list
    ui->cboDeviceSubId->addItems(noneList);

    ui->cboDeviceSubId->setEnabled(false);

    QDate today = QDate::currentDate();
    QString dateStr = today.toString("MM/dd/yy");                   // Do I need to use the system config date here?
    ui->txtEditDateId->setText(dateStr);

    update();

	session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceInstallationLog::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::translate()
{
    ui->retranslateUi( this );

    ui->cboDeviceId->setItemText( 0, tr("Select Device") );

    if ( session_->isServiceCounter() )
    {
        ui->cboDeviceId->setItemText( 1, tr("Scale") );
        ui->cboDeviceId->setItemText( 2, tr("Wireless Card") );
    }
    else
    {
        ui->cboDeviceId->setItemText( 1, tr("Console") );
        ui->cboDeviceId->setItemText( 2, tr("Printer") );
        ui->cboDeviceId->setItemText( 3, tr("Weigher") );
        ui->cboDeviceId->setItemText( 4, tr("Wireless Card") );
    }
    int deviceIndex = (ui->cboDeviceId->itemData(ui->cboDeviceId->currentIndex())).toInt();

    switch( deviceIndex )
    {
        case 7:
            ui->cboDeviceSubId->setItemText(0, tr("Primary"));
            break;
        default:
            ui->cboDeviceSubId->setItemText(0, tr("None"));
            break;
    }

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::updateDeviceInfo(const SessionMessage& msg)
{
    // 138,2 - Processes the 138,2 message
    
    DomInstallationLogDeviceData m( msg );
    
    int kind = m.deviceKind().toInt();

    int deviceId = m.deviceType().toInt();

    switch(deviceId)                                                // some of the stuff in here not yet supported
    {
        case 14:
            ui->lblEditKind->setText("");
            break;
        case 1:
            if (kind == 1)
                ui->lblEditKind->setText(tr("Ultima"));
            else if (kind == 2)
                ui->lblEditKind->setText(tr("USB"));
            else
                ui->lblEditKind->setText(tr("Unknown"));
            break;
        case 99:
            ui->lblEditKind->setText("");
            break;
        case 7:
            if (kind == 1)
                ui->lblEditKind->setText(tr("EPP"));
            else if (kind == 2)
                ui->lblEditKind->setText(tr("ESS"));
            else if (kind == 3)
                ui->lblEditKind->setText(tr("ESC"));
            else
                ui->lblEditKind->setText(tr("Unknown"));
            break;
        default:
            ui->lblEditKind->setText("");
            break;
    }

    ui->txtEditSerialId->setText( m.serialNum() );

    QDate today = QDate::currentDate();
    QString dateStr = today.toString("MM/dd/yy");                   // I think I need to use the configuration date here, or NOT?
    ui->txtEditDateId->setText(dateStr);

    ui->txtEditNoteId->clear();
    ui->txtInstallNote->appendPlainText( m.deviceHistory() );

    // turns out I really may not need this after all.
    if ( installationLogDeviceData_ )
        *installationLogDeviceData_ = m;
}

void ServiceInstallationLog::sendDeviceInfo()
{
    int myDevice = installationLogDeviceData_->deviceType().toInt();

    if(myDevice > 0)
    {
        QString msg;
        msg += installationLogDeviceData_->deviceType() + "," +
                installationLogDeviceData_->deviceSubtype() + "," +
                installationLogDeviceData_->deviceKind() + "," +
                ui->txtEditSerialId->text() + "," +
                "\"" + ui->txtInstallNote->toPlainText() + "\"";

        session_->send(PeripheralDev, PeripheralDtSave, msg);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::setupSubIdList(int index)
{
    Q_UNUSED( index );

    QStringList theList;

    int deviceIndex = (ui->cboDeviceId->itemData(ui->cboDeviceId->currentIndex())).toInt();

    switch( deviceIndex )
    {
        case 7:
            ui->cboDeviceSubId->setItemText(0, tr("Primary"));
            break;
        default:
            ui->cboDeviceSubId->setItemText(0, tr("None"));
            break;
    }

    update();

    ui->cboDeviceSubId->setCurrentIndex( 0 );

    if (ui->cboDeviceSubId->count() >= 1)
        ui->cboDeviceSubId->setEnabled( true );
    else
        ui->cboDeviceSubId->setEnabled( false );

    // Clear out the Device History (Install Note) text
    ui->txtInstallNote->clear();

    requestDeviceInfo();
}

void ServiceInstallationLog::requestDeviceInfo()
{
    bool sendRequest = true;
    QString msgData = "";
    int deviceId = (ui->cboDeviceId->itemData(ui->cboDeviceId->currentIndex())).toInt();
    int deviceSubId = ui->cboDeviceSubId->currentIndex();

    switch(deviceId)
    {
        case 14:
        case 1:
        case 99:
            msgData = QString::number(deviceId) + ",0";
            break;
        case 7:
            msgData = QString::number(deviceId) + "," + QString::number(deviceSubId);
            break;
        default:
            sendRequest = false;
            break;
    }

    if (sendRequest)
        session_->send(PeripheralDev, ReadCreatePeripheral, msgData);        //138,22

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::addNote()
{
    QString newNote =  ui->txtEditDateId->text() + " : ";

    if ( ui->txtEditSerialId->text().length() > 0 )
        newNote += ui->txtEditSerialId->text() + " : ";

    newNote += ui->txtEditNoteId->toPlainText() + "\n";

    if ( ui->cboDeviceId->currentIndex() > 0 )
    {
        ui->txtInstallNote->appendPlainText( newNote );

        sendDeviceInfo();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::performKeyMasking(QString key)
{
    LOG_CLASS_INFO( "ServiceInstallationLog", "performKeyMasking" );

    // when we come in here not a single key is passed in, but the entire text field is passed in
    // we also don't know when a clear key has been pressed so we don't know when to clear the
    // datemask. What I was able to figure out is when clear is pressed, the next character that
    // comes in is the next single character pressed. So when key comes in, and the length of
    // key is just 1, assume (yea I know) that the clear button was pressed and at that time we
    // can reset the mask to 000000. Hmmmm, what if the user backspaced? Im going to treat it as
    // a clear. Otherwise there are too many permatations of how far they backspaced.

    bool isBS = false;
    QString mask = "";

    if( key.length()  < 9 )
    {
        dateMask = "000000";

        if( key.length() > 1)
            isBS = true;                                            // on a CLEAR key.length = 1
    }


    if( isBS )
    {
        mask = "00/00/00";                                          // user clicked backspace
        ui->txtEditDateId->setText(mask);
    }
    else
    {
        int len = ui->txtEditDateId->text().length() - 1;               // essentially the 3 lines above being tested
        QString isCharAnInt = key.mid( len );                           // grab the character that was just entered
        bool ok = true;                                                 // people are going to be good so assume it true
        //int isIt = isCharAnInt.toInt(&ok, 10);                          // did the person enter a digit, if so true, if a character, false

        if(ok)                                                          // ok, we have a good 0-9 digit entered - now do something
        {
            dateMask += key.mid( len );                                 // add the good digit to the dateMask on the right side end.

            dateMask = dateMask.mid(1,dateMask.length());               // chop off the left most digit

            QString testMaskPartOne = dateMask.mid(0,2);
            QString testMaskPartTwo = dateMask.mid(2,2);
            QString testMaskPartTre = dateMask.mid(4,2);

            mask = testMaskPartOne + "/" + testMaskPartTwo + "/" + testMaskPartTre;   // slice it up
            ui->txtEditDateId->setText(mask);
        }
        else
        {
            // Evidentally the user entered something that wasn't a digit in the 0-9 range.
            // Therefore we need to chop that character off. Don't update any masks or anything
            QString tmpStr = ui->txtEditDateId->text();
            int tmpLen = tmpStr.length() - 1;
            tmpStr = tmpStr.mid(0,tmpLen);                              // chop that last digit back off
            ui->txtEditDateId->setText( tmpStr );

            LOG_DEBUG( "You can only enter 0-9", NULL );                       // Is there a way to disable chars on entry field?
        }
    }
}

void ServiceInstallationLog::clearFocusWidget()
{
    dateMask = "000000";
    ui->txtEditDateId->setText("00/00/00");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceInstallationLog::onSessionMessage( const SessionMessage& msg )
{
    if (( PeripheralDev == msg[0].toInt() ) && ( PeripheralDtWrite == msg[1].toInt() ))
    {
        // 138,2 - update the installation log
        updateDeviceInfo(msg);
    }
}
