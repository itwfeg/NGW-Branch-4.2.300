/* @file superChangeProductAllFields.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd Davis
 * @{
*/

#include <logmanager.h>
#include <QtGui>
#include <QtCore>
#include <QIntValidator>

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/texteditor.h"

#include "../modes/supervisormode.h"
#include "../screens/productmarqueeeditorscreen.h"
#include "../screens/plumainscreen.h"

#include "../screens/supernutritioneditor.h"
#include "../screens/superLabelPlacementView.h"
#include "../screens/labelplacement.h"

#include "../widgets/labelplacementwidget.h" // for the placement enum

#include <ScaleClasses.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "superChangeProductAllFields.h"
#include "ui_superChangeProductAllFields.h"
#include "TokenParser.h"
#include <filesystem.h>
#include <QFontDatabase>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <logmanager.h>

#define TEXT_EDITOR_MAX_CHARS 2000
#define ESC_CHAR 0x1B

static const int PRICING_TAB_POINTS_X          = 617;
static const int PRICING_TAB_POINTS_W          = 251;
static const int PRICING_TAB_POINTS_H          = 41;
static const int PRICING_TAB_GROUP_BOX_SPACING = 3;

static const int DEBUG_PLACEMENT_VIEW = false;
static const int UPPER_RIGHT = 0;
static const int UPPER_LEFT = 1;
static const int NONE = 2;
static const int LEFT_OF_PRIMARY = 6;
static const int RIGHT_OF_PRIMARY = 5;

namespace hobart {
class numericLineEdit;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

superChangeProductAllFields::superChangeProductAllFields(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superChangeProductAllFields),
    pluSupport_( NULL),
    showAllFieldsReuseMarquee_( false ),
    showAllFieldsReuseNutrifacts_( false )
{
    LOG_CLASS_INFO("superChangeProductAllFields", "superChangeProductAllFields");
    // saveOnExit_(false),
    supervisorMode_ = dynamic_cast<SupervisorMode*>(parent);
    LOG_INFO("Constructor START", NULL);

    QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/SPLarge/SPLarge.ttf");
    ui->setupUi(this);
    scaleScreen();
    expandTabs(ui->tabChangeProductAllFields);

    goingToNutrifactsEditorScreen = false;
    goingToMarqueeScreen = false;
    goingToLabelPlacementView = false;

    // FT-730
    initValidators();

    initializationComplete = false;                                 // first time in, false.
    ignoreDiscountPriceSignals_ = false;
    alreadyAdded = false;
    alreadyDidThis = false;
    pluEmitted = 0;                                                 // make sure we don't ask for the same thing twice, or even more times.
    labelEmitted_1 = 0;                                             // make sure we don't ask for the same thing twice, or even more times.
    labelEmitted_2 = 0;                                             // make sure we don't ask for the same thing twice, or even more times.
    labelEmitted_3 = 0;                                             // make sure we don't ask for the same thing twice, or even more times.
    suppTextEmitted = 0;
    reportingClassEmitted = 0;
    updatingFromIncomingPLUData = 0;

    cameFromCreate = true;
    popupActive = false;
    cameFromCancel = false;
    wroteTransaction = false;
    ifUpdating = false;

    this->ui->frameTextEditor->move( scalePointForScreen( QPoint( 0, 0 ) ) );
    this->ui->frameTextEditor->lower();
    this->ui->frameTextEditor->hide();

    this->ui->frameReuseText->move( scalePointForScreen( QPoint( 0, 0 ) ) );
    this->ui->frameReuseText->lower();
    this->ui->frameReuseText->hide();

    this->ui->gbReuseBlocker->move( scalePointForScreen( QPoint( 0, 0 ) ) );
    this->ui->gbReuseBlocker->lower();
    this->ui->gbReuseBlocker->hide();

    this->ui->gbMinMaxWeight->hide();
    this->ui->gbMinMaxPrice->hide();

    normalScaleOperation    = &session_->scaleConfig->normalScaleOperation;
    scaleConfigData         = &session_->scaleConfig->scaleConfig;
    currencyData            = &session_->scaleConfig->currencySettings;
    currentPLUData          = &session_->scaleConfig->currentPLU;
    internalConfigData      = &session_->scaleConfig->internalConfig;
    frequentShopperData     = &session_->scaleConfig->frequentShopper;

    escSeq = QString(ESC_CHAR);
    doubleEscSeq = escSeq + escSeq;
    reuseTextType = "";
    aspTextType = "";
    aspTextFunction = "";
    aspUsedByMany = "";
    aspSupportPLU = "";

    connect(ui->butPlacementView, SIGNAL(clicked()), this, SLOT(onPlacementViewClicked()));


    isTwoPrinterSystem_ = false;
    ui->butPlacementView->setEnabled( false );
    ui->butPlacementView->setVisible( false );

    // FT-656
    shortListEditor_ = NULL;

    // CONNECTIONS
    connect(ui->leProductNumber, SIGNAL(editingFinished()), this, SLOT(doPluLookup()));
    connect(ui->butNewOK, SIGNAL(clicked()), this, SLOT(newPLUOKButClicked()));
    connect(ui->butNewCancel, SIGNAL(clicked()), this, SLOT(newPLUCancelButClicked()));
    connect(ui->cb1ProductType,         SIGNAL(currentIndexChanged(int)), this, SLOT(writeProductType(int)));
    connect(ui->txtPrefix,              SIGNAL( editingFinished() ),      this, SLOT(onPrefixTextChanged() ) );
    connect(ui->txtEANCode,             SIGNAL( editingFinished() ),      this, SLOT(onEANTextChanged() ) );

    connect(ui->txtUPCNumber,           SIGNAL( editingFinished() ),      this, SLOT(onUpcNumTextChanged() ) );
    connect(ui->txtUPCProductNumber,    SIGNAL( editingFinished() ),      this, SLOT(onProdNumTextChanged() ) );

    connect(ui->cb6FlashkeyGraphic, SIGNAL(currentIndexChanged(int)), this, SLOT(displayTheFlashkeyGraphic(int)));
    connect(ui->cb2DiscountType,    SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateDiscountPricing(int)));

    // Pricing Field connections for Supervisor Changes Transactions with Single Field Writes.
    // unit prices
    connect(ui->le2UnitPrice,        SIGNAL( editingFinished() ),   this, SLOT(trackSuperChangeUnitPrice()));
    connect(ui->le2PoundsForPrice,   SIGNAL( editingFinished() ),   this, SLOT(trackSuperChangeLbsForPrice()));
    connect(ui->le2FractionalPrice,  SIGNAL( editingFinished() ),   this, SLOT(trackSuperChangeFractionalPrice()));
    connect(ui->le2UnitPrice,        SIGNAL( textEdited(QString) ), this, SLOT( onUnitPriceTextChanged(QString) ) );
    connect(ui->le2PoundsForPrice,   SIGNAL( textEdited(QString) ), this, SLOT( onLbsForTextChanged(QString) ) );
    connect(ui->le2FractionalPrice,  SIGNAL( textEdited(QString) ), this, SLOT( onFracPriceTextChanged(QString) ) );

    // tares
    connect(ui->le2Tare,             SIGNAL( editingFinished() ),   this,  SLOT( trackSuperChangeTare()));
    connect(ui->le2ProportionalTare, SIGNAL( editingFinished() ),   this,  SLOT( trackSuperChangePropTare()));
    connect(ui->le2Tare,             SIGNAL( textEdited(QString) ), this,  SLOT( onTareTextChanged(QString) ) );
    connect(ui->le2ProportionalTare, SIGNAL( textEdited(QString) ), this,  SLOT( onPropTareTextChanged(QString) ) );

    // bycount
    connect(ui->le2ByCount,          SIGNAL( editingFinished() ),   this,   SLOT( trackSuperChangeByCount()));
    connect(ui->le2ByCount,          SIGNAL( textEdited(QString) ), this,   SLOT( onByCountTextChanged(QString) ) );

    // weights of the net or fixed Variety
    connect(ui->le2NetWeight,        SIGNAL( editingFinished() ),   this,   SLOT( trackSuperChangeNetWeight()));
    connect(ui->le2NetWeightFluid,   SIGNAL( editingFinished() ),   this,   SLOT( trackSuperChangeFluidNetWeight()));
    connect(ui->le2NetWeight,        SIGNAL( textEdited(QString) ), this,   SLOT( onNetWtTextChanged(QString) ) );
    connect(ui->le2NetWeightFluid,   SIGNAL( textEdited(QString) ), this,   SLOT( onFluidNetWtTextChanged(QString) ) );

    // Pick5 min and max prices and weights
    connect(ui->txtMinPrice,        SIGNAL( editingFinished() ),    this,   SLOT(onPriceEditedFinished()));
    connect(ui->txtMinWeight,       SIGNAL( editingFinished() ),    this,   SLOT(onWeightEditedFinished()));
    connect(ui->txtMaxPrice,        SIGNAL( editingFinished() ),    this,   SLOT(onPriceEditedFinished()));
    connect(ui->txtMaxWeight,       SIGNAL( editingFinished() ),    this,   SLOT(onWeightEditedFinished()));

    // forced Field connections
    connect(ui->chkbox2ForcedTare,                    SIGNAL(stateChanged (int)),   this, SLOT(onForcedTareChanged(int)));
    connect(ui->chkbox2ForcedByCount,                 SIGNAL(stateChanged (int)),   this, SLOT(onForcedByCountChanged(int)));
    connect(ui->chkbox2UnitPriceForcedPrice,          SIGNAL(stateChanged (int)),   this, SLOT(onForcedUnitPriceChanged(int)));
    connect(ui->chkbox2FractionalPricingForcedPrice,  SIGNAL(stateChanged (int)),   this, SLOT(onForcedUnitPriceChanged(int)));
    connect(ui->chkbox2PoundsForPricingForcedPrice,   SIGNAL(stateChanged (int)),   this, SLOT(onForcedUnitPriceChanged(int)));

    // Reporting class connection for single field write to determine validness
    connect(ui->le1ReportingClass,      SIGNAL(editingFinished()), this, SLOT(doReportingClassLookup()));

    // Label Tab connections - need to do a single field write on entering a label to determine validity
    // signal mapper for label fields
    editingFinishedSignal_ = new QSignalMapper( this );
    connect( editingFinishedSignal_,    SIGNAL( mapped(QWidget*) ), SLOT( onEditingFinished(QWidget*) ) );

    // default labels
    editingFinishedSignal_->setMapping( ui->txtDefaultPriLabelType, ui->txtDefaultPriLabelType );
    connect( ui->txtDefaultPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtDefaultSecLabelType, ui->txtDefaultSecLabelType );
    connect( ui->txtDefaultSecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtDefaultTriLabelType, ui->txtDefaultTriLabelType );
    connect( ui->txtDefaultTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // manual labels
    editingFinishedSignal_->setMapping( ui->txtManualPriLabelType, ui->txtManualPriLabelType );
    connect( ui->txtManualPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtManualSecLabelType, ui->txtManualSecLabelType );
    connect( ui->txtManualSecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtManualTriLabelType, ui->txtManualTriLabelType );
    connect( ui->txtManualTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // prepack labels
    editingFinishedSignal_->setMapping( ui->txtPrepackPriLabelType, ui->txtPrepackPriLabelType );
    connect( ui->txtPrepackPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtPrepackSecLabelType, ui->txtPrepackSecLabelType );
    connect( ui->txtPrepackSecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtPrepackTriLabelType, ui->txtPrepackTriLabelType );
    connect( ui->txtPrepackTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // Production Entry labels
    editingFinishedSignal_->setMapping( ui->txtProdEntryPriLabelType, ui->txtProdEntryPriLabelType );
    connect( ui->txtProdEntryPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtProdEntrySecLabelType, ui->txtProdEntrySecLabelType );
    connect( ui->txtProdEntrySecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtProdEntryTriLabelType, ui->txtProdEntryTriLabelType );
    connect( ui->txtProdEntryTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // self serv labels
    editingFinishedSignal_->setMapping( ui->txtSelfServPriLabelType, ui->txtSelfServPriLabelType );
    connect( ui->txtSelfServPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtSelfServSecLabelType, ui->txtSelfServSecLabelType );
    connect( ui->txtSelfServSecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtSelfServTriLabelType, ui->txtSelfServTriLabelType );
    connect( ui->txtSelfServTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // Pick5
    editingFinishedSignal_->setMapping( ui->txtPick5PriLabelType, ui->txtPick5PriLabelType );
    connect( ui->txtPick5PriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtPick5SecLabelType, ui->txtPick5SecLabelType );
    connect( ui->txtPick5SecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtPick5TriLabelType, ui->txtPick5TriLabelType );
    connect( ui->txtPick5TriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    // OnlineOrdering
    editingFinishedSignal_->setMapping( ui->txtOnlineOrderingPriLabelType, ui->txtOnlineOrderingPriLabelType );
    connect( ui->txtOnlineOrderingPriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtOnlineOrderingSecLabelType, ui->txtOnlineOrderingSecLabelType );
    connect( ui->txtOnlineOrderingSecLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->txtOnlineOrderingTriLabelType, ui->txtOnlineOrderingTriLabelType );
    connect( ui->txtOnlineOrderingTriLabelType, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    connect(ui->cb4Graphic1,   SIGNAL(currentIndexChanged(int)), this, SLOT(doValidGraphicLookup1(int)));
    connect(ui->cb4Graphic2,   SIGNAL(currentIndexChanged(int)), this, SLOT(doValidGraphicLookup2(int)));
    connect(ui->cb4Graphic3,   SIGNAL(currentIndexChanged(int)), this, SLOT(doValidGraphicLookup3(int)));
    connect(ui->cb4Graphic4,   SIGNAL(currentIndexChanged(int)), this, SLOT(doValidGraphicLookup4(int)));

    // Text Editor connections
    textEdit = ui->textEditRegion;
    connect(ui->cutButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(cut()));
    connect(ui->copyButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(copy()));
    connect(ui->pasteButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(paste()));
    connect(ui->doneButton,SIGNAL(clicked()),this,SLOT(doneEditing()));

    // only sets the placement - but product defaults saves too.  Im gonna add that here too
    connect(ui->cboPrimaryLabelPlacement, SIGNAL( currentIndexChanged(int) ), SLOT( onPrimaryPlacementChanged(int) ));
    connect(ui->cboSecondLabelPlacement,  SIGNAL( currentIndexChanged(int) ), SLOT( onSecondPlacementChanged(int) ));
    connect(ui->cboThirdLabelPlacement,  SIGNAL( currentIndexChanged(int) ), SLOT( onThirdPlacementChanged(int) ));

    //conenct(ui->textEditRegion, SIGNAL(textChanged()), this, SLOT(textEditorLimitInputToSize()));

    connect(ui->fontComboBox,SIGNAL(activated(QString)),this,SLOT(textFamily(QString)));

    connect(textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(textSelect(bool)));
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->fontSizeComboBox,SIGNAL(activated(QString)),this,SLOT(textSize(QString)));

    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    connect(ui->butReuseDone, SIGNAL(clicked()), this, SLOT(reuseDoneButClicked()));
    // TODD
    //connect(ui->butReuseCopyEdit, SIGNAL(clicked()), this, SLOT(launchReuseReferencePLUGroupBox()));
    connect(ui->butReuseCopyEdit, SIGNAL(clicked()), this, SLOT(reuseCopyEditButClicked()));
    connect(ui->butReuseDelete, SIGNAL(clicked()), this, SLOT(reuseDeleteButClicked()));
    connect(ui->butReuseDeleteAll, SIGNAL(clicked()), this, SLOT(reuseDeleteAllButClicked()));
    connect(ui->butReuseNew, SIGNAL(clicked()), this, SLOT(reuseNewButClicked()));

    //TODD

    //connect(ui->butReuseUse, SIGNAL(clicked()), this, SLOT(launchReuseReferencePLUGroupBox()));
    connect(ui->butReuseUse, SIGNAL(clicked()), this, SLOT(reuseUseButClicked()));

    // COOL connections
    connect(ui->cb7CoolTextShortlist,   SIGNAL(currentIndexChanged(int)),   this,   SLOT(updateCoolShortList(int)));
    connect(ui->cb7CoolCategoryNum,     SIGNAL(currentIndexChanged(int)),   this,   SLOT(updateCoolCategory(int)));
    connect(ui->cb7CoolPretext,         SIGNAL(currentIndexChanged(int)),   this,   SLOT(updateCoolPretext(int)));
    connect(ui->but7ChangeView,         SIGNAL(clicked()),                  this,   SLOT(showFullText()));
    connect(ui->but7CoolTextCreate,     SIGNAL(clicked()),                  this,   SLOT(launchShortListEditor()));
    connect(ui->but7DeleteText,         SIGNAL(clicked()),                  this,   SLOT(deleteCOOL()));

    ///////////////////////////////////////////

    connect(ui->butDeleteProduct, SIGNAL(clicked()), this, SLOT(deleteProductClicked()));

    connect(ui->reuseOkayButton, SIGNAL(clicked()), this, SLOT(useSupportDataFor()));
    connect(ui->reuseCancelButton, SIGNAL(clicked()), this, SLOT(hideReuseReferencePLUGroupBox()));

    // Tab changed connection
    connect(ui->tabChangeProductAllFields, SIGNAL(currentChanged(int)), this, SLOT(tabChangedSlot(int)));

    ui->gbNewBlocker->hide();
    ui->butDeleteProduct->setVisible( true );

    scaleConfigData->scaleModel().toInt() > 1? coolTabIsPresent=true : coolTabIsPresent=false;

    unitSeparator = currencyData->unitSeparator();

    ///////////////////////////////////////////////
    //
    //  VALIDATORS
    //
    ///////////////////////////////////////////////

    // Product Number Validator
    ui->leProductNumber->setValidator(productNumberValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leProductNumber->setDecimalPlaces(0);
    ui->leProductNumber->setInputWrapping(true);
    ui->leProductNumber->setInputTruncation(true);
    productNumberValidator_->setTop(makeValidatorTop(6));

    // Tab 1 Validators
    ui->le1ReportingClass->setValidator(le1ReportingClassValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le1ReportingClass->setDecimalPlaces(0);
    ui->le1ReportingClass->setInputWrapping(true);
    le1ReportingClassValidator_->setTop(makeValidatorTop(6));

    switchToMainTab = true;
    previousPLUItemNumber = "";
    ui->lbl2UnitPriceUnitSymbol->setVisible( false);                // dont need to display a unit when we first come here.

    ui->watermark->hide();

    if(supervisorMode_->cameFromLabelPlacementView())
    {
        // had to move this up here bc it needs to happen before the initialize()
        this->ui->tabChangeProductAllFields->setCurrentIndex(3);
        this->ui->tabSubLabelTypes->setCurrentIndex(1);

        // Global saved value - saved here from the Label Viewer App
        int primaryLabelPlacement = session_->scaleConfig->primaryLabelPlacement;
        int secondyLabelPlacement = session_->scaleConfig->secondaryLabelPlacement;
        //int thirdyLabelPlacement  = currentPLUData->thirdLabelPlacement();                  // get from PLU bc this didnt go through the Label Viewer App

        int primaryLabelRotation = session_->scaleConfig->primaryLabelRotation;
        int secondyLabelRotation = session_->scaleConfig->secondaryLabelRotation;
        //int thirdyLabelRotation  = currentPLUData->thirdLabelRotation();                    // get from PLU bc this didnt go through the Label Viewer App

        if ( primaryLabelPlacement > 2 )
            primaryLabelPlacement = 2;                                                      // SET TO UPPER RIGHT if out of range

        // take the vals from the Label Viewer and save them to the local DOM (currentPLU)
        currentPLUData->primaryLabelPlacementRaw() = QString::number( primaryLabelPlacement );      // save placement
        currentPLUData->secondLabelPlacementRaw()  = QString::number( secondyLabelPlacement );

        currentPLUData->primaryLabelRotationRaw()  = QString::number( primaryLabelRotation  );      // save rotation
        currentPLUData->secondLabelRotationRaw()   = QString::number( secondyLabelRotation  );

        // Save the data to the db
        saveEntireProduct();

        supervisorMode_->setCameFromLabelPlacementView( false );
        supervisorMode_->setCameFromChangeProductAllFields( false );
    }

    realInitialize();
    initialize();

    ui->te1Description->installEventFilter(this);
    ui->te5CookingInstructions->installEventFilter(this);
    ui->te5ExpandedText->installEventFilter(this);
    ui->te5ExpandedText2->installEventFilter(this);
    ui->te4Nutrifacts->installEventFilter(this);
    if (session_->isServiceCounter())
        ui->te5ProductMarquee->installEventFilter(this);
    ui->te5ProductNote->installEventFilter(this);
    ui->te5Recipes->installEventFilter(this);
    ui->te5SpecialMessage->installEventFilter(this);
    if (session_->isServiceCounter())
        ui->te5SuggestiveSelling->installEventFilter(this);
    ui->teReuse->installEventFilter(this);
    ui->te5Allergens->installEventFilter(this);

    initializationComplete = true;                                  // now we are here, prevent some things from repeating


    if(supervisorMode_->cameFromProductMarqueeEditor())
    {
        //show the reuse screen
        this->ui->tabChangeProductAllFields->setCurrentIndex(4);
        if (session_->isServiceCounter())
            this->ui->teReuse->setText(ui->te5ProductMarquee->toPlainText());
        //frameReuseText
        this->ui->frameReuseText->raise();
        this->ui->frameReuseText->show();

        // FT-1033
        restoreReuseTextType = "productMarquee";
    }
    else if (supervisorMode_->cameFromProductNutritionEditor())
    {
        //show the reuse screen
        this->ui->tabChangeProductAllFields->setCurrentIndex(3);

        // FT-862: moved within if statement
        if(currentPLUData->itemNumber() > 0)
            session_->send(ProductDev, ProductReadSupportData, currentPLUData->itemNumber() + "," + QString::number(OperateMode::Nutrifacts));

        // FT-1033
        restoreReuseTextType = "nutrifacts";
    }
    // FT-902: refresh plu info
    else if ( currentPLUData->itemNumber().toInt() > ProductType::PRODUCTTYPE_UNKNOWN )
    {
        session_->send(ProductDev, ProductRead, currentPLUData->itemNumber());

    }


    //ui->fontComboBox->
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("cursor.pcf"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Fixed"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Liberation Mono"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Monospace"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Quantum"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Marquee12"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("FreeSerif"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("SPLarge"));

    translate();

    // HTs
    // these features are disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_FPP ) )
    {
        ui->lblShelfLife_3->setEnabled( false );
        ui->cb3ProductionPlanningType->setEnabled( false );
    }

    LOG_INFO("Constructor END", NULL);
}

int superChangeProductAllFields::indexOfFontName(QString fontName)
{
    // Hobart wants to separately list SPLarge away from its font family SP so this routine is needed to accomodate

    int index = 0;


    index = ui->fontComboBox->findText(fontName);

    // FT-977 Hobart font defaults back to Abalone when Default Label Type is attached to PLU
    // only do this if Sp is not found
    if( -1 == index && fontName == "Sp" )
    {
        // lets try "SP" instead of "Sp"
        fontName = "SP";                            // back to the original
        index = ui->fontComboBox->findText(fontName);
    }

    return index;
}

superChangeProductAllFields::~superChangeProductAllFields()
{
    showAllFieldsReuseMarquee_ = false;
    showAllFieldsReuseNutrifacts_ = false;
    supervisorMode_->setCameFromProductMarqueeEditor(false);
    supervisorMode_->setCameFromProductNutritionEditor(false);
    supervisorMode_->setCameFromLabelPlacementView(false);
    delete ui;
}

void superChangeProductAllFields::reuseDoneButClicked()
{
    ui->teReuse->setText("");
    ui->teListOfProductsThatUseThisText->setText("");
    ui->frameReuseText->lower();
    ui->frameReuseText->hide();
    this->reuseTextType = "";
    this->aspSupportPLU = "";
    this->aspSupportText = "";
    this->aspSupportText2 = "";
    this->aspSupportText3 = "";
    this->aspSupportText4 = "";
    this->aspSupportText5 = "";
    this->aspUsedByMany = "";

    this->getTextsFromBackend();
}

int superChangeProductAllFields::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void superChangeProductAllFields::reuseCopyEditButClicked()
{
    aspTextFunction = "2";

    DomPluSupport m(reuseMessageVar);

    QString msg;
    msg += currentPLUData->itemNumber() + "," + QString::number(m.textType()) + ",1," + aspTextFunction + ui->leReuseProductNumber->text() + ",";

    switch (m.textType())
    {
    case 1:
        msg.append(formattedExpandedText);
        break;
    case 2:
        msg.append(formattedSpecialMessage);
        break;
    case 3:
        msg.append(formattedProductNote);
        break;
    case 4:
        msg.append(formattedProductMarquee);
        break;
    case 5:
        msg.append(formattedCookingInstructions);
        break;
    case 6:
        msg.append(formattedExpandedText2);
        break;
    case 7:
        msg.append(formattedSuggestiveSelling);
        break;
    case 8:
        msg.append(formattedAllergens);
        break;
    case 9:
        msg.append(formattedRecipes);
        break;
    case 20:
        msg.append(formattedNutrifacts);
        break;
    }
    launchReuseReferencePLUGroupBox();
}

void superChangeProductAllFields::reuseDeleteButClicked()
{
    aspTextFunction = "4";
    QString msg;
    msg = currentPLUData->itemNumber() + "," + aspTextType + "," + aspUsedByMany + "," + aspTextFunction + "," + aspSupportPLU + ", " ;
    session_->send(ProductDev, ProductChangeSupportData, msg);
    session_->send(ProductDev, ProductSave);

    ui->teReuse->setText("");
    ui->teListOfProductsThatUseThisText->setText("");
    //pluEmitted = 0;
}

void superChangeProductAllFields::reuseDeleteAllButClicked()
{
    aspTextFunction = "5";
    QString msg;
    msg = currentPLUData->itemNumber() + "," + aspTextType + "," + aspUsedByMany + "," + aspTextFunction + "," + aspSupportPLU + ", " ;
    session_->send(ProductDev, ProductChangeSupportData, msg);
    session_->send(ProductDev, ProductSave);
    ui->teReuse->setText("");
    ui->teListOfProductsThatUseThisText->setText("");
}

void superChangeProductAllFields::launchReuseReferencePLUGroupBox()
{
    DomPluSupport m(reuseMessageVar);

    if(aspTextFunction == "3")
    {
        // use
        ui->lblEnterProductToUseQuestion->setText(tr("Enter product number to USE"));
    }
    else if(aspTextFunction == "2")
    {
        // copy and edit
        ui->lblEnterProductToUseQuestion->setText(tr("Enter PLU to COPY & USE"));
    }

    ui->gbReuseBlocker->move( scalePointForScreen( QPoint( 0, 0 ) ) );
    ui->gbReuseBlocker->raise();
    ui->gbReuseBlocker->show();
}

void superChangeProductAllFields::hideReuseReferencePLUGroupBox()
{
    ui->gbReuseBlocker->move( scalePointForScreen( QPoint( 0, 590 ) ) );
    ui->gbReuseBlocker->lower();
    ui->gbReuseBlocker->hide();
}

void superChangeProductAllFields::useSupportDataFor()
{
    int requestThisText = ui->leReuseProductNumber->text().toInt();

    if ( ui->leReuseProductNumber->text() == currentPLUData->itemNumber())
    {
        ui->gbReuseBlocker->lower();
        ui->gbReuseBlocker->hide();
        return;                                                     // can't request the same thing again   //NOTE: But you can't leave the panel up you have to close it so scale isn't locked up on this modal window.
    }

    suppTextEmitted = requestThisText;

    // FT-603: prevent bad data from being sent to application
    if ( requestThisText <= 0 )
     {
        ui->gbReuseBlocker->lower();
        ui->gbReuseBlocker->hide();
        return;
    }

    reuseReferencePLUNumber = ui->leReuseProductNumber->text();
    //      current - texttype - used by many - textfunction-     plu2Use
    //101,11,5854,1,0,3,5852,GUH

    // text function 3
    DomPluSupport m(reuseMessageVar);

    QString msg;
    msg = currentPLUData->itemNumber() + "," + QString::number(m.textType()) + ",0," + aspTextFunction + "," + reuseReferencePLUNumber + ",";

    //if(aspTextFunction != "2")
    //{
      //  msg.append("\"");
    switch (m.textType())
    {
    case 1:
        msg.append(formattedExpandedText);
        break;
    case 2:
        msg.append(formattedSpecialMessage);
        break;
    case 3:
        msg.append(formattedProductNote);
        break;
    case 4:
        msg.append(formattedProductMarquee);
        break;
    case 5:
        msg.append(formattedCookingInstructions);
        break;
    case 6:
        msg.append(formattedExpandedText2);
        break;
    case 7:
        msg.append(formattedSuggestiveSelling);
        break;
    case 8:
        msg.append(formattedAllergens);
        break;
    case 9:
        msg.append(formattedRecipes);
        break;
    case 20:
        msg.append(formattedNutrifacts);
        break;
    }

      //  msg.append("\"");
   // }
    session_->send(ProductDev, ProductChangeSupportData, msg);
    ui->leReuseProductNumber->setText("");
    ui->gbReuseBlocker->lower();
    ui->gbReuseBlocker->hide();
}

void superChangeProductAllFields::reuseNewButClicked()
{
    if(reuseTextType == "productMarquee")
    {
        if ( mainWindow_->isSupervisorMode())
        {
            goingToMarqueeScreen = true;
            mainWindow_->switchToModeScreen<SupervisorMode, ProductMarqueeEditorScreen>();
        }
    }
    else
    {
        DomPluSupport m(reuseMessageVar);
        aspTextFunction = "1";
        QString msg;
        msg = currentPLUData->itemNumber() + "," + QString::number(m.textType()) + ",0,1,,";
        session_->send(ProductDev, ProductChangeSupportData, msg);

        // Send session to get printable area
        session_->send(ProductDev, ProductReadLabelFormSize, currentPLUData->itemNumber() + "," + m.textTypeRaw());
    }
}

void superChangeProductAllFields::reuseUseButClicked()
{
    aspTextFunction = "3";
    launchReuseReferencePLUGroupBox();
}

void superChangeProductAllFields::onPlacementViewClicked()
{
    if(session_->isPrepack())
    {
        goingToLabelPlacementView = true;

        if( supervisorMode_ )
        {
            // save the currentProduct into the ScaleConfig values
            session_->scaleConfig->primaryLabelPlacement   = currentPLUData->primaryLabelPlacement();
            session_->scaleConfig->secondaryLabelPlacement = currentPLUData->secondLabelPlacement();
            session_->scaleConfig->primaryLabelRotation    = currentPLUData->primaryLabelRotation();
            session_->scaleConfig->secondaryLabelRotation  = currentPLUData->secondLabelRotation();
            supervisorMode_->setCameFromChangeProductAllFields( true );
        }

        mainWindow_->switchToModeScreen<SupervisorMode, superLabelPlacementView>();
    }
}

void superChangeProductAllFields::deleteProductClicked()
{
    //Send: 103,101,101026,item#

    QString msg;
    msg = QString::number(101026) + ",1";
    session_->send(Error, ErrProductDevice, msg);
    pluEmitted = 0;
}

////////// COOL Tab slot methods ///////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::updateCoolShortList(int newIndex)
{
    Q_UNUSED( newIndex );

    //if ( ignoreSignals_ )
    //    return;

    //	PTR 782: Send a WriteSingleField msg to update the default COOL, in case that COOL Text
    //			 is selected in order to delete it.
    if (ui->cb7CoolTextShortlist->currentIndex() >= 0)
    {
        QString oldValue = currentPLUData->coolDefaultTextNumRaw();

        // FT-581 The HLX set the new COOL text as the default.  The code was attempting to follow the
        // same functionality here.  However cb7CoolTextShortlist was already being cleared and
        // coolDefaultTextNumRaw() ended up being set to "".
        // so had to save off the new COOL text ID and use it here to set it in the currentPLUData object
        if( newCOOLTextAddedID != "")
        {
            currentPLUData->coolDefaultTextNumRaw() = newCOOLTextAddedID;
            newCOOLTextAddedID = "";
        }
        else
        {
            currentPLUData->coolDefaultTextNumRaw() =  ui->cb7CoolTextShortlist->itemData( ui->cb7CoolTextShortlist->currentIndex()).toString();
        }

        //QString msg = currentPLUData->itemNumber() + ",78," + currentPLUData->coolDefaultTextNumRaw() + "," + oldValue;
        QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetProdCoolDefaultTextNum ) + "," + currentPLUData->coolDefaultTextNumRaw() + "," + oldValue;
        session_->send(ProductDev, ProductWriteSingleField, msg);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::updateCoolCategory(int newIndex)
{
    //if ( ignoreSignals_ )
    //    return;

    //ignoreSignals_ = true;

    // selected a new index in cb7CoolCategoryNum

    QString currentCategoryNumber = ui->cb7CoolCategoryNum->itemData(newIndex).toString();

    currentPLUData->coolCategoryNumRaw() = currentCategoryNumber;

    //	Remove all elements in the list, except the None option, which is first
    // any time a new category is selected, Pretext has to be cleared and re-requested for the chosen category

    // No idea why, but using clear, on this combo, would not clear it.
    // tried a bunch of different tests and it just would not clear.
    // This works.

    ui->cb7CoolPretext->blockSignals(true);
/*
    for(int j = 0; j < ui->cb7CoolPretext->count(); j++)
        ui->cb7CoolPretext->removeItem(0);
*/
    ui->cb7CoolPretext->clear();
    ui->cb7CoolPretext->addItem(tr("None"), "0");
    ui->cb7CoolPretext->setCurrentIndex( 0 );

    update();

    //	135,78 - read all new pretexts for a given cool category selected from the combo box
    session_->send(COOLDev, COOLDevReadByAllAndByCategoryPretexts, currentPLUData->coolCategoryNumRaw() );

    ui->cb7CoolPretext->blockSignals(false);
    //ignoreSignals_ = false;                                         // needed to protect this on the clear
}

void superChangeProductAllFields::updateCoolPretext(int newIndex)
{
    //if ( ignoreSignals_ )
    //    return;

    QString pretextNumber = ui->cb7CoolPretext->itemData(newIndex).toString();
    currentPLUData->coolPretextNumRaw() = pretextNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::showFullText()
{
    int index = ui->cb7CoolTextShortlist->currentIndex();                       // currentIndex of item selected
    QString theKey = ui->cb7CoolTextShortlist->itemData(index).toString();        // the key associated with the current item
    int shortListRecord = 0;

    for(int j = 0; j < shortListNumList.count(); j++)
    {
        if(theKey == shortListNumList.at(j))
        {
            shortListRecord = j;
        }
    }

    if( isFullMode )
    {
        isFullMode = false;

        ui->te7ViewCoolText->setText( "" );
        ui->te7ViewCoolText->setVisible( false);
        ui->cb7CoolTextShortlist->setVisible( true );

        ui->but7ChangeView->setText(tr("Full view"));
    }
    // FT-860: crash fix
    else if ( shortListRecord < shortListNamesList.size() )
    {
        isFullMode = true;
        QString coolStr = shortListNamesList.at(shortListRecord);
        coolStr = coolStr.replace("\"", "");
        ui->cb7CoolTextShortlist->setVisible( false );
        ui->te7ViewCoolText->setVisible( true);
        ui->te7ViewCoolText->setText( coolStr );

        ui->but7ChangeView->setText(tr("Exit view"));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::launchShortListEditor()
{
    // clicked "create cool" button

    // FT-581
    newCOOLTextAddedID = "";

    // FT-1168: fix memory leak
    if ( shortListEditor_ )
        delete shortListEditor_;

    shortListEditor_ = new hobart::ShortListEditor(this, mainWindow_);
    shortListEditor_->move( scalePointForScreen( QPoint( 28, 10 ) ) );
    scaleWidget(shortListEditor_);
    shortListEditor_->show();

    if( isFullMode )
        showFullText();                                             // force reset of field view

    // listening for SIGNALS from the ShortList editor
    connect(shortListEditor_, SIGNAL( shortListEditorDoneClicked( bool ) ),      SLOT( onShortListEditorDoneClicked( bool )) );
    connect(shortListEditor_, SIGNAL( shortListEditorCancelClicked() ),    SLOT( onShortListEditorCancelClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::deleteCOOL()
{
    // clicked "delete cool" button

    QString recordNum = "";
    int index = ui->cb7CoolTextShortlist->currentIndex();

    if ((index >= 0) && (tr("None") != ui->cb7CoolTextShortlist->itemText(index)))
    {
        recordNum =  ui->cb7CoolTextShortlist->itemData(index).toString();
        ui->cb7CoolTextShortlist->removeItem(index);

        session_->send(COOLDev, COOLDevDeleteCoolText, recordNum );            // 135, 104 - tell app who to delete

        if (index ==  ui->cb7CoolTextShortlist->count() )
            --index;

        if ( index >= 0 )
            ui->cb7CoolTextShortlist->setCurrentIndex(index);
        else
        {
            ui->cb7CoolTextShortlist->blockSignals(true);
            ui->cb7CoolTextShortlist->clear();
            ui->cb7CoolTextShortlist->addItem(tr("None"));
            ui->cb7CoolTextShortlist->blockSignals(false);
        }

        //	Delete the COOL text record from the Product's shortlist.
        QString msg = currentPLUData->itemNumber();     				//	Product number
        msg += "," + recordNum;											//	Cool text number
        session_->send(COOLDev, COOLDevShortlistDeleteText, msg);       //	135, 116
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onShortListEditorDoneClicked(bool didAddRecord)
{
    // Just came back from clicking Done on the Create New Cool Widget.
    // didAddRecord indicates if we added a new record. If we did, add
    // it to the combo box, and then select it.  The record was already
    // saved to the CurrentPLU and also a save message was sent to the APP

    if(didAddRecord)
    {
        // FT-581 - Save the ID of the new COOL Text
        newCOOLTextAddedID = currentPLUData->coolDefaultTextNumRaw();

        // reinitialize the COOL tab
        isFullMode = false;
        ui->cb7CoolPretext->blockSignals(true);
        ui->cb7CoolTextShortlist->blockSignals(true);

        ui->cb7CoolPretext->addItem(tr("None"), "0");

        ui->cb7CoolTextShortlist->clear();
        ui->cb7CoolTextShortlist->addItem(tr("None"));

        ui->cb7CoolPretext->blockSignals(false);
        ui->cb7CoolTextShortlist->blockSignals(false);

        session_->send(COOLDev, COOLDevReadAllPretexts );
    }

    // FT-656
    // FT-1168: fix memory leak
    if ( shortListEditor_ )
    {
        shortListEditor_->deleteLater();
        shortListEditor_ = NULL;
    }
}

void superChangeProductAllFields::onPrimaryPlacementChanged(int newIndex)
{
	LOG_CLASS_INFO( "superChangeProductAllFields", "onPrimaryPlacementChanged" );
    Q_UNUSED( newIndex );
    
	ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );

    // Check what the DOM values are coming in
    LOG_DEBUG( "onPrimaryPlacementChanged() walking in currentPLUData->primaryLabelPlacementRaw() is: %s", qPrintable(currentPLUData->primaryLabelPlacementRaw()) );
    LOG_DEBUG( "onPrimaryPlacementChanged() walking in currentPLUData->secondaryLabelPlacementRaw() is: %s", qPrintable(currentPLUData->secondLabelPlacementRaw()) );

    // update the primary DOM
    int test_priLabLoc =  ui->cboPrimaryLabelPlacement->currentData().toInt();    
    currentPLUData->primaryLabelPlacementRaw() = QString::number(test_priLabLoc);

	// saved the data after changing the primary, a change forces the secondary to update the available options - so repopulate secondary based on primary
    populateSecondaryPlacementComboBox();

	// ok so we repopulated the fields, now set the secondary back to what it was
    int primaryData  = currentPLUData->primaryLabelPlacement();
    //int secondaryData = currentPLUData->secondLabelPlacement();

    // ok lets reset the secondary now
    if ( primaryData == NONE )
    {
        // The Primary to NONE so leave secondary alone - do not reset DOM
        ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( currentPLUData->secondLabelPlacementRaw() ) );
    }
    else if( primaryData >= UPPER_RIGHT )
    {
        // if we have Left or Right of Primary, and we move from Top LEft to top Right, we might be creating a weird edge case, so test is, and fix it
        // Otherwise leave where it is and force operator to update secondary
        int primaryIndex = currentPLUData->primaryLabelPlacement();
        int secondIndex  = currentPLUData->secondLabelPlacement();

        // These are both cases where the new primary index, eliminates the existing secondary as unselectable. So autoswith it with the move across
        if ( ( primaryIndex == UPPER_RIGHT ) && ( secondIndex == RIGHT_OF_PRIMARY) )
        {
            currentPLUData->secondLabelPlacementRaw() = QString::number(LEFT_OF_PRIMARY);
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( currentPLUData->secondLabelPlacementRaw() ) );
        }
        else if ( ( primaryIndex == UPPER_LEFT ) && ( secondIndex == LEFT_OF_PRIMARY ) )
        {
            currentPLUData->secondLabelPlacementRaw() = QString::number(RIGHT_OF_PRIMARY);
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( currentPLUData->secondLabelPlacementRaw() ) );
        }
        else
        {
            ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( currentPLUData->secondLabelPlacementRaw() ) );                         // fail safe but it should never be used
        }
    }
    else
    {
        // Did NOT find index in combo, so index -1, so set and bail
        ui->cboSecondLabelPlacement->setCurrentIndex( currentPLUData->secondLabelPlacement() );

    }

    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );
}

void superChangeProductAllFields::onSecondPlacementChanged(int newIndex)
{
	Q_UNUSED( newIndex );

    // onComboBox Changed
    int test_secLabLoc =  ui->cboSecondLabelPlacement->currentData().toInt();
    currentPLUData->secondLabelPlacementRaw() = QString::number(test_secLabLoc);
}

void superChangeProductAllFields::onThirdPlacementChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    // onComboBox Changed
    int test_triLabLoc =  ui->cboThirdLabelPlacement->currentData().toInt();
    currentPLUData->thirdLabelPlacementRaw() = QString::number(test_triLabLoc);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onShortListEditorCancelClicked()
{
    // TODO - jhc
    // This is a SLOT for when a user clicked CANCEL in the short list editor.
    // Don't have to do anything right now - leaving in for now as future clean up location
    // for when CANCEL is clicked in Short List Editor.

    // May have to reset a class value
    // or reset the combo box. Remove if determined we don't need this anymore.
    // In addition, don't then forget to remove the EMIT in shortListEditor.cpp
    // off of the SIGNAL when the user clicks CANCEL

    // FT-656
    // FT-1168: fix memory leak
    if ( shortListEditor_ )
    {
        shortListEditor_->deleteLater();
        shortListEditor_ = NULL;
    }
}

bool superChangeProductAllFields::eventFilter( QObject *obj, QEvent *event )
{
    QString msg;


    if(ui->te1Description == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            if( isTabMainClicked )
                isTabMainClicked = false;  // can always be set to false here, let tabChange SLOT set it true

            // Reverse comment the next 2 lines for the HTi - Larry changed the textType
            //msg = currentPLUData->itemNumber() + ",30";
            msg = currentPLUData->itemNumber() + ",30";

            reuseTextType = "description";
            session_->send(ProductDev, ProductReadLabelFormSize, msg);
        }
    }
    else if(ui->te5ExpandedText == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "expandedText";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",1";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5SpecialMessage == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "specialMessage";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",2";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5ProductNote == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "productNote";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",3";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5ProductMarquee == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            saveEntireProduct();
            mainWindow_->hideKeyboard();
            reuseTextType = "productMarquee";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",4";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5CookingInstructions == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "cookingInstructions";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",5";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5Recipes == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "recipes";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",6";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5ExpandedText2 == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "expandedText2";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",7";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5SuggestiveSelling == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "suggestiveSelling";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",8";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te5Allergens == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "allergens";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",9";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->te4Nutrifacts == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            //101,9,55,1
            mainWindow_->hideKeyboard();
            reuseTextType = "nutrifacts";
            aspTextFunction = "1";
            msg = currentPLUData->itemNumber() + ",20";
            session_->send(ProductDev, ProductReadSupportData, msg);
            return true;
        }
    }
    else if(ui->teReuse == obj)
    {
        if(QEvent::FocusIn == event->type())
        {
            DomPluSupport m(reuseMessageVar);

            if(reuseTextType == "productMarquee")
            {
                if ( mainWindow_->isSupervisorMode() )
                {
                    goingToMarqueeScreen = true;
                    mainWindow_->switchToModeScreen<SupervisorMode, ProductMarqueeEditorScreen>();
                }
            }
            else
            {
                if(m.supportText().length() == 0)
                {
                    msg = currentPLUData->itemNumber() + "," + QString::number(m.textType()) + "," + m.usedByMany() + ",1," + m.supportPLU() + "," + m.supportText();
                    session_->send(ProductDev, ProductChangeSupportData, msg);
                }
                // Send session to get printable area
                session_->send(ProductDev, ProductReadLabelFormSize, currentPLUData->itemNumber() + "," + m.textTypeRaw());

            }
        }
    }

    return _Mybase::eventFilter( obj, event );
}

void superChangeProductAllFields::productDescriptionEdit()
{
    // clicked the description field so edit it.
    QString msg;
    //Description changes for the new chobart_d it is now "30" not "10" -> Larry
    //msg = currentPLUData->itemNumber() + ",30";
    msg = currentPLUData->itemNumber() + ",10";
        session_->send(ProductDev, ProductReadLabelFormSize, msg);
}

void superChangeProductAllFields::saveEntireProduct()
{
    /**
     * Save the entire currentPLU Data
     */

    // FT-731: save product data before exiting to marquee
    //if(!goingToMarqueeScreen)
    {
        saveTabData(tabToSave);                                         // need to update and save the current tab before saving entire record.
        if( supervisorMode_->cameFromLabelPlacementView() )
            return;

        if( (!cameFromCreate && previousPLUItemNumber.toInt() > 0 ) )
        {
            unitSeparator = currencyData->unitSeparator();

            QString msg =  previousPLUItemNumber + ","; /*  itemNumber()  (2);} */                     /* any integer number */
            msg.append((ui->cb1ProductType->itemData(ui->cb1ProductType->currentIndex())).toString() + ","); /*  productType()  (3);} */                  /* RW=1,FW=2,BC=3,FL=4 */
            formattedDescription.replace("\"","\"\"");
            msg.append("\"" + formattedDescription + "\","); /*  description()  (4);} */
            msg.append(QString::number(currentPLUData->barcodeType()) + ",");/*  barcodeType()  (5);     } This doesn't change*/
            msg.append(QString::number(currentPLUData->barcodeFields()) + ","); /*  barcodeFields()  (6);} This doesn't change*/

            // ========== NEW CODE for saving barcodes ==========
            int pluType = ui->cb1ProductType->itemData( ui->cb1ProductType->currentIndex() ).toInt();
            QString thePrefix = ui->txtPrefix->text();          // only one type pf prefix now (there alway was but we faked 2)
            msg.append( thePrefix );
            msg.append(",");

            if( pluType == ProductType::PRODUCTTYPE_RANDOMWEIGHT || pluType == ProductType::PRODUCTTYPE_BYCOUNT )
            {
                // Same as usual
                // UPC Number or Product Number - samethang
                if ( Product_Number_Orig.toInt() != ui->txtUPCNumber->text().toInt() )
                    msg.append( "\"" + ui->txtUPCNumber->text() + "\"" );           // UPC Number
                msg.append(",");

                // Vendor Number
                if ( Vendor_Number_Orig.toInt() != ui->txtUPCVendorNumber->text().toInt() )
                    msg.append( "\"" + ui->txtUPCVendorNumber->text() + "\"");      // Vendor Number
                msg.append(",");
            }
            else
            {
                // Fixed or Fluid weight
                if(  ( Product_Number_Orig.toInt() != ui->txtUPCNumber->text().toInt() ) ||  ( Vendor_Number_Orig.toInt() != ui->txtUPCVendorNumber->text().toInt() ) )
                {
                    // If one of them changes, then automatically save both

                    // UPC Number or Product Number - samethang
                    msg.append( "\"" + ui->txtUPCNumber->text() + "\"" );           // UPC Number
                    msg.append(",");

                    // Vendor Number
                    msg.append( "\"" + ui->txtUPCVendorNumber->text() + "\"");      // Vendor Number
                    msg.append(",");
                }
                else
                {
                    // same as usual
                    // UPC Number or Product Number - samethang
                    if ( Product_Number_Orig.toInt() != ui->txtUPCNumber->text().toInt() )
                        msg.append( "\"" + ui->txtUPCNumber->text() + "\"" );           // UPC Number
                    msg.append(",");

                    // Vendor Number
                    if ( Vendor_Number_Orig.toInt() != ui->txtUPCVendorNumber->text().toInt() )
                        msg.append( "\"" + ui->txtUPCVendorNumber->text() + "\"");      // Vendor Number
                    msg.append(",");
                }
            }

            // EAN Code
            if ( EAN_Code_Orig != ui->txtEANCode->text() )
                msg.append( "\"" + ui->txtEANCode->text() + "\"");              // EAN Code
            msg.append(",");

            // ========== END CODE for saving barcodes ==========

            int tmpByCount = ui->le2ByCount->text().toInt();
            if(tmpByCount < 1)
                currentPLUData->byCountRaw() = "1";                        // if it happens, never let them save a bycount < 1
            else
                currentPLUData->byCountRaw() = ui->le2ByCount->text();
            msg.append(currentPLUData->byCountRaw() + ",");/*  byCount()  (11);} */
            msg.append("\"" + ui->le2ExceptionPrice->text().replace(unitSeparator,".") + "\","); /* exceptionPrice()  (12);} */

            // FT-972 Fractional Prices aren't saving on creation
            // If you are on a different tab, the price data was saved by savePrice.
            if( tabToSave == 1)         // only valid if we are on the pricing tab, otherwise we have already saved it
            {
                if(widgetUsedForPrice == "gb2FractionalPrice" && ui->gb2FractionalPrice->isVisible())
                {
                    QString myFP =  ui->le2FractionalPrice->text();                 // so I can see it
                    currentPLUData->unitPrice() = myFP;
                }
                else if(widgetUsedForPrice == "gb2UnitPrice")
                {
                    currentPLUData->unitPrice() =  ui->le2UnitPrice->text();
                }
                else if(widgetUsedForPrice == "gb2PoundsForPricing" && ui->gb2PoundsForPricing->isVisible())
                {
                    currentPLUData->unitPrice() =  ui->le2PoundsForPrice->text();
                }
            }

            msg.append("\"" + currentPLUData->unitPrice().replace(unitSeparator, ".") + "\",");

            /*  discountType()  (14);} */
            QString discType = currentPLUData->discountType();          // bc it stays the same when it comes in, or I update the DOM if it changes
            msg.append(discType + ",");
            /*  discountValue1()  (15);} */
            if(widgetUsedForDiscountValue1 == "le2RegularPrice")
            {
                msg.append("\"" + ui->le2RegularPrice->text().replace(unitSeparator, ".") + "\",");
            }
            else if(widgetUsedForDiscountValue1 == "le2Discount1")
            {
                msg.append("\"" + ui->le2Discount1->text().replace(unitSeparator, ".") + "\",");
            }

            /*  discountValue2()  (16);} */
            msg.append("\"" + ui->le2Discount2->text().replace(unitSeparator,".") + "\",");

            /*  discountValue3()  (17);} */
            msg.append("\"" + ui->le2Discount3->text().replace(unitSeparator,".") + "\",");

            // There exists no implementation by which these can change - 3/11/2013
// I'm quoting them now [CCD 6/2/14]            // I'm quoting them as per Larry on 5/14/2013 for future customer defined use, but just not now
            msg.append("\"" + currentPLUData->discountLabel1() + "\","); /*  discountLabel1()  (18);} */
            msg.append("\"" + currentPLUData->discountLabel2() + "\","); /*  discountLabel2()  (19);} */
            msg.append("\"" + currentPLUData->discountLabel3() + "\","); /*  discountLabel3()  (20);} */

            // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.
            if( !scaleConfigData->useSecondTare() )
                msg.append("\"" + ui->le2Tare->text().replace(unitSeparator,".") + "\",");/*  tare()  (21);} */
            else
                msg.append("\"" + currentPLUData->tare() + "\",");/*  tare()  (21);} */

            msg.append("\"" + ui->le2ProportionalTare->text().replace(unitSeparator,".") + "\","); /*  proportionalTare()  (22);} */

            // which net weight value to use
            /*  fixedWeight()  (23);} */
            if(widgetUsedForFixedWeight == "le2NetWeight")
            {
                msg.append(ui->le2NetWeight->text().replace(unitSeparator,".") + ",");
            }
            else if(widgetUsedForFixedWeight == "le2NetWeightFluid")
            {
                msg.append(ui->le2NetWeightFluid->text().replace(unitSeparator, ".") + ",");
            }
            else
            {
                msg.append(currentPLUData->fixedWeight() + ",");
            }

            //qDebug() << "Save 1 is: " << msg;
            if (ui->le3ShelfLifeDays->text() == "") msg.append("0,");
            else msg.append(ui->le3ShelfLifeDays->text() + ","); /*  shelfLifeDays()  (24);} */

            if (ui->le3ShelfLifeHours->text() == "") msg.append("0,");
            else msg.append(ui->le3ShelfLifeHours->text() + ","); /*  shelfLifeHours()  (25);} */

            if (ui->le3ProductLifeDays->text() == "") msg.append("0,");
            else msg.append(ui->le3ProductLifeDays->text() + ","); /*  productLifeDays()  (26);} */

            if (ui->le3ProductLifeHours->text() == "") msg.append("0,");
            else msg.append(ui->le3ProductLifeHours->text() + ","); /*  productLifeHours()  (27);} */

            if(widgetUsedForPrice == "gb2PoundsForPricing")
            {
                // values 1-9
                msg.append((ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex())).toString() + ",");
            }
            else if(widgetUsedForPrice == "gb2FractionalPrice")
            {
                // english unit values 25 or 50 or 1, metric unit values are 100 or 1
                msg.append((ui->cb2FractionalPriceEnglishUnit->itemData(ui->cb2FractionalPriceEnglishUnit->currentIndex())).toString() + ",");
            }
            else
            {
                msg.append(currentPLUData->priceModifier() + ",");
            }

            //msg += ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex()).toString() + ","; /*  priceModifier()  (28);} */
            msg.append(ui->le1ReportingClass->text() + ",");                        /*  reportingClass()  (29);} */
            msg.append(",");                                                        /*  dont use primaryLabelTypes() (30);} */
            msg.append(currentPLUData->primaryLabelRotationRaw() + ",");            /*  primaryLabelRotation()  (31);} */
            msg.append(",");                                                        /*  dont use secondLabelTypes()  (32);} */
            msg.append(currentPLUData->secondLabelRotationRaw() + ",");             /*  secondLabelRotation()  (33);} */
            msg.append(",");                                                        /*  dont use thirdLabelTypes()  (34);} */
            msg.append(currentPLUData->thirdLabelRotationRaw() + ",");              /*  thirdLabelRotation()  (35);} */
            msg.append(QString::number(currentPLUData->backDateDays()) + ",");      /*  backDateDays()  (36);} */
            msg.append(ui->le2Points->text().replace(unitSeparator,".") + ",");     /*  points()  (37);} */

            QString tmpGraphic = ui->cb4Graphic1->currentText();                    // for now used to just examining the value before saving
            msg.append(tmpGraphic + ","); /*  graphic1Name()  (38);}                */
            tmpGraphic = ui->cb4Graphic2->currentText();                            // for now used to just examining the value before saving
            msg.append(tmpGraphic + ","); /*  graphic2Name()  (39);}                */
            tmpGraphic = ui->cb4Graphic3->currentText();                            // for now used to just examining the value before saving
            msg.append(tmpGraphic + ","); /*  graphic3Name()  (40);}                */
            tmpGraphic = ui->cb4Graphic4->currentText();                            // for now used to just examining the value before saving
            msg.append(tmpGraphic + ","); /*  graphic4Name()  (41);}                */

            msg.append("\"" + currentPLUData->primaryLabelTypeWidth().replace(unitSeparator,".") + "\",\"");    // + /*  primaryLabelTypeWidth()  (42);} */
            msg.append(currentPLUData->primaryLabelTypeLength().replace(unitSeparator,".") + "\",\"");          /*  primaryLabelTypeLength()  (43);} */
            msg.append(currentPLUData->secondLabelTypeWidth().replace(unitSeparator,".") + "\",\"");            /*  secondLabelTypeWidth()  (44);} */
            msg.append(currentPLUData->secondLabelTypeLength().replace(unitSeparator,".") + "\",\"");           /*  secondLabelTypeLength()  (45);} */
            msg.append(currentPLUData->thirdLabelTypeWidth().replace(unitSeparator,".") + "\",\"");             /*  thirdLabelTypeWidth()  (46);} */
            msg.append(currentPLUData->thirdLabelTypeLength().replace(unitSeparator,".") + "\",");              /*  thirdLabelTypeLength()  (47);} */

            /*  forcedByCount()  (48);} */
            if(ui->chkbox2ForcedByCount->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }

            /*  forcedTare()  (49);} */
            if(ui->chkbox2ForcedTare->isChecked())
            {
                msg.append("1,");
            }
            else msg.append("0,");

            /*  fractionalPriceUnit() (50); */
            if(widgetUsedForPrice == "gb2FractionalPrice")
            {
                QString myFP = (ui->cb2FractionalPriceEnglishUnit->itemData(ui->cb2FractionalPriceEnglishUnit->currentIndex())).toString();
                msg.append( myFP + ",");
            }
            else
            {
                msg.append(QString::number(currentPLUData->poundsForAmount()) + ",");
            }

            /*  poundsForAmount()  (51);} */ /* valid vals: 1-9 */
            if(widgetUsedForPrice == "gb2PoundsForPricing")
            {
                msg.append((ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex())).toString() + ",");
                //msg.append(ui->cb2PriceModifier->itemData(ui->cb2PriceModifier->currentIndex()).toString() + ",");
            }
            else
            {
                msg.append(QString::number(currentPLUData->poundsForAmount()) + ",");
            }



            if( currentPLUData->forcedUnitPrice() || currentPLUData->forcedFractionalPrice() || currentPLUData->forcedPoundsFor() )
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }
            /*  forcedUnitPrice()  (52);} */

            msg.append((ui->cb2TarePlatterTareType->itemData(ui->cb2TarePlatterTareType->currentIndex())).toString() + ",");
            /*  applySecurity()  (54);} */
            if(ui->cbApplySecurityLabel->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }

            /*  dontPrintPackedOn()  (55);} */
            if(ui->cb3DontPrintPackedOn->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }
            //qDebug() << "SAVING is: " << msg;
            /*  dontPrintSellBy()  (56);} */
            if(ui->cb3DontPrintSellBy->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }
            //qDebug() << "SAVING is: " << msg;
            /*  dontPrintBestBefore()  (57);} */
            if(ui->cb3DontPrintBestBefore->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }
            //qDebug() << "SAVING is: " << msg;

            msg.append(currentPLUData->primaryLabelPlacementRaw() + ","); /*  primaryLabelPlacement()  (58);} */
            msg.append(currentPLUData->secondLabelPlacementRaw() + ","); /*  secondLabelPlacement()  (59);} */
            msg.append(currentPLUData->thirdLabelPlacementRaw() + ","); /*  thirdLabelPlacement()  (60);} */

            // fracitonal price value is set in
            if(widgetUsedForPrice == "gb2FractionalPrice")
            {
                QString myFP = ui->le2FractionalPrice->text();      // so I can peek
                msg.append("\"" + myFP + "\",");
            }
            else
            {
                if(currentPLUData->fractionalPrice() == "0")
                {
                    msg.append("0,");
                }
                else
                {
                    msg.append("\"" + currentPLUData->fractionalPrice() + "\",");
                }
            }/*  fractionalPrice()  (61);} */

            /*  forcedFractionalPrice( (62);} */
            if( ui->chkbox2FractionalPricingForcedPrice->isChecked() )
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }

            if(widgetUsedForPrice == "gb2PoundsForPricing")
            {
                msg.append("\"" + ui->le2PoundsForPrice->text().replace(unitSeparator,".") + "\",");/*  poundsForPrice()  (63);} */
            }
            else
            {
                if(currentPLUData->poundsForPrice() == "0")
                {
                    msg.append("0,");
                }
                else
                {
                    msg.append("\"" + currentPLUData->poundsForPrice() + "\",");
                }
            }

            /*  forcedPoundsFor()  (64);}*/
            if(ui->chkbox2PoundsForPricingForcedPrice && ui->chkbox2PoundsForPricingForcedPrice->isChecked())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }
            /*  sellByDate()  (65);} */
            //msg.append("\"" + currentPLUData->sellByDateRaw() + "\",");
            msg.append(",");
            /*  sellByTime()  (66);} */
            //msg.append(currentPLUData->sellByTime() + ",");
            msg.append(",");
            /*  bestBeforeDate()  (67);} */
            //msg.append("\"" + currentPLUData->bestBeforeDateRaw() + "\",");
            msg.append(",");
            /*  bestBeforeTime()  (68);} */
            //msg.append("" + currentPLUData->bestBeforeTime() + ",");
            msg.append(",");

            ////// TBD
            msg.append(currentPLUData->hasNutrifacts() + ","); /*  hasNutrifacts()  (69);} */
            ////// TBD

            msg.append(currentPLUData->intakeMode() + ","); /*  intakeMode()  (70);} */

            /*  hasFreqShopper()  (71);} */
            if(currentPLUData->hasFreqShopper())
            {
                msg.append("1,");
            }
            else
            {
                msg.append("0,");
            }

            /*  productGraphic()  (72);} */
            if(ui->cb6FlashkeyGraphic->currentIndex() < 1)
            {
                msg.append("None,");
            }
            else
            {
                // HA-1: use combo data, not text
                QString tmpFlashkey( ui->cb6FlashkeyGraphic->itemData( ui->cb6FlashkeyGraphic->currentIndex() ).toString() );        // for now used to just examining the value before saving
                msg.append(tmpFlashkey + ",");
            }

            /*  productName1()  (73);} */
            msg.append("\"" + ui->le6PrimaryLanguageCaption->text() + "\",");

            /*  productName2()  (74);} */
            msg.append("\"" + ui->le6SecondaryLanguageCaption->text() + "\",");

            /*  categoryNumber()  (75);} */
            msg.append((ui->cb6Category->itemData(ui->cb6Category->currentIndex())).toString() + ",");

            /*  specialType()  (76);} */
            int spIndex = ui->cb6TypeOfSpecial->currentIndex();
            QString spNum = specialNumbersList.at(spIndex);
            msg.append(spNum + ",");

            /*  coolCategoryNum()  (77);} */
            msg.append(currentPLUData->coolCategoryNumRaw() + ",");

            /*  coolPretextNum()  (78);} */
            msg.append(currentPLUData->coolPretextNumRaw() + ",");

            /*  coolDefaultTextNum()  (79);} */
            msg.append(currentPLUData->coolDefaultTextNumRaw() + ",");

            /*  coolTextNum()  (80);} */
            msg.append(currentPLUData->coolTextNumRaw() + ",");

            /*  coolShortListNum()  (81);} */
            msg.append(currentPLUData->coolShortListNumRaw() + ",");

            /*  forcedCool()  (82);} */
            msg.append((currentPLUData->forcedCool()?"1":"0"));
            msg.append(",");

            /*  coolTrackingText()  (83);} */
            msg.append("\"" + currentPLUData->coolTrackingText() + "\",");

            /*  coolRequired()  (84);} */
            msg.append( (currentPLUData->coolRequired()?"1":"0") );
            msg.append(",");

            // Note in superSaveDeletePLU.asp states // Don't care about returning these values below
            /*  keyboardNetWeight()  (85);} */
            msg.append(",");

            /*  acceptKbdNetWeight()  (86);} */
            msg.append(",");

            /*  numOfLabelsForced()  (87);} */
            msg.append(",");

            /*  numOfLabels()  (88);} */
            msg.append(",");

            /*  productionPlanningType()  (89);} */
            msg.append(QString::number(ui->cb3ProductionPlanningType->currentIndex()) + ",");

            // Note in superSaveDeletePLU.asp states // Don't care about returning these values below
            /*  forcedBackDateDays()  (90);} */
            msg.append(",");

            /*  expirationDaysForced()  (91);} */
            msg.append(",");

            /*  expirationDays()  (92);} */
            msg.append(",");

            // ONLY SAVE EAN INFO on Product Defaults - EAN format String - but to save it in the wrong slot seems wrong.
            /*  eanFormatString()  (93);} */
            msg.append(",");

            /* ean128CformatString */
            // ONLY SAVE EAN INFO on Product Defaults -
            msg.append(",");

            /*  gs1DatabarGTIN()  (95);} */
            msg.append(",");

            /* gs1DatabarAI() */
            msg.append(",");

            // Note in superSaveDeletePLU.asp states // Don't care about returning these values below
            /*  backdateDate()  (97);} */
            msg.append(",");

            /*  portions()  (98);} */
            msg.append(ui->le3Portions->text() + ",");

            // Note in superSaveDeletePLU.asp states // Don't care about returning these values below
            /*  showPortions()  (99);} */
            msg.append(",");

            /*  sideItemsGroupNum()  (100);} */
            msg.append(ui->le3BuildAMeal->text() + ",");

            /*  mealSideItemsFree()  (101);} */
            msg.append(ui->cb3AllSidesFree->isChecked() ? "1" : "0");
            msg.append(",");

            /*  sideItemsIncludedInMeal()  (102);} */
            msg.append(ui->le3NumberSides->text() + ",");

            /*  meatSpecificationSheet()  (103);} */
            msg.append(ui->le3MeatSpecSheet->text() + ",");     ///< Save the meat specification sheet

            // HT-136 Save the text field only into Tare "B" only if using 2nd Tare.  Otherwise use the original value.
            if( scaleConfigData->useSecondTare() )
                msg.append("\"" + ui->le2Tare->text().replace(unitSeparator,".") + "\",");/*  tareB()  (104);} */
            else
                msg.append("\"" + currentPLUData->tareB() + "\",");/*  tareB()  (104);} */

            /*  qrCodeFormatString()  (105);} */
            msg.append(",");

            /* dataMatrixFormatString()     (106);} */
            msg.append(",");

            if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
            {
                currentPLUData->minWeight() =  ui->txtMinWeight->text();
                currentPLUData->maxWeight() =  ui->txtMaxWeight->text();
            }
            else
            {
                currentPLUData->minWeight() =  "0";
                currentPLUData->maxWeight() =  "0"; // safeguards of junk data
            }

            if(  session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
            {
                currentPLUData->minPrice()  =  ui->txtMinPrice->text();
                currentPLUData->maxPrice()  =  ui->txtMaxPrice->text();
            }
            else
            {
                currentPLUData->minPrice()  =  "0";
                currentPLUData->maxPrice()  =  "0";
            }

            msg.append("\"" + currentPLUData->minWeight().replace(unitSeparator, ".") + "\",");
            msg.append("\"" + currentPLUData->maxWeight().replace(unitSeparator, ".") + "\",");
            msg.append("\"" + currentPLUData->minPrice().replace(unitSeparator, ".") + "\",");
            msg.append("\"" + currentPLUData->maxPrice().replace(unitSeparator, ".") + "\",");

            msg.append(ui->txtDefaultPriLabelType->text() + ",");    /*  primaryDefaultLabelType()  */
            msg.append(ui->txtDefaultSecLabelType->text() + ",");    /*  secondDefaultLabelType()   */
            msg.append(ui->txtDefaultTriLabelType->text() + ",");    /*  thirdDefaultLabelType()   */
            msg.append(ui->txtManualPriLabelType->text() + ",");    /*  primaryManualLabelType()  */
            msg.append(ui->txtManualSecLabelType->text() + ",");    /*  secondManualLabelType()   */
            msg.append(ui->txtManualTriLabelType->text() + ",");    /*  thirdManualLabelType()   */
            msg.append(ui->txtPrepackPriLabelType->text() + ",");   /*  primaryPrepackLabelType() */
            msg.append(ui->txtPrepackSecLabelType->text() + ",");   /*  secondPrepackLabelType() */
            msg.append(ui->txtPrepackTriLabelType->text() + ",");   /*  thirdPrepackLabelType() */
            msg.append(ui->txtProdEntryPriLabelType->text() + ","); /*  primaryProdEntryLabelType() */
            msg.append(ui->txtProdEntrySecLabelType->text() + ","); /*  secondProdEntryLabelType() */
            msg.append(ui->txtProdEntryTriLabelType->text() + ","); /*  thirdProdEntryLabelType()   */
            msg.append(ui->txtSelfServPriLabelType->text() + ",");  /*  primarySelfServLabelType()   */
            msg.append(ui->txtSelfServSecLabelType->text() + ",");  /*  secondSelfServLabelType()   */
            msg.append(ui->txtSelfServTriLabelType->text() + ",");  /*  thirdSelfServLabelType()   */
            msg.append(ui->txtPick5PriLabelType->text() + ",");     /*  primaryPick5LabelType()   */
            msg.append(ui->txtPick5SecLabelType->text() + ",");     /*  secondPick5LabelType()   */
            msg.append(ui->txtPick5TriLabelType->text() + ",");     /*  thirdPick5LabelType()   */
            msg.append(ui->txtOnlineOrderingPriLabelType->text() + ",");     /*  primaryOnlineOrderingLabelType()   */
            msg.append(ui->txtOnlineOrderingSecLabelType->text() + ",");     /*  secondOnlineOrderingLabelType()   */
            msg.append(ui->txtOnlineOrderingTriLabelType->text() + ",");     /*  thirdOnlineOrderingLabelType()   */
            msg.append(ui->txtShelfLifeMinutes->text() + ",");      /*  shelfLifeMinutes()     */
            msg.append(ui->txtProductLifeMinutes->text() + ",");    /*  productLifeMinutes()  */

            //  Keep EANprefix at the end of this list ALWAYS!
            //  EANprefix is for ProductDefaults only. If you want to add something for Product, add it ahead of it. This is not product defaults
            /*  EANprefix()  (123);} */
            msg.append(",");

            // 101,7 rest of full save message
            session_->send(ProductDev, ProductWrite, msg);
            session_->send(ProductDev, ProductSave);
        }
    }
}

void superChangeProductAllFields::saveTabData(int tabIndex)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "saveTabData" );

    // Send single write of the product's fields in the tab
    // 101, 16, itemNumber, offsetindex, newValue, oldValue, canPrint(leave it blank when sending)
    // offset is one less than actual enumeration

    switch (tabIndex)
    {
     case 0: // Main
        // Saving Main tab data

        LOG_DEBUG( "Save Main", NULL );
        SaveMain();
        break;
     case 1: // Pricing
        LOG_DEBUG( "Save Pricing", NULL );
        SavePricing();
        break;
     case 2: // Product Life
        LOG_DEBUG( "Save Product Life", NULL );
        SaveProductLife();
        break;
     case 3: // Labeling
        LOG_DEBUG( "Save Labeling", NULL );
        SaveLabeling();
        break;
     case 4: // Texts
        LOG_DEBUG( "Save Texts", NULL );
        SaveTexts();
        break;
     case 5: // Flashkeys
        LOG_DEBUG( "Save Flashkeys", NULL );
        SaveFlashkeys();
        break;
     case 6: // COOL
        LOG_DEBUG( "Save COOL", NULL );
        SaveCOOL();
        break;
    }
}

void superChangeProductAllFields::SaveMain()
{
    //Write Single Fields
}

void superChangeProductAllFields::SavePricing()
{
    /**********************************************************************************************/
    // At this point lets save any and all price fields that we need to save as single field writes
    // Compare the currentPluData against the ui field to decide to send it.  And if it's visible
    /*********************************************************************************************/

    if(widgetUsedForPrice == "gb2FractionalPrice")
    {
        QString myFP =  ui->le2FractionalPrice->text();                 // so I can see it
        currentPLUData->unitPrice() = myFP;
    }
    else if(widgetUsedForPrice == "gb2UnitPrice")
    {
        currentPLUData->unitPrice() =  ui->le2UnitPrice->text();
    }
    else if(widgetUsedForPrice == "gb2PoundsForPricing" )
    {
        currentPLUData->unitPrice() =  ui->le2PoundsForPrice->text();
    }

    currentPLUData->minPrice()  =  ui->txtMinPrice->text();
    currentPLUData->maxPrice()  =  ui->txtMaxPrice->text();
}

void superChangeProductAllFields::SaveProductLife()
{
    /**
     * Save the Product Life Tab
     */

    currentPLUData->shelfLifeDaysRaw() = ui->le3ShelfLifeDays->text() == "" ? "0" : ui->le3ShelfLifeDays->text();
    currentPLUData->shelfLifeHoursRaw() = ui->le3ShelfLifeHours->text() == "" ? "0" : ui->le3ShelfLifeHours->text();
    currentPLUData->shelfLifeMinutesRaw() = ui->txtShelfLifeMinutes->text() == "" ? "0" : ui->txtShelfLifeMinutes->text();

    currentPLUData->productLifeDaysRaw() = ui->le3ProductLifeDays->text() == "" ? "0" : ui->le3ProductLifeDays->text();
    currentPLUData->productLifeHoursRaw() = ui->le3ProductLifeHours->text() == "" ? "0" : ui->le3ProductLifeHours->text();
    currentPLUData->productLifeMinutesRaw() = ui->txtProductLifeMinutes->text() == "" ? "0" : ui->txtProductLifeMinutes->text();

    currentPLUData->dontPrintPackedOnRaw()   = ui->cb3DontPrintPackedOn->isChecked()   == true ? "1" : "0";
    currentPLUData->dontPrintSellByRaw()     = ui->cb3DontPrintSellBy->isChecked()     == true ? "1" : "0";
    currentPLUData->dontPrintBestBeforeRaw() = ui->cb3DontPrintBestBefore->isChecked() == true ? "1" : "0";

    currentPLUData->portions() = ui->le3Portions->text();
    currentPLUData->productionPlanningTypeRaw() = QString::number( ui->cb3ProductionPlanningType->currentIndex() );

    currentPLUData->sideItemsGroupNumRaw()          = ui->le3BuildAMeal->text();
    currentPLUData->mealSideItemsFreeRaw()          = ui->cb3AllSidesFree->isChecked() ? "1" : "0";
    currentPLUData->sideItemsIncludedInMealRaw()    = ui->le3NumberSides->text();
    currentPLUData->meatSpecificationSheetRaw()     = ui->le3MeatSpecSheet->text();     ///< Save the meat specification sheet
}

void superChangeProductAllFields::SaveLabeling()
{
    //    Applier rotation
    currentPLUData->primaryLabelRotationRaw() = (ui->cboPrimaryApplierRotation->itemData( ui->cboPrimaryApplierRotation->currentIndex() )).toString();
    currentPLUData->secondLabelRotationRaw() = (ui->cboSecondApplierRotation->itemData( ui->cboSecondApplierRotation->currentIndex() )).toString();
    currentPLUData->thirdLabelRotationRaw() = (ui->cboThirdApplierRotation->itemData( ui->cboThirdApplierRotation->currentIndex() )).toString();

    //    Wrapper intake
    currentPLUData->intakeMode() = (ui->cboIntakeMode->itemData( ui->cboIntakeMode->currentIndex() )).toString();

    if( supervisorMode_->cameFromLabelPlacementView() )
    {
        // if coming from the UI App, we just need these rotations, not really the whole structure, just do single field writes and exit

        QString msg;        // plu,offset,newVak,oldVal
        msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetLabelPlacement1 ) + "," + currentPLUData->primaryLabelPlacementRaw() + "," + "2";
        session_->send(ProductDev, ProductWriteSingleField, msg);           // writesinglefield 101,16
        session_->send(ProductDev, ProductSave);

        msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetLabelPlacement2 ) + "," + currentPLUData->secondLabelPlacementRaw() + "," + "2";
        session_->send(ProductDev, ProductWriteSingleField, msg);           // writesinglefield 101,16
        session_->send(ProductDev, ProductSave);

        msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetLabelRotation1 ) + "," + currentPLUData->primaryLabelRotationRaw() + "," + "0";
        session_->send(ProductDev, ProductWriteSingleField, msg);           // writesinglefield 101,16
        session_->send(ProductDev, ProductSave);

        msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetLabelRotation2 ) + "," + currentPLUData->secondLabelRotationRaw() + "," + "0";
        session_->send(ProductDev, ProductWriteSingleField, msg);           // writesinglefield 101,16
        session_->send(ProductDev, ProductSave);
    }
}

void superChangeProductAllFields::SaveTexts()
{

}

void superChangeProductAllFields::SaveFlashkeys()
{

}

void superChangeProductAllFields::SaveCOOL()
{
    // -- Cool tracking number -- //
    currentPLUData->coolTrackingTextRaw() = ui->le7CoolTrackingText->text();

    // -- Cool Category --
    // This was saved to the currentPLU when/if a new value was selected in the combo box. A SLOT method was called
    // and the data was saved, and then a single field write was performed. Therefore it does not need to be saved here.
    // See updateCoolCategory() for saving data there if needed

    // -- Cool Pretext -- //
    QString pretextNumber = ui->cb7CoolPretext->itemData( ui->cb7CoolPretext->currentIndex() ).toString();
    currentPLUData->coolPretextNumRaw() = pretextNumber;

    // -- Forced Cool Verification -- //
    currentPLUData->forcedCoolRaw() = ui->chkb7ForcedCool->isChecked() == true ? "1" : "0";

    // -- Cool is Required -- //

    currentPLUData->coolRequiredRaw() = ui->chkb7CoolRequired->isChecked() == true ? "1" : "0";
    // -- Cool Short List (Default Cool) --//
    // This was saved to the currentPLU when/if a new value was selected in the combo box. A SLOT method was called
    // and the data was saved, and then a single field write was performed. Therefore it does not need to be saved here.
    // See updateCoolShortList() for saving data there if needed

}

void superChangeProductAllFields::tabChangedSlot(int currentTabIndex)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "tabChangedSlot" );

    // FT-375 All Fields: Pressing the Main tab while the keyboard is present puts the user in the Description text editor
    mainWindow_->hideKeyboard();                                    // close any open keyboards
    isTabMainClicked = false;                                       // always set to false, and if tab), then true

    // save previous tab data
    saveTabData(tabToSave);

    if( 0 == currentTabIndex )
    {
        // due to always displaying all barcode fields, we may not have to make this call
        saveEntireProduct();
        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());       // swap out call to doPluLookup()
    }

    if( 0 == currentTabIndex )
    {
        // FT-375 All Fields: Pressing the Main tab while the keyboard is present puts the user in the Description text editor
        // This bug occurs this way: Edit the description on MAIN tab. Click done. Go to COOL tab. Edit tracking number
        // leave keyboard active and dont close it. Go to MAIN tab. Going there takes you immediately to editing the description
        // This will allow is to filter out the message that pops the desription editor
        isTabMainClicked = true;
    }
    else if( 1 == currentTabIndex )
    {
        // just want to guarantee everytime we change to this tab we have the right settings.
        if(scaleConfigData->isScaleMetric())
        {
            if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setVisible( true);
                ui->lbl2UnitPriceUnitSymbol->setText("/kg");
            }
            else
            {
                // determined that we don't need a unit at all on BC or FW
                ui->lbl2UnitPriceUnitSymbol->setVisible( false);
            }
        }
        else
        {
            if(  ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setVisible( true);
                ui->lbl2UnitPriceUnitSymbol->setText("/lb");
            }
            else
            {
                // determined that we don't need a unit at all on BC or FW
                ui->lbl2UnitPriceUnitSymbol->setVisible( false);
            }
        }
    }
    else if(4 == currentTabIndex)
    {
        ui->lblExpandedText->setFocus();
    }

    else if(5 == currentTabIndex)
    {
        // Select the PLU's current category when changing to the Flashkeys tab
        if(currentPLUData->categoryNumber() == "")
        {
            ui->cb6Category->setCurrentIndex(0);
        }
        else
        {
            ui->cb6Category->setCurrentIndex(ui->cb6Category->findData(currentPLUData->categoryNumber()));
        }
    }

    tabToSave = currentTabIndex;

    // COOL select tab -  get extra stuff
    if( currentTabIndex == 6 )
        session_->send(COOLDev, COOLDevReadAllPretexts );           // 135,61 - get the list of Pretexts for the available list

    LOG_TRACE( "tab changed %d", tabToSave );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::writeProductType(int productTypeIndex)
{
    //isupdatelegal, offset, newValue, prevValue, print

    if ( supervisorMode_->cameFromProductNutritionEditor() )
    {
        return;
    }

    if ( supervisorMode_->cameFromLabelPlacementView() )
    {
        return;
    }

    if ( currentPLUData->itemNumber().toInt() == ProductType::PRODUCTTYPE_UNKNOWN )
    {
        return;
    }

    QString msg;
    msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetProductType ) + "," + QString::number(productTypeIndex+1) + "," + QString::number(currentPLUData->productType()) + ",0";

    session_->send(ProductDev, ProductWriteSingleField, msg);           // writesinglefield 101,16
    session_->send(ProductDev, ProductSave);
    session_->send(ProductDev, ProductRead, ui->leProductNumber->text() );
}

void superChangeProductAllFields::newPLUOKButClicked()
{
    // set the current index to zero
    // ui->tabChangeProductAllFields->setCurrentIndex(0);

    // send message to create the PLU
    //101,24,PLU#,3
    switchToMainTab = true;
    QString msg = ui->leProductNumber->text() + ",";

    if(ui->rbRandomWeight->isChecked())
    {
        msg += "1";
    }
    else if(ui->rbFixedWeight->isChecked())
    {
        msg += "2";
    }
    else if(ui->rbByCount->isChecked())
    {
        msg += "3";
    }
    else if(ui->rbFluidOunces->isChecked())
    {
        msg += "4";
    }

    cameFromCreate = true;
    switchToMainTab = true;
    session_->send(ProductDev, ProductCreate, msg);

    ui->gbNewBlocker->hide();
    ui->butDeleteProduct->setVisible( true );

    cameFromCancel = false;
    popupActive = false;
}

void superChangeProductAllFields::newPLUCancelButClicked()
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "newPLUCancelButClicked" );

    cameFromCancel = true;
    // just hide the groupbox blocker
    ui->leProductNumber->setText(previousPLUItemNumber);
    LOG_DEBUG( "Cancel hit", NULL );

    ui->gbNewBlocker->hide();
    ui->butDeleteProduct->setVisible( true );

    cameFromCancel = false;
    popupActive = false;
}

void superChangeProductAllFields::doPluLookup()
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "doPluLookup" );
    LOG_DEBUG( "plu lookup", NULL );

    int requestThisPlu = ui->leProductNumber->text().toInt();

    if(  pluEmitted == requestThisPlu )
        return;                                                     // can't request the same thing again


    pluEmitted = requestThisPlu;


    if(previousPLUItemNumber != ui->leProductNumber->text() && ui->leProductNumber->text() != "" && !cameFromCancel && !popupActive)
    {
        saveEntireProduct();

        if( wroteTransaction && previousPLUItemNumber.toInt() > 0 && requestThisPlu > 0 )
            session_->send(ProductDev, ProductSaveSupervisorTransaction);
        else if( (savefbcTransaction) && ( 1 == currentPLUData->byCount() ) && ( ui->chkbox2ForcedByCount->isChecked() == false) )
            session_->send(ProductDev, ProductSaveSupervisorTransaction);

        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
    }
    else if( hasLabelBeenUpdated )
    {
        saveEntireProduct();
        session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
        hasLabelBeenUpdated = false;
    }

    whoHasFocus = SAV_NONE;
    wroteTransaction = false;
    savefbcTransaction = false;
}

void superChangeProductAllFields::doValidGraphicLookup1(int newGraphic)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "doValidGraphicLookup1" );

    LOG_DEBUG( "lookup Graphic 1", NULL );

    if( newGraphic > 0 )
    {
        QString oldValue =  currentPLUData->graphic1Name();
        currentPLUData->graphic1Name() = ui->cb4Graphic1->currentText();
        //QString msg = currentPLUData->itemNumber() + ",37," + currentPLUData->graphic1Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetGraphicName1 ) + "," + currentPLUData->graphic1Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        session_->send(ProductDev, ProductWriteSingleField, msg);
    }
    else
        currentPLUData->graphic1Name() = ui->cb4Graphic1->currentText();    // always save None graphic
}

void superChangeProductAllFields::doValidGraphicLookup2(int newGraphic)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "doValidGraphicLookup2" );

    LOG_DEBUG( "lookup graphic 2", NULL );

    if( newGraphic > 0 )
    {
        QString oldValue =  currentPLUData->graphic2Name();
        currentPLUData->graphic2Name() = ui->cb4Graphic2->currentText();
        //QString msg = currentPLUData->itemNumber() + ",38," + currentPLUData->graphic2Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetGraphicName2 ) + "," + currentPLUData->graphic2Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        session_->send(ProductDev, ProductWriteSingleField, msg);
    }
    else
        currentPLUData->graphic2Name() = ui->cb4Graphic2->currentText();    // always save None graphic
}

void superChangeProductAllFields::doValidGraphicLookup3(int newGraphic)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "doValidGraphicLookup3" );

    LOG_DEBUG( "lookup graphic 3", NULL );

    if( newGraphic > 0 )
    {
        QString oldValue =  currentPLUData->graphic3Name();
        currentPLUData->graphic3Name() = ui->cb4Graphic3->currentText();
        //QString msg = currentPLUData->itemNumber() + ",39," + currentPLUData->graphic3Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetGraphicName3 ) + "," + currentPLUData->graphic3Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        session_->send(ProductDev, ProductWriteSingleField, msg);
    }
    else
        currentPLUData->graphic3Name() = ui->cb4Graphic3->currentText();    // always save None graphic
}

void superChangeProductAllFields::doValidGraphicLookup4(int newGraphic)
{
    //if( ignoreSignals_ )
    //    return;

    LOG_CLASS_INFO( "superChangeProductAllFields", "doValidGraphicLookup4" );

    LOG_DEBUG( "lookup graphic 4", NULL );

    if( newGraphic > 0 )
    {
        QString oldValue =  currentPLUData->graphic4Name();
        currentPLUData->graphic4Name() = ui->cb4Graphic4->currentText();
        //QString msg = currentPLUData->itemNumber() + ",40," + currentPLUData->graphic4Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetGraphicName4 ) + "," + currentPLUData->graphic4Name() + "," + oldValue;   // rem offset is -1 to the DOM index
        session_->send(ProductDev, ProductWriteSingleField, msg);
    }
    else
        currentPLUData->graphic4Name() = ui->cb4Graphic4->currentText();    // always save None graphic
}

void superChangeProductAllFields::doReportingClassLookup()
{
    int requestThisClass = ui->le1ReportingClass->text().toInt();

    if ( reportingClassEmitted == requestThisClass )
        return;

    reportingClassEmitted = requestThisClass;

    QString oldValue = currentPLUData->reportingClass();
    currentPLUData->reportingClass() = ui->le1ReportingClass->text();
    QString msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetProductReportingClassNum ) + "," + currentPLUData->reportingClass() + "," + oldValue;
    session_->send(ProductDev, ProductWriteSingleField, msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onEditingFinished( QWidget *widget )
{
    QString oldValue = "";
    QString location = "";

    QLineEdit* new_label = (QLineEdit*) widget;

    if( new_label->text() == "" )
        new_label->setText( "0" );


    if( widget == ui->txtDefaultPriLabelType )
    {
        location = QString::number( Product::fieldOffsetDefaultLabelTypeNum1 );      // 111
        oldValue = currentPLUData->primaryLabelTypeRaw();
        currentPLUData->primaryLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtDefaultSecLabelType )
    {
        location = QString::number( Product::fieldOffsetDefaultLabelTypeNum2 );     // 112
        oldValue = currentPLUData->secondLabelTypeRaw();
        currentPLUData->secondLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtDefaultTriLabelType )
    {
        location = QString::number( Product::fieldOffsetDefaultLabelTypeNum3 );     // 113
        oldValue = currentPLUData->thirdLabelTypeRaw();
        currentPLUData->thirdLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtManualPriLabelType )
    {
        location = QString::number( Product::fieldOffsetManualLabelTypeNum1 );      // 114;
        oldValue = currentPLUData->primaryManualLabelTypeRaw();
        currentPLUData->primaryManualLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtManualSecLabelType )
    {
        location = QString::number( Product::fieldOffsetManualLabelTypeNum2 );      // 115;
        oldValue = currentPLUData->secondManualLabelTypeRaw();
        currentPLUData->secondManualLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtManualTriLabelType )
    {
        location = QString::number( Product::fieldOffsetManualLabelTypeNum3 );      // 116;
        oldValue = currentPLUData->thirdManualLabelTypeRaw();
        currentPLUData->thirdManualLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPrepackPriLabelType )
    {
        location = QString::number( Product::fieldOffsetPrepackLabelTypeNum1 );     // 117;
        oldValue = currentPLUData->primaryPrepackLabelTypeRaw();
        currentPLUData->primaryPrepackLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPrepackSecLabelType )
    {
        location = QString::number( Product::fieldOffsetPrepackLabelTypeNum2 );     // 118
        oldValue = currentPLUData->secondPrepackLabelTypeRaw();
        currentPLUData->secondPrepackLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPrepackTriLabelType )
    {
        location = QString::number( Product::fieldOffsetPrepackLabelTypeNum3 );     // 119
        oldValue = currentPLUData->thirdPrepackLabelTypeRaw();
        currentPLUData->thirdPrepackLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtProdEntryPriLabelType )
    {
        location = QString::number( Product::fieldOffsetProdEntryLabelTypeNum1 );   // 120
        oldValue = currentPLUData->primaryProdEntryLabelTypeRaw();
        currentPLUData->primaryProdEntryLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtProdEntrySecLabelType )
    {
        location = QString::number( Product::fieldOffsetProdEntryLabelTypeNum2 );   // 121
        oldValue = currentPLUData->secondProdEntryLabelTypeRaw();
        currentPLUData->secondProdEntryLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtProdEntryTriLabelType )
    {
        location = QString::number( Product::fieldOffsetProdEntryLabelTypeNum3 );   // 122
        oldValue = currentPLUData->thirdProdEntryLabelTypeRaw();
        currentPLUData->thirdProdEntryLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtSelfServPriLabelType )
    {
        location = QString::number( Product::fieldOffsetSelfServLabelTypeNum1 );    // 123;
        oldValue = currentPLUData->primarySelfServLabelTypeRaw();
        currentPLUData->primarySelfServLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtSelfServSecLabelType )
    {
        location = QString::number( Product::fieldOffsetSelfServLabelTypeNum2 );    // 124
        oldValue = currentPLUData->secondSelfServLabelTypeRaw();
        currentPLUData->secondSelfServLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtSelfServTriLabelType )
    {
        location = QString::number( Product::fieldOffsetSelfServLabelTypeNum3 );    // 125
        oldValue = currentPLUData->thirdSelfServLabelTypeRaw();
        currentPLUData->thirdSelfServLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPick5PriLabelType )
    {
        location = QString::number( Product::fieldOffsetPick5LabelTypeNum1 );       // 126;
        oldValue = currentPLUData->primaryPick5LabelTypeRaw();
        currentPLUData->primaryPick5LabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPick5SecLabelType )
    {
        location = QString::number( Product::fieldOffsetPick5LabelTypeNum2 );       // 127
        oldValue = currentPLUData->secondPick5LabelTypeRaw();
        currentPLUData->secondPick5LabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtPick5TriLabelType )
    {
        location = QString::number( Product::fieldOffsetPick5LabelTypeNum3 );       // 128
        oldValue = currentPLUData->thirdPick5LabelTypeRaw();
        currentPLUData->thirdPick5LabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtOnlineOrderingPriLabelType )
    {
        location = QString::number( Product::fieldOffsetOnlineOrderingLabelTypeNum1 );       // 126;
        oldValue = currentPLUData->primaryOnlineOrderingLabelTypeRaw();
        currentPLUData->primaryOnlineOrderingLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtOnlineOrderingSecLabelType )
    {
        location = QString::number( Product::fieldOffsetOnlineOrderingLabelTypeNum2 );       // 127
        oldValue = currentPLUData->secondOnlineOrderingLabelTypeRaw();
        currentPLUData->secondOnlineOrderingLabelTypeRaw() = new_label->text();
    }
    else if( widget == ui->txtOnlineOrderingTriLabelType )
    {
        location = QString::number( Product::fieldOffsetOnlineOrderingLabelTypeNum3 );       // 128
        oldValue = currentPLUData->thirdOnlineOrderingLabelTypeRaw();
        currentPLUData->thirdOnlineOrderingLabelTypeRaw() = new_label->text();
    }

    if( oldValue == new_label->text() )
        return;     // dont want to send if the label number is the same

    QString msg = currentPLUData->itemNumber() + "," + location + "," + new_label->text() + "," + oldValue;   // rem offset is -1 to the DOM index
    session_->send(ProductDev, ProductWriteSingleField, msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::blankEverything()
{
    // Ensure no leftover data
    reuseTextType = "";
    aspTextType = "";
    aspTextFunction = "";
    aspUsedByMany = "";
    aspSupportPLU = "";

    formattedDescription = "";
    formattedExpandedText = "";
    formattedSpecialMessage = "";
    formattedProductNote = "";
    formattedProductMarquee = "";
    formattedNutrifacts = "";
    formattedRecipes = "";
    formattedCookingInstructions = "";
    formattedSuggestiveSelling = "";
    formattedExpandedText2 = "";
    formattedAllergens = "";


    this->ui->te1Description->setText("");
    this->ui->te5ExpandedText->setText("");
    this->ui->te5SpecialMessage->setText("");
    this->ui->te5ProductNote->setText("");
    this->ui->te5Recipes->setText("");
    this->ui->te5CookingInstructions->setText("");
    this->ui->te5SuggestiveSelling->setText("");
    this->ui->te5Allergens->setText("");
    this->ui->te5ExpandedText2->setText("");
    this->ui->te4Nutrifacts->setText("");
    this->ui->te5ProductMarquee->setText("");

    // tab 1
    ui->leProductNumber->setText("");
    // tab 2

    // tab 3
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::initialize()
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "initialize" );

    /**
     * Initialize all tab data
     */

    ifUpdating = true;

    showForcedFields = false;
    savefbcTransaction = false;
    cameFromCreate = false;
    hasLabelBeenUpdated = false;                                    // set to true when any of the first,second,third labels fields are updated
    blankEverything();

    // There is no support for Platter tare in Supervisor mode so don't show it.
    isPlatterTareOn = false;
    switchToMainTab = true;
    isFullMode = false;                                             // COOL bool i14n
    ifUpdating = true;

    tabToSave = 0;
    isTabMainClicked = false;

    scaleConfigData    = &session_->scaleConfig->scaleConfig;
    currencyData       = &session_->scaleConfig->currencySettings;
    currentPLUData     = &session_->scaleConfig->currentPLU;
    internalConfigData = &session_->scaleConfig->internalConfig;
    defaultFontFamily = internalConfigData->defaultFontName;
    defaultFontSize = QString::number(internalConfigData->defaultFontSize);
    useFontChanges = true;                // If this is true, we expect, by default, for their to be font change sequences
    frequentShopperData = &session_->scaleConfig->frequentShopper;

    widgetUsedForPriceModifier  = "";
    widgetUsedForDiscountValue1 = "";
    widgetUsedForFixedWeight      = "";

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

    ui->txtPrefix->clear();
    ui->txtUPCVendorNumber->clear();
    ui->txtUPCProductNumber->clear();
    ui->txtEANCode->clear();
    ui->txtUPCNumber->clear();

    // Forced fields

    int bc_prefix = currentPLUData->numberSystem().toInt();

    if(scaleConfigData->scaleModel().toInt() == 1)
    {
        showForcedFields = false;                                                    // no self servs
    }
    else
    {
        if( ui->txtEANCode->text().toInt() > 0 )
            showForcedFields = true;

        if( 2 == bc_prefix || 4 == bc_prefix )
            showForcedFields = true;
    }

    // blank the top PLU and description
    if(currentPLUData->itemNumber().toInt() < 1)
    {
        ui->leProductNumber->setText("");
        previousPLUItemNumber = "";
        ui->te1Description->setText("");
        ui->le1ReportingClass->setText("");
        governFields(0,false);
        switchToMainTab = true;
        previousPLUItemNumber = "0";
        LOG_DEBUG( "NO PLU and itemNumber %s", currentPLUData->itemNumber().toLatin1().constData() );
    }
    else
    {
        // Set Main tab common PLU data
        ui->leProductNumber->setText(currentPLUData->itemNumber());

        ui->te1Description->setText(getEncodedTextOnly(currentPLUData->description()));
        formattedDescription = currentPLUData->description();
        ui->le1ReportingClass->setText(currentPLUData->reportingClass());
        governFields(0,true);
        previousPLUItemNumber = ui->leProductNumber->text();
        switchToMainTab = false;
        LOG_DEBUG( "YES PLU and itemNumber %s", currentPLUData->itemNumber().toLatin1().constData() );
    }


    // Put us on the first tab
    if(switchToMainTab)
    {
        ui->tabChangeProductAllFields->setCurrentIndex(0);
    }

    // Disable other tabs if no product number

    bool enabled = false; // safety to disable tabs

    if(currentPLUData->itemNumber().toInt() > 0 )
    {
        enabled = true;
    }

    for(int i = 1; i < ui->tabChangeProductAllFields->count(); i++)
    {
        ui->tabChangeProductAllFields->setTabEnabled(i,enabled);
    }

    // Set Measurement System
    isMetric = scaleConfigData->isScaleMetric();

    ui->lblInstruction->setVisible( true );                         // now ALWAYS

    //////////////////////////////////
    // TAB 1 - Main
    //////////////////////////////////

    // PRODUCT TYPES

    if ( txtEANCodeValidator_ )
        delete txtEANCodeValidator_;

    //QRegExp rx( QString("-?\\d{0,30}") );
    QRegExp rx( QString("[0-9]*") );
    txtEANCodeValidator_ = new QRegExpValidator( rx, this );
    ui->txtEANCode->setValidator(txtEANCodeValidator_);

    if(currentPLUData->productType() > ProductType::PRODUCTTYPE_UNKNOWN )
        ui->cb1ProductType->setCurrentIndex(ui->cb1ProductType->findData(currentPLUData->productType()));
    else
        ui->cb1ProductType->setCurrentIndex(0);

    ui->gbNewBlocker->hide();                        // Dialog PLU does not exist. New one?
    ui->butDeleteProduct->setVisible( true );

    QString thePrefix = currentPLUData->numberSystem();          // only one type pf prefix now (there alway was but we faked 2)

    ui->txtPrefix->fromSessionField( thePrefix );
    ui->txtUPCNumber->fromSessionField( currentPLUData->productNumber() );
    ui->txtUPCVendorNumber->fromSessionField( currentPLUData->vendorNumber() );
    ui->txtUPCProductNumber->fromSessionField( currentPLUData->productNumber() );
    ui->txtEANCode->setText( currentPLUData->eanCode() );

        if(currentPLUData->productType() > ProductType::PRODUCTTYPE_RANDOMWEIGHT)
        {
            ui->chkbox2ForcedByCount->show();

            if(widgetUsedForPrice == "gb2UnitPrice")
                ui->chkbox2UnitPriceForcedPrice->show();
            else if(widgetUsedForPrice == "gb2FractionalPrice")
                ui->chkbox2FractionalPricingForcedPrice->show();
            else if(widgetUsedForPrice == "gb2PoundsForPricing")
                ui->chkbox2PoundsForPricingForcedPrice->show();
        }

    // HT-136: Change label if using 2nd Tare (label_9 is no longer marked as translatable)
    QString lblTare( scaleConfigData->useSecondTare() ? tr("Tare") + " 2"  : tr("Tare") );
    ui->lblTare->setText( lblTare );

    //////////////////////////////////
    // TAB 2 - Pricing
    //////////////////////////////////
    this->loadPricingTab(currentPLUData->productType());

    //////////////////////////////////
    // TAB 3 - Product Life
    //////////////////////////////////

    // FT-1168: fix memory leak
    if ( le3ShelfLifeDaysValidator_ )
        delete le3ShelfLifeDaysValidator_;

    ui->le3ShelfLifeDays->setValidator(le3ShelfLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le3ShelfLifeDays->setDecimalPlaces(0);
    ui->le3ShelfLifeDays->setInputWrapping(true);
    ui->le3ShelfLifeDays->setInputTruncation(true);
    le3ShelfLifeDaysValidator_->setTop(makeValidatorTop(3));
    le3ShelfLifeDaysValidator_->setBottom(0);
    ui->le3ShelfLifeDays->fromSessionField(currentPLUData->shelfLifeDaysRaw());
    if (ui->le3ShelfLifeDays->text() == "") ui->le3ShelfLifeDays->setText("0");
    // FT-1168: fix memory leak
    if ( le3ShelfLifeHoursValidator_ )
        delete le3ShelfLifeHoursValidator_;

    ui->le3ShelfLifeHours->setValidator(le3ShelfLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le3ShelfLifeHours->setDecimalPlaces(0);
    ui->le3ShelfLifeHours->setInputWrapping(true);
    ui->le3ShelfLifeHours->setInputTruncation(true);
    le3ShelfLifeHoursValidator_->setTop(23);
    le3ShelfLifeHoursValidator_->setBottom(0);
    ui->le3ShelfLifeHours->fromSessionField(currentPLUData->shelfLifeHoursRaw());
    if (ui->le3ShelfLifeHours->text() == "") ui->le3ShelfLifeHours->setText("0");

    // FT-1168: fix memory leak
    if ( le3ShelfLifeMinutesValidator_ )
        delete le3ShelfLifeMinutesValidator_;

    ui->txtShelfLifeMinutes->setValidator(le3ShelfLifeMinutesValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtShelfLifeMinutes->setDecimalPlaces(0);
    ui->txtShelfLifeMinutes->setInputWrapping(true);
    ui->txtShelfLifeMinutes->setInputTruncation(true);
    le3ShelfLifeMinutesValidator_->setTop(59);
    le3ShelfLifeMinutesValidator_->setBottom(0);
    ui->txtShelfLifeMinutes->fromSessionField(currentPLUData->shelfLifeMinutesRaw());
    if (ui->txtShelfLifeMinutes->text() == "") ui->txtShelfLifeMinutes->setText("0");

    // FT-1168: fix memory leak
    if ( le3ProductLifeDaysValidator_ )
        delete le3ProductLifeDaysValidator_;

    ui->le3ProductLifeDays->setValidator(le3ProductLifeDaysValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le3ProductLifeDays->setDecimalPlaces(0);
    ui->le3ProductLifeDays->setInputWrapping(true);
    ui->le3ProductLifeDays->setInputTruncation(true);
    le3ProductLifeDaysValidator_->setTop(makeValidatorTop(3));
    le3ProductLifeDaysValidator_->setBottom(0);
    ui->le3ProductLifeDays->fromSessionField(currentPLUData->productLifeDaysRaw());
    if (ui->le3ProductLifeDays->text() == "") ui->le3ProductLifeDays->setText("0");

    // FT-1168: fix memory leak
    if ( le3ProductLifeHoursValidator_ )
        delete le3ProductLifeHoursValidator_;

    ui->le3ProductLifeHours->setValidator(le3ProductLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le3ProductLifeHours->setDecimalPlaces(0);
    ui->le3ProductLifeHours->setInputWrapping(true);
    ui->le3ProductLifeHours->setInputTruncation(true);
    le3ProductLifeHoursValidator_->setTop(23);
    le3ProductLifeHoursValidator_->setBottom(0);
    ui->le3ProductLifeHours->fromSessionField(currentPLUData->productLifeHoursRaw());
    if (ui->le3ProductLifeHours->text() == "") ui->le3ProductLifeHours->setText("0");

    if ( le3ProductLifeMinutesValidator_ )
        delete le3ProductLifeMinutesValidator_;

    ui->txtProductLifeMinutes->setValidator(le3ProductLifeMinutesValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProductLifeMinutes->setDecimalPlaces(0);
    ui->txtProductLifeMinutes->setInputWrapping(true);
    ui->txtProductLifeMinutes->setInputTruncation(true);
    le3ProductLifeMinutesValidator_->setTop(59);
    le3ProductLifeMinutesValidator_->setBottom(0);
    ui->txtProductLifeMinutes->fromSessionField(currentPLUData->productLifeMinutesRaw());
    if (ui->txtProductLifeMinutes->text() == "") ui->txtProductLifeMinutes->setText("0");

    ui->cb3DontPrintPackedOn->setChecked(currentPLUData->dontPrintPackedOn());
    ui->cb3DontPrintSellBy->setChecked(currentPLUData->dontPrintSellBy());
    ui->cb3DontPrintBestBefore->setChecked(currentPLUData->dontPrintBestBefore());

    // FT-1168: fix memory leak
    if ( le3PortionsValidator_ )
        delete le3PortionsValidator_;

    ui->le3Portions->setValidator(le3PortionsValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le3Portions->setDecimalPlaces(0);
    ui->le3Portions->setInputWrapping(true);
    le3PortionsValidator_->setTop(makeValidatorTop(3));
    ui->le3Portions->fromSessionField(currentPLUData->portions());

    if(!alreadyAdded)
    {
        ui->cb3ProductionPlanningType->addItem("0", "0");
        ui->cb3ProductionPlanningType->addItem("1", "1");
        ui->cb3ProductionPlanningType->addItem("2", "2");
        ui->cb3ProductionPlanningType->addItem("3", "3");
        ui->cb3ProductionPlanningType->addItem("4", "4");
        alreadyAdded = true;
    }

    ui->cb3ProductionPlanningType->setCurrentIndex( currentPLUData->productionPlanningType() );

    // FT-1168: fix memory leak
    if ( le3BuildAMealValidator_ )
        delete le3BuildAMealValidator_;

    ui->le3BuildAMeal->setValidator(le3BuildAMealValidator_ = new QIntValidator(0, 999999, this));
    ui->le3BuildAMeal->setDecimalPlaces(0);
    ui->le3BuildAMeal->setInputWrapping(true);
    ui->le3BuildAMeal->fromSessionField(currentPLUData->sideItemsGroupNum());

    ui->cb3AllSidesFree->setChecked(currentPLUData->mealSideItemsFree());

    // FT-1168: fix memory leak
    if ( le3NumberSidesValidator_ )
        delete le3NumberSidesValidator_;
    // fix future potential memory leak
    if ( le3MeatSpecSheetValidator_ )
        delete le3MeatSpecSheetValidator_;                                                              ///< delete any previous meat specification validator objects

    ui->le3NumberSides->setValidator(le3NumberSidesValidator_ = new QIntValidator(0, 99, this));
    ui->le3NumberSides->setDecimalPlaces(0);
    ui->le3NumberSides->setInputWrapping(true);
    ui->le3NumberSides->fromSessionField(currentPLUData->sideItemsIncludedInMeal());

    ui->le3MeatSpecSheet->setValidator(le3MeatSpecSheetValidator_ = new QIntValidator(0, 99, this));    ///< create a new meat spec validator object
    ui->le3MeatSpecSheet->setDecimalPlaces(0);                                                          ///< set the meat spec decimals to 0
    ui->le3MeatSpecSheet->setInputWrapping(true);                                                       ///< set the meat spec to allow wrapping
    ui->le3MeatSpecSheet->fromSessionField(currentPLUData->meatSpecificationSheet());                   ///< set the value int the meat spec field form the currentPLU

    //////////////////////////////////
    // TAB 4 - Labeling
    //////////////////////////////////

    // All of the validators were moved to realInitialize() 11-27-2018

    //  Add the Prepack fields to the Labeling tab
    ui->cboPrimaryApplierRotation->blockSignals( true );            // dont want these firing off save message SIGNALS
    ui->cboSecondApplierRotation->blockSignals( true );
    ui->cboThirdApplierRotation->blockSignals( true );
    ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );
    ui->cboThirdLabelPlacement->blockSignals( true );

    setupWrapperAndApplierFields();

    ui->cboPrimaryApplierRotation->setCurrentIndex(ui->cboPrimaryApplierRotation->findData(currentPLUData->primaryLabelRotationRaw()));
    ui->cboSecondApplierRotation->setCurrentIndex(ui->cboSecondApplierRotation->findData(currentPLUData->secondLabelRotationRaw()));
    ui->cboThirdApplierRotation->setCurrentIndex(ui->cboThirdApplierRotation->findData(currentPLUData->thirdLabelRotationRaw()));

    QString primaryLabelPlacementRaw = currentPLUData->primaryLabelPlacementRaw();
    QString secondyLabelPlacementRaw = currentPLUData->secondLabelPlacementRaw();
    QString thirdyLabelPlacementRaw  = currentPLUData->thirdLabelPlacementRaw();

    if( ( primaryLabelPlacementRaw.length() == 0 ) || ( primaryLabelPlacementRaw.toInt() > 2 ) )
        primaryLabelPlacementRaw = "2";                         // SET to SYS DEF - USER DATA = 2

    if( secondyLabelPlacementRaw.length() == 0 )                // IF BLANK
        secondyLabelPlacementRaw = "2";                         // SET to SYS DEF - USER DATA = 2

    if( thirdyLabelPlacementRaw.length() == 0 )                 // IF BLANK
        thirdyLabelPlacementRaw = "2";                          // SET to SYS DEF - USER DATA = 2

    ui->cboPrimaryLabelPlacement->setCurrentIndex(ui->cboPrimaryLabelPlacement->findData( primaryLabelPlacementRaw ));
    ui->cboSecondLabelPlacement->setCurrentIndex( ui->cboSecondLabelPlacement->findData( secondyLabelPlacementRaw ));
    ui->cboThirdLabelPlacement->setCurrentIndex(ui->cboThirdLabelPlacement->findData(thirdyLabelPlacementRaw ));

    ui->cboPrimaryApplierRotation->blockSignals( false );        // ok its safe now
    ui->cboSecondApplierRotation->blockSignals( false );
    ui->cboThirdApplierRotation->blockSignals( false );
    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );
    ui->cboThirdLabelPlacement->blockSignals( false );

    //int cbo2ndCount = ui->cboSecondLabelPlacement->count();
    for( int j = 0; j < 9; j++)
    {
        LOG_DEBUG( "initialize() - cboSecondLabelPlacement  findData[%d]: %d", j, ui->cboSecondLabelPlacement->findData( QString::number(j) ));

    }

    ui->cboIntakeMode->setCurrentIndex(ui->cboIntakeMode->findData(currentPLUData->intakeMode()));

    ui->cbApplySecurityLabel->setChecked(currentPLUData->applySecurity());

    if( !session_->isPrepack() )
    {
        ui->wgtRotation->hide();
        ui->wgtIntake->hide();
    }

    //////////////////////////////////
    // TAB 5 - Texts
    //////////////////////////////////

    this->getTextsFromBackend();

    //////////////////////////////////
    // TAB 6 - Flashkeys
    //////////////////////////////////
    setFlashkeyGraphicsComboIndex();      // not needed as I do it on receiving a product ONLY in updateProductInformation

    ui->le6PrimaryLanguageCaption->setText(currentPLUData->productName1());
    ui->le6SecondaryLanguageCaption->setText(currentPLUData->productName2());

    ui->cb6Category->blockSignals(true);
    LOG_DEBUG( "catnum is: %s", currentPLUData->categoryNumber().toLatin1().constData() );
    if(currentPLUData->categoryNumber() == "")
    {
        ui->cb6Category->setCurrentIndex(0);
    }
    else
    {
        ui->cb6Category->setCurrentIndex(ui->cb6Category->findData(currentPLUData->categoryNumber()));
    }
    ui->cb6Category->blockSignals(true);

    //////////////////////////////////
    // TAB 7 - COOL
    //////////////////////////////////

    ui->chkb7ForcedCool->setChecked( currentPLUData->forcedCool() );
    ui->chkb7CoolRequired->setChecked( currentPLUData->coolRequired() );
    ui->le7CoolTrackingText->setText( currentPLUData->coolTrackingText() );
    ui->te7ViewCoolText->setVisible( false );

    ui->cb7CoolCategoryNum->blockSignals(true);
    ui->cb7CoolCategoryNum->clear();
    ui->cb7CoolCategoryNum->addItem(tr("None"));
    ui->cb7CoolCategoryNum->blockSignals(false);

    // No idea why, but using clear, on this combo, would not clear it.
    // tried a bunch of different tests and it just would not clear.
    // This works.

    ui->cb7CoolPretext->blockSignals(true);
    ui->cb7CoolPretext->clear();
    ui->cb7CoolPretext->addItem(tr("None"), "0");
    update();
    ui->cb7CoolPretext->blockSignals(false);

    ui->cb7CoolTextShortlist->blockSignals(true);
    ui->cb7CoolTextShortlist->clear();
    ui->cb7CoolTextShortlist->addItem(tr("None"));
    ui->cb7CoolTextShortlist->blockSignals(false);

    // FT-431 The Value for COOL is required in Supervisor/ All Fields/ COOL cannot be changed.
    // Need to update the tabToSave which indicates which tab the user is currently on.
    tabToSave = ui->tabChangeProductAllFields->currentIndex();

    // FT-606 Newly created Cool text isn't being display in All field Cool
    // If the Cool tab is active when we request a PLU, need to then request the cool data.
    // Previously, only requested it when we clicked on the COOL tab.
    // COOL select tab -  get extra stuff
    if( tabToSave == 6 )
        session_->send(COOLDev, COOLDevReadAllPretexts );           // 135,61 - get the list of Pretexts for the available list

    ifUpdating = false;

    QString modelName = session_->scaleConfig->getScaleModelName();
    if( modelName.contains( "NGW", Qt::CaseInsensitive ) )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            isTwoPrinterSystem_ = true;
            ui->butPlacementView->setEnabled( true );
            ui->butPlacementView->setVisible( true );
        }
    }

    translate();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::realInitialize()
{
    // START adding all one time initialize stuff here

    // TESTING ONLY - set true if you want it.
    ui->saveBut->setVisible( false );

    ui->cb1ProductType->clear();

    ui->cb1ProductType->addItem(tr("Random Weight"), "1");
    if(scaleConfigData->scaleModel().toInt() > 1)
        ui->cb1ProductType->addItem(tr("Fixed weight"), "2");
    ui->cb1ProductType->addItem(tr("By count"), "3");
    if(scaleConfigData->scaleModel().toInt() > 1 && !scaleConfigData->isScaleMetric())
        ui->cb1ProductType->addItem(tr("Fluid ounces"), "4");


    ////////////// TAB 1 //////////////////////////////////////////////////////////

    ui->txtPrefix->setValidator(txtPrefixValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrefix->setDecimalPlaces(0);
    ui->txtPrefix->setInputLeadingZeros(true);
    ui->txtPrefix->setInputWrapping(true);
    txtPrefixValidator_->setTop(99);

    ui->txtUPCNumber->setValidator( txtUPCNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtUPCNumber->setDecimalPlaces(0);
    ui->txtUPCNumber->setInputWrapping(true);
    txtUPCNumberValidator_->setTop(999999);

    ui->txtUPCVendorNumber->setValidator( txtUPCVendorNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtUPCVendorNumber->setDecimalPlaces(0);
    ui->txtUPCVendorNumber->setInputWrapping(true);
    txtUPCVendorNumberValidator_->setTop(999999);

    ui->txtUPCProductNumber->setValidator( txtUPCProductNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtUPCProductNumber->setDecimalPlaces(0);
    ui->txtUPCProductNumber->setInputWrapping(true);
    txtUPCProductNumberValidator_->setTop(999999);

    // EAN Code gets redone every PLU and label change - see initialize()

    ////////////// TAB 2 /////////////////////////////////////////////////////////
    int tarePrecision( scaleConfigData->tarePrecision() );
    if ( tarePrecision <= 2 )
        tarePrecision = 2;
    else
        tarePrecision = 3;

    int total_digits = 5;
    int min_top = 29900;                    // default is 29.900 lb

    if( scaleConfigData->isScaleMetric() )
    {
        min_top = 14900;                    // metric always 3 digits 14.900 kg
    }
    else
    {
        if( 2 == tarePrecision )
        {
            min_top = 2990;                 // single range 2 digits 29.90
            total_digits = 4;
        }
    }

    ui->txtMinWeight->setValidator( minWeightValidator_ = new QIntValidator( 0, INT_MAX, this ) );
    ui->txtMinWeight->setDecimalPlaces( tarePrecision );
    ui->txtMinWeight->setInputWrapping(true);
    ui->txtMinWeight->setInputTruncation(true);
    // ui->txtMaxWeight->setDecimalPlaces( 2 );                 // 2 worked with hardcoded 2
    // minWeightValidator_->setTop(makeValidatorTop( 4 ));      // worked with 2 digits FINALLY but lim it was 9999
    minWeightValidator_->setTop( min_top );
    ui->txtMinWeight->fromSessionField(currentPLUData->minWeight());

    int max_top = 30000;        // default is 30.000
    if( scaleConfigData->isScaleMetric() )
    {
        max_top = 15000;        // metric always 3 digits 15.000
    }
    else
    {
        if( 2 == tarePrecision )
        {
            max_top = 3000;     // single range is 2 digits 30.00
            total_digits = 4;
        }
    }

    ui->txtMaxWeight->setValidator(maxWeightValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtMaxWeight->setDecimalPlaces( tarePrecision );
    ui->txtMaxWeight->setInputWrapping(true);
    ui->txtMaxWeight->setInputTruncation(true);
    // ui->txtMaxWeight->setDecimalPlaces( 2 );                 // 2 worked with hardcoded 2
    // maxWeightValidator_->setTop(makeValidatorTop( 4 ));      // worked with 2 digits but limit was 9999
    maxWeightValidator_->setTop( max_top );
    ui->txtMaxWeight->fromSessionField(currentPLUData->maxWeight());

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    ui->txtMinPrice->setValidator(minPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtMinPrice->setDecimalPlaces(2);
    ui->txtMinPrice->setInputWrapping(true);
    ui->txtMinPrice->setInputTruncation(true);
    minPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
    ui->txtMinPrice->fromSessionField(currentPLUData->minPrice());

    ui->txtMaxPrice->setValidator(maxPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtMaxPrice->setDecimalPlaces(2);
    ui->txtMaxPrice->setInputWrapping(true);
    ui->txtMaxPrice->setInputTruncation(true);
    maxPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
    ui->txtMaxPrice->fromSessionField(currentPLUData->maxPrice());

    requestCategories();
    requestSpecials();
    requestFlashkeyGraphics();
    session_->send(GraphicDev, GraphicReadAllNames); //105,20 request label graphic names

    //////////////////////////////////
    // TAB 4 - Labeling
    //////////////////////////////////

    ui->txtDefaultPriLabelType->setValidator(priDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultPriLabelType->setDecimalPlaces(0);
    ui->txtDefaultPriLabelType->setInputWrapping(true);
    ui->txtDefaultPriLabelType->setInputTruncation(true);
    priDefaultLabelTypeValidator_->setTop(makeValidatorTop(5));

    ui->txtDefaultSecLabelType->setValidator(secDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultSecLabelType->setDecimalPlaces(0);
    ui->txtDefaultSecLabelType->setInputWrapping(true);
    ui->txtDefaultSecLabelType->setInputTruncation(true);
    secDefaultLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtDefaultTriLabelType->setValidator(triDefaultLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtDefaultTriLabelType->setDecimalPlaces(0);
    ui->txtDefaultTriLabelType->setInputWrapping(true);
    ui->txtDefaultTriLabelType->setInputTruncation(true);
    triDefaultLabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtManualPriLabelType->setValidator(priManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualPriLabelType->setDecimalPlaces(0);
    ui->txtManualPriLabelType->setInputWrapping(true);
    ui->txtManualPriLabelType->setInputTruncation(true);
    priManualLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtManualSecLabelType->setValidator(secManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualSecLabelType->setDecimalPlaces(0);
    ui->txtManualSecLabelType->setInputWrapping(true);
    ui->txtManualSecLabelType->setInputTruncation(true);
    secManualLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtManualTriLabelType->setValidator(triManualLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtManualTriLabelType->setDecimalPlaces(0);
    ui->txtManualTriLabelType->setInputWrapping(true);
    ui->txtManualTriLabelType->setInputTruncation(true);
    triManualLabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtPrepackPriLabelType->setValidator(priPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackPriLabelType->setDecimalPlaces(0);
    ui->txtPrepackPriLabelType->setInputWrapping(true);
    ui->txtPrepackPriLabelType->setInputTruncation(true);
    priPrepackLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtPrepackSecLabelType->setValidator(secPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackSecLabelType->setDecimalPlaces(0);
    ui->txtPrepackSecLabelType->setInputWrapping(true);
    ui->txtPrepackSecLabelType->setInputTruncation(true);
    secPrepackLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtPrepackTriLabelType->setValidator(triPrepackLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPrepackTriLabelType->setDecimalPlaces(0);
    ui->txtPrepackTriLabelType->setInputWrapping(true);
    ui->txtPrepackTriLabelType->setInputTruncation(true);
    triPrepackLabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtProdEntryPriLabelType->setValidator(priProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntryPriLabelType->setDecimalPlaces(0);
    ui->txtProdEntryPriLabelType->setInputWrapping(true);
    ui->txtProdEntryPriLabelType->setInputTruncation(true);
    priProdEntryLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtProdEntrySecLabelType->setValidator(secProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntrySecLabelType->setDecimalPlaces(0);
    ui->txtProdEntrySecLabelType->setInputWrapping(true);
    ui->txtProdEntrySecLabelType->setInputTruncation(true);
    secProdEntryLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtProdEntryTriLabelType->setValidator(triProdEntryLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtProdEntryTriLabelType->setDecimalPlaces(0);
    ui->txtProdEntryTriLabelType->setInputWrapping(true);
    ui->txtProdEntryTriLabelType->setInputTruncation(true);
    triProdEntryLabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtSelfServPriLabelType->setValidator(priSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServPriLabelType->setDecimalPlaces(0);
    ui->txtSelfServPriLabelType->setInputWrapping(true);
    ui->txtSelfServPriLabelType->setInputTruncation(true);
    priSelfServLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtSelfServSecLabelType->setValidator(secSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServSecLabelType->setDecimalPlaces(0);
    ui->txtSelfServSecLabelType->setInputWrapping(true);
    ui->txtSelfServSecLabelType->setInputTruncation(true);
    secSelfServLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtSelfServTriLabelType->setValidator(triSelfServLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtSelfServTriLabelType->setDecimalPlaces(0);
    ui->txtSelfServTriLabelType->setInputWrapping(true);
    ui->txtSelfServTriLabelType->setInputTruncation(true);
    triSelfServLabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtPick5PriLabelType->setValidator(priPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5PriLabelType->setDecimalPlaces(0);
    ui->txtPick5PriLabelType->setInputWrapping(true);
    ui->txtPick5PriLabelType->setInputTruncation(true);
    priPick5LabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtPick5SecLabelType->setValidator(secPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5SecLabelType->setDecimalPlaces(0);
    ui->txtPick5SecLabelType->setInputWrapping(true);
    ui->txtPick5SecLabelType->setInputTruncation(true);
    secPick5LabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtPick5TriLabelType->setValidator(triPick5LabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtPick5TriLabelType->setDecimalPlaces(0);
    ui->txtPick5TriLabelType->setInputWrapping(true);
    ui->txtPick5TriLabelType->setInputTruncation(true);
    triPick5LabelTypeValidator_->setTop(makeValidatorTop(4));
    /////
    ui->txtOnlineOrderingPriLabelType->setValidator(priOnlineOrderingLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtOnlineOrderingPriLabelType->setDecimalPlaces(0);
    ui->txtOnlineOrderingPriLabelType->setInputWrapping(true);
    ui->txtOnlineOrderingPriLabelType->setInputTruncation(true);
    priOnlineOrderingLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtOnlineOrderingSecLabelType->setValidator(secOnlineOrderingLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtOnlineOrderingSecLabelType->setDecimalPlaces(0);
    ui->txtOnlineOrderingSecLabelType->setInputWrapping(true);
    ui->txtOnlineOrderingSecLabelType->setInputTruncation(true);
    secOnlineOrderingLabelTypeValidator_->setTop(makeValidatorTop(4));

    ui->txtOnlineOrderingTriLabelType->setValidator(triOnlineOrderingLabelTypeValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtOnlineOrderingTriLabelType->setDecimalPlaces(0);
    ui->txtOnlineOrderingTriLabelType->setInputWrapping(true);
    ui->txtOnlineOrderingTriLabelType->setInputTruncation(true);
    triOnlineOrderingLabelTypeValidator_->setTop(makeValidatorTop(4));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::updateProductInformation()
{
    // SPECIALS - update selected value in specials combo
    ui->cb6TypeOfSpecial->blockSignals(true);
    int numOf = ui->cb6TypeOfSpecial->count();                          // just checking - having an issue here

    // moved to where PLUs are updated when read in
    if(currentPLUData->specialType() == "")
    {
        ui->cb6TypeOfSpecial->setCurrentIndex(0);
    }
    else
    {
        int specialNum = currentPLUData->specialType().toInt();
        int specialIndex = -1;

        for(int j = 0; j < numOf; j++)
        {
            if(specialNum == specialNumbersList.at(j).toInt())
            {
                specialIndex = j;
                break;
            }
        }

        ui->cb6TypeOfSpecial->setCurrentIndex( specialIndex );
    }

    //update();
    ui->cb6TypeOfSpecial->blockSignals(false);
    // END SPECIALS

    // LABEL GRAPHICS - update values in combo
    ui->cb4Graphic1->blockSignals(true);
    ui->cb4Graphic2->blockSignals(true);
    ui->cb4Graphic3->blockSignals(true);
    ui->cb4Graphic4->blockSignals(true);

    setGraphicCurrentIndex(ui->cb4Graphic1, currentPLUData->graphic1Name() );
    setGraphicCurrentIndex(ui->cb4Graphic2, currentPLUData->graphic2Name() );
    setGraphicCurrentIndex(ui->cb4Graphic3, currentPLUData->graphic3Name() );
    setGraphicCurrentIndex(ui->cb4Graphic4, currentPLUData->graphic4Name() );

    ui->cb4Graphic1->blockSignals(false);
    ui->cb4Graphic2->blockSignals(false);
    ui->cb4Graphic3->blockSignals(false);
    ui->cb4Graphic4->blockSignals(false);
    // END LABEL GRAPHICS

    // LABELS - update labels tab
    ui->txtDefaultPriLabelType->fromSessionField(QString::number(currentPLUData->primaryDefaultLabelType()));
    ui->txtDefaultSecLabelType->fromSessionField(QString::number(currentPLUData->secondDefaultLabelType()));
    ui->txtDefaultTriLabelType->fromSessionField(QString::number(currentPLUData->thirdDefaultLabelType()));
    ui->txtManualPriLabelType->fromSessionField(QString::number(currentPLUData->primaryManualLabelType()));
    ui->txtManualSecLabelType->fromSessionField(QString::number(currentPLUData->secondManualLabelType()));
    ui->txtManualTriLabelType->fromSessionField(QString::number(currentPLUData->thirdManualLabelType()));
    ui->txtPrepackPriLabelType->fromSessionField(QString::number(currentPLUData->primaryPrepackLabelType()));
    ui->txtPrepackSecLabelType->fromSessionField(QString::number(currentPLUData->secondPrepackLabelType()));
    ui->txtPrepackTriLabelType->fromSessionField(QString::number(currentPLUData->thirdPrepackLabelType()));
    ui->txtProdEntryPriLabelType->fromSessionField(QString::number(currentPLUData->primaryProdEntryLabelType()));
    ui->txtProdEntrySecLabelType->fromSessionField(QString::number(currentPLUData->secondProdEntryLabelType()));
    ui->txtProdEntryTriLabelType->fromSessionField(QString::number(currentPLUData->thirdProdEntryLabelType()));
    ui->txtSelfServPriLabelType->fromSessionField(QString::number(currentPLUData->primarySelfServLabelType()));
    ui->txtSelfServSecLabelType->fromSessionField(QString::number(currentPLUData->secondSelfServLabelType()));
    ui->txtSelfServTriLabelType->fromSessionField(QString::number(currentPLUData->thirdSelfServLabelType()));
    ui->txtPick5PriLabelType->fromSessionField(QString::number(currentPLUData->primaryPick5LabelType()));
    ui->txtPick5SecLabelType->fromSessionField(QString::number(currentPLUData->secondPick5LabelType()));
    ui->txtPick5TriLabelType->fromSessionField(QString::number(currentPLUData->thirdPick5LabelType()));
    ui->txtOnlineOrderingPriLabelType->fromSessionField(QString::number(currentPLUData->primaryOnlineOrderingLabelType()));
    ui->txtOnlineOrderingSecLabelType->fromSessionField(QString::number(currentPLUData->secondOnlineOrderingLabelType()));
    ui->txtOnlineOrderingTriLabelType->fromSessionField(QString::number(currentPLUData->thirdOnlineOrderingLabelType()));

    // END LABELS
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int superChangeProductAllFields::countChars(QString incomingString, QChar character)
{
    return(incomingString.count(character, Qt::CaseSensitive));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::getTextsFromBackend()
{
    if( currentPLUData->itemNumber().toInt() == ProductType::PRODUCTTYPE_UNKNOWN )
        return;

    ui->te5ExpandedText->setText("");
    ui->te5SpecialMessage->setText("");
    ui->te5ProductNote->setText("");
    ui->te5ProductMarquee->setText("");
    ui->te5CookingInstructions->setText("");
    ui->te5Recipes->setText("");
    ui->te5ExpandedText2->setText("");
    ui->te5SuggestiveSelling->setText("");
    ui->te5Allergens->setText("");

    // send messages asking for the texts, the backend does not return an empty strnig if these do not exist.
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",1");
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",2");
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",3");

    if (session_->isPrepack())
    {
        //  HA-72
        ui->te5ProductMarquee->hide();
        ui->frmProductMarquee->hide();
        ui->lblProductMarquee->hide();
    }
    else
    {
        //Send: 000018101,28,333,4 Marquee
        session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",4");
    }

    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",5");
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",6");
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",7");

    if (session_->isPrepack())
    {
        //  HA-72
        ui->te5SuggestiveSelling->hide();
        ui->frmSuggestiveSelling->hide();
        ui->lblSuggestiveSelling->hide();
    }
    else
    {
        //Send: 000018101,28,333,8 Suggestive Selling
        session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",8");
    }

    //Send: 000018101,28,333,9 Allergens
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",9");
    //Send: 000018101,28,333,20 Nutrifacts
    session_->send(ProductDev, ProductReadSupportRecord, currentPLUData->itemNumber() + ",20");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::requestCategories()
{
    // 121,1
    session_->send(CategoryDev, CategoryDtReadAll);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::loadCategoriesCombo(const SessionMessage &msg)
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "loadCategoriesCombo" );

    // 121,3 has come in
    DomSelfServAllCategories m( msg );

    QVector<QString> names( TokenParser::splitStringTokens( m.CatNamesList() ) );
    QStringList categoryNamesList   = m.CatNamesList().split(",", QString::KeepEmptyParts);
    QStringList categoryNumbersList = m.CatNumsList().split(",", QString::KeepEmptyParts);

    // FT-XXXX: prevent crash with invalid names
    int categoryCount( qMin( categoryNamesList.count(), categoryNumbersList.count() ) );

    ui->cb6Category->blockSignals(true);
    ui->cb6Category->clear();
    ui->cb6Category->addItem(tr("None"), "0");


    for(int i = 0; i < categoryCount; i++)
    {
        ui->cb6Category->addItem(names.at(i), categoryNumbersList.at(i));
    }

    LOG_DEBUG( "catnum is: %s", currentPLUData->categoryNumber().toLatin1().constData() );
    if(currentPLUData->categoryNumber() == "")
    {
        ui->cb6Category->setCurrentIndex(0);
    }
    else
    {
        ui->cb6Category->setCurrentIndex(ui->cb6Category->findData(currentPLUData->categoryNumber()));
    }

    ui->cb6Category->blockSignals(false);

    // blank the top PLU and description
    if(currentPLUData->itemNumber().toInt() < 1)
    {
        ui->leProductNumber->setText("");
        previousPLUItemNumber = "0";
    }
    else
    {
        ui->leProductNumber->setText(currentPLUData->itemNumber());
        previousPLUItemNumber = currentPLUData->itemNumber();
    }
}

void superChangeProductAllFields::displayTheFlashkeyGraphic(int)
{
    //if ( ignoreSignals_ )
     //   return;
    // HA-1: use combo data, not text
    // which graphic to display + full path
    QString graphicFileName( ui->cb6FlashkeyGraphic->itemData( ui->cb6FlashkeyGraphic->currentIndex() ).toString() );

    QString fullGraphicFilename = filesystem::absoluteFilePath( graphicFileName, folderType::ProductsImages );

    // Load graphic in graphics view
    //if(file)

    QPixmap pixmap(fullGraphicFilename);

    if(pixmap.isNull())
    {
        if("EN.US" == mainWindow_->language()) // English
            fullGraphicFilename = filesystem::absoluteFilePath("noImage_en.jpg",folderType::Images);
        else if("EN.CA" == mainWindow_->language()) //Could be different than English
            fullGraphicFilename = filesystem::absoluteFilePath("noImage_en.jpg",folderType::Images);
        else if("ES.MX" == mainWindow_->language()) // Spanish
            fullGraphicFilename = filesystem::absoluteFilePath("noImage_es.jpg",folderType::Images);
        else if("FR.CA" == mainWindow_->language()) // French
            fullGraphicFilename = filesystem::absoluteFilePath("noImage_fr.jpg",folderType::Images); // TODO Need this french graphic

        QPixmap pixmap(fullGraphicFilename);

        QGraphicsScene *scene = new QGraphicsScene( this );
        scene->addPixmap(pixmap);

        QGraphicsScene *oldScene( ui->gv6FlashkeyGraphic->scene() );

        ui->gv6FlashkeyGraphic->setScene(scene);
        ui->gv6FlashkeyGraphic->show();

        // FT-1168: fix memory leak
        if ( oldScene )
            delete oldScene;
    }
    else
    {
        QGraphicsScene *scene = new QGraphicsScene( this );
        scene->addPixmap(pixmap);

        QGraphicsScene *oldScene( ui->gv6FlashkeyGraphic->scene() );

        ui->gv6FlashkeyGraphic->setScene(scene);
        ui->gv6FlashkeyGraphic->show();

        // FT-1168: fix memory leak
        if ( oldScene )
            delete oldScene;
    }

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onUpdateDiscountPricing(int newIndex)
{
    int prec_digits = currencyData->primaryPrecision();

    if( ( initializationComplete ) && ( 0 == prec_digits ) )
    {
        // reset to previous value and exit.
        // Hmmmm, will this cause a circular condition? May have to do an ignoreSignal
        if( 1 == newIndex )
        {
            ui->cb2DiscountType->blockSignals(true);
            QString tmpStr = QString::number(ProductDevError::ProductErrZeroPriceDigitsWarning) + ",5";
            session_->send( Error, ErrProductDevice, tmpStr );      // The Unit Price is less than the Minimum Price
            ui->cb2DiscountType->setCurrentIndex(ui->cb2DiscountType->findData(currentPLUData->discountType()));
            ui->cb2DiscountType->blockSignals(false);
            return;
        }
    }

    currentPLUData->discountType() = QString::number( newIndex );

    int currentX = ui->gb2DiscountType->geometry().x();
    int currentY = ui->gb2DiscountType->geometry().y();
    currentY += ui->gb2DiscountType->geometry().height() + PRICING_TAB_GROUP_BOX_SPACING;
    currentY = loadDiscountInformation(currentX, currentY);         // dont need the return value, but loadPricingTab() that calls this does
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::requestSpecials()
{
    // 124,1
    session_->send(SpecialsDev, SpecialsDtRead);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::loadSpecialsCombo(const SessionMessage &msg)
{
    //ignoreSignals_ = true;
    ui->cb6TypeOfSpecial->blockSignals(true);

    // 124,3 has come in
    DomSpecialsList m(msg);

    QStringList specialNamesList   = m.typeList().split(",", QString::KeepEmptyParts);
    specialNamesList.replaceInStrings("\"", "");                    // remove the quotes
    specialNumbersList.clear();
    specialNumbersList = m.idList().split(",", QString::KeepEmptyParts);
    specialNumbersList.insert(0, "0");
    specialNamesList.insert(0, tr("None"));
    //ui->cb6TypeOfSpecial->clear();
    // No idea why, but using clear, on this combo, would not clear it.
    // tried a bunch of different tests and it just would not clear.
    // This works.
    int numOf = ui->cb6TypeOfSpecial->count();
    for(int j = 0; j < numOf; j++)
        ui->cb6TypeOfSpecial->removeItem(0);

    numOf = ui->cb6TypeOfSpecial->count();                          // just checking - having an issue here

    for(int i = 0; i < specialNamesList.count(); i++)
    {
        ui->cb6TypeOfSpecial->addItem(specialNamesList.at(i) );
    }

    update();
    ui->cb6TypeOfSpecial->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::requestFlashkeyGraphics()
{
    // 101,52
    session_->send(ProductDev, ProductReadAllSoftKeyGraphicNames);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::loadFlashkeyGraphicsCombo(const SessionMessage &msg)
{
    //LOG_CLASS_INFO( "superChangeProductAllFields", "loadFlashkeyGraphicsCombo" );

    //ignoreSignals_ = true;
    ui->cb6FlashkeyGraphic->blockSignals(true);

    // 101,53 has come in
    DomSelfServProdGraphics m(msg);

    QStringList flashkeyGraphicsList   = m.graphicsList().split(",", QString::KeepEmptyParts);
    flashkeyGraphicsList.replaceInStrings("\"", "");                // remove the quotes

    //ui->cb6FlashkeyGraphic->clear();
    // No idea why, but using clear, on this combo, would not clear it.
    // tried a bunch of different tests and it just would not clear.
    // This works.
    while ( ui->cb6FlashkeyGraphic->count() )
        ui->cb6FlashkeyGraphic->removeItem(0);

    update();

    ui->cb6FlashkeyGraphic->addItem(tr("None"), "0");

    for(int i = 0; i < flashkeyGraphicsList.count(); i++)
    {
        ui->cb6FlashkeyGraphic->addItem(flashkeyGraphicsList.at(i), flashkeyGraphicsList.at(i));
    }

    //ignoreSignals_ = false;
    ui->cb6FlashkeyGraphic->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::setFlashkeyGraphicsComboIndex()
{
    if ( !ui->cb6FlashkeyGraphic->count() )
        return;

    if(currentPLUData->productGraphic() == "" || currentPLUData->productGraphic() == "None")
    {
        ui->cb6FlashkeyGraphic->setCurrentIndex(0);
    }
    else
    {
        bool foundIt = false;
        for(int i = 0; i < ui->cb6FlashkeyGraphic->count() && !foundIt; i++)
        {
            // HA-1: use combo data, not text
            if(currentPLUData->productGraphic() == ui->cb6FlashkeyGraphic->itemData(i).toString())
            {
                foundIt = true;
                ui->cb6FlashkeyGraphic->setCurrentIndex(i);
            }
        }
        if(!foundIt)
        {
            ui->cb6FlashkeyGraphic->addItem(tr("Missing: ") + currentPLUData->productGraphic(), currentPLUData->productGraphic());
            ui->cb6FlashkeyGraphic->setCurrentIndex( ui->cb6FlashkeyGraphic->count() - 1 );
        }
    }

    update();
}

// COOL private methods area
///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::buildAllPretextsList(const SessionMessage& msg)
{
    Q_UNUSED( msg );

    // COOL - 135,63, build a pretext list according to its category, sent 135,61

    session_->send(COOLDev,COOLDevReadAllCategories);               // send 135,81 - Read all COOL Categories
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::buildAllCategoriesList(const SessionMessage &msg)
{
    // COOL - 135,83, build a Categories list according to its product number, sent 135,81

    DomCoolCategoryList m( msg );

    //bool removeNoneText = false;

    int currentCatNum =  currentPLUData->coolCategoryNum();
    if( currentCatNum < 0 )
        currentCatNum = 0;

    buildCoolList(ui->cb7CoolCategoryNum, m.names(), m.recordNums(), m.numOfItems(), currentCatNum );
    session_->send(COOLDev,COOLDevShortlistReadAllCoolText, currentPLUData->itemNumber() );               // send 135,113 - Read all COOL Text by product
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::buildAllCoolTextByProduct(const SessionMessage &msg)
{
    // COOL - 135,114, build a shortlist list according to its product number, sent 135,113

    DomCoolTextList m( msg );
    QString coolCatNum = currentPLUData->coolCategoryNumRaw();
    int numOfItems = m.numOfItems();

    if (numOfItems == 0)
    {
        session_->send(COOLDev,COOLDevReadByAllAndByCategoryPretexts, coolCatNum);               // send 135,78 - Read all COOL pretext for the current COOL category and for all categories
    }
    else
    {
        packCombo( m.names(), m.recordNums(), m.numOfItems() );
        session_->send(COOLDev,COOLDevReadByAllAndByCategoryPretexts, coolCatNum);               // send 135,78 - Read all COOL pretext for the current COOL category and for all categories
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::buildPretextsListByCategory(const SessionMessage& msg)
{
    // COOL - 135,74, build a pretext list according to its category, sent 135,78
    //ignoreSignals_ = true;
    ui->cb7CoolPretext->blockSignals(true);
    DomCoolPretextCatList m( msg );         // myDom


    ui->cb7CoolPretext->clear();                                    // clear any previous pretext

    ui->cb7CoolPretext->addItem(tr("None"), "0");                   // Need to add the None Option back in
    ui->cb7CoolPretext->setCurrentIndex( 0 );

    int index = -1;
    int pretextNum =  currentPLUData->coolPretextNum();

    if (pretextNum < 0)
        pretextNum = 0;

    int numOfItems = m.numOfItems();
    int numRecord = 0;

    if (numOfItems > 0)
    {
        QStringList myNameArray = m.names().split(",");
        QStringList myNumArray  =  m.recordNums().split(",");
        QString tmpNameStr = "";

        for (int j = 0; j < numOfItems; j++)
        {
            tmpNameStr = myNameArray.at(j);
            tmpNameStr.replace( "\"", "");

            numRecord = myNumArray.at(j).toInt();
            ui->cb7CoolPretext->addItem(tmpNameStr, numRecord);


            if (numRecord == pretextNum)
                index = j + 1;
        }
    }

    if (index != -1)
        ui->cb7CoolPretext->setCurrentIndex(index);
    else
    {
        ui->cb7CoolPretext->setCurrentIndex(0);

        //	PTR 1391:  If there are no Pretexts in the list, clear the Pretext num in the PLU record.
        currentPLUData->coolPretextNumRaw() = "0";
    }

    setDefaultCoolTextForProduct();
    ui->cb7CoolPretext->blockSignals(false);
    //ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::updateRunModeValue(const SessionMessage& msg)
{

    DomUpdateRunModeField m( msg );     // var domRunModeField = parent.dom_frame.document.forms["updateRunModeField"];


    bool isLegal =  m.isUpdateLegal();
    int offsetIndex = m.offsetIndex();

    if ( isLegal )
    {
        if (offsetIndex == Product::fieldOffsetProdCoolDefaultTextNum)
        {
            //	The default COOL text has changed because it was deleted. Update the DOM.
            currentPLUData->coolDefaultTextNumRaw() = m.newValue();
        }
        else if( offsetIndex == Product::fieldOffsetProductReportingClassNum )
        {
            currentPLUData->reportingClass() = m.newValue();        // update reporting class (tho in theory we already did)
        }

        else if(offsetIndex == Product::fieldOffsetDefaultLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->primaryDefaultLabelTypeRaw() = m.newValue();   // update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetDefaultLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->secondDefaultLabelTypeRaw() = m.newValue();    // update label type
        }
        else if( offsetIndex == Product::fieldOffsetDefaultLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->thirdDefaultLabelTypeRaw() = m.newValue();     // update label type
        }
        //
        else if(offsetIndex == Product::fieldOffsetManualLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->primaryManualLabelTypeRaw() = m.newValue();   // update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetManualLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->secondManualLabelTypeRaw() = m.newValue();    // update label type
        }
        else if( offsetIndex == Product::fieldOffsetManualLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->thirdManualLabelTypeRaw() = m.newValue();     // update label type
        }
        //
        else if(offsetIndex == Product::fieldOffsetPrepackLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->primaryPrepackLabelTypeRaw() = m.newValue();   // update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetPrepackLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->secondPrepackLabelTypeRaw() = m.newValue();    // update label type
        }
        else if( offsetIndex == Product::fieldOffsetPrepackLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->thirdPrepackLabelTypeRaw() = m.newValue();     // update label type
        }
        //
        else if(offsetIndex == Product::fieldOffsetProdEntryLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             // request the plu again if going to the first tab
            currentPLUData->primaryProdEntryLabelTypeRaw() = m.newValue();   // update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetProdEntryLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->secondProdEntryLabelTypeRaw() = m.newValue();    // update label type
        }
        else if( offsetIndex == Product::fieldOffsetProdEntryLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->thirdProdEntryLabelTypeRaw() = m.newValue();     // update label type
        }
        //
        else if(offsetIndex == Product::fieldOffsetSelfServLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->primarySelfServLabelTypeRaw() = m.newValue();   // update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetSelfServLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->secondSelfServLabelTypeRaw() = m.newValue();    // update label type
        }
        else if( offsetIndex == Product::fieldOffsetSelfServLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->thirdSelfServLabelTypeRaw() = m.newValue();     // update label type
        }
        //
        else if(offsetIndex == Product::fieldOffsetPick5LabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->primaryPick5LabelTypeRaw() = m.newValue();   	// update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetPick5LabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->secondPick5LabelTypeRaw() = m.newValue();    	// update label type
        }
        else if( offsetIndex == Product::fieldOffsetPick5LabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->thirdPick5LabelTypeRaw() = m.newValue();     	// update label type
        }
        else if(offsetIndex == Product::fieldOffsetOnlineOrderingLabelTypeNum1)
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->primaryOnlineOrderingLabelTypeRaw() = m.newValue();   	// update label type
        }
        else if( offsetIndex ==  Product::fieldOffsetOnlineOrderingLabelTypeNum2 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->secondOnlineOrderingLabelTypeRaw() = m.newValue();    	// update label type
        }
        else if( offsetIndex == Product::fieldOffsetOnlineOrderingLabelTypeNum3 )
        {
            hasLabelBeenUpdated = true;                             		// request the plu again if going to the first tab
            currentPLUData->thirdOnlineOrderingLabelTypeRaw() = m.newValue();     	// update label type
        }
    }
    else
    {
        if ( offsetIndex == Product::fieldOffsetProductReportingClassNum )
        {
            QString prevVal = m.prevDomElem();
            currentPLUData->reportingClass() = prevVal;
            ui->le1ReportingClass->setText(prevVal);
            session_->send( Error, ErrProductDevice, QString::number(101081) + ",5");
        }
        else if(offsetIndex == Product::fieldOffsetLabelTypeNum1 )
            currentPLUData->primaryLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetLabelTypeNum2 )
            currentPLUData->secondLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetLabelTypeNum3 )
            currentPLUData->thirdLabelTypeRaw()   = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetGraphicName1 )
        {
            currentPLUData->graphic1Name() = m.prevDomElem();
            setGraphicCurrentIndex(ui->cb4Graphic1, currentPLUData->graphic1Name() );
            session_->send( Error, ErrProductDevice, QString::number(101006) + ",5");
        }
        else if( offsetIndex == Product::fieldOffsetGraphicName2 )
        {
            currentPLUData->graphic2Name() = m.prevDomElem();
            setGraphicCurrentIndex(ui->cb4Graphic2, currentPLUData->graphic2Name() );
            session_->send( Error, ErrProductDevice, QString::number(101006) + ",5");
        }
        else if( offsetIndex == Product::fieldOffsetGraphicName3 )
        {
            currentPLUData->graphic3Name() = m.prevDomElem();
            setGraphicCurrentIndex(ui->cb4Graphic3, currentPLUData->graphic3Name() );
            session_->send( Error, ErrProductDevice, QString::number(101006) + ",5");
        }
        else if( offsetIndex == Product::fieldOffsetGraphicName4 )
        {
            currentPLUData->graphic4Name() = m.prevDomElem();
            setGraphicCurrentIndex(ui->cb4Graphic4, currentPLUData->graphic4Name() );
            session_->send( Error, ErrProductDevice, QString::number(101006) + ",5");
        }
        else if(offsetIndex == Product::fieldOffsetDefaultLabelTypeNum1 )
            currentPLUData->primaryDefaultLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetDefaultLabelTypeNum2 )
            currentPLUData->secondDefaultLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetDefaultLabelTypeNum3 )
            currentPLUData->thirdDefaultLabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetManualLabelTypeNum1 )
            currentPLUData->primaryManualLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetManualLabelTypeNum2 )
            currentPLUData->secondManualLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetManualLabelTypeNum3 )
            currentPLUData->thirdManualLabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetPrepackLabelTypeNum1 )
            currentPLUData->primaryPrepackLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetPrepackLabelTypeNum2 )
            currentPLUData->secondPrepackLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetPrepackLabelTypeNum3 )
            currentPLUData->thirdPrepackLabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetProdEntryLabelTypeNum1 )
            currentPLUData->primaryProdEntryLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetProdEntryLabelTypeNum2 )
            currentPLUData->secondProdEntryLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetProdEntryLabelTypeNum3 )
            currentPLUData->thirdProdEntryLabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetSelfServLabelTypeNum1 )
            currentPLUData->primarySelfServLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetSelfServLabelTypeNum2 )
            currentPLUData->secondSelfServLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetSelfServLabelTypeNum3 )
            currentPLUData->thirdSelfServLabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetPick5LabelTypeNum1 )
            currentPLUData->primaryPick5LabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetPick5LabelTypeNum2 )
            currentPLUData->secondPick5LabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetPick5LabelTypeNum3 )
            currentPLUData->thirdPick5LabelTypeRaw() = m.prevDomElem();
        else if(offsetIndex == Product::fieldOffsetOnlineOrderingLabelTypeNum1 )
            currentPLUData->primaryOnlineOrderingLabelTypeRaw() = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetOnlineOrderingLabelTypeNum2 )
            currentPLUData->secondOnlineOrderingLabelTypeRaw()  = m.prevDomElem();
        else if( offsetIndex == Product::fieldOffsetOnlineOrderingLabelTypeNum3 )
            currentPLUData->thirdOnlineOrderingLabelTypeRaw() = m.prevDomElem();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::buildCoolList(QWidget *widget, QString names, QString recordIds, int numOfItems, int numToSelect)
{
    // called so cool lists can be dynamically built

    QComboBox *myCombo = dynamic_cast<QComboBox*> ( widget );
    int index  = -1;
    int recordNum;

    myCombo->blockSignals(true);                                    // have to block even on a clear or a SIGNAL will fire
    myCombo->clear();                                               // clear previous data
    ui->cb7CoolCategoryNum->addItem(tr("None"), "0");               // have to add None back in

    if( numOfItems > 0)
    {
        QStringList namesList = names.split(",");
        QStringList numList  = recordIds.split(",");
        QString tmpNameStr = "";

        int originalLength = myCombo->count();

        for(int j = 0; j < numOfItems; j++)
        {
            tmpNameStr = namesList.at(j);
            tmpNameStr.replace( "\"", "");

            recordNum = numList.at(j).toInt();

            if(recordNum == numToSelect)
                index = j + originalLength;

            myCombo->addItem(tmpNameStr, numList.at(j));
        }
    }

    if( index != -1 )
        myCombo->setCurrentIndex(index);

    myCombo->blockSignals(false);                                   // ok let them back in
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::packCombo( QString names, QString recordNums, int numOfItems )
{
    //ignoreSignals_ = true;
    int index = -1;

    ui->cb7CoolTextShortlist->blockSignals(true);
    ui->cb7CoolTextShortlist->clear();                              // clean it up first from other preteexts
    ui->cb7CoolTextShortlist->addItem(tr("None"));

    //shortListNamesList = names.split(",");                       // FT-919 Cool jacked when more than one country added
    shortListNamesList = TokenParser::splitStringTokens( names );  // save these lists to be used elsewhere
    shortListNumList  = recordNums.split(",");

    int defaultCoolTextNum  = currentPLUData->coolDefaultTextNum();
    QString tmpNameStr = "";

    for(int j = 0; j < numOfItems; j++)
    {
        tmpNameStr = shortListNamesList.at(j);
        tmpNameStr.replace( "\"", "");

        ui->cb7CoolTextShortlist->addItem(tmpNameStr, shortListNumList.at(j));

        if(defaultCoolTextNum == shortListNumList.at(j).toInt() )
            index = j;
    }

    if( index >= 0)
        ui->cb7CoolTextShortlist->setCurrentIndex( index );

    ui->cb7CoolTextShortlist->blockSignals(false);
    //ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::setDefaultCoolTextForProduct()
{
    int j = 0;
    int index = 0;
    int listNum = 0;
    int textNumber = currentPLUData->coolDefaultTextNum();
    int shortListLen = ui->cb7CoolTextShortlist->count();

    if ( textNumber < 0)
        textNumber = 0;

    if (textNumber > 0)
    {
        while ( (j < shortListLen) && (index == 0) )
        {
            listNum = ui->cb7CoolTextShortlist->itemData(j).toInt();
            if (listNum == textNumber)
                index = j;
            ++j;
        }
    }

    ui->cb7CoolTextShortlist->setCurrentIndex(index);
}

void superChangeProductAllFields::hidePriceFields()
{
    // hide group boxes
    ui->gb2ByCount->hide();
    ui->gb2Discount->hide();
    ui->gb2ExceptionPrice->hide();
    ui->gb2FractionalPrice->hide();
    ui->gb2NetWeightFluidOunces->hide();
    ui->gb2NetWeightOunces->hide();
    ui->gb2Points->hide();
    ui->gb2PoundsForPricing->hide();
    ui->gb2Tare->hide();
    ui->gb2UnitPrice->hide();
    ui->gb2ProportionalTare->hide();
    ui->gb2PlatterTare->hide();
    ui->gb2RegularPrice->hide();
    ui->gb2DiscountType->hide();
    ui->gbMinMaxPrice->hide();
    ui->gbMinMaxWeight->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductAllFields::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

void superChangeProductAllFields::governFields(int tabNumber, bool decision)
{
    switch (tabNumber)
    {
    case 0: // Main
        ui->cb1ProductType->setEnabled(decision);
        ui->te1Description->setEnabled(decision);
        ui->le1ReportingClass->setEnabled(decision);
        break;
    case 1: // Pricing
        //TBD
        break;
    case 2: // Product Life
        break;
    case 3: // Labeling
        break;
    case 4: // Texts
        break;
    case 5: // Flashkeys
        break;
    case 6: // COOL
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::translate()
{
    ui->retranslateUi( this );
    ui->cb1ProductType->setItemText(0, tr("Random Weight"));
    if(scaleConfigData->scaleModel().toInt() > 1)
    {
        ui->cb1ProductType->setItemText(1, tr("Fixed weight"));
    }
    ui->cb1ProductType->setItemText(2, tr("By count"));
    if(scaleConfigData->scaleModel().toInt() > 1 && !scaleConfigData->isScaleMetric())
    {
        ui->cb1ProductType->setItemText(3, tr("Fluid ounces"));
    }

    ui->cb2DiscountType->setItemText(0, tr("None"));
    ui->cb2DiscountType->setItemText(1, tr("Cents off"));
    ui->cb2DiscountType->setItemText(2, tr("Discount price"));
    ui->cb2DiscountType->setItemText(3, tr("Percent off"));
    ui->cb2DiscountType->setItemText(4, tr("Advertised price"));
    ui->cb2DiscountType->setItemText(5, tr("Percent added"));
    ui->cb2DiscountType->setItemText(6, tr("Package discount"));

    ui->cb6Category->setItemText(0, tr("None"));
    ui->cb6FlashkeyGraphic->setItemText(0, tr("None"));
    ui->cb6TypeOfSpecial->setItemText(0, tr("None"));

    if( 5 == tabToSave )
        requestCategories();

    if(currentPLUData->hasNutrifacts() == "1")
        ui->te4Nutrifacts->setText(tr("Nutrifacts data exists"));
    else
        ui->te4Nutrifacts->setText(tr("No nutrifacts data"));

    // FT-656
    if( shortListEditor_ )
        shortListEditor_->translate();

    ui->cboPrimaryLabelPlacement->blockSignals( true );
    ui->cboSecondLabelPlacement->blockSignals( true );
    ui->cboThirdLabelPlacement->blockSignals( true );

    ui->cboPrimaryLabelPlacement->setItemText(0, tr("System default"));    //    Make first in list in case of no applier.
    ui->cboPrimaryLabelPlacement->setItemText(1, tr("Upper Right"));
    ui->cboPrimaryLabelPlacement->setItemText(2, tr("Upper Left"));

    int primaryIndex = currentPLUData->primaryLabelPlacement();
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

    ui->cboPrimaryLabelPlacement->blockSignals( false );
    ui->cboSecondLabelPlacement->blockSignals( false );
    ui->cboThirdLabelPlacement->blockSignals( false );

    ui->cboThirdLabelPlacement->setItemText(0, tr("System default"));    //    Make first in list in case of no applier.
    ui->cboThirdLabelPlacement->setItemText(1, tr("Upper Right"));
    ui->cboThirdLabelPlacement->setItemText(2, tr("Upper Left"));

    ui->cboIntakeMode->setItemText(0, tr("System default"));
    ui->cboIntakeMode->setItemText(1, tr("Gentle"));
    ui->cboIntakeMode->setItemText(2, tr("Normal"));

    ui->lblMinPrice->setText(  tr("Min total price"));
    ui->lblMaxPrice->setText(  tr("Max total price"));
    ui->lblMinWt->setText( tr("Min total weight"));
    ui->lblMaxWt->setText( tr("Max total weight"));

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void superChangeProductAllFields::loadIncomingPLUData(const SessionMessage &msg)
{
    Q_UNUSED( msg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::loadLabelGraphicNames(const SessionMessage &msg)
{
    DomGraphicNameListProduct m( msg );

    //ignoreSignals_ = true;

    ui->cb4Graphic1->blockSignals(true);                            // on clear and adding none options, dont fire SIGNALS
    ui->cb4Graphic2->blockSignals(true);                            // and updating the combos, dont fire SIGs on itself
    ui->cb4Graphic3->blockSignals(true);
    ui->cb4Graphic4->blockSignals(true);

    ui->cb4Graphic1->clear();
    ui->cb4Graphic2->clear();
    ui->cb4Graphic3->clear();
    ui->cb4Graphic4->clear();

    QStringList graphicNames = m.graphicList().split(",", QString::KeepEmptyParts);

    ui->cb4Graphic1->addItem(tr("None"));
    ui->cb4Graphic2->addItem(tr("None"));
    ui->cb4Graphic3->addItem(tr("None"));
    ui->cb4Graphic4->addItem(tr("None"));

    ui->cb4Graphic1->addItems(graphicNames);
    ui->cb4Graphic2->addItems(graphicNames);
    ui->cb4Graphic3->addItems(graphicNames);
    ui->cb4Graphic4->addItems(graphicNames);

    update();

    ui->cb4Graphic1->blockSignals(false);                            // OK we are good now
    ui->cb4Graphic2->blockSignals(false);
    ui->cb4Graphic3->blockSignals(false);
    ui->cb4Graphic4->blockSignals(false);
}

void superChangeProductAllFields::setGraphicCurrentIndex(QWidget *widget, const QString& graphicName)
{
    QComboBox *graphicCombo = dynamic_cast<QComboBox*> ( widget );
    graphicCombo->blockSignals(true);
    int j = 0;

    if(graphicName != tr("None") && currentPLUData->itemNumber().toInt() > 0 )
    {
        for(j = 0; j < graphicCombo->count(); j++)
        {
            if( graphicName == graphicCombo->itemText(j) )
                break;
        }

        graphicCombo->setCurrentIndex( j );
    }
    else
    {
        graphicCombo->setCurrentIndex(0);
    }

    graphicCombo->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::loadPricingTab(int productType)
{
    //////////////////////////////////
    // TAB 2 - Pricing
    //////////////////////////////////
    if( ignoreDiscountPriceSignals_ )
        return;

    ifUpdating = true;

    ignoreDiscountPriceSignals_ = true;

    ifUpdating = true;

    int nextAvailableX( scaleWidthForScreen( 30 ) );
    int nextAvailableY( scaleHeightForScreen( 20 ) );

    hidePriceFields();

    // By count
    if(productType == 2 || productType == 3 || productType == 4)
    {
        // FT-1168: fix memory leak
        if ( le2ByCountValidator_ )
            delete le2ByCountValidator_;

        ui->gb2ByCount->move( nextAvailableX, nextAvailableY );
        ui->gb2ByCount->show();

        nextAvailableY += ui->gb2ByCount->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->le2ByCount->setValidator(le2ByCountValidator_ = new QIntValidator(0, INT_MAX, this));   // never let users enter < 1
        ui->le2ByCount->setDecimalPlaces(0);
        ui->le2ByCount->setInputWrapping(true);
        ui->le2ByCount->setInputTruncation(true);
        le2ByCountValidator_->setTop(makeValidatorTop(2));
        le2ByCountValidator_->setBottom(1);

        if(showForcedFields)
        {
            ui->chkbox2ForcedByCount->show();
        }
        else
        {
            ui->chkbox2ForcedByCount->hide();
        }
    }

    if( currentPLUData->byCount() < 1 )                             // not allowed to be less than 1
        currentPLUData->byCountRaw() = "1";                         // bad data if < 1 so reset it.

    ui->le2ByCount->fromSessionField(currentPLUData->byCountRaw());
    currentPLUData->forcedByCount() ? ui->chkbox2ForcedByCount->setChecked(true) : ui->chkbox2ForcedByCount->setChecked(false);

    if(ui->chkbox2ForcedByCount->isChecked()) ui->le2ByCount->setText("");

    // FT-949 The fractional pricing control in Supervisor mode is a repeating list
    // We need to clear the combo each time thru before we add to it again
    ui->cb2FractionalPriceEnglishUnit->clear();


    if( isMetric )
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

    // As per Larry on 5/14/2013 use fractional price unit as the value
    QString myFP = QString::number( currentPLUData->fractionalPriceUnit() );
    ui->cb2FractionalPriceEnglishUnit->setCurrentIndex( ui->cb2FractionalPriceEnglishUnit->findData(myFP) );

     if( !initializationComplete )
     {
        ui->cb2PriceModifier->addItem("1","1");
        ui->cb2PriceModifier->addItem("2","2");
        ui->cb2PriceModifier->addItem("3","3");
        ui->cb2PriceModifier->addItem("4","4");
        ui->cb2PriceModifier->addItem("5","5");
        ui->cb2PriceModifier->addItem("6","6");
        ui->cb2PriceModifier->addItem("7","7");
        ui->cb2PriceModifier->addItem("8","8");
        ui->cb2PriceModifier->addItem("9","9");
     }

    // As per Larry on 5/14/2013 use pounds for ammount as the value
    ui->cb2PriceModifier->setCurrentIndex(ui->cb2PriceModifier->findData(currentPLUData->poundsForAmount()));
    //ui->cb2PriceModifier->setCurrentIndex(ui->cb2PriceModifier->findData(currentPLUData->priceModifier()));

    // FT-1168: fix memory leak
    if ( le2UnitPriceValidator_ )
        delete le2UnitPriceValidator_;

    ui->le2UnitPrice->setValidator(le2UnitPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2UnitPrice->setDecimalPlaces(2);
    ui->le2UnitPrice->setInputWrapping(true);
    ui->le2UnitPrice->setInputTruncation(true);
    le2UnitPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
    ui->le2UnitPrice->fromSessionField(currentPLUData->unitPrice());

    // validators set up in realInitialize()
    // ui->txtMinWeight->fromSessionField( "0.490" );      // tester
    // ui->txtMaxWeight->fromSessionField( "1.990" );      // tester
    // HAVE to put into the field the right seed value with right number of digits or it blows the validator up

    QString min_wt = currentPLUData->minWeight();
    QString max_wt = currentPLUData->maxWeight();

    QString separator = currencyData->unitSeparator();
    QStringList minweightList( min_wt.split( separator ) );
    QStringList maxweightList( max_wt.split( separator ) );
    int minDigitsLen( 0 );
    int maxDigitsLen( 0 );

    if ( 2 == minweightList.size() )
        minDigitsLen = minweightList.at( 1 ).length();
    if ( 2 == maxweightList.size() )
        maxDigitsLen = maxweightList.at( 1 ).length();

    if ( scaleConfigData->tarePrecision() == 2 )
    {
        if( 3 == minDigitsLen )
            min_wt.chop(1);        // essentially, if our precision is 2, and we have 3 chars after the decimal, crop the last one.
        if( 3 == maxDigitsLen )
            max_wt.chop(1);        // essentially, if our precision is 2, and we have 3 chars after the decimal, crop the last one.
    }
    else
    {
        if( 2 == minDigitsLen )
            min_wt = min_wt.append("0");    // essentially, if our precision is 3, and we have 2 chars after the decimal, add one.
        if( 2 == maxDigitsLen )
            max_wt = max_wt.append("0");    // essentially, if our precision is 3, and we have 2 chars after the decimal, add one.
    }

    ui->txtMinWeight->fromSessionField( min_wt );
    ui->txtMaxWeight->fromSessionField( max_wt );
    ui->txtMinPrice->fromSessionField(  currentPLUData->minPrice()  );
    ui->txtMaxPrice->fromSessionField(  currentPLUData->maxPrice()  );

    if(currentPLUData->forcedUnitPrice())
    {
        ui->chkbox2UnitPriceForcedPrice->setChecked(true);
    }
    else
    {
        ui->chkbox2UnitPriceForcedPrice->setChecked(false);
    }

    // Before this can be done again, all previous items need cleared.
    // Sometime clear doesn't work, so do this
    if( !initializationComplete )                                   // no need to repeat
    {
        while ( ui->cb2DiscountType->count() ) // as a safety precaution, and clear doesn't always work
            ui->cb2DiscountType->removeItem( 0 );

        ui->cb2DiscountType->addItem(tr("None"),"0");
        ui->cb2DiscountType->addItem(tr("Cents off"),"1");
        ui->cb2DiscountType->addItem(tr("Discount price"),"2");
        ui->cb2DiscountType->addItem(tr("Percent off"),"3");
        ui->cb2DiscountType->addItem(tr("Advertised price"),"4");
        ui->cb2DiscountType->addItem(tr("Percent added"),"5");
        ui->cb2DiscountType->addItem(tr("Package discount"),"6");
     }

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
        }
        else
        {
            ui->chkbox2TarePlatterForcedTare->setChecked(false);
        }
    }

    // FT-1497: reworked this section to use max tare from normal scale operation
    int tarePrecision( scaleConfigData->tarePrecision() );
    QString maximumTare( normalScaleOperation->maxTare() );

    if ( tarePrecision < 2 )
        tarePrecision = 2;
    else if ( 3 < tarePrecision )
        tarePrecision = 3;

    if ( !le2TareValidator_ )
        ui->le2Tare->setValidator( le2TareValidator_ = new QIntValidator( 0, 99999, this ) );

    ui->le2Tare->setDecimalPlaces( tarePrecision );
    ui->le2Tare->setInputPrecision( tarePrecision );
    ui->le2Tare->setInputWrapping( true );
    ui->le2Tare->setInputTruncation(true);

    // setup validator
    if ( maximumTare.length() )
    {
        // here we are using the maximum tare from normal scale operations
        maximumTare.replace( ".", "" );
        // HTA-13 Just set the top to maximum tare no need to check precision since the max tare was already checked for precision.

        le2TareValidator_->setTop(maximumTare.toInt());
    }
    else
    {
        // here we are default maximum tare
        int top( scaleConfigData->isScaleMetric() ? 15000 : 30000 );

        if ( 3 == tarePrecision )
            le2TareValidator_->setTop( top );
        else if ( 2 == tarePrecision )
            le2TareValidator_->setTop( top / 10 );
    }

    // HT-136: Set the text field to the correct tare value
    if( scaleConfigData->useSecondTare() )
        ui->le2Tare->fromSessionField( currentPLUData->tareB() );
    else
        ui->le2Tare->fromSessionField( currentPLUData->tare() );

    if(currentPLUData->forcedTare())
    {
        ui->chkbox2ForcedTare->setChecked(true);
    }
    else
    {
        ui->chkbox2ForcedTare->setChecked(false);
    }

    // FT-1168: fix memory leak
    if ( le2ProportionalTareValidator_ )
        delete le2ProportionalTareValidator_;

    ui->le2ProportionalTare->setValidator(le2ProportionalTareValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2ProportionalTare->setDecimalPlaces(1);
    ui->le2ProportionalTare->setInputWrapping(true);
    ui->le2ProportionalTare->setInputTruncation(true);
    le2ProportionalTareValidator_->setTop(makeValidatorTop(3));
    ui->le2ProportionalTare->fromSessionField(currentPLUData->proportionalTare());

    // FT-1168: fix memory leak
    if ( le2ExceptionPriceValidator_ )
        delete le2ExceptionPriceValidator_;

    ui->le2ExceptionPrice->setValidator(le2ExceptionPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2ExceptionPrice->setDecimalPlaces(currencyData->primaryPrecision());
    ui->le2ExceptionPrice->setInputWrapping(true);
    le2ExceptionPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
    ui->le2ExceptionPrice->fromSessionField(currentPLUData->exceptionPrice());

    // Fractional Pricing
    if(scaleConfigData->fractionalPricingOn())
    {
        ui->gb2FractionalPrice->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2FractionalPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2FractionalPrice->show();
        widgetUsedForPrice = "gb2FractionalPrice";
        ui->lbl2CurrencySymbolFractionalPrice->setText(currencyData->priCurrSymbol());

        // FT-950 Super unit price is incorrect
        ui->lbl2MetricUnitFractionalPrice->hide();
        ui->cb2FractionalPriceEnglishUnit->show();

        if(currentPLUData->itemNumber().toInt() > 0)
        {
            // FT-1168: fix memory leak
            if ( le2FractionalPriceValidator_ )
                delete le2FractionalPriceValidator_;

            ui->le2FractionalPrice->setValidator(le2FractionalPriceValidator_ = new QIntValidator(0, INT_MAX, this));
            ui->le2FractionalPrice->setDecimalPlaces(2);
            ui->le2FractionalPrice->setInputWrapping(true);
            ui->le2FractionalPrice->setInputTruncation(true);
            le2FractionalPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
            ui->le2FractionalPrice->fromSessionField(currentPLUData->unitPrice());
            QString myFP = QString::number( currentPLUData->fractionalPriceUnit() );
            ui->cb2FractionalPriceEnglishUnit->setCurrentIndex( ui->cb2FractionalPriceEnglishUnit->findData(myFP) );

        }
        else
        {
            ui->le2FractionalPrice->setText("0.00");
            ui->cb2FractionalPriceEnglishUnit->setCurrentIndex(0);
        }


        if( (currentPLUData->forcedFractionalPrice()) || (currentPLUData->forcedUnitPrice() && widgetUsedForPrice == "gb2FractionalPrice") )
        {
            ui->chkbox2FractionalPricingForcedPrice->setChecked(true);
        }
        else
        {
            ui->chkbox2FractionalPricingForcedPrice->setChecked(false);
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
    // Pounds for Pricing
    else if(scaleConfigData->poundsForPricingOn())
    {
        ui->gb2PoundsForPricing->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2PoundsForPricing->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2PoundsForPricing->show();
        widgetUsedForPrice = "gb2PoundsForPricing";
        ui->lbl2CurrencySymbolPoundsForPricing->setText(currencyData->priCurrSymbol());
        scaleConfigData->isScaleMetric() ? ui->lbl2PoundsForPricingUnit->setText("kgs") : ui->lbl2PoundsForPricingUnit->setText("lbs"); //Universal Symbols cannot be translated
        ui->lbl2CurrencySymbolPoundsForPricing->setText(currencyData->priCurrSymbol());

        if(currentPLUData->itemNumber().toInt() > 0)
        {
            // FT-1168: fix memory leak
            if ( le2PoundsForPriceValidator_ )
                delete le2PoundsForPriceValidator_;

            ui->le2PoundsForPrice->setValidator(le2PoundsForPriceValidator_ = new QIntValidator(0, INT_MAX, this));
            ui->le2PoundsForPrice->setDecimalPlaces(2);
            ui->le2PoundsForPrice->setInputWrapping(true);
            ui->le2PoundsForPrice->setInputTruncation(true);
            le2PoundsForPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
            ui->le2PoundsForPrice->fromSessionField(currentPLUData->unitPrice());
            ui->cb2PriceModifier->setCurrentIndex(ui->cb2PriceModifier->findData(currentPLUData->priceModifier()));
        }
        else
        {
            ui->le2PoundsForPrice->setText("0.00");
            ui->cb2PriceModifier->setCurrentIndex(0);
        }

        if( (currentPLUData->forcedPoundsFor()) || (currentPLUData->forcedUnitPrice() && widgetUsedForPrice == "gb2PoundsForPricing") )
        {
            ui->chkbox2PoundsForPricingForcedPrice->setChecked(true);
        }
        else
        {
            ui->chkbox2PoundsForPricingForcedPrice->setChecked(false);
        }

        showForcedFields ? ui->chkbox2PoundsForPricingForcedPrice->show() : ui->chkbox2PoundsForPricingForcedPrice->hide();

        widgetUsedForPriceModifier = "cb2PriceModifier";
    }
    // Unit Price
    else
    {
        ui->gb2UnitPrice->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2UnitPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2UnitPrice->show();
        widgetUsedForPrice = "gb2UnitPrice";

        ui->lbl2UnitPriceCurrencySymbol->setText(currencyData->priCurrSymbol());
        ui->lblMinCurSym->setText(currencyData->priCurrSymbol());
        ui->lblMaxCurSym->setText(currencyData->priCurrSymbol());

        if(scaleConfigData->isScaleMetric())
        {
            if( 1 == productType ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setVisible( true);
                ui->lbl2UnitPriceUnitSymbol->setText("/kg");
            }
            else
            {
                // determined that we don't need a unit at all on BC or FW
                ui->lbl2UnitPriceUnitSymbol->setVisible( false);
            }
        }
        else
        {
            if( 1 == productType ) //random weight - use per
            {
                ui->lbl2UnitPriceUnitSymbol->setVisible( true);
                ui->lbl2UnitPriceUnitSymbol->setText("/lb");
            }
            else
            {
                // determined that we don't need a unit at all on BC or FW
                ui->lbl2UnitPriceUnitSymbol->setVisible( false);
            }
        }

        showForcedFields ? ui->chkbox2UnitPriceForcedPrice->show() : ui->chkbox2UnitPriceForcedPrice->hide();

        ui->le2UnitPrice->fromSessionField(currentPLUData->unitPrice());

        if(currentPLUData->forcedUnitPrice())
        {
            ui->chkbox2UnitPriceForcedPrice->setChecked(true);
        }
        else
        {
            ui->chkbox2UnitPriceForcedPrice->setChecked(false);
        }
    }

    if(ui->gb2UnitPrice->isVisible())
    {
        widgetUsedForPrice = "gb2UnitPrice";
    }

    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
    {
        if( 1 <= productType && 4 >= productType )
        {
            ui->gbMinMaxPrice->move( nextAvailableX, nextAvailableY );
            nextAvailableY += ui->gbMinMaxPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
            ui->gbMinMaxPrice->show();
        }
    }

    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
    {
        if( ( 1 == productType ) )
        {
            ui->gbMinMaxWeight->move( nextAvailableX, nextAvailableY );
            nextAvailableY += ui->gbMinMaxWeight->height() + PRICING_TAB_GROUP_BOX_SPACING;
            ui->gbMinMaxWeight->show();
        }
    }
    // Discount Pricing - (Frequent Shopper)
    // This has it's own discount code block here, but I had to pull it out becauase if someone changed the discount
    // type to a percent type or advertised price, labels may have to change like $ or % and we may have to
    // display the regular price instead of the discount price so all of this needs recalculated.

    ui->gb2DiscountType->move( nextAvailableX, nextAvailableY );
    nextAvailableY += ui->gb2DiscountType->height() + PRICING_TAB_GROUP_BOX_SPACING;
    ui->gb2DiscountType->show();

    if(frequentShopperData->perProductOrScale().toInt() == 0)
    {
        // per product
        ui->cb2DiscountType->setEnabled(true);

        if(currentPLUData->itemNumber().toInt() > 0)
        {
            ui->cb2DiscountType->setCurrentIndex(ui->cb2DiscountType->findData(currentPLUData->discountType()));
        }
        else
        {
            ui->cb2DiscountType->setCurrentIndex(0);                //on init, no PLU yet
        }
    }
    else
    {
        // per scale
        ui->cb2DiscountType->setEnabled(false);
        ui->cb2DiscountType->setCurrentIndex(ui->cb2DiscountType->findData(currentPLUData->discountType()));
    }

    int newY = loadDiscountInformation(nextAvailableX, nextAvailableY);

    // need to update this for the positioning that occurred in loadDiscountInformation()
    nextAvailableY = newY;


    // Points - Always shown to bottom right of Discount Groupbox
    ui->gb2Points->move( scaleWidthForScreen( PRICING_TAB_POINTS_X ), nextAvailableY );
    nextAvailableY += ui->gb2Points->height() + PRICING_TAB_GROUP_BOX_SPACING;

    // FT-1168: fix memory leak
    if ( le2PointsValidator_ )
        delete le2PointsValidator_;

    ui->le2Points->setValidator(le2PointsValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->le2Points->setDecimalPlaces(0);
    ui->le2Points->setInputWrapping(true);
    ui->le2Points->setInputTruncation(true);
    le2PointsValidator_->setTop(makeValidatorTop(4));
    ui->le2Points->fromSessionField(currentPLUData->points());
    ui->gb2Points->show();

    // Tare
    if((productType == 1 || productType == 0) || (productType == 3 && scaleConfigData->weightOnByCount()))
    {
        // Show Combo for Platter/Keyboard Tare Combo
        if(scaleConfigData->isPlatterTareOn() == "1")
        {
            ui->gb2PlatterTare->move( nextAvailableX, nextAvailableY );
            nextAvailableY += ui->gb2PlatterTare->height();
            ui->gb2PlatterTare->show();
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
                }
                else
                {
                    ui->chkbox2TarePlatterForcedTare->setChecked(false);
                }
            }
        }
        else
        {
            // Below if block as per Jim Highley on 5/1/2013
            // if by count product type and by count is 1 and print weight on by count is true
            // hlx is currently bugged to always show tare, when it should show tare according to the rule by Jim
            if((productType == 3 && currentPLUData->byCount() == 1 && scaleConfigData->weightOnByCount()) || productType == 2 || productType == 1 || productType == 4)
            {
                // Show Regular Tare
                ui->gb2Tare->move( nextAvailableX, nextAvailableY );
                nextAvailableY += ui->gb2Tare->height() + PRICING_TAB_GROUP_BOX_SPACING;
                ui->gb2Tare->show();

                showForcedFields ? ui->chkbox2ForcedTare->show() : ui->chkbox2ForcedTare->hide();
            }

            ui->lbl2UnitPriceCurrencySymbol->setText(currencyData->priCurrSymbol());
            ui->lblMinCurSym->setText(currencyData->priCurrSymbol());
            ui->lblMaxCurSym->setText(currencyData->priCurrSymbol());

            if(currentPLUData->itemNumber().toInt() > 0 && currentPLUData->forcedTare())
            {
                ui->chkbox2ForcedTare->setChecked(true);
            }
            else
            {
                ui->chkbox2ForcedTare->setChecked(false);
            }
            if(scaleConfigData->isScaleMetric())
            {
                ui->lbl2UnitSymbolTare->setText("kg");
                ui->lblLbMinWt->setText("kg");
                ui->lblLbMaxWt->setText("kg");
            }
            else
            {
                ui->lbl2UnitSymbolTare->setText("lb");
                ui->lblLbMinWt->setText("lb");
                ui->lblLbMaxWt->setText("lb");
            }
        }
    }

    // Proportional Tare
    int modelNum = scaleConfigData->scaleModel().toInt();
    modelNum = 3;                   // REMOVE ME - TESTING ONLY
    if( modelNum == 3)
    {
        if((productType == 1 || productType == 0) )
        {
            // FT-1168: fix memory leak
            if ( le2ProportionalTareValidator_ )
                delete le2ProportionalTareValidator_;

            ui->gb2ProportionalTare->move( nextAvailableX, nextAvailableY );
            nextAvailableY += ui->gb2ProportionalTare->height() + PRICING_TAB_GROUP_BOX_SPACING;
            ui->gb2ProportionalTare->show();
            ui->le2ProportionalTare->setValidator(le2ProportionalTareValidator_ = new QIntValidator(0, INT_MAX, this));
            ui->le2ProportionalTare->setDecimalPlaces(1);
            ui->le2ProportionalTare->setInputWrapping(true);
            le2ProportionalTareValidator_->setTop(makeValidatorTop(3));
            ui->le2ProportionalTare->fromSessionField(currentPLUData->proportionalTare());
            //OLD ui->le2ProportionalTare->setText(currentPLUData->proportionalTare());
        }
    }

    // Exception Price
    if(productType == 3)
    {
        // FT-1168: fix memory leak
        if ( le2ExceptionPriceValidator_ )
            delete le2ExceptionPriceValidator_;

        ui->gb2ExceptionPrice->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2ExceptionPrice->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2ExceptionPrice->show();
        ui->lbl2CurrencySymbolExceptionPrice->setText(currencyData->priCurrSymbol());
        ui->le2ExceptionPrice->setValidator(le2ExceptionPriceValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2ExceptionPrice->setDecimalPlaces(currencyData->primaryPrecision());
        ui->le2ExceptionPrice->setInputWrapping(true);
        le2ExceptionPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
        ui->le2ExceptionPrice->fromSessionField(currentPLUData->exceptionPrice());
        //OLD ui->le2ExceptionPrice->setText(currentPLUData->exceptionPrice());
    }

    // Net Weight
    if(productType == 2)
    {
        ui->gb2NetWeightOunces->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2NetWeightOunces->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2NetWeightOunces->show();

        if(scaleConfigData->isScaleMetric())
        {
            ui->lbl2UnitSymbolNetWeight->setText("kg");
            ui->lblLbMinWt->setText("kg");
            ui->lblLbMaxWt->setText("kg");
        }
        else
        {
            ui->lbl2UnitSymbolNetWeight->setText("oz");
            ui->lblLbMinWt->setText("lb");
            ui->lblLbMaxWt->setText("lb");
        }

        // FT-1168: fix memory leak
        if ( le2NetWeightValidator_ )
            delete le2NetWeightValidator_;

        ui->le2NetWeight->setValidator(le2NetWeightValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2NetWeight->setDecimalPlaces(0);
        ui->le2NetWeight->setInputWrapping(true);
        ui->le2NetWeight->setInputTruncation(true);
        le2NetWeightValidator_->setTop(makeValidatorTop(4));
        ui->le2NetWeight->fromSessionField(currentPLUData->fixedWeight());
        widgetUsedForFixedWeight = "le2NetWeight";
    }
    else if(productType == 4)
    {
        ui->gb2NetWeightFluidOunces->move( nextAvailableX, nextAvailableY );
        nextAvailableY += ui->gb2NetWeightFluidOunces->height() + PRICING_TAB_GROUP_BOX_SPACING;
        ui->gb2NetWeightFluidOunces->show();

        // FT-1168: fix memory leak
        if ( le2NetWeightFluidValidator_ )
            delete le2NetWeightFluidValidator_;

        ui->le2NetWeightFluid->setValidator(le2NetWeightFluidValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2NetWeightFluid->setDecimalPlaces(0);
        ui->le2NetWeightFluid->setInputWrapping(true);
        ui->le2NetWeightFluid->setInputTruncation(true);
        le2NetWeightFluidValidator_->setTop(makeValidatorTop(4));
        ui->le2NetWeightFluid->fromSessionField(currentPLUData->fixedWeight());
        widgetUsedForFixedWeight = "le2NetWeightFluid";
    }

    ifUpdating = false;

    // There is a regularPrice field that is hidden
    ignoreDiscountPriceSignals_ = false;
}

int superChangeProductAllFields::loadDiscountInformation(int xPos, int yPos)
{
    // where is #1 - could not find it so I set it? - currentPLUData->discountValue1() can be set in the regular price too.
    ui->le2Discount1->fromSessionField(currentPLUData->discountValue1());
    ui->le2Discount2->fromSessionField(currentPLUData->discountValue2());
    ui->le2Discount3->fromSessionField(currentPLUData->discountValue3());

    if(currentPLUData->discountType().toInt() == 4)
    {
        ui->gb2Discount->hide();
        ui->gb2RegularPrice->move( QPoint( xPos, yPos ) );
        yPos += ui->gb2RegularPrice->height();
        ui->gb2RegularPrice->show();

        // FT-1168: fix memory leak
        if ( le2RegularPriceValidator_ )
            delete le2RegularPriceValidator_;

        ui->le2RegularPrice->setValidator(le2RegularPriceValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2RegularPrice->setDecimalPlaces(currencyData->primaryPrecision());
        ui->le2RegularPrice->setInputWrapping(true);
        le2RegularPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));
        ui->le2RegularPrice->setText(currentPLUData->discountValue1());
        widgetUsedForDiscountValue1 = "le2RegularPrice";
    }
    else
    {
        ui->gb2RegularPrice->hide();
        ui->gb2Discount->move( xPos, yPos );
        yPos += ui->gb2Discount->height();
        ui->gb2Discount->show();

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
        }

        // FT-1168: fix memory leak
        if ( le2Discount1Validator_ )
            delete le2Discount1Validator_;

        // Yikes, I hate to do this but currently, percentage values are being equated to currency values. If scale's frequent shopper values were set to use percent off then switched to cents off, then a 10.00% discount would turn into $10.00 off discount. SOLUTION: Derive the percentage off in terms of cents and display that, or vice versa, e.g., 20.00% off a $3.00 item would display $0.60 instead of $20.00 as it does now.
        ui->le2Discount1->setValidator(le2Discount1Validator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2Discount1->setDecimalPlaces(2);
        ui->le2Discount1->setInputWrapping(true);
        le2Discount1Validator_->setTop(makeValidatorTop(5));
        ui->le2Discount1->fromSessionField(currentPLUData->discountValue1());
        widgetUsedForDiscountValue1 = "le2Discount1";

        // FT-1168: fix memory leak
        if ( le2Discount2Validator_ )
            delete le2Discount2Validator_;

        ui->le2Discount2->setValidator(le2Discount2Validator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2Discount2->setDecimalPlaces(2);
        ui->le2Discount2->setInputWrapping(true);
        le2Discount2Validator_->setTop(makeValidatorTop(5));
        ui->le2Discount2->fromSessionField(currentPLUData->discountValue2());

        // FT-1168: fix memory leak
        if ( le2Discount3Validator_ )
            delete le2Discount3Validator_;

        ui->le2Discount3->setValidator(le2Discount3Validator_ = new QIntValidator(0, INT_MAX, this));
        ui->le2Discount3->setDecimalPlaces(2);
        ui->le2Discount3->setInputWrapping(true);
        le2Discount3Validator_->setTop(makeValidatorTop(5));
        ui->le2Discount3->fromSessionField(currentPLUData->discountValue3());
    }

    return yPos;
}

void superChangeProductAllFields::textEditorLimitInputToSize()
{
    //Check fontmetrics of current text document
    //ui->textEditRegion->fontMetrics()
}

void superChangeProductAllFields::editText(QString incomingText, QString labelWidth, QString labelHeight, QString fieldType, QString fontName, QString fontSize)
{
    Q_UNUSED( incomingText );

    labelFormSizeIncomingFont = indexOfFontName(fontName);
    labelFormSizeIncomingFontSize = ui->fontSizeComboBox->findText(fontSize);

    fieldName = fieldType;

    if(fieldName == "description")
        initialContents = formattedDescription;
    else if (fieldName == "expandedText") // Expanded Text
        initialContents = formattedExpandedText;
    else if (fieldName == "specialMessage") // Special Message
        initialContents = formattedSpecialMessage;
    else if (fieldName == "productNote") // Product Note
        initialContents = formattedProductNote;
    else if (fieldName == "nutrifacts") // Nutrifacts
        initialContents = formattedNutrifacts;
    else if (fieldName == "recipes") // Recipe
        initialContents = formattedRecipes;
    else if (fieldName == "cookingInstructions") // Cooking Instruction
        initialContents = formattedCookingInstructions;
    else if (fieldName == "suggestiveSelling") // Suggestive Selling
        initialContents = formattedSuggestiveSelling;
    else if (fieldName == "allergens") // Allergens
        initialContents = formattedAllergens;
    else if (fieldName == "expandedText2") // ExpandedText2
        initialContents = formattedExpandedText2;

    internalConfigData = &session_->scaleConfig->internalConfig;

    setAutoFillBackground( true );

    textEdit = ui->textEditRegion;

    // May have to test this on a 14inch screen to see if we need to skew this
    int maxlabel_width  = (labelWidth.toInt()  + 40);
    int maxlabel_height = (labelHeight.toInt() + 40);

    if( maxlabel_width > 900 )
        maxlabel_width = 900;
    if ( maxlabel_height > 500 )
        maxlabel_height = 500;

    textEdit->setMaximumSize( maxlabel_width, maxlabel_height );
    textEdit->setMinimumSize( maxlabel_width, maxlabel_height );
    textEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    textEdit->resize( maxlabel_width, maxlabel_height );

    editable = true;
    if (!editable)
    {
        textEdit->setEnabled(false);
    }

    lastTextEditHtml = textEdit->document()->toHtml();

    // new
    defaultFontFamily = fontName;
    defaultFontSize = fontSize;

    // old
    defaultLabelWidth = QString::number(internalConfigData->defaultLabelWidth);
    defaultLabelHeight = QString::number(internalConfigData->defaultLabelHeight);

    // get font in text edit region
    textFont = ui->textEditRegion->font();

    // ensure that keyboard stays displayed so force all widgets to not allow focus
    // other than textEditRegion
    if (editable)
    {
        ui->cutButton->setEnabled(false);
        ui->cutButton->setFocusPolicy( Qt::NoFocus );

        ui->copyButton->setFocusPolicy( Qt::NoFocus );
        ui->copyButton->setEnabled(false);

        ui->pasteButton->setFocusPolicy( Qt::NoFocus );
    }
    else
    {
        ui->cutButton->hide();
        ui->copyButton->hide();
        ui->pasteButton->hide();
        ui->fontSizeComboBox->hide();
        ui->fontComboBox->hide();
    }
    ui->cutButton->setIconSize(QSize(60,60));
    ui->pasteButton->setIconSize(QSize(60,60));
    ui->copyButton->setIconSize(QSize(60,60));

    updateFontSizeComboBox();

    bool changeTheDefaultFont = false;

    if(fontName != "")
    {
        changeTheDefaultFont = true;
        decodeBufferIncomingFontName = fontName;       // new code implemented
        defaultFontFamily = fontName;                  // new code implemented
        defaultFontSize = fontSize;

        int myIndex = ui->fontComboBox->findText(fontName);
        ui->fontComboBox->setCurrentIndex( myIndex );
        chosenFontIndex = ui->fontComboBox->currentIndex();

        textEdit->setFontFamily(fontName);
    }
    else
    {
        changeTheDefaultFont = false;
        decodeBufferIncomingFontName = "";
    }

    if(fontSize != "")
    {
        changeTheDefaultFont = true;
        decodeBufferIncomingFontSize = fontSize;

        ui->fontSizeComboBox->setCurrentIndex(ui->fontSizeComboBox->findText(fontSize));
        textEdit->setFontPointSize(fontSize.toInt());
    }
    else
    {
        changeTheDefaultFont = false;
        decodeBufferIncomingFontSize = "";
    }

    textSelected = false;

    setFieldNameWatermark(fieldName);


    // check if we should change the default font and point size

    if(changeTheDefaultFont)
    {
        defaultFontFamily = decodeBufferIncomingFontName;
        defaultFontSize = decodeBufferIncomingFontSize;
       // qDebug() << "XX decode family" << decodeBufferIncomingFontName;
       // qDebug() << "XX decode size" << decodeBufferIncomingFontSize;
        defaultFontFamily = fontName;       // new code implemented
        defaultFontSize = fontSize;         // new code implemented
        defaultLabelWidth = QString::number(internalConfigData->defaultLabelWidth);     // new code implemented
        defaultLabelHeight = QString::number(internalConfigData->defaultLabelHeight);   // new code implemented

    }

    if (editable)
    {
            decodeBuffer(initialContents);
    }
    else
    {
        if (initialContents != "")
        {
            ui->watermark->hide();
            decodeBuffer(initialContents);
        }
        else
            ui->watermark->show();
    }

    textEdit->setFocus();

    //textEdit->zoomIn(+3);
    this->ui->frameTextEditor->raise();
    this->ui->frameTextEditor->show();
}

QString superChangeProductAllFields::getEncodedMarqueeTextOnly(QString marqueeText)
{
    // give just plaintext of a marquee encoded text string
    // strip out the codes
    marqueeText.replace("\\S","");
    marqueeText.replace("\\F","");
    marqueeText.replace("\\B","");
    marqueeText.replace("\\R","");
    marqueeText.replace("\\H","");
    marqueeText.replace("\\C","");
    marqueeText.replace("\\L","");
    marqueeText.replace("\\N","");
    marqueeText.replace("\\Q","");
    marqueeText.replace("\\X","");
    return marqueeText;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superChangeProductAllFields::onScreenChanging()
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

    saveEntireProduct();

    if( wroteTransaction )
        session_->send(ProductDev, ProductSaveSupervisorTransaction);
    else if( (savefbcTransaction) && ( 1 == currentPLUData->byCount() ) && ( ui->chkbox2ForcedByCount->isChecked() == false) )
        session_->send(ProductDev, ProductSaveSupervisorTransaction);

    whoHasFocus = SAV_NONE;
    wroteTransaction = false;

    showAllFieldsReuseMarquee_ = false;
    showAllFieldsReuseNutrifacts_ = false;

    if (( !internalConfigData->changeProductActive ) &&             // if we are not going to another Change Product Page
        ( !goingToMarqueeScreen ) &&                                // FT-978
        ( !goingToNutrifactsEditorScreen ) &&                       // FT-1007
        ( !goingToLabelPlacementView )
    )
    {
        session_->send(ProductDev, ProductRead, "0");               // clear out currentPLU
    }

    savefbcTransaction = false;

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superChangeProductAllFields", "onSessionMessage" );

    if(ProductDev == msg[0].toInt() && ProductWrite == msg[1].toInt())
    {
        ////////////////////////////////////////////
        //  P L U   L O O K U P   R E S P O N S E // 101,7
        ////////////////////////////////////////////


        switchToMainTab = false;
        // FT-1012 Tare Precision
        // If the incoming digits do not match the actual tare precision, it messes stuff
        // up. Whatever the incoming number of digits is for the tare value, it overrides
        // the setDecimals() value of the tare QValidator. So right here I had to enforce
        // the number of tare digits to match the scaleCOnfig.tarePrecision.

        Product_Number_Orig = "";
        Vendor_Number_Orig = "";
        EAN_Code_Orig = "";

        if ( 0 < currentPLUData->itemNumber().toInt() )
        {
            QString tare( scaleConfigData->useSecondTare() ? currentPLUData->tareB() : currentPLUData->tare() );

            // Internal Fix: fix crash when forced tare (i.e. empty field)
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

        Product_Number_Orig = currentPLUData->productNumber();
        Vendor_Number_Orig = currentPLUData->vendorNumber();
        EAN_Code_Orig = currentPLUData->eanCode();
        }

        // FT-528
        updatingFromIncomingPLUData = 1;

        initialize();       // terrible this is called each time
        updateProductInformation();

        // check the pick 5 limits
        onPriceEditedFinished();
        onWeightEditedFinished();

        updatingFromIncomingPLUData = 0;
        //loadIncomingPLUData(msg);

        // FT-1033
        if ( restoreReuseTextType.length() )
        {
            reuseTextType = restoreReuseTextType;
            restoreReuseTextType.clear();
        }
    }
    else if(ProductDev == msg[0].toInt() && ProductClearUI == msg[1].toInt() && 1 == msg[2].toInt())
    {
        //101,47 Clear after a delete
        //currentPLUData->itemNumber() = "0";
        previousPLUItemNumber = "0";
        switchToMainTab = true;
        initialize();
    }
    else if(ProductDev == msg[0].toInt() && ProductWriteSingleField == msg[1].toInt() ) // && 1 == msg[2].toInt() && 2 == msg[3].toInt())
    {
        // we can possibly receive multiple different single field writes here, so must test accordingly

        int currentOffset = msg[3].toInt();
        // I wonder if this is a bug? msg[2] holds the plu number, why test against that? What am I missing?
        if(  1 == msg[2].toInt() && 2 == currentOffset )
        {
            ////////////////////////////////////////////
            //  P L U PRODUCT TYPE CHANGE   R E S P O N S E // 101,16
            ////////////////////////////////////////////
            // It is legal change and product types match

            this->loadPricingTab(msg[4].toInt());
        }
        else if( Product::fieldOffsetProdCoolDefaultTextNum == currentOffset ) // rem to test against -1 less than the offset in Messages->currentPLU
        {
            // This class can have multiple single field writes received so we have to parse them out.
            // COOL - 101,16 - used by COOL tab when cool short list sends a single field write
            // update the coolDefaultTextNum();
            updateRunModeValue(msg);
        }
        else if ( Product::fieldOffsetProductReportingClassNum == currentOffset )
        {
            // reporting class
            updateRunModeValue(msg);            // 101,16 telling us if it is valid .... or not
        }
        else if( Product::fieldOffsetGraphicName1 <= currentOffset && Product::fieldOffsetGraphicName4 >= currentOffset )
        {   // 37-40
            // all label graphics - are they valid
            updateRunModeValue(msg);
        }

        else if( Product::fieldOffsetDefaultLabelTypeNum1 <= currentOffset && Product::fieldOffsetOnlineOrderingLabelTypeNum3 >= currentOffset )
        {   // 111-125
            // all labels - are they valid
            updateRunModeValue(msg);

            if( 1 == msg[2].toInt() )
            {
                // SOF-3894 Nutrifacts can't be entered when first creating a PLU in All Fields
                // If this is a valid label to be updated (if test above) then save the product so we can get
                // all of the label information when requested
                saveEntireProduct();
                session_->send(ProductDev, ProductRead, ui->leProductNumber->text());
            }
        }
    }
    else if (Error == msg[0].toInt() && ErrProductDevice == msg[1].toInt() && 101001 == msg[2].toInt() && 4 == msg[3].toInt())
    {
        /////////////////////////////////////
        //  P L U   L O O K U P   I T E M  //
        //   C O D E   N O T   F O U N D   // 103,101,101001,4
        //   C R E A T E   N E W   P L U   //
        /////////////////////////////////////
        popupActive = true;
        ui->gbNewBlocker->move( scalePointForScreen( QPoint( 4, 4 ) ) );
        ui->lblNewQuestion->setText(tr("Product") + " " + msg[4] + " " + tr("does not exist. Create new product?"));

        ui->gbNewBlocker->show();
        ui->butDeleteProduct->setVisible( false );
    }
    else if (Error == msg[0].toInt() && ErrProductDevice == msg[1].toInt() && 101026 == msg[2].toInt() && 1 == msg[4].toInt() && 1 == msg[7].toInt())
    {
        // delete product
        // 101,8
        session_->send(ProductDev, ProductDelete);
        session_->send(ProductDev, ProductRead, "-1");
    }
    else if (GraphicDev == msg[0].toInt() && GraphicReadAllNames == msg[1].toInt())
    {
        loadLabelGraphicNames(msg);
    }
    else if (CategoryDev == msg[0].toInt() && CategoryDtWriteAll == msg[1].toInt())
    {
        loadCategoriesCombo(msg);
    }
    else if (SpecialsDev == msg[0].toInt() && SpecialsDtWriteAll == msg[1].toInt())
    {
        loadSpecialsCombo(msg);
    }
    else if (ProductDev == msg[0].toInt() && ProductWriteAllSoftKeyGraphicNames == msg[1].toInt())
    {
        loadFlashkeyGraphicsCombo(msg);
    }
    else if(ProductDev == msg[0].toInt() && ProductWriteLabelFormSize == msg[1].toInt() && currentPLUData->itemNumber() == msg[2])
    {
        // FT-107: use nutrifacts editor for nutrifacts
        if ( "nutrifacts" == reuseTextType )
        {
            showAllFieldsReuseMarquee_ = false;
            showAllFieldsReuseNutrifacts_ = true;

            goingToNutrifactsEditorScreen = true;
            mainWindow_->switchToModeScreen<SupervisorMode, superNutritionEditorScreen>();
        }
        else
        {
            if(isTabMainClicked)
            {
                isTabMainClicked = false;
                return;
            }
            //text editor label size for product description
                        //    height  width                   font    fontsize
            decodeBufferIncomingFontName = msg[6];
            decodeBufferIncomingFontSize = msg[7];
            this->editText("", msg[5], msg[4], reuseTextType ,msg[6], msg[7]);

            //QString incomingText, QString labelWidth, QString labelHeight, QString fieldType
        }
    }
    else if(ProductDev == msg[0].toInt() && ProductWriteSupportData == msg[1].toInt())
    {
        // EX. 101,10,55,1,0,,,"expandedtext here","","",""
        messageTextType = msg[3].toInt();
        bool loadReuse = true;
        DomPluSupport m(msg);
        switch (messageTextType)
        {
        case 1:
            // Expanded Text
            messageTextText = "expandedText";
            break;
        case 2:
            // Special Message
            messageTextText = "specialMessage";
            break;
        case 3:
            // Product Note
            messageTextText = "productNote";
            break;
        case 4:
            // Product Marquee
            messageTextText = "productMarquee";

            ui->teReuse->setPlainText((m.supportText()));
            if (session_->isServiceCounter())
                ui->te5ProductMarquee->setText(m.supportText());
            ui->tabChangeProductAllFields->setCurrentIndex(4);
            //loadReuseFrame(m, messageTextType);
            formattedProductMarquee = m.supportText();
            ui->frameReuseText->raise();
            ui->frameReuseText->show();
            if(pluSupport_)
            {
                *pluSupport_ = m;
            }

            break;
        case 5:
            // Cooking Instructions
            messageTextText = "cookingInstructions";
            break;
        case 6:
            // Recipes
            messageTextText = "recipes";
            break;
        case 7:
            // Expanded Text 2
            messageTextText = "expandedText2";
            break;
        case 8:
            // Suggestive Selling
            messageTextText = "suggestiveSelling";
            break;
        case 9:
            // Allergens
            messageTextText = "allergens";
            break;
        case 20:
            // Nutrifacts
            loadReuse = true;
            messageTextText = "nutrifacts";
            ui->te4Nutrifacts->removeEventFilter(this);
            if(m.supportText().isEmpty() && m.supportText2().isEmpty() && m.supportText3().isEmpty() && m.supportText4().isEmpty() && m.supportText5().isEmpty())
            {
                ui->te4Nutrifacts->setText(tr("No nutrifacts data"));
            }
            else
            {
                ui->te4Nutrifacts->setText(tr("Nutrifacts data exists"));
            }
            ui->te4Nutrifacts->installEventFilter(this);
            this->update();
            break;
        }
        reuseMessageVar = msg;
        if(loadReuse) this->loadReuseFrame(msg, messageTextText);
    }
    else if(ProductDev == msg[0].toInt() && ProductShowLinksToSupportData == msg[1].toInt())
    {
        //101,23,#links,textOfLinks-1,2,55
        ui->teListOfProductsThatUseThisText->setText(msg[3]);
    }
    else if(ExpandedTextDev == msg[0].toInt() && ExpandedDtWrite == msg[1].toInt())
    {
        //106,2
        formattedExpandedText = msg[3];
        this->ui->te5ExpandedText->setText(getEncodedTextOnly(msg[3]));
    }
    else if(SpecialMessageDev == msg[0].toInt() && SpecialDtWrite == msg[1].toInt())
    {
        //107,2
        formattedSpecialMessage = msg[3];
        this->ui->te5SpecialMessage->setText(getEncodedTextOnly(msg[3]));
    }
    else if(ProductNotesDev == msg[0].toInt() && ProductDtWrite == msg[1].toInt())
    {
        //108,2
        formattedProductNote = msg[3];
        this->ui->te5ProductNote->setText(getEncodedTextOnly(msg[3]));
    }
    else if(CookingInstructionsDev == msg[0].toInt() && CookingInstructionsDtWrite == msg[1].toInt())
    {
        //165,2
        formattedCookingInstructions = msg[3];
        this->ui->te5CookingInstructions->setText(getEncodedTextOnly(msg[3]));
    }
    else if(RecipeDev == msg[0].toInt() && RecipeDtWrite == msg[1].toInt())
    {
        //166,2
        formattedRecipes = msg[3];
        this->ui->te5Recipes->setText(getEncodedTextOnly(msg[3]));
    }
    else if(ExpandedText2Dev == msg[0].toInt() && ExpandedText2DtWrite == msg[1].toInt())
    {
        //167,2
        formattedExpandedText2 = msg[3];
        this->ui->te5ExpandedText2->setText(getEncodedTextOnly(msg[3]));
    }
    else if(SuggestiveSellingDev == msg[0].toInt() && SuggestiveSellingDtWrite == msg[1].toInt())
    {
        //168,2
        if (session_->isServiceCounter())       //  HA-72
        {
            formattedSuggestiveSelling = msg[3];
            this->ui->te5SuggestiveSelling->setText(getEncodedTextOnly(msg[3]));
        }
    }
    else if(AllergensDev == msg[0].toInt() && AllergensDtWrite == msg[1].toInt())
    {
        //169,2
        formattedAllergens = msg[3];
        this->ui->te5Allergens->setText(getEncodedTextOnly(msg[3]));
    }
    else if(MarqueeTextDev == msg[0].toInt() && MarqueeTextDtWrite == msg[1].toInt())
    {
        //137,2
        if (session_->isServiceCounter())       //  HA-72
        {
            formattedProductMarquee = msg[3];
            this->ui->te5ProductMarquee->setText(getEncodedMarqueeTextOnly(msg[3]));
            if(supervisorMode_->cameFromProductMarqueeEditor())
            {
                ui->teReuse->setText(this->ui->te5ProductMarquee->toPlainText());
            }
        }
    }
    else if( COOLDev == msg[0].toInt() && COOLDevWriteAllPretexts == msg[1].toInt() )
    {
        // COOL - 135,63, build a pretext list according to its category, sent 135,61
        buildAllPretextsList(msg);
    }
    else if( COOLDev == msg[0].toInt() && COOLDevWriteAllCategories == msg[1].toInt() )
    {
        // COOL - 135,83, build a Categories list according to its product number, sent 135,81
        buildAllCategoriesList(msg);
    }
    else if( COOLDev == msg[0].toInt() && COOLDevShortlistWriteAllCoolText == msg[1].toInt() )
    {
        // COOL - 135,114, build a shortlist list according to its product number, sent 135,113
        buildAllCoolTextByProduct(msg);
    }
    else if( COOLDev == msg[0].toInt() && COOLDevWritePretextsByCategory == msg[1].toInt() )
    {
        // COOL - 135,74, build a pretext list according to its category, sent 135,78
        buildPretextsListByCategory(msg);
    }
}

void superChangeProductAllFields::loadReuseFrame(const SessionMessage& msg, QString textType)
{
    DomPluSupport m(msg);
    QString message;
    message = m.pluNum() + ",";
    reuseTextType = textType;
    aspUsedByMany = m.usedByMany();
    aspSupportPLU = m.supportPLU();
    aspSupportText = m.supportText();
    aspSupportText2 = m.supportText2();
    aspSupportText3 = m.supportText3();
    aspSupportText4 = m.supportText4();
    aspSupportText5 = m.supportText5();


    if(textType == "expandedText")
    {
        message += "1";
        aspTextType = "1";
        ui->reuseTextHeader->setText(tr("Expanded Text"));
        formattedExpandedText = m.supportText();
    }
    else if (textType == "specialMessage")
    {
        message += "2";
        aspTextType = "2";
        ui->reuseTextHeader->setText(tr("Special Message"));
        formattedSpecialMessage = m.supportText();
    }
    else if (textType == "productNote")
    {
        message += "3";
        aspTextType = "3";
        ui->reuseTextHeader->setText(tr("Product Note"));
        formattedProductNote = m.supportText();
    }
    else if(textType == "productMarquee")
    {
        message += "4";
        aspTextType = "4";
        ui->reuseTextHeader->setText(tr("Product Marquee"));
        formattedProductMarquee = m.supportText();
    }
    else if(textType == "cookingInstructions")
    {
        message += "5";
        aspTextType = "5";
        ui->reuseTextHeader->setText(tr("Cooking Instructions"));
        formattedCookingInstructions = m.supportText();
    }
    else if(textType == "recipes")
    {
        message += "6";
        aspTextType = "6";
        ui->reuseTextHeader->setText(tr("Recipe"));
        formattedRecipes = m.supportText();
    }
    else if(textType == "expandedText2")
    {
        message += "7";
        aspTextType = "7";
        ui->reuseTextHeader->setText(tr("Expanded Text 2"));
        formattedExpandedText2 = m.supportText();
    }
    else if(textType == "suggestiveSelling")
    {
        message += "8";
        aspTextType = "8";
        ui->reuseTextHeader->setText(tr("Suggestive Selling"));
        formattedSuggestiveSelling = m.supportText();
    }
    else if(textType == "allergens")
    {
        message += "9";
        aspTextType = "9";
        ui->reuseTextHeader->setText(tr("Allergens"));
        formattedAllergens = m.supportText();
    }
    else if(textType == "nutrifacts")
    {
        message += "20";
        aspTextType = "20";
        ui->reuseTextHeader->setText(tr("Nutrifacts"));
        formattedNutrifacts = m.supportText();
    }


    session_->send(ProductDev,ProductShowLinksToSupportData, message);

    if(textType == "productMarquee")
    {
       // qDebug() << "before marquee is: " << m.supportText();
        ui->teReuse->setText(m.marqueeSupportTextStripped());

        //qDebug() << "after marquee is : " << m.marqueeSupportTextStripped();
        //qDebug() << "formatted marquee is: " << formattedProductMarquee;
    }
    else if(textType == "nutrifacts")
    {
        ui->teReuse->setText(ui->te4Nutrifacts->toPlainText());
    }
    else
    {
        ui->teReuse->setText(getEncodedTextOnly(m.supportText()));
    }
    mainWindow_->hideInputWidgets();
    ui->frameReuseText->raise();
    ui->frameReuseText->show();
    //mainWindow_->hideInputWidgets();

}

void superChangeProductAllFields::setFieldNameWatermark(QString fieldName)
{
    QBrush brush(QColor(205, 205, 205, 205));
    brush.setStyle(Qt::SolidPattern);

    QPalette palette;

    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);

    ui->watermark->setPalette(palette);
    ui->watermark->setText(fieldName);
}

// set text editor region font based on font combo box
void superChangeProductAllFields::setTextEditRegionFont()
{
    ui->textEditRegion->setFont(ui->fontComboBox->currentFont() );
}

void superChangeProductAllFields::setTextEditRegionFontSize()
{
    // get the currently selected font size in font size combo box
    QString fontSizeStr = QString::number(ui->fontSizeComboBox->currentText().toInt() *2);

    QFont currentFontSpec = ui->fontComboBox->currentFont();
    QFont newFontSpec(currentFontSpec.family(),fontSizeStr.toInt(),-1,false);

    ui->textEditRegion->setFont(newFontSpec);
}

void superChangeProductAllFields::updateFontSizeComboBox()
{
    QFontDatabase fdb;

    ui->fontSizeComboBox->clear();
    // load into font size combo box the family's point sizes
    // Hardcoded sizes for combobox added 9/12/2013

    // Font sizes 3 and 4 removed per Jim Highley on 9/13/2013
    ui->fontSizeComboBox->addItem("5");
    ui->fontSizeComboBox->addItem("6");
    ui->fontSizeComboBox->addItem("7");
    ui->fontSizeComboBox->addItem("8");
    ui->fontSizeComboBox->addItem("9");
    ui->fontSizeComboBox->addItem("10");
    ui->fontSizeComboBox->addItem("11");
    ui->fontSizeComboBox->addItem("12");
    ui->fontSizeComboBox->addItem("13");
    ui->fontSizeComboBox->addItem("14");
    ui->fontSizeComboBox->addItem("15");
    ui->fontSizeComboBox->addItem("16");
    ui->fontSizeComboBox->addItem("18");
    ui->fontSizeComboBox->addItem("20");
    ui->fontSizeComboBox->addItem("22");
    ui->fontSizeComboBox->addItem("24");
    ui->fontSizeComboBox->addItem("26");
    ui->fontSizeComboBox->addItem("28");
    ui->fontSizeComboBox->addItem("34");
    ui->fontSizeComboBox->addItem("36");
    ui->fontSizeComboBox->addItem("48");
    ui->fontSizeComboBox->addItem("60");
    ui->fontSizeComboBox->addItem("72");

    // OLD CODE
    /*foreach (int size, fdb.standardSizes())
    {
        // insert all point size values valid for font family
        ui->fontSizeComboBox->addItem(QString::number(size));
    }*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::doneEditing()
{
    //LOG_CLASS_INFO( "textEditor", "doneEditing" );

    // ui->textEditRegion->setText("");

    QString msg;
    // process qTextEdit contents if text modified
    encodeBuffer();

    if(reuseTextType == "description")
    {
        // save single field description
        qDebug() << "formatted Description is : " << formattedDescription;
        //msg = currentPLUData->itemNumber() + ",3,\"" + formattedDescription + "\",\"\",0";
        msg = currentPLUData->itemNumber() + "," + QString::number( Product::fieldOffsetDescription ) + "," + "\"" + formattedDescription + "\"" + "," + "\"\"" + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "expandedText")
    {
        //101,10,55,1,1,0,,"fn=Agency FB;ps=12epxanfn=Agency FB;ps=18dedText heretodd"
        //C.Send: 000012101,22
        msg = currentPLUData->itemNumber() + ",1,0,0,,\"" + formattedExpandedText + "\"";
        //msg = currentPLUData->itemNumber() + ",1,0,0,,\"something\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
        //session_->send(ProductDev,ProductReadSupportData, currentPLUData->itemNumber() + ",1" );
    }
    else if(reuseTextType == "specialMessage")
    {
        msg = currentPLUData->itemNumber() + ",2,0,0,,\"" + formattedSpecialMessage + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "productNote")
    {
        msg = currentPLUData->itemNumber() + ",3,0,0,,\"" + formattedProductNote + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "productMarquee")
    {
        msg = currentPLUData->itemNumber() + ",4,0,0,,\"" + formattedProductMarquee + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "cookingInstructions")
    {
        msg = currentPLUData->itemNumber() + ",5,0,0,,\"" + formattedCookingInstructions + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "recipes")
    {
        msg = currentPLUData->itemNumber() + ",6,0,0,,\"" + formattedRecipes + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "expandedText2")
    {
        msg = currentPLUData->itemNumber() + ",7,0,0,,\"" + formattedExpandedText2 + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "suggestiveSelling")
    {
        msg = currentPLUData->itemNumber() + ",8,0,0,,\"" + formattedSuggestiveSelling + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "allergens")
    {
        msg = currentPLUData->itemNumber() + ",9,0,0,,\"" + formattedAllergens + "\"";
        session_->send(ProductDev, ProductWriteSupportData, msg);
        session_->send(ProductDev, ProductSave);
    }
    else if(reuseTextType == "nutrifacts")
    {
        //msg = currentPLUData->itemNumber() + ",20,0,0,,\"" + formattedProductNote + "\"";
        //session_->send(ProductDev, ProductWriteSupportData, msg);
        //session_->send(ProductDev, ProductSave);
    }

    useFontChanges = true;                // If this is true, we expect, by default, for their to be font change sequences next time
    this->ui->frameTextEditor->lower();
    this->ui->frameTextEditor->hide();
    this->ui->butReuseDone->setFocus();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString superChangeProductAllFields::getEncodedTextOnly(QString incomingString)
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
            //if (dataItem.trimmed() != "")
            {
                result += dataItem;
            }
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::encodeBuffer()
{
    LOG_CLASS_INFO( "textEditor", "encodeBuffer" );

    //textEdit->setAutoFormatting(QTextEdit::AutoNone);
    QString myHtml = textEdit->document()->toHtml();
   // qDebug() << "myHtml: " << myHtml;
    QXmlStreamReader reader(textEdit->document()->toHtml());

    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;
    QString currentFont="";
    QString currentFontSize="";
    QString currentText;

    QString lastFont="";
    QString lastFontSize="";

    encodedBuffer.clear();
    bool isFirstRead = true;
    int newLineCount = 0;
    bool addDefaultFormat = false;          // new code implemented

    while (!reader.atEnd())
    {
        if ( isFirstRead )
            isFirstRead = false;
        else
        {
            reader.readNext();
        }

        if (reader.isStartElement()) {
            //qDebug() << "start Element: " << reader.name();

            //QString myReaderText = reader.text().toString();
            //QString myReaderName = reader.name().toString();

            if ((reader.name() == "span") /*|| (reader.name() == "body")*/)
            {
                if (reader.attributes().hasAttribute(QString("style")))
                {
                    // read style attribute data
                    QString styleSpecString = reader.attributes().value("style").toString();
                    // parse attributes based on semicolon
                    QStringList styleSpecItems = styleSpecString.split(QChar(';'),QString::SkipEmptyParts,Qt::CaseInsensitive);
                    foreach(QString styleSpec, styleSpecItems)
                    {
                        // parse style items based
                        QStringList styleTypeItems = styleSpec.split(QChar(':'),QString::KeepEmptyParts,Qt::CaseInsensitive);
                        if(styleSpecString.contains("font-family"))
                        {
                            if (styleTypeItems[0].trimmed() == "font-family")
                            currentFont = styleTypeItems[1].replace("'","");
                        }
                        else currentFont = decodeBufferIncomingFontName;

                        if (styleTypeItems[0].trimmed() == "font-size")
                            currentFontSize = styleTypeItems[1].replace("pt","");
                        currentFontSize = currentFontSize.replace("px","");
                    }

                    if((indexOfFontName(currentFont) != labelFormSizeIncomingFont || ui->fontSizeComboBox->findText(currentFontSize) != labelFormSizeIncomingFontSize) || addDefaultFormat)
                    {
                        if(/*addDefaultFormat &&*/ useFontChanges)        // new code implemented - The if part is new
                        {
                            encodedBuffer.append(doubleEscSeq);
                            encodedBuffer.append("fn=");
                            if(currentFont == "SP") currentFont = "Sp";
                            else if (currentFont == "SPLarge") currentFont = "SpLarge";
                            encodedBuffer.append(currentFont);
                            encodedBuffer.append(";ps=");
                            encodedBuffer.append(QString::number(currentFontSize.toInt() / 2));
                            encodedBuffer.append(escSeq);
                            addDefaultFormat = true;
                        }
                    }
                    if(currentFont == defaultFontFamily && currentFontSize == defaultFontSize && useFontChanges)  // new code implemented - the if is new
                    {
                        addDefaultFormat = true;
                    }
                    // Check the font specs with debug
                }
            }
        }



        // text of tag
        else if (reader.isCharacters())
        {
           // qDebug() << "chars are: " << reader.text().toString();
            if(reader.text().toString() != "\np, li { white-space: pre-wrap; }\n" && reader.text().toString() != "\n")
            {
                //encodedBuffer.append(reader.text().toString().replace('"',"\""));
                // Replaced the line above with this line below
                QString intermediateText = reader.text().toString();
                intermediateText = intermediateText.replace('"', "\"\"");

                encodedBuffer.append(intermediateText);
                //encodedBuffer.append(reader.text().toString().replace('"',"\"\""));

            }
            else if(reader.text().toString() == "\n")
            {
                if(newLineCount > 0)
                {
                    encodedBuffer.append("\n");
                }
                newLineCount++;

            }

            LOG_DEBUG( "characters = %s", currentText.toLatin1().constData() );
        }
    }

    // right trim all whitespace and carriage control
    encodedBuffer = rstrip(encodedBuffer);

    if(fieldName == "description")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedDescription = encodedBuffer;
            this->ui->te1Description->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedDescription = "";
            this->ui->te1Description->clear();
        }
    }
    else if (fieldName == "expandedText")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedExpandedText = encodedBuffer;
            this->ui->te5ExpandedText->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedExpandedText = "";
            this->ui->te5ExpandedText->clear();
            // Debug - The te5ExpandedText should be empty at this point
        }
    }
    else if (fieldName == "expandedText2")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedExpandedText2 = encodedBuffer;
            this->ui->te5ExpandedText2->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedExpandedText2 = "";
            this->ui->te5ExpandedText2->clear();
        }
    }
    else if (fieldName == "specialMessage")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedSpecialMessage = encodedBuffer;
            this->ui->te5SpecialMessage->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedSpecialMessage = "";
            this->ui->te5SpecialMessage->clear();
        }
    }
    else if (fieldName == "productNote")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedProductNote = encodedBuffer;
            this->ui->te5ProductNote->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedProductNote = "";
            this->ui->te5ProductNote->clear();
        }
    }
    else if (fieldName == "nutrifacts")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedNutrifacts = encodedBuffer;
            this->ui->te4Nutrifacts->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedNutrifacts = "";
            this->ui->te4Nutrifacts->clear();
        }
    }
    else if (fieldName == "recipes")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedRecipes = encodedBuffer;
            this->ui->te5Recipes->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedRecipes = "";
            this->ui->te5Recipes->clear();
        }
    }
    else if (fieldName == "cookingInstructions")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedCookingInstructions = encodedBuffer;
            this->ui->te5CookingInstructions->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedCookingInstructions = "";
            this->ui->te5CookingInstructions->clear();
        }
    }
    else if (fieldName == "suggestiveSelling")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedSuggestiveSelling = encodedBuffer;
            this->ui->te5SuggestiveSelling->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedSuggestiveSelling = "";
            this->ui->te5SuggestiveSelling->clear();
        }
    }
    else if (fieldName == "allergens")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedAllergens = encodedBuffer;
            this->ui->te5Allergens->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedAllergens = "";
            this->ui->te5Allergens->clear();
        }

    }
    if(ui->frameReuseText->isVisible())
    {
        ui->teReuse->clear();
        ui->teReuse->setText(ui->textEditRegion->toPlainText());
    }
    this->textEdit->clear();
    //qDebug() << "encodedBuffer final is : " << encodedBuffer;
    defaultFontFamily = internalConfigData->defaultFontName;
    defaultFontSize = QString::number(internalConfigData->defaultFontSize);
    //qDebug() << "this is clearing text";
    QFont clearingFont;
    clearingFont.setFamily(defaultFontFamily);
    clearingFont.setPointSize(defaultFontSize.toInt() * 2);
    font4Backspacing = clearingFont;
    decodeBufferIncomingFontName = "";
    decodeBufferIncomingFontSize = "";
}

// TODO: I had to implement string right trim
// No right string strip in Qt?
QString superChangeProductAllFields::rstrip(const QString& str, const char *skip) {
  int n = str.size() - 1;
  for (; n >= 0; --n) {
    if (0 == strchr(skip, str.at(n).toLatin1()))
      return str.left(n + 1);
  }
  return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::decodeBuffer(QString contents)
{
    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;

    QString attribute;
    QString displayBuffer;
    QXmlStreamWriter stream(&displayBuffer);

   // qDebug() << " decoding is: " << contents;
    //if (contents.trimmed() == "")
    //    return;
    //contents = contents.trimmed();
    // fn=[font family name];ps=[point size];
    QRegExp rx("^fn=([a-z,A-Z,0-9\\s]+);ps=(\\d\\d?)$");

    // Split contents
    QStringList dataItems = contents.split( escSeq );

    // If the size is 1, then no font change sequences were found in the incoming string,
    // therefore, when we save it off to the application, we better not add any on the way out.
    if(dataItems.size() == 1)
        useFontChanges = false;

    QString currentFontFamily;
    if(decodeBufferIncomingFontName != "")
    {
        currentFontFamily = decodeBufferIncomingFontName;
    }
    else
    {
        currentFontFamily = defaultFontFamily;
    }

   // decodeBufferIncomingFontName = "";


    QString currentFontSize;

    if(decodeBufferIncomingFontSize != "")
        currentFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
    else
        currentFontSize = QString::number(defaultFontSize.toInt() * 2);

    foreach(QString dataItem, dataItems)
    {
        // if data item is a font specifier
        if (rx.indexIn(dataItem) > -1 )
        {
            // capture the font specification found
            // cap(0) is the entire font specification matched
            // so start at cap(1)
            currentFontFamily = rx.cap(1);
            //if(rx.cap(2).toInt() < 5)
            //{
            //    currentFontSize = "10";
            //}
            //else
            {
                currentFontSize = QString::number(rx.cap(2).toInt() * 2);
            }
        }
        // else text found
        else
        {
            // only add if text found
            //if (dataItem.trimmed() != "")
            {
                fontFamilies.append(currentFontFamily);
                fontSizes.append(currentFontSize);
                fontTextItems.append(dataItem);
            }
        }
    }

    // generate HTML representation of input
    stream.setAutoFormatting(false);
    stream.writeStartDocument();
    stream.writeStartElement("html");
    stream.writeStartElement("head");
    stream.writeStartElement("meta");
    stream.writeAttribute("name","qrichtext");
    stream.writeAttribute("content","1");
    stream.writeEndElement();//meta
    //stream.writeEndElement("head"); //head
    stream.writeEndElement();//head
    stream.writeStartElement("body");
    attribute = "";

    QFont defaultFont;
    if(decodeBufferIncomingFontName != "")
    {
        defaultFont.setFamily(decodeBufferIncomingFontName);
        defaultFontFamily = decodeBufferIncomingFontName;
    }
    else
    {
        defaultFont.setFamily(defaultFontFamily);
    }
    if(decodeBufferIncomingFontSize != "")
    {
        defaultFont.setPointSize(decodeBufferIncomingFontSize.toInt() * 2);
        defaultFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
    }
    else
    {
        defaultFont.setPointSize(defaultFontSize.toInt() * 2);
    }

        this->fontChanged(defaultFont);

        textEdit->setCurrentFont(defaultFont);
        textEdit->document()->setDefaultFont(defaultFont);

        //OLD
  //  attribute.append(" font-family:'" + defaultFontFamily + "';");
  //  attribute.append(" font-size:" + defaultFontSize + "px;");
        //NEW
       // attribute.append(" text-decoration-color:red;");
        attribute.append(" font-family:'" + defaultFont.family() + "';");
    attribute.append(" font-size:" + QString::number(defaultFont.pointSize()) + "px;");
    stream.writeAttribute("style", attribute);

    // traverse all fontspecs in buffer
    for (int i=0; i < fontFamilies.size(); i++)
    {
        // generate span style tag


        attribute = "";

        attribute.append(" font-family:'" + fontFamilies[i] + "';");
        attribute.append(" font-size:" + QString::number(fontSizes[i].toInt()) + "pt;");
      //  attribute.append(" text-decoration-color: red;");
        stream.writeStartElement("span");
        stream.writeAttribute("style", attribute);

        QString holder = "";
        if (fontTextItems[i].count('\n') > 0)
        {
            //stream.writeCharacters(fontTextItems[i].replace("\n", "\n"));
            //split the textitem up looking for multiple \n's

            QStringList newLinesInFontTextItem;
            newLinesInFontTextItem = fontTextItems[i].split("\n", QString::KeepEmptyParts);

            for(int j = 0; j < newLinesInFontTextItem.size(); j++)
            {
                // write start tag for paragraph
                if(j > 0)
                {
                    holder = "\n";
                    //stream.writeStartElement("br");
                    stream.writeStartElement("p");
                    attribute = "";
                    attribute.append(" margin-top:0px;");
                    attribute.append(" margin-bottom:0px;");
                    attribute.append(" margin-left:0px;");
                    attribute.append(" margin-right:0px;");
                    attribute.append(" -qt-block-indent:0;");
                    attribute.append(" text-indent:0px;");
                    stream.writeAttribute("style", attribute);
                      //stream.writeEmptyElement("p");
                  //  stream.writeCharacters("\n");
                    //stream.writeStartElement("br");
                    //stream.writeEndElement();

                    if(newLinesInFontTextItem[j] == "")
                    {
                        stream.writeCharacters("");
                        //stream.writeStartElement("br");
                        //stream.writeEndElement();
                    }
                    else
                    {
                        QString interText = newLinesInFontTextItem[j].replace(QString("\"\""),QString("\""));
                        stream.writeCharacters(interText);
                        //stream.writeCharacters(newLinesInFontTextItem[j]);
                    }
                    stream.writeEndElement();

                }
                else
                {
                    QString interText1 = newLinesInFontTextItem[j].replace(QString("\"\""),QString("\""));
                    stream.writeCharacters(interText1);

                    //stream.writeCharacters(newLinesInFontTextItem[j]);
                }
            }

        }
        else
        {
            // write out characters
            QString interText2 = fontTextItems[i].replace(QString("\"\""),QString("\""));
            stream.writeCharacters(interText2);

            //stream.writeCharacters(fontTextItems[i]);
           // stream.writeCharacters("TODD");
        }

        // new SEP 19
        stream.writeEndElement();
    }

    stream.writeEndElement(); // body
    stream.writeEndElement(); // html
    stream.writeEndDocument();

    textEdit->document()->clear();
   // qDebug() << "DECODE display buffer is: " << displayBuffer;

    displayBuffer = rstrip(displayBuffer);
    textEdit->document()->setHtml(displayBuffer);

    QTextCursor cursor = textEdit->textCursor();
    textEdit->mergeCurrentCharFormat(cursor.charFormat());
    //decodeBufferIncomingFontName = "";
   // decodeBufferIncomingFontSize = "";

   // qDebug() << "decode html is: " << textEdit->toHtml();

}

void superChangeProductAllFields::textFamily( const QString& f )
{
    Q_UNUSED( f );

    // if we make changes, then this must be true. Could of been false on the way in
    useFontChanges = true;

    QString fontFamily( ui->fontComboBox->itemText( chosenFontIndex = ui->fontComboBox->currentIndex() ) );
    qreal fontPointSize( ui->fontSizeComboBox->itemText( ui->fontSizeComboBox->currentIndex() ).toFloat() * 2.0 );

    QTextCharFormat fmt;
    fmt.setFontFamily( fontFamily );

    if ( 0 < fontPointSize )
        fmt.setFontPointSize( fontPointSize );

    mergeFormatOnWordOrSelection( fmt );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::textSelect(bool yes)
{
    textSelected = yes;
    ui->cutButton->setEnabled(yes);
    ui->copyButton->setEnabled(yes);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::textSize( const QString& p )
{
    // if we make changes, then this must be true. Could of been false on the way in
    useFontChanges = true;

    QString fontFamily( ui->fontComboBox->itemText( ui->fontComboBox->currentIndex() ) );
    qreal fontPointSize( p.toFloat() * 2.0 );

    if ( fontPointSize <= 0 )
        return;

    QTextCharFormat fmt;
    fmt.setFontFamily( fontFamily );
    fmt.setFontPointSize( fontPointSize );

    mergeFormatOnWordOrSelection( fmt );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::mergeFormatOnWordOrSelection( const QTextCharFormat& format )
{
    bool blockSignals( "" == ui->textEditRegion->toPlainText() );

    QTextCursor cursor( textEdit->textCursor() );
    cursor.mergeCharFormat( format );

    if ( blockSignals )
        textEdit->blockSignals( true );

    textEdit->setTextCursor( cursor );
    textEdit->mergeCurrentCharFormat( format );

    if ( blockSignals )
        textEdit->blockSignals( false );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::currentCharFormatChanged(const QTextCharFormat &format)
{
    if(ui->textEditRegion->toPlainText() != "")
    {
        fontChanged(format.font());
        font4Backspacing = format.font();
    }
    else
    {
        if(decodeBufferIncomingFontName != "" && decodeBufferIncomingFontSize != "")
        {
            // change the font4backspacing
            font4Backspacing.setFamily(decodeBufferIncomingFontName);
            font4Backspacing.setPointSize(decodeBufferIncomingFontSize.toInt()* 2);
        }
        ui->textEditRegion->setCurrentFont(font4Backspacing);
        //ui->textEditRegion->currentCharFormat().setFont(font4Backspacing);
    }
}

void superChangeProductAllFields::fontChanged(const QFont &f)
{
    ui->fontComboBox->setCurrentIndex(indexOfFontName(f.family()) );

    if (( f.pointSize() / 2 ) < 5 )
        ui->fontSizeComboBox->setCurrentIndex( 0 );
    else
        ui->fontSizeComboBox->setCurrentIndex(
            ui->fontSizeComboBox->findText( QString::number( f.pointSize() / 2 ) ) );
}

void superChangeProductAllFields::onTextChanged()
{
    // only allow TEXT_EDITOR_MAX_CHARS in text editor excluding carriage return
    int numCarriageReturns = textEdit->document()->toPlainText().count('\n');

    if ((textEdit->document()->toPlainText().length() - numCarriageReturns) > TEXT_EDITOR_MAX_CHARS)
    {
        textEdit->document()->setHtml(lastTextEditHtml);
        //qDebug() << "htmlHeight is: " << textEdit->size().height();
    }
    else
    {
        lastTextEditHtml = textEdit->document()->toHtml();
        //qDebug() << "htmlHeight1 is: " << textEdit->size().height();

    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeUnitPrice()
{
    // if tracking supervisor changes send a single field write to app for Unit Price
    // Send the UnitPrice as the Unit Price. Forced UnitPrice field not send

    if ( ifUpdating )
        return;

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();
    QString upFieldVal    = ui->le2UnitPrice->text().replace(separator,".");
    QString uptester      = currentPLUData->unitPrice();

    if( upFieldVal == currentPLUData->unitPrice() || upFieldVal == prev_unitprice )
        return;

    wroteTransaction = true;
    prev_unitprice = upFieldVal;


    //QString msg = productNumber + "," + "12" + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeLbsForPrice()
{
    // if tracking supervisor changes send a single field write to app for  lbsForPrice
    // Send the PoundsForPrice as the Unit Price. he price modifiers and forced field have been left out. Not requested

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal = ui->le2PoundsForPrice->text().replace(separator,".");

    if( scaleConfigData->poundsForPricingOn() == false )
        return;

    if( upFieldVal == prev_lbsforprice || upFieldVal == currentPLUData->unitPrice() )
        return;

    wroteTransaction = true;
    prev_lbsforprice = upFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    //QString msg = productNumber + "," + "12" + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeFractionalPrice()
{
    // if tracking supervisor changes send a single field write to app for Fractional Price
    // Send the FractionalPrice as the Unit Price. The price modifiers and forced field have been left out. Not requested

    if( scaleConfigData->fractionalPricingOn() == false )
        return;

    if ( ifUpdating )
        return;

    QString separator     = currencyData->unitSeparator();
    QString upFieldVal = ui->le2FractionalPrice->text().replace(separator,".");

    if( upFieldVal == prev_fracprice || upFieldVal == currentPLUData->unitPrice() )
        return;

    wroteTransaction = true;
    prev_fracprice = upFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    //QString msg = productNumber + "," + "12" + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetUnitPrice ) + ",\"" + upFieldVal + "\"," + currentPLUData->unitPrice() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->unitPrice() = upFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeTare()
{
    // if tracking supervisor changes send a single field write to app for Tare or 2nd Tare
    // HT-136 Save the text field into Tare "A" only if not using 2nd Tare.  Otherwise use the original value.

    QString productNumber = currentPLUData->itemNumber();
    QString separator     = currencyData->unitSeparator();

    if ( ifUpdating )
        return;

    if( !scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->le2Tare->text().replace(separator,".");  //  tare() = 21

        if( tareFieldVal == currentPLUData->tare() || tareFieldVal == prev_tareA )
            return;

        wroteTransaction = true;
        prev_tareA = tareFieldVal;

        // QString msg = productNumber + "," + "20" + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";
        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareAValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tare() + ",0";

        session_->send(ProductDev, ProductWriteSingleField, msg);               // write it 101,16

        currentPLUData->tare() = tareFieldVal;
    }
    else if( scaleConfigData->useSecondTare() )
    {
        QString tareFieldVal = ui->le2Tare->text().replace(separator,".");      //  2ndtare() = 104

        if( tareFieldVal == prev_tareB || tareFieldVal == currentPLUData->tareB() )
            return;

        wroteTransaction = true;
        prev_tareB = tareFieldVal;

        // BIG NOTE - I THINK THIS WAS LOST. SHOULD BE 104 now with the addition of an INTERNAL FLAG in ScaleApp
        //QString msg = productNumber + "," + "103" + ",\"" + tareFieldVal + "\"," + currentPLUData->tareB() + ",0";
        QString msg = productNumber + "," + QString::number( Product::fieldOffsetTareBValue ) + ",\"" + tareFieldVal + "\"," + currentPLUData->tareB() + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->tareB() = tareFieldVal;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangePropTare()
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

    //QString msg = productNumber + "," + "21" + ",\"" + tareFieldVal + "\"," + currentPLUData->proportionalTare() + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetProportionalTare ) + ",\"" + tareFieldVal + "\"," + currentPLUData->proportionalTare() + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->tare() = tareFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeByCount()
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

        //QString msg = productNumber + "," + "10" + ",\"" + bcFieldVal + "\"," + "\"" + bcPluVal + "\"" + ",0";
        QString msg = productNumber + "," + QString::number( Product::fieldOffsetByCount ) + ",\"" + bcFieldVal + "\"," + "\"" + bcPluVal + "\"" + ",0";
        session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

        currentPLUData->byCountRaw() = bcPluVal;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeNetWeight()
{
    if ( ifUpdating )
        return;

    // if tracking supervisor changes send a single field write to app for Net Weight
    QString separator     = currencyData->unitSeparator();
    QString fwFieldVal = ui->le2NetWeight->text().replace(separator,".");                        //  byCount() = 23
    QString fwPluVal   = currentPLUData->fixedWeight();

    if( fwFieldVal == currentPLUData->fixedWeight() )
        return;

    wroteTransaction = true;
    prev_netwt = fwFieldVal;

    QString productNumber = currentPLUData->itemNumber();
    //QString msg = productNumber + "," + "22" + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->fixedWeight() = fwFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::trackSuperChangeFluidNetWeight()
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
    //QString msg = productNumber + "," + "22" + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    QString msg = productNumber + "," + QString::number( Product::fieldOffsetFixedWeight ) + ",\"" + fwFieldVal + "\"," + fwPluVal + ",0";
    session_->send(ProductDev, ProductWriteSingleField, msg);       // write it 101,16

    currentPLUData->fixedWeight() = fwFieldVal;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onPrefixTextChanged()
{
    QString newPrefix = ui->txtPrefix->text();
    int newIntPrefix = ui->txtPrefix->text().toInt();
    int curIntPrefix = currentPLUData->numberSystem().toInt();
    int pluType = currentPLUData->productType();
    int upcNum = ui->txtUPCNumber->text().toInt();
    int eanNum = ui->txtEANCode->text().toInt();

    if( newIntPrefix == curIntPrefix )
        return;                                                                         // nothing changed so exit

    if ( ( ProductType::PRODUCTTYPE_RANDOMWEIGHT == pluType ) || ( ProductType::PRODUCTTYPE_BYCOUNT == pluType ) )
    {
        // if we have a RW or BC, and we are doing UPC fields, WE MUST have a prefix of 2 or 4.
        // if we have a RW or BC, and doing EAN, GS1, etc, the Prefix can be anything
        if( 2 == newIntPrefix || 4 == newIntPrefix )
        {
            if( ( 0 == upcNum ) && ( 0 < eanNum ) )
            {
                QString errStr = QString::number(ProductDevError::ProductErrInvalidBarcodePrefix) + ",5";
                session_->send( Error, ErrProductDevice, errStr );                          // invalid prefix for plu type
                return;
            }
        }
        else if( ( newIntPrefix != 2 ) &&  ( newIntPrefix != 4 ) )
        {
            QString errStr = QString::number(ProductDevError::ProductErrInvalidBarcodeUPCPrefix) + ",5";
            session_->send( Error, ErrProductDevice, errStr );                          // invalid prefix for plu type
            return;
        }
    }
    else if( ( ProductType::PRODUCTTYPE_FIXEDWEIGHT == pluType ) || ( ProductType::PRODUCTTYPE_FLUIDOUNCE == pluType ) )
    {
        if( 3 == newIntPrefix || 5 == newIntPrefix )
        {
            QString errStr = QString::number(ProductDevError::ProductErrInvalidBarcodeUPCPrefixOnFW) + ",5";
            session_->send( Error, ErrProductDevice, errStr );                          // invalid prefix for plu type
            return;
        }
    }

    currentPLUData->numberSystem()  = newPrefix;
    currentPLUData->vendorNumber()  = ui->txtUPCVendorNumber->text();               // save off current values if any there
    currentPLUData->productNumber() = ui->txtUPCNumber->text();                     // always save UPC since it's the same as Product
    currentPLUData->eanCode()       = ui->txtEANCode->text();

     if(scaleConfigData->scaleModel().toInt() == 1)
     {
         showForcedFields = false;                                                    // no self servs
     }
     else
     {
         if( 2 == newIntPrefix || 4 == newIntPrefix )
             showForcedFields = true;

         if( eanNum > 0 )
             showForcedFields = true;
     }

     if( showForcedFields )
         loadPricingTab( pluType );
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onUpcNumTextChanged()
{
    int curPrefix = ui->txtPrefix->text().toInt();
    int pluType = currentPLUData->productType();
    int upcNum = ui->txtUPCNumber->text().toInt();
    int eanNum = ui->txtEANCode->text().toInt();

    ui->txtUPCProductNumber->setText( ui->txtUPCNumber->text() );
    currentPLUData->productNumber() = ui->txtUPCNumber->text();

    if ( ( ProductType::PRODUCTTYPE_RANDOMWEIGHT == pluType ) || ( ProductType::PRODUCTTYPE_BYCOUNT == pluType ) )
    {
        // if we have a RW or BC, and we are doing UPC fields, WE MUST have a prefix of 2 or 4.
        // if we have a RW or BC, and doing EAN, GS1, etc, the Prefix can be anything
        if( ( 2 != curPrefix ) && ( 4 != curPrefix ) )
        {
            if( ( 0 == upcNum ) && ( eanNum > 0 ) )
            {
                QString errStr = QString::number(ProductDevError::ProductErrInvalidBarcodeUPCPrefix) + ",5";
                session_->send( Error, ErrProductDevice, errStr );                          // invalid prefix for plu type
                return;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onProdNumTextChanged()
{
    int curPrefix = ui->txtPrefix->text().toInt();
    int pluType = currentPLUData->productType();
    int upcNum = ui->txtUPCNumber->text().toInt();
    int eanNum = ui->txtEANCode->text().toInt();

    ui->txtUPCNumber->setText( ui->txtUPCProductNumber->text() );
    currentPLUData->productNumber() = ui->txtUPCNumber->text();

    if ( ( ProductType::PRODUCTTYPE_RANDOMWEIGHT == pluType ) || ( ProductType::PRODUCTTYPE_BYCOUNT == pluType ) )
    {
        // if we have a RW or BC, and we are doing UPC fields, WE MUST have a prefix of 2 or 4.
        // if we have a RW or BC, and doing EAN, GS1, etc, the Prefix can be anything
        if( ( 2 != curPrefix ) && ( 4 != curPrefix ) )
        {
            if( ( 0 == upcNum ) && ( eanNum > 0 ) )
            {
                QString errStr = QString::number(ProductDevError::ProductErrInvalidBarcodeUPCPrefix) + ",5";
                session_->send( Error, ErrProductDevice, errStr );                          // invalid prefix for plu type
                return;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onEANTextChanged()
{
    // RULE: Can only have forced fields when the Prefix is either 2 or 4
    // OR - we have value in the EAN Code
    // AND we cant be a self service scale

    int prefix = ui->txtPrefix->text().toInt();

    if(scaleConfigData->scaleModel().toInt() == 1)
    {
        showForcedFields = false;                                                    // no self servs
    }
    else
    {
        if( ui->txtEANCode->text().toInt() > 0 )
            showForcedFields = true;

        if( 2 == prefix || 4 == prefix )
            showForcedFields = true;
    }

    if( showForcedFields )
        loadPricingTab( currentPLUData->productType() );
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onUnitPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_UNITPRICE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onPriceEditedFinished()
{
    const QString minPrStr  = ui->txtMinPrice->text();
    const QString maxPrStr  = ui->txtMaxPrice->text();
    const QString unitPrStr = currentPLUData->unitPrice();

    float minPr  = minPrStr.toFloat();
    float maxPr  = maxPrStr.toFloat();
    float unitPr = unitPrStr.toFloat();

    if( minPr == 0 || maxPr == 0 )
        return;                     // if we are still 0, no limits, dont complain dont compare

    if( maxPr <= minPr )
    {
        QString tmpStr = QString::number(ProductDevError::ProductErrMaxPriceBelowMinPrice) + ",5";
        session_->send( Error, ErrSoftkeyDevice, tmpStr );      // The Maximum Price is less than the Minimum Price.
    }

    if( ProductType::PRODUCTTYPE_RANDOMWEIGHT == currentPLUData->productType() )
        return;

    if( unitPr == 0 )
        return;                     // if we are still 0, no limits, dont complain dont compare

    if( unitPr < minPr )
    {
        QString tmpStr = QString::number(ProductDevError::ProductErrMinBelowUnitPrice) + ",5";
        session_->send( Error, ErrSoftkeyDevice, tmpStr );      // The Unit Price is less than the Minimum Price
    }
    else if( unitPr > maxPr )
    {
        QString tmpStr = QString::number(ProductDevError::ProductErrMaxBelowUnitPrice) + ",5";
        session_->send( Error, ErrSoftkeyDevice, tmpStr );      // The Unit Price is more than the Maximum Price
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onWeightEditedFinished()
{
    const QString minWtStr  = ui->txtMinWeight->text();
    const QString maxWtStr  = ui->txtMaxWeight->text();

    float minWt = minWtStr.toFloat();
    float maxWt = maxWtStr.toFloat();

    if( minWt == 0 || maxWt == 0 )
        return;                     // if one of them are still 0, cant compare anything

    if( maxWt <= minWt )
        session_->send( Error, ErrSoftkeyDevice, QString::number(ProductDevError::ProductErrMaxWeightBelowMinWeight) + ",5" );      // "The Maximum Weight is less than the Minimum Weight."
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onLbsForTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_LBSFOR;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onFracPriceTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_FRACPRICE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_TARE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onPropTareTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_PROPTARE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onByCountTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_BYCOUNT;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onNetWtTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_NETWT;
}

//////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::onFluidNetWtTextChanged(QString focusTxt)
{
    Q_UNUSED( focusTxt );
    whoHasFocus = SAV_FLNETWT;
}

// NOTE to self: Need to handle this when the forced value is read in
// as well for all 3 of these fields. Check to see if its being done.
void superChangeProductAllFields::onForcedTareChanged(int state)
{
    UiUtilities *util = session_->uiUtilities;

    if(state == 2)
    {
        // we checked the checkbox so clear the tare field. Also save it off to the DOM
        // HT-136:  Save off the correct Tare value
        QString taretester = ui->le2Tare->text();
        if( scaleConfigData->useSecondTare() )
            currentPLUData->tareB() = ui->le2Tare->text();
        else
            currentPLUData->tare() = ui->le2Tare->text();
        QString taretester2 = currentPLUData->tare();

        ui->le2Tare->setText("");
        ui->le2Tare->setEnabled(false);

        trackSuperChangeTare();
    }
    else
    {
        // elsewise bring back its default value, not necessarily it's previous value
        // HT-136:  Use the correct Tare value
        QString forcedVal = ( scaleConfigData->useSecondTare() ? currentPLUData->tareB() : currentPLUData->tare() );

        /*if( 2 == scaleConfigData->tarePrecision() )
            forcedVal = "0.00";
        else
            forcedVal = "0.000";
        */
        util->youGottaSetTheSeparator( forcedVal );
        ui->le2Tare->setEnabled(true);
        ui->le2Tare->setText(forcedVal);

        trackSuperChangeTare();
    }
}

void superChangeProductAllFields::onForcedUnitPriceChanged(int state)
{
    UiUtilities *util = session_->uiUtilities;

    // FT-528
    if( !updatingFromIncomingPLUData )
    {
        if(state == 2)
        {
            // we checked the checkbox so clear the tare field. Also save it off to the DOM
            QString uptester = ui->le2UnitPrice->text();
            //currentPLUData->unitPrice() = "";                         // ORIG
            currentPLUData->forcedUnitPriceRaw() = "1";

            if(widgetUsedForPrice == "gb2UnitPrice")
            {
                currentPLUData->unitPrice() = ui->le2UnitPrice->text(); // NEW
                QString uptester2 = ui->le2UnitPrice->text();
                ui->le2UnitPrice->setText("");
            }
            else if(widgetUsedForPrice == "gb2FractionalPrice")
            {
                currentPLUData->unitPrice() = "";
                currentPLUData->fractionalPrice() = "";
                currentPLUData->forcedFractionalPriceRaw() = "1";
                ui->le2FractionalPrice->setText("");
            }
            else if(widgetUsedForPrice == "gb2PoundsForPricing")
            {
                currentPLUData->unitPrice() = "";
                currentPLUData->poundsForPrice() = "";
                currentPLUData->forcedPoundsForRaw() = "1";
                ui->le2PoundsForPrice->setText("");
            }
            trackSuperChangeUnitPrice();
        }
        else
        {
            // elsewise bring back its default value, not necessarily it's previous value
            QString forcedVal = "0.00";
            util->youGottaSetTheSeparator( forcedVal );
            currentPLUData->unitPrice() = forcedVal;
            currentPLUData->forcedUnitPriceRaw() = "0";

            if(widgetUsedForPrice == "gb2UnitPrice")
            {
                ui->le2UnitPrice->setText( forcedVal );
            }
            else if(widgetUsedForPrice == "gb2FractionalPrice")
            {
                currentPLUData->fractionalPrice() = forcedVal;
                ui->le2FractionalPrice->setText( forcedVal );
                currentPLUData->forcedFractionalPriceRaw() = "0";
            }
            else if(widgetUsedForPrice == "gb2PoundsForPricing")
            {
                currentPLUData->poundsForPrice() = forcedVal;
                ui->le2PoundsForPrice->setText( forcedVal );
                currentPLUData->forcedPoundsForRaw() = "0";
            }
            trackSuperChangeUnitPrice();
        }
    }
}

void superChangeProductAllFields::onForcedByCountChanged(int state)
{
    savefbcTransaction = false;

    if(state == 2)
    {
        //We checked the force by count checkbox so save the current state of the by count
        currentPLUData->byCountRaw() = ui->le2ByCount->text();
        ui->le2ByCount->setText("");
        ui->le2ByCount->setEnabled(false);
        trackSuperChangeByCount();
    }
    else
    {
        //Else reset the by count to the currentPLUData saved for this plu and re-enable the field

        // SOF-3510 if we had a forced bycount, and then it goes unforced, and the bycount value is 1
        savefbcTransaction = true;

        ui->le2ByCount->setText(currentPLUData->byCountRaw());
        ui->le2ByCount->setEnabled(true);
        trackSuperChangeByCount();
    }
    // by count set to blank the bycount field upon forced field click
}

bool superChangeProductAllFields::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    // The create PLU dialog is handled by the screen, we do not create a generic dialog.
    if ( 4 == errorMsg[3].toInt() )
        return true;

    return _Mybase::isErrorMessageHandler( errorMsg );
}

void superChangeProductAllFields::initValidators()
{

    txtEANCodeValidator_ = NULL;

    productNumberValidator_ =

    //Tab1
    le1ReportingClassValidator_ =
    txtPrefixValidator_ =
    txtUPCVendorNumberValidator_ =
    txtUPCProductNumberValidator_ =
    txtUPCNumberValidator_ =

    //Tab2
    le2Discount1Validator_ =
    le2Discount2Validator_ =
    le2Discount3Validator_ =
    le2UnitPriceValidator_ =
    minPriceValidator_     =
    maxPriceValidator_     =
    le2PointsValidator_ =
    le2ByCountValidator_ =
    le2TareValidator_ =
    le2ProportionalTareValidator_ =
    le2NetWeightValidator_ =
    minWeightValidator_    =
    maxWeightValidator_    =
    le2NetWeightFluidValidator_ =
    le2ExceptionPriceValidator_ =
    le2RegularPriceValidator_ =
    le2FractionalPriceValidator_ =
    le2PoundsForPriceValidator_ =
    le2TarePlatterValidator_ =

    //Tab3
    le3ShelfLifeDaysValidator_ =
    le3ShelfLifeHoursValidator_ =
    le3ShelfLifeMinutesValidator_ =
    le3ProductLifeDaysValidator_ =
    le3ProductLifeHoursValidator_ =
    le3ProductLifeMinutesValidator_ =

    le3PortionsValidator_ =
    le3ProductionPlanningTypeValidator_ =
    le3BuildAMealValidator_ =
    le3NumberSidesValidator_ =
    le3MeatSpecSheetValidator_ =

    //Tab4
    priDefaultLabelTypeValidator_ =
    secDefaultLabelTypeValidator_ =
    triDefaultLabelTypeValidator_ =
    priManualLabelTypeValidator_ =
    secManualLabelTypeValidator_ =
    triManualLabelTypeValidator_ =
    priPrepackLabelTypeValidator_ =
    secPrepackLabelTypeValidator_ =
    triPrepackLabelTypeValidator_ =
    priProdEntryLabelTypeValidator_ =
    secProdEntryLabelTypeValidator_ =
    triProdEntryLabelTypeValidator_ =
    priSelfServLabelTypeValidator_ =
    secSelfServLabelTypeValidator_ =
    triSelfServLabelTypeValidator_ =
    priPick5LabelTypeValidator_ =
    secPick5LabelTypeValidator_ =
    triPick5LabelTypeValidator_ =
    priOnlineOrderingLabelTypeValidator_ =
    secOnlineOrderingLabelTypeValidator_ =
    triOnlineOrderingLabelTypeValidator_ =

    NULL;
}

void superChangeProductAllFields::setupWrapperAndApplierFields()
{
    //    Applier rotations
    this->ui->cboPrimaryApplierRotation->clear();
    ui->cboPrimaryApplierRotation->addItem(tr("0"), "0");
    if ( !isTwoPrinterSystem_ )
        ui->cboPrimaryApplierRotation->addItem(tr("45"), "45");
    ui->cboPrimaryApplierRotation->addItem(tr("90"), "90");
    if ( !isTwoPrinterSystem_ )
        ui->cboPrimaryApplierRotation->addItem(tr("135"), "135");
    ui->cboPrimaryApplierRotation->addItem(tr("180"), "180");
    if ( !isTwoPrinterSystem_ )
        ui->cboPrimaryApplierRotation->addItem(tr("225"), "225");
    ui->cboPrimaryApplierRotation->addItem(tr("270"), "270");
    if ( !isTwoPrinterSystem_ )
        ui->cboPrimaryApplierRotation->addItem(tr("315"), "315");

    ui->cboSecondApplierRotation->clear();
    ui->cboSecondApplierRotation->addItem(tr("0"), "0");
    if ( !isTwoPrinterSystem_ )
        ui->cboSecondApplierRotation->addItem(tr("45"), "45");
    ui->cboSecondApplierRotation->addItem(tr("90"), "90");
    if ( !isTwoPrinterSystem_ )
        ui->cboSecondApplierRotation->addItem(tr("135"), "135");
    ui->cboSecondApplierRotation->addItem(tr("180"), "180");
    if ( !isTwoPrinterSystem_ )
        ui->cboSecondApplierRotation->addItem(tr("225"), "225");
    ui->cboSecondApplierRotation->addItem(tr("270"), "270");
    if ( !isTwoPrinterSystem_ )
        ui->cboSecondApplierRotation->addItem(tr("315"), "315");

    ui->cboThirdApplierRotation->clear();
    ui->cboThirdApplierRotation->addItem(tr("0"), "0");
    if ( !isTwoPrinterSystem_ )
        ui->cboThirdApplierRotation->addItem(tr("45"), "45");
    ui->cboThirdApplierRotation->addItem(tr("90"), "90");
    if ( !isTwoPrinterSystem_ )
        ui->cboThirdApplierRotation->addItem(tr("135"), "135");
    ui->cboThirdApplierRotation->addItem(tr("180"), "180");
    if ( !isTwoPrinterSystem_ )
        ui->cboThirdApplierRotation->addItem(tr("225"), "225");
    ui->cboThirdApplierRotation->addItem(tr("270"), "270");
    if ( !isTwoPrinterSystem_ )
        ui->cboThirdApplierRotation->addItem(tr("315"), "315");

    //    Label placement
    ui->cboPrimaryLabelPlacement->clear();
    ui->cboPrimaryLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
    ui->cboPrimaryLabelPlacement->addItem(tr("Upper Right"), "0");
    ui->cboPrimaryLabelPlacement->addItem(tr("Upper Left"), "1");

    populateSecondaryPlacementComboBox();

    ui->cboThirdLabelPlacement->clear();
    ui->cboThirdLabelPlacement->addItem(tr("System default"), "2");    //    Make first in list in case of no applier.
    ui->cboThirdLabelPlacement->addItem(tr("Upper Right"), "0");
    ui->cboThirdLabelPlacement->addItem(tr("Upper Left"), "1");

    //    Intake mode
    ui->cboIntakeMode->clear();
    ui->cboIntakeMode->addItem(tr("System default"), "0");
    ui->cboIntakeMode->addItem(tr("Gentle"), "1");
    ui->cboIntakeMode->addItem(tr("Normal"), "2");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::doxygenImpactAndRegressionTestingComments()
{
    /**
     * The following method will contain only comments to discuss any and all newly added code
     * and its impact to other code modules to identify regression test requirements
     */

    ///< Meat Specification Sheet feature impact on existing code.
    ///< Impact: Anywhere the currentPLU is read, saved.  This is due to the EANPrefix is ALWAYS the last field
    ///< in the configuration of the currentPLU or defaultProduct. If the meatspecsheet is not included properly or
    ///< misnumbered then the EAN Code will not be read, saved, cleared properly
    ///<    1.Default Products because currentPLU and Default Products have to match configuration wise
    ///<      even if the field is not displayed in Default Products
    ///<    2.Where the plu is cleared. This is when we leave operate or supervisor modes and after a print.
    ///<    3. Saving, reading, displaying the EAN Prefix in All Fields or Default Product
    ///
}

// secondary can be in a conflicting position with the primary position (IE if primary is upper left, then the secondary cant be in
// above or left of primary position)
// Here we are preventing the user from putting the secondary position as one of those conflicting positions
void superChangeProductAllFields::populateSecondaryPlacementComboBox()
{
    int primaryIndex = currentPLUData->primaryLabelPlacement();


    // need to check if secondary is not in a conflicting position, if its not then we dont need to set the secondary to system default

    switch( primaryIndex )
    {
        case upperRight:
        {
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void superChangeProductAllFields::scaleWidgets(QWidget *widgetToScale){
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
