/* @file superChangeProductPriceFields.cpp
 * superChangeProductPriceFields 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd
 */

#include <logmanager.h>

#include "superChangeProductAllFields.h"
#include "superChangeProductPriceFields.h"
#include "ui_superChangeProductPriceFields.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>
#include <ScaleClasses.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

// Set UPGRADE to 1 when you want to
// require User to enter a PLU to use
// this form.
static const int UPGRADE = 0;

static const int SECTION_2_Y = 124;
static const int SECTION_3_Y = 277;
static const int SECTION_4_Y = 551;
static const int SECTION_5_Y = 551;
static const int POINTS_X    = 716;
static const int POINTS_W    = 201;
static const int POINTS_H    = 41;

static const int PRICING_TAB_GROUP_BOX_SPACING = 3;
static const int PRICING_TAB_POINTS_X          = 654;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart{
class numericLineEdit;
}

superChangeProductPriceFields::superChangeProductPriceFields(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductPriceFields),
    hasChanged( false )
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    // hide group boxes
    hidePriceFields();

    nextSectionAvailable = 2;
    nextSectionAvailableX = scaleWidthForScreen( 67 );
    nextSectionAvailableY = scaleHeightForScreen( 131 );

    scaleConfigData      = &session_->scaleConfig->scaleConfig;
    normalScaleOperation = &session_->scaleConfig->normalScaleOperation;
    currencyData         = &session_->scaleConfig->currencySettings;
    currentPLUData       = &session_->scaleConfig->currentPLU;
    internalConfigData   = &session_->scaleConfig->internalConfig;
    frequentShopperData  = &session_->scaleConfig->frequentShopper;

    prev_unitprice   = "0.00";
    prev_lbsforprice = "0.00";
    prev_fracprice   = "0.00";
    prev_tareA       = "0.00";
    prev_tareB       = "0.00";
    prev_proptare    = "0.0";
    prev_bycount     = "1";
    prev_netwt       = "0";
    prev_fluidnetwt  = "0";

    whoHasFocus =  SAV_NONE;

    wroteTransaction = false;
    
    this->separator = currencyData->unitSeparator();

    //ui->leProductNumber->setValidator(new QIntValidator(1,999999, this));
    connect(ui->leProductNumber,                     SIGNAL(editingFinished()),        this, SLOT(doPluLookup()));
    connect(ui->chkbox2ForcedByCount,                SIGNAL(stateChanged(int)),        this, SLOT(reactToForcedByCountClick(int)));
    connect(ui->chkbox2ForcedTare,                   SIGNAL(stateChanged(int)),        this, SLOT(reactToForcedTareClick(int)));
    connect(ui->chkbox2UnitPriceForcedPrice,         SIGNAL(stateChanged(int)),        this, SLOT(reactToForcedPriceClick(int)));
    connect(ui->chkbox2FractionalPricingForcedPrice, SIGNAL(stateChanged(int)),        this, SLOT(reactToFractionalPricingForcedPriceClick(int)));
    connect(ui->chkbox2PoundsForPricingForcedPrice,  SIGNAL(stateChanged(int)),        this, SLOT(reactToPoundsForPriceForcedPriceClick(int)));
    connect(ui->cb2DiscountType,                     SIGNAL(currentIndexChanged(int)), this, SLOT(reactToDiscountTypeChanged(int)));

     // Pricing Field connections for Supervisor Changes Transactions with Single Field Writes.
    // unit prices
    connect(ui->le2UnitPrice,        SIGNAL(editingFinished()), this, SLOT(trackSuperChangeUnitPrice()));
    connect(ui->le2PoundsForPrice,   SIGNAL(editingFinished()), this, SLOT(trackSuperChangeLbsForPrice()));
    connect(ui->le2FractionalPrice,  SIGNAL(editingFinished()), this, SLOT(trackSuperChangeFractionalPrice()));

    // bycount
    connect(ui->le2ByCount,          SIGNAL(editingFinished()), this, SLOT(trackSuperChangeByCount()));

    // weights of the net or fixed Variety
    connect(ui->le2NetWeight,        SIGNAL(editingFinished()), this, SLOT(trackSuperChangeNetWeight()));
    connect(ui->le2NetWeightFluid,   SIGNAL(editingFinished()), this, SLOT(trackSuperChangeFluidNetWeight()));

    connect(ui->le2UnitPrice,        SIGNAL( textEdited(QString) ), this, SLOT( onUnitPriceTextChanged(QString) ) );
    connect(ui->le2PoundsForPrice,   SIGNAL( textEdited(QString) ), this, SLOT( onLbsForTextChanged(QString) ) );
    connect(ui->le2FractionalPrice,  SIGNAL( textEdited(QString) ), this, SLOT( onFracPriceTextChanged(QString) ) );

    // tares
    connect(ui->le2Tare,             SIGNAL( editingFinished() ), this, SLOT(trackSuperChangeTare()));
    connect(ui->le2ProportionalTare, SIGNAL( editingFinished() ), this, SLOT(trackSuperChangePropTare()));
    connect(ui->le2Tare,             SIGNAL( textEdited(QString) ), this, SLOT( onTareTextChanged(QString) ) );
    connect(ui->le2ProportionalTare, SIGNAL( textEdited(QString) ), this, SLOT( onPropTareTextChanged(QString) ) );

    // bycount
    connect(ui->le2ByCount,          SIGNAL( editingFinished() ), this, SLOT(trackSuperChangeByCount()));
    connect(ui->le2ByCount,          SIGNAL( textEdited(QString) ), this, SLOT( onByCountTextChanged(QString) ) );

    // weights of the net or fixed Variety
    connect(ui->le2NetWeight,        SIGNAL( editingFinished() ), this, SLOT(trackSuperChangeNetWeight()));
    connect(ui->le2NetWeightFluid,   SIGNAL( editingFinished() ), this, SLOT(trackSuperChangeFluidNetWeight()));
    connect(ui->le2NetWeight,        SIGNAL( textEdited(QString) ), this, SLOT( onNetWtTextChanged(QString) ) );
    connect(ui->le2NetWeightFluid,   SIGNAL( textEdited(QString) ), this, SLOT( onFluidNetWtTextChanged(QString) ) );

    connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(createNewProductCancel_clicked()));
    connect(ui->butOK, SIGNAL(clicked()), this, SLOT(createNewProductOK_clicked()));

    showForcedFields = false;
    pluEmitted = 0;
    requestedPlu = false;
    ifUpdating = false;

    // VALIDATORS
    ///////////////////////////////////
    // Product Number
    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));

    // By Count
    ui->le2ByCount->setValidator(byCountValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2ByCount->setDecimalPlaces(0);
    ui->le2ByCount->setInputWrapping(true);
    ui->le2ByCount->setInputTruncation(true);
    byCountValidator_->setTop(makeValidatorTop(2));
    byCountValidator_->setBottom(1);

    // Discount Values - Give a default currency top and decimal places, set the top and decimal places later depending on whether they are percent or currency if a current PLU has been set
    // FT-81 Discount Price fields on Price Fields screen function incorrectly. This is tied to bug
    // FT-143 Discount field only allows 4 digits and the fields only show percent signs or nothing at all.
    // Where in the description its stated the operator are supposed to be able to enter 5 digits.  I'm not sure if 5
    // hardcoded digits is really the path we should of went down in fixing this, but I hardcoded 5.
    //1
    ui->le2Discount1->setValidator(discountValue1Validator_ = new QIntValidator(0, INT_MAX, this));
    discountValue1Validator_->setTop(makeValidatorTop(5));
    ui->le2Discount1->setInputWrapping(true);
    ui->le2Discount1->setInputTruncation(true);
    ui->le2Discount1->setDecimalPlaces(2);
    //2
    ui->le2Discount2->setValidator(discountValue2Validator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2Discount2->setDecimalPlaces(2);
    ui->le2Discount2->setInputWrapping(true);
    ui->le2Discount2->setInputTruncation(true);
    discountValue2Validator_->setTop(makeValidatorTop(5));
    //3
    ui->le2Discount3->setValidator(discountValue3Validator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2Discount3->setDecimalPlaces(2);
    ui->le2Discount3->setInputWrapping(true);
    ui->le2Discount3->setInputTruncation(true);
    discountValue3Validator_->setTop(makeValidatorTop(5));

    // Unit Price
    ui->le2UnitPrice->setValidator(unitPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2UnitPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->le2UnitPrice->setDecimal(currencyData->unitSeparator());
    ui->le2UnitPrice->setInputWrapping(true);
    ui->le2UnitPrice->setInputTruncation(true);
    unitPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    // Tare
    // SOF-2750: Re-worked this section to use Max Tare from normal scale operation.
    //           Borrowed from "All Fields".
    int tarePrecision( scaleConfigData->tarePrecision() );
    QString maximumTare( normalScaleOperation->maxTare() );

    if( tarePrecision < 2 )
        tarePrecision = 2;
    else if ( 3 < tarePrecision )
        tarePrecision = 3;

    ui->le2Tare->setValidator( tareValidator_ = new QIntValidator( 0, 99999, this ) );
    ui->le2Tare->setDecimalPlaces( tarePrecision );
    ui->le2Tare->setInputPrecision( tarePrecision );
    ui->le2Tare->setInputWrapping( true );
    ui->le2Tare->setInputTruncation( true );

    if ( maximumTare.length() )
    {
        // Here we are using the Maximum Tare from Normal Scale operations
        maximumTare.replace( ".", "" );

        tareValidator_->setTop( maximumTare.toInt() );
    }
    else
    {
        // here we are default Maximum Tare
        int top( scaleConfigData->isScaleMetric() ? 15000 : 30000 );

        if ( 3 == tarePrecision )
            tareValidator_->setTop( top );
        else if ( 2 == tarePrecision )
            tareValidator_->setTop( top / 10 );
    }

    // ProportionalTare
    ui->le2ProportionalTare->setValidator(proportionalTareValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2ProportionalTare->setDecimalPlaces(1);
     ui->le2ProportionalTare->setDecimal(currencyData->unitSeparator());
     ui->le2ProportionalTare->setInputWrapping(true);
     ui->le2ProportionalTare->setInputTruncation(true);
     proportionalTareValidator_->setTop(makeValidatorTop(3));

     // Net Weight
     ui->le2NetWeight->setValidator(netWeightValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2NetWeight->setDecimalPlaces(0);
     ui->le2NetWeight->setInputWrapping(true);
     ui->le2NetWeight->setInputTruncation(true);
     netWeightValidator_->setTop(makeValidatorTop(4));

     //Net Weight Fluid
     ui->le2NetWeightFluid->setValidator(netWeightFluidValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2NetWeightFluid->setDecimalPlaces(0);
     ui->le2NetWeightFluid->setInputWrapping(true);
     ui->le2NetWeightFluid->setInputTruncation(true);
     netWeightFluidValidator_->setTop(makeValidatorTop(4));

     // Exception Price
     ui->le2ExceptionPrice->setValidator(exceptionPriceValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2ExceptionPrice->setDecimalPlaces(currencyData->primaryPrecision());
     ui->le2ExceptionPrice->setDecimal(currencyData->unitSeparator());
     ui->le2ExceptionPrice->setInputWrapping(true);
     ui->le2ExceptionPrice->setInputTruncation(true);
     exceptionPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

     // Fractional Price
     ui->le2FractionalPrice->setValidator(fractionalPriceValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2FractionalPrice->setDecimalPlaces(currencyData->primaryPrecision());
     ui->le2FractionalPrice->setDecimal(currencyData->unitSeparator());
     ui->le2FractionalPrice->setInputWrapping(true);
     ui->le2FractionalPrice->setInputTruncation(true);
     fractionalPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

     // Pounds for Pricing Price
     ui->le2PoundsForPrice->setValidator(poundsForPriceValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2PoundsForPrice->setDecimalPlaces(currencyData->primaryPrecision());
     ui->le2PoundsForPrice->setDecimal(currencyData->unitSeparator());
     ui->le2PoundsForPrice->setInputWrapping(true);
     ui->le2PoundsForPrice->setInputTruncation(true);
     poundsForPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

     // Points
     ui->le2Points->setValidator(pointsValidator_ = new QIntValidator(0, INT_MAX, this));
     ui->le2Points->setDecimalPlaces(0);
     ui->le2Points->setInputWrapping(true);
     ui->le2Points->setInputTruncation(true);
     pointsValidator_->setTop(makeValidatorTop(4)); //.asp Note says: 0-65535

     ui->le2Tare->installEventFilter(this);
     ui->le2PoundsForPrice->installEventFilter(this);
     ui->le2FractionalPrice->installEventFilter(this);
     ui->le2UnitPrice->installEventFilter(this);
     ui->le2ByCount->installEventFilter(this);

     popupActive = false;
     cameFromCancel = false;
     switchScreensOnNewPLU = false;

     //
    // END VALIDATORS ///////////////////////////////////
    session_->send(ConfigurationDev, CfgScaleOperationRead); // 112,70 to get max tare message returned

    // FT-902: Have to force lookup of PLU if currentPLUData has a PLU #, as the rest
    // of the currentPLUData was not current when processed by initialize() method.
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber() );

    // make sure translate works
    translate();
    this->update();
}

int superChangeProductPriceFields::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

bool superChangeProductPriceFields::eventFilter( QObject *obj, QEvent *event )
{
    if(ui->le2Tare == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            ui->chkbox2ForcedTare->setChecked(false);
        }
    }
    else if(ui->le2UnitPrice == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            ui->chkbox2UnitPriceForcedPrice->setChecked(false);
        }
    }
    else if(ui->le2ByCount == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            ui->chkbox2ForcedByCount->setChecked(false);
        }
    }
    else if(ui->le2FractionalPrice == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            ui->chkbox2FractionalPricingForcedPrice->setChecked(false);
        }
    }
    else if(ui->le2PoundsForPrice == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            ui->chkbox2PoundsForPricingForcedPrice->setChecked(false);
        }
    }

    return _Mybase::eventFilter( obj, event );
}

void superChangeProductPriceFields::zeroOutTareFromForcedTareClick()
{
    QString zeroOutPrecisionText = "";
    if(tareEntryPrecisionFromScaleConfig == "4")
    {
        zeroOutPrecisionText = "0000";
    }
    else if(tareEntryPrecisionFromScaleConfig == "3")
    {
        zeroOutPrecisionText = "000";
    }
    else if(tareEntryPrecisionFromScaleConfig == "2")
    {
        zeroOutPrecisionText = "00";
    }
    else if(tareEntryPrecisionFromScaleConfig == "1")
    {
        zeroOutPrecisionText = "0";
    }
    ui->le2Tare->setText("0" + currencyData->unitSeparator() + zeroOutPrecisionText);
}

void superChangeProductPriceFields::zeroOutAPrice(QString field)
{
    QString zeroOutPrecisionText = "";
    if(currencyData->primaryPrecision() == 4)
    {
        zeroOutPrecisionText = "0000";
    }
    else if(currencyData->primaryPrecision() == 3)
    {
        zeroOutPrecisionText = "000";
    }
    else if(currencyData->primaryPrecision() == 2)
    {
        zeroOutPrecisionText = "00";
    }
    else if(currencyData->primaryPrecision() == 1)
    {
        zeroOutPrecisionText = "0";
    }

    if(field == "unit")
    {
        ui->le2UnitPrice->setText("0" + currencyData->unitSeparator() + zeroOutPrecisionText);
    }
    else if(field == "fractional")
    {
        ui->le2FractionalPrice->setText("0" + currencyData->unitSeparator() + zeroOutPrecisionText);
        ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(0);
    }
    else if (field == "poundsFor")
    {
        ui->le2PoundsForPrice->setText("0" + currencyData->unitSeparator() + zeroOutPrecisionText);
        ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(0);
    }
}

void superChangeProductPriceFields::reactToForcedTareClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->le2Tare->setText("");
        trackSuperChangeTare();
    }
    else
    {
        zeroOutTareFromForcedTareClick();
        trackSuperChangeTare();
    }
}

void superChangeProductPriceFields::reactToPoundsForPriceForcedPriceClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->le2PoundsForPrice->setText("");
        trackSuperChangeLbsForPrice();
    }
    else
    {
        zeroOutAPrice("poundsFor");
        trackSuperChangeLbsForPrice();
    }
}

void superChangeProductPriceFields::reactToDiscountTypeChanged(int index)
{
    switch (index)
    {
        case 0:
        case 1:
        case 2:
        case 6:
            ui->gb2RegularPrice->hide();
            ui->gb2Discount->move( nextSectionAvailableX, locationOfDiscountOrRegularPriceGroupBoxY );
            ui->gb2Discount->show();
            ui->lbl2Discount1PercentSymbol->hide();
            ui->lbl2Discount2PercentSymbol->hide();
            ui->lbl2Discount3PercentSymbol->hide();
            ui->lbl2Discount1CurrencySymbol->show();
            ui->lbl2Discount2CurrencySymbol->show();
            ui->lbl2Discount3CurrencySymbol->show();
            break;
        case 3:
        case 5:
            ui->gb2RegularPrice->hide();
            ui->gb2Discount->move( nextSectionAvailableX, locationOfDiscountOrRegularPriceGroupBoxY );
            ui->gb2Discount->show();
            ui->lbl2Discount1CurrencySymbol->hide();
            ui->lbl2Discount2CurrencySymbol->hide();
            ui->lbl2Discount3CurrencySymbol->hide();
            ui->lbl2Discount1PercentSymbol->show();
            ui->lbl2Discount2PercentSymbol->show();
            ui->lbl2Discount3PercentSymbol->show();
            break;
        case 4:
            // show regular price group box only
            ui->gb2Discount->hide();
            ui->gb2RegularPrice->move( nextSectionAvailableX, locationOfDiscountOrRegularPriceGroupBoxY );
            ui->gb2RegularPrice->show();
        default:
            break;
    }
}

void superChangeProductPriceFields::reactToForcedByCountClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        currentPLUData->byCountRaw() = ui->le2ByCount->text();
        ui->le2ByCount->setText("");
        ui->le2ByCount->setEnabled(false);
        trackSuperChangeByCount();
    }
    else
    {
        ui->le2ByCount->setText(currentPLUData->byCountRaw());
        ui->le2ByCount->setEnabled(true);
        trackSuperChangeByCount();
    }
}

void superChangeProductPriceFields::reactToForcedPriceClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->le2UnitPrice->setText("");
        trackSuperChangeUnitPrice();
    }
    else
    {
        zeroOutAPrice("unit");
        trackSuperChangeUnitPrice();
    }
}

void superChangeProductPriceFields::reactToFractionalPricingForcedPriceClick(int state)
{
    if ( ifUpdating )
        return;

    if(state)
    {
        ui->le2FractionalPrice->setText("");
        trackSuperChangeFractionalPrice();
    }
    else
    {
        zeroOutAPrice("fractional");
        trackSuperChangeFractionalPrice();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::trackSuperChangeUnitPrice()
{
    // if tracking supervisor changes send a single field write to app for Unit Price
    // Send the UnitPrice as the Unit Price. Forced UnitPrice field not send

    if( scaleConfigData->fractionalPricingOn() || scaleConfigData->poundsForPricingOn() )
        return;

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal = ui->le2UnitPrice->text().replace(separator,".");

    if( upFieldVal == currentPLUData->unitPrice() || upFieldVal == prev_unitprice )
        return;

    wroteTransaction = true;
    prev_unitprice = upFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    //QString msg = productNumber + "," + "12" + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

void superChangeProductPriceFields::trackSuperChangeLbsForPrice()
{
    // if tracking supervisor changes send a single field write to app for  lbsForPrice
    // Send the PoundsForPrice as the Unit Price. he price modifiers and forced field have been left out. Not requested

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal = ui->le2PoundsForPrice->text().replace(separator,".");

    if( scaleConfigData->poundsForPricingOn() == false )
        return;

    if( upFieldVal == currentPLUData->unitPrice() || upFieldVal == prev_lbsforprice )
        return;

    wroteTransaction = true;
    prev_lbsforprice = upFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    //QString msg = productNumber + "," + "12" + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";

    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

void superChangeProductPriceFields::trackSuperChangeFractionalPrice()
{
    // if tracking supervisor changes send a single field write to app for Fractional Price
    // Send the FractionalPrice as the Unit Price. The price modifiers and forced field have been left out. Not requested

    if ( ifUpdating )
        return;

    if( scaleConfigData->fractionalPricingOn() == false )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal = ui->le2FractionalPrice->text().replace(separator,".");

    if( upFieldVal == currentPLUData->unitPrice() || upFieldVal == prev_fracprice )
        return;

    wroteTransaction = true;
    prev_fracprice = upFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

void superChangeProductPriceFields::trackSuperChangeTare()
{
    // if tracking supervisor changes send a single field write to app for Tare or 2nd Tare
    // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.

    if ( ifUpdating )
        return;

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();

    if( !scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->le2Tare->text().replace(separator,".");  //  tare() = 21

        if( tareFieldVal == currentPLUData->tare() || prev_tareA == tareFieldVal )
            return;

        wroteTransaction = true;
        prev_tareA = tareFieldVal;

        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->tare() = tareFieldVal;
    }
    else if( scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->le2Tare->text().replace(separator,".");  //  2ndtare() = 104

        if( tareFieldVal == currentPLUData->tare() || prev_tareB == tareFieldVal )
            return;

        wroteTransaction = true;
        prev_tareB = tareFieldVal;

        //QString msg = productNumber + "," + "103" + ",\"" + tareFieldVal + "\"," + currentPLUData->tareB() + ",0";
        // THIS IS NOW 104, thus the enum implementation.
        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tareB() + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->tare() = tareFieldVal;
    }
}

void superChangeProductPriceFields::trackSuperChangePropTare()
{
    // if tracking supervisor changes send a single field write to app for Proportional Tare

    if ( ifUpdating )
        return;

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();
    QString tareFieldVal = ui->le2ProportionalTare->text().replace(separator,".");

    if( tareFieldVal == currentPLUData->proportionalTare() )
        return;

    wroteTransaction = true;
    prev_proptare = tareFieldVal;

    QString msg = productNumber + "," + QString::number( Product::fieldOffsetProportionalTare ) + ",\"" + tareFieldVal + "\"," + currentPLUData->proportionalTare() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->tare() = tareFieldVal;
}

void superChangeProductPriceFields::trackSuperChangeByCount()
{
    // if tracking supervisor changes send a single field write to app for  By Count

    if ( ifUpdating )
        return;

    if( currentPLUData->productType() > ProductType::PRODUCTTYPE_RANDOMWEIGHT )
    {
        QString productNumber = currentPLUData->itemNumber();
        QString bcFieldVal = ui->le2ByCount->text();                        //  byCount() = 11
        QString bcPluVal   = QString::number( currentPLUData->byCount() );

        if( bcFieldVal.toInt() == currentPLUData->byCount() || prev_bycount == bcFieldVal )
            return;

        if( bcFieldVal == "" )
            bcFieldVal = "-1";

        wroteTransaction = true;
        prev_bycount = bcFieldVal;

        QString msg = productNumber + "," + QString::number( Product::fieldOffsetByCount ) + ",\"" + bcFieldVal + "\"," + "\"" + bcPluVal + "\"" + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->byCountRaw() = bcPluVal;
    }
}

void superChangeProductPriceFields::trackSuperChangeNetWeight()
{
    // if tracking supervisor changes send a single field write to app for Net Weight

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString fwFieldVal = ui->le2NetWeight->text().replace(separator,".");                        //  byCount() = 23
    QString fwPluVal   = currentPLUData->fixedWeight();

    if( fwFieldVal == currentPLUData->fixedWeight() )
        return;

    wroteTransaction = true;
    prev_netwt = fwFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->fixedWeight() = fwFieldVal;
}

void superChangeProductPriceFields::trackSuperChangeFluidNetWeight()
{
    // if tracking supervisor changes send a single field write to app for Fluid Net Weight

    if ( ifUpdating )
        return;

    QString separator  = currencyData->unitSeparator();
    QString fwFieldVal = ui->le2NetWeightFluid->text().replace(separator,".");                        //  byCount() = 23
    QString fwPluVal   = currentPLUData->fixedWeight();

    if( fwFieldVal == currentPLUData->unitPrice() )
        return;

    wroteTransaction = true;
    prev_fluidnetwt = fwFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->fixedWeight() = fwFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onUnitPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_UNITPRICE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onLbsForTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_LBSFOR;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onFracPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_FRACPRICE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_TARE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onPropTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_PROPTARE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onByCountTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_BYCOUNT;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onNetWtTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_NETWT;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onFluidNetWtTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_FLNETWT;
}

superChangeProductPriceFields::~superChangeProductPriceFields()
{
    saveProduct();
    delete ui;
}

void superChangeProductPriceFields::saveProduct()
{
    if(switchScreensOnNewPLU)
        return;

    if( requestedPlu )                               // set in doPluLookup bc we sent a 101,5. Dont save anything then
        return;

    if(productNumber != "" && productNumber != "0")
    {
        /**********************************************************************************************/
        // At this point lets save any and all price fields that we need to save as single field writes
        // Compare the currentPluData against the ui field to decide to send it.  And if it's visible
        /*********************************************************************************************/

        ///// UNIT PRICE /////
        if( gbUnitPriceShown )
        {
            // Send the UnitPrice as the Unit Price. Forced UnitPrice field not send
            QString upFieldVal = ui->le2UnitPrice->text().replace(separator,".");
            if( upFieldVal != currentPLUData->unitPrice() )                     // unitPrice() = 13
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }

        }
        else if( gbPoundsForPriceShown )
        {
            // Send the PoundsForPrice as the Unit Price. he price modifiers and forced field have been left out. Not requested
            QString upFieldVal = ui->le2PoundsForPrice->text().replace(separator,".");
            if( upFieldVal != currentPLUData->unitPrice() )                     // unitPrice() = 13
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }
        else if( gbFractionalPriceShown )
        {
            // Send the FractionalPrice as the Unit Price. The price modifiers and forced field have been left out. Not requested
            QString upFieldVal = ui->le2FractionalPrice->text().replace(separator,".");
            if( upFieldVal != currentPLUData->unitPrice() )                     // unitPrice() = 13
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }

        ///// Tare /////
        if(gbTareShown)
        {
            // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.
            // For this, we dont need to send the original value.  Also, where is the tareB field?  Why is it not on this screen?
            // Only sending tare, not Forced Tare.
            if( !scaleConfigData->useSecondTare() )
            {
                QString tareFieldVal = ui->le2Tare->text().replace(separator,".");  //  tare() = 21

                if( tareFieldVal != currentPLUData->tare() )
                {
                    QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";
                    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                    session_->send(ProductDev, ProductSave);                        // save it 101,22
                }
            }
            else if( scaleConfigData->useSecondTare() )
            {
                QString tareFieldVal = ui->le2Tare->text().replace(separator,".");  //  tare() = 104

                if( tareFieldVal != currentPLUData->tare() )
                {
                    QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";
                    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                    session_->send(ProductDev, ProductSave);                        // save  it 101,22
                }
            }
        }
        else if(gbProportionalTareShown)
        {
            QString tareFieldVal = ui->le2ProportionalTare->text().replace(separator,".");

            if( tareFieldVal != currentPLUData->proportionalTare() )            //  proptare() = 22
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetProportionalTare ) + ",\"" + tareFieldVal + "\"," + currentPLUData->proportionalTare() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }
        else if(gbTarePlatterTypeShown)
        {
            QString tareFieldVal = ui->cb2TarePlatterTareType->itemData(ui->cb2TarePlatterTareType->currentIndex()).toString();

            if( tareFieldVal != currentPLUData->tareType() )                    //  plattertare() = 53
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareType ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tareType() + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }

        ///// ByCount /////
        if( gbByCountShown )
        {
            QString bcFieldVal = ui->le2ByCount->text();                        //  byCount() = 11
            QString bcPluVal   = QString::number( currentPLUData->byCount() );
            if( bcFieldVal != bcPluVal )
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetByCount ) + ",\"" + bcFieldVal + "\"," + bcPluVal + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }

        ///// Weight of the Net or Fixed Variety /////

        if(gbNetWeightOuncesShown)
        {
            QString fwFieldVal = ui->le2NetWeight->text().replace(separator,".");                        //  byCount() = 23
            QString fwPluVal   = currentPLUData->fixedWeight();
            if( fwFieldVal != fwPluVal )
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }
        else if(gbNetWeightFluidShown)
        {
            QString fwFieldVal = ui->le2NetWeightFluid->text().replace(separator,".");                        //  byCount() = 23
            QString fwPluVal   = currentPLUData->fixedWeight();
            if( fwFieldVal != fwPluVal )
            {
                QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
                session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16
                session_->send(ProductDev, ProductSave);                        // save it 101,22
            }
        }
        //////////////////// END SENDING SINGLE FIELD WRITES ////////////////////

        //////////////////// START SENDING CURRENTPLU SAVE MESSAGE ////////////////////

        separator = currencyData->unitSeparator();
	    QString msg;
        msg =  productNumber + "," +                    /*  itemNumber()        (2);} */    /* any integer number */
        currentPLUData->productTypeRaw() + "," +        /*  productType()       (3);} */    /* RW=1,FW=2,BC=3,FL=4 */
        "\"" + currentPLUData->description() + "\"," +  /*  description()       (4);} */
        currentPLUData->barcodeTypeRaw() + ","  +       /*  barcodeType()       (5);} */
        currentPLUData->barcodeFieldsRaw() + ","  +     /*  barcodeFields()     (6);} */
        currentPLUData->numberSystem() + ","  +         /*  numberSystem()      (7);} */
        currentPLUData->productNumber() + ","  +        /*  productNumber()     (8);} Umm...this is barcode product number*/
        currentPLUData->vendorNumber() + ","  +         /*  vendorNumber()      (9);} */
        currentPLUData->eanCode() + ",";                /*  eanCode()           (10);} */
	    if(gbByCountShown)
	    {
	        msg.append(ui->le2ByCount->text() + ","); /*  byCount()  (11);} */
	    }
	    else
	    {
	        msg.append(QString::number(currentPLUData->byCount()) + ",");
	    }
	
	    if(gbExceptionPriceShown)
	    {
	
	       msg.append("\"" + ui->le2ExceptionPrice->text().replace(separator,".") + "\",");/*  exceptionPrice()  (12);} */
	   }
	   else
	   {
	       msg.append("\"" + currentPLUData->exceptionPrice() + "\",");
	   }
	
       // One of these 3 go into the UnitPrice field in the currentPlu
        if(gbUnitPriceShown)
	   {
	       msg.append("\"" + ui->le2UnitPrice->text().replace(separator,".") + "\","); /*  unitPrice()  (13);} */
	   }
	   else if(gbPoundsForPriceShown)
	   {
	       msg.append("\"" + ui->le2PoundsForPrice->text().replace(separator,".") + "\",");
	   }
       else if( gbFractionalPriceShown)
	   {
           msg.append("\"" + ui->le2FractionalPrice->text().replace(separator,".") + "\",");
	   }
	
	   msg.append(ui->cb2DiscountType->itemData(ui->cb2DiscountType->currentIndex()).toString() + ","); /*  discountType()  (14);} */
	
	   if(gbRegularPriceShown)
	   {
	       msg.append("\"" + ui->le2RegularPrice->text().replace(separator, ".") + "\",");
	   }
	   else if(gbDiscountShown)
	   {
	       msg.append("\"" + ui->le2Discount1->text().replace(separator, ".") + "\",");
	   }
	   else
	   {
	       msg.append("\"" + currentPLUData->discountValue1() + "\",");
	   }
	
	   msg.append("\"" + ui->le2Discount2->text().replace(separator,".") + "\","  + /*  discountValue2()  (16);} */
	           "\"" + ui->le2Discount3->text().replace(separator,".") + "\"," + currentPLUData->discountLabel1() +
	          "," + currentPLUData->discountLabel2() + ","  + /*  discountLabel2()  (19);} */
	          currentPLUData->discountLabel3() +",");/*  discountLabel3()  (20);} */
	
       if(gbTareShown)
	   {
           // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.
           if( !scaleConfigData->useSecondTare() )
                msg.append("\"" + ui->le2Tare->text().replace(separator,".") + "\",");/*  tare()  (21);} */
           else
                msg.append("\"" + currentPLUData->tare() + "\",");/*  tare()  (21);} */
	   }
	   else
	   {
	       msg.append("\"" + currentPLUData->tare() + "\",");
	   }
	
	   if(gbProportionalTareShown)
	   {
	       msg.append("\"" + ui->le2ProportionalTare->text().replace(separator,".") + "\","); /*  proportionalTare()  (22);} */
	   }
	   else
	   {
	       msg.append("\"" + currentPLUData->proportionalTare() + "\",");
	   }
	
	   // which net weight value to use
	           /*  fixedWeight()  (23);} */
	   if(gbNetWeightOuncesShown)
	   {
	       msg.append(ui->le2NetWeight->text().replace(separator,".") + ",");
	   }
	   else if(gbNetWeightFluidShown)
	   {
	       msg.append(ui->le2NetWeightFluid->text().replace(separator, ".") + ",");
	   }
	   else
	   {
	       msg.append(currentPLUData->fixedWeight() + ",");
	   }
	
	   msg.append(QString::number(currentPLUData->shelfLifeDays()) + ","  +
	           QString::number(currentPLUData->shelfLifeHours()) + ","  +
	           QString::number(currentPLUData->productLifeDays()) + ","  +
	           QString::number(currentPLUData->productLifeHours()) + ",");
	
       if( gbPoundsForPriceShown ) 
           msg.append((ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex())).toString() + ","); // values 1-9
       else
           msg.append(currentPLUData->priceModifier() + ",");

	   msg.append(currentPLUData->reportingClass() + ","  +
               ","  +                                     /* no more label types */
               QString::number(currentPLUData->primaryLabelRotation()) + ","  +
               ","  +
               QString::number(currentPLUData->secondLabelRotation()) + ","  +
               ","  +
               QString::number(currentPLUData->thirdLabelRotation()) + ","  +
	           currentPLUData->backDateDaysRaw() + ","  +
	           ui->le2Points->text().replace(separator,".") + ","  +
	           currentPLUData->graphic1Name() + ","  +
	           currentPLUData->graphic2Name() + ","  +
	           currentPLUData->graphic3Name() + ","  +
	           currentPLUData->graphic4Name() + ","  + /*12*/
	           "\"" + currentPLUData->primaryLabelTypeWidth().replace(separator,".") + "\","  +
	           "\"" + currentPLUData->primaryLabelTypeLength().replace(separator,".") + "\","  +
	           "\"" + currentPLUData->secondLabelTypeWidth().replace(separator,".") + "\","  +
	           "\"" + currentPLUData->secondLabelTypeLength().replace(separator,".") + "\","  +
	           "\"" + currentPLUData->thirdLabelTypeWidth().replace(separator,".") + "\","  +
	           "\"" + currentPLUData->thirdLabelTypeLength().replace(separator,".") + "\",");

	   // Forced By Count
	   if(gbByCountShown && showForcedFields)
	       msg.append((ui->chkbox2ForcedByCount->isChecked()?"1":"0"));
	   else
	       msg.append((currentPLUData->forcedByCount()?"1":"0"));

	   msg.append(",");
	
	   // Forced Tare
	   if(gbTareShown && showForcedFields)
           msg.append((ui->chkbox2ForcedTare->isChecked()?"1":"0") );
	   else
           msg.append((currentPLUData->forcedTare()?"1":"0") );
       msg.append(",");

       // fractionalPriceUnit() (50);
       // valid values: 1, 25, 50, 100, where 1 = 1 lb, 25 = 1/4 lb, 50 = 1/2 lb, , 100 = 100g
       if(gbFractionalPriceShown)
       {
           QString myFP = (ui->cb2FractionalPriceEnglishUnit->itemData(ui->cb2FractionalPriceEnglishUnit->currentIndex())).toString();
           msg.append( myFP + ",");
       }
       else
       {
           msg.append( currentPLUData->fractionalPrice()  + ",");
       }

       // poundsForAmount()
       // valid values: 1-9
       if(gbPoundsForPriceShown)
           msg.append((ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex())).toString() + ",");
       else
           msg.append(QString::number(currentPLUData->poundsForAmount()) + ",");


	   if(gbUnitPriceShown)
	   {
	       msg.append((ui->chkbox2UnitPriceForcedPrice->isChecked()?"1":"0"));
	       msg.append(",");
	   }
	   else if(gbPoundsForPriceShown)
	   {
	       msg.append((ui->chkbox2PoundsForPricingForcedPrice->isChecked()?"1":"0"));
	       msg.append(",");
	   }
       else if( gbFractionalPriceShown )
       {
           msg.append((ui->chkbox2FractionalPricingForcedPrice->isChecked()?"1":"0"));
           msg.append(",");
       }
       else
	   {
	       msg.append((currentPLUData->forcedUnitPrice()?"1":"0"));
	       msg.append(",");
	   }
	
	   if(gbTarePlatterTypeShown)
	   {
	       msg.append(ui->cb2TarePlatterTareType->itemData(ui->cb2TarePlatterTareType->currentIndex()).toString() + ",");
	   }
	   else
	   {
	       msg.append(currentPLUData->tareType() + ",");
	   }
       msg.append(currentPLUData->applySecurity()?"1":"0").append(",");
       msg.append(currentPLUData->dontPrintPackedOn()?"1":"0").append(",");
       msg.append(currentPLUData->dontPrintSellBy()?"1":"0").append(",");
       msg.append(currentPLUData->dontPrintBestBefore()?"1":"0").append(",");
       msg.append( QString::number(currentPLUData->primaryLabelPlacement() ) ).append(",");
       msg.append( QString::number(currentPLUData->secondLabelPlacement() ) ).append(",");
       msg.append( QString::number(currentPLUData->thirdLabelPlacement() ) ).append(",");

       if( gbFractionalPriceShown )
       {
           // Pretty sure these are not even used
           //QString myFP = (ui->cb2FractionalPriceEnglishUnit->itemData(ui->cb2FractionalPriceEnglishUnit->currentIndex())).toString();
           //msg.append( myFP + ",");
           msg.append("\"" + ui->le2FractionalPrice->text().replace(separator,".") + "\",");
           msg.append((ui->chkbox2FractionalPricingForcedPrice->isChecked()?"1":"0")).append(",");
       }
       else
       {
           msg.append(currentPLUData->fractionalPrice()).append(",");
           msg.append(currentPLUData->forcedFractionalPriceRaw()).append(",");
       }
	
	   if(gbPoundsForPriceShown)
	   {
           // Pretty sure these are not even used.
           //msg.append((ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex())).toString() + ",");
           msg.append("\"" + ui->le2PoundsForPrice->text().replace(separator,".") + "\",");
           msg.append((ui->chkbox2PoundsForPricingForcedPrice->isChecked()?"1":"0"));
	   }
	   else
	   {
           msg.append(currentPLUData->poundsForPrice() + ",");
           msg.append(currentPLUData->forcedPoundsForRaw());
	   }
	   msg.append(",");

       // HT-136 Need to have the other fields in place since Tare B was added onto the end
       msg.append(",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
       if( gbTareShown )
       {
           // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.
           if( scaleConfigData->useSecondTare() )
                msg.append("\"" + ui->le2Tare->text().replace(separator,".") + "\",");/*  tareB()  (103);} */
           else
                msg.append("\"" + currentPLUData->tare() + "\",");/*  tareB()  (104);} */
       }
       else
       {
           msg.append("\"" + currentPLUData->tare() + "\",");
       }

       // ADDED - There are a lot of fields missing here after tareB was not shown
       msg.append(",,,,,,,,,,,,,,,,,,,,,,,,");

       //  Keep EANprefix at the end of this list ALWAYS!
       /*  EANprefix()  (129);} */
       msg.append(",");


	   session_->send(ProductDev, ProductWrite, msg);
	   session_->send(ProductDev, ProductSave);
	}
}

void superChangeProductPriceFields::hidePriceFields()
{
    // hide group boxes
    ui->gb2ByCount->hide();

    ui->gb2Discount->hide();
    ui->gb2ExceptionPrice->hide();
    ui->gb2RegularPrice->hide();
    ui->gb2DiscountType->hide();

    ui->gb2NetWeightFluidOunces->hide();
    ui->gb2NetWeightOunces->hide();
    ui->gb2Points->hide();

    ui->gb2UnitPrice->hide();
    ui->gb2PoundsForPricing->hide();
    ui->gb2FractionalPrice->hide();

    ui->gb2Tare->hide();
    ui->gb2ProportionalTare->hide();
    ui->gb2PlatterTare->hide();
}

void superChangeProductPriceFields::reloadFields()
{
    currentPLUData     = &session_->scaleConfig->currentPLU;
    ui->gb2ByCount->hide();
    ui->gb2Discount->hide();
    ui->gb2ExceptionPrice->hide();
    ui->gb2UnitPrice->hide();
    ui->gb2FractionalPrice->hide();
    ui->gb2PoundsForPricing->hide();

    ui->gb2NetWeightFluidOunces->hide();
    ui->gb2NetWeightOunces->hide();
    ui->gbNewProduct->hide();
    ui->gb2Points->hide();

    ui->gb2Tare->hide();
    ui->gb2ProportionalTare->hide();
    ui->lblDescription->hide();

    ifUpdating = true;

    gbUnitPriceShown = false;
    gbFractionalPriceShown = false;
    gbPoundsForPriceShown = false;
    gbTareShown = false;
    gbTarePlatterTypeShown = false;
    gbByCountShown = false;
    gbNetWeightOuncesShown = false;
    gbNetWeightFluidShown = false;
    gbExceptionPriceShown = false;
    gbRegularPriceShown = false;
    gbProportionalTareShown = false;
    gbTarePlatterTypeShown = false;

    // future use
    nextSectionAvailable = 2;

    // nextSectionAvailableX;
    nextSectionAvailableY = scaleHeightForScreen( 124 );

    scaleConfigData      = &session_->scaleConfig->scaleConfig;
    normalScaleOperation = &session_->scaleConfig->normalScaleOperation;
    currencyData         = &session_->scaleConfig->currencySettings;
    currentPLUData       = &session_->scaleConfig->currentPLU;
    internalConfigData   = &session_->scaleConfig->internalConfig;
    frequentShopperData  = &session_->scaleConfig->frequentShopper;

    widgetUsedForPriceModifier  = "";
    widgetUsedForDiscountValue1 = "";
    widgetUsedForFixedWeight    = "";

    bool noPLU = false;
    bool validPlu = false;

    ui->lblDescription->show();
    ui->lblDescriptionContent->show();

    showForcedFields = false;

    // RULE: Can only have forced fields when the Prefix is either 2 or 4
    // OR - we have value in the EAN Code
    // AND we cant be a self service scale
    int newIntPrefix = currentPLUData->numberSystem().toInt();
    int pluType = currentPLUData->productType();

    if( ( scaleConfigData->scaleModel().toInt() == ScaleModel::ScaleModel_ESS ) || ( ProductType::PRODUCTTYPE_UNKNOWN == pluType ) )
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

    ///////////////
    //SECTION 1
    ///////////////
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
    {
	    ui->leProductNumber->fromSessionField(currentPLUData->itemNumber());
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
        validPlu = true;
    }
    else
    {
        ui->leProductNumber->setText("");
        ui->lblDescriptionContent->setText("");
        noPLU = true;
    }

    ///////////////
    //SECTION 2
    ///////////////

  //  nextSectionAvailableX = 30;
    nextSectionAvailableY = scaleHeightForScreen( 131 );

    hidePriceFields();

    // By count
    if(currentPLUData->productType() == ProductType::PRODUCTTYPE_FIXEDWEIGHT || currentPLUData->productType() == ProductType::PRODUCTTYPE_BYCOUNT || currentPLUData->productType() == ProductType::PRODUCTTYPE_FLUIDOUNCE )
    {
        if ( byCountValidator_ )
            delete byCountValidator_;

        ui->gb2ByCount->move( nextSectionAvailableX, nextSectionAvailableY );
        ui->gb2ByCount->show();
        gbByCountShown = true;
        nextSectionAvailableY += ui->gb2ByCount->height() + PRICING_TAB_GROUP_BOX_SPACING;

        ui->le2ByCount->setValidator(byCountValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2ByCount->setDecimalPlaces(0);
        ui->le2ByCount->setInputWrapping(true);
        ui->le2ByCount->setInputTruncation(true);
        byCountValidator_->setTop(makeValidatorTop(2));
        byCountValidator_->setBottom(1);

        if(showForcedFields)
        {
            ui->chkbox2ForcedByCount->show();
        }
        else
        {
            ui->chkbox2ForcedByCount->hide();
        }
    }
    if(currentPLUData->forcedByCount())
    {
        ui->chkbox2ForcedByCount->setChecked(true);
        ui->le2ByCount->setText("");
    }
    else
    {
        ui->chkbox2ForcedByCount->setChecked(false);
        if (!noPLU)
            ui->le2ByCount->fromSessionField(currentPLUData->byCountRaw());
    }

    ui->cb2FractionalPriceEnglishUnit->clear();

    if( scaleConfigData->isScaleMetric() )
    {
        ui->cb2FractionalPriceEnglishUnit->addItem("kg","1");
        ui->cb2FractionalPriceEnglishUnit->addItem("100g","100");
    }
    else
    {
        ui->cb2FractionalPriceEnglishUnit->addItem("1/4","25");
        ui->cb2FractionalPriceEnglishUnit->addItem("1/2","50");
        ui->cb2FractionalPriceEnglishUnit->addItem("1","1");
    }

    if (!noPLU)
        ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(ui->cb2FractionalPriceEnglishUnit->findData(currentPLUData->fractionalPriceUnit()));

    ui->cb2PriceModifier->clear();
    ui->cb2PriceModifier->addItem("1","1");
    ui->cb2PriceModifier->addItem("2","2");
    ui->cb2PriceModifier->addItem("3","3");
    ui->cb2PriceModifier->addItem("4","4");
    ui->cb2PriceModifier->addItem("5","5");
    ui->cb2PriceModifier->addItem("6","6");
    ui->cb2PriceModifier->addItem("7","7");
    ui->cb2PriceModifier->addItem("8","8");
    ui->cb2PriceModifier->addItem("9","9");

    if (!noPLU)
        ui->cb2PriceModifier->setCurrentIndex(ui->cb2PriceModifier->findData(currentPLUData->priceModifier()));

    if(currentPLUData->forcedUnitPrice())
    {
        ui->chkbox2UnitPriceForcedPrice->setChecked(true);
        ui->le2UnitPrice->setText("");
    }
    else
    {
        ui->chkbox2UnitPriceForcedPrice->setChecked(false);
        if ( validPlu )
            ui->le2UnitPrice->fromSessionField(currentPLUData->unitPrice());  // what about fractional and poundsFor?
    }

    // had to remove them each time thru. Yea clear doesn't always work
    while ( ui->cb2DiscountType->count() ) // as a safety precaution, and clear doesn't always work
        ui->cb2DiscountType->removeItem( 0 );

    ui->cb2DiscountType->clear();
    ui->cb2DiscountType->addItem(tr("None"),"0");
    ui->cb2DiscountType->addItem(tr("Cents off"),"1");
    ui->cb2DiscountType->addItem(tr("Discount price"),"2");
    ui->cb2DiscountType->addItem(tr("Percent off"),"3");
    ui->cb2DiscountType->addItem(tr("Advertised price"),"4");
    ui->cb2DiscountType->addItem(tr("Percent added"),"5");
    ui->cb2DiscountType->addItem(tr("Package discount"),"6");

    if (!noPLU)
        ui->cb2DiscountType->setCurrentIndex(ui->cb2DiscountType->findData(currentPLUData->discountType()));

    if (!noPLU)
        ui->le2Points->fromSessionField(currentPLUData->points());

    ui->cb2TarePlatterTareType->clear();
    ui->cb2TarePlatterTareType->addItem(tr("Keyboard"), "0");
    ui->cb2TarePlatterTareType->addItem(tr("Platter"), "1");

    if(currentPLUData->itemNumber().toInt() > 0)
    {
        ui->cb2TarePlatterTareType->setCurrentIndex(ui->cb2TarePlatterTareType->findData(currentPLUData->tareType()));
    }

    if(showForcedFields)
    {
        ui->chkbox2TarePlatterForcedTare->show();
        if(currentPLUData->itemNumber().toInt() > 0 && currentPLUData->forcedTare())
        {
           ui->chkbox2TarePlatterForcedTare->setChecked(true);
           ui->le2TarePlatter->setText("");
        }
        else
        {
            ui->chkbox2TarePlatterForcedTare->setChecked(false);
            if (!noPLU)
                ui->le2TarePlatter->fromSessionField(currentPLUData->tare());
        }
    }
    if ( !noPLU )
    {
        // HT-136: Set the text field to the correct tare value
        if( scaleConfigData->useSecondTare() )
            ui->le2Tare->setText( currentPLUData->tareB() );
        else
            ui->le2Tare->setText( currentPLUData->tare() );
    }

    if(currentPLUData->forcedTare())
    {
        ui->chkbox2ForcedTare->setChecked(true);
        ui->le2Tare->setText("");
    }
    else
    {
        ui->chkbox2ForcedTare->setChecked(false);
        if( !noPLU )
        {
            // HT-136: Set the text field to the correct tare value
            if( scaleConfigData->useSecondTare() )
                ui->le2Tare->fromSessionField( currentPLUData->tareB() );
            else
                ui->le2Tare->fromSessionField( currentPLUData->tare() );
        }
    }

    if (!noPLU)
        ui->le2ProportionalTare->fromSessionField(currentPLUData->proportionalTare());
    if (!noPLU)
        ui->le2ExceptionPrice->fromSessionField(currentPLUData->exceptionPrice());


    // Fractional Pricing - not supported - 10-03-2016-Not true, we have to support it now.  I see it all thru the scale

    if( scaleConfigData->fractionalPricingOn() )
    {
        ui->gb2FractionalPrice->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2FractionalPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2FractionalPrice->show();
        gbFractionalPriceShown = true;
        if ( validPlu )
            ui->lbl2CurrencySymbolFractionalPrice->setText(currencyData->priCurrSymbol());

        ui->lbl2MetricUnitFractionalPrice->hide();
        ui->cb2FractionalPriceEnglishUnit->show();
        if( validPlu )
        {
            ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(ui->cb2FractionalPriceEnglishUnit->findData(currentPLUData->fractionalPriceUnit()));
        }
        else
        {
            ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(0);
        }

        if(currentPLUData->forcedFractionalPrice())
        {
            ui->chkbox2FractionalPricingForcedPrice->setChecked(true);
            ui->le2FractionalPrice->setText("");
        }
        else
        {
            ui->chkbox2FractionalPricingForcedPrice->setChecked(false);
            if ( validPlu )
            {
                ui->le2FractionalPrice->fromSessionField(currentPLUData->unitPrice() );
            }
        }

        if(showForcedFields)
        {
            ui->chkbox2FractionalPricingForcedPrice->show();
        }
        else
        {
            ui->chkbox2FractionalPricingForcedPrice->hide();
        }
    }
    else if(scaleConfigData->poundsForPricingOn())
    {
        // Pounds for Pricing
        ui->gb2PoundsForPricing->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2PoundsForPricing->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2PoundsForPricing->show();
        gbPoundsForPriceShown = true;
        ui->lbl2CurrencySymbolPoundsForPricing->setText(currencyData->priCurrSymbol());
        scaleConfigData->isScaleMetric() ? ui->lbl2PoundsForPricingUnit->setText("kgs") : ui->lbl2PoundsForPricingUnit->setText("lbs"); //Universal Symbols cannot be translated
        ui->lbl2CurrencySymbolPoundsForPricing->setText(currencyData->priCurrSymbol());

        if( currentPLUData->forcedUnitPrice() )
        {
            ui->chkbox2PoundsForPricingForcedPrice->setChecked(true);
            ui->le2PoundsForPrice->setText("");
        }
        else
        {
            ui->chkbox2PoundsForPricingForcedPrice->setChecked(false);

            if ( validPlu )
                ui->le2PoundsForPrice->fromSessionField( currentPLUData->unitPrice() );
        }

        showForcedFields ? ui->chkbox2PoundsForPricingForcedPrice->show() : ui->chkbox2PoundsForPricingForcedPrice->hide();

        if( validPlu )
        {
            ui->cb2PriceModifier->setCurrentIndex(ui->cb2PriceModifier->findData(currentPLUData->priceModifier()));
        }
        else
        {
            ui->cb2PriceModifier->setCurrentIndex(0);
        }

        widgetUsedForPriceModifier = "cb2PriceModifier";
    }
    // Unit Price
    else
    {
        ui->gb2UnitPrice->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2UnitPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2UnitPrice->show();
        gbUnitPriceShown = true;
        ui->lbl2UnitPriceCurrencySymbol->setText(currencyData->priCurrSymbol());
        if(scaleConfigData->isScaleMetric())
        {
            if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setText("/kg");
            }
            else
            {
                ui->lbl2UnitPriceUnitSymbol->setText("kg");
            }
        }
        else
        {
            if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setText("/lb");
            }
            else
            {
                ui->lbl2UnitPriceUnitSymbol->setText("lb");
            }
        }

        showForcedFields ? ui->chkbox2UnitPriceForcedPrice->show() : ui->chkbox2UnitPriceForcedPrice->hide();

        if ( validPlu )
            ui->le2UnitPrice->setText(currentPLUData->unitPrice());

        if(currentPLUData->forcedUnitPrice())
        {
            ui->chkbox2UnitPriceForcedPrice->setChecked(true);
            ui->le2UnitPrice->setText("");
        }

        else
        {
            ui->chkbox2UnitPriceForcedPrice->setChecked(false);
            if ( validPlu )
                ui->le2UnitPrice->fromSessionField(currentPLUData->unitPrice());
        }
    }

    // Discount Pricing - (Frequent Shopper)

    ui->gb2DiscountType->move( nextSectionAvailableX, nextSectionAvailableY );
    nextSectionAvailableY += ui->gb2DiscountType->height() + PRICING_TAB_GROUP_BOX_SPACING;

    ui->gb2DiscountType->show();


    if(currentPLUData->itemNumber().toInt() > 0)
    {
        ui->cb2DiscountType->setCurrentIndex(ui->cb2DiscountType->findData(currentPLUData->discountType()));
    }
    else
    {
        ui->cb2DiscountType->setCurrentIndex(0);
    }

    // enable or disable the discount type combo
    if(frequentShopperData->perProductOrScale().toInt() == 0)
    {
        // per product
        ui->cb2DiscountType->setEnabled(true);
    }
    else
    {
        // per scale
        ui->cb2DiscountType->setEnabled(false);
    }

    locationOfDiscountOrRegularPriceGroupBoxY = nextSectionAvailableY;

    if(currentPLUData->discountType().toInt() == 4)
    {
        // It is Regular Pricing
        ui->gb2RegularPrice->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2RegularPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2RegularPrice->show();
        gbRegularPriceShown = true;
        widgetUsedForDiscountValue1 = "le2RegularPrice";
    }
    else
    {
        // It is Discount Pricing
        ui->gb2Discount->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2Discount->height();
        ui->gb2Discount->show();
        gbDiscountShown = true;

        if(currentPLUData->discountType().toInt() == 3 || currentPLUData->discountType().toInt() == 5)
        {
            // It is percentage
            ui->lbl2Discount1CurrencySymbol->hide();
            ui->lbl2Discount2CurrencySymbol->hide();
            ui->lbl2Discount3CurrencySymbol->hide();
            ui->lbl2Discount1PercentSymbol->show();
            ui->lbl2Discount2PercentSymbol->show();
            ui->lbl2Discount3PercentSymbol->show();
            //Percentage is a symbol, not translated

            // Change the top and decimal places of the Validator
            discountValue1Validator_->setTop(makeValidatorTop(4));
            ui->le2Discount1->setDecimalPlaces(2);
            discountValue2Validator_->setTop(makeValidatorTop(4));
            ui->le2Discount2->setDecimalPlaces(2);
            discountValue3Validator_->setTop(makeValidatorTop(4));
            ui->le2Discount3->setDecimalPlaces(2);
        }
        else
        {
            // It is currency
            ui->lbl2Discount1PercentSymbol->hide();
            ui->lbl2Discount2PercentSymbol->hide();
            ui->lbl2Discount3PercentSymbol->hide();
            ui->lbl2Discount1CurrencySymbol->show();
            ui->lbl2Discount2CurrencySymbol->show();
            ui->lbl2Discount3CurrencySymbol->show();
            // User choice of currency symbol
            ui->lbl2Discount1CurrencySymbol->setText(currencyData->priCurrSymbol());
            ui->lbl2Discount2CurrencySymbol->setText(currencyData->priCurrSymbol());
            ui->lbl2Discount3CurrencySymbol->setText(currencyData->priCurrSymbol());

            // FT-81 Discount Price fields on Price Fields screen function incorrectly. This is tied to bug
            // FT-143 Discount field only allows 4 digits and the fields only show percent signs or nothing at all.
            // Where in the description its stated the operator are supposed to be able to enter 5 digits.  I'm not sure if 5
            // hardcoded digits is really the path we should of went down in fixing this, but I hardcoded 5.
            // Change the top and decimal places of the validator
            discountValue1Validator_->setTop(makeValidatorTop(5));
            ui->le2Discount1->setDecimalPlaces(2);
            discountValue2Validator_->setTop(makeValidatorTop(5));
            ui->le2Discount2->setDecimalPlaces(2);
            discountValue3Validator_->setTop(makeValidatorTop(5));
            ui->le2Discount3->setDecimalPlaces(2);
        }
        // Yikes, I hate to do this but currently, percentage values are being equated to currency values. If scale's frequent shopper values were set to use percent off then switched to cents off, then a 10.00% discount would turn into $10.00 off discount. SOLUTION: Derive the percentage off in terms of cents and display that, or vice versa, e.g., 20.00% off a $3.00 item would display $0.60 instead of $20.00 as it does now.
        widgetUsedForDiscountValue1 = "le2Discount1";
    }
    if(!noPLU)
    {
        ui->le2RegularPrice->fromSessionField(currentPLUData->discountValue1());
        ui->le2Discount1->fromSessionField(currentPLUData->discountValue1());
        ui->le2Discount2->fromSessionField(currentPLUData->discountValue2());
        ui->le2Discount3->fromSessionField(currentPLUData->discountValue3());
    }
    else
    {
        ui->le2RegularPrice->setText("");
        ui->le2Discount1->setText("");
        ui->le2Discount2->setText("");
        ui->le2Discount3->setText("");
    }
    // Points - Always shown to bottom right of Discount Groupbox
    ui->gb2Points->move( scaleWidthForScreen( PRICING_TAB_POINTS_X ), nextSectionAvailableY );
    //nextSectionAvailableY += ui->gb2Points->geometry().height() + PRICING_TAB_GROUP_BOX_SPACING;
    ui->gb2Points->show();

    if(!noPLU) ui->le2Points->fromSessionField(currentPLUData->points());

    // Tare
    if((currentPLUData->productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT || currentPLUData->productType() == ProductType::PRODUCTTYPE_BYCOUNT ) || (currentPLUData->productType() == ProductType::PRODUCTTYPE_BYCOUNT && scaleConfigData->weightOnByCount()))
    {
        // Show Combo for Platter/Keyboard Tare Combo
        if(scaleConfigData->isPlatterTareOn() == "1")
        {
            ui->gb2PlatterTare->move( nextSectionAvailableX, nextSectionAvailableY );
            nextSectionAvailableY += ui->gb2PlatterTare->height();
            ui->gb2PlatterTare->show();
            gbTarePlatterTypeShown = true;
            ui->cb2TarePlatterTareType->clear();
            ui->cb2TarePlatterTareType->addItem(tr("Keyboard"), "0");
            ui->cb2TarePlatterTareType->addItem(tr("Platter"), "1");

            if(currentPLUData->itemNumber().toInt() > 0)
            {
                ui->cb2TarePlatterTareType->setCurrentIndex(ui->cb2TarePlatterTareType->findData(currentPLUData->tareType()));
            }

            if(scaleConfigData->isScaleMetric())
            {
                ui->lbl2TarePlatterUnitSymbolTare->setText("kg");
            }
            else
            {
                ui->lbl2TarePlatterUnitSymbolTare->setText("lb");
            }
            if(showForcedFields)
            {
                ui->chkbox2TarePlatterForcedTare->show();
                if(currentPLUData->itemNumber().toInt() > 0 && currentPLUData->forcedTare())
                {
                   ui->chkbox2TarePlatterForcedTare->setChecked(true);
                   ui->le2TarePlatter->setText("");
                }
                else
                {
                    ui->chkbox2TarePlatterForcedTare->setChecked(false);
                    if (!noPLU)ui->le2TarePlatter->fromSessionField(currentPLUData->tare());
                }
            }
        }
        else
        {
            // Show Regular Tare
            ui->gb2Tare->move( nextSectionAvailableX, nextSectionAvailableY );
            nextSectionAvailableY += ui->gb2Tare->height() + PRICING_TAB_GROUP_BOX_SPACING;

            // HT-136: Change label if using 2nd Tare (label_9 is no longer marked as translatable)
            QString lblTare( scaleConfigData->useSecondTare() ? tr("Tare") + " 2"  : tr("Tare") );
            ui->label_9->setText( lblTare );

            ui->gb2Tare->show();
            gbTareShown = true;

            if( !noPLU )
            {
                // HT-136: Set the text field to the correct tare value
                if( scaleConfigData->useSecondTare() )
                    ui->le2Tare->fromSessionField( currentPLUData->tareB() );
                else
                    ui->le2Tare->fromSessionField( currentPLUData->tare() );
            }

            ui->lbl2UnitPriceCurrencySymbol->setText(currencyData->priCurrSymbol());
            showForcedFields ? ui->chkbox2ForcedTare->show() : ui->chkbox2ForcedTare->hide();
            if(currentPLUData->forcedTare())
            {
                ui->chkbox2ForcedTare->setChecked(true);
                ui->le2Tare->setText("");
            }
            else
            {
                ui->chkbox2ForcedTare->setChecked(false);

                if( !noPLU )
                {
                    // HT-136: Set the text field to the correct tare value
                    if( scaleConfigData->useSecondTare() )
                        ui->le2Tare->fromSessionField( currentPLUData->tareB() );
                    else
                        ui->le2Tare->fromSessionField( currentPLUData->tare() );
                }
            }
            if(scaleConfigData->isScaleMetric())
            {
                ui->lbl2UnitSymbolTare->setText("kg");
            }
            else
            {
                ui->lbl2UnitSymbolTare->setText("lb");
            }
        }
    }

    // Proportional Tare
    int modelNum = scaleConfigData->scaleModel().toInt();
    if(modelNum == ScaleModel::ScaleModel_ESC )
    {
        if((currentPLUData->productType() == ProductType::PRODUCTTYPE_RANDOMWEIGHT || currentPLUData->productType() == ProductType::PRODUCTTYPE_UNKNOWN ) && internalConfigData->isProportionalTareOn)
        {
            ui->gb2ProportionalTare->move( nextSectionAvailableX, nextSectionAvailableY );
            nextSectionAvailableY += ui->gb2ProportionalTare->height() + PRICING_TAB_GROUP_BOX_SPACING;
            ui->gb2ProportionalTare->show();
            gbProportionalTareShown = true;
            if (!noPLU) ui->le2ProportionalTare->fromSessionField(currentPLUData->proportionalTare());
        }
    }

    // Exception Price
    if( ProductType::PRODUCTTYPE_BYCOUNT == currentPLUData->productType() )
    {
        ui->gb2ExceptionPrice->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2ExceptionPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2ExceptionPrice->show();
        gbExceptionPriceShown = true;
        ui->lbl2CurrencySymbolExceptionPrice->setText(currencyData->priCurrSymbol());
        if (!noPLU) ui->le2ExceptionPrice->fromSessionField(currentPLUData->exceptionPrice());
    }

    // Net Weight
    if(currentPLUData->productType() == ProductType::PRODUCTTYPE_FIXEDWEIGHT)
    {
        ui->gb2NetWeightOunces->move(nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2NetWeightOunces->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2NetWeightOunces->show();
        gbNetWeightOuncesShown = true;
        if(scaleConfigData->isScaleMetric())
        {
            ui->lbl2UnitSymbolNetWeight->setText("g");
        }
        else
        {
            ui->lbl2UnitSymbolNetWeight->setText("oz");
        }
        if (!noPLU) ui->le2NetWeight->fromSessionField(currentPLUData->fixedWeight());
        widgetUsedForFixedWeight = "le2NetWeight";
    }
    else if( currentPLUData->productType() == ProductType::PRODUCTTYPE_FLUIDOUNCE )
    {
        ui->gb2NetWeightFluidOunces->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gb2NetWeightFluidOunces->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2NetWeightFluidOunces->show();
        gbNetWeightFluidShown = true;
        if (!noPLU) ui->le2NetWeightFluid->fromSessionField(currentPLUData->fixedWeight());
        widgetUsedForFixedWeight = "le2NetWeightFluid";
    }

    ifUpdating = false;

    this->update();
}

void superChangeProductPriceFields::doPluLookup()
{
    // in process of loading plu already, so do nothing
    if ( hasChanged )
        return;

    if( requestedPlu )                     // set just later in this method. Don't do multiple lookups on same item from multiple emits
        return;

	int requestThisPlu = ui->leProductNumber->text().toInt();

    if(  pluEmitted == requestThisPlu )
        return;                                                     // can't request the same thing again

    pluEmitted = requestThisPlu;

    //lookup PLU
    if(productNumber != ui->leProductNumber->text() && ui->leProductNumber->text() != "" && !cameFromCancel && !popupActive)
    {
        if(productNumber != "" && productNumber != "0")
            saveProduct();

        if( wroteTransaction && productNumber.toInt() > 0 && requestThisPlu > 0 )
            session_->send(ProductDev, ProductSaveSupervisorTransaction);

        requestedPlu = true;    // FT-925 Unit price fields not populating
        hasChanged = true;
        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
    }

    whoHasFocus = SAV_NONE;
    wroteTransaction = false;
}

void superChangeProductPriceFields::createNewProductCancel_clicked()
{
    ui->leProductNumber->setText(productNumber);
    ui->gbNewProduct->hide();
    popupActive = false;
    cameFromCancel = false;
    requestedPlu = false;                   // need to be able to go active again after a lookup
}

void superChangeProductPriceFields::createNewProductOK_clicked()
{

    QString msg = ui->leProductNumber->text() + ",";

    // FT-1029 PLUs created on the Price Page (and all of the others) becomes the previous entered PLU
    // So we need to make sure once we go from a sub page, to All Fields, we seed the currentPLU with the new PLU
    currentPLUData->itemNumber() = ui->leProductNumber->text();

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


void superChangeProductPriceFields::popupCreateNewPLUDialog()
{
    ifUpdating = true;
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(true);
    ui->butOK->setEnabled(true);
    ifUpdating = false;

    // ask producttype
    ui->lblcreateNewProductQuestion->setText(tr("Product") + " " + ui->leProductNumber->text() + " " + tr("does not exist. Create new product?"));

    scaleConfigData->isScaleMetric()? ui->rbFluidOunces->hide() : ui->rbFluidOunces->show();

    ui->gbNewProduct->move( scalePointForScreen( QPoint( 12, 20 ) ) );
    ui->gbNewProduct->raise();
    ui->gbNewProduct->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductPriceFields::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::translate()
{
	/* ALL Widget Text MUST be in here even if its the default text */

    ui->retranslateUi( this );
    ui->cb2DiscountType->setItemText(0, tr("None"));
    ui->cb2DiscountType->setItemText(1, tr("Cents off"));
    ui->cb2DiscountType->setItemText(2, tr("Discount price"));
    ui->cb2DiscountType->setItemText(3, tr("Percent off"));
    ui->cb2DiscountType->setItemText(4, tr("Advertised price"));
    ui->cb2DiscountType->setItemText(5, tr("Percent added"));
    ui->cb2DiscountType->setItemText(6, tr("Package discount"));

    this->update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

bool superChangeProductPriceFields::onScreenChanging()
{
    // Need to check if we missed the last call to these
    if( whoHasFocus == SAV_UNITPRICE )
        trackSuperChangeUnitPrice();
    else if( whoHasFocus == SAV_LBSFOR )
        trackSuperChangeLbsForPrice();
    else if( whoHasFocus == SAV_FRACPRICE )
        trackSuperChangeFractionalPrice();
    else if( whoHasFocus == SAV_TARE )
        trackSuperChangeTare();
    else if( whoHasFocus == SAV_PROPTARE )
        trackSuperChangePropTare();
    else if( whoHasFocus == SAV_BYCOUNT )
         trackSuperChangeByCount();
    else if( whoHasFocus == SAV_NETWT )
        trackSuperChangeNetWeight();
    else if( whoHasFocus == SAV_FLNETWT )
        trackSuperChangeFluidNetWeight();

    saveProduct();

    if( wroteTransaction )
        session_->send(ProductDev, ProductSaveSupervisorTransaction);

    whoHasFocus = SAV_NONE;
    wroteTransaction = false;

    if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
    {
        requestedPlu = true;                                        // set bc we are sending a 101,5
        session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductPriceFields::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        popupActive = true;
        popupCreateNewPLUDialog();

        hasChanged = false;
    }
    else if(ProductDev == msg[0].toInt() && ProductWrite == msg[1].toInt())
    {
        //currentPLUData = &session_->scaleConfig->currentPLU;
        productNumber = msg[2];
        requestedPlu = false;                                       // reset bc we got our 101,7 back

        if(productNumber.toInt() > 0)
        {
            if (!currentPLUData->forcedTare())  //  HA-145
            {
                // Then we need to check the precision digits digits and maybe update the tare
                int digitsOfPrecision = scaleConfigData->tarePrecision();
                QStringList tareList = ( scaleConfigData->useSecondTare() ? currentPLUData->tareB().split(".") : currentPLUData->tare().split(".") );
                int tareDigitsLen = tareList.at(1).length();

                // Now make sure the tare has the same number of digits of its required precision
                if(3 == digitsOfPrecision && 2 == tareDigitsLen)
                {
                    // Means our tare has 2 digits of precision, and we need 3
                    if( scaleConfigData->useSecondTare() )
                        currentPLUData->tareB() = currentPLUData->tareB() + "0";
                    else
                        currentPLUData->tare() = currentPLUData->tare() + "0";
                }
                else if(2 == digitsOfPrecision && 3 == tareDigitsLen)
                {
                    // we have too much tare, crop the last digit
                    if( scaleConfigData->useSecondTare() )
                        currentPLUData->tareB() = currentPLUData->tareB().mid(0, currentPLUData->tareB().length() - 1);
                    else
                        currentPLUData->tare() = currentPLUData->tare().mid(0, currentPLUData->tare().length() - 1);
                }
            }
        }

        reloadFields();

        hasChanged = false;
    }
    // plu not on file
    else if (( ProductDev == msg[0].toInt() ) && ( ProductClearUI == msg[1].toInt() ))
    {
        reloadFields();
    }
}

bool superChangeProductPriceFields::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
