/* @file superChangeProductUnitPrice.cpp
 * superChangeProductUnitPrice
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd Davis
 */

#include <logmanager.h>

#include "superChangeProductAllFields.h"
#include "superChangeProductUnitPrice.h"
#include "ui_superChangeProductUnitPrice.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>
#include <ScaleClasses.h>

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

superChangeProductUnitPrice::superChangeProductUnitPrice(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductUnitPrice)
{
    ui->setupUi(this);
    scaleScreen();

    ignoreSignals = true;

    connect(ui->cbUnitPriceForcedPrice,         SIGNAL(stateChanged(int)), this, SLOT(reactToForcedUnitPriceClick(int)));
    connect(ui->cbFractionalPricingForcedPrice, SIGNAL(stateChanged(int)), this, SLOT(reactToForcedFractionalPricingClick(int)));
    connect(ui->cbPoundsForPriceForcedPrice,    SIGNAL(stateChanged(int)), this, SLOT(reactToForcedPoundsForPricingClick(int)));

    connect(ui->butCancel,                      SIGNAL(clicked()),         this, SLOT(createNewProductCancel_clicked()));
    connect(ui->butOK,                          SIGNAL(clicked()),         this, SLOT(createNewProductOK_clicked()));
    connect(ui->leProductNumber,                SIGNAL(editingFinished()), this, SLOT(doPluLookup()));
    
    // Did away with these signals because now these fields need their own WriteSingleField
    // routines. But each of these fields now just call this method when done.
    //connect(ui->leUnitPrice, SIGNAL(editingFinished()), this, SLOT(transferToThePluField()));				// does nothing, could never make this work even with eventFilter REMOVED. WHYYYYYYYYY?
    //connect(ui->leFractionalPrice, SIGNAL(editingFinished()), this, SLOT(transferToThePluField()));
    //connect(ui->lePoundsForPrice, SIGNAL(editingFinished()), this, SLOT(transferToThePluField()));

    // tracking unit prices
    connect(ui->leUnitPrice,        SIGNAL(editingFinished()), this, SLOT(trackSuperChangeUnitPrice()));
    connect(ui->lePoundsForPrice,   SIGNAL(editingFinished()), this, SLOT(trackSuperChangeLbsForPrice()));
    connect(ui->leFractionalPrice,  SIGNAL(editingFinished()), this, SLOT(trackSuperChangeFractionalPrice()));
    // who has focus on unit prices
    connect(ui->leUnitPrice,        SIGNAL( textEdited(QString) ), this, SLOT( onUnitPriceTextChanged(QString) ) );
    connect(ui->lePoundsForPrice,   SIGNAL( textEdited(QString) ), this, SLOT( onLbsForTextChanged(QString) ) );
    connect(ui->leFractionalPrice,  SIGNAL( textEdited(QString) ), this, SLOT( onFracPriceTextChanged(QString) ) );

    scaleConfigData    = &session_->scaleConfig->scaleConfig;
    currencyData       = &session_->scaleConfig->currencySettings;
    currentPLUData     = &session_->scaleConfig->currentPLU;
    internalConfigData = &session_->scaleConfig->internalConfig;

    ui->comboBoxPriceModifier->addItem("1", 1);
    ui->comboBoxPriceModifier->addItem("2", 2);
    ui->comboBoxPriceModifier->addItem("3", 3);
    ui->comboBoxPriceModifier->addItem("4", 4);
    ui->comboBoxPriceModifier->addItem("5", 5);
    ui->comboBoxPriceModifier->addItem("6", 6);
    ui->comboBoxPriceModifier->addItem("7", 7);
    ui->comboBoxPriceModifier->addItem("8", 8);
    ui->comboBoxPriceModifier->addItem("9", 9);

    if( scaleConfigData->isScaleMetric() )
    {
        ui->comboBoxFractionalPriceEnglishUnit->addItem("kg", 0);
        ui->comboBoxFractionalPriceEnglishUnit->addItem("100g", 1);
    }
    else
    {
        ui->comboBoxFractionalPriceEnglishUnit->addItem("1/4", 0);
        ui->comboBoxFractionalPriceEnglishUnit->addItem("1/2", 1);
        ui->comboBoxFractionalPriceEnglishUnit->addItem("1", 2);
    }

    showForcedFields = false;

    // NUMERIC VALIDATORS
    ui->leFractionalPrice->setValidator(fractionalPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leFractionalPrice->setDecimal(currencyData->unitSeparator());
    ui->leFractionalPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->leFractionalPrice->setInputWrapping(true);
    ui->leFractionalPrice->setInputTruncation(true);
    fractionalPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    ui->lePoundsForPrice->setValidator( poundsForPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->lePoundsForPrice->setDecimal(currencyData->unitSeparator());
    ui->lePoundsForPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->lePoundsForPrice->setInputWrapping(true);
    ui->lePoundsForPrice->setInputTruncation(true);
    poundsForPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    ui->leUnitPrice->setValidator( unitPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leUnitPrice->setDecimal(currencyData->unitSeparator());
    ui->leUnitPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->leUnitPrice->setInputWrapping(true);
    ui->leUnitPrice->setInputTruncation(true);
    unitPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));

    //ui->leProductNumber->setDecimal("");
    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));

    //ui->leProductNumber->installEventFilter(this);
    ui->leUnitPrice->installEventFilter( this );
    ui->lePoundsForPrice->installEventFilter(this);
    ui->leFractionalPrice->installEventFilter(this);

    popupActive = false;
    cameFromCancel = false;
    switchScreensOnNewPLU = false;
    requestedPlu = false;
    savingPLUData = false;
	ignoreSignals = false;
    didThisAlready = false;
    alwaysSaveThisPlu = false;
    wroteTransaction = false;
    justUncheckedForcedPrice = false;
    ifUpdating = false;
    prev_unitprice   = "0.00";
    prev_lbsforprice = "0.00";
    prev_fracprice   = "0.00";
    whoHasFocus =  SAV_NONE;

    // FT-902: Have to force lookup of PLU if currentPLUData has a PLU #, as the rest
    // of the currentPLUData was not current when processed by initialize() method.
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber() );

    reloadFields();

    translate();
}

superChangeProductUnitPrice::~superChangeProductUnitPrice()
{
    delete ui;
}

void superChangeProductUnitPrice::saveProduct()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "saveProduct" );

    LOG_DEBUG( "STARTING saveProduct() with plu %s", productNumber.toLatin1().constData() );

    if(switchScreensOnNewPLU)
        return;

    if( requestedPlu )                               // set in doPluLookup bc we sent a 101,5. Dont save anything then
        return;

    ignoreSignals = true;

    QString msg;
    if( productNumber != "" && productNumber != "0")
    {
        if(savingPLUData)
            return;

        savingPLUData = true;		// just dont allow us to do double saves

        // on 2/12/2013 Larry said since it is a business rule to zero out the pounds for amount and fractional 1/4, 1/2,
        // and 1 fields that he will take care of zeroing out the backend values for these if the forced price checkbox is checked.
        if( (shownGroupBox == 1) && (alwaysSaveThisPlu) ) //unit price box
        {
            if( ui->cbUnitPriceForcedPrice->isChecked() )
            {
                // save forced unit price checked -> field 51
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "1,0,0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeUnitPrice();
            }
            else if( justUncheckedForcedPrice )
            {
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "0,1,0";                    // forced
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeUnitPrice();
                justUncheckedForcedPrice = false;
            }
            else if( wroteTransaction )
            {
                session_->send(ProductDev, ProductSave);
            }
        }
        else if(shownGroupBox == 2  && alwaysSaveThisPlu )                      // pounds for price box
        {
            if( ui->cbPoundsForPriceForcedPrice->isChecked() )
            {
                // Just CHECKED the checkbox
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "1,0,0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeLbsForPrice(); 
            }
            else if( justUncheckedForcedPrice )
            {
                // Just UNCHECKED the checkbox
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "0,1,0";                    // forced
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeLbsForPrice();
                justUncheckedForcedPrice = false;
            }
            else if( !ui->cbPoundsForPriceForcedPrice->isChecked() && justUncheckedForcedPrice == false)
            {
                // Just CHANGED THE PRICE but didnt check or uncheck anything
                //save pounds for price pound modifier -> field 27 per Larry

                // Just CHECKED the checkbox
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "0,1,0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                msg = productNumber + "," + QString::number( Product::fieldOffsetPriceModifier ) + "," + ui->comboBoxPriceModifier->itemData(ui->comboBoxPriceModifier->currentIndex()).toString() + "," + currentPLUData->priceModifier() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                msg = productNumber + "," + QString::number( Product::fieldOffsetPoundsForAmount ) + "," + ui->comboBoxPriceModifier->itemData(ui->comboBoxPriceModifier->currentIndex()).toString() + "," + currentPLUData->poundsForAmountRaw() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);
                session_->send(ProductDev, ProductSave);

                trackSuperChangeLbsForPrice();
            }
            else if( wroteTransaction )
            {
                session_->send(ProductDev, ProductSave);
            }
        }
        else if(shownGroupBox == 3  && alwaysSaveThisPlu )                      // fractional pricing box
        {
            if( ui->cbFractionalPricingForcedPrice->isChecked()  )
            {
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "1,0,0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeFractionalPrice();
                justUncheckedForcedPrice = false;                               // just in case
            }
            else if( justUncheckedForcedPrice )
            {
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "0,1,0";                    // forced
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                trackSuperChangeFractionalPrice();
                justUncheckedForcedPrice = false;
            }
            else if( !ui->cbPoundsForPriceForcedPrice->isChecked() && justUncheckedForcedPrice == false )
            {
                msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPriceForced ) + "," + "0,1,0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101, 22

                // save fractional price unit -> field 27
                //25  = 1/4
                //50  = 1/2
                //100 = 1
                //save pounds for price modifier -> field 27

                QString fractionalPriceModifier = "";

                if( scaleConfigData->isScaleMetric() )
                {
                    if(ui->comboBoxFractionalPriceEnglishUnit->currentIndex() == 0)
                    {
                        fractionalPriceModifier = "1";
                    }
                    else if (ui->comboBoxFractionalPriceEnglishUnit->currentIndex() == 1)
                    {
                        fractionalPriceModifier = "100";
                    }
                }
                else
                {
                    if(ui->comboBoxFractionalPriceEnglishUnit->currentIndex() == 0)
                    {
                        fractionalPriceModifier = "25";
                    }
                    else if (ui->comboBoxFractionalPriceEnglishUnit->currentIndex() == 1)
                    {
                        fractionalPriceModifier = "50";
                    }
                    else if (ui->comboBoxFractionalPriceEnglishUnit->currentIndex() == 2)
                    {
                        fractionalPriceModifier = "100";
                    }
                }

                msg = productNumber + "," + QString::number( Product::fieldOffsetPriceModifier ) + "," + fractionalPriceModifier + "," + currentPLUData->priceModifier() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);           // write it 101,16
                session_->send(ProductDev, ProductSave);                            // save it 101, 22

                msg = productNumber + "," + QString::number( Product::fieldOffsetFractionalPriceUnit ) + "," + fractionalPriceModifier + "," + currentPLUData->fractionalPriceUnitRaw() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);
                session_->send(ProductDev, ProductSave);
            }
            else if( wroteTransaction )
            {
                session_->send(ProductDev, ProductSave);
            }
        }
    }
    else
        LOG_DEBUG( "Didnt have a product to save", NULL );

    alwaysSaveThisPlu = false;
    ignoreSignals = false;
    savingPLUData = false;
}

void superChangeProductUnitPrice::reactToForcedPoundsForPricingClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->lePoundsForPrice->setText("");
        trackSuperChangeLbsForPrice();
    }
    else
    {
        ui->lePoundsForPrice->setText("0" + separator + "00");
        trackSuperChangeLbsForPrice();
        justUncheckedForcedPrice = true;
    }
}

void superChangeProductUnitPrice::reactToForcedFractionalPricingClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->leFractionalPrice->setText("");
        trackSuperChangeFractionalPrice();
    }
    else
    {
        ui->leFractionalPrice->setText("0" + separator + "00");
        trackSuperChangeFractionalPrice();
        justUncheckedForcedPrice = true;
    }
}

void superChangeProductUnitPrice::reactToForcedUnitPriceClick(int state)
{
    ignoreSignals = true;

    if ( ifUpdating )
        return;

    if(state)
    {
        ui->leUnitPrice->setText("");
        trackSuperChangeUnitPrice();
    }
    else
    {
        ui->leUnitPrice->setText("0" + separator + "00");
        trackSuperChangeUnitPrice();
        justUncheckedForcedPrice = true;
    }

    ignoreSignals = false;
}

void superChangeProductUnitPrice::doPluLookup()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "doPluLookup" );

    LOG_DEBUG( "Maybe Lookup", NULL );

    if( requestedPlu )                     // set just later in this method. Don't do multiple lookups on same item from multiple emits
        return;

    hasChanged = true;
    //lookup PLU

    LOG_DEBUG( "came %d", cameFromCancel );
    LOG_DEBUG( "popupActive %d", popupActive );

    QString pluField = ui->leProductNumber->text();

    if( (productNumber != pluField) && (pluField != "" ) && ( pluField != "0") && !cameFromCancel && !popupActive)
    {
        LOG_DEBUG( " call saveProduct then Request Lookup", NULL );
        alwaysSaveThisPlu = true;
        saveProduct();
        requestedPlu = true;    // FT-925 Unit price fields not populating
        
        if( wroteTransaction && productNumber.toInt() > 0 )
        {
            session_->send(ProductDev, ProductSaveSupervisorTransaction);
            whoHasFocus = SAV_NONE;
            wroteTransaction = false;
        }

        prev_unitprice   = "0.00";
        prev_lbsforprice = "0.00";
        prev_fracprice   = "0.00";

        session_->send(ProductDev, ProductRead, ui->leProductNumber->text() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::trackSuperChangeUnitPrice()
{
    // if tracking supervisor changes send a single field write to app for Unit Price
    // Send the UnitPrice as the Unit Price. Forced UnitPrice field not send

    if( scaleConfigData->fractionalPricingOn() || scaleConfigData->poundsForPricingOn() )
        return;

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal    = ui->leUnitPrice->text().replace(separator,".");
    QString plu_price     = currentPLUData->unitPrice();

    if( upFieldVal == prev_unitprice || upFieldVal == plu_price )
        return;

    wroteTransaction = true;
    prev_unitprice = upFieldVal;

    //if ( plu_price.length() == 0 )
    //    plu_price = "0.00";
    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"" + ",\"" + plu_price + "\""  + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;

    transferToThePluField();
}

void superChangeProductUnitPrice::trackSuperChangeLbsForPrice()
{
    // if tracking supervisor changes send a single field write to app for  lbsForPrice
    // Send the PoundsForPrice as the Unit Price. he price modifiers and forced field have been left out. Not requested

    if( scaleConfigData->poundsForPricingOn() == false )
        return;

    if ( ifUpdating )
        return;

    QString separator    = currencyData->unitSeparator();
    QString upFieldVal   = ui->lePoundsForPrice->text().replace(separator,".");
    QString plu_price    = currentPLUData->unitPrice();

    if( upFieldVal == prev_lbsforprice || upFieldVal == plu_price )
        return;

    wroteTransaction = true;
    prev_lbsforprice = upFieldVal;

    //if ( plu_price.length() == 0 )
    //    plu_price = "0.00";

    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + "," + "\"" + upFieldVal + "\"," + "\"" + plu_price + "\"," + "0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;

    transferToThePluField();
}

void superChangeProductUnitPrice::trackSuperChangeFractionalPrice()
{
    // if tracking supervisor changes send a single field write to app for Fractional Price
    // Send the FractionalPrice as the Unit Price. The price modifiers and forced field have been left out. Not requested

    if( scaleConfigData->fractionalPricingOn() == false )
        return;

    if ( ifUpdating )
        return;

    QString separator    = currencyData->unitSeparator();
    QString upFieldVal   = ui->leFractionalPrice->text().replace(separator,".");
    QString plu_price    = currentPLUData->unitPrice();

    if( upFieldVal == prev_fracprice || upFieldVal == plu_price )
        return;

    wroteTransaction = true;
    prev_fracprice = upFieldVal;


    //if ( plu_price.length() == 0 )
    //    plu_price = "0.00";

    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + "\"" + plu_price + "\"," + "0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;

    transferToThePluField();
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::onUnitPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_UNITPRICE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::onLbsForTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_LBSFOR;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::onFracPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_FRACPRICE;
}

void superChangeProductUnitPrice::transferToThePluField()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "transferToThePluField" );

    // This SLOT handles clicking ENTER on the onscreen keyboard, NOT a USB keyboard.
    // For the USB keyboard check out the eventFilter() method.

    // There is also a timing issue here. This is happening too soon and not having enough
    // time in between the keyboard dismiss and show and adding focus. Lets wait a sec

    LOG_DEBUG( "Call single shot to add focus to PLU", NULL );
    QTimer::singleShot(500, this, SLOT(setHardFocusToPluField()));
}

void superChangeProductUnitPrice::setHardFocusToPluField()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "setHardFocusToPluField" );

    ui->leProductNumber->setFocus();
    LOG_DEBUG( "Just set focus to the product number", NULL );
}

void superChangeProductUnitPrice::transferToTheUnitPriceField()
{
    // Called when ENTER clicked on the PLU field, moves focus to Unit Price
    ignoreSignals = true;                   // otherwise will call eventFilter on itself

    //ui->cbUnitPriceForcedPrice->setChecked(false);
    ui->leUnitPrice->setFocus();

    ignoreSignals = false;
}

void superChangeProductUnitPrice::createNewProductCancel_clicked()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "createNewProductCancel_clicked" );

    LOG_DEBUG( "Cancel", NULL );
    cameFromCancel = true;

    ui->leProductNumber->setText(productNumber);
    ui->gbNewProduct->hide();
    popupActive = false;
    cameFromCancel = false;
    requestedPlu = false;                   // need to be able to go active again after a lookup
}

void superChangeProductUnitPrice::clearCreatePLURadios()
{
    ifUpdating = true;
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(false);
    ifUpdating = false;
}

void superChangeProductUnitPrice::createNewProductOK_clicked()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "createNewProductOK_clicked" );

    LOG_DEBUG( "OK", NULL );
    QString msg = ui->leProductNumber->text() + ",";

    // FT-1029 PLUs created on the Price Page (and all of the others) becomes the previous entered PLU
    // So we need to make sure once we go from a sub page, to All Fields, we seed the currentPLU with the new PLU
    currentPLUData->itemNumber() = ui->leProductNumber->text();

    ifUpdating = true;
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(false);
    ifUpdating = false;

    if(ui->rbRandomWeight->isChecked())
    {
        msg += "1";
    }
    else if (ui->rbFixedWeight->isChecked())
    {
        msg += "2";
    }
    else if (ui->rbByCount->isChecked())
    {
        msg += "3";
    }
    else if (ui->rbFluidOunces->isChecked())
    {
        msg += "4";
    }
    msg += ",1";

    session_->send(ProductDev, ProductCreate, msg);
    ui->gbNewProduct->hide();

    cameFromCancel = false;
    popupActive = false;
    switchScreensOnNewPLU = true;

    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
 }

void superChangeProductUnitPrice::popupCreateNewPLUDialog()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "popupCreateNewPLUDialog" );

    LOG_DEBUG( "Popup", NULL );

    ifUpdating = true;
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(true);
    ifUpdating = false;

    ui->butOK->setEnabled(true);

    // ask producttype
    ui->lblcreateNewProductQuestion->setText(tr("Product") + " " + ui->leProductNumber->text() + " " + tr("does not exist. Create new product?"));

    scaleConfigData->isScaleMetric()? ui->rbFluidOunces->hide() : ui->rbFluidOunces->show();

    ui->gbNewProduct->move( scalePointForScreen( QPoint( 15, 20 ) ) );
    ui->gbNewProduct->raise();
    ui->gbNewProduct->show();
}

void superChangeProductUnitPrice::reloadFields()
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "reloadFields" );

    ifUpdating = true;

    ui->gbUnitPrice->show();
    scaleConfigData    = &session_->scaleConfig->scaleConfig;
    currencyData       = &session_->scaleConfig->currencySettings;
    currentPLUData     = &session_->scaleConfig->currentPLU;
    internalConfigData = &session_->scaleConfig->internalConfig;

    this->separator = currencyData->unitSeparator();
    ui->lblCurrencySymbolFractionalPrice->setText(currencyData->priCurrSymbol());
    ui->lblCurrencySymbolPoundsForPrice->setText(currencyData->priCurrSymbol());

    bool noPLU = false;

    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
    {
        ui->leProductNumber->fromSessionField(currentPLUData->itemNumber());
        //ui->lblDescriptionContent->setText(currentPLUData->description());
        //superChangeProductAllFields allFields;
        //QString descriptionText = superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description());
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
        productNumber = currentPLUData->itemNumber();
    }
    else
    {
        ui->leProductNumber->setText("");
        ui->leUnitPrice->setText("");
        ui->leFractionalPrice->setText("");
        ui->lePoundsForPrice->setText("");
        ui->lblDescriptionContent->setText("");
        productNumber = "0";
        noPLU = true;
    }


    //ui->lblDescriptionContent->setText(currentPLUData->description());
    //qDebug() << " description is: " << currentPLUData->description();
    ui->gbUnitPrice->show();

    ui->gbFractionalPrice->hide();
    ui->gbNewProduct->hide();
    ui->gbPoundsForPricing->hide();

    showForcedFields = false;

    // RULE: Can only have forced fields when the Prefix is either 2 or 4
    // OR - we have value in the EAN Code
    // AND we cant be a self service scale
    int newIntPrefix = currentPLUData->numberSystem().toInt();
    int pluType = currentPLUData->productType();

    if( ( ScaleModel::ScaleModel_ESS == scaleConfigData->scaleModel().toInt() ) || ( ProductType::PRODUCTTYPE_UNKNOWN == pluType ) )
    {
        showForcedFields = false;                                                    // no self servs
    }
    else
    {
        if( ProductType::PRODUCTTYPE_FIXEDWEIGHT == pluType || ProductType::PRODUCTTYPE_FLUIDOUNCE == pluType )
            showForcedFields = true;
        else if( newIntPrefix > 0 )
            showForcedFields = true;
    }

    if(scaleConfigData->fractionalPricingOn())
    {
        //show fractional groupbox only
        ui->gbFractionalPrice->move( scalePointForScreen( QPoint( 40, 190 ) ) );
        shownGroupBox = 3;

        ui->gbFractionalPrice->show();
        ui->gbUnitPrice->hide();
        if(!noPLU)
        {
            if( currentPLUData->forcedUnitPrice() )
                ui->leFractionalPrice->setText("");
            else
                ui->leFractionalPrice->fromSessionField(currentPLUData->unitPrice());
        }
        else
            ui->leFractionalPrice->setText("");

        // cant see why we are doing this if we are running through the switch statement a few lines down
        //if(!noPLU) ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(ui->comboBoxFractionalPriceEnglishUnit->findData(currentPLUData->priceModifier()));

        ui->cbFractionalPricingForcedPrice->setChecked(currentPLUData->forcedUnitPrice());

        if(showForcedFields)
        {
            ui->cbFractionalPricingForcedPrice->show();
        }
        else
        {
            ui->cbFractionalPricingForcedPrice->hide();
        }

        //if(ui->cbFractionalPricingForcedPrice->isChecked())
        //{
        //    ui->leFractionalPrice->setText("");
        //}

        QString myFP = currentPLUData->fractionalPriceUnitRaw();
        QString myPM = currentPLUData->priceModifier();
        switch (currentPLUData->priceModifier().toInt())
        {
            case (1):
                // 1kg
                if( scaleConfigData->isScaleMetric() )
                    ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(0);     // 1 kg METRIC
                else
                    ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(2);     // 1 pound ENGLISH
                break;
           case (25):
                ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(0);         // 1/4 pound ENGLISH
                break;
           case(50):
                ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(1);         // 1/2 pound ENGLISH
                break;
           case(100):
              ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(1);           // 100g METRIC
              break;
           default:
              ui->comboBoxFractionalPriceEnglishUnit->setCurrentIndex(0);
              break;
        }
    }
    else if(scaleConfigData->poundsForPricingOn()) // Pounds for Pricing
    {
        ui->gbPoundsForPricing->move( scalePointForScreen( QPoint( 40, 190 ) ) );
        shownGroupBox = 2;
        ui->gbPoundsForPricing->show();
        ui->gbUnitPrice->hide();
        if(showForcedFields)
        {
            ui->cbPoundsForPriceForcedPrice->show();
        }
        else
        {
            ui->cbPoundsForPriceForcedPrice->hide();
        }

        if(!noPLU)
        {
            if( currentPLUData->forcedUnitPrice() )
                ui->lePoundsForPrice->setText("");
            else
                ui->lePoundsForPrice->fromSessionField(currentPLUData->unitPrice());
        }
        else
            ui->lePoundsForPrice->setText("");

        ui->cbPoundsForPriceForcedPrice->setChecked(currentPLUData->forcedUnitPrice());

        //if(ui->cbPoundsForPriceForcedPrice->isChecked())
        //{
        //    ui->lePoundsForPrice->setText("");
        //}

        ui->comboBoxPriceModifier->setCurrentIndex(currentPLUData->priceModifier().toInt()-1);

    }
    else // Unit Price
    {
        ui->gbUnitPrice->move( scalePointForScreen( QPoint( 40, 190 ) ) );

        shownGroupBox = 1;
        ui->gbUnitPrice->show();

        LOG_DEBUG( "separator is: %s", currencyData->unitSeparator().toLatin1().constData() );
        LOG_DEBUG( "precision is: %d", currencyData->primaryPrecision() );
        LOG_DEBUG( "unitprice is: %s", currentPLUData->unitPrice().toLatin1().constData() );

        //unitPriceValidator_

        // Is kiosk or not
        if(scaleConfigData->scaleModel().toInt() == 1)
        {
            showForcedFields = false;
        }

        if(showForcedFields)
        {
            ui->cbUnitPriceForcedPrice->show();
        }
        else
        {
            ui->cbUnitPriceForcedPrice->hide();
        }

        QString myfpStr = currentPLUData->forcedUnitPriceRaw();
        bool myfp = currentPLUData->forcedUnitPrice();

        if( myfp )
             ui->cbUnitPriceForcedPrice->setChecked( true );
        else
             ui->cbUnitPriceForcedPrice->setChecked( false );


        //ui->cbUnitPriceForcedPrice->setChecked(currentPLUData->forcedUnitPrice());

        ////////////////////////////////////
        ////////////////////////////////////

        ignoreSignals = true;
        if(!noPLU)
        {
            // At this point, on clicking ENTER on keyboard, we gave focus to the unit price field in the
            // doPlulookup().  But if we do that, because if a field has focus, fromSessionField()
            // will not allow an update. So, instead of moving focus after the lookup which I originally
            // put it place as a SIGNAL\SLOT on ENTER, rip it out and do it here instead

            if( currentPLUData->forcedUnitPrice() )
                ui->leUnitPrice->setText("");
            else
                ui->leUnitPrice->fromSessionField(currentPLUData->unitPrice());

            transferToTheUnitPriceField();                          // was originally a slot
        }
        else
            ui->leUnitPrice->setText("");


        ////////////////////////////////////
        ////////////////////////////////////

        //if(ui->cbUnitPriceForcedPrice->isChecked())
        //{
        //    LOG_DEBUG( "set unit price to blank", NULL );
        //    ui->leUnitPrice->setText("");
        //}

        ignoreSignals = false;

        if(scaleConfigData->isScaleMetric())
        {
            ui->lblUnitPriceUnit->setText("kg");

            if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType())
            {
                ui->lblUnitPriceUnit->setText("/kg");
            }
            else
            {
                ui->lblUnitPriceUnit->setText("kg");
            }
        }
        else // English
        {
            ui->lblUnitPriceUnit->setText("lb");

            if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType())
            {
                ui->lblUnitPriceUnit->setText("/lb");
            }
            else
            {
                ui->lblUnitPriceUnit->setText("lb");
            }
        }
    }

    ifUpdating = false;

    this->update();
}

int superChangeProductUnitPrice::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

bool superChangeProductUnitPrice::eventFilter( QObject *obj, QEvent *event )
{
    //LOG_CLASS_INFO( "superChangeProductUnitPrice", "eventFilter" );

    if( ignoreSignals )
        return false;

    if ( ui->leUnitPrice == obj || ui->leFractionalPrice == obj || ui->lePoundsForPrice == obj)
    {
        if ( event->type() == QEvent::KeyPress )
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if( keyEvent->key() ==  Qt::Key_Return )
            {
                // Since the SIGNAL/SLOT won't call it, have the eventFilter call it
                ui->leProductNumber->setFocus();        // This one is for clicking ENTER on a USB keyboard
            }
        }
    }
    else if(ui->lePoundsForPrice == obj)
    {
        if ( QEvent::FocusIn == event->type() )
        {
            ui->cbPoundsForPriceForcedPrice->setChecked(false);
        }
    }
    else if(ui->leFractionalPrice == obj)
    {
        if ( QEvent::FocusIn == event->type() )
        {
            ui->cbFractionalPricingForcedPrice->setChecked(false);
        }
    }
    //else if( ui->leProductNumber == obj )
    //{
    //    LOG_DEBUG( "hit the obj = PLU field", NULL );
    //    if( QEvent::FocusIn == event->type() )
    //    {
    //        LOG_DEBUG( "hit the keyboard event code for productNumber", NULL );
    //
    //        mainWindow_->showKeyboard();
    //    }
    //}
    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductUnitPrice::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

void superChangeProductUnitPrice::hasUIChanged()
{
    hasChanged = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::translate()
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
bool superChangeProductUnitPrice::onScreenChanging()
{
    if( whoHasFocus == SAV_UNITPRICE )
        trackSuperChangeUnitPrice();
    else if( whoHasFocus == SAV_LBSFOR )
        trackSuperChangeLbsForPrice();
    else if( whoHasFocus == SAV_FRACPRICE )
        trackSuperChangeFractionalPrice();

    alwaysSaveThisPlu = true;
    this->saveProduct();

    if( wroteTransaction )
    {
        session_->send(ProductDev, ProductSaveSupervisorTransaction);
        whoHasFocus = SAV_NONE;
        wroteTransaction = false;
    }

    // when not working - this  true. false in All Fields

    if( productNumber != "" && productNumber != "0" )
    {

        if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
        {
            requestedPlu = true;                                        // set bc we are sending a 101,5
            session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
        }
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductUnitPrice::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superChangeProductUnitPrice", "onSessionMessage" );

    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        popupActive = true;
        popupCreateNewPLUDialog();
    }
    else if(ProductDev == msg[0].toInt() && ProductWrite == msg[1].toInt())
    {
        productNumber = msg[2];
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(msg[4]));
        reloadFields();
        this->update();
        requestedPlu = false;                                       // reset bc we got our 101,7 back
    }
    // plu not on file
    else if (( ProductDev == msg[0].toInt() ) && ( ProductClearUI == msg[1].toInt() ))
    {
        reloadFields();
    }
    else if( ( ProductDev == msg[0].toInt() ) && ( ProductWriteSingleField == msg[1].toInt() ) )
    {
        savingPLUData = false;
        LOG_DEBUG( "onSessionMessage.savingPLUData just now set to false", NULL );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductUnitPrice::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
