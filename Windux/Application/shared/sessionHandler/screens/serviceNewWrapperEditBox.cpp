/* @file serviceNewWrapperEditBox.cpp
 * Allows Service technicians to modify wrap box timings.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 */
#if 1
#define USE_QDEBUG
#endif

#if 0
#define USE_MIN_MAX
#endif

#if 0
#define TEST_WRAPPER_OPERATION
#endif

#include <logmanager.h>

#include "serviceNewWrapperEditBox.h"
#include "ui_serviceNewWrapperEditBox.h"
#include "serviceNewWrapperSetTimings.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/servicemode.h"

#include "../dialogs/messageboxdialog.h"
#include "../dialogs/numericinputboxdialog.h"
#include "../dialogs/wrapperforcetraysizedialog.h"

#include "serviceNewWrapperDiagActuators.h"
#include "operateNewWrapperRunIn.h"



#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QCoreApplication>
#include <QtGui>
#include <QtCore>
#include <QFile>
#include <QFileInfo>
#include <QIntValidator>
#include <QSignalMapper>
#include <palette.h>
#include <backgroundwidgetstyles.h>
#include <numericlineedit.h>

#include "../sessionmainwindow.h"
#include "../dialogs/filedialog.h"
#include "../dialogs/messageboxdialog.h"
#include "python.h"
#include "filesystem.h"
#include "python.h"


#define xstr(s) str(s)
#define str(s) #s

const bool TEST_WRAPPER = false;
const bool OG = false;

static const int DONE_STATE = 0;
static const int CANCEL_STATE = 1;

namespace hobart{
    class numericLineEdit;
}

//		pageMode = 1	//	View/Edit a tray
//		pageMode = 2 	//	Auto-configure a new tray
//		pageMode = 3	//	Auto detect a tray
//		pageMode = 4	//	New tray with a PLUExcption (GONE)

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

serviceNewWrapperEditBox::serviceNewWrapperEditBox(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperEditBox),
    wrTimingsData_(NULL),
    cycleWrapperTestLabel_( NULL ),
    leadingEdge(false),
    savedExport_(false)
{
    LOG_CLASS_INFO( "serviceEditBox", "serviceEditBox" );

    setObjectName( "serviceEditBox" );

    LOG_DEBUG( "serviceEditBox CTOR", NULL );

//	Retrieve config object
    if ( mainWindow_->isServiceMode() )
        wrTimingsData_ = &dynamic_cast<ServiceMode*>( mode_ )->wrTimingsData_;
    else
        wrTimingsData_ = new WrapperTimingsData;

    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure )
        ui->txtTrayName->setFocus();
}

serviceNewWrapperEditBox::~serviceNewWrapperEditBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "DTOR" );

    LOG_DEBUG( "serviceEditBox() START", NULL );

    if ( !mainWindow_->isServiceMode() )
        delete wrTimingsData_;

    delete ui;

    pythonProcess->cleanup();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::initialize()
{
    LOG_CLASS_INFO( "serviceEditBox", "initialize" );

    LOG_DEBUG( "initialize() START", NULL );

    firstPass = true;
    gotNewTrayName_ = false;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    boxCount = 0;
    wrTimingsData_->exitByMenu = true;      //  We'll set this to false when we change to another Timings page.

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;
    heightBoxSelected_ = wrTimingsData_->heightBoxSelected;

    heightBox_ = NULL;

    cycleWrapperInProgress_ = false;

    // SOF-2550
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
        ui->butCycleWrapperLeadingTab1->hide();
        ui->butCycleWrapperTrailingTab1->hide();
        ui->butCycleWrapperLeadingTab2->hide();
        ui->butCycleWrapperTrailingTab2->hide();
        ui->butCycleWrapperLeadingTab3->hide();
        ui->butCycleWrapperTrailingTab3->hide();
        ui->butCycleWrapperLeadingTab4->hide();
        ui->butCycleWrapperTrailingTab4->hide();
        ui->butCycleWrapperLeadingTab5->hide();
        ui->butCycleWrapperTrailingTab5->hide();
    }
    else
    {
        // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
        ui->butCycleWrapperLeadingTab1->show();
        ui->butCycleWrapperTrailingTab1->show();
        ui->butCycleWrapperLeadingTab2->show();
        ui->butCycleWrapperTrailingTab2->show();
        ui->butCycleWrapperLeadingTab3->show();
        ui->butCycleWrapperTrailingTab3->show();
        ui->butCycleWrapperLeadingTab4->show();
        ui->butCycleWrapperTrailingTab4->show();
        ui->butCycleWrapperLeadingTab5->show();
        ui->butCycleWrapperTrailingTab5->show();
    }

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure )
    {
        LOG_DEBUG( "initialize() pageMode = pmAutoConfigure so Hide Advanced and Other Timings", NULL );

        if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
        {
            //  Hide these items since they can't be used in this mode.
            ui->butSetMin->hide();
            ui->butSetMinAndMax->hide();
            ui->butSetMax->hide();
            ui->butNewHeightBox->hide();
            ui->butDeleteHeightBox->hide();

            ui->txtMinHeight->hide();
            ui->txtMaxHeight->hide();

            ui->lblHeightBox->hide();
            ui->cboHeightBox->hide();

            ui->lblTrayFound->hide();

            //  Disable these fields until a tray name is entered and they cycle a package.
            ui->txtMinLength->setEnabled(false);
            ui->txtMaxLength->setEnabled(false);
            ui->txtMinWidth->setEnabled(false);
            ui->txtMaxWidth->setEnabled(false);
            ui->txtMaxLip->setEnabled(false);

            hideShowForNewTray(true);       //  Hide all the other items until the tray name is entered.
        }

        ui->butCycleWrapperTab1->hide();
        ui->butCycleWrapperTab2->hide();
        ui->butCycleWrapperTab3->hide();
        ui->butCycleWrapperTab4->hide();
        ui->butCycleWrapperTab5->hide();

        ui->butSave->hide();   
    }

    if ( wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        ui->butAutoConfig->hide();
        ui->lblPromptTrayName->hide();

        if (!viewCurrentTimings_)
        {
            ui->butSetMin->hide();
            ui->butSetMinAndMax->hide();
            ui->butSetMax->hide();
            ui->butNewHeightBox->hide();
            ui->butDeleteHeightBox->hide();
            ui->butSave->hide();
        }
    }

    connect(ui->butSetMin,    				   SIGNAL(clicked()),              	   this,       SLOT(setMinValues()));
    connect(ui->butSetMinAndMax,    		   SIGNAL(clicked()),              	   this,       SLOT(setMinMaxValues()));
    connect(ui->butSetMax,    				   SIGNAL(clicked()),      			   this,       SLOT(setMaxValues()));
    connect(ui->butNewHeightBox,               SIGNAL(clicked()),                  this,       SLOT(newHeightBox()));
    connect(ui->butCancelNewHeightBox,         SIGNAL(clicked()),                  this,       SLOT(cancelNewHeightBox()));
    connect(ui->butDeleteHeightBox,			   SIGNAL(clicked()),              	   this,       SLOT(deleteSelectedHeight()));
    connect(ui->cboAvailableHeightBoxes,       SIGNAL(currentIndexChanged(int)),   this,       SLOT(addNewHeightBox()));
    connect(ui->cboHeightBox,                  SIGNAL(currentIndexChanged(int)),   this,       SLOT(requestHeightBox()));

    connect(ui->butCycleWrapperTab1,           SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
    connect(ui->butCycleWrapperLeadingTab1,    SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
    connect(ui->butCycleWrapperTrailingTab1,   SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );
    connect(ui->butCycleWrapperTab2,           SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
    connect(ui->butCycleWrapperLeadingTab2,    SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
    connect(ui->butCycleWrapperTrailingTab2,   SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );
    connect(ui->butCycleWrapperTab3,           SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
    connect(ui->butCycleWrapperLeadingTab3,    SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
    connect(ui->butCycleWrapperTrailingTab3,   SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );
    connect(ui->butCycleWrapperTab4,           SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
    connect(ui->butCycleWrapperLeadingTab4,    SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
    connect(ui->butCycleWrapperTrailingTab4,   SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );
    connect(ui->butCycleWrapperTab5,           SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
    connect(ui->butCycleWrapperLeadingTab5,    SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
    connect(ui->butCycleWrapperTrailingTab5,   SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );

    connect(ui->butExportTimingsTab2,           SIGNAL(clicked()),                  this,       SLOT(onExportTimings() ) );
    connect(ui->butExportTimingsTab3,           SIGNAL(clicked()),                  this,       SLOT(onExportTimings()));
    connect(ui->butExportTimingsTab4,           SIGNAL(clicked()),                  this,       SLOT(onExportTimings() ) );
    connect(ui->butExportTimingsTab5,           SIGNAL(clicked()),                  this,       SLOT(onExportTimings() ) );

    connect(ui->butPromptTraySize,      SIGNAL(clicked()),      			this,       SLOT(promptForcedTraySize()));
    connect(ui->txtMinLength,           SIGNAL(editingFinished()),          this,       SLOT(updateMinLength()));
    connect(ui->txtMaxLength,           SIGNAL(editingFinished()),          this,       SLOT(updateMaxLength()));
    connect(ui->txtMinWidth,            SIGNAL(editingFinished()),          this,       SLOT(updateMinWidth()));
    connect(ui->txtMaxWidth,            SIGNAL(editingFinished()),          this,       SLOT(updateMaxWidth()));
    connect(ui->txtMaxLip,              SIGNAL(editingFinished()),          this,       SLOT(updateMaxLip()));

    if (( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect) || ( wrTimingsData_->canEditTrayName == false ))
        ui->txtTrayName->setEnabled(false);
    else
        ui->txtTrayName->setEnabled(true);

    ui->tabServiceTimings->setCurrentIndex(0);
    ui->butPrintTab->setVisible( true );

    connect(ui->butCancelDone,          SIGNAL(clicked()),                  this,       SLOT(onCancelChanges()));
    connect(ui->butSave,                SIGNAL(clicked()),      			this,       SLOT(onSaveChanges()));

    // INITIALIZE COMBO
    ui->param14->addItem( tr("Narrow roll"), QString( "100" ) );    // ***
    ui->param14->addItem( tr("Wide roll"),   QString( "200" ) );    // ***
    ui->param14->setCurrentIndex( 0 );                              // ***

    if ( !viewCurrentTimings_ )
        ui->param14->setEnabled( false );

    // Connect the main Timings inputs
    // Label Tab connections - need to do a single field write on entering a label to determine validity signal mapper for label fields
    editingFinishedSignal_ = new QSignalMapper( this );
    connect( editingFinishedSignal_,    SIGNAL( mapped(QWidget*) ), SLOT( onEditingFinished(QWidget*) ) );

    editingFinishedSignal_->setMapping( ui->param3, ui->param3 );
    connect(ui->param3,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param0, ui->param0 );
    connect(ui->param0,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param2, ui->param2 );
    connect(ui->param2,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param9, ui->param9 );
    connect(ui->param9,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param5, ui->param5 );
    connect(ui->param5,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param15, ui->param15 );
    connect(ui->param15,            SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param10, ui->param10 );
    connect(ui->param10,            SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param16, ui->param16 );
    connect(ui->param16,            SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param7, ui->param7 );
    connect(ui->param7,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param8, ui->param8 );
    connect(ui->param8,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param20, ui->param20 );
    connect(ui->param20,            SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );
    editingFinishedSignal_->setMapping( ui->param4, ui->param4 );
    connect(ui->param4,             SIGNAL(editingFinished()),          editingFinishedSignal_, SLOT( map() ) );

    // combo box index changed
    connect( ui->param14,           SIGNAL( currentIndexChanged(int) ),     SLOT( onComboBoxChanged(int) ) );

    // Connect the up and down buttons
    connect( ui->butLeftParam3,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam3,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam0,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam0,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam2,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam2,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam9,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam9,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam5,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam5,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam15,        SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam15,       SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam10,        SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam10,       SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam16,        SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam16,       SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam7,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam7,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam8,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam8,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam6,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam6,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam20,        SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam20,       SIGNAL(clicked()),      this,       SLOT(onStepUp()) );
    connect( ui->butLeftParam4,         SIGNAL(clicked()),      this,       SLOT(onStepDown()) );
    connect( ui->butRightParam4,        SIGNAL(clicked()),      this,       SLOT(onStepUp()) );

    connect( ui->butParam3,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam0,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam2,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam9,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam5,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam15,            SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam10,            SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam16,            SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam7,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam8,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam6,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam20,            SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );
    connect( ui->butParam4,             SIGNAL(clicked()),      this,       SLOT(onDisplayHelpText()) );

    // connect the tray information
    connect(ui->butPrintTab,            SIGNAL(clicked()),      			this,       SLOT(printTimings()));
    connect(ui->tabServiceTimings,      SIGNAL(currentChanged(int)),        this,       SLOT(currentChangeTab(int)));

    // Validators for timing parameters
    ui->param3->setValidator( baseTen40IntValidator = new QIntValidator(0, 40, this) );      // 10-40
    ui->param3->setDecimalPlaces(0);
    ui->param3->setInputWrapping( true );
    ui->param3->setInputTruncation( true );
    baseTen40IntValidator->setTop(40);
    baseTen40IntValidator->setBottom(0);

    ui->param0->setValidator( baseZero60IntValidator = new QIntValidator(0, 60, this) );      // 0-60
    ui->param0->setDecimalPlaces(0);
    ui->param0->setInputWrapping( true );
    ui->param0->setInputTruncation( true );
    baseZero60IntValidator->setTop(60);
    baseZero60IntValidator->setBottom(0);

    ui->param2->setValidator( baseNegFive10IntValidator = new QIntValidator(-5, 10, this) );      // -5-10
    ui->param2->setDecimalPlaces(0);
    ui->param2->setInputWrapping( true );
    ui->param2->setInputTruncation( true );
    baseNegFive10IntValidator->setTop(10);
    baseNegFive10IntValidator->setBottom(-5);

    ui->param9->setValidator( baseZero10IntValidator = new QIntValidator(0, 10, this) );      // 0-10
    ui->param9->setDecimalPlaces(0);
    ui->param9->setInputWrapping( true );
    ui->param9->setInputTruncation( true );
    baseZero10IntValidator->setTop(10);
    baseZero10IntValidator->setBottom(0);
    //
    ui->param5->setValidator( baseNegFive10IntValidator );      // -5-10
    ui->param5->setDecimalPlaces(0);
    ui->param5->setInputWrapping( true );
    ui->param5->setInputTruncation( true );

    ui->param15->setValidator( baseZero5IntValidator = new QIntValidator(0, 5, this) );      // 0-5
    ui->param15->setDecimalPlaces(0);
    ui->param15->setInputWrapping( true );
    ui->param15->setInputTruncation( true );
    baseZero5IntValidator->setTop(5);
    baseZero5IntValidator->setBottom(0);

    ui->param10->setValidator( baseZero10IntValidator );      // 0-10
    ui->param10->setDecimalPlaces(0);
    ui->param10->setInputWrapping( true );
    ui->param10->setInputTruncation( true );

    ui->param16->setValidator( baseZero10IntValidator );      // 0-10
    ui->param16->setDecimalPlaces(0);
    ui->param16->setInputWrapping( true );
    ui->param16->setInputTruncation( true );
    //
    //ui->param7->setValidator( baseTwelve30IntValidator = new QIntValidator(HB_MIN_12, HB_MAX_30, this) );      // 12-30
    ui->param7->setValidator( baseTwelve30IntValidator = new QIntValidator(0, 30, this) );      // 12-30
    ui->param7->setDecimalPlaces(0);
    ui->param7->setInputWrapping( true );
    ui->param7->setInputTruncation( true );
    baseTwelve30IntValidator->setTop(30);
    baseTwelve30IntValidator->setBottom(0);

    ui->param8->setValidator( baseOne30IntValidator = new QIntValidator(0, 30, this) );      // 1-30
    ui->param8->setDecimalPlaces(0);
    ui->param8->setInputWrapping( true );
    ui->param8->setInputTruncation( true );
    baseOne30IntValidator->setTop(30);
    baseOne30IntValidator->setBottom(0);

    ui->param6->setValidator( baseZero10IntValidator );      // 0-10
    ui->param6->setDecimalPlaces(0);
    ui->param6->setInputWrapping( true );
    ui->param6->setInputTruncation( true );

    ui->param20->setValidator( baseZero10IntValidator );      // 0-10
    ui->param20->setDecimalPlaces(0);
    ui->param20->setInputWrapping( true );
    ui->param20->setInputTruncation( true );

    ui->param4->setValidator( baseZero10IntValidator );      // 0-10
    ui->param4->setDecimalPlaces(0);
    ui->param4->setInputWrapping( true );
    ui->param4->setInputTruncation( true );

    if (viewCurrentTimings_)
    {
        LOG_DEBUG( "initialize() if( viewCurrentTimings_ )", NULL );

        ui->txtMinLength->setValidator( lengthValidator_ = new QIntValidator(-5, WB_MAX_LENGTH, this) );
        ui->txtMinLength->setDecimalPlaces(1);
        ui->txtMinLength->setInputWrapping( true );
        ui->txtMinLength->setInputTruncation( true );
        lengthValidator_->setTop(WB_MAX_LENGTH);

        ui->txtMaxLength->setValidator( lengthValidator_ );
        ui->txtMaxLength->setDecimalPlaces(1);
        ui->txtMaxLength->setInputTruncation( true );
        ui->txtMaxLength->setInputWrapping(true);

        ui->txtMinWidth->setValidator( widthValidator_ = new QIntValidator(-5, WB_MAX_WIDTH, this) );
        ui->txtMinWidth->setDecimalPlaces(1);
        ui->txtMinWidth->setInputTruncation( true );
        ui->txtMinWidth->setInputWrapping(true);
        widthValidator_->setTop(WB_MAX_WIDTH);

        ui->txtMaxWidth->setValidator( widthValidator_ );
        ui->txtMaxWidth->setDecimalPlaces(1);
        ui->txtMaxWidth->setInputTruncation( true );
        ui->txtMaxWidth->setInputWrapping(true);

        ui->txtMaxLip->setValidator( lipValidator_ = new QIntValidator(-5, WB_MAX_LIP, this) );
        ui->txtMaxLip->setDecimalPlaces(1);
        ui->txtMaxLip->setInputTruncation( true );
        ui->txtMaxLip->setInputWrapping(true);
        widthValidator_->setTop(WB_MAX_WIDTH);
    }
    else
    {
        ui->txtMinLength->setEnabled(false);
        ui->txtMaxLength->setEnabled(false);
        ui->txtMinWidth->setEnabled(false);
        ui->txtMaxWidth->setEnabled(false);
        ui->txtMaxLip->setEnabled(false);
    }

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    if (viewCurrentTimings_)
    {
        modifyPaletteBackground setEditBoxBackground( ( QApplication::palette() ).background());
        setEditBoxBackground( serviceMode );
    }
    else
    {
        modifyPaletteBackground setDefaultTimingsBackground( hobart::colorPalette::red );
        setDefaultTimingsBackground( serviceMode );
    }

    //	Setup All Height Boxes combo.
    ui->cboAllHeightBoxes->clear();
    ui->cboAllHeightBoxes->addItem("0.5","0.5");
    ui->cboAllHeightBoxes->addItem("0.7","0.7");
    ui->cboAllHeightBoxes->addItem("1.0","1.0");
    ui->cboAllHeightBoxes->addItem("1.2","1.2");
    ui->cboAllHeightBoxes->addItem("1.5","1.5");
    ui->cboAllHeightBoxes->addItem("1.7","1.7");
    ui->cboAllHeightBoxes->addItem("2.0","2.0");
    ui->cboAllHeightBoxes->addItem("2.2","2.2");
    ui->cboAllHeightBoxes->addItem("2.5","2.5");
    ui->cboAllHeightBoxes->addItem("3.0","3.0");
    ui->cboAllHeightBoxes->addItem("3.5","3.5");
    ui->cboAllHeightBoxes->addItem("4.0","4.0");
    ui->cboAllHeightBoxes->addItem("4.5","4.5");
    ui->cboAllHeightBoxes->addItem("5.0","5.0");
    ui->cboAllHeightBoxes->addItem("6.0","6.0");

    if ( wrTimingsData_->updateActualPkg )
    {
        ui->lblTrayFound->setText(wrTimingsData_->cycledTrayName);
    }

    ui->cboAllHeightBoxes->hide();
    ui->cboAvailableHeightBoxes->hide();
    ui->butCancelNewHeightBox->hide();
    ui->lblAddHeightBox->hide();

    wrapParamOrdering_.clear();

    update();

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmViewEdit)
    {
        session_->send(Wrapper, WrEvolReadWrapBox, QString::number( activeRow ));              // 9,240,precedence
    }
    else if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
    {
        //  Create a new Tray name
        connect(ui->butAutoConfig,            SIGNAL(clicked()),                  this,       SLOT(autoConfigureTray()));
    }
    else if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect )
    {
        LOG_DEBUG(  "serviceEditBox::initialize():  session_->send(Wrapper, WrEvolAutoDetectTrayType)", NULL );
        session_->send(Wrapper, WrEvolAutoDetectTrayType);              // 9,232
    }

    LOG_DEBUG( "wrTimingsData_->heightBoxSelected = %s", qPrintable(wrTimingsData_->heightBoxSelected) );
    if ( wrTimingsData_->heightBoxSelected != "" )
    {
        QString msg2 = QString::number( activeRow ) + "," + wrTimingsData_->heightBoxSelected;

        LOG_DEBUG(  "serviceEditBox::initialize():  session_->send(Wrapper, WrEvolReadHeightBox) %s ", qPrintable(msg2) );
        session_->send(Wrapper, WrEvolReadHeightBox, msg2);              // 9,246,precedence,heightBox
    }

    cycleWrapperTestLabel_ = new QTimer( this );
    connect( cycleWrapperTestLabel_, SIGNAL( timeout() ), SLOT( cycleWrapperSetupTime() ) );

    // SOF-2808: Need to hide the Print button when in Auto Config mode
    if( WrapperTimingsData::pmAutoConfigure == wrTimingsData_->pageMode )
    {
        // TODO - will need whole list
        ui->butPrintTab->hide();
    }

   if( !session_->scaleConfig->isWrapperTwoPrinter() )
   {
       ui->butParam20->setVisible(false);
       ui->lblMove20->setVisible(false);
       ui->butLeftParam20->setVisible(false);
       ui->param20->setVisible(false);
       ui->lblOrigParam20->setVisible(false);
       ui->butRightParam20->setVisible(false);

       ui->butParam4->setVisible(false);
       ui->lblMove4->setVisible(false);
       ui->butLeftParam4->setVisible(false);
       ui->param4->setVisible(false);
       ui->lblOrigParam4->setVisible(false);
       ui->butRightParam4->setVisible(false);
   }

    pLabelSize_ = "0";
    sLabelSize_ = "0";

    session_->send( Printer, PrReqStockMetrics, QString::number( 0 ) );         // 7,40,0 - send in case we decide to cycle wrapper, 0 = Primary Printer

    hasChanged_ = false;
    finishedState = DONE_STATE;
    pythonProcess = new python();

    update();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::translate()
{
    ui->retranslateUi( this );

    ui->param14->setItemText(0, tr("Narrow roll") );
    ui->param14->setItemText(1, tr("Wide roll") );

    update();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperEditBox::writeHeightBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "writeHeightBox" );
    //LOG_DEBUG( "writeHeightBox() START", NULL );
    //LOG_DEBUG( "writeHeightBox() Num ofParams %d", wrTimingsData_->wrapParameterList.count() );

    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        if( heightBox_ )//HA-127 this variable wasn't initialized when attempting to send the message
        {
            int numOfParams = wrTimingsData_->wrapParameterList.count();
            QString msg2 = QString::number(activeRow);
            msg2 += "," + heightBox_->maxHeight();
            msg2 += "," + QString::number(numOfParams);
            msg2 += ",\"";
            LOG_DEBUG( "writeHeightBox() saveParams() msg2 #1: %s", qPrintable(msg2) );

            for (int j = 0; j < numOfParams; j++)
            {
                if (j > 0)
                    msg2 += ",";

                int value( wrTimingsData_->wrapParameterList[j]->getData() );

                //LOG_DEBUG( "writeHeightBox() saveParams() value: d", value );

                // LOG_DEBUG( "writeHeightBox() for loop value[%d]: %d", j, value);
                // Right now, the 2 new values for 2P, Pri and Sec VERT
                // offsets are saving, so I'm not going to touch the 4.2.0 mods made.
                if ( ( WrapParameter::FRONT_UF_ADJUST == j ) || ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == j ) )
                {
                    if( ( value > 0 ) && ( value <= 100 )  )
                    {
                        value += 100;
                    }
                }

                //                                     [2]Side to Side wrap tension(2)            [5]Front to Back Wrap Tension(5)
                if ( ( WrapParameter::CLAMP_HEIGHT == j ) || ( WrapParameter::GRIPPER_ADJUST == j ) )
                {
                    if( ( value >= -5 ) && ( value < 0 ) )
                        value = ( ( value * -1 ) + 100 );
                }

                if( ( WrapParameter::PRIMARY_LABEL_VERT_OFFSET == j ) || ( WrapParameter::SECONDARY_LABEL_VERT_OFFSET == j ) )
                {
                    if( session_->scaleConfig->isWrapperTwoPrinter() )
                        msg2 += QString::number(value);     // so dont send the current last 2 params if we are not 2P. Could of sent them as the same
                }
                else
                    msg2 += QString::number(value);

                LOG_DEBUG( "writeHeightBox() Loop[%d] msg2 #2: %s", j, qPrintable(msg2) );
            }

            msg2 += "\"";

            LOG_DEBUG( "writeHeightBox() sending msg2: %s", qPrintable(msg2) );

            session_->send(Wrapper, WrEvolWriteHeightBox, msg2);              // 9,247,precedence,maxHeight,heightBoxes list
        }
    }

    //LOG_DEBUG( "writeHeightBox() END", NULL );
}

void serviceNewWrapperEditBox::writeWrapBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "writeWrapBox" );

    LOG_DEBUG(  "writeWrapBox() START ", NULL );

    // SOF-2734
    QString trayNameQuotedForCSV( ui->txtTrayName->text() );
    trayNameQuotedForCSV.replace( "\"", "\"\"" );
    trayNameQuotedForCSV.prepend( "\"" );
    trayNameQuotedForCSV.append( "\"" );

    QString msg2 = QString::number(activeRow);
    msg2 += "," + trayNameQuotedForCSV;
    msg2 += ",0";                                              // once was the plu excption - now just 0
    msg2 += "," + ui->txtMinLength->text();
    msg2 += "," + ui->txtMaxLength->text();
    msg2 += "," + ui->txtMinWidth->text();
    msg2 += "," + ui->txtMaxWidth->text();
    msg2 += "," + ui->txtMaxLip->text();
    msg2 += "," + QString::number(ui->cboHeightBox->count());
    msg2 += ",\"";
    for (int j = 0; j < ui->cboHeightBox->count(); j++)
    {
        if (j > 0)
            msg2 += ",";
        msg2 += ui->cboHeightBox->itemData(j).toString();
    }
    msg2 += "\"";


    LOG_DEBUG(  "writeWrapBox():  send(9,241) %s ", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolWriteWrapBox, msg2);              // 9,241,precedence,trayName,...
    LOG_DEBUG(  "writeWrapBox() DONE ", NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperEditBox::makeValidatorTop( int digits )
{   LOG_CLASS_INFO( "serviceEditBox", "makeValidatorTop" );

    LOG_DEBUG("makeValidatorTop() digits %d ", digits );
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }

    LOG_DEBUG(  "makeValidatorTop() digits %d ", top );
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::saveDataOnExit()
{
    LOG_CLASS_INFO( "serviceEditBox", "saveDataOnExit" );
    LOG_DEBUG( "saveDataOnExit() wrTimingsData_->pageMode: %d", wrTimingsData_->pageMode );

    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect))
    {
        if (firstPass == false)
            onSaveChanges();
    }
    else
    {
        onSaveChanges();
    }

    LOG_DEBUG( "saveDataOnExit() DONE", NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperEditBox::setMinValues()
{
    LOG_CLASS_INFO( "serviceEditBox", "setMinValues" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    QString msg2 = QString::number( activeRow );

    LOG_DEBUG( "setMinValues() session_->send(Wrapper, WrEvolSetMinTrayDims): %s", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolSetMinTrayDims, msg2);			// 9,147,precedence
}

void serviceNewWrapperEditBox::setMaxValues()
{
    LOG_CLASS_INFO( "serviceEditBox", "setMaxValues" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    QString msg2 = QString::number( activeRow );

    LOG_DEBUG( "setMaxValues() session_->send(Wrapper, WrEvolSetMinTrayDims): %s", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolSetMaxTrayDims, msg2);			// 9,148,precedence
}

void serviceNewWrapperEditBox::setMinMaxValues()
{
    LOG_CLASS_INFO( "serviceEditBox", "setMinMaxValues" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    QString msg2 = QString::number( activeRow );

    LOG_DEBUG( "setMinMaxValues() session_->send(Wrapper, WrEvolSetMinTrayDims): %s", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolSetMinMaxTrayDims, msg2);			// 9,149,precedence
}

void serviceNewWrapperEditBox::cycleWrapper()
{
    LOG_CLASS_INFO("serviceEditBox", "cycleWrapper");

    if ( cycleWrapperInProgress_ )
        return;

    ui->butCycleWrapperTab1->setStyleSheet("background-color: red;");
    ui->butCycleWrapperTab2->setStyleSheet("background-color: red;");
    ui->butCycleWrapperTab3->setStyleSheet("background-color: red;");
    ui->butCycleWrapperTab4->setStyleSheet("background-color: red;");
    ui->butCycleWrapperTab5->setStyleSheet("background-color: red;");

    if (hasChanged_)
    {
        //  Only save if we are viewing current timings - not default timings.
        writeWrapBox();
        writeHeightBox();

        //	HA-81
        session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));		// 9,244, precedence
    }

    //send test label to apply
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        //send lasetup
        QString pLabelSize   = pLabelSize_ + ",";
        QString pRotation    = "0,";
        QString pPlacement   = "1,";		//leading = 0, trailing = 1     ( Primary is trailing )

        QString sLabelSize = sLabelSize_ + ",";
        QString sRotation    = "0,";
        QString sPlacement   = "0,";		//leading = 0, trailing = 1     ( Secondary is leading )
        QString sRotateLabel = QString::number( operatorDisplayFields->secLabelStockRotated() );

        QString setup = pLabelSize + pRotation + pPlacement + sLabelSize + sRotation + sPlacement + sRotateLabel;
        LOG_DEBUG( "cycleWrapper() if setup: %s", qPrintable(setup) );
        session_->send( LabelApplier, LaSetup, setup );                                         // SEND #1: 5,14(LaSetup)

        QString printer = "0,";
        QString testLabel = "2,";
        QString qty = "0";
        QString label = printer + testLabel + qty;
        LOG_DEBUG( "cycleWrapper() if label: %s", qPrintable(label) );
        session_->send( Printer, PrTestLabel, label );                                          // SEND #2: 7,31,test label checkboard

        QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;
        LOG_DEBUG( "cycleWrapper() if verifyMsg: %s", qPrintable(verifyMsg) );
        session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);                            // SEND #3: 9,250 (print label)
        session_->send(Wrapper, WrEvolTest, "24,0,0");           // SEND #4: 9,108,24,plu,printer (primary)     WrapperCycle, PLU Excption( now just 0) (this fires the Label Ready Message on the Application Side

        printer = "1,";
        label = printer + testLabel + qty;
        LOG_DEBUG( "cycleWrapper() if label: %s", qPrintable(label) );
        session_->send( Printer, PrTestLabel, label );                                          // SEND #5: 7,31,test label checkboard
    }
    else
    {
        QString printer = "0,";
        if(leadingEdge)
            printer = "1,";

        QString testLabel = "2,";
        QString qty = "0";
        QString label = printer + testLabel + qty;

        session_->send( Printer, PrTestLabel, label );
        LOG_DEBUG( "cycleWrapper() else label: %s", qPrintable(label) );
        QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;        //	HA-135
        session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);
        LOG_DEBUG( "cycleWrapper() else verifyMsg: %s", qPrintable(verifyMsg) );
        session_->send(Wrapper, WrEvolTest, "24,0," + printer);				// 9,108, WrapperCycle, PLU Excption (0 NOW - excption gone)
    }
    cycleWrapperInProgress_ = true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::currentChangeTab(int tabNum)
{
    // dont have anything for now, but I will
    // could use it to save tab data
    Q_UNUSED(tabNum);

    return;
}

// SOF-2550
void serviceNewWrapperEditBox::cycleWrapperLeadingEdge()
{
    if(session_->scaleConfig->isWrapperTwoPrinter())
    {
        // SOF-5559 Cycle wrapper leading edge and Trailing edge doesn't work
        // which working this issue on a single printer wrapper in the else down below,
        // I'm not completely convinced this actually works on a 2p System.  TEST IT

        leadingEdge = true;

        // need to request the printer stock to get the label sizes
        // cycling the wrapper will now happen when we receive the printer stock
        session_->send( Printer, PrReqStockMetrics, QString::number( 1 ) );
    }
    else
    {
        // Then we have to handle a Single Printer Wrapper like we always have done, but was removed here
        // SOF-5559 Cycle wrapper leading edge and Trailing edge doesn't work
        // Bring back original single printer functionality which was removed

        //send lasetup
        QString pLabelSize = "0,";
        QString pRotation = "0,";
        QString pPlacement = "0,";		//leading = 0, trailing = 1

        QString sLabelSize = "0,";
        QString sRotation = "0,";
        QString sPlacement = "0";		//leading = 0, trailing = 1

        QString setup = pLabelSize + pRotation + pPlacement + sLabelSize + sRotation + sPlacement;
        session_->send( LabelApplier, LaSetup, setup );

        // Now cycle the wrapper with this setup
        cycleWrapper();
    }
}

// SOF-2550
void serviceNewWrapperEditBox::cycleWrapperTrailingEdge()
{
    if(session_->scaleConfig->isWrapperTwoPrinter())
    {
        // SOF-5559 Cycle wrapper leading edge and Trailing edge doesn't work
        // which working this issue on a single printer wrapper in the else down below,
        // I'm not completely convinced this actually works on a 2p System.  TEST IT

        leadingEdge = false;

        // need to request the printer stock to get the label sizes
        // cycling the wrapper will now happen when we receive the printer stock
        session_->send( Printer, PrReqStockMetrics, QString::number( 0 ) );
    }
    else
    {
        // Then we have to handle a Single Printer Wrapper like we always have done, but was removed here
        // SOF-5559 Cycle wrapper leading edge and Trailing edge doesn't work
        // Bring back original single printer functionality which was removed

        //send lasetup
        QString pLabelSize = "0,";
        QString pRotation = "0,";
        QString pPlacement = "0,";		//leading = 0, trailing = 1

        QString sLabelSize = "0,";
        QString sRotation = "0,";
        QString sPlacement = "1";		//leading = 0, trailing = 1

        QString setup = pLabelSize + pRotation + pPlacement + sLabelSize + sRotation + sPlacement;
        session_->send( LabelApplier, LaSetup, setup );

        // Now cycle the wrapper with this setup
        cycleWrapper();
    }
}

void serviceNewWrapperEditBox::onExportTimings()
{
    LOG_CLASS_INFO( "serviceEditBox", "onExportTimings" );

    LOG_DEBUG( "onExportTimings() - START", NULL );

    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    FileDialog filepicker( mainWindow_ );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    LOG_DEBUG( "onExportTimings() - set up filepicker", NULL );

    QStringList folders;

#if defined( Q_OS_LINUX )
    folders.push_back( "/media/thumb" );
    folders.push_back( "/media/" );
    folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
    folders.push_back( "C:\\Hobart\\Timings" );
#endif

    LOG_DEBUG( "onExportTimings() - set up folders", NULL );

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    LOG_DEBUG( "onExportTimings() - found existing folder", NULL );

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save file" ) );

    LOG_DEBUG( "onExportTimings() - Pop Filedialog", NULL );


    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            //session_->send(Wrapper, WrEvolSavePackageTimingsToDisc, fileName);

            QStringList args;
            args.append(fileName);
            args.append(QString::number(session_->scaleConfig->internalConfig.priVertPrintPosition));
            args.append(QString::number(session_->scaleConfig->internalConfig.secVertPrintPosition));
            args.append(session_->scaleConfig->evolWrapperStatus.sealerBeltTemp());

            LOG_DEBUG( "onExportTimings() - WHAT IS the fileName: '%s'", qPrintable(args[0]) );
            //LOG_DEBUG( "onExportTimings() - WHAT IS the fileName: '%s'", qPrintable(args[1]) );
            //LOG_DEBUG( "onExportTimings() - WHAT IS the fileName: '%s'", qPrintable(args[2]) );
            //LOG_DEBUG( "onExportTimings() - WHAT IS the fileName: '%s'", qPrintable(args[3]) );

            if( pythonProcess->execCommand("exportWrapperTimings", args))
            {

                DomCurrentScaleSetup *versioninfo_ = &session_->scaleConfig->currentScaleSetup;
                QString ipAddr = versioninfo_->scaleIpAddress();
                ipAddr = ipAddr.replace('.', '_');
                QString fileExt = ".csv";
                QString savedFile = fileName + '_' + ipAddr + fileExt;

                if(QFile::exists( savedFile ))
                    ui->lblHelpTab2->setText("* Timings report saved as: " + savedFile);
                else
                    ui->lblHelpTab2->setText("* Problem saving timings report: " + savedFile);
            }

#if defined( Q_OS_LINUX )
                system("sync");         // flush the file to disc
#endif
        }
    }

    LOG_DEBUG( "onExportTimings() - END", NULL );
}

/** checkFileName()
 *
 *  checkFileName checks to determine if a valid file was entered. If the filename
 *  is an empty string, or one of a number of illegal characters, a warning will
 *  be displayed to the user indicating the problem.
 *  \param: myfile       - the file name the user entered
 *          fileOrFolder - indicates if a filename or foldername was entered
 *  \return: bool - indicating if a valid file was entered, or not.
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
bool serviceNewWrapperEditBox::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );          // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                else
                    validFile = true;
            }
        }

        int tabIndex = ui->tabServiceTimings->currentIndex();
        savedExport_ = true;
        if ( validFile )
        {
            if( ( tabIndex >= 2 ) && ( tabIndex <= 4 ) )
                ui->lblHelpTab2->setText("* Timings report has been saved.");
        }
        else
        {
            if( 2 == tabIndex)
                ui->lblHelpTab2->setText( errStr );
            else if( 3 == tabIndex )
                ui->lblHelpTab3->setText( errStr );
            else if( 4 == tabIndex )
                ui->lblHelpTab4->setText( errStr );
            else if( 5 == tabIndex )
                ui->lblHelpTab5->setText( errStr );
        }
    }

    return validFile;
}

/** getFileName()
 *
 *  getFileName determines if a filename was entered with an extension. The extension
 *  is displayed in the file type combo box, and is automatically append the extension
 *  selected by default. However, without question some user will add the extension
 *  while entering the file name itself. This method fixes any issues that may be
 *  encountered if the user adds the entension themselves.
 *  \param: myfile       - the file name the user entered
 *  \return: String - the valid file name with the extension properly appended.
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
// Need to figure out, what this is trying to figure out.
QString serviceNewWrapperEditBox::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

   // FT-101 Backing up a database results in a file with no extension
   // Still may have to make some more additions for LINUX. Needs seriously tested

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if(myfile.indexOf(".") > 0)
        {
            QFileInfo fileInfo1(myfile);
            QString strippedFileName = fileInfo1.completeBaseName();
            retFile = strippedFileName;
        }
        else
        {
            retFile = myfile;
        }
    }


    return retFile;
}

void serviceNewWrapperEditBox::onCancelChanges()
{
    LOG_CLASS_INFO( "serviceEditBox", "onCancelChanges" );

    if( finishedState == DONE_STATE )
    {
        LOG_DEBUG( "serviceEditBox::onCancelChanges() DONE_STATE so just leave", NULL );
        hasChanged_ = false;
        wrTimingsData_->exitByMenu = false;
        session_->scaleConfig->internalConfig.returningFromTabs = true;
        finishedState = DONE_STATE;
        ui->butCancelDone->setText( tr("Done"));
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
    }
    else if( finishedState == CANCEL_STATE )
    {
        LOG_DEBUG( "serviceEditBox::onCancelChanges() CANCEL_STATE so CANCEL save", NULL );
        hasChanged_ = false;
        wrTimingsData_->exitByMenu = true;
        ui->butCancelDone->setText( tr("Done") );
        finishedState = DONE_STATE;

        // original cancel message
        QString msg2 = QString::number( activeRow );

        session_->send(Wrapper, WrEvolCancelWrapBoxChanges, msg2);                      // 9,243,precedence
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
    }
}

void serviceNewWrapperEditBox::onSaveChanges()
{
    LOG_CLASS_INFO( "serviceEditBox", "onSaveChanges" );

    if( hasChanged_ )
    {
        LOG_DEBUG( "onSaveChanges() hasChanged_ = TRUE so CALL writeWrapBox() and writeHeightBox()", NULL );
        QString msg2 = QString::number( activeRow );

        writeWrapBox();
        writeHeightBox();                                       // save timings

        session_->send(Wrapper, WrEvolDoneWrapBox, msg2);		// 9,244,precedence

        if( finishedState == CANCEL_STATE )
        {
            finishedState = DONE_STATE;
            ui->butCancelDone->setText( tr("Done"));
        }

        // save current values over to the originals now
        ui->lblOrigParam0->setText(ui->param0->text());
        ui->lblOrigParam2->setText(ui->param2->text());
        ui->lblOrigParam9->setText(ui->param9->text());

        ui->lblOrigParam5->setText(ui->param5->text());
        ui->lblOrigParam15->setText(ui->param15->text());
        ui->lblOrigParam10->setText(ui->param10->text());
        ui->lblOrigParam3->setText(ui->param3->text());

        ui->lblOrigParam6->setText(ui->param6->text());
        ui->lblOrigParam4->setText(ui->param4->text());
        ui->lblOrigParam20->setText(ui->param20->text());

        ui->lblOrigParam7->setText(ui->param7->text());
        ui->lblOrigParam8->setText(ui->param8->text());
        ui->lblOrigParam16->setText(ui->param16->text());
        ui->lblOrigParam14->setText( QString::number(ui->param14->currentIndex()) );

        hasChanged_ = false;
    }

    if( savedExport_ )
    {
        // means we saved an export and we changed the help text someplace.  Reset them all
        ui->lblHelpTab2->setText("* Click any text link to see HELP and a full description how the modified Timing change affects the Wrapper. ");
        ui->lblHelpTab3->setText("* Click any text link to see HELP and a full description how the modified Timing change affects the Wrapper. ");
        ui->lblHelpTab4->setText("* Click any text link to see HELP and a full description how the modified Timing change affects the Wrapper. ");
        ui->lblHelpTab5->setText("* Click any text link to see HELP and a full description how the modified Timing change affects the Wrapper. ");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onEditingFinished( QWidget *widget )
{
    LOG_CLASS_INFO( "serviceEditBox", "onEditingFinished" );

    // on clicking [ ENTER ] key on keyboard
    QLineEdit* new_label = (QLineEdit*) widget;

    LOG_DEBUG( "serviceEditBox::onEditingFinished() incoming Text: %s", qPrintable( new_label->text() ) );

    int value = new_label->text().toInt();

    if( widget == ui->param3 )
    {
        // debug - in case I want to do something to an individual parameter - if not, clean all this up
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param3", NULL );
        if ( value < 10 )
            new_label->setText("10");
        else if( value > 40)
            new_label->setText("40");

        validateEntry( WrapParameter::DISTANCE, value );
    }
    else if( widget == ui->param0 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param0", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 60)
            new_label->setText("60");

        validateEntry( WrapParameter::PERCENT_STRETCH_CLAMP_ON, value );
    }
    else if( widget == ui->param2 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param2", NULL );
        if ( value < -5)
            new_label->setText("-5");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::CLAMP_HEIGHT, value );
    }
    else if( widget == ui->param9 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param9", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::SIDE_UF_ADJUST, value );
    }
    else if( widget == ui->param5 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param5", NULL );
        if ( value < -5 )
            new_label->setText("-5");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::GRIPPER_ADJUST, value );
    }
    else if( widget == ui->param15 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param15", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 5)
            new_label->setText("5");

        validateEntry( WrapParameter::REAR_UF_TRIP_DELAY, value );
    }
    else if( widget == ui->param10 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param10", NULL );
        if ( value < 0 )
            new_label->setText("10");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::HEIGHT_RESTRICT, value );
    }
    else if( widget == ui->param16 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param16", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 10)
            new_label->setText("10");

         validateEntry( WrapParameter::BRAKE_TIMING, value );
    }
    else if( widget == ui->param7 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param7", NULL );
        if ( value < 12 )
            new_label->setText("12");
        else if( value > 30)
            new_label->setText("30");

        validateEntry( WrapParameter::WRAP_SPEED, value );
    }
    else if( widget == ui->param8 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param8", NULL );
        if ( value < 1 )
            new_label->setText("1");
        else if( value > 30)
            new_label->setText("30");

       validateEntry( WrapParameter::DISCHARGE_SPEED, value );
    }
    else if( widget == ui->param6 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param6", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::FRONT_UF_ADJUST, value );
    }
    else if( widget == ui->param4 )
    {
        LOG_DEBUG( "serviceEditBox::onEditingFinished() param4", NULL );
        if ( value < 0 )
            new_label->setText("0");
        else if( value > 10)
            new_label->setText("10");

        validateEntry( WrapParameter::FORCE_OUTSIDE_ELEVATOR, value );
    }
    else if( widget == ui->param20 )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            LOG_DEBUG( "serviceEditBox::onEditingFinished() param20", NULL );
            if ( value < 0 )
                new_label->setText("0");
            else if( value > 10)
                new_label->setText("10");

            validateEntry( WrapParameter::SECONDARY_LABEL_VERT_OFFSET, value );
        }
    }


    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    hasChanged_ = true;
}

// come back to check hasChanged_
void serviceNewWrapperEditBox::requestHeightBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "requstHeightBox" );

    LOG_DEBUG( "serviceEditBox::requestHeightBox() occurs on keyboard entered data and clicking [ ENTER ] key", NULL);

    heightBoxSelected_ = ui->cboHeightBox->itemText(ui->cboHeightBox->currentIndex());

    QString msg2 = QString::number( activeRow ) + "," + heightBoxSelected_;

    if( hasChanged_ )
        writeHeightBox();

    LOG_DEBUG("serviceEditBox::requestHeightBox():  session_->send(Wrapper, WrEvolReadHeightBox msg2: %s", qPrintable(msg2));

    session_->send(Wrapper, WrEvolReadHeightBox, msg2);

    finishedState = CANCEL_STATE;
    ui->butCancelDone->setText( tr("Cancel"));
}

void serviceNewWrapperEditBox::newHeightBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "newHeightBox" );

    hasChanged_ = true;


    LOG_DEBUG(  "serviceEditBox::newHeightBox()", NULL);

    ui->cboHeightBox->hide();              //  Hide the current height box list
    ui->butNewHeightBox->hide();            //  Hide the New height box button
    ui->butDeleteHeightBox->hide();         //  Hide the Delete height box button
    ui->cboAvailableHeightBoxes->show();   //  Show the other heights available
    ui->butCancelNewHeightBox->show();      //  Allow them to cancel without adding a new height
    ui->butCancelNewHeightBox->blockSignals(false);
    ui->lblAddHeightBox->show();
}

void serviceNewWrapperEditBox::addNewHeightBox()
{
    LOG_CLASS_INFO( "serviceEditBox", "addNewHeightBox" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    LOG_DEBUG( "serviceEditBox::addNewHeightBox()", NULL);

    //  Get the new height string
    QString newHeight = ui->cboAvailableHeightBoxes->itemText(ui->cboAvailableHeightBoxes->currentIndex());
    QString currHeight = ui->cboHeightBox->itemText(ui->cboHeightBox->currentIndex());

    ui->cboAvailableHeightBoxes->blockSignals(true);
    ui->cboHeightBox->blockSignals(true);

    //  Remove the height from the available list
    ui->cboAvailableHeightBoxes->removeItem(ui->cboAvailableHeightBoxes->currentIndex());

    //  Add the height to the current list
    ui->cboHeightBox->addItem(newHeight, newHeight);

    //  Put the fields back to normal
    ui->cboAvailableHeightBoxes->hide();   //  Hide the other heights available
    ui->butCancelNewHeightBox->hide();      //  Hide the cancel new height button
    ui->lblAddHeightBox->hide();
    ui->butDeleteHeightBox->show();         //  Show the Delete height box button
    ui->butNewHeightBox->show();            //  Show the New height box button
    ui->cboHeightBox->show();              //  Show the current height box list

    ui->cboAvailableHeightBoxes->blockSignals(false);
    ui->cboHeightBox->blockSignals(false);

    QString msg2 = QString::number( activeRow ) + "," + newHeight + "," + currHeight;
    LOG_DEBUG( "send: Wrapper, WrEvolCreateHeightBox, %s", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolCreateHeightBox, msg2);
//    heightBoxChanged = true;
}

void serviceNewWrapperEditBox::cancelNewHeightBox()
{
    //  Put the fields back to normal
    ui->cboAvailableHeightBoxes->hide();   //  Hide the other heights available
    ui->butCancelNewHeightBox->hide();      //  Hide the cancel new height button
    ui->lblAddHeightBox->hide();
    ui->butDeleteHeightBox->show();         //  Show the Delete height box button
    ui->butNewHeightBox->show();            //  Show the New height box button
    ui->cboHeightBox->show();              //  Show the current height box list
}

void serviceNewWrapperEditBox::deleteSelectedHeight()
{
    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    //	Send message to verify deletion of the tray
    session_->send(Error, Wrapper, "9709,1");              // 103,9,9709,1
}

void serviceNewWrapperEditBox::autoConfigureTray()
{
    LOG_CLASS_INFO( "serviceEditBox", "autoConfigureTray" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    // SOF-2734
    QString trayNameQuotedForCSV( wrTimingsData_->trayName );
    trayNameQuotedForCSV.replace( "\"", "\"\"" );
    trayNameQuotedForCSV.prepend( "\"" );
    trayNameQuotedForCSV.append( "\"" );

    QString msg2 = QString::number( activeRow ) + "," + trayNameQuotedForCSV;

    LOG_DEBUG( "autoConfigureTray msg2 %s", qPrintable(msg2) );

    session_->send(Wrapper, WrEvolAutoConfigTrayType, msg2);
}

void serviceNewWrapperEditBox::updateTrayName()
{
    LOG_CLASS_INFO( "serviceEditBox", "updateTrayName" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) && !gotNewTrayName_)
    {

        LOG_DEBUG("serviceEditBox::updateTrayName(): Requesting a new tray", NULL);

        //	HTA-79
        if (ui->txtTrayName->text().isEmpty())
        {
            //	This will force them to either reselect the Tray name field
            //	of press "Cancel" or "Done". There are no other options.
            LOG_DEBUG( "Tray name is null. Not sending Auto config message.", NULL );
        }
        else
        {
            gotNewTrayName_ = true;
            wrTimingsData_->trayName = ui->txtTrayName->text();

            LOG_DEBUG( "1) wrTimingsData_->trayName = [%s]", qPrintable( wrTimingsData_->trayName ) );
            autoConfigureTray();
            hideShowForNewTray(false);      //  Show all the fields for AutoConfig mode
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::cycleWrapperSetupTime()
{
    LOG_CLASS_INFO( "serviceEditBox", "cycleWrapperSetupTime" );

    // have to give the wrapper time to set up, make sure everyone is good to go and ready, and then cycle wrapper
    LOG_DEBUG( "cycleWrapperSetupTime(): Finish up cycling the wrapper after the delay to allow object setup on Firmware Side", NULL );

    QString printer = "0,";
    QString testLabel = "2,";
    QString qty = "0";
    QString label = printer + testLabel + qty;
    LOG_DEBUG("Label = %s", label.toStdString().c_str());
    session_->send( Printer, PrTestLabel, label );                                          // 7,31,test label checkboard

    QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;            //	HA-135
    session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);                            // 9,250
    session_->send(Wrapper, WrEvolTest, "24,0," + printer);  // 9,108      WrapperCycle, PLU Excption (0 NOW - excption gone) (this fires the Label Ready Message on the Application Side

    printer = "1,";
    label = printer + testLabel + qty;
    LOG_DEBUG("Label = %s", label.toStdString().c_str());
    session_->send( Printer, PrTestLabel, label );

    cycleWrapperInProgress_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onDisplayHelpText()
{
    LOG_CLASS_INFO( "serviceEditBox", "onDisplayHelpText" );


    QString dialogText;
    QPushButton* paramWidget = qobject_cast<QPushButton*>(sender());        // get label clicked

    if( !paramWidget )
        return;

    QString name = paramWidget->objectName();                               // get label name
    QString nameNumStr = name.replace("butParam", "");
    int paramNum = nameNumStr.toInt();

    LOG_DEBUG( "onDisplayHelpText() - get the HELP tetext", NULL);

    MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );

    // setup help text for popup
    switch ( paramNum )
    {
        case WrapParameter::DISTANCE:
            dialog->setTitle( tr( "Package Position on Elevator" ) );
            dialogText =
                tr( "The distance the package travels past the elevator sensor across the intake\n"
                    "\n"
                    "Use this parameter to set the package position on the elevator\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is closest to the operator, $HB_MAX is the farthest from the operator\n"
                    "Adjust position on the elevator until the rear and front flap are gathered the same changes the length of the rear and front flap" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_DISTANCE) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_DISTANCE) );
        break;

        case WrapParameter::PERCENT_STRETCH_CLAMP_ON:                       // Used by SideToSidePreStretch
            dialog->setTitle( tr( "Side to Side Pre-Stretch" ) );
            dialogText =
                tr( "Controls the distance the side clamps move in before closing on the film\n"
                    "\n"
                    "Use this parameter to adjust the prestretch of the film\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is least film in clamps; least prestretch, tension, $HB_MAX is the most film in clamps; most prestretch" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_PERCENT) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_PERCENT) );
        break;

        case WrapParameter::CLAMP_HEIGHT:
            dialog->setTitle( tr( "Side to Side Wrap Tension" ) );
            dialogText =
                tr( "Controls when the side clamps open\n"
                    "\n"
                    "Use this parameter to control side to side tension\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is the earliest release; looser, and $HB_MAX being the latest release; tighter\n"
                    "* Use in conjunction with side to side pre-stretch.  The higher the prestretch the earlier you can open the side clamps" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_CLAMP_OPEN) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_CLAMP_OPEN) );
        break;

        case WrapParameter::SIDE_UF_ADJUST:
            dialog->setTitle( tr( "Side Folder Travel" ) );
            dialogText =
                tr( "Controls the distance the side underfolders travel in toward the center\n"
                    "\n"
                    "Use this parameter to guide the film under the footprint of the tray, usually deep trays require additional travel\n"
                    "Use this parameter to increase support of wide trays such as 10s, 16s, 7s\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is the least travel and $HB_MAX the most travel" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_SIDE_FOLD_ADJ) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_SIDE_FOLD_ADJ) );
        break;

        case WrapParameter::GRIPPER_ADJUST:
            dialog->setTitle( tr( "Front to Back Wrap Tension" ) );
            dialogText =
                tr( "Controls the brake pulse rate as the package is pushed up through the film plane and as the rear underfolder moves under the package before the knife cuts the film\n"
                    "\n"
                    "Use this parameter to increase or decrease the tension of the rear flap of film\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is least tension, $HB_MAX is the highest tension\n"
                    "* Wrap tension in service is medium, if wrap tension is set to high in operate, packages set up in service may crush" );
            dialogText.replace( "$HB_MIN", xstr(HB_NEG_MIN_GRIPPER_ADJ) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_GRIPPER_ADJ) );
        break;

        case WrapParameter::REAR_UF_TRIP_DELAY:
            dialog->setTitle( tr( "Rear Underfolder Travel" ) );
            dialogText =
                tr( "Controls how far the rear underfolder travels under the package before cutting\n"
                    "\n"
                    "Use this parameter to increase the length of the rear flap of film\n"
                    "Use this parameter to increase the tension of the rear flap\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is the minimum rear underfolder travel under the package $HB_MAX is the most travel\n"
                    "Usually start with front to back tension 0, rear underfolder travel 0, then increase front to back tension until the rear flap is tight\n"
                    "If you get to 9 without a tight rear flap, increase rear underfolder travel by 1 and return front to back tension to 0 then increase front to back tension until the rear flap is tight\n"
                    "Normally rear underfolder travel is 0 for short trays, 2 or 3 for medium length trays" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_REAR_TRIP_DELAY) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_REAR_TRIP_DELAY) );
        break;

        case WrapParameter::HEIGHT_RESTRICT:                            // Used by GripperRelax
            dialog->setTitle( tr( "Gripper Relax" ) );
            dialogText =
                tr( "Controls the distance the gripper moves back toward the elevator to relax the front flap tension\n"
                    "\n"
                    "Use this parameter to reduce front flap tension for trays that tear holes in front corners by increasing amount of relax\n"
                    "Use this parameter to reduce front tension on trays that roll over by increasing amount of relax\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is the least relax, $HB_MAX is the most relax\n"
                    "* This parameter will increase tension front to back as the height overide value is reduced" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_HEIGHT_RESTRICT) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_HEIGHT_RESTRICT) );
        break;

        case WrapParameter::BRAKE_TIMING:                               // Used by FilmPullLength
            dialog->setTitle( tr( "Film Pull Length" ) );
            dialogText =
                tr( "Controls the gripper travel on film pull to get more or less film front to back\n"
                    "\n"
                    "Use this parameter to control the front to back overlap\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN being the shortest lengtn of film pulled and $HB_MAX being the longest length of film pulled" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_BRAKE_TIME) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_BRAKE_TIME) );
        break;

        case WrapParameter::FILM_ROLL_TO_USE:
            dialog->setTitle( tr( "" ) );
            dialogText =
                tr( "Controls the film selector to force the machine to use upper or lower film\n"
                    "\n"
                    "Use this parameter to lock in the desired film roll to wrap the package\n"
                    "Range Narrow or Wide\n"
                    "When narrow is selected the machine will select whichever roll width is less as defined in operate / wrapper settings or service /wrapper configuration\n"
                    "When wide is selected the machine will select whichever roll width is greater as defined in operate / wrapper settings or service /wrapper configuration\n"
                    "* If film width has been overridden in wrapper settings, wrapper will select that film in service regardless of what the wrap parameter says" );
        break;

        case WrapParameter::WRAP_SPEED:
            dialog->setTitle( tr( "Wrap Speed" ) );
            dialogText =
                tr( "Controls the speed of the intake and the speed of the elevator\n"
                    "\n"
                    "Use this parameter to control wrap speed for unstable, top heavy, or frozen products\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is slowest, $HB_MAX is fastest, usually not less than 20 and not greater than 26" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_WRAP_SPEED) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_WRAP_SPEED) );
        break;

        case WrapParameter::DISCHARGE_SPEED:
            dialog->setTitle( tr( "Discharge Speed" ) );
            dialogText =
                tr( "Controls the speed of the upper pusher during discharge of the package\n"
                    "\n"
                    "Use this parameter to control wrap speed for unstable, top heavy products\n"
                    "Range $HB_MIN - $HB_MAX, ppm, $HB_MIN is slowest, $HB_MAX is fastest, usually not less than 5 and not greater than 30\n"
                    "* You may have different wrap and discharge speeds" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_DISCHARGE_SPEED) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_DISCHARGE_SPEED) );
        break;

        case WrapParameter::FRONT_UF_ADJUST:                                // Used by PrimaryLabelOffse
            dialog->setTitle( tr( "Primary Label Offset" ) );
            dialogText =
                tr( "Adjusts the label placement right or left when trailing edge\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_TRAILEDGE_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_TRAILEDGE_LABEL_OFFSET) );
        break;

        case WrapParameter::FORCE_OUTSIDE_ELEVATOR:                         // USED by SecondaryLabelOffset
            dialog->setTitle( tr( "Secondary Label Offset" ) );
            dialogText =
                tr( "Adjusts the label placement right or left when leading edge\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_TRAILEDGE_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_TRAILEDGE_LABEL_OFFSET) );
        break;

        case WrapParameter::SECONDARY_LABEL_VERT_OFFSET:
            if( session_->scaleConfig->isWrapperTwoPrinter() )
            {
	            dialog->setTitle( tr( "" ) );
	            dialogText =
	                tr( "Adjusts the label placement  up or down vertically\n"
	                    "\n"
	                    "Use this parameter to control label placement in quarter inch increments\n"
	                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
	            dialogText.replace( "$HB_MIN", xstr(HB_MIN_VERTICAL_LABEL_OFFSET) );
	            dialogText.replace( "$HB_MAX", xstr(HB_MAX_VERTICAL_LABEL_OFFSET) );
            }
        break;

    }

    if ( dialogText.length() )
    {
        dialog->setSizeToFit( true );
        dialog->setText( dialogText );
        dialog->setStandardButtons( QDialogButtonBox::Ok );

        dialog->exec();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::onScreenChanging()
{
    LOG_CLASS_INFO( "serviceEditBox", "onScreenChanging" );

    if (wrTimingsData_->exitByMenu)
    {
        LOG_DEBUG( "IF onScreenChanging() exitByMenu IS true then return FALSE and dont exit", NULL);
        return false;
    }
    else
    {
        LOG_DEBUG( "ELSE onScreenChanging() exitByMenu IS false, so CALL saveDataOnExit()", NULL);
        wrTimingsData_->heightBoxSelected = heightBoxSelected_;     //  Save the current height box.
        saveDataOnExit();
    }

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperEditBox::buildWrapDimensions(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "serviceEditBox", "buildWrapDimensions" );
    LOG_DEBUG( "buildWrapDimensions() START", NULL);

    DomWrapBoxParams wrapBoxParams( msg );

    wrTimingsData_->precedence = wrapBoxParams.precedence().toInt();
    wrTimingsData_->trayName = wrapBoxParams.trayName();
    wrTimingsData_->pluException = "0";             // now 0, could it be blank too?

    activeRow = wrTimingsData_->precedence;

    ui->txtTrayName->setText(wrTimingsData_->trayName);
    ui->lblTraySizeTab2->setText(wrTimingsData_->trayName);
    ui->lblTraySizeTab3->setText(wrTimingsData_->trayName);
    ui->lblTraySizeTab4->setText(wrTimingsData_->trayName);
    ui->lblTraySizeTab5->setText(wrTimingsData_->trayName);

    ui->txtMinLength->setText(wrapBoxParams.minLength());
    ui->txtMaxLength->setText(wrapBoxParams.maxLength());
    ui->txtMinWidth->setText(wrapBoxParams.minWidth());
    ui->txtMaxWidth->setText(wrapBoxParams.maxWidth());
    ui->txtMaxLip->setText(wrapBoxParams.maxLip());

    //  Set up our copy of the wrap box dimensions
    wrapBoxData_.setMinLength(getIntFromString(wrapBoxParams.minLength()));
    wrapBoxData_.setMaxLength(getIntFromString(wrapBoxParams.maxLength()));
    wrapBoxData_.setMinWidth(getIntFromString(wrapBoxParams.minWidth()));
    wrapBoxData_.setMaxWidth(getIntFromString(wrapBoxParams.maxWidth()));
    wrapBoxData_.setMaxLip(getIntFromString(wrapBoxParams.maxLip()));

    if (wrTimingsData_->canEditTrayName)
    {
        ui->txtMinLength->setDisabled(false);
        ui->txtMaxLength->setDisabled(false);
        ui->txtMinWidth->setDisabled(false);
        ui->txtMaxWidth->setDisabled(false);
        ui->txtMaxLip->setDisabled(false);
    }

    buildHeightCombo(wrapBoxParams);
}

void serviceNewWrapperEditBox::buildHeightCombo(DomWrapBoxParams &wrapBoxParams)
{
    LOG_CLASS_INFO( "serviceEditBox", "buildHeightCombo" );
    LOG_DEBUG( "serviceEditBox::buildHeightCombo()", NULL);

    boxCount = wrapBoxParams.numHeightBoxes().toInt();
    if (boxCount > 0)
    {
        //  Block the signals on these combo boxes until they are set up.
        ui->cboAvailableHeightBoxes->blockSignals(true);
        ui->cboHeightBox->blockSignals(true);

        ui->cboHeightBox->clear();
        ui->cboAvailableHeightBoxes->clear();

        //  Get the current list of height boxes for this wrap box
        QVector<QString> heightArray = TokenParser::splitStringTokens( wrapBoxParams.heightBoxes() );

        int x, index;
        int totalHeightBoxes = ui->cboAllHeightBoxes->count();
        QString allHeightStr, currHeightStr;
        for (x = 0, index = 0; x < totalHeightBoxes; x++)
        {
            allHeightStr = ui->cboAllHeightBoxes->itemText(x);
            if (index < boxCount)
            {
                currHeightStr = heightArray.at(index);
            }

            if (allHeightStr != currHeightStr)
            {
                ui->cboAvailableHeightBoxes->addItem(allHeightStr, allHeightStr);
            }
            else if (index < boxCount)
            {
                ui->cboHeightBox->addItem(currHeightStr, currHeightStr);
                ++index;
            }
        }
        //	Select the current height box
        if (heightBoxSelected_.isEmpty())
            index = 0;
        else
            index = ui->cboHeightBox->findText(heightBoxSelected_);
        ui->cboHeightBox->setCurrentIndex(index);

        ui->cboHeightBox->setEnabled(true);

        ui->cboAvailableHeightBoxes->hide();
        ui->cboAllHeightBoxes->hide();

        ui->cboAvailableHeightBoxes->blockSignals(false);
        ui->cboHeightBox->blockSignals(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::setupHeightBox(const SessionMessage& msg)
{
    if ( heightBox_ )
        delete heightBox_;

    heightBox_ = new DomHeightBox( msg );

    selectHeight();
    setupHeightBoxParams();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::selectHeight()
{
    if ( boxCount <= 0 )
        return;

    const int index( ui->cboHeightBox->findData( heightBox_->maxHeight() ) );

    if ( index < 0 )
        return;

    ui->cboHeightBox->blockSignals( true );
    ui->cboHeightBox->setCurrentIndex( index );
    ui->cboHeightBox->blockSignals( false );


    //  Set the Min height value
    ui->txtMinHeight->setText( 0 < index ? ui->cboHeightBox->itemData( index - 1 ).toString() : QString( "0" ) );

    //  Set the Max height value
    ui->txtMaxHeight->setText( ui->cboHeightBox->itemData( index ).toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::setupHeightBoxParams()
{
    LOG_CLASS_INFO( "serviceEditBox", "setupHeightBoxParams" );

    LOG_DEBUG( "setupHeightBoxParams() START box.attributes string from 9,147 msg: %s", qPrintable(heightBox_->wrapAttributes()) );

    QString test_attribs = heightBox_->wrapAttributes();

    LOG_DEBUG( "setupHeightBoxParams() create paramArray Vector with enum of params: %d", wrTimingsData_->wrapParameterList.count());
    QVector<QString> paramArray( TokenParser::splitStringTokens( heightBox_->wrapAttributes() ) );
    LOG_DEBUG( "setupHeightBoxParams() Num of Params split on paramArray Vector: %d", paramArray.count() );

    //Update our wrapParameterList with the height box params
    int numOfParams = wrTimingsData_->wrapParameterList.count();
    LOG_DEBUG( "setupHeightBoxParams() Num of Params of wrapParameterList: %d", wrTimingsData_->wrapParameterList.count() );

    for ( unsigned int i( numOfParams ); i--; )
    {
        WrapParameter *tmpParam;

        if ( (tmpParam = wrTimingsData_->wrapParameterList[i]) )                            // can /*trim this list down to only required elements
        {
            int value( paramArray[i].toInt() );
            LOG_DEBUG( "setupHeightBoxParams() for loop value[%d]: %d", i, value);

            if ( ( ( WrapParameter::FRONT_UF_ADJUST == i ) && ( 100 <= value ) ) ||
                 ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == i ) && ( 100 <= value ) ) )
            {
                paramArray[i] = QString::number( value -= 100 );
            }

            if ( ( WrapParameter::CLAMP_HEIGHT == i ) || ( WrapParameter::GRIPPER_ADJUST == i ) )
            {
                if( ( value > 100 ) && ( value <= 105 ) )
                {
                    value = ( ( value - 100 ) * -1 );
                    paramArray[i] = QString::number( value );
                }
            }

            tmpParam->setData( value );
        }
    }

    LOG_DEBUG( "setupHeightBoxParams() wrapParamOrdering_.count(): %d", wrapParamOrdering_.count() );
    //Now update the wrap parameters for the height box

    // Tab 2
    updateWrapParamNew(WrapParameter::DISTANCE, HB_MIN_10, HB_MAX_40,                   paramArray, ui->param3,  ui->lblOrigParam3);
    updateWrapParamNew(WrapParameter::PERCENT_STRETCH_CLAMP_ON, HB_MIN_00, HB_MAX_60,   paramArray, ui->param0,  ui->lblOrigParam0);
    updateWrapParamNew(WrapParameter::CLAMP_HEIGHT, HB_MIN_NEG_05, HB_MAX_10,           paramArray, ui->param2,  ui->lblOrigParam2);
    updateWrapParamNew(WrapParameter::SIDE_UF_ADJUST, HB_MIN_00, HB_MAX_10,             paramArray, ui->param9,  ui->lblOrigParam9);

    // tab3
    updateWrapParamNew(WrapParameter::GRIPPER_ADJUST, HB_MIN_NEG_05, HB_MAX_10,         paramArray, ui->param5,  ui->lblOrigParam5);
    updateWrapParamNew(WrapParameter::REAR_UF_TRIP_DELAY, HB_MIN_00, HB_MAX_05,         paramArray, ui->param15, ui->lblOrigParam15);
    updateWrapParamNew(WrapParameter::HEIGHT_RESTRICT, HB_MIN_00, HB_MAX_10,            paramArray, ui->param10, ui->lblOrigParam10);
    updateWrapParamNew(WrapParameter::BRAKE_TIMING, HB_MIN_00, HB_MAX_10,               paramArray, ui->param16, ui->lblOrigParam16);
    // ui->param14 COMBO at the bottom);

    // tab4
    updateWrapParamNew(WrapParameter::WRAP_SPEED, HB_MIN_12, HB_MAX_30,                  paramArray, ui->param7,  ui->lblOrigParam7);
    updateWrapParamNew(WrapParameter::DISCHARGE_SPEED, HB_MIN_01, HB_MAX_30,             paramArray, ui->param8,  ui->lblOrigParam8);
    updateWrapParamNew(WrapParameter::FRONT_UF_ADJUST, HB_MIN_00, HB_MAX_10,             paramArray, ui->param6,  ui->lblOrigParam6);
    updateWrapParamNew(WrapParameter::FORCE_OUTSIDE_ELEVATOR, HB_MIN_00, HB_MAX_10,      paramArray, ui->param4,  ui->lblOrigParam4);
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    	updateWrapParamNew(WrapParameter::SECONDARY_LABEL_VERT_OFFSET, HB_MIN_00, HB_MAX_10, paramArray, ui->param20, ui->lblOrigParam20);

    int index( ui->param14->findData( paramArray.at( WrapParameter::FILM_ROLL_TO_USE ) ) );
    LOG_DEBUG( "setupHeightBoxParams comboBox Original Parameter index=%d", index );

    if( ( index < 0 ) || ( index != 100 ) || index != 200 )
    {
        index = 0;
        wrTimingsData_->wrapParameterList[WrapParameter::FILM_ROLL_TO_USE]->setData( 100 );     // index0 = 100, index1 = 200, no idea why.
        LOG_DEBUG( "setupHeightBoxParams comboBox index out of range reset to ZERO", NULL );
    }

    if( index == 100 )

    LOG_DEBUG( "setupHeightBoxParams comboBox: at index=%d", index );

    ui->param14->blockSignals( true );
    if( index == 100 )
        ui->param14->setCurrentIndex( 0 );
    else if( index == 200 )
        ui->param14->setCurrentIndex( 1 );

    ui->param14->blockSignals( false );

    ui->lblOrigParam14->setText( QString::number(index) );
    LOG_DEBUG( "setupHeightBoxParams() if comboBox index: %d", index );

    LOG_DEBUG( "setupHeightBoxParams() END", NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::isUsingSimplifiedTimings()
{
    // SHOULD ALWAYS BE TRUE
    LOG_CLASS_INFO( "serviceEditBox", "isUsingSimplifiedTimings" );

    LOG_DEBUG( "isUsingSimplifiedTimings() is TRUE if model >= 10, model: %d ", wrTimingsData_->wrapperModelNum );

    return ( wrTimingsData_->wrapperModelNum >= DomEvolWrapperSystemInformation::reducedTimingModels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateWrapParam( QTableWidget* table, int wrapParam, QVector<QString> &paramArray, unsigned int row )
{
    LOG_CLASS_INFO( "serviceEditBox", "updateWrapParam" );

    if ( !table )
        return;

    int column( 2 );

    QSpinBox *spinBox;
    QComboBox *comboBox;

    if ( (spinBox = qobject_cast<QSpinBox*>( table->cellWidget( row, column ) )) )
    {
        LOG_DEBUG( "updateWrapParam() if spinBox: row[%d] x column[%d]", row, column );

        int value = 0;
        QString strVal = "";

        strVal = paramArray.at( wrapParam );
        value = paramArray.at( wrapParam ).toInt();

        LOG_DEBUG( "updateWrapParam() if spinBox: strVal=%s amd value=%d", qPrintable(strVal), value );

        // enforce min/max values
        if ( value < spinBox->minimum() )
            value = spinBox->minimum();
        else if ( spinBox->maximum() < value )
            value = spinBox->maximum();
        LOG_DEBUG( "updateWrapParam() if spinBox value: %d", value );

        spinBox->setValue( value );
    }

    else if ( (comboBox = qobject_cast<QComboBox*>( table->cellWidget( row, column ) )) )
    {
        LOG_DEBUG( "updateWrapParam() if comboBox: row[%d] x column[%d]", row, column );

        const int index( comboBox->findData( paramArray.at( wrapParam ) ) );
        if ( 0 <= index )
        {
            comboBox->blockSignals( true );
            comboBox->setCurrentIndex( index );
            comboBox->blockSignals( false );
        }
        LOG_DEBUG( "updateWrapParam() if comboBox index: %d", index );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateWrapParamNew(int wrapParam, int paramMin, int paramMax, QVector<QString> &paramArray, hobart::numericLineEdit* paramWidget, QLabel *labelWidget)
{
    LOG_CLASS_INFO( "serviceEditBox", "updateWrapParamNew" );

    int value = 0;
    QString strVal = "";


    strVal = paramArray.at( wrapParam );
    value = paramArray.at( wrapParam ).toInt();

    LOG_DEBUG( "updateWrapParamNew(): at wrapParam=%d for value=%d", wrapParam, value );
    LOG_DEBUG( "updateWrapParamNew() min: %d x max: %d", paramMin, paramMax );

    if ( ( ( WrapParameter::FRONT_UF_ADJUST == wrapParam ) && ( 100 <= value ) ) ||
         ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == wrapParam ) && ( 100 <= value ) ) )
    {
        value -= 100;
        paramArray[wrapParam] = QString::number( value );
        LOG_DEBUG( "updateWrapParamNew() if value UPDATED1 value = %d", qPrintable(strVal), value );
    }

    if ( ( WrapParameter::CLAMP_HEIGHT == wrapParam ) || ( WrapParameter::GRIPPER_ADJUST == wrapParam ) )
    {
        if( ( value > 100 ) && ( value <= 105 ) )
        {
            value = ( ( value - 100 ) * -1 );
            paramArray[wrapParam] = QString::number( value );
            LOG_DEBUG( "updateWrapParamNew() if value UPDATED2 value = %d", qPrintable(strVal), value );
        }
    }

    // enforce min/max values
    if ( value < paramMin )
        value = paramMin;
    else if ( value > paramMax )
        value = paramMax;

    LOG_DEBUG( "updateWrapParamNew() value: %d", value );

    paramWidget->setText( QString::number( value ) );
    labelWidget->setText( QString::number(value) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::validateEntry(int offset, int value )
{
    LOG_CLASS_INFO( "serviceEditBox", "validateEntry" );

    const int oldValue( wrTimingsData_->wrapParameterList[offset]->getData() );

    LOG_DEBUG( "validateEntry() oldValue =  %d,    value = %d", oldValue, value);

    if ( oldValue == value )
        return;

    LOG_DEBUG( "validateEntry() update wrapParameterList[offset][%d]: %d", offset, value);

    wrTimingsData_->wrapParameterList[offset]->setData( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperEditBox::validateField( int oldValue, int newValue, int MinValue, int MaxValue )
{
    LOG_CLASS_INFO( "serviceEditBox", "validateField" );

    LOG_DEBUG( "oldValue = %d, newValue = %d, MinValue = %d, MaxValue = %d", oldValue, newValue, MinValue, MaxValue );

    if ( oldValue != newValue )
    {
#ifdef USE_MIN_MAX
        if ( newValue < MinValue )
            newValue = MinValue;
        else if ( newValue > MaxValue )
            newValue = MaxValue;
#else
        if (( newValue < MinValue ) || ( newValue > MaxValue ))
            newValue = oldValue;
#endif
    }

    LOG_DEBUG( "Returning %d", newValue );

    return newValue;
}

void serviceNewWrapperEditBox::updateWrapperPkgDescription(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "serviceEditBox", "updateWrapperPkgDescription" );

    DomEvolWrapperPkgDesc pkgDescription( msg );

    LOG_DEBUG( "updateWrapperPkgDescription() trayName = %s", qPrintable( pkgDescription.trayName() ) );

    ui->txtActualHeight->setText( pkgDescription.height() );
    ui->txtActualLength->setText( pkgDescription.length() );
    ui->txtActualLip->setText( pkgDescription.lip() );
    ui->txtActualWidth->setText( pkgDescription.width() );

    ui->lblActualLengthTab2->setText( pkgDescription.length() );
    ui->lblActualLengthTab3->setText( pkgDescription.length() );
    ui->lblActualLengthTab4->setText( pkgDescription.length() );
    ui->lblActualLengthTab5->setText( pkgDescription.length() );
    ui->lblActualWidthTab2->setText( pkgDescription.length() );
    ui->lblActualWidthTab3->setText( pkgDescription.length() );
    ui->lblActualWidthTab4->setText( pkgDescription.length() );
    ui->lblActualWidthTab5->setText( pkgDescription.length() );
    ui->lblActualHeightTab2->setText( pkgDescription.length() );
    ui->lblActualHeightTab3->setText( pkgDescription.length() );
    ui->lblActualHeightTab4->setText( pkgDescription.length() );
    ui->lblActualHeightTab5->setText( pkgDescription.length() );
    ui->lblActualLipTab2->setText( pkgDescription.length() );
    ui->lblActualLipTab3->setText( pkgDescription.length() );
    ui->lblActualLipTab4->setText( pkgDescription.length() );
    ui->lblActualLipTab5->setText( pkgDescription.length() );

    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        ui->butSetMin->setEnabled(true);
        ui->butSetMinAndMax->setEnabled(true);
        ui->butSetMax->setEnabled(true);
    }

    LOG_DEBUG( "updateWrapperPkgDescription() pkgDescription.usedWrapBoxBestFit() = %s", qPrintable(pkgDescription.usedWrapBoxBestFit()) );

    static const QString fieldStyleHighlight("background-color: yellow;");
    static const QString fieldStyleNormal("background-color: white;");

    if ( pkgDescription.usedWrapBoxBestFit() == "1")
    {
        ui->txtActualHeight->setStyleSheet( fieldStyleHighlight );
        ui->txtActualLength->setStyleSheet( fieldStyleHighlight );
        ui->txtActualLip->setStyleSheet( fieldStyleHighlight );
        ui->txtActualWidth->setStyleSheet( fieldStyleHighlight );
    }
    else
    {
        ui->txtActualHeight->setStyleSheet( fieldStyleNormal );
        ui->txtActualLength->setStyleSheet( fieldStyleNormal );
        ui->txtActualLip->setStyleSheet( fieldStyleNormal );
        ui->txtActualWidth->setStyleSheet( fieldStyleNormal );
    }

    static const QString spinBoxStyleHighlight("background-color: yellow; font: 20pt \"MS Shell Dlg 2\";");
    static const QString spinBoxStyleNormal("background-color: white; font: 20pt \"MS Shell Dlg 2\";");

    const int wrModel( wrTimingsData_->wrapperModelNum );
    LOG_DEBUG( "updateWrapperPkgDescription() wrModel = %d", wrModel );

    if ( WrapperTimingsData::pmAutoConfigure != wrTimingsData_->pageMode )
    {
        ui->lblTrayFound->setText( (wrTimingsData_->cycledTrayName = pkgDescription.trayName()) );
        wrTimingsData_->updateActualPkg = true;
    }

    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect))
    {
        if (firstPass == true)
        {
            LOG_DEBUG( "updateWrapperPkgDescription() firstPass == true", NULL );
            if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect)
            {
                ui->butCycleWrapperTab1->setEnabled(true);
                ui->butCycleWrapperTab2->setEnabled(true);
                ui->butCycleWrapperTab3->setEnabled(true);
                ui->butCycleWrapperTab4->setEnabled(true);
                ui->butCycleWrapperTab5->setEnabled(true);
                ui->txtTrayName->setEnabled(true);
            }
            else if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
            {
                ui->txtMinLength->setEnabled(true);
                ui->txtMaxLength->setEnabled(true);
                ui->txtMinWidth->setEnabled(true);
                ui->txtMaxWidth->setEnabled(true);
                ui->txtMaxLip->setEnabled(true);
            }
            firstPass = false;
        }
    }

    LOG_DEBUG( "updateWrapperPkgDescription DONE", NULL );
}

void serviceNewWrapperEditBox::processReturnedWrapperErrors(const SessionMessage& msg)
{
    returnedErrorsWrapperDev m( msg );

    if (9709 == m.errorCode())
    {
        bool okPressed( 0 < m.errorData2() );
        if (okPressed)
            continueDeleteSelectedHeight();
    }
}

void serviceNewWrapperEditBox::continueDeleteSelectedHeight()
{
    int theIndex = ui->cboHeightBox->currentIndex();
    QString currHeightStr = ui->cboHeightBox->itemText(theIndex);
    int currHeightVal = (int) (ui->cboHeightBox->itemData(theIndex).toFloat() * 10);

    ui->cboAvailableHeightBoxes->blockSignals(true);
    ui->cboHeightBox->blockSignals(true);

    //  Remove the Height from the current list.
    ui->cboHeightBox->removeItem(theIndex);

    //  Add the Height back to the available list.
    int j = 0;
    int availHeightVal;
    do
    {
        availHeightVal = (int) (ui->cboAvailableHeightBoxes->itemData(j).toFloat() * 10);
        if (availHeightVal < currHeightVal)
            ++j;
        else
            break;
    } while (j < ui->cboAvailableHeightBoxes->count());


    //  Add the Height to the available list.
    ui->cboAvailableHeightBoxes->insertItem(j, currHeightStr, currHeightStr);

    //  Tell the session to remove this height box.
    QString msg2 = QString::number( activeRow ) + "," + currHeightStr;
    session_->send(Wrapper, WrEvolDeleteHeightBox, msg2);

    ui->cboAvailableHeightBoxes->blockSignals(false);
    ui->cboHeightBox->blockSignals(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::autoDetectTray()
{
    session_->send( Wrapper, WrEvolAutoDetectTrayType );          //  9,232
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::wrapperCycleComplete( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceEditBox", "wrapperCycleComplete" );

    DomEvolWrapperTestComplete m( msg );

    LOG_DEBUG( "wrapperCycleComplete() Test number = %s", qPrintable(m.testNum()) );

    if ( 24 == m.testNum().toInt() )
        cycleWrapperInProgress_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::showTablesAndButtons()
{
    LOG_CLASS_INFO( "serviceEditBox", "showTablesAndButtons" );

    ui->butCycleWrapperTab1->show();
    ui->butCycleWrapperTab1->show();
    ui->butCycleWrapperTab2->show();
    ui->butCycleWrapperTab3->show();
    ui->butCycleWrapperTab4->show();
    ui->butCycleWrapperTab5->show();

    LOG_DEBUG( "showTablesAndButtons() isUsingSimplifiedTimings() must be TRUE", NULL );
    // SOF-2550 - added in 2P
    if( session_->scaleConfig->isWrapperTwoPrinter() == false  )
    {
        LOG_DEBUG( "showTablesAndButtons() isWrapperTwoPrinter is FALSE", NULL );
        // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
        ui->butCycleWrapperLeadingTab1->show();
        ui->butCycleWrapperTrailingTab1->show();
        ui->butCycleWrapperLeadingTab2->show();
        ui->butCycleWrapperTrailingTab2->show();
        ui->butCycleWrapperLeadingTab3->show();
        ui->butCycleWrapperTrailingTab3->show();
        ui->butCycleWrapperLeadingTab4->show();
        ui->butCycleWrapperTrailingTab4->show();
        ui->butCycleWrapperLeadingTab5->show();
        ui->butCycleWrapperTrailingTab5->show();
    }

    ui->butSave->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperEditBox::getIntFromString( QString &decimalStr )
{
    int returnVal( 0 );

    if ( decimalStr.length() )
    {
        decimalStr.replace( ".", "" );

        returnVal = decimalStr.toInt();
    }

    return returnVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMinLength()
{
    LOG_CLASS_INFO( "serviceEditBox", "updateMinLength" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    const int valWas( wrapBoxData_.getMinLength() );

    LOG_DEBUG( "updateMinLength() to %s ", qPrintable(ui->txtMinLength->text() ) );

    int valIs( ui->txtMinLength->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let maxVal be equal to the maxLength. It should already be less than or equal to WB_MAX_LENGTH.
    const int maxVal( wrapBoxData_.getMaxLength() - 1 );

    valIs = validateField( valWas, valIs, WB_MIN_LENGTH, maxVal );

    LOG_DEBUG( "updateMinLength(): ui->txtMinLength: %d", valIs );

    ui->txtMinLength->fromInt( valIs );
    wrapBoxData_.setMinLength( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxLength()
{
    LOG_CLASS_INFO( "serviceEditBox", "updateMaxLength" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    const int valWas( wrapBoxData_.getMaxLength() );

    LOG_DEBUG( "serviceEditBox::updateMaxLength() to %s", qPrintable(ui->txtMaxLength->text()) );

    int valIs( ui->txtMaxLength->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let minVal be equal to the minLength. It should already be greater than or equal to WB_MIN_LENGTH.
    const int minVal( wrapBoxData_.getMinLength() + 1 );

    valIs = validateField( valWas, valIs, minVal, WB_MAX_LENGTH );

    LOG_DEBUG( "serviceEditBox::updateMaxLength(): ui->txtMinLength: %d", valIs );

    ui->txtMaxLength->fromInt( valIs );
    wrapBoxData_.setMaxLength( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMinWidth()
{
    //LOG_CLASS_INFO( "serviceEditBox", "updateMinWidth" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    const int valWas( wrapBoxData_.getMinWidth() );

#ifdef USE_QDEBUG
    qDebug() << "serviceEditBox::updateMinWidth() to " << ui->txtMinWidth->text();
#endif

    int valIs( ui->txtMinWidth->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let maxVal be equal to the maxWidth. It should already be less than or equal to WB_MAX_WIDTH.
    const int maxVal( wrapBoxData_.getMaxWidth() - 1 );

    valIs = validateField( valWas, valIs, WB_MIN_WIDTH, maxVal );

#ifdef USE_QDEBUG
    qDebug() << "serviceEditBox::updateMinWidth():  ui->txtMinWidth->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->txtMinWidth->fromInt( valIs );
    wrapBoxData_.setMinWidth( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxWidth()
{
    //LOG_CLASS_INFO( "serviceEditBox", "updateMaxWidth" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    const int valWas( wrapBoxData_.getMaxWidth() );

#ifdef USE_QDEBUG
    qDebug() << "serviceEditBox::updateMaxWidth() to " << ui->txtMaxWidth->text();
#endif
    int valIs( ui->txtMaxWidth->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let minVal be equal to the minWidth. It should already be greater than or equal to WB_MIN_WIDTH.
    const int minVal( wrapBoxData_.getMinWidth() + 1 );

    valIs = validateField( valWas, valIs, minVal, WB_MAX_WIDTH );

#ifdef USE_QDEBUG
    qDebug() << "serviceEditBox::updateMaxWidth():  ui->txtMaxWidth->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->txtMaxWidth->fromInt( valIs );
    wrapBoxData_.setMaxWidth( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxLip()
{
    LOG_CLASS_INFO( "serviceEditBox", "updateMaxLip" );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    const int valWas( wrapBoxData_.getMaxLip() );

    LOG_DEBUG("serviceEditBox::updateMaxLip() to %s", qPrintable(ui->txtMaxLip->text() ) );

    int valIs( ui->txtMaxLip->toInt() );

    if ( valIs == valWas )
        return;

    valIs = validateField( valWas, valIs, WB_MIN_LIP, WB_MAX_LIP );

    LOG_DEBUG("updateMaxLip() to %d", valIs);

    ui->txtMaxLip->fromInt(valIs);
    wrapBoxData_.setMaxLip(valIs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::hideShowForNewTray( bool hideThem )
{
    LOG_CLASS_INFO( "serviceEditBox", "hideShowForNewTray" );

    LOG_DEBUG("onSessionMessage() Hide a bunch of stuff for new tray creation", NULL);

    ui->lblTitleMin->setVisible( !hideThem );
    ui->lblTitleActual->setVisible( !hideThem );
    ui->lblTitleMax->setVisible( !hideThem );

    ui->lblLength->setVisible( !hideThem );
    ui->txtMinLength->setVisible( !hideThem );
    ui->txtActualLength->setVisible( !hideThem );
    ui->txtMaxLength->setVisible( !hideThem );

    ui->lblWidth->setVisible( !hideThem );
    ui->txtMinWidth->setVisible( !hideThem );
    ui->txtActualWidth->setVisible( !hideThem );
    ui->txtMaxWidth->setVisible( !hideThem );

    ui->lblLip->setVisible( !hideThem );
    ui->txtActualLip->setVisible( !hideThem );
    ui->txtMaxLip->setVisible( !hideThem );

    ui->lblHeight->setVisible( !hideThem );
    ui->txtActualHeight->setVisible( !hideThem );

    ui->butAutoConfig->setVisible( !hideThem );

    ui->lblPromptTrayName->setVisible( hideThem );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceEditBox", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    LOG_DEBUG( "onSessionMessage() device = %d,  event = %d", device, event );

    if (( Wrapper == device ) && ( WrEvolWriteWrapBox == event ))
    {
        // Received 9,241 - Update the Basic timings info / Height box info for this tray
        buildWrapDimensions( msg );
    }
    else if (( Wrapper == device ) && ( WrEvolWriteHeightBox == event ))
    {
        // Received 9,247 - Update the Basic timings info / Height box info for this tray
        setupHeightBox( msg );
    }
    else if (( Wrapper == device ) && ( WrEvolPackageDesc == event ))
    {
        // Received 9,125 - Update the Basic timings info / Height box info for this tray
        updateWrapperPkgDescription( msg );
    }
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ))
    {
        // Received 9,165 - Test complete, or for these pages, the Wrapper cycle is complete
        wrapperCycleComplete( msg );
    }
    else if (( Error == device ) && ( Wrapper == event ))
    {
        processReturnedWrapperErrors( msg );
    }
    else if ((Printer == msg[0].toInt() ) && ( PrStockMetrics == msg[1].toInt() ))
    {
        LOG_DEBUG("onSessionMessage() PrStockMetrics received", NULL);
        DomPrinterStockMetrics m( msg );

        if( m.printerType() == 0 )
        {
            pLabelSize_ = m.stockLengthInLinesRaw();
            session_->send( Printer, PrReqStockMetrics, QString::number( 1 ) );             // 7,40,secondPrinter
        }

        if( m.printerType() == 1 )
        {
            sLabelSize_ = m.stockLengthInLinesRaw();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::isDualRollWrapper() const
{
     LOG_CLASS_INFO( "serviceEditBox", "isDualRollWrapper" );

    if( ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_RL ) ||   // 20 SOF-2926
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_LR ) )    // 21 SOF-2926
        {
            LOG_DEBUG("isDualRollWrapper() isDualRollWrapper IS TRUE model: %d",  wrTimingsData_->wrapperModelNum);
            return true;
        }

    LOG_DEBUG("isDualRollWrapper() isDualRollWrapper IS FALSE model: %d", wrTimingsData_->wrapperModelNum );
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onComboBoxChanged(int index)
{
    LOG_CLASS_INFO( "serviceEditBox", "onComboBoxChanged" );

    // TODO: update any vars with index if needed

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    //index
    int indexData = ui->param14->currentData().toInt();
    LOG_DEBUG("onComboBoxChanged() data for combo (0=100, 1=200): %d", indexData );

    wrTimingsData_->wrapParameterList[WrapParameter::FILM_ROLL_TO_USE]->setData( indexData );

    LOG_DEBUG("onComboBoxChanged() new index for file: %d", index );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onStepDown()
{
    LOG_CLASS_INFO( "serviceEditBox", "onStepDown" );
    LOG_DEBUG("onStepDown() START", NULL );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    QPushButton* paramWidget = qobject_cast<QPushButton*>(sender());

    if( ui->butLeftParam3 == paramWidget )
    {
        int value = ui->param3->text().toInt();
        LOG_DEBUG("onStepDown() param3 with value: %d", value );
        if( value > HB_MIN_10 )
        {
            value = value - 1;
            ui->param3->setText( QString::number( value ) );
            validateEntry( WrapParameter::DISTANCE, value );
        }
    }
    else if( ui->butLeftParam0 == paramWidget )
    {
        int value = ui->param0->text().toInt();
        LOG_DEBUG("onStepDown() param0 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param0->setText( QString::number( value ) );
            validateEntry( WrapParameter::PERCENT_STRETCH_CLAMP_ON, value );
        }
    }
    else if( ui->butLeftParam2 == paramWidget )
    {
        int value = ui->param2->text().toInt();
        LOG_DEBUG("onStepDown() param2 with value: %d", value );
        if( value > HB_MIN_NEG_05 )
        {
            value = value - 1;
            ui->param2->setText( QString::number( value ) );
            validateEntry( WrapParameter::CLAMP_HEIGHT, value );
        }
    }
    else if( ui->butLeftParam9 == paramWidget )
    {
        int value = ui->param9->text().toInt();
        LOG_DEBUG("onStepDown() param9 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param9->setText( QString::number( value ) );
            validateEntry( WrapParameter::SIDE_UF_ADJUST, value );
        }
    }
    else if( ui->butLeftParam5 == paramWidget )
    {
        int value = ui->param5->text().toInt();
        LOG_DEBUG("onStepDown() param5 with value: %d", value );
        if( value > HB_MIN_NEG_05 )
        {
            value = value - 1;
            ui->param5->setText( QString::number( value ) );
            validateEntry( WrapParameter::GRIPPER_ADJUST, value );
        }
    }
    else if( ui->butLeftParam15 == paramWidget )
    {
        int value = ui->param15->text().toInt();
        LOG_DEBUG("onStepDown() param15 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param15->setText( QString::number( value ) );
            validateEntry( WrapParameter::REAR_UF_TRIP_DELAY, value );
        }
    }
    else if( ui->butLeftParam10 == paramWidget )
    {
        int value = ui->param10->text().toInt();
        LOG_DEBUG("onStepDown() param10 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param10->setText( QString::number( value ) );
            validateEntry( WrapParameter::HEIGHT_RESTRICT, value );
        }
    }
    else if( ui->butLeftParam16 == paramWidget )
    {
        int value = ui->param16->text().toInt();
        LOG_DEBUG("onStepDown() param16 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param16->setText( QString::number( value ) );
            validateEntry( WrapParameter::BRAKE_TIMING, value );
        }
    }
    else if( ui->butLeftParam7 == paramWidget )
    {
        int value = ui->param7->text().toInt();
        LOG_DEBUG("onStepDown() param7 with value: %d", value );
        if( value > HB_MIN_12 )
        {
            value = value - 1;
            ui->param7->setText( QString::number( value ) );
            validateEntry( WrapParameter::WRAP_SPEED, value );
        }
    }
    else if( ui->butLeftParam8 == paramWidget )
    {
        int value = ui->param8->text().toInt();
        LOG_DEBUG("onStepDown() param8 with value: %d", value );
        if( value > HB_MIN_01 )
        {
            value = value - 1;
            ui->param8->setText( QString::number( value ) );
            validateEntry( WrapParameter::DISCHARGE_SPEED, value );
        }
    }
    else if( ui->butLeftParam6 == paramWidget )
    {
        int value = ui->param6->text().toInt();
        LOG_DEBUG("onStepDown() param6 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param6->setText( QString::number( value ) );
            validateEntry( WrapParameter::FRONT_UF_ADJUST, value );
        }
    }
    else if( ui->butLeftParam4 == paramWidget )
    {
        int value = ui->param4->text().toInt();
        LOG_DEBUG("onStepDown() param4 with value: %d", value );
        if( value > HB_MIN_00 )
        {
            value = value - 1;
            ui->param4->setText( QString::number( value ) );
            validateEntry( WrapParameter::FORCE_OUTSIDE_ELEVATOR, value );
        }
    }
    else if( ui->butLeftParam20 == paramWidget )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
	        int value = ui->param20->text().toInt();
	        LOG_DEBUG("onStepDown() param20 with value: %d", value );
	        if( value > HB_MIN_00 )
	        {
	            value = value - 1;
	            ui->param20->setText( QString::number( value ) );
	            validateEntry( WrapParameter::SECONDARY_LABEL_VERT_OFFSET, value );
	        }
        }
    }

    LOG_DEBUG("onStepDown() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onStepUp()
{
    LOG_CLASS_INFO( "serviceEditBox", "onStepUp" );
    LOG_DEBUG("onStepUp() START", NULL );

    hasChanged_ = true;
    if( finishedState == DONE_STATE )
    {
        finishedState = CANCEL_STATE;
        ui->butCancelDone->setText( tr("Cancel"));
    }

    QPushButton* paramWidget = qobject_cast<QPushButton*>(sender());

    if( ui->butRightParam3 == paramWidget )
    {
        int value = ui->param3->text().toInt();
        LOG_DEBUG("onStepUp() param3 with value: %d", value );
        if( value < HB_MAX_40 )
        {
            value = value + 1;
            ui->param3->setText( QString::number( value ) );
            validateEntry( WrapParameter::DISTANCE, value );
        }
    }
    else if( ui->butRightParam0 == paramWidget )
    {
        int value = ui->param0->text().toInt();
        LOG_DEBUG("onStepUp() param0 with value: %d", value );
        if( value < HB_MAX_60 )
        {
            value = value + 1;
            ui->param0->setText( QString::number( value ) );
            validateEntry( WrapParameter::PERCENT_STRETCH_CLAMP_ON, value );
        }
    }
    else if( ui->butRightParam2 == paramWidget )
    {
        int value = ui->param2->text().toInt();
        LOG_DEBUG("onStepUp() param2 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param2->setText( QString::number( value ) );
            validateEntry( WrapParameter::CLAMP_HEIGHT, value );
        }
    }
    else if( ui->butRightParam9 == paramWidget )
    {
        int value = ui->param9->text().toInt();
        LOG_DEBUG("onStepUp() param9 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param9->setText( QString::number( value ) );
            validateEntry( WrapParameter::SIDE_UF_ADJUST, value );
        }
    }
    else if( ui->butRightParam5 == paramWidget )
    {
        int value = ui->param5->text().toInt();
        LOG_DEBUG("onStepUp() param5 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param5->setText( QString::number( value ) );
            validateEntry( WrapParameter::GRIPPER_ADJUST, value );
        }
    }
    else if( ui->butRightParam15 == paramWidget )
    {
        int value = ui->param15->text().toInt();
        LOG_DEBUG("onStepUp() param15 with value: %d", value );
        if( value < HB_MAX_05 )
        {
            value = value + 1;
            ui->param15->setText( QString::number( value ) );
            validateEntry( WrapParameter::REAR_UF_TRIP_DELAY, value );
        }
    }
    else if( ui->butRightParam10 == paramWidget )
    {
        int value = ui->param10->text().toInt();
        LOG_DEBUG("onStepUp() param10 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param10->setText( QString::number( value ) );
            validateEntry( WrapParameter::HEIGHT_RESTRICT, value );
        }
    }
    else if( ui->butRightParam16 == paramWidget )
    {
        int value = ui->param16->text().toInt();
        LOG_DEBUG("onStepUp() param16 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param16->setText( QString::number( value ) );
            validateEntry( WrapParameter::BRAKE_TIMING, value );
        }
    }
    else if( ui->butRightParam7 ==  paramWidget )
    {
        int value = ui->param7->text().toInt();
        LOG_DEBUG("onStepUp() param7 with value: %d", value );
        if( value < HB_MAX_30 )
        {
            value = value + 1;
            ui->param7->setText( QString::number( value ) );
            validateEntry( WrapParameter::WRAP_SPEED, value );
        }
    }
    else if( ui->butRightParam8 ==  paramWidget )
    {
        int value = ui->param8->text().toInt();
        LOG_DEBUG("onStepUp() param8 with value: %d", value );
        if( value < HB_MAX_30 )
        {
            value = value + 1;
            ui->param8->setText( QString::number( value ) );
            validateEntry( WrapParameter::DISCHARGE_SPEED, value );
        }
    }
    else if( ui->butRightParam6 ==  paramWidget )
    {
        int value = ui->param6->text().toInt();
        LOG_DEBUG("onStepUp() param6 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param6->setText( QString::number( value ) );
            validateEntry( WrapParameter::FRONT_UF_ADJUST, value );
        }
    }
    else if( ui->butRightParam4 ==  paramWidget )
    {
        int value = ui->param4->text().toInt();
        LOG_DEBUG("onStepUp() param4 with value: %d", value );
        if( value < HB_MAX_10 )
        {
            value = value + 1;
            ui->param4->setText( QString::number( value ) );
            validateEntry( WrapParameter::FORCE_OUTSIDE_ELEVATOR, value );
        }
    }
    else if( ui->butRightParam20 ==  paramWidget )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
	        int value = ui->param20->text().toInt();
	        LOG_DEBUG("onStepUp() param20 with value: %d", value );
	        if( value < HB_MAX_10 )
	        {
	            value = value + 1;
	            ui->param20->setText( QString::number( value ) );
	            validateEntry( WrapParameter::SECONDARY_LABEL_VERT_OFFSET, value );
	        }
        }
    }

    LOG_DEBUG("onStepUp() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::printTimings()
{
    LOG_CLASS_INFO( "serviceEditBox", "printTimings" );

    // Send message to generate the Summary Report
    QString title( "TRAY: " + ui->txtTrayName->text() );
    QString msg;

    msg += "Length MIN:  ";
    msg += ui->txtMinLength->text();
    msg += "\n";

    msg += "Length MAX:  ";
    msg += ui->txtMaxLength->text();
    msg += "\n";

    msg += "\n";

    msg += "Width MIN:  ";
    msg += ui->txtMinWidth->text();
    msg += "\n";

    msg += "Width MAX:  ";
    msg += ui->txtMaxWidth->text();
    msg += "\n";

    msg += "\n";

    msg += "Lip MAX:  ";
    msg += ui->txtMaxLip->text();
    msg += "\n";

    msg += "\n";

    msg += "Height MIN:  ";
    msg += ui->txtMinHeight->text();
    msg += "\n";

    msg += "Height MAX:  ";
    msg += ui->txtMaxHeight->text();
    msg += "\n";

    msg += "\n";

    msg += "HEIGHT BOX ";
    msg += ui->cboHeightBox->currentText();
    msg += " TIMINGS:";
    msg += "\n";

    LOG_DEBUG("printTimings() msg 1: %s", qPrintable(msg));

    //----- TAB 2
    msg += "Side to Side Pre-Stretch:  ";
    msg += ui->param0->text();
    msg += "\n";

    msg += "Side to Side Wrap Tension:  ";
    msg += ui->param2->text();
    msg += "\n";

    msg += "Side to Side Pre-Stretch:  ";
    msg += ui->param9->text();
    msg += "\n";

    //----- TAB 3
    msg += "Front to Back Wrap Tension:  ";
    msg +=ui->param5->text();
    msg += "\n";

    msg += "Rear Underfolder Travel:  ";
    msg += ui->param15->text();
    msg += "\n";

    msg += "Gripper Relax:  ";
    msg += ui->param10->text();
    msg += "\n";

    msg += "Package Position on Elevator:  ";
    msg += ui->param3->text();
    msg += "\n";

    //----- TAB 4
    msg += "Primary Label Offset:  ";
    msg +=  ui->param6->text();
    msg += "\n";

    msg += "Secondary Label Offset:  ";
    msg += ui->param4->text();
    msg += "\n";

    msg += "Secondary Label Vertical Offsets:  ";
    msg += ui->param20->text();
    msg += "\n";

    // TAB 5
    msg += "Wrap Speed:  ";
    msg += ui->param7->text();
    msg += "\n";

    msg += "Discharge Speed:  ";
    msg += ui->param8->text();
    msg += "\n";

    msg += "Film Length Pull:  ";
    msg += ui->param16->text();
    msg += "\n";

    msg += "Film Width to Use:  ";
    int filmWidth(  ui->param14->currentIndex() );

    if( filmWidth == 0 )
        msg += "Narrow Film";
    else
        msg += "Wide Film";
    msg += "\n";

    // TODO
    // LEGACY - I will have to come up with my own print routine - dont delete this I hopefully can use idea of it
    // Need the order of the original table, that I no longer have, so I can build message and send off
    // LEGACY -
    /*
    for ( int i( 0 ); i < ui->tblBasicTimingsList->rowCount(); i++ )
    {
        QSpinBox *spinBox;
        QComboBox *comboBox;

        if( ui->tblBasicTimingsList->item( i, 0 ) )
        {
            msg += ui->tblBasicTimingsList->item( i, 0 )->text();
            msg += ":  ";

            LOG_DEBUG( "printTimings() isUsingSimplefiedTimings() is TRUE so access cellWidget 2", NULL );
            QWidget *w( ui->tblBasicTimingsList->cellWidget( i, 2 ));

            if ( (spinBox = qobject_cast<QSpinBox*>( w )) )
            {
                msg += QString::number( spinBox->value() );
                LOG_DEBUG( "printTimings() widget is spinBox, name = %s and value = %s", qPrintable( spinBox->objectName() ), qPrintable( spinBox->value() ) );
            }
            else if ( (comboBox = qobject_cast<QComboBox*>( w )) )
            {
                msg += comboBox->currentText();
                LOG_DEBUG( "printTimings() widget is spinBox, name = %s and value = %s", qPrintable( comboBox->objectName() ), qPrintable( comboBox->currentText() ) );
            }

            msg += "\n";
        }
    }
    */

    LOG_DEBUG("printTimings() msg 2: %s", qPrintable(msg));

    session_->send(ReportManagerDev, StartReport, "2,0,0,0,0,\"" + title + "\",\"" + msg + "\"" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::promptForcedTraySize()
{
    WrapperForcedTraySizeDialog d( mainWindow_ );

    if ( d.traySizesExist() > 0 )
        d.exec();
    else
    {
        MessageBoxDialog dialog( mainWindow_ );
        dialog.setText( tr( "No trays exist within the flashkey group. Please add trays to the group and try again." ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();
    }
}
