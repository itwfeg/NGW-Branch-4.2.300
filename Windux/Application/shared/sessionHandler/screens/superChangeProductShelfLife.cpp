/* @file superChangeProductShelfLife.cpp
 * superChangeProductShelfLife 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd
 */

#include <logmanager.h>

#include "superChangeProductAllFields.h"
#include "superChangeProductShelfLife.h"
#include "ui_superChangeProductShelfLife.h"

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

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart{
    class numericLineEdit;
}

superChangeProductShelfLife::superChangeProductShelfLife(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductShelfLife)
{
    ui->setupUi(this);
    scaleScreen();

    internalConfigData = &session_->scaleConfig->internalConfig;

    ui->gbNewProduct->hide();

    // NUMERIC VALIDATORS
    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));

    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));

    ui->leShelfLifeDays->setValidator(shelfLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leShelfLifeDays->setDecimalPlaces(0);
    ui->leShelfLifeDays->setInputWrapping(true);
    ui->leShelfLifeDays->setInputTruncation(true);
    shelfLifeDaysValidator_->setTop(makeValidatorTop(3));

    ui->leShelfLifeHours->setValidator(shelfLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leShelfLifeHours->setDecimalPlaces(0);
    ui->leShelfLifeHours->setInputWrapping(true);
    ui->leShelfLifeHours->setInputTruncation(true);
    shelfLifeHoursValidator_->setTop(23);
    shelfLifeHoursValidator_->setBottom(0);

    ui->txtShelfLifeMinutes->setValidator(shelfLifeMinutesValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtShelfLifeMinutes->setDecimalPlaces(0);
    ui->txtShelfLifeMinutes->setInputWrapping(true);
    ui->txtShelfLifeMinutes->setInputTruncation(true);
    shelfLifeMinutesValidator_->setTop(59);
    shelfLifeMinutesValidator_->setBottom(0);

    ui->leProductLifeDays->setValidator(productLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductLifeDays->setDecimalPlaces(0);
    ui->leProductLifeDays->setInputWrapping(true);
    ui->leProductLifeDays->setInputWrapping(true);
    productLifeDaysValidator_->setTop(makeValidatorTop(3));

    ui->leProductLifeHours->setValidator(productLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductLifeHours->setDecimalPlaces(0);
    ui->leProductLifeHours->setInputWrapping(true);
    ui->leProductLifeHours->setInputWrapping(true);
    productLifeHoursValidator_->setTop(23);
    productLifeHoursValidator_->setBottom(0);

    ui->txtProductLifeMinutes->setValidator(productLifeMinutesValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProductLifeMinutes->setDecimalPlaces(0);
    ui->txtProductLifeMinutes->setInputWrapping(true);
    ui->txtProductLifeMinutes->setInputTruncation(true);
    productLifeMinutesValidator_->setTop(59);
    productLifeMinutesValidator_->setBottom(0);

    connect(ui->butOK, SIGNAL(clicked()), this, SLOT(createNewProductOK_clicked()));
    connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(createNewProductCancel_clicked()));
    connect(ui->leProductNumber, SIGNAL(editingFinished()), this, SLOT(doPluLookup()));

    currentPLUData = &session_->scaleConfig->currentPLU;
    scaleConfigData = &session_->scaleConfig->scaleConfig;

    popupActive = false;
    cameFromCancel = false;
    switchScreensOnNewPLU = false;
    requestedPlu = false;

    pluEmitted = 0;
    // FT-471: Have to force lookup of PLU if currentPLUData has a PLU #, as the rest
    // of the currentPLUData was not current when processed by initialize() method.
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber() );

    initialize();

    translate();
}

superChangeProductShelfLife::~superChangeProductShelfLife()
{
    delete ui;
}

int superChangeProductShelfLife::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductShelfLife::initialize()
{
    LOG_CLASS_INFO( "superChangeProductShelfLife", "initialize" );
    ui->gbNewProduct->hide();

    hasChanged = false;
    pluPassedIn = false;

    ui->lblDescriptionContent->setText("");

    currentPLUData = &session_->scaleConfig->currentPLU;
    scaleConfigData = &session_->scaleConfig->scaleConfig;
    LOG_DEBUG( "itemnumber is: %s", currentPLUData->itemNumber().toLatin1().constData() );
    LOG_DEBUG( "length is: %d", currentPLUData->itemNumber().length() );

    this->isScaleMetric = scaleConfigData->isScaleMetric();

    this->productNumber = currentPLUData->itemNumber();
    this->description = currentPLUData->description();
    this->productNumber = currentPLUData->itemNumber();

    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
    {
        LOG_DEBUG( "yes plu", NULL );
        ui->leProductNumber->fromSessionField(currentPLUData->itemNumber());
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
        //ui->lblDescriptionContent->setText(currentPLUData->description());

        ui->leShelfLifeDays->fromSessionField(currentPLUData->shelfLifeDaysRaw() );
        ui->leShelfLifeHours->fromSessionField(currentPLUData->shelfLifeHoursRaw());
        ui->txtShelfLifeMinutes->fromSessionField(currentPLUData->shelfLifeMinutesRaw());

        ui->leProductLifeDays->fromSessionField(currentPLUData->productLifeDaysRaw());
        ui->leProductLifeHours->fromSessionField(currentPLUData->productLifeHoursRaw());
        ui->txtProductLifeMinutes->fromSessionField(currentPLUData->productLifeMinutesRaw());

        ui->cbDontPrintPackedOn->setChecked(currentPLUData->dontPrintPackedOn()?true:false);
        ui->cbDontPrintSellBy->setChecked(currentPLUData->dontPrintSellBy()?true:false);
        ui->cbDontPrintBestBefore->setChecked(currentPLUData->dontPrintBestBefore()?true:false);
    }
    else
    {
        LOG_DEBUG( "no plu", NULL );
        ui->leProductNumber->setText("");
        ui->lblDescriptionContent->setText("");

        ui->leProductLifeDays->setText("");
        ui->leProductLifeHours->setText("");
        ui->txtProductLifeMinutes->setText("");

        ui->leShelfLifeDays->setText("");
        ui->leShelfLifeHours->setText("");
        ui->txtShelfLifeMinutes->setText("");

        ui->cbDontPrintBestBefore->setChecked(false);
        ui->cbDontPrintPackedOn->setChecked(false);
        ui->cbDontPrintSellBy->setChecked(false);
    }

    update();
}

void superChangeProductShelfLife::createNewProductCancel_clicked()
{
    cameFromCancel = true;
    ui->leProductNumber->setText(productNumber);
    ui->gbNewProduct->hide();
    popupActive = false;
    cameFromCancel = false;
    requestedPlu = false;                   // need to be able to go active again after a lookup
}

void superChangeProductShelfLife::createNewProductOK_clicked()
{
    hasChanged = true;

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
  //  accept();
}

void superChangeProductShelfLife::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

void superChangeProductShelfLife::doPluLookup()
{
    LOG_CLASS_INFO( "superChangeProductShelfLife", "doPluLookup" );

	int requestThisPlu = ui->leProductNumber->text().toInt();

    if(  pluEmitted == requestThisPlu )
        return;                                                     // can't request the same thing again

    if( requestedPlu )                     // set just later in this method. Don't do multiple lookups on same item from multiple emits
        return;

    hasChanged = true;
    //if(productNumber != ui->leProductNumber->text() && ui->leProductNumber->text().le )

    LOG_DEBUG( "Lookup", NULL );
    if(this->productNumber !=  ui->leProductNumber->text() && ui->leProductNumber->text() != "" && !cameFromCancel && !popupActive)
    {
        if(currentPLUData->itemNumber() != "" && currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().length() > 0)
        {
            saveProduct();
            requestedPlu = true;    // FT-925 Unit price fields not populating
        }
        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
    }
}

void superChangeProductShelfLife::popupCreateNewPLUDialog()
{
    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(true);

    ui->butOK->setEnabled(true);

    // ask producttype
    ui->lblcreateNewProductQuestion->setText(tr("Product") + " " + ui->leProductNumber->text() + " " + tr("does not exist. Create new product?"));

    scaleConfigData->isScaleMetric()?ui->rbFluidOunces->hide():ui->rbFluidOunces->show();

    ui->gbNewProduct->move( scalePointForScreen( QPoint( 15, 20 ) ) );
    ui->gbNewProduct->raise();
    ui->gbNewProduct->show();
}

bool superChangeProductShelfLife::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

void superChangeProductShelfLife::translate()
{
	/* ALL Widget Text MUST be in here even if its the default text */

    ui->retranslateUi( this );

    update();
}

void superChangeProductShelfLife::reloadFields()
{
    DomCurrentPLU *currentPLUData = &session_->scaleConfig->currentPLU;
    ui->leProductNumber->setText(currentPLUData->itemNumber());

    ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
    //ui->lblDescriptionContent->setText(currentPLUData->description());
    ui->leShelfLifeDays->setText( QString::number( currentPLUData->shelfLifeDays() ) );
    ui->leShelfLifeHours->setText( QString::number( currentPLUData->shelfLifeHours() ) );
    ui->txtShelfLifeMinutes->setText( QString::number( currentPLUData->shelfLifeMinutes() ) );

    ui->leProductLifeDays->setText( QString::number( currentPLUData->productLifeDays() ) );
    ui->leProductLifeHours->setText( QString::number( currentPLUData->productLifeHours() ) );
    ui->txtProductLifeMinutes->setText( QString::number( currentPLUData->productLifeMinutes() ) );

    ui->cbDontPrintBestBefore->setChecked(currentPLUData->dontPrintBestBefore());
    ui->cbDontPrintSellBy->setChecked(currentPLUData->dontPrintSellBy());
    ui->cbDontPrintPackedOn->setChecked(currentPLUData->dontPrintPackedOn());

    //  initialize();
}

void superChangeProductShelfLife::saveProduct()
{
    LOG_CLASS_INFO( "superChangeProductShelfLife", "saveProduct" );

    if(switchScreensOnNewPLU)
        return;

    if( requestedPlu )                               // set in doPluLookup bc we sent a 101,5. Dont save anything then
        return;

    if(currentPLUData->itemNumber().length() > 0 && currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber() != "") // don't save if no product number or it is 0
    {

        //DomCurrentPLU *currentPLUData = &session_->scaleConfig->currentPLU;

        LOG_DEBUG( "%s = %s", productNumber.toLatin1().constData(), currentPLUData->itemNumber().toLatin1().constData() );
        QString msg;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // A) save the shelf life days
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeDays ) + "," + ui->leShelfLifeDays->text() + "," + currentPLUData->shelfLifeDaysRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );
        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // B) save the shelf life hours
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeHours ) + "," + ui->leShelfLifeHours->text() + "," + currentPLUData->shelfLifeHoursRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // C) save the shelf life minutes
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeMinutes ) + "," + ui->txtShelfLifeMinutes->text() + "," + currentPLUData->shelfLifeMinutesRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // D) save the product life days
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetProductLifeDays ) + "," + ui->leProductLifeDays->text() + "," + currentPLUData->productLifeDaysRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // E) save the product life hours
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetProductLifeHours ) + "," + ui->leProductLifeHours->text() + "," + currentPLUData->productLifeHoursRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // F) save the product life minutes
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetProductLifeMinutes ) + "," + ui->txtProductLifeMinutes->text() + "," + currentPLUData->productLifeMinutesRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // G) save dont print packed on checkbox
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetDontPrintPackedOn ) + ",";
        if(ui->cbDontPrintPackedOn->isChecked()) msg += "1,";
        else msg+= "0,";
        msg += currentPLUData->dontPrintPackedOnRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // H) save dont print sell by checkbox
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetDontPrintSellBy ) + ",";
        if(ui->cbDontPrintSellBy->isChecked()) msg += "1,";
        else msg+= "0,";
        msg += currentPLUData->dontPrintSellByRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // I) save dont print best before checkbox
        msg = currentPLUData->itemNumber();
        msg += "," + QString::number( Product::fieldOffsetDontPrintBestBefore ) + ",";

        if(ui->cbDontPrintBestBefore->isChecked())
            msg += "1,";
        else
            msg+= "0,";

        msg += currentPLUData->dontPrintBestBeforeRaw() + ",0";
        LOG_DEBUG( "%s", msg.toLatin1().constData() );

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductShelfLife::onScreenChanging()
{
    // do a save

    // FT-1037 PLUs created in Shelf life show the all the values for the previous PLU entered - add switchScreensOnNewPLU

    if(!switchScreensOnNewPLU && ui->leProductNumber->text().length() > 0 && ui->leProductNumber->text() != "0") // don't save if no product number or it is 0
    {

        DomCurrentPLU *currentPLUData = &session_->scaleConfig->currentPLU;

        QString msg;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // A) save the shelf life days
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeDays ) + "," + ui->leShelfLifeDays->text() + "," + currentPLUData->shelfLifeDaysRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // B) save the shelf life hours
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeHours ) + "," + ui->leShelfLifeHours->text() + "," + currentPLUData->shelfLifeHoursRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // C) save the shelf life minutes
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetShelfLifeMinutes ) + "," + ui->txtShelfLifeMinutes->text() + "," + currentPLUData->shelfLifeMinutesRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        //////////////////////////////////////////////////////////////////////////////////////////

        // D) save the product life days
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetProductLifeDays ) + "," + ui->leProductLifeDays->text() + "," + currentPLUData->productLifeDaysRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // E) save the product life hours
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetProductLifeHours ) + "," + ui->leProductLifeHours->text() + "," + currentPLUData->productLifeHoursRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // F) save the product life minutes
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetProductLifeMinutes ) + "," + ui->txtProductLifeMinutes->text() + "," + currentPLUData->productLifeMinutesRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // G) save dont print packed on checkbox
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetDontPrintPackedOn ) + ",";
        if(ui->cbDontPrintPackedOn->isChecked()) msg += "1,";
        else msg+= "0,";
        msg += currentPLUData->dontPrintPackedOnRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // H) save dont print sell by checkbox
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetDontPrintSellBy ) + ",";
        if(ui->cbDontPrintSellBy->isChecked()) msg += "1,";
        else msg+= "0,";
        msg += currentPLUData->dontPrintSellByRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);

        // I) save dont print best before checkbox
        msg = ui->leProductNumber->text();
        msg += "," + QString::number( Product::fieldOffsetDontPrintBestBefore ) + ",";
        if(ui->cbDontPrintBestBefore->isChecked()) msg += "1,";
        else msg+= "0,";
        msg += currentPLUData->dontPrintBestBeforeRaw() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);
    }

    if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
    {
        requestedPlu = true;                                        // set bc we are sending a 101,5
        session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductShelfLife::onSessionMessage( const SessionMessage& msg )
{

// This is just an example of handling incoming messages to the class.  All of the methods
// listed below MUST have a declaration in the header file and a matching method in this file.

    if (Error == msg[0].toInt() &&  ErrProductDevice == msg[1].toInt() && 4 == msg[3].toInt())
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
        productNumber = msg[2];
        requestedPlu = false;                                       // reset bc we got our 101,7 back
        initialize();
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgScaleOperationWrite == msg[1].toInt()) //112,71
    {
       // ui->leTare->setMaxLength(3+tareEntryPrecisionFromScaleConfig.toInt());
        initialize();
    }
    // plu not on file
    else if (( ProductDev == msg[0].toInt() ) && ( ProductClearUI == msg[1].toInt() ))
    {
        initialize();
    }

	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductShelfLife::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
