/* @file superChangeProductTare.cpp
 * superChangeProductTare 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd
 */

#include <logmanager.h>

#include "superChangeProductAllFields.h"
#include "superChangeProductTare.h"
#include "ui_superChangeProductTare.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>

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

superChangeProductTare::superChangeProductTare(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductTare)
{
    // saveOnExit_(false),

    ui->setupUi(this);
    scaleScreen();

    ui->gbTareNotAvailable->hide();
    ui->gbNewProduct->hide();
    ui->gbTareMaxReached->hide();
    ui->gbProportionalTare->hide();
    ui->gbTare->show();

    ui->cbForcedTare->blockSignals(true);

    connect(ui->cbForcedTare, SIGNAL(stateChanged(int)), this, SLOT(reactToForcedTareClick(int)));

    ui->lblDescriptionContent->setText("");
    connect(ui->butOK,     SIGNAL(clicked()), this, SLOT(createNewProductOK_clicked()));
    connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(createNewProductCancel_clicked()));

    connect(ui->leProductNumber, SIGNAL(editingFinished()), this, SLOT(doPluLookup()));
    connect(ui->leTare, SIGNAL(editingFinished()),          this, SLOT(hideMaxReachedGroupBox()));
    connect(ui->leTare, SIGNAL(textChanged(QString)),       this, SLOT(checkAgainstMaxTare(QString)));

    // tares
    //connect(ui->leTare,             SIGNAL(editingFinished()), this, SLOT(trackSuperChangeTare()));
    connect(ui->leProportionalTare, SIGNAL(editingFinished()), this, SLOT(trackSuperChangePropTare()));

    connect(ui->leTare,             SIGNAL( textEdited(QString) ), this, SLOT( onTareTextChanged(QString) ) );
    connect(ui->leProportionalTare, SIGNAL( textEdited(QString) ), this, SLOT( onPropTareTextChanged(QString) ) );

    currentPLUData  = &session_->scaleConfig->currentPLU;
    scaleConfigData = &session_->scaleConfig->scaleConfig;
    currencyData    = &session_->scaleConfig->currencySettings;
    weigherConfig   = &session_->scaleConfig->weigherConfigRead;
    internalConfig  = &session_->scaleConfig->internalConfig;

    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));

    weigherConfig->dualRangeLimitRaw();
    int tmpPrec = scaleConfigData->tarePrecision();
    ui->leTare->setValidator(tareValidator_ = new QIntValidator(0, INT_MAX, this));
    if(tmpPrec == 2)
        ui->leTare->setDecimalPlaces(2);
    else
        ui->leTare->setDecimalPlaces(3);

    ui->leTare->setDecimal(currencyData->unitSeparator());
    ui->leTare->setInputWrapping(true);
    ui->leTare->setInputTruncation(true);

    ui->leProportionalTare->setValidator(proportionalTareValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProportionalTare->setDecimalPlaces(1);
    ui->leProportionalTare->setDecimal(currencyData->unitSeparator());
    ui->leProportionalTare->setInputWrapping(true);
    ui->leProportionalTare->setInputTruncation(true);

    proportionalTareValidator_->setTop(makeValidatorTop(3));

    popupActive = false;
    cameFromCancel = false;
    switchScreensOnNewPLU = false;
    requestedPlu = false;
    wroteTransaction = false;
    ifUpdating = false;

    prev_tareA       = "0.00";
    prev_tareB       = "0.00";
    prev_proptare    = "0.0";

    pluEmitted = 0;
    ui->leTare->installEventFilter( this );


    session_->send(ConfigurationDev, CfgScaleOperationRead); // 112,70 to get max tare message returned

    // FT-902: Have to force lookup of PLU if currentPLUData has a PLU #, as the rest
    // of the currentPLUData was not current when processed by initialize() method.
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber() );

    translate();
}

void superChangeProductTare::reactToForcedTareClick(int state)
{
    if ( ifUpdating )
        return;

    ui->cbForcedTare->blockSignals(true);
    QString newValue = (state) ? "1" : "0";
    QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareAForced ) + "," + newValue + "," + currentPLUData->forcedTareRaw() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    if(state)
    {
        ui->leTare->setText("");
        trackSuperChangeTare();
    }
    else
    {
        zeroOutTareFromForcedTareClick();
    }
}

int superChangeProductTare::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

superChangeProductTare::~superChangeProductTare()
{
    delete ui;
}

void superChangeProductTare::zeroOutTareFromForcedTareClick()
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
    ui->leTare->setText("0" + currencyData->unitSeparator() + zeroOutPrecisionText);
}

void superChangeProductTare::initialize()
{
    currentPLUData = &session_->scaleConfig->currentPLU;
    currencyData = & session_->scaleConfig->currencySettings;
    internalConfig = &session_->scaleConfig->internalConfig;

    showForcedFields = false;
    nextSectionAvailableX = scaleWidthForScreen( 29 );
    nextSectionAvailableY = scaleHeightForScreen( 190 );

    bool noPLU = false;
    ifUpdating = true;

    whoHasFocus =  SAV_NONE;

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
        if( 2 == pluType || 4 == pluType )
            showForcedFields = true;
        else if( newIntPrefix > 0 )
            showForcedFields = true;
    }

    //Don't set max tare top because there is a notice when user exceeds the max tare so the User knows why a value cannot be entered. If I just set the top to the max tare, the User could think that the scale is malfunctioning because it won't accept a value the User wants to enter. This way the scale lets the User know what the max is. Discussed with Highley on 3/15/2013

    ui->gbNewProduct->hide();
    ui->gbTareNotAvailable->hide();
    ui->gbTareMaxReached->hide();
    ui->gbProportionalTare->hide();

    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
    {
        ui->gbTare->hide();
        ui->leProductNumber->fromSessionField(currentPLUData->itemNumber());
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
        //ui->lblDescriptionContent->setText(currentPLUData->description());
        firstTimeIn = false;
    }
    else
    {
        // give some default blank display
        ui->gbTare->show();
        ui->leTare->setText("");
        ui->lblDescriptionContent->setText("");
        ui->cbForcedTare->setChecked(false);
        noPLU = true;
        firstTimeIn = true;
    }

    // 4,6 - Operation Mode Supervisor must be sent according to .asp pages (It has been removed from my pages)

    hasChanged = false;
    pluPassedIn = false;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->separator = currencyData->unitSeparator();

    if(scaleConfigData->isScaleMetric())
    {
        ui->lblTareUnit->setText("kg");
    }
    else
    {
        ui->lblTareUnit->setText("lb");
    }

    // TARE
    if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() || currentPLUData->productType() == ProductType::PRODUCTTYPE_UNKNOWN || ( ProductType::PRODUCTTYPE_BYCOUNT == currentPLUData->productType() && scaleConfigData->weightOnByCount() ) )
    {
        ui->gbTare->move( nextSectionAvailableX, nextSectionAvailableY );
        nextSectionAvailableY += ui->gbTare->height() + 3;

        // HT-136: Change label if using 2nd Tare (lblTare is no longer marked as translatable)
        QString lblTare( scaleConfigData->useSecondTare() ? tr("Tare") + " 2"  : tr("Tare") );
        ui->lblTare->setText( lblTare );

        ui->gbTare->show();
        if( ProductType::PRODUCTTYPE_UNKNOWN == currentPLUData->productType() || !showForcedFields)
        {
            ui->cbForcedTare->hide();
        }
        else
        {
            ui->cbForcedTare->show();
        }

        // Set forced tare whether it is shown or not - FOR SAVING LATER
        if(currentPLUData->forcedTare())
        {
            ui->cbForcedTare->setChecked(true);
            ui->leTare->setText("");
        }
        else
        {
            ui->cbForcedTare->setChecked(false);
            // HT-136: Set the text field to the correct tare value
            noPLU ? ui->leTare->setText("") : ui->leTare->fromSessionField( (scaleConfigData->useSecondTare() ? currentPLUData->tareB() : currentPLUData->tare() ) );
        }
    }
    else
    {
        // Show red "tare not available" message according to Highley 3/15/2013
        if(!firstTimeIn)
        {
            ui->gbTareNotAvailable->move( scaleWidthForScreen( 2 ), nextSectionAvailableY );
            nextSectionAvailableY += ui->gbTareNotAvailable->height() + 3;
            ui->gbTareNotAvailable->show();
        }
    }

    // PROPORTIONAL TARE
    if(scaleConfigData->scaleModel().toInt() == ScaleModel::ScaleModel_ESC)
    {
        if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() && internalConfig->isProportionalTareOn)
        {
            ui->gbProportionalTare->move( nextSectionAvailableX, nextSectionAvailableY );
            nextSectionAvailableY += ui->gbProportionalTare->height();
            ui->gbProportionalTare->show();
        }
        ui->leProportionalTare->fromSessionField(currentPLUData->proportionalTare());
    }

    ifUpdating = false;

    update();
}

void superChangeProductTare::checkAgainstMaxTare(QString text)
{

    text = text.replace(",",".");

    if(ui->leTare->text().toDouble() > maxTareFromScaleConfig.toDouble() && (ui->leTare->text() != "" || ui->leTare->text().length() > 0))
    {
        reachedLimit = true;
        ui->gbTareMaxReached->move( scaleWidthForScreen( 2 ), nextSectionAvailableY );
        ui->lblTareMaxReached->setText(tr("A Tare of ") + ui->leTare->text() + " " + ui->lblTareUnit->text() + " exceeds maximum Tare of " + maxTareFromScaleConfig.replace(".", currencyData->unitSeparator()) + " " + ui->lblTareUnit->text());
        ui->gbTareMaxReached->raise();
        ui->gbTareMaxReached->show();
        ui->leTare->setText("");
    }
    else
    {
        if (ui->leTare->text() != "")
        {
            ui->gbTareMaxReached->hide();

        }
    }
    latentCheck = true;
}

void superChangeProductTare::hideMaxReachedGroupBox()
{
    ui->gbTareMaxReached->hide();

    trackSuperChangeTare();
}


void superChangeProductTare::createNewProductCancel_clicked()
{
    cameFromCancel = true;
    ui->leProductNumber->setText(productNumber);
    ui->gbNewProduct->hide();
    requestedPlu = false;                   // need to be able to go active again after a lookup
    popupActive = false;
    cameFromCancel = false;
    requestedPlu = false;                   // need to be able to go active again after a lookup
}

// usually this is a SLOT method, but a SLOT method already existed for editingFinished.  So from
// that slot method I just decided to call this method and everybody lived happily ever after.
void superChangeProductTare::trackSuperChangeTare()
{
    // if tracking supervisor changes send a single field write to app for Tare or 2nd Tare
    // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.

    if ( ifUpdating )
        return;

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();

    if( !scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->leTare->text().replace(separator,".");  //  tare() = 21

        if( tareFieldVal == prev_tareA || tareFieldVal == currentPLUData->tare() )
            return;

        wroteTransaction = true;
        prev_tareA = tareFieldVal;

        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->tare() = tareFieldVal;
    }
    else if( scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->leTare->text().replace(separator,".");  //  2ndtare() = 104

        if( tareFieldVal == prev_tareA || tareFieldVal == currentPLUData->tare() )
            return;

        wroteTransaction = true;
        prev_tareB = tareFieldVal;

        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tareB() + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->tare() = tareFieldVal;
    }
}

void superChangeProductTare::trackSuperChangePropTare()
{
    // if tracking supervisor changes send a single field write to app for Proportional Tare

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();
    QString tareFieldVal = ui->leProportionalTare->text().replace(separator,".");

    if( tareFieldVal == currentPLUData->proportionalTare() )
        return;

    wroteTransaction = true;
    prev_proptare = tareFieldVal;

    QString msg = productNumber + "," + QString::number( Product::fieldOffsetProportionalTare ) + ",\"" + tareFieldVal + "\"," + currentPLUData->proportionalTare() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->tare() = tareFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductTare::onTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_TARE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductTare::onPropTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_PROPTARE;
}

void superChangeProductTare::createNewProductOK_clicked()
{
    ui->gbNewProduct->hide();
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


void superChangeProductTare::doSave()
{
    hasChanged = true;
}

void superChangeProductTare::doPluLookup()
{
	int requestThisPlu = ui->leProductNumber->text().toInt();

    if(  pluEmitted == requestThisPlu )
        return;                                                     // can't request the same thing again
    
    pluEmitted = requestThisPlu;
    
    if( requestedPlu )                     // set just later in this method. Don't do multiple lookups on same item from multiple emits
        return;

    hasChanged = true;
    if(productNumber != ui->leProductNumber->text() && ui->leProductNumber->text() != "" && !cameFromCancel && !popupActive)
    {
        saveProduct();

        if( wroteTransaction && productNumber.toInt() > 0 && requestThisPlu > 0 )
            session_->send(ProductDev, ProductSaveSupervisorTransaction);

        wroteTransaction = false;
        requestedPlu = true;    // FT-925 Unit price fields not populating
        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
    }

    whoHasFocus = SAV_NONE;
    wroteTransaction = false;
}

void superChangeProductTare::popupCreateNewPLUDialog()
{
    ifUpdating = true;

    ui->gbTareNotAvailable->hide();
    ui->gbTareMaxReached->hide();
    update();
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(true);

    ui->butOK->setEnabled(true);

    // ask producttype
    ui->lblcreateNewProductQuestion->setText(tr("Product") + " " + ui->leProductNumber->text() + " " + tr("does not exist. Create new product?"));
    ui->rbFluidOunces->hide();
    ui->rbFixedWeight->show();

    if(scaleConfigData->isScaleMetric())
    {
        ui->rbFluidOunces->hide();
    }
    else
    {
        ui->rbFluidOunces->show();
    }
    ui->gbNewProduct->move( scalePointForScreen( QPoint( 15, 20 ) ) );
    ui->gbNewProduct->raise();
    ui->gbNewProduct->show();

    ifUpdating = false;
}

void superChangeProductTare::translate()
{
    ui->retranslateUi( this );
    update();
}

bool superChangeProductTare::eventFilter( QObject *obj, QEvent *event )
{
    ifUpdating = true;
    if ( ui->leTare == obj )
    {
        if ( QEvent::FocusIn == event->type() )
        {
            ui->cbForcedTare->setChecked(false);
        }
    }
    ifUpdating = false;
    return _Mybase::eventFilter( obj, event );
}

void superChangeProductTare::saveProduct()
{
    if(switchScreensOnNewPLU)
        return;

    if( requestedPlu )                               // set in doPluLookup bc we sent a 101,5. Dont save anything then
        return;

    QString msg;
    msg = ui->leProductNumber->text();

    if( currentPLUData->itemNumber() != "" && currentPLUData->itemNumber() != "0")
    {
        if(ui->cbForcedTare->isChecked())
        {
            // blank tare value and save
            // HT-136 Save correct Tare, using its proper offset
            if( scaleConfigData->useSecondTare() )
                msg += currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"\",\"" + currentPLUData->tareB() + "\",0";
            else
                msg += currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"\",\"" + currentPLUData->tare() + "\",0";

            session_->send(ProductDev, ProductWriteSingleField, msg);
            session_->send(ProductDev, ProductSave);
            // set forced tare value to 1
            msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareAForced ) + ",1" + "," + currentPLUData->forcedTareRaw() + ",0";
            session_->send(ProductDev, ProductWriteSingleField, msg);
            session_->send(ProductDev, ProductSave);
        }
        else
        {
            // Save tare value
            QString tareWeight = ui->leTare->text();

            // HT-136 Save correct Tare, using its proper offset
            if( scaleConfigData->useSecondTare() )
                msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"" + session_->uiUtilities->youGottaResetTheSeparator(tareWeight) + "\",\"" + currentPLUData->tareB() + "\",0";
            else
                msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"" + session_->uiUtilities->youGottaResetTheSeparator(tareWeight) + "\",\"" + currentPLUData->tare() + "\",0";

            session_->send(ProductDev, ProductWriteSingleField, msg);
            session_->send(ProductDev, ProductSave);
            // Save forced value to 0
            msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetTareAForced ) + "," + "0" + "," + currentPLUData->forcedTareRaw() + ",0";
            session_->send(ProductDev, ProductWriteSingleField, msg);
            session_->send(ProductDev, ProductSave);
        }

        // Save proportional tare value
        if(ui->gbProportionalTare->isVisible())
        {
            QString leProportionalTare = ui->leProportionalTare->text();
            msg = currentPLUData->itemNumber()+ "," + QString::number( Product::fieldOffsetProportionalTare ) + ",\"" + session_->uiUtilities->youGottaResetTheSeparator(leProportionalTare) + "\",\"" + currentPLUData->proportionalTare() + "\",0";
            session_->send(ProductDev, ProductWriteSingleField, msg);
            session_->send(ProductDev, ProductSave);
        }
    }
}

bool superChangeProductTare::onScreenChanging()
{
    if( whoHasFocus == SAV_TARE )
        trackSuperChangeTare();
    else if( whoHasFocus == SAV_PROPTARE )
        trackSuperChangePropTare();

    // do a save
    saveProduct();

    if( wroteTransaction )
        session_->send(ProductDev, ProductSaveSupervisorTransaction);

    wroteTransaction = false;
    whoHasFocus = SAV_NONE;

    if( !internalConfig->changeProductActive )                  // if we are not going to another Change Product Page
    {
        requestedPlu = true;                                        // set bc we are sending a 101,5
        session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductTare::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        //currentPLUData = &session_->scaleConfig->currentPLU;
        popupActive = true;
        popupCreateNewPLUDialog();
    }
    else if (ProductDev == msg[0].toInt() && ProductWriteSingleField == msg[1].toInt() && 1 == msg[2].toInt())
    {
        ui->gbNewProduct->hide(); //101,16
    }
    else if(ProductDev == msg[0].toInt() && ProductWrite == msg[1].toInt()) //101,7
    {
        ui->cbForcedTare->blockSignals(true);
        productNumber = msg[2];

        requestedPlu = false;                                       // reset bc we got our 101,7 back

        // FT-1012 Tare Precision
        // If the incoming digits do not match the actual tare precision, it messes stuff
        // up. Whatever the incoming number of digits is for the tare value, it overrides
        // the setDecimals() value of the tare QValidator. So right here I had to enforce
        // the number of tare digits to match the scaleCOnfig.tarePrecision.
        if ( 0 < productNumber.toInt() )
        {
            QString tare( scaleConfigData->useSecondTare() ? currentPLUData->tareB() : currentPLUData->tare() );

            // FT-????: Fix of crash when forced tare
            if ( tare.length() )
            {
                // Then we need to check the precision digits digits and maybe update the tare
                int digitsOfPrecision( scaleConfigData->tarePrecision() );

                QStringList tareList( tare.split( "." ) );
                int tareDigitsLen( 0 );

                if ( 2 <= tareList.size() )
                    tareDigitsLen = tareList.at( 1 ).length();

                // Now make sure the tare has the same number of digits of its required precision
                if (( 3 == digitsOfPrecision ) && ( 2 == tareDigitsLen ))
                {
                    // Means our tare has 2 digits of precision, and we need 3
                    if( scaleConfigData->useSecondTare() )
                        currentPLUData->tareB() = tare + "0";
                    else
                        currentPLUData->tare() = tare + "0";
                }
                else if (( 2 == digitsOfPrecision ) && ( 3 == tareDigitsLen ))
                {
                    // we have too much tare, crop the last digit
                    if( scaleConfigData->useSecondTare() )
                        currentPLUData->tareB() = tare.mid( 0, tare.length() - 1 );
                    else
                        currentPLUData->tare() = tare.mid( 0, tare.length() - 1 );
                }
            }
        }

        initialize();

        ui->cbForcedTare->blockSignals(false);
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgScaleOperationWrite == msg[1].toInt()) //112,71
    {
        maxTareFromScaleConfig = msg[21];
        tareEntryPrecisionFromScaleConfig = msg[26];
        tareValidator_->setTop(makeValidatorTop(2 + msg[26].toInt()));
        initialize();
    }
    // plu not on file
    else if (( ProductDev == msg[0].toInt() ) && ( ProductClearUI == msg[1].toInt() ))
    {
        initialize();
    }
}

bool superChangeProductTare::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
