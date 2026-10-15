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

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/servicemode.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QIntValidator>
#include <QTableWidget>
#include <palette.h>
#include <backgroundwidgetstyles.h>
#include <QTimer>


static const int DEFAULT_ROWS = 7;
static const int DEFAULT_COLS = 2;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want
static const int MAX_PARAMS = 21;                                     // there are 21 items, 0-20

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

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    setHighlight( ui->tblWrapBoxList );

//	wrTimingsData_ = &session_->scaleConfig->wrTimingsData_;

    initialize();
    translate();
}

serviceNewWrapperSetTimings::~serviceNewWrapperSetTimings()
{
    LOG_CLASS_INFO( "snwSetTimings", "serviceNewWrapperSetTimings" );

    LOG_TRACE( "~serviceNewWrapperSetTimings() START", NULL );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::~serviceNewWrapperSetTimings()";
#endif

    delete ui;

    LOG_TRACE( "~serviceNewWrapperSetTimings() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::initialize()
{
#ifdef USE_QDEBUG
	qDebug() << "serviceNewWrapperSetTimings::initialize():  Begin";
#endif
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
    connect(ui->tblWrapBoxList,     SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));       // click cell
	//	Ways to change or set up a new Wrap box (Tray).
    connect(ui->butEdit,    		SIGNAL(clicked()),              this,       SLOT(viewEditTray()));

    connect(ui->butDelete,    		SIGNAL(clicked()),              this,       SLOT(deleteTray()));
 
    connect(ui->butPLUException,    SIGNAL(clicked()),              this,       SLOT(pluExceptionTray()));

    connect(ui->butAutoConfig,      SIGNAL(clicked()),              this,       SLOT(autoConfigureTray()));

    connect(ui->butAutoDetect,    	SIGNAL(clicked()),              this,       SLOT(autoDetectTray()));

    connect(ui->butOneParameter,    SIGNAL(clicked()),              this,       SLOT(viewEditOneParamForAll()));

    connect(ui->butMoveUp,       	SIGNAL(clicked()),              this,       SLOT(moveUp()));
    connect(ui->butMoveDown,      	SIGNAL(clicked()),              this,       SLOT(moveDown()));
    connect(ui->butViewDefaults,    SIGNAL(clicked()),              this,       SLOT(toggleViewDefaultSettings()));
    connect(ui->butResetToDefault,  SIGNAL(clicked()),              this,       SLOT(resetToFactoryDefaults()));

    // SOF-1522: Display "Waiting" message on the screen until the data is received, processed & displayed
    //           Code snippet below stolen from buildTrayList()
    clearTheTrayList();
    int tmpNumRows = 2;

    ui->tblWrapBoxList->setRowCount(DEFAULT_ROWS);
    ui->tblWrapBoxList->setColumnCount(DEFAULT_COLS);

    ui->tblWrapBoxList->setColumnWidth(0,526);
    ui->tblWrapBoxList->setColumnWidth(1,182);

    for( int j = 0; j < tmpNumRows; j++ )
    {
        QTableWidgetItem *trayTypeItem = new QTableWidgetItem;
        trayTypeItem->setText( ( j == ( tmpNumRows - 1 ) ) ? tr( "Waiting..." ) : "" );
        trayTypeItem->setTextAlignment( Qt::AlignCenter );
        ui->tblWrapBoxList->setItem( j, 0, trayTypeItem );

        QTableWidgetItem *PLUExceptionItem = new QTableWidgetItem;
        PLUExceptionItem->setText( "" );
        ui->tblWrapBoxList->setItem( j, 1, PLUExceptionItem );
    }

    update();

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

#ifdef USE_QDEBUG
	qDebug() << "serviceNewWrapperSetTimings::initialize():  Done";
#endif
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
    ui->retranslateUi( this );
    currentOrDefaultSettings();

    update();
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
	qDebug() << "serviceNewWrapperSetTimings::clearTheTrayList()";
#endif
    ui->tblWrapBoxList->clear();                                       // clear it before we do anything
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//void serviceNewWrapperSetTimings::buildTrayList(const SessionMessage& msg)
void serviceNewWrapperSetTimings::buildTrayList()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::buildTrayList()";
#endif

    //  Either we received 9,237 or we have come back to this page without exiting Wrapper Timings.
    //  Display the list of Wrap boxes by the Tray name, including the PLU exception for each Wrapbox/Tray.
    if (wrTimingsData_->trayList == NULL)
    {
#ifdef USE_QDEBUG
        qDebug() << "serviceNewWrapperSetTimings::buildTrayList():  wrTimingsData_->trayList is NULL! Exiting...";
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

    ui->tblWrapBoxList->setColumnCount(DEFAULT_COLS);

    ui->tblWrapBoxList->setColumnWidth(0,526);                         //	Width of the first column (Tray type)
    ui->tblWrapBoxList->setColumnWidth(1,182);                         //	Width of the second column (PLU Exception)

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::buildTrayList():  " << domList->numOfTrays() << " trays total.";
#endif
    if (myItems > 0)
    {
        QVector<QString> trayTypeArray = TokenParser::splitStringTokens( domList->trayTypes() );
        QVector<QString> PLUExceptionArray = TokenParser::splitStringTokens( domList->pluExceptions() );
	    canChangeArray = domList->canChange().split(",");       // Parse the can change list

        for(int j = 0; j < myItems; j++ )
        {
            QTableWidgetItem *trayTypeItem = new QTableWidgetItem;    	// Create a column for the Tray type name.
            trayTypeItem->setText( trayTypeArray.at(j) );
            ui->tblWrapBoxList->setItem(j, 0, trayTypeItem);            // add it

            QTableWidgetItem *PLUExceptionItem = new QTableWidgetItem;  // Create a column for the PLU Exception.
            PLUExceptionItem->setText( PLUExceptionArray.at(j) );
            ui->tblWrapBoxList->setItem(j, 1, PLUExceptionItem);        // add it
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
	qDebug() << "serviceNewWrapperSetTimings::saveDataOnExit()";
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
	qDebug() << "serviceNewWrapperSetTimings::exitWrapperTimings()";
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
    qDebug() << "serviceNewWrapperSetTimings::clickRow(" << QString::number(row) << ")";
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
    qDebug() << "serviceNewWrapperSetTimings::setActiveRow(" << QString::number(row) << ")";
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
    qDebug() << "serviceNewWrapperSetTimings::getCurrentTrayName():  " << trayName;
#endif
    return trayName;
}

QString serviceNewWrapperSetTimings::getCurrentTrayPLUException()
{
    QTableWidgetItem *trayTypeItem = new QTableWidgetItem;
    trayTypeItem = ui->tblWrapBoxList->item(activeRow, 1);
    QString PLUException = trayTypeItem->text();
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::getCurrentTrayPLUException():  " << PLUException;
#endif
    return PLUException;
}

void serviceNewWrapperSetTimings::prepareTimingsPageSwitch()
{
    LOG_CLASS_INFO( "snwSetTimings", "prepareTimingsPageSwitch" );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::prepareTimingsPageSwitch()";
#endif

    wrTimingsData_->precedence = activeRow;
    wrTimingsData_->viewCurrentTimings = viewCurrentTimings_;
    wrTimingsData_->trayName = getCurrentTrayName();
    wrTimingsData_->pluException = getCurrentTrayPLUException();
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
    qDebug() << "serviceNewWrapperSetTimings::viewEditTray()";
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
    qDebug() << "serviceNewWrapperSetTimings::autoConfigureTray():  precedence = " << QString::number(activeRow);
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
    qDebug() << "serviceNewWrapperSetTimings::autoDetectTray():  precedence = " << QString::number(activeRow);
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
    qDebug() << "serviceNewWrapperSetTimings::viewEditOneParamForAll()";
#endif
	LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperOneHeightBoxParam>();
}

void serviceNewWrapperSetTimings::pluExceptionTray()
{
    LOG_CLASS_INFO( "snwSetTimings", "pluExceptionTray" );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::pluExceptionTray()";
#endif
    LOG_TRACE( "Calling prepareTimingsPageSwitch()", NULL );

    prepareTimingsPageSwitch();
    wrTimingsData_->pageMode = WrapperTimingsData::pmPLUException;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperSetTimings::deleteTray()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::deleteTray()";
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
    qDebug() << "serviceNewWrapperSetTimings::checkMoveAvailability()";
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
    qDebug() << "serviceNewWrapperSetTimings::takeRow(" + QString::number(row) + ")";
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
    qDebug() << "serviceNewWrapperSetTimings::setRow()";
#endif
    for (int col = 0; col < ui->tblWrapBoxList->columnCount(); ++col)
    {
        ui->tblWrapBoxList->setItem(row, col, rowItems.at(col));
    }
}

void serviceNewWrapperSetTimings::resetToFactoryDefaults()
{
    LOG_CLASS_INFO( "snwSetTimings", "resetToFactoryDefaults" );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::resetToFactoryDefaults()";
#endif
    activeRow = 0;
	LOG_TRACE( "Set activeRow = 0. Send 9719 message.", NULL );
    session_->send(Error, Wrapper, "9717,1");   //  HA-86
}

void serviceNewWrapperSetTimings::toggleViewDefaultSettings()
{
    LOG_CLASS_INFO( "snwSetTimings", "toggleViewDefaultSettings" );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::toggleViewDefaultSettings()";
#endif

	LOG_TRACE( "viewCurrentTimings_ = %u,  trayDeleted_ = %u", viewCurrentTimings_, trayDeleted_ );
	if (trayDeleted_)
	{
		//	Save the changes before we switch to View defaults.
		session_->send(Wrapper, WrEvolDonePackageTimings);      //  9,230
		sendDonePkgTimingsMsg_ = false;
		trayDeleted_ = false;
    }

    if (viewCurrentTimings_)
    {
        viewCurrentTimings_ = false;
        disableForViewDefault();
    }
    else
    {
        viewCurrentTimings_ = true;
        enableForViewCurrent();
    }
    wrTimingsData_->viewCurrentTimings = viewCurrentTimings_;
    currentOrDefaultSettings();
    activeRow = 0;
    session_->send(Wrapper, WrEvolLoadPackageTimings, (viewCurrentTimings_ ? "1" : "0"));   // 9,226, viewCurrentTimings_
}

void serviceNewWrapperSetTimings::setupWrapBoxParameters()
{
    LOG_CLASS_INFO( "snwSetTimings", "setupWrapBoxParameters" );
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::setupWrapBoxParameters()";
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

	//SOF-5094 determine max viewable params based on machine type
    int max_parameters;
    if( session_->scaleConfig->isWrapperTwoPrinter() )
        max_parameters = MAX_PARAMS;//21 - added vertical offsets
    else
        max_parameters = MAX_PARAMS - 2;

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
                    LOG_INFO( "SetTimings::setupWrapBoxParameters case PRIMARY_LABEL_VERT_OFFSET:", NULL );

                    if( evolWrapperSystemInformation->isNextGenModel() )
                    {
                        name = tr( "Primary label vertical offset" );

                        increaseName = tr( "Up" );
                        decreaseName = tr( "Down" );

                        fieldType = WrapParameter::typeSignedInt;
                    }
                }
                else
                    addParam = false;

                break;
            }

            case WrapParameter::SECONDARY_LABEL_VERT_OFFSET:
            {
                if( session_->scaleConfig->isWrapperTwoPrinter() )
                {
                    LOG_INFO( "SetTimings::setupWrapBoxParameters case SECONDARY_LABEL_VERT_OFFSET:", NULL );

                    if( evolWrapperSystemInformation->isNextGenModel() )
                    {
                        name = tr( "Secondary label vertical offset" );

                        increaseName = tr( "Up" );
                        decreaseName = tr( "Down" );

                        fieldType = WrapParameter::typeSignedInt;
                    }
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
            LOG_DEBUG( "SetTimings::setupWrapBoxParameters wrapParameterList name = %s and append(%d)", qPrintable(name), wrParam->getData() );
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
    qDebug() << "serviceNewWrapperSetTimings::onScreenChanging()";
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
    qDebug() << "serviceNewWrapperSetTimings::processReturnedWrapperErrors():  errCode = " << m.errorCode();
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
    qDebug() << "serviceNewWrapperSetTimings::continueResetToFactoryDefaults()";
#endif
    session_->send(Wrapper, WrEvolResetDefaultPackageTimings);              		// 9,229
}

void serviceNewWrapperSetTimings::continueRemoveTray()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::continueRemoveTray()";
#endif
    session_->send(Wrapper, WrEvolDeleteTrayType, QString::number(activeRow));   	// 9,238,tray
	trayDeleted_ = true;
}

void serviceNewWrapperSetTimings::currentOrDefaultSettings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperSetTimings::currentOrDefaultSettings()";
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
    ui->butPLUException->setEnabled(false);
    ui->butAutoConfig->setEnabled(false);
    ui->butAutoDetect->setEnabled(false);
    ui->butOneParameter->setEnabled(true);
    ui->butResetToDefault->setEnabled(true);
    ui->butViewDefaults->setEnabled(true);
    ui->butMoveDown->setEnabled(true);
}

void serviceNewWrapperSetTimings::enableForViewCurrent()
{
    ui->butEdit->setEnabled(true);
    ui->butDelete->setEnabled(true);
    ui->butPLUException->setEnabled(true);
    ui->butAutoConfig->setEnabled(true);
    ui->butAutoDetect->setEnabled(true);
    ui->butOneParameter->setEnabled(true);
    ui->butResetToDefault->setEnabled(true);
    ui->butViewDefaults->setEnabled(true);
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
        qDebug() << "serviceNewWrapperSetTimings::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
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
				qDebug() << "serviceNewWrapperSetTimings: Cannot change param name to " << ewsParam;
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
