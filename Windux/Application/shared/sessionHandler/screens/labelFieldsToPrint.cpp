/**
 * @file labelFieldsToPrint.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */


#include "labelFieldsToPrint.h"
#include "ui_labelFieldsToPrint.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

labelFieldsToPrint::labelFieldsToPrint(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::labelFieldsToPrint)
{
    ui->setupUi(this);
    scaleScreen();

    bSetupChanged = false;
    initialized = false;
    dataHasBeenInitialized = false;                                 // set to true when message comes in and is processed
    priceChangedLabelIndicator = "";

    //--------------------------------------------------------------
    // Set the connections for when a checkbox is checked/un-checked
    // or a dropdown is changed.  These all call the same method to
    // set a property that indicates something in the setup changed
    // and the data needs to be saved.
    //--------------------------------------------------------------
    connect(ui->ckSellByDate, SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->cbSellByDate, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));

    connect(ui->ckPackedOnDate, SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->cbPackedOnDate, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));

    connect(ui->ckBestBeforeDate, SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->cbBestBeforeDate, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));

    connect(ui->ckUnitPrice,         SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckByCount,           SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckUpcEanBarcode,     SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckCurrencySymbol,    SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckPrePrintTextNames, SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckWeightOnByCount,   SIGNAL(clicked()), this, SLOT(changeWeightOnAByCount()));
    connect(ui->ckUpcCheckDigit,     SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckNetWtOnFixedWt,    SIGNAL(clicked()), this, SLOT(SetupChanged()));
    connect(ui->ckOneOnByCount,      SIGNAL(clicked()), this, SLOT(changeOneOnAByCount()));
    connect(ui->cbFixedWeightFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(SetupChanged()));

    //-----------------------------------------
    // Request the Label-> Fields to Print data
    //-----------------------------------------
    session_->send( ConfigurationDev, CfgPrintedFieldsRead );

    //------------------------------------------------------
    // Request the data for any additional transaction types
    //------------------------------------------------------
    session_->send( TransactionTypeDev, TransactionReadNonVoidGroup );

    ui->cbFixedWeightFormat->addItem(QString(), 0);
    ui->cbFixedWeightFormat->addItem(QString(), 1);

    ui->cbUnitPriceFormat->addItem(QString(), 1);
    ui->cbUnitPriceFormat->addItem(QString(), 2);
    ui->cbUnitPriceFormat->addItem(QString(), 3);
    ui->cbUnitPriceFormat->addItem(QString(), 4);

    translate();

    initialized = true;
}

labelFieldsToPrint::~labelFieldsToPrint()
{
    delete ui;
}

void labelFieldsToPrint::translate()
{
    ui->retranslateUi(this);

    // only do this once initialization is done the first time
    //if( initialized )
    {
        ui->cbFixedWeightFormat->setItemText( 0, tr("Pounds and ounces (2 LB 4 OZ)") );
        ui->cbFixedWeightFormat->setItemText( 1, tr("Decimal pounds (2.25 LB)") );

        ui->cbUnitPriceFormat->setItemText( 0, tr("Print 'For'") );
        ui->cbUnitPriceFormat->setItemText( 1, tr("Use actual Unit Price if possible") );
        ui->cbUnitPriceFormat->setItemText( 2, tr("Use the format '3 / 1.00'") );
        ui->cbUnitPriceFormat->setItemText( 3, tr("Use the format '3 For 1.00'") );
    }

    update();
}

bool labelFieldsToPrint::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void labelFieldsToPrint::onSessionMessage( const SessionMessage& msg )
{
    // Fields to Print data
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgPrintedFieldsWrite == msg[1].toInt() ))
    {
        DomPrintSpecialFields m( msg );

        // Update UI with the Fields to Print settings
        ui->ckSellByDate->setChecked((m.sellByDate().toInt() == 1 ? true : false));
        ui->cbSellByDate->setCurrentIndex(m.sellByDateFormat().toInt());

        ui->ckPackedOnDate->setChecked((m.packedOnDate().toInt() == 1 ? true : false));
        ui->cbPackedOnDate->setCurrentIndex(m.packedOnDateFormat().toInt());

        ui->ckBestBeforeDate->setChecked((m.productLifeDate().toInt() == 1 ? true : false));
        ui->cbBestBeforeDate->setCurrentIndex(m.productLifeDateFormat().toInt());

        ui->ckUnitPrice->setChecked((m.unitPrice().toInt() == 1 ? true : false));
        ui->ckByCount->setChecked((m.byCount().toInt() == 1 ? true : false));
        ui->ckUpcEanBarcode->setChecked((m.barcode().toInt() == 1 ? true : false));
        ui->ckCurrencySymbol->setChecked((m.currency().toInt() == 1 ? true : false));

        ui->ckPrePrintTextNames->setChecked((m.textNames().toInt() == 1 ? true : false));

        // FT-924 split out to see what was going on.
        int mywobc = m.weightOnByCount().toInt();
        if( mywobc == 1)
            ui->ckWeightOnByCount->setChecked(true);
        else
            ui->ckWeightOnByCount->setChecked(false);

        ui->ckUpcCheckDigit->setChecked((m.upcCheckDigit().toInt() == 1 ? true : false));
        ui->ckNetWtOnFixedWt->setChecked((m.netWeightOnFixedWt().toInt() == 1 ? true : false));

        ui->ckOneOnByCount->setChecked((m.oneOnByCount().toInt() == 1 ? true : false));

        priceChangedLabelIndicator = m.priceChangedLabelIndicator();
        ui->txtPriceChangedLabelIndicator->setText( priceChangedLabelIndicator );

        ui->cbFixedWeightFormat->setCurrentIndex( m.fixedWeightFormat().toInt() );

        int index( ui->cbUnitPriceFormat->findData( m.unitPriceFormat().toInt() ) );

        if ( 0 <= index )
            ui->cbUnitPriceFormat->setCurrentIndex( index );
        else
            ui->cbUnitPriceFormat->setCurrentIndex( 0 );

        bSetupChanged = false;
        dataHasBeenInitialized = true;
    }

    //--------------------------------------------------------------
    // Populate dropdown for "First letter of transaction type name"
    //--------------------------------------------------------------

    // Check if any additional transaction types have been set up
    if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeWriteAllNames == msg[1].toInt() ))
    {
        DomTotalTypesList m( msg );

        numRecords = m.numOfRecordIds();

        // Get the list of ID's
        allRecordIDsList = TokenParser::splitStringTokens(m.allRecordIds());

        // Get the list of type names
        namesList = TokenParser::splitStringTokens(m.namesList());

        // Get the secondary lists of names and number of
        numOfSecRecords = m.secNumOfNames();
        secNamesList = TokenParser::splitStringTokens(m.secNamesList());

        bSetupChanged = false;
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved and don't allow One on a by
// count be able to be checked when Weight on a by count
// gets checked
//------------------------------------------------------
void labelFieldsToPrint::changeWeightOnAByCount()
{
    if( ui->ckOneOnByCount->isChecked() )
        ui->ckOneOnByCount->setChecked(false);

    bSetupChanged = true;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved and don't allow Weight on a by
// count be able to be checked when One on a by count
// gets checked
//------------------------------------------------------
void labelFieldsToPrint::changeOneOnAByCount()
{
    if( ui->ckWeightOnByCount->isChecked() )
        ui->ckWeightOnByCount->setChecked(false);

    bSetupChanged = true;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void labelFieldsToPrint::SetupChanged()
{
    bSetupChanged = true;
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void labelFieldsToPrint::SaveDataOnExit()
{
    if(!dataHasBeenInitialized)
        return;                                                     // FT-1091: if we havent processed initial message, no save occurs

    // FT-924  Fields to print will not save best before date and weight on a bycount
    // I found the reason best before date would not save, SIGNAL was a copy paste that needed fixed.
    // Did that, but could not find a reason why weight on a by count did not save. Its like sometimes
    // the SIGNAL doesn't fire, or the SLOT.  Everything is correct and it only happenns on LINUX, so
    // I'm just going to take away the save condition and always do it.
    //if (bSetupChanged)
    //{
    int currentIndex;
    QString param;

    param = QString(ui->ckSellByDate->isChecked() ? "1" : "0");
    currentIndex = ui->cbSellByDate->currentIndex();
    param += "," + QString::number(currentIndex,10);

    param += "," + QString(ui->ckPackedOnDate->isChecked() ? "1" : "0");
    currentIndex = ui->cbPackedOnDate->currentIndex();
    param += "," + QString::number(currentIndex,10);

    param += "," + QString(ui->ckBestBeforeDate->isChecked() ? "1" : "0");
    currentIndex = ui->cbBestBeforeDate->currentIndex();
    param += "," + QString::number(currentIndex,10);

    param += "," + QString(ui->ckUnitPrice->isChecked() ? "1" : "0");
    param += "," + QString(ui->ckByCount->isChecked() ? "1" : "0");
    param += "," + QString(ui->ckUpcEanBarcode->isChecked() ? "1" : "0");
    param += "," + QString(ui->ckCurrencySymbol->isChecked() ? "1" : "0");
    param += "," + QString(ui->ckPrePrintTextNames->isChecked() ? "1" : "0");

    // FT-924 - split out to see what was going on
    //param += "," + QString(ui->ckWeightOnByCount->isChecked() ? "1" : "0");
    if( ui->ckWeightOnByCount->isChecked() )
        param += ",1";
    else
        param += ",0";

    param += "," + QString(ui->ckOneOnByCount->isChecked() ? "1" : "0");
    param += "," + QString(ui->ckUpcCheckDigit->isChecked() ? "1" : "0");

    param += "," + QString(ui->ckNetWtOnFixedWt->isChecked() ? "1" : "0");

    currentIndex = ui->cbFixedWeightFormat->currentIndex();
    param += "," + QString::number(currentIndex,10);

    priceChangedLabelIndicator = ui->txtPriceChangedLabelIndicator->text();
    param += ",\"" + priceChangedLabelIndicator + "\"";

    currentIndex = ui->cbUnitPriceFormat->currentData().toInt();
    param += "," + QString::number(currentIndex,10);

    session_->send( ConfigurationDev, CfgPrintedFieldsSave, param);

    // now ask for it back so we have it in operate mode
    session_->send( ConfigurationDev, CfgPrintedFieldsRead );
    //}
}


