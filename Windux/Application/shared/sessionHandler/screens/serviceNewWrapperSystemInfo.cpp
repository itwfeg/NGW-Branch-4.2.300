/* @file serviceNewWrapperSystemInfo.cpp
 * serviceNewWrapperSystemInfo
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief Shows Wrapper System info
 */

#include "serviceNewWrapperSystemInfo.h"
#include "ui_serviceNewWrapperSystemInfo.h"

#include "../sessionmainwindow.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/servicemode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <logmanager.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QTimer>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

serviceNewWrapperSystemInfo::serviceNewWrapperSystemInfo(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperSystemInfo),
    isExitingSysInfo_( false )
{
    ui->setupUi(this);
    scaleScreen();

    //acknowledgeSystemInfoMessage = true;
    internalConfigData_ = &session_->scaleConfig->internalConfig;
    internalConfigData_->messagesSuppressed = true;

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    connect(ui->butErase, SIGNAL(clicked()), this, SLOT(onEraseButClicked()));
    connect(ui->easterEgg, SIGNAL(clicked()), this, SLOT(bunnyComesOut()));
    connect(ui->butSave, SIGNAL(clicked()), this, SLOT(onSaveButClicked()));
    connect(ui->butPrint, SIGNAL(clicked()), this, SLOT(onPrintButClicked()));

    connect(ui->leSerialNumber, SIGNAL(textChanged(QString)), this, SLOT(truncatingSerialNumber(QString)));
//    connect(ui->leSerialNumber, SIGNAL(textChanged(QString)), this, SLOT(serailNumberChanged(QString)));
//    connect(ui->leSerialNumber, SIGNAL(textChanged(QString)), this, SLOT(serialNumberChanged(QString)));
    connect(ui->leDateOfManufacture, SIGNAL(textChanged(QString)), this, SLOT(manuDateChanged(QString)));
    connect(ui->leDateOfInstallation, SIGNAL(textChanged(QString)), this, SLOT(installDateChanged(QString)));
    connect(ui->leCycleCount, SIGNAL(textChanged(QString)), this, SLOT(cycleCountChanged(QString)));

   // ui->leSerialNumber->setValidator(serialNumberValidator_ = new QIntValidator(0, INT_MAX, this));
   // ui->leSerialNumber->setDecimalPlaces(0);
   // ui->leSerialNumber->setInputWrapping(true);
   // ui->leSerialNumber->setInputLeadingZeros(true);
   // serialNumberValidator_->setTop(makeValidatorTop(12));

    // as per Gary, lifetime of wrapper is 1000000 cycles so limit input
    ui->leCycleCount->setValidator(cycleCountValidator_ = new QIntValidator(0, 9999999, this));
    ui->leCycleCount->setDecimalPlaces(0);
    ui->leCycleCount->setInputWrapping(true);
    //ui->leCycleCount->setInputTruncation(true);

    //ui->leSerialNumber->setInputMask("999999999999");
    ui->leSerialNumber->setMaxLength(15);
    ui->leSerialNumber->setCursorPosition(0);

    initModelNumber = -1;

    update();
    this->initialize();
    translate();
}

void serviceNewWrapperSystemInfo::manuDateChanged(QString incomingDate)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "manuDateChanged" );
    ui->leDateOfManufacture->setText(truncatingDate(incomingDate));
    ui->butSave->raise();
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    LOG_DEBUG( "(%s)", qPrintable(incomingDate) );
}

void serviceNewWrapperSystemInfo::installDateChanged(QString incomingDate)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "installDateChanged" );
    ui->leDateOfInstallation->setText(truncatingDate(incomingDate));
    ui->butSave->raise();
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    LOG_DEBUG( "(%s)", qPrintable(incomingDate) );
}

void serviceNewWrapperSystemInfo::cycleCountChanged(QString newCycleCount)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "cycleCountChanged" );
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    LOG_DEBUG( "(%s)", qPrintable(newCycleCount) );
}

void serviceNewWrapperSystemInfo::serialNumberChanged(QString newSerialNumber)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "serialNumberChanged" );
    ui->butSave->raise();
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    LOG_DEBUG( "(%s)", qPrintable(newSerialNumber) );
}


void serviceNewWrapperSystemInfo::truncatingSerialNumber(QString incomingSerial)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "truncatingSerialNumber" );
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
            //ui->leSerialNumber->setText(incomingSerial.mid(1));
            //qDebug() << "outgoing is : " << incomingSerial.mid(1);
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
    ui->butSave->raise();
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    LOG_DEBUG( "(%s)", qPrintable(incomingSerial) );
}

bool serviceNewWrapperSystemInfo::eventFilter( QObject *obj, QEvent *event )
{
    QString msg;
    return _Mybase::eventFilter( obj, event );
}

QString serviceNewWrapperSystemInfo::truncatingDate(QString incomingDate)
{
    LOG_CLASS_INFO( "sNWSystemInfo", "truncatingDate" );
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
    LOG_DEBUG( "(%s):  Returning %s.", qPrintable(incomingDate), qPrintable(finalDate) );
    return finalDate;
}

void serviceNewWrapperSystemInfo::bunnyComesOut()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "bunnyComesOut" );
    ui->label_warning->show();
    ui->butErase->show();
    ui->butErase->setEnabled(true);
    LOG_DEBUG( "Enable the Erase button", NULL );
}

void serviceNewWrapperSystemInfo::onSaveButClicked()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "onSaveButClicked" );

    // SOF-2527: Do not allow user to save with "No Model"
    if( 0 == ( ui->cboxModelNumber->itemData(ui->cboxModelNumber->currentIndex() ) ) )
    {
        MessageBoxDialog dialog( real_time_priority, mainWindow_ );

        dialog.setTitle( tr( "SAVE FAILED" ) );
        dialog.setText( tr( "You must select a valid Model Number.\n\nPress OK to continue." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );
        dialog.exec();

        return;
    }

    if (ui->butSave->isEnabled())
    {
        QString msg = ui->leSerialNumber->text().replace("-", "");
        // prepare dates
        msg += "," + ui->leDateOfManufacture->text().replace("/", "");

        msg += "," + ui->leDateOfInstallation->text().replace("/", "");

        msg += "," + ui->cboxModelNumber->itemData(ui->cboxModelNumber->currentIndex()).toString();
        msg += "," + ui->leCycleCount->text();
	    LOG_DEBUG( "Sending 9,146,%s", qPrintable(msg) );
        session_->send(Wrapper, WrEvolSystemInfoWrite, msg);
        ui->butSave->setEnabled(false);
        acknowledgeSystemInfoMessage = true;
        ui->butErase->hide();
        ui->butSave->hide();
    }
    LOG_DEBUG( "Done", NULL );
}

void serviceNewWrapperSystemInfo::onPrintButClicked()
{
    // Send message to generate the Summary Report
    QString title( "WRAPPER SYSTEM INFO");
    QString msg;

    msg =  "Date:  ";
    msg += QDate::currentDate().toString("MM/dd/yyyy  ");
    msg += QTime::currentTime().toString("hh:mm:ss AP");
    msg += "\n";

    msg += "Serial:  ";
    msg += scaleSerial;
    msg += "\n";

    msg += "\n";

    msg += "Wrapper Model:  ";
    msg += ui->cboxModelNumber->currentText();
    msg += "\n";

    msg += "Wrapper Serial:  ";
    msg += ui->leSerialNumber->text();
    msg += "\n";

    msg += "Manufacture Date:  ";
    msg += ui->leDateOfManufacture->text();
    msg += "\n";

    msg += "Installation Date:  ";
    msg += ui->leDateOfInstallation->text();
    msg += "\n";

    msg += "Cycle Count:  ";
    msg += ui->leCycleCount->text();
    msg += "\n";

    session_->send(ReportManagerDev,StartReport,"2,0,0,0,0," + title + "," + msg );
}


void serviceNewWrapperSystemInfo::onEraseButClicked()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "onEraseButClicked" );
    session_->send(Wrapper, WrEvolSystemInfoErase);
    ui->butSave->show();
    ui->butSave->setEnabled(true);
    ui->butErase->hide();
    // hide the print button on purpose that way they cannot print bogus information
    ui->butPrint->hide();
    LOG_DEBUG( "Done", NULL );
}


serviceNewWrapperSystemInfo::~serviceNewWrapperSystemInfo()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "Destructor" );
    delete ui;
    LOG_DEBUG( "Done", NULL );
}

int serviceNewWrapperSystemInfo::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceNewWrapperSystemInfo::initialize()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "initialize" );
    hasChanged = false;

    // SOF-2909: Request serial number earlier
    scaleSerial = "";
    session_->send( MfgTestDev, RequestMfgData );

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->scaleModel = scaleConfigData->scaleModel();
    ui->label_warning->hide();
    ui->butErase->hide();
    ui->butSave->hide();

    ui->cboxModelNumber->setDisabled(true);
    ui->leCycleCount->setDisabled(true);
    ui->leDateOfInstallation->setDisabled(true);

    //ui->deDateOfInstallation->setDisabled(true);
    //ui->deDateOfManufacture->setDisabled(true);
    //ui->leDateOfInstallation->setDisabled(true);
    //ui->leDateOfManufacture->setDisabled(true);
    ui->leSerialNumber->setDisabled(true);

    session_->send(Wrapper, WrReqEvolSystemInfo);

    update();
    LOG_DEBUG( "Done", NULL );
}

void serviceNewWrapperSystemInfo::doSave()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "doSave" );
    hasChanged = true;
    //set the saving state for the destructor
    LOG_DEBUG( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSystemInfo::translate()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "translate" );

    ui->retranslateUi(this);
    ui->cboxModelNumber->setItemText(0, tr("No Model"));

    update();
    LOG_DEBUG( "Done", NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperSystemInfo::onScreenChanging()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "onScreenChanging" );

    LOG_DEBUG( "onScreenChanging() START", NULL );

    isExitingSysInfo_ = true;

    internalConfigData_->sysInfoUpdatesInProgress = false;
    internalConfigData_->sysInfoUpdatesAreComplete = false;
    internalConfigData_->sysInfoDialogSuppressed = false;

    LOG_DEBUG( "Calling onSaveButClicked()", NULL );

    onSaveButClicked();

    // Need to check if the dialog "Wrapper is updating ..." is displayed
    startScreenExitTimer_ = new QTimer( this );
    startScreenExitTimer_->setSingleShot( true );
    startScreenExitTimer_->setInterval( 500 );
    okToCheckForWaitMessage = false;

    connect( startScreenExitTimer_, SIGNAL( timeout() ), SLOT( chkForWaitMessage() ) );

    startScreenExitTimer_->start();

    //-----------------------------------------------------------------------------------------------------
    // First need to wait for the "Wrapper is updating ..." dialog to be displayed ... if it is going to be
    //-----------------------------------------------------------------------------------------------------
    LOG_DEBUG( "before while okToCheckForWaitMessage", NULL);

    while ( !okToCheckForWaitMessage )
    {
        LOG_DEBUG( "Inside LOOP for okToCheckForWaitMessage", NULL);
        sleepFor( 50 ); // so this while-loop doesn't peg the CPU
    }

    startScreenExitTimer_->stop();

    LOG_DEBUG( "after while okToCheckForWaitMessage", NULL);

    if( internalConfigData_->sysInfoUpdatesInProgress == true )
    {
        LOG_DEBUG( "if internalConfigData_->sysInfoUpdatesInProgress", NULL);

        //----------------------------------------------------------
        // The dialog for "Wrapper is updating ..." is displayed.
        // Now wait until it is dismissed before we exit the screen.
        //----------------------------------------------------------

        LOG_DEBUG ( "Waiting for \"Wrapper is updating ...\" to be dismissed", NULL );

        while( internalConfigData_->sysInfoUpdatesAreComplete == false )
        {
            LOG_DEBUG( "Inside LOOP for sysInfoUpdatesAreComplete", NULL);
            sleepFor( 500 );
        }

        LOG_DEBUG ( "Received dismiss for \"Wrapper is updating ...\"", NULL );
    }
    else
        LOG_DEBUG( "else internalConfigData_->sysInfoUpdatesInProgress", NULL);

    internalConfigData_->messagesSuppressed = false;
    internalConfigData_->sysInfoUpdatesInProgress = false;
    internalConfigData_->sysInfoUpdatesAreComplete = false;
    internalConfigData_->sysInfoDialogSuppressed = false;
    isExitingSysInfo_ = false;

    LOG_DEBUG( "onScreenChanging() END", NULL );

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSystemInfo::chkForWaitMessage()
{
    LOG_CLASS_INFO( "sNWSystemInfo", "chkForWaitMessage" );

    LOG_DEBUG( "Inside TIMER for chkForWaitMessage: EXPIRED", NULL);
    okToCheckForWaitMessage = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSystemInfo::sleepFor( int ms ) const
{
    // amount of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

// SOF-1915
///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperSystemInfo::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    LOG_CLASS_INFO( "sNWSystemInfo", "isErrorMessageHandler" );

    if( !isExitingSysInfo_ )
        return false;

    if ( ( Error == errorMsg[0].toInt() ) &&
         ( Wrapper == errorMsg[1].toInt() ) &&
         ( 9705 == errorMsg[2].toInt() ) &&
         ( 3 == errorMsg[3].toInt() ) )
    {
        //------------------------------------------------------------------
        // Message that "Wrapper is updating ..." is displayed: 103,9,9705,3
        //------------------------------------------------------------------

        internalConfigData_->sysInfoUpdatesInProgress = true;
        internalConfigData_->sysInfoDialogSuppressed = false;

        LOG_DEBUG("isErrorMessageHandler(): Received 103,9,9705 message", NULL );

        if( mainWindow_->isDialogOpen() )
        {
            //-------------------------------------------------------------------
            // When going to another screen via the menu, the UI was crashing due
            // to the message dialog, menu and processEvents() in the sleepFor()
            // colliding.  So in this case if the menu is open, indicated by
            // isDialogOpen(), we suppress the message dialog.
            //-------------------------------------------------------------------
            internalConfigData_->sysInfoDialogSuppressed = true;

            LOG_DEBUG("isErrorMessageHandler(): Suppressed dialog", NULL );

            return true;
        }
    }
    else if ( ( Error == errorMsg[0].toInt() ) &&
              ( ErrRemoveAnyModalDialogDevice == errorMsg[1].toInt() ) &&
              ( 9705 == errorMsg[2].toInt() ) &&
              ( 3 == errorMsg[3].toInt() ) )
    {
        //--------------------------------------------------------------------
        // Message that "Wrapper is updating ..." is dismissed: 103,255,9705,3
        //--------------------------------------------------------------------

        internalConfigData_->sysInfoUpdatesAreComplete = true;

        LOG_DEBUG( "isErrorMessageHandler(): Received 103,255,9705 message", NULL );

        if( internalConfigData_->sysInfoDialogSuppressed )
            return true;
    }

    return _Mybase::isErrorMessageHandler( errorMsg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSystemInfo::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "sNWSystemInfo", "onSessionMessage" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    if (Wrapper == device && WrEvolSystemInfo == event)
    {
        DomEvolWrapperSystemInformation m(msg);

        ui->lblPartNumberValue->setText(m.boardPartNumber());
        ui->lblRevisionValue->setText(m.boardRev());
        QString tempMotorBoardManufactureDate = "";
        tempMotorBoardManufactureDate = m.boardManufactureDate().mid(0,2) + "/" + m.boardManufactureDate().mid(2,2) + "/" + m.boardManufactureDate().mid(4,2);
        ui->lblDateOfManufactureValue->setText(tempMotorBoardManufactureDate);

        if( initModelNumber == -1 )
        {
            initModelNumber = m.awsModelNumber();

            populateModelNumber( initModelNumber );

        }

        if(0 == m.awsModelNumber())
        {
            ui->cboxModelNumber->setEnabled(true);
            ui->leCycleCount->setEnabled(true);
            ui->leDateOfInstallation->setEnabled(true);
            ui->leDateOfManufacture->setEnabled(true);
            ui->leSerialNumber->setEnabled(true);
            ui->butSave->show();
            ui->butSave->setEnabled(true);

            // request serial number
            session_->send( MfgTestDev, RequestMfgData );
        }
        else
        {
            ui->cboxModelNumber->setEnabled(false);
            ui->leCycleCount->setEnabled(false);
            ui->leDateOfInstallation->setEnabled(false);
            ui->leDateOfManufacture->setEnabled(false);
            ui->leSerialNumber->setEnabled(false);
            ui->butSave->hide();
            ui->butSave->setEnabled(false);
            ui->butErase->hide();
        }

        //  Block all the signals on the fields while we update them.
        ui->cboxModelNumber->blockSignals(true);
        ui->leCycleCount->blockSignals(true);
        ui->leDateOfInstallation->blockSignals(true);
        ui->leDateOfManufacture->blockSignals(true);
        ui->leSerialNumber->blockSignals(true);

        int index( ui->cboxModelNumber->findData( QString::number( m.awsModelNumber() ) ) );

        if ( index < 0 )
            index = 0;

        ui->cboxModelNumber->setCurrentIndex( index );

        QString cycleCount( m.awsWrapperCycleCount() );
        int pos;

        if ( QValidator::Acceptable == cycleCountValidator_->validate( cycleCount, pos ) )
            ui->leCycleCount->setText(m.awsWrapperCycleCount());

        //qDebug() << "install date is: " << m.awsInstallDate();
        ui->leDateOfInstallation->setText(truncatingDate(m.awsInstallDate()));

        //qDebug() << "manu date is: " << m.awsManufactureDate();
        ui->leDateOfManufacture->setText(truncatingDate(m.awsManufactureDate()));

        // SOF-2909: Always set the Serial number to the value from the flash
        ui->leSerialNumber->setText( scaleSerial );

        if(m.awsManufactureDate() == "000000")
        {
            ui->leDateOfManufacture->setEnabled(true);
            ui->butSave->show();
            ui->butSave->setEnabled(true);
        }

        if(m.awsInstallDate() == "000000")
        {
            ui->leDateOfInstallation->setEnabled(true);
            ui->butSave->show();
            ui->butSave->setEnabled(true);
        }
        //acknowledgeSystemInfoMessage = false;

        //initialize();
        //reloadFields();

        //  Enable all the signals on the fields.
        ui->cboxModelNumber->blockSignals(false);
        ui->leCycleCount->blockSignals(false);
        ui->leDateOfInstallation->blockSignals(false);
        ui->leDateOfManufacture->blockSignals(false);
        ui->leSerialNumber->blockSignals(false);

        if (( "0" != ui->cboxModelNumber->currentData() ) && ( "000000" == m.awsInstallDate() ))
        {
            QTimer *prompt( new QTimer( this ) );
            prompt->setSingleShot( true );

            connect( prompt, SIGNAL(timeout()), SLOT(promptInstallDate()) );
            prompt->start( 0 );
        }
    }

    // scale serial info
    else if (( MfgTestDev == device ) && ( ReadMfgData == event ))
    {
        LOG_DEBUG( "got scale version info...", NULL );
        DomMfgReadData m( msg );

        scaleModel = m.scaleStyle();
        scaleSerial = m.serialNumber();
        dateOfManf = m.dateOfManufacture();

        if (( "0" == ui->cboxModelNumber->currentData() ) && ( "NGW" == scaleModel.left( 3 ) ))
        {
            QTimer *prompt( new QTimer( this ) );
            prompt->setSingleShot( true );

            connect( prompt, SIGNAL(timeout()), SLOT(promptAutoConfig()) );
            prompt->start( 0 );
        }
    }
}

void serviceNewWrapperSystemInfo::promptAutoConfig()
{
    LOG_CLASS_INFO( "serviceNewWrapperSystemInfo", "promptAutoConfig" );

    LOG_INFO( "model: %s", qPrintable( scaleModel ) );
    LOG_INFO( "serial: %s", qPrintable( scaleSerial ) );
    LOG_INFO( "dom: %s", qPrintable( dateOfManf ) );

    if ((( "NGW1" == scaleModel.left( 4 ) ) || ( "NGW2" == scaleModel.left( 4 ) )) && ( 10 <= scaleModel.length() ))
    {
        const QString direction( scaleModel.right( 2 ) );
        const QString generation( scaleModel.mid( 7, 1 ) ); // USB/CAN Hybrid=1; Full-CAN=2

        if (( "LR" != direction ) && ( "RL" != direction ))
        {
            LOG_WARN( "badly formed direction string", NULL );
            return;
        }

        MessageBoxDialog dialog( mainWindow_ );
        dialog.setTitle( tr( "AUTO POPULATE" ) );
        dialog.setText( tr( "Do you want to auto populate wrapper settings?" ) );
        dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

        int result = dialog.exec();

        if ( QDialog::Accepted != result )
            return;

        if ( "NGW2" == scaleModel.left( 4 ) )
        {
            if( "1" == generation )
            {
                //---------------
                // USB/CAN Hybrid
                //---------------

                if ( "RL" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "14" ) ) );
                else if ( "LR" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "15" ) ) );
            }
            else if( "2" == generation )
            {
                //---------
                // Full CAN
                //---------

                if ( "RL" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "20" ) ) );
                else if ( "LR" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "21" ) ) );
            }
        }
        else if ( "NGW1" == scaleModel.left( 4 ) )
        {
            if( "1" == generation )
            {
                //---------------
                // USB/CAN Hybrid
                //---------------

                if ( "RL" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "16" ) ) );
                else if ( "LR" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "17" ) ) );
            }
            else if( "2" == generation )
            {
                //---------
                // Full CAN
                //---------

                if ( "RL" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "22" ) ) );
                else if ( "LR" == direction )
                    ui->cboxModelNumber->setCurrentIndex( ui->cboxModelNumber->findData( QString( "23" ) ) );
            }
        }

        ui->leSerialNumber->setText( scaleSerial );

        // convert mm/dd/yyyy to mm/dd/yy
        if ( 10 == dateOfManf.length() )
            ui->leDateOfManufacture->setText( dateOfManf.mid( 0, 6 ) + dateOfManf.mid( 8, 2 ) );
    }
    else
    {
        LOG_WARN( "badly formed model number", NULL );
    }
}

void serviceNewWrapperSystemInfo::promptInstallDate()
{
    if( internalConfigData_->sysInfoUpdatesAreComplete )
    //if( internalConfigData_->sysInfoUpdatesInProgress )
        internalConfigData_->sysInfoUpdatesInProgress = false;

    MessageBoxDialog dialog( mainWindow_ );
    dialog.setTitle( tr( "INSTALLATION" ) );
    dialog.setText( tr( "Are you installing this unit today?\n\n YES - Fill in installation date\n NO - Do nothing" ) );
    dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

    int result = dialog.exec();

    if ( QDialog::Accepted != result )
        return;

    const QDate now( QDate::currentDate() );

    ui->leDateOfInstallation->setText( now.toString( "MM/dd/yy" ) );
}

//-----------------------------------------------------------------
// SOF-2295
// Only want to populate the Model Number combo box with the models
// for Full CAN or Hybrid USB/CAN.  Use the model number provided
// by the Wrapper firmware to determine this.
//-----------------------------------------------------------------
void serviceNewWrapperSystemInfo::populateModelNumber( int modelNumber )
{
#define HYBRID_CAN_MODEL_GROUP_   100
#define FULL_CAN_MODEL_GROUP_     200

    LOG_CLASS_INFO( "sNWSystemInfo", "populateModelNumber" );

    LOG_DEBUG( "populateModelNumber(): modelNumber=%d", modelNumber );

    if( modelNumber == 0 )
    {
        //------------------------------------------------------------
        // Wrapper firmware has no model number, so get the model name
        // programmed into flash, parse it, and use it to determine if
        // wrapper is Full CAN or Hybrid USB/CAN
        //------------------------------------------------------------

        ScaleConfiguration *scaleConfig_ = session_->scaleConfig;
        const QString modelName = scaleConfig_->scaleConfig.scaleName();

        LOG_DEBUG( "populateModelNumber(): modelName from flash=%s", qPrintable( modelName ) );

        if( ( 8 <= modelName.length() ) &&
            ( ( ( modelName.left( 3 ) == "NGW" ) && ( modelName.mid( 7,1) == "2" ) ) ||
              ( ( modelName.left( 3 ) ) == "NGS" )
            )
          )
        {
            modelNumber = FULL_CAN_MODEL_GROUP_;

            LOG_DEBUG( "populateModelNumber(): Setting to FULL CAN", NULL );
        }
        else
        {
            modelNumber = HYBRID_CAN_MODEL_GROUP_;

            LOG_DEBUG( "populateModelNumber(): Setting to HYBRID CAN", NULL );
        }
    }

    //-------------------------------
    // Load the Model Number combobox
    //-------------------------------

    ui->cboxModelNumber->clear();
    ui->cboxModelNumber->addItem(tr("No Model"),  QString("0"));

    switch( modelNumber )
    {
        //----------------------
        // Hybrid CAN/USB models
        //----------------------

        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case HYBRID_CAN_MODEL_GROUP_:

            ui->cboxModelNumber->addItem(tr("NGW2-RL"),   QString("14"));
            ui->cboxModelNumber->addItem(tr("NGW2-LR"),   QString("15"));
            ui->cboxModelNumber->addItem(tr("NGW1-RL"),   QString("16"));
            ui->cboxModelNumber->addItem(tr("NGW1-LR"),   QString("17"));
            ui->cboxModelNumber->addItem(tr("AWS-3RL"),   QString("10"));
            ui->cboxModelNumber->addItem(tr("AWS-3LR"),   QString("11"));
            ui->cboxModelNumber->addItem(tr("EWS-3RL"),   QString("12"));
            ui->cboxModelNumber->addItem(tr("EWS-3LR"),   QString("13"));
            ui->cboxModelNumber->addItem(tr("AWS-2RL"),   QString("8"));
            ui->cboxModelNumber->addItem(tr("AWS-2LR"),   QString("9"));
            ui->cboxModelNumber->addItem(tr("AWS-1RL"),   QString("1"));
            ui->cboxModelNumber->addItem(tr("AWS-1LR"),   QString("2"));
            ui->cboxModelNumber->addItem(tr("AWS-MLR"),   QString("3"));
            //ui->cboxModelNumber->addItem(tr("AWS-1LR_I"), QString("4")); removed as per Kraig
            //ui->cboxModelNumber->addItem(tr("AWS-1RL_I"), QString("5")); removed as per Kraig
            ui->cboxModelNumber->addItem(tr("AWS-1BRL"),  QString("6"));
            ui->cboxModelNumber->addItem(tr("AWS-1BLR"),  QString("7"));

            break;

        //----------------
        // Full CAN models
        //----------------

        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case FULL_CAN_MODEL_GROUP_:

            ui->cboxModelNumber->addItem(tr("NGW2-S2RL"), QString("20"));
            ui->cboxModelNumber->addItem(tr("NGW2-S2LR"), QString("21"));
            ui->cboxModelNumber->addItem(tr("NGW1-S2RL"), QString("22"));
            ui->cboxModelNumber->addItem(tr("NGW1-S2LR"), QString("23"));
            //ui->cboxModelNumber->addItem(tr("NGS1-I1RL"), QString("18")); removed as per Kraig
            //ui->cboxModelNumber->addItem(tr("NGS1-I1LR"), QString("19")); removed as per Kraig

            break;
    }
}
