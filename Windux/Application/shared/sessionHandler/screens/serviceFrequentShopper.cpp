/**
 * @file serviceFrequentShopper.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceFrequentShopper.h"
#include "ui_serviceFrequentShopper.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"
#include <QIntValidator>

// HT-155
namespace hobart {
    class numericLineEdit;
}

serviceFrequentShopper::serviceFrequentShopper(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceFrequentShopper)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    //----------------------------------------------------------------
    // Set the connections for when one of the fields on the screen is
    // changed.  These all call the same method to set a property that
    // indicates something in the setup changed and the data needs to
    // be saved.
    //----------------------------------------------------------------
    connect(ui->cbRoundingMethod, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect(ui->cbSetupFrequentShopper, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect(ui->cbDefaultDiscountType, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect(ui->cbDefaultDiscountFormula, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect(ui->cbRoundingWeightPrecision, SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );

    connect(ui->cbDefaultDiscountType, SIGNAL( currentIndexChanged(int) ), this, SLOT( CheckDiscountFormula(int) ) );

    // HT-155
    connect(ui->chkEnableGlobalPercentOff, SIGNAL(stateChanged( int )), this, SLOT(checkboxChangedEnableGlobalDiscount( int )));
    connect(ui->txtGlobalPercentageOff, SIGNAL( textChanged( QString ) ), this, SLOT( SetupChanged() ) );

    //--------------------------------------
    // Request the Frequent Shopper settings
    //--------------------------------------
    session_->send( ConfigurationDev, CfgFrequentShopperRead );

    bSetupChanged = false;

    translate();
}

serviceFrequentShopper::~serviceFrequentShopper()
{
    delete ui;
}

void serviceFrequentShopper::translate()
{
    ui->retranslateUi(this);

    ui->cbRoundingMethod->setItemText(0, tr("Conventional rounding"));
    ui->cbRoundingMethod->setItemText(1, tr("Round up"));
    ui->cbRoundingMethod->setItemText(2, tr("Round down"));

    ui->cbSetupFrequentShopper->setItemText(0, tr("Per product"));
    ui->cbSetupFrequentShopper->setItemText(1, tr("Per scale"));

    ui->cbDefaultDiscountType->setItemText(0, tr("Discount price"));
    ui->cbDefaultDiscountType->setItemText(1, tr("Cents off"));
    ui->cbDefaultDiscountType->setItemText(2, tr("Package discount"));
    ui->cbDefaultDiscountType->setItemText(3, tr("Percent off"));
    ui->cbDefaultDiscountType->setItemText(4, tr("Advertised price"));
    ui->cbDefaultDiscountType->setItemText(5, tr("Percent added"));

    ui->cbDefaultDiscountFormula->setItemText(0, tr("IBM"));
    ui->cbDefaultDiscountFormula->setItemText(1, tr("NCR"));
    ui->cbDefaultDiscountFormula->setItemText(2, tr("RII"));
    ui->cbDefaultDiscountFormula->setItemText(3, tr("IBM-EM2"));
    ui->cbDefaultDiscountFormula->setItemText(4, tr("ICL-SASI"));
    ui->cbDefaultDiscountFormula->setItemText(5, tr("IBM-ACE"));

    update();
}

//------------------------------------------------
// Populate the dropdowns with the text and values
//------------------------------------------------
void serviceFrequentShopper::initialize()
{
    // Rounding Method
    ui->cbRoundingMethod->addItem(tr ( "Conventional rounding" ), 0 );
    ui->cbRoundingMethod->addItem(tr ( "Round up" ), 2 );
    ui->cbRoundingMethod->addItem(tr ( "Round down" ), 1 );

    // Set up Frequent Shopper
    ui->cbSetupFrequentShopper->addItem(tr ( "Per product" ), 0 );
    ui->cbSetupFrequentShopper->addItem(tr ( "Per scale" ), 1 );

    // Default Discount Type
    ui->cbDefaultDiscountType->addItem(tr ( "Discount price" ), 2 );
    ui->cbDefaultDiscountType->addItem(tr ( "Cents off" ), 1 );
    ui->cbDefaultDiscountType->addItem(tr ( "Package discount" ), 6 );
    ui->cbDefaultDiscountType->addItem(tr ( "Percent off" ), 3 );
    ui->cbDefaultDiscountType->addItem(tr ( "Advertised price" ), 4 );
    ui->cbDefaultDiscountType->addItem(tr ( "Percent added" ), 5 );

    // Default Discount Formula
    ui->cbDefaultDiscountFormula->addItem(tr ( "IBM" ), 1 );
    ui->cbDefaultDiscountFormula->addItem(tr ( "NCR" ), 4 );
    ui->cbDefaultDiscountFormula->addItem(tr ( "RII" ), 0 );
    ui->cbDefaultDiscountFormula->addItem(tr ( "IBM-EM2" ), 2 );
    ui->cbDefaultDiscountFormula->addItem(tr ( "ICL-SASI" ), 7 );
    ui->cbDefaultDiscountFormula->addItem(tr ( "IBM-ACE" ), 9 );

    // Rounding Weight Precision
    ui->cbRoundingWeightPrecision->addItem("2","2");
    ui->cbRoundingWeightPrecision->addItem("3","3");

    // HT-155 - Set validation fomat for Global Percentage Off:  99.9999
    ui->txtGlobalPercentageOff->setValidator( txtGlobalPercentageOffValidator_ = new QIntValidator( 0, INT_MAX, this ) );
    ui->txtGlobalPercentageOff->setDecimalPlaces( 4 );
    ui->txtGlobalPercentageOff->setInputWrapping( true );
    txtGlobalPercentageOffValidator_->setTop( makeValidatorTop( 6 ) );
}

bool serviceFrequentShopper::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void serviceFrequentShopper::onSessionMessage( const SessionMessage& msg )
{
    // Frequent Shopper Settings
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgFrequentShopperWrite == msg[1].toInt() ))
    {
        MadpFrequentShopper m( msg );

        //-------------------------------------------
        // Select the correct option in the dropdowns
        //-------------------------------------------

        // HT-155
        ui->chkEnableGlobalPercentOff->setChecked((m.enableGlobalPercentOff().toInt() == 1 ? true : false));

        ui->cbRoundingMethod->setCurrentIndex(ui->cbRoundingMethod->findData(m.roundingMethod()));

        ui->cbSetupFrequentShopper->setCurrentIndex(ui->cbSetupFrequentShopper->findData(m.perProductOrScale()));

        ui->cbDefaultDiscountType->setCurrentIndex(ui->cbDefaultDiscountType->findData(m.discountType()));

        ui->cbDefaultDiscountFormula->setCurrentIndex(ui->cbDefaultDiscountFormula->findData(m.discountFormula()));

        ui->cbRoundingWeightPrecision->setCurrentIndex(ui->cbRoundingWeightPrecision->findData(m.roundingWeightPrecision()));

        // HT-155
        QString tempGlobalPercentoff = m.globalPercentageOff();
        tempGlobalPercentoff.insert((tempGlobalPercentoff.length() - 4), ".");
        ui->txtGlobalPercentageOff->setText( tempGlobalPercentoff );

        // Enable/Disable fields according to the Global Discount checkbox
        checkboxChangedEnableGlobalDiscount( 1 );


        // Save values from initial data request - to be used when the screen is saved
        totalPriceText1 = m.totalPriceText1();
        totalPriceText2 = m.totalPriceText2();
        savingsText1 = m.savingsText1();
        savingsText2 = m.savingsText2();
        unitPriceText1 = m.unitPriceText1();
        unitPriceText2 = m.unitPriceText2();

        bSetupChanged = false;

        // HT-213
        setRoundingWeightPrecision();
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void serviceFrequentShopper::SetupChanged()
{
    bSetupChanged = true;
}

//--------------------------------------------
// Enable/Disable the Default Discount Formula
// based on the Default Discount Type.
//--------------------------------------------
void serviceFrequentShopper::CheckDiscountFormula(int index)
{
    Q_UNUSED( index );

    QString discountType = ui->cbDefaultDiscountType->itemData((ui->cbDefaultDiscountType->currentIndex())).toString();

    if((discountType == "3") || // Percent Off
       (discountType == "4") || // Advertised Price
       (discountType == "5") || // Percent Added
       (discountType == "6"))   // Package Discount
    {
        ui->cbDefaultDiscountFormula->setDisabled(true);
    }
    else
    {
        ui->cbDefaultDiscountFormula->setDisabled(false);
    }
}


// HT-155
void serviceFrequentShopper::checkboxChangedEnableGlobalDiscount( int state )
{
    Q_UNUSED( state );

    bSetupChanged = true;

    if( ui->chkEnableGlobalPercentOff->isChecked() )
    {
        // Set to "Per Scale"
        ui->cbSetupFrequentShopper->setCurrentIndex( 1 );
        // Set to "Percent Off"
        ui->cbDefaultDiscountType->setCurrentIndex( 3 );

        ui->cbRoundingWeightPrecision->setDisabled( true );
        ui->cbSetupFrequentShopper->setDisabled( true );
        ui->cbDefaultDiscountType->setDisabled( true );
        ui->cbDefaultDiscountFormula->setDisabled( true );

        ui->txtGlobalPercentageOff->setDisabled( false );
    }
    else
    {
        ui->cbRoundingWeightPrecision->setDisabled( false );
        ui->cbSetupFrequentShopper->setDisabled( false );
        ui->cbDefaultDiscountType->setDisabled( false );
        ui->cbDefaultDiscountFormula->setDisabled( false );

        ui->txtGlobalPercentageOff->setDisabled( true );
    }
}

//---------------------------------------------------------
// HT-213 Rounding Weight Precision should only allow "2"
// and be disabled when in single-range mode and NOT metric
//---------------------------------------------------------
void serviceFrequentShopper::setRoundingWeightPrecision()
{
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;
    if( scaleConfig->isSingleRangeWeigher() && !scaleConfig->isScaleMetric() )
    {
        ui->cbRoundingWeightPrecision->setCurrentIndex( ui->cbRoundingWeightPrecision->findData( "2" ) );

        ui->cbRoundingWeightPrecision->setDisabled( true );
    }

}

// HT-155
int serviceFrequentShopper::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void serviceFrequentShopper::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = (ui->cbRoundingMethod->itemData(ui->cbRoundingMethod->currentIndex())).toString();
        param += "," + (ui->cbSetupFrequentShopper->itemData(ui->cbSetupFrequentShopper->currentIndex())).toString();
        param += "," + (ui->cbDefaultDiscountType->itemData(ui->cbDefaultDiscountType->currentIndex())).toString();
        param += "," + (ui->cbDefaultDiscountFormula->itemData(ui->cbDefaultDiscountFormula->currentIndex())).toString();

        param += ",\"" + totalPriceText1 + "\"";
        param += ",\"" + totalPriceText2 + "\"";
        param += ",\"" + savingsText1 + "\"";
        param += ",\"" + savingsText2 + "\"";
        param += ",\"" + unitPriceText1 + "\"";
        param += ",\"" + unitPriceText2 + "\"";

        param += "," + (ui->cbRoundingWeightPrecision->itemData(ui->cbRoundingWeightPrecision->currentIndex())).toString();

        // HT-155
        param += "," + QString(ui->chkEnableGlobalPercentOff->isChecked() ? "1" : "0");
        QString tempGlobalPercentoff( ui->txtGlobalPercentageOff->text() );
        tempGlobalPercentoff = tempGlobalPercentoff.replace( ".", "" );
        param += "," + tempGlobalPercentoff;

        session_->send( ConfigurationDev, CfgFrequentShopperSave, param );
    }
}
