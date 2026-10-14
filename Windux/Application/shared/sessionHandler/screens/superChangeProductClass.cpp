/* @file superChangeProductClass.cpp
 * superChangeProductClass 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief there is alot of code inthis file that implements the renumber and delete product number. It was decided on 1/25/2013 to eliminate the renumber and delte product number from this page but Larry wanted to maybe reuse it for the all product fields page so it is left in this file. The renumber and delte buttons are hidden so no execution will take place
 */

#include "superChangeProductAllFields.h"
#include "superChangeProductClass.h"
#include "ui_superChangeProductClass.h"

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

superChangeProductClass::superChangeProductClass(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductClass)
{
    reportingClass = "999999";
    reportingClassEmitted = 0;

    ui->setupUi(this);
    scaleScreen();

    internalConfigData = &session_->scaleConfig->internalConfig;

    connect(ui->butOK, SIGNAL(clicked()), this, SLOT(createNewProductOK_clicked()));
    connect(ui->butCancel, SIGNAL(clicked()), this, SLOT(createNewProductCancel_clicked()));

    connect(ui->leProductNumber, SIGNAL(editingFinished()), this, SLOT(doPluLookup()));

    // Reporting class connection for single field write to determine validness
    connect(ui->leClass,      SIGNAL(editingFinished()), this, SLOT(doReportingClassLookup()));

    //connect(ui->leClass, SIGNAL(textChanged()), this, SLOT(doSave()));
    // Numeric Validators
    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));
    ui->leClass->setValidator(reportingClassValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leClass->setDecimalPlaces(0);
    ui->leClass->setInputWrapping(true);
    reportingClassValidator_->setTop(makeValidatorTop(6));

    popupActive = false;
    cameFromCancel = false;
    switchScreensOnNewPLU = false;
    requestedPlu = false;

    ui->leClass->installEventFilter(this);
    currentPLUData = &session_->scaleConfig->currentPLU;
    scaleConfigData = &session_->scaleConfig->scaleConfig;
    if(currentPLUData->itemNumber() != "0" && currentPLUData->itemNumber().toInt() > 0)
    {
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber());
    }
    else // we have a current PLU
    {
        pluPassedIn = false;
        ui->leClass->setText("");
        ui->leProductNumber->setText("");
        ui->lblDescriptionContent->setText("");
         //   ui->leProductNumber->setFocus();
    }
    update();
    //this->initialize();
    translate();
    ui->gbNewProduct->hide();

}

bool superChangeProductClass::eventFilter( QObject *obj, QEvent *event )
{
    QString msg;

    if(ui->leClass == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            this->doSave();
        }
    }
    return _Mybase::eventFilter( obj, event );
}

superChangeProductClass::~superChangeProductClass()
{
    delete ui;
}

int superChangeProductClass::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void superChangeProductClass::initialize()
{
    ui->gbNewProduct->hide();

    hasChanged = false;
    pluPassedIn = false;

    ui->lblDescriptionContent->setText("");
    currentPLUData = &session_->scaleConfig->currentPLU;
    scaleConfigData = &session_->scaleConfig->scaleConfig;

    this->isScaleMetric = scaleConfigData->isScaleMetric();
    this->scaleModel = scaleConfigData->scaleModel();

    if(currentPLUData->itemNumber() == "0" || currentPLUData->itemNumber().toInt() < 1)
    {
        pluPassedIn = false;
        ui->leClass->setText("");
        ui->leProductNumber->setText("");
        ui->lblDescriptionContent->setText("");
     //   ui->leProductNumber->setFocus();
    }
    else // we have a current PLU
    {
        pluPassedIn = true;

        //session_->send(ProductDev, ProductRead, this->productNumber);
        // Display Current PLU
        ui->leProductNumber->fromSessionField(currentPLUData->itemNumber());
        ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
        //ui->lblDescriptionContent->setText(currentPLUData->description());
        // Load class number
        ui->leClass->setText(reportingClass);
        update();
    }
    //this->productNumber = ui->leProductNumber->text();

   // this->productNumber = "0";
   // ui->leClass->setText("");
   // ui->leProductNumber->setText("");
   // ui->lblDescriptionContent->setText("");
   // pluPassedIn = false;
   // ui->leProductNumber->setFocus();
    update();
}

void superChangeProductClass::doSave()
{
    hasChanged = true;
    //set the saving state for the destructor
}

void superChangeProductClass::doPluLookup()
{
    hasChanged = true;

    if( requestedPlu )                     // set just later in this method. Don't do multiple lookups on same item from multiple emits
        return;

    saveProduct();
    // save old product number and class
    // 101,
    if(productNumber != ui->leProductNumber->text() && ui->leProductNumber->text() != "" && !cameFromCancel && !popupActive)
    {
        //lookup PLU
        requestedPlu = true;    // FT-925 Unit price fields not populating

        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
    }
}

void superChangeProductClass::doReportingClassLookup()
{
    int requestThisClass = ui->leClass->text().toInt();

    if ( reportingClassEmitted == requestThisClass )
        return;

    reportingClassEmitted = requestThisClass;

    QString oldValue = currentPLUData->reportingClass();
    currentPLUData->reportingClass() = ui->leClass->text();
    //QString msg = currentPLUData->itemNumber() + ",28," + currentPLUData->reportingClass() + "," + oldValue;
    QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetProductReportingClassNum ) + "," + currentPLUData->reportingClass() + "," + oldValue;

    session_->send(ProductDev, ProductWriteSingleField, msg);
}

void superChangeProductClass::saveProduct()
{
    if(switchScreensOnNewPLU)
        return;

    if( requestedPlu )                               // set in doPluLookup bc we sent a 101,5. Dont save anything then
        return;

    // do a save
    if(productNumber != "" && productNumber != "0")
    {
        if(hasChanged && ui->leClass->text() != "" && ui->leClass->text() != "0")
        {
            // Send single write of the product's class field  101, 16, itemNumber, offsetindex, newValue, oldValue, canPrint(leave it blank when sending)

            //QString msg = productNumber + "," + "28" + "," + ui->leClass->text() + "," + currentPLUData->reportingClass() + ",0";
            QString msg = productNumber + "," + QString::number( Product::fieldOffsetProductReportingClassNum ) + "," + ui->leClass->text() + "," + currentPLUData->reportingClass() + ",0";
            qDebug() << "saving message is : " << msg;
            session_->send(ProductDev, ProductWriteSingleField, msg);

            // do a save 101, 22
            session_->send(ProductDev, ProductSave);
        }
    }
}

void superChangeProductClass::createNewProductCancel_clicked()
{
    cameFromCancel = true;
    ui->leProductNumber->setText(productNumber);
    ui->leClass->setText(reportingClass);

    ui->gbNewProduct->hide();
    popupActive = false;
    cameFromCancel = false;
    requestedPlu = false;                   // need to be able to go active again after a lookup
}

void superChangeProductClass::createNewProductOK_clicked()
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

    ui->gbNewProduct->hide();

    session_->send(ProductDev, ProductCreate, msg);
    cameFromCancel = false;
    popupActive = false;
    switchScreensOnNewPLU = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductClass::translate()
{
    ui->retranslateUi(this);
    update();
}

void superChangeProductClass::reloadFields()
{
    DomCurrentPLU *currentPLUData = &session_->scaleConfig->currentPLU;
    ui->leProductNumber->setText(currentPLUData->itemNumber());
    ui->leClass->setText(currentPLUData->reportingClass());
    ui->lblDescriptionContent->setText(superChangeProductAllFields::getEncodedTextOnly(currentPLUData->description()));
    //ui->lblDescriptionContent->setText(currentPLUData->description());
    ui->leClass->setFocus();
}

void superChangeProductClass::popupCreateNewPLUDialog()
{
    ui->gbNewProduct->move( scalePointForScreen( QPoint( 15, 20 ) ) );
    ui->gbNewProduct->raise();

    if(scaleConfigData->isScaleMetric())
    {
        ui->rbFluidOunces->hide();
    }

    ui->gbNewProduct->show();

    ui->rbByCount->setChecked(false);
    ui->rbFixedWeight->setChecked(false);
    ui->rbFluidOunces->setChecked(false);
    ui->rbRandomWeight->setChecked(true);

    ui->butOK->setEnabled(true);

    // ask producttype    
    ui->lblcreateNewProductQuestion->setText(tr("Product") + " " + ui->leProductNumber->text() + " " + tr("does not exist. Create new product?"));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductClass::onScreenChanging()
{
    this->saveProduct();

    if( !internalConfigData->changeProductActive )                  // if we are not going to another Change Product Page
    {
        requestedPlu = true;                                        // set bc we are sending a 101,5
        session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductClass::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        // 103,101,101001 - error reading PLU number
        popupActive = true;
        popupCreateNewPLUDialog();
    }
    else if(ProductDev == msg[0].toInt() && ProductWrite == msg[1].toInt())
    {
        productNumber = msg[2];
        reportingClass = msg[29];
        requestedPlu = false;                                       // reset bc we got our 101,7 back
        initialize();
        //reloadFields();
    }
    else if (ProductDev == msg[0].toInt() && ProductWriteSingleField == msg[1].toInt() )
    {
        int currentOffset = msg[3].toInt();
        DomUpdateRunModeField m( msg );

        if ( 28 == currentOffset )
        {
          // reporting clas - 101,16 telling us if it is valid .... or not

            bool isLegal =  m.isUpdateLegal();
            if ( isLegal )
            {
                currentPLUData->reportingClass() = m.newValue();        // update reporting class (tho in theory we already did)
            }
            else
            {
                QString prevVal = m.prevDomElem();
                currentPLUData->reportingClass() = prevVal;
                ui->leClass->setText(prevVal);
                session_->send( Error, ErrProductDevice, "101004,5");
            }

        }
    }
    // plu not on file
    else if (( ProductDev == msg[0].toInt() ) && ( ProductClearUI == msg[1].toInt() ))
    {
        initialize();
    }
}

bool superChangeProductClass::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}
