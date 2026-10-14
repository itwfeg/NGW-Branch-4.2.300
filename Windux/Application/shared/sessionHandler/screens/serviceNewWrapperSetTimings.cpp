/* @file serviceNewWrapperSetTimings.cpp
 * Allows Service technicians to modify wrap box timings.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 */
#if 0
#define USE_QDEBUG
#endif

#include <logmanager.h>

#include "serviceNewWrapperSetTimings.h"
#include "ui_serviceNewWrapperSetTimings.h"
#include "serviceNewWrapperEditBox.h"
#include "serviceNewWrapperOneHeightBoxParam.h"
#include "serviceNewWrapperDiagActuators.h"
#include "operateNewWrapperRunIn.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/servicemode.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>
#include <TokenParser.h>

#include <QVector>
#include <QIntValidator>
#include <QTableWidget>
#include <palette.h>
#include <backgroundwidgetstyles.h>
#include <QTimer>


static const int DEFAULT_ROWS = 7;
static const int DEFAULT_COLS = 2;
static const int MAX_PARAMS = 21;                                     // there are 21 items, 0-20
static const int TESTING = 0;
static const int DONE_STATE = 0;
static const int CANCEL_STATE = 1;

static const bool DEMO_ALL_GOOD_MODE = false;

namespace hobart{
    class numericLineEdit;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

serviceNewWrapperSetTimings::serviceNewWrapperSetTimings(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperSetTimings),
    wrTimingsData_(NULL),
    isExitingTimings_( false )
{
//	Retrieve config object
    if ( mainWindow_->isServiceMode() )
		wrTimingsData_ = &dynamic_cast<ServiceMode*>( mode_ )->wrTimingsData_;

    ui->setupUi(this);
    scaleScreen();

    internalConfigData_ = &session_->scaleConfig->internalConfig;
    internalConfigData_->messagesSuppressed = true;

    wrTimingsTestResults_ = &session_->scaleConfig->wrTimingsTestResults;

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    setHighlight( ui->tblWrapBoxList );

    initialize();
    translate();
}

serviceNewWrapperSetTimings::~serviceNewWrapperSetTimings()
{
    LOG_CLASS_INFO( "serviceSetTimings", "serviceSetTimings" );

    LOG_TRACE( "~serviceNewWrapperSetTimings() START", NULL );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::~serviceNewWrapperSetTimings()";
#endif

    delete ui;

    if( enabledTimer )
        delete enabledTimer;

    LOG_TRACE( "~serviceSetTimings() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::initialize()
{
    LOG_CLASS_INFO( "serviceSetTimings", "initialize" );

    LOG_DEBUG( "initialize() START", NULL );

    requestingData = false;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    numOfRows = 0;
	switchToTimingsPage_ = false;
	sendDonePkgTimingsMsg_ = true;
	trayDeleted_ = false;
	wrTimingsData_->updateActualPkg = false;	//	Clear out from any previously cycled package.
	wrTimingsData_->cycledTrayName = "";		//	Clear out from any previously cycled package.
    wrTimingsData_->exitByMenu = true;          //  We'll set this to false when we change to another Timings page.

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;
    currentOrDefaultSettings();

	if (activeRow == -1)
	{
		setupWrapBoxParameters();		
		activeRow = 0;
        wrTimingsData_->precedence = activeRow;
	}

    ui->lblFailures->setText("");

    // Set to False by Default - Set to True when going TO RunIn or Actuators Screen
    // When leaving RunIn or Actuators, we set this to false. False by default too.
    // When TRUE, will activate "Return to Timings" button, and if TRUE,
    // won't allow leaving the Test Screen. MUST click "Return to Timings"
    session_->scaleConfig->internalConfig.isTimingsActive = false;

    connect(ui->butActuatorsTest,   SIGNAL(clicked()),                  this,   SLOT(onActualtorsClicked()));
    connect(ui->butRunInTest,       SIGNAL(clicked()),                  this,   SLOT(onRunInClicked()));

    connect(ui->tblWrapBoxList,     SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));       // click cell
	//	Ways to change or set up a new Wrap box (Tray).
    connect(ui->butEdit,    		SIGNAL(clicked()),              this,       SLOT(viewEditTray()));

    connect(ui->butDelete,    		SIGNAL(clicked()),              this,       SLOT(deleteTray()));

    connect(ui->butAutoConfig,      SIGNAL(clicked()),              this,       SLOT(autoConfigureTray()));

    connect(ui->butAutoDetect,    	SIGNAL(clicked()),              this,       SLOT(autoDetectTray()));

    connect(ui->butOneParameter,    SIGNAL(clicked()),              this,       SLOT(viewEditOneParamForAll()));

    connect(ui->butMoveUp,       	SIGNAL(clicked()),              this,       SLOT(moveUp()));
    connect(ui->butMoveDown,      	SIGNAL(clicked()),              this,       SLOT(moveDown()));
    connect(ui->butViewDefaults,    SIGNAL(clicked()),              this,       SLOT(toggleViewDefaultSettings()));
    connect(ui->butResetToDefault,  SIGNAL(clicked()),              this,       SLOT(resetToFactoryDefaults()));

    connect(ui->butTimingsOverride,  SIGNAL(clicked()),              this,      SLOT(onIgnoreTests() ));
    connect(ui->butRunElevatorUp,    SIGNAL(clicked()),              this,      SLOT(onRunElevatorUp() ));
    connect(ui->butReturnToTests,    SIGNAL(clicked()),              this,      SLOT(onReturnToTests() ));

    enabledTimer = new QTimer(this);
    connect( enabledTimer, SIGNAL(timeout()), SLOT(onEnabledTimerTimeout()));

    // SOF-1522: Display "Waiting" message on the screen until the data is received, processed & displayed
    //           Code snippet below stolen from buildTrayList()
    clearTheTrayList();
    int tmpNumRows = 2;

    ui->tblWrapBoxList->setRowCount(DEFAULT_ROWS);
    ui->tblWrapBoxList->setColumnCount(1);

    ui->tblWrapBoxList->setColumnWidth(0,708);
    //ui->tblWrapBoxList->setColumnWidth(0,526);
    //ui->tblWrapBoxList->setColumnWidth(1,182);

    for( int j = 0; j < tmpNumRows; j++ )
    {
        QTableWidgetItem *trayTypeItem = new QTableWidgetItem;
        trayTypeItem->setText( ( j == ( tmpNumRows - 1 ) ) ? tr( "Waiting..." ) : "" );
        trayTypeItem->setTextAlignment( Qt::AlignCenter );
        ui->tblWrapBoxList->setItem( j, 0, trayTypeItem );
    }

    requestingData = true;
    if (wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::None)
    {
        session_->send(Wrapper, WrReqEvolSystemInfo);          // 9,143
    }
    if (wrTimingsData_->loadTrays)
    {
        session_->send(Wrapper, WrEvolLoadPackageTimings, (viewCurrentTimings_ ? "1" : "0"));   // 9,226, viewCurrentTimings_
    }
    else
    {
        buildTrayList();
    }

    if( TESTING )
    {
        if (wrTimingsData_->trayList != NULL)
        {
            delete wrTimingsData_->trayList;
            wrTimingsData_->trayList = NULL;
        }

        QVector<QString> tstMsg;
        tstMsg.append("9");
        tstMsg.append("237");
        tstMsg.append("5");
        tstMsg.append(" \"\"9H\"\",\"\"25H\"\",\"\"8s 8H\"\",\"\"Dflt Wide\"\",\"\"Dflt Narr\"\"");
        tstMsg.append("5");
        tstMsg.append(" \"\"0 \"\", \"\"0 \"\", \"\"0 \"\", \"\"0 \"\", \"\"0\"\"");
        tstMsg.append("5");
        tstMsg.append(" \"\"0 \"\", \"\"0 \"\", \"\"0 \"\", \"\"0 \"\", \"\"0\"\"");

        wrTimingsData_->trayList = new DomNewWrapperAllTrays(tstMsg);
        buildTrayList();
    }

    if( enabledTimer )
    {
        enabledTimer->setSingleShot(true);
        enabledTimer->start(3000);      // 3 sec timeout
    }

    ui->chkFilmLoaded->setChecked( wrTimingsTestResults_->testFilmLoadProperly );
    ui->chkFilmSelectorClean->setChecked( wrTimingsTestResults_->testFilmRollerClean );
    ui->chkFilmRollerTensioned->setChecked( wrTimingsTestResults_->testFilmRollerTensioned );
    ui->chkMeasureGripper->setChecked( wrTimingsTestResults_->testGripperFilmSelectorHeight );
    ui->chkMeasureGripperSlideBlocks->setChecked( wrTimingsTestResults_->testGripperSlideBlocks );
    ui->chkElevatorHeight->setChecked( wrTimingsTestResults_->testElevatorHeight );
    ui->chkBrakeGap->setChecked( wrTimingsTestResults_->testBrakeGap );
    ui->chkBrakeTorque->setChecked( wrTimingsTestResults_->testBrakeTorque );

    connect(ui->chkFilmLoaded,                  SIGNAL(stateChanged(int)),        this, SLOT(onFilmLoadedChecked(int)));
    connect(ui->chkFilmSelectorClean,           SIGNAL(stateChanged(int)),        this, SLOT(onFilmSelectorCleanChecked(int)));
    connect(ui->chkFilmRollerTensioned,         SIGNAL(stateChanged(int)),        this, SLOT(onFilmRollerTensionedChecked(int)));
    connect(ui->chkMeasureGripper,              SIGNAL(stateChanged(int)),        this, SLOT(onMeasureGripperChecked(int)));
    connect(ui->chkMeasureGripperSlideBlocks,   SIGNAL(stateChanged(int)),        this, SLOT(onMeasureGripperSlideBlocksChecked(int)));
    connect(ui->chkElevatorHeight,              SIGNAL(stateChanged(int)),        this, SLOT(onElevatorHeightChecked(int)));
    connect(ui->chkBrakeGap,                    SIGNAL(stateChanged(int)),        this, SLOT(onBrakeGapChecked(int)));
    connect(ui->chkBrakeTorque,                 SIGNAL(stateChanged(int)),        this, SLOT(onBrakeTorqueChecked(int)));

    LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM initialize()", NULL );
    setStackTestLevel();                    // what stack item do we display coming in here?  Have all tests been completed?

    LOG_DEBUG( "serviceSetTimings::initialize():  Done", NULL );
}

void serviceNewWrapperSetTimings::updateTestResults()
{
    LOG_CLASS_INFO( "serviceSetTimings", "updateTestResults" );
    LOG_DEBUG( "updateTestResults() START #1", NULL );

    QString actuatorStr = "----";               // indicates: PASS/FAIL/NotRun
    QString runinStr = "----";                  // indicates: PASS/FAIL/NotRun

    LOG_DEBUG( "updateTestResults() SWITCH actuatorTestState with option: %d", session_->scaleConfig->internalConfig.actuatorTestState );
    switch( session_->scaleConfig->internalConfig.actuatorTestState )
    {
        case TimingsTestState::Test_Not_Performed:
        {
            LOG_DEBUG( "actuatorTestState case Not Performed", NULL );
            ui->lblActuatorTestPassFail->setVisible( false );
            actuatorStr = tr("Test Not Run");
            break;
        }

        case TimingsTestState::Test_Failed:
        {
            if( DEMO_ALL_GOOD_MODE )
            {
                LOG_DEBUG( "actuatorTestState Test case PASSED", NULL );
                QPixmap pix(":/images/screens/pass.png");
                ui->lblActuatorTestPassFail->setPixmap(pix);
                ui->lblActuatorTestPassFail->setScaledContents(true);
                ui->lblActuatorTestPassFail->setVisible( true );
                actuatorStr = tr("Test PASSED!");
            }
            else
            {
                LOG_DEBUG( "actuatorTestState Test case FAILED", NULL );
                QPixmap pix(":/images/screens/fail.png");
                ui->lblActuatorTestPassFail->setPixmap(pix);
                ui->lblActuatorTestPassFail->setScaledContents(true);
                ui->lblActuatorTestPassFail->setVisible( true );
                actuatorStr = tr("Test FAILED!");
            }
            break;
        }

        case TimingsTestState::Test_Passed:
        {
            LOG_DEBUG( "actuatorTestState Test case PASSED", NULL );
            QPixmap pix(":/images/screens/pass.png");
            ui->lblActuatorTestPassFail->setPixmap(pix);
            ui->lblActuatorTestPassFail->setScaledContents(true);
            ui->lblActuatorTestPassFail->setVisible( true );
            actuatorStr = tr("Test PASSED!");
            break;
        }
    }

     LOG_DEBUG( "updateTestResults() SWITCH runInTestState with option: %d", session_->scaleConfig->internalConfig.runInTestState );
    switch( session_->scaleConfig->internalConfig.runInTestState )
    {
        case TimingsTestState::Test_Not_Performed:
        {
            LOG_DEBUG( "runInTestState case Not Performed", NULL );
            ui->lblRunInTestPassFail->setVisible( false );
            runinStr = tr("Test Not Run");
            break;
        }

        case TimingsTestState::Test_Failed:
        {
            if( DEMO_ALL_GOOD_MODE )
            {
                LOG_DEBUG( "runInTestState case Test PASSED", NULL );
                QPixmap pix(":/images/screens/pass.png");
                ui->lblRunInTestPassFail->setPixmap(pix);
                ui->lblRunInTestPassFail->setScaledContents(true);
                ui->lblRunInTestPassFail->setVisible( true );
                runinStr =  tr("Test PASSED!");
            }
            else
            {
                LOG_DEBUG( "runInTestState case Test FAILED", NULL );
                QPixmap pix(":/images/screens/fail.png");
                ui->lblRunInTestPassFail->setPixmap(pix);
                ui->lblRunInTestPassFail->setScaledContents(true);
                ui->lblRunInTestPassFail->setVisible( true );
                runinStr = tr("Test FAILED!");
            }
            break;
        }

        case TimingsTestState::Test_Passed:
        {
            LOG_DEBUG( "runInTestState case Test PASSED", NULL );
            QPixmap pix(":/images/screens/pass.png");
            ui->lblRunInTestPassFail->setPixmap(pix);
            ui->lblRunInTestPassFail->setScaledContents(true);
            ui->lblRunInTestPassFail->setVisible( true );
            runinStr =  tr("Test PASSED!");
            break;
        }
    }

    //LOG_DEBUG( "updateTestResults() errorCodeStr_: %s", qPrintable( errorCodeStr_) );
    //LOG_DEBUG( "updateTestResults() actuatorStr: %s", qPrintable( actuatorStr) );
    //LOG_DEBUG( "updateTestResults() runInStr: %s", qPrintable( runinStr) );

    //LOG_DEBUG( "updateTestResults() finally update test results", NULL );
    ui->lblActuatorTestResult->setText( actuatorStr );
    ui->lblRunInTestResult->setText( runinStr );

    QApplication::processEvents();          // force an update on the text
    qApp->processEvents();

    ui->lblActuatorTestResult->setVisible( false );
    ui->lblRunInTestResult->setVisible( false );
    ui->lblActuatorTestResult->setVisible( true );
    ui->lblRunInTestResult->setVisible( true );

    //LOG_DEBUG( "updateTestResults() AFTER update actuatorStr: %s", qPrintable( ui->lblActuatorTestResult->text() ) );
    //LOG_DEBUG( "updateTestResults() AFTER update runInStr: %s", qPrintable( ui->lblRunInTestResult->text() ) );

    LOG_DEBUG( "updateTestResults(): DONE", NULL );
}

void serviceNewWrapperSetTimings::updateFailureList()
{
    LOG_CLASS_INFO( "serviceSetTimings", "updateFailureList" );

    //LOG_DEBUG( "updateFailureList() START #1", NULL );
    //LOG_DEBUG( "updateFailureList(): runInFailures: %s", qPrintable(session_->scaleConfig->internalConfig.runInFailures) );
    //LOG_DEBUG( "updateFailureList(): actuatorFailures: %s", qPrintable(session_->scaleConfig->internalConfig.actuatorFailures) );

    // Combine both list of errors
    errorCodeStr_ = "";
    QString codes = session_->scaleConfig->internalConfig.runInFailures;            // Run In Dialog Box Failures
    if( codes.length() > 0 )
        codes.append(",");
    codes.append(session_->scaleConfig->internalConfig.actuatorFailures);           // Actuator Dialog Box Failures

    if(codes.length() == 0 )
    {
        LOG_DEBUG( "updateFailureList(): No dialog codes to list so tell user", NULL );
        //errorCodeStr_ = tr("No Dialog Box Errors to report.\n");
    }

    QStringList codeList;
    if( codes.length() > 0)
        codeList = codes.split(",");

    int x = codeList.length();      // does nothing - use to limit the length of the list but got rid of it
    //LOG_DEBUG( "updateFailureList(): codes: %s", qPrintable(codes) );
    //LOG_DEBUG( "updateFailureList(): Length of codes list: %d", x );

    int i = 0;
    foreach (const QString &errStr, codeList )
    {
        int errCode = errStr.toInt();

        errorCodeStr_.append( QString::number(i++) );
        errorCodeStr_.append(". ");
        //LOG_DEBUG( "updateFailureList() for loop code @ elem[%d]: %d", i-1, errCode );

        switch( errCode )
        {
            case 9003:
                errorCodeStr_.append( tr( "EEP write error on wrapper.") );
                break;
            case 9004:
            case 9005:
                errorCodeStr_.append( tr( "Check package length sensor and centering arm for debris." ) );
                break;
            case 9006:
                errorCodeStr_.append( tr( "Main motor stalled. Turn handwheel counterclockwise." ) );
                break;
            case 9007:
                errorCodeStr_.append( tr( "Wraper Intake jammed. Clean out debris") );
                break;
            case 9008:
                errorCodeStr_.append( tr( "Wrapper length switch error." ) );
                break;
            case 9009:
                errorCodeStr_.append( tr( "Wrapper width switch error." )  );
                break;
            case 9010:
                errorCodeStr_.append( tr( "Wrapper intake pusher unable to find home" ) );
                break;
            case 9011:
                errorCodeStr_.append( tr( "Wrapper package jammed. Turn handwheel to free package." ) );
                break;
            case 9012:
                errorCodeStr_.append( tr( "Wrapper did not pull wide film." ) );
                break;
            case 9013:
                errorCodeStr_.append( tr( "Wrapper did not pull narrow film.") );
                break;
            case 9014:
                errorCodeStr_.append( tr( "Wrapper right side cover open. Reposition right side cover." ) );
                break;
            case 9015:
                errorCodeStr_.append( tr( "Wrapper left side cover open. Reposition left side cover." ) );
                break;
            case 9016:
                errorCodeStr_.append( tr( "Wrapper clean out pan open. Remove, clear debris, close." ) );
                break;
            case 9017:
                errorCodeStr_.append( tr( "Wrapper top cover is open. Close top cover." ) );
                break;
            case 9018:
                errorCodeStr_.append( tr( "Wrapper top cover is open. Close top cover." ) );
                break;
            case 9019:
                errorCodeStr_.append( tr( "Package on Wrapper intake is too tall." ) );
                break;
            case 9020:
                errorCodeStr_.append( tr( "Film supply door open. Close the rear supply door." ) );
                break;
            case 9021:
                errorCodeStr_.append( tr( "Wrapper handwheel door is open. Close handwheel door." ) );
                break;
            case 9022:
                errorCodeStr_.append( tr( "Weigher platter is not seated" ) );
                break;
            case 9023:
                errorCodeStr_.append( tr( "Wrapper elevator is collapsed." ) );
                break;
            case 9024:
                errorCodeStr_.append( tr( "Sealer belt is not in locked position.") );
                break;
            case 9025:
                errorCodeStr_.append( tr( "General fault detected on wrapper sealer belt." ) );
                break;
            case 9026:
                errorCodeStr_.append( tr( "General fault detected on wrapper intake." ) );
                break;
            case 9027:
                errorCodeStr_.append( tr( "Package on Wrapper intake is too long" ) );
                break;
            case 9028:
                errorCodeStr_.append( tr( "Heater hardware error on wrapper." ) );
                break;
            case 9100:
                errorCodeStr_.append( tr( "Last package cycled does not fit this Pack code." ) );
                break;
            case 9501:
                errorCodeStr_.append( tr( "The wrapper stopped communicating." ) );
                break;
            case 9502:
                errorCodeStr_.append( tr( "Primary intake sensor or belt failure." ) );
                break;
            case 9503:
                errorCodeStr_.append( tr( "Primary intake motor failure." ) );
                break;
            case 9504:
                errorCodeStr_.append( tr( "Primary intake slow.") );
                break;
            case 9505:
                errorCodeStr_.append( tr( "Secondary intake motor failure." ) );
                break;
            case 9506:
                errorCodeStr_.append( tr( "Height sensor calibration failure." ) );
                break;
            case 9507:
                errorCodeStr_.append( tr( "Rear Folder belt or sensor failure." ) );
                break;
            case 9508:
                errorCodeStr_.append( tr( "Rear Folder motor electrical failure." ) );
                break;
            case 9509:
                errorCodeStr_.append( tr( "Rear Folder slow." ) );
                break;
            case 9510:
                errorCodeStr_.append( tr( "Front Folder belt or sensor failure." ) );
                break;
            case 9511:
                errorCodeStr_.append( tr( "Front Folder motor electrical failure" ) );
                break;
            case 9512:
                errorCodeStr_.append( tr( "Sealer Belt motor electrical failure." ) );
                break;
            case 9513:
                errorCodeStr_.append( tr( "Pusher sensor or belt failure." ) );
                break;
            case 9514:
                errorCodeStr_.append( tr( "Pusher motor electrical failure." ) );
                break;
            case 9515:
                errorCodeStr_.append( tr( "Primary conveyor not at home." ) );
                break;
            case 9516:
                errorCodeStr_.append( tr( "Camera and height sensors blocked." ) );
                break;
            case 9517:
                errorCodeStr_.append( tr( "Primary intake jammed." ) );
                break;
            case 9518:
                errorCodeStr_.append( tr( "Primary conveyor did not find home." ) );
                break;
            case 9519:
                errorCodeStr_.append( tr( "Intake blocked." ) );
                break;
            case 9520:
                errorCodeStr_.append( tr( "Secondary timing error." ) );
                break;
            case 9521:
                errorCodeStr_.append( tr( "Measurement Error." ) );
                break;
            case 9522:
                errorCodeStr_.append( tr( "Intake sensor error." ) );
                break;
            case 9523:
                errorCodeStr_.append( tr( "Secondary intake jammed." ) );
                break;
            case 9524:
                errorCodeStr_.append( tr( "Centering conveyor not at home." ) );
                break;
            case 9525:
                errorCodeStr_.append( tr( "Cameras need alignment. Clean camera, light, and sensors." ) );
                break;
            case 9526:
                errorCodeStr_.append( tr( "Package is too far left." ) );
                break;
            case 9527:
                errorCodeStr_.append( tr( "Package is too far right." ) );
                break;
            case 9528:
                errorCodeStr_.append( tr( "Centering conveyor did not find home." ) );
                break;
            case 9529:
                errorCodeStr_.append( tr( "Secondary slow." ) );
                break;
            case 9530:
                errorCodeStr_.append( tr( "Elevator not at home." ) );
                break;
            case 9531:
                errorCodeStr_.append( tr( "Elevator jammed. Remove packages, labels, and debris." ) );
                break;
            case 9532:
                errorCodeStr_.append( tr( "Elevator motor electrical failure." ) );
                break;
            case 9533:
                errorCodeStr_.append( tr( "Elevator failed to home." ) );
                break;
            case 9534:
                errorCodeStr_.append( tr( "Elevator sensor cam failure." ) );
                break;
            case 9535:
                errorCodeStr_.append( tr( "Elevator sensor is not working." ) );
                break;
            case 9536:
                errorCodeStr_.append( tr( "Elevator slow." ) );
                break;
            case 9538:
                errorCodeStr_.append( tr( "Film not pulled. Check film, gripper, and clear debris." ) );
                break;
            case 9539:
                errorCodeStr_.append( tr( "Gripper not at home." ) );
                break;
            case 9540:
                errorCodeStr_.append( tr( "Knife Error." ) );
                break;
            case 9541:
                errorCodeStr_.append( tr( "Gripper jammed." ) );
                break;
            case 9542:
                errorCodeStr_.append( tr( "Gripper did not find home." ) );
                break;
            case 9543:
                errorCodeStr_.append( tr( "Gripper sensor or belt failure." ) );
                break;
            case 9544:
                errorCodeStr_.append( tr( "Gripper motor electrical failure." ) );
                break;
            case 9545:
                errorCodeStr_.append( tr( "Late film cut." ) );
                break;
            case 9546:
                errorCodeStr_.append( tr( "Side clamps not at home." ) );
                break;
            case 9547:
                errorCodeStr_.append( tr( "Side clamps jammed." ) );
                break;
            case 9548:
                errorCodeStr_.append( tr( "Side folders not at home." ) );
                break;
            case 9549:
                errorCodeStr_.append( tr( "Side folders jammed." ) );
                break;
            case 9550:
                errorCodeStr_.append( tr( "Rear folder not at home." ) );
                break;
            case 9551:
                errorCodeStr_.append( tr( "Rear folder jammed." ) );
                break;
            case 9552:
                errorCodeStr_.append( tr( "Front folder not at home." ) );
                break;
            case 9553:
                errorCodeStr_.append( tr( "Front folder jammed." ) );
                break;
            case 9554:
                errorCodeStr_.append( tr( "Pusher not at home." ) );
                break;
            case 9555:
                errorCodeStr_.append( tr( "Pusher jammed." ) );
                break;
            case 9556:
                errorCodeStr_.append( tr( "Sealer belt jammed." ) );
                break;
            case 9557:
                errorCodeStr_.append( tr( "Knife jammed." ) );
                break;
            case 9558:
                errorCodeStr_.append( tr( "Label applier did not find home." ) );
                break;
            case 9559:
                errorCodeStr_.append( tr( "Temperature Error" ) );
                break;
            case 9560:
                errorCodeStr_.append( tr( "Heater Error" ) );
                break;
            case 9561:
                errorCodeStr_.append( tr( "Gripper slow." ) );
                break;
            case 9562:
                errorCodeStr_.append( tr( "Side Clamp sensor or belt failure." ) );
                break;
            case 9563:
                errorCodeStr_.append( tr( "Side Clamp motor electrical failure." ) );
                break;
            case 9564:
                errorCodeStr_.append( tr( "Side Clamp slow." ) );
                break;
            case 9565:
                errorCodeStr_.append( tr( "Wrapper lost CAN communications." ) );
                break;
            case 9567:
                errorCodeStr_.append( tr( "Canopy open. Please close the canopy." ) );
                break;
            case 9568:
                errorCodeStr_.append( tr( "Right-side access door open." ) );
                break;
            case 9569:
                errorCodeStr_.append( tr( "Left-side access door open." ) );
                break;
            case 9570:
                errorCodeStr_.append( tr( "Canopy open. Please close the canopy." ) );
                break;
            case 9571:
                errorCodeStr_.append( tr( "Rear film door open." ) );
                break;
            case 9572:
                errorCodeStr_.append( tr( "Rear film door open." ) );
                break;
            case 9573:
                errorCodeStr_.append( tr( "Side Folder sensor or belt failure." ) );
                break;
            case 9574:
                errorCodeStr_.append( tr( "Side Folder motor electrical failure." ) );
                break;
            case 9575:
                errorCodeStr_.append( tr( "Side clamps did not find home." ) );
                break;
            case 9576:
                errorCodeStr_.append( tr( "Side folders did not find home." ) );
                break;
            case 9577:
                errorCodeStr_.append( tr( "Rear folder did not find home." ) );
                break;
            case 9578:
                errorCodeStr_.append( tr( "Front folder did not find home." ) );
                break;
            case 9579:
                errorCodeStr_.append( tr( "Pusher did not find home." ) );
                break;
            case 9581:
                errorCodeStr_.append( tr( "Package stopped on secondary intake." ) );
                break;
            case 9582:
                errorCodeStr_.append( tr( "Side Folder slow." ) );
                break;
            case 9583:
                errorCodeStr_.append( tr( "Remove packages, labels, or debris." ) );
                break;
            case 9584:
                errorCodeStr_.append( tr( "Package is too tall" ) );
                break;
            case 9585:
                errorCodeStr_.append( tr( "Node 1 motor fuse may be blown." ) );
                break;
            case 9586:
                errorCodeStr_.append( tr( "Node 2 motor fuse may be blown." ) );
                break;
            case 9587:
                errorCodeStr_.append( tr( "Node 3 motor fuse may be blown." ) );
            case 9588:
                errorCodeStr_.append( tr( "Node 4 motor fuse may be blown." ) );
                break;
            case 9589:
                errorCodeStr_.append( tr( "Node 5 motor fuse may be blown." ) );
                break;
            case 9590:
                errorCodeStr_.append( tr( "Warning. Low film on Top roll." ) );
                break;
            case 9591:
                errorCodeStr_.append( tr( "Warning. Low film on Bottom roll." ) );
                break;
            case 9592:
                errorCodeStr_.append( tr( "Memory error on node1." ) );
                break;
            case 9593:
                errorCodeStr_.append( tr( "Memory error on node2." ) );
                break;
            case 9594:
                errorCodeStr_.append( tr( "Memory error on node3." ) );
                break;
            case 9595:
                errorCodeStr_.append( tr( "Memory error on node4." ) );
                break;
            case 9596:
                errorCodeStr_.append( tr( "Memory error on node5." ) );
                break;
            case 9597:
                errorCodeStr_.append( tr( "Wrap timings memory error on node1." ) );
                break;
            case 9630:
                errorCodeStr_.append( tr( "Too many wrap boxes read aborted." ) );
                break;
            case 9631:
                errorCodeStr_.append( tr( "Package sensor across elevator is blocked." ) );
                break;
            case 9632:
                errorCodeStr_.append( tr( "Wrap box update timeout failure." ) );
                break;
            case 9633:
                errorCodeStr_.append( tr( "Wrap box update memory failure." ) );
                break;
            case 9634:
                errorCodeStr_.append( tr( "Node1 communication failure. " ) );
                break;
            case 9635:
                errorCodeStr_.append( tr( "Node3 communication failure. " ) );
                break;
            case 9636:
                errorCodeStr_.append( tr( "Node4 communication failure. " ) );
                break;
            case 9637:
                errorCodeStr_.append( tr( "Node5 communication failure. " ) );
                break;
            case 9638:
                errorCodeStr_.append( tr( "Pusher slow. Remove packages, labels, and debris." ) );
                break;
            case 9639:
                errorCodeStr_.append( tr( "Wrap box Timings Corruption." ) );
                break;
            case 9640:
                errorCodeStr_.append( tr( "Primary Intake has Stalled." ) );
                break;
            case 9641:
                errorCodeStr_.append( tr( "Secondary Intake has Stalled." ) );
                break;
            case 9642:
                errorCodeStr_.append( tr( "Elevator has Stalled." ) );
                break;
            case 9643:
                errorCodeStr_.append( tr( "Rear Folder has Stalled." ) );
                break;
            case 9644:
                errorCodeStr_.append( tr( "Front Folder has Stalled." ) );
                break;
            case 9645:
                errorCodeStr_.append( tr( "Side Folder has Stalled." ) );
                break;
            case 9646:
                errorCodeStr_.append( tr( "Side Clamp has Stalled." ) );
                break;
            case 9647:
                errorCodeStr_.append( tr( "Sealer belt has Stalled." ) );
                break;
            case 9648:
                errorCodeStr_.append( tr( "Centering conveyor jammed." ) );
                break;
            case 9649:
                errorCodeStr_.append( tr( "Gripper has Stalled." ) );
                break;
            case 9650:
                errorCodeStr_.append( tr( "Pusher has Stalled." ) );
                break;
            case 9651:
                errorCodeStr_.append( tr( "Sealer belt motor move error." ) );
                break;
            case 9652:
                errorCodeStr_.append( tr( "The Primary label is still at the printer." ) );
                break;
            case 9653:
                errorCodeStr_.append( tr( "The Second label is still at the printer." ) );
                break;
            case 9655:
            case 9657:
            case 9659:
                errorCodeStr_.append( tr( "Gripper Failed to Close." ) );
                break;
            case 9656:
            case 9658:
            case 9660:
                errorCodeStr_.append( tr( "Gripper Failed to Open." ) );
                break;
            case 9661:
            case 9663:
                errorCodeStr_.append( tr( "Side Clamp Failed to Close." ) );
                break;
            case 9662:
            case 9664:
                errorCodeStr_.append( tr( "Side Clamp Failed to Open." ) );
                break;
            case 9665:
                errorCodeStr_.append( tr( "Upper Film Roll Failed to Switch." ) );
                break;
            case 9666:
                errorCodeStr_.append( tr( "Lower Film Roll Failed to Switch." ) );
                break;
            case 9667:
                errorCodeStr_.append( tr( "Film Selector Lever is unlatched." ) );
                break;
            case 9669:
                errorCodeStr_.append( tr( "Primary Applier Wand Failed to Move." ) );
                break;
            case 9668:
            case 9670:
                errorCodeStr_.append( tr( "Primary Applier Hand Failed to Home." ) );
                break;
            case 9671:
                errorCodeStr_.append( tr( "Primary Applier Wand Vacuum Failure." ) );
                break;
            case 9672:
                errorCodeStr_.append( tr( "Air Compressor Failure." ) );
                break;
            case 9673:
                errorCodeStr_.append( tr( "Regulated Air Pressure Failure." ) );
                break;
            case 9674:
                errorCodeStr_.append( tr( "Actuator Air Pressure Failure." ) );
                break;
            case 9675:
                errorCodeStr_.append( tr( "Upper Film Pull not Detected." ) );
                break;
            case 9676:
                errorCodeStr_.append( tr( "Lower Film Pull not Detected." ) );
                break;
            case 9677:
                errorCodeStr_.append( tr( "24V Power Supply Failure." ) );
                break;
            case 9678:
                errorCodeStr_.append( tr( "170V Inrush Failure." ) );
                break;
            case 9679:
                errorCodeStr_.append( tr( "170V Power Supply Failure." ) );
                break;
            case 9680:
                errorCodeStr_.append( tr( "Upper Film Brake Failure." ) );
                break;
            case 9681:
                errorCodeStr_.append( tr( "Lower Film Brake Failure." ) );
                break;
            case 9682:
                errorCodeStr_.append( tr( "Upper Film Roll is Loose." ) );
                break;
            case 9683:
                errorCodeStr_.append( tr( "Lower Film Roll is Loose." ) );
                break;
            case 9684:
                errorCodeStr_.append( tr( "Printer Dock Failure." ) );
                break;
            case 9685:
                errorCodeStr_.append( tr( "Package Sensor across Sealer Belt is blocked." ) );
                break;
            case 9687:
                errorCodeStr_.append( tr( "Intake Package Sensor is blocked." ) );
                break;
            case 9688:
                errorCodeStr_.append( tr( "Regulated air pressure is too high." ) );
                break;
            case 9690:
                errorCodeStr_.append( tr( "Secondary Applier Wand Failed to Move." ) );
                break;
            case 9692:
                errorCodeStr_.append( tr( "Secondary Applier Wand Vacuum Failure." ) );
                break;
            case 9693:
                errorCodeStr_.append( tr( "Secondary Applier Timeout." ) );
                break;
            case 9694:
                errorCodeStr_.append( tr( "Secondary Applier jammed." ) );
                break;
            case 9689:
            case 9691:
            case 9695:
                errorCodeStr_.append( tr( "Secondary Applier failed to return to the Home." ) );
                break;
            case 9745:
                errorCodeStr_.append( tr( "Canopy cover or doors open. Close the canopy or doors. " ) );
                break;
            default:
                errorCodeStr_.append( tr( "Unknown dialog warning: ") );
                break;
        } // switch

        errorCodeStr_.append("\n");
    } // for

    // NOW DO SENSORS
    //LOG_DEBUG( "updateFailureList(): runInSensorFailures: %s", qPrintable(session_->scaleConfig->internalConfig.runInSensorFailures) );
    //LOG_DEBUG( "updateFailureList(): actuatorSensorFailures: %s", qPrintable(session_->scaleConfig->internalConfig.actuatorSensorFailures) );

    // Combine both list of errors
    QString sensorCodes = session_->scaleConfig->internalConfig.runInSensorFailures;            // Run In Dialog Box Failures
    if( sensorCodes.length() > 0 )
        sensorCodes.append(",");
    sensorCodes.append(session_->scaleConfig->internalConfig.actuatorSensorFailures);           // Actuator Dialog Box Failures

    if(sensorCodes.length() == 0 )
        LOG_DEBUG( "updateFailureList(): No sensorCodeList to list so tell user", NULL );

    QStringList sensorCodeList;
    if( sensorCodes.length() > 0)
        sensorCodeList = sensorCodes.split(",");

    x = sensorCodeList.length();      // does nothing - use to limit the length of the list but got rid of it
    //LOG_DEBUG( "updateFailureList(): Length of sensorCodeList: %d", x );
    sensorCodes.append(session_->scaleConfig->internalConfig.actuatorSensorFailures);

    i = 0;
    foreach (const QString &errStr, sensorCodeList )
    {
        int errCode = errStr.toInt();

        errorCodeStr_.append( QString::number(i++) );
        errorCodeStr_.append(". ");
        //LOG_DEBUG( "updateFailureList() for loop code @ elem[%d]: %d", i-1, errCode );

        switch( errCode )
        {
            case 0:
                errorCodeStr_.append( tr( "Check Knife Up Status") );
                break;
            case 1:
                errorCodeStr_.append( tr( "Check Knife Down Status" ) );
                break;
            case 2:
                errorCodeStr_.append( tr( "Check Gripper Narrow Close Status." ) );
                break;
            case 3:
                errorCodeStr_.append( tr( "Check Gripper Narrow Open Status" ) );
                break;
            case 4:
                errorCodeStr_.append( tr( "Check Gripper Wide Left Close Status" ) );
                break;
            case 5:
                errorCodeStr_.append( tr( "Check Gripper Wide Left Open Status" )  );
                break;
            case 6:
                errorCodeStr_.append( tr( "Check Gripper Wide Right Close Status" ) );
                break;
            case 7:
                errorCodeStr_.append( tr( "Check Gripper Wide Right Open Status" ) );
                break;
            case 8:
                errorCodeStr_.append( tr( "Check Side Clamp Left Close Status" ) );
                break;
            case 9:
                errorCodeStr_.append( tr( "Check Side Clamp Left Open Status") );
                break;
            case 10:
                errorCodeStr_.append( tr( "Check Side Clamp Right Close Status" ) );
                break;
            case 11:
                errorCodeStr_.append( tr( "Check Side Clamp Right Open Status" ) );
                break;
            case 12:
                errorCodeStr_.append( tr( "Check Wand Up Status" ) );
                break;
            case 13:
                errorCodeStr_.append( tr( "Check Wand Down Status" ) );
                break;
            case 14:
                errorCodeStr_.append( tr( "Check Film Selector Up Status" ) );
                break;
            case 15:
                errorCodeStr_.append( tr( "Check Film Selector Down Status" ) );
                break;
            case 16:
                errorCodeStr_.append( tr( "Check Tank Pressure Reading Status" ) );
                break;
            case 17:
                errorCodeStr_.append( tr( "Check Regulated Pressure Reading Status" ) );
                break;
            case 18:
                errorCodeStr_.append( tr( "Check Applier Vacuum Status" ) );
                break;
            case 19:
                errorCodeStr_.append( tr( "Check Applier Hand Home Status" ) );
                break;
            case 20:
            case 21:
            case 22:
                errorCodeStr_.append( tr( "Unknown Code " ) );
                break;
            case 23:
                errorCodeStr_.append( tr( "Check Main AC Voltage Status" ) );
                break;
            case 24:
                errorCodeStr_.append( tr( "Check Secondary Wand Up Status" ) );
                break;
            case 25:
                errorCodeStr_.append( tr( "Check Secondary Wand Down Status" ) );
                break;
            case 26:
                errorCodeStr_.append( tr( "Check Secondary Applier Hand Home Status") );
                break;
            default:
                errorCodeStr_.append( tr( "Unknown Code: " ) );
                break;
        } // switch

        errorCodeStr_.append("\n");
    } // for

    if( DEMO_ALL_GOOD_MODE )
        ui->lblFailures->setText( tr("No errors to report.") );
    else
    {
        if( errorCodeStr_.length() > 0)
            ui->lblFailures->setText(errorCodeStr_);
        else
            ui->lblFailures->setText( tr("No errors to report.") );
    }

    //LOG_DEBUG( "updateFailureList(): DONE", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperSetTimings::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::translate()
{
    LOG_CLASS_INFO( "serviceSetTimings", "translate()" );

    LOG_DEBUG( "translate() START #1", NULL );

    ui->retranslateUi( this );

    currentOrDefaultSettings();
    updateTestResults();
    updateFailureList();

    update();

    LOG_DEBUG( "translate(): DONE", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::clearTheTrayList()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::clearTheTrayList()";
#endif
    ui->tblWrapBoxList->clear();                                       // clear it before we do anything
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//void serviceNewWrapperSetTimings::buildTrayList(const SessionMessage& msg)
void serviceNewWrapperSetTimings::buildTrayList()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::buildTrayList()";
#endif

    //  Either we received 9,237 or we have come back to this page without exiting Wrapper Timings.
    //  Display the list of Wrap boxes by the Tray name, including the PLU exception for each Wrapbox/Tray.
    if (wrTimingsData_->trayList == NULL)
    {
#ifdef USE_QDEBUG
        qDebug() << "serviceSetTimings::buildTrayList():  wrTimingsData_->trayList is NULL! Exiting...";
#endif
        return;
    }

    clearTheTrayList();                                       // clear it before we do anything

//    DomNewWrapperAllTrays domList( msg );
    DomNewWrapperAllTrays *domList = wrTimingsData_->trayList;

    int myItems = domList->numOfTrays().toInt();
    numOfItems = myItems;

    if (myItems < DEFAULT_ROWS)
    {
        ui->tblWrapBoxList->setRowCount(DEFAULT_ROWS);                  // set up how many rows and colums we have.
        numOfRows = DEFAULT_ROWS;
    }
    else
    {
        ui->tblWrapBoxList->setRowCount(myItems);
        numOfRows = myItems;
    }

    ui->tblWrapBoxList->setColumnCount(1);
    ui->tblWrapBoxList->setColumnWidth(0,708);

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::buildTrayList():  " << domList->numOfTrays() << " trays total.";
#endif
    if (myItems > 0)
    {
        QVector<QString> trayTypeArray = TokenParser::splitStringTokens( domList->trayTypes() );
	    canChangeArray = domList->canChange().split(",");       // Parse the can change list

        for(int j = 0; j < myItems; j++ )
        {
            QTableWidgetItem *trayTypeItem = new QTableWidgetItem;    	// Create a column for the Tray type name.
            trayTypeItem->setText( trayTypeArray.at(j) );
            ui->tblWrapBoxList->setItem(j, 0, trayTypeItem);            // add it
        }
    }

	if (activeRow < 0)
		activeRow = 0;
    clickRow(activeRow, 0);

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    if (viewCurrentTimings_)
	{
        modifyPaletteBackground setCurrentTimingsBackground( ( QApplication::palette() ).background());
        setCurrentTimingsBackground( serviceMode );
        enableForViewCurrent();
    }
    else
	{
	    modifyPaletteBackground setDefaultTimingsBackground( hobart::colorPalette::red );
        setDefaultTimingsBackground( serviceMode );
        disableForViewDefault();
    }
	requestingData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::saveDataOnExit()
{
    LOG_CLASS_INFO( "snwSetTimings", "saveDataOnExit" );

    LOG_TRACE( "saveDataonExit() START", NULL );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::saveDataOnExit()";
#endif

	LOG_TRACE( "Calling exitWrapperTimings()", NULL );
    exitWrapperTimings();

    LOG_TRACE( "saveDataonExit() END", NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::exitWrapperTimings()
{
    LOG_CLASS_INFO( "snwSetTimings", "exitWrapperTimings" );

    LOG_TRACE( "exitWrapperTimings() START", NULL );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::exitWrapperTimings()";
#endif

	LOG_TRACE( "switchToTimingsPage_ = %u,  sendDonePkgTimingsMsg_ = %u", switchToTimingsPage_, sendDonePkgTimingsMsg_ );

    if (!switchToTimingsPage_)		//	HA-33
    {
        if (sendDonePkgTimingsMsg_)
		{
			session_->send(Wrapper, WrEvolDonePackageTimings);      //  9,230
			sendDonePkgTimingsMsg_ = false;
	    }
		//	Do this ONLY if we are exiting Wrapper Timings.
        wrTimingsData_->viewCurrentTimings = true;
    }

    LOG_TRACE( "exitWrapperTimings() END", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::clickRow(int row, int col)
{
    Q_UNUSED( col );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::clickRow(" << QString::number(row) << ")";
#endif
    if( row < numOfItems)
    {
		activeRow = row;
		checkMoveAvailability();
	    ui->tblWrapBoxList->selectRow(activeRow);
    }
}

void serviceNewWrapperSetTimings::setActiveRow(int row)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::setActiveRow(" << QString::number(row) << ")";
#endif
    activeRow = row;
	checkMoveAvailability();
	clickRow(activeRow, 0);
}

QString serviceNewWrapperSetTimings::getCurrentTrayName()
{
    QTableWidgetItem *trayTypeItem = new QTableWidgetItem;
    trayTypeItem = ui->tblWrapBoxList->item(activeRow, 0);
    QString trayName = trayTypeItem->text();
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::getCurrentTrayName():  " << trayName;
#endif
    return trayName;
}

void serviceNewWrapperSetTimings::prepareTimingsPageSwitch()
{
    LOG_CLASS_INFO( "snwSetTimings", "prepareTimingsPageSwitch" );
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::prepareTimingsPageSwitch()";
#endif

    wrTimingsData_->precedence = activeRow;
    wrTimingsData_->viewCurrentTimings = viewCurrentTimings_;
    wrTimingsData_->trayName = getCurrentTrayName();
    wrTimingsData_->pluException = "0";                     // was pluException, now just 0
    wrTimingsData_->loadTrays = false;
    wrTimingsData_->exitByMenu = false;
    wrTimingsData_->heightBoxSelected = ""; // NGW-74
	switchToTimingsPage_ = true;
	LOG_TRACE( "precedence = %d,  viewCurrentTimings = %u,  trayName = %s", wrTimingsData_->precedence, wrTimingsData_->viewCurrentTimings, qPrintable(wrTimingsData_->trayName) );
}

void serviceNewWrapperSetTimings::viewEditTray()
{
    LOG_CLASS_INFO( "snwSetTimings", "viewEditTray" );
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::viewEditTray()";
#endif
	LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
    wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}

void serviceNewWrapperSetTimings::autoConfigureTray()
{
    LOG_CLASS_INFO( "snwSetTimings", "autoConfigureTray" );
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::autoConfigureTray():  precedence = " << QString::number(activeRow);
#endif
	LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
	wrTimingsData_->trayName = "";
    wrTimingsData_->pageMode = WrapperTimingsData::pmAutoConfigure;
	mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}

void serviceNewWrapperSetTimings::autoDetectTray()
{
    LOG_CLASS_INFO( "snwSetTimings", "autoDetectTray" );
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::autoDetectTray():  precedence = " << QString::number(activeRow);
#endif
	LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
    wrTimingsData_->pageMode = WrapperTimingsData::pmAutoDetect;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}

void serviceNewWrapperSetTimings::viewEditOneParamForAll()
{
    LOG_CLASS_INFO( "snwSetTimings", "viewEditOneParamForAll" );
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::viewEditOneParamForAll()";
#endif
	LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperOneHeightBoxParam>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::deleteTray()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::deleteTray()";
#endif
    session_->send(Error, Wrapper, "9708,1");              // 103,9,9708,1
}

void serviceNewWrapperSetTimings::moveUp()
{
	move(true);
}

void serviceNewWrapperSetTimings::moveDown()
{
	move(false);
}

void serviceNewWrapperSetTimings::onIgnoreTests()
{
    //LOG_CLASS_INFO("serviceSetTimings", "onIgnoreTests");
    //LOG_DEBUG( "onIgnoreTests() - START", NULL );

    if( ( session_->scaleConfig->internalConfig.runInTestState == TimingsTestState::Test_Not_Performed ) ||
        ( session_->scaleConfig->internalConfig.actuatorTestState == TimingsTestState::Test_Not_Performed ) )
        session_->send(Error, Wrapper, "9730,1");

    ui->stkTimings->setCurrentIndex(1);

    session_->send( Wrapper, WrEvolUnlockPackageTimings );

    //LOG_DEBUG( "onIgnoreTests() - START", NULL );
}

void serviceNewWrapperSetTimings::onRunElevatorUp()
{
    // Clean Wrapper or Run Elevator Up
    session_->send( Wrapper, WrEvolTest, QString::number( WrWrapperCleanTest ) );
}

void serviceNewWrapperSetTimings::onReturnToTests()
{
    ui->stkTimings->setCurrentIndex(0);
}

void serviceNewWrapperSetTimings::onFilmLoadedChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onFilmLoadedChecked" );
    Q_UNUSED( state );
    ui->chkFilmLoaded->isChecked() ? wrTimingsTestResults_->testFilmLoadProperly = 1 : wrTimingsTestResults_->testFilmLoadProperly = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmLoadedChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onFilmSelectorCleanChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onFileSelectorCleanChecked" );
    Q_UNUSED( state );
    ui->chkFilmSelectorClean->isChecked() ? wrTimingsTestResults_->testFilmRollerClean = 1 : wrTimingsTestResults_->testFilmRollerClean = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onFilmRollerTensionedChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onFileRollerTensionedChecked" );
    Q_UNUSED( state );
    ui->chkFilmRollerTensioned->isChecked() ?  wrTimingsTestResults_->testFilmRollerTensioned = 1 : wrTimingsTestResults_->testFilmRollerTensioned = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onMeasureGripperChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onMeasureGripperChecked" );
    Q_UNUSED( state );
    ui->chkMeasureGripper->isChecked() ? wrTimingsTestResults_->testGripperFilmSelectorHeight = 1 : wrTimingsTestResults_->testGripperFilmSelectorHeight = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onMeasureGripperSlideBlocksChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onMeasureGripperSlideBlocksChecked" );
    Q_UNUSED( state );
    ui->chkMeasureGripperSlideBlocks->isChecked() ? wrTimingsTestResults_->testGripperSlideBlocks = 1 : wrTimingsTestResults_->testGripperSlideBlocks = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onElevatorHeightChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onElevatorHeightChecked" );
    Q_UNUSED( state );
    ui->chkElevatorHeight->isChecked() ? wrTimingsTestResults_->testElevatorHeight = 1 : wrTimingsTestResults_->testElevatorHeight = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onBrakeGapChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onBrakeGapChecked" );
    Q_UNUSED( state );
    ui->chkBrakeGap->isChecked() ? wrTimingsTestResults_->testBrakeGap = 1 : wrTimingsTestResults_->testBrakeGap = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onBrakeTorqueChecked(int state)
{
    //LOG_CLASS_INFO( "serviceSetTimings", "onBrakeTorqueChecked" );
    Q_UNUSED( state );
    ui->chkBrakeTorque->isChecked() ? wrTimingsTestResults_->testBrakeTorque = 1 : wrTimingsTestResults_->testBrakeTorque = 0;

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onFilmSelectorCleanChecked()", NULL );
    setStackTestLevel();
}

void serviceNewWrapperSetTimings::onActualtorsClicked()
{
    // Go to the actuators test screen
    LOG_CLASS_INFO("serviceSetTimings", "onActualtorsClicked");
    LOG_DEBUG( "onActualtorsClicked() - should be going to Actualtors Screen", NULL );

    wrTimingsData_->exitByMenu = false;
    session_->scaleConfig->internalConfig.isTimingsActive = true;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperActuatorDiag>();

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onActuatorsClicked()", NULL );
    setStackTestLevel();

    LOG_DEBUG( "onActualtorsClicked() - DONE", NULL );
}

void serviceNewWrapperSetTimings::onRunInClicked()
{
    LOG_CLASS_INFO("serviceSetTimings", "onRunInClicked");
    LOG_DEBUG( "onRunInClicked() - should be going to Run In Screen", NULL );

    wrTimingsData_->exitByMenu = false;
    session_->scaleConfig->internalConfig.isTimingsActive = true;
    session_->send( Wrapper, WrEvolStartMfgRunin );
    mainWindow_->switchToModeScreen<ServiceMode, operateNewWrapperRunIn>();

    //LOG_DEBUG( "serviceSetTimings CALL setStackTestLevel FROM onRunInClicked()", NULL );
    setStackTestLevel();

    LOG_DEBUG( "onRunInClicked() - DONE", NULL );
}

void serviceNewWrapperSetTimings::setStackTestLevel()
{
    //LOG_CLASS_INFO("serviceSetTimings", "setStackTestLevel");
    //LOG_DEBUG( "setStackTestLevel() - START", NULL );

    //LOG_DEBUG( "setStackTestLevel() ui->chkFilmLoaded->isChecked(): %d", ui->chkFilmLoaded->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkFilmSelectorClean->isChecked(): %d", ui->chkFilmSelectorClean->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkFilmRollerTensioned->isChecked(): %d", ui->chkFilmRollerTensioned->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkMeasureGripper->isChecked(): %d", ui->chkMeasureGripper->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkMeasureGripperSlideBlocks->isChecked(): %d", ui->chkMeasureGripperSlideBlocks->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkElevatorHeight->isChecked(): %d", ui->chkElevatorHeight->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkBrakeGap->isChecked(): %d", ui->chkBrakeGap->isChecked() );
    //LOG_DEBUG( "setStackTestLevel() ui->chkBrakeTorque->isChecked(): %d", ui->chkBrakeTorque->isChecked() );

    //LOG_DEBUG( "setStackTestLevel() actuatorTestState: %d", session_->scaleConfig->internalConfig.actuatorTestState );
    //LOG_DEBUG( "setStackTestLevel() runInTestState: %d", session_->scaleConfig->internalConfig.runInTestState );

    if( session_->scaleConfig->internalConfig.returningFromTabs )
    {
        //LOG_DEBUG( "setStackTestLevel() IF - set the stack to 1", NULL );
        ui->stkTimings->setCurrentIndex(1);                 // if returning from tabs, go to tray screen
        session_->scaleConfig->internalConfig.returningFromTabs = false;
    }
    else if( ui->chkFilmLoaded->isChecked() &&  ui->chkFilmSelectorClean->isChecked() &&  ui->chkFilmRollerTensioned->isChecked() &&  ui->chkMeasureGripper->isChecked()  &&
        ui->chkMeasureGripperSlideBlocks->isChecked() &&  ui->chkElevatorHeight->isChecked()  &&  ui->chkBrakeGap->isChecked()  &&  ui->chkBrakeTorque->isChecked() &&
        ( session_->scaleConfig->internalConfig.actuatorTestState ==  TimingsTestState::Test_Passed  ) &&
        ( session_->scaleConfig->internalConfig.runInTestState ==  TimingsTestState::Test_Passed  ) )
    {
        //LOG_DEBUG( "setStackTestLevel() ELSE IF - set the stack to 1", NULL );
        ui->stkTimings->setCurrentIndex(1);                 // IF ALL TESTS PASSED AND CHECKED - auto advance stack to Timings
    }
    else
    {
         //LOG_DEBUG( "setStackTestLevel() ELSE - set the stack to 0", NULL );
         ui->stkTimings->setCurrentIndex(0);
    }
    //LOG_DEBUG( "setStackTestLevel() - END", NULL );
}

void serviceNewWrapperSetTimings::move(bool up)
{
    Q_ASSERT(ui->tblWrapBoxList->rowCount() > 0);
    const int sourceRow = activeRow;
    const int destRow = (up ? sourceRow-1 : sourceRow+1);
    Q_ASSERT(destRow >= 0 && destRow < ui->tblWrapBoxList->rowCount());

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    //  Reset the active row
    activeRow = destRow;

    session_->send(Wrapper, WrEvolChangeTrayTypePrecedence, QString::number(sourceRow) + "," + (up ? "-1" : "1"));   // 9,235, sourceRow, whichWay
}

void serviceNewWrapperSetTimings::checkMoveAvailability()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::checkMoveAvailability()";
#endif
    if (viewCurrentTimings_)
	{
        bool canChangeData;
		if (canChangeArray.at(activeRow) == "1")
        {
		    wrTimingsData_->canEditTrayName = true;
        	canChangeData = true;
	    }
		else
        {
            wrTimingsData_->canEditTrayName = false;
        	canChangeData = false;
	    }

		if (activeRow == 0)
	    {
	        ui->butMoveUp->setEnabled(false);
	    }
	    else
	    {
            if (canChangeData)
            {
                ui->butMoveUp->setEnabled(true);
                if (canChangeArray.at(activeRow + 1) == "1")
                {
                    ui->butMoveDown->setEnabled(true);
                }
                else
                {
                    ui->butMoveDown->setEnabled(false);
                }
                ui->butDelete->setEnabled(true);
            }
            else
            {
                ui->butMoveUp->setEnabled(false);
                ui->butMoveDown->setEnabled(false);
                ui->butDelete->setEnabled(false);
            }
        }
    }
    else
    {
	    wrTimingsData_->canEditTrayName = false;
	}
}
// takes and returns the whole row
QList<QTableWidgetItem*> serviceNewWrapperSetTimings::takeRow(int row)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::takeRow(" + QString::number(row) + ")";
#endif
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < ui->tblWrapBoxList->columnCount(); ++col)
    {
        rowItems << ui->tblWrapBoxList->takeItem(row, col);
    }
    return rowItems;
}

// sets the whole row
void serviceNewWrapperSetTimings::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::setRow()";
#endif
    for (int col = 0; col < ui->tblWrapBoxList->columnCount(); ++col)
    {
        ui->tblWrapBoxList->setItem(row, col, rowItems.at(col));
    }
}

void serviceNewWrapperSetTimings::resetToFactoryDefaults()
{
    LOG_CLASS_INFO( "snwSetTimings", "resetToFactoryDefaults" );

    ui->butResetToDefault->setEnabled( false );
    ui->butViewDefaults->setEnabled( false );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::resetToFactoryDefaults()";
#endif
    activeRow = 0;
	LOG_TRACE( "Set activeRow = 0. Send 9719 message.", NULL );
    session_->send(Error, Wrapper, "9717,1");   //  HA-86

    if(enabledTimer)
    {
        enabledTimer->setSingleShot(true);
        enabledTimer->start(3000);      //3 sec timeout
    }
}

void serviceNewWrapperSetTimings::toggleViewDefaultSettings()
{
    LOG_CLASS_INFO( "snwSetTimings", "toggleViewDefaultSettings" );
    ui->butViewDefaults->setEnabled( false );
    ui->butResetToDefault->setEnabled( false );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::toggleViewDefaultSettings()";
#endif

	LOG_TRACE( "viewCurrentTimings_ = %u,  trayDeleted_ = %u", viewCurrentTimings_, trayDeleted_ );
	if (trayDeleted_)
	{
		//	Save the changes before we switch to View defaults.
		session_->send(Wrapper, WrEvolDonePackageTimings);      //  9,230
		sendDonePkgTimingsMsg_ = false;
		trayDeleted_ = false;
    }

    if (viewCurrentTimings_)            // if current switch to default
    {
        viewCurrentTimings_ = false;
        disableForViewDefault();
    }
    else                                // if default, switch to current
    {
        viewCurrentTimings_ = true;
        enableForViewCurrent();
    }
    wrTimingsData_->viewCurrentTimings = viewCurrentTimings_;
    currentOrDefaultSettings();
    activeRow = 0;
    session_->send(Wrapper, WrEvolLoadPackageTimings, (viewCurrentTimings_ ? "1" : "0"));   // 9,226, viewCurrentTimings_

    if( enabledTimer )
    {
        enabledTimer->setSingleShot(true);
        enabledTimer->start(3000);      //3 sec timeout
    }
}

void serviceNewWrapperSetTimings::onEnabledTimerTimeout()
{
    ui->butResetToDefault->setEnabled( true );
    ui->butViewDefaults->setEnabled( true );
    qDebug() << "serviceSetTimings::onEnabledTimerTimeout - set button enabled()";
}

void serviceNewWrapperSetTimings::setupWrapBoxParameters()
{
    LOG_CLASS_INFO( "snwSetTimings", "setupWrapBoxParameters" );
#ifdef USE_QDEBUG
    LOG_DEBUG( "setupWrapBoxParameters() START", NULL );
#endif
    int numOfParams( wrTimingsData_->wrapParameterList.count() );
    if ( numOfParams > 0)
	{
		LOG_TRACE( "There are no parameters! Exiting.", NULL );
        return;
	}

    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );      // 9,144
    LOG_INFO( "Wrapper model num %d", evolWrapperSystemInformation->awsModelNumber() );

	QString name;
    WrapParameter::wrTimingsFieldType fieldType;

    QString decreaseName;
    QString increaseName;

    bool addParam = true;
    for (int i = 0; i < MAX_PARAMS; i++)
    {
        switch (i)
        {
            case WrapParameter::PERCENT_STRETCH_CLAMP_ON:
            {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Side to side pre-stretch");
                else if ( DomEvolWrapperSystemInformation::AWS1BWRL <= evolWrapperSystemInformation->awsModelNumber() )
                    name = tr("Clamp on position");
                else
                    name = tr("Percent net stretch");

                decreaseName = tr("Lower");
                increaseName = tr("Higher");

                fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::KNIFE_HEIGHT:
	        {
	            name = tr("Knife fire height");
	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::CLAMP_HEIGHT:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Side to side wrap tension");
                else
                    name = tr("Clamp open height");

                decreaseName = tr("Earlier");
                increaseName = tr("Later");

	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::DISTANCE:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Package position on elevator");
                else
                    name = tr("Distance");

                decreaseName = tr("Forward");
                increaseName = tr("Backward");

                fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::FORCE_OUTSIDE_ELEVATOR:
	        {
                if( evolWrapperSystemInformation->isNextGenModel() )
                {
                    name = tr( "Secondary label offset" );                      // was Leading edge label offse

                    decreaseName = tr( "Left" );
                    increaseName = tr( "Right" );

                    fieldType = WrapParameter::typeSignedInt;
                }
                else
                {
                    name = tr("Force outside elevator up");

                    fieldType = WrapParameter::typeForceOutsideElevatorSelect;
                }

                break;
            }

            case WrapParameter::GRIPPER_ADJUST:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Front to back wrap tension");
                else
                    name = tr("Gripper pull adjust");

                decreaseName = tr("Lower");
                increaseName = tr("Higher");

                fieldType = WrapParameter::typeSignedInt;
                break;
            }
            case WrapParameter::FRONT_UF_ADJUST:
	        {
                // SOF-870
                if ( evolWrapperSystemInformation->isNextGenModel() )
                    name = tr("Primary label offset");                          // was Trailing edge label offset
                else
                    name = tr("Front underfolder adjust");

                decreaseName = tr("Left");
                increaseName = tr("Right");

                fieldType = WrapParameter::typeSignedInt;
                break;
            }
            case WrapParameter::WRAP_SPEED:
	        {
	            name = tr("Wrap speed");
                decreaseName = tr("Slower");
                increaseName = tr("Faster");
                fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::DISCHARGE_SPEED:
	        {
	            name = tr("Discharge speed");
                decreaseName = tr("Slower");
                increaseName = tr("Faster");
                fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::SIDE_UF_ADJUST:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Side folder travel");
                else
                    name = tr("Side underfolder adjust");

                decreaseName = tr("Less");
                increaseName = tr("More");

	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::HEIGHT_RESTRICT:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Gripper relax");
                else
                    name = tr("Height restrict");

                decreaseName = tr("Less");
                increaseName = tr("More");

                fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::SIDE_UF_RETURN:
	        {
                // no longer used
	            name = tr("Side underfolder return home");
	            fieldType = WrapParameter::typeSideUnderfolderReturnHomeStates;
                break;
            }
            case WrapParameter::UNUSED_12:
	        {
                // no longer used
	            name = tr("UNUSED PARAMETER");
	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::SIDE_CLAMP_GAP:
	        {
                // no longer used
	            name = tr("Side clamp gap");
	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::FILM_ROLL_TO_USE:
	        {
                name = tr("Film width to use");
	            fieldType = WrapParameter::typeFilmRollToUse;
                break;
            }
            case WrapParameter::REAR_UF_TRIP_DELAY:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Rear underfolder travel");
                else
                    name = tr("Rear underfolder trip delay");

                decreaseName = tr("Less");
                increaseName = tr("More");

	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::BRAKE_TIMING:
	        {
                // NGW-140
                if ( evolWrapperSystemInformation->isClosedLoop() )
                    name = tr("Film pull length");
                else
                    name = tr("Brake timing");

                decreaseName = tr("Less");
                increaseName = tr("More");

	            fieldType = WrapParameter::typeUnsignedInt;
                break;
            }
            case WrapParameter::OUTSIDE_GRIPPER_RELEASE:
	        {
	            name = tr("Outside gripper release");
	            fieldType = WrapParameter::typeOutsideGripperReleaseStates;
                break;
            }
            case WrapParameter::INSIDE_GRIPPER_RELEASE:
	        {
	            name = tr("Inside Gripper Release");
	            fieldType = WrapParameter::typeUnsignedInt;
                break;
	        }

            case WrapParameter::PRIMARY_LABEL_VERT_OFFSET:
            {
                if( session_->scaleConfig->isWrapperTwoPrinter() )
                {
                    //LOG_INFO( "SetTimings::setupWrapBoxParameters case PRIMARY_LABEL_VERT_OFFSET:", NULL );

                    name = tr( "Primary label vertical offset" );

                    increaseName = tr( "Up" );
                    decreaseName = tr( "Down" );

                    fieldType = WrapParameter::typeSignedInt;

                }
                else
                    addParam = false;

                break;
            }

            case WrapParameter::SECONDARY_LABEL_VERT_OFFSET:
            {
                if( session_->scaleConfig->isWrapperTwoPrinter() )
                {
                    //LOG_INFO( "SetTimings::setupWrapBoxParameters case SECONDARY_LABEL_VERT_OFFSET:", NULL );

                    name = tr( "Secondary label vertical offset" );

                    increaseName = tr( "Up" );
                    decreaseName = tr( "Down" );

                    fieldType = WrapParameter::typeSignedInt;
                }
                else
                    addParam = false;

                break;
            }
        }

        // The PRIMARY_LABEL_VERT_OFFSET might add a blank field to the screen or do something unpredictable.  We will have to see.
        // I addded two new ones to the end. but the one who was at the end, it's not even sent by the Application to UI. So I can't add it here either.
        if( addParam )
        {
            WrapParameter *wrParam = new WrapParameter(name, fieldType);
            wrParam->setDecreaseName(decreaseName);
            wrParam->setIncreaseName(increaseName);
            wrTimingsData_->wrapParameterList.append(wrParam);
            //LOG_DEBUG( "SetTimings::setupWrapBoxParameters wrapParameterList name = %s and append(%d)", qPrintable(name), wrParam->getData() );
        }
        else
            addParam = true;            // reset it for the next loop
    }

	LOG_TRACE( "Done", NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperSetTimings::onScreenChanging()
{
    LOG_CLASS_INFO( "snwSetTimings", "onScreenChanging" );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::onScreenChanging()";
#endif

    LOG_TRACE( "onScreenChanging() Start", NULL );

    isExitingTimings_ = true;

    internalConfigData_->timingsUpdatesInProgress = false;
    internalConfigData_->timingsUpdatesAreComplete = false;
    internalConfigData_->dialogSuppressed = false;

    LOG_TRACE( "Calling saveDataOnExit()", NULL );
    saveDataOnExit();

    if (wrTimingsData_->exitByMenu)
    {
        LOG_TRACE( "wrTimingsData_->exitByMenu = %u,  viewCurrentTimings_ = %u", wrTimingsData_->exitByMenu, viewCurrentTimings_ );

        if (!viewCurrentTimings_)
        {
            LOG_TRACE( "if NOT viewCurrentTimings_", NULL);

            ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
            modifyPaletteBackground setCurrentTimingsBackground( ( QApplication::palette() ).background() );
            setCurrentTimingsBackground( serviceMode );
        }

        viewCurrentTimings_ = true;     // For if/when we return to Wrapper timings.
        wrTimingsData_->precedence = 0;

        if (wrTimingsData_->trayList != NULL)
        {
            LOG_TRACE( "if wrTimingsData_->trayList", NULL);

            delete wrTimingsData_->trayList;
            wrTimingsData_->trayList = NULL;
            LOG_TRACE( "if wrTimingsData_->trayList DONE with delete", NULL);
        }
        wrTimingsData_->loadTrays = true;
    }

    wrTimingsData_->viewCurrentTimings = viewCurrentTimings_;

    //  TODO:   We need to prompt them to see if they want to save the timings.


    // SOF-1142 / SOF-1279
    if ( wrTimingsData_->exitByMenu )
    {
        LOG_TRACE( "if wrTimingsData_->exitByMenu", NULL);

        // Need to check if the dialog "Wrapper is updating ..." is displayed
        startScreenExitTimer = new QTimer( this );
        startScreenExitTimer->setSingleShot( true );
        startScreenExitTimer->setInterval( 500 );
        okToCheckForWaitMessage = false;

        connect( startScreenExitTimer, SIGNAL( timeout() ), SLOT( chkForWaitMessage() ) );

        startScreenExitTimer->start();

        //-----------------------------------------------------------------------------------------------------
        // First need to wait for the "Wrapper is updating ..." dialog to be displayed ... if it is going to be
        //-----------------------------------------------------------------------------------------------------
        LOG_TRACE( "before while okToCheckForWaitMessage", NULL);

        while ( !okToCheckForWaitMessage )
        {
            sleepFor( 50 ); // so this while-loop doesn't peg the CPU
        }

        startScreenExitTimer->stop();

        LOG_TRACE( "after while okToCheckForWaitMessage", NULL);

        if( internalConfigData_->timingsUpdatesInProgress == true )
        {
            LOG_TRACE( "if internalConfigData_->timingsUpdatesInProgress", NULL);

            //----------------------------------------------------------
            // The dialog for "Wrapper is updating ..." is displayed.
            // Now wait until it is dismissed before we exit the screen.
            //----------------------------------------------------------

            LOG_DEBUG ( "Waiting for \"Wrapper is updating ...\" to be dismissed", NULL );

            while( internalConfigData_->timingsUpdatesAreComplete == false )
            {
                sleepFor( 500 );
            }

            LOG_DEBUG ( "Received dismiss for \"Wrapper is updating ...\"", NULL );
        }
        else
            LOG_TRACE( "else internalConfigData_->timingsUpdatesInProgress", NULL);
    }

    internalConfigData_->messagesSuppressed = false;
    internalConfigData_->timingsUpdatesInProgress = false;
    internalConfigData_->timingsUpdatesAreComplete = false;
    internalConfigData_->dialogSuppressed = false;
    isExitingTimings_ = false;

    LOG_TRACE( "onScreenChanging() END", NULL );

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperSetTimings::chkForWaitMessage()
{
    okToCheckForWaitMessage = true;
}

void serviceNewWrapperSetTimings::sleepFor( int ms ) const
{
    // amount of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

// SOF-1142 / SOF-1279
bool serviceNewWrapperSetTimings::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    LOG_CLASS_INFO( "snwSetTimings", "isErrorMessageHandler" );

    if( !isExitingTimings_ )
        return false;

    if ( ( Error == errorMsg[0].toInt() ) &&
         ( Wrapper == errorMsg[1].toInt() ) &&
         ( 9705 == errorMsg[2].toInt() ) &&
         ( 3 == errorMsg[3].toInt() ) )
    {
        //------------------------------------------------------------------
        // Message that "Wrapper is updating ..." is displayed: 103,9,9705,3
        //------------------------------------------------------------------

        internalConfigData_->timingsUpdatesInProgress = true;
        internalConfigData_->dialogSuppressed = false;

        LOG_DEBUG("isErrorMessageHandler(): Received 103,9,9705 message", NULL );

        if( mainWindow_->isDialogOpen() )
        {
            //-------------------------------------------------------------------
            // When going to another screen via the menu, the UI was crashing due
            // to the message dialog, menu and processEvents() in the sleepFor()
            // colliding.  So in this case if the menu is open, indicated by
            // isDialogOpen(), we suppress the message dialog.
            //-------------------------------------------------------------------
            internalConfigData_->dialogSuppressed = true;

            LOG_DEBUG("isErrorMessageHandler(): Suppressed dialog", NULL );

            return true;
        }
    }
    else if ( ( Error == errorMsg[0].toInt() ) &&
              ( ErrRemoveAnyModalDialogDevice == errorMsg[1].toInt() ) &&
              ( 9705 == errorMsg[2].toInt() ) &&
              ( 3 == errorMsg[3].toInt() ) )
    {
        //--------------------------------------------------------------------
        // Message that "Wrapper is updating ..." is dismissed: 103,255,9705,3
        //--------------------------------------------------------------------

        internalConfigData_->timingsUpdatesAreComplete = true;

        LOG_DEBUG( "isErrorMessageHandler(): Received 103,255,9705 message", NULL );

        if( internalConfigData_->dialogSuppressed )
            return true;
    }

    return _Mybase::isErrorMessageHandler( errorMsg );
}

void serviceNewWrapperSetTimings::processReturnedWrapperErrors(const SessionMessage& msg)
{
    returnedErrorsWrapperDev m( msg );

#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::processReturnedWrapperErrors():  errCode = " << m.errorCode();
#endif
    if (9717 == m.errorCode())
    {
        bool okPressed( 0 < m.errorData2() );
        if (okPressed)
            continueResetToFactoryDefaults();
    }
    else if (9708 == m.errorCode())
    {
        bool okPressed( 0 < m.errorData2() );
        if (okPressed)
            continueRemoveTray();
    }
}

void serviceNewWrapperSetTimings::continueResetToFactoryDefaults()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::continueResetToFactoryDefaults()";
#endif
    session_->send(Wrapper, WrEvolResetDefaultPackageTimings);              		// 9,229
}

void serviceNewWrapperSetTimings::continueRemoveTray()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::continueRemoveTray()";
#endif
    session_->send(Wrapper, WrEvolDeleteTrayType, QString::number(activeRow));   	// 9,238,tray
	trayDeleted_ = true;
}

void serviceNewWrapperSetTimings::currentOrDefaultSettings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceSetTimings::currentOrDefaultSettings()";
#endif
    if (viewCurrentTimings_)
    {
        ui->butViewDefaults->setText( tr( "View default"));
        ui->butEdit->setText( tr("Edit") );
    }
    else
    {
        ui->butViewDefaults->setText( tr( "View current"));
        ui->butEdit->setText( tr("View") );
    }
}

void serviceNewWrapperSetTimings::disableForViewDefault()
{
    ui->butEdit->setEnabled(true);
    ui->butDelete->setEnabled(false);
    ui->butAutoConfig->setEnabled(false);
    ui->butAutoDetect->setEnabled(false);
    ui->butOneParameter->setEnabled(true);
    ui->butMoveDown->setEnabled(true);
}

void serviceNewWrapperSetTimings::enableForViewCurrent()
{
    ui->butEdit->setEnabled(true);
    ui->butDelete->setEnabled(true);
    ui->butAutoConfig->setEnabled(true);
    ui->butAutoDetect->setEnabled(true);
    ui->butOneParameter->setEnabled(true);
    ui->butMoveDown->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    LOG_CLASS_INFO( "snwSetTimings", "onSessionMessage" );

    if ((device != 145) && (device != 170) && !((device == 9) && (event == 51)))
	{
#ifdef USE_QDEBUG
        qDebug() << "serviceSetTimings::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
#endif
		LOG_TRACE( "device = %d,  event = %d", device, event );
	}

    if (( Wrapper == device ) && ( WrEvolWriteAllTrayTypes == event ))
    {
        // Received 9,237 - Update the Wrap box / Tray list
        if (wrTimingsData_->trayList != NULL)
        {
            delete wrTimingsData_->trayList;
            wrTimingsData_->trayList = NULL;
        }
        wrTimingsData_->trayList = new DomNewWrapperAllTrays(msg);
        buildTrayList();
		LOG_TRACE( "Done building the tray list.", NULL );
    }
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
	{
	    DomEvolWrapperSystemInformation m( msg );
        wrTimingsData_->wrapperModelNum = m.awsModelNumber();
        if ((wrTimingsData_->wrapperModelNum >= DomEvolWrapperSystemInformation::AWS1BWRL) && (wrTimingsData_->wrapperModelNum <= DomEvolWrapperSystemInformation::AWS2LR))
		{
            QString ewsParam = tr("Clamp on position");
			if (wrTimingsData_->wrapParameterList.count() > 0)
	            wrTimingsData_->wrapParameterList[0]->setName(ewsParam);
#ifdef USE_QDEBUG
    		else
                qDebug() << "serviceSetTimings: Cannot change param name to " << ewsParam;
#endif
		}
	}
    else if (( Error == device ) && ( Wrapper == event ))
    {
        processReturnedWrapperErrors(msg);
    }
    else
    {
        requestingData = false;
    }
}
