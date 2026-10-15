/* @file serviceNewWrapperEditBox.cpp
 * Allows Service technicians to modify wrap box timings.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 */
#if 0
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
#include "serviceNewWrapperAdvancedTimings.h"
#include "serviceNewWrapperOtherTimings.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/servicemode.h"

#include "../dialogs/messageboxdialog.h"
#include "../dialogs/numericinputboxdialog.h"
#include "../dialogs/wrapperforcetraysizedialog.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QSignalMapper>
#include <palette.h>
#include <backgroundwidgetstyles.h>
#include <numericlineedit.h>

#define xstr(s) str(s)
#define str(s) #s


namespace hobart{
    class numericLineEdit;
}

//		pageMode = 1	//	View/Edit a tray
//		pageMode = 2 	//	Auto-configure a new tray
//		pageMode = 3	//	Auto detect a tray
//		pageMode = 4	//	New tray with a PLUException


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

serviceNewWrapperEditBox::serviceNewWrapperEditBox(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperEditBox),
    wrTimingsData_(NULL),
    leadingEdge(false),
    cycleWrapperTestLabel_( NULL )
{
    LOG_CLASS_INFO( "snwEditBox", "snwEditBox" );

    setObjectName( "serviceNewWrapperEditBox" );

//	Retrieve config object
    if ( mainWindow_->isServiceMode() )
        wrTimingsData_ = &dynamic_cast<ServiceMode*>( mode_ )->wrTimingsData_;
    else
        wrTimingsData_ = new WrapperTimingsData;

    ui->setupUi(this);
    scaleScreen();

    if ( !isUsingSimplifiedTimings() )
    {
        // 2P and 4.2.0-5.2.0 merge notes.  
        // 2P branch changes this row value from 6 to 7
        // 4.2.0-5.2.0 changed this row value from 7 to 6.
        // so I have no way of knowing which is going to be
        // actually correct until we test. Not sure if 2P
        // tested AWS, I know 4.2.0-5.2.0 did. So I did that one
        ui->tblBasicTimingsList->setRowCount( 6 );//AWS style timings
    }
    else
    {
        // followed the  4.2.0-5.2.0 branch here too since it had early 2p
        // the 2P nums are as follows; 7, 15:14 (no else so 12:11 good ), 
        // then 12:11 for final else
        if ( isNGWWrapper() )
        {
            if(session_->scaleConfig->isWrapperTwoPrinter())
                ui->tblBasicTimingsList->setRowCount( isDualRollWrapper() ? 15 : 14 );
            else
                ui->tblBasicTimingsList->setRowCount( isDualRollWrapper() ? 12 : 11 );
        }
        else
            ui->tblBasicTimingsList->setRowCount( isDualRollWrapper() ? 11 : 10 );      // why this not 12:11 as previously implemented?
    }

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    setHighlight( ui->tblBasicTimingsList );

    initialize();
    translate();

    if ( timerAutoConfigTrayNameEntry_ && (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) )	//&& firstPass )
    {
        LOG_TRACE( "Set up timerAutoConfigTrayNameEntry_", NULL );
        //	HTA-79:  Schedule a timer to activate Tray name entry
        timerAutoConfigTrayNameEntry_->setSingleShot( true );
        timerAutoConfigTrayNameEntry_->start( 0 );
    }
}

serviceNewWrapperEditBox::~serviceNewWrapperEditBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::~serviceNewWrapperEditBox()";
#endif
    if ( !mainWindow_->isServiceMode() )
        delete wrTimingsData_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::initialize()
{
    LOG_CLASS_INFO( "snwEditBox", "initialize" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::initialize()";
#endif
    firstPass = true;
    gotNewTrayName_ = false;
    currentTrayName_ = wrTimingsData_->trayName;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    boxCount = 0;
    wrTimingsData_->exitByMenu = true;      //  We'll set this to false when we change to another Timings page.
    clampOnRow = 0;

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;
    heightBoxSelected_ = wrTimingsData_->heightBoxSelected;

    le_PercentStretchClampOn_ = NULL;
    heightBox_ = NULL;
    
    saveAnyChanges_ = false;

    cycleButtonStyle = "";
    cycleWrapperInProgress_ = false;

    // SOF-2550
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
        ui->butCycleWrapperLeading->hide();
        ui->butCycleWrapperTrailing->hide();
    }
    else
    {
        // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
        ui->butCycleWrapperLeading->show();
        ui->butCycleWrapperTrailing->show();
    }

    //Before we start, lets hide the other timing and advanced timings pages for the AWS Enhanced
    if ( isUsingSimplifiedTimings() )
    {
        ui->butOtherTimings->hide();
        ui->butAdvancedTimings->hide();
    }
    else
    {
        // SOF-2550 - added in 2P
        if( false == session_->scaleConfig->isWrapperTwoPrinter() )
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->show();
            ui->butCycleWrapperTrailing->show();
        }
    }

    if (( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || ( wrTimingsData_->pageMode == WrapperTimingsData::pmPLUException))
    {
        if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
        {
            //  Hide these items since they can't be used in this mode.
            ui->butSetMin->hide();
            ui->butSetMinAndMax->hide();
            ui->butSetMax->hide();
            ui->butNewHeightBox->hide();
            ui->butDeleteHeightBox->hide();

            ui->le_MinHeight->hide();
            ui->le_MaxHeight->hide();

            ui->lblHeightBox->hide();
            ui->cbo_HeightBox->hide();

            ui->label_trayFound->hide();

            //  Disable these fields until a tray name is entered and they cycle a package.
            ui->le_MinLength->setEnabled(false);
            ui->le_MaxLength->setEnabled(false);
            ui->le_MinWidth->setEnabled(false);
            ui->le_MaxWidth->setEnabled(false);
            ui->le_MaxLip->setEnabled(false);

            hideShowForNewTray(true);       //  Hide all the other items until the tray name is entered.
        }
        //  Hide the table and buttons below it.
        ui->tblBasicTimingsList->hide();
        ui->butCycleWrapper->hide();
        ui->butOtherTimings->hide();
        ui->butAdvancedTimings->hide();
        ui->butSaveChanges->hide();

        // SOF-2550
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->hide();
            ui->butCycleWrapperTrailing->hide();
        }
        else
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->show();
            ui->butCycleWrapperTrailing->show();
        }
    }

    if ( wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        ui->butAutoConfig->hide();

        ui->label_PromptTrayName->hide();

        if (viewCurrentTimings_)
        {
            connect(ui->butSetMin,    				SIGNAL(clicked()),              	this,       SLOT(setMinValues()));

            connect(ui->butSetMinAndMax,    		SIGNAL(clicked()),              	this,       SLOT(setMinMaxValues()));

            connect(ui->butSetMax,    				SIGNAL(clicked()),      			this,       SLOT(setMaxValues()));

            connect(ui->butNewHeightBox,            SIGNAL(clicked()),                  this,       SLOT(newHeightBox()));

            connect(ui->cbo_AvailableHeightBoxes,   SIGNAL(currentIndexChanged(int)),   this,       SLOT(addNewHeightBox()));

            connect(ui->butCancelNewHeightBox,      SIGNAL(clicked()),                  this,       SLOT(cancelNewHeightBox()));

            connect(ui->butDeleteHeightBox,			SIGNAL(clicked()),              	this,       SLOT(deleteSelectedHeight()));

            connect(ui->butSaveChanges,    			SIGNAL(clicked()),      			this,       SLOT(saveChanges()));
        }
        else
        {
            ui->butSetMin->hide();
            ui->butSetMinAndMax->hide();
            ui->butSetMax->hide();
            ui->butNewHeightBox->hide();
            ui->butDeleteHeightBox->hide();
            ui->butSaveChanges->hide();
        }

        connect(ui->cbo_HeightBox,              SIGNAL(currentIndexChanged(int)),   this,       SLOT(requestHeightBox()));

        connect(ui->butCycleWrapper,			SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));		//	HA-80

        // SOF-2550
        connect(ui->butCycleWrapperLeading,     SIGNAL(clicked()),                  this,       SLOT(cycleWrapperLeadingEdge() ) );
        connect(ui->butCycleWrapperTrailing,	SIGNAL(clicked()),                  this,       SLOT(cycleWrapperTrailingEdge() ) );

        connect(ui->tblBasicTimingsList,        SIGNAL(cellClicked(int,int)),       this,       SLOT(onCellClicked(int,int))); // SOF-204

#ifndef TEST_WRAPPER_OPERATION
        if ( !mainWindow_->isPrimarySessionType() || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect) )
        {
            ui->butCycleWrapper->setEnabled(false);
        }
#endif
    }

    if (( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect) || ( wrTimingsData_->canEditTrayName == false ))
        ui->le_TrayName->setEnabled(false);
    else
        ui->le_TrayName->setEnabled(true);
    ui->le_PLUNumber->setEnabled(false);    //  Allow entry only upon selecting PLU Exception in Set timings page.

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect)
    {
        ui->tblBasicTimingsList->hide();
    }

    ui->butPrint->setVisible( isUsingSimplifiedTimings() );


    connect(ui->butOtherTimings,            	SIGNAL(clicked()),                  this,       SLOT(goToOtherTimings()));

    connect(ui->butAdvancedTimings,         	SIGNAL(clicked()),                  this,       SLOT(goToAdvancedTimings()));

    connect(ui->butCancel,                  	SIGNAL(clicked()),                  this,       SLOT(cancelChanges()));

    connect(ui->butDone,                    	SIGNAL(clicked()),                  this,       SLOT(doneWithChanges()));

    connect(ui->butPrint,                       SIGNAL(clicked()),      			this,       SLOT(printTimings()));
    connect(ui->butPromptTraySize,              SIGNAL(clicked()),      			this,       SLOT(promptForcedTraySize()));

    connect(ui->le_MinLength,             SIGNAL(editingFinished()),          this,       SLOT(updateMinLength()));
    connect(ui->le_MaxLength,             SIGNAL(editingFinished()),          this,       SLOT(updateMaxLength()));
    connect(ui->le_MinWidth,              SIGNAL(editingFinished()),          this,       SLOT(updateMinWidth()));
    connect(ui->le_MaxWidth,              SIGNAL(editingFinished()),          this,       SLOT(updateMaxWidth()));
    connect(ui->le_MaxLip,                SIGNAL(editingFinished()),          this,       SLOT(updateMaxLip()));

    connect(ui->le_TrayName,              SIGNAL(editingFinished()),          this,       SLOT(updateTrayName()));

    if (viewCurrentTimings_)
    {
        ui->le_MinLength->setValidator( lengthValidator_ = new QIntValidator(-5, WB_MAX_LENGTH, this) );
        ui->le_MinLength->setDecimalPlaces(1);
        ui->le_MinLength->setInputTruncation( true );
        ui->le_MinLength->setInputWrapping( true );

        ui->le_MaxLength->setValidator( lengthValidator_ );
        ui->le_MaxLength->setDecimalPlaces(1);
        ui->le_MaxLength->setInputTruncation( true );
        ui->le_MaxLength->setInputWrapping(true);

        ui->le_MinWidth->setValidator( widthValidator_ = new QIntValidator(-5, WB_MAX_WIDTH, this) );
        ui->le_MinWidth->setDecimalPlaces(1);
        ui->le_MinWidth->setInputTruncation( true );
        ui->le_MinWidth->setInputWrapping(true);

        ui->le_MaxWidth->setValidator( widthValidator_ );
        ui->le_MaxWidth->setDecimalPlaces(1);
        ui->le_MaxWidth->setInputTruncation( true );
        ui->le_MaxWidth->setInputWrapping(true);

        ui->le_MaxLip->setValidator( lipValidator_ = new QIntValidator(-5, WB_MAX_LIP, this) );
        ui->le_MaxLip->setDecimalPlaces(1);
        ui->le_MaxLip->setInputTruncation( true );
        ui->le_MaxLip->setInputWrapping(true);

        ui->le_MinLength->installEventFilter( this );
        ui->le_MaxLength->installEventFilter( this );
        ui->le_MinWidth->installEventFilter( this );
        ui->le_MaxWidth->installEventFilter( this );
        ui->le_MaxLip->installEventFilter( this );
    }
    else
    {
        ui->le_MinLength->setEnabled(false);
        ui->le_MaxLength->setEnabled(false);
        ui->le_MinWidth->setEnabled(false);
        ui->le_MaxWidth->setEnabled(false);
        ui->le_MaxLip->setEnabled(false);
    }

//    if ( wrTimingsData_->canEditTrayName == 0 )
//    {
//        ui->le_TrayName->setEnabled(false);
//        ui->le_PLUNumber->setEnabled(false);
//    }

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
    ui->cbo_AllHeightBoxes->clear();
    ui->cbo_AllHeightBoxes->addItem("0.5","0.5");
    ui->cbo_AllHeightBoxes->addItem("0.7","0.7");
    ui->cbo_AllHeightBoxes->addItem("1.0","1.0");
    ui->cbo_AllHeightBoxes->addItem("1.2","1.2");
    ui->cbo_AllHeightBoxes->addItem("1.5","1.5");
    ui->cbo_AllHeightBoxes->addItem("1.7","1.7");
    ui->cbo_AllHeightBoxes->addItem("2.0","2.0");
    ui->cbo_AllHeightBoxes->addItem("2.2","2.2");
    ui->cbo_AllHeightBoxes->addItem("2.5","2.5");
    ui->cbo_AllHeightBoxes->addItem("3.0","3.0");
    ui->cbo_AllHeightBoxes->addItem("3.5","3.5");
    ui->cbo_AllHeightBoxes->addItem("4.0","4.0");
    ui->cbo_AllHeightBoxes->addItem("4.5","4.5");
    ui->cbo_AllHeightBoxes->addItem("5.0","5.0");
    ui->cbo_AllHeightBoxes->addItem("6.0","6.0");

    if ( wrTimingsData_->updateActualPkg )
    {
        ui->label_trayFound->setText(wrTimingsData_->cycledTrayName);
    }
    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmPLUException )
    {
        //  Show the tray name for the PLU exception to be entered.
        ui->le_TrayName->setText(wrTimingsData_->trayName);
    }

    ui->cbo_AllHeightBoxes->hide();
    ui->cbo_AvailableHeightBoxes->hide();
    ui->butCancelNewHeightBox->hide();
    ui->lblAddHeightBox->hide();

    buildParamTable();      // added 13 items to the table initially set to 15, if 1P system

    update();

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmViewEdit)
    {
        session_->send(Wrapper, WrEvolReadWrapBox, QString::number( activeRow ));              // 9,240,precedence
    }
    else if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
    {
        //  Disable PLUException
        //  Create a new Tray name
        connect(ui->butAutoConfig,            SIGNAL(clicked()),                  this,       SLOT(autoConfigureTray()));
    }
    else if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect )
    {
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::initialize():  session_->send(Wrapper, WrEvolAutoDetectTrayType)";
#endif
        session_->send(Wrapper, WrEvolAutoDetectTrayType);              // 9,232
    }

    LOG_TRACE( "wrTimingsData_->heightBoxSelected = %s", qPrintable(wrTimingsData_->heightBoxSelected) );
    if ( wrTimingsData_->heightBoxSelected != "" )
    {
        QString msg2 = QString::number( activeRow ) + "," + wrTimingsData_->heightBoxSelected;
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::initialize():  session_->send(Wrapper, WrEvolReadHeightBox, " << msg2 << ")";
#endif
        session_->send(Wrapper, WrEvolReadHeightBox, msg2);              // 9,246,precedence,heightBox
    }

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmPLUException )
    {
        //  Force entry of a PLU number for the PLU Exception
        saveAnyChanges_ = true;
        promptPLUException();
    }
    cycleButtonStyle = ui->butCycleWrapper->styleSheet();	//	Get the original style of this button.

    if ( wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure )		//	HTA-79
    {
        LOG_TRACE( "Create timerAutoConfigTrayNameEntry_", NULL );
        timerAutoConfigTrayNameEntry_ = new QTimer( this );
        connect( timerAutoConfigTrayNameEntry_, SIGNAL( timeout() ), SLOT( activateTrayName() ) );
    }
    else
    {
        LOG_TRACE( "Did NOT create timerAutoConfigTrayNameEntry_", NULL );
        timerAutoConfigTrayNameEntry_ = NULL;
    }

    cycleWrapperTestLabel_ = new QTimer( this );
    connect( cycleWrapperTestLabel_, SIGNAL( timeout() ), SLOT( cycleWrapperSetupTime() ) );

    // SOF-2808: Need to hide the Print button when in Auto Config mode
    if( WrapperTimingsData::pmAutoConfigure == wrTimingsData_->pageMode )
        ui->butPrint->hide();

    pLabelSize_ = "0";
    sLabelSize_ = "0";

    session_->send( Printer, PrReqStockMetrics, QString::number( 0 ) );         // 7,40,0 - send in case we decide to cycle wrapper, 0 = Primary Printer
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

    update();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperEditBox::writeHeightBox()
{
    LOG_CLASS_INFO( "snwEditBox", "writeHeightBox" );

    //LOG_INFO( "writeHeightBox() START", NULL );
    //LOG_INFO( "writeHeightBox() Num ofParams %d", wrTimingsData_->wrapParameterList.count() );

    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        if( heightBox_ )//HA-127 this variable wasn't initialized when attempting to send the message
        {
            int numOfParams = wrTimingsData_->wrapParameterList.count();
            QString msg2 = QString::number(activeRow);
            msg2 += "," + heightBox_->maxHeight();
            msg2 += "," + QString::number(numOfParams);
            msg2 += ",\"";

            for (int j = 0; j < numOfParams; j++)
            {
                if (j > 0)
                    msg2 += ",";

                int value( wrTimingsData_->wrapParameterList[j]->getData() );
                // LOG_INFO( "writeHeightBox() for loop value[%d]: %d", j, value);
                // Right now, the 2 new values for 2P, Pri and Sec VERT
                // offsets are saving, so I'm not going to touch the 4.2.0 mods made.
                if ( isNGWWrapper() && ( ( WrapParameter::FRONT_UF_ADJUST == j ) || ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == j ) ) )
                {
                    if( ( value > 0 ) && ( value <= 100 )  )
                    {
                        value += 100;
                    }
                }

                //                                     [2]Side to Side wrap tension(2)            [5]Front to Back Wrap Tension(5)
                if ( isNGWWrapper() && ( ( WrapParameter::CLAMP_HEIGHT == j ) || ( WrapParameter::GRIPPER_ADJUST == j ) ) )
                {
                    if( ( value >= -5 ) && ( value < 0 ) )
                        value = ( ( value * -1 ) + 100 );
                }

                if( ( WrapParameter::PRIMARY_LABEL_VERT_OFFSET == j ) || ( WrapParameter::SECONDARY_LABEL_VERT_OFFSET == j ) )
                {
                    if( isNGWWrapper() && session_->scaleConfig->isWrapperTwoPrinter() )
                        msg2 += QString::number(value);     // so dont send the current last 2 params if we are not 2P. Could of sent them as the same
                }
                else
                    msg2 += QString::number(value);
            }

            msg2 += "\"";

            //LOG_INFO( "writeHeightBox() sending msg2: %s", qPrintable(msg2) );

            session_->send(Wrapper, WrEvolWriteHeightBox, msg2);              // 9,247,precedence,maxHeight,heightBoxes list
        }
    }

    //LOG_INFO( "writeHeightBox() END", NULL );
}

void serviceNewWrapperEditBox::writeWrapBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::writeWrapBox()";
#endif

    // SOF-2734
    QString trayNameQuotedForCSV( ui->le_TrayName->text() );
    trayNameQuotedForCSV.replace( "\"", "\"\"" );
    trayNameQuotedForCSV.prepend( "\"" );
    trayNameQuotedForCSV.append( "\"" );

    QString msg2 = QString::number(activeRow);
    msg2 += "," + trayNameQuotedForCSV;
    msg2 += "," + ui->le_PLUNumber->text();
    msg2 += "," + ui->le_MinLength->text();
    msg2 += "," + ui->le_MaxLength->text();
    msg2 += "," + ui->le_MinWidth->text();
    msg2 += "," + ui->le_MaxWidth->text();
    msg2 += "," + ui->le_MaxLip->text();
    msg2 += "," + QString::number(ui->cbo_HeightBox->count());
    msg2 += ",\"";
    for (int j = 0; j < ui->cbo_HeightBox->count(); j++)
    {
        if (j > 0)
            msg2 += ",";
        msg2 += ui->cbo_HeightBox->itemData(j).toString();
    }
    msg2 += "\"";

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::writeWrapBox():  session_->send(Wrapper, WrEvolWriteWrapBox, " << msg2 << ")";
#endif

    session_->send(Wrapper, WrEvolWriteWrapBox, msg2);              // 9,241,precedence,trayName,...
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperEditBox::makeValidatorTop( int digits )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::makeValidatorTop()";
#endif
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::saveDataOnExit()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::saveDataOnExit():  wrTimingsData_->pageMode = " << QString::number(wrTimingsData_->pageMode);
#endif
    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect))
    {
        if (firstPass == false)
            saveChanges();
    }
    else
    {
        saveChanges();
    }
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::saveDataOnExit():  Done.";
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperEditBox::setMinValues()
{
    saveAnyChanges_ = true;

    QString msg2 = QString::number( activeRow );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::setMinValues():  session_->send(Wrapper, WrEvolSetMinTrayDims, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolSetMinTrayDims, msg2);			// 9,147,precedence
//	waitForTrayUpdates = true;
}

void serviceNewWrapperEditBox::setMaxValues()
{
    saveAnyChanges_ = true;

    QString msg2 = QString::number( activeRow );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::setMaxValues():  session_->send(Wrapper, WrEvolSetMaxTrayDims, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolSetMaxTrayDims, msg2);			// 9,148,precedence
//	waitForTrayUpdates = true;
}

void serviceNewWrapperEditBox::setMinMaxValues()
{
    saveAnyChanges_ = true;

    QString msg2 = QString::number( activeRow );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::setMinMaxValues():  session_->send(Wrapper, WrEvolSetMinMaxTrayDims, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolSetMinMaxTrayDims, msg2);			// 9,149,precedence
//	waitForTrayUpdates = true;
}

void serviceNewWrapperEditBox::cycleWrapper()
{
    LOG_CLASS_INFO("serviceNewWrapperEditBox", "setMinMaxValues");

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::cycleWrapper()";
#endif

    if ( cycleWrapperInProgress_ )
        return;

    ui->butCycleWrapper->setStyleSheet("background-color: red;");

    if (saveAnyChanges_)
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
        session_->send( LabelApplier, LaSetup, setup );                                         // SEND #1: 5,14(LaSetup)

        QString printer = "0,";
        QString testLabel = "2,";
        QString qty = "0";
        QString label = printer + testLabel + qty;
        session_->send( Printer, PrTestLabel, label );                                          // SEND #2: 7,31,test label checkboard

        QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;
        session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);                            // SEND #3: 9,250 (print label)
        session_->send(Wrapper, WrEvolTest, "24," + ui->le_PLUNumber->text() + ",0");           // SEND #4: 9,108,24,plu,printer (primary)     WrapperCycle, PLU Exception (this fires the Label Ready Message on the Application Side

        printer = "1,";
        label = printer + testLabel + qty;
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

        QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;        //	HA-135
        session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);
        session_->send(Wrapper, WrEvolTest, "24," + ui->le_PLUNumber->text() + "," + printer);				// 9,108, WrapperCycle, PLU Exception
    }
    cycleWrapperInProgress_ = true;
}

void serviceNewWrapperEditBox::cancelChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::cancelChanges()";
#endif
    QString msg2 = QString::number( activeRow );
    saveAnyChanges_ = false;
    wrTimingsData_->exitByMenu = false;
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::cancelChanges():  session_->send(Wrapper, WrEvolCancelWrapBoxChanges, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolCancelWrapBoxChanges, msg2);		// 9,243,precedence
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperEditBox::saveChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::saveChanges():  saveAnyChanges_ = " + QString::number(saveAnyChanges_);
#endif

    if( saveAnyChanges_ )
    {
        QString msg2 = QString::number( activeRow );
        writeWrapBox();
        writeHeightBox();                                       // save timings

        session_->send(Wrapper, WrEvolDoneWrapBox, msg2);		// 9,244,precedence
    }
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

void serviceNewWrapperEditBox::doneWithChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::doneWithChanges()";
#endif
//  Don't call this here as it will be called when switching pages.    saveDataOnExit();
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperEditBox::goToOtherTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::goToOtherTimings()";
#endif
    wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperOtherTimings>();
}

void serviceNewWrapperEditBox::goToAdvancedTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::goToAdvancedTimings()";
#endif
    wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperAdvancedTimings>();
}

void serviceNewWrapperEditBox::requestHeightBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::requestHeightBox()";
#endif

    heightBoxSelected_ = ui->cbo_HeightBox->itemText(ui->cbo_HeightBox->currentIndex());

    QString msg2 = QString::number( activeRow ) + "," + heightBoxSelected_;

    if( saveAnyChanges_ )
        writeHeightBox();

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::requestHeightBox():  session_->send(Wrapper, WrEvolReadHeightBox, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolReadHeightBox, msg2);
}

void serviceNewWrapperEditBox::newHeightBox()
{
    saveAnyChanges_ = true;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::newHeightBox()";
#endif
    ui->cbo_HeightBox->hide();              //  Hide the current height box list
    ui->butNewHeightBox->hide();            //  Hide the New height box button
    ui->butDeleteHeightBox->hide();         //  Hide the Delete height box button
    ui->cbo_AvailableHeightBoxes->show();   //  Show the other heights available
    ui->butCancelNewHeightBox->show();      //  Allow them to cancel without adding a new height
    ui->butCancelNewHeightBox->blockSignals(false);
    ui->lblAddHeightBox->show();
}

void serviceNewWrapperEditBox::addNewHeightBox()
{
    saveAnyChanges_ = true;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::addNewHeightBox()";
#endif
    //  Get the new height string
    QString newHeight = ui->cbo_AvailableHeightBoxes->itemText(ui->cbo_AvailableHeightBoxes->currentIndex());
    QString currHeight = ui->cbo_HeightBox->itemText(ui->cbo_HeightBox->currentIndex());

    ui->cbo_AvailableHeightBoxes->blockSignals(true);
    ui->cbo_HeightBox->blockSignals(true);

    //  Remove the height from the available list
    ui->cbo_AvailableHeightBoxes->removeItem(ui->cbo_AvailableHeightBoxes->currentIndex());

    //  Add the height to the current list
    ui->cbo_HeightBox->addItem(newHeight, newHeight);

    //  Put the fields back to normal
    ui->cbo_AvailableHeightBoxes->hide();   //  Hide the other heights available
    ui->butCancelNewHeightBox->hide();      //  Hide the cancel new height button
    ui->lblAddHeightBox->hide();
    ui->butDeleteHeightBox->show();         //  Show the Delete height box button
    ui->butNewHeightBox->show();            //  Show the New height box button
    ui->cbo_HeightBox->show();              //  Show the current height box list

    ui->cbo_AvailableHeightBoxes->blockSignals(false);
    ui->cbo_HeightBox->blockSignals(false);

    QString msg2 = QString::number( activeRow ) + "," + newHeight + "," + currHeight;
//	LOG_TRACE( "send: Wrapper, WrEvolCreateHeightBox, %s", qPrintable(msg2) );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::addNewHeightBox():  session_->send(Wrapper, WrEvolCreateHeightBox, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolCreateHeightBox, msg2);
//    heightBoxChanged = true;
}

void serviceNewWrapperEditBox::cancelNewHeightBox()
{
    saveAnyChanges_ = false;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::cancelNewHeightBox()";
#endif
    //  Put the fields back to normal
    ui->cbo_AvailableHeightBoxes->hide();   //  Hide the other heights available
    ui->butCancelNewHeightBox->hide();      //  Hide the cancel new height button
    ui->lblAddHeightBox->hide();
    ui->butDeleteHeightBox->show();         //  Show the Delete height box button
    ui->butNewHeightBox->show();            //  Show the New height box button
    ui->cbo_HeightBox->show();              //  Show the current height box list
}

void serviceNewWrapperEditBox::deleteSelectedHeight()
{
    saveAnyChanges_ = true;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::deleteSelectedHeight()";
#endif
    //	Send message to verify deletion of the tray
    session_->send(Error, Wrapper, "9709,1");              // 103,9,9709,1
}

void serviceNewWrapperEditBox::autoConfigureTray()
{
    saveAnyChanges_ = true;

    // SOF-2734
    QString trayNameQuotedForCSV( wrTimingsData_->trayName );
    trayNameQuotedForCSV.replace( "\"", "\"\"" );
    trayNameQuotedForCSV.prepend( "\"" );
    trayNameQuotedForCSV.append( "\"" );

    QString msg2 = QString::number( activeRow ) + "," + trayNameQuotedForCSV;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::autoConfigureTray():  session_->send(Wrapper, WrEvolAutoConfigTrayType, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolAutoConfigTrayType, msg2);
}

void serviceNewWrapperEditBox::updateTrayName()
{
    LOG_CLASS_INFO( "snwEditBox", "updateTrayName" );

    saveAnyChanges_ = true;

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateTrayName()";
#endif
    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) && !gotNewTrayName_)
    {
#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::updateTrayName(): Requesting a new tray";
#endif
        //	HTA-79
        if (ui->le_TrayName->text().isEmpty())
        {
            //	This will force them to either reselect the Tray name field
            //	of press "Cancel" or "Done". There are no other options.
            LOG_TRACE( "Tray name is null. Not sending Auto config message.", NULL );
        }
        else
        {
            gotNewTrayName_ = true;
            wrTimingsData_->trayName = ui->le_TrayName->text();

            LOG_TRACE( "1) wrTimingsData_->trayName = [%s]", qPrintable( wrTimingsData_->trayName ) );
            autoConfigureTray();
            hideShowForNewTray(false);      //  Show all the fields for AutoConfig mode
        }
    }
#if 0	//	HA-85, HA-133:	When we request timings after saving Temporary, we get back the data before the temporary changes.
        //					This seems to be a bug in the Wrapper, but we don't request it in Windux either and the new names or PLU exceptions
        //					show up in the list without having to request the timings again [CCD Jan 2015].
    else if (currentTrayName_ != ui->le_TrayName->text())
    {
        QString msg2 = QString::number( activeRow ) + "," + ui->le_TrayName->text();
        wrTimingsData_->trayName = ui->le_TrayName->text();

#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::updateTrayName(): Updating the tray name";
        qDebug() << "serviceNewWrapperEditBox::updateTrayName():  session_->send(Wrapper, WrEvolWriteTrayType, " << msg2 << ")";
#endif
        session_->send(Wrapper, WrEvolWriteTrayType, msg2);
        wrTimingsData_->loadTrays = true;
  //      session_->send(Wrapper, WrEvolLoadPackageTimings, "1");   // 9,226,CurrentTimings
    }
#endif
    currentTrayName_ = ui->le_TrayName->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::activateTrayName()	//	HTA-79
{
    LOG_CLASS_INFO( "snwEditBox", "activateTrayName" );

//        ui->le_TrayName->setFocusPolicy(Qt::StrongFocus);
    LOG_TRACE( "ui->le_TrayName->setFocus()", NULL );
    ui->le_TrayName->setFocus();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::cycleWrapperSetupTime()
{
    LOG_CLASS_INFO( "snwEditBox", "cycleWrapperSetupTime" );

    // have to give the wrapper time to set up, make sure everyone is good to go and ready, and then cycle wrapper
    LOG_TRACE( "cycleWrapperSetupTime(): Finish up cycling the wrapper after the delay to allow object setup on Firmware Side", NULL );

    QString printer = "0,";
    QString testLabel = "2,";
    QString qty = "0";
    QString label = printer + testLabel + qty;
    LOG_INFO("Label = %s", label.toStdString().c_str());
    session_->send( Printer, PrTestLabel, label );                                          // 7,31,test label checkboard

    QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;            //	HA-135
    session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);                            // 9,250
    session_->send(Wrapper, WrEvolTest, "24," + ui->le_PLUNumber->text() + "," + printer);  // 9,108      WrapperCycle, PLU Exception (this fires the Label Ready Message on the Application Side

    printer = "1,";
    label = printer + testLabel + qty;
    LOG_INFO("Label = %s", label.toStdString().c_str());
    session_->send( Printer, PrTestLabel, label );

    cycleWrapperInProgress_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onWidgetUpdated()
{
    saveAnyChanges_ = true;

    onWidgetUpdatedInternal( qobject_cast<QWidget*>( sender() ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onWidgetUpdatedInternal( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::onWidgetUpdated()";
#endif

    QSpinBox *spinBox;
    QComboBox *comboBox;

    const int offset( offsetMap_[widget] );

    //
    // spin box edits
    //
    if ( (spinBox = qobject_cast<QSpinBox*>( widget )) )
    {
        validateEntry( offset, spinBox );
    }

    //
    // combo box edits
    //
    else if ( (comboBox = qobject_cast<QComboBox*>( widget )) )
    {
        const int valIs( comboBox->currentData().toString().toInt() );

        wrTimingsData_->wrapParameterList[offset]->setData( valIs );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onCellClicked( int row, int column )
{
    if (( isUsingSimplifiedTimings() ) && ( 0 == column ) && ( row < wrapParamOrdering_.count() ))
    {
        QString dialogText;

        // setup help text for popups
        switch ( wrapParamOrdering_[row] )
        {
        case WrapParameter::DISTANCE:
            dialogText =
                tr( "The distance the package travels past the elevator sensor across the intake\n"
                    "\n"
                    "Use this parameter to set the package position on the elevator\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is closest to the operator, $HB_MAX is the farthest from the operator\n"
                    "Adjust position on the elevator until the rear and front flap are gathered the same changes the length of the rear and front flap" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_DISTANCE) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_DISTANCE) );
            break;

        case WrapParameter::FORCE_OUTSIDE_ELEVATOR:
            dialogText =
                tr( "Adjusts the label placement right or left when leading edge\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_TRAILEDGE_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_TRAILEDGE_LABEL_OFFSET) );
            break;

        case WrapParameter::GRIPPER_ADJUST:
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

        case WrapParameter::HEIGHT_RESTRICT:
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

        case WrapParameter::BRAKE_TIMING:
            dialogText =
                tr( "Controls the gripper travel on film pull to get more or less film front to back\n"
                    "\n"
                    "Use this parameter to control the front to back overlap\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN being the shortest lengtn of film pulled and $HB_MAX being the longest length of film pulled" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_BRAKE_TIME) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_BRAKE_TIME) );
            break;

        case WrapParameter::PERCENT_STRETCH_CLAMP_ON:
            dialogText =
                tr( "Controls the distance the side clamps move in before closing on the film\n"
                    "\n"
                    "Use this parameter to adjust the prestretch of the film\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is least film in clamps; least prestretch, tension, $HB_MAX is the most film in clamps; most prestretch" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_PERCENT) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_PERCENT) );
            break;

        case WrapParameter::CLAMP_HEIGHT:
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
            dialogText =
                tr( "Controls the distance the side underfolders travel in toward the center\n"
                    "\n"
                    "Use this parameter to guide the film under the footprint of the tray, usually deep trays require additional travel\n"
                    "Use this parameter to increase support of wide trays such as 10s, 16s, 7s\n"
                    "Range $HB_MIN - $HB_MAX, $HB_MIN is the least travel and $HB_MAX the most travel" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_SIDE_FOLD_ADJ) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_SIDE_FOLD_ADJ) );
            break;

        case WrapParameter::FILM_ROLL_TO_USE:
            dialogText =
                tr( "Controls the film selector to force the machine to use upper or lower film\n"
                    "\n"
                    "Use this parameter to lock in the desired film roll to wrap the package\n"
                    "Range Narrow or Wide\n"
                    "When narrow is selected the machine will select whichever roll width is less as defined in operate / wrapper settings or service /wrapper configuration\n"
                    "When wide is selected the machine will select whichever roll width is greater as defined in operate / wrapper settings or service /wrapper configuration\n"
                    "* If film width has been overridden in wrapper settings, wrapper will select that film in service regardless of what the wrap parameter says" );
            break;

        case WrapParameter::FRONT_UF_ADJUST:
            dialogText =
                tr( "Adjusts the label placement right or left when trailing edge\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_TRAILEDGE_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_TRAILEDGE_LABEL_OFFSET) );
            break;
/*
        case WrapParameter::PRIMARY_LABEL_VERT_OFFSET:
            dialogText =
                tr( "Adjusts the label placement up or down vertically\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_VERTICAL_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_VERTICAL_LABEL_OFFSET) );
            break;
*/
        case WrapParameter::SECONDARY_LABEL_VERT_OFFSET:
            dialogText =
                tr( "Adjusts the label placement  up or down vertically\n"
                    "\n"
                    "Use this parameter to control label placement in quarter inch increments\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is left most, $HB_MAX is right most, default setting is 5" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_VERTICAL_LABEL_OFFSET) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_VERTICAL_LABEL_OFFSET) );
            break;

        case WrapParameter::WRAP_SPEED:
            dialogText =
                tr( "Controls the speed of the intake and the speed of the elevator\n"
                    "\n"
                    "Use this parameter to control wrap speed for unstable, top heavy, or frozen products\n"
                    "Range $HB_MIN - $HB_MAX ppm, $HB_MIN is slowest, $HB_MAX is fastest, usually not less than 20 and not greater than 26" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_WRAP_SPEED) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_WRAP_SPEED) );
            break;

        case WrapParameter::DISCHARGE_SPEED:
            dialogText =
                tr( "Controls the speed of the upper pusher during discharge of the package\n"
                    "\n"
                    "Use this parameter to control wrap speed for unstable, top heavy products\n"
                    "Range $HB_MIN - $HB_MAX, ppm, $HB_MIN is slowest, $HB_MAX is fastest, usually not less than 5 and not greater than 30\n"
                    "* You may have different wrap and discharge speeds" );
            dialogText.replace( "$HB_MIN", xstr(HB_MIN_DISCHARGE_SPEED) );
            dialogText.replace( "$HB_MAX", xstr(HB_MAX_DISCHARGE_SPEED) );
            break;

        default:
            break;
        }

        if ( dialogText.length() )
        {
            MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
            dialog->setSizeToFit( true );
            dialog->setTitle( tr( "Timings Help" ) );
            dialog->setText( dialogText );
            dialog->setStandardButtons( QDialogButtonBox::Ok );

            dialog->exec();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::onScreenChanging()";
#endif

    if (wrTimingsData_->exitByMenu)
        return false;
    else
    {
        wrTimingsData_->heightBoxSelected = heightBoxSelected_;     //  Save the current height box.
        saveDataOnExit();
    }

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperEditBox::buildWrapDimensions(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::buildWrapDimensions()";
#endif
    DomWrapBoxParams wrapBoxParams( msg );

    wrTimingsData_->precedence = wrapBoxParams.precedence().toInt();
    wrTimingsData_->trayName = wrapBoxParams.trayName();
    wrTimingsData_->pluException = wrapBoxParams.pluException();

    activeRow = wrTimingsData_->precedence;

    ui->le_TrayName->setText(wrTimingsData_->trayName);
    ui->le_PLUNumber->setText(wrTimingsData_->pluException);

    ui->le_MinLength->setText(wrapBoxParams.minLength());
    ui->le_MaxLength->setText(wrapBoxParams.maxLength());
    ui->le_MinWidth->setText(wrapBoxParams.minWidth());
    ui->le_MaxWidth->setText(wrapBoxParams.maxWidth());
    ui->le_MaxLip->setText(wrapBoxParams.maxLip());

    //  Set up our copy of the wrap box dimensions
    wrapBoxData_.setMinLength(getIntFromString(wrapBoxParams.minLength()));
    wrapBoxData_.setMaxLength(getIntFromString(wrapBoxParams.maxLength()));
    wrapBoxData_.setMinWidth(getIntFromString(wrapBoxParams.minWidth()));
    wrapBoxData_.setMaxWidth(getIntFromString(wrapBoxParams.maxWidth()));
    wrapBoxData_.setMaxLip(getIntFromString(wrapBoxParams.maxLip()));

    if (wrTimingsData_->canEditTrayName)
    {
        ui->le_MinLength->setDisabled(false);
        ui->le_MaxLength->setDisabled(false);
        ui->le_MinWidth->setDisabled(false);
        ui->le_MaxWidth->setDisabled(false);
        ui->le_MaxLip->setDisabled(false);
    }

    buildHeightCombo(wrapBoxParams);
}

void serviceNewWrapperEditBox::buildHeightCombo(DomWrapBoxParams &wrapBoxParams)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::buildHeightCombo()";
#endif
    boxCount = wrapBoxParams.numHeightBoxes().toInt();
    if (boxCount > 0)
    {
        //  Block the signals on these combo boxes until they are set up.
        ui->cbo_AvailableHeightBoxes->blockSignals(true);
        ui->cbo_HeightBox->blockSignals(true);

        ui->cbo_HeightBox->clear();
        ui->cbo_AvailableHeightBoxes->clear();

        //  Get the current list of height boxes for this wrap box
        QVector<QString> heightArray = TokenParser::splitStringTokens( wrapBoxParams.heightBoxes() );

        int x, index;
        int totalHeightBoxes = ui->cbo_AllHeightBoxes->count();
        QString allHeightStr, currHeightStr;
        for (x = 0, index = 0; x < totalHeightBoxes; x++)
        {
            allHeightStr = ui->cbo_AllHeightBoxes->itemText(x);
            if (index < boxCount)
            {
                currHeightStr = heightArray.at(index);
            }

            if (allHeightStr != currHeightStr)
            {
                ui->cbo_AvailableHeightBoxes->addItem(allHeightStr, allHeightStr);
            }
            else if (index < boxCount)
            {
                ui->cbo_HeightBox->addItem(currHeightStr, currHeightStr);
                ++index;
            }
        }
        //	Select the current height box
        if (heightBoxSelected_.isEmpty())
            index = 0;
        else
            index = ui->cbo_HeightBox->findText(heightBoxSelected_);
        ui->cbo_HeightBox->setCurrentIndex(index);

        ui->cbo_HeightBox->setEnabled(true);

        ui->cbo_AvailableHeightBoxes->hide();
        ui->cbo_AllHeightBoxes->hide();

        ui->cbo_AvailableHeightBoxes->blockSignals(false);
        ui->cbo_HeightBox->blockSignals(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::setupHeightBox(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::setupHeightBox()";
#endif

    if ( heightBox_ )
        delete heightBox_;

    heightBox_ = new DomHeightBox( msg );

    selectHeight();
    setupHeightBoxParams();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::selectHeight()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::selectHeight()";
#endif

    if ( boxCount <= 0 )
        return;

    const int index( ui->cbo_HeightBox->findData( heightBox_->maxHeight() ) );

    if ( index < 0 )
        return;

    ui->cbo_HeightBox->blockSignals( true );
    ui->cbo_HeightBox->setCurrentIndex( index );
    ui->cbo_HeightBox->blockSignals( false );


    //  Set the Min height value
    ui->le_MinHeight->setText( 0 < index ? ui->cbo_HeightBox->itemData( index - 1 ).toString() : QString( "0" ) );

    //  Set the Max height value
    ui->le_MaxHeight->setText( ui->cbo_HeightBox->itemData( index ).toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::buildParamTable()
{
    if ( WrapperTimingsData::pmAutoConfigure == wrTimingsData_->pageMode )
        return;

    //Set the default table size parameters for the column widths
    setDefaultTableSize( ui->tblBasicTimingsList );

    //Clear the previous wrap parameters
    ui->tblBasicTimingsList->clear();

    wrapParamOrdering_.clear();

    //Create timings
    if ( isUsingSimplifiedTimings() )
    {
        const QColor orange( 255, 230, 153);
        const QColor blue( 217, 225, 242 );
        const QColor red( 248, 203, 173 );
        const QColor yellow( 255, 230, 153 );

        addWrapParameter( WrapParameter::DISTANCE, ui->tblBasicTimingsList, HB_MIN_DISTANCE, HB_MAX_DISTANCE, orange );

        //addWrapParameter( WrapParameter::GRIPPER_ADJUST, ui->tblBasicTimingsList, HB_MIN_GRIPPER_ADJ, HB_MAX_GRIPPER_ADJ, blue );        // 0 to 10
        addWrapParameter( WrapParameter::GRIPPER_ADJUST, ui->tblBasicTimingsList, HB_NEG_MIN_GRIPPER_ADJ, HB_MAX_GRIPPER_ADJ, blue );      // NGW2: -5 to 10
        addWrapParameter( WrapParameter::REAR_UF_TRIP_DELAY, ui->tblBasicTimingsList, HB_MIN_REAR_TRIP_DELAY, HB_MAX_REAR_TRIP_DELAY, blue );
        addWrapParameter( WrapParameter::HEIGHT_RESTRICT, ui->tblBasicTimingsList, HB_MIN_HEIGHT_RESTRICT, HB_MAX_HEIGHT_RESTRICT, blue );
        addWrapParameter( WrapParameter::BRAKE_TIMING, ui->tblBasicTimingsList, HB_MIN_BRAKE_TIME, HB_MAX_BRAKE_TIME, blue );

        addWrapParameter( WrapParameter::PERCENT_STRETCH_CLAMP_ON, ui->tblBasicTimingsList, HB_MIN_PERCENT, HB_MAX_PERCENT, red );
        //addWrapParameter( WrapParameter::CLAMP_HEIGHT, ui->tblBasicTimingsList, HB_MIN_CLAMP_OPEN, HB_MAX_CLAMP_OPEN, red );              // 0 to 10
        addWrapParameter( WrapParameter::CLAMP_HEIGHT, ui->tblBasicTimingsList, HB_NEG_MIN_CLAMP_OPEN, HB_MAX_CLAMP_OPEN, red );            // NGW2: -5 to 10
        addWrapParameter( WrapParameter::SIDE_UF_ADJUST, ui->tblBasicTimingsList, HB_MIN_SIDE_FOLD_ADJ, HB_MAX_SIDE_FOLD_ADJ, red );

        if ( isDualRollWrapper() )
            addWrapParameter( WrapParameter::FILM_ROLL_TO_USE, ui->tblBasicTimingsList, 0, 0, yellow );

        if ( isNGWWrapper() )
        {
            // moved to 2P section ( WrapParameter::FORCE_OUTSIDE_ELEVATOR, ui->tblBasicTimingsList, HB_MIN_TRAILEDGE_LABEL_OFFSET, HB_MAX_TRAILEDGE_LABEL_OFFSET, yellow );
            addWrapParameter( WrapParameter::FRONT_UF_ADJUST, ui->tblBasicTimingsList, HB_MIN_TRAILEDGE_LABEL_OFFSET, HB_MAX_TRAILEDGE_LABEL_OFFSET, yellow );

            if(session_->scaleConfig->isWrapperTwoPrinter())//Only NGW2P has Vertical offset params SOF-5094
            {
                addWrapParameter( WrapParameter::FORCE_OUTSIDE_ELEVATOR, ui->tblBasicTimingsList, HB_MIN_TRAILEDGE_LABEL_OFFSET, HB_MAX_TRAILEDGE_LABEL_OFFSET, yellow );
                //addWrapParameter( WrapParameter::PRIMARY_LABEL_VERT_OFFSET, ui->tblBasicTimingsList, HB_MIN_VERTICAL_LABEL_OFFSET, HB_MAX_VERTICAL_LABEL_OFFSET, yellow );
                addWrapParameter( WrapParameter::SECONDARY_LABEL_VERT_OFFSET, ui->tblBasicTimingsList, HB_MIN_VERTICAL_LABEL_OFFSET, HB_MAX_VERTICAL_LABEL_OFFSET, yellow );
            }
        }

        addWrapParameter( WrapParameter::WRAP_SPEED, ui->tblBasicTimingsList, HB_MIN_WRAP_SPEED, HB_MAX_WRAP_SPEED, yellow );
        addWrapParameter( WrapParameter::DISCHARGE_SPEED, ui->tblBasicTimingsList, HB_MIN_DISCHARGE_SPEED, HB_MAX_DISCHARGE_SPEED, yellow );

    }
    else
    {
        //  These fields are all integers, so no need to check the type.
        addWrapParameter( WrapParameter::PERCENT_STRETCH_CLAMP_ON, ui->tblBasicTimingsList, HB_MIN_PERCENT, HB_MAX_PERCENT_AWS1_AND_2 );
        addWrapParameter( WrapParameter::KNIFE_HEIGHT, ui->tblBasicTimingsList, HB_MIN_KNIFE_UP, HB_MAX_KNIFE_UP );

        // SOF-4434 Need to allow larger range for AWS1 and AWS2
        DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );
        if ( evolWrapperSystemInformation->awsModelNumber() < DomEvolWrapperSystemInformation::reducedTimingModels )
            addWrapParameter( WrapParameter::CLAMP_HEIGHT, ui->tblBasicTimingsList, HB_MIN_CLAMP_OPEN, HB_MAX_CLAMP_OPEN_AWS1_AND_2 );      // 0 to 1230
        else
        {
            //addWrapParameter( WrapParameter::CLAMP_HEIGHT, ui->tblBasicTimingsList, HB_MIN_CLAMP_OPEN, HB_MAX_CLAMP_OPEN );                 // 0 to 10
            addWrapParameter( WrapParameter::CLAMP_HEIGHT, ui->tblBasicTimingsList, HB_NEG_MIN_CLAMP_OPEN, HB_MAX_CLAMP_OPEN );                 // -5 to 10
        }
        addWrapParameter( WrapParameter::DISTANCE, ui->tblBasicTimingsList, HB_MIN_DISTANCE_AWS1_AND_2, HB_MAX_DISTANCE_AWS1_AND_2 );
        addWrapParameter( WrapParameter::GRIPPER_ADJUST, ui->tblBasicTimingsList, HB_MIN_GRIPPER_ADJ_AWS1_AND_2, HB_MAX_GRIPPER_ADJ_AWS1_AND_2 );
        addWrapParameter( WrapParameter::INSIDE_GRIPPER_RELEASE, ui->tblBasicTimingsList, HB_MIN_INSIDE_GRIPPER_RELEASE, HB_MAX_INSIDE_GRIPPER_RELEASE );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::setDefaultTableSize( QTableWidget* table )
{
    if ( !table )
        return;

    if ( isUsingSimplifiedTimings() )
    {
        // Leave 32px for the scroll bar
        const int tableWidth( table->width() - 32 );

        table->setColumnCount( 4 );
        table->setColumnWidth( 0, (tableWidth * 45) / 100 );
        table->setColumnWidth( 1, (tableWidth * 15) / 100 );
        table->setColumnWidth( 2, (tableWidth * 25) / 100 );
        table->setColumnWidth( 3, (tableWidth * 15) / 100 );
    }
    else
    {
        table->setColumnCount( 2 );
        table->setColumnWidth( 0, (table->width() * 55) / 100 );
        table->setColumnWidth( 1, (table->width() * 45) / 100 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::addWrapParameter( int wrapParam, QTableWidget* table, int min, int max, const QColor &c )
{
    const unsigned int row( wrapParamOrdering_.size() );

    wrapParamOrdering_.push_back( wrapParam );              // list of enums

    if ( WrapParameter::PERCENT_STRETCH_CLAMP_ON == wrapParam )
        clampOnRow = row;

    //Set the font size
    QFont textFont( font() );
    textFont.setPointSize( 20 );

    QTableWidgetItem *paramNameItem( new QTableWidgetItem );
    paramNameItem->setBackgroundColor( c );

    paramNameItem->setText( wrTimingsData_->wrapParameterList[wrapParam]->getName() );

    paramNameItem->setFont( textFont );

    table->setItem( row, 0, paramNameItem );

    //Set the value widget
    QWidget *valueWidget( NULL );
    bool isSpinBox( false );

    if ( WrapParameter::FILM_ROLL_TO_USE == wrapParam )
        valueWidget = createFilmRollToUseCombo( wrapParam, 0 );
    else
    {
        valueWidget = createQSpinBox( wrapParam, min, max, 0 );
        isSpinBox = true;
    }

    if ( valueWidget )
    {
        if ( isUsingSimplifiedTimings() )
        {
            table->setCellWidget( row, 2, valueWidget );

            if ( isSpinBox )
            {
                QSpinBox *spinBox( qobject_cast<QSpinBox*>( valueWidget ) );
                spinBox->installEventFilter( this );

                QFont buttonFont( font() );
                buttonFont.setPointSize( 14 );

                QString decreaseName( wrTimingsData_->wrapParameterList[wrapParam]->getDecreaseName() + " [" + QString::number(min) + "]" );
                QString increaseName( "[" + QString::number(max) + "] " + wrTimingsData_->wrapParameterList[wrapParam]->getIncreaseName() );

                QPushButton *decrease( new QPushButton( decreaseName ) );
                decrease->setFont( buttonFont );
                decrease->setMinimumHeight( 16 );

                QPushButton *increase( new QPushButton( increaseName ) );
                increase->setFont( buttonFont );
                increase->setMinimumHeight( 16 );

                table->setCellWidget( row, 1, decrease );
                table->setCellWidget( row, 3, increase );

                connect( decrease, SIGNAL(clicked(bool)), SLOT(onStepDown()) );
                connect( increase, SIGNAL(clicked(bool)), SLOT(onStepUp()) );

                incDecMap_[decrease] = spinBox;
                incDecMap_[increase] = spinBox;
            }
        }
        else
        {
            table->setCellWidget( row, 1, valueWidget );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::setupHeightBoxParams()
{
    LOG_CLASS_INFO( "snwEditBox", "setupHeightBoxParams" );

    //LOG_INFO( "snw_EditBox() initial box.attributes string from 9,147 msg: %s", qPrintable(heightBox_->wrapAttributes()) );

    QString test_attribs = heightBox_->wrapAttributes();

    QVector<QString> paramArray( TokenParser::splitStringTokens( heightBox_->wrapAttributes() ) );
    //LOG_INFO( "snwEditBox() Num of Params split on paramArray Vector: %d", paramArray.count() );

    //Update our wrapParameterList with the height box params
    int numOfParams = wrTimingsData_->wrapParameterList.count();
    //LOG_INFO( "snwEditBox() Num of Params of wrapParameterList: %d", wrTimingsData_->wrapParameterList.count() );

    for ( unsigned int i( numOfParams ); i--; )
    {
        WrapParameter *tmpParam;

        //int tester1 = wrTimingsData_->wrapParameterList[i]->getData();
        //LOG_INFO( "snwEditBox() tester1[i]: %d", tester1 );

        if ( (tmpParam = wrTimingsData_->wrapParameterList[i]) )
        {
            int value( paramArray[i].toInt() );
            //LOG_INFO( "snwEditBox() for loop value[%d]: %d", i, value);

            if ( ( ( WrapParameter::FRONT_UF_ADJUST == i ) && ( isNGWWrapper() ) && ( 100 <= value ) ) ||
                 ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == i ) && ( isNGWWrapper() ) && ( 100 <= value ) ) )
            {
                paramArray[i] = QString::number( value -= 100 );
            }

            if ( isNGWWrapper() && ( ( WrapParameter::CLAMP_HEIGHT == i ) || ( WrapParameter::GRIPPER_ADJUST == i ) ) )
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

    //int tester2 = wrapParamOrdering_.count();
    //LOG_INFO( "snwEditBox() wrapParamOrdering_.count(): %d", wrapParamOrdering_.count() );

    //Now update the wrap parameters for the height box
    for ( unsigned int i( wrapParamOrdering_.count() ); i--; )
        updateWrapParam( ui->tblBasicTimingsList, wrapParamOrdering_[i], paramArray, i );

    //LOG_INFO( "snwEditBox", "setupHeightBoxParams END");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::isUsingSimplifiedTimings()
{
    return ( DomEvolWrapperSystemInformation::reducedTimingModels <= wrTimingsData_->wrapperModelNum );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateWrapParam( QTableWidget* table, int wrapParam, QVector<QString> &paramArray, unsigned int row )
{
    //LOG_CLASS_INFO( "snwEditBox", "updateWrapParam START" );

    if ( !table )
        return;

    int column( 1 );

    if ( isUsingSimplifiedTimings() )
        column = 2;

    QSpinBox *spinBox;
    QComboBox *comboBox;

    if ( (spinBox = qobject_cast<QSpinBox*>( table->cellWidget( row, column ) )) )
    {
        int value = 0;
        QString strVal = "";

        strVal = paramArray.at( wrapParam );
        value = paramArray.at( wrapParam ).toInt();

        // enforce min/max values
        if ( value < spinBox->minimum() )
            value = spinBox->minimum();
        else if ( spinBox->maximum() < value )
            value = spinBox->maximum();

        spinBox->setValue( value );
    }

    else if ( (comboBox = qobject_cast<QComboBox*>( table->cellWidget( row, column ) )) )
    {
        const int index( comboBox->findData( paramArray.at( wrapParam ) ) );
        if ( 0 <= index )
        {
            comboBox->blockSignals( true );
            comboBox->setCurrentIndex( index );
            comboBox->blockSignals( false );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::validateEntry( int offset, QSpinBox *intField )
{
    //LOG_CLASS_INFO( "snwEditBox", "validateEntry START" );

    const int valWas( wrTimingsData_->wrapParameterList[offset]->getData() );
    const int valIs( intField->value() );

    //LOG_INFO( "validateEntry() valWas =  %d,    valIs = %d", valWas, valIs);

    if ( valWas == valIs )
        return;

    //LOG_INFO( "validateEntry() update wrapParameterList[offset][%d]: %d", offset, valIs);

    wrTimingsData_->wrapParameterList[offset]->setData( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSpinBox *serviceNewWrapperEditBox::createQSpinBox( int offset, int minVal, int maxVal, QString currVal )
{
    LOG_CLASS_INFO( "snwEditBox", "createQSpinBox" );

    LOG_TRACE( "offset = %d, minVal = %d, maxVal = %d, currVal = %s", offset, minVal, maxVal, qPrintable(currVal) );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::createQSpinBox(" << QString::number(offset) << ", " << QString::number(minVal) << ", " << QString::number(maxVal) << ", " << currVal << ")";

#endif
    QSpinBox *integerField( new QSpinBox( this ) );

    if ( !integerField )
        return NULL;

    integerField->setObjectName( "param" + offset );
    integerField->setStyleSheet( "background-color: white; font: 20pt \"MS Shell Dlg 2\";" );

    integerField->setRange( minVal, maxVal );		//	HA-129: Set up the integerField regardless of Current or Default settings.
    integerField->setValue( currVal.toInt() );
    integerField->setButtonSymbols( QAbstractSpinBox::NoButtons );

    if ( !viewCurrentTimings_ )
        integerField->setEnabled( false );
    else
    {
        connect( integerField, SIGNAL( editingFinished() ), SLOT( onWidgetUpdated() ) );
    }

    offsetMap_[integerField] = offset;

    if ( WrapParameter::PERCENT_STRETCH_CLAMP_ON == offset )
        le_PercentStretchClampOn_ = integerField;

    LOG_TRACE( "Done", NULL );
    return integerField;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QComboBox *serviceNewWrapperEditBox::createFilmRollToUseCombo(int offset, QString currVal )
{
    QFont cboFont( font() );
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_FilmRoll( new QComboBox( this ) );

    if ( !cbo_FilmRoll )
        return NULL;

    cbo_FilmRoll->setFont( cboFont );

    cbo_FilmRoll->setObjectName( "param" + offset );
    cbo_FilmRoll->addItem( tr("Narrow roll"), QString( "100" ) );
    cbo_FilmRoll->addItem( tr("Wide roll"), QString( "200" ) );

    const int index( cbo_FilmRoll->findData( currVal ) );
    if ( 0 <= index )
        cbo_FilmRoll->setCurrentIndex( index );

    if ( !viewCurrentTimings_ )
        cbo_FilmRoll->setEnabled( false );
    else
    {
        connect( cbo_FilmRoll, SIGNAL( currentIndexChanged(int) ), SLOT( onWidgetUpdated() ) );
    }

    offsetMap_[cbo_FilmRoll] = offset;

    return cbo_FilmRoll;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperEditBox::validateField( int oldValue, int newValue, int MinValue, int MaxValue )
{
    LOG_CLASS_INFO( "snwEditBox", "validateField" );

    LOG_TRACE( "oldValue = %d, newValue = %d, MinValue = %d, MaxValue = %d", oldValue, newValue, MinValue, MaxValue );

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

    LOG_TRACE( "Returning %d", newValue );

    return newValue;
}


void serviceNewWrapperEditBox::updateWrapperPkgDescription(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "snwEditBox", "updateWrapperPkgDescription" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateWrapperPkgDescription()";
#endif
    DomEvolWrapperPkgDesc pkgDescription( msg );

    LOG_TRACE( "trayName = %s", qPrintable( pkgDescription.trayName() ) );

    ui->le_ActualHeight->setText( pkgDescription.height() );
    ui->le_ActualLength->setText( pkgDescription.length() );
    ui->le_ActualLip->setText( pkgDescription.lip() );
    ui->le_ActualWidth->setText( pkgDescription.width() );

    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
//        if (oLengthActual.value != "0" || oWidthActual.value != "0" || oHeightActual.value != "0" || oLipActual.value != "0")
//        {
//            enableButton(osetMinButton);
//            enableButton(osetMinMaxButton);
//            enableButton(osetMaxButton);
//        }

        ui->butSetMin->setEnabled(true);
        ui->butSetMinAndMax->setEnabled(true);
        ui->butSetMax->setEnabled(true);
    }

    LOG_TRACE( "pkgDescription.usedWrapBoxBestFit() = %s", qPrintable(pkgDescription.usedWrapBoxBestFit()) );

    static const QString fieldStyleHighlight("background-color: yellow;");
    static const QString fieldStyleNormal("background-color: white;");

    if ( pkgDescription.usedWrapBoxBestFit() == "1")
    {
        ui->le_ActualHeight->setStyleSheet( fieldStyleHighlight );
        ui->le_ActualLength->setStyleSheet( fieldStyleHighlight );
        ui->le_ActualLip->setStyleSheet( fieldStyleHighlight );
        ui->le_ActualWidth->setStyleSheet( fieldStyleHighlight );
    }
    else
    {
        ui->le_ActualHeight->setStyleSheet( fieldStyleNormal );
        ui->le_ActualLength->setStyleSheet( fieldStyleNormal );
        ui->le_ActualLip->setStyleSheet( fieldStyleNormal );
        ui->le_ActualWidth->setStyleSheet( fieldStyleNormal );
    }

    static const QString spinBoxStyleHighlight("background-color: yellow; font: 20pt \"MS Shell Dlg 2\";");
    static const QString spinBoxStyleNormal("background-color: white; font: 20pt \"MS Shell Dlg 2\";");

    const int wrModel( wrTimingsData_->wrapperModelNum );
    LOG_TRACE( "wrModel = %d", wrModel );

    if ( WrapperTimingsData::pmAutoConfigure != wrTimingsData_->pageMode )
    {
        //	HTA-8:  We only check & set the table if we are NOT in Auto Config.
        // SOF-240: disable yellow highlight
        //if (( DomEvolWrapperSystemInformation::None < wrModel ) && ( wrModel < DomEvolWrapperSystemInformation::endWrapperModels ))
        //{
        //    if (( ui->tblBasicTimingsList->rowCount() ) && ( clampOnRow < ui->tblBasicTimingsList->columnCount() ))
        //    {
        //        QTableWidgetItem *item;

        //        if ( (item = ui->tblBasicTimingsList->item( clampOnRow, 0 )) )
        //            item->setBackgroundColor( QColor( pkgDescription.modifiedNetStretch() ? "yellow" : "white" ) );
        //    }

        //    if ( le_PercentStretchClampOn_ )
        //        le_PercentStretchClampOn_->setStyleSheet( pkgDescription.modifiedNetStretch() ? spinBoxStyleHighlight : spinBoxStyleNormal ); // HA-132
        //}

        ui->label_trayFound->setText( (wrTimingsData_->cycledTrayName = pkgDescription.trayName()) );
        wrTimingsData_->updateActualPkg = true;
    }

    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect))
    {
        LOG_TRACE( "firstPass == true", NULL );
        if (firstPass == true)
        {
            if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect)
            {
                ui->tblBasicTimingsList->show();
                activeRow = pkgDescription.wrapBoxIndex().toInt();
                ui->butCycleWrapper->setEnabled(true);      //  TODO: What about a remote connection?
                ui->le_TrayName->setEnabled(true);
                ui->le_PLUNumber->setEnabled(true);
            }
            else if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
            {
                //  Enable these fields now.
                ui->le_MinLength->setEnabled(true);
                ui->le_MaxLength->setEnabled(true);
                ui->le_MinWidth->setEnabled(true);
                ui->le_MaxWidth->setEnabled(true);
                ui->le_MaxLip->setEnabled(true);
            }
            firstPass = false;
        }
//        if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure)
//        {
//            wrapperWasCycled = true;
//        }
    }

    LOG_TRACE( "done", NULL );
}

void serviceNewWrapperEditBox::processReturnedWrapperErrors(const SessionMessage& msg)
{
    returnedErrorsWrapperDev m( msg );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::processReturnedWrapperErrors():  errCode = " << m.errorCode();
#endif
    if (9709 == m.errorCode())
    {
        bool okPressed( 0 < m.errorData2() );
        if (okPressed)
            continueDeleteSelectedHeight();
    }
}

void serviceNewWrapperEditBox::continueDeleteSelectedHeight()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::continueDeleteSelectedHeight()";
#endif
    int theIndex = ui->cbo_HeightBox->currentIndex();
    QString currHeightStr = ui->cbo_HeightBox->itemText(theIndex);
    int currHeightVal = (int) (ui->cbo_HeightBox->itemData(theIndex).toFloat() * 10);

    ui->cbo_AvailableHeightBoxes->blockSignals(true);
    ui->cbo_HeightBox->blockSignals(true);

    //  Remove the Height from the current list.
    ui->cbo_HeightBox->removeItem(theIndex);

    //  Add the Height back to the available list.
    int j = 0;
    int availHeightVal;
    do
    {
        availHeightVal = (int) (ui->cbo_AvailableHeightBoxes->itemData(j).toFloat() * 10);
        if (availHeightVal < currHeightVal)
            ++j;
        else
            break;
    } while (j < ui->cbo_AvailableHeightBoxes->count());


    //  Add the Height to the available list.
    ui->cbo_AvailableHeightBoxes->insertItem(j, currHeightStr, currHeightStr);

    //  Tell the session to remove this height box.
    QString msg2 = QString::number( activeRow ) + "," + currHeightStr;
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::continueDeleteSelectedHeight():  session_->send(Wrapper, WrEvolDeleteHeightBox, " << msg2 << ")";
#endif
    session_->send(Wrapper, WrEvolDeleteHeightBox, msg2);

    ui->cbo_AvailableHeightBoxes->blockSignals(false);
    ui->cbo_HeightBox->blockSignals(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::autoDetectTray()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::autoDetectTray():  session_->send(Wrapper, WrEvolAutoDetectTrayType)";
#endif

    session_->send( Wrapper, WrEvolAutoDetectTrayType );          //  9,232
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::wrapperCycleComplete( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "snwEditBox", "wrapperCycleComplete" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::wrapperCycleComplete()";
#endif

    DomEvolWrapperTestComplete m( msg );

    LOG_TRACE( "Test number = %s. Normal style = [%s]", qPrintable(m.testNum()), qPrintable(cycleButtonStyle) );

    if ( 24 == m.testNum().toInt() )
    {
        ui->butCycleWrapper->setStyleSheet( cycleButtonStyle );
        cycleWrapperInProgress_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::showTablesAndButtons()
{
    // Show the table and buttons below it.
    ui->tblBasicTimingsList->show();
    ui->butCycleWrapper->show();

    if ( !isUsingSimplifiedTimings() )
    {
        if( session_->scaleConfig->isWrapperTwoPrinter() )
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->hide();
            ui->butCycleWrapperTrailing->hide();
        }
        else
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->show();
            ui->butCycleWrapperTrailing->show();
        }
    }
    else
    {
        // SOF-2550 - added in 2P
        if( false == session_->scaleConfig->isWrapperTwoPrinter() )
        {
            // on two applier systems we only want the Cycle Wrapper Button per agreement b/t JHC, Kraig, and Carlos 8-15-23
            ui->butCycleWrapperLeading->show();
            ui->butCycleWrapperTrailing->show();
        }
    }

    ui->butSaveChanges->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::promptPLUException()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::promptPLUException()";
#endif

    const QString trayName( wrTimingsData_->trayName );

    NumericInputBoxDialog dialog( mainWindow_ );
    dialog.setText( tr("Please enter PLU Exception number for tray") + " [" + trayName + "] ");
    dialog.setRequired( true );

    if ( QDialog::Accepted != dialog.exec() )
    {
#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::promptPLUException():  User cancelled the PLU exception.";
#endif

        saveAnyChanges_ = false;
    }

    const QString result( dialog.inputText() );

    if (( result.isEmpty() ) || ( 0 == result.toInt() ))
    {
#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::promptPLUException():  No PLU exception entered.";
#endif

        saveAnyChanges_ = false;
    }

    //  Enable this after an attempt to get a PLU exception.
    connect( ui->le_PLUNumber, SIGNAL(editingFinished()), SLOT(pluExceptionChanged()) );

    if ( saveAnyChanges_ )
    {
        showTablesAndButtons();

        // SOF-2734
        QString trayNameQuotedForCSV( trayName );
        trayNameQuotedForCSV.replace( "\"", "\"\"" );
        trayNameQuotedForCSV.prepend( "\"" );
        trayNameQuotedForCSV.append( "\"" );

        const QString msg2( QString::number( activeRow ) + "," + trayNameQuotedForCSV + "," + result );

#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::promptPLUException():  session_->send(Wrapper, WrEvolCreateTrayType, " << msg2 << ")";
#endif

        session_->send( Wrapper, WrEvolCreateTrayType, msg2 );              // 9,234,activeRow,trayName,pluException
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::pluExceptionChanged()
{
    const QString pluExceptionStr( ui->le_PLUNumber->text() );
    saveAnyChanges_ = true;

    showTablesAndButtons();

    // SOF-2734
    QString trayNameQuotedForCSV( wrTimingsData_->trayName );
    trayNameQuotedForCSV.replace( "\"", "\"\"" );
    trayNameQuotedForCSV.prepend( "\"" );
    trayNameQuotedForCSV.append( "\"" );

    const QString msg2( QString::number( activeRow ) + "," + trayNameQuotedForCSV + "," + pluExceptionStr );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::pluExceptionChanged():  session_->send(Wrapper, WrEvolCreateTrayType, " << msg2 << ")";

#endif

    session_->send( Wrapper, WrEvolCreateTrayType, msg2 );     // 9,234,activeRow,trayName,pluException
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
    saveAnyChanges_ = true;

    const int valWas( wrapBoxData_.getMinLength() );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMinLength() to " << ui->le_MinLength->text();
#endif

    int valIs( ui->le_MinLength->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let maxVal be equal to the maxLength. It should already be less than or equal to WB_MAX_LENGTH.
    const int maxVal( wrapBoxData_.getMaxLength() - 1 );

    valIs = validateField( valWas, valIs, WB_MIN_LENGTH, maxVal );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMinLength():  ui->le_MinLength->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->le_MinLength->fromInt( valIs );
    wrapBoxData_.setMinLength( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxLength()
{
    saveAnyChanges_ = true;

    const int valWas( wrapBoxData_.getMaxLength() );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxLength() to " << ui->le_MaxLength->text();
#endif

    int valIs( ui->le_MaxLength->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let minVal be equal to the minLength. It should already be greater than or equal to WB_MIN_LENGTH.
    const int minVal( wrapBoxData_.getMinLength() + 1 );

    valIs = validateField( valWas, valIs, minVal, WB_MAX_LENGTH );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxLength():  ui->le_MinLength->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->le_MaxLength->fromInt( valIs );
    wrapBoxData_.setMaxLength( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMinWidth()
{
    saveAnyChanges_ = true;

    const int valWas( wrapBoxData_.getMinWidth() );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMinWidth() to " << ui->le_MinWidth->text();
#endif

    int valIs( ui->le_MinWidth->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let maxVal be equal to the maxWidth. It should already be less than or equal to WB_MAX_WIDTH.
    const int maxVal( wrapBoxData_.getMaxWidth() - 1 );

    valIs = validateField( valWas, valIs, WB_MIN_WIDTH, maxVal );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMinWidth():  ui->le_MinWidth->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->le_MinWidth->fromInt( valIs );
    wrapBoxData_.setMinWidth( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxWidth()
{
    saveAnyChanges_ = true;

    const int valWas( wrapBoxData_.getMaxWidth() );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxWidth() to " << ui->le_MaxWidth->text();
#endif
    int valIs( ui->le_MaxWidth->toInt() );

    if ( valIs == valWas )
        return;

    //	Don't let minVal be equal to the minWidth. It should already be greater than or equal to WB_MIN_WIDTH.
    const int minVal( wrapBoxData_.getMinWidth() + 1 );

    valIs = validateField( valWas, valIs, minVal, WB_MAX_WIDTH );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxWidth():  ui->le_MaxWidth->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->le_MaxWidth->fromInt( valIs );
    wrapBoxData_.setMaxWidth( valIs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::updateMaxLip()
{
    saveAnyChanges_ = true;

    const int valWas( wrapBoxData_.getMaxLip() );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxLip() to " << ui->le_MaxLip->text();
#endif

    int valIs( ui->le_MaxLip->toInt() );

    if ( valIs == valWas )
        return;

    valIs = validateField( valWas, valIs, WB_MIN_LIP, WB_MAX_LIP );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperEditBox::updateMaxLip():  ui->le_MaxLip->fromInt(" << QString::number(valIs) << ")";
#endif

    ui->le_MaxLip->fromInt(valIs);
    wrapBoxData_.setMaxLip(valIs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::hideShowForNewTray( bool hideThem )
{
    ui->lblTitleMin->setVisible( !hideThem );
    ui->lblTitleActual->setVisible( !hideThem );
    ui->lblTitleMax->setVisible( !hideThem );

    ui->lblLength->setVisible( !hideThem );
    ui->le_MinLength->setVisible( !hideThem );
    ui->le_ActualLength->setVisible( !hideThem );
    ui->le_MaxLength->setVisible( !hideThem );

    ui->lblWidth->setVisible( !hideThem );
    ui->le_MinWidth->setVisible( !hideThem );
    ui->le_ActualWidth->setVisible( !hideThem );
    ui->le_MaxWidth->setVisible( !hideThem );

    ui->lblLip->setVisible( !hideThem );
    ui->le_ActualLip->setVisible( !hideThem );
    ui->le_MaxLip->setVisible( !hideThem );

    ui->lblHeight->setVisible( !hideThem );
    ui->le_ActualHeight->setVisible( !hideThem );

    ui->lblPLUNumber->setVisible( !hideThem );
    ui->le_PLUNumber->setVisible( !hideThem );

    ui->butAutoConfig->setVisible( !hideThem );

    ui->label_PromptTrayName->setVisible( hideThem );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "snwEditBox", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if (( Wrapper == device ) && (WrEvolStatus != event ) && ( WrEvolSensors != event ))
    {
        LOG_TRACE( "device = %d,  event = %d", device, event );
#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperEditBox::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
#endif
    }

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
        LOG_INFO("PrStockMetrics received", NULL);
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

/*      ORIGINAL CODE
        QString pLabelSize = m.stockLengthInLinesRaw() + ",";
        QString pRotation = "0,";
        QString pPlacement = "0,";

        QString sLabelSize = m.stockLengthInLinesRaw() + ",";
        QString sRotation = "0,";
        QString sPlacement = "0";

        // set other size to 0
        if( !leadingEdge )
        {
            pPlacement = "1,";
            sLabelSize = "0,";
        }
        else
        {
            pLabelSize = "0,";
        }

        // for some reason the rotation values were being overwritten??? so just hardcoding in 0s
        QString setup = pLabelSize + pRotation + pPlacement + sLabelSize + sRotation + sPlacement;

        //LOG_INFO("Label Applier setup: %s", setup.toStdString().c_str());
        session_->send( LabelApplier, LaSetup, setup );
*/
#if 0 //Fix SOF-5150 Unprompted Wrapper cycling in timings
        // Now cycle the wrapper with this setup
        cycleWrapper();
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::isDualRollWrapper() const
{
    if (( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS3RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS3LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS4RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS4LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_RL ) || // SOF-2926
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_LR ))   // SOF-2926
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::isNGWWrapper() const
{
    return (DomEvolWrapperSystemInformation::nextGenModels <= wrTimingsData_->wrapperModelNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onStepDown()
{
    saveAnyChanges_ = true;

    QSpinBox *spinBox( incDecMap_[ qobject_cast<QWidget*>( sender() ) ] );

    spinBox->stepDown();
    onWidgetUpdatedInternal( spinBox );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::onStepUp()
{
    saveAnyChanges_ = true;

    QSpinBox *spinBox( incDecMap_[ qobject_cast<QWidget*>( sender() ) ] );

    spinBox->stepUp();
    onWidgetUpdatedInternal( spinBox );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperEditBox::printTimings()
{
    // Send message to generate the Summary Report
    QString title( "TRAY: " + ui->le_TrayName->text() );

    if ( "0" != ui->le_PLUNumber->text() )
        title += "  PLU EXCEPTION: " + ui->le_PLUNumber->text();

    QString msg;

    msg += "Length MIN:  ";
    msg += ui->le_MinLength->text();
    msg += "\n";

    msg += "Length MAX:  ";
    msg += ui->le_MaxLength->text();
    msg += "\n";

    msg += "\n";

    msg += "Width MIN:  ";
    msg += ui->le_MinWidth->text();
    msg += "\n";

    msg += "Width MAX:  ";
    msg += ui->le_MaxWidth->text();
    msg += "\n";

    msg += "\n";

    msg += "Lip MAX:  ";
    msg += ui->le_MaxLip->text();
    msg += "\n";

    msg += "\n";

    msg += "Height MIN:  ";
    msg += ui->le_MinHeight->text();
    msg += "\n";

    msg += "Height MAX:  ";
    msg += ui->le_MaxHeight->text();
    msg += "\n";

    msg += "\n";

    msg += "HEIGHT BOX ";
    msg += ui->cbo_HeightBox->currentText();
    msg += " TIMINGS:";
    msg += "\n";

    for ( int i( 0 ); i < ui->tblBasicTimingsList->rowCount(); i++ )
    {
        QSpinBox *spinBox;
        QComboBox *comboBox;

        if( ui->tblBasicTimingsList->item( i, 0 ) )
        {
            msg += ui->tblBasicTimingsList->item( i, 0 )->text();
            msg += ":  ";

            QWidget *w( ui->tblBasicTimingsList->cellWidget( i, isUsingSimplifiedTimings() ? 2 : 1 ) );

            if ( (spinBox = qobject_cast<QSpinBox*>( w )) )
                msg += QString::number( spinBox->value() );
            else if ( (comboBox = qobject_cast<QComboBox*>( w )) )
                msg += comboBox->currentText();

            msg += "\n";
        }
    }

    session_->send(ReportManagerDev,StartReport,"2,0,0,0,0,\"" + title + "\",\"" + msg + "\"" );
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

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperEditBox::eventFilter( QObject *obj, QEvent *e )
{
   // NGW-144
    if ( QEvent::FocusIn == e->type() )
    {
        QSpinBox *spinBox( qobject_cast<QSpinBox*>( obj ) );

        if ( spinBox )
        {
            QTimer *t( new QTimer( this ) );

            connect( t, SIGNAL(timeout()), spinBox, SLOT(selectAll()) );

            t->setSingleShot( true );
            t->start( 0 );
        }
    }

    return _Mybase::eventFilter( obj, e );
}
