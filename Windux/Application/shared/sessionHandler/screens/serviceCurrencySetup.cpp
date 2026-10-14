/* @file serviceCurrencySetup.cpp
 * CurrencySettings
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd Davis
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "serviceCurrencySetup.h"
#include "ui_serviceCurrencySetup.h"
#include "TokenParser.h"

static const int NOT_CHECKED = 0;
static const int CHECKED = 2;
static const bool TESTING = false;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

serviceCurrencySetup::serviceCurrencySetup(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceCurrencySetup)
{
    allAreDisabled = true;
    weigherSwitchPressed = 0;

    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();

    connect(ui->currencySymbolPlacementComboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));
    connect(ui->primaryPrecisionDigitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));
    connect(ui->roundingMethodComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));
    connect(ui->totalPriceDigitsComboBox, SIGNAL(currentIndexChanged(int) ), this, SLOT(SetupChanged()));
    connect(ui->unitPriceDigitsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));
    connect(ui->unitSeparatorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));
    connect(ui->primaryCurrencySymbolLineEdit, SIGNAL(editingFinished()), this, SLOT(SetupChanged()));
}

void serviceCurrencySetup::SetupChanged()
{
    serviceCurrencySetupChanged = true;
}

serviceCurrencySetup::~serviceCurrencySetup()
{
    delete ui;
}

void serviceCurrencySetup::saveOnExit()
{
    if(serviceCurrencySetupChanged)
    {
        QString messageOut;

        messageOut = "\"" + ui->unitSeparatorComboBox->itemData(ui->unitSeparatorComboBox->currentIndex()).toString() + "\"";
        messageOut += "," + ui->currencySymbolPlacementComboBox->itemData(ui->currencySymbolPlacementComboBox->currentIndex()).toString();
        messageOut += "," + ui->totalPriceDigitsComboBox->itemData(ui->totalPriceDigitsComboBox->currentIndex()).toString();
        messageOut += "," + ui->unitPriceDigitsComboBox->itemData(ui->unitPriceDigitsComboBox->currentIndex()).toString();
        messageOut += ",1";             // roundingFactor - guess it's always 1.
        messageOut += "," + QString::number( ui->roundingMethodComboBox->currentIndex() );
        messageOut += ",\"" + ui->primaryCurrencySymbolLineEdit->text() + "\"";
        messageOut += "," + ui->primaryPrecisionDigitsComboBox->itemData(ui->primaryPrecisionDigitsComboBox->currentIndex()).toString();
        messageOut += ",,";           // secCurrSymbol,secondaryPrecision

        session_->send( ConfigurationDev, CfgCurrencySave, messageOut ); //Send to backend
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceCurrencySetup::initialize()
{
    ui->primaryCurrencySymbolLineEdit->setText("");
    ui->unitSeparatorComboBox->addItem(tr(" . ( Period )"), ".");
    ui->unitSeparatorComboBox->addItem(tr(" , ( Comma )"), ",");
    ui->unitSeparatorComboBox->setCurrentIndex(0);

    ui->currencySymbolPlacementComboBox->addItem(tr("Leading"), 1);
    ui->currencySymbolPlacementComboBox->addItem(tr("Trailing"), 2);
    ui->currencySymbolPlacementComboBox->setCurrentIndex(0);

    // We don't translate numbers
    ui->totalPriceDigitsComboBox->addItem("3" ,3);
    ui->totalPriceDigitsComboBox->addItem("4",4);
    ui->totalPriceDigitsComboBox->addItem("5",5);
    ui->totalPriceDigitsComboBox->addItem("6",6);
    ui->totalPriceDigitsComboBox->addItem("7",7);
    ui->totalPriceDigitsComboBox->addItem("8",8);
    ui->totalPriceDigitsComboBox->setCurrentIndex(3);

    ui->unitPriceDigitsComboBox->addItem("3",3);
    ui->unitPriceDigitsComboBox->addItem("4",4);
    ui->unitPriceDigitsComboBox->addItem("5",5);
    ui->unitPriceDigitsComboBox->addItem("6",6);
    ui->unitPriceDigitsComboBox->addItem("7",7);
    ui->unitPriceDigitsComboBox->setCurrentIndex(1);

    ui->primaryPrecisionDigitsComboBox->addItem("0", 0);
    ui->primaryPrecisionDigitsComboBox->addItem("1",1);
    ui->primaryPrecisionDigitsComboBox->addItem("2",2);
    ui->primaryPrecisionDigitsComboBox->addItem("3",3);
    ui->primaryPrecisionDigitsComboBox->setCurrentIndex(2);

    ui->roundingMethodComboBox->addItem(tr("Conventional"),0);
    ui->roundingMethodComboBox->addItem(tr("Down"),1);
    ui->roundingMethodComboBox->addItem(tr("Up"),2);

    if( session_->uiUtilities->isFeatureLicensed( SCALE_INTERNATIONAL_ROUNDING ) )
    {
        ui->roundingMethodComboBox->setVisible( true );
        ui->roundingMethodLabel->setText( tr("Rounding Method") );
    }
    else
    {
        ui->roundingMethodComboBox->setVisible( false );
        ui->roundingMethodLabel->setText( tr("Rounding: Conventional") );
    }

    serviceCurrencySetupChanged = false;

    session_->send(ConfigurationDev, CfgCurrencyRead);           // 112,100 -> get date ranges

    if( wasWeigherSwitchPressed() ) //reads if weigher switch was pressed
    {
        enableAll();
    }
    else if( TESTING )
    {
        setSwitchToPressed();
    }
    else
    {
        session_->send(Weigher, WgMode, "3"); // the 3 needs to be an enum
        ui->weigherNotice->show();
    }
}

bool serviceCurrencySetup::wasWeigherSwitchPressed()
{

    DomWeigherSwitch *mySwitch = &session_->scaleConfig->weigherSwitch;


    weigherSwitchPressed = mySwitch->switchPressed().toInt();

    if(weigherSwitchPressed == 1)
        return true;
    else
    {
        // send 1,7,3
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceCurrencySetup::translate()
{
/*
    ui->currencySymbolPlacementLabel->setText(tr("Currency Symbol Placement"));
    ui->primaryCurrencySymbolLabel->setText(tr("Primary Currency Symbol"));
    ui->primaryDecisionDigitsLabel->setText(tr("Primary Precision Digits"));
    ui->roundingMethodLabel->setText(tr("Rounding Method"));
    ui->totalPriceDigitsLabel->setText(tr("Total Price Digits"));
    ui->unitPriceDigitsLabel->setText(tr("Unit Price Digits"));
    ui->unitSeparatorLabel->setText(tr("Unit Separator"));
 */
    ui->retranslateUi(this);
    /*
    ui->unitSeparatorComboBox->setItemText(0, tr(" . ( Period )"));
    ui->unitSeparatorComboBox->setItemText(1, tr(" , ( Comma )"));

    ui->currencySymbolPlacementComboBox->setItemText(0, tr("Leading"));
    ui->currencySymbolPlacementComboBox->setItemText(1, tr("Trailing"));

    ui->roundingMethodComboBox->setItemText(0, tr("Conventional"));
    ui->roundingMethodComboBox->setItemText(1, tr("Down"));
    ui->roundingMethodComboBox->setItemText(2, tr("Up"));
    */

    if( session_->uiUtilities->isFeatureLicensed( SCALE_INTERNATIONAL_ROUNDING ) )
        ui->roundingMethodLabel->setText( tr("Rounding Method") );
    else
        ui->roundingMethodLabel->setText( tr("Rounding: Conventional") );

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

void serviceCurrencySetup::enableAll()
{
    ui->currencySymbolPlacementComboBox->setEnabled(true);
    ui->primaryCurrencySymbolLineEdit->setEnabled(true);
    ui->primaryPrecisionDigitsComboBox->setEnabled(true);
    ui->totalPriceDigitsComboBox->setEnabled(true);
    ui->unitPriceDigitsComboBox->setEnabled(true);
    ui->unitSeparatorComboBox->setEnabled(true);
    if( session_->uiUtilities->isFeatureLicensed( SCALE_INTERNATIONAL_ROUNDING ) )
    {
        ui->roundingMethodLabel->setText( tr("Rounding Method") );
        ui->roundingMethodComboBox->setVisible( true );
        ui->roundingMethodComboBox->setEnabled(true);
    }
    else
    {
       ui->roundingMethodLabel->setText( tr("Rounding: Conventional") );
       ui->roundingMethodComboBox->setVisible( false );
    }

    ui->weigherNotice->hide();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void serviceCurrencySetup::setSwitchToPressed()
{    
    // XXX dom's weigher switch pressed needs set to 1 FROM JOE's ENUMS
    weigherSwitchPressed = 1;
    DomWeigherSwitch *mySwitch = &session_->scaleConfig->weigherSwitch;
    mySwitch->switchPressed()  = "1";
    enableAll();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceCurrencySetup::setUICurrencySettings(const SessionMessage& msg)
{
    MadpCurrencySettings m (msg);
    ui->primaryCurrencySymbolLineEdit->setText(m.priCurrSymbol());

    if(m.unitSeparator() == ".")
    {
        ui->unitSeparatorComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->unitSeparatorComboBox->setCurrentIndex(1);
    }

    ui->currencySymbolPlacementComboBox->setCurrentIndex( ui->currencySymbolPlacementComboBox->findData(m.unitPlacement().toInt()));
    ui->totalPriceDigitsComboBox->setCurrentIndex(ui->totalPriceDigitsComboBox->findData(m.totalPriceDigits()));
    ui->unitPriceDigitsComboBox->setCurrentIndex(ui->unitPriceDigitsComboBox->findData(m.unitPriceDigits()));
    ui->primaryPrecisionDigitsComboBox->setCurrentIndex(ui->primaryPrecisionDigitsComboBox->findData(m.primaryPrecision()));
    ui->roundingMethodComboBox->setCurrentIndex( m.roundingMethod().toInt() );

     session_->send(Weigher, WgConfigurationBegin);           // 1,110 -> get date ranges
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceCurrencySetup::onScreenChanging()
{
    saveOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceCurrencySetup::onSessionMessage( const SessionMessage& msg )
{
    if(ConfigurationDev == msg[0].toInt() && CfgCurrencyWrite == msg[1].toInt()) //112,101
    {
        setUICurrencySettings(msg);
    }
    else if (Weigher == msg[0].toInt() && CfgSelfServiceWrite == msg[1].toInt()) // 1,111
    {
        setSwitchToPressed();
    }
}
