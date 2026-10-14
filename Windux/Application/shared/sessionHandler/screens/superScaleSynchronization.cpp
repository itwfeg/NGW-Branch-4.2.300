/* @file superScaleSynchronization.cpp
 * SuperProductDefaults 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew
 */


#include "superScaleSynchronization.h"
#include "ui_superScaleSynchronization.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QDebug>

static const int HOSTNAME_AS_SERVER = 1;
static const int IP_ADDRESS_AS_SERVER = 2;
static const int STANDALONE_SCALE = 0;
static const int PRIMARY_SCALE = 1;
static const int SECONDARY_SCALE = 2;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart{
    class numericLineEdit;
}

SuperScaleSynchronization::SuperScaleSynchronization(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperScaleSynchronization),
    automaticDataTypes_(NULL),
    automaticUpdate_(NULL)
{
    if ( mainWindow_->isSupervisorMode() )
        automaticDataTypes_ = &dynamic_cast<SupervisorMode*>( mode_ )->automaticDataTypes_;

    if ( mainWindow_->isSupervisorMode() )
        automaticUpdate_ = &dynamic_cast<SupervisorMode*>( mode_ )->automaticUpdate_;

    ui->setupUi(this);
    scaleScreen();

    internalConfigData = &session_->scaleConfig->internalConfig;

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SuperScaleSynchronization::~SuperScaleSynchronization()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::initialize()
{
    // initialize vars
    ignoreSignals_ = false;
    hasChanged_ = false;

    localScaleType_ = STANDALONE_SCALE;                             // standalone=0,primary=1,2=secondary
    localHostType_ = HOSTNAME_AS_SERVER;                            // 0=N/A, 1=hostname, 2=ipaddress

    // hide stuff
    ui->lblHostServerName->setVisible(false);
    ui->butFlipIt->setVisible(false);
    ui->txtHostName->setVisible(false);
    ui->lblDot1->setVisible(false);
    ui->lblDot2->setVisible(false);
    ui->lblDot3->setVisible(false);
    ui->txtIpAddress1->setVisible(false);
    ui->txtIpAddress2->setVisible(false);
    ui->txtIpAddress3->setVisible(false);
    ui->txtIpAddress4->setVisible(false);
    ui->lblSyncStatus->setText("");

    // initialize vars
    connect(ui->radStandAloneScale,         SIGNAL(clicked()),       	  this,   SLOT(clickedStandAlone()));
    connect(ui->radPrimaryScale,            SIGNAL(clicked()),       	  this,   SLOT(clickedPrimary()));
    connect(ui->radSecondaryScale,          SIGNAL(clicked()),       	  this,   SLOT(clickedSecondary()));

    connect(ui->chkTransactions,            SIGNAL(clicked()),       	  this,   SLOT(checkboxChanged()));
    connect(ui->chkTransactionTypes,        SIGNAL(clicked()),       	  this,   SLOT(checkboxChanged()));
    connect(ui->chkExpirationLabels,        SIGNAL(clicked()),       	  this,   SLOT(checkboxChanged()));
    connect(ui->chkOperators,               SIGNAL(clicked()),       	  this,   SLOT(checkboxChanged()));
    connect(ui->chkOperatorNotes,           SIGNAL(clicked()),       	  this,   SLOT(checkboxChanged()));

    connect(ui->txtHostName,                SIGNAL(editingFinished()),    this, SLOT(textboxChanged()));
    connect(ui->txtIpAddress1,              SIGNAL(editingFinished()),    this, SLOT(textboxChanged()));
    connect(ui->txtIpAddress2,              SIGNAL(editingFinished()),    this, SLOT(textboxChanged()));
    connect(ui->txtIpAddress3,              SIGNAL(editingFinished()),    this, SLOT(textboxChanged()));
    connect(ui->txtIpAddress4,              SIGNAL(editingFinished()),    this, SLOT(textboxChanged()));

    connect(ui->butFlipIt,                  SIGNAL(clicked()),            this, SLOT(clickFlipIt()));
    connect(ui->butSaveIt,                  SIGNAL(clicked()),            this, SLOT(clickSaveIt()));

    updateStatusLine(internalConfigData->statusLineError.toInt(), 0);        // pass in previous message

    ui->txtIpAddress1->setValidator(ipTarget1_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtIpAddress1->setDecimalPlaces(0);
    ui->txtIpAddress1->setInputWrapping(true);
    ipTarget1_->setTop(255);

    ui->txtIpAddress2->setValidator(ipTarget2_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtIpAddress2->setDecimalPlaces(0);
    ui->txtIpAddress2->setInputWrapping(true);
    ipTarget2_->setTop(255);

    ui->txtIpAddress3->setValidator(ipTarget3_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtIpAddress3->setDecimalPlaces(0);
    ui->txtIpAddress3->setInputWrapping(true);
    ipTarget3_->setTop(255);

    ui->txtIpAddress4->setValidator(ipTarget4_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtIpAddress4->setDecimalPlaces(0);
    ui->txtIpAddress4->setInputWrapping(true);
    ipTarget4_->setTop(255);


    update();

    // send initial messaging
    session_->send(ConfigurationDev, CfgSynchronizationRead);       // read the synchronization type
    session_->send(ConfigurationDev, CfgSyncListRead);              // read the fields that are synchronized

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperScaleSynchronization::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::translate()
{        
    ui->retranslateUi( this );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperScaleSynchronization::onScreenChanging()
{
    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::updateStatusLine(int errorNum, int errorEvent)
{
    // update the status line with one of these messages

    internalConfigData->statusLineError = QString::number( errorNum );

    if( errorEvent == 255)
    {
        internalConfigData->statusLineError = "0";
        ui->lblSyncStatus->setText("");
    }
    else
    {
        switch( errorNum )
        {
            case 146000:
                ui->lblSyncStatus->setText(tr("Unknown Status"));
                break;
            case 146001:
                ui->lblSyncStatus->setText(tr("Synchronization active"));
                 break;
            case 146002:
                ui->lblSyncStatus->setText(tr("Sending changes"));
                break;
            case 146003:
                ui->lblSyncStatus->setText(tr("Receiving changes"));
                break;
            case 146004:
                ui->lblSyncStatus->setText(tr("Applying changes"));
                break;
            case 146005:
                ui->lblSyncStatus->setText(tr("Can't find primary"));
                break;
            case 146006:
                ui->lblSyncStatus->setText("");
                break;
            case 1460020:
                ui->lblSyncStatus->setText(tr("Synchronizing scales"));
                break;
            default:
                ui->lblSyncStatus->setText("");
                break;
        }
    }
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::saveDataOnExit()
{
    // write out the DOM data that has already been saved

    if( hasChanged_ )
    {
        QString hostNameStr = "";

        if(localScaleType_ == SECONDARY_SCALE)                      // if we have a secondary
        {
            if(localHostType_ == IP_ADDRESS_AS_SERVER )             // if we have an ip address
            {
                hostNameStr  = ui->txtIpAddress1->text() + ".";
                hostNameStr += ui->txtIpAddress2->text() + ".";
                hostNameStr += ui->txtIpAddress3->text() + ".";
                hostNameStr += ui->txtIpAddress4->text();
            }
            else if( localHostType_ == HOSTNAME_AS_SERVER )
                hostNameStr += ui->txtHostName->text();             // else we have a host name

        }

        QString dataTypesStr =
            automaticDataTypes_->configurations() + "," +
            automaticDataTypes_->emailAlerts() + "," +
            automaticDataTypes_->productDefaults() + "," +
            automaticDataTypes_->classes() + "," +
            automaticDataTypes_->expandedTexts() + "," +
            automaticDataTypes_->specialMsgs() + "," +
            automaticDataTypes_->nutritionFacts() + "," +
            automaticDataTypes_->productNotes() + "," +
            automaticDataTypes_->marqueeTexts() + "," +
            QString(ui->chkTransactions->isChecked() ? "1" : "0") + "," +
            QString(ui->chkTransactionTypes->isChecked() ? "1" : "0") + "," +
            automaticDataTypes_->categories() + "," +
            automaticDataTypes_->specials() + "," +
            automaticDataTypes_->coolCategories() + "," +
            automaticDataTypes_->coolCountries() + "," +
            automaticDataTypes_->coolProdSteps() + "," +
            automaticDataTypes_->coolPreTexts() + "," +
            automaticDataTypes_->coolTexts() + "," +
            automaticDataTypes_->coolListTexts() + "," +
            automaticDataTypes_->coolLists() + "," +
            automaticDataTypes_->coolCatCountries() + "," +
            automaticDataTypes_->coolCatProdSteps() + "," +
            automaticDataTypes_->coolCatPreTexts() + "," +
            automaticDataTypes_->products() + "," +
            QString(ui->chkExpirationLabels->isChecked() ? "1" : "0") + "," +
            automaticDataTypes_->labels() + "," +
            automaticDataTypes_->labelTexts() + "," +
            QString(ui->chkOperatorNotes->isChecked() ? "1" : "0") + "," +
            QString(ui->chkOperators->isChecked() ? "1" : "0") + "," +
            automaticDataTypes_->softkeyGroups() + "," +
            automaticDataTypes_->softkeyStyles() + "," +
            automaticDataTypes_->softkeyFeatures()  + "," +
            automaticDataTypes_->softkeys() + "," +
            automaticDataTypes_->labelGraphics() + "," +
            automaticDataTypes_->catGraphics() + "," +
            automaticDataTypes_->prodGraphics() + "," +
            automaticDataTypes_->softkeyGraphics()  + "," +
            automaticDataTypes_->specialsGraphics();

        session_->send(ConfigurationDev, CfgSyncListSave, dataTypesStr);
        QString msgStr = QString::number(localScaleType_) + "," + hostNameStr;
        session_->send(ConfigurationDev, CfgSynchronizationSave, msgStr);

        hasChanged_ = false;
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::showHostName(bool displayField)
{
    ui->lblHostServerName->setVisible( displayField );
    ui->txtHostName->setVisible( displayField );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::showIpAddress( bool displayField )
{
    ui->lblIpAddress->setVisible( displayField );
    ui->txtIpAddress1->setVisible( displayField );
    ui->txtIpAddress2->setVisible( displayField );
    ui->txtIpAddress3->setVisible( displayField );
    ui->txtIpAddress4->setVisible( displayField );
    ui->lblDot1->setVisible( displayField);
    ui->lblDot2->setVisible( displayField);
    ui->lblDot3->setVisible( displayField);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::showFlipIt( bool displayField )
{
    // by default, when we decide to show this button, its going to be the hostname field that is
    // being displayed, so we want to provide the opposite option to switch to, ie IP Address

    ui->butFlipIt->setVisible( displayField );
    localHostType_ = HOSTNAME_AS_SERVER;                            // flip the value

    if( displayField )
        ui->butFlipIt->setText(tr("Use IP Address"));
    else
        ui->butFlipIt->setText("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::showHostNameStuff(bool displayField)
{
    clearIpAddress();
    clearHostName();
    showIpAddress( false );                                         // always hide this field

    if ( displayField )
        ui->gbScaleType->setGeometry( scaleRectForScreen( QRect( 50, 40, 531, 301 ) ) );
    else
        ui->gbScaleType->setGeometry( scaleRectForScreen( QRect( 50, 40, 531, 221 ) ) );

    showFlipIt( displayField );
    showHostName( displayField );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clearIpAddress()
{
    ui->txtIpAddress1->setText("0");
    ui->txtIpAddress2->setText("0");
    ui->txtIpAddress3->setText("0");
    ui->txtIpAddress4->setText("0");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clearHostName()
{
    ui->txtHostName->setText("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::updateScaleSynchronizationType(const SessionMessage &msg)
{
    // update the scale synchronization as primary, secondary, or standalone. If we have a secondary
    // scale, the primary scale address or host name the secondary is pointing to is also updated.
    // Received a 112,181

    DomAutomaticUpdate m( msg );

    if ( automaticUpdate_ )
        *automaticUpdate_ = m;

    ignoreSignals_ = true;

    localScaleType_ = automaticUpdate_->scaleType();
    QString localHostName  = automaticUpdate_->hostName();

    if ( localScaleType_ == SECONDARY_SCALE )
        ui->radSecondaryScale->setChecked( true );                  // secondary
    else if( localScaleType_ == PRIMARY_SCALE )
        ui->radPrimaryScale->setChecked( true );                    // primary
    else                                                            // should always be 0 here, but just in case set it standalone by default
        ui->radStandAloneScale->setChecked( true );                 // standalone

    if( localScaleType_ < 2 )
    {
        // if we are a standalone or a primary, we dont need the host name or IP address of primary scale
        showHostNameStuff( false );
    }
    else if(localScaleType_ == SECONDARY_SCALE )
    {
        // if we are a secondary, then we need the host name or IP address of primary scale
        if( localHostName.length() > 0)
        {
            QStringList myHost = localHostName.split(".");
            bool isIpAddr = false;

            if(myHost.length() == 4)
                isIpAddr = true;

            showHostNameStuff( true );

            if( isIpAddr )
            {
                // If we have an IP address
                localHostType_ = HOSTNAME_AS_SERVER;                // yes this should be 2, but clickFlipIt() flips it to 2 so init to 1
                clickFlipIt();
                ui->txtIpAddress1->setText(myHost.at(0));
                ui->txtIpAddress2->setText(myHost.at(1));
                ui->txtIpAddress3->setText(myHost.at(2));
                ui->txtIpAddress4->setText(myHost.at(3));
            }
            else
            {
                // Then we have a host name
                localHostType_ = IP_ADDRESS_AS_SERVER;              // yes this should be 1, but clickFlipIt() flips it to 1 sso init to 2.
                clickFlipIt();
                ui->txtHostName->setText( localHostName );
            }
        }
    }

    ignoreSignals_  = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::updateScaleSynchronizationData(const SessionMessage &msg)
{
    // update the scale synchronization checkboxes being saved
    // Recieved a 112,185

    DomAutomaticDataTypes m( msg );

    if ( automaticDataTypes_ )
        *automaticDataTypes_ = m;

    ignoreSignals_ = true;

    ui->chkTransactions->setChecked( m.transactions() );
    ui->chkTransactionTypes->setChecked( m.transactionTypes() );
    ui->chkExpirationLabels->setChecked( m.expirationLabels() );
    ui->chkOperators->setChecked( m.operators() );
    ui->chkOperatorNotes->setChecked( m.operatorNotes() );

    hasChanged_ = 0;
    ignoreSignals_ = false;

    update();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clickedStandAlone()
{
    // clicked standalone - send messages and update screen

    if( ignoreSignals_ )
        return;

    localScaleType_ = STANDALONE_SCALE;
    showHostNameStuff( false );

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clickedPrimary()
{
    // clicked primary - send messages and update screen

    if( ignoreSignals_ )
        return;

    localScaleType_  = PRIMARY_SCALE;
    showHostNameStuff( false );

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clickedSecondary()
{
    // clicked secondary - send messages and update screen
    // also ask for the primary scale's address

    if( ignoreSignals_ )
        return;

    localScaleType_ = SECONDARY_SCALE;
    showHostNameStuff( true );

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::checkboxChanged()
{
    // we clicked a checkbox, so force a save

    if( ignoreSignals_ )
        return;

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::textboxChanged()
{
    // if the host name or IP address has changed, force a save

    if( ignoreSignals_ )
        return;

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clickFlipIt()
{
    if( localHostType_ == HOSTNAME_AS_SERVER )
    {
        // we are currently displaying a host name, so clear it, hide it, and display the ipAddress

        localHostType_ = IP_ADDRESS_AS_SERVER;                      // flip the value
        clearHostName();
        showHostName( false );
        showIpAddress(  true );
        clearIpAddress();
        ui->butFlipIt->setText(tr("Use Hostname"));
    }
    else if(  localHostType_ == IP_ADDRESS_AS_SERVER )
    {
        // we are currently displaying an ip address, so clear it, hide it, and display the host name

        localHostType_ = HOSTNAME_AS_SERVER;                        // flip the value
        clearIpAddress();
        showIpAddress( false );
        showHostName( true );
        clearHostName();
        ui->butFlipIt->setText(tr("Use Address"));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::clickSaveIt()
{
    // force a save everytime to the scale application when the SAVE button is pressed
    // no matter if the data has changed or not.

    hasChanged_ = true;                                             // set to true to allow every press of SAVE

    saveDataOnExit();
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          MESSAGE CATCHER
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperScaleSynchronization::onSessionMessage( const SessionMessage& msg )
{
    if (( ConfigurationDev == msg[0].toInt() ) && ( CfgSynchronizationWrite == msg[1].toInt() ))
    {
        // 112,181 - update sync scale type
        updateScaleSynchronizationType(msg);
    }
    else if (( ConfigurationDev == msg[0].toInt() ) && ( CfgSyncListWrite == msg[1].toInt() ))
    {
        // 112,185 - update list of operators in the combo
        updateScaleSynchronizationData(msg);
    }
    else if ( Error == msg[0].toInt() )
    {
        // handle status messages to the screen using 103 error messages

        int event = msg[1].toInt();
        int msgNum = msg[2].toInt();

        updateStatusLine(msgNum, event);           // pass the messagenumber and event
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperScaleSynchronization::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // because if the style = 0, we just want (maybe) display an error on the screen, a status message
    // never a dialog box. So don't pop dialogs if style = 0, let the individual screen handle them.
    if ( 0 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
