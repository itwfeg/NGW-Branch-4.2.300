/**
 * @file cuttingTestInitialInformation.cpp
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * @{
 *
 */

#include <logmanager.h>

#include "cuttingTestInitialInformation.h"
#include "ui_cuttingTestInitialInformation.h"

#include "../sessionmainwindow.h"
#include "../modes/operatemode.h"
#include <softkeyswidget.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>
#include <QVector>
#include <QStringList>
#include <softkeyswidget.h>

CuttingTestInitialInformation::CuttingTestInitialInformation(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::CuttingTestInitialInformation),
    testnum_( 0 ),
    plu_num( 0 ),
    beginOrEndTest_( 0 ),
    testActive_( true ),
    canceledTest_( false ),
    imOuttaHere( false )
{
    ui->setupUi(this);
    
    ui->testNumber->setText( QString::number(testnum_) );

    // check if in operate mode
    if ( (operateMode_ = dynamic_cast<OperateMode*>( parent )) )
    {
        // connect slots
        connect( operateMode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
    }

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CuttingTestInitialInformation::~CuttingTestInitialInformation()
{
    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    if( ( beginOrEndTest_ == CutTestState::CutTestState_Begin ) && ( startedTest_ == false ) && ( suspendedTest_ == false ) && ( canceledTest_ == false ) && ( continueTest_ == false ) )
    {
        // then we popped up the dialog but we clicked off of it
        // (clicked the background) and didnt start the test. So
        // we must suspend this one we half started and roll on.

        onSuspendButtonClicked();
    }

    // turn dialogs back on for cut test
    if ( scaleConfig->setAreDialogsTurnedOff( false ) )
        session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

void CuttingTestInitialInformation::translate()
{
    ui->retranslateUi(this);

    QLocale::Language lang = QLocale::English;
    if("EN.US" == mainWindow_->language())
    {
        lang = QLocale::English;
    }
    else if("EN.CA" == mainWindow_->language()) //Could be different than English
    {
        lang = QLocale::English;
    }
    else if("ES.MX" == mainWindow_->language()) // Spanish
    {
        lang = QLocale::Spanish;
    }
    else if("FR.CA" == mainWindow_->language()) // French
    {
        lang = QLocale::French;
    }

    if( beginOrEndTest_ == CutTestState::CutTestState_End )
        ui->lblInstructions->setText( tr("Please enter any final comments") );
    else
        ui->lblInstructions->setText( tr("Please enter any additional comments") );

    ui->packingDateCalendar->setLocale( lang );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestInitialInformation::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestInitialInformation::onScreenChanging()
{
    imOuttaHere = true;

    if( ( beginOrEndTest_ == CutTestState::CutTestState_Begin ) && ( startedTest_ == false ) && ( suspendedTest_ == false ) && ( canceledTest_ == false ) && ( continueTest_ == false ) )
    {
        // then we popped up the dialog but we clicked off of it
        // (clicked the background) and didnt start the test. So
        // we must suspend this one we half started and roll on.

        // Walmart Mexico requested that instead of suspending the cut test when we hit the home menu, but CANCEL instead
        // onSuspendButtonClicked();

        onCancelCutTestClicked();
    }

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onScreenChanged()
{
    // only execute code when we are active screen (skip when first coming in)
    if ( mode_->screen() != this )
    {
        return;
    }

    // hide any weight bar (do this when we initially come into the screen)
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}
void CuttingTestInitialInformation::initialize()
{
    if ( session_->scaleConfig->setAreDialogsTurnedOff( true ) )
        session_->send( SessionDev, SnStopDialogs );

    ///< create some pointer to session data
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );
    DomWeigherConfigRead *weigherConfig ( &session_->scaleConfig->weigherConfigRead );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

    ignoreSignals_ = true;

    ///< initialize validators
    prodNumValidator_      = NULL;                                  // initialize all of the validators to null
    supplierValidator_     = NULL;
    lotNumValidator_       = NULL;
    prodCostValidator_     = NULL;
    prodWtValidator_       = NULL;
    packingHoursValidator_ = NULL;
    packingMinsValidator_  = NULL;

    ///< initialize variables
    isPluValid_ = false;
    startedTest_ = false;
    suspendedTest_ = false;
    continueTest_ = false;

    saveddate_ = "";
    displaydate_ = "";
    displayhour_ = "";
    displaymins_ = "";
    comments_ = "";
    finalcomments_ = "";

    ui->lblMessages->setText("");                                   // initially clear all messages
    ui->packingDateCalendar->setVisible( false );                   // intially hide the calendar

    ///< clear out the fields in the primal plu
    /// we can only do this to begin bc on an END, we may return and not end
    if ( beginOrEndTest_ == CutTestState::CutTestState_Begin && !scaleConfiguration->resumeIncompleteTest )
    {
        cutTestPlu->pluNumber     = 0;
        cutTestPlu->meatSpecSheet = 0;
        cutTestPlu->cutTestNumber = 0;
    }

    comments_           = "";                                       // holds the string for additional comments
    finalcomments_      = "";                                       // holds the string for final comments

    ///< set up the SIGNALS and SLOTS
    connect(ui->productNumber,       SIGNAL(editingFinished()), this,       SLOT( onProductNumberFinished()));
    connect(ui->startButton,         SIGNAL( clicked() ),       this,       SLOT( onStartButtonClicked()   ) );         // clicked Start Cut Test
    connect(ui->suspendButton,       SIGNAL( clicked() ),       this,       SLOT( onSuspendButtonClicked() ) );         // clicked Put Cut Test on Hold
    connect(ui->commentButton,       SIGNAL( clicked() ),       this,       SLOT( onCommentButtonClicked() ) );         // clicked Additional Comments
    connect(ui->cancelCutTestButton, SIGNAL( clicked() ),       this,       SLOT( onCancelCutTestClicked() ) );         // clicked Don't Start Cut Test

    connect(ui->saveCommentsButton,  SIGNAL( clicked()  ),      this,       SLOT( onCutTestSaveComments()));            // clicked Save Comments
    connect(ui->skipCommentsButton,  SIGNAL( clicked()  ),      this,       SLOT( onCutTestCancelComments()));          // clicked Cancel Comments
    connect(ui->returnToTestButton,  SIGNAL( clicked()  ),      this,       SLOT( onReturnToCutTest()));                // clicked Return To Cut Test

    connect(ui->packingDate,         SIGNAL( clicked()  ),      this,       SLOT( onPackingDateClicked()));
    connect(ui->packingDateCalendar, SIGNAL( clicked(QDate)),   this,       SLOT( onCalendarClicked(QDate) ));
    connect( operateMode_,           SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );

    ///< Set up validators
    ///< set up product number 6 digit numeric
    ui->productNumber->setValidator(prodNumValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->productNumber->setDecimalPlaces(0);
    ui->productNumber->setInputWrapping( true );
    ui->productNumber->setInputTruncation( true );
    prodNumValidator_->setTop( makeValidatorTop(6) );

    ///< set up supplier number 10 digit alpha numeric (original cut test limit was 9999999)
    QRegExp supplierRegEx("^([A-Za-z0-9]){0,10}$");
    supplierValidator_ = new QRegExpValidator(supplierRegEx, this);
    ui->supplierNumber->setValidator(supplierValidator_);

    ///< set up lot number 10 digit alpha numeric (SRD indicates 9999999999. That makes no sense)
    QRegExp lotRegEx("^([A-Za-z0-9]){0,10}$");
    lotNumValidator_ = new QRegExpValidator(lotRegEx, this);
    ui->lotNumber->setValidator(lotNumValidator_);

    ///< product cost was supposed to be 6 max digits with 2 digit precision. See Appendix C.
    ///< Instead I used the currency settings in Service which the first cutting test also implemented.
    ///< So max number can be either 9999.99 or 9999.999 depending on primary currency precision.
    ///< Also note in the UI there is no way to enter secondary precisions even thou current cutting test uses them.

    ///< set up product cost validator
    ui->productCost->setValidator( prodCostValidator_ = new QIntValidator( 0, INT_MAX, this ) );
    ui->productCost->setDecimal( currencySettings->unitSeparator() );
    ui->productCost->setDecimalPlaces( 2 );
    ui->productCost->setInputWrapping( true );    // whats this? setInputTruncation( true )
    ui->productCost->setPrefix( currencySettings->priCurrSymbol() );
    prodCostValidator_->setTop( makeValidatorTop(6) );        // document specified


    int wayerModel =  weigherConfig->weigherModel();
    if( CutTestState::CutTestState_None )
        wayerModel = SessionMainWindow::HLX1;                     // JHC Cut Test - my laptop doesn't have a weigher

    int num_decs = 2;
    if( wayerModel == SessionMainWindow::HLX1 || wayerModel == SessionMainWindow::AWS ||
        wayerModel == SessionMainWindow::SASCALE2 || wayerModel == SessionMainWindow::HTM )
        num_decs = 3;

    ///< set up product weight validator
    ui->productWeight->setValidator(prodWtValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->productWeight->setDecimal( currencySettings->unitSeparator() );
    ui->productWeight->setDecimalPlaces( num_decs );
    ui->productWeight->setInputWrapping( true );
    ui->productWeight->setSuffix( scaleConfig->isScaleMetric()? " kg" : " lb" );
    prodWtValidator_->setTop( makeValidatorTop(5) );      // document specified

    ///< set up packing hours validator
    ui->packingHours->setValidator(packingHoursValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->packingHours->setDecimalPlaces(0);
    ui->packingHours->setInputWrapping(true);
    packingHoursValidator_->setTop(23);
    packingHoursValidator_->setBottom(0);

    ///< set up packing minutes validator
    ui->packingMinutes->setValidator(packingMinsValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->packingMinutes->setDecimalPlaces(0);
    ui->packingMinutes->setInputLeadingZeros(true);
    ui->packingMinutes->setInputWrapping(true);
    ui->packingMinutes->setInputTruncation(true);
    packingMinsValidator_->setTop(59);
    packingMinsValidator_->setBottom(0);

    ///< Did not set up a validator for the calendar date

    ///< Set up the initial packing date and time to the scale's current date and time
    QDateTime now( session_->scaleConfig->currentDate(), session_->scaleConfig->currentTime() );  // QString now_what = now.toString();      // used for testing
    QDate today  = now.date();
    QTime time   = now.time();

    int intday   = today.dayOfWeek();
    int intmon   = today.month();

    QString hour    = QString::number( time.hour() );
    QString mins    = QString::number( time.minute() );
    QString date    = QString::number( today.day() );
    QString year    = QString::number( today.year() );
    QString weekday = QDate::shortDayName( intday );
    QString month   = QDate::shortMonthName( intmon );

    QString thedate = weekday + " " + month + " " + date + " " + year;
    QDate mydate      = QDate::fromString(thedate);
    QString mydatenow = mydate.toString("dd/MMMM/yyyy");
    saveddate_        = mydate.toString("yy/MM/dd");
    displaydate_ = mydatenow;
    displayhour_ = hour;
    displaymins_ = mins;

    translate();
    //ui->productNumber->setText("");
    ui->packingDate->setText( displaydate_ );                  // after we get a valid PLU update the date time
    ui->packingHours->setText( displayhour_ );
    ui->packingMinutes->setText( displaymins_ );

    ignoreSignals_ = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::updateInformation(int testnum, int testState)
{
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    testnum_ = testnum;
    beginOrEndTest_ = testState;

    ///< hide something real quick we dont want if starting a test. We only want this button when we are
    /// in a cut test, and we click the Finish Cut Test Flashkey, and we need to add final comments
    /// but we made a mistake and want to continue the test instead of exiting the cut test.
    if( beginOrEndTest_ == CutTestState::CutTestState_Begin )
        ui->returnToTestButton->setVisible( false );
    else
        ui->returnToTestButton->setVisible( true );

    displayInitialState();

    if( CutTestState::CutTestState_Begin == beginOrEndTest_ )
        ui->testNumber->setText( QString::number( testnum ));

    if( CutTestState::CutTestState_Begin == beginOrEndTest_ && scaleConfiguration->resumeIncompleteTest )
    {
        // if this is the case, we may have, or not, some or partial information to update

        DomCuttingTest *currentCuttingTest( &scaleConfiguration->cuttingTestWrite );

        if( currentCuttingTest->productNum().length() > 0  )
            ui->productNumber->setText( currentCuttingTest->productNum() );
        if( currentCuttingTest->primalCost().length() > 0 )
            ui->productCost->setText( currentCuttingTest->primalCost() );
        if( currentCuttingTest->primalWeight().length() > 0 )
            ui->productWeight->setText( currentCuttingTest->primalWeight() );
        if( currentCuttingTest->vendorNum().length() > 0 )
            ui->supplierNumber->setText( currentCuttingTest->vendorNum() );
        if( currentCuttingTest->lotNumber().length() > 0 )
            ui->lotNumber->setText( currentCuttingTest->lotNumber() );
        if( currentCuttingTest->packingDate().length() > 0 )
            ui->packingDate->setText( currentCuttingTest->packingDate() );
        if( currentCuttingTest->packingHours().length() > 0 )
            ui->packingHours->setText( currentCuttingTest->packingHours() );
        if( currentCuttingTest->packingMinutes().length() > 0 )
            ui->packingMinutes->setText( currentCuttingTest->packingMinutes() );
    }

    scaleConfiguration->resumeIncompleteTest = false;       // need to reset this for next time

    int tmp_plu = ui->productNumber->text().toInt();
    if(  tmp_plu > 0 )
        onProductNumberFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onProductNumberFinished()
{
    if( ignoreSignals_ )
        return;

    if(  ui->productNumber->text().isEmpty() )
        return;

    int tmp_plu = ui->productNumber->text().toInt();

    if( (tmp_plu > 0) && (tmp_plu != plu_num) && (suspendedTest_ == false) && (imOuttaHere == false) )
    {
        onLookupPluNumber();
        plu_num = ui->productNumber->text().toInt();
    }
    else if( tmp_plu == 0 )
    {
        ui->productNumber->setText("");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *CuttingTestInitialInformation::currentProduct() const
{
    DomCurrentPLU *product = (operateMode_ ? operateMode_->currentProduct() : NULL);

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onCurrentProductChanged( bool nof )
{ 
    if ( nof )
    {
        ui->lblMessages->setText( tr("Invalid product number. The entered product does not exist.") );
        isPluValid_ = false;
        return;
    }
    else if ( !operateMode_ )
        return;

    if ( beginOrEndTest_ == CutTestState::CutTestState_End )
        return;

    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    DomCurrentPLU *product( currentProduct() );

    if( !product )
    {
        ui->lblMessages->setText( tr("Invalid product number. The entered product does not exist.") );
        isPluValid_ = false;
        return;
    }

    if( product->itemNumber().toInt() > 0 )
    {
        if( scaleConfig->enableMeatSpecSheetRequired() )
        {
            if( product->meatSpecificationSheet().toInt() > 0)
            {
                isPluValid_ = true;
                ui->lblMessages->setText( tr("Valid product. The product exists and contains a Meat Specification Sheet.") );
            }
            else
            {
                isPluValid_ = false;
                ui->productNumber->setText("");
                //ui->productNumber->setFocus();
                ui->lblMessages->setText( tr("Invalid Spec Sheet. The product exists but does not contains a Meat Specification Sheet.") );
            }
        }
        else
        {
            ui->lblMessages->setText( tr("Valid product. The entered product exists.") );
            isPluValid_ = true;
        }
    }
    else
    {
        isPluValid_ = false;
        ui->productNumber->setText("");
        //ui->productNumber->setFocus();
        ui->lblMessages->setText( tr("Invalid product number. The entered product does not exist.") );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onPackingDateClicked()
{
    // when the user clicked the packing date field, immediately pop the calendar so they can select a date
    mainWindow_->hideKeyboard();
    ui->packingDateCalendar->setVisible( true );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestInitialInformation::validData()
{
    // on a Save or Suspend, tell the user if any of the entered data is junk

    if( !isPluValid_ )
    {
        ui->lblMessages->setText( tr("The Product Number field contains invalid data.") );
        return false;
    }
    else if( ui->productNumber->text().isEmpty() || ui->productNumber->text() == "0"  )
    {
        ui->lblMessages->setText( tr("The Product Number field contains invalid data.") );
        return false;
    }
    else if( ui->productCost->text().isEmpty() || ui->productCost->text() == "$0.00" )
    {
        ui->lblMessages->setText( tr("The Product Cost field contains invalid data.") );
        return false;
    }
    else if( ui->productWeight->text().isEmpty() || ui->productWeight->text() == "0.00 lb" || ui->productWeight->text() == "0.000 lb" || ui->productWeight->text() == "0.000 kg" )
    {
        ui->lblMessages->setText( tr("The Product Weight field contains invalid data.") );
        return false;
    }
    else if ( ui->packingHours->text().isEmpty() )
    {
        ui->lblMessages->setText( tr("The Packing Hours field contains invalid data.") );
        return false;
    }
    else if ( ui->packingMinutes->text().isEmpty() )
    {
        ui->lblMessages->setText( tr("The Packing Minutes field contains invalid data.") );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::inactivityLogout()
{
    //onCancelCutTestClicked();
    onSuspendButtonClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onStartButtonClicked()
{
    // allow them to start test if the fields have valid data
    if ( !validData() )
        return;

    ///< create some pointer to session data
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );
    DomCurrentPLU *product( currentProduct() );

    cutTestPlu->pluNumber = product->itemNumber().toInt();
    cutTestPlu->meatSpecSheet = product->meatSpecificationSheet().toInt();
    cutTestPlu->cutTestNumber = testnum_;

    // if we do have a meat spec sheet with this PLU, we need to request it.
    if( cutTestPlu->pluNumber > 0 )
    {
        QString group   = QString::number( cutTestPlu->meatSpecSheet );
        QString feature = QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
        session_->send( SoftkeyDev, LoadSoftkeyGroup, feature + "," + group );       // (136,93)
    }

    // because while requesting if the Primal PLU entered to do a test on,
    // it displayed it on run screen if it existed. Otherwise we have to prevent
    // it from displaying to the screen If cut test dialog popped dont display

    // this is causing all kinds of grief like jumping screens and redrawing flashkeys. I need the
    // screen to clear but I dont need all of the extra baggage.  Conflicted what to do right now.
    // maybe if I DONT have a spec sheet, then do this.
    if( cutTestPlu->meatSpecSheet <= 0 )
        session_->send( ProductDev, ProductRead, "0" );

    // get the operator
    QString operatorNumber = session_->scaleConfig->getLoggedOperatorId();

    // clean up the price and weight
    QString packweight = ui->productWeight->text();
    QString packcost   = ui->productCost->text();
    packweight.replace("kg", "");      // get rid of it if its present
    packweight.replace("lb", "");      // get rid of it if its present
    packweight.trimmed();
    packcost.replace("$","");

    session_->send( CutTestDev, BeginCuttingTest,
        QString::number( testnum_ )       + "," +
        "\"" + operatorNumber             + "\"," +
        "\"" + ui->supplierNumber->text() + "\"," +
        ui->productNumber->text()         + "," +
        packweight                        + "," +
        packcost                          + "," +
        "," +                                                      // startDate - not used
        "," +                                                      // endDate - not used
        "\"" + ui->lotNumber->text()      + "\"," +                // new
        saveddate_                        + "," +                  // new
        ui->packingHours->text()          + "," +                  // new
        ui->packingMinutes->text()        + "," +                  // new
        "\"" + comments_                  + "\"," +                // new
        "\"" + finalcomments_             + "\""   );              // new

    // done! so lets bail
    mainWindow_->toggleCuttingTest( true );
    startedTest_ = true;
    testActive_ = false;                                           // means we are done and are closing this screen
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onSuspendButtonClicked()
{
    // allow them to suspend test if the fields have valid data
    //if ( !validData() )
    //    return;

    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

    suspendedTest_ = true;

    cutTestPlu->pluNumber = 0;
    cutTestPlu->meatSpecSheet = 0;
    cutTestPlu->cutTestNumber = 0;

    // send a suspend cut test flashkey type message
    // decided we didnt need this.

    // clean up the weight and price
    QString packwgt = ui->productWeight->text();
    QString packcst = ui->productCost->text();
    packwgt.replace("kg", "");      // get rid of it if its present
    packwgt.replace("lb", "");      // get rid of it if its present
    packwgt.trimmed();
    packcst.replace("$","");

    // Cant trust that the operator has filled out any fields
    QString operNum = session_->scaleConfig->getLoggedOperatorId();      // get the operator
    QString suppNum = ui->supplierNumber->text();
    QString prodNum = ui->productNumber->text();
    QString lottNum = ui->lotNumber->text();
    QString packHrs = ui->packingHours->text();
    QString packMin = ui->packingMinutes->text();

    if( operNum == "" )
        operNum = "0";
    if( suppNum == "" )
        suppNum = "";
    if( prodNum == "" )
        prodNum = "0";
    if( packwgt == "" )
        packwgt = "0.000";
    if( packcst == "" )
        packcst = "0.00";
    if( lottNum == "" )
        lottNum = "";
    if( packHrs == "")
        packHrs = "12";
    if( packMin == "" )
        packMin = "0";

    session_->send( CutTestDev, SuspendCuttingTest,
        QString::number( testnum_ ) + "," +
        "\"" + operNum              + "\"," +
        "\"" + suppNum              + "\"," +
        prodNum                     + "," +
        packwgt                     + "," +
        packcst                     + "," +
        "," +                                                // startDate - not used
        "," +                                                // endDate - not used
        "\"" + lottNum              + "\"," +                // new
        saveddate_                  + "," +                  // new
        packHrs                     + "," +                  // new
        packMin                     + "," +                  // new
        "\"" + comments_            + "\"," +                // new
        "\"" + finalcomments_       + "\""   );              // new

    session_->scaleConfig->cuttingTestDialogActive = false;

    // Now turn off the current test and return to Start a Run Screen
    mainWindow_->toggleCuttingTest( false );

    // clear out the validated PLU off of the screen behind this dialog
    session_->send( ProductDev, ProductRead, "0" );

    testActive_ = false;                                     // means we are done and are closing this screen
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onCommentButtonClicked()
{
    // Launch a Comments dialog
    // The additional comments and initial field values are the same dialog, we just hide and display
    // what we want to display at any given time. Here we are displaying the data entry information
    // and we just clicked the Additional Comments button, so we hide the data entry portion and display
    // the Additional Comments area. We also seed the text area with any comments that may already exist.

    ui->lblInstructions->setText( tr("Please enter any additional comments") );
    ui->txtComments->setPlainText( comments_ );
    ui->grpStartCutTest->hide();

    ui->grpAdditionalComments->show();
    ui->txtComments->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onCancelCutTestClicked()
{
    // we need a cancel button because we just may not want to do it, but don't want to suspend it.
    // Also may have gotten to this screen because we didn't know what we were doing or accident.

    canceledTest_ = true;

    // send message to cancel the current cut test.  Will this save the number for reuse? Dont know.
    session_->send( CutTestDev, CuttingTestClear );

    // Get some pointers to stuff
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

    cutTestPlu->pluNumber = 0;
    cutTestPlu->meatSpecSheet = 0;
    cutTestPlu->cutTestNumber = 0;

    session_->scaleConfig->cuttingTestDialogActive = false;

    // Now turn off the current test and return to Start a Run Screen
    mainWindow_->toggleCuttingTest( false );

    // return to the Start Screen because we got here clicking Save on Final Comments
    mainWindow_->navigateHome();
    // reset the screen of the current PLU
    session_->send( ProductDev, ProductRead, "0" );

    // means we are done and are closing this screen
    testActive_ = false;                                           // means we are done and are closing this screen
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::displayInitialState()
{
    if ( beginOrEndTest_ == CutTestState::CutTestState_End )
    {
        // display the final comments dialog
        ui->lblInstructions->setText( tr("Please enter any final comments") );
        ui->grpStartCutTest->hide();

        ui->grpAdditionalComments->show();
        ui->txtComments->setFocus();
    }
    else
    {
        // by default we have to be beginning a test so let them enter data
        ui->lblInstructions->setText( tr("Please enter any additional comments") );
        ui->grpAdditionalComments->hide();
        ui->grpStartCutTest->show();
        // ui->productNumber->setFocus();  // had to get rid of this because we could not see the Cancel button on Startup
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::endCurrentCuttingTest()
{
    session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_EndCutTest ) + "," + QString::number( Action_EndCutTest)  + "," + QString::number( hobart::softkeysWidget::RunScreenCutTestOnOptionOne ) );

    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

    cutTestPlu->pluNumber = 0;
    cutTestPlu->meatSpecSheet = 0;
    cutTestPlu->cutTestNumber = 0;

    //PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

    // Reset the Main Screen Flashkeys to 999003
    //if ( pluMainScreen )
    //    pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onCutTestSaveComments()
{
    // The additional comments and initial field values are the same dialog, we just hide and display
    // what we want to display at any given time. When the initial comments overlay is being displayed
    // and we click the SAVE button, we hide the additional comments and then show the data entry parts
    // of course we also save off the entered additional comments

    if ( beginOrEndTest_ == CutTestState::CutTestState_End )
    {
        endCurrentCuttingTest();

        finalcomments_ = ui->txtComments->toPlainText();     // get the comments
        finalcomments_.trimmed();                            // clean up the comments

        session_->send( CutTestDev, CuttingTestComplete,
            QString::number( testnum_ ) + "," +
            "," +
            "," +
            "," +
            "," +
            "," +
            "," +                               // startDate - not used
            "," +                               // endDate - not used
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "\"" + finalcomments_ + "\"" );     // new

        // This makes it go to home screen. we dont want that
        mainWindow_->toggleCuttingTest( false );    // return to run screen

        // this just does everything as toggleCuttingTest but no going home
        //mainWindow_->resetCuttingTest( false );

        // need to go to the Details Screen
        session_->send( CutTestDev, CuttingTestDetailsRead, QString::number( testnum_ ) );
        // Send a request to see the Details.  The reply will pop the details screen
    }
    else
    {
        comments_ = ui->txtComments->toPlainText();     // get the comments
        comments_.trimmed();                            // clean up the comments
        ui->txtComments->clear();
        ui->grpAdditionalComments->hide();
        ui->grpStartCutTest->show();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onCutTestCancelComments()
{
    // The additional comments and initial field values are the same dialog, we just hide and display
    // what we want to display at any given time. When the initial comments overlay is being displayed
    // and we click the CANCEL button, we hide the additional comments and then show the data entry parts
    // and of course we dont save any entered additional comments

    if ( beginOrEndTest_ == CutTestState::CutTestState_End )
    {
        endCurrentCuttingTest();

        session_->send( CutTestDev, CuttingTestComplete,
            QString::number( testnum_ ) + "," +
            "," +
            "," +
            "," +
            "," +
            "," +
            "," +                               // startDate - not used
            "," +                               // endDate - not used
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "," +                               // new
            "" );                               // new

        // done!  Lets jump
        mainWindow_->toggleCuttingTest( false );    // return to run screen

        // need to go to the Details Screen
        //mainWindow_->launchCuttingTestDetails();

        // Even though I dodnt save anything I need to go to the Details Screen
        session_->send( CutTestDev, CuttingTestDetailsRead, QString::number( testnum_ ) );
    }
    else
    {
        ui->grpAdditionalComments->hide();
        ui->grpStartCutTest->show();
    }
}

void CuttingTestInitialInformation::onReturnToCutTest()
{
    continueTest_ = true;

    session_->scaleConfig->cuttingTestDialogActive = false;

    ///< create some pointer to session data
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

    // if we do have a meat spec sheet with this PLU, we need to request it.
    if( cutTestPlu->pluNumber > 0 )
    {
        QString group   = QString::number( cutTestPlu->meatSpecSheet );
        QString feature = QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
        session_->send( SoftkeyDev, LoadSoftkeyGroup, feature + "," + group );       // (136,93)
    }

    // because while requesting if the Primal PLU entered to do a test on,
    // it displayed it on run screen if it existed. Otherwise we have to prevent
    // it from displaying to the screen If cut test dialog popped dont display

    // this is causing all kinds of grief like jumping screens and redrawing flashkeys. I need the
    // screen to clear but I dont need all of the extra baggage.  Conflicted what to do right now.
    // maybe if I DONT have a spec sheet, then do this.
    if( cutTestPlu->meatSpecSheet <= 0 )
        session_->send( ProductDev, ProductRead, "0" );

    // done! so lets bail
    mainWindow_->toggleCuttingTest( true );

    mainWindow_->flipBackgroundImage();

    // means we are done and are closing this screen
    startedTest_ = true;
    testActive_ = true;                                           // means we are done and are closing this screen
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::onLookupPluNumber()
{
    if( suspendedTest_ || imOuttaHere )
        return;

    QString mynum = ui->productNumber->text();
    session_->send( ProductDev, ProductRead, mynum );
}

void CuttingTestInitialInformation::onCalendarClicked(QDate newDate)
{
    // the user clicked on a day in a month.

    ui->packingDateCalendar->setVisible( false );          // hide the calendar

    int intday   = newDate.dayOfWeek();                     // get the day of week (like Tue)
    int intmon   = newDate.month();                         // get a month, (like Feb)

    QString date    = QString::number( newDate.day() );     // process the clicked infor
    QString year    = QString::number( newDate.year() );    // to get the date, year
    QString weekday = QDate::shortDayName( intday );        // day of the week
    QString month   = QDate::shortMonthName( intmon );      // and month. short here means Mar instead of March

    QString thedate = weekday + " " + month + " " + date + " " + year;
    QDate mydate      = QDate::fromString(thedate);
    QString mydatenow = mydate.toString("dd/MMMM/yyyy");    // format the displayed like customer wanted

    // Set the initial Date in Calendar control
    ui->packingDateCalendar->setSelectedDate( mydate );

    ui->packingDate->setText(mydatenow);                    // update the packing date field
    saveddate_        = mydate.toString("yy/MM/dd");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int CuttingTestInitialInformation::makeValidatorTop( int digits )
{
    // when we have reached the max number of digits, cut one off the front to add new one to the end
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestInitialInformation::doxygenImpactAndRegressionTestingComments()
{
    /**
     * The following method will contain only comments to discuss any and all newly added code
     * and its impact to other code modules to identify regression test requirements
     */

    ///< Cutting Test feature impact on existing code.
    ///< Impact: Interferring with existing Default Cutting Test. Mainly this will be in the form of:
    ///< 1: The Option1 dialog popping instead of the Default.
    ///< 2: Sending Option1 messages (which will be a little different) when the Default Cutting Test is active, or vice versa
    ///< 3: Received messages from the application go to the Default instead of Option1, and vice versa
    ///
}
