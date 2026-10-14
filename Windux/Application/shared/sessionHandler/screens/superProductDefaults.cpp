/* @file SuperProductDefaults.cpp
 * SuperProductDefaults
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "superProductDefaults.h"
#include "ui_superProductDefaults.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/texteditor.h"

#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include "superLabelPlacementView.h"

#include <logmanager.h>
#include <numericlineedit.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QDebug>

#define ESC_CHAR 0x1B

static const int EAN_13  = 1;
static const int EAN_128 = 2;

static const int GS1_RW_GTIN = 1;
static const int GS1_FW_GTIN = 2;
static const int GS1_BC_GTIN = 3;
static const int GS1_FL_GTIN = 4;
static const int GS1_RW_AI = 5;
static const int GS1_FW_AI = 6;
static const int GS1_BC_AI = 7;
static const int GS1_FL_AI = 8;

static const int DM_QR_CODE = 1;
static const int DM_DATA_MATRIX = 2;

static const int MAX_DIGITS_EAN13 = 12;
static const int MAX_DIGITS_EAN128 = 128;       // was 17. Should it be 127 or 128? I ask bc EAN13 is 12.
static const int MAX_DIGITS_GS1_G10 = 13;
static const int MAX_DIGITS_GS1_A1 = 15;
static const int MAX_DIGITS_DATA_MATRIX = 10000;

static const int RW_PLU = 1;
static const int FW_PLU = 2;
static const int BC_PLU = 3;
static const int FL_PLU = 4;

static const int UPPER_RIGHT = 0;
static const int UPPER_LEFT = 1;
static const int NONE = 2;
static const int LEFT_OF_PRIMARY = 6;
static const int RIGHT_OF_PRIMARY = 5;

static const bool DEBUG_PLACEMENT_VIEW = false;

namespace hobart{
    class numericLineEdit;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperProductDefaults::SuperProductDefaults(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperProductDefaults),
    defaultCurrentPLU_( NULL )
{
    //------------------------------
    // Coming from Supervisor panel:
    //------------------------------
    if ( mainWindow_->isSupervisorMode() )
        defaultCurrentPLU_ = &dynamic_cast<SupervisorMode*>( mode_ )->defaultCurrentPLU_;

    //---------------------------
    // Coming from Service panel:
    //---------------------------
    if ( mainWindow_->isServiceMode() )
        defaultCurrentPLU_ = &dynamic_cast<ServiceMode*>( mode_ )->defaultCurrentPLU_;

    scaleConfigData_   = &session_->scaleConfig->scaleConfig;
    currencyData       = &session_->scaleConfig->currencySettings;
    tareEntryPrecision = 0;
    ui->setupUi(this);
    scaleScreen();

    eanConfigEditor_ = NULL;
    gs1ConfigEditor_ = NULL;

    if( mainWindow_->isSupervisorMode() )
    {
        supervisorMode_ = dynamic_cast<SupervisorMode*>( parent );

        if( supervisorMode_->cameFromLabelPlacementView() )
        {
            hasChanged_ = true;                                                                     // always set so we trigger a save
            this->ui->tabProductDefaults->setCurrentIndex(3);
            this->ui->tabSubLabelTypes->setCurrentIndex(1);

            int primaryLabelPlacement = session_->scaleConfig->primaryLabelPlacement;
            int secondyLabelPlacement = session_->scaleConfig->secondaryLabelPlacement;
            //int thirdyLabelPlacement  = defaultCurrentPLU_->thirdLabelPlacement();                  // get from PLU bc this didnt go through the Label Viewer App

            int primaryLabelRotation = session_->scaleConfig->primaryLabelRotation;
            int secondyLabelRotation = session_->scaleConfig->secondaryLabelRotation;
            //int thirdyLabelRotation  = defaultCurrentPLU_->thirdLabelRotation();                    // get from PLU bc this didnt go through the Label Viewer App

            if( primaryLabelPlacement > 2 )
                primaryLabelPlacement = 2;

            // take the vals from the Label Viewer and save them to the local DOM (productDefaults)
            defaultCurrentPLU_->primaryLabelPlacementRaw() = QString::number( primaryLabelPlacement );
            defaultCurrentPLU_->secondLabelPlacementRaw()  = QString::number( secondyLabelPlacement );

            defaultCurrentPLU_->primaryLabelRotationRaw()  = QString::number( primaryLabelRotation  );
            defaultCurrentPLU_->secondLabelRotationRaw()   = QString::number( secondyLabelRotation  );

            // Set the combo boxes to the maybe new values
            saveDataOnExit();

            supervisorMode_->setCameFromLabelPlacementView( false );
            supervisorMode_->setCameFromProductDefaults( false );
        }
    }
    else
    {
        supervisorMode_ = NULL;
    }

    initialize();

    expandTabs(ui->tabProductDefaults);

    translate();


    // HTs
    // these features are disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_FPP ) )
    {
        ui->lblProdPlanningType->setEnabled( false );
        ui->cboProductionPlanningType->setEnabled( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SuperProductDefaults::~SuperProductDefaults()
{
    if(eanConfigEditor_ != NULL)
        delete eanConfigEditor_;

    if(gs1ConfigEditor_ != NULL)
        delete gs1ConfigEditor_;

    delete eanNumberSystemValidator_;
    delete reportingClassValidator_;
    delete discountValidator1_;
    delete discountValidator2_;
    delete discountValidator3_;

    if( unitPriceValidator_ != NULL)
        delete unitPriceValidator_;
    if( byCountValidator_ != NULL)
        delete byCountValidator_;
    if( tareValidator_ != NULL)
        delete tareValidator_;
    if( proportionalTareValidator_ != NULL)
        delete proportionalTareValidator_;
    if( netWeightValidator_ != NULL)
        delete netWeightValidator_;
    if( exceptionPriceValidator_ != NULL)
        delete exceptionPriceValidator_;

    delete pointsValidator;
    delete shelfLifeDaysValidator_;
    delete shelfLifeHoursValidator_;
    delete productLifeDaysValidator_;
    delete productLifeHoursValidator_;
    delete priDefaultLabelTypeValidator_;
    delete secDefaultLabelTypeValidator_;
    delete triDefaultLabelTypeValidator_;
    delete priManualLabelTypeValidator_;
    delete secManualLabelTypeValidator_;
    delete triManualLabelTypeValidator_;
    delete priPrepackLabelTypeValidator_;
    delete secPrepackLabelTypeValidator_;
    delete triPrepackLabelTypeValidator_;
    delete priProdEntryLabelTypeValidator_;
    delete secProdEntryLabelTypeValidator_;
    delete triProdEntryLabelTypeValidator_;
    delete priSelfServLabelTypeValidator_;
    delete secSelfServLabelTypeValidator_;
    delete triSelfServLabelTypeValidator_;
    delete priPick5LabelTypeValidator_;
    delete secPick5LabelTypeValidator_;
    delete triPick5LabelTypeValidator_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::initialize()
{
    // initialize vars
    isUpdating_ = false;
    ignoreSignals_ = false;
    hasChanged_ = false;
    isScaleMetric_ = false;
    currentTab = 0;                                                 // the current tab always starts with the 0 tab
    savedProductType = "1";                                         // can set it to 1 because we request random product by default
    zeros << "" << "0" << "00" << "000" << "0000";

    ui->gbProportionalTare->move( scalePointForScreen( QPoint( 30, 300 ) ) );
    ui->gbNetWeight->move( scalePointForScreen( QPoint( 30, 230 ) ) );

    unitSeparator = currencyData->unitSeparator();                  // decimal or comma

    if( scaleConfigData_->isScaleMetric() )
        unitSymbol = "kg";
    else
        unitSymbol = "lb";

    setupValidators();
    setupSlotsAndSignals();
    setupWidgets();
    setupCountrySymbols();

    ui->cboPrimaryApplierRotation->blockSignals( true );            // dont want these firing off save message SIGNALS
    ui->cboSecondApplierRotation->blockSignals( true );
    ui->cboThirdApplierRotation->blockSignals( true );
    ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );
    ui->cboThirdLabelPlacement->blockSignals( true );

    setupWrapperAndApplierFields();

    ui->cboPrimaryLabelPlacement->setCurrentIndex( ui->cboPrimaryLabelPlacement->findData( defaultCurrentPLU_->primaryLabelPlacementRaw() ) );
    ui->cboSecondLabelPlacement->setCurrentIndex(  ui->cboSecondLabelPlacement->findData(  defaultCurrentPLU_->secondLabelPlacementRaw()  ) );
    ui->cboThirdLabelPlacement->setCurrentIndex(   ui->cboThirdLabelPlacement->findData(   defaultCurrentPLU_->thirdLabelPlacementRaw()   ) );
    ui->cboPrimaryApplierRotation->setCurrentIndex( ui->cboPrimaryApplierRotation->findData( defaultCurrentPLU_->primaryLabelRotationRaw() ) );
    ui->cboSecondApplierRotation->setCurrentIndex( ui->cboSecondApplierRotation->findData(   defaultCurrentPLU_->secondLabelRotationRaw()  ) );
    ui->cboThirdApplierRotation->setCurrentIndex(ui->cboThirdApplierRotation->findData(      defaultCurrentPLU_->thirdLabelRotation()      ) );

    ui->cboPrimaryApplierRotation->blockSignals( false );            // dont want these firing off save message SIGNALS
    ui->cboSecondApplierRotation->blockSignals( false );
    ui->cboThirdApplierRotation->blockSignals( false );
    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );
    ui->cboThirdLabelPlacement->blockSignals( false );

    ui->txtDescription->installEventFilter(this);
    update();

    // HT-136: Change label if using 2nd Tare (lblTare is no longer marked as translatable)
    QString lblTare( scaleConfigData_->useSecondTare() ? tr("Tare") + " 2"  : tr("Tare") );
    ui->lblTare->setText( lblTare );

    // Now determine if we are a prepack or not so we can hide some fields
    if( !session_->isPrepack() )
    {
        ui->wgtWrapper->hide();
        ui->wgtIntake->hide();
    }
    else
    {
        session_->send( LabelApplier, LaReqInquire );
    }


    QString modelName = session_->scaleConfig->getScaleModelName();
    if( modelName.contains( "NGW", Qt::CaseInsensitive ) )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            ui->butPlacementView->setEnabled( true );
            ui->butPlacementView->setVisible( true );
        }
        else
        {
            ui->butPlacementView->setEnabled( false );
            ui->butPlacementView->setVisible( false );
        }
    }

    // send initial messaging
    session_->send(ProductDev, ProductReadDefault, "0,1");          // by default, if coming in here the first time, request random weight product type
    session_->send(ConfigurationDev, CfgScaleOperationRead);        // 112,70 to get max tare message returned
    session_->send(GraphicDev, GraphicReadAllNames);                // 105,20 request label graphic names
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperProductDefaults::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::translate()
{
    ui->retranslateUi( this );

    ui->cboDiscountType->setItemText(0, tr("None"));
    ui->cboDiscountType->setItemText(1, tr("Cents off"));
    ui->cboDiscountType->setItemText(2, tr("Discount price"));
    ui->cboDiscountType->setItemText(3, tr("Percent off"));
    ui->cboDiscountType->setItemText(4, tr("Advertised price"));
    ui->cboDiscountType->setItemText(5, tr("Percent added"));
    ui->cboDiscountType->setItemText(6, tr("Package discount"));

    ui->cboProductType->setItemText(0, tr("Random Weight"));

    if(scaleConfigData_->scaleModel().toInt() > 1)
        ui->cboProductType->setItemText(1, tr("Fixed weight"));

    ui->cboProductType->setItemText(2, tr("By count"));

    if(scaleConfigData_->scaleModel().toInt() > 1 && !scaleConfigData_->isScaleMetric())
        ui->cboProductType->setItemText(3, tr("Fluid ounces"));

    // HTA-122
    ui->cboPrimaryLabelPlacement->setItemText(0, tr("System default"));
    ui->cboPrimaryLabelPlacement->setItemText(1, tr("Upper Right"));
    ui->cboPrimaryLabelPlacement->setItemText(2, tr("Upper Left"));

    int primaryIndex = defaultCurrentPLU_->primaryLabelPlacement();
    switch( primaryIndex )
    {
        case upperRight:
        {
            ui->cboSecondLabelPlacement->setItemText(0, tr("System default"));
            ui->cboSecondLabelPlacement->setItemText(1, tr("Upper Left"));
            ui->cboSecondLabelPlacement->setItemText(2, tr("Lower Right"));
            ui->cboSecondLabelPlacement->setItemText(3, tr("Lower Left"));
            ui->cboSecondLabelPlacement->setItemText(4, tr("Left of Primary"));
            ui->cboSecondLabelPlacement->setItemText(5, tr("Below Primary"));
            break;
        }
        case upperLeft:
        {
            ui->cboSecondLabelPlacement->setItemText(0, tr("System default"));
            ui->cboSecondLabelPlacement->setItemText(1, tr("Upper Right"));
            ui->cboSecondLabelPlacement->setItemText(2, tr("Lower Right"));
            ui->cboSecondLabelPlacement->setItemText(3, tr("Lower Left"));
            ui->cboSecondLabelPlacement->setItemText(4, tr("Right of Primary"));
            ui->cboSecondLabelPlacement->setItemText(5, tr("Below Primary"));
            break;
        }
        default:
        {
            ui->cboSecondLabelPlacement->setItemText(0, tr("System default"));
            ui->cboSecondLabelPlacement->setItemText(1, tr("Upper Right"));
            ui->cboSecondLabelPlacement->setItemText(2, tr("Upper Left"));
            ui->cboSecondLabelPlacement->setItemText(3, tr("Lower Right"));
            ui->cboSecondLabelPlacement->setItemText(4, tr("Lower Left"));
            ui->cboSecondLabelPlacement->setItemText(5, tr("Right of Primary"));
            ui->cboSecondLabelPlacement->setItemText(6, tr("Left of Primary"));
            ui->cboSecondLabelPlacement->setItemText(7, tr("Below Primary"));
            break;
        }
    }

    ui->cboThirdLabelPlacement->setItemText(0, tr("System default"));
    ui->cboThirdLabelPlacement->setItemText(1, tr("Right side"));
    ui->cboThirdLabelPlacement->setItemText(2, tr("Left side"));

    ui->cboIntakeMode->setItemText(0, tr("System default"));
    ui->cboIntakeMode->setItemText(1, tr("Gentle"));
    ui->cboIntakeMode->setItemText(2, tr("Normal"));

    // FT-695
    if( eanConfigEditor_ )
        eanConfigEditor_->translate();

    // FT-695
    if( gs1ConfigEditor_ )
        gs1ConfigEditor_->translate();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperProductDefaults::onScreenChanging()
{
    currentChangeTab( -1 );                                         // save current tab, pass a -1 will save current no matter what

    int test_priLabLoc =  ui->cboPrimaryLabelPlacement->currentData().toInt();
    int test_secLabLoc =  ui->cboSecondLabelPlacement->currentData().toInt();

    defaultCurrentPLU_->primaryLabelPlacementRaw() = QString::number(test_priLabLoc);
    defaultCurrentPLU_->secondLabelPlacementRaw()  = QString::number(test_secLabLoc);

    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::setupSlotsAndSignals()
{
    // set up signals and slots

    // TAB 1 - Main
    connect(ui->cboProductType,         SIGNAL(currentIndexChanged(int)),       this,   SLOT(requestNewProductType(int)));
    connect(ui->cboUPCType,             SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));
    connect(ui->txtEANNumberSystem,     SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->butEanFormatString,     SIGNAL(clicked()),                      this,   SLOT(launchEanEditor()) );
    connect(ui->butEan128FormatString,  SIGNAL(clicked()),                      this,   SLOT(launchEan128Editor()));
    connect(ui->txtReportingClass,      SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    // connect(description)

    // TAB 2 - Pricing
    connect(ui->cboDiscountType,        SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));
    connect(ui->txtDiscount1,           SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtDiscount2,           SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtDiscount3,           SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtUnitPrice,           SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->chkForcedUnitPrice,     SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChangedForcedUnitPrice(int)));
    connect(ui->txtByCount,             SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->chkForcedByCount,       SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChangedForcedByCount(int)));
    connect(ui->txtTare,                SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->chkForcedTare,          SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChangedForcedTare(int)));
    connect(ui->txtProportionalTare,    SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtNetWeight,           SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtExceptionPrice,      SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));

    // TAB 3 - Product Life
    connect(ui->txtShelfLifeDays,       SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtShelfLifeHours,      SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtProductLifeDays,     SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtProductLifeHours,    SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->txtPoints,              SIGNAL(editingFinished()),              this,   SLOT(textboxChanged()));
    connect(ui->cboProductionPlanningType, SIGNAL(currentIndexChanged(int)),    this,   SLOT(comboboxChanged(int)));
    connect(ui->chkDontPrintPackedOn,   SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChanged(int)));
    connect(ui->chkDontPrintSellBy,     SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChanged(int)));
    connect(ui->chkDontPrintBestBefore, SIGNAL(stateChanged(int)),              this,   SLOT(checkboxChanged(int)));


    // TAB 4 - Labeling
    connect(ui->txtDefaultPriLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtDefaultSecLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtDefaultTriLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtManualPriLabelType,    SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtManualSecLabelType,    SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtManualTriLabelType,    SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtPrepackPriLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtPrepackSecLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtPrepackTriLabelType,   SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtProdEntryPriLabelType, SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtProdEntrySecLabelType, SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtProdEntryTriLabelType, SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtSelfServPriLabelType,  SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtSelfServSecLabelType,  SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtSelfServTriLabelType,  SIGNAL(editingFinished()),            this,   SLOT(textboxChanged()));
    connect(ui->txtPick5PriLabelType,  SIGNAL(editingFinished()),               this,   SLOT(textboxChanged()));
    connect(ui->txtPick5SecLabelType,  SIGNAL(editingFinished()),               this,   SLOT(textboxChanged()));
    connect(ui->txtPick5TriLabelType,  SIGNAL(editingFinished()),               this,   SLOT(textboxChanged()));

    connect(ui->cboGraphic1,            SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));
    connect(ui->cboGraphic2,            SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));
    connect(ui->cboGraphic3,            SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));
    connect(ui->cboGraphic4,            SIGNAL(currentIndexChanged(int)),       this,   SLOT(comboboxChanged(int)));

    // TAB 4 - Labeling - Prepack
    connect(ui->cboPrimaryApplierRotation, SIGNAL(currentIndexChanged(int)),    this,   SLOT(onPrimaryRotationComboChanged(int)));
    connect(ui->cboSecondApplierRotation,  SIGNAL(currentIndexChanged(int)),    this,   SLOT(onSecondaryRotationComboChanged(int)));
    connect(ui->cboThirdApplierRotation,   SIGNAL(currentIndexChanged(int)),    this,   SLOT(onThirdRotationComboChanged(int)));
    connect(ui->cboPrimaryLabelPlacement,  SIGNAL(currentIndexChanged(int)),    this,   SLOT(onPrimaryPlacementComboChanged(int)));
    connect(ui->cboSecondLabelPlacement,   SIGNAL(currentIndexChanged(int)),    this,   SLOT(onSecondaryPlacementComboChanged(int)));
    connect(ui->cboThirdLabelPlacement,    SIGNAL(currentIndexChanged(int)),    this,   SLOT(onThirdPlacementComboChanged(int)));
    connect(ui->cboIntakeMode,             SIGNAL(currentIndexChanged(int)),    this,   SLOT(comboboxChanged(int)));

    // TAB 5 - GS1 Databar
    connect(ui->butRW_GTIN,             SIGNAL(clicked()),                      this,   SLOT(launchRwGTIN()) );
    connect(ui->butFW_GTIN,             SIGNAL(clicked()),                      this,   SLOT(launchFwGTIN()) );
    connect(ui->butBC_GTIN,             SIGNAL(clicked()),                      this,   SLOT(launchBcGTIN()) );
    connect(ui->butFL_GTIN,             SIGNAL(clicked()),                      this,   SLOT(launchFlGTIN()) );

    connect(ui->butRW_AI,               SIGNAL(clicked()),                      this,   SLOT(launchRwAI()) );
    connect(ui->butFW_AI,               SIGNAL(clicked()),                      this,   SLOT(launchFwAI()) );
    connect(ui->butBC_AI,               SIGNAL(clicked()),                      this,   SLOT(launchBcAI()) );
    connect(ui->butFL_AI,               SIGNAL(clicked()),                      this,   SLOT(launchFlAI()) );

    // launch the QR Code Editor
    connect(ui->butRW_QR,               SIGNAL(clicked()),                      this,   SLOT(launchQRCodeEditor()) );
    connect(ui->butFW_QR,               SIGNAL(clicked()),                      this,   SLOT(launchQRCodeEditor()) );
    connect(ui->butBC_QR,               SIGNAL(clicked()),                      this,   SLOT(launchQRCodeEditor()) );
    connect(ui->butFL_QR,               SIGNAL(clicked()),                      this,   SLOT(launchQRCodeEditor()) );

    // launch the Data Matrix Editor
    connect(ui->butRW_DM,               SIGNAL(clicked()),                      this,   SLOT(launchDataMatrixEditor()) );
    connect(ui->butFW_DM,               SIGNAL(clicked()),                      this,   SLOT(launchDataMatrixEditor()) );
    connect(ui->butBC_DM,               SIGNAL(clicked()),                      this,   SLOT(launchDataMatrixEditor()) );
    connect(ui->butFL_DM,               SIGNAL(clicked()),                      this,   SLOT(launchDataMatrixEditor()) );

    // Click any Tab
    connect(ui->tabProductDefaults,     SIGNAL(currentChanged(int)),            this, SLOT(currentChangeTab(int)));

    connect(ui->butPlacementView, SIGNAL(clicked()), this, SLOT(onPlacementButtonClicked()));
}

void SuperProductDefaults::setupWidgets()
{
    // Really all we are doing is updating and initializing checkboxes
    ignoreSignals_ = true;

    // tab 0
    ui->cboProductType->addItem(tr("Random Weight"), "1");

    if(scaleConfigData_->scaleModel().toInt() > 1)
        ui->cboProductType->addItem(tr("Fixed weight"), "2");

    ui->cboProductType->addItem(tr("By count"), "3");

    if(scaleConfigData_->scaleModel().toInt() > 1 && !scaleConfigData_->isScaleMetric())
        ui->cboProductType->addItem(tr("Fluid ounces"), "4");

    // tab 1
    ui->cboDiscountType->addItem(tr("None"));
    ui->cboDiscountType->addItem(tr("Cents off"));
    ui->cboDiscountType->addItem(tr("Discount price"));
    ui->cboDiscountType->addItem(tr("Percent off"));
    ui->cboDiscountType->addItem(tr("Advertised price"));
    ui->cboDiscountType->addItem(tr("Percent added"));
    ui->cboDiscountType->addItem(tr("Package discount"));

    // tab 2
    ui->cboProductionPlanningType->addItem("0", "0");
    ui->cboProductionPlanningType->addItem("1", "1");
    ui->cboProductionPlanningType->addItem("2", "2");
    ui->cboProductionPlanningType->addItem("3", "3");
    ui->cboProductionPlanningType->addItem("4", "4");

    // random weight is the default for all barcode strings
    ///////////////////////////////////////////////////////
    ui->lblFW_GTIN->setVisible( false );
    ui->lblBC_GTIN->setVisible( false );
    ui->lblFL_GTIN->setVisible( false );

    ui->lblFW_AI->setVisible( false );
    ui->lblBC_AI->setVisible( false );
    ui->lblFL_AI->setVisible( false );

    ui->lblFW_QR->setVisible( false );
    ui->lblBC_QR->setVisible( false );
    ui->lblFL_QR->setVisible( false );

    ui->lblFW_DM->setVisible( false );
    ui->lblBC_DM->setVisible( false );
    ui->lblFL_DM->setVisible( false );
    ///////////////////////////////////////////////////////

    ignoreSignals_ = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::setupCountrySymbols()
{
    QString currencySymbol = currencyData->priCurrSymbol();

    ui->lblDisc1CurrSymbol->setText( currencySymbol );              // set up the currency symbols
    ui->lblDisc2CurrSymbol->setText( currencySymbol );
    ui->lblDisc3CurrSymbol->setText( currencySymbol );
    ui->lblUnitPriceCurrSymbol->setText( currencySymbol );
    ui->lblExceptionPriceCurrSymbol->setText( currencySymbol );

    ui->lblUnitPriceUnitSymbol->setText( unitSymbol );              // set up weight symbols kg or lb
    ui->lblTareUnitSymbol->setText( unitSymbol );
    ui->lblNetWeightUnitSymbol->setText( unitSymbol );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::setupWrapperAndApplierFields()
{
    ignoreSignals_ = true;

    //    Applier rotations
    ui->cboPrimaryApplierRotation->clear();
    ui->cboPrimaryApplierRotation->addItem(tr("0"), "0");
    ui->cboPrimaryApplierRotation->addItem(tr("45"), "45");
    ui->cboPrimaryApplierRotation->addItem(tr("90"), "90");
    ui->cboPrimaryApplierRotation->addItem(tr("135"), "135");
    ui->cboPrimaryApplierRotation->addItem(tr("180"), "180");
    ui->cboPrimaryApplierRotation->addItem(tr("225"), "225");
    ui->cboPrimaryApplierRotation->addItem(tr("270"), "270");
    ui->cboPrimaryApplierRotation->addItem(tr("315"), "315");

    ui->cboSecondApplierRotation->clear();
    ui->cboSecondApplierRotation->addItem(tr("0"), "0");
    ui->cboSecondApplierRotation->addItem(tr("45"), "45");
    ui->cboSecondApplierRotation->addItem(tr("90"), "90");
    ui->cboSecondApplierRotation->addItem(tr("135"), "135");
    ui->cboSecondApplierRotation->addItem(tr("180"), "180");
    ui->cboSecondApplierRotation->addItem(tr("225"), "225");
    ui->cboSecondApplierRotation->addItem(tr("270"), "270");
    ui->cboSecondApplierRotation->addItem(tr("315"), "315");

    ui->cboThirdApplierRotation->clear();
    ui->cboThirdApplierRotation->addItem(tr("0"), "0");
    ui->cboThirdApplierRotation->addItem(tr("45"), "45");
    ui->cboThirdApplierRotation->addItem(tr("90"), "90");
    ui->cboThirdApplierRotation->addItem(tr("135"), "135");
    ui->cboThirdApplierRotation->addItem(tr("180"), "180");
    ui->cboThirdApplierRotation->addItem(tr("225"), "225");
    ui->cboThirdApplierRotation->addItem(tr("270"), "270");
    ui->cboThirdApplierRotation->addItem(tr("315"), "315");

    //    Label placement
    ui->cboPrimaryLabelPlacement->clear();
    ui->cboPrimaryLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
    ui->cboPrimaryLabelPlacement->addItem(tr("Upper Right"), "0");
    ui->cboPrimaryLabelPlacement->addItem(tr("Upper Left"), "1");

    int primaryIndex = defaultCurrentPLU_->primaryLabelPlacementRaw().toInt();
    switch( primaryIndex )
    {
        case upperRight:
        {
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");
            ui->cboSecondLabelPlacement->addItem(tr("Upper Left"), "1");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Left of Primary"), "6");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
            break;
        }
        case upperLeft:
        {
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");
            ui->cboSecondLabelPlacement->addItem(tr("Upper Right"), "0");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Right of Primary"), "5");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
            break;
        }
        default:
            // Probably set to None(2)
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
            ui->cboSecondLabelPlacement->addItem(tr("Upper Right"), "0");
            ui->cboSecondLabelPlacement->addItem(tr("Upper Left"), "1");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Right of Primary"), "5");
            ui->cboSecondLabelPlacement->addItem(tr("Left of Primary"), "6");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
        break;

    }

    ui->cboThirdLabelPlacement->clear();
    ui->cboThirdLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
    ui->cboThirdLabelPlacement->addItem(tr("Right side"), "0");
    ui->cboThirdLabelPlacement->addItem(tr("Left side"), "1");

    //    Intake mode
    ui->cboIntakeMode->clear();
    ui->cboIntakeMode->addItem(tr("System default"), "0");
    ui->cboIntakeMode->addItem(tr("Gentle"), "1");
    ui->cboIntakeMode->addItem(tr("Normal"), "2");

    ignoreSignals_ = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::setupVisibility(int pluType)
{
    if( 1 == pluType)                                               // RW
    {
        ui->gbByCount->hide();
        ui->gbNetWeight->hide();
        ui->gbExceptionPrice->hide();
        ui->gbTare->show();
        int myModel = scaleConfigData_->scaleModel().toInt();
        if( myModel > 2 )
            ui->gbProportionalTare->show();
        else
            ui->gbProportionalTare->hide();
    }
    else if( 3 == pluType )                                         // BC
    {
        ui->gbProportionalTare->hide();
        ui->gbNetWeight->hide();
        ui->gbByCount->show();
        ui->gbExceptionPrice->show();
        ui->gbTare->show();
    }
    else                                                            // FW FL
    {
        ui->gbProportionalTare->hide();
        ui->gbTare->hide();
        ui->gbExceptionPrice->hide();
        ui->gbByCount->show();
        ui->gbNetWeight->show();

        if( pluType == 2)
            ui->lblNetWeight->setText(tr("Net Weight"));
        else
            ui->lblNetWeight->setText(tr("Fluid Weight"));
    }
}

bool SuperProductDefaults::eventFilter( QObject *obj, QEvent *event )
{
    //saveEntireProduct();
    QString msg;

    if(ui->txtDescription == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            // go to the editor directly
            if ( mainWindow_->isSupervisorMode() || mainWindow_->isServiceMode())
            {
                QString oldDescription = defaultCurrentPLU_->description();
                QString initialContents = defaultCurrentPLU_->description();
                textEditor textEditorDialog(mainWindow_, "Description", true, initialContents);
                if ( QDialog::Accepted == textEditorDialog.exec() )
                {
                    defaultCurrentPLU_->description() = textEditorDialog.getEncodedEditorContents();
                    oldDescription != defaultCurrentPLU_->description() ? hasChanged_ = true : hasChanged_ = false;
                    ui->txtDescription->setText(textEditorDialog.getPlainTextEditorContents());
                    //qDebug() << "enocded is: " << encodedDescription;
                    //qDebug() << "plainText is: " << plainTextDescription;
                    //ui
                }
            }
        }

    }
    return _Mybase::eventFilter( obj, event );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::setupValidators()
{
    // NUMERIC VALIDATORS

    // ean code
    ui->txtEANNumberSystem->setValidator(eanNumberSystemValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtEANNumberSystem->setDecimalPlaces(0);
    ui->txtEANNumberSystem->setInputWrapping(true);
    eanNumberSystemValidator_->setTop(makeValidatorTop(2));

    // reporting class
    ui->txtReportingClass->setValidator(reportingClassValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtReportingClass->setDecimalPlaces(0);
    ui->txtReportingClass->setInputWrapping(true);
    reportingClassValidator_->setTop(makeValidatorTop(6));

    // discount 1
    ui->txtDiscount1->setValidator(discountValidator1_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtDiscount1->setDecimal(currencyData->unitSeparator());
    ui->txtDiscount1->setDecimalPlaces(currencyData->primaryPrecision());
    ui->txtDiscount1->setInputWrapping(true);
    discountValidator1_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    // discount 2
    ui->txtDiscount2->setValidator(discountValidator2_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtDiscount2->setDecimal(currencyData->unitSeparator());
    ui->txtDiscount2->setDecimalPlaces(currencyData->primaryPrecision());
    ui->txtDiscount2->setInputWrapping(true);
    discountValidator2_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    // discount 3
    ui->txtDiscount3->setValidator(discountValidator3_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtDiscount3->setDecimal(currencyData->unitSeparator());
    ui->txtDiscount3->setDecimalPlaces(currencyData->primaryPrecision());
    ui->txtDiscount3->setInputWrapping(true);
    discountValidator3_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    // unit price
    ui->txtUnitPrice->setValidator(unitPriceValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtUnitPrice->setDecimalPlaces( currencyData->primaryPrecision() );
    ui->txtUnitPrice->setDecimal( currencyData->unitSeparator() );
    ui->txtUnitPrice->setInputWrapping(true);
    unitPriceValidator_->setTop( makeValidatorTop(currencyData->unitPriceDigits()) );

    // by count
    ui->txtByCount->setValidator(byCountValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtByCount->setDecimalPlaces( 0 );
    ui->txtByCount->setInputWrapping(true);
    byCountValidator_->setTop( makeValidatorTop(2) );

    // Tare
    int tmpPrec = scaleConfigData_->tarePrecision();
    ui->txtTare->setValidator(tareValidator_ = new QIntValidator(0, INT_MAX, this) );
    if( 2 == tmpPrec )
        ui->txtTare->setDecimalPlaces( tmpPrec );
    else
        ui->txtTare->setDecimalPlaces( tmpPrec );

    ui->txtTare->setDecimal( currencyData->unitSeparator() );
    ui->txtTare->setInputWrapping(true);
    //Tare top is set from the incoming normal scale operation message in onSessionMessage() below

    // Proportional Tare
    ui->txtProportionalTare->setValidator(proportionalTareValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtProportionalTare->setDecimalPlaces( 1 );
    ui->txtProportionalTare->setDecimal( currencyData->unitSeparator() );
    ui->txtProportionalTare->setInputWrapping(true);
    proportionalTareValidator_->setTop( makeValidatorTop(3) );

    // Net Weight
    ui->txtNetWeight->setValidator(netWeightValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtNetWeight->setDecimalPlaces(0);
    ui->txtNetWeight->setInputWrapping(true);
    netWeightValidator_->setTop(makeValidatorTop(4));

    // Exception Price
    ui->txtExceptionPrice->setValidator(exceptionPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtExceptionPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->txtExceptionPrice->setDecimal(currencyData->unitSeparator());
    ui->txtExceptionPrice->setInputWrapping(true);
    exceptionPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    // points
    ui->txtPoints->setValidator(pointsValidator = new QIntValidator(0, INT_MAX, this));
    ui->txtPoints->setDecimalPlaces(0);
    ui->txtPoints->setInputWrapping(true);
    pointsValidator->setTop(makeValidatorTop(4));

    // shelf life days
    ui->txtShelfLifeDays->setValidator(shelfLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtShelfLifeDays->setDecimalPlaces(0);
    ui->txtShelfLifeDays->setInputWrapping(true);
    shelfLifeDaysValidator_->setTop(makeValidatorTop(3));

    // shelf life hours
    ui->txtShelfLifeHours->setValidator(shelfLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtShelfLifeHours->setDecimalPlaces(0);
    ui->txtShelfLifeHours->setInputWrapping(true);
    shelfLifeHoursValidator_->setTop( 23 );

    // product life days
    ui->txtProductLifeDays->setValidator(productLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProductLifeDays->setDecimalPlaces(0);
    ui->txtProductLifeDays->setInputWrapping(true);
    productLifeDaysValidator_->setTop( makeValidatorTop(3) );

    // product life hours
    ui->txtProductLifeHours->setValidator(productLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProductLifeHours->setDecimalPlaces(0);
    ui->txtProductLifeHours->setInputWrapping(true);
    productLifeHoursValidator_->setTop( 23 );

    // primary label type
    ui->txtDefaultPriLabelType->setValidator(priDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultPriLabelType->setDecimalPlaces(0);
    ui->txtDefaultPriLabelType->setInputWrapping(true);
    priDefaultLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second label type
    ui->txtDefaultSecLabelType->setValidator(secDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultSecLabelType->setDecimalPlaces(0);
    ui->txtDefaultSecLabelType->setInputWrapping(true);
    secDefaultLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third label type
    ui->txtDefaultTriLabelType->setValidator(triDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultTriLabelType->setDecimalPlaces(0);
    ui->txtDefaultTriLabelType->setInputWrapping(true);
    triDefaultLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // primary manual label type
    ui->txtManualPriLabelType->setValidator(priManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualPriLabelType->setDecimalPlaces(0);
    ui->txtManualPriLabelType->setInputWrapping(true);
    priManualLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second manual label type
    ui->txtManualSecLabelType->setValidator(secManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualSecLabelType->setDecimalPlaces(0);
    ui->txtManualSecLabelType->setInputWrapping(true);
    secManualLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third manual label type
    ui->txtManualTriLabelType->setValidator(triManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualTriLabelType->setDecimalPlaces(0);
    ui->txtManualTriLabelType->setInputWrapping(true);
    triManualLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // primary prepack label type
    ui->txtPrepackPriLabelType->setValidator(priPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackPriLabelType->setDecimalPlaces(0);
    ui->txtPrepackPriLabelType->setInputWrapping(true);
    priPrepackLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second prepack label type
    ui->txtPrepackSecLabelType->setValidator(secPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackSecLabelType->setDecimalPlaces(0);
    ui->txtPrepackSecLabelType->setInputWrapping(true);
    secPrepackLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third prepack label type
    ui->txtPrepackTriLabelType->setValidator(triPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackTriLabelType->setDecimalPlaces(0);
    ui->txtPrepackTriLabelType->setInputWrapping(true);
    triPrepackLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // primary prod entry label type
    ui->txtProdEntryPriLabelType->setValidator(priProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntryPriLabelType->setDecimalPlaces(0);
    ui->txtProdEntryPriLabelType->setInputWrapping(true);
    priProdEntryLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second prod entry label type
    ui->txtProdEntrySecLabelType->setValidator(secProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntrySecLabelType->setDecimalPlaces(0);
    ui->txtProdEntrySecLabelType->setInputWrapping(true);
    secProdEntryLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third prod entry label type
    ui->txtProdEntryTriLabelType->setValidator(triProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntryTriLabelType->setDecimalPlaces(0);
    ui->txtProdEntryTriLabelType->setInputWrapping(true);
    triProdEntryLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // primary self serv label type
    ui->txtSelfServPriLabelType->setValidator(priSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServPriLabelType->setDecimalPlaces(0);
    ui->txtSelfServPriLabelType->setInputWrapping(true);
    priSelfServLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second self serv label type
    ui->txtSelfServSecLabelType->setValidator(secSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServSecLabelType->setDecimalPlaces(0);
    ui->txtSelfServSecLabelType->setInputWrapping(true);
    secSelfServLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third self serv label type
    ui->txtSelfServTriLabelType->setValidator(triSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServTriLabelType->setDecimalPlaces(0);
    ui->txtSelfServTriLabelType->setInputWrapping(true);
    triSelfServLabelTypeValidator_->setTop( makeValidatorTop(4) );

    // primary pick5 label type
    ui->txtPick5PriLabelType->setValidator(priPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5PriLabelType->setDecimalPlaces(0);
    ui->txtPick5PriLabelType->setInputWrapping(true);
    priPick5LabelTypeValidator_->setTop( makeValidatorTop(4) );

    // second pick5 label type
    ui->txtPick5SecLabelType->setValidator(secPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5SecLabelType->setDecimalPlaces(0);
    ui->txtPick5SecLabelType->setInputWrapping(true);
    secPick5LabelTypeValidator_->setTop( makeValidatorTop(4) );

    // Third pick5 label type
    ui->txtPick5TriLabelType->setValidator(triPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5TriLabelType->setDecimalPlaces(0);
    ui->txtPick5TriLabelType->setInputWrapping(true);
    triPick5LabelTypeValidator_->setTop( makeValidatorTop(4) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperProductDefaults::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::repopulateUPCTypeCombo(int productTypeIndex)
{
    ignoreSignals_ = true;

    ui->cboUPCType->clear();

    if(productTypeIndex == 1 || productTypeIndex == 3)
    {
        ui->cboUPCType->addItem("2", "2");
        ui->cboUPCType->addItem("4", "4");
    }
    else
    {
        ui->cboUPCType->addItem("0", "0");
        ui->cboUPCType->addItem("1", "1");
        ui->cboUPCType->addItem("2", "2");
        ui->cboUPCType->addItem("4", "4");
        ui->cboUPCType->addItem("6", "6");
        ui->cboUPCType->addItem("7", "7");
        ui->cboUPCType->addItem("8", "8");
        ui->cboUPCType->addItem("9", "9");
    }

    ignoreSignals_ = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::saveDataOnExit()
{
    // write out the DOM data that has already been saved, but have to save the current tab first.

    if( hasChanged_ )
    {
        QString msg = defaultCurrentPLU_->itemNumber() + ",";
        msg.append(savedProductType).append(",");
        msg.append("\"").append(defaultCurrentPLU_->description()).append("\",");
        msg.append(defaultCurrentPLU_->barcodeTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->barcodeFieldsRaw()).append(",");
        msg.append(defaultCurrentPLU_->numberSystem()).append(",");
        msg.append(",");    //  productNumber
        msg.append(",");    //  vendorNumber
        msg.append(",");    //  eanCode
        msg.append(defaultCurrentPLU_->byCountRaw()).append(",");
        msg.append("\"").append(defaultCurrentPLU_->exceptionPrice().replace(unitSeparator, ".")).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->unitPrice().replace(unitSeparator, ".")).append("\",");
        msg.append(defaultCurrentPLU_->discountTypeRaw()).append(",");
        msg.append("\"").append(defaultCurrentPLU_->discountValue1().replace(unitSeparator, ".")).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->discountValue2().replace(unitSeparator, ".")).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->discountValue3().replace(unitSeparator, ".")).append("\",");
        msg.append(defaultCurrentPLU_->discountLabel1()).append(",");
        msg.append(defaultCurrentPLU_->discountLabel2()).append(",");
        msg.append(defaultCurrentPLU_->discountLabel3()).append(",");
        msg.append("\"").append(defaultCurrentPLU_->tare().replace(unitSeparator, ".")).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->proportionalTare().replace(unitSeparator, ".")).append("\",");
        msg.append(defaultCurrentPLU_->fixedWeight()).append(",");
        msg.append(defaultCurrentPLU_->shelfLifeDaysRaw()).append(",");
        msg.append(defaultCurrentPLU_->shelfLifeHoursRaw()).append(",");
        msg.append(defaultCurrentPLU_->productLifeDaysRaw()).append(",");
        msg.append(defaultCurrentPLU_->productLifeHoursRaw()).append(",");
        msg.append(defaultCurrentPLU_->priceModifier()).append(",");
        msg.append(defaultCurrentPLU_->reportingClass()).append(",");
        msg.append(",");        // dont use this anymore
        msg.append(defaultCurrentPLU_->primaryLabelRotationRaw()).append(",");
        msg.append(",");        // dont use this anymore
        msg.append(defaultCurrentPLU_->secondLabelRotationRaw()).append(",");
        msg.append(",");        // dont use this anymore
        msg.append(defaultCurrentPLU_->thirdLabelRotationRaw()).append(",");
        msg.append("0,");       //  backDateDays - set to 0 b\c sometimes it was hex. Not needed here
        msg.append(defaultCurrentPLU_->points()).append(",");
        msg.append(defaultCurrentPLU_->graphic1Name()).append(",");
        msg.append(defaultCurrentPLU_->graphic2Name()).append(",");
        msg.append(defaultCurrentPLU_->graphic3Name()).append(",");
        msg.append(defaultCurrentPLU_->graphic4Name()).append(",");
        msg.append("\"").append(defaultCurrentPLU_->primaryLabelTypeWidth()).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->primaryLabelTypeLength()).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->secondLabelTypeWidth()).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->secondLabelTypeLength()).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->thirdLabelTypeWidth()).append("\",");
        msg.append("\"").append(defaultCurrentPLU_->thirdLabelTypeLength()).append("\",");
        msg.append(defaultCurrentPLU_->forcedByCountRaw()).append(",");
        msg.append(defaultCurrentPLU_->forcedTareRaw()).append(",");
        msg.append(defaultCurrentPLU_->fractionalPriceUnitRaw()).append(",");
        msg.append(defaultCurrentPLU_->poundsForAmountRaw()).append(",");
        msg.append(defaultCurrentPLU_->forcedUnitPriceRaw()).append(",");
        msg.append(defaultCurrentPLU_->tareType()).append(",");
        msg.append( QString::number( defaultCurrentPLU_->applySecurity() ) ).append(",");
        msg.append(defaultCurrentPLU_->dontPrintPackedOnRaw()).append(",");
        msg.append(defaultCurrentPLU_->dontPrintSellByRaw()).append(",");
        msg.append(defaultCurrentPLU_->dontPrintBestBeforeRaw()).append(",");
        msg.append(defaultCurrentPLU_->primaryLabelPlacementRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondLabelPlacementRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdLabelPlacementRaw()).append(",");
        msg.append(",");    //  fractionalPrice
        msg.append(",");    //  forcedFractionalPrice
        msg.append(",");    //  poundsForPrice
        msg.append(",");    //  forcedPoundsForPrice
        msg.append(",");    //  sellByDate
        msg.append(",");    //  sellByTime
        msg.append(",");    //  bestBeforeDate
        msg.append(",");    //  bestBeforeTime
        msg.append(",");    //  hasNutrifacts
        msg.append(defaultCurrentPLU_->intakeMode()).append(",");	//	HA-38
        msg.append(",");    //  hasFrequentShopper
        msg.append(",");    //  productGraphic
        msg.append(",");    //  productName1
        msg.append(",");    //  productName2
        msg.append(",");    //  categoryNumber
        msg.append(",");    //  specialType
        msg.append(",");    //  coolCategoryNum
        msg.append(",");    //  coolPretextNum
        msg.append(",");    //  coolDefaultTextNum
        msg.append(",");    //  coolTextNum
        msg.append(",");    //  coolShortListNum
        msg.append(",");    //  forcedCool
        msg.append(",");    //  coolTrackingText
        msg.append(",");    //  coolRequired
        msg.append(",");    //  keyboardNetWeight
        msg.append(",");    //  acceptKbdNetWeight
        msg.append(",");    //  numOfLabelsForced
        msg.append(",");    //  numOfLabels
        msg.append(defaultCurrentPLU_->productionPlanningType()).append(",");
        msg.append(",");    //  forcedBackDateDays
        msg.append(",");    //  expirationDaysForced
        msg.append(",");    //  expirationDays

        if( defaultCurrentPLU_->eanFormatString().length() > 0)
            msg.append(defaultCurrentPLU_->eanFormatString()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        if( defaultCurrentPLU_->ean128CFormatString().length() > 0 )
            msg.append(defaultCurrentPLU_->ean128CFormatString()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        if( defaultCurrentPLU_->gs1DatabarGTIN().length() > 0)
            msg.append(defaultCurrentPLU_->gs1DatabarGTIN()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        if( defaultCurrentPLU_->gs1DatabarAI().length() > 0 )
            msg.append(defaultCurrentPLU_->gs1DatabarAI()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        msg += ",";                                                 //backdateDate
        msg += defaultCurrentPLU_->portions() + ",";
        msg += defaultCurrentPLU_->showPortions() + ",";
        msg += defaultCurrentPLU_->sideItemsGroupNumRaw() + ",";
        msg += defaultCurrentPLU_->mealSideItemsFreeRaw() + ",";
        msg += defaultCurrentPLU_->sideItemsIncludedInMealRaw() + ",";
        msg += defaultCurrentPLU_->meatSpecificationSheetRaw() + ",";
        msg.append("\"").append(defaultCurrentPLU_->tareB().replace(unitSeparator, ".")).append("\",");

        if( defaultCurrentPLU_->qrCodeFormatString().length() > 0)
            msg.append(defaultCurrentPLU_->qrCodeFormatString()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        if( defaultCurrentPLU_->dataMatrixFormatString().length() > 0 )
            msg.append(defaultCurrentPLU_->dataMatrixFormatString()).append(",");
        else
            msg.append("\"\",");	// because of some bug in LEGACY, the app needs a "" empty string

        msg.append(",");    //  minWeight
        msg.append(",");    //  maxWeight
        msg.append(",");    //  minPrice
        msg.append(",");    //  maxPrice

        msg.append(defaultCurrentPLU_->primaryDefaultLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondDefaultLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdDefaultLabelTypeRaw()).append(",");

        msg.append(defaultCurrentPLU_->primaryManualLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondManualLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdManualLabelTypeRaw()).append(",");

        msg.append(defaultCurrentPLU_->primaryPrepackLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondPrepackLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdPrepackLabelTypeRaw()).append(",");

        msg.append(defaultCurrentPLU_->primaryProdEntryLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondProdEntryLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdProdEntryLabelTypeRaw()).append(",");

        msg.append(defaultCurrentPLU_->primarySelfServLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondSelfServLabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdSelfServLabelTypeRaw()).append(",");

        msg.append(defaultCurrentPLU_->primaryPick5LabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->secondPick5LabelTypeRaw()).append(",");
        msg.append(defaultCurrentPLU_->thirdPick5LabelTypeRaw()).append(",");

        msg += defaultCurrentPLU_->EANprefix();

        session_->send(ProductDev, ProductWriteDefault, msg);
        session_->send(ProductDev, ProductSaveDefault);

        hasChanged_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::updateDefaultPluData(const SessionMessage &msg)
{
    // NOTE: Previously this page, in the ASP code, was supported by it's own messages, 101,18 and 101,19,
    // but the 101,19 write was actually stored into the 101,7 dom form (currentPLU). Since we have broken
    // completely away from sharing data across the same class, a new class based upon DomCurrentPLU (honestly
    // just copied, pasted, and renamed) is now a new class named DomDefaultCurrentPLU. It has all of the same
    // data members,variables,storage, etc. I just wanted to inform the person who ends up coding this whole page.

    DomDefaultCurrentPLU m( msg );

    if ( defaultCurrentPLU_ )
        *defaultCurrentPLU_ = m;

    isUpdating_ = true;

    UiUtilities *util = session_->uiUtilities;


    // tab 0
    QString tmpProdType = m.productTypeRaw();
    ui->cboProductType->setCurrentIndex( ui->cboProductType->findData( tmpProdType ) );
    savedProductType = tmpProdType;

    int savedProductIntType = savedProductType.toInt();
    if( ( savedProductIntType < 1 ) || ( savedProductIntType > 4 ))
    {
        savedProductType    = "1";
        savedProductIntType = RW_PLU;       // just in case
    }

    // tab 3 - GS1
    if( savedProductIntType == RW_PLU )
    {
        // random weight
        enableRandomWtBarcodes( true );
        enableFixedWtBarcodes( false );
        enableByCountBarcodes( false );
        enableFluidWtBarcodes( false );

        ui->butRW_GTIN->setText( m.gs1DatabarGTIN() );
        ui->butRW_AI->setText( m.gs1DatabarAI() );
        ui->butRW_QR->setText( m.qrCodeFormatString() );
        ui->butRW_DM->setText( m.dataMatrixFormatString() );
    }
    else if( savedProductIntType == FW_PLU )
    {
        // fixed weight
        enableRandomWtBarcodes( false );
        enableFixedWtBarcodes( true );
        enableByCountBarcodes( false );
        enableFluidWtBarcodes( false );

        ui->butFW_GTIN->setText( m.gs1DatabarGTIN() );
        ui->butFW_AI->setText( m.gs1DatabarAI() );
        ui->butFW_QR->setText( m.qrCodeFormatString() );
        ui->butFW_DM->setText( m.dataMatrixFormatString() );
    }
    else if( savedProductIntType == BC_PLU )
    {
        // bycount
        enableRandomWtBarcodes( false );
        enableFixedWtBarcodes( false );
        enableByCountBarcodes( true );
        enableFluidWtBarcodes( false );

        ui->butBC_GTIN->setText( m.gs1DatabarGTIN() );
        ui->butBC_AI->setText( m.gs1DatabarAI() );
        ui->butBC_QR->setText( m.qrCodeFormatString() );
        ui->butBC_DM->setText( m.dataMatrixFormatString() );
    }
    else if( savedProductIntType == FL_PLU )
    {
        // fluid weight
        enableRandomWtBarcodes( false );
        enableFixedWtBarcodes( false );
        enableByCountBarcodes( false );
        enableFluidWtBarcodes( true );

        ui->butFL_GTIN->setText( m.gs1DatabarGTIN() );
        ui->butFL_AI->setText( m.gs1DatabarAI() );
        ui->butFL_QR->setText( m.qrCodeFormatString() );
        ui->butFL_DM->setText( m.dataMatrixFormatString() );
    }

    repopulateUPCTypeCombo( savedProductIntType );
    QString tmpNumSys = m.numberSystem();           // did this to see the value since I was having an issue
    ui->cboUPCType->setCurrentIndex( ui->cboUPCType->findData(tmpNumSys) );

    QString tmpEanPrefix = m.EANprefix();
    ui->txtEANNumberSystem->setText( tmpEanPrefix );
    ui->butEanFormatString->setText( m.eanFormatString() );
    ui->butEan128FormatString->setText( m.ean128CFormatString() );

    ui->txtReportingClass->setText( m.reportingClass() );

    // need to strip the decsription
    //ui->txtDescription->setText( m.description() );
    ui->txtDescription->setText(this->getEncodedTextOnly(m.description()));

    // tab 1
    ui->cboDiscountType->setCurrentIndex( m.discountType() );
    ui->txtDiscount1->setText( util->youGottaSetTheSeparator(m.discountValue1()) );
    ui->txtDiscount2->setText( util->youGottaSetTheSeparator(m.discountValue2()) );
    ui->txtDiscount3->setText( util->youGottaSetTheSeparator(m.discountValue3()) );

    ui->chkForcedUnitPrice->setChecked( m.forcedUnitPrice() );
    if( m.forcedUnitPrice() )
        ui->txtUnitPrice->setText("");
    else
        ui->txtUnitPrice->fromSessionField( util->youGottaSetTheSeparator(m.unitPrice()) );

    ui->chkForcedByCount->setChecked( m.forcedByCount() );
    if(  m.forcedByCount() )
        ui->txtByCount->setText("");
    else
        ui->txtByCount->fromSessionField( m.byCountRaw() );

    ui->chkForcedTare->setChecked( m.forcedTare() );
    //////////////////////////////////////////////////////////////////////////
    // FT-1012 Tare Precision
    // If the incoming digits do not match the actual tare precision, it messes stuff
    // up. Whatever the incoming number of digits is for the tare value, it overrides
    // the setDecimals() value of the tare QValidator. So right here I had to enforce
    // the number of tare digits to match the scaleCOnfig.tarePrecision.

    // Then we need to check the precision digits digits and maybe update the tare
    int digitsOfPrecision = scaleConfigData_->tarePrecision();
    QStringList tareList = ( scaleConfigData_->useSecondTare() ? defaultCurrentPLU_->tareB().split(".") : defaultCurrentPLU_->tare().split(".") );
    int tareDigitsLen = tareList.at(1).length();

    // Now make sure the tare has the same number of digits of its required precision
    if(3 == digitsOfPrecision && 2 == tareDigitsLen)
    {
        // Means our tare has 2 digits of precision, and we need 3
        if( scaleConfigData_->useSecondTare() )
            defaultCurrentPLU_->tareB() = defaultCurrentPLU_->tareB() + "0";
        else
            defaultCurrentPLU_->tare() = defaultCurrentPLU_->tare() + "0";
    }
    else if(2 == digitsOfPrecision && 3 == tareDigitsLen)
    {
        // we have too much tare, crop the last digit
        if( scaleConfigData_->useSecondTare() )
            defaultCurrentPLU_->tareB() = defaultCurrentPLU_->tareB().mid(0, defaultCurrentPLU_->tareB().length() - 1);
        else
            defaultCurrentPLU_->tare() = defaultCurrentPLU_->tare().mid(0, defaultCurrentPLU_->tare().length() - 1);
    }

    //////////////////////////////////////////////////////////////////////////

    // HT-136: Set the text field to the correct tare value
    if( scaleConfigData_->useSecondTare() )
        ui->txtTare->fromSessionField( defaultCurrentPLU_->tareB() );
    else
        ui->txtTare->fromSessionField( defaultCurrentPLU_->tare() );

    //QString myTare =  util->youGottaSetTheSeparator(defaultCurrentPLU_->tare());
    //if( defaultCurrentPLU_->forcedTare() )
    //    ui->txtTare->setText("");
    //else
    //    ui->txtTare->fromSessionField( myTare );

    ui->txtProportionalTare->fromSessionField( util->youGottaSetTheSeparator(m.proportionalTare()) );
    ui->txtNetWeight->fromSessionField( m.fixedWeight() );
    ui->txtExceptionPrice->fromSessionField( util->youGottaSetTheSeparator(m.exceptionPrice()) );
    ui->txtPoints->setText( m.points() );

    // tab 2
    ui->txtShelfLifeDays->setText( m.shelfLifeDaysRaw() );
    ui->txtShelfLifeHours->setText( m.shelfLifeHoursRaw());
    ui->txtProductLifeDays->setText( m.productLifeDaysRaw() );
    ui->txtProductLifeHours->setText( m.productLifeHoursRaw());
    ui->cboProductionPlanningType->setCurrentIndex( ui->cboProductionPlanningType->findData( m.productionPlanningType() ) );
    ui->chkDontPrintPackedOn->setChecked( m.dontPrintPackedOn() );
    ui->chkDontPrintSellBy->setChecked( m.dontPrintSellBy() );
    ui->chkDontPrintBestBefore->setChecked( m.dontPrintBestBefore() );

    // tab 3
    ui->txtDefaultPriLabelType->setText( m.primaryDefaultLabelTypeRaw() );
    ui->txtDefaultSecLabelType->setText( m.secondDefaultLabelTypeRaw() );

    ui->txtDefaultTriLabelType->setText(  m.thirdDefaultLabelTypeRaw() );
    ui->txtManualPriLabelType->setText( m.primaryManualLabelTypeRaw() );
    ui->txtManualSecLabelType->setText( m.secondManualLabelTypeRaw() );
    ui->txtManualTriLabelType->setText(  m.thirdManualLabelTypeRaw() );
    ui->txtPrepackPriLabelType->setText( m.primaryPrepackLabelTypeRaw() );
    ui->txtPrepackSecLabelType->setText( m.secondPrepackLabelTypeRaw() );
    ui->txtPrepackTriLabelType->setText(  m.thirdPrepackLabelTypeRaw() );
    ui->txtProdEntryPriLabelType->setText( m.primaryProdEntryLabelTypeRaw() );
    ui->txtProdEntrySecLabelType->setText( m.secondProdEntryLabelTypeRaw() );
    ui->txtProdEntryTriLabelType->setText(  m.thirdProdEntryLabelTypeRaw() );
    ui->txtSelfServPriLabelType->setText( m.primarySelfServLabelTypeRaw() );
    ui->txtSelfServSecLabelType->setText( m.secondSelfServLabelTypeRaw() );
    ui->txtSelfServTriLabelType->setText(  m.thirdSelfServLabelTypeRaw() );
    ui->txtPick5PriLabelType->setText( m.primaryPick5LabelTypeRaw() );
    ui->txtPick5SecLabelType->setText( m.secondPick5LabelTypeRaw() );
    ui->txtPick5TriLabelType->setText(  m.thirdPick5LabelTypeRaw() );

    int defPrimaryLabelPlacement = m.primaryLabelPlacement();
    if( defPrimaryLabelPlacement > 2 )
    {
        defPrimaryLabelPlacement = 2;
        defaultCurrentPLU_->primaryLabelPlacementRaw() = QString::number(defPrimaryLabelPlacement);
    }

    ui->cboPrimaryApplierRotation->blockSignals( true );            // dont want these firing off save message SIGNALS
    ui->cboSecondApplierRotation->blockSignals( true );
    ui->cboThirdApplierRotation->blockSignals( true );
    ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );
    ui->cboThirdLabelPlacement->blockSignals( true );

    populateSecondaryPositions();

    ui->cboPrimaryLabelPlacement->setCurrentIndex( ui->cboPrimaryLabelPlacement->findData( m.primaryLabelPlacementRaw() ) );
    ui->cboSecondLabelPlacement->setCurrentIndex(  ui->cboSecondLabelPlacement->findData(  m.secondLabelPlacementRaw()  ) );
    ui->cboThirdLabelPlacement->setCurrentIndex(   ui->cboThirdLabelPlacement->findData(   m.thirdLabelPlacementRaw()   ) );

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboPrimaryApplierRotation->setCurrentIndex( ui->cboPrimaryApplierRotation->findData( m.primaryLabelRotationRaw() ) );
    ui->cboSecondApplierRotation->setCurrentIndex( ui->cboSecondApplierRotation->findData(   m.secondLabelRotationRaw()  ) );
    ui->cboThirdApplierRotation->setCurrentIndex(ui->cboThirdApplierRotation->findData(      m.thirdLabelRotation()      ) );

    ui->cboPrimaryApplierRotation->blockSignals( false );            // dont want these firing off save message SIGNALS
    ui->cboSecondApplierRotation->blockSignals( false );
    ui->cboThirdApplierRotation->blockSignals( false );
    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );
    ui->cboThirdLabelPlacement->blockSignals( false );

    ui->cboIntakeMode->setCurrentIndex(ui->cboIntakeMode->findData(m.intakeMode()));

    // FT-715:  Graphics were not being updated when you change the Product Type
    if( defaultCurrentPLU_->graphic1Name() != "None" )
        ui->cboGraphic1->setCurrentIndex(ui->cboGraphic1->findText( defaultCurrentPLU_->graphic1Name()));
    else
        ui->cboGraphic1->setCurrentIndex(0);

    if( defaultCurrentPLU_->graphic2Name() != "None" )
        ui->cboGraphic2->setCurrentIndex(ui->cboGraphic2->findText(defaultCurrentPLU_->graphic2Name()));
    else
        ui->cboGraphic2->setCurrentIndex(0);

    if( defaultCurrentPLU_->graphic3Name() != "None" )
        ui->cboGraphic3->setCurrentIndex(ui->cboGraphic3->findText(defaultCurrentPLU_->graphic3Name()));
    else
        ui->cboGraphic3->setCurrentIndex(0);

    if(defaultCurrentPLU_->graphic4Name() != "None" )
        ui->cboGraphic4->setCurrentIndex(ui->cboGraphic4->findText(defaultCurrentPLU_->graphic4Name()));
    else
        ui->cboGraphic4->setCurrentIndex(0);

    setupVisibility( m.productType() );

    hasChanged_ = false;
    isUpdating_ = false;
    currentTab = 0;

    update();
}

QString SuperProductDefaults::getEncodedTextOnly(QString incomingString)
{
    QString result;

    if (incomingString.trimmed() == "")
        return incomingString;

    // fn=[font family name];ps=[point size];
    QRegExp rx("^fn=([a-z,A-Z,0-9\\s]+);ps=(\\d\\d?)$");
    QString localEscSeq = QString(ESC_CHAR);
    // Split contents
    QStringList dataItems = incomingString.split(localEscSeq );

    // [ESC][ESC]fn={font family};ps={point size};[ESC][text]
    foreach(QString dataItem, dataItems)
    {
        // if data item is a font specifier
        if (rx.indexIn(dataItem) > -1 )
        {
        }
        else
        {
            // only add if text found
            if (dataItem.trimmed() != "")
            {
                result += dataItem;
            }
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::updateGraphicNames(const SessionMessage &msg)
{
    ignoreSignals_ = true;

    DomGraphicNameListProduct m( msg );
    QStringList graphicNames = m.graphicList().split(",", QString::KeepEmptyParts);

    ui->cboGraphic1->addItem(tr("None"), "None");
    ui->cboGraphic2->addItem(tr("None"), "None");
    ui->cboGraphic3->addItem(tr("None"), "None");
    ui->cboGraphic4->addItem(tr("None"), "None");

    ui->cboGraphic1->addItems(graphicNames);
    ui->cboGraphic2->addItems(graphicNames);
    ui->cboGraphic3->addItems(graphicNames);
    ui->cboGraphic4->addItems(graphicNames);

    if( defaultCurrentPLU_->graphic1Name() != "None" )
        ui->cboGraphic1->setCurrentIndex(ui->cboGraphic1->findText( defaultCurrentPLU_->graphic1Name()));
    else
        ui->cboGraphic1->setCurrentIndex(0);

    if( defaultCurrentPLU_->graphic2Name() != "None" )
        ui->cboGraphic2->setCurrentIndex(ui->cboGraphic2->findText(defaultCurrentPLU_->graphic2Name()));
    else
        ui->cboGraphic2->setCurrentIndex(0);

    if( defaultCurrentPLU_->graphic3Name() != "None" )
        ui->cboGraphic3->setCurrentIndex(ui->cboGraphic3->findText(defaultCurrentPLU_->graphic3Name()));
    else
        ui->cboGraphic3->setCurrentIndex(0);

    if(defaultCurrentPLU_->graphic4Name() != "None" )
        ui->cboGraphic4->setCurrentIndex(ui->cboGraphic4->findText(defaultCurrentPLU_->graphic4Name()));
    else
        ui->cboGraphic4->setCurrentIndex(0);

    update();

    ignoreSignals_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::currentChangeTab(int tabNum)
{
    if( currentTab != tabNum )
    {
        if( currentTab == 0 )
        {
            if(hasChanged_)
            {
                QString tmpUpcType = ( ui->cboUPCType->itemData( ui->cboUPCType->currentIndex())).toString();
                defaultCurrentPLU_->numberSystem() = tmpUpcType;
                defaultCurrentPLU_->EANprefix() = ui->txtEANNumberSystem->text();
                defaultCurrentPLU_->eanFormatString() = ui->butEanFormatString->text();
                defaultCurrentPLU_->ean128CFormatString() = ui->butEan128FormatString->text();
                defaultCurrentPLU_->reportingClass() = ui->txtReportingClass->text();
                //defaultCurrentPLU_->description()= ui->txtDescription->toPlainText();
            }
        }
        else if( currentTab == 1 )
        {
            if(hasChanged_)
            {
                defaultCurrentPLU_->discountTypeRaw() = QString::number( ui->cboDiscountType->currentIndex());
                defaultCurrentPLU_->discountValue1() = ui->txtDiscount1->text();
                defaultCurrentPLU_->discountValue2() = ui->txtDiscount2->text();
                defaultCurrentPLU_->discountValue3() = ui->txtDiscount3->text();
                defaultCurrentPLU_->unitPrice() = ui->txtUnitPrice->text();
                defaultCurrentPLU_->byCountRaw() = ui->txtByCount->text();

                if( scaleConfigData_->useSecondTare() )
                    defaultCurrentPLU_->tareB() = ui->txtTare->text();
                else
                    defaultCurrentPLU_->tare() = ui->txtTare->text();

                defaultCurrentPLU_->proportionalTare() = ui->txtProportionalTare->text();
                defaultCurrentPLU_->fixedWeight() = ui->txtNetWeight->text();
                defaultCurrentPLU_->exceptionPrice() = ui->txtExceptionPrice->text();
                defaultCurrentPLU_->points() = ui->txtPoints->text();

                //save forced fields
                int pluType = defaultCurrentPLU_->productType();

                if( 1 < pluType )                           // cant be a random weight
                {
                    if(ui->chkForcedByCount->isChecked() )
                        defaultCurrentPLU_->forcedByCountRaw() = "1";
                    else
                        defaultCurrentPLU_->forcedByCountRaw() = "0";
                }

                if( 1 == pluType || 3 == pluType )          // random weights or bycounts
                {
                    if(ui->chkForcedTare->isChecked() )
                        defaultCurrentPLU_->forcedTareRaw() = "1";
                    else
                        defaultCurrentPLU_->forcedTareRaw() = "0";
                }

                // price doesnt have to be checked at all
                if(ui->chkForcedUnitPrice->isChecked() )
                    defaultCurrentPLU_->forcedUnitPriceRaw() = "1";
                else
                    defaultCurrentPLU_->forcedUnitPriceRaw() = "0";
            }
        }
        else if( currentTab == 2 )
        {
            if(hasChanged_)
            {
                defaultCurrentPLU_->shelfLifeDaysRaw() = ui->txtShelfLifeDays->text();
                defaultCurrentPLU_->shelfLifeHoursRaw() = ui->txtShelfLifeHours->text();
                defaultCurrentPLU_->productLifeDaysRaw() = ui->txtProductLifeDays->text();
                defaultCurrentPLU_->productLifeHoursRaw() = ui->txtProductLifeHours->text();
                QString myFPP = QString::number( ui->cboProductionPlanningType->currentIndex() );
                defaultCurrentPLU_->productionPlanningType() = myFPP;
                defaultCurrentPLU_->dontPrintPackedOnRaw() = ui->chkDontPrintPackedOn->isChecked() == true ? "1" : "0";
                defaultCurrentPLU_->dontPrintSellByRaw() = ui->chkDontPrintSellBy->isChecked() == true ? "1" : "0";
                defaultCurrentPLU_->dontPrintBestBeforeRaw() = ui->chkDontPrintBestBefore->isChecked() == true ? "1" : "0";
            }
        }
        else if( currentTab == 3 )
        {
            if(hasChanged_)
            {
                defaultCurrentPLU_->primaryDefaultLabelTypeRaw()   = ui->txtDefaultPriLabelType->text();
                defaultCurrentPLU_->secondDefaultLabelTypeRaw()    = ui->txtDefaultSecLabelType->text();
                defaultCurrentPLU_->thirdDefaultLabelTypeRaw()     = ui->txtDefaultTriLabelType->text();
                defaultCurrentPLU_->primaryManualLabelTypeRaw()    = ui->txtManualPriLabelType->text();
                defaultCurrentPLU_->secondManualLabelTypeRaw()     = ui->txtManualSecLabelType->text();
                defaultCurrentPLU_->thirdManualLabelTypeRaw()      = ui->txtManualTriLabelType->text();
                defaultCurrentPLU_->primaryPrepackLabelTypeRaw()   = ui->txtPrepackPriLabelType->text();
                defaultCurrentPLU_->secondPrepackLabelTypeRaw()    = ui->txtPrepackSecLabelType->text();
                defaultCurrentPLU_->thirdPrepackLabelTypeRaw()     = ui->txtPrepackTriLabelType->text();
                defaultCurrentPLU_->primaryProdEntryLabelTypeRaw() = ui->txtProdEntryPriLabelType->text();
                defaultCurrentPLU_->secondProdEntryLabelTypeRaw()  = ui->txtProdEntrySecLabelType->text();
                defaultCurrentPLU_->thirdProdEntryLabelTypeRaw()   = ui->txtProdEntryTriLabelType->text();
                defaultCurrentPLU_->primarySelfServLabelTypeRaw()  = ui->txtSelfServPriLabelType->text();
                defaultCurrentPLU_->secondSelfServLabelTypeRaw()   = ui->txtSelfServSecLabelType->text();
                defaultCurrentPLU_->thirdSelfServLabelTypeRaw()    = ui->txtSelfServTriLabelType->text();
                defaultCurrentPLU_->primaryPick5LabelTypeRaw()     = ui->txtPick5PriLabelType->text();
                defaultCurrentPLU_->secondPick5LabelTypeRaw()      = ui->txtPick5SecLabelType->text();
                defaultCurrentPLU_->thirdPick5LabelTypeRaw()       = ui->txtPick5TriLabelType->text();

                // FT-715
                defaultCurrentPLU_->graphic1Name() = ui->cboGraphic1->itemText( ui->cboGraphic1->currentIndex() );
                defaultCurrentPLU_->graphic2Name() = ui->cboGraphic2->itemText( ui->cboGraphic2->currentIndex() );
                defaultCurrentPLU_->graphic3Name() = ui->cboGraphic3->itemText( ui->cboGraphic3->currentIndex() );
                defaultCurrentPLU_->graphic4Name() = ui->cboGraphic4->itemText( ui->cboGraphic4->currentIndex() );

                //    Applier rotation
                defaultCurrentPLU_->primaryLabelRotationRaw() = (ui->cboPrimaryApplierRotation->itemData( ui->cboPrimaryApplierRotation->currentIndex() )).toString();
                defaultCurrentPLU_->secondLabelRotationRaw() = (ui->cboSecondApplierRotation->itemData( ui->cboSecondApplierRotation->currentIndex() )).toString();
                defaultCurrentPLU_->thirdLabelRotationRaw() = (ui->cboThirdApplierRotation->itemData( ui->cboThirdApplierRotation->currentIndex() )).toString();

                //    Label placement
                int test_priLabLoc =  ui->cboPrimaryLabelPlacement->currentData().toInt();
                int test_secLabLoc =  ui->cboSecondLabelPlacement->currentData().toInt();
                int test_trdLabLoc =  ui->cboThirdLabelPlacement->currentData().toInt();

                defaultCurrentPLU_->primaryLabelPlacementRaw() = QString::number(test_priLabLoc);
                defaultCurrentPLU_->secondLabelPlacementRaw()  = QString::number(test_secLabLoc);
                defaultCurrentPLU_->thirdLabelPlacementRaw()   = QString::number(test_trdLabLoc);

                //    Wrapper intake
                defaultCurrentPLU_->intakeMode() = (ui->cboIntakeMode->itemData( ui->cboIntakeMode->currentIndex() )).toString();
            }
        }
        else if( currentTab == 4 )
        {
            if( hasChanged_ )
            {
                if( savedProductType == "1" )
                {
                    defaultCurrentPLU_->gs1DatabarGTIN() = ui->butRW_GTIN->text();
                    defaultCurrentPLU_->gs1DatabarAI() = ui->butRW_AI->text();
                    defaultCurrentPLU_->qrCodeFormatString() = ui->butRW_QR->text();
                    defaultCurrentPLU_->dataMatrixFormatString() = ui->butRW_DM->text();
                }
                else if( savedProductType == "2" )
                {
                    defaultCurrentPLU_->gs1DatabarGTIN() = ui->butFW_GTIN->text();
                    defaultCurrentPLU_->gs1DatabarAI() = ui->butFW_AI->text();
                    defaultCurrentPLU_->qrCodeFormatString() = ui->butFW_QR->text();
                    defaultCurrentPLU_->dataMatrixFormatString() = ui->butFW_DM->text();
                }
                else if( savedProductType == "3" )
                {
                    defaultCurrentPLU_->gs1DatabarGTIN() = ui->butBC_GTIN->text();
                    defaultCurrentPLU_->gs1DatabarAI() = ui->butBC_AI->text();
                    defaultCurrentPLU_->qrCodeFormatString() = ui->butBC_QR->text();
                    defaultCurrentPLU_->dataMatrixFormatString() = ui->butBC_DM->text();
                }
                else
                {
                    defaultCurrentPLU_->gs1DatabarGTIN() = ui->butFL_GTIN->text();
                    defaultCurrentPLU_->gs1DatabarAI() = ui->butFL_AI->text();
                    defaultCurrentPLU_->qrCodeFormatString() = ui->butFL_QR->text();
                    defaultCurrentPLU_->dataMatrixFormatString() = ui->butFL_DM->text();
                }
            }
        }
    }

    currentTab = tabNum;
}

void SuperProductDefaults::onPlacementButtonClicked()
{
    if(session_->isPrepack())
    {
        if( supervisorMode_ )
        {
            // store product defaults in scaleConfigs
            session_->scaleConfig->primaryLabelPlacement   = defaultCurrentPLU_->primaryLabelPlacement();
            session_->scaleConfig->secondaryLabelPlacement = defaultCurrentPLU_->secondLabelPlacement();
            session_->scaleConfig->primaryLabelRotation    = defaultCurrentPLU_->primaryLabelRotation();
            session_->scaleConfig->secondaryLabelRotation  = defaultCurrentPLU_->secondLabelRotation();

            supervisorMode_->setCameFromProductDefaults( true );
        }

        mainWindow_->switchToModeScreen<SupervisorMode, superLabelPlacementView>();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::comboboxChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    hasChanged_ = true;
}

void SuperProductDefaults::onPrimaryPlacementComboChanged(int newIndex)
{
    LOG_CLASS_INFO( "SuperProductDefaults", "onPrimaryPlacementComboChanged" );
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );

    // Check what the DOM values are coming in
    LOG_DEBUG( "onPrimaryPlacementChanged() walking in defaultCurrentPLU_->primaryLabelPlacementRaw() is: %s", qPrintable(defaultCurrentPLU_->primaryLabelPlacementRaw()) );
    LOG_DEBUG( "onPrimaryPlacementChanged() walking in defaultCurrentPLU_->secondaryLabelPlacementRaw() is: %s", qPrintable(defaultCurrentPLU_->secondLabelPlacementRaw()) );

    // update the primary DOM
    defaultCurrentPLU_->primaryLabelPlacementRaw() = (ui->cboPrimaryLabelPlacement->itemData( ui->cboPrimaryLabelPlacement->currentIndex() )).toString();

    // saved the data after changing the primary, a change forces the secondary to update the available options - so repopulate secondary based on primary
    populateSecondaryPositions();

    // ok so we repopulated the fields, now set the secondary back to what it was
    int primaryData  = defaultCurrentPLU_->primaryLabelPlacement();
    //int secondaryData = defaultCurrentPLU_->secondLabelPlacement();

    // ok lets reset the secondary now
    if ( primaryData == NONE )
    {
        // The Primary to NONE so leave secondary alone - do not reset DOM
        ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( defaultCurrentPLU_->secondLabelPlacementRaw() ) );
    }
    else if( primaryData >= UPPER_RIGHT )
    {
        // if we have Left or Right of Primary, and we move from Top LEft to top Right, we might be creating a weird edge case, so test is, and fix it
        // Otherwise leave where it is and force operator to update secondary
        int primaryIndex = defaultCurrentPLU_->primaryLabelPlacement();
        int secondIndex  = defaultCurrentPLU_->secondLabelPlacement();

        // These are both cases where the new primary index, eliminates the existing secondary as unselectable. So autoswith it with the move across
        if ( ( primaryIndex == UPPER_RIGHT ) && ( secondIndex == RIGHT_OF_PRIMARY) )
        {
            defaultCurrentPLU_->secondLabelPlacementRaw() = QString::number(LEFT_OF_PRIMARY);
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( defaultCurrentPLU_->secondLabelPlacementRaw() ) );
        }
        else if ( ( primaryIndex == UPPER_LEFT ) && ( secondIndex == LEFT_OF_PRIMARY ) )
        {
            defaultCurrentPLU_->secondLabelPlacementRaw() = QString::number(RIGHT_OF_PRIMARY);
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( defaultCurrentPLU_->secondLabelPlacementRaw() ) );
        }
        else
        {
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( defaultCurrentPLU_->secondLabelPlacementRaw() ) );                         // fail safe but it should never be used
        }
    }
    else
    {
        // Did NOT find index in combo, so index -1, so set and bail
        ui->cboSecondLabelPlacement->setCurrentIndex( defaultCurrentPLU_->secondLabelPlacement() );

    }

    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );

    hasChanged_ = true;
}

void SuperProductDefaults::onSecondaryPlacementComboChanged(int newIndex)
{
    LOG_CLASS_INFO( "SuperProductDefaults", "onSecondaryPlacementComboChanged" );

    LOG_DEBUG( "onSecondaryPlacementComboChanged() What is the new index selected for cboSecondaryCombo? %d", newIndex );

    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    defaultCurrentPLU_->secondLabelPlacementRaw() = (ui->cboSecondLabelPlacement->itemData( ui->cboSecondLabelPlacement->currentIndex() )).toString();

    hasChanged_ = true;
}

void SuperProductDefaults::onThirdPlacementComboChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    defaultCurrentPLU_->thirdLabelPlacementRaw() = (ui->cboThirdLabelPlacement->itemData( ui->cboThirdLabelPlacement->currentIndex() )).toString();

    hasChanged_ = true;
}

void SuperProductDefaults::onPrimaryRotationComboChanged(int newIndex)
{   
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    defaultCurrentPLU_->primaryLabelRotationRaw() = (ui->cboPrimaryApplierRotation->itemData( ui->cboPrimaryApplierRotation->currentIndex() )).toString();
    hasChanged_ = true;
}

void SuperProductDefaults::onSecondaryRotationComboChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    defaultCurrentPLU_->secondLabelRotationRaw() = (ui->cboSecondApplierRotation->itemData( ui->cboSecondApplierRotation->currentIndex() )).toString();

    hasChanged_ = true;
}

void SuperProductDefaults::onThirdRotationComboChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( ignoreSignals_ || isUpdating_ )
        return;

    defaultCurrentPLU_->thirdLabelRotationRaw() = (ui->cboThirdApplierRotation->itemData( ui->cboThirdApplierRotation->currentIndex() )).toString();

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::textboxChanged()
{
    if( ignoreSignals_ || isUpdating_ )
        return;

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::checkboxChanged(int state)
{
    Q_UNUSED( state );

    if( isUpdating_ )
        return;

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::checkboxChangedForcedUnitPrice(int state)
{
    if( isUpdating_ )
        return;

    if(2 == state)
        ui->txtUnitPrice->setText("");
    else
    {
        int precision = currencyData->primaryPrecision();

        if (( 0 <= precision ) && ( precision < zeros.count() ))
            ui->txtUnitPrice->setText("0" + currencyData->unitSeparator() + zeros[precision] );
        else
            ui->txtUnitPrice->setText("0");
    }

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::checkboxChangedForcedByCount(int state)
{
    if( isUpdating_ )
        return;

    if(2 == state)
        ui->txtByCount->setText("");
    else
        ui->txtByCount->setText("1");

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::checkboxChangedForcedTare(int state)
{
    if( isUpdating_ )
        return;

    if(2 == state)
        ui->txtTare->setText("");
    else
    {
        if (( 0 <= tareEntryPrecision ) && ( tareEntryPrecision < zeros.count() ))
            ui->txtTare->setText("0" + currencyData->unitSeparator() + zeros[tareEntryPrecision] );
        else
            ui->txtTare->setText("");
    }

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::requestNewProductType(int newProductType)
{
    if( ignoreSignals_ || isUpdating_ )
        return;

    currentChangeTab(-1);                                           // save the current tab
    saveDataOnExit();                                               // save the current product information to application

    // request new product type information
    QString tmpProdType = ( ui->cboProductType->itemData( newProductType )).toString();
    session_->send(ProductDev, ProductReadDefault, "0," + tmpProdType );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchEanEditor()
{
    eanEditorType = EAN_13;
    eanConfigEditor_ = new hobart::eanEditor(this, ui->butEanFormatString->text(), MAX_DIGITS_EAN13, eanEditorType, mainWindow_);
    eanConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(eanConfigEditor_);
    eanConfigEditor_->show();

    // listening for SIGNALS from the EAN editor
    connect(eanConfigEditor_, SIGNAL( eanEditorDoneClicked( QString ) ), SLOT( onEanEditorDoneClicked( QString )) );
    connect(eanConfigEditor_, SIGNAL( eanEditorCancelClicked( QString ) ), SLOT( onEanEditorCancelClicked( QString )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchEan128Editor()
{
    eanEditorType = EAN_128;
    eanConfigEditor_ = new hobart::eanEditor(this, ui->butEan128FormatString->text(), MAX_DIGITS_EAN128, eanEditorType, mainWindow_);
    eanConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(eanConfigEditor_);
    eanConfigEditor_->show();

    // listening for SIGNALS from the EAN editor
    connect(eanConfigEditor_, SIGNAL( eanEditorDoneClicked( QString ) ), SLOT( onEanEditorDoneClicked( QString )) );
    connect(eanConfigEditor_, SIGNAL( eanEditorCancelClicked( QString ) ), SLOT( onEanEditorCancelClicked( QString )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchRwGTIN()
{
    gs1EditorType = GS1_RW_GTIN;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butRW_GTIN->text(), false, this );
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchFwGTIN()
{
    gs1EditorType = GS1_FW_GTIN;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butFW_GTIN->text(),false, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchBcGTIN()
{
    gs1EditorType = GS1_BC_GTIN;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butBC_GTIN->text(), false, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchFlGTIN()
{
    gs1EditorType = GS1_FL_GTIN;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butFL_GTIN->text(), false, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchRwAI()
{
    gs1EditorType = GS1_RW_AI;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butRW_AI->text(), true, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchFwAI()
{
    gs1EditorType = GS1_FW_AI;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butFW_AI->text(), true, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchBcAI()
{
    gs1EditorType = GS1_BC_AI;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butBC_AI->text(), true, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 5 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchFlAI()
{
    gs1EditorType = GS1_FL_AI;
    gs1ConfigEditor_ = new hobart::gs1Editor(ui->butFL_AI->text(), true, this);
    gs1ConfigEditor_->move( scalePointForScreen( QPoint( 32, 20 ) ) );
    scaleWidgets(gs1ConfigEditor_);
    gs1ConfigEditor_->show();

    // listening for SIGNALS from the GS1 editor
    connect(gs1ConfigEditor_, SIGNAL( doneClicked( QString ) ), SLOT( onGS1EditorDoneClicked( QString )) );
    connect(gs1ConfigEditor_, SIGNAL( cancelClicked(  ) ), SLOT( onGS1EditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchQRCodeEditor()
{
    matrixEditorType = DM_QR_CODE;

    int index = ui->cboProductType->currentIndex();
    int pluType( ui->cboProductType->itemData( index ).toInt() );
    QString barcodeTxt = "";
    switch( pluType )
    {
        case RW_PLU:
            barcodeTxt = ui->butRW_QR->text();
            break;
        case FW_PLU:
            barcodeTxt = ui->butFW_QR->text();
            break;
        case BC_PLU:
            barcodeTxt = ui->butBC_QR->text();
            break;
        case FL_PLU:
            barcodeTxt = ui->butFL_QR->text();
            break;
    }

    datamatrixConfigEditor_ = new hobart::datamatrixEditor(barcodeTxt, this);
    datamatrixConfigEditor_->move( scalePointForScreen( QPoint( 32, 20 ) ) );
    scaleWidgets(datamatrixConfigEditor_);
    datamatrixConfigEditor_->show();

    // listening for SIGNALS from the Data Matrix editor
    connect(datamatrixConfigEditor_, SIGNAL( doneClickedDM( QString ) ), SLOT( onDataMatrixEditorDoneClicked( QString )) );
    connect(datamatrixConfigEditor_, SIGNAL( cancelClickedDM(  ) ),      SLOT( onDataMatrixEditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::launchDataMatrixEditor()
{
    matrixEditorType = DM_DATA_MATRIX;

    int index = ui->cboProductType->currentIndex();
    int pluType( ui->cboProductType->itemData( index ).toInt() );
    QString barcodeTxt = "";
    switch( pluType )
    {
        case RW_PLU:
            barcodeTxt = ui->butRW_DM->text();
            break;
        case FW_PLU:
            barcodeTxt = ui->butFW_DM->text();
            break;
        case BC_PLU:
            barcodeTxt = ui->butBC_DM->text();
            break;
        case FL_PLU:
            barcodeTxt = ui->butFL_DM->text();
            break;
    }

    datamatrixConfigEditor_ = new hobart::datamatrixEditor(barcodeTxt, this);
    datamatrixConfigEditor_->move( scalePointForScreen( QPoint( 32, 20 ) ) );
    scaleWidgets(datamatrixConfigEditor_);
    datamatrixConfigEditor_->show();

    // listening for SIGNALS from the Data Matrix editor
    connect(datamatrixConfigEditor_, SIGNAL( doneClickedDM( QString ) ), SLOT( onDataMatrixEditorDoneClicked( QString )) );
    connect(datamatrixConfigEditor_, SIGNAL( cancelClickedDM(  ) ), SLOT( onDataMatrixEditorCancelClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onEanEditorDoneClicked(QString eanString)
{
    if( eanEditorType == EAN_13 )
        ui->butEanFormatString->setText( eanString );
    else if( eanEditorType == EAN_128)
        ui->butEan128FormatString->setText( eanString );

    hasChanged_ = true;

    // FT-695
    eanConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onEanEditorCancelClicked(QString eanString)
{
    if( eanEditorType == EAN_13 )
        ui->butEanFormatString->setText( eanString );               // this needs to change to a DOM value
    else if( eanEditorType == EAN_128)
        ui->butEan128FormatString->setText( eanString );            // this needs to change to a DOM value

    // FT-695
    eanConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onGS1EditorDoneClicked(const QString &gs1String)
{
    switch( gs1EditorType )
    {
    case GS1_RW_GTIN:
        ui->butRW_GTIN->setText( gs1String );
        break;
    case GS1_FW_GTIN:
        ui->butFW_GTIN->setText( gs1String );
        break;
    case GS1_BC_GTIN:
        ui->butBC_GTIN->setText( gs1String );
        break;
    case GS1_FL_GTIN:
        ui->butFL_GTIN->setText( gs1String );
        break;
    case GS1_RW_AI:
        ui->butRW_AI->setText( gs1String );
        break;
    case GS1_FW_AI:
        ui->butFW_AI->setText( gs1String );
        break;
    case GS1_BC_AI:
        ui->butBC_AI->setText( gs1String );
        break;
    case GS1_FL_AI:
        ui->butFL_AI->setText( gs1String );
        break;
    }

    hasChanged_ = true;

    // FT-695
    gs1ConfigEditor_->deleteLater();
    gs1ConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onGS1EditorCancelClicked()
{
    // FT-695
    gs1ConfigEditor_->deleteLater();
    gs1ConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onDataMatrixEditorDoneClicked(QString datamatrixString)
{
    int index = ui->cboProductType->currentIndex();
    int pluType( ui->cboProductType->itemData( index ).toInt() );

    datamatrixString = datamatrixString.trimmed();

    switch( pluType )
    {
        case RW_PLU:
            if( matrixEditorType == DM_QR_CODE)
                ui->butRW_QR->setText( datamatrixString );      // IF QR Code
            else
                ui->butRW_DM->setText( datamatrixString );      // else DM_DATA_MATRIX
            break;
        case FW_PLU:
            if( matrixEditorType == DM_QR_CODE)
                ui->butFW_QR->setText( datamatrixString );
            else
                ui->butFW_DM->setText( datamatrixString );
            break;
        case BC_PLU:
            if( matrixEditorType == DM_QR_CODE)
                ui->butBC_QR->setText( datamatrixString );
            else
                ui->butBC_DM->setText( datamatrixString );
            break;
        case FL_PLU:
            if( matrixEditorType == DM_QR_CODE)
                ui->butFL_QR->setText( datamatrixString );
            else
                ui->butFL_DM->setText( datamatrixString );
            break;
    }

    hasChanged_ = true;

    datamatrixConfigEditor_->deleteLater();
    datamatrixConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onDataMatrixEditorCancelClicked()
{
    datamatrixConfigEditor_->deleteLater();
    datamatrixConfigEditor_ = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::enableRandomWtBarcodes(bool isEnabled)
{
    ui->lblRW_GTIN->setVisible( isEnabled );
    ui->butRW_GTIN->setVisible( isEnabled );
    ui->lblRW_AI->setVisible( isEnabled );
    ui->butRW_AI->setVisible( isEnabled );

    ui->lblRW_QR->setVisible( isEnabled );
    ui->butRW_QR->setVisible( isEnabled );
    ui->lblRW_DM->setVisible( isEnabled );
    ui->butRW_DM->setVisible( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::enableFixedWtBarcodes(bool isEnabled)
{
    ui->lblFW_GTIN->setVisible( isEnabled );
    ui->butFW_GTIN->setVisible( isEnabled );
    ui->lblFW_AI->setVisible( isEnabled );
    ui->butFW_AI->setVisible( isEnabled );

    ui->lblFW_DM->setVisible( isEnabled );
    ui->butFW_DM->setVisible( isEnabled );
    ui->lblFW_QR->setVisible( isEnabled );
    ui->butFW_QR->setVisible( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::enableByCountBarcodes(bool isEnabled)
{
    ui->lblBC_GTIN->setVisible( isEnabled );
    ui->butBC_GTIN->setVisible( isEnabled );
    ui->lblBC_AI->setVisible( isEnabled );
    ui->butBC_AI->setVisible( isEnabled );

    ui->lblBC_QR->setVisible( isEnabled );
    ui->butBC_QR->setVisible( isEnabled );
    ui->lblBC_DM->setVisible( isEnabled );
    ui->butBC_DM->setVisible( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::enableFluidWtBarcodes(bool isEnabled)
{
    ui->lblFL_GTIN->setVisible( isEnabled );
    ui->butFL_GTIN->setVisible( isEnabled );
    ui->butFL_AI->setVisible( isEnabled );
    ui->butFL_AI->setVisible( isEnabled );

    ui->lblFL_QR->setVisible( isEnabled );
    ui->butFL_QR->setVisible( isEnabled );
    ui->lblFL_DM->setVisible( isEnabled );
    ui->butFL_DM->setVisible( isEnabled );
}

void SuperProductDefaults::populateSecondaryPositions()
{
    LOG_CLASS_INFO( "SuperProductDefaults", "populateSecondaryPositions" );

    LOG_DEBUG( "populateSecondaryPositions()  START", NULL );

    int primaryIndex = defaultCurrentPLU_->primaryLabelPlacement();

    LOG_DEBUG( "populateSecondaryPositions()  with primaryIndex: %d", primaryIndex );

    switch( primaryIndex )
    {
        LOG_DEBUG( "populateSecondaryPositions()  with switch: %d", primaryIndex );
        case upperRight:
        {
            LOG_DEBUG( "populateSecondaryPositions() case upperRight", NULL );
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
            ui->cboSecondLabelPlacement->addItem(tr("Upper Left"), "1");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Left of Primary"), "6");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
            break;
        }
        case upperLeft:
        {
            LOG_DEBUG( "populateSecondaryPositions() case upperLeft", NULL );
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
            ui->cboSecondLabelPlacement->addItem(tr("Upper Right"), "0");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Right of Primary"), "5");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
            break;
        }
        default:
        {
            LOG_DEBUG( "populateSecondaryPositions() case default", NULL );
            ui->cboSecondLabelPlacement->clear();
            ui->cboSecondLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
            ui->cboSecondLabelPlacement->addItem(tr("Upper Right"), "0");
            ui->cboSecondLabelPlacement->addItem(tr("Upper Left"), "1");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Right"), "3");
            ui->cboSecondLabelPlacement->addItem(tr("Lower Left"), "4");
            ui->cboSecondLabelPlacement->addItem(tr("Right of Primary"), "5");
            ui->cboSecondLabelPlacement->addItem(tr("Left of Primary"), "6");
            ui->cboSecondLabelPlacement->addItem(tr("Below Primary"), "8");
            break;
        }
    }
    LOG_DEBUG( "populateSecondaryPositions()  END", NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          MESSAGE CATCHER
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::onSessionMessage( const SessionMessage& msg )
{

    if (( ProductDev == msg[0].toInt() ) && ( ProductWriteDefault == msg[1].toInt() ))
    {
        // 101,19 - update list of operators in the combo
        updateDefaultPluData(msg);
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgScaleOperationWrite == msg[1].toInt())
    {
        //112,71  - Normal Scale Operations
        tareEntryPrecision = msg[26].toInt();
        tareValidator_->setTop( makeValidatorTop( 2 + msg[26].toInt()) );
    }
    else if (GraphicDev == msg[0].toInt() && GraphicReadAllNames == msg[1].toInt())
    {
        // 105,20 - read all graphic names, sent a 105,20 to get them. Huh? Shouldn't this be a 21? A write?
        updateGraphicNames(msg);
    }
    else if (( LabelApplier == msg[0].toInt() ) && (( LaConfig == msg[1].toInt() ) || ( LaInquire == msg[1].toInt() )))
    {
        DomLabelApplierConfig m( msg );

        if( m.secondaryStationInstalled() || DEBUG_PLACEMENT_VIEW )
        {
            ui->butPlacementView->setEnabled( true );
            ui->butPlacementView->setVisible( true );
        }
        else
        {
            ui->butPlacementView->setEnabled( false );
            ui->butPlacementView->setVisible( false );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductDefaults::scaleWidgets(QWidget *widgetToScale){
    if ( (mainWindow_->width() > SessionMainWindow::scalingDividerX_)  ||
         (mainWindow_->height() > SessionMainWindow::scalingDividerY_) )
    {

        // block all events via event filter
        qApp->installEventFilter( this );
        scaling_ = true;

        // scale widget
        scaleWidget(widgetToScale);

        //qApp->processEvents();

        // stop blocking events
        qApp->removeEventFilter( this );
        scaling_ = false;

        // force update
        update();

    }

    return;

}
